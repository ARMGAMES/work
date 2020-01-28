#ifndef _commtmrwp_h_included
#define _commtmrwp_h_included


#include "ppinclude.h"
#ifdef PWIN32
#include "ppstring.h"
#include "pplogfile.h"
#include "ppprocess.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "commsharedmem.h"
#include "commsharedmemloop.h"



class _SlotsForProcess
{
public:
	enum {MaxSlot = 31 };
	struct  _Slot
	{
		CommRWThreadManagerProcess::Socket sock;
		UINT32 channelId;
		int remoteSlot;
		bool occupied;
		bool init;
	};
	_Slot slots[MaxSlot];
	_SlotsForProcess()
	{
		int i;
		for (i=0;i<MaxSlot;i++)
		{
			slots[i].occupied=false;
			slots[i].init=false;
		}
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
				return i;
			}
		}
		PASSERT(0);
		return -1;
	}

};

class _CommRWThreadProcess : public _CommRWThreadBase
{

	protected:
	CommRWThreadManagerProcess& mgr;
	CommSharedMemoryLoop comm;
	PProcess process;
	_SlotsForProcess rwdata;

	UINT32 maxTimeInQueue;
	UINT32 maxQueueSize;
	bool statInit;

	bool init;


	public:
	_CommRWThreadProcess( CommRWThreadManagerProcess& mgr_, int mgrIdx_ );
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
		return _SlotsForProcess::MaxSlot;
	}

	protected:
    void _clearAllConnections()
	{
		int i;
		for (i=0;i<_SlotsForProcess::MaxSlot;i++)
		{
			if (rwdata.slots[i].occupied)
			{
				_freeSocket(mgrIdx);
				if (rwdata.slots[i].init == false)
				{
					if (closesocket(rwdata.slots[i].sock.sockDescr.sock)==SOCKET_ERROR)
					{
						PLog("CommRWThreadProcess -ClearAll- closesocket() failed= %d",::WSAGetLastError());
					}
				}
				else
				{
					mgr.routingTable.physicallyDisconnected( rwdata.slots[i].sock.physConn );
				}
				rwdata.slots[i].init=false;
				rwdata.slots[i].occupied=false;
				rwdata.slots[i].channelId = mgr.nextChannelId();
			}
		}
	}

	void _physConn( int slot, int remoteSlot );
	void rundump()
	{
		PLog("Dump=== size of queue = %d",q.size());
		comm.dump();
		int i;
		for (i=0; i<_SlotsForProcess::MaxSlot;i++)
		{
			if (rwdata.slots[i].occupied == true)
			{
				PLog("Dump=== --- Slot %d: init:%s, channelId=%x remoteSlot=%d, PhysConn=%p",
					i,
					(rwdata.slots[i].init?"true":"false"),
					rwdata.slots[i].channelId,
					rwdata.slots[i].remoteSlot,
					rwdata.slots[i].sock.physConn);
			}
		}
	}
	void _statInit()
	{
		maxQueueSize=0;
	    maxTimeInQueue=0;
		statInit=true;
	}


};

void _CommRWThreadProcess::_physConn( int slot, int remoteSlot_ )
{
	PASSERT(slot>=0 && slot< _SlotsForProcess::MaxSlot);

	_SlotsForProcess::_Slot & data = rwdata.slots[slot];
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
		PLog("CommRWThreadProcess[ %d ] slot=%d - closesocket() failed= %d",mgrIdx,slot,::WSAGetLastError());
	}

	data.remoteSlot = remoteSlot_;
	data.init=true;
	mgr.routingTable.physicallyConnected( data.sock.physConn, mgrIdx, slot, data.channelId, isLocal );
	PLog("CommRWThreadProcess[ %d ] - connected: addr= %s, slot=%d, remote slot=%d", mgrIdx, addr.c_str(), slot, remoteSlot_);
}

void _CommRWThreadProcess::run()
{
	try
	{
		const int maxSocket = 4;
		PWaitForObjects multiWait(maxSocket);
		_statInit();
		try
		{
			char sharedName[100];
			sprintf(sharedName,"sslp%8.8lx%8.8lx",::THREADID(),SYSTEMTICKS());
			comm.Create(sharedName);
			comm.enableMux();
			PString exeFileName;
			const char* sep = strchr(mgr.communicatorProcessName.c_str(), '\\');
			if (!sep)
			{
				sep = strchr(mgr.communicatorProcessName.c_str(), '/');
			}
			if (sep)
			{
				exeFileName.assign(mgr.communicatorProcessName);
			}
			else
			{
				exeFileName.assign(mgr.dirName).append(mgr.communicatorProcessName);
			}
			const char* parms[ 2 ];
			int npar=1;
			parms[ 0 ] = sharedName;
			if (*mgr.logFileDirName.c_str())
				{
				npar=2;
				parms[ 1 ] = mgr.logFileDirName.c_str();
				}
			process.start( exeFileName, npar, parms );
			PThread::sleep(100);  // hack - we should give process some time to start
		}
		catch( PError& err )
		{
			PLog( "CommRWThreadProcess[ %d ] - thread initialization failed: %s",mgrIdx, err.why() );
			_freeSocket(mgrIdx);
			return;
		}
		bool queueProcessed=false;
		for(;;)
		{

			SOCKET hnds[maxSocket];
			hnds[0] = comm.getReadHandle();
			hnds[1] = comm.getWriteHandle();
			hnds[2] = (SOCKET) process.getHandle();
			if (!hnds[0] || !hnds[1] || hnds[2]==(SOCKET) INVALID_HANDLE_VALUE)
			{
				PTRACE3( "CommRWThreadProcess[ %d ] invalid handles - exiting", mgrIdx );
				process.kill();
				process.closeHandles();
				_clearAllConnections();
				exit();
				return;
			}
			if (init)
			{
				hnds[3] = q.getSocket();
			}
			int w;
			int qSize=0;
			if (init &&  ( (queueProcessed && (qSize=q.sizeAndReset()) > 0) || (!queueProcessed && (qSize=q.size()) > 0)) )
			{
				w=3;
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
				int wait = init?4:3;
				w = multiWait.waitMultiple(wait,hnds,PWaitForObjects::infinite);
				if (w == PWaitForObjects::waitTimeout || w == PWaitForObjects::waitFailed)
				{
					PTRACE3( "CommRWThreadProcess[ %d ] shutdown - exiting", mgrIdx );
					process.kill();
					process.closeHandles();
					_clearAllConnections();
					exit();
					return;
				}
				else if (w==3)
					q.increaseAutoResetCounter();
			}
			queueProcessed = false;
			switch( w )
			{
			case 0:
				{
					int code;
					_CommMsg msg;
					while( 0 != (code=comm.getMessage(msg)))
					{
						if (code==-1)
						{
							PTRACE3( "CommRWThreadProcess[ %d ] shutdown - exiting", mgrIdx );
							comm.forceDisconnectSharedMemory();
							_clearAllConnections();
							exit();
							return;
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
									// Compose parameters
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

									comm.pushMessage(m);
									init=true;
									break;
								}
							case _SERVICE_REQUEST_PROCESS_CONNECT:
								{
									int slot = msg.internal.slot;
									PASSERT(slot>=0 && slot<_SlotsForProcess::MaxSlot);
									int remoteSlot;
									parser.parseINT32(remoteSlot);
									PASSERT(remoteSlot>0 && remoteSlot<PMultiSocketRW::MaxSockets);
									_physConn(slot,remoteSlot);
									break;
								}
							case _SERVICE_REQUEST_PROCESS_CONNECT_FAILED:
								{
									int slot = msg.internal.slot;
									PASSERT(slot>=0 && slot<_SlotsForProcess::MaxSlot);
									_SlotsForProcess::_Slot &data=rwdata.slots[slot];
									const char * failed;
									const char * errFailed;
									parser.parseString(failed).parseString(errFailed);
									PLog( "CommRWThreadProcess[ %d ]: slot #%d failed to connect: %s: %s", mgrIdx, slot, failed, errFailed);
									if (closesocket(data.sock.sockDescr.sock) == SOCKET_ERROR)
									{
										PLog("CommRWThreadProcess[ %d ]: slot #%d -CONNECT_FAILED- closesocket() failed= %d",mgrIdx, slot,::WSAGetLastError());
									}
									mgr.routingTable.physicallyDisconnected( data.sock.physConn );
									data.sock.physConn = 0;
									data.init = false;
									data.occupied = false;
									data.channelId = mgr.nextChannelId();//to enable protection from using failed connections - see comment above
									int idx = mgrIdx;
									if( _freeSocket( idx ) )
									{//no thread operations allowed after freeSocket returned true
										PTRACE3( "Exiting CommRWThreadProcess[ %d ]",  idx );
										comm.forceDisconnectSharedMemory();
										process.kill();
										process.closeHandles();
										return;
									}

									break;
								}
							case _SERVICE_REQUEST_PROCESS_DISCONNECT:
								{
									int slot = msg.internal.slot;
									PASSERT(slot>=0 && slot<_SlotsForProcess::MaxSlot);
									_SlotsForProcess::_Slot &data=rwdata.slots[slot];
									const char * failed;
									const char * errFailed;
									int errCode;
									parser.parseString(failed).parseString(errFailed).parseINT32(errCode);
									PLog( "CommRWThreadProcess[ %d ]: slot #%d closed: error %x - %s: %s", mgrIdx, slot, errCode, failed, errFailed);
									mgr.routingTable.physicallyDisconnected( data.sock.physConn, errCode );
									if (data.init == false)
									{
										PLog("CommRWThreadProcess[ %d ] slot#%d - socket not closed - closesocket()",mgrIdx, slot); 
										if (closesocket(data.sock.sockDescr.sock)==SOCKET_ERROR)
										{
											PLog("CommRWThreadProcess[ %d ] slot #%d  -PROCESS_DISCONNECT- closesocket() failed= %d",mgrIdx, slot,::WSAGetLastError());
										}

									}

									data.sock.physConn = 0;
									data.init = false;
									data.occupied = false;
									data.channelId = mgr.nextChannelId();//to enable protection from using failed connections - see comment above
									int idx = mgrIdx;
									if( _freeSocket( idx ) )
									{//no thread operations allowed after freeSocket returned true
										PTRACE3( "Exiting CommRWThreadProcess[ %d ]",  idx );
										comm.forceDisconnectSharedMemory();
										process.kill();
										process.closeHandles();
										return;
									}
									break;
								}
							case _SERVICE_REQUEST_SERVICE_CALL:
								{
									int slot = msg.internal.slot;
									PASSERT(slot>=0 && slot<_SlotsForProcess::MaxSlot);
									_SlotsForProcess::_Slot &data=rwdata.slots[slot];
									if (!(data.occupied && data.init))
									{
										PLog( "CommRWThreadProcess[ %d ] slot #%d: service request for closed channelId=%08X - ignored", mgrIdx, slot, msg.internal.channelId );
										break;
									}
									else if( msg.internal.channelId != data.channelId )
									{
										PLog( "CommRWThreadProcess[ %d ] slot #%d: service request for obsolete channelId=%08X - ignored", mgrIdx, slot, msg.internal.channelId );
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
							PASSERT(slot>=0 && slot<_SlotsForProcess::MaxSlot);
							_SlotsForProcess::_Slot &data=rwdata.slots[slot];
							if (!(data.occupied && data.init))
							{
								PLog( "CommRWThreadProcess[ %d ] slot #%d: message for closed channelId=%08X - ignored", mgrIdx, slot, msg.internal.channelId );
							}
							else if( msg.internal.channelId != data.channelId )
							{
								PLog( "CommRWThreadProcess[ %d ] slot #%d: message for obsolete channelId=%08X - ignored", mgrIdx, slot, msg.internal.channelId );
							}
							else
							{
								mgr.routingTable.routeIncomingMessage( data.sock.physConn, 0, 0, msg );
							}
						}
					}
					break;
				}
			case 1:
				comm.peerReadData();
				break;

			case 2:
				PTRACE3( "CommRWThreadProcess[ %d ] process died - exiting", mgrIdx );
				comm.forceDisconnectSharedMemory();
				_clearAllConnections();
				exit();
				return;

			case 3:
				{
					if( q.shuttingDown() )
					{
						PTRACE3( "CommRWThreadProcess[ %d ] shutdown - exiting", mgrIdx );
						comm.forceDisconnectSharedMemory();
						process.kill();
						process.closeHandles();
						exit();
						return;
					}
					queueProcessed = true;
					_CommMsg msg;
					UINT32 _timeInQueueTmp;
					if( !q.pop( msg,_timeInQueueTmp ) )
					{
						PTRACE3( "CommRWThreadProcess[ %d ]: empty queue awaken - ignored", mgrIdx );
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
						PASSERT(slot>=0 && slot<_SlotsForProcess::MaxSlot);
						_SlotsForProcess::_Slot &data=rwdata.slots[slot];
						if( msg.internal.channelId != data.channelId )
						{// Critical check. Protects from disconnecting the wrong channel
							PLog( "CommRWThreadProcess[ %d ]: slot #%d service request for obsolete channelId=%08X - ignored", mgrIdx, slot, msg.internal.channelId );
							break;
						}
						msg.internal.mux=data.remoteSlot;
						msg.internal.muxChannelId=data.channelId;
						comm.pushMessage(msg);
					}
					else if( msg.type == _COMM_MSGTYPE_NONE )//'special message' flag
					{
						if( msg.internal.slot == 0xFFFF )//pushSocket
						{
							CommMsgParser parser( msg.body );
							const BYTE* sock_;
							parser._parseFixedBlock( sock_, sizeof(CommRWThreadManagerProcess::Socket) );
							CommRWThreadManagerProcess::Socket* sock = (CommRWThreadManagerProcess::Socket*)sock_;
							int slot = rwdata.addSlot( *sock );

							PTRACE3( "CommRWThreadProcess[ %d ]: Socket placed to slot #%d", mgrIdx, slot );
							_SlotsForProcess::_Slot &data=rwdata.slots[slot];
							_CommMsg m( _COMM_MSGTYPE_SERVICEREQUEST, _COMM_MSGPRIORITIES-1 );
							m.body.composeUINT32(_SERVICE_REQUEST_PROCESS_CONNECT);
							m.body.composeINT32(slot);
							CommRWThreadManagerBase::Socket::composeSockDescriptor(sock->sockDescr, m.body);
//							m.body._composeFixedBlock((const BYTE *)&sock->sockDescr,sizeof(sock->sockDescr));
							data.channelId = mgr.nextChannelId();
							m.internal.mux=0xFFFF;
							m.internal.muxChannelId = data.channelId;
							comm.pushMessage(m);
						}
						else//forceDisconnect
						{
							int slot = msg.internal.slot;
							PASSERT(slot>=0 && slot<_SlotsForProcess::MaxSlot);
							_SlotsForProcess::_Slot &data=rwdata.slots[slot];
							if( msg.internal.channelId != data.channelId )
							{
								PLog( "CommRWThread[ %d ]: forceDisconnect obsolete channelId=%08X - ignored", mgrIdx, msg.internal.channelId );
								break;
							}
							_CommMsg m( _COMM_MSGTYPE_SERVICEREQUEST, _COMM_MSGPRIORITIES-1 );
							m.body.composeUINT32(_SERVICE_REQUEST_PROCESS_DISCONNECT);
							m.internal.mux=data.remoteSlot;
							m.internal.muxChannelId = data.channelId;
							comm.pushMessage(m);
						}
					}
					else
					{
						int slot = msg.internal.slot;
						PASSERT(slot>=0 && slot<_SlotsForProcess::MaxSlot);
						_SlotsForProcess::_Slot &data=rwdata.slots[slot];
						if( msg.internal.channelId != data.channelId )
						{
							PLog( "CommRWThreadProcess[ %d ] slot #%d: message to obsolete channelId=%08X - ignored", mgrIdx, slot, msg.internal.channelId );
							break;
						}
						msg.internal.mux=data.remoteSlot;
						msg.internal.muxChannelId = data.channelId;
						comm.pushMessage(msg);

					}
					break;
				}
			}

		}
	}
	catch( PFatalError& err )
		{
		PLogError( "CommRWThreadProcess[ %d ] last hope catch: %s",mgrIdx, err.why() );
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
		PLogError( "CommRWThreadProcess[ %d ] last hope catch: %s",mgrIdx, err.why() );
		process.kill();
		process.closeHandles();
		exit(true);
	}

}
#endif
#endif
