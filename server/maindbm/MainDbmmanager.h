#pragma once

#include "ppstring.h"
#include "commlib.h"
#include "dbcommon.h"
#include "pyrtime.h"
#include "MainDbmprepstmt.h"
#include "userdbmprepstmt.h"
#include "MainCacheTypes.h"

#define CACHEDUSERS_MAXSIZE				50000
#define CACHEDUSERS_CUTOFFSECONDS		12 * 60 * 60  // 12 HOURS

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

	CachedUsers					cachedUsers;
	UINT32						cachedUsersMaxSize;
	UINT32						cachedUsersCutoffSeconds;

public:
	MainDbmManager();
	~MainDbmManager();

	void init(MainDbmServerObject* obj);
	void dynamicInit(const PIniFile& iniFile);
	bool openDataStorage(const char* fullFileName, const char* sectionName);
	void rollbackOnError();
	void setAutoCommitTrueOnError();
	void setAutoCommitTrue();
	UINT32 getDbReconnectInterval() const { return dbReconnectInterval; }
	void composeHeLoggerParameters( HeLoggerParameters& parameters ) const;

	// helper 
	void sweepCaches();
	User* findCachedUser(const char* userId);

	// Messages
	INT16 insertUser(User& user, PString& sqlErr);
	INT16 getUsers(const vector<PString>& userIds, CommMsgBody& body, PString& sqlErr);

private:
	// statements
	InsertUserStmt* insertUserStmt;
	GetUserByUserIdStmt* getUserByUserIdStmt;

	void prepareStatements();
	void deleteStatements();
	void zeroStatements();

};

