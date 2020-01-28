#include "ppinclude.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "commrtable.h"
#include "commsrv.h"
#include "commtmlocal.h"

#define _MAXLOCALTHREADOBJECTS 32

class _CommLocalThreadInterface : public _CommInterface//, public _CommTimerInterface
{
private:
	CommRoutingTable* routingTable;
	_CommTimer* timer;
	_CommPhysicalConnection* physConn;
	CommMsgTimeControlQueue* q;

public:
	_CommLocalThreadInterface()
	{
		routingTable = 0;
		physConn = 0;
		q = 0;
	}
	void _construct( CommRoutingTable& routingTable_, _CommTimer& timer_, CommMsgTimeControlQueue& q_ )
	{
		routingTable = &routingTable_;
		timer = &timer_;
		q = &q_;
	}
	void _init( _CommPhysicalConnection* physConn_ )
	{
		physConn = physConn_;
	}
	void _deinit()
	{
		physConn = 0;
	}

public://_CommInterface
	void _internalPost(_CommMsg& m ) override
	{
		m.internal.channelId = physConn->channelId;
		m.internal.slot = physConn->slot;
		q->push(m);
	}

	void _post( _CommMsg& msg ) override
	{
		routingTable->routeIncomingMessage( physConn, 0, 0, msg );
	}
	UINT32 _postClientTimer( _CommMsg& msg, int msec ) override
	{
		msg.internal.channelId = physConn->channelId;
		msg.internal.slot = physConn->slot;
		return timer->addMsg( msg, msec );
	}
	UINT32 _addServerTimer( UINT32 msgId, CommMsgBody& body, int msec ) override
	{
		_CommMsg msg( _COMM_MSGTYPE_NONE, _COMM_MSGPRIORITY_INVALID );
		msg.body.moveFrom( body );
		msg.internal.channelId = physConn->channelId;
		msg.internal.slot = physConn->slot;
		msg.internal.patch0 = msgId;
		return timer->addMsg( msg, msec );
	}
	bool _delServerTimer( UINT32 handle ) override
	{
		return timer->delMsg( handle );
	}
	int _getInQueueSize() const override
	{
		return q ? q->size() : -1;
	}
	int _getOutQueueSize() const override
	{
		return 0;
	}

	const char * _getIdent() const override
	{
		return routingTable->_getIdent();
	}
};

struct _CommLocalThreadObject
{
	PString name;
	bool nameRegistered;
	_CommPhysicalConnection* physConn;
	_CommLocalThreadInterface inter;
	CommServerObject* obj;
	int channelId;
};

class _CommLocalThreadObjects
{
private:
	_CommLocalThreadObject servers[ _MAXLOCALTHREADOBJECTS ];

public:
	_CommLocalThreadObjects( CommRoutingTable& routingTable, _CommTimer& timer, CommMsgTimeControlQueue& q );
	void shutdown();
	bool terminated();
	~_CommLocalThreadObjects();

public:
	_CommLocalThreadObject& operator[] ( int i )
	{
		PASSERT3( i >= 0 && i < _MAXLOCALTHREADOBJECTS );
		return servers[ i ];
	}
};

class _CommLocalThread : public _CommManageableThread
{
private:
	friend class CommLocalThreadManager;
	const CommServerObjectFactory* factory;
	CommLocalThreadManager* mgr;
	int mgrIdx;

public:
	_CommLocalThread( CommLocalThreadManager& mgr_, int mgrIdx_, const CommServerObjectFactory& factory_ );
	void run() override;
	void pushForceDisconnect( int slot, UINT32 channelId );

private:
	bool _destroyServerObject( _CommTimer& timer, _CommLocalThreadObjects& servers, int slot );//returns mustExit
};

_CommLocalThreadObjects::_CommLocalThreadObjects( CommRoutingTable& routingTable, _CommTimer& timer, CommMsgTimeControlQueue& q )
{
	for( int i=0; i < _MAXLOCALTHREADOBJECTS ; ++i )
	{
		servers[ i ].physConn = 0;
		servers[ i ].inter._construct( routingTable, timer, q );
		servers[ i ].obj = 0;
	}
}

void _CommLocalThreadObjects::shutdown()
{
	for( int i=0; i < _MAXLOCALTHREADOBJECTS ; ++i )
		if( servers[ i ].obj )
			servers[ i ].obj->_safeShutdown();
}

bool _CommLocalThreadObjects::terminated()
{
	for( int i=0; i < _MAXLOCALTHREADOBJECTS ; ++i )
		if( servers[ i ].obj && servers[ i ].obj->_safeTerminated() == 0 )
			return false;
	return true;
}

_CommLocalThreadObjects::~_CommLocalThreadObjects()
{
	for( int i=_MAXLOCALTHREADOBJECTS - 1; i >= 0; --i )
		delete servers[ i ].obj;
}

_CommLocalThread::_CommLocalThread( CommLocalThreadManager& mgr_, int mgrIdx_, const CommServerObjectFactory& factory_ )
: mgr( &mgr_ ),
factory( &factory_ )
{
	mgrIdx = mgrIdx_;
}

void _CommLocalThread::pushForceDisconnect( int slot, UINT32 channelId )
{
	_CommMsg m( _COMM_MSGTYPE_NONE, _COMM_MSGPRIORITIES - 1 );
	m.internal.slot = slot;
	m.internal.channelId = channelId;
	q.push( m );
}

bool _CommLocalThread::_destroyServerObject( _CommTimer& timer, _CommLocalThreadObjects& servers, int slot )
{
	_CommLocalThreadObject& server = servers[ slot ];
	timer.delAllForSlot( slot );
	server.inter._deinit();
	mgr->routingTable.physicallyDisconnected( server.physConn );
	server.physConn = 0;
    server.channelId = 0;
	delete server.obj;
	server.obj = 0;

	int saveMgrIdx = mgrIdx;
	if( mgr->_freeObject( mgrIdx ) )
	{//no thread operations allowed after _freeObject returned true
		PTRACE3( "Exiting LocalThread #%d", saveMgrIdx );
		//exit(); - neither needed nor allowed as this is already deleted
		return true;
	}
	return false;
}

/* virtual */ void _CommLocalThread::run()
{
	try
	{
		_CommTimer timer;
		_CommLocalThreadObjects servers( mgr->routingTable, timer, q );

		for(;;)
		{
			int timeout;
			for(;;)
			{
				timeout = timer.msecUntilFirst();
				if( timeout == 0 )
				{
					_CommMsg msg;
					UINT32 timerHandle = timer.pop( msg );
					int slot = msg.internal.slot;

					if( msg.internal.channelId != servers[ slot ].channelId )
					{// Critical check. Protects from using wrong channel
						PLog( "LocalThread[ %d ]: timer message for obsolete channelId=%08X - ignored", mgrIdx, msg.internal.channelId );
						continue;//forever timer
					}

					_CommLocalThreadObject& server = servers[ slot ];

					bool destroy = false;
					if( msg.type == _COMM_MSGTYPE_NONE )
						destroy = server.obj->_safeProcessTimerMessage( timerHandle, msg.internal.patch0, msg.body );
					else
					{
						PASSERT3( msg.type == _COMM_MSGTYPE_CONNPOOL_TIMER || msg.type == _COMM_MSGTYPE_SUBSCRPOOL_TIMER ||
							msg.type == _COMM_MSGTYPE_CLICONNPOOL_TIMER_CONNECTION_EX || msg.type == _COMM_MSGTYPE_SRVCONNPOOL_TIMER_CONNECTION_EX);

						destroy = server.obj->_safeProcessMessage( msg );
					}

					if( destroy )
					{
						bool mustExit = _destroyServerObject( timer, servers, slot );
						if( mustExit )
							return;
					}
				}
				else
					break;
			}

			int w;
			_CommMsg msg;
			if( timeout < 0 )
				w = q.waitPop( msg );
			else
				w = q.waitPop( msg, timeout + 1 );
			switch( w )
			{
			case CommMsgQueue::waitPopOk:
				{
					switch( msg.type )
					{
					case _COMM_MSGTYPE_NONE://forceDisconnect
						{
							int slot = msg.internal.slot;

							PLog( "LocalThread[ %d ]: ServerObject( %d ) - forceDisconnect received", mgrIdx, slot );
							PASSERT3( slot >= 0 && slot < _MAXLOCALTHREADOBJECTS );
							if( msg.internal.channelId != servers[ slot ].channelId )
							{// Critical check. Protects from closing wrong channel
								PLog( "LocalThread[ %d ]: forceDisconnect for obsolete channelId=%08X - ignored", mgrIdx, msg.internal.channelId );
								break;
							}

							_CommLocalThreadObject& server = servers[ slot ];
							if( !server.nameRegistered )//no name yet and forceDisconnect? - destroy!
							{
								PLog( "LocalThread[ %d ]: ServerObject( %d ) has no registered name yet and forceDisconnect called - will be destroyed", mgrIdx, slot );
								bool mustExit = _destroyServerObject( timer, servers, slot );
								if( mustExit )
									return;
								break;//switch
							}

							_CommMsg m( _COMM_MSGTYPE_PHYSICAL_DISCONNECT, _COMM_MSGPRIORITY_INVALID );
							bool destroy = server.obj->_safeProcessMessage( m );
							if( destroy )
							{
								bool mustExit = _destroyServerObject( timer, servers, slot );
								if( mustExit )
									return;
								else
									break;
							}

							PLog( "LocalThread[ %d ]: ServerObject( %d ) - forceDisconnect: disconnecting...", mgrIdx, slot );
							server.inter._deinit();
							mgr->routingTable.physicallyDisconnected( server.physConn );
							server.physConn = 0;

							PLog( "LocalThread[ %d ]: ServerObject( %d ) - forceDisconnect: reconnecting...", mgrIdx, slot );
							server.physConn = mgr->routingTable.createIncomingPhysicalConnection( "pyrlocal://", mgr );
							server.channelId = mgr->nextChannelId();
							mgr->routingTable.physicallyConnected( server.physConn, mgrIdx, slot, server.channelId, 3 );
							server.inter._init( server.physConn );

							//inform routing table about server object connected
							_CommMsg m2( _COMM_MSGTYPE_REGISTER_SERVEROBJECT, _COMM_MSGPRIORITIES - 1 );
							m2.body.composeUINT32( server.channelId ).composeString( server.name );
							server.inter._post( m2 );
							server.nameRegistered = false;
						}
						break;

					case _COMM_MSGTYPE_LOCAL_CREATESERVEROBJECT:
						{
							const char* name;
							CommMsgBody initMsg;
							CommMsgParser parser( msg.body );
							parser.parseString( name ).parseMsgBody( initMsg );
							PLog( "LocalThread[ %d ]: Creating Server '%s'", mgrIdx, name );

							int slot = -1;
							for( int i=0; i < _MAXLOCALTHREADOBJECTS ; ++i )
								if( servers[ i ].physConn == 0 )
								{
									slot = i;
									break;
								}
							PASSERT3( slot >= 0 );
							_CommLocalThreadObject& server = servers[ slot ];

							server.physConn = mgr->routingTable.createIncomingPhysicalConnection( "pyrlocal://", mgr );
							server.channelId = mgr->nextChannelId();
							mgr->routingTable.physicallyConnected( server.physConn, mgrIdx, slot, server.channelId, 3 );
							server.inter._init( server.physConn );
							server.obj = factory->createServerObject( server.inter );
							//							server.obj->_init( /*slot, server.channelId,*/ server.inter /* as _CommTimerInterface */ );

							bool destroy = server.obj->_safeInit( initMsg );
							if( destroy )
							{
								bool mustExit = _destroyServerObject( timer, servers, slot );
								if( mustExit )
									return;
								else
									break;
							}

							//inform routing table about server object connected
							_CommMsg m( _COMM_MSGTYPE_REGISTER_SERVEROBJECT, _COMM_MSGPRIORITIES - 1 );
							m.body.composeUINT32( server.channelId ).composeString( name );
							server.inter._post( m );
							server.nameRegistered = false;
							server.name = name;

							PLog( "LocalThread[ %d ]: ServerObject '%s' created in slot %d", mgrIdx, name, slot );
						}
						break;

					case _COMM_MSGTYPE_REGISTER_SERVEROBJECT_REPLY:
						{
							BYTE err;
							UINT32 reqId;
							CommMsgParser parser( msg.body );
							parser.parseUINT32( reqId ).parseBYTE( err );

							int slot = -1;
							for( int i=0; i < _MAXLOCALTHREADOBJECTS ; ++i )
								if( servers[ i ].channelId == reqId )
								{
									slot = i;
									break;//for( i )
								}
							if( slot < 0 )
							{
								PLog( "LocalThread[ %d ]: _COMM_MSGTYPE_REGISTER_SERVEROBJECT_REPLY with unknown id=%08X - ignored", mgrIdx, reqId );
								break;//switch
							}

							_CommLocalThreadObject& server = servers[ slot ];
							if( err )
							{
								PLog( "LocalThread[ %d ]: ServerObject( %d ): Unable to register name with CommRoutingTable (err=%02X) - object destroyed", mgrIdx, slot, err );
								timer.delAllForSlot( slot );
								bool mustExit = _destroyServerObject( timer, servers, slot );
								if( mustExit )
									return;
								break;//switch
							}

							PLog( "LocalThread[ %d ]: ServerObject( %d ) registered", mgrIdx, slot );

							server.nameRegistered = true;
							//inform server object about connection
							_CommMsg m( _COMM_MSGTYPE_PHYSICAL_CONNECT, _COMM_MSGPRIORITY_INVALID );
							bool destroy = server.obj->_safeProcessMessage( m );
							if( destroy )
							{
								bool mustExit = _destroyServerObject( timer, servers, slot );
								if( mustExit )
									return;
								//else
								//	break;
							}
						}
						break;
					case  _COMM_MSGTYPE_SERVICEREQUEST:
						{
							CommMsgParser parser( msg.body );
							UINT32 id;
							parser.parseUINT32(id);
							if (id == _SERVICE_REQUEST_DUMP)
							{
								rundump();
								break;
							}
						}
						break;

					default:
						{
							int slot = msg.internal.slot;

							PASSERT3( slot >= 0 && slot < _MAXLOCALTHREADOBJECTS );
							if( msg.internal.channelId != servers[ slot ].channelId )
							{// Critical check. Protects from dispatching the message to the wrong channel
								// when connection was deleted and then another connection created in the same slot.
								// Also protects from using failed connection: in case of close
								// servers[ slot ].channelId must be changed
								PTRACE3( "LocalThread[ %d ]: message to obsolete channelId=%08X - ignored", mgrIdx, msg.internal.channelId );
								break;
							}

							_CommLocalThreadObject& server = servers[ slot ];
							bool destroy;
							if( msg.internal.flags & _CommMsg::Patch8 )
							{
								_CommMsg m;
								CommThreadManager::_copyPatchMsg( m, msg );
								destroy = server.obj->_safeProcessMessage( m );
							}
							else
								destroy = server.obj->_safeProcessMessage( msg );
							if( destroy )
							{
								bool mustExit = _destroyServerObject( timer, servers, slot );
								if( mustExit )
									return;
								else
									break;
							}
						}
						break;
					}
				}
				break;

			case CommMsgQueue::waitPopShutdown:
				PTRACE3( "LocalThread #%d shutdown - exiting", mgrIdx );

				servers.shutdown();
				_CommWaitUntilTerminated( servers );
				PLog( "LocalThread: objects terminated" );

				//					exit();
				return;

			case CommMsgQueue::waitPopTimeout:
				//do nothing - action will be done in the beginning of the forever cycle
				break;
			}
		}
	}
	catch( PError& err )
	{
		PLog( "LocalThread[ %d ] exception: %s", mgrIdx, err.why() );
	}
	catch(  exception& exc  )
	{
		PLogError( "LocalThread[ %d ] last hope STD exception catch: %s",  mgrIdx,  exc.what() );
		exit(true);
	}
	catch( ... )
	{
		PLogError( "LocalThread[ %d ] last hope ... catch", mgrIdx );
		exit(true);
	}

	//	exit();
}

//****************************************************************************

CommLocalThreadManager::CommLocalThreadManager( CommRoutingTable& routingTable_ )
: CommThreadManager( routingTable_ )
{
}

void CommLocalThreadManager::registerServerObjectFactory( const char* typeName, CommServerObjectFactory& factory, int maxObjectsPerThread )
{
	PWLock lock( cs );
	_Factory f;
	f.factory = &factory;
	f.maxObjectsPerThread = maxObjectsPerThread;
	factories.insert( _FactoriesType::value_type( typeName, f ) );
}

void CommLocalThreadManager::forceDisconnect( _CommPhysicalConnection* physConn )
{
	PWLock lock( cs );
	Thread& th = threads[ physConn->thread ];
	_CommLocalThread* thread = static_cast< _CommLocalThread* >( th.thread );//safe because th.thread is always created by CommLocalThreadManager as _CommLocalThread
	thread->pushForceDisconnect( physConn->slot, physConn->channelId );
}

bool CommLocalThreadManager::createServerObject( const char* type, const char* name, const CommMsgBody* initMsg )
{
	_CommMsg m( _COMM_MSGTYPE_LOCAL_CREATESERVEROBJECT, _COMM_MSGPRIORITY_INITCONNECT );
	m.body.composeString( name );
	if( initMsg )
		m.body.composeMsgBody( *initMsg );
	else
	{
		CommMsgBody dummy;
		m.body.composeMsgBody( dummy );
	}

	PWLock lock( cs );
	_FactoriesType::iterator found = factories.find( type );
	if( found == factories.end() )
		return false;

	_Factory f = (*found).second;

	int threadIdx = -1;
	int n = threads.size();
	for( int i=0; i < n ; ++i )
	{
		Thread& thread = threads[ i ];
		if( thread.thread != 0 )
		{
			_CommLocalThread* localThread = (_CommLocalThread*)thread.thread;//typecast is safe - always a _CommLocalThread*
			if( localThread->factory == f.factory && thread.slotsLeft > 0 )
			{
				threadIdx = i;
				break;//for( i )
			}
		}
	}

	if( threadIdx < 0 )
	{
		threadIdx = _allocThread();
		Thread& thread = threads[ threadIdx ];
		thread.thread = new _CommLocalThread( *this, threadIdx, (*f.factory) );
		thread.maxSlots = thread.slotsLeft = f.maxObjectsPerThread;
		thread.thread->start();
		PTRACE3( "Started Local thread #%d - type '%s'", threadIdx, type );
	}

	Thread& thread = threads[ threadIdx ];
	PASSERT3( thread.slotsLeft > 0 );
	--thread.slotsLeft;
	PTRACE3( "Sending CREATESERVER msg to local thread #%d", threadIdx );
	thread.thread->push( m );
	return true;
}

bool CommLocalThreadManager::_freeObject( int threadIdx )//no thread operations allowed after freeSocket returned true//no thread operations allowed after _freeObject returned true
{
	PWLock lock( cs );
	Thread& thread = threads[ threadIdx ];
	if( ++thread.slotsLeft != thread.maxSlots )
		return false;

	//{+++ to preserve some threads for the future use - return false
	//}

	delete thread.thread;
	thread.thread = 0;
	return true;
}
