#include "ppinclude.h"
#include "ppstring.h"
#include "pperror.h"
#include "pplogfile.h"
#include "ppthread.h"
#include "ppsocket.h"

#include "commmsgbody.h"
#include "commqueue.h"
#include "commrtable.h"
#include "commtmsimple.h"
#include "_commtcp0.h"
#include "_commtmsimple.h"
#include "commoption.h"

/* virtual */ void _CommSimpleThreadWriter::run()
{
	PTRACE3( "SimpleWriter thread started" );
	PThread::setCurrentThreadPriority( manager->mgr->threadPriority );

	int failureWeight = 0;
	int errorCode = 0;
	try
	{
		PString addr;
		int port;
		manager->mgr->parseSocketAddr( addr, port, manager->address );

		if( *manager->mgr->httpsProxy )
		{
			const char* connAddr = manager->mgr->httpsProxy;
			int connPort = manager->mgr->httpsProxyPort;

			try
			{
				PLog( "Connecting to https proxy %s:%d", connAddr, connPort );
				sock.connect( connAddr, connPort );
				PLog( "Connected to https proxy %s:%d", connAddr, connPort );
			}
			catch( PError& err )
			{//rethrow with weight
				throw _CommSimpleThreadError( err.why(), _HTTPSPROXY_ERROR_WEIGHT );
			}
			// THIS CODE IS DUPLICATED IN COMMLOCALSSLCLIENT.CPP Make changes there if you change something here
			CommHttpRequest req;
			PString url( "http://" );
			url.append( addr ).append( ':' ).appendInt( port ).append( '/' );
			req.setUrl( url );
			req.setProxy( manager->mgr->httpsProxy, manager->mgr->httpsProxyPort );

			PString reqStr;
			req.createConnectRequest( reqStr );
			sock.sendComplete( (const BYTE*)reqStr.c_str(), strlen( reqStr ) );
		}
		else
		{
			const char* connAddr = addr;
			int connPort = port;

			PLog( "Connecting to %s:%d", connAddr, connPort );
			sock.connect( connAddr, connPort );
			PLog( "Connected to %s:%d", connAddr, connPort );
		}

		manager->_writerConnected();

		bool exit = false;
		while( !exit )
		{
			_CommMsg msg;
			int res = q.waitPop( msg );
			switch( res )
			{
			case CommMsgQueue::waitPopOk:
				{
					CommMsgParser parser( msg.body );
					const BYTE* p;
					size_t sz;
					parser._parseVarBlock( p, sz );
					sock.sendComplete( p, sz );
				}
				break;
			case CommMsgQueue::waitPopShutdown:
				{
					exit = true;
					break;
				}
			}
		}
	}
	catch( PSSLError& err )
	{
		PLog( "Error in SimpleWriter stream: %s", err.why() );
		failureWeight = -1;
		errorCode=_SOCKET_SSLCERTIFICATEFAILED;
	}
	catch( _CommSimpleThreadError& err )
	{
		PLog( "Error in SimpleWriter stream: %s", err.why() );
		failureWeight = err.weight();
	}

	catch( PError& err )
	{
		PLog( "Error in SimpleWriter stream: %s", err.why() );
		failureWeight = 1;
	}
	manager->_readerWriterFailure( failureWeight,errorCode );
	PTRACE3( "SimpleWriter thread terminated" );
}

//****************************************************************************

/* virtual */ void _CommSimpleThreadReader::run()
{
	PTRACE3( "SimpleReader thread started" );
	PThread::setCurrentThreadPriority( manager->mgr->threadPriority );

	int failureWeight = 0;
	int errorCode = 0;
	try
	{
		BYTE buf[ 256 ];
		if( *manager->mgr->httpsProxy )
		{
			CommHttpResponse resp;
			for(;;)
			{
				// THIS CODE IS DUPLICATED IN COMMLOCALSSLCLIENT.CPP Make changes there if you change something here
				size_t rd = sock.recv( buf, sizeof(buf) );//incomplete
				const BYTE* p = buf;
				bool respOk = resp.read( p, rd );
				if( respOk )
				{
					int code = resp.getRespCode();
					if( code != 200 )
					{
						PString err( "HTTP Proxy response: " );
						err.appendInt( code );
						if( code == 403 )
							throw _CommSimpleThreadError( err, _HTTPSPROXY_403ERROR_WEIGHT );
						else
							throw _CommSimpleThreadError( err, _HTTPSPROXY_ERROR_WEIGHT );
					}
					manager->_pushRead( p, buf + rd - p );
					break;
				}
			}
		}

		for(;;)
		{
			size_t rd = sock.recv( buf, sizeof(buf) );//incomplete
			manager->_pushRead( buf, rd );
		}
	}
	catch( PSSLError& err )
	{
		PLog( "Error in SimpleWriter stream: %s", err.why() );
		failureWeight = -1;
		errorCode=_SOCKET_SSLCERTIFICATEFAILED;
	}
	catch( _CommSimpleThreadError& err )
	{
		PLog( "Error in SimpleReader stream: %s", err.why() );
		failureWeight = err.weight();
	}
	catch( PError& err )
	{
		PLog( "Error in SimpleReader stream: %s", err.why() );
		failureWeight = 1;
	}
	manager->_readerWriterFailure( failureWeight,errorCode );
	PTRACE3( "SimpleReader thread terminated" );
}

//****************************************************************************

_CommSimpleThreadBase::_CommSimpleThreadBase( CommSimpleThreadManagerBase& mgr_, int mgrIdx_, _CommPhysicalConnection* physConn_, UINT32 channelId_ )
{
	mgr = &mgr_;
	mgrIdx = mgrIdx_;
	physConn = physConn_;
	channelId = channelId_;

	masterIdx = -1;
	slaveIdx = -1;
}

//****************************************************************************

CommSimpleThreadManagerBase::CommSimpleThreadManagerBase( CommRoutingTable& routingTable_ )
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
	comm_logging=0;
	tcpBufferRcv = -1;
	tcpBufferSnd = -1;
#ifndef COMM_CLIENT_ONLY
	maximumMessageSize = 0;
	maximumMessageRate = 0;
	maximumMessageRatePeriod = 0;
	maxMessageSizeDebug = false; 
	maxMessageRateDebug = true; 
	enableServerEncryption = false;
	minimumProtocol = MIN_SUPPORTED_PROTOCOL;
#endif
	enableClientEncryption = DEFAULT_CLIENT_ENCRYPTION;


}

/* virtual */ void CommSimpleThreadManagerBase::setOption( const char* optName, const char* optValue )
{
	PWLock lock( cs );
	if( strcmp( optName, COMM_OPTION_HTTPS_PROXY ) == 0 )
	{
		const char* col = strchr( optValue, ':' );
		if( col == 0 )
		{
			PLog( "CommSimpleThreadManagerBase: Invalid COMM_OPTION_HTTPS_PROXY option - no ':' found" );
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
#ifndef COMM_CLIENT_ONLY
		if (_DefaultProtocol < minimumProtocol)
			_DefaultProtocol = minimumProtocol;
#endif
		if (_DefaultProtocol >= MIN_SUPPORTED_PROTOCOL && _DefaultProtocol <= MAX_SUPPORTED_PROTOCOL)
			DefaultProtocol = _DefaultProtocol;
	}
#ifndef COMM_CLIENT_ONLY
	else if( strcmp( optName, COMM_OPTION_PROTOCOL_MINIMUM ) == 0 )
	{
		UINT32 _minimumProtocol =  atoi( optValue );
		if (_minimumProtocol >= MIN_SUPPORTED_PROTOCOL && _minimumProtocol <= MAX_SUPPORTED_PROTOCOL)
		{
			minimumProtocol = _minimumProtocol;
			if (DefaultProtocol < minimumProtocol)
				DefaultProtocol = minimumProtocol;
		}
	}
#endif
	else if ( strcmp( optName, COMM_OPTION_PROTOCOL_LOG_FILE ) == 0 && comm_logging == 0)
		comm_logging = new _CommProtocolLog(optValue,PLogFile::perDateLimited);
	else if( strcmp( optName, COMM_OPTION_CLIENT_ENCRYPTION ) == 0 )
		enableClientEncryption =  atoi( optValue ) != 0;
#ifndef COMM_CLIENT_ONLY
	else if( strcmp( optName, COMM_OPTION_MAXIMUM_MESSAGE_SIZE ) == 0 )
		maximumMessageSize =  atoi( optValue );
	else if( strcmp( optName, COMM_OPTION_MAXIMUM_MESSAGE_RATE ) == 0 )
		maximumMessageRate =  atoi( optValue );
	else if( strcmp( optName, COMM_OPTION_MAXIMUM_MESSAGE_RATE_PERIOD ) == 0 )
		maximumMessageRatePeriod =  atoi( optValue );
	else if( strcmp( optName, COMM_OPTION_MAXIMUM_MESSAGE_SIZE_DEBUG ) == 0 )
		maxMessageSizeDebug =  atoi( optValue ) != 0;
	else if( strcmp( optName, COMM_OPTION_MAXIMUM_MESSAGE_RATE_DEBUG ) == 0 )
		maxMessageRateDebug =  atoi( optValue ) != 0;
	else if( strcmp( optName, COMM_OPTION_SERVER_ENCRYPTION ) == 0 )
	{	
		enableServerEncryption = true;
		sscanf(optValue,"%x,%x,%x,%x,%x,%x,%x,%x,%x,%x",&publicKey[0],&publicKey[1],&publicKey[2],&publicKey[3],&publicKey[4],
			&privateKey[0],&privateKey[1],&privateKey[2],&privateKey[3],&privateKey[4]);
	}
#endif
	else
		CommThreadManager::setOption( optName, optValue );
}

void CommSimpleThreadManagerBase::_freeSocket( int threadIdx )
{
	PWLock lock( cs );
	Thread& thread = threads[ threadIdx ];

	delete thread.thread;
	thread.thread = 0;
}

int CommSimpleThreadManagerBase::_createThread( UINT32& channelId, _CommPhysicalConnection* physConn, const char* addr, int masterIdx, UINT32 masterChannelId )
//MUST be locked before call
{
	int threadIdx = _allocThread();

	Thread& thread = threads[ threadIdx ];
	_CommSimpleThreadBase* th = _createNewThread( threadIdx, physConn );//new _CommSimpleThread< _CommSimplePlain >( *this, threadIdx, physConn, nextChannelId() );
	channelId = th->channelId;
	th->address = addr;
	//	th->httpsProxy = httpsProxy;
	//	th->httpsProxyPort = httpsProxyPort;
	//	th->freezeReconnectTimeout = freezeReconnectTimeout;

	th->masterIdx = masterIdx;
	th->masterChannelId = masterChannelId;
	th->slaveIdx = -1;
	th->slaveChannelId = 0;
	thread.thread = th;
	thread.maxSlots = 1;
	thread.slotsLeft = 0;
	thread.thread->start();
	return threadIdx;
}

void CommSimpleThreadManagerBase::connect( _CommPhysicalConnection* physConn )
{
	PWLock lock( cs );
	UINT32 slaveChannelId;//dummy
	int threadIdx = _createThread( slaveChannelId, physConn, physConn->address, -1, 0 );
	PTRACE3( "Started Simple thread #%d", threadIdx );
}

void CommSimpleThreadManagerBase::createSlaveThread( int& slaveIdx, UINT32& slaveChannelId, const char* addr, int masterIdx, UINT32 masterChannelId )
{
	PWLock lock( cs );
	int threadIdx = _createThread( slaveChannelId, 0, addr, masterIdx, masterChannelId );
	slaveIdx = threadIdx;
	PTRACE3( "Started Slave Simple thread #%d (master=%d)", threadIdx, masterIdx );
}

void CommSimpleThreadManagerBase::pushToThread( int slaveIdx, _CommMsg& m )
{
	PWLock lock( cs );
	_CommManageableThread* thread = threads[ slaveIdx ].thread;
	if( !thread )
		return;

	m.priority = _COMM_MSGPRIORITIES - 1;
	thread->push( m );
}

void CommSimpleThreadManagerBase::forceDisconnect( _CommPhysicalConnection* physConn )
{
	PWLock lock( cs );
	Thread& th = threads[ physConn->thread ];
	_CommSimpleThreadBase* thread = static_cast< _CommSimpleThreadBase* >( th.thread );//safe because all Thread.threads are created by CommSimpleThreadManager
	if (thread)
		thread->forceDisconnect();
}

void CommSimpleThreadManagerBase::_debugEmulateNetworkFailure()
{
	PWLock lock( cs );
	int n = threads.size();
	for( int i=0; i < n ; ++i )
	{
		Thread& th = threads[ i ];
		_CommSimpleThreadBase* thread = static_cast< _CommSimpleThreadBase* >( th.thread );//safe because all Thread.threads are created by CommSimpleThreadManager
		if( thread )
			thread->_debugEmulateNetworkFailure();
	}
}

