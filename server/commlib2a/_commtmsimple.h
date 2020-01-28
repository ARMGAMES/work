#ifndef _commtmsimple_h_included
#define _commtmsimple_h_included

#include "ppinclude.h"
#include "ppstring.h"
#include "pplogfile.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "_commcomm.h"
#include "commtmsimple.h"
#include "commhttp.h"
#include "commstreamencryption.h"

#define _HTTPSPROXY_ERROR_WEIGHT	5
#define _HTTPSPROXY_403ERROR_WEIGHT 50

class _CommSimpleThreadError : public PInternalError
{
private:
	int w;

public:
	_CommSimpleThreadError( const char* msg, int w_ )
		: PInternalError( msg )
	{
		w = w_;
	}

	int weight() const { return w; }
};

class _CommSimpleCommunicator
{
private:
	CommRoutingTable* routingTable;
	_CommPhysicalConnection* physConn;
	//	PIPSocket* sock;
	CommMsgQueue* q;

	PBlock rd;
	size_t rdOffset;

	BYTE* rdBuf;
	size_t rdBufSz;

public:
	static bool hasCompression( int type ) { return false; }
	void setCompression( int type,size_t BuffSize ) {}
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
	_CommSimpleCommunicator()
	{
		routingTable = 0;
		physConn = 0;
		//sock = 0;
		rdOffset = 0;
		rdBuf = 0;
		rdBufSz = 0;
		q = 0;
	}
	void construct( /*PIPSocket& sock_, */ CommMsgQueue& q_, CommRoutingTable& routingTable_, _CommPhysicalConnection* physConn_ )
	{
		routingTable = &routingTable_;
		physConn = physConn_;
		//sock = &sock_;
		q = &q_;
	}
	void setPhysConn( _CommPhysicalConnection* physConn_ )
	{
		physConn = physConn_;
	}
	void bytesRead( const BYTE* p, size_t sz )
	{
		memcpy( rd.append( sz ), p, sz );
	}

public://for Handler
	size_t writeIncomplete( const BYTE* p, size_t sz )
	{
		//		sock->sendComplete( p, sz );
		_CommMsg msg( _COMM_MSGTYPE_NONE, _COMM_MSGPRIORITIES - 1 );
		msg.body._composeVarBlock( p, sz );
		q->push( msg );
		return sz;
	}
	_CommAsyncRet asyncWriteOk()
	{
		return _CommAsyncRet();
	}
	size_t readIncomplete( BYTE* p, size_t sz )
	{
		PASSERT3( rdBuf == 0 );
		PASSERT3( rd.size() >= rdOffset );
		size_t avail = rd.size() - rdOffset;
		if( avail == 0 )
		{
			rdBuf = p;
			rdBufSz = sz;
			return 0;
		}

		if( avail < sz )
			sz = avail;
		memcpy( p, rd.ptr() + rdOffset, sz );
		rdOffset += sz;
		PASSERT3( rd.size() >= rdOffset );
		if( rdOffset > 256 )//why not?
		{
			size_t newSz = rd.size() - rdOffset;
			memmove( rd.ptr(), rd.ptr() + rdOffset, newSz );
			rd.cut( newSz );
			rdOffset = 0;
		}
		return sz;
	}
	_CommAsyncRet asyncReadOk()
	{
		_CommAsyncRet ret;
		if( rdBuf == 0 )
			return ret;
		BYTE* p = rdBuf;
		size_t sz = rdBufSz;
		rdBuf = 0;
		rdBufSz = 0;

		ret.read = readIncomplete( p, sz );
		return ret;
	}

	void msg( int mux, UINT32 muxChannelId, _CommMsg& msg )
	{
		//		PTRACE("MsgType = %x", msg.type);
		routingTable->routeIncomingMessage( physConn, mux, muxChannelId, msg );
	}

	void serviceCall(UINT32 id, CommMsgBody * msg_body)
	{
		routingTable->serviceCall(physConn,id,msg_body);
	}
};

//****************************************************************************

class _CommSimpleThreadReader;
class _CommSimpleThreadWriter;
class _CommSimpleThreadBase : public _CommManageableThread
{
public:
	enum { _KeepAlivePoll = 1000 };

protected:
	friend class _CommSimpleThreadReader;
	friend class _CommSimpleThreadWriter;
	friend class CommSimpleThreadManagerBase;
	CommSimpleThreadManagerBase* mgr;
	int mgrIdx;
	_CommPhysicalConnection* physConn;
	PString address;
	UINT32 channelId;

	//	PString httpsProxy;
	//	int httpsProxyPort;
	//	int freezeReconnectTimeout;

	int masterIdx;
	UINT32 masterChannelId;
	int slaveIdx;
	UINT32 slaveChannelId;

public:
	_CommSimpleThreadBase( CommSimpleThreadManagerBase& mgr_, int mgrIdx_, _CommPhysicalConnection* physConn_, UINT32 channelId_ );

	void _writerConnected()
	{
		_CommMsg msg( _COMM_MSGTYPE_NONE, _COMM_MSGPRIORITIES - 1 );
		msg.internal.slot = 9;
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
	void _pushRead( const BYTE* p, size_t sz )
	{
		_CommMsg msg( _COMM_MSGTYPE_NONE, _COMM_MSGPRIORITIES - 1 );
		msg.body._composeVarBlock( p, sz );
		msg.internal.slot = 4;
		msg.internal.channelId = channelId;
		push( msg );
	}

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

protected://to allow access for _CommSimpleThread
	void _freeSocket( int threadIdx )
	{
		mgr->_freeSocket( threadIdx );
	}
};

//****************************************************************************

class _CommSimpleThreadWriter : public PThread
{
private:
	_CommSimpleThreadBase* manager;
	CommMsgQueue q;
	PIPSocket sock;

public:
	_CommSimpleThreadWriter( _CommSimpleThreadBase& manager_, const PIPSocket::Descriptor& descr )
		: manager( &manager_ ),
		sock( descr )
	{
	}
	CommMsgQueue& queue() { return q; }

public:
	void run() override;
};

//****************************************************************************

class _CommSimpleThreadReader : public PThread
{
private:
	_CommSimpleThreadBase* manager;
	PIPSocket sock;

public:
	_CommSimpleThreadReader( _CommSimpleThreadBase& manager_, const PIPSocket::Descriptor& descr )
		: manager( &manager_ ),
		sock( descr )
	{
	}

public:
	void run() override;
};

//****************************************************************************

template< class _Comm >
class _CommSimpleThread : public _CommSimpleThreadBase
{
public:
	_CommSimpleThread( CommSimpleThreadManagerBase& mgr_, int mgrIdx_, _CommPhysicalConnection* physConn_, UINT32 channelId_ )
		: _CommSimpleThreadBase( mgr_, mgrIdx_, physConn_, channelId_ )
	{
	}
	void run() override;
};
template< class _Comm >
class _CommSimpleSSLThread : public _CommSimpleThread<_Comm>
{
public:
	_CommSimpleSSLThread( CommSimpleThreadManagerBase& mgr_, int mgrIdx_, _CommPhysicalConnection* physConn_, UINT32 channelId_ )
		: _CommSimpleThread<_Comm>( mgr_, mgrIdx_, physConn_, channelId_ )
	{
	}
	void run() override
	{
		_CommSimpleThread<_Comm>::run();
		PTRACE4("_CommSimpleSSLThread: threadCleanup() called");
		CommSSL::threadCleanup();
	}
};

template< class _Comm >
/* virtual */ void _CommSimpleThread< _Comm >::run()
{
	PTRACE3( "SimpleManager thread started" );
	PThread::setCurrentThreadPriority( mgr->getThreadPriority() );

	try
	{
		PString addr;
		int port;
		mgr->parseSocketAddr( addr, port, address );

		_Comm comm;
		int state = 0;	//0 - socket not connected, comm not constructed;
		//1 - socket connected, comm constructed, comm not connected;
		//2 - socket connected. comm constructed&connected, this is slave, switch request pending;
		//3 - socket connected, comm constructed&connected
		_CommSimpleThreadReader* reader = 0;
		_CommSimpleThreadWriter* writer = 0;
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
				PLog( "Unable to set TCP recv buffer %d",mgr->getTcpBufferRcv() );
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
		try
		{
			{
				PIPSocket::Descriptor descr;
				sock.copyDescriptor( descr );
				PASSERT3( writer == 0 );
				writer = new _CommSimpleThreadWriter( *this, descr );
				writer->start();
			}

			UINT32 lastKeepAlive = SYSTEMTICKS();
			for(;;)
			{
				UINT32 now = SYSTEMTICKS();
				int diff = now - lastKeepAlive;//subtracted as UINT32, then casted to int
				if( state == 0 )
				{
					if( mgr->getTcpConnectTimeout() && diff > mgr->getTcpConnectTimeout() * TICKS_PER_SECOND )//6s: double TCP SYN timeout
					{
						PTRACE3( "TCP connect %ds timeout exceeded - closing thread...", mgr->getTcpConnectTimeout() );
						failureWeight = 1;
						break;//forever
					}
					diff = 0;
				}
				else if( state == 1 )
				{
					if( mgr->getSslConnectTimeout() && diff > mgr->getSslConnectTimeout() * TICKS_PER_SECOND )//30s - should be enough to avoid disconnects due to the server overload
					{
						PTRACE3( "SSL handshake connect %ds timeout exceeded - closing thread...", mgr->getSslConnectTimeout() );
						failureWeight = 1;
						break;//forever
					}
					diff = 0;
				}
				else//if( state > 1 )
				{
					if( diff >= _KeepAlivePoll * TICKS_PER_SECOND / 1000 )
					{
						CommMsgBody msg_body;
						UINT32 readFreeze;
						bool Ok = comm.serviceCallTransport( _SERVICE_REQUEST_KEEP_ALIVE,&msg_body);
						if( !Ok )
						{
							PLog( "SimpleThread: keepAlive timeout exceeded - closing" );
							break;//forever
						}

						CommMsgParser parser( msg_body );
						parser.parseUINT32(readFreeze);
						if( mgr->getFreezeReconnectTimeout() && readFreeze > mgr->getFreezeReconnectTimeout() * TICKS_PER_SECOND && slaveIdx < 0 && masterIdx < 0 )
						{
							PTRACE3( "freezeReconnectTimeout exceeded - creating slave thread..." );
							mgr->createSlaveThread( slaveIdx, slaveChannelId, physConn->address, mgrIdx, channelId );
						}
						else if( readFreeze == 0 && slaveIdx >= 0 )
						{
							PTRACE3( "connection restored - killing slave thread..." );
							_CommMsg m;
							m.internal.slot = 5;//slave - die
							m.internal.channelId = slaveChannelId;
							mgr->pushToThread( slaveIdx, m );
							slaveIdx = -1;
							slaveChannelId = 0;
						}
						lastKeepAlive = now;
						diff = 0;
					}
				}

				_CommMsg msg;
				//PTRACE( "%d", diff );
				//UINT32 start = SYSTEMTICKS();
				int res = q.waitPop( msg, _KeepAlivePoll * TICKS_PER_SECOND / 1000 - diff );
				//UINT32 stop = SYSTEMTICKS();
				//UINT32 actual = stop - start;
				//if( actual > _KeepAlivePoll - diff )
				//	PTRACE( "!: %d", actual - (_KeepAlivePoll - diff ) );
				bool exit = false;
				switch( res )
				{
				case CommMsgQueue::waitPopOk:
					{
						if( msg.internal.channelId != channelId )
						{
							PTRACE3( "SimpleThread: message to wrong channelId=%08X - ignored", msg.internal.channelId );
							break;//switch
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


						switch( msg.internal.slot )
						{
						case 0://regular msg
							if( masterIdx >= 0 )
								break;//just in case - ignore
							comm.writeMsg( msg.internal.mux, msg.internal.muxChannelId, msg );
							break;
						case 1://reader/writer thread failure
							PLog( "SimpleThread: Read/write thread failure notification received - closing connection" );
							failureWeight = msg.internal.patch0;
							errorCode = msg.internal.patch1;
							PASSERT3( failureWeight >= 0 );
							exit = true;
							break;
						case 2://DEBUG: emulate physical connection failure
							PLog( "SimpleThread: debugEmulateNetworkFailure received - closing" );
							exit = true;
							break;
						case 3://force disconnect
							PLog( "SimpleThread: forceDisconnect received - closing" );
							exit = true;
							break;
						case 4://bytes read
							{
								const BYTE* p;
								size_t sz;
								CommMsgParser parser( msg.body );
								parser._parseVarBlock( p, sz );
								comm.bytesRead( p, sz );
								comm.asyncReadOk();
								if( state == 1 && comm.isConnected() )
								{
									if( masterIdx >= 0 )
									{
										_CommMsg m;
										m.internal.slot = 7;//slave connected
										m.internal.channelId = masterChannelId;
										mgr->pushToThread( masterIdx, m );
										state = 2;
									}
									else
									{
										CommMsgBody msg_body;
										msg_body.composeINT32(0);
										msg_body.composeString(addr);
										comm.serviceCallTransport( _SERVICE_REQUEST_SETIPADDRESS,&msg_body);
										mgr->routingTable.physicallyConnected( physConn, mgrIdx, 0, channelId, 0 );
										state = 3;
										comm.setFirstRead();
									}
									lastKeepAlive = SYSTEMTICKS();
								}
							}
							break;
						case 5://slave die
							{
								if( masterIdx < 0 )
									break;//just in case - ignore

								exit = true;
							}
							break;
						case 6://slave dead
							{
								if( slaveIdx < 0 )
									break;//ignore

								PTRACE3( "slave thread dead - recreating slave thread..." );

								//slave thread sudden death - recreate
								slaveIdx = -1;
								slaveChannelId = 0;
								mgr->createSlaveThread( slaveIdx, slaveChannelId, physConn->address, mgrIdx, channelId );
							}
							break;
						case 7://slave connected
							{
								if( slaveIdx < 0 )
									break;//ignore

								PTRACE3( "slave thread connected - switching to slave thread..." );
								mgr->routingTable.physicalReconnectBegin( physConn );

								_CommMsg m;
								m.body._composeFixedBlock( (const BYTE*)&physConn, sizeof(physConn) );
								m.internal.slot = 8;//switch to slave
								m.internal.channelId = slaveChannelId;
								mgr->pushToThread( slaveIdx, m );
								physConn = 0;
								slaveIdx = -1;
								slaveChannelId = 0;
								exit = true;
							}
							break;
						case 8://switch to slave
							{
								if( masterIdx < 0 )
									break;//ignore - just in case

								PTRACE3( "switching slave->master..." );
								PASSERT3( physConn == 0 );
								CommMsgParser parser( msg.body );
								const BYTE* p;
								parser._parseFixedBlock( p, sizeof(physConn) );
								memcpy( &physConn, p, sizeof(physConn) );
								comm.setPhysConn( physConn );
								masterIdx = -1;
								masterChannelId = 0;
								mgr->routingTable.physicalReconnectEnd( physConn, mgrIdx, 0, channelId );
								state = 3;
							}
							break;
						case 9://writer connected
							{
								PASSERT( state == 0 );
								comm.construct( writer->queue(), *mgr, physConn, false, mgr->comm_logging);
								state = 1;
								comm.setKeepAlivePoll( _KeepAlivePoll );
								lastKeepAlive = SYSTEMTICKS();

								PIPSocket::Descriptor descr;
								sock.copyDescriptor( descr );
								PASSERT3( reader == 0 );
								reader = new _CommSimpleThreadReader( *this, descr );
								reader->start();

								if( comm.isConnected() )
								{
									if( masterIdx >= 0 )
									{
										_CommMsg m;
										m.internal.slot = 7;//slave connected
										m.internal.channelId = masterChannelId;
										mgr->pushToThread( masterIdx, m );
										state = 2;
									}
									else
									{
										CommMsgBody msg_body;
										msg_body.composeINT32(0);
										msg_body.composeString(addr);
										comm.serviceCallTransport( _SERVICE_REQUEST_SETIPADDRESS,&msg_body);
										mgr->routingTable.physicallyConnected( physConn, mgrIdx, 0, channelId, 0 );
										state = 3;
									}
									//lastKeepAlive = SYSTEMTICKS(); - redundant
								}
							}
							break;
						default:
							PASSERT3( 0 );
							break;
						}//switch( slot )
					}
					break;

				case CommMsgQueue::waitPopShutdown:
					{
						exit = true;
						break;
					}
				}//switch( res )
				if( exit )
					break;//forever
			}//forever
		}
		catch( PSSLError& err )
		{
			PLog( "Error in SimpleWriter stream: %s", err.why() );
			failureWeight = -1;
			errorCode=_SOCKET_SSLCERTIFICATEFAILED;
		}

		catch( _CommSimpleThreadError& err )
		{
			PLog( "Error writing to socket: %s", err.why() );
			failureWeight = err.weight();
		}
		catch( PError& err )
		{
			PLog( "Error writing to socket: %s", err.why() );
			failureWeight = 1;
		}

		{
			PIPSocket::Descriptor descr;
			sock.copyDescriptor( descr );
			PTRACE3( "Closing socket %d", descr.sock );
		}
		sock.close();

		if( masterIdx >= 0 )
		{
			if( state == 2 )//slave switch pending
			{//wait until either switch or death
				for(;;)
				{
					_CommMsg msg;
					int res = q.waitPop( msg );
					bool exit = false;
					switch( res )
					{
					case CommMsgQueue::waitPopOk:
						{
							if( msg.internal.channelId != channelId )
							{
								PTRACE3( "SimpleThread: message to wrong channelId=%08X - ignored", msg.internal.channelId );
								break;//switch
							}

							switch( msg.internal.slot )
							{
							case 5://die
								exit = true;
								break;
							case 8://switch to slave
								{
									PTRACE3( "switching slave->master..." );
									PASSERT3( physConn == 0 );
									CommMsgParser parser( msg.body );
									const BYTE* p;
									parser._parseFixedBlock( p, sizeof(physConn) );
									memcpy( &physConn, p, sizeof(physConn) );
									comm.setPhysConn( physConn );
									masterIdx = -1;
									masterChannelId = 0;
									mgr->routingTable.physicalReconnectEnd( physConn, mgrIdx, 0, channelId );
									state = 3;
									exit = true;
								}
								break;
							}
						}
						break;
					case CommMsgQueue::waitPopShutdown:
						{
							exit = true;
							break;
						}
					}
					if( exit )
						break;//forever
				}
			}

			_CommMsg m;
			m.internal.slot = 6;//slave dead
			m.internal.channelId = masterChannelId;
			mgr->pushToThread( masterIdx, m );
		}

		if( slaveIdx >= 0 )
		{//kill slave
			_CommMsg m;
			m.internal.slot = 5;//slave die
			m.internal.channelId = slaveChannelId;
			mgr->pushToThread( slaveIdx, m );
		}

		if( state == 3 )
		{//channel error
			if( physConn )
				mgr->routingTable.physicallyDisconnected( physConn );
		}
		else
		{//unable to connect
			if( physConn )
				mgr->routingTable.physicalConnectFailed( physConn, failureWeight, errorCode );
		}
		if( reader )
		{
			reader->join();
			delete reader;
		}
		if( writer )
		{
			writer->queue().shutdown();
			writer->join();
			delete writer;
		}
	}
	catch( PFatalError& err )
	{
		PLog( "Last hope catch in SimpleManager stream: %s", err.why() );
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
		PLog( "Last hope catch in SimpleManager stream: %s", err.why() );
	}
	PTRACE3( "SimpleManager thread terminated" );

	_freeSocket( mgrIdx );//MUST be the last line before run() exits
}

#endif
