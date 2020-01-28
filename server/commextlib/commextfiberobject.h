#ifndef CommExtFiberServerObject_h_included
#define CommExtFiberServerObject_h_included

#include "ppincludebase.h"


#include "commextcliconn.h"
#include "commextfiberrequest.h"

#define DEFAULT_MAX_FIBERS 10

class _CommExtFiberGenericAsyncCall;

class CommExtFiberServerObject : public CommServerObject
{
private:
	__declspec(thread) static void * fiberMain;
	deque<UINT32> newRequests;
	deque<void *> fibers;
	CommIdMap<CommExtProxyAsyncCall *> asyncCalls;

#ifdef PWIN32
	static void CALLBACK FiberProc(void * param);
#endif

	struct FiberControlBlock
	{
		CommExtFiberRequest * req;
		UINT32  reqId;
		int retCode;
		FiberControlBlock()
		{
			req=0;
			reqId=0;
			retCode=0;
		}
	};
	FiberControlBlock fiberControlBlok;

protected:
	int  fiberCount;
	int  maximumFibers;
	struct Statistics
	{
		UINT32 switchesToFiber;
		UINT32 newRequests;
		UINT32 maxNewReadyRequests;
		UINT32 maxTotalReadyRequests;
		Statistics()
		{
			newRequests = 0;
			switchesToFiber = 0;
			maxNewReadyRequests = 0;
			maxTotalReadyRequests = 0;
		}
	};
	Statistics stat;

public:
	CommExtLocking lockSystem;
	void dynamicInit(const PIniFile & ini)
	{
		const PIniFile::Section* sec = ini.getSection("config");
		if (sec)
		{
			maximumFibers = sec->getIntProperty( "fibers",DEFAULT_MAX_FIBERS );
		}
	}
	CommExtFiberServerObject(_CommInterface& inter,int _maxFibers=DEFAULT_MAX_FIBERS):
		CommServerObject( inter ),lockSystem(true)
	{
		fiberCount = 0;
		maximumFibers = _maxFibers;
	}
	void fiberCheck();
	int getFibersSize() const
	{
		return fibers.size();
	}
	int getAsyncCallsSize() const
	{
		return asyncCalls.size();
	}

	virtual ~CommExtFiberServerObject();

	void addNewRequest(CommExtFiberRequest * req);
	void setToProcess(CommExtFiberRequest * req);
	void postToPool(CommExtCliConnPool & extPool, CommExtFiberRequest *req,  UINT32 msgId, CommMsgBody &request, CommMsgBody & reply);
	void postToPool(CommExtCliConnPool & extPool, UINT32 msgId, CommMsgBody &request);
	void switchToMainWait();
	void switchToMain();
	INT16 removeRequest(UINT32 reqId, PString& err);
	INT16 debugPrintRequest(UINT32 reqId, PString &err);
	void processNextRequest();
	void waitForRequest(CommExtFiberRequest *req, CommExtFiberRequest *currentRequest);
	CommExtFiberRequest * getActiveRequest() const
	{
		PASSERT3(fiberControlBlok.req && fiberControlBlok.req->getRequestId() == fiberControlBlok.reqId);
		return fiberControlBlok.req;
	}

protected:
	virtual void _releaseObjects(CommExtFiberRequest * req) = 0; // release resources assotiated with this request
private:
	void _releaseWaitingRequests(CommExtFiberRequest * req);
	void _processReplyFromGenericAsyncCall(UINT32 reqId, UINT32 callId);
	UINT32 _getNewReadyRequests();
	friend class _CommExtFiberGenericAsyncCall;
};



class _CommExtFiberGenericAsyncCall: public CommExtProxyAsyncCall
{
private:
	CommExtFiberServerObject * obj;
	CommExtCliConnPool *pool;
	CommMsgBody &reply;
	UINT32 reqId;
	UINT32 callId;

	void processReply( UINT32 msgId, CommMsgBody& body ) override
	{
		pool = 0;
		reply.moveFrom(body);
		obj->asyncCalls.remove(callId);
		obj->_processReplyFromGenericAsyncCall( reqId, callId );
	}

public:
	_CommExtFiberGenericAsyncCall(CommExtCliConnPool * pool_, CommExtFiberServerObject *  obj_, UINT32 msgId_, UINT32 reqId_, CommMsgBody & requestBody_, CommMsgBody & reply_ ):
	  CommExtProxyAsyncCall(msgId_,requestBody_),reply(reply_)
	{
		pool = pool_;
		obj = obj_;
		reqId = reqId_;
		callId = obj->asyncCalls.add(this);
		PTRACE5("rId=%08X:_CommExtFiberGenericAsyncCall:  msgId=%d, callId=%x",reqId, msgId_, callId);
	}
	friend class CommExtFiberServerObject;
};



#endif
