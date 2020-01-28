///dbmstmt.cpp
#include "dbmstmt.h"

DbmStatement::DbmStatement( DatabaseManagerCommon& manager_ )
		: manager(manager_)
{
	prepared = false;
	hstmt = 0;
	manager.checkDbcRetcode(
			SQLAllocHandle(SQL_HANDLE_STMT, manager.getHdbc(), &hstmt ),
			manager.getHdbc(),
			DatabaseManagerCommon::eCheckNotSuccess
		);
	autoParamOrdinal = false;
	autoColOrdinal = false;
	currentParamOrdinal = 0;
	currentColOrdinal = 0;
}

/*virtual*/ DbmStatement::~DbmStatement()
{
	freeStmt();
}

void DbmStatement::checkRetcode( SQLRETURN retcode, DatabaseManagerCommon::CheckModifier modifier )
{
	manager.checkStmtRetcode(retcode, hstmt, modifier);
}

void DbmStatement::checkRetcodeWithTrace( SQLRETURN retcode, DatabaseManagerCommon::CheckModifier modifier ) // PYR-27855
{
	try
	{
		checkRetcode(retcode, modifier);
	}
	catch (const PDbDeadlockError& /*er*/) //-V2010
	{
		if (manager.getStmtFailureLogging() != DatabaseManagerCommon::eStmtFailureLogging_Off)
		{
			logStmtTextWithParams();
		}
		throw;
	}
	catch (const PDbDisconnectError& /*er*/) //-V2010
	{
		if (manager.getStmtFailureLogging() != DatabaseManagerCommon::eStmtFailureLogging_Off)
		{
			logStmtTextWithParams();
		}
		throw;
	}
	catch (const PDbFederatedDeadInternalConnectError& /*er*/)
	{
		if (manager.getStmtFailureLogging() != DatabaseManagerCommon::eStmtFailureLogging_Off)
		{
			logStmtTextWithParams();
		}
		throw;
	}
	catch (const PSqlError& er)
	{
		if (manager.isStmtFalureLoggingOn(er.sqlState))
		{
			logStmtTextWithParams();
		}
		throw;
	}
}

void DbmStatement::logStmtTextWithParams() const // PYR-27855 + PYR-29476
{
	PLog("StmtText %s", typeid(*this).name());
	if (stmtText.length() || paramsMap.size())
	{
		PDump(stmtText.length() ? stmtText.c_str() : "(code)");
		PString logStr;
		logStr.assign("\n" "Params[").appendUint(paramsMap.size()).append("]\n");
		paramsToString(logStr);
		PDump(logStr);
	}
	else
	{
		PLog("No Failure Logging for stmt");
	}
}

void DbmStatement::freeStmt()
{
	if (0 != hstmt)
	{
		SQLFreeHandle( SQL_HANDLE_STMT, hstmt );
		hstmt = 0;
	}
}

void	DbmStatement::execDirect( const SQLCHAR* statement, DatabaseManagerCommon::CheckModifier modifier/* = DatabaseManagerCommon::eCheckNotSuccess*/ )
{
	manager.incrementRountripCounter(); // PYR-108648
	SQLINTEGER len = strlen((const char*)statement);
	checkRetcode(SQLExecDirect(hstmt, const_cast<SQLCHAR*>(statement), len), modifier );
}

void	DbmStatement::execDirect( const char* statement, DatabaseManagerCommon::CheckModifier modifier/* = DatabaseManagerCommon::eCheckNotSuccess*/ )
{
	manager.incrementRountripCounter(); // PYR-108648
	SQLINTEGER len = strlen(statement);
	checkRetcode(SQLExecDirect(hstmt, (SQLCHAR*)statement, len), modifier );
}

// will return false if no rows were affected
bool	DbmStatement::execDirectUpdateDelete( const char* statement, DatabaseManagerCommon::CheckModifier modifier /*= DatabaseManagerCommon::eCheckNotSuccess*/ )
{
	manager.incrementRountripCounter(); // PYR-108648
	SQLINTEGER len = strlen(statement);
	SQLRETURN ret = SQLExecDirect(hstmt, (SQLCHAR*)statement, len);
	if ( SQL_NO_DATA == ret )
	{
		return false;
	}
	checkRetcode(ret, modifier);
	
	return true;
}

// DB2 doc: SQLRowCount() returns the number of rows in a table that were affected by an UPDATE, INSERT, DELETE, or MERGE statement.
// can NOT be used for SELECT
SQLLEN DbmStatement::getRowCount()
{
	SQLLEN result;
	checkRetcode( SQLRowCount( hstmt, &result), DatabaseManagerCommon::eCheckNotSuccess);
	return result;
}

// PYR-108648
const UINT64 fetchLocalTickThreshold = 500000;
// The fetch call may or may not perform a roundtrip to the database. The driver does not report this level of detail.
// A good approximation is to measure the time of execution of SQLFetch() call: if roundtrip was performed, it executes an order of magnitude slower.
// The roundtrip to a local DB one the same machine takes ~2,000,000 ticks
// The roundtrip to a remote DB in the same network takes ~5,000,000 ticks
// The fetch without roundtrip (from a local buffer) takes ~5,000..200,000 ticks

bool DbmStatement::fetch()
{
	// PYR-108648
	UINT64 hiresStart = getHighResCounter();
	SQLRETURN retcode = SQLFetch(hstmt);
	INT64 hiresElapsed = getHighResCounter() - hiresStart;
	if (hiresElapsed > fetchLocalTickThreshold)
		manager.incrementRountripCounter();

	if ( SQL_NO_DATA == retcode )
	{
		return false;
	}
	checkRetcodeWithTrace(retcode, DatabaseManagerCommon::eCheckNotSuccess);
	return true;
}

bool DbmStatement::fetchIgnoreWarning()
{
	// PYR-108648
	UINT64 hiresStart = getHighResCounter();
	SQLRETURN retcode = SQLFetch(hstmt);
	INT64 hiresElapsed = getHighResCounter() - hiresStart;
	if (hiresElapsed > fetchLocalTickThreshold)
		manager.incrementRountripCounter();

	if ( SQL_NO_DATA == retcode )
	{
		return false;
	}
	checkRetcodeWithTrace(retcode, DatabaseManagerCommon::eCheckError);
	return true;
}

void	DbmStatement::setTimeOut(SQLINTEGER seconds)
{
	SQLRETURN rc = SQLSetStmtAttr(hstmt, SQL_ATTR_QUERY_TIMEOUT, (SQLPOINTER) seconds, 0); //-V204
	checkRetcode(rc, DatabaseManagerCommon::eCheckNotSuccess);
}

void DbmStatement::bindCol( 
	SQLUSMALLINT	colOrdinal, 
	SQLSMALLINT		targetType, 
	SQLPOINTER		targetValuePtr,
	SQLINTEGER		bufferLength,
	SQLLEN*		StrLen_or_IndPtr)
{
	checkRetcode(SQLBindCol(hstmt, colOrdinal, targetType, targetValuePtr, bufferLength, StrLen_or_IndPtr), DatabaseManagerCommon::eCheckNotSuccess);
}

void DbmStatement::bindSmallIntCol(
	SQLUSMALLINT	colOrdinal, 
	SQLSMALLINT*	targetValuePtr,
	SQLLEN*		strLen_or_IndPtr
	)
{
	bindCol( colOrdinal, SQL_SMALLINT, targetValuePtr, 0, strLen_or_IndPtr);
}

void DbmStatement::bindIntCol(
	SQLUSMALLINT	colOrdinal, 
	SQLINTEGER*		targetValuePtr,
	SQLLEN*		strLen_or_IndPtr
	)
{
	bindCol( colOrdinal, SQL_INTEGER, targetValuePtr, 0, strLen_or_IndPtr);
}

void DbmStatement::bindBigIntCol(
	SQLUSMALLINT	colOrdinal, 
	SQLBIGINT*		targetValuePtr,
	SQLLEN*		strLen_or_IndPtr
	)
{
	bindCol( colOrdinal, SQL_C_SBIGINT, targetValuePtr, 0, strLen_or_IndPtr);
}


void DbmStatement::bindStrCol(
	SQLUSMALLINT	colOrdinal, 
	SQLPOINTER		targetValuePtr,
	SQLINTEGER		bufferLength,
	SQLLEN*		StrLen_or_IndPtr)
{
	bindCol( colOrdinal, SQL_C_CHAR, targetValuePtr, bufferLength, StrLen_or_IndPtr);
}

void DbmStatement::bindTimeStampCol(
	SQLUSMALLINT			colOrdinal,
	SQL_TIMESTAMP_STRUCT*	ts, 
	SQLLEN*				StrLen_or_IndPtr
	)
{
	bindCol( colOrdinal, SQL_C_TYPE_TIMESTAMP, ts, sizeof(SQL_TIMESTAMP_STRUCT), StrLen_or_IndPtr );
}

void DbmStatement::bindDateCol(
	SQLUSMALLINT			colOrdinal,
	SQL_DATE_STRUCT*		ds, 
	SQLLEN*				StrLen_or_IndPtr
	)
{
	bindCol( colOrdinal, SQL_C_TYPE_DATE, ds, sizeof(SQL_DATE_STRUCT), StrLen_or_IndPtr );
}

void DbmStatement::prepare(const PString& statement)
{
	prepare(statement.c_str());
	if (manager.getStmtFailureLogging() != DatabaseManagerCommon::eStmtFailureLogging_Off)
	{
		stmtText = statement;
	}
}

void DbmStatement::prepare(const char* statement)
{
	checkRetcode(SQLPrepare( hstmt, (SQLCHAR*)statement, SQL_NTS ), DatabaseManagerCommon::eCheckNotSuccess);
	prepared = true;
}

void DbmStatement::bindInputParam( // PYR-27855
	SQLUSMALLINT    ordinal, // >=1
	SQLSMALLINT     valueType, // C
	SQLSMALLINT     parameterType, // SQL
	SQLULEN         columnSize, // non-zero for strings
	SQLPOINTER      valuePtr,
	SQLLEN *        strLen_or_indPtr)
{
	SQLRETURN retCode = SQLBindParameter(hstmt, ordinal, SQL_PARAM_INPUT, valueType, parameterType, columnSize, 0/*decimalDigits*/, valuePtr, 0/*bufferLength*/, strLen_or_indPtr);
	checkRetcode(retCode, DatabaseManagerCommon::eCheckNotSuccess);

	if (manager.getStmtFailureLogging() != DatabaseManagerCommon::eStmtFailureLogging_Off)
	{
		StmtParam stmtParam(valueType, parameterType, valuePtr, strLen_or_indPtr);
		pair<map<int,StmtParam>::iterator,bool> p = paramsMap.insert(make_pair(ordinal, stmtParam));
		if (!p.second)
		{
			// PTRACE5(" rebind %d", ordinal);
			p.first->second = stmtParam; // update
		}
	}
}

void DbmStatement::bindIntParam( int ordinal, SQLINTEGER* valuePtr, SQLLEN* indPtr )
{
	*indPtr = 0;
	bindInputParam(ordinal, SQL_INTEGER, SQL_INTEGER, 0, valuePtr, indPtr);
}

void DbmStatement::bindBigIntParam( int ordinal, SQLBIGINT* valuePtr, SQLLEN* indPtr )
{
	*indPtr = 0;
	bindInputParam(ordinal, SQL_C_SBIGINT, SQL_INTEGER, 0, valuePtr, indPtr);
}

void DbmStatement::bindSmallIntParam( int ordinal, SQLSMALLINT* valuePtr, SQLLEN* indPtr )
{
	*indPtr = 0;
	bindInputParam(ordinal, SQL_SMALLINT, SQL_INTEGER, 0, valuePtr, indPtr);
}

void DbmStatement::bindTimeParam( int ordinal, SQL_TIMESTAMP_STRUCT* valuePtr, SQLLEN* indPtr )
{
	*indPtr = 0;
	bindInputParam(ordinal, SQL_C_TYPE_TIMESTAMP, SQL_TYPE_TIMESTAMP, 0, valuePtr, indPtr);
}

void DbmStatement::bindDateParam( int ordinal, SQL_DATE_STRUCT* valuePtr, SQLLEN* indPtr )
{
	*indPtr = 0;
	bindInputParam(ordinal, SQL_C_TYPE_DATE, SQL_TYPE_DATE, 0, valuePtr, indPtr);
}

void DbmStatement::bindStrParam( int ordinal, SQLINTEGER buffLen, SQLCHAR* valuePtr, SQLLEN* strLen )
{
	*strLen = SQL_NTS;
	bindInputParam(ordinal, SQL_C_CHAR, SQL_CHAR, buffLen, valuePtr, strLen );
}

void DbmStatement::bindBinParam( int ordinal, SQLLEN* cbParam )
{
	// please note that DBM_SQL_BLOB is not ODBC constant
	// and is user here to work exclusively with DB2.
	// Try to replace it with SQL_LONGVARBINARY when working with other SQL server
	bindInputParam(ordinal, SQL_C_BINARY, DBM_SQL_BLOB, 0, (SQLPOINTER) ordinal, cbParam ); //-V204
}

void DbmStatement::initAndBindXmlParam( const PString& xmlStrUtf8, int ordinal, PSqlXml& sqlXml )
{
	sqlXml.init(xmlStrUtf8);
	bindInputParam(ordinal, SQL_C_BINARY, SQL_XML, 0, (SQLPOINTER)sqlXml.c_str(), &sqlXml.cb);
}

void DbmStatement::bindNullXmlParam( int ordinal, PSqlXml& sqlXml )
{
	sqlXml.clearAndSetNull(); // sets cb to SQL_NULL_DATA
	bindInputParam(ordinal, SQL_C_BINARY, SQL_XML, 0, (SQLPOINTER)sqlXml.c_str(), &sqlXml.cb);
}

SQLRETURN	DbmStatement::paramData(SQLPOINTER ValuePtr)
{
	return SQLParamData(hstmt, &ValuePtr);
}

SQLRETURN	DbmStatement::putData( const SQLPOINTER Data, SQLINTEGER cbData )
{
	return SQLPutData(hstmt, (SQLPOINTER)Data, cbData);
}

void DbmStatement::execute()
{
	if ( !prepared )
		throw(PSqlError("execute ... Attempt to execute unprepared statement!"));

	manager.incrementRountripCounter(); // PYR-108648
	checkRetcodeWithTrace(SQLExecute( hstmt ), DatabaseManagerCommon::eCheckNotSuccess);
}

SQLRETURN  DbmStatement::executeRet()
{
	if ( !prepared )
		throw(PSqlError("execute ... Attempt to execute unprepared statement!"));

	manager.incrementRountripCounter(); // PYR-108648
	return SQLExecute( hstmt );
}

bool DbmStatement::execUpdateDelete()
{
	if ( !prepared )
		throw(PSqlError("execUpdateDelete ... Attempt to execute unprepared statement!"));

	manager.incrementRountripCounter(); // PYR-108648
	SQLRETURN ret = SQLExecute(hstmt);
	if ( SQL_NO_DATA == ret )
	{
		return false;
	}
	checkRetcodeWithTrace(ret, DatabaseManagerCommon::eCheckNotSuccess);
	return true;
}

bool DbmStatement::execConditionalInsert()
{
	// to be used only with LZ or AR approval
	// written for the statement of the following kind:
	// insert into T1 (PK, Data) select ?,? from sysibm.sysdummy1 where not exists (select 1 from T1 where PK=?)
	return execUpdateDelete();
}

// PYR-37710
bool DbmStatement::execInsertIgnoreDuplicate()
{
	if ( !prepared )
		throw(PSqlError("execInsertIgnoreDuplicate ... Attempt to execute unprepared statement!"));

	manager.incrementRountripCounter(); // PYR-108648
	SQLRETURN ret = SQLExecute(hstmt);

	try
	{
		DatabaseManagerCommon::SqlErrDetail detail(ret);
		detail.checkRetCode(SQL_HANDLE_STMT, hstmt, DatabaseManagerCommon::eCheckNotSuccess);
		if (detail.hasError())
		{
			if (0 == strcmp((char*)detail.SqlState, DUPLICATE_KEY_ERROR_STR))
			{
				return false;
			}
			detail.raiseOnError();
		}
	}
	catch (const PDbDeadlockError& /*er*/) //-V2010
	{
		if (manager.getStmtFailureLogging() != DatabaseManagerCommon::eStmtFailureLogging_Off)
		{
			logStmtTextWithParams();
		}
		throw;
	}
	catch (const PDbDisconnectError& /*er*/) //-V2010
	{
		if (manager.getStmtFailureLogging() != DatabaseManagerCommon::eStmtFailureLogging_Off)
		{
			logStmtTextWithParams();
		}
		throw;
	}
	catch (const PDbFederatedDeadInternalConnectError& /*er*/)
	{
		if (manager.getStmtFailureLogging() != DatabaseManagerCommon::eStmtFailureLogging_Off)
		{
			logStmtTextWithParams();
		}
		throw;
	}
	catch (const PSqlError& er)
	{
		if (manager.isStmtFalureLoggingOn(er.sqlState))
		{
			logStmtTextWithParams();
		}
		throw;
	}

	return true;
}

void	DbmStatement::closeCursor()
{
	manager.incrementRountripCounter(); // PYR-108648
	checkRetcode(SQLCloseCursor(hstmt), DatabaseManagerCommon::eCheckNotSuccess);
}

void DbmStatement::getBlob( SQLINTEGER ordinal, PBlock& ret )
{
	SQLCHAR   buffer[5000];
	SQLINTEGER numBytes;
	SQLLEN bufferLen;
	SQLRETURN rc;
	while ((rc = SQLGetData
		(hstmt, ordinal, SQL_C_BINARY, buffer, sizeof(buffer), &bufferLen)) != SQL_NO_DATA)
	{
		checkRetcode(rc, DatabaseManagerCommon::eCheckError);
		numBytes = 
			(bufferLen > 5000) || (bufferLen == SQL_NO_TOTAL) 
			? 5000 : bufferLen;
		memcpy( ret.append( numBytes ), buffer, numBytes );
	}
}

/*static*/ void DbmStatement::prepareOdbcSafeString(PString& safeStr, const char* inStr)
{
	safeStr.assign("");
	int len = strlen(inStr);
	for(int n = 0; n < len; n++)
	{
		if (inStr[n] == '\'')
		{
			safeStr.append("''");
		}
		else
		{
			safeStr.append(inStr[n]);
		}
	}
}

/*static*/ void DbmStatement::prepareInString(const vector<PString>& inVector, PString& inString)
{
	// select col1 from table(values('boba'),('Lobo'),('Dmitry'))as q1(col1)
	// this is a vendor-specific syntax to 
	inString.assign("select col1 from table(values");
	for( int n = 0; n < inVector.size(); n++ )
	{
		PString oneString;
		prepareOdbcSafeString(oneString, inVector[n]);
		if(0 != n) //not the first iteration
			inString.append(',');
		inString.
			append("('").
			append(oneString).
			append("')");
	}
	inString.append(")as q1(col1)");
}

/*static*/ void DbmStatement::prepareInString(const vector<const char*>& inVector, PString& inString)
{
	// select col1 from table(values('boba'),('Lobo'),('Dmitry'))as q1(col1)
	// this is a vendor-specific syntax to 
	inString.assign("select col1 from table(values");
	for( int n = 0; n < inVector.size(); n++ )
	{
		PString oneString;
		prepareOdbcSafeString(oneString, inVector[n]);
		if(0 != n) //not the first iteration
			inString.append(',');
		inString.
			append("('").
			append(oneString).
			append("')");
	}
	inString.append(")as q1(col1)");
}

/*static*/ void DbmStatement::prepareInString(const vector<UINT32>& inVector, PString& inString)
{
	// select col1 from table(values(1),(23),(456))as q1(col1)
	// this is a vendor-specific syntax to 
	inString.assign("select col1 from table(values");
	for( int n = 0; n < inVector.size(); n++ )
	{
		if(0 != n) //not the first iteration
			inString.append(',');
		inString.
			append("(").
			appendUint(inVector[n]).
			append(")");
	}
	inString.append(")as q1(col1)");
}

/*static*/ void DbmStatement::prepareInString(const vector<UINT64>& inVector, PString& inString)
{
	// select col1 from table(values('boba'),('Lobo'),('Dmitry'))as q1(col1)
	// this is a vendor-specific syntax to 
	inString.assign("select col1 from table(values");
	for( int n = 0; n < inVector.size(); n++ )
	{
		if(0 != n) //not the first iteration
			inString.append(',');
		inString.
			append("(").
			appendUint64(inVector[n]).
			append(")");
	}
	inString.append(")as q1(col1)");
}

/*static*/ void DbmStatement::prepareInStringParams( const size_t n, PString& inString )
{
	// #DBA_REVIEWED #PYR-63589 NataliaV 2018.BUILD.02
	// select col1 from table(values(?),(?),(?))as q1(col1)
	// this is a vendor-specific syntax to
	inString.assign( "select col1 from table(values" );
	for( size_t i = 0; i < n; ++i )
	{
		inString.append( ( i == 0 ? "(?)" : ",(?)" ) );
	}
	inString.append( ")as q1(col1)" );
}

//
// Support for SqlValue-derived classes
//
void DbmStatement::bindParam( int ordinal, PSqlSmallInt& sqlValue )
{
	ensureNotAutoParamOrdinal();
	bindSmallIntParam( ordinal, &sqlValue.value, &sqlValue.cb );
}

void DbmStatement::bindParam( int ordinal, PSqlInt& sqlValue )
{
	ensureNotAutoParamOrdinal();
	bindIntParam( ordinal, &sqlValue.value, &sqlValue.cb );
}

void DbmStatement::bindParam( int ordinal, PSqlBigInt& sqlValue )
{
	ensureNotAutoParamOrdinal();
	bindBigIntParam( ordinal, &sqlValue.value, &sqlValue.cb );
}

void DbmStatement::bindParam( int ordinal, PSqlTimestamp& sqlValue )
{
	ensureNotAutoParamOrdinal();
	bindTimeParam( ordinal, &sqlValue.value, &sqlValue.cb );
}

void DbmStatement::bindParam( int ordinal, PSqlDate& sqlValue )
{
	ensureNotAutoParamOrdinal();
	bindDateParam( ordinal, &sqlValue.value, &sqlValue.cb );
}
// PYR-46890
void DbmStatement::bindParam( int ordinal, PSqlTournamentId& sqlValue )
{
	ensureNotAutoParamOrdinal();
	bindBigIntParam( ordinal, &sqlValue.value, &sqlValue.cb );
}
// PYR-69538
void DbmStatement::bindParam( int ordinal, PSqlBigIntAsUInt32& sqlValue )
{
	ensureNotAutoParamOrdinal();
	bindBigIntParam(ordinal, &sqlValue.value, &sqlValue.cb);
}

void DbmStatement::bindCol( int ordinal, PSqlSmallInt& sqlValue )
{
	ensureNotAutoColOrdinal();
	bindSmallIntCol( ordinal, &sqlValue.value, &sqlValue.cb );
}

void DbmStatement::bindCol( int ordinal, PSqlInt& sqlValue )
{
	ensureNotAutoColOrdinal();
	bindIntCol( ordinal, &sqlValue.value, &sqlValue.cb );
}

void DbmStatement::bindCol( int ordinal, PSqlBigInt& sqlValue )
{
	ensureNotAutoColOrdinal();
	bindBigIntCol( ordinal, &sqlValue.value, &sqlValue.cb );
}

void DbmStatement::bindCol( int ordinal, PSqlTimestamp& sqlValue )
{
	ensureNotAutoColOrdinal();
	bindTimeStampCol( ordinal, &sqlValue.value, &sqlValue.cb );
}

void DbmStatement::bindCol( int ordinal, PSqlDate& sqlValue )
{
	ensureNotAutoColOrdinal();
	bindDateCol( ordinal, &sqlValue.value, &sqlValue.cb );
}
// PYR-46890
void DbmStatement::bindCol( int ordinal, PSqlTournamentId& sqlValue )
{
	ensureNotAutoColOrdinal();
	bindBigIntCol( ordinal, &sqlValue.value, &sqlValue.cb );
}
// PYR-69538
void DbmStatement::bindCol( int ordinal, PSqlBigIntAsUInt32& sqlValue )
{
	ensureNotAutoColOrdinal();
	bindBigIntCol( ordinal, &sqlValue.value, &sqlValue.cb );
}
//
// Binding mode enforcement
//
void DbmStatement::ensureFirstParam()
{
	if (currentParamOrdinal != 0)
	{
		PLog("ensureFirstParam '%s' currentParamOrdinal=%d", typeid(*this).name(), currentParamOrdinal);
		throw PSqlError("Param binding has already started");
	}
	else
		autoParamOrdinal = true;
}

void DbmStatement::ensureNextParam() const
{
	if (currentParamOrdinal < 1)
	{
		PLog("ensureNextParam '%s' currentParamOrdinal=%d", typeid(*this).name(), currentParamOrdinal);
		throw PSqlError("bindFirstParam has not been called");
	}
}

void DbmStatement::ensureNotAutoParamOrdinal()
{
	if(autoParamOrdinal)
		throw PSqlError("Explicit param ordinal used in auto-ordinal mode");
	else
		currentParamOrdinal = -1; // To ensure no auto-ordinal mode
}

void DbmStatement::ensureFirstCol()
{
	if(currentColOrdinal != 0)
		throw PSqlError("Column binding has already started");
	else
		autoColOrdinal = true;
}

void DbmStatement::ensureNextCol() const
{
	if(currentColOrdinal < 1)
		throw PSqlError("bindFirstCol has not been called");
}

void DbmStatement::ensureNotAutoColOrdinal()
{
	if(autoColOrdinal)
		throw PSqlError("Explicit column ordinal used in auto-ordinal mode");
	else
		currentColOrdinal = -1; // To ensure no auto-ordinal mode
}

//
// Auto-ordinal param mode binding
//
void DbmStatement::bindFirstParam(PSqlInt& sqlValue )
{
	ensureFirstParam();
	bindIntParam( ++currentParamOrdinal, &sqlValue.value, &sqlValue.cb );
}

void DbmStatement::bindFirstParam(PSqlBigInt& sqlValue )
{
	ensureFirstParam();
	bindBigIntParam( ++currentParamOrdinal, &sqlValue.value, &sqlValue.cb );
}

void DbmStatement::bindFirstParam(PSqlSmallInt& sqlValue )
{
	ensureFirstParam();
	bindSmallIntParam( ++currentParamOrdinal, &sqlValue.value, &sqlValue.cb );
}

void DbmStatement::bindFirstParam( PSqlTimestamp& sqlValue )
{
	ensureFirstParam();
	bindTimeParam( ++currentParamOrdinal, &sqlValue.value, &sqlValue.cb );
}

void DbmStatement::bindFirstParam( PSqlDate& sqlValue )
{
	ensureFirstParam();
	bindDateParam( ++currentParamOrdinal, &sqlValue.value, &sqlValue.cb );
}
// PYR-46890
void DbmStatement::bindFirstParam( PSqlTournamentId& sqlValue )
{
	ensureFirstParam();
	bindBigIntParam( ++currentParamOrdinal, &sqlValue.value, &sqlValue.cb );
}

// PYR-69531
void DbmStatement::bindFirstParam( PSqlBigIntAsUInt32& sqlValue )
{
	ensureFirstParam();
	bindBigIntParam(++currentParamOrdinal, &sqlValue.value, &sqlValue.cb);
}

void DbmStatement::bindNextParam( PSqlInt& sqlValue )
{
	ensureNextParam();
	bindIntParam( ++currentParamOrdinal, &sqlValue.value, &sqlValue.cb );
}

void DbmStatement::bindNextParam( PSqlBigInt& sqlValue )
{
	ensureNextParam();
	bindBigIntParam( ++currentParamOrdinal, &sqlValue.value, &sqlValue.cb );
}

void DbmStatement::bindNextParam( PSqlSmallInt& sqlValue )
{
	ensureNextParam();
	bindSmallIntParam( ++currentParamOrdinal, &sqlValue.value, &sqlValue.cb );
}

void DbmStatement::bindNextParam( PSqlTimestamp& sqlValue )
{
	ensureNextParam();
	bindTimeParam( ++currentParamOrdinal, &sqlValue.value, &sqlValue.cb );
}

void DbmStatement::bindNextParam( PSqlDate& sqlValue )
{
	ensureNextParam();
	bindDateParam( ++currentParamOrdinal, &sqlValue.value, &sqlValue.cb );
}
// PYR-46890
void DbmStatement::bindNextParam( PSqlTournamentId& sqlValue )
{
	ensureNextParam();
	bindBigIntParam( ++currentParamOrdinal, &sqlValue.value, &sqlValue.cb );
}
// PYR-69531
void DbmStatement::bindNextParam( PSqlBigIntAsUInt32& sqlValue )
{
	ensureNextParam();
	bindBigIntParam( ++currentParamOrdinal, &sqlValue.value, &sqlValue.cb );
}
//
// Auto-ordinal column mode binding
//
void DbmStatement::bindFirstCol(PSqlInt& sqlValue )
{
	ensureFirstCol();
	bindIntCol( ++currentColOrdinal, &sqlValue.value, &sqlValue.cb );
}

void DbmStatement::bindFirstCol(PSqlBigInt& sqlValue )
{
	ensureFirstCol();
	bindBigIntCol( ++currentColOrdinal, &sqlValue.value, &sqlValue.cb );
}

void DbmStatement::bindFirstCol(PSqlSmallInt& sqlValue )
{
	ensureFirstCol();
	bindSmallIntCol( ++currentColOrdinal, &sqlValue.value, &sqlValue.cb );
}

void DbmStatement::bindFirstCol( PSqlTimestamp& sqlValue )
{
	ensureFirstCol();
	bindTimeStampCol( ++currentColOrdinal, &sqlValue.value, &sqlValue.cb );
}

void DbmStatement::bindFirstCol( PSqlDate& sqlValue )
{
	ensureFirstCol();
	bindDateCol( ++currentColOrdinal, &sqlValue.value, &sqlValue.cb );
}
// PYR-46890
void DbmStatement::bindFirstCol( PSqlTournamentId& sqlValue )
{
	ensureFirstCol();
	bindBigIntCol( ++currentColOrdinal, &sqlValue.value, &sqlValue.cb );
}

// PYR-69531
void DbmStatement::bindFirstCol( PSqlBigIntAsUInt32& sqlValue )
{
	ensureFirstCol();
	bindBigIntCol(++currentColOrdinal, &sqlValue.value, &sqlValue.cb);
}

void DbmStatement::bindNextCol( PSqlInt& sqlValue )
{
	ensureNextCol();
	bindIntCol( ++currentColOrdinal, &sqlValue.value, &sqlValue.cb );
}

void DbmStatement::bindNextCol( PSqlBigInt& sqlValue )
{
	ensureNextCol();
	bindBigIntCol( ++currentColOrdinal, &sqlValue.value, &sqlValue.cb );
}

void DbmStatement::bindNextCol( PSqlSmallInt& sqlValue )
{
	ensureNextCol();
	bindSmallIntCol( ++currentColOrdinal, &sqlValue.value, &sqlValue.cb );
}

void DbmStatement::bindNextCol( PSqlTimestamp& sqlValue )
{
	ensureNextCol();
	bindTimeStampCol( ++currentColOrdinal, &sqlValue.value, &sqlValue.cb );
}

void DbmStatement::bindNextCol( PSqlDate& sqlValue )
{
	ensureNextCol();
	bindDateCol( ++currentColOrdinal, &sqlValue.value, &sqlValue.cb );
}
// PYR-46890
void DbmStatement::bindNextCol( PSqlTournamentId& sqlValue )
{
	ensureNextCol();
	bindBigIntCol( ++currentColOrdinal, &sqlValue.value, &sqlValue.cb );
}
// PYR-69531
void DbmStatement::bindNextCol( PSqlBigIntAsUInt32& sqlValue )
{
	ensureNextCol();
	bindBigIntCol( ++currentColOrdinal, &sqlValue.value, &sqlValue.cb );
}
const char* DbmStatement::paramsToString(PString& out) const // PYR-27855
{
	for (map<int, StmtParam>::const_iterator it = paramsMap.begin(); it != paramsMap.end(); ++it)
	{
		int ordinal = it->first;
		const StmtParam& data = it->second;
		out.appendInt(ordinal).append(':');
		data.toString(out);
		out.append('\n');
	}
	return out;
}

static const char* appendFullyOrTruncated(PString& out, const char* str)
{
	int strLen = strlen(str);
	const int MAX_STR_LEN = 2048;
	if (strLen > MAX_STR_LEN)
	{
		return out.append("truncated from ").appendInt(strLen).append(" to ").appendInt(MAX_STR_LEN)
			.append(" '").append(str, MAX_STR_LEN).append(" ...'");
	}
	return out.append('\'').append(str).append('\'');
}

const char* DbmStatement::StmtParam::toString(PString& out) const // PYR-27855
{
  try
  {
	if (!strLen_or_indPtr)
		return out.append("(strLen_or_indPtr=0)");

	if (*strLen_or_indPtr == SQL_NULL_DATA)
		return out.append("(null)");

	if (!pValue)
		return out.append("(pValue=0)");

	switch (cType)
	{
		case SQL_SMALLINT:
		{
			SQLSMALLINT val = *(SQLSMALLINT *)pValue;
			return out.appendInt(val);
		}
		case SQL_INTEGER:
		{
			SQLINTEGER val = *(SQLINTEGER *)pValue; //-V206
			return out.appendInt(val);
		}
		case SQL_C_SBIGINT:
		{
			SQLBIGINT val = *(SQLBIGINT *)pValue; //-V206
			return out.appendInt64(val);
		}
		case SQL_C_TYPE_TIMESTAMP:
		{
			SQL_TIMESTAMP_STRUCT& t = *(SQL_TIMESTAMP_STRUCT *)pValue;
			char buf[128];
			sprintf(buf, "%d-%02d-%02d-%02d.%02d.%02d", t.year, t.month, t.day, t.hour, t.minute, t.second);
			return out.append(buf);
		}
		case SQL_C_TYPE_DATE:
		{
			SQL_DATE_STRUCT& d = *(SQL_DATE_STRUCT *)pValue;
			char buf[64];
			sprintf(buf, "%d-%02d-%02d", d.year, d.month, d.day);
			return out.append(buf);
		}
		case SQL_C_CHAR:
		{
			if (sqlType == SQL_XML)
			{
				return appendFullyOrTruncated(out, (const char *)pValue);
			}
			if (*strLen_or_indPtr != SQL_NTS)
			{
				// may happen if strLen was set to non-NTS via assignment of just selected pair of (value + cb)
				return out.append("(string) !NTS len=").appendInt(*strLen_or_indPtr);
			}
			const char* str = (const char *)(SQLCHAR *)pValue;
			return appendFullyOrTruncated(out, str);
		}
		case SQL_C_BINARY:
		{
			if (sqlType == SQL_XML)
			{
				return appendFullyOrTruncated(out, (const char *)pValue);
			}
			// pValue is invalid here
			return out.append("(blob)");
		}
		default:
		{
			return out.append("(unknown) type=").appendInt(cType);
		}
	}
  }
  catch (const PError& er)
  {
	  return out.append('\n').append(er.why());
  }
}

// Start Re-factored from picdbmprepstmt.h

//
// Base class
//

DbmStatementWithBlob::DbmStatementWithBlob(DatabaseManagerCommon& man, const Blob& blob_)
	:	DbmStatement(man),
	blob(blob_)
{
}

SQLINTEGER DbmStatementWithBlob::getBlobData(const Blob& blob, const BYTE*& retPtr, size_t& offset) const
{
	const BYTE* bytePtr = blob.ptr();
	size_t size = blob.size();
	size_t left = size - offset;
	if( left > MAX_DATA_LEN )
		left = MAX_DATA_LEN;
	retPtr = ( bytePtr + offset );
	offset += left;
	return left;
}

void DbmStatementWithBlob::execute()
{
	SQLRETURN retcode = executeRet();

	SQLPOINTER pToken;

	if ( SQL_NEED_DATA == retcode )
	{
		retcode = paramData( &pToken );
		if (retcode == SQL_NEED_DATA) 
		{
			size_t offset = 0;
			for(;;)
			{
				const BYTE* ptr;
				const SQLINTEGER sz = getBlobData( blob, ptr, offset );
				if( sz == 0 )
					break;
				putData((const SQLPOINTER)ptr, sz);
			}
		}
		retcode = paramData( &pToken );
		checkRetcodeWithTrace( retcode, DatabaseManagerCommon::eCheckNotSuccess );
	}
	else
	{
		checkRetcodeWithTrace( retcode, DatabaseManagerCommon::eCheckNotSuccess );
	}
}
// End Re-factored from picdbmprepstmt.h

// PYR-48098
void DbmStatement::bindOutputParam(
								  SQLUSMALLINT	ordinal, // >=1
								  SQLSMALLINT	valueType, // C
								  SQLSMALLINT	parameterType, // SQL
								  SQLULEN		columnSize, // non-zero for strings
								  SQLPOINTER	valuePtr,
								  SQLLEN*		strLen_or_indPtr)
{
	SQLRETURN retCode = SQLBindParameter( hstmt, ordinal, SQL_PARAM_OUTPUT, valueType, parameterType, columnSize, 0/*decimalDigits*/, valuePtr, 0/*bufferLength*/, strLen_or_indPtr );
	checkRetcode( retCode, DatabaseManagerCommon::eCheckNotSuccess );
}

void DbmStatement::bindBigIntParam_ForOutput( int ordinal, SQLBIGINT* valuePtr, SQLLEN* indPtr )
{
	*indPtr = 0;
	bindOutputParam( ordinal, SQL_C_SBIGINT, SQL_INTEGER, 0, valuePtr, indPtr );
}

void DbmStatement::bindNextParam_ForOutput( PSqlBigInt& sqlValue )
{
	ensureNextParam();
	bindBigIntParam_ForOutput( ++currentParamOrdinal, &sqlValue.value, &sqlValue.cb );
}
