
#include "ppinifile.h"
#include "SrvTemplateDbmmanager.h"
#include "pperror.h"
#include "pplogfile.h"
#include "SrvTemplateDbmobject.h"
#include "commonmacros.h"

SrvTemplateDbmManager::SrvTemplateDbmManager()
{
	errorsInMessages = 0;
	dbReconnectInterval = 0;
	zeroStatements();
}

SrvTemplateDbmManager::~SrvTemplateDbmManager()
{ 
	try //for pclint as in "dbm\dbmanager.h"
	{
		deleteStatements();
	}
	catch(...)
	{
	}
}

bool SrvTemplateDbmManager::openDataStorage(const char* fullFileName, const char* sectionName)
{
	PLog(__FUNCTION__" start");
	deleteStatements();

	bool bResult = DatabaseManagerCommon::openDataStorage(fullFileName, sectionName);
	if (!bResult)
		return false;

	prepareStatements();

	getNextId64("testTB");
	PLog("SrvTemplateDbmManager::openDataStorage end");
	return bResult;
}

// reporting db manager specific parameters to Monitor
void SrvTemplateDbmManager::composeHeLoggerParameters( HeLoggerParameters& parameters ) const
{
	PString strValue;

	strValue.assign("").appendUint(errorsInMessages);
	HeLoggerParameter errInMessages("errorsInMessages", strValue);

	parameters.push_back(errInMessages);
}

//
// SrvTemplateDbmManager initialization
//

void SrvTemplateDbmManager::init(SrvTemplateDbmServerObject* obj)
{
	dbmObj = obj;
}

void SrvTemplateDbmManager::rollbackOnError()
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

void SrvTemplateDbmManager::setAutoCommitTrueOnError()
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

void SrvTemplateDbmManager::setAutoCommitTrue()
{
	setAutoCommit(true);
}

/* Prepared statement management */

void SrvTemplateDbmManager::prepareStatements()
{   
	sampleTestingStmt = new SampleTestingStmt(*this);

}

void SrvTemplateDbmManager::deleteStatements()
{
	delete sampleTestingStmt;

	zeroStatements();
}

void SrvTemplateDbmManager::zeroStatements()
{
	sampleTestingStmt = 0;
}

/* Message processing */


////////////////////////////////////////////////////////////////////////////////////////////////////
