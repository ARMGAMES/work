#ifdef P_ANDROID
#include "ppinclude.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "commrtable.h"
#include "commtmandroid.h"

/* virtual */ void CommAndroidInterface::_post( _CommMsg& msg )
{
	mgr->routingTable.routeIncomingMessage( physConn, 0, 0, msg );
}

/* virtual */ UINT32 CommAndroidInterface::_postClientTimer( _CommMsg& msg, int msec )
{
	return mgr->_postTimer( physConn, msg, msec );
}

/* virtual */ void CommAndroidInterface::_internalPost( _CommMsg& msg )
{
	msg.internal.channelId = physConn->channelId;
	mgr->virtualPush( physConn, msg );
}

//****************************************************************************

class _CommAndroidTimerThread : public _CommTimerThread
{
private:
	CommAndroidThreadManager* mgr;

public:
	_CommAndroidTimerThread( CommAndroidThreadManager& mgr_ )
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

class _CommDummyAndroidThread : public _CommManageableThread//dummy - will never run
{
	friend class CommAndroidThreadManager;
public:
	_CommDummyAndroidThread()
	{
	}
	UINT32 channelId;
	UINT32 counter;
};

class _CommDummyAndroidQueueThread : public _CommDummyAndroidThread
{
	friend class CommAndroidThreadManager;
	CommMsgQueue & queue;
public:
	_CommDummyAndroidQueueThread(CommMsgQueue & queue_) : queue(queue_)
	{
	}
};

//****************************************************************************

CommAndroidThreadManager::CommAndroidThreadManager( CommRoutingTable& routingTable_ )
: CommThreadManagerWithTimerThread( routingTable_, CommThreadManager::_flagVirtualPush | CommThreadManager::_flagPrepatch )
{
	timerThread = new _CommAndroidTimerThread( *this );
}

void CommAndroidThreadManager::start()
{
	PWLock lock( cs );
	timerThread->start();
}

void CommAndroidThreadManager::shutdown()
{
	PWLock lock( cs );
	_Parent::shutdown();
	timerThread->shutdown();
}

bool CommAndroidThreadManager::terminated()
{
	PWLock lock( cs );
	if( !_Parent::terminated() )
		return false;
	if( !timerThread->terminated() )
		return false;
	return true;
}

CommAndroidThreadManager::~CommAndroidThreadManager()
{
	PWLock lock( cs );
	delete timerThread;
}

void CommAndroidThreadManager::_pushToThread( _CommManageableThread* dummyThread_, _CommMsg& m ) // override
{
	_CommDummyAndroidThread* dummyThread = static_cast<_CommDummyAndroidThread *>(dummyThread_);
	if( dummyThread == 0 || dummyThread->channelId != m.internal.channelId )
	{
		PLog( "CommAndroidThreadManager: message for obsolete channelId=%08X - ignored", m.internal.channelId );
		return;
	}
	_CommDummyAndroidQueueThread* dummyThreadQueue = static_cast<_CommDummyAndroidQueueThread *>(dummyThread);
	dummyThreadQueue->queue.push(m);
	++dummyThread->counter;
}

/* virtual */ void CommAndroidThreadManager::virtualPush( _CommPhysicalConnection* physConn, /*non-const!*/ _CommMsg& m )
{
	PWLock lock( cs );
	PASSERT3( physConn->mgr == this );
	Thread& thread = threads[ physConn->thread ];
	_pushToThread( thread.thread, m );
}
/* virtual */ void CommAndroidThreadManager::virtualPushRead( _CommPhysicalConnection* physConn, /*non-const!*/ _CommMsg& m )
{
	PASSERT3( physConn->mgr == this );
	Thread& thread = threads[ physConn->thread ];
	_pushToThread( thread.thread, m );
}

/* virtual */ void CommAndroidThreadManager::forceDisconnect( _CommPhysicalConnection* physConn )
{
	PLog( "FATAL: CommAndroidThreadManager::forceDisconnect() called" );
	PASSERT3( 0 );
}
void CommAndroidThreadManager::createTransport( CommAndroidInterface& inter, CommMsgQueue & queue )
{
	PASSERT3( inter.mgr == 0 );
	PASSERT3( inter.physConn == 0 );

	PWLock lock( cs );
	int threadIdx = _allocThread();
	Thread& thread = threads[ threadIdx ];
	_CommDummyAndroidThread* dummyThread;

	dummyThread = new _CommDummyAndroidQueueThread(queue);

	dummyThread->channelId = nextChannelId();
	dummyThread->counter = 0;
	thread.thread = dummyThread;
	inter.mgr = this;
	_CommPhysicalConnection* physConn = routingTable.createIncomingPhysicalConnection( "pyrandroid://", this );
	routingTable.physicallyConnected( physConn, threadIdx, 0, dummyThread->channelId, 3 );
	inter.physConn = physConn;

	//inform window object about connection
	{
		_CommMsg m( _COMM_MSGTYPE_PHYSICAL_CONNECT, _COMM_MSGPRIORITY_INITCONNECT );
		push( physConn, m );
	}
}

void CommAndroidThreadManager::destroyTransport( CommAndroidInterface& inter )
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

void CommAndroidThreadManager::translateAndroidMsg( _CommMsg& ret, _CommMsg* androidMsg)
{
	_CommMsg* m = androidMsg;
	ret.moveFrom( *m );
	delete m;
}

#endif
