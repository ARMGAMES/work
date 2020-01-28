////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "GenericDbmmanager.h"
#include "dbmstmt.h"

////////////////////////////////////////////////////////////////////////////////

GenericDbmManager::GenericDbmManager()
	: dbmObj(nullptr)
	, commitThreshold(DEFAULT_COMMIT_THRESHOLD)
	, dbReconnectIntervalMSec(DEFAULT_DB_RECONNECT_INTERVAL_MSEC)
	, errorsInMessages(0)
	, totalErrorsInMessages(0)
{

}

void GenericDbmManager::init(GenericServerObjectBase* obj)
{
	dbmObj = obj;
}

void GenericDbmManager::dynamicInit(const PIniFile& iniFile)
{
	doPreDynamicInit(iniFile);

	const PIniFile::Section* section = iniFile.getSection("DYNAMICSETTINGS");
	if (!section)
	{
		PLog("Error: no [DYNAMICSETTINGS] section in ini file. Default/old values are used");
	}
	else
	{
		doDynamicInitReadSettings(*section);
	}

	// Reset error counters
	totalErrorsInMessages = errorsInMessages = 0;
	PLog("Error counters reset");

	doDynamicInitLogSettings();

	doPostDynamicInit(iniFile);
}

bool GenericDbmManager::openDataStorage(const char* fullFileName, const char* sectionName)
{
	PLog(__FUNCTION__ "(fullFileName: %s, sectionName: %s)", fullFileName, sectionName);

	doPreOpenDataStorage(fullFileName, sectionName);

	deleteStatements();

	if (!DatabaseManagerCommon::openDataStorage(fullFileName, sectionName))
	{
		return false;
	}

	setCurrentSchema();

	prepareStatements();

	doPostOpenDataStorage(fullFileName, sectionName);

	PLog("#" __FUNCTION__);

	return true;
}

void GenericDbmManager::commit()
{
	DatabaseManagerCommon::commit();
}

void GenericDbmManager::rollbackOnError()
{
	try
	{
		rollback();
		generator.rollback();
	}
	catch (const PSqlError& er)
	{
		PLog("rollbackOnError ... %s", er.why());
		throw PDbDisconnectError(er.why());
	}
}

void GenericDbmManager::setAutoCommitTrue()
{
	setAutoCommit(true);
}

void GenericDbmManager::composeHeLoggerParameters(HeLoggerParameters& parameters)
{
	{
		PString val;
		val.appendUint(errorsInMessages);
		parameters.push_back(HeLoggerParameter("errorsInMessages", val));
	}

	{
		PString val;
		val.appendUint(totalErrorsInMessages);
		parameters.push_back(HeLoggerParameter("totalErrorsInMessages", val));
	}

	totalErrorsInMessages += errorsInMessages;
	errorsInMessages = 0;
}

void GenericDbmManager::setCurrentSchema()
{
	if (!*schemaStr)
	{
		return;
	}

	try
	{
		PString exec = "SET CURRENT SCHEMA ";
		exec.append(schemaStr);
		DbmStatement stmt(*this);
		stmt.execDirect(exec);

		PLog("Current schema was changed to '%s'", schemaStr.c_str());
	}

	catch (const PSqlError& er)
	{
		PLog("Current schema was not changed to '%s', error code %d, reason: %s",
			schemaStr.c_str(), er.code(), er.why());
	}
}

void GenericDbmManager::doDynamicInitReadSettings(const PIniFile::Section& section)
{
	UINT32 rangeIncrement = section.getIntProperty("rangeIncrement", DEFAULT_RANGE_INCREMENT);
	generator.setRangeIncrement(rangeIncrement);
	PLog("rangeIncrement changed to %u", rangeIncrement);

	dbReconnectIntervalMSec = section.getIntProperty("dbReconnectIntervalMsec", DEFAULT_DB_RECONNECT_INTERVAL_MSEC);
	commitThreshold = section.getIntProperty("commitThreshold", DEFAULT_COMMIT_THRESHOLD);
}

void GenericDbmManager::doDynamicInitLogSettings() const
{
	PLog("dbReconnectIntervalMSec = %u mSec", dbReconnectIntervalMSec);
	PLog("commitThreshold = %u", commitThreshold);
}

////////////////////////////////////////////////////////////////////////////////
