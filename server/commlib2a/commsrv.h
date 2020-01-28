#ifndef commsrv_h_included
#define commsrv_h_included

#include "ppinclude.h"
#include "pptime.h"
#include "ppthread.h"
#include "pplogfile.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "commidmap.h"
#include "commsubscr.h"
#include "commtime.h"
#include "ppcontainer.h"
#include "commcli.h"
#include "ppiofile.h"

#define SERVER_START_VER_STR "ver"

/* abstract */ class CommServerObject
{
private:
	//int slot;
	//UINT32 channelId;
	_CommInterface* inter;
	//	_CommTimerInterface* timer;
	bool _exited;

public:
	CommServerObject( _CommInterface& inter_ );
	//	void _init( /*int slot, UINT32 channelId,*/ _CommTimerInterface& timer_ );

	virtual void init( CommMsgBody& body ) {}
	UINT32 postTimerMessage( UINT32 msgId, CommMsgBody& body, int msec );//returns handle
	bool deleteTimerMessage( UINT32 handle );//returns false if handle is not found
	int getInQueueSize() const;
	UINT64 getWaitInQueueTime(UINT64 *readTime =0, UINT64 *writeTime =0);
	const char * getIdentification() const
	{
		return inter->_getIdent();
	}

	virtual void processTimerMessage( UINT32 handle, UINT32 msgId, CommMsgBody& body ) {}
	virtual void processMessage( _CommMsg& msg ) = 0;
	/*	_CommInterface& getInterface()
	{
	return *inter;
	}*/
	bool exited() const
	{
		return _exited;
	}
	void exit();//object will be destroyed immediately AFTER processing current message

	virtual void shutdown() {}//anti-init, to initiate destruction of all object-created threads
	virtual bool terminated() { return true; }//to check that shutdown is successfully completed
	virtual ~CommServerObject() {}

public://helpers for CommLocalThread etc.
	bool _safeInit( CommMsgBody& body );//returns true if object should be destroyed
	bool _safeProcessMessage( _CommMsg& msg );//returns true if object should be destroyed
	bool _safeProcessTimerMessage( UINT32 handle, UINT32 msgId, CommMsgBody& body );//returns true if object should be destroyed
	void _safeShutdown();
	int _safeTerminated();//returns: 0, 1 - as terminated() returned, -1 - exception

	//public://semi-public - for CommLib internal usage
	//UINT32 _getChannelId() const { return channelId; }

private://prohibited
	CommServerObject( const CommServerObject& );
	CommServerObject& operator =( const CommServerObject& );
};

/* abstract */ class CommServerObjectFactory
{
public:
	virtual /* new */ CommServerObject* createServerObject( _CommInterface& inter ) const = 0;
	virtual  ~CommServerObjectFactory() {}
};

//****************************************************************************

class CommServerConnection;
/* abstract */ class CommServerGuard
{
protected:
	CommServerConnection* conn;
	const char* _name;

public:
	CommServerGuard( const char* name_ );//name_ MUST be constant pointer
	void _setConnection( CommServerConnection* conn_ );
	virtual ~CommServerGuard() {}

public:
	const char* name() const
	{
		return _name;
	}
	virtual const char* currentName() const//override for 'impersonating' guards
	{
		return name();
	}

public://overridable
	virtual void processFirstRequest( CommMsgParser& parser ) = 0;
	virtual void processAddRequest( CommMsgParser& parser ) = 0;

	virtual void processOutBlock( CommMsgBody& dst, CommMsgBody& src ) = 0;
	virtual void processInBlock( CommMsgBody& dst, CommMsgParser& src ) = 0;//can throw PError


	virtual bool gotUserAuth( CommMsgParser* parser, UINT16 errCode, const char* errMsg, const CommMsgBody* errMsgBody )//should be called only in response to conn->getUserAuth()
	{
		PASSERT3( 0 );
		return false;
	}
	virtual void delegateUserAuth(  UINT16 errCode, const char* errMsg, const CommMsgBody* errMsgBody )//should be called only in response to conn->getUserAuth()
	{
		PASSERT3( 0 );
	}

	virtual bool gotGuardUserInfo( CommMsgParser* parser, UINT16 errCode, const char* errMsg, const CommMsgBody* errMsgBody )//should be called only in response to conn->getUserInfo()
	{
		return true;
	}

	virtual bool checkUserAuth( CommMsgParser* parser ) const
	{
		PASSERT3( 0 );
		return false;
	}

	// any usage of this function requires explicit approval from LingZ
	virtual const char* getPlainTextPassword_useWithExtremeCaution() const
	{
		return nullptr;
	}

public:
	virtual bool isSimple() const
	{
		return true;
	}
	virtual void encryptMsg(const CommMsgBody& src, PBlock& dst) const
	{
		memcpy( dst.append(src._size()),src._readPtr(),src._size());
	}
	virtual void decryptMsg(const BYTE * block, size_t len, CommMsgBody& dst) const
	{
		memcpy( dst._append(len),block,len);		
	}
	virtual bool isEncryptionInit() const 
	{
		return false;
	}

protected:
	void _postGuardError( UINT16 errCode, const char* errMsg, const CommMsgBody* errMsgBody );
	void _postGuardAddResponse( const CommMsgBody& body );
	void _postGuardRequestGranted( const CommMsgBody& body, const CommMsgBody* infoMsgBody  );
};

/* abstract */ class CommServerGuardFactory
{
public:
	virtual /* new */ CommServerGuard* createGuard( const vector< const char* >& guardNames ) = 0;
	virtual ~CommServerGuardFactory() {}
};

/* abstract */ class CommServerConnectionFactory
{
private:
	vector<CommServerGuardFactory*> guardFactories;
public:
	BYTE authPriority;
	BYTE normalPriority;
	bool clientPriorityAllowed;

public:
	CommServerConnectionFactory( CommServerGuardFactory& guardFactory_ );
	virtual ~CommServerConnectionFactory() {}
	virtual void addGuardFactory(CommServerGuardFactory& guardFactory_);
	CommServerGuard * createGuard(const vector< const char* >& guardNames );
public:
	virtual /* new */ CommServerConnection* createConnection() = 0;
};


class CommServerConnectionPool;
class CommServerConnection
{
private:
	enum {versionBase = 0, versionExtended=1, versionProxy=2};
	friend class CommServerGuard;
	CommServerConnectionPool* pool;

private:
	friend class CommServerConnectionPool;
	friend class CommServerConnectionEx;
	friend class CommRedundantServerConnection;
	friend class CommProxyServerConnectionPool;
	friend class CommProxyServerConnection;
	CommServerGuard* guard;
	PString server;
	PString serverObject;

	PString factoryType;//aka serverConnType
	int state;//_StateConn*
	BYTE priority;
	UINT32 thisId;
	UINT32 peerId;
	UINT32 sessionId;
	bool qMonitor;
	bool inPreConnectedMap;
	bool closePending;

	BYTE normalPriority;//priority for normal (after-auth) stream

	vector< PString > physAddresses;

//	'zombie' stuff; support for DBM locker/worker
//	int zombieStatus;//0 - 'no zombie', 1 - 'zombie enabled', 2 - 'zombie'
	UINT32 version;

public:
	CommServerConnection();
	virtual ~CommServerConnection();

	bool isConnected() const
	{
		return pool != 0;
	}
	bool isClosePending() const
	{
		return closePending;
	}
	UINT32 id() const
	{
		return thisId;
	}
	BYTE getPriority() const
	{
		return priority;
	}
	const char* getServer() const
	{
		return server;
	}
	const char* getServerObject() const
	{
		return serverObject;
	}
	const char* getServerConnType() const
	{
		return factoryType;
	}

	const CommServerGuard* getGuard() const { return guard; }

protected:
	CommServerConnectionPool* _getPool() { return pool; }

public:
	int connPathLen() const;
	//returns number of physical connections on the client-to-server path
	const char* connPathPoint( int i ) const;
	//returns i-th physical connection on the client-to-server path (#0 is the closest to the client)

	virtual void post( UINT32 msgId, CommMsgBody& body );
	void monitorQuality();//enables qualityUpdated calls
	void monitorIsAlive();//requests one isAlive() call

	virtual void processMessage( UINT32 msgId, const CommMsgBody& body ) {}
	virtual void connected() {}//called after successful authentication
	virtual void qualityUpdated( int quality ) {}//0 - 100; called only if monitorQuality() was called
	virtual void isAlive() {}//called only once after monitorIsAlive() was called

	virtual void closedOrDisconnected( int errCode, const char* errMsg ) {}
	// errCode is on of _COMM_ERROR_* messages (commqueue.h) or 0 if gracefully closed by server

public://guard-related
	virtual void getUserAuth( const char* guardType, const char* user, CommMsgBody& clientExtra, CommMsgBody& request )//is called before connected()
	{
		gotUserAuth( 0 );
	}
	virtual void getUserAuth( const char* guardType, UINT32 userIntId, CommMsgBody& clientExtra, CommMsgBody& request )//is called before connected()
	{
		gotUserAuth( 0 );
	}

	bool gotUserAuth( CommMsgParser* parser, UINT16 errCode = 0, const char* errMsg = 0, const CommMsgBody* errMsgBody = 0 )
		// if not found, parser should be = 0, which means 'not found'; if failed for this or other reason, errMsg is used as an error string
		// IMPORTANT: when gotUserAuth returns false, corresponding connection is already deleted
	{
		PASSERT3( guard );
		return guard->gotUserAuth( parser, errCode, errMsg, errMsgBody );
	}
	void delegateUserAuth( UINT16 errCode = 0, const char* errMsg = 0, const CommMsgBody* errMsgBody = 0 )
		// if not found, parser should be = 0, which means 'not found'; if failed for this or other reason, errMsg is used as an error string
		// IMPORTANT: when gotUserAuth returns false, corresponding connection is already deleted
	{
		PASSERT3( guard );
		guard->delegateUserAuth( errCode, errMsg, errMsgBody );
	}

	bool checkUserAuth( CommMsgParser* parser ) const
	{
		PASSERT3( guard );
		return guard->checkUserAuth( parser );
	}
	virtual void getGuardUserInfo( const char* guardType, const char* user, CommMsgBody& clientExtra, CommMsgBody& request )//is called before connected()
	{
		gotGuardUserInfo( 0 );
	}
	bool gotGuardUserInfo( CommMsgParser* parser, UINT16 errCode = 0, const char* errMsg = 0, const CommMsgBody* errMsgBody = 0 )
		// if not found, parser should be = 0, which means 'not found'; if failed for this or other reason, errMsg is used as an error string
		// IMPORTANT: when gotUserInfo returns false, corresponding connection is already deleted
	{
		PASSERT3( guard );
		return guard->gotGuardUserInfo( parser, errCode, errMsg, errMsgBody );
	}

//public:	//'zombie' stuff; support for DBM locker/worker
//	void _enableZombie( bool enable )
//	{
//		PASSERT( zombieStatus == 0 || zombieStatus == 1 );
//		zombieStatus = enable ? 1 : 0;
//	}
//	bool _isZombie() const//zombie was enabled and then should be deleted, but instead was turned to 'real' zombie
//	{
//		return zombieStatus == 2;
//	}

private:
	void _postQMonitorRequest();

private://safety wrappers
	void _safeProcessMessage( UINT32 msgId, const CommMsgBody& body );
	void _safeConnected();
	void _safeQualityUpdated( int quality );
	void _safeIsAlive();
	void _safeClosed( int errCode, const char* errMsg );

private: //prohibited
    CommServerConnection(const CommServerConnection&) = delete;
    CommServerConnection& operator=(const CommServerConnection&) = delete;
};

class CommServerConnectionEx : public CommServerConnection
{
private:
	friend class CommServerConnectionPool;

	UINT32 currentNumber;
	UINT32 receivedNumber;
	UINT32 clientThisId;
	UINT32 confirmedNumber;
	INT32 disconnectTimeOut;
	INT32 pollTime;
	UINT32 disconnectStarted;
	bool inTransDisconnectState;
	UINT32 timerHandle;
	struct _Item
	{
		UINT32 currentNumber;
		CommMsgBody body;
	};
	typedef list< /* new */ _Item* > _List;
	_List msgList;

public:
	CommServerConnectionEx();
	virtual ~CommServerConnectionEx();
	virtual void post( UINT32 msgId, /*non-const!*/ CommMsgBody& body );
	bool setPollTime(INT32 pollTime_);
private:
	void _clearAll();
	bool _processUserMessage(CommMsgParser &parser);
	void _processErrors(UINT16 errCode,const char * errMsg);
	void _postWaitingRequests();
	bool processTimer( );
};


class CommServerConnectionPool
	//thread-unaware
{
private:
	typedef PStringMap< CommServerConnectionFactory* > _FactoryMap;

private:
	bool connected;
	_CommInterface* inter;
	_FactoryMap factoryMap;


	CommIdMap< CommServerConnection* > connMap;
	typedef map<UINT32, CommServerConnection *> _PreConnMap;

	_PreConnMap preConns;

	PIOFile _recordReplayFile;
	bool _initReplay;
	UINT32 _lastReplayRecord;

public:
	enum eTimeType
	{
		normalTime,
		recordingType,
		replayingType
	};

private:
	eTimeType  _typeOfMessageTime; 
	time_t _currentMessageTime;
	struct tm _currentLocalTime;
	INT16  _currentBias;

public:

	void setTypeOfTime(eTimeType type);
	time_t messageTime() const;
	void   messageSrvLocalTime(CommSrvTime & t) const;
	void _setMessageTime(time_t sec)
	{
		_currentMessageTime = sec;
	}
	void _setMessageSrvLocalTime(const CommSrvTime & t)
	{
		t.mk_tm(_currentLocalTime);
	}


	class ConnectionIterator
	{
	private:
		friend class CommServerConnectionPool;
		int i;
	};

public:
	CommServerConnectionPool( _CommInterface& inter_ );
	void registerFactory( const char* name, CommServerConnectionFactory& factory );
	void unregisterFactory( const char* name );
	virtual ~CommServerConnectionPool();
	void deleteAllConnections();

	CommServerConnection* findConnection( UINT32 connId );
	const CommServerConnection* findConnection( UINT32 connId ) const;
	CommServerConnection* firstConnection( ConnectionIterator& i );//returns 0 if not found
	CommServerConnection* nextConnection( ConnectionIterator& i );//returns 0 if not found
	size_t numberOfConnections() const
	{
		return connMap.size();
	}


	void initRecordReplay(const char * name);
	void closeRecordReplay();

	void disconnect( CommServerConnection* conn, UINT16 errCode = 0, const char * errMessage = 0);

public:
	bool _processMessage( _CommMsg& msg );
	
	// do not use without approval.
	virtual bool _aboutToConnect(const CommMsgBody & body, const char * connType) 
	{ 
		return false; 
	}

private:
	friend class CommServerConnection;
	friend class CommServerConnectionEx;
	friend class CommProxyServerConnectionPool;
	void _post( _CommMsg& msg )
	{
		inter->_post( msg );
	}

private:
	CommServerConnection* _getConn( UINT32 thisId, BYTE msgType );
	CommServerConnection* _getConn( UINT32 thisId, UINT32 peerId, BYTE msgType );
	CommServerConnection* _getConn( UINT32 thisId, UINT32 peerId, UINT32 sessionId, BYTE msgType );

	CommServerConnection* _lookForConn( UINT32 cliId );
	void _delConn( CommServerConnection* conn );

private:
	friend class CommServerGuard;
	friend class CommRedundantServerConnection;
	void _postGuardError( CommServerConnection* conn, UINT16 errCode, const char* errMsg, const CommMsgBody* errMsgBody );
	void _postGuardAddResponse( CommServerConnection* conn, const CommMsgBody& body );
	void _postGuardRequestGranted( CommServerConnection* conn, const CommMsgBody& body, const CommMsgBody* infoMsgBody  );
};

#if 0
template< class Conn >
class CommServerConnectionFactoryEx
{
public:
	virtual /* new */ Conn* createConnection();
};

template< class Conn >
class CommServerConnectionPoolEx : public CommServerConnectionPool
{
private:
	class _Factory : public CommServerConnectionFactory
		//introduced to prevent overriding of CommServerConnectionFactory::createConnection() 
		//by classes derived from CommServerConnectionFactoryEx< Conn > only
	{
		CommServerConnectionFactoryEx< Conn >* factoryEx;
		/* new */ CommServerConnection* createConnection() override
		{
			return factoryEx->createConnection();
		}
	};
	PNewPtrVector< _Factory > _factories;//to store instances of _Factory

public:
	CommServerConnectionPoolEx( _CommInterface& inter_ )
		: CommServerConnectionPool( inter_ )
	{
	}
	void registerFactory( const char* name, CommServerConnectionFactoryEx< Conn >& factoryEx )
	{
		_Factory* f = new _Factory();
		f->factoryEx = &factoryEx;
		_factories.push_back( f );
		CommServerConnectionPool::registerFactory( name, *f );
	}

public:
	Conn* findConnection( UINT32 connId )
	{
		return (Conn*)CommServerConnectionPool::findConnection( connId );
	}
	Conn* firstConnection( ConnectionIterator& i )
	{
		return (Conn*)CommServerConnectionPool::firstConnection( i );
	}
	Conn* nextConnection( ConnectionIterator& i )
	{
		return (Conn*)CommServerConnectionPool::nextConnection( i );
	}

	void disconnect( Conn* conn )
	{
		CommServerConnectionPool::disconnect( conn );
	}
};
#endif

//****************************************************************************

class CommServerPublisherPool;

class CommServerPublisher : public CommSubscription
{
public:
	struct UpdateMsg
	{
	private:
		friend class CommServerPublisher;
		friend class CommServerPublisherPool;
		bool active;
		bool empty;
		bool newGen;
		CommMsgBody msg;

	public:
		bool shouldBePosted() const { return active && !empty; }
		const CommMsgBody& getUpdateMsgBody() const { return msg; }
	};

	BYTE priority;

private:
	friend class CommServerPublisherPool;
	struct _Subscr
	{
		CommServerPublisher* publisher;
		UINT32 thisId;
		UINT32 peerId;
	};

private:
	CommServerPublisherPool* pool;
	UINT32 ida, idb, idc, idd;
	UINT32 rev;
	typedef set< _Subscr* > _SubscrSet;
	_SubscrSet children;

	PString name;
	UINT32 numberOfSubscribers;
	bool   countingOn;

public:
	CommServerPublisher();
	bool isPublished() const
	{
		return pool != 0;
	}
	const char* publishedAs() const
	{
		return pool ? name.c_str() : 0;
	}
	const UINT32 getNumberOfSubscribers() const
	{
		if (countingOn)
			return numberOfSubscribers;
		else
			return 0;
	}

	void startUpdateMsg( UpdateMsg& updateMsg ) const;

	int appendNode( const int* path, int pathLen, UpdateMsg* updateMsg /* can be 0 */);
	//void insertNode( const int* path, int pathLen, int pos, UpdateMsg* updateMsg /* can be 0 */);
	int appendLeaf( const int* path, int pathLen, /*non-const!*/ CommMsgBody& msg, UpdateMsg* updateMsg /* can be 0 */);
	//void insertLeaf( const int* path, int pathLen, int pos, /*non-const!*/ CommMsgBody& msg, UpdateMsg* updateMsg /* can be 0 */);
	void updateLeaf( const int* path, int pathLen, /*non-const!*/ CommMsgBody& msg, UpdateMsg* updateMsg /* can be 0 */);
	void removeLeaf( const int* path, int pathLen, UpdateMsg* updateMsg /* can be 0 */);
	void removeNode( const int* path, int pathLen, UpdateMsg* updateMsg /* can be 0 */);

	static void attachMsg( UINT32 attachId, const CommMsgBody& attachment, UpdateMsg* updateMsg /* can be 0 */);

	void finishUpdateMsg( UpdateMsg& updateMsg ) const;
	void postUpdate( UpdateMsg& update );
	void startCountNumberOfSubscribers();
	void stopCountNumberOfSubscribers();



private:
	/* inline */ UINT32 _nextRev( UpdateMsg* updateMsg );
	/* inline */ _Node* _findNode( const int* path, int pathLen, UINT32 id );
	/* inline */ _Node* _findNodeNoChanges( const int* path, int pathLen );
	void _nextGeneration();
	void _startCountNumberOfSubscribers();
};

class CommServerPublisherPool
{
private:
	typedef PStringMap< CommServerPublisher* > _PublisherMap;
	_PublisherMap publisherMap;
	bool connected;
	_CommInterface* inter;
	CommIdMap< CommServerPublisher::_Subscr* > subscrMap;

public:
	CommServerPublisherPool( _CommInterface& inter_ );
	void publish( const char* name, CommServerPublisher& publisher );
	bool havePublisher( const char* name ) const
	{
		return publisherMap.find( name ) != publisherMap.end();
	}
	void unpublish( const char* name );
	void _postUpdate( const CommServerPublisher& publisher, CommServerPublisher::UpdateMsg& update );

public:
	bool _processMessage( _CommMsg& msg );
	virtual bool _aboutToSubscribe(const CommMsgBody & body, const char * channel) { return true; }
	virtual void _afterUnsubscribe(const char * channel) { return; }
	
	void processSubscrRequest(CommMsgBody & body);
private:
	friend class CommServerPublisher;

private:
	CommServerPublisher::_Subscr* _getSubscr( UINT32 thisId, BYTE msgType );
	CommServerPublisher::_Subscr* _getSubscr( UINT32 thisId, UINT32 peerId, BYTE msgType );
	CommServerPublisher::_Subscr* _lookForSubscr( UINT32 cliId );
	void _delSubscr( CommServerPublisher::_Subscr* subscr );
};

#endif
