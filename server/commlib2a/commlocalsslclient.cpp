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
#include "commsharedmemloop.h"
#include "_commtcp0.h"
#include "_commcomp.h"
#include "_commssl.h"
#include "commhttp.h"
#include "commtmrw.h"

#ifdef PROTOCOLLOGGING
#define _log2  data.comm._log
#else
#define _log2 
#endif

#define TIMEOUT_FOR_CONNECTING 10000 

class _CommRWProcessClientWriteThread : public PThread
{
private:
	CommSharedMemoryEncryptedLoop& commShared;

public:
	_CommRWProcessClientWriteThread( CommSharedMemoryEncryptedLoop& commShared_):commShared(commShared_)
	{
	}
	void run() override;
	void shutdown();
};



struct _CommRWProcessClientCommunicator :public _CommVirtualCommunicator
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


typedef _CommCompressedCommunicator< _CommSSLCommunicator > _CommCompressedSSLProcessClientCommunicator;
typedef _CommCompleteCommunicator< _CommCompressedSSLProcessClientCommunicator > _CommProcessClientCompressedSSLCommunicator;

struct _CommRWProcessClientData;

class _CommRWSSLProcessClient
{
private:
	CommSharedMemoryEncryptedLoop * commShared;
	_CommRWProcessClientCommunicator commBase;
	_CommProcessClientCompressedSSLCommunicator comm;
	_CommStream0ProtocolHandler< _CommProcessClientCompressedSSLCommunicator, _CommRWSSLProcessClient > handler;
	UINT32 channelId;
	int slotRemote;


public:

	void construct( PMultiSocketRW& rw, int slot, CommSSLClient* cln_,CommSharedMemoryEncryptedLoop * commShared_,_CommProtocolLog * logger_ )
	{
		channelId=0;
		slotRemote=-1;
		commShared=commShared_;
		commBase.construct( rw, slot );
		comm.construct( cln_, 0, &commBase );
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
#endif
	void setClientEncryption(bool enable)
	{
		handler.setClientEncryption(enable);
	}

	void init(int slotRemote_,UINT32 channelId_)
	{
		slotRemote = slotRemote_;
		channelId = channelId_;
		comm._CommSSLCommunicator::init( false );
		comm._CommCompressedSSLProcessClientCommunicator::init();
		comm.init();//init CompleteCompressedSSLCommunicator
		handler.init( false );
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



struct _CommRWProcessClientData
{
	PIPSocket::Descriptor sock;
	UINT32 channelId;
	_CommRWSSLProcessClient comm;
	UINT32 timeConnect;
	bool commInit;
	bool commConn;
	int remoteSlot;
};


class _CommLocalTcpProcessClientWriteThread : public PThread
{
private:
	CommSharedMemoryEncryptedLoop &commShared;
	SOCKET parentHandle;
	PWaitForObjects multiWait;
public:
	_CommLocalTcpProcessClientWriteThread( CommSharedMemoryEncryptedLoop & cmShared):commShared(cmShared),multiWait(2)
	{
	}
	void setProcessHandle(SOCKET parentHandle_)
	{
		parentHandle=parentHandle_;
	}

	void run()
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

class _CommLocalTcpProcessClientInterface 
{

public: 
	enum { _KeepAlivePoll = 1000 };
	_CommProtocolLog * comm_logging;
private:
	CommSharedMemoryEncryptedLoop commShared;
	_CommLocalTcpProcessClientWriteThread writeThread;
	bool init;
	HANDLE parentHandle;

public:
	_CommLocalTcpProcessClientInterface():writeThread(commShared)
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

class _CommLocalClientKillHandler : public PProcessKillHandler
{
private:
	bool _shutdown;
	CommSharedMemoryEncryptedLoop & comm;

public:
	_CommLocalClientKillHandler( CommSharedMemoryEncryptedLoop& comm_ ):comm(comm_)
	{
		_shutdown = false;
	}
	bool shutdown()
	{
		return _shutdown;
	}

public:
	/* virtual */ bool signal()
	{
		PTRACE("Shutdown signal received for %s",comm.getResourceName().c_str());
		_shutdown = true;
		comm.forceDisconnectSharedMemory();
		return true;
	}
};




void _CommLocalTcpProcessClientInterface::_run(const char* name )
{
	try
	{

		bool justComeIn = true;
		construct(name);
		_CommLocalClientKillHandler killHandler(commShared);
		_CommMsg m( _COMM_MSGTYPE_SERVICEREQUEST, _COMM_MSGPRIORITIES-1 );
		m.body.composeUINT32(_SERVICE_REQUEST_PROCESS_INIT);
		commShared.pushMessage(m); // Write thread hasn't started yet but we don't need it because not enough data to overwrite buffer.

		SOCKET hnds = commShared.getReadHandle();
		if (!hnds)
		{
			forceInitialError("LocalTcpProcess: invalid read handle",0);
			return;
		}
		_CommMsg msg;
		int code;

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
		const char * commLoggingName;
		bool optimizePacketSize=false;;
		const char * passReceive;
		const char * passSend;
		const char * httpsProxy = "";
		INT32 httpsProxyPort = 0;
		bool  tcpNoDelay = false;
		INT32 sslConnectTimeout = 0;
		INT32 tcpBuffer = -1;
		bool enableClientEncryption = false;

		CommSSLClient * cli = new CommSSLClient();

		parser.parseUINT32(parentProcessId)
			.parseUINT32(keepAliveWrite)
			.parseUINT32(keepAliveRead)
			.parseUINT32(MaxBlockSize)
			.parseUINT32(CompressBitsSize)
			.parseUINT32(DefaultProtocol)
			.parseString(commLoggingName);
		UINT32 certs, nCerts;
		parser.parseUINT32(nCerts);
		for (certs = 0; certs < nCerts; certs++)
		{
			bool encr;
			UINT32 key;
			const BYTE * certFile;
			size_t certFileSize;
			parser.parseBOOL(encr)
				._parseVarBlock(certFile,certFileSize)
				.parseUINT32(key);
			if (encr)
			{
				cli->addTrustedCertificateEncrypted( certFile, key );
			}
			else
			{
				cli->addTrustedCertificate( certFile, certFileSize );
			}

		}
		parser.parseUINT32(nCerts);
		for (certs = 0; certs < nCerts; certs++)
		{
			bool encr;
			UINT32 key;
			const char * cn;
			parser.parseBOOL(encr)
				.parseString(cn)
				.parseUINT32(key);
			if (encr)
			{
				cli->requireServerCertificateCNEncrypted( cn, key );
			}
			else
			{
				cli->requireServerCertificateCN( cn );
			}

		}

		parser.parseBOOL(optimizePacketSize)
			.parseString(passReceive)
			.parseString(passSend);
		if (!parser.parseEnded())
		{
			parser.parseString(httpsProxy)
				.parseINT32(httpsProxyPort)
				.parseBOOL(tcpNoDelay)
				.parseINT32(sslConnectTimeout)
				.parseINT32(tcpBuffer)
				.parseBOOL(enableClientEncryption);
				if (!parser.parseEnded())
				{
					const char *sniHostName;
					parser.parseString(sniHostName);
					if (*sniHostName)
						cli->setSniHostName(sniHostName);
				}
		}

		commShared.setPasswords(passSend,passReceive, (const unsigned char *)"\x8B\x43\x70\x85\xC1\x9F\x84\x29", (const unsigned char *)"\x5F\x7E\x3B\x8A\xE5\x5D\xC3\x90");

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
		_CommRWProcessClientData rwData[ PMultiSocketRW::MaxSockets ];

		for( int i=0; i < PMultiSocketRW::MaxSockets ; ++i )
		{
			rwData[ i ].comm.construct( rw, i, cli, &commShared, comm_logging);
			rwData[ i ].commInit = false;
			rwData[ i ].commConn = false;
			rwData[ i ].comm.setDefaultProtocol(DefaultProtocol);
			rwData[ i ].comm.setKeepAlive( _KeepAlivePoll, keepAliveWrite, keepAliveRead, false );
			rwData[ i ].comm.setDefaultFrameSize(MaxBlockSize);
			rwData[ i ].comm.setDefaultCompressSize(CompressBitsSize);
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
					if( rwData[ i ].commInit)
					{
						_CommRWProcessClientData& data = rwData[ i ];
						if(data.commConn)
						{
							try
							{
								bool Ok = data.comm.serviceCallTransport( _SERVICE_REQUEST_KEEP_ALIVE,0);
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
								if ((now -  data.timeConnect) > (sslConnectTimeout ? (sslConnectTimeout*TICKS_PER_SECOND) : TIMEOUT_FOR_CONNECTING))
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
					for (;;)
					{
						_CommMsg msg;
						code=commShared.getMessage(msg);
						if (code)
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
										_CommRWProcessClientData & data = rwData[ slot ];
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
										_CommRWProcessClientData & data = rwData[ slot ];
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
										PTRACE3( "Connect request");
										int slotRemote;
										PIPSocket::Descriptor sockDescr;
										parser.parseINT32(slotRemote);
										CommRWThreadManagerBase::Socket::parseSockDescriptor(sockDescr,parser);


										PIPSocket sock;
										PString address;
										PIPSocket::address(address,sockDescr);
										try
										{
											// THIS CODE IS DUPLICATED IN COMMTMSIMPLE.CPP Make changes there if you change something here
											if (tcpNoDelay)
											{
												PLog( "Disabling TCP delay" );
												bool Ok = sock.tcpNoDelay();
												if( !Ok )
													PLog( "Unable to disable TCP delay" );
											}
											if(tcpBuffer != -1 )
											{
												PLog( "Set TCP buffer %d", tcpBuffer);
												bool Ok = sock.tcpSetBuffer(tcpBuffer);
												if( !Ok )
													PLog( "Unable to set TCP buffer" );
											}

											if(*httpsProxy)
											{
												PLog( "Connecting to https proxy %s:%d", httpsProxy, httpsProxyPort );
												sock.connect( httpsProxy, httpsProxyPort );
												PLog( "Connected to https proxy %s:%d", httpsProxy, httpsProxyPort );

												CommHttpRequest req;
												PString url( "http://" );
												url.append( address ).append( ':' ).appendInt( PIPSocket::port(sockDescr) ).append( '/' );
												req.setUrl( url );
												req.setProxy( httpsProxy, httpsProxyPort );

												PString reqStr;
												req.createConnectRequest( reqStr );
												sock.sendComplete( (const BYTE*)reqStr.c_str(), strlen( reqStr ) );
												BYTE buf[ 2 ];
												CommHttpResponse resp;
												for(;;)
												{
													size_t rd = sock.recv( buf, 1,TIMEOUT_FOR_CONNECTING );//incomplete
													if ( rd <= 0 )
													{
														PString err( "HTTP Proxy response timeout" );
														throw PError( err );
													}

													const BYTE* p = buf;
													bool respOk = resp.read( p, rd );
													if( respOk )
													{
														int code = resp.getRespCode();
														if( code != 200 )
														{
															PString err( "HTTP Proxy response: " );
															err.appendInt( code );
															throw PError( err );
														}
														break;
													}
												}
											}
											else
											{
												PLog( "Connecting: %s:%d", address.c_str(), PIPSocket::port(sockDescr) );
												sock.connect( address, PIPSocket::port(sockDescr) );
												PLog( "Connected: %s:%d", address.c_str(), PIPSocket::port(sockDescr) );
											}
										}
										catch( PError& err )
										{
											PLog( "Exception/3 in slot #%d: %s", slot, err.why() );
											forceRemoteSlotFailure(slotRemote, "Couldn't connect",0);
											break;
										}

										PIPSocket::Descriptor sockDescrNew;
										sock.extractDescriptor( sockDescrNew );
										slot = rw.addSocket( sockDescrNew.sock);
										PTRACE3( "Socket placed to slot #%d, remoteSlot #%d", slot, slotRemote );
										_CommRWProcessClientData& data = rwData[ slot ];
										try
										{
											data.comm.init(slotRemote,msg.internal.channelId);
											PString addr;
											PIPSocket::address( addr, sockDescrNew );
											data.sock = sockDescrNew;
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
										catch( PSSLError& err )
										{
											PLog( "SSL exception/4 in slot #%d: %s", slot, err.why() );
											data.comm.forceSlotFailure( err.why(),_SOCKET_SSLCERTIFICATEFAILED );
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
										_CommRWProcessClientData& data = rwData[ slot ];
										data.comm.forceSlotFailure( "forceDisconnect received" );
										break;
									}

								default:
									PTRACE3("Unknown service request ID %X",id);
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
									_CommRWProcessClientData& data = rwData[ slot ];
									try
									{
										if (optimizePacketSize)
											data.comm.storeMsg( 0, 0, msg );
										else
											data.comm.writeMsg( 0, 0, msg );
									}
									catch( PSSLError& err )
									{
										PLog( "SSL exception/4 in slot #%d: %s", slot, err.why() );
										data.comm.forceSlotFailure( err.why(),_SOCKET_SSLCERTIFICATEFAILED );
									}

									catch( PError& err )
									{
										PLog( "Exception/4 in slot #%d: %s", slot, err.why() );
										data.comm.forceSlotFailure( err.why(),_SOCKET_EXCEPTION );
									}
								}
							}
						}
						else
							break;
					}
					if (optimizePacketSize)
					{
						for( int i=0; i < PMultiSocketRW::MaxSockets ; ++i )
							if( rwData[ i ].commInit)
							{
								_CommRWProcessClientData& data = rwData[ i ];
								if(data.commConn)
								{
									try
									{
										data.comm.flushStoredMsgs();
									}
									catch( PSSLError& err )
									{
										PLog( "SSL exception/1 in slot #%d: %s", i, err.why() );
										data.comm.forceSlotFailure( err.why(),_SOCKET_SSLCERTIFICATEFAILED );
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
					_CommRWProcessClientData& data = rwData[ slot ];

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
					catch( PSSLError& err )
					{
						PLog( "SSL exception/rd in slot #%d: %s", slot, err.why() );
						data.comm.forceSlotFailure( err.why(),_SOCKET_SSLCERTIFICATEFAILED );
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
					_CommRWProcessClientData& data = rwData[ slot ];
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
					catch( PSSLError& err )
					{
						PLog( "SSL exception/wr in slot #%d: %s", slot, err.why() );
						data.comm.forceSlotFailure( err.why(),_SOCKET_SSLCERTIFICATEFAILED );
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
					_CommRWProcessClientData& data = rwData[ slot ];

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



int CommLocalTcpAppClientMain( int argc, char** argv )
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
	dir.append("PokerStarsCommunicate.log");
	PLogInit( new PLogFile( dir, PLogFile::perDateLimited ) );
	PLog( "Starting ...");

	void (*oldTranslator)( unsigned int, _EXCEPTION_POINTERS* );
	oldTranslator = _set_se_translator( _translate );
	try
	{
		PSocket::startup();
		CommSSL::startup();
		_CommLocalTcpProcessClientInterface inter;
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
