#pragma once


///*
///////// headers to use ODBC ////////////////
#include "ppinclude.h"
#ifdef PWIN32
#include	<windows.h>
#include	<sqltypes.h>
#include	<sql.h>
#include	<sqlext.h>

#ifndef PWIN64
#ifndef SQLLEN
#define SQLLEN	SQLINTEGER
#endif
#endif

//*/
///////// headers to use DB2 CLI /////////////
/*
#include "sqlcli.h"
#include "sqlcli1.h"
*/


#elif defined (PPOSIX)
#include	"sqlcli.h"
#include	"sqlcli1.h"
#endif

#include	"ppstring.h"
#include	"pplib.h"

#include	"dbmgenerator.h"
#include	"pyrtime.h"
#include	"sqlerror.h"
#include	"dbmconstants.h"

class	PDbDisconnectError
//NOT derived from PError to avoid catching by catch( PError& )
{
private:
PString s;

public:
	PDbDisconnectError( const char* msg_): s( msg_ ){}
	const char* why() const { return s; }
};

class	PDbDeadlockError
//NOT derived from PError to avoid catching by catch( PError& )
{
private:
PString s;

public:
	PDbDeadlockError( const char* msg_): s( msg_ ){}
	const char* why() const { return s; }
};

// PYR-112629 SQLSTATE=40506 dead connection between DB_s in federated (STAR) schema
// auto-healing: internal reconnects should happen after error, so call re-try should work
class	PDbFederatedDeadInternalConnectError
{
public:
	const PString sqlError;
	PDbFederatedDeadInternalConnectError(const char* msg_) : sqlError(msg_) {}
};

class CommServerObjectWithOlapInteface;
class CommMsgBody;


#define DEPLOYMENT_SECTION_NAME		"DEPLOYMENT" 
#define STATICSETTINGS_SECTION_NAME	"STATICSETTINGS"
#define DB_LOCKTIMEOUT_PARAM_NAME "locktimeout" 
#define DB_DEADLOCK_RETRIES_PARAM_NAME "maxdeadlockretries" 
#define DB_RECONNECT_ATTEMPTS_PARAM_NAME "reconnectattempts"
#define DB_DEFAULT_PARAL_PARAM_NAME "default_paral" 
#define DB_MAX_PARAL_PARAM_NAME "max_paral"


#define RECONNECT_SLEEP		1000

class DatabaseManagerCommon
{
public:
	enum eStmtFailureLogging 
	{
		eStmtFailureLogging_Off = 0,
		eStmtFailureLogging_All = 1,
		eStmtFailureLogging_AllButAllowedForOlap = 2,
	};

	enum CheckModifier {
		eCheckNotSuccess,	// fail if != SQL_SUCCESS
		eCheckError,		// fail if == SQL_ERROR
		eCheckErrorOrInfo //,	// fail if == SQL_ERROR || SQL_SUCCESS_WITH_INFO

	};
	struct SqlErrDetail // PYR-37710
	{
		SQLRETURN sqlRetCode;
		bool haveToCheck;
		static const INT32 SqlStateSize = 6;
		SQLCHAR SqlState[SqlStateSize];
		PString sqlErr;
		PString sqlStateLst;

		SqlErrDetail(SQLRETURN retcode);
		void checkRetCode(SQLSMALLINT HandleType, SQLHSTMT hstmt, CheckModifier modifier);
		bool hasError() const { return haveToCheck; }
		void raiseOnError() const;

	};

	DatabaseManagerCommon()
	{
		henv = 0;
		hdbc = 0;
		maxDbReconnectAttempts = 5;//why not?
		maxDbDeadlockRetries = 5; // PYR-37539 - just some reasonable value
		globalAutoCommitFlag = true;
		lockTimeout = 0; // do not set lock timeout
		stmtFailureLogging = eStmtFailureLogging_Off;

		generator.init(this);
	}
	/*lint -save -e1740 */
	virtual ~DatabaseManagerCommon()
	{
		closeDataStorage(); //lint !e1551
	}
	/*lint -restore */
	
	int getMaxDbReconnectAttempts() const { return maxDbReconnectAttempts; }
	INT32 getMaxDbDeadlockRetries() const { return maxDbDeadlockRetries; } // PYR-37539

	void readStmtFailureLoggingState(const PIniFile::Section& section);
	bool isStmtFalureLoggingOn(const PString& sqlState) const;
	eStmtFailureLogging getStmtFailureLogging() const { return stmtFailureLogging; }

	bool	openDataStorage(const char* fullFileName, const char* sectionName);
	void	closeDataStorage();
	const char * getDsName() const
	{
		return dsName.c_str();
	}

	void checkStmtRetcode(
		SQLRETURN		retcode,	//	in
		SQLHSTMT		hstmt,		//	in
		CheckModifier	modifier //= DatabaseManager::eCheckNotSuccess 
	) const

	{
		checkRetcode(SQL_HANDLE_STMT, retcode, hstmt, modifier);
	}

	void checkDbcRetcode(
		SQLRETURN		retcode,	//	in
		SQLHDBC			hdbc_,		//	in
		CheckModifier	modifier = eCheckNotSuccess
	) const
	{
		checkRetcode(SQL_HANDLE_DBC, retcode, hdbc_, modifier);
	}


	SQLHDBC getHdbc() const { return hdbc; }
	/*lint -save -e1536 */
	DbmGenerator& _getGenerator() { return generator; }
	/*lint -restore */
	UINT32 getNextId(const char* objectName)
	{
		return (UINT32)generator.getNextId(objectName);
	}
	UINT64 getNextId64(const char* objectName)
	{
		return generator.getNextId(objectName);
	}
	virtual bool isMainThreadManager() const 
	{
		return false; // default is no multithreading
	}

	virtual void ensureCanProcessUserRelatedTables(UINT32 licenseId, UINT32 userIntId) const /* can throw PSqlError */ {} // PYR-27872

/////////////////// PYR-108648 - collect roundtrip stats /////////////////////
	void resetRoundtripCounter() { roundtripCounter = 0; };
	void incrementRountripCounter() { roundtripCounter++; }
	UINT32 getRoundtripCounter() const { return roundtripCounter; }

	// PYR-112629 dbms that catch PDbFederatedDeadInternalConnectError exception should override this function to return 'true'

	virtual bool dbmSupportsRetryOnFederatedDeadConnection() const { return false; } // PYR-112629

protected:
	SQLHENV		henv;
	SQLHDBC		hdbc;
	int			maxDbReconnectAttempts;
	PString		schemaStr;
	bool		globalAutoCommitFlag;
	PString		dsName;
	INT32		lockTimeout; // PYR-35295
	INT32		maxDbDeadlockRetries; // PYR-37539
	PString		defaultParallelismDegree; // PYR-49252
	PString		maxParallelismDegree; // PYR-49252

	DbmGenerator generator;

	void setCurrentSchema();

	void _openDataStorage(const char* fullFileName, const char* sectionName);
	void setLockTimeout(); // PYR-35295
	void setParallelismDegree(const char* parallelismDegree); // PYR-49252
	void setDefaultParallelismDegree(); // PYR-49252
	void setMaxParallelismDegree(); // PYR-49252
	bool setDefaultParallelismDegreeNoThrow();	// PYR-72993, returns true if CURRENT DEGREE was reset to defaultParallelismDegree, false otherwise
	bool setMaxParallelismDegreeNoThrow();		// PYR-72993, returns true if CURRENT DEGREE was reset to maxParallelismDegree, false otherwise


	void checkRetcode(
		SQLSMALLINT HandleType,
		SQLRETURN retcode,	//	in
		SQLHANDLE handle,	//	in
		CheckModifier modifier) const;

	void setAutoCommit(bool bOn);
	bool isAutoCommit() const { return globalAutoCommitFlag; }
	void rollback();
	void commit();

private:
	eStmtFailureLogging stmtFailureLogging;
	UINT32 roundtripCounter = 0;
};

#ifdef POSTGRES // PYR-124995
#define SQL_WARNING_NON_OPTIMAL_PLAN	"54001"
#else
#define SQL_WARNING_NON_OPTIMAL_PLAN	"01602"
#endif

// MOVED here from dbmstmt.h within PYR-27855
inline bool errorStateAllowedForOlap (const PString& sqlState)
{
	return sqlState.equals("23505")       // constraint
		|| sqlState.equals("02000")       // nothing to update
		|| sqlState.equals(SQL_WARNING_NON_OPTIMAL_PLAN) // warning "non-optimal plan"
		|| sqlState.equals("77777")       // PYR-23400: artificial error state
		|| sqlState.equals("77778")       // PYR-23400: artificial error state (for future use)
		;
}





