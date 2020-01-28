#include "ppinclude.h"
#include "ppsocket.h"
#include "ppprocess.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "commsrv.h"
#include "commcli.h"
#include "commgrdnull.h"
#include "commappfactory.h"
#include "commsharedmemintrf.h"
#include "commsharedmem.h"
#include "commsharedmemloop.h"

#ifdef PWIN32
#include "ppstacktrace.h"
#endif

struct _CommRunToSubFactoryInterface 
{
	_CommTimer & timer;
	_CommRunToSubFactoryInterface(_CommTimer &_timer):timer(_timer)
	{
	}
};

#if PTRACELVL >= 5
struct ObjectMsgStats
{
    ObjectMsgStats() { reset(); }

    void reset()
    {
        numMsgsRecvd = numMsgsSent = 0;
    }

    UINT32 numMsgsRecvd;
    UINT32 numMsgsSent;
};
#endif

class _CommLocalAppSharedMemInterface : public _CommInterface//, public _CommTimerInterface
{
private:
	PString resource;//host is always LOCALHOST
	PString localResource;
	PString ident;

	bool _connected;
	CommMsgSimpleQueue q;
	_CommTimer* timer;
	UINT64 _timeInWait;
	UINT64 _timeInRead;
	UINT64 _timeInWrite;

	bool _calculateTimeInWait;

	CommMsgSimpleQueue rdQueue;
	CommMsgQueue       internalCommQueue;
	PWaitForObjects multiWait;

#if PTRACELVL >= 5
    ObjectMsgStats objMsgsStats[_COMM_MAX_LOCALAPP_OBJECTS];
#endif


public:
	CommSharedMemoryLoop comm;

	_CommLocalAppSharedMemInterface( int port_, const char * ident_ ): multiWait(3)
	{
		char tmpName[256];
		sprintf(tmpName,"pyrl_%8.8lx%8.8lx%8.8lx",PID(),THREADID(),SYSTEMTICKS());
		localResource.assign(tmpName);
		comm.Create(tmpName);
		sprintf(tmpName,"pyr_%4.4x",port_);
		resource.assign(tmpName);
		_connected = false;
		timer = 0;
		_calculateTimeInWait = false;
		_timeInWait = 0;
		_timeInRead = 0;
		_timeInWrite = 0;
		ident = ident_;

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
	void enableMux()
	{
		comm.enableMux();
	}

public://{_CommInterface
	void _internalPost(_CommMsg& m ) override
	{
		m.type=_COMM_MSGTYPE_INTERNAL_COMMUNICATION;
		m.internal.mux = 0;
		m.internal.muxChannelId = 0;
		internalCommQueue.push(m);
	}
	void _post( _CommMsg& m ) override
	{
		m.internal.mux = 0;
		m.internal.muxChannelId = 0;
		q.push( m );
#if PTRACELVL >= 5
        auto& stats = _getMsgStats(m.internal.mux);
        ++stats.numMsgsSent;
#endif
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
	int _getOutQueueSize() const override
	{
		return q.size() + comm.wrQueue.size();
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
			if (readTime)
				*readTime=0;
			if (writeTime)
				*writeTime=0;
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

	void _internalPostFromChild(int mux, UINT32 muxChannelId,_CommMsg& m )
	{
		m.internal.mux = mux;
		m.internal.muxChannelId = muxChannelId;
		internalCommQueue.push(m);
	}

	void _postFromChild( int mux, UINT32 muxChannelId, _CommMsg& m )
	{
		m.internal.mux = mux;
		m.internal.muxChannelId = muxChannelId;
		q.push( m );
#if PTRACELVL >= 5
        auto& stats = _getMsgStats(m.internal.mux);
        ++stats.numMsgsSent;
#endif
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
		SOCKET hnds[3];
		hnds[1]=comm.getReadHandle();
		hnds[0]=comm.getWriteHandle();
		hnds[2]=internalCommQueue.getSocket();
		for (;;)
		{
			UINT64 tWait = 0;
			if (_calculateTimeInWait)
			{
				tWait = getHighResCounter();
			}
			int w = multiWait.waitMultiple(sizeof(hnds)/sizeof(*hnds),hnds, ((msecTimeout < 0) ? PWaitForObjects::infinite : msecTimeout) );
			if (_calculateTimeInWait)
			{
				_timeInWait += (getHighResCounter() - tWait);
			}

			if (w==PWaitForObjects::waitTimeout)
				return;

			if (w==PWaitForObjects::waitFailed)
				throw PInternalError("Wait abandoned");
			switch(w)
			{
			case 1:
				{
					comm.resetReadEvent();
					if (_calculateTimeInWait)
					{
						tWait = getHighResCounter();
					}
					int code;
					_CommMsg msg;
					bool recvSomething = false;
					while( 0 != (code=comm.getMessage(msg)))
					{
						if (code == -1)
						{
							throw PInternalError("Peer close connection");
						}
						else
						{
							recvSomething = true;
							rdQueue.push( msg );
						}
					}
					if (_calculateTimeInWait)
					{
						_timeInRead += (getHighResCounter() - tWait);
					}

					if (recvSomething)
						return;
				}
				break;

			case 2:
				{
					_CommMsg msg;
					bool recvSomething = false;
					while(internalCommQueue.pop(msg))
					{
						recvSomething = true;
						rdQueue.push( msg );
					}
					if (recvSomething)
						return;
				}
				break;

			case 0:
				{
					comm.resetWriteEvent();
					if (_calculateTimeInWait)
					{
						tWait = getHighResCounter();
					}
					comm.peerReadData();
					if (_calculateTimeInWait)
					{
						_timeInWrite += (getHighResCounter() - tWait);
					}
				}
				break;
			default:
				throw PInternalError("Wait abandoned");
			}
		}
	}

	bool connect();
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
		UINT64 tWait;
		if (_calculateTimeInWait)
		{
			tWait = getHighResCounter();
		}
		PASSERT3( rdQueue.size() );
		m.moveFrom( rdQueue.peek() );
		rdQueue.pop();
		if (_calculateTimeInWait)
		{
			_timeInRead += (getHighResCounter() - tWait);
		}

	}
	void extractMsg( int& mux, UINT32& muxChannelId, _CommMsg& m )//MUST have msg
	{
		UINT64 tWait = 0;
		if (_calculateTimeInWait)
		{
			tWait = getHighResCounter();
		}

		PASSERT3( rdQueue.size() );
		m.moveFrom( rdQueue.peek() );
		mux = m.internal.mux;
		muxChannelId = m.internal.muxChannelId;
		rdQueue.pop();
		if (_calculateTimeInWait)
		{
			_timeInRead += (getHighResCounter() - tWait);
		}

	}

#if PTRACELVL >= 5
    ObjectMsgStats& _getMsgStats(int i)
    {
        PASSERT3(i >= 0 && i < _COMM_MAX_LOCALAPP_OBJECTS);
        return objMsgsStats[i];
    }
#endif
};


bool _CommLocalAppSharedMemInterface::connect()
{
	PASSERT3( !_connected );
	try
	{
		CommSharedMemory connect;
		bool ret = connect.Connect(resource.c_str(),localResource.c_str());
		if (ret)
		{
			_connected = true;
			return true;
		}
		else
		{
			PLog( "CommLocalApp: Connect to sharedmem:%s failed: Resource is busy", resource.c_str() );
			return false;
		}
	}
	catch( PError& err )
	{
		PLog( "CommLocalApp: Connect to sharedmem:%s failed: %s", resource.c_str(), err.why() );
		return false;
	}
}

void _CommLocalAppSharedMemInterface::postAll()
{
	UINT64 tWait = 0;
	if (_calculateTimeInWait)
	{
		tWait = getHighResCounter();
	}

	for(;;)
	{
		bool notEmpty = q.size() > 0;
		if( !notEmpty )
			break;
		_CommMsg& msg = q.peek();
		comm.pushMessage(msg );
		q.pop();
	}
	if (_calculateTimeInWait)
	{
		_timeInWrite += (getHighResCounter() - tWait);
	}

}

class _CommLocalKillHandler : public PProcessKillHandler
{
private:
	bool _shutdown;
	CommSharedMemoryIntrf *comm;

public:
	_CommLocalKillHandler( CommSharedMemoryIntrf& comm_, const char * pyrHandler = 0, UINT32 ppId = 0 ) :
	  PProcessKillHandler(pyrHandler, ppId)

	  {
		  comm = &comm_;
		  _shutdown = false;
	  }
	  bool shutdown()
	  {
		  return _shutdown;
	  }

public:
	bool signal() override
	{
		PTRACE("Shutdown signal received for %s",comm->getResourceName().c_str());
		_shutdown = true;
		comm->forceDisconnectSharedMemory();
		return true;
	}
};


static int _usage( const char* argv0 )
{
	PLogInit( new PLogFile( 0, PLogFile::alsoToStdout | PLogFile::catchPErrors | PLogFile::catchSystemDbgOutput /* no thread id */ ) );
	PLog( "%s: <resource> <name> <logFileName> [<initMsgEnvVarName>]", argv0 );
	return -1;
}

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


static void _hexdec( PString &p, const char *src, const int sz )
{
	for( int i=0; i < sz ; ++i )
	{
		char c0 = *src++;
		char c1 = *src++;
		int cc = ( _hex( c0 ) << 4 ) | _hex( c1 );
		p.append( BYTE(cc) );
	}
}

static void _loadInitMsg( CommMsgBody& initMsg, const char* envName, const char *data )
{
	PASSERT4( envName && !data || !envName && data);

	const char *env = 0;
	if( data )
		env = data;
	else
		env = getenv( envName );

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

//****************************************************************************

#if 1

class _CommLocalAppChildSharedMemInterface : public _CommInterface//, public _CommTimerInterface
{
private:
    friend class _CommLocalAppSharedMemManager;

	_CommLocalAppSharedMemInterface* parent;
	int mux;
	UINT32 muxChannelId;

public:
	_CommLocalAppChildSharedMemInterface( _CommLocalAppSharedMemInterface& parent_, int mux_ )
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
	void _internalPost(_CommMsg& m ) override
	{
		parent->_internalPostFromChild(mux, muxChannelId,m);
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
		return parent->_CommLocalAppSharedMemInterface::_getInQueueSize();
	}
	int _getOutQueueSize() const override
	{
		return parent->_CommLocalAppSharedMemInterface::_getOutQueueSize();
	}

	UINT64 _getWaitInQueueTime(UINT64 *readTime , UINT64 *writeTime ) override
	{
		return parent->_CommLocalAppSharedMemInterface::_getWaitInQueueTime(readTime , writeTime );
	}
	const char * _getIdent() const override
	{
		return parent->_getIdent();
	}


};

class _CommLocalAppSharedMemManager
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
	/* new */ _CommLocalAppChildSharedMemInterface* inters[ _COMM_MAX_LOCALAPP_OBJECTS ];
	/* new */ CommServerObject* objects[ _COMM_MAX_LOCALAPP_OBJECTS ];
	UINT32 channelIds[ _COMM_MAX_LOCALAPP_OBJECTS ];

public:
	UINT32 nextChannelId;

public:
	_CommLocalAppSharedMemManager( CommServerObjectFactory& factory_, _CommLocalAppSharedMemInterface& inter, _CommTimer& timer_ );

	void _setCallback( Callback* callback_ )
	{
		callback = callback_;
	}
	_CommLocalAppChildSharedMemInterface& _getInterface0()
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
	bool _destroyObject( int idx );//returns true if object #0 (factory) is destroyed
	bool destroyObjectAndNotify(_CommTimer &timer, int idx );//returns true if object #0 (factory) is destroyed
	void shutdown();
	bool terminated();
	~_CommLocalAppSharedMemManager();

#if PTRACELVL >= 5
    ObjectMsgStats* _getMsgStats(int i)
    {
        return (inters[0] ? &inters[0]->parent->_getMsgStats(i) : nullptr);
    }

    const char* _getName(int i) const
    {
        PASSERT3(i >= 0 && i < _COMM_MAX_LOCALAPP_OBJECTS);
        return names[i];
    }
#endif
};

_CommLocalAppSharedMemManager::_CommLocalAppSharedMemManager( CommServerObjectFactory& factory_, _CommLocalAppSharedMemInterface& inter, _CommTimer& timer_ )
: factory( factory_ ),
timer( timer_ )
{
	callback = 0;
	nextChannelId = RANDOMTIME();
	for( int i=0; i < _COMM_MAX_LOCALAPP_OBJECTS ; ++i )
	{
		inters[ i ] = new _CommLocalAppChildSharedMemInterface( inter, i );
		objects[ i ] = 0;
		channelIds[ i ] = 0;
	}
}

bool _CommLocalAppSharedMemManager::createNewObject( const char* name, CommMsgBody& initMsg )
{
	int idx = -1;
	int i;
	for( i=0; i < _COMM_MAX_LOCALAPP_OBJECTS ; ++i )
		if( objects[ i ] == 0 )
		{
			idx = i;
			break;
		}
	PASSERT3( idx >= 0 );

	PTRACE3( "_CommLocalAppSharedMemManager::createNewObject: object '%s' placed to slot %d", name, idx );

	CommServerObject* obj = factory.createServerObject( *inters[ idx ] );
	names[ idx ] = name;
	objects[ idx ] = obj;

	UINT32 channelId = nextChannelId++;
	//	obj->_init( /*idx, channelId,*/ *inters[ idx ] /* as _CommTimerInterface */ );
	inters[ i ]->_setChannelId( channelId );
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
	inters[ i ]->_post( m );
	return true;
}

bool _CommLocalAppSharedMemManager::_destroyObject( int idx )//returns true if object #0 (factory) is destroyed
{
	PASSERT3( idx >= 0 && idx < _COMM_MAX_LOCALAPP_OBJECTS );
	PASSERT3( objects[ idx ] != 0 );

	PTRACE3( "_CommLocalAppSharedMemManager::_destroyObject: object '%s' in slot %d destroyed", names[ idx ].c_str(), idx );

	delete objects[ idx ];
	objects[ idx ] = 0;
	names[ idx ] = ""; 
	channelIds[ idx ] = 0;
#if PTRACELVL >= 5
    auto stats = _getMsgStats(idx);
    if (stats)
    {
        stats->reset();
    }
#endif
	return idx == 0;
}

bool _CommLocalAppSharedMemManager::destroyObjectAndNotify(_CommTimer &timer, int idx )//returns true if object #0 (factory) is destroyed
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

void _CommLocalAppSharedMemManager::connected()
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

void _CommLocalAppSharedMemManager::shutdown()
{
	for( int i=0; i < _COMM_MAX_LOCALAPP_OBJECTS ; ++i )
		if( objects[ i ] )
			objects[ i ]->_safeShutdown();
}

bool _CommLocalAppSharedMemManager::terminated()
{
	for( int i=0; i < _COMM_MAX_LOCALAPP_OBJECTS ; ++i )
		if( objects[ i ] && objects[ i ]->_safeTerminated() == 0 )
			return false;
	return true;
}

_CommLocalAppSharedMemManager::~_CommLocalAppSharedMemManager()
{
	for( int i=_COMM_MAX_LOCALAPP_OBJECTS - 1; i >= 0 ; --i )
	{
		delete objects[ i ];
		delete inters[ i ];
	}
}

class _CommLocalAppSharedMemSubFactory;
class _CommLocalAppSharedMemSubFactoryConnection : public CommClientConnection
{
private:
	_CommLocalAppSharedMemSubFactory* obj;

public:
	_CommLocalAppSharedMemSubFactoryConnection( CommClientGuardFactory& guardFactory )
		: CommClientConnection( guardFactory )
	{
		obj = 0;
	}
	void init( _CommLocalAppSharedMemSubFactory* obj_ )
	{
		obj = obj_;
	}

	void connected() override;
	void processMessage( UINT32 msgId, const CommMsgBody& body ) override;
};

class _CommLocalAppSharedMemSubFactory : public CommServerObject, public _CommLocalAppSharedMemManager::Callback
{
private:
	PString name;
	_CommLocalAppSharedMemManager& mgr;
	CommClientConnectionPool cliPool;
	_CommLocalAppSharedMemSubFactoryConnection conn;
	vector< PString > objects;
	static CommClientNullGuardFactory clientGuardFactory;
	_CommRunToSubFactoryInterface & run2subfactory;

public:
	_CommLocalAppSharedMemSubFactory( _CommInterface& inter_, _CommLocalAppSharedMemManager& mgr_, _CommRunToSubFactoryInterface & run2subfactory_  );
	/* virtual - CommServerObject */ void init( CommMsgBody& body ) override;
	/* virtual - CommServerObject */ void processMessage( _CommMsg& msg ) override;
	/* virtual - Callback */ void objectDestroyed( const char* name ) override;

	void connected();
	void processMessage( UINT32 msgId, const CommMsgBody& body );
	void updateSEHNumber(INT32 num,INT32 numAsserts,INT32 numMemory);
};

/* static */ CommClientNullGuardFactory _CommLocalAppSharedMemSubFactory::clientGuardFactory;

_CommLocalAppSharedMemSubFactory::_CommLocalAppSharedMemSubFactory( _CommInterface& inter_, _CommLocalAppSharedMemManager& mgr_,_CommRunToSubFactoryInterface & run2subfactory_  )
: CommServerObject( inter_ ),
mgr( mgr_ ),
cliPool( inter_ ),
conn( clientGuardFactory ),
run2subfactory(run2subfactory_)
{
	mgr._setCallback( this );
	conn.init( this );
}

/* virtual - CommServerObject */ void _CommLocalAppSharedMemSubFactory::init( CommMsgBody& body )
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

/* virtual - CommServerObject */ void _CommLocalAppSharedMemSubFactory::processMessage( _CommMsg& msg )
{
	cliPool._processMessage( msg );
}

/* virtual */ void _CommLocalAppSharedMemSubFactoryConnection::connected()
{
	obj->connected();
}

/* virtual */ void _CommLocalAppSharedMemSubFactoryConnection::processMessage( UINT32 msgId, const CommMsgBody& body )
{
	obj->processMessage( msgId, body );
}


void _CommLocalAppSharedMemSubFactory::connected()
{
	CommMsgBody body;
	body.composeString( name );
	int n = objects.size();
	for( int i=0; i < n ; ++i )
		body.composeString( objects[ i ] );
	body.composeString( "" );
	conn.post( _COMMAPPFACTORY_SUBFACTORY_CONNECTED, body );
}

void _CommLocalAppSharedMemSubFactory::updateSEHNumber(INT32 num,INT32 numAsserts,INT32 numMemory)
{
	CommMsgBody body;
	body.composeUINT32(1234)
		.composeINT32(num)
		.composeINT32(numAsserts)
		.composeINT32(numMemory);
	conn.post( _COMMAPPFACTORY_SUBFACTORY_SEH_NUMBER_UPDATE, body );
}


void _CommLocalAppSharedMemSubFactory::processMessage( UINT32 msgId, const CommMsgBody& body )
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
		PLog( "_CommLocalAppSharedMemSubFactory::processMessage: unknown msgId=%X", msgId );
		break;
	}
}

/* virtual - Callback */ void _CommLocalAppSharedMemSubFactory::objectDestroyed( const char* objectName )
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
	PLog( "_CommLocalAppSharedMemSubFactory::objectDestroyed(): object '%s' not found", objectName );
}

static void _runMulti_shared( CommServerObjectFactory& factory, int port, const char* name, CommMsgBody& initMsg, const char * pyrHandler, UINT32 pPid, const char * ident )
{
	try
	{
		bool interfaceClosed=false;
		int numberOfSEH = 0;
		int numberOfAsrt = 0;
		int numberOfMemoryFailure = 0;
		_CommLocalAppSharedMemInterface inter( port, ident );
		inter.enableMux();
		_CommLocalKillHandler killHandler( inter.comm, pyrHandler, pPid );
		_CommTimer timer;
		inter._setTimer( timer );
		_CommRunToSubFactoryInterface run2subfactory(timer);
		_CommLocalAppSharedMemManager mgr( factory, inter, timer );

		PLog( "CommMultiLocalAppSharedMem:: Creating Factory Server '%s'", name );
		_CommLocalAppSharedMemSubFactory* subFactory = new _CommLocalAppSharedMemSubFactory( mgr._getInterface0() /* as _CommInterface */, mgr, run2subfactory );
		UINT32 channelId = mgr.nextChannelId++;
		mgr._setObject0( name, channelId, subFactory );
		//		subFactory->_init( /*0, channelId,*/ mgr._getInterface0() /* as _CommTimerInterface */ );
		bool destroy = subFactory->_safeInit( initMsg );
		if( destroy )
			return;

#if PTRACELVL >= 5
        UINT32 numMsgsRecvd = 0, numTimerMsgsProcessed = 0;
        size_t now = SYSTEMTICKS();
#endif
		
        for(;;)
		{
#if PTRACELVL >= 5
            if (SYSTEMTICKS() - now >= 10000)
            {
                PLog("Msg count report: numMsgsRecvd = %u, numTimerMsgsProcessed = %u", numMsgsRecvd, numTimerMsgsProcessed);
                PLog("Per-object stats:");
                for (int i = 0; i < _COMM_MAX_LOCALAPP_OBJECTS; ++i)
                {
                    auto objMsgStats = mgr._getMsgStats(i);
                    const char* objName = mgr._getName(i);
                    if (objMsgStats && objName && *objName)
                    {
                        PLog("%d. name = %s numMsgsRecvd = %u numMsgsSent = %u", i, objName, objMsgStats->numMsgsRecvd, objMsgStats->numMsgsSent);
                    }
                }
                now = SYSTEMTICKS();
            }
#endif

			if( killHandler.shutdown() )
			{
				PLog( "CommLocalSharedMemApp: Kill signal received - exiting" );

				mgr.shutdown();
				bool Ok = _CommWaitUntilTerminated( mgr, 10 );
				if( Ok )
					PLog( "CommLocalSharedMemApp: objects terminated" );
				else
				{
					PLog( "CommLocalSharedMemApp: objects not terminated - exiting anyway" );
					FATALEXIT();
				}

				return;
			}
			else if (interfaceClosed)
			{
				PLog( "CommLocalSharedMemApp: Interface closed unexpectedly, exit application..." );
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
#if PTRACELVL >= 5
                        ++numTimerMsgsProcessed;
#endif
						_CommMsg msg;
						UINT32 timerHandle = timer.pop( msg );

						int objIdx = msg.internal.slot;
						UINT32 channelId;
						CommServerObject* obj = mgr._getObject( channelId, objIdx );
						if( !obj )
						{
							PTRACE3( "LocalMultiSharedMemApp[ %d ]: timer message for non-existing object - ignored", objIdx );
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
#if PTRACELVL >= 5
                    ++numMsgsRecvd;
#endif
					_CommMsg msg;
					int mux;
					UINT32 muxChannelId;
					inter.extractMsg( mux, muxChannelId, msg );
					int objIdx = mux;
					UINT32 channelId;
					CommServerObject* obj = mgr._getObject( channelId, objIdx );
					if( !obj )
					{
						PTRACE3( "LocalMultiSharedMemApp[ %d ]: message for non-existing object - ignored", objIdx );
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
									PLog( "CommLocalSharedMemApp: ServerObject: Unable to register name with CommRoutingTable (err=%02X) - object destroyed", err );
									if( mgr.destroyObjectAndNotify(	timer, objIdx ) )
										return;
									inter.postAll();
									break;
								}

								PLog( "CommLocalSharedMemApp: ServerObject registered" );

								//inform server object about connection
								_CommMsg m( _COMM_MSGTYPE_PHYSICAL_CONNECT, _COMM_MSGPRIORITY_INVALID );
								bool destroy = obj->_safeProcessMessage( m );
								inter.postAll();
								if( destroy && mgr.destroyObjectAndNotify( timer, objIdx ) )
									return;
								inter.postAll();
							}
							break;

                        case _COMM_MSGTYPE_DESTROY_SERVEROBJECT:
                            {
                                UINT32 reqId;
                                CommMsgParser parser(msg.body);
                                parser.parseUINT32(reqId);

                                PLog("CommLocalSharedMemApp: ServerObject: object destroyed by CommRoutingTable");

                                if (mgr.destroyObjectAndNotify(timer, objIdx))
                                    return;
                                inter.postAll();
                            }
                            break;

						default:
							{
#if PTRACELVL >= 5
                                auto objMsgStats = mgr._getMsgStats(objIdx);
                                PASSERT(objMsgStats);
                                ++objMsgStats->numMsgsRecvd;
#endif
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
				PLog( "LocalSharedMemApp::runMulti_shared() exception - interface closed: '%s'", err.why() );
				inter.error();
				interfaceClosed=true;
			}
		}//forever
	}
	catch( PError& err )
	{
		PLog( "LocalSharedMemApp::runMulti_shared() last hope exception caught: '%s'", err.why() );
	}
	catch(  exception& exc  )
	{
		PLog( "LocalSharedMemApp::runMulti_shared() last hope STD exception caught: '%s'", exc.what() );
	}

	catch( ... )
	{
		PLog( "LocalSharedMemApp::runMulti_shared() unknown last hope exception caught" );
	}
}

static void _CommMultiLocalAppSharedMemMain( int argc, char** argv, CommServerObjectFactory& factory, const char* logMsg )
{
#if PTRACELVL > 3
	PLogInit( new PLogFile( argv[ 3 ], PLogFile::printThreadId | PLogFile::perDate | PLogFile::catchPErrors | PLogFile::catchSystemDbgOutput  ) );
#else
	PLogInit( new PLogFile( argv[ 3 ], /*PLogFile::printProcessId |*/ PLogFile::perDate | PLogFile::catchPErrors | PLogFile::catchSystemDbgOutput /* no thread id */ ) );
#endif

	PLog( "CommMultiLocalSharedMemApp: Starting %s...", logMsg ? logMsg : "" );

	int port = atoi( argv[ 1 ] );
	const char* name = argv[ 2 ];
//	int restoreFlag = atoi(argv[ 4 ]);
	const char * ident="";
	CommMsgBody initMsg;
	if( argc >= 6 )
		_loadInitMsg( initMsg, argv[ 5 ], 0 );
	if( argc >= 7 )
	{
		PLog("Identification = '%s'",argv[ 6 ]);
		ident = argv[ 6 ];
	}

	PLog( "CommMultiLocalSharedMemApp: Running..." );
	_runMulti_shared( factory, port, name, initMsg, 0, 0, ident );
	PLog( "CommMultiLocalSharedMemApp: Exiting..." );
}

static void _CommMultiLocalAppSharedMemMainElevated( int argc, char** argv, CommServerObjectFactory& factory, const char* logMsg, const PStringEncoding* fileNameEncoding )
{
	PString logPath;
	_hexdec( logPath, argv[ 4 ], strlen( argv[ 4 ] )/2 );

	PLogInit( new PLogFile( logPath, /*PLogFile::printProcessId |*/ PLogFile::perDateLimited | PLogFile::catchPErrors | PLogFile::catchSystemDbgOutput /* no thread id */, fileNameEncoding ) );
	PLog( "CommMultiLocalSharedMemApp: Starting %s...", logMsg ? logMsg : "" );

	int port = atoi( argv[ 2 ] );
	const char* name = argv[ 3 ];
	const char * ident="";
//	int restoreFlag = atoi(argv[ 5 ]);
	CommMsgBody initMsg;
	_loadInitMsg( initMsg, 0, argv[ 6 ] );
	UINT32 pPid = strtoul(argv[7],0,10);

	if( argc >= 9 )
	{
		PLog("Identification = '%s'",argv[ 8 ]);
		ident = argv[ 8 ];
	}


	PLog( "CommMultiLocalSharedMemApp: Running..." );
	_runMulti_shared( factory, port, name, initMsg, argv[1], pPid, ident );
	PLog( "CommMultiLocalSharedMemApp: Exiting..." );
}

//****************************************************************************

class _CommMultiLocalSharedMemApp : public PErrorHandler//to create safety wrapper around _CommLocalAppMain
{
private:
	int argc;
	char** argv;
	CommServerObjectFactory& factory;
	const char* logMsg;

public:
	_CommMultiLocalSharedMemApp( int argc_, char** argv_, CommServerObjectFactory& factory_, const char* logMsg_ )
		: factory( factory_ )
	{
		argc = argc_;
		argv = argv_;
		logMsg = logMsg_;
	}
	void run() override;
};

/* virtual */ void _CommMultiLocalSharedMemApp::run()
{
	_CommMultiLocalAppSharedMemMain( argc, argv, factory, logMsg );
}

class _CommMultiLocalSharedMemAppElevated : public PErrorHandler//to create safety wrapper around _CommLocalAppMain
{
private:
	int argc;
	char** argv;
	CommServerObjectFactory& factory;
	const char* logMsg;
	const PStringEncoding* fileNameEncoding;

public:
	_CommMultiLocalSharedMemAppElevated( int argc_, char** argv_, CommServerObjectFactory& factory_, const char* logMsg_, const PStringEncoding* fileNameEncoding_ )
		: factory( factory_ )
	{
		argc = argc_;
		argv = argv_;
		logMsg = logMsg_;
		fileNameEncoding = fileNameEncoding_;
	}
	void run() override
	{
		_CommMultiLocalAppSharedMemMainElevated( argc, argv, factory, logMsg, fileNameEncoding );
	}
};

int CommMultiLocalAppSharedMemMain( int argc, char** argv, CommServerObjectFactory& factory, const char* logMsg )
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

	_CommMultiLocalSharedMemApp app( argc, argv, factory, logMsg );
	app.execute();
	return 0;
}

int CommLocalAppSharedMemMain( int argc, char** argv, CommServerObjectFactory& factory, const char* logMsg )
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

	_CommMultiLocalSharedMemApp app( argc, argv, factory, logMsg );
	app.execute();
	return 0;
}

int CommLocalAppSharedMemMainElevated( int argc, char** argv, CommServerObjectFactory& factory, const char* logMsg, const PStringEncoding* fileNameEncoding )
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

	if( argc < 7 )
		return _usage( argv[ 0 ] );

	_CommMultiLocalSharedMemAppElevated app( argc, argv, factory, logMsg, fileNameEncoding );
	app.execute();
	return 0;
}

#endif

