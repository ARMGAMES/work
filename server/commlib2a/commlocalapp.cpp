#include "ppinclude.h"
#include "ppsocket.h"
#include "ppprocess.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "commsrv.h"
#include "commcli.h"
#include "commgrdnull.h"
#include "commappfactory.h"
#include "_commtcp0.h"
#include "commstreamencryption.h"
#ifdef PWIN32
#include "ppstacktrace.h"
#endif

#define LOCALHOST "127.0.0.1"

struct _CommRunToSubFactoryInterface 
{
	_CommTimer & timer;
	_CommRunToSubFactoryInterface(_CommTimer &_timer):timer(_timer)
	{
	}
};
class _CommLocalAppInterfaceBase;
class _CommLocalAppCommunicator
{
private:
	PBaseSocket* sock;
	BYTE* rdBuf;
	size_t rdBufSz;
	size_t rdRd;

public:
	_CommLocalAppCommunicator()
	{
		sock = 0;
		rdBuf = 0;
		rdBufSz = 0;
	}
	void construct( PBaseSocket& sock_ )
	{
		sock = &sock_;
	}

	static bool hasCompression( int type ) { return false; }
	void setCompression( int type,size_t BuffSize ) {}
	void setEncryption( CommStreamEncryption * encryption_ ) {}
	void flush() {}
	bool initConnection(UINT32 & checkSumm )
	{
		checkSumm = 0;
		return true;
	}
	bool generateRandom(UINT32 * secureRandom, int size)
	{
		return false;
	}

	bool _tryRead( int msecTimeout );

public://for *Handlers
	size_t writeIncomplete( const BYTE* p, size_t sz )
	{
		sock->sendComplete( p, sz );
		return sz;
	}
	size_t readIncomplete( BYTE* p, size_t sz )
	{
		PASSERT3( rdBuf == 0 );
		size_t rd = sock->recv( p, sz, 0 );//incomplete
		if( rd == 0 )
		{
			rdBuf = p;
			rdBufSz = sz;
			rdRd = 0;
		}
		return rd;
	}
	_CommAsyncRet asyncReadOk()
	{
		_CommAsyncRet ret;
		ret.read = rdRd;
		rdBuf = 0;
		rdBufSz = 0;
		rdRd = 0;
		return ret;
	}
	_CommAsyncRet asyncWriteOk()
	{
		return _CommAsyncRet();
	}
};

typedef _CommCompleteCommunicator< _CommLocalAppCommunicator > _CommLocalAppCompleteCommunicator;

class _CommLocalAppInterfaceBase : public _CommInterface//, public _CommTimerInterface
{
protected:
	bool _connected;
	CommMsgSimpleQueue q;
	_CommLocalAppCompleteCommunicator comm;
	_CommStream0ProtocolHandler< _CommLocalAppCompleteCommunicator, _CommLocalAppInterfaceBase > handler;
	_CommTimer* timer;

	CommMsgSimpleQueue rdQueue;
	UINT64 _timeInWait;
	UINT64 _timeInRead;
	UINT64 _timeInWrite;
	bool _calculateTimeInWait;
	PString ident;


public:
	_CommLocalAppInterfaceBase(const char * ident_)
	{
		_connected = false;
		handler.construct( comm, *this,0 );
		timer = 0;
		_calculateTimeInWait = false;
		_timeInWait = 0;
		_timeInRead = 0;
		_timeInWrite = 0;
		ident = ident_;


	}
	void enableMux()
	{
		handler.enableMux();
	}
	void _setTimer( _CommTimer& timer_ )
	{
		PASSERT3( timer == 0 );
		timer = &timer_;
	}

	bool connected() const
	{
		return _connected;
	}
	virtual PBaseSocket& socket() = 0;
	virtual bool connect() = 0;

public://{_CommInterface
	void _post( _CommMsg& m ) override
	{
		m.internal.mux = 0;
		m.internal.muxChannelId = 0;
		q.push( m );
	}
	UINT32 _postClientTimer( _CommMsg& msg, int msec ) override
	{
		//		msg.internal.mux = 0;
		//		msg.internal.muxChannelId = 0;
		msg.internal.slot = 0;
		msg.internal.channelId = 0;
		PASSERT3( timer );
		return timer->addMsg( msg, msec );
	}
	UINT32 _addServerTimer( UINT32 msgId, CommMsgBody& body, int msec ) override
	{
		_CommMsg msg( _COMM_MSGTYPE_NONE, _COMM_MSGPRIORITY_INVALID );
		msg.body.moveFrom( body );
		msg.internal.channelId = 0;
		msg.internal.slot = 0;
		msg.internal.patch0 = msgId;
		return timer->addMsg( msg, msec );
	}
	bool _delServerTimer( UINT32 handle ) override
	{
		return timer->delMsg( handle );
	}
	int _getInQueueSize() const override
	{
		return rdQueue.size();
	}
	const char * _getIdent() const override
	{
		return ident.c_str();
	}

	UINT64 _getWaitInQueueTime(UINT64 *readTime , UINT64 *writeTime ) override
	{
		if (! _calculateTimeInWait)
		{
			_calculateTimeInWait = true;
			_timeInWait = 0;
			_timeInRead = 0;
			_timeInWrite = 0;

			return 0;
		}
		UINT64 tWait = _timeInWait;
		if (readTime)
			*readTime = _timeInRead;
		if (writeTime)
			*writeTime = _timeInWrite;
		_timeInWait = 0;
		_timeInRead = 0;
		_timeInWrite = 0;

		return tWait;
	}

	//}_CommInterface

	void _postFromChild( int mux, UINT32 muxChannelId, _CommMsg& m )
	{
		m.internal.mux = mux;
		m.internal.muxChannelId = muxChannelId;
		q.push( m );
	}
	UINT32 _postTimerFromChild( int mux, UINT32 muxChannelId, _CommMsg& msg, int msec )
	{
		//msg.internal.mux = mux;
		//msg.internal.muxChannelId = muxChannelId;
		msg.internal.slot = mux;//assuming that objIdx is the same as mux
		msg.internal.channelId = muxChannelId;
		PASSERT3( timer );
		return timer->addMsg( msg, msec );
	}

	UINT32 _addTimerFromChild( int mux, UINT32 muxChannelId, UINT32 msgId, CommMsgBody& body, int msec )
	{
		_CommMsg msg( _COMM_MSGTYPE_NONE, _COMM_MSGPRIORITY_INVALID );
		msg.body.moveFrom( body );
		msg.internal.channelId = muxChannelId;
		msg.internal.slot = mux;
		msg.internal.patch0 = msgId;
		return timer->addMsg( msg, msec );
	}
	bool _delTimerFromChild( UINT32 handle )
	{
		return timer->delMsg( handle );
	}

	void read( int msecTimeout )
	{
		UINT64 tWait = 0;
		if (_calculateTimeInWait)
		{
			tWait = getHighResCounter();
		}
		bool Ok = comm._tryRead( msecTimeout );
		if (_calculateTimeInWait)
		{
			_timeInWait += (getHighResCounter() - tWait);
		}
		if( Ok )
			handler.asyncReadOk();
	}

	void error()
	{
		_connected = false;
	}
	void postAll();

	bool hasMsg() const
	{
		return rdQueue.size() != 0;
	}
	void extractMsg( _CommMsg& m )//MUST have msg
	{
		PASSERT3( rdQueue.size() );
		m.moveFrom( rdQueue.peek() );
		rdQueue.pop();
	}
	void extractMsg( int& mux, UINT32& muxChannelId, _CommMsg& m )//MUST have msg
	{
		PASSERT3( rdQueue.size() );
		m.moveFrom( rdQueue.peek() );
		mux = m.internal.mux;
		muxChannelId = m.internal.muxChannelId;
		rdQueue.pop();
	}

public://for *Handler
	void msg( int mux, UINT32 muxChannelId, _CommMsg& msg )
	{
		msg.internal.mux = mux;
		msg.internal.muxChannelId = muxChannelId;
		rdQueue.push( msg );
	}
	void serviceCall(UINT32 id, CommMsgBody * msg_body)
	{
		_CommMsg msg( _COMM_MSGTYPE_SERVICEREQUEST, _COMM_MSGPRIORITIES-1 );
		msg.body.composeUINT32( id );
		if(msg_body)
		{
			msg.body.composeMsgBody(*msg_body);
		}
		rdQueue.push( msg );
	}
	int _getOutQueueSize() const override
	{
		return q.size() + handler.outQueueSize();
	}
};

class _CommLocalAppInterface : public _CommLocalAppInterfaceBase
{
private:
	int port;//host is always LOCALHOST
	PIPSocket sock;
public:
	_CommLocalAppInterface( int port_, const char * ident_ ) :
	  _CommLocalAppInterfaceBase(ident_)

	{
		comm.construct( sock );
		port = port_;
	}

	PBaseSocket& socket() override //for killHandler
	{
		return sock;
	}
	bool connect() override;
};
#ifdef PPOSIX
class _CommLocalAppInterfaceUnix : public _CommLocalAppInterfaceBase
{
private:
	PString address;//host is always LOCALHOST
	PUnixSocket sock;
public:
	_CommLocalAppInterfaceUnix( const char * address_, const char * ident_ ) :
		_CommLocalAppInterfaceBase(ident_)
	{
		comm.construct( sock );
		address = address_;
	}

	PBaseSocket& socket() override //for killHandler
	{
		return sock;
	}
	bool connect() override;
};
#endif

bool _CommLocalAppCommunicator::_tryRead( int msecTimeout )
{
	if( rdBuf )
	{
		rdRd = msecTimeout < 0 ? sock->recv( rdBuf, rdBufSz ) : sock->recv( rdBuf, rdBufSz, msecTimeout );
		return rdRd != 0;
	}
	else
	{
		PThread::sleep( msecTimeout );
		return false;
	}
}

bool _CommLocalAppInterface::connect()
{
	PASSERT3( !_connected );
	try
	{
		sock.connect( LOCALHOST, port );
		comm.init();
		handler.init( false );
		while( !handler.isConnected() )
		{
			comm._tryRead( -1 );
			handler.asyncReadOk();
		}
		_connected = true;
		handler.setFirstRead();
		return true;
	}
	catch( PError& err )
	{
		PLog( "CommLocalApp: Connect to %s:%d failed: %s", LOCALHOST, port, err.why() );
		return false;
	}
}
#ifdef PPOSIX
bool _CommLocalAppInterfaceUnix::connect()
{
	PASSERT3( !_connected );
	try
	{
		sock.connect( address.c_str() );
		comm.init();
		handler.init( false );
		while( !handler.isConnected() )
		{
			comm._tryRead( -1 );
			handler.asyncReadOk();
		}
		_connected = true;
		handler.setFirstRead();
		return true;
	}
	catch( PError& err )
	{
		PLog( "CommLocalApp: Connect to %s failed: %s", address.c_str(), err.why() );
		return false;
	}
}
#endif
void _CommLocalAppInterfaceBase::postAll()
{
	for(;;)
	{
		bool notEmpty = q.size() > 0;
		if( !notEmpty )
			break;
		_CommMsg& msg = q.peek();
		handler.storeMsg( msg.internal.mux, msg.internal.muxChannelId, msg );
		q.pop();
	}
	handler.flushStoredMsgs();

}

class _CommLocalKillHandler : public PProcessKillHandler
{
private:
	bool _shutdown;
	PBaseSocket* sock;

public:
	_CommLocalKillHandler( PBaseSocket& sock_, const char * pyrHandler = 0) :
	  PProcessKillHandler(pyrHandler)
	  {
		  sock = &sock_;
		  _shutdown = false;
	  }
	  bool shutdown()
	  {
		  return _shutdown;
	  }

public:
	bool signal() override
	{
		_shutdown = true;
		sock->close();
		return true;
	}
};


static inline int _hex( char c )
{
	if( c >= '0' && c <= '9' )
		return c - '0';
	else if( c >= 'a' && c <= 'f' )
		return c - 'a' + 10;
	else if( c >= 'A' && c <= 'F' )
		return c - 'A' + 10;
	else
		return 0;
}

static void _loadInitMsg( CommMsgBody& initMsg, const char* envName )
{
	char* env = getenv( envName );
	if( env )
	{
		int sz = strlen( env ) / 2;
		BYTE* dst = initMsg._append( sz );
		const char* src = env;
		for( int i=0; i < sz ; ++i )
		{
			char c0 = *src++;
			char c1 = *src++;
			int cc = ( _hex( c0 ) << 4 ) | _hex( c1 );
			*dst++ = (BYTE)cc;
		}
	}
}

static int _usage( const char* argv0 )
{
	PLogInit( new PLogFile( 0, PLogFile::alsoToStdout | PLogFile::catchPErrors | PLogFile::catchSystemDbgOutput /* no thread id */ ) );
	PLog( "%s: <port> <name> <logFileName> [<initMsgEnvVarName>]", argv0 );
	return -1;
}


//****************************************************************************

#if 1

class _CommLocalAppChildInterface : public _CommInterface//, public _CommTimerInterface
{
private:
	_CommLocalAppInterfaceBase* parent;
	int mux;
	UINT32 muxChannelId;

public:
	_CommLocalAppChildInterface( _CommLocalAppInterfaceBase& parent_, int mux_ )
	{
		parent = &parent_;
		mux = mux_;
	}
	void _setChannelId( UINT32 channelId )
	{
		muxChannelId = channelId;
	}
	void _post( _CommMsg& m ) override
	{
		parent->_postFromChild( mux, muxChannelId, m );
	}
	UINT32 _postClientTimer( _CommMsg& msg, int msec ) override
	{
		return parent->_postTimerFromChild( mux, muxChannelId, msg, msec );
	}
	UINT32 _addServerTimer( UINT32 msgId, CommMsgBody& body, int msec ) override
	{
		return parent->_addTimerFromChild( mux, muxChannelId, msgId, body, msec );
	}
	bool _delServerTimer( UINT32 handle ) override
	{
		return parent->_delTimerFromChild( handle );
	}
	int _getInQueueSize() const override
	{
		return parent->_CommLocalAppInterfaceBase::_getInQueueSize();
	}
	UINT64 _getWaitInQueueTime(UINT64 *readTime , UINT64 *writeTime ) override
	{
		return parent->_CommLocalAppInterfaceBase::_getWaitInQueueTime(readTime , writeTime );
	}
	const char * _getIdent() const override
	{
		return parent->_getIdent();
	}
	int _getOutQueueSize() const override
	{
		return parent->_getOutQueueSize();
	}

};

class _CommLocalAppManager
{
public:
	/* abstract */ class Callback
	{
	public:
		virtual void objectDestroyed( const char* name ) = 0;
	};

private:
	CommServerObjectFactory& factory;
	_CommTimer& timer;
	Callback* callback;
	PString names[ _COMM_MAX_LOCALAPP_OBJECTS ];
	/* new */ _CommLocalAppChildInterface* inters[ _COMM_MAX_LOCALAPP_OBJECTS ];
	/* new */ CommServerObject* objects[ _COMM_MAX_LOCALAPP_OBJECTS ];
	UINT32 channelIds[ _COMM_MAX_LOCALAPP_OBJECTS ];
public:
	UINT32 nextChannelId;

public:
	_CommLocalAppManager( CommServerObjectFactory& factory_, _CommLocalAppInterfaceBase& inter, _CommTimer& timer_ );

	void _setCallback( Callback* callback_ )
	{
		callback = callback_;
	}
	_CommLocalAppChildInterface& _getInterface0()
	{
		return *inters[ 0 ];
	}
	void _setObject0( const char* name, UINT32 channelId, /* new */ CommServerObject* obj )
	{
		PASSERT3( objects[ 0 ] == 0 );
		objects[ 0 ] = obj;
		names[ 0 ] = name;
		channelIds[ 0 ] = channelId;
		inters[ 0 ]->_setChannelId( channelId );
	}
	void connected();

	bool createNewObject( const char* name, CommMsgBody& initMsg );
	CommServerObject* _getObject( UINT32& channelId, int i )
	{
		PASSERT3( i >= 0 && i < _COMM_MAX_LOCALAPP_OBJECTS );
		channelId = channelIds[ i ];
		return objects[ i ];
	}
	bool _destroyObject(int idx );//returns true if object #0 (factory) is destroyed
	bool destroyObjectAndNotify(_CommTimer & timer, int idx );//returns true if object #0 (factory) is destroyed
	void shutdown();
	bool terminated();
	~_CommLocalAppManager();
};

_CommLocalAppManager::_CommLocalAppManager( CommServerObjectFactory& factory_, _CommLocalAppInterfaceBase& inter, _CommTimer& timer_ )
: factory( factory_ ),
timer( timer_ )
{
	callback = 0;
	nextChannelId = RANDOMTIME();
	for( int i=0; i < _COMM_MAX_LOCALAPP_OBJECTS ; ++i )
	{
		inters[ i ] = new _CommLocalAppChildInterface( inter, i );
		objects[ i ] = 0;
		channelIds[ i ] = 0;
	}
}

bool _CommLocalAppManager::createNewObject( const char* name, CommMsgBody& initMsg )
{
	int idx = -1;
	for( int i=0; i < _COMM_MAX_LOCALAPP_OBJECTS ; ++i )
		if( objects[ i ] == 0 )
		{
			idx = i;
			break;
		}
	PASSERT3( idx >= 0 );

	PTRACE3( "_CommLocalAppManager::createNewObject: object '%s' placed to slot %d", name, idx );

	CommServerObject* obj = factory.createServerObject( *inters[ idx ]  );
	names[ idx ] = name;
	objects[ idx ] = obj;

	UINT32 channelId = nextChannelId++;
	//	obj->_init( /*idx, channelId,*/ *inters[ idx ] /* as _CommTimerInterface */ );
	inters[ idx ]->_setChannelId( channelId );
	channelIds[ idx ] = channelId;
	bool destroy = obj->_safeInit( initMsg );
	if( destroy )
	{
		destroy = _destroyObject( idx );
		PASSERT3( !destroy );
		return false;
	}

	_CommMsg m( _COMM_MSGTYPE_REGISTER_SERVEROBJECT, _COMM_MSGPRIORITY_INITCONNECT );
	m.body.composeUINT32( 0 ).composeString( name );
	inters[ idx ]->_post( m );
	return true;
}

bool _CommLocalAppManager::_destroyObject( int idx )//returns true if object #0 (factory) is destroyed
{
	PASSERT3( idx >= 0 && idx < _COMM_MAX_LOCALAPP_OBJECTS );
	PASSERT3( objects[ idx ] != 0 );

	PTRACE3( "_CommLocalAppManager::_destroyObject: object '%s' in slot %d destroyed", names[ idx ].c_str(), idx );

	delete objects[ idx ];
	objects[ idx ] = 0;
	names[ idx ] = ""; 
	channelIds[ idx ] = 0;
	return idx == 0;
}

bool _CommLocalAppManager::destroyObjectAndNotify(_CommTimer & timer,  int idx )//returns true if object #0 (factory) is destroyed
{
	PASSERT3( idx >= 0 && idx < _COMM_MAX_LOCALAPP_OBJECTS );
	PASSERT3( objects[ idx ] != 0 );

	//it is essential that _COMM_MSGTYPE_UNREGISTER_SERVEROBJECT should be posted before calling callback->objectDestroyed()
	//AND connection to AppFactory should have 0 priority
	if( idx > 0 )//for factory object does not make much sense
	{
		_CommMsg m( _COMM_MSGTYPE_UNREGISTER_SERVEROBJECT, _COMM_MSGPRIORITY_INITCONNECT );
		inters[ idx ]->_post( m );
	}
	if( callback )
		callback->objectDestroyed( names[ idx ] );

	timer.delAllForSlot(idx);
	return _destroyObject( idx );
}

void _CommLocalAppManager::connected()
{
	//inform routing table about server objects connected
	for( int i=0; i < _COMM_MAX_LOCALAPP_OBJECTS ; ++i )
	{
		if( objects[ i ] )
		{
			const char* name = names[ i ];
			_CommMsg m( _COMM_MSGTYPE_REGISTER_SERVEROBJECT, _COMM_MSGPRIORITY_INITCONNECT );
			m.body.composeUINT32( 0 ).composeString( name );
			inters[ i ]->_post( m );
		}
	}
}

void _CommLocalAppManager::shutdown()
{
	for( int i=0; i < _COMM_MAX_LOCALAPP_OBJECTS ; ++i )
		if( objects[ i ] )
			objects[ i ]->_safeShutdown();
}

bool _CommLocalAppManager::terminated()
{
	for( int i=0; i < _COMM_MAX_LOCALAPP_OBJECTS ; ++i )
		if( objects[ i ] && objects[ i ]->_safeTerminated() == 0 )
			return false;
	return true;
}

_CommLocalAppManager::~_CommLocalAppManager()
{
	for( int i=_COMM_MAX_LOCALAPP_OBJECTS - 1; i >= 0 ; --i )
	{
		delete objects[ i ];
		delete inters[ i ];
	}
}

class _CommLocalAppSubFactory;
class _CommLocalAppSubFactoryConnection : public CommClientConnection
{
private:
	_CommLocalAppSubFactory* obj;

public:
	_CommLocalAppSubFactoryConnection( CommClientGuardFactory& guardFactory )
		: CommClientConnection( guardFactory )
	{
		obj = 0;
	}
	void init( _CommLocalAppSubFactory* obj_ )
	{
		obj = obj_;
	}

	void connected() override;
	void processMessage( UINT32 msgId, const CommMsgBody& body ) override;
};

class _CommLocalAppSubFactory : public CommServerObject, public _CommLocalAppManager::Callback
{
private:
	PString name;
	_CommLocalAppManager& mgr;
	CommClientConnectionPool cliPool;
	_CommLocalAppSubFactoryConnection conn;
	vector< PString > objects;
	static CommClientNullGuardFactory clientGuardFactory;
	_CommRunToSubFactoryInterface & run2subfactory;

public:
	_CommLocalAppSubFactory( _CommInterface& inter_, _CommLocalAppManager& mgr_, _CommRunToSubFactoryInterface & run2subfactory_ );
	/* virtual - CommServerObject */ void init( CommMsgBody& body ) override;
	/* virtual - CommServerObject */ void processMessage( _CommMsg& msg ) override;
	/* virtual - Callback */ void objectDestroyed( const char* name ) override;

	void connected();
	void processMessage( UINT32 msgId, const CommMsgBody& body );
	void updateSEHNumber(INT32 num,INT32 numAsserts,INT32 numMemory);
};

/* static */ CommClientNullGuardFactory _CommLocalAppSubFactory::clientGuardFactory;

_CommLocalAppSubFactory::_CommLocalAppSubFactory( _CommInterface& inter_, _CommLocalAppManager& mgr_,_CommRunToSubFactoryInterface & run2subfactory_ )
: CommServerObject( inter_ ),
mgr( mgr_ ),
cliPool( inter_ ),
conn( clientGuardFactory ),
run2subfactory(run2subfactory_)
{
	mgr._setCallback( this );
	conn.init( this );
}

/* virtual - CommServerObject */ void _CommLocalAppSubFactory::init( CommMsgBody& body )
{
	CommMsgParser parser( body );
	const char* name_;
	const char* server;
	const char* serverObject;
	const char* serverChannel;
	parser.parseString( name_ );
	name = name_;
	parser.parseString( server ).parseString( serverObject ).parseString( serverChannel );
	cliPool.connect( conn, server, serverObject, serverChannel );
}

/* virtual - CommServerObject */ void _CommLocalAppSubFactory::processMessage( _CommMsg& msg )
{
	cliPool._processMessage( msg );
}

/* virtual */ void _CommLocalAppSubFactoryConnection::connected()
{
	obj->connected();
}

/* virtual */ void _CommLocalAppSubFactoryConnection::processMessage( UINT32 msgId, const CommMsgBody& body )
{
	obj->processMessage( msgId, body );
}

void _CommLocalAppSubFactory::connected()
{
	CommMsgBody body;
	body.composeString( name );
	int n = objects.size();
	for( int i=0; i < n ; ++i )
		body.composeString( objects[ i ] );
	body.composeString( "" );
	conn.post( _COMMAPPFACTORY_SUBFACTORY_CONNECTED, body );
}

void _CommLocalAppSubFactory::updateSEHNumber(INT32 num,INT32 numAsserts,INT32 numMemory)
{
	CommMsgBody body;
	body.composeUINT32(1234)
		.composeINT32(num)
		.composeINT32(numAsserts)
		.composeINT32(numMemory);
	conn.post( _COMMAPPFACTORY_SUBFACTORY_SEH_NUMBER_UPDATE, body );
}


void _CommLocalAppSubFactory::processMessage( UINT32 msgId, const CommMsgBody& body )
{
	switch( msgId )
	{
	case _COMMAPPFACTORY_SUBFACTORY_CREATEOBJECT:
		{
			CommMsgParser parser( body );

			UINT32 reqId;
			const char* objName;
			CommMsgBody initMsg;
			parser.parseUINT32( reqId ).parseString( objName ).parseMsgBody( initMsg );

			bool Ok = mgr.createNewObject( objName, initMsg );
			if( Ok )
			{
				objects.push_back( objName );

				CommMsgBody reply;
				reply.composeUINT32( reqId ).composeINT16( 0 ).composeString( "" );
				conn.post( _COMMAPPFACTORY_SUBFACTORY_CREATEOBJECT_REPLY, reply );
			}
			else
			{
				CommMsgBody reply;
				reply.composeUINT32( reqId ).composeINT16( COMMAPPFACTORY_ERROR_INITFAILED ).composeString( "Object initialization failed" );
				conn.post( _COMMAPPFACTORY_SUBFACTORY_CREATEOBJECT_REPLY, reply );
			}
		}
		break;

	case _COMMAPPFACTORY_SUBFACTORY_EXIT:
		exit();
		break;

	case _COMMAPPFACTORY_SUBFACTORY_REOPENLOG:
		{
			PTRACE3("_CommLocalAppSharedMemSubFactory::processMessage(): Request to reopen log file");
			PLogReopen();
		}
		break;

	default:
		PLog( "_CommLocalAppSubFactory::processMessage: unknown msgId=%X", msgId );
		break;
	}
}

/* virtual - Callback */ void _CommLocalAppSubFactory::objectDestroyed( const char* objectName )
{
	int n = objects.size();
	for( int i=0; i < n ; ++i )
	{
		if( strcmp( objects[ i ], objectName ) == 0 )
		{
			objects.erase( objects.begin() + i );//safe only because of return
			CommMsgBody body;
			body.composeString( name ).composeString( objectName );
			conn.post( _COMMAPPFACTORY_SUBFACTORY_OBJECTDEAD, body );
			return;
		}
	}
	PLog( "_CommLocalAppSubFactory::objectDestroyed(): object '%s' not found", objectName );
}

static void _runMulti0( CommServerObjectFactory& factory, _CommLocalAppInterfaceBase & inter, const char* name, CommMsgBody& initMsg )
{
	try
	{
		bool interfaceClosed=false;
		int numberOfSEH = 0;
		int numberOfAsrt = 0;
		int numberOfMemoryFailure = 0;
		inter.enableMux();
		_CommLocalKillHandler killHandler( inter.socket() );
		_CommTimer timer;
		inter._setTimer( timer );
		_CommRunToSubFactoryInterface run2subfactory(timer);
		_CommLocalAppManager mgr( factory, inter, timer );

		PLog( "CommMultiLocalApp:: Creating Factory Server '%s'", name );
		_CommLocalAppSubFactory* subFactory = new _CommLocalAppSubFactory( mgr._getInterface0() /* as _CommInterface */ , mgr, run2subfactory );
		UINT32 channelId = mgr.nextChannelId++;
		mgr._setObject0( name, channelId, subFactory );
		//		subFactory->_init( /*0, channelId,*/ mgr._getInterface0() /* as _CommTimerInterface */ );
		bool destroy = subFactory->_safeInit( initMsg );
		if( destroy )
			return;

		for(;;)
		{
			if( killHandler.shutdown() )
			{
				PLog( "CommLocalApp: Kill signal received - exiting" );

				mgr.shutdown();
				bool Ok = _CommWaitUntilTerminated( mgr, 10 );
				if( Ok )
					PLog( "CommLocalApp: objects terminated" );
				else
				{
					PLog( "CommLocalApp: objects not terminated - exiting anyway" );
					FATALEXIT();
				}

				return;
			}
			else if (interfaceClosed)
			{
				PLog( "CommLocalApp: Interface closed unexpectedly, exit application..." );
				return;
			}


			try
			{
				if( !inter.connected() )
				{
					bool Ok = inter.connect();
					if( Ok )
					{
						mgr.connected();
						inter.postAll();
					}
					else
						PThread::sleep( 1000 );
				}
				int timeout=-1;
				struct tm tm = { 0 };
				int timerCycles = _COMM_MAX_LOCALAPP_OBJECTS * 2;
				while(timerCycles--)
				{
					PCurrentLocalTime( &tm );
					int untilMsec = (((23 - tm.tm_hour)*60 + (59 - tm.tm_min))*60 + (59 - tm.tm_sec) + 2)*1000;
					timeout = timer.msecUntilFirstOrParam(untilMsec);

					if( timeout == 0 )
					{
						_CommMsg msg;
						UINT32 timerHandle = timer.pop( msg );

						int objIdx = msg.internal.slot;
						UINT32 channelId;
						CommServerObject* obj = mgr._getObject( channelId, objIdx );
						if( !obj )
						{
							PTRACE3( "LocalMultiApp[ %d ]: timer message for non-existing object - ignored", objIdx );
						}
						else if( channelId != msg.internal.channelId )
						{// Critical check. Protects from dispatching the message to the wrong channel
							// when connection was deleted and then another connection created in the same slot.
							PTRACE3( "LocalMultiApp[ %d ]: timer message to obsolete channelId=%08X - ignored", objIdx, msg.internal.channelId );
							obj = 0;//to ignore message
						}
						if( obj )
						{
							bool destroy = false;
							if( msg.type == _COMM_MSGTYPE_NONE )
								destroy = obj->_safeProcessTimerMessage( timerHandle, msg.internal.patch0, msg.body );
							else
							{
								PASSERT3( msg.type == _COMM_MSGTYPE_CONNPOOL_TIMER || msg.type == _COMM_MSGTYPE_SUBSCRPOOL_TIMER ||
									msg.type == _COMM_MSGTYPE_CLICONNPOOL_TIMER_CONNECTION_EX || msg.type == _COMM_MSGTYPE_SRVCONNPOOL_TIMER_CONNECTION_EX);
								destroy = obj->_safeProcessMessage( msg );
							}
							inter.postAll();
							if( destroy && mgr.destroyObjectAndNotify( timer, objIdx ) )
								return;
							inter.postAll();
						}
					}
					else
						break;//forever timer
				}
				if( !inter.connected() )
					continue;//forever

				int numSEH = PSystemExceptionError::getNumberOfExceptions();
				int numAsrt = PAssert::getNumberOfAsserts();
				int numMemoryFailure = PSystemMemoryError::getNumberOfExceptions();

				if (numberOfSEH != numSEH || numAsrt != numberOfAsrt || numMemoryFailure != numberOfMemoryFailure)
				{
					subFactory->updateSEHNumber(numSEH - numberOfSEH,numAsrt - numberOfAsrt,numMemoryFailure - numberOfMemoryFailure);
					inter.postAll();
					numberOfSEH = numSEH;
					numberOfAsrt = numAsrt;
					numberOfMemoryFailure = numMemoryFailure;
				}
				inter.read( timeout );
				struct tm tm1;
				PCurrentLocalTime( &tm1 );
				if( tm.tm_mday != tm1.tm_mday )
				{
					PLogCheckRotate();
				}
				while( inter.hasMsg() )
				{
					_CommMsg msg;
					int mux;
					UINT32 muxChannelId;
					inter.extractMsg( mux, muxChannelId, msg );
					int objIdx = mux;
					UINT32 channelId;
					CommServerObject* obj = mgr._getObject( channelId, objIdx );
					if( !obj )
					{
						PTRACE3( "LocalMultiApp[ %d ]: message for non-existing object - ignored", objIdx );
					}
					else if( channelId != muxChannelId )
					{// Critical check. Protects from dispatching the message to the wrong channel
						// when connection was deleted and then another connection created in the same slot.
						PTRACE3( "LocalMultiApp[ %d ]: message to obsolete channelId=%08X - ignored", objIdx, muxChannelId );
						obj = 0;//to ignore message
					}

					if( obj )
					{
						switch( msg.type )
						{
						case _COMM_MSGTYPE_REGISTER_SERVEROBJECT_REPLY:
							{
								BYTE err;
								UINT32 reqId;
								CommMsgParser parser( msg.body );
								parser.parseUINT32( reqId ).parseBYTE( err );

								if( err )
								{
									PLog( "CommLocalApp: ServerObject: Unable to register name with CommRoutingTable (err=%02X) - object destroyed", err );
									if( mgr.destroyObjectAndNotify( timer, objIdx ) )
										return;
									inter.postAll();
									break;
								}

								PLog( "CommLocalApp: ServerObject registered" );

								//inform server object about connection
								_CommMsg m( _COMM_MSGTYPE_PHYSICAL_CONNECT, _COMM_MSGPRIORITY_INVALID );
								bool destroy = obj->_safeProcessMessage( m );
								inter.postAll();
								if( destroy && mgr.destroyObjectAndNotify( timer, objIdx ) )
									return;
								inter.postAll();
							}
							break;

						default:
							{
								bool destroy = obj->_safeProcessMessage( msg );
								inter.postAll();
								if( destroy && mgr.destroyObjectAndNotify( timer, objIdx ) )
									return;
								inter.postAll();
							}
							break;
						}
					}
				}
			}
			catch( PError& err )
			{
				PLog( "LocalApp::runMulti() exception - interface closed: '%s'", err.why() );
				inter.error();
				interfaceClosed=true;
			}
		}//forever
	}
	catch( PError& err )
	{
		PLog( "LocalApp::runMulti0() last hope exception caught: '%s'", err.why() );
	}
	catch(  exception& exc  )
	{
		PLog( "LocalApp::runMulti0() last hope STD exception caught: '%s'", exc.what() );
	}

	catch( ... )
	{
		PLog( "LocalApp::runMulti0() unknown last hope exception caught" );
	}
}

static void _runMulti( CommServerObjectFactory& factory, int port, const char* name, CommMsgBody& initMsg, const char * ident_ )
{
	try
	{
		_CommLocalAppInterface inter( port , ident_);
		_runMulti0( factory, inter, name, initMsg );
	}
	catch( PError& err )
	{
		PLog( "LocalApp::runMulti() last hope exception caught: '%s'", err.why() );
	}
	catch(  exception& exc  )
	{
		PLog( "LocalApp::runMulti() last hope STD exception caught: '%s'", exc.what() );
	}

	catch( ... )
	{
		PLog( "LocalApp::runMulti() unknown last hope exception caught" );
	}
}
#ifdef PPOSIX
static void _runMulti( CommServerObjectFactory& factory, const char * port, const char* name, CommMsgBody& initMsg, const char * ident_ )
{
	try
	{
		_CommLocalAppInterfaceUnix inter( port, ident_ );
		_runMulti0( factory, inter, name, initMsg );
	}
	catch( PError& err )
	{
		PLog( "LocalApp::runMulti() last hope exception caught: '%s'", err.why() );
	}
	catch(  exception& exc  )
	{
		PLog( "LocalApp::runMulti() last hope STD exception caught: '%s'", exc.what() );
	}

	catch( ... )
	{
		PLog( "LocalApp::runMulti() unknown last hope exception caught" );
	}
}
#endif

static void _CommMultiLocalAppMain( int argc, char** argv, CommServerObjectFactory& factory, const char* logMsg )
{
	PLogInit( new PLogFile( argv[ 3 ],  /*PLogFile::printProcessId |*/ PLogFile::perDate | PLogFile::catchPErrors | PLogFile::catchSystemDbgOutput /* no thread id */ ) );
	PLog( "CommMultiLocalApp: Starting %s...", logMsg ? logMsg : "" );
	PSocket::startup();

	int port = atoi( argv[ 1 ] );
	const char* name = argv[ 2 ];
	const char * ident = "";
//	int restoreFlag = atoi(argv[ 4 ]);
	CommMsgBody initMsg;
	if( argc >= 6 )
	{
		const char* envName = argv[ 5 ];
		_loadInitMsg( initMsg, envName );
	}
	if( argc >= 7 )
	{
		PLog("Identification = '%s'",argv[ 6 ]);
		ident = argv[ 6 ];
	}

	PLog( "CommMultiLocalApp: Running..." );
	_runMulti( factory, port, name,  initMsg, ident );
	PLog( "CommMultiLocalApp: Exiting..." );

	PSocket::cleanup();
}
#ifdef PPOSIX
static void _CommMultiLocalAppUnixMain( int argc, char** argv, CommServerObjectFactory& factory, const char* logMsg )
{
#ifdef P_MAC10X
	PLogInit( new PLogFile( argv[ 3 ],  /*PLogFile::printProcessId |*/ PLogFile::perDateLimited | PLogFile::catchPErrors | PLogFile::catchSystemDbgOutput /* no thread id */ ) );
#else
	PLogInit( new PLogFile( argv[ 3 ],  /*PLogFile::printProcessId |*/ PLogFile::perDate | PLogFile::catchPErrors | PLogFile::catchSystemDbgOutput /* no thread id */ ) );
#endif
	PLog( "CommMultiLocalAppUnix: Starting %s...", logMsg ? logMsg : "" );
	PSocket::startup();

	const char * address = argv[ 1 ];
	const char* name = argv[ 2 ];
	const char * ident = "";
	int restoreFlag = atoi(argv[ 4 ]);
	CommMsgBody initMsg( false );
	if( argc >= 6 )
	{
		const char* envName = argv[ 5 ];
		_loadInitMsg( initMsg, envName );
	}
	if( argc >= 7 )
	{
		PLog("Identification = '%s'",argv[ 6 ]);
		ident = argv[ 6 ];
	}

	PLog( "CommMultiLocalApp: Running..." );
	_runMulti( factory, address, name, initMsg, ident );
	PLog( "CommMultiLocalAppUnix: Exiting..." );

	PSocket::cleanup();
}
#endif
//****************************************************************************

class _CommMultiLocalApp : public PErrorHandler//to create safety wrapper around _CommLocalAppMain
{
private:
	int argc;
	char** argv;
	CommServerObjectFactory& factory;
	const char* logMsg;

public:
	_CommMultiLocalApp( int argc_, char** argv_, CommServerObjectFactory& factory_, const char* logMsg_ )
		: factory( factory_ )
	{
		argc = argc_;
		argv = argv_;
		logMsg = logMsg_;
	}
	void run() override;
};

/* virtual */ void _CommMultiLocalApp::run()
{
	_CommMultiLocalAppMain( argc, argv, factory, logMsg );
}

int CommMultiLocalAppMain( int argc, char** argv, CommServerObjectFactory& factory, const char* logMsg )
{
	if (argc == 2 && !strcmp(argv[1],"-v"))
	{
		printf("%s", logMsg);
		return 0;
	}
#if defined(PWIN32) && !defined(COMM_CLIENT_ONLY)
	else if (argc == 3 && !strcmp(argv[1], "-recoverstacktrace"))
	{
		StackTrace::decodeStackTrace(argv[2]);
		return 0;
	}
#endif

	if( argc < 5 )
		return _usage( argv[ 0 ] );

	_CommMultiLocalApp app( argc, argv, factory, logMsg );
	app.execute();
	return 0;
}


int CommLocalAppMain( int argc, char** argv, CommServerObjectFactory& factory, const char* logMsg )
{
	if (argc == 2 && !strcmp(argv[1],"-v"))
	{
		printf("%s", logMsg);
		return 0;
	}
#if defined(PWIN32) && !defined(COMM_CLIENT_ONLY)
	else if (argc == 3 && !strcmp(argv[1], "-recoverstacktrace"))
	{
		StackTrace::decodeStackTrace(argv[2]);
		return 0;
	}
#endif
	
	if( argc < 5 )
		return _usage( argv[ 0 ] );

	_CommMultiLocalApp app( argc, argv, factory, logMsg );
	app.execute();
	return 0;
}

//****************************************************************************
#ifdef PPOSIX
class _CommMultiLocalUnixApp : public PErrorHandler//to create safety wrapper around _CommLocalAppMain
{
private:
	int argc;
	char** argv;
	CommServerObjectFactory& factory;
	const char* logMsg;

public:
	_CommMultiLocalUnixApp( int argc_, char** argv_, CommServerObjectFactory& factory_, const char* logMsg_ )
		: factory( factory_ )
	{
		argc = argc_;
		argv = argv_;
		logMsg = logMsg_;
	}
	void run() override;
};

/* virtual */ void _CommMultiLocalUnixApp::run()
{
	_CommMultiLocalAppUnixMain( argc, argv, factory, logMsg );
}

int CommMultiLocalAppUnixMain( int argc, char** argv, CommServerObjectFactory& factory, const char* logMsg )
{
	if (argc == 2 && !strcmp(argv[1],"-v"))
	{
		printf("%s", logMsg);
		return 0;
	}

	if( argc < 5 )
		return _usage( argv[ 0 ] );

	_CommMultiLocalUnixApp app( argc, argv, factory, logMsg );
	app.execute();
	return 0;
}


int CommLocalAppUnixMain( int argc, char** argv, CommServerObjectFactory& factory, const char* logMsg )
{
	if (argc == 2 && !strcmp(argv[1],"-v"))
	{
		printf("%s", logMsg);
		return 0;
	}

	if( argc < 5 )
		return _usage( argv[ 0 ] );

	_CommMultiLocalUnixApp app( argc, argv, factory, logMsg );
	app.execute();
	return 0;
}
#endif


#endif

