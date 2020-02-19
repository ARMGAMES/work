// ===========================================================
// Copyright (C) 2013 PYR Software Ltd. All rights reserved.
// ===========================================================

#include "dbmserverobjectcommon.h"
#include "counters.h"
#include "HeLogger.h"
#include "dbmmsg.h"
#include "dbm.h"

DbmServerObjectCommon::~DbmServerObjectCommon()
{
}

void DbmServerObjectCommon::reportToHeLogger( const CommMonitorLoggerObject& heLogger, CommMsgBody& reportBody )
{
}


void DbmServerObjectCommon::initDBMServerObject(
	DatabaseManagerCommon* dbManagerCommon_
	, const PIniFile& /*iniFile*/
)
{
	dbManagerCommon = dbManagerCommon_;
}

void DbmServerObjectCommon::dynamicInit( const PIniFile& iniFile )
{
}


// virtual
bool DbmServerObjectCommon::registerAllFactories()
{
	return true;
}
