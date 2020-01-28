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

#ifndef sitemasks_h_included
#define sitemasks_h_included

#include "PBitmask.h"
#include "sites.h"

#define SITE_UNKNOWN_CODE "UKN"

class PSiteMask : public PBitmask
{
	class PSiteMaskIterator : public std::iterator<forward_iterator_tag, Sites>
	{
		INT32 currentBit; // -1 indicates the end
		const PSiteMask* siteMask;

	public:
		PSiteMaskIterator(const PSiteMask& mask, INT32 bit);
		PSiteMaskIterator(const PSiteMaskIterator& other);
		PSiteMaskIterator& operator=(const PSiteMaskIterator& other);
		Sites operator*() const;
		PSiteMaskIterator& operator++();
		bool operator==(const PSiteMaskIterator& other) const
		{
			if (currentBit == EndBit && other.currentBit == EndBit)
			{
				return true;
			}

			return (siteMask == other.siteMask) && (currentBit == other.currentBit);
		}

		bool operator!=(const PSiteMaskIterator& other) const
		{
			return !operator==(other);
		}

		static const INT32 EndBit = -1;
	};

public:
	static const PSiteMask zero;

	PSiteMask(int) = delete;

	explicit PSiteMask(const char* str)
		: PBitmask(str)
	{}

	PSiteMask()
		: PBitmask()
	{}

	PSiteMask(const PSiteMask& other)
		: PBitmask(other)
	{}

	// PYR-114529 Temporary constructor, to be removed when all usage of PBitmask for sites is changed in order to use PSiteMask instead.
	PSiteMask(const PBitmask& other)
		: PBitmask(other)
	{}

	PSiteMask& operator=(const PSiteMask& other)
	{
		PBitmask::operator=(other);
		return *this;
	}

	PSiteMask(PSiteMask&& other)
		: PBitmask(std::move(other))
	{}

	PSiteMask& operator=(PSiteMask&& other)
	{
		PBitmask::operator=(std::move(other));
		return *this;
	}

	bool getSiteNumber(Sites& siteNum) const;

	const char* toSiteListStr(PString& siteStr, const char* separator = ".") const;

	// PYR-114529 Moved from LicenseRules::getSiteMaskFromCommaSeparatedList.
	bool fromSiteListStr(const char* siteStr, const char* separator = ".");

	typedef PSiteMaskIterator const_iterator;
	const_iterator begin() const;
	const_iterator end() const;

	bool containsSiteId(UINT32 siteId) const;

	static bool maskFromSiteNumber(Sites siteNum, PSiteMask& bitmask);

	static PSiteMask fromUINT64(UINT64 n);
	static PSiteMask fromDbStringS(const char* hexStr);
	static const char* eternityAllStr() {return "7FFFFFFF";}

	virtual const char* toDbString(PString& hexStr) const override;
	virtual void fromDbString(const char* hexStr) override;
	using PBitmask::fromDbString; // for the `void fromDbString(const char* dbString, UINT64 integerMaskFallback)` overload

	virtual void compose(CommMsgBody& body) const override;
	virtual void parse(CommMsgParser& parser) override;

	PSiteMask& operator|=(const PSiteMask& other);
	PSiteMask& operator^=(const PSiteMask& other);
	PSiteMask& operator&=(const PSiteMask& other);
	PSiteMask& operator<<=(INT32 n);
	PSiteMask& operator>>=(INT32 n);
	bool operator==(const PSiteMask& other) const;
	bool operator!=(const PSiteMask& other) const;
	bool intersects(const PSiteMask& other) const;
	PSiteMask& andNot(const PSiteMask& other);

	// PYR-114529 Moved from LicenseRules.
	static const char* getSiteIdCode(Sites siteId)
	{
		if (siteId < PokerStars_Ita
			|| siteId == PokerStars_RUSO
			|| siteId == PokerStars_RUSOPM
			)
		{
			return "COM";
		}
		switch (siteId)
		{
//	<<== @site2-site-id-to-code: (E) Get site code from site id @ ====
		case PokerStars_Ita:
			return "IT";
		case PokerStars_Fra:
			return "FR";
		case PokerStars_Est:
			return "EE";
		case PokerStars_ESPN:
			return "ESPN";
		case PokerStars_FOX:
			return "FOX";
		case PokerStars_Bel:
			return "BE";
		case PokerStars_Spa:
			return "ES";
		case PokerStars_Den:
			return "DK";
		case PokerStars_EU:
			return "EU";
		case PokerStars_FB:
			return "FB";
		case PokerStars_US_NJ:
			return "USNJ";
		case PokerStars_UK:
			return "UK";
		case PokerStars_Bul:
			return "BG";
		case PokerStars_DE_SH:
			return "DESH";
		case PokerStars_Rom:
			return "RO";
		case PokerStars_Por:
			return "PT";
		case PokerStars_Cze:
			return "CZ";
		case PokerStars_Ind:
			return "IN";
		case PokerStars_Asia:
			return "ASIA";
		case PokerStars_RU:
			return "RU";
		case PokerStars_US_PA:
			return "USPA";
		case PokerStars_Swe:
			return "SE";
		case PokerStars_CH:
			return "CH";
		case PokerStars_ARBA:
			return "ARBA";
		case PokerStars_US_CO:
			return "USCO";
		case PokerStars_DE:
			return "DE";
		case PokerStars_GR:
			return "GR";
//	==== @site2-site-id-to-code: (E) Get site code from site id @ ==>>
		default:
			return SITE_UNKNOWN_CODE;
		}
	}

	// PYR-114529 Moved from LicenseRules.
	//The name getSiteIdCodeWithNet is misleading. In reality - can recognize NET, RUSO, RUSOPM.
	static const char* getSiteIdCodeWithNet(Sites siteId)
	{
//	<<== @site2-iom-non-com-sites: (E) IOM license non COM sites: id to code @ ====
		if (siteId == PokerStars_Net)
			return "NET";
		if (siteId == PokerStars_RUSO)
			return "RUSO";
		if (siteId == PokerStars_RUSOPM)
			return "RUSOPM";
//	==== @site2-iom-non-com-sites: (E) IOM license non COM sites: id to code @ ==>>
		return getSiteIdCode(siteId);
	}


	// PYR-114529 Moved from LicenseRules.
	static Sites getSiteId(const char* siteCode)
	{
		if (!siteCode || !*siteCode)
		{
			return PokerStars_SiteUnknown;
		}
//	<<== @site2-site-code-to-enumeration: (E) Get site enumeration from site code @ ====
		if (PString::equals(siteCode, "COM")
			|| PString::equals(siteCode, "RUSO")
			)
		{
			return PokerStars_Com;
		}
		if (PString::equals(siteCode, "IT"))
		{
			return PokerStars_Ita;
		}
		if (PString::equals(siteCode, "FR"))
		{
			return PokerStars_Fra;
		}
		if (PString::equals(siteCode, "EE"))
		{
			return PokerStars_Est;
		}
		if (PString::equals(siteCode, "ESPN"))
		{
			return PokerStars_ESPN;
		}
		if (PString::equals(siteCode, "FOX"))
		{
			return PokerStars_FOX;
		}
		if (PString::equals(siteCode, "BE"))
		{
			return PokerStars_Bel;
		}
		if (PString::equals(siteCode, "ES"))
		{
			return PokerStars_Spa;
		}
		if (PString::equals(siteCode, "DK"))
		{
			return PokerStars_Den;
		}
		if (PString::equals(siteCode, "EU"))
		{
			return PokerStars_EU;
		}
		if (PString::equals(siteCode, "FB"))
		{
			return PokerStars_FB;
		}
		if (PString::equals(siteCode, "USNJ"))
		{
			return PokerStars_US_NJ;
		}
		if (PString::equals(siteCode, "UK"))
		{
			return PokerStars_UK;
		}
		if (PString::equals(siteCode, "BG"))
		{
			return PokerStars_Bul;
		}
		if (PString::equals(siteCode, "DESH"))
		{
			return PokerStars_DE_SH;
		}
		if (PString::equals(siteCode, "RO"))
		{
			return PokerStars_Rom;
		}
		if (PString::equals(siteCode, "PT"))
		{
			return PokerStars_Por;
		}
		if (PString::equals(siteCode, "CZ"))
		{
			return PokerStars_Cze;
		}
		if (PString::equals(siteCode, "IN"))
		{
			return PokerStars_Ind;
		}
		if (PString::equals(siteCode, "ASIA"))
		{
			return PokerStars_Asia;
		}
		if (PString::equals(siteCode, "RU"))
		{
			return PokerStars_RU;
		}
		if (PString::equals(siteCode, "USPA"))
		{
			return PokerStars_US_PA;
		}
		if (PString::equals(siteCode, "SE"))
		{
			return PokerStars_Swe;
		}
		if (PString::equals(siteCode, "CH"))
		{
			return PokerStars_CH;
		}
		if (PString::equals(siteCode, "ARBA"))
		{
			return PokerStars_ARBA;
		}
		if (PString::equals(siteCode, "USCO"))
		{
			return PokerStars_US_CO;
		}
		if (PString::equals(siteCode, "DE"))
		{
			return PokerStars_DE;
		}
		if (PString::equals(siteCode, "GR"))
		{
			return PokerStars_GR;
		}
//	==== @site2-site-code-to-enumeration: (E) Get site enumeration from site code @ ==>>
		//PYR-36050 -this is NOT a valid site name, internal Majordomo use only
		if (PString::equals(siteCode, "NIL"))
		{
			return (Sites)(1 << 31);
		}

		return PokerStars_SiteUnknown; // Unknown
	}

	// PYR-114529 Moved from LicenseRules.
	//The name getSiteIdWithNet is misleading. In reality - can recognize NET, RUSO, RUSOPM.
	static Sites getSiteIdWithNet(const char* siteCode)
	{
		if (!siteCode || !*siteCode)
		{
			return PokerStars_SiteUnknown;
		}
//	<<== @site2-iom-non-com-sites2: (E) IOM license non COM sites: code to id @ ====
		if (PString::equals(siteCode, "NET"))
		{
			return PokerStars_Net;
		}
		if (PString::equals(siteCode, "RUSO"))
		{
			return PokerStars_RUSO;
		}
		if (PString::equals(siteCode, "RUSOPM"))
		{
			return PokerStars_RUSOPM;
		}
//	==== @site2-iom-non-com-sites2: (E) IOM license non COM sites: code to id @ ==>>
		return getSiteId(siteCode);
	}

};

inline PSiteMask operator|(const PSiteMask& left, const PSiteMask& right)
{
	PSiteMask answer = left;
	return answer.operator|=(right);
}

inline PSiteMask operator&(const PSiteMask& left, const PSiteMask& right)
{
	PSiteMask answer = left;
	return answer.operator&=(right);
}

// PYR-114529 Temporary operator, to be removed when all usage of PBitmask for sites is changed in order to use PSiteMask instead.
inline PSiteMask operator&(const PBitmask& left, const PSiteMask& right)
{
	PSiteMask answer = left;
	return answer.operator&=(right);
}

// PYR-114529 Temporary operator, to be removed when all usage of PBitmask for sites is changed in order to use PSiteMask instead.
inline PSiteMask operator&(const PSiteMask& left, const PBitmask& right)
{
	PSiteMask answer = left;
	return answer.operator&=(right);
}

inline PSiteMask operator^(const PSiteMask& left, const PSiteMask& right)
{
	PSiteMask answer = left;
	return answer.operator^=(right);
}

// PYR-114529 Temporary operator, to be removed when all usage of PBitmask for sites is changed in order to use PSiteMask instead.
inline PSiteMask operator^(const PBitmask& left, const PSiteMask& right)
{
	PSiteMask answer = left;
	return answer.operator^=(right);
}

inline PSiteMask operator>>(const PSiteMask& left, INT32 shift)
{
	PSiteMask answer = left;
	return answer.operator>>=(shift);
}

inline PSiteMask operator<<(const PSiteMask& left, INT32 shift)
{
	PSiteMask answer = left;
	return answer.operator<<=(shift);
}

inline PSiteMask andNot(const PSiteMask& left, const PSiteMask& right)
{
	PSiteMask answer = left;
	return answer.andNot(right);
}

// PYR-114529 Temporary method, to be removed when all usage of PBitmask for sites is changed in order to use PSiteMask instead.
inline PSiteMask andNot(const PBitmask& left, const PSiteMask& right)
{
	PSiteMask answer = left;
	return answer.andNot(right);
}

extern const PSiteMask SiteBitmask_SiteUnknown;

//	<<== @site2-list-full: All site bitmasks @ ====
extern const PSiteMask SiteBitmask_Com;		//	1
extern const PSiteMask SiteBitmask_Net;		//	2
extern const PSiteMask SiteBitmask_Lite;	//	4
extern const PSiteMask SiteBitmask_Ita;		//	16
extern const PSiteMask SiteBitmask_Fra;		//	32
extern const PSiteMask SiteBitmask_Est;		//	64
extern const PSiteMask SiteBitmask_ESPN;	//	128
extern const PSiteMask SiteBitmask_FOX;		//	256
extern const PSiteMask SiteBitmask_Bel;		//	512
extern const PSiteMask SiteBitmask_Spa;		//	1024
extern const PSiteMask SiteBitmask_Den;		//	2048
extern const PSiteMask SiteBitmask_EU;		//	4096
extern const PSiteMask SiteBitmask_FB;		//	8192
extern const PSiteMask SiteBitmask_US_NJ;	//	16384
extern const PSiteMask SiteBitmask_UK;		//	32768
extern const PSiteMask SiteBitmask_Bul;		//	65536
extern const PSiteMask SiteBitmask_DE_SH;	//	131072
extern const PSiteMask SiteBitmask_Por_Old;	//	262144
extern const PSiteMask SiteBitmask_Rom;		//	524288
extern const PSiteMask SiteBitmask_Por;		//	1048576
extern const PSiteMask SiteBitmask_Cze;		//	2097152
extern const PSiteMask SiteBitmask_RUSO;	//	4194304
extern const PSiteMask SiteBitmask_RUSOPM;	//	8388608
extern const PSiteMask SiteBitmask_Ind;		//	16777216
extern const PSiteMask SiteBitmask_Asia;	//	33554432
extern const PSiteMask SiteBitmask_RU;		//	67108864
extern const PSiteMask SiteBitmask_US_PA;	//	134217728
extern const PSiteMask SiteBitmask_Swe;		//	268435456
extern const PSiteMask SiteBitmask_CH;		//	536870912
extern const PSiteMask SiteBitmask_ARBA;	//	536870913
extern const PSiteMask SiteBitmask_US_CO;	//	536870914
extern const PSiteMask SiteBitmask_DE;		//	536870915
extern const PSiteMask SiteBitmask_GR;		//	536870957
//	==== @site2-list-full: All site bitmasks @ ==>>

extern const PSiteMask SiteBitmask_Last;
extern const PSiteMask SiteBitmask_End;

extern const PSiteMask SiteBitmask_RealMoneyMask;
extern const PSiteMask SiteBitmask_SharedLiquidity;

extern const PSiteMask SiteBitmask_Segregated;
extern const PSiteMask SiteBitmask_IOM_Segregated;
extern const PSiteMask SiteBitmask_SharedLiquidityPool;
extern const PSiteMask SiteBitmask_PM_only;
extern const PSiteMask SiteBitmask_VipStoreSites;

extern const PSiteMask SiteBitmask_AllValidMask;
extern const PSiteMask SiteBitmask_ALLMask;

inline PSiteMask operator~(const PSiteMask& input)
{
	return andNot(SiteBitmask_AllValidMask, input);
}

#endif // sitemasks_h_included
