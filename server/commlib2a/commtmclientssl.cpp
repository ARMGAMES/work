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
#include "_commssl.h"

class _CommClientVCommunicator : public _CommVirtualCommunicator
{
private:
	_CommClientCommunicator comm;

public:
	void construct( PMultiSocketRW& rw_, CommRoutingTable& routingTable_, _CommPhysicalConnection* physConn_ )
	{
		comm.construct( rw_, routingTable_, physConn_ );
	}
	/* virtual */ size_t writeIncomplete( const BYTE* p, size_t sz )
	{
		return comm.writeIncomplete( p, sz );
	}
	/* virtual */ size_t readIncomplete( BYTE* p, size_t sz )
	{
		return comm.readIncomplete( p, sz );
	}
	/* virtual */ _CommAsyncRet asyncReadOk()
	{
		return comm.asyncReadOk();
	}
	/* virtual */ _CommAsyncRet asyncWriteOk()
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
};

typedef _CommCompressedCommunicator< _CommSSLCommunicator > _CommCompressedSSLCommunicator;
typedef _CommCompleteCommunicator< _CommCompressedSSLCommunicator > _CommCompleteCompressedSSLCommunicator;

//#define _COMMDEBUGLOGTRAFFIC

class _CommClientSSL
{
private:
	_CommClientVCommunicator commBase;
#ifdef _COMMDEBUGLOGTRAFFIC
	typedef _CommCompleteCommunicator< _CommDebugCommunicator< _CommSSLCommunicator > > _CommCompleteDebugSSLCommunicator;
	_CommCompleteDebugSSLCommunicator comm;
	_CommStream0ProtocolHandler< _CommCompleteDebugSSLCommunicator, _CommClientVCommunicator > handler;
#else
	_CommCompleteCompressedSSLCommunicator comm;
	_CommStream0ProtocolHandler< _CommCompleteCompressedSSLCommunicator, _CommClientVCommunicator > handler;
#endif

public:
	void construct( PMultiSocketRW& rw_, CommClientThreadManagerBase& mgr_, _CommPhysicalConnection* physConn_, bool inbound,_CommProtocolLog *logging)
	{
		CommClientThreadManagerSSL& mgr = static_cast< CommClientThreadManagerSSL& >( mgr_ );
		commBase.construct( rw_, mgr.routingTable, physConn_ );
		comm.construct( mgr.cli, 0, &commBase );
		handler.construct( comm, commBase,logging );
		handler.setDefaultProtocol(mgr.DefaultProtocol);
		handler.setDefaultFrameSize(mgr.MaxBlockSize);
		handler.setDefaultCompressSize(mgr.CompressBitsSize);
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

CommClientThreadManagerSSL::CommClientThreadManagerSSL( CommRoutingTable& routingTable_ )
: CommClientThreadManagerBase( routingTable_ )
{
}


/* virtual */ _CommClientThreadBase* CommClientThreadManagerSSL::_createNewThread( int threadIdx, _CommPhysicalConnection* physConn )
{
	if( cli == 0 )
		cli = new CommSSLClient();
	return new _CommClientSSLThread< _CommClientSSL >( *this, threadIdx, physConn, nextChannelId() );
}

CommClientThreadManagerSSL::~CommClientThreadManagerSSL()
{
}

/* virtual */ bool CommClientThreadManagerSSL::handles( const char* address ) const
{
	return CommSsl0ProtocolHandler::handles( address );
}

/* virtual */ void CommClientThreadManagerSSL::parseSocketAddr( PString& retAddr, int& port, const char* address )
{
	CommSsl0ProtocolHandler::parseSocketAddr( retAddr, port, address );
}

/* virtual */ void CommClientThreadManagerSSL::composeSocketAddr( PString& retAddr, const char* address, int port )
{
	CommSsl0ProtocolHandler::composeSocketAddr( retAddr, address, port );
}
