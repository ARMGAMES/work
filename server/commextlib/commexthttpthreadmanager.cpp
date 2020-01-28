#include "_commexthttp.h"
#include "_commextthreadcommunicator.h"
#include "commexthttpthreadmanager.h"


struct _CommExtThreadCommunicator:public _CommExtThreadUpCommunicator,_CommExtThreadDownCommunicator
{
	void construct( PMultiSocketRW& rw, int slot,  CommExtThreadManager &mgr_)
	{
		_CommExtThreadDownCommunicator::construct(rw,slot);
		_CommExtThreadUpCommunicator::construct(mgr_);
	}
	_CommAsyncRet parentAsyncReadOk()
	{
		return _CommExtThreadDownCommunicator::asyncReadOk();
	}
};


template <class HttpHeader>
class _CommExtHttpCommunicator
{
private:
	
	typedef _CommCompleteCommunicator< _CommExtThreadCommunicator> _CommExtCompleteCommunicator;
	_CommExtCompleteCommunicator commBase;
	_CommExtHttpClientHandler< _CommExtCompleteCommunicator,HttpHeader> handler;

public:

	void construct( PMultiSocketRW& rw, int slot,  CommExtThreadManager &mgr_ )
	{
		commBase.construct( rw, slot, mgr_);
		handler.construct( commBase );
	}

	void init(UINT32 connId, bool inbound, void *ssl_)
	{
		commBase.init();
		commBase._CommExtThreadCommunicator::init(connId);
		handler.init();
	}
	void deinit()
	{
		commBase.deinit();
		handler.deinit();
	}
	bool isActive() const
	{
		return handler.isActive();
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
	void readStart()
	{
		handler.readStart();
	}
	void failed(int errCode)
	{
		handler.failed(errCode);
	}


};



_CommExtManagableThread* CommExtHttpClientInstance::_createNewThread(int threadIdx)
{
	return new _CommExtThreadComm<_CommExtHttpCommunicator<CommHttpResponse> >(*this,threadIdx);
}

#if !defined( COMM_CLIENT_ONLY )

_CommExtManagableThread* CommExtHttpServerInstance::_createNewThread(int threadIdx)
{
	return new _CommExtThreadComm<_CommExtHttpCommunicator<CommHttpServerRequest> >(*this,threadIdx);
}

#endif