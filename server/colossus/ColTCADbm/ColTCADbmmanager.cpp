// ===========================================================
// Copyright (C) 2018 PYR Software Ltd. All rights reserved.
// ===========================================================

//dbmanager.cpp

#include "ppinifile.h"
#include "ColTCADbmmanager.h"
#include "pperror.h"
#include "pplogfile.h"
#include "ColTCADbmobject.h"
#include "../utils.h"
#include "../DateExtents.h"
#include "../shared/TCAShared.h"

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

void ColTCADbmManager::setCurrentSchema()//throw
{
	if (!*schemaStr)
		return;
	PString exec = "SET CURRENT SCHEMA ";
	exec.append(schemaStr);
	DbmStatement stmt(*this);
	stmt.execDirect( (SQLCHAR*)exec.c_str(), DatabaseManagerCommon::eCheckNotSuccess );
}

bool ColTCADbmManager::openDataStorage(const char* fullFileName, const char* sectionName)
{
	PLog("ColTCADbmManager::openDataStorage start");
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
		{ TCA_CONFIG_TABLE_NAME,	false },
		{ TCA_TASK_TABLE_NAME,		false },
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

INT16 ColTCADbmManager::getTCAUsersTournData(TCAColTaskData& tcaColTaskData, PString& sqlErr)
{
	PLog("getTCAUsersTournData: configId=%u", tcaColTaskData.tcaConfig.configId);
	
	try
	{
		PString query;
		TCAROIQueryBuilderAgg aggQueryBuilder(tcaColTaskData.tcaConfig, tcaColTaskData.tournROIExtentOutputs.extents);
		aggQueryBuilder.buildQuery(query);

		// if no agg data needed, query is empty.
		if (query.length() > 0)
		{
			GetTCAUserTournStmt getTCAUserTournStmt(*this, query);
			getTCAUserTournStmt.execGet(tcaColTaskData.tcaConfig.maxRunTimeMinutes, tcaColTaskData.userTournDataMap);
		}
		return DBM_NO_ERROR;
	}
	catch (const PSqlError& er)
	{
		++errorsInMessages;
		sqlErr = er.why();
		return er.code();
	}
}

INT16 ColTCADbmManager::getTCAPairsByUidStartLetter(TCAColTaskData& tcaColTaskData, char uidStartLetter, PString& sqlErr)
{
	PLog("getTCAPairsByUidStartLetter: configId=%u", tcaColTaskData.tcaConfig.configId);
	try
	{
		PString query;
		TCAPartnersQueryBuilderAgg aggQueryBuilder(tcaColTaskData.tcaConfig, tcaColTaskData.tournPartnerExtentOutputs.extents, uidStartLetter);
		aggQueryBuilder.buildQuery(query);

		// if no agg data needed, query is empty.
		if (query.length() > 0)
		{
			GetTCAPartnersStmt getTCAPartnersStmt(*this, query);
			getTCAPartnersStmt.execGet(tcaColTaskData.tcaConfig.maxRunTimeMinutes, tcaColTaskData.tcaConfig.maxColPairSize, tcaColTaskData.colAggPairMap);
		}

		return DBM_NO_ERROR;
	}
	catch (const PSqlError& er)
	{
		++errorsInMessages;
		sqlErr = er.why();
		return er.code();
	}
}

INT16 ColTCADbmManager::getTCATaskExtent(TCAColTaskData& tcaColTaskData, PString& sqlErr)
{
	PLog("getTCATaskExtent");
	try
	{
		SrvTime fromTime, toTime;
		fromTime.currentLocalTime();
		toTime.currentLocalTime();
		ColUtils::addDays(fromTime, -tcaColTaskData.tcaConfig.daysStartOffset);
		ColUtils::addDays(toTime, -tcaColTaskData.tcaConfig.daysEndOffset);

		DateRange requested;
		requested.first.assign(fromTime);
		requested.second.assign(toTime);

		// Get TournROI extent
		GetDateExtentsOptions tournROIExtentsOptions(true /* verbose */, ColAggTabType::eAggTypeTournRoi, requested);

		bool res = ::getDateExtents(tournROIExtentsOptions, *this, tcaColTaskData.tournROIExtentOutputs);

		if (!res)
		{
			PLog("Warning: found no tournROI agg date!");
			tcaColTaskData.lastTournROIAggDate = requested.first;
			ColUtils::addDays(tcaColTaskData.lastTournROIAggDate, -1);
		}
		else
		{
			tcaColTaskData.lastTournROIAggDate = tcaColTaskData.tournROIExtentOutputs.actual.second;
		}
		

		// Get TournPartner extent
		GetDateExtentsOptions tournPartnerExtentsOptions(true /* verbose */, ColAggTabType::eAggTypeTournPartners, requested);

		res = ::getDateExtents(tournPartnerExtentsOptions, *this, tcaColTaskData.tournPartnerExtentOutputs);

		if (!res)
		{
			PLog("Warning: found no tournPartner agg date!");
			tcaColTaskData.lastTournPartnerAggDate = requested.first;
			ColUtils::addDays(tcaColTaskData.lastTournPartnerAggDate, -1);
		}
		else
		{
			tcaColTaskData.lastTournPartnerAggDate = tcaColTaskData.tournPartnerExtentOutputs.actual.second;
		}
		
		return DBM_NO_ERROR;
	}
	catch (const PSqlError& er)
	{
		++errorsInMessages;
		sqlErr = er.why();
		return er.code();
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////
