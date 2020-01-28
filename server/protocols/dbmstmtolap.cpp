#include "dbmstmtolap.h"
#include "licenserules.h"

/**************   DbmStatementWithLog********************************/
// !!!DO NOT USE!!! - obsolete. MG TODO: remove after no longer being used anywhere in the dbm code.
DbmStatementWithLog::DbmStatementWithLog( DatabaseManagerCommonWithOlap& manager_, OlapMsgSysInfo::eOlapDataGroup dataGroup, const bool logActive_ /*=true*/ )
		:
		DbmStatement( manager_ ),
		logMan(manager_),
		logMessage(dataGroup)
		, logActive( logActive_ ) // PYR-36192
{
	replicateOnly = false;
	// PYR-36192 - only call getDbmId when it's needed
	if( logActive )
	{
		logMessage.sysInfo.setSourceHostId(logMan.getHostId()); // PYR-27618
		logMessage.sysInfo.setDbmId(logMan.getDbmId()); // PYR-30404
	}
}
// PYR-91675
DbmStatementWithLog::DbmStatementWithLog( DatabaseManagerCommonWithOlap& manager_, OlapMsgSysInfo::eOlapDataGroup dataGroup, const eExecutionMode mode /*=eExecuteAndReplicate*/ )
	: DbmStatement( manager_ )
	, replicateOnly( mode == eReplicateOnly ) // PYR-91675
	, logMan( manager_ )
	, logMessage( dataGroup )
	, logActive( mode != eExecuteOnly ) // PYR-36192, PYR-91675
{
	// PYR-36192 - only call getDbmId when it's needed
	if( logActive )
	{
		logMessage.sysInfo.setSourceHostId(logMan.getHostId()); // PYR-27618
		logMessage.sysInfo.setDbmId(logMan.getDbmId()); // PYR-30404
	}
}

void DbmStatementWithLog::executeLog()
{
	if( !logActive && replicateOnly ) // PYR-91675
	{
		PLog( LOG_TRACE_FAULTY_LOGIC ": DbmStatementWithLog::executeLog() - !logActive && replicateOnly" );
	}
	if (!replicateOnly)
	{
		try
		{
			execute();
		}
		catch(const PSqlError&) //-V2010
		{
			clearLogMessage();
			throw;
		}
		catch(const PDbDisconnectError&) //-V2010
		{
			clearLogMessage();
			throw;
		}
		catch(const PDbDeadlockError&) //-V2010
		{
			clearLogMessage();
			throw;
		}
		catch (const PDbFederatedDeadInternalConnectError&)
		{
			clearLogMessage();
			throw;
		}
	}
	replicateToOlap();
	clearLogMessage();
}

bool DbmStatementWithLog::execLogUpdateDelete()
{
	if( !logActive && replicateOnly ) // PYR-91675
	{
		PLog( LOG_TRACE_FAULTY_LOGIC ": DbmStatementWithLog::execLogUpdateDelete() - !logActive && replicateOnly" );
	}
	bool result = true;
	if (!replicateOnly)
	{
		try
		{
			result = execUpdateDelete();
		}
		catch(const PSqlError&) //-V2010
		{
			clearLogMessage();
			throw;
		}
		catch(const PDbDisconnectError&) //-V2010
		{
			clearLogMessage();
			throw;
		}
		catch(const PDbDeadlockError&) //-V2010
		{
			clearLogMessage();
			throw;
		}
		catch (const PDbFederatedDeadInternalConnectError&)
		{
			clearLogMessage();
			throw;
		}
	}
	if (result)
	{
		replicateToOlap();
	}
	clearLogMessage();
	return result;
}

bool DbmStatementWithLog::execLogConditionalInsert()
{
	// to be used only with LZ or AR approval
	// written for the statement of the following kind:
	// insert into T1 (PK, Data) select ?,? from sysibm.sysdummy1 where not exists (select 1 from T1 where PK=?)
	return execLogUpdateDelete();
}

// PYR-37710
bool DbmStatementWithLog::execLogInsertIgnoreDuplicate()
{
	if( !logActive && replicateOnly ) // PYR-91675
	{
		PLog( LOG_TRACE_FAULTY_LOGIC ": DbmStatementWithLog::execLogInsertIgnoreDuplicate() - !logActive && replicateOnly" );
	}
	bool result = true;
	if (!replicateOnly)
	{
		try
		{
			result = execInsertIgnoreDuplicate();
		}
		catch(const PSqlError&) //-V2010
		{
			clearLogMessage();
			throw;
		}
		catch(const PDbDisconnectError&) //-V2010
		{
			clearLogMessage();
			throw;
		}
		catch(const PDbDeadlockError&) //-V2010
		{
			clearLogMessage();
			throw;
		}
		catch (const PDbFederatedDeadInternalConnectError&)
		{
			clearLogMessage();
			throw;
		}
	}
	if (result)
	{
		replicateToOlap();
	}
	clearLogMessage();
	return result;
}


void DbmStatementWithLog::replicateToOlap()
{
	if (logActive) 
	{
		if (logMessage.msgId)
		{
			logMan.insertTransactionUpdate (logMessage.msgId, logMessage.msgBody, logMessage.msgMask, logMessage.sysInfo);
		}
		else
		{
			PLog("Empty Log for DbmStatementWithLog"); 
		}
	}
}

void DbmStatementWithLog::clearLogMessage()
{
	logMessage.msgId = 0;
	logMessage.msgBody.clear();
	logMessage.sysInfo.clearDynamicData(); // PYR-28804
}

void DbmStatementWithLog::initLog()
{
	logMessage.msgId = getMsgCode();
	logMessage.msgBody.clear();
	logMessage.sysInfo.clearDynamicData(); // PYR-28804

	// PYR-27214
	CommUtcTime tfo;
	if( logMan.getUtcTimeForOlap(tfo) )
	{
		logMessage.sysInfo.setUtcTimeForOlap(tfo);
	}
}

/*virtual*/ void DbmStatementWithLog::parseOlapMessage(CommMsgParser& /*parser*/)
{
	throw PSqlError("DbmStatementWithLog::parseOlapMessage not implemented");
}

/*virtual*/ UINT32 DbmStatementWithLog::getMsgCode() const
{
	throw PSqlError("DbmStatementWithLog::getMsgCode not implemented");
}

/* Obsolete by #22086
void DbmStatementWithLog::initFromOlapMessage(const CommMsgBody& olapMsgBody)
{
	initLog();
	// Make a copy to send to the next OLAP.
	CommMsgBody msgCopy;
	msgCopy.copyFrom(olapMsgBody);
	msgCopy._shift(sizeof(UINT32));
	logMessage.msgBody.moveFrom(msgCopy);
	// Initialize the statement
	CommMsgParser parser(logMessage.msgBody);
	parseOlapMessage(parser);
}
*/

void DbmStatementWithLog::initFromOlapMessage(CommMsgParser& olapMsgParser)
{
	initLog();
	// Make a copy to send to the next OLAP.
	logMessage.msgBody.copyFromSkipReqId(olapMsgParser.getMsgBody());
	// Initialize the statement
	parseOlapMessage(olapMsgParser);
}

//
// Support for SqlValue-derived classes
//
void DbmStatementWithLog::composeLog(const PSqlValue& value)
{
	value.compose(logMessage.msgBody);
}

UINT32 DbmStatementWithLog::setOlapLicenseMaskByUserIntId(UINT32 userIntId)	// PYR-25464
{
	UINT32 licenseId = eLicenceDefault;
	if (logMan.getLicenseIdByUserIntId(userIntId, licenseId))
	{
		PLicenseMask licenseMask;
		if (!LicenseRules::maskFromLicenseId(licenseId, licenseMask))
		{
			PLog(LOG_TRACE_FAULTY_LOGIC "setOlapLicenseMaskByUserIntId() licenseId %d does not translate to mask", licenseId);
		}
		logMessage.sysInfo.setLicenseMask(licenseMask);
	}
	return licenseId;
}

void DbmStatementWithLog::setOlapLicenseMaskFromLicenseId(UINT32 licenseId_)
{
	PLicenseMask licenseMask;
	if (!LicenseRules::maskFromLicenseId(licenseId_, licenseMask))
	{
		PLog(LOG_TRACE_FAULTY_LOGIC "setOlapLicenseMaskFromLicenseId() licenseId %d does not translate to mask", licenseId_);
	}
	logMessage.sysInfo.setLicenseMask(licenseMask);
}

/**************   DbmStatementWithLog end ********************************/
