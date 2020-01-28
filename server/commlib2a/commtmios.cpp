#if defined( P_IOS )

#include "ppinclude.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "commrtable.h"
#include "commtmios.h"

/* virtual */ void CommIOSInterface::_post( _CommMsg& msg )
{
	mgr->routingTable.routeIncomingMessage( physConn, 0, 0, msg );
}

/* virtual */ UINT32 CommIOSInterface::_postClientTimer( _CommMsg& msg, int msec )
{
	return mgr->_postTimer( physConn, msg, msec );
}

//****************************************************************************

class _CommIOSTimerThread : public _CommTimerThread
{
private:
	CommIOSThreadManager* mgr;

public:
	_CommIOSTimerThread( CommIOSThreadManager& mgr_ )
	{
		mgr = &mgr_;
	}

public://overridden
	void timer( _CommMsg& msg ) override
	{
		mgr->_timer( msg );
	}
};

//****************************************************************************

class _CommDummyIOSThread : public _CommManageableThread//dummy - will never run
{
	friend class CommIOSThreadManager;
public:
	_CommDummyIOSThread()
	{
	}
	UINT32 channelId;
	UINT32 counter;
};

class _CommDummyIOSQueueThread : public _CommDummyIOSThread
{
	friend class CommIOSThreadManager;
	CommMsgQueue & queue;
public:
	_CommDummyIOSQueueThread(CommMsgQueue & queue_) : queue(queue_)
	{
	}
};

//****************************************************************************

CommIOSThreadManager::CommIOSThreadManager( CommRoutingTable& routingTable_ )
: CommThreadManagerWithTimerThread( routingTable_, CommThreadManager::_flagVirtualPush | CommThreadManager::_flagPrepatch )
{
	timerThread = new _CommIOSTimerThread( *this );
}

void CommIOSThreadManager::start()
{
	PWLock lock( cs );
	timerThread->start();
}

void CommIOSThreadManager::shutdown()
{
	PWLock lock( cs );
	_Parent::shutdown();
	timerThread->shutdown();
}

bool CommIOSThreadManager::terminated()
{
	PWLock lock( cs );
	if( !_Parent::terminated() )
		return false;
	if( !timerThread->terminated() )
		return false;
	return true;
}

CommIOSThreadManager::~CommIOSThreadManager()
{
	PWLock lock( cs );
	delete timerThread;
}

void CommIOSThreadManager::_pushToThread( _CommManageableThread* dummyThread_, _CommMsg& m ) //override
{
	_CommDummyIOSThread* dummyThread = static_cast<_CommDummyIOSThread *>(dummyThread_);
	if( dummyThread == 0 || dummyThread->channelId != m.internal.channelId )
	{
		PLog( "CommIOSThreadManager: message for obsolete channelId=%08X - ignored", m.internal.channelId );
		return;
	}
	_CommDummyIOSQueueThread* dummyThreadQueue = static_cast<_CommDummyIOSQueueThread *>(dummyThread);
	dummyThreadQueue->queue.push(m);
	++dummyThread->counter;
}

/* virtual */ void CommIOSThreadManager::virtualPush( _CommPhysicalConnection* physConn, /*non-const!*/ _CommMsg& m )
{
	PWLock lock( cs );
	PASSERT3( physConn->mgr == this );
	Thread& thread = threads[ physConn->thread ];
	_pushToThread( thread.thread, m );
}
/* virtual */ void CommIOSThreadManager::virtualPushRead( _CommPhysicalConnection* physConn, /*non-const!*/ _CommMsg& m )
{
	PASSERT3( physConn->mgr == this );
	Thread& thread = threads[ physConn->thread ];
	_pushToThread( thread.thread, m );
}

/* virtual */ void CommIOSThreadManager::forceDisconnect( _CommPhysicalConnection* physConn )
{
	PLog( "FATAL: CommIOSThreadManager::forceDisconnect() called" );
	PASSERT3( 0 );
}
void CommIOSThreadManager::createTransport( CommIOSInterface& inter, CommMsgQueue & queue )
{
	PASSERT3( inter.mgr == 0 );
	PASSERT3( inter.physConn == 0 );

	PWLock lock( cs );
	int threadIdx = _allocThread();
	Thread& thread = threads[ threadIdx ];
	_CommDummyIOSThread* dummyThread;

	dummyThread = new _CommDummyIOSQueueThread(queue);

	dummyThread->channelId = nextChannelId();
	dummyThread->counter = 0;
	thread.thread = dummyThread;
	inter.mgr = this;
	_CommPhysicalConnection* physConn = routingTable.createIncomingPhysicalConnection( "pyrios://", this );
	routingTable.physicallyConnected( physConn, threadIdx, 0, dummyThread->channelId, 3 );
	inter.physConn = physConn;

	//inform window object about connection
	{
		_CommMsg m( _COMM_MSGTYPE_PHYSICAL_CONNECT, _COMM_MSGPRIORITY_INITCONNECT );
		push( physConn, m );
	}
}

void CommIOSThreadManager::destroyTransport( CommIOSInterface& inter )
{
	int threadIdx = inter.physConn->thread;
	routingTable.physicallyDisconnected( inter.physConn );

	PWLock lock( cs );
	PASSERT3( threadIdx >= 0 && threadIdx < threads.size() );
	Thread& thread = threads[ threadIdx ];
	delete thread.thread;
	thread.thread = 0;

	inter.mgr = 0;
	inter.physConn = 0;
}

#endif // defined( P_IOS )
