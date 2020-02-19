#pragma once

#include "dbmconstants.h"
#include "dbmstmt.h"

class InsertGeneratorObjectStmt : public DbmStatement
{
	PSqlString<OBJECT_NAME_LEN> objectName;
	PSqlBigInt					objectId;

	void prepareStmt()
	{
		prepare("INSERT INTO GENERATOR64(ObjectName, ObjectId) VALUES (?, ?)");
		
		bindFirstParam(objectName);
		bindNextParam(objectId);
	}

public:
	InsertGeneratorObjectStmt(DatabaseManagerCommon& man) : DbmStatement(man)
	{
		prepareStmt();
	}
	void init(const char* name, UINT64 object_id)
	{
		objectName.initCut(name);
		objectId = object_id;
	}
};

class GetGeneratorObjectIdStmt : public DbmStatement
{
	PSqlString<OBJECT_NAME_LEN> objectName;
	PSqlBigInt					objectId;

	void prepareStmt()
	{
		PString query = "SELECT ObjectId FROM " DB_TABLE_GENERATOR64 " WHERE ObjectName = ?";

		prepare(query);
		bindFirstCol(objectId);
		bindFirstParam(objectName);
	}
public:
	GetGeneratorObjectIdStmt(DatabaseManagerCommon& man)	:DbmStatement(man) 
	{
		prepareStmt();
	}
	void init( const char* name )
	{
		objectName.initCut(name);
	}
	UINT64 getObjectId() const
	{ 
		return objectId.value; 
	}
};

class SetGeneratorObjectIdStmt : public DbmStatement
{
	PSqlString<OBJECT_NAME_LEN> objectName;
	PSqlBigInt					objectId;

	void prepareStmt()
	{
		prepare("UPDATE GENERATOR64 SET ObjectId = ? WHERE ObjectName = ?");
		bindFirstParam(objectId);
		bindNextParam(objectName);
	}

public:
	SetGeneratorObjectIdStmt(DatabaseManagerCommon& man) : DbmStatement(man)
	{
		prepareStmt();
	}
	void init(const char* name, UINT64 id)
	{
		objectName.initCut(name);
		objectId = id;
	}
};


