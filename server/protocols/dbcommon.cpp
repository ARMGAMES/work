//dbcommon.cpp
#include	"dbcommon.h"
#include	"commssl.h"
#include	"dbmstmt.h"

#define REMAINING_IDS_THRESHOLD_DEFAULT	900000
#define REQUIRED_IDS_BUFFER_DEFAULT		1000000

void DatabaseManagerCommon::setAutoCommit( bool bOn )
{
	if(bOn == globalAutoCommitFlag)
		return;
	SQLRETURN retCode = SQLSetConnectAttr( 
		hdbc, 
		SQL_ATTR_AUTOCOMMIT, 
		bOn ? (SQLPOINTER)SQL_AUTOCOMMIT_ON : (SQLPOINTER)SQL_AUTOCOMMIT_OFF, //-V566
		0);
	checkDbcRetcode( retCode, hdbc, eCheckError ); //throws if error
	globalAutoCommitFlag = bOn; //if SQLSetConnectAttr() fails we do not get here
}

void	DatabaseManagerCommon::rollback()
{
	incrementRountripCounter(); // PYR-108648
	SQLRETURN retCode = SQLEndTran(SQL_HANDLE_DBC, hdbc, SQL_ROLLBACK);
	checkDbcRetcode( retCode, hdbc );
}

void	DatabaseManagerCommon::commit()
{
	incrementRountripCounter(); // PYR-108648
	SQLRETURN retCode = SQLEndTran(SQL_HANDLE_DBC, hdbc, SQL_COMMIT);
	checkDbcRetcode( retCode, hdbc );
}

// PYR-33265
class PLogCallback : public PLogFile::Callback
{
	PString applicationId;

public:
	PLogCallback(const char* applicationId_) : applicationId(applicationId_)
	{
		PLog("DB2 application id=%s", applicationId.c_str());
	}

	void opened() override
	{
		PLog("DB2 application id=%s", applicationId.c_str());
	}
};

// PYR-33265
void 	DatabaseManagerCommon::reportDB2ApplicationId()
{
	try
	{
		GetDB2ApplicationIdStmt getDB2ApplicationIdStmt(*this);
		getDB2ApplicationIdStmt.execute();
		bool found = getDB2ApplicationIdStmt.fetch();
		getDB2ApplicationIdStmt.closeCursor();
		if (found)
		{
			PLogSetCallback(new PLogCallback(getDB2ApplicationIdStmt.getApplicationId()));
		}
	}
	catch(const PError& err)
	{
		PLog("Error in DatabaseManager::reportDB2ApplicationId() - %s", err.why());
	}
}

bool	DatabaseManagerCommon::openDataStorage(const char* fullFileName, const char* sectionName)
{
	PLog("DatabaseManagerCommon::openDataStorage start");
	closeDataStorage();
	try
	{
		_openDataStorage(fullFileName, sectionName);
	}
	catch(const PError& err)
	{
		PLog("Error in DatabaseManager::openDataStorage() - %s", err.why());
		return false;
	}
	PLog("DatabaseManagerCommon::openDataStorage end");
	reportDB2ApplicationId(); // PYR-33265
	generator.prepareStatements( *this );
	return true;
}

void	DatabaseManagerCommon::_openDataStorage(const char* fullFileName, const char* sectionName)
{
	PLog("DatabaseManagerCommon::_openDataStorage start");
	PIniFile iniFile( fullFileName );

	PIniFile::Section* section = iniFile.getSection(sectionName);
	if (!section)
	{
		PString errStr;
		errStr.append("No [").append(sectionName).append("] section in ini file");
		throw( PError(errStr) );
	}

	const char* login;
	const char* password;
	const char* schema;

	dsName = section->getProperty("dsname","");
	login  = section->getProperty("login","");
	password = section->getProperty("password","");
	schema = section->getProperty("schema");
	int isolation = section->getIntProperty("isolation", 0);
	lockTimeout = section->getIntProperty( DB_LOCKTIMEOUT_PARAM_NAME, 0 ); // PYR-35295
	// PYR-49252
	defaultParallelismDegree = section->getProperty( DB_DEFAULT_PARAL_PARAM_NAME, "" );
	maxParallelismDegree = section->getProperty( DB_MAX_PARAL_PARAM_NAME, "" );
	PLog( DB_DEFAULT_PARAL_PARAM_NAME ":'%s' " DB_MAX_PARAL_PARAM_NAME ":'%s'", defaultParallelismDegree.c_str(), maxParallelismDegree.c_str() );

	schemaStr.assign(schema);
	maxDbReconnectAttempts = section->getIntProperty(DB_RECONNECT_ATTEMPTS_PARAM_NAME, 3600); // PYR-37068 - new value 3600
	maxDbDeadlockRetries = section->getIntProperty( DB_DEADLOCK_RETRIES_PARAM_NAME, 5 ); // PYR-37539
	PLog( DB_RECONNECT_ATTEMPTS_PARAM_NAME ":%d " DB_DEADLOCK_RETRIES_PARAM_NAME ":%d", maxDbReconnectAttempts, maxDbDeadlockRetries );

	if ( !*dsName.c_str() )  //  PYR-16309
	{
		PString errStr;
		errStr.append("Incorrect [").append(sectionName).
			append("] section in ini file (provide dsname, login, password )");
		throw( PError(errStr) );
	}

	readStmtFailureLoggingState(*section); // PYR-27855

	// PYR-20813
	// PYR-28457
	// allow any value
	forUpdateClause = section->getProperty("forUpdateClause", "FOR UPDATE WITH RS");
	
	PLog("... connecting to %s as %s", dsName.c_str(), login);
	PLog("... generator 'for update' clause '%s'", forUpdateClause.c_str());
	PLog("... locktimeout: %d", lockTimeout );

	PString decodedPwd;
	CommSSLSystemInfo::decodePasswordEx(decodedPwd, password);
	PLog( "Password decoded." );

	SQLRETURN  retcode;

	/*Allocate environment handle */
	retcode = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);

	if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) 
	{
		PLog( "SQL Env handle allocated. (%d)", (int)retcode );
		/* Set the ODBC version environment attribute */
		retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);  //-V566

		if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) 
		{
			PLog( "ODBC version is set to %d. (%d)", (int)SQL_OV_ODBC3, (int)retcode );
			/* Allocate connection handle */
			retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc);

			if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO) 
			{
				PLog( "Connection handle allocated. (%d)", (int)retcode );
				/* Set login timeout to 5 seconds. */
				retcode = SQLSetConnectAttr(hdbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0); //-V566
				PLog( "Login timeout set to 5. (%d)", (int)retcode );

				/* Connect to data source */
				retcode = SQLConnect(hdbc, (SQLCHAR*) dsName.c_str(), SQL_NTS,
					(SQLCHAR*) login, SQL_NTS,
					(SQLCHAR*) decodedPwd.c_str(), SQL_NTS);
				PLog( "Connected. (%d)", (int)retcode );

				if (retcode == SQL_SUCCESS || retcode == SQL_SUCCESS_WITH_INFO)
				{
					//connected ok, both henv and hdbc are valid
					if ( isolation )//change default isolation level
					{
						/* possible values are:
						1 = Read Uncommitted (Uncommitted read) 
						2 = Read Committed (Cursor stability) (default) 
						4 = Repeatable Read (Read Stability) 
						8 = Serializable (Repeatable read) 
						(see also SQL_TXN_* constants in "sql.h"
						*/
						retcode = SQLSetConnectAttr(hdbc, SQL_ATTR_TXN_ISOLATION, (SQLPOINTER)isolation, 0); //-V204
						PLog("Transaction isolation level was%s set to %d. (%d)", retcode ? " NOT" : "", isolation, (int)retcode);
					} 
					/// set auto commit = TRUE
					SQLRETURN retCode = SQLSetConnectAttr( 
						hdbc, 
						SQL_ATTR_AUTOCOMMIT, 
						(SQLPOINTER)SQL_AUTOCOMMIT_ON, //-V566
						0);
					PLog( "Auto commit set to true. (%d)", (int)retCode );
					checkDbcRetcode( retCode, hdbc, eCheckError ); //throws if error
					globalAutoCommitFlag = true; //if SQLSetConnectAttr() fails we do not get here
					PLog( "globalAutoCommitFlag = true." );
					// PYR-35295
					if( lockTimeout ) // change the default lock timeout value
					{
						setLockTimeout();
					}
					// PYR-49252
					if( defaultParallelismDegree.length() )
					{
						setDefaultParallelismDegree();
					}
				}
				else
				{
					SQLCHAR message[1024];
					SQLCHAR state[32];
					SQLINTEGER nativeErr;
					SQLSMALLINT len;
					SQLGetDiagRec(SQL_HANDLE_DBC, hdbc,	1, state, &nativeErr, message, sizeof( message ), &len); 
					PLog( "Got diag rec from hdbc." );
					SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
					PLog( "hdbc freed." );
					hdbc = 0;
					SQLFreeHandle(SQL_HANDLE_ENV, henv);
					henv = 0;
					PString err("Could not connect, retcode = ");
					err.appendInt(retcode);
					err.append( ", DiagRec: " );
					err.append( (const char*)message );
					throw PError(err);
				}
			}
			else
			{
				SQLFreeHandle(SQL_HANDLE_ENV, henv);
				henv = 0;
				PString err("Could not allocate connection handle, retcode = ");
				err.appendInt(retcode);
				throw PError(err);
			}
		}
		else
		{
			SQLFreeHandle(SQL_HANDLE_ENV, henv);
			henv = 0;
			PString err("SQLSetEnvAttr() failed, retcode = ");
			err.appendInt(retcode);
			throw PError(err);
		}
	}
	else 
	{
		PString err("Could not allocate environment handle, retcode = ");
		err.appendInt(retcode);
		throw PError(err);
	}
	// PYR-27418
	const PIniFile::Section* deplSection = iniFile.getSection(DEPLOYMENT_SECTION_NAME);
	if (deplSection)
	{
		PLog("reading [%s] size=%u", DEPLOYMENT_SECTION_NAME, deplSection->items.size());
		remainingIdsThreshold = deplSection->getIntProperty("remainingIdsThreshold", REMAINING_IDS_THRESHOLD_DEFAULT);
		requiredIdsBuffer = deplSection->getIntProperty("requiredIdsBuffer", REQUIRED_IDS_BUFFER_DEFAULT);
		useMasterGenerator = deplSection->getIntProperty("useMasterGenerator", 0) != 0;
	}
	else
	{
		PLog("no [%s] section in ini file", DEPLOYMENT_SECTION_NAME);
		remainingIdsThreshold = REMAINING_IDS_THRESHOLD_DEFAULT;
		requiredIdsBuffer = REQUIRED_IDS_BUFFER_DEFAULT;
		useMasterGenerator = false;
	}
	if( remainingIdsThreshold < 2 || remainingIdsThreshold >= requiredIdsBuffer )
	{
		PLog("ERROR: invalid values in configuration - 'remainingIdsThreshold' < 2 or 'remainingIdsThreshold' >= 'requiredIdsBuffer'. Correcting.");
		remainingIdsThreshold = REMAINING_IDS_THRESHOLD_DEFAULT;
		requiredIdsBuffer = REQUIRED_IDS_BUFFER_DEFAULT;
	}
	if( generator.getRangeIncrement() >= requiredIdsBuffer - remainingIdsThreshold )
	{
		PLog( "WARNING: rangeIncrement (%d) is less than minimum requested Ids buffer (%d).", generator.getRangeIncrement(), requiredIdsBuffer - remainingIdsThreshold );
	}
	PLog("remainingIdsThreshold=%d, requiredIdsBuffer=%d", remainingIdsThreshold, requiredIdsBuffer);
	PLog("useMasterGenerator=%s", useMasterGenerator ? "true" : "false");

	size_t generatorsSize;
	const DbmGenerator::Generator* const generators = getGenerators( generatorsSize );
	PASSERT( !generatorsSize || generators );
	for( size_t i = 0; i < generatorsSize; ++i )
	{
		addObjectNameToGenerator( generators[i].objectName, generators[i].local ); //-V522
	}
	if( useMasterGenerator && !useSharedIds() )
	{
		PASSERT(0);
	}
	// PYR-40317
	const char* const * const monotonicGenerators = getMonotonicGenerators( generatorsSize );
	PASSERT( !generatorsSize || monotonicGenerators );
	for( size_t i = 0; i < generatorsSize; ++i )
	{
		addMonotonicGenerator( monotonicGenerators[i] ); //-V522
	}

}

void DatabaseManagerCommon::closeDataStorage()
{
	generator.deleteStatements();
	if ( 0 != hdbc )
	{
		SQLDisconnect(hdbc); //pclint
		SQLFreeHandle(SQL_HANDLE_DBC, hdbc);
		hdbc = 0;
	}
	if ( 0 != henv )
	{
		SQLFreeHandle(SQL_HANDLE_ENV, henv);
		henv = 0;
	}
}



// PYR-37710
DatabaseManagerCommon::SqlErrDetail::SqlErrDetail(SQLRETURN retcode)
	: sqlRetCode(retcode)
	, haveToCheck(false)
{
	SqlState[0]= 0;
}

// PYR-37710
void DatabaseManagerCommon::SqlErrDetail::checkRetCode(SQLSMALLINT HandleType, SQLHSTMT hstmt, CheckModifier modifier)
{
	PASSERT(0 != hstmt);

	if ( eCheckNotSuccess == modifier )
	{
		haveToCheck = (sqlRetCode != SQL_SUCCESS);
	}
	else if ( eCheckError == modifier )
	{
		haveToCheck = (sqlRetCode == SQL_ERROR);
	}
	else if ( eCheckErrorOrInfo == modifier ) //-V547
	{
		haveToCheck = (sqlRetCode == SQL_ERROR || sqlRetCode == SQL_SUCCESS_WITH_INFO);
	}
	else	// modifier is insane!..
	{	
		throw PSqlError("Insane modifier in DatabaseManager::checkRetcode()");
	}
			
	if ( haveToCheck ) 
	{
		SQLCHAR		tmpSqlState[SqlStateSize], 
					Msg[SQL_MAX_MESSAGE_LENGTH]; //512 bytes
		SQLINTEGER	NativeError;
		SQLSMALLINT i, MsgLen;
		i = 1;
		while	
		( SQL_NO_DATA != (/*rc = */SQLGetDiagRec(HandleType, hstmt, i, tmpSqlState, &NativeError, Msg, sizeof(Msg), &MsgLen))) //pclint
		{
			// in CLI, tmpSqlState will be set to "00000" when function returns SQL_NO_DATA
			// in ODBC it is not changed when function returns SQL_NO_DATA
			memcpy(SqlState, tmpSqlState, sizeof(tmpSqlState));
			PString errorStr;
			errorStr.
				append((char *)SqlState).
				append(" ").
				appendInt(NativeError).
				append(" ").
				append((char*)Msg).
				append('|');
			sqlErr.append( errorStr );
			if (1 == i)
			{
				sqlStateLst.append((char *)SqlState);
			}
			else 
			{
				sqlStateLst.
					append('|').
					append((char *)SqlState);
			}
			i++;
			if( i > 20 ) //protection against insanity
				break;
		}
		// PLog("error checkRetcode ... %s", sqlErr.c_str());
		if( SQL_HANDLE_STMT == HandleType )
		{
			SQLCancel(hstmt);
		}
	}
}

// PYR-124995
const set<PString, PStringCmp> disconnect_err_codes = {
#ifdef POSTGRES
	"57P01",	//disconnect (connection to db is broken)
	"57P02",	//disconnect (connection to db is broken)
	"08S01",	//ms odbc driver: connection lost
	"HY000",	//ms odbc driver: connection lost
#else
	"40003",	//disconnect (connection to db is broken)
#endif
	"08003"		//PYR-22707: Connection is closed.
};

// PYR-124995
const set<PString, PStringCmp> deadlock_err_codes = {
#ifdef POSTGRES
	"40P01"		//SQL0911N The current transaction has been rolled back because of a deadlock or timeout.
#else
	"40001"		//SQL0911N The current transaction has been rolled back because of a deadlock or timeout.
#endif
};

// PYR-124995
const set<PString, PStringCmp> federated_conn_err_codes = {
#ifdef POSTGRES
	// not applicable to POSTGRES
#else
	"40506"		// PYR-112629 (not yet supported in SqlErrDetail)
#endif
};



// PYR-37710
void DatabaseManagerCommon::SqlErrDetail::raiseOnError() const
{
	if ( haveToCheck )
	{
		PLog("error checkRetcode ... %s", sqlErr.c_str());
		if (disconnect_err_codes.find((const char*)SqlState) != disconnect_err_codes.end()) // PYR-124995
		{
			throw PDbDisconnectError( sqlErr );
		}
		if (deadlock_err_codes.find((const char*)SqlState) != deadlock_err_codes.end()) // PYR-124995
		{
			throw PDbDeadlockError( sqlErr );
		}
		throw PSqlError( sqlErr, sqlStateLst );
	}
}


void DatabaseManagerCommon::checkRetcode(
		SQLSMALLINT HandleType,	
		SQLRETURN retcode,
		SQLHSTMT hstmt, 
		CheckModifier modifier // = DatabaseManager::eCheckNotSuccess
		) const
{
	PASSERT(0 != hstmt);

	bool haveToCheck = false; 

	if ( eCheckNotSuccess == modifier )
	{
		haveToCheck = (retcode != SQL_SUCCESS);
	}
	else if ( eCheckError == modifier )
	{
		haveToCheck = (retcode == SQL_ERROR);
	}
	else if ( eCheckErrorOrInfo == modifier ) //-V547
	{
		haveToCheck = (retcode == SQL_ERROR || retcode == SQL_SUCCESS_WITH_INFO);
	}
	else	// modifier is insane!..
	{	
		throw PSqlError("Insane modifier in DatabaseManager::checkRetcode()");
	}
			
	if ( haveToCheck ) 
	{
		SQLCHAR		SqlState[6] = "", 
					tmpSqlState[6], 
					Msg[SQL_MAX_MESSAGE_LENGTH]; //512 bytes
		SQLINTEGER	NativeError;
		SQLSMALLINT i, MsgLen;
		PString sqlErr;
		PString sqlStateLst;
		i = 1;
		while	
		( SQL_NO_DATA != (/*rc = */SQLGetDiagRec(HandleType, hstmt, i, tmpSqlState, &NativeError, Msg, sizeof(Msg), &MsgLen))) //pclint
		{
			// in CLI, tmpSqlState will be set to "00000" when function returns SQL_NO_DATA
			// in ODBC it is not changed when function returns SQL_NO_DATA
			memcpy(SqlState, tmpSqlState, sizeof(tmpSqlState));
			PString errorStr;
			errorStr.
				append((char *)SqlState).
				append(" ").
				appendInt(NativeError).
				append(" ").
				append((char*)Msg).
				append('|');
			sqlErr.append( errorStr );
			if (1 == i)
			{
				sqlStateLst.append((char *)SqlState);
			}
			else 
			{
				sqlStateLst.
					append('|').
					append((char *)SqlState);
			}
			i++;
			if( i > 20 ) //protection against insanity
				break;
		}
		PLog("error checkRetcode ... %s", sqlErr.c_str());
		if( SQL_HANDLE_STMT == HandleType )
		{
			SQLCancel(hstmt);
		}

		if (disconnect_err_codes.find((const char*)SqlState) != disconnect_err_codes.end()) // PYR-124995
		{
			throw PDbDisconnectError( sqlErr );
		}
		if (deadlock_err_codes.find((const char*)SqlState) != deadlock_err_codes.end()) // PYR-124995
		{
			throw PDbDeadlockError( sqlErr );
		}
		if (dbmSupportsRetryOnFederatedDeadConnection() && federated_conn_err_codes.find((const char*)SqlState) != federated_conn_err_codes.end()) // PYR-124995
		{
			throw PDbFederatedDeadInternalConnectError(sqlErr); // SQLSTATE=40506 dead connection between DB_s in federated (STAR) schema
		}
		throw PSqlError( sqlErr, sqlStateLst );
	}
}


//virtual 
const char* DatabaseManagerCommon::getTableName(eTableNames tableNameEnum) const // PYR-26147
{
	PASSERT5(0);
	return "";
}

void DatabaseManagerCommon::addObjectNameToGenerator(const char* objectName, bool local) // PYR-27418
{
	generator.addObjectName( objectName, local );
}

// PYR-40317
void DatabaseManagerCommon::addMonotonicGenerator( const char* objectName )
{
	generator.addMonotonicObjectName( objectName );
}

void DatabaseManagerCommon::readStmtFailureLoggingState(const PIniFile::Section& section) // PYR-27855
{
#ifdef _DEBUG
	const int defaultValue = eStmtFailureLogging_All;
#else
	const int defaultValue = eStmtFailureLogging_Off;
#endif
	stmtFailureLogging = (eStmtFailureLogging)section.getIntProperty("stmtFailureLogging", defaultValue);
	PLog("stmtFailureLogging=%d", stmtFailureLogging);
}

bool DatabaseManagerCommon::isStmtFalureLoggingOn(const PString& sqlState) const // PYR-27855
{
	return (stmtFailureLogging == eStmtFailureLogging_All) ||
		(  (stmtFailureLogging == eStmtFailureLogging_AllButAllowedForOlap) && !errorStateAllowedForOlap(sqlState));
}

void DatabaseManagerCommon::setLockTimeout() // can throw
{
	PString exec = "SET CURRENT LOCK TIMEOUT ";
	exec.appendInt( lockTimeout );
	DbmStatement stmt( *this );
	stmt.execDirect( exec, DatabaseManagerCommon::eCheckNotSuccess );
}
// PYR-49252
void DatabaseManagerCommon::setParallelismDegree( const char* parallelismDegree) // can throw
{
	// #DBA_REVIEWED #PYR-49252 VladimirB 2017.build01
	PString exec = "SET CURRENT DEGREE = '";
	exec.append( parallelismDegree ).append( "'" );
	DbmStatement stmt( *this );
	PLog( " %s", exec.c_str() );
	stmt.execDirect( exec, DatabaseManagerCommon::eCheckNotSuccess );
}
void DatabaseManagerCommon::setDefaultParallelismDegree() // can throw
{
	PLog( "Setting parallelism degree to default:" );
	setParallelismDegree( defaultParallelismDegree );
}

void DatabaseManagerCommon::setMaxParallelismDegree() // can throw
{
	PLog( "Setting parallelism degree to max:" );
	setParallelismDegree( maxParallelismDegree );
}

bool DatabaseManagerCommon::setDefaultParallelismDegreeNoThrow()
{
	try
	{
		if (defaultParallelismDegree.length() == 0)
		{
			PLog("setDefaultParallelismDegreeNoThrow failed, undefined defaultParallelismDegree");
			return false;
		}
		setDefaultParallelismDegree();
		return true;
	}
	catch (const PSqlError& er)
	{
		PLog("setDefaultParallelismDegreeNoThrow failed, %d %s", er.code(), er.why());
		return false;
	}
}

bool DatabaseManagerCommon::setMaxParallelismDegreeNoThrow()
{
	try
	{
		if (maxParallelismDegree.length() == 0)
		{
			PLog("setMaxParallelismDegreeNoThrow failed, undefined maxParallelismDegree");
			return false;
		}
		if (defaultParallelismDegree.length() == 0)
		{
			return false;
		}
		setMaxParallelismDegree();
		return true;
	}
	catch (const PSqlError& er)
	{
		PLog("setMaxParallelismDegreeNoThrow failed %d %s", er.code(), er.why());
		return false;
	}
}



