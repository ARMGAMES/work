#pragma once

#include "ppstring.h"
#include "commlib.h"
#include "dbcommon.h"
#include "pyrtime.h"
#include "MainDbmprepstmt.h"

class  MainDbmServerObject;
struct TCAColTaskData;
struct GetDateExtentsOutputs;

typedef pair<const char*, PString> HeLoggerParameter;
typedef vector<HeLoggerParameter> HeLoggerParameters;

class MainDbmManager : public DatabaseManagerCommon
{
private:
	MainDbmServerObject*		dbmObj;
	int							errorsInMessages;
	UINT32						dbReconnectInterval;

public:
	MainDbmManager();
	~MainDbmManager();

	void init(MainDbmServerObject* obj);
	bool openDataStorage(const char* fullFileName, const char* sectionName);
	void rollbackOnError();
	void setAutoCommitTrueOnError();
	void setAutoCommitTrue();
	UINT32 getDbReconnectInterval() const { return dbReconnectInterval; }
	void composeHeLoggerParameters( HeLoggerParameters& parameters ) const;

	// Messages
	INT16 insertUser(User& user, PString& sqlErr);

private:
	// statements
	InsertUserStmt* insertUserStmt;

	void prepareStatements();
	void deleteStatements();
	void zeroStatements();

};

