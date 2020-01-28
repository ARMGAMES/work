#include "ppinclude.h"
#include "ppstring.h"
#include "pperror.h"
#include "pplogfile.h"
#include "ppthread.h"
#include "ppsocket.h"

#include "commmsgbody.h"
#include "commqueue.h"
#include "commrtable.h"
#include "commtmclient.h"
#include "_commtcp0.h"
#include "_commtmclient.h"
#include "commoption.h"


_CommClientThreadBase::_CommClientThreadBase( CommClientThreadManagerBase& mgr_, int mgrIdx_, _CommPhysicalConnection* physConn_, UINT32 channelId_ )
{
	mgr = &mgr_;
	mgrIdx = mgrIdx_;
	physConn = physConn_;
	channelId = channelId_;
	statInit = false;
}

//****************************************************************************

CommClientThreadManagerBase::CommClientThreadManagerBase( CommRoutingTable& routingTable_ )
: _Parent( routingTable_ )
{
	httpsProxyPort = 0;
	freezeReconnectTimeout = 0;
	tcpConnectTimeout = 0;
	sslConnectTimeout = 0;
	tcpNoDelay = false;
	MaxBlockSize = DEFAULT_FRAME_SIZE;
	CompressBitsSize = DEFAULT_COMPRESS_BITS_SIZE;
	DefaultProtocol = DEFAULT_PROTOCOL;
	optimizePacketSize = false;

	comm_logging=0;
	tcpBufferRcv = -1;
	tcpBufferSnd = -1;
	enableClientEncryption = DEFAULT_CLIENT_ENCRYPTION;
}

/* virtual */ void CommClientThreadManagerBase::setOption( const char* optName, const char* optValue )
{
	PWLock lock( cs );
	if( strcmp( optName, COMM_OPTION_HTTPS_PROXY ) == 0 )
	{
		const char* col = strchr( optValue, ':' );
		if( col == 0 )
		{
			PLog( "CommClientThreadManagerBase: Invalid COMM_OPTION_HTTPS_PROXY option - no ':' found" );
			return;
		}
		httpsProxy.assign( optValue, col );
		httpsProxyPort = atoi( col + 1 );
	}
	else if( strcmp( optName, COMM_OPTION_FREEZERECONNECT ) == 0 )
		freezeReconnectTimeout = atoi( optValue );
	else if( strcmp( optName, COMM_OPTION_TCPCONNECT ) == 0 )
		tcpConnectTimeout = atoi( optValue );
	else if( strcmp( optName, COMM_OPTION_SSLCONNECT ) == 0 )
		sslConnectTimeout = atoi( optValue );
	else if( strcmp( optName, COMM_OPTION_THREADPRIORITY ) == 0 )
		threadPriority = optValue;
	else if( strcmp( optName, COMM_OPTION_TCPNODELAY ) == 0 )
		tcpNoDelay = atoi( optValue ) != 0;
	else if( strcmp( optName, COMM_OPTION_TCPBUFFER ) == 0 )
	{
		tcpBufferRcv = tcpBufferSnd = atoi( optValue );
	}
	else if( strcmp( optName, COMM_OPTION_TCPBUFFER_RCV ) == 0 )
	{
		tcpBufferRcv = atoi( optValue );
	}
	else if( strcmp( optName, COMM_OPTION_TCPBUFFER_SND ) == 0 )
	{
		tcpBufferSnd = atoi( optValue );
	}
	else if( strcmp( optName, COMM_OPTION_FRAMESIZE ) == 0 )
		MaxBlockSize =  atoi( optValue );
	else if( strcmp( optName, COMM_OPTION_COMPRESS ) == 0 )
		CompressBitsSize =  atoi( optValue );
	else if( strcmp( optName, COMM_OPTION_PROTOCOL ) == 0 )
	{
		UINT32 _DefaultProtocol =  atoi( optValue );
		if (_DefaultProtocol >= MIN_SUPPORTED_PROTOCOL && _DefaultProtocol <= MAX_SUPPORTED_PROTOCOL)
			DefaultProtocol = _DefaultProtocol;
	}
	else if ( strcmp( optName, COMM_OPTION_PROTOCOL_LOG_FILE ) == 0 && comm_logging == 0)
		comm_logging = new _CommProtocolLog(optValue,PLogFile::perDateLimited);
	else if( strcmp( optName, COMM_OPTION_CLIENT_ENCRYPTION ) == 0 )
		enableClientEncryption =  atoi( optValue ) != 0;
	else
		CommThreadManager::setOption( optName, optValue );
}

void CommClientThreadManagerBase::_freeSocket( int threadIdx )
{
	PWLock lock( cs );
	Thread& thread = threads[ threadIdx ];

	delete thread.thread;
	thread.thread = 0;
}

int CommClientThreadManagerBase::_createThread( UINT32& channelId, _CommPhysicalConnection* physConn, const char* addr, int masterIdx, UINT32 masterChannelId )
//MUST be locked before call
{
	int threadIdx = _allocThread();

	Thread& thread = threads[ threadIdx ];
	_CommClientThreadBase* th = _createNewThread( threadIdx, physConn );//new _CommClientThread< _CommClientPlain >( *this, threadIdx, physConn, nextChannelId() );
	channelId = th->channelId;
	th->address = addr;

	thread.thread = th;
	thread.maxSlots = 1;
	thread.slotsLeft = 0;
	thread.thread->start();
	return threadIdx;
}

void CommClientThreadManagerBase::connect( _CommPhysicalConnection* physConn )
{
	PWLock lock( cs );
	UINT32 slaveChannelId;//dummy
	int threadIdx = _createThread( slaveChannelId, physConn, physConn->address, -1, 0 );
	PTRACE3( "Started Client thread #%d", threadIdx );
}


void CommClientThreadManagerBase::forceDisconnect( _CommPhysicalConnection* physConn )
{
	PWLock lock( cs );
	Thread& th = threads[ physConn->thread ];
	_CommClientThreadBase* thread = static_cast< _CommClientThreadBase* >( th.thread );//safe because all Thread.threads are created by CommClientThreadManager
	if (thread)
		thread->forceDisconnect();
}

void CommClientThreadManagerBase::_debugEmulateNetworkFailure()
{
	PWLock lock( cs );
	const size_t n = threads.size();
	for( size_t i=0; i < n ; ++i )
	{
		Thread& th = threads[ i ];
		_CommClientThreadBase* thread = static_cast< _CommClientThreadBase* >( th.thread );//safe because all Thread.threads are created by CommClientThreadManager
		if( thread )
			thread->_debugEmulateNetworkFailure();
	}
}

