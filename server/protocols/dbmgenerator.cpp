
#include	"dbmgenerator.h"
#include	"dbmstmt.h"
#include	"dbm.h"
#include	"dbmgeneratorprepstmt.h" // PYR-24146 - moved class GetNextIdStmt to new file dbmgeneratorprepstmt.h
#include	"commcli.h"
#include	"dbmserverobjectcommon.h"


DbmGenerator::DbmGenerator()
	: dbManagerCommon(0)
	, idIncrement(GENERATOR_ID_INCREMENT)
{
	zeroStatements();
}

void DbmGenerator::init(DatabaseManagerCommon* manager)
{
	dbManagerCommon = manager;
}

DbmGenerator::~DbmGenerator()
{
	try //for pclint as in "dbm\dbmanager.h"
	{
		deleteStatements();
	}
	catch (...)
	{
	}
}

void DbmGenerator::prepareStatements()
{
	getGeneratorObjectIdStmt	= new GetGeneratorObjectIdStmt(*dbManagerCommon);
	setGeneratorObjectIdStmt	= new SetGeneratorObjectIdStmt(*dbManagerCommon);
	insertGeneratorObjectStmt	= new InsertGeneratorObjectStmt(*dbManagerCommon);
}

void DbmGenerator::deleteStatements()
{
	delete	insertGeneratorObjectStmt;
	delete	setGeneratorObjectIdStmt;
	delete	getGeneratorObjectIdStmt;
	
	zeroStatements();
}

void DbmGenerator::zeroStatements()
{
	getGeneratorObjectIdStmt	= 0;
	setGeneratorObjectIdStmt	= 0;
	insertGeneratorObjectStmt	= 0;
}

void DbmGenerator::insertObject(const char* objectName, UINT64 objectId)
{
	insertGeneratorObjectStmt->init(objectName, objectId);
	insertGeneratorObjectStmt->execute();
}

void DbmGenerator::setObjectId(const char* objectName, UINT64 objectId)
{
	setGeneratorObjectIdStmt->init(objectName, objectId);
	setGeneratorObjectIdStmt->execute();
}

bool DbmGenerator::getObjectId(const char* objectName, UINT64& objectId)
{
	objectId = 0;
	getGeneratorObjectIdStmt->init(objectName);
	getGeneratorObjectIdStmt->execute();
	bool found = getGeneratorObjectIdStmt->fetch();
	getGeneratorObjectIdStmt->closeCursor();
	if (found)
	{
		objectId = getGeneratorObjectIdStmt->getObjectId();
	}
	return found;
}

UINT64 DbmGenerator::getNextId(const char* objectName)
{
	UINT64 objectId = 0;
	if (getObjectId(objectName, objectId))
	{
		setObjectId(objectName, objectId + idIncrement);
		return objectId;
	}

	objectId = 1;
	insertObject(objectName, objectId);
	return objectId;
}
