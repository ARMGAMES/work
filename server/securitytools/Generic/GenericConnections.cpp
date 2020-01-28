////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2019 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "GenericConnections.h"
#include "GenericSrvObject.h"
#include "msguserauth.h"
#include "getservconnip.h"

////////////////////////////////////////////////////////////////////////////////

GenericSrvConn::GenericSrvConn(GenericServerObjectBase& obj_, ServerConnectionType connectionType_)
	: obj(obj_), connectionType(connectionType_)
{

}

void GenericSrvConn::processMessage(UINT32 msgId, const CommMsgBody& body)
{
	obj.processCommonServerMessage(msgId, body, connectionType, this);
}

// May throw
const char* GenericSrvConn::getConnMarker() const
{
	return ::getConnMarker(connectionType);
}

////////////////////////////////////////////////////////////////////////////////

GenericPrivilegedSrvConn::GenericPrivilegedSrvConn(GenericServerObjectBase& obj_, ServerConnectionType connectionType_)
	: GenericSrvConn(obj_, connectionType_), userIntId(0)
{

}

const char* GenericPrivilegedSrvConn::getAdminId() const
{
	return adminId.c_str();
}

UINT32 GenericPrivilegedSrvConn::getUserIntId() const
{
	return userIntId;
}

void GenericPrivilegedSrvConn::setUserIntId(UINT32 userIntId_)
{
	userIntId = userIntId_;
}

void GenericPrivilegedSrvConn::addAdminRight(const char* right)
{
	adminRights.insert(right);
}

bool GenericPrivilegedSrvConn::checkAdminRight(const char* right) const
{
	return adminRights.find(right) != adminRights.end();
}

void GenericPrivilegedSrvConn::getAdminRights(vector<PString>& rights) const
{
	for (const auto& right : adminRights)
	{
		rights.push_back(right);
	}
}

void GenericPrivilegedSrvConn::getExtraLogMarkers(PString& logStr) const
{
	logStr.append(" ").append(getAdminId());
}

////////////////////////////////////////////////////////////////////////////////

AdminSrvConnBase::AdminSrvConnBase(GenericServerWithLoginAuthObjectBase& obj_) :
	GenericPrivilegedSrvConn(obj_, Admin), loginAuthSrvObj(obj_), authError(0)
{

}

void AdminSrvConnBase::connected()
{
	GenericPrivilegedSrvConn::connected();

	PLog("Admin %s connected to '%s'", adminId.c_str(), getServerConnType());

	CommMsgBody request;
	composeLoginConfirmMsg(adminId, request);
	loginAuthSrvObj.postToLoginAuth(DBM_Q_LOGIN_CONFIRM, request);
}

void AdminSrvConnBase::closedOrDisconnected(int errCode, const char* errMsg)
{
	GenericPrivilegedSrvConn::closedOrDisconnected(errCode, errMsg);

	PLog("Admin %s disconnected from '%s': %d '%s'", adminId.c_str(), getServerConnType(), errCode, errMsg);

	if (adminId.length() > 0 && !authError)
	{
		// Translate specific codes
		if (errCode == _COMM_ERROR_PHYSICAL_DISCONNECTED)
		{
			errCode = LOGOUT_DISCONNECTED;
		}

		CommMsgBody body;
		composeUserLogout(getSrvObjType(), 0, adminId, errCode, body);
		loginAuthSrvObj.postToLoginAuth(DBM_Q_USER_LOGOUT, body);
	}
}

void AdminSrvConnBase::saveAuthError(INT16 errCode)
{
	authError = errCode;
}

void AdminSrvConnBase::getUserAuth(const char* guardType, const char* user, CommMsgBody& /*clientExtra*/, CommMsgBody& authBody)
{
	if (!strcmp(guardType, _COMM_NULL_GUARD_NAME)) //PYR-44258
	{
		gotUserAuth(nullptr);
		return;
	}

	adminId.assign(user);

	PString ipAddr;
	int port = 0;
	getServConnIp(*this, ipAddr, port);

	CommMsgBody request;
	composeUserAuthMsg(user, "", "", "", ipAddr, port, getSrvObjType(), 0, authBody, 0, 0, guardType, request);

	loginAuthSrvObj.postToLoginAuth(DBM_Q_USER_AUTH, request,
		new AdminAuthAsyncCall(loginAuthSrvObj, &GenericServerWithLoginAuthObjectBase::receiveAdminAuth, this->id()));
}

////////////////////////////////////////////////////////////////////////////////

TrustedAdminSrvConn::TrustedAdminSrvConn(GenericServerObjectBase& obj_)
	: GenericPrivilegedSrvConn(obj_, TrustedAdmin)
{

}

void TrustedAdminSrvConn::parseInfo(CommMsgParser& parser)
{
	UINT32 size;
	parser.
		parseStringP(adminId).
		parseUINT32(userIntId).
		parseUINT32(size);

	for (UINT32 i = 0; i < size; ++i)
	{
		const char* right;
		parser.parseString(right);
		adminRights.insert(right);
	}
}

////////////////////////////////////////////////////////////////////////////////

void GenericClientGConnection::connected()
{
	CommClientGConnection::connected();

	isconnected = 1;

	PLog("Outgoing connection to %s connected", name.c_str());
}

bool GenericClientGConnection::closedOrDisconnected(int errCode, const char* errMsg)
{
	bool res = CommClientGConnection::closedOrDisconnected(errCode, errMsg);

	isconnected = 0;

	PLog("Outgoing connection disconnected from %s, err=%d (%s), reconnect=%s",
		name.c_str(), errCode, errMsg, res ? "no" : "yes");

	return res;
}

UINT32 GenericClientGConnection::post(UINT32 msgId, CommMsgBody& body, AsyncCall* call)
{
	UINT32 reqId = CommClientGConnection::post(msgId, body, call);
	PLog(">%s %u %08X [%08X]%s", getName(), msgId, getConnId(), reqId, call ? "" : " (no AsyncCall)");
	return reqId;
}

void GenericClientGConnection::processGMessage(UINT32 reqId, UINT32 msgId, const CommMsgBody& body, AsyncCall* call)
{
	CommClientGConnection::processGMessage(reqId, msgId, body, call);

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

UINT32 GenericClientGConnection::getConnId() const
{
	return underlyingConnection().id();
}

////////////////////////////////////////////////////////////////////////////////
