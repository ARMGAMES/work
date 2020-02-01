#ifndef AccumulatorDbManager_h_included
#define	AccumulatorDbManager_h_included

////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "dbcommon.h"
#include "pyrtime.h"
#include "ppiofile.h"
#include "AccumulatorPrepStmt.h"

////////////////////////////////////////////////////////////////////////////////

class AccumulatorDbManager : public DatabaseManagerCommon
{
public:
	AccumulatorDbManager();

	~AccumulatorDbManager();

	void initAccumMode(AccumulationType accumType_, const SrvDate& accumDate_);
	void initAccumFixMode(const SrvDate& startDate_, const PString& fileName_);

	void init();
	void startAccumulation();
	void startAccumWithSQLAccum();

	void startAccumulationFix();

	void commit() { DatabaseManagerCommon::commit(); } 

private:
	const char* toDBDateStr(const SrvDate& date, PString& dateStr);
	void initStmt();
	void createOutputFiles(const char* date);
	void closeOutputFiles();
	void loadAccumAuxData();
	void loadTournROIData();
	void loadHandStatsData();
	void writeAccumDataToFile();
	void writeAccumDataToFileV2();
	bool loadLatestWeeklyAccumStatsData(const AccumAuxKey& accumAuxKey, AccumStatsData& prevAccumData);

	void formatAccumAuxFormat(const AccumAuxKey& auxKey, const AccumAuxDateRange& auxData, PString& str);
	void formatAccumDataFormat(const AccumStatsKey& statsKey, const AccumStatsData& statsData, PString& str);
	void writeCountStats();

	const char* getAccumAuxTableName();
	const char* getHandStatsTableName();
	const char* getTournROITableName();
	const char* getAccumStatsTableName();
	const char* getAccumStatsFileName();
	const char* getAccumAuxInsertFileName();
	const char* getAccumAuxUpdateFileName();
	const char* getAccumCountFileName();
	void getPrevAccumEndDate(SrvDate& date);

	bool checkAccumDate();
	bool isContinuousDateRange(const SrvDate& lastEndDate, const SrvDate& date) const;

	bool checkAccumFixDate(AccumulationType accumType_);
	bool loadFixUsers();
	bool deleteUserDailyAccumData(const char* userId);
	bool deleteUserWeeklyAccumData(const char* userId);

	bool addUserDailyAccumData(const char* userId);
	bool addUserWeeklyAccumData(const char* userId);

	// class variables///////////////
	AccumulationType  accumType;
	SrvDate			  accumStartDate;

	// PYR-113533
	SrvDate			  accumFixStartDate;
	SrvDate			  accumFixDayEndDate;
	SrvDate			  accumFixWeekEndDate;
	PString			  accumFixFileName;
	vector<PString>   accumFixUsers;

	AccumAuxMap		  accumAuxMap;				// <AccumStatsKey, SrvDate>
	AccumStatsMap	  accumStatsMap;			// <AccumStatsKey, AccumStatsData>

	PIOFile accumStatsFile;						// For PYRCLS.ACCUMSTATSDAY/ACCUMSTATSWEEK
	PIOFile accumStatsAuxInsertFile;			// For PYRCLS.ACCUMSTATSAUXDAY/ACCUMSTATSAUXWEEK
	PIOFile accumStatsAuxUpdateFile;			// For PYRCLS.ACCUMSTATSAUXDAY/ACCUMSTATSAUXWEEK
	PIOFile accumCountsFile;

	UINT32 readHandCount;
	UINT32 readTournCount;
	UINT32 readAccumAuxCount;
	UINT32 readHandSeconds;
	UINT32 readTournSeconds;
	UINT32 readAccumAuxSeconds;

	UINT32 readAccumulatedCount;
	UINT32 readAccumulatedSeconds;
	UINT32 insertAuxCount;
	UINT32 updateAuxCount;

	UINT32 accumOverflowCount;
	/////////////////////////////////

	// Setting variables ///////////
	PString directory;

	////////////////////////////////

	ReadLatestAccumAuxDataStmt* readLatestAccumAuxWeekStmt; // used for daily accumulation
	ReadAccumStatsStmt* readAccumStatsWeekStmt;			    // used for daily accumulation

	DeleteAccumAuxStmt* deleteAccumDayAuxStmt;
	DeleteAccumAuxStmt* deleteAccumWeekAuxStmt;

	FixAccumAuxEndDateStmt* fixAccumDayAuxEndDateStmt;
	FixAccumAuxEndDateStmt* fixAccumWeekAuxEndDateStmt;

	DeleteAccumDataStmt* deleteAccumDayDataStmt;
	DeleteAccumDataStmt* deleteAccumWeekDataStmt;

	UpdateAccumAuxEndDateStmt* updateAccumDayAuxEndDateStmt;
	UpdateAccumAuxEndDateStmt* updateAccumWeekAuxEndDateStmt;

	InsertAccumAuxStmt* insertAccumDayAuxStmt;
	InsertAccumAuxStmt* insertAccumWeekAuxStmt;

	InsertAccumStatsStmt* insertAccumDayStatsStmt;
	InsertAccumStatsStmt* insertAccumWeekStatsStmt;

	AccumulateUserDailyDataStmt* accumUserDayStmt;
	AccumulateUserWeeklyDataStmt* accumUserWeekStmt;

};

////////////////////////////////////////////////////////////////////////////////

#endif // AccumulatorDbManager_h_included
