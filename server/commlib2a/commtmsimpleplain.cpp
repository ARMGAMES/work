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

typedef _CommCompressedCommunicator< _CommSimpleCommunicator > _CommCompressedSimpleCommunicator;
typedef _CommCompleteCommunicator< _CommCompressedSimpleCommunicator > _CommCompleteCompressedSimpleCommunicator;

class _CommSimplePlain
{
private:
	_CommCompleteCompressedSimpleCommunicator commBase;
	_CommStream0ProtocolHandler< _CommCompleteCompressedSimpleCommunicator, _CommSimpleCommunicator > handler;

public:
	void construct( CommMsgQueue& q_, CommSimpleThreadManagerBase& mgr, _CommPhysicalConnection* physConn_, bool inbound,_CommProtocolLog * logging)
	{
		commBase.construct( q_, mgr.routingTable, physConn_ );
		handler.construct( commBase, commBase,logging);
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


		commBase._CommCompressedSimpleCommunicator::init();
		commBase.init();//init CompleteCompressedSimpleCommunicator
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

//****************************************************************************

/* virtual */ _CommSimpleThreadBase* CommSimpleThreadManager::_createNewThread( int threadIdx, _CommPhysicalConnection* physConn )
{
	return new _CommSimpleThread< _CommSimplePlain >( *this, threadIdx, physConn, nextChannelId() );
}

/* virtual */ bool CommSimpleThreadManager::handles( const char* address ) const
{
	return CommTcp0ProtocolHandler::handles( address );
}

/* virtual */ void CommSimpleThreadManager::parseSocketAddr( PString& retAddr, int& port, const char* address )
{
	CommTcp0ProtocolHandler::parseSocketAddr( retAddr, port, address );
}

/* virtual */ void CommSimpleThreadManager::composeSocketAddr( PString& retAddr, const char* address, int port )
{
	CommTcp0ProtocolHandler::composeSocketAddr( retAddr, address, port );
}
