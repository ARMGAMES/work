
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
	generator.setRangeIncrement(1);
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

void ColTCADbmManager::setCurrentSchema()
{
	if (!*schemaStr)
		return;
	PString exec = "SET CURRENT SCHEMA ";
	// ALTER USER dbdev WITH DEFAULT_SCHEMA = dbo;
	exec.append(schemaStr);
	DbmStatement stmt(*this);
	stmt.execDirect( (SQLCHAR*)exec.c_str(), DatabaseManagerCommon::eCheckNotSuccess );
}

bool ColTCADbmManager::openDataStorage(const char* fullFileName, const char* sectionName)
{
	PLog(__FUNCTION__" start");
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

	PLog("ColTCADbmManager::openDataStorage end");
	return bResult;
}

const DbmGenerator::Generator* ColTCADbmManager::getGenerators( size_t& generatorsSize ) const // override
{
	static const DbmGenerator::Generator generators[] =
	{
		//  objectName  			local
		//	--------------			-----------
		{ "SampleTable1",		false },
		{ "SampleTable2",		false },
	};
	generatorsSize = SZARR( generators );
	return generators;
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
}

void ColTCADbmManager::deleteStatements()
{
	zeroStatements();
}

void ColTCADbmManager::zeroStatements()
{
}

/* Message processing */


////////////////////////////////////////////////////////////////////////////////////////////////////
