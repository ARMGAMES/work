#ifndef commproxy_h_included
#define commproxy_h_included

// PYR-38736 #oleksandrb "Moved" from arjel folder

#include "ppstring.h"
#include "commlib.h"
#include "commsrv.h"
#include "ppinifile.h"
#include "commgrdencryption.h"
#include "commgrdwebtokenint.h"

class CommProxyConnection;
class CommProxyServerGuard;

class CommProxyServerConnection : public CommServerConnection
{

public:
	void post(UINT32 msgId,CommMsgBody & body) override;
	void qualityUpdated( int quality )override;
	void isAlive(  )  override;

private:
	~CommProxyServerConnection(  );
	CommProxyServerConnection( CommProxyConnection & root_);
	CommProxyConnection * root;

	void _print(const char * n)
	{
		PTRACE5("CommProxyServerConnection::%s: id=%08X %s/%s/%s %s", n, id(), getServer(),getServerObject(),getServerConnType(), getGuard()->currentName());
	}

	bool _processConnectRequest();
	void _processError(UINT32 code, UINT16 errCode, const char *errMsg);
	void _processClientHighPriorityTransdisconnect();
	CommProxyServerGuard* _getGuard() const { return (CommProxyServerGuard*) guard; }
	void _processMessage(CommMsgBody & body);
	void _processQualityClientMonitor();
	void _setPriority(BYTE priority_)
	{
		priority = priority_;
		normalPriority = priority_;
	}

	bool _safeProcessConnectRequest();
	void _safeError(UINT32 code, UINT16 errCode, const char *errMsg);
	void _safeClientHighPriorityTransdisconnect();
	void _safeProcessMessage( CommMsgBody& body );
	void _setState(int state_)
	{
		state = state_;
	}
	void postQMonitorRequest()
	{
		_postQMonitorRequest();
	}
	void _safeQualityClientMonitor();

	friend class CommProxyServerConnectionPool;
	friend class CommProxyConnectionObject;
	friend class CommProxyConnection;
	friend class CommProxyServerGuard;
	friend class CommProxyConnectionFactory;
};

class CommProxyClientConnection: public CommClientConnection
{
public:
	void post(UINT32 msgId,CommMsgBody & body) override;
	void qualityUpdated( int quality ) override;

private:

	CommProxyConnection * root;
	void _print(const char * n)
	{
		PTRACE5("CommProxyClientConnection::%s: id=%08X %s/%s/%s", n, id(), getServer(),getServerObject(),getServerConnType());
	}

	~CommProxyClientConnection();
	CommProxyClientConnection(CommProxyConnection & root_, CommClientGuardFactory & guardFactory);


	void _processConnectRequestGranted(const char * guardName);
	void _processGuardAddResponse(CommMsgParser & parser);
	void _processGuardGranted(CommMsgParser & parser);

	void _processError(UINT32 code, UINT16 errCode, const char *errMsg, const CommMsgBody * errBody);
	void _processServerHighPriorityTransdisconnect();
	void _processMessage(CommMsgBody & body);
	void _processQualityServerMonitor();
	BYTE _getPriority() const
	{
		return priority;
	}
	void _safeProcessConnectRequestGranted(const char * guardName);
	void _safeProcessGuardAddResponse(CommMsgParser & parser);
	void _safeProcessGuardGranted(CommMsgParser & parser);
	void _safeError(UINT32 code, UINT16 errCode, const char *errMsg, const CommMsgBody * errBody);
	void _safeServerHighPriorityTransdisconnect();
	void _safeProcessMessage( CommMsgBody& body );
	void _safeQualityServerMonitor();
	void _setState(int state_)
	{
		state = state_;
	}
	void postQMonitorRequest()
	{
		_postQMonitorRequest();
	}
	void _processRequestIsAlive();
	void _safeRequestIsAlive();

	friend class CommProxyClientConnectionPool;
	friend class CommProxyConnectionObject;
	friend class CommProxyConnection;
};

class CommProxyConnectionObject;
class CommProxyConnectionFactory;

class CommProxyUserData
{
public:
	virtual ~CommProxyUserData(){}
};


//////////////////////////////////////////////////////////////////////////////////

class CommProxyServerGuard : public CommServerGuard
{
private:
	list <PString> names;
	PString cName;
	

public:
	int version;	
	CommProxyServerGuard(  );

	void add(const char * name)
	{
		names.push_back(PString (name));
	}
	void composeGuardNameList(CommMsgBody& msgBody) const;

public://overridden
	const char* currentName() const override;

	void processFirstRequest( CommMsgParser& parser ) override;
	void processAddRequest( CommMsgParser& parser ) override;
	void processOutBlock( CommMsgBody& dst, CommMsgBody& src ) override 
	{
		PASSERT3(0);
	}
	void processInBlock( CommMsgBody& dst, CommMsgParser& src ) override 
	{
		PASSERT3(0);
	}

	bool gotUserAuth( CommMsgParser* parser, UINT16 errCode, const char* errMsg, const CommMsgBody* errMsgBody ) override 
	{
		PASSERT3(0);
	}
	bool checkUserAuth( CommMsgParser* parser ) const override
	{
		PASSERT3(0);
	}
	bool checkGuard( const char * guardName ); 

};



class CommProxyServerGuardFactory : public CommServerGuardFactory
{
	public:
	/* new */ CommServerGuard* createGuard( const vector< const char* >& guardNames ) override;

};


class CommProxyClientGuardFactory : public CommClientGuardFactory
{
private:
	CommProxyServerGuard * serverGuard;
	CommProxyConnection  * rootConn;
public:
	CommProxyClientGuardFactory()
	{
		serverGuard = 0;
		rootConn = 0;
	}
	void setServerGuard( CommProxyServerGuard * serverGuard_)
	{
		serverGuard = serverGuard_;
	}
	void composeGuardNameList( CommMsgBody& msgBody ) const override;
	CommClientGuard* createGuard( const char* guardName, CommClientGuardAuth* guardAuth ) override
	{
		PASSERT3(0);
	}
	~CommProxyClientGuardFactory() {}
	friend class CommProxyConnection;
};



class CommProxyConnection
{
private:
	CommProxyClientGuardFactory	proxyClientGuardFactory;

	CommProxyUserData * userData;

	PString user;
	UINT32 rootId;
	CommProxyConnectionObject  & obj;
	CommProxyServerConnection  * serverConn;
	CommProxyClientConnection  * clientConn;
	bool authenticatedByUserIntId; // PYR-42014 default is false

	void _composeConnectionList(CommMsgBody & body);

public:
	CommMsgBody clientExtraDataBody; // PYR-45140

	CommProxyConnection(CommProxyConnectionObject & obj_ );
	~CommProxyConnection();


	const char * getServer() const
	{
		return serverConn->getServer();
	}
	const char * getServerObject() const
	{
		return serverConn->getServerObject();
	}
	const char * getServerConnType() const
	{
		return serverConn->getServerConnType();
	}
	const char * getUser() const
	{
		return user.c_str();
	}
	bool getClientIP(PString &ipAddr, int &port)
	{
		if( serverConn->connPathLen() )
		{
			const char* str_IP = serverConn->connPathPoint( 0 );
			if( CommSsl0ProtocolHandler::handles( str_IP ) )
			{
				CommSsl0ProtocolHandler::parseSocketAddr( ipAddr, port, str_IP );
				return true;
			}
			if( CommTcp0ProtocolHandler::handles( str_IP ) )
			{
				CommTcp0ProtocolHandler::parseSocketAddr( ipAddr, port, str_IP );
				return true;
			}
		}
		ipAddr = "";
		port = 0;
		return false;
	}
	UINT32 id() const 
	{
		return rootId;
	}
	void setUserData(CommProxyUserData * data)
	{
		userData = data;
	}
	CommProxyUserData * getUserData() const
	{
		return userData;
	}
	bool isAuthenticatedByUserIntId() const { return authenticatedByUserIntId; }
	void setUser(const char * user_) { user = user_; }

	void postToClient(UINT32 msgId, CommMsgBody & body);
	void postToServer(UINT32 msgId, CommMsgBody & body);

	friend class CommProxyConnectionFactory;
	friend class CommProxyClientConnection;
	friend class CommProxyServerConnection;
	friend class CommProxyConnectionObject;
	friend class CommProxyServerGuard;
	friend class CommProxyClientConnectionPool;
};


class CommProxyConnectionFactory : public CommServerConnectionFactory
{
public:
	CommProxyConnectionObject* obj;

public:
	CommProxyConnectionFactory( CommServerGuardFactory & guard );

	public:
	/* virtual */ /* new */ CommProxyServerConnection* createConnection() override;
};



class CommProxyClientConnectionPool : public CommClientConnectionPool
//thread-unaware
{

	public:
	CommProxyClientConnectionPool( _CommInterface& inter_ ):
		  CommClientConnectionPool(inter_)
	{}

	~CommProxyClientConnectionPool(){}

	public:
	bool _processMessage( _CommMsg& msg );


	private:

	void _post( _CommMsg& msg )
	{
		inter->_post( msg );
	}
	void _connect( CommClientConnection& conn ) override;
	void _clientSideAuthFailed( CommProxyClientConnection& conn );
	void _postGuardRequest(CommProxyClientConnection& conn, CommMsgBody & body  );
	void _postGuardAddRequest(CommProxyClientConnection& conn, CommMsgBody & body );
	void _processGuardGranted(CommProxyClientConnection& conn);
	void _postError( CommProxyClientConnection& conn, UINT32 code, UINT16 errCode, const char* errMsg );
	void _postClientDisconnect( CommProxyClientConnection& conn);
	void _postHighPriorityTransDisconnect( CommProxyClientConnection& conn);
	void _removeConnection(CommClientConnection& conn);
	void _postSrvQualityUpdate( CommProxyClientConnection & conn, int quality );
	void _postSrvIsAlive( CommProxyClientConnection & conn);
	
	friend class CommProxyConnectionObject;
	friend class CommProxyClientConnection;
};



class CommProxyServerConnectionPool : public CommServerConnectionPool
//thread-unaware
	{
	private:


	private:
	CommProxyConnectionFactory * defaultFactory;

	public:

	public:
	CommProxyServerConnectionPool( _CommInterface& inter_ ):
		  CommServerConnectionPool(inter_)
	{}
	void setDefaultFactory( CommProxyConnectionFactory& factory )
	{
		defaultFactory = &factory;
	}

	~CommProxyServerConnectionPool(){}

	public:
	bool _processMessage( _CommMsg& msg );

	private:
	void _post( _CommMsg& msg )
	{
		inter->_post( msg );
	}
	void _postRequestGranted(CommProxyServerConnection* conn );
	void _postGuardError( CommProxyServerConnection* conn, UINT16 errCode, const char* errMsg, const CommMsgBody* errMsgBody );
	void _postGuardAddResponse( CommProxyServerConnection* conn, const CommMsgBody& body );
	void _postGuardRequestGranted( CommProxyServerConnection* conn, const CommMsgBody& body, const CommMsgBody* infoMsgBody );
	void _postNoAcceptableGuard(CommProxyServerConnection* conn );
	void _postError( CommProxyServerConnection* conn, UINT32 code, UINT16 errCode, const char* errMsg, const CommMsgBody * body );
	void _postServerDisconnect( CommProxyServerConnection* conn, UINT32 code, UINT16 errCode, const char* errMsg);
	void _postHighPriorityTransDisconnect( CommProxyServerConnection * conn);
	void _postCliQualityUpdate( CommProxyServerConnection * conn, int quality );
	void _delConn(CommProxyServerConnection* conn )
	{
		CommServerConnectionPool::_delConn(conn);
	}
	friend class CommProxyConnectionObject;
	friend class CommProxyServerConnection;
	};



class CommProxyConnectionObject : public CommServerObject
{
private:
	CommProxyServerGuardFactory		   proxyServerGuardFactory;
	CommProxyConnectionFactory         connProxyFactory;

	typedef CommIdMap<CommProxyConnection *> ConnMap;
	ConnMap connMap;
	
	friend class CommProxyServerConnection;
	friend class CommProxyClientConnection;
	friend class CommProxyConnection;
	friend class CommProxyServerGuard;

public:

	CommProxyServerConnectionPool      srvPool;
	CommProxyClientConnectionPool      cliPool;

	CommProxyConnectionObject( _CommInterface& inter );
	~CommProxyConnectionObject() {};
	void remove(UINT32 rootId);

	/* virtual */ void processMessage( _CommMsg& msg ) override
	{
		srvPool._processMessage( msg );
		cliPool._processMessage( msg );
	}
	CommProxyConnection * findConnection(UINT32 id)
	{
		CommProxyConnection ** connPtr = connMap.find(id);
		if (connPtr)
			return *connPtr;
		else
			return 0;
	}
	virtual void processClientMessage(UINT32 msgId, const CommMsgBody & body, CommProxyConnection* conn  ) = 0;
	virtual void processServerMessage(UINT32 msgId, const CommMsgBody & body, CommProxyConnection* conn  ) = 0;

	virtual void processClientConnected( CommProxyConnection* conn, CommMsgParser& parser );
	virtual void processClientDisconnected( CommProxyConnection* conn ,INT16 errCode, const char * errMsg) ;

private:
	bool _serverProcessConnectRequest(CommProxyConnection * proxyConn);
	void _serverProcessError(CommProxyConnection * proxyConn, UINT32 code, UINT16 errCode, const char *errMsg);
	void _serverProcessServerHighPriorityTransdisconnect(CommProxyConnection * proxyConn);

	void _clientProcessConnectRequestGranted(CommProxyConnection * proxyConn, const char * guardName);
	void _clientProcessGuardAddResponse(CommProxyConnection * proxyConn, CommMsgParser & paser);
	void _clientProcessGuardGranted(CommProxyConnection * proxyConn, CommMsgParser & paser);
	void _clientPostGuardRequest(CommProxyConnection * proxyConn, CommMsgBody & body);
	void _clientPostGuardAddRequest(CommProxyConnection * proxyConn, CommMsgBody & body);

	void _clientProcessError(CommProxyConnection * proxyConn, UINT32 code, UINT16 errCode, const char *errMsg, const CommMsgBody * errBody);
	void _clientProcessServerHighPriorityTransdisconnect(CommProxyConnection * proxyConn);


	void _processClientMessage(CommProxyConnection * proxyConn,CommMsgBody & body);
	void _processServerMessage(CommProxyConnection * proxyConn,CommMsgBody & body);
	void _processQualityServerMonitor(CommProxyConnection * proxyConn);
	void _clientQualityUpdated(CommProxyConnection * proxyConn,int quality);
	void _processQualityClientMonitor(CommProxyConnection * proxyConn);
	void _serverQualityUpdated(CommProxyConnection * proxyConn,int quality);
	void _isAlive(CommProxyConnection * proxyConn);
	void _processRequestIsAlive(CommProxyConnection * proxyConn);

};

#endif
