
#include "ppinifile.h"
#include "MainDbmmanager.h"
#include "pperror.h"
#include "pplogfile.h"
#include "MainDbmobject.h"
#include "commonmacros.h"

MainDbmManager::MainDbmManager()
{
	errorsInMessages = 0;
	dbReconnectInterval = 0;
	zeroStatements();
}

MainDbmManager::~MainDbmManager()
{ 
	try //for pclint as in "dbm\dbmanager.h"
	{
		deleteStatements();
	}
	catch(...)
	{
	}
}

bool MainDbmManager::openDataStorage(const char* fullFileName, const char* sectionName)
{
	PLog(__FUNCTION__" start");
	deleteStatements();

	bool bResult = DatabaseManagerCommon::openDataStorage(fullFileName, sectionName);
	if (!bResult)
		return false;

	prepareStatements();

	getNextId64("testTB");
	PLog("MainDbmManager::openDataStorage end");
	return bResult;
}

// reporting db manager specific parameters to Monitor
void MainDbmManager::composeHeLoggerParameters( HeLoggerParameters& parameters ) const
{
	PString strValue;

	strValue.assign("").appendUint(errorsInMessages);
	HeLoggerParameter errInMessages("errorsInMessages", strValue);

	parameters.push_back(errInMessages);
}

//
// MainDbmManager initialization
//

void MainDbmManager::init(MainDbmServerObject* obj)
{
	dbmObj = obj;
}

void MainDbmManager::rollbackOnError()
{
	try
	{
		rollback();
	}
	catch( const PSqlError& er )
	{
		PLog("rollbackOnError ... %s", er.why());
		throw PDbDisconnectError( er.why() );
	}
}

void MainDbmManager::setAutoCommitTrueOnError()
{
	try
	{
		setAutoCommit(true);
	}
	catch (const PSqlError& er)
	{
		PLog("setAutoCommitTrueOnError ... %s", er.why());
		throw PDbDisconnectError(er.why());
	}
}

void MainDbmManager::setAutoCommitTrue()
{
	setAutoCommit(true);
}

/* Prepared statement management */

void MainDbmManager::prepareStatements()
{   
	insertUserStmt = new InsertUserStmt(*this);


}

void MainDbmManager::deleteStatements()
{
	delete insertUserStmt;

	zeroStatements();
}

void MainDbmManager::zeroStatements()
{
	insertUserStmt = 0;
}

/* Message processing */


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

////////////////////////////////////////////////////////////////////////////////////////////////////
