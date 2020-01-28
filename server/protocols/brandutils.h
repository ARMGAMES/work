// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Parts of this file are being maintained using automated scripts.
// The script maintenance parts are surrounded by language-specific
// comment lines that begin with <<== and end with ==>> markers.
// When required, manual changes can made to this file as usual. 
// Such changes should be communicated to the current maintainer 
// of GIT module in ADMIN/asppages/admin/_licenses. 
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#ifndef brandutils_h_included
#define brandutils_h_included

////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2015 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "brands.h"
#include "ppstring.h"
#include "pplogfile.h"
#include "dbm.h"
#include "clientplatforms.h"

#define TODO_BRANDID_TO_BE_PROVIDED BrandType_Unknown // PYR-42031 TODO FK - this define better be used as rare as possible

// PYR-38530
inline UINT32 getBrandId(const char* nameName)
{
	if(!nameName || !*nameName)
	{
		return BrandType_None;
	}
		
//	<<== @brand-list20: Brand: id to enumeration @ ====
	if( 0 == PString::compareIgnoreCase(nameName, "PS") )
	{
		return BrandType_PokerStars;
	}

	if( 0 == PString::compareIgnoreCase(nameName, "SC") )	// PYR-46066
	{
		return BrandType_StarsCasino;
	}

	if( 0 == PString::compareIgnoreCase(nameName, "BS") )
	{
		return BrandType_BetStars;
	}

	if( 0 == PString::compareIgnoreCase(nameName, "SD") )
	{
		return BrandType_StarsDraft;
	}

	if( 0 == PString::compareIgnoreCase(nameName, "JP") )
	{
		return BrandType_JackPotPoker;
	}

	if( 0 == PString::compareIgnoreCase(nameName, "FT") )
	{
		return BrandType_FullTilt;
	}

	if( 0 == PString::compareIgnoreCase(nameName, "SA") )	// PYR-42031
	{
		return BrandType_Unknown;
	}

	if( 0 == PString::compareIgnoreCase(nameName, "VS") )	// PYR-46235
	{
		return BrandType_PokerStarsVegas;
	}

	if( 0 == PString::compareIgnoreCase(nameName, "SKB") )	// PYR-81321
	{
		return BrandType_SkyBet;
	}

	if( 0 == PString::compareIgnoreCase(nameName, "SKP") )	// PYR-81321
	{
		return BrandType_SkyPoker;
	}

	if( 0 == PString::compareIgnoreCase(nameName, "SKC") )	// PYR-81321
	{
		return BrandType_SkyCasino;
	}

	if( 0 == PString::compareIgnoreCase(nameName, "SKV") )	// PYR-81321
	{
		return BrandType_SkyVegas;
	}

	if( 0 == PString::compareIgnoreCase(nameName, "FXB") )	// PYR-98242
	{
		return BrandType_FoxBet;
	}

//	==== @brand-list20: Brand: id to enumeration @ ==>>
	
	return BrandType_None;
}

//	PYR-40651
inline UINT32 getBrandIdFromMobileAppId( UINT32 mobileAppId )
{
	switch ( mobileAppId )
	{
	case eMobileAppId_Pokerstars: return BrandType_PokerStars;

	default: return BrandType_None;
	}
}
//	PYR-40651
inline UINT32 getMobileAppIdFromBrandId( UINT32 brandId )
{
	switch ( brandId )
	{
	case BrandType_PokerStars: return eMobileAppId_Pokerstars;

	default: return eMobileAppId_Pokerstars;
	}
}

class BrandRules
{
public:
	//
	// Common
	//
	static const char* getBrandIdName(UINT32 brandId)
	{
		switch(brandId)
		{
//	<<== @brand-list21: Brand: enumeration to name @ ====
			case BrandType_None:
				return "None";
			case BrandType_PokerStars:
				return "PokerStars";
			case BrandType_StarsCasino:	// PYR-46066
				return "PokerStarsCasino";
			case BrandType_BetStars:
				return "BetStars";
			case BrandType_StarsDraft:
				return "StarsDraft";
			case BrandType_JackPotPoker:
				return "JackpotPoker";
			case BrandType_FullTilt:
				return "FullTilt";
			case BrandType_Unknown:	// PYR-42031
				return "Unknown";
			case BrandType_PokerStarsVegas:	// PYR-46235
				return "PokerStarsVegas";
			case BrandType_SkyBet:	// PYR-81321
				return "SkyBet";
			case BrandType_SkyPoker:	// PYR-81321
				return "SkyPoker";
			case BrandType_SkyCasino:	// PYR-81321
				return "SkyCasino";
			case BrandType_SkyVegas:	// PYR-81321
				return "SkyVegas";
			case BrandType_FoxBet:	// PYR-98242
				return "FoxBet";
//	==== @brand-list21: Brand: enumeration to name @ ==>>
		}
		return "Unknown";
	}

	static BrandTypes getBrandIdByName(const char* brandName)
	{
//	<<== @brand-list22: Brand: enumeration by name @ ====
		if (PString::compareIgnoreCase(brandName, "None") == 0 )
		{
			return BrandType_None;
		}
		if (PString::compareIgnoreCase(brandName, "PokerStars") == 0 )
		{
			return BrandType_PokerStars;
		}
		if (PString::compareIgnoreCase(brandName, "PokerStarsCasino") == 0 )	// PYR-46066
		{
			return BrandType_StarsCasino;
		}
		if (PString::compareIgnoreCase(brandName, "BetStars") == 0 )
		{
			return BrandType_BetStars;
		}
		if (PString::compareIgnoreCase(brandName, "StarsDraft") == 0 )
		{
			return BrandType_StarsDraft;
		}
		if (PString::compareIgnoreCase(brandName, "JackpotPoker") == 0 )
		{
			return BrandType_JackPotPoker;
		}
		if (PString::compareIgnoreCase(brandName, "FullTilt") == 0 )
		{
			return BrandType_FullTilt;
		}
		if (PString::compareIgnoreCase(brandName, "Unknown") == 0 )	// PYR-42031
		{
			return BrandType_Unknown;
		}
		if (PString::compareIgnoreCase(brandName, "PokerStarsVegas") == 0 )	// PYR-46235
		{
			return BrandType_PokerStarsVegas;
		}
		if (PString::compareIgnoreCase(brandName, "SkyBet") == 0 )	// PYR-81321
		{
			return BrandType_SkyBet;
		}
		if (PString::compareIgnoreCase(brandName, "SkyPoker") == 0 )	// PYR-81321
		{
			return BrandType_SkyPoker;
		}
		if (PString::compareIgnoreCase(brandName, "SkyCasino") == 0 )	// PYR-81321
		{
			return BrandType_SkyCasino;
		}
		if (PString::compareIgnoreCase(brandName, "SkyVegas") == 0 )	// PYR-81321
		{
			return BrandType_SkyVegas;
		}
		if (PString::compareIgnoreCase(brandName, "FoxBet") == 0 )	// PYR-98242
		{
			return BrandType_FoxBet;
		}
//	==== @brand-list22: Brand: enumeration by name @ ==>>

		return BrandType_None; // actually unknown
	}

	// PYR-41051
	static const char* getBrandIdCode(BrandTypes brandId)
	{
		switch (brandId)
		{
//	<<== @brand-list23: Brand: enumeration to code @ ====
		case BrandType_PokerStars:
			return "PS";
		case BrandType_StarsCasino:	// PYR-46066
			return "SC";
		case BrandType_BetStars:
			return "BS";
		case BrandType_StarsDraft:
			return "SD";
		case BrandType_JackPotPoker:
			return "JP";
		case BrandType_FullTilt:
			return "FT";
		case BrandType_Unknown:	// PYR-42031
			return "SA";
		case BrandType_PokerStarsVegas:	// PYR-46235
			return "VS";
		case BrandType_SkyBet:	// PYR-81321
			return "SKB";
		case BrandType_SkyPoker:	// PYR-81321
			return "SKP";
		case BrandType_SkyCasino:	// PYR-81321
			return "SKC";
		case BrandType_SkyVegas:	// PYR-81321
			return "SKV";
		case BrandType_FoxBet:	// PYR-98242
			return "FXB";
		default:
			return "PS";
//	==== @brand-list23: Brand: enumeration to code @ ==>>
		}
	}
};

class AppTypeRules
{
public:
	static AppTypeId getAppTypeIdByName(const char* name) // PYR-40865
	{
		if(!name || !*name)
			return AppTypeId_Unknown;

//	<<== @apptype-by-name: Application type by name @ ====
		if (0 == PString::compareIgnoreCase(name, "NDC"))
			return AppTypeId_NDC;
		if (0 == PString::compareIgnoreCase(name, "NMA"))
			return AppTypeId_NMA;
		if (0 == PString::compareIgnoreCase(name, "RAM"))
			return AppTypeId_RAM;
		if (0 == PString::compareIgnoreCase(name, "WebCashierRM"))
			return AppTypeId_WebCashierRM;
		if (0 == PString::compareIgnoreCase(name, "WebCashierPM"))
			return AppTypeId_WebCashierPM;
		if (0 == PString::compareIgnoreCase(name, "ICE"))
			return AppTypeId_ICE;
		if (0 == PString::compareIgnoreCase(name, "Facebook"))
			return AppTypeId_Facebook;
		if (0 == PString::compareIgnoreCase(name, "WCSF"))
			return AppTypeId_WCSF;
		if (0 == PString::compareIgnoreCase(name, "VIPStore"))
			return AppTypeId_VIPStore;
		if (0 == PString::compareIgnoreCase(name, "WRMF"))
			return AppTypeId_WRMF;
		if (0 == PString::compareIgnoreCase(name, "OnBoarding"))
			return AppTypeId_OnBoarding;
		if (0 == PString::compareIgnoreCase(name, "WebCashierDeal"))
			return AppTypeId_WebCashierDeal;
		if (0 == PString::compareIgnoreCase(name, "RWS"))
			return AppTypeId_RWS;
		if (0 == PString::compareIgnoreCase(name, "FtpMigrate"))
			return AppTypeId_FtpMigrate;
		if (0 == PString::compareIgnoreCase(name, "Storm"))
			return AppTypeId_Storm;
		if (0 == PString::compareIgnoreCase(name, "NMA3"))
			return AppTypeId_NMA3;
		if (0 == PString::compareIgnoreCase(name, "PSPlay"))
			return AppTypeId_PSPlay;
		if (0 == PString::compareIgnoreCase(name, "ChestWidget"))
			return AppTypeId_ChestWidget;
		if (0 == PString::compareIgnoreCase(name, "ChestCIPWA"))
			return AppTypeId_ChestCIPWA;
//	==== @apptype-by-name: Application type by name @ ==>>

		return AppTypeId_Unknown;
	}

	static const char* getAppTypeIdName(UINT32 appTypeId)
	{
		switch (appTypeId)
		{
//	<<== @app-name-by-type: Application name by type @ ====
			case AppTypeId_NDC:				return "NDC";
			case AppTypeId_NMA:				return "NMA";
			case AppTypeId_RAM:				return "RAM";
			case AppTypeId_WebCashierRM:	return "WebCashierRM";
			case AppTypeId_WebCashierPM:	return "WebCashierPM";
			case AppTypeId_ICE:				return "ICE";
			case AppTypeId_Facebook:		return "Facebook";
			case AppTypeId_WCSF:			return "WCSF";
			case AppTypeId_VIPStore:		return "VIPStore";
			case AppTypeId_WRMF:			return "WRMF";
			case AppTypeId_OnBoarding:		return "OnBoarding";
			case AppTypeId_WebCashierDeal:	return "WebCashierDeal";
			case AppTypeId_RWS:				return "RWS";
			case AppTypeId_FtpMigrate:		return "FtpMigrate";
			case AppTypeId_Storm:			return "Storm";
			case AppTypeId_NMA3:			return "NMA3";
			case AppTypeId_PSPlay:			return "PSPlay";
			case AppTypeId_ChestWidget:		return "ChestWidget";
			case AppTypeId_ChestCIPWA:		return "ChestCIPWA";
//	==== @app-name-by-type: Application name by type @ ==>>
			default:						return "Unknown";
		}
	}

	static bool isNativeMobileApp(UINT32 appTypeId)
	{
		return (appTypeId & AppTypeId_NativeMobile_AllMask) != 0;
	}

	static bool isNativeApp(UINT32 appTypeId)
	{
		return (appTypeId & (AppTypeId_NativeMobile_AllMask | AppTypeId_NDC)) != 0;
	}
};

// PYR-40278
inline bool isBrandIdEligible(UINT32 brandId, UINT32 brandsMask)
{
	if (brandId == BrandType_None)
	{
		PLog("brandid=%d", BrandType_None);
		return true;
	}
	return (brandsMask & brandId) != 0;
}

#endif // brandutils_h_included
