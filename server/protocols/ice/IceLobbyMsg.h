// IceLobbyMsg.h
////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2015 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef IceLobbyMsg_h_included
#define IceLobbyMsg_h_included


//////////////////////////////////////////////////////////////////////////
// Messages

#define MSG_ICELOBBY_CLI_RECONNECT						3000	//	( server -> client )

// master iceLobby -> slave iceLobby
#define MSG_ICELOBBY_GET_SECONDARY_COUNTERS				1000	// 4 - reqId
#define MSG_ICELOBBY_GET_SECONDARY_COUNTERS_REPLY		1001	// 46[s|4<0>] - reqId, errCode, [errDescr | n<IceEventCounterData>]

// ice leader board server -> web app
#define MSG_ICE_LEADERBOARD_NOTIFY_SEASON_USERS_CHANGED	1002	// 44<4> - reqId=0, numSeasons<seasonId>

// ice lobby -> ice client proxy + web app (not to client)
#define MSG_ICELOBBY_NOTIFY_CLI_DATA_UPDATE				1003	// 4411s488488 - reqId=0, userIntId, isLeaderBoardParticipateByDefault, rmOk, dftCurrency, flags,flags2,flags3,prevs,prevs2,prevs3

// ice client proxy -> ice lobby Web App does not send [M(o)] - it is composed by the client proxy and shown in []
#define MSG_ICELOBBY_CLIENT_GET_USER_UPDATES			1004	// 4[M(o)] - reqId, IceUserRequestData
#define MSG_ICELOBBY_CLIENT_GET_USER_UPDATES_REPLY		1005	// 46[s|o] - reqId,errCode[errDescr|IceUserStateUpdatesNudge]

// ice lobby -> ice client proxy (to client)
#define MSG_ICELOBBY_NOTIFY_ICE_NUDGE					1006	// 44o - reqId=0,userIntId,IceUserStateUpdatesNudge

// ice client proxy -> web app
#define MSG_ICELOBBY_WEBAPP_NOTIFY_ICE_NUDGE			1007	// 444s - reqId(always 0), numChanges,flags,toolTip

// web app  -> ice client proxy -> ice lobby Web App does not send [M(o)] - it is composed by the client proxy and shown in []
#define MSG_ICELOBBY_WEBAPP_GET_USER_UPDATES_DATA		1008	// 4[M(o)]b4 - reqId, [IceUserRequestData not sent by webApp], bAllData, localeId
#define MSG_ICELOBBY_WEBAPP_GET_USER_UPDATES_DATA_REPLY	1009	// 46[s|o] - reqId,errCode[errDescr|IceUserStateUpdatesData]

// web app  -> ice client proxy -> ice lobby
// !!! WARNING !!! If you modify this message, please let test automation team know.
#define MSG_ICELOBBY_WEBAPP_USER_ACCEPT_TASK			1010	// 4[M(o)]44 - reqId, [IceUserRequestData not sent by webApp], taskId, localeId
#define MSG_ICELOBBY_WEBAPP_USER_ACCEPT_TASK_REPLY		1011	// 46[s|o] - reqId,errCode[errDescr]

// web app  -> ice client proxy -> ice lobby
#define MSG_ICELOBBY_WEBAPP_USER_CLEAR_NEW_STATUS		1012	// 4[M(o)]4<88> - reqId, [IceUserRequestData not sent by webApp], numTasks<acceptInstanceId, flagsToReset>
#define MSG_ICELOBBY_WEBAPP_USER_CLEAR_NEW_STATUS_REPLY	1013	// 46[s|o] - reqId,errCode[errDescr]

// web app  -> ice client proxy -> ice lobby
#define MSG_ICELOBBY_WEBAPP_GET_USER_UPDATES			1014	// 4[M(o)] - reqId, [IceUserRequestData not sent by webApp]
#define MSG_ICELOBBY_WEBAPP_GET_USER_UPDATES_REPLY		1015	// 46[s|o] - reqId,errCode[errDescr|IceUserStateUpdatesNudge]

#define MSG_ICELOBBY_LOBBYETC_USER_DISCONNECTED			1016	// 44 - reqId, userIntId
#define MSG_ICELOBBY_LOBBYETC_USER_DISCONNECTED_REPLY	1017	// 46[s] - reqId,errCode[errDescr]

// ice lobby -> ice client proxy -> lobby etc
#define MSG_ICELOBBY_NOTIFY_ICE_GET_CONNECTED_USERS		1018	// 4 - reqId=0

// ice leader board server -> web app
#define MSG_ICE_LEADERBOARD_NOTIFY_BOARDS_CHANGED		1019	// 4 - reqId=0

// lobby etc -> ice client proxy -> ice lobby
#define MSG_ICE_LOBBY_SEND_LOAD_CONNECTED_USERS			1020	// 4t4<o> - reqId, proxyTime, count<IceUserRequestData>
#define MSG_ICE_LOBBY_SEND_LOAD_CONNECTED_USERS_REPLY	1021	// 46[s] - reqId,errCode[errDescr]

// ice lobby master -> slave
#define MSG_ICE_LOBBY_CHECK_NEED_USERS					1022	// 4 - reqId
#define MSG_ICE_LOBBY_CHECK_NEED_USERS_REPLY			1023	// 46[s|b] - reqId,errCode[errDescr | needUsers ]

// ice lobby master -> slave
#define MSG_ICE_LOBBY_UPDATE_SECONDARY_SEASONS			1024	// 44<44<o>> - reqId,numTasks<taskId,numSeasons<IceTaskSeasonData>>
#define MSMSG_ICE_LOBBY_UPDATE_SECONDARY_SEASONS_REPLY	1025	// 46[s|b] - reqId,errCode[errDescr | needUsers ]

// admin(MSG_ADMIN_LOBBYETC_ICELOBBY_RESET_TASK) -> lobbyetc(MSG_ICELOBBY_RESET_USER_TASK_STATE) -> iceclientproxy -> ice lobby
#define MSG_ICELOBBY_RESET_USER_TASK_STATE				1026	// 4448444 - reqId,adminIntId, userIntId, seasonId, acceptedInstanceId, taskId,stateId
#define MSG_ICELOBBY_RESET_USER_TASK_STATE_REPLY		1027	// 46[s] - reqId, errCode[errDescr]

// webapp -> iceclientproxy -> icelobby -> ice leader board
#define MSG_ICELOBBY_WEBAPP_USER_GET_BOARD_RANK			1028	// 4[M(o)]4 - reqId, [IceUserRequestData not sent by webApp], localeId
#define MSG_ICELOBBY_WEBAPP_USER_GET_BOARD_RANK_REPLY	1029	// 46[s|o] - reqId, errCode[errDescr|replyObject]

// webapp -> ice leader board
#define MSG_ICELOBBY_WEBAPP_GET_ALL_BOARD_DATA			1030	// 4 - reqId
#define MSG_ICELOBBY_WEBAPP_GET_ALL_BOARD_DATA_REPLY	1031	// 46[s|o] - reqId, errCode[errDescr|replyObject]

// webapp -> ice leader board
#define MSG_ICELOBBY_WEBAPP_GET_SEASON_TOP_USERS		1032	// 44<4> - reqId
#define MSG_ICELOBBY_WEBAPP_GET_SEASON_TOP_USERS_REPLY	1033	// 46[s|o] - reqId, errCode[errDescr|replyObject]

// ice client proxy -> ice lobby Web App does not send [M(o)] - it is composed by the client proxy and shown in []
#define MSG_ICELOBBY_SAVE_USER_BOARD_PREFERENCE			1034	// 4[M(o)]4<4b> - reqId, IceUserRequestData, numBoards<boardId,eligible>
#define MSG_ICELOBBY_SAVE_USER_BOARD_PREFERENCE_REPLY	1035	// 46[s|4<46>] - reqId,errCode[errDescr|numItems<boardId,errCode>]

// ice client proxy -> ice lobby Web App does not send [M(o)] - it is composed by the client proxy and shown in []
#define MSG_ICELOBBY_SET_ALL_USER_BOARD_PREF			1036	// 4[M(o)]b - reqId, IceUserRequestData, eligible
#define MSG_ICELOBBY_SET_ALL_USER_BOARD_PREF_REPLY		1037	// 46[s|o] - reqId,errCode[errDescr]

// admin(MSG_ADMIN_LOBBYETC_ICELOBBY_RESET_TASK) -> lobbyetc(MSG_ICELOBBY_COMPLETE_USER_TASK)  -> iceclientproxy -> ice lobby
#define MSG_ICELOBBY_COMPLETE_USER_TASK					1038	// 4[M(o)]ob - reqId, ResetUserTaskStateAuditRequest, isCompleteSuccess
#define MSG_ICELOBBY_COMPLETE_USER_TASK_REPLY			1039	// 46[s|o] - reqId,errCode[errDescr]

// webapp -> iceclientproxy -> icelobby
#define MSG_ICELOBBY_WEBAPP_USER_GET_ELIGIBLE_BOARDS		1040	// 4[M(o)]4 - reqId, [IceUserRequestData not sent by webApp], localeId
#define MSG_ICELOBBY_WEBAPP_USER_GET_ELIGIBLE_BOARDS_REPLY	1041	// 46[s|4<4>4<4>] - reqId, errCode[errDescr|numBorads<boardId>numOptInLB<boardId>]

// ice lobby -> ice client proxy (to lobbyetc)
#define MSG_ICELOBBY_NOTIFY_ICE_TOTALS					1042	// 44<o> - reqId=0,n<IceTotalsState::composeForNotify>

// lobby etc -> ice client proxy -> ice lobby - update cached user data in last user request
#define MSG_ICE_LOBBY_UPDATE_USER_DATA					1043	// 4o - reqId, IceUserRequestData
#define MSG_ICE_LOBBY_UPDATE_USER_DATA_REPLY			1044	// 46[s] - reqId,errCode[errDescr]

// ice lobby master -> slave
#define MSG_ICE_LOBBY_SET_SHARED_ACTIVE_TRACKCODE_COUNT			1045	// 44 - reqId,numCodes
#define MSG_ICE_LOBBY_SET_SHARED_ACTIVE_TRACKCODE_COUNT_REPLY	1046	// 46[s] - reqId,errCode[errDescr]

// ice lobby -> ice client proxy (to client)
#define MSG_ICELOBBY_NOTIFY_INCREMENT_LOCAL_ACTIVE_TRACKCODE	1047	// 444 - reqId=0,lastUserIntId,numIssued

// ice client proxy -> ice lobby
#define MSG_ICELOBBY_INCREMENT_SHARED_ACTIVE_TRACKCODE			1048	// 44 - reqId,numCodes
#define MSG_ICELOBBY_INCREMENT_SHARED_ACTIVE_TRACKCODE_REPLY	1049	// 46[s] - reqId,errCode[errDescr]

// ice client proxy -> ice lobby Web App does not send [M(o)] - it is composed by the client proxy and shown in []
// similar to MSG_ICELOBBY_SAVE_USER_BOARD_PREFERENCE_OPTIN except cannot opt-out 
// and the MSG_ICELOBBY_SAVE_USER_BOARD_PREFERENCE_OPTIN cannot save preference for LBs that require opt-in
// 
#define MSG_ICELOBBY_SAVE_USER_BOARD_PREFERENCE_OPTIN			1050	// 4[M(o)]4<4b> - reqId, IceUserRequestData, numBoards<boardId,eligible>
#define MSG_ICELOBBY_SAVE_USER_BOARD_PREFERENCE_OPTIN_REPLY		1051	// 46[s|4<46>] - reqId,errCode[errDescr|numItems<boardId,errCode>]

//////////////////////////////////////////////////////////////////////////
// poker duel messages

// DLobby->iceclientproxy
#define MSG_ICE_LOBBY_GET_ALL_ACHIEVEMENTS				2001	//4o - reqId, IceUserRequestData
#define MSG_ICE_LOBBY_GET_ALL_ACHIEVEMENTS_REPLY		2002	// 46[s|4<o>] - reqId,errCode[errDescr|numItems<IceDuelsAchievementDescriptionData>]

// DLobby->iceclientproxy
#define MSG_ICE_LOBBY_GET_USER_ACHIEVEMENTS				2003	//4o - reqId, IceUserRequestData
#define MSG_ICE_LOBBY_GET_USER_ACHIEVEMENTS_REPLY		2004	// 46[s|4<o>] - reqId,errCode[errDescr|numItems<IceDuelsUserAchievementData>]

// DLobby->iceclientproxy
#define MSG_ICE_LOBBY_GET_USER_DUEL_STATS				2005	//4o - reqId, IceUserRequestData
#define MSG_ICE_LOBBY_GET_USER_DUEL_STATS_REPLY			2006	// 46[s|o] - reqId,errCode[errDescr|IcePokerDuelUserStatsFullData]

// DLobby->iceclientproxy
#define MSG_ICE_LOBBY_GET_USER_DUEL_PVP_STATS			2007	//4o4 - reqId, IceUserRequestData, userIntId2
#define MSG_ICE_LOBBY_GET_USER_DUEL_PVP_STATS_REPLY		2008	// 46[s|o] - reqId,errCode[errDescr|IcePokerDuelUserStatsPvpData]

// DLobby->iceclientproxy
#define MSG_ICE_LOBBY_GET_USER_STATE_SUMMARY			2009	//4o - reqId, IceUserRequestData
#define MSG_ICE_LOBBY_GET_USER_STATE_SUMMARY_REPLY		2010	// 46[s|o] - reqId,errCode[errDescr|IceUserStateSummary]



//////////////////////////////////////////////////////////////////////////
// internal messages

// admin -> ice lobby
#define MSG_ICE_LOBBY_SAVE_TASK_GROUPS_MASTER			20001	// 44<o> - reqId, numItems<IceTaskGroupData()>
#define MSG_ICE_LOBBY_SAVE_TASK_GROUPS_MASTER_REPLY		20002	// 46[s|4<4>] - reqId,errCode,[errDescr|numIds<groupId>]

// master lobby -> slave
#define MSG_ICE_LOBBY_SAVE_TASK_GROUPS_SLAVE			20003	// 44<o> - reqId, numItems<IceTaskGroupData()>
#define MSG_ICE_LOBBY_SAVE_TASK_GROUPS_SLAVE_REPLY		20004	// 46[s] - reqId,errCode,[errDescr]

// admin -> ice lobby
#define MSG_ICE_LOBBY_SAVE_TASK_SCRIPT_MASTER			20005	// 4so4<o>4<o>4<o> - reqId, questScript,IceTaskData,n<IceTaskParamData>,n<IceTaskRewardData>,n<IceTaskEligibilityData>
#define MSG_ICE_LOBBY_SAVE_TASK_SCRIPT_MASTER_REPLY		20006	// 46[s|4] - reqId,errCode,[errDescr|taskId]

// master lobby -> slave
#define MSG_ICE_LOBBY_SAVE_TASK_SCRIPT_SLAVE			20007	// 4o4<o>4<o>4<o> - reqId,IceTaskData,n<IceTaskParamData>,n<IceTaskRewardData>,n<IceTaskEligibilityData>
#define MSG_ICE_LOBBY_SAVE_TASK_SCRIPT_SLAVE_REPLY		20008	// 46[s] - reqId,errCode,[errDescr]

// admin -> ice lobby or master ice lobby -> secondary ice lobby
#define MSG_ICE_LOBBY_RELOAD_TEMPLATES					20023	// 4b44 - reqId, sendNudge, licenseId, siteId,
#define MSG_ICE_LOBBY_RELOAD_TEMPLATES_REPLY			20024	// 46[s]

// admin -> master ice lobby
// !!! WARNING !!! If you modify this message, please let automated test team know.
#define MSG_ICE_DEBUG_SAVE_SCRIPT_FROM_FILE				20025	// 44s - reqId,taskId,fileName
#define MSG_ICE_DEBUG_SAVE_SCRIPT_FROM_FILE_REPLY		20026	// 46[s]

#define MSG_ICE_LOBBY_SEND_NUDGE						20027	// 4 - reqId, userIntId
#define MSG_ICE_LOBBY_SEND_NUDGE_REPLY					20028	// 46[s]

// lobby etc -> ice client proxy -> ice lobby
#define MSG_ICE_LOBBY_SEND_USER_CONNECTED				20029	// 4o - reqId, IceUserRequestData
#define MSG_ICE_LOBBY_SEND_USER_CONNECTED_REPLY			20030	// 46[s] - reqId, errCode[errDescr]

// admin -> ice lobby
#define MSG_ICE_LOBBY_SAVE_PROMO_MASTER					20031	// 4o - reqId, IcePromoData()
#define MSG_ICE_LOBBY_SAVE_PROMO_MASTER_REPLY			20032	// 46[s|4] - reqId,errCode,[errDescr|promoId]

// master lobby -> slave
#define MSG_ICE_LOBBY_SAVE_PROMO_SLAVE					20033	// 4o - reqId, IcePromoData()
#define MSG_ICE_LOBBY_SAVE_PROMO_SLAVE_REPLY			20034	// 46[s] - reqId,errCode,[errDescr]

// admin -> ice lobby or master ice lobby -> secondary ice lobby
#define MSG_ICE_LOBBY_RELOAD_COUNTRYREGIONS				20035	//	4 - reqId
#define MSG_ICE_LOBBY_RELOAD_COUNTRYREGIONS_REPLY		20036	//	46[s] - reqId,errCode,[errDescr]

// admin -> ice lobby and master lobby->slave lobbies (with checked tasks)
#define MSG_ICE_LOBBY_EXPIRE_PROMO_TASKS				20037	// 4tt44<4> - reqId,expireTime,unpublishTime,promoId,n<taskId>
#define MSG_ICE_LOBBY_EXPIRE_PROMO_TASKS_REPLY			20038	// 46[s] - reqId,errCode,[errDescr]

// master lobby ->slave lobbies 
#define MSG_ICE_LOBBY_GET_TOTALS						20039	// 4b - reqId,isFull
#define MSG_ICE_LOBBY_GET_TOTALS_REPLY					20040	// 46[s|4<o>] - reqId,errCode,[errDescr | n<IceGlobalDataData>] // IceTotalsState::composeForNotify

// lobby etc -> ice client proxy -> ice lobbies
#define MSG_ICE_LOBBY_FLUSH_TOTALS						20041	// 4b - reqId
#define MSG_ICE_LOBBY_FLUSH_TOTALS_REPLY				20042	// 46[s] - reqId,errCode,[errDescr]


//////////////////////////////////////////////////////////////////////////
// leader board messages

// ice lobby -> ice leader board server
#define MSG_ICE_LEADERBOARD_SEND_BOARDS_AND_SEASONS				30000	// 44<o4<o>>4<o> - reqId, boardCount<IceLeaderBoardData,count<IceLeaderBoardSeasonData>>
#define MSG_ICE_LEADERBOARD_SEND_BOARDS_AND_SEASONS_REPLY		30001	// 46[s] - reqId,errCode,[errDescr]

//#define available message										30002	// 4t4<o> - reqId, proxyTime, count<IceLeaderBoardUserUpdate>
//#define available message										30003	// 46[s] - reqId,errCode,[errDescr]

#define MSG_ICE_LEADERBOARD_USER_SEASON_UPDATE					30004	// 4o - reqId, IceLeaderBoardUserUpdate
#define MSG_ICE_LEADERBOARD_USER_SEASON_UPDATE_REPLY			30005	// 46[s] - reqId,errCode,[errDescr]

#define MSG_ICE_LEADERBOARD_USER_SEASON_ADDED					30006	// 4o - reqId, IceLeaderBoardUserUpdate
#define MSG_ICE_LEADERBOARD_USER_SEASON_ADDED_REPLY				30007	// 46[s] - reqId,errCode,[errDescr]

//#define available message										30008	// 44<o> - reqId, count<IceLeaderBoardUserUpdate>
//#define available message										30009	// 46[s] - reqId,errCode,[errDescr]

#define MSG_ICE_LEADERBOARD_USER_UNLOADED						30010	// 444 - reqId, numUsers<userIntId>
#define MSG_ICE_LEADERBOARD_USER_UNLOADED_REPLY					30011	// 46[s] - reqId,errCode,[errDescr]

// webapp -> iceclientproxy -> icelobby -> ice leader board
#define MSG_ICE_LEADERBOARD_USER_GET_BOARD_RANK					30012	// 444<4> - reqId, IceUserRequestData, n<boardId>
#define MSG_ICE_LEADERBOARD_USER_GET_BOARD_RANK_REPLY			30013	// 46[s|o] - reqId, errCode[errDescr|replyObject]

#define MSG_ICE_LEADERBOARD_SEASON_CLOSED						30014	// 44<o> - reqId, count<IceLeaderBoardSeasonData>
#define MSG_ICE_LEADERBOARD_SEASON_CLOSED_REPLY					30015	// 46[s] - reqId,errCode,[errDescr]

// icelobby -> ice leader board
#define MSG_ICE_LEADERBOARD_USER_GET_BOARD_SCORES				30016	// 4[M(o)] - reqId, [IceUserRequestData]
#define MSG_ICE_LEADERBOARD_USER_GET_BOARD_SCORES_REPLY			30017	// 46[s|o] - reqId, errCode[errDescr|replyObject]

// icelobby -> ice leader board
#define MSG_ICE_LEADERBOARD_GET_MAX_GLOBAL_UPDATE_ID			30018	// 4 - reqId
#define MSG_ICE_LEADERBOARD_GET_MAX_GLOBAL_UPDATE_ID_REPLY		30019	// 46[s|8] - reqId, errCode[errDescr|maxGlobalUpdateId]

#endif

