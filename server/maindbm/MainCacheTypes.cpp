
#include "MainCacheTypes.h"

CachedUsers::CachedUsers()
{

}

void CachedUsers::insertUser(User& user)
{
	SrvTime now;
	now.currentLocalTime();

	if (container.find(user.userId) != container.end())
	{
		container.erase(user.userId);
	}

	user.lastAccessed = now.encode();
	container.insert(make_pair(user.userId, user));

	PLog("[+user]-%s", user.userId);
	return;
}

const User* CachedUsers::getCachedUser(const char* userId)
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

void CachedUsers::cull(UINT32 secondsLimit, UINT32 maxUserNumber)
{
	PLog(__FUNCTION__ "user size=%u", (UINT32)container.size());


}