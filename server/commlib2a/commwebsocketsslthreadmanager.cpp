#include "_commwebsocket.h"
#include "_commtmwebsocket.h"
#include "_commssl.h"


struct _CommThreadWebSocketSSLCommunicator:public _CommSSLCommunicator
{
	_CommAsyncRet parentAsyncReadOk()
	{
		return _CommSSLCommunicator::asyncReadOk();
	}
};


class _CommWebSocketSSLCommunicator
{
private:
	
	typedef _CommCompleteCommunicator< _CommThreadWebSocketSSLCommunicator > _CommCompleteCommunicatorType;
	_CommThreadDownCommunicator  commLowLevel;
	_CommCompleteCommunicatorType commBase;
	_CommWebSocketHandler< _CommCompleteCommunicatorType,_CommThreadDownCommunicator> handler;

public:

	void construct( PMultiSocketRW& rw, int slot,  CommRWThreadManagerBase &mgr_, int mgrIdx_ )
	{
		commLowLevel.construct( rw, slot, mgr_.routingTable);
		commBase._CommSSLCommunicator::construct(0,((CommThreadManagerWebSocketSSL &)mgr_).srv, &commLowLevel);
		handler.construct( commBase, commLowLevel,slot,mgrIdx_);
	}

	void init( _CommPhysicalConnection* physConn)
	{
		commBase.init();
		commBase._CommSSLCommunicator::init(true);
		commLowLevel.init(physConn);
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
		commLowLevel.forceSlotFailure( errMsg, errCode );
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

#define _COMMWEBSOCKSSLPREFIX "pyrwss://"
#define _COMMWEBSOCKSSLREFIXLEN 9

struct CommWebSocketSSLProtocolHandler
{
	static bool handles( const char* address )
	{
		return PString::startsWith( address, _COMMWEBSOCKSSLPREFIX );
	}
	static void parseSocketAddr( PString& retAddr, int& port, const char* address )
	{
		PASSERT3( PString::startsWith( address, _COMMWEBSOCKSSLPREFIX ) );
		const char* p = address + _COMMWEBSOCKSSLREFIXLEN;
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
		retAddr.append( _COMMWEBSOCKSSLPREFIX ).append( address ).append( ':' ).appendInt( port );
	}

};



_CommRWThreadBase* CommThreadManagerWebSocketSSL::_createNewThread(int threadIdx)
{
	return new _CommThreadWebSocket<_CommWebSocketSSLCommunicator>(*this,threadIdx);
}

/* virtual */ bool CommThreadManagerWebSocketSSL::handles( const char* address ) const
{
	return CommWebSocketSSLProtocolHandler::handles( address );
}

/* virtual */ void CommThreadManagerWebSocketSSL::parseSocketAddr( PString& retAddr, int& port, const char* address )
{
	CommWebSocketSSLProtocolHandler::parseSocketAddr( retAddr, port, address );
}

/* virtual */ void CommThreadManagerWebSocketSSL::composeSocketAddr( PString& retAddr, const char* address, int port )
{
	CommWebSocketSSLProtocolHandler::composeSocketAddr( retAddr, address, port );
}
