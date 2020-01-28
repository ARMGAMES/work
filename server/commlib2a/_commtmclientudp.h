#ifndef _commtmclientudp_h_included
#define _commtmclientudp_h_included


#include "ppinclude.h"
#include "ppstring.h"
#include "pplogfile.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "_commcomm.h"
#include "commtmclientudp.h"

#ifdef PWIN32

#define NUM_WSABUF 5

struct _CommClientUDPCommunicator
{
private:
	PMultiSocketRW* rw;
	CommRoutingTable* routingTable;
	_CommPhysicalConnection* physConn;


	BYTE sendBuffer[UDP_TOTAL_MAXIMUM_PACKET_SIZE];
	BYTE recBuffer[UDP_TOTAL_MAXIMUM_PACKET_SIZE];
	bool sendBusy;
	size_t readReady;
	WSABUF data[NUM_WSABUF];


	void * key;

public:
	void construct( PMultiSocketRW& rw_, CommRoutingTable& routingTable_,_CommPhysicalConnection* physConn_ )
	{
		rw = &rw_;
		routingTable = &routingTable_;
		physConn = physConn_;
		sendBusy = false;
		readReady = rw->read( 1, recBuffer, 600, true );
	}
/*
	size_t writeIncomplete( const BYTE* p, size_t sz )
	{
		return rw->write( 1, p, sz, true );
	}
	size_t readIncomplete( BYTE* p, size_t sz )
	{
		return rw->read( 1, p, sz, true );
	}
*/
	BYTE * asyncReadOk(size_t & size)
	{
		if (readReady)
		{
			size = readReady;
			return recBuffer;
		}
		_CommAsyncRet ret;
		if( rw->readPending( 1 ) )
		{
			readReady = rw->read( 1, recBuffer, UDP_TOTAL_MAXIMUM_PACKET_SIZE, true );
			if (readReady)
			{
				size = readReady;
				return recBuffer;
			}
			else
			{
				return 0;
			}
		}
		ret.read = rw->getReadSize( 1 );
		size = ret.read;
		return recBuffer;
	}

	void asyncReadCompleted()
	{
		readReady = rw->read( 1, recBuffer, UDP_TOTAL_MAXIMUM_PACKET_SIZE, true );
	}

	bool isReadReady() const
	{
		return !!readReady;
	}


	void * asyncWriteOk(size_t &size)
	{
		_CommAsyncRet ret;
		if( rw->writePending( 1 ) )
			return 0;
		ret.written = rw->getWrittenSize( 1 );
		size = ret.written;
		sendBusy = false;
		return key;
	}

	void msg( _CommMsg& msg )
	{
		routingTable->routeIncomingMessage( physConn, 0, 0, msg );
	}

	void forceSlotFailure( const char* errMsg, int errCode = 0)
	{
		rw->forceSlotFailure( 1, errMsg, errCode );
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
		BYTE * p = sendBuffer;
		for ( int i=0;i<n;i++)
		{
			memcpy(p, buf[i].buf, buf[i].len);
			p+=buf[i].len;
		}
		size_t wr = rw->write( 1, sendBuffer, p - sendBuffer, true );
		if (wr)
			sendBusy = false;
		return wr;
	}

};



class _CommClientUDPThreadBase : public _CommManageableThread
{
public:

protected:
	friend class CommClientUDPThreadManager;

	_CommPhysicalConnection* physConn;
	CommClientUDPThreadManager* mgr;
	PString address;
	UINT32 channelId;
	int mgrIdx;

public:
	_CommClientUDPThreadBase( CommClientUDPThreadManager& mgr_, int mgrIdx_, _CommPhysicalConnection* physConn_, UINT32 channelId_ );

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


protected:
	void _freeSocket( int idx )
	{
		mgr->_freeSocket( idx );
	}
};



//****************************************************************************

template< class _Comm >
class _CommClientUDPThread : public _CommClientUDPThreadBase
{
private:


public:
	_CommClientUDPThread( CommClientUDPThreadManager& mgr_, int mgrIdx_, _CommPhysicalConnection* physConn_, UINT32 channelId_)
		: _CommClientUDPThreadBase( mgr_, mgrIdx_,physConn_,channelId_ )
	{
	}

public:
	void run() override;

private:
	void _physConn();
};



template< class _Comm >
void _CommClientUDPThread< _Comm >::_physConn()
{
	mgr->routingTable.physicallyConnected(physConn, mgrIdx, 0, channelId, 0 );
}

template< class _Comm >
/* virtual */ void _CommClientUDPThread< _Comm >::run()
{
	try
	{
		PMultiSocketRW rw( q.getSocket(), 1);

		PString addr;
		int port;
		mgr->parseSocketAddr( addr, port, address );
		_Comm comm;

		PUDPSocket sock;
		{
			PBaseSocket::Descriptor descr;
			sock.copyDescriptor( descr );
			PTRACE3( "Creating socket %d", descr.sock );
		}

		int failureWeight = 0;
		int errorCode = 0;
		bool exit = false;
		UINT32 lastKeepAlive = SYSTEMTICKS();
		int state =1;

		try
		{
			const char* connAddr = addr;
			int connPort = port;
			PLog( "Connecting to %s:%d", connAddr, connPort );
			sock.connect( connAddr, connPort);
			PBaseSocket::Descriptor descr;
			sock.extractDescriptor( descr );
			int slot = rw.addSocket( descr.sock );
			PASSERT3(slot == 1);
			comm.construct(rw,*mgr, physConn, 516,  mgr->comm_logging);
			lastKeepAlive = SYSTEMTICKS();
			try
			{
				if( comm.isConnected() )
				{
					CommMsgBody msg_body;
					msg_body.composeINT32(0);
					msg_body.composeString(addr);
					comm.serviceCallTransport( _SERVICE_REQUEST_SETIPADDRESS,&msg_body);
					state =2;
					_physConn();
//					comm.setFirstRead();
				}
			}
			catch( PError& err )
			{
				PLog( "ClientUDPThread error %s", err.why() );
				exit = true;
			}

			int w = -1;
			while(!exit)
			{
				UINT32 now = SYSTEMTICKS();
				int diff = now - lastKeepAlive;//subtracted as UINT32, then casted to int
				if( state == 0 )
				{
				}
				else if( state == 1 )
				{
					if( mgr->getUdpConnectTimeout() && diff > mgr->getUdpConnectTimeout() * TICKS_PER_SECOND )
					{
						PTRACE3( "UDP handshake connect %ds timeout exceeded - closing thread...", mgr->getUdpConnectTimeout() );
						failureWeight = 1;
						exit = true;
						break;//forever
					}
					diff = 0;
				}
				else//if( state > 1 )
				{
					if( diff >= mgr->getKeepAlivePoll() * TICKS_PER_SECOND / 1000 )
					{
						bool Ok = comm.serviceCallTransport( _SERVICE_REQUEST_KEEP_ALIVE,0);
						if( !Ok )
						{
							PLog( "ClientThread: keepAlive timeout exceeded - closing" );
							break;//forever
						}
						lastKeepAlive = now;
						diff = 0;
					}
				}

				int qSize;
				if (w == PMultiSocketRW::waitQueue)
					qSize = q.sizeAndReset();
				else
					qSize = q.size();
				if (qSize > 0 )
				{
					w = PMultiSocketRW::waitQueue;
				}
				else if (comm.isReadReady())
				{
					w = PMultiSocketRW::waitRead;
				}
				else
				{
					UINT32 wait = diff < mgr->getKeepAlivePoll() ? mgr->getKeepAlivePoll() - diff : 0;
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
							PTRACE3( "ClientUDPThread shutdown - exiting");
							exit = true;
							break;
						}
						//PTRACE3( "_DBG: RW:waitQueue qSize=%d", q.size() );
						_CommMsg msg;
						if( !q.pop( msg ) )
						{
							break;//do-while
						}
						if( msg.internal.channelId != channelId )
						{
							PTRACE3( "ClientUDPThread: message to wrong channelId=%08X - ignored", msg.internal.channelId );
							break;
						}
						if( msg.type == _COMM_MSGTYPE_SERVICEREQUEST)
						{
							CommMsgParser parser( msg.body );
							UINT32 id;
							parser.parseUINT32(id);
							PTRACE3("Unknown service request ID %X",id);
							break;
						}
						else if( msg.type == _COMM_MSGTYPE_NONE )//'special message' flag
						{
							switch( msg.internal.slot )
							{
							case 2://DEBUG: emulate physical connection failure
								{
									PLog( "ClientUDPThread: debugEmulateNetworkFailure received - closing" );
									exit = true;
									break;
								}
							case 3://force disconnect
								{
									PLog( "ClientUDPThread: forceDisconnect received - closing" );
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
								comm.writeMsg( msg );
							}
							catch( PError& err )
							{
								PLog( "ClientUDPThread error %s", err.why() );
								exit = true;
								break;
							}
						}
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
//								comm.setFirstRead();
							}
							comm.asyncReadCompleted();
						}
						catch( PError& err )
						{
							PLog( "ClientUDPThread read error %s", err.why() );
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
//								comm.setFirstRead();
							}

						}
						catch( PError& err )
						{
							PLog( "ClientUDPThread write error %s", err.why() );
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
						PLog( "ClientUDPThread closed: error %x - %s: %s",  errCode, rw.failedMsg( 1 ), rw.failedErrMsg( 1 ) );
						exit = true;
						break;
					}
					break;
				}
				if (exit)
					break;
			}
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
		rw.removeSocket( 1 );
	}
	catch( PFatalError& err )
	{
		PLog( "Last hope catch in ClientUDPManager stream: %s", err.why() );
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
		PLog( "Last hope catch in ClientUDPManager stream: %s", err.why() );
	}
	PTRACE3( "ClientUDPManager thread terminated" );
	_freeSocket( mgrIdx );//MUST be the last line before run() exits

}

#endif
#endif
