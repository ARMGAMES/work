////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "AggregatorDbManager.h"
#include "HandStats.h"
#include "initdatetime.h"

//lint -esym(1793, PString::append*) : Operations on non-const temporary

////////////////////////////////////////////////////////////////////////////////

const char* const HandStatsStmtBase::columnNames = 
/*   5 */	"UserId, GameType, Structure, BigBlind, SmallBlind, "
/*  10 */	"Currency, MaxPerTable, BuyIn, TotalWin, TotalBlindsAntesPaid, "
/*  15 */	"TotalRakePaid, NumBigBlinds, NumSmallBlinds, NumBlindsPaidOutOfPosition, NumVpip, "
/*  20 */	"NumVpipFromSmallBlind, NumVpipFromBigBlind, NumSawShowdown, NumWonShowdown, NumWonAfterFlop, "
/*  25 */	"NumLimp, NumLimpRaise, NumLimpCall, NumLimpFold, NumAllinPreflop, "
/*  30 */	"NumColdCallOpportunities, NumColdCalls, Num3BetOpportunities, Num3Bets, NumFacing3Bet, "
/*  35 */	"NumFoldTo3Bet, Num4plusBet, NumFacing4plusBet, NumFoldTo4plusBet, NumSawFlop, "
/*  40 */	"NumSawTurn, NumSawRiver, NumSaw7th, NumTurnsPreflop, NumTurnsFacingBetOnFlop, "
/*  45 */	"NumTurnsFacingNoBetOnFlop, NumTurnsFacingBetOnTurn, NumTurnsFacingNoBetOnTurn, NumTurnsFacingBetOnRiver, NumTurnsFacingNoBetOnRiver, "
/*  50 */	"NumTurnsFacingBetOn7th, NumTurnsFacingNoBetOn7th, NumLastAgressorPreflop, NumLastAgressorOnFlop, NumLastAgressorOnTurn, "
/*  55 */	"NumLastAgressorOnRiver, NumHandsRaisedPreflop, NumContBetOnFlop, NumContBetOnTurn, NumContBetOnRiver, "
/*  60 */	"NumContBetOn7th, NumBetPreflop, NumBetOnFlop, NumBetOnTurn, NumBetOnRiver, "
/*  65 */	"NumBetOn7th, NumCheckOnFlop, NumCheckOnTurn, NumCheckOnRiver, NumCheckOn7th, "
/*  70 */	"NumFoldPreflop, NumFoldOnFlop, numFoldOnTurn, NumFoldOnRiver, NumFoldOn7th, "
/*  75 */	"NumCallsPreflop, NumCallsOnFlop, NumCallsOnTurn, NumCallsOnRiver, NumCallsOn7th, "
/*  80 */	"NumRaisesPreflop, NumRaisesOnFlop, NumRaisesOnTurn, NumRaisesOnRiver, NumRaisesOn7th, "
/*  85 */	"NumCheckCallOnFlop, NumCheckCallOnTurn, NumCheckCallOnRiver, NumCheckCallOn7th, NumCheckRaiseOnFlop, "
/*  90 */	"NumCheckRaiseOnTurn, NumCheckRaiseOnRiver, NumCheckRaiseOn7th, NumCheckFoldOnFlop, NumCheckFoldOnTurn, "
/*  95 */	"NumCheckFoldOnRiver, NumCheckFoldOn7th, NumOpenFoldPowerfulHand, NumFoldToExtremeOdds, NumReraiseWithGarbage, "
/* 100 */	"NumCheckBehindOnRiver, NumFoldWhenCouldCheck, NumAttemptStealBlinds, NumFoldToStealFromSBOpps, NumFoldToStealFromSB, "
/* 105 */	"NumFoldToStealFromBBOpps, NumFoldToStealFromBB, NumCheckRaiseOppsOnFlop, NumCheckRaiseOppsOnTurn, NumCheckRaiseOppsOnRiver, "
/* 110 */	"NumCheckRaiseOppsOn7th, NumAttemptStealBlindsOpps, Num4BetOpportunities, NumTurnsFacingBetOnlyOnFlop, NumTurnsFacingBetOnlyOnTurn, "
/* 115 */	"NumTurnsFacingBetOnlyOnRiver, NumTurnsFacingBetOnlyOn7th, numCallBetsOnlyPreflop, NumCallBetsOnlyOnFlop, NumCallBetsOnlyOnTurn, "
/* 120 */	"NumCallBetsOnlyOnRiver, NumCallBetsOnlyOn7th, NumFoldToBetOnlyPreflop, NumFoldToBetOnlyOnFlop, NumFoldToBetOnlyOnTurn, "
/* 125 */	"NumFoldToBetOnlyOnRiver, NumFoldToBetOnlyOn7th, Partner0Name, Partner0Profit, Partner1Name, "
/* 130 */	"Partner1Profit, Partner2Name, Partner2Profit, Partner3Name, Partner3Profit, "
/* 135 */	"Partner4Name, Partner4Profit, Partner5Name, Partner5Profit, Partner6Name, "
/* 140 */	"Partner6Profit, Partner7Name, Partner7Profit, Partner8Name, Partner8Profit, "
/* 145 */	"Partner9Name, Partner9Profit, TournamentId, TournSchedulingType, TournGameType, "
/* 150 */	"SessionType, SessionId, TournMaxEntry, Flags, Finished, "
/* 155 */	"AllInConfrontations, ShowdownAllInConfrontations, ptPosition, ptHoleCards, TotalBet, "
/* 160 */	"HandId, AggregationFlags, StatsFlags, StatsFlags2, StatsFlags3, "
/* 165 */	"StatsFlags4, StatsFlags5, StatsFlags6, StatsFlags7, StatsFlags8, "
/* 169 */	"StatsFlags9, StatsFlags10, StatsFlags11, HostId"; // New column must be added in ReadHandStatsStmt and WriteDeferredHandStmt class


void HandStatsStmtBase::prepareAndBind()
{
	prepare(query);

	bindFirst(cols.userId);
	bindNext(cols.gameType);
	bindNext(cols.structure);
	bindNext(cols.bigBlind);
	bindNext(cols.smallBlind);						// 5
	bindNext(cols.currency);
	bindNext(cols.maxPerTable);
	bindNext(cols.buyIn);
	bindNext(cols.totalWin);
	bindNext(cols.totalBlindsAntesPaid);			// 10
	bindNext(cols.totalRakePaid);
	bindNext(cols.numBigBlinds);
	bindNext(cols.numSmallBlinds);
	bindNext(cols.numBlindsPaidOutOfPosition);
	bindNext(cols.numVpip);							// 15
	bindNext(cols.numVpipFromSmallBlind);
	bindNext(cols.numVpipFromBigBlind);
	bindNext(cols.numSawShowdown);
	bindNext(cols.numWonShowdown);
	bindNext(cols.numWonAfterFlop);					// 20
	bindNext(cols.numLimp);
	bindNext(cols.numLimpRaise);
	bindNext(cols.numLimpCall);
	bindNext(cols.numLimpFold);
	bindNext(cols.numAllinPreflop);					// 25
	bindNext(cols.numColdCallOpportunities);
	bindNext(cols.numColdCalls);
	bindNext(cols.num3BetOpportunities);
	bindNext(cols.num3Bets);
	bindNext(cols.numFacing3Bet);					// 30
	bindNext(cols.numFoldTo3Bet);
	bindNext(cols.num4plusBet);
	bindNext(cols.numFacing4plusBet);
	bindNext(cols.numFoldTo4plusBet);
	bindNext(cols.numSawFlop);						// 35
	bindNext(cols.numSawTurn);
	bindNext(cols.numSawRiver);
	bindNext(cols.numSaw7th);
	bindNext(cols.numTurnsPreflop);
	bindNext(cols.numTurnsFacingBetOnFlop);			// 40
	bindNext(cols.numTurnsFacingNoBetOnFlop);
	bindNext(cols.numTurnsFacingBetOnTurn);
	bindNext(cols.numTurnsFacingNoBetOnTurn);
	bindNext(cols.numTurnsFacingBetOnRiver);
	bindNext(cols.numTurnsFacingNoBetOnRiver);		// 45
	bindNext(cols.numTurnsFacingBetOn7th);
	bindNext(cols.numTurnsFacingNoBetOn7th);
	bindNext(cols.numLastAgressorPreflop);
	bindNext(cols.numLastAgressorOnFlop);
	bindNext(cols.numLastAgressorOnTurn);			// 50
	bindNext(cols.numLastAgressorOnRiver);
	bindNext(cols.numHandsRaisedPreflop);
	bindNext(cols.numContBetOnFlop);
	bindNext(cols.numContBetOnTurn);
	bindNext(cols.numContBetOnRiver);				// 55
	bindNext(cols.numContBetOn7th);
	bindNext(cols.numBetPreflop);
	bindNext(cols.numBetOnFlop);
	bindNext(cols.numBetOnTurn);
	bindNext(cols.numBetOnRiver);					// 60
	bindNext(cols.numBetOn7th);
	bindNext(cols.numCheckOnFlop);
	bindNext(cols.numCheckOnTurn);
	bindNext(cols.numCheckOnRiver);
	bindNext(cols.numCheckOn7th);					// 65
	bindNext(cols.numFoldPreflop);
	bindNext(cols.numFoldOnFlop);
	bindNext(cols.numFoldOnTurn);
	bindNext(cols.numFoldOnRiver);
	bindNext(cols.numFoldOn7th);					// 70
	bindNext(cols.numCallsPreflop);
	bindNext(cols.numCallsOnFlop);
	bindNext(cols.numCallsOnTurn);
	bindNext(cols.numCallsOnRiver);
	bindNext(cols.numCallsOn7th);					// 75
	bindNext(cols.numRaisesPreflop);
	bindNext(cols.numRaisesOnFlop);
	bindNext(cols.numRaisesOnTurn);
	bindNext(cols.numRaisesOnRiver);
	bindNext(cols.numRaisesOn7th);					// 80
	bindNext(cols.numCheckCallOnFlop);
	bindNext(cols.numCheckCallOnTurn);
	bindNext(cols.numCheckCallOnRiver);
	bindNext(cols.numCheckCallOn7th);
	bindNext(cols.numCheckRaiseOnFlop);				// 85
	bindNext(cols.numCheckRaiseOnTurn);
	bindNext(cols.numCheckRaiseOnRiver);
	bindNext(cols.numCheckRaiseOn7th);
	bindNext(cols.numCheckFoldOnFlop);
	bindNext(cols.numCheckFoldOnTurn);				// 90
	bindNext(cols.numCheckFoldOnRiver);
	bindNext(cols.numCheckFoldOn7th);
	bindNext(cols.numOpenFoldPowerfulHand);
	bindNext(cols.numFoldToExtremeOdds);
	bindNext(cols.numReraiseWithGarbage);			// 95
	bindNext(cols.numCheckBehindOnRiver);
	bindNext(cols.numFoldWhenCouldCheck);
	bindNext(cols.numAttemptStealBlinds);
	bindNext(cols.numFoldToStealFromSBOpps);
	bindNext(cols.numFoldToStealFromSB);			// 100
	bindNext(cols.numFoldToStealFromBBOpps);
	bindNext(cols.numFoldToStealFromBB);
	bindNext(cols.numCheckRaiseOppsOnFlop);
	bindNext(cols.numCheckRaiseOppsOnTurn);
	bindNext(cols.numCheckRaiseOppsOnRiver);		// 105
	bindNext(cols.numCheckRaiseOppsOn7th);
	bindNext(cols.numAttemptStealBlindsOpps);
	bindNext(cols.num4BetOpportunities);
	bindNext(cols.numTurnsFacingBetOnlyOnFlop);
	bindNext(cols.numTurnsFacingBetOnlyOnTurn);		// 110
	bindNext(cols.numTurnsFacingBetOnlyOnRiver);
	bindNext(cols.numTurnsFacingBetOnlyOn7th);
	bindNext(cols.numCallBetsOnlyPreflop);
	bindNext(cols.numCallBetsOnlyOnFlop);
	bindNext(cols.numCallBetsOnlyOnTurn);			// 115
	bindNext(cols.numCallBetsOnlyOnRiver);
	bindNext(cols.numCallBetsOnlyOn7th);
	bindNext(cols.numFoldToBetOnlyPreflop);
	bindNext(cols.numFoldToBetOnlyOnFlop);
	bindNext(cols.numFoldToBetOnlyOnTurn);			// 120
	bindNext(cols.numFoldToBetOnlyOnRiver);
	bindNext(cols.numFoldToBetOnlyOn7th);			// 122

	for (UINT32 i = 0; i < MAX_TABLE_PLAYERS; ++i)	// 123 - 142
	{
		bindNext(cols.partnerNames[i]);
		bindNext(cols.partnerProfit[i]);
	}

	bindNext(cols.tournamentId);					// 143
	bindNext(cols.tournSchedulingType);
	bindNext(cols.tournGameType);					// 145
	bindNext(cols.sessionType);
	bindNext(cols.sessionId);
	bindNext(cols.tournMaxEntry);
	bindNext(cols.flags);
	bindNext(cols.finished);						// 150
	bindNext(cols.allInConfrontations);
	bindNext(cols.showdownAllInConfrontations);
	bindNext(cols.ptPosition);
	bindNext(cols.ptHoleCards);
	bindNext(cols.totalBet);						// 155
	bindNext(cols.handId);
	bindNext(cols.aggregationFlags);
	bindNext(cols.statsFlags);
	bindNext(cols.statsFlags2);
	bindNext(cols.statsFlags3);						// 160
	bindNext(cols.statsFlags4);
	bindNext(cols.statsFlags5);
	bindNext(cols.statsFlags6);
	bindNext(cols.statsFlags7);
	bindNext(cols.statsFlags8);						// 165
	bindNext(cols.statsFlags9);
	bindNext(cols.statsFlags10);
	bindNext(cols.statsFlags11);
	bindNext(cols.hostId);							
	bindNext(cols.tournActualEntries);				// 170
}

////////////////////////////////////////////////////////////////////////////////

ReadHandStatsStmt::ReadHandStatsStmt(DatabaseManagerCommon& man, HandStatsStmtColumns& cols_,
									 SrvDate date, UINT32 maxDeferredDays,
									 char const* handStatsRawTableName,
									 char const* deferredHandsTableName,
									 char const* completedTournsTableName) :
	HandStatsStmtBase(man, cols_, false /*isWriting*/)
{
	PString readHandStatsRawColumnNames(columnNames);
	readHandStatsRawColumnNames.append(", VALUE(ActualEntries, 0) as TournActualEntries");			/* 170 */

	PString buf, dateRange;
	srvDateToString(date, buf);
	dateRange.assign("BETWEEN '").append(buf).append("' AND '").append(buf).append("-23.59.59.999999'");

	query.assign("SELECT ").append(readHandStatsRawColumnNames)
		 .append(" FROM ").append(handStatsRawTableName)
		 .append(" LEFT JOIN (SELECT Finished as Fin, TournamentId as TournId, TournActualEntries as ActualEntries FROM ").append(completedTournsTableName)
		 .append(" WHERE Finished ").append(dateRange).append(") ON TournamentId = TournId")
		 .append(" WHERE Finished ").append(dateRange);

	if (maxDeferredDays)
	{
		PString readHandStatsDeferColumnNames(columnNames);
		readHandStatsDeferColumnNames.append(", ActualEntries as TournActualEntries");				/* 170 */

		query.append(" UNION ALL SELECT ").append(readHandStatsDeferColumnNames)
			 .append(" FROM ").append(*deferredHandsTableName ? deferredHandsTableName : handStatsRawTableName)
			 .append(" INNER JOIN (SELECT Finished as Fin, TournamentId as TournId, TournActualEntries as ActualEntries FROM ").append(completedTournsTableName)
			 .append(" WHERE Finished ").append(dateRange)
			 .append(") ON TournamentId = TournId WHERE Finished ");

		ColUtils::addDays(date, -static_cast<int>(maxDeferredDays));
		query.append("BETWEEN '").append(srvDateToString(date, buf));

		ColUtils::addDays(date, maxDeferredDays - 1);
		query.append("' AND '").append(srvDateToString(date, buf)).append("-23.59.59.999999'");
	}

	query.append(" ORDER BY UserId, HostId, SessionType, TournSchedulingType, TournGameType, BuyIn, TournMaxEntry, TournActualEntries, Flags, Currency, "
				 "MaxPerTable, GameType, Structure, BigBlind, SmallBlind, ptPosition, ptHoleCards, Finished "
				 "WITH UR");

	PLog("- ReadHandStatsStmt query: ");
	PDumpEndLine(query);

	prepareAndBind();
}

////////////////////////////////////////////////////////////////////////////////

WriteDeferredHandStmt::WriteDeferredHandStmt(DatabaseManagerCommon& man, HandStatsStmtColumns& cols_,
											 const char* deferredHandsTableName) :
	HandStatsStmtBase(man, cols_, true /*isWriting*/)
{
	PString writeHandStatsColumnNames(columnNames);
	writeHandStatsColumnNames.append(", TournActualEntries");					/* 170 */

	query.
		assign("INSERT INTO ").
		append(deferredHandsTableName).
		append(" (").
		append(writeHandStatsColumnNames).
		append(") VALUES (?");

	for (const char* p = writeHandStatsColumnNames.c_str(); *p; ++p)
	{
		if (*p == ',')
		{
			query.append(",?");
		}
	}

	query.append(")");

	prepareAndBind();
}

////////////////////////////////////////////////////////////////////////////////

HandStatsManager::~HandStatsManager()
{
	try
	{
		delete pReadHandStatsStmt;
		delete pWriteDeferredHandStmt;
	}
	catch (...) {}
}


void HandStatsManager::setup(SrvDate const& date, UINT32 maxDeferredDays,
							 char const* handStatsRawTableName, char const* deferredHandsTableName,
							 char const* completedTournsTableName)
{
	pReadHandStatsStmt = new ReadHandStatsStmt(man, cols, date, maxDeferredDays, handStatsRawTableName,
											   deferredHandsTableName, completedTournsTableName);
	pWriteDeferredHandStmt = new WriteDeferredHandStmt(man, cols, deferredHandsTableName);
}


void HandStatsManager::getKey(AggregateHandData& data) const
{
	data.smallBlind = cols.smallBlind.value; // ???
	data.bigBlind = cols.bigBlind.value;
	data.structure = cols.structure.value;
	data.gameType = cols.gameType.value;
	data.maxPerTable = cols.maxPerTable.value;
	data.currency = cols.currency.c_str();
	data.flags = cols.flags.value;
	data.buyIn = cols.buyIn.value;
	data.tournGameType = cols.tournGameType.value;
	data.tournSchedulingType = cols.tournSchedulingType.value;
	data.sessionType = cols.sessionType.value;
	data.userId = cols.userId.c_str();
	data.hostId = cols.hostId.value;
	data.tournMaxEntry = cols.tournMaxEntry.value;
	data.tournActualEntries = cols.tournActualEntries.value;
}


bool HandStatsManager::isGameOrPlayerChanged(AggregateHandData const& data) const
{
	return data.structure != cols.structure.value
		|| data.gameType != cols.gameType.value
		|| data.maxPerTable != cols.maxPerTable.value
		|| !data.currency.equals(cols.currency.c_str())
		|| data.flags != static_cast<UINT64>(cols.flags.value)
		|| data.buyIn != cols.buyIn.value
		|| data.tournGameType != static_cast<UINT16>(cols.tournGameType.value)
		|| data.tournSchedulingType != cols.tournSchedulingType.value
		|| data.sessionType != static_cast<UINT32>(cols.sessionType.value)
		|| !data.userId.equals(cols.userId.c_str())
		|| data.hostId != cols.hostId.value
		|| data.tournMaxEntry != cols.tournMaxEntry.value
		|| data.tournActualEntries != cols.tournActualEntries.value;
}


bool HandStatsManager::isBlindsChanged(AggregateHandData const& data) const
{
	return data.smallBlind != static_cast<UINT32>(cols.smallBlind.value) // ???
		|| data.bigBlind != static_cast<UINT32>(cols.bigBlind.value);
}


void HandStatsManager::aggregateStats(AggregateHandData& data) const
{ //lint --e{737} loss of sign
	++data.totalHands;
	data.totalWin += cols.totalWin.value;

	if (data.buyIn == -1) // TODO: remove after June build
		data.totalBet += cols.totalBet.value;

	data.totalBlindsAntesPaid += cols.totalBlindsAntesPaid.value;
	data.totalRakePaid += cols.totalRakePaid.value;
	data.numBigBlinds += cols.numBigBlinds.value;
	data.numSmallBlinds += cols.numSmallBlinds.value;
	data.numBlindsPaidOutOfPosition += cols.numBlindsPaidOutOfPosition.value;
	data.numVpip += cols.numVpip.value;
	data.numVpipFromSmallBlind += cols.numVpipFromSmallBlind.value;
	data.numVpipFromBigBlind += cols.numVpipFromBigBlind.value;
	data.numSawShowdown += cols.numSawShowdown.value;
	data.numWonShowdown += cols.numWonShowdown.value;
	data.numWonAfterFlop += cols.numWonAfterFlop.value;
	data.numHandsRaisedPreflop += cols.numHandsRaisedPreflop.value;
	data.numLimp += cols.numLimp.value;
	data.numLimpRaise += cols.numLimpRaise.value;
	data.numLimpCall += cols.numLimpCall.value;
	data.numLimpFold += cols.numLimpFold.value;
	data.numAllinPreflop += cols.numAllinPreflop.value;
	data.numColdCallOpportunities += cols.numColdCallOpportunities.value;
	data.numColdCalls += cols.numColdCalls.value;
	data.num3BetOpportunities += cols.num3BetOpportunities.value;
	data.num3Bets += cols.num3Bets.value;
	data.numFacing3Bet += cols.numFacing3Bet.value;
	data.numFoldTo3Bet += cols.numFoldTo3Bet.value;
	data.num4BetOpportunities += cols.num4BetOpportunities.value;
	data.num4plusBet += cols.num4plusBet.value;
	data.numFacing4plusBet += cols.numFacing4plusBet.value;
	data.numFoldTo4plusBet += cols.numFoldTo4plusBet.value;
	data.numAttemptStealBlindsOpps += cols.numAttemptStealBlindsOpps.value;
	data.numAttemptStealBlinds += cols.numAttemptStealBlinds.value;
	data.numFoldToStealFromSBOpps += cols.numFoldToStealFromSBOpps.value;
	data.numFoldToStealFromSB += cols.numFoldToStealFromSB.value;
	data.numFoldToStealFromBBOpps += cols.numFoldToStealFromBBOpps.value;
	data.numFoldToStealFromBB += cols.numFoldToStealFromBB.value;
	data.perRoundData[0].numSawRound += cols.numSawFlop.value;
	data.perRoundData[1].numSawRound += cols.numSawTurn.value;
	data.perRoundData[2].numSawRound += cols.numSawRiver.value;
	data.perRoundData[3].numSawRound += cols.numSaw7th.value;
	data.numTurnsPreflop += cols.numTurnsPreflop.value;
	data.perRoundData[0].numTurnsFacingBetOnRound += cols.numTurnsFacingBetOnFlop.value;
	data.perRoundData[0].numTurnsFacingBetOnlyOnRound += cols.numTurnsFacingBetOnlyOnFlop.value;
	data.perRoundData[0].numTurnsFacingNoBetOnRound += cols.numTurnsFacingNoBetOnFlop.value;
	data.perRoundData[1].numTurnsFacingBetOnRound += cols.numTurnsFacingBetOnTurn.value;
	data.perRoundData[1].numTurnsFacingBetOnlyOnRound += cols.numTurnsFacingBetOnlyOnTurn.value;
	data.perRoundData[1].numTurnsFacingNoBetOnRound += cols.numTurnsFacingNoBetOnTurn.value;
	data.perRoundData[2].numTurnsFacingBetOnRound += cols.numTurnsFacingBetOnRiver.value;
	data.perRoundData[2].numTurnsFacingBetOnlyOnRound += cols.numTurnsFacingBetOnlyOnRiver.value;
	data.perRoundData[2].numTurnsFacingNoBetOnRound += cols.numTurnsFacingNoBetOnRiver.value;
	data.perRoundData[3].numTurnsFacingBetOnRound += cols.numTurnsFacingBetOn7th.value;
	data.perRoundData[3].numTurnsFacingBetOnlyOnRound += cols.numTurnsFacingBetOnlyOn7th.value;
	data.perRoundData[3].numTurnsFacingNoBetOnRound += cols.numTurnsFacingNoBetOn7th.value;
	data.perRoundData[0].numContBetOppsOnRound += cols.numLastAgressorPreflop.value;
	data.perRoundData[1].numContBetOppsOnRound += cols.numLastAgressorOnFlop.value;
	data.perRoundData[2].numContBetOppsOnRound += cols.numLastAgressorOnTurn.value;
	data.perRoundData[3].numContBetOppsOnRound += cols.numLastAgressorOnRiver.value;
	data.perRoundData[0].numContBetOnRound += cols.numContBetOnFlop.value;
	data.perRoundData[1].numContBetOnRound += cols.numContBetOnTurn.value;
	data.perRoundData[2].numContBetOnRound += cols.numContBetOnRiver.value;
	data.perRoundData[3].numContBetOnRound += cols.numContBetOn7th.value;
	data.numBetPreflop += cols.numBetPreflop.value;
	data.perRoundData[0].numBetOnRound += cols.numBetOnFlop.value;
	data.perRoundData[1].numBetOnRound += cols.numBetOnTurn.value;
	data.perRoundData[2].numBetOnRound += cols.numBetOnRiver.value;
	data.perRoundData[3].numBetOnRound += cols.numBetOn7th.value;
	data.perRoundData[0].numCheckOnRound += cols.numCheckOnFlop.value;
	data.perRoundData[1].numCheckOnRound += cols.numCheckOnTurn.value;
	data.perRoundData[2].numCheckOnRound += cols.numCheckOnRiver.value;
	data.perRoundData[3].numCheckOnRound += cols.numCheckOn7th.value;
	data.numFoldPreflop += cols.numFoldPreflop.value;
	data.perRoundData[0].numFoldOnRound += cols.numFoldOnFlop.value;
	data.perRoundData[1].numFoldOnRound += cols.numFoldOnTurn.value;
	data.perRoundData[2].numFoldOnRound += cols.numFoldOnRiver.value;
	data.perRoundData[3].numFoldOnRound += cols.numFoldOn7th.value;
	data.numFoldToBetOnlyPreflop += cols.numFoldToBetOnlyPreflop.value;
	data.perRoundData[0].numFoldToBetOnlyOnRound += cols.numFoldToBetOnlyOnFlop.value;
	data.perRoundData[1].numFoldToBetOnlyOnRound += cols.numFoldToBetOnlyOnTurn.value;
	data.perRoundData[2].numFoldToBetOnlyOnRound += cols.numFoldToBetOnlyOnRiver.value;
	data.perRoundData[3].numFoldToBetOnlyOnRound += cols.numFoldToBetOnlyOn7th.value;
	data.numCallsPreflop += cols.numCallsPreflop.value;
	data.numCallBetsOnlyPreflop += cols.numCallBetsOnlyPreflop.value;
	data.perRoundData[0].numCallsOnRound += cols.numCallsOnFlop.value;
	data.perRoundData[1].numCallsOnRound += cols.numCallsOnTurn.value;
	data.perRoundData[2].numCallsOnRound += cols.numCallsOnRiver.value;
	data.perRoundData[3].numCallsOnRound += cols.numCallsOn7th.value;
	data.perRoundData[0].numCallBetsOnlyOnRound += cols.numCallBetsOnlyOnFlop.value;
	data.perRoundData[1].numCallBetsOnlyOnRound += cols.numCallBetsOnlyOnTurn.value;
	data.perRoundData[2].numCallBetsOnlyOnRound += cols.numCallBetsOnlyOnRiver.value;
	data.perRoundData[3].numCallBetsOnlyOnRound += cols.numCallBetsOnlyOn7th.value;
	data.numRaisesPreflop += cols.numRaisesPreflop.value;
	data.perRoundData[0].numRaisesOnRound += cols.numRaisesOnFlop.value;
	data.perRoundData[1].numRaisesOnRound += cols.numRaisesOnTurn.value;
	data.perRoundData[2].numRaisesOnRound += cols.numRaisesOnRiver.value;
	data.perRoundData[3].numRaisesOnRound += cols.numRaisesOn7th.value;
	data.perRoundData[0].numCheckCallOnRound += cols.numCheckCallOnFlop.value;
	data.perRoundData[1].numCheckCallOnRound += cols.numCheckCallOnTurn.value;
	data.perRoundData[2].numCheckCallOnRound += cols.numCheckCallOnRiver.value;
	data.perRoundData[3].numCheckCallOnRound += cols.numCheckCallOn7th.value;
	data.perRoundData[0].numCheckRaiseOnRound += cols.numCheckRaiseOnFlop.value;
	data.perRoundData[1].numCheckRaiseOnRound += cols.numCheckRaiseOnTurn.value;
	data.perRoundData[2].numCheckRaiseOnRound += cols.numCheckRaiseOnRiver.value;
	data.perRoundData[3].numCheckRaiseOnRound += cols.numCheckRaiseOn7th.value;
	data.perRoundData[0].numCheckFoldOnRound += cols.numCheckFoldOnFlop.value;
	data.perRoundData[1].numCheckFoldOnRound += cols.numCheckFoldOnTurn.value;
	data.perRoundData[2].numCheckFoldOnRound += cols.numCheckFoldOnRiver.value;
	data.perRoundData[3].numCheckFoldOnRound += cols.numCheckFoldOn7th.value;
	data.perRoundData[0].numCheckRaiseOppsOnRound += cols.numCheckRaiseOppsOnFlop.value;
	data.perRoundData[1].numCheckRaiseOppsOnRound += cols.numCheckRaiseOppsOnTurn.value;
	data.perRoundData[2].numCheckRaiseOppsOnRound += cols.numCheckRaiseOppsOnRiver.value;
	data.perRoundData[3].numCheckRaiseOppsOnRound += cols.numCheckRaiseOppsOn7th.value;
	data.numOpenFoldPowerfulHand += cols.numOpenFoldPowerfulHand.value;
	data.numFoldToExtremeOdds += cols.numFoldToExtremeOdds.value;
	data.numReraiseWithGarbage += cols.numReraiseWithGarbage.value;
	data.numCheckBehindOnRiver += cols.numCheckBehindOnRiver.value;
	data.numFoldWhenCouldCheck += cols.numFoldWhenCouldCheck.value;

	// PYR-30006
	data.numFacing3BetHEM += cols.isFacing3BetHEM() ? 1 : 0;
	data.numFoldTo3BetHEM += cols.isFoldTo3BetHEM() ? 1 : 0;
	data.numFacing4PlusBetHEM += cols.isFacing4PlusBetHEM() ? 1 : 0;
	data.numFoldTo4PlusBetHEM += cols.isFoldTo4PlusBetHEM() ? 1 : 0;

	// PYR-36556
	data.perRoundData[0].numContBetOppsVs2InPosOnRound += cols.isLastAgressorVs2InPosPreflop() ? 1 : 0;
	data.perRoundData[1].numContBetOppsVs2InPosOnRound += cols.isLastAgressorVs2InPosOnFlop() ? 1 : 0;
	data.perRoundData[2].numContBetOppsVs2InPosOnRound += cols.isLastAgressorVs2InPosOnTurn() ? 1 : 0;
	data.perRoundData[3].numContBetOppsVs2InPosOnRound += cols.isLastAgressorVs2InPosOnRiver() ? 1 : 0;
	data.perRoundData[0].numContBetVs2InPosOnRound += cols.isContBetVs2InPosOnFlop() ? 1 : 0;
	data.perRoundData[1].numContBetVs2InPosOnRound += cols.isContBetVs2InPosOnTurn() ? 1 : 0;
	data.perRoundData[2].numContBetVs2InPosOnRound += cols.isContBetVs2InPosOnRiver() ? 1 : 0;
	data.perRoundData[3].numContBetVs2InPosOnRound += cols.isContBetVs2InPosOn7th() ? 1 : 0;

	// PYR-62032
	data.numVpipBbRaised14PlusMBot50 += cols.isVpipBbRaised14PlusMBot50() ? 1 : 0;
	data.numVpipBbRaised14PlusMBot50Opps += cols.isVpipBbRaised14PlusMBot50Opps() ? 1 : 0;
	data.numVpipBbRaised14PlusMTop10 += cols.isVpipBbRaised14PlusMTop10() ? 1 : 0;
	data.numVpipBbRaised14PlusMTop10Opps += cols.isVpipBbRaised14PlusMTop10Opps() ? 1 : 0;
	data.numVpipBbHuRaised0To4MBot50 += cols.isVpipBbHuRaised0To4MBot50() ? 1 : 0;
	data.numVpipBbHuRaised0To4MBot50Opps += cols.isVpipBbHuRaised0To4MBot50Opps() ? 1 : 0;
	data.numVpipBbHuRaised0To4MTop30To50 += cols.isVpipBbHuRaised0To4MTop30To50() ? 1 : 0;
	data.numVpipBbHuRaised0To4MTop30To50Opps += cols.isVpipBbHuRaised0To4MTop30To50Opps() ? 1 : 0;
	data.numVpipBbHuRaised4To7MTop30To50 += cols.isVpipBbHuRaised4To7MTop30To50() ? 1 : 0;
	data.numVpipBbHuRaised4To7MTop30To50Opps += cols.isVpipBbHuRaised4To7MTop30To50Opps() ? 1 : 0;
	data.numVpipBbHuRaised4To7MBot50 += cols.isVpipBbHuRaised4To7MBot50() ? 1 : 0;
	data.numVpipBbHuRaised4To7MBot50Opps += cols.isVpipBbHuRaised4To7MBot50Opps() ? 1 : 0;
	data.numVpipBbHuRaised7To14MTop30To50 += cols.isVpipBbHuRaised7To14MTop30To50() ? 1 : 0;
	data.numVpipBbHuRaised7To14MTop30To50Opps += cols.isVpipBbHuRaised7To14MTop30To50Opps() ? 1 : 0;
	data.numVpipBbHuRaised7To14MBot50 += cols.isVpipBbHuRaised7To14MBot50() ? 1 : 0;
	data.numVpipBbHuRaised7To14MBot50Opps += cols.isVpipBbHuRaised7To14MBot50Opps() ? 1 : 0;
	data.numVpipBbHuRaised14PlusMTop30To50 += cols.isVpipBbHuRaised14PlusMTop30To50() ? 1 : 0;
	data.numVpipBbHuRaised14PlusMTop30To50Opps += cols.isVpipBbHuRaised14PlusMTop30To50Opps() ? 1 : 0;
	data.numVpipBbHuRaised14PlusMBot50 += cols.isVpipBbHuRaised14PlusMBot50() ? 1 : 0;
	data.numVpipBbHuRaised14PlusMBot50Opps += cols.isVpipBbHuRaised14PlusMBot50Opps() ? 1 : 0;
	data.numVpipBtnRaised0To4MTop10 += cols.isVpipBtnRaised0To4MTop10() ? 1 : 0;
	data.numVpipBtnRaised0To4MTop10Opps += cols.isVpipBtnRaised0To4MTop10Opps() ? 1 : 0;
	data.numVpipBtnRaised0To4MTop10To30 += cols.isVpipBtnRaised0To4MTop10To30() ? 1 : 0;
	data.numVpipBtnRaised0To4MTop10To30Opps += cols.isVpipBtnRaised0To4MTop10To30Opps() ? 1 : 0;
	data.numVpipBtnRaised10To14MTop10 += cols.isVpipBtnRaised10To14MTop10() ? 1 : 0;
	data.numVpipBtnRaised10To14MTop10Opps += cols.isVpipBtnRaised10To14MTop10Opps() ? 1 : 0;
	data.numVpipBtnRaised10To14MTop10To30 += cols.isVpipBtnRaised10To14MTop10To30() ? 1 : 0;
	data.numVpipBtnRaised10To14MTop10To30Opps += cols.isVpipBtnRaised10To14MTop10To30Opps() ? 1 : 0;
	data.numVpipBtnCoMpRaised14PlusMTop10 += cols.isVpipBtnCoMpRaised14PlusMTop10() ? 1 : 0;
	data.numVpipBtnCoMpRaised14PlusMTop10Opps += cols.isVpipBtnCoMpRaised14PlusMTop10Opps() ? 1 : 0;
	data.numVpipCoRaised0To4MTop10 += cols.isVpipCoRaised0To4MTop10() ? 1 : 0;
	data.numVpipCoRaised0To4MTop10Opps += cols.isVpipCoRaised0To4MTop10Opps() ? 1 : 0;
	data.numVpipCoRaised0To4MTop10To30 += cols.isVpipCoRaised0To4MTop10To30() ? 1 : 0;
	data.numVpipCoRaised0To4MTop10To30Opps += cols.isVpipCoRaised0To4MTop10To30Opps() ? 1 : 0;
	data.numVpipCoRaised4To7MTop10 += cols.isVpipCoRaised4To7MTop10() ? 1 : 0;
	data.numVpipCoRaised4To7MTop10Opps += cols.isVpipCoRaised4To7MTop10Opps() ? 1 : 0;
	data.numVpipCoRaised4To7MTop10To30 += cols.isVpipCoRaised4To7MTop10To30() ? 1 : 0;
	data.numVpipCoRaised4To7MTop10To30Opps += cols.isVpipCoRaised4To7MTop10To30Opps() ? 1 : 0;
	data.numVpipCoRaised7To10MTop10 += cols.isVpipCoRaised7To10MTop10() ? 1 : 0;
	data.numVpipCoRaised7To10MTop10Opps += cols.isVpipCoRaised7To10MTop10Opps() ? 1 : 0;
	data.numVpipCoRaised7To10MTop10To30 += cols.isVpipCoRaised7To10MTop10To30() ? 1 : 0;
	data.numVpipCoRaised7To10MTop10To30Opps += cols.isVpipCoRaised7To10MTop10To30Opps() ? 1 : 0;
	data.numVpipCoRaised10To14MTop10 += cols.isVpipCoRaised10To14MTop10() ? 1 : 0;
	data.numVpipCoRaised10To14MTop10Opps += cols.isVpipCoRaised10To14MTop10Opps() ? 1 : 0;
	data.numVpipCoRaised10To14MTop10To30 += cols.isVpipCoRaised10To14MTop10To30() ? 1 : 0;
	data.numVpipCoRaised10To14MTop10To30Opps += cols.isVpipCoRaised10To14MTop10To30Opps() ? 1 : 0;
	data.numVpipCoMpRaised14PlusMTop10To50 += cols.isVpipCoMpRaised14PlusMTop10To50() ? 1 : 0;
	data.numVpipCoMpRaised14PlusMTop10To50Opps += cols.isVpipCoMpRaised14PlusMTop10To50Opps() ? 1 : 0;
	data.numVpipEpRaised4To7MTop10 += cols.isVpipEpRaised4To7MTop10() ? 1 : 0;
	data.numVpipEpRaised4To7MTop10Opps += cols.isVpipEpRaised4To7MTop10Opps() ? 1 : 0;
	data.numVpipEpRaised7To10MTop10 += cols.isVpipEpRaised7To10MTop10() ? 1 : 0;
	data.numVpipEpRaised7To10MTop10Opps += cols.isVpipEpRaised7To10MTop10Opps() ? 1 : 0;
	data.numVpipEpRaised10To14MTop10 += cols.isVpipEpRaised10To14MTop10() ? 1 : 0;
	data.numVpipEpRaised10To14MTop10Opps += cols.isVpipEpRaised10To14MTop10Opps() ? 1 : 0;
	data.numVpipMpRaised0To4MTop10 += cols.isVpipMpRaised0To4MTop10() ? 1 : 0;
	data.numVpipMpRaised0To4MTop10Opps += cols.isVpipMpRaised0To4MTop10Opps() ? 1 : 0;
	data.numVpipMpRaised0To4MTop10To30 += cols.isVpipMpRaised0To4MTop10To30() ? 1 : 0;
	data.numVpipMpRaised0To4MTop10To30Opps += cols.isVpipMpRaised0To4MTop10To30Opps() ? 1 : 0;
	data.numVpipMpRaised4To7MTop10 += cols.isVpipMpRaised4To7MTop10() ? 1 : 0;
	data.numVpipMpRaised4To7MTop10Opps += cols.isVpipMpRaised4To7MTop10Opps() ? 1 : 0;
	data.numVpipMpRaised7To10MTop10 += cols.isVpipMpRaised7To10MTop10() ? 1 : 0;
	data.numVpipMpRaised7To10MTop10Opps += cols.isVpipMpRaised7To10MTop10Opps() ? 1 : 0;
	data.numVpipMpRaised10To14MTop10 += cols.isVpipMpRaised10To14MTop10() ? 1 : 0;
	data.numVpipMpRaised10To14MTop10Opps += cols.isVpipMpRaised10To14MTop10Opps() ? 1 : 0;
	data.numVpipSbRaised14PlusMTop30To50 += cols.isVpipSbRaised14PlusMTop30To50() ? 1 : 0;
	data.numVpipSbRaised14PlusMTop30To50Opps += cols.isVpipSbRaised14PlusMTop30To50Opps() ? 1 : 0;
	data.numVpipSbLimped7To14MBot50 += cols.isVpipSbLimped7To14MBot50() ? 1 : 0;
	data.numVpipSbLimped7To14MBot50Opps += cols.isVpipSbLimped7To14MBot50Opps() ? 1 : 0;
	data.numVpipSbLimped10To14MTop30To50 += cols.isVpipSbLimped10To14MTop30To50() ? 1 : 0;
	data.numVpipSbLimped10To14MTop30To50Opps += cols.isVpipSbLimped10To14MTop30To50Opps() ? 1 : 0;
	data.numVpipSbLimped14PlusMTop10 += cols.isVpipSbLimped14PlusMTop10() ? 1 : 0;
	data.numVpipSbLimped14PlusMTop10Opps += cols.isVpipSbLimped14PlusMTop10Opps() ? 1 : 0;
	data.numVpipSbLimped14PlusMTop10To30 += cols.isVpipSbLimped14PlusMTop10To30() ? 1 : 0;
	data.numVpipSbLimped14PlusMTop10To30Opps += cols.isVpipSbLimped14PlusMTop10To30Opps() ? 1 : 0;
	data.numVpipSbLimped14PlusMTop30To100 += cols.isVpipSbLimped14PlusMTop30To100() ? 1 : 0;
	data.numVpipSbLimped14PlusMTop30To100Opps += cols.isVpipSbLimped14PlusMTop30To100Opps() ? 1 : 0;
	data.numVpipSbUnopened4To14MBot50 += cols.isVpipSbUnopened4To14MBot50() ? 1 : 0;
	data.numVpipSbUnopened4To14MBot50Opps += cols.isVpipSbUnopened4To14MBot50Opps() ? 1 : 0;
	data.numVpipSbUnopened4To14MTop30To50 += cols.isVpipSbUnopened4To14MTop30To50() ? 1 : 0;
	data.numVpipSbUnopened4To14MTop30To50Opps += cols.isVpipSbUnopened4To14MTop30To50Opps() ? 1 : 0;
	data.numVpipSbHuUnopened0To4MBot50 += cols.isVpipSbHuUnopened0To4MBot50() ? 1 : 0;
	data.numVpipSbHuUnopened0To4MBot50Opps += cols.isVpipSbHuUnopened0To4MBot50Opps() ? 1 : 0;
	data.numVpipSbHuUnopened4To14MBot50 += cols.isVpipSbHuUnopened4To14MBot50() ? 1 : 0;
	data.numVpipSbHuUnopened4To14MBot50Opps += cols.isVpipSbHuUnopened4To14MBot50Opps() ? 1 : 0;
	data.numPfrBbLimped10To14MTop10To30 += cols.isPfrBbLimped10To14MTop10To30() ? 1 : 0;
	data.numPfrBbLimped10To14MTop10To30Opps += cols.isPfrBbLimped10To14MTop10To30Opps() ? 1 : 0;
	data.numPfrBbLimped14PlusMBot50 += cols.isPfrBbLimped14PlusMBot50() ? 1 : 0;
	data.numPfrBbLimped14PlusMBot50Opps += cols.isPfrBbLimped14PlusMBot50Opps() ? 1 : 0;
	data.numPfrBbLimped14PlusMTop10To50 += cols.isPfrBbLimped14PlusMTop10To50() ? 1 : 0;
	data.numPfrBbLimped14PlusMTop10To50Opps += cols.isPfrBbLimped14PlusMTop10To50Opps() ? 1 : 0;
	data.numPfrBbLimped4To7MBot50 += cols.isPfrBbLimped4To7MBot50() ? 1 : 0;
	data.numPfrBbLimped4To7MBot50Opps += cols.isPfrBbLimped4To7MBot50Opps() ? 1 : 0;
	data.numPfrBbLimped4To7MTop10To30 += cols.isPfrBbLimped4To7MTop10To30() ? 1 : 0;
	data.numPfrBbLimped4To7MTop10To30Opps += cols.isPfrBbLimped4To7MTop10To30Opps() ? 1 : 0;
	data.numPfrBbLimped4To7MTop30To50 += cols.isPfrBbLimped4To7MTop30To50() ? 1 : 0;
	data.numPfrBbLimped4To7MTop30To50Opps += cols.isPfrBbLimped4To7MTop30To50Opps() ? 1 : 0;
	data.numPfrBbLimped7To10MTop10To30 += cols.isPfrBbLimped7To10MTop10To30() ? 1 : 0;
	data.numPfrBbLimped7To10MTop10To30Opps += cols.isPfrBbLimped7To10MTop10To30Opps() ? 1 : 0;
	data.numPfrBbLimped7To14MBot50 += cols.isPfrBbLimped7To14MBot50() ? 1 : 0;
	data.numPfrBbLimped7To14MBot50Opps += cols.isPfrBbLimped7To14MBot50Opps() ? 1 : 0;
	data.numPfrBbLimped7To14MTop30To50 += cols.isPfrBbLimped7To14MTop30To50() ? 1 : 0;
	data.numPfrBbLimped7To14MTop30To50Opps += cols.isPfrBbLimped7To14MTop30To50Opps() ? 1 : 0;
	data.numPfrBbHuLimped0To4MBot50 += cols.isPfrBbHuLimped0To4MBot50() ? 1 : 0;
	data.numPfrBbHuLimped0To4MBot50Opps += cols.isPfrBbHuLimped0To4MBot50Opps() ? 1 : 0;
	data.numPfrBbHuLimped0To4MTop10 += cols.isPfrBbHuLimped0To4MTop10() ? 1 : 0;
	data.numPfrBbHuLimped0To4MTop10Opps += cols.isPfrBbHuLimped0To4MTop10Opps() ? 1 : 0;
	data.numPfrBbHuLimped0To4MTop10To30 += cols.isPfrBbHuLimped0To4MTop10To30() ? 1 : 0;
	data.numPfrBbHuLimped0To4MTop10To30Opps += cols.isPfrBbHuLimped0To4MTop10To30Opps() ? 1 : 0;
	data.numPfrBbHuLimped0To4MTop30To50 += cols.isPfrBbHuLimped0To4MTop30To50() ? 1 : 0;
	data.numPfrBbHuLimped0To4MTop30To50Opps += cols.isPfrBbHuLimped0To4MTop30To50Opps() ? 1 : 0;
	data.numPfrBbHuLimped10To14MBot50 += cols.isPfrBbHuLimped10To14MBot50() ? 1 : 0;
	data.numPfrBbHuLimped10To14MBot50Opps += cols.isPfrBbHuLimped10To14MBot50Opps() ? 1 : 0;
	data.numPfrBbHuLimped10To14MTop10 += cols.isPfrBbHuLimped10To14MTop10() ? 1 : 0;
	data.numPfrBbHuLimped10To14MTop10Opps += cols.isPfrBbHuLimped10To14MTop10Opps() ? 1 : 0;
	data.numPfrBbHuLimped10To14MTop10To30 += cols.isPfrBbHuLimped10To14MTop10To30() ? 1 : 0;
	data.numPfrBbHuLimped10To14MTop10To30Opps += cols.isPfrBbHuLimped10To14MTop10To30Opps() ? 1 : 0;
	data.numPfrBbHuLimped10To14MTop30To50 += cols.isPfrBbHuLimped10To14MTop30To50() ? 1 : 0;
	data.numPfrBbHuLimped10To14MTop30To50Opps += cols.isPfrBbHuLimped10To14MTop30To50Opps() ? 1 : 0;
	data.numPfrBbHuLimped14PlusMBot50 += cols.isPfrBbHuLimped14PlusMBot50() ? 1 : 0;
	data.numPfrBbHuLimped14PlusMBot50Opps += cols.isPfrBbHuLimped14PlusMBot50Opps() ? 1 : 0;
	data.numPfrBbHuLimped14PlusMTop10To30 += cols.isPfrBbHuLimped14PlusMTop10To30() ? 1 : 0;
	data.numPfrBbHuLimped14PlusMTop10To30Opps += cols.isPfrBbHuLimped14PlusMTop10To30Opps() ? 1 : 0;
	data.numPfrBbHuLimped14PlusMTop30To50 += cols.isPfrBbHuLimped14PlusMTop30To50() ? 1 : 0;
	data.numPfrBbHuLimped14PlusMTop30To50Opps += cols.isPfrBbHuLimped14PlusMTop30To50Opps() ? 1 : 0;
	data.numPfrBbHuLimped4To7MBot50 += cols.isPfrBbHuLimped4To7MBot50() ? 1 : 0;
	data.numPfrBbHuLimped4To7MBot50Opps += cols.isPfrBbHuLimped4To7MBot50Opps() ? 1 : 0;
	data.numPfrBbHuLimped4To7MTop10 += cols.isPfrBbHuLimped4To7MTop10() ? 1 : 0;
	data.numPfrBbHuLimped4To7MTop10Opps += cols.isPfrBbHuLimped4To7MTop10Opps() ? 1 : 0;
	data.numPfrBbHuLimped4To7MTop10To30 += cols.isPfrBbHuLimped4To7MTop10To30() ? 1 : 0;
	data.numPfrBbHuLimped4To7MTop10To30Opps += cols.isPfrBbHuLimped4To7MTop10To30Opps() ? 1 : 0;
	data.numPfrBbHuLimped4To7MTop30To50 += cols.isPfrBbHuLimped4To7MTop30To50() ? 1 : 0;
	data.numPfrBbHuLimped4To7MTop30To50Opps += cols.isPfrBbHuLimped4To7MTop30To50Opps() ? 1 : 0;
	data.numPfrBbHuLimped7To10MBot50 += cols.isPfrBbHuLimped7To10MBot50() ? 1 : 0;
	data.numPfrBbHuLimped7To10MBot50Opps += cols.isPfrBbHuLimped7To10MBot50Opps() ? 1 : 0;
	data.numPfrBbHuLimped7To10MTop10 += cols.isPfrBbHuLimped7To10MTop10() ? 1 : 0;
	data.numPfrBbHuLimped7To10MTop10Opps += cols.isPfrBbHuLimped7To10MTop10Opps() ? 1 : 0;
	data.numPfrBbHuLimped7To10MTop10To30 += cols.isPfrBbHuLimped7To10MTop10To30() ? 1 : 0;
	data.numPfrBbHuLimped7To10MTop10To30Opps += cols.isPfrBbHuLimped7To10MTop10To30Opps() ? 1 : 0;
	data.numPfrBbHuLimped7To10MTop30To50 += cols.isPfrBbHuLimped7To10MTop30To50() ? 1 : 0;
	data.numPfrBbHuLimped7To10MTop30To50Opps += cols.isPfrBbHuLimped7To10MTop30To50Opps() ? 1 : 0;
	data.numPfrBbSbLimped14PlusMTop10 += cols.isPfrBbSbLimped14PlusMTop10() ? 1 : 0;
	data.numPfrBbSbLimped14PlusMTop10Opps += cols.isPfrBbSbLimped14PlusMTop10Opps() ? 1 : 0;
	data.numPfrBtnLimped14PlusMTop10To50 += cols.isPfrBtnLimped14PlusMTop10To50() ? 1 : 0;
	data.numPfrBtnLimped14PlusMTop10To50Opps += cols.isPfrBtnLimped14PlusMTop10To50Opps() ? 1 : 0;
	data.numPfrBtnLimped4To14MTop10To50 += cols.isPfrBtnLimped4To14MTop10To50() ? 1 : 0;
	data.numPfrBtnLimped4To14MTop10To50Opps += cols.isPfrBtnLimped4To14MTop10To50Opps() ? 1 : 0;
	data.numPfrBtnUnopened0To4MBot50 += cols.isPfrBtnUnopened0To4MBot50() ? 1 : 0;
	data.numPfrBtnUnopened0To4MBot50Opps += cols.isPfrBtnUnopened0To4MBot50Opps() ? 1 : 0;
	data.numPfrBtnUnopened0To4MTop30To50 += cols.isPfrBtnUnopened0To4MTop30To50() ? 1 : 0;
	data.numPfrBtnUnopened0To4MTop30To50Opps += cols.isPfrBtnUnopened0To4MTop30To50Opps() ? 1 : 0;
	data.numPfrBtnUnopened14PlusMBot50 += cols.isPfrBtnUnopened14PlusMBot50() ? 1 : 0;
	data.numPfrBtnUnopened14PlusMBot50Opps += cols.isPfrBtnUnopened14PlusMBot50Opps() ? 1 : 0;
	data.numPfrBtnUnopened14PlusMTop10To50 += cols.isPfrBtnUnopened14PlusMTop10To50() ? 1 : 0;
	data.numPfrBtnUnopened14PlusMTop10To50Opps += cols.isPfrBtnUnopened14PlusMTop10To50Opps() ? 1 : 0;
	data.numPfrBtnUnopened4To7MTop10To30 += cols.isPfrBtnUnopened4To7MTop10To30() ? 1 : 0;
	data.numPfrBtnUnopened4To7MTop10To30Opps += cols.isPfrBtnUnopened4To7MTop10To30Opps() ? 1 : 0;
	data.numPfrBtnUnopened4To7MTop30To50 += cols.isPfrBtnUnopened4To7MTop30To50() ? 1 : 0;
	data.numPfrBtnUnopened4To7MTop30To50Opps += cols.isPfrBtnUnopened4To7MTop30To50Opps() ? 1 : 0;
	data.numPfrBtnUnopened7To14MTop10To30 += cols.isPfrBtnUnopened7To14MTop10To30() ? 1 : 0;
	data.numPfrBtnUnopened7To14MTop10To30Opps += cols.isPfrBtnUnopened7To14MTop10To30Opps() ? 1 : 0;
	data.numPfrBtnUnopened7To14MTop30To50 += cols.isPfrBtnUnopened7To14MTop30To50() ? 1 : 0;
	data.numPfrBtnUnopened7To14MTop30To50Opps += cols.isPfrBtnUnopened7To14MTop30To50Opps() ? 1 : 0;
	data.numPfrCoLimped14PlusMTop10To30 += cols.isPfrCoLimped14PlusMTop10To30() ? 1 : 0;
	data.numPfrCoLimped14PlusMTop10To30Opps += cols.isPfrCoLimped14PlusMTop10To30Opps() ? 1 : 0;
	data.numPfrCoLimped14PlusMTop30To50 += cols.isPfrCoLimped14PlusMTop30To50() ? 1 : 0;
	data.numPfrCoLimped14PlusMTop30To50Opps += cols.isPfrCoLimped14PlusMTop30To50Opps() ? 1 : 0;
	data.numPfrCoUnopened0To4MTop10To30 += cols.isPfrCoUnopened0To4MTop10To30() ? 1 : 0;
	data.numPfrCoUnopened0To4MTop10To30Opps += cols.isPfrCoUnopened0To4MTop10To30Opps() ? 1 : 0;
	data.numPfrCoUnopened0To4MTop30To50 += cols.isPfrCoUnopened0To4MTop30To50() ? 1 : 0;
	data.numPfrCoUnopened0To4MTop30To50Opps += cols.isPfrCoUnopened0To4MTop30To50Opps() ? 1 : 0;
	data.numPfrCoUnopened10To14MTop10To30 += cols.isPfrCoUnopened10To14MTop10To30() ? 1 : 0;
	data.numPfrCoUnopened10To14MTop10To30Opps += cols.isPfrCoUnopened10To14MTop10To30Opps() ? 1 : 0;
	data.numPfrCoUnopened10To14MTop30To50 += cols.isPfrCoUnopened10To14MTop30To50() ? 1 : 0;
	data.numPfrCoUnopened10To14MTop30To50Opps += cols.isPfrCoUnopened10To14MTop30To50Opps() ? 1 : 0;
	data.numPfrCoUnopened14PlusMTop30To50 += cols.isPfrCoUnopened14PlusMTop30To50() ? 1 : 0;
	data.numPfrCoUnopened14PlusMTop30To50Opps += cols.isPfrCoUnopened14PlusMTop30To50Opps() ? 1 : 0;
	data.numPfrCoUnopened4To7MTop10To30 += cols.isPfrCoUnopened4To7MTop10To30() ? 1 : 0;
	data.numPfrCoUnopened4To7MTop10To30Opps += cols.isPfrCoUnopened4To7MTop10To30Opps() ? 1 : 0;
	data.numPfrCoUnopened4To7MTop30To50 += cols.isPfrCoUnopened4To7MTop30To50() ? 1 : 0;
	data.numPfrCoUnopened4To7MTop30To50Opps += cols.isPfrCoUnopened4To7MTop30To50Opps() ? 1 : 0;
	data.numPfrCoUnopened7To10MTop10To30 += cols.isPfrCoUnopened7To10MTop10To30() ? 1 : 0;
	data.numPfrCoUnopened7To10MTop10To30Opps += cols.isPfrCoUnopened7To10MTop10To30Opps() ? 1 : 0;
	data.numPfrCoUnopened7To10MTop30To50 += cols.isPfrCoUnopened7To10MTop30To50() ? 1 : 0;
	data.numPfrCoUnopened7To10MTop30To50Opps += cols.isPfrCoUnopened7To10MTop30To50Opps() ? 1 : 0;
	data.numPfrCoBtnUnopened14PlusMTop10To30 += cols.isPfrCoBtnUnopened14PlusMTop10To30() ? 1 : 0;
	data.numPfrCoBtnUnopened14PlusMTop10To30Opps += cols.isPfrCoBtnUnopened14PlusMTop10To30Opps() ? 1 : 0;
	data.numPfrEpUnopened0To4MTop10 += cols.isPfrEpUnopened0To4MTop10() ? 1 : 0;
	data.numPfrEpUnopened0To4MTop10Opps += cols.isPfrEpUnopened0To4MTop10Opps() ? 1 : 0;
	data.numPfrEpUnopened0To4MTop10To30 += cols.isPfrEpUnopened0To4MTop10To30() ? 1 : 0;
	data.numPfrEpUnopened0To4MTop10To30Opps += cols.isPfrEpUnopened0To4MTop10To30Opps() ? 1 : 0;
	data.numPfrEpUnopened4To14MTop10 += cols.isPfrEpUnopened4To14MTop10() ? 1 : 0;
	data.numPfrEpUnopened4To14MTop10Opps += cols.isPfrEpUnopened4To14MTop10Opps() ? 1 : 0;
	data.numPfrEpUnopened4To14MTop10To30 += cols.isPfrEpUnopened4To14MTop10To30() ? 1 : 0;
	data.numPfrEpUnopened4To14MTop10To30Opps += cols.isPfrEpUnopened4To14MTop10To30Opps() ? 1 : 0;
	data.numPfrMpUnopened0To4MTop10To30 += cols.isPfrMpUnopened0To4MTop10To30() ? 1 : 0;
	data.numPfrMpUnopened0To4MTop10To30Opps += cols.isPfrMpUnopened0To4MTop10To30Opps() ? 1 : 0;
	data.numPfrMpUnopened0To4MTop30To50 += cols.isPfrMpUnopened0To4MTop30To50() ? 1 : 0;
	data.numPfrMpUnopened0To4MTop30To50Opps += cols.isPfrMpUnopened0To4MTop30To50Opps() ? 1 : 0;
	data.numPfrMpUnopened4To14MTop10To30 += cols.isPfrMpUnopened4To14MTop10To30() ? 1 : 0;
	data.numPfrMpUnopened4To14MTop10To30Opps += cols.isPfrMpUnopened4To14MTop10To30Opps() ? 1 : 0;
	data.numPfrMpEPUnopened14PlusMTop10To30 += cols.isPfrMpEPUnopened14PlusMTop10To30() ? 1 : 0;
	data.numPfrMpEPUnopened14PlusMTop10To30Opps += cols.isPfrMpEPUnopened14PlusMTop10To30Opps() ? 1 : 0;
	data.numPfrMpEPUnopened14PlusMTop30To50 += cols.isPfrMpEPUnopened14PlusMTop30To50() ? 1 : 0;
	data.numPfrMpEPUnopened14PlusMTop30To50Opps += cols.isPfrMpEPUnopened14PlusMTop30To50Opps() ? 1 : 0;
	data.numPfrSbLimped10To14MTop10 += cols.isPfrSbLimped10To14MTop10() ? 1 : 0;
	data.numPfrSbLimped10To14MTop10Opps += cols.isPfrSbLimped10To14MTop10Opps() ? 1 : 0;
	data.numPfrSbLimped10To14MTop10To30 += cols.isPfrSbLimped10To14MTop10To30() ? 1 : 0;
	data.numPfrSbLimped10To14MTop10To30Opps += cols.isPfrSbLimped10To14MTop10To30Opps() ? 1 : 0;
	data.numPfrSbLimped10To14MTop30To50 += cols.isPfrSbLimped10To14MTop30To50() ? 1 : 0;
	data.numPfrSbLimped10To14MTop30To50Opps += cols.isPfrSbLimped10To14MTop30To50Opps() ? 1 : 0;
	data.numPfrSbLimped14PlusMTop10To30 += cols.isPfrSbLimped14PlusMTop10To30() ? 1 : 0;
	data.numPfrSbLimped14PlusMTop10To30Opps += cols.isPfrSbLimped14PlusMTop10To30Opps() ? 1 : 0;
	data.numPfrSbLimped14PlusMTop30To50 += cols.isPfrSbLimped14PlusMTop30To50() ? 1 : 0;
	data.numPfrSbLimped14PlusMTop30To50Opps += cols.isPfrSbLimped14PlusMTop30To50Opps() ? 1 : 0;
	data.numPfrSbLimped7To10MTop10 += cols.isPfrSbLimped7To10MTop10() ? 1 : 0;
	data.numPfrSbLimped7To10MTop10Opps += cols.isPfrSbLimped7To10MTop10Opps() ? 1 : 0;
	data.numPfrSbLimped7To10MTop10To30 += cols.isPfrSbLimped7To10MTop10To30() ? 1 : 0;
	data.numPfrSbLimped7To10MTop10To30Opps += cols.isPfrSbLimped7To10MTop10To30Opps() ? 1 : 0;
	data.numPfrSbLimped7To10MTop30To50 += cols.isPfrSbLimped7To10MTop30To50() ? 1 : 0;
	data.numPfrSbLimped7To10MTop30To50Opps += cols.isPfrSbLimped7To10MTop30To50Opps() ? 1 : 0;
	data.numPfrSbUnopened0To4MTop30To50 += cols.isPfrSbUnopened0To4MTop30To50() ? 1 : 0;
	data.numPfrSbUnopened0To4MTop30To50Opps += cols.isPfrSbUnopened0To4MTop30To50Opps() ? 1 : 0;
	data.numPfrSbUnopened0To7MBot50 += cols.isPfrSbUnopened0To7MBot50() ? 1 : 0;
	data.numPfrSbUnopened0To7MBot50Opps += cols.isPfrSbUnopened0To7MBot50Opps() ? 1 : 0;
	data.numPfrSbUnopened10To14MTop10 += cols.isPfrSbUnopened10To14MTop10() ? 1 : 0;
	data.numPfrSbUnopened10To14MTop10Opps += cols.isPfrSbUnopened10To14MTop10Opps() ? 1 : 0;
	data.numPfrSbUnopened10To14MTop10To30 += cols.isPfrSbUnopened10To14MTop10To30() ? 1 : 0;
	data.numPfrSbUnopened10To14MTop10To30Opps += cols.isPfrSbUnopened10To14MTop10To30Opps() ? 1 : 0;
	data.numPfrSbUnopened10To14MTop30To50 += cols.isPfrSbUnopened10To14MTop30To50() ? 1 : 0;
	data.numPfrSbUnopened10To14MTop30To50Opps += cols.isPfrSbUnopened10To14MTop30To50Opps() ? 1 : 0;
	data.numPfrSbUnopened14PlusMBot50 += cols.isPfrSbUnopened14PlusMBot50() ? 1 : 0;
	data.numPfrSbUnopened14PlusMBot50Opps += cols.isPfrSbUnopened14PlusMBot50Opps() ? 1 : 0;
	data.numPfrSbUnopened14PlusMTop10To30 += cols.isPfrSbUnopened14PlusMTop10To30() ? 1 : 0;
	data.numPfrSbUnopened14PlusMTop10To30Opps += cols.isPfrSbUnopened14PlusMTop10To30Opps() ? 1 : 0;
	data.numPfrSbUnopened14PlusMTop30To50 += cols.isPfrSbUnopened14PlusMTop30To50() ? 1 : 0;
	data.numPfrSbUnopened14PlusMTop30To50Opps += cols.isPfrSbUnopened14PlusMTop30To50Opps() ? 1 : 0;
	data.numPfrSbUnopened4To7MTop10 += cols.isPfrSbUnopened4To7MTop10() ? 1 : 0;
	data.numPfrSbUnopened4To7MTop10Opps += cols.isPfrSbUnopened4To7MTop10Opps() ? 1 : 0;
	data.numPfrSbUnopened4To7MTop10To30 += cols.isPfrSbUnopened4To7MTop10To30() ? 1 : 0;
	data.numPfrSbUnopened4To7MTop10To30Opps += cols.isPfrSbUnopened4To7MTop10To30Opps() ? 1 : 0;
	data.numPfrSbUnopened7To10MTop10 += cols.isPfrSbUnopened7To10MTop10() ? 1 : 0;
	data.numPfrSbUnopened7To10MTop10Opps += cols.isPfrSbUnopened7To10MTop10Opps() ? 1 : 0;
	data.numPfrSbUnopened7To10MTop10To30 += cols.isPfrSbUnopened7To10MTop10To30() ? 1 : 0;
	data.numPfrSbUnopened7To10MTop10To30Opps += cols.isPfrSbUnopened7To10MTop10To30Opps() ? 1 : 0;
	data.numPfrSbUnopened7To10MTop30To50 += cols.isPfrSbUnopened7To10MTop30To50() ? 1 : 0;
	data.numPfrSbUnopened7To10MTop30To50Opps += cols.isPfrSbUnopened7To10MTop30To50Opps() ? 1 : 0;
	data.numPfrSbUnopened7To14MBot50 += cols.isPfrSbUnopened7To14MBot50() ? 1 : 0;
	data.numPfrSbUnopened7To14MBot50Opps += cols.isPfrSbUnopened7To14MBot50Opps() ? 1 : 0;
	data.numPfrSbHuUnopened0To4MTop10 += cols.isPfrSbHuUnopened0To4MTop10() ? 1 : 0;
	data.numPfrSbHuUnopened0To4MTop10Opps += cols.isPfrSbHuUnopened0To4MTop10Opps() ? 1 : 0;
	data.numPfrSbHuUnopened0To4MTop10To30 += cols.isPfrSbHuUnopened0To4MTop10To30() ? 1 : 0;
	data.numPfrSbHuUnopened0To4MTop10To30Opps += cols.isPfrSbHuUnopened0To4MTop10To30Opps() ? 1 : 0;
	data.numPfrSbHuUnopened0To4MTop30To50 += cols.isPfrSbHuUnopened0To4MTop30To50() ? 1 : 0;
	data.numPfrSbHuUnopened0To4MTop30To50Opps += cols.isPfrSbHuUnopened0To4MTop30To50Opps() ? 1 : 0;
	data.numPfrSbHuUnopened14PlusMBot50 += cols.isPfrSbHuUnopened14PlusMBot50() ? 1 : 0;
	data.numPfrSbHuUnopened14PlusMBot50Opps += cols.isPfrSbHuUnopened14PlusMBot50Opps() ? 1 : 0;
	data.numPfrSbHuUnopened14PlusMTop30To50 += cols.isPfrSbHuUnopened14PlusMTop30To50() ? 1 : 0;
	data.numPfrSbHuUnopened14PlusMTop30To50Opps += cols.isPfrSbHuUnopened14PlusMTop30To50Opps() ? 1 : 0;
	data.numPfrSbHuUnopened4To14MBot50 += cols.isPfrSbHuUnopened4To14MBot50() ? 1 : 0;
	data.numPfrSbHuUnopened4To14MBot50Opps += cols.isPfrSbHuUnopened4To14MBot50Opps() ? 1 : 0;
	data.numPfrSbHuUnopened4To7MTop10 += cols.isPfrSbHuUnopened4To7MTop10() ? 1 : 0;
	data.numPfrSbHuUnopened4To7MTop10Opps += cols.isPfrSbHuUnopened4To7MTop10Opps() ? 1 : 0;
	data.numPfrSbHuUnopened4To7MTop10To30 += cols.isPfrSbHuUnopened4To7MTop10To30() ? 1 : 0;
	data.numPfrSbHuUnopened4To7MTop10To30Opps += cols.isPfrSbHuUnopened4To7MTop10To30Opps() ? 1 : 0;
	data.numPfrSbHuUnopened4To7MTop30To50 += cols.isPfrSbHuUnopened4To7MTop30To50() ? 1 : 0;
	data.numPfrSbHuUnopened4To7MTop30To50Opps += cols.isPfrSbHuUnopened4To7MTop30To50Opps() ? 1 : 0;
	data.numPfrSbHuUnopened7To14MTop10 += cols.isPfrSbHuUnopened7To14MTop10() ? 1 : 0;
	data.numPfrSbHuUnopened7To14MTop10Opps += cols.isPfrSbHuUnopened7To14MTop10Opps() ? 1 : 0;
	data.numPfrSbHuUnopened7To14MTop10To30 += cols.isPfrSbHuUnopened7To14MTop10To30() ? 1 : 0;
	data.numPfrSbHuUnopened7To14MTop10To30Opps += cols.isPfrSbHuUnopened7To14MTop10To30Opps() ? 1 : 0;
	data.numPfrSbHuUnopened7To14MTop30To50 += cols.isPfrSbHuUnopened7To14MTop30To50() ? 1 : 0;
	data.numPfrSbHuUnopened7To14MTop30To50Opps += cols.isPfrSbHuUnopened7To14MTop30To50Opps() ? 1 : 0;
	data.num3BetBb10To14MTop10 += cols.is_3BetBb10To14MTop10() ? 1 : 0;
	data.num3BetBb10To14MTop10Opps += cols.is_3BetBb10To14MTop10Opps() ? 1 : 0;
	data.num3BetBb10To14MTop10To30 += cols.is_3BetBb10To14MTop10To30() ? 1 : 0;
	data.num3BetBb10To14MTop10To30Opps += cols.is_3BetBb10To14MTop10To30Opps() ? 1 : 0;
	data.num3BetBb10To14MTop30To50 += cols.is_3BetBb10To14MTop30To50() ? 1 : 0;
	data.num3BetBb10To14MTop30To50Opps += cols.is_3BetBb10To14MTop30To50Opps() ? 1 : 0;
	data.num3BetBb14PlusMTop10 += cols.is_3BetBb14PlusMTop10() ? 1 : 0;
	data.num3BetBb14PlusMTop10Opps += cols.is_3BetBb14PlusMTop10Opps() ? 1 : 0;
	data.num3BetBb14PlusMTop10To30 += cols.is_3BetBb14PlusMTop10To30() ? 1 : 0;
	data.num3BetBb14PlusMTop10To30Opps += cols.is_3BetBb14PlusMTop10To30Opps() ? 1 : 0;
	data.num3BetBb4To7MTop10 += cols.is_3BetBb4To7MTop10() ? 1 : 0;
	data.num3BetBb4To7MTop10Opps += cols.is_3BetBb4To7MTop10Opps() ? 1 : 0;
	data.num3BetBb4To7MTop10To30 += cols.is_3BetBb4To7MTop10To30() ? 1 : 0;
	data.num3BetBb4To7MTop10To30Opps += cols.is_3BetBb4To7MTop10To30Opps() ? 1 : 0;
	data.num3BetBb4To7MTop30To50 += cols.is_3BetBb4To7MTop30To50() ? 1 : 0;
	data.num3BetBb4To7MTop30To50Opps += cols.is_3BetBb4To7MTop30To50Opps() ? 1 : 0;
	data.num3BetBb7To10MTop10 += cols.is_3BetBb7To10MTop10() ? 1 : 0;
	data.num3BetBb7To10MTop10Opps += cols.is_3BetBb7To10MTop10Opps() ? 1 : 0;
	data.num3BetBb7To10MTop10To30 += cols.is_3BetBb7To10MTop10To30() ? 1 : 0;
	data.num3BetBb7To10MTop10To30Opps += cols.is_3BetBb7To10MTop10To30Opps() ? 1 : 0;
	data.num3BetBb7To10MTop30To50 += cols.is_3BetBb7To10MTop30To50() ? 1 : 0;
	data.num3BetBb7To10MTop30To50Opps += cols.is_3BetBb7To10MTop30To50Opps() ? 1 : 0;
	data.num3BetBbHu10To14MTop10 += cols.is_3BetBbHu10To14MTop10() ? 1 : 0;
	data.num3BetBbHu10To14MTop10Opps += cols.is_3BetBbHu10To14MTop10Opps() ? 1 : 0;
	data.num3BetBbHu10To14MTop10To30 += cols.is_3BetBbHu10To14MTop10To30() ? 1 : 0;
	data.num3BetBbHu10To14MTop10To30Opps += cols.is_3BetBbHu10To14MTop10To30Opps() ? 1 : 0;
	data.num3BetBbHu10To14MTop30To50 += cols.is_3BetBbHu10To14MTop30To50() ? 1 : 0;
	data.num3BetBbHu10To14MTop30To50Opps += cols.is_3BetBbHu10To14MTop30To50Opps() ? 1 : 0;
	data.num3BetBbHu14PlusMTop10 += cols.is_3BetBbHu14PlusMTop10() ? 1 : 0;
	data.num3BetBbHu14PlusMTop10Opps += cols.is_3BetBbHu14PlusMTop10Opps() ? 1 : 0;
	data.num3BetBbHu14PlusMTop10To30 += cols.is_3BetBbHu14PlusMTop10To30() ? 1 : 0;
	data.num3BetBbHu14PlusMTop10To30Opps += cols.is_3BetBbHu14PlusMTop10To30Opps() ? 1 : 0;
	data.num3BetBbHu14PlusMTop30To50 += cols.is_3BetBbHu14PlusMTop30To50() ? 1 : 0;
	data.num3BetBbHu14PlusMTop30To50Opps += cols.is_3BetBbHu14PlusMTop30To50Opps() ? 1 : 0;
	data.num3BetBbHu4To7MTop10To30 += cols.is_3BetBbHu4To7MTop10To30() ? 1 : 0;
	data.num3BetBbHu4To7MTop10To30Opps += cols.is_3BetBbHu4To7MTop10To30Opps() ? 1 : 0;
	data.num3BetBbHu4To7MTop30To50 += cols.is_3BetBbHu4To7MTop30To50() ? 1 : 0;
	data.num3BetBbHu4To7MTop30To50Opps += cols.is_3BetBbHu4To7MTop30To50Opps() ? 1 : 0;
	data.num3BetBbHu7To10MTop10 += cols.is_3BetBbHu7To10MTop10() ? 1 : 0;
	data.num3BetBbHu7To10MTop10Opps += cols.is_3BetBbHu7To10MTop10Opps() ? 1 : 0;
	data.num3BetBbHu7To10MTop10To30 += cols.is_3BetBbHu7To10MTop10To30() ? 1 : 0;
	data.num3BetBbHu7To10MTop10To30Opps += cols.is_3BetBbHu7To10MTop10To30Opps() ? 1 : 0;
	data.num3BetBbHu7To10MTop30To50 += cols.is_3BetBbHu7To10MTop30To50() ? 1 : 0;
	data.num3BetBbHu7To10MTop30To50Opps += cols.is_3BetBbHu7To10MTop30To50Opps() ? 1 : 0;
	data.num3BetBtn10To14MTop10 += cols.is_3BetBtn10To14MTop10() ? 1 : 0;
	data.num3BetBtn10To14MTop10Opps += cols.is_3BetBtn10To14MTop10Opps() ? 1 : 0;
	data.num3BetBtn10To14MTop10To30 += cols.is_3BetBtn10To14MTop10To30() ? 1 : 0;
	data.num3BetBtn10To14MTop10To30Opps += cols.is_3BetBtn10To14MTop10To30Opps() ? 1 : 0;
	data.num3BetBtn14PlusMTop10 += cols.is_3BetBtn14PlusMTop10() ? 1 : 0;
	data.num3BetBtn14PlusMTop10Opps += cols.is_3BetBtn14PlusMTop10Opps() ? 1 : 0;
	data.num3BetBtn14PlusMTop10To30 += cols.is_3BetBtn14PlusMTop10To30() ? 1 : 0;
	data.num3BetBtn14PlusMTop10To30Opps += cols.is_3BetBtn14PlusMTop10To30Opps() ? 1 : 0;
	data.num3BetBtn4To7MTop10 += cols.is_3BetBtn4To7MTop10() ? 1 : 0;
	data.num3BetBtn4To7MTop10Opps += cols.is_3BetBtn4To7MTop10Opps() ? 1 : 0;
	data.num3BetBtn4To7MTop10To30 += cols.is_3BetBtn4To7MTop10To30() ? 1 : 0;
	data.num3BetBtn4To7MTop10To30Opps += cols.is_3BetBtn4To7MTop10To30Opps() ? 1 : 0;
	data.num3BetBtn7To10MTop10 += cols.is_3BetBtn7To10MTop10() ? 1 : 0;
	data.num3BetBtn7To10MTop10Opps += cols.is_3BetBtn7To10MTop10Opps() ? 1 : 0;
	data.num3BetBtn7To10MTop10To30 += cols.is_3BetBtn7To10MTop10To30() ? 1 : 0;
	data.num3BetBtn7To10MTop10To30Opps += cols.is_3BetBtn7To10MTop10To30Opps() ? 1 : 0;
	data.num3BetCo10To14MTop10 += cols.is_3BetCo10To14MTop10() ? 1 : 0;
	data.num3BetCo10To14MTop10Opps += cols.is_3BetCo10To14MTop10Opps() ? 1 : 0;
	data.num3BetCo14PlusMTop10To30 += cols.is_3BetCo14PlusMTop10To30() ? 1 : 0;
	data.num3BetCo14PlusMTop10To30Opps += cols.is_3BetCo14PlusMTop10To30Opps() ? 1 : 0;
	data.num3BetCo4To7MTop10 += cols.is_3BetCo4To7MTop10() ? 1 : 0;
	data.num3BetCo4To7MTop10Opps += cols.is_3BetCo4To7MTop10Opps() ? 1 : 0;
	data.num3BetCo7To10MTop10 += cols.is_3BetCo7To10MTop10() ? 1 : 0;
	data.num3BetCo7To10MTop10Opps += cols.is_3BetCo7To10MTop10Opps() ? 1 : 0;
	data.num3BetCoMp14PlusMTop10 += cols.is_3BetCoMp14PlusMTop10() ? 1 : 0;
	data.num3BetCoMp14PlusMTop10Opps += cols.is_3BetCoMp14PlusMTop10Opps() ? 1 : 0;
	data.num3BetMp10To14MTop10 += cols.is_3BetMp10To14MTop10() ? 1 : 0;
	data.num3BetMp10To14MTop10Opps += cols.is_3BetMp10To14MTop10Opps() ? 1 : 0;
	data.num3BetMp7To10MTop10 += cols.is_3BetMp7To10MTop10() ? 1 : 0;
	data.num3BetMp7To10MTop10Opps += cols.is_3BetMp7To10MTop10Opps() ? 1 : 0;
	data.num3BetSb10To14MTop10 += cols.is_3BetSb10To14MTop10() ? 1 : 0;
	data.num3BetSb10To14MTop10Opps += cols.is_3BetSb10To14MTop10Opps() ? 1 : 0;
	data.num3BetSb10To14MTop10To30 += cols.is_3BetSb10To14MTop10To30() ? 1 : 0;
	data.num3BetSb10To14MTop10To30Opps += cols.is_3BetSb10To14MTop10To30Opps() ? 1 : 0;
	data.num3BetSb14PlusMTop10 += cols.is_3BetSb14PlusMTop10() ? 1 : 0;
	data.num3BetSb14PlusMTop10Opps += cols.is_3BetSb14PlusMTop10Opps() ? 1 : 0;
	data.num3BetSb14PlusMTop10To30 += cols.is_3BetSb14PlusMTop10To30() ? 1 : 0;
	data.num3BetSb14PlusMTop10To30Opps += cols.is_3BetSb14PlusMTop10To30Opps() ? 1 : 0;
	data.num3BetSb4To7MTop10 += cols.is_3BetSb4To7MTop10() ? 1 : 0;
	data.num3BetSb4To7MTop10Opps += cols.is_3BetSb4To7MTop10Opps() ? 1 : 0;
	data.num3BetSb4To7MTop10To30 += cols.is_3BetSb4To7MTop10To30() ? 1 : 0;
	data.num3BetSb4To7MTop10To30Opps += cols.is_3BetSb4To7MTop10To30Opps() ? 1 : 0;
	data.num3BetSb4To7MTop30To50 += cols.is_3BetSb4To7MTop30To50() ? 1 : 0;
	data.num3BetSb4To7MTop30To50Opps += cols.is_3BetSb4To7MTop30To50Opps() ? 1 : 0;
	data.num3BetSb7To10MTop10 += cols.is_3BetSb7To10MTop10() ? 1 : 0;
	data.num3BetSb7To10MTop10Opps += cols.is_3BetSb7To10MTop10Opps() ? 1 : 0;
	data.num3BetSb7To10MTop10To30 += cols.is_3BetSb7To10MTop10To30() ? 1 : 0;
	data.num3BetSb7To10MTop10To30Opps += cols.is_3BetSb7To10MTop10To30Opps() ? 1 : 0;
	data.num3BetSb7To10MTop30To50 += cols.is_3BetSb7To10MTop30To50() ? 1 : 0;
	data.num3BetSb7To10MTop30To50Opps += cols.is_3BetSb7To10MTop30To50Opps() ? 1 : 0;
	data.num4BetBtn14PlusMTop10 += cols.is_4BetBtn14PlusMTop10() ? 1 : 0;
	data.num4BetBtn14PlusMTop10Opps += cols.is_4BetBtn14PlusMTop10Opps() ? 1 : 0;
	data.num4BetBtn14PlusMTop10To30 += cols.is_4BetBtn14PlusMTop10To30() ? 1 : 0;
	data.num4BetBtn14PlusMTop10To30Opps += cols.is_4BetBtn14PlusMTop10To30Opps() ? 1 : 0;
	data.num4BetCo14PlusMTop10to30 += cols.is_4BetCo14PlusMTop10to30() ? 1 : 0;
	data.num4BetCo14PlusMTop10to30Opps += cols.is_4BetCo14PlusMTop10to30Opps() ? 1 : 0;
	data.num4BetCoMpEp14PlusMTop10 += cols.is_4BetCoMpEp14PlusMTop10() ? 1 : 0;
	data.num4BetCoMpEp14PlusMTop10Opps += cols.is_4BetCoMpEp14PlusMTop10Opps() ? 1 : 0;
	data.num4BetMpEp14PlusMTop10To30 += cols.is_4BetMpEp14PlusMTop10To30() ? 1 : 0;
	data.num4BetMpEp14PlusMTop10To30Opps += cols.is_4BetMpEp14PlusMTop10To30Opps() ? 1 : 0;
	data.num4BetSbHu14PlusMTop10 += cols.is_4BetSbHu14PlusMTop10() ? 1 : 0;
	data.num4BetSbHu14PlusMTop10Opps += cols.is_4BetSbHu14PlusMTop10Opps() ? 1 : 0;
	data.numDefendBb0To4MBot50vsSBRaise += cols.isDefendBb0To4MBot50vsSBRaise() ? 1 : 0;
	data.numDefendBb0To4MBot50vsSBRaiseOpps += cols.isDefendBb0To4MBot50vsSBRaiseOpps() ? 1 : 0;
	data.numDefendBb0To4MTop10To30vsCoMpEpRaise += cols.isDefendBb0To4MTop10To30vsCoMpEpRaise() ? 1 : 0;
	data.numDefendBb0To4MTop10To30vsCoMpEpRaiseOpps += cols.isDefendBb0To4MTop10To30vsCoMpEpRaiseOpps() ? 1 : 0;
	data.numDefendBb0To4MTop30To50vsCoMpEpRaise += cols.isDefendBb0To4MTop30To50vsCoMpEpRaise() ? 1 : 0;
	data.numDefendBb0To4MTop30To50vsCoMpEpRaiseOpps += cols.isDefendBb0To4MTop30To50vsCoMpEpRaiseOpps() ? 1 : 0;
	data.numDefendBb0To4MTop30To50vsSBRaise += cols.isDefendBb0To4MTop30To50vsSBRaise() ? 1 : 0;
	data.numDefendBb0To4MTop30To50vsSBRaiseOpps += cols.isDefendBb0To4MTop30To50vsSBRaiseOpps() ? 1 : 0;
	data.numDefendBb10To14MBot50vsCoMpEpRaise += cols.isDefendBb10To14MBot50vsCoMpEpRaise() ? 1 : 0;
	data.numDefendBb10To14MBot50vsCoMpEpRaiseOpps += cols.isDefendBb10To14MBot50vsCoMpEpRaiseOpps() ? 1 : 0;
	data.numDefendBb10To14MTop10To30vsBtnRaise += cols.isDefendBb10To14MTop10To30vsBtnRaise() ? 1 : 0;
	data.numDefendBb10To14MTop10To30vsBtnRaiseOpps += cols.isDefendBb10To14MTop10To30vsBtnRaiseOpps() ? 1 : 0;
	data.numDefendBb10To14MTop10To30vsCoMpEpRaise += cols.isDefendBb10To14MTop10To30vsCoMpEpRaise() ? 1 : 0;
	data.numDefendBb10To14MTop10To30vsCoMpEpRaiseOpps += cols.isDefendBb10To14MTop10To30vsCoMpEpRaiseOpps() ? 1 : 0;
	data.numDefendBb10To14MTop10To30vsSBRaise += cols.isDefendBb10To14MTop10To30vsSBRaise() ? 1 : 0;
	data.numDefendBb10To14MTop10To30vsSBRaiseOpps += cols.isDefendBb10To14MTop10To30vsSBRaiseOpps() ? 1 : 0;
	data.numDefendBb10To14MTop30To50vsBtnRaise += cols.isDefendBb10To14MTop30To50vsBtnRaise() ? 1 : 0;
	data.numDefendBb10To14MTop30To50vsBtnRaiseOpps += cols.isDefendBb10To14MTop30To50vsBtnRaiseOpps() ? 1 : 0;
	data.numDefendBb10To14MTop30To50vsCoMpEpRaise += cols.isDefendBb10To14MTop30To50vsCoMpEpRaise() ? 1 : 0;
	data.numDefendBb10To14MTop30To50vsCoMpEpRaiseOpps += cols.isDefendBb10To14MTop30To50vsCoMpEpRaiseOpps() ? 1 : 0;
	data.numDefendBb10To14MTop30To50vsSBRaise += cols.isDefendBb10To14MTop30To50vsSBRaise() ? 1 : 0;
	data.numDefendBb10To14MTop30To50vsSBRaiseOpps += cols.isDefendBb10To14MTop30To50vsSBRaiseOpps() ? 1 : 0;
	data.numDefendBb14PlusMBot50vsBtnRaise += cols.isDefendBb14PlusMBot50vsBtnRaise() ? 1 : 0;
	data.numDefendBb14PlusMBot50vsBtnRaiseOpps += cols.isDefendBb14PlusMBot50vsBtnRaiseOpps() ? 1 : 0;
	data.numDefendBb14PlusMTop10vsMpEpRaise += cols.isDefendBb14PlusMTop10vsMpEpRaise() ? 1 : 0;
	data.numDefendBb14PlusMTop10vsMpEpRaiseOpps += cols.isDefendBb14PlusMTop10vsMpEpRaiseOpps() ? 1 : 0;
	data.numDefendBb14PlusMTop10To30vsCoMpEpRaise += cols.isDefendBb14PlusMTop10To30vsCoMpEpRaise() ? 1 : 0;
	data.numDefendBb14PlusMTop10To30vsCoMpEpRaiseOpps += cols.isDefendBb14PlusMTop10To30vsCoMpEpRaiseOpps() ? 1 : 0;
	data.numDefendBb14PlusMTop10To30vsSBRaise += cols.isDefendBb14PlusMTop10To30vsSBRaise() ? 1 : 0;
	data.numDefendBb14PlusMTop10To30vsSBRaiseOpps += cols.isDefendBb14PlusMTop10To30vsSBRaiseOpps() ? 1 : 0;
	data.numDefendBb14PlusMTop10To50vsBtnRaise += cols.isDefendBb14PlusMTop10To50vsBtnRaise() ? 1 : 0;
	data.numDefendBb14PlusMTop10To50vsBtnRaiseOpps += cols.isDefendBb14PlusMTop10To50vsBtnRaiseOpps() ? 1 : 0;
	data.numDefendBb14PlusMTop30To100vsSBRaise += cols.isDefendBb14PlusMTop30To100vsSBRaise() ? 1 : 0;
	data.numDefendBb14PlusMTop30To100vsSBRaiseOpps += cols.isDefendBb14PlusMTop30To100vsSBRaiseOpps() ? 1 : 0;
	data.numDefendBb14PlusMTop30To50vsCoMpEpRaise += cols.isDefendBb14PlusMTop30To50vsCoMpEpRaise() ? 1 : 0;
	data.numDefendBb14PlusMTop30To50vsCoMpEpRaiseOpps += cols.isDefendBb14PlusMTop30To50vsCoMpEpRaiseOpps() ? 1 : 0;
	data.numDefendBb4To7MBot50vsSBRaise += cols.isDefendBb4To7MBot50vsSBRaise() ? 1 : 0;
	data.numDefendBb4To7MBot50vsSBRaiseOpps += cols.isDefendBb4To7MBot50vsSBRaiseOpps() ? 1 : 0;
	data.numDefendBb4To7MTop10To30vsBtnRaise += cols.isDefendBb4To7MTop10To30vsBtnRaise() ? 1 : 0;
	data.numDefendBb4To7MTop10To30vsBtnRaiseOpps += cols.isDefendBb4To7MTop10To30vsBtnRaiseOpps() ? 1 : 0;
	data.numDefendBb4To7MTop10To30vsCoRaise += cols.isDefendBb4To7MTop10To30vsCoRaise() ? 1 : 0;
	data.numDefendBb4To7MTop10To30vsCoRaiseOpps += cols.isDefendBb4To7MTop10To30vsCoRaiseOpps() ? 1 : 0;
	data.numDefendBb4To7MTop10To30vsMpEpRaise += cols.isDefendBb4To7MTop10To30vsMpEpRaise() ? 1 : 0;
	data.numDefendBb4To7MTop10To30vsMpEpRaiseOpps += cols.isDefendBb4To7MTop10To30vsMpEpRaiseOpps() ? 1 : 0;
	data.numDefendBb4To7MTop10To30vsSBRaise += cols.isDefendBb4To7MTop10To30vsSBRaise() ? 1 : 0;
	data.numDefendBb4To7MTop10To30vsSBRaiseOpps += cols.isDefendBb4To7MTop10To30vsSBRaiseOpps() ? 1 : 0;
	data.numDefendBb4To7MTop30To50vsBtnRaise += cols.isDefendBb4To7MTop30To50vsBtnRaise() ? 1 : 0;
	data.numDefendBb4To7MTop30To50vsBtnRaiseOpps += cols.isDefendBb4To7MTop30To50vsBtnRaiseOpps() ? 1 : 0;
	data.numDefendBb4To7MTop30To50vsCoMpEpRaise += cols.isDefendBb4To7MTop30To50vsCoMpEpRaise() ? 1 : 0;
	data.numDefendBb4To7MTop30To50vsCoMpEpRaiseOpps += cols.isDefendBb4To7MTop30To50vsCoMpEpRaiseOpps() ? 1 : 0;
	data.numDefendBb4To7MTop30To50vsSBRaise += cols.isDefendBb4To7MTop30To50vsSBRaise() ? 1 : 0;
	data.numDefendBb4To7MTop30To50vsSBRaiseOpps += cols.isDefendBb4To7MTop30To50vsSBRaiseOpps() ? 1 : 0;
	data.numDefendBb7To10MBot50vsCoMpEpRaise += cols.isDefendBb7To10MBot50vsCoMpEpRaise() ? 1 : 0;
	data.numDefendBb7To10MBot50vsCoMpEpRaiseOpps += cols.isDefendBb7To10MBot50vsCoMpEpRaiseOpps() ? 1 : 0;
	data.numDefendBb7To10MTop10To30vsBtnRaise += cols.isDefendBb7To10MTop10To30vsBtnRaise() ? 1 : 0;
	data.numDefendBb7To10MTop10To30vsBtnRaiseOpps += cols.isDefendBb7To10MTop10To30vsBtnRaiseOpps() ? 1 : 0;
	data.numDefendBb7To10MTop10To30vsCoRaise += cols.isDefendBb7To10MTop10To30vsCoRaise() ? 1 : 0;
	data.numDefendBb7To10MTop10To30vsCoRaiseOpps += cols.isDefendBb7To10MTop10To30vsCoRaiseOpps() ? 1 : 0;
	data.numDefendBb7To10MTop10To30vsMpEpRaise += cols.isDefendBb7To10MTop10To30vsMpEpRaise() ? 1 : 0;
	data.numDefendBb7To10MTop10To30vsMpEpRaiseOpps += cols.isDefendBb7To10MTop10To30vsMpEpRaiseOpps() ? 1 : 0;
	data.numDefendBb7To10MTop10To30vsSBRaise += cols.isDefendBb7To10MTop10To30vsSBRaise() ? 1 : 0;
	data.numDefendBb7To10MTop10To30vsSBRaiseOpps += cols.isDefendBb7To10MTop10To30vsSBRaiseOpps() ? 1 : 0;
	data.numDefendBb7To10MTop30To50vsBtnRaise += cols.isDefendBb7To10MTop30To50vsBtnRaise() ? 1 : 0;
	data.numDefendBb7To10MTop30To50vsBtnRaiseOpps += cols.isDefendBb7To10MTop30To50vsBtnRaiseOpps() ? 1 : 0;
	data.numDefendBb7To10MTop30To50vsCoMpEpRaise += cols.isDefendBb7To10MTop30To50vsCoMpEpRaise() ? 1 : 0;
	data.numDefendBb7To10MTop30To50vsCoMpEpRaiseOpps += cols.isDefendBb7To10MTop30To50vsCoMpEpRaiseOpps() ? 1 : 0;
	data.numDefendBb7To10MTop30To50vsSBRaise += cols.isDefendBb7To10MTop30To50vsSBRaise() ? 1 : 0;
	data.numDefendBb7To10MTop30To50vsSBRaiseOpps += cols.isDefendBb7To10MTop30To50vsSBRaiseOpps() ? 1 : 0;
	data.numDefendBtn14PlusMTop10vsCoMpEpRaise += cols.isDefendBtn14PlusMTop10vsCoMpEpRaise() ? 1 : 0;
	data.numDefendBtn14PlusMTop10vsCoMpEpRaiseOpps += cols.isDefendBtn14PlusMTop10vsCoMpEpRaiseOpps() ? 1 : 0;
	data.numDefendBtn14PlusMTop10To30vsCoMpEpRaise += cols.isDefendBtn14PlusMTop10To30vsCoMpEpRaise() ? 1 : 0;
	data.numDefendBtn14PlusMTop10To30vsCoMpEpRaiseOpps += cols.isDefendBtn14PlusMTop10To30vsCoMpEpRaiseOpps() ? 1 : 0;
	data.numDefendSb0To4MTop10vsCoMpEpRaise += cols.isDefendSb0To4MTop10vsCoMpEpRaise() ? 1 : 0;
	data.numDefendSb0To4MTop10vsCoMpEpRaiseOpps += cols.isDefendSb0To4MTop10vsCoMpEpRaiseOpps() ? 1 : 0;
	data.numDefendSb0To4MTop10To30vsCoMpEpRaise += cols.isDefendSb0To4MTop10To30vsCoMpEpRaise() ? 1 : 0;
	data.numDefendSb0To4MTop10To30vsCoMpEpRaiseOpps += cols.isDefendSb0To4MTop10To30vsCoMpEpRaiseOpps() ? 1 : 0;
	data.numDefendSb10To14MTop10vsCoRaise += cols.isDefendSb10To14MTop10vsCoRaise() ? 1 : 0;
	data.numDefendSb10To14MTop10vsCoRaiseOpps += cols.isDefendSb10To14MTop10vsCoRaiseOpps() ? 1 : 0;
	data.numDefendSb10To14MTop10vsMpEpRaise += cols.isDefendSb10To14MTop10vsMpEpRaise() ? 1 : 0;
	data.numDefendSb10To14MTop10vsMpEpRaiseOpps += cols.isDefendSb10To14MTop10vsMpEpRaiseOpps() ? 1 : 0;
	data.numDefendSb10To14MTop10To30vsCoRaise += cols.isDefendSb10To14MTop10To30vsCoRaise() ? 1 : 0;
	data.numDefendSb10To14MTop10To30vsCoRaiseOpps += cols.isDefendSb10To14MTop10To30vsCoRaiseOpps() ? 1 : 0;
	data.numDefendSb10To14MTop10To30vsMpEpRaise += cols.isDefendSb10To14MTop10To30vsMpEpRaise() ? 1 : 0;
	data.numDefendSb10To14MTop10To30vsMpEpRaiseOpps += cols.isDefendSb10To14MTop10To30vsMpEpRaiseOpps() ? 1 : 0;
	data.numDefendSb14PlusMTop10vsCoMpEpRaise += cols.isDefendSb14PlusMTop10vsCoMpEpRaise() ? 1 : 0;
	data.numDefendSb14PlusMTop10vsCoMpEpRaiseOpps += cols.isDefendSb14PlusMTop10vsCoMpEpRaiseOpps() ? 1 : 0;
	data.numDefendSb14PlusMTop10To30vsBtnRaise += cols.isDefendSb14PlusMTop10To30vsBtnRaise() ? 1 : 0;
	data.numDefendSb14PlusMTop10To30vsBtnRaiseOpps += cols.isDefendSb14PlusMTop10To30vsBtnRaiseOpps() ? 1 : 0;
	data.numDefendSb14PlusMTop10To30vsCoMpEpRaise += cols.isDefendSb14PlusMTop10To30vsCoMpEpRaise() ? 1 : 0;
	data.numDefendSb14PlusMTop10To30vsCoMpEpRaiseOpps += cols.isDefendSb14PlusMTop10To30vsCoMpEpRaiseOpps() ? 1 : 0;
	data.numDefendSb14PlusMTop30To50vsBtnRaise += cols.isDefendSb14PlusMTop30To50vsBtnRaise() ? 1 : 0;
	data.numDefendSb14PlusMTop30To50vsBtnRaiseOpps += cols.isDefendSb14PlusMTop30To50vsBtnRaiseOpps() ? 1 : 0;
	data.numDefendSb14PlusMTop30To50vsCoMpEpRaise += cols.isDefendSb14PlusMTop30To50vsCoMpEpRaise() ? 1 : 0;
	data.numDefendSb14PlusMTop30To50vsCoMpEpRaiseOpps += cols.isDefendSb14PlusMTop30To50vsCoMpEpRaiseOpps() ? 1 : 0;
	data.numDefendSb4To7MTop10vsCoMpEpRaise += cols.isDefendSb4To7MTop10vsCoMpEpRaise() ? 1 : 0;
	data.numDefendSb4To7MTop10vsCoMpEpRaiseOpps += cols.isDefendSb4To7MTop10vsCoMpEpRaiseOpps() ? 1 : 0;
	data.numDefendSb4To7MTop10To30vsBtnRaise += cols.isDefendSb4To7MTop10To30vsBtnRaise() ? 1 : 0;
	data.numDefendSb4To7MTop10To30vsBtnRaiseOpps += cols.isDefendSb4To7MTop10To30vsBtnRaiseOpps() ? 1 : 0;
	data.numDefendSb4To7MTop10To30vsCoMpEpRaise += cols.isDefendSb4To7MTop10To30vsCoMpEpRaise() ? 1 : 0;
	data.numDefendSb4To7MTop10To30vsCoMpEpRaiseOpps += cols.isDefendSb4To7MTop10To30vsCoMpEpRaiseOpps() ? 1 : 0;
	data.numDefendSb4To7MTop30To50vsBtnRaise += cols.isDefendSb4To7MTop30To50vsBtnRaise() ? 1 : 0;
	data.numDefendSb4To7MTop30To50vsBtnRaiseOpps += cols.isDefendSb4To7MTop30To50vsBtnRaiseOpps() ? 1 : 0;
	data.numDefendSb7To10MTop10vsCoRaise += cols.isDefendSb7To10MTop10vsCoRaise() ? 1 : 0;
	data.numDefendSb7To10MTop10vsCoRaiseOpps += cols.isDefendSb7To10MTop10vsCoRaiseOpps() ? 1 : 0;
	data.numDefendSb7To10MTop10vsMpEpRaise += cols.isDefendSb7To10MTop10vsMpEpRaise() ? 1 : 0;
	data.numDefendSb7To10MTop10vsMpEpRaiseOpps += cols.isDefendSb7To10MTop10vsMpEpRaiseOpps() ? 1 : 0;
	data.numDefendSb7To10MTop10To30vsCoRaise += cols.isDefendSb7To10MTop10To30vsCoRaise() ? 1 : 0;
	data.numDefendSb7To10MTop10To30vsCoRaiseOpps += cols.isDefendSb7To10MTop10To30vsCoRaiseOpps() ? 1 : 0;
	data.numDefendSb7To10MTop10To30vsMpEpRaise += cols.isDefendSb7To10MTop10To30vsMpEpRaise() ? 1 : 0;
	data.numDefendSb7To10MTop10To30vsMpEpRaiseOpps += cols.isDefendSb7To10MTop10To30vsMpEpRaiseOpps() ? 1 : 0;
	data.numBetSizePreflopTotal += cols.isBetSizePreflopTotal() ? 1 : 0;
	data.numBetSizePreflop2To2_05BB += cols.isBetSizePreflop2To2_05BB() ? 1 : 0;
	data.numBetSizePreflop2_05To3BB += cols.isBetSizePreflop2_05To3BB() ? 1 : 0;
	data.numBetSizePreflop3To5BB += cols.isBetSizePreflop3To5BB() ? 1 : 0;
	data.numBetSizePreflop5PlusBB += cols.isBetSizePreflop5PlusBB() ? 1 : 0;
	data.perRoundData[0].numBetSize0To40OnRound += cols.isBetSize0To40OnFlop() ? 1 : 0;
	data.perRoundData[1].numBetSize0To40OnRound += cols.isBetSize0To40OnTurn() ? 1 : 0;
	data.perRoundData[2].numBetSize0To40OnRound += cols.isBetSize0To40OnRiver() ? 1 : 0;
	data.perRoundData[3].numBetSize0To40OnRound += cols.isBetSize0To40On7th() ? 1 : 0;
	data.perRoundData[0].numBetSize40To49_5OnRound += cols.isBetSize40To49_5OnFlop() ? 1 : 0;
	data.perRoundData[1].numBetSize40To49_5OnRound += cols.isBetSize40To49_5OnTurn() ? 1 : 0;
	data.perRoundData[2].numBetSize40To49_5OnRound += cols.isBetSize40To49_5OnRiver() ? 1 : 0;
	data.perRoundData[3].numBetSize40To49_5OnRound += cols.isBetSize40To49_5On7th() ? 1 : 0;
	data.perRoundData[0].numBetSize49_5To50_5OnRound += cols.isBetSize49_5To50_5OnFlop() ? 1 : 0;
	data.perRoundData[1].numBetSize49_5To50_5OnRound += cols.isBetSize49_5To50_5OnTurn() ? 1 : 0;
	data.perRoundData[2].numBetSize49_5To50_5OnRound += cols.isBetSize49_5To50_5OnRiver() ? 1 : 0;
	data.perRoundData[3].numBetSize49_5To50_5OnRound += cols.isBetSize49_5To50_5On7th() ? 1 : 0;
	data.perRoundData[0].numBetSize50_5To60OnRound += cols.isBetSize50_5To60OnFlop() ? 1 : 0;
	data.perRoundData[1].numBetSize50_5To60OnRound += cols.isBetSize50_5To60OnTurn() ? 1 : 0;
	data.perRoundData[2].numBetSize50_5To60OnRound += cols.isBetSize50_5To60OnRiver() ? 1 : 0;
	data.perRoundData[3].numBetSize50_5To60OnRound += cols.isBetSize50_5To60On7th() ? 1 : 0;
	data.perRoundData[0].numBetSize60To70OnRound += cols.isBetSize60To70OnFlop() ? 1 : 0;
	data.perRoundData[1].numBetSize60To70OnRound += cols.isBetSize60To70OnTurn() ? 1 : 0;
	data.perRoundData[2].numBetSize60To70OnRound += cols.isBetSize60To70OnRiver() ? 1 : 0;
	data.perRoundData[3].numBetSize60To70OnRound += cols.isBetSize60To70On7th() ? 1 : 0;
	data.perRoundData[0].numBetSize70To99_5OnRound += cols.isBetSize70To99_5OnFlop() ? 1 : 0;
	data.perRoundData[1].numBetSize70To99_5OnRound += cols.isBetSize70To99_5OnTurn() ? 1 : 0;
	data.perRoundData[2].numBetSize70To99_5OnRound += cols.isBetSize70To99_5OnRiver() ? 1 : 0;
	data.perRoundData[3].numBetSize70To99_5OnRound += cols.isBetSize70To99_5On7th() ? 1 : 0;
	data.perRoundData[0].numBetSize99_5To100_5OnRound += cols.isBetSize99_5To100_5OnFlop() ? 1 : 0;
	data.perRoundData[1].numBetSize99_5To100_5OnRound += cols.isBetSize99_5To100_5OnTurn() ? 1 : 0;
	data.perRoundData[2].numBetSize99_5To100_5OnRound += cols.isBetSize99_5To100_5OnRiver() ? 1 : 0;
	data.perRoundData[3].numBetSize99_5To100_5OnRound += cols.isBetSize99_5To100_5On7th() ? 1 : 0;
	data.perRoundData[0].numBetSize100_5PlusOnRound += cols.isBetSize100_5PlusOnFlop() ? 1 : 0;
	data.perRoundData[1].numBetSize100_5PlusOnRound += cols.isBetSize100_5PlusOnTurn() ? 1 : 0;
	data.perRoundData[2].numBetSize100_5PlusOnRound += cols.isBetSize100_5PlusOnRiver() ? 1 : 0;
	data.perRoundData[3].numBetSize100_5PlusOnRound += cols.isBetSize100_5PlusOn7th() ? 1 : 0;
	data.numPfrBbHuLimped14PlusMRaiseTo2To3BB += cols.isPfrBbHuLimped14PlusMRaiseTo2To3BB() ? 1 : 0;
	data.numPfrBbHuLimped14PlusMRaiseTo3To3_5BB += cols.isPfrBbHuLimped14PlusMRaiseTo3To3_5BB() ? 1 : 0;
	data.numPfrBbHuLimped14PlusMRaiseTo3_5To4BB += cols.isPfrBbHuLimped14PlusMRaiseTo3_5To4BB() ? 1 : 0;
	data.numPfrBbHuLimped14PlusMRaiseTo4To4_5BB += cols.isPfrBbHuLimped14PlusMRaiseTo4To4_5BB() ? 1 : 0;
	data.numPfrBbHuLimped14PlusMRaiseTo4_5To5BB += cols.isPfrBbHuLimped14PlusMRaiseTo4_5To5BB() ? 1 : 0;
	data.numPfrBbHuLimped14PlusMRaiseTo5To6BB += cols.isPfrBbHuLimped14PlusMRaiseTo5To6BB() ? 1 : 0;
	data.numPfrBbHuLimped14PlusMRaiseTo6To7BB += cols.isPfrBbHuLimped14PlusMRaiseTo6To7BB() ? 1 : 0;
	data.numPfrBbHuLimped14PlusMRaiseTo7To10BB += cols.isPfrBbHuLimped14PlusMRaiseTo7To10BB() ? 1 : 0;
	data.numPfrBbHuLimped14PlusMRaiseTo10PlusBB += cols.isPfrBbHuLimped14PlusMRaiseTo10PlusBB() ? 1 : 0;
	data.numPfrBbHuMicroRaised14PlusMRaiseTo3To5BB += cols.isPfrBbHuMicroRaised14PlusMRaiseTo3To5BB() ? 1 : 0;
	data.numPfrBbHuMicroRaised14PlusMRaiseTo5To6BB += cols.isPfrBbHuMicroRaised14PlusMRaiseTo5To6BB() ? 1 : 0;
	data.numPfrBbHuMicroRaised14PlusMRaiseTo6To7BB += cols.isPfrBbHuMicroRaised14PlusMRaiseTo6To7BB() ? 1 : 0;
	data.numPfrBbHuMicroRaised14PlusMRaiseTo7To10BB += cols.isPfrBbHuMicroRaised14PlusMRaiseTo7To10BB() ? 1 : 0;
	data.numPfrBbHuMicroRaised14PlusMRaiseTo10PlusBB += cols.isPfrBbHuMicroRaised14PlusMRaiseTo10PlusBB() ? 1 : 0;
	data.numPfrBbHuLimped7To10MRaiseTo2To3BB += cols.isPfrBbHuLimped7To10MRaiseTo2To3BB() ? 1 : 0;
	data.numPfrBbHuLimped7To10MRaiseTo3To3_5BB += cols.isPfrBbHuLimped7To10MRaiseTo3To3_5BB() ? 1 : 0;
	data.numPfrBbHuLimped7To10MRaiseTo3_5To4BB += cols.isPfrBbHuLimped7To10MRaiseTo3_5To4BB() ? 1 : 0;
	data.numPfrBbHuLimped7To10MRaiseTo4To4_5BB += cols.isPfrBbHuLimped7To10MRaiseTo4To4_5BB() ? 1 : 0;
	data.numPfrBbHuLimped7To10MRaiseTo4_5To5BB += cols.isPfrBbHuLimped7To10MRaiseTo4_5To5BB() ? 1 : 0;
	data.numPfrBbHuLimped7To10MRaiseTo5To6BB += cols.isPfrBbHuLimped7To10MRaiseTo5To6BB() ? 1 : 0;
	data.numPfrBbHuLimped7To10MRaiseTo6To10BB += cols.isPfrBbHuLimped7To10MRaiseTo6To10BB() ? 1 : 0;
	data.numPfrBbHuLimped7To10MRaiseTo10PlusBB += cols.isPfrBbHuLimped7To10MRaiseTo10PlusBB() ? 1 : 0;
	data.numFoldTo3BetHEM_Btn10To14MTop10Opps += cols.isFoldTo3BetHEM_Btn10To14MTop10Opps() ? 1 : 0;
	data.numFoldTo3BetHEM_Btn10To14MTop10To30Opps += cols.isFoldTo3BetHEM_Btn10To14MTop10To30Opps() ? 1 : 0;
	data.numFoldTo3BetHEM_Btn10To14MTop30To50Opps += cols.isFoldTo3BetHEM_Btn10To14MTop30To50Opps() ? 1 : 0;
	data.numFoldTo3BetHEM_Btn14PlusMTop10Opps += cols.isFoldTo3BetHEM_Btn14PlusMTop10Opps() ? 1 : 0;
	data.numFoldTo3BetHEM_Btn14PlusMTop10To30Opps += cols.isFoldTo3BetHEM_Btn14PlusMTop10To30Opps() ? 1 : 0;
	data.numFoldTo3BetHEM_Btn7To10MTop30Opps += cols.isFoldTo3BetHEM_Btn7To10MTop30Opps() ? 1 : 0;
	data.numFoldTo3BetHEM_Co14PlusMTop10Opps += cols.isFoldTo3BetHEM_Co14PlusMTop10Opps() ? 1 : 0;
	data.numFoldTo3BetHEM_Co7To14MTop10Opps += cols.isFoldTo3BetHEM_Co7To14MTop10Opps() ? 1 : 0;
	data.numFoldTo3BetHEM_Co7To14MTop10To30Opps += cols.isFoldTo3BetHEM_Co7To14MTop10To30Opps() ? 1 : 0;
	data.numFoldTo3BetHEM_CoMpEp14PlusMTop10To30Opps += cols.isFoldTo3BetHEM_CoMpEp14PlusMTop10To30Opps() ? 1 : 0;
	data.numFoldTo3BetHEM_Ep7To14MTop10Opps += cols.isFoldTo3BetHEM_Ep7To14MTop10Opps() ? 1 : 0;
	data.numFoldTo3BetHEM_Ep7To14MTop10To30Opps += cols.isFoldTo3BetHEM_Ep7To14MTop10To30Opps() ? 1 : 0;
	data.numFoldTo3BetHEM_Mp7To14MTop10Opps += cols.isFoldTo3BetHEM_Mp7To14MTop10Opps() ? 1 : 0;
	data.numFoldTo3BetHEM_Mp7To14MTop10To30Opps += cols.isFoldTo3BetHEM_Mp7To14MTop10To30Opps() ? 1 : 0;
	data.numFoldTo3BetHEM_MpEp14PlusMTop10Opps += cols.isFoldTo3BetHEM_MpEp14PlusMTop10Opps() ? 1 : 0;
	data.numFoldTo3BetHEM_Sb7To14MTop10Opps += cols.isFoldTo3BetHEM_Sb7To14MTop10Opps() ? 1 : 0;
	data.numFoldTo3BetHEM_Sb7To14MTop10To30Opps += cols.isFoldTo3BetHEM_Sb7To14MTop10To30Opps() ? 1 : 0;
	data.numFoldTo3BetHEM_SbHu10To14MTop10To30Opps += cols.isFoldTo3BetHEM_SbHu10To14MTop10To30Opps() ? 1 : 0;
	data.numFoldTo3BetHEM_SbHu10To14MTop30To50Opps += cols.isFoldTo3BetHEM_SbHu10To14MTop30To50Opps() ? 1 : 0;
	data.numFoldTo3BetHEM_SbHu14PlusMBot50Opps += cols.isFoldTo3BetHEM_SbHu14PlusMBot50Opps() ? 1 : 0;
	data.numFoldTo3BetHEM_SbHu14PlusMTop10To30Opps += cols.isFoldTo3BetHEM_SbHu14PlusMTop10To30Opps() ? 1 : 0;
	data.numFoldTo3BetHEM_SbHu14PlusMTop30To50Opps += cols.isFoldTo3BetHEM_SbHu14PlusMTop30To50Opps() ? 1 : 0;
	data.numFoldTo3BetHEM_SbHu7To10MTop10To50Opps += cols.isFoldTo3BetHEM_SbHu7To10MTop10To50Opps() ? 1 : 0;
	data.numFoldTo3BetHEM_Btn10To14MTop10 += cols.isFoldTo3BetHEM_Btn10To14MTop10() ? 1 : 0;
	data.numFoldTo3BetHEM_Btn10To14MTop10To30 += cols.isFoldTo3BetHEM_Btn10To14MTop10To30() ? 1 : 0;
	data.numFoldTo3BetHEM_Btn10To14MTop30To50 += cols.isFoldTo3BetHEM_Btn10To14MTop30To50() ? 1 : 0;
	data.numFoldTo3BetHEM_Btn14PlusMTop10 += cols.isFoldTo3BetHEM_Btn14PlusMTop10() ? 1 : 0;
	data.numFoldTo3BetHEM_Btn14PlusMTop10To30 += cols.isFoldTo3BetHEM_Btn14PlusMTop10To30() ? 1 : 0;
	data.numFoldTo3BetHEM_Btn7To10MTop30 += cols.isFoldTo3BetHEM_Btn7To10MTop30() ? 1 : 0;
	data.numFoldTo3BetHEM_Co14PlusMTop10 += cols.isFoldTo3BetHEM_Co14PlusMTop10() ? 1 : 0;
	data.numFoldTo3BetHEM_Co7To14MTop10 += cols.isFoldTo3BetHEM_Co7To14MTop10() ? 1 : 0;
	data.numFoldTo3BetHEM_Co7To14MTop10To30 += cols.isFoldTo3BetHEM_Co7To14MTop10To30() ? 1 : 0;
	data.numFoldTo3BetHEM_CoMpEp14PlusMTop10To30 += cols.isFoldTo3BetHEM_CoMpEp14PlusMTop10To30() ? 1 : 0;
	data.numFoldTo3BetHEM_Ep7To14MTop10 += cols.isFoldTo3BetHEM_Ep7To14MTop10() ? 1 : 0;
	data.numFoldTo3BetHEM_Ep7To14MTop10To30 += cols.isFoldTo3BetHEM_Ep7To14MTop10To30() ? 1 : 0;
	data.numFoldTo3BetHEM_Mp7To14MTop10 += cols.isFoldTo3BetHEM_Mp7To14MTop10() ? 1 : 0;
	data.numFoldTo3BetHEM_Mp7To14MTop10To30 += cols.isFoldTo3BetHEM_Mp7To14MTop10To30() ? 1 : 0;
	data.numFoldTo3BetHEM_MpEp14PlusMTop10 += cols.isFoldTo3BetHEM_MpEp14PlusMTop10() ? 1 : 0;
	data.numFoldTo3BetHEM_Sb7To14MTop10 += cols.isFoldTo3BetHEM_Sb7To14MTop10() ? 1 : 0;
	data.numFoldTo3BetHEM_Sb7To14MTop10To30 += cols.isFoldTo3BetHEM_Sb7To14MTop10To30() ? 1 : 0;
	data.numFoldTo3BetHEM_SbHu10To14MTop10To30 += cols.isFoldTo3BetHEM_SbHu10To14MTop10To30() ? 1 : 0;
	data.numFoldTo3BetHEM_SbHu10To14MTop30To50 += cols.isFoldTo3BetHEM_SbHu10To14MTop30To50() ? 1 : 0;
	data.numFoldTo3BetHEM_SbHu14PlusMBot50 += cols.isFoldTo3BetHEM_SbHu14PlusMBot50() ? 1 : 0;
	data.numFoldTo3BetHEM_SbHu14PlusMTop10To30 += cols.isFoldTo3BetHEM_SbHu14PlusMTop10To30() ? 1 : 0;
	data.numFoldTo3BetHEM_SbHu14PlusMTop30To50 += cols.isFoldTo3BetHEM_SbHu14PlusMTop30To50() ? 1 : 0;
	data.numFoldTo3BetHEM_SbHu7To10MTop10To50 += cols.isFoldTo3BetHEM_SbHu7To10MTop10To50() ? 1 : 0;
	data.numFoldBbHuLimped14PlusM += cols.isFoldBbHuLimped14PlusM() ? 1 : 0;
	data.numCallBbHuLimped14PlusM += cols.isCallBbHuLimped14PlusM() ? 1 : 0;
	data.numHULCPreflopCheckBet1To1_5BBOnFlop14PlusM += cols.isHULCPreflopCheckBet1To1_5BBOnFlop14PlusM() ? 1 : 0;
	data.numHULCPreflopCheckBet1_5To2BBOnFlop14PlusM += cols.isHULCPreflopCheckBet1_5To2BBOnFlop14PlusM() ? 1 : 0;
	data.numHULCPreflopCheckBet2PlusBBOnFlop14PlusM += cols.isHULCPreflopCheckBet2PlusBBOnFlop14PlusM() ? 1 : 0;
	data.numHULCPreflopCheckCheckOnFlop14PlusM += cols.isHULCPreflopCheckCheckOnFlop14PlusM() ? 1 : 0;
	data.numHUMRCalledPreflopCheckBet1To1_5BBOnFlop14PlusM += cols.isHUMRCalledPreflopCheckBet1To1_5BBOnFlop14PlusM() ? 1 : 0;
	data.numHUMRCalledPreflopCheckBet1_5To2BBOnFlop14PlusM += cols.isHUMRCalledPreflopCheckBet1_5To2BBOnFlop14PlusM() ? 1 : 0;
	data.numHUMRCalledPreflopCheckBet2To2_5BBOnFlop14PlusM += cols.isHUMRCalledPreflopCheckBet2To2_5BBOnFlop14PlusM() ? 1 : 0;
	data.numHUMRCalledPreflopCheckBet2_5To3BBOnFlop14PlusM += cols.isHUMRCalledPreflopCheckBet2_5To3BBOnFlop14PlusM() ? 1 : 0;
	data.numHUMRCalledPreflopCheckBet3To3_5BBOnFlop14PlusM += cols.isHUMRCalledPreflopCheckBet3To3_5BBOnFlop14PlusM() ? 1 : 0;
	data.numHUMRCalledPreflopCheckBet3_5PlusBBOnFlop14PlusM += cols.isHUMRCalledPreflopCheckBet3_5PlusBBOnFlop14PlusM() ? 1 : 0;
	data.numHUMRCalledPreflopCheckCheckOnFlop14PlusM += cols.isHUMRCalledPreflopCheckCheckOnFlop14PlusM() ? 1 : 0;

	data.numPfrBbHuLimped10To14MRaiseTo2To3BB += cols.isPfrBbHuLimped10To14MRaiseTo2To3BB() ? 1 : 0;
	data.numPfrBbHuLimped10To14MRaiseTo3To3_5BB += cols.isPfrBbHuLimped10To14MRaiseTo3To3_5BB() ? 1 : 0;
	data.numPfrBbHuLimped10To14MRaiseTo3_5To4BB += cols.isPfrBbHuLimped10To14MRaiseTo3_5To4BB() ? 1 : 0;
	data.numPfrBbHuLimped10To14MRaiseTo4To4_5BB += cols.isPfrBbHuLimped10To14MRaiseTo4To4_5BB() ? 1 : 0;
	data.numPfrBbHuLimped10To14MRaiseTo4_5To5BB += cols.isPfrBbHuLimped10To14MRaiseTo4_5To5BB() ? 1 : 0;
	data.numPfrBbHuLimped10To14MRaiseTo5To6BB += cols.isPfrBbHuLimped10To14MRaiseTo5To6BB() ? 1 : 0;
	data.numPfrBbHuLimped10To14MRaiseTo6To10BB += cols.isPfrBbHuLimped10To14MRaiseTo6To10BB() ? 1 : 0;
	data.numPfrBbHuLimped10To14MRaiseTo10PlusBB += cols.isPfrBbHuLimped10To14MRaiseTo10PlusBB() ? 1 : 0;
	data.numPfrBbHuLimped14PlusMTop10 += cols.isPfrBbHuLimped14PlusMTop10() ? 1 : 0;
	data.numPfrBbHuLimped14PlusMTop10Opps += cols.isPfrBbHuLimped14PlusMTop10Opps() ? 1 : 0;
}


void HandStatsManager::aggregateConsistency(ConsistencyData& data) const
{ //lint --e{737} loss of sign
	++data.numHands;
	data.numVpip += cols.numVpip.value;
	data.numHandsRaisedPreflop += cols.numHandsRaisedPreflop.value;
	data.numColdCallOpportunities += cols.numColdCallOpportunities.value;
	data.numColdCalls += cols.numColdCalls.value;
}


UINT32 HandStatsManager::aggregateHandPartnerData(HandPartnersMap& handPartners) const
{
	UINT32 i;
	for (i = 0; i < MAX_TABLE_PLAYERS; ++i)
	{
		char const* partnerName = cols.partnerNames[i].c_str();
		if (!*partnerName)
			break;

		HandPartnerData& data = handPartners.getData(partnerName);

		++data.numHandsWithPartner;
		data.totalProfitWithPartner += cols.totalWin.value;

		INT32 profit = cols.partnerProfit[i].value;
		if (profit)
		{
			++data.numHandsProfitFromPartner;
			data.totalProfitFromPartner += profit;
		}

		data.allInConfrontations += (cols.allInConfrontations.value >> i) & 1; //lint !e702
		data.showdownAllInConfrontations += (cols.showdownAllInConfrontations.value >> i) & 1; //lint !e702
		data.hostId = cols.hostId.value;
	}

	return i + 1; // Number of players in hand
}


void HandStatsManager::aggregateTournPartnerData(TournPartnerDataMap& tournPartners) const
{
	for (UINT32 i = 0; i < MAX_TABLE_PLAYERS; ++i)
	{
		char const* partnerName = cols.partnerNames[i].c_str();
		if (!*partnerName)
			break;

		TournPartnerData& data = tournPartners.getData(partnerName);

		data.allInConfrontations += (cols.allInConfrontations.value >> i) & 1; //lint !e702
		data.showdownAllInConfrontations += (cols.showdownAllInConfrontations.value >> i) & 1; //lint !e702
	}
}


void HandStatsManager::getBaseSessionKey(SessionKey& key) const
{
	key.sessionType = cols.sessionType.value;
	key.userId = cols.userId.c_str();
	key.structure = cols.structure.value;
	key.currency = cols.currency.c_str();
	key.maxPerTable = cols.maxPerTable.value;
	key.hostId = cols.hostId.value;
}


void HandStatsManager::getTournsSessionKey(TournsSessionKey& key) const
{
	getBaseSessionKey(key);
	key.tournGameType = cols.tournGameType.value;
	key.buyIn = cols.buyIn.value;
	key.tournSchedulingType = cols.tournSchedulingType.value;
	key.tournMaxEntry = cols.tournMaxEntry.value;
	key.flags = cols.flags.value;
	key.tournActualEntries = cols.tournActualEntries.value;
}


void HandStatsManager::getHandsSessionKey(HandsSessionKey& key) const
{
	getBaseSessionKey(key);
	key.gameType = cols.gameType.value;
	key.bigBlind = cols.bigBlind.value;
	key.smallBlind = cols.smallBlind.value;
}


void HandStatsManager::getConsistencyKey(ConsistencyKey& key) const
{
	key.currency = cols.currency.c_str();
	key.maxPerTable = cols.maxPerTable.value;
	key.gameType = cols.gameType.value;
	key.structure = cols.structure.value;
	key.bigBlind = cols.bigBlind.value;
	key.position = cols.ptPosition.value;
	key.holeCards = cols.ptHoleCards.c_str();
}


char const* HandStatsManager::formatDeferredHand(PString& str)
{
	SQL_TIMESTAMP_STRUCT const& ts = cols.finished.value;
	char finished[29]; // "YYYY-MM-DD-hh.mm.ss.ffffff"
	::sprintf(finished, "\"%04.4d-%02.2u-%02.2u-%02.2u.%02.2u.%02.2u.%06.6lu\"",
			  ts.year, ts.month, ts.day, ts.hour, ts.minute, ts.second, ts.fraction);

	ColUtils::dbOutQuote(cols.userId.c_str(), str).append(',')
		.appendUint64(cols.handId.value).append(',')
		.append(finished).append(',')
		.appendUint(cols.tournamentId.getValue()).append(',')
		.appendUint(cols.gameType.value).append(',')
		.appendUint(cols.structure.value).append(',')
		.appendUint(cols.bigBlind.value).append(',')
		.appendUint(cols.smallBlind.value).append(',')
		.append('"').append(cols.currency.c_str()).append("\",")
		.appendUint(cols.maxPerTable.value).append(',')
		.appendInt(cols.buyIn.value).append(',')
		.appendInt(cols.totalWin.value).append(',')
		.appendUint(cols.totalBlindsAntesPaid.value).append(',')
		.appendUint(cols.totalRakePaid.value).append(',')
		.appendUint(cols.numBigBlinds.value).append(',')
		.appendUint(cols.numSmallBlinds.value).append(',')
		.appendUint(cols.numBlindsPaidOutOfPosition.value).append(',')
		.appendUint(cols.numVpip.value).append(',')
		.appendUint(cols.numVpipFromSmallBlind.value).append(',')
		.appendUint(cols.numVpipFromBigBlind.value).append(',')
		.appendUint(cols.numSawShowdown.value).append(',')
		.appendUint(cols.numWonShowdown.value).append(',')
		.appendUint(cols.numWonAfterFlop.value).append(',')
		.appendUint(cols.numLimp.value).append(',')
		.appendUint(cols.numLimpRaise.value).append(',')
		.appendUint(cols.numLimpCall.value).append(',')
		.appendUint(cols.numLimpFold.value).append(',')
		.appendUint(cols.numAllinPreflop.value).append(',')
		.appendUint(cols.numColdCallOpportunities.value).append(',')
		.appendUint(cols.numColdCalls.value).append(',')
		.appendUint(cols.num3BetOpportunities.value).append(',')
		.appendUint(cols.num3Bets.value).append(',')
		.appendUint(cols.numFacing3Bet.value).append(',')
		.appendUint(cols.numFoldTo3Bet.value).append(',')
		.appendUint(cols.num4plusBet.value).append(',')
		.appendUint(cols.numSawFlop.value).append(',')
		.appendUint(cols.numSawTurn.value).append(',')
		.appendUint(cols.numSawRiver.value).append(',')
		.appendUint(cols.numSaw7th.value).append(',')
		.appendUint(cols.numFacing4plusBet.value).append(',')
		.appendUint(cols.numFoldTo4plusBet.value).append(',')
		.appendUint(cols.numTurnsPreflop.value).append(',')
		.appendUint(cols.numTurnsFacingBetOnFlop.value).append(',')
		.appendUint(cols.numTurnsFacingNoBetOnFlop.value).append(',')
		.appendUint(cols.numTurnsFacingBetOnTurn.value).append(',')
		.appendUint(cols.numTurnsFacingNoBetOnTurn.value).append(',')
		.appendUint(cols.numTurnsFacingBetOnRiver.value).append(',')
		.appendUint(cols.numTurnsFacingNoBetOnRiver.value).append(',')
		.appendUint(cols.numTurnsFacingBetOn7th.value).append(',')
		.appendUint(cols.numTurnsFacingNoBetOn7th.value).append(',')
		.appendUint(cols.numLastAgressorPreflop.value).append(',')
		.appendUint(cols.numLastAgressorOnFlop.value).append(',')
		.appendUint(cols.numLastAgressorOnTurn.value).append(',')
		.appendUint(cols.numLastAgressorOnRiver.value).append(',')
		.appendUint(cols.numContBetOnFlop.value).append(',')
		.appendUint(cols.numContBetOnTurn.value).append(',')
		.appendUint(cols.numContBetOnRiver.value).append(',')
		.appendUint(cols.numContBetOn7th.value).append(',')
		.appendUint(cols.numBetPreflop.value).append(',')
		.appendUint(cols.numBetOnFlop.value).append(',')
		.appendUint(cols.numBetOnTurn.value).append(',')
		.appendUint(cols.numBetOnRiver.value).append(',')
		.appendUint(cols.numBetOn7th.value).append(',')
		.appendUint(cols.numCheckOnFlop.value).append(',')
		.appendUint(cols.numCheckOnTurn.value).append(',')
		.appendUint(cols.numCheckOnRiver.value).append(',')
		.appendUint(cols.numCheckOn7th.value).append(',')
		.appendUint(cols.numFoldPreflop.value).append(',')
		.appendUint(cols.numFoldOnFlop.value).append(',')
		.appendUint(cols.numFoldOnTurn.value).append(',')
		.appendUint(cols.numFoldOnRiver.value).append(',')
		.appendUint(cols.numFoldOn7th.value).append(',')
		.appendUint(cols.numCallsPreflop.value).append(',')
		.appendUint(cols.numCallsOnFlop.value).append(',')
		.appendUint(cols.numCallsOnTurn.value).append(',')
		.appendUint(cols.numCallsOnRiver.value).append(',')
		.appendUint(cols.numCallsOn7th.value).append(',')
		.appendUint(cols.numRaisesPreflop.value).append(',')
		.appendUint(cols.numRaisesOnFlop.value).append(',')
		.appendUint(cols.numRaisesOnTurn.value).append(',')
		.appendUint(cols.numRaisesOnRiver.value).append(',')
		.appendUint(cols.numRaisesOn7th.value).append(',')
		.appendUint(cols.numCheckCallOnFlop.value).append(',')
		.appendUint(cols.numCheckCallOnTurn.value).append(',')
		.appendUint(cols.numCheckCallOnRiver.value).append(',')
		.appendUint(cols.numCheckCallOn7th.value).append(',')
		.appendUint(cols.numCheckRaiseOnFlop.value).append(',')
		.appendUint(cols.numCheckRaiseOnTurn.value).append(',')
		.appendUint(cols.numCheckRaiseOnRiver.value).append(',')
		.appendUint(cols.numCheckRaiseOn7th.value).append(',')
		.appendUint(cols.numCheckFoldOnFlop.value).append(',')
		.appendUint(cols.numCheckFoldOnTurn.value).append(',')
		.appendUint(cols.numCheckFoldOnRiver.value).append(',')
		.appendUint(cols.numCheckFoldOn7th.value).append(',')
		.appendUint(cols.numOpenFoldPowerfulHand.value).append(',')
		.appendUint(cols.numFoldToExtremeOdds.value).append(',')
		.appendUint(cols.numReraiseWithGarbage.value).append(',')
		.appendUint(cols.numCheckBehindOnRiver.value).append(',')
		.appendUint(cols.numFoldWhenCouldCheck.value).append(',')
		.appendUint(cols.numAttemptStealBlinds.value).append(',')
		.appendUint(cols.numFoldToStealFromSBOpps.value).append(',')
		.appendUint(cols.numFoldToStealFromSB.value).append(',')
		.appendUint(cols.numFoldToStealFromBBOpps.value).append(',')
		.appendUint(cols.numFoldToStealFromBB.value).append(',')
		.appendUint(cols.numCheckRaiseOppsOnFlop.value).append(',')
		.appendUint(cols.numCheckRaiseOppsOnTurn.value).append(',')
		.appendUint(cols.numCheckRaiseOppsOnRiver.value).append(',')
		.appendUint(cols.numCheckRaiseOppsOn7th.value).append(',')
		.appendUint(cols.numAttemptStealBlindsOpps.value).append(',')
		.appendUint(cols.num4BetOpportunities.value).append(',')
		.appendUint(cols.numHandsRaisedPreflop.value).append(',')
		.appendUint(cols.numCallBetsOnlyPreflop.value).append(',')
		.appendUint(cols.numCallBetsOnlyOnFlop.value).append(',')
		.appendUint(cols.numCallBetsOnlyOnTurn.value).append(',')
		.appendUint(cols.numCallBetsOnlyOnRiver.value).append(',')
		.appendUint(cols.numCallBetsOnlyOn7th.value).append(',')
		.appendUint(cols.numTurnsFacingBetOnlyOnFlop.value).append(',')
		.appendUint(cols.numTurnsFacingBetOnlyOnTurn.value).append(',')
		.appendUint(cols.numTurnsFacingBetOnlyOnRiver.value).append(',')
		.appendUint(cols.numTurnsFacingBetOnlyOn7th.value).append(',')
		.appendUint(cols.numFoldToBetOnlyPreflop.value).append(',')
		.appendUint(cols.numFoldToBetOnlyOnFlop.value).append(',')
		.appendUint(cols.numFoldToBetOnlyOnTurn.value).append(',')
		.appendUint(cols.numFoldToBetOnlyOnRiver.value).append(',')
		.appendUint(cols.numFoldToBetOnlyOn7th.value).append(',');

	for (size_t i = 0; i < MAX_TABLE_PLAYERS; ++i)
	{
		ColUtils::dbOutQuote(cols.partnerNames[i].c_str(), str).append(',')
		   .appendUint(cols.partnerProfit[i].value).append(',');
	}

	str.appendUint(cols.tournSchedulingType.value).append(',')
		.appendUint(cols.tournGameType.value).append(',')
		.appendUint(cols.sessionType.value).append(',')
		.appendUint64(cols.sessionId.value).append(',')
		.appendUint(cols.tournMaxEntry.value).append(',')
		.appendUint64(cols.flags.value).append(',')
		.appendUint(cols.allInConfrontations.value).append(',')
		.appendUint(cols.showdownAllInConfrontations.value).append(',')
		.appendUint(cols.ptPosition.value).append(',')
		.append('"').append(cols.ptHoleCards.c_str()).append("\",")
		.appendUint(cols.totalBet.value).append(',')
		.appendUint(cols.aggregationFlags.value).append(',')
		.appendUint(cols.isFacing3BetHEM() ? 1 : 0).append(',')
		.appendUint(cols.isFoldTo3BetHEM() ? 1 : 0).append(',')
		.appendUint(cols.isFacing4PlusBetHEM() ? 1 : 0).append(',')
		.appendUint(cols.isFoldTo4PlusBetHEM() ? 1 : 0).append(',')
		.append("\r\n");

	return str;
}

////////////////////////////////////////////////////////////////////////////////

WriteTournDataStmt::WriteTournDataStmt(DatabaseManagerCommon& man, char const* tournDataTableName) :
	NoExecStmt(man)
{
	PString query;
	query.assign("INSERT INTO ").append(tournDataTableName)
		 .append(" (StartOfPeriod, TournId, UserId, PartnerId, AllInConfrontations, ShowdownAllInConfrontations) "
				 "VALUES (?, ?, ?, ?, ?, ?)");
	prepare(query);

	bindFirstParam(startOfPeriod);
	bindNextParam(tournId);
	bindNextParam(userId);
	bindNextParam(partnerId);
	bindNextParam(allInConfrontations);
	bindNextParam(showdownAllInConfrontations);
}


void WriteTournDataStmt::exec(SrvDate const& startOfPeriod_, UINT32 tournId_,
							  char const* userId_, char const* partnerId_, TournPartnerData const& tournData_)
{
	startOfPeriod = startOfPeriod_;
	tournId = tournId_;
	userId = userId_;
	partnerId = partnerId_;
	allInConfrontations = tournData_.allInConfrontations;
	showdownAllInConfrontations = tournData_.showdownAllInConfrontations;

	execute();
}

////////////////////////////////////////////////////////////////////////////////

ReadCompletedTournsStmt::ReadCompletedTournsStmt(DatabaseManagerCommon& man,
												 SrvDate const& finished,
												 char const* completedTournsTableName) :
	NoExecStmt(man)
{
	PString buf;
	srvDateToString(finished, buf);

	PString query;
	query.assign("SELECT TournamentId FROM ").append(completedTournsTableName)
		 .append(" WHERE Finished BETWEEN '").append(buf).append("' AND '").append(buf).append("-23.59.59.999999'")
		 .append(" WITH UR");

	prepare(query);
	bindFirstCol(tournId);
}


void ReadCompletedTournsStmt::execGet(CompletedTourns& completedTourns)
{
	execute();
	while (fetch())
	{
		completedTourns.add(tournId.getValue());
	}
	closeCursor();

	completedTourns.sort();
}

////////////////////////////////////////////////////////////////////////////////
