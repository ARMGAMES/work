#pragma

#include "dbmconstants.h"
#include "dbmstmt.h"

class GetNextIdStmt : public DbmStatement
{
	friend class DbmGenerator;
	PSqlString<OBJECT_NAME_LEN> objectName;
	PSqlBigInt					objectId;

	void prepareStmt(const char* forUpdate)
	{
		PString query = "SELECT ObjectId FROM " DB_TABLE_GENERATOR64 " WHERE ObjectName = ?";
		if (*forUpdate) // PYR-28457 ("generatorIsolationLevel parameter from PYR-20813 is obsolete now)
		{
			query.append(' ').append(forUpdate);
		}
		prepare(query);
		bindFirstCol(objectId);
		bindFirstParam(objectName);
	}
public:
	GetNextIdStmt(DatabaseManagerCommon& man, const bool useForUpdateClause = true /*PYR-45721*/)	:DbmStatement(man) 
	{
		prepareStmt(useForUpdateClause ? man.getForUpdateClause() : "");
	}
	void setObjectName( const char* name )
	{
		objectName.initCut(name);
	}
	UINT64 getObjectId() const
	{ 
		return objectId.value; 
	}
};

class IdRangeStmtBase : public DbmStatement
{
protected:
	PSqlString<OBJECT_NAME_LEN> objectName;
	PSqlBigInt					startRange;
	PSqlBigInt					endRange;
	PSqlTimestamp				when;
	PSqlBigInt					objectId;

	IdRangeStmtBase( DatabaseManagerCommon& man ) : DbmStatement( man )
	{}
};

class InsertIdRangeStmt : public IdRangeStmtBase
{
private:
	void prepareStmt()
	{
		prepare( "INSERT INTO " DB_TABLE_IDRANGES " (OBJECTNAME, STARTRANGE, ENDRANGE, WHEN) VALUES (?, ?, ?, ?)" );

		bindFirstParam( objectName );
		bindNextParam( startRange );
		bindNextParam( endRange );
		bindNextParam( when );
	}

public:
	InsertIdRangeStmt( DatabaseManagerCommon& man ) : IdRangeStmtBase( man )
	{
		prepareStmt();
	}

	void init( const char* objectName_, const UINT64 startId, const UINT64 endId, const SrvTime& when_ )
	{
		objectName = objectName_;
		startRange = startId;
		endRange = endId;
		when = when_;
	}
};

class GetIdRangesStmt : public IdRangeStmtBase
{
private:
	void prepareStmt()
	{
		prepare( "SELECT G64.OBJECTNAME, G64.OBJECTID, STARTRANGE, ENDRANGE FROM " DB_TABLE_IDRANGES " IDR JOIN GENERATOR64 G64 ON IDR.OBJECTNAME = G64.OBJECTNAME AND IDR.ENDRANGE >= G64.OBJECTID ORDER BY ENDRANGE" );

		bindFirstCol( objectName );
		bindNextCol( objectId );
		bindNextCol( startRange );
		bindNextCol( endRange );
	}

public:
	GetIdRangesStmt( DatabaseManagerCommon& man ) : IdRangeStmtBase( man )
	{
		prepareStmt();
	}

	const char* getObjectName() const { return objectName.c_str(); }
	UINT64 getStartRange() const { return startRange.value; }
	UINT64 getEndRange() const { return endRange.value; }
	UINT64 getObjectId() const { return objectId.value; }
};

class GetCurrentIdRangesStmt : public IdRangeStmtBase
{
private:
	void prepareStmt()
	{
		prepare( "SELECT STARTRANGE, ENDRANGE FROM " DB_TABLE_IDRANGES " WHERE OBJECTNAME = ? AND ENDRANGE >= ?" );

		bindFirstCol( startRange );
		bindNextCol( endRange );

		bindFirstParam( objectName );
		bindNextParam( objectId );
	}

public:
	GetCurrentIdRangesStmt( DatabaseManagerCommon& man ) : IdRangeStmtBase( man )
	{
		prepareStmt();
	}

	void init( const char* objectName_, const UINT64 objectId_ )
	{
		objectName = objectName_;
		objectId = objectId_;
	}
	UINT64 getStartRange() const { return startRange.value; }
	UINT64 getEndRange() const { return endRange.value; }
};

class InsertObjectRowStmt : public DbmStatement
{
	SQLCHAR objectName[OBJECT_NAME_LEN];
	SQLLEN cbName, cbId;
	SQLBIGINT objectId;
	void prepareStmt()
	{
		prepare("INSERT INTO GENERATOR64(ObjectId, ObjectName) VALUES (?, ?)");
		bindBigIntParam( 1, &objectId, &cbId );
		bindStrParam( 2, OBJECT_NAME_LEN, objectName, &cbName );
	}

public:
	InsertObjectRowStmt(DatabaseManagerCommon& man)	: DbmStatement(man) 
	{
		prepareStmt();
	}
	void init( const char* name, UINT64 object_id )
	{
		objectId = object_id;
		strCopy((char*) objectName, sizeof(objectName), name);
	}
};

class SetNextIdStmt : public DbmStatement
{
	SQLCHAR objectName[OBJECT_NAME_LEN];
	SQLLEN cbName, cbId;
	SQLBIGINT objectId;

	void prepareStmt()
	{
		prepare("UPDATE GENERATOR64 SET ObjectId = ? WHERE ObjectName = ?");
		bindBigIntParam( 1, &objectId, &cbId );
		bindStrParam( 2, OBJECT_NAME_LEN, objectName, &cbName );
	}

public:
	SetNextIdStmt(DatabaseManagerCommon& man)	: DbmStatement(man) 
	{
		prepareStmt();
	}
	void init( const char* name, UINT64 id )
	{
		strCopy((char*) objectName, sizeof(objectName), name);
		objectId = id;
	}
};

// PYR-45721
class SelectAndSetNextIdStmt : public DbmStatement
{
	PSqlString<OBJECT_NAME_LEN> objectName;
	PSqlBigInt objectId;
	PSqlBigInt objectIdIncrement;

	void prepareStmt()
	{
#ifdef POSTGRES // PYR-124995
		// #DBA_REVIEWED #PYR-124995 Eugene M. 2020.Main.02
		prepare(" UPDATE GENERATOR64 t1 "
				" SET OBJECTID = t2.OBJECTID + ? "
				" FROM (SELECT OBJECTNAME, OBJECTID FROM GENERATOR64 WHERE OBJECTNAME = ? FOR UPDATE) t2 "
				" WHERE t1.OBJECTNAME = t2.OBJECTNAME "
				" RETURNING t2.OBJECTID AS OBJECTID ");
#else
		prepare( "SELECT OBJECTID FROM OLD TABLE (UPDATE GENERATOR64 SET OBJECTID = OBJECTID + ? WHERE OBJECTNAME = ?)" );
#endif
		bindFirstCol( objectId );
		bindFirstParam( objectIdIncrement );
		// WHERE
		bindNextParam( objectName );
	}

public:
	SelectAndSetNextIdStmt( DatabaseManagerCommon& man ) : DbmStatement( man )
	{
		prepareStmt();
	}
	void init( const char* name, UINT64 idIncrement )
	{
		objectName = name;
		objectIdIncrement = idIncrement;
	}
	UINT64 getObjectId() const { return objectId.value; }
};

