
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
	cachedUsersMaxSize = CACHEDUSERS_MAXSIZE;
	cachedUsersCutoffSeconds = CACHEDUSERS_CUTOFFSECONDS;

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

void MainDbmManager::dynamicInit(const PIniFile& iniFile)
{
	PLog(__FUNCTION__);

	const PIniFile::Section* pSection = iniFile.getSection("DBSETTINGS");
	if (pSection)
	{
		cachedUsersMaxSize = pSection->getIntProperty("CachedUsersMaxSize", CACHEDUSERS_MAXSIZE);
		cachedUsersCutoffSeconds = pSection->getIntProperty("CachedUsersCutoffSeconds", CACHEDUSERS_CUTOFFSECONDS);
	}
	
	PLog("-cachedUsersMaxSize=%u", cachedUsersMaxSize);
	PLog("-cachedUsersCutoffSeconds=%u", cachedUsersCutoffSeconds);
	cachedUsers.setMaxSize(cachedUsersMaxSize);

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
	getUserByUserIdStmt = new GetUserByUserIdStmt(*this);

}

void MainDbmManager::deleteStatements()
{
	delete insertUserStmt;
	delete getUserByUserIdStmt;

	zeroStatements();
}

void MainDbmManager::zeroStatements()
{
	insertUserStmt = 0;
	getUserByUserIdStmt = 0;

}

void MainDbmManager::sweepCaches()
{
	cachedUsers.cull(cachedUsersCutoffSeconds);
}

////////////////////////////////////////////////////////////////////////////////////////////////////







