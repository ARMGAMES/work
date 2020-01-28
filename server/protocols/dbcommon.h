#ifndef dbcommon_h_included
#define	dbcommon_h_included


///*
///////// headers to use ODBC ////////////////
#include "ppinclude.h"
#ifdef PWIN32
#include	<windows.h>

#ifdef USE_ODBC
	#if defined USE_DB2_CLI
	#error "Error: Both ODBC and CLI interfaces defined"
	#endif
#elif !defined USE_DB2_CLI
	#define USE_DB2_CLI
#endif

#ifdef USE_DB2_CLI
#include "sqlcli.h"
#include "sqlcli1.h"
#else
#include	"sqltypes.h"
#include	"sql.h"
#include	"sqlext.h"
#endif

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
#include	"olapmsghandler.h"
#include	"sqlerror.h"

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

class OlapReplicator;
class OlapManager;
class CommServerObjectWithOlapInteface;
class CommMsgBody;

struct EndMarkerData
{
	UINT32 transId;
	PString endTransStr;

	EndMarkerData(UINT32 transId_, const char* endTransStr_)
		: transId(transId_), endTransStr(endTransStr_)
	{
	}
};

enum eTableNames
{
	eTableName_Cashtrans,
//	eTableName_Transfers,
};

#define DEPLOYMENT_SECTION_NAME		"DEPLOYMENT" // host specific info is in this section
#define STATICSETTINGS_SECTION_NAME	"STATICSETTINGS"
#define DB_LOCKTIMEOUT_PARAM_NAME "locktimeout" // PYR-35295
#define DB_DEADLOCK_RETRIES_PARAM_NAME "maxdeadlockretries" // PYR-37539
#define DB_RECONNECT_ATTEMPTS_PARAM_NAME "reconnectattempts"
#define DB_DEFAULT_PARAL_PARAM_NAME "default_paral" // PYR-49252
#define DB_MAX_PARAL_PARAM_NAME "max_paral" // PYR-49252

class DatabaseManagerCommon
{
	friend class OlapReplicator;
	friend class OlapManager;

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

	PString		forUpdateClause; // PYR-28457
	DbmGenerator generator;
	// PYR-27418
	// This method must be overridden when support for sharing of IDs between NJ and IOM site is required by certain Dbms
	virtual const DbmGenerator::Generator* getGenerators( size_t& generatorsSize ) const
	{
		// by default - no IDs sharing support is required
		generatorsSize = 0;
		return nullptr;
	}
	// PYR-40317
	virtual const char* const * getMonotonicGenerators( size_t& arraySize ) const
	{
		// by default nothing is required to be monotonic
		arraySize = 0;
		return nullptr;
	}
	eHost hostId;
	INT32 remainingIdsThreshold;
	INT32 requiredIdsBuffer;
	bool useMasterGenerator; // some dbms have read-only instances which don't need generator functionality
	// end of PYR-27418

public:
	enum eStmtFailureLogging // PYR-27855
	{
		eStmtFailureLogging_Off = 0,
		eStmtFailureLogging_All = 1,
		eStmtFailureLogging_AllButAllowedForOlap = 2,
	};
	void readStmtFailureLoggingState(const PIniFile::Section& section);
	bool isStmtFalureLoggingOn(const PString& sqlState) const;
	eStmtFailureLogging getStmtFailureLogging() const { return stmtFailureLogging; }
private:
	eStmtFailureLogging stmtFailureLogging;

	void reportDB2ApplicationId();  // PYR-33265

protected:
	void addObjectNameToGenerator(const char* objectName, bool local); // PYR-27418
	void addMonotonicGenerator( const char* objectName ); // PYR-40317

	void		_openDataStorage(const char* fullFileName, const char* sectionName);
	void		setLockTimeout(); // PYR-35295
	void		setParallelismDegree( const char* parallelismDegree); // PYR-49252
	void		setDefaultParallelismDegree(); // PYR-49252
	void		setMaxParallelismDegree(); // PYR-49252
	bool		setDefaultParallelismDegreeNoThrow();	// PYR-72993, returns true if CURRENT DEGREE was reset to defaultParallelismDegree, false otherwise
	bool		setMaxParallelismDegreeNoThrow();		// PYR-72993, returns true if CURRENT DEGREE was reset to maxParallelismDegree, false otherwise

public: 

	DatabaseManagerCommon()
		: remainingIdsThreshold( 0 )
		, requiredIdsBuffer( 0 )
		, useMasterGenerator( false ) // by default do not use master generator functionality
	{
		henv = 0;
		hdbc = 0;
		maxDbReconnectAttempts = 5;//why not?
		maxDbDeadlockRetries = 5; // PYR-37539 - just some reasonable value
		globalAutoCommitFlag = true;
		lockTimeout = 0; // do not set lock timeout
		olapReplicator = 0;
		stmtFailureLogging = eStmtFailureLogging_Off;
		hostId = eHost_IOM;
	}
/*lint -save -e1740 */
	virtual ~DatabaseManagerCommon()
	{
		closeDataStorage(); //lint !e1551
	}
/*lint -restore */
	enum CheckModifier {
		eCheckNotSuccess,	// fail if != SQL_SUCCESS
		eCheckError,		// fail if == SQL_ERROR
		eCheckErrorOrInfo //,	// fail if == SQL_ERROR || SQL_SUCCESS_WITH_INFO
		//eCheckHwm
	};
	int getMaxDbReconnectAttempts() const { return maxDbReconnectAttempts; }
	INT32 getMaxDbDeadlockRetries() const { return maxDbDeadlockRetries; } // PYR-37539
	eHost getHostId() const { return hostId; }

	// PYR-27418
	bool useSharedIds() const { return generator.useSharedGenerators(); }
	bool isUseMasterGenerator() const { return useMasterGenerator; }
	// PYR-40317
	bool isMonotonicByUserId( const char* objectName ) const { return generator.isMonotonicByUserId( objectName ); }

public:
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


protected:
	void checkRetcode( 
		SQLSMALLINT HandleType,
		SQLRETURN retcode,	//	in
		SQLHANDLE handle,	//	in
		CheckModifier modifier ) const;

	void setAutoCommit( bool bOn );
	bool isAutoCommit() const { return globalAutoCommitFlag; }
	void rollback();
	void commit();


public:
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
		checkRetcode( SQL_HANDLE_STMT, retcode, hstmt, modifier );
	}

	void checkDbcRetcode ( 
		SQLRETURN		retcode,	//	in
		SQLHDBC			hdbc_,		//	in
		CheckModifier	modifier = eCheckNotSuccess 
		) const
	{
		checkRetcode( SQL_HANDLE_DBC, retcode, hdbc_, modifier );
	}


	SQLHDBC getHdbc() const { return hdbc; }
/*lint -save -e1536 */
	DbmGenerator& _getGenerator() { return generator; }
/*lint -restore */
	UINT32 getNextId( const char* objectName )
	{
		return (UINT32)generator.getNextId64( objectName );
	}
	UINT64 getNextId64( const char* objectName )
	{
		return generator.getNextId64( objectName );
	}
	virtual bool isMainThreadManager() const // PYR-40317
	{
		return false; // default is no multithreading
	}
	void getNext2Ids64( const char* objectName, UINT64& id1, UINT64& id2 ) // PYR-38720 - get next two consecutive ids
	{
		return generator.getNext2Ids64( objectName, id1, id2 );
	}
	void getNext2Ids( const char* objectName, UINT32& id1, UINT32& id2 ) // PYR-38720 - get next two consecutive ids (helper for UINT32 ids)
	{
		UINT64 id1_, id2_;
		getNext2Ids64( objectName, id1_, id2_ );
		id1 = id1_; id2 = id2_;
	}
	// only used to get "multiple" handId
	UINT64 getNextId64UnCached( const char* objectName )
	{
		return generator.getNextId64UnCached( objectName );
	}

	const char* getForUpdateClause() const { return forUpdateClause.c_str(); } // PYR-28457
	virtual INT16 getIdRange( const char* objectName, INT32 rangeSize, UINT64& startId, UINT64& endId, PString& sqlErr ) // PYR-27418
	{
		return generator.getIdRangeForSlave( objectName, rangeSize, startId, endId, sqlErr );
	}
	// Called by server object on static init for slave dbm
	bool initConnectionToMasterGenerator( IdRangeClientConnection* idRangeConn, DbmGenerator::DbmGeneratorCallback* initFinishedCallback ) // PYR-27418
	{
		PASSERT5( useMasterGenerator );
		generator.setIdRangeConn( idRangeConn );
		return generator.init( remainingIdsThreshold, requiredIdsBuffer, initFinishedCallback );
	}

///////////////// Interface from DatabaseManagerCommonWithOlap ///////////////////////
//protected:
private:
	OlapReplicator* olapReplicator;
public:
	const char* getMsgTableName() const;
	const char* getMsgObjectName() const;
	const char* getOrdinalPropName() const;
	OlapManager* _getOlapManager();
	virtual	INT16 getIntProperty(const char* /*propName*/, int& /*propValue*/, PString& /*sqlErr*/ );
	virtual	INT16 saveIntProperty(const char* /*propName*/, int /*propValue*/, PString& /*sqlErr*/ );
	//void commit();no need, already there
	void commitTransactionMessages(CommServerObjectWithOlapInteface* dbmObj);
	void rollbackTransactionMessages();
	void insertTransactionUpdate(const UINT32 msgId, CommMsgBody& body, BYTE msgMask, const OlapMsgSysInfo& sysInfo);
	virtual void allOlapsInDirectSend(bool inDirectSend) {};
	virtual bool getLicenseIdByUserIntId(UINT32 userIntId, UINT32& licenseId) 	 	// PYR-25464
	{
		return false;
	}
	// PYR-27214 -	default implementation. in dbm's that need this data
	//				override must set utcTime to a stored value and return true
	virtual bool getUtcTimeForOlap( CommUtcTime& utcTime ) const
	{
		utcTime.setNull();
		return false;
	}
	virtual UINT32 getDbmId() const { return MAX_DBM_ID_SUPPORTED; } // PYR-30404
protected:
	void setReplicator(OlapReplicator* repl);

///////////////// Interface from DatabaseManagerCommonWithOlap end ///////////////////////

/////////////////// #21575 - send end transaction marker /////////////////////
private:
	vector<EndMarkerData> transMarkers;													// PYR-26586
	void composeEndMarker();
public:
	void setSendEndMarker(UINT32 endTransInt_ = 0, const char* endTransStr_ = "");
/////////////////// #21575 - send end transaction marker end /////////////////

	virtual const char* getTableName(eTableNames tableNameEnum) const; // PYR-26147
	virtual void ensureCanProcessUserRelatedTables(UINT32 licenseId, UINT32 userIntId) const /* can throw PSqlError */ {} // PYR-27872

/////////////////// PYR-108648 - collect roundtrip stats /////////////////////
public: 
	void resetRoundtripCounter() { roundtripCounter = 0; };
	void incrementRountripCounter() { roundtripCounter++; }
	UINT32 getRoundtripCounter() const { return roundtripCounter; } 
private:
	UINT32 roundtripCounter = 0;
public:
	// PYR-112629 dbms that catch PDbFederatedDeadInternalConnectError exception should override this function to return 'true'
	virtual bool dbmSupportsRetryOnFederatedDeadConnection() const { return false; } // PYR-112629
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

// PYR-34474
#define RECONNECT_SLEEP		1000

template <class T>
void dbmConnectOnInit(T& dbManager, const char* fullFileName, const char* sectionName)
{
	int n = 0;
	for(; n < dbManager.getMaxDbReconnectAttempts(); ++n )
	{
		if ( dbManager.openDataStorage(fullFileName, sectionName) )
		{
			break;
		}
		PLog("dbmConnectOnInit: error, attempt #%d failed", n + 1 );
		PThread::sleep(RECONNECT_SLEEP);
	}
	if (n == dbManager.getMaxDbReconnectAttempts())
	{
		throw (PError("ERROR: Main data storage not open"));
	}
}
#endif //dbcommon_h_included
