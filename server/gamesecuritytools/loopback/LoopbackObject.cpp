////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2013 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "Loopbackobject.h"
#include "pyrtime.h"

////////////////////////////////////////////////////////////////////////////////

CommServerNullGuardFactory LoopbackServerObject::guardFactory;

////////////////////////////////////////////////////////////////////////////////

static char const* getTime(PString& str)
{
	int msec;
	SrvTime t(TIMEMSEC(&msec));

	char buf[64] = "";
	sprintf(buf, "%04u-%02u-%02u %02u:%02u:%02u.%d", t._year, t._month, t._day, t._hour, t._minute, t._second, msec);

	str = buf;
	return str;
}

////////////////////////////////////////////////////////////////////////////////

void IncomingConnection::connected()
{
	PString str;
	PTRACE("==> [%s] Connected", getTime(str));
}


void IncomingConnection::closedOrDisconnected(int errCode, const char* errMsg)
{
	PString str;
	PTRACE("==> [%s] Disonnected: %d - %s", getTime(str), errCode, errMsg);
}


void IncomingConnection::processMessage(UINT32 msgId, const CommMsgBody& body)
{
	PString str;
	PTRACE("==> [%s] Message: %u", getTime(str), msgId);

	CommMsgBody reply;
	reply.copyFrom(body);
	post(msgId + MESSAGE_REPLY_OFFSET, reply);
}

////////////////////////////////////////////////////////////////////////////////

LoopbackServerObject::LoopbackServerObject(_CommInterface& iface) :
	CommServerObject(iface),
	srvPool(iface),
	connFactory(guardFactory, this)
{}


void LoopbackServerObject::init(CommMsgBody& initMsg)
{
	CommMsgParser parser(initMsg);
	parser.parseStringP(initDir);

	srvPool.registerFactory("loopback", connFactory);
}

////////////////////////////////////////////////////////////////////////////////
