#include "ppinclude.h"
#include "ppthread.h"
#include "ppsocket.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "commrtable.h"
#include "commtmclient.h"
#include "commproto.h"
#include "_commtcp0.h"
#include "_commcomp.h"
#include "_commtmclient.h"

typedef _CommCompressedCommunicator< _CommClientCommunicator > _CommCompressedClientCommunicator;
typedef _CommCompleteCommunicator< _CommCompressedClientCommunicator > _CommCompleteCompressedClientCommunicator;

class _CommClientPlain
{
private:
	_CommCompleteCompressedClientCommunicator commBase;
	_CommStream0ProtocolHandler< _CommCompleteCompressedClientCommunicator, _CommClientCommunicator > handler;

public:
	void construct( PMultiSocketRW& rw_, CommClientThreadManagerBase& mgr, _CommPhysicalConnection* physConn_, bool inbound,_CommProtocolLog * logging)
	{
		commBase.construct( rw_, mgr.routingTable, physConn_ );
		handler.construct( commBase, commBase,logging);
		handler.setDefaultProtocol(mgr.DefaultProtocol);
		handler.setDefaultFrameSize(mgr.MaxBlockSize);
		handler.setDefaultCompressSize(mgr.CompressBitsSize);
		handler.setClientEncryption(mgr.enableClientEncryption);
        if (mgr.getNetworkStatsLogThreshold())
        {
            handler.setNetworkStatsLogThreshold(mgr.getNetworkStatsLogThreshold());
        }

		commBase._CommCompressedClientCommunicator::init();
		commBase.init();//init CompleteCompressedClientCommunicator
		handler.init( false );
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

/* virtual */ _CommClientThreadBase* CommClientThreadManager::_createNewThread( int threadIdx, _CommPhysicalConnection* physConn )
{
	return new _CommClientThread< _CommClientPlain >( *this, threadIdx, physConn, nextChannelId() );
}

/* virtual */ bool CommClientThreadManager::handles( const char* address ) const
{
	return CommTcp0ProtocolHandler::handles( address );
}

/* virtual */ void CommClientThreadManager::parseSocketAddr( PString& retAddr, int& port, const char* address )
{
	CommTcp0ProtocolHandler::parseSocketAddr( retAddr, port, address );
}

/* virtual */ void CommClientThreadManager::composeSocketAddr( PString& retAddr, const char* address, int port )
{
	CommTcp0ProtocolHandler::composeSocketAddr( retAddr, address, port );
}
