#ifndef _commtmwebsocket_h_included
#define _commtmwebsocket_h_included

#include <pplib.h>
#include <commlib.h>
#include "commrtable.h"
#include "_commcomm.h"
#include "_commtmrw.h"
#include "commtmwebsocket.h"


struct _CommThreadDownCommunicator : public _CommVirtualCommunicator
{
private:
	PMultiSocketRW* rw;
	int slot;
	CommRoutingTable* routingTable;
	_CommPhysicalConnection* physConn;

public:
	void construct( PMultiSocketRW& rw_, int slot_, CommRoutingTable& routingTable_) 
	{
		rw = &rw_;
		slot = slot_;
		routingTable = &routingTable_;
		physConn=0;
	}
	void init( _CommPhysicalConnection* physConn_ )
	{
		physConn = physConn_;
	}
	bool initConnection(UINT32 &checkSum) const 
	{
		return true;
	}

	size_t writeIncomplete( const BYTE* p, size_t sz )
	{
		return rw->write( slot, p, sz, true );
	}
	size_t readIncomplete( BYTE* p, size_t sz )
	{
		return rw->read( slot, p, sz, true );
	}
	_CommAsyncRet asyncReadOk() 
	{
		_CommAsyncRet ret;
		if( rw->readPending( slot ) )
			return ret;
		ret.read = rw->getReadSize( slot );
		return ret;
	}
	_CommAsyncRet asyncWriteOk() 
	{
		_CommAsyncRet ret;
		if( rw->writePending( slot ) )
			return ret;
		ret.written = rw->getWrittenSize( slot );
		return ret;
	}
	void forceSlotFailure( const char* errMsg, int errCode = 0)
	{
		rw->forceSlotFailure( slot, errMsg, errCode );
	}
	void msg( int mux, UINT32 muxChannelId, _CommMsg& msg )
	{
		routingTable->routeIncomingMessage( physConn, mux, muxChannelId, msg );
	}
};




class _CommThreadWebSocketBase : public _CommRWThreadBase
{
public:
enum { _KeepAlivePoll = 1000 };
	_CommThreadWebSocketBase( CommThreadManagerWebSocket& mgr_, int mgrIdx_): _CommRWThreadBase(mgr_, mgrIdx_,false)
	{
	}
	int maxRWSockets() const override
	{
		return PMultiSocketRW::MaxSockets - 1;
	}
};


template< class _Comm >
struct _CommThreadWebSocketData
{
	CommRWThreadManagerBase::Socket sock;
	UINT32 channelId;

	_Comm comm;
	UINT32 timeConnect;
	bool commInit;
	bool commConn;
};

template< class _Comm >
class _CommThreadWebSocket : public _CommThreadWebSocketBase
{
public:
	_CommThreadWebSocket( CommThreadManagerWebSocket& mgr_, int mgrIdx_ )
		: _CommThreadWebSocketBase(mgr_,mgrIdx_)
	{
	}

public:

	void run() override;
private:

	void _physConn( int slot, _CommThreadWebSocketData< _Comm >& data );
};

template< class _Comm >
class _CommThreadWebSocketSSL : public _CommThreadWebSocket<_Comm>
{
private:
public:
	_CommThreadWebSocketSSL( CommThreadManagerWebSocket& mgr_, int mgrIdx_ )
		: _CommThreadWebSocket<_Comm>( mgr_ ,mgrIdx_)
	{
	}

public:

	void run() override
	{
		_CommThreadWebSocket<_Comm>::run();
		CommSSL::threadCleanup();
	};

};


template< class _Comm >
void _CommThreadWebSocket< _Comm >::_physConn( int slot, _CommThreadWebSocketData< _Comm >& data )
{
	mgr.routingTable.physicallyConnected( data.sock.physConn, mgrIdx, slot, data.channelId, 0 );
}


template< class _Comm >
/* virtual */ void _CommThreadWebSocket< _Comm >::run()
{
	try
	{
		PMultiSocketRW rw( q.getSocket(), PMultiSocketRW::MaxSockets - 1 );
		_CommThreadWebSocketData< _Comm > rwData[ PMultiSocketRW::MaxSockets ];

		for( int i=0; i < PMultiSocketRW::MaxSockets ; ++i )
		{
			rwData[ i ].comm.construct( rw, i, mgr, mgrIdx );
			rwData[ i ].commInit = false;
			rwData[ i ].commConn = false;
			rwData[ i ].channelId = 0;
			rwData[ i ].timeConnect = 0;
		}
		UINT32 lastKeepAlive = SYSTEMTICKS();
		int w = -1;
		for(;;)
		{
			int slot = 0;
			UINT32 now = SYSTEMTICKS();
			UINT32 diff = now - lastKeepAlive;
			if( diff >= _KeepAlivePoll )
			{

				for( int i=0; i < PMultiSocketRW::MaxSockets ; ++i )
				{
					if( rwData[ i ].commInit)
					{
						_CommThreadWebSocketData< _Comm >& data = rwData[ i ];
						try
						{
							if(!data.commConn && mgr.handshakeTimeout > 0 &&  (now -  data.timeConnect) > mgr.handshakeTimeout)
							{
								data.comm.forceSlotFailure( "Handshake timeout expired", _SOCKET_HANDSHAKE_TIMEOUT_EXPIRED);
							}
							if(data.commConn)
							{
								CommMsgBody msg;
								bool Ok = data.comm.serviceCallTransport( _SERVICE_REQUEST_KEEP_ALIVE, &msg);
								if( !Ok )
									data.comm.forceSlotFailure( "keepAlive timeout expired", _SOCKET_TIMEOUT_EXPIRED );
							}
						}
						catch( PError& err )
						{
							PLog( "_CommThreadWebSocket[ %d ][ %d ] exception/1: %s", mgrIdx, i, err.why() );
							data.comm.forceSlotFailure( err.why(),_SOCKET_EXCEPTION );
						}
					}
				}
				lastKeepAlive = now;
				diff = 0;
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
			else
			{
				UINT32 wait = diff < _KeepAlivePoll ? _KeepAlivePoll - diff : 0;
				w = rw.wait( slot, wait );
				if (w == PMultiSocketRW::waitQueue)
					q.increaseAutoResetCounter();
			}

			switch( w )
			{
			case PMultiSocketRW::waitQueue:
				{
					if( q.shuttingDown() )
					{
						PTRACE3( "_CommThreadWebSocket[ %d ] shutdown - exiting", mgrIdx );
						rw.shutdownSockets();
						exit();
						return;
					}

					_CommMsg msg;
					if( !q.pop( msg ) )
					{
						PTRACE5( "_CommThreadWebSocket[ %d ]: empty queue awaken - ignored", mgrIdx );
						break;//do-while
					}

					if( msg.type == _COMM_MSGTYPE_NONE )
					{
						if( msg.internal.slot == 0xFFFF )
						{
							CommMsgParser parser( msg.body );
							const BYTE* sock_;
							parser._parseFixedBlock( sock_, sizeof(CommRWThreadManagerBase::Socket) );
							CommRWThreadManagerBase::Socket* sock = (CommRWThreadManagerBase::Socket*)sock_;
							int slot = rw.addSocket( sock->sockDescr.sock );

							PTRACE3( "_CommThreadWebSocket[ %d ][ %d ]: Socket placed to slot",  mgrIdx, slot );

							_CommThreadWebSocketData< _Comm >& data = rwData[ slot ];
							try
							{
								PString addr;
								PBaseSocket::address( addr, sock->sockDescr );

								data.channelId = mgr.nextChannelId();
								data.sock = *sock;

								data.comm.init( data.sock.physConn);
								data.commInit = true;
								data.timeConnect = SYSTEMTICKS();

								if( data.comm.isConnected() )
								{
									_physConn( slot, data );
									data.commConn = true;
									data.comm.setFirstRead();
								}
								else
								{
									data.commConn = false;
								}

							}
							catch( PError& err )
							{
								PLog( "_CommThreadWebSocket[ %d ][ %d ] exception/2: %s", mgrIdx, slot, err.why() );
								data.comm.forceSlotFailure( err.why(),_SOCKET_EXCEPTION );
							}

						}
						else//forceDisconnect
						{
							slot = msg.internal.slot;

							PASSERT3( slot < PMultiSocketRW::MaxSockets );
							if( msg.internal.channelId != rwData[ slot ].channelId )
							{
								PLog( "_CommThreadWebSocket[ %d ][ %d ]: forceDisconnect obsolete channelId=%08X - ignored",  mgrIdx, slot, msg.internal.channelId );
								break;
							}
							_CommThreadWebSocketData< _Comm >& data = rwData[ slot ];
							data.comm.forceSlotFailure( "forceDisconnect received", -1 );
						}
					}
					else
					{
						slot = msg.internal.slot;
						PASSERT3( slot < PMultiSocketRW::MaxSockets );
						if( msg.internal.channelId != rwData[ slot ].channelId )
						{
							PLog( "_CommThreadWebSocket[ %d ][ %d ]: message to obsolete channelId=%08X - ignored",  mgrIdx, slot, msg.internal.channelId );
							break;
						}
						_CommThreadWebSocketData< _Comm >& data = rwData[ slot ];
						try
						{
							data.comm.writeMsg( msg );
						}
						catch( PError& err )
						{
							PLog( "_CommThreadWebSocket[ %d ][ %d ] exception/3: %s",  mgrIdx, slot, err.why() );
							data.comm.forceSlotFailure( err.why(),_SOCKET_EXCEPTION );
						}
					}
				}
				break;

			case PMultiSocketRW::waitRead:
				{
					_CommThreadWebSocketData< _Comm >& data = rwData[ slot ];
					try
					{
						data.comm.asyncReadOk();
						if (!data.commConn && data.comm.isConnected())
						{
							_physConn( slot, data );
							data.commConn = true;
							data.comm.setFirstRead();
						}
					}
					catch( PError& err )
					{
						PLog( "_CommThreadWebSocket[ %d ][ %d ] exception/rd: %s",  mgrIdx, slot, err.why() );
						data.comm.forceSlotFailure( err.why(),_SOCKET_EXCEPTION );
					}
				}
				break;

			case PMultiSocketRW::waitWrite:
				{
					_CommThreadWebSocketData< _Comm >& data = rwData[ slot ];
					try
					{
						data.comm.asyncWriteOk();
						if (!data.commConn && data.comm.isConnected())
						{
							_physConn( slot, data );
							data.commConn = true;
							data.comm.setFirstRead();
						}

					}
					catch( PError& err )
					{
						PLog( "_CommThreadWebSocket[ %d ][ %d ] exception/wr: %s",  mgrIdx, slot, err.why() );
						data.comm.forceSlotFailure( err.why(),_SOCKET_EXCEPTION );
					}
				}
				break;

			case PMultiSocketRW::waitTimeout:

				break;

			case PMultiSocketRW::waitFailed:
				{
					_CommThreadWebSocketData< _Comm >& data = rwData[ slot ];

					int errCode = rw.failedErrCode( slot );
					PString errStr;
					errStr.append(rw.failedMsg( slot )).append(" : ").append(rw.failedErrMsg( slot ));
					PLog( "_CommThreadWebSocket[ %d ][ %d ]: closed: error %d - %s", mgrIdx, slot, errCode, errStr.c_str());

					rw.removeSocket( slot );
					mgr.routingTable.physicallyDisconnected( data.sock.physConn, errCode );
					data.sock.physConn = 0;
					data.comm.deinit();
					data.commInit = false;
					data.commConn = false;
					data.channelId = mgr.nextChannelId();//to enable protection from using failed connections - see comment above
					int idx = mgrIdx;
					if( _freeSocket( idx ) )
					{
						PTRACE3( "Exiting _CommThreadComm[ %d ]",  idx );
						return;
					}
				}
				break;
			}
		}
	}
	catch( PFatalError& err )
	{
		PLogError( "_CommThreadWebSocket[ %d ] last hope catch: %s", mgrIdx, err.why() );
		exit();
	}
	catch( PError& err )
	{
		PLogError( "_CommThreadWebSocket[ %d ] last hope catch: %s",  mgrIdx, err.why() );
		exit();
	}
	catch(  exception& exc  )
	{
		PLogError( "_CommThreadWebSocket[ %d ] last hope STD exception catch: %s", mgrIdx,  exc.what() );
		exit();
	}
	catch( ... )
	{
		PLogError( "_CommThreadWebSocket[ %d ] last hope ... catch",  mgrIdx );
		exit();
	}
}








#endif
