#include "ppinclude.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "commrtable.h"
#include "commsrv.h"
#include "commconcli.h"

//****************************************************************************

class _CommConsoleClientThread : public _CommManageableThread
{
private:
	friend class CommConsoleClientThreadManager;
	UINT32 channelId;
	CommConsoleClientObject* obj;
	CommConsoleClientThreadManager* mgr;

public:
	_CommConsoleClientThread( CommConsoleClientThreadManager& mgr_, CommConsoleClientObject* obj_ )
	{
		mgr = &mgr_;
		obj = obj_;
	}
	void run() override;
};

class _CommConsoleClientTimerThread : public _CommTimerThread
{
private:
	CommConsoleClientThreadManager* mgr;

public:
	_CommConsoleClientTimerThread( CommConsoleClientThreadManager& mgr_ )
	{
		mgr = &mgr_;
	}

public://overridden
	void timer( _CommMsg& msg ) override;
};

//****************************************************************************

CommConsoleClientObject::CommConsoleClientObject( CommConsoleClientInterface& inter_ )
{
	inter = &inter_;
}

//****************************************************************************

CommConsoleClientInterface::CommConsoleClientInterface( CommConsoleClientThreadManager& mgr_ )
{
	mgr = &mgr_;
	physConn = 0;
}

void CommConsoleClientInterface::_init( _CommPhysicalConnection* physConn_ )
{
	physConn = physConn_;
}

/* virtual */ void CommConsoleClientInterface::_post( _CommMsg& msg )
{
	mgr->routingTable.routeIncomingMessage( physConn, 0, 0, msg );
}

/* virtual */ UINT32 CommConsoleClientInterface::_postClientTimer( _CommMsg& msg, int msec )
{
	return mgr->_postTimer( physConn, msg, msec );
}

//****************************************************************************

/* virtual */ void _CommConsoleClientTimerThread::timer( _CommMsg& msg )
{
	mgr->_timer( msg );
}

/* virtual */ void _CommConsoleClientThread::run()
{
	try
	{
		for(;;)
		{
			_CommMsg msg;
			int w;
			w = q.waitPop( msg );
			switch( w )
			{
			case CommMsgQueue::waitPopOk:
				{
					switch( msg.internal.slot )
					{
					case 0://regular msg
						{
							if( msg.internal.channelId != channelId )
							{
								PLog( "CommConsoleClientThread: message for obsolete channelId=%08X - ignored", msg.internal.channelId );
								break;//forever timer
							}

							//prepatched!
							try
							{
								PSIGRETURN()
									obj->processMessage( msg );
							}
							catch( PError& err )
							{
								PLog( "PError caught in ClientObject::processMessage(): %s", err.why() );
							}
							catch( ... )
							{
								PLog( "... caught in ClientObject::processMessage()" );
							}
						}
						break;

					case 1://cmd
						{
							CommMsgParser parser( msg.body );
							const char* cmd;
							parser.parseString( cmd );
							try
							{
								PSIGRETURN()
									obj->processCommand( cmd );
							}
							catch( PError& err )
							{
								PLog( "PError caught in ClientObject::processCommand('%s'): %s", cmd, err.why() );
							}
							catch( ... )
							{
								PLog( "... caught in ClientObject::processCommand('%s')", cmd );
							}
						}
						break;
					}
				}
				break;

			case CommMsgQueue::waitPopShutdown:
				PTRACE3( "ConsoleClientThread shutdown - exiting" );
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
		PLog( "ConsoleClientThread exception: %s", err.why() );
	}
	//	exit();
}

//****************************************************************************

CommConsoleClientThreadManager::CommConsoleClientThreadManager( CommRoutingTable& routingTable_ )
	: CommThreadManagerWithTimerThread( routingTable_, CommThreadManager::_flagPrepatch )
{
	timerThread = new _CommConsoleClientTimerThread( *this );
}

void CommConsoleClientThreadManager::shutdown()
{
	PWLock lock( cs );
	_Parent::shutdown();
	timerThread->shutdown();
}

bool CommConsoleClientThreadManager::terminated()
{
	PWLock lock( cs );
	if( !_Parent::terminated() )
		return false;
	if( !timerThread->terminated() )
		return false;
	return true;
}

CommConsoleClientThreadManager::~CommConsoleClientThreadManager()
{
	PWLock lock( cs );
	delete timerThread;
}

CommConsoleClientInterface& CommConsoleClientThreadManager::createInterface()
{
	PWLock lock( cs );

	_Object& obj = *objs.insert( objs.end(), _Object());
	obj.threadIdx = -1;
	obj.obj = 0;
	obj.inter = new CommConsoleClientInterface( *this );
	obj.inter->objIdx = objs.size() - 1;
	return *obj.inter;
}

void CommConsoleClientThreadManager::registerObject( CommConsoleClientObject& obj )
{
	PWLock lock( cs );

	CommConsoleClientInterface* inter = obj.inter;
	_Object& obj_ = objs[ inter->objIdx ];
	PASSERT3( obj_.obj == 0 );
	obj_.obj = &obj;
}

void CommConsoleClientThreadManager::start()
{
	PWLock lock( cs );

	int n = objs.size();
	for( int i=0; i < n ; ++i )
	{
		_Object& obj_ = objs[ i ];
		CommConsoleClientObject* obj = obj_.obj;
		PASSERT3( obj );

		_CommPhysicalConnection* physConn = routingTable.createIncomingPhysicalConnection( "pyrlocal://", this );
		obj_.inter->_init( physConn );

		obj_.threadIdx = _allocThread();
		Thread& thread = threads[ obj_.threadIdx ];
		_CommConsoleClientThread* conThread = new _CommConsoleClientThread( *this, obj );
		thread.thread = conThread;
		conThread->channelId = nextChannelId();
		routingTable.physicallyConnected( physConn, obj_.threadIdx, 0, conThread->channelId, 3 );

		//inform console client object about connection
		{
			_CommMsg m( _COMM_MSGTYPE_PHYSICAL_CONNECT, _COMM_MSGPRIORITY_INVALID );
			obj->processMessage( m );
		}

		thread.thread->start();

		PLog( "Started Console Client thread #%d", obj_.threadIdx );
	}
	timerThread->start();
}

void CommConsoleClientThreadManager::pushCommand( const char* s )
{
	PWLock lock( cs );

	int n = objs.size();
	for( int i=0; i < n ; ++i )
	{
		_CommMsg m( _COMM_MSGTYPE_NONE, _COMM_MSGPRIORITY_INITCONNECT );
		m.internal.slot = 1;
		m.body.composeString( s );
		threads[ objs[ i ].threadIdx ].thread->push( m );
	}
}

void CommConsoleClientThreadManager::_pushToThread(_CommManageableThread* thread, _CommMsg& msg) // override
{
	thread->push( msg );
}
