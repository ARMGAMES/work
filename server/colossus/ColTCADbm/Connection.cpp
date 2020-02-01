
#include "ColTCADbmobject.h"
#include "msguserauth.h"
#include "getservconnip.h"

const char* getConnMarker(int connType)
{
	PASSERT(connType > LowerGuard && connType < UpperGuard);
	return CONN_TYPES[connType].marker;
}

////////////////////////////////////////////////////////////////////////////////

GenericSrvConnection::GenericSrvConnection(ColTCADbmServerObject& obj_, ServerConnectionType connectionType_)
	:obj(obj_), connectionType(connectionType_)
{}

ServerConnectionType GenericSrvConnection::getConnType() const
{
	return connectionType;
}

const char* GenericSrvConnection::getConnMarker() const
{
	return ::getConnMarker(connectionType);
}

void GenericSrvConnection::processMessage(UINT32 msgId, const CommMsgBody& body)
{
	obj.processCommonServerMessage(msgId, body, connectionType, this);
}

///////////////////////////////////////////////////////////////////////////////
// Privileged Processing

GenericPrivilegedSrvConnection::GenericPrivilegedSrvConnection(ColTCADbmServerObject& obj_, ServerConnectionType connectionType_) :
	GenericSrvConnection(obj_, connectionType_), userIntId(0)
{}

const char* GenericPrivilegedSrvConnection::getAdminId() const
{ 
	return adminId.c_str(); 
}

UINT32 GenericPrivilegedSrvConnection::getUserIntId() const 
{ 
	return userIntId; 
}

void GenericPrivilegedSrvConnection::setUserIntId(UINT32 intId)
{ 
	userIntId = intId; 
}

void GenericPrivilegedSrvConnection::addAdminRight(const char* right) 
{ 
	adminRights.insert(right); 
}

bool GenericPrivilegedSrvConnection::checkAdminRight(const char* right) const 
{ 
	return adminRights.find(right) != adminRights.end(); 
}

void GenericPrivilegedSrvConnection::getAdminRights(vector<PString>& rights) const
{
	for (const auto& right : adminRights)
	{
		rights.push_back(right);
	}
}


///////////////////////////////////////////////////////////////////////////////
// Admin Processing

AdminSrvConnection::AdminSrvConnection(ColTCADbmServerObject& obj_) :
	GenericPrivilegedSrvConnection(obj_, Admin), authError(0)
{}

void AdminSrvConnection::connected()
{
	GenericPrivilegedSrvConnection::connected();

	PLog("Admin %s connected to '%s'", adminId.c_str(), getServerConnType());

	CommMsgBody request;
	composeLoginConfirmMsg(adminId, request);
	obj.postToLogin(DBM_Q_LOGIN_CONFIRM, request);
}

void AdminSrvConnection::closedOrDisconnected( int errCode, const char* errMsg )
{
	GenericPrivilegedSrvConnection::closedOrDisconnected(errCode, errMsg);

	PLog("Admin %s disconnected from '%s': %d '%s'", adminId.c_str(), getServerConnType(), errCode, errMsg);

	if (adminId.length() > 0 && !authError)
	{
		// Translate specific codes
		if (errCode == _COMM_ERROR_PHYSICAL_DISCONNECTED)
		{
			errCode = LOGOUT_DISCONNECTED;
		}

		CommMsgBody body;
		composeUserLogout(OBJ_TYPE_COLDBM, 0, adminId, errCode, body);
		obj.postToLogin(DBM_Q_USER_LOGOUT, body);
	}
}

void AdminSrvConnection::saveAuthError(INT16 errCode)
{
	authError = errCode;
}

void AdminSrvConnection::getUserAuth( const char* guardType, const char* user, CommMsgBody& /*clientExtra*/, CommMsgBody& authBody )
{
	if(!strcmp(guardType, _COMM_NULL_GUARD_NAME)) 
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

	obj.postToLogin(DBM_Q_USER_AUTH, request, 
		new AdminAuthAsyncCall(obj, &ColTCADbmServerObject::receiveAdminAuth, this->id()));
}

///////////////////////////////////////////////////////////////////////////////

TrustedAdminSrvConnection::TrustedAdminSrvConnection(ColTCADbmServerObject& obj_) :
	GenericPrivilegedSrvConnection(obj_, TrustedAdmin)
{}

void TrustedAdminSrvConnection::parseInfo(CommMsgParser& parser)
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

////////////////////////////////////////////////////////////////////////////////////

void GenericOutgoingConnection::connected()
{
	CommClientGConnection::connected();

	isConnected = 1;
	PLog("Outgoing connection to %s connected", name.c_str());
}


bool GenericOutgoingConnection::closedOrDisconnected(int errCode, const char* errMsg)
{
	bool rc = CommClientGConnection::closedOrDisconnected(errCode, errMsg);
	isConnected = 0;
	PLog("Outgoing connection disconnected from %s, err=%d (%s), reconnect=%s",
	     name.c_str(), errCode, errMsg, rc ? "no" : "yes");
	
	return rc;
}


void GenericOutgoingConnection::processGMessage(UINT32 reqId, UINT32 msgId, const CommMsgBody& body, AsyncCall* call)
{
	CommClientGConnection::processGMessage(reqId, msgId, body, call);

	PLog("<%s %c %u [%08X]", getName(), msgId, reqId);
	if (call)
		obj.processCallback(reqId, msgId, body, call);
	else
		PLog("No AsyncCall, message not handled");
}

UINT32 GenericOutgoingConnection::post(UINT32 msgId, CommMsgBody& body, AsyncCall* call)
{
	UINT32 reqId = CommClientGConnection::post(msgId, body, call);
	PLog(">%s %u %08X [%08X]%s", getName(), msgId, underlyingConnection().id(), reqId, call ? "" : " (no AsyncCall)");
	return reqId;
}

UINT32 GenericOutgoingConnection::getConnId() const
{
	return underlyingConnection().id();
}

