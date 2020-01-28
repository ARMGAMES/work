//dbmanager.h

#ifndef PROJECTmanager_h_included
#define	PROJECTmanager_h_included

#include	"ppstring.h"
#include	"commlib.h"
#include	"dbcommon.h"
#include    "dbmstruct.h"
#include	"pyrtime.h"
#include    "PROJECTprepstmt.h"

class	PROJECTServerObject;
typedef pair<const char*, PString> HeLoggerParameter;
typedef vector<HeLoggerParameter> HeLoggerParameters;

class PROJECTManager : public DatabaseManagerCommon
{
private:
	PROJECTServerObject*		dbmObj;
	int							errorsInMessages;
	UINT32						dbReconnectInterval;

public:
	PROJECTManager();
	~PROJECTManager();

	void init(PROJECTServerObject* obj);
	bool openDataStorage(const char* fullFileName, const char* sectionName);
	void commit();
	void rollbackOnError();
	void setAutoCommitTrue();
	UINT32 getDbReconnectInterval() const { return dbReconnectInterval; }
	void composeHeLoggerParameters( HeLoggerParameters& parameters ) const;

private:
	void setCurrentSchema();

	// statements
    SomeDummyStmt*	someDummyStmt;

	void prepareStatements();
	void deleteStatements();
	void zeroStatements();

	// PYR-27418
	const DbmGenerator::Generator* getGenerators( size_t& generatorsSize ) const override;
};

#endif	//PROJECTanager_h_included
