#include "ppinclude.h"
#include "ppthread.h"
#include "ppsocket.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "commrtable.h"
#include "commtmsimple.h"
#include "commproto.h"
#include "_commtcp0.h"
#include "_commcomp.h"
#include "_commtmsimple.h"
#include "_commssl.h"

class _CommSimpleVCommunicator : public _CommVirtualCommunicator
{
private:
	_CommSimpleCommunicator comm;

public:
	void construct( CommMsgQueue& q_, CommRoutingTable& routingTable_, _CommPhysicalConnection* physConn_ )
	{
		comm.construct( q_, routingTable_, physConn_ );
	}
	void setPhysConn( _CommPhysicalConnection* physConn_ )
	{
		comm.setPhysConn( physConn_ );
	}

	size_t writeIncomplete( const BYTE* p, size_t sz ) override
	{
		return comm.writeIncomplete( p, sz );
	}
	size_t readIncomplete( BYTE* p, size_t sz ) override
	{
		return comm.readIncomplete( p, sz );
	}
	_CommAsyncRet asyncReadOk() override
	{
		return comm.asyncReadOk();
	}
	_CommAsyncRet asyncWriteOk() override
	{
		return comm.asyncWriteOk();
	}

	void msg( int mux, UINT32 muxChannelId, _CommMsg& msg )
	{
		comm.msg( mux, muxChannelId, msg );
	}
	void serviceCall(UINT32 id, CommMsgBody * msg_body)
	{
		comm.serviceCall(id,msg_body);
	}

	void bytesRead( const BYTE* p, size_t sz )
	{
		comm.bytesRead( p, sz );
	}
};

typedef _CommCompressedCommunicator< _CommSSLCommunicator > _CommCompressedSSLCommunicator;
typedef _CommCompleteCommunicator< _CommCompressedSSLCommunicator > _CommCompleteCompressedSSLCommunicator;

//#define _COMMDEBUGLOGTRAFFIC

class _CommSimpleSSL
{
private:
	_CommSimpleVCommunicator commBase;
#ifdef _COMMDEBUGLOGTRAFFIC
	typedef _CommCompleteCommunicator< _CommDebugCommunicator< _CommSSLCommunicator > > _CommCompleteDebugSSLCommunicator;
	_CommCompleteDebugSSLCommunicator comm;
	_CommStream0ProtocolHandler< _CommCompleteDebugSSLCommunicator, _CommSimpleVCommunicator > handler;
#else
	_CommCompleteCompressedSSLCommunicator comm;
	_CommStream0ProtocolHandler< _CommCompleteCompressedSSLCommunicator, _CommSimpleVCommunicator > handler;
#endif

public:
	void construct( CommMsgQueue& q_, CommSimpleThreadManagerBase& mgr_, _CommPhysicalConnection* physConn_, bool inbound,_CommProtocolLog *logging)
	{
		CommSimpleThreadManagerSSL& mgr = static_cast< CommSimpleThreadManagerSSL& >( mgr_ );
		commBase.construct( q_, mgr.routingTable, physConn_ );
		comm.construct( mgr.cli, 0, &commBase );
		handler.construct( comm, commBase,logging );
#ifndef COMM_CLIENT_ONLY
		handler.setMinimumProtocol(mgr.minimumProtocol);
#endif
		handler.setDefaultProtocol(mgr.DefaultProtocol);
		handler.setDefaultFrameSize(mgr.MaxBlockSize);
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


		comm._CommSSLCommunicator::init( inbound );
#ifndef _COMMDEBUGLOGTRAFFIC
		comm._CommCompressedSSLCommunicator::init();
#endif
		comm.init();//init CompleteSSLCommunicator
#ifdef _COMMDEBUGLOGTRAFFIC
		comm.initDebug( "rd.dat", "wr.dat" );
#endif
		handler.init( false );
	}
	void setPhysConn( _CommPhysicalConnection* physConn_ )
	{
		commBase.setPhysConn( physConn_ );
	}
	void setKeepAlivePoll( int msecPoll )
	{
		handler.setKeepAlive( msecPoll, 0, 0, false );
	}
	void setDefaultFrameSize(UINT32 FrameSize)
	{
		handler.setDefaultFrameSize(FrameSize);
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

	bool isConnected() const
	{
		return handler.isConnected();
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

	bool serviceCallTransport(UINT32 id, CommMsgBody *msg_body)
	{
		return handler.serviceCallTransport(id,msg_body );
	}
	void bytesRead( const BYTE* p, size_t sz )
	{
		commBase.bytesRead( p, sz );
	}
};

CommSimpleThreadManagerSSL::CommSimpleThreadManagerSSL( CommRoutingTable& routingTable_ )
: CommSimpleThreadManagerBase( routingTable_ )
{
	cli = 0;
}

void CommSimpleThreadManagerSSL::addClientTrustedCertificate( const BYTE* p, size_t sz )
{
	if( cli == 0 )
		cli = new CommSSLClient();
	cli->addTrustedCertificate( p, sz );
}

void CommSimpleThreadManagerSSL::clientRequireServerCertificateCN( const char* cn )
{
	if( cli == 0 )
		cli = new CommSSLClient();
	cli->requireServerCertificateCN( cn );
}

void CommSimpleThreadManagerSSL::addClientTrustedCertificateEncrypted( const BYTE* p, UINT32 key )
{
	if( cli == 0 )
		cli = new CommSSLClient();
	cli->addTrustedCertificateEncrypted( p, key);
}

void CommSimpleThreadManagerSSL::clientRequireServerCertificateCNEncrypted( const char* cn, UINT32 key )
{
	if( cli == 0 )
		cli = new CommSSLClient();
	cli->requireServerCertificateCNEncrypted( cn, key );
}


/* virtual */ _CommSimpleThreadBase* CommSimpleThreadManagerSSL::_createNewThread( int threadIdx, _CommPhysicalConnection* physConn )
{
	if( cli == 0 )
		cli = new CommSSLClient();
	return new _CommSimpleSSLThread< _CommSimpleSSL >( *this, threadIdx, physConn, nextChannelId() );
}

CommSimpleThreadManagerSSL::~CommSimpleThreadManagerSSL()
{
	delete cli;
}

/* virtual */ bool CommSimpleThreadManagerSSL::handles( const char* address ) const
{
	return CommSsl0ProtocolHandler::handles( address );
}

/* virtual */ void CommSimpleThreadManagerSSL::parseSocketAddr( PString& retAddr, int& port, const char* address )
{
	CommSsl0ProtocolHandler::parseSocketAddr( retAddr, port, address );
}

/* virtual */ void CommSimpleThreadManagerSSL::composeSocketAddr( PString& retAddr, const char* address, int port )
{
	CommSsl0ProtocolHandler::composeSocketAddr( retAddr, address, port );
}
