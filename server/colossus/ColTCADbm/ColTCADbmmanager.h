#pragma once

#include "ppstring.h"
#include "commlib.h"
#include "dbcommon.h"
#include "pyrtime.h"
#include "ColTCADbmprepstmt.h"

class  ColTCADbmServerObject;
struct TCAColTaskData;
struct GetDateExtentsOutputs;

typedef pair<const char*, PString> HeLoggerParameter;
typedef vector<HeLoggerParameter> HeLoggerParameters;

class ColTCADbmManager : public DatabaseManagerCommon
{
private:
	ColTCADbmServerObject*		dbmObj;
	int							errorsInMessages;
	UINT32						dbReconnectInterval;

public:
	ColTCADbmManager();
	~ColTCADbmManager();

	void init(ColTCADbmServerObject* obj);
	bool openDataStorage(const char* fullFileName, const char* sectionName);
	void rollbackOnError();
	void setAutoCommitTrueOnError();
	void setAutoCommitTrue();
	UINT32 getDbReconnectInterval() const { return dbReconnectInterval; }
	void composeHeLoggerParameters( HeLoggerParameters& parameters ) const;

	// Messages

private:
	// statements

	void prepareStatements();
	void deleteStatements();
	void zeroStatements();

	// PYR-27418
	const DbmGenerator::Generator* getGenerators( size_t& generatorsSize ) const override;
};

