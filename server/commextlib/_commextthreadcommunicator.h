#ifndef _commextthreadcommunicator_h_included
#define _commextthreadcommunicator_h_included

#include <pplib.h>
#include <commlib.h>
#include "_commcomm.h"
#include "_commextthreadmanager.h"


struct _CommExtThreadDownCommunicator : public _CommVirtualCommunicator
{
	PMultiSocketRW* rw;
	int slot;
	void construct( PMultiSocketRW& rw_, int slot_)
	{
		rw = &rw_;
		slot = slot_;
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
	bool initConnection(UINT32 & checkSumm)
	{
		checkSumm = 0;
		return true;
	}
};

struct _CommExtThreadUpCommunicator 
{
	UINT32 connId;
	CommExtThreadManager * mgr;
	void construct( CommExtThreadManager &mgr_)
	{
		connId = 0;
		mgr = &mgr_;
	}
	void init(UINT32 connId_)
	{
		connId = connId_;
	}
	void deinit()
	{
		connId =0;
	}
	void readDone(CommMsgBody &body)
	{
		mgr->readDone(connId, body);
	}
	void sentDone()
	{
		mgr->sentDone(connId);
	}
};


template< class _Comm >
struct _CommExtThreadCommunicatorData
{
	UINT32 channelId;
	UINT32 connId;
	_Comm comm;
	bool commInit;
	bool commConnected;
	UINT32 connectCounter;
};

template< class _Comm >
class _CommExtThreadComm : public _CommExtManagableThread
{
private:
public:
	_CommExtThreadComm( CommExtThreadManager& mgr_, int mgrIdx_ )
		: _CommExtManagableThread( mgr_,mgrIdx_)
	{
	}

public:

	void run() override;
};

template< class _Comm >
class _CommExtSSLThreadComm : public _CommExtThreadComm<_Comm>
{
private:
public:
	_CommExtSSLThreadComm( CommExtThreadManager& mgr_, int mgrIdx_ )
		: _CommExtThreadComm<_Comm>( mgr_ ,mgrIdx_)
	{
	}

public:

	void run() override
	{
		_CommExtThreadComm<_Comm>::run();
		CommSSL::threadCleanup();
	};

};

template< class _Comm >
/* virtual */ void _CommExtThreadComm< _Comm >::run()
{
	try
	{
		PMultiSocketRW rw( q.getSocket(),PMultiSocketRW::MaxSockets - 1 );
		_CommExtThreadCommunicatorData< _Comm > rwData[ PMultiSocketRW::MaxSockets ];

		for( int i=0; i < PMultiSocketRW::MaxSockets ; ++i )
		{
			rwData[ i ].comm.construct( rw, i, mgr );
			rwData[ i ].commInit = false;
			rwData[ i ].commConnected = false;
			rwData[ i ].channelId = 0;
			rwData[ i ].connId = 0;
			rwData[ i ].connectCounter = 0;
		}
		UINT32 lastKeepAlive = SYSTEMTICKS();
		int w = -1;
		for(;;)
		{
			int slot = 0;
			UINT32 now = SYSTEMTICKS();
			UINT32 diff = now - lastKeepAlive;
			if( mgr.handshakeTimeout > 0 && diff >= _KeepAlivePoll )
			{

				for( int i=0; i < PMultiSocketRW::MaxSockets ; ++i )
				{
					if( rwData[ i ].commInit)
					{
						_CommExtThreadCommunicatorData< _Comm >& data = rwData[ i ];
						try
						{
							if(!data.commConnected && ++data.connectCounter > mgr.handshakeTimeout)
							{
								data.comm.forceSlotFailure( "Handshake timeout expired", _SOCKET_HANDSHAKE_TIMEOUT_EXPIRED);
							}
						}
						catch( PError& err )
						{
							PLog( "_CommExtThreadComm[ %d ][ %d ] exception/1: %s", mgrIdx, i, err.why() );
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
			if (qSize > 0)
			{
				w = PMultiSocketRW::waitQueue;
			}
			else
			{
				UINT32 wait = mgr.handshakeTimeout > 0 ? (diff < _KeepAlivePoll ? _KeepAlivePoll - diff : 0) : INFINITE;
				w = rw.wait(slot, wait);
				if (w == PMultiSocketRW::waitQueue)
					q.increaseAutoResetCounter();
			}
			switch( w )
			{
			case PMultiSocketRW::waitQueue:
				{
					if( q.shuttingDown() )
					{
						PTRACE3( "_CommExtThreadComm[ %d ] shutdown - exiting", mgrIdx );
						rw.shutdownSockets();
						exit();
						return;
					}

					_CommMsg msg;
					if( !q.pop( msg ) )
					{
						PTRACE5( "_CommExtThreadComm[ %d ]: empty queue awaken - ignored", mgrIdx );
						break;//do-while
					}

					if( msg.type == _COMM_MSGTYPE_NONE )
					{
						if( msg.internal.slot == 0xFFFF )
						{
							CommMsgParser parser( msg.body );
							UINT32 connId;
							PIPSocket::Descriptor *descr;
							bool incoming;
							void **ssl_ = nullptr;
							parser.parseUINT32(connId);
							parser._parseFixedBlock((const BYTE *&) descr, sizeof(PIPSocket::Descriptor));
							parser.parseBOOL(incoming);
							if (!incoming)
							{
								parser._parseFixedBlock((const BYTE *&) ssl_, sizeof(void**));
							}
							int slot = rw.addSocket(descr->sock);
							PTRACE3( "_CommExtThreadComm[ %d ][ %d ] connId=%08X: Socket placed to slot",  mgrIdx, slot, connId );
							_CommExtThreadCommunicatorData< _Comm >& data = rwData[ slot ];
							try
							{
								PString addr;
								PIPSocket::address(addr, *descr);
								int port = PIPSocket::port(*descr);
								PString addr2;
								mgr.composeSocketAddr(addr2, addr, port);
								data.channelId = _nextChannelId();
								data.connId = connId;
								data.comm.init(connId, incoming, (ssl_?*ssl_:0));
								data.commInit = true;
								mgr.connecting(data.connId,addr2, mgrIdx, slot, data.channelId);
								data.connectCounter = 0;
								if (data.comm.isActive())
								{
									data.commConnected = true;
									mgr.connected(data.connId);
									data.comm.readStart();
								}
							}
							catch( PError& err )
							{
								PLog( "_CommExtThreadComm[ %d ][ %d ] connId=%08X: exception/2: %s",  mgrIdx, slot, connId, err.why() );
								data.comm.forceSlotFailure( err.why(),_SOCKET_EXCEPTION );
							}
						}
						else//forceDisconnect
						{
							slot = msg.internal.slot;

							PASSERT3( slot < PMultiSocketRW::MaxSockets );
							if( msg.internal.channelId != rwData[ slot ].channelId )
							{
								PLog( "_CommExtThreadComm[ %d ][ %d ]: forceDisconnect obsolete channelId=%08X - ignored",  mgrIdx, slot, msg.internal.channelId );
								break;
							}
							_CommExtThreadCommunicatorData< _Comm >& data = rwData[ slot ];
							data.comm.forceSlotFailure( "forceDisconnect received", -1 );
						}
					}
					else
					{
						slot = msg.internal.slot;
						PASSERT3( slot < PMultiSocketRW::MaxSockets );
						if( msg.internal.channelId != rwData[ slot ].channelId )
						{
							PLog( "_CommExtThreadComm[ %d ][ %d ]: message to obsolete channelId=%08X - ignored",  mgrIdx, slot, msg.internal.channelId );
							break;
						}
						_CommExtThreadCommunicatorData< _Comm >& data = rwData[ slot ];
						try
						{
							data.comm.writeMsg( msg );
						}
						catch( PError& err )
						{
							PLog( "_CommExtThreadComm[ %d ][ %d ] connId=%08X, exception/3: %s",  mgrIdx, slot, data.connId, err.why() );
							data.comm.forceSlotFailure( err.why(),_SOCKET_EXCEPTION );
						}
					}
				}
				break;

			case PMultiSocketRW::waitRead:
				{
					_CommExtThreadCommunicatorData< _Comm >& data = rwData[ slot ];
					try
					{
						data.comm.asyncReadOk();
						if (!data.commConnected && data.comm.isActive())
						{
							data.commConnected = true;
							mgr.connected(data.connId);
							data.comm.readStart();
						}


					}
					catch( PError& err )
					{
						PLog( "_CommExtThreadComm[ %d ][ %d ], connId=%08X, exception/rd: %s",  mgrIdx, slot, data.connId, err.why() );
						data.comm.forceSlotFailure( err.why(),_SOCKET_EXCEPTION );
					}
				}
				break;

			case PMultiSocketRW::waitWrite:
				{
					_CommExtThreadCommunicatorData< _Comm >& data = rwData[ slot ];
					try
					{
						data.comm.asyncWriteOk();
						if (!data.commConnected && data.comm.isActive())
						{
							data.commConnected = true;
							mgr.connected(data.connId);
							data.comm.readStart();
						}

					}
					catch( PError& err )
					{
						PLog( "_CommExtThreadComm[ %d ][ %d ] connId=%08X, exception/wr: %s",  mgrIdx, slot, data.connId, err.why() );
						data.comm.forceSlotFailure( err.why(),_SOCKET_EXCEPTION );
					}
				}
				break;

			case PMultiSocketRW::waitTimeout:

				break;

			case PMultiSocketRW::waitFailed:
				{
					_CommExtThreadCommunicatorData< _Comm >& data = rwData[ slot ];

					int errCode = rw.failedErrCode( slot );
					data.comm.failed(errCode);
					PString errStr;
					errStr.append(rw.failedMsg( slot )).append(" : ").append(rw.failedErrMsg( slot ));
					PLog( "_CommExtThreadComm[ %d ][ %d ] connId=%08X: closed: error %x - %s", mgrIdx, slot, data.connId, errCode, errStr.c_str());
					rw.removeSocket( slot );
					mgr.disconnected(data.connId, errCode, errStr.c_str() );
					data.comm.deinit();
					data.commInit = false;
					data.commConnected = false;
					data.connectCounter = 0;
					data.connId=0;
					data.channelId = _nextChannelId();//to enable protection from using failed connections - see comment above

					int idx = mgrIdx;
					if( _freeSocket( idx ) )
					{
						PTRACE3( "Exiting _CommExtThreadComm[ %d ]",  idx );
						return;
					}
				}
				break;
			}
		}
	}
	catch( PFatalError& err )
	{
		PLogError( "_CommExtThreadComm[ %d ] last hope catch: %s", mgrIdx, err.why() );
		exit();
	}
	catch( PError& err )
	{
		PLogError( "_CommExtThreadComm[ %d ] last hope catch: %s",  mgrIdx, err.why() );
		exit();
	}
	catch(  exception& exc  )
	{
		PLogError( "_CommExtThreadComm[ %d ] last hope STD exception catch: %s", mgrIdx,  exc.what() );
		exit();
	}
	catch( ... )
	{
		PLogError( "_CommExtThreadComm[ %d ] last hope ... catch",  mgrIdx );
		exit();
	}
}








#endif
