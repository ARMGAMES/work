
#pragma once

#include "maindbmstruct.h"

class CachedUsers
{
private:
	typedef PStringMap<User> UsersMap;
	UsersMap container;
	map<UINT32, PString> userIdMap;
	UINT32 maxSize;

	// prohibited
	CachedUsers(const CachedUsers& other);
	CachedUsers& operator=(const CachedUsers& other);

public:
	CachedUsers();

	void setMaxSize(UINT32 num) { maxSize = num; }
	User* insertUser(User& user);
	User* getCachedUser(const char* userId);

	void cull(UINT32 secondsLimit);
};

