// ===========================================================
// Copyright (C) 2018 Stars Technologies Services  All rights reserved.
// =========================================================== 

//mlobbyNG.h - poker NG mobile lobby support

#ifndef mlobbyNG_h_included
#define mlobbyNG_h_included

#include "mlobbyNG_c.h"

#define MSG_MLOBBYNG_FIND_TOURNAMENTS				1 // see Protocol_MSG_MLOBBY_FIND_TOURNAMENTS
#define MSG_MLOBBYNG_FIND_TOURNAMENTS_REPLY			2 // see Protocol_MSG_MLOBBY_FIND_TOURNAMENTS_REPLY

#define MSG_MLOBBYNG_FIND_TABLE_TYPES				3 // see Protocol_MSG_MLOBBYNG_FIND_TABLE_TYPES
#define MSG_MLOBBYNG_FIND_TABLE_TYPES_REPLY			4 // see Protocol_MSG_MLOBBYNG_FIND_TABLE_TYPES_REPLY

#define MSG_MLOBBYNG_FIND_SNGS						5 // see Protocol_MSG_MLOBBYNG_FIND_SNGS
#define MSG_MLOBBYNG_FIND_SNGS_REPLY				6 // see Protocol_MSG_MLOBBYNG_FIND_SNGS_REPLY

#define MSG_MLOBBYNG_GET_GAME_LIST					7 // see Protocol_MSG_MLOBBYNG_GET_GAME_LIST
#define MSG_MLOBBYNG_GET_GAME_LIST_REPLY			8 // see Protocol_MSG_MLOBBYNG_GET_GAME_LIST_REPLY

#define MSG_MLOBBYNG_GET_SUGGESTED_GAMES			9 // see Protocol_MSG_MLOBBYNG_GET_SUGGESTED_GAMES
#define MSG_MLOBBYNG_GET_SUGGESTED_GAMES_REPLY		10 // see Protocol_MSG_MLOBBYNG_GET_SUGGESTED_GAMES_REPLY

#define MSG_MLOBBYNG_TABLE_TYPE_COUNTS				11 // see Protocol_MSG_MLOBBYNG_TABLE_TYPE_COUNTS
#define MSG_MLOBBYNG_TABLE_TYPE_COUNTS_REPLY		12 // see Protocol_MSG_MLOBBYNG_TABLE_TYPE_COUNTS_REPLY

#define MSG_MLOBBYNG_TOURNAMENT_COUNTS				13 // see Protocol_MSG_MLOBBYNG_TOURNAMENT_COUNTS
#define MSG_MLOBBYNG_TOURNAMENT_COUNTS_REPLY		14 // see Protocol_MSG_MLOBBYNG_TOURNAMENT_COUNTS_REPLY

#define MSG_MLOBBYNG_SNG_COUNTS						15 // see Protocol_MSG_MLOBBYNG_SNG_COUNTS
#define MSG_MLOBBYNG_SNG_COUNTS_REPLY				16 // see Protocol_MSG_MLOBBYNG_SNG_COUNTS_REPLY

#define MSG_MLOBBYNG_GET_GAME_CONN_INFO				17 // see Protocol_MSG_MLOBBYNG_GET_GAME_CONN_INFO	
#define MSG_MLOBBYNG_GET_GAME_CONN_INFO_REPLY		18 // see Protocol_MSG_MLOBBYNG_GET_GAME_CONN_INFO_REPLY

#define MSG_MLOBBYNG_TICKET_TOURNS					19 // see Protocol_MSG_MLOBBYNG_TICKET_TOURNS
#define MSG_MLOBBYNG_TICKET_TOURNS_REPLY			20 // see Protocol_MSG_MLOBBYNG_TICKET_TOURNS_REPLY

#define MSG_MLOBBYNG_GET_GAME_INFO					21 // see Protocol_MSG_MLOBBYNG_GET_GAME_INFO
#define MSG_MLOBBYNG_GET_GAME_INFO_REPLY			22 // see Protocol_MSG_MLOBBYNG_GET_GAME_INFO_REPLY

#define MSG_MLOBBYNG_FILTER_GAMES					23 // see Protocol_MSG_MLOBBYNG_FILTER_GAMES
#define MSG_MLOBBYNG_FILTER_GAMES_REPLY				24 // see Protocol_MSG_MLOBBYNG_FILTER_GAMES_REPLY

#define MSG_MLOBBYNG_GET_SUGGESTED_GAMES2			25 // see Protocol_MSG_MLOBBYNG_GET_SUGGESTED_GAMES2
#define MSG_MLOBBYNG_GET_SUGGESTED_GAMES2_REPLY		26 // see Protocol_MSG_MLOBBYNG_GET_SUGGESTED_GAMES2_REPLY

#define MSG_MLOBBYNG_SEARCH_BY_NAME					27 // see Protocol_MSG_MLOBBYNG_SEARCH_BY_NAME
#define MSG_MLOBBYNG_SEARCH_BY_NAME_REPLY			28 // see Protocol_MSG_MLOBBYNG_SEARCH_BY_NAME_REPLY

#define MSG_MLOBBYNG_FILTER_TICKET_TOURNS			29 // see Protocol_MSG_MLOBBYNG_FILTER_TICKET_TOURNS
#define MSG_MLOBBYNG_FILTER_TICKET_TOURNS_REPLY		30 // see Protocol_MSG_MLOBBYNG_FILTER_TICKET_TOURNS_REPLY

#define MSG_MLOBBYNG_FILTER_GAMES_PS				31 // see Protocol_MSG_MLOBBYNG_FILTER_GAMES + reqId
#define MSG_MLOBBYNG_FILTER_GAMES_PS_REPLY			32 // see Protocol_MSG_MLOBBYNG_FILTER_GAMES_REPLY + reqId

#define MSG_MLOBBYNG_GET_BRANDED_LOBBIES			33 // see Protocol_MSG_MLOBBYNG_GET_BRANDED_LOBBIES
#define MSG_MLOBBYNG_GET_BRANDED_LOBBIES_REPLY		34 // see Protocol_MSG_MLOBBYNG_GET_BRANDED_LOBBIES_REPLY

#define MSG_MLOBBYNG_GET_SUGGESTED_GAMES2_PS		35 // see Protocol_MSG_MLOBBYNG_GET_SUGGESTED_GAMES2_PS
#define MSG_MLOBBYNG_GET_SUGGESTED_GAMES2_PS_REPLY	36 // see Protocol_MSG_MLOBBYNG_GET_SUGGESTED_GAMES2_PS_REPLY

#define MSG_MLOBBYNG_SEARCH_BY_NAME_PS				37 // see Protocol_MSG_MLOBBYNG_SEARCH_BY_NAME_PS
#define MSG_MLOBBYNG_SEARCH_BY_NAME_PS_REPLY		38 // see Protocol_MSG_MLOBBYNG_SEARCH_BY_NAME_PS_REPLY

#define MSG_MLOBBYNG_FIND_TOURNS_BY_REF				39 // see Protocol_MSG_MLOBBYNG_FIND_TOURNS_BY_REF	
#define MSG_MLOBBYNG_FIND_TOURNS_BY_REF_REPLY		40 // see Protocol_MSG_MLOBBYNG_FIND_TOURNS_BY_REF_REPLY

#define MSG_MLOBBYNG_GET_BRANDED_LOBBIES_PS			41 // see Protocol_MSG_MLOBBYNG_GET_BRANDED_LOBBIES_PS
#define MSG_MLOBBYNG_GET_BRANDED_LOBBIES_PS_REPLY	42 // see Protocol_MSG_MLOBBYNG_GET_BRANDED_LOBBIES_PS_REPLY


enum class NGGameTypes : UINT64
{
	NGGame_None					= 0ull,
	NGGame_NLHoldem				= 1ull << 0,
	NGGame_PLOmaha				= 1ull << 1,
	NGGame_NotUsed				= 1ull << 2,
	NGGame_HORSE				= 1ull << 3,
	NGGame_HOSE					= 1ull << 4,
	NGGame_NLHoldem_PLOmaha		= 1ull << 5,
	NGGame_PLHoldem_PLOmaha		= 1ull << 6,
	NGGame_MixedHoldem			= 1ull << 7,
	NGGame_MixedOmahaHL			= 1ull << 8,
	NGGame_MixedOmaha			= 1ull << 9,
	NGGame_TripleStud			= 1ull << 10,
	NGGame_LimitHoldem			= 1ull << 11,
	NGGame_PLHoldem				= 1ull << 12,
	NGGame_NLOmaha				= 1ull << 13,
	NGGame_LimitOmaha			= 1ull << 14,
	NGGame_NLOmahaHL			= 1ull << 15,
	NGGame_PLOmahaHL			= 1ull << 16,
	NGGame_LimitOmahaHL			= 1ull << 17,
	NGGame_PL5Omaha				= 1ull << 18,
	NGGame_NL5Omaha				= 1ull << 19,
	NGGame_NL5OmahaHL			= 1ull << 20,
	NGGame_PL5OmahaHL			= 1ull << 21,
	NGGame_Limit5OmahaHL		= 1ull << 22,
	NGGame_NLCourchevel			= 1ull << 23,
	NGGame_PLCourchevel			= 1ull << 24,
	NGGame_NLCourchevelHL		= 1ull << 25,
	NGGame_PLCourchevelHL		= 1ull << 26,
	NGGame_LimitCourchevelHL	= 1ull << 27,
	NGGame_7Stud				= 1ull << 28,
	NGGame_7StudHL				= 1ull << 29,
	NGGame_Razz					= 1ull << 30,
	NGGame_6PlusHoldemNL		= 1ull << 31,
	NGGame_TempestHoldemNL		= 1ull << 32,
	NGGame_DeepWaterHoldemNL	= 1ull << 33,
	NGGame_NLSplitHoldem		= 1ull << 34,
	NGGame_PLSplitOmaha			= 1ull << 35,
	NGGame_NLShowtimeHoldem		= 1ull << 36,
	NGGame_PLShowtimeOmaha		= 1ull << 37,
	NGGame_PLFusion				= 1ull << 38,
	NGGame_NLSwapHoldem			= 1ull << 39,
	NGGame_PL6Omaha				= 1ull << 40,
	NGGame_Last = NGGame_PL6Omaha, // !!!please keep up to date, update NUM_NG_GAME_TYPES when adding/deleting game types
};

#define NUM_NG_GAME_TYPES 41

#endif	//mlobbyNG_h_included
