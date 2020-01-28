#ifndef _commtmrw_h_included
#define _commtmrw_h_included


#include "ppinclude.h"
#include "ppstring.h"
#include "pplogfile.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "_commcomm.h"
#include "commtmrw.h"
#include "commstreamencryption.h"

#ifdef PROTOCOLLOGGING
#define _log2  data.comm._log
#else
#define _log2( ... ) 
#endif



#define READ_BUFFER_SIZE 4096


template< class _MultiSocket >
struct _CommRWCommunicator
{
private:
	_MultiSocket* rw;
	int slot;
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
	void construct( _MultiSocket& rw_, int slot_, CommRoutingTable& routingTable_ )
	{
		rw = &rw_;
		slot = slot_;
		routingTable = &routingTable_;
		physConn = 0;
	}

	void init( _CommPhysicalConnection* physConn_ )
	{
		physConn = physConn_;
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

	void msg( int mux, UINT32 muxChannelId, _CommMsg& msg )
	{
		routingTable->routeIncomingMessage( physConn, mux, muxChannelId, msg );
	}

	void forceSlotFailure( const char* errMsg, int errCode = 0)
	{
		rw->forceSlotFailure( slot, errMsg, errCode );
	}

	void serviceCall(UINT32 id, CommMsgBody * msg_body)
	{
		routingTable->serviceCall(physConn,id,msg_body);
	}
};

//****************************************************************************
class _CommRWThreadBase : public _CommManageableThread
{
protected:
	CommRWThreadManagerBase& mgr;
	int mgrIdx;
	bool muxEnabled;

public:
	_CommRWThreadBase( CommRWThreadManagerBase& mgr_, int mgrIdx_, bool enableMux );
	void pushSocket( const CommRWThreadManagerBase::Socket& sock );
	void pushForceDisconnect( int slot, UINT32 channelId );
	virtual int maxRWSockets() const = 0;

protected:
	bool _freeSocket( int idx )//to solve access problem for _RWThread
	{
		return mgr._freeSocket( idx );
	}

};

template<class _MultiSocket >
class _CommRWThreadBaseRW : public _CommRWThreadBase
{
public:
	enum { _KeepAlivePoll = 1000 };
	
protected:
	bool   *slotActive;
	UINT32 *commQueueSize;
	UINT32 *maxCommQueueSize;
	UINT32 *commTotalTime;
	UINT32 *commTimeInQueue;
	UINT32 *maxCommTimeInQueue;

	UINT32 timeInQueue;
	UINT32 totalTimeInQueue;
	UINT32 maxTimeInQueue;
	UINT32 queueSize;
	UINT32 totalQueueSize;
	UINT32 maxQueueSize;
	UINT32 timeInWait;
	UINT32 measures;
	UINT32 totalMeasures;
	UINT32 totalTime;
	UINT32 readTime;
	UINT32 writeTime;
	bool statInit;
	int numberOfConnections;

public:
	_MultiSocket rw;

	_CommRWThreadBaseRW( CommRWThreadManagerBase& mgr_, int mgrIdx_, bool enableMux ):_CommRWThreadBase(mgr_,mgrIdx_,enableMux),
		numberOfConnections(mgr_.getMaxNumberOfConnectionsPerThread() ? mgr_.getMaxNumberOfConnectionsPerThread() : (_MultiSocket::MaxSockets - 1)),
		rw(q.getSocket(),numberOfConnections)
	{
		slotActive = new bool [numberOfConnections];
		commQueueSize = new UINT32[numberOfConnections];
		maxCommQueueSize = new UINT32[numberOfConnections];
		commTotalTime = new UINT32[numberOfConnections];
		commTimeInQueue = new UINT32[numberOfConnections];
		maxCommTimeInQueue = new UINT32[numberOfConnections];
		_statInit();
	}
	~_CommRWThreadBaseRW()
	{
		delete [] slotActive;
		delete [] commQueueSize;
		delete [] maxCommQueueSize;
		delete [] commTotalTime;
		delete [] commTimeInQueue;
		delete [] maxCommTimeInQueue;
	}

	int maxRWSockets() const override
	{
		return _MultiSocket::MaxSockets - 1;
	}
	UINT32 printCounters(int n,UINT32 & qSize, UINT32 & qTime, UINT32 & qSlotSize, UINT32 & qSlotTime) override
	{
		UINT32 _max = _CommManageableThread::printCounters(n,qSize,qTime,qSlotSize,qSlotTime);
		if (mgr.isGatherStatistic())
		{
			UINT32 q = queueSize;
			UINT32 mq = maxQueueSize;
			double aq = (totalMeasures ? static_cast<double>(totalQueueSize) / totalMeasures : 0);
			UINT32 m = measures;
			UINT32 t = timeInWait;
			UINT32 tt = totalTime;
			UINT32 r = readTime;
			UINT32 w = writeTime;
			UINT32 tq = timeInQueue;
			UINT32 mtq = maxTimeInQueue;
			double atq = (totalMeasures ? static_cast<double>(totalTimeInQueue) / totalMeasures : 0);

			queueSize = 0;
			maxQueueSize = 0;
			measures = 0;
			timeInWait = 0;
			readTime=0;
			writeTime=0;
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
			totalTime = SYSTEMTICKS();
			if (tt)
				tt = totalTime - tt;

			PLog("QueueSize(max;avg)=%u(%u;%f), time in queue(max;avg)=%u(%u;%f), totalTime=%u, timeInWait=%u, readTime=%u, writeTime=%u", q,mq,aq,tq,mtq,atq,tt,t,r,w);

			qSlotSize = 0;
			qSlotTime = 0;
			if (mgr.isGatherStatistic() > 1)
			{
				for (int i = 0; i < numberOfConnections; i++)
				{
					if (slotActive[i])
					{
						UINT32 q = commQueueSize[i];
						UINT32 mq = maxCommQueueSize[i];
						UINT32 tt = commTotalTime[i];
						UINT32 tq = commTimeInQueue[i];
						UINT32 mtq = maxCommTimeInQueue[i];
						commQueueSize[i]=0;
						maxCommQueueSize[i]=0;
						commTotalTime[i]=0;
						commTimeInQueue[i]=0;
						maxCommTimeInQueue[i]=0;

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

						if (mgr.isGatherStatistic() > 2)
						{
							if (q || mq || tt || tq > 32 || mtq > 32)
								PLog("   slot %d - queue(max) = %u(%u), time in queue(max) = %u(%u), time = %u",i+1,q,mq,tq,mtq,tt);
						}
						else
						{
							if (q || mq || tq > 32 || mtq > 32)
								PLog("   slot %d - queue(max) = %u(%u), time in queue(max) = %u(%u)",i+1,q,mq,tq,mtq);
						}
					}
				}
			}
		}
		return _max;

	}
protected:
	void _statInit()
	{
		queueSize=0;
		maxQueueSize=0;
		measures=0;
		timeInWait=0;
		totalTime=0;
		readTime=0;
		writeTime=0;
		timeInQueue=0;
	    maxTimeInQueue=0;
		totalTimeInQueue = 0;
		totalQueueSize = 0;
		totalMeasures = 0;
		statInit=true;
	}
	void _slotStatInit(int i)
	{
		PASSERT3(i >=0 && i < numberOfConnections);
		commQueueSize[i] = 0;
		maxCommQueueSize[i]=0;
		commTotalTime[i]=0;
		maxCommTimeInQueue[i]=0;
		commTimeInQueue[i]=0;
	}

};

//****************************************************************************

template< class _Comm >
struct _CommRWData
{
	CommRWThreadManagerBase::Socket sock;
	UINT32 channelId;

	_Comm comm;
	UINT32 timeConnect;
	bool commInit;
	bool commConn;
};

template< class _Comm, class _MultiSocket >
class _CommRWThread : public _CommRWThreadBaseRW<_MultiSocket>
{

	typedef _CommRWThreadBaseRW<_MultiSocket> Super;

public:
	using Super::rw;

protected:


	using Super::q;
	using Super::mgr;
	using Super::mgrIdx;
	using Super::statInit;
	using Super::_statInit;
	using Super::_slotStatInit;
	using Super::slotActive;
	using Super::muxEnabled;
	using Super::_KeepAlivePoll;
	using Super::_freeSocket;
	using Super::queueSize;
	using Super::maxQueueSize;
	using Super::timeInQueue;
	using Super::maxTimeInQueue;
	using Super::commQueueSize;
	using Super::maxCommQueueSize;
	using Super::commTimeInQueue;
	using Super::maxCommTimeInQueue;
	using Super::commTotalTime;
	using Super::timeInWait;
	using Super::writeTime;
	using Super::readTime;
	using Super::measures;
	using Super::totalTimeInQueue;
	using Super::totalQueueSize;
	using Super::totalMeasures;
	using Super::rundump;
	using Super::exit;
	using Super::numberOfConnections;

public:
	_CommRWThread( CommRWThreadManagerBase& mgr_, int mgrIdx_, bool enableMux )
		: _CommRWThreadBaseRW<_MultiSocket>( mgr_, mgrIdx_, enableMux )
	{
	}

public:

	void run() override;

private:
	void _physConn( int slot, _CommRWData< _Comm >& data );
};

template< class _Comm, class _MultiSocket >
class _CommRWSSLThread : public _CommRWThread<_Comm,_MultiSocket>
{
public:
	_CommRWSSLThread( CommRWThreadManagerBase& mgr_, int mgrIdx_, bool enableMux )
		: _CommRWThread<_Comm,_MultiSocket>( mgr_, mgrIdx_, enableMux )
	{
	}

public:

	void run() override
	{
		_CommRWThread<_Comm,_MultiSocket>::run();
		PTRACE4("_CommRWSSLThread[ %d ]: threadCleanup() called",this->mgrIdx);
		CommSSL::threadCleanup();
	};

private:
	void _physConn( int slot, _CommRWData< _Comm >& data );
};

template< class _Comm, class _MultiSocket >
void _CommRWThread< _Comm,_MultiSocket >::_physConn( int slot, _CommRWData< _Comm >& data )
{
	int isLocal = 1;
	if (PBaseSocket::isLocal(data.sock.sockDescr))
		isLocal = 2;
	else if( this->mgr.isFar )
		isLocal = 0;
	this->mgr.routingTable.physicallyConnected( data.sock.physConn, this->mgrIdx, slot, data.channelId, isLocal );
}

template< class _Comm, class _MultiSocket >
/* virtual */ void _CommRWThread< _Comm,_MultiSocket >::run()
{
	_CommRWData< _Comm > *rwData = 0;
	try
	{
		UINT32 _queueSize  = 0;
		UINT32 _timeInQueue = 0;
		rwData = new _CommRWData< _Comm >[ numberOfConnections ];

		_statInit();

		for( int i=0; i < numberOfConnections ; ++i )
		{
			_slotStatInit( i );
			slotActive[ i ]=false;
			rwData[ i ].comm.construct( rw, i + 1, mgr, mgr.comm_logging );
			if( muxEnabled )
				rwData[ i ].comm.enableMux();
			rwData[ i ].commInit = false;
#ifndef COMM_CLIENT_ONLY
			rwData[ i ].comm.setMinimumProtocol(mgr.minimumProtocol);
			rwData[ i ].comm.setParametersEnforced(mgr.minEnforcedBlock,mgr.maxEnforcedBlock,mgr.randomProtectionEnforced);
#endif
			rwData[ i ].comm.setDefaultProtocol(mgr.DefaultProtocol);
			rwData[ i ].comm.setKeepAlive( _KeepAlivePoll, mgr.keepAliveWrite, mgr.keepAliveRead, mgr.keepAliveIgnoreDataPackets );
			rwData[ i ].comm.setDefaultFrameSize(mgr.MaxBlockSize);
			rwData[ i ].comm.setDefaultLargeFrameSize(mgr.MaxBlockSizeVer5);
			rwData[ i ].comm.setDefaultCompressSize(mgr.CompressBitsSize);
#ifndef COMM_CLIENT_ONLY
			rwData[ i ].comm.setMaximumMessageSize(mgr.maximumMessageSize,mgr.maxMessageSizeDebug);
			rwData[ i ].comm.setMaximumMessageRate(mgr.maximumMessageRate,mgr.maximumMessageRatePeriod,mgr.maxMessageRateDebug);

			if (mgr.enableServerEncryption)
			{
				rwData[ i ].comm.setServerEncryption(mgr.publicKey,mgr.privateKey);
			}
#endif
			rwData[ i ].comm.setClientEncryption(mgr.enableClientEncryption);
		}
		UINT32 lastKeepAlive = SYSTEMTICKS();
		int w = -1;
		for(;;)
		{
			int slot = 0;
			UINT32 now = SYSTEMTICKS();
			UINT32 diff = now - lastKeepAlive;
			if( diff >= _KeepAlivePoll * TICKS_PER_SECOND / 1000 )
			{
				if (mgr.isGatherStatistic())
				{
#if defined (LINUX)
					PThread::setTimers();
#endif
					statInit = false;
					PInterlockedIncrement(&measures);
					PInterlockedIncrement(&totalMeasures);
					PInterlockedExchangeAdd(&queueSize, _queueSize);
					PInterlockedExchangeAdd(&totalQueueSize, _queueSize);
					PInterlockedExchangeAdd(&timeInQueue,_timeInQueue);
					PInterlockedExchangeAdd(&totalTimeInQueue,_timeInQueue);
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

				for( int i=0; i < rw.maxActiveSocket() - 1; ++i )
				{
					if( rwData[ i ].commInit)
					{
						_CommRWData< _Comm >& data = rwData[ i ];
						if(data.commConn)
						{
							try
							{
								CommMsgBody msg;
								msg.composeBYTE(mgr.isGatherStatistic()>1 ? 1:0);
								bool Ok = data.comm.serviceCallTransport( _SERVICE_REQUEST_KEEP_ALIVE,&msg);
								if( !Ok )
									data.comm.forceSlotFailure( "keepAlive timeout expired", _SOCKET_TIMEOUT_EXPIRED );
								if (mgr.isGatherStatistic()>1)
								{
									CommMsgBody msg;
									data.comm.serviceCallTransport( _SERVICE_REQUEST_STATISTIC,&msg);
									CommMsgParser parser( msg );
									UINT32 tmp;
									parser.parseUINT32(tmp);
									PInterlockedExchangeAdd(&commQueueSize[i], tmp);
									if ( maxCommQueueSize[i] < tmp)
									{
										maxCommQueueSize[i] = tmp;
									}
									if (!parser.parseEnded())
									{
										parser.parseUINT32(tmp);
										PInterlockedExchangeAdd(&commTimeInQueue[i],tmp);
										if ( maxCommTimeInQueue[i] < tmp)
										{
											maxCommTimeInQueue[i] = tmp;
										}
									}
								}
							}
							catch( PError& err )
							{
								PLog( "CommRW%sThread[ %d ] exception/1 in slot #%d: %s", _Comm::stringId(), mgrIdx, i + 1, err.why() );
								data.comm.forceSlotFailure( err.why(),_SOCKET_EXCEPTION );
							}
						}
						else
						{
							try
							{
								if ( mgr.handshakeTimeout>0 && (now -  data.timeConnect) > mgr.handshakeTimeout)
								{
									data.comm.forceSlotFailure( "SSL/TCP0 handshake timeout has expired", _SOCKET_HANDSHAKE_TIMEOUT_EXPIRED);
								}
							}
							catch( PError& err )
							{
								PLog( "CommRW%sThread[ %d ] exception/1 in slot #%d: %s", _Comm::stringId(), mgrIdx, i + 1, err.why() );
								data.comm.forceSlotFailure( err.why(),_SOCKET_EXCEPTION );
							}
						}

					}
				}
				lastKeepAlive = now;
				diff = 0;
			}


			//PTRACE( "_DBG: RW:wait( %d ), diff = %d", wait, diff );

			UINT32 qSize;
			if (w == _MultiSocket::waitQueue)
				qSize = static_cast< UINT32 >( q.sizeAndReset() );
			else
				qSize = static_cast< UINT32 >( q.size() );
			if (qSize > 0 )
			{
				w = _MultiSocket::waitQueue;
				if (mgr.isGatherStatistic())
				{
					_queueSize = _queueSize > qSize ? _queueSize:qSize;
				}
			}
			else
			{
				UINT32 wait = diff < _KeepAlivePoll ? _KeepAlivePoll - diff : 0;
				if (mgr.isGatherStatistic())
				{
					UINT32 tmp = SYSTEMTICKS();
					w = rw.wait( slot, wait );
					timeInWait += (SYSTEMTICKS() - tmp); 
				}
				else
				{
					w = rw.wait( slot, wait );
				}
				if (w == _MultiSocket::waitQueue)
					q.increaseAutoResetCounter();
			}
			//PTRACE3( "_DBG: RW:wait(): ret=%d qsizeOld=%d, qSizeNew=%d", w, qSize,q.size() );
			switch( w )
			{
			case _MultiSocket::waitQueue:
				{
					if( q.shuttingDown() )
					{
						PTRACE3( "CommRW%sThread[ %d ] shutdown - exiting", _Comm::stringId(), mgrIdx );
						rw.shutdownSockets();
						delete [] rwData;
						exit();
						return;
					}

					UINT32 t0 = 0;
					if (mgr.isGatherStatistic())
						t0 = SYSTEMTICKS();
					do
					{
						//PTRACE3( "_DBG: RW:waitQueue qSize=%d", q.size() );
						_CommMsg msg;
						UINT32 _timeInQueueTmp;
						if( !q.pop( msg,_timeInQueueTmp ) )
						{
							if (!mgr.optimizePacketSize)
							{
								PTRACE5( "CommRW%sThread[ %d ]: empty queue awaken - ignored", _Comm::stringId(), mgrIdx );
								//							PThread::sleep(0);
							}
                            else
							{
								for( int i=0; i <  rw.maxActiveSocket() - 1; ++i )
									if( rwData[ i ].commInit)
									{
										_CommRWData< _Comm >& data = rwData[ i ];
										if(data.commConn)
										{
											try
											{
												data.comm.flushStoredMsgs();
											}
											catch( PError& err )
											{
												PLog( "CommRW%sThread[ %d ] exception/1 in slot #%d: %s", _Comm::stringId(), mgrIdx, i + 1, err.why() );
												data.comm.forceSlotFailure( err.why(), _SOCKET_EXCEPTION );
											}
										}
									}
							}
							break;//do-while
						}
						_timeInQueue = _timeInQueue > _timeInQueueTmp ? _timeInQueue:_timeInQueueTmp;

						if( msg.type == _COMM_MSGTYPE_SERVICEREQUEST)
						{
							CommMsgParser parser( msg.body );
							UINT32 id;
							parser.parseUINT32(id);
							if (id == _SERVICE_REQUEST_DUMP)
							{
								rundump();
								if (mgr.optimizePacketSize)
									continue; //do-while
								else
									break;
							}

							int slot = msg.internal.slot;
							PASSERT3( slot <= numberOfConnections );
							if( msg.internal.channelId != rwData[ slot - 1 ].channelId )
							{// Critical check. Protects from disconnecting the wrong channel
								PLog( "CommRW%sThread[ %d ]: forceDisconnect obsolete channelId=%08X - ignored", _Comm::stringId(), mgrIdx, msg.internal.channelId );
								if (mgr.optimizePacketSize)
									continue; //do-while
								else
									break;

							}

							_CommRWData< _Comm >& data = rwData[ slot - 1 ];
							switch(id)
							{
							case _SERVICE_REQUEST_FORCE_KEEP_ALIVE:
								try 
								{
									bool Ok =data.comm.serviceCallTransport( _SERVICE_REQUEST_FORCE_KEEP_ALIVE,0);
									if( !Ok )
										data.comm.forceSlotFailure( "keepAlive timeout expired",_SOCKET_TIMEOUT_EXPIRED );
								}
								catch( PError& err )
								{
									PLog( "CommRW%sThread[ %d ] exception _SERVICE_REQUEST_FORCE_KEEP_ALIVE in slot #%d: %s", _Comm::stringId(), mgrIdx, slot, err.why() );
									data.comm.forceSlotFailure( err.why(),_SOCKET_EXCEPTION );
								}
								break;
							case _SERVICE_REQUEST_QUALITY:
								try
								{
									data.comm.serviceCallTransport( _SERVICE_REQUEST_QUALITY,0);
								}
								catch( PError& err )
								{
									PLog( "CommRW%sThread[ %d ] exception _SERVICE_REQUEST_QUALITY in slot #%d: %s", _Comm::stringId(), mgrIdx, slot, err.why() );
									data.comm.forceSlotFailure( err.why(),_SOCKET_EXCEPTION );
								}
								break;
							default:
								PTRACE3("_CommRWThread : Unknown service request ID %X",id);
								break;
							}
						}
						else if( msg.type == _COMM_MSGTYPE_NONE )//'special message' flag
						{
							if( msg.internal.slot == 0xFFFF )//pushSocket
							{
								CommMsgParser parser( msg.body );
								const BYTE* sock_;
								parser._parseFixedBlock( sock_, sizeof(CommRWThreadManagerBase::Socket) );
								CommRWThreadManagerBase::Socket* sock = (CommRWThreadManagerBase::Socket*)sock_;
								int slot = rw.addSocket( sock->sockDescr.sock );

								PTRACE3( "CommRW%sThread[ %d ]: Socket placed to slot #%d", _Comm::stringId(), mgrIdx, slot );

								_CommRWData< _Comm >& data = rwData[ slot - 1 ];
								try
								{
									PString addr;
									PBaseSocket::address( addr, sock->sockDescr );

									_log2("before Init - slot=%d",slot);
									data.channelId = mgr.nextChannelId();
									data.sock = *sock;

									data.comm.init( data.sock.physConn );
									data.commInit = true;
									data.timeConnect = SYSTEMTICKS();
									CommMsgBody msg_body;
									msg_body.composeINT32(slot);
									msg_body.composeString(addr);
									data.comm.serviceCallTransport( _SERVICE_REQUEST_SETIPADDRESS,&msg_body);

									if( data.comm.isConnected() )
									{
										_slotStatInit(slot - 1);
										_physConn( slot, data );
										_log2("after _physConn - slot=%d",slot);
										data.commConn = true;
										slotActive[slot - 1]=true;
										data.comm.setFirstRead();
									}
									else
									{
										data.commConn = false;
										slotActive[slot - 1]=false;
									}

								}
								catch( PError& err )
								{
									PLog( "CommRW%sThread[ %d ] exception/2 in slot #%d: %s", _Comm::stringId(), mgrIdx, slot, err.why() );
									data.comm.forceSlotFailure( err.why(),_SOCKET_EXCEPTION );
								}
							}
							else//forceDisconnect
							{
								int slot = msg.internal.slot;

								PASSERT3( slot <= numberOfConnections );
								if( msg.internal.channelId != rwData[ slot - 1 ].channelId )
								{// Critical check. Protects from disconnecting the wrong channel
									PLog( "CommRW%sThread[ %d ]: forceDisconnect obsolete channelId=%08X - ignored", _Comm::stringId(), mgrIdx, msg.internal.channelId );
									if (mgr.optimizePacketSize)
										continue; //do-while
									else
										break;
								}

								_CommRWData< _Comm >& data = rwData[ slot - 1 ];
								data.comm.forceSlotFailure( "forceDisconnect received" );
							}
						}
						else
						{
							int slot = msg.internal.slot;
							UINT32 t0PerSlot = 0;
							if (mgr.isGatherStatistic()>2)
								t0PerSlot = SYSTEMTICKS();

							PASSERT3( slot <= numberOfConnections );
							if( msg.internal.channelId != rwData[ slot - 1 ].channelId )
							{// Critical check. Protects from dispatching the message to the wrong channel
								// when connection was deleted and then another connection created in the same slot.
								// Also protects from using failed connection: in case of failure 
								// rwData[ slot ].channelId must be changed
								PLog( "CommRW%sThread[ %d ]: message to obsolete channelId=%08X - ignored", _Comm::stringId(), mgrIdx, msg.internal.channelId );
								if (mgr.optimizePacketSize)
									continue; //do-while
								else
									break;

							}

							_CommRWData< _Comm >& data = rwData[ slot - 1 ];
							try
							{
								if (mgr.optimizePacketSize)
								{
									data.comm.storeMsg( msg.internal.mux, msg.internal.muxChannelId, msg );
								}
								else
								{
									data.comm.writeMsg( msg.internal.mux, msg.internal.muxChannelId, msg );
								}
							}
							catch( PError& err )
							{
								PLog( "CommRW%sThread[ %d ] exception/3 in slot #%d: %s", _Comm::stringId(), mgrIdx, slot, err.why() );
								data.comm.forceSlotFailure( err.why(),_SOCKET_EXCEPTION );
							}
							if (mgr.isGatherStatistic()>2)
								commTotalTime[slot] += SYSTEMTICKS() - t0PerSlot;

						}
					}while (mgr.optimizePacketSize);

					if (mgr.isGatherStatistic())
						writeTime += SYSTEMTICKS()-t0;
				}
				break;

			case _MultiSocket::waitRead:
				{
					//PTRACE( "_DBG: RW:waitRead" );
					UINT32 t0 = 0;
					if (mgr.isGatherStatistic())
						t0 = SYSTEMTICKS();


					_CommRWData< _Comm >& data = rwData[ slot  - 1];

					try
					{
						_log2("wait read - slot=%d",slot);
						data.comm.asyncReadOk();
						_log2("after asyncReadOk - slot=%d",slot);
						if( !data.commConn && data.comm.isConnected() )
						{
							_slotStatInit(slot - 1);
							_log2("wait read - not connected  slot=%d",slot);
							_physConn( slot, data );
							_log2("wait read - after _physConn slot=%d",slot);
							data.commConn = true;
							slotActive[slot - 1]=true;
							data.comm.setFirstRead();
						}
					}
					catch( PError& err )
					{
						PLog( "CommRW%sThread[ %d ] exception/rd in slot #%d: %s", _Comm::stringId(), mgrIdx, slot, err.why() );
						data.comm.forceSlotFailure( err.why(),_SOCKET_EXCEPTION );
					}
					if (mgr.isGatherStatistic())
					{
						UINT32 tmp = SYSTEMTICKS() - t0;
						readTime += tmp;
						if (mgr.isGatherStatistic()>2)
							commTotalTime[slot - 1] += tmp;
					}

				}
				break;

			case _MultiSocket::waitWrite:
				{
					//PTRACE( "_DBG: RW:waitWrite" );
					UINT32 t0 = 0;
					if (mgr.isGatherStatistic())
						t0 = SYSTEMTICKS();

					_CommRWData< _Comm >& data = rwData[ slot  - 1];
					try
					{
						_log2("wait write - slot=%d",slot);
						data.comm.asyncWriteOk();
						_log2("wait write - after async  slot=%d",slot);
						if( !data.commConn && data.comm.isConnected() )
						{
							_slotStatInit(slot - 1);
							_log2("wait write - not connected  slot=%d",slot);
							_physConn( slot, data );
							_log2("wait write - after _physConn slot=%d",slot);
							data.commConn = true;
							slotActive[slot - 1]=true;
							data.comm.setFirstRead();
						}
					}
					catch( PError& err )
					{
						PLog( "CommRW%sThread[ %d ] exception/wr in slot #%d: %s", _Comm::stringId(), mgrIdx, slot, err.why() );
						data.comm.forceSlotFailure( err.why(),_SOCKET_EXCEPTION );
					}

					if (mgr.isGatherStatistic())
					{
						UINT32 tmp = SYSTEMTICKS() - t0;
						writeTime += tmp;
						if (mgr.isGatherStatistic()>2)
							commTotalTime[slot - 1] += tmp;
					}


				}
				break;

			case _MultiSocket::waitTimeout:

				break;

			case _MultiSocket::waitFailed:
				{
					_CommRWData< _Comm >& data = rwData[ slot - 1 ];

					int errCode = rw.failedErrCode( slot );
					PLog( "CommRW%sThread[ %d ]: slot #%d closed: error %x - %s: %s", _Comm::stringId(), mgrIdx, slot, errCode, rw.failedMsg( slot ), rw.failedErrMsg( slot ) );
					data.comm._log("Closed: error %x -  %s: %s", errCode, rw.failedMsg( slot ), rw.failedErrMsg( slot ));

					rw.removeSocket( slot );

					_log2("wait failed - slot=%d",slot);
					mgr.routingTable.physicallyDisconnected( data.sock.physConn, errCode );
					data.sock.physConn = 0;
					data.comm.deinit();
					data.commInit = false;
					slotActive[slot - 1]=false;
					data.channelId = mgr.nextChannelId();//to enable protection from using failed connections - see comment above

					int idx = mgrIdx;
					if( _freeSocket( idx ) )
					{//no thread operations allowed after freeSocket returned true
						PTRACE3( "Exiting CommRW%sThread[ %d ]", _Comm::stringId(), idx );
						//exit(); - neither needed nor allowed as this is already deleted
						delete[] rwData;
						return;
					}
				}
				break;
			}
		}
	}
	catch( PFatalError& err )
	{
		PLogError( "CommRW%sThread[ %d ] last hope catch: %s", _Comm::stringId(), mgrIdx, err.why() );

		if (mgr.isExitOnFatalError())
		{
			delete[] rwData;
			FATALEXIT();
		}
		else
		{
			exit(true);
		}
	}
	catch( PError& err )
	{
		PLogError( "CommRW%sThread[ %d ] last hope catch: %s", _Comm::stringId(), mgrIdx, err.why() );
		exit(true);
	}
	catch(  exception& exc  )
	{
		PLogError( "CommRW%sThread[ %d ] last hope STD exception catch: %s", _Comm::stringId(), mgrIdx,  exc.what() );
		exit(true);
	}
	catch( ... )
	{
		PLogError( "CommRW%sThread[ %d ] last hope ... catch", _Comm::stringId(), mgrIdx );
		exit(true);
	}
	delete[] rwData;

}




#endif
