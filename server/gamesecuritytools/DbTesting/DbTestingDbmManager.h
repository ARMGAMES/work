#ifndef DbTestingDbmManager_h_included
#define DbTestingDbmManager_h_included

//////////////////////////////////////////////////////////////////////////

#include "dbcommon.h"
#include "DbTestingDbmPrepStmt.h"

#define DEFAULT_DB_RECONNECT_INTERVAL		1000
#define DEFAULT_STMT_SLEEP_INTERVAL			(1000*60*2)


class DbTestingObject;

//////////////////////////////////////////////////////////////////////////

template<typename T>
class AutoPtr : public PAutoPtr<T>
{
public:
	explicit AutoPtr(T* p_ = 0) : PAutoPtr(p_) {}

	//lint -e(1721)
	void operator=(T* p_)
	{
		PAutoPtr<T> temp(p_);
		swap(temp);
	}
};

//////////////////////////////////////////////////////////////////////////

class DbTestingDbmManager : public DatabaseManagerCommon
{
public:
	DbTestingDbmManager();
	~DbTestingDbmManager();

	void init(DbTestingObject* obj) { dbmObj = obj; }
	void readDynamicConfiguration(PIniFile& iniFile);
	bool openDataStorage(const char* fullFileName, const char* sectionName);
	UINT32 getDbReconnectInterval() const { return dbReconnectInterval; }

	void setCurrentSchema();

	INT16 getAggEndDate(SrvDate& endDate, PString& errBuf);

private:

	// Statements
	GetLastFinishedAggregationStmt*			getDateStmt;
	AutoPtr<GetLastFinishedAggregationStmt>	getDateAutoPtrStmt;

	void prepareStatements();
	void deleteStatements();
	void zeroStatements();

	// Internal data
	DbTestingObject* dbmObj;
	UINT32 dbReconnectInterval;

	bool useAutoPtrStmt;
	bool sleepBeforeExec;
	bool sleepAfterExec;
	bool deleteStmtsBeforeOpenDataStorage;
	bool deleteStatementsInPrepareStatements;
	bool doSecondStatement;
	UINT32 sleepInterval;
};

//////////////////////////////////////////////////////////////////////////

#endif // DbTestingDbmManager_h_included