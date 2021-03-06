
#include "ppinifile.h"
#include "ColTCADbmmanager.h"
#include "pperror.h"
#include "pplogfile.h"
#include "ColTCADbmobject.h"
#include "commonmacros.h"

ColTCADbmManager::ColTCADbmManager()
{
	errorsInMessages = 0;
	dbReconnectInterval = 0;
	zeroStatements();
}

ColTCADbmManager::~ColTCADbmManager()
{ 
	try //for pclint as in "dbm\dbmanager.h"
	{
		deleteStatements();
	}
	catch(...)
	{
	}
}

bool ColTCADbmManager::openDataStorage(const char* fullFileName, const char* sectionName)
{
	PLog(__FUNCTION__" start");
	deleteStatements();

	bool bResult = DatabaseManagerCommon::openDataStorage(fullFileName, sectionName);
	if (!bResult)
		return false;

	prepareStatements();

	getNextId64("testTB");
	PLog("ColTCADbmManager::openDataStorage end");
	return bResult;
}

// reporting db manager specific parameters to Monitor
void ColTCADbmManager::composeHeLoggerParameters( HeLoggerParameters& parameters ) const
{
	PString strValue;

	strValue.assign("").appendUint(errorsInMessages);
	HeLoggerParameter errInMessages("errorsInMessages", strValue);

	parameters.push_back(errInMessages);
}

//
// ColTCADbmManager initialization
//

void ColTCADbmManager::init(ColTCADbmServerObject* obj)
{
	dbmObj = obj;
}

void ColTCADbmManager::rollbackOnError()
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

void ColTCADbmManager::setAutoCommitTrueOnError()
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

void ColTCADbmManager::setAutoCommitTrue()
{
	setAutoCommit(true);
}

/* Prepared statement management */

void ColTCADbmManager::prepareStatements()
{   
	sampleTestingStmt = new SampleTestingStmt(*this);

}

void ColTCADbmManager::deleteStatements()
{
	delete sampleTestingStmt;

	zeroStatements();
}

void ColTCADbmManager::zeroStatements()
{
	sampleTestingStmt = 0;
}

/* Message processing */


////////////////////////////////////////////////////////////////////////////////////////////////////
