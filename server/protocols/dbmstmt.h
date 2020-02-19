//dbmstmt.h

#ifndef dbmstmt_hincluded
#define dbmstmt_hincluded

#include "dbcommon.h"
#include "commmsgbody.h"
#include "psqlvalue.h"

#define DUPLICATE_KEY_ERROR_STR "23505"
#define GRANT_ERROR_STR	"42501"

// THE FOLLOWING DEFINE MUST NOT CHANGE.
//
// olap and majordomo replication depend on this value and it represents the size of a db field.
// if you ever need to do change it - involve core server, payment security and dba team heads.
//
#define OLAP_MSG_BODY_LEN (16000 + 1)

/////////////////////////// NB!!!/////////////////////////////////
// This code will work ONLY with db2
// If using some other SQL server - try to use SQL_LONGVARBINARY instead, 
/// though I am not sure it will handle big (>32K) BLOBs without data truncation...
#ifndef DBM_DB_TYPE_DB2
#define DBM_DB_TYPE_DB2 
#endif
///////////////////////////////////////////////////////////////////

/// workaround for SQL_BLOB which is missing in "sqlext.h"
#ifdef DBM_DB_TYPE_DB2
#define DBM_SQL_BLOB (-98)
//#define DBM_SQL_CLOB -98
#endif

// for backward compatibility with older CLI version
// must be removed once all dbms move to CLI9
#ifndef SQL_XML
#define SQL_XML (-370)
#endif

class DbmStatement
{
protected:
	DatabaseManagerCommon& manager; //lint !e1725
	SQLHSTMT getHandle() const { return hstmt; } // PYR-46901

private:
	SQLHSTMT	hstmt;
	
	bool prepared;

	// PYR-63589: This method for internal use only, because one need to keep types of 'inValues' and 'inParams'
	// parameters in sync. For example, currently this function used only for following types:
	// const vector<const char*>& inValues, vector<PSqlString<N>>& inParams
	// const vector<PString>& inValues, vector<PSqlString<N>>& inParams
	// Example of usage could be found below.
	template< typename T, typename S >
	void bindCopyInStringParams( const T& inValues, vector<S>& inParams, bool first )
	{
		inParams.clear();
		inParams.reserve( inValues.size() );
		for( const auto& value : inValues )
		{
			S param;
			param.init( value );
			inParams.emplace_back( param );
			S& last = inParams.back();
			if( first && inParams.size() == 1 )
			{
				bindFirstParam( last );
			}
			else
			{
				bindNextParam( last );
			}
		}
	}
public:
	DbmStatement( DatabaseManagerCommon& manager_ );
	virtual ~DbmStatement(); // dtor became virtual after 2018.Build.02

	void	checkRetcode( SQLRETURN retcode, DatabaseManagerCommon::CheckModifier modifier );
	void	checkRetcodeWithTrace( SQLRETURN retcode, DatabaseManagerCommon::CheckModifier modifier ); // PYR-27855

	void	freeStmt();
	void	execDirect( const SQLCHAR* statement, DatabaseManagerCommon::CheckModifier modifier = DatabaseManagerCommon::eCheckNotSuccess );
	void	execDirect( const char* statement, DatabaseManagerCommon::CheckModifier modifier = DatabaseManagerCommon::eCheckNotSuccess );
	bool	execDirectUpdateDelete( const char* statement, DatabaseManagerCommon::CheckModifier modifier = DatabaseManagerCommon::eCheckNotSuccess );
	void	prepare(const PString& statement);
	void	prepare(const char* statement);
	void	bindIntParam( int ordinal, SQLINTEGER* valuePtr, SQLLEN* indPtr );
	void	bindBigIntParam( int ordinal, SQLBIGINT* valuePtr, SQLLEN* indPtr );
	void	bindStrParam( int ordinal, SQLINTEGER buffLen, SQLCHAR* valuePtr, SQLLEN* strLen );
	void	bindTimeParam( int ordinal, SQL_TIMESTAMP_STRUCT* valuePtr, SQLLEN* indPtr );
	void	bindDateParam( int ordinal, SQL_DATE_STRUCT* valuePtr, SQLLEN* indPtr );
	void	bindBinParam( int ordinal, SQLLEN* cbParam );
	void	bindSmallIntParam( int ordinal, SQLSMALLINT* valuePtr, SQLLEN* indPtr );
	//void	bindCLOBParam( int ordinal, SQLINTEGER* cbParam );
	// PYR-48098 - stored procedure requires output parameters
	void	bindBigIntParam_ForOutput( int ordinal, SQLBIGINT* valuePtr, SQLLEN* indPtr );

	// xml binding was introduced in scope of PYR-36002
	void	initAndBindXmlParam( const PString& xmlStrUtf8, int ordinal, PSqlXml& sqlXml ); // better be the last param if bindNextParam is used
	void	bindNullXmlParam( int ordinal, PSqlXml& sqlXml );

	//void		prepareTableStructure(SQLCHAR * tblName);
	SQLRETURN	paramData(SQLPOINTER pToken);
	SQLRETURN	putData( const SQLPOINTER Data, SQLINTEGER cbData );

	void		execute();
	SQLRETURN	executeRet();
	bool		execUpdateDelete();
	bool		execConditionalInsert(); // to be used only with LZ or AR approval
	bool 		execInsertIgnoreDuplicate(); // PYR-37710 to be used only with LZ or AR approval
	bool		fetch();
	bool		fetchIgnoreWarning();
	void		getBlob( SQLINTEGER ordinal, PBlock& ret );
	void		closeCursor();

	void		setTimeOut(SQLINTEGER seconds);

	SQLLEN getRowCount();

	void bindCol( 
		SQLUSMALLINT	colOrdinal, 
		SQLSMALLINT		targetType, 
		SQLPOINTER		targetValuePtr,
		SQLINTEGER		bufferLength,
		SQLLEN*		StrLen_or_IndPtr);

	void bindSmallIntCol(
		SQLUSMALLINT	colOrdinal, 
		SQLSMALLINT*		targetValuePtr,
		SQLLEN*		strLen_or_IndPtr);

	void bindIntCol(
		SQLUSMALLINT	colOrdinal, 
		SQLINTEGER*		targetValuePtr,
		SQLLEN*		strLen_or_IndPtr);

	void bindBigIntCol(
		SQLUSMALLINT	colOrdinal, 
		SQLBIGINT*		targetValuePtr,
		SQLLEN*		strLen_or_IndPtr);

	void bindStrCol(
		SQLUSMALLINT	colOrdinal, 
		SQLPOINTER		targetValuePtr,
		SQLINTEGER		bufferLength,
		SQLLEN*		StrLen_or_IndPtr);

	void bindTimeStampCol(
		SQLUSMALLINT			colOrdinal,
		SQL_TIMESTAMP_STRUCT*	ts, 
		SQLLEN*				StrLen_or_IndPtr);

	void bindDateCol(
		SQLUSMALLINT			colOrdinal,
		SQL_DATE_STRUCT*		ds, 
		SQLLEN*				StrLen_or_IndPtr
		);

	template< typename T >
	void bindFirstInStringParams(T& params)
	{
		for( auto pit = params.begin(); params.end() != pit; ++pit )
		{
			if( params.begin() == pit )
			{
				bindFirstParam( *pit );
			}
			else
			{
				bindNextParam( *pit );
			}
		}
	}

	template< typename T >
	void bindNextInStringParams(T& params)
	{
		for( auto& param : params )
		{
			bindNextParam( param );
		}
	}

	template< int N >
	void bindFirstInStringParams(const vector<const char*>& inValues, vector<PSqlString<N>>& inParams)
	{
		bindCopyInStringParams( inValues, inParams, true /* first */ );
	}

	template< int N >
	void bindFirstInStringParams(const vector<PString>& inValues, vector<PSqlString<N>>& inParams)
	{
		bindCopyInStringParams( inValues, inParams, true /* first */ );
	}

	template< int N >
	void bindNextInStringParams(const vector<const char*>& inValues, vector<PSqlString<N>>& inParams)
	{
		bindCopyInStringParams( inValues, inParams, false /* first */ );
	}

	template< int N >
	void bindNextInStringParams(const vector<PString>& inValues, vector<PSqlString<N>>& inParams)
	{
		bindCopyInStringParams( inValues, inParams, false /* first */ );
	}

	// PYR-63589: The following method is deprecated, please use 'prepareInStringParams' and 'bind*InStringParams' methods instead.
	static void prepareOdbcSafeString(PString& safeStr, const char* inStr);
	// DB2 specific member functions
	// PYR-63589: The following method is deprecated, please use 'prepareInStringParams' and 'bind*InStringParams' methods instead.
	static void prepareInString(const vector<PString>& inVector, PString& inString);
	// PYR-63589: The following method is deprecated, please use 'prepareInStringParams' and 'bind*InStringParams' methods instead.
	static void prepareInString(const vector<const char*>& inVector, PString& inString);
	static void prepareInString(const vector<UINT32>& inVector, PString& inString);
	static void prepareInString(const vector<UINT64>& inVector, PString& inString);
	static void prepareInStringParams(const size_t n, PString& inString); // PYR-63589


protected:
	static void appendIntField(PString& s, SQLINTEGER value, SQLINTEGER cbValue);
	static void appendStrField(PString& s, const SQLCHAR* value, SQLINTEGER cbValue);
	static void appendTimeStampField(PString& s, const SQL_TIMESTAMP_STRUCT& value, SQLINTEGER cbValue);

	//
	// Support for SqlValue-derived classes
	//

	//
	// Parameters
	//
	void bindParam( int ordinal, PSqlSmallInt& sqlValue );
	void bindParam( int ordinal, PSqlInt& sqlValue );
	void bindParam( int ordinal, PSqlBigInt& sqlValue );
	template<int N>
	void bindParam( int ordinal, PSqlString<N>& sqlValue )
	{
		ensureNotAutoParamOrdinal();
		bindStrParam( ordinal, N, sqlValue.value, &sqlValue.cb );
	}
	void bindParam( int ordinal, PSqlTimestamp& sqlValue );
	void bindParam( int ordinal, PSqlDate& sqlValue );
	void bindParam( int ordinal, PSqlTournamentId& sqlValue ); // PYR-46890
	void bindParam( int ordinal, PSqlBigIntAsUInt32& sqlValue ); // PYR-69538

	//
	// Columns
	//
	void bindCol( int ordinal, PSqlSmallInt& sqlValue );
	void bindCol( int ordinal, PSqlInt& sqlValue );
	void bindCol( int ordinal, PSqlBigInt& sqlValue );
	template<int N>
	void bindCol( int ordinal, PSqlString<N>& sqlValue )
	{
		ensureNotAutoColOrdinal();
		bindStrCol( ordinal, sqlValue.value, N, &sqlValue.cb );
	}
	void bindCol( int ordinal, PSqlTimestamp& sqlValue );
	void bindCol( int ordinal, PSqlDate& sqlValue );
	void bindCol( int ordinal, PSqlTournamentId& sqlValue ); // PYR-46890
	void bindCol( int ordinal, PSqlBigIntAsUInt32& sqlValue ); // PYR-69538
	// bindCol with PSqlXml is NOT implemented - use getBlob instead

private:
	bool autoParamOrdinal;
	bool autoColOrdinal;
	int currentParamOrdinal;
	int currentColOrdinal;
	void ensureFirstParam();
	void ensureNextParam() const;
	void ensureFirstCol();
	void ensureNextCol() const;
	void ensureNotAutoParamOrdinal();
	void ensureNotAutoColOrdinal();

protected:
	//
	// Param auto-binding
	//
	void bindFirstParam( PSqlInt& sqlValue );
	void bindFirstParam( PSqlBigInt& sqlValue );
	void bindFirstParam( PSqlSmallInt& sqlValue );
	template<int N>
	void bindFirstParam( PSqlString<N>& sqlValue )
	{
		ensureFirstParam();
		bindStrParam( ++currentParamOrdinal, N, sqlValue.value, &sqlValue.cb );
	}
	void bindFirstParam( PSqlTimestamp& sqlValue );
	void bindFirstParam( PSqlDate& sqlValue );
	void bindFirstParam( PSqlTournamentId& sqlValue ); // PYR-46890
	void bindFirstParam( PSqlBigIntAsUInt32& sqlValue ); // PYR-69531

	void bindNextParam( PSqlInt& sqlValue );
	void bindNextParam( PSqlBigInt& sqlValue );
	void bindNextParam_ForOutput( PSqlBigInt& sqlValue ); // PYR-48098
	void bindNextParam( PSqlSmallInt& sqlValue );
	template<int N>
	void bindNextParam( PSqlString<N>& sqlValue )
	{
		ensureNextParam();
		bindStrParam( ++currentParamOrdinal, N, sqlValue.value, &sqlValue.cb );
	}
	void bindNextParam( PSqlTimestamp& sqlValue );
	void bindNextParam( PSqlDate& sqlValue );
	void bindNextParam( PSqlTournamentId& sqlValue ); // PYR-46890
	void bindNextParam( PSqlBigIntAsUInt32& sqlValue ); // PYR-69531
	//
	// Column auto-binding
	//
	void bindFirstCol( PSqlInt& sqlValue );
	void bindFirstCol( PSqlBigInt& sqlValue );
	void bindFirstCol( PSqlSmallInt& sqlValue );
	template<int N>
	void bindFirstCol( PSqlString<N>& sqlValue )
	{
		ensureFirstCol();
		bindStrCol( ++currentColOrdinal, sqlValue.value, N, &sqlValue.cb );
	}
	void bindFirstCol( PSqlTimestamp& sqlValue );
	void bindFirstCol( PSqlDate& sqlValue );
	void bindFirstCol( PSqlTournamentId& sqlValue ); // PYR-46890
	void bindFirstCol( PSqlBigIntAsUInt32& sqlValue ); // PYR-69531

	void bindNextCol( PSqlInt& sqlValue );
	void bindNextCol( PSqlBigInt& sqlValue );
	void bindNextCol( PSqlSmallInt& sqlValue );
	template<int N>
	void bindNextCol( PSqlString<N>& sqlValue )
	{
		ensureNextCol();
		bindStrCol( ++currentColOrdinal, sqlValue.value, N, &sqlValue.cb );
	}
	void bindNextCol( PSqlTimestamp& sqlValue );
	void bindNextCol( PSqlDate& sqlValue );
	void bindNextCol( PSqlTournamentId& sqlValue ); // PYR-46890
	void bindNextCol( PSqlBigIntAsUInt32& sqlValue ); // PYR-69531

private: // PYR-27855
	void bindInputParam( // wrapper over SQLBindParameter
		SQLUSMALLINT    ordinal,
		SQLSMALLINT     valueType, // C
		SQLSMALLINT     parameterType, // SQL
		SQLULEN         columnSize, // only for strings
		SQLPOINTER      parameterValuePtr,
		SQLLEN *        strLen_or_indPtr);
	// PYR-48098
	void bindOutputParam( // wrapper over SQLBindParameter for output parameters
		SQLUSMALLINT	ordinal,
		SQLSMALLINT		valueType, // C
		SQLSMALLINT		parameterType, // SQL
		SQLULEN			columnSize, // only for strings
		SQLPOINTER		parameterValuePtr,
		SQLLEN*			strLen_or_indPtr);

	struct StmtParam
	{
		int cType; // = SQL_C_SBIGINT, SQL_SMALLINT, SQL_C_TYPE_TIMESTAMP, etc...
		int sqlType; // = SQL_INTEGER, SQL_CHAR, SQL_TYPE_TIMESTAMP, SQL_XML, etc...
		void* pValue;
		SQLLEN* strLen_or_indPtr;

		StmtParam(SQLSMALLINT cType_, SQLSMALLINT sqlType_, SQLPOINTER pValue, SQLLEN* strLen_or_indPtr_)
			: cType(cType_), sqlType(sqlType_), pValue(pValue), strLen_or_indPtr(strLen_or_indPtr_) {}
		const char* toString(PString& out) const;
	};

	map<int, StmtParam> paramsMap; // key: ordinal param Number
	PString stmtText; // is filled only for dynamic statements, not for prepare("...")
	const char* paramsToString(PString& out) const;
public:
	void logStmtTextWithParams() const; // refactored into dedicated function in scope of PYR-29476
	// PYR-27855 end
};



// Start Re-factored from picdbmprepstmt.h
#define  MAX_DATA_LEN 1024

class Blob
{
	const BYTE* blobPtr;
	size_t blobSz;

	Blob();

public:
	Blob(const PBlock& blobPBlock)
	{
		blobPtr = blobPBlock.ptr();
		blobSz = blobPBlock.size();
	}
	Blob(const BYTE* blobPtr_, size_t blobSz_)
	{
		blobPtr = blobPtr_;
		blobSz = blobSz_;
	}
	const BYTE* ptr() const { return blobPtr; }
	size_t size() const { return blobSz; }
};

//
// Base class
//

class DbmStatementWithBlob : public DbmStatement
{
private:
	const Blob& blob;
	SQLINTEGER getBlobData(const Blob& blob, const BYTE*& retPtr, size_t& offset) const;
public:
	DbmStatementWithBlob(DatabaseManagerCommon& man, const Blob& blob_);
	void execute();
};
// End Re-factored from picdbmprepstmt.h

#endif	//dbmstmt_hincluded
