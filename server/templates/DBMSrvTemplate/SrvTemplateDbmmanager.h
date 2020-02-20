#pragma once

#include "ppstring.h"
#include "commlib.h"
#include "dbcommon.h"
#include "pyrtime.h"
#include "SrvTemplateDbmprepstmt.h"

class  SrvTemplateDbmServerObject;
struct TCAColTaskData;
struct GetDateExtentsOutputs;

typedef pair<const char*, PString> HeLoggerParameter;
typedef vector<HeLoggerParameter> HeLoggerParameters;

class SrvTemplateDbmManager : public DatabaseManagerCommon
{
private:
	SrvTemplateDbmServerObject*		dbmObj;
	int							errorsInMessages;
	UINT32						dbReconnectInterval;

public:
	SrvTemplateDbmManager();
	~SrvTemplateDbmManager();

	void init(SrvTemplateDbmServerObject* obj);
	bool openDataStorage(const char* fullFileName, const char* sectionName);
	void rollbackOnError();
	void setAutoCommitTrueOnError();
	void setAutoCommitTrue();
	UINT32 getDbReconnectInterval() const { return dbReconnectInterval; }
	void composeHeLoggerParameters( HeLoggerParameters& parameters ) const;

	// Messages

private:
	// statements
	SampleTestingStmt* sampleTestingStmt;

	void prepareStatements();
	void deleteStatements();
	void zeroStatements();

};

