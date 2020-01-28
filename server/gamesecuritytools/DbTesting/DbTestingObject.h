#ifndef DbTestingObject_h_included
#define DbTestingObject_h_included

//////////////////////////////////////////////////////////////////////////
// Copyright (C) 2013 PYR Software Ltd. All rights reserved.
//////////////////////////////////////////////////////////////////////////

#include "commlib.h"
#include "DbTestingDbmManager.h"

class DbTestingObject;

//////////////////////////////////////////////////////////////////////////

class DbTestingServerConnection : public CommServerConnection
{
public:
	DbTestingServerConnection(DbTestingObject* obj_) : obj(obj_) { }
	virtual void processMessage(UINT32 msgId, const CommMsgBody& body);

protected:
	DbTestingObject* obj;
};

//////////////////////////////////////////////////////////////////////////

class DbTestingServerConnFactory : public CommServerConnectionFactory
{
public:
	DbTestingServerConnFactory(DbTestingObject* obj_, CommServerGuardFactory& guardFactory) :
		CommServerConnectionFactory(guardFactory), obj(obj_)
	{}

	/* virtual */ /* new */ CommServerConnection* createConnection()
	{
		return new DbTestingServerConnection(obj);
	}

private:
	DbTestingObject* obj;
};

//////////////////////////////////////////////////////////////////////////

class DbTestingObject : public CommServerObject
{
public:
	explicit DbTestingObject(_CommInterface& inter) :
		CommServerObject(inter),
		useLogger(false), 
		srvPool(inter), cliPool(inter),
		connFactory(this, guardFactory)
	{ }
	~DbTestingObject() { }

	void init(CommMsgBody& initMsg) override;

	void processMessage(_CommMsg& msg) override
	{
		cliPool._processMessage(msg) || srvPool._processMessage(msg);
	}

	void processServerMessage(UINT32 msgId, const CommMsgBody& body, CommServerConnection* conn);

private:

	void processGetAggEndDate(CommMsgParser& parser, CommServerConnection* conn);
	void processRefreshSettings(CommMsgParser& parser, CommServerConnection* conn);

	static CommServerNullGuardFactory guardFactory;

	DbTestingDbmManager dbManagerMain;

	PString fullFileName;
	PString initDir;

	bool useLogger;
	PString loggerServerAddress;
	PString loggerStationName;

	CommServerConnectionPool srvPool;
	CommClientConnectionPool cliPool;
	DbTestingServerConnFactory connFactory;
};

//////////////////////////////////////////////////////////////////////////

class DbTestingObjectFactory : public CommServerObjectFactory
{
public:
	/* virtual */ /* new */ CommServerObject* createServerObject(_CommInterface& inter) const
	{
		return new DbTestingObject(inter);
	}
};

//////////////////////////////////////////////////////////////////////////

#endif // DbTestingObject_h_included