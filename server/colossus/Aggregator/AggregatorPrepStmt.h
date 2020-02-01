#ifndef AggregatorPrepStmt_h_included
#define	AggregatorPrepStmt_h_included

////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "dbm.h"
#include "dbmstruct.h"
#include "dbmstmt.h"
#include "ColossusConstants.h"
#include "ColossusCommon.h"
#include "AggregatorStructs.h"

struct AggregateHandData;
class AggregatorDbManager;

static UINT32 const HOLE_CARDS_LEN =  4 + 1;

////////////////////////////////////////////////////////////////////////////////

//lint -esym(1509, NoExecStmt) -esym(1512, NoExecStmt)
class NoExecStmt : public DbmStatement
{
public:
	explicit NoExecStmt(DatabaseManagerCommon& man) : DbmStatement(man) {}

protected: // Disallow invocation of execute methods
	using DbmStatement::execute;
	using DbmStatement::executeRet;
	using DbmStatement::execUpdateDelete;
};

////////////////////////////////////////////////////////////////////////////////

struct HandStatsStmtColumns
{
	PSqlString<USER_NAME_LEN> userId;
	PSqlBigInt handId;
	PSqlTimestamp finished;
	PSqlTournamentId tournamentId;
	PSqlSmallInt gameType;
	PSqlSmallInt structure;
	PSqlInt bigBlind;
	PSqlInt smallBlind;
	PSqlString<CURRENCY_LEN> currency;
	PSqlSmallInt maxPerTable;
	PSqlInt buyIn;
	PSqlInt totalWin;
	PSqlInt totalBlindsAntesPaid;
	PSqlInt totalRakePaid;
	PSqlInt numBigBlinds;
	PSqlInt numSmallBlinds;
	PSqlInt numBlindsPaidOutOfPosition;
	PSqlInt numVpip;
	PSqlInt numVpipFromSmallBlind;
	PSqlInt numVpipFromBigBlind;
	PSqlInt numSawShowdown;
	PSqlInt numWonShowdown;
	PSqlInt numWonAfterFlop;
	PSqlInt numLimp;
	PSqlInt numLimpRaise;
	PSqlInt numLimpCall;
	PSqlInt numLimpFold;
	PSqlInt numAllinPreflop;
	PSqlInt numColdCallOpportunities;
	PSqlInt numColdCalls;
	PSqlInt num3BetOpportunities;
	PSqlInt num3Bets;
	PSqlInt numFacing3Bet;
	PSqlInt numFoldTo3Bet;
	PSqlInt num4plusBet;
	PSqlInt numSawFlop;
	PSqlInt numSawTurn;
	PSqlInt numSawRiver;
	PSqlInt numSaw7th;
	PSqlInt numFacing4plusBet;
	PSqlInt numFoldTo4plusBet;
	PSqlInt numTurnsPreflop;
	PSqlInt numTurnsFacingBetOnFlop;
	PSqlInt numTurnsFacingNoBetOnFlop;
	PSqlInt numTurnsFacingBetOnTurn;
	PSqlInt numTurnsFacingNoBetOnTurn;
	PSqlInt numTurnsFacingBetOnRiver;
	PSqlInt numTurnsFacingNoBetOnRiver;
	PSqlInt numTurnsFacingBetOn7th;
	PSqlInt numTurnsFacingNoBetOn7th;
	PSqlInt numLastAgressorPreflop;
	PSqlInt numLastAgressorOnFlop;
	PSqlInt numLastAgressorOnTurn;
	PSqlInt numLastAgressorOnRiver;
	PSqlInt numContBetOnFlop;
	PSqlInt numContBetOnTurn;
	PSqlInt numContBetOnRiver;
	PSqlInt numContBetOn7th;
	PSqlInt numBetPreflop;
	PSqlInt numBetOnFlop;
	PSqlInt numBetOnTurn;
	PSqlInt numBetOnRiver;
	PSqlInt numBetOn7th;
	PSqlInt numCheckOnFlop;
	PSqlInt numCheckOnTurn;
	PSqlInt numCheckOnRiver;
	PSqlInt numCheckOn7th;
	PSqlInt numFoldPreflop;
	PSqlInt numFoldOnFlop;
	PSqlInt numFoldOnTurn;
	PSqlInt numFoldOnRiver;
	PSqlInt numFoldOn7th;
	PSqlInt numCallsPreflop;
	PSqlInt numCallsOnFlop;
	PSqlInt numCallsOnTurn;
	PSqlInt numCallsOnRiver;
	PSqlInt numCallsOn7th;
	PSqlInt numRaisesPreflop;
	PSqlInt numRaisesOnFlop;
	PSqlInt numRaisesOnTurn;
	PSqlInt numRaisesOnRiver;
	PSqlInt numRaisesOn7th;
	PSqlInt numCheckCallOnFlop;
	PSqlInt numCheckCallOnTurn;
	PSqlInt numCheckCallOnRiver;
	PSqlInt numCheckCallOn7th;
	PSqlInt numCheckRaiseOnFlop;
	PSqlInt numCheckRaiseOnTurn;
	PSqlInt numCheckRaiseOnRiver;
	PSqlInt numCheckRaiseOn7th;
	PSqlInt numCheckFoldOnFlop;
	PSqlInt numCheckFoldOnTurn;
	PSqlInt numCheckFoldOnRiver;
	PSqlInt numCheckFoldOn7th;
	PSqlInt numOpenFoldPowerfulHand;
	PSqlInt numFoldToExtremeOdds;
	PSqlInt numReraiseWithGarbage;
	PSqlInt numCheckBehindOnRiver;
	PSqlInt numFoldWhenCouldCheck;
	PSqlInt numAttemptStealBlinds;
	PSqlInt numFoldToStealFromSBOpps;
	PSqlInt numFoldToStealFromSB;
	PSqlInt numFoldToStealFromBBOpps;
	PSqlInt numFoldToStealFromBB;
	PSqlInt numCheckRaiseOppsOnFlop;
	PSqlInt numCheckRaiseOppsOnTurn;
	PSqlInt numCheckRaiseOppsOnRiver;
	PSqlInt numCheckRaiseOppsOn7th;
	PSqlInt numAttemptStealBlindsOpps;
	PSqlInt num4BetOpportunities;
	PSqlInt numHandsRaisedPreflop;
	PSqlInt numCallBetsOnlyPreflop;
	PSqlInt numCallBetsOnlyOnFlop;
	PSqlInt numCallBetsOnlyOnTurn;
	PSqlInt numCallBetsOnlyOnRiver;
	PSqlInt numCallBetsOnlyOn7th;
	PSqlInt numTurnsFacingBetOnlyOnFlop;
	PSqlInt numTurnsFacingBetOnlyOnTurn;
	PSqlInt numTurnsFacingBetOnlyOnRiver;
	PSqlInt numTurnsFacingBetOnlyOn7th;
	PSqlInt numFoldToBetOnlyPreflop;
	PSqlInt numFoldToBetOnlyOnFlop;
	PSqlInt numFoldToBetOnlyOnTurn;
	PSqlInt numFoldToBetOnlyOnRiver;
	PSqlInt numFoldToBetOnlyOn7th;

	PSqlString<USER_NAME_LEN> partnerNames[MAX_TABLE_PLAYERS];
	PSqlInt partnerProfit[MAX_TABLE_PLAYERS];

	PSqlSmallInt tournSchedulingType;
	PSqlSmallInt tournGameType;
	PSqlInt sessionType;
	PSqlBigInt sessionId;
	PSqlInt tournMaxEntry;
	PSqlBigInt flags;
	PSqlSmallInt allInConfrontations;
	PSqlSmallInt showdownAllInConfrontations;
	PSqlInt ptPosition;
	PSqlString<HOLE_CARDS_LEN> ptHoleCards;
	PSqlInt totalBet;
	PSqlInt aggregationFlags;

	PSqlBigInt statsFlags;				// PYR-30006
	PSqlBigInt statsFlags2;				// PYR-62032
	PSqlBigInt statsFlags3;				// PYR-62032
	PSqlBigInt statsFlags4;				// PYR-62032
	PSqlBigInt statsFlags5;				// PYR-62032
	PSqlBigInt statsFlags6;				// PYR-62032
	PSqlBigInt statsFlags7;				// PYR-62032
	PSqlBigInt statsFlags8;				// PYR-62032
	PSqlBigInt statsFlags9;				// PYR-62032
	PSqlBigInt statsFlags10;			// PYR-62032
	PSqlBigInt statsFlags11;			// PYR-62032
	PSqlInt hostId;						// PYR-45403
	PSqlInt tournActualEntries;			// PYR-46781

	UINT32 isFacing3BetHEM() const { return (statsFlags.value & eColHandStats1Facing3BetHEM) != 0; }
	UINT32 isFoldTo3BetHEM() const { return (statsFlags.value & eColHandStats1FoldTo3BetHEM) != 0; }
	UINT32 isFacing4PlusBetHEM() const { return (statsFlags.value & eColHandStats1Facing4PlusBetHEM) != 0; }
	UINT32 isFoldTo4PlusBetHEM() const { return (statsFlags.value & eColHandStats1FoldTo4PlusBetHEM) != 0; }

	// PYR-36556
	UINT32 isLastAgressorVs2InPosPreflop() const { return (statsFlags.value & eColHandStats1LastAgressorVs2InPosPreflop) != 0; }
	UINT32 isLastAgressorVs2InPosOnFlop() const { return (statsFlags.value & eColHandStats1LastAgressorVs2InPosOnFlop) != 0; }
	UINT32 isLastAgressorVs2InPosOnTurn() const { return (statsFlags.value & eColHandStats1LastAgressorVs2InPosOnTurn) != 0; }
	UINT32 isLastAgressorVs2InPosOnRiver() const { return (statsFlags.value & eColHandStats1LastAgressorVs2InPosOnRiver) != 0; }
	UINT32 isContBetVs2InPosOnFlop() const { return (statsFlags.value & eColHandStats1ContBetVs2InPosOnFlop) != 0; }
	UINT32 isContBetVs2InPosOnTurn() const { return (statsFlags.value & eColHandStats1ContBetVs2InPosOnTurn) != 0; }
	UINT32 isContBetVs2InPosOnRiver() const { return (statsFlags.value & eColHandStats1ContBetVs2InPosOnRiver) != 0; }
	UINT32 isContBetVs2InPosOn7th() const { return (statsFlags.value & eColHandStats1ContBetVs2InPosOn7th) != 0; }

	// PYR-62032
	UINT32 isVpipBbRaised14PlusMBot50() const { return (statsFlags.value & eColHandStats1VpipBbRaised14PlusMBot50) != 0; }
	UINT32 isVpipBbRaised14PlusMBot50Opps() const { return (statsFlags.value & eColHandStats1VpipBbRaised14PlusMBot50Opps) != 0; }
	UINT32 isVpipBbRaised14PlusMTop10() const { return (statsFlags.value & eColHandStats1VpipBbRaised14PlusMTop10) != 0; }
	UINT32 isVpipBbRaised14PlusMTop10Opps() const { return (statsFlags.value & eColHandStats1VpipBbRaised14PlusMTop10Opps) != 0; }
	UINT32 isVpipBbHuRaised0To4MBot50() const { return (statsFlags.value & eColHandStats1VpipBbHuRaised0To4MBot50) != 0; }
	UINT32 isVpipBbHuRaised0To4MBot50Opps() const { return (statsFlags.value & eColHandStats1VpipBbHuRaised0To4MBot50Opps) != 0; }
	UINT32 isVpipBbHuRaised0To4MTop30To50() const { return (statsFlags.value & eColHandStats1VpipBbHuRaised0To4MTop30To50) != 0; }
	UINT32 isVpipBbHuRaised0To4MTop30To50Opps() const { return (statsFlags.value & eColHandStats1VpipBbHuRaised0To4MTop30To50Opps) != 0; }
	UINT32 isVpipBbHuRaised4To7MTop30To50() const { return (statsFlags.value & eColHandStats1VpipBbHuRaised4To7MTop30To50) != 0; }
	UINT32 isVpipBbHuRaised4To7MTop30To50Opps() const { return (statsFlags.value & eColHandStats1VpipBbHuRaised4To7MTop30To50Opps) != 0; }
	UINT32 isVpipBbHuRaised4To7MBot50() const { return (statsFlags.value & eColHandStats1VpipBbHuRaised4To7MBot50) != 0; }
	UINT32 isVpipBbHuRaised4To7MBot50Opps() const { return (statsFlags.value & eColHandStats1VpipBbHuRaised4To7MBot50Opps) != 0; }
	UINT32 isVpipBbHuRaised7To14MTop30To50() const { return (statsFlags.value & eColHandStats1VpipBbHuRaised7To14MTop30To50) != 0; }
	UINT32 isVpipBbHuRaised7To14MTop30To50Opps() const { return (statsFlags.value & eColHandStats1VpipBbHuRaised7To14MTop30To50Opps) != 0; }
	UINT32 isVpipBbHuRaised7To14MBot50() const { return (statsFlags.value & eColHandStats1VpipBbHuRaised7To14MBot50) != 0; }
	UINT32 isVpipBbHuRaised7To14MBot50Opps() const { return (statsFlags.value & eColHandStats1VpipBbHuRaised7To14MBot50Opps) != 0; }
	UINT32 isVpipBbHuRaised14PlusMTop30To50() const { return (statsFlags.value & eColHandStats1VpipBbHuRaised14PlusMTop30To50) != 0; }
	UINT32 isVpipBbHuRaised14PlusMTop30To50Opps() const { return (statsFlags.value & eColHandStats1VpipBbHuRaised14PlusMTop30To50Opps) != 0; }
	UINT32 isVpipBbHuRaised14PlusMBot50() const { return (statsFlags.value & eColHandStats1VpipBbHuRaised14PlusMBot50) != 0; }
	UINT32 isVpipBbHuRaised14PlusMBot50Opps() const { return (statsFlags.value & eColHandStats1VpipBbHuRaised14PlusMBot50Opps) != 0; }
	UINT32 isVpipBtnRaised0To4MTop10() const { return (statsFlags.value & eColHandStats1VpipBtnRaised0To4MTop10) != 0; }
	UINT32 isVpipBtnRaised0To4MTop10Opps() const { return (statsFlags.value & eColHandStats1VpipBtnRaised0To4MTop10Opps) != 0; }
	UINT32 isVpipBtnRaised0To4MTop10To30() const { return (statsFlags.value & eColHandStats1VpipBtnRaised0To4MTop10To30) != 0; }
	UINT32 isVpipBtnRaised0To4MTop10To30Opps() const { return (statsFlags.value & eColHandStats1VpipBtnRaised0To4MTop10To30Opps) != 0; }
	UINT32 isVpipBtnRaised10To14MTop10() const { return (statsFlags.value & eColHandStats1VpipBtnRaised10To14MTop10) != 0; }
	UINT32 isVpipBtnRaised10To14MTop10Opps() const { return (statsFlags.value & eColHandStats1VpipBtnRaised10To14MTop10Opps) != 0; }
	UINT32 isVpipBtnRaised10To14MTop10To30() const { return (statsFlags.value & eColHandStats1VpipBtnRaised10To14MTop10To30) != 0; }
	UINT32 isVpipBtnRaised10To14MTop10To30Opps() const { return (statsFlags.value & eColHandStats1VpipBtnRaised10To14MTop10To30Opps) != 0; }
	UINT32 isVpipBtnCoMpRaised14PlusMTop10() const { return (statsFlags.value & eColHandStats1VpipBtnCoMpRaised14PlusMTop10) != 0; }
	UINT32 isVpipBtnCoMpRaised14PlusMTop10Opps() const { return (statsFlags.value & eColHandStats1VpipBtnCoMpRaised14PlusMTop10Opps) != 0; }
	UINT32 isVpipCoRaised0To4MTop10() const { return (statsFlags.value & eColHandStats1VpipCoRaised0To4MTop10) != 0; }
	UINT32 isVpipCoRaised0To4MTop10Opps() const { return (statsFlags.value & eColHandStats1VpipCoRaised0To4MTop10Opps) != 0; }
	UINT32 isVpipCoRaised0To4MTop10To30() const { return (statsFlags.value & eColHandStats1VpipCoRaised0To4MTop10To30) != 0; }
	UINT32 isVpipCoRaised0To4MTop10To30Opps() const { return (statsFlags.value & eColHandStats1VpipCoRaised0To4MTop10To30Opps) != 0; }
	UINT32 isVpipCoRaised4To7MTop10() const { return (statsFlags.value & eColHandStats1VpipCoRaised4To7MTop10) != 0; }
	UINT32 isVpipCoRaised4To7MTop10Opps() const { return (statsFlags.value & eColHandStats1VpipCoRaised4To7MTop10Opps) != 0; }
	UINT32 isVpipCoRaised4To7MTop10To30() const { return (statsFlags.value & eColHandStats1VpipCoRaised4To7MTop10To30) != 0; }
	UINT32 isVpipCoRaised4To7MTop10To30Opps() const { return (statsFlags.value & eColHandStats1VpipCoRaised4To7MTop10To30Opps) != 0; }
	UINT32 isVpipCoRaised7To10MTop10() const { return (statsFlags.value & eColHandStats1VpipCoRaised7To10MTop10) != 0; }
	UINT32 isVpipCoRaised7To10MTop10Opps() const { return (statsFlags.value & eColHandStats1VpipCoRaised7To10MTop10Opps) != 0; }
	UINT32 isVpipCoRaised7To10MTop10To30() const { return (statsFlags.value & eColHandStats1VpipCoRaised7To10MTop10To30) != 0; }
	UINT32 isVpipCoRaised7To10MTop10To30Opps() const { return (statsFlags.value & eColHandStats1VpipCoRaised7To10MTop10To30Opps) != 0; }
	UINT32 isVpipCoRaised10To14MTop10() const { return (statsFlags.value & eColHandStats1VpipCoRaised10To14MTop10) != 0; }
	UINT32 isVpipCoRaised10To14MTop10Opps() const { return (statsFlags.value & eColHandStats1VpipCoRaised10To14MTop10Opps) != 0; }
	UINT32 isVpipCoRaised10To14MTop10To30() const { return (statsFlags.value & eColHandStats1VpipCoRaised10To14MTop10To30) != 0; }
	UINT32 isVpipCoRaised10To14MTop10To30Opps() const { return (statsFlags.value & eColHandStats1VpipCoRaised10To14MTop10To30Opps) != 0; }
	UINT32 isVpipCoMpRaised14PlusMTop10To50() const { return (statsFlags.value & eColHandStats1VpipCoMpRaised14PlusMTop10To50) != 0; }
	UINT32 isVpipCoMpRaised14PlusMTop10To50Opps() const { return (statsFlags.value & eColHandStats1VpipCoMpRaised14PlusMTop10To50Opps) != 0; }
	UINT32 isVpipEpRaised4To7MTop10() const { return (statsFlags2.value & eColHandStats2VpipEpRaised4To7MTop10) != 0; }
	UINT32 isVpipEpRaised4To7MTop10Opps() const { return (statsFlags2.value & eColHandStats2VpipEpRaised4To7MTop10Opps) != 0; }
	UINT32 isVpipEpRaised7To10MTop10() const { return (statsFlags2.value & eColHandStats2VpipEpRaised7To10MTop10) != 0; }
	UINT32 isVpipEpRaised7To10MTop10Opps() const { return (statsFlags2.value & eColHandStats2VpipEpRaised7To10MTop10Opps) != 0; }
	UINT32 isVpipEpRaised10To14MTop10() const { return (statsFlags2.value & eColHandStats2VpipEpRaised10To14MTop10) != 0; }
	UINT32 isVpipEpRaised10To14MTop10Opps() const { return (statsFlags2.value & eColHandStats2VpipEpRaised10To14MTop10Opps) != 0; }
	UINT32 isVpipMpRaised0To4MTop10() const { return (statsFlags2.value & eColHandStats2VpipMpRaised0To4MTop10) != 0; }
	UINT32 isVpipMpRaised0To4MTop10Opps() const { return (statsFlags2.value & eColHandStats2VpipMpRaised0To4MTop10Opps) != 0; }
	UINT32 isVpipMpRaised0To4MTop10To30() const { return (statsFlags2.value & eColHandStats2VpipMpRaised0To4MTop10To30) != 0; }
	UINT32 isVpipMpRaised0To4MTop10To30Opps() const { return (statsFlags2.value & eColHandStats2VpipMpRaised0To4MTop10To30Opps) != 0; }
	UINT32 isVpipMpRaised4To7MTop10() const { return (statsFlags2.value & eColHandStats2VpipMpRaised4To7MTop10) != 0; }
	UINT32 isVpipMpRaised4To7MTop10Opps() const { return (statsFlags2.value & eColHandStats2VpipMpRaised4To7MTop10Opps) != 0; }
	UINT32 isVpipMpRaised7To10MTop10() const { return (statsFlags2.value & eColHandStats2VpipMpRaised7To10MTop10) != 0; }
	UINT32 isVpipMpRaised7To10MTop10Opps() const { return (statsFlags2.value & eColHandStats2VpipMpRaised7To10MTop10Opps) != 0; }
	UINT32 isVpipMpRaised10To14MTop10() const { return (statsFlags2.value & eColHandStats2VpipMpRaised10To14MTop10) != 0; }
	UINT32 isVpipMpRaised10To14MTop10Opps() const { return (statsFlags2.value & eColHandStats2VpipMpRaised10To14MTop10Opps) != 0; }
	UINT32 isVpipSbRaised14PlusMTop30To50() const { return (statsFlags2.value & eColHandStats2VpipSbRaised14PlusMTop30To50) != 0; }
	UINT32 isVpipSbRaised14PlusMTop30To50Opps() const { return (statsFlags2.value & eColHandStats2VpipSbRaised14PlusMTop30To50Opps) != 0; }
	UINT32 isVpipSbLimped7To14MBot50() const { return (statsFlags2.value & eColHandStats2VpipSbLimped7To14MBot50) != 0; }
	UINT32 isVpipSbLimped7To14MBot50Opps() const { return (statsFlags2.value & eColHandStats2VpipSbLimped7To14MBot50Opps) != 0; }
	UINT32 isVpipSbLimped10To14MTop30To50() const { return (statsFlags2.value & eColHandStats2VpipSbLimped10To14MTop30To50) != 0; }
	UINT32 isVpipSbLimped10To14MTop30To50Opps() const { return (statsFlags2.value & eColHandStats2VpipSbLimped10To14MTop30To50Opps) != 0; }
	UINT32 isVpipSbLimped14PlusMTop10() const { return (statsFlags2.value & eColHandStats2VpipSbLimped14PlusMTop10) != 0; }
	UINT32 isVpipSbLimped14PlusMTop10Opps() const { return (statsFlags2.value & eColHandStats2VpipSbLimped14PlusMTop10Opps) != 0; }
	UINT32 isVpipSbLimped14PlusMTop10To30() const { return (statsFlags2.value & eColHandStats2VpipSbLimped14PlusMTop10To30) != 0; }
	UINT32 isVpipSbLimped14PlusMTop10To30Opps() const { return (statsFlags2.value & eColHandStats2VpipSbLimped14PlusMTop10To30Opps) != 0; }
	UINT32 isVpipSbLimped14PlusMTop30To100() const { return (statsFlags2.value & eColHandStats2VpipSbLimped14PlusMTop30To100) != 0; }
	UINT32 isVpipSbLimped14PlusMTop30To100Opps() const { return (statsFlags2.value & eColHandStats2VpipSbLimped14PlusMTop30To100Opps) != 0; }
	UINT32 isVpipSbUnopened4To14MBot50() const { return (statsFlags2.value & eColHandStats2VpipSbUnopened4To14MBot50) != 0; }
	UINT32 isVpipSbUnopened4To14MBot50Opps() const { return (statsFlags2.value & eColHandStats2VpipSbUnopened4To14MBot50Opps) != 0; }
	UINT32 isVpipSbUnopened4To14MTop30To50() const { return (statsFlags2.value & eColHandStats2VpipSbUnopened4To14MTop30To50) != 0; }
	UINT32 isVpipSbUnopened4To14MTop30To50Opps() const { return (statsFlags2.value & eColHandStats2VpipSbUnopened4To14MTop30To50Opps) != 0; }
	UINT32 isVpipSbHuUnopened0To4MBot50() const { return (statsFlags2.value & eColHandStats2VpipSbHuUnopened0To4MBot50) != 0; }
	UINT32 isVpipSbHuUnopened0To4MBot50Opps() const { return (statsFlags2.value & eColHandStats2VpipSbHuUnopened0To4MBot50Opps) != 0; }
	UINT32 isVpipSbHuUnopened4To14MBot50() const { return (statsFlags2.value & eColHandStats2VpipSbHuUnopened4To14MBot50) != 0; }
	UINT32 isVpipSbHuUnopened4To14MBot50Opps() const { return (statsFlags2.value & eColHandStats2VpipSbHuUnopened4To14MBot50Opps) != 0; }
	UINT32 isPfrBbLimped10To14MTop10To30() const { return (statsFlags2.value & eColHandStats2PfrBbLimped10To14MTop10To30) != 0; }
	UINT32 isPfrBbLimped10To14MTop10To30Opps() const { return (statsFlags2.value & eColHandStats2PfrBbLimped10To14MTop10To30Opps) != 0; }
	UINT32 isPfrBbLimped14PlusMBot50() const { return (statsFlags2.value & eColHandStats2PfrBbLimped14PlusMBot50) != 0; }
	UINT32 isPfrBbLimped14PlusMBot50Opps() const { return (statsFlags2.value & eColHandStats2PfrBbLimped14PlusMBot50Opps) != 0; }
	UINT32 isPfrBbLimped14PlusMTop10To50() const { return (statsFlags2.value & eColHandStats2PfrBbLimped14PlusMTop10To50) != 0; }
	UINT32 isPfrBbLimped14PlusMTop10To50Opps() const { return (statsFlags2.value & eColHandStats2PfrBbLimped14PlusMTop10To50Opps) != 0; }
	UINT32 isPfrBbLimped4To7MBot50() const { return (statsFlags2.value & eColHandStats2PfrBbLimped4To7MBot50) != 0; }
	UINT32 isPfrBbLimped4To7MBot50Opps() const { return (statsFlags2.value & eColHandStats2PfrBbLimped4To7MBot50Opps) != 0; }
	UINT32 isPfrBbLimped4To7MTop10To30() const { return (statsFlags2.value & eColHandStats2PfrBbLimped4To7MTop10To30) != 0; }
	UINT32 isPfrBbLimped4To7MTop10To30Opps() const { return (statsFlags2.value & eColHandStats2PfrBbLimped4To7MTop10To30Opps) != 0; }
	UINT32 isPfrBbLimped4To7MTop30To50() const { return (statsFlags2.value & eColHandStats2PfrBbLimped4To7MTop30To50) != 0; }
	UINT32 isPfrBbLimped4To7MTop30To50Opps() const { return (statsFlags2.value & eColHandStats2PfrBbLimped4To7MTop30To50Opps) != 0; }
	UINT32 isPfrBbLimped7To10MTop10To30() const { return (statsFlags2.value & eColHandStats2PfrBbLimped7To10MTop10To30) != 0; }
	UINT32 isPfrBbLimped7To10MTop10To30Opps() const { return (statsFlags2.value & eColHandStats2PfrBbLimped7To10MTop10To30Opps) != 0; }
	UINT32 isPfrBbLimped7To14MBot50() const { return (statsFlags2.value & eColHandStats2PfrBbLimped7To14MBot50) != 0; }
	UINT32 isPfrBbLimped7To14MBot50Opps() const { return (statsFlags2.value & eColHandStats2PfrBbLimped7To14MBot50Opps) != 0; }
	UINT32 isPfrBbLimped7To14MTop30To50() const { return (statsFlags2.value & eColHandStats2PfrBbLimped7To14MTop30To50) != 0; }
	UINT32 isPfrBbLimped7To14MTop30To50Opps() const { return (statsFlags2.value & eColHandStats2PfrBbLimped7To14MTop30To50Opps) != 0; }
	UINT32 isPfrBbHuLimped0To4MBot50() const { return (statsFlags2.value & eColHandStats2PfrBbHuLimped0To4MBot50) != 0; }
	UINT32 isPfrBbHuLimped0To4MBot50Opps() const { return (statsFlags2.value & eColHandStats2PfrBbHuLimped0To4MBot50Opps) != 0; }
	UINT32 isPfrBbHuLimped0To4MTop10() const { return (statsFlags2.value & eColHandStats2PfrBbHuLimped0To4MTop10) != 0; }
	UINT32 isPfrBbHuLimped0To4MTop10Opps() const { return (statsFlags2.value & eColHandStats2PfrBbHuLimped0To4MTop10Opps) != 0; }
	UINT32 isPfrBbHuLimped0To4MTop10To30() const { return (statsFlags2.value & eColHandStats2PfrBbHuLimped0To4MTop10To30) != 0; }
	UINT32 isPfrBbHuLimped0To4MTop10To30Opps() const { return (statsFlags2.value & eColHandStats2PfrBbHuLimped0To4MTop10To30Opps) != 0; }
	UINT32 isPfrBbHuLimped0To4MTop30To50() const { return (statsFlags3.value & eColHandStats3PfrBbHuLimped0To4MTop30To50) != 0; }
	UINT32 isPfrBbHuLimped0To4MTop30To50Opps() const { return (statsFlags3.value & eColHandStats3PfrBbHuLimped0To4MTop30To50Opps) != 0; }
	UINT32 isPfrBbHuLimped10To14MBot50() const { return (statsFlags3.value & eColHandStats3PfrBbHuLimped10To14MBot50) != 0; }
	UINT32 isPfrBbHuLimped10To14MBot50Opps() const { return (statsFlags3.value & eColHandStats3PfrBbHuLimped10To14MBot50Opps) != 0; }
	UINT32 isPfrBbHuLimped10To14MTop10() const { return (statsFlags3.value & eColHandStats3PfrBbHuLimped10To14MTop10) != 0; }
	UINT32 isPfrBbHuLimped10To14MTop10Opps() const { return (statsFlags3.value & eColHandStats3PfrBbHuLimped10To14MTop10Opps) != 0; }
	UINT32 isPfrBbHuLimped10To14MTop10To30() const { return (statsFlags3.value & eColHandStats3PfrBbHuLimped10To14MTop10To30) != 0; }
	UINT32 isPfrBbHuLimped10To14MTop10To30Opps() const { return (statsFlags3.value & eColHandStats3PfrBbHuLimped10To14MTop10To30Opps) != 0; }
	UINT32 isPfrBbHuLimped10To14MTop30To50() const { return (statsFlags3.value & eColHandStats3PfrBbHuLimped10To14MTop30To50) != 0; }
	UINT32 isPfrBbHuLimped10To14MTop30To50Opps() const { return (statsFlags3.value & eColHandStats3PfrBbHuLimped10To14MTop30To50Opps) != 0; }
	UINT32 isPfrBbHuLimped14PlusMBot50() const { return (statsFlags3.value & eColHandStats3PfrBbHuLimped14PlusMBot50) != 0; }
	UINT32 isPfrBbHuLimped14PlusMBot50Opps() const { return (statsFlags3.value & eColHandStats3PfrBbHuLimped14PlusMBot50Opps) != 0; }
	UINT32 isPfrBbHuLimped14PlusMTop10To30() const { return (statsFlags3.value & eColHandStats3PfrBbHuLimped14PlusMTop10To30) != 0; }
	UINT32 isPfrBbHuLimped14PlusMTop10To30Opps() const { return (statsFlags3.value & eColHandStats3PfrBbHuLimped14PlusMTop10To30Opps) != 0; }
	UINT32 isPfrBbHuLimped14PlusMTop30To50() const { return (statsFlags3.value & eColHandStats3PfrBbHuLimped14PlusMTop30To50) != 0; }
	UINT32 isPfrBbHuLimped14PlusMTop30To50Opps() const { return (statsFlags3.value & eColHandStats3PfrBbHuLimped14PlusMTop30To50Opps) != 0; }
	UINT32 isPfrBbHuLimped4To7MBot50() const { return (statsFlags3.value & eColHandStats3PfrBbHuLimped4To7MBot50) != 0; }
	UINT32 isPfrBbHuLimped4To7MBot50Opps() const { return (statsFlags3.value & eColHandStats3PfrBbHuLimped4To7MBot50Opps) != 0; }
	UINT32 isPfrBbHuLimped4To7MTop10() const { return (statsFlags3.value & eColHandStats3PfrBbHuLimped4To7MTop10) != 0; }
	UINT32 isPfrBbHuLimped4To7MTop10Opps() const { return (statsFlags3.value & eColHandStats3PfrBbHuLimped4To7MTop10Opps) != 0; }
	UINT32 isPfrBbHuLimped4To7MTop10To30() const { return (statsFlags3.value & eColHandStats3PfrBbHuLimped4To7MTop10To30) != 0; }
	UINT32 isPfrBbHuLimped4To7MTop10To30Opps() const { return (statsFlags3.value & eColHandStats3PfrBbHuLimped4To7MTop10To30Opps) != 0; }
	UINT32 isPfrBbHuLimped4To7MTop30To50() const { return (statsFlags3.value & eColHandStats3PfrBbHuLimped4To7MTop30To50) != 0; }
	UINT32 isPfrBbHuLimped4To7MTop30To50Opps() const { return (statsFlags3.value & eColHandStats3PfrBbHuLimped4To7MTop30To50Opps) != 0; }
	UINT32 isPfrBbHuLimped7To10MBot50() const { return (statsFlags3.value & eColHandStats3PfrBbHuLimped7To10MBot50) != 0; }
	UINT32 isPfrBbHuLimped7To10MBot50Opps() const { return (statsFlags3.value & eColHandStats3PfrBbHuLimped7To10MBot50Opps) != 0; }
	UINT32 isPfrBbHuLimped7To10MTop10() const { return (statsFlags3.value & eColHandStats3PfrBbHuLimped7To10MTop10) != 0; }
	UINT32 isPfrBbHuLimped7To10MTop10Opps() const { return (statsFlags3.value & eColHandStats3PfrBbHuLimped7To10MTop10Opps) != 0; }
	UINT32 isPfrBbHuLimped7To10MTop10To30() const { return (statsFlags3.value & eColHandStats3PfrBbHuLimped7To10MTop10To30) != 0; }
	UINT32 isPfrBbHuLimped7To10MTop10To30Opps() const { return (statsFlags3.value & eColHandStats3PfrBbHuLimped7To10MTop10To30Opps) != 0; }
	UINT32 isPfrBbHuLimped7To10MTop30To50() const { return (statsFlags3.value & eColHandStats3PfrBbHuLimped7To10MTop30To50) != 0; }
	UINT32 isPfrBbHuLimped7To10MTop30To50Opps() const { return (statsFlags3.value & eColHandStats3PfrBbHuLimped7To10MTop30To50Opps) != 0; }
	UINT32 isPfrBbSbLimped14PlusMTop10() const { return (statsFlags3.value & eColHandStats3PfrBbSbLimped14PlusMTop10) != 0; }
	UINT32 isPfrBbSbLimped14PlusMTop10Opps() const { return (statsFlags3.value & eColHandStats3PfrBbSbLimped14PlusMTop10Opps) != 0; }
	UINT32 isPfrBtnLimped14PlusMTop10To50() const { return (statsFlags3.value & eColHandStats3PfrBtnLimped14PlusMTop10To50) != 0; }
	UINT32 isPfrBtnLimped14PlusMTop10To50Opps() const { return (statsFlags3.value & eColHandStats3PfrBtnLimped14PlusMTop10To50Opps) != 0; }
	UINT32 isPfrBtnLimped4To14MTop10To50() const { return (statsFlags3.value & eColHandStats3PfrBtnLimped4To14MTop10To50) != 0; }
	UINT32 isPfrBtnLimped4To14MTop10To50Opps() const { return (statsFlags3.value & eColHandStats3PfrBtnLimped4To14MTop10To50Opps) != 0; }
	UINT32 isPfrBtnUnopened0To4MBot50() const { return (statsFlags3.value & eColHandStats3PfrBtnUnopened0To4MBot50) != 0; }
	UINT32 isPfrBtnUnopened0To4MBot50Opps() const { return (statsFlags3.value & eColHandStats3PfrBtnUnopened0To4MBot50Opps) != 0; }
	UINT32 isPfrBtnUnopened0To4MTop30To50() const { return (statsFlags3.value & eColHandStats3PfrBtnUnopened0To4MTop30To50) != 0; }
	UINT32 isPfrBtnUnopened0To4MTop30To50Opps() const { return (statsFlags3.value & eColHandStats3PfrBtnUnopened0To4MTop30To50Opps) != 0; }
	UINT32 isPfrBtnUnopened14PlusMBot50() const { return (statsFlags3.value & eColHandStats3PfrBtnUnopened14PlusMBot50) != 0; }
	UINT32 isPfrBtnUnopened14PlusMBot50Opps() const { return (statsFlags3.value & eColHandStats3PfrBtnUnopened14PlusMBot50Opps) != 0; }
	UINT32 isPfrBtnUnopened14PlusMTop10To50() const { return (statsFlags3.value & eColHandStats3PfrBtnUnopened14PlusMTop10To50) != 0; }
	UINT32 isPfrBtnUnopened14PlusMTop10To50Opps() const { return (statsFlags3.value & eColHandStats3PfrBtnUnopened14PlusMTop10To50Opps) != 0; }
	UINT32 isPfrBtnUnopened4To7MTop10To30() const { return (statsFlags3.value & eColHandStats3PfrBtnUnopened4To7MTop10To30) != 0; }
	UINT32 isPfrBtnUnopened4To7MTop10To30Opps() const { return (statsFlags3.value & eColHandStats3PfrBtnUnopened4To7MTop10To30Opps) != 0; }
	UINT32 isPfrBtnUnopened4To7MTop30To50() const { return (statsFlags3.value & eColHandStats3PfrBtnUnopened4To7MTop30To50) != 0; }
	UINT32 isPfrBtnUnopened4To7MTop30To50Opps() const { return (statsFlags3.value & eColHandStats3PfrBtnUnopened4To7MTop30To50Opps) != 0; }
	UINT32 isPfrBtnUnopened7To14MTop10To30() const { return (statsFlags3.value & eColHandStats3PfrBtnUnopened7To14MTop10To30) != 0; }
	UINT32 isPfrBtnUnopened7To14MTop10To30Opps() const { return (statsFlags3.value & eColHandStats3PfrBtnUnopened7To14MTop10To30Opps) != 0; }
	UINT32 isPfrBtnUnopened7To14MTop30To50() const { return (statsFlags3.value & eColHandStats3PfrBtnUnopened7To14MTop30To50) != 0; }
	UINT32 isPfrBtnUnopened7To14MTop30To50Opps() const { return (statsFlags3.value & eColHandStats3PfrBtnUnopened7To14MTop30To50Opps) != 0; }
	UINT32 isPfrCoLimped14PlusMTop10To30() const { return (statsFlags3.value & eColHandStats3PfrCoLimped14PlusMTop10To30) != 0; }
	UINT32 isPfrCoLimped14PlusMTop10To30Opps() const { return (statsFlags3.value & eColHandStats3PfrCoLimped14PlusMTop10To30Opps) != 0; }
	UINT32 isPfrCoLimped14PlusMTop30To50() const { return (statsFlags3.value & eColHandStats3PfrCoLimped14PlusMTop30To50) != 0; }
	UINT32 isPfrCoLimped14PlusMTop30To50Opps() const { return (statsFlags3.value & eColHandStats3PfrCoLimped14PlusMTop30To50Opps) != 0; }
	UINT32 isPfrCoUnopened0To4MTop10To30() const { return (statsFlags3.value & eColHandStats3PfrCoUnopened0To4MTop10To30) != 0; }
	UINT32 isPfrCoUnopened0To4MTop10To30Opps() const { return (statsFlags3.value & eColHandStats3PfrCoUnopened0To4MTop10To30Opps) != 0; }
	UINT32 isPfrCoUnopened0To4MTop30To50() const { return (statsFlags4.value & eColHandStats4PfrCoUnopened0To4MTop30To50) != 0; }
	UINT32 isPfrCoUnopened0To4MTop30To50Opps() const { return (statsFlags4.value & eColHandStats4PfrCoUnopened0To4MTop30To50Opps) != 0; }
	UINT32 isPfrCoUnopened10To14MTop10To30() const { return (statsFlags4.value & eColHandStats4PfrCoUnopened10To14MTop10To30) != 0; }
	UINT32 isPfrCoUnopened10To14MTop10To30Opps() const { return (statsFlags4.value & eColHandStats4PfrCoUnopened10To14MTop10To30Opps) != 0; }
	UINT32 isPfrCoUnopened10To14MTop30To50() const { return (statsFlags4.value & eColHandStats4PfrCoUnopened10To14MTop30To50) != 0; }
	UINT32 isPfrCoUnopened10To14MTop30To50Opps() const { return (statsFlags4.value & eColHandStats4PfrCoUnopened10To14MTop30To50Opps) != 0; }
	UINT32 isPfrCoUnopened14PlusMTop30To50() const { return (statsFlags4.value & eColHandStats4PfrCoUnopened14PlusMTop30To50) != 0; }
	UINT32 isPfrCoUnopened14PlusMTop30To50Opps() const { return (statsFlags4.value & eColHandStats4PfrCoUnopened14PlusMTop30To50Opps) != 0; }
	UINT32 isPfrCoUnopened4To7MTop10To30() const { return (statsFlags4.value & eColHandStats4PfrCoUnopened4To7MTop10To30) != 0; }
	UINT32 isPfrCoUnopened4To7MTop10To30Opps() const { return (statsFlags4.value & eColHandStats4PfrCoUnopened4To7MTop10To30Opps) != 0; }
	UINT32 isPfrCoUnopened4To7MTop30To50() const { return (statsFlags4.value & eColHandStats4PfrCoUnopened4To7MTop30To50) != 0; }
	UINT32 isPfrCoUnopened4To7MTop30To50Opps() const { return (statsFlags4.value & eColHandStats4PfrCoUnopened4To7MTop30To50Opps) != 0; }
	UINT32 isPfrCoUnopened7To10MTop10To30() const { return (statsFlags4.value & eColHandStats4PfrCoUnopened7To10MTop10To30) != 0; }
	UINT32 isPfrCoUnopened7To10MTop10To30Opps() const { return (statsFlags4.value & eColHandStats4PfrCoUnopened7To10MTop10To30Opps) != 0; }
	UINT32 isPfrCoUnopened7To10MTop30To50() const { return (statsFlags4.value & eColHandStats4PfrCoUnopened7To10MTop30To50) != 0; }
	UINT32 isPfrCoUnopened7To10MTop30To50Opps() const { return (statsFlags4.value & eColHandStats4PfrCoUnopened7To10MTop30To50Opps) != 0; }
	UINT32 isPfrCoBtnUnopened14PlusMTop10To30() const { return (statsFlags4.value & eColHandStats4PfrCoBtnUnopened14PlusMTop10To30) != 0; }
	UINT32 isPfrCoBtnUnopened14PlusMTop10To30Opps() const { return (statsFlags4.value & eColHandStats4PfrCoBtnUnopened14PlusMTop10To30Opps) != 0; }
	UINT32 isPfrEpUnopened0To4MTop10() const { return (statsFlags4.value & eColHandStats4PfrEpUnopened0To4MTop10) != 0; }
	UINT32 isPfrEpUnopened0To4MTop10Opps() const { return (statsFlags4.value & eColHandStats4PfrEpUnopened0To4MTop10Opps) != 0; }
	UINT32 isPfrEpUnopened0To4MTop10To30() const { return (statsFlags4.value & eColHandStats4PfrEpUnopened0To4MTop10To30) != 0; }
	UINT32 isPfrEpUnopened0To4MTop10To30Opps() const { return (statsFlags4.value & eColHandStats4PfrEpUnopened0To4MTop10To30Opps) != 0; }
	UINT32 isPfrEpUnopened4To14MTop10() const { return (statsFlags4.value & eColHandStats4PfrEpUnopened4To14MTop10) != 0; }
	UINT32 isPfrEpUnopened4To14MTop10Opps() const { return (statsFlags4.value & eColHandStats4PfrEpUnopened4To14MTop10Opps) != 0; }
	UINT32 isPfrEpUnopened4To14MTop10To30() const { return (statsFlags4.value & eColHandStats4PfrEpUnopened4To14MTop10To30) != 0; }
	UINT32 isPfrEpUnopened4To14MTop10To30Opps() const { return (statsFlags4.value & eColHandStats4PfrEpUnopened4To14MTop10To30Opps) != 0; }
	UINT32 isPfrMpUnopened0To4MTop10To30() const { return (statsFlags4.value & eColHandStats4PfrMpUnopened0To4MTop10To30) != 0; }
	UINT32 isPfrMpUnopened0To4MTop10To30Opps() const { return (statsFlags4.value & eColHandStats4PfrMpUnopened0To4MTop10To30Opps) != 0; }
	UINT32 isPfrMpUnopened0To4MTop30To50() const { return (statsFlags4.value & eColHandStats4PfrMpUnopened0To4MTop30To50) != 0; }
	UINT32 isPfrMpUnopened0To4MTop30To50Opps() const { return (statsFlags4.value & eColHandStats4PfrMpUnopened0To4MTop30To50Opps) != 0; }
	UINT32 isPfrMpUnopened4To14MTop10To30() const { return (statsFlags4.value & eColHandStats4PfrMpUnopened4To14MTop10To30) != 0; }
	UINT32 isPfrMpUnopened4To14MTop10To30Opps() const { return (statsFlags4.value & eColHandStats4PfrMpUnopened4To14MTop10To30Opps) != 0; }
	UINT32 isPfrMpEPUnopened14PlusMTop10To30() const { return (statsFlags4.value & eColHandStats4PfrMpEPUnopened14PlusMTop10To30) != 0; }
	UINT32 isPfrMpEPUnopened14PlusMTop10To30Opps() const { return (statsFlags4.value & eColHandStats4PfrMpEPUnopened14PlusMTop10To30Opps) != 0; }
	UINT32 isPfrMpEPUnopened14PlusMTop30To50() const { return (statsFlags4.value & eColHandStats4PfrMpEPUnopened14PlusMTop30To50) != 0; }
	UINT32 isPfrMpEPUnopened14PlusMTop30To50Opps() const { return (statsFlags4.value & eColHandStats4PfrMpEPUnopened14PlusMTop30To50Opps) != 0; }
	UINT32 isPfrSbLimped10To14MTop10() const { return (statsFlags4.value & eColHandStats4PfrSbLimped10To14MTop10) != 0; }
	UINT32 isPfrSbLimped10To14MTop10Opps() const { return (statsFlags4.value & eColHandStats4PfrSbLimped10To14MTop10Opps) != 0; }
	UINT32 isPfrSbLimped10To14MTop10To30() const { return (statsFlags4.value & eColHandStats4PfrSbLimped10To14MTop10To30) != 0; }
	UINT32 isPfrSbLimped10To14MTop10To30Opps() const { return (statsFlags4.value & eColHandStats4PfrSbLimped10To14MTop10To30Opps) != 0; }
	UINT32 isPfrSbLimped10To14MTop30To50() const { return (statsFlags4.value & eColHandStats4PfrSbLimped10To14MTop30To50) != 0; }
	UINT32 isPfrSbLimped10To14MTop30To50Opps() const { return (statsFlags4.value & eColHandStats4PfrSbLimped10To14MTop30To50Opps) != 0; }
	UINT32 isPfrSbLimped14PlusMTop10To30() const { return (statsFlags4.value & eColHandStats4PfrSbLimped14PlusMTop10To30) != 0; }
	UINT32 isPfrSbLimped14PlusMTop10To30Opps() const { return (statsFlags4.value & eColHandStats4PfrSbLimped14PlusMTop10To30Opps) != 0; }
	UINT32 isPfrSbLimped14PlusMTop30To50() const { return (statsFlags4.value & eColHandStats4PfrSbLimped14PlusMTop30To50) != 0; }
	UINT32 isPfrSbLimped14PlusMTop30To50Opps() const { return (statsFlags4.value & eColHandStats4PfrSbLimped14PlusMTop30To50Opps) != 0; }
	UINT32 isPfrSbLimped7To10MTop10() const { return (statsFlags4.value & eColHandStats4PfrSbLimped7To10MTop10) != 0; }
	UINT32 isPfrSbLimped7To10MTop10Opps() const { return (statsFlags4.value & eColHandStats4PfrSbLimped7To10MTop10Opps) != 0; }
	UINT32 isPfrSbLimped7To10MTop10To30() const { return (statsFlags4.value & eColHandStats4PfrSbLimped7To10MTop10To30) != 0; }
	UINT32 isPfrSbLimped7To10MTop10To30Opps() const { return (statsFlags4.value & eColHandStats4PfrSbLimped7To10MTop10To30Opps) != 0; }
	UINT32 isPfrSbLimped7To10MTop30To50() const { return (statsFlags4.value & eColHandStats4PfrSbLimped7To10MTop30To50) != 0; }
	UINT32 isPfrSbLimped7To10MTop30To50Opps() const { return (statsFlags4.value & eColHandStats4PfrSbLimped7To10MTop30To50Opps) != 0; }
	UINT32 isPfrSbUnopened0To4MTop30To50() const { return (statsFlags4.value & eColHandStats4PfrSbUnopened0To4MTop30To50) != 0; }
	UINT32 isPfrSbUnopened0To4MTop30To50Opps() const { return (statsFlags4.value & eColHandStats4PfrSbUnopened0To4MTop30To50Opps) != 0; }
	UINT32 isPfrSbUnopened0To7MBot50() const { return (statsFlags4.value & eColHandStats4PfrSbUnopened0To7MBot50) != 0; }
	UINT32 isPfrSbUnopened0To7MBot50Opps() const { return (statsFlags4.value & eColHandStats4PfrSbUnopened0To7MBot50Opps) != 0; }
	UINT32 isPfrSbUnopened10To14MTop10() const { return (statsFlags4.value & eColHandStats4PfrSbUnopened10To14MTop10) != 0; }
	UINT32 isPfrSbUnopened10To14MTop10Opps() const { return (statsFlags4.value & eColHandStats4PfrSbUnopened10To14MTop10Opps) != 0; }
	UINT32 isPfrSbUnopened10To14MTop10To30() const { return (statsFlags4.value & eColHandStats4PfrSbUnopened10To14MTop10To30) != 0; }
	UINT32 isPfrSbUnopened10To14MTop10To30Opps() const { return (statsFlags4.value & eColHandStats4PfrSbUnopened10To14MTop10To30Opps) != 0; }
	UINT32 isPfrSbUnopened10To14MTop30To50() const { return (statsFlags5.value & eColHandStats5PfrSbUnopened10To14MTop30To50) != 0; }
	UINT32 isPfrSbUnopened10To14MTop30To50Opps() const { return (statsFlags5.value & eColHandStats5PfrSbUnopened10To14MTop30To50Opps) != 0; }
	UINT32 isPfrSbUnopened14PlusMBot50() const { return (statsFlags5.value & eColHandStats5PfrSbUnopened14PlusMBot50) != 0; }
	UINT32 isPfrSbUnopened14PlusMBot50Opps() const { return (statsFlags5.value & eColHandStats5PfrSbUnopened14PlusMBot50Opps) != 0; }
	UINT32 isPfrSbUnopened14PlusMTop10To30() const { return (statsFlags5.value & eColHandStats5PfrSbUnopened14PlusMTop10To30) != 0; }
	UINT32 isPfrSbUnopened14PlusMTop10To30Opps() const { return (statsFlags5.value & eColHandStats5PfrSbUnopened14PlusMTop10To30Opps) != 0; }
	UINT32 isPfrSbUnopened14PlusMTop30To50() const { return (statsFlags5.value & eColHandStats5PfrSbUnopened14PlusMTop30To50) != 0; }
	UINT32 isPfrSbUnopened14PlusMTop30To50Opps() const { return (statsFlags5.value & eColHandStats5PfrSbUnopened14PlusMTop30To50Opps) != 0; }
	UINT32 isPfrSbUnopened4To7MTop10() const { return (statsFlags5.value & eColHandStats5PfrSbUnopened4To7MTop10) != 0; }
	UINT32 isPfrSbUnopened4To7MTop10Opps() const { return (statsFlags5.value & eColHandStats5PfrSbUnopened4To7MTop10Opps) != 0; }
	UINT32 isPfrSbUnopened4To7MTop10To30() const { return (statsFlags5.value & eColHandStats5PfrSbUnopened4To7MTop10To30) != 0; }
	UINT32 isPfrSbUnopened4To7MTop10To30Opps() const { return (statsFlags5.value & eColHandStats5PfrSbUnopened4To7MTop10To30Opps) != 0; }
	UINT32 isPfrSbUnopened7To10MTop10() const { return (statsFlags5.value & eColHandStats5PfrSbUnopened7To10MTop10) != 0; }
	UINT32 isPfrSbUnopened7To10MTop10Opps() const { return (statsFlags5.value & eColHandStats5PfrSbUnopened7To10MTop10Opps) != 0; }
	UINT32 isPfrSbUnopened7To10MTop10To30() const { return (statsFlags5.value & eColHandStats5PfrSbUnopened7To10MTop10To30) != 0; }
	UINT32 isPfrSbUnopened7To10MTop10To30Opps() const { return (statsFlags5.value & eColHandStats5PfrSbUnopened7To10MTop10To30Opps) != 0; }
	UINT32 isPfrSbUnopened7To10MTop30To50() const { return (statsFlags5.value & eColHandStats5PfrSbUnopened7To10MTop30To50) != 0; }
	UINT32 isPfrSbUnopened7To10MTop30To50Opps() const { return (statsFlags5.value & eColHandStats5PfrSbUnopened7To10MTop30To50Opps) != 0; }
	UINT32 isPfrSbUnopened7To14MBot50() const { return (statsFlags5.value & eColHandStats5PfrSbUnopened7To14MBot50) != 0; }
	UINT32 isPfrSbUnopened7To14MBot50Opps() const { return (statsFlags5.value & eColHandStats5PfrSbUnopened7To14MBot50Opps) != 0; }
	UINT32 isPfrSbHuUnopened0To4MTop10() const { return (statsFlags5.value & eColHandStats5PfrSbHuUnopened0To4MTop10) != 0; }
	UINT32 isPfrSbHuUnopened0To4MTop10Opps() const { return (statsFlags5.value & eColHandStats5PfrSbHuUnopened0To4MTop10Opps) != 0; }
	UINT32 isPfrSbHuUnopened0To4MTop10To30() const { return (statsFlags5.value & eColHandStats5PfrSbHuUnopened0To4MTop10To30) != 0; }
	UINT32 isPfrSbHuUnopened0To4MTop10To30Opps() const { return (statsFlags5.value & eColHandStats5PfrSbHuUnopened0To4MTop10To30Opps) != 0; }
	UINT32 isPfrSbHuUnopened0To4MTop30To50() const { return (statsFlags5.value & eColHandStats5PfrSbHuUnopened0To4MTop30To50) != 0; }
	UINT32 isPfrSbHuUnopened0To4MTop30To50Opps() const { return (statsFlags5.value & eColHandStats5PfrSbHuUnopened0To4MTop30To50Opps) != 0; }
	UINT32 isPfrSbHuUnopened14PlusMBot50() const { return (statsFlags5.value & eColHandStats5PfrSbHuUnopened14PlusMBot50) != 0; }
	UINT32 isPfrSbHuUnopened14PlusMBot50Opps() const { return (statsFlags5.value & eColHandStats5PfrSbHuUnopened14PlusMBot50Opps) != 0; }
	UINT32 isPfrSbHuUnopened14PlusMTop30To50() const { return (statsFlags5.value & eColHandStats5PfrSbHuUnopened14PlusMTop30To50) != 0; }
	UINT32 isPfrSbHuUnopened14PlusMTop30To50Opps() const { return (statsFlags5.value & eColHandStats5PfrSbHuUnopened14PlusMTop30To50Opps) != 0; }
	UINT32 isPfrSbHuUnopened4To14MBot50() const { return (statsFlags5.value & eColHandStats5PfrSbHuUnopened4To14MBot50) != 0; }
	UINT32 isPfrSbHuUnopened4To14MBot50Opps() const { return (statsFlags5.value & eColHandStats5PfrSbHuUnopened4To14MBot50Opps) != 0; }
	UINT32 isPfrSbHuUnopened4To7MTop10() const { return (statsFlags5.value & eColHandStats5PfrSbHuUnopened4To7MTop10) != 0; }
	UINT32 isPfrSbHuUnopened4To7MTop10Opps() const { return (statsFlags5.value & eColHandStats5PfrSbHuUnopened4To7MTop10Opps) != 0; }
	UINT32 isPfrSbHuUnopened4To7MTop10To30() const { return (statsFlags5.value & eColHandStats5PfrSbHuUnopened4To7MTop10To30) != 0; }
	UINT32 isPfrSbHuUnopened4To7MTop10To30Opps() const { return (statsFlags5.value & eColHandStats5PfrSbHuUnopened4To7MTop10To30Opps) != 0; }
	UINT32 isPfrSbHuUnopened4To7MTop30To50() const { return (statsFlags5.value & eColHandStats5PfrSbHuUnopened4To7MTop30To50) != 0; }
	UINT32 isPfrSbHuUnopened4To7MTop30To50Opps() const { return (statsFlags5.value & eColHandStats5PfrSbHuUnopened4To7MTop30To50Opps) != 0; }
	UINT32 isPfrSbHuUnopened7To14MTop10() const { return (statsFlags5.value & eColHandStats5PfrSbHuUnopened7To14MTop10) != 0; }
	UINT32 isPfrSbHuUnopened7To14MTop10Opps() const { return (statsFlags5.value & eColHandStats5PfrSbHuUnopened7To14MTop10Opps) != 0; }
	UINT32 isPfrSbHuUnopened7To14MTop10To30() const { return (statsFlags5.value & eColHandStats5PfrSbHuUnopened7To14MTop10To30) != 0; }
	UINT32 isPfrSbHuUnopened7To14MTop10To30Opps() const { return (statsFlags5.value & eColHandStats5PfrSbHuUnopened7To14MTop10To30Opps) != 0; }
	UINT32 isPfrSbHuUnopened7To14MTop30To50() const { return (statsFlags5.value & eColHandStats5PfrSbHuUnopened7To14MTop30To50) != 0; }
	UINT32 isPfrSbHuUnopened7To14MTop30To50Opps() const { return (statsFlags5.value & eColHandStats5PfrSbHuUnopened7To14MTop30To50Opps) != 0; }
	UINT32 is_3BetBb10To14MTop10() const { return (statsFlags5.value & eColHandStats5_3BetBb10To14MTop10) != 0; }
	UINT32 is_3BetBb10To14MTop10Opps() const { return (statsFlags5.value & eColHandStats5_3BetBb10To14MTop10Opps) != 0; }
	UINT32 is_3BetBb10To14MTop10To30() const { return (statsFlags5.value & eColHandStats5_3BetBb10To14MTop10To30) != 0; }
	UINT32 is_3BetBb10To14MTop10To30Opps() const { return (statsFlags5.value & eColHandStats5_3BetBb10To14MTop10To30Opps) != 0; }
	UINT32 is_3BetBb10To14MTop30To50() const { return (statsFlags5.value & eColHandStats5_3BetBb10To14MTop30To50) != 0; }
	UINT32 is_3BetBb10To14MTop30To50Opps() const { return (statsFlags5.value & eColHandStats5_3BetBb10To14MTop30To50Opps) != 0; }
	UINT32 is_3BetBb14PlusMTop10() const { return (statsFlags5.value & eColHandStats5_3BetBb14PlusMTop10) != 0; }
	UINT32 is_3BetBb14PlusMTop10Opps() const { return (statsFlags5.value & eColHandStats5_3BetBb14PlusMTop10Opps) != 0; }
	UINT32 is_3BetBb14PlusMTop10To30() const { return (statsFlags5.value & eColHandStats5_3BetBb14PlusMTop10To30) != 0; }
	UINT32 is_3BetBb14PlusMTop10To30Opps() const { return (statsFlags5.value & eColHandStats5_3BetBb14PlusMTop10To30Opps) != 0; }
	UINT32 is_3BetBb4To7MTop10() const { return (statsFlags5.value & eColHandStats5_3BetBb4To7MTop10) != 0; }
	UINT32 is_3BetBb4To7MTop10Opps() const { return (statsFlags5.value & eColHandStats5_3BetBb4To7MTop10Opps) != 0; }
	UINT32 is_3BetBb4To7MTop10To30() const { return (statsFlags5.value & eColHandStats5_3BetBb4To7MTop10To30) != 0; }
	UINT32 is_3BetBb4To7MTop10To30Opps() const { return (statsFlags5.value & eColHandStats5_3BetBb4To7MTop10To30Opps) != 0; }
	UINT32 is_3BetBb4To7MTop30To50() const { return (statsFlags5.value & eColHandStats5_3BetBb4To7MTop30To50) != 0; }
	UINT32 is_3BetBb4To7MTop30To50Opps() const { return (statsFlags5.value & eColHandStats5_3BetBb4To7MTop30To50Opps) != 0; }
	UINT32 is_3BetBb7To10MTop10() const { return (statsFlags6.value & eColHandStats6_3BetBb7To10MTop10) != 0; }
	UINT32 is_3BetBb7To10MTop10Opps() const { return (statsFlags6.value & eColHandStats6_3BetBb7To10MTop10Opps) != 0; }
	UINT32 is_3BetBb7To10MTop10To30() const { return (statsFlags6.value & eColHandStats6_3BetBb7To10MTop10To30) != 0; }
	UINT32 is_3BetBb7To10MTop10To30Opps() const { return (statsFlags6.value & eColHandStats6_3BetBb7To10MTop10To30Opps) != 0; }
	UINT32 is_3BetBb7To10MTop30To50() const { return (statsFlags6.value & eColHandStats6_3BetBb7To10MTop30To50) != 0; }
	UINT32 is_3BetBb7To10MTop30To50Opps() const { return (statsFlags6.value & eColHandStats6_3BetBb7To10MTop30To50Opps) != 0; }
	UINT32 is_3BetBbHu10To14MTop10() const { return (statsFlags6.value & eColHandStats6_3BetBbHu10To14MTop10) != 0; }
	UINT32 is_3BetBbHu10To14MTop10Opps() const { return (statsFlags6.value & eColHandStats6_3BetBbHu10To14MTop10Opps) != 0; }
	UINT32 is_3BetBbHu10To14MTop10To30() const { return (statsFlags6.value & eColHandStats6_3BetBbHu10To14MTop10To30) != 0; }
	UINT32 is_3BetBbHu10To14MTop10To30Opps() const { return (statsFlags6.value & eColHandStats6_3BetBbHu10To14MTop10To30Opps) != 0; }
	UINT32 is_3BetBbHu10To14MTop30To50() const { return (statsFlags6.value & eColHandStats6_3BetBbHu10To14MTop30To50) != 0; }
	UINT32 is_3BetBbHu10To14MTop30To50Opps() const { return (statsFlags6.value & eColHandStats6_3BetBbHu10To14MTop30To50Opps) != 0; }
	UINT32 is_3BetBbHu14PlusMTop10() const { return (statsFlags6.value & eColHandStats6_3BetBbHu14PlusMTop10) != 0; }
	UINT32 is_3BetBbHu14PlusMTop10Opps() const { return (statsFlags6.value & eColHandStats6_3BetBbHu14PlusMTop10Opps) != 0; }
	UINT32 is_3BetBbHu14PlusMTop10To30() const { return (statsFlags6.value & eColHandStats6_3BetBbHu14PlusMTop10To30) != 0; }
	UINT32 is_3BetBbHu14PlusMTop10To30Opps() const { return (statsFlags6.value & eColHandStats6_3BetBbHu14PlusMTop10To30Opps) != 0; }
	UINT32 is_3BetBbHu14PlusMTop30To50() const { return (statsFlags6.value & eColHandStats6_3BetBbHu14PlusMTop30To50) != 0; }
	UINT32 is_3BetBbHu14PlusMTop30To50Opps() const { return (statsFlags6.value & eColHandStats6_3BetBbHu14PlusMTop30To50Opps) != 0; }
	UINT32 is_3BetBbHu4To7MTop10To30() const { return (statsFlags6.value & eColHandStats6_3BetBbHu4To7MTop10To30) != 0; }
	UINT32 is_3BetBbHu4To7MTop10To30Opps() const { return (statsFlags6.value & eColHandStats6_3BetBbHu4To7MTop10To30Opps) != 0; }
	UINT32 is_3BetBbHu4To7MTop30To50() const { return (statsFlags6.value & eColHandStats6_3BetBbHu4To7MTop30To50) != 0; }
	UINT32 is_3BetBbHu4To7MTop30To50Opps() const { return (statsFlags6.value & eColHandStats6_3BetBbHu4To7MTop30To50Opps) != 0; }
	UINT32 is_3BetBbHu7To10MTop10() const { return (statsFlags6.value & eColHandStats6_3BetBbHu7To10MTop10) != 0; }
	UINT32 is_3BetBbHu7To10MTop10Opps() const { return (statsFlags6.value & eColHandStats6_3BetBbHu7To10MTop10Opps) != 0; }
	UINT32 is_3BetBbHu7To10MTop10To30() const { return (statsFlags6.value & eColHandStats6_3BetBbHu7To10MTop10To30) != 0; }
	UINT32 is_3BetBbHu7To10MTop10To30Opps() const { return (statsFlags6.value & eColHandStats6_3BetBbHu7To10MTop10To30Opps) != 0; }
	UINT32 is_3BetBbHu7To10MTop30To50() const { return (statsFlags6.value & eColHandStats6_3BetBbHu7To10MTop30To50) != 0; }
	UINT32 is_3BetBbHu7To10MTop30To50Opps() const { return (statsFlags6.value & eColHandStats6_3BetBbHu7To10MTop30To50Opps) != 0; }
	UINT32 is_3BetBtn10To14MTop10() const { return (statsFlags6.value & eColHandStats6_3BetBtn10To14MTop10) != 0; }
	UINT32 is_3BetBtn10To14MTop10Opps() const { return (statsFlags6.value & eColHandStats6_3BetBtn10To14MTop10Opps) != 0; }
	UINT32 is_3BetBtn10To14MTop10To30() const { return (statsFlags6.value & eColHandStats6_3BetBtn10To14MTop10To30) != 0; }
	UINT32 is_3BetBtn10To14MTop10To30Opps() const { return (statsFlags6.value & eColHandStats6_3BetBtn10To14MTop10To30Opps) != 0; }
	UINT32 is_3BetBtn14PlusMTop10() const { return (statsFlags6.value & eColHandStats6_3BetBtn14PlusMTop10) != 0; }
	UINT32 is_3BetBtn14PlusMTop10Opps() const { return (statsFlags6.value & eColHandStats6_3BetBtn14PlusMTop10Opps) != 0; }
	UINT32 is_3BetBtn14PlusMTop10To30() const { return (statsFlags6.value & eColHandStats6_3BetBtn14PlusMTop10To30) != 0; }
	UINT32 is_3BetBtn14PlusMTop10To30Opps() const { return (statsFlags6.value & eColHandStats6_3BetBtn14PlusMTop10To30Opps) != 0; }
	UINT32 is_3BetBtn4To7MTop10() const { return (statsFlags6.value & eColHandStats6_3BetBtn4To7MTop10) != 0; }
	UINT32 is_3BetBtn4To7MTop10Opps() const { return (statsFlags6.value & eColHandStats6_3BetBtn4To7MTop10Opps) != 0; }
	UINT32 is_3BetBtn4To7MTop10To30() const { return (statsFlags6.value & eColHandStats6_3BetBtn4To7MTop10To30) != 0; }
	UINT32 is_3BetBtn4To7MTop10To30Opps() const { return (statsFlags6.value & eColHandStats6_3BetBtn4To7MTop10To30Opps) != 0; }
	UINT32 is_3BetBtn7To10MTop10() const { return (statsFlags6.value & eColHandStats6_3BetBtn7To10MTop10) != 0; }
	UINT32 is_3BetBtn7To10MTop10Opps() const { return (statsFlags6.value & eColHandStats6_3BetBtn7To10MTop10Opps) != 0; }
	UINT32 is_3BetBtn7To10MTop10To30() const { return (statsFlags6.value & eColHandStats6_3BetBtn7To10MTop10To30) != 0; }
	UINT32 is_3BetBtn7To10MTop10To30Opps() const { return (statsFlags6.value & eColHandStats6_3BetBtn7To10MTop10To30Opps) != 0; }
	UINT32 is_3BetCo10To14MTop10() const { return (statsFlags6.value & eColHandStats6_3BetCo10To14MTop10) != 0; }
	UINT32 is_3BetCo10To14MTop10Opps() const { return (statsFlags6.value & eColHandStats6_3BetCo10To14MTop10Opps) != 0; }
	UINT32 is_3BetCo14PlusMTop10To30() const { return (statsFlags6.value & eColHandStats6_3BetCo14PlusMTop10To30) != 0; }
	UINT32 is_3BetCo14PlusMTop10To30Opps() const { return (statsFlags6.value & eColHandStats6_3BetCo14PlusMTop10To30Opps) != 0; }
	UINT32 is_3BetCo4To7MTop10() const { return (statsFlags6.value & eColHandStats6_3BetCo4To7MTop10) != 0; }
	UINT32 is_3BetCo4To7MTop10Opps() const { return (statsFlags6.value & eColHandStats6_3BetCo4To7MTop10Opps) != 0; }
	UINT32 is_3BetCo7To10MTop10() const { return (statsFlags6.value & eColHandStats6_3BetCo7To10MTop10) != 0; }
	UINT32 is_3BetCo7To10MTop10Opps() const { return (statsFlags6.value & eColHandStats6_3BetCo7To10MTop10Opps) != 0; }
	UINT32 is_3BetCoMp14PlusMTop10() const { return (statsFlags6.value & eColHandStats6_3BetCoMp14PlusMTop10) != 0; }
	UINT32 is_3BetCoMp14PlusMTop10Opps() const { return (statsFlags6.value & eColHandStats6_3BetCoMp14PlusMTop10Opps) != 0; }
	UINT32 is_3BetMp10To14MTop10() const { return (statsFlags6.value & eColHandStats6_3BetMp10To14MTop10) != 0; }
	UINT32 is_3BetMp10To14MTop10Opps() const { return (statsFlags6.value & eColHandStats6_3BetMp10To14MTop10Opps) != 0; }
	UINT32 is_3BetMp7To10MTop10() const { return (statsFlags6.value & eColHandStats6_3BetMp7To10MTop10) != 0; }
	UINT32 is_3BetMp7To10MTop10Opps() const { return (statsFlags6.value & eColHandStats6_3BetMp7To10MTop10Opps) != 0; }
	UINT32 is_3BetSb10To14MTop10() const { return (statsFlags6.value & eColHandStats6_3BetSb10To14MTop10) != 0; }
	UINT32 is_3BetSb10To14MTop10Opps() const { return (statsFlags6.value & eColHandStats6_3BetSb10To14MTop10Opps) != 0; }
	UINT32 is_3BetSb10To14MTop10To30() const { return (statsFlags7.value & eColHandStats7_3BetSb10To14MTop10To30) != 0; }
	UINT32 is_3BetSb10To14MTop10To30Opps() const { return (statsFlags7.value & eColHandStats7_3BetSb10To14MTop10To30Opps) != 0; }
	UINT32 is_3BetSb14PlusMTop10() const { return (statsFlags7.value & eColHandStats7_3BetSb14PlusMTop10) != 0; }
	UINT32 is_3BetSb14PlusMTop10Opps() const { return (statsFlags7.value & eColHandStats7_3BetSb14PlusMTop10Opps) != 0; }
	UINT32 is_3BetSb14PlusMTop10To30() const { return (statsFlags7.value & eColHandStats7_3BetSb14PlusMTop10To30) != 0; }
	UINT32 is_3BetSb14PlusMTop10To30Opps() const { return (statsFlags7.value & eColHandStats7_3BetSb14PlusMTop10To30Opps) != 0; }
	UINT32 is_3BetSb4To7MTop10() const { return (statsFlags7.value & eColHandStats7_3BetSb4To7MTop10) != 0; }
	UINT32 is_3BetSb4To7MTop10Opps() const { return (statsFlags7.value & eColHandStats7_3BetSb4To7MTop10Opps) != 0; }
	UINT32 is_3BetSb4To7MTop10To30() const { return (statsFlags7.value & eColHandStats7_3BetSb4To7MTop10To30) != 0; }
	UINT32 is_3BetSb4To7MTop10To30Opps() const { return (statsFlags7.value & eColHandStats7_3BetSb4To7MTop10To30Opps) != 0; }
	UINT32 is_3BetSb4To7MTop30To50() const { return (statsFlags7.value & eColHandStats7_3BetSb4To7MTop30To50) != 0; }
	UINT32 is_3BetSb4To7MTop30To50Opps() const { return (statsFlags7.value & eColHandStats7_3BetSb4To7MTop30To50Opps) != 0; }
	UINT32 is_3BetSb7To10MTop10() const { return (statsFlags7.value & eColHandStats7_3BetSb7To10MTop10) != 0; }
	UINT32 is_3BetSb7To10MTop10Opps() const { return (statsFlags7.value & eColHandStats7_3BetSb7To10MTop10Opps) != 0; }
	UINT32 is_3BetSb7To10MTop10To30() const { return (statsFlags7.value & eColHandStats7_3BetSb7To10MTop10To30) != 0; }
	UINT32 is_3BetSb7To10MTop10To30Opps() const { return (statsFlags7.value & eColHandStats7_3BetSb7To10MTop10To30Opps) != 0; }
	UINT32 is_3BetSb7To10MTop30To50() const { return (statsFlags7.value & eColHandStats7_3BetSb7To10MTop30To50) != 0; }
	UINT32 is_3BetSb7To10MTop30To50Opps() const { return (statsFlags7.value & eColHandStats7_3BetSb7To10MTop30To50Opps) != 0; }
	UINT32 is_4BetBtn14PlusMTop10() const { return (statsFlags7.value & eColHandStats7_4BetBtn14PlusMTop10) != 0; }
	UINT32 is_4BetBtn14PlusMTop10Opps() const { return (statsFlags7.value & eColHandStats7_4BetBtn14PlusMTop10Opps) != 0; }
	UINT32 is_4BetBtn14PlusMTop10To30() const { return (statsFlags7.value & eColHandStats7_4BetBtn14PlusMTop10To30) != 0; }
	UINT32 is_4BetBtn14PlusMTop10To30Opps() const { return (statsFlags7.value & eColHandStats7_4BetBtn14PlusMTop10To30Opps) != 0; }
	UINT32 is_4BetCo14PlusMTop10to30() const { return (statsFlags7.value & eColHandStats7_4BetCo14PlusMTop10to30) != 0; }
	UINT32 is_4BetCo14PlusMTop10to30Opps() const { return (statsFlags7.value & eColHandStats7_4BetCo14PlusMTop10to30Opps) != 0; }
	UINT32 is_4BetCoMpEp14PlusMTop10() const { return (statsFlags7.value & eColHandStats7_4BetCoMpEp14PlusMTop10) != 0; }
	UINT32 is_4BetCoMpEp14PlusMTop10Opps() const { return (statsFlags7.value & eColHandStats7_4BetCoMpEp14PlusMTop10Opps) != 0; }
	UINT32 is_4BetMpEp14PlusMTop10To30() const { return (statsFlags7.value & eColHandStats7_4BetMpEp14PlusMTop10To30) != 0; }
	UINT32 is_4BetMpEp14PlusMTop10To30Opps() const { return (statsFlags7.value & eColHandStats7_4BetMpEp14PlusMTop10To30Opps) != 0; }
	UINT32 is_4BetSbHu14PlusMTop10() const { return (statsFlags7.value & eColHandStats7_4BetSbHu14PlusMTop10) != 0; }
	UINT32 is_4BetSbHu14PlusMTop10Opps() const { return (statsFlags7.value & eColHandStats7_4BetSbHu14PlusMTop10Opps) != 0; }
	UINT32 isDefendBb0To4MBot50vsSBRaise() const { return (statsFlags7.value & eColHandStats7DefendBb0To4MBot50vsSBRaise) != 0; }
	UINT32 isDefendBb0To4MBot50vsSBRaiseOpps() const { return (statsFlags7.value & eColHandStats7DefendBb0To4MBot50vsSBRaiseOpps) != 0; }
	UINT32 isDefendBb0To4MTop10To30vsCoMpEpRaise() const { return (statsFlags7.value & eColHandStats7DefendBb0To4MTop10To30vsCoMpEpRaise) != 0; }
	UINT32 isDefendBb0To4MTop10To30vsCoMpEpRaiseOpps() const { return (statsFlags7.value & eColHandStats7DefendBb0To4MTop10To30vsCoMpEpRaiseOpps) != 0; }
	UINT32 isDefendBb0To4MTop30To50vsCoMpEpRaise() const { return (statsFlags7.value & eColHandStats7DefendBb0To4MTop30To50vsCoMpEpRaise) != 0; }
	UINT32 isDefendBb0To4MTop30To50vsCoMpEpRaiseOpps() const { return (statsFlags7.value & eColHandStats7DefendBb0To4MTop30To50vsCoMpEpRaiseOpps) != 0; }
	UINT32 isDefendBb0To4MTop30To50vsSBRaise() const { return (statsFlags7.value & eColHandStats7DefendBb0To4MTop30To50vsSBRaise) != 0; }
	UINT32 isDefendBb0To4MTop30To50vsSBRaiseOpps() const { return (statsFlags7.value & eColHandStats7DefendBb0To4MTop30To50vsSBRaiseOpps) != 0; }
	UINT32 isDefendBb10To14MBot50vsCoMpEpRaise() const { return (statsFlags7.value & eColHandStats7DefendBb10To14MBot50vsCoMpEpRaise) != 0; }
	UINT32 isDefendBb10To14MBot50vsCoMpEpRaiseOpps() const { return (statsFlags7.value & eColHandStats7DefendBb10To14MBot50vsCoMpEpRaiseOpps) != 0; }
	UINT32 isDefendBb10To14MTop10To30vsBtnRaise() const { return (statsFlags7.value & eColHandStats7DefendBb10To14MTop10To30vsBtnRaise) != 0; }
	UINT32 isDefendBb10To14MTop10To30vsBtnRaiseOpps() const { return (statsFlags7.value & eColHandStats7DefendBb10To14MTop10To30vsBtnRaiseOpps) != 0; }
	UINT32 isDefendBb10To14MTop10To30vsCoMpEpRaise() const { return (statsFlags7.value & eColHandStats7DefendBb10To14MTop10To30vsCoMpEpRaise) != 0; }
	UINT32 isDefendBb10To14MTop10To30vsCoMpEpRaiseOpps() const { return (statsFlags7.value & eColHandStats7DefendBb10To14MTop10To30vsCoMpEpRaiseOpps) != 0; }
	UINT32 isDefendBb10To14MTop10To30vsSBRaise() const { return (statsFlags7.value & eColHandStats7DefendBb10To14MTop10To30vsSBRaise) != 0; }
	UINT32 isDefendBb10To14MTop10To30vsSBRaiseOpps() const { return (statsFlags7.value & eColHandStats7DefendBb10To14MTop10To30vsSBRaiseOpps) != 0; }
	UINT32 isDefendBb10To14MTop30To50vsBtnRaise() const { return (statsFlags7.value & eColHandStats7DefendBb10To14MTop30To50vsBtnRaise) != 0; }
	UINT32 isDefendBb10To14MTop30To50vsBtnRaiseOpps() const { return (statsFlags7.value & eColHandStats7DefendBb10To14MTop30To50vsBtnRaiseOpps) != 0; }
	UINT32 isDefendBb10To14MTop30To50vsCoMpEpRaise() const { return (statsFlags7.value & eColHandStats7DefendBb10To14MTop30To50vsCoMpEpRaise) != 0; }
	UINT32 isDefendBb10To14MTop30To50vsCoMpEpRaiseOpps() const { return (statsFlags7.value & eColHandStats7DefendBb10To14MTop30To50vsCoMpEpRaiseOpps) != 0; }
	UINT32 isDefendBb10To14MTop30To50vsSBRaise() const { return (statsFlags7.value & eColHandStats7DefendBb10To14MTop30To50vsSBRaise) != 0; }
	UINT32 isDefendBb10To14MTop30To50vsSBRaiseOpps() const { return (statsFlags7.value & eColHandStats7DefendBb10To14MTop30To50vsSBRaiseOpps) != 0; }
	UINT32 isDefendBb14PlusMBot50vsBtnRaise() const { return (statsFlags7.value & eColHandStats7DefendBb14PlusMBot50vsBtnRaise) != 0; }
	UINT32 isDefendBb14PlusMBot50vsBtnRaiseOpps() const { return (statsFlags7.value & eColHandStats7DefendBb14PlusMBot50vsBtnRaiseOpps) != 0; }
	UINT32 isDefendBb14PlusMTop10vsMpEpRaise() const { return (statsFlags7.value & eColHandStats7DefendBb14PlusMTop10vsMpEpRaise) != 0; }
	UINT32 isDefendBb14PlusMTop10vsMpEpRaiseOpps() const { return (statsFlags7.value & eColHandStats7DefendBb14PlusMTop10vsMpEpRaiseOpps) != 0; }
	UINT32 isDefendBb14PlusMTop10To30vsCoMpEpRaise() const { return (statsFlags7.value & eColHandStats7DefendBb14PlusMTop10To30vsCoMpEpRaise) != 0; }
	UINT32 isDefendBb14PlusMTop10To30vsCoMpEpRaiseOpps() const { return (statsFlags7.value & eColHandStats7DefendBb14PlusMTop10To30vsCoMpEpRaiseOpps) != 0; }
	UINT32 isDefendBb14PlusMTop10To30vsSBRaise() const { return (statsFlags7.value & eColHandStats7DefendBb14PlusMTop10To30vsSBRaise) != 0; }
	UINT32 isDefendBb14PlusMTop10To30vsSBRaiseOpps() const { return (statsFlags7.value & eColHandStats7DefendBb14PlusMTop10To30vsSBRaiseOpps) != 0; }
	UINT32 isDefendBb14PlusMTop10To50vsBtnRaise() const { return (statsFlags8.value & eColHandStats8DefendBb14PlusMTop10To50vsBtnRaise) != 0; }
	UINT32 isDefendBb14PlusMTop10To50vsBtnRaiseOpps() const { return (statsFlags8.value & eColHandStats8DefendBb14PlusMTop10To50vsBtnRaiseOpps) != 0; }
	UINT32 isDefendBb14PlusMTop30To100vsSBRaise() const { return (statsFlags8.value & eColHandStats8DefendBb14PlusMTop30To100vsSBRaise) != 0; }
	UINT32 isDefendBb14PlusMTop30To100vsSBRaiseOpps() const { return (statsFlags8.value & eColHandStats8DefendBb14PlusMTop30To100vsSBRaiseOpps) != 0; }
	UINT32 isDefendBb14PlusMTop30To50vsCoMpEpRaise() const { return (statsFlags8.value & eColHandStats8DefendBb14PlusMTop30To50vsCoMpEpRaise) != 0; }
	UINT32 isDefendBb14PlusMTop30To50vsCoMpEpRaiseOpps() const { return (statsFlags8.value & eColHandStats8DefendBb14PlusMTop30To50vsCoMpEpRaiseOpps) != 0; }
	UINT32 isDefendBb4To7MBot50vsSBRaise() const { return (statsFlags8.value & eColHandStats8DefendBb4To7MBot50vsSBRaise) != 0; }
	UINT32 isDefendBb4To7MBot50vsSBRaiseOpps() const { return (statsFlags8.value & eColHandStats8DefendBb4To7MBot50vsSBRaiseOpps) != 0; }
	UINT32 isDefendBb4To7MTop10To30vsBtnRaise() const { return (statsFlags8.value & eColHandStats8DefendBb4To7MTop10To30vsBtnRaise) != 0; }
	UINT32 isDefendBb4To7MTop10To30vsBtnRaiseOpps() const { return (statsFlags8.value & eColHandStats8DefendBb4To7MTop10To30vsBtnRaiseOpps) != 0; }
	UINT32 isDefendBb4To7MTop10To30vsCoRaise() const { return (statsFlags8.value & eColHandStats8DefendBb4To7MTop10To30vsCoRaise) != 0; }
	UINT32 isDefendBb4To7MTop10To30vsCoRaiseOpps() const { return (statsFlags8.value & eColHandStats8DefendBb4To7MTop10To30vsCoRaiseOpps) != 0; }
	UINT32 isDefendBb4To7MTop10To30vsMpEpRaise() const { return (statsFlags8.value & eColHandStats8DefendBb4To7MTop10To30vsMpEpRaise) != 0; }
	UINT32 isDefendBb4To7MTop10To30vsMpEpRaiseOpps() const { return (statsFlags8.value & eColHandStats8DefendBb4To7MTop10To30vsMpEpRaiseOpps) != 0; }
	UINT32 isDefendBb4To7MTop10To30vsSBRaise() const { return (statsFlags8.value & eColHandStats8DefendBb4To7MTop10To30vsSBRaise) != 0; }
	UINT32 isDefendBb4To7MTop10To30vsSBRaiseOpps() const { return (statsFlags8.value & eColHandStats8DefendBb4To7MTop10To30vsSBRaiseOpps) != 0; }
	UINT32 isDefendBb4To7MTop30To50vsBtnRaise() const { return (statsFlags8.value & eColHandStats8DefendBb4To7MTop30To50vsBtnRaise) != 0; }
	UINT32 isDefendBb4To7MTop30To50vsBtnRaiseOpps() const { return (statsFlags8.value & eColHandStats8DefendBb4To7MTop30To50vsBtnRaiseOpps) != 0; }
	UINT32 isDefendBb4To7MTop30To50vsCoMpEpRaise() const { return (statsFlags8.value & eColHandStats8DefendBb4To7MTop30To50vsCoMpEpRaise) != 0; }
	UINT32 isDefendBb4To7MTop30To50vsCoMpEpRaiseOpps() const { return (statsFlags8.value & eColHandStats8DefendBb4To7MTop30To50vsCoMpEpRaiseOpps) != 0; }
	UINT32 isDefendBb4To7MTop30To50vsSBRaise() const { return (statsFlags8.value & eColHandStats8DefendBb4To7MTop30To50vsSBRaise) != 0; }
	UINT32 isDefendBb4To7MTop30To50vsSBRaiseOpps() const { return (statsFlags8.value & eColHandStats8DefendBb4To7MTop30To50vsSBRaiseOpps) != 0; }
	UINT32 isDefendBb7To10MBot50vsCoMpEpRaise() const { return (statsFlags8.value & eColHandStats8DefendBb7To10MBot50vsCoMpEpRaise) != 0; }
	UINT32 isDefendBb7To10MBot50vsCoMpEpRaiseOpps() const { return (statsFlags8.value & eColHandStats8DefendBb7To10MBot50vsCoMpEpRaiseOpps) != 0; }
	UINT32 isDefendBb7To10MTop10To30vsBtnRaise() const { return (statsFlags8.value & eColHandStats8DefendBb7To10MTop10To30vsBtnRaise) != 0; }
	UINT32 isDefendBb7To10MTop10To30vsBtnRaiseOpps() const { return (statsFlags8.value & eColHandStats8DefendBb7To10MTop10To30vsBtnRaiseOpps) != 0; }
	UINT32 isDefendBb7To10MTop10To30vsCoRaise() const { return (statsFlags8.value & eColHandStats8DefendBb7To10MTop10To30vsCoRaise) != 0; }
	UINT32 isDefendBb7To10MTop10To30vsCoRaiseOpps() const { return (statsFlags8.value & eColHandStats8DefendBb7To10MTop10To30vsCoRaiseOpps) != 0; }
	UINT32 isDefendBb7To10MTop10To30vsMpEpRaise() const { return (statsFlags8.value & eColHandStats8DefendBb7To10MTop10To30vsMpEpRaise) != 0; }
	UINT32 isDefendBb7To10MTop10To30vsMpEpRaiseOpps() const { return (statsFlags8.value & eColHandStats8DefendBb7To10MTop10To30vsMpEpRaiseOpps) != 0; }
	UINT32 isDefendBb7To10MTop10To30vsSBRaise() const { return (statsFlags8.value & eColHandStats8DefendBb7To10MTop10To30vsSBRaise) != 0; }
	UINT32 isDefendBb7To10MTop10To30vsSBRaiseOpps() const { return (statsFlags8.value & eColHandStats8DefendBb7To10MTop10To30vsSBRaiseOpps) != 0; }
	UINT32 isDefendBb7To10MTop30To50vsBtnRaise() const { return (statsFlags8.value & eColHandStats8DefendBb7To10MTop30To50vsBtnRaise) != 0; }
	UINT32 isDefendBb7To10MTop30To50vsBtnRaiseOpps() const { return (statsFlags8.value & eColHandStats8DefendBb7To10MTop30To50vsBtnRaiseOpps) != 0; }
	UINT32 isDefendBb7To10MTop30To50vsCoMpEpRaise() const { return (statsFlags8.value & eColHandStats8DefendBb7To10MTop30To50vsCoMpEpRaise) != 0; }
	UINT32 isDefendBb7To10MTop30To50vsCoMpEpRaiseOpps() const { return (statsFlags8.value & eColHandStats8DefendBb7To10MTop30To50vsCoMpEpRaiseOpps) != 0; }
	UINT32 isDefendBb7To10MTop30To50vsSBRaise() const { return (statsFlags8.value & eColHandStats8DefendBb7To10MTop30To50vsSBRaise) != 0; }
	UINT32 isDefendBb7To10MTop30To50vsSBRaiseOpps() const { return (statsFlags8.value & eColHandStats8DefendBb7To10MTop30To50vsSBRaiseOpps) != 0; }
	UINT32 isDefendBtn14PlusMTop10vsCoMpEpRaise() const { return (statsFlags8.value & eColHandStats8DefendBtn14PlusMTop10vsCoMpEpRaise) != 0; }
	UINT32 isDefendBtn14PlusMTop10vsCoMpEpRaiseOpps() const { return (statsFlags8.value & eColHandStats8DefendBtn14PlusMTop10vsCoMpEpRaiseOpps) != 0; }
	UINT32 isDefendBtn14PlusMTop10To30vsCoMpEpRaise() const { return (statsFlags8.value & eColHandStats8DefendBtn14PlusMTop10To30vsCoMpEpRaise) != 0; }
	UINT32 isDefendBtn14PlusMTop10To30vsCoMpEpRaiseOpps() const { return (statsFlags8.value & eColHandStats8DefendBtn14PlusMTop10To30vsCoMpEpRaiseOpps) != 0; }
	UINT32 isDefendSb0To4MTop10vsCoMpEpRaise() const { return (statsFlags8.value & eColHandStats8DefendSb0To4MTop10vsCoMpEpRaise) != 0; }
	UINT32 isDefendSb0To4MTop10vsCoMpEpRaiseOpps() const { return (statsFlags8.value & eColHandStats8DefendSb0To4MTop10vsCoMpEpRaiseOpps) != 0; }
	UINT32 isDefendSb0To4MTop10To30vsCoMpEpRaise() const { return (statsFlags8.value & eColHandStats8DefendSb0To4MTop10To30vsCoMpEpRaise) != 0; }
	UINT32 isDefendSb0To4MTop10To30vsCoMpEpRaiseOpps() const { return (statsFlags8.value & eColHandStats8DefendSb0To4MTop10To30vsCoMpEpRaiseOpps) != 0; }
	UINT32 isDefendSb10To14MTop10vsCoRaise() const { return (statsFlags8.value & eColHandStats8DefendSb10To14MTop10vsCoRaise) != 0; }
	UINT32 isDefendSb10To14MTop10vsCoRaiseOpps() const { return (statsFlags8.value & eColHandStats8DefendSb10To14MTop10vsCoRaiseOpps) != 0; }
	UINT32 isDefendSb10To14MTop10vsMpEpRaise() const { return (statsFlags8.value & eColHandStats8DefendSb10To14MTop10vsMpEpRaise) != 0; }
	UINT32 isDefendSb10To14MTop10vsMpEpRaiseOpps() const { return (statsFlags8.value & eColHandStats8DefendSb10To14MTop10vsMpEpRaiseOpps) != 0; }
	UINT32 isDefendSb10To14MTop10To30vsCoRaise() const { return (statsFlags8.value & eColHandStats8DefendSb10To14MTop10To30vsCoRaise) != 0; }
	UINT32 isDefendSb10To14MTop10To30vsCoRaiseOpps() const { return (statsFlags8.value & eColHandStats8DefendSb10To14MTop10To30vsCoRaiseOpps) != 0; }
	UINT32 isDefendSb10To14MTop10To30vsMpEpRaise() const { return (statsFlags8.value & eColHandStats8DefendSb10To14MTop10To30vsMpEpRaise) != 0; }
	UINT32 isDefendSb10To14MTop10To30vsMpEpRaiseOpps() const { return (statsFlags8.value & eColHandStats8DefendSb10To14MTop10To30vsMpEpRaiseOpps) != 0; }
	UINT32 isDefendSb14PlusMTop10vsCoMpEpRaise() const { return (statsFlags8.value & eColHandStats8DefendSb14PlusMTop10vsCoMpEpRaise) != 0; }
	UINT32 isDefendSb14PlusMTop10vsCoMpEpRaiseOpps() const { return (statsFlags8.value & eColHandStats8DefendSb14PlusMTop10vsCoMpEpRaiseOpps) != 0; }
	UINT32 isDefendSb14PlusMTop10To30vsBtnRaise() const { return (statsFlags8.value & eColHandStats8DefendSb14PlusMTop10To30vsBtnRaise) != 0; }
	UINT32 isDefendSb14PlusMTop10To30vsBtnRaiseOpps() const { return (statsFlags8.value & eColHandStats8DefendSb14PlusMTop10To30vsBtnRaiseOpps) != 0; }
	UINT32 isDefendSb14PlusMTop10To30vsCoMpEpRaise() const { return (statsFlags8.value & eColHandStats8DefendSb14PlusMTop10To30vsCoMpEpRaise) != 0; }
	UINT32 isDefendSb14PlusMTop10To30vsCoMpEpRaiseOpps() const { return (statsFlags8.value & eColHandStats8DefendSb14PlusMTop10To30vsCoMpEpRaiseOpps) != 0; }
	UINT32 isDefendSb14PlusMTop30To50vsBtnRaise() const { return (statsFlags9.value & eColHandStats9DefendSb14PlusMTop30To50vsBtnRaise) != 0; }
	UINT32 isDefendSb14PlusMTop30To50vsBtnRaiseOpps() const { return (statsFlags9.value & eColHandStats9DefendSb14PlusMTop30To50vsBtnRaiseOpps) != 0; }
	UINT32 isDefendSb14PlusMTop30To50vsCoMpEpRaise() const { return (statsFlags9.value & eColHandStats9DefendSb14PlusMTop30To50vsCoMpEpRaise) != 0; }
	UINT32 isDefendSb14PlusMTop30To50vsCoMpEpRaiseOpps() const { return (statsFlags9.value & eColHandStats9DefendSb14PlusMTop30To50vsCoMpEpRaiseOpps) != 0; }
	UINT32 isDefendSb4To7MTop10vsCoMpEpRaise() const { return (statsFlags9.value & eColHandStats9DefendSb4To7MTop10vsCoMpEpRaise) != 0; }
	UINT32 isDefendSb4To7MTop10vsCoMpEpRaiseOpps() const { return (statsFlags9.value & eColHandStats9DefendSb4To7MTop10vsCoMpEpRaiseOpps) != 0; }
	UINT32 isDefendSb4To7MTop10To30vsBtnRaise() const { return (statsFlags9.value & eColHandStats9DefendSb4To7MTop10To30vsBtnRaise) != 0; }
	UINT32 isDefendSb4To7MTop10To30vsBtnRaiseOpps() const { return (statsFlags9.value & eColHandStats9DefendSb4To7MTop10To30vsBtnRaiseOpps) != 0; }
	UINT32 isDefendSb4To7MTop10To30vsCoMpEpRaise() const { return (statsFlags9.value & eColHandStats9DefendSb4To7MTop10To30vsCoMpEpRaise) != 0; }
	UINT32 isDefendSb4To7MTop10To30vsCoMpEpRaiseOpps() const { return (statsFlags9.value & eColHandStats9DefendSb4To7MTop10To30vsCoMpEpRaiseOpps) != 0; }
	UINT32 isDefendSb4To7MTop30To50vsBtnRaise() const { return (statsFlags9.value & eColHandStats9DefendSb4To7MTop30To50vsBtnRaise) != 0; }
	UINT32 isDefendSb4To7MTop30To50vsBtnRaiseOpps() const { return (statsFlags9.value & eColHandStats9DefendSb4To7MTop30To50vsBtnRaiseOpps) != 0; }
	UINT32 isDefendSb7To10MTop10vsCoRaise() const { return (statsFlags9.value & eColHandStats9DefendSb7To10MTop10vsCoRaise) != 0; }
	UINT32 isDefendSb7To10MTop10vsCoRaiseOpps() const { return (statsFlags9.value & eColHandStats9DefendSb7To10MTop10vsCoRaiseOpps) != 0; }
	UINT32 isDefendSb7To10MTop10vsMpEpRaise() const { return (statsFlags9.value & eColHandStats9DefendSb7To10MTop10vsMpEpRaise) != 0; }
	UINT32 isDefendSb7To10MTop10vsMpEpRaiseOpps() const { return (statsFlags9.value & eColHandStats9DefendSb7To10MTop10vsMpEpRaiseOpps) != 0; }
	UINT32 isDefendSb7To10MTop10To30vsCoRaise() const { return (statsFlags9.value & eColHandStats9DefendSb7To10MTop10To30vsCoRaise) != 0; }
	UINT32 isDefendSb7To10MTop10To30vsCoRaiseOpps() const { return (statsFlags9.value & eColHandStats9DefendSb7To10MTop10To30vsCoRaiseOpps) != 0; }
	UINT32 isDefendSb7To10MTop10To30vsMpEpRaise() const { return (statsFlags9.value & eColHandStats9DefendSb7To10MTop10To30vsMpEpRaise) != 0; }
	UINT32 isDefendSb7To10MTop10To30vsMpEpRaiseOpps() const { return (statsFlags9.value & eColHandStats9DefendSb7To10MTop10To30vsMpEpRaiseOpps) != 0; }
	UINT32 isBetSizePreflopTotal() const { return (statsFlags9.value & eColHandStats9BetSizePreflopTotal) != 0; }
	UINT32 isBetSizePreflop2To2_05BB() const { return (statsFlags9.value & eColHandStats9BetSizePreflop2To2_05BB) != 0; }
	UINT32 isBetSizePreflop2_05To3BB() const { return (statsFlags9.value & eColHandStats9BetSizePreflop2_05To3BB) != 0; }
	UINT32 isBetSizePreflop3To5BB() const { return (statsFlags9.value & eColHandStats9BetSizePreflop3To5BB) != 0; }
	UINT32 isBetSizePreflop5PlusBB() const { return (statsFlags9.value & eColHandStats9BetSizePreflop5PlusBB) != 0; }
	UINT32 isBetSize0To40OnFlop() const { return (statsFlags9.value & eColHandStats9BetSize0To40OnFlop) != 0; }
	UINT32 isBetSize0To40OnTurn() const { return (statsFlags9.value & eColHandStats9BetSize0To40OnTurn) != 0; }
	UINT32 isBetSize0To40OnRiver() const { return (statsFlags9.value & eColHandStats9BetSize0To40OnRiver) != 0; }
	UINT32 isBetSize0To40On7th() const { return (statsFlags9.value & eColHandStats9BetSize0To40On7th) != 0; }
	UINT32 isBetSize40To49_5OnFlop() const { return (statsFlags9.value & eColHandStats9BetSize40To49_5OnFlop) != 0; }
	UINT32 isBetSize40To49_5OnTurn() const { return (statsFlags9.value & eColHandStats9BetSize40To49_5OnTurn) != 0; }
	UINT32 isBetSize40To49_5OnRiver() const { return (statsFlags9.value & eColHandStats9BetSize40To49_5OnRiver) != 0; }
	UINT32 isBetSize40To49_5On7th() const { return (statsFlags9.value & eColHandStats9BetSize40To49_5On7th) != 0; }
	UINT32 isBetSize49_5To50_5OnFlop() const { return (statsFlags9.value & eColHandStats9BetSize49_5To50_5OnFlop) != 0; }
	UINT32 isBetSize49_5To50_5OnTurn() const { return (statsFlags9.value & eColHandStats9BetSize49_5To50_5OnTurn) != 0; }
	UINT32 isBetSize49_5To50_5OnRiver() const { return (statsFlags9.value & eColHandStats9BetSize49_5To50_5OnRiver) != 0; }
	UINT32 isBetSize49_5To50_5On7th() const { return (statsFlags9.value & eColHandStats9BetSize49_5To50_5On7th) != 0; }
	UINT32 isBetSize50_5To60OnFlop() const { return (statsFlags9.value & eColHandStats9BetSize50_5To60OnFlop) != 0; }
	UINT32 isBetSize50_5To60OnTurn() const { return (statsFlags9.value & eColHandStats9BetSize50_5To60OnTurn) != 0; }
	UINT32 isBetSize50_5To60OnRiver() const { return (statsFlags9.value & eColHandStats9BetSize50_5To60OnRiver) != 0; }
	UINT32 isBetSize50_5To60On7th() const { return (statsFlags9.value & eColHandStats9BetSize50_5To60On7th) != 0; }
	UINT32 isBetSize60To70OnFlop() const { return (statsFlags9.value & eColHandStats9BetSize60To70OnFlop) != 0; }
	UINT32 isBetSize60To70OnTurn() const { return (statsFlags9.value & eColHandStats9BetSize60To70OnTurn) != 0; }
	UINT32 isBetSize60To70OnRiver() const { return (statsFlags9.value & eColHandStats9BetSize60To70OnRiver) != 0; }
	UINT32 isBetSize60To70On7th() const { return (statsFlags9.value & eColHandStats9BetSize60To70On7th) != 0; }
	UINT32 isBetSize70To99_5OnFlop() const { return (statsFlags9.value & eColHandStats9BetSize70To99_5OnFlop) != 0; }
	UINT32 isBetSize70To99_5OnTurn() const { return (statsFlags9.value & eColHandStats9BetSize70To99_5OnTurn) != 0; }
	UINT32 isBetSize70To99_5OnRiver() const { return (statsFlags9.value & eColHandStats9BetSize70To99_5OnRiver) != 0; }
	UINT32 isBetSize70To99_5On7th() const { return (statsFlags9.value & eColHandStats9BetSize70To99_5On7th) != 0; }
	UINT32 isBetSize99_5To100_5OnFlop() const { return (statsFlags9.value & eColHandStats9BetSize99_5To100_5OnFlop) != 0; }
	UINT32 isBetSize99_5To100_5OnTurn() const { return (statsFlags9.value & eColHandStats9BetSize99_5To100_5OnTurn) != 0; }
	UINT32 isBetSize99_5To100_5OnRiver() const { return (statsFlags9.value & eColHandStats9BetSize99_5To100_5OnRiver) != 0; }
	UINT32 isBetSize99_5To100_5On7th() const { return (statsFlags9.value & eColHandStats9BetSize99_5To100_5On7th) != 0; }
	UINT32 isBetSize100_5PlusOnFlop() const { return (statsFlags9.value & eColHandStats9BetSize100_5PlusOnFlop) != 0; }
	UINT32 isBetSize100_5PlusOnTurn() const { return (statsFlags9.value & eColHandStats9BetSize100_5PlusOnTurn) != 0; }
	UINT32 isBetSize100_5PlusOnRiver() const { return (statsFlags9.value & eColHandStats9BetSize100_5PlusOnRiver) != 0; }
	UINT32 isBetSize100_5PlusOn7th() const { return (statsFlags9.value & eColHandStats9BetSize100_5PlusOn7th) != 0; }
	UINT32 isPfrBbHuLimped14PlusMRaiseTo2To3BB() const { return (statsFlags9.value & eColHandStats9PfrBbHuLimped14PlusMRaiseTo2To3BB) != 0; }
	UINT32 isPfrBbHuLimped14PlusMRaiseTo3To3_5BB() const { return (statsFlags9.value & eColHandStats9PfrBbHuLimped14PlusMRaiseTo3To3_5BB) != 0; }
	UINT32 isPfrBbHuLimped14PlusMRaiseTo3_5To4BB() const { return (statsFlags10.value & eColHandStats10PfrBbHuLimped14PlusMRaiseTo3_5To4BB) != 0; }
	UINT32 isPfrBbHuLimped14PlusMRaiseTo4To4_5BB() const { return (statsFlags10.value & eColHandStats10PfrBbHuLimped14PlusMRaiseTo4To4_5BB) != 0; }
	UINT32 isPfrBbHuLimped14PlusMRaiseTo4_5To5BB() const { return (statsFlags10.value & eColHandStats10PfrBbHuLimped14PlusMRaiseTo4_5To5BB) != 0; }
	UINT32 isPfrBbHuLimped14PlusMRaiseTo5To6BB() const { return (statsFlags10.value & eColHandStats10PfrBbHuLimped14PlusMRaiseTo5To6BB) != 0; }
	UINT32 isPfrBbHuLimped14PlusMRaiseTo6To7BB() const { return (statsFlags10.value & eColHandStats10PfrBbHuLimped14PlusMRaiseTo6To7BB) != 0; }
	UINT32 isPfrBbHuLimped14PlusMRaiseTo7To10BB() const { return (statsFlags10.value & eColHandStats10PfrBbHuLimped14PlusMRaiseTo7To10BB) != 0; }
	UINT32 isPfrBbHuLimped14PlusMRaiseTo10PlusBB() const { return (statsFlags10.value & eColHandStats10PfrBbHuLimped14PlusMRaiseTo10PlusBB) != 0; }
	UINT32 isPfrBbHuMicroRaised14PlusMRaiseTo3To5BB() const { return (statsFlags10.value & eColHandStats10PfrBbHuMicroRaised14PlusMRaiseTo3To5BB) != 0; }
	UINT32 isPfrBbHuMicroRaised14PlusMRaiseTo5To6BB() const { return (statsFlags10.value & eColHandStats10PfrBbHuMicroRaised14PlusMRaiseTo5To6BB) != 0; }
	UINT32 isPfrBbHuMicroRaised14PlusMRaiseTo6To7BB() const { return (statsFlags10.value & eColHandStats10PfrBbHuMicroRaised14PlusMRaiseTo6To7BB) != 0; }
	UINT32 isPfrBbHuMicroRaised14PlusMRaiseTo7To10BB() const { return (statsFlags10.value & eColHandStats10PfrBbHuMicroRaised14PlusMRaiseTo7To10BB) != 0; }
	UINT32 isPfrBbHuMicroRaised14PlusMRaiseTo10PlusBB() const { return (statsFlags10.value & eColHandStats10PfrBbHuMicroRaised14PlusMRaiseTo10PlusBB) != 0; }
	UINT32 isPfrBbHuLimped7To10MRaiseTo2To3BB() const { return (statsFlags10.value & eColHandStats10PfrBbHuLimped7To10MRaiseTo2To3BB) != 0; }
	UINT32 isPfrBbHuLimped7To10MRaiseTo3To3_5BB() const { return (statsFlags10.value & eColHandStats10PfrBbHuLimped7To10MRaiseTo3To3_5BB) != 0; }
	UINT32 isPfrBbHuLimped7To10MRaiseTo3_5To4BB() const { return (statsFlags10.value & eColHandStats10PfrBbHuLimped7To10MRaiseTo3_5To4BB) != 0; }
	UINT32 isPfrBbHuLimped7To10MRaiseTo4To4_5BB() const { return (statsFlags10.value & eColHandStats10PfrBbHuLimped7To10MRaiseTo4To4_5BB) != 0; }
	UINT32 isPfrBbHuLimped7To10MRaiseTo4_5To5BB() const { return (statsFlags10.value & eColHandStats10PfrBbHuLimped7To10MRaiseTo4_5To5BB) != 0; }
	UINT32 isPfrBbHuLimped7To10MRaiseTo5To6BB() const { return (statsFlags10.value & eColHandStats10PfrBbHuLimped7To10MRaiseTo5To6BB) != 0; }
	UINT32 isPfrBbHuLimped7To10MRaiseTo6To10BB() const { return (statsFlags10.value & eColHandStats10PfrBbHuLimped7To10MRaiseTo6To10BB) != 0; }
	UINT32 isPfrBbHuLimped7To10MRaiseTo10PlusBB() const { return (statsFlags10.value & eColHandStats10PfrBbHuLimped7To10MRaiseTo10PlusBB) != 0; }
	UINT32 isFoldTo3BetHEM_Btn10To14MTop10Opps() const { return (statsFlags10.value & eColHandStats10FoldTo3BetHEM_Btn10To14MTop10Opps) != 0; }
	UINT32 isFoldTo3BetHEM_Btn10To14MTop10To30Opps() const { return (statsFlags10.value & eColHandStats10FoldTo3BetHEM_Btn10To14MTop10To30Opps) != 0; }
	UINT32 isFoldTo3BetHEM_Btn10To14MTop30To50Opps() const { return (statsFlags10.value & eColHandStats10FoldTo3BetHEM_Btn10To14MTop30To50Opps) != 0; }
	UINT32 isFoldTo3BetHEM_Btn14PlusMTop10Opps() const { return (statsFlags10.value & eColHandStats10FoldTo3BetHEM_Btn14PlusMTop10Opps) != 0; }
	UINT32 isFoldTo3BetHEM_Btn14PlusMTop10To30Opps() const { return (statsFlags10.value & eColHandStats10FoldTo3BetHEM_Btn14PlusMTop10To30Opps) != 0; }
	UINT32 isFoldTo3BetHEM_Btn7To10MTop30Opps() const { return (statsFlags10.value & eColHandStats10FoldTo3BetHEM_Btn7To10MTop30Opps) != 0; }
	UINT32 isFoldTo3BetHEM_Co14PlusMTop10Opps() const { return (statsFlags10.value & eColHandStats10FoldTo3BetHEM_Co14PlusMTop10Opps) != 0; }
	UINT32 isFoldTo3BetHEM_Co7To14MTop10Opps() const { return (statsFlags10.value & eColHandStats10FoldTo3BetHEM_Co7To14MTop10Opps) != 0; }
	UINT32 isFoldTo3BetHEM_Co7To14MTop10To30Opps() const { return (statsFlags10.value & eColHandStats10FoldTo3BetHEM_Co7To14MTop10To30Opps) != 0; }
	UINT32 isFoldTo3BetHEM_CoMpEp14PlusMTop10To30Opps() const { return (statsFlags10.value & eColHandStats10FoldTo3BetHEM_CoMpEp14PlusMTop10To30Opps) != 0; }
	UINT32 isFoldTo3BetHEM_Ep7To14MTop10Opps() const { return (statsFlags10.value & eColHandStats10FoldTo3BetHEM_Ep7To14MTop10Opps) != 0; }
	UINT32 isFoldTo3BetHEM_Ep7To14MTop10To30Opps() const { return (statsFlags10.value & eColHandStats10FoldTo3BetHEM_Ep7To14MTop10To30Opps) != 0; }
	UINT32 isFoldTo3BetHEM_Mp7To14MTop10Opps() const { return (statsFlags10.value & eColHandStats10FoldTo3BetHEM_Mp7To14MTop10Opps) != 0; }
	UINT32 isFoldTo3BetHEM_Mp7To14MTop10To30Opps() const { return (statsFlags10.value & eColHandStats10FoldTo3BetHEM_Mp7To14MTop10To30Opps) != 0; }
	UINT32 isFoldTo3BetHEM_MpEp14PlusMTop10Opps() const { return (statsFlags10.value & eColHandStats10FoldTo3BetHEM_MpEp14PlusMTop10Opps) != 0; }
	UINT32 isFoldTo3BetHEM_Sb7To14MTop10Opps() const { return (statsFlags10.value & eColHandStats10FoldTo3BetHEM_Sb7To14MTop10Opps) != 0; }
	UINT32 isFoldTo3BetHEM_Sb7To14MTop10To30Opps() const { return (statsFlags10.value & eColHandStats10FoldTo3BetHEM_Sb7To14MTop10To30Opps) != 0; }
	UINT32 isFoldTo3BetHEM_SbHu10To14MTop10To30Opps() const { return (statsFlags10.value & eColHandStats10FoldTo3BetHEM_SbHu10To14MTop10To30Opps) != 0; }
	UINT32 isFoldTo3BetHEM_SbHu10To14MTop30To50Opps() const { return (statsFlags10.value & eColHandStats10FoldTo3BetHEM_SbHu10To14MTop30To50Opps) != 0; }
	UINT32 isFoldTo3BetHEM_SbHu14PlusMBot50Opps() const { return (statsFlags10.value & eColHandStats10FoldTo3BetHEM_SbHu14PlusMBot50Opps) != 0; }
	UINT32 isFoldTo3BetHEM_SbHu14PlusMTop10To30Opps() const { return (statsFlags10.value & eColHandStats10FoldTo3BetHEM_SbHu14PlusMTop10To30Opps) != 0; }
	UINT32 isFoldTo3BetHEM_SbHu14PlusMTop30To50Opps() const { return (statsFlags10.value & eColHandStats10FoldTo3BetHEM_SbHu14PlusMTop30To50Opps) != 0; }
	UINT32 isFoldTo3BetHEM_SbHu7To10MTop10To50Opps() const { return (statsFlags10.value & eColHandStats10FoldTo3BetHEM_SbHu7To10MTop10To50Opps) != 0; }
	UINT32 isFoldTo3BetHEM_Btn10To14MTop10() const { return (statsFlags10.value & eColHandStats10FoldTo3BetHEM_Btn10To14MTop10) != 0; }
	UINT32 isFoldTo3BetHEM_Btn10To14MTop10To30() const { return (statsFlags10.value & eColHandStats10FoldTo3BetHEM_Btn10To14MTop10To30) != 0; }
	UINT32 isFoldTo3BetHEM_Btn10To14MTop30To50() const { return (statsFlags10.value & eColHandStats10FoldTo3BetHEM_Btn10To14MTop30To50) != 0; }
	UINT32 isFoldTo3BetHEM_Btn14PlusMTop10() const { return (statsFlags10.value & eColHandStats10FoldTo3BetHEM_Btn14PlusMTop10) != 0; }
	UINT32 isFoldTo3BetHEM_Btn14PlusMTop10To30() const { return (statsFlags10.value & eColHandStats10FoldTo3BetHEM_Btn14PlusMTop10To30) != 0; }
	UINT32 isFoldTo3BetHEM_Btn7To10MTop30() const { return (statsFlags10.value & eColHandStats10FoldTo3BetHEM_Btn7To10MTop30) != 0; }
	UINT32 isFoldTo3BetHEM_Co14PlusMTop10() const { return (statsFlags10.value & eColHandStats10FoldTo3BetHEM_Co14PlusMTop10) != 0; }
	UINT32 isFoldTo3BetHEM_Co7To14MTop10() const { return (statsFlags10.value & eColHandStats10FoldTo3BetHEM_Co7To14MTop10) != 0; }
	UINT32 isFoldTo3BetHEM_Co7To14MTop10To30() const { return (statsFlags10.value & eColHandStats10FoldTo3BetHEM_Co7To14MTop10To30) != 0; }
	UINT32 isFoldTo3BetHEM_CoMpEp14PlusMTop10To30() const { return (statsFlags10.value & eColHandStats10FoldTo3BetHEM_CoMpEp14PlusMTop10To30) != 0; }
	UINT32 isFoldTo3BetHEM_Ep7To14MTop10() const { return (statsFlags10.value & eColHandStats10FoldTo3BetHEM_Ep7To14MTop10) != 0; }
	UINT32 isFoldTo3BetHEM_Ep7To14MTop10To30() const { return (statsFlags10.value & eColHandStats10FoldTo3BetHEM_Ep7To14MTop10To30) != 0; }
	UINT32 isFoldTo3BetHEM_Mp7To14MTop10() const { return (statsFlags10.value & eColHandStats10FoldTo3BetHEM_Mp7To14MTop10) != 0; }
	UINT32 isFoldTo3BetHEM_Mp7To14MTop10To30() const { return (statsFlags10.value & eColHandStats10FoldTo3BetHEM_Mp7To14MTop10To30) != 0; }
	UINT32 isFoldTo3BetHEM_MpEp14PlusMTop10() const { return (statsFlags10.value & eColHandStats10FoldTo3BetHEM_MpEp14PlusMTop10) != 0; }
	UINT32 isFoldTo3BetHEM_Sb7To14MTop10() const { return (statsFlags10.value & eColHandStats10FoldTo3BetHEM_Sb7To14MTop10) != 0; }
	UINT32 isFoldTo3BetHEM_Sb7To14MTop10To30() const { return (statsFlags10.value & eColHandStats10FoldTo3BetHEM_Sb7To14MTop10To30) != 0; }
	UINT32 isFoldTo3BetHEM_SbHu10To14MTop10To30() const { return (statsFlags11.value & eColHandStats11FoldTo3BetHEM_SbHu10To14MTop10To30) != 0; }
	UINT32 isFoldTo3BetHEM_SbHu10To14MTop30To50() const { return (statsFlags11.value & eColHandStats11FoldTo3BetHEM_SbHu10To14MTop30To50) != 0; }
	UINT32 isFoldTo3BetHEM_SbHu14PlusMBot50() const { return (statsFlags11.value & eColHandStats11FoldTo3BetHEM_SbHu14PlusMBot50) != 0; }
	UINT32 isFoldTo3BetHEM_SbHu14PlusMTop10To30() const { return (statsFlags11.value & eColHandStats11FoldTo3BetHEM_SbHu14PlusMTop10To30) != 0; }
	UINT32 isFoldTo3BetHEM_SbHu14PlusMTop30To50() const { return (statsFlags11.value & eColHandStats11FoldTo3BetHEM_SbHu14PlusMTop30To50) != 0; }
	UINT32 isFoldTo3BetHEM_SbHu7To10MTop10To50() const { return (statsFlags11.value & eColHandStats11FoldTo3BetHEM_SbHu7To10MTop10To50) != 0; }
	UINT32 isFoldBbHuLimped14PlusM() const { return (statsFlags11.value & eColHandStats11FoldBbHuLimped14PlusM) != 0; }
	UINT32 isCallBbHuLimped14PlusM() const { return (statsFlags11.value & eColHandStats11CallBbHuLimped14PlusM) != 0; }
	UINT32 isHULCPreflopCheckBet1To1_5BBOnFlop14PlusM() const { return (statsFlags11.value & eColHandStats11HULCPreflopCheckBet1To1_5BBOnFlop14PlusM) != 0; }
	UINT32 isHULCPreflopCheckBet1_5To2BBOnFlop14PlusM() const { return (statsFlags11.value & eColHandStats11HULCPreflopCheckBet1_5To2BBOnFlop14PlusM) != 0; }
	UINT32 isHULCPreflopCheckBet2PlusBBOnFlop14PlusM() const { return (statsFlags11.value & eColHandStats11HULCPreflopCheckBet2PlusBBOnFlop14PlusM) != 0; }
	UINT32 isHULCPreflopCheckCheckOnFlop14PlusM() const { return (statsFlags11.value & eColHandStats11HULCPreflopCheckCheckOnFlop14PlusM) != 0; }
	UINT32 isHUMRCalledPreflopCheckBet1To1_5BBOnFlop14PlusM() const { return (statsFlags11.value & eColHandStats11HUMRCalledPreflopCheckBet1To1_5BBOnFlop14PlusM) != 0; }
	UINT32 isHUMRCalledPreflopCheckBet1_5To2BBOnFlop14PlusM() const { return (statsFlags11.value & eColHandStats11HUMRCalledPreflopCheckBet1_5To2BBOnFlop14PlusM) != 0; }
	UINT32 isHUMRCalledPreflopCheckBet2To2_5BBOnFlop14PlusM() const { return (statsFlags11.value & eColHandStats11HUMRCalledPreflopCheckBet2To2_5BBOnFlop14PlusM) != 0; }
	UINT32 isHUMRCalledPreflopCheckBet2_5To3BBOnFlop14PlusM() const { return (statsFlags11.value & eColHandStats11HUMRCalledPreflopCheckBet2_5To3BBOnFlop14PlusM) != 0; }
	UINT32 isHUMRCalledPreflopCheckBet3To3_5BBOnFlop14PlusM() const { return (statsFlags11.value & eColHandStats11HUMRCalledPreflopCheckBet3To3_5BBOnFlop14PlusM) != 0; }
	UINT32 isHUMRCalledPreflopCheckBet3_5PlusBBOnFlop14PlusM() const { return (statsFlags11.value & eColHandStats11HUMRCalledPreflopCheckBet3_5PlusBBOnFlop14PlusM) != 0; }
	UINT32 isHUMRCalledPreflopCheckCheckOnFlop14PlusM() const { return (statsFlags11.value & eColHandStats11HUMRCalledPreflopCheckCheckOnFlop14PlusM) != 0; }
	UINT32 isPfrBbHuLimped10To14MRaiseTo2To3BB() const { return (statsFlags11.value & eColHandStats11PfrBbHuLimped10To14MRaiseTo2To3BB) != 0; }
	UINT32 isPfrBbHuLimped10To14MRaiseTo3To3_5BB() const { return (statsFlags11.value & eColHandStats11PfrBbHuLimped10To14MRaiseTo3To3_5BB) != 0; }
	UINT32 isPfrBbHuLimped10To14MRaiseTo3_5To4BB() const { return (statsFlags11.value & eColHandStats11PfrBbHuLimped10To14MRaiseTo3_5To4BB) != 0; }
	UINT32 isPfrBbHuLimped10To14MRaiseTo4To4_5BB() const { return (statsFlags11.value & eColHandStats11PfrBbHuLimped10To14MRaiseTo4To4_5BB) != 0; }
	UINT32 isPfrBbHuLimped10To14MRaiseTo4_5To5BB() const { return (statsFlags11.value & eColHandStats11PfrBbHuLimped10To14MRaiseTo4_5To5BB) != 0; }
	UINT32 isPfrBbHuLimped10To14MRaiseTo5To6BB() const { return (statsFlags11.value & eColHandStats11PfrBbHuLimped10To14MRaiseTo5To6BB) != 0; }
	UINT32 isPfrBbHuLimped10To14MRaiseTo6To10BB() const { return (statsFlags11.value & eColHandStats11PfrBbHuLimped10To14MRaiseTo6To10BB) != 0; }
	UINT32 isPfrBbHuLimped10To14MRaiseTo10PlusBB() const { return (statsFlags11.value & eColHandStats11PfrBbHuLimped10To14MRaiseTo10PlusBB) != 0; }
	UINT32 isPfrBbHuLimped14PlusMTop10() const { return (statsFlags11.value & eColHandStats11PfrBbHuLimped14PlusMTop10) != 0; }
	UINT32 isPfrBbHuLimped14PlusMTop10Opps() const { return (statsFlags11.value & eColHandStats11PfrBbHuLimped14PlusMTop10Opps) != 0; }
};

////////////////////////////////////////////////////////////////////////////////

//lint -esym(1509, HandStatsStmtBase) -esym(1512, HandStatsStmtBase)
class HandStatsStmtBase : public DbmStatement
{
protected:
	HandStatsStmtBase(DatabaseManagerCommon& man, HandStatsStmtColumns& cols_, bool isParam_) :
		 DbmStatement(man), cols(cols_), isWriting(isParam_)
	{}

	void prepareAndBind();

private: // Disallow copying
	HandStatsStmtBase(HandStatsStmtBase const&);
	HandStatsStmtBase& operator=(HandStatsStmtBase const&);

private:
	template<typename T>
	void bindFirst(T& t) { isWriting ? bindFirstParam(t) : bindFirstCol(t); }

	template<typename T>
	void bindNext(T& t) { isWriting ? bindNextParam(t) : bindNextCol(t); }

protected:
	static char const* const columnNames;
	HandStatsStmtColumns& cols; //lint !e1725
	PString query;

private:
	bool isWriting;
};

////////////////////////////////////////////////////////////////////////////////

class ReadHandStatsStmt : public HandStatsStmtBase
{
public:
	ReadHandStatsStmt(DatabaseManagerCommon& man, HandStatsStmtColumns& cols_, SrvDate date, UINT32 maxDeferredDays,
					  char const* handStatsRawTableName, char const* deferredHandsTableName, char const* completedTournsTableName);
};

////////////////////////////////////////////////////////////////////////////////

class WriteDeferredHandStmt : public HandStatsStmtBase
{
public:
	WriteDeferredHandStmt(DatabaseManagerCommon& man, HandStatsStmtColumns& cols_, char const* deferredHandsTableName);
};

////////////////////////////////////////////////////////////////////////////////

class HandStatsManager
{
public:
	explicit HandStatsManager(AggregatorDbManager& man_) : man(man_), pReadHandStatsStmt(0), pWriteDeferredHandStmt(0) {}
	~HandStatsManager();

	void setup(SrvDate const& date, UINT32 maxDeferredDays,
			   char const* handStatsRawTableName, char const* deferredHandsTableName, char const* completedTournsTableName);

	void read()		{ pReadHandStatsStmt->execute(); }
	bool fetch()	{ return pReadHandStatsStmt->fetch(); }
	void finish()	{ pReadHandStatsStmt->closeCursor(); }
	void write()	{ pWriteDeferredHandStmt->execute(); }

	void getKey(AggregateHandData& data) const;
	void getTournsSessionKey(TournsSessionKey& key) const;
	void getHandsSessionKey(HandsSessionKey& key) const;
	void getConsistencyKey(ConsistencyKey& key) const;

	bool isGameOrPlayerChanged(AggregateHandData const& data) const;
	bool isBlindsChanged(AggregateHandData const& data) const;
	bool isKeyChanged(AggregateHandData const& data) const { return isGameOrPlayerChanged(data) || isBlindsChanged(data); }

	void aggregateStats(AggregateHandData& data) const;
	void aggregateConsistency(ConsistencyData& data) const;
	UINT32 aggregateHandPartnerData(HandPartnersMap& handPartners) const;
	void aggregateTournPartnerData(TournPartnerDataMap& tournPartners) const;

	UINT32 getTournId() const				{ return cols.tournamentId.getValue(); }
	void getFinished(SrvTime& val) const	{ cols.finished.getValue(val); }
	UINT64 getSessionId() const				{ return cols.sessionId.value; }
	UINT32 getSessionType() const			{ return cols.sessionType.value; }
	char const* getUserId() const			{ return cols.userId.c_str(); }
	UINT32 getAggregationFlags() const		{ return cols.aggregationFlags.value; }
	UINT32 getHostId() const				{ return cols.hostId.value; }
	UINT32 getTournEntries() const			{ return cols.tournActualEntries.value? cols.tournActualEntries.value : cols.tournMaxEntry.value; }

	char const* formatDeferredHand(PString& str);

private:
	void getBaseSessionKey(SessionKey& key) const;

private:
	HandStatsStmtColumns cols;
	AggregatorDbManager& man; //lint !e1725
	ReadHandStatsStmt* pReadHandStatsStmt;
	WriteDeferredHandStmt* pWriteDeferredHandStmt;
};

////////////////////////////////////////////////////////////////////////////////

class WriteTournDataStmt : public NoExecStmt
{
public:
	WriteTournDataStmt(DatabaseManagerCommon& man, char const* tournDataTableName);

	void exec(SrvDate const& startOfPeriod_, UINT32 tournId_,
			  char const* userId_, char const* partnerId_, TournPartnerData const& tournData_);

private:
	PSqlDate startOfPeriod;
	PSqlTournamentId tournId;
	PSqlString<USER_NAME_LEN> userId;
	PSqlString<USER_NAME_LEN> partnerId;
	PSqlInt allInConfrontations;
	PSqlInt showdownAllInConfrontations;
};

////////////////////////////////////////////////////////////////////////////////

class ReadCompletedTournsStmt : public NoExecStmt
{
public:
	ReadCompletedTournsStmt(DatabaseManagerCommon& man, SrvDate const& finished,
							char const* completedTournsTableName);

	void execGet(CompletedTourns& completedTourns);

private:
	PSqlTournamentId tournId;
};

////////////////////////////////////////////////////////////////////////////////

#endif // AggregatorPrepStmt_h_included
