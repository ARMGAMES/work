#include "ppinclude.h"
#include "ppthread.h"
#include "ppsocket.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "commrtable.h"
#include "commtmrw.h"
#include "commtmrwudp.h"
#include "commproto.h"
#include "_commtcp0.h"
#include "_commcomp.h"
#include "_commssl.h"
#include "_commtmrw.h"
#include "commmonitorlogger.h"

template< class _MultiSocket >
class _CommRWVCommunicator : public _CommVirtualCommunicator
{
private:
	_CommRWCommunicator<_MultiSocket> comm;

public:
	void construct( _MultiSocket& rw_, int slot_, CommRoutingTable& routingTable_ )
	{
		comm.construct( rw_, slot_, routingTable_ );
	}

	void init( _CommPhysicalConnection* physConn_ )
	{
		comm.init( physConn_ );
	}

	/* virtual */ size_t writeIncomplete( const BYTE* p, size_t sz )
	{
		return comm.writeIncomplete( p, sz );
	}
	/* virtual */ size_t readIncomplete( BYTE* p, size_t sz )
	{
		size_t ret = comm.readIncomplete( p, sz );
		//PTRACE( "readIncomplete: %d", ret );
		return ret;
	}
	/* virtual */ _CommAsyncRet asyncReadOk()
	{
		_CommAsyncRet ret = comm.asyncReadOk();
		//PTRACE( "asyncReadOk: %d", ret.read );
		return ret;
	}
	/* virtual */ _CommAsyncRet asyncWriteOk()
	{
		return comm.asyncWriteOk();
	}

	void msg( int mux, UINT32 muxChannelId, _CommMsg& msg )
	{
		comm.msg( mux, muxChannelId, msg );
	}

	void forceSlotFailure( const char* errMsg, int errCode = 0 )
	{
		comm.forceSlotFailure( errMsg, errCode );
	}
	void serviceCall(UINT32 id, CommMsgBody * msg_body)
	{
		comm.serviceCall(id,msg_body);
	}

};

typedef _CommCompressedCommunicator< _CommSSLCommunicator > _CommCompressedSSLCommunicator;
typedef _CommCompleteCommunicator< _CommCompressedSSLCommunicator > _CommCompleteCompressedSSLCommunicator;


template< class _MultiSocket >
class _CommRWSSL
{
private:
	_CommRWVCommunicator<_MultiSocket> commBase;
	_CommCompleteCompressedSSLCommunicator comm;
	_CommStream0ProtocolHandler< _CommCompleteCompressedSSLCommunicator, _CommRWVCommunicator<_MultiSocket> > handler;

public:
	static const char* stringId() { return "SSL"; }
	void construct( _MultiSocket& rw, int slot, CommRWThreadManagerBase& mgr_,_CommProtocolLog * logger_  )
	{
		CommRWThreadManagerSSL& mgr = static_cast< CommRWThreadManagerSSL& >( mgr_ );
		commBase.construct( rw, slot, mgr.routingTable );
		comm.construct( mgr.cli, mgr.srv, &commBase );
		handler.construct( comm, commBase, logger_  );
#ifndef COMM_CLIENT_ONLY
		handler.setMinimumProtocol(mgr.minimumProtocol);
		handler.setParametersEnforced(mgr.minEnforcedBlock,mgr.maxEnforcedBlock, mgr.randomProtectionEnforced);
#endif
		handler.setDefaultProtocol(mgr.DefaultProtocol);
		handler.setDefaultFrameSize(mgr.MaxBlockSize);
		handler.setDefaultLargeFrameSize(mgr.MaxBlockSizeVer5);
		handler.setDefaultCompressSize(mgr.CompressBitsSize);
#ifndef COMM_CLIENT_ONLY
		handler.setMaximumMessageSize(mgr.maximumMessageSize,mgr.maxMessageSizeDebug);
		handler.setMaximumMessageRate(mgr.maximumMessageRate,mgr.maximumMessageRatePeriod,mgr.maxMessageRateDebug);

		if (mgr.enableServerEncryption)
		{
			handler.setServerEncryption(mgr.publicKey,mgr.privateKey);
		}
#endif
		handler.setClientEncryption(mgr.enableClientEncryption);
        if (mgr.getNetworkStatsLogThreshold())
        {
            handler.setNetworkStatsLogThreshold(mgr.getNetworkStatsLogThreshold());
        }

	}
	void enableMux()
	{
		handler.enableMux();
	}
	void setKeepAlive( int msecPoll, int msecWrite, int msecRead, bool keepAliveIgnoreDataPackets )
	{
		handler.setKeepAlive( msecPoll, msecWrite, msecRead, keepAliveIgnoreDataPackets );
	}
	void setDefaultFrameSize(UINT32 FrameSize)
	{
		handler.setDefaultFrameSize(FrameSize);
	}
	void setDefaultLargeFrameSize(UINT32 FrameSize)
	{
		handler.setDefaultLargeFrameSize(FrameSize);
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
	void setParametersEnforced(UINT32 minEnforcedBlock,UINT32 maxEnforcedBlock, UINT32 randomProtectionEnforced)
	{
		handler.setParametersEnforced(minEnforcedBlock,maxEnforcedBlock,randomProtectionEnforced);
	}
	void setMaximumMessageSize(UINT32 maximumMessageSize, bool maxMessageSizeDebug)
	{
		handler.setMaximumMessageSize(maximumMessageSize,maxMessageSizeDebug);
	}
	void setMaximumMessageRate(UINT32 maximumMessageRate, UINT32 maximumMessageRatePeriod, bool maxMessageRateDebug)
	{
		handler.setMaximumMessageRate(maximumMessageRate,maximumMessageRatePeriod,maxMessageRateDebug);
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


	void init( _CommPhysicalConnection* physConn )
	{
		commBase.init( physConn );
		comm._CommSSLCommunicator::init( physConn->inbound );
		comm._CommCompressedSSLCommunicator::init();
		comm.init();//init CompleteCompressedSSLCommunicator
		handler.init( physConn->inbound );
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
	void storeMsg( int mux, UINT32 muxChannelId, _CommMsg& msg )
	{
		handler.storeMsg( mux, muxChannelId, msg );
	}
	void flushStoredMsgs()
	{
		handler.flushStoredMsgs();
	}
};

//****************************************************************************

CommRWThreadManagerBaseServerSSL::CommRWThreadManagerBaseServerSSL(  )
{
	srv = 0;
}

CommRWThreadManagerBaseServerSSL::~CommRWThreadManagerBaseServerSSL()
{
	delete srv;
}


void CommRWThreadManagerBaseServerSSL::setServerKeyCert( const char* keyFile, const char* certFile, const char* password,  const char * sniName )
{
	if( srv == 0 )
		srv = new CommSSLServer();
	srv->setKeyCert( keyFile, certFile, password,sniName );
}

void CommRWThreadManagerBaseServerSSL::setServerKeyCertFromMemory( const BYTE* keyFileContent, 
														size_t keyFileContentSize,
														const BYTE* certFileContent,
														size_t certFileContentSize,
														const char* password,  const char * sniName )
{
	if( srv == 0 )
		srv = new CommSSLServer();
	srv->setKeyCertFromMemory( keyFileContent,keyFileContentSize, certFileContent,certFileContentSize, password, sniName );
}
void CommRWThreadManagerBaseServerSSL::setServerKeyCert( PBlock & keyFile_, PBlock & certFile_, const char* password_, const char * sniHost)
{
	if( srv == 0 )
		srv = new CommSSLServer();
	srv->setKeyCertFromMemory( keyFile_.ptr(),keyFile_.size(), certFile_.ptr(),certFile_.size(), password_, sniHost );
}


CommRWThreadManagerBaseClientSSL::CommRWThreadManagerBaseClientSSL(  )
{
	cli = 0;
}

CommRWThreadManagerBaseClientSSL::~CommRWThreadManagerBaseClientSSL()
{
	delete cli;
}

void CommRWThreadManagerBaseClientSSL::addClientTrustedCertificate( const BYTE* p, size_t sz )
{
	if( cli == 0 )
		cli = new CommSSLClient();
	cli->addTrustedCertificate( p, sz );
}

void CommRWThreadManagerBaseClientSSL::clientRequireServerCertificateCN( const char* cn )
{
	if( cli == 0 )
		cli = new CommSSLClient();
	cli->requireServerCertificateCN( cn );
}

void CommRWThreadManagerBaseClientSSL::addClientTrustedCertificateEncrypted( const BYTE* p, UINT32 key )
{
	if( cli == 0 )
		cli = new CommSSLClient();
	cli->addTrustedCertificateEncrypted( p, key);
}

void CommRWThreadManagerBaseClientSSL::clientRequireServerCertificateCNEncrypted( const char* cn, UINT32 key )
{
	if( cli == 0 )
		cli = new CommSSLClient();
	cli->requireServerCertificateCNEncrypted( cn, key );
}
void CommRWThreadManagerBaseClientSSL::setClientSniHostName(const char *hostName )
{
	if( cli == 0 )
		cli = new CommSSLClient();
	cli->setSniHostName( hostName );
}


CommRWThreadManagerSSL::CommRWThreadManagerSSL( CommRoutingTable& routingTable_ )
: CommRWThreadManagerBaseIP( routingTable_,false )
{
	PASSERT3(!connThread);
	connThread = new _CommRWConnectThreadIP(*this);
}

CommRWThreadManagerSSL::~CommRWThreadManagerSSL()
{
}

/* virtual */ _CommRWThreadBase* CommRWThreadManagerSSL::_createNewThread( int threadIdx )
{
	return new _CommRWSSLThread< _CommRWSSL<PMultiSocketRW>,PMultiSocketRW >( *this, threadIdx, muxEnabled );
}

/* virtual */ bool CommRWThreadManagerSSL::handles( const char* address ) const
{
	return CommSsl0ProtocolHandler::handles( address );
}

/* virtual */ void CommRWThreadManagerSSL::parseSocketAddr( PString& retAddr, int& port, const char* address )
{
	CommSsl0ProtocolHandler::parseSocketAddr( retAddr, port, address );
}

/* virtual */ void CommRWThreadManagerSSL::composeSocketAddr( PString& retAddr, const char* address, int port )
{
	CommSsl0ProtocolHandler::composeSocketAddr( retAddr, address, port );
}

void CommRWThreadManagerSSL::loggerInformation(CommMsgBody &body)
{
	PWLock lock( cs );
	CommRWThreadManagerBase::loggerInformation(body);

	if (isGatherStatistic())
	{
		PString s;
		s.assign("Max queue size(").append(managerType.c_str()).append(")");
		composeLoggerStat(body, s.c_str(), qSize);

		s.assign("Max time in queue (").append(managerType.c_str()).append(")");
		composeLoggerStat(body, s.c_str(), qTime);
		qSize=0;
		qTime=0;
		if (isGatherStatistic()>1)
		{
			s.assign("Max slot queue (").append(managerType.c_str()).append(")");
			composeLoggerStat(body, s.c_str(), qSlotSize);
			s.assign("Max time in slot queue (").append(managerType.c_str()).append(")");
			composeLoggerStat(body, s.c_str(), qSlotTime);
			qSlotSize=0;
			qSlotTime=0;
		}
	}

}
#ifdef UDT_DEVELOPMENT

/* virtual */ _CommRWThreadBase* CommRWThreadManagerSSLUDT::_createNewThread( int threadIdx )
{
	_CommRWThread< _CommRWSSL<PMultiSocketRWUDT>,PMultiSocketRWUDT > *thread = new _CommRWThread< _CommRWSSL<PMultiSocketRWUDT>,PMultiSocketRWUDT >( *this, threadIdx, muxEnabled );
	thread->rw.setSocketContainer(&pudtContainer);
	return thread;

}

/* virtual */ bool CommRWThreadManagerSSLUDT::handles( const char* address ) const
{
	return CommUdtSSL0ProtocolHandler::handles( address );
}

/* virtual */ void CommRWThreadManagerSSLUDT::parseSocketAddr( PString& retAddr, int& port, const char* address )
{
	CommUdtSSL0ProtocolHandler::parseSocketAddr( retAddr, port, address );
}

/* virtual */ void CommRWThreadManagerSSLUDT::composeSocketAddr( PString& retAddr, const char* address, int port )
{
	CommUdtSSL0ProtocolHandler::composeSocketAddr( retAddr, address, port );
}

void CommRWThreadManagerSSLUDT::loggerInformation(CommMsgBody &body)
{
	PWLock lock( cs );
	CommRWThreadManagerBase::loggerInformation(body);

	if (isGatherStatistic())
	{
		PString s;
		s.assign("Max queue size(").append(managerType.c_str()).append(")");
		composeLoggerStat(body, s.c_str(), qSize);

		s.assign("Max time in queue (").append(managerType.c_str()).append(")");
		composeLoggerStat(body, s.c_str(), qTime);
		qSize=0;
		qTime=0;
		if (isGatherStatistic()>1)
		{
			s.assign("Max slot queue (").append(managerType.c_str()).append(")");
			composeLoggerStat(body, s.c_str(), qSlotSize);
			s.assign("Max time in slot queue (").append(managerType.c_str()).append(")");
			composeLoggerStat(body, s.c_str(), qSlotTime);
			qSlotSize=0;
			qSlotTime=0;
		}
	}

}

#endif
