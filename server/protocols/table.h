// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Parts of this file are being maintained using automated scripts.
// The script maintenance parts are surrounded by language-specific
// comment lines that begin with <<== and end with ==>> markers.
// When required, manual changes can made to this file as usual. 
// Such changes should be communicated to the current maintainer 
// of GIT module in ADMIN/asppages/admin/_licenses. 
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#ifndef table_h_included
#define table_h_included

#include "lobby.h"
#include "cards.h"

#define TABLE_PROTOCOL_VERSION "v0.1.32"

enum PlayerSitStatus { SittingIn = 0x0, SittingOut, SitInRequest, SitOutRequest };
enum RoundOrdinal { NonDetermined = 0x80, TournNotStartedYet = -7, BreakWait = -6, BreakOn = -5, NoGame = -4, NoHand = -3, Showdown = -2, Blinds = -1,
					FirstRound, SecondRound, ThirdRound, FourthRound, FifthRound, SixthRound, SeventhRound, EndGamePlayerRound /* place holder, the real game round should never be equal this number */,
					RebuyRound = 101, ReloadRound = 102, UnfoldRound = 103, UnfoldShowDownRound = 104, CashoutRound = 105,
					/* tbRebuyRound and tbReloadStackRound are deprecated: for use in legacy table code only! */ tbRebuyRound = 777, tbReloadStackRound = 888 };

enum Tournament { NoTableTour = NON_TOURNAMENT , DenominatedTableTour = DENOMINATED_MT_TOURNAMENT, MultiTableTour = MT_TOURNAMENT };

enum PotCategory  // PYR-65734
{
	PCID_DefaultGamePot = 0,
	PCID_UnfoldPot = 1,

	PCID_Category_Last  // MUST be on the last place
};

enum ChatCategory
{
	ChatError = -1,
	ChatAdmin = 0,
	ChatDealerAlways,
	ChatDealerMin,
	ChatDealerMax,
	ChatUser,
	ChatObserver,
	ChatManager,
	ChatSupport,
	ChatSupervisor,
	HiddenChatObserver,
	ChatCommentator,
	ChatOptional,
	ChatBroadcastAll,
	ChatBroadcastCom,
	ChatBroadcastNet,
	ChatModerator,
	ChatTeamPro,
	ChatAutoBroadcast,
	ChatBroadcastIta,
	ChatBroadcastFra,
	ChatBroadcastEst,
	ChatTeamOnline,
	ChatBroadcastBel,
	ChatBroadcastSpa,
	ChatBroadcastDen,
	ChatBroadcastEU,
	ChatBroadcastUS_NJ,
	ChatAutoDeal,
	ChatBroadcastUK,
	ChatBroadcastBul,
	ChatBroadcastDE_SH,
	ChatBroadcastPor,
	ChatBroadcastRom,
	ChatBroadcastCz,
	ChatBroadcastRuSo,
	ChatBroadcastRuSoPM,
	ChatBroadcastInd,
	ChatBroadcastUS_PA,
	ChatBroadcastAsia,
	ChatBroadcastSwe,
	ChatBroadcastCH,
//	<<== @site2-hhchat-category: Site enumerations for hand history table chat @ ====
	ChatBroadcastARBA,	// PYR-114693 PYR-114733
	ChatBroadcastUSCO,	// PYR-124292
	ChatBroadcastDE,	// PYR-124555
	ChatBroadcastGR,	// PYR-120936
//	==== @site2-hhchat-category: Site enumerations for hand history table chat @ ==>>
	// +++ when updating ChatCategory check also TABLE_HISTORY_CHAT_RESERVED
	ChatReserved,
};

enum ErrorCodeSrvClnt { NoError = 0, NoErrorButMes, SmallError, FatalClientError, FatalServerError, ErrorNoCurrency, ErrorNoCurrencyNotAllowed };
enum ErrorCodeSrvMtlb { OkCode = 0, WrongTableID, UsersEqualZero, OccupiedSeat, FreeSeat, UsersNotEqualZero, HandIsActive, OutstandingKickUsers, UserNotFound, NotEnoughUsers };

enum ClientServiceFlag
{
	MisMinLimit			= 0x01,
	MisMaxLimit			= 0x02,
	PwdWanted			= 0x04,
	MisRatholeMax		= 0x08,
	MisGlobalMinLimit	= 0x10,
};

enum ClientLoginReplyFlag
{
	ReplayCommentator = 0x1,
	IsNewFlag = 0x2,
	MissBlindFlag = 0x4,
	FoldnStayFlag = 0x8
};

enum ClientHandHistoryFlag
{
	NoPrintInChat = 0x1,
	RoundMask = 0xffff0000,
	RoundBitOffset = 0x10,
	DrawCardMaskOffset = 0x08
};

enum ClientSeatIndicator
{ 
	ClientEmptySeat			= 0x00, 
	ClientOccupiedSeat		= 0x08, 
	ClientReservedSeat		= 0x18
};

enum ClientChatError
{
	NoChatError = 0,
	EmailNotVaidated			= 1,	// CLIENTMSG_EMAIL_NOT_VALIDATED
	NoPrivChat					= 2,	// CLIENTMSG_NO_PRIV_CHAT
	NoPlayerChat				= 3,	// CLIENTMSG_TOURNMT_NOPLAYERCHAT
	TournBubblePlayerChat		= 4,	// CLIENTMSG_TOURNMT_BUBBLE_PLAYER_CHAT
	BlockChatIfAllIn			= 5,	// CLIENTMSG_TOURNMT_BLOCKCHATALLIN
	BlockChatIfInHand			= 6,	// CLIENTMSG_TOURNMT_BLOCKCHATINHAND
	NoObserverChatTourn			= 7,	// CLIENTMSG_TOURNMT_NOOBSERVERCHAT
	RestrictObserverChat		= 8,	// CLIENTMSG_RESTRICTEDOBSERVERCHAT_TABLE
	RestrictObserverChatTourn	= 9,	// CLIENTMSG_TOURNMT_RESTRICTEDOBSERVERCHAT
	TournBubbleObserverChat		= 10,	// CLIENTMSG_TOURNMT_BUBBLE_OBSERV_CHAT
	NoObserverChat				= 11,	// CLIENTMSG_NOOBSERVERCHAT_TABLE
	SuspendChat					= 12,	// PYR-20489
	//NoChatGuestAccount			= 13,	// PYR-25263 - obsolete by PYR-91342
    NoChatDuringAutoDeal        = 14,   // PYR-25565
	ChatAutoBanned              = 15,   // PYR-28673
};

enum ClientMsgStrMap
{
	AdminBroadcastMessage = 0,  // no i18n processor for one
	DealerChat_TournStarted = 1,
	ClientMsg_Tournmt_NewRound,
	ClientMsg_Tournmt_NewStakes,
	DealerChat_LastTable,
	DealerChat_LastTwoPlayers,
	DealerChat_HandByHand_On,
	DealerChat_HandByHand_Off,
	DealerChat_HandByHand_Table,
	// MSG_TABLE_SIT2_REPLY::
	ClientMsg_Incorrect_Password,		// CLIENTMSG_INCORRECT_PASSWORD
	ClientMsg_Too_Much_Chips,			// CLIENTMSG_TOO_MUCH_CHIPS
	ClientMsg_Not_Allocated_Chips,		// CLIENTMSG_NOT_ALLOCATED_CHIPS
	Lmtl_Internal_Error,				// LMTL_INTERNAL_ERROR   +BYTE, +UINT16 ("2000","2001",..)
	ClientMsg_Leave_Table_Requested,	// CLIENTMSG_LEAVE_TABLE_REQUESTED
	ClientMsg_Noplay_Colluder,			// CLIENTMSG_NOPLAY_COLLUDER  +Pstr
	ClientMsg_Too_Many_Tables_PM,		// CLIENTMSG_TOO_MANY_TABLES  +INT32
	ClientMsg_Too_Many_Tables_RM,		// CLIENTMSG_TOO_MANY_TABLES  +INT32
	// MSG_TABLE_SIT1_REPLY::
	ClientMsg_Email_Not_Validated,		// CLIENTMSG_EMAIL_NOT_VALIDATED   +str
	ClientMsg_No_Priv_Play,				// CLIENTMSG_NO_PRIV_PLAY
	ClientMsg_HighRoller_Table,			// CLIENTMSG_HIGHROLLER_TABLE      +str //THIS_SITE
	ClientMsg_Occupied,					// CLIENTMSG_OCCUPIED
	ClientMsg_Reserved,					// CLIENTMSG_RESERVED
	ClientMsg_Thinking_Too_Much,		// CLIENTMSG_THINKING_TOO_MUCH
	ClientMsg_Not_Enough_Funds_PM,		// CLIENTMSG_NOT_ENOUGH_FUNDS_PM   +UINT32(chips in cents)
	ClientMsg_Not_Enough_Funds,			// CLIENTMSG_NOT_ENOUGH_FUNDS      +UINT32(chips in cents) 
	DealerChat_RatHoling,				// DEALERCHAT_RATHOLING       +UINT32, +UINT32(chips in cents)
	//MSG_TABLE_ADDCHIPS_REPLY:: 
	// & MSG_TABLE_ADDCHIPS_AUTO_REPLY::
	ClientMsg_No_Add_Chips,				// CLIENTMSG_NO_ADD_CHIPS
	ClientMsg_Too_Small_Chips,			// CLIENTMSG_TOO_SMALL_CHIPS
	ClientMsg_Stack_Updated,			// CLIENTMSG_STACK_UPDATED
	ClientMsg_Too_Much_Add,				// CLIENTMSG_TOO_MUCH_ADD
	//MSG_TABLE_REBUYCHIPS_REPLY::
	ClientMsg_Rebuy_Requested,			// CLIENTMSG_REBUY_REQUESTED
	ClientMsg_NoMoreRebuys,				// CLIENTMSG_NOMOREREBUYS
	ClientMsg_RebuyClosed,				// CLIENTMSG_REBUYCLOSED
	ClientMsg_NoRebuy,					// CLIENTMSG_NOREBUY
	//MSG_TABLE_ADDONCHIPS_REPLY::
	ClientMsg_NoMoreAddons,				// CLIENTMSG_NOMOREADDONS
	ClientMsg_AddonClosed,				// CLIENTMSG_ADDONCLOSED
	ClientMsg_NoAddon,					// CLIENTMSG_NOADDON
	//MSG_TABLE_ADMIN_INFO::
	ClientMsg_Priv_Play,				// CLIENTMSG_PRIV_PLAY
	ClientMsg_Kicking_Warning,			// CLIENTMSG_KICKING_WARNING
	//MSG_TABLE_SIT_KICK::
	ClientMsg_Action_Kicked_MultiGame,	// CLIENTMSG_ACTION_KICKED_MULTIGAME
	ClientMsg_Action_Kicked_Ante,		// CLIENTMSG_ACTION_KICKED  +_int
	ClientMsg_Action_Kicked_BBlinds,	// CLIENTMSG_ACTION_KICKED  +_int
	ClientMsg_Action_Kicked_ByAdmin,	// CLIENTMSG_ACTION_KICKED_BYADMIN
	ClientMsg_Tournmt_Kicked,			// CLIENTMSG_TOURNMT_KICKED  +UINT32
	ClientMsg_Tournmt_Kicked_Popup,		// CLIENTMSG_TOURNMT_KICKED  +UINT32, (+ ". "), +Pstr 
	ClientMsg_Tournmt_Winner,			// CLIENTMSG_TOURNMT_WINNER
	//MSG_TABLE_FORCE_SITOUT::
	ClientMsg_Not_Enough_Chips,			// CLIENTMSG_NOT_ENOUGH_CHIPS
	ClientMsg_Too_Much_For_Hand,		// CLIENTMSG_TOO_MUCH_FOR_HAND
	ClientMsg_No_Priv_AllIn,			// CLIENTMSG_NO_PRIV_ALLIN
	ClientMsg_Last_AllIn,				// CLIENTMSG_LAST_ALLIN
	ClientMsg_NoAllIns,					// CLIENTMSG_NOALLINS
	ClientMsg_All_In,					// CLIENTMSG_ALL_IN
	ClientMsg_AllIns_Exeeded,			// CLIENTMSG_ALLINS_EXEEDED
	//:Publication:: advertisementString( ClientMsgStrMap& msgId )
	ClientMsg_Tournmt_Finished,			// CLIENTMSG_TOURNMT_FINISHED
	ClientMsg_Tournmt_Shootout,			// CLIENTMSG_TOURNMT_SHOOTOUT
	ClientMsg_Tournmt_Olympic,			// CLIENTMSG_TOURNMT_OLYMPIC
	//MSG_TABLE_SIT_KICK::
	ClientMsg_Tournmt_Kicked_Replace,
	ClientMsg_Account_Not_Found,
	//--------------------------------------------------------------
	ClientMsg_User_Prefs_Changed,
	ClientMsg_Conversion_Rate_Error,
	ClientMsg_Milestone_Hand,
	ClientMsg_Aams_Cannot_Process,
	ClientMsg_Aams_Not_Available,
	ClientMsg_No_Account_Rebuy,
	ClientMsg_No_Account_Addon,
	ClientMsg_Too_Many_Tables_HU,
	ClientMsg_Not_Allowed_HuTables,
	ClientMsg_Must_Have_DepositLimit,
	ClientMsg_Blocked_RM_Tables,
	ClientMsg_Blocked_RM_Tourns,
	ClientMsg_Blocked_PM_Tables,
	ClientMsg_Blocked_PM_Tourns,
	ClientMsg_Country_Restriction,
	ClientMsg_Buyin_Limit_Not_Set,
	ClientMsg_Buyin_Limit_Reached,
	ClientMsg_Must_Have_Deposit_n_Buyin_Limits,
	// PYR-15369: MUST_HAVE_WITHDRAWAL_LIMIT error
	ClientMsg_Must_Have_Withdrawal_Limit,
	ClientMsg_Single_Player_Country_Restriction,
	// PYR-18531: AAMS over the game limit error
	ClientMsg_Aams_Exceeding_Game_Limit,	// <chips rebuy>, <chips limit>
	ClientMsg_Not_Allowed_Without_Player_Token, // PYR-18904
	ClientMsg_Not_Allowed_Without_RealMoneyOk, // PYR-19149 
	ClientMsg_Tournmt_Flight_Winner, // PYR-19167
	ClientMsg_Automatic_Rebuy_Turned_Off, // PYR-19819
	ClientMsg_Not_Allowed_PL_OMAHA_Tables,		// PYR-20119
	ClientMsg_Suspend_Priv_Play,	// PYR-20489
	ClientMsg_Tournament_Is_Now_In_Regular, //PYR-22885
	ClientMsg_Ratholing_Bypass,
	ClientMsg_Login_Time_Reached,  // PYR-27741
	ClientMsg_Staff_Restricted,		// PYR-26597
	__obsolete_ClientMsg_Guest_FeatureNotAvailable,	//	PYR-28490 - obsolete by PYR-91342
	ClientMsg_Player_Kick_Camping_Timeout,  // PYR-25229
	ClientMsg_Player_Kick_No_Blind_Posted,  // PYR-25229
	ClientMsg_Reload_Stacks_Only_When_Empty,   // PYR-25226
	ClientMsg_Reload_Stacks_No_Enough_Stacks,  // PYR-25226
	ClientMsg_Reload_Stacks_Use_Default,  // PYR-25226

	ClientMsg_Dealer_Chat_Special,  // this value must be less than 256 (in BYTE field)

	ClientMsg_Tourn_MileStone_Prior_Draw, // PYR-28689
	ClientMsg_Tourn_MileStone_Winning_Table, // PYR-28689
	ClientMsg_Tourn_MileStone_Losing_Table, // PYR-28689

	ClientMsg_Player_Kick_GeoIP_Unknown,  // PYR-27243

	ClientMsg_ACM_BannedChatTerm,  // PYR-28673
	ClientMsg_ACM_SimilarContentRestrict,  // PYR-28673
	ClientMsg_ACM_SingleTableFloodRestrict,  // PYR-28673
	ClientMsg_ACM_MultiTableFloodRestrict,  // PYR-28673
	ClientMsg_ACM_DomainSpamRestrict,  // PYR-28673
	ClientMsg_ACM_UserChatBanOnTable,  // PYR-28673

	ClientMsg_SIT_TooManyAttempts,  // PYR-32995
	ClientMsg_Tournmt_Sag_Winner_Custom_Prize, // PYR-33574
	ClientMsg_PoolBlock_ClientHasTimePenalty, // PYR-42791
	ClientMsg_Tournmt_NewBLSubparam, // PYR-32616

	DealerChat_WinTheButton_Off,  // PYR-32587

	ClientMsg_DailyLossLimit_Reached,  // PYR-47770
	ClientMsg_MonthLossLimit_Reached,  // PYR-47770
	ClientMsg_DailySpendLimit_Reached,  // PYR-47770
	ClientMsg_MonthSpendLimit_Reached,  // PYR-47770

	ClientMsg_Seat_Not_Allowed,         // PYR-49747
	ClientMsg_Seat_User_Self_Excluded,  // PYR-49747
	ClientMsg_Seat_Table_Limit_Exceeded, // PYR-49747

	ClientMsg_Action_Kicked_ByAdmin_Silent,	// PYR-50064

	ClientMsg_RGLimits_Not_Set,			//	PYR-51502
	ClientMsg_FR_Unath_No_Priv_Play,	// PYR-55375
	ClientMsg_NSF,						// PYR-55137
	ClientMsg_SuspendRM_Banned_By_Regulator,	// PYR-50999
	ClientMsg_Suspend_Priv_Play_Chat, // PYR-65392

	ClientMsg_Not_Allowed_Unfold_Tables, //PYR-70870
	ClientMsg_Action_Kicked_ForMerge, // PYR-84882
	ClientMsg_Action_Kicked_WebToken_Expired,  // PYR-97543
	ClientMsg_Action_Tournament_Spend_Limit_Exceeded, // PYR-90566
	// to be expanded here...
};

enum ClientMsgStrMap_DealerChatSpecial
{
	// for TABLE_HISTORY_CHATDEALERALWAYS
	// all following value should be published in history.amount field (not in history.position field)

	ClientMsg_DealerChat_Tourn_MileStone_Winners = 1, // PYR-28689
	ClientMsg_Player_Win_The_Button_From_Prev_Hand,  // PYR-32587
	ClientMsg_DealerChat_AllInCashout,

	ClientMsg_DealerChat_Msg_Reserved_53 = ClientMsg_Tournmt_Finished,  // PYR-82885 ClientMsg_Tournmt_Finished (53) is used in TABLE_HISTORY_CHATDEALERALWAYS
};

// PYR-25565
enum AutoDealChatCodes
{
	AutoDealChat_First = 0,
	AutoDealChat_Tourn_Player_Cancelled_Deal = 0, //  "s" - userId
	AutoDealChat_Tourn_Deal_Proposed = 1,           //  "4<s4>4" - payArray<User,payment>, leftForFirst
	AutoDealChat_Tourn_Player_Agreed = 2,           //  "s" - userId
	AutoDealChat_Support_Called = 3,                //  none
	AutoDealChat_Manager_Control_Passed = 4,        //  "ss" - oldManager, newManager
	AutoDealChat_Player_Inv_Disconnected = 5,       //  "s" - userId
	AutoDealChat_Deal_Cancelled_Because_Player_Disconnected  = 6,  // none
	AutoDealChat_Player_Inv_Reconnected = 7,        //  "s" - userId
	AutoDealChat_Deal_Manager_On_Negotiate = 8,     //  "sb" - userId, isFirstTime
	// expand here...
	AutoDealChat_Last
};

enum eAllInCashoutChatCodes // used for ClientMsg_DealerChat_AllInCashout
{
	eAllInCashoutChatCodes_UsersConsidering = 0,
	eAllInCashoutChatCodes_UserCachedOut = 1, // username in 'user' field
	eAllInCashoutChatCodes_ResumingHand = 2, // 0 or more users in msgClientExtra
};

enum PlayerKickedReasonCode   // BYTE
{
	TABLE_KICK_REASON_SERVER_REMOVE_PLAYER_SILENTLY = 0xFF,  // only internally used for server implementation convenience.
	TABLE_KICK_REASON_MISS_TOO_MANY_ORBITS = 0,
	TABLE_KICK_REASON_DISCONNECT_TOO_LONG = 1,
	TABLE_KICK_REASON_BY_ADMIN_GENERAL = 2,
	TABLE_KICK_REASON_BY_ADMIN_WITH_SPECIAL_MSG = 3,
	TABLE_KICK_REASON_CAMPING_TIMEOUT = 4,
	TABLE_KICK_REASON_NO_BLIND_POST = 5,
	TABLE_KICK_REASON_GEOIP_UNKNOWN = 6,
	TABLE_KICK_REASON_PGA_DOWN = 7,
	TABLE_KICK_REASON_WEBTOKEN_EXPIRED = 8,
	TABLE_KICK_REASON_NO_FR_TEMP_SL = 9,
	TABLE_KICK_REASON_MERGE = 10,
};

enum CardsShownType
{
	Closed			= 0x00,
	FoldedButShown	= 0x01,
	MuckedButShown  = 0x02,	// #15261
	ClosedButShown  = 0x04,	//PYR-21939
	FoldedAndShownPermanently = 0x08, //PYR-64093
	Shown			= 0xFF
};

enum TimeBankState { None = 0, TBRequested, TBStarted };

enum TableActionTimeType { ATNone = 0, ATNormalActionTime, ATTimeBank, ATExtraTime };

enum AutoRebuyOptions
{
	_autoRebuyDisabled = -1, //turned off at the table
	_autoRebuyOff = 0,		// no autoRebuy
	_autoRebuyMin,			// rebuy to table min
	_autoRebuyIni,			// rebuy to player's initial buy-in
	_autoRebuyMax,			// rebuy to table max
	_autoRebuyBigBets		// rebuy to X big bets(blinds)
};

enum TableHistoryExtraActionCodes
{
	TH_EX_Act_Hand_Cancelled_Bets_Returned = 0,
	TH_EX_Act_Betting_is_Capped = 1,
	TH_EX_Act_Player_Allowed_Play_After_Button = 2,
	TH_EX_Act_Pair_On_Board_Double_Bet_Allowed = 3,
	TH_EX_Act_No_Low_Hand = 4,
	TH_EX_Act_Player_Opens_With_Card = 5,
	TH_EX_Act_Player_Opens_Betting_Round = 6,
	TH_EX_Act_Player_Lo_Hand_Ranking = 7,
	TH_EX_Act_Time_New_Game_Start_Or_Time_To_Act = 8,
	TH_EX_Act_Tourn_Clock_Stop_Or_Player_Discon_Time_Added = 9,
	TH_EX_Act_Many_Players_Disconnect_Or_Reconnect = 10,
	TH_EX_Act_Player_Finish_Ordinal_And_Prizes_Won_In_Tourn = 11,
	TH_EX_Act_Tourn_Hand_For_Hand_Or_Normal_Play = 12,
	TH_EX_Act_Add_On_Or_Rebuy_Status = 13,
	TH_EX_Act_Break_Will_Start_When_Current_Hand_Finishes = 14,
	TH_EX_Act_Break_Skipped_And_Game_Resuming = 15,
	TH_EX_Act_Player_User_Name_Alias = 16,   // stealth table, for db only (never goes to client publication)

	//........ Who cut this?
	TH_EX_Act_Tourn_Deal_Attempt_Completed = 105, // PYR-61307
	TH_EX_Act_Set_Variable_Ante = 106,	//PYR-83034
	TH_EX_Act_Unfold_Ante_To_Be_Returned = 107,  // PYR-65734
	TH_EX_Act_WonCustomAward = 108, //PYR-33574
	TH_EX_Act_Pot_CarryOver = 109,
	TH_EX_Act_Bounty_In_Golden_SNG_Reward = 110,
	TH_EX_Act_PrizePoolReward = 111,
	TH_EX_Act_Deck_Is_Reshuffled = 112, // 0x70
	TH_EX_Act_Draw_Ordinal = 113, // 0x71
	TH_EX_Act_Uncalled_Bet_Returned_To_Player = 114, // 0x72
	TH_EX_Act_All_In_Show = 115, // 0x73
	TH_EX_Act_Player_Bounty_Before_Hand = 116, // 0x74
	TH_EX_Act_Win_Bounty_For_Eliminating = 117,  // 0x75
	TH_EX_Act_FinishedTourn = 118, // 0x76
	TH_EX_Act_WonTourn = 119, // 0x77
	TH_EX_Act_WonAward = 120, // 0x78
	TH_EX_Set_Cap_Value = 121, // 0x79
	TH_EX_Act_HideCards_After_FoldAndShow = 122,	// 0x7A
	TH_EX_Act_Start_Run_It_Twice = 123,   // 0x7B 
	TH_EX_Act_Run_It_Twice_Next_Board = 124,  //0x7C
	TH_EX_Act_Run_It_Twice_Evaluate_First_Board = 125,  //0x7D
	TH_EX_Act_Run_It_Twice_Evaluate_Second_Board = 126,  //0x7E
	TH_EX_Act_Win_Bounty_In_Progressive_Knockout = 127  //0x7F
	//Stop increasing! It's a char. Use diapason from 0x70 and below
};


// The following is a set of MUTUALLY EXCLUSIVE states represented with a pair of a value and a mask.
// This is designed in a way to allow further server extension with old clients backward compatibility.
// Never check the state value directly, never check separate bit flags with your own logic,
// instead solely use the following pattern to check that the 'state' is 'XXX':
// if ( (state & XXX_Mask) == XXX_Value ) { }
// or not 'XXX':
// if ( (state & XXX_Mask) != XXX_Value ) { }

enum UserSeatComposedState
{
	NoPlayer_Value                = 0x0, //   00
	NoPlayer_Mask                 = 0x3,

	SitOut_Value                  = 0x1, //   01
	SitOut_Mask                   = 0x3,

	NotInHand_Value               = 0x2, //   10
	NotInHand_Mask                = 0x3,

	InHand_Value                  = 0x3, //  011 - obsolete
	InHand_Mask                   = 0x7,

	InHandNew_Value               = 0x3, // 0011
	InHandNew_Mask                = 0xF,

	InHand_Folded_WithCards_Value = 0xB, // 1011     // for the state when the player folded but his cards are still shown
	InHand_Folded_WithCards_Mask  = 0xF,

	InHand_Folded_Value           = 0x7, //  111
	InHand_Folded_Mask            = 0x7,
};

enum FeatureType  // PYR-79935
{
	FeatureType_AllInCashOut = 0,

	// add new types above here
	FeatureType_Count
};

enum PlayerState2Masks   // UINT64, published in table {5,x,0}
{
	TablePlayerState2_PlayerCashedOut = ( ONE64 << 0 ),
};


#define MAX_TABLE_COLUMNS     0xFF

#define MAX_FULL_CIRCLES_ALLOWED	3  // for Hold'em & Omaha
#define MAX_FULL_HANDS_ALLOWED		20 // for Stud only
#define MAX_MIXED_MULTIGAME_ALLOWED	24 // for HO(R)SE...
#define MULTIGAME_HANDS				8  //8 for multiGames
#define MULTIGAME_HANDS_SIX			6  //6 for 8-Game/MixedHoldem/MixedOmaha (min value)

#define MAX_CHAT_LEN	( HISTORY_CONTENTS_LEN - 1 )
#define QUALITY_QUANTUM	1

#define TXT_TABLE_HAWK_LOGIN_NOT_ALLOWED "This action is not allowed on this table."	// Used in broadcast and in client

/*****************************************************************

 1 - BYTE 
 2 - UINT16
 3 - nonDefined
 4 - UINT32
 5 - INT8
 6 - INT16
 7 - INT32
 8 - UINT64
 ? - INT64
 m - body
 s - PString
 b - bool
 T - SrvTime
 D - SrvDate

******************************************************************

 ERROR CODES with String messages (for MT_lobby/Table interface ): 
 0 - Ok
 1 - "WRONG TABLE ID",
 2 - "NUMBER OF USERS EQUAL ZERO",
 3 - "SEAT IS OCCUPIED ALREADY", 
 4 - "SEAT IS FREE ALREADY", 
 5 - "NUMBER OF USERS NOT EQUAL ZERO"
 6 - "THE HAND IS ACTVE: NO KICK USER"

*******************************************************************

 ERROR CODES (for Client/Table interface ): 
 0 - Ok
 1 - Ok with added message 
 2 - some minor error with added message of explain
 3 - Fatal Client Error [added message] 
 4 - Fatal Server Error [added message] 
 
*******************************************************************/

// C- means Client, S-means Server

#define MSG_TABLE_LOGIN				0x0001
	//C->S, "s4b11" userName, clientSiteId, options, checkBox, useAllIniStacks

#define MSG_TABLE_LOGIN_REPLY		0x0002
	//"S->C, "1s51s4" err_code, err_str, sitNum, playerSitStatus, IP_address, ClientLoginReplyFlag

#define MSG_TABLE_LOGOUT			0x0003 
	//C->S, ""

#define MSG_TABLE_CHAT				0x0004 
	//C->S, "s" 

#define MSG_TABLE_SIT2				0x0005
	//C->S, "4s5222" chips, password, autoRebuyOptions, bigBets, underBigBets, underPercents

#define MSG_TABLE_SIT2_REPLY		0x0006
	//S->C, "1s14m47" err_code, err_str, playerSitStatus, ClientLoginReplyFlag, msgExtra,maxPlayers, chipsAfter
	//msgExtra: "2[extra...]" clientMsgStrMap[..],  /msgExtra could be empty/

#define MSG_TABLE_REQUESTACTION		0x0007
	// S->C, "(14)\0x0,4717" ( action, amount ), actionId, seconds, isNewGame, round 
	// if action == '*' format is 114444, i.e. '*', action, min, max, step, chipsQuantum


#define MSG_TABLE_ACTION			0x0008
	//C->S, "141", action, amount, fold&stay

//#define MSG_TABLE_PLAYERCARDS		0x0009
	//S->C, ("111")*n,'\0',48, suit('s'/'h'/'d'/'c'), rank (2-10 - as is, 11 - J, 12 - Q, 13 - K, 14 - A), shown_flag
	//End of list is marked with '\0', specialServClientInt, handId64

#define MSG_TABLE_CANCELACTION		0x000A
	//S->C, "1s" code (NoErrorButMes possible), reason

#define MSG_TABLE_SIT_KICK			0x000B
	//S->C, "sm", reason, msgExtra - last string in msgExtra contains socialToken - it will be empty unless it's a club tournament and user award > 0

#define MSG_TABLE_LOGOUT_REPLY		0x000C
	//S->C, "1s" err_code, err_str

#define MSG_TABLE_SITIN				0x000D
	//C->S, "" 

#define MSG_TABLE_SITIN_REPLY		0x000E
	//S->C, "1s14" err_code, err_str, playerSitStatus, ClientLoginReplyFlag

#define MSG_TABLE_SITOUT			0x000F
	//C->S, "" 

#define MSG_TABLE_SITOUT_REPLY		0x0010
	//S->C, "1s1" err_code, err_str, playerSitStatus

#define MSG_TABLE_GET_CARTOON		0x0011
	//C->S, "4" cartoonName

#define MSG_TABLE_GET_CARTOON_REPLY	0x0012
	//S->C, "4B" cartoonName, cartoon

#define MSG_TABLE_FORCE_SITOUT		0x0013
	//S->C, "1s1m" code (NoErrorButMes possible), reason, playerSitStatus, msgExtra

#define MSG_TABLE_ADVANCED_BET		0x0014
	//S->C, ("14")*, action, amount, BYTE( 0 );
	// C 0xFFFFFFFF - Call_any
	// R 0xFFFFFFFF - Raise_any

#define MSG_TABLE_HAND_RANK			0x0015
	//S->C, "ss" line_Hi, line_Lo

#define MSG_TABLE_SIT1				0x0016
	//C->S, "14" sitNum, verifyKey

#define MSG_TABLE_SIT1_REPLY		0x0017
	//S->C, "1s4444 14 51m m448" err_code, reason(if err_code), chipsAvail, minTableChips,
	//   maxBuyIn,chipsQuantum, playAddedMask, playAddedAmount, sitNum, clientServFlag, clientMsg_txtEx (see MSG_TABLE_SIT2_REPLY), 
	//	 currencyContextCli, defaultBuyIn, ratHoleSeconds, pmChips64

#define MSG_TABLE_MTT_CHANGETABLE	0x0018
	//S->C, "1s" code (NoErrorButMes possible), reason

#define MSG_TABLE_ADDCHIPS			0x0019
	//C->S "4" chips

#define MSG_TABLE_ADDCHIPS_REPLY	0x001A
	//S->C "1sm4" err_code, (reason), msgExtra (see MSG_TABLE_SIT2_REPLY), chipsReserve

#define MSG_TABLE_STATISTIC			0x001B
	//S->C "4448" statisticWord, earnedVPP, mileStoneAward, handId, vppThisHand

#define MSG_TABLE_ENTROPY			0x001C
	//C->S "B" var block

#define MSG_TABLE_CHIPSAVAIL		0x001D
	//C->S

#define MSG_TABLE_CHIPSAVAIL_REPLY	0x001E
	//S->C, "1s44 4414 4mm4 448" err_code, reason(if err_code), chips, minTableChips,
	//                 maxBuyIn, chipsQuantum, playAddedMask, playAddedAmount,
	//                 chipsHandStarted, msgTxtExtra, msgCurrency, ratHoleSeconds,
	//                 it.maxAllowed, totalRebuys, pmChips64

#define MSG_TABLE_SIT2_KICK			0x001F
	//S->C, "sm", reason, msgExtra 

#define MSG_TABLE_ADMIN_INFO		0x0020
	//S->C, "sm", message/info, msgExtra 

#define MSG_TABLE_TIMEBANK			0x0021
	//C->S, S->C "4" time (in seconds), added "+1" S->C, auto-activate sign 

#define MSG_TABLE_SITTING_STATUS	0x0022
	//S->C, "1", playerSitStatus

#define MSG_TABLE_WARNING_BELL		0x0023
	//S->C, "7" timeLeft (in seconds)

#define MSG_TABLE_REQUESTACTION_REPLY 0x0024
	//C->S

#define MSG_TABLE_REBUYCHIPS		0x0025
	//C->S, "6" 1 = regular rebuy, 2-3 = double, triple, etc... rebuys

#define MSG_TABLE_REBUYCHIPS_REPLY	0x0026
	//S->C, "1[sm|4444444s866647777]" err_code, [reason, msgExtra (if err_code)|tournamentchips,chips,playchips,fpp,tchips,wchips,isAamsTicket,info,pChips64,rebuysDone,addonsDone,rebuysLef,finalFlightId,moneyAfter,realMoney,realTchips,tChipsAfter]

#define MSG_TABLE_ADDONCHIPS		0x0027
	//C->S

#define MSG_TABLE_ADDONCHIPS_REPLY	0x0028
	//S->C, "1[sm|4444444s866647777]" err_code, [reason, msgExtra (if err_code)|tournamentchips,chips,playchips32,fpp,tchips,wchips,isAamsTicket,info,pChips64,rebuysDone,addonsDone,rebuysLef,finalFlightId,moneyAfter,realMoney,realTchips,tChipsAfter]

//#define MSG_TABLE_AUTO_RELOAD		0x0029  //is obsolete due to automatic auto-rebuy on server side
	//S->C, "44" minTableChips, maxBuyIn

//#define MSG_TABLE_AUTO_RELOAD_1	0x0030 // never used, see up & down
	//S->C, "444" minTableChips, maxBuyIn, playerChipsReal

//#define MSG_TABLE_ADDCHIPS_AUTO	0x0031  //is obsolete due to automatic auto-rebuy on server side
	//C->S "4" chips

#define MSG_TABLE_ADDCHIPS_AUTO_REPLY	0x0032
	//S->C "1s4m" err_code, (reason), realChipsAdded, msgExtra (see MSG_TABLE_SIT2_REPLY), chipsReserve

//#define MSG_TABLE_PLAYERCARDS_DRAWSETS	0x0033
	//S->C "81,(1(11)*n,)*r , handId64, num_sets, draw_mask, suit, rank
		// r - number of sets, n - number(bitmask actually) of cards in set

#define MSG_TABLE_CHAT2					0x0034 
	//C->S, "s" 

#define MSG_TABLE_CHAT2_REPLY			0x0035
	//S->C, "2ms, err_code, body( parameters to format error descr), original chat string  

//#define MSG_TABLE_FOLDCARDS				0x0036
	//S->C, ("11")*n,'\0', (suit, rank)*n, 0-terminator

#define MSG_TABLE_SET_AUTO_REBUY		0x0037
	//C->S, "5222" autoRebuyOptions, bigBets, underBigBets, underPercents

#define MSG_TABLE_SET_AUTO_REBUY_REPLY	0x0038
	//S->C, "1", 0-Ok, 1-autoRebuy format error (to be extended probably..)

#define MSG_TABLE_SET_SHOW_WHEN_ALLIN	0x0039
	//C->S, "b"

#define MSG_TABLE_BOUNTY_GREETINGS		0x0040
	//S->C, *("4s")'\0x0'- (chips, bountyLoser)*, UINT32(0)-terminator

#define MSG_TABLE_SET_DEAL				0x0041
	//C->S, "b", usage lobby.h::enum eDiscussDeal, true=set, false=reset

#define MSG_TABLE_BLITZ_KICK			0x0042
	//S->C, "8444", handId, statistcs, chips, flags

#define MSG_TABLE_FAST_FOLD				0x0043
	//C->S, "81", handId, fold&stay

#define MSG_TABLE_SITOUT_NEXTHAND		0x0044
	//C->S, "1", byte 1- checkBox, 0 - uncheck

#define MSG_TABLE_GOTO_NEXTHAND			0x0045
	//C->S, "8",  handId //For Blitz: Cancel fold&stay action

#define MSG_TABLE_PLAYNOW				0x0046
	//C->S, ""

#define MSG_TABLE_REFRESH				0x0047
	//C->S, ""

#define MSG_TABLE_REFRESH_REPLY			0x0048
	//S->C, "4" chipsReserve

#define MSG_TABLE_SET_RUN_IT_TWICE_AND_MORE      0x0049
	//C->S, "bb" runItTwiceOption, cashoutNotFavorOption

#define MSG_TABLE_BLITZ_MTT_KICK		0x004A
	//S->C, ""
#define MSG_TABLE_START_CAMPING_PREVENTION 0x004B
	//S->C, "21" campingTimeOut, sittingStatus

#define MSG_TABLE_USER_CHAT_WARN           0x004C
	//S->C "2s"  warnMsgId, filterChatMsg-or-empty

#define MSG_TABLE_USER_CHAT_BAN            0x004D
	//S->C "24"  banMsgId, banTimeInMin

#define MSG_TABLE_GET_DEAL				0x004E
	//C->S, ""

#define MSG_TABLE_GET_DEAL_REPLY			0x004F
	//S->C, "b", usage lobby.h::enum eDiscussDeal, true=set, false=unset

#define MSG_TABLE_PLAYER_CARD_DICTIONARY_UPDATE	0x0050  // PYR-49502
	//S->C, see storm/TableClientProtocols.txt

#define MSG_TABLE_USER_DICTIONARY_UPDATE 0x0051
	//S->C, see storm/TableClientProtocols.txt

#define MSG_TABLE_SUBSCRIPTION_OVER_CONNECTION 0x0100   
	//S<->C, PYR-30088, for implementing table publication over player's private connection

//****************************************************************************
//All the below do not work alone within one update, if no one Leaf/Node updated.
//So add something like chat event in combination with one update.
//Or fix the problem on global level (commlib or tableData?)
//(discoverd first time at 40B hand celebrating: MSG_TABLE_SUBSCR_MESSAGE)

#define MSG_TABLE_SUBSCR_MOVETOPOT			0x1001
	// "1(14)*", potNum,sitNum, amount 
	// end of list is marked with (UINT16)(-1) 

#define MSG_TABLE_SUBSCR_MOVEFROMPOT		0x1002
	// "1(14)*", potNum,sitNum, amount
	// end of list is marked with (UINT16)(-1) 

#define MSG_TABLE_SUBSCR_ACTION				0x1003
	// "114", sitNum, action, amount

#define MSG_TABLE_SUBSCR_DEALPLAYERCARDS	0x1004
	// ("11")*, (sitNum, nCards)*, list terminator(-1), dealer 

#define MSG_TABLE_SUBSCR_DEALBOARD			0x1005
	// "1", nCards

#define MSG_TABLE_SUBSCR_BEGINHAND			0x1006
	// ""

#define MSG_TABLE_SUBSCR_ENDHAND			0x1007
	// "1", reason: 0=normal, 1=canceled, 2=notstarted

#define MSG_TABLE_SUBSCR_MESSAGE			0x1008
	// "s242m" string of message, Delay_time in seconds, Priority( 0 - usual, 1 - admin, 2 - popup etc.), clientMsgStrMap, msgExtra/could be empty/

#define MSG_TABLE_SUBSCR_NEWGAME			0x1009
	// "777111" ,stakeSmall, stakeBig, ante, handType, isHiLo, potStruct

#define MSG_TABLE_SUBSCR_MESSAGE2			0x1010
	// "s244" string of message, Delay_time in seconds, Priority( 0 - usual, 1 - admin, 2 - popup etc.),site 

#define MSG_TABLE_SUBSCR_I18N_MESSAGE		0x1011
	// "4(4s)2444" i18nString of message, Delay_time in seconds, flags, msgId, brandMask

#define MSG_TABLE_SUBSCR_CHAT_BUBBLE		0x1012
	// "1s" sitNum, chatStr

#define MSG_TABLE_SUBSCR_SET_RECORDING_PAUSE 0x1013	// #18024
	// "b", pause set

#define MSG_TABLE_SUBSCR_ACTIVESEAT			0x1014	// #18562
	// "51", active seat, action flag

#define MSG_TABLE_SUBSCR_DEALBOARD_RIT      0x1015  // #16658
	// "111", boardnumber, numOfCardsDealt, numOfBoardCards

#define MSG_TABLE_SUBSCR_DELAYED_TABLECLOSE  0x1016  // #25513
	// "14", startFlag, countingDownSecs

#define MSG_TABLE_NSTACK_RELOAD              0x1017  // #25226
    // "4", stacks

#define MSG_TABLE_NSTACK_RELOAD_REPLY        0x1018  // #25226
    // "1s4"/"14", errcode, errstring,clientErrorMsg/remaining stacks

#define MSG_TABLE_INVITE_CHOOSE_STARTING_STACKS  0x1019  // #25226  
	// 4 - stacks

#define MSG_TABLE_AUTO_RELOAD_STACKS_NOTIFY    0x101A  // #25226  
	// 4 - stacks loaded

#define MSG_TABLE_USER_SPECIFIC_BROADCAST    0x101B  // PYR-23129
	// 4<I4> - msgCount<i18nmsgText, msgId>

#define MSG_TABLE_USER_MINI_CHAT_REQUEST     0x101C // PYR-40718
	// 2 - textId
#define MSG_TABLE_USER_MINI_CHAT_BROADCAST   0x101D // PYR-40718
	// 12 - seat, textId

#define MSG_TABLE_USER_POOL_BLOCK_STATE_UPDATE  0x101E  // PYR-42791
	// 1444<4> - penaltyEnabled, numHandsPlayed, minNumHandsRequired, timePenaltyArraySize<timePenaltyMin>

#define MSG_TABLE_SUBSCR_ANIMATION              0x101F  // PYR-39403
	// s14m - userId, seat, animationType, extraBody

#define MSG_TABLE_SUBSCR_POT_DISTRIBUTION		0x1021  // PYR-49389
	// see TableClientProtocols.txt

#define MSG_TABLE_SAGMAX_USER_DRAW				0x1022  // PYR-51310
	// C->S 4 - actionCode: 0 -timeout, 1- 1st spinner,...4 - cashout

#define MSG_TABLE_SUBSCR_REVEAL_UNFOLD_ACTIONS	0x1023  // PYR-65734
	// see TableClientProtocols.txt

#define MSG_TABLE_SUBSCR_SHOW_POT_WINNERS		0x1024  // PYR-65734
	// see TableClientProtocols.txt

#define MSG_TABLE_SUBSCR_RETURN_UNCALLED_BET    0x1025  // PYR-65734
	// see TableClientProtocols.txt

#define MSG_TABLE_IHH_GET_HANDBLOB              0x1026  // PYR-72484
#define MSG_TABLE_IHH_GET_HANDBLOB_REPLY        0x1027  // PYR-72484
	// see TableClientProtocols.txt

#define MSG_TABLE_THROW							0x1028  // PYR-98733
#define MSG_TABLE_THROW_REPLY					0x1029  // PYR-98733
	// see TableClientProtocols.txt

#define MSG_TABLE_SUBSCR_THROW					0x1030  // PYR-98733
	// see TableClientProtocols.txt

#define MSG_TABLE_SUBSCR_KNOCKOUT				0x1031  // PYR-123867
	// see TableClientProtocols.txt


//****************************************************************************
// S->DBM
/*
Hand History Events:
--------------------
HISTORY_HAND_ACTION  - Any of Player actions (Fold,Call,Raise,Show,Muck and others, All-in including)
HISTORY_HAND_DEALING - Dealing one card (any of this, board, or player down or up ) Position = seat, = -1 if Board, Amount = Cards_number
HISTORY_HAND_TIMEOUT - Any of Player' timeout, when default action is taken 
HISTORY_HAND_ALLIN   - Player post last money and is all_in 
HISTORY_HAND_MOVETOPOT - Action = Pot_number
HISTORY_HAND_TAKEFROMPOT - Player win (tie) any pot (side or main, Lo or Hi)
HISTORY_HAND_EXTRA -	Cancel      - Position_number = -1,             Action = 0x0
						Capped      - Position_number = -1,             Action = 0x1
						Button      - Position_number = num (!= -1),    Action = 0x2
						Pair4stud   - Position_number = num (!= -1),    Action = 0x3
						NoLowHand   - Position_number = -1,             Action = 0x4
	Actions = 0x5 - 0xE currently used at Client side for various special chat events:
						OpensLowest - Position_number = num (!= -1),    Action = 0x5,  amount = 1|0 high/low
						Opens_betting_round								Action = 0x6
						Has_for_Lo										Action = 0x7,  contents = combLo
						Its_your_turn_You_have_N_sec_to_act				Action = 0x8,  Position = seat - warning
						A_new_game_will_start_in									   Position_number = -1

						Player_has_disconnected                         Action = 0x9,  Position = seat
						Tournament_clock_is_stopped                                    Position_number = -1

						Many_players_have_disconnected	                Action = 0xA,  Position = 0
						Players_have_reconnected                                       Position = 1  

						Was_moved_from_another_table    amount = 0,     Action = 0xB,  Position_number = seat
						Finished_the_tournament         amount = N,
						Wins_the_tournament             amount = 1

						Tournament_is_now_played_Hand-for-Hand			Action = 0xC,  Position = 1
						Resuming_normal_play_no_longer_Hand-for-Hand                   Position = 0

						Re-buy_period_is_closed			                Action = 0xD,  Position = 0
						Add-on_available_during_this_break                             Position = 1

						The_break_will_start							Action = 0xE,  Position = 0
						Players_are_now_on_break                                       Position = 1


													free to use:        Action = 0xF (see formatHistory2.cpp)
																	.....reserved......

						Reshuffling - Position_number = num (!= -1),    Action = 0x70,  amount = numCards
						DrawSpecial - Position_number = round,			Action = 0x71
						MoveChipsToPocket - Position = seat (!= -1),    Action = 0x72,  amount = Chips
						AllInSwow   - Position_number = round,			Action = 0x73,  amount = PlayersMask
						Knocking    - Position_number = num (!= -1),    Action = 0x74,  contents = knockers\v, amount = award
						Bounty      - Position_number = num = -1,       Action = 0x75,  contents = knockers\v, amount = award
						FinishedT   - Position_number = winFlags,       Action = 0x76,  amount = place, contents = award
						WonTourn    - Position_number = winFlags,       Action = 0x77,  amount = place, contents = award
						WonAward    - Position_number = seat (!= -1),   Action = 0x78,  amount = tournId, contents = ticketAdmission


Table History Events:
--------------------
HISTORY_TABLE_JOINS   - Player sits/logOut to/from the table (Action = 0x1 Joins, 0x0 - Leaves )
HISTORY_TABLE_SITIN   - Player have been moved to SitIn/SitOut state (Action = 0x1 - Joins, 0x0 - Leaves )
HISTORY_TABLE_CONNECT - Player have connected (reconnected )/disconnected to the table ( Action = 0x1 - connect, 0x0 - disconnect )
HISTORY_TABLE_KICKED  - Player have been kicked from the table for missing X blinds or ante (or other reason)

Hand Label structure is:

Label (hand) = 444451s1[U...] 2[f...]
table_ID, hand_ID, rake_Size, pot_Size, dealer, NewGame_Flag, board_Cards, num_users, [UserInHand...], num_frames [HistoryItem...]

UsersNumber (UserInHand) = 1[U...] 
U (UserInHand) = s147s1ss userId, position, startAmount, delta(signed), cards, BestHand, Comb_Hi, Comb_Lo

NumFrames(frame): hand_id, num_items, [HistoryItem...] = 42[h...] // hand_Id = 0 means no_hand history Frame

h (HistoryItem) = 442554ss
TableOrdinal, Time, EventCode,/|\/|\Position, Action, Amount, UserId, Contents

TableOriginal: N=0, N++
Event Code   : HISTORY_PLAYER_ACTION, or HISTORY_HAND_DEALING, etc..
Position     : Seat_Number
Action       : 'F','c','R','M' /Fold,Check,Raise,Muck/ etc.
Amount       : 300 ($ | play chips)
UserID       : str: NickName
Contents     : str_chat:"Hi, there!" (also: Cards, Pot_Num, etc. )
*/


//==============================================================================================================
// table storm power definitions and messages
//==============================================================================================================

enum PokerGamePowerStatus
{
	GamePowerDisabled = 0,
	GamePowerAvailable = 1,
	GamePowerPreSelected = 2,
	GamePowerActivated = 3,
	GamePowerYellow = 4,
	GamePowerDisabledWithEMP = 5,
};


enum PokerGamePowerId
{
	PokerGamePowerDealPowerCards = -100,   // not a real power id, just for the protocol reuse convenience

	PokerGamePowerStealth = 1,
	PokerGamePowerDisarm = 2,
	PokerGamePowerDetect = 3,
	PokerGamePowerXRay = 4,
	PokerGamePowerPacify = 5,  // Leap
	PokerGamePowerMindControl = 6,
	PokerGamePowerStretch = 7,
	PokerGamePowerParasite = 8,
	PokerGamePowerUpgrade = 9,  // Telekinesis
	PokerGamePowerReload = 10,
	PokerGamePowerRunItTwice = 11,
	PokerGamePowerIntel = 12,  // Sense
	PokerGamePowerMorph = 13,
	PokerGamePowerZombie = 14,
	PokerGamePowerLeeroyJenkins = 15,
	PokerGamePowerClone = 16,
	PokerGamePowerChaos = 17,
	PokerGamePowerTeleport = 18,
	PokerGamePowerMint = 19,  // Summon
	PokerGamePowerSwap = 20,
	PokerGamePowerLowBlow = 21,
	PokerGamePowerAnteUp = 22,
	PokerGamePowerDisintegrate = 23,  // Assassinate
	PokerGamePowerScanner = 24,  // ChangeFuture
	PokerGamePowerExtraLife = 25,
	PokerGamePowerEngineer = 26,  // PlayersChoice
	PokerGamePowerSplashPot = 27,
	PokerGamePowerSuitUp = 28,
	PokerGamePowerTakeBack = 29,
	PokerGamePowerDummySpecimen = 30,
	PokerGamePowerFreeze = 31,
	PokerGamePowerEMP = 32,  // Silence
	PokerGamePowerSecretPlaceholder = 33,
	PokerGamePowerSabotage = 34,
	PokerGamePowerSyphon = 35,  // ManaDrain
	PokerGamePowerCounterSpell = 36,
	PokerGamePowerReboot = 37,
	PokerGamePowerDeploy=38,  // Genesis
	PokerGamePowerEqualizer=39,
	PokerGamePowerDarkBet=40,
// add new power ids ABOVE this line

	PokerGamePowerDebugSlot1, // note: no fixed ids for Debug powers, they must start immediately after the last production power-up
	PokerGamePowerDebugSlot2,
	PokerGamePowerDebugSlot3,
 
	PokerGamePowerLastSlot  // MUST be on the last place
};

enum PokerGamePowerEffect
{
	PokerGamePowerEffectListUpdate = 1,
	PokerGamePowerEffectConfigUpdate = 2,
	PokerGamePowerEffectReplaceHoleCards = 3,
	PokerGamePowerEffectReplaceBoardCard = 4,
	PokerGamePowerEffectPlayerPowerListUpdated = 5,
	PokerGamePowerEffectPlayerPowerStatusUpdate = 6,
	PokerGamePowerEffectPlayerUnselectPower = 7,
	PokerGamePowerEffectReplaceMultiBoardCards = 8,
	PokerGamePowerEffectShowOneHoleCard = 9,
	PokerGamePowerEffectDiscardHoleCards = 10,
	PokerGamePowerEffectReceiveHoleCards = 11,
	PokerGamePowerEffectFrozenCardHit = 12,
	PokerGamePowerEffectManaDelta = 13,
	PokerGamePowerEffectManaRewardDelta = 14,
	PokerGamePowerEffectFreeChipsReceived = 15,
	PokerGamePowerEffectMindControlDrawNewPower = 16,
	PokerGamePowerEffectFreezeBoardCardsByPlayerAllIn = 17,
	PokerGamePowerEffectExchangePowers = 18,
	PokerGamePowerEffectChipsCarryOver = 19,
	PokerGamePowerEffectRebootPowers   = 20,
	PokerGamePowerEffectAddBoardCard   = 21,
	PokerGamePowerEffectEMPTurnsOff = 22,
	PokerGamePowerEffectDarkBetEnd = 23,
	PokerGamePowerEffectDarkBetCall = 24,
	PokerGamePowerEffectIntelCard = 25,
	PokerGamePowerEffectXRay = 26,
	PokerGamePowerEffectDisintegrate = 27,
	PokerGamePowerEffectScanner = 28,
//	PokerGamePowerEffectUpgrade = 29,
//	PokerGamePowerEffectReloadHoleCards = 30,
	PokerGamePowerEffectPowerPlayEnd = 31,
};


#define MSG_TABLE_POWER_FAKED_GAME_ACTION                0x5000  // pure server internal
#define MSG_TABLE_POWER_CHOOSE_POWER_SLOTS               0x5001  // S->C:  444114<4> - powerActionSeqId, powerId, timerInterval, minDraws, maxDraws, vector< powerId >
#define MSG_TABLE_POWER_CHOOSE_POWER_SLOTS_ACK           0x5002  // C->S:  4 - powerActionSeqId
#define MSG_TABLE_POWER_CHOOSE_POWER_SLOTS_REPLY         0x5003  // C->S:  44<4> - powerActionSeqId, vector< powerIdChosen >
#define MSG_TABLE_POWER_CHOOSE_POWER_SLOTS_CANCEL        0x5004  // S->C:  4 - powerActionSeqId
#define MSG_TABLE_POWER_CHOOSE_HOLECARDS_DISCARD         0x5005  // S->C:  44411 - powerActionSeqId, powerId, timerInterval, minNumChosen, maxNumChosen
#define MSG_TABLE_POWER_CHOOSE_HOLECARDS_DISCARD_ACK     0x5006  // C->S:  4 - powerActionSeqId
#define MSG_TABLE_POWER_CHOOSE_HOLECARDS_DISCARD_REPLY   0x5007  // C->S:  41<11> - powerActionSeqId, vector<suit,rank>
#define MSG_TABLE_POWER_CHOOSE_HOLECARDS_DISCARD_CANCEL  0x5008  // S->C:  4 - powerActionSeqId
#define MSG_TABLE_POWER_CARD_PLAY                        0x5009  // C->S:  44[1[11[11]]] - actionId, powerId, [ targetPlayer, [ targetHoleCardSuit, targetHoleCardRank, [ targetBoardCardSuit, targetBoardCardRank] ] ]
#define MSG_TABLE_POWER_CARD_PRESELECT                   0x500A  // C->S:  4b[1[11[11]]] - powerId, selectOrUnselect, [targetPlayer, [ targetHoleCardSuit, targetHoleCardRank, [ targetBoardCardSuit, targetBoardCardRank] ] ]
//#define MSG_TABLE_POWER_DISCARD_POWERS                   0x500B  // C->S:  4<4> - vector< powerId >
//#define MSG_TABLE_POWER_ENABLE_DISCARD_POWERS            0x500C  // S->C:  b - enableDiscard
//#define MSG_TABLE_POWER_DISCARD_POWERS_CANCEL            0x500D  // S->C:  <empty msg>  // only when Zombie is used
#define MSG_TABLE_POWER_FAILED_TO_PLAY                   0x500E  // S->C:  1s4 - errcode, errstr, powerId  -- obsolete
#define MSG_TABLE_POWER_DETECT_HOLECARDS                 0x500F  // S->C:  11<11> - targetPlayer, vector<suit,rank>
#define MSG_TABLE_POWER_POSSESSION                       0x5011  // S->C:  4<M(44)> - vector< MsgBody(powerId, status) >
#define MSG_TABLE_POWER_STATUS_UPDATE                    0x5012  // S->C:  4<M(44)> - vector< MsgBody(powerId, newStatus) >
#define MSG_TABLE_POWER_REDRAW                           0x5013  // S->C:  4444 - seqId, costChips, maxAllowedCardsToRedraw,timeoutSec
#define MSG_TABLE_POWER_REDRAW_ACK                       0x5014  // C->S:  4 - seqId 
#define MSG_TABLE_POWER_REDRAW_REPLY                     0x5015  // C->S:  4b2<4> - seqId, isFinal, vector<powerId>
#define MSG_TABLE_POWER_REDRAW_CANCEL                    0x5016  // S->C:  4 - seqId
#define MSG_TABLE_POWER_CHANGE_FUTURE                    0x5017  // S->C:  4114 - seqId, cardSuit, cardRank, timeout  
#define MSG_TABLE_POWER_CHANGE_FUTURE_ACK                0x5018  // C->S:  4 - seqId
#define MSG_TABLE_POWER_CHANGE_FUTURE_REPLY              0x5019  // C->S:  4b11 - seqId, discardCard, cardSuit, cardRank 
#define MSG_TABLE_POWER_CHANGE_FUTURE_CANCEL             0x501A  // S->C:  4 - seqId
#define MSG_TABLE_POWER_PLAYERS_CHOICE                   0x501B  // S->C:  41<11>4 - seqId, vector<cardSuit, cardRank>, timeout  
#define MSG_TABLE_POWER_PLAYERS_CHOICE_ACK               0x501C  // C->S:  4 - seqId
#define MSG_TABLE_POWER_PLAYERS_CHOICE_REPLY             0x501D  // C->S:  41 - seqId, selectedIndex
#define MSG_TABLE_POWER_PLAYERS_CHOICE_CANCEL            0x501E  // S->C:  4 - seqId
#define MSG_TABLE_POWER_PRIVATE_EFFECT					 0x501F  // S->C:  8444s - handId, clientOrdinal, powerId, effectId, content
//#define MSG_TABLE_POWER_SENSED_CARDS                     0x5020  // S->C:  1<1111> - vector<seat,position,cardSuit,cardRank>
#define MSG_TABLE_POWER_SABOTAGE_TRAPPED_CARDS           0x5021  // S->C:  1<111> - vector<boardNumber,cardPosition,bombCountr>
#define MSG_TABLE_POWER_CARD_PLAY_REPLY                  0x5022  // S->C:  1s4 - errcode, errstr, powerId
#define MSG_TABLE_POWER_DARK_BET						 0x5023  // S->C:  41<4> - seqId, vector<darkBetValue>, timeout  
#define MSG_TABLE_POWER_DARK_BET_ACK					 0x5024  // C->S:  4 - seqId
#define MSG_TABLE_POWER_DARK_BET_REPLY		             0x5025  // C->S:  44 - seqId, value 
#define MSG_TABLE_POWER_DARK_BET_CANCEL			         0x5026  // S->C:  4 - seqId
#define MSG_TABLE_POWER_MOUSE_OVER						 0x5027  // C->S:  1 - position  // PYR-41963
#define MSG_TABLE_POWER_PRIVATE_ANIMATION				 0x5028  // S->C:  84M() - handId, powerId, body




#define MSG_TABLE_SUBSCR_POWER_PLAY                      0x5501  // PYR-36156
                                                                 // 4M<1>M<> - powerId, M(playerId), M()
//#define MSG_TABLE_SUBSCR_POWER_PLAYERS_CHOICE_START      0x5502  // PYR-35534
                                                                 // 11<11>   - seat, vector<cardSuit, cardRank>
//#define MSG_TABLE_SUBSCR_POWER_PLAYERS_CHOICE_END        0x5503  // PYR-35534
                                                                 // 11<11>11 - seat, vector<cardSuit, cardRank>,selectedCardSuit, selectedCardRank
//#define MSG_TABLE_SUBSCR_POWER_EFFECT_BOARD_CARD		 0x5504  // PYR-36651
																 // 41 - powerId, cardPosition
//#define MSG_TABLE_SUBSCR_POWER_EFFECT_HOLE_CARD			 0x5505  // PYR-36652
																 // 411 - powerId, seat, cardPosition
//#define MSG_TABLE_SUBSCR_POWER_EFFECT_PLAYER			 0x5506  // PYR-36655
																 // 41 - powerId, seat

enum eBoardCardFlags  // used in PowerInfoOnBoardCard.flag (INT8 type)
{
	eBoardCardFlags_Frozen = 1 << 0, // 0x01
	eBoardCardFlags_Common = 1 << 1, // 0x02
};

#define DECK_PSEUDO_SEAT_NUMBER (-10)

//==============================================================================================================
// table definitions and messages for Poker Duel
//==============================================================================================================
// Note: when adding a new poker duel game message, please also check DuelGameController::clearPlayerCachedMsgs()
// PYR-53789 - obsolete
//#define MSG_DUEL_TABLE_REQUEST_ACTION                   0x6000    // 24576
//#define MSG_DUEL_TABLE_ACTION                           0x6001    // 24577
//#define MSG_DUEL_TABLE_ACTIONREQUEST_CANCELATION        0x6002    // 24578
//#define MSG_DUEL_TABLE_INIT_SNAPSHOT                    0x6003    // 24579
//#define MSG_DUEL_TABLE_DEAL_HOLE_CARDS                  0x6004    // 24580
//#define MSG_DUEL_TABLE_REVEAL_HISTORY_ITEMS             0x6005    // 24581
//#define MSG_DUEL_SYNC_ALL                               0x6006    // 24582
//#define MSG_DUEL_SYNC_ALL_END                           0x6007    // 24583
//#define MSG_DUEL_TABLE_SYNC                             0x6008    // 24584
//#define MSG_DUEL_TABLE_SYNC_END                         0x6009    // 24585
//#define MSG_DUEL_TABLE_CHAT                             0x600A    // 24586
//#define MSG_DUEL_TABLE_PLAYER_TIMEBANK_TRIGGERED        0x600B    // 24587
//#define MSG_DUEL_TABLE_COMPOUND                         0x600C    // 24588
//#define MSG_DUEL_TABLE_COMPOUND_ACK                     0x600D    // 24589
//#define MSG_DUEL_TABLE_TEMP_NO_MORE_ACTION              0x600E    // 24590
//#define MSG_DUEL_TABLE_CHECK_GAME_PENDING_ACTION        0x600F    // 24591
//#define MSG_DUEL_PLAYER_SCORES_UPDATE                   0x6010    // 24592
//#define MSG_DUEL_TABLE_PLAYER_HAND_RANKS                0x6011    // 24593
//#define MSG_DUEL_PLAYER_REVEALED_ALL_HANDRESULTS        0x6012    // 24594
//#define MSG_DUEL_PAUSED_STATUS_CLIENT_NOTIF             0x6013    // 24595
//#define MSG_DUEL_CLIENT_STORE_SPECIFIC_DATA             0x6014    // 24596
//#define MSG_DUEL_CLIENT_RETRIEVE_SPECIFIC_DATA          0x6015    // 24597
//#define MSG_DUEL_TABLE_HAND_REVEAL_CONFIRM_REQUEST      0x6016    // 24598
//#define MSG_DUEL_TABLE_HAND_REVEAL_CONFIRMED            0x6017    // 24599


#endif
