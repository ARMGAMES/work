
#include "ppinifile.h"
#include "LoginDbmmanager.h"
#include "pperror.h"
#include "pplogfile.h"
#include "LoginDbmobject.h"
#include "commonmacros.h"

LoginDbmManager::LoginDbmManager()
{
	errorsInMessages = 0;
	dbReconnectInterval = 0;
	zeroStatements();
}

LoginDbmManager::~LoginDbmManager()
{ 
	try //for pclint as in "dbm\dbmanager.h"
	{
		deleteStatements();
	}
	catch(...)
	{
	}
}

bool LoginDbmManager::openDataStorage(const char* fullFileName, const char* sectionName)
{
	PLog(__FUNCTION__" start");
	deleteStatements();

	bool bResult = DatabaseManagerCommon::openDataStorage(fullFileName, sectionName);
	if (!bResult)
		return false;

	prepareStatements();

	getNextId64("testTB");
	PLog("LoginDbmManager::openDataStorage end");
	return bResult;
}

// reporting db manager specific parameters to Monitor
void LoginDbmManager::composeHeLoggerParameters( HeLoggerParameters& parameters ) const
{
	PString strValue;

	strValue.assign("").appendUint(errorsInMessages);
	HeLoggerParameter errInMessages("errorsInMessages", strValue);

	parameters.push_back(errInMessages);
}

//
// LoginDbmManager initialization
//

void LoginDbmManager::init(LoginDbmServerObject* obj)
{
	dbmObj = obj;
}

void LoginDbmManager::rollbackOnError()
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

void LoginDbmManager::setAutoCommitTrueOnError()
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

void LoginDbmManager::setAutoCommitTrue()
{
	setAutoCommit(true);
}

/* Prepared statement management */

void LoginDbmManager::prepareStatements()
{   
	insertLoginStmt = new InsertLoginStmt(*this);
	getLoginByLoginIdStmt = new GetLoginByLoginIdStmt(*this);
	insertLogoutStmt = new InsertLogoutStmt(*this);
	getLogoutByLoginIdStmt = new GetLogoutByLoginIdStmt(*this);
}

void LoginDbmManager::deleteStatements()
{
	delete insertLoginStmt;
	delete getLoginByLoginIdStmt;
	delete insertLogoutStmt;
	delete getLogoutByLoginIdStmt;

	zeroStatements();
}

void LoginDbmManager::zeroStatements()
{
	insertLoginStmt = 0;
	getLoginByLoginIdStmt = 0;
	insertLogoutStmt = 0;
	getLogoutByLoginIdStmt = 0;
}

/* Message processing */


////////////////////////////////////////////////////////////////////////////////////////////////////
