// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Parts of this file are being maintained using automated scripts.
// The script maintenance parts are surrounded by language-specific
// comment lines that begin with <<== and end with ==>> markers.
// When required, manual changes can made to this file as usual. 
// Such changes should be communicated to the current maintainer 
// of GIT module in ADMIN/asppages/admin/_licenses. 
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#ifndef clientplatforms_h_included
#define clientplatforms_h_included

////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "brands.h"
#include "ppincludebase.h"

// IMPORATANT: do not add new values. Consult Anton or Ling if you have questions. // PYR-38296 
//	<<== @platform-list: Platform definitions @ ====
enum ClientPlatform
{
	Client_UnknownPlatform		= 0,	
	Client_Win					= 1,	
	Client_Mac					= 2,	
	Client_iOS					= 3,	
	Client_Android				= 4,	
	Client_Web					= 5,	//	Web Cashier,RAM
	// Client_Win_Pipe			= 6,	
	// Client_Mac_Pipe			= 7,	
	Client_CORE					= 8,	//	Facebook CORE client

	Client_Last = Client_CORE,		//	must be kept up to date

	// IMPORTANT: do not create more than 32 platforms as the number is
	//			  used to create a 32 bit mask. see PYR-23460
};
//	==== @platform-list: Platform definitions @ ==>>

// IMPORATANT: do not add new values. Consult Anton or Ling if you have questions. // PYR-38296 
enum ClientType // bit mask
{
	ClientType_Unknown		= 0,
	ClientType_Default		= 1 << 0,
	ClientType_Pipe			= 1 << 1,
	ClientType_CORE			= 1 << 2,

	// Added as of PYR-29902
	ClientType_CORE_TOUCH		= 1 << 3,
	ClientType_WebCasher_RM		= 1 << 4,
	ClientType_WebCasher_PM		= 1 << 5,
	ClientType_WebCasher_DEAL	= 1 << 6,
	ClientType_Onboarding		= 1 << 7,
	// End of PYR-29902
	ClientType_ContactSupportForm	= 1 << 8, //PYR-29531
	ClientType_ICE	= 1 << 9, //PYR-29929
    ClientType_Sportsbook = 1 << 10, // SBK-56
	ClientType_Poker = 1 << 11, // PYR-31224
	ClientType_Casino = 1 << 12, 
	ClientType_VipStore = 1 << 13, // PYR-28871
	ClientType_CORE_Touch_Panda = 1 << 14, // PYR-33340
	ClientType_RationalWebServices = 1 << 15, //PYR-33416
	ClientType_PokerDuel = 1 << 16, // PYR-35000 // PYR-53789 - obsolete
	ClientType_RealMoneyForm = 1 << 17, // PYR-36157
	ClientType_DFS = 1 << 18, // PYR-36555

	ClientType_Last = ClientType_DFS,	//	must be kept up to date
	// IMPORTANT: do not create more than 32 client types as the number is
	//			  used to create a 32 bit mask. see PYR-30167
	//	When adding new ClientType's, also update monitor parameters in lobby2\lobbyobject.cpp, reportPerformance()
};

enum ClientDevice // bit mask
{
//	<<== @device-list: Device type list @ ====
	ClientDevice_Unknown		= 0,
	ClientDevice_Desktop		= 1 << 0, // 1	
	ClientDevice_Tablet			= 1 << 1, // 2	
	ClientDevice_Phone			= 1 << 2, // 4	
	ClientDevice_GameConsole	= 1 << 3, // 8	
	ClientDevice_SmartTV		= 1 << 4, // 16	
	ClientDevice_Wearable		= 1 << 5, // 32	
	ClientDevice_PDA			= 1 << 6, // 64	
	ClientDevice_Other			= 1 << 7, // 128	

	ClientDevice_Last			= ClientDevice_Other,		//	must be kept up to date
//	==== @device-list: Device type list @ ==>>
};

enum OSId
{
//	<<== @os-list: OS type list @ ====
	OSId_Unknown		= 0,
	OSId_Windows		= 1 << 0, // 1	
	OSId_Mac			= 1 << 1, // 2	
	OSId_iOS			= 1 << 2, // 4	
	OSId_AndroId		= 1 << 3, // 8	
	OSId_Linux			= 1 << 4, // 16	
	OsId_WindowsPhone	= 1 << 5, // 32	
	OsId_ChromeOS		= 1 << 6, // 64	
	OsId_FireOS			= 1 << 7, // 128	
	OsId_Tizen			= 1 << 8, // 256	
	OsId_Other			= 1 << 9, // 512	

	OsId_Last			= OsId_Other,		//	must be kept up to date
//	==== @os-list: OS type list @ ==>>
};

enum AppTypeId // is a mask
{
//	<<== @apptype-list: Application type list @ ====
	AppTypeId_Unknown			= 0,	 	// 0			
	AppTypeId_NDC				= 1 << 0, 	// 1			native desktop client
	AppTypeId_NMA				= 1 << 1, 	// 2			native mobile app (not unique, see below for other mobile apps)
	AppTypeId_RAM				= 1 << 2, 	// 4			
	AppTypeId_WebCashierRM		= 1 << 3, 	// 8			
	AppTypeId_WebCashierPM		= 1 << 4, 	// 16			
	AppTypeId_ICE				= 1 << 5, 	// 32			
	AppTypeId_Facebook			= 1 << 6, 	// 64			
	AppTypeId_WCSF				= 1 << 7, 	// 128			web contact support form
	AppTypeId_VIPStore			= 1 << 8, 	// 256			
	AppTypeId_WRMF				= 1 << 9, 	// 512			web real money form
	AppTypeId_OnBoarding		= 1 << 10, 	// 1024		
	AppTypeId_WebCashierDeal	= 1 << 11, 	// 2048		
	AppTypeId_RWS				= 1 << 12, 	// 4096		rational web services
	AppTypeId_FtpMigrate		= 1 << 13, 	// 8192		PYR-40997
	AppTypeId_Storm				= 1 << 14, 	// 16384		PYR-44178
	AppTypeId_NMA3				= 1 << 15, 	// 32768		PYR-47996 - native mobile app built on top of Mobile3 platform
	AppTypeId_PSPlay			= 1 << 16, 	// 65536		PYR-47688, Poker Stars Play
	AppTypeId_ChestWidget		= 1 << 17, 	// 131072		PYR-51252, Stars Rewards Chest and Widget
	AppTypeId_ChestCIPWA		= 1 << 18, 	// 262144		PYR-52186: Chest connection through CIPWA

	// !!! update AppTypeId_Last below !!!
	// !!! update AppTypeId_NativeMobile_AllMask below if it is mobile native app !!!
	// !!! update getAppTypeIdByName & getAppTypeIdName of AppTypeRules !!!
	// IMPORTANT do not exceed 31 bits (maximum 1 << 30)
	AppTypeId_Last				   = AppTypeId_ChestCIPWA,
	AppTypeId_NativeMobile_AllMask = AppTypeId_NMA | AppTypeId_NMA3 | AppTypeId_PSPlay,
	AppTypeId_NativeClient_AllMask = AppTypeId_NativeMobile_AllMask | AppTypeId_NDC,
	AppTypeId_ALLMask		=  0x7FFFFFFF,	//  mask for ALL app type Ids (only using 31 bits)
//	==== @apptype-list: Application type list @ ==>>
};

enum ProductId
{
//	<<== @product-list: Product type list @ ====
	ProductId_Unknown			= 0,
	ProductId_Poker				= 1 << 0,	// 1	
	ProductId_SportsBet			= 1 << 1,	// 2	
	ProductId_Casino			= 1 << 2,	// 4	
	ProductId_PokerDuel			= 1 << 3,	// 8	PYR-53789
	ProductId_DFS				= 1 << 4,	// 16	
	ProductId_JackpotPoker		= 1 << 5,	// 32	
	ProductId_Vegas				= 1 << 6,	// 64	
	ProductId_PoolBetting		= 1 << 7,	// 128	PYR-42410
	ProductId_SocialCasino		= 1 << 8,	// 256	PYR-54541
	ProductId_Predictor			= 1 << 9,	// 512	PYR-63110
	ProductId_StarsStore		= 1 << 10,	// 1024	PYR-81644
	ProductId_Free2Play			= 1 << 11,	// 2048	PYR-118078

	ProductId_Last				= ProductId_Free2Play, // must be kept up to date
//	==== @product-list: Product type list @ ==>>
};

class ClientInfoUtils
{
public:
	// whether client is sending the new login parameters. Note brandId is left out since it is defaulted to BrandType_PokerStars instead of unknown.
	static bool hasNewClientLoginInfo(UINT32 /*osId*/, UINT32 appTypeId, UINT32 /*productId*/)
	{
		// currently it is solely based on app type
		return appTypeId != AppTypeId_Unknown;
	}
};

#endif // clientplatforms_h_included
