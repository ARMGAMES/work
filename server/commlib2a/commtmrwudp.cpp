
// Use _WIN32 instead of PWIN32 here. 
#ifdef _WIN32

#include <winsock2.h>
#include "commtmrwudp.h"
#include "_commudp0.h"
#include "commoption.h"
#include "commmonitorlogger.h"

#define NUM_WSABUF 5

struct CommRWUDPThreadManager::UdpCommBuffer
{
	OVERLAPPED over;
	UINT32     type;
	BYTE * buf;
	size_t size;
	WSABUF DataBuf;

	struct sockaddr addr;
	int addrSize;
	
	UdpCommBuffer()
	{
		type = 1;
		buf = new BYTE[UDP_TOTAL_MAXIMUM_PACKET_SIZE];
		size = 0;
		memset( &over, 0, sizeof(OVERLAPPED) );
		memset( &addr, 0, sizeof(addr) );
		addrSize = sizeof(addr);
		DataBuf.buf = (char *)buf;
		DataBuf.len = UDP_TOTAL_MAXIMUM_PACKET_SIZE;
		PTRACE5("UdpCommBuffer %p", this);
	}
	~UdpCommBuffer()
	{
		delete [] buf;
	}
};

class _CommMultiUDPCommunicator
{
public:
	OVERLAPPED over;
	UINT32     type;


	CommRoutingTable * routingTable;
	PUDPSocket* sock;
	PUDPSocket::Descriptor * peerDescr;
	_CommPhysicalConnection* physConn;



//	struct sockaddr addr;
//	int addrSize;
	bool sendBusy;
	WSABUF data[NUM_WSABUF];
	size_t written;

	CommRWUDPThreadManager::UdpCommBuffer * readReady;
	void * key;


public:
	_CommMultiUDPCommunicator()
	{
		type = 2;
		PTRACE5("_CommMultiUDPCommunicator %p", this);
	}
	void construct( CommRoutingTable& routingTable_, PUDPSocket& sock_, PUDPSocket::Descriptor & peerDescr_,_CommPhysicalConnection & physConn_)
	{
		sock = &sock_;
		routingTable = &routingTable_;
		sendBusy = false;
		written = 0;
		peerDescr = &peerDescr_;
		physConn = &physConn_;
	}
	void init()
	{
	}


	BYTE * asyncReadOk(size_t & size)
	{
		if (readReady)
		{
			size = readReady->size;
			return readReady->buf;
		}
		return 0;
	}


	bool isReadReady() const
	{
		return !!readReady;
	}


	void * asyncWriteOk(size_t &size)
	{
		PASSERT3(sendBusy);
		size = written;
		sendBusy = false;
		return key;
	}

	void msg( _CommMsg& msg )
	{
		routingTable->routeIncomingMessage( physConn, 0, 0, msg );
	}

	void serviceCall(UINT32 id, CommMsgBody * msg_body)
	{
		routingTable->serviceCall(physConn,id,msg_body);
	}

	WSABUF * getSendBuffer()
	{
		if (sendBusy)
			return 0;
		else
		{
			sendBusy = true;
			return data;
		}
	}

	size_t send(WSABUF * buf, int n, void *key_)
	{
		PASSERT3(sendBusy);
		PASSERT3(n<NUM_WSABUF);

		key = key_;
		DWORD flags = 0;
		DWORD retb;
		memset( &over, 0, sizeof(OVERLAPPED) );
		WSASendTo( sock->getSocket(), buf, n, &retb, flags, &peerDescr->addr, sizeof(struct sockaddr), &over, NULL );
		return 0;
	}

};


struct CommRWUDPThreadManager::Peer
{
	_CommMultiUDPCommunicator commBase;
	PCriticalSection cps;
	CommRWUDPThreadManager & mgr;
	PUDPSocket       * sock;
	PUDPSocket::Descriptor peerDescr;
	_CommPhysicalConnection * physConn;
	_CommUDP0ProtocolHandler<_CommMultiUDPCommunicator,_CommMultiUDPCommunicator> comm;
	int slot;
	int mgrIdx;
	bool connected;
	bool exited;
	bool deleted;
	UINT32 channelId;


	Peer(CommRWUDPThreadManager &mgr_,const PUDPSocket::Descriptor &peerDescr_,PUDPSocket * sock_):
		mgr(mgr_), peerDescr(peerDescr_)
	{
		sock = sock_;
		slot = -1;
		mgrIdx = -1;
		connected = false;
		exited = false;
		deleted = false;
		physConn = 0;
		PTRACE5("Peer %p", this);
	}

	void construct()
	{
		commBase.construct(mgr.routingTable, *sock, peerDescr, *physConn);
		comm.construct( commBase, commBase, mgr.comm_logging);
		comm.setDefaultFrameSize(mgr.udpPacketSize);
		comm.setKeepAlive(mgr.keepAliveWrite,mgr.keepAliveRead);
		comm.setResendTimeout(mgr.resend);
	}
	void init()
	{
		commBase.init();
		comm.init(true,0);
	}

};




struct CommUDPData
{
	CommRWUDPThreadManager::Peer *peer;
	CommUDPData()
	{
		peer = 0;
	}

};





class _CommRWUDPWorkerThread : public PThread
{
private:
	bool shutdownFlag;
	
public:
	CommRWUDPThreadManager* mgr;
	_CommRWUDPWorkerThread()
	{
		mgr = 0;
		shutdownFlag = false;
	}

	void shutdown()
	{
		shutdownFlag=true;
	}

	void run()
	{
		while(1)
		{
			DWORD numberOfBytes;
			ULONG_PTR key;
			OVERLAPPED * over;
			BOOL ret = GetQueuedCompletionStatus(mgr->completionPort,&numberOfBytes,&key, &over, INFINITE);
			if (shutdownFlag)
			{
				PLog("_CommRWUDPWorkerThread exiting");
				return;
			}
			if (ret)
			{
				CommRWUDPThreadManager::UdpCommBuffer * buff = (CommRWUDPThreadManager::UdpCommBuffer*) over;
				
				if (buff->type == 1)
				{
//					PTRACE5("Read UdpCommBuffer %p, bytes %d", buff, numberOfBytes);
					buff->size = numberOfBytes;
					mgr->processMsg(buff);
					DWORD flags = 0;
					DWORD retb;
					memset( &buff->over, 0, sizeof(OVERLAPPED) );
//					PTRACE5("Setting buffer %p", buff);
//					Sleep(500);
					WSARecvFrom( mgr->sock->getSocket(), &buff->DataBuf, 1, &retb, &flags, &buff->addr, &buff->addrSize, &buff->over, NULL );
//					PTRACE5("Buffr %p is set", buff);
					
				}
				else if (buff->type == 2)
				{
					CommRWUDPThreadManager::Peer * peer =  (CommRWUDPThreadManager::Peer *) over;
					PLock lock(peer->cps);
					if (peer->deleted)
					{
						lock.unlock();
						PTRACE3("Deleting peer in worker");
						delete peer;
					}
					else
					{
						peer->commBase.written = numberOfBytes;
						peer->comm.asyncWriteOk();
					}
				}
			}
			else
			{
				DWORD err = ::GetLastError();
				PString ret;
				PSystemError::errorMessage(ret,err);

				if ( err == 1234)
				{
					CommRWUDPThreadManager::UdpCommBuffer * buff = (CommRWUDPThreadManager::UdpCommBuffer*) over;
					if (buff->type == 1)
					{
						mgr->processError(buff,err,ret.c_str());
						DWORD flags = 0;
						DWORD retb;
						memset( &buff->over, 0, sizeof(OVERLAPPED) );
						WSARecvFrom( mgr->sock->getSocket(), &buff->DataBuf, 1, &retb, &flags, &buff->addr, &buff->addrSize, &buff->over, NULL );
						continue;
					}
					else if (buff->type == 2)
					{

						CommRWUDPThreadManager::Peer * peer =  (CommRWUDPThreadManager::Peer *) over;
						PLock lock(peer->cps);
						if (peer->deleted)
						{
							lock.unlock();
							PTRACE3("Deleting peer in worker");
							delete peer;
						}
						else
						{
							mgr->_internalForceDisconnect(peer, err, ret.c_str());
						}
						continue;
					}
				}
				PLog("_CommRWUDPWorkerThread error %s, exited", ret.c_str());
				if ( err == 1234)
					continue;

				return;
			}
		}
	}

};




class _CommRWUDPThread : public _CommManageableThread
{
public:


protected:
	CommRWUDPThreadManager& mgr;
	int mgrIdx;


public:
	_CommRWUDPThread( CommRWUDPThreadManager& mgr_, int mgrIdx_);
	void pushSocket( const CommRWUDPThreadManager::Peer* peer );
	void pushForceDisconnect( int slot, UINT32 channelId, int errCode, const char * errorMsg );
	void run() override;


protected:
	bool _freeSocket( int idx )//to solve access problem for _RWThread
	{
		return mgr._freeSocket( idx );
	}


private:
	void _physConn(CommRWUDPThreadManager::Peer* peer);
};



void _CommRWUDPThread::_physConn(CommRWUDPThreadManager::Peer* peer)
{
	mgr.routingTable.physicallyConnected( peer->commBase.physConn, peer->mgrIdx, peer->slot, peer->channelId, 0 );
}

/* virtual */ void _CommRWUDPThread::run()
{
	CommUDPData * rwData = new CommUDPData [ mgr.numSlots ];
	try
	{
		UINT32 lastKeepAlive = SYSTEMTICKS();
		for(;;)
		{
			UINT32 now = SYSTEMTICKS();
			UINT32 diff = now - lastKeepAlive;
			if( diff >= mgr.keepAlivePoll)
			{
				for( int i=0; i < mgr.numSlots ; ++i )
				{
					if( rwData[i].peer)
					{
						CommUDPData & data = rwData[ i ];
						PLock lock(data.peer->cps);
						try
						{
							bool Ok = data.peer->comm.serviceCallTransport( _SERVICE_REQUEST_KEEP_ALIVE,0);
							if( !Ok )
							{
								PLog( "UDP keepAlive timeout expired" );
								mgr._internalForceDisconnect(data.peer,_SOCKET_TIMEOUT_EXPIRED,"keepAlive timeout expired" );
							}
						}
						catch( PError& err )
						{
							PLog( "CommRWUDPThread[ %d ] exception/1 in slot #%d: %s", mgrIdx, i, err.why() );
							mgr._internalForceDisconnect(data.peer,_SOCKET_EXCEPTION,err.why());
						}
					}
				}
				lastKeepAlive = now;
				diff = 0;
			}

			UINT32 wait = diff < mgr.keepAlivePoll ? mgr.keepAlivePoll - diff : 0;
			_CommMsg msg;
			int  w = q.waitPop( msg, wait );
			if (w == CommMsgQueue::waitPopOk)
			{
				if( msg.type == _COMM_MSGTYPE_SERVICEREQUEST)
				{
					//ignore
				}
				else if( msg.type == _COMM_MSGTYPE_NONE )//'special message' flag
				{
					if( msg.internal.slot == 0xFFFF )//pushSocket
					{
						CommMsgParser parser( msg.body );

						UINT64 p64;
						parser.parseUINT64(p64);
						CommRWUDPThreadManager::Peer* peer = (CommRWUDPThreadManager::Peer*) p64;
						PLock lock(peer->cps);
						int slot;
						for( slot=0; slot < mgr.numSlots ; ++slot )
						{
							if (!rwData[slot].peer)
								break;
						}
						PASSERT3(slot<mgr.numSlots);

						PTRACE3( "CommRWUDPThread[ %d ]: Socket placed to slot #%d",  mgrIdx, slot );

						CommUDPData & data = rwData[ slot ];
						data.peer = peer;
						data.peer->channelId = mgr.nextChannelId();
						data.peer->slot = slot;
						data.peer->mgrIdx = mgrIdx;
						data.peer->connected = false;
						try
						{
							data.peer->construct();
							data.peer->init();
							PString addr;
							PUDPSocket::address( addr, peer->peerDescr );
							CommMsgBody msg_body;
							msg_body.composeINT32(slot);
							msg_body.composeString(addr);
							data.peer->comm.serviceCallTransport( _SERVICE_REQUEST_SETIPADDRESS,&msg_body);

							if( data.peer->connected == false && data.peer->comm.isConnected() )
							{
								data.peer->connected = true;
								_physConn( data.peer );
								data.peer->comm.asyncReadOk(true);
							}
							delete data.peer->commBase.readReady;
							data.peer->commBase.readReady = 0;

						}
						catch( PError& err )
						{
							delete data.peer->commBase.readReady;
							data.peer->commBase.readReady=0;
							PLog( "CommRWUDPThread[ %d ] exception/1 in slot #%d: %s", mgrIdx, slot, err.why() );
							mgr._internalForceDisconnect(data.peer,_SOCKET_EXCEPTION,err.why());
						}


					}
					else//forceDisconnect
					{
						int slot = msg.internal.slot;

						PASSERT3( slot < mgr.numSlots);
						if( msg.internal.channelId != rwData[ slot ].peer->channelId )
						{// Critical check. Protects from disconnecting the wrong channel
							PLog( "CommRWUDPThread[ %d ]: forceDisconnect obsolete channelId=%08X - ignored", mgrIdx, msg.internal.channelId );
							break;
						}
						CommMsgParser parser( msg.body );
						INT32 errCode;
						const char * errorMsg;
						parser.parseINT32(errCode)
							  .parseString(errorMsg);
						CommUDPData & data = rwData[ slot ];

						_CommPhysicalConnection *physConn=0;
						{
							PLock lock(data.peer->cps);
							data.peer->exited = true;
							PLog( "CommRWUDPThread[ %d ]: slot #%d closed: error %x - %s",  mgrIdx, slot, errCode, errorMsg );
							data.peer->comm._log("Closed: error %x -  %s", errCode, errorMsg);
							physConn = data.peer->physConn;
							mgr.removePeer( data.peer );
							PTRACE5("marking peer as deleted");
							data.peer->deleted=true;
						}
						if (!data.peer->commBase.sendBusy)
						{
							PTRACE5("deleting peer %p", data.peer);
							delete data.peer;
						}
					
						if (physConn)
							mgr.routingTable.physicallyDisconnected( physConn, errCode );
						data.peer = 0;

						int idx = mgrIdx;
						if( _freeSocket( idx ) )
						{//no thread operations allowed after freeSocket returned true
							PTRACE3( "Exiting CommRWUDPThread[ %d ]",  idx );
							//exit(); - neither needed nor allowed as this is already deleted
							delete [] rwData;
							return;
						}
					}
				}
				else
				{
					int slot = msg.internal.slot;


					PASSERT3( slot <mgr.numSlots );
					if( msg.internal.channelId != rwData[ slot ].peer->channelId )
					{// Critical check. Protects from dispatching the message to the wrong channel
						// when connection was deleted and then another connection created in the same slot.
						// Also protects from using failed connection: in case of failure 
						// rwData[ slot ].channelId must be changed
						PLog( "CommRWUDPThread[ %d ]: message to obsolete channelId=%08X - ignored",  mgrIdx, msg.internal.channelId );
						break;
					}

					CommUDPData & data = rwData[ slot ];
					try
					{
						PLock lock(data.peer->cps);
						data.peer->comm.writeMsg( msg );
					}
					catch( PError& err )
					{
						PLog( "CommRWUDPThread[ %d ] exception/3 in slot #%d: %s",  mgrIdx, slot, err.why() );
						mgr._internalForceDisconnect(data.peer,_SOCKET_EXCEPTION,err.why());
					}

				}
			}
			else if (w == CommMsgQueue::waitPopShutdown)
			{
				delete [] rwData;
				return;
				}
		}
	}
	catch( PFatalError& err )
	{
		PLogError( "CommRWUDPThread[ %d ] last hope catch: %s",  mgrIdx, err.why() );
		if (mgr.isExitOnFatalError())
		{
			FATALEXIT();
		}
		else
		{
			exit(true);
		}
	}
	catch( PError& err )
	{
		PLogError( "CommRWUDPThread[ %d ] last hope catch: %s",  mgrIdx, err.why() );
		exit(true);
	}
	catch(  exception& exc  )
	{
		PLogError( "CommRWUDPThread[ %d ] last hope STD exception catch: %s",  mgrIdx,  exc.what() );
		exit(true);
	}
	catch( ... )
	{
		PLogError( "CommRWUDPThread[ %d ] last hope ... catch",  mgrIdx );
		exit(true);
	}
	delete [] rwData;
}


//****************************************************************************


_CommRWUDPThread::_CommRWUDPThread( CommRWUDPThreadManager& mgr_, int mgrIdx_ )
: mgr( mgr_ )
{
	mgrIdx = mgrIdx_;
}

void _CommRWUDPThread::pushSocket( const CommRWUDPThreadManager::Peer* peer )
{
	_CommMsg m( _COMM_MSGTYPE_NONE, _COMM_MSGPRIORITY_INITCONNECT );
	m.internal.slot = 0xFFFF;
	UINT64 p64 = (UINT64)(ULONG)peer;
	m.body.composeUINT64(p64);
	q.push( m );
}

void _CommRWUDPThread::pushForceDisconnect( int slot, UINT32 channelId, int errCode, const char * errorMsg )
{
	_CommMsg m( _COMM_MSGTYPE_NONE, _COMM_MSGPRIORITIES - 1 );
	PASSERT3( slot != 0xFFFF );
	m.internal.slot = slot;
	m.internal.channelId = channelId;
	m.body.composeINT32(errCode)
		  .composeString(errorMsg);
	q.push( m );
}

//****************************************************************************

inline /* static */ _CommRWUDPThread* CommRWUDPThreadManager::_rwThread( Thread& thread )
{
	return static_cast< _CommRWUDPThread* >( thread.thread );//safe, because all RWThreadManager threads are in fact RWThreads
}

CommRWUDPThreadManager::CommRWUDPThreadManager( CommRoutingTable& routingTable_)
: CommThreadManager( routingTable_ )
{
	started = false;
	sock = 0;
	comm_logging = 0;

	numBuffers = UDP_NUMBUFFS;
	numThreads = UDP_NUMTHREADS;
	numConcurrent = UDP_NUMCONCURRENT;
	udpPacketSize = UDP_DEFAULT_FRAME_SIZE;
	keepAliveRead = UDP_DEFAULT_KEEP_ALIVE_READ;
	keepAliveWrite = UDP_DEFAULT_KEEP_ALIVE_WRITE;
	keepAlivePoll = UDP_DEFAULT_KEEP_ALIVE_POLL;
	numSlots = UDP_NUMSLOTS;
	resend = UDP_RESENT_TIMEOUT;
	udpBufferSizeSnd = -1;
	udpBufferSizeRcv = -1;

}


void CommRWUDPThreadManager::listenPort( int port )
{
	lPort = port;

}
/* virtual */ void CommRWUDPThreadManager::setOption( const char* optName, const char* optValue )
{
	if ( strcmp( optName, COMM_OPTION_PROTOCOL_LOG_FILE ) == 0 && comm_logging == 0)
		comm_logging = new _CommProtocolLog(optValue,PLogFile::perDate|PLogFile::printTimePerLine);

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
	else if( strcmp( optName, COMM_OPTION_WORKINGTHREADS ) == 0 )
		numThreads =  atoi( optValue );
	else if( strcmp( optName, COMM_OPTION_BUFFERS ) == 0 )
		numBuffers =  atoi( optValue );
	else if( strcmp( optName, COMM_OPTION_CONCURRENT ) == 0 )
		numConcurrent =  atoi( optValue );
	else if( strcmp( optName, COMM_OPTION_KEEPALIVEPOLL ) == 0 )
		keepAlivePoll =  atoi( optValue );
	else if( strcmp( optName, COMM_OPTION_SLOTS ) == 0 )
		numSlots =  atoi( optValue );
	else if( strcmp( optName, COMM_OPTION_RESEND ) == 0 )
		resend =  atoi( optValue );
	else if( strcmp( optName, COMM_OPTION_UDPBUFFER ) == 0 )
	{
		udpBufferSizeRcv = udpBufferSizeSnd =  atoi( optValue );
	}
	else if( strcmp( optName, COMM_OPTION_UDPBUFFER_RCV ) == 0 )
		udpBufferSizeRcv =  atoi( optValue );
	else if( strcmp( optName, COMM_OPTION_UDPBUFFER_SND ) == 0 )
		udpBufferSizeSnd =  atoi( optValue );

	else
		CommThreadManager::setOption( optName, optValue );
}

/* virtual */ void CommRWUDPThreadManager::dynamicInit(PIniFile& iniFile)
{
	CommThreadManager::dynamicInit(iniFile);
}

void CommRWUDPThreadManager::start()
{
	PWLock lock( cs );
	PASSERT3( !started );

	udpbuffs = new UdpCommBuffer[numBuffers];
	workerThreads = new _CommRWUDPWorkerThread[numThreads];

	sock = new PUDPSocket();
	sock->bind(lPort);

	if (udpBufferSizeRcv != -1 )
	{
		PTRACE3( "CommRWUDPThreadManager: Set recv UDP buffer = %d", udpBufferSizeRcv );
		bool Ok = sock->udpSetRecvBuffer(udpBufferSizeRcv);
		if( !Ok )
			PLog( "CommRWUDPThreadManager: Unable to set recv UDP buffer %d", udpBufferSizeRcv );
	}
	if (udpBufferSizeSnd != -1 )
	{
		PTRACE3( "CommRWUDPThreadManager: Set snd UDP buffer = %d", udpBufferSizeSnd );
		bool Ok = sock->udpSetSendBuffer(udpBufferSizeSnd);
		if( !Ok )
			PLog( "CommRWUDPThreadManager: Unable to set snd UDP buffer %d", udpBufferSizeSnd );
	}

	completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,NULL,numConcurrent);
	CreateIoCompletionPort((HANDLE)sock->getSocket(),completionPort,1,0);

	int i;
	for( i=0; i <numBuffers ; ++i )
	{
		DWORD flags = 0;
		DWORD retb;
		int err = WSARecvFrom( sock->getSocket(), &udpbuffs[i].DataBuf, 1, &retb, &flags, &udpbuffs[i].addr, &udpbuffs[i].addrSize, &udpbuffs[i].over, NULL );
		err = WSAGetLastError();
	}

	for(  i=0; i < numThreads; ++i )
	{
		workerThreads[ i ].mgr = this;
		workerThreads[ i ].start();
	}
	started = true;
}

void CommRWUDPThreadManager::shutdown()
{
	PWLock lock( cs );
	int i;
	for( i=0; i < numThreads ; ++i )
	{
		workerThreads[ i ].shutdown();
	}

	sock->close();
	::CloseHandle(completionPort);
	_Parent::shutdown();
}


bool CommRWUDPThreadManager::terminated()
{
	PWLock lock( cs );
	if( !_Parent::terminated() )
		return false;
	for( int i=0; i < numThreads ; ++i )
		if( !workerThreads[ i ].terminated() )
			return false;
	return true;
}

CommRWUDPThreadManager::~CommRWUDPThreadManager()
{
	PWLock lock( cs );

	delete [] workerThreads;
	delete [] udpbuffs;

	delete sock;
	PeerMap::iterator it;
	for (it = peers.begin(); it != peers.end(); ++it)
	{
		delete (*it).second;
	}
	if(comm_logging)
		delete comm_logging;
}

void CommRWUDPThreadManager::_pushInSocket( const PUDPSocket::Descriptor& sockDescr, const char* addr, UINT64 key, UdpCommBuffer * buff )
{
	PWLock lock( cs );
	CommRWUDPThreadManager::Peer * peer =  new CommRWUDPThreadManager::Peer(*this,sockDescr,sock);
	peer->physConn = routingTable.createIncomingPhysicalConnection( addr, this );
	peer->commBase.readReady = new UdpCommBuffer();
	peer->commBase.readReady->size = buff->size;
	memcpy(peer->commBase.readReady->buf,buff->buf, buff->size);
	peers.insert(PeerMap::value_type(key,peer));
	_pushSocket(peer);

}

void CommRWUDPThreadManager::_pushSocket( const CommRWUDPThreadManager::Peer* sock )
{
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
	thread.slotsLeft = thread.maxSlots = numSlots;
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


int CommRWUDPThreadManager::_numberOfConnections()
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

bool CommRWUDPThreadManager::_freeSocket( int threadIdx )//no thread operations allowed after freeSocket returned true//no thread operations allowed after freeSocket returned true
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

void CommRWUDPThreadManager::forceDisconnect( _CommPhysicalConnection* physConn )
{
	PWLock lock( cs );
	Thread& thread = threads[ physConn->thread ];
	_rwThread( thread )->pushForceDisconnect( physConn->slot, physConn->channelId, 0, "forceDisconnect received" );
}

void CommRWUDPThreadManager::_internalForceDisconnect(Peer* peer, int errCode, const char * errMsg)
{
	PRLock lock( cs );
	if (peer->exited)
		return;
	peer->exited = true;
	Thread& thread = threads[ peer->mgrIdx ];
	_rwThread( thread )->pushForceDisconnect( peer->slot, peer->channelId, errCode, errMsg);
}


void CommRWUDPThreadManager::loggerInformation(CommMsgBody &body)
{
	PWLock lock( cs );
	int num = _numberOfConnections();
	PString s;
	s.assign("Connections(").append(managerType.c_str()).append(")");
	composeLoggerStat(body, s.c_str(), num);
}

_CommRWUDPThread* CommRWUDPThreadManager::_createNewThread( int threadIdx )
{
	return new _CommRWUDPThread( *this, threadIdx);
}

void CommRWUDPThreadManager::_processMsg(Peer* peer, UdpCommBuffer * buff)
{
	PLock lock(peer->cps);
	if (peer->slot == -1)
	{
		PString addr;
		PUDPSocket::address( addr, peer->peerDescr );
		PLog( "CommRWUDPThreadManager::_processMsg: communicator for %s:%d not ready",  addr.c_str(), PUDPSocket::port( peer->peerDescr ));
		return;
	}
	if (peer->exited)
	{
		PTRACE5( "CommRWUDPThreadManager::_processMsg: message for closing connection, ignored");
		return;
	}
	peer->commBase.readReady = buff;
	try
	{
		peer->comm.asyncReadOk();
		if( peer->connected == false && peer->comm.isConnected() )
		{
			peer->connected = true;
			routingTable.physicallyConnected( peer->commBase.physConn, peer->mgrIdx, peer->slot, peer->channelId, 0 );
			peer->comm.asyncReadOk(true);
		}
		peer->commBase.readReady = 0;
	}
	catch( PError& err )
	{
		PLog( "CommRWUDPThreadManager::_processMsg error: %s",  err.why() );
		_internalForceDisconnect(peer, _SOCKET_EXCEPTION, err.why());
	}



}

void CommRWUDPThreadManager::processError(UdpCommBuffer * buff, UINT32 err, const char * errorMsg)
{
	UINT64 key;
	key = (      (UINT64) ((struct sockaddr_in *)&buff->addr)->sin_addr.S_un.S_addr << 32) |((struct sockaddr_in *)&buff->addr)->sin_port;
	PeerMap::iterator it;
	PRLock lock(localLock);
	it = peers.find(key);
	if (it != peers.end())
	{
		Peer* peer = (*it).second;
		PLock lock(peer->cps);
		if (peer->slot == -1)
		{
			PString addr;
			PUDPSocket::address( addr, peer->peerDescr );
			PLog( "CommRWUDPThreadManager::_processMsg: communicator for %s:%d not ready",  addr.c_str(), PUDPSocket::port( peer->peerDescr ));
		}
		else if (peer->exited)
		{
			PTRACE5( "CommRWUDPThreadManager::_processMsg: message for closing connection, ignored");
			return;
		}
		else
		{
			_internalForceDisconnect(peer, err, errorMsg);
		}
	}
}
void CommRWUDPThreadManager::processMsg(UdpCommBuffer * buff)
{
	UINT64 key;
	key = (      (UINT64) ((struct sockaddr_in *)&buff->addr)->sin_addr.S_un.S_addr << 32) |((struct sockaddr_in *)&buff->addr)->sin_port;
	PeerMap::iterator it;
	{
		PRLock lock(localLock);
		it = peers.find(key);
		if (it != peers.end())
		{
			Peer* peer = (*it).second;
			_processMsg(peer, buff);
			return;
		}
	}
	PWLock lock(localLock);
	it = peers.find(key);
	if (it != peers.end())
	{
		Peer* peer = (*it).second;
		_processMsg(peer,buff);
		return;
	}
	else
	{
		// only connect requests should be processed
		if (*buff->buf != 1 || buff->size != 50)
			return;
		PUDPSocket::Descriptor accepted;
		accepted.addr = buff->addr;
		PString addr;
		PUDPSocket::address( addr, accepted );
		int port = PUDPSocket::port( accepted );
		PString addr2;
		composeSocketAddr( addr2, addr, port );
		PLog( "Incoming connection from '%s'", addr2.c_str() );
		_pushInSocket( accepted, addr2, key, buff);
	}

}


void CommRWUDPThreadManager::removePeer(Peer* peer)
{
	UINT64 key;
	key = (      (UINT64) ((struct sockaddr_in *)&peer->peerDescr.addr)->sin_addr.S_un.S_addr << 32) |((struct sockaddr_in *)&peer->peerDescr.addr)->sin_port;
	PeerMap::iterator it;
	PWLock lock(localLock);
	it = peers.find(key);
	if (it != peers.end())
	{
		peers.erase(it);
	}
}
#endif