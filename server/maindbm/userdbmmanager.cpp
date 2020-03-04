
#include "MainDbmmanager.h"

/////////////////////////////////////////////////////////////////////////////////////

INT16 MainDbmManager::insertUser(User& user, PString& sqlErr)
{
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