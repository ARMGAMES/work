#include "_commssl.h"
#include "_commexthttp.h"
#include "_commextthreadcommunicator.h"
#include "commexthttpthreadmanager.h"
//#include "commexthttpssl.h"



struct _CommExtThreadSSLCommunicator:public _CommExtThreadUpCommunicator,_CommSSLCommunicator
{
	_CommAsyncRet parentAsyncReadOk()
	{
		return _CommSSLCommunicator::asyncReadOk();
	}

};



template <class HttpHeader>
class _CommExtHttpsCommunicator
{
private:
	typedef _CommCompleteCommunicator< _CommExtThreadSSLCommunicator> _CommExtCompleteCommunicator;
	_CommExtThreadDownCommunicator  commLowLevel;
	_CommExtCompleteCommunicator commBase;
	_CommExtHttpClientHandler< _CommExtCompleteCommunicator,HttpHeader> handler;
	CommExtThreadManager * mgr;
	CommSSLInterlock<CommSSLClient> * cli;
#if !defined( COMM_CLIENT_ONLY )
	CommSSLInterlock<CommSSLServer> * srv;
#endif
public:
	_CommExtHttpsCommunicator<HttpHeader>()
	{
		cli = 0;
#if !defined( COMM_CLIENT_ONLY )
		srv = 0;
#endif
	}
	~_CommExtHttpsCommunicator<HttpHeader>()
	{
		if (cli && cli->removeLock())
		{
			delete cli;
		}
#if !defined( COMM_CLIENT_ONLY )
		if (srv && srv->removeLock())
		{
			delete srv;
		}
#endif
	}

	void construct( PMultiSocketRW& rw, int slot,  CommExtThreadManager &mgr_ )
	{
		mgr = &mgr_;
		commLowLevel.construct( rw, slot);
		commBase._CommExtThreadUpCommunicator::construct(mgr_);
		handler.construct( commBase );
		cli = 0;
#if !defined( COMM_CLIENT_ONLY )
		srv = 0;
#endif
	}

	void init( UINT32 connId, bool inbound, void *ssl_)
	{
		commBase.init();
#if !defined( COMM_CLIENT_ONLY )
		if (inbound)
		{
			PASSERT3(((CommExtHttpsServerInstance *)mgr)->srv);
			srv = ((CommExtHttpsServerInstance *)mgr)->srv;
			srv->setLock();
			commBase._CommSSLCommunicator::construct(0, srv->ssl_, &commLowLevel);
			commBase._CommSSLCommunicator::init(true);
		}
		else
#endif
		{
			PASSERT3(ssl_);
			cli = ( CommSSLInterlock<CommSSLClient> *) ssl_;
            cli->setLock();
			commBase._CommSSLCommunicator::construct(cli->ssl_, 0, &commLowLevel);
			commBase._CommSSLCommunicator::init(false);
		}
		commBase._CommExtThreadUpCommunicator::init(connId);
		handler.init();
	}
	void deinit()
	{
		if (cli && cli->removeLock())
		{
			delete cli;
		}
		cli = nullptr;
#if !defined( COMM_CLIENT_ONLY )
		if (srv && srv->removeLock())
		{
			delete srv;
		}
		srv = nullptr;
#endif
		commBase.deinit();
		handler.deinit();
	}
	bool isActive() const
	{
		return handler.isActive();
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
	void readStart()
	{
		handler.readStart();
	}
	void failed(int errCode)
	{
		handler.failed(errCode);
	}


};



_CommExtManagableThread* CommExtHttpsClientInstance::_createNewThread(int threadIdx)
{
	return new _CommExtThreadComm<_CommExtHttpsCommunicator<CommHttpResponse> >(*this,threadIdx);
}

#if !defined( COMM_CLIENT_ONLY )

_CommExtManagableThread* CommExtHttpsServerInstance::_createNewThread(int threadIdx)
{
	return new _CommExtThreadComm<_CommExtHttpsCommunicator<CommHttpServerRequest> >(*this,threadIdx);
}

#endif