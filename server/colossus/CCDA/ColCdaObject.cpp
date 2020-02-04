////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "ColCdaObject.h"
#include "msguserauth.h"
#include "getservconnip.h"
#include "ColCda.h"
#include "connUtils.h"
#include "AuthRedirector.h"
#include "admincommon.h"
#include "olapmsghandler.h"
#include "coldbm.h"
#include "wagerMeas.h"
#include "../utils.h"
#include "../ColTournRetryData.h"
#include "../ColossusStructures.h"
#include "conndefines.h"
#include "templateserver.h"

////////////////////////////////////////////////////////////////////////////////

/*static*/ CommServerNullGuardFactory ColCdaServerObject::guardFactory;
/*static*/ CommClientNullGuardFactory ColCdaServerObject::guardFactoryClient;
/*static*/ CommServerPlainTextPasswordSidGuardFactory ColCdaServerObject::guardPwdFactory;

////////////////////////////////////////////////////////////////////////////////

using namespace ColCDA;

class CCDARatesCall : public CommClientGConnection::AsyncCall
{
    ColCdaServerObject& obj;
public:
    CCDARatesCall(ColCdaServerObject& obj_)
		: obj(obj_)
    {
    }
    void processReply(UINT32 /*reqId*/, UINT32 msgId, const CommMsgBody& body) override
    {
        obj.processRates(body);
    }
};

class TournRetryCall : public CommClientGConnection::AsyncCall
{
    ColCdaServerObject& obj;
	TournRetryData tournRetryData;
public:
    TournRetryCall(ColCdaServerObject& obj_, const TournRetryData& data_)
		: obj(obj_), tournRetryData(data_)
    {
    }
    void processReply(UINT32 /*reqId*/, UINT32 msgId, const CommMsgBody& body) override
    {
        obj.processReceiveTournData(body, tournRetryData);
    }
};

class EmailNotificationRequestCall : public CommClientGConnection::AsyncCall
{
	ColCdaServerObject* obj;

public:
	EmailNotificationRequestCall(ColCdaServerObject* obj_)
		: obj(obj_)
	{}

	void processReply(UINT32 /*reqId*/, UINT32 msgId, const CommMsgBody& body) override
	{
		CommMsgParser parser(body);
		switch (msgId)
		{
		case DBM_A_GET_INIDATA2:
			obj->processGotIniDataReply(parser);
			break;
		case TS_A_COMMON_SEND_EMAIL:
			obj->processGotEmailSentReply(parser);
			break;
		default:
			PLog("EmailNotificationRequestCall: Unknown msgId=%u", msgId);
			break;
		}
	}
};

////////////////////////////////////////////////////////////////////////////////
void ColCdaServerObject::setReply(UINT32 reqId, INT16 errCode, char const* errDesc, CommMsgBody& reply)
{
	reply.composeUINT32(reqId).composeINT16(errCode);
	if (errCode)
	{
		reply.composeString(errDesc);
	}
}

void ColCdaServerObject::postReply(CommServerConnection* conn, UINT32 msgId, CommMsgBody& body)
{
	if (conn)
	{
		conn->post(msgId, body);
	}
	else
	{
		PLog("Message #%u not sent (no connection)", msgId);
	}
}

////////////////////////////////////////////////////////////////////////////////

void TrustedAdminConnection::processMessage(UINT32 msgId, const CommMsgBody& body)
{
	CommServerConnection::processMessage(msgId, body);

	obj->processCommonServerMessage(eColCdaConnTypeTrustedAdmin, msgId, body, this);
}

void TrustedAdminConnection::connected()
{
	PLog("TrustedAdminConnection::connected");

	CommServerConnection::connected();
}

void TrustedAdminConnection::closedOrDisconnected(int errCode, const char* errMsg)
{
	PLog("TrustedAdminConnection::closedOrDisconnected(errCode: %d, errMsg: %s)", errCode, errMsg);

	CommServerConnection::closedOrDisconnected(errCode, errMsg);
}

////////////////////////////////////////////////////////////////////////////////

void AdminConnection::processMessage(UINT32 msgId, const CommMsgBody& body)
{
	CommServerConnection::processMessage(msgId, body);

	obj->processCommonServerMessage(eColCdaConnTypeAdmin, msgId, body, this);
}

void AdminConnection::connected()
{
	CommServerConnection::connected();

	PLog("Admin '%s'", userId.c_str());

	CommMsgBody request;
	composeLoginConfirmMsg(userId, request);
	obj->postToLoginAuth(DBM_Q_LOGIN_CONFIRM, request, NULL);
}

void AdminConnection::closedOrDisconnected(int errCode, const char* errMsg)
{
	CommServerConnection::closedOrDisconnected(errCode, errMsg);

	PLog("Admin '%s' disconnected: %d '%s'", userId.c_str(), errCode, errMsg);

	if (userId.length() == 0 || authError)
	{
		return;
	}

	// Translate specific codes
	if (errCode == _COMM_ERROR_PHYSICAL_DISCONNECTED)
	{
		errCode = LOGOUT_DISCONNECTED;
	}

	CommMsgBody msg;
	composeUserLogout(_OBJ_TYPE_COL_CDA, 0, userId, errCode, msg);
	obj->postToLoginAuth(DBM_Q_USER_LOGOUT, msg, NULL);
}

void AdminConnection::getUserAuth(const char* guardType, const char* user_, CommMsgBody& clientExtra, CommMsgBody& authBody)
{
	if(!strcmp(guardType, _COMM_NULL_GUARD_NAME)) //PYR-44258
	{
		gotUserAuth(0);
		return;
	}

	userId.assign(user_);

	PString ipAddr;
	int port;
	getServConnIp(*this, ipAddr, port);

	CommMsgBody request;
	composeUserAuthMsg(user_, "", "", "", ipAddr, port, _OBJ_TYPE_COL_CDA, 0, authBody, 0, 0, guardType, request);

	obj->requestUserAuth(request, this);
}

void AdminConnection::receiveAdminAuth(CommMsgParser& parser)
{
	authError = 0;
	parser.parseINT16(authError);
	if (authError)
	{
		char const* errBuf;
		parser.parseString(errBuf);
		gotUserAuth(0, authError, errBuf);

		PLog("receiveAdminAuth error %d (%s)", authError, errBuf);
		return;
	}

	// Some information in the authentication message is not needed
	UINT32 dummyUint32;
	char const* pDummyStr;
	CommMsgBody dummyBody;

	// Parse the authentication message
	UINT16 numRights = 0;
	CommMsgBody authBody;
	parser.
		parseMsgBody(authBody).
		parseUINT32(dummyUint32).		// Privileges
		parseString(pDummyStr).			// User city
		parseUINT32(dummyUint32).		// Flags
		parseString(pDummyStr).			// User email
		parseUINT32(userIntId).			// UserIntId
		parseString(pDummyStr).			// User country
		parseString(pDummyStr).			// User state
		parseMsgBody(dummyBody).		// Obsolete
		parseUINT16(numRights);

	// Extract and store rights
	for (UINT32 i = 0; i < numRights; ++i)
	{
		char const* right;
		parser.parseString(right);
		adminRights.insert(right);
	}

	// Skip the rest of the message as it is not needed.
	CommMsgParser authParser(authBody);
	if (!gotUserAuth(&authParser))
	{
		PLog("Authentication failed");
	}
}

/* static */ const char* AdminConnection::tryGetAdminId(CommServerConnection* conn)
{
	AdminConnection* adminConn = dynamic_cast<AdminConnection*>(conn);
	if (adminConn)
	{
		return adminConn->getUser();
	}
	else
	{
		return nullptr;
	}
}

////////////////////////////////////////////////////////////////////////////////

void OlapMDServerConnection::processMessage(UINT32 msgId, const CommMsgBody& body)
{
	CommServerConnection::processMessage(msgId, body);

	obj->processCommonServerMessage(eColCdaConnTypeOlapMD, msgId, body, this);
}

void OlapMDServerConnection::connected()
{
	PLog("OlapMDServerConnection::connected");

	CommServerConnection::connected();
}

void OlapMDServerConnection::closedOrDisconnected(int errCode, const char* errMsg)
{
	PLog("OlapMDServerConnection::closedOrDisconnected(errCode: %d, errMsg: %s)", errCode, errMsg);

	CommServerConnection::closedOrDisconnected(errCode, errMsg);
}

////////////////////////////////////////////////////////////////////////////////

void ColossusSrvConn::processMessage( UINT32 msgId, const CommMsgBody& body )
{
	CommServerConnection::processMessage(msgId, body);

	obj->processCommonServerMessage(eColCdaConnTypeColossus, msgId, body, this);
}

void ColossusSrvConn::connected()
{
	PLog("ColossusSrvConn::connected");

	CommServerConnection::connected();
}

void ColossusSrvConn::closedOrDisconnected( int errCode, const char* errMsg )
{
	PLog("ColossusSrvConn::closedOrDisconnected(errCode: %d, errMsg: %s)", errCode, errMsg);

	CommServerConnection::closedOrDisconnected(errCode, errMsg);
}

////////////////////////////////////////////////////////////////////////////////

void ColUtilSrvConn::processMessage( UINT32 msgId, const CommMsgBody& body )
{
	CommServerConnection::processMessage(msgId, body);

	obj->processCommonServerMessage(eColCdaConnTypeColUtil, msgId, body, this);
}

void ColUtilSrvConn::connected()
{
	PLog("ColUtilSrvConn::connected");

	CommServerConnection::connected();
}

void ColUtilSrvConn::closedOrDisconnected( int errCode, const char* errMsg )
{
	PLog("ColUtilSrvConn::closedOrDisconnected(errCode: %d, errMsg: %s)", errCode, errMsg);

	CommServerConnection::closedOrDisconnected(errCode, errMsg);
}

////////////////////////////////////////////////////////////////////////////////

void ColCdaClientConn::connected()
{
	CommClientGConnection::connected();

	PLog("ColCdaClientConn::connected(%c)", name);
	isconnected = 1;
}

bool ColCdaClientConn::closedOrDisconnected(int errCode, const char* errMsg)
{
	isconnected = 0;
	PLog("ColCdaClientConn::closed(%c): %d '%s'", name, errCode, errMsg);

	return CommClientGConnection::closedOrDisconnected(errCode, errMsg);
}

UINT32 ColCdaClientConn::post(UINT32 msgId, CommMsgBody& body, AsyncCall* call)
{
	UINT32 reqId = CommClientGConnection::post(msgId, body, call);
	PLog(">>%c %u [%08X]%s", name, msgId, reqId, call ? "" : " (no AsyncCall)");
	return reqId;
}

void ColCdaClientConn::processGMessage(UINT32 requestId, UINT32 msgId, const CommMsgBody& body, AsyncCall* call)
{
	CommClientGConnection::processGMessage(requestId, msgId, body, call);

	PTRACE("<<%c %u [%08X]%s", name, msgId, requestId, call ? "" : " (no AsyncCall)");

	if(call)
	{
		obj->processCallback(requestId, msgId, body, call);
		return;
	}

	obj->processClientGMessage(requestId, msgId, body);
}


////////////////////////////////////////////////////////////////////////////////
ColCdaServerObject::ColCdaServerObject(_CommInterface& inter)
	: CommServerObject(inter)
	, srvPool(inter)
	, cliPool(inter)
	, adminFactory(guardPwdFactory)
	, adminTrustedFactory(guardFactory)
	, olapMDConnFactory(guardFactory)
	, colossusSrvConnFactory(guardFactory)
	, colUtilSrvConnFactory(guardFactory)
	, loginAuthConn(guardFactoryClient, 'L')
	, colReaderConn(guardFactoryClient, 'R')
	, colDbmWriterConn(guardFactoryClient, 'W')
	, colDbmConn(guardFactoryClient, 'C')
	, colRawDbmConn(guardFactoryClient, 'A')
	, wagerMeasConn(guardFactoryClient, 'M')
	, collusionConn(guardFactoryClient, 'U')
	, olapConn(guardFactoryClient, 'O')
	, templateSrvConn(guardFactoryClient, 'T')
	, sweeperTimerHandle(0)
	, heLoggerTimerHandle(0)
	, heLogger(NULL)
	, serverEnabled(false), handleRing(false), handleTourn(false), moreTrace(false)
	, handTimeTooOldSecs(DEFAULT_HAND_TOO_OLD_THRESH_SECS)
	, maxNumHandsAllowed(DEFAULT_MAX_NUM_HANDS_ALLOWED)
	, maxNumCDHUserAllowed(DEFAULT_MAX_NUM_CDHUSER_ALLOWED)
	, maxNumTournAllowed(DEFAULT_MAX_NUM_TOURN_ALLOWED)
	, firstTournRetryDelay(DEFAULT_FIRST_TOURN_RETRY_DELAY)
	, tournRetryDelay(DEFAULT_TOURN_RETRY_DELAY)
	, maxTournRetries(DEFAULT_MAX_TOURN_RETRIES)
	, enableAlertAutoAction(false)
	, timerConverterHandle(0)
	, evaluationStatus(CCDA_EVAL_STATUS_RUNNING)
	, maxNumUsersToEval(DEFAULT_MAX_NUM_CACHE_USERS_EVAL)
{
	adminFactory.obj = this;
	adminTrustedFactory.obj = this;
	olapMDConnFactory.obj = this;
	colossusSrvConnFactory.obj = this;
	colUtilSrvConnFactory.obj = this;

	loginAuthConn.obj = this;
	colReaderConn.obj = this;
	colDbmConn.obj = this;
	colRawDbmConn.obj = this;
	colDbmWriterConn.obj = this;
	wagerMeasConn.obj = this;
	collusionConn.obj = this;
	olapConn.obj = this;
	templateSrvConn.obj = this;
}

ColCdaServerObject::~ColCdaServerObject()
{
	try
	{
	}
	catch (...)
	{

	}
}

void ColCdaServerObject::init(CommMsgBody& initMsg)
{
	CommMsgParser parser(initMsg);
	char const* initDir_;
	parser.parseString(initDir_);

	if (!parser.parseEnded())
	{
		parser.parseStringP(loggerStationName);
		PLog("DBM logger station name [%s] was provided at startup and will override correspondent logger's INI property", loggerStationName.c_str());
	}

	initDir.assign(initDir_);
	fullFileName.assign(initDir_);
	fullFileName.append("colcda.ini");

	dynamicInit();

	if (sweeperTimerHandle)
    {
        bool found = deleteTimerMessage( sweeperTimerHandle );
        if (!found)
            PLog("ColCdaServerObject::init: cannot find/stop sweeper timer #%u", sweeperTimerHandle);
        sweeperTimerHandle = 0;
    }

	CommMsgBody timerBody;
	sweeperTimerHandle = postTimerMessage(TIMER_SWEEPER, timerBody, TIMER_SWEEPER_MSEC);

	srvPool.registerFactory(COLCDA_CONN_TYPE_ADMIN, adminFactory);
	srvPool.registerFactory(COLCDA_CONN_TYPE_TRUSTEDADMIN, adminTrustedFactory);
	srvPool.registerFactory(COLCDA_CONN_TYPE_OLAPMD, olapMDConnFactory);
	srvPool.registerFactory(COLCDA_CONN_TYPE_INTERNAL, colossusSrvConnFactory);
	srvPool.registerFactory(COLCDA_CONN_TYPE_COLUTIL, colUtilSrvConnFactory);
}

void ColCdaServerObject::dynamicInit()
{
	PLog("dynamicInit");

	PIniFile ini(fullFileName);
	const PIniFile::Section* section = nullptr;

	section = ini.getSection("DYNAMICSETTINGS");
	if (section)
	{
		serverEnabled = (section->getIntProperty("serverEnabled", 0) != 0);
		handleRing = (section->getIntProperty("handleRing", 0) != 0);
		handleTourn = (section->getIntProperty("handleTourn", 0) != 0);
		moreTrace = (section->getIntProperty("moreTrace", 0) != 0);
		handTimeTooOldSecs = section->getIntProperty("handTimeTooOldSecs", DEFAULT_HAND_TOO_OLD_THRESH_SECS);
		maxNumHandsAllowed = section->getIntProperty("maxNumHandsAllowed", DEFAULT_MAX_NUM_HANDS_ALLOWED);
		maxNumCDHUserAllowed = section->getIntProperty("maxNumCDHUserAllowed", DEFAULT_MAX_NUM_CDHUSER_ALLOWED);
		maxNumTournAllowed = section->getIntProperty("maxNumTournAllowed", DEFAULT_MAX_NUM_TOURN_ALLOWED);
		firstTournRetryDelay = section->getIntProperty("firstTournRetryDelay", DEFAULT_FIRST_TOURN_RETRY_DELAY);
		tournRetryDelay = section->getIntProperty("tournRetryDelay", DEFAULT_TOURN_RETRY_DELAY);
		maxTournRetries = section->getIntProperty("maxTournRetries", DEFAULT_MAX_TOURN_RETRIES);
		enableAlertAutoAction = (section->getIntProperty("enableAlertAutoAction", 0) != 0);
		maxNumUsersToEval = section->getIntProperty("maxNumUsersToEval", DEFAULT_MAX_NUM_CACHE_USERS_EVAL);
	}
	else
	{
		PTRACE("No DYNAMICSETTINGS configured");
	}

	section = ini.getSection("LOGGER");
	if (section)
	{
		dynamicInit_Logger(*section);
	}
	else
	{
		PTRACE("No LOGGER configured");
	}

	section = ini.getSection("DYNAMICCONNECT");
	if (section)
	{
		dynamicInit_DynamicConnect(*section);
	}
	else
	{
		PTRACE("No DYNAMICCONNECT configured");
	}

	PLog("serverEnabled = %s", serverEnabled ? "YES" : "NO");
	PLog("handleRing = %s", handleRing ? "YES" : "NO");
	PLog("handleTourn = %s", handleTourn ? "YES" : "NO");
	PLog("moreTrace = %s", moreTrace ? "YES" : "NO");
	PLog("handTimeTooOldSecs = %u", handTimeTooOldSecs);
	PLog("maxNumHandsAllowed = %u", maxNumHandsAllowed);
	PLog("maxNumCDHUserAllowed = %u", maxNumCDHUserAllowed);
	PLog("maxNumTournAllowed = %u", maxNumTournAllowed);
	PLog("firstTournRetryDelay = %u", firstTournRetryDelay);
	PLog("tournRetryDelay = %u", tournRetryDelay);
	PLog("maxTournRetries = %u", maxTournRetries);
	PLog("loggerStationName = %s", loggerStationName.c_str());
	PLog("enableAlertAutoAction = %s", enableAlertAutoAction ? "YES" : "NO");
	PLog("maxNumUsersToEval = %u", maxNumUsersToEval);
}

void ColCdaServerObject::dynamicInit_Logger(const PIniFile::Section& section)
{
	stopHeLogger();

	bool useLogger = (0 != section.getIntProperty("useLogger", 0));
	PString loggerServerAddress = section.getProperty("LoggerAddress", "");

	if (!loggerStationName.length())
	{
		loggerStationName = section.getProperty("LogStationName", "");
	}

	if (useLogger && !loggerServerAddress.equals("") && !loggerStationName.equals(""))
	{
		startHeLogger(loggerServerAddress, loggerStationName);
	}
	else
	{
		PTRACE("No LOGGER started");
	}
}

void ColCdaServerObject::dynamicInit_DynamicConnect(const PIniFile::Section& section)
{
	{
		const char* server = section.getProperty("LoginAuthDbmServer");
		const char* serverObject = section.getProperty("LoginAuthDbmObject", "DbmLoginInstance");
		const char* connectionType = section.getProperty("LoginAuthDbmConnectionType", "colcda");
		reconnect(loginAuthConn, server, serverObject, connectionType);
	}

	{
		const char* server = section.getProperty("ColossusReadServer");
		const char* serverObject = section.getProperty("ColossusReadObject", "ColossusInstance");
		const char* connectionType = section.getProperty("ColossusReadConnectionType", COLOSSUS_READ_CONNECTION_TYPE);
		reconnect(colReaderConn, server, serverObject, connectionType);
	}

	{
		const char* server = section.getProperty("ColDbmServer");
		const char* serverObject = section.getProperty("ColDbmServerObject", "ColDbmInstance");
		const char* connectionType = section.getProperty("ColDbmServerConnectionType", COLDBM_CCDA_CONNECTION_TYPE);
		reconnect(colDbmConn, server, serverObject, connectionType);
	}

	{
		const char* server = section.getProperty("ColRawDbmServer");
		const char* serverObject = section.getProperty("ColRawDbmServerObject", "ColDbmInstance");
		const char* connectionType = section.getProperty("ColRawDbmServerConnectionType", COLDBM_CCDA_CONNECTION_TYPE);
		if (server && *server) // TODO: remove it
		{
			reconnect(colRawDbmConn, server, serverObject, connectionType);
		}
	}

	{
		const char* server = section.getProperty("ColDbmWriterServer");
		const char* serverObject = section.getProperty("ColDbmWriterServerObject", "ColUtilDbmWriteInstance");
		const char* connectionType = section.getProperty("ColDbmWriterServerConnectionType", COLDBM_CCDA_WRITE_CONNECTION_TYPE);
		reconnect(colDbmWriterConn, server, serverObject, connectionType);
	}

	{
		const char* server = section.getProperty("WagerMeasServer");
		const char* serverObject = section.getProperty("WagerMeasServerObject", "WagerMeasInstance");
		const char* connectionType = section.getProperty("WagerMeasServerConnectionType", WMSRV_CONN_TYPE_CCDA);
		reconnect(wagerMeasConn, server, serverObject, connectionType);
	}

	{
		const char* server = section.getProperty("CollusionServer");
		const char* serverObject = section.getProperty("CollusionServerObject", "CollusionServerInstance");
		const char* connectionType = section.getProperty("CollusionServerConnectionType", COLCDA_CLIENT_CONN_COLLUSION);
		if (server && *server)  // TODO: remove it
		{
			reconnect(collusionConn, server, serverObject, connectionType);
		}
	}

	{
		const char* server = section.getProperty("OlapServer");
		const char* serverObject = section.getProperty("OlapObject", "OlapDbmServerInstance");
		const char* connectionType = section.getProperty("OlapConnectionType", COLOSSUS_CCDA_CONN_TYPE_TO_OLAP);
		if (server && *server)  // TODO: remove it
		{
			reconnect(olapConn, server, serverObject, connectionType);
		}
	}

	{
		const char* server = section.getProperty("TemplateServer");
		const char* serverObject = section.getProperty("TemplateServerObject", "TemplateServerInstance");
		const char* connectionType = section.getProperty("TemplateConnectionType", "collusion");
		if (server && *server) // TODO: remove this condition once QA config is complete 
		{
			reconnect(templateSrvConn, server, serverObject, connectionType);
		}
	}

	// load CCDA rules from DBM
	if (ringGameRules.size() == 0 && sngHUGameRules.size() == 0)
	{
		CommMsgBody body;
		body.composeUINT32(CCDA_RuleType_RingGame);
		postToColDbm(COL_Q_CCDA_READ_CURRENT_RULE_LIST, body, new ColUtilSrvCall(this, 0, 0));

		CommMsgBody body2;
		body2.composeUINT32(CCDA_RuleType_SNGHU);
		postToColDbm(COL_Q_CCDA_READ_CURRENT_RULE_LIST, body2, new ColUtilSrvCall(this, 0, 0));
	}
	
	// load velocity control rule
	CommMsgBody velocityControlbody;
	postToColDbm(COL_Q_CCDA_GET_VELOCITY_CONTROL_RULE, velocityControlbody, new ColUtilSrvCall(this, 0, 0));

	UINT32 cdaWhitelisType = ColWhitelistType_User | ColWhitelistType_PLAYERPAIR;
	CommMsgBody whitelistBody;
	whitelistBody
		.composeUINT32(ColWhitelistModule_CCDA)	// load CCDA module only
		.composeUINT32(cdaWhitelisType)	
		.composeUINT32(ColWhitelistState_Active)
		;
	postToColDbm(COL_Q_COLWHITELIST_GET_CURR_LIST, whitelistBody, new ColUtilSrvCall(this, 0, 0));

	if (timerConverterHandle)
	{
		bool found = deleteTimerMessage(timerConverterHandle);
		if (!found)
			PLog("ColCdaServerObject::dynamicInit: ERROR: cannot find/stop timer #%u", timerConverterHandle);
		timerConverterHandle = 0;
	}

	CommMsgBody timerBody;
	timerConverterHandle = postTimerMessage(TIMER_CONVERTER, timerBody, TIMER_CONVERTER_MSEC);

	// load currency rates
	CommMsgBody rateBody;
	postToReader(COLOSSUS_GET_CONVRATES_TO_USD, rateBody, new CCDARatesCall(*this));
}

void ColCdaServerObject::shutdown()
{
	if (heLoggerTimerHandle)
	{
		deleteTimerMessage(heLoggerTimerHandle);
		heLoggerTimerHandle = 0;
	}
}

void ColCdaServerObject::processCallback(UINT32 reqId, UINT32 msgId, const CommMsgBody& body, CommClientGConnection::AsyncCall* pCall)
{
	HiresCounter counter;

	PLog("{ASC %u", msgId);

	pCall->processReply(reqId, msgId, body);

	counter.stop();
	UINT32 ticks = counter.getElapsedTick();
	ticksAccumulator.addMessageTicks(counter);

	PLog("}%u%s", ticks, (ticks > TICKS_PER_SECOND) ? " overspan" : "");
}

void ColCdaServerObject::addMessageTicks(HiresCounter& counter)
{
	ticksAccumulator.addMessageTicks(counter);
}

void ColCdaServerObject::processMessage(_CommMsg& msg)
{
	cliPool._processMessage(msg) || srvPool._processMessage(msg);
}

void ColCdaServerObject::logServerMsg(const char* prefix, UINT32 msgId, CommServerConnection* conn, const char* userId)
{
	PLog("{%s %04u %08X %s", prefix, msgId, conn ? conn->id() : 0, userId);
}

void ColCdaServerObject::replyMsg(UINT32 connId, UINT32 msgId, CommMsgBody& body)
{
	if (!connId)
	{
		PLog("Warning - Message #%u failed to send, no connId.", msgId);
		return;
	}

	CommServerConnection* conn = srvPool.findConnection(connId);
	if(!conn)
	{
		PLog("Warning - Message #%u failed to send, lost connection.", msgId);
		return;
	}

	conn->post(msgId, body);
}

void ColCdaServerObject::processCommonServerMessage(COLCDA_CONN_TYPE connType, UINT32 msgId, const CommMsgBody& body, CommServerConnection* conn)
{
	HiresCounter counter;

	switch(connType)
	{
	case eColCdaConnTypeTrustedAdmin:
		{
			processTrustedAdminMessage(msgId, body, conn);
			break;
		}
	case eColCdaConnTypeAdmin:
		{
			processAdminMessage(msgId, body, conn);
			break;
		}
	case eColCdaConnTypeOlapMD:
		{
			processOlapMDMessage(msgId, body, conn);
			break;
		}
	case eColCdaConnTypeColossus:
		{
			processColossusMessage(msgId, body, conn);
			break;
		}
	case eColCdaConnTypeColUtil:
		{
			processColUtilMessage(msgId, body, conn);
			break;
		}
	default:
		{
			throw PError("Unsupported connType value.");
		}
	}

	counter.stop();
	UINT32 ticks = counter.getElapsedTick();
	PLog("}%u%s", ticks, (ticks > TICKS_PER_SECOND) ? " overspan" : "");

	ticksAccumulator.addMessageTicks(counter);
}

void ColCdaServerObject::processClientGMessage(UINT32 /*requestId*/, UINT32 msgId, const CommMsgBody& /*body*/)
{
	switch(msgId)
	{
	case DBM_A_LOGIN_CONFIRM:
	case DBM_A_USER_LOGOUT:
		{
			break;
		}
	default:
		{
			PLog("ColCdaServerObject::processClientGMessage: Unknown message=%u", msgId);
			break;
		}
	}
}

void ColCdaServerObject::processTimerMessage(UINT32 handle, UINT32 msgId, CommMsgBody& body)
{
	HiresCounter counter;
	PLog("{T %u %08X", msgId, 0 /* connId */);

	CommServerObject::processTimerMessage(handle, msgId, body);

	CommMsgParser parser(body);

	switch (msgId)
	{
	case TIMER_LOGGER:
		{
			CommMsgBody timerBody;
			heLoggerTimerHandle = postTimerMessage(TIMER_LOGGER, timerBody, TIMER_LOGGER_MSEC);

			ticksAccumulator.stop();
			reportPerformance();
			ticksAccumulator.reset();

			break;
		}
	case TIMER_CONVERTER:
		{
			CommMsgBody timerBody;
			timerConverterHandle = postTimerMessage(TIMER_CONVERTER, timerBody, TIMER_CONVERTER_MSEC);

			CommMsgBody emptyBody;
			postToReader(COLOSSUS_GET_CONVRATES_TO_USD, emptyBody, new CCDARatesCall(*this));
			break;
		}
	case TIMER_TOURN_DELAY:
		{
			processGetTournInfo(parser);
			break;
		}
	case TIMER_SWEEPER:
		{
			CommMsgBody timerBody;
			sweeperTimerHandle = postTimerMessage(TIMER_SWEEPER, timerBody, TIMER_SWEEPER_MSEC);
			sweepCaches();
			break;
		}
	case TIMER_EVAL_CACHE_USERS:
		{
			PLog("EVAL_CACHE_USERS");

			if (!shouldRunEvaluation())
			{
				break;
			}

			UINT32 evaluatedUserNum = 0;

			auto it = userEvaluationCache.container.begin();
			while (it != userEvaluationCache.container.end())
			{
				auto removeIt = it++;
				if (removeIt->evalType == CCDA_RuleType_RingGame)
				{
					triggerRingEval(removeIt->userId, removeIt->evalTime);
				}
				else if (removeIt->evalType == CCDA_RuleType_SNGHU)
				{
					triggerTournEval(removeIt->userId, removeIt->evalTime);
				}
				else
				{
					PLog(LOG_TRACE_FAULTY_LOGIC "Unknown eval type = %u", removeIt->evalType);
				}
				++evaluatedUserNum;
				userEvaluationCache.container.erase(removeIt);

				if (evaluatedUserNum >= maxNumUsersToEval)
				{
					break;

				}
			}
			PLog("Evaluated %u num users.", evaluatedUserNum);

			if (userEvaluationCache.size() > 0)
			{
				CommMsgBody body;
				postTimerMessage(TIMER_EVAL_CACHE_USERS, body, TIMER_EVAL_CACHE_USERS_MSEC);
			}
			break;
		}
	default:
		{
			PLog("Unknown timer msg %d", msgId);
			break;
		}
	}

	counter.stop();
	UINT32 ticks = counter.getElapsedTick();
	ticksAccumulator.addMessageTicks(counter);
	PLog("}%u%s", ticks, (ticks > TICKS_PER_SECOND) ? " overspan" : "");
}

void ColCdaServerObject::processRates(const CommMsgBody& body)
{
	CommMsgParser parser(body);

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	if (errCode)
	{
		const char* errStr;
		parser.parseString(errStr);
		PLog(LOG_TRACE_FAULTY_LOGIC " DBM error[%d]-%s", errCode, errStr);
		return;
	}

	parseMidConvRatesVector(currencies, parser);
	PLog("processRates (%d)", (int)currencies.size());
}

void ColCdaServerObject::requestUserAuth(CommMsgBody& request, AdminConnection* conn)
{
	postToLoginAuth(DBM_Q_USER_AUTH, request, new ColCdaAuthAsyncCall(this, conn->id()));
}

void ColCdaServerObject::receiveAuthFromDbm(CommMsgParser& parser, UINT32 connId)
{
	PLog("<<ADMIN connId=%08X", connId);

	AdminConnection* adminConn = dynamic_cast<AdminConnection*>(srvPool.findConnection(connId));
	if (!adminConn)
	{
		PLog("Warning: admin dropped connection");
		return;
	}

	adminConn->receiveAdminAuth(parser);
}

void ColCdaServerObject::reportPerformance()
{
	PLog("reportPerformance");

	try
	{
		PLog("- avgProcessingMksec: %u", ticksAccumulator.avgProcessingMkSec());
		PLog("- messagesPerSec: %u", ticksAccumulator.messagesPerSec());
		PLog("- loadPercentCntTick: %u", ticksAccumulator.loadPercentCntTick());
		PLog("- avgProcessingMkSecTick: %u", ticksAccumulator.avgProcessingMkSecTick());

		int loginAuthConnSz = loginAuthConn._size();
		PLog("- loginAuthConn._size(): %d", loginAuthConnSz);

		int readerConnSz = colReaderConn._size();
		PLog("- colReaderConn._size(): %d", readerConnSz);

		int colDbmWriterConnSz = colDbmWriterConn._size();
		PLog("- colDbmWriterConn._size(): %d", colDbmWriterConnSz);

		int colDbmConnSz = colDbmConn._size();
		PLog("- colDbmConn._size(): %d", colDbmConnSz);

		int colRawDbmConnSz = colRawDbmConn._size();
		PLog("- colRawDbmConn._size(): %d", colRawDbmConnSz);

		int wagerMeasConnSz = wagerMeasConn._size();
		PLog("- wagerMeasConn._size(): %d", wagerMeasConnSz);

		int collusionConnSz = collusionConn._size();
		PLog("- collusionConn._size(): %d", collusionConnSz);

		if (heLogger)
		{
			CommMsgBody reportBody;
			PString strValue;

			strValue.assign("").appendUint(ticksAccumulator.avgProcessingMkSec());
			heLogger->compose(reportBody, "All:avgProcessingMksec", strValue);

			strValue.assign("").appendUint(ticksAccumulator.messagesPerSec());
			heLogger->compose(reportBody, "All:messagesPerSec", strValue);

			strValue.assign("").appendUint(ticksAccumulator.loadPercentCntTick());
			heLogger->compose(reportBody, "All:loadPercentCounter", strValue);

			strValue.assign("").appendUint(ticksAccumulator.avgProcessingMkSecTick());
			heLogger->compose(reportBody, "All:avgProcessingMksecTick", strValue);

			strValue.assign("").appendInt(loginAuthConn.isConnected());
			heLogger->compose(reportBody, "connection.loginAuthConn", strValue);

			strValue.assign("").appendInt(colReaderConn.isConnected());
			heLogger->compose(reportBody, "connection.colReaderConn", strValue);

			strValue.assign("").appendInt(colDbmConn.isConnected());
			heLogger->compose(reportBody, "connection.colDbmConn", strValue);

			strValue.assign("").appendInt(colRawDbmConn.isConnected());
			heLogger->compose(reportBody, "connection.colRawDbmConn", strValue);

			strValue.assign("").appendInt(colDbmWriterConn.isConnected());
			heLogger->compose(reportBody, "connection.colDbmWriterConn", strValue);

			strValue.assign("").appendInt(wagerMeasConn.isConnected());
			heLogger->compose(reportBody, "connection.wagerMeasConn", strValue);

			strValue.assign("").appendInt(collusionConn.isConnected());
			heLogger->compose(reportBody, "connection.collusionConn", strValue);

			strValue.assign("").appendInt(olapConn.isConnected());
			heLogger->compose(reportBody, "connection.olapConn", strValue);

			strValue.assign("").appendInt(templateSrvConn.isConnected());
			heLogger->compose(reportBody, "connection.templateSrvConn", strValue);

			strValue.assign("").appendUint(ringHandCache.size());
			heLogger->compose(reportBody, "ringHandCacheSz", strValue);

			strValue.assign("").appendUint(cdhHandCache.size());
			heLogger->compose(reportBody, "cdhHandCacheSz", strValue);

			strValue.assign("").appendUint(newTournCache.size());
			heLogger->compose(reportBody, "newTournCacheSz", strValue);

			strValue.assign("").appendUint(tournDataCache.size());
			heLogger->compose(reportBody, "tournDataCacheSz", strValue);

			strValue.assign("").appendUint(playerGameCache.size());
			heLogger->compose(reportBody, "playerGameCacheSz", strValue);

			strValue.assign("").appendUint(evaluationStatus);
			heLogger->compose(reportBody, "evaluationStatus", strValue);

			heLogger->post(reportBody);
		}
	}
	catch (const PError& er)
	{
		PLog("Error: '%s'", er.why());
	}

	PLog("#reportPerformance");
}

void ColCdaServerObject::reconnect(CommClientGConnection& conn, 
								   const char* const serverName, 
								   const char* const serverObject, 
								   const char* const serverConnectionType)
{
	PLog("reconnect(serverName: %s, serverObject: %s, serverConnectionType: %s)"
		, (serverName && *serverName) ? serverName : "none"
		, (serverObject && *serverObject) ? serverObject : "none"
		, (serverConnectionType && *serverConnectionType) ? serverConnectionType : "none"
		);

	ConnUtils::createClientConnection(cliPool, conn, serverName, serverObject, serverConnectionType);

	// Based on assumption that isConnected() returns logical status, but not status of physical connection, let's check that it got 'connected'
	if (!conn.underlyingConnection().isConnected())
	{
		PString err("Failed to connect to ");
		err
			.append(serverName)
			.append("/")
			.append(serverObject)
			.append("/")
			.append(serverConnectionType)
			;

		PLog(LOG_TRACE_FAULTY_LOGIC ": %s", err.c_str());
	}
}


void ColCdaServerObject::stopHeLogger()
{
	PLog("stopHeLogger");

	if (heLogger)
	{
		delete heLogger;
		heLogger = NULL;
	}

	if (heLoggerTimerHandle)
	{
		deleteTimerMessage(heLoggerTimerHandle);
		heLoggerTimerHandle = 0;
	}
}

void ColCdaServerObject::startHeLogger(const char* serverAddress, const char* stationName)
{
	PLog("startHeLogger");
	PLog("- serverAddress: %s", serverAddress);
	PLog("- stationName: %s", stationName);

	stopHeLogger();

	if (!heLogger)
	{
		heLogger = new HeLogger;
		heLogger->init(cliPool, serverAddress, stationName, HE_LOGGER_LOCAL_TIME_ANSWERING);
	}

	if (!heLoggerTimerHandle)
	{
		CommMsgBody emptyBody;
		heLoggerTimerHandle = postTimerMessage(TIMER_LOGGER, emptyBody, TIMER_LOGGER_MSEC);
	}
}

bool ColCdaServerObject::checkAdminRight(const char* right, const AdminConnection& adminConn)
{
	if (adminConn.checkAdminRight(right))
	{
		return true;
	}

	return false;
}

void ColCdaServerObject::processAdminMessage(UINT32 msgId, const CommMsgBody& body, CommServerConnection* conn)
{
	logServerMsg("A", msgId, conn, AdminConnection::tryGetAdminId(conn));

	AdminConnection* adminConn = dynamic_cast<AdminConnection*>(conn);
	if (!adminConn)
	{
		PLog("Incoming connection from admin lost!");
		return;	// no reply!
	}

	CommMsgParser parser(body);
	UINT32 reqId;
	parser.parseUINT32(reqId);

	UINT32 replyId = msgId + 1; // By local convention
	CommMsgBody reply;
	reply.composeUINT32(reqId);

	INT16 errCode = DBM_NO_ERROR;
	PString errBuf;

	switch(msgId)
	{
	case MSG_ADMIN_REREAD_INI:
		{
			PLog("[ColCdaServerObject::processAdminMessage] MSG_ADMIN_REREAD_INI has been received");
			if (checkAdminRight(SYSADMIN_STR, *adminConn))
			{
				dynamicInit();
				PLog("[ColCdaServerObject::processAdminMessage] Dynamic section of INI File has been successfully reloaded");
			}
			else
			{
				errCode = NOT_ALLOWED;
				errBuf = "Insufficient rights";
				PLog("[ColCdaServerObject::processAdminMessage] Not enough right");

			}

			break;
		}
	default:
		{
			PLog("Error: Unhandled Admin message #%u", msgId);
			return; // No reply
		}
	}

	setReply(reqId, errCode, errBuf, reply);
	postReply(adminConn, replyId, reply);
}

void ColCdaServerObject::processTrustedAdminMessage(UINT32 msgId, const CommMsgBody& body, CommServerConnection* conn)
{
	UINT32 reqId = 0;
	CommMsgParser wrapperParser(body);
	wrapperParser.parseUINT32(reqId);

	MsgAuthRedirectorToGateway msg(wrapperParser);

	logServerMsg("TA", msgId, conn, msg.adminId);

	CommMsgBody reply;

	switch (msgId)
	{
	case MSG_ADMIN_REREAD_INI:
		{
			PLog("[ColCdaServerObject::processTrustedAdminMessage] MSG_ADMIN_REREAD_INI has been received");

			try
			{
				INT16 errCode = DBM_NO_ERROR;
				PString errBuf;

				if (checkAdminRight(SYSADMIN_STR, msg.adminRights))
				{
					dynamicInit();
					PLog("[ColCdaServerObject::processTrustedAdminMessage] Dynamic section of INI File has been successfully reloaded");
				}
				else
				{
					errCode = NOT_ALLOWED;
					errBuf = "Insufficient rights";
					PLog("[ColCdaServerObject::processTrustedAdminMessage] Not enough rights for user: %s", msg.adminId.c_str());
				}

				setReply(reqId, errCode, errBuf, reply);
			}
			catch(const PError& er)
			{
				setReply(reqId, 1, er.why(), reply);

				PLog("[ColCdaServerObject::processTrustedAdminMessage] Exception: %s", er.why());
			}

			postReply(conn, MSG_ADMIN_REREAD_INI_REPLY, reply);

			break;
		}

	case MSG_ADMIN_KILL_ITSELF:
		{
			PLog("[ColCdaServerObject::processTrustedAdminMessage] MSG_ADMIN_KILL_ITSELF has been received");

			if (checkAdminRight(HANDLE_KILL_ITSELF, msg.adminRights))
			{
				setReply(reqId, DBM_NO_ERROR, "", reply);
				postReply(conn, MSG_ADMIN_KILL_ITSELF_REPLY, reply);

				shutdown();
				this->exit();
			}

			PLog("[ColCdaServerObject::processTrustedAdminMessage] MSG_ADMIN_KILL_ITSELF");
			break;
		}

	default:
		{
			PLog("ColCdaServerObject::processTrustedAdminMessage =====> Unsupported message msgId=%d", msgId);
			break; // No reply
		}
	}
}

void ColCdaServerObject::processOlapMDMessage(UINT32 msgId, const CommMsgBody& body, CommServerConnection* conn)
{
	logServerMsg("O", msgId, conn, "");

	CommMsgParser parser(body);
	UINT32 reqId;

	UINT32 dbmId = 0;
	OlapMsgSysInfo msgSysInfo(OlapMsgSysInfo::eOlapGroupDefault, eHost::eHost_Unknown, dbmId);
	OlapMsgHandler::extractMsgData(msgId, parser, reqId, &msgSysInfo);

	UINT32 sourceHostId = 0;
	if (!msgSysInfo.retrieveSourceHostId(sourceHostId))
	{
		sourceHostId = eHost_Unknown;
	}

	PTRACE5("HOSTID=%u", sourceHostId);

	CommMsgBody reply;
	setReply(reqId, DBM_NO_ERROR, "", reply);
	postReply(conn, msgId + DBM_OLAP_DELTA, reply);

	if (!serverEnabled)
	{
		PLog("# Server");

		switch (msgId)
		{
		case DBM_Q_MESSAGE_FRAME_FINISHED: // OLAP message chaining protocol
			{
				BYTE toBeContinued;
				parser.
					parseBYTE(toBeContinued);
				if (toBeContinued)
				{
					CommMsgBody request;
					request.
						composeUINT32(0);  // dummy ID to send upstream
					conn->post(DBM_Q_GET_SAVED_MESSAGES, request);
					PLog("Expecting more saved messages");
				}
				else
				{
					PLog("All saved messages received");
				}

				return; // return, not break
			}
		default:
			{
				PTRACE("ColCdaServerObject::processOlapMDMessage =====> Message #%u from OlapMD", msgId);

				return; // return, not break
			}
		}

		return;  // Ensure we do not continue to process
	}

	switch (msgId)
	{
	case DBM_Q_MESSAGE_FRAME_FINISHED: // OLAP message chaining protocol
		{
			BYTE toBeContinued;
			parser.
				parseBYTE(toBeContinued);
			if (toBeContinued)
			{
				CommMsgBody request;
				request.
					composeUINT32(0);  // dummy ID to send upstream
				conn->post(DBM_Q_GET_SAVED_MESSAGES, request);
				PLog("Expecting more saved messages");
			}
			else
			{
				PLog("All saved messages received");
			}

			return; // return, not break
		}
	case DBM_Q_OLAP_INSERT_TRANSACT:
		{
			processTransact(parser);
			break;
		}
	case DBM_Q_OLAP_TOURNAMENT_NEW:			// 20016
		{
			processTournNew(parser, false);
			break;
		}
	case DBM_Q_OLAP_TOURNAMENT_CREATE:		// 22065
		{
			processTournNew(parser, true);
			break;
		}
	case DBM_Q_OLAP_SET_TOURN_CLOSED:		// 21032
		{
			processTournClosed(parser, sourceHostId);
			break;
		}
	case DBM_Q_OLAP_UPDATE_TOURNSTRUCTURE:	// 20870
		{
			processUpdateTournStructure(parser);
			break;
		}
	default:
		{
			PLog("Error: Unhandled OLAP message #%u", msgId);
			break;
		}
	}
}

void ColCdaServerObject::processColossusMessage(UINT32 msgId, const CommMsgBody& body, CommServerConnection* conn)
{
	logServerMsg("C", msgId, conn, "");

	CommMsgParser parser(body);
	UINT32 reqId;
	parser.parseUINT32(reqId);

	UINT32 replyId = msgId + 1; // By local convention
	CommMsgBody reply;

	switch(msgId)
	{
	case COLCDA_NOTIFY_RING_HAND:
		{
			processRingHand(parser);
			break;
		}
	case COLCDA_NOTIFY_TOURN_HAND:
		{
			processTournHand(parser);
			break;
		}
	case COLCDA_NOTIFY_CDH:
		{
			processCDHHand(parser);
			break;
		}
	default:
		{
			PLog("Error: Unhandled Colossus message #%u", msgId);
			return; // No reply
		}
	}

	setReply(reqId, DBM_NO_ERROR, "", reply);
	postReply(conn, replyId, reply);
}

void ColCdaServerObject::processRingHand(CommMsgParser& parser)
{
	if (!handleRing)
	{
		return;
	}

	UINT64 handId = 0;
	parser.parseUINT64(handId);
	ColCDA::RingHand ccdaRingHand(parser);

	if (moreTrace)
	{
		PLog("get RH=%llu", handId);
	}

	if (ccdaRingHand.winLossMatrix_USD.size() == 0)
	{
		PLog("Error- invalid winlossmatrix handId=%llu", handId);
		return;
	}

	convertRingHandCurrency(ccdaRingHand);
	if (isRingHandQualifiedAnyRules(ccdaRingHand))
	{
		time_t now = time(0);
		time_t handTime = now - handTimeTooOldSecs;
		ringHandCache.add(handTime, handId, ccdaRingHand);
	}
}

void ColCdaServerObject::processTournHand(CommMsgParser& parser)
{
	if (!handleTourn)
	{
		return;
	}

	ColCDA::TournHand cdaTournHand(parser);

	if (moreTrace)
	{
		PLog("get TH=%llu", cdaTournHand.handId);
	}

	newTournCache.addTournHand(cdaTournHand);
}

void ColCdaServerObject::processCDHHand(CommMsgParser& parser)
{
	if (!handleRing && !handleTourn)
	{
		return;
	}

	SrvTime now;
	now.currentLocalTime();

	vector<ColShd::CdhEvent> cdhEventVec;
	parseVector(parser, cdhEventVec);
	for (int i = 0; i < cdhEventVec.size(); ++i)
	{
		if (moreTrace)
		{
			PLog("get CDH=%llu", cdhEventVec[i].handId);
		}

		convertCDHEventCurrency(cdhEventVec[i]);

		cdhHandCache.add(cdhEventVec[i]);

		// evaluate banker
		if (cdhEventVec[i].tournId)
		{
			triggerTournEval(cdhEventVec[i].bankerUserId, now);
		}
		else
		{
			triggerRingEval(cdhEventVec[i].bankerUserId, now);
		}
	}
}

void ColCdaServerObject::processTransact(CommMsgParser& parser)
{
	Transact transact(parser);

	if (!transact.isRelevant())
		return;

	bool shouldTriggerEval = false;
	playerGameCache.addTransaction(transact, shouldTriggerEval);

	if (shouldTriggerEval)
	{
		SrvTime now;
		now.currentLocalTime();
		triggerRingEval(transact.userId, now);
	}
}

void ColCdaServerObject::processTournNew(CommMsgParser& parser, bool isTournCreateMessage)
{
	SrvTime srvNow;
	UINT32 dummy;
	parser.
		parseSrvTime(srvNow).
		parseUINT32(dummy).				// emailId
		parseUINT32(dummy);				// dbmId

	ColossusTournInfo ti;
	UINT32 tournId = ti.parseFromScript(parser);

	PLog("NewTourn-%u", ti.tournId);
	if (isTournQualifiedAnyRules(ti))
	{
		SrvTime now;
		now.currentLocalTime();
		newTournCache.add(ti.tournId, now);
	}
}

void ColCdaServerObject::processTournClosed(CommMsgParser& parser, UINT32 hostId)
{
	UINT32 tournId;
	BYTE   status;
    SrvTime closed;

	parser
		.parseUINT32(tournId)
		.parseBYTE(status)
		.parseSrvTime(closed);

	PLog("CloseTourn-%u", tournId);

	if (status != TOURN_STATUS_CLOSED && status != TOURN_STATUS_NORMAL)
	{
		newTournCache.erase(tournId);
		return;
	}

	NewTourn tourn;
	if (!newTournCache.find(tournId, tourn))
	{
		//PLog("Warning: tourn not found in NewTournCache, id=%u", tournId);
		return;
	}

	TournRetryData tournRetryData(tournId, hostId, 0 /* retries */);
	CommMsgBody timerRequest;
	tournRetryData.compose(timerRequest);
	postTimerMessage(TIMER_TOURN_DELAY, timerRequest, firstTournRetryDelay * 1000);
}

void ColCdaServerObject::processGetTournInfo(CommMsgParser& parser)
{
	TournRetryData tournRetryData(parser);

	PLog("GetTournInfo-%u", tournRetryData.tournId);

	CommMsgBody request;
	request.composeUINT32(tournRetryData.tournId);

	postToReader(COLOSSUS_GET_PPT_INFO, request, new TournRetryCall(*this, tournRetryData));
}

void ColCdaServerObject::processReceiveTournData(const CommMsgBody& body, TournRetryData& tournRetryData)
{
	CommMsgParser parser(body);

	INT16 errCode;
	PString sqlErr;
	parser.parseINT16(errCode);

	if (errCode)
	{
		parser.parseStringP(sqlErr);
		PLog("Warning: COLOSSUS_GET_PPT_INFO failed: %s", sqlErr.c_str());
	}
	else
	{
		ColossusPPTInfo info;
		info.parse(parser);

		if (info.isTournClosed())
		{
			// add tourn data into cache
			UINT64 tournId = info.tournInfo.tournId;
			NewTourn newTourn;
			if (!newTournCache.find(tournId, newTourn))
			{
				// keep going. still add tourn into cache
				PLog("Warning: not found NewTournCache=%llu", tournId);
			}

			TournData tournData(info, newTourn.numHands);

			tournDataCache.add(info.tournInfo.tournId, tournData);
			newTournCache.erase(tournId);

			// convert tourn data currency
			convertTournDataCurrency(tournData);

			PString firstPlaceUserId;

			PLog("Tourn player size=%u", info.players.size());

			for (UINT32 i = 0; i < info.players.size(); ++i)
			{
				const ColossusPPTPlayerInfo & playerInfo = info.players[i];

				PLog("PlayerId='%s', place=%u", playerInfo.userId.c_str(), playerInfo.place);

				if (playerInfo.place == 1)
				{
					firstPlaceUserId = playerInfo.userId;
					break;
				}
			}

			SrvTime now;
			now.currentLocalTime();
			if (firstPlaceUserId.length() > 0)
			{
				triggerTournEval(firstPlaceUserId, now);
			}
			else
			{
				PLog("Error: did not find first place user. tourId=%llu", tournId);
			}
			return;
		}
	}

	++tournRetryData.retries;
	tournRetryData.log();

	if (tournRetryData.retries > maxTournRetries)
	{
		PLog("Error: Maximum retries (%u) exceeded - Aborting.", maxTournRetries);
		return;
	}

	PLog("Will retry in %u Sec.", tournRetryDelay);

	CommMsgBody timerRequest;
	tournRetryData.compose(timerRequest);
	postTimerMessage(TIMER_TOURN_DELAY, timerRequest, tournRetryDelay * 1000);
}

// PYR-114096
void ColCdaServerObject::processUpdateTournStructure(CommMsgParser& parser)
{
	// See /coreserver/dbm/sharedtournstmt.cpp, UpdateTournStructureStmt::parseOlapMessage()

	UINT32 tournId = 0;
	UINT64 tournFlags2 = 0;
	parser
		.parseUINT32(tournId)
		.parseUINT64(tournFlags2);

	PLog("UpdateTourn-%u,0x%llX", tournId, tournFlags2);

	tournDataCache.updateTournFlags2(tournId, tournFlags2);
}

INT64 ColCdaServerObject::convertCurrencyM64(INT64 amtSrc, const char* curSrc, const char* curDst) const
{
	return ::convertCurrency64(amtSrc, curSrc, curDst, currencies);
}

INT64 ColCdaServerObject::convertUserWagerAmt(const WageringPerCurrVec& wagerCurVec, const char* desCur)
{
	INT64 wagerAmt = 0;
	for (int i = 0; i < wagerCurVec.size(); ++i)
	{
		wagerAmt += convertCurrencyM64(wagerCurVec[i].amount, wagerCurVec[i].currency, desCur);
	}
	return wagerAmt;
}

INT64 ColCdaServerObject::convertAmountCurr(const AmountPerCurrMap& amountPerCurrMap, const char* desCur)
{
	INT64 totalAmount = 0;

	for (const auto& amountPerCurrency : amountPerCurrMap)
	{
		totalAmount += convertCurrencyM64(amountPerCurrency.second, amountPerCurrency.first, desCur);
	}
	return totalAmount;
}

void ColCdaServerObject::convertCDHEventCurrency(ColShd::CdhEvent& cdhEvent)
{
	if (moreTrace)
	{
		PLog("convertCDHEventCurrency - handid=%llu, curr=%s", cdhEvent.handId, cdhEvent.currency.c_str());
	}

	if (cdhEvent.currency.equals("INR"))
	{
		// LT Poker MPW has been converted to USD in CDH event
		// need to convert to INR since LT Poker MPW filter value depends on currency.
		const char* curDest = "INR";
		cdhEvent.dumperLtPokerMpwCents = convertCurrencyM64(cdhEvent.dumperLtPokerMpwCents, "USD", curDest);
	}

	if (cdhEvent.tournId != 0)
	{
		// no need to convert Tourn chips
		return;
	}

	// PYR-86936
	if (!cdhEvent.currency.equals("INR"))
	{
		// If CDH currency is not INR, convert to USD equivalent
		const char* curSrc = cdhEvent.currency;
		cdhEvent.blindSzCents = convertCurrencyM64(cdhEvent.blindSzCents, curSrc, "USD");
		
		cdhEvent.potSizeFinalCents = convertCurrencyM64(cdhEvent.potSizeFinalCents, curSrc, "USD");
		cdhEvent.potSizeFlopCents = convertCurrencyM64(cdhEvent.potSizeFlopCents, curSrc, "USD");
		cdhEvent.potSizeTurnCents = convertCurrencyM64(cdhEvent.potSizeTurnCents, curSrc, "USD");
		cdhEvent.potSizeRiverCents = convertCurrencyM64(cdhEvent.potSizeRiverCents, curSrc, "USD");
		cdhEvent.potSizeSeventhCents = convertCurrencyM64(cdhEvent.potSizeSeventhCents, curSrc, "USD");

		cdhEvent.bankerChipsStartingCents = convertCurrencyM64(cdhEvent.bankerChipsStartingCents, curSrc, "USD");
		cdhEvent.bankerChipsOnFlopCents = convertCurrencyM64(cdhEvent.bankerChipsOnFlopCents, curSrc, "USD");
		cdhEvent.bankerChipsOnTurnCents = convertCurrencyM64(cdhEvent.bankerChipsOnTurnCents, curSrc, "USD");
		cdhEvent.bankerChipsOnRiverCents = convertCurrencyM64(cdhEvent.bankerChipsOnRiverCents, curSrc, "USD");
		cdhEvent.bankerChipsEndingCents = convertCurrencyM64(cdhEvent.bankerChipsEndingCents, curSrc, "USD");

		cdhEvent.dumperChipsStartingCents = convertCurrencyM64(cdhEvent.dumperChipsStartingCents, curSrc, "USD");
		cdhEvent.dumperChipsOnFlopCents = convertCurrencyM64(cdhEvent.dumperChipsOnFlopCents, curSrc, "USD");
		cdhEvent.dumperChipsOnTurnCents = convertCurrencyM64(cdhEvent.dumperChipsOnTurnCents, curSrc, "USD");
		cdhEvent.dumperChipsOnRiverCents = convertCurrencyM64(cdhEvent.dumperChipsOnRiverCents, curSrc, "USD");
		cdhEvent.dumperChipsEndingCents = convertCurrencyM64(cdhEvent.dumperChipsEndingCents, curSrc, "USD");
	}
}

void ColCdaServerObject::convertRingHandCurrency(RingHand& ringHand)
{
	const char* curSrc = ringHand.currency;

	ringHand.blindSzCents_USD = convertCurrencyM64(ringHand.blindSzHandCurrencyCents, curSrc, "USD");

	// Dependent on Currency Selector.  USD Equivalent for USD/GBP/EUR. INR for INR. 
	if (!ringHand.currency.equals("INR"))
	{
		ringHand.blindSzHandCurrencyCents = convertCurrencyM64(ringHand.blindSzHandCurrencyCents, curSrc, "USD");
	}

	for (int i = 0; i < ringHand.playersVec.size(); ++i)
	{
		UserInRingHand& uir = ringHand.playersVec[i];
		uir.deltaCents_USD = convertCurrencyM64(uir.deltaCents_USD, curSrc, "USD");
		uir.potChips_USD = convertCurrencyM64(uir.potChips_USD, curSrc, "USD");
	}

	for (int i = 0; i < ringHand.winLossMatrix_USD.size(); ++i)
	{
		WinLossRow& wlr = ringHand.winLossMatrix_USD[i];
		for (int j = 0; j < wlr.size(); ++j)
		{
			wlr[j] = convertCurrencyM64(wlr[j], curSrc, "USD");
		}
	}
}

void ColCdaServerObject::convertTournDataCurrency(TournData& tournData)
{
	const char* curSrc = tournData.currency;

	// Dependent on Currency Selector.  USD Equivalent for USD/GBP/EUR. INR for INR. 
	if (!tournData.currency.equals("INR"))
	{
		tournData.buyIn_RuleCurrency = convertCurrencyM64(tournData.buyIn_RuleCurrency, curSrc, "USD");
	}

	for (int i = 0; i < tournData.players.size(); ++i)
	{
		TournPlayer& player = tournData.players[i];
		player.totalInvested_USD = convertCurrencyM64(player.totalInvested_USD, curSrc, "USD");
		player.rakePaid_USD = convertCurrencyM64(player.rakePaid_USD, curSrc, "USD");
		player.payout_USD = convertCurrencyM64(player.payout_USD, curSrc, "USD");
	}
}

void ColCdaServerObject::processVelocityControlNotification()
{
	PTRACE("%s", __FUNCTION__);

	CommMsgBody body;
	body.composeString(COLCDA_INIDATA_OBJ_NAME);

	if (olapConn.isConnected())
	{
		postToOlap(DBM_Q_GET_INIDATA2, body, new EmailNotificationRequestCall(this));
	}
	else
	{
		PTRACE("Lost connection to OLAP, aborting");
	}
}

void ColCdaServerObject::processGotIniDataReply(CommMsgParser& parser)
{
	PTRACE("%s", __FUNCTION__);

	INT16 errCode = 0;
	parser.parseINT16(errCode);
	if (errCode != DBM_NO_ERROR)
	{
		const char* errStr;
		parser.parseString(errStr);
		PLog("DBM Error: %s", errStr);
		return;
	}

	IniDataFromDbm iniData;
	iniData.parse(parser);

	const IniDataFromDbm::Section* pSection = iniData.getSection(COLCDA_INIDATA_CCDA_SECTION);
	if (pSection == nullptr)
	{
		PTRACE("Warning: no such section '%s'", COLCDA_INIDATA_CCDA_SECTION);
		return;
	}

	vector<PString> emailVec;
	for (const IniDataFromDbm::Item& item : pSection->items)
	{
		if (item.name.equals(COLCDA_INIDATA_CCDA_EMAIL_ITEM))
		{
			emailVec.push_back(item.value);
		}
	}

	if (emailVec.empty())
	{
		PTRACE("No emails to send, aborting");
		return;
	}

	PTRACE("%u emails to send", emailVec.size());

	PString bodyStr;
	bodyStr.assign("CCDA Velocity Control is triggered. CCDA system has stopped evaluation. Please update CCDA rules and resume evaluation.");

	CommMsgBody body;
	body.composeUINT32(eEmailDirectMultiTo);
	composeVector(body, emailVec, ComposerPString());
	body.
		composeString("colcda_DO_NOT_REPLY@pokerstars.com").					// fromEmail
		composeString("CCDA Velocity Control Notification").					// subj
		composeString(bodyStr).													// body
		composeString("");														// htmlBody

	if (templateSrvConn.isConnected())
	{
		postToTemplateSrv(TS_Q_COMMON_SEND_EMAIL, body, new EmailNotificationRequestCall(this));
	}
	else
	{
		PTRACE("Lost connection to Template Server, aborting");
	}
}

void ColCdaServerObject::processGotEmailSentReply(CommMsgParser& parser)
{
	PTRACE("%s", __FUNCTION__);

	PTRACE("Email sent, done");
}

void ColCdaServerObject::sweepCaches()
{
	SrvTime lookBackTime;
	lookBackTime.currentLocalTime();
	INT32 lookbackSeconds = -CCDA_MAX_LOOKBACK_PERIOD_MINUTES * 60;
	ColUtils::addSeconds(lookBackTime, lookbackSeconds);
	
	ringHandCache.cull(lookBackTime.encode(), maxNumHandsAllowed);
	cdhHandCache.cull(lookBackTime.encode(), maxNumCDHUserAllowed);
	newTournCache.cull(lookBackTime.encode());
	tournDataCache.cull(lookBackTime.encode(), maxNumTournAllowed);
	playerGameCache.cull(lookBackTime.encode());

	if (velocityControlRule.isVelocityControlEnabled())
	{
		srmMonitorCache.cull(velocityControlRule.getVelocityControlLookbackPeriod());
	}
}

bool ColCdaServerObject::checkAdminRight(const char* rightName, const vector<PString> adminRights) const
{
	for (UINT32 i = 0; i < adminRights.size(); ++i)
	{
		if (adminRights[i].equals(rightName))
		{
			return true;
		}
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////

