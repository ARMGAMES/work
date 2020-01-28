#include "ppinclude.h"
#include "ppthread.h"
#include "ppsocket.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "commrtable.h"
#include "commtmrw.h"
#include "_commtcp0.h"
#include "_commtmrw.h"
#include "commoption.h"
#include "commmonitorlogger.h"




class _CommRWAcceptThreadIP : public _CommRWAcceptThread
{
private:
	//	bool exited;
	CommRWThreadManagerBaseIP& mgr;
	PIPListeningSocket sock;
	bool shutdownFlag;
	bool exitFlag;


public:
	_CommRWAcceptThreadIP( CommRWThreadManagerBaseIP& mgr_, int port );
	void run() override;
	void shutdown() override;
	void threadState(CommMsgBody &body) override
	{
		char buff[20];

		PIPSocket::Descriptor rt;
		sock.copyDescriptor(rt);
		sprintf(buff,"%d",PIPSocket::port( rt ));
		composeLoggerStat(body, buff, exitFlag);
	}

};



//****************************************************************************

_CommRWAcceptThreadIP::_CommRWAcceptThreadIP( CommRWThreadManagerBaseIP& mgr_, int port )
: mgr( mgr_ )
{
	//	exited = false;
	shutdownFlag=false;
	exitFlag= false;
	sock.bind( port );
}

/* virtual */ void _CommRWAcceptThreadIP::run()
{
	try
	{

		PTRACE3( "CommRWAcceptThreadIP: listen( %d )", mgr.listenBacklog );
		if( mgr.inTcpBufferRcv != -1 )
		{
			PTRACE3( "RWAcceptThreadIP: Set TCP recv buffer = %d", mgr.inTcpBufferRcv );
			bool Ok = sock.tcpSetRecvBuffer( mgr.inTcpBufferRcv);
			if( !Ok )
				PLog( "RWAcceptThreadIP: Unable to set TCP recv buffer %d", mgr.inTcpBufferRcv );
		}
		if( mgr.inTcpBufferSnd != -1 )
		{
			PTRACE3( "RWAcceptThreadIP: Set TCP send buffer = %d", mgr.inTcpBufferSnd );
			bool Ok = sock.tcpSetSendBuffer( mgr.inTcpBufferSnd);
			if( !Ok )
				PLog( "RWAcceptThreadIP: Unable to set TCP send buffer %d", mgr.inTcpBufferSnd );
		}

		sock.listen( mgr.listenBacklog );
		for(;;)
		{
			PIPSocket::Descriptor accepted;
			sock.accept( accepted );
			if (shutdownFlag)
			{
				PTRACE3( "CommRWAcceptThreadIP: thread shutdown");
				break;
			}
			if (mgr._allowNewConnection())
			{
				if( mgr.inTcpNoDelay )
				{
					PTRACE3( "RWAcceptThreadIP: Disabling TCP delay" );
					bool Ok = PIPSocket::tcpNoDelay( accepted.sock );
					if( !Ok )
						PLog( "RWAcceptThreadIP: Unable to disable TCP delay" );
				}
				if( mgr.inTcpBufferRcv != -1 )
				{
					PTRACE3( "RWAcceptThreadIP: Set TCP recv buffer = %d", mgr.inTcpBufferRcv );
					bool Ok = sock.tcpSetRecvBuffer( mgr.inTcpBufferRcv);
					if( !Ok )
						PLog( "RWAcceptThreadIP: Unable to set TCP recv buffer %d", mgr.inTcpBufferRcv );
				}
				if( mgr.inTcpBufferSnd != -1 )
				{
					PTRACE3( "RWAcceptThreadIP: Set TCP send buffer = %d", mgr.inTcpBufferSnd );
					bool Ok = sock.tcpSetSendBuffer( mgr.inTcpBufferSnd);
					if( !Ok )
						PLog( "RWAcceptThreadIP: Unable to set TCP send buffer %d", mgr.inTcpBufferSnd );
				}


				PString addr;
				PIPSocket::address( addr, accepted );
				int port = PIPSocket::port( accepted );

				PString addr2;
				mgr.composeSocketAddr( addr2, addr, port );
				PLog( "Incoming connection from '%s'", addr2.c_str() );
				mgr._pushInSocket( accepted, addr2 );
			}
			else
			{
				PTRACE3( "RWAcceptThreadIP: number of connections exceeds limit" );
				PIPSocket sockToClose(accepted);
				sockToClose.close();
			}
		}
	}
	catch( PError& err )
	{
		PLog( "RWAcceptThreadIP exception: %s", err.why() );
	}
	exitFlag = true;
	//	exited = true;
}

void _CommRWAcceptThreadIP::shutdown()
{
	if (shutdownFlag == false)
	{
		sock.close();
	}
	shutdownFlag=true;
}

//****************************************************************************

_CommRWConnectThreadIP::_CommRWConnectThreadIP( CommRWThreadManagerBaseIP& mgr_ )
: mgr( mgr_ )
{
	//	exited = false;
}

/* virtual */ void _CommRWConnectThreadIP::run()
{
	try
	{
		for(;;)
		{
			_CommPhysicalConnection* conn;
			int ok = q.waitPop( conn );
			switch( ok )
			{
			case Queue::waitPopOk:
				_connect( conn );
				break;
			case Queue::waitPopShutdown:
				//					exited = true;
				return;
			default:
				PASSERT3( 0 );
				break;
			}
		}
	}
	catch( PError& err )
	{
		PLog( "RWConnectThread exception: %s", err.why() );
	}
	//	exited = true;
}

void _CommRWConnectThreadIP::_connect( _CommPhysicalConnection* conn )
{
	if (mgr.connectIsOutOfProcess())
	{
		PString addr;
		int port;
		mgr.parseSocketAddr( addr, port, conn->address );
		CommRWThreadManagerBase::Socket rwSock;
		PIPSocket::composeDescriptor(rwSock.sockDescr, port,addr);
		rwSock.physConn = conn;
		mgr._pushOutSocket( rwSock );
	}
	else
	{
		try
		{
			//resolve conn->address
			PString addr;
			int port;
			mgr.parseSocketAddr( addr, port, conn->address );

			PIPSocket sock;
			if( mgr.outTcpNoDelay )
			{
				PTRACE3( "RWConnectThread: Disabling TCP delay" );
				bool Ok = sock.tcpNoDelay();
				if( !Ok )
					PLog( "RWConnectThread: Unable to disable TCP delay" );
			}
			if( mgr.outTcpBufferRcv != -1 )
			{
				PTRACE3( "RWConnectThread: Set recv TCP buffer = %d", mgr.outTcpBufferRcv );
				bool Ok = sock.tcpSetRecvBuffer(mgr.outTcpBufferRcv);
				if( !Ok )
					PLog( "RWConnectThread: Unable to set recv TCP buffer %d",mgr.outTcpBufferRcv );
			}
			if( mgr.outTcpBufferSnd != -1 )
			{
				PTRACE3( "RWConnectThread: Set snd TCP buffer = %d", mgr.outTcpBufferSnd );
				bool Ok = sock.tcpSetSendBuffer(mgr.outTcpBufferSnd);
				if( !Ok )
					PLog( "RWConnectThread: Unable to set snd TCP buffer %d",mgr.outTcpBufferSnd );
			}
			if (mgr.tcpConnectTimeout)
			{
				if(!sock.connect( addr, port,mgr.tcpConnectTimeout * TICKS_PER_SECOND ))
					throw PError("Connect timed out");
			}
			else
				sock.connect( addr, port );
			CommRWThreadManagerBase::Socket rwSock;
			sock.extractDescriptor( rwSock.sockDescr );
			rwSock.physConn = conn;
			mgr._pushOutSocket( rwSock );
		}
		catch( PError& err )
		{
			PLog( "RWConnect to '%s' failed: %s", conn->address.c_str(), err.why() );
			mgr.routingTable.physicalConnectFailed( conn );
		}
	}
}

//****************************************************************************

#ifdef PPOSIX
class _CommRWAcceptThreadUnix : public _CommRWAcceptThread
{
private:
	//	bool exited;
	CommRWThreadManagerUnix& mgr;
	PUnixListeningSocket sock;
	bool shutdownFlag;
	bool exitFlag;


public:
	_CommRWAcceptThreadUnix( CommRWThreadManagerUnix& mgr_, const char * address );
	void run() override;
	void shutdown() override;
	void threadState(CommMsgBody &body) override
	{
		PUnixSocket::Descriptor rt;
		sock.copyDescriptor(rt);
		PString address;
		sock.address(address);
		composeLoggerStat(body, address.c_str(), exitFlag);
	}
};



class _CommRWConnectThreadUnix : public _CommRWConnectThread
{
private:
	CommRWThreadManagerUnix& mgr;

public:
	_CommRWConnectThreadUnix( CommRWThreadManagerUnix& mgr_ );
public:
	void run() override;
private:
	void _connect( _CommPhysicalConnection* conn );
};

//****************************************************************************

_CommRWAcceptThreadUnix::_CommRWAcceptThreadUnix( CommRWThreadManagerUnix& mgr_, const char * address )
: mgr( mgr_ )
{
	//	exited = false;
	shutdownFlag=false;
	exitFlag= false;
	sock.bind( address );
}

/* virtual */ void _CommRWAcceptThreadUnix::run()
{
	try
	{

		PTRACE3( "_CommRWAcceptThreadUnix: listen( %d )", mgr.listenBacklog );
		sock.listen( mgr.listenBacklog );
		for(;;)
		{
			PUnixSocket::Descriptor accepted;
			sock.accept( accepted );
			if (shutdownFlag)
			{
				PTRACE3( "_CommRWAcceptThreadUnix: thread shutdown");
				break;
			}
			if (mgr._allowNewConnection())
			{
				PString addr;
				PUnixSocket::address( addr, accepted );
				PString addr2;
				mgr.composeSocketAddr( addr2, addr, 0 );
				PLog( "_CommRWAcceptThreadUnix::Incoming connection from '%s'", addr2.c_str() );
				mgr._pushInSocket( accepted, addr2 );
			}
			else
			{
				PTRACE3( "_CommRWAcceptThreadUnix: number of connections exceeds limit" );
				PUnixSocket sockToClose(accepted);
				sockToClose.close();
			}
		}
	}
	catch( PError& err )
	{
		PLog( "_CommRWAcceptThreadUnix exception: %s", err.why() );
	}
	exitFlag = true;
	//	exited = true;
}

void _CommRWAcceptThreadUnix::shutdown()
{
	if (shutdownFlag == false)
	{
		sock.close();
	}
	shutdownFlag=true;
}

//****************************************************************************

_CommRWConnectThreadUnix::_CommRWConnectThreadUnix( CommRWThreadManagerUnix& mgr_ )
: mgr( mgr_ )
{
	//	exited = false;
}

/* virtual */ void _CommRWConnectThreadUnix::run()
{
	try
	{
		for(;;)
		{
			_CommPhysicalConnection* conn;
			int ok = q.waitPop( conn );
			switch( ok )
			{
			case Queue::waitPopOk:
				_connect( conn );
				break;
			case Queue::waitPopShutdown:
				//					exited = true;
				return;
			default:
				PASSERT3( 0 );
				break;
			}
		}
	}
	catch( PError& err )
	{
		PLog( "_CommRWConnectThreadUnix exception: %s", err.why() );
	}
	//	exited = true;
}

void _CommRWConnectThreadUnix::_connect( _CommPhysicalConnection* conn )
{
	try
	{
		//resolve conn->address
		PString addr;
		int dummy;
		mgr.parseSocketAddr( addr, dummy, conn->address );
		PUnixSocket sock;

		sock.connect( addr );
		CommRWThreadManagerBase::Socket rwSock;
		sock.extractDescriptor( rwSock.sockDescr );
		rwSock.physConn = conn;
		mgr._pushOutSocket( rwSock );
	}
	catch( PError& err )
	{
		PLog( "_CommRWConnectThreadUnix to '%s' failed: %s", conn->address.c_str(), err.why() );
		mgr.routingTable.physicalConnectFailed( conn );
	}
}
#endif
//****************************************************************************

_CommRWThreadBase::_CommRWThreadBase( CommRWThreadManagerBase& mgr_, int mgrIdx_, bool enableMux )
: mgr( mgr_ )
{
	muxEnabled = enableMux;
	mgrIdx = mgrIdx_;
}

void _CommRWThreadBase::pushSocket( const CommRWThreadManagerBase::Socket& sock )
{
	_CommMsg m( _COMM_MSGTYPE_NONE, _COMM_MSGPRIORITY_INITCONNECT );
	m.internal.slot = 0xFFFF;
	m.body._composeFixedBlock( (const BYTE*)&sock, sizeof(CommRWThreadManagerBase::Socket) );
	q.push( m );
}

void _CommRWThreadBase::pushForceDisconnect( int slot, UINT32 channelId )
{
	_CommMsg m( _COMM_MSGTYPE_NONE, _COMM_MSGPRIORITIES - 1 );
	PASSERT3( slot != 0xFFFF );
	m.internal.slot = slot;
	m.internal.channelId = channelId;
	q.push( m );
}

//****************************************************************************

inline /* static */ _CommRWThreadBase* CommRWThreadManagerBase::_rwThread( Thread& thread )
{
	return static_cast< _CommRWThreadBase* >( thread.thread );//safe, because all RWThreadManager threads are in fact RWThreads
}

CommRWThreadManagerBase::CommRWThreadManagerBase( CommRoutingTable& routingTable_, bool enableMux )
: CommThreadManager( routingTable_ )
{
	started = false;
	muxEnabled = enableMux;
	isFar = true;
	keepAliveWrite = keepAliveRead = 0;
	keepAliveIgnoreDataPackets=false;
	listenBacklog = 25;//default
	inTcpNoDelay = false;
	outTcpNoDelay = false;
	inTcpBufferRcv = -1;
	inTcpBufferSnd = -1;
	outTcpBufferRcv = -1;
	outTcpBufferSnd = -1;
	optimizePacketSize = false;
	CompressBitsSize = DEFAULT_COMPRESS_BITS_SIZE;
	MaxBlockSize = DEFAULT_FRAME_SIZE;
	MaxBlockSizeVer5 = DEFAULT_FRAME_SIZE_VER5;
	DefaultProtocol = DEFAULT_PROTOCOL;
	
	comm_logging = 0;
	maxNumberOfConnections=0;
#ifndef COMM_CLIENT_ONLY
	maximumMessageSize = 0;
	maximumMessageRate = 0;
	maximumMessageRatePeriod = 0;
	maxMessageSizeDebug = false; 
	maxMessageRateDebug = true; 
	minimumProtocol = MIN_SUPPORTED_PROTOCOL;
	enableServerEncryption = false;
	minEnforcedBlock = 0;
	maxEnforcedBlock = 0;
	randomProtectionEnforced=0;

#endif
	tcpConnectTimeout = 0;
	handshakeTimeout = DEFAULT_TIMEOUT_FOR_CONNECTING;
	enableClientEncryption = DEFAULT_CLIENT_ENCRYPTION;
	connThread = 0;
}



CommRWThreadManagerBaseIP::CommRWThreadManagerBaseIP( CommRoutingTable& routingTable_, bool enableMux )
: CommRWThreadManagerBase( routingTable_,  enableMux)
{
	
}

#ifdef PPOSIX
CommRWThreadManagerUnix::CommRWThreadManagerUnix( CommRoutingTable& routingTable_, bool enableMux )
: CommRWThreadManagerBase( routingTable_,  enableMux)
{
	connThread = new _CommRWConnectThreadUnix( *this );
}
#endif

void CommRWThreadManagerBaseIP::listenPort( int port )
{
	PWLock lock( cs );
	_CommRWAcceptThread* acceptThread = new _CommRWAcceptThreadIP( *this, port );
	acceptThreads.push_back( acceptThread );
	listenPorts.push_back(port);
}
#ifdef PPOSIX

void CommRWThreadManagerUnix::listenName( const char * port )
{
#ifdef LINUX
	PWLock lock( cs );
#else
	PLock lock( cs );
#endif
	_CommRWAcceptThread* acceptThread = new _CommRWAcceptThreadUnix( *this, port );
	acceptThreads.push_back( acceptThread );
	listenPorts.push_back(PString(port));
}
#endif
/* virtual */ void CommRWThreadManagerBase::setOption( const char* optName, const char* optValue )
{
	if( strcmp( optName, COMM_OPTION_KEEPALIVE ) == 0 )
	{
		const char* sl = strchr( optValue, '/' );
		if( sl == 0 )
		{
			PLog( "CommRWThreadManagerBase: Invalid COMM_OPTION_KEEPALIVE option - no '/' found" );
			return;
		}
		keepAliveWrite = atoi( optValue );
		keepAliveRead = atoi( sl + 1 );
		sl = strchr( sl + 1, '/' );
		if (sl)
		{
			keepAliveIgnoreDataPackets=!!atoi(sl+1);
		}
	}
	else if( strcmp( optName, COMM_OPTION_ISLOCAL ) == 0 )
		isFar = atoi( optValue ) == 0;
	else if( strcmp( optName, COMM_OPTION_LISTENBACKLOG ) == 0 )
		listenBacklog = atoi( optValue );
	else if( strcmp( optName, COMM_OPTION_INTCPNODELAY ) == 0 )
		inTcpNoDelay = atoi( optValue ) != 0;
	else if( strcmp( optName, COMM_OPTION_OUTTCPNODELAY ) == 0 )
		outTcpNoDelay = atoi( optValue ) != 0;
	else if( strcmp( optName, COMM_OPTION_INTCPBUFFER ) == 0 )
	{
		inTcpBufferRcv = inTcpBufferSnd = atoi( optValue );
	}
	else if( strcmp( optName, COMM_OPTION_INTCPBUFFER_RCV ) == 0 )
	{
		inTcpBufferRcv = atoi( optValue );
	}
	else if( strcmp( optName, COMM_OPTION_INTCPBUFFER_SND ) == 0 )
	{
		inTcpBufferSnd = atoi( optValue );
	}
	else if( strcmp( optName, COMM_OPTION_OUTTCPBUFFER ) == 0 )
	{
		outTcpBufferRcv = outTcpBufferSnd = atoi( optValue );
	}
	else if( strcmp( optName, COMM_OPTION_OUTTCPBUFFER_RCV ) == 0 )
	{
		outTcpBufferRcv = atoi( optValue );
	}
	else if( strcmp( optName, COMM_OPTION_OUTTCPBUFFER_SND ) == 0 )
	{
		outTcpBufferSnd = atoi( optValue );
	}
	else if( strcmp( optName, COMM_OPTION_OPTIMIZEPACKETSIZE ) == 0 )
		optimizePacketSize = atoi( optValue ) != 0;
	else if( strcmp( optName, COMM_OPTION_FRAMESIZE ) == 0 )
		MaxBlockSize =  atoi( optValue );
	else if( strcmp( optName, COMM_OPTION_FRAMESIZE_VER5 ) == 0 )
		MaxBlockSizeVer5 =  atoi( optValue );
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
	else if( strcmp( optName, COMM_OPTION_BLOCK_ENFORCED ) == 0 )
	{
		const char* sl = strchr( optValue, ',' );
		if( sl == 0 )
		{
			minEnforcedBlock = maxEnforcedBlock = atoi( optValue );
			randomProtectionEnforced = 0;
		}
		else
		{
			const char* sl1 = strchr( sl+1, ',' );
			if (sl1 == 0)
			{
				minEnforcedBlock = atoi( optValue );
				maxEnforcedBlock = atoi( sl + 1 );
				randomProtectionEnforced = 0;
			}
			else
			{
				minEnforcedBlock = atoi( optValue );
				maxEnforcedBlock = atoi( sl + 1 );
				randomProtectionEnforced = atoi( sl1 + 1 );
			}
		}
	}
#endif
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
	else if ( strcmp( optName, COMM_OPTION_PROTOCOL_LOG_FILE ) == 0 && comm_logging == 0)
		comm_logging = new _CommProtocolLog(optValue,PLogFile::perDate|PLogFile::printTimePerLine);
	else if( strcmp( optName, COMM_OPTION_CLIENTDISCONNECT ) == 0 )
		forceClientDisconnect = atoi( optValue ) != 0;
	else if( strcmp( optName, COMM_OPTION_ALLOWCLIENTDISCONNECT ) == 0 )
		allowClientDisconnect = atoi( optValue ) != 0;
	else if( strcmp( optName, COMM_OPTION_TCPCONNECT ) == 0 )
		tcpConnectTimeout = atoi( optValue );
	else if( strcmp( optName, COMM_OPTION_HANDSHAKE_TIMEOUT ) == 0 )
		handshakeTimeout = atoi( optValue );
	else
		CommThreadManager::setOption( optName, optValue );
}

/* virtual */ void CommRWThreadManagerBase::dynamicInit(PIniFile& iniFile)
{
	PIniFile::Section* section = iniFile.getSection( managerType.c_str() );
	if( section )
	{
		const char *priority = section->getProperty(COMM_OPTION_ACCEPTTHREADPRIORITY);
		if (priority)
		{
			PWLock lock( cs );
			int n = acceptThreads.size();
			for( int i=0; i < n ; ++i )
				acceptThreads[ i ]->setThreadPriority(priority);
			acceptThreadPriority.assign(priority);
		}
		priority = section->getProperty(COMM_OPTION_CONNECTTHREADPRIORITY);
		if (priority)
		{
			PWLock lock( cs );
			connThread->setThreadPriority(priority);
			connectThreadPriority.assign(priority);
		}
		maxNumberOfConnections=section->getIntProperty(COMM_OPTION_MAXNUMBEROFCONNECTIONS,0);
	}
	CommThreadManager::dynamicInit(iniFile);
}

void CommRWThreadManagerBase::start()
{
	PWLock lock( cs );
	PASSERT3( !started );
	if (connThread)
	{
		connThread->start();
		if (*connectThreadPriority.c_str())
		{
			connThread->setThreadPriority(connectThreadPriority);
		}
	}

	int n = acceptThreads.size();
	for( int i=0; i < n ; ++i )
	{
		acceptThreads[ i ]->start();
		if (*acceptThreadPriority.c_str())
		{
			acceptThreads[i]->setThreadPriority(acceptThreadPriority);
		}
	}

	started = true;
}

void CommRWThreadManagerBase::shutdown()
{
	PWLock lock( cs );
	if (connThread)
		connThread->shutdown();
	int n = acceptThreads.size();
	for( int i=0; i < n ; ++i )
	{
		acceptThreads[ i ]->shutdown();
	}
	_Parent::shutdown();
}

void CommRWThreadManagerBaseIP::acceptCancel()
{
	PWLock lock( cs );
	PTRACE3( "%s acceptCancel() - acceptState=%s",whoAreYou(),inStateOfClosedAccept?"true":"false");
	if (inStateOfClosedAccept)
		return;
	int n = acceptThreads.size();
	for( int i=0; i < n ; ++i )
	{
		acceptThreads[ i ]->shutdown();
	}
	inStateOfClosedAccept = true;
}
#ifdef PPOSIX
void CommRWThreadManagerUnix::acceptCancel()
{
#ifdef LINUX
	PWLock lock( cs );
#else
	PLock lock( cs );
#endif

	PTRACE3( "%s acceptCancel() - acceptState=%s",whoAreYou(),inStateOfClosedAccept?"true":"false");
	if (inStateOfClosedAccept)
		return;
	int n = acceptThreads.size();
	for( int i=0; i < n ; ++i )
	{
		acceptThreads[ i ]->shutdown();
	}
	inStateOfClosedAccept = true;
}
#endif
bool CommRWThreadManagerBaseIP::acceptReinstate()
{
	PWLock lock( cs );
	PTRACE3( "%s acceptReinstate() - acceptState=%s",whoAreYou(),inStateOfClosedAccept?"true":"false");
	if (inStateOfClosedAccept == false)
		return true;
	int n = listenPorts.size();
	int i;
	for( i=0; i < n ; ++i )
	{
		if (acceptThreads[ i ]->terminated()==false)
		{
			PTRACE3( "%s acceptReinstate() - thread not terminated",whoAreYou());
			return false;
		}
	}

	for( i=0; i < n ; ++i )
	{
		delete acceptThreads[ i ];
	}
	acceptThreads.clear();
	for( i=0; i < n ; ++i )
	{
		_CommRWAcceptThread* acceptThread = new _CommRWAcceptThreadIP( *this, listenPorts[i] );
		acceptThreads.push_back( acceptThread );
		acceptThread->start();
		if (*acceptThreadPriority.c_str())
		{
			acceptThread->setThreadPriority(acceptThreadPriority.c_str());
		}
	}
	inStateOfClosedAccept = false;
	return true;
}
#ifdef PPOSIX
bool CommRWThreadManagerUnix::acceptReinstate()
{
#ifdef LINUX
	PWLock lock( cs );
#else
	PLock lock( cs );
#endif

	PTRACE3( "%s acceptReinstate() - acceptState=%s",whoAreYou(),inStateOfClosedAccept?"true":"false");
	if (inStateOfClosedAccept == false)
		return true;
	int n = listenPorts.size();
	int i;
	for( i=0; i < n ; ++i )
	{
		if (acceptThreads[ i ]->terminated()==false)
		{
			PTRACE3( "%s acceptReinstate() - thread not terminated",whoAreYou());
			return false;
		}
	}

	for( i=0; i < n ; ++i )
	{
		delete acceptThreads[ i ];
	}
	acceptThreads.clear();
	for( i=0; i < n ; ++i )
	{
		_CommRWAcceptThread* acceptThread = new _CommRWAcceptThreadUnix( *this, listenPorts[i] );
		acceptThreads.push_back( acceptThread );
		acceptThread->start();
		if (*acceptThreadPriority.c_str())
		{
			acceptThread->setThreadPriority(acceptThreadPriority.c_str());
		}
	}
	inStateOfClosedAccept = false;
	return true;
}
#endif
bool CommRWThreadManagerBase::terminated()
{
	PWLock lock( cs );
	if( !_Parent::terminated() )
		return false;
	if (connThread)
	{
		if( !connThread->terminated() )
			return false;
	}
	int n = acceptThreads.size();
	for( int i=0; i < n ; ++i )
		if( !acceptThreads[ i ]->terminated() )
			return false;
	return true;
}

CommRWThreadManagerBase::~CommRWThreadManagerBase()
{
	PWLock lock( cs );
	delete connThread;
	int n = acceptThreads.size();
	for( int i=0; i < n ; ++i )
		delete acceptThreads[ i ];
	if(comm_logging)
		delete comm_logging;
}

void CommRWThreadManagerBase::_pushInSocket( const PIPSocket::Descriptor& sockDescr, const char* addr )
{
	CommRWThreadManagerBase::Socket sock;
	sock.sockDescr = sockDescr;
	sock.physConn = routingTable.createIncomingPhysicalConnection( addr, this );
	_pushSocket( sock );
}

void CommRWThreadManagerBase::_pushSocket( const CommRWThreadManagerBase::Socket& sock )
{
	PWLock lock( cs );

	int threadIdx = _findSlot();
	if( threadIdx >= 0 )
	{
		Thread& thread = threads[ threadIdx ];
		_rwThread( thread )->pushSocket( sock );
		--thread.slotsLeft;
		PTRACE3( "Socket pushed to existing thread #%d; --slotsLeft=%d", threadIdx, thread.slotsLeft );
		return;
	}

	threadIdx = _allocThread();
	Thread& thread = threads[ threadIdx ];
	thread.thread = _createNewThread( threadIdx );
	thread.slotsLeft = thread.maxSlots = _rwThread( thread )->maxRWSockets();
	thread.thread->start();
	if (*baseThreadPriority.c_str())
	{
		thread.thread->setThreadPriority(baseThreadPriority.c_str());
	}
	PTRACE3( "Started RW thread #%d", threadIdx );

	_rwThread( thread )->pushSocket( sock );
	--thread.slotsLeft;
	PTRACE3( "Socket pushed to thread #%d", threadIdx );
}

bool CommRWThreadManagerBase::_allowNewConnection()
{
	if (maxNumberOfConnections)
	{
		int numberOfConnections = _numberOfConnections();
		return numberOfConnections < maxNumberOfConnections;
	}
	else
	{
		return true;
	}
}

int CommRWThreadManagerBase::_numberOfConnections()
{
	PWLock lock( cs );
	int numberOfConnections=0;
	int n = threads.size();
	for( int i=0; i < n ; ++i )
	{
		Thread& thread = threads[ i ];
		numberOfConnections += (thread.maxSlots - thread.slotsLeft);
	}
	return numberOfConnections;
}

bool CommRWThreadManagerBase::_freeSocket( int threadIdx )//no thread operations allowed after freeSocket returned true//no thread operations allowed after freeSocket returned true
{
	PWLock lock( cs );
	Thread& thread = threads[ threadIdx ];
	if( ++thread.slotsLeft != thread.maxSlots )
		return false;

	//{+++ to preserve some threads for the future use - return false
	//}

	//	delete thread.thread;
	//	thread.thread = 0;
	thread.thread->exit();
	return true;
}

void CommRWThreadManagerBase::connect( _CommPhysicalConnection* physConn )
{
	if(connThread)
		connThread->pushConnection( physConn );
	else
		throw PError("Thread manager does not support connection");
}

void CommRWThreadManagerBase::forceDisconnect( _CommPhysicalConnection* physConn )
{
	PWLock lock( cs );
	Thread& thread = threads[ physConn->thread ];
	_rwThread( thread )->pushForceDisconnect( physConn->slot, physConn->channelId );
}

void CommRWThreadManagerBase::loggerInformation(CommMsgBody &body)
{
	PWLock lock( cs );
	vector< /* new */ _CommRWAcceptThread* > :: iterator it;
	for (it=acceptThreads.begin(); it != acceptThreads.end();++it)
	{
		(*it)->threadState(body);
	}
	int num = _numberOfConnections();
	PString s;
	s.assign("Connections(").append(managerType.c_str()).append(")");
	composeLoggerStat(body, s.c_str(), num);

}

CommRWThreadManager::CommRWThreadManager( CommRoutingTable& routingTable_, bool enableMux) : CommRWThreadManagerBaseIP( routingTable_, enableMux )
{
	connThread = new _CommRWConnectThreadIP(*this);
}



