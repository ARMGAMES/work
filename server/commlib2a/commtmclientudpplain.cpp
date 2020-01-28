// Use _WIN32 instead of PWIN32 here. 
#ifdef _WIN32

#include <winsock2.h>

#include "ppinclude.h"
#include "ppthread.h"
#include "ppsocket.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "commrtable.h"
#include "commtmclientudp.h"
#include "commproto.h"
#include "_commudp0.h"
#include "_commcomp.h"
#include "_commtmclientudp.h"


typedef _CommCompleteUDPCommunicator< _CommClientUDPCommunicator > _CommCompleteClientUDPCommunicator;

class _CommClientUDPPlain
{
private:
	_CommCompleteClientUDPCommunicator commBase;
	_CommUDP0ProtocolHandler< _CommCompleteClientUDPCommunicator, _CommClientUDPCommunicator > handler;

public:
	void construct( PMultiSocketRW& rw_, CommClientUDPThreadManager& mgr, _CommPhysicalConnection* physConn_, UINT32 frameSize_,  _CommProtocolLog * logging)
	{
		commBase.construct( rw_, mgr.routingTable, physConn_ );
		handler.construct( commBase, commBase,logging);
		handler.setDefaultFrameSize(mgr.udpPacketSize);
		handler.setKeepAlive(mgr.keepAliveWrite,mgr.keepAliveRead);
		handler.setResendTimeout(mgr.resend);

		commBase.init();//init CompleteCompressedClientCommunicator
		handler.init( false,0 );
	}
	bool isConnected() const
	{
		return handler.isConnected();
	}
	bool isReadReady() const
	{
		return commBase.isReadReady();
	}

	void writeMsg( _CommMsg& msg )
	{
		handler.writeMsg( msg );
	}
	void asyncWriteOk()
	{
		handler.asyncWriteOk();
	}
	void asyncReadOk()
	{
		handler.asyncReadOk();
	}
	void asyncReadCompleted()
	{
		commBase.asyncReadCompleted();
	}
	bool serviceCallTransport(UINT32 id, CommMsgBody *msg_body)
	{
		return handler.serviceCallTransport(id,msg_body );
	}
};

//****************************************************************************

/* virtual */ _CommClientUDPThreadBase* CommClientUDPThreadManager::_createNewThread( int threadIdx, _CommPhysicalConnection* physConn )
{
	return new _CommClientUDPThread< _CommClientUDPPlain >( *this, threadIdx, physConn, nextChannelId() );
}

/* virtual */ bool CommClientUDPThreadManager::handles( const char* address ) const
{
	return CommUdp0ProtocolHandler::handles( address );
}

/* virtual */ void CommClientUDPThreadManager::parseSocketAddr( PString& retAddr, int& port, const char* address )
{
	CommUdp0ProtocolHandler::parseSocketAddr( retAddr, port, address );
}

/* virtual */ void CommClientUDPThreadManager::composeSocketAddr( PString& retAddr, const char* address, int port )
{
	CommUdp0ProtocolHandler::composeSocketAddr( retAddr, address, port );
}
#endif