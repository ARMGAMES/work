// Use _WIN32 instead of PWIN32 here. 
#ifdef _WIN32

#include <winsock2.h>

#include "ppinclude.h"
#include "ppstring.h"
#include "pperror.h"
#include "pplogfile.h"
#include "ppthread.h"
#include "ppsocket.h"

#include "commmsgbody.h"
#include "commqueue.h"
#include "commrtable.h"
#include "commtmclientudp.h"
#include "_commudp0.h"
#include "_commtmclientudp.h"
#include "commoption.h"



_CommClientUDPThreadBase::_CommClientUDPThreadBase( CommClientUDPThreadManager& mgr_, int mgrIdx_, _CommPhysicalConnection* physConn_, UINT32 channelId_ )
{
	mgr = &mgr_;
	mgrIdx = mgrIdx_;
	physConn = physConn_;
	channelId = channelId_;
}

//****************************************************************************

CommClientUDPThreadManager::CommClientUDPThreadManager( CommRoutingTable& routingTable_ )
: _Parent( routingTable_ )
{
	comm_logging=0;
	udpConnectTimeout=0;
	udpPacketSize = UDP_DEFAULT_FRAME_SIZE;
	keepAliveRead = UDP_DEFAULT_KEEP_ALIVE_READ;
	keepAliveWrite = UDP_DEFAULT_KEEP_ALIVE_WRITE;
	keepAlivePoll = UDP_DEFAULT_KEEP_ALIVE_POLL;
	resend = UDP_RESENT_TIMEOUT;

}

/* virtual */ void CommClientUDPThreadManager::setOption( const char* optName, const char* optValue )
{
	PWLock lock( cs );
	if ( strcmp( optName, COMM_OPTION_PROTOCOL_LOG_FILE ) == 0 && comm_logging == 0)
		comm_logging = new _CommProtocolLog(optValue,PLogFile::perDate|PLogFile::printTimePerLine);
	else if( strcmp( optName, COMM_OPTION_THREADPRIORITY ) == 0 )
		threadPriority = optValue;
	else if( strcmp( optName, COMM_OPTION_UDPCONNECT ) == 0 )
		udpConnectTimeout = atoi( optValue );
	else if( strcmp( optName, COMM_OPTION_KEEPALIVE ) == 0 )
	{
		const char* sl = strchr( optValue, '/' );
		if( sl == 0 )
		{
			PLog( "CommRWUDPThreadManager: Invalid COMM_OPTION_KEEPALIVE option - no '/' found" );
			return;
		}
		keepAliveWrite = atoi( optValue );
		keepAliveRead = atoi( sl + 1 );
	}
	else if( strcmp( optName, COMM_OPTION_FRAMESIZE ) == 0 )
		udpPacketSize =  atoi( optValue );
	else if( strcmp( optName, COMM_OPTION_KEEPALIVEPOLL ) == 0 )
		keepAlivePoll =  atoi( optValue );
	else if( strcmp( optName, COMM_OPTION_RESEND ) == 0 )
		resend =  atoi( optValue );

	else
		CommThreadManager::setOption( optName, optValue );
}

void CommClientUDPThreadManager::_freeSocket( int threadIdx )
{
	PWLock lock( cs );
	Thread& thread = threads[ threadIdx ];

	delete thread.thread;
	thread.thread = 0;
}

int CommClientUDPThreadManager::_createThread( UINT32& channelId, _CommPhysicalConnection* physConn, const char* addr, int masterIdx, UINT32 masterChannelId )
//MUST be locked before call
{
	int threadIdx = _allocThread();

	Thread& thread = threads[ threadIdx ];
	_CommClientUDPThreadBase* th = _createNewThread( threadIdx, physConn );//new _CommClientThread< _CommClientPlain >( *this, threadIdx, physConn, nextChannelId() );
	channelId = th->channelId;
	th->address = addr;

	thread.thread = th;
	thread.maxSlots = 1;
	thread.slotsLeft = 0;
	thread.thread->start();
	return threadIdx;
}

void CommClientUDPThreadManager::connect( _CommPhysicalConnection* physConn )
{
	PWLock lock( cs );
	UINT32 slaveChannelId;//dummy
	int threadIdx = _createThread( slaveChannelId, physConn, physConn->address, -1, 0 );
	PTRACE3( "Started Client thread #%d", threadIdx );
}


void CommClientUDPThreadManager::forceDisconnect( _CommPhysicalConnection* physConn )
{
	PWLock lock( cs );
	Thread& th = threads[ physConn->thread ];
	_CommClientUDPThreadBase* thread = static_cast< _CommClientUDPThreadBase* >( th.thread );//safe because all Thread.threads are created by CommClientThreadManager
	if (thread)
		thread->forceDisconnect();
}

void CommClientUDPThreadManager::_debugEmulateNetworkFailure()
{
	PWLock lock( cs );
	int n = threads.size();
	for( int i=0; i < n ; ++i )
	{
		Thread& th = threads[ i ];
		_CommClientUDPThreadBase* thread = static_cast< _CommClientUDPThreadBase* >( th.thread );//safe because all Thread.threads are created by CommClientThreadManager
		if( thread )
			thread->_debugEmulateNetworkFailure();
	}
}

#endif