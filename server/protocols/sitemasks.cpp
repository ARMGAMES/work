// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Parts of this file are being maintained using automated scripts.
// The script maintenance parts are surrounded by language-specific
// comment lines that begin with <<== and end with ==>> markers.
// When required, manual changes can made to this file as usual. 
// Such changes should be communicated to the current maintainer 
// of GIT module in ADMIN/asppages/admin/_licenses. 
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2019 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "sitemasks.h"
#include "pplogfile.h"
#include "dbm.h" // Just for LOG_TRACE_FAULTY_LOGIC
#include "vectorutils.h"

const PSiteMask PSiteMask::zero;

extern const PSiteMask SiteBitmask_SiteUnknown = PSiteMask();

#define LEGACY_SITE_BITMASK( name ) extern const PSiteMask SiteBitmask_##name( PSiteMask::fromUINT64( PokerStars_##name ) )

//	<<== @site2-list: List of all sites @ ====
LEGACY_SITE_BITMASK( Com );
LEGACY_SITE_BITMASK( Net );
LEGACY_SITE_BITMASK( Lite );
LEGACY_SITE_BITMASK( Ita );
LEGACY_SITE_BITMASK( Fra );
LEGACY_SITE_BITMASK( Est );
LEGACY_SITE_BITMASK( ESPN );
LEGACY_SITE_BITMASK( FOX );
LEGACY_SITE_BITMASK( Bel );
LEGACY_SITE_BITMASK( Spa );
LEGACY_SITE_BITMASK( Den );
LEGACY_SITE_BITMASK( EU );
LEGACY_SITE_BITMASK( FB );
LEGACY_SITE_BITMASK( US_NJ );
LEGACY_SITE_BITMASK( UK );
LEGACY_SITE_BITMASK( Bul );
LEGACY_SITE_BITMASK( DE_SH );
LEGACY_SITE_BITMASK( Por_Old );
LEGACY_SITE_BITMASK( Rom );
LEGACY_SITE_BITMASK( Por );
LEGACY_SITE_BITMASK( Cze );
LEGACY_SITE_BITMASK( RUSO );
LEGACY_SITE_BITMASK( RUSOPM );
LEGACY_SITE_BITMASK( Ind );
LEGACY_SITE_BITMASK( Asia );
LEGACY_SITE_BITMASK( RU );
LEGACY_SITE_BITMASK( US_PA );
LEGACY_SITE_BITMASK( Swe );
LEGACY_SITE_BITMASK( CH );

#undef LEGACY_SITE_BITMASK

  extern const PSiteMask SiteBitmask_ARBA = PSiteMask::fromUINT64(ONE64 << 33);
  extern const PSiteMask SiteBitmask_US_CO = PSiteMask::fromUINT64(ONE64 << 34);
  extern const PSiteMask SiteBitmask_DE = PSiteMask::fromUINT64(ONE64 << 35);
  extern const PSiteMask SiteBitmask_GR = PSiteMask::fromDbStringS("20000000000000000000");	//	bit 77

//	Future masks must be defined as follows:
//extern const PSiteMask SiteBitmask_Future1 = PSiteMask::fromUINT64(ONE64 << 36);
//extern const PSiteMask SiteBitmask_Future2 = PSiteMask::fromUINT64(ONE64 << 37);
//extern const PSiteMask SiteBitmask_Future3 = PSiteMask::fromUINT64(ONE64 << 38);
//extern const PSiteMask SiteBitmask_Future4 = PSiteMask::fromUINT64(ONE64 << 39);
//extern const PSiteMask SiteBitmask_Future5 = PSiteMask::fromUINT64(ONE64 << 40);
//extern const PSiteMask SiteBitmask_Future6 = PSiteMask::fromUINT64(ONE64 << 41);
//extern const PSiteMask SiteBitmask_Future7 = PSiteMask::fromUINT64(ONE64 << 42);
//extern const PSiteMask SiteBitmask_Future8 = PSiteMask::fromUINT64(ONE64 << 43);
//extern const PSiteMask SiteBitmask_Future9 = PSiteMask::fromUINT64(ONE64 << 44);
//extern const PSiteMask SiteBitmask_Future10 = PSiteMask::fromUINT64(ONE64 << 45);
//extern const PSiteMask SiteBitmask_Future11 = PSiteMask::fromUINT64(ONE64 << 46);
//extern const PSiteMask SiteBitmask_Future12 = PSiteMask::fromUINT64(ONE64 << 47);
//extern const PSiteMask SiteBitmask_Future13 = PSiteMask::fromUINT64(ONE64 << 48);
//extern const PSiteMask SiteBitmask_Future14 = PSiteMask::fromUINT64(ONE64 << 49);
//extern const PSiteMask SiteBitmask_Future15 = PSiteMask::fromUINT64(ONE64 << 50);
//extern const PSiteMask SiteBitmask_Future16 = PSiteMask::fromUINT64(ONE64 << 51);
//extern const PSiteMask SiteBitmask_Future17 = PSiteMask::fromUINT64(ONE64 << 52);
//extern const PSiteMask SiteBitmask_Future18 = PSiteMask::fromUINT64(ONE64 << 53);
//extern const PSiteMask SiteBitmask_Future19 = PSiteMask::fromUINT64(ONE64 << 54);
//extern const PSiteMask SiteBitmask_Future20 = PSiteMask::fromUINT64(ONE64 << 55);
//extern const PSiteMask SiteBitmask_Future21 = PSiteMask::fromUINT64(ONE64 << 56);
//extern const PSiteMask SiteBitmask_Future22 = PSiteMask::fromUINT64(ONE64 << 57);
//extern const PSiteMask SiteBitmask_Future23 = PSiteMask::fromUINT64(ONE64 << 58);
//extern const PSiteMask SiteBitmask_Future24 = PSiteMask::fromUINT64(ONE64 << 59);
//extern const PSiteMask SiteBitmask_Future25 = PSiteMask::fromUINT64(ONE64 << 60);
//extern const PSiteMask SiteBitmask_Future26 = PSiteMask::fromUINT64(ONE64 << 61);
//extern const PSiteMask SiteBitmask_Future27 = PSiteMask::fromUINT64(ONE64 << 62);
//extern const PSiteMask SiteBitmask_Future28 = PSiteMask::fromUINT64(ONE64 << 63);
//extern const PSiteMask SiteBitmask_Future29 = PSiteMask::fromDbStringS("20000000000000000");	//	bit 65
//extern const PSiteMask SiteBitmask_Future30 = PSiteMask::fromDbStringS("40000000000000000");	//	bit 66
//extern const PSiteMask SiteBitmask_Future31 = PSiteMask::fromDbStringS("80000000000000000");	//	bit 67
//extern const PSiteMask SiteBitmask_Future32 = PSiteMask::fromDbStringS("100000000000000000");	//	bit 68
//extern const PSiteMask SiteBitmask_Future33 = PSiteMask::fromDbStringS("200000000000000000");	//	bit 69
//extern const PSiteMask SiteBitmask_Future34 = PSiteMask::fromDbStringS("400000000000000000");	//	bit 70
//extern const PSiteMask SiteBitmask_Future35 = PSiteMask::fromDbStringS("800000000000000000");	//	bit 71
//extern const PSiteMask SiteBitmask_Future36 = PSiteMask::fromDbStringS("1000000000000000000");	//	bit 72
//extern const PSiteMask SiteBitmask_Future37 = PSiteMask::fromDbStringS("2000000000000000000");	//	bit 73
//extern const PSiteMask SiteBitmask_Future38 = PSiteMask::fromDbStringS("4000000000000000000");	//	bit 74
//extern const PSiteMask SiteBitmask_Future39 = PSiteMask::fromDbStringS("8000000000000000000");	//	bit 75
//extern const PSiteMask SiteBitmask_Future40 = PSiteMask::fromDbStringS("10000000000000000000");	//	bit 76
//extern const PSiteMask SiteBitmask_Future41 = PSiteMask::fromDbStringS("40000000000000000000");	//	bit 78
//extern const PSiteMask SiteBitmask_Future42 = PSiteMask::fromDbStringS("80000000000000000000");	//	bit 79
//extern const PSiteMask SiteBitmask_Future43 = PSiteMask::fromDbStringS("100000000000000000000");	//	bit 80
//extern const PSiteMask SiteBitmask_Future44 = PSiteMask::fromDbStringS("200000000000000000000");	//	bit 81
//extern const PSiteMask SiteBitmask_Future45 = PSiteMask::fromDbStringS("400000000000000000000");	//	bit 82
//extern const PSiteMask SiteBitmask_Future46 = PSiteMask::fromDbStringS("800000000000000000000");	//	bit 83
//	==== @site2-list: List of all sites @ ==>>
////	From DB pov, define the last available mask which fits in LICENCEMASK_EX_LEN
////															           |12345678901234567890123456789012
//extern const PSiteMask SiteBitmask_FutureLast = PSiteMask::fromDbStringS("80000000000000000000000000000000");



//	Next site:	(0x40000000, as 0x80000000 is reserved by Majordomo!)
//
//		extern const PSiteMask SiteBitmask_nextMask		= PSiteMask::fromUINT64(0x100000000);
//
//		update SiteBitmask_AllValidMask accordingly

//	<<== @site2-last: Last bitmask @ ====
extern const PSiteMask SiteBitmask_Last = SiteBitmask_GR;	// Must be kept up to date
//	==== @site2-last: Last bitmask @ ==>>
extern const PSiteMask SiteBitmask_End = SiteBitmask_Last << 1;

//	
//	<<== @site2-rm: Real money sites @ ====
extern const PSiteMask SiteBitmask_RealMoneyMask =
(SiteBitmask_Com
	| SiteBitmask_Ita
	| SiteBitmask_Fra
	| SiteBitmask_Est
	| SiteBitmask_Bel
	| SiteBitmask_Spa
	| SiteBitmask_Den
	| SiteBitmask_EU
	| SiteBitmask_US_NJ
	| SiteBitmask_UK
	| SiteBitmask_Bul
	| SiteBitmask_DE_SH
	| SiteBitmask_Rom
	| SiteBitmask_Por
	| SiteBitmask_Cze
	| SiteBitmask_RUSO
	| SiteBitmask_Ind
	| SiteBitmask_Asia
	| SiteBitmask_RU
	| SiteBitmask_US_PA
	| SiteBitmask_Swe
	| SiteBitmask_CH
	| SiteBitmask_ARBA
	| SiteBitmask_US_CO
	| SiteBitmask_DE
	| SiteBitmask_GR
	);
//	==== @site2-rm: Real money sites @ ==>>
//Used as default for gaming objects, consult VadimSh if you change anything here
//	<<== @site2-iom-liquidity: IOM shared liquidity @ ====
extern const PSiteMask 	SiteBitmask_SharedLiquidity =
(SiteBitmask_Com
	| SiteBitmask_Est
	| SiteBitmask_Bel
	| SiteBitmask_Den
	| SiteBitmask_EU
	| SiteBitmask_UK
	| SiteBitmask_Bul
	| SiteBitmask_DE_SH
	| SiteBitmask_Por_Old
	| SiteBitmask_Rom
	| SiteBitmask_Cze
	| SiteBitmask_RUSO
	| SiteBitmask_Asia
	| SiteBitmask_Swe
	| SiteBitmask_CH
	| SiteBitmask_ARBA
	| SiteBitmask_GR
	);
//	==== @site2-iom-liquidity: IOM shared liquidity @ ==>>

//	<<== @site2-vip-stores: RM sites that have Vip Stores @ ====
const PSiteMask 	SiteBitmask_VipStoreSites =
(SiteBitmask_Com
	| SiteBitmask_Ita
	| SiteBitmask_Fra
	| SiteBitmask_Est
	| SiteBitmask_Bel
	| SiteBitmask_Spa
	| SiteBitmask_Den
	| SiteBitmask_EU
	| SiteBitmask_US_NJ
	| SiteBitmask_UK
	| SiteBitmask_Bul
	| SiteBitmask_DE_SH
	| SiteBitmask_Rom
	| SiteBitmask_Por
	| SiteBitmask_Cze
	| SiteBitmask_RUSO
	| SiteBitmask_Ind
	| SiteBitmask_Asia
	| SiteBitmask_US_PA
	| SiteBitmask_Swe
	| SiteBitmask_CH
	| SiteBitmask_ARBA
	| SiteBitmask_US_CO
	| SiteBitmask_DE
	);
//	==== @site2-vip-stores: RM sites that have Vip Stores @ ==>>
//	<<== @site2-site-pools: various site pools @ ====
extern const PSiteMask SiteBitmask_Segregated = (SiteBitmask_Ita | SiteBitmask_US_NJ | SiteBitmask_Ind | SiteBitmask_RU | SiteBitmask_US_PA | SiteBitmask_US_CO | SiteBitmask_DE);
extern const PSiteMask SiteBitmask_IOM_Segregated = (SiteBitmask_Ita | SiteBitmask_Fra | SiteBitmask_Spa | SiteBitmask_Por | SiteBitmask_Ind | SiteBitmask_RU);
extern const PSiteMask SiteBitmask_SharedLiquidityPool = (SiteBitmask_Fra | SiteBitmask_Spa | SiteBitmask_Por);
// FB included match definition in licenserules.cpp => LicenseRules::getCommonCurrencyOrNull 
extern const PSiteMask SiteBitmask_PM_only = (SiteBitmask_Net | SiteBitmask_Lite | SiteBitmask_FB | SiteBitmask_RUSOPM);
// FB ...
//	==== @site2-site-pools: various site pools @ ==>>


//	<<== @site2-all-sites: All valid site masks @ ====
extern const PSiteMask SiteBitmask_AllValidMask  =
		SiteBitmask_Com |
		SiteBitmask_Net |
		SiteBitmask_Lite |
		SiteBitmask_Ita |
		SiteBitmask_Fra |
		SiteBitmask_Est |
		SiteBitmask_ESPN |
		SiteBitmask_FOX |
		SiteBitmask_Bel |
		SiteBitmask_Spa |
		SiteBitmask_Den |
		SiteBitmask_EU |
		SiteBitmask_FB |
		SiteBitmask_US_NJ |
		SiteBitmask_UK |
		SiteBitmask_Bul |
		SiteBitmask_DE_SH |
		SiteBitmask_Por_Old |
		SiteBitmask_Rom |
		SiteBitmask_Por |
		SiteBitmask_Cze |
		SiteBitmask_RUSO |
		SiteBitmask_RUSOPM |
		SiteBitmask_Ind |
		SiteBitmask_Asia |
		SiteBitmask_RU |
		SiteBitmask_US_PA |
		SiteBitmask_Swe |
		SiteBitmask_CH |
		SiteBitmask_ARBA |
		SiteBitmask_US_CO |
		SiteBitmask_DE |
		SiteBitmask_GR;
//	==== @site2-all-sites: All valid site masks @ ==>>
extern const PSiteMask SiteBitmask_ALLMask = SiteBitmask_AllValidMask;

//	<<== @site2-pbitmask-to-site-number: Get site number (id) from site PBitMask @ ====
bool PSiteMask::getSiteNumber(Sites& siteNum) const
{
	if (*this == SiteBitmask_SiteUnknown)
	{
		siteNum = PokerStars_SiteUnknown;
		return true;
	}
	if (*this == SiteBitmask_Com)
	{
		siteNum = PokerStars_Com;
		return true;
	}
	if (*this == SiteBitmask_Net)
	{
		siteNum = PokerStars_Net;
		return true;
	}
	if (*this == SiteBitmask_Lite)
	{
		siteNum = PokerStars_Lite;
		return true;
	}
	if (*this == SiteBitmask_Ita)
	{
		siteNum = PokerStars_Ita;
		return true;
	}
	if (*this == SiteBitmask_Fra)
	{
		siteNum = PokerStars_Fra;
		return true;
	}
	if (*this == SiteBitmask_Est)
	{
		siteNum = PokerStars_Est;
		return true;
	}
	if (*this == SiteBitmask_ESPN)
	{
		siteNum = PokerStars_ESPN;
		return true;
	}
	if (*this == SiteBitmask_FOX)
	{
		siteNum = PokerStars_FOX;
		return true;
	}
	if (*this == SiteBitmask_Bel)
	{
		siteNum = PokerStars_Bel;
		return true;
	}
	if (*this == SiteBitmask_Spa)
	{
		siteNum = PokerStars_Spa;
		return true;
	}
	if (*this == SiteBitmask_Den)
	{
		siteNum = PokerStars_Den;
		return true;
	}
	if (*this == SiteBitmask_EU)
	{
		siteNum = PokerStars_EU;
		return true;
	}
	if (*this == SiteBitmask_FB)
	{
		siteNum = PokerStars_FB;
		return true;
	}
	if (*this == SiteBitmask_US_NJ)
	{
		siteNum = PokerStars_US_NJ;
		return true;
	}
	if (*this == SiteBitmask_UK)
	{
		siteNum = PokerStars_UK;
		return true;
	}
	if (*this == SiteBitmask_Bul)
	{
		siteNum = PokerStars_Bul;
		return true;
	}
	if (*this == SiteBitmask_DE_SH)
	{
		siteNum = PokerStars_DE_SH;
		return true;
	}
	if (*this == SiteBitmask_Por_Old)
	{
		siteNum = PokerStars_Por_Old;
		return true;
	}
	if (*this == SiteBitmask_Rom)
	{
		siteNum = PokerStars_Rom;
		return true;
	}
	if (*this == SiteBitmask_Por)
	{
		siteNum = PokerStars_Por;
		return true;
	}
	if (*this == SiteBitmask_Cze)
	{
		siteNum = PokerStars_Cze;
		return true;
	}
	if (*this == SiteBitmask_RUSO)
	{
		siteNum = PokerStars_RUSO;
		return true;
	}
	if (*this == SiteBitmask_RUSOPM)
	{
		siteNum = PokerStars_RUSOPM;
		return true;
	}
	if (*this == SiteBitmask_Ind)
	{
		siteNum = PokerStars_Ind;
		return true;
	}
	if (*this == SiteBitmask_Asia)
	{
		siteNum = PokerStars_Asia;
		return true;
	}
	if (*this == SiteBitmask_RU)
	{
		siteNum = PokerStars_RU;
		return true;
	}
	if (*this == SiteBitmask_US_PA)
	{
		siteNum = PokerStars_US_PA;
		return true;
	}
	if (*this == SiteBitmask_Swe)
	{
		siteNum = PokerStars_Swe;
		return true;
	}
	if (*this == SiteBitmask_CH)
	{
		siteNum = PokerStars_CH;
		return true;
	}
	if (*this == SiteBitmask_ARBA)
	{
		siteNum = PokerStars_ARBA;
		return true;
	}
	if (*this == SiteBitmask_US_CO)
	{
		siteNum = PokerStars_US_CO;
		return true;
	}
	if (*this == SiteBitmask_DE)
	{
		siteNum = PokerStars_DE;
		return true;
	}
	if (*this == SiteBitmask_GR)
	{
		siteNum = PokerStars_GR;
		return true;
	}
	/*
	add new masks as follows: (see sitemasks.h)
	if (*this == SiteBitmask_nextMask)
	{
	siteNum = PokerStars_newsite;
	return true;
	}
	also, you'll need to update the function getSiteMask() in coreserver\dbm\BitmaskUdf\BitmaskUdf\BitmaskUdf.c and possibly the #define's at the top of that file
	*/
	return false;
}
//	==== @site2-pbitmask-to-site-number: Get site number (id) from site PBitMask @ ==>>

const char * PSiteMask::toSiteListStr(PString& siteStr, const char* separator) const
{
	for (auto it = begin(); it != end(); ++it)
	{
		const Sites& site = *it;
		if (siteStr.length() > 0)
		{
			siteStr.append(separator);
		}
		siteStr.append(getSiteIdCodeWithNet(site));
	}
	return siteStr.c_str();
}

// PYR-114529 Moved from LicenseRules::getSiteMaskFromCommaSeparatedList.
bool PSiteMask::fromSiteListStr(const char * siteStr, const char* separator)
{
	*this = zero;

	if (!siteStr)
	{
		PLog("Error: siteList is undefined");
		return false;
	}

	if (!*siteStr)
	{
		PLog("siteList is empty string");
		return false;
	}

	vector<PString> tmpVector, tokens;
	StringVectorConvert::toStringVector(siteStr, tmpVector, separator);
	for (size_t i = 0; i < tmpVector.size(); ++i)
	{
		PString str(tmpVector[i].ltrim(), tmpVector[i].rtrim());
		if (str[0] != '\0')
			tokens.push_back(str);
	}

	for (const PString& token : tokens)
	{
		if (token.equals("-1") || token.equalsIgnoreCase("ALL"))
		{
			*this = SiteBitmask_ALLMask;
			return true;
		}

		Sites siteId;
		if (isdigit(token[0]))
		{
			// This is a site number.
			siteId = (Sites)strtoul(token.c_str(), nullptr, 10);
		}
		else
		{
			// We assume this is a site code.
			siteId = getSiteIdWithNet(token);
			if (siteId == PokerStars_SiteUnknown)
			{
				PLog("Error: invalid site '%s' in siteList", token.c_str());
				continue;
			}
		}

		PSiteMask siteIdMask;
		if (!maskFromSiteNumber(siteId, siteIdMask))
		{
			PLog("Error: cannot convert siteId=%u to PSiteMask when parsing siteList", siteId);
			continue;
		}

		*this |= siteIdMask;
	}

	return true;
}

PSiteMask::PSiteMaskIterator::PSiteMaskIterator(const PSiteMask& mask, INT32 bit)
	: siteMask(&mask)
	, currentBit(bit)
{}

PSiteMask::PSiteMaskIterator::PSiteMaskIterator(const PSiteMaskIterator& other)
{
	*this = other;
}

PSiteMask::PSiteMaskIterator& PSiteMask::PSiteMaskIterator::operator=(const PSiteMaskIterator& other)
{
	if (this != &other)
	{
		siteMask = other.siteMask;
		currentBit = other.currentBit;
	}

	return *this;
}

Sites PSiteMask::PSiteMaskIterator::operator*() const
{
	PSiteMask currentBitmask;
	currentBitmask.setBit(currentBit, true);
	Sites currentSite = PokerStars_SiteUnknown;
	currentBitmask.getSiteNumber(currentSite);
	return currentSite;
}

PSiteMask::PSiteMaskIterator& PSiteMask::PSiteMaskIterator::operator++()
{
	if (currentBit != EndBit)
	{
		// Advance to next bit.
		++currentBit;
		const decltype(currentBit) siteMaskSize(siteMask->size());
		while ((currentBit < siteMaskSize) && !siteMask->testBit(currentBit))
		{
			// Current bit is not set or needs to be skipped.
			++currentBit;
		}

		if (currentBit >= siteMaskSize)
		{
			// We reached the end of the mask.
			currentBit = EndBit;
		}
	}
	return *this;
}

PSiteMask::const_iterator PSiteMask::begin() const
{
	// Create iterator pointing to first bit.
	const_iterator it(*this, 0);
	if (!testBit(0))
	{
		// The first bit is not set, advance to the first set bit (or to the end, if no bit is set).
		++it;
	}
	return it;
}

PSiteMask::const_iterator PSiteMask::end() const
{
	return const_iterator(*this, PSiteMask::PSiteMaskIterator::EndBit);
}
// PYR-109216 - moved from licenserules.cpp
/*static*/ bool PSiteMask::maskFromSiteNumber(Sites siteNum, PSiteMask& bitmask)
{
	switch (siteNum)
	{
	case PokerStars_SiteUnknown:
//	<<== @site2-mask-from-number: (E) Get mask from site number @ ====
	case PokerStars_Com:
	case PokerStars_Net:
	case PokerStars_Lite:
	case PokerStars_Ita:
	case PokerStars_Fra:
	case PokerStars_Est:
	case PokerStars_ESPN:
	case PokerStars_FOX:
	case PokerStars_Bel:
	case PokerStars_Spa:
	case PokerStars_Den:
	case PokerStars_EU:
	case PokerStars_FB:
	case PokerStars_US_NJ:
	case PokerStars_UK:
	case PokerStars_Bul:
	case PokerStars_DE_SH:
	case PokerStars_Por_Old:
	case PokerStars_Rom:
	case PokerStars_Por:
	case PokerStars_Cze:
	case PokerStars_RUSO:
	case PokerStars_RUSOPM:
	case PokerStars_Ind:
	case PokerStars_Asia:
	case PokerStars_RU:
	case PokerStars_US_PA:
	case PokerStars_Swe:
	case PokerStars_CH:
//	==== @site2-mask-from-number: (E) Get mask from site number @ ==>>
	{
		bitmask = PSiteMask::fromUINT64(siteNum);
		return true;
	}
//	<<== @site2-bitmask-from-number: Get mask from site number @ ====
		case PokerStars_ARBA:
		{
		bitmask = SiteBitmask_ARBA;
		return true;
		}
		case PokerStars_US_CO:
		{
		bitmask = SiteBitmask_US_CO;
		return true;
		}
		case PokerStars_DE:
		{
		bitmask = SiteBitmask_DE;
		return true;
		}
		case PokerStars_GR:
		{
		bitmask = SiteBitmask_GR;
		return true;
		}
	// add new masks as follows: (see sitemasks.h)
	/*case PokerStars_newsite:
	{
	bitmask = SiteBitmask_nextMask;
	return true;
	}*/

	default:
		return false;
	}
//	==== @site2-bitmask-from-number: Get mask from site number @ ==>>
}

// PYR-109216 - moved from licenserules.cpp
bool PSiteMask::containsSiteId(UINT32 siteId) const
{
	PSiteMask siteBitmask;
	if (!maskFromSiteNumber((Sites)siteId, siteBitmask))
	{
		PTRACE(LOG_TRACE_FAULTY_LOGIC ": maskContainsSiteId: invalid siteId %u", siteId);
		return false;
	}
	return (*this & siteBitmask) != zero;
}

PSiteMask PSiteMask::fromUINT64(UINT64 n)
{
	PSiteMask b;
	if (n == 0x7FFFFFFF)
	{
		b = SiteBitmask_ALLMask;
	}
	else
	{
		b.setFromUINT64(n);
	}
	return b;
}

/*static */ PSiteMask PSiteMask::fromDbStringS(const char* hexStr)
{
	PSiteMask mask;
	mask.fromDbString ( hexStr );
	return mask;
}

const char* PSiteMask::toDbString(PString& hexStr) const
{
	if (*this == SiteBitmask_ALLMask)
	{
		hexStr.append(eternityAllStr());	//	Append and not assign, see PBitmask::toJSONString
		return hexStr.c_str();
	}
	return PBitmask::toDbString(hexStr);
}

void PSiteMask::fromDbString(const char* hexStr)
{
	if (!strcmp(hexStr, eternityAllStr()))
	{
		*this = SiteBitmask_ALLMask;
		return;
	}
	PBitmask::fromDbString(hexStr);
}

void PSiteMask::compose(CommMsgBody& body) const
{
	if (*this == SiteBitmask_ALLMask)
	{
		static const PBitmask eternityAll(eternityAllStr());
		return eternityAll.compose(body);
	}
	PBitmask::compose(body);
}

void PSiteMask::parse(CommMsgParser& parser)
{
	PBitmask::parse(parser);
	static const PBitmask eternityAll(eternityAllStr());
	if (eternityAll == *this)
	{
		*this = SiteBitmask_ALLMask;
	}
}

PSiteMask& PSiteMask::operator|=(const PSiteMask& other)
{
	PBitmask::operator|=(other);
	return *this;
}

PSiteMask& PSiteMask::operator^=(const PSiteMask& other)
{
	PBitmask::operator^=(other);
	return *this;
}

PSiteMask& PSiteMask::operator&=(const PSiteMask& other)
{
	PBitmask::operator&=(other);
	return *this;
}

PSiteMask& PSiteMask::operator<<=(INT32 n)
{
	PBitmask::operator<<=(n);
	return *this;
}

PSiteMask& PSiteMask::operator>>=(INT32 n)
{
	PBitmask::operator>>=(n);
	return *this;
}

PSiteMask& PSiteMask::andNot(const PSiteMask& other)
{
	PBitmask::andNot(other);
	return *this;
}

bool PSiteMask::operator==(const PSiteMask& other) const
{
	return PBitmask::operator==(other);
}

bool PSiteMask::operator!=(const PSiteMask& other) const
{
	return PBitmask::operator!=(other);
}

bool PSiteMask::intersects(const PSiteMask& other) const
{
	return PBitmask::intersects(other);
}
