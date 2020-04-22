#pragma once

#include "ppstring.h"
#include "commlib.h"
#include "dbcommon.h"
#include "pyrtime.h"
#include "LoginDbmprepstmt.h"

class  LoginDbmServerObject;
struct TCAColTaskData;
struct GetDateExtentsOutputs;

typedef pair<const char*, PString> HeLoggerParameter;
typedef vector<HeLoggerParameter> HeLoggerParameters;

class LoginDbmManager : public DatabaseManagerCommon
{
private:
	LoginDbmServerObject*		dbmObj;
	int							errorsInMessages;
	UINT32						dbReconnectInterval;

public:
	LoginDbmManager();
	~LoginDbmManager();

	void init(LoginDbmServerObject* obj);
	bool openDataStorage(const char* fullFileName, const char* sectionName);
	void rollbackOnError();
	void setAutoCommitTrueOnError();
	void setAutoCommitTrue();
	UINT32 getDbReconnectInterval() const { return dbReconnectInterval; }
	void composeHeLoggerParameters( HeLoggerParameters& parameters ) const;

	// Messages

private:
	// statements
	InsertLoginStmt*		insertLoginStmt;
	GetLoginByLoginIdStmt*	getLoginByLoginIdStmt;
	InsertLogoutStmt*		insertLogoutStmt;
	GetLogoutByLoginIdStmt*	getLogoutByLoginIdStmt;

	void prepareStatements();
	void deleteStatements();
	void zeroStatements();

};

