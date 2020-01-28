////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef GenericDbmmanager_h_included
#define GenericDbmmanager_h_included

////////////////////////////////////////////////////////////////////////////////

#include "dbcommon.h"

class GenericServerObjectBase;

using HeLoggerParameter = pair<const char*, PString>;
using HeLoggerParameters = vector<HeLoggerParameter>;

static constexpr UINT32 DEFAULT_COMMIT_THRESHOLD = 100;
static constexpr UINT32 DEFAULT_DB_RECONNECT_INTERVAL_MSEC = 1000;
static constexpr UINT32 DEFAULT_RANGE_INCREMENT = 100;

////////////////////////////////////////////////////////////////////////////////

class GenericDbmManager : public DatabaseManagerCommon
{
public:

	GenericDbmManager();
	virtual ~GenericDbmManager() { }

	virtual void incErrorsInMessages() { ++errorsInMessages; }

	virtual void init(GenericServerObjectBase* obj);
	virtual void dynamicInit(const PIniFile& iniFile);
	virtual bool openDataStorage(const char* fullFileName, const char* sectionName); //lint !e1511 (Hiding)
	virtual void commit(); //lint !e1511 (Hiding)
	virtual void rollbackOnError();
	virtual void setAutoCommitTrue();
	virtual void composeHeLoggerParameters(HeLoggerParameters& parameters);
	virtual UINT32 getDbReconnectIntervalMsec() const { return dbReconnectIntervalMSec; }
	virtual void setCurrentSchema();
	virtual void prepareStatements() = 0;
	virtual void deleteStatements() = 0;
	virtual void zeroStatements() = 0;

protected:
	GenericServerObjectBase * dbmObj; // Non-owning pointer
	UINT32 commitThreshold;
	UINT32 dbReconnectIntervalMSec;
	UINT32 errorsInMessages;
	UINT32 totalErrorsInMessages;

	// "Life cycle" events. Override as required
	virtual void doPreDynamicInit(const PIniFile& /*iniFile*/) { }
	virtual void doDynamicInitReadSettings(const PIniFile::Section& section);
	virtual void doDynamicInitLogSettings() const;
	virtual void doPostDynamicInit(const PIniFile& /*iniFile*/) { }
	virtual void doPreOpenDataStorage(const char* /*fullFileName*/, const char* /*sectionName*/) { }
	virtual void doPostOpenDataStorage(const char* /*fullFileName*/, const char* /*sectionName*/) { }
};

////////////////////////////////////////////////////////////////////////////////

#endif // GenericDbmmanager_h_included
