// ===========================================================
// Copyright (C) 2017 PYR Software Ltd. All rights reserved.
// ===========================================================

#ifndef MINIGAMESCOMMONPROT_H
#define MINIGAMESCOMMONPROT_H

// NOTE: this file used to be ATF generated, but complicated inclusion in various servers due to thin/non-thin inclusions.
// For PYR-45550 - we opted to remove the ATF generation of this file in order to simplify these inclusion cases

//PYR-65428
#define PRIZE_PROP_MPC_TYPE				"MPCType"
//PYR-73972
#define PRIZE_PROP_TRACKCODE_INCLUDE	"TrackingCodeInclude"
#define PRIZE_PROP_TRACKCODE_EXCLUDE	"TrackingCodeExclude"
//PYR-81808
#define PRIZE_PROP_C2L_URL				"Click2LaunchURL"
#define PRIZE_PROP_C2L_BUTTON_TEXT		"Click2LaunchButtonText"
//PYR-70655
#define PRIZE_PROP_BRAND_MASK			"BrandMask"

enum eMinigameStatus
{
	MinigameStatus_Active   = 1,
	MinigameStatus_Paused   = 2,
	MinigameStatus_Expired  = 3,
	MinigameStatus_Pending  = 4,
};

namespace MiniGameCommon	// this is an old namespace kept to reduce build problems when switching this file away from being ATF-generated
{
	//------------------------------------------------------------------------------
	
	enum GameType
	{
		// minigames
		GameType_TheDeal            = 1,
		GameType_TheDealHighRoller  = 2,
		GameType_TheDealJackpot     = 3,

		// sidegames
		GameType_PokerSideGame_First = 101,
		GameType_CardMatch           = GameType_PokerSideGame_First,
		GameType_SuitRace            = 102,
		GameType_PokerSideBet		 = 103,
		GameType_PokerSideGame_Last  = 200,
		
		// bonus games
		GameType_BonusGame_First    = 201,
		GameType_RandomDrawGame     = GameType_BonusGame_First,
		GameType_PickAnObject		= 202,
		GameType_BonusWheel			= 203,
		GameType_FixedPrize			= 204, // PYR-107267
		GameType_BonusGame_Last     = 300,
		
		// chest games
		GameType_ChestGame_First       = 301,
		GameType_StarsRewardsChestGame = GameType_ChestGame_First,
		GameType_MultiPurposeChestGame = 302,
		GameType_ChestGame_Last        = 400,

		//Keep this in sync with MiniGameDef.h AbstractGameTypes when introducing new type ranges
	};

	//------------------------------------------------------------------------------

	enum GameStatus
	{
		GameStatus_Null      = 0, 	// this minigameDef is permanently unavailable for new plays (either it has been superseded by a new instance of the same minigameType, or the minigameType is no longer being offered)
		GameStatus_Active    = 1, 	// this minigameDef is currently available for play
		GameStatus_Paused    = 2, 	// this minigameDef is temporarily unavailable for new plays
	};

	//------------------------------------------------------------------------------

	enum PrizeType
	{
		PrizeType_Null               = 0,
		PrizeType_RealMoney          = 1,  	// prize amount in cents
		PrizeType_PlayMoney          = 2,  	// prize amount in Play Money cents
		PrizeType_FPP                = 3,  	// prize amount in FPP cents
		PrizeType_Ticket             = 4,  	// prize is a ticket
		PrizeType_MiniGame           = 5,  	// prize is a play of another mini game
		PrizeType_Jackpot            = 6,  	// prize triggers the jackpot payment mechanism
		PrizeType_CasinoRoll         = 7,  	// prize is a casino ROLL (non-free spins bonus), business calls this CasinoInstantBonus
		PrizeType_RewardPoints       = 8,  	// prize is reward points (not payable by MGLobby)
		PrizeType_TChips             = 9,  	// prize amount in tournament chips cents
		PrizeType_GameTicket         = 10, 	// prize is a ticket for a minigame
		PrizeType_FreeSpins          = 11,	// prize is a casino free spins bonus (internally this is a FSB-ROLL)
		PrizeType_FreeSportsBet      = 12,	// prize is a free sports book bet
		PrizeType_IcePuzzle		     = 13,	// prize is an Ice puzzle (support for this PrizeType is added in main.07 by Pyr-52967.)
		PrizeType_MPC                = 14,	// prize is an Multi-Purpose Chest, payable by LobbyETC #PYR-65248
		PrizeType_TournTicketBundle  = 15,	// prize is a Tournament Ticket Bundle, which map to n tickets all of the same ticket id. #PYR-65248
		PrizeType_GiftCode           = 16,	// prize is a gift code #PYR-111558

		// if a new prize type is added be sure to update defaultPrizeTypeSupportedBrandsMap

		PrizeType_MultiPrize     = 16384, 	// 0x4000 denotes that this is a multi-prize. Not payable, and shouldn't be client facing.
	};

	//------------------------------------------------------------------------------

	//Any update to this enum will require a change in mglobbydbmmanager.cpp in the function TableDbManager::postToIceMinigameCompleted
	enum PokerSideGameStates
	{
		PokerSideGameState_Winner            = 0, 	// game instance is finished, prizes have been claimed
		PokerSideGameState_Pending           = 1, 	// game will start next eligible hand
		PokerSideGameState_InProgress        = 2, 	// game started
		PokerSideGameState_PrizeUnclaimed    = 3, 	// game finished and awaiting for player to claim prize
		PokerSideGameState_Forfeited         = 4, 	// game card was forfeited
		PokerSideGameState_Abandoned         = 5, 	// game card was abandoned by player leaving the table
		PokerSideGameState_Expired           = 6, 	// game card expired due to too many hands
		PokerSideGameState_DailyMaxReached   = 7, 	// game card forced to end due to daily max reached on another card
		PokerSideGameState_TableUnrecovered  = 8, 	// game card lost due to a non-recoverable table
		PokerSideGameState_TableInvalid      = 9, 	// game card lost due to table not matching help_support_portal parameters
		PokerSideGameState_PendingWinPrizeNotClaimable = 10, 	// game card win conditions met but prize not yet claimable due to player still active in hand
		PokerSideGameState_PendingWinPrizeClaimable    = 11, 	// game card win conditions met and prize claimable due to player's participation ending, but hand still active so prize payment will be deferred
		PokerSideGameState_PendingWinPrizeClaimed      = 12, 	// game card win conditions met and prize claimed by player, but hand still active so prize payment will be deferred
		PokerSideGameState_PendingExpired              = 13, 	// game card expired from player's perspective due to player's participation in hand ending, but hand still active so state transition will be deferred
		PokerSideGameState_PendingLoss                 = 14, // game will end in a loss for the player after the active hand
		PokerSideGameState_Loss                        = 15, // game ended in a loss for the player
		PokerSideGameState_Push							= 16, // game ended in a state that was neither a win nor a loss (i.e. flop cards side bet on hand where no flop is dealt)
		PokerSideGameState_PlayerCanceled				= 17, // game ended by player request while in pending state
		PokerSideGameState_PlayerLeftBeforeStart		= 18, // game ended by player leaving the table while in pending state
		PokerSideGameState_SystemCanceledAfterRestart	= 19, // game ended by MGLobby because it couldn't fully recover state after a restart -> player gets a refund
		PokerSideGameState_PlayerNotDealtIn				= 20, // game ended by player not being dealt in for next hand
		PokerSideGameState_EventTimeout					= 21, // game ended by timeout waiting for next dealt cards event exceeded
		PokerSideGameState_AutoPlayCanceled				= 22, // game ended by autoPlay context
	};

	//------------------------------------------------------------------------------

	enum EntryType
	{
		EntryType_NotApplicable			= -1, 	// no associated poker entry
		EntryType_Table					= 0, 	// Table sidegame entries
		EntryType_Blitz					= 1, 	// Blitz/Zoom sidegame entries
		EntryType_Tournament			= 2, 	// Tournament sidegame entries
		EntryType_ObserverTable			= 3, 	// Table observer sidegame entries		(PYR-118059)
		EntryType_ObserverTournament	= 4, 	// Tournament observer sidegame entries (PYR-118059)

		EntryType_Max					= EntryType_ObserverTournament // needs to be kept up-to-date
	};

	enum BarType
	{
		BarType_StarsRewards = 0,
		BarType_MultiPurposeChest = 1,
	};

	// PYR-79304
	enum StarsRewardsBarVersion
	{
		StarsRewardsBarVersion_Undefined = 0,
		StarsRewardsBarVersion_Initial	= 1,
		StarsRewardsBarVersion_CVL2		= 2,
	};
};

namespace MiniGameClient	// this is a namespace kept to reduce build problems when switching some enums into a common file
{
	enum BuyInType
	{
		BuyInType_NotAvailable       = 1000, // direct buy-in not available
		BuyInType_RealMoney          = 1001, // buy-in amount in cents
		BuyInType_PlayMoney          = 1002, // buy-in amount in Play Money cents
		BuyInType_FPP                = 1003, // buy-in amount in FPP cents
		BuyInType_FPPGameTicketOnly  = 1004, // game ticket buy-in only (Only FPP game tickets are currently supported)
		BuyInType_BarId              = 1005, // barId
		BuyInType_FreeRealMoney      = 1006, // Free to play with real money prizes
		BuyInType_FreePlayMoney      = 1007, // Free to play with play chips prizes only
		BuyInType_FreeFPP            = 1008, // Free to play fpp game
		BuyInType_RealMoneyVariable	 = 1009, // variable buy-in amount in cents within a min and max range
		BuyInType_PlayMoneyVariable	 = 1010, // variable buy-in amount in Play Money cents within a min and max range
	};

	namespace sidegame
	{
		// PYR-106565 moved from minigamesclientprot.h
		enum ePokerSideBetResult_HoleCards
		{
			ePokerSideBetResult_HoleCards_Loss = 0,
			ePokerSideBetResult_HoleCards_PairNotAces = 1,
			ePokerSideBetResult_HoleCards_AceFaceUnsuited = 2,
			ePokerSideBetResult_HoleCards_AceFaceSuited = 3,
			ePokerSideBetResult_HoleCards_PairAces = 4,
		};

		enum ePokerSideBetResult_FlopCards
		{
			ePokerSideBetResult_FlopCards_Loss = 0,
			ePokerSideBetResult_FlopCards_Pair = 1,
			ePokerSideBetResult_FlopCards_Flush = 2,
			ePokerSideBetResult_FlopCards_Straight = 3,
			ePokerSideBetResult_FlopCards_Trips = 4,
			ePokerSideBetResult_FlopCards_StraightFlush = 5,
			ePokerSideBetResult_FlopCards_MiniRoyal = 6,
		};
	}
};

#endif
