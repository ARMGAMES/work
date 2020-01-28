// ===========================================================
// Copyright (C) 2014 PYR Software Ltd. All rights reserved.
// ===========================================================

#ifndef PROJECTobject_h_included
#define PROJECTobject_h_included

#include "ppstring.h"
#include "commlib.h"
#include "commsrv.h"
#include "commgrdnull.h"
#include "counters.h"
#include "heLogger.h"

#define LOGGER_TIMER						100
#define TIMER_LOGGER_MSEC                   15000

class PROJECTServerObject;

//////////////////////////////////////////////////////////////////////////////////

enum PROJECTConnectionType
{
	TrustedAdmin = 1,
	SomeType,																						// any descriptive enum for connection type - OlapMd, Colossus, etc...
};

class PROJECTServerConnection : public CommServerConnection
{
protected:
	PROJECTServerObject* obj;

public:
	PROJECTServerConnection( PROJECTServerObject* obj_ )
	{
		obj = obj_;
	}
	void processMessage( UINT32 msgId, const CommMsgBody& body ) override;
	void connected() override;
	void closedOrDisconnected( int errCode, const char* errMsg ) override;
};

class AdminConnection : public CommServerConnection
{
protected:
	PROJECTServerObject* obj;

public:
	AdminConnection( PROJECTServerObject* obj_ )
	{
		obj = obj_;
	}
	void processMessage( UINT32 msgId, const CommMsgBody& body ) override;
	void connected() override;
	void closedOrDisconnected( int errCode, const char* errMsg ) override;
};

class PROJECTServerAuthConnection : public CommServerConnection
{
protected:
	PROJECTServerObject* obj;
	PString user;

public:
	PROJECTServerAuthConnection( PROJECTServerObject* obj_ )
	{
		obj = obj_;
	}
	void processMessage( UINT32 msgId, const CommMsgBody& body ) override;
	void connected() override;
	void closedOrDisconnected( int errCode, const char* errMsg ) override;
	void getUserAuth( const char* guardType, const char* user_, CommMsgBody& clientExtra, CommMsgBody& request ) override;
	const char * getUser() const {return user.c_str();}
};


class PROJECTClientConnection: public CommClientConnection
{
public:
	PROJECTServerObject* obj;

	PROJECTClientConnection(CommClientGuardFactory& guardFactoryClient )
	: CommClientConnection( guardFactoryClient )
	{
		obj = 0;
	}

public://overridden
	void connected() override;
	bool closedOrDisconnected( int errCode, const char* errMsg ) override;
	void processMessage( UINT32 msgId, const CommMsgBody& body ) override;
	bool isTheSame(const char * server,const char * serverObject, const char * connectionType)
	{
		return (!strcmp(getServer(),server) &&
			    !strcmp(getServerObject(),serverObject) &&
				!strcmp(getServerConnType(),connectionType)
				);
	}

};

class PROJECTClientGConnection : public CommClientGConnection
{
protected:
	int						isconnected;

public:
	PROJECTServerObject* obj;

public:
	PROJECTClientGConnection(CommClientGuardFactory& guardFactoryClient )
	: CommClientGConnection( guardFactoryClient )
	{
		obj = 0;
		isconnected = 0;
	}

public:
	void connected() override;
	bool closedOrDisconnected( int errCode, const char* errMsg ) override;
	void processGMessage( UINT32 requestId, UINT32 msgId, const CommMsgBody& body, AsyncCall* call ) override;
	bool isTheSame(const char * server,const char * serverObject, const char * connectionType)
	{
		return (!strcmp(underlyingConnection().getServer(),server) &&
			    !strcmp(underlyingConnection().getServerObject(),serverObject) &&
				!strcmp(underlyingConnection().getServerConnType(),connectionType)
				);
	}

	int isConnected() const
	{
		return isconnected;
	}
};

class PROJECTClientAuthConnection : public CommClientGConnection
{
	public:
	PROJECTServerObject* obj;

	public:
	PROJECTClientAuthConnection(PROJECTServerObject & obj_,CommClientGuardFactory& guardFactoryClient )
	: CommClientGConnection( guardFactoryClient ) 
	{
		obj = &obj_;
	}

	public://overridden
	void connected() override;
	bool closedOrDisconnected( int errCode, const char* errMsg ) override;
	void processGMessage( UINT32 requestId, UINT32 msgId, const CommMsgBody& body, AsyncCall* call ) override;
	bool isTheSame(const char * server,const char * serverObject, const char * connectionType)
	{
		return (!strcmp(underlyingConnection().getServer(),server) &&
			    !strcmp(underlyingConnection().getServerObject(),serverObject) &&
				!strcmp(underlyingConnection().getServerConnType(),connectionType)
				);
	}

};


//////////////////////////////////////////////////////////////////////////////////

class PROJECTServerConnFactory : public CommServerConnectionFactory
{
public:
	PROJECTServerObject* obj;

public:
	PROJECTServerConnFactory( CommServerGuardFactory& guardFactory )
	: CommServerConnectionFactory( guardFactory )
	{
		obj = 0;
	}

	 /* new */ CommServerConnection* createConnection() override
	{
		return new PROJECTServerConnection( obj );
	}
};

class PROJECTServerAuthConnFactory : public CommServerConnectionFactory
{
public:
	PROJECTServerObject* obj;

public:
	PROJECTServerAuthConnFactory( CommServerGuardFactory& guardFactory )
	: CommServerConnectionFactory( guardFactory )
	{
		obj = 0;
        ++normalPriority;
        ++authPriority;
	}

	 /* new */ CommServerConnection* createConnection() override
	{
		return new PROJECTServerAuthConnection( obj );
	}
};

class AdminConnFactory : public CommServerConnectionFactory
{
public:
	PROJECTServerObject* obj;

public:
	AdminConnFactory( CommServerGuardFactory& guardFactory )
		: CommServerConnectionFactory( guardFactory )
	{
		obj = 0;
		++normalPriority;
		++authPriority;
	}

	/* new */ CommServerConnection* createConnection() override
	{
		return new AdminConnection( obj );
	}
};



//////////////////////////////////////////////////////////////////////////////////
class PROJECTServerObject : public CommServerObject
{
	static CommServerNullGuardFactory	guardFactory;
	static CommClientNullGuardFactory	guardFactoryClient;
	static CommServerPlainTextPasswordSidGuardFactory guardPwdFactory;

	CommServerConnectionPool			srvPool;
	CommClientConnectionPool			cliPool;

	PROJECTServerConnFactory			connPROJECTFactory;
	PROJECTServerAuthConnFactory		connPROJECTAuthFactory;
	AdminConnFactory					adminTrustedFactory;
	PROJECTClientConnection				clientConnection;
	PROJECTClientGConnection			clientGConnection;
	PROJECTClientAuthConnection*		connectionToAuthDbm;

	PString								fullFileName;
	PString								initDir;
	UINT32								heLoggerTimerHandle;
	HeLogger*							heLogger;

public:
	TicksAccumulator					ticksAccumulator;

public:
	PROJECTServerObject( _CommInterface& inter );
	~PROJECTServerObject() 
	{
		if (heLogger)
		{
			stopHeLogger();
			delete heLogger;
			heLogger = NULL;
		}

		if (connectionToAuthDbm)
		{
			connectionToAuthDbm->closedOrDisconnected(0,"");
			cliPool.disconnect(connectionToAuthDbm->underlyingConnection());
			delete connectionToAuthDbm;
			connectionToAuthDbm = NULL;
		}
	}

	
	void init(CommMsgBody& initMsg) override;
	void dynamicInit();

	void processServerMessage( UINT32 msgId, const CommMsgBody& body, PROJECTServerConnection* conn );
	void processServerAuthMessage( UINT32 msgId, const CommMsgBody & body, PROJECTServerAuthConnection* conn );
	
	virtual void processTimerMessage(UINT32 handle, UINT32 msgId, CommMsgBody& body) override;
	void processMessage( _CommMsg& msg ) override
	{
		srvPool._processMessage( msg );
		cliPool._processMessage( msg );
	}

private:
	void processSomeTypeMessage( UINT32 msgId, const CommMsgBody & body, PROJECTServerConnection* conn );
	bool checkAdminRight(const char* rightName, UINT32 replyMsgId, UINT32 reqId, const vector<PString>& adminRights, CommServerConnection* conn) const;
	void processAdminMessage(UINT32 msgId, const CommMsgBody& body, AdminConnection* conn);
	void startHeLogger(const char * strServerAddress, const char * strStationName);
	void stopHeLogger();
	void reportPerformance();

public:
	void processClientMessage(UINT32 msgId, const CommMsgBody& body );
	void processClientGMessage(UINT32 requestId, UINT32 msgId, const CommMsgBody& body );
	void processClientAuthMessage(UINT32 requestId, UINT32 msgId, const CommMsgBody& body );
	void receiveAuthFromDbm( CommMsgParser& parser, UINT32 connId );
	void requestUserAuth( CommMsgBody & request,PROJECTServerAuthConnection * conn);
	void processCommonServerMessage(UINT32 msgId, const CommMsgBody& body, CommServerConnection* conn_, PROJECTConnectionType type);

};

class PROJECTServerObjectFactory : public CommServerObjectFactory
{
public:
	/* new */ CommServerObject* createServerObject( _CommInterface& inter ) const override
	{
		return new PROJECTServerObject( inter );
	}
};

class _PROJECTAuthAsyncCall: public CommClientGConnection::AsyncCall
{
	PROJECTServerObject * obj;
	UINT32 connId;

	virtual void processReply( UINT32 requestId, UINT32 msgId, const CommMsgBody& body )
	{
		CommMsgParser parser( body );
		obj->receiveAuthFromDbm( parser, connId );
	}

public:
	_PROJECTAuthAsyncCall( PROJECTServerObject *  obj_, UINT32 connId_ )
	{
		obj = obj_;
		connId = connId_;
	}
};

class PROJECTAsyncCall : public CommClientGConnection::AsyncCall
{
	PROJECTServerObject* obj;

public:
	virtual void processReply(UINT32 requestId, UINT32 msgId_, const CommMsgBody& body) override;

public:
	PROJECTAsyncCall(PROJECTServerObject* obj_)
		: obj(obj_)
	{
	}
};


#endif	