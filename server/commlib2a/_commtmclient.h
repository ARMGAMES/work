#ifndef _commtmclient_h_included
#define _commtmclient_h_included

#include "ppinclude.h"
#include "ppstring.h"
#include "pplogfile.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "_commcomm.h"
#include "commtmclient.h"
#include "commhttp.h"
#include "commstreamencryption.h"

#define _HTTPSPROXY_ERROR_WEIGHT	5
#define _HTTPSPROXY_403ERROR_WEIGHT 50

//#define USE_CONNECTOR_THREAD 



class _CommClientThreadError : public PInternalError
{
private:
	int w;

public:
	_CommClientThreadError( const char* msg, int w_ )
		: PInternalError( msg )
	{
		w = w_;
	}

	int weight() const { return w; }
};

struct _CommClientCommunicator
{
private:
	PMultiSocketRW* rw;
	CommRoutingTable* routingTable;
	_CommPhysicalConnection* physConn;

public:
	static bool hasCompression( int type ) { return false; }
	void setCompression( int type, size_t BuffSize ) {}
	void setEncryption( CommStreamEncryption * encryption_ ) {}
	void flush() {}
	bool initConnection(UINT32 & checkSumm)
	{
		checkSumm = 0;
		return true;
	}
	bool generateRandom(UINT32 * secureRandom, int size)
	{
		return false;
	}
public:
	void construct( PMultiSocketRW& rw_, CommRoutingTable& routingTable_,_CommPhysicalConnection* physConn_ )
	{
		rw = &rw_;
		routingTable = &routingTable_;
		physConn = physConn_;
	}

	size_t writeIncomplete( const BYTE* p, size_t sz )
	{
		return rw->write( 1, p, sz, true );
	}
	size_t readIncomplete( BYTE* p, size_t sz )
	{
		return rw->read( 1, p, sz, true );
	}
	_CommAsyncRet asyncReadOk()
	{
		_CommAsyncRet ret;
		if( rw->readPending( 1 ) )
			return ret;
		ret.read = rw->getReadSize( 1 );
		return ret;
	}
	_CommAsyncRet asyncWriteOk()
	{
		_CommAsyncRet ret;
		if( rw->writePending( 1 ) )
			return ret;
		ret.written = rw->getWrittenSize( 1 );
		return ret;
	}

	void msg( int mux, UINT32 muxChannelId, _CommMsg& msg )
	{
		routingTable->routeIncomingMessage( physConn, mux, muxChannelId, msg );
	}

	void forceSlotFailure( const char* errMsg, int errCode = 0)
	{
		rw->forceSlotFailure( 1, errMsg, errCode );
	}

	void serviceCall(UINT32 id, CommMsgBody * msg_body)
	{
		routingTable->serviceCall(physConn,id,msg_body);
	}
};

#ifdef USE_CONNECTOR_THREAD 
class _CommClientThreadConnect; 
#endif



class _CommClientThreadBase : public _CommManageableThread
{
public:
	enum { _KeepAlivePoll = 1000 };

protected:
	friend class _CommClientThreadConnect;
	friend class CommClientThreadManagerBase;

	_CommPhysicalConnection* physConn;
	CommClientThreadManagerBase* mgr;
	PString address;
	UINT32 channelId;
	int mgrIdx;

	UINT32 commQueueSize;
	UINT32 maxCommQueueSize;
	UINT32 commTimeInQueue;
	UINT32 maxCommTimeInQueue;
	UINT32 timeInQueue;
	UINT32 maxTimeInQueue;

	UINT32 queueSize;
	UINT32 maxQueueSize;

	UINT32 measures;

	bool statInit;


public:
	_CommClientThreadBase( CommClientThreadManagerBase& mgr_, int mgrIdx_, _CommPhysicalConnection* physConn_, UINT32 channelId_ );

	UINT32 printCounters(int n,UINT32 & qSize, UINT32 & qTime, UINT32 & qSlotSize, UINT32 & qSlotTime) override
	{
		UINT32 _max = _CommManageableThread::printCounters(n,qSize,qTime,qSlotSize,qSlotTime);
		if (mgr->isGatherStatistic())
		{
			UINT32 q = queueSize;
			UINT32 mq = maxQueueSize;
			UINT32 m = measures;
			UINT32 tq = timeInQueue;
			UINT32 mtq = maxTimeInQueue;

			queueSize = 0;
			maxQueueSize = 0;
			measures = 0;
			timeInQueue=0;
			maxTimeInQueue=0;
			qSize = mq;
			qTime = mtq;


			if (m)
			{
				q = q/m;
				tq = tq/m;
			}
			else
			{
				q=0;
				tq=0;
			}


			PLog("QueueSize(max)=%u(%u), time in queue(max)=%u(%u)",q,mq,tq,mtq);

			qSlotSize = 0;
			qSlotTime = 0;
			if (mgr->isGatherStatistic() > 1)
			{
				UINT32 q = commQueueSize;
				UINT32 mq = maxCommQueueSize;
				UINT32 tq = commTimeInQueue;
				UINT32 mtq = maxCommTimeInQueue;
				commQueueSize=0;
				maxCommQueueSize=0;
				commTimeInQueue=0;
				maxCommTimeInQueue=0;

				if (m)
				{
					q = q/m;
					tq = tq/m;
				}
				else
				{
					q=0;
					tq=0;
				}
				qSlotSize = qSlotSize > mq ? qSlotSize : mq;
				qSlotTime = qSlotTime > mtq ? qSlotTime : mtq;
				if (q || mq || tq > 32 || mtq > 32)
					PLog("queue(max) = %u(%u), time in queue(max) = %u(%u)",q,mq,tq,mtq);
				commQueueSize=0;
				maxCommQueueSize=0;
				commTimeInQueue=0;
				maxCommTimeInQueue=0;

			}
		}
		return _max;

	}


#ifdef USE_CONNECTOR_THREAD 
	void _writerConnected()
	{
		_CommMsg msg( _COMM_MSGTYPE_NONE, _COMM_MSGPRIORITIES - 1 );
		msg.internal.slot = 0xffff;
		msg.internal.channelId = channelId;
		push( msg );
	}
	void _readerWriterFailure( int weight, int errorCode )
	{
		_CommMsg msg( _COMM_MSGTYPE_NONE, _COMM_MSGPRIORITIES - 1 );
		msg.internal.slot = 1;
		msg.internal.channelId = channelId;
		msg.internal.patch0 = weight;
		msg.internal.patch1 = errorCode;
		push( msg );
	}
#endif

	void forceDisconnect()
	{
		_CommMsg msg( _COMM_MSGTYPE_NONE, _COMM_MSGPRIORITIES - 1 );
		msg.internal.slot = 3;
		msg.internal.channelId = channelId;
		push( msg );
	}
	void _debugEmulateNetworkFailure()
	{
		_CommMsg msg( _COMM_MSGTYPE_NONE, _COMM_MSGPRIORITIES - 1 );
		msg.internal.slot = 2;
		msg.internal.channelId = channelId;
		push( msg );
	}
	void _statInit()
	{
		queueSize=0;
		maxQueueSize=0;
		measures=0;
		timeInQueue=0;
	    maxTimeInQueue=0;
		commQueueSize = 0;
		maxCommQueueSize=0;
		maxCommTimeInQueue=0;
		commTimeInQueue=0;
		statInit=true;
	}


protected:
	void _freeSocket( int idx )
	{
		mgr->_freeSocket( idx );
	}
};



#ifdef USE_CONNECTOR_THREAD 
class _CommClientThreadConnect : public PThread
{
private:
	_CommClientThreadBase* manager;
	PIPSocket sock;

public:
	_CommClientThreadConnect( _CommClientThreadBase& manager_, const PIPSocket::Descriptor& descr )
		: manager( &manager_ ),
		sock( descr )
	{
	}
public:
	void run() override
	{
		PTRACE3( "Connect thread started" );
		PThread::setCurrentThreadPriority( manager->mgr->getThreadPriority() );

		int failureWeight = 0;
		int errorCode = 0;
		try
		{
			PString addr;
			int port;
			manager->mgr->parseSocketAddr( addr, port, manager->address );

			if( *manager->mgr->getHttpsProxy() )
			{
				const char* connAddr = manager->mgr->getHttpsProxy();
				int connPort = manager->mgr->getHttpsProxyPort();

				try
				{
					PLog( "Connecting to https proxy %s:%d", connAddr, connPort );
					sock.connect( connAddr, connPort );
					PLog( "Connected to https proxy %s:%d", connAddr, connPort );
				}
				catch( PError& err )
				{//rethrow with weight
					throw _CommClientThreadError( err.why(), _HTTPSPROXY_ERROR_WEIGHT );
				}
				// THIS CODE IS DUPLICATED IN COMMLOCALSSLCLIENT.CPP Make changes there if you change something here

				CommHttpRequest req;
				PString url( "http://" );
				url.append( addr ).append( ':' ).appendInt( port ).append( '/' );
				req.setUrl( url );
				req.setProxy( manager->mgr->getHttpsProxy(), manager->mgr->getHttpsProxyPort() );

				PString reqStr;
				req.createConnectRequest( reqStr );
				sock.sendComplete( (const BYTE*)reqStr.c_str(), strlen( reqStr ) );
				BYTE buf[ 2 ];
				CommHttpResponse resp;
				for(;;)
				{
					size_t rd = sock.recv( buf, 1,manager->mgr->getTcpConnectTimeout()* TICKS_PER_SECOND );//incomplete
					if ( rd <= 0 )
					{
						PString err( "HTTP Proxy response timeout" );
						throw PError( err );
					}

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
								throw _CommClientThreadError( err, _HTTPSPROXY_403ERROR_WEIGHT );
							else
								throw _CommClientThreadError( err, _HTTPSPROXY_ERROR_WEIGHT );
						}
						break;
					}
				}

			}
			else
			{
				const char* connAddr = addr;
				int connPort = port;

				PLog( "Connecting to %s:%d", connAddr, connPort );
				sock.connect( connAddr, connPort );
				PLog( "Connected to %s:%d", connAddr, connPort );
			}
			PIPSocket::Descriptor retDescr;
			sock.extractDescriptor(retDescr); // to protect against closing socket
			manager->_writerConnected();
			return;
		}
		catch( PSSLError& err )
		{
			PLog( "Error in SimpleWriter stream: %s", err.why() );
			failureWeight = -1;
			errorCode=_SOCKET_SSLCERTIFICATEFAILED;
		}
		catch( _CommClientThreadError& err )
		{
			PLog( "Error in SimpleWriter stream: %s", err.why() );
			failureWeight = err.weight();
		}

		catch( PError& err )
		{
			PLog( "Error in SimpleWriter stream: %s", err.why() );
			failureWeight = 1;
		}

		PIPSocket::Descriptor retDescr;
		sock.extractDescriptor(retDescr); // to protect against closing socket
		manager->_readerWriterFailure( failureWeight,errorCode );
		PTRACE3( "ClientThreadConnect thread terminated" );
	}
};

#endif

//****************************************************************************

template< class _Comm >
class _CommClientThread : public _CommClientThreadBase
{
private:


public:
	_CommClientThread( CommClientThreadManagerBase& mgr_, int mgrIdx_, _CommPhysicalConnection* physConn_, UINT32 channelId_)
		: _CommClientThreadBase( mgr_, mgrIdx_,physConn_,channelId_ )
	{
	}

public:
	void run() override;

private:
	void _physConn();
};

template< class _Comm >
class _CommClientSSLThread : public _CommClientThread<_Comm>
{
private:
public:
	_CommClientSSLThread( CommClientThreadManagerBase& mgr_, int mgrIdx_, _CommPhysicalConnection* physConn_, UINT32 channelId_ )
		: _CommClientThread<_Comm>( mgr_, mgrIdx_,physConn_,channelId_ )
	{
	}

public:

	void run() override
	{
		_CommClientThread<_Comm>::run();
		CommSSL::threadCleanup();
	};

private:
	void _physConn();
};




template< class _Comm >
void _CommClientThread< _Comm >::_physConn()
{
	mgr->routingTable.physicallyConnected(physConn, mgrIdx, 0, channelId, 0 );
}

template< class _Comm >
/* virtual */ void _CommClientThread< _Comm >::run()
{
	try
	{
		PMultiSocketRW rw( q.getSocket(), 1);
		_statInit();

		PString addr;
		int port;
		mgr->parseSocketAddr( addr, port, address );

		_Comm comm;
		int state = 0;	//0 - socket not connected, comm not constructed;
		//1 - socket connected, comm constructed, comm not connected;
		//2 - socket connected, comm constructed&connected
#ifdef USE_CONNECTOR_THREAD 
		_CommClientThreadConnect* connector = 0;
#endif
		PIPSocket sock;
		{
			PIPSocket::Descriptor descr;
			sock.copyDescriptor( descr );
			PTRACE3( "Creating socket %d", descr.sock );
		}

		if( mgr->getTcpNoDelay() )
		{
			PTRACE3( "Disabling TCP delay" );
			bool Ok = sock.tcpNoDelay();
			if( !Ok )
				PLog( "Unable to disable TCP delay" );
		}
		if( mgr->getTcpBufferRcv() != -1 )
		{
			PTRACE3( "Set TCP recv buffer %d", mgr->getTcpBufferRcv());
			bool Ok = sock.tcpSetRecvBuffer(mgr->getTcpBufferRcv());
			if( !Ok )
				PLog( "Unable to set recv TCP buffer %d",mgr->getTcpBufferRcv() );
		}
		if( mgr->getTcpBufferSnd() != -1 )
		{
			PTRACE3( "Set TCP snd buffer %d", mgr->getTcpBufferSnd());
			bool Ok = sock.tcpSetSendBuffer(mgr->getTcpBufferSnd());
			if( !Ok )
				PLog( "Unable to set send TCP buffer %d",mgr->getTcpBufferSnd() );
		}

		int failureWeight = 0;
		int errorCode = 0;
		bool exit = false;
		UINT32 lastKeepAlive = SYSTEMTICKS();

		try
		{
#ifdef USE_CONNECTOR_THREAD 
			{
				PIPSocket::Descriptor descr;
				sock.copyDescriptor( descr );
				PASSERT3( connector == 0 );
				connector = new _CommClientThreadConnect( *this, descr );
				connector->start();
			}
#else
			UINT32 _queueSize  = 0;
			UINT32 _timeInQueue = 0;

			{
				if( *mgr->getHttpsProxy() )
				{
					const char* connAddr = mgr->getHttpsProxy();
					int connPort = mgr->getHttpsProxyPort();
					try
					{
						PLog( "Connecting to https proxy %s:%d", connAddr, connPort );
						if (sock.connect( connAddr, connPort, mgr->getTcpConnectTimeout()* TICKS_PER_SECOND))
						{
							PLog( "Connected to https proxy %s:%d", connAddr, connPort );
						}
						else
						{
							PTRACE3( "Connection to https proxy %ds timeout exceeded - closing thread...", mgr->getTcpConnectTimeout() );
							failureWeight = 1;
							exit = true;
						}
					}
					catch( PError& err )
					{//rethrow with weight
						throw _CommClientThreadError( err.why(), _HTTPSPROXY_ERROR_WEIGHT );
					}
					// THIS CODE IS DUPLICATED IN COMMLOCALSSLCLIENT.CPP Make changes there if you change something here
					if (exit == false)
					{
						CommHttpRequest req;
						PString url( "http://" );
						url.append( addr ).append( ':' ).appendInt( port ).append( '/' );
						req.setUrl( url );
						req.setProxy( mgr->getHttpsProxy(), mgr->getHttpsProxyPort() );

						PString reqStr;
						req.createConnectRequest( reqStr );
						sock.sendComplete( (const BYTE*)reqStr.c_str(), strlen( reqStr ) );
						BYTE buf[ 2 ];
						CommHttpResponse resp;
						for(;;)
						{
							size_t rd = sock.recv( buf, 1, mgr->getTcpConnectTimeout()* TICKS_PER_SECOND );//incomplete
							if ( rd <= 0 )
							{
								PString err( "HTTP Proxy response timeout" );
								throw PError( err );
							}

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
										throw _CommClientThreadError( err, _HTTPSPROXY_403ERROR_WEIGHT );
									else
										throw _CommClientThreadError( err, _HTTPSPROXY_ERROR_WEIGHT );
								}
								state = 1;
								break;
							}
						}
					}
				}
				else
				{
					const char* connAddr = addr;
					int connPort = port;
					PLog( "Connecting to %s:%d", connAddr, connPort );
					if (sock.connect( connAddr, connPort, mgr->getTcpConnectTimeout()* TICKS_PER_SECOND))
					{
						PLog( "Connected to %s:%d", connAddr, connPort );
						state = 1;
					}
					else
					{
						PTRACE3( "TCP connection %ds timeout exceeded - closing thread...", mgr->getTcpConnectTimeout() );
						failureWeight = 1;
						exit = true;
					}
				}
				if (!exit)
				{
					PIPSocket::Descriptor descr;
					sock.extractDescriptor( descr );
					int slot = rw.addSocket( descr.sock );
					PASSERT3(slot == 1);
					comm.construct(rw,*mgr, physConn, false, mgr->comm_logging);
					state = 1;
					comm.setKeepAlivePoll( _KeepAlivePoll );
					lastKeepAlive = SYSTEMTICKS();
					try
					{
						if( comm.isConnected() )
						{
							CommMsgBody msg_body;
							msg_body.composeINT32(0);
							msg_body.composeString(addr);
							comm.serviceCallTransport( _SERVICE_REQUEST_SETIPADDRESS,&msg_body);
							_physConn();
							state = 2;
							comm.setFirstRead();
						}

					}
					catch( PError& err )
					{
						PLog( "ClientThread error %s", err.why() );
						exit = true;
					}
				}
			}
#endif

			int w = -1;
			while(!exit)
			{
				UINT32 now = SYSTEMTICKS();
				int diff = now - lastKeepAlive;//subtracted as UINT32, then casted to int
				if( state == 0 )
				{
#ifdef USE_CONNECTOR_THREAD 
					if( mgr->getTcpConnectTimeout() && diff > mgr->getTcpConnectTimeout() * TICKS_PER_SECOND )//6s: double TCP SYN timeout
					{
						PTRACE3( "TCP connect %ds timeout exceeded - closing thread...", mgr->getTcpConnectTimeout() );
						failureWeight = 1;
						exit = true;
						break;//forever
					}
					diff = 0;
				
#else 
					PASSERT3(0);
#endif
				}
				else if( state == 1 )
				{
					if( mgr->getSslConnectTimeout() && diff > mgr->getSslConnectTimeout() * TICKS_PER_SECOND )//30s - should be enough to avoid disconnects due to the server overload
					{
						PTRACE3( "SSL handshake connect %ds timeout exceeded - closing thread...", mgr->getSslConnectTimeout() );
						failureWeight = 1;
						exit = true;
						break;//forever
					}
					diff = 0;
				}
				else//if( state > 1 )
				{
					if( diff >= _KeepAlivePoll * TICKS_PER_SECOND / 1000 )
					{
						if (mgr->isGatherStatistic())
						{
							statInit = false;
							PInterlockedIncrement(&measures);
							PInterlockedExchangeAdd(&queueSize, _queueSize);
							PInterlockedExchangeAdd(&timeInQueue,_timeInQueue);
							if ( maxTimeInQueue < _timeInQueue)
							{
								maxTimeInQueue = _timeInQueue;
							}
							if ( maxQueueSize < _queueSize)
							{
								maxQueueSize = _queueSize;
							}
							_queueSize = 0;
							_timeInQueue = 0;
						}
						else
						{
							if (statInit == false)
							{
								_queueSize  = 0;
								_timeInQueue = 0;
								_statInit();
							}
						}
						bool Ok = comm.serviceCallTransport( _SERVICE_REQUEST_KEEP_ALIVE,0);
						if( !Ok )
						{
							PLog( "ClientThread: keepAlive timeout exceeded - closing" );
							break;//forever
						}
						if (mgr->isGatherStatistic()>1)
						{
							CommMsgBody msg;
							comm.serviceCallTransport( _SERVICE_REQUEST_STATISTIC,&msg);
							CommMsgParser parser( msg );
							UINT32 tmp;
							parser.parseUINT32(tmp);
							PInterlockedExchangeAdd(&commQueueSize, tmp);
							if ( maxCommQueueSize < tmp)
							{
								maxCommQueueSize = tmp;
							}
							if (!parser.parseEnded())
							{
								parser.parseUINT32(tmp);
								PInterlockedExchangeAdd(&commTimeInQueue,tmp);
								if ( maxCommTimeInQueue < tmp)
								{
									maxCommTimeInQueue = tmp;
								}
							}
						}

						lastKeepAlive = now;
						diff = 0;
					}
				}

				UINT32 qSize;
				if (w == PMultiSocketRW::waitQueue)
					qSize = static_cast< UINT32 >( q.sizeAndReset() );
				else
					qSize = static_cast< UINT32 >( q.size() );
				if (qSize > 0 )
				{
					w = PMultiSocketRW::waitQueue;
					if (mgr->isGatherStatistic())
					{
						_queueSize = _queueSize > qSize ? _queueSize:qSize;
					}
				}
				else
				{
					UINT32 wait = diff < _KeepAlivePoll ? _KeepAlivePoll - diff : 0;
					int slot;
					w = rw.wait( slot, wait );
				}

				bool exit = false;
				switch( w )
				{
				case PMultiSocketRW::waitQueue:
					{
						if( q.shuttingDown() )
						{
							PTRACE3( "ClientThread shutdown - exiting");
							exit = true;
							break;
						}
						do
						{
							//PTRACE3( "_DBG: RW:waitQueue qSize=%d", q.size() );
							_CommMsg msg;
							UINT32 _timeInQueueTmp;
							if( !q.pop( msg, _timeInQueueTmp ) )
							{
								if (mgr->getOptimizePacketSize())
								{
									try
									{
										comm.flushStoredMsgs();
									}
									catch( PError& err )
									{
										PLog( "ClientThread error: %s", err.why() );
										exit = true;
										break;
									}
								}
								break;//do-while
							}
							_timeInQueue = _timeInQueue > _timeInQueueTmp ? _timeInQueue:_timeInQueueTmp;
							if( msg.internal.channelId != channelId )
							{
								PTRACE3( "ClientThread: message to wrong channelId=%08X - ignored", msg.internal.channelId );
								if (mgr->getOptimizePacketSize())
									continue; //do-while
								else
									break;
							}

							if( msg.type == _COMM_MSGTYPE_SERVICEREQUEST)
							{
								CommMsgParser parser( msg.body );
								UINT32 id;
								parser.parseUINT32(id);

								switch(id)
								{
								case _SERVICE_REQUEST_FORCE_KEEP_ALIVE:
									break;
								case _SERVICE_REQUEST_QUALITY:
									comm.serviceCallTransport( _SERVICE_REQUEST_QUALITY,0);
									break;
								default:
									PTRACE3("Unknown service request ID %X",id);
									break;
								}
								break;
							}
							else if( msg.type == _COMM_MSGTYPE_NONE )//'special message' flag
							{
								switch( msg.internal.slot )
								{
#ifdef USE_CONNECTOR_THREAD 
								case 0xFFFF:
									{
										PASSERT( state == 0 );
										PIPSocket::Descriptor descr;
										sock.extractDescriptor( descr );
										delete connector;
										connector = 0;
										int slot = rw.addSocket( descr.sock );
										PASSERT3(slot == 1);
										comm.construct(rw,*mgr, physConn, false, mgr->comm_logging);
										state = 1;
										comm.setKeepAlivePoll( _KeepAlivePoll );
										lastKeepAlive = SYSTEMTICKS();
										try
										{
											if( comm.isConnected() )
											{
												CommMsgBody msg_body;
												msg_body.composeINT32(0);
												msg_body.composeString(addr);
												comm.serviceCallTransport( _SERVICE_REQUEST_SETIPADDRESS,&msg_body);
												_physConn();
												state = 2;
												comm.setFirstRead();
											}

										}
										catch( PError& err )
										{
											PLog( "ClientThread error %s", err.why() );
											exit = true;
										}
										break;
									}
								case 1://reader/writer thread failure
									{
										PLog( "ClientThread: Connection failure notification received - closing connection" );
										failureWeight = msg.internal.patch0;
										errorCode = msg.internal.patch1;
										delete connector;
										connector = 0;
										PASSERT3( failureWeight >= 0 );
										exit = true;
										break;
									}
#endif
								case 2://DEBUG: emulate physical connection failure
									{
										PLog( "ClientThread: debugEmulateNetworkFailure received - closing" );
										exit = true;
										break;
									}
								case 3://force disconnect
									{
										PLog( "ClientThread: forceDisconnect received - closing" );
										exit = true;
										break;
									}
								default:
									PASSERT3( 0 );
									break;
								}
							}
							else
							{
								int slot = msg.internal.slot;
								PASSERT3( slot == 0);
								try
								{
									if (mgr->getOptimizePacketSize())
									{
										comm.storeMsg( msg.internal.mux, msg.internal.muxChannelId, msg );
									}
									else
									{
										comm.writeMsg( msg.internal.mux, msg.internal.muxChannelId, msg );
									}
								}
								catch( PError& err )
								{
									PLog( "ClientThread error %s", err.why() );
									exit = true;
									break;
								}
							}
						}while (exit == false && mgr->getOptimizePacketSize());
					}
					break;
				case PMultiSocketRW::waitRead:
					{
						try
						{
							comm.asyncReadOk();
							if( state == 1 && comm.isConnected() )
							{
								CommMsgBody msg_body;
								msg_body.composeINT32(0);
								msg_body.composeString(addr);
								comm.serviceCallTransport( _SERVICE_REQUEST_SETIPADDRESS,&msg_body);
								_physConn();
								state = 2;
								comm.setFirstRead();
							}
						}
						catch( PError& err )
						{
							PLog( "ClientThread read error %s", err.why() );
							exit = true;
							break;
						}
					}
					break;

				case PMultiSocketRW::waitWrite:
					{
						try
						{
							comm.asyncWriteOk();
							if( state == 1 && comm.isConnected() )
							{
								CommMsgBody msg_body;
								msg_body.composeINT32(0);
								msg_body.composeString(addr);
								comm.serviceCallTransport( _SERVICE_REQUEST_SETIPADDRESS,&msg_body);
								_physConn();
								state = 2;
								comm.setFirstRead();
							}
						}
						catch( PError& err )
						{
							PLog( "ClientThread write error %s", err.why() );
							exit = true;
							break;
						}
						break;
					}

				case PMultiSocketRW::waitTimeout:
					break;

				case PMultiSocketRW::waitFailed:
					{
						int errCode = rw.failedErrCode( 1 );
						PLog( "ClientThread closed: error %x - %s: %s",  errCode, rw.failedMsg( 1 ), rw.failedErrMsg( 1 ) );
						exit = true;
						break;
					}
					break;
				}
				if (exit)
					break;
			}
		}
		catch( PSSLError& err )
		{
			PLog( "Error in ClientThread: %s", err.why() );
			failureWeight = -1;
			errorCode=_SOCKET_SSLCERTIFICATEFAILED;
		}
		catch( _CommClientThreadError& err )
		{
			PLog( "Error in ClientThread: %s", err.why() );
			failureWeight = err.weight();
		}
		catch( PError& err )
		{
			PLog( "Error in ClientThread: %s", err.why() );
			failureWeight = 1;
		}

		if( state == 2 )
		{//channel error
			if( physConn )
				mgr->routingTable.physicallyDisconnected( physConn );
		}
		else
		{//unable to connect
			if( physConn )
				mgr->routingTable.physicalConnectFailed( physConn, failureWeight, errorCode );
		}
		if (state == 0)
		{
			sock.close();
#ifdef USE_CONNECTOR_THREAD 
			if( connector )
			{
				connector->join();
				delete connector;
				connector = 0;
			}
#endif
		}
		else if (state == 1)
		{
			rw.removeSocket( 1 );
		}

	}
	catch( PFatalError& err )
	{
		PLog( "Last hope catch in ClientManager stream: %s", err.why() );
		if (mgr->isExitOnFatalError())
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
		PLog( "Last hope catch in ClientManager stream: %s", err.why() );
	}
	PTRACE3( "ClientManager thread terminated" );
	_freeSocket( mgrIdx );//MUST be the last line before run() exits

}

#endif
