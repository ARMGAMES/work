////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "AggregatorDbManager.h"
#include "AggregatorTemplates.h"
#include "HandStats.h"

//lint -esym(1793, PString::append*)

////////////////////////////////////////////////////////////////////////////////

static void writeToFile(PIOFile& file, const PString& str)
{
	file.writeFile(reinterpret_cast<const BYTE*>(str.c_str()), str.length()); //lint !e926
}

////////////////////////////////////////////////////////////////////////////////

static UINT32 aggregateSessionIntervals(TimeIntervalVec& intervals)
{
	sort(intervals.begin(), intervals.end(), TimeIntervalCmpLess());

	TimeIntervalVec aggIntervals;

	for (UINT32 i = 0; i < intervals.size(); ++i)
	{
		const TimeInterval& rawInterval = intervals[i];

		if (aggIntervals.empty())
		{
			aggIntervals.push_back(rawInterval);
			continue;
		}

		TimeInterval& aggInterval = *(aggIntervals.end() - 1);

		// If the aggregate interval and raw interval can be combined, combine them
		if (rawInterval.first <= aggInterval.second && rawInterval.second > aggInterval.second)
		{
			aggInterval.second = rawInterval.second;
		}

		// If the raw interval comes after the aggregated interval, add a new aggregated interval
		else if (rawInterval.first > aggInterval.second)
		{
			aggIntervals.push_back(rawInterval);
		}
	}

	UINT32 numSecondsAtTable = 0;
	for (UINT32 i = 0; i < aggIntervals.size(); ++i)
	{
		const TimeInterval& aggInterval = aggIntervals[i];

		time_t numSeconds = 1 + aggInterval.second - aggInterval.first;
		numSecondsAtTable += static_cast<UINT32>(numSeconds);
	}

	return numSecondsAtTable;
}

////////////////////////////////////////////////////////////////////////////////

static const char* formatDailyAggregate(const AggregateHandData& data, UINT32 sideTableJoinId, PString& str)
{

	ColUtils::dbOutQuote(data.userId, str).
		append(',').appendUint(data.gameType).
		append(',').appendUint(data.structure).
		append(',').appendUint(data.bigBlind).
		append(',').appendUint(data.smallBlind).
		append(',').append('"').append(data.currency).append('"').
		append(',').appendUint(data.maxPerTable).
		append(',').appendInt(data.buyIn).
		append(',').appendUint(data.totalHands).
		append(',').appendInt(data.totalWin).
		append(',').appendUint(data.totalBlindsAntesPaid).
		append(',').appendUint(data.totalRakePaid).
		append(',').appendUint(data.numBigBlinds).
		append(',').appendUint(data.numSmallBlinds).
		append(',').appendUint(data.numBlindsPaidOutOfPosition).
		append(',').appendUint(data.numVpip).
		append(',').appendUint(data.numVpipFromSmallBlind).
		append(',').appendUint(data.numVpipFromBigBlind).
		append(',').appendUint(data.numSawShowdown).
		append(',').appendUint(data.numWonShowdown).
		append(',').appendUint(data.numWonAfterFlop).
		append(',').appendUint(data.numLimp).
		append(',').appendUint(data.numLimpRaise).
		append(',').appendUint(data.numLimpCall).
		append(',').appendUint(data.numLimpFold).
		append(',').appendUint(data.numAllinPreflop).
		append(',').appendUint(data.numColdCallOpportunities).
		append(',').appendUint(data.numColdCalls).
		append(',').appendUint(data.num3BetOpportunities).
		append(',').appendUint(data.num3Bets).
		append(',').appendUint(data.numFacing3Bet).
		append(',').appendUint(data.numFoldTo3Bet).
		append(',').appendUint(data.num4plusBet).
		append(',').appendUint(data.perRoundData[0].numSawRound).
		append(',').appendUint(data.perRoundData[1].numSawRound).
		append(',').appendUint(data.perRoundData[2].numSawRound).
		append(',').appendUint(data.perRoundData[3].numSawRound).
		append(',').appendUint(data.numFacing4plusBet).
		append(',').appendUint(data.numFoldTo4plusBet).
		append(',').appendUint(data.numTurnsPreflop).
		append(',').appendUint(data.perRoundData[0].numTurnsFacingBetOnRound).
		append(',').appendUint(data.perRoundData[0].numTurnsFacingNoBetOnRound).
		append(',').appendUint(data.perRoundData[1].numTurnsFacingBetOnRound).
		append(',').appendUint(data.perRoundData[1].numTurnsFacingNoBetOnRound).
		append(',').appendUint(data.perRoundData[2].numTurnsFacingBetOnRound).
		append(',').appendUint(data.perRoundData[2].numTurnsFacingNoBetOnRound).
		append(',').appendUint(data.perRoundData[3].numTurnsFacingBetOnRound).
		append(',').appendUint(data.perRoundData[3].numTurnsFacingNoBetOnRound).
		append(',').appendUint(data.perRoundData[0].numContBetOppsOnRound).
		append(',').appendUint(data.perRoundData[1].numContBetOppsOnRound).
		append(',').appendUint(data.perRoundData[2].numContBetOppsOnRound).
		append(',').appendUint(data.perRoundData[3].numContBetOppsOnRound).
		append(',').appendUint(data.perRoundData[0].numContBetOnRound).
		append(',').appendUint(data.perRoundData[1].numContBetOnRound).
		append(',').appendUint(data.perRoundData[2].numContBetOnRound).
		append(',').appendUint(data.perRoundData[3].numContBetOnRound).
		append(',').appendUint(data.numBetPreflop).
		append(',').appendUint(data.perRoundData[0].numBetOnRound).
		append(',').appendUint(data.perRoundData[1].numBetOnRound).
		append(',').appendUint(data.perRoundData[2].numBetOnRound).
		append(',').appendUint(data.perRoundData[3].numBetOnRound).
		append(',').appendUint(data.perRoundData[0].numCheckOnRound).
		append(',').appendUint(data.perRoundData[1].numCheckOnRound).
		append(',').appendUint(data.perRoundData[2].numCheckOnRound).
		append(',').appendUint(data.perRoundData[3].numCheckOnRound).
		append(',').appendUint(data.numFoldPreflop).
		append(',').appendUint(data.perRoundData[0].numFoldOnRound).
		append(',').appendUint(data.perRoundData[1].numFoldOnRound).
		append(',').appendUint(data.perRoundData[2].numFoldOnRound).
		append(',').appendUint(data.perRoundData[3].numFoldOnRound).
		append(',').appendUint(data.numCallsPreflop).
		append(',').appendUint(data.perRoundData[0].numCallsOnRound).
		append(',').appendUint(data.perRoundData[1].numCallsOnRound).
		append(',').appendUint(data.perRoundData[2].numCallsOnRound).
		append(',').appendUint(data.perRoundData[3].numCallsOnRound).
		append(',').appendUint(data.numRaisesPreflop).
		append(',').appendUint(data.perRoundData[0].numRaisesOnRound).
		append(',').appendUint(data.perRoundData[1].numRaisesOnRound).
		append(',').appendUint(data.perRoundData[2].numRaisesOnRound).
		append(',').appendUint(data.perRoundData[3].numRaisesOnRound).
		append(',').appendUint(data.perRoundData[0].numCheckCallOnRound).
		append(',').appendUint(data.perRoundData[1].numCheckCallOnRound).
		append(',').appendUint(data.perRoundData[2].numCheckCallOnRound).
		append(',').appendUint(data.perRoundData[3].numCheckCallOnRound).
		append(',').appendUint(data.perRoundData[0].numCheckRaiseOnRound).
		append(',').appendUint(data.perRoundData[1].numCheckRaiseOnRound).
		append(',').appendUint(data.perRoundData[2].numCheckRaiseOnRound).
		append(',').appendUint(data.perRoundData[3].numCheckRaiseOnRound).
		append(',').appendUint(data.perRoundData[0].numCheckFoldOnRound).
		append(',').appendUint(data.perRoundData[1].numCheckFoldOnRound).
		append(',').appendUint(data.perRoundData[2].numCheckFoldOnRound).
		append(',').appendUint(data.perRoundData[3].numCheckFoldOnRound).
		append(',').appendUint(data.numOpenFoldPowerfulHand).
		append(',').appendUint(data.numFoldToExtremeOdds).
		append(',').appendUint(data.numReraiseWithGarbage).
		append(',').appendUint(data.numCheckBehindOnRiver).
		append(',').appendUint(data.numFoldWhenCouldCheck).
		append(',').appendUint(data.numAttemptStealBlinds).
		append(',').appendUint(data.numFoldToStealFromSBOpps).
		append(',').appendUint(data.numFoldToStealFromSB).
		append(',').appendUint(data.numFoldToStealFromBBOpps).
		append(',').appendUint(data.numFoldToStealFromBB).
		append(',').appendUint(data.perRoundData[0].numCheckRaiseOppsOnRound).
		append(',').appendUint(data.perRoundData[1].numCheckRaiseOppsOnRound).
		append(',').appendUint(data.perRoundData[2].numCheckRaiseOppsOnRound).
		append(',').appendUint(data.perRoundData[3].numCheckRaiseOppsOnRound).
		append(',').appendUint(data.numAttemptStealBlindsOpps).
		append(',').appendUint(data.num4BetOpportunities).
		append(',').appendUint(data.numHandsRaisedPreflop).
		append(',').appendUint(data.numCallBetsOnlyPreflop).
		append(',').appendUint(data.perRoundData[0].numCallBetsOnlyOnRound).
		append(',').appendUint(data.perRoundData[1].numCallBetsOnlyOnRound).
		append(',').appendUint(data.perRoundData[2].numCallBetsOnlyOnRound).
		append(',').appendUint(data.perRoundData[3].numCallBetsOnlyOnRound).
		append(',').appendUint(data.perRoundData[0].numTurnsFacingBetOnlyOnRound).
		append(',').appendUint(data.perRoundData[1].numTurnsFacingBetOnlyOnRound).
		append(',').appendUint(data.perRoundData[2].numTurnsFacingBetOnlyOnRound).
		append(',').appendUint(data.perRoundData[3].numTurnsFacingBetOnlyOnRound).
		append(',').appendUint(data.numFoldToBetOnlyPreflop).
		append(',').appendUint(data.perRoundData[0].numFoldToBetOnlyOnRound).
		append(',').appendUint(data.perRoundData[1].numFoldToBetOnlyOnRound).
		append(',').appendUint(data.perRoundData[2].numFoldToBetOnlyOnRound).
		append(',').appendUint(data.perRoundData[3].numFoldToBetOnlyOnRound).
		append(',').appendUint(data.tournSchedulingType).
		append(',').appendUint(data.tournGameType).
		append(',').appendUint(data.totalPlayersInHands).
		append(',').appendUint64(data.flags).
		append(',').appendUint(data.totalBet).
		append(',').appendUint(data.numFacing3BetHEM).
		append(',').appendUint(data.numFoldTo3BetHEM).
		append(',').appendUint(data.numFacing4PlusBetHEM).
		append(',').appendUint(data.numFoldTo4PlusBetHEM).
		append(',').appendUint(data.perRoundData[0].numContBetOppsVs2InPosOnRound).
		append(',').appendUint(data.perRoundData[1].numContBetOppsVs2InPosOnRound).
		append(',').appendUint(data.perRoundData[2].numContBetOppsVs2InPosOnRound).
		append(',').appendUint(data.perRoundData[3].numContBetOppsVs2InPosOnRound).
		append(',').appendUint(data.perRoundData[0].numContBetVs2InPosOnRound).
		append(',').appendUint(data.perRoundData[1].numContBetVs2InPosOnRound).
		append(',').appendUint(data.perRoundData[2].numContBetVs2InPosOnRound).
		append(',').appendUint(data.perRoundData[3].numContBetVs2InPosOnRound).
		append(',').appendUint(data.hostId).
		append(',').appendUint(data.tournActualEntries).
		append(',').appendUint(data.tournMaxEntry).
		append(',').appendUint(sideTableJoinId).
		append("\r\n");

	return str;
}

////////////////////////////////////////////////////////////////////////////////

AggregatorDbManager::~AggregatorDbManager()
{
	try
	{
		delete pWriteTournDataStmt;
	}
	catch (...) {}
}

////////////////////////////////////////////////////////////////////////////////

void AggregatorDbManager::checkCommitThreshold()
{
	if (commitThreshold)
	{
		if (++pendingCommits >= commitThreshold)
		{
			doCommit();
		}
	}
}


void AggregatorDbManager::checkCommitTimeout()
{
	if (pendingCommits && commitTimeout)
	{
		if (time(0) - lastCommit >= commitTimeout)
		{
			doCommit();
		}
	}
}


void AggregatorDbManager::doCommit()
{
	commit();
	pendingCommits = 0;
	lastCommit = time(0);
}

////////////////////////////////////////////////////////////////////////////////

void AggregatorDbManager::init(const char* fullFileName, const SrvDate& day)
{
	PIniFile iniFile(fullFileName);
	const PIniFile::Section* pSection = iniFile.getSection("SETTINGS");

	PString rawTablePrefix, outTablePrefix;

	if (pSection)
	{
		rawTablePrefix = pSection->getProperty("rawTablePrefix", "");
		outTablePrefix = pSection->getProperty("outTablePrefix", rawTablePrefix);

		directory = pSection->getProperty("directory", "");
		PSTR_SIZE_T len = directory.length();
		if (len && directory[len] != FILESEPARATORCHAR)
		{
			directory.append(FILESEPARATORCHAR);
		}

		maxRingSessionTableBreak = pSection->getIntProperty("maxRingSessionTableBreak", DEFAULT_SESSION_BREAK);
		maxRingSessionBreak = pSection->getIntProperty("maxRingSessionBreak", DEFAULT_SESSION_BREAK);
		maxTournSessionBreak = pSection->getIntProperty("maxTournSessionBreak", DEFAULT_SESSION_BREAK);
		maxBlitzSessionBreak = pSection->getIntProperty("maxBlitzSessionBreak", DEFAULT_SESSION_BREAK);
		maxTournEntrySize = pSection->getIntProperty("maxTournEntrySize", MAX_TOURN_ENTRY_SIZE_DEFAULT);
		consistencyThreshold = pSection->getIntProperty("consistencyThreshold", CONSISTENCY_THRESHOLD);
		writeToDb = pSection->getIntProperty("writeToDb", 0) != 0;
		commitThreshold = writeToDb ? pSection->getIntProperty("commitThreshold", COMMIT_THRESHOLD) : 0;
		commitTimeout = commitThreshold ? pSection->getIntProperty("commitTimeout", COMMIT_TIMEOUT) : 0;
		maxDeferredDays = pSection->getIntProperty("maxDeferredDays", 0);
		checkTodayTournRecords = pSection->getIntProperty("checkTodayTournRecords", DEFAULT_CHECK_TODAY_TOURN_RECS);
		useDeferredTable = maxDeferredDays ? pSection->getIntProperty("useDeferredTable", 0) != 0 : false;
		checkNextDayRecords = pSection->getIntProperty("checkNextDayRecords", 0) != 0;
		includedHostIdsMask = pSection->getIntProperty("includedHostIdsMask", eHost_Unknown);

	}
	else
	{
		PLog("Warning: section [SETTINGS] is missing in INI file %s.  Default values are used", fullFileName);
	}

	PLog("- maxRingSessionTableBreak: %u", maxRingSessionTableBreak);
	PLog("- maxRingSessionBreak: %u", maxRingSessionBreak);
	PLog("- maxTournSessionBreak: %u", maxTournSessionBreak);
	PLog("- maxBlitzSessionBreak: %u", maxBlitzSessionBreak);
	PLog("- maxTournEntrySize: %u", maxTournEntrySize);
	PLog("- consistencyThreshold: %u", consistencyThreshold);
	PLog("- maxDeferredDays: %u%s", maxDeferredDays, !maxDeferredDays ? " (not deferring)" : "");
	PLog("- checkTodayTournRecords: %s", checkTodayTournRecords ? "YES" : "NO");
	PLog("- checkNextDayRecords: %s", checkNextDayRecords ? "Yes" : "No");
	PLog("- includedHostIdsMask: %u", includedHostIdsMask);

	// Current code does NOT handle maxDeferredDays = 0.  
	// If hand and tournament are not finished in same day, hand should NOT be aggregated. 
	if (!maxDeferredDays)
	{
		PLog(LOG_TRACE_FAULTY_LOGIC": ERROR - AggregatorDbManager::init - maxDeferredDays should NOT be set to 0.");
		return;
	}

	PLog("- Output directory set to: %s", directory.c_str());

	handStatsRawTableName.assign(rawTablePrefix).append("handStatsRaw");
	PLog("- Raw hands table used: %s", handStatsRawTableName.c_str());

	completedTournsTableName.assign(rawTablePrefix).append("CompletedTourns");
	PLog("- Completed tournaments table used: %s", completedTournsTableName.c_str());

	if (useDeferredTable)
	{
		deferredHandsTableName.assign(outTablePrefix).append("HandStatsDefer");
		PLog("- Deferred hands read from%s table: %s",
			 writeToDb ? " and written to" : "", deferredHandsTableName.c_str());
	}
	else if (maxDeferredDays)
	{
		PLog("- Deferred hands read from previous partition%s", maxDeferredDays > 1 ? "s" :"");
	}

	PLog("- Writing to DB: %s", writeToDb ? "yes" : "no");
	if (writeToDb)
	{
		tournDataTableName.assign(outTablePrefix).append("TournDataInt");
		PLog("- Tourn data written to table: %s", tournDataTableName.c_str());

		PLog("- commitThreshold: %u%s", commitThreshold, commitThreshold ? "" : " (auto-commit)");
		if (commitThreshold)
		{
			PLog("- commitTimeout: %u seconds", commitTimeout);
		}
	}

	if (!DatabaseManagerCommon::openDataStorage(fullFileName, "MAINSTORAGE"))
	{
		throw PError("Cannot open data storage");
	}

	PString exec;
	DbmStatement stmt(*this);

	if (*schemaStr)
	{
		PLog("Setting current schema to '%s'", schemaStr.c_str());
		exec.assign("SET CURRENT SCHEMA ").append(schemaStr);
		stmt.execDirect(exec);
	}

	// TODO: Add MAINTJOBSETUP validation

	if (checkNextDayRecords) // PYR-27866
	{
		PLog("Validating existence of next-day records");

		SrvDate nextDate = day;
		ColUtils::addDays(nextDate, 1);

		PString buf;
		srvDateToString(nextDate, buf);
		exec.assign("SELECT 1 FROM ").append(handStatsRawTableName)
			.append(" WHERE Finished BETWEEN '").append(buf).append("' AND '")
			.append(buf).append("-23.59.59.999999' FETCH FIRST ROW ONLY WITH UR");

		stmt.execDirect(exec);
		if (!stmt.fetch())
		{
			PString errBuf;
			errBuf.assign(handStatsRawTableName).append(" does not contain records from ").append(buf);
			throw PSqlError(NO_SUCH_RECORD, errBuf);
		}
		stmt.closeCursor();
	}

	if (writeToDb && checkTodayTournRecords)
	{
		PLog("Validating absence of output tables records");

		PString buf;
		srvDateToString(day, buf);

		exec.assign("SELECT 1 FROM ").append(tournDataTableName)
			.append(" WHERE StartOfPeriod = '").append(buf)
			.append("' FETCH FIRST ROW ONLY WITH UR");

		stmt.execDirect(exec);
		if (stmt.fetch())
		{
			PString errBuf;
			errBuf.assign(tournDataTableName).append(" already contains records for ").append(buf);
			throw PSqlError(RECORD_ALREADY_EXISTS, errBuf);
		}
		stmt.closeCursor();

		if (deferredHandsTableName.length())
		{
			exec.assign("SELECT 1 FROM ").append(deferredHandsTableName)
				.append(" WHERE DATE(Finished) = '").append(buf)
				.append("' FETCH FIRST ROW ONLY WITH UR");

			stmt.execDirect(exec);
			if (stmt.fetch())
			{
				PString errBuf;
				errBuf.assign(deferredHandsTableName).append(" already contains records for ").append(buf);
				throw PSqlError(RECORD_ALREADY_EXISTS, errBuf);
			}
			stmt.closeCursor();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

void AggregatorDbManager::aggregateRawHands(const SrvDate& day)
{
	char date[9];
	::sprintf(date, "%04.4u%02.2u%02.2u", day._year, day._month, day._day);

	handStats.setup(day, maxDeferredDays, handStatsRawTableName, deferredHandsTableName, completedTournsTableName);
	pWriteTournDataStmt = new WriteTournDataStmt(*this, tournDataTableName);

	createOutputFiles(date);

	time_t timeStart = time(0);

	PLog("Fetching and sorting finished tourn IDs...");

	CompletedTourns completedTourns;
	if (maxDeferredDays)
	{
		ReadCompletedTournsStmt readCompletedTournsStmt(*this, day, completedTournsTableName);
		readCompletedTournsStmt.execGet(completedTourns);
	
		PLog("Finished tourn count: %u", (UINT32)completedTourns.size());
	}

	PLog("Starting main query...");
	handStats.read();

	PLog("Aggregating results...");

	if (commitThreshold)
	{
		setAutoCommit(false); // Combine commits for speed
	}

	// --- Main loop starts here ---

	AggregateHandData data;
	AggregateHandData intelliPokerData;
	AggregateHandData micromillionsData;
	AggregateHandData micromillions10Data;					// PYR-34683

	HandPartnersMap handPartners;
	TournPartnersMap tournPartners;
	TournSessionMap tournSessionMap;
	HandSessionMap handSessionMap;
	ConsistencyMap consistencyMap;

	TimeIntervalVec sessionIntervals; // Time intervals for all sessions for a player
	UINT32 sessionMaxInsts[COLOSSUS_AGGSESSIONGAMETYPE_MAX + 1] = { 0 }; // Maximum number of simultaneous sessions for a player

	UINT32 numRecords = 0;
	while(handStats.fetch())
	{
		UINT32 curHostId = handStats.getHostId();
		incRecordsPerHost(curHostId);

		// PYR-45403
		if (ignoreRecordByHost(curHostId))
		{
			continue;
		}

		// --- Writing the aggregated data on key changes	---
		// --- data contains *old* key values at this point	---

		bool isSessionTypeChanged = data.sessionType != handStats.getSessionType();
		bool isUserIdChanged = !data.userId.equals(handStats.getUserId());

		if (isSessionTypeChanged || isUserIdChanged)
		{
			writeAndResetSessionData(data.userId, data.sessionType,
									 tournSessionMap, handSessionMap, sessionIntervals, sessionMaxInsts);
		}

		if (isUserIdChanged && numRecords > 0)
		{
			UINT32 secondsPlayed = aggregateSessionIntervals(sessionIntervals);
			writeAggregateSession(date, data.userId, secondsPlayed, data.hostId, sessionMaxInsts);
			++writtenAggSessionsRecords;

			sessionIntervals.clear();
			fill_n(sessionMaxInsts, SZARR(sessionMaxInsts), 0);
		}

		if (isUserIdChanged)
		{
			// Write caches
			writeAndResetTournPartnerData(day, date, data.userId, tournPartners);
			writeAndResetConsistencyData(date, data.userId, consistencyMap);
		}

		if (handStats.isGameOrPlayerChanged(intelliPokerData))					// PYR-26226
		{
			writeAndResetIntellipokerData(date, intelliPokerData, intellipokerFile, writtenIntellipokerRecords);

			handStats.getKey(intelliPokerData);
			intelliPokerData.bigBlind = intelliPokerData.smallBlind = 0;
		}

		if (handStats.isGameOrPlayerChanged(micromillionsData))					// PYR-32185
		{
			writeAndResetIntellipokerData(date, micromillionsData, micromillionsFile, writtenMicromillionsRecords);

			handStats.getKey(micromillionsData);
			micromillionsData.bigBlind = micromillionsData.smallBlind = 0;
		}

		if (handStats.isGameOrPlayerChanged(micromillions10Data))				// PYR-34683
		{
			writeAndResetIntellipokerData(date, micromillions10Data, micromillions10File, writtenMicromillions10Records);

			handStats.getKey(micromillions10Data);
			micromillions10Data.bigBlind = micromillions10Data.smallBlind = 0;
		}

		if (handStats.isKeyChanged(data))
		{
			writeAndResetDailyData(date, data, handPartners);
			handStats.getKey(data);
		}

		// --- Aggregating values from the current fetch	---
		// --- data contains *new* key values at this point	---

		SrvTime finishedTime;
		handStats.getFinished(finishedTime);

		UINT32 tournId = handStats.getTournId();
		UINT64 sessionId = handStats.getSessionId();

		bool isDeferredHand = maxDeferredDays && !day.equals(finishedTime);
		if (!isDeferredHand) // We do not defer session processing
		{
			if (data.sessionType == COLOSSUS_SESSIONTYPE_TOURN)
			{
				TournsSessionKey key(maxTournSessionBreak, 0);
				handStats.getTournsSessionKey(key);
				tournSessionMap.addHand(key, sessionId, finishedTime);
			}

			else // COLOSSUS_SESSIONTYPE_RING or COLOSSUS_SESSIONTYPE_BLITZ
			{
				UINT32 maxSessionBreak = (data.sessionType == COLOSSUS_SESSIONTYPE_RING) ? maxRingSessionBreak : maxBlitzSessionBreak;
				UINT32 maxTableSessionBreak = (data.sessionType == COLOSSUS_SESSIONTYPE_RING) ? maxRingSessionTableBreak : 0;
				HandsSessionKey key(maxSessionBreak, maxTableSessionBreak);
				handStats.getHandsSessionKey(key);
				handSessionMap.addHand(key, sessionId, finishedTime);
			}
		}

		else // Hand was deferred from previous date
		{
			++readDeferredHands;
		}

		bool dontDeferHand  = !maxDeferredDays || !tournId || completedTourns.find(tournId);
		if (dontDeferHand) // PYR-25820
		{
			handStats.aggregateStats(data);

			if (handStats.getAggregationFlags() & eColAggFlags_Intellipoker)
			{
				handStats.aggregateStats(intelliPokerData);
			}

			// PYR-32185
			if (handStats.getAggregationFlags() & eColAggFlags_Micromillions)
			{
				handStats.aggregateStats(micromillionsData);
			}

			// PYR-34683
			if (handStats.getAggregationFlags() & eColAggFlags_Micromillions10)
			{
				handStats.aggregateStats(micromillions10Data);
			}
		}

		else if (useDeferredTable)
		{
			writeDeferredHand();
		}

		bool isConsistencyHand = !tournId && ((data.gameType & 0xFF) == 1); // PYR-24676 - Only ring Hold'em games
		if (isConsistencyHand)
		{
			ConsistencyKey key;
			handStats.getConsistencyKey(key);

			ConsistencyData& consistencyData = consistencyMap[key];
			handStats.aggregateConsistency(consistencyData);
			consistencyMap.addHand();
		}

		if (!tournId) // Ring game hand
		{
			data.totalPlayersInHands += handStats.aggregateHandPartnerData(handPartners);
		}

		else if (data.tournSchedulingType == COLOSSUS_TOURNSCHEDULINGTYPE_SNG &&	// S&G tournament
				 handStats.getTournEntries() <= maxTournEntrySize &&				// of up to a given size
				 dontDeferHand)														// already finished (PYR-25820)
		{
			handStats.aggregateTournPartnerData(tournPartners[tournId]);
		}

		if (numRecords && (numRecords % 10000000 == 0))
		{
			PLog("Processed %u millions rows", numRecords / 1000000);
		}

		checkCommitTimeout();

		++numRecords;
	}

	handStats.finish();

	if (data.isValid())
	{
		// --- Writing last batches of aggregated data ---
		writeAndResetTournPartnerData(day, date, data.userId, tournPartners);

		if (commitThreshold)
		{
			commit(); // Finished Writing to DB
			setAutoCommit(true);
		}

		writeAndResetSessionData(data.userId, data.sessionType,
			tournSessionMap, handSessionMap, sessionIntervals, sessionMaxInsts);

		UINT32 secondsPlayed = aggregateSessionIntervals(sessionIntervals);
		writeAggregateSession(date, data.userId, secondsPlayed, data.hostId, sessionMaxInsts);
		++writtenAggSessionsRecords;

		writeAndResetConsistencyData(date, data.userId, consistencyMap);
		writeAndResetIntellipokerData(date, intelliPokerData, intellipokerFile, writtenIntellipokerRecords);
		writeAndResetIntellipokerData(date, micromillionsData, micromillionsFile, writtenMicromillionsRecords);
		writeAndResetIntellipokerData(date, micromillions10Data, micromillions10File, writtenMicromillions10Records);
		writeAndResetDailyData(date, data, handPartners); // Must be last: data key is cleared
	}

	UINT32 secRead = static_cast<UINT32>(time(0) - timeStart);
	writeStats(date, numRecords, secRead);

	// May throw
	closeOutputFiles();
}

////////////////////////////////////////////////////////////////////////////////

void AggregatorDbManager::createOutputFiles(const char* date)
{
	PLog("Opening files for %s in %s", date, directory.length() ? directory.c_str() : "current directory");

	PString file;
	file.assign(directory).append("stats.txt.").append(date);
	statsFile.openFile(file, PLIB_FILE_CREATE_OR_OPEN | PLIB_FILE_FLAG_TRUNCATE);

	file.assign(directory).append("stats2.txt.").append(date);
	stats2File.openFile(file, PLIB_FILE_CREATE_OR_OPEN | PLIB_FILE_FLAG_TRUNCATE);

	file.assign(directory).append("profits.txt.").append(date);
	profitsFile.openFile(file, PLIB_FILE_CREATE_OR_OPEN | PLIB_FILE_FLAG_TRUNCATE);

	file.assign(directory).append("sessions.txt.").append(date);
	sessionFile.openFile(file, PLIB_FILE_CREATE_OR_OPEN | PLIB_FILE_FLAG_TRUNCATE);

	file.assign(directory).append("aggsessions.txt.").append(date);
	aggSessionFile.openFile(file, PLIB_FILE_CREATE_OR_OPEN | PLIB_FILE_FLAG_TRUNCATE);

	file.assign(directory).append("counts.txt.").append(date);
	countsFile.openFile(file, PLIB_FILE_CREATE_OR_OPEN | PLIB_FILE_FLAG_TRUNCATE);

	file.assign(directory).append("consistency.txt.").append(date);
	consistencyFile.openFile(file, PLIB_FILE_CREATE_OR_OPEN | PLIB_FILE_FLAG_TRUNCATE);

	file.assign(directory).append("intellipoker.txt.").append(date);
	intellipokerFile.openFile(file, PLIB_FILE_CREATE_OR_OPEN | PLIB_FILE_FLAG_TRUNCATE);

	file.assign(directory).append("micromillions.txt.").append(date);
	micromillionsFile.openFile(file, PLIB_FILE_CREATE_OR_OPEN | PLIB_FILE_FLAG_TRUNCATE);

	file.assign(directory).append("ip_micromillions10.txt.").append(date);
	micromillions10File.openFile(file, PLIB_FILE_CREATE_OR_OPEN | PLIB_FILE_FLAG_TRUNCATE);

	file.assign(directory).append("counts.txt.").append(date);
	countsFile.openFile(file, PLIB_FILE_CREATE_OR_OPEN | PLIB_FILE_FLAG_TRUNCATE);

	if (!writeToDb)
	{
		file.assign(directory).append("tournData.txt.").append(date);
		tournDataFile.openFile(file, PLIB_FILE_CREATE_OR_OPEN | PLIB_FILE_FLAG_TRUNCATE);

		if (useDeferredTable)
		{
			file.assign(directory).append("deferredHands.txt.").append(date);
			deferredHandsFile.openFile(file, PLIB_FILE_CREATE_OR_OPEN | PLIB_FILE_FLAG_TRUNCATE);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

// PYR-32185
// May throw
void AggregatorDbManager::closeOutputFiles()
{
	PLog("Closing files...");

	statsFile.closeFile();
	stats2File.closeFile();
	profitsFile.closeFile();
	sessionFile.closeFile();
	aggSessionFile.closeFile();
	countsFile.closeFile();
	consistencyFile.closeFile();
	intellipokerFile.closeFile();
	micromillionsFile.closeFile();
	micromillions10File.closeFile();
	countsFile.closeFile();

	if (!writeToDb)
	{
		tournDataFile.closeFile();

		if (useDeferredTable)
		{
			deferredHandsFile.closeFile();
		}
	}

	PLog("Closed files");
}

////////////////////////////////////////////////////////////////////////////////
// PYR-24927

void AggregatorDbManager::writeStats(const char* date, UINT32 num, UINT32 secRead)
{
	PString str;
	str.
		assign(date).append(",\"HANDSTATSRAW\",").appendUint(num).append(",0,0\n").
		append(date).append(",\"HANDSTATSDAY\",").appendUint(writtenHandsRecords).append(',').appendUint(secRead).append(",0\n").
		append(date).append(",\"HANDPARTNERSDAY\",").appendUint(writtenProfitsRecords).append(',').appendUint(secRead).append(",0\n").
		append(date).append(",\"HANDCONSISTENCYDAY\",").appendUint(writtenConsistencyRecords).append(",0,0\n").
		append(date).append(",\"SESSIONDAY\",").appendUint(writtenHandSessionsRecords + writtenTournSessionsRecords).append(',').appendUint(secRead).append(",0\n").
		append(date).append(",\"SESSIONDAY_HANDS\",").appendUint(writtenHandSessionsRecords).append(",0,0\n").
		append(date).append(",\"SESSIONDAY_TOURNS\",").appendUint(writtenTournSessionsRecords).append(",0,0\n").
		append(date).append(",\"SESSIONAGGDAY\",").appendUint(writtenAggSessionsRecords).append(",0,0\n").
		append(date).append(",\"TOURNDATAINT\",").appendUint(writtenTournPartnersRecords).append(',').appendUint(secRead).append(",0\n").
		append(date).append(",\"INTELLIPOKER_FILE\",").appendUint(writtenIntellipokerRecords).append(',').appendUint(secRead).append(",0\n").
		append(date).append(",\"MICROMILLIONS_FILE\",").appendUint(writtenMicromillionsRecords).append(',').appendUint(secRead).append(",0\n").
		append(date).append(",\"MICROMILLIONS10_FILE\",").appendUint(writtenMicromillions10Records).append(',').appendUint(secRead).append(",0\n")
		;

	if (maxDeferredDays)
	{
		str.append(date).append(",\"DEFERREDHANDS_READ\",").appendUint(readDeferredHands).append(',').appendUint(secRead).append(",0\n");
		if (useDeferredTable)
		{
			str.append(date).append(",\"HANDSTATSDEFER\",").appendUint(writtenDeferredHands).append(',').appendUint(secRead).append(",0\n");
		}
	}

	for (NumRecordsPerHost::const_iterator it = numRecsPerHost.begin(); it != numRecsPerHost.end(); ++it)
	{
		str.append(date).append(",\"HANDSTATSRAW.").appendUint(it->first).append("\",").appendUint(it->second).append(",0,0\n");

		PLog("Read %u raw hands records for hostId=%u", it->second, it->first);
	}

	writeToFile(countsFile, str);

	PLog("Aggregated %u rows", num);
	PLog("Records written:\n\tHands: %u\n\tProfits: %u\n\tTournPartners: %u\n\tHandSessions: %u\n"
		"\tTournSessions: %u\n\tConsistency: %u\n\tIntellipoker: %u\n\tMicromillions: %u\n\tMicromillions10: %u\n\tAggSessions: %u",
		writtenHandsRecords, writtenProfitsRecords, writtenTournPartnersRecords, writtenHandSessionsRecords,
		writtenTournSessionsRecords, writtenConsistencyRecords, writtenIntellipokerRecords, writtenMicromillionsRecords, writtenMicromillions10Records, writtenAggSessionsRecords);

	if (maxDeferredDays)
	{
		PLog("Deferred hands read from previous date: %u", readDeferredHands);
		if (useDeferredTable)
		{
			PLog("Deferred hands written for next date: %u", writtenDeferredHands);
		}
	}

	PLog("Maximum TournPartners count in memory was %u", maxTournPartnersCount);
	PLog("Maximum Sessions counts in memory were Hands:%u (%s) Tourns:%u (%s)",
		 maxHandSessionsCount, handsMaxSessionUserId.c_str(), maxTournSessionsCount, tournsMaxSessionUserId.c_str());
	PLog("Maximum Consistency count in memory was %u", maxConsistencyCount);
}

////////////////////////////////////////////////////////////////////////////////

void AggregatorDbManager::writeAndResetDailyData(const char* date,
												 AggregateHandData& data,
												 HandPartnersMap& handPartners)
{
	if (!data.isValid())
	{
		return;
	}

	// Writing to data file
	++sideTableJoinId;
	PString str;
	str.assign(date).append(',');
	writeToFile(statsFile, formatDailyAggregate(data, sideTableJoinId, str));

	for (const auto& entry : statsDescriptors)
	{
		int statId = entry.first;
		UINT32 statVal = data.getStatValByStatId(statId);
		if (!statVal)
		{
			continue;
		}

		PString strSideData(date);
		strSideData.append(',').appendUint(sideTableJoinId);
		strSideData.append(',').appendInt(statId);
		strSideData.append(',').appendUint(statVal);
		strSideData.append("\r\n");

		writeToFile(stats2File, strSideData);
	}

	++writtenHandsRecords;

	// Writing to profits file
	for (const auto& entry : handPartners)
	{
		const char* partnerId = entry.first;
		const HandPartnerData& profit = entry.second;

		str.assign("");
		ColUtils::dbOutQuote(data.userId, str).append(',');
		ColUtils::dbOutQuote(partnerId, str).
			append(',').append(date).
			append(',').appendUint(data.gameType).
			append(',').appendUint(data.structure).
			append(',').appendUint(data.bigBlind).
			append(',').appendUint(data.smallBlind).
			append(',').append('"').append(data.currency).append('"').
			append(',').appendUint(data.maxPerTable).
			append(',').appendInt(profit.totalProfitFromPartner).
			append(',').appendInt(profit.totalProfitWithPartner).
			append(',').appendUint(data.totalRakePaid).
			append(',').appendUint(profit.numHandsWithPartner).
			append(',').appendUint(profit.numHandsProfitFromPartner).
			append(',').appendUint(profit.allInConfrontations).
			append(',').appendUint(profit.showdownAllInConfrontations).
			append(',').appendUint(data.hostId);

		// PYR-74693
		str.append(',').appendUint64(data.flags);
		
		str.append("\r\n");

		writeToFile(profitsFile, str);
		++writtenProfitsRecords;
	}

	data.clear();
	handPartners.clear();
}

////////////////////////////////////////////////////////////////////////////////

// PYR-32185
void AggregatorDbManager::writeAndResetIntellipokerData(const char* date, AggregateHandData& data, PIOFile& outputFile, UINT32& recordCounter) const
{
	if (!data.isValid())
	{
		return;
	}

	PString str;
	str.assign(date).append(',');
	writeToFile(outputFile, formatDailyAggregate(data, sideTableJoinId, str));

	++recordCounter;
	data.clear();
}

////////////////////////////////////////////////////////////////////////////////

void AggregatorDbManager::writeAndResetTournPartnerData(const SrvDate& day, const char* date,
														const char* userId,
														TournPartnersMap& tournPartners)
{
	if (tournPartners.empty())
	{
		return;
	}

	UINT32 cnt = 0;
	for (TournPartnersMap::const_iterator itMap = tournPartners.begin(); itMap != tournPartners.end(); ++itMap)
	{
		UINT32 tournId = itMap->first;
		const TournPartnerDataMap& dataMap = itMap->second;

		for (TournPartnerDataMap::const_iterator it = dataMap.begin(); it != dataMap.end(); ++it)
		{
			const char* partnerId = it->first;
			const TournPartnerData& tournData = it->second;

			if (writeToDb)
			{
				try
				{
					pWriteTournDataStmt->exec(day, tournId, userId, partnerId, tournData);
					checkCommitThreshold();
				}
				catch (...) // Not handling the exception - after logging, application will abort
				{

					PString dateStr;
					PLog("Error Writing tourn data: count=%u tournId=%u userId='%s' partnerId='%s' day=%s", 
						 cnt, tournId, userId, partnerId, srvDateToString(day, dateStr));

					rollback();
					throw;
				}
			}

			else // write to file
			{
				PString str;
				str.assign(date).append(',').
					appendUint(tournId).append(',');
				ColUtils::dbOutQuote(userId, str).append(',');
				ColUtils::dbOutQuote(partnerId, str).append(',').
					appendUint(tournData.allInConfrontations).append(',').
					appendUint(tournData.showdownAllInConfrontations).
					append("\r\n");

				writeToFile(tournDataFile, str);
			}

			++cnt;
		}
	}

	maxTournPartnersCount = max(maxTournPartnersCount, cnt);
	writtenTournPartnersRecords += cnt;
	tournPartners.clear();
}

////////////////////////////////////////////////////////////////////////////////

void AggregatorDbManager::writeAndResetConsistencyData(const char* date, const char* userId,
													   ConsistencyMap& consistencyMap)
{
	if (consistencyMap.empty())
	{
		return;
	}

	maxConsistencyCount = max(maxConsistencyCount, consistencyMap.size());

	if (consistencyMap.numHands() >= consistencyThreshold)
	{
		for (ConsistencyMap::const_iterator it = consistencyMap.begin(); it != consistencyMap.end(); ++it)
		{
			const ConsistencyKey& key = it->first;
			const ConsistencyData& data = it->second;

			PString str;
			str.
				assign(date).
				append(',');
			ColUtils::dbOutQuote(userId, str).
				append(',').appendUint(key.gameType).
				append(',').appendUint(key.structure).
				append(',').appendUint(key.bigBlind).
				append(',').append('"').append(key.currency).append('"').
				append(',').appendUint(key.maxPerTable).
				append(',').appendUint(key.position).
				append(',').append('"').append(key.holeCards).append('"').
				append(',').appendUint(data.numHands).
				append(',').appendUint(data.numVpip).
				append(',').appendUint(data.numHandsRaisedPreflop).
				append(',').appendUint(data.numColdCallOpportunities).
				append(',').appendUint(data.numColdCalls).
				append("\r\n");

			writeToFile(consistencyFile, str);
			++writtenConsistencyRecords;
		}
	}

	consistencyMap.clear();
}

////////////////////////////////////////////////////////////////////////////////

void AggregatorDbManager::writeDeferredHand()
{
	if (writeToDb)
	{
		try
		{
			handStats.write();
			checkCommitThreshold();
		}

		catch (...) // Not handling the exception - application will abort
		{
			rollback();
			throw;
		}
	}

	else // write to file
	{
		PString str;
		writeToFile(deferredHandsFile, handStats.formatDeferredHand(str));
	}

	++writtenDeferredHands;
}

////////////////////////////////////////////////////////////////////////////////

void AggregatorDbManager::writeAndResetSessionData(const char* userId, UINT32 sessionType,
												   TournSessionMap& tournSessionMap, HandSessionMap& handSessionMap,
												   TimeIntervalVec& intervals, UINT32 maxInsts[])
{
	TimeIntervalVec aggIntervals[COLOSSUS_AGGSESSIONGAMETYPE_MAX + 1];

	if (sessionType == COLOSSUS_SESSIONTYPE_TOURN)
	{
		TournsSingleSessionMap tournSingleSessionMap;
		tournSessionMap.calcSessionData(tournSingleSessionMap, aggIntervals);
		tournSessionMap.clear();

		// Aggregate session operations
		maxInsts[COLOSSUS_AGGSESSIONGAMETYPE_SNG] = calcAggSessionMaxInstances(aggIntervals[COLOSSUS_AGGSESSIONGAMETYPE_SNG]);
		maxInsts[COLOSSUS_AGGSESSIONGAMETYPE_MTT] = calcAggSessionMaxInstances(aggIntervals[COLOSSUS_AGGSESSIONGAMETYPE_MTT]);

		intervals.insert(intervals.end(), 
						 aggIntervals[COLOSSUS_AGGSESSIONGAMETYPE_SNG].begin(), aggIntervals[COLOSSUS_AGGSESSIONGAMETYPE_SNG].end());
		intervals.insert(intervals.end(), 
						 aggIntervals[COLOSSUS_AGGSESSIONGAMETYPE_MTT].begin(), aggIntervals[COLOSSUS_AGGSESSIONGAMETYPE_MTT].end());

		// Single session operations
		UINT32 numSessionsWritten = writeSingleSessionData(tournSingleSessionMap);
		writtenTournSessionsRecords += numSessionsWritten;

		// Keep track of the user with the most sessions
		if (numSessionsWritten > maxTournSessionsCount)
		{
			maxTournSessionsCount = numSessionsWritten;
			tournsMaxSessionUserId = userId;
		}
	}

	else // COLOSSUS_SESSIONTYPE_RING or COLOSSUS_SESSIONTYPE_BLITZ
	{
		HandsSingleSessionMap handsSingleSessionMap;
		handSessionMap.calcSessionData(handsSingleSessionMap, aggIntervals);
		handSessionMap.clear();

		// Aggregate session operations
		AggregateSessionGameType aggSessionType = 
			(sessionType == COLOSSUS_SESSIONTYPE_RING) ? COLOSSUS_AGGSESSIONGAMETYPE_RING : COLOSSUS_AGGSESSIONGAMETYPE_ZOOM;
		maxInsts[aggSessionType] = calcAggSessionMaxInstances(aggIntervals[aggSessionType]);
		intervals.insert(intervals.end(), aggIntervals[aggSessionType].begin(), aggIntervals[aggSessionType].end());

		// Single session operations
		UINT32 numSessionsWritten = writeSingleSessionData(handsSingleSessionMap);
		writtenHandSessionsRecords += numSessionsWritten;

		// Keep track of the user with the most sessions
		if (numSessionsWritten > maxHandSessionsCount)
		{
			maxHandSessionsCount = numSessionsWritten;
			handsMaxSessionUserId = userId;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

// Checks the maximum number of concurrent session intervals.
UINT32 AggregatorDbManager::calcAggSessionMaxInstances(const TimeIntervalVec& intervals) const
{
	UINT32 maxInstances = 0;

	for (UINT32 i = 0; i < intervals.size(); ++i)
	{
		// First, get the start and end points of each interval.
		const TimeInterval& interval = intervals[i];
		time_t eventStart = interval.first;
		time_t eventEnd = interval.second;

		// Then, check how many other intervals those endpoints intersect.
		UINT32 numInstancesStart = 1;
		UINT32 numInstancesEnd = 1;
		for (UINT32 j = 0; j < intervals.size(); ++j)
		{
			if (i == j)
			{
				continue;
			}

			const TimeInterval& otherInterval = intervals[j];

			if (eventStart >= otherInterval.first && eventStart <= otherInterval.second)
			{
				++numInstancesStart;
			}

			if (eventEnd >= otherInterval.first && eventEnd <= otherInterval.second)
			{
				++numInstancesEnd;
			}
		}

		maxInstances = max(maxInstances, max(numInstancesStart, numInstancesEnd));
	}

	return maxInstances;
}

////////////////////////////////////////////////////////////////////////////////

void AggregatorDbManager::writeAggregateSession(const char* date, const char* userId, UINT32 secondsAtTable, UINT32 hostId,
												const UINT32 (&maxInsts)[COLOSSUS_AGGSESSIONGAMETYPE_MAX + 1])
{
	PString str;
	ColUtils::dbOutQuote(userId, str).
		append(',').append(date).
		append(',').appendUint(secondsAtTable).
		append(',').appendUint(maxInsts[COLOSSUS_AGGSESSIONGAMETYPE_RING]).
		append(',').appendUint(maxInsts[COLOSSUS_AGGSESSIONGAMETYPE_SNG]).
		append(',').appendUint(maxInsts[COLOSSUS_AGGSESSIONGAMETYPE_MTT]).
		append(',').appendUint(maxInsts[COLOSSUS_AGGSESSIONGAMETYPE_ZOOM]).
		append(',').appendUint(hostId).
		append("\r\n");

	writeToFile(aggSessionFile, str);
}

////////////////////////////////////////////////////////////////////////////////

template <typename KeyType>
UINT32 AggregatorDbManager::writeSingleSessionData(const map<KeyType, SingleSessionDataVec>& dataMap)
{
	UINT32 numSessionsWritten = 0;
	for (map<KeyType, SingleSessionDataVec>::const_iterator mapIt = dataMap.begin(); mapIt != dataMap.end(); ++mapIt)
	{
		const KeyType& key = mapIt->first;
		const SingleSessionDataVec& dataVec = mapIt->second;

		UINT32 num = dataVec.size();
		for (UINT32 i = 0; i < num; ++i)
		{
			const SingleSessionData& data = dataVec[i];

			PString str, sessionStart, sessionEnd;
			ColUtils::dbOutQuote(key.userId, str).
				append(',').append('"').append(srvTimeToString(data.sessionStartEndTimes.timeFrom, sessionStart, '-', '-', '.')).append(".000000\"").
				append(',').append('"').append(srvTimeToString(data.sessionStartEndTimes.timeTo, sessionEnd, '-', '-', '.')).append(".000000\"").
				append(',');

			key.toString(data.maxInstances, str);

			str.append("\r\n");

			writeToFile(sessionFile, str);
		}

		numSessionsWritten += num;
	}

	return numSessionsWritten;
}

////////////////////////////////////////////////////////////////////////////////

// PYR-45403
bool AggregatorDbManager::ignoreRecordByHost(UINT32 recordHostId) const
{
	// PYR-90648 - Host id is a simple enumeration, not a bitmask, so we convert it to a bitmask.
	PASSERT(recordHostId > 0);
	UINT32 hostShiftAmount = recordHostId - 1; // Bit shift for IOM = 0; bit shift for NJ = 1.
	UINT32 recordHostIdAsMask = 1 << hostShiftAmount;

	return (recordHostIdAsMask & includedHostIdsMask) == 0;
}

// PYR-45403
void AggregatorDbManager::incRecordsPerHost(UINT32 recordHostId)
{
	NumRecordsPerHost::iterator it = numRecsPerHost.find(recordHostId);
	if (it == numRecsPerHost.end())
	{
		it = numRecsPerHost.insert(make_pair(recordHostId, 0)).first;
	}

	it->second += 1;
}

////////////////////////////////////////////////////////////////////////////////