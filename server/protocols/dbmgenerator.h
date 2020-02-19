#pragma once

#include "pplib.h"
#include "ppcontainer.h"
#include "dbmconstants.h"

class	DatabaseManagerCommon;
class	CommMsgParser;

class	GetGeneratorObjectIdStmt;
class	InsertGeneratorObjectStmt;
class	SetGeneratorObjectIdStmt;

class DbmGenerator
{
public:	
	DbmGenerator();
	~DbmGenerator();

	void init(DatabaseManagerCommon* manager);
	void setIdIncrement(UINT32 newIncrement) 
	{
		PLog("DbmGenerator: idIncrement=%u", newIncrement);
		idIncrement = newIncrement;
	}
	UINT32 getIdIncrement() const
	{
		return idIncrement;
	}
	
	UINT64 getNextId( const char* objectName ); // may throw

	void prepareStatements();
	void deleteStatements();
	void zeroStatements();

private:
	GetGeneratorObjectIdStmt*	getGeneratorObjectIdStmt;
	InsertGeneratorObjectStmt*	insertGeneratorObjectStmt;
	SetGeneratorObjectIdStmt*	setGeneratorObjectIdStmt;

	DatabaseManagerCommon*	dbManagerCommon;
	INT32 idIncrement;
	PStringSet localGenerators;
	
	void insertObject(const char* objectName, UINT64 objectId);
	void setObjectId(const char* objectName, UINT64 objectId);
	bool getObjectId(const char* objectName, UINT64& objectId);

	DbmGenerator(const DbmGenerator& other);
	const DbmGenerator& operator =(const DbmGenerator& other);
};

