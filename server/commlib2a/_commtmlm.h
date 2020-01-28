#ifndef _commtmlm_h_included
#define _commtmlm_h_included


#include "ppinclude.h"
#include "ppstring.h"
#include "pplogfile.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "_commcomm.h"
#include "commtmlm.h"
#define MAX_SLOTS 31
#define POLL_PROCESS_DIED 1000


struct _CommLMData
{
	CommLMThreadManager::CommunicationPoint point;
	UINT32 channelId;
	CommSharedMemoryLoop comm;
	bool commInit;
};


class _CommLMThreadBase : public _CommManageableThread
{

protected:
	CommLMThreadManager& mgr;
	int mgrIdx;
	bool muxEnable;
	bool   slotActive[MAX_SLOTS];
	UINT32 commQueueSize[MAX_SLOTS];
	UINT32 maxCommQueueSize[MAX_SLOTS];
	UINT32 commTotalTime[MAX_SLOTS];

	UINT32 commTimeInQueue[PMultiSocketRW::MaxSockets];
	UINT32 maxCommTimeInQueue[PMultiSocketRW::MaxSockets];
	UINT32 timeInQueue;
	UINT32 maxTimeInQueue;

	UINT32 queueSize;
	UINT32 maxQueueSize;
	UINT32 timeInWait;
	UINT32 measures;
	UINT32 totalTime;
	UINT32 readTime;
	UINT32 writeTime;
	bool statInit;
	int maxActiveSlots;

public:
	_CommLMThreadBase( CommLMThreadManager& mgr_, int mgrIdx_, bool muxEnable_ );
	void pushSocket( const CommLMThreadManager::CommunicationPoint& point );
	void pushForceDisconnect( int slot, UINT32 channelId );

	int maxLMPoints() const
	{
		return MAX_SLOTS;
	}
	UINT32 printCounters(int n,UINT32 & qSize, UINT32 & qTime, UINT32 & qSlotSize, UINT32 & qSlotTime) override
	{
		UINT32 _max = _CommManageableThread::printCounters(n,qSize,qTime,qSlotSize,qSlotTime);
		if (mgr.isGatherStatistic())
		{
			UINT32 q = queueSize;
			UINT32 mq = maxQueueSize;
			UINT32 m = measures;
			UINT32 t = timeInWait;
			UINT32 tt = totalTime;
			UINT32 r = readTime;
			UINT32 w = writeTime;
			UINT32 tq = timeInQueue;
			UINT32 mtq = maxTimeInQueue;
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

			PLog("QueueSize(max)=%u(%u), time in queue(max)=%u(%u), totalTime=%u, timeInWait=%u, readTime=%u, writeTime=%u",q,mq,tq,mtq,tt,t,r,w);

			qSlotSize = 0;
			qSlotTime = 0;

			if (mgr.isGatherStatistic() > 1)
			{
				for (int i = 0;i<MAX_SLOTS;i++)
				{
					if (slotActive[i])
					{
						UINT32 q = commQueueSize[i];
						UINT32 mq = maxCommQueueSize[i];
						UINT32 tt = commTotalTime[i];
						UINT32 tq = commTimeInQueue[i];
						UINT32 mtq = maxCommTimeInQueue[i];
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
								PLog("   slot %d - queue(max) = %u(%u), time in queue(max) = %u(%u), time = %u",i,q,mq,tq,mtq,tt);
						}
						else
						{
							if (q || mq || tq > 32 || mtq > 32)
								PLog("   slot %d - queue(max) = %u(%u), time in queue(max) = %u(%u)",i,q,mq,tq,mtq);
						}
					}
					commQueueSize[i]=0;
					maxCommQueueSize[i]=0;
					commTotalTime[i]=0;
					commTimeInQueue[i]=0;
					maxCommTimeInQueue[i]=0;
				}
			}
		}
		return _max;
	}

protected:
	bool forceSlotFailure( int slot, _CommLMData& data, const char * why);
	bool _freeSocket( int idx )
	{
		return mgr._freeSocket( idx );
	}
private:
	void _physConn( int slot, _CommLMData& data );
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
		statInit=true;
	}
	void _slotStatInit(int i)
	{
		PASSERT3( i >=0 && i < MAX_SLOTS);
		commQueueSize[i] = 0;
		maxCommQueueSize[i]=0;
		commTotalTime[i]=0;
		maxCommTimeInQueue[i]=0;
		commTimeInQueue[i]=0;
	}


public:
	void run() override;
};


void _CommLMThreadBase::_physConn( int slot, _CommLMData& data )
{
	mgr.routingTable.physicallyConnected( data.point.physConn, mgrIdx, slot, data.channelId, 2 );
}

bool _CommLMThreadBase::forceSlotFailure( int slot, _CommLMData& data, const char * why)
{

	slotActive[slot] = false;

	PLog( "CommLMThread[ %d ]: slot #%d closed: %s", mgrIdx, slot, why );

	mgr.routingTable.physicallyDisconnected( data.point.physConn );
	data.point.physConn = 0;
	data.comm.forceDisconnectSharedMemory();
	data.commInit = false;
	data.channelId = mgr.nextChannelId();//to enable protection from using failed connections - see comment above

	while(maxActiveSlots > 0 && slotActive[maxActiveSlots - 1] == false)
		maxActiveSlots--;


	int idx = mgrIdx;
	if( _freeSocket( idx ) )
	{//no thread operations allowed after freeSocket returned true
		PTRACE( "Exiting CommLMThread[ %d ]",  idx );
		//exit(); - neither needed nor allowed as this is already deleted
		return true;

	}
	return false;
}




void _CommLMThreadBase::run()
{
	try
	{
		UINT32 _queueSize  = 0;
		UINT32 _timeInQueue = 0;
		const int maxSockets = MAX_SLOTS*2+1;
		_CommLMData rwData[ MAX_SLOTS ];
		PWaitForObjects multiWait(maxSockets);
		_statInit();
		int i;
		int startSocket = 0;
		for( i=0; i < MAX_SLOTS ; ++i )
		{
			rwData[ i ].commInit = false;
			slotActive[i]=false;
			_slotStatInit(i);
			rwData[ i ].comm.construct(mgr.comm_logging,i);
			if (muxEnable)
				rwData[ i ].comm.enableMux();
		}
		UINT32 lastPoll = SYSTEMTICKS();
		int w = -1;
		for(;;)
		{
			int slot;
			SOCKET hnds[maxSockets];
			int maps[maxSockets];

			UINT32 now = SYSTEMTICKS();
			UINT32 diff = now - lastPoll;
			if( diff >= POLL_PROCESS_DIED * TICKS_PER_SECOND / 1000 )
			{
				if (mgr.isGatherStatistic())
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
					if (mgr.isGatherStatistic() > 1)
					{
						for ( i=0;i<MAX_SLOTS;i++)
						{
							if (rwData[i].comm.init())
							{
								CommMsgBody msg;
								rwData[i].comm.serviceCallTransport( _SERVICE_REQUEST_STATISTIC,&msg);
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
					}
				}
				else
				{
					if (statInit == false)
					{
						_queueSize  = 0 ;
						_statInit();
					}
				}
#ifdef PWIN32
				while(1)
				{
					slot=0;
					for ( i=0;i<maxActiveSlots;i++)
					{
						if (rwData[i].commInit && rwData[i].point.incoming)
						{
							hnds[slot]=(SOCKET) rwData[i].comm.getPeerProcessHandle();
							if (hnds[slot])
							{
								maps[slot]=i;
								slot++;
							}
						}
					}
					if (slot)
					{
						int w = multiWait.waitMultiple(slot,hnds,0);
						if (w != PWaitForObjects::waitTimeout)
						{
							_CommLMData& data = rwData[ maps[w] ];
							if (forceSlotFailure(maps[w],data,  "Peer process died"))
								return;
						}
						else
						{
							break;
						}
					}
					else
						break;
				}
#endif
				lastPoll = now;
				diff = 0;
			}
			int qSize;
			if (w == 0)
				qSize = q.sizeAndReset();
			else
				qSize = q.size();

			if (qSize > 0 )
			{
				w = 0;
				if (mgr.isGatherStatistic())
				{
					_queueSize = _queueSize > qSize ? _queueSize:qSize;
				}
			}
			else
			{
				UINT32 wait = diff < POLL_PROCESS_DIED ? POLL_PROCESS_DIED - diff : 0;
				hnds[0]=q.getSocket();
				startSocket++;
				if (startSocket>=maxActiveSlots)
					startSocket = 0;
				slot=1;
				int j;
				for ( j=0, i=startSocket; j<maxActiveSlots; j++, i++, i=i>=maxActiveSlots?0:i)
				{
					if (rwData[i].comm.init())
					{
						hnds[slot]=rwData[i].comm.getReadHandle();
						maps[slot]=i+1;
						slot++;
						hnds[slot]=rwData[i].comm.getWriteHandle();
						maps[slot]=-(i+1);
						slot++;
					}
				}
				if (mgr.isGatherStatistic())
				{
					UINT32 tmp = SYSTEMTICKS();
					w = multiWait.waitMultiple(slot,hnds,wait);
					timeInWait += (SYSTEMTICKS() - tmp); 
					_queueSize = _queueSize > qSize ? _queueSize:qSize;
				}
				else
				{
					w = multiWait.waitMultiple(slot,hnds,wait);
				}
				if (w == PWaitForObjects::waitTimeout)
					continue;
				else if (w == PWaitForObjects::waitFailed)
					throw PError("Multiple wWait failed");
				else if (w == 0)
					q.increaseAutoResetCounter();

			}

			switch( w )
			{
			case 0:
				if( q.shuttingDown() )
				{
					PTRACE3( "CommLMThread[ %d ] shutdown - exiting", mgrIdx );
					return;
				}
				{
					UINT32 t0 = 0;
					if (mgr.isGatherStatistic())
						t0 = SYSTEMTICKS();

					_CommMsg msg;
					UINT32 _timeInQueueTmp;
					if( !q.pop( msg,_timeInQueueTmp ) )
					{
						PTRACE5( "CommLMThread[ %d ]: empty queue awaken - ignored", mgrIdx );
						break;//switch
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
							break;
						}
						int slot = msg.internal.slot;
						if( msg.internal.channelId != rwData[ slot ].channelId )
						{// Critical check. Protects from disconnecting the wrong channel
							PLog( "CommLMThread[ %d ]: _COMM_MSGTYPE_SERVICEREQUEST obsolete channelId=%08X - ignored", mgrIdx, msg.internal.channelId );
							break;
						}
						if ( slot >= maxActiveSlots )
						{
							PLog( "CommLMThread[ %d ]: _COMM_MSGTYPE_SERVICEREQUEST slot=%d greater than maxActiveSlots=%d - ignored", mgrIdx, slot,maxActiveSlots );
							break;
						}

						switch(id)
						{
						case _SERVICE_REQUEST_FORCE_KEEP_ALIVE:
							break;
						case _SERVICE_REQUEST_QUALITY:
							break;
						default:
							PTRACE3("_CommLMThreadBase : Unknown service request ID %X",id);
							break;
						}
					}
					else if( msg.type == _COMM_MSGTYPE_NONE )//'special message' flag
					{
						if( msg.internal.slot == 0xFFFF )//pushSocket
						{
							CommMsgParser parser( msg.body );
							const BYTE* point_;
							parser._parseFixedBlock( point_, sizeof(CommLMThreadManager::CommunicationPoint) );
							CommLMThreadManager::CommunicationPoint* point = (CommLMThreadManager::CommunicationPoint*)point_;
							int slot;
							for (slot=0;slot<MAX_SLOTS;slot++)
							{
								if (slotActive[slot] == false)
								{
									break;
								}
							}

							PASSERT(slot<MAX_SLOTS);
							PTRACE3( "CommLMThread[ %d ]: Socket placed to slot #%d", mgrIdx, slot );
							if (slot>=maxActiveSlots)
								maxActiveSlots = slot+1;
							_CommLMData& data = rwData[ slot ];
							try
							{
								data.channelId = mgr.nextChannelId();
								data.point=*point;
								if (point->incoming)
								{
									data.comm.Open(point->sharedResourceName);
								}
								else
								{
									char tmpName[256];
									sprintf(tmpName,"pyr_%8.8lx%8.8lx%4.4x%8.8lx",PID(),THREADID(),slot,SYSTEMTICKS());
									data.comm.Create(tmpName);
									CommSharedMemory connect;
									bool ret = connect.Connect(point->sharedResourceName,tmpName);
									if (ret == false)
									{
										throw PError("Unable to connect");
									}
								}
								_slotStatInit(slot);
								data.commInit = true;
								_physConn( slot, data );
								slotActive[slot]=true;

							}
							catch( PError& err )
							{
								PLog( "CommLMThread[ %d ] exception/2 in slot #%d: %s", mgrIdx, slot, err.why() );
								if (forceSlotFailure(slot,data,  err.why()))
									return;
							}

						}
						else//forceDisconnect
						{
							int slot = msg.internal.slot;

							if( msg.internal.channelId != rwData[ slot ].channelId )
							{// Critical check. Protects from disconnecting the wrong channel
								PLog( "CommLMThread[ %d ]: forceDisconnect obsolete channelId=%08X - ignored", mgrIdx, msg.internal.channelId );
								break;
							}
							if ( slot >= maxActiveSlots )
							{
								PLog( "CommLMThread[ %d ]: forceDisconnect slot=%d greater than maxActiveSlots=%d - ignored", mgrIdx, slot,maxActiveSlots );
								break;
							}


							_CommLMData& data = rwData[ slot ];
							if (forceSlotFailure(slot,data,  "Disconnect received"))
								return;

						}
					}
					else
					{
						int slot = msg.internal.slot;
						UINT32 t0PerSlot = 0;
						if (mgr.isGatherStatistic()>2)
							t0PerSlot = SYSTEMTICKS();

						if( msg.internal.channelId != rwData[ slot ].channelId )
						{	// Critical check. Protects from dispatching the message to the wrong channel
							// when connection was deleted and then another connection created in the same slot.
							// Also protects from using failed connection: in case of failure 
							// rwData[ slot ].channelId must be changed
							PLog( "CommLMThread[ %d ]: message to obsolete channelId=%08X - ignored", mgrIdx, msg.internal.channelId );
							break;
						}
						if ( slot >= maxActiveSlots )
						{
							PLog( "CommLMThread[ %d ]: message slot=%d greater than maxActiveSlots=%d - ignored", mgrIdx, slot,maxActiveSlots );
							break;
						}

						_CommLMData& data = rwData[ slot ];
						try
						{
							data.comm.pushMessage(msg);
						}
						catch( PError& err )
						{
							PLog( "CommLMThread[ %d ] exception/3 in slot #%d: %s", mgrIdx, slot, err.why() );
							if (forceSlotFailure(slot,data,  err.why()))
								return;
						}
						if (mgr.isGatherStatistic()>2)
							commTotalTime[slot] += SYSTEMTICKS() - t0PerSlot;

					}
					if (mgr.isGatherStatistic())
						writeTime += SYSTEMTICKS()-t0;

				}
				break;

			default:
				slot = maps[w];
				if (slot<0)
				{
					UINT32 t0 = 0;
					if (mgr.isGatherStatistic())
						t0 = SYSTEMTICKS();

					slot=-slot;
					slot--;

					_CommLMData& data = rwData[ slot ];
					data.comm.resetWriteEvent();
					try
					{
						data.comm.peerReadData();
					}
					catch( PError& err )
					{
						PLog( "CommLMThread[ %d ] exception/3 in slot #%d: %s", mgrIdx, slot, err.why() );
						if (forceSlotFailure(slot,data,  err.why()))
							return;
					}
					if (mgr.isGatherStatistic())
					{
						UINT32 tmp = SYSTEMTICKS() - t0;
						writeTime += tmp;
						if (mgr.isGatherStatistic()>2)
							commTotalTime[slot] += tmp;
					}

					break;
				}
				else if (slot > 0)
				{
					UINT32 t0 = 0;
					if (mgr.isGatherStatistic())
						t0 = SYSTEMTICKS();

					slot--;
					_CommLMData& data = rwData[ slot ];
					data.comm.resetReadEvent();
					try
					{
						int code;
						_CommMsg msg;
						while( 0 != (code=data.comm.getMessage(msg)))
						{
							if (code == -1)
							{
								if (forceSlotFailure(slot,data, "Peer close connection"))
									return;
								break;
							}
							else
							{
								mgr.routingTable.routeIncomingMessage( data.point.physConn, msg.internal.mux, msg.internal.muxChannelId, msg );
								msg.body._dealloc();
							}
						}

					}
					catch( PError& err )
					{
						PLog( "CommLMThread[ %d ] exception/3 in slot #%d: %s", mgrIdx, slot, err.why() );
						if (forceSlotFailure(slot,data,  err.why()))
							return;
					}
					if (mgr.isGatherStatistic())
					{
						UINT32 tmp = SYSTEMTICKS() - t0;
						readTime += tmp;
						if (mgr.isGatherStatistic()>2)
							commTotalTime[slot] += tmp;
					}
					break;
				}
				else
					PASSERT3(0);
				break;
			}
		}
	}
	catch( PFatalError& err )
	{
		PLogError( "CommLMThread[ %d ] last hope catch: %s", mgrIdx, err.why() );
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
		PLogError( "CommLMThread[ %d ] last hope catch: %s", mgrIdx, err.why() );
		exit(true);
	}
	catch(  exception& exc  )
	{
		PLogError( "CommLMThread[ %d ] last hope STD exception catch: %s", mgrIdx, exc.what() );
		exit(true);
	}
	catch( ... )
	{
		PLogError( "CommLMThread[ %d ] last hope ... catch", mgrIdx );
		exit(true);
	}

}

#endif
