

#ifdef P_MAC10X


#include <CoreFoundation/CoreFoundation.h>
#include "ppinclude.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "commrtable.h"
#include "commtmmac.h"

/* virtual */ void CommMac10XInterface::_post( _CommMsg& msg )
{
	mgr->routingTable.routeIncomingMessage( physConn, 0, 0, msg );
}

/* virtual */ UINT32 CommMac10XInterface::_postClientTimer( _CommMsg& msg, int msec )
{
	return mgr->_postTimer( physConn, msg, msec );
}
/* virtual */ void CommMac10XInterface::_internalPost( _CommMsg& msg )
{
	msg.internal.channelId = physConn->channelId;
	mgr->virtualPush( physConn, msg );
}


//****************************************************************************

class _CommMac10XTimerThread : public _CommTimerThread
{
private:
	CommMac10XThreadManager* mgr;

public:
	_CommMac10XTimerThread( CommMac10XThreadManager& mgr_ )
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

class _CommDummyMac10XThread : public _CommManageableThread//dummy - will never run
{
public:
	UINT32 channelId;
	UINT32 counter;
	CFMessagePortRef portRef;
	SInt32 msgId;
};

//****************************************************************************

CommMac10XThreadManager::CommMac10XThreadManager( CommRoutingTable& routingTable_ )
	: CommThreadManagerWithTimerThread( routingTable_, CommThreadManager::_flagVirtualPush | CommThreadManager::_flagPrepatch )
{
	timerThread = new _CommMac10XTimerThread( *this );
}

void CommMac10XThreadManager::start()
{
	PLock lock( cs );
	timerThread->start();
}

void CommMac10XThreadManager::shutdown()
{
	PLock lock( cs );
	_Parent::shutdown();
	timerThread->shutdown();
}

bool CommMac10XThreadManager::terminated()
{
	PLock lock( cs );
	if( !_Parent::terminated() )
		return false;
	if( !timerThread->terminated() )
		return false;
	return true;
}

CommMac10XThreadManager::~CommMac10XThreadManager()
{
	PLock lock( cs );
	delete timerThread;
}

void CommMac10XThreadManager::_pushToThread( _CommManageableThread* dummyThread_, _CommMsg& m ) // override
{
	_CommDummyMac10XThread* dummyThread = (_CommDummyMac10XThread*)dummyThread_;//safe because always created as _CommDummyWin32WinThread
	if( dummyThread == 0 || dummyThread->channelId != m.internal.channelId )
	{
		PLog( "CommMac10XThreadManager: message for obsolete channelId=%08X - ignored", m.internal.channelId );
		return;
	}

	unsigned char data[sizeof(UINT32) + sizeof(_CommMsg)];
	*(UINT32 *) data = dummyThread->counter;
	new ( (_CommMsg *) (data+sizeof(UINT32))) _CommMsg(m, true);
	CFDataRef msg = CFDataCreate(0,(const UInt8 *)data,sizeof(data));
	SInt32 errCode = 1;
	while(errCode)
	{
		errCode = CFMessagePortSendRequest(dummyThread->portRef,dummyThread->msgId,msg,10.,0.,0,0);
		if (errCode)
		{
			PTRACE3("MessagePortSendRequest failed, msg=%x, err=%d",dummyThread->msgId, errCode);	
			PThread::sleep(100);
		}
	}
	CFRelease (msg);
	++dummyThread->counter;
}

/* virtual */ void CommMac10XThreadManager::virtualPush( _CommPhysicalConnection* physConn, /*non-const!*/ _CommMsg& m )
{
	PLock lock( cs );
	PASSERT3( physConn->mgr == this );
	PTRACE4("virtualPush: m.type=%d, address=%s",m.type,physConn->address.c_str());
	Thread& thread = threads[ physConn->thread ];
	_pushToThread( thread.thread, m );
}
/* virtual */ void CommMac10XThreadManager::virtualPushRead( _CommPhysicalConnection* physConn, /*non-const!*/ _CommMsg& m )
{
	PASSERT3( physConn->mgr == this );
	PTRACE4("virtualPush: m.type=%d, address=%s",m.type,physConn->address.c_str());
	Thread& thread = threads[ physConn->thread ];
	_pushToThread( thread.thread, m );
}

/* virtual */ void CommMac10XThreadManager::forceDisconnect( _CommPhysicalConnection* physConn )
{
	PLog( "FATAL: CommMac10XThreadManager::forceDisconnect() called" );
	PASSERT3( 0 );
}

void CommMac10XThreadManager::createMac10XTransport( CommMac10XInterface& inter,const char * portName, SInt32 msgId_ )
{
	_createTransport( inter,portName,msgId_);
}


void CommMac10XThreadManager::_createTransport( CommMac10XInterface& inter, const char * portName, SInt32 msgId_ )
{

	PASSERT3( inter.mgr == 0 );
	PASSERT3( inter.physConn == 0 );

	PLock lock( cs );
	PTRACE3("_createTransport: ");

	int threadIdx = _allocThread();
	Thread& thread = threads[ threadIdx ];
	_CommDummyMac10XThread* dummyThread = new _CommDummyMac10XThread();//dummy. will never run
	dummyThread->channelId = nextChannelId();
	dummyThread->counter = 0;
	dummyThread->msgId = msgId_;
	CFStringRef portNameRef = CFStringCreateWithCString(0,portName,CFStringGetSystemEncoding());
	dummyThread->portRef = CFMessagePortCreateRemote(0, portNameRef);
	CFRelease(portNameRef);
	thread.thread = dummyThread;

	inter.mgr = this;
	_CommPhysicalConnection* physConn = routingTable.createIncomingPhysicalConnection( "pyrmac://", this );
	routingTable.physicallyConnected( physConn, threadIdx, 0, dummyThread->channelId, 3 );
	inter.physConn = physConn;

	//inform window object about connection
	{
		_CommMsg m( _COMM_MSGTYPE_PHYSICAL_CONNECT, _COMM_MSGPRIORITY_INITCONNECT );
		push( physConn, m );
	}
}

void CommMac10XThreadManager::destroyTransport( CommMac10XInterface& inter )
{
	int threadIdx = inter.physConn->thread;
	routingTable.physicallyDisconnected( inter.physConn );

	PLock lock( cs );
	PASSERT3( threadIdx >= 0 && threadIdx < threads.size() );
	Thread& thread = threads[ threadIdx ];
	CFMessagePortInvalidate(((_CommDummyMac10XThread*)thread.thread)->portRef);
	CFRelease(((_CommDummyMac10XThread*)thread.thread)->portRef);

	delete thread.thread;
	thread.thread = 0;

	inter.mgr = 0;
	inter.physConn = 0;
}

/* static */ UINT32 CommMac10XThreadManager::translateMacEvent( _CommMsg& ret, CFDataRef dataRef )
{
	unsigned char data[sizeof(UINT32) + sizeof(_CommMsg)];
	CFRange range;
	range.location=0;
	range.length=sizeof(data);
	CFDataGetBytes(dataRef,range,data);
	UINT32 counter = *(UINT32*)data;
	ret.moveFrom( *(_CommMsg*)(data+sizeof(UINT32)));
	PTRACE4("translateMacEvent - m.type=%d, counter=%d",ret.type, counter);
	return counter;
}

#endif
