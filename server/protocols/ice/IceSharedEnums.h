// icesharedenums.h 
////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2014 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef icesharedenums_h_included
#define icesharedenums_h_included

#define NUM_STATE_FIELDS		5
#define NUM_DISPLAY_STATES		5
#define MAX_LOCAL_EVENT_VARS	100

#define ICE_C2L_HANDLER_TAG					"{c2l_handler}"
#define ICE_C2L_HANDLER_DEFAULT				"pokerstars"
#define ICE_C2L_URL_TAG						"{c2l_url}"
#define ICE_C2L_URL_DEFAULT					"www.pokerstars.com"
#define ICE_LEADERBOARD_YEAR				2099
#define SPORTS_CAMPAIGN_DEFAULT				"SCD"
#define IB_CURRENCY_DEFAULT					"IBCD"
#define ICE_REWARD_RANDOM_GRANULARITY		100000000
#define ICE_DYN_CONTENT_TAG					"{Content"
#define ICE_DYN_CONTENT_NUM_SCRIPT_PARTS	3
#define ICE_DYN_CONTENT_NUM_TASK_PARTS		3
#define ICE_DYN_CONTENT_NUM_REWARD_PARTS	4
#define ICE_DYN_CONTENT_MAX_TAGS			56

#define DEBUG_FORCE_OPT_IN_NO_REF_NEEDED	-1
#define PYR_DEBUG_NO_FORCE_REF				"PYR_DEBUG_NO_FORCE_REF"

enum IceNotifyFlags
{
	eIceNotifyFlagIceAvailable		= 1<<0, // 1
	eIceNotifyFlagIceNewQuest		= 1<<1,	// 2
	eIceNotifyFlagIceTaskCompleted	= 1<<2,	// 4
};


// task is created with InDevelopment state
// admin can switch a task to Active state from InDevelopemt. 
// Then it will automatically switch to appropriate state based on task timing parameters.
//
enum eIceTaskStatus
{
	eIceTaskStatus_Unknown			= 0,
	eIceTaskStatus_Obsolete			= 1,	// task is not loaded
	eIceTaskStatus_InDevelopment	= 2,
	eIceTaskStatus_Active			= 3,
	eIceTaskStatus_Archived			= 4,
};

enum eIceTaskFlags
{
	eIceTaskFlags_InviteOnly							= 1<<0,
	eIceTaskFlags_Recurrent								= 1<<1,
	eIceTaskFlags_HideUntilOptIn						= 1<<2,
	eIceTaskFlags_LeaderBoard							= 1<<3,
	eIceTaskFlags_LeaderBoardAchievement				= 1<<4,
	eIceTaskFlags_HiddenCompletely						= 1<<5,
	eIceTaskFlags_NotifyOnSuccessfulCompletion			= 1<<6,
	eIceTaskFlags_RestrictMinutesToComplete				= 1<<7,
	eIceTaskFlags_HideAfterAvailabilityEnds				= 1<<8,
	eIceTaskFlags_SucceedOnMinutesToCompleteTimeout		= 1<<9,
	eIceTaskFlags_ShowIfInProgressQRefEligible			= 1<<10,
	eIceTaskFlags_OptInSchedule							= 1<<11, // cannot be set on a leader board
	eIceTaskFlags_FullUpdateOnComplete					= 1<<12,
	eIceTaskFlags_DiscardDoneState						= 1<<13,
	eIceTaskFlags_SystemTask							= 1<<14,
	eIceTaskFlags_FragmentedSeasonIgnored				= 1<<15, // this is a transient flag set in memory only - not persisted to DB
	eIceTaskFlags_CreateTrackingInstanceOnActivate		= 1<<16,
	eIceTaskFlags_DynamicPromoContentSource				= 1<<17,
	eIceTaskFlags_DynamicPromoContentSourceIfEligible	= 1<<18,
};

enum eIceTaskPropertyTypes // properties associated with a task for admin to store pretty much anything
{
	eIceTaskProperty_Unknown						= 0,
	eIceTaskProperty_TaskTrackCodeRevision			= 1,
	eIceTaskProperty_TaskMaxAttempts				= 2,
	eIceTaskProperty_TaskMaxSuccesses				= 3,
	eIceTaskProperty_TaskMaxFails					= 4,
	eIceTaskProperty_TaskMinsToDisplayCompleted		= 5,
	eIceTaskProperty_TaskVisibilitySite				= 6,
	eIceTaskProperty_TaskEligibilityRef				= 7,
	eIceTaskProperty_TaskVisibilityBrand			= 8,	
	eIceTaskProperty_TaskVisibilityPlatform			= 9,
	eIceTaskProperty_TaskDynamicTextArray			= 10,
	eIceTaskProperty_TaskForceOptInAfter			= 11,
	eIceTaskProperty_TaskInProgressEligibilityRef	= 12,
	eIceTaskProperty_TaskOptInSchedule				= 13,
	eIceTaskProperty_TotalsGroupRef					= 14,
	eIceTaskProperty_TaskCompletedEligibilityRef	= 15,
	eIceTaskProperty_PuzzleRef						= 16,
	eIceTaskProperty_LBDelaySeasonStartMins			= 17,
	eIceTaskProperty_LBCompleteSeasonEarlierMins	= 18,
	eIceTaskProperty_FailInstanceOnAcceptRef		= 19,
	eIceTaskProperty_DisplayStateMapping			= 20,
	eIceTaskProperty_HasInProgressInstanceQuestRef	= 21, // PYR-94892
	eIceTaskProperty_NotifyCompletionChannel		= 22,
};

//enum eIceTaskCompletionStatus // same as eIceWebUserTaskState
//{
//	eIceTaskCompletionStatus_Unknown	= 0,
//	eIceTaskCompletionStatus_InProgress	= 1,
//	eIceTaskCompletionStatus_Success	= 2,
//	eIceTaskCompletionStatus_Expiring	= 3, // not stored in db
//	eIceTaskCompletionStatus_Expired	= 4,
//	eIceTaskCompletionStatus_Failed		= 5,
//	eIceTaskCompletionStatus_Timedout	= 6,
//};

enum eIceCasinoGameMode
{
	eIceCasinoGameMode_Unknown			= 0,
	eIceCasinoGameMode_SinglePlayer		= 1,
	eIceCasinoGameMode_MultiPlayer		= 2,
};

enum eIceTaskRewardType
{
	eIceTaskRewardType_Unknown				= 0,
	eIceTaskRewardType_Manual				= 1,

	eIceTaskRewardType_TournTicket			= 100,
	eIceTaskRewardType_TournRegister		= 101, // PYR-32125
	eIceTaskRewardType_Cash					= 102, // PYR-32134
	eIceTaskRewardType_Fpp					= 103,
	eIceTaskRewardType_Bonus				= 104,
	eIceTaskRewardType_InstantBonus			= 105,
	eIceTaskRewardType_SportsFreeBet		= 106,
	eIceTaskRewardType_TrackingCode			= 107, // PYR-44572
	eIceTaskRewardType_BonusGameTicket		= 108,
	eIceTaskRewardType_ActiveTrackingCode	= 109, // PYR-55245
	eIceTaskRewardType_RemoveTrackingCode	= 110, // PYR-55474
	eIceTaskRewardType_MultiPurposeChest	= 111, // PYR-59899
	eIceTaskRewardType_PredictorTicket		= 112, // PYR-63220
	eIceTaskRewardType_UnlockChest			= 113, // PYR-77110
	eIceTaskRewardType_StarCode				= 114, // PYR-88826
	eIceTaskRewardType_VoidChest			= 115, // PYR-95957
	eIceTaskRewardType_GiftCode				= 116, // PYR-111873


	eIceTaskRewardType_Random				= 150,
	eIceTaskRewardType_ByState				= 151,

	eIceTaskRewardType_PlayMoney			= 200,
	eIceTaskRewardType_QuestTicket			= 300,
	eIceTaskRewardType_EnrollIntoQuest		= 301,
	eIceTaskRewardType_Badge				= 302,
};

enum eIceTaskRewardConfigParamId
{
	eIceTaskRewardConfigParamId_NotMulti				= 0,
	eIceTaskRewardConfigParamId_Currency				= 1,
	eIceTaskRewardConfigParamId_BudgetCode				= 2,
	eIceTaskRewardConfigParamId_BudgetCodeId			= 3,
	eIceTaskRewardConfigParamId_RandomRewardRef			= 4,
	eIceTaskRewardConfigParamId_SportsFreebetCampainId	= 5,
	eIceTaskRewardConfigParamId_ByState					= 6,
	eIceTaskRewardConfigParamId_CustomCashPctStateLim	= 7,
	eIceTaskRewardConfigParamId_StatePercentage			= 8,
	eIceTaskRewardConfigParamId_StatePercentageLimit	= 9,
	eIceTaskRewardConfigParamId_StatePercentageState	= 10,
	eIceTaskRewardConfigParamId_InstantBonusId			= 11,
	eIceTaskRewardConfigParamId_TournId64				= 12,
	eIceTaskRewardConfigParamId_ChestTypeName			= 13,
	eIceTaskRewardConfigParamId_ChestTypeId				= 14,
	eIceTaskRewardConfigParamId_StatePercentageLimitMin	= 15,
};

enum eIceRewardFlags
{
	eIceRewardFlag_Hide		= 1<<0,
	eIceRewardFlag_Obsolete	= 1<<1,
	eIceRewardFlag_Random	= 1<<2,
	eIceRewardFlag_ByState	= 1<<3,
};

// when the main system issues a reward there must be a way of linking back to it
// for example when we give user cash - it must be a cash transaction id
// when we give user a ticket - it must be a ticket transaction id
// when we register user into a tournament it may be a tournament id or a tournament registration transaction id (to be decided later)
//
enum eIceRewardObjectReferenceType
{
	eIceRewardObjectReferenceType_Unknown			= 0,
	eIceRewardObjectReferenceType_CashTrans			= 1,
	eIceRewardObjectReferenceType_TicketTrans		= 2,
	eIceRewardObjectReferenceType_UserRollTrans		= 3,
	eIceRewardObjectReferenceType_FppTrans			= 4,
	eIceRewardObjectReferenceType_PlayMoneyTrans	= 5,
	eIceRewardObjectReferenceType_SportsFreeBetId	= 6,
	eIceRewardObjectReferenceType_BonusGameTicket	= 7,
	eIceRewardObjectReferenceType_MultiPurposeChest	= 8,
	eIceRewardObjectReferenceType_PredictorTicket	= 9,
	eIceRewardObjectReferenceType_UnlockChest		= 10,
	eIceRewardObjectReferenceType_StatCodeUserId	= 11,
	eIceRewardObjectReferenceType_VoidChest			= 12,
	eIceRewardObjectReferenceType_GiftCode			= 13,
};

enum eIceTaskParamFlags
{
	eIceTaskParamFlags_None									= 0,
	eIceTaskParamFlags_ManualAccept							= 1<<0, // if set on even one event parameter for a task - the event will not be used for auto-accept
	eIceTaskParamFlags_CompareWithState						= 1<<1, // comparison is done not on the content of value field but on the state accumulator value itself
	eIceTaskParamFlags_Obsolete								= 1<<2, // TODO: reuse once you run out of other flags
	eIceTaskParamFlags_CompareWithUserStateAcceptSnapshot	= 1<<3,
	eIceTaskParamFlags_SetValueFromVar						= 1<<4,
	eIceTaskParamFlags_CustomCompareSpecialInterface		= 1<<5,
};

enum eIceTaskEventFlowControl
{
	eIceTaskEventFlowControl_None					= 0,
	eIceTaskEventFlowControl_IgnoreOnFilterMatch	= 1,	// filter match on event instance must result in ignoring the event and not passing it to another filter instance
};

enum eHandFinishPosition
{
	eHandFinishPosition_PreFlop			= 1,
	eHandFinishPosition_SeenFlop		= 2,
	eHandFinishPosition_Turn			= 3,
	eHandFinishPosition_River			= 4,
	eHandFinishPosition_Showdown		= 5,
	eHandFinishPosition_Ocean			= 6, // PYR-49457 (Power-Up hands only)
	eHandFinishPosition_Sea				= 7, // PYR-49457 (Power-Up hands only)
};

enum eHandFinishResult
{
	eHandFinishResult_Lost	= 1,
	eHandFinishResult_Win	= 2,
	eHandFinishResult_Even	= 3,		// PYR-91140 (Unfold hands only)
};

enum eIceTaskUserFlags
{
	eIceTaskUserFlags_NewData			= 1<<0, // data available for user consideration
	eIceTaskUserFlags_LeaderBoard		= 1<<1,
	eIceTaskUserFlags_Achievement		= 1<<2,
	eIceTaskUserFlags_HiddenFromUser	= 1<<3,
	eIceTaskUserFlags_ScheduleExpire	= 1<<4, // expire task instance state for opt-in schedule task if a new interval has started
	eIceTaskUserFlags_DiscardDoneState	= 1<<5,
	eIceTaskUserFlags_UserResettableFlagsMask = 0x1
};

// PYR-40096
enum eIceFirstLTPointsType
{
	eIceFirstLTPointsType_Poker		= 1,
	eIceFirstLTPointsType_Duel		= 2,
	eIceFirstLTPointsType_Casino	= 3,
	eIceFirstLTPointsType_Sports	= 4,
};

// PYR-31472
enum eIceTrackCodeChangeType
{
	eIceTrackCodeChangeType_Assigned	= 1,
	eIceTrackCodeChangeType_Removed		= 2,
};

// PYR-47136
enum eIceDepositFlags8
{
	eIceDepositFlags8_IgnoreBySignals = 1 << 0,
};

// PYR-46864
enum eIceDynContentType
{
	eIceDynContentType_Task				= 1,
	eIceDynContentType_Reward			= 2,
	eIceDynContentType_Promo			= 3,
};

//////////////////////////////////////////////////////////////////////////

enum eIceEventType
{
	eIceEvent_Unknown					= 0,					 // *** Brand related comments for future brand support ***
	eIceEvent_RingHandPlayed			= 1,					 // brand by table connection
	eIceEvent_RegisterForDummyTourn		= 2,					 // brand by lobby connection unless via Admin 
	eIceEvent_PlayedInTourn				= 3,					 // by tournusers.regbrandid if not logged in. otherwise mtlobby connection
	eIceEvent_KickedTournPlayer			= 4,					 // by tournusers.regbrandid if not logged in. otherwise mtlobby connection
	eIceEvent_ReceivedTournAward		= 5,					 // by tournusers.regbrandid if not logged in. otherwise mtlobby connection
	eIceEvent_VppsEarned				= 6,					 // no brand. use transaction type (VppSource= as per PYR-42661)
    eIceEvent_RmDeposit					= 7,					 // from CashTransDataSimple
	eIceEvent_QuestCompleted			= 8,					 // brand of last event that changed state?
	eIceEvent_EnterStarCode				= 9,					 // lobbyetc or regulator connection brand
	eIceEvent_TicketUsed				= 10,					 // *** event not supported yet
	eIceEvent_NewUserAccount			= 11, // PYR-31590		 // whatever dbm gets from regulator
	eIceEvent_PrivilegeChange			= 12,					 // no brand. internal event.
	eIceEvent_FppChange					= 13,					 // no brand
	eIceEvent_QuestStarted				= 14, // PYR-36162		 // can find brand for manual opt-in. auto opt-in brand from event if available.
	eIceEvent_VipLevelChanged			= 15,					 // no brand
	eIceEvent_DuelPlay					= 16, // PYR-35915		 // duel only
	eIceEvent_DuelFinish				= 17,					 // duel only
	eIceEvent_DuelHand					= 18,					 // duel only
	eIceEvent_AccountChanged			= 19, // PYR-32027		 // no brand. internal event.
	eIceEvent_CasinoGameRound			= 20, // PYR-34344		 // from casino
	eIceEvent_Bonus						= 21, // PYR-35734		 // from DatabaseManager::issueBonus2( pBonusData->bonusData->brandsMask)
	eIceEvent_VipStepCompleted			= 22, // PYR-38213		 // lobby etc connection brand
	eIceEvent_SportsBetPlaced			= 23, // PYR-37407		 // being discussed
	eIceEvent_SportsBetSettled			= 24, // PYR-37407		 // being discussed
	eIceEvent_FinishedTourn				= 25, // PYR-33583		 // by tournusers.regbrandid if not logged in. otherwise mtlobby connection
	eIceEvent_FirstLTPoints				= 26, // PYR-40096		 // no brand. internal event.
	eIceEvent_UserTrackCodeChange		= 27, // PYR-31472		 // no brand. internal event.
	eIceEvent_QuestFailed				= 28, // PYR-31488		 // last event that changed state
	eIceEvent_CasinoBlackjackHand		= 29, // PYR-34347		 // from casino
	eIceEvent_CasinoBlackjackSeat		= 30, // PYR-34347		 // from casino
	eIceEvent_MGCompleted				= 31, // PYR-43429		 // from tableDbm
	eIceEvent_FreemiumPurchase			= 32, // PYR-39805		 // whatever dbm gets from regulator
	eIceEvent_CasinoRouletteSpin		= 33, // PYR-43854		 // from casino
	eIceEvent_CasinoRouletteBet			= 34, // PYR-43854		 // from casino
	eIceEvent_AccountValidation			= 35, // PYR-32798		 // no brand. internal event.
	eIceEvent_CasinoSlotSpin			= 36, // PYR-45971		 // from casino
	eIceEvent_CasinoSlotBonusPlay		= 37, // PYR-45971		 // from casino
	eIceEvent_ForceOptInBypassFilter	= 38,					 // internal event generated inside ice lobby. brand from connection *** not supported in the script ***
	eIceEvent_CppsEarned				= 39,
	eIceEvent_SppsEarned				= 40,
	eIceEvent_TournHand					= 41, // PYR-49457		 // by tournusers.regbrandid if not logged in. otherwise mtlobby connection
	eIceEvent_ChangeUserDefaultAccount	= 42,
	eIceEvent_MGPrizeAwarded			= 43, // PYR-52970		 // from tableDbm
	eIceEvent_CrmOfferAccepted			= 44, // PYR-54633		 // from CrmDBM
	eIceEvent_QuestTimingOut			= 45, // PYR-56270		 // internal event - similar to eIceEvent_QuestStarted
	eIceEvent_ChestEarn					= 46, // PYR-56058		 // from EtcDbm
	eIceEvent_DreamCatcherGameRound		= 47, // PYR-59835		 // from casino
	eIceEvent_PredictorTicketCredited	= 48, // PYR-63218		 // from predictor
	eIceEvent_LeaderBoardSeasonFinish	= 49, // PYR-62728		 // ice dbm master
	eIceEvent_RingGameBuyin				= 50, // PYR-66899	
	eIceEvent_CasinoTexasHoldemBonusHand= 51, // PYR-67812		 // from casino
	eIceEvent_CasinoTexasHoldemBonusBet = 52, // PYR-67812		 // from casino	
	eIceEvent_CasinoBaccaratHand		= 53, // PYR-66738		 // from casino
	eIceEvent_CasinoBaccaratBet			= 54, // PYR-66738		 // from casino	
	eIceEvent_RingGameCashOut			= 55, // PYR-70530	
	eIceEvent_TournRegister				= 56, // PYR-70533	
	eIceEvent_TournUnregister			= 57, // PYR-70533	
	eIceEvent_CasinoDragonTigerHand		= 58, // PYR-66224		 // from casino
	eIceEvent_CasinoDragonTigerBet		= 59, // PYR-66224		 // from casino	
	eIceEvent_MpChestIssued				= 60, // PYR-62775		 // from EtcDbm
	eIceEvent_InternalFlowControl		= 61, // PYR-85459		 // internal event used for auto-opt-in control and not exposed to script
	eIceEvent_CasinoCommunityJackpot	= 62, // PYR-82915		 // from casino	
	eIceEvent_VictoryTribesBonusTournFinish	= 63, // PYR-82915	 // from casino	
	eIceEvent_VictoryTribesSlotSpin		= 64, // PYR-82915		 // from casino	
	eIceEvent_CasinoRaceFinish			= 65, // PYR-84989		 // from casino	
	eIceEvent_QuestStateChange			= 66, // PYR-80269		 // internal ice lobby event for user 0 - always handled by master instance
	eIceEvent_Timer						= 67, // PYR-80269		 // internal ice lobby event for user 0 - always handled by master instance
	eIceEvent_PrefilterSectionNotEvent	= 68, // PYR-80269		 // not an actual event and task does not register for it but checks before other events it registered for
	eIceEvent_KYCDocApproved			= 69, // PYR-92702		 // from security
	eIceEvent_RewardPoints				= 70, // PYR-54691		 // from EtcDbm
	eIceEvent_GameDataAnalytics 		= 71, // PYR-96173		 // from CrmDbm
	eIceEvent_SportsBetCashout			= 72, // PYR-57429
	eIceEvent_Login						= 73, // PYR-107193
	eIceEvent_TwitchAccountLinked		= 74, // PYR-112222
	eIceEvent_PokerSideBetSettled		= 75, // PYR-106565
	eIceEvent_ProgressivePlusEntry		= 76, // PYR-125878
	eIceEvent_ProgressivePlusFinish		= 77, // PYR-125878
	eIceEvent_NewTicket					= 78, // PYR-124673
	eIceEvent_DealJackpotSettled		= 79, // PYR-124673
};


enum eIceEventNameSpaceType
{
	eIceEventNameSpaceType_Unknown		= 0,
	eIceEventNameSpaceType_Completion	= 1,
};

// *** IMPORTANT***
// when adding operator IDs which define actions that do not use state
// update IceTaskParamData::isOperatorIdMayUseState() 
//
enum eIceParamOperator
{
	// SumParam, SumVal, SumState
	// SaveParam, SaveVal, SaveState
	eIceEventOperator_Unknown		= 0, // meaning			: operation						: comparison [ optional ]

	eIceEventOperator_Save			= 1, // SaveParam		: stateDst =	event.param		: [ stateDst	  ? dbparam.values	]
	eIceEventOperator_Sum			= 2, // SumParam		: stateDst +=	event.param		: [ stateDst	  ? dbparam.values	]
	eIceEventOperator_Count			= 3, // SumVal			: stateDst +=	dbparam.valFrom	: [ stateDst	  ? dbparam.values	] // Event - translates to 1
	eIceEventOperator_Filter		= 4, // 				:								: event.param ? stateDst || eventParam ? dbparam.values
	eIceEventOperator_DoNotEval		= 5, // NOP				: -								: 
	eIceEventOperator_SaveState		= 6, // SaveState		: stateDst =	stateSrc		: [ stateDst	  ? dbparam.values	]
	eIceEventOperator_SumState		= 7, // SumState		: stateDst +=	stateSrc		: [ stateDst	  ? dbparam.values	]
	eIceEventOperator_SaveVal		= 8, // SaveVal			: stateDst =	dbParam.valFrom	: [ stateDst	  ? dbparam.values	]
	eIceEventOperator_ForwardEvent	= 9, // forwards event	: no task state change			: false
	eIceEventOperator_Timeout		= 10,// timeout task in	: no task state change			: false
	eIceEventOperator_SetVar		= 11,// set variable	: no change - transient variable: false
	eIceEventOperator_CheckValue	= 12,// check variable	: no change - transient variable: [ stateDst	  ? dbparam.values	]
	eIceEventOperator_ScheduleTimer	= 13,// scheduleTimer	: stateStr	+=	str				: false
	eIceEventOperator_ExpireQuestRef= 14,// scheduleTimer	: no change						: false
};

struct IceParamOperatorToString
{
	static const char* toString( eIceParamOperator val )
	{
		switch( val ) //-V719
		{
			case eIceEventOperator_Unknown	:	return "Unknown	"	;

			case eIceEventOperator_Save		:	return "Save"		; // SaveParam	// Success=(Save=param,state=dst_name,min=valDst)
			case eIceEventOperator_SaveState:	return "SaveState"	; // SaveState	// Success=(SaveState=src_state_name,state=dst_name,min=valDst)
			case eIceEventOperator_SaveVal	:	return "SaveVal"	; // SaveVal	// Success=(SaveVal=valSrc,state=dst_name,min=valDst)
																					//
			case eIceEventOperator_Sum		:	return "Sum"		; // SumParam	// Success=(Sum=param,state=dst_name,min=valDst)
			case eIceEventOperator_Count	:	return "Count"		; // SumVal		// Success=(Count=valSrc,state=dst_name,min=valDst)
			case eIceEventOperator_SumState	:	return "SumState"	; // SumState	// Success=(SumState=src_state_name,state=dst_name,min=valDst)
																					//
			case eIceEventOperator_Filter	:	return "Filter"		;				//
			case eIceEventOperator_DoNotEval:	return "DoNotEval"	;				//

			case eIceEventOperator_Timeout	:	return "Timeout"	;
			case eIceEventOperator_SetVar	:	return "SetVar"		;
			case eIceEventOperator_ForwardEvent:return "ForwardEvent";

			case eIceEventOperator_CheckValue:	return "CheckValue";
		}
		return "unknown operator id";
	}
};

enum eIceParamComparator
{
	eIceParamComparator_Unknown			= 0,
	eIceParamComparator_Less			= 1,
	eIceParamComparator_LessOrEqual		= 2,
	eIceParamComparator_Equal			= 3,
	eIceParamComparator_GreaterOrEqual	= 4,
	eIceParamComparator_Greater			= 5,
	eIceParamComparator_NotEqual		= 6,
};

enum eIceEventParamFlags
{
};

enum eIceEventParam_GameFlags8_Bits
{
	eIceEventParam_GameFlags8_Bits_IsPlayMoney	= 1<<0,
	eIceEventParam_GameFlags8_Bits_IsZoom		= 1<<1,
};

//PYR-84639: eIceEventParam_SeatPositionFlags8_Bits should be deprecated once we're sure no more ring hand events will be received using ePt_Flags8 as the type for the seat position
//Should be safe to deprecate this for 2019.Main.03 since eIceEventParam_SeatPositionFlags16_Bits was introduced in 2019.Main.02
enum eIceEventParam_SeatPositionFlags8_Bits_Deprecated
{
	eIceEventParam_SeatPositionFlags8_Bits_SmallBlind	= 1 << 0,
	eIceEventParam_SeatPositionFlags8_Bits_BigBlind		= 1 << 1,
	eIceEventParam_SeatPositionFlags8_Bits_Button		= 1 << 2,
};

enum eIceEventParam_SeatPositionFlags16_Bits
{
	eIceEventParam_SeatPositionFlags16_Bits_SmallBlind		= 1<<0,
	eIceEventParam_SeatPositionFlags16_Bits_BigBlind		= 1<<1,
	eIceEventParam_SeatPositionFlags16_Bits_Button			= 1<<2,
	eIceEventParam_SeatPositionFlags16_Bits_Button_Minus_1	= 1<<3,
	eIceEventParam_SeatPositionFlags16_Bits_Button_Minus_2	= 1<<4,
	eIceEventParam_SeatPositionFlags16_Bits_Button_Minus_3	= 1<<5,
	eIceEventParam_SeatPositionFlags16_Bits_Button_Minus_4	= 1<<6,
	eIceEventParam_SeatPositionFlags16_Bits_Button_Minus_5	= 1<<7,
	eIceEventParam_SeatPositionFlags16_Bits_Button_Minus_6	= 1<<8,
};

enum eIceEventParam_HandActionFlags8_Bits
{
	eIceEventParam_HandActionFlags8_Bits_Fold	= 1 << 0,
	eIceEventParam_HandActionFlags8_Bits_Check	= 1 << 1,
	eIceEventParam_HandActionFlags8_Bits_Call	= 1 << 2,
	eIceEventParam_HandActionFlags8_Bits_Bet	= 1 << 3,
	eIceEventParam_HandActionFlags8_Bits_Raise	= 1 << 4,
};

enum eIceEventParam_Tourn_Flags16_Bits
{
	eIceEventParam_Tourn_Flags16_Bits_SNG				= 1<<0,
	eIceEventParam_Tourn_Flags16_Bits_SAG				= 1<<1,
	eIceEventParam_Tourn_Flags16_Bits_Timed				= 1<<2,
	eIceEventParam_Tourn_Flags16_Bits_Reentry			= 1<<3,
	eIceEventParam_Tourn_Flags16_Bits_Flighted			= 1<<4,
	eIceEventParam_Tourn_Flags16_Bits_Rebuy				= 1<<5,
	eIceEventParam_Tourn_Flags16_Bits_Addon				= 1<<6,
	eIceEventParam_Tourn_Flags16_Bits_Bounty			= 1<<7,
	eIceEventParam_Tourn_Flags16_Bits_Shootout			= 1<<8,
	eIceEventParam_Tourn_Flags16_Bits_HiLo				= 1<<9,
	eIceEventParam_Tourn_Flags16_Bits_Powerup			= 1<<10,
	eIceEventParam_Tourn_Flags16_Bits_Satellite			= 1<<11,
	eIceEventParam_Tourn_Flags16_Bits_WinButton			= 1<<12,
	eIceEventParam_Tourn_Flags16_Bits_NStack			= 1<<13,
	eIceEventParam_Tourn_Flags16_Bits_Fifty50			= 1<<14,
	// use negative int value because flags are signed INT16 and this way does not cause casting errors
	// this is the last value in this enum
	// when/if we extend it to INT32 then we can start using 1<<15 syntax
	//
	eIceEventParam_Tourn_Flags16_Bits_ProgressPlus		= -32768 // 0x8000 mask
};

enum eIceEventParam_Tourn_PPP_Flags8_Bits		// PYR-123925
{
	eIceEventParam_Tourn_PPP_Flags8_Bits_None			= 0,
	eIceEventParam_Tourn_PPP_Flags8_Bits_Entry			= 1 << 0,
	eIceEventParam_Tourn_PPP_Flags8_Bits_MidJourney		= 1 << 1,
};

enum eIceEventTournResultFlags8_Bits
{
	eIceEventTournResultFlags_Win			= 1<<0,
	eIceEventTournResultFlags_FinalTable	= 1<<1, // PYR-33583
};

enum eIceEventParam_Tourn_WinType_Flags8_Bits		// PYR-33583
{
	eIceEventTournResultFlags_Cash				= 1<<0,
	eIceEventTournResultFlags_TCash				= 1<<1,
	eIceEventTournResultFlags_FPP				= 1<<2,
	eIceEventTournResultFlags_TournTicket		= 1<<3,
	eIceEventTournResultFlags_TournRegistration	= 1<<4,
	eIceEventTournResultFlags_None				= 1<<5,
};

enum eIceEventParam_RingHandFlags8_Bits
{
	eIceEventParam_RingHandFlags8_Bits_RakeTaken	= 1<<0,
	eIceEventParam_RingHandFlags8_Bits_SeatFinder	= 1<<1,
	eIceEventParam_RingHandFlags8_Bits_Cashout		= 1<<2,
	eIceEventParam_RingHandFlags8_Bits_HiLo			= 1<<3,
};

enum eIceEventParam_BlackjackFlags8_Bits
{
	eIceEventParam_BlackjackFlags8_Bits_PlayerBlackjack		= 1<<0,
	eIceEventParam_BlackjackFlags8_Bits_DealerBlackjack		= 1<<1,
	eIceEventParam_BlackjackFlags8_Bits_Insurance			= 1<<2,
	eIceEventParam_BlackjackFlags8_Bits_Surrender			= 1<<3,
	eIceEventParam_BlackjackFlags8_Bits_InsuranceOffered	= 1<<4,
	eIceEventParam_BlackjackFlags8_Bits_SurrenderOffered	= 1<<5,
};

enum eIceEventParam_PrivilegeFlags32_Bits
{
	eIceEventParam_PrivilegeFlags32_Bits_BoardParticipateOff = 1<<0,
};

// NOTE: if changed update DatabaseManager::getUserIceEventFlagForFppBundles accordinlgy
enum eIceEventParam_VppsFlags8_Bits
{
	eIceEventParam_VppsFlags8_FppBundleUser			= 1<<0,
	eIceEventParam_VppsFlags8_FppBundleAdminUpdate	= 1<<1,
	eIceEventParam_VppsFlags8_FppBundleVppAwarded	= 1<<2,
	eIceEventParam_VppsFlags8_FtpMigration			= 1<<3, // PYR-40124
	eIceEventParam_VppsFlags8_YearlyVpp				= 1<<4,
};

enum eIceEventParam_CppsFlags8_Bits
{
};
enum eIceEventParam_SppsFlags8_Bits
{
};

enum eIceEventParam_DuelPlayerFlags8_Bits	// PYR-35915
{
	eIceEventParam_DuelPlayerFlags8_Bits_First		= 1<<0,
	eIceEventParam_DuelPlayerFlags8_Bits_Friend		= 1<<1,
	eIceEventParam_DuelPlayerFlags8_Bits_Challenge	= 1<<2,
};

enum eIceDuelResult			// PYR-35915
{
	eIceDuelResult_Unknown					= 0,
	eIceDuelResult_Win						= 1,
	eIceDuelResult_Lost						= 2,
	eIceDuelResult_Tie						= 3,
};

//////////////////////////////////////////////////////////////////////
// Regular sport types
//////////////////////////////////////////////////////////////////////
#define	eIceEventParam_SportFlag64_Unknown				0
#define	eIceEventParam_SportFlag64_AmericanFootball		((INT64)1<<0 )
#define	eIceEventParam_SportFlag64_Archery				((INT64)1<<1 )
#define	eIceEventParam_SportFlag64_AussieRules			((INT64)1<<2 )
#define	eIceEventParam_SportFlag64_Badminton			((INT64)1<<3 )
#define	eIceEventParam_SportFlag64_Baseball				((INT64)1<<4 )
#define	eIceEventParam_SportFlag64_Basketball			((INT64)1<<5 )
#define	eIceEventParam_SportFlag64_BeachSoccer			((INT64)1<<6 )
#define	eIceEventParam_SportFlag64_BeachVolley			((INT64)1<<7 )
#define	eIceEventParam_SportFlag64_Boxing				((INT64)1<<8 )
#define	eIceEventParam_SportFlag64_Bowling				((INT64)1<<9 )
#define	eIceEventParam_SportFlag64_Canoeing				((INT64)1<<10)
#define	eIceEventParam_SportFlag64_Chess				((INT64)1<<11)
#define	eIceEventParam_SportFlag64_Cricket				((INT64)1<<12)
#define	eIceEventParam_SportFlag64_Curling				((INT64)1<<13)
#define	eIceEventParam_SportFlag64_Cycling				((INT64)1<<14)
#define	eIceEventParam_SportFlag64_Darts				((INT64)1<<15)
#define	eIceEventParam_SportFlag64_Esports				((INT64)1<<16)
#define	eIceEventParam_SportFlag64_F1					((INT64)1<<17)
#define	eIceEventParam_SportFlag64_Fencing				((INT64)1<<18)
#define	eIceEventParam_SportFlag64_FieldHockey			((INT64)1<<19)
#define	eIceEventParam_SportFlag64_FigureSkating		((INT64)1<<20)
#define	eIceEventParam_SportFlag64_Floorball			((INT64)1<<21)
#define	eIceEventParam_SportFlag64_Football				((INT64)1<<22)
#define	eIceEventParam_SportFlag64_Futsal				((INT64)1<<23)
#define	eIceEventParam_SportFlag64_GaelicSports			((INT64)1<<24)
#define	eIceEventParam_SportFlag64_Golf					((INT64)1<<25)
#define	eIceEventParam_SportFlag64_Gymnastics			((INT64)1<<26)
#define	eIceEventParam_SportFlag64_Handball				((INT64)1<<27)
#define	eIceEventParam_SportFlag64_HorseRacing			((INT64)1<<28)
#define	eIceEventParam_SportFlag64_IceHockey			((INT64)1<<29)
#define	eIceEventParam_SportFlag64_Judo					((INT64)1<<30)
#define	eIceEventParam_SportFlag64_Lacrosse				((INT64)1<<31)
#define	eIceEventParam_SportFlag64_MotorBikes			((INT64)1<<32)
#define	eIceEventParam_SportFlag64_Netball				((INT64)1<<33)
#define	eIceEventParam_SportFlag64_Pesapallo			((INT64)1<<34)
#define	eIceEventParam_SportFlag64_Poker				((INT64)1<<35)
#define	eIceEventParam_SportFlag64_Pool					((INT64)1<<36)
#define	eIceEventParam_SportFlag64_Rowing				((INT64)1<<37)
#define	eIceEventParam_SportFlag64_Rugby				((INT64)1<<38)
#define	eIceEventParam_SportFlag64_RugbyLeague			((INT64)1<<39)
#define	eIceEventParam_SportFlag64_RugbyUnion			((INT64)1<<40)
#define	eIceEventParam_SportFlag64_Sailing				((INT64)1<<41)
#define	eIceEventParam_SportFlag64_Snooker				((INT64)1<<42)
#define	eIceEventParam_SportFlag64_Softball				((INT64)1<<43)
#define	eIceEventParam_SportFlag64_Squash				((INT64)1<<44)
#define	eIceEventParam_SportFlag64_Swimming				((INT64)1<<45)
#define	eIceEventParam_SportFlag64_TableTennis			((INT64)1<<46)
#define	eIceEventParam_SportFlag64_Taekwando			((INT64)1<<47)
#define	eIceEventParam_SportFlag64_Tennis				((INT64)1<<48)
#define	eIceEventParam_SportFlag64_Volleyball			((INT64)1<<49)
#define	eIceEventParam_SportFlag64_Waterpolo			((INT64)1<<50)
#define	eIceEventParam_SportFlag64_Weightlifting		((INT64)1<<51)
#define	eIceEventParam_SportFlag64_Wrestling			((INT64)1<<52)
#define	eIceEventParam_SportFlag64_Bowls				((INT64)1<<53)
// reserved sport IDs which can be used in script if we need to urgently add a sport
// a parser will currently recognize them as 'Reserved1/2/3' but later parser can be
// modified to use an actual sports name
//
#define	eIceEventParam_SportFlag64_Reserved1			((INT64)1<<54)
#define	eIceEventParam_SportFlag64_Reserved2			((INT64)1<<55)
#define	eIceEventParam_SportFlag64_Reserved3			((INT64)1<<56)
#define	eIceEventParam_SportFlag64_Reserved4			((INT64)1<<57)
#define	eIceEventParam_SportFlag64_Reserved5			((INT64)1<<58)
#define	eIceEventParam_SportFlag64_Reserved6			((INT64)1<<59)
#define	eIceEventParam_SportFlag64_Reserved7			((INT64)1<<60)
#define	eIceEventParam_SportFlag64_Reserved8			((INT64)1<<61)
#define	eIceEventParam_SportFlag64_Reserved9			((INT64)1<<62)

#define eIceEventParam_SportFlag64_AllKnownSports		0x7FFFFFFFFFFFFFFF
// TODO: decide if we need multi-sport at all
#define	eIceEventParam_SportFlag64_MultiSport			((INT64)1<<63)


//////////////////////////////////////////////////////////////////////////
#include "PBitmask.h"

struct SportsFlagsLong
{
	const PBitmask zero = PBitmask::fromUINT64(0); // same as unknown but I don't want to use them interchangeably

	const PBitmask eIceEventParam_SportFlagLong_Unknown				= PBitmask::fromINT64( eIceEventParam_SportFlag64_Unknown			);
	const PBitmask eIceEventParam_SportFlagLong_AmericanFootball	= PBitmask::fromINT64( eIceEventParam_SportFlag64_AmericanFootball	);
	const PBitmask eIceEventParam_SportFlagLong_Archery				= PBitmask::fromINT64( eIceEventParam_SportFlag64_Archery			);
	const PBitmask eIceEventParam_SportFlagLong_AussieRules			= PBitmask::fromINT64( eIceEventParam_SportFlag64_AussieRules		);
	const PBitmask eIceEventParam_SportFlagLong_Badminton			= PBitmask::fromINT64( eIceEventParam_SportFlag64_Badminton			);
	const PBitmask eIceEventParam_SportFlagLong_Baseball			= PBitmask::fromINT64( eIceEventParam_SportFlag64_Baseball			);
	const PBitmask eIceEventParam_SportFlagLong_Basketball			= PBitmask::fromINT64( eIceEventParam_SportFlag64_Basketball		);
	const PBitmask eIceEventParam_SportFlagLong_BeachSoccer			= PBitmask::fromINT64( eIceEventParam_SportFlag64_BeachSoccer		);
	const PBitmask eIceEventParam_SportFlagLong_BeachVolley			= PBitmask::fromINT64( eIceEventParam_SportFlag64_BeachVolley		);
	const PBitmask eIceEventParam_SportFlagLong_Boxing				= PBitmask::fromINT64( eIceEventParam_SportFlag64_Boxing			);
	const PBitmask eIceEventParam_SportFlagLong_Bowling				= PBitmask::fromINT64( eIceEventParam_SportFlag64_Bowling			);
	const PBitmask eIceEventParam_SportFlagLong_Canoeing			= PBitmask::fromINT64( eIceEventParam_SportFlag64_Canoeing			);
	const PBitmask eIceEventParam_SportFlagLong_Chess				= PBitmask::fromINT64( eIceEventParam_SportFlag64_Chess				);
	const PBitmask eIceEventParam_SportFlagLong_Cricket				= PBitmask::fromINT64( eIceEventParam_SportFlag64_Cricket			);
	const PBitmask eIceEventParam_SportFlagLong_Curling				= PBitmask::fromINT64( eIceEventParam_SportFlag64_Curling			);
	const PBitmask eIceEventParam_SportFlagLong_Cycling				= PBitmask::fromINT64( eIceEventParam_SportFlag64_Cycling			);
	const PBitmask eIceEventParam_SportFlagLong_Darts				= PBitmask::fromINT64( eIceEventParam_SportFlag64_Darts				);
	const PBitmask eIceEventParam_SportFlagLong_Esports				= PBitmask::fromINT64( eIceEventParam_SportFlag64_Esports			);
	const PBitmask eIceEventParam_SportFlagLong_F1					= PBitmask::fromINT64( eIceEventParam_SportFlag64_F1				);
	const PBitmask eIceEventParam_SportFlagLong_Fencing				= PBitmask::fromINT64( eIceEventParam_SportFlag64_Fencing			);
	const PBitmask eIceEventParam_SportFlagLong_FieldHockey			= PBitmask::fromINT64( eIceEventParam_SportFlag64_FieldHockey		);
	const PBitmask eIceEventParam_SportFlagLong_FigureSkating		= PBitmask::fromINT64( eIceEventParam_SportFlag64_FigureSkating		);
	const PBitmask eIceEventParam_SportFlagLong_Floorball			= PBitmask::fromINT64( eIceEventParam_SportFlag64_Floorball			);
	const PBitmask eIceEventParam_SportFlagLong_Football			= PBitmask::fromINT64( eIceEventParam_SportFlag64_Football			);
	const PBitmask eIceEventParam_SportFlagLong_Futsal				= PBitmask::fromINT64( eIceEventParam_SportFlag64_Futsal			);
	const PBitmask eIceEventParam_SportFlagLong_GaelicSports		= PBitmask::fromINT64( eIceEventParam_SportFlag64_GaelicSports		);
	const PBitmask eIceEventParam_SportFlagLong_Golf				= PBitmask::fromINT64( eIceEventParam_SportFlag64_Golf				);
	const PBitmask eIceEventParam_SportFlagLong_Gymnastics			= PBitmask::fromINT64( eIceEventParam_SportFlag64_Gymnastics		);
	const PBitmask eIceEventParam_SportFlagLong_Handball			= PBitmask::fromINT64( eIceEventParam_SportFlag64_Handball			);
	const PBitmask eIceEventParam_SportFlagLong_HorseRacing			= PBitmask::fromINT64( eIceEventParam_SportFlag64_HorseRacing		);
	const PBitmask eIceEventParam_SportFlagLong_IceHockey			= PBitmask::fromINT64( eIceEventParam_SportFlag64_IceHockey			);
	const PBitmask eIceEventParam_SportFlagLong_Judo				= PBitmask::fromINT64( eIceEventParam_SportFlag64_Judo				);
	const PBitmask eIceEventParam_SportFlagLong_Lacrosse			= PBitmask::fromINT64( eIceEventParam_SportFlag64_Lacrosse			);
	const PBitmask eIceEventParam_SportFlagLong_MotorBikes			= PBitmask::fromINT64( eIceEventParam_SportFlag64_MotorBikes		);
	const PBitmask eIceEventParam_SportFlagLong_Netball				= PBitmask::fromINT64( eIceEventParam_SportFlag64_Netball			);
	const PBitmask eIceEventParam_SportFlagLong_Pesapallo			= PBitmask::fromINT64( eIceEventParam_SportFlag64_Pesapallo			);
	const PBitmask eIceEventParam_SportFlagLong_Poker				= PBitmask::fromINT64( eIceEventParam_SportFlag64_Poker				);
	const PBitmask eIceEventParam_SportFlagLong_Pool				= PBitmask::fromINT64( eIceEventParam_SportFlag64_Pool				);
	const PBitmask eIceEventParam_SportFlagLong_Rowing				= PBitmask::fromINT64( eIceEventParam_SportFlag64_Rowing			);
	const PBitmask eIceEventParam_SportFlagLong_Rugby				= PBitmask::fromINT64( eIceEventParam_SportFlag64_Rugby				);
	const PBitmask eIceEventParam_SportFlagLong_RugbyLeague			= PBitmask::fromINT64( eIceEventParam_SportFlag64_RugbyLeague		);
	const PBitmask eIceEventParam_SportFlagLong_RugbyUnion			= PBitmask::fromINT64( eIceEventParam_SportFlag64_RugbyUnion		);
	const PBitmask eIceEventParam_SportFlagLong_Sailing				= PBitmask::fromINT64( eIceEventParam_SportFlag64_Sailing			);
	const PBitmask eIceEventParam_SportFlagLong_Snooker				= PBitmask::fromINT64( eIceEventParam_SportFlag64_Snooker			);
	const PBitmask eIceEventParam_SportFlagLong_Softball			= PBitmask::fromINT64( eIceEventParam_SportFlag64_Softball			);
	const PBitmask eIceEventParam_SportFlagLong_Squash				= PBitmask::fromINT64( eIceEventParam_SportFlag64_Squash			);
	const PBitmask eIceEventParam_SportFlagLong_Swimming			= PBitmask::fromINT64( eIceEventParam_SportFlag64_Swimming			);
	const PBitmask eIceEventParam_SportFlagLong_TableTennis			= PBitmask::fromINT64( eIceEventParam_SportFlag64_TableTennis		);
	const PBitmask eIceEventParam_SportFlagLong_Taekwando			= PBitmask::fromINT64( eIceEventParam_SportFlag64_Taekwando			);
	const PBitmask eIceEventParam_SportFlagLong_Tennis				= PBitmask::fromINT64( eIceEventParam_SportFlag64_Tennis			);
	const PBitmask eIceEventParam_SportFlagLong_Volleyball			= PBitmask::fromINT64( eIceEventParam_SportFlag64_Volleyball		);
	const PBitmask eIceEventParam_SportFlagLong_Waterpolo			= PBitmask::fromINT64( eIceEventParam_SportFlag64_Waterpolo			);
	const PBitmask eIceEventParam_SportFlagLong_Weightlifting		= PBitmask::fromINT64( eIceEventParam_SportFlag64_Weightlifting		);
	const PBitmask eIceEventParam_SportFlagLong_Wrestling			= PBitmask::fromINT64( eIceEventParam_SportFlag64_Wrestling			);
	const PBitmask eIceEventParam_SportFlagLong_Bowls				= PBitmask::fromINT64( eIceEventParam_SportFlag64_Bowls				);

	const PBitmask eIceEventParam_SportFlagLong_Reserved1			= PBitmask::fromINT64( eIceEventParam_SportFlag64_Reserved1			);
	const PBitmask eIceEventParam_SportFlagLong_Reserved2			= PBitmask::fromINT64( eIceEventParam_SportFlag64_Reserved2			);
	const PBitmask eIceEventParam_SportFlagLong_Reserved3			= PBitmask::fromINT64( eIceEventParam_SportFlag64_Reserved3			);
	const PBitmask eIceEventParam_SportFlagLong_Reserved4			= PBitmask::fromINT64( eIceEventParam_SportFlag64_Reserved4			);
	const PBitmask eIceEventParam_SportFlagLong_Reserved5			= PBitmask::fromINT64( eIceEventParam_SportFlag64_Reserved5			);
	const PBitmask eIceEventParam_SportFlagLong_Reserved6			= PBitmask::fromINT64( eIceEventParam_SportFlag64_Reserved6			);
	const PBitmask eIceEventParam_SportFlagLong_Reserved7			= PBitmask::fromINT64( eIceEventParam_SportFlag64_Reserved7			);
	const PBitmask eIceEventParam_SportFlagLong_Reserved8			= PBitmask::fromINT64( eIceEventParam_SportFlag64_Reserved8			);
	const PBitmask eIceEventParam_SportFlagLong_Reserved9			= PBitmask::fromINT64( eIceEventParam_SportFlag64_Reserved9			);

	const PBitmask eIceEventParam_SportFlagLong_AllKnownSports		= PBitmask::fromINT64( eIceEventParam_SportFlag64_AllKnownSports	);
	const PBitmask eIceEventParam_SportFlagLong_MultiSport			= PBitmask::fromINT64( eIceEventParam_SportFlag64_MultiSport		);

	const PBitmask nextBunchOfBits = PBitmask::fromUINT64( 0x8000000000000000 ) << 1;

	const PBitmask eIceEventParam_SportFlagLong_AlpineSkiing			= nextBunchOfBits << 0;
	const PBitmask eIceEventParam_SportFlagLong_Athletics				= nextBunchOfBits << 1;
	const PBitmask eIceEventParam_SportFlagLong_Biathlon				= nextBunchOfBits << 2;
	const PBitmask eIceEventParam_SportFlagLong_Bobsleigh				= nextBunchOfBits << 3;
	const PBitmask eIceEventParam_SportFlagLong_CrossCountrySkiing		= nextBunchOfBits << 4;
	const PBitmask eIceEventParam_SportFlagLong_Dota2					= nextBunchOfBits << 5;
	const PBitmask eIceEventParam_SportFlagLong_FreestyleSkiing			= nextBunchOfBits << 6;
	const PBitmask eIceEventParam_SportFlagLong_Luge					= nextBunchOfBits << 7;
	const PBitmask eIceEventParam_SportFlagLong_MotorSports				= nextBunchOfBits << 8;
	const PBitmask eIceEventParam_SportFlagLong_Nascar					= nextBunchOfBits << 9;
	const PBitmask eIceEventParam_SportFlagLong_NordicCombined			= nextBunchOfBits << 10;
	const PBitmask eIceEventParam_SportFlagLong_Olympics				= nextBunchOfBits << 11;
	const PBitmask eIceEventParam_SportFlagLong_Rally					= nextBunchOfBits << 12;
	const PBitmask eIceEventParam_SportFlagLong_ShortTrackSpeedSkating	= nextBunchOfBits << 13;
	const PBitmask eIceEventParam_SportFlagLong_Skeleton				= nextBunchOfBits << 14;
	const PBitmask eIceEventParam_SportFlagLong_SkiJumping				= nextBunchOfBits << 15;
	const PBitmask eIceEventParam_SportFlagLong_Snowboard				= nextBunchOfBits << 16;
	const PBitmask eIceEventParam_SportFlagLong_Soccer					= nextBunchOfBits << 17;
	const PBitmask eIceEventParam_SportFlagLong_SpecialSoccer			= nextBunchOfBits << 18;
	const PBitmask eIceEventParam_SportFlagLong_Specials				= nextBunchOfBits << 19;
	const PBitmask eIceEventParam_SportFlagLong_SpeedSkating			= nextBunchOfBits << 20;
	const PBitmask eIceEventParam_SportFlagLong_Speedway				= nextBunchOfBits << 21;
};


//////////////////////////////////////////////////////////////////////
// End of Regular sport types
//////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////
// Virtual sport types
//////////////////////////////////////////////////////////////////////
#define	eIceEventParam_VirtualSportFlag64_Unknown			0
#define	eIceEventParam_VirtualSportFlag64_Basketball		((INT64)1<<0 )
#define	eIceEventParam_VirtualSportFlag64_Boxing			((INT64)1<<1 )
#define	eIceEventParam_VirtualSportFlag64_Cricket			((INT64)1<<2)
#define	eIceEventParam_VirtualSportFlag64_Cycling			((INT64)1<<3)
#define	eIceEventParam_VirtualSportFlag64_Darts				((INT64)1<<4)
#define	eIceEventParam_VirtualSportFlag64_Dogs				((INT64)1<<5)
#define	eIceEventParam_VirtualSportFlag64_F1				((INT64)1<<6)
#define	eIceEventParam_VirtualSportFlag64_Football			((INT64)1<<7)
#define	eIceEventParam_VirtualSportFlag64_Horses			((INT64)1<<8)
#define	eIceEventParam_VirtualSportFlag64_MatchDay			((INT64)1<<9)
#define	eIceEventParam_VirtualSportFlag64_Numbers			((INT64)1<<10)
#define	eIceEventParam_VirtualSportFlag64_RushFootball		((INT64)1<<11)
#define	eIceEventParam_VirtualSportFlag64_Speedway			((INT64)1<<12)
#define	eIceEventParam_VirtualSportFlag64_Tennis			((INT64)1<<13)
#define	eIceEventParam_VirtualSportFlag64_Trotting			((INT64)1<<14)

#define eIceEventParam_VirtualSportFlag64_AllKnownSports	0x7FFFFFFFFFFFFFFF

#define	eIceEventParam_VirtualSportFlag64_MultiSport		((INT64)1<<63)
//////////////////////////////////////////////////////////////////////
// End of virtual sport types
//////////////////////////////////////////////////////////////////////

enum eIceEventParam_SportBetFlag8
{
	// bet type
	eIceEventParam_SportBetFlag8_BetType_Single		= 1<<0,
	eIceEventParam_SportBetFlag8_BetType_Multiple	= 1<<1,
	eIceEventParam_SportBetFlag8_BetType_System		= 1<<2,
};

enum eIceEventParam_SportBetFlag8_Group
{
	eIceEventParam_SportBetFlag8_Group_BuildABet	= 1<<0,
};

enum eIceEventParam_SportBetResultFlag8
{
	eIceEventParam_SportBetResultFlag8_Win	= 1<<0,
	eIceEventParam_SportBetResultFlag8_Loss	= 1<<1,
	eIceEventParam_SportBetResultFlag8_Push	= 1<<2,
};

enum eIceSportsBetFundSource
{
	eIceSportsBetFundSource_Unknown			= 0,
	eIceSportsBetFundSource_Cash			= 1,
	eIceSportsBetFundSource_FreeBet			= 2,
	eIceSportsBetFundSource_Mixed			= 3,
};

enum eIceSportsBetComposition
{
	eIceSportsBetComposition_Regular	= 0,
	eIceSportsBetComposition_Virtual	= 1,
	eIceSportsBetComposition_Mixed		= 2,
};

enum eIceCasinoBetFundSource
{
	eIceCasinoBetFundSource_Unknown			= 0,
	eIceCasinoBetFundSource_Cash			= 1,
	eIceCasinoBetFundSource_FreeSpins		= 2,
	eIceCasinoBetFundSource_InstantBonus	= 3,
	eIceCasinoBetFundSource_Mixed			= 4,
};

enum eIceCasinoRacePrizeType
{
	eIceCasinoRacePrizeType_Unknown			= 0,
	eIceCasinoRacePrizeType_RealMoney		= 1,
	eIceCasinoRacePrizeType_Roll			= 2,
};

enum eIceMiniGameResult
{
	eIceMiniGameResult_Unknown				= 0,
	eIceMiniGameResult_Win					= 1,
	eIceMiniGameResult_Fail					= 2,
	eIceMiniGameResult_Forfeit				= 3,
	eIceMiniGameResult_Abandon				= 4,
};

enum eIceEventParam_RouletteBetTypeFlags16_Bits
{
	eIceEventParam_RouletteBetTypeFlags16_Bits_StraightBet	= 1<<0,
	eIceEventParam_RouletteBetTypeFlags16_Bits_SplitBet		= 1<<1,
	eIceEventParam_RouletteBetTypeFlags16_Bits_CornerBet	= 1<<2,
	eIceEventParam_RouletteBetTypeFlags16_Bits_StreetBet	= 1<<3,
	eIceEventParam_RouletteBetTypeFlags16_Bits_LineBet		= 1<<4,
	eIceEventParam_RouletteBetTypeFlags16_Bits_RedBlackBet	= 1<<5,
	eIceEventParam_RouletteBetTypeFlags16_Bits_OddEvenBet	= 1<<6,
	eIceEventParam_RouletteBetTypeFlags16_Bits_HighLowBet	= 1<<7,
	eIceEventParam_RouletteBetTypeFlags16_Bits_DozensBet	= 1<<8,
	eIceEventParam_RouletteBetTypeFlags16_Bits_ColumnsBet	= 1<<9,
	eIceEventParam_RouletteBetTypeFlags16_Bits_FiveBet		= 1<<10,
};

enum eIceEventParam_AccountValidationTypeFlags16_Bits
{
	eIceEventParam_AccountValidationTypeFlags16_Bits_EmailValidated		= 1<<0,
	eIceEventParam_AccountValidationTypeFlags16_Bits_RealMoneyOK		= 1<<1,
	eIceEventParam_AccountValidationTypeFlags16_Bits_Authenticated		= 1<<2,
	eIceEventParam_AccountValidationTypeFlags16_Bits_AgeVerified		= 1<<3,
	eIceEventParam_AccountValidationTypeFlags16_Bits_RMPokerPlayer		= 1<<4,
	eIceEventParam_AccountValidationTypeFlags16_Bits_RMCasinoPlayer		= 1<<5,
	eIceEventParam_AccountValidationTypeFlags16_Bits_RMSportsPlayer		= 1<<6,
	eIceEventParam_AccountValidationTypeFlags16_Bits_StarsRewardsOptIn	= 1<<7,
	eIceEventParam_AccountValidationTypeFlags16_Bits_FraudulentAccount	= 1<<8,		// PYR-71368
	eIceEventParam_AccountValidationTypeFlags16_Bits_DoNotPayBonus		= 1<<9,		// PYR-74170
	eIceEventParam_AccountValidationTypeFlags16_Bits_NoSportsBonus		= 1<<10,	// PYR-74170
	eIceEventParam_AccountValidationTypeFlags16_Bits_NoCasinoPlay		= 1<<11,	// PYR-74170
	eIceEventParam_AccountValidationTypeFlags16_Bits_NoSportsPlay		= 1<<12,	// PYR-74170
};

enum eIceSlotSymbol	
{
	eIceSlotSymbol_Unknown				= 0,
	eIceSlotSymbol_1					= 1,
	eIceSlotSymbol_2					= 2,
	eIceSlotSymbol_3					= 3,
	eIceSlotSymbol_4					= 4,
	eIceSlotSymbol_5					= 5,
	eIceSlotSymbol_6					= 6,
	eIceSlotSymbol_7					= 7,
	eIceSlotSymbol_8					= 8,
	eIceSlotSymbol_9					= 9,
	eIceSlotSymbol_10					= 10,
	eIceSlotSymbol_11					= 11,
	eIceSlotSymbol_12					= 12,
	eIceSlotSymbol_13					= 13,
	eIceSlotSymbol_14					= 14,
	eIceSlotSymbol_15					= 15,
};

enum eIceSlotBonusGameType	
{
	eIceSlotBonusGameType_Unknown		= 0,
	eIceSlotBonusGameType_Submarine		= 1,
	eIceSlotBonusGameType_Card			= 2,
};

enum eIceTournSpinGoType
{
	eIceTournSpinGoType_Unknown			= 0,
	eIceTournSpinGoType_Regular			= 1,
	eIceTournSpinGoType_Max				= 2,
	eIceTournSpinGoType_Flash			= 3,
};

enum eIceEventParam_TexasHoldemBonusBetTypeFlags8_Bits
{
	eIceEventParam_TexasHoldemBonusBetTypeFlags8_Bits_Ante		= 1 << 0,
	eIceEventParam_TexasHoldemBonusBetTypeFlags8_Bits_Bonus		= 1 << 1,
	eIceEventParam_TexasHoldemBonusBetTypeFlags8_Bits_Flop		= 1 << 2,
	eIceEventParam_TexasHoldemBonusBetTypeFlags8_Bits_Turn		= 1 << 3,
	eIceEventParam_TexasHoldemBonusBetTypeFlags8_Bits_River		= 1 << 4,
	eIceEventParam_TexasHoldemBonusBetTypeFlags8_Bits_Jackpot	= 1 << 5,
};

enum eIceEventParam_BaccaratBetTypeFlags16_Bits
{
	eIceEventParam_BaccaratBetTypeFlags16_Bits_Player		= 1 << 0,
	eIceEventParam_BaccaratBetTypeFlags16_Bits_Banker		= 1 << 1,
	eIceEventParam_BaccaratBetTypeFlags16_Bits_Tie			= 1 << 2,
	eIceEventParam_BaccaratBetTypeFlags16_Bits_BankerPair	= 1 << 3,
	eIceEventParam_BaccaratBetTypeFlags16_Bits_PlayerPair	= 1 << 4,
	eIceEventParam_BaccaratBetTypeFlags16_Bits_PerfectPair	= 1 << 5,
	eIceEventParam_BaccaratBetTypeFlags16_Bits_EitherPair	= 1 << 6,
	eIceEventParam_BaccaratBetTypeFlags16_Bits_PlayerBonus	= 1 << 7,
	eIceEventParam_BaccaratBetTypeFlags16_Bits_BankerBonus	= 1 << 8,
};

enum eIceEventParam_DragonTigerBetTypeFlags8_Bits
{
	eIceEventParam_DragonTigerBetTypeFlags8_Bits_Dragon		= 1 << 0,
	eIceEventParam_DragonTigerBetTypeFlags8_Bits_Tiger		= 1 << 1,
	eIceEventParam_DragonTigerBetTypeFlags8_Bits_Tie		= 1 << 2,
	eIceEventParam_DragonTigerBetTypeFlags8_Bits_SuitedTie	= 1 << 3,
};

enum eIceJackpotWinType
{
	eIceJackpotWinType_Main		= 0,
	eIceJackpotWinType_Share	= 1,
};

enum eIceJackpotType
{
	eIceJackpotType_Extra		= 0,
	eIceJackpotType_Super		= 1,
	eIceJackpotType_Mega		= 2,
};

enum eIceVictoryTribesTribeSelection
{
	eIceVictoryTribesTribeSelection_Barbarian	= 0,
	eIceVictoryTribesTribeSelection_Druid		= 1,
	eIceVictoryTribesTribeSelection_Mongol		= 2,
	eIceVictoryTribesTribeSelection_Viking		= 3,
};

enum eIceVictoryTribesBonusTournType
{
	eIceVictoryTribesBonusTournType_Global		= 0,
	eIceVictoryTribesBonusTournType_HeadsUp		= 1,
};

enum eIceChestExchangeType
{
	eIceChestExchangeType_None					= 0,
	eIceChestExchangeType_Auto					= 1,
	eIceChestExchangeType_Manual				= 2,
};

enum eIceKYCDocType
{
	eIceKYCDocType_Unknown							= 0,
	eIceKYCDocType_AgeVerified						= 1,
	eIceKYCDocType_Authenticated					= 2,
	eIceKYCDocType_MobilePhone						= 3,
	eIceKYCDocType_Form								= 4,
	eIceKYCDocType_ID								= 5,
	eIceKYCDocType_Address							= 6,
	eIceKYCDocType_Finance							= 7,
	eIceKYCDocType_Legal							= 8,
	eIceKYCDocType_Other							= 9,
};

enum eIceKYCDocSubCategory
{
	eIceKYCDocSubCategory_FormSourceOfWealth		= 0,
	eIceKYCDocSubCategory_FormUncategorised			= 1,
	eIceKYCDocSubCategory_FormPSVerificationForm	= 2,
	eIceKYCDocSubCategory_FormOther					= 3,
	eIceKYCDocSubCategory_IDIndianID				= 4,
	eIceKYCDocSubCategory_IDUncategorised			= 5,
	eIceKYCDocSubCategory_IDPassport				= 6,
	eIceKYCDocSubCategory_IDDriverLicense			= 7,
	eIceKYCDocSubCategory_IDCountryCard				= 8,
	eIceKYCDocSubCategory_IDBirthCertificate		= 9,
	eIceKYCDocSubCategory_IDOther					= 10,
	eIceKYCDocSubCategory_IDVerifiedInPerson		= 11,
	eIceKYCDocSubCategory_IDDupe					= 12,
	eIceKYCDocSubCategory_IDItalianID				= 13,
	eIceKYCDocSubCategory_AddressUncategorised		= 14,
	eIceKYCDocSubCategory_AddressUtilityBill		= 15,
	eIceKYCDocSubCategory_AddressOther				= 16,
	eIceKYCDocSubCategory_AddressDupe				= 17,
	eIceKYCDocSubCategory_FinanceCZPaymentMethod	= 18,
	eIceKYCDocSubCategory_FinanceUncategorised		= 19,
	eIceKYCDocSubCategory_FinanceBankStatement		= 20,
	eIceKYCDocSubCategory_FinanceCreditCard			= 21,
	eIceKYCDocSubCategory_FinanceOther				= 22,
	eIceKYCDocSubCategory_FinanceDupe				= 23,
	eIceKYCDocSubCategory_FinanceUnmaskedPAN		= 24,
	eIceKYCDocSubCategory_FinanceELVMandate			= 25,
	eIceKYCDocSubCategory_LegalUncategorised		= 26,
	eIceKYCDocSubCategory_LegalItalianContract		= 27,
	eIceKYCDocSubCategory_LegalOther				= 28,
	eIceKYCDocSubCategory_OtherPortugueseID			= 29,
	eIceKYCDocSubCategory_OtherRomanianID			= 30,
	eIceKYCDocSubCategory_OtherCZFace2Face			= 31,
	eIceKYCDocSubCategory_OtherUncategorised		= 32,
	eIceKYCDocSubCategory_OtherItalianID			= 33,
	eIceKYCDocSubCategory_OtherEstonianID			= 34,
	eIceKYCDocSubCategory_OtherSpanishID			= 35,
	eIceKYCDocSubCategory_OtherDanishID				= 36,
	eIceKYCDocSubCategory_OtherSpanishUB			= 37,
};

enum eIceRewardPointSource
{
	eIceRewardPointSource_Unknown		= 0,
	eIceRewardPointSource_Poker			= 1,
	eIceRewardPointSource_Sports		= 2,
	eIceRewardPointSource_Casino		= 3,
	eIceRewardPointSource_StarsRewards	= 4,
};


enum eIceEventParamIds
{
	eIceEventParam_Unknown					= 0,	//
	eIceEventParam_EventType				= 1,	//	INT16			eIceEventType
	eIceEventParam_Platform					= 2,	//	INT32			Any, Desktop, Mobile
	eIceEventParam_RingGameType				= 3,	//	INT8			Any, Holdem, Omaha, Stud, Other
	eIceEventParam_Structure				= 4,	//	INT8			Any, NL/PL/L
	eIceEventParam_GameFlags8				= 5,	//	Flags8			see eIceEventParam_GameFlags8_Bits
	eIceEventParam_dbmId					= 6,	//	INT32			
	eIceEventParam_TableId					= 7,	//	INT64
	eIceEventParam_PlayersPerTable			= 8,	//	INT8			between 2 and 10 inclusively
	eIceEventParam_PrivilegeFlags32			= 9,	//	Flags32			see eIceEventParam_PrivilegeFlags32_Bits
	eIceEventParam_CountryBefore			= 10,	//  String	
	eIceEventParam_CountryAfter				= 11,	//  String
	eIceEventParam_LicenseBefore			= 12,	//  INT32
	eIceEventParam_LicenseAfter				= 13,	//  INT32
	eIceEventParam_NewAccountPromoCode		= 14,	//	String
	eIceEventParam_NewAccountRefDomain		= 15,	//	String
	eIceEventParam_NewAccountInstallId		= 16,	//	String
	eIceEventParam_NewAccountClientPlatform	= 17,	//	INT32
	eIceEventParam_AccountValidationType	= 18,	//	Flags16			// eIceEventParam_AccountValidationTypeFlags16_Bits
	eIceEventParam_Brand                    = 19,   //  enum BrandTypes // PYR-40455
	eIceEventParam_NewAccountTinfo			= 20,	//	String
	eIceEventParam_NewAccountSiteId			= 21,	//	INT32			// enum Sites // internal not supported by script now
	eIceEventParam_NewAccountCountry		= 22,	//	String			// internal not supported by script now
	eIceEventParam_NewAccountAffiliateTracker=23,	//	String
	eIceEventParam_DefaultCurrency			= 24,	//	String
	eIceEventParam_UserStateTrackCode		= 25,	//	INT32
	eIceEventParam_UserFlags				= 26,	//  INT32			// not exposed through script - internal use only. Not ice flag but integer type.
	eIceEventParam_UserFlags2				= 27,	//  INT64			// not exposed through script - internal use only. Not ice flag but integer type.
	eIceEventParam_UserFlags3				= 28,	//  INT64			// not exposed through script - internal use only. Not ice flag but integer type.
	eIceEventParam_UserPrivs				= 29,	//  INT32			// not exposed through script - internal use only. Not ice flag but integer type.
	eIceEventParam_UserPrivs2				= 30,	//  INT64			// not exposed through script - internal use only. Not ice flag but integer type.
	eIceEventParam_UserPrivs3				= 31,	//  INT64			// not exposed through script - internal use only. Not ice flag but integer type.
	eIceEventParam_NewAccountEmail			= 32,	//	String			// not exposed through script - internal use only.
	eIceEventParam_RingLowStake				= 33,	//	INT32			// PYR-66899
	eIceEventParam_RingBuyInTime			= 34,	//	SrvTime			// PYR-66899
	eIceEventParam_RingCap					= 35,	//	INT32			// PYR-66899
	eIceEventParam_RingAnte					= 36,	//	INT32			// PYR-66899
	eIceEventParam_RingMultiGameType		= 37,	//	INT32			// PYR-66899
	eIceEventParam_RingCurrency				= 38,	//	String			// PYR-66899
	eIceEventParam_Site						= 39,   //  Flags32			// enum Sites (see sites.h)
	eIceEventParam_TimerId					= 40,	//	INT64			// PYR-80269
	eIceEventParam_TaskState				= 41,	//	INT8			// PYR-80269
	eIceEventParam_TotalsState				= 42,	//	INT16			// PYR-80269
	eIceEventParam_MergedTableId			= 43,	//	INT64			// PYR-84882
	eIceEventParam_NewAccountMktgPromoCode	= 44,	//	String			// PYR-92250
	eIceEventParam_GameAnalyticsProgramId	= 45,	//	INT32			// PYR-96173
	eIceEventParam_SiteLong					= 46,	//	FlagsLong		// PYR-100633 	- new uploads will use this id
	eIceEventParam_GameAnalyticsProgramName	= 47,	//	String			// PYR-109274
		
	eIceEventParam_TableSize				= 101,	//	INT8			Any, 2, 3, 4, 6, 9, 10
	eIceEventParam_HandResult				= 102,	//	INT8			eHandFinishResult
	eIceEventParam_FinishedHandAt			= 103,	//	INT8			eHandFinishPosition
	eIceEventParam_RingHighStake			= 104,	//	INT32			
	eIceEventParam_RingHandFlags8			= 105,	//	INT8			eIceEventParam_RingHandFlags8_Bits
	eIceEventParam_RingNumPlayersDealtIn	= 106,	//	INT8
	eIceEventParam_SeatPositionFlags8_Deprecated = 107,	//	Flags8		--> to be deprecated. See comment at eIceEventParam_SeatPositionFlags8_Bits definition
	eIceEventParam_RingHandNetLossAmount	= 108,	//  INT64
	eIceEventParam_RingHandNetWinAmount		= 109,	//  INT64
	eIceEventParam_RingHandGrossWinAmount	= 110,	//  INT64
	eIceEventParam_RingHandBetAmount		= 111,	//  INT64
	eIceEventParam_SeatPositionFlags16		= 112,	//	Flags16			see eIceEventParam_SeatPositionFlags16_Bits
	eIceEventParam_RingHandNumberOfSwaps	= 113,	//	INT8			// PYR-107361 
	
	eIceEventParam_HandHoleCardsPair		= 150,	//	String			
	eIceEventParam_HandHoleCardsFilter		= 151,	//	CustomFilter	input value String, input param id eIceEventParam_HandHoleCards

	eIceEventParam_HandHoleCardsSuitCount	= 152,	//	INT8			Suit counts
	eIceEventParam_HandHoleCardsHighValue	= 153,	//	INT8			
	eIceEventParam_HandHoleCardsLowValue	= 154,	//	INT8			
	eIceEventParam_HandHoleCardsHighSuit	= 155,	//	INT8			
	eIceEventParam_HandHoleCardsLowSuit		= 156,	//	INT8			
	eIceEventParam_HandHoleCardsGap			= 157,	//	INT8			from 0 to 11 (2 - K)
	eIceEventParam_HandStrengthRank			= 158,	//  INT8
	eIceEventParam_HandStrengthSuit			= 159,	//  INT8
	eIceEventParam_HandStrengthHigh			= 160,	//  INT8
	eIceEventParam_HandStrengthLow			= 161,	//  INT8
	eIceEventParam_HandHoleCards			= 162,	//	Flags64		eIceEventParam_Card_Bits64
	eIceEventParam_HandBoardCards			= 163,	//	Flags64		eIceEventParam_Card_Bits64
	eIceEventParam_RingHoleAndFlopCards		= 164,	//	Flags64		eIceEventParam_Card_Bits64
	eIceEventParam_HandPowerPlayedFlags64	= 165,	//  Flags64		see PokerGamePowerId enum in protocols/table.h for possible values
	eIceEventParam_UnfoldAntesReturned		= 166,	//	INT8
	eIceEventParam_UnfoldResult				= 167,	//	INT8		eHandFinishResult
	eIceEventParam_UnfoldBet				= 168,	//	INT8
	eIceEventParam_UnfoldFinish				= 169,	//	INT8		eHandFinishPosition
	eIceEventParam_UnfoldHandStrengthRank	= 170,	//	INT8		eIceHandStrength
	eIceEventParam_HandPotSizeAmountBB		= 171,	//	INT64
	eIceEventParam_HandPotSizeAmount		= 172,	//	INT64
	eIceEventParam_HandPreFlopAction		= 173,	//  Flags8		see eIceEventParam_HandActionFlags8_Bits
	eIceEventParam_HandFlopAction			= 174,	//  Flags8		see eIceEventParam_HandActionFlags8_Bits
	eIceEventParam_HandTurnAction			= 175,	//  Flags8		see eIceEventParam_HandActionFlags8_Bits
	eIceEventParam_HandRiverAction			= 176,	//  Flags8		see eIceEventParam_HandActionFlags8_Bits
	eIceEventParam_PlayersStacked			= 177,	//  INT16
		
	eIceEventParam_PokerSideBetAmount			= 178,	// INT64
	eIceEventParam_PokerSideBetWinAmount		= 179,	// INT64
	eIceEventParam_PokerSideBetNetWinAmount		= 180,	// INT64
	eIceEventParam_PokerSideBetNetLossAmount	= 181,	// INT64
	eIceEventParam_PokerSideBetCurrency			= 182,	// String 
	eIceEventParam_PokerSideBetReturn			= 183,	// INT32
	eIceEventParam_PokerSideBetType				= 184,	// Flags8	see eIceEventParam_PokerSideBetType_Flags8
	eIceEventParam_PokerSideBetPayoutHoleCards	= 185,	// INT8		see sidegame::ePokerSideBetResult_HoleCards in protocols/minigames/minigamescommonprot.h for possible values
	eIceEventParam_PokerSideBetPayoutFlopBet	= 186,	// INT8		see sidegame::ePokerSideBetResult_FlopCards in protocols/minigames/minigamescommonprot.h for possible values
	eIceEventParam_PokerSideBetResult			= 187,	// INT8		see eIceEventParam_PokerSideBetResult_INT8 
	
	eIceEventParam_TournId					= 200,	// INT64			
	eIceEventParam_Tourn_Group				= 201,	// String		(20 characters)
	eIceEventParam_Tourn_MaxPlayers			= 202,	// INT32		Any, 2 (heads up)	Any	Any, 2	Play in any HUSNG	
	eIceEventParam_Tourn_ResultFlags8		= 203,	// Flags8		eIceEventTournResultFlags8_Bits
	eIceEventParam_Tourn_BuyIn				= 204,	// INT64		Any, <amount>
	eIceEventParam_Tourn_WinAmount			= 205,	// INT64		Any, <minAmount>
	eIceEventParam_Tourn_TargetTourn		= 206,	// INT32
	eIceEventParam_Tourn_Flags16			= 207,	// Flags16		see eIceEventParam_Tourn_Flags16_Bits
	eIceEventParam_Tourn_Speed				= 208,	// INT8
	eIceEventParam_Tourn_HandType			= 209,	// INT8
	eIceEventParam_Tourn_NumKicked			= 210,	// INT8
	eIceEventParam_Tourn_NumPlayers			= 211,	// INT32
	eIceEventParam_Tourn_WinType			= 212,	// Flags8		see eIceEventParam_Tourn_WinType_Flags8_Bits
	eIceEventParam_Tourn_Rank				= 213,	// INT32
	eIceEventParam_Tourn_RankPercentile		= 214,	// INT16
	eIceEventParam_Tourn_SpnGoMltplier		= 215,	// INT32		obsolete by PYR-108380
 	eIceEventParam_Tourn_SpnGoType			= 216,	// INT8			see eIceTournSpinGoType
	eIceEventParam_Tourn_SpnGoJckptLvl		= 217,	// INT8
	eIceEventParam_Tourn_InternalRef		= 218,	// String		(20 characters) PYR-66899
	eIceEventParam_Tourn_PlayedTime			= 219,	// SrvTime		// PYR-66899
	eIceEventParam_Tourn_Currency			= 220,	// String		// PYR-66899
	eIceEventParam_Tourn_MinPlayers			= 221,	// INT32		// PYR-66899
	eIceEventParam_Tourn_TargetRef			= 222,	// String		(20 characters) PYR-66899
	eIceEventParam_Tourn_PotSizeAmount		= 223,	// INT64		// amounts are in chips, not cents, so can't share with ring pot size amount
	eIceEventParam_Tourn_HandBetAmount		= 224,	// INT64		// amounts are in chips, not cents, so can't share with ring hand bet amount
	eIceEventParam_Tourn_Stake				= 225,	// INT64
	eIceEventParam_Tourn_SpnGoMltprDec		= 226,	// INT32		PYR-108380 replaces eIceEventParam_Tourn_SpnGoMltplier
	eIceEventParam_Tourn_PPPFlags			= 227,	// Flags8		PYR-123925 eIceEventParam_Tourn_ProgressiveType_Flags8_Bits
	eIceEventParam_Tourn_PPPBuyIn			= 228,	// INT64		PYR-123925
	eIceEventParam_ProgressivePlusBuyIn		= 229,	// INT64		PYR-125878
	eIceEventParam_ProgressivePlusWinAmount	= 230,	// INT64		PYR-125878
	eIceEventParam_Tourn_TotalPrizePool		= 231,	// INT64		PYR-124673
	eIceEventParam_Tourn_BountiesWon		= 232,	// INT32		PYR-124673 - currently only for first place
		
	eIceEventParam_VppsAmount				= 300,	//	INT64			Any, <amount>
	eIceEventParam_TaskGroupId				= 301,	//	INT32
	eIceEventParam_TaskId					= 302,	//	INT32
	eIceEventParam_StarCodeName				= 303,	//	String
	eIceEventParam_TournTicketTypeId		= 304,	//	INT32
	eIceEventParam_RmDepositAmount			= 305,	//	INT32
	eIceEventParam_RmDepositCurrency		= 306,	//	String
	eIceEventParam_RmDepositPaySystemName	= 307,	//	String
	eIceEventParam_TaskRefName				= 308,	//	String
	eIceEventParam_RmDepositFirst			= 309,	//	INT8
	eIceEventParam_VppsVipLevel				= 310,	//	INT8 VipStatusLevel
	eIceEventParam_VppsMonthly				= 311,	//	INT64
	eIceEventParam_VppsYearly				= 312,	//	INT64
	eIceEventParam_FppAmount64				= 313,	//	INT64
	eIceEventParam_VppsFlags8				= 314,	//	Flags8		eIceEventParam_VppsFlags8_Bits - same as for eIceEventParam_VppsFlags8
	eIceEventParam_VppsMonthlyBefore		= 315,	//	INT64
	eIceEventParam_VppsYearlyBefore			= 316,	//	INT64
	eIceEventParam_VppsVipLevelBefore		= 317,	//	INT8 VipStatusLevel
	eIceEventParam_RmDepositTransId			= 318,	//	INT32
	eIceEventParam_BonusId					= 319,	//	INT32
	eIceEventParam_FirstLTPointsType		= 320,	//	INT8		eIceFirstLTPointsType
	eIceEventParam_TrackCode				= 321,	//	String
	eIceEventParam_TrackCodeChange			= 322,	//	INT8		eIceTrackCodeChangeType
	eIceEventParam_TaskSuccessCount			= 333,	//  INT8
	eIceEventParam_TaskFailCount			= 334,	//  INT8
	eIceEventParam_VppsSource				= 335,	//  INT8
	eIceEventParam_AppLoginId				= 336,	//	INT64		eIceFirstLTPointsType, eIceEvent_RmDeposit
	eIceEventParam_RmDepositFlags8			= 337,	//	Flags8		eIceDepositFlags8
	eIceEventParam_CppsAmount				= 338,	//	INT64
	eIceEventParam_CppsMonthly				= 339,	//	INT64
	eIceEventParam_CppsYearly				= 340,	//	INT64
	eIceEventParam_CppsLifetime				= 341,	//	INT64
	eIceEventParam_CppsFlags8				= 342,	//	Flags8		eIceEventParam_CppsFlags8_Bits
	eIceEventParam_CppsSource				= 343,	//	INT8
	eIceEventParam_SppsAmount				= 344,	//	INT64
	eIceEventParam_SppsMonthly				= 345,	//	INT64
	eIceEventParam_SppsYearly				= 346,	//	INT64
	eIceEventParam_SppsLifetime				= 347,	//	INT64
	eIceEventParam_SppsFlags8				= 348,	//	Flags8		eIceEventParam_SppsFlags8_Bits
	eIceEventParam_SppsSource				= 349,	//	INT8
	eIceEventParam_CrmOfferRef				= 350,	//	String
	eIceEventParam_BoardId					= 351,	//	INT32
	eIceEventParam_BoardSeasonId			= 352,	//	INT32
	eIceEventParam_BoardSeasonRank			= 353,	//	INT32
	eIceEventParam_BoardSeasonScore			= 354,	//	INT64
	eIceEventParam_BoardSeasonRankPercentile= 355,	//	INT16
	eIceEventParam_CvlSchema				= 356,	//	INT32
	eIceEventParam_TaskInstanceStateValue1	= 357,	//	INT64
	eIceEventParam_TaskInstanceStateValue2	= 358,	//	INT64
	eIceEventParam_TaskInstanceStateValue3	= 359,	//	INT64
	eIceEventParam_KYCDocType				= 360,	//	INT8		eIceKYCDocType
	eIceEventParam_KYCDocSubCategory		= 361,	//	INT8		eIceKYCDocSubCategory
	eIceEventParam_RewardPointAmount		= 362,	//	INT32
	eIceEventParam_RewardPointSource		= 363,	//	INT8		eIceRewardPointSource
	eIceEventParam_FirstTwitchTimeLink		= 364,	//	INT8		PYR-112222
	eIceEventParam_RmDepositTransId64		= 365,	//	INT64	PYR-116913
	eIceEventParam_TaskInstanceStateValue4	= 366,	//	INT64
	eIceEventParam_TaskInstanceStateValue5	= 367,	//	INT64
	
	eIceEventParam_Special_FilterMatchTime	= 400,	// TIME		event passed filters - always 1
	eIceEventParam_Special_JoinedQuestTime	= 401,	// TIME		when joined quest
	eIceEventParam_Special_TaskCreatedTime	= 402,	// TIME		
	eIceEventParam_Special_TaskExpireTime	= 403,	// TIME		
	eIceEventParam_Special_TaskActivateTime	= 404,	// TIME		

	eIceEventParam_FilterMatchParam			= 500,	// INT64	used internally only for counter comparisons. 
													//			Means that all filters have matched but the action itself does not evaluate a parameter
													//			instead it will likely do event counting
	eIceEventParam_EventFlowControl			= 501,	// INT32	eIceTaskEventFlowControl - not directly evaluated but stored in db
	eIceEventParam_SetVar					= 502,	// String	- in the parser but not directly evaluated as string but stored in db and used to build expression evaluation tree - xFilter in validator
//	eIceEventParam_Unused					= 503,	// 
	eIceEventParam_StateFilterFirst			= 504,	// INT64	reserved several param IDs for state filters - will include 503 once they are reclaimed
	eIceEventParam_StateFilterLast			= 510,	// INT64	reserved several param IDs for state filters - will include 503 once they are reclaimed
	
	eIceEventParam_DuelTypeId				= 600,	// INT32
	eIceEventParam_DuelBuyIn				= 601,	// INT64
	eIceEventParam_DuelNumHands				= 602,	// INT32
	eIceEventParam_DuelPlayerFlags8			= 603,	// Flags8		eIceEventParam_DuelPlayerFlags8_Bits
	eIceEventParam_DuelNumHandsWon			= 604,	// INT32
	eIceEventParam_DuelResult				= 605,	// INT8			eIceDuelResult
	eIceEventParam_DuelScoreAmount			= 606,	// INT32
	eIceEventParam_DuelScoreMargin			= 607,	// INT32
	eIceEventParam_DuelScorePercent			= 608,	// INT16
	eIceEventParam_DuelWinAmount			= 609,	// INT64
	eIceEventParam_DuelHandAllin			= 610,	// INT8				
	eIceEventParam_DuelHandScoreAmount		= 611,	// INT32
	eIceEventParam_DuelHandFinish			= 612,	// INT8			eIceDuelHandFinish
	eIceEventParam_DuelHoleCardsSuitCount	= 613,	// INT8
	eIceEventParam_DuelHoleCardsHighValue	= 614,	// INT8
	eIceEventParam_DuelHoleCardsHighSuit	= 615,	// INT8
	eIceEventParam_DuelHoleCardsLowValue	= 616,	// INT8
	eIceEventParam_DuelHoleCardsLowSuit		= 617,	// INT8
	eIceEventParam_DuelHoleCardsGap			= 618,	// INT8
	eIceEventParam_DuelHandStrength			= 619,	// INT64
	eIceEventParam_DuelHandResult			= 620,	// INT8			eIceDuelResult
	eIceEventParam_DuelOpponentIntId		= 621,	// INT32
	eIceEventParam_DuelScoreMaxAvailable	= 622,	// INT64
	
	
	// PYR-34344
	eIceEventParam_CasinoGameCategory			= 700,	// INT32
	eIceEventParam_CasinoGameType				= 701,	// INT32
	eIceEventParam_CasinoGameVariant			= 702,	// INT32
	eIceEventParam_CasinoGameVendor				= 703,	// INT32
	eIceEventParam_CasinoGameMode				= 704,	// INT8
	eIceEventParam_CPPAmount					= 705,	// INT64
	eIceEventParam_CasinoBetAmount				= 706,	// INT64	
	eIceEventParam_CasinoGrossWinAmount			= 707,	// INT64
	eIceEventParam_CasinoNetWinAmount			= 708,	// INT64
	eIceEventParam_CasinoBetFundSource			= 709,	// INT8			eIceCasinoBetFundSource
	eIceEventParam_CasinoNetLossAmount			= 710,	// INT64
	eIceEventParam_CasinoBetFundSourceBonusId	= 711,	// INT32
	eIceEventParam_CasinoBetReturn				= 712,	// INT32
	
	// PYR-84989 EventCasinoRaceFinish
	eIceEventParam_CasinoRaceID                 = 713,	// INT64
	eIceEventParam_CasinoRaceGameTemplateID		= 714,	// INT32
	eIceEventParam_CasinoRaceNumPlayers			= 715,	// INT32
	eIceEventParam_CasinoRaceRank				= 716,	// INT32
	eIceEventParam_CasinoRaceRankPercentile		= 717,	// INT32
	eIceEventParam_CasinoRacePoints				= 718,	// INT64
	eIceEventParam_CasinoRaceWinAmount			= 719,	// INT64
	eIceEventParam_CasinoRacePrizeType			= 720,	// INT8
	eIceEventParam_CasinoRaceSpinCount			= 721,	// INT32

	eIceEventParam_SportsBetAmount			= 751,	// INT64
	eIceEventParam_SportsBetCurrency		= 752,	// String
	eIceEventParam_SportsBetOdds			= 753,	// INT32
	eIceEventParam_SportsKindFlag64			= 754,	// Flags64		eIceEventParam_SportFlag64
	eIceEventParam_SportsBetFlags8			= 755,	// Flags8		eIceEventParam_SportBetFlag8 - bet type flag actually
	eIceEventParam_SportsEventID			= 756,	// INT64
	eIceEventParam_SportsWinAmount			= 757,	// INT64
	eIceEventParam_SportsBetReturn			= 758,	// INT32
	eIceEventParam_SportsBetResultFlags8	= 759,	// Flags8		eIceEventParam_SportBetResultFlag8
	eIceEventParam_SportsBetTmngInPlayBets	= 760,	// INT16
	eIceEventParam_SportsBetTmngPreMtchBts	= 761,	// INT16
	eIceEventParam_SportsSpinBet			= 762,	// INT8
	eIceEventParam_SportsSpinBetMultiplier	= 763,	// INT32
	eIceEventParam_SportsCompetition		= 764,	// String
	eIceEventParam_SportsMarketId			= 765,	// INT32
	eIceEventParam_SportsMarketName			= 766,	// String
	eIceEventParam_SportsBetFundSource		= 767,	// INT8			eIceSportsBetFundSource
	eIceEventParam_SportsBetSelection		= 768,	// String
	eIceEventParam_SportsBetSelectionCode	= 769,	// String
	eIceEventParam_SportsBetSelectionId		= 770,	// INT64
	eIceEventParam_SportsBetMarketCode		= 771,	// String
	eIceEventParam_SportsBetMultipleNumLegs = 772,	// INT32
	eIceEventParam_SportsBetMultipleLegsWon = 773,	// INT32
	eIceEventParam_SportsBetNetWin			= 774,	// INT64
	eIceEventParam_SportsBetNetLoss			= 775,	// INT64
	eIceEventParam_SportsBetFlags8Group		= 776,	// Flags8		eIceEventParam_SportBetFlag8_Group - a group of 'yes'/'no' like parameters to use with mergeId
	eIceEventParam_SportsVirtualKindFlag64	= 777,	// Flags64		eIceEventParam_VirtualSportFlag64
	eIceEventParam_SportsBetComposition		= 778,	// INT8			eIceSportsBetComposition
	eIceEventParam_SportsBetFundSrcCampgnID	= 779,	// INT64
	eIceEventParam_SportsKindFlagLong		= 780,	// FlagsLong
	
	eIceEventParam_BlackjackResult				= 800,	// Flags8		// eIceEventParam_SportBetResultFlag8 - same as sports book
	eIceEventParam_BlackjackBetCurrency       	= 801,	// String
	eIceEventParam_BlackjackInitialBetAmount  	= 802,	// INT64
	eIceEventParam_BlackjackTotalBetAmount    	= 803,	// INT64
	eIceEventParam_BlackjackGrossWinAmount    	= 804,	// INT64
	eIceEventParam_BlackjackNetWinAmount      	= 805,	// INT64
	eIceEventParam_BlackjackNumCards          	= 806,	// INT16
	eIceEventParam_BlackjackDealerTotal       	= 807,	// INT64
	eIceEventParam_BlackjackFlags8   			= 808,	// Flags8		// eIceEventParam_BlackjackFlags8_Bits
	eIceEventParam_BlackjackPlayerCards       	= 809,	// Flags64		// eIceEventParam_Card_Bits64
	eIceEventParam_BlackjackDealerCards       	= 810,	// Flags64		// eIceEventParam_Card_Bits64
	eIceEventParam_BlackjackSeatNumSplits		= 811,	// INT16
	eIceEventParam_BlackjackNumDoubleDowns		= 812,	// INT16
	eIceEventParam_BlackjackPlayerTotal			= 813,	// INT8
	eIceEventParam_BlackjackFirstTwoCards      	= 814,	// Flags64		// eIceEventParam_Card_Bits64
											
	eIceEventParam_RouletteBetAmount				= 815,	// INT64
	eIceEventParam_RouletteBetCurrency				= 816,	// String
	eIceEventParam_RouletteGrossWinAmount			= 817,	// INT64
	eIceEventParam_RouletteBetResult				= 818,	// Flags8		// eIceEventParam_SportBetResultFlag8 - same as sports book
	eIceEventParam_RouletteNumberSpun				= 819,	// INT8			// eIceRouletteNumber
	eIceEventParam_RouletteNumBetsPlaced			= 820,	// INT16
	eIceEventParam_RouletteNumBetsWon				= 821,	// INT16
	eIceEventParam_RouletteTypeBetsPlaced			= 822,	// Flags16		// eIceEventParam_RouletteBetTypeFlags16_Bits
	eIceEventParam_RouletteNumStraightBets			= 823,	// INT8
	eIceEventParam_RouletteNumSplitBets				= 824,	// INT8
	eIceEventParam_RouletteNumCornerBets			= 825,	// INT8
	eIceEventParam_RouletteNumStreetBets			= 826,	// INT8
	eIceEventParam_RouletteNumLineBets				= 827,	// INT8
	eIceEventParam_RouletteBetType					= 828,	// Flags16		// eIceEventParam_RouletteBetTypeFlags16_Bits
	eIceEventParam_RouletteBetNumber				= 829,	// INT8			// eIceRouletteNumber
	eIceEventParam_RouletteBetSubNumber				= 830,	// INT8			// eIceRouletteNumber
	eIceEventParam_RouletteNumTypeBetsPlaced		= 831,	// INT16
	eIceEventParam_RouletteNumRedBlackBetsPlaced	= 832,	// INT8
	eIceEventParam_RouletteNumOddEvenBetPlaced		= 833,	// INT8
	eIceEventParam_RouletteNumHighLowBetPlaced		= 834,	// INT8
	eIceEventParam_RouletteNumDozensBetPlaced		= 835,	// INT8
	eIceEventParam_RouletteNumColumnBetPlaced		= 836,	// INT8
	eIceEventParam_RouletteSpinReturn				= 837,	// INT32
	eIceEventParam_RouletteBetReturn				= 838,	// INT32

	eIceEventParam_MiniGameRef					= 850,	// String
	eIceEventParam_MiniGameResult				= 851,	// INT8			// eIceMiniGameResult
	eIceEventParam_MiniGamePrizeType			= 852,	// INT32		// see PrizeType enum in protocols/minigames/minigamescommonprot.h for possible values
	eIceEventParam_MiniGamePrizeAmount			= 853,	// INT64
	eIceEventParam_MiniGamePrizeRefId			= 854,	// INT64
	eIceEventParam_MiniGamePlayId				= 855,	// INT64  PYR-124673
	
	eIceEventParam_FreemiumPurchaseAmount		= 875,	// INT64
	eIceEventParam_FreemiumPurchaseFirst		= 876,	// INT8

	eIceEventParam_SlotLinesPlayed				= 900,	// INT8
	eIceEventParam_SlotLinesWon					= 901,	// INT8
	eIceEventParam_SlotNumDifferentSymbols		= 902,	// INT8
	eIceEventParam_SlotBonusGameGrossWinAmount	= 903,	// INT64
	eIceEventParam_SlotBonusGameType			= 904,	// INT8			// eIceSlotBonusGameType
	eIceEventParam_SlotNumSymbol				= 905,	// INT8[15]		// array 
	eIceEventParam_SlotLocation					= 906,	// INT8[3][5]	// array eIceSlotSymbol		(SlotLocationXY is an X-Y co-ordinate of a symbol position)

	eIceEventParam_ChestTier					= 925,	// INT8
	eIceEventParam_ChestLevelUp					= 926,	// INT8
	eIceEventParam_ChestTypeId					= 927,	// INT32
	eIceEventParam_ChestIssuerType				= 928,	// INT8
	eIceEventParam_ChestExchangeType			= 929,	// INT8			// eIceChestExchangeType

	eIceEventParam_DreamCatcherBetAmount		= 950,	// INT64
	eIceEventParam_DreamCatcherBetCurrency		= 951,	// String
	eIceEventParam_DreamCatcherGrossWinAmount	= 952,	// INT64
	eIceEventParam_DreamCatcherNetWinAmount		= 953,	// INT64
	eIceEventParam_DreamCatcherNetLossAmount	= 954,	// INT64
	eIceEventParam_DreamCatcherBetResult		= 955,	// Flags8		// eIceEventParam_SportBetResultFlag8 - same as sports book
	eIceEventParam_DreamCatcherNumberSpun		= 956,	// INT8			// we use the eIceEventParam_DreamCatcherNumber_Flags8_Bits values to set a single value 
	eIceEventParam_DreamCatcherNumBetsPlaced	= 957,	// INT16
	eIceEventParam_DreamCatcherBetNumber		= 958,	// Flags8		// eIceEventParam_DreamCatcherNumber_Flags8_Bits
	eIceEventParam_DreamCatcherSpinReturn		= 959,	// INT32
	eIceEventParam_DreamCatcherBetReturn		= 960,	// INT32
	eIceEventParam_DreamCatcherMultiplier		= 961,	// INT8

	eIceEventParam_TexasHoldemBonusBetAmount		= 975,	// INT64
	eIceEventParam_TexasHoldemBonusBetCurrency		= 976,	// String
	eIceEventParam_TexasHoldemBonusGrossWinAmount	= 977,	// INT64
	eIceEventParam_TexasHoldemBonusNetWinAmount		= 978,	// INT64
	eIceEventParam_TexasHoldemBonusNetLossAmount	= 979,	// INT64
	eIceEventParam_TexasHoldemBonusResult			= 980,	// Flags8		// eIceEventParam_SportBetResultFlag8 - same as sports book
	eIceEventParam_TexasHoldemBonusPlayerCards     	= 981,	// Flags64		// eIceEventParam_Card_Bits64
	eIceEventParam_TexasHoldemBonusDealerCards    	= 982,	// Flags64		// eIceEventParam_Card_Bits64
	eIceEventParam_TexasHoldemBonusBoardCards		= 983,	// Flags64		// eIceEventParam_Card_Bits64
	eIceEventParam_TexasHoldemBonusHandStrengthRank	= 984,	// INT8
	eIceEventParam_TexasHoldemBonusBetReturn		= 985,	// INT32
	eIceEventParam_TexasHoldemBonusBetType			= 986,	// Flags8		// eIceEventParam_TexasHoldemBonusBetTypeFlags8_Bits

	eIceEventParam_BaccaratBetAmount				= 1000,	// INT64
	eIceEventParam_BaccaratBetCurrency				= 1001,	// String
	eIceEventParam_BaccaratGrossWinAmount			= 1002,	// INT64
	eIceEventParam_BaccaratNetWinAmount				= 1003,	// INT64
	eIceEventParam_BaccaratNetLossAmount			= 1004,	// INT64
	eIceEventParam_BaccaratBetResult				= 1005,	// Flags8		// eIceEventParam_SportBetResultFlag8 - same as sports book
	eIceEventParam_BaccaratPlayerCard     			= 1006,	// Flags64		// eIceEventParam_Card_Bits64
	eIceEventParam_BaccaratBankerCard				= 1007,	// Flags64		// eIceEventParam_Card_Bits64
	eIceEventParam_BaccaratNumBetsPlaced			= 1008,	// INT16
	eIceEventParam_BaccaratBetType					= 1009,	// Flags16		// eIceEventParam_BaccaratBetTypeFlags16_Bits
	eIceEventParam_BaccaratPlayerTotal				= 1010,	// INT8
	eIceEventParam_BaccaratBankerTotal				= 1011,	// INT8

	eIceEventParam_DragonTigerBetAmount				= 1025,	// INT64
	eIceEventParam_DragonTigerBetCurrency			= 1026,	// String
	eIceEventParam_DragonTigerGrossWinAmount		= 1027,	// INT64
	eIceEventParam_DragonTigerNetWinAmount			= 1028,	// INT64
	eIceEventParam_DragonTigerNetLossAmount			= 1029,	// INT64
	eIceEventParam_DragonTigerBetResult				= 1030,	// Flags8		// eIceEventParam_SportBetResultFlag8 - same as sports book
	eIceEventParam_DragonCard						= 1031,	// Flags64		// eIceEventParam_Card_Bits64
	eIceEventParam_TigerCard						= 1032,	// Flags64		// eIceEventParam_Card_Bits64
	eIceEventParam_DragonTigerNumBetsPlaced			= 1033,	// INT16
	eIceEventParam_DragonTigerBetType				= 1034,	// Flags8		// eIceEventParam_DragonTigerBetTypeFlags8_Bits

	eIceEventParam_JackpotWinType					= 1050,	// INT8			// eIceJackpotWinType
	eIceEventParam_JackpotType						= 1051,	// INT8			// eIceJackpotType
	eIceEventParam_JackpotWinAmount					= 1052,	// INT64
	
	eIceEventParam_VictoryTribesTribeSelection		= 1060,	// INT8			// eIceVictoryTribesTribeSelection
	eIceEventParam_VictoryTribesLevel				= 1061,	// INT8
	eIceEventParam_VictoryTribesBonusTournType		= 1062,	// INT8			// eIceVictoryTribesBonusTournType
	eIceEventParam_VictoryTribesTournNumPlayers		= 1063,	// INT32
	eIceEventParam_VictoryTribesTournamentCoins		= 1064,	// INT64
	eIceEventParam_VictoryTribesTournRank			= 1065,	// INT32
	eIceEventParam_VictoryTribesTournRankPercentile	= 1066,	// INT16
	eIceEventParam_VictoryTribesTournPrizeAmount	= 1067,	// INT64
	eIceEventParam_VictoryTribesLevelUp				= 1068,	// INT8
	eIceEventParam_VictoryTribesLevelDown			= 1069,	// INT8
	eIceEventParam_VictoryTribesFreeSpinsTrigger	= 1070,	// INT8
	eIceEventParam_VictoryTribesTribalFreeSpinsTrigger=1071,// INT8
		
	//////////////////////////////////////////////////////////////////////////
	// debug event parameters range
	eIceEventParam_DbgExpectedEventType			= 32000,	//	INT16
	eIceEventParam_DbgReceivedEventType			= 32001,	//	INT16
};

//////////////////////////////////////////////////////////////////////////

enum eIceTaskParamMatchOutcome
{
	eIceTaskParamMatchOutcome_Undefined		= 0,
	eIceTaskParamMatchOutcome_Match			= 1,
	eIceTaskParamMatchOutcome_Success		= 2,
	eIceTaskParamMatchOutcome_Fail			= 3,
	eIceTaskParamMatchOutcome_NotMatch		= 4,	
};

//////////////////////////////////////////////////////////////////////////
enum eIceParamTypes
{
	ePt_Unknown		= 0,
	ePt_Bool		= 1,
	ePt_INT8		= 2,
	ePt_INT16		= 3,
	ePt_INT32		= 4,
	ePt_INT64		= 5,
	ePt_DateTime	= 6,
	ePt_String		= 7,
	ePt_Flags8		= 8,
	ePt_Flags16		= 9,
	ePt_Flags32		= 10,
	ePt_Flags64		= 11,
	ePt_MsgBody		= 12,	// not for IceParamData
	ePt_xFilter		= 13,	// Only used inside ice lobby.
	ePt_Extended	= 14,	// Only used inside ice lobby.
	ePt_FlagsLong	= 15,
};

//////////////////////////////////////////////////////////////////////////

enum eIceExtendedParamSource
{
	eEpSrc_Unknown					= 0,
	eEpSrc_UserState				= 1, // check only user state
	eEpSrc_UserStateAcceptSnapshot	= 2, // first check task state snapshot and then check user state
	eEpSrc_CountersGroup			= 3, // group totals linked via [Task] CountersGroupRef=. param ID is the counter ID in the the group totals array (index in the array for now)
};

enum eIceExtendedParamId
{
	eEpId_Unknown						= 0,
	eEpId_DefaultCurrency				= 1, //	String	// user default currency
	eEpId_EligibilityDataTrackCodeId	= 2, // INT32
};


//////////////////////////////////////////////////////////////////////////

enum eIceLeaderBoardTypes
{
	eLT_Unknown		= 0,
	eLT_Daily		= 1,
	eLT_Weekly		= 2,
	eLT_Monthly		= 3,
	eLT_Quarterly	= 4,
	eLT_Yearly		= 5,
};

enum eIceLeaderBoardStatus
{
	eLB_Open	= 0,
	eLB_Closed	= 1,
};

enum eIceLeaderBoardSeasonStatus
{
	eLB_Season_NotSaved	= 0,
	eLB_Season_TopSaved	= 1, // PYR-34915 - season top users have been saved
};

enum eIceLeaderBoardDbAction
{
	eIceLeaderBoard_DbAction_None			= 0,
	eIceLeaderBoard_DbAction_Insert			= 1,
	eIceLeaderBoard_DbAction_UpdateFull		= 2,
	eIceLeaderBoard_DbAction_UpdateOutOnly	= 3,
};

enum eIceLeaderBoardFlags32
{
	eIceLeaderBoardFlags32_NoOptOut						= 1<<0,
	eIceLeaderBoardFlags32_Achievement					= 1<<1,
	eIceLeaderBoardFlags32_TimeTieBreaking				= 1<<2,
	eIceLeaderBoardFlags32_ForceDurationChange			= 1<<3,
	eIceLeaderBoardFlags32_FragmentedSeason				= 1<<4,
	eIceLeaderBoardFlags32_RequiresParticipationReqest	= 1<<5,
};

enum eIceCardsSuits		// PYR-33924
{
	eIceCardSuits_Clubs						= 0,
	eIceCardSuits_Diamonds					= 1,
	eIceCardSuits_Hearts					= 2,
	eIceCardSuits_Spades					= 3,
};

enum eIceScriptLoadMode
{
	eIceScriptLoadModeAnyScript				= 0,
	eIceScriptLoadModeRegularScriptOnly		= 1,
	eIceScriptLoadModeLeaderBoardOnly		= 2,
	eIceScriptLoadModeAchievementOnly		= 3,
};

enum eIcePromoType
{
	eIcePromoType_Default				= 0, // "QuestList"
	eIcePromoType_Puzzle				= 1, // "Puzzle"
	eIcePromoType_PokerHand				= 2, // "PokerHand"
	eIcePromoType_Bingo					= 3, // "Bingo"
};

enum eIcePromoVerticalType // vertical in marketing terms PYR-34392
{
	eIcePromoVerticalType_Global		= 1,
	eIcePromoVerticalType_Poker			= 2,
	eIcePromoVerticalType_Casino		= 3,
	eIcePromoVerticalType_SportsBook	= 4,
};

enum eIceAchievementStatsField
{
	eIceAchievementStatsField_None					= 0,

	// poker duels stats fields
	eIceAchievementStatsField_DuelsWon				= 1,  // INT32 Duels.Won				
	eIceAchievementStatsField_DuelsLost				= 2,  // INT32 Duels.Lost				
	eIceAchievementStatsField_DuelsDraw				= 3,  // INT32 Duels.Draw				
	eIceAchievementStatsField_DuelsHandsWon			= 4,  // INT32 Duels.HandsWon			
	eIceAchievementStatsField_DuelsHandsPlayed		= 5,  // INT32 Duels.HandsPlayed		
	eIceAchievementStatsField_DuelsWinStreak		= 6,  // INT32 Duels.WinStreak		
	eIceAchievementStatsField_DuelsBestFive			= 7,  // INT64 Duels.BestFive			
	eIceAchievementStatsField_DuelsTotalEarnedChips	= 8,  // INT64 Duels.TotalEarnedChips	
	eIceAchievementStatsField_DuelsTotalStartChips	= 9,  // INT64 Duels.TotalStartChips	

	// others - leave some space for additional duels fields

};

enum eIcePokerDuelTableIds
{
	eIcePokerDuelTableId_DuelStats		= 1,
	eIcePokerDuelTableId_DuelPvpStats	= 2,
};

enum eGetUserRankMode
{
	eGUR_ModeUnknown				= 0,
	eGUR_ModeGetWebAppData			= 1,
	eGUR_ModeGetNativeScoresOnly	= 2,
};

enum eIceHandStrength		// PYR-33084
{
	eIceHandStrength_Unknown				= 0,
	eIceHandStrength_HighCard				= 1,
	eIceHandStrength_OnePair				= 2,
	eIceHandStrength_TwoPair				= 3,
	eIceHandStrength_Trips					= 4,
	eIceHandStrength_Straight				= 5,
	eIceHandStrength_Flush					= 6,
	eIceHandStrength_FullHouse				= 7,
	eIceHandStrength_Quads					= 8,
	eIceHandStrength_StraightFlush			= 9,
	eIceHandStrength_RoyalFlush				= 10,
};

enum eIceResetAuditType // PYR-33958
{
	eIceResetAuditType_Leaderboard					= 0,
	eIceResetAuditType_AdminQuestCompleteSuccess	= 1,
	eIceResetAuditType_AdminQuestCompleteFail		= 2,
};

enum eIceVppsSource // PYR-42661
{
	eIceVppsSource_Unknown           = 0,
	eIceVppsSource_Tourn             = 1,
	eIceVppsSource_Cash_Zoom         = 2,
	eIceVppsSource_Cash_Regular      = 3,
};

enum eIceRouletteNumber // PYR-43854
{
	eIceRouletteNumber_0             = 0,
	eIceRouletteNumber_1             = 1,
	eIceRouletteNumber_2             = 2,
	eIceRouletteNumber_3             = 3,
	eIceRouletteNumber_4             = 4,
	eIceRouletteNumber_5             = 5,
	eIceRouletteNumber_6             = 6,
	eIceRouletteNumber_7             = 7,
	eIceRouletteNumber_8             = 8,
	eIceRouletteNumber_9             = 9,
	eIceRouletteNumber_10            = 10,
	eIceRouletteNumber_11            = 11,
	eIceRouletteNumber_12            = 12,
	eIceRouletteNumber_13            = 13,
	eIceRouletteNumber_14            = 14,
	eIceRouletteNumber_15            = 15,
	eIceRouletteNumber_16            = 16,
	eIceRouletteNumber_17            = 17,
	eIceRouletteNumber_18            = 18,
	eIceRouletteNumber_19            = 19,
	eIceRouletteNumber_20            = 20,
	eIceRouletteNumber_21            = 21,
	eIceRouletteNumber_22            = 22,
	eIceRouletteNumber_23            = 23,
	eIceRouletteNumber_24            = 24,
	eIceRouletteNumber_25            = 25,
	eIceRouletteNumber_26            = 26,
	eIceRouletteNumber_27            = 27,
	eIceRouletteNumber_28            = 28,
	eIceRouletteNumber_29            = 29,
	eIceRouletteNumber_30            = 30,
	eIceRouletteNumber_31            = 31,
	eIceRouletteNumber_32            = 32,
	eIceRouletteNumber_33            = 33,
	eIceRouletteNumber_34            = 34,
	eIceRouletteNumber_35            = 35,
	eIceRouletteNumber_36            = 36,
	eIceRouletteNumber_00            = 37,
};

#define	eIceEventParam_Card_Bits64_2h	((INT64)1<<0)	// 2
#define	eIceEventParam_Card_Bits64_3h	((INT64)1<<1)	// 3
#define	eIceEventParam_Card_Bits64_4h	((INT64)1<<2)	// 4
#define	eIceEventParam_Card_Bits64_5h	((INT64)1<<3)	// 5
#define	eIceEventParam_Card_Bits64_6h	((INT64)1<<4)	// 6
#define	eIceEventParam_Card_Bits64_7h	((INT64)1<<5)	// 7
#define	eIceEventParam_Card_Bits64_8h	((INT64)1<<6)	// 8
#define	eIceEventParam_Card_Bits64_9h	((INT64)1<<7)	// 9
#define	eIceEventParam_Card_Bits64_10h	((INT64)1<<8)	// 10
#define	eIceEventParam_Card_Bits64_Jh	((INT64)1<<9)	// 11
#define	eIceEventParam_Card_Bits64_Qh	((INT64)1<<10)	// 12
#define	eIceEventParam_Card_Bits64_Kh	((INT64)1<<11)	// 13
#define	eIceEventParam_Card_Bits64_Ah	((INT64)1<<12)	// 14

#define	eIceEventParam_Card_Bits64_2c	((INT64)1<<13)									
#define	eIceEventParam_Card_Bits64_3c	((INT64)1<<14)									
#define	eIceEventParam_Card_Bits64_4c	((INT64)1<<15)									
#define	eIceEventParam_Card_Bits64_5c	((INT64)1<<16)									
#define	eIceEventParam_Card_Bits64_6c	((INT64)1<<17)									
#define	eIceEventParam_Card_Bits64_7c	((INT64)1<<18)									
#define	eIceEventParam_Card_Bits64_8c	((INT64)1<<19)									
#define	eIceEventParam_Card_Bits64_9c	((INT64)1<<20)									
#define	eIceEventParam_Card_Bits64_10c	((INT64)1<<21)									
#define	eIceEventParam_Card_Bits64_Jc	((INT64)1<<22)									
#define	eIceEventParam_Card_Bits64_Qc	((INT64)1<<23)									
#define	eIceEventParam_Card_Bits64_Kc	((INT64)1<<24)									
#define	eIceEventParam_Card_Bits64_Ac	((INT64)1<<25)									
																						
#define	eIceEventParam_Card_Bits64_2s	((INT64)1<<26)
#define	eIceEventParam_Card_Bits64_3s	((INT64)1<<27)
#define	eIceEventParam_Card_Bits64_4s	((INT64)1<<28)
#define	eIceEventParam_Card_Bits64_5s	((INT64)1<<29)
#define	eIceEventParam_Card_Bits64_6s	((INT64)1<<30)
#define	eIceEventParam_Card_Bits64_7s	((INT64)1<<31)
#define	eIceEventParam_Card_Bits64_8s	((INT64)1<<32)
#define	eIceEventParam_Card_Bits64_9s	((INT64)1<<33)
#define	eIceEventParam_Card_Bits64_10s	((INT64)1<<34)
#define	eIceEventParam_Card_Bits64_Js	((INT64)1<<35)
#define	eIceEventParam_Card_Bits64_Qs	((INT64)1<<36)
#define	eIceEventParam_Card_Bits64_Ks	((INT64)1<<37)
#define	eIceEventParam_Card_Bits64_As	((INT64)1<<38)

#define	eIceEventParam_Card_Bits64_2d	((INT64)1<<39)
#define	eIceEventParam_Card_Bits64_3d	((INT64)1<<40)
#define	eIceEventParam_Card_Bits64_4d	((INT64)1<<41)
#define	eIceEventParam_Card_Bits64_5d	((INT64)1<<42)
#define	eIceEventParam_Card_Bits64_6d	((INT64)1<<43)
#define	eIceEventParam_Card_Bits64_7d	((INT64)1<<44)
#define	eIceEventParam_Card_Bits64_8d	((INT64)1<<45)
#define	eIceEventParam_Card_Bits64_9d	((INT64)1<<46)
#define	eIceEventParam_Card_Bits64_10d	((INT64)1<<47)
#define	eIceEventParam_Card_Bits64_Jd	((INT64)1<<48)
#define	eIceEventParam_Card_Bits64_Qd	((INT64)1<<49)
#define	eIceEventParam_Card_Bits64_Kd	((INT64)1<<50)
#define	eIceEventParam_Card_Bits64_Ad	((INT64)1<<51)

enum eIceEventParam_DreamCatcherNumber_Flags8_Bits // PYR-59835
{
	eIceEventParam_DreamCatcherNumber_Flags8_Bits_1		= 1<<0,
	eIceEventParam_DreamCatcherNumber_Flags8_Bits_2		= 1<<1,
	eIceEventParam_DreamCatcherNumber_Flags8_Bits_5		= 1<<2,
	eIceEventParam_DreamCatcherNumber_Flags8_Bits_10	= 1<<3,
	eIceEventParam_DreamCatcherNumber_Flags8_Bits_20	= 1<<4,
	eIceEventParam_DreamCatcherNumber_Flags8_Bits_40	= 1<<5,
};

enum eIceEventParam_ChestIssuerType_INT8
{
	eIceEventParam_ChestIssuerType_Unknown		= 0,
	eIceEventParam_ChestIssuerType_IceLobby		= 1,
	eIceEventParam_ChestIssuerType_Admin		= 2,
	eIceEventParam_ChestIssuerType_Automation	= 3,
	eIceEventParam_ChestIssuerType_MgLobby		= 4,
};

// PYR-106565
enum eIceEventParam_PokerSideBetType_Flags8
{
	eIceEventParam_PokerSideBetType_Flags8_HoleCards = 1 << 0,
	eIceEventParam_PokerSideBetType_Flags8_FlopCards = 1 << 1,
};

// PYR-106565
enum eIceEventParam_PokerSideBetResult_INT8
{
	eIceEventParam_PokerSideBetResult_Unknown	= 0,
	eIceEventParam_PokerSideBetResult_Won		= 1,
	eIceEventParam_PokerSideBetResult_Lost		= 2,
	eIceEventParam_PokerSideBetResult_Cancelled = 3,
};

#endif // icesharedenums_h_included
