// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Parts of this file are being maintained using automated scripts.
// The script maintenance parts are surrounded by language-specific
// comment lines that begin with <<== and end with ==>> markers.
// When required, manual changes can made to this file as usual. 
// Such changes should be communicated to the current maintainer 
// of GIT module in ADMIN/asppages/admin/_licenses. 
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#ifndef Sites_h_included
#define Sites_h_included

#include "ppincludebase.h"
#include "liquidityPoolDefs.h"  // PYR-58269

////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2010 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

//	<<== @site2-definitions: Site definitions @ ====
enum Sites : UINT32
{
	PokerStars_SiteUnknown	=	0x00000000,		// 0	:	0 // was PokerStars_Test before 2013
	PokerStars_Com			=	0x00000001,		// 0	:	1
	PokerStars_Net			=	0x00000002,		// 1	:	2
	PokerStars_Lite			=	0x00000004,		// 2	:	4
///* >>> */ HoyleSierra_Com	=	0x00000008,		// 3	:	8	<<< TO BE RETIRED !!! >>>
	PokerStars_Ita			=	0x00000010,		// 4	:	16
	PokerStars_Fra			=	0x00000020,		// 5	:	32
	PokerStars_Est			=	0x00000040,		// 6	:	64
	PokerStars_ESPN			=	0x00000080,		// 7	:	128
	PokerStars_FOX			=	0x00000100,		// 8	:	256
	PokerStars_Bel			=	0x00000200,		// 9	:	512
	PokerStars_Spa			=	0x00000400,		// 10	:	1024
	PokerStars_Den			=	0x00000800,		// 11	:	2048
	PokerStars_EU			=	0x00001000,		// 12	:	4096
	PokerStars_FB			=	0x00002000,		// 13	:	8192
	PokerStars_US_NJ		=	0x00004000,		// 14	:	16384
	PokerStars_UK			=	0x00008000,		// 15	:	32768
	PokerStars_Bul			=	0x00010000,		// 16	:	65536
	PokerStars_DE_SH		=	0x00020000,		// 17	:	131072
	PokerStars_Por_Old		=	0x00040000,		// 18	:	262144
	PokerStars_Rom			=	0x00080000,		// 19	:	524288
	PokerStars_Por			=	0x00100000,		// 20	:	1048576
	PokerStars_Cze			=	0x00200000,		// 21	:	2097152
	PokerStars_RUSO			=	0x00400000,		// 22	:	4194304
	PokerStars_RUSOPM		=	0x00800000,		// 23	:	8388608
	PokerStars_Ind			=	0x01000000,		// 24	:	16777216
	PokerStars_Asia			=	0x02000000,		// 25	:	33554432
	PokerStars_RU			=	0x04000000,		// 26	:	67108864
	PokerStars_US_PA		=	0x08000000,		// 27	:	134217728
	PokerStars_Swe			=	0x10000000,		// 28	:	268435456
	PokerStars_CH			=	0x20000000,		// 29	:	536870912
	PokerStars_ARBA			=	0x20000001,		// 33	:	536870913
	PokerStars_US_CO		=	0x20000002,		// 34	:	536870914
	PokerStars_DE			=	0x20000003,		// 35	:	536870915
	PokerStars_GR			=	0x2000002D,		// 77	:	536870957
	//	Procedure for manually adding sites:
	//
	//		1. Choose next site definition from possible candidates:
	//				PokerStars_nextSite1 = 0x20000004, 
	//				PokerStars_nextSite1 = 0x20000005, 
	//				PokerStars_nextSite1 = 0x20000006,	... 
	//			
	//		2. Update sitemasks.cpp/h. 
	//

	// MaxAllowed	=	0x40000000, as 0x80000000 is reserved by Majordomo!
	PokerStars_Last			= PokerStars_GR,	// Must be kept up to date
//	==== @site2-definitions: Site definitions @ ==>>

	//	Deprecated. Use SiteBitmask_RealMoneyMask
	PokerStars_RealMoneyMask = 
	( PokerStars_Com
	 |PokerStars_Ita
	 |PokerStars_Fra
	 |PokerStars_Est
	 |PokerStars_Bel
	 |PokerStars_Spa
	 |PokerStars_Den
	 |PokerStars_EU
	 |PokerStars_US_NJ
	 |PokerStars_UK
	 |PokerStars_Bul
	 |PokerStars_DE_SH
	 |PokerStars_Por
	 |PokerStars_Rom
	 |PokerStars_Cze
	 |PokerStars_RUSO
	 |PokerStars_Ind
	 |PokerStars_Asia
	 |PokerStars_RU
	 |PokerStars_US_PA
	 |PokerStars_Swe
	 |PokerStars_CH
	),

	//	The following are deprecated. Use the equivalents in sitemasks.h
	PokerStars_AllValidMask		= (PokerStars_Last*2 - 1),
	PokerStars_ALLMask			=  0xFFFFFFFF
};

////////////////////////////////////////////////////////////////////////////////

#endif // Sites_h_included
