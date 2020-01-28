#include "ppinclude.h"

#ifdef PWIN32
#include "commmsgbody.h"
#include "commqueue.h"
#include "commrtable.h"
#include "commtmwin.h"
#include "ppwinwnd.h"

/* virtual */ void CommWin32WinInterface::_post( _CommMsg& msg )
{
	mgr->routingTable.routeIncomingMessage( physConn, 0, 0, msg );
	//mgr->routingTable.routeIncomingMessage( physConn, 0, msg );
}

/* virtual */ UINT32 CommWin32WinInterface::_postClientTimer( _CommMsg& msg, int msec )
{
	return mgr->_postTimer( physConn, msg, msec );
}

/* virtual */ void CommWin32WinInterface::_internalPost( _CommMsg& msg )
{
	msg.internal.channelId = physConn->channelId;
	mgr->virtualPush( physConn, msg );
}

//****************************************************************************

class _CommWin32WinTimerThread : public _CommTimerThread
{
private:
	CommWin32WinThreadManager* mgr;

public:
	_CommWin32WinTimerThread( CommWin32WinThreadManager& mgr_ )
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

class _CommDummyWin32WinThread : public _CommManageableThread//dummy - will never run
{
public:
	bool isWnd;
	HWND wnd;
	DWORD threadId;
	UINT32 channelId;
	UINT32 counter;
};

//****************************************************************************

CommWin32WinThreadManager::CommWin32WinThreadManager( UINT winMsg_, CommRoutingTable& routingTable_ )
	: CommThreadManagerWithTimerThread( routingTable_, CommThreadManager::_flagVirtualPush | CommThreadManager::_flagPrepatch )
{
	winMsg = winMsg_;
	timerThread = new _CommWin32WinTimerThread(*this);
}

void CommWin32WinThreadManager::start()
{
	PWLock lock( cs );
	timerThread->start();
}

void CommWin32WinThreadManager::shutdown()
{
	PWLock lock( cs );
	_Parent::shutdown();
	timerThread->shutdown();
}

bool CommWin32WinThreadManager::terminated()
{
	PWLock lock( cs );
	if( !_Parent::terminated() )
		return false;
	if( !timerThread->terminated() )
		return false;
	return true;
}

CommWin32WinThreadManager::~CommWin32WinThreadManager()
{
	PWLock lock( cs );
	delete timerThread;
}

void CommWin32WinThreadManager::_pushToThread( _CommManageableThread* dummyThread_, _CommMsg& m ) // override
{
	_CommDummyWin32WinThread* dummyThread = (_CommDummyWin32WinThread*)dummyThread_;//safe because always created as _CommDummyWin32WinThread
	if( dummyThread == 0 || dummyThread->channelId != m.internal.channelId )
	{
		PLog( "CommWin32WinThreadManager: message for obsolete channelId=%08X - ignored", m.internal.channelId );
		return;
	}

	_CommMsg* m2 = new _CommMsg( m, true );
	if( dummyThread->isWnd )
	{
		while( ! ::_PWin32PostMessage( dummyThread->wnd, winMsg, dummyThread->counter, (LPARAM)m2 ) )
		{
			DWORD err = GetLastError();
			PTRACE3( "CommWin32WinThreadManager: PostMessage() failed: err=%d", err );
			if( ! ::IsWindow( dummyThread->wnd ) )
			{
				PTRACE3( "CommWin32WinThreadManager: PostMessage() aborted" );
				delete m2;
				break;
			}
			PThread::sleep(100);
		}
	}
	else
	{
		while( ! ::_PWin32PostThreadMessage( dummyThread->threadId, winMsg, dummyThread->counter, (LPARAM)m2 ) )
		{
			DWORD err = GetLastError();
			PTRACE3( "CommWin32WinThreadManager: PostThreadMessage() failed: err=%d", err );
			if( ERROR_INVALID_THREAD_ID == err ) // the thread is dead or message queue is not created yet
			{
				PTRACE3( "CommWin32WinThreadManager: PostThreadMessage() aborted" );
				delete m2;
				break;
			}
			PThread::sleep( 100 );
		}
	}
	++dummyThread->counter;
}

/* virtual */ void CommWin32WinThreadManager::virtualPush( _CommPhysicalConnection* physConn, /*non-const!*/ _CommMsg& m )
{
	PWLock lock( cs );
	PASSERT3( physConn->mgr == this );
	Thread& thread = threads[ physConn->thread ];
	_pushToThread( thread.thread, m );
}

/* virtual */ void CommWin32WinThreadManager::virtualPushRead( _CommPhysicalConnection* physConn, /*non-const!*/ _CommMsg& m )
{
	PASSERT3( physConn->mgr == this );
	Thread& thread = threads[ physConn->thread ];
	_pushToThread( thread.thread, m );
}

/* virtual */ void CommWin32WinThreadManager::forceDisconnect( _CommPhysicalConnection* physConn )
{
	PLog( "FATAL: CommWin32WinThreadManager::forceDisconnect() called" );
	PASSERT3( 0 );
}

void CommWin32WinThreadManager::createWindowTransport( CommWin32WinInterface& inter, HWND wnd  )
{
	_createTransport( inter, true, wnd, 0 );
}

void CommWin32WinThreadManager::createThreadTransport( CommWin32WinInterface& inter, DWORD threadId )
{
	_createTransport( inter, false, 0, threadId );
}

void CommWin32WinThreadManager::_createTransport( CommWin32WinInterface& inter, bool isWnd, HWND wnd, DWORD threadId )
{
	PASSERT3( inter.mgr == 0 );
	PASSERT3( inter.physConn == 0 );

	PWLock lock( cs );
	int threadIdx = _allocThread();
	Thread& thread = threads[ threadIdx ];
	_CommDummyWin32WinThread* dummyThread = new _CommDummyWin32WinThread();//dummy. will never run
	dummyThread->isWnd = isWnd;
	dummyThread->wnd = wnd;
	dummyThread->threadId = threadId;
	dummyThread->channelId = nextChannelId();
	dummyThread->counter = 0;
	thread.thread = dummyThread;
	//	thread.thread->exit();

	inter.mgr = this;
	_CommPhysicalConnection* physConn = routingTable.createIncomingPhysicalConnection( "pyrwin://", this );
	routingTable.physicallyConnected( physConn, threadIdx, 0, dummyThread->channelId, 3 );
	inter.physConn = physConn;

	//inform window object about connection
	{
		_CommMsg m( _COMM_MSGTYPE_PHYSICAL_CONNECT, _COMM_MSGPRIORITY_INITCONNECT );
		push( physConn, m );
	}
}

void CommWin32WinThreadManager::destroyTransport( CommWin32WinInterface& inter )
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

/* static */ UINT32 CommWin32WinThreadManager::translateWinMsg( _CommMsg& ret, WPARAM w, LPARAM l )
{
	_CommMsg* m = (_CommMsg*)l;
	ret.moveFrom( *m );
	delete m;
	return w;
}

/* static */ unique_ptr< _CommMsg > CommWin32WinThreadManager::translateWinMsg( WPARAM w, LPARAM l )
{
	UNUSED( w );
	return unique_ptr< _CommMsg >( reinterpret_cast< _CommMsg* >( l ) );
}

/* static */ const _CommMsg& CommWin32WinThreadManager::peekTranslateWinMsg( WPARAM w, LPARAM l )
{
	_CommMsg* m = (_CommMsg*)l;
	return *m;
}

#endif
