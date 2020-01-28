// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Parts of this file are being maintained using automated scripts.
// The script maintenance parts are surrounded by language-specific
// comment lines that begin with <<== and end with ==>> markers.
// When required, manual changes can made to this file as usual. 
// Such changes should be communicated to the current maintainer 
// of GIT module in ADMIN/asppages/admin/_licenses. 
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#ifndef brands_h_included
#define brands_h_included

#include "ppincludebase.h"

////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2015 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

enum BrandTypes : UINT32 // bit mask
{
//	<<== @brand-list19: Brand enumeration @ ====
	BrandType_None				= 0,		// Should only be used for account creation.  Defaults in the login flow should use BrandType_PokerStars
	BrandType_PokerStars		= 1,
	BrandType_StarsCasino		= 1 << 1, 	// 2	PYR-46066
	BrandType_BetStars			= 1 << 2, 	// 4	
	BrandType_StarsDraft		= 1 << 3, 	// 8	
	BrandType_Duel				= 1 << 4, 	// 16	PYR-53789 -  obsolete
	BrandType_JackPotPoker		= 1 << 5, 	// 32	
	BrandType_FullTilt			= 1 << 6, 	// 64	
	BrandType_Unknown			= 1 << 7, 	// 128	PYR-42031
	BrandType_PokerStarsVegas	= 1 << 8, 	// 256	PYR-46235
	BrandType_SkyBet			= 1 << 9, 	// 512	PYR-81321
	BrandType_SkyPoker			= 1 << 10, 	// 1024	PYR-81321
	BrandType_SkyCasino			= 1 << 11, 	// 2048	PYR-81321
	BrandType_SkyVegas			= 1 << 12, 	// 4096	PYR-81321
	BrandType_FoxBet			= 1 << 13, 	// 8192	PYR-98242

	BrandType_Last = BrandType_FoxBet,	//	must be kept up to date
	BrandType_AllDefined =  (
							  BrandType_PokerStars
							| BrandType_StarsCasino
							| BrandType_BetStars
							| BrandType_StarsDraft
							| BrandType_JackPotPoker
							| BrandType_FullTilt
							| BrandType_Unknown
							| BrandType_PokerStarsVegas
							| BrandType_SkyBet
							| BrandType_SkyPoker
							| BrandType_SkyCasino
							| BrandType_SkyVegas
							| BrandType_FoxBet
							),	//	must be kept up to date
//	==== @brand-list19: Brand enumeration @ ==>>
	BrandType_ALLMask		=  0x7FFFFFFF	//  mask for ALL brands (only using 31 bits)
	// IMPORTANT: do not create more than 31 brand types as the number is
	//			  used to create a bit mask. see PYR-37793
	//	When adding new BrandType's, also update the various functions in brandutils.h, the brand list in scripts.h (if its intended on being read by tournament/table scripts), and speak with Mikhail Maximov about adding it to i18nFormatBrandedName
	// Also update BrandRules
};

#endif // brands_h_included
