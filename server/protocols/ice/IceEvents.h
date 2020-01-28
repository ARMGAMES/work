// iceevnts.h 
////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2014 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef iceevnts_h_included
#define iceevnts_h_included

#include "IceShared.h"
#include "IceStructs.h"
#include "dbmolap.h"
#include "clientplatforms.h"
#include "brands.h"
#include "PBitmask.h"
#include "sitemasks.h"

#define ICE_EVENT_SOURCE_INTERNAL_BASE	20000
#define ICE_EVENT_SOURCE_DBM_BASE		40000
#define ICE_EVENT_SOURCE_CRMDBM_BASE	60000
#define ICE_EVENT_SOURCE_XC_BASE		70000
#define ICE_EVENT_SOURCE_ETCDBM_BASE	80000
#define ICE_EVENT_SOURCE_SPORTS_BASE	90000
#define ICE_EVENT_SOURCE_ICEDBM_BASE	100000

//////////////////////////////////////////////////////////////////////////
// Gaming events

class IceEventRegisterForDummyTourn : public IceEvent
{
public:
	IceEventRegisterForDummyTourn(INT64 eventId_, INT32 sourceId_) 
		: IceEvent(DBM_Q_OLAP_ICE_EVENT_REGISTER_IN_DUMMY_TOURN, eIceEvent_RegisterForDummyTourn, eventId_, sourceId_){}

	void addTournId		( INT64	val ) { addParam( IceParamData( eIceEventParam_TournId,		true	, val ) ); }

	// generated in dbm
public:
	IceEvent::composeForIceOlap;
private:
	IceEvent::composeForDbm;
	IceEvent::reComposeForIceOlap;
};

// PYR-39805 
class IceEventFreemiumPurchase : public IceEvent
{
public:
	IceEventFreemiumPurchase(INT64 eventId_, INT32 sourceId_) 
		: IceEvent(DBM_Q_OLAP_ICE_EVENT_FREEMIUM_PURCHASE, eIceEvent_FreemiumPurchase, eventId_, sourceId_){}

	void addFreemiumPurchaseAmount	( INT64			val ) { addParam( IceParamData( eIceEventParam_FreemiumPurchaseAmount,		true	, val ) ); }
	void addFreemiumPurchaseFirst	( INT8			val ) { addParam( IceParamData( eIceEventParam_FreemiumPurchaseFirst,		true	, val ) ); }

	// generated in dbm
public:
	IceEvent::composeForIceOlap;
private:
	IceEvent::composeForDbm;
	IceEvent::reComposeForIceOlap;
};

class IceBaseTournEvent : public  IceEvent
{
protected:
	IceBaseTournEvent( UINT32 msgId_, INT16 eventType_ ) : IceEvent( msgId_, eventType_ ){}

public:
	// mandatory
	void addTournId				( INT64	val ) { addParam( IceParamData( eIceEventParam_TournId				, true , val ) ); }
	
protected:
	void addGameType			( INT8	val ) { addParam( IceParamData( eIceEventParam_Tourn_HandType		, true , val ) ); }
	void addTournBuyIn			( INT64	val ) { addParam( IceParamData( eIceEventParam_Tourn_BuyIn			, true , val ) ); }
	void addTournMaxPlayers		( INT32	val ) { addParam( IceParamData( eIceEventParam_Tourn_MaxPlayers		, true , val ) ); }
	void addTournTargetTourn	( INT32	val ) { addParam( IceParamData( eIceEventParam_Tourn_TargetTourn	, true , val ) ); }
	void addTournSpeed			( INT8	val ) { addParam( IceParamData( eIceEventParam_Tourn_Speed			, true , val ) ); }
	void addStructure			( INT8	val ) { addParam( IceParamData( eIceEventParam_Structure			, true , val ) ); }
	void addPlayersPerTable		( INT8	val ) { addParam( IceParamData( eIceEventParam_PlayersPerTable		, true , val ) ); }
	void addSpinGoType			( INT8	val ) { addParam( IceParamData( eIceEventParam_Tourn_SpnGoType		, true , val ) ); }
	void addSpinGoJackpotLevel	( INT8	val ) { addParam( IceParamData( eIceEventParam_Tourn_SpnGoJckptLvl	, true , val ) ); }
	void addSpinGoMultiplier	( INT32	val ) // the multiplier value * 10000. For example 2.75 -> 27000
	{ 
		addParam( IceParamData( eIceEventParam_Tourn_SpnGoMltplier, true, val / 10000	) ); // the obsolete param was kept for compatibility 
		addParam( IceParamData( eIceEventParam_Tourn_SpnGoMltprDec, true, val			) ); // 4 decimal points
	}

	void addGameFlags( INT8	val ) 
	{ 
		addParam( IceParamData( eIceEventParam_GameFlags8,			true, val, (INT8)0 /*mask*/ ) ); // see enum eIceEventParam_GameFlags8_Bits
	} 
	void addTournFlags( INT16 val ) 
	{ 
		addParam( IceParamData( eIceEventParam_Tourn_Flags16,		true, val, (INT16)0 /*mask*/ ) ); // see enum eIceEventParam_Tourn_Flags16_Bits
	} 
	
	void addTournGroup( const char* val ) 
	{ 
		if( val )
		{
			addParam( IceParamData( eIceEventParam_Tourn_Group, true, val ) ); 
		}
	}

// generated outside of dbm
public:
	IceEvent::composeForDbm;
protected:
	IceEvent::composeForIceOlap;
	IceEvent::reComposeForIceOlap;
};

class IceEventTournAward : public IceBaseTournEvent
{
public:
	IceEventTournAward() : IceBaseTournEvent( DBM_Q_OLAP_ICE_EVENT_TOURNAMENT_AWARD, eIceEvent_ReceivedTournAward ){}

	void addTournWinAmount		( INT64	val ) { addParam( IceParamData( eIceEventParam_Tourn_WinAmount		, true , val ) ); }
	void addTournResultFlags8	( INT8	val ) { addParam( IceParamData( eIceEventParam_Tourn_ResultFlags8	, true , val, (INT8)0 /*mask*/) ); }
	void addTournStake			( INT64	val ) { addParam( IceParamData( eIceEventParam_Tourn_Stake			, true , val ) ); }

public:
	//IceBaseTournEvent::addGameFlags; // TODO: remove completely
};

class EventTournFinish : public IceBaseTournEvent // PYR-33583
{
public:
	EventTournFinish() : IceBaseTournEvent(DBM_Q_OLAP_ICE_EVENT_FINISH_TOURNAMENT, eIceEvent_FinishedTourn){}

public:
	void addTournWinAmount		( INT64	val ) { addParam( IceParamData( eIceEventParam_Tourn_WinAmount			, true , val ) ); }
	void addTournResultFlags8	( INT8	val ) { addParam( IceParamData( eIceEventParam_Tourn_ResultFlags8		, true , val, (INT8)0 /*mask*/) ); }
	void addTournStake			( INT64	val ) { addParam( IceParamData( eIceEventParam_Tourn_Stake				, true , val ) ); }

	void addTournNumPlayers		( INT32	val ) { addParam( IceParamData( eIceEventParam_Tourn_NumPlayers			, true , val ) ); }
	void addTournWinType		( INT8	val ) { addParam( IceParamData( eIceEventParam_Tourn_WinType			, true , val, (INT8)0 /*mask*/) ); }
	void addTournRank			( INT32	val ) { addParam( IceParamData( eIceEventParam_Tourn_Rank				, true , val ) ); }
	void addTournRankPercentile	( INT16	val ) { addParam( IceParamData( eIceEventParam_Tourn_RankPercentile		, true , val ) ); }
	void addTournPPPFlags		( INT8	val	) {	addParam( IceParamData( eIceEventParam_Tourn_PPPFlags			, true , val, (INT8)0 /*mask*/) ); }// see enum eIceEventParam_Tourn_PPP_Flags8_Bits

	// PYR-124673
	void addInternalRef			( const char* val	) { addParam( IceParamData( eIceEventParam_Tourn_InternalRef	, true , val ) ); }
	void addTotalPrizePool		( INT64	val			) { addParam( IceParamData( eIceEventParam_Tourn_TotalPrizePool	, true , val ) ); }
	void addBountiesWon			( INT32	val			) { addParam( IceParamData( eIceEventParam_Tourn_BountiesWon	, true , val ) ); }
public:
	//IceBaseTournEvent::addGameFlags; // TODO: remove completely
};

class IceEventPlayTourn : public IceBaseTournEvent
{
public:
	IceEventPlayTourn() : IceBaseTournEvent(DBM_Q_OLAP_ICE_EVENT_PLAY_TOURNAMENT, eIceEvent_PlayedInTourn){}

	// PYR-66899
	void addTournPlayedTime		( SrvTime& val		) { addParam( IceParamData(eIceEventParam_Tourn_PlayedTime	, true	, val ) ); } // user played time so shouldn't be shared
	void addInternalRef			( const char* val	) { addParam( IceParamData(eIceEventParam_Tourn_InternalRef	, true	, val ) ); }
	void addTargetRef			( const char* val	) { addParam( IceParamData(eIceEventParam_Tourn_TargetRef	, true	, val ) ); }
	void addTournCurrency		( const char* val	) { addParam( IceParamData(eIceEventParam_Tourn_Currency	, true	, val ) ); }
	void addTournMinPlayers		( INT32	val			) { addParam( IceParamData(eIceEventParam_Tourn_MinPlayers	, true	, val ) ); }

	// PYR-123925
	void addTournPPPFlags		( INT8 val			) {	addParam( IceParamData(eIceEventParam_Tourn_PPPFlags	, true	, val, (INT8)0 /*mask*/)); } // see enum eIceEventParam_Tourn_PPP_Flags8_Bits
	void addTournPPPBuyIn		( INT64	val			) { addParam( IceParamData(eIceEventParam_Tourn_PPPBuyIn	, true	, val ) ); }

public:
	IceBaseTournEvent::addTournBuyIn;
	IceBaseTournEvent::addGameType;
	IceBaseTournEvent::addTournMaxPlayers;
	IceBaseTournEvent::addTournTargetTourn;
	IceBaseTournEvent::addTournSpeed;
	IceBaseTournEvent::addGameFlags;
	IceBaseTournEvent::addTournFlags;
	IceBaseTournEvent::addTournGroup;
	IceBaseTournEvent::addStructure;
	IceBaseTournEvent::addPlayersPerTable;
	IceBaseTournEvent::addSpinGoMultiplier;
	IceBaseTournEvent::addSpinGoType;
	IceBaseTournEvent::addSpinGoJackpotLevel;

};

// sent in response to a DRC request by the DBM as shared tourn data
class IceEventPlayTournShared : public IceEventPlayTourn
{
protected:
	IceEventPlayTourn::addTournPlayedTime; // PYR-66899  user played time - not shared
public:
	IceEvent::composeForIceOlap;
protected:
	IceEvent::composeForDbm;	
	IceEvent::reComposeForIceOlap;
};

class IceEventKickTournPlayer : public IceBaseTournEvent
{
public:
	IceEventKickTournPlayer() : IceBaseTournEvent(DBM_Q_OLAP_ICE_EVENT_KICKED_TOURN_PLAYER, eIceEvent_KickedTournPlayer){}

	void addNumKickedPlayers	( INT8	val ) { addParam( IceParamData( eIceEventParam_Tourn_NumKicked		, true , val ) ); }

public:
	//IceBaseTournEvent::addGameFlags; // TODO: remove completely
};

class IceEventTournHand : public IceBaseTournEvent
{
public:
	IceEventTournHand() : IceBaseTournEvent(DBM_Q_OLAP_ICE_EVENT_TOURN_HAND, eIceEvent_TournHand){}

	void addTournHandResult		( INT8	val ) { addParam( IceParamData( eIceEventParam_HandResult				, true , val ) ); }
	void addFinishedHandAt		( INT8	val ) { addParam( IceParamData( eIceEventParam_FinishedHandAt			, true , val ) ); }
	void addHoleCardsSuitCount	( INT8	val ) { addParam( IceParamData( eIceEventParam_HandHoleCardsSuitCount	, true , val ) ); }
	void addHoleCardsHighValue	( INT8	val ) { addParam( IceParamData( eIceEventParam_HandHoleCardsHighValue	, true , val ) ); }
	void addHoleCardsHighSuit	( INT8	val ) { addParam( IceParamData( eIceEventParam_HandHoleCardsHighSuit	, true , val ) ); }
	void addHoleCardsLowValue	( INT8	val ) { addParam( IceParamData( eIceEventParam_HandHoleCardsLowValue	, true , val ) ); }
	void addHoleCardsLowSuit	( INT8	val ) { addParam( IceParamData( eIceEventParam_HandHoleCardsLowSuit		, true , val ) ); }
	void addHoleCardsGap		( INT8	val ) { addParam( IceParamData( eIceEventParam_HandHoleCardsGap			, true , val ) ); }
	void addHoleCards			( INT64	val ) { addParam( IceParamData( eIceEventParam_HandHoleCards			, true , val, (INT64)0 /*mask*/ ) ); }
	void addBoardCards			( INT64	val ) { addParam( IceParamData( eIceEventParam_HandBoardCards			, true , val, (INT64)0 /*mask*/ ) ); }
	void addHandStrenghtRank	( INT8	val ) { addParam( IceParamData( eIceEventParam_HandStrengthRank			, true , val ) ); }
	void addHandStrengthHigh	( INT8	val ) { addParam( IceParamData( eIceEventParam_HandStrengthHigh			, true , val ) ); }
	void addHandStrengthLow		( INT8	val ) { addParam( IceParamData( eIceEventParam_HandStrengthLow			, true , val ) ); }
	void addHandStrenghtSuit	( INT8	val ) { addParam( IceParamData( eIceEventParam_HandStrengthSuit			, true , val ) ); }
	void addSeatPosition8		( INT8	val ) {	addParam( IceParamData( eIceEventParam_SeatPositionFlags8_Deprecated,true,val, (INT8)0 /*mask*/ ) ); }
	void addSeatPosition		( INT16	val ) {	addParam( IceParamData( eIceEventParam_SeatPositionFlags16		, true , val, (INT16)0 /*mask*/ ) ); }
	void addPowerFlags			( INT64	val ) { addParam( IceParamData( eIceEventParam_HandPowerPlayedFlags64	, true , val, (INT64)0 /*mask*/ ) ); }
	// PYR-63482
	void addHandBetAmount		( INT64	val ) { addParam( IceParamData( eIceEventParam_Tourn_HandBetAmount		, true , val ) ); }
	void addPotSizeAmountBB		( INT64	val ) { addParam( IceParamData( eIceEventParam_HandPotSizeAmountBB		, true , val ) ); }
	void addPotSizeAmount		( INT64	val ) { addParam( IceParamData( eIceEventParam_Tourn_PotSizeAmount		, true , val ) ); }
	// PYR-63483
	void addPreFlopAction		( INT8	val ) { addParam( IceParamData( eIceEventParam_HandPreFlopAction		, true , val, (INT8)0 /*mask*/ ) ); }
	void addFlopAction			( INT8	val ) { addParam( IceParamData( eIceEventParam_HandFlopAction			, true , val, (INT8)0 /*mask*/ ) ); }
	void addTurnAction			( INT8	val ) { addParam( IceParamData( eIceEventParam_HandTurnAction			, true , val, (INT8)0 /*mask*/ ) ); }
	void addRiverAction			( INT8	val ) { addParam( IceParamData( eIceEventParam_HandRiverAction			, true , val, (INT8)0 /*mask*/ ) ); }
	// PYR-100104
	void addPlayersStacked		( INT16	val ) { addParam( IceParamData( eIceEventParam_PlayersStacked			, true, val ) ); }
public:
	//IceBaseTournEvent::addGameFlags; // TODO: remove completely
};

class IceEventPlayRingHand : public IceEvent
{
public:

	IceEventPlayRingHand() : IceEvent(DBM_Q_OLAP_ICE_EVENT_RING_HAND, eIceEvent_RingHandPlayed){}

	void addTableId				( INT64	val ) { addParam( IceParamData( eIceEventParam_TableId			, true, val ) ); }
	void addMergedTableId		( INT64	val ) { addParam( IceParamData( eIceEventParam_MergedTableId	, true, val ) ); }

	void addGameFlags( INT8	val ) 
	{ 
		addParam( IceParamData( eIceEventParam_GameFlags8, true, val , (INT8)0 /*mask*/ ) ); // see enum eIceEventParam_GameFlags8_Bits
	} 
	void addRingHandFlags( INT8	val ) // PYR-31469
	{ 
		addParam( IceParamData( eIceEventParam_RingHandFlags8, true, val , (INT8)0 /*mask*/ ) ); // see enum eIceEventParam_RingHandFlags8_Bits
	}
	void addRingSeatPosition8( INT8	val ) // PYR-84639
	{
		addParam( IceParamData( eIceEventParam_SeatPositionFlags8_Deprecated, true, val, ( INT8 )0 /*mask*/ ) ); // see enum eIceEventParam_SeatPositionFlags8_Bits
	}
	void addRingSeatPosition( INT16	val ) // PYR-47383
	{
		addParam( IceParamData( eIceEventParam_SeatPositionFlags16, true, val, ( INT16 )0 /*mask*/ ) ); // see enum eIceEventParam_SeatPositionFlags16_Bits
	}
	void addRingHighStake( const INT32 highStake ) // PYR-31467
	{
		addParam( IceParamData( eIceEventParam_RingHighStake , true , highStake ) );
	}
	void addGameType				( INT8	val ) { addParam( IceParamData( eIceEventParam_RingGameType				, true , val ) ); }
	void addStructure				( INT8	val ) { addParam( IceParamData( eIceEventParam_Structure				, true , val ) ); }
	void addFinishedHandAt			( INT8	val ) { addParam( IceParamData( eIceEventParam_FinishedHandAt			, true , val ) ); }
	void addRingHandResult			( INT8	val ) { addParam( IceParamData( eIceEventParam_HandResult				, true , val ) ); }
	void addPlayersPerTable			( INT8	val ) { addParam( IceParamData( eIceEventParam_PlayersPerTable			, true , val ) ); }
	void addRingNumPlayersDealtIn	( INT8	val ) { addParam( IceParamData( eIceEventParam_RingNumPlayersDealtIn	, true , val ) ); }

	// PYR-107361
	void addNumberOfCardsSwapped	( INT8	val ) { addParam( IceParamData( eIceEventParam_RingHandNumberOfSwaps	, true , val ) ); }


	// PYR-71993
	void addRingNetLossAmount		( INT64	val ) { addParam( IceParamData( eIceEventParam_RingHandNetLossAmount	, true , val ) ); }
	void addRingNetWinAmount		( INT64	val ) { addParam( IceParamData( eIceEventParam_RingHandNetWinAmount		, true , val ) ); }
	void addRingGrossWinAmount		( INT64	val ) { addParam( IceParamData( eIceEventParam_RingHandGrossWinAmount	, true , val ) ); }
	void addRingBetAmount			( INT64	val ) { addParam( IceParamData( eIceEventParam_RingHandBetAmount		, true , val ) ); }

	// PYR-33924
	void addHoleCardsSuitCount	( INT8	val ) { addParam( IceParamData( eIceEventParam_HandHoleCardsSuitCount	, true , val ) ); }
	void addHoleCardsHighValue	( INT8	val ) { addParam( IceParamData( eIceEventParam_HandHoleCardsHighValue	, true , val ) ); }
	void addHoleCardsLowValue	( INT8	val ) { addParam( IceParamData( eIceEventParam_HandHoleCardsLowValue	, true , val ) ); }
	void addHoleCardsHighSuit	( INT8	val ) { addParam( IceParamData( eIceEventParam_HandHoleCardsHighSuit	, true , val ) ); }
	void addHoleCardsLowSuit	( INT8	val ) { addParam( IceParamData( eIceEventParam_HandHoleCardsLowSuit		, true , val ) ); }
	void addHoleCardsGap		( INT8	val ) { addParam( IceParamData( eIceEventParam_HandHoleCardsGap			, true , val ) ); }
	// PYR-33084
	void addHandStrenghtRank	( INT8	val ) { addParam( IceParamData( eIceEventParam_HandStrengthRank			, true , val ) ); }
	void addHandStrenghtSuit	( INT8	val ) { addParam( IceParamData( eIceEventParam_HandStrengthSuit			, true , val ) ); }
	void addHandStrengthHigh	( INT8	val ) { addParam( IceParamData( eIceEventParam_HandStrengthHigh			, true , val ) ); }
	void addHandStrengthLow		( INT8	val ) { addParam( IceParamData( eIceEventParam_HandStrengthLow			, true , val ) ); }
	
	// PYR-42615
	void addRingHoleCards		( INT64	val ) { addParam( IceParamData( eIceEventParam_HandHoleCards			, true , val, (INT64)0 /*mask*/ ) ); }
	void addRingBoardCards		( INT64	val ) { addParam( IceParamData( eIceEventParam_HandBoardCards			, true , val, (INT64)0 /*mask*/ ) ); }
	// PYR-39372
	void addRingHoleAndFlopCards( INT64	val ) { addParam( IceParamData( eIceEventParam_RingHoleAndFlopCards		, true , val, (INT64)0 /*mask*/ ) ); }
	// PYR-72156
	void addUnfoldResult			( INT8	val ) { addParam( IceParamData( eIceEventParam_UnfoldResult				, true , val ) ); }
	void addUnfoldFinish			( INT8	val ) { addParam( IceParamData( eIceEventParam_UnfoldFinish				, true , val ) ); }
	void addUnfoldHandStrengthRank	( INT8	val ) { addParam( IceParamData( eIceEventParam_UnfoldHandStrengthRank	, true , val ) ); }
	void addUnfoldAntesReturned( const bool val )
	{
		INT8 val8 = val ? 1 : 0;
		addParam( IceParamData( eIceEventParam_UnfoldAntesReturned, true, val8 ) );
	}
	void addUnfoldBet( const bool val )
	{
		INT8 val8 = val ? 1 : 0;
		addParam( IceParamData( eIceEventParam_UnfoldBet, true, val8 ) );
	}
	// PYR-63482
	void addPotSizeAmountBB		( INT64	val ) { addParam( IceParamData( eIceEventParam_HandPotSizeAmountBB		, true , val ) ); }
	void addPotSizeAmount		( INT64	val ) { addParam( IceParamData( eIceEventParam_HandPotSizeAmount		, true , val ) ); }
	// PYR-63483
	void addPreFlopAction		( INT8	val ) { addParam( IceParamData( eIceEventParam_HandPreFlopAction		, true , val, (INT8)0 /*mask*/ ) ); }
	void addFlopAction			( INT8	val ) { addParam( IceParamData( eIceEventParam_HandFlopAction			, true , val, (INT8)0 /*mask*/ ) ); }
	void addTurnAction			( INT8	val ) { addParam( IceParamData( eIceEventParam_HandTurnAction			, true , val, (INT8)0 /*mask*/ ) ); }
	void addRiverAction			( INT8	val ) { addParam( IceParamData( eIceEventParam_HandRiverAction			, true , val, (INT8)0 /*mask*/ ) ); }
	// PYR-100104
	void addPlayersStacked		( INT16	val ) { addParam( IceParamData( eIceEventParam_PlayersStacked			, true, val ) ); }

	// generated outside of dbm
public:
	IceEvent::composeForDbm;
private:
	IceEvent::composeForIceOlap;
	IceEvent::reComposeForIceOlap;
};

// PYR-106565
class IceEventPokerSideBetSettled : public IceEvent
{
public:

	IceEventPokerSideBetSettled( INT64 eventId_, INT32 sourceId_ ) : IceEvent( DBM_Q_OLAP_ICE_EVENT_POKER_SIDE_BET_SETTLED, eIceEvent_PokerSideBetSettled, eventId_, sourceId_ ) {}

	void addGameFlags		( INT8	val ) { addParam( IceParamData( eIceEventParam_GameFlags8,		true, val, (INT8)0 /*mask*/ ) ); } // see enum eIceEventParam_GameFlags8_Bits
	void addGameType		( INT8	val ) { addParam( IceParamData( eIceEventParam_RingGameType,	true, val ) ); }
	void addStructure		( INT8	val ) { addParam( IceParamData( eIceEventParam_Structure,		true, val ) ); }
	void addRingHighStake	( INT32	val ) { addParam( IceParamData( eIceEventParam_RingHighStake,	true, val ) ); }
	void addPlayersPerTable	( INT8	val ) { addParam( IceParamData( eIceEventParam_PlayersPerTable, true, val ) ); }
		
	void addPokerSideBetAmount			( INT64	val			) { addParam( IceParamData( eIceEventParam_PokerSideBetAmount,			true, val ) ); }
	void addPokerSideBetWinAmount		( INT64	val			) { addParam( IceParamData( eIceEventParam_PokerSideBetWinAmount,		true, val ) ); }
	void addPokerSideBetNetWinAmount	( INT64	val			) { addParam( IceParamData( eIceEventParam_PokerSideBetNetWinAmount,	true, val ) ); }
	void addPokerSideBetNetLossAmount	( INT64	val			) { addParam( IceParamData( eIceEventParam_PokerSideBetNetLossAmount,	true, val ) ); }
	void addPokerSideBetReturn			( INT32	val			) { addParam( IceParamData( eIceEventParam_PokerSideBetReturn,			true, val ) ); }
	void addPokerSideBetType			( INT8	val			) { addParam( IceParamData( eIceEventParam_PokerSideBetType,			true, val, (INT8)0 /*mask*/ ) ); }	
	void addPokerSideBetPayoutHoleCards	( INT8	val			) { addParam( IceParamData( eIceEventParam_PokerSideBetPayoutHoleCards, true, val ) ); }
	void addPokerSideBetPayoutFlopBet	( INT8	val			) { addParam( IceParamData( eIceEventParam_PokerSideBetPayoutFlopBet,	true, val ) ); }
	void addPokerSideBetResult			( INT8	val			) { addParam( IceParamData( eIceEventParam_PokerSideBetResult,			true, val ) ); }
	void addPokerSideBetCurrency		( const char *val	) { addParam( IceParamData( eIceEventParam_PokerSideBetCurrency,		true, val ) ); }
	
	// generated in dbm
public:
	IceEvent::composeForIceOlap;
private:
	IceEvent::composeForDbm;
	IceEvent::reComposeForIceOlap;
};


//////////////////////////////////////////////////////////////////////////
// Other gaming events

class IceEventTicketUsed : public IceEvent
{
public:
	IceEventTicketUsed(INT64 eventId_, INT32 sourceId_) : IceEvent(DBM_Q_OLAP_ICE_EVENT_TICKET_USED, eIceEvent_TicketUsed, eventId_, sourceId_){}

	void addTournTicketTypeId ( INT32	val ) { addParam( IceParamData( eIceEventParam_TournTicketTypeId, true , val ) ); }

// generated in dbm
public:
	IceEvent::composeForIceOlap;
private:
	IceEvent::composeForDbm;
	IceEvent::reComposeForIceOlap;
};

class IceEventEnterStarCode : public IceEvent
{
public:
	IceEventEnterStarCode(INT64 eventId_, INT32 sourceId_) : IceEvent(DBM_Q_OLAP_ICE_EVENT_ENTER_STAR_CODE, eIceEvent_EnterStarCode, eventId_, sourceId_){}

	void addStarCodeName ( const char* val ) 
	{ 
		if( val )
		{
			addParam( IceParamData( eIceEventParam_StarCodeName	, true , val ) ); 
		}
	}
// generated in crm dbm
public:
	IceEvent::composeForIceOlap;
private:
	IceEvent::composeForDbm;
	IceEvent::reComposeForIceOlap;
};

class IceEventCrmOfferAccepted : public IceEvent
{
public:
	IceEventCrmOfferAccepted(INT64 eventId_, INT32 sourceId_) : IceEvent(DBM_Q_OLAP_ICE_EVENT_CRM_OFFER_ACCEPTED, eIceEvent_CrmOfferAccepted, eventId_, sourceId_){}

	void addStarOfferRef ( const char* val ) 
	{ 
		if( val )
		{
			addParam( IceParamData( eIceEventParam_CrmOfferRef	, true , val ) ); 
		}
	}
	// generated in crm dbm
public:
	IceEvent::composeForIceOlap;
private:
	IceEvent::composeForDbm;
	IceEvent::reComposeForIceOlap;
};

class IceEventVppsEarned : public IceEvent
{
public:
	IceEventVppsEarned(INT64 eventId_, INT32 sourceId_) : IceEvent(DBM_Q_OLAP_ICE_EVENT_VPPS_EARNED, eIceEvent_VppsEarned, eventId_, sourceId_){}

	void addVppsAmount	( INT64	val ) { addParam( IceParamData( eIceEventParam_VppsAmount	, true , val ) ); }
	void addVppsMonthly	( INT64	val ) { addParam( IceParamData( eIceEventParam_VppsMonthly	, true , val ) ); }
	void addVppsYearly	( INT64	val ) { addParam( IceParamData( eIceEventParam_VppsYearly	, true , val ) ); }
	void addVppsVip		( INT8	val ) { addParam( IceParamData( eIceEventParam_VppsVipLevel	, true , val ) ); }
	void addVppsFlags	( INT8	val ) { addParam( IceParamData( eIceEventParam_VppsFlags8	, true , val , (INT8)0 /*mask*/ ) ); }
	void addVppsSource	( INT8	val ) { addParam( IceParamData( eIceEventParam_VppsSource	, true , val ) ); }
	void addCvlSchema	( INT32	val ) { addParam( IceParamData( eIceEventParam_CvlSchema	, true , val ) ); }

// generated in dbm
public:
	IceEvent::composeForIceOlap;
private:
	IceEvent::composeForDbm;
	IceEvent::reComposeForIceOlap;
};


class IceEventCppsEarned : public IceEvent
{
public:
	IceEventCppsEarned(INT64 eventId_, INT32 sourceId_) : IceEvent(DBM_Q_OLAP_ICE_EVENT_CPPS_EARNED, eIceEvent_CppsEarned, eventId_, sourceId_){}

	void addCppsAmount	( INT64	val ) { addParam( IceParamData( eIceEventParam_CppsAmount	, true , val ) ); }
	void addCppsMonthly	( INT64	val ) { addParam( IceParamData( eIceEventParam_CppsMonthly	, true , val ) ); }
	void addCppsYearly	( INT64	val ) { addParam( IceParamData( eIceEventParam_CppsYearly	, true , val ) ); }
	void addCppsLifetime( INT64	val ) { addParam( IceParamData( eIceEventParam_CppsLifetime	, true , val ) ); }
	void addCppsFlags	( INT8	val ) { addParam( IceParamData( eIceEventParam_CppsFlags8	, true , val , (INT8)0 /*mask*/ ) ); }
	void addCppsSource	( INT8	val ) { addParam( IceParamData( eIceEventParam_CppsSource	, true , val ) ); }

	// generated in dbm
public:
	IceEvent::composeForIceOlap;
private:
	IceEvent::composeForDbm;
	IceEvent::reComposeForIceOlap;
};

class IceEventSppsEarned : public IceEvent
{
public:
	IceEventSppsEarned(INT64 eventId_, INT32 sourceId_) : IceEvent(DBM_Q_OLAP_ICE_EVENT_SPPS_EARNED, eIceEvent_SppsEarned, eventId_, sourceId_){}

	void addSppsAmount	( INT64	val ) { addParam( IceParamData( eIceEventParam_SppsAmount	, true , val ) ); }
	void addSppsMonthly	( INT64	val ) { addParam( IceParamData( eIceEventParam_SppsMonthly	, true , val ) ); }
	void addSppsYearly	( INT64	val ) { addParam( IceParamData( eIceEventParam_SppsYearly	, true , val ) ); }
	void addSppsLifetime( INT64	val ) { addParam( IceParamData( eIceEventParam_SppsLifetime	, true , val ) ); }
	void addSppsFlags	( INT8	val ) { addParam( IceParamData( eIceEventParam_SppsFlags8	, true , val , (INT8)0 /*mask*/ ) ); }
	void addSppsSource	( INT8	val ) { addParam( IceParamData( eIceEventParam_SppsSource	, true , val ) ); }

	// generated in dbm
public:
	IceEvent::composeForIceOlap;
private:
	IceEvent::composeForDbm;
	IceEvent::reComposeForIceOlap;
};

class IceEvenFppChange : public IceEvent // only generated for players that use fpp bundles
{
public:
	IceEvenFppChange(INT64 eventId_, INT32 sourceId_) : IceEvent(DBM_Q_OLAP_ICE_EVENT_FPP_CHANGED, eIceEvent_FppChange, eventId_, sourceId_){}

	void addFppAmount	( INT64	val ) { addParam( IceParamData( eIceEventParam_FppAmount64	, true , val ) ); }
	void addFppFlags	( INT8	val ) { addParam( IceParamData( eIceEventParam_VppsFlags8	, true , val , (INT8)0 /*mask*/ ) ); } // same as 
	
	// generated in dbm
public:
	IceEvent::composeForIceOlap;
private:
	IceEvent::composeForDbm;
	IceEvent::reComposeForIceOlap;
};

class IceEventVipLevelChanged : public IceEvent
{
public:
	IceEventVipLevelChanged(INT64 eventId_, INT32 sourceId_) : IceEvent(DBM_Q_OLAP_ICE_EVENT_VIP_LEVEL_CHANGED, eIceEvent_VipLevelChanged, eventId_, sourceId_){}

	void addVppsMonthlyBefore	( INT64	val ) { addParam( IceParamData( eIceEventParam_VppsMonthlyBefore	, true , val ) ); }
	void addVppsYearlyBefore	( INT64	val ) { addParam( IceParamData( eIceEventParam_VppsYearlyBefore		, true , val ) ); }
	void addVipBefore			( INT8	val ) { addParam( IceParamData( eIceEventParam_VppsVipLevelBefore	, true , val ) ); }
	void addVipCurrent			( INT8	val ) { addParam( IceParamData( eIceEventParam_VppsVipLevel			, true , val ) ); }
	void addVppsMonthlyCurrent	( INT64	val ) { addParam( IceParamData( eIceEventParam_VppsMonthly			, true , val ) ); }
	void addVppsYearlyCurrent	( INT64	val ) { addParam( IceParamData( eIceEventParam_VppsYearly			, true , val ) ); }
	void addVppsFlags			( INT8	val ) { addParam( IceParamData( eIceEventParam_VppsFlags8			, true , val , (INT8)0 /*mask*/ ) ); }
	

	// generated in crm dbm
public:
	IceEvent::composeForIceOlap;
private:
	IceEvent::composeForDbm;
	IceEvent::reComposeForIceOlap;
};

//
class IceEventVipStepCompleted : public IceEvent
{
public:
	IceEventVipStepCompleted(INT64 eventId_, INT32 sourceId_) : IceEvent(DBM_Q_OLAP_ICE_EVENT_VIP_STEP_COMPLETED, eIceEvent_VipStepCompleted, eventId_, sourceId_){}


	// generated in dbm
public:
	IceEvent::composeForIceOlap;
private:
	IceEvent::composeForDbm;
	IceEvent::reComposeForIceOlap;
};

//////////////////////////////////////////////////////////////////////////
// Payments events

class IceEventRmDeposit : public IceEvent
{
public:
	IceEventRmDeposit(INT64 eventId_, INT32 sourceId_) : IceEvent(DBM_Q_OLAP_ICE_EVENT_RM_DEPOSIT, eIceEvent_RmDeposit, eventId_, sourceId_){}

	void addRmDepositAmount( const INT32 val ) 
	{ 
		addParam( IceParamData( eIceEventParam_RmDepositAmount, true , val ) );	
	}
	void addRmDepositTransId( const INT32 val ) 
	{ 
		addParam( IceParamData( eIceEventParam_RmDepositTransId, true , val ) );	
	}
	void addRmDepositTransId64(const INT64 val)
	{
		addParam(IceParamData(eIceEventParam_RmDepositTransId64, true, val));
	}
	void addRmDepositCurrency( const char *val ) 
	{ 
		if( val && *val )
		{
			addParam( IceParamData( eIceEventParam_RmDepositCurrency, true, val ) );
		}
	}
	void addRmDepositPaysystemName( const char *val ) 
	{ 
		if( val && *val )
		{
			addParam( IceParamData( eIceEventParam_RmDepositPaySystemName, true, val ) );
		}
	}
	void addFirstDeposit( const bool val )
	{
		INT8 val8 = val ? 1 : 0;
		addParam( IceParamData( eIceEventParam_RmDepositFirst, true , val8 ) );
	}
	void addAppLoginId( const INT64 val ) 
	{ 
		addParam( IceParamData( eIceEventParam_AppLoginId, true , val ) );	
	}
	void addDepositFlags( const INT8 val ) 
	{ 
		addParam( IceParamData( eIceEventParam_RmDepositFlags8, true , val, (INT8)0 /*mask*/ ) );
	}
// generated in dbm
public:
	IceEvent::composeForIceOlap;
private:
	IceEvent::composeForDbm;
	IceEvent::reComposeForIceOlap;
};

//////////////////////////////////////////////////////////////////////////
// Non-play events

// PYR-92702
class IceEventKYCDocApproved : public IceEvent
{
public:
	IceEventKYCDocApproved( INT64 eventId_, INT32 sourceId_ ) : IceEvent( DBM_Q_OLAP_ICE_EVENT_KYCDOC_APPROVED, eIceEvent_KYCDocApproved, eventId_, sourceId_ ) {}

	// for creation in security
	IceEventKYCDocApproved() : IceEvent( DBM_Q_OLAP_ICE_EVENT_KYCDOC_APPROVED, eIceEvent_KYCDocApproved ) {}

	// construct from a message received from security server 
	// so that we can recompose the constructed message it for ice olap
	// in one of the core server dbms 
	// because security does not have an olap replication path for events implemented
	// so security server will the event and send it in a regular message with userIntId outside of this message
	// and core server dbm (likely crm dbm) will put it on the majordomo replication bus
	//
	IceEventKYCDocApproved( CommMsgParser& parser, INT64 eventId_, INT32 sourceId_ )
		: IceEvent( DBM_Q_OLAP_ICE_EVENT_KYCDOC_APPROVED, eIceEvent_KYCDocApproved )
	{
		parseByDbm( parser, eventId_, sourceId_ );
	}

	void addKYCDocType				( INT8 val /*enum eIceKYCDocType */			) { addParam( IceParamData( eIceEventParam_KYCDocType		, true, val ) ); }
	void KYCDocSubCategory			( INT8 val /*enum eIceKYCDocSubCategory*/	) { addParam( IceParamData( eIceEventParam_KYCDocSubCategory, true, val ) ); }
	
	// generated in dbm
public:
	IceEvent::composeForIceOlap;
private:
	IceEvent::composeForDbm;
	IceEvent::reComposeForIceOlap;
};

// PYR-54691
class IceEventRewardPoints : public IceEvent
{
public:
	IceEventRewardPoints( INT64 eventId_, INT32 sourceId_ ) : IceEvent( DBM_Q_OLAP_ICE_EVENT_REWARDPOINTS, eIceEvent_RewardPoints, eventId_, sourceId_ ) {}

	void addRewardPointAmount	( INT32	val									) { addParam( IceParamData( eIceEventParam_RewardPointAmount, true, val ) ); }
	void addRewardPointSource	( INT8	val /*enum eIceRewardPointSource*/	) { addParam( IceParamData( eIceEventParam_RewardPointSource, true, val ) ); }
	
	// generated in dbm
public:
	IceEvent::composeForIceOlap;
private:
	IceEvent::composeForDbm;
	IceEvent::reComposeForIceOlap;
};

// PYR-96173
class IceEventGameDataAnalytic : public IceEvent
{
public:
	IceEventGameDataAnalytic( INT64 eventId_, INT32 sourceId_ ) : IceEvent( DBM_Q_OLAP_ICE_EVENT_GAMEDATAANALYTICS, eIceEvent_GameDataAnalytics, eventId_, sourceId_ ) {}

	void addProgramId	( INT32	val			) { addParam( IceParamData( eIceEventParam_GameAnalyticsProgramId, true, val ) ); }
	void addProgramName	( const char* val	) { addParam( IceParamData( eIceEventParam_GameAnalyticsProgramName, true, val ) ); }

	// generated in dbm
public:
	IceEvent::composeForIceOlap;
private:
	IceEvent::composeForDbm;
	IceEvent::reComposeForIceOlap;
};

// PYR-31590
class IceEventNewUserAccount : public IceEvent
{
public:
	IceEventNewUserAccount(INT64 eventId_, INT32 sourceId_) : IceEvent(DBM_Q_OLAP_ICE_EVENT_NEW_USER_ACCOUNT, eIceEvent_NewUserAccount, eventId_, sourceId_){}

	void addNewAccountPromoCode			( const char* val	) { addParam( IceParamData( eIceEventParam_NewAccountPromoCode			, true , val ) ); }
	void addNewAccountMktgPromoCode		( const char* val	) { addParam( IceParamData( eIceEventParam_NewAccountMktgPromoCode		, true , val ) ); } // PYR-92250
	void addNewAccountRefDomain			( const char* val	) { addParam( IceParamData( eIceEventParam_NewAccountRefDomain			, true , val ) ); }
	void addNewAccountInstallId			( const char* val	) { addParam( IceParamData( eIceEventParam_NewAccountInstallId			, true , val ) ); }
	void addNewAccountClientPlatform	( INT32 val			) { addParam( IceParamData( eIceEventParam_NewAccountClientPlatform		, true , val ) ); }
	void addNewAccountTinfo				( const char* val	) { addParam( IceParamData( eIceEventParam_NewAccountTinfo				, true , val ) ); }
	void addNewAccountEmail				( const char* val	) { addParam( IceParamData( eIceEventParam_NewAccountEmail				, true , val ) ); }
	// PYR-49359
	void addNewAccountSiteId			( INT32 val			) { addParam( IceParamData( eIceEventParam_NewAccountSiteId				, true , val ) ); }
	void addNewAccountCountry			( const char* val	) { addParam( IceParamData( eIceEventParam_NewAccountCountry			, true , val ) ); }
	void addAffiliateTracker			( const char* val	) { addParam( IceParamData( eIceEventParam_NewAccountAffiliateTracker	, true , val ) ); }
	
	// generated in dbm
public:
	IceEvent::composeForIceOlap;
private:
	IceEvent::composeForDbm;
	IceEvent::reComposeForIceOlap;
};

// PYR-50053
class IceEventChangeUserDefaultAccount : public IceEvent
{
public:
	IceEventChangeUserDefaultAccount(INT64 eventId_, INT32 sourceId_) : IceEvent(DBM_Q_OLAP_ICE_EVENT_USER_DEFAULT_ACCOUNT, eIceEvent_ChangeUserDefaultAccount, eventId_, sourceId_){}

	void addNewDefaultCurrency			( const char* val	) { addParam( IceParamData( eIceEventParam_DefaultCurrency				, true , val ) ); }
	
	// generated in dbm
public:
	IceEvent::composeForIceOlap;
private:
	IceEvent::composeForDbm;
	IceEvent::reComposeForIceOlap;
};
// PYR-32798
class IceEventAccountValidation : public IceEvent
{
public:
	IceEventAccountValidation(INT64 eventId_, INT32 sourceId_) : IceEvent(DBM_Q_OLAP_ICE_EVENT_ACCOUNT_VALIDATION, eIceEvent_AccountValidation, eventId_, sourceId_){}

	void addValidationType	( INT16	val	) { addParam( IceParamData( eIceEventParam_AccountValidationType	, true , val, (INT16)0  /*mask*/) ); }
	void addUserFlags		( INT32 val ) { addParam( IceParamData( eIceEventParam_UserFlags				, true, val ) ); }	// not an Ice flag but Integer type
	void addUserFlags2		( INT64 val ) { addParam( IceParamData( eIceEventParam_UserFlags2				, true, val ) ); }	// not an Ice flag but Integer type
	void addUserFlags3		( INT64 val ) { addParam( IceParamData( eIceEventParam_UserFlags3				, true, val ) ); }	// not an Ice flag but Integer type
	void addUserPrivs		( INT32 val ) { addParam( IceParamData( eIceEventParam_UserPrivs				, true, val ) ); }	// not an Ice flag but Integer type
	void addUserPrivs2		( INT64 val ) { addParam( IceParamData( eIceEventParam_UserPrivs2				, true, val ) ); }	// not an Ice flag but Integer type
	void addUserPrivs3		( INT64 val ) { addParam( IceParamData( eIceEventParam_UserPrivs3				, true, val ) ); }	// not an Ice flag but Integer type
	
	// generated in dbm
public:
	IceEvent::composeForIceOlap;
private:
	IceEvent::composeForDbm;
	IceEvent::reComposeForIceOlap;
};

class IceEventUserPrivilegeChange : public IceEvent
{
public:
	IceEventUserPrivilegeChange(INT64 eventId_, INT32 sourceId_) : IceEvent(DBM_Q_OLAP_ICE_EVENT_USER_PRIVILEGE_CHANGE, eIceEvent_PrivilegeChange, eventId_, sourceId_){}

	void addPrivilegeFlags( INT32 val ) 
	{ 
		addParam( IceParamData( eIceEventParam_PrivilegeFlags32, true, val , (INT32)0 /*mask*/ ) ); // see enum eIceEventParam_PrivilegeFlags32_Bits
	}
	
	// generated in dbm
public:
	IceEvent::composeForIceOlap;
private:
	IceEvent::composeForDbm;
	IceEvent::reComposeForIceOlap;
};

//////////////////////////////////////////////////////////////////////////
// Internal events

class IceEventForceOptInBypassFilter : public IceEvent
{
public:
	IceEventForceOptInBypassFilter( INT64 eventId_, INT32 sourceIdAsIceId_ ) : IceEvent(0 /*internal event - no olap msg ID*/, eIceEvent_ForceOptInBypassFilter, eventId_, sourceIdAsIceId_ ){}

	// generated inside IceLobby as if it came from the dbm
public:
	IceEvent::composeForIceOlap;
private:
	IceEvent::composeForDbm;
	IceEvent::reComposeForIceOlap;
};

class IceEventQuestCompleted : public IceEvent
{
public:
	IceEventQuestCompleted( INT64 eventId_, INT32 sourceIdAsIceId_ ) : IceEvent(0 /*internal event - no olap msg ID*/, eIceEvent_QuestCompleted, eventId_, sourceIdAsIceId_ ){}

	void addTaskId				( INT32	val			) { addParam( IceParamData( eIceEventParam_TaskId					, true , val ) ); }
	void addTaskGroupId			( INT32	val			) { addParam( IceParamData( eIceEventParam_TaskGroupId				, true , val ) ); }
	void addTaskSuccessCount	( INT8 val			) { addParam( IceParamData( eIceEventParam_TaskSuccessCount			, true , val ) ); }
	void addTaskRefName			( const char* val	) { addParam( IceParamData( eIceEventParam_TaskRefName				, true , val ) ); }
	void addState1				( INT64	val			) { addParam( IceParamData( eIceEventParam_TaskInstanceStateValue1	, true , val ) ); }
	void addState2				( INT64	val			) { addParam( IceParamData( eIceEventParam_TaskInstanceStateValue2	, true , val ) ); }
	void addState3				( INT64	val			) { addParam( IceParamData( eIceEventParam_TaskInstanceStateValue3	, true , val ) ); }
	void addState4				( INT64	val			) { addParam( IceParamData( eIceEventParam_TaskInstanceStateValue4	, true , val ) ); }
	void addState5				( INT64	val			) { addParam( IceParamData( eIceEventParam_TaskInstanceStateValue5	, true , val ) ); }

	// generated inside IceLobby as if it came from the dbm
public:
	IceEvent::composeForIceOlap;
private:
	IceEvent::composeForDbm;
	IceEvent::reComposeForIceOlap;
};

class IceEventQuestFailed : public IceEvent
{
public:
	IceEventQuestFailed( INT64 eventId_, INT32 sourceIdAsIceId_ ) : IceEvent(0 /*internal event - no olap msg ID*/, eIceEvent_QuestFailed, eventId_, sourceIdAsIceId_ ){}

	void addTaskId				( INT32	val			) { addParam( IceParamData( eIceEventParam_TaskId					, true , val ) ); }
	void addTaskGroupId			( INT32	val			) { addParam( IceParamData( eIceEventParam_TaskGroupId				, true , val ) ); }
	void addTaskFailCount		( INT8	val			) { addParam( IceParamData( eIceEventParam_TaskFailCount			, true , val ) ); }
	void addTaskRefName			( const char* val	) { addParam( IceParamData( eIceEventParam_TaskRefName				, true , val ) ); }
	void addState1				( INT64	val			) { addParam( IceParamData( eIceEventParam_TaskInstanceStateValue1	, true , val ) ); }
	void addState2				( INT64	val			) { addParam( IceParamData( eIceEventParam_TaskInstanceStateValue2	, true , val ) ); }
	void addState3				( INT64	val			) { addParam( IceParamData( eIceEventParam_TaskInstanceStateValue3	, true , val ) ); }
	void addState4				( INT64	val			) { addParam( IceParamData( eIceEventParam_TaskInstanceStateValue4	, true , val ) ); }
	void addState5				( INT64	val			) { addParam( IceParamData( eIceEventParam_TaskInstanceStateValue5	, true , val ) ); }

	// generated inside IceLobby as if it came from the dbm
public:
	IceEvent::composeForIceOlap;
private:
	IceEvent::composeForDbm;
	IceEvent::reComposeForIceOlap;
};

class IceEventQuestStarted : public IceEvent	// PYR-36162
{
public:
	IceEventQuestStarted( INT64 eventId_, INT32 sourceIdAsIceId_ ) : IceEvent(0 /*internal event - no olap msg ID*/, eIceEvent_QuestStarted, eventId_, sourceIdAsIceId_ ){}

	void addTaskId				( INT32	val			) { addParam( IceParamData( eIceEventParam_TaskId				, true , val ) ); }
	void addTaskGroupId			( INT32	val			) { addParam( IceParamData( eIceEventParam_TaskGroupId			, true , val ) ); }
	void addTaskRefName			( const char* val	) { addParam( IceParamData( eIceEventParam_TaskRefName			, true , val ) ); }

	// generated inside IceLobby as if it came from the dbm
public:
	IceEvent::composeForIceOlap;
private:
	IceEvent::composeForDbm;
	IceEvent::reComposeForIceOlap;
};

class IceEventQuestTimingOut : public IceEvent	// PYR-34378
{
public:
	IceEventQuestTimingOut( INT64 eventId_, INT32 sourceIdAsIceId_ ) : IceEvent(0 /*internal event - no olap msg ID*/, eIceEvent_QuestTimingOut, eventId_, sourceIdAsIceId_ ){}

	// generated inside IceLobby as if it came from the dbm
public:
	IceEvent::composeForIceOlap;
private:
	IceEvent::composeForDbm;
	IceEvent::reComposeForIceOlap;
};

class IceEventQuestStateChangeUser0 : public IceEvent	// PYR-80269
{
public:
	IceEventQuestStateChangeUser0( INT64 eventId_, INT32 sourceIdAsIceId_ ) : IceEvent( 0 /*internal event - no olap msg ID*/, eIceEvent_QuestStateChange, eventId_, sourceIdAsIceId_ ) {}

	void addTaskRefName	( const char* val	) { addParam( IceParamData( eIceEventParam_TaskRefName, true , val ) ); }

	// generated inside IceLobby as if it came from the dbm
public:
	IceEvent::composeForIceOlap;
private:
	IceEvent::composeForDbm;
	IceEvent::reComposeForIceOlap;
};

class IceEventTimerUser0 : public IceEvent	//PYR-80269
{
public:
	IceEventTimerUser0( INT64 eventId_, INT32 sourceIdAsIceId_ ) : IceEvent( 0 /*internal event - no olap msg ID*/, eIceEvent_Timer, eventId_, sourceIdAsIceId_ ) {}

	void addTaskRefName			( const char* val	) { addParam( IceParamData( eIceEventParam_TaskRefName	, true , val ) ); }
	void addTimerId				( INT64	val			) { addParam( IceParamData( eIceEventParam_TimerId		, true , val ) ); }

	// generated inside IceLobby as if it came from the dbm
public:
	IceEvent::composeForIceOlap;
private:
	IceEvent::composeForDbm;
	IceEvent::reComposeForIceOlap;
};

// no such event but the class is declared so it was clear it was not missing although the constructor is private and the class cannot be created
class IceEventPrefilter : public IceEvent	// PYR-80269
{
private:
	IceEventPrefilter( INT64 eventId_, INT32 sourceIdAsIceId_ ) : IceEvent( 0 /*internal event - no olap msg ID*/, eIceEvent_Timer, eventId_, sourceIdAsIceId_ ) {}

	// generated inside IceLobby as if it came from the dbm
public:
	IceEvent::composeForIceOlap;
private:
	IceEvent::composeForDbm;
	IceEvent::reComposeForIceOlap;
};

class IceEventInternalFlowControl : public IceEvent	// PYR-34378
{
public:
	IceEventInternalFlowControl( INT64 eventId_, INT32 sourceIdAsIceId_ ) : IceEvent(0 /*internal event - no olap msg ID*/, eIceEvent_InternalFlowControl, eventId_, sourceIdAsIceId_ ){}

	// generated inside IceLobby as if it came from the dbm
public:
	IceEvent::composeForIceOlap;
private:
	IceEvent::composeForDbm;
	IceEvent::reComposeForIceOlap;
};

class IceEventDuelPlay : public IceEvent	// PYR-35915
{
public:

	IceEventDuelPlay() : IceEvent(DBM_Q_OLAP_ICE_EVENT_DUEL_PLAY, eIceEvent_DuelPlay){}

	void addPlayType( INT8	val ) 
	{ 
		addParam( IceParamData( eIceEventParam_GameFlags8, true, val  , (INT8)0 /*mask*/ ) ); // see enum eIceEventParam_GameFlags8_Bits
	} 
	
	void addStructure			( INT8	val ) { addParam( IceParamData( eIceEventParam_Structure			, true , val ) ); }
	
	void addDuelTypeId	( INT32	val ) { addParam( IceParamData( eIceEventParam_DuelTypeId	, true , val ) ); }
	void addDuelBuyin	( INT64	val ) { addParam( IceParamData( eIceEventParam_DuelBuyIn	, true , val ) ); }
	void addDuelNumHands( INT32	val ) { addParam( IceParamData( eIceEventParam_DuelNumHands	, true , val ) ); }

	void addDuelPlayerFlags( INT8	val ) 
	{ 
		addParam( IceParamData( eIceEventParam_DuelPlayerFlags8, true, val , (INT8)0 /*mask*/ ) ); // see enum eIceEventParam_DuelPlayerFlags8_Bits
	} 
	
	// generated outside of dbm
public:
	IceEvent::composeForDbm;
private:
	IceEvent::composeForIceOlap;
	IceEvent::reComposeForIceOlap;
};

class IceEventDuelHand : public IceEvent	// PYR-35915
{
public:

	IceEventDuelHand() : IceEvent(DBM_Q_OLAP_ICE_EVENT_DUEL_HAND, eIceEvent_DuelHand){}

	void addPlayType( INT8	val ) 
	{ 
		addParam( IceParamData( eIceEventParam_GameFlags8, true, val  , (INT8)0 /*mask*/ ) ); // see enum eIceEventParam_GameFlags8_Bits
	} 
	void addStructure				( INT8	val ) { addParam( IceParamData( eIceEventParam_Structure				, true , val ) ); }
	
	void addDuelTypeId				( INT32	val ) { addParam( IceParamData( eIceEventParam_DuelTypeId				, true , val ) ); }
	void addDuelBuyin				( INT64	val ) { addParam( IceParamData( eIceEventParam_DuelBuyIn				, true , val ) ); }
	void addDuelHandAllin			( INT8	val ) { addParam( IceParamData( eIceEventParam_DuelHandAllin			, true , val ) ); }
	void addDuelHandScoreAmount		( INT32	val ) { addParam( IceParamData( eIceEventParam_DuelHandScoreAmount		, true , val ) ); }
	void addDuelHandFinish			( INT8	val ) { addParam( IceParamData( eIceEventParam_DuelHandFinish			, true , val ) ); }
	void addHoleCardsSuitCount		( INT8	val ) { addParam( IceParamData( eIceEventParam_DuelHoleCardsSuitCount	, true , val ) ); }
	void addHoleCardsHighValue		( INT8	val ) { addParam( IceParamData( eIceEventParam_DuelHoleCardsHighValue	, true , val ) ); }
	void addHoleCardsHighSuit		( INT8	val ) { addParam( IceParamData( eIceEventParam_DuelHoleCardsHighSuit	, true , val ) ); }
	void addHoleCardsLowValue		( INT8	val ) { addParam( IceParamData( eIceEventParam_DuelHoleCardsLowValue	, true , val ) ); }
	void addHoleCardsLowSuit		( INT8	val ) { addParam( IceParamData( eIceEventParam_DuelHoleCardsLowSuit		, true , val ) ); }
	void addHoleCardsGap			( INT8	val ) { addParam( IceParamData( eIceEventParam_DuelHoleCardsGap			, true , val ) ); }
	void addDuelHandStrength		( INT64	val ) { addParam( IceParamData( eIceEventParam_DuelHandStrength			, true , val ) ); }
	void addDuelHandResult			( INT8	val ) { addParam( IceParamData( eIceEventParam_DuelHandResult			, true , val ) ); }

	// generated outside of dbm
public:
	IceEvent::composeForDbm;
private:
	IceEvent::composeForIceOlap;
	IceEvent::reComposeForIceOlap;
};

class IceEventDuelFinish : public IceEvent	// PYR-35915
{
public:

	IceEventDuelFinish() : IceEvent(DBM_Q_OLAP_ICE_EVENT_DUEL_FINISH, eIceEvent_DuelFinish){}

	void addPlayType( INT8	val ) 
	{ 
		addParam( IceParamData( eIceEventParam_GameFlags8, true, val  , (INT8)0 /*mask*/ ) ); // see enum eIceEventParam_GameFlags8_Bits
	} 
	
	void addStructure			( INT8	val ) { addParam( IceParamData( eIceEventParam_Structure					, true , val ) ); }

	void addDuelTypeId				( INT32	val ) { addParam( IceParamData( eIceEventParam_DuelTypeId				, true , val ) ); }
	void addDuelBuyin				( INT64	val ) { addParam( IceParamData( eIceEventParam_DuelBuyIn				, true , val ) ); }
	void addDuelNumHands			( INT32	val ) { addParam( IceParamData( eIceEventParam_DuelNumHands				, true , val ) ); }
	void addDuelNumHandsWon			( INT32	val ) { addParam( IceParamData( eIceEventParam_DuelNumHandsWon			, true , val ) ); }
	void addDuelResult				( INT8	val ) { addParam( IceParamData( eIceEventParam_DuelResult				, true , val ) ); }
	void addDuelHandScoreAmount		( INT32	val ) { addParam( IceParamData( eIceEventParam_DuelScoreAmount			, true , val ) ); }
	void addDuelHandScoreMargin		( INT32	val ) { addParam( IceParamData( eIceEventParam_DuelScoreMargin			, true , val ) ); }
	void addDuelHandScorePercent	( INT16	val ) { addParam( IceParamData( eIceEventParam_DuelScorePercent			, true , val ) ); }
	void addDuelWinAmount			( INT64	val ) { addParam( IceParamData( eIceEventParam_DuelWinAmount			, true , val ) ); }
	void addDuelOpponentIntId		( INT32	val ) { addParam( IceParamData( eIceEventParam_DuelOpponentIntId		, true , val ) ); }
	void addDuelScoreMaxAvailable	( INT64	val ) { addParam( IceParamData( eIceEventParam_DuelScoreMaxAvailable	, true , val ) ); }

	void addDuelPlayerFlags( INT8	val ) 
	{ 
		addParam( IceParamData( eIceEventParam_DuelPlayerFlags8, true, val , (INT8)0 /*mask*/ ) ); // see enum eIceEventParam_DuelPlayerFlags8_Bits
	} 

	// generated outside of dbm
public:
	IceEvent::composeForDbm;
private:
	IceEvent::composeForIceOlap;
	IceEvent::reComposeForIceOlap;
};

class IceEventUserAccountChanged : public IceEvent	// PYR-32027
{
public:
	IceEventUserAccountChanged(INT64 eventId_, INT32 sourceId_) 
		: IceEvent(DBM_Q_OLAP_ICE_EVENT_USER_ACCOUNT_CHANGED, eIceEvent_AccountChanged, eventId_, sourceId_){}

	void addCountryBefore	( const char*	val ) { addParam( IceParamData( eIceEventParam_CountryBefore	, true , val ) ); }
	void addCountryAfter	( const char*	val ) { addParam( IceParamData( eIceEventParam_CountryAfter		, true , val ) ); }
	void addLicenseBefore	( INT32			val	) { addParam( IceParamData( eIceEventParam_LicenseBefore	, true , val ) ); }
	void addLicenseAfter	( INT32			val ) { addParam( IceParamData( eIceEventParam_LicenseAfter		, true , val ) ); }
	void addVppsMonthly		( INT64			val ) { addParam( IceParamData( eIceEventParam_VppsMonthly		, true , val ) ); }
	void addVppsYearly		( INT64			val ) { addParam( IceParamData( eIceEventParam_VppsYearly		, true , val ) ); }
	void addVppsVip			( INT8			val ) { addParam( IceParamData( eIceEventParam_VppsVipLevel		, true , val ) ); }

	void addVppsFlags		( INT8			val ) { addParam( IceParamData( eIceEventParam_VppsFlags8		, true , val , (INT8)0 /*mask*/ ) ); }

	// generated in dbm
public:
	IceEvent::composeForIceOlap;
private:
	IceEvent::composeForDbm;
	IceEvent::reComposeForIceOlap;
};

//////////////////////////////////////////////////////////////////////////

class IceEventCasinoGameRound : public IceEvent	// PYR-34344
{
public:

	// for creation in casino
	IceEventCasinoGameRound() : IceEvent( DBM_Q_OLAP_ICE_EVENT_CASINO_GAME_ROUND, eIceEvent_CasinoGameRound ){}

	// construct from casino - before it was recomposed for ice olap
	// done in dbm so the dbm could only call addCPPAmount() since the rest of the data
	// should already be present in the intermediate event composed by the casino
	//
	IceEventCasinoGameRound( CommMsgParser& parser, INT64 eventId_, INT32 sourceId_ ) 
		: IceEvent( DBM_Q_OLAP_ICE_EVENT_CASINO_GAME_ROUND, eIceEvent_CasinoGameRound )
	{
		parseByDbm(parser, eventId_, sourceId_);
	}

	void addPlayType					( INT8	val ) { addParam( IceParamData( eIceEventParam_GameFlags8					, true , val  , (INT8)0 /*mask*/ ) ); } // see enum eIceEventParam_GameFlags8_Bits
	void addCasinoGameCategory			( INT32	val ) { addParam( IceParamData( eIceEventParam_CasinoGameCategory			, true , val ) ); }
	void addCasinoGameType				( INT32	val ) { addParam( IceParamData( eIceEventParam_CasinoGameType				, true , val ) ); }			
	void addCasinoGameVariant			( INT32	val ) { addParam( IceParamData( eIceEventParam_CasinoGameVariant			, true , val ) ); }		
	void addCasinoGameVendor			( INT32	val ) { addParam( IceParamData( eIceEventParam_CasinoGameVendor				, true , val ) ); }				
	void addCasinoGameMode				( INT8	val ) { addParam( IceParamData( eIceEventParam_CasinoGameMode				, true , val ) ); }			
	void addCPPAmount					( INT64	val ) { addParam( IceParamData( eIceEventParam_CPPAmount					, true , val ) ); }				
	void addCasinoBetAmount				( INT64	val ) { addParam( IceParamData( eIceEventParam_CasinoBetAmount				, true , val ) ); }			
	void addCasinoGrossWinAmount		( INT64	val ) { addParam( IceParamData( eIceEventParam_CasinoGrossWinAmount			, true , val ) ); }	
	void addCasinoNetWinAmount			( INT64	val ) { addParam( IceParamData( eIceEventParam_CasinoNetWinAmount			, true , val ) ); }		
	void addCasinoNetLossAmount			( INT64	val ) { addParam( IceParamData( eIceEventParam_CasinoNetLossAmount			, true , val ) ); }		
	void addCasinoBetFundSource			( INT8	val ) { addParam( IceParamData( eIceEventParam_CasinoBetFundSource			, true , val ) ); }
	void addCasinoBetFundSourceBonusId	( INT32	val ) { addParam( IceParamData( eIceEventParam_CasinoBetFundSourceBonusId	, true , val ) ); }
	void addCasinoBetReturn				( INT32	val ) { addParam( IceParamData( eIceEventParam_CasinoBetReturn				, true , val ) ); }

	// generated in dbm
public:
	IceEvent::composeForIceOlap;
	IceEvent::composeForDbm;
private:
	IceEvent::reComposeForIceOlap;
};

class EventCasinoRaceFinish : public IceEvent	// PYR-84989
{
public:

	// for creation in casino
	EventCasinoRaceFinish() : IceEvent( DBM_Q_OLAP_ICE_EVENT_CASINO_RACE_FINISH, eIceEvent_CasinoRaceFinish ) {}

	// construct from casino - before it was recomposed for ice olap
	// done in dbm so the dbm could only call addCPPAmount() since the rest of the data
	// should already be present in the intermediate event composed by the casino
	//
	EventCasinoRaceFinish( CommMsgParser& parser, INT64 eventId_, INT32 sourceId_ )
		: IceEvent( DBM_Q_OLAP_ICE_EVENT_CASINO_RACE_FINISH, eIceEvent_CasinoRaceFinish )
	{
		parseByDbm( parser, eventId_, sourceId_ );
	}
	void addPlayType					( INT8	val ) { addParam( IceParamData( eIceEventParam_GameFlags8					, true , val  , (INT8)0 /*mask*/ ) ); } // see enum eIceEventParam_GameFlags8_Bits
	void addCasinoRaceID				( INT64 val ) { addParam( IceParamData( eIceEventParam_CasinoRaceID					, true, val ) ); }
	void addCasinoRaceGameTemplateID	( INT32 val ) { addParam( IceParamData( eIceEventParam_CasinoRaceGameTemplateID		, true, val ) ); }
	void addCasinoRaceNumPlayers		( INT32 val ) { addParam( IceParamData( eIceEventParam_CasinoRaceNumPlayers			, true, val ) ); }
	void addCasinoRaceRank				( INT32 val ) { addParam( IceParamData( eIceEventParam_CasinoRaceRank				, true, val ) ); }
	void addCasinoRaceRankPercentile	( INT32 val ) { addParam( IceParamData( eIceEventParam_CasinoRaceRankPercentile		, true, val ) ); }
	void addCasinoRacePoints			( INT64 val ) { addParam( IceParamData( eIceEventParam_CasinoRacePoints				, true, val ) ); }
	void addCasinoRaceWinAmount			( INT64 val ) { addParam( IceParamData( eIceEventParam_CasinoRaceWinAmount			, true, val ) ); }
	void addCasinoRacePrizeType			( INT8  val ) { addParam( IceParamData( eIceEventParam_CasinoRacePrizeType			, true, val ) ); }
	void addCasinoRaceSpinCount			( INT32  val ) { addParam( IceParamData( eIceEventParam_CasinoRaceSpinCount			, true, val ) ); }

	// generated in dbm
public:
	IceEvent::composeForIceOlap;
	IceEvent::composeForDbm;
private:
	IceEvent::reComposeForIceOlap;
};

class IceEventBaseCasinoBlackjack : public IceEvent	// PYR-34347
{
protected:

	IceEventBaseCasinoBlackjack( UINT32 msgId_, INT16 eventType_ ) 
		: IceEvent( msgId_, eventType_ ){}
	
 	IceEventBaseCasinoBlackjack( UINT32 olapMsgId_, INT16 eventType_, INT64 eventId_, INT32 sourceId_ ) 
 		: IceEvent( olapMsgId_, eventType_, eventId_, sourceId_ ){}


public:
	void addPlayType					( INT8	val			) { addParam( IceParamData( eIceEventParam_GameFlags8					, true, val  , (INT8)0 /*mask*/ ) ); } // see enum eIceEventParam_GameFlags8_Bits
	void addCasinoGameType				( INT32	val			) { addParam( IceParamData( eIceEventParam_CasinoGameType				, true , val					) ); }			
	void addCasinoGameVariant			( INT32	val			) { addParam( IceParamData( eIceEventParam_CasinoGameVariant			, true , val					) ); }		
	void addCasinoGameVendor			( INT32	val			) { addParam( IceParamData( eIceEventParam_CasinoGameVendor				, true , val					) ); }				
	void addCasinoGameMode				( INT8	val			) { addParam( IceParamData( eIceEventParam_CasinoGameMode				, true , val					) ); }			
	void addCasinoBetFundSource			( INT8	val			) { addParam( IceParamData( eIceEventParam_CasinoBetFundSource			, true , val					) ); }
	void addCasinoBetFundSourceBonusId	( INT32	val			) { addParam( IceParamData( eIceEventParam_CasinoBetFundSourceBonusId	, true , val					) ); }

	void addBlackjackResult				(	INT8		val ) { addParam( IceParamData( eIceEventParam_BlackjackResult				, true, val, (INT8)0  /*mask*/	) ); }
	void addBlackjackBetCurrency       	(	const char*	val ) { addParam( IceParamData( eIceEventParam_BlackjackBetCurrency       	, true, val						) ); }
	void addBlackjackInitialBetAmount  	(	INT64		val ) { addParam( IceParamData( eIceEventParam_BlackjackInitialBetAmount  	, true, val						) ); }
	void addBlackjackTotalBetAmount    	(	INT64		val ) { addParam( IceParamData( eIceEventParam_BlackjackTotalBetAmount    	, true, val						) ); }
	void addBlackjackGrossWinAmount    	(	INT64		val ) { addParam( IceParamData( eIceEventParam_BlackjackGrossWinAmount    	, true, val						) ); }
	void addBlackjackNetWinAmount      	(	INT64		val ) { addParam( IceParamData( eIceEventParam_BlackjackNetWinAmount      	, true, val						) ); }
	void addBlackjackNumCards          	(	INT16		val ) { addParam( IceParamData( eIceEventParam_BlackjackNumCards          	, true, val						) ); }
	void addBlackjackDealerTotal       	(	INT64		val ) { addParam( IceParamData( eIceEventParam_BlackjackDealerTotal       	, true, val						) ); }
	void addBlackjackFlags8   			(	INT8		val ) { addParam( IceParamData( eIceEventParam_BlackjackFlags8   			, true, val, (INT8)0  /*mask*/	) ); }
	void addBlackjackPlayerCards       	(	INT64		val ) { addParam( IceParamData( eIceEventParam_BlackjackPlayerCards       	, true, val, (INT64)0 /*mask*/	) ); }
	void addBlackjackDealerCards       	(	INT64		val ) { addParam( IceParamData( eIceEventParam_BlackjackDealerCards       	, true, val, (INT64)0 /*mask*/	) ); }
	void addBlackjackNumDoubleDowns		(	INT16		val ) { addParam( IceParamData( eIceEventParam_BlackjackNumDoubleDowns		, true, val						) ); }
	
	// generated in dbm
public:
	IceEvent::composeForIceOlap;
	IceEvent::composeForDbm;
private:
	IceEvent::reComposeForIceOlap;
};

class IceEventCasinoBlackjackHand : public IceEventBaseCasinoBlackjack
{
public:
	// for creation in casino
	IceEventCasinoBlackjackHand() 
		: IceEventBaseCasinoBlackjack( DBM_Q_OLAP_ICE_EVENT_CASINO_BLACKJACK_HAND, eIceEvent_CasinoBlackjackHand ){}

	// construct from casino - before it was recomposed for ice olap
	// done in dbm so the dbm could only call addCPPAmount() since the rest of the data
	// should already be present in the intermediate event composed by the casino
	//
	IceEventCasinoBlackjackHand( CommMsgParser& parser, INT64 eventId_, INT32 sourceId_ ) 
		: IceEventBaseCasinoBlackjack( DBM_Q_OLAP_ICE_EVENT_CASINO_BLACKJACK_HAND, eIceEvent_CasinoBlackjackHand )
	{
		parseByDbm(parser, eventId_, sourceId_);
	}
	//////////////////////////////////////////////////////////////////////////
	// for one stage event creation - if not going through OLTP but created and posted directly to ice from casino
	IceEventCasinoBlackjackHand( INT64 eventId_, INT32 sourceId_ ) 
		: IceEventBaseCasinoBlackjack( DBM_Q_OLAP_ICE_EVENT_CASINO_BLACKJACK_HAND, eIceEvent_CasinoBlackjackHand, eventId_, sourceId_ ){}
	//////////////////////////////////////////////////////////////////////////
	
	void addBlackjackPlayerTotal( INT8 val ) 
	{ 
		addParam( IceParamData( eIceEventParam_BlackjackPlayerTotal, true, val	) ); 
	}
};

class IceEventCasinoBlackjackSeat : public IceEventBaseCasinoBlackjack
{
public:
	// for creation in casino
	IceEventCasinoBlackjackSeat() 
		: IceEventBaseCasinoBlackjack( DBM_Q_OLAP_ICE_EVENT_CASINO_BLACKJACK_SEAT, eIceEvent_CasinoBlackjackSeat ){}

	// construct from casino - before it was recomposed for ice olap
	// done in dbm so the dbm could only call addCPPAmount() since the rest of the data
	// should already be present in the intermediate event composed by the casino
	//
	IceEventCasinoBlackjackSeat( CommMsgParser& parser, INT64 eventId_, INT32 sourceId_ ) 
		: IceEventBaseCasinoBlackjack( DBM_Q_OLAP_ICE_EVENT_CASINO_BLACKJACK_SEAT, eIceEvent_CasinoBlackjackSeat )
	{
		parseByDbm(parser, eventId_, sourceId_);
	}
	//////////////////////////////////////////////////////////////////////////
	// for one stage event creation - if not going through OLTP but created and posted directly to ice from casino
	IceEventCasinoBlackjackSeat( INT64 eventId_, INT32 sourceId_ ) 
		: IceEventBaseCasinoBlackjack( DBM_Q_OLAP_ICE_EVENT_CASINO_BLACKJACK_SEAT, eIceEvent_CasinoBlackjackSeat, eventId_, sourceId_ ){}
	//////////////////////////////////////////////////////////////////////////

	void addBlackjackSeatNumSplits(	INT16	val ) 
	{ 
		addParam( IceParamData( eIceEventParam_BlackjackSeatNumSplits	, true, val						) ); 
	}
	void addBlackjackFirstTwoCards(	INT64 val ) 
	{ 
		addParam( IceParamData( eIceEventParam_BlackjackFirstTwoCards    , true, val, (INT64)0 /*mask*/	) ); 
	}
};

class IceEventBaseCasinoRoulette : public IceEvent	// PYR-43854
{
protected:

	IceEventBaseCasinoRoulette( UINT32 msgId_, INT16 eventType_ ) 
		: IceEvent( msgId_, eventType_ ){}
	
 	IceEventBaseCasinoRoulette( UINT32 olapMsgId_, INT16 eventType_, INT64 eventId_, INT32 sourceId_ ) 
 		: IceEvent( olapMsgId_, eventType_, eventId_, sourceId_ ){}


public:
	void addPlayType					( INT8	val			) { addParam( IceParamData( eIceEventParam_GameFlags8					, true , val , (INT8)0 /*mask*/ ) ); } // see enum eIceEventParam_GameFlags8_Bits
	void addCasinoGameCategory			( INT32	val			) { addParam( IceParamData( eIceEventParam_CasinoGameCategory			, true , val					) ); }
	void addCasinoGameType				( INT32	val			) { addParam( IceParamData( eIceEventParam_CasinoGameType				, true , val					) ); }			
	void addCasinoGameVariant			( INT32	val			) { addParam( IceParamData( eIceEventParam_CasinoGameVariant			, true , val					) ); }		
	void addCasinoGameVendor			( INT32	val			) { addParam( IceParamData( eIceEventParam_CasinoGameVendor				, true , val					) ); }				
	void addCasinoGameMode				( INT8	val			) { addParam( IceParamData( eIceEventParam_CasinoGameMode				, true , val					) ); }			
	void addCasinoBetFundSource			( INT8	val			) { addParam( IceParamData( eIceEventParam_CasinoBetFundSource			, true , val					) ); }
	void addCasinoBetFundSourceBonusId	( INT32	val			) { addParam( IceParamData( eIceEventParam_CasinoBetFundSourceBonusId	, true , val					) ); }

	void addRouletteBetAmount			( INT64	val			) { addParam( IceParamData( eIceEventParam_RouletteBetAmount			, true , val					) ); }			
	void addRouletteBetCurrency       	( const char*	val ) { addParam( IceParamData( eIceEventParam_RouletteBetCurrency       	, true , val					) ); }
	void addRouletteGrossWinAmount		( INT64	val			) { addParam( IceParamData( eIceEventParam_RouletteGrossWinAmount		, true , val					) ); }	
	void addRouletteResult				( INT8	val			) { addParam( IceParamData( eIceEventParam_RouletteBetResult			, true , val, (INT8)0  /*mask*/	) ); }
	void addRouletteNumberSpun			( INT8	val			) { addParam( IceParamData( eIceEventParam_RouletteNumberSpun			, true , val					) ); }	
	void addRouletteNumBetsPlaced		( INT16	val			) { addParam( IceParamData( eIceEventParam_RouletteNumBetsPlaced		, true , val					) ); }	
	
	// generated in dbm
public:
	IceEvent::composeForIceOlap;
	IceEvent::composeForDbm;
private:
	IceEvent::reComposeForIceOlap;
};

class IceEventCasinoRouletteSpin : public IceEventBaseCasinoRoulette
{
public:
	// for creation in casino
	IceEventCasinoRouletteSpin() 
		: IceEventBaseCasinoRoulette( DBM_Q_OLAP_ICE_EVENT_CASINO_ROULETTE_SPIN, eIceEvent_CasinoRouletteSpin ){}

	// construct from casino - before it was recomposed for ice olap
	// done in dbm so the dbm could only call addCPPAmount() since the rest of the data
	// should already be present in the intermediate event composed by the casino
	//
	IceEventCasinoRouletteSpin( CommMsgParser& parser, INT64 eventId_, INT32 sourceId_ ) 
		: IceEventBaseCasinoRoulette( DBM_Q_OLAP_ICE_EVENT_CASINO_ROULETTE_SPIN, eIceEvent_CasinoRouletteSpin )
	{
		parseByDbm(parser, eventId_, sourceId_);
	}
	//////////////////////////////////////////////////////////////////////////
	// for one stage event creation - if not going through OLTP but created and posted directly to ice from casino
	IceEventCasinoRouletteSpin( INT64 eventId_, INT32 sourceId_ ) 
		: IceEventBaseCasinoRoulette( DBM_Q_OLAP_ICE_EVENT_CASINO_ROULETTE_SPIN, eIceEvent_CasinoRouletteSpin, eventId_, sourceId_ ){}
	//////////////////////////////////////////////////////////////////////////
	
	void addRouletteNumBetsWon				( INT16	val			) { addParam( IceParamData( eIceEventParam_RouletteNumBetsWon			, true , val					) ); }
	void addRouletteTypeBetsPlaced			( INT16	val			) { addParam( IceParamData( eIceEventParam_RouletteTypeBetsPlaced		, true , val, (INT16)0  /*mask*/) ); }
	void addRouletteNumStraightBetsPlaced	( INT8  val			) { addParam( IceParamData( eIceEventParam_RouletteNumStraightBets		, true , val					) ); }
	void addRouletteNumSplitBetsPlaced		( INT8	val			) { addParam( IceParamData( eIceEventParam_RouletteNumSplitBets			, true , val					) ); }
	void addRouletteNumCornerBetsPlaced		( INT8	val			) { addParam( IceParamData( eIceEventParam_RouletteNumCornerBets		, true , val					) ); }
	void addRouletteNumStreetBetsPlaced		( INT8	val			) { addParam( IceParamData( eIceEventParam_RouletteNumStreetBets		, true , val					) ); }
	void addRouletteNumLineBetsPlaced		( INT8	val			) { addParam( IceParamData( eIceEventParam_RouletteNumLineBets			, true , val					) ); }
	void addRouletteNumRedBlackBetsPlaced	( INT8	val			) { addParam( IceParamData( eIceEventParam_RouletteNumRedBlackBetsPlaced, true , val					) ); }
	void addRouletteNumOddEvenBetPlaced		( INT8	val			) { addParam( IceParamData( eIceEventParam_RouletteNumOddEvenBetPlaced	, true , val					) ); }
	void addRouletteNumHighLowBetPlaced		( INT8	val			) { addParam( IceParamData( eIceEventParam_RouletteNumHighLowBetPlaced	, true , val					) ); }
	void addRouletteNumDozensBetPlaced		( INT8	val			) { addParam( IceParamData( eIceEventParam_RouletteNumDozensBetPlaced	, true , val					) ); }
	void addRouletteNumColumnBetPlaced		( INT8	val			) { addParam( IceParamData( eIceEventParam_RouletteNumColumnBetPlaced	, true , val					) ); }
	void addRouletteSpinReturn				( INT32	val			) { addParam( IceParamData( eIceEventParam_RouletteSpinReturn			, true , val					) ); }
};

class IceEventCasinoRouletteBet : public IceEventBaseCasinoRoulette
{
public:
	// for creation in casino
	IceEventCasinoRouletteBet() 
		: IceEventBaseCasinoRoulette( DBM_Q_OLAP_ICE_EVENT_CASINO_ROULETTE_BET, eIceEvent_CasinoRouletteBet ){}

	// construct from casino - before it was recomposed for ice olap
	// done in dbm so the dbm could only call addCPPAmount() since the rest of the data
	// should already be present in the intermediate event composed by the casino
	//
	IceEventCasinoRouletteBet( CommMsgParser& parser, INT64 eventId_, INT32 sourceId_ ) 
		: IceEventBaseCasinoRoulette( DBM_Q_OLAP_ICE_EVENT_CASINO_ROULETTE_BET, eIceEvent_CasinoRouletteBet )
	{
		parseByDbm(parser, eventId_, sourceId_);
	}
	//////////////////////////////////////////////////////////////////////////
	// for one stage event creation - if not going through OLTP but created and posted directly to ice from casino
	IceEventCasinoRouletteBet( INT64 eventId_, INT32 sourceId_ ) 
		: IceEventBaseCasinoRoulette( DBM_Q_OLAP_ICE_EVENT_CASINO_ROULETTE_BET, eIceEvent_CasinoRouletteBet, eventId_, sourceId_ ){}
	//////////////////////////////////////////////////////////////////////////

	void addRouletteBetType					( INT16	val			) { addParam( IceParamData( eIceEventParam_RouletteBetType				, true , val, (INT16)0  /*mask*/) ); }
	void addRouletteBetNumber				( INT8	val			) { addParam( IceParamData( eIceEventParam_RouletteBetNumber			, true , val					) ); }	
	void addRouletteBetSubNumber			( INT8	val			) { addParam( IceParamData( eIceEventParam_RouletteBetSubNumber			, true , val					) ); }	
	void addRouletteBetNumTypeBetsPlaced	( INT16	val			) { addParam( IceParamData( eIceEventParam_RouletteNumTypeBetsPlaced	, true , val					) ); }
	void addRouletteBetReturn				( INT32	val			) { addParam( IceParamData( eIceEventParam_RouletteBetReturn			, true , val					) ); }
};

class IceEventBaseCasinoSlotSpin : public IceEvent
{
protected:
	IceEventBaseCasinoSlotSpin( UINT32 msgId_, INT16 eventType_ )
		: IceEvent( msgId_, eventType_ ){}

	IceEventBaseCasinoSlotSpin( UINT32 olapMsgId_, INT16 eventType_, INT64 eventId_, INT32 sourceId_ )
 		: IceEvent( olapMsgId_, eventType_, eventId_, sourceId_ ){}

	IceEventBaseCasinoSlotSpin( CommMsgParser& parser, UINT32 olapMsgId_, INT16 eventType_, INT64 eventId_, INT32 sourceId_ )
		: IceEvent( olapMsgId_, eventType_ )
	{
		parseByDbm( parser, eventId_, sourceId_ );
	}

public:
	void addPlayType					( INT8	val			) { addParam( IceParamData( eIceEventParam_GameFlags8					, true , val , (INT8)0 /*mask*/ ) ); } // see enum eIceEventParam_GameFlags8_Bits
	void addCasinoBetAmount				( INT64	val			) { addParam( IceParamData( eIceEventParam_CasinoBetAmount				, true , val					) ); }			
	void addCasinoGrossWinAmount		( INT64	val			) { addParam( IceParamData( eIceEventParam_CasinoGrossWinAmount			, true , val					) ); }	
	void addCasinoBetFundSource			( INT8	val			) { addParam( IceParamData( eIceEventParam_CasinoBetFundSource			, true , val					) ); }
	void addCasinoBetFundSourceBonusId	( INT32	val			) { addParam( IceParamData( eIceEventParam_CasinoBetFundSourceBonusId	, true , val					) ); }

	void addSlotLinesPlayed				( INT8	val			) { addParam( IceParamData( eIceEventParam_SlotLinesPlayed				, true , val					) ); }				
	void addSlotLinesWon				( INT8	val			) { addParam( IceParamData( eIceEventParam_SlotLinesWon					, true , val					) ); }				
	void addSlotNumDifferentSymbols		( INT8	val			) { addParam( IceParamData( eIceEventParam_SlotNumDifferentSymbols		, true , val					) ); }				
	
	// call 12 or 15 times in sequence for symbols 1 through 12 for casino slots or 1 through 15 for VictoryTribes slots
	void addSlotNumSymbol				( INT8	val			) { addParam( IceParamData( eIceEventParam_SlotNumSymbol				, false , val			) , false ); }
	// call 15 times in sequence for 11,12,13,14,15,21,22,23,24,25,31,32,33,34,35																		  
	void addSlotLocation       			( INT8	val			) { addParam( IceParamData( eIceEventParam_SlotLocation		       		, false , val			) , false ); }

	// generated in dbm
public:
	IceEvent::composeForIceOlap;
	IceEvent::composeForDbm;
private:
	IceEvent::reComposeForIceOlap;
};

class IceEventCasinoSlotSpin : public IceEventBaseCasinoSlotSpin
{
public:
	// for creation in casino
	IceEventCasinoSlotSpin() 
		: IceEventBaseCasinoSlotSpin( DBM_Q_OLAP_ICE_EVENT_CASINO_SLOT_SPIN, eIceEvent_CasinoSlotSpin ){}

	IceEventCasinoSlotSpin( INT64 eventId_, INT32 sourceId_ ) 
		: IceEventBaseCasinoSlotSpin( DBM_Q_OLAP_ICE_EVENT_CASINO_SLOT_SPIN, eIceEvent_CasinoSlotSpin, eventId_, sourceId_ ){}

	IceEventCasinoSlotSpin( CommMsgParser& parser, INT64 eventId_, INT32 sourceId_ ) 
		: IceEventBaseCasinoSlotSpin( parser, DBM_Q_OLAP_ICE_EVENT_CASINO_SLOT_SPIN, eIceEvent_CasinoSlotSpin, eventId_, sourceId_ ) {}

	void addCasinoGameCategory			( INT32	val			) { addParam( IceParamData( eIceEventParam_CasinoGameCategory			, true , val					) ); }
	void addCasinoGameType				( INT32	val			) { addParam( IceParamData( eIceEventParam_CasinoGameType				, true , val					) ); }			
	void addCasinoGameVariant			( INT32	val			) { addParam( IceParamData( eIceEventParam_CasinoGameVariant			, true , val					) ); }		
	void addCasinoGameVendor			( INT32	val			) { addParam( IceParamData( eIceEventParam_CasinoGameVendor				, true , val					) ); }				
	void addCasinoGameMode				( INT8	val			) { addParam( IceParamData( eIceEventParam_CasinoGameMode				, true , val					) ); }			
};

class IceEventCasinoSlotBonusPlay : public IceEvent
{
public:
	// for creation in casino
	IceEventCasinoSlotBonusPlay() 
		: IceEvent( DBM_Q_OLAP_ICE_EVENT_CASINO_SLOT_BONUS_PLAY, eIceEvent_CasinoSlotBonusPlay ){}

	IceEventCasinoSlotBonusPlay(INT64 eventId_, INT32 sourceId_) 
		: IceEvent( DBM_Q_OLAP_ICE_EVENT_CASINO_SLOT_BONUS_PLAY, eIceEvent_CasinoSlotBonusPlay, eventId_, sourceId_ ){}

	IceEventCasinoSlotBonusPlay( CommMsgParser& parser, INT64 eventId_, INT32 sourceId_ ) 
		: IceEvent( DBM_Q_OLAP_ICE_EVENT_CASINO_SLOT_BONUS_PLAY, eIceEvent_CasinoSlotBonusPlay )
	{
		parseByDbm(parser, eventId_, sourceId_);
	}

	void addPlayType					( INT8	val			) { addParam( IceParamData( eIceEventParam_GameFlags8					, true , val , (INT8)0 /*mask*/ ) ); } // see enum eIceEventParam_GameFlags8_Bits
	void addCasinoGameCategory			( INT32	val			) { addParam( IceParamData( eIceEventParam_CasinoGameCategory			, true , val					) ); }
	void addCasinoGameType				( INT32	val			) { addParam( IceParamData( eIceEventParam_CasinoGameType				, true , val					) ); }			
	void addCasinoGameVariant			( INT32	val			) { addParam( IceParamData( eIceEventParam_CasinoGameVariant			, true , val					) ); }		
	void addCasinoGameVendor			( INT32	val			) { addParam( IceParamData( eIceEventParam_CasinoGameVendor				, true , val					) ); }				
	void addCasinoGameMode				( INT8	val			) { addParam( IceParamData( eIceEventParam_CasinoGameMode				, true , val					) ); }			
	void addCasinoBetFundSource			( INT8	val			) { addParam( IceParamData( eIceEventParam_CasinoBetFundSource			, true , val					) ); }
	void addCasinoBetFundSourceBonusId	( INT32	val			) { addParam( IceParamData( eIceEventParam_CasinoBetFundSourceBonusId	, true , val					) ); }

	void addSlotBonusGameGrossWinAmount	( INT64	val			) { addParam( IceParamData( eIceEventParam_SlotBonusGameGrossWinAmount	, true , val					) ); }				
	void addSlotBonusGameType			( INT8	val			) { addParam( IceParamData( eIceEventParam_SlotBonusGameType			, true , val					) ); }				
	
	// generated in dbm
public:
	IceEvent::composeForIceOlap;
	IceEvent::composeForDbm;
private:
	IceEvent::reComposeForIceOlap;
};

//////////////////////////////////////////////////////////////////////////

class IceEventBaseSportsBet : public IceEvent	// PYR-37407
{
protected:	
	IceEventBaseSportsBet( UINT32 olapMsgId_, INT16 eventType_, INT64 eventId_, INT32 sourceId_ )
		: IceEvent( olapMsgId_, eventType_, eventId_, sourceId_ ){}

public:

	void addSportsBetAmount				( INT64	val			) { addParam( IceParamData( eIceEventParam_SportsBetAmount			, true , val					) ); }
	void addSportsBetCurrency			( const char*	val ) { addParam( IceParamData( eIceEventParam_SportsBetCurrency		, true , val					) ); }			
	void addSportsBetOdds				( INT32	val			) { addParam( IceParamData( eIceEventParam_SportsBetOdds			, true , val					) ); }		
	void addSportsBetTimingInPlayBets	( INT16	val			) { addParam( IceParamData( eIceEventParam_SportsBetTmngInPlayBets	, true , val					) ); }		
	void addSportsBetTimingPreMatchBets	( INT16	val			) { addParam( IceParamData( eIceEventParam_SportsBetTmngPreMtchBts	, true , val					) ); }		
	// addSport is deprecated and will be removed after Main.06 2019
	void addSport						( INT64	val			) { addParam( IceParamData( eIceEventParam_SportsKindFlag64			, true , val, (INT64)0 /*mask*/ ) ); }	// eIceEventParam_SportFlag64	
	void addSportsBetFlags				( INT8	val			) { addParam( IceParamData( eIceEventParam_SportsBetFlags8			, true , val, (INT8 )0 /*mask*/	) ); }	// eIceEventParam_SportBetFlag8
	void addSportsBetFlagsGroup			( INT8	val			) { addParam( IceParamData( eIceEventParam_SportsBetFlags8Group		, true , val, (INT8 )0 /*mask*/	) ); }	// eIceEventParam_SportBetFlag8Group
	void addSportsEventID				( INT64	val			) { addParam( IceParamData( eIceEventParam_SportsEventID			, true , val					) ); }				
	void addSportsSpinBet				( INT8	val			) { addParam( IceParamData( eIceEventParam_SportsSpinBet			, true , val					) ); }				
	void addSportsSpinBetMultiplier		( INT32	val			) { addParam( IceParamData( eIceEventParam_SportsSpinBetMultiplier	, true , val					) ); }				
	void addSportsCompetition			( const char*	val ) { addParam( IceParamData( eIceEventParam_SportsCompetition		, true , val					) ); }		
	void addSportsMarketId				( INT32	val			) { addParam( IceParamData( eIceEventParam_SportsMarketId			, true , val					) ); }		
	void addSportsMarketName			( const char*	val ) { addParam( IceParamData( eIceEventParam_SportsMarketName			, true , val					) ); }
	void addSportsBetFundSource			( INT8	val			) { addParam( IceParamData( eIceEventParam_SportsBetFundSource		, true , val					) ); }				
	void addSportsBetSelection			( const char*	val ) { addParam( IceParamData( eIceEventParam_SportsBetSelection		, true , val					) ); }
	void addSportsBetSelectionCode		( const char*	val ) { addParam( IceParamData( eIceEventParam_SportsBetSelectionCode	, true , val					) ); }
	void addSportsBetSelectionId		( INT64	val			) { addParam( IceParamData( eIceEventParam_SportsBetSelectionId		, true , val					) ); }		
	void addSportsBetMarketCode			( const char*	val ) { addParam( IceParamData( eIceEventParam_SportsBetMarketCode		, true , val					) ); }
	void addSportsBetMultipleNumberLegs	( INT32	val			) { addParam( IceParamData( eIceEventParam_SportsBetMultipleNumLegs	, true , val					) ); }
	void addVirtualSport				( INT64	val			) { addParam( IceParamData( eIceEventParam_SportsVirtualKindFlag64	, true , val, (INT64)0 /*mask*/ ) ); }	// eIceEventParam_VirtualSportFlag64	
	void addSportsBetComposition		( INT8	val			) { addParam( IceParamData( eIceEventParam_SportsBetComposition		, true , val					) ); }
	void addSportsBetFundSrcCampaignID	( INT64	val			) { addParam( IceParamData( eIceEventParam_SportsBetFundSrcCampgnID	, true , val					) ); }

	// Usage:
	// 	declare global static const SportsFlagsLong sfl;
	// 	e.addSport	( sfl.eIceEventParam_SportFlagLong_AlpineSkiing
	//				| sfl.eIceEventParam_SportFlagLong_AmericanFootball 
	//				);
	void addSport( const PBitmask& val )
	{ 
		addParam( IceParamData( eIceEventParam_SportsKindFlag64			, true , val.getLower64(), (INT64)0 /*mask*/ ) );
		addParam( IceParamData( eIceEventParam_SportsKindFlagLong		, true , val, PBitmask::zero /*mask*/ ) ); 
	}	// eIceEventParam_SportFlagLong
	
	// generated in dbm
public:
	IceEvent::composeForIceOlap;
	IceEvent::composeForDbm;
private:
	IceEvent::reComposeForIceOlap;
};

class IceEventSportsBetPlaced : public IceEventBaseSportsBet	// PYR-37407
{
public:
	//////////////////////////////////////////////////////////////////////////
	// for one stage event creation
	IceEventSportsBetPlaced( INT64 eventId_, INT32 sourceId_ ) 
		: IceEventBaseSportsBet( DBM_Q_OLAP_ICE_EVENT_SPORTS_BET_PACED, eIceEvent_SportsBetPlaced, eventId_, sourceId_ ){}
	//////////////////////////////////////////////////////////////////////////	
};

class IceEventSportsBetSettled : public IceEventBaseSportsBet	// PYR-37407
{
public:
	//////////////////////////////////////////////////////////////////////////
	// for one stage event creation
	IceEventSportsBetSettled( INT64 eventId_, INT32 sourceId_ ) 
		: IceEventBaseSportsBet( DBM_Q_OLAP_ICE_EVENT_SPORTS_BET_SETTLED, eIceEvent_SportsBetSettled, eventId_, sourceId_ ){}

	//////////////////////////////////////////////////////////////////////////
	
	void addSportsReturnAmount		( INT64	val ){ addParam( IceParamData( eIceEventParam_SportsWinAmount			, true , val						) ); }
	void addSportsBetReturn			( INT32	val ){ addParam( IceParamData( eIceEventParam_SportsBetReturn			, true , val						) ); }			
	void addSportsBetResult			( INT8	val ){ addParam( IceParamData( eIceEventParam_SportsBetResultFlags8		, true , val, (INT8 )0 /*mask*/		) ); }	// eIceEventParam_SportBetResultFlag8			
	void addSportsBetMultipleLegsWon( INT32	val	){ addParam( IceParamData( eIceEventParam_SportsBetMultipleLegsWon	, true , val						) ); }
	void addSportsBetNetWin			( INT64	val ){ addParam( IceParamData( eIceEventParam_SportsBetNetWin			, true , val						) ); }
	void addSportsBetNetLoss		( INT64	val ){ addParam( IceParamData( eIceEventParam_SportsBetNetLoss			, true , val						) ); }
};

//////////////////////////////////////////////////////////////////////////

class IceEventSportsBetCashout : public IceEventBaseSportsBet	// PYR-57429
{
public:
	//////////////////////////////////////////////////////////////////////////
	// for one stage event creation
	IceEventSportsBetCashout( INT64 eventId_, INT32 sourceId_ )
		: IceEventBaseSportsBet( DBM_Q_OLAP_ICE_EVENT_SPORTS_BET_CASHOUT, eIceEvent_SportsBetCashout, eventId_, sourceId_ ) {}

	//////////////////////////////////////////////////////////////////////////

	void addSportsReturnAmount		(INT64	val) { addParam( IceParamData(eIceEventParam_SportsWinAmount			, true, val							) ); }
	void addSportsBetReturn			(INT32	val) { addParam( IceParamData(eIceEventParam_SportsBetReturn			, true, val							) ); }
	void addSportsBetNetWin			(INT64	val) { addParam( IceParamData(eIceEventParam_SportsBetNetWin			, true, val							) ); }
	void addSportsBetNetLoss		(INT64	val) { addParam( IceParamData(eIceEventParam_SportsBetNetLoss			, true, val							) ); }
};

//////////////////////////////////////////////////////////////////////////

class IceEventBonus : public IceEvent	// PYR-34344
{
public:

	IceEventBonus(INT64 eventId_, INT32 sourceId_) : IceEvent(DBM_Q_OLAP_ICE_EVENT_BONUS, eIceEvent_Bonus, eventId_, sourceId_){}

	
	void addBonusId	( INT32	val ) { addParam( IceParamData( eIceEventParam_BonusId	, true , val ) ); }
	
	// generated in dbm
public:
	IceEvent::composeForIceOlap;
private:
	IceEvent::composeForDbm;
	IceEvent::reComposeForIceOlap;
};

//////////////////////////////////////////////////////////////////////////

class IceEventFirstLTPoints : public IceEvent	// PYR-40096
{
public:

	IceEventFirstLTPoints(INT64 eventId_, INT32 sourceId_)
		: IceEvent(DBM_Q_OLAP_ICE_EVENT_FIRST_LT_POINTS, eIceEvent_FirstLTPoints, eventId_, sourceId_){}


	void addFirstLTPointsType	( INT8	val ) { addParam( IceParamData( eIceEventParam_FirstLTPointsType, true , val ) ); }
	void addAppLoginId			( INT64	val ) { addParam( IceParamData( eIceEventParam_AppLoginId,		  true , val ) ); }

	// generated in dbm
public:
	IceEvent::composeForIceOlap;
private:
	IceEvent::composeForDbm;
	IceEvent::reComposeForIceOlap;
};

//////////////////////////////////////////////////////////////////////////

class IceEventUserTrackCodeChange : public IceEvent	// PYR-40096
{
public:

	IceEventUserTrackCodeChange(INT64 eventId_, INT32 sourceId_)
		: IceEvent(DBM_Q_OLAP_ICE_EVENT_USER_TRACK_CODE_CHANGE, eIceEvent_UserTrackCodeChange, eventId_, sourceId_){}


	void addTrackCode		( const char*	val ) { addParam( IceParamData( eIceEventParam_TrackCode		, true , val ) ); }
	void addTrackCodeChange	( INT8	val			) { addParam( IceParamData( eIceEventParam_TrackCodeChange	, true , val ) ); }
		
	// generated in dbm
public:
	IceEvent::composeForIceOlap;
private:
	IceEvent::composeForDbm;
	IceEvent::reComposeForIceOlap;
};

//////////////////////////////////////////////////////////////////////////

class IceEventMGBase : public IceEvent	// PYR-40096
{
public:

	IceEventMGBase( UINT32 olapMsgId_, INT16 eventType_, INT64 eventId_, INT32 sourceId_ ) 
		: IceEvent( olapMsgId_, eventType_, eventId_, sourceId_ ){}

	void addMiniGameRef		( const char*	val ) { addParam( IceParamData( eIceEventParam_MiniGameRef		, true , val ) ); }

	// generated in dbm
public:
	IceEvent::composeForIceOlap;
private:
	IceEvent::composeForDbm;
	IceEvent::reComposeForIceOlap;
};

class IceEventMGCompleted : public IceEventMGBase	// PYR-40096
{
public:

	IceEventMGCompleted( INT64 eventId_, INT32 sourceId_ )
		: IceEventMGBase( DBM_Q_OLAP_ICE_EVENT_MG_COMPLETED, eIceEvent_MGCompleted, eventId_, sourceId_ ){}

	void addMiniGameResult	( INT8	val			) { addParam( IceParamData( eIceEventParam_MiniGameResult	, true , val ) ); }
};

class IceEventMGPrizeAwarded : public IceEventMGBase	// PYR-40096
{
public:

	IceEventMGPrizeAwarded(INT64 eventId_, INT32 sourceId_)
		: IceEventMGBase( DBM_Q_OLAP_ICE_EVENT_MG_PRIZE_AWARDED, eIceEvent_MGPrizeAwarded, eventId_, sourceId_ ){}

	void addMiniGamePrizeType	( INT32	val		) { addParam( IceParamData( eIceEventParam_MiniGamePrizeType		, true , val	) ); }
	void addMiniGamePrizeAmount	( INT64	val		) { addParam( IceParamData( eIceEventParam_MiniGamePrizeAmount		, true , val	) ); }				
	void addMiniGamePrizeRefId	( INT64	val		) { addParam( IceParamData( eIceEventParam_MiniGamePrizeRefId		, true , val	) ); }				
};

//////////////////////////////////////////////////////////////////////////

class IceEventChestEarn : public IceEvent	// PYR-56058
{
public:

	IceEventChestEarn( INT64 eventId_, INT32 sourceId_ )
		: IceEvent( DBM_Q_OLAP_ICE_EVENT_CHEST_EARN, eIceEvent_ChestEarn, eventId_, sourceId_ ){}


	void addChestTier			( INT8	val			) { addParam( IceParamData( eIceEventParam_ChestTier		, true , val ) ); }
	void addChestExchangeType	( INT8	val			) { addParam( IceParamData( eIceEventParam_ChestExchangeType, true , val ) ); }
	void addChestLevelUp		( bool	val			) 
	{ 
		INT8 val8 = val ? 1 : 0; 
		addParam( IceParamData( eIceEventParam_ChestLevelUp, true , val8 ) ); 
	}

	// generated in dbm
public:
	IceEvent::composeForIceOlap;
private:
	IceEvent::composeForDbm;
	IceEvent::reComposeForIceOlap;
};
//////////////////////////////////////////////////////////////////////////

class IceEventMpChestIssued : public IceEvent	// PYR-62775
{
public:

	IceEventMpChestIssued( INT64 eventId_, INT32 sourceId_ )
		: IceEvent( DBM_Q_OLAP_ICE_EVENT_MPC_CHEST_CREDITED, eIceEvent_MpChestIssued, eventId_, sourceId_ ){}

	void addChestTypeId( INT32	val	) { addParam( IceParamData( eIceEventParam_ChestTypeId		, true , val ) ); }
	void addChestIssuer( INT8	val	) { addParam( IceParamData( eIceEventParam_ChestIssuerType	, true , val ) ); }

	// generated in dbm
public:
	IceEvent::composeForIceOlap;
private:
	IceEvent::composeForDbm;
	IceEvent::reComposeForIceOlap;
};
//////////////////////////////////////////////////////////////////////////

class IceEventDreamCatcherGameRound : public IceEvent	// PYR-59835
{
public:
	// for creation in casino
	IceEventDreamCatcherGameRound() 
		: IceEvent( DBM_Q_OLAP_ICE_EVENT_DREAMCATCHER_GAME_ROUND, eIceEvent_DreamCatcherGameRound ){}

	IceEventDreamCatcherGameRound( INT64 eventId_, INT32 sourceId_ ) 
		: IceEvent( DBM_Q_OLAP_ICE_EVENT_DREAMCATCHER_GAME_ROUND, eIceEvent_DreamCatcherGameRound, eventId_, sourceId_ ){}

	IceEventDreamCatcherGameRound( CommMsgParser& parser, INT64 eventId_, INT32 sourceId_ ) 
		: IceEvent( DBM_Q_OLAP_ICE_EVENT_DREAMCATCHER_GAME_ROUND, eIceEvent_DreamCatcherGameRound )
	{
		parseByDbm(parser, eventId_, sourceId_);
	}

public:
	void addPlayType					( INT8	val			) { addParam( IceParamData( eIceEventParam_GameFlags8					, true , val, (INT8)0 /*mask*/ ) ); } // see enum eIceEventParam_GameFlags8_Bits
	void addCPPAmount					( INT64	val			) { addParam( IceParamData( eIceEventParam_CPPAmount					, true , val ) ); }	
	void addCasinoBetFundSource			( INT8	val			) { addParam( IceParamData( eIceEventParam_CasinoBetFundSource			, true , val ) ); }
	void addCasinoBetFundSourceBonusId	( INT32	val			) { addParam( IceParamData( eIceEventParam_CasinoBetFundSourceBonusId	, true , val ) ); }

	void addDreamCatcherBetAmount		( INT64	val			) { addParam( IceParamData( eIceEventParam_DreamCatcherBetAmount		, true , val ) ); }			
	void addDreamCatcherBetCurrency		( const char*	val ) { addParam( IceParamData( eIceEventParam_DreamCatcherBetCurrency      , true , val ) ); }
	void addDreamCatcherGrossWinAmount	( INT64	val			) { addParam( IceParamData( eIceEventParam_DreamCatcherGrossWinAmount	, true , val ) ); }	
	void addDreamCatcherNetWinAmount	( INT64	val			) { addParam( IceParamData( eIceEventParam_DreamCatcherNetWinAmount		, true , val ) ); }		
	void addDreamCatcherNetLossAmount	( INT64	val			) { addParam( IceParamData( eIceEventParam_DreamCatcherNetLossAmount	, true , val ) ); }		
	void addDreamCatcherBetResult		( INT8	val			) { addParam( IceParamData( eIceEventParam_DreamCatcherBetResult		, true , val, ( INT8 )0 /*mask*/		) ); }	// eIceEventParam_SportBetResultFlag8			
	void addDreamCatcherNumberSpun		( INT8	val			) { addParam( IceParamData( eIceEventParam_DreamCatcherNumberSpun		, true , val ) ); }	// only a single value allowed; see enum eIceEventParam_DreamCatcherNumber_Flags8_Bits for allowed values
	void addDreamCatcherNumBetsPlaced	( INT16	val			) { addParam( IceParamData( eIceEventParam_DreamCatcherNumBetsPlaced	, true , val ) ); }	
	void addDreamCatcherBetNumber		( INT8	val			) { addParam( IceParamData( eIceEventParam_DreamCatcherBetNumber		, true , val, ( INT8 )0 /*mask*/		) ); }  // see enum eIceEventParam_DreamCatcherNumber_Flags8_Bits
	void addDreamCatcherSpinReturn		( INT32	val			) { addParam( IceParamData( eIceEventParam_DreamCatcherSpinReturn		, true , val ) ); }	
	void addDreamCatcherBetReturn		( INT32	val			) { addParam( IceParamData( eIceEventParam_DreamCatcherBetReturn		, true , val ) ); }	
	void addDreamCatcherMultiplier		( const bool	val )
	{
		INT8 val8 = val ? 1 : 0;
		addParam( IceParamData( eIceEventParam_DreamCatcherMultiplier, true , val8 ) );
	}

	// generated in dbm
public:
	IceEvent::composeForIceOlap;
	IceEvent::composeForDbm;
private:
	IceEvent::reComposeForIceOlap;
};

class IceEventPredictorTicketCredited : public IceEvent // PYR-63218
{
public:
	IceEventPredictorTicketCredited(INT64 eventId_, INT32 sourceId_) 
		: IceEvent(DBM_Q_OLAP_ICE_EVENT_PREDICTOR_TICKET_CREDITED, eIceEvent_PredictorTicketCredited, eventId_, sourceId_){}

	// generated in dbm
public:
	IceEvent::composeForIceOlap;
private:
	IceEvent::composeForDbm;
	IceEvent::reComposeForIceOlap;
};

class IceEventLeaderBoardSeasonFinish : public IceEvent // PYR-62728
{
public:
	IceEventLeaderBoardSeasonFinish(INT64 eventId_, INT32 sourceId_) 
		: IceEvent(DBM_Q_OLAP_ICE_EVENT_LEADERBOARD_SEASON_FINISH, eIceEvent_LeaderBoardSeasonFinish, eventId_, sourceId_){}

	void addTaskRefName			( const char*	val ) { addParam( IceParamData( eIceEventParam_TaskRefName				, true , val ) ); }
	void addTaskId				( INT32	val			) { addParam( IceParamData( eIceEventParam_TaskId					, true , val ) ); }	
	void addBoardId				( INT32	val			) { addParam( IceParamData( eIceEventParam_BoardId					, true , val ) ); }		
	void addSeasonId			( INT32	val			) { addParam( IceParamData( eIceEventParam_BoardSeasonId			, true , val ) ); }		
	void addSeasonRank			( INT32	val			) { addParam( IceParamData( eIceEventParam_BoardSeasonRank			, true , val ) ); }
	void addSeasonScore			( INT64	val			) { addParam( IceParamData( eIceEventParam_BoardSeasonScore			, true , val ) ); }
	void addSeasonPercentile	( INT16	val			) { addParam( IceParamData( eIceEventParam_BoardSeasonRankPercentile, true , val ) ); }

	// generated in dbm
public:
	IceEvent::composeForIceOlap;
private:
	IceEvent::composeForDbm;
	IceEvent::reComposeForIceOlap;
};

// PYR-66899
class IceEventRingGameBuyin : public IceEvent
{
public:
	IceEventRingGameBuyin() : IceEvent(DBM_Q_OLAP_ICE_EVENT_RING_GAME_BUYIN, eIceEvent_RingGameBuyin) {}

public:

	void addGameFlags		( INT8 val		)	{ addParam( IceParamData( eIceEventParam_GameFlags8			, true , val, (INT8)0 /*mask*/) ); } // see enum eIceEventParam_GameFlags8_Bits
	void addTableId			( INT64	val		)	{ addParam( IceParamData( eIceEventParam_TableId			, true , val ) ); }
	void addMergedTableId	( INT64	val		)	{ addParam( IceParamData( eIceEventParam_MergedTableId		, true , val ) ); }
	void addRingLowStake	( INT32	val		)	{ addParam( IceParamData( eIceEventParam_RingLowStake		, true , val ) ); }
	void addRingHighStake	( INT32	val		)	{ addParam( IceParamData( eIceEventParam_RingHighStake		, true , val ) ); }
	void addRingBuyInTime	( SrvTime& val	)	{ addParam( IceParamData( eIceEventParam_RingBuyInTime		, true , val ) ); }
	void addRingHandFlags	( INT8 val		)	{ addParam( IceParamData( eIceEventParam_RingHandFlags8		, true , val, (INT8)0 /*mask*/) ); } // see enum eIceEventParam_RingHandFlags8_Bits
	void addRingCap			( INT32	val		)	{ addParam( IceParamData( eIceEventParam_RingCap			, true , val ) ); }
	void addRingAnte		( INT32	val		)	{ addParam( IceParamData( eIceEventParam_RingAnte			, true , val ) ); }
	void addMultiGameType	( INT32	val		)	{ addParam( IceParamData( eIceEventParam_RingMultiGameType	, true , val ) ); }
	void addStructure		( INT8	val		)	{ addParam( IceParamData( eIceEventParam_Structure			, true , val ) ); }
	void addPlayersPerTable	( INT8	val		)	{ addParam( IceParamData( eIceEventParam_PlayersPerTable	, true , val ) ); }
	void addRingCurrency	( const char* val )	{ addParam( IceParamData( eIceEventParam_RingCurrency		, true , val ) ); }


// generated outside of dbm
public:
	IceEvent::composeForDbm;
};

// PYR-67812
class IceEventBaseCasinoTexasHoldemBonus : public IceEvent
{
protected:

	IceEventBaseCasinoTexasHoldemBonus( UINT32 msgId_, INT16 eventType_ )
		: IceEvent( msgId_, eventType_ )
	{
	}

	IceEventBaseCasinoTexasHoldemBonus( UINT32 olapMsgId_, INT16 eventType_, INT64 eventId_, INT32 sourceId_ )
		: IceEvent( olapMsgId_, eventType_, eventId_, sourceId_ )
	{
	}

public:
	void addPlayType								( INT8	val			) { addParam( IceParamData( eIceEventParam_GameFlags8						, true , val  , (INT8)0 /*mask*/ ) ); } // see enum eIceEventParam_GameFlags8_Bits
	void addCPPAmount								( INT64	val			) { addParam( IceParamData( eIceEventParam_CPPAmount						, true , val					) ); }		
	void addCasinoBetFundSource						( INT8	val			) { addParam( IceParamData( eIceEventParam_CasinoBetFundSource				, true , val ) ); }
	void addCasinoBetFundSourceBonusId				( INT32	val			) { addParam( IceParamData( eIceEventParam_CasinoBetFundSourceBonusId		, true , val ) ); }

	void addTexasHoldemBonusBetAmount				( INT64	val			) { addParam( IceParamData( eIceEventParam_TexasHoldemBonusBetAmount		, true , val ) ); }
	void addTexasHoldemBonusBetCurrency				( const char*	val ) { addParam( IceParamData( eIceEventParam_TexasHoldemBonusBetCurrency      , true , val ) ); }
	void addTexasHoldemBonusGrossWinAmount			( INT64	val			) { addParam( IceParamData( eIceEventParam_TexasHoldemBonusGrossWinAmount	, true , val ) ); }
	void addTexasHoldemBonusNetWinAmount			( INT64	val			) { addParam( IceParamData( eIceEventParam_TexasHoldemBonusNetWinAmount		, true , val ) ); }
	void addTexasHoldemBonusNetLossAmount			( INT64	val			) { addParam( IceParamData( eIceEventParam_TexasHoldemBonusNetLossAmount	, true , val ) ); }
	void addTexasHoldemBonusResult					( INT8	val			) { addParam( IceParamData( eIceEventParam_TexasHoldemBonusResult			, true , val, ( INT8 )0 /*mask*/		) ); }	// eIceEventParam_SportBetResultFlag8			
	void addTexasHoldemBonusPlayerCards				( INT64	val			) { addParam( IceParamData( eIceEventParam_TexasHoldemBonusPlayerCards		, true , val, ( INT64 )0 /*mask*/ ) ); }
	void addTexasHoldemBonusDealerCards				( INT64	val			) { addParam( IceParamData( eIceEventParam_TexasHoldemBonusDealerCards		, true , val, ( INT64 )0 /*mask*/ ) ); }
	void addTexasHoldemBonusBoardCards				( INT64	val			) { addParam( IceParamData( eIceEventParam_TexasHoldemBonusBoardCards		, true , val, ( INT64 )0 /*mask*/ ) ); }
	void addTexasHoldemBonusPlayerHandStrenghtRank	( INT8	val			) { addParam( IceParamData( eIceEventParam_TexasHoldemBonusHandStrengthRank	, true , val ) ); }
	void addTexasHoldemBonusBetReturn				( INT32	val			) { addParam( IceParamData( eIceEventParam_TexasHoldemBonusBetReturn		, true , val ) ); }
	
	// generated in dbm
public:
	IceEvent::composeForIceOlap;
	IceEvent::composeForDbm;
private:
	IceEvent::reComposeForIceOlap;
};

// PYR-67812
class IceEventCasinoTexasHoldemBonusHand : public IceEventBaseCasinoTexasHoldemBonus
{
public:
	// for creation in casino
	IceEventCasinoTexasHoldemBonusHand()
		: IceEventBaseCasinoTexasHoldemBonus( DBM_Q_OLAP_ICE_EVENT_CASINO_TEXAS_HOLDEM_BONUS_HAND, eIceEvent_CasinoTexasHoldemBonusHand )
	{
	}

	// construct from casino - before it was recomposed for ice olap
	// done in dbm so the dbm could only call addCPPAmount() since the rest of the data
	// should already be present in the intermediate event composed by the casino
	//
	IceEventCasinoTexasHoldemBonusHand( CommMsgParser& parser, INT64 eventId_, INT32 sourceId_ )
		: IceEventBaseCasinoTexasHoldemBonus( DBM_Q_OLAP_ICE_EVENT_CASINO_TEXAS_HOLDEM_BONUS_HAND, eIceEvent_CasinoTexasHoldemBonusHand )
	{
		parseByDbm( parser, eventId_, sourceId_ );
	}
	//////////////////////////////////////////////////////////////////////////
	// for one stage event creation - if not going through OLTP but created and posted directly to ice from casino
	IceEventCasinoTexasHoldemBonusHand( INT64 eventId_, INT32 sourceId_ )
		: IceEventBaseCasinoTexasHoldemBonus( DBM_Q_OLAP_ICE_EVENT_CASINO_TEXAS_HOLDEM_BONUS_HAND, eIceEvent_CasinoTexasHoldemBonusHand, eventId_, sourceId_ )
	{
	}
	//////////////////////////////////////////////////////////////////////////
};

// PYR-67812
class IceEventCasinoTexasHoldemBonusBet : public IceEventBaseCasinoTexasHoldemBonus
{
public:
	// for creation in casino
	IceEventCasinoTexasHoldemBonusBet()
		: IceEventBaseCasinoTexasHoldemBonus( DBM_Q_OLAP_ICE_EVENT_CASINO_TEXAS_HOLDEM_BONUS_BET, eIceEvent_CasinoTexasHoldemBonusBet )
	{
	}

	// construct from casino - before it was recomposed for ice olap
	// done in dbm so the dbm could only call addCPPAmount() since the rest of the data
	// should already be present in the intermediate event composed by the casino
	//
	IceEventCasinoTexasHoldemBonusBet( CommMsgParser& parser, INT64 eventId_, INT32 sourceId_ )
		: IceEventBaseCasinoTexasHoldemBonus( DBM_Q_OLAP_ICE_EVENT_CASINO_TEXAS_HOLDEM_BONUS_BET, eIceEvent_CasinoTexasHoldemBonusBet )
	{
		parseByDbm( parser, eventId_, sourceId_ );
	}
	//////////////////////////////////////////////////////////////////////////
	// for one stage event creation - if not going through OLTP but created and posted directly to ice from casino
	IceEventCasinoTexasHoldemBonusBet( INT64 eventId_, INT32 sourceId_ )
		: IceEventBaseCasinoTexasHoldemBonus( DBM_Q_OLAP_ICE_EVENT_CASINO_TEXAS_HOLDEM_BONUS_BET, eIceEvent_CasinoTexasHoldemBonusBet, eventId_, sourceId_ )
	{
	}
	//////////////////////////////////////////////////////////////////////////

	void addTexasHoldemBonusBetType( INT8	val ) { addParam( IceParamData( eIceEventParam_TexasHoldemBonusBetType, true, val, ( INT8 )0 /*mask*/ ) ); } // see enum eIceEventParam_TexasHoldemBonusBetTypeFlag8
};

// PYR-66738
class IceEventBaseCasinoBaccarat : public IceEvent
{
protected:

	IceEventBaseCasinoBaccarat( UINT32 msgId_, INT16 eventType_ )
		: IceEvent( msgId_, eventType_ )
	{
	}

	IceEventBaseCasinoBaccarat( UINT32 olapMsgId_, INT16 eventType_, INT64 eventId_, INT32 sourceId_ )
		: IceEvent( olapMsgId_, eventType_, eventId_, sourceId_ )
	{
	}

public:
	void addPlayType					( INT8	val			) { addParam( IceParamData( eIceEventParam_GameFlags8					, true, val, ( INT8 )0 /*mask*/ ) ); } // see enum eIceEventParam_GameFlags8_Bits
	void addCPPAmount					( INT64	val			) { addParam( IceParamData( eIceEventParam_CPPAmount					, true, val ) ); }
	void addCasinoGameType				( INT32	val			) { addParam( IceParamData( eIceEventParam_CasinoGameType				, true, val ) ); }			
	void addCasinoGameVariant			( INT32	val			) { addParam( IceParamData( eIceEventParam_CasinoGameVariant			, true, val ) ); }		
	void addCasinoGameVendor			( INT32	val			) { addParam( IceParamData( eIceEventParam_CasinoGameVendor				, true, val ) ); }				
	
	void addCasinoBetFundSource			( INT8	val			) { addParam( IceParamData( eIceEventParam_CasinoBetFundSource			, true, val ) ); }
	void addCasinoBetFundSourceBonusId	( INT32	val			) { addParam( IceParamData( eIceEventParam_CasinoBetFundSourceBonusId	, true, val ) ); }

	void addBaccaratBetAmount			( INT64	val			) { addParam( IceParamData( eIceEventParam_BaccaratBetAmount			, true, val ) ); }
	void addBaccaratBetCurrency			( const char*	val ) { addParam( IceParamData( eIceEventParam_BaccaratBetCurrency			, true, val ) ); }
	void addBaccaratGrossWinAmount		( INT64	val			) { addParam( IceParamData( eIceEventParam_BaccaratGrossWinAmount		, true, val ) ); }
	void addBaccaratNetWinAmount		( INT64	val			) { addParam( IceParamData( eIceEventParam_BaccaratNetWinAmount			, true, val ) ); }
	void addBaccaratNetLossAmount		( INT64	val			) { addParam( IceParamData( eIceEventParam_BaccaratNetLossAmount		, true, val ) ); }
	void addBaccaratBetResult			( INT8	val			) { addParam( IceParamData( eIceEventParam_BaccaratBetResult			, true, val, ( INT8 )0 /*mask*/ ) ); }	// eIceEventParam_SportBetResultFlag8			
	void addBaccaratPlayerCard			( INT64	val			) { addParam( IceParamData( eIceEventParam_BaccaratPlayerCard			, true, val, ( INT64 )0 /*mask*/ ) ); }
	void addBaccaratBankerCard			( INT64	val			) { addParam( IceParamData( eIceEventParam_BaccaratBankerCard			, true, val, ( INT64 )0 /*mask*/ ) ); }
	void addBaccaratPlayerTotal			( INT8	val			) { addParam( IceParamData( eIceEventParam_BaccaratPlayerTotal			, true, val ) ); }
	void addBaccaratBankerTotal			( INT8	val			) { addParam( IceParamData( eIceEventParam_BaccaratBankerTotal			, true, val ) ); }


	// generated in dbm
public:
	IceEvent::composeForIceOlap;
	IceEvent::composeForDbm;
private:
	IceEvent::reComposeForIceOlap;
};

// PYR-67812
class IceEventCasinoBaccaratHand : public IceEventBaseCasinoBaccarat
{
public:
	// for creation in casino
	IceEventCasinoBaccaratHand()
		: IceEventBaseCasinoBaccarat( DBM_Q_OLAP_ICE_EVENT_CASINO_BACCARAT_HAND, eIceEvent_CasinoBaccaratHand )
	{
	}

	// construct from casino - before it was recomposed for ice olap
	// done in dbm so the dbm could only call addCPPAmount() since the rest of the data
	// should already be present in the intermediate event composed by the casino
	//
	IceEventCasinoBaccaratHand( CommMsgParser& parser, INT64 eventId_, INT32 sourceId_ )
		: IceEventBaseCasinoBaccarat( DBM_Q_OLAP_ICE_EVENT_CASINO_BACCARAT_HAND, eIceEvent_CasinoBaccaratHand )
	{
		parseByDbm( parser, eventId_, sourceId_ );
	}
	//////////////////////////////////////////////////////////////////////////
	// for one stage event creation - if not going through OLTP but created and posted directly to ice from casino
	IceEventCasinoBaccaratHand( INT64 eventId_, INT32 sourceId_ )
		: IceEventBaseCasinoBaccarat( DBM_Q_OLAP_ICE_EVENT_CASINO_BACCARAT_HAND, eIceEvent_CasinoBaccaratHand, eventId_, sourceId_ )
	{
	}
	//////////////////////////////////////////////////////////////////////////

	void addBaccaratNumBetsPlaced( INT16	val ) { addParam( IceParamData( eIceEventParam_BaccaratNumBetsPlaced, true, val ) ); }
};

// PYR-67812
class IceEventCasinoBaccaratBet : public IceEventBaseCasinoBaccarat
{
public:
	// for creation in casino
	IceEventCasinoBaccaratBet()
		: IceEventBaseCasinoBaccarat( DBM_Q_OLAP_ICE_EVENT_CASINO_BACCARAT_BET, eIceEvent_CasinoBaccaratBet )
	{
	}

	// construct from casino - before it was recomposed for ice olap
	// done in dbm so the dbm could only call addCPPAmount() since the rest of the data
	// should already be present in the intermediate event composed by the casino
	//
	IceEventCasinoBaccaratBet( CommMsgParser& parser, INT64 eventId_, INT32 sourceId_ )
		: IceEventBaseCasinoBaccarat( DBM_Q_OLAP_ICE_EVENT_CASINO_BACCARAT_BET, eIceEvent_CasinoBaccaratBet )
	{
		parseByDbm( parser, eventId_, sourceId_ );
	}
	//////////////////////////////////////////////////////////////////////////
	// for one stage event creation - if not going through OLTP but created and posted directly to ice from casino
	IceEventCasinoBaccaratBet( INT64 eventId_, INT32 sourceId_ )
		: IceEventBaseCasinoBaccarat( DBM_Q_OLAP_ICE_EVENT_CASINO_BACCARAT_BET, eIceEvent_CasinoBaccaratBet, eventId_, sourceId_ )
	{
	}
	//////////////////////////////////////////////////////////////////////////

	void addBaccaratBetType( INT16	val ) { addParam( IceParamData( eIceEventParam_BaccaratBetType, true, val, ( INT16 )0 /*mask*/ ) ); } // see enum eIceEventParam_BaccaratBetTypeFlag16
};

// PYR-70530
class IceEventRingCashOut: public IceEvent
{
public:
	IceEventRingCashOut(): IceEvent(DBM_Q_OLAP_ICE_EVENT_RING_CASH_OUT, eIceEvent_RingGameCashOut) {}

public:
	void addGameFlags			( INT8 val	) { addParam( IceParamData( eIceEventParam_GameFlags8			, true , val, (INT8)0 /*mask*/) ); } // see enum eIceEventParam_GameFlags8_Bits
	void addTableId				( INT64	val ) { addParam( IceParamData( eIceEventParam_TableId				, true , val ) ); }
	void addMergedTableId		( INT64	val ) { addParam( IceParamData( eIceEventParam_MergedTableId		, true , val ) ); }

	// generated outside of dbm
public:
	IceEvent::composeForDbm;

	// generated in dbm
public:
	IceEvent::composeForIceOlap;
private:
	IceEvent::reComposeForIceOlap;
};

// PYR-70533
class IceEventTournRegister: public IceEvent
{
public:
	IceEventTournRegister(): IceEvent(DBM_Q_OLAP_ICE_EVENT_TOURN_REGISTER, eIceEvent_TournRegister) {}

public:
	void addGameFlags			( INT8 val	) { addParam( IceParamData( eIceEventParam_GameFlags8			, true , val, (INT8)0 /*mask*/) ); } // see enum eIceEventParam_GameFlags8_Bits
	void addTournId				( INT64	val ) { addParam( IceParamData( eIceEventParam_TournId				, true , val ) ); }

	// generated outside of dbm
public:
	IceEvent::composeForDbm;

	// generated in dbm
public:
	IceEvent::composeForIceOlap;
private:
	IceEvent::reComposeForIceOlap;
};

// PYR-70533
class IceEventTournUnregister: public IceEvent
{
public:
	IceEventTournUnregister(): IceEvent(DBM_Q_OLAP_ICE_EVENT_TOURN_UNREGISTER, eIceEvent_TournUnregister) {}

public:
	void addGameFlags			( INT8 val	) { addParam( IceParamData( eIceEventParam_GameFlags8			, true , val, (INT8)0 /*mask*/) ); } // see enum eIceEventParam_GameFlags8_Bits
	void addTournId				( INT64	val ) { addParam( IceParamData( eIceEventParam_TournId				, true , val ) ); }

	// generated outside of dbm
public:
	IceEvent::composeForDbm;

	// generated in dbm
public:
	IceEvent::composeForIceOlap;
private:
	IceEvent::reComposeForIceOlap;
};

// PYR-66224
class IceEventBaseCasinoDragonTiger : public IceEvent
{
protected:

	IceEventBaseCasinoDragonTiger( UINT32 msgId_, INT16 eventType_ )
		: IceEvent( msgId_, eventType_ )
	{
	}

	IceEventBaseCasinoDragonTiger( UINT32 olapMsgId_, INT16 eventType_, INT64 eventId_, INT32 sourceId_ )
		: IceEvent( olapMsgId_, eventType_, eventId_, sourceId_ )
	{
	}

public:
	void addPlayType					( INT8	val			) { addParam( IceParamData( eIceEventParam_GameFlags8					, true, val, ( INT8 )0 /*mask*/ ) ); } // see enum eIceEventParam_GameFlags8_Bits
	void addCPPAmount					( INT64	val			) { addParam( IceParamData( eIceEventParam_CPPAmount					, true, val ) ); }
	void addCasinoBetFundSource			( INT8	val			) { addParam( IceParamData( eIceEventParam_CasinoBetFundSource			, true, val ) ); }
	void addCasinoBetFundSourceBonusId	( INT32	val			) { addParam( IceParamData( eIceEventParam_CasinoBetFundSourceBonusId	, true, val ) ); }

	void addDragonTigerBetAmount		( INT64	val			) { addParam( IceParamData( eIceEventParam_DragonTigerBetAmount			, true, val ) ); }
	void addDragonTigerBetCurrency		( const char*	val ) { addParam( IceParamData( eIceEventParam_DragonTigerBetCurrency		, true, val ) ); }
	void addDragonTigerGrossWinAmount	( INT64	val			) { addParam( IceParamData( eIceEventParam_DragonTigerGrossWinAmount	, true, val ) ); }
	void addDragonTigerNetWinAmount		( INT64	val			) { addParam( IceParamData( eIceEventParam_DragonTigerNetWinAmount		, true, val ) ); }
	void addDragonTigerNetLossAmount	( INT64	val			) { addParam( IceParamData( eIceEventParam_DragonTigerNetLossAmount		, true, val ) ); }
	void addDragonTigerBetResult		( INT8	val			) { addParam( IceParamData( eIceEventParam_DragonTigerBetResult			, true, val, ( INT8 )0 /*mask*/ ) ); }	// eIceEventParam_SportBetResultFlag8			
	void addDragonCard					( INT64	val			) { addParam( IceParamData( eIceEventParam_DragonCard					, true, val, ( INT64 )0 /*mask*/ ) ); }
	void addTigerCard					( INT64	val			) { addParam( IceParamData( eIceEventParam_TigerCard					, true, val, ( INT64 )0 /*mask*/ ) ); }
	void addDragonTigerNumBetsPlaced	( INT16	val			) { addParam( IceParamData( eIceEventParam_DragonTigerNumBetsPlaced		, true, val ) ); }

	// generated in dbm
public:
	IceEvent::composeForIceOlap;
	IceEvent::composeForDbm;
private:
	IceEvent::reComposeForIceOlap;
};

// PYR-66224
class IceEventCasinoDragonTigerHand : public IceEventBaseCasinoDragonTiger
{
public:
	// for creation in casino
	IceEventCasinoDragonTigerHand()
		: IceEventBaseCasinoDragonTiger( DBM_Q_OLAP_ICE_EVENT_CASINO_DRAGON_TIGER_HAND, eIceEvent_CasinoDragonTigerHand )
	{
	}

	// construct from casino - before it was recomposed for ice olap
	// done in dbm so the dbm could only call addCPPAmount() since the rest of the data
	// should already be present in the intermediate event composed by the casino
	//
	IceEventCasinoDragonTigerHand( CommMsgParser& parser, INT64 eventId_, INT32 sourceId_ )
		: IceEventBaseCasinoDragonTiger( DBM_Q_OLAP_ICE_EVENT_CASINO_DRAGON_TIGER_HAND, eIceEvent_CasinoDragonTigerHand )
	{
		parseByDbm( parser, eventId_, sourceId_ );
	}
	//////////////////////////////////////////////////////////////////////////
	// for one stage event creation - if not going through OLTP but created and posted directly to ice from casino
	IceEventCasinoDragonTigerHand( INT64 eventId_, INT32 sourceId_ )
		: IceEventBaseCasinoDragonTiger( DBM_Q_OLAP_ICE_EVENT_CASINO_DRAGON_TIGER_HAND, eIceEvent_CasinoDragonTigerHand, eventId_, sourceId_ )
	{
	}
	//////////////////////////////////////////////////////////////////////////

	
};

// PYR-66224
class IceEventCasinoDragonTigerBet : public IceEventBaseCasinoDragonTiger
{
public:
	// for creation in casino
	IceEventCasinoDragonTigerBet()
		: IceEventBaseCasinoDragonTiger( DBM_Q_OLAP_ICE_EVENT_CASINO_DRAGON_TIGER_BET, eIceEvent_CasinoDragonTigerBet )
	{
	}

	// construct from casino - before it was recomposed for ice olap
	// done in dbm so the dbm could only call addCPPAmount() since the rest of the data
	// should already be present in the intermediate event composed by the casino
	//
	IceEventCasinoDragonTigerBet( CommMsgParser& parser, INT64 eventId_, INT32 sourceId_ )
		: IceEventBaseCasinoDragonTiger( DBM_Q_OLAP_ICE_EVENT_CASINO_DRAGON_TIGER_BET, eIceEvent_CasinoDragonTigerBet )
	{
		parseByDbm( parser, eventId_, sourceId_ );
	}
	//////////////////////////////////////////////////////////////////////////
	// for one stage event creation - if not going through OLTP but created and posted directly to ice from casino
	IceEventCasinoDragonTigerBet( INT64 eventId_, INT32 sourceId_ )
		: IceEventBaseCasinoDragonTiger( DBM_Q_OLAP_ICE_EVENT_CASINO_DRAGON_TIGER_BET, eIceEvent_CasinoDragonTigerBet, eventId_, sourceId_ )
	{
	}
	//////////////////////////////////////////////////////////////////////////

	void addDragonTigerBetType			( INT8	val ) { addParam( IceParamData( eIceEventParam_DragonTigerBetType			, true, val, ( INT8 )0 /*mask*/ ) ); } // see enum eIceEventParam_DragonTigerBetTypeFlags8_Bits
};

// PYR-82915
// Since IceEventVictoryTribesSlotSpin inherits from IceEventBaseCasinoSlotSpin this class does not inherit from IceEvent because that would lead to multiple inheritance from the same IceEvent class 
class IceEventBaseVictoryTribes
{
	IceEvent& iceEvent;
public:
	IceEventBaseVictoryTribes( IceEvent* iceEvent_ ) : iceEvent( *iceEvent_ ) {}

	void addVictoryTribesTribeSelection	( INT8	val ) { iceEvent.addParam( IceParamData( eIceEventParam_VictoryTribesTribeSelection	, true, val ) ); }
	void addVictoryTribesLevel			( INT8	val ) { iceEvent.addParam( IceParamData( eIceEventParam_VictoryTribesLevel			, true, val ) ); }
};

#pragma warning( push )
#pragma warning( disable : 4355 ) // hide the "warning C4355: 'this': used in base member initializer list build warning"

// PYR-82915
class IceEventCasinoCommunityJackpot : public IceEvent, public IceEventBaseVictoryTribes
{
public:
	// for creation in casino
	IceEventCasinoCommunityJackpot()
		: IceEvent( DBM_Q_OLAP_ICE_EVENT_COMMUNITY_JACKPOT, eIceEvent_CasinoCommunityJackpot ), IceEventBaseVictoryTribes( this )
	{
	}

	IceEventCasinoCommunityJackpot( INT64 eventId_, INT32 sourceId_ )
		: IceEvent( DBM_Q_OLAP_ICE_EVENT_COMMUNITY_JACKPOT, eIceEvent_CasinoCommunityJackpot, eventId_, sourceId_ ), IceEventBaseVictoryTribes( this )
	{
	}

	IceEventCasinoCommunityJackpot( CommMsgParser& parser, INT64 eventId_, INT32 sourceId_ )
		: IceEvent( DBM_Q_OLAP_ICE_EVENT_COMMUNITY_JACKPOT, eIceEvent_CasinoCommunityJackpot ), IceEventBaseVictoryTribes( this )
	{
		parseByDbm( parser, eventId_, sourceId_ );
	}

	void addPlayType					( INT8	val			) { addParam( IceParamData( eIceEventParam_GameFlags8					, true , val , (INT8)0 /*mask*/ ) ); } // see enum eIceEventParam_GameFlags8_Bits
	void addCasinoGameType				( INT32	val			) { addParam( IceParamData( eIceEventParam_CasinoGameType				, true , val					) ); }			
	void addCasinoGameVariant			( INT32	val			) { addParam( IceParamData( eIceEventParam_CasinoGameVariant			, true , val					) ); }		
	
	void addJackpotWinType				( INT8	val			) { addParam( IceParamData( eIceEventParam_JackpotWinType				, true , val					) ); }		
	void addJackpoType					( INT8	val			) { addParam( IceParamData( eIceEventParam_JackpotType					, true , val					) ); }
	void addJackpotWinAmount			( INT64	val			) { addParam( IceParamData( eIceEventParam_JackpotWinAmount				, true , val					) ); }

	// generated in dbm
public:
	IceEvent::composeForIceOlap;
	IceEvent::composeForDbm;
private:
	IceEvent::reComposeForIceOlap;
};

// PYR-82915
class IceEventVictoryTribesBonusTournFinish : public IceEvent, public IceEventBaseVictoryTribes
{
public:
	// for creation in casino
	IceEventVictoryTribesBonusTournFinish()
		: IceEvent( DBM_Q_OLAP_ICE_EVENT_VICTORY_TRIBES_BONUS_TOURN_FINISH, eIceEvent_VictoryTribesBonusTournFinish ), IceEventBaseVictoryTribes( this )
	{
	}

	IceEventVictoryTribesBonusTournFinish( INT64 eventId_, INT32 sourceId_ )
		: IceEvent( DBM_Q_OLAP_ICE_EVENT_VICTORY_TRIBES_BONUS_TOURN_FINISH, eIceEvent_VictoryTribesBonusTournFinish, eventId_, sourceId_ ), IceEventBaseVictoryTribes( this )
	{
	}

	IceEventVictoryTribesBonusTournFinish( CommMsgParser& parser, INT64 eventId_, INT32 sourceId_ )
		: IceEvent( DBM_Q_OLAP_ICE_EVENT_VICTORY_TRIBES_BONUS_TOURN_FINISH, eIceEvent_VictoryTribesBonusTournFinish ), IceEventBaseVictoryTribes( this )
	{
		parseByDbm( parser, eventId_, sourceId_ );
	}

	void addPlayType						( INT8	val			) { addParam( IceParamData( eIceEventParam_GameFlags8						, true , val , (INT8)0 /*mask*/ ) ); } // see enum eIceEventParam_GameFlags8_Bits
	void addVictoryTribesBonusTournType		( INT8	val			) { addParam( IceParamData( eIceEventParam_VictoryTribesBonusTournType		, true , val					) ); }
	void addVictoryTribesTournNumPlayers	( INT32	val			) { addParam( IceParamData( eIceEventParam_VictoryTribesTournNumPlayers		, true , val					) ); }
	void addVictoryTribesTournamentCoins	( INT64	val			) { addParam( IceParamData( eIceEventParam_VictoryTribesTournamentCoins		, true , val					) ); }
	void addVictoryTribesTournRank			( INT32	val			) { addParam( IceParamData( eIceEventParam_VictoryTribesTournRank			, true , val					) ); }
	void addVictoryTribesTournRankPercentile( INT16	val			) { addParam( IceParamData( eIceEventParam_VictoryTribesTournRankPercentile	, true , val					) ); }
	void addVictoryTribesTournPrizeAmount	( INT64	val			) { addParam( IceParamData( eIceEventParam_VictoryTribesTournPrizeAmount	, true , val					) ); }

	// generated in dbm
public:
	IceEvent::composeForIceOlap;
	IceEvent::composeForDbm;
private:
	IceEvent::reComposeForIceOlap;
};

// PYR-82915
class IceEventVictoryTribesSlotSpin : public IceEventBaseCasinoSlotSpin, public IceEventBaseVictoryTribes
{
public:
	// for creation in casino
	IceEventVictoryTribesSlotSpin()
		: IceEventBaseCasinoSlotSpin( DBM_Q_OLAP_ICE_EVENT_VICTORY_TRIBES_SLOT_SPIN, eIceEvent_VictoryTribesSlotSpin ), IceEventBaseVictoryTribes( this )
	{
	}

	IceEventVictoryTribesSlotSpin( INT64 eventId_, INT32 sourceId_ )
		: IceEventBaseCasinoSlotSpin( DBM_Q_OLAP_ICE_EVENT_VICTORY_TRIBES_SLOT_SPIN, eIceEvent_VictoryTribesSlotSpin, eventId_, sourceId_ ), IceEventBaseVictoryTribes( this )
	{
	}

	IceEventVictoryTribesSlotSpin( CommMsgParser& parser, INT64 eventId_, INT32 sourceId_ )
		: IceEventBaseCasinoSlotSpin( parser, DBM_Q_OLAP_ICE_EVENT_VICTORY_TRIBES_SLOT_SPIN, eIceEvent_VictoryTribesSlotSpin, eventId_, sourceId_ ), IceEventBaseVictoryTribes( this )
	{
	}

	void addVictoryTribesLevelUp				( const bool val	) { INT8 val8 = val ? 1 : 0; addParam( IceParamData( eIceEventParam_VictoryTribesLevelUp				, true, val8 ) ); }
	void addVictoryTribesLevelDown				( const bool val	) { INT8 val8 = val ? 1 : 0; addParam( IceParamData( eIceEventParam_VictoryTribesLevelDown				, true, val8 ) ); }
	void addVictoryTribesFreeSpinsTrigger		( const bool val	) { INT8 val8 = val ? 1 : 0; addParam( IceParamData( eIceEventParam_VictoryTribesFreeSpinsTrigger		, true, val8 ) ); }
	void addVictoryTribesTribalFreeSpinsTrigger	( const bool val	) { INT8 val8 = val ? 1 : 0; addParam( IceParamData( eIceEventParam_VictoryTribesTribalFreeSpinsTrigger	, true, val8 ) ); }
};

// PYR-107193
class IceEventLogin : public IceEvent
{
public:

	IceEventLogin( INT64 eventId_, INT32 sourceId_ ) : IceEvent( DBM_Q_OLAP_ICE_EVENT_LOGIN, eIceEvent_Login, eventId_, sourceId_ ) {}

	void addAppLoginId( const INT64	val	) { addParam( IceParamData( eIceEventParam_AppLoginId,	true , val ) ); }

	// generated in dbm
public:
	IceEvent::composeForIceOlap;
private:
	IceEvent::composeForDbm;
	IceEvent::reComposeForIceOlap;
};

// PYR-112222
class IceEventTwitchAccountLinked : public IceEvent
{
public:

	IceEventTwitchAccountLinked( INT64 eventId_, INT32 sourceId_ ) : IceEvent( DBM_Q_OLAP_ICE_EVENT_TWITCH_ACCOUNT_LINKED, eIceEvent_TwitchAccountLinked, eventId_, sourceId_ ) {}
		
	void addFirstTimeLink( const bool val ) { INT8 val8 = val ? 1 : 0; addParam( IceParamData( eIceEventParam_FirstTwitchTimeLink, true, val8 ) ); }

		// generated in dbm
public:
	IceEvent::composeForIceOlap;
private:
	IceEvent::composeForDbm;
	IceEvent::reComposeForIceOlap;
};

// PYR-125878
class IceEventProgressivePlusEntry : public IceEvent
{
public:

	IceEventProgressivePlusEntry( INT64 eventId_, INT32 sourceId_ ) : IceEvent( DBM_Q_OLAP_ICE_EVENT_PROGRESSIVEPLUS_ENTRY, eIceEvent_ProgressivePlusEntry, eventId_, sourceId_ ) {}

	void addProgPlusPoolBuyin		( const INT64 val64 ) { addParam( IceParamData( eIceEventParam_ProgressivePlusBuyIn		, true, val64 ) ); }

	// generated in dbm
public:
	IceEvent::composeForIceOlap;
private:
	IceEvent::composeForDbm;
	IceEvent::reComposeForIceOlap;
};

class IceEventProgressivePlusFinish : public IceEvent
{
public:

	IceEventProgressivePlusFinish( INT64 eventId_, INT32 sourceId_ ) : IceEvent( DBM_Q_OLAP_ICE_EVENT_PROGRESSIVEPLUS_FINISH, eIceEvent_ProgressivePlusFinish, eventId_, sourceId_ ) {}

	void addProgPlusPoolBuyin		( const INT64 val64 ) { addParam( IceParamData( eIceEventParam_ProgressivePlusBuyIn		, true, val64 ) ); }
	void addProgPlusPoolWinAmount	( const INT64 val64 ) { addParam( IceParamData( eIceEventParam_ProgressivePlusWinAmount	, true, val64 ) ); }

	// generated in dbm
public:
	IceEvent::composeForIceOlap;
private:
	IceEvent::composeForDbm;
	IceEvent::reComposeForIceOlap;
};

// PYR-124673
class IceEventNewTicket: public IceEvent
{
public:
	IceEventNewTicket(): IceEvent( DBM_Q_OLAP_ICE_EVENT_NEW_TICKET, eIceEvent_NewTicket ) {}

	// generated in dbm
public:
	IceEvent::composeForIceOlap;
private:
	IceEvent::composeForDbm;
	IceEvent::reComposeForIceOlap;
};

// PYR-124673
class IceEventDealJackpotSettled: public IceEvent
{
public:
	IceEventDealJackpotSettled(): IceEvent( DBM_Q_OLAP_ICE_EVENT_DEAL_JACKPOT_SETTLED, eIceEvent_DealJackpotSettled ) {}

	void addMinigamePlayId			( INT64	val		)	{ addParam( IceParamData( eIceEventParam_MiniGamePlayId		, true , val	) ); }

	// generated outside of dbm
public:
	IceEvent::composeForDbm;

	// generated in dbm
public:
	IceEvent::composeForIceOlap;
private:
	IceEvent::reComposeForIceOlap;
};
#pragma warning( pop )

/////////////////////////////////////////////////////////////////////////

class IceEventUtils
{
public:
	static void addPlatform( IceEvent& e, INT32 platformId )
	{ 
		if( platformId > Client_Last )
		{
			PLog( LOG_TRACE_FAULTY_LOGIC ": bad platform %d", platformId );
			platformId = 0;
		}
		e.addParam( IceParamData( eIceEventParam_Platform, true , 1<<platformId ) );
	}

	// PYR-40455
	static void addBrand( IceEvent& e, INT32 brandId )
	{ 
		if( brandId > BrandType_Last )
		{
			PLog( LOG_TRACE_FAULTY_LOGIC ": bad brand %d", brandId );
			brandId = 0;
		}

		e.addParam( IceParamData( eIceEventParam_Brand, true , brandId ) );
	}
	// PYR-84447 TODO: remove 'addSite' before the build
	static void addSite( IceEvent& e, INT32 siteId )
	{
		if( siteId > PokerStars_Last )
		{
			PLog( LOG_TRACE_FAULTY_LOGIC ": bad siteId %d", siteId );
			siteId = PokerStars_SiteUnknown;
		}

		e.addParam( IceParamData( eIceEventParam_Site, true, siteId, (INT32) 0 /*mask*/ ) );
	}
	// PYR-84447 and PYR-100632
	static void addSingleSiteIdMask( IceEvent& e, const PSiteMask& singleSiteIdMask )
	{ 
		if( SiteBitmask_Last < singleSiteIdMask  )
		{
			PString sSSI,sBML;
			PLog( LOG_TRACE_FAULTY_LOGIC ": bad siteId '%s' < '%s'", SiteBitmask_Last.toDbString( sBML ), singleSiteIdMask.toDbString( sSSI ) );
			e.addParam( IceParamData( eIceEventParam_Site, true, SiteBitmask_SiteUnknown, PBitmask::zero /*mask*/ ) ); 
			return;
		}
		// still have to add a 32 bit mask because of existing legacy scripts that had already been uploaded and have not expired
		e.addParam( IceParamData( eIceEventParam_Site, true, singleSiteIdMask.getLower32(), 0 /*mask*/ ) ); 

		// adding a new site id mask for new scripts - they will use the same parameter name but will create different parameter id
		e.addParam( IceParamData( eIceEventParam_SiteLong, true, singleSiteIdMask, PBitmask::zero /*mask*/ ) ); 
	}
};
//////////////////////////////////////////////////////////////////////////



#endif // iceevnts_h_included
