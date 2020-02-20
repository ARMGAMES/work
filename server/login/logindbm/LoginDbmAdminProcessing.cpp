
#include "LoginDbmobject.h"
#include "admincommon.h"

void LoginDbmServerObject::processAdminMessage(UINT32 msgId, const CommMsgBody& body, GenericSrvConnection* conn)
{
	AdminSrvConnection* adminConn = dynamic_cast<AdminSrvConnection*>(conn);
	if (!adminConn)
	{
		PLog("Incoming connection from admin lost!");
		return;	// no reply!
	}

	PLog("AdminConnection-Id:%s", adminConn->getAdminId());

	CommMsgParser parser(body);
	UINT32 reqId;
	parser.parseUINT32(reqId);

	UINT32 replyId = msgId + 1; // By local convention
	CommMsgBody reply;
	reply.composeUINT32(reqId);

	switch (msgId)
	{
	case MSG_ADMIN_KILL_ITSELF:
		if (checkAdminRight(HANDLE_KILL_ITSELF, adminConn, reply))
		{
			return processKillServer(adminConn->getAdminId());
		}
		break;

	case MSG_ADMIN_REREAD_INI:
		if (checkAdminRight(SYSADMIN_STR, adminConn, reply))
		{
			processRereadSettings(adminConn->getAdminId(), reply);
		}
		break;
	default:
		PLog("Error: Unhandled Admin message #%u from: %s", msgId, adminConn->getAdminId());
		return; // No reply!
	}

	postReply(adminConn, replyId, reply);
}


void LoginDbmServerObject::processTrustedAdminMessage(UINT32 msgId, const CommMsgBody& body, GenericSrvConnection* conn)
{
	TrustedAdminSrvConnection* tadminConn = dynamic_cast<TrustedAdminSrvConnection*>(conn);
	if (!tadminConn)
	{
		PLog("Incoming connection from admin lost!");
		return;	// no reply!
	}

	PLog("TAdminConnection-Id:%s", tadminConn->getAdminId());

	UINT32 reqId;
	CommMsgParser outerParser(body);
	outerParser.parseUINT32(reqId);

	// Extract the admin credentials and the actual message body from the wrapper
	// See MsgAuthRedirectorToGateway for reference
	tadminConn->parseInfo(outerParser);

	CommMsgBody actualBody;
	outerParser.parseMsgBody(actualBody);
	CommMsgParser parser(actualBody);

	UINT32 replyId = msgId + 1; // By local convention
	CommMsgBody reply;
	reply.composeUINT32(reqId);

	switch (msgId)
	{
		case MSG_ADMIN_KILL_ITSELF:
			if (checkAdminRight(HANDLE_KILL_ITSELF, tadminConn, reply))
			{
				return processKillServer(tadminConn->getAdminId());
			}
			break;

		case MSG_ADMIN_REREAD_INI:
			if (checkAdminRight(SYSADMIN_STR, tadminConn, reply))
			{
				processRereadSettings(tadminConn->getAdminId(), reply);
			}
			break;

		default:
			PLog("Error: Unhandled TrustedAdmin message #%u from: %s", msgId, tadminConn->getAdminId());
			return; // No reply!
	}

	postReply(tadminConn, replyId, reply);
}

void LoginDbmServerObject::processKillServer(const char* adminId)
{
	PLog("processKillServer: admin id: %s", adminId);

	shutdown();
	this->exit();
	PLog("Exit");
}

void LoginDbmServerObject::processRereadSettings(const char* adminId, CommMsgBody& reply)
{
	PLog("processRereadSettings: admin id: %s", adminId);
	dynamicInit();

	setReply(reply, DBM_NO_ERROR);
}

void LoginDbmServerObject::receiveAdminAuth(UINT32 /*replyId*/, CommMsgParser& parser, AdminAuthAsyncCall& callState)
{
	INT16 errCode;
	parser.parseINT16(errCode);
	
	UINT32 connId = callState.data;
	CommServerConnection* pConn = findConnection(connId);
	AdminSrvConnection* adminConn = dynamic_cast<AdminSrvConnection*>(pConn);

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
	parser.parseMsgBody(authBody)
		.parseUINT32(dummyUint32)	// Privileges
		.parseString(pDummyStr)	// User city
		.parseUINT32(dummyUint32)	// Flags
		.parseString(pDummyStr)	// User email
		.parseUINT32(userIntId)
		.parseString(pDummyStr)	// User country
		.parseString(pDummyStr)	// User state
		.parseMsgBody(dummyBody)	// Obsolete
		.parseUINT16(numRights);

	// Extract and store rights
	for (UINT32 i = 0; i < numRights; ++i)
	{
		const char* right;
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

bool LoginDbmServerObject::checkAdminRight(const char* right, const GenericPrivilegedSrvConnection* pConn, CommMsgBody& reply)
{
	if (pConn->checkAdminRight(right))
		return true;

	setReply(reply, NOT_ALLOWED, "Insufficient rights");
	return false;
}