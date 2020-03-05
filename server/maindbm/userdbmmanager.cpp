
#include "MainDbmmanager.h"

/////////////////////////////////////////////////////////////////////////////////////

INT16 MainDbmManager::insertUser(User& user, PString& sqlErr)
{
	PLog(__FUNCTION__ "-uid=%s", user.userId.c_str());
	try
	{
		setAutoCommit(false);
		user.userIntId = getNextId(DB_TABLE_USERS);

		insertUserStmt->exec(user);

		commit();
		setAutoCommit(true);

		return DBM_NO_ERROR;
	}
	catch (const PSqlError& er)
	{
		++errorsInMessages;
		rollbackOnError();
		setAutoCommit(true);
		sqlErr = er.why();
		return er.code();
	}

}

INT16 MainDbmManager::getUsers(const vector<PString>& userIds, CommMsgBody& body, PString& sqlErr)
{
	PLog(__FUNCTION__ );

	try
	{
		body.composeUINT32(userIds.size());

		for (auto userId : userIds)
		{
			User* user = findCachedUser(userId);
			if (!user)
			{
				sqlErr.assign("Can not find userId-").append(userId);
				return NO_SUCH_USER;
			}
			user->compose(body);
		}

		return DBM_NO_ERROR;

	}
	catch (const PSqlError& er)
	{
		++errorsInMessages;
		sqlErr = er.why();
		return er.code();
	}
}

User* MainDbmManager::findCachedUser(const char* userId)
{
	User* cachedUser = cachedUsers.getCachedUser(userId);

	if (cachedUser)
	{
		return cachedUser;
	}

	User user;
	bool found = getUserByUserIdStmt->execGet(userId, user);

	if (found)
	{
		cachedUser = cachedUsers.insertUser(user);
	}
	
	return cachedUser;
}