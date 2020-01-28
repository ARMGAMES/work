////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2013 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef LoopbackObject_h_included
#define LoopbackObject_h_included

#include "commsrv.h"
#include "commgrdnull.h"

static const UINT32 MESSAGE_REPLY_OFFSET = 1;

////////////////////////////////////////////////////////////////////////////////

class LoopbackServerObject;

//////////////////////////////////////////////////////////////////////////////////

class IncomingConnection : public CommServerConnection
{
public:
	explicit IncomingConnection(LoopbackServerObject* obj_) : obj(obj_) {}
	void connected() override;
	void closedOrDisconnected(int errCode, const char* errMsg) override;
	void processMessage(UINT32 msgId, const CommMsgBody& body) override;

private:
	LoopbackServerObject* obj;
};

//////////////////////////////////////////////////////////////////////////////////

class ConnFactory : public CommServerConnectionFactory
{
public:
	ConnFactory(CommServerGuardFactory& guardFactory, LoopbackServerObject* pObj_) :
		CommServerConnectionFactory(guardFactory), pObj(pObj_)
	{}

	CommServerConnection* createConnection() override
	{
		return new IncomingConnection(pObj);
	}

private:
	LoopbackServerObject* pObj;
};

//////////////////////////////////////////////////////////////////////////////////

class LoopbackServerObject : public CommServerObject
{
public:
	explicit LoopbackServerObject(_CommInterface& iface);
	void init(CommMsgBody& initMsg) override;
	void processMessage(_CommMsg& msg) override
	{
		srvPool._processMessage(msg);
	}

private:
	static CommServerNullGuardFactory guardFactory;

	CommServerConnectionPool srvPool;
	ConnFactory connFactory;

private:
	PString initDir;
};

////////////////////////////////////////////////////////////////////////////////

class LoopbackServerObjectFactory : public CommServerObjectFactory
{
public:
	CommServerObject* createServerObject(_CommInterface& inter) const override
	{
		return new LoopbackServerObject(inter);
	}
};

////////////////////////////////////////////////////////////////////////////////

#endif // LoopbackObject_h_included
