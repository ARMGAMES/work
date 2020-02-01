////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2017 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "ColSrvObject.h"
#include "msguserauth.h"
#include "getservconnip.h"
#include "colutilserver.h"
#include "connUtils.h"
#include "admincommon.h"
#include "AuthRedirector.h"
#include "coldbm.h"
#include "HandStats.h"
#include "ColossusConstants.h"
#include "colshd.h"
#include "conndefines.h"
#include "templateserver.h"
#include "../shared/AccumulatorShared.h"

/* static */ CommClientNullGuardFactory ColUtilServerObject::clientNullGuardFactory;
/* static */ CommServerPlainTextPasswordSidGuardFactory ColUtilServerObject::authGuardFactory;
/* static */ CommServerNullGuardFactory ColUtilServerObject::guardFactory;

/* static */ UINT32 GetUserTotalRakePaidAsyncCall::UNIQUE_ID = 0;

static const UINT32 DEFAULT_MAX_IN_QUEUE_FAST_REPORTS = 10;
static const UINT32 DEFAULT_IN_PROCESS_FAST_REPORTS = 5;
static const UINT32 DEFAULT_FAST_REPORT_TIMEOUT_SECONDS = 9 * 60; // Nine minutes

static void postReply(CommServerConnection* pConn, UINT32 msgId, CommMsgBody& body)
{
	if (pConn)
	{
		pConn->post(msgId, body);
	}
	else
	{
		PLog("Message #%u not sent (no connection)", msgId);
	}
}

static void setReply(CommMsgBody& reply, INT16 errCode, const char* errDesc)
{
	reply.composeINT16(errCode);

	if (errCode)
	{
		reply.composeString(errDesc);
	}
}

static bool checkAdminRight(const char* right, const ColUtilSrvPrivilegedConnection* pConn, CommMsgBody& reply)
{
	if (pConn->checkAdminRight(right))
	{
		return true;
	}

	setReply(reply, NOT_ALLOWED, "Insufficient rights");
	return false;
}

////////////////////////////////////////////////////////////////////////////////

void ColUtilServerConnection::processMessage(UINT32 msgId, const CommMsgBody& body)
{
	obj.processCommonServerMessage(msgId, body, connectionType, this);
}

void AdminSrvCon::connected()
{
	ColUtilSrvPrivilegedConnection::connected();

	PLog("Admin %s connected to '%s'", adminId.c_str(), getServerConnType());

	CommMsgBody request;
	composeLoginConfirmMsg(adminId, request);
	obj.postToLoginAuth(DBM_Q_LOGIN_CONFIRM, request);
}

void AdminSrvCon::closedOrDisconnected(int errCode, const char* errMsg)
{
	ColUtilSrvPrivilegedConnection::closedOrDisconnected(errCode, errMsg);

	PLog("Admin %s disconnected from '%s': %d '%s'", adminId.c_str(), getServerConnType(), errCode, errMsg);

	if (adminId.length() > 0 && !authError)
	{
		// Translate specific codes
		if (errCode == _COMM_ERROR_PHYSICAL_DISCONNECTED)
		{
			errCode = LOGOUT_DISCONNECTED;
		}

		CommMsgBody body;
		composeUserLogout(_OBJ_TYPE_COLUTIL_SERVER, 0, adminId, errCode, body);
		obj.postToLoginAuth(DBM_Q_USER_LOGOUT, body);
	}
}

void AdminSrvCon::getUserAuth(const char* guardType, const char* user, CommMsgBody& clientExtra, CommMsgBody& authBody)
{
	if(!strcmp(guardType, _COMM_NULL_GUARD_NAME)) //PYR-44258
	{
		gotUserAuth(0);
		return;
	}

	adminId.assign(user);

	PString ipAddr;
	int port;
	getServConnIp(*this, ipAddr, port);

	CommMsgBody request;
	composeUserAuthMsg(user, "", "", "", ipAddr, port, _OBJ_TYPE_COLUTIL_SERVER, 0, authBody, 0, 0, guardType, request);

	obj.postToLoginAuth(DBM_Q_USER_AUTH, request, new AdminAuthAsyncCall(obj, this));
}

void TrustedAdminSrvConn::parseInfo(CommMsgParser& parser)
{
	UINT32 size;
	parser.
		parseStringP(adminId).
		parseUINT32(userIntId).
		parseUINT32(size)
		;

	for (UINT32 i = 0; i < size; ++i)
	{
		char const* right;
		parser.parseString(right);
		adminRights.insert(right);
	}
}

void ColUtilSrvCliGConn::connected()
{
	CommClientGConnection::connected();

	isconnected = 1;

	PLog("Outgoing connection to %s connected", name.c_str());
}

bool ColUtilSrvCliGConn::closedOrDisconnected(int errCode, const char* errMsg)
{
	bool res = CommClientGConnection::closedOrDisconnected(errCode, errMsg);

	isconnected = 0;

	PLog("Outgoing connection disconnected from %s, err=%d (%s), reconnect=%s",
		name.c_str(), errCode, errMsg, res ? "no" : "yes");
	
	return res;
}

UINT32 ColUtilSrvCliGConn::post(UINT32 msgId, CommMsgBody& body, AsyncCall* call)
{
	UINT32 reqId = CommClientGConnection::post(msgId, body, call);
	PLog(">%s %u %08X [%08X]%s", getName(), msgId, getConnId(), reqId, call ? "" : " (no AsyncCall)");
	return reqId;
} 

void ColUtilSrvCliGConn::processGMessage(UINT32 reqId, UINT32 msgId, const CommMsgBody& body, AsyncCall* call)
{
	PLog("<<%s %u [%08X]%s", name.c_str(), msgId, reqId, call ? "" : " (no AsyncCall)");

	if (call)
	{
		obj.processCallback(reqId, msgId, body, call);
	}
	else
	{
		PLog("No AsyncCall, message not handled");
	}
}

UINT32 ColUtilSrvCliGConn::getConnId() const
{
	return underlyingConnection().id();
}

////////////////////////////////////////////////////////////////////////////////

// PYR-81917
void ColCdhCliGConn::connected()
{
	Parent_::connected();

	obj.processGetCurCdhWhitelist();
}

////////////////////////////////////////////////////////////////////////////////

void AdminAuthAsyncCall::processReply(UINT32 requestId, UINT32 msgId, const CommMsgBody& body)
{
	PLog("<<ADMIN connId=%08X", connId);

	AdminSrvCon* pConnection = dynamic_cast<AdminSrvCon*>(findConnection());
	if (pConnection)
	{
		pObj->receiveAdminAuth(body, pConnection);
	}
	else
	{
		PLog("Warning: admin dropped connection");
	}
}

////////////////////////////////////////////////////////////////////////////////

// PYR-64045
ConvRateAsyncCall::ConvRateAsyncCall(ColUtilServerObject& obj_)
	: obj(obj_)
{

}

// PYR-64045
void ConvRateAsyncCall::processReply(UINT32 requestId, UINT32 replyId, const CommMsgBody& body)
{
	CommMsgParser parser(body);

	switch (replyId)
	{
	case COLOSSUS_GET_CONVRATES_TO_USD_REPLY:
		{
			obj.processConvRates(parser);
			break;
		}
	default:
		{
			PLog("Error: Unknown reply id type %u. Reply not handled.", replyId);
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

void ColUtilServerObject::init(CommMsgBody& initMsg)
{
	CommMsgParser parser(initMsg);
	const char* initDir_;
	parser.parseString(initDir_);
	initDir.assign(initDir_);
	fullFileName.assign(initDir_);
	fullFileName.append("colutilsrv.ini");
	PIniFile ini(fullFileName);

	if (!parser.parseEnded())
	{
		parser.parseStringP(loggerStationName);
		PLog("logger station name [%s] was provided at startup and will override correspondent logger's INI property", loggerStationName.c_str());
	}

	dynamicInit();

	srvPool.registerFactory(COLUTILSRV_INTERNAL_CONNTYPE, connInternalFactory);
	srvPool.registerFactory(COLUTILSRV_WEB_CONNTYPE, connAdminFactory);
	srvPool.registerFactory(COLUTILSRV_ADMIN_CONNTYPE, connAdminFactory);
	srvPool.registerFactory(COLUTILSRV_TRUSTEDADMIN_CONNTYPE, connTrustedAdminFactory);

	CommMsgBody emptyBody;
	postTimerMessage(RPT_CACHES_TIMER, emptyBody, TIMER_RPT_CACHES_MSEC);
	postTimerMessage(CONV_RATE_TIMER, emptyBody, 1);
}

void ColUtilServerObject::shutdown()
{
	if (useLogger)
	{
		stopHeLogger();
		delete heLogger;
	}
}

void ColUtilServerObject::processCommonServerMessage(UINT32 msgId, const CommMsgBody& body, ServerConnectionType srvConnType, CommServerConnection* conn)
{
	if (srvConnType <= LowerGuard || srvConnType >= UpperGuard)
	{
		PString buf("Unsupported eServerConnection value: ");
		buf.appendInt(srvConnType);
		throw PError(buf);
	}

	static const char* ConnName[] = { "", "A", "TA", "INT" };
	PLog("{%s %u %08x", ConnName[srvConnType], msgId, conn->id());

	HiresCounter counter;

	switch (srvConnType)
	{
		case Admin:
		{
			AdminSrvCon* conn_ = dynamic_cast<AdminSrvCon*>(conn);
			processAdminMessage(msgId, body, conn_);

			break;
		}
		case TrustedAdmin:
		{
			TrustedAdminSrvConn* conn_ = dynamic_cast<TrustedAdminSrvConn*>(conn);
			processTrustedAdminMessage(msgId, body, conn_);

			break;
		}
		case Internal:
		{
			processInternalMessage(msgId, body, conn);
			break;
		}
		default:
		{
			PLog("Error: Unknown connection type %d. Message unhandled.", srvConnType);
			break;
		}
	}

	counter.stop();
	UINT32 ticks = counter.getElapsedTick();
	ticksAccumulator.addMessageTicks(counter);
	PTRACE("}%d%s", ticks, (ticks > TICKS_PER_SECOND) ? " overspan" : "");
}

void ColUtilServerObject::processMessage(_CommMsg& msg)
{
	srvPool._processMessage(msg) || cliPool._processMessage(msg);
}

void ColUtilServerObject::processCallback(UINT32 reqId, UINT32 msgId, const CommMsgBody& body, CommClientGConnection::AsyncCall* pCall)
{
	HiresCounter counter;

	PLog("{ASC %u", msgId);

	pCall->processReply(reqId, msgId, body);

	counter.stop();
	UINT32 ticks = counter.getElapsedTick();
	ticksAccumulator.addMessageTicks(counter);

	PLog("}%u%s", ticks, (ticks > TICKS_PER_SECOND) ? " overspan" : "");
}

void ColUtilServerObject::receiveAdminAuth(const CommMsgBody& body, AdminSrvCon* pConn)
{
	CommMsgParser parser(body);

	INT16 errCode;
	parser.parseINT16(errCode);
	pConn->saveAuthError(errCode);

	if (errCode)
	{
		char const* errDescr;
		parser.parseString(errDescr);
		pConn->gotUserAuth(0, errCode, errDescr);

		PLog("ColUtilServerObject::receiveAdminAuth error %d (%s)", errCode, errDescr);
		return;
	}

	CommMsgBody msg;
	composeUserLogout(_OBJ_TYPE_COLUTIL_SERVER, 0, pConn->getAdminId(), LOGOUT_DISCONNECTED, msg);
	pConn->getColUtilServerObject().postToLoginAuth(DBM_Q_USER_LOGOUT, msg);

	// Some information in the authentication message is not needed
	UINT32 dummyUint32;
	char const* pDummyStr;
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
		parseUINT16(numRights)
		;

	// Extract and store rights
	for (UINT32 i = 0; i < numRights; ++i)
	{
		const char* right;
		parser.parseString(right);
		pConn->addAdminRight(right);
		if (moreTrace)
		{
			if (PString::compare(right, MANAGE_CCDA_CONTROL) == 0)
			{
				PLog("+ar[%s]", right);
			}
		}
	}

	// Skip the rest of the message as it is not needed.

	pConn->setUserIntId(userIntId);

	CommMsgParser authParser(authBody);
	if (!pConn->gotUserAuth(&authParser))
	{
		PLog("Authentication failed");
	}
}

// PYR-64045
void ColUtilServerObject::insertAmpsShdHandsCache(const ColAmps::GetShdHandsInputs& inputs, const ColAmps::GetShdHandsOutputs& outputs)
{
	reportCaches.putIfAbsent(inputs, outputs);
}

// PYR-64045
void ColUtilServerObject::insertAmpsTopSngPartnersCache(const ColAmps::TopPartnersInputs& inputs, const ColAmps::TopSngPartnersCachedOutputs& outputs)
{
	reportCaches.putIfAbsent(inputs, outputs);
}

// PYR-64045
void ColUtilServerObject::insertAmpsTopRingPartnersCache(const ColAmps::TopPartnersInputs& inputs, const ColAmps::TopRingPartnersCachedOutputs& outputs)
{
	reportCaches.putIfAbsent(inputs, outputs);
}

// PYR-75383
void ColUtilServerObject::insertAmpsTopSngPartnersRootUserCache(const ColAmps::TopPartnersInputs& inputs, const ColAmps::TopSngPartners_RootUser& outputs)
{
	reportCaches.putIfAbsent(inputs, outputs);
}

// PYR-75383
void ColUtilServerObject::insertAmpsTopRingPartnersRootUserCache(const ColAmps::TopPartnersInputs & inputs, const ColAmps::TopRingPartners_RootUser & outputs)
{
	reportCaches.putIfAbsent(inputs, outputs);
}

// PYR-64045
void ColUtilServerObject::insertAmpsSngGameActivityCache(const ColAmps::GameActivityInputs& inputs, const ColAmps::SngGameActivityCachedOutputs& outputs)
{
	reportCaches.putIfAbsent(inputs, outputs);
}

// PYR-64045
void ColUtilServerObject::insertAmpsRingGameActivityCache(const ColAmps::GameActivityInputs& inputs, const ColAmps::RingGameActivityCachedOutputs& outputs)
{
	reportCaches.putIfAbsent(inputs, outputs);
}

// PYR-64045
void ColUtilServerObject::insertAmpsSngGamesTogetherCache(const ColAmps::GamesTogetherInputs& inputs, const ColAmps::SngGamesTogetherOutputs& outputs)
{
	reportCaches.putIfAbsent(inputs, outputs);
}

// PYR-64045
void ColUtilServerObject::insertAmpsRingGamesTogetherCache(const ColAmps::GamesTogetherInputs& inputs, const ColAmps::RingGamesTogetherOutputs& outputs)
{
	reportCaches.putIfAbsent(inputs, outputs);
}

// PYR-64045
void ColUtilServerObject::processConvRates(CommMsgParser& parser)
{
	PLog("processConvRates");

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	if (errCode)
	{
		const char* errStr;
		parser.parseString(errStr);
		PLog("Error: DBM error[%d]-%s", errCode, errStr);
		return;
	}

	parseMidConvRatesVector(currencies, parser);
	PLog("processConvRates (%d)", (int)currencies.size());
}

// PYR-64045
INT64 ColUtilServerObject::convertCurrencyM(INT64 amtSrc, const char* curSrc, const char* curDst) const
{
	return ::convertCurrency64(amtSrc, curSrc, curDst, currencies);
}

////////////////////////////////////////////////////////////////////////////////

void ColUtilServerObject::processGetShdFreqPartnersReport(UINT32 reqId, CommMsgParser& parser, CommServerConnection* pConn)
{
	time_t now = time(nullptr);

	AdminSrvCon* adminConn = dynamic_cast<AdminSrvCon*>(pConn);
	if (adminConn == nullptr)
	{
		PLog("ColUtilServerObject::processGetShdFreqPartnersReport - lost admin connection, aborting");
		return;
	}

	ShdFreqPartnersInputs inputs(parser);
	inputs.timeoutTime = now + shdFreqPartnersReportTimeoutSeconds;
	
	unique_ptr<FastReportItemBase> reportItem = make_unique<ColShdFreqPartnersFastReport>(reqId, adminConn->id(), COLSRV_A_SHD_FREQPARTNERS_REPORT, adminConn->getAdminId(), *this, inputs);
	
	bool res = fastReportQueue.pushBack(move(reportItem));
	if (!res)
	{
		PLog("ColUtilServerObject::processGetShdFreqPartnersReport - queue full, aborting");
		
		CommMsgBody reply;
		reply.composeUINT32(reqId).composeINT16(TOO_MANY_ITEMS).composeString("Queue full");
		adminConn->post(COLSRV_A_SHD_FREQPARTNERS_REPORT, reply);

		return; // No more processing
	}

	res = fastReportQueue.submitNext();
	if (!res)
	{
		PLog("ColUtilServerObject::processGetShdFreqPartnersReport - max submissions reached, waiting");
	}
}

void ColUtilServerObject::processGetShdFreqPartnersReport_Submitted(const ColShdFreqPartnersFastReport& reportItem)
{
	PLog("processGetShdFreqPartnersReport_Submitted");
	reportItem.log();

	CommMsgBody body;
	reportItem.compose(body);
	postToColReportingRaw(COLRAW_Q_GET_SHDFREQPARTNERS, body, 
		new ShdFreqPartnersAsyncCall(this, reportItem.connId, reportItem.reqId, reportItem.replyId, &ColUtilServerObject::processGetShdFreqPartnersReport_GotPPSData, reportItem));
}

void ColUtilServerObject::processGetShdFreqPartnersReport_GotPPSData(CommMsgParser& parser, ShdFreqPartnersAsyncCall* pCall)
{
	PLog("processGetShdFreqPartnersReport_GotPPSData");
	
	ColShdFreqPartnersFastReport& reportState = pCall->data;
	reportState.log();

	fastReportQueue.reportJobComplete(reportState.uniqueId);

	INT16 errCode = 0;
	parser.parseINT16(errCode);
	
	if (errCode != DBM_NO_ERROR)
	{
		const char* errBuf;
		parser.parseString(errBuf);

		PLog("Error: %s", errBuf);
		pCall->postReply(errCode, errBuf);

		return;	// No more processing
	}
	
	CommMsgBody body;
	body.copyFrom(parser.getMsgBody()); // Copy the whole message body, not just unparsed part

	/* 
	Leave this comment here.  This details what we expect to pass back to the incoming connection.
	body.composeINT16(errCode);
	vector<ShdAggPPSRecord> ppsRecords;
	composeVector(body, ppsRecords);
	*/

	pCall->postReply(body);
}

////////////////////////////////////////////////////////////////////////////////

// PYR-55192
void ColUtilServerObject::processGetStatsForHand(UINT32 reqId, CommMsgParser& parser, CommServerConnection* pConn)
{
	time_t now = time(nullptr);

	AdminSrvCon* adminConn = dynamic_cast<AdminSrvCon*>(pConn);
	if (adminConn == nullptr)
	{
		PLog("ColUtilServerObject::processGetStatsForHand - lost admin connection, aborting");
		return;
	}

	GetHandStatsInputs inputs(parser);
	inputs.timeoutTime = now + getHandStatsReportTimeoutSeconds;

	unique_ptr<FastReportItemBase> reportItem = make_unique<ColGetHandStatsFastReport>(reqId, adminConn->id(), COLSRV_A_STATS_FOR_HAND, adminConn->getAdminId(), *this, inputs);

	bool res = fastReportQueue.pushBack(move(reportItem));
	if (!res)
	{
		PLog("ColUtilServerObject::processGetStatsForHand - queue full, aborting");

		CommMsgBody reply;
		reply.composeUINT32(reqId).composeINT16(TOO_MANY_ITEMS).composeString("Queue full");
		adminConn->post(COLSRV_A_STATS_FOR_HAND, reply);

		return; // No more processing
	}

	res = fastReportQueue.submitNext();
	if (!res)
	{
		PLog("ColUtilServerObject::processGetStatsForHand - max submissions reached, waiting");
	}
}

// PYR-55192
void ColUtilServerObject::processGetStatsForHand_Submitted(const ColGetHandStatsFastReport& reportItem)
{
	PLog("processGetStatsForHand_Submitted");
	reportItem.log();

	CommMsgBody body;
	reportItem.compose(body);
	postToColReader(COLOSSUS_GET_HAND_STATS, body, 
		new GetHandStatsAsyncCall(this, reportItem.connId, reportItem.reqId, reportItem.replyId, &ColUtilServerObject::processGetStatsForHand_GotData, reportItem));
}

// PYR-55192
void ColUtilServerObject::processGetStatsForHand_GotData(CommMsgParser& parser, GetHandStatsAsyncCall* pCall)
{
	PLog("processGetStatsForHand_GotData");

	ColGetHandStatsFastReport& reportState = pCall->data;
	reportState.log();

	fastReportQueue.reportJobComplete(reportState.uniqueId);

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	if (errCode != DBM_NO_ERROR)
	{
		const char* errBuf;
		parser.parseString(errBuf);

		PLog("Error: %s", errBuf);
		pCall->postReply(errCode, errBuf);

		return;	// No more processing
	}

	CommMsgBody body;
	body.copyFrom(parser.getMsgBody()); // Copy the whole message body, not just unparsed part

	/* 
	Leave this comment here.  This details what we expect to pass back to the incoming connection.

	UINT32 num = calcHandResults.rawHands.size();
	reply.
		composeUINT16(DBM_NO_ERROR).
		composeUINT32(num);

	for (UINT32 i = 0; i < num; ++i)
	{
		calcHandResults.rawHands[i].compose(reply);
	}

	calcHandResults.winLoss.compose(reply);	
	*/

	pCall->postReply(body);
}

////////////////////////////////////////////////////////////////////////////////

// PYR-64045
void ColUtilServerObject::processGetShdHandsForAmps(UINT32 reqId, CommMsgParser& parser, CommServerConnection* pConn)
{
	PLog("%s", __FUNCTION__);

	AdminSrvCon* adminConn = dynamic_cast<AdminSrvCon*>(pConn);
	if (adminConn == nullptr)
	{
		PLog("Lost admin connection, aborting");
		return;
	}

	ColAmps::GetShdHandsInputs inputs(parser);

	ColAmps::GetShdHandsOutputs outputs;
	bool found = reportCaches.getIfPresent(inputs, outputs);
	if (found)
	{
		PLog("Found in cache, replying");

		CommMsgBody reply;
		reply.composeUINT32(reqId).composeINT16(DBM_NO_ERROR);
		outputs.compose(reply);
		adminConn->post(COLSRV_A_AMPS_SHD_HANDS2, reply);

		return;
	}

	time_t now = time(nullptr);
	time_t timeoutTime = now + getShdHandsForAmpsTimeoutSeconds;
	ColAmps::GetShdHandsJobState jobState(timeoutTime, inputs);

	unique_ptr<FastReportItemBase> reportItem = make_unique<ColGetShdHandsForAmpsFastReport>(reqId, adminConn->id(), COLSRV_A_AMPS_SHD_HANDS2, adminConn->getAdminId(), *this, jobState);

	bool res = fastReportQueue.pushBack(move(reportItem));
	if (!res)
	{
		PLog("Queue full, aborting");

		CommMsgBody reply;
		reply.composeUINT32(reqId).composeINT16(TOO_MANY_ITEMS).composeString("Queue full");
		adminConn->post(COLSRV_A_AMPS_SHD_HANDS2, reply);

		return; // No more processing
	}

	res = fastReportQueue.submitNext();
	if (!res)
	{
		PLog("Max submissions reached, waiting");
	}
}

// PYR-64045
void ColUtilServerObject::processGetShdHandsForAmps_Submitted(const ColGetShdHandsForAmpsFastReport& reportItem)
{
	PLog("%s", __FUNCTION__);
	reportItem.log();

	CommServerConnection* pConn = findConnection(reportItem.connId);
	if (pConn == nullptr)
	{
		PLog("Lost admin connection, aborting");
		fastReportQueue.reportJobComplete(reportItem.uniqueId);
	}
	else
	{
		CommMsgBody body;
		reportItem.jobState.compose(body);

		postToColReportingRaw(COLRAW_Q_GET_SHDHANDSFORAMPS, body, 
			new GetShdHandsForAmpsAsyncCall(*this, reportItem));
	}
}

// PYR-64045
void ColUtilServerObject::processGetShdHandsForAmps_GotShdData(CommMsgParser& parser, GetShdHandsForAmpsAsyncCall& callState)
{
	PLog("%s", __FUNCTION__);

	ColGetShdHandsForAmpsFastReport& reportItem = callState.reportItem;
	reportItem.log();

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	if (errCode != DBM_NO_ERROR)
	{
		const char* errDesc;
		parser.parseString(errDesc);
		PLog("Error: %s", errDesc);

		callState.postResultReply(errCode, errDesc);

		fastReportQueue.reportJobComplete(reportItem.uniqueId);

		return;
	}

	callState.parseShdData(parser);
	PLog("Found %u SHD events", (UINT32)reportItem.outputs.events.size());

	callState.keepOnlyRelevantRecords();
	PLog("Keeping %u relevant SHD events", (UINT32)reportItem.outputs.events.size());

	CommServerConnection* pConn = callState.findConnection();
	if (!callState.isAdminStillConnected())
	{
		PLog("Lost admin connection, aborting");
		fastReportQueue.reportJobComplete(reportItem.uniqueId);
	}
	else if (callState.isShdEventsEmpty())
	{
		PLog("No SHD events, returning");
		callState.postResultReply(DBM_NO_ERROR, "");
		fastReportQueue.reportJobComplete(reportItem.uniqueId);
	}
	else
	{
		processGetShdHandsForAmps_GetUserInfoBatch(callState);
	}
}

// PYR-64045
void ColUtilServerObject::processGetShdHandsForAmps_GotUserInfoBatch(CommMsgParser& parser, GetShdHandsForAmpsAsyncCall& callState)
{
	PLog("%s", __FUNCTION__);

	ColGetShdHandsForAmpsFastReport& reportItem = callState.reportItem;
	reportItem.log();

	CommServerConnection* pConn = findConnection(callState.reportItem.connId);
	if (pConn == nullptr)
	{
		PLog("Lost admin connection, aborting");

		fastReportQueue.reportJobComplete(reportItem.uniqueId);

		return;
	}

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	if (errCode != DBM_NO_ERROR)
	{
		const char* errDesc;
		parser.parseString(errDesc);
		PLog("Error: %s", errDesc);

		callState.postResultReply(errCode, errDesc);

		fastReportQueue.reportJobComplete(reportItem.uniqueId);

		return;
	}

	vector<ColossusUserInfo> userInfoVec;
	parseVector(parser, userInfoVec);

	callState.updateOlapUserInfo(userInfoVec);
	callState.applyOlapUserInfoBatch();

	processGetShdHandsForAmps_GetUserInfoBatch(callState);
}

// PYR-64045
void ColUtilServerObject::processGetShdHandsForAmps_GetUserInfoBatch(GetShdHandsForAmpsAsyncCall& callState)
{
	PLog("%s", __FUNCTION__);

	vector<UINT32> userIntIdsBatch;
	callState.getNextUserIntIdBatch(userIntIdsBatch);

	if (userIntIdsBatch.empty() && callState.gotAllUserInfo())
	{
		PLog("No more userIds to fetch, returning");

		callState.postResultReply(DBM_NO_ERROR, "");

		fastReportQueue.reportJobComplete(callState.reportItem.uniqueId);

		return;
	}
	else
	{
		PLog("Fetching batch of %u userIds", (UINT32)userIntIdsBatch.size());

		CommMsgBody body;
		composeVector(body, userIntIdsBatch, ComposerUINT32());

		postToColReader(COLOSSUS_GET_USER_INFO_BATCH, body, &callState);
	}
}

// PYR-64045
GetShdHandsForAmpsAsyncCall::GetShdHandsForAmpsAsyncCall(ColUtilServerObject& obj_, const ColGetShdHandsForAmpsFastReport& reportItem_)
	: obj(obj_)
	, curEventIndex(0)
	, reportItem(reportItem_)
{

}

// PYR-64045
void GetShdHandsForAmpsAsyncCall::processReply(UINT32 reqId, UINT32 replyId, const CommMsgBody& body)
{
	CommMsgParser parser(body);

	switch (replyId)
	{
	case COLRAW_A_GET_SHDHANDSFORAMPS:
		{
			obj.processGetShdHandsForAmps_GotShdData(parser, *this);

			break;
		}
	case COLOSSUS_GET_USER_INFO_BATCH_REPLY:
		{
			obj.processGetShdHandsForAmps_GotUserInfoBatch(parser, *this);

			break;
		}
	default:
		{
			PLog("Error: Unknown reply id type %u. Reply not handled.", replyId);
			break;
		}
	}
}

// PYR-64045
void GetShdHandsForAmpsAsyncCall::getNextUserIntIdBatch(vector<UINT32>& userIntIdsBatch)
{
	ColAmps::GetShdHandsOutputs& outputs = reportItem.outputs;
	vector<ColShd::SHDEvent>& events = outputs.events;
	UINT32 batchLastEvtIndex = getLastEventIndex(events.size());

	for (UINT32 i = curEventIndex; i <= batchLastEvtIndex; ++i)
	{
		ColShd::SHDEvent& evt = events[i];

		if (evt.suspectIntId != 0)
		{
			UserInfoMapConstIt suspectInfoIt = userInfoMap.find(evt.suspectIntId);
			if (suspectInfoIt == userInfoMap.end())
			{
				userIntIdsBatch.push_back(evt.suspectIntId);
			}
		}

		for (UINT32 j = 0; j < SZARR(evt.partnersInfo); ++j)
		{
			ColShd::SHDEventPartnerInfo& partnerInfo = evt.partnersInfo[j];

			if (partnerInfo.userIntId == 0)
			{
				continue;
			}

			UserInfoMapConstIt partnerInfoIt = userInfoMap.find(partnerInfo.userIntId);
			if (partnerInfoIt == userInfoMap.end())
			{
				userIntIdsBatch.push_back(partnerInfo.userIntId);
			}
		}
	}

	sort(userIntIdsBatch.begin(), userIntIdsBatch.end());
	vector<UINT32>::iterator it = unique(userIntIdsBatch.begin(), userIntIdsBatch.end());
	userIntIdsBatch.erase(it, userIntIdsBatch.end());
}

// PYR-64045
void GetShdHandsForAmpsAsyncCall::updateOlapUserInfo(vector<ColossusUserInfo>& userInfoVec)
{
	for (const ColossusUserInfo& userInfo : userInfoVec)
	{
		if (userInfo.found)
		{
			userInfoMap.insert(make_pair(userInfo.userIntId, userInfo.userId));
		}
	}
}

// PYR-64045
void GetShdHandsForAmpsAsyncCall::applyOlapUserInfoBatch()
{
	PLog("%s", __FUNCTION__);

	ColAmps::GetShdHandsOutputs& outputs = reportItem.outputs;
	vector<ColShd::SHDEvent>& events = outputs.events;
	UINT32 batchLastEvtIndex = getLastEventIndex(events.size());

	for (int i = curEventIndex; i <= batchLastEvtIndex; ++i)
	{
		ColShd::SHDEvent& evt = events[i];

		if (evt.suspectIntId != 0)
		{
			UserInfoMapIt suspectIt = userInfoMap.find(evt.suspectIntId);
			if (suspectIt != userInfoMap.end())
			{
				evt.suspectName = suspectIt->second;
			}
		}

		for (int j = 0; j < SZARR(evt.partnersInfo); ++j)
		{
			ColShd::SHDEventPartnerInfo& partnerInfo = evt.partnersInfo[j];

			if (partnerInfo.userIntId == 0)
			{
				continue;
			}

			UserInfoMapIt partnerIt = userInfoMap.find(partnerInfo.userIntId);
			if (partnerIt != userInfoMap.end())
			{
				partnerInfo.userId = partnerIt->second;
			}
		}
	}

	curEventIndex += EVENT_BATCH_SZ_FOR_OLAP_INFO;
}

// PYR-64045
UINT32 GetShdHandsForAmpsAsyncCall::getLastEventIndex(UINT32 numEvents) const
{
	INT32 lastEvtIndex_ = numEvents - 1;
	PASSERT(lastEvtIndex_ >= 0);
	UINT32 lastEvtIndex = lastEvtIndex_;
	return min(curEventIndex + EVENT_BATCH_SZ_FOR_OLAP_INFO - 1, lastEvtIndex);
}

// PYR-64045
void GetShdHandsForAmpsAsyncCall::postResultReply(INT16 errCode, const char* errDesc) const
{
	PLog("%s", __FUNCTION__);
	PLog("num events: %u", (UINT32)reportItem.outputs.events.size());

	CommMsgBody reply;
	reply.composeUINT32(reportItem.reqId);

	setReply(reply, errCode, errDesc);

	if (errCode == DBM_NO_ERROR)
	{
		obj.insertAmpsShdHandsCache(reportItem.jobState.inputs, reportItem.outputs);
		reportItem.outputs.compose(reply);
	}

	CommServerConnection* pConn = obj.findConnection(reportItem.connId);
	postReply(pConn, reportItem.replyId, reply);
}

// PYR-64045
CommServerConnection* GetShdHandsForAmpsAsyncCall::findConnection()
{
	return obj.findConnection(reportItem.connId);
}

// PYR-64045
bool GetShdHandsForAmpsAsyncCall::isShdEventsEmpty() const
{
	const ColAmps::GetShdHandsOutputs& outputs = reportItem.outputs;
	const vector<ColShd::SHDEvent>& events = outputs.events;

	return events.empty();
}

// PYR-64045
bool GetShdHandsForAmpsAsyncCall::isAdminStillConnected() const
{
	CommServerConnection* pConn = obj.findConnection(reportItem.connId);
	return pConn != nullptr;
}

// PYR-64045
void GetShdHandsForAmpsAsyncCall::parseShdData(CommMsgParser& parser)
{
	ColAmps::GetShdHandsOutputs& outputs = reportItem.outputs;
	outputs.parse(parser);
}

// PYR-75383
void GetShdHandsForAmpsAsyncCall::keepOnlyRelevantRecords()
{
	vector<ColShd::SHDEvent> eventsToKeep;

	ColAmps::GetShdHandsOutputs& outputs = reportItem.outputs;
	for (const ColShd::SHDEvent& event : outputs.events)
	{
		bool foundSuspect = false;
		for (int i = 0; !foundSuspect && i < SZARR(event.partnersInfo); ++i)
		{
			UINT32 suspectInEventIntId = event.partnersInfo[i].userIntId;

			const vector<UINT32>& inputsSuspectIntIdVec = reportItem.jobState.inputs.userIntIds;
			for (int j = 0; !foundSuspect && j < inputsSuspectIntIdVec.size(); ++j)
			{
				if (suspectInEventIntId == inputsSuspectIntIdVec[j])
				{
					foundSuspect = true;
				}
			}
		}

		if (foundSuspect)
		{
			eventsToKeep.push_back(event);
		}
	}

	outputs.events.swap(eventsToKeep);
}

// PYR-64045
bool GetShdHandsForAmpsAsyncCall::gotAllUserInfo() const
{
	const ColAmps::GetShdHandsOutputs& outputs = reportItem.outputs;
	const vector<ColShd::SHDEvent>& events = outputs.events;

	return curEventIndex >= events.size();
}

////////////////////////////////////////////////////////////////////////////////

// PYR-64045
void ColUtilServerObject::processAmpsTopPartners(UINT32 reqId, CommMsgParser& parser, CommServerConnection* pConn)
{
	PLog("%s", __FUNCTION__);

	time_t now = time(nullptr);

	AdminSrvCon* adminConn = dynamic_cast<AdminSrvCon*>(pConn);
	if (adminConn == nullptr)
	{
		PLog("Lost admin connection, aborting");
		return;
	}

	ColAmps::TopPartnersInputs inputs(parser);

	ColAmps::TopSngPartnersCachedOutputs sngOutputs;
	ColAmps::TopRingPartnersCachedOutputs ringOutputs;
	ColAmps::TopSngPartners_RootUser sngRootUserInfo;
	ColAmps::TopRingPartners_RootUser ringRootUserInfo;

	bool isInCache = reportCaches.getIfPresent(inputs, sngOutputs);
	isInCache = isInCache && reportCaches.getIfPresent(inputs, ringOutputs);
	isInCache = isInCache && reportCaches.getIfPresent(inputs, sngRootUserInfo);
	isInCache = isInCache && reportCaches.getIfPresent(inputs, ringRootUserInfo);

	if (isInCache)
	{
		CommMsgBody reply;
		reply.
			composeUINT32(reqId).
			composeINT16(DBM_NO_ERROR);
		composeVectorOfComposableValues(sngOutputs.records, reply);
		composeVectorOfComposableValues(ringOutputs.records, reply);
		reply.
			composeSrvDate(sngOutputs.dateRange.first).
			composeSrvDate(sngOutputs.dateRange.second).
			composeSrvDate(ringOutputs.dateRange.first).
			composeSrvDate(ringOutputs.dateRange.second);

		sngRootUserInfo.compose(reply);
		ringRootUserInfo.compose(reply);

		postReply(pConn, COLSRV_A_AMPS_TOP_PARTNERS, reply);

		return;
	}

	time_t timeoutTime = now + getAmpsTopPartnersTimeoutSeconds;
	ColAmps::TopPartnersJobState jobState(timeoutTime, inputs);

	unique_ptr<FastReportItemBase> reportItem = make_unique<AmpsTopPartnersFastReport>(reqId, adminConn->id(), COLSRV_A_AMPS_TOP_PARTNERS, adminConn->getAdminId(), *this, jobState);
	reportItem->log();

	bool res = fastReportQueue.pushBack(move(reportItem));
	if (!res)
	{
		PLog("Queue full, aborting");

		CommMsgBody reply;
		reply.composeUINT32(reqId).composeINT16(TOO_MANY_ITEMS).composeString("Queue full");
		adminConn->post(COLSRV_A_AMPS_TOP_PARTNERS, reply);

		return; // No more processing
	}
	res = fastReportQueue.submitNext();
	if (!res)
	{
		PLog("Max submissions reached, waiting");
	}
}

// PYR-64045
bool ColUtilServerObject::processAmpsTopPartners_PreProcessing(const char* logStr, const AmpsTopPartnersAsyncCall& callState)
{
	PLog("%s", logStr);

	const AmpsTopPartnersFastReport& reportItem = callState.reportItem;
	reportItem.log();

	if (!callState.isAdminStillConnected())
	{
		PLog("lost admin connection, aborting");
		fastReportQueue.reportJobComplete(reportItem.uniqueId);

		return false;
	}
	else if (reportItem.isTimeout())
	{
		callState.postErrorReply(DBM_QUERY_TIMEOUT, "Timed out");
		PLog("job timed out, aborting");
		fastReportQueue.reportJobComplete(reportItem.uniqueId);

		return false;
	}
	else
	{
		return true;
	}
}

// PYR-64045
void ColUtilServerObject::processAmpsTopPartners_Submitted(const AmpsTopPartnersFastReport& reportItem)
{
	PLog("%s", __FUNCTION__);
	reportItem.log();

	CommServerConnection* pConn = findConnection(reportItem.connId);
	if (pConn == nullptr)
	{
		PLog("Lost admin connection, aborting");
		fastReportQueue.reportJobComplete(reportItem.uniqueId);

		return;
	}

	CommMsgBody body;
	reportItem.composeJobState(body);

	postToColReportingAgg(COLAGG_Q_GET_TOPPARTNERS_SNG, body, new AmpsTopPartnersAsyncCall(*this, reportItem));
}

// PYR-64045
void ColUtilServerObject::processAmpsTopPartners_SNG_GotAggData(CommMsgParser& parser, AmpsTopPartnersAsyncCall& callState)
{
	if (!processAmpsTopPartners_PreProcessing(__FUNCTION__, callState))
	{
		return;
	}

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	if (errCode != DBM_NO_ERROR)
	{
		const char* errDesc;
		parser.parseString(errDesc);
		PLog("Error: %s", errDesc);

		callState.postErrorReply(errCode, errDesc);

		fastReportQueue.reportJobComplete(callState.reportItem.uniqueId);

		return;
	}

	ColAmps::TopSngPartnersAggOutputs& aggOutputs = callState.aggSngOutputs;
	aggOutputs.parse(parser);

	DateRange& aggDateRange = aggOutputs.aggDateRange;
	if (!aggDateRange.first.isNull())
	{
		ColAmps::TopSngPartnersCachedOutputs& sngOutputs = callState.finalSngOutputs;
		sngOutputs.dateRange = aggDateRange;
	}

	ColAmps::TopPartnersJobState& jobState = callState.reportItem.jobState;

	if (!jobState.aggInputs.includeUTMSNG)
	{
		callState.calcSngOutputRecords();
		processAmpsTopPartners_SNG_GetUserInfoBatch(callState);

		return;
	}

	jobState.setRawInputPeriod(aggDateRange);

	ColAmps::TopPartnersInputs& rawInputs = jobState.rawInputs;
	if (!rawInputs.requestedStartTime.isNull() &&
		!rawInputs.requestedEndTime.isNull() &&
		rawInputs.requestedStartTime.encode() < rawInputs.requestedEndTime.encode())
	{
		CommMsgBody body;
		jobState.compose(body);

		postToColReportingRaw(COLRAW_Q_GET_TOPPARTNERS_SNG, body, &callState);
	}
	else
	{
		callState.calcSngOutputRecords();
		processAmpsTopPartners_SNG_GetUserInfoBatch(callState);
	}
}

// PYR-64045
void ColUtilServerObject::processAmpsTopPartners_SNG_GotRawData(CommMsgParser& parser, AmpsTopPartnersAsyncCall& callState)
{
	if (!processAmpsTopPartners_PreProcessing(__FUNCTION__, callState))
	{
		return;
	}

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	if (errCode != DBM_NO_ERROR)
	{
		const char* errDesc;
		parser.parseString(errDesc);
		PLog("Error: %s", errDesc);

		callState.postErrorReply(errCode, errDesc);

		fastReportQueue.reportJobComplete(callState.reportItem.uniqueId);

		return;
	}

	ColAmps::TopSngPartnersRawOutputs& rawOutputs = callState.rawSngOutputs;
	rawOutputs.parse(parser);

	ColAmps::TopSngPartnersCachedOutputs& sngOutputs = callState.finalSngOutputs;
	ColAmps::TopPartnersJobState& jobState = callState.reportItem.jobState;
	ColAmps::TopPartnersInputs& rawInputs = jobState.rawInputs;
	DateRange& outputDateRange = sngOutputs.dateRange;

	if (outputDateRange.first.isNull())
	{
		outputDateRange.first.decode(rawInputs.requestedStartTime.encode());
	}
	outputDateRange.second.decode(rawInputs.requestedEndTime.encode());

	callState.calcSngOutputRecords();
	processAmpsTopPartners_SNG_GetUserInfoBatch(callState);
}

// PYR-64045
void ColUtilServerObject::processAmpsTopPartners_SNG_GetUserInfoBatch(AmpsTopPartnersAsyncCall& callState)
{
	if (!processAmpsTopPartners_PreProcessing(__FUNCTION__, callState))
	{
		return;
	}

	vector<PString> userIdsBatch;
	callState.getNextSngUserIdsBatch(userIdsBatch);

	if (userIdsBatch.empty() && callState.gotAllSngUsersInfo())
	{
		PLog("No more userIds to fetch, get RING data");

		CommMsgBody body;
		callState.composeJobState(body);

		postToColReportingAgg(COLAGG_Q_GET_TOPPARTNERS_RING, body, &callState);
	}
	else
	{
		PLog("Fetching batch of %u user info", (UINT32)userIdsBatch.size());

		CommMsgBody body;
		composeVector(body, userIdsBatch, ComposerPString());

		postToColReader(COLOSSUS_GET_USER_INFO_BYUID_BATCH, body, &callState);
	}
}

// PYR-64045
void ColUtilServerObject::processAmpsTopPartners_RING_GotAggData(CommMsgParser& parser, AmpsTopPartnersAsyncCall& callState)
{
	if (!processAmpsTopPartners_PreProcessing(__FUNCTION__, callState))
	{
		return;
	}

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	if (errCode != DBM_NO_ERROR)
	{
		const char* errDesc;
		parser.parseString(errDesc);
		PLog("Error: %s", errDesc);

		callState.postErrorReply(errCode, errDesc);

		fastReportQueue.reportJobComplete(callState.reportItem.uniqueId);

		return;
	}

	ColAmps::TopRingPartnersAggOutputs& aggOutputs = callState.aggRingOutputs;
	aggOutputs.parse(parser);

	DateRange& aggDateRange = aggOutputs.aggDateRange;
	if (!aggDateRange.first.isNull())
	{
		ColAmps::TopRingPartnersCachedOutputs& ringOutputs = callState.finalRingOutputs;
		ringOutputs.dateRange = aggDateRange;
	}

	ColAmps::TopPartnersJobState& jobState = callState.reportItem.jobState;

	if (!jobState.aggInputs.includeUTMRing)
	{
		callState.calcRingOutputRecords();
		processAmpsTopPartners_RING_GetUserInfoBatch(callState);

		return;
	}

	jobState.setRawInputPeriod(aggDateRange);

	ColAmps::TopPartnersInputs& rawInputs = jobState.rawInputs;
	if (!rawInputs.requestedStartTime.isNull() &&
		!rawInputs.requestedEndTime.isNull() &&
		rawInputs.requestedStartTime.encode() < rawInputs.requestedEndTime.encode())
	{
		CommMsgBody body;
		jobState.compose(body);

		postToColReportingRaw(COLRAW_Q_GET_TOPPARTNERS_RING, body, &callState);
	}
	else
	{
		callState.calcRingOutputRecords();
		processAmpsTopPartners_RING_GetUserInfoBatch(callState);
	}
}

// PYR-64045
void ColUtilServerObject::processAmpsTopPartners_RING_GotRawData(CommMsgParser& parser, AmpsTopPartnersAsyncCall& callState)
{
	if (!processAmpsTopPartners_PreProcessing(__FUNCTION__, callState))
	{
		return;
	}

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	if (errCode != DBM_NO_ERROR)
	{
		const char* errDesc;
		parser.parseString(errDesc);
		PLog("Error: %s", errDesc);

		callState.postErrorReply(errCode, errDesc);

		fastReportQueue.reportJobComplete(callState.reportItem.uniqueId);

		return;
	}

	ColAmps::TopRingPartnersRawOutputs& rawOutputs = callState.rawRingOutputs;
	rawOutputs.parse(parser);

	ColAmps::TopRingPartnersCachedOutputs& ringOutputs = callState.finalRingOutputs;
	ColAmps::TopPartnersJobState& jobState = callState.reportItem.jobState;
	ColAmps::TopPartnersInputs& rawInputs = jobState.rawInputs;
	DateRange& outputDateRange = ringOutputs.dateRange;

	if (outputDateRange.first.isNull())
	{
		outputDateRange.first.decode(rawInputs.requestedStartTime.encode());
	}
	outputDateRange.second.decode(rawInputs.requestedEndTime.encode());

	callState.calcRingOutputRecords();
	processAmpsTopPartners_RING_GetUserInfoBatch(callState);
}

// PYR-64045
void ColUtilServerObject::processAmpsTopPartners_RING_GetUserInfoBatch(AmpsTopPartnersAsyncCall& callState)
{
	if (!processAmpsTopPartners_PreProcessing(__FUNCTION__, callState))
	{
		return;
	}

	vector<PString> userIdsBatch;
	callState.getNextRingUserIdsBatch(userIdsBatch);

	if (userIdsBatch.empty() && callState.gotAllRingUsersInfo())
	{
		PLog("No more userIds to fetch, returning");

		callState.postResultReply();

		fastReportQueue.reportJobComplete(callState.reportItem.uniqueId);

		return;
	}
	else
	{
		PLog("Fetching batch of %u user info", (UINT32)userIdsBatch.size());

		CommMsgBody body;
		composeVector(body, userIdsBatch, ComposerPString());

		postToColReader(COLOSSUS_GET_USER_INFO_BYUID_BATCH, body, &callState);
	}
}

// PYR-64045
void ColUtilServerObject::processAmpsTopPartners_GotUserInfoBatch(CommMsgParser& parser, AmpsTopPartnersAsyncCall& callState)
{
	if (!processAmpsTopPartners_PreProcessing(__FUNCTION__, callState))
	{
		return;
	}

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	if (errCode != DBM_NO_ERROR)
	{
		const char* errDesc;
		parser.parseString(errDesc);
		PLog("Error: %s", errDesc);

		callState.postErrorReply(errCode, errDesc);

		fastReportQueue.reportJobComplete(callState.reportItem.uniqueId);

		return;
	}

	vector<ColossusUserInfo> userInfoVec;
	parseVector(parser, userInfoVec);

	callState.updateOlapUserInfo(userInfoVec);

	if (callState.processingState == AmpsTopPartnersAsyncCall::eProcessingSNG)
	{
		callState.applySngOlapUserInfoBatch();
		processAmpsTopPartners_SNG_GetUserInfoBatch(callState);
	}
	else
	{
		callState.applyRingOlapUserInfoBatch();
		processAmpsTopPartners_RING_GetUserInfoBatch(callState);
	}
}

// PYR-64045
AmpsTopPartnersAsyncCall::AmpsTopPartnersAsyncCall(ColUtilServerObject& obj_, const AmpsTopPartnersFastReport& reportItem_)
	: obj(obj_)
	, reportItem(reportItem_)
	, processingState(AmpsTopPartnersAsyncCall::eProcessingInvalid)
	, curSngRecIndex(0)
	, curRingRecIndex(0)
{

}

// PYR-64045
AmpsTopPartnersAsyncCall::~AmpsTopPartnersAsyncCall()
{
	PTRACE5("~AmpsTopPartnersAsyncCall");
}

// PYR-64045
void AmpsTopPartnersAsyncCall::processReply(UINT32 requestId, UINT32 replyId, const CommMsgBody& body)
{
	CommMsgParser parser(body);

	switch (replyId)
	{
	case COLAGG_A_GET_TOPPARTNERS_SNG:
		{
			processingState = eProcessingSNG;
			obj.processAmpsTopPartners_SNG_GotAggData(parser, *this);
			break;
		}
	case COLRAW_A_GET_TOPPARTNERS_SNG:
		{
			processingState = eProcessingSNG;
			obj.processAmpsTopPartners_SNG_GotRawData(parser, *this);
			break;
		}
	case COLAGG_A_GET_TOPPARTNERS_RING:
		{
			processingState = eProcessingRing;
			obj.processAmpsTopPartners_RING_GotAggData(parser, *this);
			break;
		}
	case COLRAW_A_GET_TOPPARTNERS_RING:
		{
			processingState = eProcessingRing;
			obj.processAmpsTopPartners_RING_GotRawData(parser, *this);
			break;
		}
	case COLOSSUS_GET_USER_INFO_BYUID_BATCH_REPLY:
		{
			obj.processAmpsTopPartners_GotUserInfoBatch(parser, *this);
			break;
		}
	default:
		{
			PLog("Error: Unknown reply id type %u. Reply not handled.", replyId);
			break;
		}
	}
}

// PYR-64045
void AmpsTopPartnersAsyncCall::postErrorReply(INT16 errCode, const char* errDesc) const
{
	CommMsgBody reply;
	reply.composeUINT32(reportItem.reqId);

	setReply(reply, errCode, errDesc);

	CommServerConnection* pConn = obj.findConnection(reportItem.connId);
	postReply(pConn, reportItem.replyId, reply);
}

// PYR-64045
void AmpsTopPartnersAsyncCall::postResultReply() const
{
	PLog("AmpsTopPartnersAsyncCall::postResultReply");
	PLog("sng recs size: %u", finalSngOutputs.size());
	PLog("ring recs size: %u", finalRingOutputs.size());

	obj.insertAmpsTopSngPartnersCache(reportItem.jobState.requestedInputs, finalSngOutputs);
	obj.insertAmpsTopRingPartnersCache(reportItem.jobState.requestedInputs, finalRingOutputs);
	obj.insertAmpsTopSngPartnersRootUserCache(reportItem.jobState.requestedInputs, sngRootUserInfo);
	obj.insertAmpsTopRingPartnersRootUserCache(reportItem.jobState.requestedInputs, ringRootUserInfo);

	CommMsgBody reply;
	reply.
		composeUINT32(reportItem.reqId).
		composeINT16(DBM_NO_ERROR);
	composeVectorOfComposableValues(finalSngOutputs.records, reply);
	composeVectorOfComposableValues(finalRingOutputs.records, reply);
	reply.
		composeSrvDate(finalSngOutputs.dateRange.first).
		composeSrvDate(finalSngOutputs.dateRange.second).
		composeSrvDate(finalRingOutputs.dateRange.first).
		composeSrvDate(finalRingOutputs.dateRange.second);

	sngRootUserInfo.compose(reply);
	ringRootUserInfo.compose(reply);

	CommServerConnection* pConn = obj.findConnection(reportItem.connId);
	postReply(pConn, reportItem.replyId, reply);
}

// PYR-64045
bool AmpsTopPartnersAsyncCall::isAdminStillConnected() const
{
	CommServerConnection* pConn = obj.findConnection(reportItem.connId);
	return pConn != nullptr;
}

// PYR-64045
void AmpsTopPartnersAsyncCall::composeJobState(CommMsgBody& body) const
{
	reportItem.composeJobState(body);
}

// PYR-64045
void AmpsTopPartnersAsyncCall::updateOlapUserInfo(const vector<ColossusUserInfo>& userInfoVec)
{
	for_each(userInfoVec.begin(), userInfoVec.end(),
		[&](const ColossusUserInfo& userInfo) 
		{
			if (userInfo.found)
			{
				userInfoMap.insert(make_pair(userInfo.userId, userInfo));
			}
		}
	);
}

// PYR-64045
void AmpsTopPartnersAsyncCall::calcSngOutputRecords()
{
	PLog("%s", __FUNCTION__);

	vector<ColAmps::TopSngPartnersRecord> allRecs;
	ColAmps::mergeSngAggAndRawOutputs(aggSngOutputs, rawSngOutputs, allRecs);
	PLog("found %u overall records", (UINT32)allRecs.size());

	sort(allRecs.begin(), allRecs.end(), ColAmps::TopSngPartnersRecordComparer());

	copySngRelevantRecords(allRecs, finalSngOutputs.records);

	PLog("found %u relevant records", finalSngOutputs.size());

	sngRootUserInfo.clear();
	sngRootUserInfo.merge(aggSngOutputs.rootUser);
	sngRootUserInfo.merge(rawSngOutputs.rootUser);
	sngRootUserInfo.log();
}

// PYR-75383 - Deleted obsolete function AmpsTopPartnersAsyncCall::mergeSngAggAndRawOutputs()

// PYR-64045
void AmpsTopPartnersAsyncCall::copySngRelevantRecords(const vector<ColAmps::TopSngPartnersRecord>& records, vector<ColAmps::TopSngPartnersRecord>& relevantRecords) const
{
	const ColAmps::TopPartnersJobState& job = reportItem.jobState;
	const ColAmps::TopPartnersInputs& jobInputs = job.aggInputs;
	const UINT32& numRequiredRecords = jobInputs.numRequiredRecords;

	for (const auto& rec : records)
	{
		// Add top 15 records (or 'numRequiredRecords')
		if (relevantRecords.size() < numRequiredRecords)
		{
			relevantRecords.push_back(rec);
		}
		else
		{
			// Also add records for any suspects

			const vector<PString>& suspectIds = jobInputs.suspectIds;
			auto it = find_if(suspectIds.begin(), suspectIds.end(), 
				[&](const PString& suspectId) { return PString::equals(suspectId, rec.partnerId); }
			);

			if (it != suspectIds.end())
			{
				relevantRecords.push_back(rec);
			}
		}
	}
}

// PYR-64045
INT32 AmpsTopPartnersAsyncCall::getLastSngOutputRecIndex() const
{
	INT32 numOutputRecs = finalSngOutputs.size();
	INT32 lastRecIndex = numOutputRecs - 1;
	return min(curSngRecIndex + PARTNER_BATCH_SZ_FOR_OLAP_INFO - 1, lastRecIndex);
}

// PYR-64045
void AmpsTopPartnersAsyncCall::getNextSngUserIdsBatch(vector<PString>& userIdsBatch) const
{
	INT32 batchLastRecIndex = getLastSngOutputRecIndex();

	for (INT32 i = curSngRecIndex; i <= batchLastRecIndex; ++i)
	{
		const ColAmps::TopSngPartnersRecord& rec = finalSngOutputs.records[i];

		UserInfoMapConstIt suspectInfoIt = userInfoMap.find(rec.partnerId);
		if (suspectInfoIt == userInfoMap.end())
		{
			userIdsBatch.push_back(rec.partnerId);
		}
	}

	sort(userIdsBatch.begin(), userIdsBatch.end(), PStringCmp());
	vector<PString>::iterator it = unique(userIdsBatch.begin(), userIdsBatch.end(), 
		[](const PString& left, const PString& right)
		{
			return PString::equals(left, right);
		}
	);
	userIdsBatch.erase(it, userIdsBatch.end());
}

// PYR-64045
bool AmpsTopPartnersAsyncCall::gotAllSngUsersInfo() const
{
	return curSngRecIndex >= finalSngOutputs.size();
}

// PYR-64045
void AmpsTopPartnersAsyncCall::applySngOlapUserInfoBatch()
{
	UINT32 batchLastRecIndex = getLastSngOutputRecIndex();

	for (int i = curSngRecIndex; i <= batchLastRecIndex; ++i)
	{
		ColAmps::TopSngPartnersRecord& rec = finalSngOutputs.records[i];

		UserInfoMapIt suspectIt = userInfoMap.find(rec.partnerId);
		if (suspectIt != userInfoMap.end())
		{
			const ColossusUserInfo& ui = suspectIt->second;
			rec.ltVpp = ui.ltFpp;
			rec.partnerCountry = ui.country;
		}
	}

	curSngRecIndex += PARTNER_BATCH_SZ_FOR_OLAP_INFO;
}

// PYR-64045
void AmpsTopPartnersAsyncCall::calcRingOutputRecords()
{
	PLog("%s", __FUNCTION__);

	vector<ColAmps::TopRingPartnersRecord> allRecs;
	mergeRingAggAndRawOutputs(allRecs);

	PLog("found %u overall records", (UINT32)allRecs.size());

	sort(allRecs.begin(), allRecs.end(), ColAmps::TopRingPartnersRecordComparer());

	copyRingRelevantRecords(allRecs, finalRingOutputs.records);

	PLog("found %u relevant records", finalRingOutputs.size());

	ringRootUserInfo.clear();
	ringRootUserInfo.merge(aggRingOutputs.rootUser);
	ringRootUserInfo.merge(rawRingOutputs.rootUser);
	ringRootUserInfo.log();
}

// PYR-64045
void AmpsTopPartnersAsyncCall::getNextRingUserIdsBatch(vector<PString>& userIdsBatch) const
{
	INT32 batchLastRecIndex = getLastRingOutputRecIndex();

	for (INT32 i = curRingRecIndex; i <= batchLastRecIndex; ++i)
	{
		const ColAmps::TopRingPartnersRecord& rec = finalRingOutputs.records[i];

		UserInfoMapConstIt suspectInfoIt = userInfoMap.find(rec.partnerId);
		if (suspectInfoIt == userInfoMap.end())
		{
			userIdsBatch.push_back(rec.partnerId);
		}
	}

	sort(userIdsBatch.begin(), userIdsBatch.end(), PStringCmp());
	vector<PString>::iterator it = unique(userIdsBatch.begin(), userIdsBatch.end(), 
		[](const PString& left, const PString& right)
	{
		return PString::equals(left, right);
	}
	);
	userIdsBatch.erase(it, userIdsBatch.end());
}

// PYR-64045
bool AmpsTopPartnersAsyncCall::gotAllRingUsersInfo() const
{
	return curRingRecIndex >= finalRingOutputs.size();
}

// PYR-64045
void AmpsTopPartnersAsyncCall::applyRingOlapUserInfoBatch()
{
	UINT32 batchLastRecIndex = getLastRingOutputRecIndex();

	for (int i = curRingRecIndex; i <= batchLastRecIndex; ++i)
	{
		ColAmps::TopRingPartnersRecord& rec = finalRingOutputs.records[i];

		UserInfoMapIt suspectIt = userInfoMap.find(rec.partnerId);
		if (suspectIt != userInfoMap.end())
		{
			const ColossusUserInfo& ui = suspectIt->second;
			rec.ltVpp = ui.ltFpp;
			rec.partnerCountry = ui.country;
		}
	}

	curRingRecIndex += PARTNER_BATCH_SZ_FOR_OLAP_INFO;
}

// PYR-64045
void AmpsTopPartnersAsyncCall::mergeRingAggAndRawOutputs(vector<ColAmps::TopRingPartnersRecord>& records)
{
	ColAmps::TopRingPartners_RootUser rootUser(aggRingOutputs.rootUser);
	rootUser.merge(rawRingOutputs.rootUser);

	PStringMap<INT32> aggIndexMap;
	aggRingOutputs.createUserToRecordIndexMap(aggIndexMap);

	PStringMap<INT32> rawIndexMap;
	rawRingOutputs.createUserToRecordIndexMap(rawIndexMap);

	vector<ColAmps::TopRingPartners_RootUserWithPartner> withPartnerRecords;

	// Merge raw recordset into agg recordset
	for (auto& aggKeyValPair : aggIndexMap)
	{
		const PString& partnerId = aggKeyValPair.first;
		const INT32 aggIndex = aggKeyValPair.second;
		ColAmps::TopRingPartners_RootUserWithPartner& aggPartnerRec = aggRingOutputs.withPartnerRecords[aggIndex];

		auto it = rawIndexMap.find(partnerId);
		if (it != rawIndexMap.end())
		{
			const INT32 rawIndex = it->second;

			ColAmps::TopRingPartners_RootUserWithPartner partnerRec(aggPartnerRec);
			ColAmps::TopRingPartners_RootUserWithPartner& rawPartnerRec = rawRingOutputs.withPartnerRecords[rawIndex];
			partnerRec.merge(rawPartnerRec);
			withPartnerRecords.push_back(partnerRec);
		}
		else
		{
			withPartnerRecords.push_back(aggPartnerRec);
		}
	}

	// Append raw records that were unmatched in agg recordset
	for (auto& rawKeyValPair : rawIndexMap)
	{
		const PString& partnerId = rawKeyValPair.first;
		const INT32 rawIndex = rawKeyValPair.second;

		auto it = aggIndexMap.find(partnerId);
		if (it == aggIndexMap.end())
		{
			withPartnerRecords.push_back(rawRingOutputs.withPartnerRecords[rawIndex]);
		}
	}

	for (const auto& withPartnerRec : withPartnerRecords)
	{
		//rootUser.state.numOverlappingTourns += withPartnerRec.numTourns;

		ColAmps::TopRingPartnersRecord rec(rootUser, withPartnerRec);
		records.push_back(rec);
	}
}

// PYR-64045
void AmpsTopPartnersAsyncCall::copyRingRelevantRecords(const vector<ColAmps::TopRingPartnersRecord>& records, vector<ColAmps::TopRingPartnersRecord>& relevantRecords) const
{
	const ColAmps::TopPartnersJobState& job = reportItem.jobState;
	const ColAmps::TopPartnersInputs& jobInputs = job.aggInputs;
	const UINT32& numRequiredRecords = jobInputs.numRequiredRecords;

	for (const auto& rec : records)
	{
		// Add top 15 records (or 'numRequiredRecords')
		if (relevantRecords.size() < numRequiredRecords)
		{
			relevantRecords.push_back(rec);
		}
		else
		{
			// Also add records for any suspects

			const vector<PString>& suspectIds = jobInputs.suspectIds;
			auto it = find_if(suspectIds.begin(), suspectIds.end(), 
				[&](const PString& suspectId) { return PString::equals(suspectId, rec.partnerId); }
			);

			if (it != suspectIds.end())
			{
				relevantRecords.push_back(rec);
			}
		}
	}
}

// PYR-64045
INT32 AmpsTopPartnersAsyncCall::getLastRingOutputRecIndex() const
{
	INT32 numOutputRecs = finalRingOutputs.size();
	INT32 lastRecIndex = numOutputRecs - 1;
	return min(curRingRecIndex + PARTNER_BATCH_SZ_FOR_OLAP_INFO - 1, lastRecIndex);
}

// PYR-64045
CommServerConnection* AmpsTopPartnersAsyncCall::findConnection() const
{
	return obj.findConnection(reportItem.connId);
}

////////////////////////////////////////////////////////////////////////////////

// PYR-64045
void ColUtilServerObject::processAmpsGameActivity(UINT32 reqId, CommMsgParser& parser, CommServerConnection* pConn)
{
	PLog("%s", __FUNCTION__);

	time_t now = time(nullptr);

	AdminSrvCon* adminConn = dynamic_cast<AdminSrvCon*>(pConn);
	if (adminConn == nullptr)
	{
		PLog("Lost admin connection, aborting");
		return;
	}

	if (currencies.empty())
	{
		PLog("No currency information yet, aborting");

		CommMsgBody reply;
		reply.composeUINT32(reqId).composeINT16(NOT_INITIALIZED).composeString("No currency information yet, please wait");
		adminConn->post(COLSRV_A_AMPS_GAME_ACTIVITY, reply);

		return;
	}

	ColAmps::GameActivityInputs inputs(parser);

	ColAmps::SngGameActivityCachedOutputs sngOutputs;
	ColAmps::RingGameActivityCachedOutputs ringOutputs;
	if (reportCaches.getIfPresent(inputs, sngOutputs) && reportCaches.getIfPresent(inputs, ringOutputs))
	{
		CommMsgBody reply;
		reply.
			composeUINT32(reqId).
			composeINT16(DBM_NO_ERROR);
		composeVectorOfComposableValues(sngOutputs.records, reply);
		composeInt32PstringMap(reply, sngOutputs.numTournsByUserMap);
		composeVectorOfComposableValues(ringOutputs.records, reply);
		composeInt32PstringMap(reply, ringOutputs.numHandsByUserMap);

		reply.
			composeSrvDate(sngOutputs.dateRange.first).
			composeSrvDate(sngOutputs.dateRange.second).
			composeSrvDate(ringOutputs.dateRange.first).
			composeSrvDate(ringOutputs.dateRange.second);

		postReply(pConn, COLSRV_A_AMPS_GAME_ACTIVITY, reply);

		return;
	}

	time_t timeoutTime = now + getAmpsGameActivityTimeoutSeconds;
	ColAmps::GameActivityJobState jobState(timeoutTime, inputs);

	unique_ptr<FastReportItemBase> reportItem = make_unique<AmpsGameActivityFastReport>(reqId, adminConn->id(), COLSRV_A_AMPS_GAME_ACTIVITY, adminConn->getAdminId(), *this, jobState);

	reportItem->log();

	bool res = fastReportQueue.pushBack(move(reportItem));
	if (!res)
	{
		PLog("Queue full, aborting");

		CommMsgBody reply;
		reply.composeUINT32(reqId).composeINT16(TOO_MANY_ITEMS).composeString("Queue full");
		adminConn->post(COLSRV_A_AMPS_GAME_ACTIVITY, reply);

		return; // No more processing
	}

	res = fastReportQueue.submitNext();
	if (!res)
	{
		PLog("Max submissions reached, waiting");
	}
}

// PYR-64045
bool ColUtilServerObject::processAmpsGameActivity_PreProcessing(const char* logStr, const AmpsGameActivityAsyncCall& callState)
{
	PLog("%s", logStr);

	const AmpsGameActivityFastReport& reportItem = callState.reportItem;
	reportItem.log();

	if (!callState.isAdminStillConnected())
	{
		PLog("lost admin connection, aborting");
		fastReportQueue.reportJobComplete(reportItem.uniqueId);

		return false;
	}
	else if (reportItem.isTimeout())
	{
		callState.postErrorReply(DBM_QUERY_TIMEOUT, "Timed out");
		PLog("job timed out, aborting");
		fastReportQueue.reportJobComplete(reportItem.uniqueId);

		return false;
	}
	else
	{
		return true;
	}
}

// PYR-64045
void ColUtilServerObject::processAmpsGameActivity_Submitted(const AmpsGameActivityFastReport& reportItem)
{
	PLog("%s", __FUNCTION__);
	reportItem.log();

	CommServerConnection* pConn = findConnection(reportItem.connId);
	if (pConn == nullptr)
	{
		PLog("Lost admin connection, aborting");
		fastReportQueue.reportJobComplete(reportItem.uniqueId);

		return;
	}

	CommMsgBody body;
	reportItem.composeJobState(body);

	postToColReportingAgg(COLAGG_Q_GET_GAMEACTIVITY_SNG, body, new AmpsGameActivityAsyncCall(*this, reportItem));
}

// PYR-64045
void ColUtilServerObject::processAmpsGameActivity_SNG_GotAggData(CommMsgParser& parser, AmpsGameActivityAsyncCall& callState)
{
	if (!processAmpsGameActivity_PreProcessing(__FUNCTION__, callState))
	{
		return;
	}

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	if (errCode != DBM_NO_ERROR)
	{
		const char* errDesc;
		parser.parseString(errDesc);
		PLog("Error: %s", errDesc);

		callState.postErrorReply(errCode, errDesc);

		fastReportQueue.reportJobComplete(callState.reportItem.uniqueId);

		return;
	}

	ColAmps::SngGameActivityAggOutputs& aggOutputs = callState.aggSngOutputs;
	aggOutputs.parse(parser);

	DateRange& aggDateRange = aggOutputs.aggDateRange;
	if (!aggDateRange.first.isNull())
	{
		ColAmps::SngGameActivityCachedOutputs& sngOutputs = callState.finalSngOutputs;
		sngOutputs.dateRange = aggDateRange;
	}

	ColAmps::GameActivityJobState& jobState = callState.reportItem.jobState;

	if (!jobState.aggInputs.includeUTMSNG)
	{
		callState.mergeSngAggAndRawOutputs();

		PLog("Not getting SNG UTM data, get RING data");
		CommMsgBody body;
		callState.composeJobState(body);
		postToColReportingAgg(COLAGG_Q_GET_GAMEACTIVITY_RING, body, &callState);

		return;
	}

	jobState.setRawInputPeriod(aggOutputs.aggDateRange);

	ColAmps::GameActivityInputs& rawInputs = jobState.rawInputs;
	if (!rawInputs.requestedStartTime.isNull() &&
		!rawInputs.requestedEndTime.isNull() &&
		rawInputs.requestedStartTime.encode() < rawInputs.requestedEndTime.encode())
	{
		CommMsgBody body;
		jobState.compose(body);

		postToColReportingRaw(COLRAW_Q_GET_GAMEACTIVITY_SNG, body, &callState);
	}
	else
	{
		callState.mergeSngAggAndRawOutputs();

		PLog("Not getting SNG UTM data, get RING data");
		CommMsgBody body;
		callState.composeJobState(body);
		postToColReportingAgg(COLAGG_Q_GET_GAMEACTIVITY_RING, body, &callState);

		return;
	}
}

// PYR-64045
void ColUtilServerObject::processAmpsGameActivity_SNG_GotRawData(CommMsgParser& parser, AmpsGameActivityAsyncCall& callState)
{
	if (!processAmpsGameActivity_PreProcessing(__FUNCTION__, callState))
	{
		return;
	}

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	if (errCode != DBM_NO_ERROR)
	{
		const char* errDesc;
		parser.parseString(errDesc);
		PLog("Error: %s", errDesc);

		callState.postErrorReply(errCode, errDesc);

		fastReportQueue.reportJobComplete(callState.reportItem.uniqueId);

		return;
	}

	ColAmps::SngGameActivityRawOutputs& rawOutputs = callState.rawSngOutputs;
	rawOutputs.parse(parser);

	ColAmps::SngGameActivityCachedOutputs& sngOutputs = callState.finalSngOutputs;
	ColAmps::GameActivityJobState& jobState = callState.reportItem.jobState;
	ColAmps::GameActivityInputs& rawInputs = jobState.rawInputs;
	DateRange& outputDateRange = sngOutputs.dateRange;

	if (outputDateRange.first.isNull())
	{
		outputDateRange.first.decode(rawInputs.requestedStartTime.encode());
	}
	outputDateRange.second.decode(rawInputs.requestedEndTime.encode());

	callState.mergeSngAggAndRawOutputs();

	CommMsgBody body;
	callState.composeJobState(body);
	postToColReportingAgg(COLAGG_Q_GET_GAMEACTIVITY_RING, body, &callState);
}

// PYR-64045
void ColUtilServerObject::processAmpsGameActivity_RING_GotAggData(CommMsgParser& parser, AmpsGameActivityAsyncCall& callState)
{
	if (!processAmpsGameActivity_PreProcessing(__FUNCTION__, callState))
	{
		return;
	}

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	if (errCode != DBM_NO_ERROR)
	{
		const char* errDesc;
		parser.parseString(errDesc);
		PLog("Error: %s", errDesc);

		callState.postErrorReply(errCode, errDesc);

		fastReportQueue.reportJobComplete(callState.reportItem.uniqueId);

		return;
	}

	ColAmps::RingGameActivityAggOutputs& aggOutputs = callState.aggRingOutputs;
	aggOutputs.parse(parser);

	DateRange& aggDateRange = aggOutputs.aggDateRange;
	if (!aggDateRange.first.isNull())
	{
		ColAmps::RingGameActivityCachedOutputs& ringOutputs = callState.finalRingOutputs;
		ringOutputs.dateRange = aggDateRange;
	}

	ColAmps::GameActivityJobState& jobState = callState.reportItem.jobState;

	if (!jobState.aggInputs.includeUTMRing)
	{
		callState.mergeRingAggAndRawOutputs();

		PLog("Not getting RING UTM data, done.");
		
		callState.postResultReply();
		fastReportQueue.reportJobComplete(callState.reportItem.uniqueId);

		return;
	}

	jobState.setRawInputPeriod(aggOutputs.aggDateRange);

	ColAmps::GameActivityInputs& rawInputs = jobState.rawInputs;
	if (!rawInputs.requestedStartTime.isNull() &&
		!rawInputs.requestedEndTime.isNull() &&
		rawInputs.requestedStartTime.encode() < rawInputs.requestedEndTime.encode())
	{
		CommMsgBody body;
		jobState.compose(body);

		postToColReportingRaw(COLRAW_Q_GET_GAMEACTIVITY_RING, body, &callState);
	}
	else
	{
		callState.mergeRingAggAndRawOutputs();

		PLog("Not getting RING UTM data, done.");

		callState.postResultReply();
		fastReportQueue.reportJobComplete(callState.reportItem.uniqueId);

		return;
	}
}

// PYR-64045
void ColUtilServerObject::processAmpsGameActivity_RING_GotRawData(CommMsgParser& parser, AmpsGameActivityAsyncCall& callState)
{
	if (!processAmpsGameActivity_PreProcessing(__FUNCTION__, callState))
	{
		return;
	}

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	if (errCode != DBM_NO_ERROR)
	{
		const char* errDesc;
		parser.parseString(errDesc);
		PLog("Error: %s", errDesc);

		callState.postErrorReply(errCode, errDesc);

		fastReportQueue.reportJobComplete(callState.reportItem.uniqueId);

		return;
	}

	ColAmps::RingGameActivityRawOutputs& rawOutputs = callState.rawRingOutputs;
	rawOutputs.parse(parser);

	ColAmps::RingGameActivityCachedOutputs& ringOutputs = callState.finalRingOutputs;
	ColAmps::GameActivityJobState& jobState = callState.reportItem.jobState;
	ColAmps::GameActivityInputs& rawInputs = jobState.rawInputs;
	DateRange& outputDateRange = ringOutputs.dateRange;

	if (outputDateRange.first.isNull())
	{
		outputDateRange.first.decode(rawInputs.requestedStartTime.encode());
	}
	outputDateRange.second.decode(rawInputs.requestedEndTime.encode());

	callState.mergeRingAggAndRawOutputs();

	callState.postResultReply();
	fastReportQueue.reportJobComplete(callState.reportItem.uniqueId);
}

// PYR-64045
AmpsGameActivityAsyncCall::AmpsGameActivityAsyncCall(ColUtilServerObject& obj_, const AmpsGameActivityFastReport& reportItem_)
	: obj(obj_)
	, reportItem(reportItem_)
	, processingState(AmpsGameActivityAsyncCall::eProcessingInvalid)
{

}

// PYR-64045
AmpsGameActivityAsyncCall::~AmpsGameActivityAsyncCall()
{
	PTRACE5("~AmpsGameActivityAsyncCall");
}

// PYR-64045
void AmpsGameActivityAsyncCall::processReply(UINT32 requestId, UINT32 replyId, const CommMsgBody& body)
{
	CommMsgParser parser(body);

	switch (replyId)
	{
	case COLAGG_A_GET_GAMEACTIVITY_SNG:
		{
			processingState = eProcessingSNG;
			obj.processAmpsGameActivity_SNG_GotAggData(parser, *this);
			break;
		}
	case COLRAW_A_GET_GAMEACTIVITY_SNG:
		{
			processingState = eProcessingSNG;
			obj.processAmpsGameActivity_SNG_GotRawData(parser, *this);
			break;
		}
	case COLAGG_A_GET_GAMEACTIVITY_RING:
		{
			processingState = eProcessingRing;
			obj.processAmpsGameActivity_RING_GotAggData(parser, *this);
			break;
		}
	case COLRAW_A_GET_GAMEACTIVITY_RING:
		{
			processingState = eProcessingRing;
			obj.processAmpsGameActivity_RING_GotRawData(parser, *this);
			break;
		}
	default:
		{
			PLog("Error: Unknown reply id type %u. Reply not handled.", replyId);
			break;
		}
	}
}

// PYR-64045
void AmpsGameActivityAsyncCall::postErrorReply(INT16 errCode, const char* errDesc) const
{
	CommMsgBody reply;
	reply.composeUINT32(reportItem.reqId);

	setReply(reply, errCode, errDesc);

	CommServerConnection* pConn = obj.findConnection(reportItem.connId);
	postReply(pConn, reportItem.replyId, reply);
}

// PYR-64045
void AmpsGameActivityAsyncCall::postResultReply() const
{
	PLog("AmpsGameActivityAsyncCall::postResultReply");
	PLog("sng recs by user size: %u", finalSngOutputs.numTournsByUserMap.size());
	PLog("sng recs with partner size: %u", finalSngOutputs.records.size());
	PLog("ring recs by user size: %u", finalRingOutputs.numHandsByUserMap.size());
	PLog("ring recs with partner size: %u", finalRingOutputs.records.size());

	obj.insertAmpsSngGameActivityCache(reportItem.jobState.requestedInputs, finalSngOutputs);
	obj.insertAmpsRingGameActivityCache(reportItem.jobState.requestedInputs, finalRingOutputs);

	CommMsgBody reply;
	reply.
		composeUINT32(reportItem.reqId).
		composeINT16(DBM_NO_ERROR);
	composeVectorOfComposableValues(finalSngOutputs.records, reply);
	composeInt32PstringMap(reply, finalSngOutputs.numTournsByUserMap);
	composeVectorOfComposableValues(finalRingOutputs.records, reply);
	composeInt32PstringMap(reply, finalRingOutputs.numHandsByUserMap);
	reply.
		composeSrvDate(finalSngOutputs.dateRange.first).
		composeSrvDate(finalSngOutputs.dateRange.second).
		composeSrvDate(finalRingOutputs.dateRange.first).
		composeSrvDate(finalRingOutputs.dateRange.second);

	CommServerConnection* pConn = obj.findConnection(reportItem.connId);
	postReply(pConn, reportItem.replyId, reply);
}

// PYR-64045
bool AmpsGameActivityAsyncCall::isAdminStillConnected() const
{
	CommServerConnection* pConn = obj.findConnection(reportItem.connId);
	return pConn != nullptr;
}

// PYR-64045
void AmpsGameActivityAsyncCall::composeJobState(CommMsgBody& body) const
{
	reportItem.composeJobState(body);
}

// PYR-64045
void AmpsGameActivityAsyncCall::mergeSngAggAndRawOutputs()
{
	finalSngOutputs.records.clear();
	finalSngOutputs.numTournsByUserMap.clear();

	// We expect very few partners, so loop inside loop should be OK

	for (const auto& aggByCurrencyRec : aggSngOutputs.withPartnerRecords)
	{
		INT64 withPartnerAmtWonCents = obj.convertCurrencyM(aggByCurrencyRec.withPartnerAmtWonCents, aggByCurrencyRec.currency, CONV_RATE_DEFAULT_CURRENCY);
		ColAmps::SngGameActivityRecord newSngRec(withPartnerAmtWonCents, aggByCurrencyRec);

		bool found = false;
		for (auto& curSngRec : finalSngOutputs.records)
		{
			if (newSngRec.hasSameKey(curSngRec))
			{
				found = true;
				curSngRec.merge(newSngRec);
			}
		}

		if (!found)
		{
			finalSngOutputs.records.push_back(newSngRec);
		}
	}

	for (const auto& rawByCurrencyRec : rawSngOutputs.withPartnerRecords)
	{
		INT64 withPartnerAmtWonCents = obj.convertCurrencyM(rawByCurrencyRec.withPartnerAmtWonCents, rawByCurrencyRec.currency, CONV_RATE_DEFAULT_CURRENCY);
		ColAmps::SngGameActivityRecord newSngRec(withPartnerAmtWonCents, rawByCurrencyRec);

		bool found = false;
		for (auto& curSngRec : finalSngOutputs.records)
		{
			if (newSngRec.hasSameKey(curSngRec))
			{
				found = true;
				curSngRec.merge(newSngRec);
			}
		}

		if (!found)
		{
			finalSngOutputs.records.push_back(newSngRec);
		}
	}

	for (const auto& aggTournsByUserEntry : aggSngOutputs.numTournsByUserMap)
	{
		const PString& userId = aggTournsByUserEntry.first;
		INT32 numTourns = aggTournsByUserEntry.second;

		finalSngOutputs.numTournsByUserMap.insert(make_pair(userId, numTourns));
	}

	for (const auto& rawTournsByUserEntry : rawSngOutputs.numTournsByUserMap)
	{
		const PString& userId = rawTournsByUserEntry.first;
		INT32 numTourns = rawTournsByUserEntry.second;

		auto it = finalSngOutputs.numTournsByUserMap.find(userId);
		if (it == finalSngOutputs.numTournsByUserMap.end())
		{
			finalSngOutputs.numTournsByUserMap.insert(make_pair(userId, numTourns));
		}
		else
		{
			INT32& numTournsThisUser = it->second;
			numTournsThisUser += numTourns;
		}
	}

	vector<ColAmps::SngGameActivityRecord> inverseRecords;
	for (const auto& record : finalSngOutputs.records)
	{
		ColAmps::SngGameActivityRecord inverseRecord;

		inverseRecord = record;
		swap(inverseRecord.userId, inverseRecord.partnerId);
		inverseRecords.push_back(inverseRecord);
	}
	copy(inverseRecords.begin(), inverseRecords.end(), back_inserter(finalSngOutputs.records));
}

// PYR-64045
void AmpsGameActivityAsyncCall::mergeRingAggAndRawOutputs()
{
	finalRingOutputs.records.clear();
	finalRingOutputs.numHandsByUserMap.clear();

	// We expect very few partners, so loop inside loop should be OK

	for (const auto& aggByCurrencyRec : aggRingOutputs.withPartnerRecords)
	{
		INT64 withPartnerAmtWonCents = obj.convertCurrencyM(aggByCurrencyRec.withPartnerAmtWonCents, aggByCurrencyRec.currency, CONV_RATE_DEFAULT_CURRENCY);
		ColAmps::RingGameActivityRecord newRingRec(withPartnerAmtWonCents, aggByCurrencyRec);

		bool found = false;
		for (auto& curRingRec : finalRingOutputs.records)
		{
			if (newRingRec.hasSameKey(curRingRec))
			{
				found = true;
				curRingRec.merge(newRingRec);
			}
		}

		if (!found)
		{
			finalRingOutputs.records.push_back(newRingRec);
		}
	}

	for (const auto& rawByCurrencyRec : rawRingOutputs.withPartnerRecords)
	{
		INT64 withPartnerAmtWonCents = obj.convertCurrencyM(rawByCurrencyRec.withPartnerAmtWonCents, rawByCurrencyRec.currency, CONV_RATE_DEFAULT_CURRENCY);
		ColAmps::RingGameActivityRecord newRingRec(withPartnerAmtWonCents, rawByCurrencyRec);

		bool found = false;
		for (auto& curRingRec : finalRingOutputs.records)
		{
			if (newRingRec.hasSameKey(curRingRec))
			{
				found = true;
				curRingRec.merge(newRingRec);
			}
		}

		if (!found)
		{
			finalRingOutputs.records.push_back(newRingRec);
		}
	}

	for (const auto& aggHandsByUserEntry : aggRingOutputs.numHandsByUserMap)
	{
		const PString& userId = aggHandsByUserEntry.first;
		INT32 numHands = aggHandsByUserEntry.second;

		finalRingOutputs.numHandsByUserMap.insert(make_pair(userId, numHands));
	}

	for (const auto& rawHandsByUserEntry : rawRingOutputs.numHandsByUserMap)
	{
		const PString& userId = rawHandsByUserEntry.first;
		INT32 numHands = rawHandsByUserEntry.second;

		auto it = finalRingOutputs.numHandsByUserMap.find(userId);
		if (it == finalRingOutputs.numHandsByUserMap.end())
		{
			finalRingOutputs.numHandsByUserMap.insert(make_pair(userId, numHands));
		}
		else
		{
			it->second += numHands;
		}
	}

	vector<ColAmps::RingGameActivityRecord> inverseRecords;
	for (const auto& record : finalRingOutputs.records)
	{
		ColAmps::RingGameActivityRecord inverseRecord;

		inverseRecord = record;
		swap(inverseRecord.userId, inverseRecord.partnerId);
		inverseRecords.push_back(inverseRecord);
	}
	copy(inverseRecords.begin(), inverseRecords.end(), back_inserter(finalRingOutputs.records));
}

// PYR-64045
CommServerConnection* AmpsGameActivityAsyncCall::findConnection() const
{
	return obj.findConnection(reportItem.connId);
}

////////////////////////////////////////////////////////////////////////////////

// PYR-64045
void ColUtilServerObject::processAmpsGamesTogether(UINT32 reqId, CommMsgParser& parser, CommServerConnection* pConn)
{
	PLog("%s", __FUNCTION__);

	time_t now = time(nullptr);

	AdminSrvCon* adminConn = dynamic_cast<AdminSrvCon*>(pConn);
	if (adminConn == nullptr)
	{
		PLog("Lost admin connection, aborting");
		return;
	}

	ColAmps::GamesTogetherInputs inputs(parser);

	ColAmps::SngGamesTogetherOutputs sngOutputs;
	ColAmps::RingGamesTogetherOutputs ringOutputs;

	if (reportCaches.getIfPresent(inputs, sngOutputs) && reportCaches.getIfPresent(inputs, ringOutputs))
	{
		CommMsgBody reply;
		reply.
			composeUINT32(reqId).
			composeINT16(DBM_NO_ERROR);
		sngOutputs.compose(reply);
		ringOutputs.compose(reply);

		postReply(pConn, COLSRV_A_AMPS_GAMES_TOGETHER, reply);

		return;
	}

	time_t timeoutTime = now + getAmpsGamesTogetherTimeoutSeconds;
	ColAmps::GamesTogetherJobState jobState(timeoutTime, inputs);

	unique_ptr<FastReportItemBase> reportItem = make_unique<AmpsGamesTogetherFastReport>(reqId, adminConn->id(), COLSRV_A_AMPS_GAMES_TOGETHER, adminConn->getAdminId(), *this, jobState);

	reportItem->log();

	bool res = fastReportQueue.pushBack(move(reportItem));
	if (!res)
	{
		PLog("Queue full, aborting");

		CommMsgBody reply;
		reply.composeUINT32(reqId).composeINT16(TOO_MANY_ITEMS).composeString("Queue full");
		adminConn->post(COLSRV_A_AMPS_GAMES_TOGETHER, reply);

		return; // No more processing
	}

	res = fastReportQueue.submitNext();
	if (!res)
	{
		PLog("Max submissions reached, waiting");
	}
}

// PYR-64045
bool ColUtilServerObject::processAmpsGamesTogether_PreProcessing(const char* logStr, const AmpsGamesTogetherAsyncCall& callState)
{
	PLog("%s", logStr);

	const AmpsGamesTogetherFastReport& reportItem = callState.reportItem;
	reportItem.log();

	if (!callState.isAdminStillConnected())
	{
		PLog("lost admin connection, aborting");
		fastReportQueue.reportJobComplete(reportItem.uniqueId);
		return false;
	}
	else if (reportItem.isTimeout())
	{
		callState.postErrorReply(DBM_QUERY_TIMEOUT, "Timed out");
		PLog("job timed out, aborting");
		fastReportQueue.reportJobComplete(reportItem.uniqueId);
		return false;
	}
	else
	{
		return true;
	}
}

// PYR-64045
void ColUtilServerObject::processAmpsGamesTogether_Submitted(const AmpsGamesTogetherFastReport& reportItem)
{
	PLog("%s", __FUNCTION__);
	reportItem.log();

	CommServerConnection* pConn = findConnection(reportItem.connId);
	if (pConn == nullptr)
	{
		PLog("Lost admin connection, aborting");
		fastReportQueue.reportJobComplete(reportItem.uniqueId);

		return;
	}

	CommMsgBody body;
	reportItem.composeJobState(body);

	postToColReportingAgg(COLAGG_Q_GET_AMPS_GAMESTOGETHER_SNG, body, new AmpsGamesTogetherAsyncCall(*this, reportItem));
}

// PYR-64045
void ColUtilServerObject::processAmpsGamesTogether_SNG_GotAggData(CommMsgParser& parser, AmpsGamesTogetherAsyncCall& callState)
{
	if (!processAmpsGamesTogether_PreProcessing(__FUNCTION__, callState))
	{
		return;
	}

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	if (errCode != DBM_NO_ERROR)
	{
		const char* errDesc;
		parser.parseString(errDesc);
		PLog("Error: %s", errDesc);

		callState.postErrorReply(errCode, errDesc);

		fastReportQueue.reportJobComplete(callState.reportItem.uniqueId);

		return;
	}

	ColUtils::SngGamesTogetherAggOutputs& aggOutputs = callState.aggSngOutputs;
	aggOutputs.parse(parser);

	ColAmps::GamesTogetherJobState& jobState = callState.reportItem.jobState;

	if (!jobState.inputs.includeUTMSNG)
	{
		callState.mergeSngAggAndRawOutputs();

		PLog("Not getting SNG UTM data, get RING data");
		CommMsgBody body;
		callState.composeJobState(body);
		postToColReportingAgg(COLAGG_Q_GET_AMPS_GAMESTOGETHER_RING, body, &callState);

		return;
	}

	jobState.setSngRawInputPeriod(aggOutputs.aggDateRange);

	const ColUtils::TimeRange& sngRawTimeRange = jobState.sngRawTimeRange;
	if (!sngRawTimeRange.start.isNull() &&
		!sngRawTimeRange.end.isNull() &&
		sngRawTimeRange.start.encode() < sngRawTimeRange.end.encode())
	{
		CommMsgBody body;
		jobState.compose(body);

		postToColReportingRaw(COLRAW_Q_GET_AMPS_GAMESTOGETHER_SNG, body, &callState);
	}
	else
	{
		callState.mergeSngAggAndRawOutputs();

		PLog("Not getting SNG UTM data, get RING data");
		CommMsgBody body;
		callState.composeJobState(body);
		postToColReportingAgg(COLAGG_Q_GET_AMPS_GAMESTOGETHER_RING, body, &callState);

		return;
	}
}

// PYR-64045
void ColUtilServerObject::processAmpsGamesTogether_SNG_GotRawData(CommMsgParser& parser, AmpsGamesTogetherAsyncCall& callState)
{
	if (!processAmpsGamesTogether_PreProcessing(__FUNCTION__, callState))
	{
		return;
	}

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	if (errCode != DBM_NO_ERROR)
	{
		const char* errDesc;
		parser.parseString(errDesc);
		PLog("Error: %s", errDesc);

		callState.postErrorReply(errCode, errDesc);

		fastReportQueue.reportJobComplete(callState.reportItem.uniqueId);

		return;
	}

	vector<ColUtils::SngGamesTogetherRecord>& rawOutputVec = callState.rawSngOutputVec;
	parseVector(parser, rawOutputVec);

	callState.mergeSngAggAndRawOutputs();

	CommMsgBody body;
	callState.composeJobState(body);
	postToColReportingAgg(COLAGG_Q_GET_AMPS_GAMESTOGETHER_RING, body, &callState);
}

// PYR-64045
void ColUtilServerObject::processAmpsGamesTogether_RING_GotAggData(CommMsgParser& parser, AmpsGamesTogetherAsyncCall& callState)
{
	if (!processAmpsGamesTogether_PreProcessing(__FUNCTION__, callState))
	{
		return;
	}

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	if (errCode != DBM_NO_ERROR)
	{
		const char* errDesc;
		parser.parseString(errDesc);
		PLog("Error: %s", errDesc);

		callState.postErrorReply(errCode, errDesc);

		fastReportQueue.reportJobComplete(callState.reportItem.uniqueId);

		return;
	}

	ColUtils::RingGamesTogetherAggOutputs& aggOutputs = callState.aggRingOutputs;
	aggOutputs.parse(parser);

	ColAmps::GamesTogetherJobState& jobState = callState.reportItem.jobState;

	if (!jobState.inputs.includeUTMRing)
	{
		callState.mergeRingAggAndRawOutputs();

		PLog("Not getting RING UTM data, done.");

		callState.postResultReply();
		fastReportQueue.reportJobComplete(callState.reportItem.uniqueId);

		return;
	}

	const DateRange& aggDateRange = aggOutputs.aggDateRange;
	jobState.setRingRawInputPeriod(aggDateRange);

	ColUtils::TimeRange& ringRawTimeRange = jobState.ringRawTimeRange;
	if (!ringRawTimeRange.start.isNull() &&
		!ringRawTimeRange.end.isNull() &&
		ringRawTimeRange.start.encode() < ringRawTimeRange.end.encode())
	{
		CommMsgBody body;
		jobState.compose(body);

		postToColReportingRaw(COLRAW_Q_GET_AMPS_GAMESTOGETHER_RING, body, &callState);
	}
	else
	{
		callState.mergeRingAggAndRawOutputs();

		PLog("Not getting RING UTM data, done.");

		callState.postResultReply();
		fastReportQueue.reportJobComplete(callState.reportItem.uniqueId);

		return;
	}
}

// PYR-64045
void ColUtilServerObject::processAmpsGamesTogether_RING_GotRawData(CommMsgParser& parser, AmpsGamesTogetherAsyncCall& callState)
{
	if (!processAmpsGamesTogether_PreProcessing(__FUNCTION__, callState))
	{
		return;
	}

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	if (errCode != DBM_NO_ERROR)
	{
		const char* errDesc;
		parser.parseString(errDesc);
		PLog("Error: %s", errDesc);

		callState.postErrorReply(errCode, errDesc);

		fastReportQueue.reportJobComplete(callState.reportItem.uniqueId);

		return;
	}

	vector<ColUtils::RingGamesTogetherRecord>& rawOutputVec = callState.rawRingOutputVec;
	parseVector(parser, rawOutputVec);

	callState.mergeRingAggAndRawOutputs();

	callState.postResultReply();
	fastReportQueue.reportJobComplete(callState.reportItem.uniqueId);
}

// PYR-64045
AmpsGamesTogetherAsyncCall::AmpsGamesTogetherAsyncCall(ColUtilServerObject& obj_, const AmpsGamesTogetherFastReport& reportItem_)
	: obj(obj_)
	, reportItem(reportItem_)
{

}

// PYR-64045
AmpsGamesTogetherAsyncCall::~AmpsGamesTogetherAsyncCall()
{
	PTRACE5("~AmpsGamesTogetherAsyncCall");
}

// PYR-64045
void AmpsGamesTogetherAsyncCall::processReply(UINT32 requestId, UINT32 replyId, const CommMsgBody& body)
{
	CommMsgParser parser(body);

	switch (replyId)
	{
	case COLAGG_A_GET_AMPS_GAMESTOGETHER_SNG:
		{
			obj.processAmpsGamesTogether_SNG_GotAggData(parser, *this);
			break;
		}
	case COLRAW_A_GET_AMPS_GAMESTOGETHER_SNG:
		{
			obj.processAmpsGamesTogether_SNG_GotRawData(parser, *this);
			break;
		}
	case COLAGG_A_GET_AMPS_GAMESTOGETHER_RING:
		{
			obj.processAmpsGamesTogether_RING_GotAggData(parser, *this);
			break;
		}
	case COLRAW_A_GET_AMPS_GAMESTOGETHER_RING:
		{
			obj.processAmpsGamesTogether_RING_GotRawData(parser, *this);
			break;
		}
	default:
		{
			PLog("Error: Unknown reply id type %u. Reply not handled.", replyId);
			break;
		}
	}
}

// PYR-64045
void AmpsGamesTogetherAsyncCall::postErrorReply(INT16 errCode, const char* errDesc) const
{
	CommMsgBody reply;
	reply.composeUINT32(reportItem.reqId);

	setReply(reply, errCode, errDesc);

	CommServerConnection* pConn = obj.findConnection(reportItem.connId);
	postReply(pConn, reportItem.replyId, reply);
}

// PYR-64045
void AmpsGamesTogetherAsyncCall::postResultReply() const
{
	PLog("AmpsGamesTogetherAsyncCall::postResultReply");
	PLog("sng recs size: %u", (UINT32)sngOutputs.outputVec.size());
	PLog("ring recs size: %u", (UINT32)ringOutputs.outputVec.size());

	const ColAmps::GamesTogetherJobState& jobState = reportItem.jobState;
	const ColAmps::GamesTogetherInputs& inputs = jobState.inputs;

	obj.insertAmpsSngGamesTogetherCache(jobState.inputs, sngOutputs);
	obj.insertAmpsRingGamesTogetherCache(jobState.inputs, ringOutputs);

	CommMsgBody reply;
	reply.
		composeUINT32(reportItem.reqId).
		composeINT16(DBM_NO_ERROR);
	sngOutputs.compose(reply);
	ringOutputs.compose(reply);

	CommServerConnection* pConn = obj.findConnection(reportItem.connId);
	postReply(pConn, reportItem.replyId, reply);
}

// PYR-64045
bool AmpsGamesTogetherAsyncCall::isAdminStillConnected() const
{
	CommServerConnection* pConn = obj.findConnection(reportItem.connId);
	return pConn != nullptr;
}

// PYR-64045
void AmpsGamesTogetherAsyncCall::composeJobState(CommMsgBody& body) const
{
	reportItem.composeJobState(body);
}

// PYR-64045
void AmpsGamesTogetherAsyncCall::mergeSngAggAndRawOutputs()
{
	DateRange& outputDateRange = sngOutputs.dateRange;

	const DateRange& aggDateRange = aggSngOutputs.aggDateRange;
	if (!aggDateRange.first.isNull())
	{
		outputDateRange = aggDateRange;
	}

	ColAmps::GamesTogetherJobState& jobState = reportItem.jobState;
	const ColUtils::TimeRange& rawTimeRange = jobState.sngRawTimeRange;

	if (jobState.inputs.includeUTMSNG)
	{
		if (outputDateRange.first.isNull() && !rawTimeRange.start.isNull())
		{
			outputDateRange.first.decode(rawTimeRange.start.encode());
		}

		if (!rawTimeRange.end.isNull())
		{
			outputDateRange.second.decode(rawTimeRange.end.encode());
		}
	}

	// Copy all aggregated records to output vector
	vector<ColUtils::SngGamesTogetherRecord>& outputVec = sngOutputs.outputVec;
	outputVec.swap(aggSngOutputs.records);

	// We expect very few partners, so loop inside loop should be OK.
	// For each raw record, either merge or append
	for (const auto& rawRec : rawSngOutputVec)
	{
		auto foundIt = find_if(outputVec.begin(), outputVec.end(),
			[&rawRec](ColUtils::SngGamesTogetherRecord& existingRec) {
				return rawRec.key == existingRec.key;
		});

		if (foundIt == sngOutputs.outputVec.end())
		{
			ColUtils::SngGamesTogetherRecord newRec(rawRec);
			sngOutputs.outputVec.push_back(newRec);
		}
		else
		{
			foundIt->merge(rawRec);
		}
	}

	// Don't need intermediate results anymore.
	aggSngOutputs.records.clear();
	rawSngOutputVec.clear();

	// Merge together unwanted SNG game flags
	ColAmps::mergeSngGamesTogetherRecords(sngOutputs.outputVec);
}

// PYR-64045
void AmpsGamesTogetherAsyncCall::mergeRingAggAndRawOutputs()
{
	DateRange& outputDateRange = ringOutputs.dateRange;

	const DateRange& aggDateRange = aggRingOutputs.aggDateRange;
	if (!aggDateRange.first.isNull())
	{
		outputDateRange = aggDateRange;
	}

	ColAmps::GamesTogetherJobState& jobState = reportItem.jobState;
	const ColUtils::TimeRange& rawTimeRange = jobState.ringRawTimeRange;

	if (jobState.inputs.includeUTMRing)
	{
		if (outputDateRange.first.isNull() && !rawTimeRange.start.isNull())
		{
			outputDateRange.first.decode(rawTimeRange.start.encode());
		}

		if (!rawTimeRange.end.isNull())
		{
			outputDateRange.second.decode(rawTimeRange.end.encode());
		}
	}

	// Copy all aggregated records to output vector
	vector<ColUtils::RingGamesTogetherRecord>& outputVec = ringOutputs.outputVec;
	outputVec.swap(aggRingOutputs.records);

	// We expect very few partners, so loop inside loop should be OK.
	// For each raw record, either merge or append
	for (const auto& rawRec : rawRingOutputVec)
	{
		auto foundIt = find_if(outputVec.begin(), outputVec.end(),
			[&rawRec](ColUtils::RingGamesTogetherRecord& existingRec) {
				return rawRec.key == existingRec.key;
		});

		if (foundIt == ringOutputs.outputVec.end())
		{
			ColUtils::RingGamesTogetherRecord newRec(rawRec);
			ringOutputs.outputVec.push_back(newRec);
		}
		else
		{
			foundIt->merge(rawRec);
		}
	}

	// Don't need intermediate results anymore.
	aggRingOutputs.records.clear();
	rawRingOutputVec.clear();
}

// PYR-64045
CommServerConnection* AmpsGamesTogetherAsyncCall::findConnection() const
{
	return obj.findConnection(reportItem.connId);
}

////////////////////////////////////////////////////////////////////////////////

// PYR-82878
void ColUtilServerObject::processGetCdhEventReport(UINT32 reqId, CommMsgParser & parser, CommServerConnection * pConn)
{
	PLog("%s", __FUNCTION__);

	time_t now = time(nullptr);

	AdminSrvCon* adminConn = dynamic_cast<AdminSrvCon*>(pConn);
	if (adminConn == nullptr)
	{
		PLog("lost admin connection, aborting");
		return;
	}

	ColUtils::CdhEventReportFilterEx inputs;
	ColUtils::CdhEventReportFilter& filters = inputs.filters;
	filters.parse(parser);
	inputs.timeoutTime.decode(now + getCdhEventReportTimeoutSeconds);

	for (UINT32& gt : filters.gameTypes)
	{
		BYTE baseGameType = gt & COLOSSUS_GAMETYPE_BASEGAMETYPE;
		BYTE isHiLo = (gt & COLOSSUS_GAMETYPE_HILO_FLAG) == COLOSSUS_GAMETYPE_HILO_FLAG ? 1 : 0;
		gt = translateTableGame(baseGameType, isHiLo);
		if (gt == 0)
		{
			CommMsgBody reply;
			reply.composeUINT32(reqId).composeINT16(INTERNAL_LOGICAL_ERROR).composeString("Bad game type encountered: ").composeINT32(gt);
			adminConn->post(COLSRV_A_GET_CDH_EVENTS, reply);
			return;
		}
	}

	unique_ptr<FastReportItemBase> reportItem = make_unique<ColCdhEventFastReport>(reqId, adminConn->id(), COLSRV_A_GET_CDH_EVENTS, adminConn->getAdminId(), *this, inputs);

	bool res = fastReportQueue.pushBack(move(reportItem));
	if (!res)
	{
		PLog("queue full, aborting");

		CommMsgBody reply;
		reply.composeUINT32(reqId).composeINT16(TOO_MANY_ITEMS).composeString("Queue full");
		adminConn->post(COLSRV_A_GET_CDH_EVENTS, reply);

		return; // No more processing
	}

	res = fastReportQueue.submitNext();
	if (!res)
	{
		PLog("max submissions reached, waiting");
	}
}

// PYR-82878
void ColUtilServerObject::processGetCdhEventReport_Submitted(const ColCdhEventFastReport& reportItem)
{
	PLog("processGetCdhEventReport_Submitted");
	reportItem.log();

	CommMsgBody body;
	reportItem.compose(body);
	postToColReportingRaw(COLRAW_Q_GET_CDH_EVENT_REPORT, body,
		new GetCdhEventsAsyncCall(this, reportItem.connId, reportItem.reqId, reportItem.replyId, &ColUtilServerObject::processGetCdhEventReport_GotData, reportItem));
}

// PYR-82878
void ColUtilServerObject::processGetCdhEventReport_GotData(CommMsgParser& parser, GetCdhEventsAsyncCall* pCall)
{
	PLog("%s", __FUNCTION__);

	ColCdhEventFastReport& reportState = pCall->data;
	reportState.log();

	fastReportQueue.reportJobComplete(reportState.uniqueId);

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	if (errCode != DBM_NO_ERROR)
	{
		const char* errBuf;
		parser.parseString(errBuf);

		PLog("Error: %s", errBuf);
		pCall->postReply(errCode, errBuf);

		return;	// No more processing
	}

	CommMsgBody body;
	body.copyFrom(parser.getMsgBody()); // Copy the whole message body, not just unparsed part
	pCall->postReply(body);
}

////////////////////////////////////////////////////////////////////////////////

// PYR-89537
void ColUtilServerObject::processGetNormalBPSReport(UINT32 reqId, CommMsgParser& parser, AdminSrvCon& adminConn)
{
	PLog("%s", __FUNCTION__);

	BPS::BPSNormalFilter normalFilter;
	normalFilter.parse(parser);

	PString inputUserIdUtf8 = normalFilter.userId;
	normalFilter.userId.clear();
	i18nUtf8_Psx(normalFilter.userId, inputUserIdUtf8);

	ColUtils::ColAdminReport adminReport;
	adminReport.reportType = AdminReport_RHODES_BPS;
	adminReport.adminid = adminConn.getAdminId();

	// check filters
	if (!normalFilter.isValid())
	{
		CommMsgBody reply;
		reply.composeUINT32(reqId).composeINT16(INTERNAL_LOGICAL_ERROR).composeString("Wrong BPS filters.");
		adminConn.post(COLSRV_A_GET_NORMAL_BPS_REPORT, reply);
		insertAdminReport(adminReport);
		return;
	}

	BPS::BPSReportCacheData bpsReportCacheData;
	if (reportCaches.getIfPresent(normalFilter, bpsReportCacheData))
	{
		PLog("Found in cache, replying");

		BPS::BPSOutputReport outputReport;
		outputReport.createReportWithNormalFilter(bpsReportCacheData, normalFilter);

		CommMsgBody body;
		body.composeUINT32(reqId);
		body.composeINT16(DBM_NO_ERROR);
		outputReport.compose(body);
		
		postReply(&adminConn, COLSRV_A_GET_NORMAL_BPS_REPORT, body);
		insertAdminReport(adminReport);

		return; // No more processing
	}

	unique_ptr<FastReportItemBase> reportItem = make_unique<ColNormalBPSFastReport>(reqId, adminConn.id(), COLSRV_A_GET_NORMAL_BPS_REPORT, adminConn.getAdminId(), *this, normalFilter, adminReport);

	bool res = fastReportQueue.pushBack(move(reportItem));
	if (!res)
	{
		PLog("queue full, aborting");

		CommMsgBody reply;
		reply.composeUINT32(reqId).composeINT16(TOO_MANY_ITEMS).composeString("Queue full");
		adminConn.post(COLSRV_A_GET_NORMAL_BPS_REPORT, reply);
		insertAdminReport(adminReport);

		return; // No more processing
	}

	res = fastReportQueue.submitNext();
	if (!res)
	{
		PLog("max submissions reached, waiting");
	}
}

void ColUtilServerObject::processGetNormalBPSReport_Submitted(const ColNormalBPSFastReport& reportItem)
{
	PLog("processGetNormalBPSReport_Submitted");
	reportItem.log();

	ColUtils::ColDBMUserBPSRequest bpsRequest;
	bpsRequest.userId = reportItem.inputs.userId;
	bpsRequest.startDate = reportItem.inputs.startDate;
	bpsRequest.endDate = reportItem.inputs.endDate;
	bpsRequest.aggType = reportItem.inputs.aggType;

	CommMsgBody body;
	bpsRequest.compose(body);
	postToColReportingAgg(COL_Q_GET_USER_BPS, body,
		new GetNormalBPSAsyncCall(this, reportItem.connId, reportItem.reqId, COL_A_GET_USER_BPS, &ColUtilServerObject::processGetNormalBPSReport_GotData, reportItem));
}

void ColUtilServerObject::processGetNormalBPSReport_GotData(CommMsgParser& parser, GetNormalBPSAsyncCall* pCall)
{
	PLog("%s", __FUNCTION__);

	ColNormalBPSFastReport& reportState = pCall->data;
	reportState.log();

	fastReportQueue.reportJobComplete(reportState.uniqueId);

	insertAdminReport(reportState.adminReport);

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	if (errCode != DBM_NO_ERROR)
	{
		const char* errBuf;
		parser.parseString(errBuf);

		PLog("Error: %s", errBuf);
		pCall->postReply(errCode, errBuf);

		return;	// No more processing
	}

	ColUtils::ColDBMUserBPSResult bpsResult;
	bpsResult.parse(parser);
	
	// merge HandStats and TournROI
	BPS::BPSReportCacheData bpsReportCacheData;
	bpsReportCacheData.actualFrom = bpsResult.actualFrom;
	bpsReportCacheData.actualTo = bpsResult.actualTo;
	bpsReportCacheData.addColDBMBPSResult(bpsResult);

	BPS::BPSNormalFilter& normalFilter = reportState.inputs;
	reportCaches.putIfAbsent(normalFilter, bpsReportCacheData);

	BPS::BPSOutputReport outputReport;
	outputReport.createReportWithNormalFilter(bpsReportCacheData, normalFilter);

	CommMsgBody body;
	body.composeINT16(DBM_NO_ERROR);
	outputReport.compose(body);
	pCall->postReply(body);
}


void ColUtilServerObject::processGetXStakesBPSReport(UINT32 reqId, CommMsgParser& parser, AdminSrvCon& adminConn)
{
	PLog("%s", __FUNCTION__);

	BPS::BPSXStakesFilter xstakesFilter;
	xstakesFilter.parse(parser);

	PString inputUserIdUtf8 = xstakesFilter.userId;
	xstakesFilter.userId.clear();
	i18nUtf8_Psx(xstakesFilter.userId, inputUserIdUtf8);

	ColUtils::ColAdminReport adminReport;
	adminReport.reportType = AdminReport_RHODES_BPS;
	adminReport.adminid = adminConn.getAdminId();

	// check filters
	if (!xstakesFilter.isValid())
	{
		CommMsgBody reply;
		reply.composeUINT32(reqId).composeINT16(INTERNAL_LOGICAL_ERROR).composeString("Wrong BPS filters.");
		adminConn.post(COLSRV_A_GET_XSTAKES_BPS_REPORT, reply);
		insertAdminReport(adminReport);
		return;
	}

	BPS::BPSReportCacheData bpsReportCacheData;
	if (reportCaches.getIfPresent(xstakesFilter, bpsReportCacheData))
	{
		PLog("Found in cache, replying");

		BPS::BPSOutputReport outputReport;
		outputReport.createReportWithXStakesFilter(bpsReportCacheData, xstakesFilter);

		CommMsgBody body;
		body.composeUINT32(reqId);
		body.composeINT16(DBM_NO_ERROR);
		outputReport.compose(body);

		postReply(&adminConn, COLSRV_A_GET_XSTAKES_BPS_REPORT, body);
		insertAdminReport(adminReport);
		return; // No more processing
	}


	unique_ptr<FastReportItemBase> reportItem = make_unique<ColXStakesBPSFastReport>(reqId, adminConn.id(), COLSRV_A_GET_XSTAKES_BPS_REPORT, adminConn.getAdminId(), *this, xstakesFilter, adminReport);

	bool res = fastReportQueue.pushBack(move(reportItem));
	if (!res)
	{
		PLog("queue full, aborting");

		CommMsgBody reply;
		reply.composeUINT32(reqId).composeINT16(TOO_MANY_ITEMS).composeString("Queue full");
		adminConn.post(COLSRV_A_GET_XSTAKES_BPS_REPORT, reply);
		insertAdminReport(adminReport);
		return; // No more processing
	}

	res = fastReportQueue.submitNext();
	if (!res)
	{
		PLog("max submissions reached, waiting");
	}
}

void ColUtilServerObject::processGetXStakesBPSReport_Submitted(const ColXStakesBPSFastReport& reportItem)
{
	PLog("processGetNormalBPSReport_Submitted");
	reportItem.log();

	ColUtils::ColDBMUserBPSRequest bpsRequest;
	bpsRequest.userId = reportItem.inputs.userId;
	bpsRequest.startDate = reportItem.inputs.startDate;
	bpsRequest.endDate = reportItem.inputs.endDate;
	bpsRequest.aggType = reportItem.inputs.aggType;

	CommMsgBody body;
	bpsRequest.compose(body);
	postToColReportingAgg(COL_Q_GET_USER_BPS, body,
		new GetXStakesBPSAsyncCall(this, reportItem.connId, reportItem.reqId, COL_A_GET_USER_BPS, &ColUtilServerObject::processGetXStakesBPSReport_GotData, reportItem));
}

void ColUtilServerObject::processGetXStakesBPSReport_GotData(CommMsgParser& parser, GetXStakesBPSAsyncCall* pCall)
{
	PLog("%s", __FUNCTION__);

	ColXStakesBPSFastReport& reportState = pCall->data;
	reportState.log();

	fastReportQueue.reportJobComplete(reportState.uniqueId);

	insertAdminReport(reportState.adminReport);

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	if (errCode != DBM_NO_ERROR)
	{
		const char* errBuf;
		parser.parseString(errBuf);

		PLog("Error: %s", errBuf);
		pCall->postReply(errCode, errBuf);

		return;	// No more processing
	}

	ColUtils::ColDBMUserBPSResult bpsResult;
	bpsResult.parse(parser);

	BPS::BPSReportCacheData bpsReportCacheData;
	bpsReportCacheData.actualFrom = bpsResult.actualFrom;
	bpsReportCacheData.actualTo = bpsResult.actualTo;
	bpsReportCacheData.addColDBMBPSResult(bpsResult);

	BPS::BPSXStakesFilter& xstakesFilter = reportState.inputs;
	reportCaches.putIfAbsent(xstakesFilter, bpsReportCacheData);

	BPS::BPSOutputReport outputReport;
	outputReport.createReportWithXStakesFilter(bpsReportCacheData, xstakesFilter);

	CommMsgBody body;
	body.composeINT16(DBM_NO_ERROR);
	outputReport.compose(body);
	pCall->postReply(body);
}

////////////////////////////////////////////////////////////////////////////////

// PYR-89537
void ColUtilServerObject::processGetBpsUserStakedPlayedReport(UINT32 reqId, CommMsgParser& parser, CommServerConnection* pConn)
{
	PLog("%s", __FUNCTION__);

	time_t now = time(nullptr);

	AdminSrvCon* adminConn = dynamic_cast<AdminSrvCon*>(pConn);
	if (adminConn == nullptr)
	{
		PLog("lost admin connection, aborting");
		return;
	}

	BPS::UserStakesPlayedInputsEx inputsEx;
	BPS::UserStakesPlayedInputs& inputs = inputsEx.inputs;
	inputs.parse(parser);

	PString inputUserIdUtf8 = inputs.userId;
	inputs.userId.clear();
	i18nUtf8_Psx(inputs.userId, inputUserIdUtf8);

	BPS::UserStakesPlayedOutputs outputs;

	if (reportCaches.getIfPresent(inputs, outputs))
	{
		PLog("Found in cache, replying");

		BPS::FilteredUserStakesPlayed filteredStakes;
		filteredStakes.actualFrom = outputs.dateRange.first;
		filteredStakes.actualTo = outputs.dateRange.second;
		filteredStakes.calculateStakesFilters(inputs, outputs.recordVec);

		CommMsgBody reply;
		reply.
			composeUINT32(reqId).
			composeINT16(DBM_NO_ERROR);
		filteredStakes.compose(reply);

		postReply(pConn, COLSRV_A_GET_USER_STAKES_PLAYED, reply);

		return; // No more processing
	}

	inputsEx.timeoutTime.decode(now + getBpsUserStakesPlayedTimeoutSeconds);

	unique_ptr<FastReportItemBase> reportItem = make_unique<ColBpsUserStakesPlayedFastReport>(reqId, adminConn->id(), COLSRV_A_GET_USER_STAKES_PLAYED, adminConn->getAdminId(), *this, inputsEx);

	bool res = fastReportQueue.pushBack(move(reportItem));
	if (!res)
	{
		PLog("queue full, aborting");

		CommMsgBody reply;
		reply.composeUINT32(reqId).composeINT16(TOO_MANY_ITEMS).composeString("Queue full");
		adminConn->post(COLSRV_A_GET_USER_STAKES_PLAYED, reply);

		return; // No more processing
	}

	res = fastReportQueue.submitNext();
	if (!res)
	{
		PLog("max submissions reached, waiting");
	}
}

// PYR-89537
void ColUtilServerObject::processGetBpsUserStakedPlayedReport_Submitted(const ColBpsUserStakesPlayedFastReport& reportItem)
{
	PLog("%s", __FUNCTION__);
	reportItem.log();

	CommMsgBody body;
	reportItem.compose(body);
	postToColReportingAgg(COL_Q_GET_USER_STAKES_PLAYED, body,
		new GetBpsUserStakesPlayedAsyncCall(this, reportItem.connId, reportItem.reqId, reportItem.replyId, &ColUtilServerObject::processGetBpsUserStakedPlayedReport_GotData, reportItem));
}

// PYR-89537
void ColUtilServerObject::processGetBpsUserStakedPlayedReport_GotData(CommMsgParser& parser, GetBpsUserStakesPlayedAsyncCall* pCall)
{
	PLog("%s", __FUNCTION__);

	ColBpsUserStakesPlayedFastReport& reportState = pCall->data;
	reportState.log();

	fastReportQueue.reportJobComplete(reportState.uniqueId);

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	if (errCode != DBM_NO_ERROR)
	{
		const char* errBuf;
		parser.parseString(errBuf);

		PLog("Error: %s", errBuf);
		pCall->postReply(errCode, errBuf);

		return;	// No more processing
	}

	BPS::UserStakesPlayedOutputs outputs(parser);
	PTRACE("Num records from DBM: %u", (UINT32)outputs.recordVec.size());

	BPS::UserStakesPlayedInputsEx& inputsEx = reportState.inputsEx;
	BPS::UserStakesPlayedInputs& inputs = inputsEx.inputs;
	
	reportCaches.putIfAbsent(inputs, outputs);

	BPS::FilteredUserStakesPlayed filteredStakes;
	filteredStakes.actualFrom = outputs.dateRange.first;
	filteredStakes.actualTo = outputs.dateRange.second;
	filteredStakes.calculateStakesFilters(inputs, outputs.recordVec);
	PTRACE("Num records post-filters: %u", (UINT32)filteredStakes.stakesVec.size());

	CommMsgBody body;
	body.composeINT16(DBM_NO_ERROR);
	filteredStakes.compose(body);
	pCall->postReply(body);
}

////////////////////////////////////////////////////////////////////////////////

// PYR-89537
void ColUtilServerObject::processGetAggTableExtentsReport(UINT32 reqId, CommMsgParser& parser, CommServerConnection* pConn)
{
	PLog("%s", __FUNCTION__);

	time_t now = time(nullptr);

	AdminSrvCon* adminConn = dynamic_cast<AdminSrvCon*>(pConn);
	if (adminConn == nullptr)
	{
		PLog("lost admin connection, aborting");
		return;
	}

	ColUtils::MaintJobSetupQueryParamsEx inputsEx;
	ColUtils::MaintJobSetupQueryParams& inputs = inputsEx.queryParams;
	inputs.parse(parser);

	ColUtils::MaintJobSetupEntries outputs;

	if (reportCaches.getIfPresent(inputs, outputs))
	{
		PLog("Found in cache, replying");

		CommMsgBody reply;
		reply.
			composeUINT32(reqId).
			composeINT16(DBM_NO_ERROR);
		composeVector(reply, outputs.entryVec);

		postReply(pConn, COLSRV_A_GET_AGG_TABLE_EXTENTS, reply);

		return; // No more processing
	}

	inputsEx.timeoutTime.decode(now + getAggTableExtentsTimeoutSeconds);

	unique_ptr<FastReportItemBase> reportItem = make_unique<ColGetAggTableExtentsFastReport>(reqId, adminConn->id(), COLSRV_A_GET_AGG_TABLE_EXTENTS, adminConn->getAdminId(), *this, inputsEx);

	bool res = fastReportQueue.pushBack(move(reportItem));
	if (!res)
	{
		PLog("queue full, aborting");

		CommMsgBody reply;
		reply.composeUINT32(reqId).composeINT16(TOO_MANY_ITEMS).composeString("Queue full");
		adminConn->post(COLSRV_A_GET_AGG_TABLE_EXTENTS, reply);

		return; // No more processing
	}

	res = fastReportQueue.submitNext();
	if (!res)
	{
		PLog("max submissions reached, waiting");
	}
}

// PYR-89537
void ColUtilServerObject::processGetAggTableExtentsReport_Submitted(const ColGetAggTableExtentsFastReport& reportItem)
{
	PLog("%s", __FUNCTION__);
	reportItem.log();

	CommMsgBody body;
	reportItem.compose(body);
	postToColReportingAgg(COL_Q_GET_AGG_TABLE_EXTENTS, body,
		new GetAggTableExtentsAsyncCall(this, reportItem.connId, reportItem.reqId, reportItem.replyId, &ColUtilServerObject::processGetAggTableExtentsReport_GotData, reportItem));
}

// PYR-89537
void ColUtilServerObject::processGetAggTableExtentsReport_OnSchedule_Submitted(const ColGetAggTableExtentsFastReport& reportItem)
{
	PLog("%s", __FUNCTION__);
	reportItem.log();

	CommMsgBody body;
	reportItem.compose(body);
	postToColReportingAgg(COL_Q_GET_AGG_TABLE_EXTENTS, body,
		new GetAggTableExtentsNoReplyAsyncCall(this, &ColUtilServerObject::processGetAggTableExtentsReport_OnSchedule_GotData, reportItem));
}

// PYR-89537
void ColUtilServerObject::processGetAggTableExtentsReport_GotData(CommMsgParser& parser, GetAggTableExtentsAsyncCall* pCall)
{
	PLog("%s", __FUNCTION__);

	ColGetAggTableExtentsFastReport& reportState = pCall->data;
	reportState.log();

	fastReportQueue.reportJobComplete(reportState.uniqueId);

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	if (errCode != DBM_NO_ERROR)
	{
		const char* errBuf;
		parser.parseString(errBuf);

		PLog("Error: %s", errBuf);
		pCall->postReply(errCode, errBuf);

		return;	// No more processing
	}

	ColUtils::MaintJobSetupEntries outputs;
	vector<ColUtils::MaintJobSetupEntry>& entryVec = outputs.entryVec;
	parseVectorOfParsableMessageBodies(entryVec, parser);

	PTRACE("Num records from DBM: %u", (UINT32)entryVec.size());

	ColUtils::MaintJobSetupQueryParamsEx& inputsEx = reportState.inputsEx;
	ColUtils::MaintJobSetupQueryParams& inputs = inputsEx.queryParams;

	reportCaches.putIfAbsent(inputs, outputs);

	for (const ColUtils::MaintJobSetupEntry& entry : entryVec)
	{
		PString dummy1, dummy2;
		PTRACE("(aggRangeType: %d, aggTableType: %d) from: %s, to: %s",
			entry.aggRangeType, entry.aggTableType,
			srvDateToString(entry.fromDate, dummy1), srvDateToString(entry.toDate, dummy2));
	}

	CommMsgBody reply;
	reply.composeINT16(DBM_NO_ERROR);
	composeVector(reply, outputs.entryVec);

	pCall->postReply(reply);
}

// PYR-89537
void ColUtilServerObject::processGetAggTableExtentsReport_OnSchedule_GotData(CommMsgParser& parser, GetAggTableExtentsNoReplyAsyncCall* pCall)
{
	PLog("%s", __FUNCTION__);

	ColGetAggTableExtentsFastReport& reportState = pCall->data;
	reportState.log();

	fastReportQueue.reportJobComplete(reportState.uniqueId);

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	if (errCode != DBM_NO_ERROR)
	{
		const char* errBuf;
		parser.parseString(errBuf);

		PLog("Error: %s", errBuf);

		return;	// No more processing
	}

	ColUtils::MaintJobSetupEntries outputs;
	vector<ColUtils::MaintJobSetupEntry>& entryVec = outputs.entryVec;
	parseVectorOfParsableMessageBodies(entryVec, parser);

	PTRACE("%sNum records from DBM: %u", entryVec.empty() ? "Warning: " : "", (UINT32)entryVec.size());

	ColUtils::MaintJobSetupQueryParamsEx& inputsEx = reportState.inputsEx;
	ColUtils::MaintJobSetupQueryParams& inputs = inputsEx.queryParams;

	reportCaches.putIfAbsent(inputs, outputs);

	for (const ColUtils::MaintJobSetupEntry& entry : entryVec)
	{
		PString dummy1, dummy2;
		PTRACE("(aggRangeType: %d, aggTableType: %d) from: %s, to: %s",
			entry.aggRangeType, entry.aggTableType,
			srvDateToString(entry.fromDate, dummy1), srvDateToString(entry.toDate, dummy2));
	}

	// For "On Schedule", there is no reply to client
	//
	// CommMsgBody reply;
	// reply.composeINT16(DBM_NO_ERROR);
	// composeVector(reply, outputs.entryVec);
	//
	// pCall->postReply(reply);
}

////////////////////////////////////////////////////////////////////////////////

// PYR-81917
CdaChangeRuleAsyncCallBase::CdaChangeRuleAsyncCallBase(ColUtilServerObject& obj_, UINT32 reqId_, UINT32 connId_, UINT32 replyId_)
	: obj(obj_), reqId(reqId_), connId(connId_), replyToOriginId(replyId_), replyErrCode(DBM_NO_ERROR)
{

}

// PYR-81917
CdaChangeRuleAsyncCallBase::~CdaChangeRuleAsyncCallBase()
{
	PTRACE5("~CdaChangeRuleAsyncCallBase");
}

// PYR-81917
void CdaChangeRuleAsyncCallBase::processReply(UINT32 requestId, UINT32 msgId, const CommMsgBody& body)
{
	PTRACE("CdaChangeRuleAsyncCallBase::processReply(reqId: %u, msgId: %u)", requestId, msgId);

	CommMsgParser parser(body);

	parser.parseINT16(replyErrCode);
	if (replyErrCode != DBM_NO_ERROR)
	{
		parser.parseStringP(replyErrDesc);
		PLog("DBM Error: %s", replyErrDesc.c_str());

		return;
	}

	switch (msgId)
	{
		case COLCDA_CREATE_RULE_REPLY:
		case COLCDA_MODIFY_RULE_REPLY:
		case COLCDA_ACTIVATE_RULE_REPLY:
		case COLCDA_DELETE_RULE_REPLY:
		case COLCDA_SET_VELOCITY_CONTROL_REPLY:
		{
			obj.processCdaChangeRule_CdaDone(parser, this);
			break;
		}
		case COLCDA_REORDER_RULES_REPLY:
		{
			obj.processCdaChangeRule_CdaDoneReorder(parser, this);
			break;
		}
		case DBM_A_GET_INIDATA2:
		{
			obj.processCdaChangeRule_GotEmailList(parser, this);
			break;
		}
		case TS_A_COMMON_SEND_EMAIL:
		{
			obj.processCdaChangeRule_EmailSent(parser, this);
			break;
		}
		default:
		{
			PLog("Error: Unknown reply id type %u. Reply not handled.", msgId);
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

// PYR-81917
CdaChangeRuleAsyncCall::CdaChangeRuleAsyncCall(ColUtilServerObject& obj_, UINT32 reqId_, UINT32 connId_, UINT32 replyId_)
	: CdaChangeRuleAsyncCallBase(obj_, reqId_, connId_, replyId_)
{

}

// PYR-81917
CdaChangeRuleAsyncCall::~CdaChangeRuleAsyncCall()
{
	PTRACE5("~CdaChangeRuleAsyncCall");
	replyToOrigin();
}

// PYR-81917
void CdaChangeRuleAsyncCall::replyToOrigin() const
{
	CommMsgBody reply;
	reply.
		composeUINT32(reqId).
		composeINT16(replyErrCode);

	if (replyErrCode)
	{
		reply.composeString(replyErrDesc);
	}
	else if (!ccdaRuleVec.empty())
	{
		ccdaRuleVec.begin()->compose(reply);
	}

	CommServerConnection* pConn = obj.findConnection(connId);
	postReply(pConn, replyToOriginId, reply);
}

////////////////////////////////////////////////////////////////////////////////

// PYR-81917
CdaReorderRuleAsyncCall::CdaReorderRuleAsyncCall(ColUtilServerObject& obj_, UINT32 reqId_, UINT32 connId_, UINT32 replyId_)
	: CdaChangeRuleAsyncCallBase(obj_, reqId_, connId_, replyId_)
{

}

// PYR-81917
CdaReorderRuleAsyncCall::~CdaReorderRuleAsyncCall()
{
	PTRACE5("~CdaChangeRuleAsyncCall");
	replyToOrigin();
}

// PYR-81917
void CdaReorderRuleAsyncCall::replyToOrigin() const
{
	CommMsgBody reply;
	reply.
		composeUINT32(reqId).
		composeINT16(replyErrCode);

	if (replyErrCode)
	{
		reply.composeString(replyErrDesc);
	}
	else
	{
		composeVectorOfMessageBodiesOfComposableValues(ccdaRuleVec, reply);
	}

	CommServerConnection* pConn = obj.findConnection(connId);
	postReply(pConn, replyToOriginId, reply);
}

////////////////////////////////////////////////////////////////////////////////

// PYR-92626
CdaStartEvalAsyncCall::CdaStartEvalAsyncCall(ColUtilServerObject& obj_, UINT32 reqId_, UINT32 connId_, UINT32 replyId_) 
	: obj(obj_)
	, reqId(reqId_)
	, connId(connId_)
	, replyToOriginId(replyId_)
	, replyErrCode(DBM_NO_ERROR)
{

}

// PYR-92626
void CdaStartEvalAsyncCall::processReply(UINT32 requestId, UINT32 msgId, const CommMsgBody& body)
{
	PTRACE("CdaStartEvalAsyncCall::processReply(reqId: %u, msgId: %u)", requestId, msgId);

	CommMsgParser parser(body);
	parser.parseINT16(replyErrCode);
	if (replyErrCode)
	{
		parser.parseStringP(replyErrDesc);
	}

	switch (msgId)
	{
		case COLCDA_START_EVALUATION_REPLY:
		{
			CommMsgBody reply;
			reply.
				composeUINT32(reqId).
				composeINT16(replyErrCode);

			if (replyErrCode)
			{
				reply.composeString(replyErrDesc);

				PLog("DBM Error: %s", replyErrDesc.c_str());
			}
			else
			{
				UINT32 evaluationStatus = 0;
				parser.parseUINT32(evaluationStatus);
				reply.composeUINT32(evaluationStatus);

				PLog("evaluationStatus: %u", evaluationStatus);
			}

			CommServerConnection* pConn = obj.findConnection(connId);
			postReply(pConn, replyToOriginId, reply);

			break;
		}
		default:
		{
			PLog("Error: Unknown reply id type %u. Reply not handled.", msgId);
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

// PYR-106157
GetUserTotalRakePaidAsyncCall::GetUserTotalRakePaidAsyncCall(ColUtilServerObject& obj_, UINT32 reqId_, UINT32 connId_, 
	const time_t& timeOfRequest_, const ColUtils::GetUserWageringRequest& userWageringRequest_)
	: obj(obj_)
	, reqId(reqId_)
	, connId(connId_)
	, timeOfRequest(timeOfRequest_)
	, userWageringRequest(userWageringRequest_)
{
	++UNIQUE_ID;
	curIteration = UNIQUE_ID;
	PTRACE("GURP Request %u", curIteration);
}

// PYR-106157
GetUserTotalRakePaidAsyncCall::~GetUserTotalRakePaidAsyncCall()
{
	PTRACE("#GURP Request %u", curIteration);
}

// PYR-106157
void GetUserTotalRakePaidAsyncCall::processReply(UINT32 requestId, UINT32 replyMsgId, const CommMsgBody& body)
{
	CommMsgParser parser(body);

	PTRACE("GURP Request %u", curIteration);

	switch (replyMsgId)
	{
		case COL_A_GET_WAGERING_EX:
		{
			obj.processGetUserRakePaid_GotAggData(parser, *this);
			break;
		}
		case COLRAW_A_GET_WAGERING_EX:
		{
			obj.processGetUserRakePaid_GotRawData(parser, *this);
			break;
		}
		default:
		{
			PLog("Error: " __FUNCTION__ " Unknown reply id type %u. Reply not handled.", replyMsgId);
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// PYR-87500
void ColUtilServerObject::processNormalizeUserIds(UINT32 reqId, CommMsgParser& parser, AdminSrvCon& conn)
{
	PLog("%s", __FUNCTION__);

	vector<PString> inputUserIds_Utf8;
	parseStringVector(parser, inputUserIds_Utf8);

	vector<PString> inputUserIds_PStringX;
	for (PString& userId : inputUserIds_Utf8)
	{
		static PString emptyStr;
		
		inputUserIds_PStringX.push_back(emptyStr);
		PString& inputUserId_PStringX = inputUserIds_PStringX.back();
		
		i18nUtf8_Psx(inputUserId_PStringX, userId);

		PTRACE5("- uid_utf8: '%s', uid_psx: '%s'", userId.c_str(), inputUserId_PStringX.c_str());
	}

	CommMsgBody body;
	composeStringVector(body, inputUserIds_PStringX);

	postToColReader(COLOSSUS_NORMALIZE_USERIDS, body,
		new NormalizeUserIdsAsyncCall(this, conn.id(), reqId, COLOSSUS_NORMALIZE_USERIDS_REPLY, &ColUtilServerObject::processNormalizeUserIds_GotData));

}

void ColUtilServerObject::processNormalizeUserIds_GotData(CommMsgParser& parser, NormalizeUserIdsAsyncCall* pCall)
{
	PLog("%s", __FUNCTION__);

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	if (errCode == DBM_NO_ERROR)
	{
		CommMsgBody replyBody;
		replyBody.composeINT16(errCode);
		replyBody.copyMsgBodyFromParser(parser);

		pCall->postReply(replyBody);
	}
	else
	{
		const char* errStr;
		parser.parseString(errStr);
		PLog("Error[%u]-%s", errCode, errStr);

		pCall->postReply(errCode, errStr);
	}
}

////////////////////////////////////////////////////////////////////////////////
// PYR-84380

void ColUtilServerObject::processCreateColWhitelist(UINT32 reqId, CommMsgParser& parser, AdminSrvCon& conn)
{
	PLog("%s", __FUNCTION__);

	CommMsgBody body;
	body.copyFromSkipReqId(parser.getMsgBody());

	postToColAggWriter(COL_Q_COLWHITELIST_CREATE, body,
		new ColWhitelistAsyncCall(this, conn.id(), reqId, COLSRV_A_WHITELIST_CREATE, &ColUtilServerObject::processCreateColWhitelist_GotData));
}

void ColUtilServerObject::processCreateColWhitelist_GotData(CommMsgParser& parser, ColWhitelistAsyncCall* pCall)
{
	PLog("%s", __FUNCTION__);

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	if (errCode == DBM_NO_ERROR)
	{
		ColWhitelist record;
		record.parse(parser);

		CommMsgBody replyBody;
		replyBody.composeINT16(errCode);
		record.compose(replyBody);

		pCall->postReply(replyBody);

		CommMsgBody notifyBody;
		record.compose(notifyBody);
		detectColWhitelistChange(COLWHITELIST_CREATE_RECORD, notifyBody);
	}
	else
	{
		const char* errStr;
		parser.parseString(errStr);
		PLog("Error[%u]-%s", errCode, errStr);

		pCall->postReply(errCode, errStr);
	}
}

void ColUtilServerObject::processModifyColWhitelist(UINT32 reqId, CommMsgParser& parser, AdminSrvCon& conn)
{
	PLog("%s", __FUNCTION__);

	CommMsgBody body;
	body.copyFromSkipReqId(parser.getMsgBody());

	postToColAggWriter(COL_Q_COLWHITELIST_MODIFY, body,
		new ColWhitelistAsyncCall(this, conn.id(), reqId, COLSRV_A_WHITELIST_MODIFY, &ColUtilServerObject::processModifyColWhitelist_GotData));
}

void ColUtilServerObject::processModifyColWhitelist_GotData(CommMsgParser& parser, ColWhitelistAsyncCall* pCall)
{
	PLog("%s", __FUNCTION__);
	
	INT16 errCode = 0;
	parser.parseINT16(errCode);

	if (errCode == DBM_NO_ERROR)
	{
		ColWhitelist record;
		record.parse(parser);

		CommMsgBody replyBody;
		replyBody.composeINT16(errCode);
		record.compose(replyBody);

		pCall->postReply(replyBody);

		CommMsgBody notifyBody;
		record.compose(notifyBody);
		detectColWhitelistChange(COLWHITELIST_MODIFY_RECORD, notifyBody);
	}
	else
	{
		const char* errStr;
		parser.parseString(errStr);
		PLog("Error[%u]-%s", errCode, errStr);
		
		pCall->postReply(errCode, errStr);
	}
}

void ColUtilServerObject::processDeleteColWhitelist(UINT32 reqId, CommMsgParser& parser, AdminSrvCon& conn)
{
	PLog("%s", __FUNCTION__);

	CommMsgBody body;
	body.copyFromSkipReqId(parser.getMsgBody());

	postToColAggWriter(COL_Q_COLWHITELIST_DELETE, body,
		new ColWhitelistAsyncCall(this, conn.id(), reqId, COLSRV_A_WHITELIST_DELETE, &ColUtilServerObject::processDeleteColWhitelist_GotData));
}

void ColUtilServerObject::processDeleteColWhitelist_GotData(CommMsgParser& parser, ColWhitelistAsyncCall* pCall)
{
	PLog("%s", __FUNCTION__);
	INT16 errCode = 0;
	parser.parseINT16(errCode);

	if (errCode == DBM_NO_ERROR)
	{
		ColWhitelist record;
		record.parse(parser);

		CommMsgBody replyBody;
		replyBody.composeINT16(errCode);
		record.compose(replyBody);

		pCall->postReply(replyBody);

		CommMsgBody notifyBody;
		record.compose(notifyBody);
		detectColWhitelistChange(COLWHITELIST_DELETE_RECORD, notifyBody);
	}
	else
	{
		const char* errStr;
		parser.parseString(errStr);
		PLog("Error[%u]-%s", errCode, errStr);

		pCall->postReply(errCode, errStr);
	}
}

// Notify colossus modules.
void ColUtilServerObject::detectColWhitelistChange(UINT32 msgId, const CommMsgBody& body)
{
	CommMsgBody ccdaBody;
	ccdaBody.copyFrom(body);
	postToCCDA(msgId, ccdaBody, nullptr);

	CommMsgBody cdhBody;
	cdhBody.copyFrom(body);
	postToCDH(msgId, cdhBody, nullptr);

	CommMsgBody tcaBody;
	tcaBody.copyFrom(body);
	postToTCA(msgId, tcaBody, nullptr);
}

// PYR-81917
void ColUtilServerObject::processGetCurCdhWhitelist()
{
	PLog("%s", __FUNCTION__);

	CommMsgBody body;
	body.composeUINT32(ColWhitelistModule_CDH);
	body.composeUINT32(ColWhitelistType_User + ColWhitelistType_PLAYERPAIR);
	body.composeUINT32(ColWhitelistState_Active);

	postToColReportingAgg(COL_Q_COLWHITELIST_GET_CURR_LIST, body,
		new ColWhitelistNoReplyAsyncCall(this, &ColUtilServerObject::processGetCurCdhWhitelist_GotData));
}

// PYR-81917
void ColUtilServerObject::processGetCurCdhWhitelist_GotData(CommMsgParser& parser, ColWhitelistNoReplyAsyncCall* pCall)
{
	PLog("%s", __FUNCTION__);

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	if (errCode != DBM_NO_ERROR)
	{
		const char* errStr;
		parser.parseString(errStr);
		PLog("Error[%u]-%s", errCode, errStr);

		return;
	}

	vector<ColWhitelist> recordVec;
	parseVectorOfParsableMessageBodies(recordVec, parser);

	UINT32 sz = recordVec.size();
	if (sz == 0)
	{
		PTRACE("No records to send, aborting");
		return;
	}

	PTRACE("Sending %u records", sz);

	CommMsgBody body;
	composeVectorOfMessageBodiesOfComposableValues(recordVec, body);

	if (cdhConn.isConnected())
	{
		pCall->set(&ColUtilServerObject::processGetCurCdhWhitelist_GotCdhReply);
		postToCDH(COLWHITELIST_UPD_RECORD_LIST, body, pCall);
	}
	else
	{
		PTRACE("CDH Connection lost, aborting");
	}
}

// PYR-81917
void ColUtilServerObject::processGetCurCdhWhitelist_GotCdhReply(CommMsgParser& parser, ColWhitelistNoReplyAsyncCall* pCall)
{
	PLog("%s", __FUNCTION__);

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	if (errCode != DBM_NO_ERROR)
	{
		const char* errStr;
		parser.parseString(errStr);
		PLog("Error[%u]-%s", errCode, errStr);
	}
	else
	{
		PTRACE("CDH White list updated");
	}
}

// PYR-81917
void ColUtilServerObject::processCdaChangeRule_CdaDone(CommMsgParser& parser, CdaChangeRuleAsyncCallBase* callState)
{
	PTRACE("%s", __FUNCTION__);

	CCDARule ccdaRule;
	ccdaRule.parse(parser);

	callState->ccdaRuleVec.push_back(ccdaRule);
	PTRACE("Parsed %u rules", (UINT32)callState->ccdaRuleVec.size());

	CommMsgBody body;
	body.composeString(COLUTILSRV_INIDATA_OBJ_NAME);

	if (olapConn.isConnected())
	{
		postToOlap(DBM_Q_GET_INIDATA2, body, callState);
	}
	else
	{
		PTRACE("Lost connection to OLAP, aborting");
	}
}

// PYR-81917
void ColUtilServerObject::processCdaChangeRule_CdaDoneReorder(CommMsgParser& parser, CdaChangeRuleAsyncCallBase* callState)
{
	PTRACE("%s", __FUNCTION__);

	parseVectorOfParsableMessageBodies(callState->ccdaRuleVec, parser);
	PTRACE("Parsed %u rules", (UINT32)callState->ccdaRuleVec.size());

	CommMsgBody body;
	body.composeString(COLUTILSRV_INIDATA_OBJ_NAME);

	if (olapConn.isConnected())
	{
		postToOlap(DBM_Q_GET_INIDATA2, body, callState);
	}
	else
	{
		PTRACE("Lost connection to OLAP, aborting");
	}
}

// PYR-81917
void ColUtilServerObject::processCdaChangeRule_GotEmailList(CommMsgParser& parser, CdaChangeRuleAsyncCallBase* callState)
{
	PTRACE("%s", __FUNCTION__);

	IniDataFromDbm iniData;
	iniData.parse(parser);

	const IniDataFromDbm::Section* pSection = iniData.getSection(COLUTILSRV_INIDATA_CCDA_SECTION);
	if (pSection == nullptr)
	{
		PTRACE("Warning: no such section '%s'", COLUTILSRV_INIDATA_CCDA_SECTION);
		return;
	}

	vector<PString> emailVec;
	for (const IniDataFromDbm::Item& item : pSection->items)
	{
		if (item.name.equals(COLUTILSRV_INIDATA_CCDA_EMAIL_ITEM))
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

	static const char* const LINE_SEPARATOR = "<br />";
	PString htmlBodyStr;
	htmlBodyStr.assign("The following ").appendUint(callState->ccdaRuleVec.size()).append(" rules were changed (can be more than one in case of re-ordering)");

	for (const CCDARule& rule : callState->ccdaRuleVec)
	{
		htmlBodyStr.append(LINE_SEPARATOR).append(LINE_SEPARATOR);
		rule.getLogStr(htmlBodyStr);
	}

	CommMsgBody body;
	body.composeUINT32(eEmailDirectMultiTo);
	composeVector(body, emailVec, ComposerPString());
	body.
		composeString("col_util_DO_NOT_REPLY@pokerstars.com").					// fromEmail
		composeString("CCDA Update Notification").								// subj
		composeString("").														// body
		composeString(htmlBodyStr);												// htmlBody

	if (templateSrvConn.isConnected())
	{
		postToTemplateSrv(TS_Q_COMMON_SEND_EMAIL, body, callState);
	}
	else
	{
		PTRACE("Lost connection to Template Server, aborting");
	}

}

void ColUtilServerObject::processCdaChangeRule_EmailSent(CommMsgParser& parser, CdaChangeRuleAsyncCallBase* callState)
{
	PTRACE("%s", __FUNCTION__);

	PTRACE("Email sent, done");
}

void ColUtilServerObject::processGetCurrColWhitelist(UINT32 reqId, CommMsgParser& parser, AdminSrvCon& conn)
{
	PLog("%s", __FUNCTION__);

	CommMsgBody body;
	body.copyFromSkipReqId(parser.getMsgBody());

	postToColReportingAgg(COL_Q_COLWHITELIST_GET_CURR_LIST, body,
		new ColWhitelistAsyncCall(this, conn.id(), reqId, COL_A_COLWHITELIST_GET_CURR_LIST, &ColUtilServerObject::processGetCurrColWhitelist_GotData));
}

void ColUtilServerObject::processGetCurrColWhitelist_GotData(CommMsgParser& parser, ColWhitelistAsyncCall* pCall)
{
	PLog("%s", __FUNCTION__);
	CommMsgBody body;
	body.copyFrom(parser.getMsgBody());
	pCall->postReply(body);
}

void ColUtilServerObject::processGetColWhitelistHisotry(UINT32 reqId, CommMsgParser& parser, AdminSrvCon& conn)
{
	PLog("%s", __FUNCTION__);

	CommMsgBody body;
	body.copyFromSkipReqId(parser.getMsgBody());

	postToColReportingAgg(COL_Q_COLWHITELIST_GET_HISTORY, body,
		new ColWhitelistAsyncCall(this, conn.id(), reqId, COL_A_COLWHITELIST_GET_HISTORY, &ColUtilServerObject::processGetColWhitelistHisotry_GotData));

}

void ColUtilServerObject::processGetColWhitelistHisotry_GotData(CommMsgParser& parser, ColWhitelistAsyncCall* pCall)
{
	PLog("%s", __FUNCTION__);
	CommMsgBody body;
	body.copyFrom(parser.getMsgBody());
	postReply(pCall->findConnection(), COLSRV_A_WHITELIST_GET_HISTORY, body);
}

// PYR-81917
void ColUtilServerObject::processGetColWhitelistEntry(UINT32 reqId, CommMsgParser& parser, AdminSrvCon& conn)
{
	PLog("%s", __FUNCTION__);

	CommMsgBody body;
	body.copyFromSkipReqId(parser.getMsgBody());

	postToColReportingAgg(COL_Q_COLWHITELIST_GET_ENTRY, body,
		new ColWhitelistAsyncCall(this, conn.id(), reqId, COLSRV_A_WHITELIST_GET_ENTRY, &ColUtilServerObject::processGetColWhitelistEntry_GotData));
}

void ColUtilServerObject::processGetColWhitelistEntry_GotData(CommMsgParser& parser, ColWhitelistAsyncCall* pCall)
{
	PLog("%s", __FUNCTION__);
	CommMsgBody body;
	body.copyFrom(parser.getMsgBody());
	pCall->postReply(body);
}

// PYR-106157
void ColUtilServerObject::processGetUserRakePaid(UINT32 reqId, UINT32 connId, CommMsgParser& parser)
{
	PTRACE("processGURP");

	ColUtils::GetUserWageringRequest request(parser);
	
	time_t now = time(nullptr);

	PString logStr;
	PTRACE("%s", request.appendLog(logStr));

	vector<PString> userVec{ request.userId };

	SrvTime fromTime;
	if (!request.sinceBeginningOfTime)
	{
		fromTime = request.fromTime;
	}

	SrvTime toTime;
	if (request.toCurrentTime)
	{
		toTime.decode(now);
	}
	else
	{
		toTime = request.toTime;
	}

	// Input validation
	if (!request.sinceBeginningOfTime && fromTime.encode() > toTime.encode())
	{
		PTRACE("Warning: Bad input times, replying to client");

		processGetUserRakePaid_ReplyErrorToClient(reqId, connId, 1, "Bad input date extents");
		return; // No further processing
	}
	
	// Check cache for lifetime requests
	if (request.sinceBeginningOfTime && request.toCurrentTime)
	{
		ColUtils::UserWageringLifetimeAggKey cacheKey{ request.userId };
		ColUtils::UserWageringLifetimeAggData cacheData;
		if (reportCaches.getIfPresent(cacheKey, cacheData))
		{
			// Just take the most recent
			SrvDate lastAggDate(max(cacheData.lastDateCashGames.encode(), cacheData.lastDateTournaments.encode()));

			SrvTime rawStartTime;
			SrvTime rawEndTime;
			processGetUserRakePaid_CalcRawExtents(lastAggDate, request, now, rawStartTime, rawEndTime);

			PString dummy1, dummy2;
			PTRACE("-> raw(startTime: %s, endTime: %s)",
				srvTimeToString(rawStartTime, dummy1), srvTimeToString(rawEndTime, dummy2));

			vector<PString> userVec{ request.userId };
			CommMsgBody body;
			body
				.composeSrvTime(rawStartTime)					// timeFromCG
				.composeSrvTime(rawStartTime)					// timeFromTourn
				.composeSrvTime(rawEndTime);					// timeTo
			composeVector(body, userVec, ComposerPString());

			GetUserTotalRakePaidAsyncCall* pCall = new GetUserTotalRakePaidAsyncCall(*this, reqId, connId, now, request);
			pCall->userWagering = cacheData.userWagering;

			postToColDataRaw(COLRAW_Q_GET_WAGERING_EX, body, pCall);

			return; // No more processing
		}
	}

	CommMsgBody body;
	body
		.composeSrvTime(fromTime)
		.composeSrvTime(toTime);
	composeVector(body, userVec, ComposerPString());

	postToColDataAgg(COL_Q_GET_WAGERING_EX, body,
		new GetUserTotalRakePaidAsyncCall(*this, reqId, connId, now, request));
}

// PYR-106157
void ColUtilServerObject::processGetUserRakePaid_GotAggData(CommMsgParser& parser, GetUserTotalRakePaidAsyncCall& callState)
{
	PTRACE("processGURP AGG");

	PString logStr;
	PTRACE("%s", callState.userWageringRequest.appendLog(logStr));

	INT16 errCode = 0;
	parser.parseINT16(errCode);
	if (errCode)
	{
		const char* errBuf;
		parser.parseString(errBuf);
		PTRACE("Error: DBM error[%d]-%s", errCode, errBuf);

		processGetUserRakePaid_ReplyErrorToClient(callState.reqId, callState.connId, errCode, errBuf);
		
		return; // No more processing
	}

	SrvDate lastDateCashGames;
	SrvDate lastDateTournaments;

	parser
		.parseSrvDate(lastDateCashGames)
		.parseSrvDate(lastDateTournaments);
	
	vector<UserWageringEx> userWageringVec;
	parseVector(parser, userWageringVec);

	if (!userWageringVec.empty())
	{
		callState.userWagering = userWageringVec[0];
	}

	// Only put into cache for lifetime requests
	if (callState.userWageringRequest.sinceBeginningOfTime && callState.userWageringRequest.toCurrentTime)
	{
		ColUtils::UserWageringLifetimeAggKey cacheKey(callState.userWageringRequest.userId);
		ColUtils::UserWageringLifetimeAggData cacheData(lastDateCashGames, lastDateTournaments, callState.userWagering);
		reportCaches.putIfAbsent(cacheKey, cacheData);
	}

	// Just take the most recent
	SrvDate lastAggDate(max(lastDateCashGames.encode(), lastDateTournaments.encode()));

	SrvTime aggFromTime;
	if (!callState.userWageringRequest.sinceBeginningOfTime)
	{
		aggFromTime = callState.userWageringRequest.fromTime;
	}

	PString dummy1, dummy2;
	PTRACE("agg: %s - %s", srvTimeToString(aggFromTime, dummy1), srvDateToString(lastAggDate, dummy2));

	SrvTime rawStartTime;
	SrvTime rawEndTime;
	bool needSendRawRequest = processGetUserRakePaid_CalcRawExtents(
		lastAggDate, callState.userWageringRequest, callState.timeOfRequest, 
		rawStartTime, rawEndTime);

	if (needSendRawRequest)
	{
		PString dummy1, dummy2;
		PTRACE("-> raw(startTime: %s, endTime: %s)",
			srvTimeToString(rawStartTime, dummy1), srvTimeToString(rawEndTime, dummy2));

		vector<PString> userVec{ callState.userWageringRequest.userId };
		CommMsgBody body;
		body
			.composeSrvTime(rawStartTime)					// timeFromCG
			.composeSrvTime(rawStartTime)					// timeFromTourn
			.composeSrvTime(rawEndTime);					// timeTo
		composeVector(body, userVec, ComposerPString());

		postToColDataRaw(COLRAW_Q_GET_WAGERING_EX, body, &callState);
	}
	else
	{
		processGetUserRakePaid_ReplyToClient(callState);
	}
}

// PYR-106157
bool ColUtilServerObject::processGetUserRakePaid_CalcRawExtents(
	const SrvDate& lastAggDate, const ColUtils::GetUserWageringRequest& userWageringRequest, const time_t& now,
	SrvTime& rawStartTime, SrvTime& rawEndTime)
{
	bool needSendRawRequest = true;

	if (!lastAggDate.isNull())
	{
		SrvDate rawStartDate;
		normalizeSrvDate(lastAggDate, rawStartDate, 1);
		rawStartTime.decode(rawStartDate.encode());

		if (userWageringRequest.toCurrentTime)
		{
			if (lastAggDate.encode() >= now)
			{
				needSendRawRequest = false;
			}
			else
			{
				rawEndTime.decode(now);
			}
		}
		else
		{
			if (lastAggDate.encode() >= userWageringRequest.toTime.encode())
			{
				needSendRawRequest = false;
			}
			else
			{
				rawEndTime = userWageringRequest.toTime;
			}
		}
	}
	else
	{
		// Degenerate cases where lastAggDate was null.

		// Don't allow to check further than yesterday at midnight
		SrvTime farthestRawStartTime(now);
		ColUtils::setStartOfDay(farthestRawStartTime);
		ColUtils::addDays(farthestRawStartTime, -1);

		if (userWageringRequest.sinceBeginningOfTime)
		{
			PTRACE("Warning: failed to find agg extents");

			rawStartTime = farthestRawStartTime;
		}
		else
		{
			rawStartTime.decode(max(userWageringRequest.fromTime.encode(), farthestRawStartTime.encode()));
		}

		if (userWageringRequest.toCurrentTime)
		{
			rawEndTime.decode(now);
		}
		else
		{
			rawEndTime = userWageringRequest.toTime;
		}
	}

	if (needSendRawRequest && rawStartTime.encode() >= rawEndTime.encode())
	{
		PString dummy1, dummy2;
		PTRACE("Warning: invalid raw extents %s - %s, not sending raw request",
			srvTimeToString(rawStartTime, dummy1), srvTimeToString(rawEndTime, dummy2));

		needSendRawRequest = false;
	}


	return needSendRawRequest;
}

// PYR-106157
void ColUtilServerObject::processGetUserRakePaid_GotRawData(CommMsgParser& parser, GetUserTotalRakePaidAsyncCall& callState)
{
	PTRACE("processGURP RAW");

	PString logStr;
	PTRACE("%s", callState.userWageringRequest.appendLog(logStr));

	INT16 errCode = 0;
	parser.parseINT16(errCode);
	if (errCode)
	{
		const char* errBuf;
		parser.parseString(errBuf);
		PTRACE("Error: DBM error[%d]-%s", errCode, errBuf);

		processGetUserRakePaid_ReplyErrorToClient(callState.reqId, callState.connId, errCode, errBuf);
		return; // No more processing
	}

	vector<UserWageringEx> userRawWageringVec;
	parseVector(parser, userRawWageringVec);

	if (userRawWageringVec.empty()) 
	{
		processGetUserRakePaid_ReplyToClient(callState);
		return; // No more processing
	}

	UserWageringEx userRawWagering = userRawWageringVec[0];
	for (const auto& entry : userRawWagering.currAmountMap) 
	{
		const char* currency = entry.first;
		const AccumWageringData& wageringData = entry.second;
		callState.userWagering.addCurrencyAmount(currency, wageringData.totalWagered, wageringData.totalRakePaid);
	}

	processGetUserRakePaid_ReplyToClient(callState);
}

// PYR-106157
void ColUtilServerObject::processGetUserRakePaid_ReplyToClient(const GetUserTotalRakePaidAsyncCall& callState)
{
	const UserWageringEx& uw = callState.userWagering;
	const UserWageringEx::CurrencyAmountMap& currAmountMap = uw.currAmountMap;

	CommMsgBody reply;
	reply.composeUINT32(callState.reqId);
	reply.composeINT16(DBM_NO_ERROR);
	reply.composeUINT32(currAmountMap.size());

	for (const auto& entry : currAmountMap)
	{
		const char* currency = entry.first;
		const AccumWageringData& wageringData = entry.second;

		CommMsgBody innerBody;
		innerBody.composeString(currency);
		innerBody.composeINT64(wageringData.totalRakePaid);

		INT64 convertedRakePaid = convertCurrencyM(wageringData.totalRakePaid, currency, CONV_RATE_DEFAULT_CURRENCY);
		innerBody.composeINT64(convertedRakePaid);

		reply.composeMsgBody(innerBody);
	}

	CommServerConnection* pConn = findConnection(callState.connId);
	postReply(pConn, COLSRV_A_GET_USER_RAKE_PAID, reply);
}

// PYR-106157
void ColUtilServerObject::processGetUserRakePaid_ReplyErrorToClient(UINT32 reqId, UINT32 connId, INT16 errCode, const char* errBuf)
{
	CommMsgBody reply;
	reply.composeUINT32(reqId);
	setReply(reply, errCode, errBuf);
	CommServerConnection* pConn = findConnection(connId);
	postReply(pConn, COLSRV_A_GET_USER_RAKE_PAID, reply);
}

////////////////////////////////////////////////////////////////////////////////

void ColUtilServerObject::dynamicInit()
{
	PLog("ColUtilServerObject::dynamicInit");

	PIniFile ini(fullFileName);
	PIniFile::Section* section = ini.getSection("DYNAMICCONNECT");
	if (section)
	{
		{
			const char* server = section->getProperty("LoginDbmServer");
			const char* serverObject = section->getProperty("LoginDbmObject", "DbmLoginInstance");
			const char* connectionType = section->getProperty("LoginAuthDbmConnectionType", COLUTILSRV_LOGINAUTH_CONNTYPE);
			reconnect(cliPool, loginAuthConn, server, serverObject, connectionType);
		}

		{
			const char* server = section->getProperty("ColRawDbmServer");
			const char* serverObject = section->getProperty("ColRawDbmServerObject", "ColDbmProxyInstance");
			const char* connectionType = section->getProperty("ColRawDbmConnectionType", COLUTILSRV_INTERNAL_CONNTYPE);
			reconnect(cliPool, colRawReportDbmConn, server, serverObject, connectionType);
		}

		{
			const char* server = section->getProperty("ColAggDbmServer");
			const char* serverObject = section->getProperty("ColAggDbmServerObject", "ColDbmProxyInstance");
			const char* connectionType = section->getProperty("ColAggDbmConnectionType", COLUTILSRV_INTERNAL_CONNTYPE);
			reconnect(cliPool, colAggReportDbmConn, server, serverObject, connectionType);
		}

		{
			const char* server = section->getProperty("ColReaderServer");
			const char* serverObject = section->getProperty("ColReaderServerObject", "ColossusReadInstance");
			const char* connectionType = section->getProperty("ColReaderConnectionType", COLUTILSRV_INTERNAL_CONNTYPE);
			reconnect(cliPool, colReaderDbmConn, server, serverObject, connectionType);
		}

		{
			const char* server = section->getProperty("TcaServer");
			const char* serverObject = section->getProperty("TcaServerObject", "TcaDetectorInstance");
			const char* connectionType = section->getProperty("TcaConnectionType", TCA_SERVER_COLUTIL_CONNECTION_TYPE);
			reconnect(cliPool, tcaConn, server, serverObject, connectionType);
		}

		{
			const char* server = section->getProperty("ColCdaServer");
			const char* serverObject = section->getProperty("ColCdaServerObject", "ColCDAInstance");
			const char* connectionType = section->getProperty("ColCdaConnectionType", COLCDA_CONN_TYPE_COLUTIL);
			reconnect(cliPool, ccdaConn, server, serverObject, connectionType);
		}

		{
			const char* server = section->getProperty("CdhServer");
			const char* serverObject = section->getProperty("CdhServerObject", "ColossusCdhProxyInstance");
			const char* connectionType = section->getProperty("CdhConnectionType", COLSHD_CONN_TYPE_INTERNAL);
			reconnect(cliPool, cdhConn, server, serverObject, connectionType);
		}

		{
			const char* server = section->getProperty("ColAggDBMWriterServer");
			const char* serverObject = section->getProperty("ColAggDBMWriterServerObject", "ColUtilDbmWriteInstance");
			const char* connectionType = section->getProperty("ColAggDBMConnectionType", COLDBM_COLUTIL_WRITE_CONNECTION_TYPE);
			reconnect(cliPool, colAggDbmWriterConn, server, serverObject, connectionType);
		}

		{
			const char* server = section->getProperty("OlapServer");
			const char* serverObject = section->getProperty("OlapObject", "OlapDbm3ServerInstance");
			const char* connectionType = section->getProperty("OlapConnectionType", COLOSSUS_UTL_CONN_TYPE_TO_OLAP);
			reconnect(cliPool, olapConn, server, serverObject, connectionType);
		}

		{
			const char* server = section->getProperty("TemplateServer");
			const char* serverObject = section->getProperty("TemplateServerObject", "TemplateServerInstance");
			const char* connectionType = section->getProperty("TemplateConnectionType", "collusion");
			reconnect(cliPool, templateSrvConn, server, serverObject, connectionType);
		}

		// PYR-106157
		{
			const char* server = section->getProperty("ColUtilAggDbmServer");
			const char* serverObject = section->getProperty("ColUtilAggDbmServerObject", "ColDbmProxyInstance");
			const char* connectionType = section->getProperty("ColUtilAggDbmConnType", COLUTILSRV_INTERNAL_CONNTYPE);

			// Remove condition after configuration in QA is complete
			if (server && *server)
			{
				reconnect(cliPool, colAggDataDbmConn, server, serverObject, connectionType);
			}
		}

		// PYR-106157
		{
			const char* server = section->getProperty("ColUtilRawDbmServer");
			const char* serverObject = section->getProperty("ColUtilRawDbmServerObject", "ColRawDbmProxyInstance");
			const char* connectionType = section->getProperty("ColUtilRawDbmConnType", COLUTILSRV_INTERNAL_CONNTYPE);

			// Remove condition after configuration in QA is complete
			if (server && *server)
			{
				reconnect(cliPool, colRawDataDbmConn, server, serverObject, connectionType);
			}
		}
	}
	else
	{
		PLog(LOG_TRACE_FAULTY_LOGIC ": Cannot read [DYNAMICCONNECT]");
		throw PError("dynamicInit - Cannot read [DYNAMICCONNECT]");
	}

	section = ini.getSection("DYNAMICSETTINGS");
	if (section)
	{
		moreTrace = section->getIntProperty("moreTrace", 1) != 0;
		fastReportsMaxQueueSize = section->getIntProperty("fastReportsMaxQueueSize", DEFAULT_MAX_IN_QUEUE_FAST_REPORTS);
		fastReportsMaxNumProcessing = section->getIntProperty("fastReportsMaxNumProcessing", DEFAULT_IN_PROCESS_FAST_REPORTS);
		shdFreqPartnersReportTimeoutSeconds = section->getIntProperty("shdFreqPartnersReportTimeoutSeconds", DEFAULT_FAST_REPORT_TIMEOUT_SECONDS);
		getHandStatsReportTimeoutSeconds = section->getIntProperty("getHandStatsReportTimeoutSeconds", DEFAULT_FAST_REPORT_TIMEOUT_SECONDS);
		getShdHandsForAmpsTimeoutSeconds = section->getIntProperty("getShdHandsForAmpsTimeoutSeconds", DEFAULT_FAST_REPORT_TIMEOUT_SECONDS);
		getAmpsTopPartnersTimeoutSeconds = section->getIntProperty("getAmpsTopPartnersTimeoutSeconds", DEFAULT_FAST_REPORT_TIMEOUT_SECONDS);
		getAmpsGameActivityTimeoutSeconds = section->getIntProperty("getAmpsGameActivityTimeoutSeconds", DEFAULT_FAST_REPORT_TIMEOUT_SECONDS);
		getAmpsGamesTogetherTimeoutSeconds = section->getIntProperty("getAmpsGamesTogetherTimeoutSeconds", DEFAULT_FAST_REPORT_TIMEOUT_SECONDS);
		getCdhEventReportTimeoutSeconds = section->getIntProperty("getCdhEventReportTimeoutSeconds", DEFAULT_FAST_REPORT_TIMEOUT_SECONDS);
		getBpsUserStakesPlayedTimeoutSeconds = section->getIntProperty("getBpsUserStakesPlayedTimeoutSeconds", DEFAULT_FAST_REPORT_TIMEOUT_SECONDS);
		getAggTableExtentsTimeoutSeconds = section->getIntProperty("getAggTableExtentsTimeoutSeconds", DEFAULT_FAST_REPORT_TIMEOUT_SECONDS);
	}
	else
	{
		PLog(LOG_TRACE_FAULTY_LOGIC ": Cannot read [DYNAMICSETTINGS]");
		throw PError("dynamicInit - Cannot read [DYNAMICSETTINGS]");
	}

	PLog("moreTrace=%s", moreTrace ? "T" : "F");
	PLog("fastReportsMaxQueueSize: %u", fastReportsMaxQueueSize);
	PLog("fastReportsMaxNumProcessing: %u", fastReportsMaxNumProcessing);
	PLog("shdFreqPartnersReportTimeoutSeconds: %u", shdFreqPartnersReportTimeoutSeconds);
	PLog("getHandStatsReportTimeoutSeconds: %u", getHandStatsReportTimeoutSeconds);
	PLog("getShdHandsForAmpsTimeoutSeconds: %u", getShdHandsForAmpsTimeoutSeconds);
	PLog("getAmpsTopPartnersTimeoutSeconds: %u", getAmpsTopPartnersTimeoutSeconds);
	PLog("getAmpsGameActivityTimeoutSeconds: %u", getAmpsGameActivityTimeoutSeconds);
	PLog("getAmpsGamesTogetherTimeoutSeconds: %u", getAmpsGamesTogetherTimeoutSeconds);
	PLog("getCdhEventReportTimeoutSeconds: %u", getCdhEventReportTimeoutSeconds);
	PLog("getBpsUserStakesPlayedTimeoutSeconds: %u", getBpsUserStakesPlayedTimeoutSeconds);
	PLog("getAggTableExtentsTimeoutSeconds: %u", getAggTableExtentsTimeoutSeconds);

	fastReportQueue.configure(fastReportsMaxQueueSize, fastReportsMaxNumProcessing);

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
		useLogger = false;
		PTRACE("No LOGGER configured");
	}

	// PYR-81917
	if (cdhConn.isConnected())
	{
		processGetCurCdhWhitelist();
	}

	PLog("#ColUtilServerObject::dynamicInit");
}

void ColUtilServerObject::reconnect(CommClientConnectionPool& cliConnPool, CommClientGConnection& conn, const char* const serverName, const char* const serverObject, const char* const serverConnectionType)
{
	PLog("reconnect(serverName: %s, serverObject: %s, serverConnectionType: %s)"
		, (serverName && *serverName) ? serverName : "none"
		, (serverObject && *serverObject) ? serverObject : "none"
		, (serverConnectionType && *serverConnectionType) ? serverConnectionType : "none"
		);

	ConnUtils::createClientConnection(cliConnPool, conn, serverName, serverObject, serverConnectionType);

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

void ColUtilServerObject::processTimerMessage(UINT32 handle, UINT32 msgId, CommMsgBody& body)
{
	HiresCounter counter;
	PTRACE("{T");

	CommServerObject::processTimerMessage(handle, msgId, body);

	switch (msgId)
	{
	case LOGGER_TIMER:
		{
			CommMsgBody timerBody;
			heLoggerTimerHandle = postTimerMessage(LOGGER_TIMER, timerBody, TIMER_LOGGER_MSEC);
			reportPerformance();

			break;
		}
	case RPT_CACHES_TIMER:
		{
			CommMsgBody timerBody;
			postTimerMessage(RPT_CACHES_TIMER, timerBody, TIMER_RPT_CACHES_MSEC);

			reportCaches.sweep();
			reportCaches.logStats();

			// PYR-89537
			getAggTableExtents();

			break;
		}
	case CONV_RATE_TIMER:
		{
			CommMsgBody timerBody;
			postTimerMessage(CONV_RATE_TIMER, timerBody, CONV_RATE_MSEC);

			postToColReader(COLOSSUS_GET_CONVRATES_TO_USD, body, new ConvRateAsyncCall(*this));

			break;
		}
	default:
		{
			PTRACE("Unknown timer msg %d", msgId);
			break;
		}
	}

	counter.stop();
	UINT32 ticks = counter.getElapsedTick();
	ticksAccumulator.addMessageTicks(counter);
	PTRACE("}%d%s", ticks, (ticks > TICKS_PER_SECOND) ? " overspan" : "");
}

void ColUtilServerObject::processAdminMessage(UINT32 msgId, const CommMsgBody& body, CommServerConnection* conn)
{
	AdminSrvCon* adminConn = dynamic_cast<AdminSrvCon*>(conn);
	if (!adminConn)
	{
		PLog("Incoming connection from admin lost!");
		return;	// no reply!
	}

	PTRACE("admin id: %s", adminConn->getAdminId());

	CommMsgParser parser(body);
	UINT32 reqId;
	parser.parseUINT32(reqId);

	UINT32 replyId = msgId + 1; // By local convention
	CommMsgBody reply;
	reply.composeUINT32(reqId);

	switch (msgId)
	{
		case MSG_ADMIN_KILL_ITSELF:
		{
			if (checkAdminRight(HANDLE_KILL_ITSELF, adminConn, reply))
			{
				return processKillServer(adminConn->getAdminId()); // No return
			}
			break;
		}
		case MSG_ADMIN_REREAD_INI:
		{
			if (checkAdminRight(SYSADMIN_STR, adminConn, reply))
			{
				processRefreshSettings(reply);
			}
			break;
		}
		case COLSRV_Q_SHD_FREQPARTNERS_REPORT:
		{
			processGetShdFreqPartnersReport(reqId, parser, adminConn);
			return; // No reply!
		}
		case COLSRV_Q_STATS_FOR_HAND:
		{
			processGetStatsForHand(reqId, parser, adminConn);
			return;	// No reply!
		}
		case TCA_Q_CREATE_CONFIG:
		case TCA_Q_MODIFY_CONFIG:
		case TCA_Q_ACTIVATE_CONFIG:
		case TCA_Q_DELETE_CONFIG:
		case TCA_Q_MANUAL_RUN_CONFIG:
		case TCA_Q_CANCEL_WAITING_TASK:
		{
			if (checkAdminRight(MANAGE_TCA_CONTROL, adminConn, reply))
			{
				forwardAsyncMsg<TcaConfigAsyncCall>(msgId, reqId, parser, adminConn, tcaConn);
				return; // No reply!
			}
			break;
		}
		case TCA_Q_READ_CONFIG:
		case TCA_Q_READ_AUDIT_CONFIG:
		case TCA_Q_READ_CURRENT_CONFIG_LIST:
		case TCA_Q_READ_FULL_CONFIG_LIST:
		case TCA_Q_GET_TASK_LIST:
		case TCA_Q_GET_TCA_ALERTS_BY_TASK:
		case TCA_Q_GET_TASK_BY_TASKID:
		case TCA_Q_READ_FILTERED_CONFIG_LIST:
		case TCA_Q_GET_ALERTS_FOR_TASKS:
		{
			forwardAsyncMsg<TcaConfigAsyncCall>(msgId, reqId, parser, adminConn, tcaConn);
			return; // No reply!
		}
		case COLCDA_CREATE_RULE:
		case COLCDA_MODIFY_RULE:
		case COLCDA_ACTIVATE_RULE:
		case COLCDA_DELETE_RULE:
		case COLCDA_SET_VELOCITY_CONTROL:
		{
			if (checkAdminRight(MANAGE_CCDA_CONTROL, adminConn, reply))
			{
				forwardAsyncMsg<CdaChangeRuleAsyncCall>(msgId, reqId, parser, adminConn, ccdaConn);
				return; // No reply!
			}
			break;
		}
		case COLCDA_START_EVALUATION:
		{
			if (checkAdminRight(MANAGE_CCDA_CONTROL, adminConn, reply))
			{
				forwardAsyncMsg<CdaStartEvalAsyncCall>(msgId, reqId, parser, adminConn, ccdaConn);
				return; // No reply!
			}
			break;
		}
		case COLCDA_REORDER_RULES:
		{
			if (checkAdminRight(MANAGE_CCDA_CONTROL, adminConn, reply))
			{
				forwardAsyncMsg<CdaReorderRuleAsyncCall>(msgId, reqId, parser, adminConn, ccdaConn);
				return; // No reply!
			}
			break;
		}
		case COLCDA_READ_RULE:
		case COLCDA_READ_CURRENT_RULE_LIST:
		case COLCDA_READ_FULL_RULE_LIST:
		case COLCDA_GET_RULE_HISTORY:
		case COLCDA_GET_VELOCITY_CONTROL:
		case COLCDA_GET_EVALUATION_STATUS:
		{
			if (checkAdminRight(VIEW_CCDA_RULES, adminConn, reply))
			{
				forwardAsyncMsg<CdaRuleAsyncCall>(msgId, reqId, parser, adminConn, ccdaConn);
				return; // No reply!
			}
			break;
		}
		case COLSRV_Q_AMPS_SHD_HANDS:
		{
			PLog("Warning: msgId %u (COLSRV_Q_AMPS_SHD_HANDS) is obsolete", msgId);
			reply.composeINT16(1).composeString("This is an obsolete message");

			break;
		}
		case COLSRV_Q_AMPS_SHD_HANDS2:
		{
			processGetShdHandsForAmps(reqId, parser, adminConn);
			return; // No reply!
		}
		case COLSRV_Q_AMPS_TOP_PARTNERS:
		{
			processAmpsTopPartners(reqId, parser, adminConn);
			return; // No reply!
		}
		case COLSRV_Q_AMPS_GAME_ACTIVITY:
		{
			processAmpsGameActivity(reqId, parser, adminConn);
			return; // No reply!
		}
		case COLSRV_Q_AMPS_GAMES_TOGETHER:
		{
			processAmpsGamesTogether(reqId, parser, adminConn);
			return; // No reply!
		}
		case COLSRV_Q_WHITELIST_CREATE:
		{
			if (checkAdminRight(COLOSSUS_MANAGER, adminConn, reply))
			{
				processCreateColWhitelist(reqId, parser, *adminConn);
				return;
			}
			break;
		}
		case COLSRV_Q_WHITELIST_MODIFY:
		{
			if (checkAdminRight(COLOSSUS_MANAGER, adminConn, reply))
			{
				processModifyColWhitelist(reqId, parser, *adminConn);
				return;
			}
			break;
		}
		case COLSRV_Q_WHITELIST_DELETE:
		{
			if (checkAdminRight(COLOSSUS_MANAGER, adminConn, reply))
			{
				processDeleteColWhitelist(reqId, parser, *adminConn);
				return;
			}
			break;
		}
		case COLSRV_Q_WHITELIST_GET_CURR_LIST:
		{
			processGetCurrColWhitelist(reqId, parser, *adminConn);
			return;
		}
		case COLSRV_Q_WHITELIST_GET_HISTORY:
		{
			processGetColWhitelistHisotry(reqId, parser, *adminConn);
			return;
		}
		case COLSRV_Q_WHITELIST_GET_ENTRY:
		{
			processGetColWhitelistEntry(reqId, parser, *adminConn);
			return;
		}
		case COLSRV_Q_NORMALIZE_USERIDS:
		{
			processNormalizeUserIds(reqId, parser, *adminConn);
			return;
		}
		case COLSRV_Q_GET_CDH_EVENTS:
		{
			processGetCdhEventReport(reqId, parser, adminConn);
			return; // No reply!
		}
		case COLSRV_Q_GET_NORMAL_BPS_REPORT:
		{
			processGetNormalBPSReport(reqId, parser, *adminConn);
			return; // no reply
		}
		case COLSRV_Q_GET_XSTAKES_BPS_REPORT:
		{
			processGetXStakesBPSReport(reqId, parser, *adminConn);
			return; // no reply
		}
		case COLSRV_Q_GET_USER_STAKES_PLAYED:
		{
			processGetBpsUserStakedPlayedReport(reqId, parser, adminConn);
			return; // No reply!
		}
		case COLSRV_Q_GET_AGG_TABLE_EXTENTS:
		{
			processGetAggTableExtentsReport(reqId, parser, adminConn);
			return; // No reply!
		}
		default:
		{
			PLog("Error: Unhandled Admin message #%u", msgId);
			return; // No reply!
		}
	}

	postReply(adminConn, replyId, reply);
}

void ColUtilServerObject::processTrustedAdminMessage(UINT32 msgId, const CommMsgBody& body, CommServerConnection* conn)
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

	UINT32 replyId = msgId + 1; // By local convention
	CommMsgBody reply;
	reply.composeUINT32(reqId);

	switch (msgId)
	{

	case MSG_ADMIN_REREAD_INI:
		{
			replyId = MSG_ADMIN_REREAD_INI_REPLY;

			PLog("[ColUtilServerObject::processTrustedAdminMessage] MSG_ADMIN_REREAD_INI has been received");

			if (checkAdminRight(SYSADMIN_STR, adminConn, reply))
			{
				processRefreshSettings(reply);
			}

			break;
		}

	case MSG_ADMIN_KILL_ITSELF:
		{
			replyId = MSG_ADMIN_KILL_ITSELF_REPLY;

			PLog("[ColUtilServerObject::processTrustedAdminMessage] MSG_ADMIN_KILL_ITSELF has been received");

			if (checkAdminRight(HANDLE_KILL_ITSELF, adminConn, reply))
			{
				return processKillServer(adminConn->getAdminId()); // No return
			}
			break;
		}

	default:
		{
			PLog("ColUtilServerObject::processTrustedAdminMessage =====> Unsupported message msgId=%d", msgId);
			break;
		}
	}

	postReply(adminConn, replyId, reply);
}

void ColUtilServerObject::processInternalMessage(UINT32 msgId, const CommMsgBody& body, CommServerConnection* conn)
{
	ColUtilServerConnection* conn_ = dynamic_cast<ColUtilServerConnection*>(conn);
	if (!conn_)
	{
		PLog("Incoming connection lost!");
		return;	// no reply!
	}

	CommMsgParser parser(body);
	UINT32 reqId;
	parser.parseUINT32(reqId);

	UINT32 replyId = msgId + 1; // By local convention
	CommMsgBody reply;
	reply.composeUINT32(reqId);

	switch (msgId)
	{
		case COLSRV_Q_GET_USER_RAKE_PAID:
		{
			processGetUserRakePaid(reqId, conn->id(), parser);
			return; // No reply!
		}
		default:
		{
			PLog("Error: Unhandled message #%u", msgId);
			return; // No reply!
		}
	}

	postReply(conn, replyId, reply);
}

void ColUtilServerObject::startHeLogger(const char * strServerAddress, const char * strStationName)
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

void ColUtilServerObject::stopHeLogger()
{
	if (heLoggerTimerHandle)
	{
		deleteTimerMessage(heLoggerTimerHandle);
		heLoggerTimerHandle = 0;
	}
}

void ColUtilServerObject::reportPerformance()
{
	ticksAccumulator.stop();

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

			strValue.assign("").appendInt(loginAuthConn.isConnected());
			heLogger->compose(reportBody, "connection.loginAuthConn", strValue);

			strValue.assign("").appendInt(colRawReportDbmConn.isConnected());
			heLogger->compose(reportBody, "connection.colRawReportDbmConn", strValue);

			strValue.assign("").appendInt(colAggReportDbmConn.isConnected());
			heLogger->compose(reportBody, "connection.colAggReportDbmConn", strValue);

			strValue.assign("").appendInt(colReaderDbmConn.isConnected());
			heLogger->compose(reportBody, "connection.colReaderDbmConn", strValue);

			strValue.assign("").appendInt(tcaConn.isConnected());
			heLogger->compose(reportBody, "connection.tcaConn", strValue);

			strValue.assign("").appendInt(ccdaConn.isConnected());
			heLogger->compose(reportBody, "connection.ccdaConn", strValue);

			strValue.assign("").appendInt(cdhConn.isConnected());
			heLogger->compose(reportBody, "connection.cdhConn", strValue);

			strValue.assign("").appendInt(olapConn.isConnected());
			heLogger->compose(reportBody, "connection.olapConn", strValue);

			strValue.assign("").appendInt(templateSrvConn.isConnected());
			heLogger->compose(reportBody, "connection.templateSrvConn", strValue);

			strValue.assign("").appendInt(colAggDbmWriterConn.isConnected());
			heLogger->compose(reportBody, "connection.colAggDbmWriter", strValue);

			strValue.assign("").appendInt(colAggDataDbmConn.isConnected());
			heLogger->compose(reportBody, "connection.colAggDataDbmConn", strValue);

			strValue.assign("").appendInt(colRawDataDbmConn.isConnected());
			heLogger->compose(reportBody, "connection.colRawDataDbmConn", strValue);

			strValue.assign("").appendUint(fastReportQueue.getOvertimeReportNum());
			heLogger->compose(reportBody, "OvertimeReportNum", strValue);

			heLogger->post(reportBody);
		}

		catch (const PError& er)
		{
			PLog("ColUtilServerObject::reportPerformance() error: '%s'", er.why());
		}
	}

	fastReportQueue.log();

	ticksAccumulator.reset();
}

void ColUtilServerObject::processRefreshSettings(CommMsgBody& reply)
{
	PLog("processRefreshSettings");

	dynamicInit();

	setReply(reply, DBM_NO_ERROR, "");
}

void ColUtilServerObject::processKillServer(const char* adminId)
{
	PLog("Kill application message received from admin id: %s", adminId);

	shutdown();
	this->exit();
}

// PYR-89537
void ColUtilServerObject::getAggTableExtents()
{
	PLog("%s", __FUNCTION__);

	time_t now = time(nullptr);

	ColUtils::MaintJobSetupQueryParamsEx defaultInputsEx;
	ColUtils::MaintJobSetupQueryParams& defaultInputs = defaultInputsEx.queryParams;
	vector<ColUtils::AggTableParams>& aggTableParamVec = defaultInputs.aggTableParamVec;
	ColUtils::getAllAggTableParams(aggTableParamVec); // PYR-92828

	ColUtils::MaintJobSetupEntries outputs;

	// Don't check the cache here, just get the most up to date result
	//if (reportCaches.getIfPresent(defaultInputs, outputs))
	//{
	//	PTRACE("in cache, aborting");

	//	return; // No more processing
	//}

	defaultInputsEx.timeoutTime.decode(now + getAggTableExtentsTimeoutSeconds);

	static const UINT32 EMPTY_REQ_ID = 0;
	static const UINT32 EMPTY_CONN_ID = 0;
	static const UINT32 EMPTY_REPLY_ID = 0;
	static const char* EMPTY_ADMIN_ID = "";
	
	unique_ptr<ColGetAggTableExtentsFastReport> reportItem = make_unique<ColGetAggTableExtentsFastReport>(EMPTY_REQ_ID, EMPTY_CONN_ID, EMPTY_REPLY_ID, EMPTY_ADMIN_ID, *this, defaultInputsEx);
	reportItem->requestProcessorFn = &ColUtilServerObject::processGetAggTableExtentsReport_OnSchedule_Submitted;

	bool res = fastReportQueue.pushBack(move(reportItem));
	if (!res)
	{
		PLog("queue full, aborting");

		return; // No more processing
	}

	res = fastReportQueue.submitNext();
	if (!res)
	{
		PLog("max submissions reached, waiting");
	}
}

void ColUtilServerObject::insertAdminReport(ColUtils::ColAdminReport& adminReport)
{
	PLog("%s", __FUNCTION__);

	adminReport.finished.currentLocalTime();

	CommMsgBody request;
	adminReport.compose(request);
	postToColAggWriter(COL_Q_INSERT_ADMIN_REPORT, request, 
		new ColAdminReportAsyncCall(this, &ColUtilServerObject::processInsertAdminReport_GotData));
	
}

void ColUtilServerObject::processInsertAdminReport_GotData(CommMsgParser& parser, ColAdminReportAsyncCall* pCall)
{
	PLog("%s", __FUNCTION__);

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	if (errCode != DBM_NO_ERROR)
	{
		const char* errBuf;
		parser.parseString(errBuf);
		PLog("Error: %s", errBuf);
	}
}

void ColUtilServerObject::postReply(CommServerConnection* pConn, UINT32 msgId, CommMsgBody& body)
{
	if (pConn)
	{
		pConn->post(msgId, body);
	}
	else
	{
		PLog("Message #%u not sent (no connection)", msgId);
	}
}

void ColUtilServerObject::setReply(CommMsgBody& reply, INT16 errCode, const char* errDesc /*= ""*/)
{
	reply.composeINT16(errCode);

	if (errCode)
	{
		reply.composeString(errDesc);
	}
}

bool ColUtilServerObject::checkAdminRight(const char* right, const ColUtilSrvPrivilegedConnection* pConn, CommMsgBody& reply)
{
	if (pConn->checkAdminRight(right))
	{
		return true;
	}

	setReply(reply, NOT_ALLOWED, "Insufficient rights");
	return false;
}

////////////////////////////////////////////////////////////////////////////////
