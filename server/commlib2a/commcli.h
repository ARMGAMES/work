#ifndef commcli_h_included
#define commcli_h_included

#include "ppinclude.h"
#include "ppstring.h"
#include "pptime.h"
#include "pplogfile.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "commidmap.h"
#include "commconn.h"
#include "commsubscr.h"

class CommServerConnection;


/* abstruct */class CommClientAuthCallback
{
public:
	virtual void auth(PString &user, PString &pwd, PString  &sid, CommMsgBody & extra) = 0;
	virtual void auth(UINT32 &userIntId, PString &pwd, PString  &sid, CommMsgBody & extra) 
	{
		PASSERT3(0);
	}
};



/* abstract */ class CommClientGuard
{
private:
	const char* _name;

public:
	CommClientGuard( const char* name_ );//name_ MUST be constant pointer

public:
	const char* name() const
	{
		return _name;
	}

public:
	virtual void composeFirstRequest( CommMsgBody& msgBody ) = 0;
	virtual bool processResponse( CommMsgBody& msgBody, CommMsgParser& parser ) = 0;
	virtual bool processGranted( CommMsgParser& parser ) = 0;
	virtual void composeGuardNameList( CommMsgBody& msgBody ) const = 0;

	virtual void processOutBlock( CommMsgBody& dst, CommMsgBody& src ) = 0;
	virtual void processInBlock( CommMsgBody& dst, CommMsgParser& src ) = 0;//can throw PError
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

public:

	virtual bool isSimple() const
	{
		return true;
	}

	virtual ~CommClientGuard() {}
};

class CommClientGuardAuth
{
public:
	virtual ~CommClientGuardAuth() {}

};

/* abstract */ class CommClientGuardFactory
{
public:
	CommClientGuardFactory() {}
	virtual /* new */ CommClientGuardAuth* auth() { return 0; } 
	virtual void composeGuardNameList( CommMsgBody& msgBody ) const = 0;//("s")* format, no terminating empty string
	virtual /* new */ CommClientGuard* createGuard( const char* guardName, CommClientGuardAuth* guardAuth ) = 0;
	virtual ~CommClientGuardFactory() {}

private://prohibited
	CommClientGuardFactory( const CommClientGuardFactory& other );
	CommClientGuardFactory& operator =( const CommClientGuardFactory& other );
};

class CommClientConnectionPool;
class CommClientConnection
{
private:
	enum {versionBase = 0, versionExtended=1, versionProxy=2};
	CommClientConnectionPool* pool;

private:
	friend class CommClientConnectionPool;
	friend class CommRedundantClientConnection;
	friend class CommClientConnectionEx;
	friend class CommProxyClientConnectionPool;
	friend class CommProxyClientConnection;
	CommClientGuardFactory* guardFactory;
	/* new */ CommClientGuard* guard;
	/* new */ CommClientGuardAuth* guardAuth;

	PString server;
	PString serverObject;
	PString serverConnType;
	bool qMonitor;
	int state;//_StateConn*
	BYTE priority;
	UINT32 thisId;
	UINT32 peerId;
	UINT32 sessionId;
	bool closePending;
	UINT32 version;
	bool _qualityRequested;
	BYTE _quality;

public:
	CommClientConnection( CommClientGuardFactory& guardFactory_ );
	virtual ~CommClientConnection();

	bool isMyGuardFactory( const CommClientGuardFactory& guardFactory_ ) const
	{
		return guardFactory == &guardFactory_;
	}
	bool isConnected() const
	{
		return pool != 0;
	}
	bool isClosePending() const
	{
		return closePending;
	}
	void setPriority(BYTE priority_)
	{
		PASSERT3(priority_< _COMM_MSGPRIORITIES);
		priority = priority_;
	}

	UINT32 id() const
	{
		return thisId;
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
		return serverConnType;
	}
	const CommClientGuard* getGuard() const { return guard; }

public:
	virtual void post( UINT32 msgId, /*non-const!*/ CommMsgBody& body );
	void monitorQuality();

	virtual void processMessage( UINT32 msgId, const CommMsgBody& body ) {}
	virtual void connected() {}
	virtual void connectedWithInfo(CommMsgBody *infoBody) 
	{
		return connected();
	}
	virtual void qualityUpdated( int quality ) {}//0 - 100
	virtual bool closedOrDisconnected( int errCode, const char* errMsg );
	virtual bool closedOrDisconnected( int errCode, const char* errMsg, CommMsgBody* errMsgBody )
	{
		return closedOrDisconnected(errCode, errMsg);
	}
	// errCode is on of _COMM_ERROR_* messages (commqueue.h) or 0 if gracefully closed by server
	// should return false to re-attempt establish connection, true otherwise
	// by default returns false only to _COMM_ERROR_NOLOCALOBJECT
	virtual void processQualityRequest();
	void postQualityUpdate(BYTE quality_);
	virtual void processKeepAliveRequest();
	void postKeepAlive();
private:
	/* inline */ bool _createGuard( const char* guardName );
	void _postQMonitorRequest();
	void _postQualityUpdate(BYTE quality_);
	void _postKeepAlive();

private://safety wrappers
	void _safeProcessMessage( UINT32 msgId, const CommMsgBody& body );
	void _safeConnected(CommMsgBody * infoBody);
	void _safeQualityUpdated( int quality );
	bool _safeClosed( int errCode, const char* errMsg, CommMsgBody* errMsgBody = 0 );
	void _safeProcessQualityRequest();
	void _safeProcessKeepAliveRequest();
	virtual void _clearAll(){}

private://prohibited
	CommClientConnection( const CommClientConnection& other );
	CommClientConnection& operator =( const CommClientConnection& other );
};

class CommClientConnectionEx : public CommClientConnection
{
private:
	friend class CommClientConnectionPool;

	UINT32 currentNumber;
	UINT32 receivedNumber;
	UINT32 confirmedNumber;
	UINT32 serverThisId;
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
	CommClientConnectionEx( CommClientGuardFactory& guardFactory_ );
	virtual ~CommClientConnectionEx();
	virtual void post( UINT32 msgId, /*non-const!*/ CommMsgBody& body );
	bool setPollTime(INT32 pollTime_);
private:
	void _processUserMessage(CommMsgParser &parser);
	void _processErrors(UINT16 errCode,const char * errMsg);
	void _postWaitingRequests(UINT32 receivedFromClient);
	bool processTimer();
	void _clearAll();
};

class CommClientGConnection 
	// so-called "Guaranteed" connection
	// !! inserts UINT32 request_id to the beginning of each message on send
	// !! assumes that server replies to each message and each reply begins with request_id
	// !! removes UINT32 request_id from the beginning of each message on receive
	// keeps track of messages sent and replies received
	// performs automatic resend on reconnect
	// !! can produce duplicate messages on disconnects
{
	struct _Item; // Forward for access
public:
	/* abstract */ class AsyncCall
	{
	private:
		friend struct CommClientGConnection::_Item;
		void addRef() 
		{
			++refCounter;
		}
		void release()
		{
			if( --refCounter==0 )
				delete this;
		}
		int refCounter;

	protected:
		// make the destructor protected, so that only release() can delete
		virtual ~AsyncCall() 
		{
			if( refCounter != 0 )
				PLog("ERROR: deleting an object with reference counter %d", refCounter);
		}

	public:
		AsyncCall()
			: refCounter(0)	
		{}
		INT32 getRefCount() const // PYR-80802
		{
			return refCounter;
		}

		virtual void processReply( UINT32 requestId, UINT32 msgId, const CommMsgBody& body ) = 0;
	};
public:
	class _ConnHandler : public CommClientConnection
	{
	private:
		CommClientGConnection* gConn;

	public:
		_ConnHandler( CommClientGuardFactory& guardFactory_ );
		void _init( CommClientGConnection* gConn_ );
		CommClientGConnection* getGconn() { return gConn; } 

	public://overridden
		void processMessage( UINT32 msgId, const CommMsgBody& body ) override;
		void connected() override;
		void qualityUpdated( int quality ) override;
		bool closedOrDisconnected( int errCode, const char* errMsg, CommMsgBody* errMsgBody ) override;
	};
private:
	struct _Item
	{
		/* inline */ _Item( UINT32 _msgId, CommMsgBody& _body, AsyncCall* _call );
		/* inline */ ~_Item();
		UINT32 msgId;
		CommMsgBody patchedBody;
		/* new */ AsyncCall* call;
	};
	typedef list< /* new */ _Item* > _List;
	typedef CommIdMap< _List::iterator > _Map;

public:
	typedef list< _Item* >::const_iterator const_iterator;

private:
	_ConnHandler conn;
	_List msgList;
	_Map msgMap;

public:
	CommClientGConnection( CommClientGuardFactory& guardFactory_);
	CommClientConnection& underlyingConnection()//use with caution
	{
		return conn;
	}
	const CommClientConnection& underlyingConnection() const//use with caution
	{
		return conn;
	}
	virtual UINT32 post( UINT32 msgId, /*non-const!*/ CommMsgBody& body, /* new */ AsyncCall* call = 0 );//returns requestId
	void cancelPost( UINT32 requestId );
	virtual ~CommClientGConnection();

public://access to outgoing queue
	size_t _size() const { return msgMap.size(); }
	const_iterator _begin() const { return msgList.begin(); }
	const_iterator _end() const { return msgList.end(); }
	static UINT32 _getRequestId( const_iterator it );
	static UINT32 _getMsgId( const_iterator it ) { return (*it)->msgId; }
	static void _getMsgBody( const_iterator it, CommMsgBody& ret );
	static AsyncCall* _getCall( const_iterator it ) { return (*it)->call; }
	const_iterator _getIterator(UINT32 reqId) const 
	{ 
		if (reqId ) 
		{
			const _List::iterator * found = msgMap.find( reqId );
			if (found)
				return *found;
			else 
				return msgList.end();
		}
		else
		{
			return msgList.end();
		}
	}

	void _clearAll();

public://overridables - the same as CommClientConnection overridables
	virtual void processGMessage( UINT32 requestId, UINT32 msgId, const CommMsgBody& body, AsyncCall* call ) {}
	virtual void connected() {}
	virtual void qualityUpdated( int quality ) {}
	virtual bool closedOrDisconnected( int errCode, const char* errMsg ) 
	{
		return conn.CommClientConnection::closedOrDisconnected( errCode, errMsg ); 
	}
	virtual bool closedOrDisconnected( int errCode, const char* errMsg, CommMsgBody* errMsgBody )
	{
		return closedOrDisconnected(errCode, errMsg);
	}
	// PYR-76565, if override below returns false then the msg will NOT be re-sent on G-re-connection of the client
	virtual bool beforeGConnectedPost(UINT32 msgId, /*non-const */ CommMsgBody& body) { return true; }


private:
	friend class _ConnHandler;
	void _gConnected();
	void _gProcessMessage( UINT32 msgId, const CommMsgBody& body );

private://safety wrappers
	void _safeProcessGMessage( UINT32 requestId, UINT32 msgId, const CommMsgBody& body, AsyncCall* call );
	void _safeConnected();
	void _safeQualityUpdated( int quality );
	bool _safeClosed( int errCode, const char* errMsg, CommMsgBody* errMsgBody );
	//	void _safeProcessReply( AsyncCall* call, UINT32 requestId, UINT32 msgId, const CommMsgBody& body );
};

class CommClientGConnectionEx 
	// so-called "Guaranteed" connection
	// !! inserts UINT32 request_id to the beginning of each message on send
	// !! assumes that server replies to each message and each reply begins with request_id
	// !! removes UINT32 request_id from the beginning of each message on receive
	// keeps track of messages sent and replies received
	// performs automatic resend on reconnect
	// !! can produce duplicate messages on disconnects
{
	struct _Item; // Forward for access
public:
	/* abstract */ class AsyncCall
	{
	private:
		friend struct CommClientGConnectionEx::_Item;
		void addRef() 
		{
			++refCounter;
		}
		void release()
		{
			if( --refCounter==0 )
				delete this;
		}
		int refCounter;

	protected:
		// make the destructor protected, so that only release() can delete
		virtual ~AsyncCall() 
		{
			if( refCounter != 0 )
				PLog("ERROR: deleting an object with reference counter %d", refCounter);
		}

	public:
		AsyncCall()
			: refCounter(0)	
		{}
		INT32 getRefCount() const // PYR-80802
		{
			return refCounter;
		}

		virtual void processReply( UINT32 requestId, UINT32 msgId, const CommMsgBody& body ) = 0;
	};


private:
	class _ConnHandler : public CommClientConnectionEx
	{
	private:
		CommClientGConnectionEx* gConn;

	public:
		_ConnHandler( CommClientGuardFactory& guardFactory_ );
		void _init( CommClientGConnectionEx* gConn_ );

	public://overridden
		void processMessage( UINT32 msgId, const CommMsgBody& body ) override;
		void connected() override;
		void qualityUpdated( int quality ) override;
		bool closedOrDisconnected( int errCode, const char* errMsg, CommMsgBody* errMsgBody ) override;
	};
	struct _Item
	{
		/* inline */ _Item( UINT32 _msgId, CommMsgBody& _body, AsyncCall* _call );
		/* inline */ ~_Item();
		UINT32 msgId;
		CommMsgBody patchedBody;
		/* new */ AsyncCall* call;
	};
	typedef list< /* new */ _Item* > _List;
	typedef CommIdMap< _List::iterator > _Map;

public:
	typedef list< _Item* >::const_iterator const_iterator;

private:
	_ConnHandler conn;
	_List msgList;
	_Map msgMap;

public:
	CommClientGConnectionEx( CommClientGuardFactory& guardFactory_);
	CommClientConnectionEx& underlyingConnection()//use with caution
	{
		return conn;
	}
	const CommClientConnectionEx& underlyingConnection() const
	{
		return conn;
	}
	virtual UINT32 post( UINT32 msgId, /*non-const!*/ CommMsgBody& body, /* new */ AsyncCall* call = 0 );//returns requestId
	void cancelPost( UINT32 requestId );
	virtual ~CommClientGConnectionEx();
	bool setPollTime(INT32 pollTime_) { return conn.setPollTime(pollTime_); }

public://access to outgoing queue
	size_t _size() const { return msgMap.size(); }
	const_iterator _begin() const { return msgList.begin(); }
	const_iterator _end() const { return msgList.end(); }
	static UINT32 _getRequestId( const_iterator it );
	static UINT32 _getMsgId( const_iterator it ) { return (*it)->msgId; }
	static void _getMsgBody( const_iterator it, CommMsgBody& ret );
	static AsyncCall* _getCall( const_iterator it ) { return (*it)->call; }
	const_iterator _getIterator(UINT32 reqId) const
	{ 
		if (reqId ) 
		{
			const _List::iterator * found = msgMap.find( reqId );
			if (found)
				return *found;
			else 
				return msgList.end();
		}
		else
		{
			return msgList.end();
		}
	}

	void _clearAll();

public://overridables - the same as CommClientConnection overridables
	virtual void processGMessage( UINT32 requestId, UINT32 msgId, const CommMsgBody& body, AsyncCall* call ) {}
	virtual void connected() {}
	virtual void qualityUpdated( int quality ) {}
	virtual bool closedOrDisconnected( int errCode, const char* errMsg ) 
	{
		return conn.CommClientConnection::closedOrDisconnected( errCode, errMsg ); 
	}
	virtual bool closedOrDisconnected( int errCode, const char* errMsg, CommMsgBody* errMsgBody )
	{
		return closedOrDisconnected(errCode, errMsg);
	}

private:
	friend class _ConnHandler;
	void _gConnected();
	void _gProcessMessage( UINT32 msgId, const CommMsgBody& body );

private://safety wrappers
	void _safeProcessGMessage( UINT32 requestId, UINT32 msgId, const CommMsgBody& body, AsyncCall* call );
	void _safeConnected();
	void _safeQualityUpdated( int quality );
	bool _safeClosed( int errCode, const char* errMsg, CommMsgBody* errMsgBody );
	//	void _safeProcessReply( AsyncCall* call, UINT32 requestId, UINT32 msgId, const CommMsgBody& body );
};


class CommClientConnectionPool
	//thread-unaware
{
public:
	class ConnectionIterator
	{
	private:
		friend class CommClientConnectionPool;
		int i;
	};

private:
	enum { _ReconnectTimeout = 1000 };

	bool connected;
	_CommInterface* inter;
	CommIdMap< CommClientConnection* > connMap;

public:
	CommClientConnectionPool( _CommInterface& inter_ );
	void connect( CommClientConnection& conn, const char* server, const char* serverObject, const char* serverConnType = "" );
	void disconnect( CommClientConnection& conn );
	virtual ~CommClientConnectionPool();

public:
	CommClientConnection* firstConnection( ConnectionIterator& i );//returns 0 if not found
	CommClientConnection* nextConnection( ConnectionIterator& i );//returns 0 if not found
	CommClientConnection* findConnection( UINT32 connId );

public:
	bool _processMessage( _CommMsg& msg );

	class CommEmbeddedMessageOnTheFlyCallback
	{
	public:
		virtual void callback( const _CommMsg & msg) = 0;
	};

	static void _getEmbeddedMessageOnTheFly( const _CommMsg& msg, UINT32 subscrMsgId, CommEmbeddedMessageOnTheFlyCallback & callBack);

private:
	friend class CommClientConnection;
	friend class CommClientConnectionEx;
	friend class CommRedundantClientConnection;
	friend class CommProxyClientConnectionPool;
	void _post( _CommMsg& msg )
	{
		inter->_post( msg );
	}
private:
	CommClientConnection* _getConn( UINT32 thisId, BYTE msgType );
	CommClientConnection* _getConn( UINT32 thisId, UINT32 peerId, BYTE msgType );
	CommClientConnection* _getConn( UINT32 thisId, UINT32 peerId, UINT32 sessionId, BYTE msgType );

	virtual void _connect( CommClientConnection& conn );
	void _reconnect( CommClientConnection& conn );
	void _scheduleReconnect( CommClientConnection& conn );
	void _postClientError( CommClientConnection& conn, UINT16 errCode, const char* errMsg );
	void _clientSideAuthFailed( CommClientConnection& conn );
};

//****************************************************************************

class CommClientSubscriberPool;
class CommClientSubscriber : public CommSubscription
{
private:
	CommClientSubscriberPool* pool;

private:
	friend class CommClientSubscriberPool;
	PString server;
	PString serverObject;
	PString serverChannel;
	int state;//_StateSubscr*
	BYTE priority;
	UINT32 thisId;
	UINT32 peerId;

public:
	CommClientSubscriber();
	~CommClientSubscriber();

	bool isSubscribed() const
	{
		return pool != 0;
	}
	UINT32 id() const
	{
		return pool ? thisId : 0;
	}
	const char* getServer() const
	{
		return server;
	}
	const char* getServerObject() const
	{
		return serverObject;
	}
	const char* getServerChannel() const
	{
		return serverChannel;
	}

public:
	virtual void beginTrans() {}
	virtual void commitTrans() {}

	virtual void synchronized() {}
	virtual void desynchronized() {}

	virtual bool closed( int errCode, const char* errMsg );
	// errCode is on of _COMM_ERROR_* messages (commqueue.h) or 0 if gracefully closed by server
	// should return false to re-attempt establish connection, true otherwise
	// by default returns false only to _COMM_ERROR_NOLOCALOBJECT

protected://safety wrappers
	void _safeBeginTrans();
	void _safeCommitTrans();
	void _safeSynchronized();
	void _safeDesynchronized();
	bool _safeClosed( int errCode, const char* errMsg );
};

class CommClientSubscriberPool
	//thread-unaware
{
private:
	enum { _ResubscribeTimeout = 1000 };

	bool connected;
	_CommInterface* inter;
	CommIdMap< CommClientSubscriber* > subscrMap;

public:
	CommClientSubscriberPool( _CommInterface& inter_ );
	void subscribe( CommClientSubscriber& subscr, const char* server, const char* serverObject, const char* serverChannel );
	void unsubscribe( CommClientSubscriber& subscr );
	void freeze( CommClientSubscriber& subscr );
	void defrost( CommClientSubscriber& subscr );
	void subscribeFrozen( CommClientSubscriber& subscr, const char* server, const char* serverObject, const char* serverChannel );
	~CommClientSubscriberPool();

public:
	static void parseMessageOnTheFly( CommSubscription::OnTheFlyCallback& callback, const _CommMsg& msg, UINT32 subscrId = 0 );
	bool _processMessage( _CommMsg& msg );

private:
	friend class CommClientSubscriber;
	void _post( _CommMsg& msg )
	{
		inter->_post( msg );
	}

private:
	CommClientSubscriber* _getSubscr( UINT32 thisId, BYTE msgType );
	CommClientSubscriber* _getSubscr( UINT32 thisId, UINT32 peerId, BYTE msgType );

	void _subscribe( CommClientSubscriber& subscr );
	void _resubscribe( CommClientSubscriber& subscr );
	void _scheduleResubscribe( CommClientSubscriber& subscr );
	void _postUnsubscribe( CommClientSubscriber& subscr );
	void _postDefrost( CommClientSubscriber& subscr );
	void _postFreeze( CommClientSubscriber& subscr );
};

#endif
