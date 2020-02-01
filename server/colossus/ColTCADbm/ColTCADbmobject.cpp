// ===========================================================
// Copyright (C) 2018 PYR Software Ltd. All rights reserved.
// ===========================================================

#include "TCADetector.h"
#include "ColTCADbmobject.h"
#include "AuthRedirector.h"
#include "admincommon.h"
#include "connUtils.h"
#include "../utils.h"
#include "coldbm.h"
#include "ColossusConstants.h"

/* static */ CommServerNullGuardFactory ColTCADbmServerObject::guardFactory;
/* static */ CommServerPlainTextPasswordSidGuardFactory ColTCADbmServerObject::authGuardFactory;
/* static */ CommClientNullGuardFactory ColTCADbmServerObject::clientNullGuardFactory;

class ColTCADataRequestCall : public CommClientGConnection::AsyncCall
{
	ColTCADbmServerObject* obj;
	UINT32 taskId;

public:
	ColTCADataRequestCall(ColTCADbmServerObject* obj_, UINT32 taskId_)
		: obj(obj_), taskId(taskId_)
	{}

	void processReply(UINT32 /*reqId*/, UINT32 msgId, const CommMsgBody& body) override
	{
		CommMsgParser parser(body);
		switch (msgId)
		{
		case COLRAW_A_GET_TCA_TOURN_PARTNERS:
			obj->gotColRawPairsData(parser, taskId);
			break;
		case COLRAW_A_GET_TCA_USER_TOURN_DATA:
			obj->gotColRawUserTournData(parser, taskId);
			break;
		case COLOSSUS_GET_UID_LETTERS_REPLY:
			obj->gotUserStartLetters(parser, taskId);
			break;
		default:
			PLog("ColTCADataRequestCall: Unknown msgId=%u", msgId);
			break;
		}
	}
};

ColTCADbmServerObject::ColTCADbmServerObject(_CommInterface& inter) : 
	DbmServerObjectCommon( inter ), 
	srvPool( inter ), cliPool( inter),
	connAdminFactory( authGuardFactory),
	connTrustedAdminFactory( guardFactory),
	connTCADetectorFactory( guardFactory, TCADetector),
	loginConnection(clientNullGuardFactory, "Login", 'L'),
	colRawConnection(clientNullGuardFactory, "ColRaw", 'R'),
	colossusReadConnection(clientNullGuardFactory, "ColRead", 'O'),
	heLoggerTimerHandle(0), heLogger(NULL),	useLogger(false),
	checkTaskTimerHandle(0), moreTrace(false)
{
	connAdminFactory.init(this);
	connTrustedAdminFactory.init(this);
	connTCADetectorFactory.init(this);

	loginConnection.init(this);
	colRawConnection.init(this);
	colossusReadConnection.init(this);
}

ColTCADbmServerObject::~ColTCADbmServerObject()
{
	if (heLogger)
		{
			stopHeLogger();
			delete heLogger;
			heLogger = NULL;
		}
}

bool ColTCADbmServerObject::registerAllFactories() // override
{
	if( !Parent::registerAllFactories() )
	{
		// Id generator not ready yet
		return false;
	}
	srvPool.registerFactory( TCA_ADMIN_CONNECTION_TYPE, connAdminFactory );
	srvPool.registerFactory( TCA_TRUSTEDADMIN_CONNECTION_TYPE, connTrustedAdminFactory );
	srvPool.registerFactory(TCA_DBM_CONNECTION_TYPE, connTCADetectorFactory);

	return true;
}

void ColTCADbmServerObject::init(CommMsgBody& initMsg)
{
	CommMsgParser parser(initMsg);
	const char* initDir_;
	parser.parseString(initDir_);

	if (!parser.parseEnded())
	{
		parser.parseStringP(loggerStationName);
		PLog("logger station name [%s] was provided at startup and will override correspondent logger's INI property", loggerStationName.c_str());
	}

	initDir.assign(initDir_);
	fullFileName.assign(initDir_);
	fullFileName.append("ColTCADbm.ini");
	PIniFile ini(fullFileName);

	dbManager.init( this );
	PString sectionName("MAINSTORAGE");
	if (!dbManager.openDataStorage(fullFileName, sectionName))
	{
		throw PError("ERROR: Main data storage not open");
	}

	dynamicInit();
	initGenerator( &dbManager, &srvPool, &cliPool, &clientNullGuardFactory, ini );
	registerAllFactories();

	if (checkTaskTimerHandle)
	{
		bool found = deleteTimerMessage(checkTaskTimerHandle);
		if (!found)
			PLog("ColTCADbmServerObject::init: cannot find/stop sweeper timer #%u", checkTaskTimerHandle);
		checkTaskTimerHandle = 0;
	}

	CommMsgBody timerBody;
	checkTaskTimerHandle = postTimerMessage(TCA_COL_TIMER_CHECK_TASK, timerBody, TIMER_CHECK_TASK_MSEC);
}

void ColTCADbmServerObject::dynamicInit()
{
	PIniFile ini(fullFileName);
	
	PIniFile::Section * pSection = ini.getSection("DYNAMICCONNECT");
	if( pSection )
	{
		const char* server = pSection->getProperty("LoginDbmServer");
		const char* object = pSection->getProperty("LoginDbmObject", "LoginDbmServerInstance");
		const char* connType = pSection->getProperty("LoginDbmConnectionType", COLTCA_LOGIN_CONNECTION_TYPE);
		reconnect(loginConnection, server, object, connType);

		server = pSection->getProperty("ColRawDbmServer");
		object = pSection->getProperty("ColRawDbmServerObject", "ColDbmInstance");
		connType = pSection->getProperty("ColRawDbmServerConnectionType", TCA_DBM_CONNECTION_TYPE);
		reconnect(colRawConnection, server, object, connType);

		server = pSection->getProperty("ColossusReadServer");
		object = pSection->getProperty("ColossusReadServerObject", "ColossusReadInstance");
		if (server && *server)
		{
			reconnect(colossusReadConnection, server, object, COLOSSUS_READ_CONNECTION_TYPE);
		}
	}
	else
	{
		PLog(LOG_TRACE_FAULTY_LOGIC ": Cannot read [DYNAMICCONNECT]");
		throw PError("dynamicInit - Cannot read [DYNAMICCONNECT]");
	}

	pSection = ini.getSection("DYNAMICSETTINGS");
	if( pSection )
	{
		moreTrace = pSection->getIntProperty("moreTrace", 0) != 0;
	}
	else
	{
		PTRACE("No DYNAMICSETTINGS configured");
	}

	PLog("-moreTrace=%s", moreTrace ? "T" : "F");

	pSection = ini.getSection("LOGGER");
	if (pSection)
	{
		useLogger = (0 != pSection->getIntProperty("useLogger", 0));
		const char* loggerServerAddress = pSection->getProperty("LoggerAddress", "");
		if (!loggerStationName.length())
		{
			loggerStationName = pSection->getProperty("LogStationName", "");
		}

		PLog("useLogger: %s", useLogger ? "YES" : "NO");
		PLog("loggerServerAddress: %s", loggerServerAddress);
		PLog("loggerStationName: %s", loggerStationName.c_str());

		if (useLogger && *loggerServerAddress && loggerStationName.length())
		{
			startHeLogger(loggerServerAddress, loggerStationName);
		}
		else
		{
			useLogger = false;
			PTRACE("No LOGGER started");
		}
	}
	else
	{
		useLogger = false;
		PTRACE("No LOGGER configured");
	}


	// PYR-27418
	Parent::dynamicInit( ini );
}

void ColTCADbmServerObject::reconnect(ColTCADbmOutgoingConnection& conn, const char* const serverName, const char* const serverObject, const char* const serverConnectionType)
{
	PLog("reconnect(serverName: %s, serverObject: %s, serverConnectionType: %s)"
		, (serverName && *serverName) ? serverName : "none"
		, (serverObject && *serverObject) ? serverObject : "none"
		, (serverConnectionType && *serverConnectionType) ? serverConnectionType : "none"
	);

	ConnUtils::createClientConnection(cliPool, conn, serverName, serverObject, serverConnectionType);

	// Based on assumption that isConnected() returns logical status, but not status of phisical connection let's check that it got 'connected'
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

void ColTCADbmServerObject::processColTCADbmServerMessage( UINT32 msgId, const CommMsgBody& body, ColTCADbmConnection* conn )
{
	ServerConnectionType connType = conn->getConnType();
	if (connType <= LowerGuard || connType >= UpperGuard)
	{
		throw PError((PString("Unsupported eServerConnection value: ").appendInt(connType)));
	}

	PLog("{%s %u %08x", ConnectionNames[connType], msgId, conn->id());

	HiresCounter counter;

	dbManager.setAutoCommitTrue();
	dbManager.resetRoundtripCounter();

	bool Ok = true;
	for (int n = 0; n < dbManager.getMaxDbReconnectAttempts(); n++)
	{
		try
		{
			if (Ok)
			{
				switch (connType)
				{
				case TrustedAdmin:
				{
					processTrustedAdminMessage(msgId, body, conn);
					break;
				}
				case Admin:
				{
					processAdminMessage(msgId, body, conn);
					break;
				}
				case TCADetector:
				{
					processTCAServerMessage(msgId, body, conn);
					break;
				}
				}

				break; // out of the loop
			}
		}

		catch (const PDbDisconnectError& er)
		{
			PLog("processColTCADbmServerMessage...Disconnect from database detected, reason: '%s', trying to reconnect, attempt #%d,", er.why(), n);
		}

		PThread::sleep(dbManager.getDbReconnectInterval());
		PString sectionName("MAINSTORAGE");
		if (!dbManager.openDataStorage(fullFileName, sectionName))
		{
			PLog("processColTCADbmServerMessage error ...attempt #%d failed", n);
			Ok = false;
		}
		else
		{
			PLog("processColTCADbmServerMessage...reconnect OK");
			Ok = true;
		}
	}

	counter.stop();
	UINT32 cnt = counter.getElapsedTick();
	UINT32 rt = dbManager.getRoundtripCounter();
	PLog("}%u rt=%u%s", cnt, rt, (cnt > TICKS_PER_SECOND) ? " overspan" : "");
	ticksAccumulator.addMessageTicks(counter);
}

void ColTCADbmServerObject::processCallback(UINT32 reqId, UINT32 msgId, const CommMsgBody& body, CommClientGConnection::AsyncCall* pCall)
{
	HiresCounter counter;
	PLog("{asy %u", msgId);

	pCall->processReply(reqId, msgId, body);

	counter.stop();
	UINT32 cnt = counter.getElapsedTick();
	PLog("}asy %04u %05u%s", msgId, cnt, (cnt > TICKS_PER_SECOND) ? " overspan" : "");
	ticksAccumulator.addMessageTicks(counter);
}

void ColTCADbmServerObject::postReply(ColTCADbmConnection* pConn, UINT32 msgId, CommMsgBody& body)
{
	if (pConn)
		pConn->post(msgId, body);
	else
		PLog("Message #%u not sent (no connection)", msgId);
}

void ColTCADbmServerObject::setReply(CommMsgBody& reply, INT16 errCode, const char* errDesc /*= ""*/)
{
	reply.composeINT16(errCode);

	if (errCode)
		reply.composeString(errDesc);
}

void ColTCADbmServerObject::processTCAServerMessage(UINT32 msgId, const CommMsgBody & body, ColTCADbmConnection* conn)
{
	CommMsgParser parser(body);
	switch (msgId)
	{
		case TCA_Q_GET_QUALIFIED_COL_PAIR:
		{
			processGetTCAUserTournData(parser, conn);
			break;
		}
		default:
		{
			PLog("ColTCADbmServerObject::processTCAServerMessage error ... unhandled message ID=%u", msgId);
			break;
		}
	}
}

void ColTCADbmServerObject::processGetTCAUserTournData(CommMsgParser& parser, CommServerConnection* conn)
{
	UINT32 reqId = 0;
	UINT32 taskId = 0;
	parser
		.parseUINT32(reqId)
		.parseUINT32(taskId);

	TCAConfig tcaConfig;
	tcaConfig.parse(parser);

	PLog("processGetTCAPairs - taskId=%u", taskId);

	tcaConfig.log();

	CommMsgBody reply;
	reply
		.composeUINT32(reqId);

	if (!tcaColTaskCache.add(taskId, tcaConfig))
	{
		reply
			.composeINT16(INTERNAL_LOGICAL_ERROR)
			.composeString("Task already exists");
		conn->post(TCA_A_GET_QUALIFIED_COL_PAIR, reply);
		return;
	}

	auto taskIt = tcaColTaskCache.container.find(taskId);
	if (taskIt == tcaColTaskCache.container.end())
	{
		PLog(LOG_TRACE_FAULTY_LOGIC " can not find taskId=%u", taskId);
		reply
			.composeINT16(INTERNAL_LOGICAL_ERROR)
			.composeString("Can not find task.");
		conn->post(TCA_A_GET_QUALIFIED_COL_PAIR, reply);
		return;
	}

	TCAColTaskData& tcaColTaskData = taskIt->second;
	tcaColTaskData.connId = conn->id();
	tcaColTaskData.reqId = reqId;

	try
	{
		PString sqlErr;
		INT16 errCode = 0;

		// 1. Get task extents(ROI and Partner)
		errCode = dbManager.getTCATaskExtent(tcaColTaskData, sqlErr);
		if (errCode)
		{
			CommMsgBody errReply;
			errReply
				.composeUINT32(reqId)
				.composeINT16(errCode);

			errReply.composeString(sqlErr);
			conn->post(TCA_A_GET_QUALIFIED_COL_PAIR, errReply);

			tcaColTaskCache.remove(taskId);
			return;
		}

		// 2. Get agg users total tourns 
		errCode = dbManager.getTCAUsersTournData(tcaColTaskData, sqlErr);
		if (errCode)
		{
			CommMsgBody errReply;
			errReply
				.composeUINT32(reqId)
				.composeINT16(errCode);

			errReply.composeString(sqlErr);
			conn->post(TCA_A_GET_QUALIFIED_COL_PAIR, errReply);

			tcaColTaskCache.remove(taskId);
			return;
		}

		PLog("got %u agg tourn users", (UINT32)tcaColTaskData.userTournDataMap.size());

		SrvDate tcaRequestEndDate;
		tcaRequestEndDate.currentLocalDate();
		ColUtils::addDays(tcaRequestEndDate, -tcaColTaskData.tcaConfig.daysEndOffset);

		if (tcaColTaskData.lastTournROIAggDate.equals(tcaRequestEndDate))
		{
			processGetTCAPairsRaw(taskId, tcaColTaskData);
			return;
		}

		// 3. Get raw users total tourns
		SrvDate lastColAggDate = tcaColTaskData.lastTournROIAggDate;
		ColUtils::addDays(lastColAggDate, 1);
		SrvTime fromTime, toTime;
		fromTime.decode(lastColAggDate.encode());
		toTime.decode(tcaRequestEndDate.encode());
		ColUtils::setEndOfDay(toTime);

		// request COLRAW data
		CommMsgBody msgBody;
		msgBody
			.composeSrvTime(fromTime)
			.composeSrvTime(toTime);
		tcaColTaskData.tcaConfig.compose(msgBody);
		postToColRaw(COLRAW_Q_GET_TCA_USER_TOURN_DATA, msgBody, new ColTCADataRequestCall(this, taskId));

	}
	catch (const PError& err)
	{
		// userTournDataMap may throw memory allocation error
		CommMsgBody errReply;
		errReply
			.composeUINT32(reqId)
			.composeINT16(DBM_INTERNAL_ERROR)
			.composeString(err.why());
		conn->post(TCA_A_GET_QUALIFIED_COL_PAIR, errReply);
		tcaColTaskCache.remove(taskId);
	}
}

void ColTCADbmServerObject::gotColRawUserTournData(CommMsgParser& parser, UINT32 taskId)
{
	PLog("gotColRawUserTournData taskId=%u", taskId);

	auto taskIt = tcaColTaskCache.container.find(taskId);
	if (taskIt == tcaColTaskCache.container.end())
	{
		PLog("Error - Task is not in cache. taskId=%u", taskId);
		return;
	}

	TCAColTaskData& tcaColTaskData = taskIt->second;

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	if (errCode)
	{
		const char* errStr;
		parser.parseString(errStr);
		PLog("DBM error[%d]-%s", errCode, errStr);

		CommMsgBody reply;
		reply
			.composeUINT32(tcaColTaskData.reqId)
			.composeINT16(errCode)
			.composeString(errStr);

		replyMsg(tcaColTaskData.connId, TCA_A_GET_QUALIFIED_COL_PAIR, reply);
		tcaColTaskCache.remove(taskId);

		return;
	}
	
	tcaColTaskData.userTournDataMap.parse(parser);
	PLog("got %u agg+raw tourn users", (UINT32)tcaColTaskData.userTournDataMap.size());

	processGetTCAPairsRaw(taskId, tcaColTaskData);
}

void ColTCADbmServerObject::processGetTCAPairsRaw(UINT32 taskId, TCAColTaskData& tcaColTaskData)
{
	PLog("processGetTCAPairsRaw taskId=%u", taskId);

	// 4. Get raw tourn partner pairs
	SrvDate tcaRequestEndDate;
	tcaRequestEndDate.currentLocalDate();
	ColUtils::addDays(tcaRequestEndDate, -tcaColTaskData.tcaConfig.daysEndOffset);

	if (tcaColTaskData.lastTournPartnerAggDate.equals(tcaRequestEndDate))
	{
		// No TournPartner raw data needed
		processGetUserStartLetters(taskId, tcaColTaskData);
		return;
	}

	//Raw data starts from next day of lastDate
	SrvDate lastColAggDate = tcaColTaskData.lastTournPartnerAggDate;
	ColUtils::addDays(lastColAggDate, 1);
	SrvTime fromTime, toTime;
	fromTime.decode(lastColAggDate.encode());
	toTime.decode(tcaRequestEndDate.encode());
	ColUtils::setEndOfDay(toTime);

	// request COLRAW data
	CommMsgBody msgBody;
	msgBody
		.composeSrvTime(fromTime)
		.composeSrvTime(toTime);
	tcaColTaskData.tcaConfig.compose(msgBody);
	postToColRaw(COLRAW_Q_GET_TCA_TOURN_PARTNERS, msgBody, new ColTCADataRequestCall(this, taskId));
}

void ColTCADbmServerObject::gotColRawPairsData(CommMsgParser& parser, UINT32 taskId)
{
	PLog("gotColRawPairsData taskId=%u", taskId);

	auto taskIt = tcaColTaskCache.container.find(taskId);
	if (taskIt == tcaColTaskCache.container.end())
	{
		PLog("Error - Task is not in cache. taskId=%u", taskId);
		return;
	}

	TCAColTaskData& tcaColTaskData = taskIt->second;

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	if (errCode)
	{
		const char* errStr;
		parser.parseString(errStr);
		PLog("DBM error[%d]-%s", errCode, errStr);

		CommMsgBody reply;
		reply
			.composeUINT32(tcaColTaskData.reqId)
			.composeINT16(errCode)
			.composeString(errStr);

		replyMsg(tcaColTaskData.connId, TCA_A_GET_QUALIFIED_COL_PAIR, reply);
		tcaColTaskCache.remove(taskId);
		return;
	}

	tcaColTaskData.colRawPairMap.parse(parser);
	PLog("col raw pair#=%u", (UINT32)tcaColTaskData.colRawPairMap.size());

	appendTaskBreakDownDetails("ColRaw", tcaColTaskData.colRawPairMap.size(), tcaColTaskData.taskDetails);

	processGetUserStartLetters(taskId, tcaColTaskData);
}

void ColTCADbmServerObject::processGetUserStartLetters(UINT32 taskId, TCAColTaskData& tcaColTaskData)
{
	PLog("processGetUserStartLetters taskId=%u", taskId);

	CommMsgBody msgBody;
	postToColossusRead(COLOSSUS_GET_UID_LETTERS, msgBody, new ColTCADataRequestCall(this, taskId));
}

void ColTCADbmServerObject::gotUserStartLetters(CommMsgParser& parser, UINT32 taskId)
{
	PLog("gotUserStartLetters taskId=%u", taskId);

	auto taskIt = tcaColTaskCache.container.find(taskId);
	if (taskIt == tcaColTaskCache.container.end())
	{
		PLog("Error - Task is not in cache. taskId=%u", taskId);
		return;
	}

	TCAColTaskData& tcaColTaskData = taskIt->second;

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	if (errCode)
	{
		const char* errStr;
		parser.parseString(errStr);
		PLog("DBM error[%d]-%s", errCode, errStr);

		CommMsgBody reply;
		reply
			.composeUINT32(tcaColTaskData.reqId)
			.composeINT16(errCode)
			.composeString(errStr);

		replyMsg(tcaColTaskData.connId, TCA_A_GET_QUALIFIED_COL_PAIR, reply);
		tcaColTaskCache.remove(taskId);

		return;
	}

	PString uidStartLetters;
	parser.parseStringP(uidStartLetters);

	PLog("- found %u userChars", (UINT32)uidStartLetters.length());

	UINT32 colAggPairCount = 0;
	PString sqlErr;

	for (UINT32 i = 0; i < uidStartLetters.length(); ++i)
	{
		char currStartLetter = uidStartLetters.charAt(i);

		// 5. Get agg tourn partner pairs
		errCode = dbManager.getTCAPairsByUidStartLetter(tcaColTaskData, currStartLetter, sqlErr);

		if (errCode)
		{
			CommMsgBody reply;
			reply
				.composeUINT32(tcaColTaskData.reqId)
				.composeINT16(errCode)
				.composeString(sqlErr);

			replyMsg(tcaColTaskData.connId, TCA_A_GET_QUALIFIED_COL_PAIR, reply);
			tcaColTaskCache.remove(taskId);
			return;
		}

		PLog("got %u agg pairs", (UINT32)tcaColTaskData.colAggPairMap.size());
		colAggPairCount += tcaColTaskData.colAggPairMap.size();

		processColTournPair(taskId, tcaColTaskData, currStartLetter);

		tcaColTaskData.colAggPairMap.clear();
	}

	appendTaskBreakDownDetails("Col", colAggPairCount, tcaColTaskData.taskDetails);
	PLog("Col agg pair size=%u", (UINT32)tcaColTaskData.tcaAlertMap.size());

	appendTaskBreakDownDetails("Rule8,9,10,11,12,13,14", tcaColTaskData.tcaAlertMap.size(), tcaColTaskData.taskDetails);
	PLog("Qualified Col pair size=%u", (UINT32)tcaColTaskData.tcaAlertMap.size());

	CommMsgBody reply;
	reply
		.composeUINT32(tcaColTaskData.reqId)
		.composeINT16(0)
		.composeString(tcaColTaskData.taskDetails);
	tcaColTaskData.tcaAlertMap.compose(reply);

	replyMsg(tcaColTaskData.connId, TCA_A_GET_QUALIFIED_COL_PAIR, reply);

	tcaColTaskCache.remove(taskId);
}

void ColTCADbmServerObject::processColTournPair(UINT32 taskId, TCAColTaskData& tcaColTaskData, char uidStartLetter)
{
	PLog("processColTournPair taskId=%u, startLetter=%c", taskId, uidStartLetter);

	// merge Col raw and agg pairs
	for (auto& rawIt : tcaColTaskData.colRawPairMap)
	{
		PString startLetter;
		startLetter.append(uidStartLetter);

		if (!rawIt.first.userId.startsWith(startLetter))
		{
			continue;
		}

		tcaColTaskData.colAggPairMap.insertPair(rawIt.first, rawIt.second);
	}

	TCAConfig& config = tcaColTaskData.tcaConfig;

	PLog("-checking Rule9");

	UserColDataMap	userColDataMap;
	auto colPairIt = tcaColTaskData.colAggPairMap.begin();
	while (colPairIt != tcaColTaskData.colAggPairMap.end())
	{
		// calculate each player's total tourn and all-in confrontation.
		userColDataMap.insertPlayerData(colPairIt->first, colPairIt->second, config.timesOfMedianTopNum);

		// RULE-9
		ColPairMap::iterator removeIt = colPairIt++;
		if (removeIt->second.totalTourns < config.pairQualifyingMin ||
			removeIt->second.totalTourns > config.pairQualifyingMax)
		{
			if (moreTrace)
			{
				PLog("[Rule9]-pair ('%s'-'%s'), pair tourn#=%u", removeIt->first.userId.c_str(), removeIt->first.partnerId.c_str(), removeIt->second.totalTourns);
			}
			tcaColTaskData.colAggPairMap.erase(removeIt);
		}
	}

	PLog("-checking Rule8,10,11,12,13,14");

	SrvDate sngStartDate(tcaColTaskData.startRunningTime);
	SrvDate sngToDate(tcaColTaskData.startRunningTime);
	ColUtils::addDays(sngStartDate, -tcaColTaskData.tcaConfig.daysStartOffset);
	ColUtils::addDays(sngToDate, -tcaColTaskData.tcaConfig.daysEndOffset);

	PString sngDateRange, dummy1, dummy2;
	sngDateRange.assign(srvDateToString(sngStartDate, dummy1)).append(" To ").append(srvDateToString(sngToDate, dummy2));

	for (auto& it : tcaColTaskData.colAggPairMap)
	{
		// Get player total tourn and all in data
		auto userIt = userColDataMap.find(it.first.userId);
		if (userIt == userColDataMap.end())
		{
			PLog("Error: can not find user pair data for ('%s')", it.first.userId.c_str());
			continue;
		}

		UINT32 userTotalTourns = 0;
		UINT32 partnerTotalTourns = 0;

		if (!tcaColTaskData.userTournDataMap.getUserTotalTourn(it.first.userId, userTotalTourns) ||
			!tcaColTaskData.userTournDataMap.getUserTotalTourn(it.first.partnerId, partnerTotalTourns))
		{
			PLog("Error: can not find user tourn data for ('%s'-'%s')", it.first.userId.c_str(), it.first.partnerId.c_str());
			continue;
		}

		ColPairData& pairData = it.second;
		UserColData& userData = userIt->second;

		TCAAlertData alertData;

		alertData.comment.assign("Config=").appendUint(tcaColTaskData.tcaConfig.configId);
		alertData.comment.append(", Player=").append(it.first.userId);

		alertData.comment.append(", SNG1=").appendUint(userTotalTourns);
		alertData.comment.append(", SNG2=").appendUint(partnerTotalTourns);
		alertData.comment.append(", Together=").appendUint(it.second.totalTourns);
		alertData.comment.append(", SNG Date=").append(sngDateRange);

		// RULE-8
		if (userTotalTourns < config.playerQualifyingMin ||
			userTotalTourns > config.playerQualifyingMax)
		{
			if (moreTrace)
			{
				PLog("[Rule8]-pair ('%s'-'%s'), user tourn#=%u", it.first.userId.c_str(), it.first.partnerId.c_str(), userTotalTourns);
			}
			continue;
		}

		// RULE-10
		UINT32 freqPartner = (UINT32)100 * 100 * pairData.totalTourns / userTotalTourns;
		if (freqPartner < config.freqPartnerMin_scaled100)
		{
			if (moreTrace)
			{
				PLog("[Rule10]-pair ('%s'-'%s'), user freq=%u", it.first.userId.c_str(), it.first.partnerId.c_str(), freqPartner);
			}
			continue;
		}

		alertData.comment.append(", Freq%=").appendDouble((double)freqPartner / 100.0);

		// RULE-11 optional
		UINT32 inversFreqPartner = 0;
		if (config.inverseFreqPartnerMin_scaled100 != TCA_INVALID_RULE_VALUE)
		{
			inversFreqPartner = (UINT32)100 * 100 * pairData.totalTourns / partnerTotalTourns;
			if (inversFreqPartner < config.inverseFreqPartnerMin_scaled100)
			{
				if (moreTrace)
				{
					PLog("[Rule11]-pair ('%s'-'%s'), invers freq=%u", it.first.userId.c_str(), it.first.partnerId.c_str(), inversFreqPartner);
				}

				continue;
			}
			alertData.comment.append(", Inv.Freq%=").appendDouble((double)inversFreqPartner / 100.0);
		}

		// RULE-12 optional
		UINT32 overallFreq = 0;
		if (config.overallFreqMin_scaled100 != TCA_INVALID_RULE_VALUE)
		{
			overallFreq = (UINT32)100 * 100 * pairData.totalTourns
				/ (userTotalTourns + partnerTotalTourns - pairData.totalTourns);
			if (overallFreq < config.overallFreqMin_scaled100)
			{
				if (moreTrace)
				{
					PLog("[Rule12]-pair ('%s'-'%s'), ovr freq=%u", it.first.userId.c_str(), it.first.partnerId.c_str(), overallFreq);
				}

				continue;
			}
			alertData.comment.append(", Ovr.Freq%=").appendDouble((double)overallFreq / 100.0);
		}

		// RULE-13 optional
		UINT32 medianTimes = 0;
		if (config.timesOfMedian_scaled100 != TCA_INVALID_RULE_VALUE)
		{
			double median = userData.topPartnerSet.getMedian();
			if (median != 0)
			{
				medianTimes = (UINT32)100 * pairData.totalTourns / median;
				if (medianTimes <  config.timesOfMedian_scaled100)
				{
					if (moreTrace)
					{
						PLog("[Rule13]-pair ('%s'-'%s'), medianTimes=%u", it.first.userId.c_str(), it.first.partnerId.c_str(), medianTimes);
					}
					continue;
				}
			}
			alertData.comment.append(", timeMedian=").appendDouble((double)medianTimes / 100.0);
		}

		// RULE-14-a
		UINT32 AICRwithPartner = 0;
		if (config.AICRQualifyingMin != TCA_INVALID_RULE_VALUE)
		{
			if (userTotalTourns < config.AICRQualifyingMin)
			{
				continue;
			}

			if (pairData.totalTourns > 0)
			{
				AICRwithPartner = (UINT32)100 * pairData.numAllInConfrontations / pairData.totalTourns;
			}

			UINT32 AICRwithField = 100;
			UINT32 numPairTotalTourns = userData.numPairTotalTourns;
			if ((numPairTotalTourns - pairData.totalTourns) != 0)
			{
				AICRwithField = (UINT32)100 * (userData.numAllInConfrontations - pairData.numAllInConfrontations)
					/ (numPairTotalTourns - pairData.totalTourns);
			}

			if (AICRwithPartner != 0 && AICRwithField != 0)
			{
				// RULE14-b,c
				if ((UINT32)100 * (AICRwithField / AICRwithPartner) < config.AICRField_scaled100 &&
					(UINT32)100 * (AICRwithPartner / AICRwithField) < config.AICRPartner_scaled100)
				{
					if (moreTrace)
					{
						PLog("[Rule14]-pair ('%s'-'%s'), AICRwithField=%u, AICRwithPartner=%u", it.first.userId.c_str(), it.first.partnerId.c_str(), AICRwithField, AICRwithPartner);
					}

					continue;
				}
			}
			else if (AICRwithPartner == 0)
			{
				continue;
			}

			alertData.comment.append(", AICR w/P=").appendUint(AICRwithPartner);
			alertData.comment.append(", AICR w/F=").appendUint(AICRwithField);
		}

		alertData.freq_scaled100 = freqPartner;
		alertData.inverseFreq_scaled100 = inversFreqPartner;
		alertData.overallFreq_scaled100 = overallFreq;
		alertData.medianTimes = medianTimes;
		alertData.aicrPartner_scaled100 = 100 * AICRwithPartner;

		tcaColTaskData.tcaAlertMap.insert(TCAAlertDataMap::value_type(it.first, alertData));

		if (moreTrace)
		{
			PLog("[COL]+pair ('%s'-'%s')", it.first.userId.c_str(), it.first.partnerId.c_str());
		}
	}
}

void ColTCADbmServerObject::appendTaskBreakDownDetails(const char* module, UINT32 pairNum, PString& taskDetails)
{
	SrvTime now;
	now.currentLocalTime();

	PString dummy;
	taskDetails.append("[").append(srvTimeToString(now, dummy)).append("]")
		.append(module).append("-").appendUint(pairNum).append(";\n");
}

void ColTCADbmServerObject::replyMsg(UINT32 connId, UINT32 msgId, CommMsgBody& body)
{
	if (!connId)
	{
		PLog("Warning - Message #%u failed to send, no connId.", msgId);
		return;
	}

	CommServerConnection* conn = srvPool.findConnection(connId);
	if (!conn)
	{
		PLog("Warning - Message #%u failed to send, lost connection.", msgId);
		return;
	}

	conn->post(msgId, body);
}

void ColTCADbmServerObject::processClientGMessage( UINT32 requestId, UINT32 msgId, const CommMsgBody & body )
{
	switch( msgId )
	{
	case 1:
		{
			CommMsgParser parser( body );
			break;
		}
	}
}

void ColTCADbmServerObject::processClientMessage( UINT32 msgId, const CommMsgBody & body )
{
	switch( msgId )
	{
	case 1:
		{
			CommMsgParser parser( body );
			UINT reqId;
			parser.parseUINT32(reqId);
			break;
		}
	}
}


void ColTCADbmServerObject::processClientAuthMessage( UINT32 requestId, UINT32 msgId, const CommMsgBody & body)
{
	switch( msgId )
	{
	case 1:
		{
			CommMsgParser parser( body );
			UINT reqId;
			parser.parseUINT32(reqId);
			break;
		}
	}
}

//
// HeLogger and performance reporting
//

void ColTCADbmServerObject::startHeLogger(const char * strServerAddress, const char * strStationName )
{
	PLog("ColTCADbmServerObject::startHeLogger()");
	PLog("ServerAddress - %s", strServerAddress);
	PLog("StationName - %s", strStationName);

	if (heLogger)
	{
		stopHeLogger();
		delete heLogger;
		heLogger = NULL;
	}

	if( !heLogger )
	{
		heLogger = new HeLogger;
		heLogger->init( cliPool, strServerAddress, strStationName, HE_LOGGER_LOCAL_TIME_ANSWERING );
	}

	if( !heLoggerTimerHandle )
	{
		CommMsgBody timerBody;
		heLoggerTimerHandle = postTimerMessage( TCA_COL_TIMER_LOGGER, timerBody, TIMER_LOGGER_MSEC );
	}
}

void ColTCADbmServerObject::stopHeLogger()
{
	PLog("ColTCADbmServerObject::stopHeLogger()");

	if( heLoggerTimerHandle )
	{
		deleteTimerMessage(heLoggerTimerHandle);
		heLoggerTimerHandle = 0;
	}
}

void ColTCADbmServerObject::reportPerformance()
{
	PTRACE("ColTCADbmServerObject::reportPerformance()");

	if( useLogger )
	{
		try
		{
			ticksAccumulator.stop();

			CommMsgBody reportBody;
			PString strValue;

			UINT32 messagesPerSec = ticksAccumulator.messagesPerSec();
			strValue.assign("").appendInt(messagesPerSec);
			heLogger->compose (reportBody, "messagesPerSec", strValue);

			UINT32 avgProcessingMsec = ticksAccumulator.avgProcessingMkSec();
			strValue.assign("").appendInt(avgProcessingMsec);
			heLogger->compose (reportBody, "avgProcessingMksec", strValue);
			strValue.assign("").appendInt(ticksAccumulator.avgProcessingMkSecTick());
			heLogger->compose (reportBody, "avgProcessingMksecTick", strValue);

			strValue.assign("").appendInt(loginConnection.getConnected());
			heLogger->compose(reportBody, "connection.login", strValue);

			strValue.assign("").appendInt(colRawConnection.getConnected());
			heLogger->compose(reportBody, "connection.colraw", strValue);

			strValue.assign("").appendInt(colossusReadConnection.getConnected());
			heLogger->compose(reportBody, "connection.colossusread", strValue);

			UINT32 loadPercentCnt = ticksAccumulator.loadPercentCntTick();
			strValue.assign("").appendInt(loadPercentCnt);
			heLogger->compose (reportBody, "loadPercentCounter", strValue);

			ticksAccumulator.reset();

			HeLoggerParameters dbManagerParameters;
			dbManager.composeHeLoggerParameters(dbManagerParameters);
			for( HeLoggerParameters::const_iterator it=dbManagerParameters.begin(); it!=dbManagerParameters.end(); ++it )
			{
				heLogger->compose (reportBody, it->first, it->second);
			}

			heLogger->post(reportBody);
		}
		catch( const PError& er )
		{
			PLog( "ColTCADbmServerObject::reportPerformance() error: '%s'", er.why() );
		}
	}
}

//
// Timer message processing
//

void ColTCADbmServerObject::processTimerMessage( UINT32 /*handle*/, UINT32 msgId, CommMsgBody& body )
{
	UINT32 ticks = SYSTEMTICKS();
	PTRACE("{timer Q=%d msgId=%d", getInQueueSize(), msgId );

	CommMsgParser parser(body);

	switch( msgId )
	{
	case TCA_COL_TIMER_LOGGER:
		{
			CommMsgBody timerBody;
			heLoggerTimerHandle = postTimerMessage( TCA_COL_TIMER_LOGGER, timerBody, TIMER_LOGGER_MSEC );
			reportPerformance();
			break;
		}
	case TCA_COL_TIMER_CHECK_TASK:
		{
			CommMsgBody timerBody;
			checkTaskTimerHandle = postTimerMessage(TCA_COL_TIMER_CHECK_TASK, timerBody, TIMER_CHECK_TASK_MSEC);
			checkRunningTasks();
			break;
		}
	}
	ticks = SYSTEMTICKS() - ticks;
	PTRACE("}%04d %05d%s", msgId, ticks, ( ticks > TICKS_PER_SECOND ) ? " overspan" : "");
}

void ColTCADbmServerObject::checkRunningTasks()
{
	PLog("processScheduleTimer - running task#=%u", (UINT32)tcaColTaskCache.container.size());

	auto it = tcaColTaskCache.container.begin();
	while (it != tcaColTaskCache.container.end())
	{
		auto removeIt = it++;
		UINT32 taskId = removeIt->first;
		TCAColTaskData& tcaColTaskData = removeIt->second;
		if (tcaColTaskData.isTaskRunOverTime())
		{
			CommMsgBody reply;
			reply
				.composeUINT32(tcaColTaskData.reqId)
				.composeINT16(TCA_TASK_TIMEOUT)
				.composeString(TCA_REPORT_TASK_TIMEOUT);

			replyMsg(tcaColTaskData.connId, TCA_A_GET_QUALIFIED_COL_PAIR, reply);

			PLog("Task run over time. Removed taskId=%u", taskId);
			tcaColTaskCache.container.erase(removeIt);
		}
	}
}

/*virtual*/ void ColTCADbmServerObject::shutdown() // override
{
	if(heLogger)
	{
		stopHeLogger();
		delete heLogger;
		heLogger = NULL;
	}
}



