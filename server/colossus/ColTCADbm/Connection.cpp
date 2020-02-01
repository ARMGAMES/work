// ===========================================================
// Copyright (C) 2018 PYR Software Ltd. All rights reserved.
// ===========================================================

#include "ColTCADbmobject.h"
#include "msguserauth.h"
#include "getservconnip.h"

////////////////////////////////////////////////////////////////////////////////

void ColTCADbmConnection::processMessage(UINT32 msgId, const CommMsgBody& body)
{
	obj->processColTCADbmServerMessage(msgId, body, this);
}

///////////////////////////////////////////////////////////////////////////////
// Admin Processing

void ColTCADbmAdminServerConnection::connected()
{
	PLog("Admin %s connected to '%s'", adminId.c_str(), getServerConnType());

	CommMsgBody request;
	composeLoginConfirmMsg(adminId, request);
	obj->postToLogin(DBM_Q_LOGIN_CONFIRM, request);
}

void ColTCADbmAdminServerConnection::closedOrDisconnected( int errCode, const char* errMsg )
{
	PLog("Admin %s disconnected from '%s': %d '%s'", adminId.c_str(), getServerConnType(), errCode, errMsg);

	if (adminId.length() > 0 && !authError)
	{
		// Translate specific codes
		if (errCode == _COMM_ERROR_PHYSICAL_DISCONNECTED)
			errCode = LOGOUT_DISCONNECTED;

		CommMsgBody body;
		composeUserLogout(OBJ_TYPE_COLDBM, 0, adminId, errCode, body);
		obj->postToLogin(DBM_Q_USER_LOGOUT, body);
	}
}

void ColTCADbmAdminServerConnection::getUserAuth( const char* guardType, const char* user, CommMsgBody& /*clientExtra*/, CommMsgBody& authBody )
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
	composeUserAuthMsg(user, "", "", "", ipAddr, port, OBJ_TYPE_COLDBM, 0, authBody, 0, 0, guardType, request);

	obj->postToLogin(DBM_Q_USER_AUTH, request, new AdminAuthAsyncCall(obj, this));
}

///////////////////////////////////////////////////////////////////////////////

void ColTCADbmTrustedAdminServerConnection::parseInfo(CommMsgParser& parser)
{
	UINT32 size;
	parser.parseStringP(adminId)
		.parseUINT32(userIntId)
		.parseUINT32(size);

	for (UINT32 i = 0; i < size; ++i )
	{
		const char* right;
		parser.parseString(right);
		adminRights.insert(right);
	}
}

/////////////////////////////////////////////////////////////////////////////////

void AdminAuthAsyncCall::processReply( UINT32 requestId, UINT32 msgId, const CommMsgBody& body )
{
	ColTCADbmAdminServerConnection* pConnection = dynamic_cast<ColTCADbmAdminServerConnection*>(findConnection());
	if (pConnection)
	{
		PLog("<<ADMIN %u [%08X] connId=%08X", msgId, requestId, pConnection->id());
		pObj->receiveAdminAuth(body, pConnection);
	}
}

////////////////////////////////////////////////////////////////////////////////////

void ColTCADbmOutgoingConnection::connected()
{
	isConnected = 1;
	PLog("Outgoing connection to %s connected", name.c_str());
}


bool ColTCADbmOutgoingConnection::closedOrDisconnected(int errCode, const char* errMsg)
{
	bool rc = CommClientGConnection::closedOrDisconnected(errCode, errMsg);
	PLog("Outgoing connection disconnected from %s, err=%d (%s), reconnect=%s",
	     name.c_str(), errCode, errMsg, rc ? "no" : "yes");
	isConnected = 0;
	return rc;
}


void ColTCADbmOutgoingConnection::processGMessage(UINT32 reqId, UINT32 msgId, const CommMsgBody& body, AsyncCall* call)
{
	PLog("<%c %u [%08X]", symbol, msgId, reqId);
	if (call)
		obj->processCallback(reqId, msgId, body, call);
	else
		PLog("No AsyncCall, message not handled");
}

UINT32 ColTCADbmOutgoingConnection::post(UINT32 msgId, CommMsgBody& body, AsyncCall* call)
{
	UINT32 reqId = CommClientGConnection::post(msgId, body, call);
	PLog(">%s %u %08X [%08X]%s", getName(), msgId, underlyingConnection().id(), reqId, call ? "" : " (no AsyncCall)");
	return reqId;
}