#include "commextfiberobject.h"

#ifdef PWIN32
void * CommExtFiberServerObject::fiberMain = 0;
__declspec(thread) UINT32  currentReqId=0;

void CommExtFiberServerObject::fiberCheck()
{
	if (!fiberMain)
	{
		fiberMain = ::ConvertThreadToFiber(0);
		PASSERT3( fiberMain );
	}
}

void CommExtFiberServerObject::addNewRequest(CommExtFiberRequest * req)
{
	stat.newRequests++;
	lockSystem.addRequest(req);
	PTRACE4("rId=%08X:{R",req->getRequestId());
	newRequests.push_back(req->getRequestId());
	if( stat.maxNewReadyRequests < newRequests.size() )
		stat.maxNewReadyRequests = newRequests.size();
}

void CommExtFiberServerObject::setToProcess(CommExtFiberRequest * req)
{
	PTRACE4("rId=%08X:setToProcess",req->getRequestId());
	if (req->isReadyToProcess())
	{
		lockSystem.addReadyRequest(req->getRequestId());
		if( stat.maxTotalReadyRequests < lockSystem.getReadyRequestSize() )
			stat.maxTotalReadyRequests = lockSystem.getReadyRequestSize();
	}
}

void CommExtFiberServerObject::postToPool(CommExtCliConnPool & extPool, CommExtFiberRequest *req,  UINT32 msgId, CommMsgBody &request, CommMsgBody & reply)
{
	_CommExtFiberGenericAsyncCall *call = new _CommExtFiberGenericAsyncCall(&extPool,this,msgId,req->getRequestId(),request,reply);
	req->waitingAsynCall.insert(call->callId);
	extPool.postTo(call);
	switchToMainWait();
	PASSERT3(req->isReadyToProcess());
}

void CommExtFiberServerObject::postToPool(CommExtCliConnPool & extPool, UINT32 msgId, CommMsgBody &request)
{
	CommExtProxyAsyncCall *call = new CommExtProxyAsyncCall(msgId,request);
	extPool.postTo(call);
}



void CommExtFiberServerObject::_processReplyFromGenericAsyncCall(UINT32 reqId, UINT32 callId)
{
	UINT32 ticks = SYSTEMTICKS();
	PTRACE4("rId=%08X:{AR processReplyFromGenericAsyncCall ... callId=%08X",reqId, callId);

	CommExtFiberRequest * req =(CommExtFiberRequest*)lockSystem.getRequestSafe(reqId);
	if (req)
	{
		PASSERT3(req->waitingAsynCall.find(callId) != req->waitingAsynCall.end());
		req->waitingAsynCall.erase(callId);
		if (req->isReadyToProcess())
		{
			setToProcess(req);
			processNextRequest();
		}
	}
	else
	{
		PTRACE3("rId=%08X: processReplyFromGenericAsyncCall - request not found, ignore",reqId);
	}
	ticks = SYSTEMTICKS() - ticks;
	PTRACE4("rId=%08X:}AR %08X %05d%s", reqId, callId, ticks, ( ticks > TICKS_PER_SECOND ) ? " overspan" : "");
}

void CALLBACK CommExtFiberServerObject::FiberProc(void * param)
{
	void (*oldTranslator)( unsigned int, _EXCEPTION_POINTERS* );
	oldTranslator = _set_se_translator( PError::_translate );

	CommExtFiberServerObject * obj = (CommExtFiberServerObject*) param;
	PTRACE3("Start new fiber procedure");
	while(1)
	{
		try
		{
			PTRACE4("rId=%08X:FiberProc: start new request", obj->fiberControlBlok.req->getRequestId());
			obj->fiberControlBlok.retCode = obj->fiberControlBlok.req->run();
			obj->switchToMain();
		}
		catch( const PError &er )
		{
			PLog("rId=%08X:FiberProc: error %s", obj->fiberControlBlok.req->getRequestId(), er.why());
			obj->fiberControlBlok.retCode = CommExtFiberRequest::eError;
			obj->switchToMain();
		}
		catch( const exception& exc )
		{
			PLogError( "rId=%08X:FiberProc: error STD exception catch: %s", obj->fiberControlBlok.req->getRequestId(),  exc.what() );
			obj->fiberControlBlok.retCode = CommExtFiberRequest::eError;
			obj->switchToMain();
		}
		catch(...)
		{
			PLog("rId=%08X:FiberProc: error ...", obj->fiberControlBlok.req->getRequestId());
			obj->fiberControlBlok.retCode = CommExtFiberRequest::eError;
			obj->switchToMain();
		}
	}

	PTRACE5("Exiting fiber procedure");
	_set_se_translator( oldTranslator );
}

void CommExtFiberServerObject::switchToMainWait()
{
	PTRACE4("rId=%08X:switchToMainWait",fiberControlBlok.req->getRequestId());
	fiberControlBlok.retCode=CommExtFiberRequest::eWait;
	SwitchToFiber(fiberMain);
}

void CommExtFiberServerObject::switchToMain()
{
	PTRACE4("rId=%08X:switchToMain",fiberControlBlok.req->getRequestId());
	SwitchToFiber(fiberMain);
}

CommExtFiberServerObject::~CommExtFiberServerObject() 
{
	while (fibers.size())
	{
		DeleteFiber(*fibers.begin());
		fibers.pop_front();
	}
}

void CommExtFiberServerObject::waitForRequest(CommExtFiberRequest *req, CommExtFiberRequest *currentRequest)
{
	PTRACE4("rId=%08X:waitForRequest(%08X)",currentRequest->getRequestId(),req->getRequestId());
	req->waitedBy.insert(currentRequest->getRequestId());
	currentRequest->waitingRequests.insert(req->getRequestId());
}

void CommExtFiberServerObject::_releaseWaitingRequests(CommExtFiberRequest *req)
{
	PTRACE4("rId=%08X:_releaseWaitingRequests()",req->getRequestId());
	set<UINT32>::iterator it = req->waitedBy.begin();
	while(it != req->waitedBy.end())
	{
		CommExtFiberRequest *reqToRelease =(CommExtFiberRequest*)lockSystem.getRequestSafe(*it);
		if (reqToRelease)
		{
			PTRACE5("rId=%08X:_releaseWaitingRequests(%08X)",req->getRequestId(),reqToRelease->getRequestId());
			PASSERT3(reqToRelease->waitingRequests.find(req->getRequestId()) != reqToRelease->waitingRequests.end());
			reqToRelease->waitingRequests.erase(req->getRequestId());
			if (reqToRelease->isReadyToProcess())
			{
				setToProcess(reqToRelease);
			}
		}
		++it;
	}
	req->waitedBy.clear();
}

UINT32 CommExtFiberServerObject::_getNewReadyRequests()
{
	deque<UINT32>::iterator it = newRequests.begin();
	if (it != newRequests.end())
	{
		UINT32 reqId = (*it);
		newRequests.pop_front();
		return reqId;
	}
	return 0;
}

INT16 CommExtFiberServerObject::removeRequest(UINT32 reqId, PString& err)
{
	try
	{
		PTRACE4("CommExtFiberServerObject::removeRequest: reqId=%08X", reqId);
		CommExtFiberRequest * req =(CommExtFiberRequest*)lockSystem.getRequest(reqId);
		deque<UINT32>::iterator it;
		for ( it = newRequests.begin(); it != newRequests.end(); ++it)
		{
			if ( reqId == (*it))
			{
				newRequests.erase(it);
				break;
			}
		}
		lockSystem.removeRequest(reqId);
		if (req->fiber)
		{
			DeleteFiber(req->fiber);
			fiberCount--;
		}
		_releaseWaitingRequests(req);
		_releaseObjects(req);
		req->sendReplyWithError(-1,"Request is canceled by Administrator");
		processNextRequest();
		return 0;
	}
	catch( const PError &er )
	{
		err = er.why();
	}
	catch( const exception& exc )
	{
		err =  exc.what();
	}
	catch(...)
	{
		err =  "General exception";
	}
	return -1;
}

void CommExtFiberServerObject::processNextRequest()
{
	__declspec(thread) static int protectorCount = 0;
	struct ReEnterProtector
	{
		ReEnterProtector()
		{
			protectorCount++;
		}
		~ReEnterProtector()
		{
			protectorCount--;
		}
	};

	ReEnterProtector protector;

	if (protectorCount>1)
		return;

	UINT32 reqId = 0;
	try
	{
		while ((reqId = lockSystem.getReadyRequest()) != 0 || 
			   ( (fibers.size() || fiberCount < maximumFibers) && ((reqId = _getNewReadyRequests()) != 0) )
			   )
		{
			CommExtFiberRequest * req =(CommExtFiberRequest*)lockSystem.getRequestSafe(reqId);
			if (!req)
				continue;
			PASSERT3(req->isReadyToProcess());
			void * fiber;
			if (req->fiber)
				fiber = req->fiber;
			else
			{
				if (fibers.size())
				{
					fiber = *fibers.begin();
					fibers.pop_front();
				}
				else
				{
					fiberCount++;
					PTRACE4("rId=%08X:processNextRequest - create new fiber, counter %d", reqId, fiberCount);
					fiber = CreateFiber(0,FiberProc,this);
					PASSERT3( fiber );
				}
				req->fiber = fiber;
				req->initRun();
			}
			fiberControlBlok.req = req;
			fiberControlBlok.reqId = reqId;
			do
			{
				fiberControlBlok.retCode = CommExtFiberRequest::eContinue;
				PTRACE4("rId=%08X:processNextRequest - switch to fiber",reqId);
				stat.switchesToFiber++;
				currentReqId=reqId;
				req->beginProcessing();
				SwitchToFiber(fiber);
				req->endProcessing();
				currentReqId=0;
			}
			while (fiberControlBlok.retCode	== CommExtFiberRequest::eContinue);

			fiberControlBlok.req = 0;
			fiberControlBlok.reqId = 0;

			if(fiberControlBlok.retCode == CommExtFiberRequest::eError ||
			   fiberControlBlok.retCode == CommExtFiberRequest::eFinished)
			{
				CommExtFiberRequest * req =(CommExtFiberRequest*)lockSystem.getRequestSafe(reqId);
				if (req)
				{
					lockSystem.removeRequest(reqId);
					try
					{
						_releaseWaitingRequests(req);
						_releaseObjects(req);
					}
					catch (PError &er)
					{
						PLog("rId=%08X:processNextRequest in main: _releaseObjects exception %s", reqId,er.why());
					}
					catch(  exception& exc  )
					{
						PLog("rId=%08X:processNextRequest in main: _releaseObjects exception %s", reqId,exc.what());
					}
					catch(...)
					{
						PLog("rId=%08X:processNextRequest in main: _releaseObjects general exception", reqId);
					}
					delete req;
				}
				fibers.push_back(fiber);
			}
		}
	}
	catch (PError &er)
	{
		PLog("rId=%08X:processNextRequest in main , error=%s", reqId,er.why());
		CommExtFiberRequest * req =(CommExtFiberRequest*)lockSystem.getRequestSafe(reqId);
		if (req)
		{
			lockSystem.removeRequest(reqId);
			try
			{
				_releaseWaitingRequests(req);
				_releaseObjects(req);
			}
			catch (PError &er)
			{
				PLog("rId=%08X:processNextRequest in main: _releaseObjects exception %s", reqId,er.why());
			}
			catch(  exception& exc  )
			{
				PLog("rId=%08X:processNextRequest in main: _releaseObjects exception %s", reqId,exc.what());
			}
			catch(...)
			{
				PLog("rId=%08X:processNextRequest in main: _releaseObjects general exception", reqId);
			}

			delete req;
		}
	}
	catch(  exception& exc  )
	{
		PLog( "rId=%08X:processNextRequest in main: error STD exception catch: %s", fiberControlBlok.req->getRequestId(),  exc.what() );
		CommExtFiberRequest * req =(CommExtFiberRequest*)lockSystem.getRequestSafe(reqId);
		if (req)
		{
			lockSystem.removeRequest(reqId);
			try
			{
				_releaseWaitingRequests(req);
				_releaseObjects(req);
			}
			catch (PError &er)
			{
				PLog("rId=%08X:processNextRequest in main: _releaseObjects exception %s", reqId,er.why());
			}
			catch(  exception& exc  )
			{
				PLog("rId=%08X:processRequest: _releaseObjects exception %s", reqId,exc.what());
			}
			catch(...)
			{
				PLog("rId=%08X:processNextRequest in main: _releaseObjects general exception", reqId);
			}

			delete req;
		}
	}
	catch(...)
	{
		PLog("rId=%08X:processNextRequest in main, general exception", reqId);
		CommExtFiberRequest * req =(CommExtFiberRequest*)lockSystem.getRequestSafe(reqId);
		if (req)
		{
			lockSystem.removeRequest(reqId);
			try
			{
				_releaseWaitingRequests(req);
				_releaseObjects(req);
			}
			catch (PError &er)
			{
				PLog("rId=%08X:processNextRequest in main: _releaseObjects exception %s", reqId,er.why());
			}
			catch(  exception& exc  )
			{
				PLog("rId=%08X:processNextRequest in main: _releaseObjects exception %s", reqId,exc.what());
			}
			catch(...)
			{
				PLog("rId=%08X:processNextRequest in main: _releaseObjects general exception", reqId);
			}

			delete req;
		}
	}
}

INT16 CommExtFiberServerObject::debugPrintRequest(UINT32 reqId, PString &err)
{
	try
	{
		CommExtFiberRequest * req =(CommExtFiberRequest*)lockSystem.getRequestSafe(reqId);
		if (!req)
		{
			err.assign("Request ").appendHexInt(reqId).append(" not found");
			return -1;
		}
		req->printStat();
		return 0;
	}
	catch(PError &e)
	{
		err = e.why();
		return -1;
	}
}

#endif //PWIN32

