#ifndef friendserver_h_included
#define friendserver_h_included

#include "pyrtime.h"
#include "composecolls.h"

enum CliFrdUpdateType
{   
	efriendUpdateType_Remove			= 1,	// a friend is removed
	efriendUpdateType_AddOnline			= 2,	// an online friend is added
	efriendUpdateType_AddOffline		= 3,	// an offline friend is added
	efriendUpdateType_Online			= 4,	// a friend becomes online
	efriendUpdateType_Offline			= 5,	// a friend becomes offline
};

enum eFrdRelation
{
	eFrdRelation_none			= 0,
	eFrdRelation_isFriend		= 1,
	eFrdRelation_isBlocked		= 2,
	eFrdRelation_frdReqSent		= 3,
	eFrdRelation_frdReqReceived	= 4,
};


enum eScoreUpdateType //  FRSHIPSCOREAUDIT.UPDATETYPE
{	
	eScoreUpdate_SetDefault		= 0,
	eScoreUpdate_AddFrdReq		= 1,
	eScoreUpdate_FrdReqAccepted	= 2,
	eScoreUpdate_FrdReqRejected	= 3,
	eScoreUpdate_Unfriended		= 4,
	eScoreUpdate_Blocked		= 5, 
	eScoreUpdate_Unblocked		= 6,
	eScoreUpdate_AdminSetScore	= 7,
	eScoreUpdate_Challenger = 8,
	eScoreUpdate_Challengee = 9,
};

#define FRD_DELTA		1000

////////////////////////////////////////////////////////////////////////////////
// client -> friend server

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define	MSG_FRDSRV_GET_FRIEND_LIST					4001 //
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_FRDSRV_GET_FRIEND_LIST_REPLY			(MSG_FRDSRV_GET_FRIEND_LIST+FRD_DELTA)		// 5001 // 6[s|4<M(s4bbbb4)>] - errCode[errMsg | num<M(name, imageId, isonline_obsolete, hasPendingChallenge,isNewFriend, hasRMPendingChallenge, eDuelUserFlags)>]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define	MSG_FRDSRV_REMOVE_FRIEND					4002 // s userId
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_FRDSRV_REMOVE_FRIEND_REPLY				(MSG_FRDSRV_REMOVE_FRIEND+FRD_DELTA)		// 5002 // 6[s] - errCode[errMsg]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define	MSG_FRDSRV_GET_BLOCKED_LIST					4003 //
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_FRDSRV_GET_BLOCKED_LIST_REPLY			(MSG_FRDSRV_GET_BLOCKED_LIST+FRD_DELTA)		// 5003 // 6[s|4<M(s)>] - errCode[errMsg | num<M(name)>]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define	MSG_FRDSRV_UNBLOCK_USER						4004 // s userId
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_FRDSRV_UNBLOCK_USER_REPLY				(MSG_FRDSRV_UNBLOCK_USER+FRD_DELTA)			// 5004 // 6[s] - errCode[errMsg]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define	MSG_FRDSRV_GET_FRIEND_REQ_LIST				4005 //
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_FRDSRV_GET_FRIEND_REQ_LIST_REPLY		(MSG_FRDSRV_GET_FRIEND_REQ_LIST+FRD_DELTA)	// 5005 // 6[s|4<M(s4b4)>] - errCode[errMsg | num<M(name, imageId, isonline_obsolete, eDuelUserFlags)>]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define	MSG_FRDSRV_ADD_FRIEND_REQ					4006 // sb userId, throughUserSearch
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_FRDSRV_ADD_FRIEND_REQ_REPLY				(MSG_FRDSRV_ADD_FRIEND_REQ+FRD_DELTA)		// 5006 // 6[s] - errCode[errMsg]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define	MSG_FRDSRV_DENY_FRIEND_REQ					4007 // s userId
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_FRDSRV_DENY_FRIEND_REQ_REPLY			(MSG_FRDSRV_DENY_FRIEND_REQ+FRD_DELTA)		// 5007 // 6[s] - errCode[errMsg]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define	MSG_FRDSRV_ACCEPT_FRIEND_REQ				4008 // s userId
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_FRDSRV_ACCEPT_FRIEND_REQ_REPLY			(MSG_FRDSRV_ACCEPT_FRIEND_REQ+FRD_DELTA)	// 5008 // 6[s] - errCode[errMsg]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define	MSG_FRDSRV_BLOCK_USER						4009 // s userId
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_FRDSRV_BLOCK_USER_REPLY					(MSG_FRDSRV_BLOCK_USER+FRD_DELTA)			// 5009 // 6[s] - errCode[errMsg]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define	MSG_FRDSRV_CHECK_USER						4010 // s userId
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_FRDSRV_CHECK_USER_REPLY					(MSG_FRDSRV_CHECK_USER+FRD_DELTA)			// 5010 // 6[s|b44sb4] - errCode[errMsg|isOnline_obsolete, eFrdRelation, imageId, originalUserId, addFrdNotAllowed, eDuelUserFlags]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ		4011 // b - setNotRemove
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ_REPLY	(MSG_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ+FRD_DELTA) // 5011 6[s] - errCode[errMsg]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_FRDSRV_GET_RECENT_PLAYED_LIST			4012 //
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_FRDSRV_GET_RECENT_PLAYED_LIST_REPLY		(MSG_FRDSRV_GET_RECENT_PLAYED_LIST+FRD_DELTA) // 5012  // 6[s|4<M(s4b44)>] - errCode[errDescr|num<M(userId, imageId, isOnline_obsolete, eFrdRelation, eDuelUserFlags)>]

//	PYR-38260
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_FRDSRV_GET_SHARE_URL					4013	//	
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_FRDSRV_GET_SHARE_URL_REPLY				(MSG_FRDSRV_GET_SHARE_URL+FRD_DELTA)	//	5013	// 6[s|s] - errCode[errDescr|url]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_FRDSRV_USE_FRD_SHARE_TOKEN				4014 // s - token (note: token encodes userId and timestamp, which after validation, call processing for MSG_FRDSRV_ADD_FRIEND_REQ) 
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_FRDSRV_USE_FRD_SHARE_TOKEN_REPLY		(MSG_FRDSRV_USE_FRD_SHARE_TOKEN+FRD_DELTA) // 5014  // 6[s|s] - errCode[errMsg|tokenUserId]

////////////////////////////////////////////////////////////////////////////////
// friend server -> client, notification message
#define MSG_FRDSRV_NOTIF_CLI_FRIEND_UPDATE			4102 // 4s - friendUpdateType, userId

#define MSG_FRIEND_NOTIF_CLI_FRIEND_REQ_UPDATE		4103 // bs - addOrDrop, userId

////////////////////////////////////////////////////////////////////////////////
// duel lobby -> friend server
#define MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP          4201 // 4ss - reqId, userId1, userId2
#define MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP_REPLY    (MSG_FRDSRV_DLOBBY_CHECK_FRIENDSHIP+FRD_DELTA)	 // 5201 // 46[s|b] - reqId, errCode[errMsg|isFriend]

#define MSG_FRDSRV_DLOBBY_NOTIF_FRIENDSHIP_UPDATE   4202 // 4ssb - reqId, userId1, userId2, addOrRemove

#define MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE			4203 // 4s - reqId, userId
#define MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE_REPLY     (MSG_FRDSRV_DLOBBY_GET_OWN_PROFILE+FRD_DELTA)	 // 5203 // 46[s|4] - reqId,errCode[errMsg|numFriend]

#define MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE		4204 // 4ss - reqId, userId1, userId2
#define MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE_REPLY  (MSG_FRDSRV_DLOBBY_GET_OTHERS_PROFILE+FRD_DELTA)	 // 5204 // 46[s|4b4] - reqId, errCode[errMsg|numFriend, isOnline, eFrdRelation]

//	PYR-37201
#define MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED		4205	//	4ss - reqId, challengerUserId, challengeeUserId
#define MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED_REPLY  (MSG_FRDSRV_DLOBBY_CHALLENGE_ACCEPTED+FRD_DELTA)	 // 5205 // 46[s] - reqId, errCode[errMsg]

//	PYR-41110
#define MSG_FRDSRV_DLOBBY_CHALLENGE_SENT			4206	//	4ss - reqId, challengerUserId, challengeeUserId
#define MSG_FRDSRV_DLOBBY_CHALLENGE_SENT_REPLY		(MSG_FRDSRV_DLOBBY_CHALLENGE_SENT+FRD_DELTA)	 // 5206 // 46[s] - reqId, errCode[errMsg]

////////////////////////////////////////////////////////////////////////////////
// admin -> friend server
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_FRDSRV_ADMIN_SET_FRIEND_SCORE           4301 // 4s4 - reqId, userId, score
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_FRDSRV_ADMIN_SET_FRIEND_SCORE_REPLY     (MSG_FRDSRV_ADMIN_SET_FRIEND_SCORE+FRD_DELTA)	// 5301 // 46[s] - reqId, errCode[errMsg]

#define	MSG_FRDSRV_ADMIN_CREATE_FRIENDSHIP			4302 // 4ss - reqId, userId1, userId2
#define MSG_FRDSRV_ADMIN_CREATE_FRIENDSHIP_REPLY	(MSG_FRDSRV_ADMIN_CREATE_FRIENDSHIP+FRD_DELTA)	// 5302 // 46[s] - reqId, errCode[errMsg]

#define	MSG_FRDSRV_ADMIN_REMOVE_FRIEND				4303 // 4ssb - reqId, userId1, userId2, updateScore
#define MSG_FRDSRV_ADMIN_REMOVE_FRIEND_REPLY		(MSG_FRDSRV_ADMIN_REMOVE_FRIEND+FRD_DELTA)		// 5303 // 46[s] - reqId, errCode[errMsg]

#define	MSG_FRDSRV_ADMIN_UNBLOCK_USER				4304 // 4ssb - reqId, userId1, userId2, updateScore
#define MSG_FRDSRV_ADMIN_UNBLOCK_USER_REPLY			(MSG_FRDSRV_ADMIN_UNBLOCK_USER+FRD_DELTA)		// 5304 // 46[s] - reqId, errCode[errMsg]

#define	MSG_FRDSRV_ADMIN_ADD_FRIEND_REQ				4305 // 4ss - reqId, userId1, userId2
#define MSG_FRDSRV_ADMIN_ADD_FRIEND_REQ_REPLY		(MSG_FRDSRV_ADMIN_ADD_FRIEND_REQ+FRD_DELTA)		// 5305 // 46[s] - reqId, errCode[errMsg]

#define	MSG_FRDSRV_ADMIN_DENY_FRIEND_REQ			4306 // 4ssb - reqId, userId1, userId2, updateScore
#define MSG_FRDSRV_ADMIN_DENY_FRIEND_REQ_REPLY		(MSG_FRDSRV_ADMIN_DENY_FRIEND_REQ+FRD_DELTA)	// 5306 // 46[s] - reqId, errCode[errMsg]

#define	MSG_FRDSRV_ADMIN_ACCEPT_FRIEND_REQ			4307 // 4ssb - reqId, userId1, userId2, updateScore
#define MSG_FRDSRV_ADMIN_ACCEPT_FRIEND_REQ_REPLY	(MSG_FRDSRV_ADMIN_ACCEPT_FRIEND_REQ+FRD_DELTA)	// 5307 // 46[s] - reqId, errCode[errMsg]

#define	MSG_FRDSRV_ADMIN_BLOCK_USER					4308 // 4ssb - reqId, userId1, userId2, updateScore
#define MSG_FRDSRV_ADMIN_BLOCK_USER_REPLY			(MSG_FRDSRV_ADMIN_BLOCK_USER+FRD_DELTA)			// 5308 // 46[s] - reqId, errCode[errMsg]


// move to friend dbm definition later

enum eFrshipReqStatus // FRSHIPREQUESTS.STATUS
{
	eFrshipReqStatus_Active			= 0, // pending friendship request (the only ones we keep in ROOT DB)
	eFrshipReqStatus_Accepted		= 1,
	eFrshipReqStatus_Rejected		= 2,
	eFrshipReqStatus_Blocked		= 3, // rejected because the other user already had a block against the first user
	eFrshipReqStatus_Unfriended		= 4, // user1 unfriended user2
};

enum eFriendFlag //FRSHIPUSERS.FRIENDFLAGS
{
	eFriendFlag_BlockAllFrdReq = (1 << 0), // 1
};

enum eFriendshipFlags //FRIENDSHIPS.STATUSFLAGS1 and STATUSFLAGS2
{
	eFriendshipFlags_HasPendingAction	= (1 << 0), // 1
};

#define DBM_Q_FRDSRV_LOAD_USER_DATA						6001 // 44 - reqId, userIntId
#define DBM_A_FRDSRV_LOAD_USER_DATA						(FRD_DELTA + DBM_Q_FRDSRV_LOAD_USER_DATA) //7001 // 46[s|4<sb>4<s>4<s>4b4] - reqId, errCode[errMsg | num<friendUserId,hasNewFriendPendingAction>num<friendReqUserId>num<blockedUserId>flags,hasScore,score]

#define DBM_Q_FRDSRV_REMOVE_FRIEND						6002 // 44s - reqId, userIntId, userId2
#define DBM_A_FRDSRV_REMOVE_FRIEND						(FRD_DELTA + DBM_Q_FRDSRV_REMOVE_FRIEND) //7002 // 46[s] - reqId, errCode[errMsg]

#define DBM_Q_FRDSRV_UNBLOCK_USER						6003 // 44s - reqId, userIntId, userId2
#define DBM_A_FRDSRV_UNBLOCK_USER						(FRD_DELTA + DBM_Q_FRDSRV_UNBLOCK_USER) //7003 // 46[s] - reqId, errCode[errMsg]

#define DBM_Q_FRDSRV_ADD_FRIEND_REQ						6004 // 4s4s4 - reqId, userId1, userIntId1, userId2, userIntId2
#define DBM_A_FRDSRV_ADD_FRIEND_REQ						(FRD_DELTA + DBM_Q_FRDSRV_ADD_FRIEND_REQ) //7004 // 46[s] - reqId, errCode[errMsg]

#define DBM_Q_FRDSRV_DENY_FRIEND_REQ					6005 // 4s4s - reqId, userId1, userIntId1, userId2
#define DBM_A_FRDSRV_DENY_FRIEND_REQ					(FRD_DELTA + DBM_Q_FRDSRV_DENY_FRIEND_REQ) //7005 // 46[s] - reqId, errCode[errMsg]


// expected bahavior from dbm: 		addFriend(userId1, userId2); unblockUser(userId1, userId2);removeFriendReq(userId1, userId2); 
//									Also set the flag that userId2 has a new friend pending action from userId1
#define DBM_Q_FRDSRV_ACCEPT_FRIEND_REQ					6006 // 4s4s4 - reqId, userId1, userIntId1, userId2, userIntId2
#define DBM_A_FRDSRV_ACCEPT_FRIEND_REQ					(FRD_DELTA + DBM_Q_FRDSRV_ACCEPT_FRIEND_REQ) //7006 // 46[s] - reqId, errCode[errMsg]

// expected behavior from dbm: 	blockUser(userId1, userId2); removeFriendReq(userId1, userId2); removeFriendReq(userId2, userId1); removeFriend(userId1, userId2);
#define DBM_Q_FRDSRV_BLOCK_USER							6007 // 4s4 - reqId, userId1, userIntId1, userId2, userIntId2 
#define DBM_A_FRDSRV_BLOCK_USER							(FRD_DELTA + DBM_Q_FRDSRV_BLOCK_USER) //7007 // 46[s] - reqId, errCode[errMsg]

#define DBM_Q_FRDSRV_SET_USER_FRIEND_SCORE				6008 // 4s44 - reqId, userId, updateType, score
#define DBM_A_FRDSRV_SET_USER_FRIEND_SCORE				(FRD_DELTA + DBM_Q_FRDSRV_SET_USER_FRIEND_SCORE) //7008 // 46[s] - reqId, errCode[errMsg]

#define DBM_Q_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ			6009 // 4sb - reqId, userId, setNotRemove
#define DBM_A_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ			(FRD_DELTA + DBM_Q_FRDSRV_SET_USER_BLOCK_ALL_FRD_REQ) //7009 // 46[s] - reqId, errCode[errMsg]

// remove the flags where userId1 has new friend pending actions from a list of userIds
#define DBM_Q_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION	6010 // 4s44<s> - reqId, userId, userIntId, num<userIds>
#define DBM_A_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION	(FRD_DELTA + DBM_Q_FRDSRV_REMOVE_NEW_FRIEND_PENDING_ACTION) //7010 // 46[s] - reqId, errCode[errMsg]

#define DBM_Q_FRDSRV_GET_FRDREQ_INFO					6011 // 444<s> - reqId, userId_sender, userIntId_sender, num<userIds_receiver>
#define DBM_A_FRDSRV_GET_FRDREQ_INFO					(FRD_DELTA + DBM_Q_FRDSRV_GET_FRDREQ_INFO) //7011 // 46[s|4<4>] - reqId, errCode[errMsg|num<reqSent>]

// expected behavior from dbm: 	addFriend(userId1, userId2); removeFriendReq(userId1, userId2); removeFriendReq(userId2, userId1);
#define	DBM_Q_FRDSRV_ADMIN_CREATE_FRIENDSHIP			6012 // 4s4s4 - reqId, userId1, userIntId1, userId2, userIntId2
#define DBM_A_FRDSRV_ADMIN_CREATE_FRIENDSHIP			(FRD_DELTA + DBM_Q_FRDSRV_ADMIN_CREATE_FRIENDSHIP)	// 7012 // 46[s] - reqId, errCode[errMsg]


#endif // friendserver_h_included
