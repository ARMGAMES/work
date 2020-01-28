// Copyright (C) 2010 PYR Software Ltd. All rights reserved
// homeclub.h

#ifndef homelobbymsgs_h_included
#define homelobbymsgs_h_included


//////////////////////////////////////////////////////////////////////////////////////
// Client messages
//////////////////////////////////////////////////////////////////////////////////////

#define HG_CREATE_CLUB				    100  // ss - club name, password
#define HG_CREATE_CLUB_REPLY		    101  // 6[s | 4s] - errCode , [errDescr | clubId, socialToken]

#define HG_GET_USER_CLUBS				104  // [(empty)|b] - if not empty, isNewBehavior 
#define HG_GET_USER_CLUBS_REPLY			105  // 6[s|o] - errCode, [errDescr | HgClubInfoListForClient(parser)]

#define HG_ADD_USER_CLUB                106  // o - HgAddUserToClubParameters()
#define HG_ADD_USER_CLUB_REPLY          107  // 6s[s] - errCode, errDescr | clubName [, socialToken]

#define HG_GET_PLAYERS_BASED_ON_STATUS        108  // 44[4...] - clubId, nbStatus, HomeGamesMembershipStatus (see hgdbmshared.h)
#define HG_GET_PLAYERS_BASED_ON_STATUS_REPLY  109  // 6[errStr|o], errCode, [errDescr | HgPlayersList(parser)]

#define HG_CHANGE_PLAYERS_STATUS        110  // o - HgChangePlayersStatusParams(Parser)
#define HG_CHANGE_PLAYERS_STATUS_REPLY  111  // 6s, HgPlayerStatusError, errDescr | socialToken - social token will be empty if no users were approved

#define HG_CHANGE_CLUB_PASSWORD         118  // 4s - clubId, passwd
#define HG_CHANGE_CLUB_PASSWORD_REPLY   119  // 6[s] - errCode [, errDescr]

#define HG_CHANGE_CLUB_OPTIONS          120  // o - HgChangeClubOptionsParams(parser)
#define HG_CHANGE_CLUB_OPTIONS_REPLY    121  // 6[s] - errCode [, errDescr]

#define HG_CHANGE_CLUB_IMAGE            122  // 4Bb - clubId, image, isHeader
#define HG_CHANGE_CLUB_IMAGE_REPLY      123  // 6[s] - errCode [, errDescr]

#define HG_GET_CLUB_SEASONS             124	// 4 - clubId
#define HG_GET_CLUB_SEASONS_REPLY       125	// 6[s | 4<M(4ttb)>] - err [, errDescr | 
//	numSeasons, <M(seasonId,timeFrom,timeTo,isClosed)>]

#define HG_GET_CLUB_SEASON_RANKS        126	// 44 - clubId, seasonId
#define HG_GET_CLUB_SEASON_RANKS_REPLY  127	// 6[s | o] - err [, errDescr | HgClubSeasonRanks]

#define HG_GET_CLUB_SEASON_STATS        128	// 44b - clubId, seasonId, playMoney
#define HG_GET_CLUB_SEASON_STATS_REPLY  129	// 6[s | 4<M(ss4444444484444)>] - err [, errDescr | 
//		numUsers, <M(userId,userCity,qualifiedGames,
//					bestTournPlace,bestTournPlaceTo,playersInBestTourn,
//					worstTournPlace,worstTournPlaceTo,playersinWorstTourn,
//					inTheMoney,
//					points,
//					goldFinishes,silverFinishes,bronzeFinishes,
//					averagePlace)>]
// if seasonId is 0 - we'll get the latest open season
// if no tournaments that started in that season have finished
// the result will be an empty vector

#define HG_GET_RECENT_TOURN_SUMMARIES		130	// 4 - clubId
#define HG_GET_RECENT_TOURN_SUMMARIES_REPLY	131	// 6[s | 4<M(4stt484b)>] - err [, errDescr | 
//		numTourns, <M(tournId, tournName, timeStarted, timeClosed, numPlayers, prizePool, seasonId, isCounting)>]

#define HG_GET_RECENT_TOURN_RESULTS			132	// 4 - tournId
#define HG_GET_RECENT_TOURN_RESULTS_REPLY	133	// 6[s | 4<M(s4444)>b] - err [, errDescr | 
//		numPlayers<M(userId, place, placeTo, prize, tournSortOrder - already sorted low to high)>, isCounting]

#define HG_SET_CLUB_COLOR					134 // 44 - clubId, color
#define HG_SET_CLUB_COLOR_REPLY				135 // 6[s] - errCode [, errStr]

#define HG_CHANGE_CLUB_NAME					136  // 4s - clubId, clubName
#define HG_CHANGE_CLUB_NAME_REPLY			137  // 6[s] - errCode [, errDescr]

#define HG_GET_CLUB_OPTIONS					138 // 4 - clubId
#define HG_GET_CLUB_OPTIONS_REPLY			139 // 6[s | o] - errCode [, errDescr]
//		HgChangeClubOptionsParams(parser)

#define HG_GET_CLUB_TOURN_STRUCTURES		140  // 4s - clubId
#define HG_GET_CLUB_TOURN_STRUCTURES_REPLY	141  // 6[s | o4] - errCode [, errDescr | HgTournStructureLimits, max days tourn creation]

#define HG_GET_CLUB_TABLE_STRUCTURES		142  // 4s - clubId
#define HG_GET_CLUB_TABLE_STRUCTURES_REPLY	143  // 6[s | o] - errCode [, errDescr | HgTableParamsVariations]

#define HG_CREATE_CLUB_TOURN				144  // o[o] - HgCreateTournParams(parser), HgRecurringTourament(parser)
#define HG_CREATE_CLUB_TOURN_REPLY			145  // 6[s | s4s] - errCode , errDescr | socialToken, tournId, restartWarning

#define HG_CREATE_CLUB_TABLE				146  // 4s - HgCreateTableParams
#define HG_CREATE_CLUB_TABLE_REPLY			147  // 6[s|s4s8] - errCode , [errDescr | socialToken, tableId32, "" - empty restart warning, tableId64]

#define HG_UPDATE_MEMBERSHIP_FLAGS			148  // 4s44 - clubId, userId, flags, mask
#define HG_UPDATE_MEMBERSHIP_FLAGS_REPLY	149  // 6[s] - err [, errDescr]

#define HG_CANCEL_CLUB_TOURNAMENT           150  // 44[b] - clubid, tourn id, delete_all_occurences
#define HG_CANCEL_CLUB_TOURNAMENT_REPLY     151  // 6[s] - err [, errDescr]

#define HG_CANCEL_CLUB_TABLE                152  // 448 - club id, tableId32, tableId64
#define HG_CANCEL_CLUB_TABLE_REPLY          153  // 6[s] - err [, errDescr]

#define HG_TRANSLATE_SOCIAL_EVENT           154  // s - social event token 
#define HG_TRANSLATE_SOCIAL_EVENT_REPLY     155  // 6[s] - err [, errDescr]

#define HG_GET_CLUB_TABLE_INFO			    156  // 4448 - tableId tournId32 clubId tableId64
#define HG_GET_CLUB_TABLE_INFO_REPLY		157  // 6[s|sss11] - err [, errDescr | server, serverObj, clubPublName, game, table size, member status, member flags]

#define HG_GET_CLUB_TOURN_INFO			    158  // 44 - tournId clubId
#define HG_GET_CLUB_TOURN_INFO_REPLY		159  // 6[s|sss] - err [, errDescr | server, serverObj, clubPublName, member status, member flags ]

#define HG_GET_CLUB_INFO				    160  // 4 - clubId
#define HG_GET_CLUB_INFO_REPLY				161  // 6[s|sss44sss] - err [, errDescr | clubPublName, tablePublName, tournPublName, member status, member flags, wallPublName, anncmtPublName, membershipPubl]

#define HG_GET_SCHEDULED_GAMES              162  //
#define HG_GET_SCHEDULED_GAMES_REPLY		163  // 6[s|4<M>4<M>] 
// err, [errDescr | #tourns, <M(ScheduledTournament)>, #games, <M(ScheduledRingGame)>] // ScheduledRingGame now has tableId64 at the end

#define HG_FOUNDER_GET_CLUB_PASSWORD		164  // clubId
#define HG_FOUNDER_GET_CLUB_PASSWORD_REPLY	165	 //6s err, errDescr | password

#define HG_GET_RECRUIT_SOCIAL_TOKEN			166  // clubId
#define HG_GET_RECRUIT_SOCIAL_TOKEN_REPLY	167	 //6s err, errDescr | socialToken

#define HG_CLOSE_CLUB_BY_FOUNDER			168  // clubId
#define HG_CLOSE_CLUB_BY_FOUNDER_REPLY		169	 //6s err, errDescr

#define HG_CHANGE_SEASON_NAME				170  // 44s clubId,seasonId,seasonName
#define HG_CHANGE_SEASON_NAME_REPLY			171	 //6s err, errDescr

#define HG_PROCESS_SEASON_CLOSED			172  // 44 clubId,seasonId
#define HG_PROCESS_SEASON_CLOSED_REPLY		173	 //6s err, errDescr

#define HG_MODIFY_MEMBERSHIP_FLAGS          174  // 44 - clubId, flags
#define HG_MODIFY_MEMBERSHIP_FLAGS_REPLY    175  // 6[s] - err, [errDescr]

// PYR-17781
#define HG_SET_FAVORITE_CLUB                176  // 4b - clubId, isFavoriteClub
#define HG_SET_FAVORITE_CLUB_REPLY          177  // 6[s] - err, [errDescr]

#define HG_TABLE_SOCIAL_TOKEN               178  // 448 - clubId, tableId32, tableId64
#define HG_TABLE_SOCIAL_TOKEN_REPLY         179  // 6s - err, errDescr | socialToken

#define HG_TOURN_SOCIAL_TOKEN               180  // 44 - clubId, tournId
#define HG_TOURN_SOCIAL_TOKEN_REPLY         181  // 6s - err, errDescr | socialToken

#define HG_SAVE_CLUB_MESSAGE				182  // 44o - HgClubMessageData(parser)
#define HG_SAVE_CLUB_MESSAGE_REPLY			183  // 6[s|o] - err, errDescr | HgClubMessageData(parser)

#define HG_CLUB_LOGIN						184  // clubId
#define HG_CLUB_LOGIN_REPLY					185  // 6[s] - err, errDescr

#define HG_CLUB_LOGOUT						186  // clubId
#define HG_CLUB_LOGOUT_REPLY				187  // 6[s] - err, errDescr

#define HG_UPDATE_MEMBERSHIP_FLAGS2			188  // 4444<s> - clubId, flags, mask, numUsers<userId>
#define HG_UPDATE_MEMBERSHIP_FLAGS2_REPLY	189  // 6[s] - err [, errDescr]

#define HG_CREATE_CLUB_TOURN2				190  // M(o)[M(o)] - M(HgCreateTournParams),[M(HgRecurringTourament)]
#define HG_CREATE_CLUB_TOURN_REPLY2			191  // 6[s | s4s] - errCode , errDescr | socialToken, tournId, restartWarning

#define HG_GET_CLUBS_AND_SCHEDULED_GAMES		192  //
#define HG_GET_CLUBS_AND_SCHEDULED_GAMES_REPLY	193  // 6[s|o4<M>4<M>] - HgClubInfoListForClient(parser), #tourns, <M(ScheduledTournament)>, #games, <M(ScheduledRingGame)> // ScheduledRingGame now has tableId64 at the end

//	PYR-24340, Dbd to HgLobby.  Set a vector of clubIds as archive pending
#define HG_SET_ARCHIVE_PENDING					194	//	t4<4> - archive date, numClubs<clubIds>
#define HG_SET_ARCHIVE_PENDING_REPLY			195	//	6[s]

//	PYR-24340, Dbd to HgLobby.  Set a vector of clubIds as archived
#define HG_ARCHIVE_CLUBS						196	//	4<4> - numClubs<clubIds>
#define HG_ARCHIVE_CLUBS_REPLY					197	//	6[s]

//  PYR-40177, include play money scaled stakes
#define HG_GET_CLUB_TABLE_STRUCTURES2			198  // 4s - clubId
#define HG_GET_CLUB_TABLE_STRUCTURES_REPLY2		199  // 6[s | o] - errCode [, errDescr | HgTableParamsVariations]

//  PYR-40177, include play money scaled stakes
#define HG_GET_CLUBS_AND_SCHEDULED_GAMES2		200  //
#define HG_GET_CLUBS_AND_SCHEDULED_GAMES_REPLY2	201  // 6[s|o4<M>4<M>] - HgClubInfoListForClient(parser), #tourns, <M(ScheduledTournament)>, #games, <M(ScheduledRingGame)> // ScheduledRingGame now has scalePM at the end

//  PYR-40177, include play money scaled stakes
#define HG_GET_SCHEDULED_GAMES2			        202  //
#define HG_GET_SCHEDULED_GAMES_REPLY2			203  // 6[s|4<M>4<M>] 

//  PYR-40177, include play money scaled stakes
#define HG_GET_CLUB_TOURN_STRUCTURES2			204  //
#define HG_GET_CLUB_TOURN_STRUCTURES_REPLY2		205  // 6[s|4<M>4<M>] 

//  PYR-40177, include play money scaled stakes
#define HG_CREATE_CLUB_TOURN3					206  // M(o)[M(o)] - M(HgCreateTournParams),[M(HgRecurringTourament)]
#define HG_CREATE_CLUB_TOURN_REPLY3				207  // 6[s | s4s] - errCode , errDescr | socialToken, tournId, restartWarning

//  PYR-40177, include play money scaled stakes
#define HG_GET_RECENT_TOURN_SUMMARIES2			208	// 4 - clubId
#define HG_GET_RECENT_TOURN_SUMMARIES_REPLY2	209	// 6[s | 4<M(4stt484b)>] - err [, errDescr | 

//  PYR-40177, include play money scaled stakes
#define HG_GET_RECENT_TOURN_RESULTS2			210	// 4 - tournId
#define HG_GET_RECENT_TOURN_RESULTS_REPLY2		211	// 6[s | 4<M(s4444)>b] - err [, errDescr | 
//		numPlayers<M(userId, place, placeTo, prize, tournSortOrder - already sorted low to high)>, isCounting]

// PYR-40177 Home Games - Add scaling and higher PM Stakes
#define HG_GET_CLUB_INFO2						212 // 4 - clubId
#define HG_GET_CLUB_INFO_REPLY2					213  // 6[s|sss44sss] - err [, errDescr | clubPublName, tablePublName, tournPublName, member status, member flags, wallPublName, anncmtPublName, membershipPubl]


////////////////////////////////////////////////////////////////////////////////////
// Admin messages
////////////////////////////////////////////////////////////////////////////////////

#define HG_ADMIN_APPROVE_CLUB_NAME_IMAGE        1000  // 4o - reqId, HgClubNameImageApprovalParamsList(parser)
#define HG_ADMIN_APPROVE_CLUB_NAME_IMAGE_REPLY  1001  // 46[s] - reqId, errCode [, errDescr]

#define HG_ADMIN_SAVE_PROPS                     1002  // 4o - reqId, HgEntityProps(parser)
#define HG_ADMIN_SAVE_PROPS_REPLY               1003  // 46[s] - reqId, errCode [, errDescr]

#define HG_ADMIN_CHANGE_CLUB_STATUS             1004  // 4o - reqId, HgChangeClubStatusParams(parser)
#define HG_ADMIN_CHANGE_CLUB_STATUS_REPLY       1005  // 46[s] - reqId, errCode [, errDescr]

#define HG_ADMIN_UPDATE_CLUB					1006  // 4o - reqId, HgClub(parser)
#define HG_ADMIN_UPDATE_CLUB_REPLY				1007  // 46[s] - reqId, errCode [, errDescr ]

#define HG_ADMIN_TRANSFER_FOUNDER_STATUS		1008  // 44s - reqId, clubId, userId
#define HG_ADMIN_TRANSFER_FOUNDER_STATUS_REPLY	1009  // 46[s] - q_id, err [, errDescr]

#define HG_ADMIN_ADD_COMMENT					1010 // 44s - reqId, HgComment
#define HG_ADMIN_ADD_COMMENT_REPLY				1011 // 46[s] - q_id, err [, errDescr ]

#define HG_ADMIN_GET_CLUB_MEMBERSHIPS			1012 // 44s - reqId, clubId
#define HG_ADMIN_GET_CLUB_MEMBERSHIPS_REPLY		1013 // 46[s | o] - q_id, err [, errDescr | HgClubMemberships]

#define HG_ADMIN_REREAD_INI						1014 // 44s - reqId
#define HG_ADMIN_REREAD_INI_REPLY				1015 // 46[s] - q_id, err [, errDescr]

#define HG_ADMIN_UPDATE_MEMBERSHIP_FLAGS		1016 // 44s4 - reqId, clubId, userId, flags
#define HG_ADMIN_UPDATE_MEMBERSHIP_FLAGS_REPLY	1017 // 46[s] - q_id, err [, errDescr]

#define HG_ADMIN_ADD_GUEST_USER					1018  // 44sDD - reqId, clubId, userId, start date, end date             
#define HG_ADMIN_ADD_GUSET_USER_REPLY           1019  // 46[s] - q_id, errCode [, errDescr]

#define HG_ADMIN_CANCEL_CLUB_TOURNAMENT         1020  // 444 - reqId, clubid, tourn id
#define HG_ADMIN_CANCEL_CLUB_TOURNAMENT_REPLY   1021  // 46[s] - q_id, err [, errDescr]

#define HG_ADMIN_CANCEL_CLUB_TABLE              1022  // 444 - reqId, club id, table id
#define HG_ADMIN_CANCEL_CLUB_TABLE_REPLY        1023  // 46[s] - q_id, err [, errDescr]

#define HG_ADMIN_SET_SHOWCASE_CLUB              1024  // 44bb44<s>4[m] - reqId, club id, is_public, is_global, site_mask, #countries, <country>, nameColor, [site_mask_ex]
#define HG_ADMIN_SET_SHOWCASE_CLUB_REPLY        1025  // 46[s] - q_id, err [, errDescr]

#define HG_ADMIN_ORDER_SHOWCASE_CLUBS           1026  // 44<44> - reqId, #clubs, <club id, ordinal>
#define HG_ADMIN_ORDER_SHOWCASE_CLUBS_REPLY     1027  // 46[s] - q_id, err [, errDescr]

#define HG_ADMIN_CREATE_TOURNAMENT              1028  // 4o - reqId, TournamentScript(parser)
#define HG_ADMIN_CREATE_TOURNAMENT_REPLY        1029  // 46[s|44 4(s6s)s] - q_id, error_code[errDescr | tournId, timeLeft, numErrUsrs(UserId, error_code, errDescr), warning]

#define HG_ADMIN_RECREATE_TOURNAMENT            1030  // 44o - reqId, tournId, TournamentScript(parser)
#define HG_ADMIN_RECREATE_TOURNAMENT_REPLY      1031  // 46[s|44] - q_id, error_code[errDescr | tournId, timeLeft]


////////////////////////////////////////////////////////////////////////////////////
// Unauthorized messages
////////////////////////////////////////////////////////////////////////////////////

#define HG_GET_CLUB_APPROVED_IMAGES_DELTA        2000  // T - time stamp
#define HG_GET_CLUB_APPROVED_IMAGES_DELTA_REPLY  2001  // 6[s|T4<44>] - err [, errDescr | currentTime, nbMappings, <club id, image id> ]

//////////////////////////////////////////////////////////////////////////
// login dbm messages

#define HG_INFORM_USER_LOGIN					2002  // 4s4b reqId, userId, userIntId, isLoggedIn
#define HG_INFORM_USER_LOGIN_REPLY				2003  // 46[s] - reqId, err [, errDescr]

// PYR-55889: HGLobby needs to load unarchived players since they would have been skipped when it first started up
#define HG_INFORM_USER_UNARCHIVED				2004  // 44s reqId, userIntId, userId
#define HG_INFORM_USER_UNARCHIVED_REPLY			2005  // 46[s] reqId, err [errDescr]

#endif // homelobbymsgs_h_included
