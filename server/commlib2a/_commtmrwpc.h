#ifndef _commtmrwpc_h_included
#define _commtmrwpc_h_included


#include "ppinclude.h"
#ifdef PWIN32
#include "ppstring.h"
#include "pplogfile.h"
#include "ppprocess.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "commsharedmemloop.h"



#define PROCESS_START_TIMEOUT 15000


class _SlotsForProcess
{
public:
	enum {MaxSlot = 1 };
	struct  _Slot
	{
		CommRWThreadManagerProcessClient::Socket sock;
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


class _CommRWThreadProcessClient : public _CommRWThreadBase
{

	protected:
	CommRWThreadManagerProcessClient& mgr;
	CommSharedMemoryEncryptedLoop comm;
	PProcess process;
	_SlotsForProcess rwdata;
	bool init;


	public:
	_CommRWThreadProcessClient( CommRWThreadManagerProcessClient& mgr_, int mgrIdx_ );
	UINT32 printCounters(int n,UINT32 & qSize, UINT32 & qTime, UINT32 & qSlotSize, UINT32 & qSlotTime) override
	{
		UINT32 _max = _CommManageableThread::printCounters(n,qSize,qTime,qSlotSize,qSlotTime);
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
				if (rwdata.slots[i].init == true)
				{
					mgr.routingTable.physicallyDisconnected( rwdata.slots[i].sock.physConn );
				}
				rwdata.slots[i].init=false;
				rwdata.slots[i].occupied=false;
				rwdata.slots[i].channelId = mgr.nextChannelId();
			}
		}
	}

    void _clearOutgoingConnections()
	{
		_CommMsg msg;
		while(  !q.shuttingDown() && q.pop( msg ) )
		{
			if( msg.type == _COMM_MSGTYPE_NONE )//'special message' flag
			{
				if( msg.internal.slot == 0xFFFF )//pushSocket
				{
					CommMsgParser parser( msg.body );
					const BYTE* sock_;
					parser._parseFixedBlock( sock_, sizeof(CommRWThreadManagerProcess::Socket) );
					CommRWThreadManagerProcess::Socket* sock = (CommRWThreadManagerProcess::Socket*)sock_;
					PLog( "CommRWThreadProcessProcess [%d] - clear outgoing connections", mgrIdx);
					mgr.routingTable.physicalConnectFailed( sock->physConn, -1 );
					_freeSocket(mgrIdx);
				}
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

};

void _CommRWThreadProcessClient::_physConn( int slot, int remoteSlot_ )
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

	data.remoteSlot = remoteSlot_;
	data.init=true;
	mgr.routingTable.physicallyConnected( data.sock.physConn, mgrIdx, slot, data.channelId, isLocal );
	PLog("CommRWThreadProcess[ %d ] - connected: addr= %s, slot=%d, remote slot=%d", mgrIdx, addr.c_str(), slot, remoteSlot_);
}

void _CommRWThreadProcessClient::run()
{
	try
	{
		const int maxSocket = 4;
		PWaitForObjects multiWait(maxSocket);
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
			_clearOutgoingConnections();
			CommSSL::threadCleanup();
			return;
		}
		for(;;)
		{

			SOCKET hnds[maxSocket];
			hnds[0] = comm.getReadHandle();
			hnds[1] = comm.getWriteHandle();
			hnds[2] = (SOCKET)process.getHandle();
			if (!hnds[0] || !hnds[1] || hnds[2]==(SOCKET)INVALID_HANDLE_VALUE)
			{
				PTRACE3( "CommRWThreadProcessClient[ %d ] invalid handles - exiting", mgrIdx );
				process.kill();
				process.closeHandles();
				_clearAllConnections();
				_clearOutgoingConnections();
				CommSSL::threadCleanup();
				return;
			}
			if (init)
			{
				hnds[3] = q.getSocket();
			}
			int w;
			int wait = init?4:3;
			if (init && q.sizeAndReset() > 0)
			{
				w=3;
			}
			else
			{
				w = multiWait.waitMultiple(wait,hnds,(init ? PWaitForObjects::infinite : PROCESS_START_TIMEOUT));
				if (w == PWaitForObjects::waitTimeout)
				{
					// Process didn't start in time - close everything
					PTRACE3( "CommRWThreadProcessClient[ %d ] process didn't start in time - exiting", mgrIdx );
					process.kill();
					process.closeHandles();
					_clearOutgoingConnections();
					CommSSL::threadCleanup();
					return;
				}
				if (w == PWaitForObjects::waitFailed)
				{
					PTRACE3( "CommRWThreadProcess[ %d ] shutdown - exiting", mgrIdx );
					process.kill();
					process.closeHandles();
					_clearAllConnections();
					CommSSL::threadCleanup();
					exit();
					return;
				}
				else if (w==3)
					q.increaseAutoResetCounter();

			}
			switch( w )
			{
			case 0:
				{
					int code;
					for (;;)
					{
						_CommMsg msg;
						if ( 0 == (code=comm.getMessage(msg)))
							break;
						if (code==-1)
						{
							PTRACE3( "CommRWThreadProcess[ %d ] shutdown - exiting", mgrIdx );
							comm.forceDisconnectSharedMemory();
							_clearAllConnections();
							CommSSL::threadCleanup();
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
									PString sendPassword64;
									PString recPassword64;
									PBlock  sendPassword;
									PBlock  recPassword;
									sendPassword.append(16);
									CommSSL::generateRandomBytes(sendPassword.ptr(),16);
									CommSSLBase64::encode( sendPassword64, sendPassword.ptr(), sendPassword.size() );
									recPassword.append(16);
									CommSSL::generateRandomBytes(recPassword.ptr(),16);
									CommSSLBase64::encode( recPassword64, recPassword.ptr(), recPassword.size() );

									_CommMsg m( _COMM_MSGTYPE_SERVICEREQUEST, _COMM_MSGPRIORITIES-1 );
									m.body.composeUINT32(_SERVICE_REQUEST_PROCESS_INIT);
									m.body.composeUINT32(PID())
										  .composeUINT32(mgr.keepAliveWrite)
										  .composeUINT32(mgr.keepAliveRead)
										  .composeUINT32(mgr.MaxBlockSize)
										  .composeUINT32(mgr.CompressBitsSize)
										  .composeUINT32(mgr.DefaultProtocol)
										  .composeString(mgr.commLoggingName);
									m.body.composeUINT32(mgr.certs.size());
									int i;
									for ( i = 0;i<mgr.certs.size();i++)
									{
										m.body.composeBOOL(mgr.certs[i]->encrypted)
											  ._composeVarBlock(mgr.certs[i]->certFile.ptr(),mgr.certs[i]->certFile.size())
											  .composeUINT32(mgr.certs[i]->key);
									}
									m.body.composeUINT32(mgr.cns.size());
									for ( i = 0;i<mgr.cns.size();i++)
									{
										m.body.composeBOOL(mgr.cns[i]->encrypted)
											  .composeString(mgr.cns[i]->cn)
											  .composeUINT32(mgr.cns[i]->key);
									}
									m.body.composeBOOL(mgr.optimizePacketSize)
										  .composeString(sendPassword64.c_str())
										  .composeString(recPassword64.c_str())
										  .composeString(mgr.httpsProxy.c_str())
										  .composeINT32(mgr.httpsProxyPort)
										  .composeBOOL(mgr.tcpNoDelay)
										  .composeINT32(mgr.sslConnectTimeout)
										  .composeINT32(mgr.tcpBuffer)
										  .composeBOOL(mgr.enableClientEncryption);
									m.body.composeString(mgr.sniHostName);
									comm.pushMessage(m);
									comm.setPasswords(sendPassword64.c_str(),recPassword64.c_str(), (const unsigned char *) "\x5F\x7E\x3B\x8A\xE5\x5D\xC3\x90", (const unsigned char *) "\x8B\x43\x70\x85\xC1\x9F\x84\x29" );
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
									mgr.routingTable.physicalConnectFailed( data.sock.physConn,1 );
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
									if (errCode == _SOCKET_SSLCERTIFICATEFAILED)
										mgr.routingTable.physicalConnectFailed( data.sock.physConn,-1,errCode);
									else
										mgr.routingTable.physicallyDisconnected( data.sock.physConn, errCode );
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
										break;
									}
									else if( msg.internal.channelId != data.channelId )
									{
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
							{// Critical check. Protects from disconnecting the wrong channel
								PLog( "CommRWThreadProcess[ %d ] slot #%d: message for closed channelId=%08X - ignored", mgrIdx, slot, msg.internal.channelId );
							}
							else if( msg.internal.channelId != data.channelId )
							{// Critical check. Protects from disconnecting the wrong channel
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
				CommSSL::threadCleanup();
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
						CommSSL::threadCleanup();
						exit();
						return;
					}
					_CommMsg msg;
					if( !q.pop( msg ) )
					{
						PTRACE3( "CommRWThreadProcess[ %d ]: empty queue awaken - ignored", mgrIdx );
						break;//switch
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
	CommSSL::threadCleanup();

}
#endif
#endif
