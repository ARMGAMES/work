#include "_commwebsocket.h"
#include "_commtmwebsocket.h"



struct _CommThreadWebSocketCommunicator:public _CommThreadDownCommunicator
{
	void construct( PMultiSocketRW& rw, int slot,  CommRWThreadManagerBase &mgr_)
	{
		_CommThreadDownCommunicator::construct(rw,slot,mgr_.routingTable);
	}
	_CommAsyncRet parentAsyncReadOk()
	{
		return _CommThreadDownCommunicator::asyncReadOk();
	}
};
 

class _CommWebSocketCommunicator
{
private:
	
	typedef _CommCompleteCommunicator< _CommThreadWebSocketCommunicator > _CommCompleteCommunicatorType;
	_CommCompleteCommunicatorType commBase;
	_CommWebSocketHandler< _CommCompleteCommunicatorType, _CommCompleteCommunicatorType> handler;

public:

	void construct( PMultiSocketRW& rw, int slot,  CommRWThreadManagerBase &mgr_, int mgrIdx_ )
	{
		commBase.construct( rw, slot, mgr_);
		handler.construct( commBase,commBase,slot, mgrIdx_);
	}

	void init( _CommPhysicalConnection* physConn)
	{
		commBase.init();
		commBase._CommThreadDownCommunicator::init(physConn);
		handler.init(physConn->address.c_str());
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
	void writeMsg(  _CommMsg& msg )
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
	void setFirstRead()
	{
		handler.setFirstRead();
	}
	bool serviceCallTransport(UINT32 id, CommMsgBody *msg_body)
	{
		return handler.serviceCallTransport(id,msg_body );
	}

};

#define _COMMWEBSOCKPREFIX "pyrws://"
#define _COMMWEBSOCKREFIXLEN 8

struct CommWebSocketProtocolHandler
{
	static bool handles( const char* address )
	{
		return PString::startsWith( address, _COMMWEBSOCKPREFIX );
	}
	static void parseSocketAddr( PString& retAddr, int& port, const char* address )
	{
		PASSERT3( PString::startsWith( address, _COMMWEBSOCKPREFIX ) );
		const char* p = address + _COMMWEBSOCKREFIXLEN;
		const char* col = strchr( p, ':' );
		if( col )
		{
			retAddr.assign( p, col );
			port = atoi( col + 1 );
		}
		else
			retAddr = p;
	}
	static void composeSocketAddr( PString& retAddr, const char* address, int port )
	{
		retAddr.append( _COMMWEBSOCKPREFIX ).append( address ).append( ':' ).appendInt( port );
	}

};



_CommRWThreadBase* CommThreadManagerWebSocket::_createNewThread(int threadIdx)
{
	return new _CommThreadWebSocket<_CommWebSocketCommunicator>(*this,threadIdx);
}

/* virtual */ bool CommThreadManagerWebSocket::handles( const char* address ) const
{
	return CommWebSocketProtocolHandler::handles( address );
}

/* virtual */ void CommThreadManagerWebSocket::parseSocketAddr( PString& retAddr, int& port, const char* address )
{
	CommWebSocketProtocolHandler::parseSocketAddr( retAddr, port, address );
}

/* virtual */ void CommThreadManagerWebSocket::composeSocketAddr( PString& retAddr, const char* address, int port )
{
	CommWebSocketProtocolHandler::composeSocketAddr( retAddr, address, port );
}
