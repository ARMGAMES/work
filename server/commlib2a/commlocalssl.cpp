#include "ppinclude.h"
#ifdef PWIN32
#include "ppsocket.h"
#include "ppprocess.h"
#include "ppdaemon.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "commsrv.h"
#include "commcli.h"
#include "commgrdnull.h"
#include "commappfactory.h"
#include "commsharedmem.h"
#include "commsharedmemloop.h"
#include "_commtcp0.h"
#include "_commcomp.h"
#include "_commssl.h"
#include "commtmrw.h"


typedef CommSharedMemoryLoop CommSharedMemoryGeneric;

#ifdef PROTOCOLLOGGING
#define _log2  data.comm._log
#else
#define _log2 
#endif

#define TIMEOUT_FOR_CONNECTING 10000 

class _CommRWProcessWriteThread : public PThread
{
private:
	CommSharedMemoryGeneric& commShared;


public:
	_CommRWProcessWriteThread( CommSharedMemoryGeneric& commShared_):commShared(commShared_)
	{
	}
	void run() override;
	void shutdown();
};



struct _CommRWProcessCommunicator :public _CommVirtualCommunicator
{
private:
	PMultiSocketRW* rw;
	int slot;

public:
	void construct( PMultiSocketRW& rw_, int slot_)
	{
		rw = &rw_;
		slot = slot_;
	}
	void init()
	{
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

	void forceSlotFailure( const char* errMsg, int errCode )
	{
		rw->forceSlotFailure( slot, errMsg, errCode );
	}
};


typedef _CommCompressedCommunicator< _CommSSLCommunicator > _CommCompressedSSLProcessCommunicator;
typedef _CommCompleteCommunicator< _CommCompressedSSLProcessCommunicator > _CommProcessCompressedSSLCommunicator;

struct _CommRWProcessData;

class _CommRWSSLProcess
{
private:
	CommSharedMemoryGeneric * commShared;
	_CommRWProcessCommunicator commBase;
	_CommProcessCompressedSSLCommunicator comm;
	_CommStream0ProtocolHandler< _CommProcessCompressedSSLCommunicator, _CommRWSSLProcess > handler;
	UINT32 channelId;
	int slotRemote;


public:

	void construct( PMultiSocketRW& rw, int slot, CommSSLServer* srv_,CommSharedMemoryGeneric * commShared_,_CommProtocolLog * logger_ )
	{
		channelId=0;
		slotRemote=-1;
		commShared=commShared_;
		commBase.construct( rw, slot );
		comm.construct( 0, srv_, &commBase );
		handler.construct( comm, *this, logger_  );
	}

	void msg( int mux, UINT32 muxChannelId, _CommMsg& msg )
	{
		msg.internal.mux=slotRemote;
		msg.internal.muxChannelId=channelId;
		commShared->pushMessage( msg );
	}

	void serviceCall(UINT32 id, CommMsgBody * msg_body)
	{
		_CommMsg msg( _COMM_MSGTYPE_SERVICEREQUEST, _COMM_MSGPRIORITIES-1 );
		msg.internal.mux=slotRemote;
		msg.internal.muxChannelId=channelId;
		msg.body.composeUINT32(_SERVICE_REQUEST_SERVICE_CALL);
		msg.body.composeUINT32(id);
		if (msg_body)
		{
			msg.body.composeBYTE(1);
			msg.body.composeMsgBody(*msg_body);
		}
		else
		{
			msg.body.composeBYTE(0);
		}
		commShared->pushMessage(msg);
	}
	void _physConn(int slot)
	{
		_CommMsg msg( _COMM_MSGTYPE_SERVICEREQUEST, _COMM_MSGPRIORITIES-1 );
		msg.internal.mux=slotRemote;
		msg.internal.muxChannelId=channelId;
		msg.body.composeUINT32(_SERVICE_REQUEST_PROCESS_CONNECT);
		msg.body.composeINT32(slot);
		commShared->pushMessage(msg);
	}
	void _physDisconn(const char * failedMsg,const char * failedErrMsg, int errCode)
	{
		_CommMsg msg( _COMM_MSGTYPE_SERVICEREQUEST, 0 );
		msg.internal.mux=slotRemote;
		msg.internal.muxChannelId=channelId;
		msg.body.composeUINT32(_SERVICE_REQUEST_PROCESS_DISCONNECT);
		msg.body.composeString(failedMsg);
		msg.body.composeString(failedErrMsg);
		msg.body.composeINT32(errCode);
		commShared->pushMessage(msg);
	}
	void setKeepAlive( int msecPoll, int msecWrite, int msecRead, bool keepAliveIgnoreDataPackets )
	{
		handler.setKeepAlive( msecPoll, msecWrite, msecRead, keepAliveIgnoreDataPackets );
	}
	void setDefaultFrameSize(UINT32 FrameSize)
	{
		handler.setDefaultFrameSize(FrameSize);
	}
	void _log (const char * fmt, ...)
	{
		va_list args;
		va_start( args, fmt );
		handler._vlog(fmt, args);
		va_end(args);
	}
	void setDefaultCompressSize(UINT32 CompressBitsSize)
	{
		handler.setDefaultCompressSize(CompressBitsSize);
	}
	void setDefaultProtocol(UINT32 DefaultProtocol)
	{
		handler.setDefaultProtocol(DefaultProtocol);
	}
#ifndef COMM_CLIENT_ONLY
	void setMinimumProtocol(UINT32 minimumProtocol)
	{
		handler.setMinimumProtocol(minimumProtocol);
	}
	void setParametersEnforced(UINT32 minEnforcedBlock, UINT32 maxEnforcedBlock, UINT32 randomProtectionEnforced)
	{
		handler.setParametersEnforced(minEnforcedBlock,maxEnforcedBlock, randomProtectionEnforced);
	}
#endif
#ifndef COMM_CLIENT_ONLY
	void setMaximumMessageSize(UINT32 maximumMessageSize, bool dbg)
	{
		handler.setMaximumMessageSize(maximumMessageSize,dbg);
	}
	void setMaximumMessageRate(UINT32 maximumMessageRate, UINT32 maximumMessageRatePeriod, bool dbg)
	{
		handler.setMaximumMessageRate(maximumMessageRate,maximumMessageRatePeriod,dbg);
	}
	void setServerEncryption(UINT32 * publicKey, UINT32 * privateKey)
	{
		handler.setServerEncryption(publicKey,privateKey);
	}
#endif
	void setClientEncryption(bool enable)
	{
		handler.setClientEncryption(enable);
	}


	void init(int slotRemote_,UINT32 channelId_)
	{
		slotRemote = slotRemote_;
		channelId = channelId_;
		comm._CommSSLCommunicator::init( true );
		comm._CommCompressedSSLProcessCommunicator::init();
		comm.init();//init CompleteCompressedSSLCommunicator
		handler.init( true );
	}
	void deinit()
	{
		handler.deinit();
	}

	bool isConnected() const
	{
		return handler.isConnected();
	}

	void forceSlotFailure( const char* errMsg, int errCode = 0 )
	{
		commBase.forceSlotFailure( errMsg, errCode );
	}

	bool serviceCallTransport(UINT32 id, CommMsgBody *msg_body)
	{
		return handler.serviceCallTransport(id,msg_body );
	}

	void writeMsg( int mux, UINT32 muxChannelId, _CommMsg& msg )
	{
		handler.writeMsg( mux, muxChannelId, msg );
	}
	void asyncWriteOk()
	{
		handler.asyncWriteOk();
	}
	void asyncReadOk()
	{
		handler.asyncReadOk();
	}
	void setFirstRead()
	{
		handler.setFirstRead();
	}
	void storeMsg( int mux, UINT32 muxChannelId, _CommMsg& msg)
	{
		handler.storeMsg( mux, muxChannelId, msg);
	}
	void flushStoredMsgs()
	{
		handler.flushStoredMsgs();
	}

};



struct _CommRWProcessData
{
	PIPSocket::Descriptor sock;
	UINT32 channelId;
	_CommRWSSLProcess comm;
	UINT32 timeConnect;
	bool commInit;
	bool commConn;
	int remoteSlot;
};


class _CommLocalTcpProcessWriteThread : public PThread
{
private:
	CommSharedMemoryGeneric &commShared;
	SOCKET parentHandle;
	PWaitForObjects multiWait;
public:
	_CommLocalTcpProcessWriteThread( CommSharedMemoryGeneric & cmShared):commShared(cmShared),multiWait(2)
	{
	}
	void setProcessHandle(SOCKET parentHandle_)
	{
		parentHandle=parentHandle_;
	}

	void run() override
	{
		for(;;)
		{
			SOCKET hndl[2];
			hndl[0] = commShared.getWriteHandle();
			if (!hndl[0])
			{
				return;
			}
			hndl[1]=parentHandle;
			int w = multiWait.waitMultiple(2,hndl,PWaitForObjects::infinite);
			if (w == PWaitForObjects::waitFailed)
			{
				PString errMsg;
				PSystemError::errorMessage( errMsg, ::GetLastError() );
				PLog( "CommRWProcessWrite: wait failed - exiting : %s", errMsg.c_str());
				commShared.forceDisconnectSharedMemory();
				return;
			}

			if (w == 0)
			{
				commShared.peerReadData();
			}
			else if (w == 1)
			{
				PLog( "CommRWProcessWrite: parent process handle raised - exiting");
				commShared.forceDisconnectSharedMemory();
				return;
			}
		}
	}
};


class _CommLocalTcpProcessInterface 
{

public: 
	enum { _KeepAlivePoll = 1000 };
	_CommProtocolLog * comm_logging;
private:
	CommSharedMemoryGeneric commShared;
	_CommLocalTcpProcessWriteThread writeThread;
	bool init;
	HANDLE parentHandle;

public:
	_CommLocalTcpProcessInterface():writeThread(commShared)
	{
		init=false;
		comm_logging=0;
		parentHandle=0;
	}

	void forceRemoteSlotFailure(int slotRemote, const char * failedMsg, int err )
	{
		PString errMsg;
		PSystemError::errorMessage( errMsg, err );
		_CommMsg msg( _COMM_MSGTYPE_SERVICEREQUEST, _COMM_MSGPRIORITIES-1 );
		msg.internal.mux=slotRemote;
		msg.internal.muxChannelId=0;
		msg.body.composeUINT32(_SERVICE_REQUEST_PROCESS_CONNECT_FAILED);
		msg.body.composeString(failedMsg);
		msg.body.composeString(errMsg);
		commShared.pushMessage(msg);
	}
	void forceInitialError(const char * failedMsg, int err )
	{
		if (err)
		{
			PString errMsg;
			PSystemError::errorMessage( errMsg, err );
			PLog( "forceInitialError: %s :  '%s'", failedMsg, errMsg.c_str() );
		}
		else
		{
			PLog( "forceInitialError: %s", failedMsg );
		}
	}

	void construct(const char *name)
	{
		commShared.Open(name);
		commShared.enableMux();
	}
	void destruct()
	{
		commShared.closeSharedMemory();
		if (parentHandle)
		{
			::CloseHandle(parentHandle);
			parentHandle=0;
		}
	}

	void _run(const char* name );

};

class _CommLocalKillHandler : public PProcessKillHandler
{
private:
	bool _shutdown;
	CommSharedMemoryGeneric & comm;

public:
	_CommLocalKillHandler( CommSharedMemoryGeneric& comm_ ):comm(comm_)
	{
		_shutdown = false;
	}
	bool shutdown()
	{
		return _shutdown;
	}

public:
	bool signal() override
	{
		PTRACE("Shutdown signal received for %s",comm.getResourceName().c_str());
		_shutdown = true;
		comm.forceDisconnectSharedMemory();
		return true;
	}
};




void _CommLocalTcpProcessInterface::_run(const char* name )
{
	try
	{
		bool justComeIn = true;
		construct(name);
		_CommLocalKillHandler killHandler(commShared);
		_CommMsg m( _COMM_MSGTYPE_SERVICEREQUEST, _COMM_MSGPRIORITIES-1 );
		m.body.composeUINT32(_SERVICE_REQUEST_PROCESS_INIT);
		commShared.pushMessage(m); // Write thread hasn't started yet but we don't need it because not enough data to overwrite buffer.

		SOCKET hnds = commShared.getReadHandle();
		if (!hnds)
		{
			forceInitialError("LocalTcpProcess: invalid read handle",0);
			return;
		}
		int code;
		_CommMsg msg;
		do 
		{
			if (PWaitForObjects::waitSingle(hnds,PWaitForObjects::infinite)!= 0)
			{
				forceInitialError("WaitForSingleObject failed", ::GetLastError());
				return;
			}
			code=commShared.getMessage(msg);
			if (code ==-1)
			{
				forceInitialError("CommSharedMemory failed", 0);
				return;
			}
		} while(code == 0);

		if (msg.type != _COMM_MSGTYPE_SERVICEREQUEST)
		{
			forceInitialError("CommSharedMemory failed", 0);
			return;
		}

		CommMsgParser parser( msg.body );
		UINT32 id;
		parser.parseUINT32(id);
		if (id != _SERVICE_REQUEST_PROCESS_INIT)
		{
			forceInitialError("Invalid request", 0);
			return;
		}
		UINT32 parentProcessId;
		UINT32 keepAliveWrite;
		UINT32 keepAliveRead;
		UINT32 MaxBlockSize;
		UINT32 CompressBitsSize;
		UINT32 DefaultProtocol;
#ifndef COMM_CLIENT_ONLY
		UINT32 maximumMessageSize = 0;
		UINT32 maximumMessageRate = 0;
		UINT32 maximumMessageRatePeriod = 0;
		bool   maxMessageSizeDebug = false; 
		bool   maxMessageRateDebug = true; 

		UINT32 minimumProtocol = 0; // out of valid range, default protocol will be used
		UINT32 publicKey[5];
		UINT32 privateKey[5];
		bool   enableServerEncryption = false;
		UINT32 minEnforcedBlock = 0;
		UINT32 maxEnforcedBlock = 0;
		UINT32 randomProtectionEnforced=0;
#endif
		bool   enableClientEncryption = false;
		const char * commLoggingName;
		const BYTE * keyFile;
		const BYTE * certFile;
		size_t keyFileSize;
		size_t certFileSize;
		const char * password;
		bool optimizePacketSize=false;

		CommSSLServer * srv = new CommSSLServer();

		parser.parseUINT32(parentProcessId)
			.parseUINT32(keepAliveWrite)
			.parseUINT32(keepAliveRead)
			.parseUINT32(MaxBlockSize)
			.parseUINT32(CompressBitsSize)
			.parseUINT32(DefaultProtocol)
			.parseString(commLoggingName)
			._parseVarBlock(keyFile,keyFileSize)
			._parseVarBlock(certFile,certFileSize)
			.parseString(password);

		srv->setKeyCertFromMemory( keyFile, keyFileSize, certFile, certFileSize, password );
		
		if (!parser.parseEnded())
		{
			parser.parseBOOL(optimizePacketSize);
		}
		if (!parser.parseEnded())
		{
#ifndef COMM_CLIENT_ONLY
			parser.parseUINT32(maximumMessageSize);
			parser.parseUINT32(maximumMessageRate);
			parser.parseUINT32(maximumMessageRatePeriod);
			parser.parseBOOL(maxMessageSizeDebug);
			parser.parseBOOL(maxMessageRateDebug);
			parser.parseBOOL(enableServerEncryption);
#endif
			parser.parseBOOL(enableClientEncryption);
#ifndef COMM_CLIENT_ONLY
			if (enableServerEncryption)
			{
				parser.parseUINT32(publicKey[0])
					.parseUINT32(publicKey[1])
					.parseUINT32(publicKey[2])
					.parseUINT32(publicKey[3])
					.parseUINT32(publicKey[4])
					.parseUINT32(privateKey[0])
					.parseUINT32(privateKey[1])
					.parseUINT32(privateKey[2])
					.parseUINT32(privateKey[3])
					.parseUINT32(privateKey[4]);
			}
			parser.parseUINT32(minimumProtocol);
			if (!parser.parseEnded())
			{
				parser.parseUINT32(minEnforcedBlock)
				      .parseUINT32(maxEnforcedBlock)
				      .parseUINT32(randomProtectionEnforced);
			}
			if (!parser.parseEnded())
			{
				UINT32 numSni;
				parser.parseUINT32(numSni);
				PTRACE3("There are %d additional certificates for sni",numSni);
				for (int i=0;i<numSni;i++)
				{
					const char *sniName;
					parser.parseString(sniName)
						._parseVarBlock(keyFile,keyFileSize)
						._parseVarBlock(certFile,certFileSize)
						.parseString(password);
					srv->setKeyCertFromMemory( keyFile, keyFileSize, certFile, certFileSize, password, sniName);
				}
			}
#endif
		}

		parentHandle=::OpenProcess(PROCESS_ALL_ACCESS,FALSE,parentProcessId);
		if (!parentHandle)
		{
			forceInitialError("OpenProcess failed", ::GetLastError());
			return;
		}

		writeThread.setProcessHandle((SOCKET) parentHandle);
		writeThread.start();

		if (*commLoggingName)
		{
			PString commLog(commLoggingName);
			commLog.append(".");
			commLog.appendInt((int)PID());
			comm_logging = new _CommProtocolLog(commLog,PLogFile::perDate);
		}


		PMultiSocketRW rw( (SOCKET) commShared.getReadHandle(), PMultiSocketRW::MaxSockets - 1 );
		_CommRWProcessData rwData[ PMultiSocketRW::MaxSockets ];

		for( int i=0; i < PMultiSocketRW::MaxSockets ; ++i )
		{
			rwData[ i ].comm.construct( rw, i, srv, &commShared, comm_logging);
			rwData[ i ].commInit = false;
			rwData[ i ].commConn = false;
#ifndef COMM_CLIENT_ONLY
			rwData[ i ].comm.setMinimumProtocol(minimumProtocol);
			rwData[ i ].comm.setParametersEnforced(minEnforcedBlock,maxEnforcedBlock,randomProtectionEnforced);
#endif
			rwData[ i ].comm.setDefaultProtocol(DefaultProtocol);
			rwData[ i ].comm.setKeepAlive( _KeepAlivePoll, keepAliveWrite, keepAliveRead, false );
			rwData[ i ].comm.setDefaultFrameSize(MaxBlockSize);
			rwData[ i ].comm.setDefaultCompressSize(CompressBitsSize);
#ifndef COMM_CLIENT_ONLY
			rwData[ i ].comm.setMaximumMessageSize(maximumMessageSize,maxMessageSizeDebug);
			rwData[ i ].comm.setMaximumMessageRate(maximumMessageRate,maximumMessageRatePeriod,maxMessageRateDebug);
			if (enableServerEncryption)
			{
				rwData[ i ].comm.setServerEncryption(publicKey,privateKey);
			}
#endif
			rwData[ i ].comm.setClientEncryption(enableClientEncryption);


		}



		PLog( "Creating Server '%s'", name );
		UINT32 lastKeepAlive = SYSTEMTICKS();
		for(;;)
		{
			if( killHandler.shutdown() )
			{
				// Parent process sent kill signal - exiting
				PLog( "Kill signal received - exiting" );
				destruct();
				rw.shutdownSockets(); 
				return;
			}

			int slot = 0;

			UINT32 now = SYSTEMTICKS();
			UINT32 diff = now - lastKeepAlive;
			if( diff >= _KeepAlivePoll * TICKS_PER_SECOND / 1000 )
			{
				for( int i=0; i < PMultiSocketRW::MaxSockets ; ++i )
					if( rwData[ i ].commInit )
					{
						_CommRWProcessData& data = rwData[ i ];
						if(data.commConn)
						{
							try
							{
#ifdef COMM_PTR_TRACE
								CommMsgBody msg;
								msg.composeBYTE(1);
								bool Ok = data.comm.serviceCallTransport( _SERVICE_REQUEST_KEEP_ALIVE,&msg );
#else
								bool Ok = data.comm.serviceCallTransport( _SERVICE_REQUEST_KEEP_ALIVE,0 );
#endif
								if( !Ok )
									data.comm.forceSlotFailure( "keepAlive timeout expired",_SOCKET_TIMEOUT_EXPIRED );
							}
							catch( PError& err )
							{
								PLog( "Exception/1 in slot #%d: %s",  i, err.why() );
								data.comm.forceSlotFailure( err.why(),_SOCKET_EXCEPTION );
							}
						}
						else
						{
							try
							{
								if ((now -  data.timeConnect) > TIMEOUT_FOR_CONNECTING)
								{
									data.comm.forceSlotFailure( "SSL/TCP0 handshake timeout has expired",_SOCKET_HANDSHAKE_TIMEOUT_EXPIRED);
								}
							}
							catch( PError& err )
							{
								PLog( "Exception/2 in slot #%d: %s", i, err.why() );
								data.comm.forceSlotFailure( err.why(),_SOCKET_EXCEPTION );
							}
						}

					}
				lastKeepAlive = now;
				diff = 0;
			}

			UINT32 wait = diff < _KeepAlivePoll ? _KeepAlivePoll - diff : 0;

			if (!commShared.getReadHandle())
			{
				PTRACE3( "sharedMemoryClosed - exiting");
				destruct();
				rw.shutdownSockets();
				return;
			}

			int w;
			if (justComeIn == false)
			{
				w = rw.wait( slot, wait );
			}
			else
			{
				w = PMultiSocketRW::waitQueue;
				justComeIn = false;
			}


			switch( w )
			{
			case PMultiSocketRW::waitQueue:
				{
					int code;
					_CommMsg msg;
					while( 0 != (code=commShared.getMessage(msg)))
					{
						if (code==-1)
						{
							// Shared memory closed by the peer.
							PTRACE3( "shutdown - exiting" );
							destruct();
							rw.shutdownSockets();
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
								PLog("Unexpected _SERVICE_REQUEST_DUMP.");
								continue;
							}
							slot = msg.internal.slot;
							switch(id)
							{
							case _SERVICE_REQUEST_FORCE_KEEP_ALIVE:
								{
									PASSERT3( slot>0 && slot < PMultiSocketRW::MaxSockets );
									if( msg.internal.muxChannelId != rwData[ slot ].channelId )
									{// Critical check. Protects from disconnecting the wrong channel
										PLog( "forceKeepAlive obsolete channelId=%08X - ignored",  msg.internal.muxChannelId );
										break;
									}	
									_CommRWProcessData & data = rwData[ slot ];
									try
									{
										bool Ok =data.comm.serviceCallTransport( _SERVICE_REQUEST_FORCE_KEEP_ALIVE,0);
										if( !Ok )
											data.comm.forceSlotFailure( "keepAlive timeout expired",_SOCKET_TIMEOUT_EXPIRED );
									}
									catch( PError& err )
									{
										PLog( "CommLocalSSL exception _SERVICE_REQUEST_FORCE_KEEP_ALIVE in slot #%d: %s", slot, err.why() );
										data.comm.forceSlotFailure( err.why(),_SOCKET_EXCEPTION );
									}
									break;
								}
							case _SERVICE_REQUEST_QUALITY:
								{
									PASSERT3( slot>0 && slot < PMultiSocketRW::MaxSockets );
									if( msg.internal.muxChannelId != rwData[ slot ].channelId )
									{// Critical check. Protects from disconnecting the wrong channel
										PLog( "RequestQuality obsolete channelId=%08X - ignored", msg.internal.muxChannelId );
										break;
									}	
									_CommRWProcessData & data = rwData[ slot ];
									try
									{
										data.comm.serviceCallTransport( _SERVICE_REQUEST_QUALITY,0);
									}
									catch( PError& err )
									{
										PLog( "CommLocalSSL exception _SERVICE_REQUEST_QUALITY in slot #%d: %s", slot, err.why() );
										data.comm.forceSlotFailure( err.why(),_SOCKET_EXCEPTION );
									}
									break;
								}
							case _SERVICE_REQUEST_PROCESS_CONNECT:
								{
									int slotRemote;
									PIPSocket::Descriptor sockDescr;
									parser.parseINT32(slotRemote);
									CommRWThreadManagerBase::Socket::parseSockDescriptor(sockDescr,parser);

									HANDLE localSocket;
									if  (::DuplicateHandle(parentHandle,(HANDLE)sockDescr.sock,::GetCurrentProcess(),&localSocket,0,FALSE,DUPLICATE_SAME_ACCESS) == false)
									{
										forceRemoteSlotFailure(slotRemote, "Couldn't duplicate handle",::GetLastError() );
										break;
									}
									slot = rw.addSocket( (SOCKET)localSocket );
									PTRACE3( "Socket placed to slot #%d, remoteSlot #%d", slot, slotRemote );
									_CommRWProcessData& data = rwData[ slot ];
									try
									{
										data.comm.init(slotRemote,msg.internal.channelId);
										PString addr;
										PIPSocket::address( addr, sockDescr );
										addr.append(':').appendInt(PIPSocket::port( sockDescr ));
										data.sock = sockDescr;
										data.sock.sock=(SOCKET)localSocket;
										data.commInit = true;
										data.timeConnect = SYSTEMTICKS();
										data.channelId = msg.internal.channelId;
										data.remoteSlot = slotRemote;
										CommMsgBody msg_body;
										msg_body.composeINT32(slot);
										msg_body.composeString(addr);
										data.comm.serviceCallTransport( _SERVICE_REQUEST_SETIPADDRESS,&msg_body);
										if( data.comm.isConnected() )
										{
											data.comm._physConn( slot );
											data.commConn = true;
											data.comm.setFirstRead();
										}
										else
											data.commConn = false;
									}
									catch( PError& err )
									{
										PLog( "Exception/3 in slot #%d: %s", slot, err.why() );
										data.comm.forceSlotFailure( err.why(),_SOCKET_EXCEPTION );
									}
									break;
								}
							case _SERVICE_REQUEST_PROCESS_DISCONNECT:
								{

									PASSERT3( slot>0 && slot < PMultiSocketRW::MaxSockets );
									if( msg.internal.channelId != rwData[ slot ].channelId )
									{// Critical check. Protects from disconnecting the wrong channel
										PLog( "requestDisconnect obsolete channelId=%08X - ignored",  msg.internal.channelId );
										break;
									}
									_CommRWProcessData& data = rwData[ slot ];
									data.comm.forceSlotFailure( "forceDisconnect received" );
									break;
								}

							default:
								PTRACE3("_CommLocalTcpProcessInterface : Unknown service request ID %X",id);
								break;
							}
						}
						else
						{
							slot = msg.internal.slot;

							PASSERT3( slot>0 && slot < PMultiSocketRW::MaxSockets );
							if( msg.internal.channelId != rwData[ slot ].channelId )
							{// Critical check. Protects from dispatching the message to the wrong channel
								// when connection was deleted and then another connection created in the same slot.
								// Also protects from using failed connection: in case of failure 
								// rwData[ slot ].channelId must be changed
								PLog( "Message to obsolete channelId=%08X - ignored", msg.internal.channelId );
							}
							else
							{
								_CommRWProcessData& data = rwData[ slot ];
								try
								{
									if (optimizePacketSize)
										data.comm.storeMsg( 0, 0, msg );
									else
										data.comm.writeMsg( 0, 0, msg );
								}
								catch( PError& err )
								{
									PLog( "Exception/4 in slot #%d: %s", slot, err.why() );
									data.comm.forceSlotFailure( err.why(),_SOCKET_EXCEPTION );
								}
							}
						}
					}
					if (optimizePacketSize)
					{
						for( int i=0; i < PMultiSocketRW::MaxSockets ; ++i )
							if( rwData[ i ].commInit)
							{
								_CommRWProcessData& data = rwData[ i ];
								if(data.commConn)
								{
									try
									{
										data.comm.flushStoredMsgs();
									}
									catch( PError& err )
									{
										PLog( "Exception/1 in slot #%d: %s", i, err.why() );
										data.comm.forceSlotFailure( err.why(),_SOCKET_EXCEPTION );
									}
								}
							}
					}
					break;
				}

			case PMultiSocketRW::waitRead:
				{
					_CommRWProcessData& data = rwData[ slot ];

					try
					{
						data.comm.asyncReadOk();
						if( !data.commConn && data.comm.isConnected() )
						{
							data.comm._physConn( slot );
							data.commConn = true;
							data.comm.setFirstRead();
						}
					}
					catch( PError& err )
					{
						PLog( "Exception/rd in slot #%d: %s",  slot, err.why() );
						data.comm.forceSlotFailure( err.why() ,_SOCKET_EXCEPTION);
					}

				}
				break;

			case PMultiSocketRW::waitWrite:
				{
					_CommRWProcessData& data = rwData[ slot ];
					try
					{
						data.comm.asyncWriteOk();
						if( !data.commConn && data.comm.isConnected() )
						{
							data.comm._physConn( slot );
							data.commConn = true;
							data.comm.setFirstRead();
						}
					}
					catch( PError& err )
					{
						PLog( "Exception/wr in slot #%d: %s",  slot, err.why() );
						data.comm.forceSlotFailure( err.why(),_SOCKET_EXCEPTION );
					}
				}
				break;

			case PMultiSocketRW::waitTimeout:

				break;

			case PMultiSocketRW::waitFailed:
				{
					_CommRWProcessData& data = rwData[ slot ];

					PLog( "Slot #%d closed: error %x -  %s: %s", slot, rw.failedErrCode( slot ), rw.failedMsg( slot ), rw.failedErrMsg( slot ) );
					data.comm._log("Closed: error %x - %s: %s", rw.failedErrCode( slot ), rw.failedMsg( slot ), rw.failedErrMsg( slot ));
					data.comm._physDisconn(rw.failedMsg( slot ), rw.failedErrMsg( slot ), rw.failedErrCode(slot));
					rw.removeSocket( slot );
					data.comm.deinit();
					data.commInit = false;
					data.channelId = 0;
					data.commConn = false;
				}
				break;

			}
		}
	}
	catch( PError& err )
	{
		PLog( "Last hope catch in _run(): %s", err.why() );
	}
}



/****************************************************************************/

static void __cdecl _translate( unsigned int code, _EXCEPTION_POINTERS* xptr )
{
	char buf[ 256 ];
	sprintf( buf, "Win32 SEH Exception %08X ", code );
	EXCEPTION_RECORD* xrec = 0;
	if( xptr )
		xrec = xptr->ExceptionRecord;
	if( xrec )
	{
		sprintf( buf + strlen( buf ), "@%p", xrec->ExceptionAddress );
		for( int i=0; i < xrec->NumberParameters ; ++i )
		{
			if( i > 10 )
				break;
			sprintf( buf + strlen( buf ), ", %p", (void*)xrec->ExceptionInformation[ i ] );
		}
	}
	PLog("%s", buf);
	exit(-1);
}



int CommLocalTcpAppMain( int argc, char** argv )
{

	PString dir;
	if (argc>2)
	{
		dir.assign(argv[2]);
		if (dir[dir.length()-1] != '\\' &&
			dir[dir.length()-1] != '/' )
			dir.append(FILESEPARATORCHAR);
	}
	else
	{
		PWin32Service::serviceDir( dir );
	}
	dir.append("ssllog.log.");
	dir.appendInt((int)PID());
	PLogInit( new PLogFile( dir,  PLogFile::perDate | PLogFile::catchPErrors | PLogFile::catchSystemDbgOutput /* no thread id */ ) );
	PLog( "Starting ...");

	void (*oldTranslator)( unsigned int, _EXCEPTION_POINTERS* );
	oldTranslator = _set_se_translator( _translate );
	try
	{
		PSocket::startup();
		CommSSL::startup();
		_CommLocalTcpProcessInterface inter;
		const char* name = argv[ 1 ];
		PLog( "Running..." );
		inter._run( name );
		PLog( "Exiting..." );
		CommSSL::cleanup();
		PSocket::cleanup();
	}
	catch( PError& err )
	{
		PLog( "Last hope catch: %s", err.why() );
	}
	_set_se_translator( oldTranslator );

	return 0;

}
#endif
