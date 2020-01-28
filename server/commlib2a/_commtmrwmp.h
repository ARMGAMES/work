#ifndef _commtmrwmp_h_included
#define _commtmrwmp_h_included


#include "ppinclude.h"
#ifdef PWIN32
#include "ppstring.h"
#include "pplogfile.h"
#include "ppprocess.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "commsharedmem.h"
#include "commsharedmemloop.h"

class _CommRWThreadMultiProcess;

class _SlotsForMultiProcess
{

public:
	CommSharedMemoryLoop comm;
	CommRWThreadManagerMultiProcess* mgr;
	_CommRWThreadMultiProcess* thread;

	int mgrIdx;
	PProcess process;

	bool started;
	int  available;
	int  processSlotNum;
	enum {MaxSlot = 31 };
	struct  _Slot
	{
		CommRWThreadManagerMultiProcess::Socket sock;
		UINT32 channelId;
		int remoteSlot;
		bool occupied;
		bool init;
	};
	_Slot slots[MaxSlot];

	void init()
	{
		started = false;
		available = MaxSlot;
		int i;
		for (i=0;i<MaxSlot;i++)
		{
			slots[i].occupied=false;
			slots[i].init=false;
		}
	}
	_SlotsForMultiProcess()
	{
		mgr = 0;
		mgrIdx = -1;
		processSlotNum = -1;
		thread = 0;
		init();
	}
	void construct(CommRWThreadManagerMultiProcess& mgr_, int mgrIdx_, int slotNum_, _CommRWThreadMultiProcess* thread_)
	{
		mgr = &mgr_;
		processSlotNum = slotNum_;
		mgrIdx = mgrIdx_;
		thread = thread_;
		init();
	}

	int addSlot(CommRWThreadManagerProcess::Socket& sock_)
	{
		int i;
		for (i=0; i<MaxSlot;i++)
		{
			if (slots[i].occupied == false)
			{
				slots[i].occupied=true;
				slots[i].init=false;
				slots[i].sock=sock_;
				PASSERT3(available > 0);
				available--;
				return i;
			}
		}
		PASSERT(0);
		return -1;
	}
	void startProcess( )
	{
		PASSERT3(!started); 
		try
		{
			init();
			char sharedName[100];
			sprintf(sharedName,"sslmp%2.2d%8.8lx%8.8lx",processSlotNum,::THREADID(),SYSTEMTICKS());
			comm.Create(sharedName);
			comm.enableMux();
			PString exeFileName;
			const char* sep = strchr(mgr->communicatorProcessName.c_str(), '\\');
			if (!sep)
			{
				sep = strchr(mgr->communicatorProcessName.c_str(), '/');
			}
			if (sep)
			{
				exeFileName.assign(mgr->communicatorProcessName);
			}
			else
			{
				exeFileName.assign(mgr->dirName).append(mgr->communicatorProcessName);
			}
			const char* parms[ 2 ];
			int npar=1;
			parms[ 0 ] = sharedName;
			if (*mgr->logFileDirName.c_str())
			{
				npar=2;
				parms[ 1 ] = mgr->logFileDirName.c_str();
			}
			process.start( exeFileName, npar, parms );
			PTRACE3("CommRWThreadMultiProcess[ %d ][ %d ]: starting process %s",mgrIdx, processSlotNum,sharedName);
			started = true;
		}
		catch( PError& err )
		{
			PLog( "CommRWThreadMultiProcess[ %d ][ %d ] - process initialization failed: %s",mgrIdx, processSlotNum, err.why() );
			PASSERT3(0);
		}
	}
	bool disconnect(int slot, int errCode)
	{
		PASSERT3( slot >= 0 && slot < MaxSlot);
		PASSERT3(available < MaxSlot);
		_Slot &data=slots[slot];
		PASSERT3(data.sock.physConn);
		mgr->routingTable.physicallyDisconnected( data.sock.physConn, errCode );
		if (data.init == false)
		{
			PLog("CommRWThreadMultiProcess[ %d ][ %d ] slot#%d - socket not closed - closesocket()",mgrIdx, processSlotNum, slot); 
			if (::closesocket(data.sock.sockDescr.sock)==SOCKET_ERROR)
			{
				PLog("CommRWThreadMultiProcess[ %d ][ %d ] slot #%d  -PROCESS_DISCONNECT- closesocket() failed= %d",mgrIdx, processSlotNum, slot,::WSAGetLastError());
			}
		}
		data.sock.physConn = 0;
		data.init = false;
		data.occupied = false;
		data.channelId = mgr->nextChannelId();//to enable protection from using failed connections - see comment above

		available++;
		if ( available == MaxSlot ) 
			return true;
		else 
			return false;
	}

	void killProcess()
	{
		PASSERT3(started);
		comm.forceDisconnectSharedMemory();
		process.kill();
		process.closeHandles();
		init();
	}
	bool _clearAllConnections();
};


class _CommRWThreadMultiProcess : public _CommRWThreadBase
{

protected:
	enum {MaxSlot = 21 };
	CommRWThreadManagerMultiProcess& mgr;
	_SlotsForMultiProcess rwdata[MaxSlot];
	int maxActiveSlots;

	UINT32 maxTimeInQueue;
	UINT32 maxQueueSize;
	bool statInit;



public:
	_CommRWThreadMultiProcess( CommRWThreadManagerMultiProcess& mgr_, int mgrIdx_ );

	UINT32 printCounters(int n,UINT32 & qSize, UINT32 & qTime, UINT32 & qSlotSize, UINT32 & qSlotTime) override
	{
		UINT32 _max = _CommManageableThread::printCounters(n,qSize,qTime,qSlotSize,qSlotTime);
		if (mgr.isGatherStatistic())
		{
			UINT32 mq = maxQueueSize;
			UINT32 mtq = maxTimeInQueue;
			maxQueueSize = 0;
			maxTimeInQueue=0;

			qSize = mq;
			qTime = mtq;
			PLog("QueueSize max=%u, time in queue max=%u",mq,mtq);
		}
		return _max;
	}

	void run() override;
	virtual int maxRWSockets() const
	{
		return _SlotsForMultiProcess::MaxSlot * MaxSlot;
	}

protected:
	_SlotsForMultiProcess::_Slot & findSlot(int slot)
	{
		int idProcess = slot / _SlotsForMultiProcess::MaxSlot;
		PASSERT3( idProcess >=0 && idProcess < MaxSlot);
		int idSlot = slot % _SlotsForMultiProcess::MaxSlot;
		PASSERT3( idSlot >= 0 && idSlot < _SlotsForMultiProcess::MaxSlot);
		return rwdata[idProcess].slots[idSlot];
	}
	_SlotsForMultiProcess & findProcess(int slot)
	{
		int idProcess = slot / _SlotsForMultiProcess::MaxSlot;
		PASSERT3( idProcess >=0 && idProcess < MaxSlot);
		return rwdata[idProcess];
	}

	void disconnect(int slot, int errCode)
	{
		int idProcess = slot / _SlotsForMultiProcess::MaxSlot;
		PASSERT3( idProcess >=0 && idProcess < MaxSlot);
		_SlotsForMultiProcess & data = rwdata[idProcess];
		PASSERT3(data.started);
		int idSlot = slot % _SlotsForMultiProcess::MaxSlot;
		if (data.disconnect(idSlot, errCode))
		{
			PTRACE3( "Exiting CommRWThreadMultiProcess[ %d ][ %d ]",  mgrIdx, idProcess );
			data.killProcess();
			while(maxActiveSlots > 0 && !rwdata[maxActiveSlots - 1].started)
			{
				maxActiveSlots--;
			}
		}
	}
	void _clearAllConnections()
	{
		int i;
		for (i=0;i<maxActiveSlots;i++)
		{
			if (rwdata[i].started)
			{
				rwdata[i]._clearAllConnections();
			}
		}
	}


	void _physConn( int slot, int remoteSlot );

	int addSlot(CommRWThreadManagerProcess::Socket& sock_)
	{
		int i;
		for (i=0;i<maxActiveSlots;i++)
		{
			if (rwdata[i].started && rwdata[i].available)
			{
				int slot = rwdata[i].addSlot(sock_);
				return i * _SlotsForMultiProcess::MaxSlot + slot;
			}
		}
		for (i=0;i<MaxSlot;i++)
		{
			if (!rwdata[i].started)
			{
				if (maxActiveSlots < (i + 1) )
					maxActiveSlots = i+1;
				rwdata[i].startProcess();

				_CommMsg m( _COMM_MSGTYPE_SERVICEREQUEST, _COMM_MSGPRIORITIES-1 );
				m.body.composeUINT32(_SERVICE_REQUEST_PROCESS_INIT);
				m.body.composeUINT32(PID())
					.composeUINT32(mgr.keepAliveWrite)
					.composeUINT32(mgr.keepAliveRead)
					.composeUINT32(mgr.MaxBlockSize)
					.composeUINT32(mgr.CompressBitsSize)
					.composeUINT32(mgr.DefaultProtocol)
					.composeString(mgr.commLoggingName)
					._composeVarBlock(mgr.certFileDefault.keyFile.ptr(),mgr.certFileDefault.keyFile.size())
					._composeVarBlock(mgr.certFileDefault.certFile.ptr(),mgr.certFileDefault.certFile.size())
					.composeString(mgr.certFileDefault.password)
					.composeBOOL(mgr.optimizePacketSize)
#ifndef COMM_CLIENT_ONLY
					.composeUINT32(mgr.maximumMessageSize)
					.composeUINT32(mgr.maximumMessageRate)
					.composeUINT32(mgr.maximumMessageRatePeriod)
					.composeBOOL(mgr.maxMessageSizeDebug)
					.composeBOOL(mgr.maxMessageRateDebug)
					.composeBOOL(mgr.enableServerEncryption)
#endif
					.composeBOOL(mgr.enableClientEncryption);
#ifndef COMM_CLIENT_ONLY
				if (mgr.enableServerEncryption)
				{
					m.body.composeUINT32(mgr.publicKey[0])
						.composeUINT32(mgr.publicKey[1])
						.composeUINT32(mgr.publicKey[2])
						.composeUINT32(mgr.publicKey[3])
						.composeUINT32(mgr.publicKey[4])
						.composeUINT32(mgr.privateKey[0])
						.composeUINT32(mgr.privateKey[1])
						.composeUINT32(mgr.privateKey[2])
						.composeUINT32(mgr.privateKey[3])
						.composeUINT32(mgr.privateKey[4]);
				}
				m.body.composeUINT32(mgr.minimumProtocol)
					  .composeUINT32(mgr.minEnforcedBlock)
					  .composeUINT32(mgr.maxEnforcedBlock)
					  .composeUINT32(mgr.randomProtectionEnforced);
				m.body.composeUINT32(mgr.certFiles.size());
				PStringMap<CommRWThreadManagerProcess::CertFiles>::iterator it;
				for (it=mgr.certFiles.begin();it!=mgr.certFiles.end();++it)
				{
					m.body.composeString((*it).first)
					._composeVarBlock((*it).second.keyFile.ptr(),(*it).second.keyFile.size())
					._composeVarBlock((*it).second.certFile.ptr(),(*it).second.certFile.size())
					.composeString((*it).second.password);
				}

#endif
				int slot = rwdata[i].addSlot(sock_);
				rwdata[i].comm.pushMessage(m);
				return i * _SlotsForMultiProcess::MaxSlot + slot;
			}
		}
		PASSERT3(0);
		return -1;
	}

	void rundump()
	{
		PLog("Dump=== size of queue = %d",q.size());
		int i;
		int j;
		for (j=0;j<maxActiveSlots;j++)
		{
			if (rwdata[j].started )
			{
				rwdata[j].comm.dump();
				for (i=0; i<_SlotsForMultiProcess::MaxSlot;i++)
				{
					if (rwdata[j].slots[i].occupied == true)
					{
						PLog("Dump=== --- Slot %d: init:%s, channelId=%x remoteSlot=%d, PhysConn=%p",
							i,
							(rwdata[j].slots[i].init?"true":"false"),
							rwdata[j].slots[i].channelId,
							rwdata[j].slots[i].remoteSlot,
							rwdata[j].slots[i].sock.physConn);
					}
				}
			}
		}

	}
public:
	bool freeSocket(int idx)
	{
		return _freeSocket(idx);
	}
	void _statInit()
	{
		maxQueueSize=0;
	    maxTimeInQueue=0;
		statInit=true;
	}


};


bool _SlotsForMultiProcess::_clearAllConnections()
{
	bool r=false;
	for (int i=0;i<MaxSlot;++i)
	{
		if (slots[i].occupied)
		{
			r = thread->freeSocket(mgrIdx);
			disconnect(i,0);
		}
	}
	if (started)
	{
		killProcess();
	}
	return r;
}


void _CommRWThreadMultiProcess::_physConn( int slot, int remoteSlot_ )
{
	PASSERT(slot>=0 && slot< ( _SlotsForMultiProcess::MaxSlot * MaxSlot));

	int idProcess = slot / _SlotsForMultiProcess::MaxSlot;
	PASSERT3( idProcess >= 0 && idProcess < MaxSlot);
	int idSlot = slot % _SlotsForMultiProcess::MaxSlot;
	PASSERT3( idSlot >= 0 && idSlot < _SlotsForMultiProcess::MaxSlot);

	_SlotsForMultiProcess::_Slot & data = rwdata[idProcess].slots[idSlot];
	PASSERT(data.occupied && !data.init);
	PString addr;

	PIPSocket::address( addr, data.sock.sockDescr );
	int isLocal = 1;
	if( addr.equals( "127.0.0.1" ) )
		isLocal = 2;
	else if( mgr.isFar )
		isLocal = 0;
	if (closesocket(data.sock.sockDescr.sock)== SOCKET_ERROR)
	{
		PLog("CommRWThreadMultiProcess[ %d ] slot=%d - closesocket() failed= %d",mgrIdx,slot,::WSAGetLastError());
	}

	data.remoteSlot = remoteSlot_;
	data.init=true;
	mgr.routingTable.physicallyConnected( data.sock.physConn, mgrIdx, slot, data.channelId, isLocal );
	PLog("CommRWThreadMultiProcess[ %d ] - connected: addr= %s, slot=%d, remote slot=%d", mgrIdx, addr.c_str(), slot, remoteSlot_);
}

void _CommRWThreadMultiProcess::run()
{
	try
	{
		const int maxSocket = MaxSlot * 3 + 1 ;
		PWaitForObjects multiWait(maxSocket);
		int startSocket = 0;
		_statInit();
		for (int i=0; i < MaxSlot; i++)
		{
			rwdata[i].construct(mgr,mgrIdx,i, this);
		}
		int w = -1;
		for(;;)
		{

			int qSize;
			int w0;
			int slotProcess = 0;

			if (w == 0)
				qSize = q.sizeAndReset();
			else
				qSize = q.size();
			if (qSize > 0 )
			{
				w0 = w = 0;
				if (mgr.isGatherStatistic())
				{
					if (maxQueueSize < qSize)
					{
						 maxQueueSize = qSize;
					}
				}
			}
			else
			{
				SOCKET hnds[maxSocket];
				int maps[maxSocket];
				hnds[0]=q.getSocket();
				startSocket++;
				if (startSocket>=maxActiveSlots)
					startSocket = 0;
				int _slot=1;
				int j;
				int i;
				for ( j=0, i=startSocket; j<maxActiveSlots; j++, i++, i=i>=maxActiveSlots?0:i)
				{
					if (rwdata[i].started)
					{
						hnds[_slot]=rwdata[i].comm.getReadHandle();
						maps[_slot]=i;
						_slot++;
						hnds[_slot]=rwdata[i].comm.getWriteHandle();
						maps[_slot]=i;
						_slot++;
						hnds[_slot] = (SOCKET) rwdata[i].process.getHandle();
						maps[_slot]=i;
						_slot++;
					}
				}
				w = multiWait.waitMultiple(_slot,hnds,PWaitForObjects::infinite);
				if (w == PWaitForObjects::waitTimeout || w == PWaitForObjects::waitFailed)
				{
					PTRACE3( "CommRWThreadMultiProcess[ %d ] shutdown - exiting", mgrIdx );
					_clearAllConnections();
					exit();
					return;
				}
				else if (w==0)
					q.increaseAutoResetCounter();
				if ( w == 0 )
				{
					w0 = 0;
				}
				else 
				{
					w0 = (w - 1)%3 + 1;
					slotProcess = maps[w];
				}
			}

			switch( w0 )
			{
			case 1:
				{
					int code;
					_CommMsg msg;
					while( 0 != (code=rwdata[slotProcess].comm.getMessage(msg)))
					{
						if (code==-1)
						{
							PTRACE3( "CommRWThreadMultiProcess[ %d ][ %d ] getMessage -1", mgrIdx, slotProcess );
							if (rwdata[slotProcess]._clearAllConnections())
							{
								PTRACE3( "CommRWThreadMultiProcess[ %d ] shutdown - exiting", mgrIdx );
								exit();
								return;
							}
							else 
								break;
						}
						msg.internal.slot = msg.internal.mux;
						msg.internal.channelId = msg.internal.muxChannelId;
						if( msg.type == _COMM_MSGTYPE_SERVICEREQUEST)
						{
							CommMsgParser parser( msg.body );
							UINT32 id;
							parser.parseUINT32(id);
							if (id == _SERVICE_REQUEST_DUMP)
							{
								rundump();
								continue;
							}
							switch(id)
							{
							case _SERVICE_REQUEST_PROCESS_INIT:
								// Process initialized and ready to serve requests;
								{
									break;
								}
							case _SERVICE_REQUEST_PROCESS_CONNECT:
								{
									int remoteSlot;
									parser.parseINT32(remoteSlot);
									PASSERT(remoteSlot>0 && remoteSlot<PMultiSocketRW::MaxSockets);
									_physConn(msg.internal.slot,remoteSlot);
									break;
								}
							case _SERVICE_REQUEST_PROCESS_CONNECT_FAILED:
								{
									int slot = msg.internal.slot;
									const char * failed;
									const char * errFailed;
									parser.parseString(failed).parseString(errFailed);
									PLog( "CommRWThreadMultiProcess[ %d ]: slot #%d failed to connect: %s: %s", mgrIdx, slot, failed, errFailed);
									disconnect(slot, 0);
									int idx = mgrIdx;
									if( _freeSocket( idx ) )
									{//no thread operations allowed after freeSocket returned true
										PTRACE3( "Exiting CommRWThreadMultiProcess[ %d ]",  idx );
										return;
									}
									break;
								}
							case _SERVICE_REQUEST_PROCESS_DISCONNECT:
								{
									int slot = msg.internal.slot;
									const char * failed;
									const char * errFailed;
									int errCode;
									parser.parseString(failed).parseString(errFailed).parseINT32(errCode);
									PLog( "CommRWThreadMultiProcess[ %d ]: slot #%d closed: error %x - %s: %s", mgrIdx, slot, errCode, failed, errFailed);
									disconnect(slot, errCode);
									int idx = mgrIdx;
									if( _freeSocket( idx ) )
									{//no thread operations allowed after freeSocket returned true
										PTRACE3( "Exiting CommRWThreadMultiProcess[ %d ]",  idx );
										return;
									}
									break;
								}
							case _SERVICE_REQUEST_SERVICE_CALL:
								{
									int slot = msg.internal.slot;
									_SlotsForMultiProcess::_Slot &data=findSlot(slot);
									if (!(data.occupied && data.init))
									{
										PLog( "CommRWThreadMultiProcess[ %d ] slot #%d: service request for closed channelId=%08X - ignored", mgrIdx, slot, msg.internal.channelId );
										break;
									}
									else if( msg.internal.channelId != data.channelId )
									{
										PLog( "CommRWThreadMultiProcess[ %d ] slot #%d: service request for obsolete channelId=%08X - ignored", mgrIdx, slot, msg.internal.channelId );
										break;
									}

									UINT32 internalId;
									BYTE   hasBody;
									CommMsgBody msgBody;
									parser.parseUINT32(internalId).parseBYTE(hasBody);
									if (hasBody)
									{
										parser.parseMsgBody(msgBody);
										mgr.routingTable.serviceCall(data.sock.physConn,internalId,&msgBody);
									}
									else
									{
										mgr.routingTable.serviceCall(data.sock.physConn,internalId,0);
									}
									break;
								}
							default:
								PASSERT(0);
							}
						}
						else
						{
							// real messages 
							int slot = msg.internal.slot;
							_SlotsForMultiProcess::_Slot &data=findSlot(slot);	
							if (!(data.occupied && data.init))
							{
								PLog( "CommRWThreadMultiProcess[ %d ] slot #%d: message for closed channelId=%08X - ignored", mgrIdx, slot, msg.internal.channelId );
							}
							else if( msg.internal.channelId != data.channelId )
							{
								PLog( "CommRWThreadMultiProcess[ %d ] slot #%d: message for obsolete channelId=%08X - ignored", mgrIdx, slot, msg.internal.channelId );
							}
							else
							{
								mgr.routingTable.routeIncomingMessage( data.sock.physConn, 0, 0, msg );
							}
						}
					}
					break;
				}
			case 2:
				rwdata[slotProcess].comm.peerReadData();
				break;

			case 3:
				PTRACE3( "CommRWThreadMultiProcess[ %d ][ %d ] process died", mgrIdx,slotProcess  );
				if (rwdata[slotProcess]._clearAllConnections())
				{
					PTRACE3( "CommRWThreadMultiProcess[ %d ] process died - exiting", mgrIdx );
					exit();
					return;
				}
				break;
			case 0:
				{
					if( q.shuttingDown() )
					{
						PTRACE3( "CommRWThreadMultiProcess[ %d ] shutdown - exiting", mgrIdx );
						_clearAllConnections();
						exit();
						return;
					}
					_CommMsg msg;
					UINT32 _timeInQueueTmp;
					if( !q.pop( msg,_timeInQueueTmp ) )
					{
						PTRACE3( "CommRWThreadMultiProcess[ %d ]: empty queue awaken - ignored", mgrIdx );
						break;//switch
					}
					if (mgr.isGatherStatistic())
					{
						if (maxTimeInQueue < _timeInQueueTmp)
						{
							 maxTimeInQueue = _timeInQueueTmp;
						}
					}
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
						_SlotsForMultiProcess::_Slot &data=findSlot(slot);	
						if( msg.internal.channelId != data.channelId )
						{// Critical check. Protects from disconnecting the wrong channel
							PLog( "CommRWThreadMultiProcess[ %d ]: slot #%d service request for obsolete channelId=%08X - ignored", mgrIdx, slot, msg.internal.channelId );
							break;
						}
						msg.internal.mux=data.remoteSlot;
						msg.internal.muxChannelId=data.channelId;
						findProcess(slot).comm.pushMessage(msg);
					}
					else if( msg.type == _COMM_MSGTYPE_NONE )//'special message' flag
					{
						if( msg.internal.slot == 0xFFFF )//pushSocket
						{
							CommMsgParser parser( msg.body );
							const BYTE* sock_;
							parser._parseFixedBlock( sock_, sizeof(CommRWThreadManagerProcess::Socket) );
							CommRWThreadManagerProcess::Socket* sock = (CommRWThreadManagerProcess::Socket*)sock_;
							int slot = addSlot( *sock );

							PTRACE3( "CommRWThreadMultiProcess[ %d ]: Socket placed to slot #%d", mgrIdx, slot );
							_SlotsForMultiProcess::_Slot &data=findSlot(slot);	
							_CommMsg m( _COMM_MSGTYPE_SERVICEREQUEST, _COMM_MSGPRIORITIES-1 );
							m.body.composeUINT32(_SERVICE_REQUEST_PROCESS_CONNECT);
							m.body.composeINT32(slot);
							CommRWThreadManagerBase::Socket::composeSockDescriptor(sock->sockDescr, m.body);
							//							m.body._composeFixedBlock((const BYTE *)&sock->sockDescr,sizeof(sock->sockDescr));
							data.channelId = mgr.nextChannelId();
							m.internal.mux=0xFFFF;
							m.internal.muxChannelId = data.channelId;
							findProcess(slot).comm.pushMessage(m);
						}
						else//forceDisconnect
						{
							int slot = msg.internal.slot;
							_SlotsForMultiProcess::_Slot &data=findSlot(slot);	
							if( msg.internal.channelId != data.channelId )
							{
								PLog( "CommRWThreadMultiProcess[ %d ]: forceDisconnect obsolete channelId=%08X - ignored", mgrIdx, msg.internal.channelId );
								break;
							}
							_CommMsg m( _COMM_MSGTYPE_SERVICEREQUEST, _COMM_MSGPRIORITIES-1 );
							m.body.composeUINT32(_SERVICE_REQUEST_PROCESS_DISCONNECT);
							m.internal.mux=data.remoteSlot;
							m.internal.muxChannelId = data.channelId;
							findProcess(slot).comm.pushMessage(m);
						}
					}
					else
					{
						int slot = msg.internal.slot;
						_SlotsForMultiProcess::_Slot &data=findSlot(slot);	
						if( msg.internal.channelId != data.channelId )
						{
							PLog( "CommRWThreadMultiProcess[ %d ] slot #%d: message to obsolete channelId=%08X - ignored", mgrIdx, slot, msg.internal.channelId );
							break;
						}
						msg.internal.mux=data.remoteSlot;
						msg.internal.muxChannelId = data.channelId;
						findProcess(slot).comm.pushMessage(msg);

					}
					break;
				}
			}

		}
	}
	catch( PFatalError& err )
	{
		PLogError( "CommRWThreadMultiProcess[ %d ] last hope catch: %s",mgrIdx, err.why() );
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
		PLogError( "CommRWThreadMultiProcess[ %d ] last hope catch: %s",mgrIdx, err.why() );
		_clearAllConnections();
		exit(true);
	}

}
#endif
#endif
