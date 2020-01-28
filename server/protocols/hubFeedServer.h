// ===========================================================
// Copyright (C) 2020 Stars Technologies Services  All rights reserved.
// =========================================================== 

#ifndef hubfeedserver_h_included
#define hubfeedserver_h_included


#define HFS_Q_GET_FEED_ITEMS						37000
#define HFS_A_GET_FEED_ITEMS						37001

#define HFS_Q_ADMIN_TEMPLATE_UPDATE					37002
#define HFS_A_ADMIN_TEMPLATE_UPDATE					37003

enum eHubFeedUserVisibilityType
{
	eHubFeedUserVisibilityType_Any = 1,
	eHubFeedUserVisibilityType_PreLogin = 2,
	eHubFeedUserVisibilityType_PostLogin = 3,
};

enum eHubFeedType
{
	eHubFeedType_SiteBigWin = 1,
	eHubFeedType_PlayerBigWin = 2,
	eHubFeedType_Manual = 3,
};

enum eHubFeedIconType
{
	eHubFeedIconType_Calendar = 1,
	eHubFeedIconType_Flag = 2,
	eHubFeedIconType_Game = 3,
	eHubFeedIconType_Megaphone = 4,
	eHubFeedIconType_Present = 5,
	eHubFeedIconType_Ball = 6,
};

enum eHubFeedPersonaType
{
	eHubFeedPersonaType_NewPlayer = 1 << 0,
	eHubFeedPersonaType_HoldemSAG = 1 << 1,
	eHubFeedPersonaType_HoldemSNG = 1 << 2,
	eHubFeedPersonaType_HoldemRG = 1 << 3,
	eHubFeedPersonaType_HoldemTourn = 1 << 4,
	eHubFeedPersonaType_HoldemOther = 1 << 5,
	eHubFeedPersonaType_OmahaOnly = 1 << 6,
	eHubFeedPersonaType_MixedGamesOnly = 1 << 7,
	eHubFeedPersonaType_MultipleGames = 1 << 8,
};

#endif	//hubfeedserver_h_included
