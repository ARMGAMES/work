// duelnotifserver.h
#ifndef duelnotifserver_h_included
#define duelnotifserver_h_included

#include "pyrtime.h"
#include "composecolls.h"

#define DNSERVER_DELTA		1000

// Note to acronyms: DN - Duel Notification, PA - Pending Action

// client -> DN Server (triggered by client request)
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define	MSG_DNSERVER_CLI_GET_PENDING_ACTIONS			4001 //
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_DNSERVER_CLI_GET_PENDING_ACTIONS_REPLY (MSG_DNSERVER_CLI_GET_PENDING_ACTIONS+DNSERVER_DELTA)	// 5001 // 6[s|4<48ssstt>] - errCode[errMsg | num<type, duelId, oppUserId, param1, param2, startTime, expiryTime>]

// DN Server -> PA server modules that hold user's pending actions
#define MSG_DNSERVER_SRV_GET_PENDING_ACTIONS			4002 // 4s4 - reqId, userId, siteId
#define MSG_DNSERVER_SRV_GET_PENDING_ACTIONS_REPLY (MSG_DNSERVER_SRV_GET_PENDING_ACTIONS+DNSERVER_DELTA)	// 5002 // 46[s|4<48ssstt>] - reqId, errCode[errMsg | num<type, duelId, oppUserId, param1, param2, startTime, expiryTime>]

// DN Server -> PA server modules that hold user's pending actions
#define MSG_DNSERVER_SRV_GET_PENDING_ACTIONS_COUNT		4003 // 4s4 - reqId, userId, siteId
#define MSG_DNSERVER_SRV_GET_PENDING_ACTIONS_COUNT_REPLY (MSG_DNSERVER_SRV_GET_PENDING_ACTIONS_COUNT+DNSERVER_DELTA)	// 5003 // 46[s|4] - reqId, errCode[errMsg | num]

// PA server -> DN Server, notification only
#define MSG_DNSERVER_SRV_NOTIF_PENDING_ACTION			4004 // 4s4b48ssstt - reqId=0, userId, siteIdMask, addNotDrop, type, duelId, oppUserId, param1, param2, startTime, expiryTime

// DN Server -> Client (triggered by pending action update from other server modules)
#define MSG_DNSERVER_NOTIF_CLI_PENDING_ACTION			4005 // b48ssstt - addNotDrop, type, duelId, oppUserId, param1, param2, startTime, expiryTime

enum DuelNotifType
{                                           //                           param1				param2
	eDuelNotifType_Invalid			= 0,
	eDuelNotifType_YourTurn			= 1,	// PN parameter: oppUserId											- "It is your turn to act against Player1"
	eDuelNotifType_YourTurnReminder	= 2,	// PN parameter: oppUserId, timeRemain (in mins)					- "You have 20 hours to act against Player1"
	eDuelNotifType_DuelComplete		= 3,	// PN parameter: oppUserId											- "Your game with Player1 is over, check out the result"
	eDuelNotifType_DuelChallenge	= 4,	// PN parameter: oppUserId, amount (in cents)	currency (e.g. EUR)	- "Player1 want to play you for $10"
	eDuelNotifType_FriendRequest	= 5,	// PN parameter: oppUserId											- "You have a friend request from Player1"
	eDuelNotifType_NewFriend		= 6,	// PN parameter: oppUserId											- "Player1 is now your friend"
	eDuelNotifType_RMChallengeFail  = 7,	// PN parameter: oppUserId, amount (in cents)	currency (e.g. EUR)	- "Your challenge to <oppUserId> was not accepted and your buyin has been refunded to your account."
	eDuelNotifType_RMDuelWinNoReveal = 8,	// PN parameter: oppUserId, amount (in cents)	currency (e.g. EUR)	- "You won your duel against <oppUserId> and <winamount> was credited to your account."
											//   -> player has won their real money duel but has not viewed their last reveal
	eDuelNotifType_RMDuelDrawNoReveal = 9,	// PN parameter: oppUserId, amount (in cents)	currency (e.g. EUR)	- "You duel against <oppUserId> was a draw and <amount> was credited to your account."
};

class DuelNotifUtils
{
public:
	// The notification in the following types is for one time push notification only (no email). It won't update the badge number either. It should not come from getPendingActions either.
	static bool isOneTimePN(DuelNotifType type)
	{
		return type == eDuelNotifType_RMChallengeFail || type == eDuelNotifType_RMDuelWinNoReveal || type == eDuelNotifType_RMDuelDrawNoReveal; 
	}
};

struct DuelPendingAction
{
	DuelNotifType type;
	UINT64 duelId; // for eDuelNotifType_DuelChallenge however, it is duelTypeId.
	PString oppUserId;	// opponent user id
	PString param1;		// extra parameter see comment in DuelNotifType
	PString param2;		// extra parameter see comment in DuelNotifType
	SrvTime startTime;
	SrvTime expiryTime;

	DuelPendingAction()
		: type( eDuelNotifType_Invalid )
		, duelId( 0 )
	{
	}

	CommMsgBody& compose(CommMsgBody& body) const
	{
		body
			.composeUINT32(type)
			.composeUINT64(duelId)
			.composeString(oppUserId)
			.composeString(param1)
			.composeString(param2)
			.composeSrvTime(startTime)
			.composeSrvTime(expiryTime);
		return body;
	}

	CommMsgParser& parse(CommMsgParser& parser)
	{
		parseEnum32asUINT32(parser, type);
		parser
			.parseUINT64(duelId)
			.parseStringP(oppUserId)
			.parseStringP(param1)
			.parseStringP(param2)
			.parseSrvTime(startTime)
			.parseSrvTime(expiryTime);
		return parser;
	}
};


#endif // duelnotifserver_h_included
