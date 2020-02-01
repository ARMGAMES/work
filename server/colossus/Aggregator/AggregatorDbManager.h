#ifndef AggregatorDbManager_h_included
#define	AggregatorDbManager_h_included

////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "dbcommon.h"
#include "pyrtime.h"
#include "ppiofile.h"
#include "ColossusCommon.h"
#include "AggregatorPrepStmt.h"
#include "AggregatorConstants.h"

////////////////////////////////////////////////////////////////////////////////

class AggregatorDbManager : public DatabaseManagerCommon
{
public:

#pragma warning(push)
#pragma warning(disable: 4355)

	AggregatorDbManager() :
		DatabaseManagerCommon(),
		maxRingSessionTableBreak(DEFAULT_SESSION_BREAK), 
		maxRingSessionBreak(DEFAULT_SESSION_BREAK), 
		maxTournSessionBreak(DEFAULT_SESSION_BREAK), 
		maxBlitzSessionBreak(DEFAULT_SESSION_BREAK),
		maxTournEntrySize(MAX_TOURN_ENTRY_SIZE_DEFAULT),
		consistencyThreshold(CONSISTENCY_THRESHOLD),
		writeToDb(false),
		commitThreshold(COMMIT_THRESHOLD),
		commitTimeout(COMMIT_TIMEOUT),
		maxDeferredDays(0),
		checkTodayTournRecords(DEFAULT_CHECK_TODAY_TOURN_RECS),
		useDeferredTable(false),
		checkNextDayRecords(false),
		includedHostIdsMask(eHost_Unknown),
		pendingCommits(0),
		lastCommit(0),
		maxTournSessionsCount(0),
		maxHandSessionsCount(0),
		writtenHandsRecords(0),
		writtenIntellipokerRecords(0),
		writtenMicromillionsRecords(0),
		writtenMicromillions10Records(0),
		readDeferredHands(0),
		writtenDeferredHands(0),
		writtenProfitsRecords(0),
		writtenTournPartnersRecords(0),
		writtenHandSessionsRecords(0),
		writtenTournSessionsRecords(0),
		writtenConsistencyRecords(0),
		writtenAggSessionsRecords(0),
		maxTournPartnersCount(0),
		maxConsistencyCount(0),
		handStats(*this),
		pWriteTournDataStmt(0),
		sideTableJoinId(0)
	{
		initStatsFlagsDescriptors(statsDescriptors);
	}

#pragma warning(pop)

	~AggregatorDbManager();

	void init(const char* fullFileName, const SrvDate& day);
	void aggregateRawHands(const SrvDate& day);

	void commit() { DatabaseManagerCommon::commit(); } //lint !e1511

private:
	void createOutputFiles(const char* date);
	void closeOutputFiles();
	void writeStats(const char* date, UINT32 num, UINT32 secRead);

	void checkCommitThreshold();
	void checkCommitTimeout();
	void doCommit();

	void writeAndResetDailyData(const char* date, AggregateHandData& data, HandPartnersMap& handPartners);
	void writeAndResetTournPartnerData(const SrvDate& day, const char* date, const char* userId, TournPartnersMap& tournPartners);
	void writeAndResetConsistencyData(const char* date, const char* userId, ConsistencyMap& consistencyMap);
	void writeDeferredHand();

	// PYR-32185
	void writeAndResetIntellipokerData(const char* date, AggregateHandData& data, PIOFile& outputFile, UINT32& recordCounter) const;

	// Session aggregation related helper functions
	void writeAndResetSessionData(const char* userId, UINT32 sessionType, 
								  TournSessionMap& tournSessionMap, HandSessionMap& handSessionMap, 
								  TimeIntervalVec& intervals, UINT32 maxInsts[]);

	void writeAggregateSession(const char* date, const char* userId, UINT32 secondsAtTable, UINT32 hostId, const UINT32 (&maxInsts)[COLOSSUS_AGGSESSIONGAMETYPE_MAX + 1]);

	UINT32 calcAggSessionMaxInstances(const TimeIntervalVec& intervals) const;

	template <typename KeyType>
	UINT32 writeSingleSessionData(const map<KeyType, SingleSessionDataVec>& dataMap);

	// PYR-45403
	bool ignoreRecordByHost(UINT32 recordHostId) const;
	void incRecordsPerHost(UINT32 recordHostId);

	static const UINT32 DEFAULT_SESSION_BREAK = 15;
	static const UINT32 MAX_TOURN_ENTRY_SIZE_DEFAULT = 27;
	static const UINT32 CONSISTENCY_THRESHOLD = 1000;
	static const UINT32 COMMIT_THRESHOLD = 0;	// no combining
	static const UINT32 COMMIT_TIMEOUT = 1;		// seconds
	static const INT32 DEFAULT_CHECK_TODAY_TOURN_RECS = 1; // Default for production

	PString directory;
	UINT32 maxRingSessionTableBreak;	// A (ring game) table where a user has not played a hand for "X" minutes is considered a new table
	UINT32 maxRingSessionBreak;			// max # of mins between distinct sessions before a new session is created
	UINT32 maxTournSessionBreak;		// max # of mins between distinct sessions before a new session is created
	UINT32 maxBlitzSessionBreak;		// max # of mins between distinct sessions before a new session is created
	UINT32 maxTournEntrySize;			// max tourn entry size to aggregate
	UINT32 consistencyThreshold;		// Drop consistency data for players with less hands than the threshold
	bool writeToDb;
	UINT32 commitThreshold;				// Batch commits
	UINT32 commitTimeout;
	UINT32 maxDeferredDays;				// PYR-25820.  Limit on deferred hands checking
	INT32 checkTodayTournRecords;		// PYR-81841.  Expected to be set in QA-only. Turn off to skip checking TournDataInt table for records from "today"

	PString handStatsRawTableName;
	PString tournDataTableName;			// DB table name for tourn data
	PString completedTournsTableName;	// PYR-25820.  DB table name for completed tourns

	bool useDeferredTable;				// PYR-25820.  DB table name for deferred hands
	PString deferredHandsTableName;

	UINT32 checkNextDayRecords;			// PYR-27866
	UINT32 includedHostIdsMask;			// PYR-45403

	PIOFile statsFile;					// For PYRCLS.HandStatsDay
	PIOFile stats2File;					// PYR-62032 For PYRCLS.HandStats2Day
	PIOFile tournDataFile;				// For PYRCOL.TournDataInt
	PIOFile profitsFile;				// For PYRCLS.HandPartnersDay
	PIOFile sessionFile;				// For PYRCLS.SessionDay
	PIOFile countsFile;
	PIOFile consistencyFile;			// For PYRCLS.HandConsistencyDay
	PIOFile deferredHandsFile;			// For PYRCOL.HandStatsDefer
	PIOFile intellipokerFile;
	PIOFile micromillionsFile;
	PIOFile micromillions10File;		// PYR-34683
	PIOFile aggSessionFile;				// For PYRCLS.SessionAggDay

	UINT32 pendingCommits;
	time_t lastCommit;

	UINT32 maxTournSessionsCount;
	UINT32 maxHandSessionsCount;
	PString handsMaxSessionUserId;
	PString tournsMaxSessionUserId;

	UINT32 writtenHandsRecords;
	UINT32 writtenIntellipokerRecords;
	UINT32 writtenMicromillionsRecords;
	UINT32 writtenMicromillions10Records;
	UINT32 readDeferredHands;
	UINT32 writtenDeferredHands;
	UINT32 writtenProfitsRecords;
	UINT32 writtenTournPartnersRecords;
	UINT32 writtenHandSessionsRecords;
	UINT32 writtenTournSessionsRecords;
	UINT32 writtenConsistencyRecords;
	UINT32 writtenAggSessionsRecords;
	UINT32 maxTournPartnersCount;
	UINT32 maxConsistencyCount;

	typedef map<UINT32 /* hostId */, UINT32 /* count */> NumRecordsPerHost;
	NumRecordsPerHost numRecsPerHost;

	HandStatsManager handStats;
	WriteTournDataStmt* pWriteTournDataStmt;

	UINT32 sideTableJoinId;
	STATS_FLAGS_DESCRIPTORS statsDescriptors;
};

////////////////////////////////////////////////////////////////////////////////

#endif // AggregatorDbManager_h_included
