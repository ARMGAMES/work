////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2019 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "GenericSrvObject.h"
#include "dbm.h"
#include "connUtils.h"
#include "AuthRedirector.h"
#include "admincommon.h"

/* static */ CommClientNullGuardFactory GenericServerObjectBase::clientNullGuardFactory;
/* static */ CommServerPlainTextPasswordSidGuardFactory GenericServerObjectBase::authGuardFactory;
/* static */ CommServerNullGuardFactory GenericServerObjectBase::guardFactory;

void GenericServerObjectBase::postReply(CommServerConnection* pConn, UINT32 msgId, CommMsgBody& body)
{
	if (pConn)
	{
		pConn->post(msgId, body);
	}
	else
	{
		PTRACE("Message #%u not sent (no connection)", msgId);
	}
}

void GenericServerObjectBase::postReply(UINT32 connId, UINT32 msgId, CommMsgBody& body)
{
	CommServerConnection* pConn = findConnection(connId);
	postReply(pConn, msgId, body);
}

void GenericServerObjectBase::setReply(CommMsgBody& reply, INT16 errCode, const char* errDesc)
{
	reply.composeINT16(errCode);

	if (errCode)
	{
		reply.composeString(errDesc);
	}
}

bool GenericServerObjectBase::checkAdminRight(const char* right, const GenericPrivilegedSrvConn* pConn, CommMsgBody& reply)
{
	if (pConn->checkAdminRight(right))
	{
		return true;
	}

	setReply(reply, NOT_ALLOWED, "Insufficient rights");
	return false;
}

////////////////////////////////////////////////////////////////////////////////

GenericServerObjectBase::GenericServerObjectBase(_CommInterface& inter_)
	: CommServerObject(inter_)
	, srvPool(inter_)
	, cliPool(inter_)
	, connTrustedAdminFactory(*this, guardFactory)
	, useLogger(false)
	, heLogger(nullptr)
	, heLoggerTimerHandle(0)
{

}

/* virtual */
GenericServerObjectBase::~GenericServerObjectBase()
{
	heLogger = nullptr;
}

/* virtual */
void GenericServerObjectBase::init(CommMsgBody& initMsg)
{
	PTRACE("GenericServerObjectBase::init");

	doPreInit(initMsg);

	CommMsgParser parser(initMsg);
	const char* initDir_;
	parser.parseString(initDir_);
	initDir.assign(initDir_);
	fullFileName.assign(initDir_);
	fullFileName.append(getConfigFileName());
	PIniFile ini(fullFileName);

	if (!parser.parseEnded())
	{
		parser.parseStringP(loggerStationName);
		PLog("logger station name [%s] was provided at startup and will override correspondent logger's INI property", loggerStationName.c_str());
	}

	dynamicInit();

	srvPool.registerFactory(TRUSTEDADMIN_CONNTYPE, connTrustedAdminFactory);
	doInitRegisterFactories();

	doPostInit();

	PTRACE("#GenericServerObjectBase::init");
}

/* virtual */
void GenericServerObjectBase::dynamicInit()
{
	PTRACE("GenericServerObjectBase::dynamicInit");

	PIniFile ini(fullFileName);

	doPreDynamicInit(ini);

	PIniFile::Section* section = ini.getSection("DYNAMICCONNECT");
	if (section)
	{
		doDynamicInitReconnect(*section);
	}
	else
	{
		PLog(LOG_TRACE_FAULTY_LOGIC ": Cannot read [DYNAMICCONNECT]");
		throw PError("dynamicInit - Cannot read [DYNAMICCONNECT]");
	}

	section = ini.getSection("DYNAMICSETTINGS");
	if (section)
	{
		doDynamicInitReadSettings(*section);
	}
	else
	{
		PLog(LOG_TRACE_FAULTY_LOGIC ": Cannot read [DYNAMICSETTINGS]");
		throw PError("dynamicInit - Cannot read [DYNAMICSETTINGS]");
	}

	doDynamicInitLogSettings();

	stopHeLogger();
	section = ini.getSection("LOGGER");
	if (section)
	{
		useLogger = (0 != section->getIntProperty("uselogger", 0));
		PString loggerServerAddress = section->getProperty("server", "");

		if (!loggerStationName.length())
		{
			loggerStationName = section->getProperty("station", "");
		}

		PLog("useLogger: %s", useLogger ? "YES" : "NO");
		PLog("loggerServerAddress: %s", loggerServerAddress.c_str());
		PLog("loggerStationName: %s", loggerStationName.c_str());

		if (useLogger && !loggerServerAddress.equals("") && !loggerStationName.equals(""))
		{
			startHeLogger(loggerServerAddress, loggerStationName);
		}
		else
		{
			PTRACE("No LOGGER started");
		}
	}
	else
	{
		PTRACE("No LOGGER configured");
	}

	doPostDynamicInit(ini);

	PLog("#GenericServerObjectBase::dynamicInit");
}

/* virtual */
void GenericServerObjectBase::reconnect(CommClientGConnection& conn, const char* const serverName, const char* const serverObject, const char* const serverConnectionType)
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

/* virtual */
void GenericServerObjectBase::shutdown()
{
	PLog("shutdown");

	CommServerObject::shutdown();

	doPreShutdown();

	if (useLogger)
	{
		stopHeLogger();
		delete heLogger;
	}

	doPostShutdown();
}

/* virtual */
void GenericServerObjectBase::processMessage(_CommMsg& msg)
{
	srvPool._processMessage(msg) || cliPool._processMessage(msg);
}

/* virtual */
void GenericServerObjectBase::processCommonServerMessage(UINT32 msgId, const CommMsgBody& body, ServerConnectionType srvConnType, GenericSrvConn* conn)
{
	if (!isConnTypeSupported(srvConnType))
	{
		PString buf("Unsupported eServerConnection value: ");
		buf.appendInt(srvConnType);
		throw PError(buf);
	}

	PString extraLogMarkers;
	conn->getExtraLogMarkers(extraLogMarkers);

	PLog("{%s %u %08x%s", conn->getConnMarker(), msgId, conn->id(), extraLogMarkers.c_str());

	HiresCounter counter;

	bool wasHandled = false;

	switch (srvConnType)
	{
		case TrustedAdmin:
		{
			TrustedAdminSrvConn* conn_ = dynamic_cast<TrustedAdminSrvConn*>(conn);
			processTrustedAdminMessage(msgId, body, conn_);

			wasHandled = true;
			break;
		}
		default:
		{
			wasHandled = false;
			break;
		}
	}

	if (!wasHandled)
	{
		doProcessCommonServerMessage(msgId, body, srvConnType, conn);
	}

	counter.stop();
	UINT32 ticks = counter.getElapsedTick();
	ticksAccumulator.addMessageTicks(counter);
	PTRACE("}%u%s", ticks, (ticks > TICKS_PER_SECOND) ? " overspan" : "");
}

/* virtual */
void GenericServerObjectBase::processTimerMessage(UINT32 handle, UINT32 msgId, CommMsgBody& body)
{
	CommServerObject::processTimerMessage(handle, msgId, body);

	HiresCounter counter;
	PTRACE("{T %u", msgId);

	bool wasHandled = false;
	doProcessTimerMessage(handle, msgId, body, wasHandled);

	if (!wasHandled)
	{
		PTRACE("Error: %s invalid timer msgId: %u", __FUNCTION__, msgId);
	}

	counter.stop();
	UINT32 ticks = counter.getElapsedTick();
	ticksAccumulator.addMessageTicks(counter);
	PTRACE("}%u%s", ticks, (ticks > TICKS_PER_SECOND) ? " overspan" : "");
}

/* virtual */
void GenericServerObjectBase::processTrustedAdminMessage(UINT32 msgId, const CommMsgBody& body, CommServerConnection* conn)
{
	TrustedAdminSrvConn* adminConn = dynamic_cast<TrustedAdminSrvConn*>(conn);
	if (!adminConn)
	{
		PLog("Incoming connection from admin lost!");
		return;	// no reply!
	}

	UINT32 reqId = 0;
	CommMsgParser wrapperParser(body);
	wrapperParser.parseUINT32(reqId);

	MsgAuthRedirectorToGateway msg(wrapperParser);
	PLog("Trusted msgId = %d from %s", msgId, msg.adminId.c_str());

	for (const PString& adminRight : msg.adminRights)
	{
		adminConn->addAdminRight(adminRight);
	}

	bool wasHandled = false;
	doProcessTrustedAdminMessage(reqId, msgId, msg, adminConn, wasHandled);

	if (!wasHandled)
	{
		PTRACE("Error: %s invalid trusted admin msgId: %u", __FUNCTION__, msgId);
	}
}

/* virtual */
void GenericServerObjectBase::startHeLogger(const char* strServerAddress, const char* strStationName)
{
	if (!heLogger)
	{
		heLogger = new HeLogger;
		heLogger->init(cliPool, strServerAddress, strStationName, HE_LOGGER_LOCAL_TIME_ANSWERING);
	}

	if (!heLoggerTimerHandle)
	{
		CommMsgBody timerBody;
		heLoggerTimerHandle = postTimerMessage(LOGGER_TIMER, timerBody, TIMER_LOGGER_MSEC);
	}
}

/* virtual */
void GenericServerObjectBase::stopHeLogger()
{
	if (heLoggerTimerHandle)
	{
		deleteTimerMessage(heLoggerTimerHandle);
		heLoggerTimerHandle = 0;
	}
}

/* virtual */
void GenericServerObjectBase::reportPerformance()
{
	PLog("reportPerformance");

	ticksAccumulator.stop();

	doReportPerformanceToFile();

	if (useLogger)
	{
		try
		{
			CommMsgBody reportBody;
			PString strValue;

			UINT32 messagesPerSec = ticksAccumulator.messagesPerSec();
			strValue.assign("").appendInt(messagesPerSec);
			heLogger->compose(reportBody, "messagesPerSec", strValue);

			UINT32 avgProcessingMsec = ticksAccumulator.avgProcessingMkSec();
			strValue.assign("").appendInt(avgProcessingMsec);
			heLogger->compose(reportBody, "avgProcessingMksec", strValue);

			strValue.assign("").appendInt(ticksAccumulator.avgProcessingMkSecTick());
			heLogger->compose(reportBody, "avgProcessingMksecTick", strValue);

			UINT32 loadPercentCnt = ticksAccumulator.loadPercentCntTick();
			strValue.assign("").appendInt(loadPercentCnt);
			heLogger->compose(reportBody, "loadPercentCounter", strValue);

			doComposePerformanceStats(reportBody);

			heLogger->post(reportBody);
		}
		catch (const PError& er)
		{
			PLog("reportPerformance error: '%s'", er.why());
		}
	}

	ticksAccumulator.reset();
}

/* virtual */
void GenericServerObjectBase::processRefreshSettings(CommMsgBody& reply)
{
	dynamicInit();

	setReply(reply, DBM_NO_ERROR, "");
}

/* virtual */
void GenericServerObjectBase::processKillServer(const char* adminId)
{
	PLog("Kill application message received from admin id: %s", adminId);

	shutdown();
	this->exit();
}

/* virtual */
void GenericServerObjectBase::processCallback(UINT32 reqId, UINT32 replyId, const CommMsgBody& body, CommClientGConnection::AsyncCall* pCall)
{
	HiresCounter counter;

	PLog("{ASC %u", replyId);

	if (pCall)
	{
		pCall->processReply(reqId, replyId, body);
	}
	else
	{
		PLog("No AsyncCall, message not handled");
	}

	counter.stop();
	UINT32 ticks = counter.getElapsedTick();
	ticksAccumulator.addMessageTicks(counter);

	PLog("}%u%s", ticks, (ticks > TICKS_PER_SECOND) ? " overspan" : "");
}

/* virtual */
CommServerConnection* GenericServerObjectBase::findConnection(UINT32 connId)
{
	return srvPool.findConnection(connId);
}

/* virtual */
void GenericServerObjectBase::addMessageTicks(const HiresCounter& counter)
{
	ticksAccumulator.addMessageTicks(counter);
}

/* virtual */
bool GenericServerObjectBase::isConnTypeSupported(int connType) const
{
	return connType > LowerGuard && connType < UpperGuard;
}

/* virtual */
void GenericServerObjectBase::doProcessTimerMessage(UINT32 handle, UINT32 msgId, CommMsgBody& body, bool& wasHandled)
{
	wasHandled = false;

	switch (msgId)
	{
		case LOGGER_TIMER:
		{
			CommMsgBody timerBody;
			heLoggerTimerHandle = postTimerMessage(LOGGER_TIMER, timerBody, TIMER_LOGGER_MSEC);
			reportPerformance();

			wasHandled = true;
			break;
		}
		default:
		{
			wasHandled = false;
			break;
		}
	}
}

/* virtual */
void GenericServerObjectBase::doProcessTrustedAdminMessage(UINT32 reqId, UINT32 msgId, MsgAuthRedirectorToGateway& /*msg*/, TrustedAdminSrvConn* adminConn, bool& wasHandled)
{
	wasHandled = false;

	switch (msgId)
	{
		case MSG_ADMIN_REREAD_INI:
		{
			PTRACE("MSG_ADMIN_REREAD_INI");

			CommMsgBody reply;
			reply.composeUINT32(reqId);

			if (checkAdminRight(SYSADMIN_STR, adminConn, reply))
			{
				processRefreshSettings(reply);
			}
			else
			{
				PTRACE("Not enough rights!");
			}

			postReply(adminConn, MSG_ADMIN_REREAD_INI_REPLY, reply);

			wasHandled = true;
			break;
		}
		case MSG_ADMIN_KILL_ITSELF:
		{
			PTRACE("MSG_ADMIN_KILL_ITSELF");

			CommMsgBody reply;
			reply.composeUINT32(reqId);

			if (checkAdminRight(HANDLE_KILL_ITSELF, adminConn, reply))
			{
				processKillServer(adminConn->getAdminId());
				// No reply!
			}
			else
			{
				PTRACE("Not enough rights!");
				postReply(adminConn, MSG_ADMIN_KILL_ITSELF_REPLY, reply);
			}

			wasHandled = true;
			break;
		}
		default:
		{
			wasHandled = false;
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

GenericServerWithLoginAuthObjectBase::GenericServerWithLoginAuthObjectBase(_CommInterface& inter_)
	: GenericServerObjectBase(inter_)
	, loginAuthConn(*this, clientNullGuardFactory, "L")
{

}

/* virtual */
void GenericServerWithLoginAuthObjectBase::processAdminMessage(UINT32 msgId, const CommMsgBody& body, CommServerConnection* conn)
{
	GenericPrivilegedSrvConn* adminConn = dynamic_cast<GenericPrivilegedSrvConn*>(conn);
	if (!adminConn)
	{
		PLog("Incoming connection from admin lost!");
		return;	// no reply!
	}

	CommMsgParser parser(body);
	UINT32 reqId;
	parser.parseUINT32(reqId);

	bool wasHandled = false;

	switch (msgId)
	{
		case MSG_ADMIN_REREAD_INI:
		{
			PTRACE("MSG_ADMIN_REREAD_INI");

			CommMsgBody reply;
			reply.composeUINT32(reqId);

			if (checkAdminRight(SYSADMIN_STR, adminConn, reply))
			{
				processRefreshSettings(reply);
			}
			else
			{
				PTRACE("Not enough rights!");
			}

			postReply(adminConn, MSG_ADMIN_REREAD_INI_REPLY, reply);

			wasHandled = true;
			break;
		}
		case MSG_ADMIN_KILL_ITSELF:
		{
			PTRACE("MSG_ADMIN_KILL_ITSELF");

			CommMsgBody reply;
			reply.composeUINT32(reqId);

			if (checkAdminRight(HANDLE_KILL_ITSELF, adminConn, reply))
			{
				processKillServer(adminConn->getAdminId());
				// No reply!
			}
			else
			{
				PTRACE("Not enough rights!");
				postReply(adminConn, MSG_ADMIN_KILL_ITSELF_REPLY, reply);
			}

			wasHandled = true;
			break;
		}
		default:
		{
			wasHandled = false;
			break;
		}
	}

	if (!wasHandled)
	{
		doProcessAdminMessage(reqId, msgId, parser, adminConn);
	}
}

/* virtual */
void GenericServerWithLoginAuthObjectBase::receiveAdminAuth(UINT32 /*replyId*/, CommMsgParser& parser, AdminAuthAsyncCall& callState)
{
	INT16 errCode;
	parser.parseINT16(errCode);

	UINT32 connId = callState.data;
	CommServerConnection* pConn = findConnection(connId);
	AdminSrvConnBase* adminConn = dynamic_cast<AdminSrvConnBase*>(pConn);

	if (!adminConn)
	{
		PLog("Lost admin connection, aborting");
		return;
	}

	adminConn->saveAuthError(errCode);

	if (errCode)
	{
		const char* errDescr = nullptr;
		parser.parseString(errDescr);
		adminConn->gotUserAuth(nullptr, errCode, errDescr);

		PLog("receiveAdminAuth error %d (%s)", errCode, errDescr);
		return;
	}

	// Some information in the authentication message is not needed
	UINT32 dummyUint32;
	const char* pDummyStr;
	CommMsgBody dummyBody;

	// Parse the authentication message
	UINT16 numRights;
	UINT32 userIntId;
	CommMsgBody authBody;
	parser.
		parseMsgBody(authBody).
		parseUINT32(dummyUint32).		// Privileges
		parseString(pDummyStr).			// User city
		parseUINT32(dummyUint32).		// Flags
		parseString(pDummyStr).			// User email
		parseUINT32(userIntId).
		parseString(pDummyStr).			// User country
		parseString(pDummyStr).			// User state
		parseMsgBody(dummyBody).		// Obsolete
		parseUINT16(numRights);

	// Extract and store rights
	for (UINT32 i = 0; i < numRights; ++i)
	{
		char const* right;
		parser.parseString(right);
		adminConn->addAdminRight(right);
	}

	// Skip the rest of the message as it is not needed.

	adminConn->setUserIntId(userIntId);

	CommMsgParser authParser(authBody);
	if (!adminConn->gotUserAuth(&authParser))
	{
		PLog("Authentication failed");
	}
}

/* virtual */
UINT32 GenericServerWithLoginAuthObjectBase::postToLoginAuth(UINT32 msgId, CommMsgBody& body, CommClientGConnection::AsyncCall* pCall /*= nullptr*/)
{
	return loginAuthConn.post(msgId, body, pCall);
}

/* virtual */
void GenericServerWithLoginAuthObjectBase::doDynamicInitReconnect(const PIniFile::Section& section)
{
	Parent_::doDynamicInitReconnect(section);

	const char* server = section.getProperty("LoginDbmServer");
	const char* serverObject = section.getProperty("LoginDbmObject", "DbmLoginInstance");
	const char* connectionType = section.getProperty("LoginAuthDbmConnectionType", getLoginAuthDbmConnType());
	reconnect(loginAuthConn, server, serverObject, connectionType);
}

/* virtual */
void GenericServerWithLoginAuthObjectBase::doInitRegisterFactories()
{
	Parent_::doInitRegisterFactories();

	if (connAdminFactory)
	{
		srvPool.registerFactory(ADMIN_CONNTYPE, *connAdminFactory);
	}
}

/* virtual */
void GenericServerWithLoginAuthObjectBase::doProcessCommonServerMessage(UINT32 msgId, const CommMsgBody& body, ServerConnectionType srvConnType, GenericSrvConn* conn)
{
	Parent_::doProcessCommonServerMessage(msgId, body, srvConnType, conn);

	switch (srvConnType)
	{
		case Admin:
		{
			AdminSrvConnBase* conn_ = dynamic_cast<AdminSrvConnBase*>(conn);
			processAdminMessage(msgId, body, conn_);

			break;
		}
		default:
		{
			break;
		}
	}
}

/* virtual */
void GenericServerWithLoginAuthObjectBase::doComposePerformanceStats(CommMsgBody& reportBody)
{
	Parent_::doComposePerformanceStats(reportBody);

	PString strValue;

	strValue.assign("").appendInt(loginAuthConn.isConnected());
	heLogger->compose(reportBody, "connection.loginAuthConn", strValue);
}

////////////////////////////////////////////////////////////////////////////////
