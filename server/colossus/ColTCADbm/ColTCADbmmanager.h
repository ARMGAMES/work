// ===========================================================
// Copyright (C) 2018 PYR Software Ltd. All rights reserved.
// ===========================================================

//dbmanager.h

#ifndef ColTCADbmmanager_h_included
#define	ColTCADbmmanager_h_included

#include "ppstring.h"
#include "commlib.h"
#include "dbcommon.h"
#include "dbmstruct.h"
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
	ColTCADbmServerObject*	dbmObj;
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
	INT16 getTCAUsersTournData(TCAColTaskData& tcaColTaskData, PString& sqlErr);
	INT16 getTCAPairsByUidStartLetter(TCAColTaskData& tcaColTaskData, char uidStartLetter, PString& sqlErr);
	INT16 getTCATaskExtent(TCAColTaskData& tcaColTaskData, PString& sqlErr);

private:
	void setCurrentSchema();

	// statements

	void prepareStatements();
	void deleteStatements();
	void zeroStatements();

	// PYR-27418
	const DbmGenerator::Generator* getGenerators( size_t& generatorsSize ) const override;
};

#endif	//ColTCADbmanager_h_included
