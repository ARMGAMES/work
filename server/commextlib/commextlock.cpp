#include <pplib.h>
#include <commlib.h>

#include "commextlock.h"




CommExtLock::CommExtLock(CommExtLockType& lockType_,const char * string, UINT32 requestId_)
	:lockType(lockType_)
{
	type = lockType.type;
	stringKey = string;
	stringKey.toLower();
	intKey=0;
	requestId = requestId_;
	lockId = 0;
	recurrent = 0;
	PTRACE5("rId=%08X:CommExtLock: Create new lock '%s'",requestId,string);

}
CommExtLock::CommExtLock(CommExtLockType& lockType_,UINT32 key, UINT32 requestId_)
	:lockType(lockType_)
{
	type = lockType.type;
	intKey = key;
	requestId = requestId_;
	lockId = 0;
	recurrent = 0;
	PTRACE5("rId=%08X:CommExtLock: Create new lock '%d'",requestId,key);
}


UINT32 CommExtLockType::find(const char * key)
{
	if (type != eTypeString)
	{
		PString err;
		err.append("Find - String key is requested for CommExtLockType type '").appendInt(type).append("', key='").append(key).append("'");
		throw PError(err);
	}
	PString keyl(key);
	keyl.toLower();
	MapOfStrings::iterator it = mapOfStrings.find(keyl);
	if (it == mapOfStrings.end())
	{
		return 0;
	}
	else
	{
		return (*it).second;
	}
}
UINT32 CommExtLockType::find(UINT32 key)
{
	if (type != eTypeUINT)
	{
		PString err;
		err.append("Find - UINT32 key is requested for CommExtLockType type '").appendInt(type).append("', key='").appendUint(key).append("'");
		throw PError(err);
	}
	MapOfUINT::iterator it = mapOfUINT.find(key);
	if (it == mapOfUINT.end())
	{
		return 0;
	}
	else
	{
		return (*it).second;
	}
}
void CommExtLockType::add(const char * key,UINT32 lockId)
{
	if (type != eTypeString)
	{
		PString err;
		err.append("Add - String key is requested for CommExtLockType type '").appendInt(type).append("', key='").append(key).append("'");
		throw PError(err);
	}
	PString keyl(key);
	keyl.toLower();
	mapOfStrings.insert(MapOfStrings::value_type(keyl,lockId));
	PTRACE5("CommExtLockType: add new lock %08X, key '%s'",lockId,keyl.c_str());
}
void CommExtLockType::add(UINT32 key,UINT32 lockId)
{
	if (type != eTypeUINT)
	{
		PString err;
		err.append("Add - UINT32 key is requested for CommExtLockType type '").appendInt(type).append("', key='").appendUint(key).append("'");
		throw PError(err);
	}
	mapOfUINT.insert(MapOfUINT::value_type(key,lockId));
	PTRACE5("CommExtLockType: add new lock %08X, key '%d'",lockId,key);
}
void CommExtLockType::remove(CommExtLock * lock)
{
	if (type == eTypeUINT)
	{
		PASSERT3(lock->getType() == type);
		mapOfUINT.erase(lock->getIntKey());
		PTRACE5("CommExtLockType: remove lock %08X'",lock->getLockId());
	}
	else if (type == eTypeString)
	{
		PASSERT3(lock->getType() == type);
		MapOfStrings::iterator it = mapOfStrings.find(lock->getStringKey());
		if (it != mapOfStrings.end())
			mapOfStrings.erase(it);
		PTRACE5("CommExtLockType: remove lock %08X'",lock->getLockId());
	}
	else
	{
		PASSERT3(0);
	}
}




bool CommExtLocking::addNewLockType(const char * type, bool isString)
{
	LockTypes::iterator it = lockTypes.find(type);
	if (it != lockTypes.end())
	{
		PTRACE3("CommExtLocking::Lock type '%s' already exists",type);
		return false;
	}
	lockTypes.insert( LockTypes::value_type(type,new CommExtLockType(isString) ));
	return true;
}

CommExtLock * CommExtLocking::getLock(UINT32 lockId)
{
	CommExtLock ** lock = globalLocksMap.find(lockId);
	if (!lock)
	{
		PTRACE3("CommExtLocking::Lock Id %08X not found in global map",lockId);
		throw PError("LockId not found");
	}
	if (!*lock)
	{
		PTRACE3("CommExtLocking::Lock Id %08X found in global map, but it set to NULL",lockId);
		throw PError("Null lock pointer");
	}
	if ((*lock)->getLockId() != lockId)
	{
		PTRACE3("CommExtLocking::Lock Id %08X not equal to lockId %08X in lock structure",lockId,(*lock)->getLockId());
		throw PError("Different lockIds");
	}
	return (*lock);
}

CommExtLockRequest * CommExtLocking::getRequest(UINT32 reqId)
{
	CommExtLockRequest ** req = globalRequestsMap.find(reqId);
	if (!req)
	{
		PTRACE3("CommExtLocking::Request Id %08X not found in global map",reqId);
		throw PError("ReqId not found");
	}
	if (!*req)
	{
		PTRACE3("CommExtLocking::Request Id %08X found in global map, but it set to NULL",reqId);
		throw PError("Null req pointer");
	}
	if ((*req)->getRequestId() != reqId)
	{
		PTRACE3("CommExtLocking::Request Id %08X not equal to requestId %08X in Request structure",reqId,(*req)->getRequestId());
		throw PError("Different requestIds");
	}

	return (*req);
}
CommExtLockRequest * CommExtLocking::getRequestSafe(UINT32 reqId)
{
	try
	{
		CommExtLockRequest * r = getRequest(reqId);
		return r;
	}
	catch(PError &/*er*/)
	{
		return 0;
	}
}


bool CommExtLocking::setLock(const char * type, const char * stringKey, UINT32 requestId,UINT32 & retLockId)
{
	PTRACE5("rId=%08X:CommExtLocking: setting lock type '%s', key '%s'",requestId,type,stringKey);
	retLockId = 0;
	CommExtLockRequest * req = getRequest(requestId);
	PASSERT3(!req->getWaitingLock());
	LockTypes::iterator it = lockTypes.find(type);
	if (it == lockTypes.end())
	{
		addNewLockType(type,true);
		it = lockTypes.find(type);
		if (it == lockTypes.end())
		{
			PString err;
			err.append("Lock type '").append(type).append("' not found");
			throw PError(err);
		}
	}
	CommExtLockType *lockType = (*it).second;
	UINT32 lockId = lockType->find(stringKey);
	if (lockId)
	{
		PTRACE5("rId=%08X:CommExtLocking: setting lock - resource locked by lockId %08X",requestId,lockId);
		if (recurrentSupport)
		{
			CommExtLock * lock = getLock(lockId);
			UINT32 reqId = lock->getReqId();
			PASSERT3(reqId);
			if (reqId == requestId)
			{
				lock->increaseRec();
				retLockId = lockId;
				return true;
			}
		}
		if (_setWait(lockId,req))
		{
			retLockId = lockId;
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		CommExtLock *lock = new CommExtLock(*lockType,stringKey,requestId);
		if (recurrentSupport)
			lock->increaseRec();
		lock->setLockId(globalLocksMap.add(lock));
		lockType->add(lock->getStringKey(),lock->getLockId());
		req->addLock(lock->getLockId());
		retLockId = lock->getLockId();
		PTRACE5("rId=%08X:CommExtLocking: new lock is set, lockid %08X",requestId,lock->getLockId());
		return true;
	}
}

bool CommExtLocking::setLock(const char * type, UINT32 intKey, UINT32 requestId, UINT32 & retLockId)
{
	PTRACE5("rId=%08X:CommExtLocking: setting lock type '%s', key '%d'",requestId,type,intKey);
	retLockId=0;
	CommExtLockRequest * req = getRequest(requestId);
	PASSERT3(!req->getWaitingLock());
	LockTypes::iterator it = lockTypes.find(type);
	if (it == lockTypes.end())
	{
		addNewLockType(type,false);
		it = lockTypes.find(type);
		if (it == lockTypes.end())
		{
			PString err;
			err.append("Lock type '").append(type).append("' not found");
			throw PError(err);
		}
	}
	CommExtLockType *lockType = (*it).second;
	UINT32 lockId = lockType->find(intKey);
	if (lockId)
	{
		PTRACE5("rId=%08X:CommExtLocking: setting lock - resource locked by lockId %08X",requestId,lockId);
		if (recurrentSupport)
		{
			CommExtLock * lock = getLock(lockId);
			UINT32 reqId = lock->getReqId();
			PASSERT3(reqId);
			if (reqId == requestId)
			{
				lock->increaseRec();
				retLockId = lockId;
				return true;
			}
		}
		if (_setWait(lockId,req))
		{
			retLockId = lockId;
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		CommExtLock *lock = new CommExtLock(*lockType,intKey,requestId);
		if (recurrentSupport)
			lock->increaseRec();
		lock->setLockId(globalLocksMap.add(lock));
		lockType->add(intKey,lock->getLockId());
		req->addLock(lock->getLockId());
		retLockId = lock->getLockId();
		PTRACE5("rId=%08X:CommExtLocking: new lock is set, lockid %08X",requestId,lock->getLockId());
		return true;
	}
}


void CommExtLocking::inheritLock(UINT32 requestIdTo, UINT32 requestIdFrom, UINT32 lockId)
{
	PTRACE5("CommExtLocking: inheriting lockId %08X from  requestID=%08X to requestID=%08X",lockId, requestIdFrom, requestIdTo);
	CommExtLockRequest * reqTo = getRequest(requestIdTo);
	CommExtLockRequest * reqFrom = getRequest(requestIdFrom);
	PASSERT3(!reqTo->getWaitingLock());
	PASSERT3(!reqFrom->getWaitingLock());

	CommExtLock * lock = getLock(lockId);
	UINT32 reqId = lock->getReqId();
	PASSERT3(reqId);
	PASSERT3(reqId == requestIdFrom);
	reqFrom->removeLock(lockId);
	lock->changeRequestId(requestIdTo);
	reqTo->addLock(lock->getLockId());
}

UINT32 CommExtLocking::_checkDeadLock(UINT32 reqId,CommExtLock * lock)
{
	UINT32 lockingReqId = lock->getReqId();
	PASSERT3(lockingReqId);
	if (lockingReqId == reqId)
	{
		// deadlock  found!
		PTRACE5("CommExtLocking: deadlock found!, reqId=%08X, lock %08X",reqId,lock->getLockId());
		return lock->getLockId();
	}
	CommExtLockRequest * lockingReq = getRequest(lockingReqId);
	UINT32 waitingLockId;
	if (0!=(waitingLockId = lockingReq->getWaitingLock()))
	{
		CommExtLock * l = getLock(waitingLockId);
		return _checkDeadLock(reqId,l);
	}
	return 0;
}


bool CommExtLocking::_setWait(UINT32 lockId,CommExtLockRequest * req)
{
	CommExtLock * lock = getLock(lockId);
	UINT32 reqId = lock->getReqId();
	PASSERT3(reqId);

	// check if we are not asking for locl we alredy have

	if (reqId == req->getRequestId())
		return true;

	CommExtLockRequest * lockingReq = getRequest(reqId);
	UINT32 waitingLockId=0;
	if (req->getLocks().size() && (0!=(waitingLockId = lockingReq->getWaitingLock())))
	{
		CommExtLock * l = getLock(waitingLockId);
		waitingLockId = _checkDeadLock(req->getRequestId(),l);
	}
	if (waitingLockId)
	{
// version 1
//		removeLock(waitingLockId);
//		req->removeLock(waitingLockId);
//		req->lockTaken(waitingLockId);

// version 2

		lockingReq->removeLock(lockId);
		lockingReq->lockTaken(lockId);
		req->addLock(lockId);
		lock->changeRequestId(req->getRequestId());
		return true;

// version 3
/*
		UINT32 locks;
		while(req->getLocks().size())
		{
			locks = req->getLocks()[0];
			removeLock(locks);
			req->removeLock(locks);
			req->lockTaken(locks);
		}
		readyRequests.push_back(req->getRequestId());
		return false;
*/
	}
	lock->addWaitingRequest(req->getRequestId());
	req->setWaitingLock(lockId);
	waitingRequests.insert(req->getRequestId());
	return false;
}
/*
void CommExtLocking::removeLock(UINT32 lockId)
{
	PTRACE5("CommExtLocking: removing lock %x", lockId);
	CommExtLock * lock = getLock(lockId);
	for (int i=0;i<lock->getWaitingRequests().size();i++)
	{
		UINT32 reqId = lock->getWaitingRequests()[i];
		CommExtLockRequest * req = getRequest(reqId);
		PASSERT3(lockId == req->getWaitingLock());
		req->setWaitingLock(0);
		waitingRequests.erase(reqId);
		readyRequests.push_back(reqId);
		PTRACE5("CommExtLocking: removing lock %x, request %x released", lockId,reqId);
	}
	lock->getLockType().remove(lock);
	globalLocksMap.remove(lockId);
	delete lock;
}
*/
void CommExtLocking::removeLock(UINT32 lockId,bool ignoreRec)
{
	PTRACE5("CommExtLocking: removing lock %08X", lockId);
	CommExtLock * lock = getLock(lockId);
	if (recurrentSupport && !ignoreRec)
	{
		if (lock->decreaseRec()==false)
			return;
	}

	UINT32 reqId = lock->getReqId();
	CommExtLockRequest * req = getRequest(reqId);
	req->removeLock(lockId);
	if (lock->getWaitingRequests().size())
	{

		UINT32 reqId = lock->getWaitingRequests()[0];
		CommExtLockRequest * req = getRequest(reqId);
		PASSERT3(lockId == req->getWaitingLock());

		req->setWaitingLock(0);
		req->addLock(lockId);
		lock->changeRequestId(reqId);
		req->lockGotten(lockId);

		lock->removeWaitingRequest(reqId);
		waitingRequests.erase(reqId);
		readyRequests.push_back(reqId);
		PTRACE5("CommExtLocking: removing lock %08X, request %08X released", lockId,reqId);
		return;
	}
	lock->getLockType().remove(lock);
	globalLocksMap.remove(lockId);
	delete lock;
}

void CommExtLocking::removeRequest(UINT32 reqId)
{
	PTRACE5("rId=%08X:CommExtLocking: removing request", reqId);
	CommExtLockRequest * req = getRequest(reqId);

	UINT32 waitingLockId;
	if (0!=(waitingLockId = req->getWaitingLock()))
	{
		CommExtLock * lock = getLock(waitingLockId);
		PASSERT3(lock->getReqId() != reqId);
		bool ok = lock->removeWaitingRequest(reqId);
		if (!ok)
		{
			PTRACE3(" CommExtLocking::removeRequest reqId %08X not found in lock %08X",reqId,waitingLockId);
		}
	}
	ReadyRequests::iterator it = readyRequests.begin();
	while (it != readyRequests.end())
	{
		UINT32 reqId1 = (*it);
		if (reqId1 == reqId)
		{
			readyRequests.erase(it);
			PTRACE5("rId=%08X:CommExtLocking: removeRequest, request removed from ready Queue", reqId);
			break;
		}
		++it;
	}
	waitingRequests.erase(reqId);
	while (req->getLocks().size())
	{
		UINT32 lockId = req->getLocks()[0];
		removeLock(lockId,true);
	}
	globalRequestsMap.remove(reqId);
}

void CommExtLockRequest::composeStat(CommMsgBody &reply) const
{
	reply.composeUINT32(requestId)
		 .composeUINT32(waitingLock)
		 .composeUINT32(havingLocks.size());
	for (int i=0;i<havingLocks.size();i++)
		reply.composeUINT32(havingLocks[i]);
}

void CommExtLockRequest::printStat() 
{
	PLog("###CommExtLockRequest: requestId=%08X, waitingLock=%08X, havingLocks=%d",requestId,waitingLock,havingLocks.size());
	for (int i=0;i<havingLocks.size();i++)
	{
		PLog("###CommExtLockRequest: lock=%08X",havingLocks[i]);
	}
}

void CommExtLock::printStat() 
{
	PLog("###CommExtLock: type=%d, intKey=%x, stringKey=%s, lockId=%x, requestId=%x, waiting=%d, recurrent=%d",type,intKey, stringKey.c_str(),lockId, requestId, waitingRequests.size(),recurrent);
	for (int i=0;i<waitingRequests.size();i++)
	{
		PLog("###CommExtLock: waiting=%08X",waitingRequests[i]);
	}
}

void CommExtLock::composeStat(CommMsgBody & reply) const
{
	reply.composeINT32(type)
		 .composeUINT32(intKey)
		 .composeString(stringKey)
		 .composeUINT32(lockId)
		 .composeUINT32(requestId)
		 .composeUINT32(waitingRequests.size());
	for (int i=0;i<waitingRequests.size();i++)
	{
		reply.composeUINT32(waitingRequests[i]);
	}
}

void CommExtLocking::printStat(int level) 
{
	PLog("###CommExtLocking: requests %d, ready %d, waiting %d", globalRequestsMap.size(),readyRequests.size(), waitingRequests.size());
	if (level > 0)
	{
		if (readyRequests.size())
		{
			PString ps;
			for (int i=0; i <readyRequests.size();i++)
			{
				if (!(i%10))
				{
					if (ps.length())
						PLog("%s", ps.c_str());
					ps="####Ready requests:";
				}
				ps.appendHexInt(readyRequests[i]).append(",");
			}
			PLog("%s", ps.c_str());
		}
		else
		{
			PLog("####No ready requests");
		}

		if (waitingRequests.size())
		{
			PString ps;
			WaitingRequests::iterator it;
			int i;
			for (i=0,it=waitingRequests.begin(); it != waitingRequests.end(); ++it,i++)
			{
				if (!(i%10))
				{
					if (ps.length())
						PLog("%s", ps.c_str());
					ps="####Waiting requests:";
				}
				ps.appendHexInt((*it)).append(",");
			}
			PLog("%s", ps.c_str());
		}
		else
		{
			PLog("####No waiting requests");
		}

		;
		PLog("####All requests:");

		int i;
		for (CommExtLockRequest ** reqP = globalRequestsMap.firstIndex(i); reqP != 0;  reqP = globalRequestsMap.nextIndex(i))
		{
			(*reqP)->printStat();
		}
	}
}

