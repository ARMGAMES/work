// ===========================================================
// Copyright (C) 2018 Stars Technologies Services  All rights reserved.
// =========================================================== 

//personalization.h - user personalization server

#ifndef personalization_h_included
#define personalization_h_included


#define PS_Q_GET_RECENT_GAME_LIST				35000
#define PS_A_GET_RECENT_GAME_LIST				35001

#define PS_Q_ADMIN_GET_RECENT_GAME_LIST			35002
#define PS_A_ADMIN_GET_RECENT_GAME_LIST			35003

#define PS_U_GET_CURRENTLY_SEATED_AT			35004

#define PS_Q_GET_TICKET_TOURNS					35005
#define PS_A_GET_TICKET_TOURNS					35006

#define PS_Q_FILTER_GAMES						35007
#define PS_A_FILTER_GAMES						35008

#define PS_Q_GET_BRANDED_LOBBIES				35009
#define PS_A_GET_BRANDED_LOBBIES				35010

#define PS_Q_ADMIN_UPDATE_LOBBIES				35011
#define PS_A_ADMIN_UPDATE_LOBBIES				35012

#define PS_Q_INIT_BRANDED_LOBBIES				35013
#define PS_A_INIT_BRANDED_LOBBIES				35014

#define PS_Q_GET_SUGGESTED_GAMES				35015
#define PS_A_GET_SUGGESTED_GAMES				35016

#define PS_Q_SEARCH_BY_NAME						35017
#define PS_A_SEARCH_BY_NAME						35018

#define PS_Q_GET_TICKET_COUNTS					35019
#define PS_A_GET_TICKET_COUNTS					35020

enum GameCategoryTypes
{
	GameCategory_None		= 0,
	GameCategory_Cash		= 1<<0,
	GameCategory_MTT		= 1<<1,
	GameCategory_SNG		= 1<<2,
	GameCategory_SAG		= 1<<3,
	GameCategory_Zoom		= 1<<4,
	GameCategory_SFN		= 1<<5,
	GameCategory_Table		= 1<<6,			// Currently only used for MSG_MLOBBYNG_GET_GAME_INFO
	GameCategory_PPP		= 1<<7,
	GameCategory_Last = GameCategory_PPP, // !!!please keep up to date
};

// Tourn Variant
enum TournVariantTypes
{
	TournVariant_Regular = 0,	//default
	TournVariant_Rebuy = 1,
	TournVariant_Knockout = 2,
	TournVariant_Shootout = 3,
	TournVariant_Phase = 4,
	TournVariant_Time = 5,
	TournVariant_Zoom = 6,
	TournVariant_Satellite = 7,
	TournVariant_WinTheButton = 8,
	TournVariant_ReEntry = 9,
	TournVariant_NStack = 10,
	TournVariant_SagMax = 11,
	TournVariant_LiveSatellite = 12,
	TournVariant_Fifty50 = 13,
	TournVariant_Last = TournVariant_NStack, // !!!please keep up to date
};

enum ActivityFlagTypes
{
	ActivityFlag_isPM = 1 << 0,
	ActivityFlag_isHiLo = 1 << 1,
	ActivityFlag_hasAnte = 1 << 2,
	ActivityFlag_hasCap = 1 << 3,
	ActivityFlag_isSagMax = 1 << 4,
	ActivityFlag_isSagFlash = 1 << 5,
	ActivityFlag_Last = ActivityFlag_isSagFlash, // !!!please keep up to date
};


enum TicketTournRequestTypes
{
	TicketTournRequest_None = 0,
	TicketTournRequest_RecentlyPlayed = 1 << 0,
	TicketTournRequest_NonRecentlyPlayed = 1 << 1,
	TicketTournRequest_Last = TicketTournRequest_NonRecentlyPlayed, // !!!please keep up to date
};


#endif	//personalization_h_included
