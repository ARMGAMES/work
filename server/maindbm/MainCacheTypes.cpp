
#include "MainCacheTypes.h"

CachedUsers::CachedUsers()
	: maxSize(0)
{
}

User* CachedUsers::insertUser(User& user)
{
	if (container.size() > 0 && container.size() >= maxSize)
	{
		const char* erasedUserId = container.begin()->first;
		UINT32 userIntId = container.begin()->second.userIntId;
		PLog("[-USER]-%s", erasedUserId);

		userIdMap.erase(userIntId);
		container.erase(container.begin());
	}

	SrvTime now;
	now.currentLocalTime();

	if (container.find(user.userId) != container.end())
	{
		container.erase(user.userId);
	}

	user.lastAccessed = now.encode();

	std::pair<UsersMap::iterator, bool> ret;
	ret = container.insert(make_pair(user.userId, user));
	
	if (ret.second)
	{
		PLog("[+user]-%s", user.userId.c_str());
		userIdMap.insert(make_pair(ret.first->second.userIntId, ret.first->second.userId));
		return &(ret.first->second);
	}
	
	return nullptr;
}

User* CachedUsers::getCachedUser(const char* userId)
{
	if (!userId || !*userId)
		return nullptr;

	UsersMap::iterator it = container.find(userId);
	if (it == container.end())
	{
		return nullptr;
	}

	SrvTime now;
	now.currentLocalTime();
	it->second.lastAccessed = now.encode();

	return &(it->second);
}

void CachedUsers::cull(UINT32 secondsLimit)
{
	PLog(__FUNCTION__ "user size=%u", (UINT32)container.size());

	SrvTime now;
	now.currentLocalTime();
	time_t cutoffTime = now.encode() - secondsLimit;

	UsersMap::iterator userIt = container.begin();
	while(userIt != container.end())
	{
		UsersMap::iterator removeIt = userIt++;
		const User& user = removeIt->second;
		if (user.lastAccessed < cutoffTime)
		{
			PTRACE5("[-USER]-%s", user.userId.c_str());
			userIdMap.erase(user.userIntId);
			container.erase(removeIt);
		}
	}
}