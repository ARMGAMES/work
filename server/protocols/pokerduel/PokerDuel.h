// PokerDuelLobby.h
#ifndef PokerDuel_h_included
#define PokerDuel_h_included

//	Parameter used for display purposes to indicate whether the script can be used for random match creations, direct challenges or both. 
enum DuelMatchType		//	bitmask // DUELTYPES.MATCHTYPE
{
	DuelMatchType_Random			= 1 << 0, // 1
	DuelMatchType_FriendChallenge	= 1 << 1, // 2

	DuelMatchType_All = DuelMatchType_Random | DuelMatchType_FriendChallenge
};

enum DuelTypeStatus // DUELTYPES.STATUS
{
	DuelTypeStatus_Active					= 1,
	DuelTypeStatus_Paused					= 2,
	DuelTypeStatus_CancelledStillRunning	= 3,		//	Admin has cancelled, but there are still running duels or challenges pending. No further duel creation with this type allowed.
	DuelTypeStatus_Cancelled				= 4,		//	Admin has cancelled and all duels of this type are complete.
	DuelTypeStatus_Pending					= 5,		//	Active but before start time
	DuelTypeStatus_Expired					= 6,		//	Past end time
};

enum ChallengeResponse // DUELCHALLENGES.RESPONSE
{
	ChallengeResponse_Pending			= 1,
	ChallengeResponse_Accept			= 2,
	ChallengeResponse_Refuse			= 3,
	ChallengeResponse_DuelTypeCancelled = 4,
	ChallengeResponse_Cancelled			= 5,		//	Challenger cancelled their challenge
	ChallengeResponse_Timeout			= 6,		//	Opponent did not respond in time, or random pool timed out.
	ChallengeResponse_CancelledByAdmin	= 7,
};

// when adding new value, check all queries where DUELS.STATUS is used including OLAP queries to make sure they are not broken
enum DuelStatus // DUELS.STATUS
{
	DuelStatus_Active	= 1,
	DuelStatus_Paused	= 2,
	DuelStatus_Cancelled = 3,		//	Duel canceled by admin
	DuelStatus_Closed = 4,			//	Game is over - Duel still published.
	DuelStatus_UnPublished = 5,		//	Game is over - Duel is no longer published.
};


enum eDuelUserFlags // DUELUSERS.DUFLAGS
{
	eDuelUserFlags_SecondPlayer		= (1 << 0), // 1
	eDuelUserFlags_Awarded			= (1 << 1), // 2
	eDuelUserFlags_Refunded			= (1 << 2), // 4
	eDuelUserFlags_isOnline			= (1 << 3), // 8	//	memory only flag - NOT persisted to database
	eDuelUserFlags_SeenAllReveals   = (1 << 4),	//	16	//	memory only flag - NOT persisted to database
	eDuelUserFlags_isTeamPro		= (1 << 5), //	32	//	memory only flag - NOT persisted to database
};

// Properties in DUELTYPEPROPS table
#define PROP_DUELTYPE_ADMISSIONS			0
#define PROP_DUELTYPE_NAME					1
#define PROP_DUELTYPE_VPP_MULTIPLIERS		2

// Properties in DUELPROPS table
#define PROP_DUEL_HANDIDS					0
#define PROP_DUEL_LOG_LOCATION				1
#define PROP_DUEL_VPP_MULTIPLIERS			2

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//	Authenticated, Client -> Dlobby. Retrieve a player's running duels
//	TODO: remove when older clients are gone
#define MSG_POKERDUEL_GET_RUNNING_DUELS						1000		//	
#define MSG_POKERDUEL_GET_RUNNING_DUELS_REPLY				1001		//	2[s|4<84ss>] - errCode[errDesc|numDuels<duelId,duelTemplateId,player1,player2>]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_POKERDUEL_REPLAY_DUEL							1002		//	8 - duelId
#define MSG_POKERDUEL_REPLAY_DUEL_REPLY						1003		//	2[s] - errCode[errDesc]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_POKERDUEL_PRECONFIRMATION_CHECK					1004		//	44 - duelTypeId,enum DuelMatchType
#define MSG_POKERDUEL_PRECONFIRMATION_CHECK_REPLY			1005		//	2[sM] - errCode[errDesc,extraErrorInfo]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_POKERDUEL_GET_DUELTYPES							1006		//
#define MSG_POKERDUEL_GET_DUELTYPES_REPLY					1007		//	2[s|vector< pokerduel::DuelTemplateForClient >]

//	Client -> DLobby. Client challenges oppenentUserId to a duel
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_POKERDUEL_CHALLENGE_REQUEST						1008		//	s4 - opponentUserId,duelTemplateId
#define MSG_POKERDUEL_CHALLENGE_REQUEST_REPLY				1009		//	2[s|8] - errCode[errDesc|challengeId]

//	DLobby -> Client. DLobby forwards challenge request.
#define MSG_POKERDUEL_CHALLENGE_OFFER						1010		//	s84 - challengerUserId,challengeId,duelTemplateId

//	Client -> Dlobby.  Challenge response.
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_POKERDUEL_CHALLENGE_OFFER_RESPONSE				1012		//	84 - challengeId,answer
#define MSG_POKERDUEL_CHALLENGE_OFFER_RESPONSE_REPLY		1013		//	2[s|8] - errCode[errDesc|duelId]

//	DLobby -> Client. Send challenge response back to requestor
#define MSG_POKERDUEL_CHALLENGE_REQUEST_RESPONSE			1014		//	884 - challengeId,duelId,answer

//	Client -> Dlobby. Random challenge
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_POKERDUEL_START_RANDOM_DUEL						1016		//	4 - duelTypeId
#define MSG_POKERDUEL_START_RANDOM_DUEL_REPLY				1017		//	2[s|8s] - errCode[errDesc|duelId,opponentUserId]

//	DLobby -> Client
#define  MSG_POKERDUEL_RANDOM_OPPONENT_FOUND				1018		//	48s - duelTypeId,duelId,opponentUserId

//	Client -> DLobby
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_POKERDUEL_CANCEL_CHALLENGE						1020		//	8 - challengeId
#define MSG_POKERDUEL_CANCEL_CHALLENGE_REPLY				1021		//	2[s] - errCode[errDesc]

//	DLobby -> Client, unsolicited
#define MSG_POKERDUEL_DUEL_STATUS_CHANGE_BY_ADMIN			1024		//	84 - duelId,enum DuelStatus

//	DLobby -> Client
#define MSG_POKERDUEL_SERVER_RESTARTING						1025

//	Client -> DLobby, UnSolicited connection
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_POKERDUEL_PROFILE_REQUEST						1026		//	s - userId

//	DLobby -> Client, UnSolicited connection
#define MSG_POKERDUEL_PROFILE_RESPONSE						1027		//	

//	Client -> DLobby
#define MSG_POKERDUEL_GET_ALL_ACHIEVEMENTS					1028		//	empty
#define MSG_POKERDUEL_GET_ALL_ACHIEVEMENTS_REPLY			1029		//	2 - errCode

//	Client ->Dlobby
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_POKERDUEL_LEAVE_RANDOM_POOL						1030		//	4 - duelTypeId
#define MSG_POKERDUEL_LEAVE_RANDOM_POOL_REPLY				1031		//	2[s] - errCode[errDesc]

//	Client -> Dlobby
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_POKERDUEL_GETHISTORY							1032		//	4 - numEvents
#define MSG_POKERDUEL_GETHISTORY_REPLY						1033		//	2[s|b4<M()>4<M()>] - errCode[errDesc|moreEventsAvail,numDuels<M()>numCancelledChallenges<M()>]

//	Client -> DLobby
#define MSG_POKERDUEL_GET_ICE_CHALLENGES					1034		//
//	DLobby -> Client
#define MSG_POKERDUEL_GET_ICE_CHALLENGES_REPLY				1035		// 2[s|4] - errCode[errDesc|numAvailableIceChallenges]

//	Client -> DLobby
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_POKERDUEL_GET_PENDING_CHALLENGE_INFO			1036		//	s - userId
#define MSG_POKERDUEL_GET_PENDING_CHALLENGE_INFO_REPLY		1037		//	2[s|4<8>4<8>] - errCode[errDesc|toFriendCount<challengeId>,fromFriendCount<challengeId>]

//	DLobby -> Client, unsolicited connection, sent to challengee (player 2)
#define MSG_POKERDUEL_NOTIFY_CHALLENGE_REVOKED				1038		//	84 - challengeId,enum ChallengeResponse

// for Duel open chat
#define MSG_DUEL_CHAT_REQUEST								1039		// Client -> DLobby (Unsolicited connection) -> DChat
#define MSG_DUEL_CHAT_BROADCAST								1040		// DChat -> DLobby -> Client (Unsolicited connection)
#define MSG_DUEL_CHAT_SYNC_REQUEST							1041		// Client -> DLobby (Session connection) -> DChat
#define MSG_DUEL_CHAT_SYNC_REPLY							1042		// DChat -> DLobby -> Client (Session connection)
#define MSG_DUEL_CHAT_REPORT_CURR_MAXORDINALID				1043		// Client -> DLobby (Unsolicited connection)

//	DLobby -> Client, unsolicited connection.
#define MSG_POKERDUEL_RANDOM_ENTRY_REVOKED					1044		//	4 - duelTypeId

//	Authenticated, Client -> Dlobby. Retrieve a player's running duels
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_POKERDUEL_GET_RUNNING_DUELS2					1045		//	
#define MSG_POKERDUEL_GET_RUNNING_DUELS2_REPLY				1046		//	2[s|4<84ss>] - errCode[errDesc|numDuels<duelId,duelTemplateId,player1,player2>]

//	PYR-39717
#define MSG_POKERDUEL_GET_DUEL_INFO							1048	//	8 - duelId
#define MSG_POKERDUEL_GET_DUEL_INFO_REPLY					1049	//	2[s|bM()] -	errCode[errDesc|isRunningNotCompleted,M(runningDuel) | M(completedDuel)]

//	PYR-39717
#define MSG_POKERDUEL_GET_RANDOM_POOL_INFO					1050	//	empty
#define MSG_POKERDUEL_GET_RANDOM_POOL_INFO_REPLY			1051	//	2[s|4<M(4t)>] -	errCode[errDesc|numRandomPoolEntries<M(duelTypeId,whenJoined)>]

//	PYR-39717
#define MSG_POKERDUEL_GET_CHALLENGE_INFO					1052	//	8 - challenge Id
#define MSG_POKERDUEL_GET_CHALLENGE_INFO_REPLY				1053	//	2[s|bM()4] - errCode[errDesc|isPendingNotCancelled,M(pendingChallenge) | M(cancelledChallenge) | M(duelId),challengeType]

//	Admin messages

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_POKERDUEL_ADMIN_ADD_DUELTYPES					2000		//	44M() - reqId, numScripts,M(duelType)
#define MSG_POKERDUEL_ADMIN_ADD_DUELTYPES_REPLY				2001		//	46[s] - reqId,errCode[errDesc]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_POKERDUEL_ADMIN_CANCEL_DUELTYPE					2002		//	44b - reqId,duelTypeId,cancelPendingChallenges
#define MSG_POKERDUEL_ADMIN_CANCEL_DUELTYPE_REPLY			2003		//	46[s|4] - reqId,errCode[errDesc|newStatus]

//	Available
//		2004
//		2005

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_POKERDUEL_ADMIN_UPDATE_DUEL_STATUS				2006		//	444<8> - reqId,newStatus,numDuels<duelId>
#define MSG_POKERDUEL_ADMIN_UPDATE_DUEL_STATUS_REPLY		2007		//	46[s|4<s>] - reqId,errCode[errDesc|numErrors<errDesc>]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_POKERDUEL_ADMIN_UPDATE_DUEL_STATUS_BY_FILTER		2008		//	44s - reqId,newStatus,filter
#define MSG_POKERDUEL_ADMIN_UPDATE_DUEL_STATUS_BY_FILTER_REPLY	2009		//	46[s|4<s>] - reqId,errCode[errDesc|numErrors<errDesc>]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_POKERDUEL_ADMIN_PAUSE_DUEL_TYPES				2010		//	44<4> - reqId,numDuelTypes<duelTypeId>
#define MSG_POKERDUEL_ADMIN_PAUSE_DUEL_TYPES_REPLY			2011		//	46[s|4<s>] - reqId,errCode[errDesc|numErrors<errDesc>]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_POKERDUEL_ADMIN_RESUME_DUEL_TYPES				2012		//	44<4> - reqId,numDuelTypes<duelTypeId>
#define MSG_POKERDUEL_ADMIN_RESUME_DUEL_TYPES_REPLY			2013		//	46[s|4<s>] - reqId,errCode[errDesc|numErrors<errDesc>]

#define MSG_POKERDUEL_ADMIN_GET_DUELTYPE_INFO				2014		//	44<4> - reqId, size<duelTypeId>
#define MSG_POKERDUEL_ADMIN_GET_DUELTYPE_INFO_REPLY			2015		//	46[s|4<4444>] - reqId, errCode[errDesc|size<currentStatus,numRunningDuels,numPendingChallenges,numRandomPoolEntries>]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_POKERDUEL_ADMIN_GET_DUEL_INFO					2016		//	44<8> - reqId, size<duelId>
#define MSG_POKERDUEL_ADMIN_GET_DUEL_INFO_REPLY				2017		//	46[s|4<4tt>] - reqId, errCode[errDesc|size<numHandsRemaining,lastActionTimePlayer1,lastActionTimePlayer2>]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_POKERDUEL_ADMIN_GET_CHALLENGE_INFO				2018		//	44<8> - reqId, size<challengeId>
#define MSG_POKERDUEL_ADMIN_GET_CHALLENGE_INFO_REPLY		2019		//	46[s|4<6[s|t]>] - reqId, errCode[errDesc|size<errCode[errDesc|expiryTime]>]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_POKERDUEL_ADMIN_CANCEL_CHALLENGE				2020		//	48 - reqId, challengeId
#define MSG_POKERDUEL_ADMIN_CANCEL_CHALLENGE_REPLY			2021		//	46[s] - reqId,errCode[errDesc]

//	PYR-36881
#define MSG_POKERDUEL_ADMIN_MODIFY_DUELTYPE					2022		//	444M() - reqId, duelTypeId,numScripts,M(duelType)
#define MSG_POKERDUEL_ADMIN_MODIFY_DUELTYPE_REPLY			2023		//	46[s] = reqId,errCode[errDesc]

// Available
//		2024
//		2025

// PYR-36408
#define MSG_POKERDUEL_ADMIN_GET_OBJ_STATE					2026		//44 - reqId,stateMask 
#define MSG_POKERDUEL_ADMIN_GET_OBJ_STATE_REPLY				2027		//44<84814b> - reqId, numDueals<duelId,duelTypeId,tableId,status,siteMask,isPm>
#define MSG_POKERDUEL_ADMIN_GET_OBJ_SUMMARY					2028		//4 - reqId
#define MSG_POKERDUEL_ADMIN_GET_OBJ_SUMMARY_REPLY			2029		//44<4,DuelSummary> - reqId,count<site,DuelSummary>

//	PYR-37946
#define MSG_POKERDUEL_ADMIN_MASS_PAUSE						2030		//	4sbb - reqId,filter,includeDuels,includeDuelTypes
#define MSG_POKERDUEL_ADMIN_MASS_PAUSE_REPLY				2031		//	46[s] - reqId,errCode[errDesc]

#define MSG_POKERDUEL_ADMIN_MASS_RESUME						2032		//	4sbb - reqId,filter,includeDuels,includeDuelTypes
#define MSG_POKERDUEL_ADMIN_MASS_RESUME_REPLY				2033		//	46[s] - reqId,errCode[errDesc]

// DServer to/from DLobyy message
#define MSG_DUEL_SERVER_CONNECTED							3000   // DS -> DL
#define MSG_DUEL_SERVER_CONNECTED_REPLY						3001   // DL -> DS

#define MSG_DUEL_SERVER_CREATE_REQUEST						3002   // DL -> DS
#define MSG_DUEL_SERVER_CREATE_REPLY						3003   // DS -> DL
#define MSG_DUEL_SERVER_CREATE_REPLY_ACK					3004   // DL -> DS

#define MSG_DUEL_DSERVER_RESTORE_REQUEST					3005   // DL -> DS
#define MSG_DUEL_DSERVER_RESTORE_REPLY						3006   // DS -> DL
#define MSG_DUEL_DSERVER_RESTORE_REPLY_ACK					3007   // DL -> DS

#define MSG_DUEL_DLOBBY_DUEL_COMPLETE_REQUEST				3008   // DS -> DL
#define MSG_DUEL_DLOBBY_DUEL_COMPLETE_REPLY					3009   // DL -> DS

#define MSG_DUEL_DLOBBY_UPDATE_DUEL_STATUS_REQUEST			3010	//	DL -> DS
#define MSG_DUEL_DLOBBY_UPDATE_DUEL_STATUS_REPLY			3011	//	DS -> DL
#define MSG_DUEL_DLOBBY_UPDATE_DUEL_STATUS_REPLY_ACK		3012	//	DL -> DS	

#define MSG_DUEL_DLOBBY_DUEL_AWARD_WINNERS_REQUEST			3013	// DS -> DL
#define MSG_DUEL_DLOBBY_DUEL_AWARD_WINNERS_REPLY			3014	// DL -> DS

#define MSG_DUEL_DLOBBY_PENDING_GAME_ACTION_ADDED			3015	// DS -> DL
#define MSG_DUEL_DLOBBY_PENDING_GAME_ACTION_ADDED_REPLY		3016	// DL -> DS

#define MSG_DUEL_DLOBBY_PENDING_GAME_ACTION_REMOVED			3017	// DS -> DL
#define MSG_DUEL_DLOBBY_PENDING_GAME_ACTION_REMOVED_REPLY	3018	// DL -> DS

#define MSG_DUEL_DLOBBY_UPDATE_PLAYER_SCORES_REQUEST		3019	// DS -> DL
#define MSG_DUEL_DLOBBY_UPDATE_PLAYER_SCORES_REPLY			3020	// DL -> DS

#define MSG_DUEL_LOBBY_DSERVER_SHUTDOWN_REQUEST				3021	//	DL -> DS
#define MSG_DUEL_LOBBY_DSERVER_SHUTDOWN_REQUEST_REPLY		3022	//	DS -> DL
#define MSG_DUEL_LOBBY_DSERVER_SHUTDOWN_REQUEST_REPLY_ACK	3023	//	DL -> DS

#define MSG_DUEL_DLOBBY_UPDATE_PLAYER_ACTION_TIME_REQUEST	3024	//	DS -> DL
#define MSG_DUEL_DLOBBY_UPDATE_PLAYER_ACTION_TIME_REPLY		3025	//	SL -> DS

#define MSG_DUEL_DSERVER_CREATE_FINISHEDDUEL_FOR_REPLAY_REQUEST	3026	//	DL -> DS
#define MSG_DUEL_DSERVER_CREATE_FINISHEDDUEL_FOR_REPLAY_REPLY	3027	//	DS -> DL
#define MSG_DUEL_DSERVER_CREATE_FINISHEDDUEL_FOR_REPLAY_REPLY_ACK	3028	//	DL -> DS

#define MSG_DLOBBY_FINISHEDDUEL_REPLAY_DONE_REQUEST			3029	//	DS -> DL
#define MSG_DLOBBY_FINISHEDDUEL_REPLAY_DONE_REPLY			3030	//	DL -> DS

#define MSG_DUEL_DSERVER_CLOSE_DUEL_REQUEST					3031	//	DL -> DS
#define MSG_DUEL_DSERVER_CLOSE_DUEL_REQUEST_REPLY			3032	//	DS -> DL
#define MSG_DUEL_DSERVER_CLOSE_DUEL_REQUEST_REPLY_ACK		3033	//	DL -> DS

#define MSG_DUEL_DSERVER_PLAYER_LAST_REVEAL					3034	//	DS -> DL
#define MSG_DUEL_DSERVER_PLAYER_LAST_REVEAL_REPLY			3035	//	DL -> DS

//#define MSG_DUEL_DSERVER_BETTING_COMPLETE					3036	//	DS -> DL
//#define MSG_DUEL_DSERVER_BETTING_COMPLETE_REPLY				3037	//	DL -> DS


// DServer to/from Client message through DLobby. Unsolicited client connection
#define MSG_DUEL_USER_GAME_MESSAGE_TO_SERVER				5000	//	Client -> DL

#define MSG_DUEL_DLOBBY_POST_TO_PLAYER						5001	//	DL -> Client
//#define MSG_DUEL_DLOBBY_POST_TO_PLAYER_REPLY				5002	//	DL -> DS

#define MSG_DUEL_DSERVER_GAME_MESSAGE_TO_PLAYER				5003	//	DS -> DL
#define MSG_DUEL_DSERVER_GAME_MESSAGE_TO_PLAYER_REPLY		5004	//	DL -> DS

#define MSG_DUEL_DSERVER_GAME_MESSAGE_FROM_PLAYER			5005	//	DL -> DS


//	Friend server

//	PYR-37124
#define MSG_DUEL_FRIEND_GET_PENDING_CHALLENGES				6000	//	4s4<s> - reqId,userId,numFriends<friendUserId>
#define MSG_DUEL_FRIEND_GET_PENDING_CHALLENGES_REPLY		6001	//	46[s|4<44>] - reqId,errCode[errDesc|numFriends<numChallengesTo,numChallengesFrom>]


// DChat server
#define MSG_DUEL_DLOBBY_DCHAT_CONNECTED						7000	// DLobby -> DChat
#define MSG_DUEL_DLOBBY_DCHAT_CONNECTED_REPLY				7001	// DChat -> DLobby

#define MSG_DUEL_DLOBBY_DCHAT_GET_DUELINFO_REQUEST			7002	// DChat -> DLobby
#define MSG_DUEL_DLOBBY_DCHAT_GET_DUELINFO_REPLY			7003	// DLobby -> DChat
#define MSG_DUEL_DLOBBY_DCHAT_GET_DUELINFO_REPLY_ACK		7004	// DChat -> DLobby

#define MSG_DUEL_DLOBBY_DCHAT_UPDATE_DUELCLOSETIME			7005	// DLobby -> DChat
#define MSG_DUEL_DLOBBY_DCHAT_UPDATE_DUELCLOSETIME_REPLY	7006	// DChat -> DLobby


//	-------------------------------------------------------------------------------------------------------------------------
//	Error Codes

//	Duel Lobby to Client Error codes
enum PDuelErrorCodes
{
														//	Original table.h error code

	PDuelErr_Currency_Account_Not_Found = 100,			//	ClientMsg_Account_Not_Found
	PDuelErr_Currency_Account_Not_Found_Not_Allowed,	//	NA
	PDuelErr_Buyin_Limit_Not_Set,						//	ClientMsg_Buyin_Limit_Not_Set				empty error string - client should display responsible gaming dialog
	PDuelErr_Buyin_Limit_Reached,						//	ClientMsg_Buyin_Limit_Reached				empty error string - client should display responsible gaming dialog
	PDuelErr_Must_Have_DepositLimit,					//	ClientMsg_Must_Have_DepositLimit			empty error string - client should display responsible gaming dialog
	PDuelErr_Must_Have_Deposit_n_Buyin_Limits,			//	ClientMsg_Must_Have_Deposit_n_Buyin_Limits	empty error string - client should display responsible gaming dialog
	PDuelErr_Must_Have_Withdrawal_Limit,				//	ClientMsg_Must_Have_Withdrawal_Limit		empty error string - client should display responsible gaming dialog
	PDuelErr_Not_Allowed_Without_Player_Token,			//	ClientMsg_Not_Allowed_Without_Player_Token	empty error string - client calls appModule.handleNotAllowedWithoutRealMoneyOk
	PDuelErr_Not_Allowed_Without_RealMoneyOk,			//	ClientMsg_Not_Allowed_Without_RealMoneyOk	empty error string - client calls appModule.handleNotAllowedWithoutRealMoneyOk
	PDuelErr_User_Prefs_Changed,						//	ClientMsg_User_Prefs_Changed
	PDuelErr_Conversion_Rate_Error,						//	ClientMsg_Conversion_Rate_Error
	PDuelErr_Ratholing_Bypass,							//	ClientMsg_Ratholing_Bypass
	PDuelErr_NoSuchUser,								//
	PDuelErr_UserSelfExcluded,							//	NA											empty error string - see client handling for dbm.h error code USER_SELF_EXCLUDED
	PDuelErr_Not_Enough_Chips,							//	ClientMsg_Not_Enough_Chips
	PDuelErr_Blocked_RM_Tables,							//	ClientMsg_Blocked_RM_Tables
	PDuelErr_Blocked_PM_Tables,							//	ClientMsg_Blocked_PM_Tables
	PDuelErr_InvalidOpponentUserId,
	PDuelErr_InvalidDuelType,
	PDuelErr_CantChallengeSameOpponent,
	PDuelErr_InconsistentData,
	PDuelErr_ChallengeNoLongerAvailable,
	PDuelErr_CantJoinRandomPoolTwice,

	PDuelErr_Activity_Restricted_For_Staff,
	PDuelErr_User_Login_Italy_User_Login_Blocked,
	PDuelErr_Not_Allowed,
	PDuelErr_User_Email_Not_Validated,
	PDuelErr_ServerMaitenanceMode,
	PDuelErr_CantChallengeNonFriend,
	PDuelErr_NotAllowedInQuickAccessMode,

	PDuelErr_FatalServerError = 10000
};

// PYR-36408
enum eDuelStateMask
{
	eDuelStateMask_Active=		(1<<0),
	eDuelStateMask_Paused=		(1<<1),
	eDuelStateMask_Cancelled=	(1<<2),
	eDuelStateMask_Closed=		(1<<3),
	eDuelStateMask_UnPublished=	(1<<4),
};

#endif //PokerDuel_h_included




