// ===========================================================
// Copyright (C) 2018 PYR Software Ltd. All rights reserved.
// ===========================================================

#include	"TCADetector.h"
#include	"ColTCADbmobject.h"
#include	"admincommon.h"

void ColTCADbmServerObject::processAdminMessage(UINT32 msgId, const CommMsgBody& body, ColTCADbmConnection* conn)
{
	ColTCADbmAdminServerConnection* adminConn = dynamic_cast<ColTCADbmAdminServerConnection*>(conn);
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


void ColTCADbmServerObject::processTrustedAdminMessage(UINT32 msgId, const CommMsgBody& body, ColTCADbmConnection* conn)
{
	ColTCADbmTrustedAdminServerConnection* tadminConn = dynamic_cast<ColTCADbmTrustedAdminServerConnection*>(conn);
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

void ColTCADbmServerObject::processKillServer(const char* adminId)
{
	PLog("ColTCA::processKillServer: admin id: %s", adminId);

	shutdown();
	this->exit();
	PLog("Exit");
}

void ColTCADbmServerObject::processRereadSettings(const char* adminId, CommMsgBody& reply)
{
	PLog("ColTCA::processRereadSettings: admin id: %s", adminId);
	dynamicInit();

	setReply(reply, DBM_NO_ERROR);
}

void ColTCADbmServerObject::receiveAdminAuth( const CommMsgBody& body, ColTCADbmAdminServerConnection* pConn )
{
	CommMsgParser parser(body);

	INT16 errCode;
	parser.parseINT16(errCode);
	pConn->saveAuthError(errCode);

	if (errCode)
	{
		const char* errDescr;
		parser.parseString(errDescr);
		pConn->gotUserAuth(0, errCode, errDescr);

		PLog("ColTCADbmServerObject::receiveAdminAuth error %d (%s)", errCode, errDescr);
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
		pConn->addAdminRight(right);
	}

	// Skip the rest of the message as it is not needed.

	pConn->setUserIntId(userIntId);

	CommMsgParser authParser(authBody);
	if (!pConn->gotUserAuth(&authParser))
		PLog("Authentication failed");
}

bool ColTCADbmServerObject::checkAdminRight(const char* right, const ColTCADbmPrivilegedConnection* pConn, CommMsgBody& reply)
{
	if (pConn->checkAdminRight(right))
		return true;

	setReply(reply, NOT_ALLOWED, "Insufficient rights");
	return false;
}