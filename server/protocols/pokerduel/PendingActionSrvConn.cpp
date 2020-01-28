////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2012 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "PendingActionSrvConn.h"
#include "composecolls.h"

////////////////////////////////////////////////////////////////////////////////

/* new */ CommServerConnection* DuelNotifSrvConnFactory::createConnection() // override
{
	obj->duelNotifSrvConn = new DuelNotifSrvConn(obj, traceMarker);
	return obj->duelNotifSrvConn;
}

void PendingActionServerObjectInter::postNotifMsg(UINT32 msgId, CommMsgBody& body)
{
	PLog("PANotif %u", msgId);
	if (!duelNotifSrvConn)
	{
		PLog("!conn postNotifMsg");
		return;
	}
	duelNotifSrvConn->post(msgId, body);
}

void DuelNotifSrvConn::closedOrDisconnected(int errCode, const char* errMsg) // override
{
	if (obj->duelNotifSrvConn == this)
	{
		obj->duelNotifSrvConn = NULL;
	}
	BaseIncomingConnection::closedOrDisconnected(errCode, errMsg);
}

void DuelNotifSrvConn::processRequest(UINT32 msgId, const CommMsgBody& body) // override
{
	if (msgId != MSG_DNSERVER_SRV_GET_PENDING_ACTIONS && msgId != MSG_DNSERVER_SRV_GET_PENDING_ACTIONS_COUNT)
	{
		PLog("DuelNotifSrvConn::processRequest unhandled msgId=%u", msgId);
		return;
	}

	CommMsgParser parser(body);
	UINT32 reqId;
	const char* userId;
	UINT32 siteId;
	parser
		.parseUINT32(reqId)
		.parseString(userId)
		.parseUINT32(siteId);

	if (obj->useAsyncCallToLoadData(msgId, reqId, userId, siteId)) // for async-call processing
	{
		return;
	}
	obj->replyPendingActionMsg(msgId, reqId, userId, siteId);
}

void DuelNotifSrvConn::replyGetPendingActions(const char* userId, UINT32 siteId, UINT32 reqId)
{
	vector<DuelPendingAction> actions;
	PString errMsg;
	PTRACE5("getPendingActions '%s' site %u", userId, siteId);
	INT16 errCode = obj->getPendingActions(userId, siteId, actions, errMsg);
	CommMsgBody replyBody;
	replyBody
		.composeUINT32(reqId)
		.composeINT16(errCode);
	if (errCode)
	{
		replyBody.composeString(errMsg);
		PLog("getPendingActions '%s' err %d '%s'", userId, errCode, errMsg.c_str());
	}
	else
	{
		composeVectorOfComposableValues(actions, replyBody);
	}
	post(MSG_DNSERVER_SRV_GET_PENDING_ACTIONS_REPLY, replyBody);
}

void DuelNotifSrvConn::replyGetPendingActionsCount(const char* userId, UINT32 siteId, UINT32 reqId)
{
	vector<DuelPendingAction> actions;
	PString errMsg;
	PTRACE5("getPendingActionCount '%s' site %u", userId, siteId);
	INT16 errCode = obj->getPendingActions(userId, siteId, actions, errMsg);
	CommMsgBody replyBody;
	replyBody
		.composeUINT32(reqId)
		.composeINT16(errCode);
	if (errCode)
	{
		replyBody.composeString(errMsg);
		PLog("getPendingActions '%s' err %d '%s'", userId, errCode, errMsg.c_str());
	}
	else
	{
		replyBody.composeUINT32(actions.size());
	}
	post(MSG_DNSERVER_SRV_GET_PENDING_ACTIONS_COUNT_REPLY, replyBody);
}

////////////////////////////////////////////////////////////////////////////////
