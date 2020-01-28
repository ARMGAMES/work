////////////////////////////////////////////////////////////////////////////////

#include "DbTestingObject.h"
#include "DbTestingDbmManager.h"
#include "shared.h"

////////////////////////////////////////////////////////////////////////////////

DbTestingDbmManager::DbTestingDbmManager() :
	dbmObj(0),
	dbReconnectInterval(DEFAULT_DB_RECONNECT_INTERVAL),
	useAutoPtrStmt(false),
	sleepBeforeExec(false),
	sleepAfterExec(false),
	deleteStmtsBeforeOpenDataStorage(false),
	deleteStatementsInPrepareStatements(false),
	sleepInterval(DEFAULT_STMT_SLEEP_INTERVAL),
	doSecondStatement(false)
{
	zeroStatements();
}


DbTestingDbmManager::~DbTestingDbmManager()
{
	try { deleteStatements(); } catch(...) { }
}


void DbTestingDbmManager::readDynamicConfiguration(PIniFile& iniFile)
{
	PIniFile::Section const* pSection = iniFile.getSection("DYNAMICSETTINGS");
	if (pSection)
	{
		dbReconnectInterval = pSection->getIntProperty("dbReconnectInterval", DEFAULT_DB_RECONNECT_INTERVAL);
		useAutoPtrStmt = pSection->getIntProperty("useAutoPtrStmt", 0) != 0;
		sleepBeforeExec = pSection->getIntProperty("sleepBeforeExec", 0) != 0;
		sleepAfterExec = pSection->getIntProperty("sleepAfterExec", 0) != 0;
		deleteStmtsBeforeOpenDataStorage = pSection->getIntProperty("deleteStmtsBeforeOpenDataStorage", 0) != 0;
		deleteStatementsInPrepareStatements = pSection->getIntProperty("deleteStatementsInPrepareStatements", 0) != 0;
		sleepInterval = pSection->getIntProperty("sleepInterval", DEFAULT_STMT_SLEEP_INTERVAL);
		doSecondStatement = pSection->getIntProperty("doSecondStatement", 0) != 0;
	}
	else
		PLog("Error: no [DYNAMICSETTINGS] section in ini file. Default/old values are used");

	PLog("dbReconnectInterval = %u mSec", dbReconnectInterval);
	PLog("useAutoPtrStmt = %s", useAutoPtrStmt ? "true" : "false");
	PLog("sleepBeforeExec = %s", sleepBeforeExec ? "true" : "false");
	PLog("sleepAfterExec = %s", sleepAfterExec ? "true" : "false");
	PLog("deleteStmtsBeforeOpenDataStorage = %s", deleteStmtsBeforeOpenDataStorage ? "true" : "false");
	PLog("deleteStatementsInPrepareStatements = %s", deleteStatementsInPrepareStatements ? "true" : "false");
	PLog("sleepInterval = %u mSec", sleepInterval);
	PLog("doSecondStatement = %s", doSecondStatement ? "true" : "false");
}


bool DbTestingDbmManager::openDataStorage(const char* fullFileName, const char* sectionName)
{
	PLog("DbTestingDbmManager::openDataStorage start");

	if (deleteStmtsBeforeOpenDataStorage)
		deleteStatements();

	if (!DatabaseManagerCommon::openDataStorage(fullFileName, sectionName))
		return false;

	setCurrentSchema();

	prepareStatements();

	PLog("DbTestingDbmManager::openDataStorage end");
	return true;
}


void DbTestingDbmManager::setCurrentSchema()
{
	if (!*schemaStr)
		return;

	try
	{
		PString exec = "SET CURRENT SCHEMA ";
		exec.append(schemaStr);
		DbmStatement stmt(*this);
		stmt.execDirect(exec);

		PLog("Current schema was changed to '%s'", schemaStr.c_str());
	}

	catch (PSqlError const& er)
	{
		PLog("Current schema was not changed to '%s', error code %d, reason: %s",
			schemaStr.c_str(), er.code(), er.why());
	}
}


INT16 DbTestingDbmManager::getAggEndDate(SrvDate& endDate, PString& errBuf)
{
	PLog("getAggEndDate");
	PLog("- useAutoPtrStmt=%s", useAutoPtrStmt ? "true": "false");
	PLog("- sleepBeforeExec=%s", sleepBeforeExec ? "true": "false");
	PLog("- sleepAfterExec=%s", sleepAfterExec ? "true": "false");
	PLog("- sleepInterval=%u msec", sleepInterval);
	PLog("- doSecondStatement=%s", doSecondStatement ? "true" : "false");

	try
	{
		if (sleepBeforeExec)
		{
			PLog("Sleeping %u msec before execute...", sleepInterval);
			PThread::sleep(sleepInterval);
		}

		if (useAutoPtrStmt)
		{
			getDateAutoPtrStmt->execute();

			if (sleepAfterExec)
			{
				PLog("Sleeping %u msec after execute...", sleepInterval);
				PThread::sleep(sleepInterval);
			}

			if (!getDateAutoPtrStmt->fetch())
			{
				errBuf = "Failed to get last completed aggregated date from database (no data)";
				PLog("%s", errBuf.c_str());
				return INCONSISTENT_DATA;
			}
			getDateAutoPtrStmt->getLastDate(endDate);
			getDateAutoPtrStmt->closeCursor();
		}
		else
		{
			getDateStmt->execute();

			if (sleepAfterExec)
			{
				PLog("Sleeping %u msec after execute", sleepInterval);
				PThread::sleep(sleepInterval);
			}

			if (!getDateStmt->fetch())
			{
				errBuf = "Failed to get last completed aggregated date from database (no data)";
				PLog("%s", errBuf.c_str());
				return INCONSISTENT_DATA;
			}
			getDateStmt->getLastDate(endDate);
			getDateStmt->closeCursor();
		}

		if (doSecondStatement)
		{
			PLog("Running second statement");

			PThread::sleep(sleepInterval);

			getDateStmt->execute();
			if (!getDateStmt->fetch())
			{
				errBuf = "Failed to get last completed aggregated date from database (no data)";
				PLog("%s", errBuf.c_str());
				return INCONSISTENT_DATA;
			}
			getDateStmt->getLastDate(endDate);
			getDateStmt->closeCursor();

			PLog("Finished second statement");
		}

		PString s;
		PLog("- endDate: '%s'", srvDateToString(endDate, s));

		return DBM_NO_ERROR;
	}

	catch(const PSqlError& er)
	{
		errBuf = er.why();
		return er.code();
	}
}


void DbTestingDbmManager::prepareStatements()
{
	PLog("prepareStatements");

	if (deleteStatementsInPrepareStatements)
		deleteStatements();

    getDateStmt = new GetLastFinishedAggregationStmt(*this);
	getDateAutoPtrStmt = new GetLastFinishedAggregationStmt(*this);
}


void DbTestingDbmManager::deleteStatements()
{
	PLog("deleteStatements");

	delete getDateStmt;
	zeroStatements();
}


void DbTestingDbmManager::zeroStatements()
{
	PLog("zeroStatements");

	getDateStmt = 0;
	getDateAutoPtrStmt = 0;
}

//////////////////////////////////////////////////////////////////////////
