// ===========================================================
//  Copyright (c) 2019 PokerStars. All rights reserved.
// ===========================================================

#ifndef twitchgateway_h_included
#define twitchgateway_h_included

#define TWITCH_ERROR_BASE				1000
#define TWITCH_ERROR_OPERATION_BLOCKED	(TWITCH_ERROR_BASE + 1)		// 1001
#define TWITCH_COMMUNICATION_ERROR		(TWITCH_ERROR_BASE + 2)		// 1002
#define TWITCH_TIMOUT_ERROR				(TWITCH_ERROR_BASE + 3)		// 1003
#define TWITCH_AUTHORITHATION_ERROR		(TWITCH_ERROR_BASE + 4)		// 1004 Cannot get token by authorizarion code
#define TWITCH_USER_INFO_ERROR			(TWITCH_ERROR_BASE + 5)		// 1005 Cannot get user information
#define TWITCH_ID_DUPLICATED			(TWITCH_ERROR_BASE + 6)		// 1006 twitch Id was used in last 24 hours

#endif	// twitchgateway_h_included

