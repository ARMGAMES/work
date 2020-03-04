
#pragma once

#include "maindbmstruct.h"

class CachedUsers
{
private:
	typedef PStringMap<User> UsersMap;
	UsersMap container;
	map<UINT32, PString> userIdMap;

	// prohibited
	CachedUsers(const CachedUsers& other);
	CachedUsers& operator=(const CachedUsers& other);

public:
	CachedUsers();

	void insertUser(User& user);
	const User* getCachedUser(const char* userId);

	void cull(UINT32 secondsLimit, UINT32 maxUserNumber);
};

