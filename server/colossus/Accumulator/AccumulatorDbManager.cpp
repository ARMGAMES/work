////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "AccumulatorDbManager.h"
#include "counters.h"
#include "../utils.h"

#define INI_FILE "accumulator.ini"

static void writeToFile(PIOFile& file, const PString& str)
{
	file.writeFile(reinterpret_cast<const BYTE*>(str.c_str()), str.length()); 
}

////////////////////////////////////////////////////////////////////////////////

const char* AccumulatorDbManager::toDBDateStr(const SrvDate& date, PString& dateStr)
{
	char buff[9];
	sprintf(buff, "%04.4u%02.2u%02.2u", date._year, date._month, date._day);
	dateStr = buff;

	return dateStr;
}

void AccumulatorDbManager::initAccumMode(AccumulationType accumType_, const SrvDate& accumDate_)
{
	PLog("initAccumMode");

	accumType = accumType_;
	accumStartDate = accumDate_;

	PString dummy;
	PLog("accum type=%d, accum date=%s", accumType, srvDateToString(accumStartDate, dummy));

	init();
}

void AccumulatorDbManager::initAccumFixMode(const SrvDate& startDate_, const PString& fileName_)
{
	PLog("initAccumFixMode");

	accumFixStartDate = startDate_;
	accumFixFileName = fileName_;

	PString dummy;
	PLog("accum fix start date=%s", srvDateToString(accumFixStartDate, dummy));
	PLog("accum fix file name=%s", accumFixFileName.c_str());

	init();
}


void AccumulatorDbManager::init()
{
	PString iniFileName(INI_FILE);
	PIniFile iniFile(iniFileName);
	const PIniFile::Section* pSection = iniFile.getSection("SETTINGS");

	if (pSection)
	{
		directory = pSection->getProperty("directory", "");
		PSTR_SIZE_T len = directory.length();
		if (len && directory[len] != FILESEPARATORCHAR)
		{
			directory.append(FILESEPARATORCHAR);
		}

	}
	else
	{
		PLog("Warning: section [SETTINGS] is missing in INI file %s.  Default values are used", iniFileName.c_str());
	}

	PLog("- Output directory set to: %s", directory.c_str());


	if (!DatabaseManagerCommon::openDataStorage(iniFileName, "MAINSTORAGE"))
	{
		throw PError("Cannot open data storage");
	}

	initStmt();
	PString exec;
	DbmStatement stmt(*this);

	if (*schemaStr)
	{
		PLog("Setting current schema to '%s'", schemaStr.c_str());
		exec.assign("SET CURRENT SCHEMA ").append(schemaStr);
		stmt.execDirect(exec);
	}
}

void AccumulatorDbManager::initStmt()
{
	readLatestAccumAuxWeekStmt = new ReadLatestAccumAuxDataStmt(*this, TBL_ACCUMSTATSAUXWEEK);
	readAccumStatsWeekStmt = new ReadAccumStatsStmt(*this, TBL_ACCUMSTATSWEEK);

	deleteAccumDayAuxStmt = new DeleteAccumAuxStmt(*this, TBL_ACCUMSTATSAUXDAY);
	deleteAccumWeekAuxStmt = new DeleteAccumAuxStmt(*this, TBL_ACCUMSTATSAUXWEEK);

	fixAccumDayAuxEndDateStmt = new FixAccumAuxEndDateStmt(*this, TBL_ACCUMSTATSAUXDAY);
	fixAccumWeekAuxEndDateStmt = new FixAccumAuxEndDateStmt(*this, TBL_ACCUMSTATSAUXWEEK);

	deleteAccumDayDataStmt = new DeleteAccumDataStmt(*this, TBL_ACCUMSTATSDAY);
	deleteAccumWeekDataStmt = new DeleteAccumDataStmt(*this, TBL_ACCUMSTATSWEEK);

	updateAccumDayAuxEndDateStmt = new UpdateAccumAuxEndDateStmt(*this, TBL_ACCUMSTATSAUXDAY);
	updateAccumWeekAuxEndDateStmt = new UpdateAccumAuxEndDateStmt(*this, TBL_ACCUMSTATSAUXWEEK);

	insertAccumDayAuxStmt = new InsertAccumAuxStmt(*this, TBL_ACCUMSTATSAUXDAY);
	insertAccumWeekAuxStmt = new InsertAccumAuxStmt(*this, TBL_ACCUMSTATSAUXWEEK);

	insertAccumDayStatsStmt = new InsertAccumStatsStmt(*this, TBL_ACCUMSTATSDAY);
	insertAccumWeekStatsStmt = new InsertAccumStatsStmt(*this, TBL_ACCUMSTATSWEEK);

	accumUserDayStmt = new AccumulateUserDailyDataStmt(*this);
	accumUserWeekStmt = new AccumulateUserWeeklyDataStmt(*this);

}

bool AccumulatorDbManager::checkAccumDate()
{
	if (accumType == ACCUM_WEEKLY)
	{
		// Weekly accumulation date must be on sunday
		if (!ColUtils::isSunday(accumStartDate))
		{
			PLog("Error: Weekly accumulation date is not on Sunday.");
			return false;
		}
	}
	
	SrvDate endDay;
	GetMaintJobEndDateStmt getJobEndDate(*this);

	// check if data has been aggregated 
	getJobEndDate.execGet(getHandStatsTableName(), endDay);
	if (accumStartDate.encode() > endDay.encode())
	{
		PLog("Error:%s end day is less than accumulation day.", getHandStatsTableName());
		return false;
	}

	getJobEndDate.execGet(getTournROITableName(), endDay);
	if (accumStartDate.encode() > endDay.encode())
	{
		PLog("Error:%s end day is less than accumulation day.", getTournROITableName());
		return false;
	}

	// if ACCUMSTATSDAY end day is null, it's first time to run accumulation. 
	// if ACCUMSTATSDAY end day is not null, accumulation day should be next day. If there is a gap, return false.  
	getJobEndDate.execGet(getAccumStatsTableName(), endDay);
	if (!endDay.isNull())
	{
		SrvDate day;
		normalizeSrvDate(endDay, day, 1);
		if (accumStartDate.encode() != day.encode())
		{
			PString dummy;
			PLog("Error: expect accumulation date:%s", srvDateToString(day, dummy));
			return false;
		}
	}

	return true;
}

const char* AccumulatorDbManager::getAccumAuxTableName()
{
	if (accumType == ACCUM_DAILY)
	{
		return TBL_ACCUMSTATSAUXDAY;
	}
	else if (accumType == ACCUM_WEEKLY)
	{
		return TBL_ACCUMSTATSAUXWEEK;
	}
	return "";
}

const char* AccumulatorDbManager::getHandStatsTableName()
{
	if (accumType == ACCUM_DAILY)
	{
		return TBL_HANDSTATSDAY;
	}
	else if (accumType == ACCUM_WEEKLY)
	{
		return TBL_HANDSTATSWEEK;
	}
	return "";
}

const char* AccumulatorDbManager::getTournROITableName()
{
	if (accumType == ACCUM_DAILY)
	{
		return TBL_TOURNROIDAY;
	}
	else if (accumType == ACCUM_WEEKLY)
	{
		return TBL_TOURNROIWEEK;
	}
	return "";
}

const char* AccumulatorDbManager::getAccumStatsTableName()
{
	if (accumType == ACCUM_DAILY)
	{
		return TBL_ACCUMSTATSDAY;
	}
	else if (accumType == ACCUM_WEEKLY)
	{
		return TBL_ACCUMSTATSWEEK;
	}
	return "";
}

const char* AccumulatorDbManager::getAccumStatsFileName()
{
	if (accumType == ACCUM_DAILY)
	{
		return FILE_ACCUMSTATSDAY;
	}
	else if (accumType == ACCUM_WEEKLY)
	{
		return FILE_ACCUMSTATSWEEK;
	}
	return "";
}

const char* AccumulatorDbManager::getAccumAuxInsertFileName()
{
	if (accumType == ACCUM_DAILY)
	{
		return FILE_ACCUMSTATSAUXDAY_INSERT;
	}
	else if (accumType == ACCUM_WEEKLY)
	{
		return FILE_ACCUMSTATSAUXWEEK_INSERT;
	}
	return "";
}

const char* AccumulatorDbManager::getAccumAuxUpdateFileName()
{
	if (accumType == ACCUM_DAILY)
	{
		return FILE_ACCUMSTATSAUXDAY_UPDATE;
	}
	else if (accumType == ACCUM_WEEKLY)
	{
		return FILE_ACCUMSTATSAUXWEEK_UPDATE;
	}
	return "";
}

const char* AccumulatorDbManager::getAccumCountFileName()
{
	if (accumType == ACCUM_DAILY)
	{
		return FILE_ACCUMDAY_COUNT;
	}
	else if (accumType == ACCUM_WEEKLY)
	{
		return FILE_ACCUMWEEK_COUNT;
	}
	return "";
}

void AccumulatorDbManager::getPrevAccumEndDate(SrvDate& date)
{
	if (accumType == ACCUM_WEEKLY)
	{
		ColUtils::setToStartOfWeek(date);
	}
}

bool AccumulatorDbManager::isContinuousDateRange(const SrvDate& lastEndDate, const SrvDate& date) const
{
	SrvDate nextRangeDate;

	if (accumType == ACCUM_DAILY)
	{
		normalizeSrvDate(lastEndDate, nextRangeDate, 1);
	}
	else if (accumType == ACCUM_WEEKLY)
	{
		normalizeSrvDate(lastEndDate, nextRangeDate, 7);
	}

	return nextRangeDate.encode() == date.encode();
}

void AccumulatorDbManager::loadAccumAuxData()
{
	HiresCounter counter;
	PLog("Start loading accumulation aux data.");

	// Load aux table into memory
	ReadAccumAuxStmt readAccumAux(*this, getAccumAuxTableName());
	readAccumAux.execGet(accumAuxMap);

	counter.stop();
	readAccumAuxSeconds= counter.getElapsedTick()/TICKS_PER_SECOND;
	readAccumAuxCount = accumAuxMap.size();
	PLog("Finished loading. Time=%u seconds, Size=%u", readAccumAuxSeconds, readAccumAuxCount);
}

void AccumulatorDbManager::loadTournROIData()
{
	HiresCounter counter;
	PLog("Start loading tourn data.");

	// Load tourn roi data
	ReadAccumTournROIStmt readAccumTournROIStmt(*this, accumStartDate, getTournROITableName());
	readAccumTournROIStmt.execGet(accumStatsMap, readTournCount, accumOverflowCount);

	counter.stop();
	readTournSeconds = counter.getElapsedTick()/TICKS_PER_SECOND;
	PLog("Finished loading. Time=%u seconds, Size=%u", readTournSeconds, readTournCount);
}

void AccumulatorDbManager::loadHandStatsData()
{
	HiresCounter counter;
	PLog("Start loading hand data.");

	// Load hand stats data
	// HANDSTATSWEEK table has approximately 1.5m recors grouped by userid, currency, and ringTournGame each week
	// Each record has 576 bytes.  Total size from hand is around 1.5m * 576 bytes = 864MB.

	ReadAccumHandStmt readAccumHandStmt(*this, accumStartDate, getHandStatsTableName());
	readAccumHandStmt.execGet(accumStatsMap, readHandCount, accumOverflowCount);

	counter.stop();
	readHandSeconds = counter.getElapsedTick()/TICKS_PER_SECOND;
	PLog("Finished loading. Time=%u seconds, Size=%u", readHandSeconds, readHandCount);
}

bool AccumulatorDbManager::loadLatestWeeklyAccumStatsData(const AccumAuxKey& accumAuxKey, AccumStatsData& prevAccumData)
{
	AccumAuxDateRange auxData;
	if (readLatestAccumAuxWeekStmt->execGet(accumAuxKey, auxData))
	{
		// if found aux week data, look up accum stats week table to get prev accumulated data
		SrvDate preSunday = auxData.endDate;
		ColUtils::setToStartOfWeek(preSunday);

		AccumStatsKey statsKey(accumAuxKey.userid, preSunday, accumAuxKey.currency, accumAuxKey.ringTournGame);
		if (readAccumStatsWeekStmt->execGet(statsKey, prevAccumData, accumOverflowCount))
		{
			return true;
		}
		else
		{
			PString dummy;
			PLog("ERROR : Missing accum stats for key");
			PLog("-userid=%s", accumAuxKey.userid.c_str());
			PLog("-startdate=%s", srvDateToString(preSunday, dummy));
			PLog("-currency=%s", accumAuxKey.currency.c_str());
			PLog("-ringTournGame=%u", accumAuxKey.ringTournGame);
			return false;
		}
	}
	
	return false;
}

void AccumulatorDbManager::writeAccumDataToFile()
{
	HiresCounter counter;
	PLog("Start writing accum data to file.");

	ReadLatestAccumAuxDataStmt readLatestAccumAuxStmt(*this, getAccumAuxTableName());
	ReadAccumStatsStmt readAccumStatsStmt(*this, getAccumStatsTableName());

	AccumStatsMap::iterator it = accumStatsMap.begin();

	while(it != accumStatsMap.end())
	{
		const AccumStatsKey& currStatsKey = it->first;
		AccumStatsData& currStatsData = it->second;

		AccumAuxKey currAuxKey(currStatsKey.userid, currStatsKey.currency, currStatsKey.ringTournGame);
		AccumAuxDateRange currAuxDateRange;

		bool isStatsKeyInAux = false;
		bool isUpdataAccumAux = false;

		/*if (loadAuxTableOption == NO_PRELOAD_AUXTABLE)
		{
			isStatsKeyInAux = readLatestAccumAuxStmt.execGet(currAuxKey, currAuxDateRange);
		}
		else if (loadAuxTableOption == PRELOAD_AUXTABLE)
		{
			accumAuxMap.checkAccumStatsKey(currStatsKey, isStatsKeyInAux, currAuxDateRange);
		}*/
		
		// if find stats key in aux table, look up accumstats table to accumulate previous stats.
		if (isStatsKeyInAux)
		{
			SrvDate nextDay;
			normalizeSrvDate(currAuxDateRange.endDate, nextDay, 1);

			if (accumStartDate.encode() == nextDay.encode())
			{
				// Update aux record
				isUpdataAccumAux = true;
				++updateAuxCount;				
			}
			else
			{
				// Create new Aux recrod
				currAuxDateRange.startDate = accumStartDate;
				++insertAuxCount;
			}
			
			// look up prev stats record
			SrvDate preEndDate = currAuxDateRange.endDate;
			getPrevAccumEndDate(preEndDate);
			AccumStatsKey prevStatsKey(currStatsKey.userid, preEndDate, currStatsKey.currency, currStatsKey.ringTournGame);
			AccumStatsData prevAccumStats;
			
			if (readAccumStatsStmt.execGet(prevStatsKey, prevAccumStats, accumOverflowCount))
			{
				currStatsData += prevAccumStats;
			}
			else
			{
				PString dummy;
				PLog("ERROR : Missing accum stats for key");
				PLog("-userid=%s", prevStatsKey.userid.c_str());
				PLog("-startdate=%s", srvDateToString(prevStatsKey.startDate, dummy));
				PLog("-currency=%s", prevStatsKey.currency.c_str());
				PLog("-ringTournGame=%u", prevStatsKey.ringTournGame);
			}
		}
		else
		{
			// For daily accumulation, if there is no data in daily aux table, we need to look up latest weekly data to accumulate.
			if (accumType == ACCUM_DAILY)
			{
				AccumStatsData prevAccumStats;
				if (loadLatestWeeklyAccumStatsData(currAuxKey, prevAccumStats))
				{
					currStatsData += prevAccumStats;
				}
			}

			// Create new Aux recrod
			currAuxDateRange.startDate = accumStartDate;
			++insertAuxCount;
		}

		currAuxDateRange.endDate = accumStartDate;

		PString accumAuxStr, accumStatsStr;
		formatAccumAuxFormat(currAuxKey, currAuxDateRange, accumAuxStr);
		formatAccumDataFormat(currStatsKey, currStatsData, accumStatsStr);

		if (isUpdataAccumAux)
		{
			writeToFile(accumStatsAuxUpdateFile, accumAuxStr);
		}
		else
		{
			writeToFile(accumStatsAuxInsertFile, accumAuxStr);
		}

		writeToFile(accumStatsFile, accumStatsStr);

		++readAccumulatedCount;
		++it;
	}

	counter.stop();
	readAccumulatedSeconds = counter.getElapsedTick()/TICKS_PER_SECOND;
	PLog("Finished writing. Time=%u seconds", readAccumulatedSeconds);

	writeCountStats();
}

void AccumulatorDbManager::startAccumulation()
{
	PString error;
	PString dateStr;

	toDBDateStr(accumStartDate, dateStr);

	if (!checkAccumDate())
	{
		error.append("Invalid accumulation date:").append(dateStr.c_str());
		throw PError(error);
	}

	createOutputFiles(dateStr);

	// load accumulation aux table
	loadAccumAuxData();

	// load hand stats data
	loadHandStatsData();

	// Load tourn ROI data
	loadTournROIData();

	writeAccumDataToFile();

	closeOutputFiles();
}


void AccumulatorDbManager::writeAccumDataToFileV2()
{
	HiresCounter counter;
	PLog("Start loading weekly accumulated data.");

	AccumStatsKey statsKey;
	AccumAuxDateRange lastDateRange;
	AccumStatsData statsData;

	AccumulateDataBaseStmt* accumStmt;

	if (accumType == ACCUM_DAILY)
	{
		accumStmt = new AccumulateDailyDataStmt(*this, accumStartDate);
	}
	else if (accumType == ACCUM_WEEKLY)
	{
		accumStmt = new AccumulateWeeklyDataStmt(*this, accumStartDate);
	}
	else
	{
		PString error;
		error.append("Invalid accumType:").appendUint(accumType);
		PLog("Error: - %s", error.c_str());
		throw PError(error);
	}

	accumStmt->execute();
	while (accumStmt->fetch())
	{
		UINT32 currOverflowCount = accumOverflowCount;
		accumStmt->getAccumData(statsKey, lastDateRange, statsData, accumOverflowCount);
		if (accumOverflowCount > currOverflowCount)
		{
			PString dummy;
			PLog("Warning : Accumulated data overflow");
			PLog("-userid=%s", statsKey.userid.c_str());
			PLog("-startdate=%s", srvDateToString(statsKey.startDate, dummy));
			PLog("-currency=%s", statsKey.currency.c_str());
			PLog("-ringTournGame=%u", statsKey.ringTournGame);
		}

		++readAccumulatedCount;

		AccumAuxKey auxKey(statsKey.userid, statsKey.currency, statsKey.ringTournGame);
		AccumAuxDateRange currDateRange;
		currDateRange.endDate = accumStartDate;

		PString accumAuxStr, accumStatsStr;

		if (isContinuousDateRange(lastDateRange.endDate, accumStartDate))
		{
			// update existing aux data
			currDateRange.startDate = lastDateRange.startDate;
			formatAccumAuxFormat(auxKey, currDateRange, accumAuxStr);
			writeToFile(accumStatsAuxUpdateFile, accumAuxStr);

			updateAuxCount++;
		}
		else
		{
			// insert new aux data
			currDateRange.startDate = accumStartDate;
			formatAccumAuxFormat(auxKey, currDateRange, accumAuxStr);
			writeToFile(accumStatsAuxInsertFile, accumAuxStr);

			insertAuxCount++;
		}
		
		formatAccumDataFormat(statsKey, statsData, accumStatsStr);
		writeToFile(accumStatsFile, accumStatsStr);
	}

	accumStmt->closeCursor();

	delete accumStmt;

	counter.stop();
	readAccumulatedSeconds= counter.getElapsedTick()/TICKS_PER_SECOND;
	PLog("Finished loading. Time=%u seconds, Size=%u", readAccumulatedSeconds, readAccumulatedCount);
}

void AccumulatorDbManager::startAccumWithSQLAccum()
{
	PString error;
	PString dateStr;

	toDBDateStr(accumStartDate, dateStr);

	if (!checkAccumDate())
	{
		error.append("Invalid accumulation date:").append(dateStr.c_str());
		throw PError(error);
	}

	createOutputFiles(dateStr);

	writeAccumDataToFileV2();

	writeCountStats();
	closeOutputFiles();
}
////////////////////////////////////////////////////////////////////////////////

void AccumulatorDbManager::formatAccumAuxFormat(const AccumAuxKey& auxKey, const AccumAuxDateRange& auxData, PString& str)
{
	PString dummy1, dummy2;

	ColUtils::dbOutQuote(auxKey.userid, str).
		append(',').append(toDBDateStr(auxData.startDate, dummy1)).
		append(',').append('"').append(auxKey.currency).append('"').
		append(',').appendUint(auxKey.ringTournGame).
		append(',').append(toDBDateStr(auxData.endDate, dummy2)).
		append("\r\n");
}

void AccumulatorDbManager::formatAccumDataFormat(const AccumStatsKey& statsKey, const AccumStatsData& statsData, PString& str)
{
	PString dummy;

	ColUtils::dbOutQuote(statsKey.userid, str).
		append(',').append(toDBDateStr(statsKey.startDate, dummy)).
		append(',').append('"').append(statsKey.currency).append('"').
		append(',').appendUint(statsKey.ringTournGame).
		append(',').appendInt64(statsData.tournData.totalinvested).
		append(',').appendUint(statsData.tournData.totaltourns).
		append(',').appendUint64(statsData.tournData.usertotalprofit).
		append(',').appendUint64(statsData.tournData.rakepaid).
		append(',').appendUint(statsData.tournData.totaltournswonmoney).
		append(',').appendUint(statsData.handData.totalhands).
		append(',').appendInt64(statsData.handData.totalwin).
		append(',').appendUint64(statsData.handData.totalblindsantespaid).
		append(',').appendUint64(statsData.handData.totalrakepaid).
		append(',').appendUint(statsData.handData.numbigblinds).
		append(',').appendUint(statsData.handData.numsmallblinds).
		append(',').appendUint(statsData.handData.numblindspaidoutofposition).
		append(',').appendUint(statsData.handData.numvpip).
		append(',').appendUint(statsData.handData.numvpipfromsmallblind).
		append(',').appendUint(statsData.handData.numvpipfrombigblind).
		append(',').appendUint(statsData.handData.numsawshowdown).
		append(',').appendUint(statsData.handData.numwonshowdown).
		append(',').appendUint(statsData.handData.numwonafterflop).
		append(',').appendUint(statsData.handData.numlimp).
		append(',').appendUint(statsData.handData.numlimpraise).
		append(',').appendUint(statsData.handData.numlimpcall).
		append(',').appendUint(statsData.handData.numlimpfold).
		append(',').appendUint(statsData.handData.numallinpreflop).
		append(',').appendUint(statsData.handData.numcoldcallopportunities).
		append(',').appendUint(statsData.handData.numcoldcalls).
		append(',').appendUint(statsData.handData.num3betopportunities).
		append(',').appendUint(statsData.handData.num3bets).
		append(',').appendUint(statsData.handData.numfacing3bet).
		append(',').appendUint(statsData.handData.numfoldto3bet).
		append(',').appendUint(statsData.handData.num4plusbet).
		append(',').appendUint(statsData.handData.numsawflop).
		append(',').appendUint(statsData.handData.numsawturn).
		append(',').appendUint(statsData.handData.numsawriver).
		append(',').appendUint(statsData.handData.numsaw7th).
		append(',').appendUint(statsData.handData.numfacing4plusbet).
		append(',').appendUint(statsData.handData.numfoldto4plusbet).
		append(',').appendUint(statsData.handData.numturnspreflop).
		append(',').appendUint(statsData.handData.numturnsfacingbetonflop).
		append(',').appendUint(statsData.handData.numturnsfacingnobetonflop).
		append(',').appendUint(statsData.handData.numturnsfacingbetonturn).
		append(',').appendUint(statsData.handData.numturnsfacingnobetonturn).
		append(',').appendUint(statsData.handData.numturnsfacingbetonriver).
		append(',').appendUint(statsData.handData.numturnsfacingnobetonriver).
		append(',').appendUint(statsData.handData.numturnsfacingbeton7th).
		append(',').appendUint(statsData.handData.numturnsfacingnobeton7th).
		append(',').appendUint(statsData.handData.numlastagressorpreflop).
		append(',').appendUint(statsData.handData.numlastagressoronflop).
		append(',').appendUint(statsData.handData.numlastagressoronturn).
		append(',').appendUint(statsData.handData.numlastagressoronriver).
		append(',').appendUint(statsData.handData.numcontbetonflop).
		append(',').appendUint(statsData.handData.numcontbetonturn).
		append(',').appendUint(statsData.handData.numcontbetonriver).
		append(',').appendUint(statsData.handData.numcontbeton7th).
		append(',').appendUint(statsData.handData.numbetpreflop).
		append(',').appendUint(statsData.handData.numbetonflop).
		append(',').appendUint(statsData.handData.numbetonturn).
		append(',').appendUint(statsData.handData.numbetonriver).
		append(',').appendUint(statsData.handData.numbeton7th).
		append(',').appendUint(statsData.handData.numcheckonflop).
		append(',').appendUint(statsData.handData.numcheckonturn).
		append(',').appendUint(statsData.handData.numcheckonriver).
		append(',').appendUint(statsData.handData.numcheckon7th).
		append(',').appendUint(statsData.handData.numfoldpreflop).
		append(',').appendUint(statsData.handData.numfoldonflop).
		append(',').appendUint(statsData.handData.numfoldonturn).
		append(',').appendUint(statsData.handData.numfoldonriver).
		append(',').appendUint(statsData.handData.numfoldon7th).
		append(',').appendUint(statsData.handData.numcallspreflop).
		append(',').appendUint(statsData.handData.numcallsonflop).
		append(',').appendUint(statsData.handData.numcallsonturn).
		append(',').appendUint(statsData.handData.numcallsonriver).
		append(',').appendUint(statsData.handData.numcallson7th).
		append(',').appendUint(statsData.handData.numraisespreflop).
		append(',').appendUint(statsData.handData.numraisesonflop).
		append(',').appendUint(statsData.handData.numraisesonturn).
		append(',').appendUint(statsData.handData.numraisesonriver).
		append(',').appendUint(statsData.handData.numraiseson7th).
		append(',').appendUint(statsData.handData.numcheckcallonflop).
		append(',').appendUint(statsData.handData.numcheckcallonturn).
		append(',').appendUint(statsData.handData.numcheckcallonriver).
		append(',').appendUint(statsData.handData.numcheckcallon7th).
		append(',').appendUint(statsData.handData.numcheckraiseonflop).
		append(',').appendUint(statsData.handData.numcheckraiseonturn).
		append(',').appendUint(statsData.handData.numcheckraiseonriver).
		append(',').appendUint(statsData.handData.numcheckraiseon7th).
		append(',').appendUint(statsData.handData.numcheckfoldonflop).
		append(',').appendUint(statsData.handData.numcheckfoldonturn).
		append(',').appendUint(statsData.handData.numcheckfoldonriver).
		append(',').appendUint(statsData.handData.numcheckfoldon7th).
		append(',').appendUint(statsData.handData.numopenfoldpowerfulhand).
		append(',').appendUint(statsData.handData.numfoldtoextremeodds).
		append(',').appendUint(statsData.handData.numreraisewithgarbage).
		append(',').appendUint(statsData.handData.numcheckbehindonriver).
		append(',').appendUint(statsData.handData.numfoldwhencouldcheck).
		append(',').appendUint(statsData.handData.numattemptstealblinds).
		append(',').appendUint(statsData.handData.numfoldtostealfromsbopps).
		append(',').appendUint(statsData.handData.numfoldtostealfromsb).
		append(',').appendUint(statsData.handData.numfoldtostealfrombbopps).
		append(',').appendUint(statsData.handData.numfoldtostealfrombb).
		append(',').appendUint(statsData.handData.numcheckraiseoppsonflop).
		append(',').appendUint(statsData.handData.numcheckraiseoppsonturn).
		append(',').appendUint(statsData.handData.numcheckraiseoppsonriver).
		append(',').appendUint(statsData.handData.numcheckraiseoppson7th).
		append(',').appendUint(statsData.handData.numattemptstealblindsopps).
		append(',').appendUint(statsData.handData.num4betopportunities).
		append(',').appendUint(statsData.handData.numhandsraisedpreflop).
		append(',').appendUint(statsData.handData.numcallbetsonlypreflop).
		append(',').appendUint(statsData.handData.numcallbetsonlyonflop).
		append(',').appendUint(statsData.handData.numcallbetsonlyonturn).
		append(',').appendUint(statsData.handData.numcallbetsonlyonriver).
		append(',').appendUint(statsData.handData.numcallbetsonlyon7th).
		append(',').appendUint(statsData.handData.numturnsfacingbetonlyonflop).
		append(',').appendUint(statsData.handData.numturnsfacingbetonlyonturn).
		append(',').appendUint(statsData.handData.numturnsfacingbetonlyonriver).
		append(',').appendUint(statsData.handData.numturnsfacingbetonlyon7th).
		append(',').appendUint(statsData.handData.numfoldtobetonlypreflop).
		append(',').appendUint(statsData.handData.numfoldtobetonlyonflop).
		append(',').appendUint(statsData.handData.numfoldtobetonlyonturn).
		append(',').appendUint(statsData.handData.numfoldtobetonlyonriver).
		append(',').appendUint(statsData.handData.numfoldtobetonlyon7th).
		append(',').appendUint(statsData.handData.totalplayersinhands).
		append(',').appendUint64(statsData.handData.totalbet).
		append(',').appendUint(statsData.handData.numfacing3bet_hem).
		append(',').appendUint(statsData.handData.numfoldto3bet_hem).
		append(',').appendUint(statsData.handData.numfacing4plusbet_hem).
		append(',').appendUint(statsData.handData.numfoldto4plusbet_hem).
		append(',').appendUint(statsData.handData.numcontbetoppsvs2inposonflop).
		append(',').appendUint(statsData.handData.numcontbetoppsvs2inposonturn).
		append(',').appendUint(statsData.handData.numcontbetoppsvs2inposonriver).
		append(',').appendUint(statsData.handData.numcontbetoppsvs2inposon7th).
		append(',').appendUint(statsData.handData.numcontbetvs2inposonflop).
		append(',').appendUint(statsData.handData.numcontbetvs2inposonturn).
		append(',').appendUint(statsData.handData.numcontbetvs2inposonriver).
		append(',').appendUint(statsData.handData.numcontbetvs2inposon7th).
		append("\r\n");
}

void AccumulatorDbManager::writeCountStats()
{
	PString date;
	toDBDateStr(accumStartDate, date);

	PString str;
	str.
		assign(date).append(",\"READHANDSTATS\",").appendUint(readHandCount).append(",").appendUint(readHandSeconds).append("\n").
		append(date).append(",\"READTOURNROI\",").appendUint(readTournCount).append(",").appendUint(readTournSeconds).append("\n").
		append(date).append(",\"READACCUMAUX\",").appendUint(readAccumAuxCount).append(",").appendUint(readAccumAuxSeconds).append("\n").
		append(date).append(",\"ACCUMSTATS\",").appendUint(readAccumulatedCount).append(",").appendUint(readAccumulatedSeconds).append("\n").
		append(date).append(",\"INSERTACCUMAUX\",").appendUint(insertAuxCount).append(",0\n").
		append(date).append(",\"UPDATEACCUMAUX\",").appendUint(updateAuxCount).append(",0\n").
		append(date).append(",\"ACCUMOVERFLOWCOUNT\",").appendUint(accumOverflowCount).append(",0\n")
		;

	writeToFile(accumCountsFile, str);
}

////////////////////////////////////////////////////////////////////////////////

void AccumulatorDbManager::createOutputFiles(const char* date)
{
	PLog("Opening files for %s in %s", date, directory.length() ? directory.c_str() : "current directory");

	PString file;

	file.assign(directory).append(getAccumStatsFileName()).append(".").append(date);
	accumStatsFile.openFile(file, PLIB_FILE_CREATE_OR_OPEN | PLIB_FILE_FLAG_TRUNCATE);

	file.assign(directory).append(getAccumAuxInsertFileName()).append(".").append(date);
	accumStatsAuxInsertFile.openFile(file, PLIB_FILE_CREATE_OR_OPEN | PLIB_FILE_FLAG_TRUNCATE);

	file.assign(directory).append(getAccumAuxUpdateFileName()).append(".").append(date);
	accumStatsAuxUpdateFile.openFile(file, PLIB_FILE_CREATE_OR_OPEN | PLIB_FILE_FLAG_TRUNCATE);

	file.assign(directory).append(getAccumCountFileName()).append(".").append(date);
	accumCountsFile.openFile(file, PLIB_FILE_CREATE_OR_OPEN | PLIB_FILE_FLAG_TRUNCATE);
}

////////////////////////////////////////////////////////////////////////////////

void AccumulatorDbManager::closeOutputFiles()
{
	PLog("Closing files...");

	accumStatsFile.closeFile();
	accumStatsAuxInsertFile.closeFile();
	accumStatsAuxUpdateFile.closeFile();
	accumCountsFile.closeFile();

	PLog("Closed files");
}

AccumulatorDbManager::AccumulatorDbManager()
	: DatabaseManagerCommon(),
	accumType(ACCUM_NONE),
	readHandCount(0),
	readTournCount(0),
	readAccumAuxCount(0),
	readHandSeconds(0),
	readTournSeconds(0),
	readAccumAuxSeconds(0),
	readAccumulatedCount(0),
	readAccumulatedSeconds(0),
	insertAuxCount(0),
	updateAuxCount(0),
	readLatestAccumAuxWeekStmt(nullptr),
	readAccumStatsWeekStmt(nullptr),
	deleteAccumDayAuxStmt(nullptr),
	deleteAccumWeekAuxStmt(nullptr),
	fixAccumDayAuxEndDateStmt(nullptr),
	fixAccumWeekAuxEndDateStmt(nullptr),
	deleteAccumDayDataStmt(nullptr),
	deleteAccumWeekDataStmt(nullptr),
	updateAccumDayAuxEndDateStmt(nullptr),
	updateAccumWeekAuxEndDateStmt(nullptr),
	insertAccumDayAuxStmt(nullptr),
	insertAccumWeekAuxStmt(nullptr),
	insertAccumDayStatsStmt(nullptr),
	insertAccumWeekStatsStmt(nullptr),
	accumUserDayStmt(nullptr),
	accumUserWeekStmt(nullptr),
	accumOverflowCount(0)
{
}

AccumulatorDbManager::~AccumulatorDbManager()
{
	try
	{
		delete readLatestAccumAuxWeekStmt;
		delete readAccumStatsWeekStmt;
		delete deleteAccumDayAuxStmt;
		delete deleteAccumWeekAuxStmt;
		delete fixAccumDayAuxEndDateStmt;
		delete fixAccumWeekAuxEndDateStmt;
		delete deleteAccumDayDataStmt;
		delete deleteAccumWeekDataStmt;
		delete updateAccumDayAuxEndDateStmt;
		delete updateAccumWeekAuxEndDateStmt;
		delete insertAccumDayAuxStmt;
		delete insertAccumWeekAuxStmt;
		delete insertAccumDayStatsStmt;
		delete insertAccumWeekStatsStmt;
		delete accumUserDayStmt;
		delete accumUserWeekStmt;
	}
	catch (...) {}
}

////////////////////////////////////////////////////////////////////////////////
void AccumulatorDbManager::startAccumulationFix()
{
	PLog("startAccumulationFix");

	PString error;

	if (!checkAccumFixDate(ACCUM_DAILY))
	{
		error.append("Invalid accumulation fix dates.");
		throw PError(error);
	}

	if (!checkAccumFixDate(ACCUM_WEEKLY))
	{
		error.append("Invalid accumulation fix dates.");
		throw PError(error);
	}

	if (!loadFixUsers())
	{
		error.append("Failed to load fix users.");
		throw PError(error);
	}

	for (const PString& userid : accumFixUsers)
	{
		if (!deleteUserDailyAccumData(userid))
		{
			error.append(" failed to delete accum for user: %s", userid.c_str());
			throw PError(error);
		}
		if (!deleteUserWeeklyAccumData(userid))
		{
			error.append(" failed to delete accum for user: %s", userid.c_str());
			throw PError(error);
		}

		if (!addUserDailyAccumData(userid))
		{
			error.append(" failed to add accum for user: %s", userid.c_str());
			throw PError(error);
		}
		if (!addUserWeeklyAccumData(userid))
		{
			error.append(" failed to add accum for user: %s", userid.c_str());
			throw PError(error);
		}
	}
}

bool AccumulatorDbManager::checkAccumFixDate(AccumulationType accumType_)
{
	PLog("checkAccumFixDate: type=%u", (UINT32)accumType_);

	accumType = accumType_;
	if (!ColUtils::isSunday(accumFixStartDate))
	{
		PLog("Error: Start date must be on Sunday.");
		return false;
	}

	PString dummy;
		
	SrvDate aggStartDate;
	GetMaintJobStartDateStmt getJobStartDate(*this);
	getJobStartDate.execGet(getHandStatsTableName(), aggStartDate);
	PLog("aggStartDate=%s", srvDateToString(aggStartDate, dummy));
	if (aggStartDate.isNull() || accumFixStartDate.encode() < aggStartDate.encode())
	{
		PLog("Error:%s start date is greater than accumulation fix start date.", getHandStatsTableName());
		return false;
	}

	getJobStartDate.execGet(getTournROITableName(), aggStartDate);
	if (aggStartDate.isNull() || accumFixStartDate.encode() < aggStartDate.encode())
	{
		PLog("Error:%s start date is greater than accumulation fix start date.", getTournROITableName());
		return false;
	}

	SrvDate aggEndDate;
	GetMaintJobEndDateStmt getJobEndDate(*this);
	getJobEndDate.execGet(getHandStatsTableName(), aggEndDate);

	if (accumType == ACCUM_WEEKLY)
	{
		ColUtils::setToStartOfWeek(aggEndDate);
	}
	else if (accumType == ACCUM_DAILY)
	{
		if (accumFixStartDate.encode() > aggEndDate.encode())
		{
			PLog("Error:%s end date is less than accumulation fix end date.", getHandStatsTableName());
			return false;
		}
	}
	PLog("aggEndDate=%s", srvDateToString(aggEndDate, dummy));

	if (accumType == ACCUM_DAILY)
	{
		accumFixDayEndDate = aggEndDate;
	}
	else if (accumType == ACCUM_WEEKLY)
	{
		accumFixWeekEndDate = aggEndDate;
	}

	return true;
}

bool AccumulatorDbManager::loadFixUsers()
{
	PLog("loadFixUsers");
	try
	{
		PBlock fileData;
		PIOFile fixUsers;
		fixUsers.openFile(accumFixFileName, PLIB_FILE_READONLY);

		UINT64 sz = fixUsers.getSize();
		fileData.append(sz);
		fixUsers.readFile(fileData.ptr(), sz);

		fixUsers.closeFile();

		PString line;
		const BYTE* pStart = fileData.ptr();
		const BYTE* pEnd = pStart + fileData.size();
		while (pStart < pEnd)
		{
			ColUtils::readLine(line, pStart, pEnd);
			PString userId(line.ltrim(), line.rtrim());
			if(userId.length() == 0)
			{
				// skip empty line
				continue;
			}
			accumFixUsers.push_back(userId);
		}

		PLog("Added %u users.", (UINT32)accumFixUsers.size());
		return true;
	}
	catch (const PError& er)
	{
		PString errBuf;
		errBuf.assign("Could not find file '").append(accumFixFileName).append("'. Reason - ").append(er.why());
		PLog("%s", errBuf.c_str());
		return false;
	}
}

bool AccumulatorDbManager::deleteUserDailyAccumData(const char* userId)
{
	PLog("deleteUserDailyAccumData: uid=%s", userId);
	try
	{
		accumType = ACCUM_DAILY;
		setAutoCommit(false);

		// 1. delete accumstatsaux for user
		deleteAccumDayAuxStmt->exec(userId, accumFixStartDate);

		// 2. update accumstatsaux endDate for user
		SrvDate fixEndDate = accumFixStartDate;
		ColUtils::addDays(fixEndDate, -1);
		fixAccumDayAuxEndDateStmt->exec(userId, accumFixStartDate, fixEndDate);

		// 3. delete accumdata for user 
		deleteAccumDayDataStmt->exec(userId, accumFixStartDate);

		commit();
		setAutoCommit(true);

		PLog("deleted accum daily data");

		return true;
	}
	catch (PSqlError const& err)
	{
		rollback();

		PLog("Error: %d-%s", err.code(), err.why());
		return false;
	}
}

bool AccumulatorDbManager::deleteUserWeeklyAccumData(const char* userId)
{
	PLog("deleteUserWeeklyAccumData: uid=%s", userId);
	try
	{
		accumType = ACCUM_WEEKLY;
		setAutoCommit(false);

		// 1. delete accumstatsaux for user
		deleteAccumWeekAuxStmt->exec(userId, accumFixStartDate);

		// 2. update accumstatsaux endDate for user
		SrvDate fixEndDate = accumFixStartDate;
		ColUtils::addDays(fixEndDate, -DAYS_IN_WEEK);
		fixAccumWeekAuxEndDateStmt->exec(userId, accumFixStartDate, fixEndDate);

		// 3. delete accumdata for user 
		deleteAccumWeekDataStmt->exec(userId, accumFixStartDate);

		commit();
		setAutoCommit(true);

		PLog("deleted accum weekly data");

		return true;
	}
	catch (PSqlError const& err)
	{
		rollback();

		PLog("Error: %d-%s", err.code(), err.why());
		return false;
	}
}

bool AccumulatorDbManager::addUserDailyAccumData(const char* userId)
{
	PLog("addUserDailyAccumData: uid=%s", userId);

	try
	{
		accumType = ACCUM_DAILY;

		AccumStatsKey statsKey;
		AccumAuxDateRange lastDateRange;
		AccumStatsData statsData;

		SrvDate currDate = accumFixStartDate;
		while (currDate.encode() <= accumFixDayEndDate.encode())
		{
			PString dummy;
			PLog("processing date=%s", srvDateToString(currDate, dummy));

			accumUserDayStmt->init(userId, currDate);

			setAutoCommit(false);

			accumUserDayStmt->execute();
			while (accumUserDayStmt->fetch())
			{
				UINT32 overflowCountDummy = 0;
				accumUserDayStmt->getAccumData(statsKey, lastDateRange, statsData, overflowCountDummy);

				AccumAuxKey auxKey(statsKey.userid, statsKey.currency, statsKey.ringTournGame);
				AccumAuxDateRange currDateRange;
				currDateRange.endDate = currDate;

				PString accumAuxStr, accumStatsStr;

				if (isContinuousDateRange(lastDateRange.endDate, currDate))
				{
					// update existing aux data
					currDateRange.startDate = lastDateRange.startDate;
					updateAccumDayAuxEndDateStmt->exec(auxKey.userid, currDateRange.startDate, auxKey.currency, auxKey.ringTournGame, currDateRange.endDate);
				}
				else
				{
					// insert new aux data
					currDateRange.startDate = currDate;
					insertAccumDayAuxStmt->exec(auxKey.userid, currDateRange.startDate, auxKey.currency, auxKey.ringTournGame, currDateRange.endDate);
				}

				insertAccumDayStatsStmt->exec(statsKey, statsData);
			}

			accumUserDayStmt->closeCursor();

			commit();
			setAutoCommit(true);

			ColUtils::addDays(currDate, 1);
		}

		return true;
	}
	catch (PSqlError const& err)
	{
		rollback();

		PLog("Error: %d-%s", err.code(), err.why());
		return false;
	}
}

bool AccumulatorDbManager::addUserWeeklyAccumData(const char* userId)
{
	PLog("addUserWeeklyAccumData: uid=%s", userId);

	try
	{
		accumType = ACCUM_WEEKLY;

		AccumStatsKey statsKey;
		AccumAuxDateRange lastDateRange;
		AccumStatsData statsData;

		SrvDate currDate = accumFixStartDate;
		while (currDate.encode() <= accumFixWeekEndDate.encode())
		{
			PString dummy;
			PLog("processing date=%s", srvDateToString(currDate, dummy));

			accumUserWeekStmt->init(userId, currDate);

			setAutoCommit(false);

			accumUserWeekStmt->execute();
			while (accumUserWeekStmt->fetch())
			{
				UINT32 overflowCountDummy = 0;
				accumUserWeekStmt->getAccumData(statsKey, lastDateRange, statsData, overflowCountDummy);

				AccumAuxKey auxKey(statsKey.userid, statsKey.currency, statsKey.ringTournGame);
				AccumAuxDateRange currDateRange;
				currDateRange.endDate = currDate;

				PString accumAuxStr, accumStatsStr;

				if (isContinuousDateRange(lastDateRange.endDate, currDate))
				{
					// update existing aux data
					currDateRange.startDate = lastDateRange.startDate;
					updateAccumWeekAuxEndDateStmt->exec(auxKey.userid, currDateRange.startDate, auxKey.currency, auxKey.ringTournGame, currDateRange.endDate);
				}
				else
				{
					// insert new aux data
					currDateRange.startDate = currDate;
					insertAccumWeekAuxStmt->exec(auxKey.userid, currDateRange.startDate, auxKey.currency, auxKey.ringTournGame, currDateRange.endDate);
				}

				insertAccumWeekStatsStmt->exec(statsKey, statsData);
			}

			accumUserWeekStmt->closeCursor();

			commit();
			setAutoCommit(true);

			ColUtils::addDays(currDate, DAYS_IN_WEEK);
		}

		return true;
	}
	catch (PSqlError const& err)
	{
		rollback();

		PLog("Error: %d-%s", err.code(), err.why());
		return false;
	}
}