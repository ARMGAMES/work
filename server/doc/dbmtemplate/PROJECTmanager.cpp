//dbmanager.cpp

#include	"ppinifile.h"
#include	"PROJECTmanager.h"
#include	"pperror.h"
#include	"pplogfile.h"
#include	"PROJECTobject.h"

PROJECTManager::PROJECTManager()
{
	errorsInMessages = 0;
	dbReconnectInterval = 0;
	zeroStatements();
	generator.setRangeIncrement(1);
}

PROJECTManager::~PROJECTManager()
{ 
	try //for pclint as in "dbm\dbmanager.h"
	{
		deleteStatements();
	}
	catch(...)
	{
	}
}

void PROJECTManager::setCurrentSchema()//throw
{
	if (!*schemaStr)
		return;
	PString exec = "SET CURRENT SCHEMA ";
	exec.append(schemaStr);
	DbmStatement stmt(*this);
	stmt.execDirect( (SQLCHAR*)exec.c_str(), DatabaseManagerCommon::eCheckNotSuccess );
}

bool PROJECTManager::openDataStorage(const char* fullFileName, const char* sectionName)
{
	PLog("PROJECTManager::openDataStorage start");
	deleteStatements();

	bool bResult = DatabaseManagerCommon::openDataStorage(fullFileName, sectionName);
	if (!bResult)
		return false;
	if ( *schemaStr ) 
	{
		try
		{
			setCurrentSchema();
			PLog("Current schema was changed to '%s'", schemaStr.c_str());
		}
		catch( const PSqlError& er)
		{
			PLog("Current schema was not changed to '%s', error code %d, reason: %s", 
				schemaStr.c_str(), er.code(), er.why());
		}
	}

	prepareStatements();

	PLog("PROJECTManager::openDataStorage end");
	return bResult;
}

// reporting db manager specific parameters to Monitor
void PROJECTManager::composeHeLoggerParameters( HeLoggerParameters& parameters ) const
{
	HeLoggerParameter param1("someParam", "someValue");
	parameters.push_back(param1);
}

//
// PROJECTManager initialization
//

void PROJECTManager::init(PROJECTServerObject* obj)
{
	dbmObj = obj;
}

void PROJECTManager::commit()
{
	DatabaseManagerCommon::commit();
}

void PROJECTManager::rollbackOnError()
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

void PROJECTManager::setAutoCommitTrue()
{
	setAutoCommit(true);
}

/* Prepared statement management */

void PROJECTManager::prepareStatements()
{
    someDummyStmt		= new SomeDummyStmt(*this);       
}

void PROJECTManager::deleteStatements()
{
    delete someDummyStmt;
	zeroStatements();
}

void PROJECTManager::zeroStatements()
{
    someDummyStmt		= 0;
}

/* Message processing */
