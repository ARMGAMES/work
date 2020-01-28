#ifndef commextlock_h_included
#define commextlock_h_included

class CommExtLockType;

class CommExtLockRequest
{
	UINT32 requestId;
	vector<UINT32> havingLocks;
	UINT32 waitingLock; 
public:
	virtual void printStat();
	virtual void composeStat(CommMsgBody &reply) const;
	CommExtLockRequest()
	{
		requestId=0;
		waitingLock=0;
	}
	virtual ~CommExtLockRequest(){}
	void addLock(UINT32 lockId)
	{
		PTRACE5("CommExtLockRequest:Add lock %08X to request %08X",lockId,requestId);
#if PTRACELVL >= 4
		for (int i=0;i<havingLocks.size();i++)
		{
			if (havingLocks[i] == lockId)
			{
				PTRACE5("CommExtLockRequest: Lockid %08X already in vector!!",lockId);
				PASSERT3(0);
			}
		}
#endif
		havingLocks.push_back(lockId);
	}

	bool removeLock(UINT32 lockId)
	{
		PTRACE5("CommExtLockRequest: removing lock %08X from %08X",lockId,requestId);
		bool found=false;
		vector<UINT32>::iterator it = havingLocks.begin();
		while(it != havingLocks.end())
		{
			if ((*it) == lockId)
			{
				if (found)
				{
					PTRACE3("!CommExtLockRequest: removing lock %08X from request %08X, double appearance",lockId,requestId);
					PASSERT3(0);
				}
				PTRACE5("CommExtLockRequest: removed lock %08X from request %08X",lockId,requestId);
				it=havingLocks.erase(it);
				found=true;
			}
			else
			{
				++it;
			}
		}
		if (found == false)
		{
			PTRACE3("CommExtLockRequest: lock %08X in request %08X not found",lockId,requestId);
			PASSERT3(0);
		}
		return found;
	}

	UINT32 getWaitingLock() const
	{
		return waitingLock;
	}
	void setRequestId(UINT32 reqId)
	{
		PASSERT3(!requestId);
		requestId=reqId;
	}
	void setWaitingLock(UINT32 lockId_)
	{
		PTRACE5("CommExtLockRequest:Set waiting lock %08X to request %08X",lockId_,requestId);
		PASSERT3(lockId_? !waitingLock:waitingLock);
		waitingLock=lockId_;
	}
	UINT32 getRequestId() const
	{
//		PASSERT3(requestId);
		return requestId;
	}

	const vector<UINT32> & getLocks() const
	{
		return havingLocks;
	}
	virtual void lockTaken(UINT32 lockId){};
	virtual void lockGotten(UINT32 lockId){};
};


class CommExtLock
{
	int type;
	UINT32  intKey;
	PString stringKey;
	UINT32		   lockId;
	CommExtLockType & lockType;
	UINT32         requestId;
	vector<UINT32> waitingRequests;
	UINT32 recurrent;
	CommExtLock(); //prohibited
	CommExtLock (const CommExtLock  & other); // prohibited
	CommExtLock & operator = (const CommExtLock  & other); // prohibited


public:
	void printStat();
	void composeStat(CommMsgBody & reply) const;
	CommExtLock(CommExtLockType& lockType_,const char * string, UINT32 requestId_);
	CommExtLock(CommExtLockType& lockType_,UINT32 key, UINT32 requestId_);
	void changeRequestId(UINT32 requestId_)
	{
		PASSERT3(requestId);
		requestId = requestId_;
	}
	const char * getStringKey() const
	{
		return stringKey.c_str();
	}
	UINT32 getIntKey() const
	{
		return intKey;
	}

	UINT32 getLockId() const
	{
		PASSERT3(lockId);
		return lockId;
	}
	void setLockId(UINT32 lockId_)
	{
		PASSERT3(!lockId);
		lockId = lockId_;
	}
	UINT32 getReqId() const
	{
		PASSERT3(requestId);
		return requestId;
	}
	void increaseRec()
	{
		recurrent++;
	}
	bool decreaseRec()
	{
		return --recurrent == 0;
	}

	void addWaitingRequest(UINT32 reqId)
	{
		PTRACE5("CommExtLock: Add waiting request  %x to lock %x",reqId,lockId);
#if PTRACELVL >= 4
		for (int i=0;i<waitingRequests.size();i++)
		{
			if (waitingRequests[i] == reqId)
			{
				PTRACE5("CommExtLock: reqId %x already in vector!!",reqId);
				PASSERT3(0);
			}
		}
#endif

		waitingRequests.push_back(reqId);
	}
	bool removeWaitingRequest(UINT32 reqId)
	{
		PTRACE5("CommExtLock: removing waiting request  %x to lock %x",reqId,lockId);
		bool found=false;
		vector<UINT32>::iterator it = waitingRequests.begin();
		while(it != waitingRequests.end())
		{
			if ((*it) == reqId)
			{
				if (found)
				{
					PTRACE3("!CommExtLock: removing waiting request  %x to lock %x, double appearance",reqId,lockId);
					PASSERT3(0);
				}
				PTRACE5("CommExtLock: removed waiting request %x to lock %x",reqId,lockId);
				it=waitingRequests.erase(it);
				found=true;
			}
			else
			{
				++it;
			}
		}
		if (found == false)
		{
			PTRACE5("CommExtLock: waiting request  %x in lock %x not found",reqId,lockId);
			PASSERT3(0);
		}

		return found;
	}

	const vector<UINT32>& getWaitingRequests() const
	{
		return waitingRequests;
	}
	CommExtLockType & getLockType()
	{
		return lockType;
	}
	int getType() const
	{
		return type;
	}




};

class CommExtLockType
{
	enum {eTypeString=1,eTypeUINT=2};
	int type;
	typedef PStringMap<UINT32> MapOfStrings;
	typedef map<UINT32,UINT32> MapOfUINT;
	MapOfStrings mapOfStrings;
	MapOfUINT	 mapOfUINT;

public:	
	CommExtLockType(bool isString)
	{
		if (isString)
			type = eTypeString;
		else
			type = eTypeUINT;
	}
	UINT32 find(const char * key);
	UINT32 find(UINT32 key);
	void add(const char * key,UINT32 lockId);
	void add(UINT32 key,UINT32 lockId);
	void remove(CommExtLock * lock);
	friend class CommExtLock;
};


class CommExtLocking
{
	typedef CommIdMap<CommExtLock * > GlobalLocks;
	typedef CommIdMap<CommExtLockRequest *> GlobalRequests;
	typedef PStringMap<CommExtLockType * > LockTypes;
	typedef deque<UINT32> ReadyRequests;
	typedef set<UINT32> WaitingRequests;

	GlobalLocks			globalLocksMap;
	GlobalRequests		globalRequestsMap;
	LockTypes			lockTypes;
	ReadyRequests		readyRequests;
	WaitingRequests		waitingRequests;
	bool				recurrentSupport;

	UINT32 _checkDeadLock(UINT32 reqId,CommExtLock * lock);
	bool _setWait(UINT32 lockId,CommExtLockRequest * req);


public:
	CommExtLocking(bool recurrentSupport_ = false)
	{
		recurrentSupport = recurrentSupport_;
	}
	void printStat(int level) ;
	void composeStat(CommMsgBody & reply) const;
	bool addNewLockType(const char * type, bool isString);
	CommExtLock * getLock(UINT32 lockId);
	CommExtLockRequest * getRequest(UINT32 reqId);
	CommExtLockRequest * getRequestSafe(UINT32 reqId);
	bool setLock(const char * type, const char * stringKey, UINT32 requestId, UINT32 & lockId);
	bool setLock(const char * type, UINT32 intKey, UINT32 requestId,UINT32 & lockId);
	void inheritLock(UINT32 requestIdTo, UINT32 requestIdFrom, UINT32 lockId);
	void removeLock(UINT32 lockId, bool ignoreRec = false);
	UINT32 addRequest(CommExtLockRequest *req)
	{
		req->setRequestId(globalRequestsMap.add(req));
		PTRACE5("CommExtLocking: Add request %x to global map",req->getRequestId());
		return req->getRequestId();
	}
	void removeRequest(UINT32 reqId);
	UINT32 getReadyRequest()
	{
		ReadyRequests::iterator it = readyRequests.begin();
		if (it != readyRequests.end())
		{
			UINT32 reqId = (*it);
			readyRequests.pop_front();
			return reqId;
		}
		return 0;
	}
	void addReadyRequest(UINT32 reqId)
	{
		readyRequests.push_back(reqId);
	}
	UINT32 getReadyRequestSize() const
	{
		return readyRequests.size();
	}
	UINT32 getWaitingRequestsSize() const
	{
		return waitingRequests.size();
	}
	UINT32 getGlobalRequestSize() const
	{
		return globalRequestsMap.size();
	}

};



#endif

