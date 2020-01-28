#ifndef CommExtFiberRequest_h_included
#define CommExtFiberRequest_h_included

#include "commextlock.h"

class CommExtFiberServerObject;

class CommExtFiberRequest : public CommExtLockRequest
{
protected:
	void * fiber;
	set<UINT32> waitingAsynCall;
	set<UINT32> waitingRequests;
	set<UINT32> waitedBy;

	virtual bool isReadyToProcess() const
	{
		return waitingAsynCall.size() == 0 && waitingRequests.size() == 0;
	}



private:
	CommExtFiberServerObject & obj;

public:

	enum
	{
		eContinue,
		eWait,
		eFinished,
		eError
	};


	CommExtFiberRequest(CommExtFiberServerObject &obj_) :
		obj(obj_)
	{
		fiber = 0;
	}
	virtual void initRun(){}
	virtual void beginProcessing() {}
	virtual void endProcessing() {}
	virtual int run() = 0;
	virtual void sendReplyWithError(INT16 errCode, const char * errMsg) = 0;

friend class CommExtFiberServerObject;
};


#endif