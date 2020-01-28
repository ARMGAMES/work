// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Parts of this file are being maintained using automated scripts.
// The script maintenance parts are surrounded by language-specific
// comment lines that begin with <<== and end with ==>> markers.
// When required, manual changes can made to this file as usual. 
// Such changes should be communicated to the current maintainer 
// of GIT module in ADMIN/asppages/admin/_licenses. 
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

#include "licensemask.h"
#include "pplogfile.h"
#include "vectorutils.h"

const PLicenseMask PLicenseMask::zero;

extern const PLicenseMask LicenceBitmask_Default = PLicenseMask::fromUINT64(1);

#define LEGACY_LICENCE_BITMASK( name ) extern const PLicenseMask LicenceBitmask_##name( PLicenseMask::fromUINT64( eLicence##name ) )

//	<<== @license2-list: List of all licenses @ ====
LEGACY_LICENCE_BITMASK( Italy );
LEGACY_LICENCE_BITMASK( France );
LEGACY_LICENCE_BITMASK( Estonia );
LEGACY_LICENCE_BITMASK( Belgium );
LEGACY_LICENCE_BITMASK( Spain );
LEGACY_LICENCE_BITMASK( Denmark );
LEGACY_LICENCE_BITMASK( EU );
LEGACY_LICENCE_BITMASK( FB );
LEGACY_LICENCE_BITMASK( USNJ );
LEGACY_LICENCE_BITMASK( UK );
LEGACY_LICENCE_BITMASK( BG );
LEGACY_LICENCE_BITMASK( DESH );
LEGACY_LICENCE_BITMASK( Portugal_Old );
LEGACY_LICENCE_BITMASK( Romania );
LEGACY_LICENCE_BITMASK( Portugal );
LEGACY_LICENCE_BITMASK( CZ );
LEGACY_LICENCE_BITMASK( India );
LEGACY_LICENCE_BITMASK( Asia );
LEGACY_LICENCE_BITMASK( RU );
LEGACY_LICENCE_BITMASK( USPA );
LEGACY_LICENCE_BITMASK( SE );
LEGACY_LICENCE_BITMASK( CH );

#undef LEGACY_LICENCE_BITMASK

extern const PLicenseMask LicenceBitmask_UNDEFINED = PLicenseMask::fromINT64( eLicenceUNDEFINED );

  extern const PLicenseMask LicenceBitmask_ARBA = PLicenseMask::fromUINT64(ONE64 << 33);
  extern const PLicenseMask LicenceBitmask_USCO = PLicenseMask::fromUINT64(ONE64 << 34);
  extern const PLicenseMask LicenceBitmask_DE = PLicenseMask::fromUINT64(ONE64 << 35);
  extern const PLicenseMask LicenceBitmask_GR = PLicenseMask::fromDbStringS("40000000000000000");	//	bit 66

//	Future masks must be defined as follows:
//extern const PLicenseMask LicenceBitmask_Future1 = PLicenseMask::fromUINT64(ONE64 << 36);
//extern const PLicenseMask LicenceBitmask_Future2 = PLicenseMask::fromUINT64(ONE64 << 37);
//extern const PLicenseMask LicenceBitmask_Future3 = PLicenseMask::fromUINT64(ONE64 << 38);
//extern const PLicenseMask LicenceBitmask_Future4 = PLicenseMask::fromUINT64(ONE64 << 39);
//extern const PLicenseMask LicenceBitmask_Future5 = PLicenseMask::fromUINT64(ONE64 << 40);
//extern const PLicenseMask LicenceBitmask_Future6 = PLicenseMask::fromUINT64(ONE64 << 41);
//extern const PLicenseMask LicenceBitmask_Future7 = PLicenseMask::fromUINT64(ONE64 << 42);
//extern const PLicenseMask LicenceBitmask_Future8 = PLicenseMask::fromUINT64(ONE64 << 43);
//extern const PLicenseMask LicenceBitmask_Future9 = PLicenseMask::fromUINT64(ONE64 << 44);
//extern const PLicenseMask LicenceBitmask_Future10 = PLicenseMask::fromUINT64(ONE64 << 45);
//extern const PLicenseMask LicenceBitmask_Future11 = PLicenseMask::fromUINT64(ONE64 << 46);
//extern const PLicenseMask LicenceBitmask_Future12 = PLicenseMask::fromUINT64(ONE64 << 47);
//extern const PLicenseMask LicenceBitmask_Future13 = PLicenseMask::fromUINT64(ONE64 << 48);
//extern const PLicenseMask LicenceBitmask_Future14 = PLicenseMask::fromUINT64(ONE64 << 49);
//extern const PLicenseMask LicenceBitmask_Future15 = PLicenseMask::fromUINT64(ONE64 << 50);
//extern const PLicenseMask LicenceBitmask_Future16 = PLicenseMask::fromUINT64(ONE64 << 51);
//extern const PLicenseMask LicenceBitmask_Future17 = PLicenseMask::fromUINT64(ONE64 << 52);
//extern const PLicenseMask LicenceBitmask_Future18 = PLicenseMask::fromUINT64(ONE64 << 53);
//extern const PLicenseMask LicenceBitmask_Future19 = PLicenseMask::fromUINT64(ONE64 << 54);
//extern const PLicenseMask LicenceBitmask_Future20 = PLicenseMask::fromUINT64(ONE64 << 55);
//extern const PLicenseMask LicenceBitmask_Future21 = PLicenseMask::fromUINT64(ONE64 << 56);
//extern const PLicenseMask LicenceBitmask_Future22 = PLicenseMask::fromUINT64(ONE64 << 57);
//extern const PLicenseMask LicenceBitmask_Future23 = PLicenseMask::fromUINT64(ONE64 << 58);
//extern const PLicenseMask LicenceBitmask_Future24 = PLicenseMask::fromUINT64(ONE64 << 59);
//extern const PLicenseMask LicenceBitmask_Future25 = PLicenseMask::fromUINT64(ONE64 << 60);
//extern const PLicenseMask LicenceBitmask_Future26 = PLicenseMask::fromUINT64(ONE64 << 61);
//extern const PLicenseMask LicenceBitmask_Future27 = PLicenseMask::fromUINT64(ONE64 << 62);
//extern const PLicenseMask LicenceBitmask_Future28 = PLicenseMask::fromUINT64(ONE64 << 63);
//extern const PLicenseMask LicenceBitmask_Future29 = PLicenseMask::fromDbStringS("20000000000000000");	//	bit 65
//extern const PLicenseMask LicenceBitmask_Future30 = PLicenseMask::fromDbStringS("80000000000000000");	//	bit 67
//extern const PLicenseMask LicenceBitmask_Future31 = PLicenseMask::fromDbStringS("100000000000000000");	//	bit 68
//extern const PLicenseMask LicenceBitmask_Future32 = PLicenseMask::fromDbStringS("200000000000000000");	//	bit 69
//extern const PLicenseMask LicenceBitmask_Future33 = PLicenseMask::fromDbStringS("400000000000000000");	//	bit 70
//extern const PLicenseMask LicenceBitmask_Future34 = PLicenseMask::fromDbStringS("800000000000000000");	//	bit 71
//extern const PLicenseMask LicenceBitmask_Future35 = PLicenseMask::fromDbStringS("1000000000000000000");	//	bit 72
//extern const PLicenseMask LicenceBitmask_Future36 = PLicenseMask::fromDbStringS("2000000000000000000");	//	bit 73
//extern const PLicenseMask LicenceBitmask_Future37 = PLicenseMask::fromDbStringS("4000000000000000000");	//	bit 74
//extern const PLicenseMask LicenceBitmask_Future38 = PLicenseMask::fromDbStringS("8000000000000000000");	//	bit 75
//extern const PLicenseMask LicenceBitmask_Future39 = PLicenseMask::fromDbStringS("10000000000000000000");	//	bit 76
//extern const PLicenseMask LicenceBitmask_Future40 = PLicenseMask::fromDbStringS("20000000000000000000");	//	bit 77
//extern const PLicenseMask LicenceBitmask_Future41 = PLicenseMask::fromDbStringS("40000000000000000000");	//	bit 78
//extern const PLicenseMask LicenceBitmask_Future42 = PLicenseMask::fromDbStringS("80000000000000000000");	//	bit 79
//extern const PLicenseMask LicenceBitmask_Future43 = PLicenseMask::fromDbStringS("100000000000000000000");	//	bit 80
//extern const PLicenseMask LicenceBitmask_Future44 = PLicenseMask::fromDbStringS("200000000000000000000");	//	bit 81
//extern const PLicenseMask LicenceBitmask_Future45 = PLicenseMask::fromDbStringS("400000000000000000000");	//	bit 82
//extern const PLicenseMask LicenceBitmask_Future46 = PLicenseMask::fromDbStringS("800000000000000000000");	//	bit 83
//	==== @license2-list: List of all licenses @ ==>>
////	From DB pov, define the last available mask which fits in LICENCEMASK_EX_LEN
////																				|12345678901234567890123456789012
//extern const PLicenseMask LicenceBitmask_FutureLast = PLicenseMask::fromDbStringS("80000000000000000000000000000000");



// update LicenceBitmask_All below accordingly when adding new licence

//	<<== @license2-last: Last license bitmask @ ====
extern const PLicenseMask LicenceBitmask_Last				= LicenceBitmask_GR;
//	==== @license2-last: Last license bitmask @ ==>>
//	<<== @license2-shared-liquidity: Shared liguidity licenses @ ====
extern const PLicenseMask LicenceBitmask_SharedLiquidityPool	= LicenceBitmask_France | LicenceBitmask_Spain | LicenceBitmask_Portugal;
//	==== @license2-shared-liquidity: Shared liguidity licenses @ ==>>

// PYR-99717 - Add new licences at the end.
//	<<== @license2-all-lic-mask: Bitmask for all licenses @ ====
extern const PLicenseMask LicenceBitmask_All			= LicenceBitmask_Default
														| LicenceBitmask_Italy
														| LicenceBitmask_France
														| LicenceBitmask_Estonia
														| LicenceBitmask_Belgium
														| LicenceBitmask_Spain
														| LicenceBitmask_Denmark
														| LicenceBitmask_EU
														| LicenceBitmask_FB
														| LicenceBitmask_USNJ
														| LicenceBitmask_UK
														| LicenceBitmask_BG
														| LicenceBitmask_DESH
														| LicenceBitmask_Portugal_Old
														| LicenceBitmask_Romania
														| LicenceBitmask_Portugal
														| LicenceBitmask_CZ
														| LicenceBitmask_India
														| LicenceBitmask_Asia
														| LicenceBitmask_RU
														| LicenceBitmask_USPA
														| LicenceBitmask_SE
														| LicenceBitmask_CH
														| LicenceBitmask_ARBA
														| LicenceBitmask_USCO
														| LicenceBitmask_DE
														| LicenceBitmask_GR;
bool PLicenseMask::getLicenseId(LicenceId& licenseId) const
{
	if (*this == LicenceBitmask_Default)
	{
		licenseId = eLicenceDefault;
		return true;
	}
	if (*this == LicenceBitmask_Italy)
	{
		licenseId = eLicenceItaly;
		return true;
	}
	if (*this == LicenceBitmask_France)
	{
		licenseId = eLicenceFrance;
		return true;
	}
	if (*this == LicenceBitmask_Estonia)
	{
		licenseId = eLicenceEstonia;
		return true;
	}
	if (*this == LicenceBitmask_Belgium)
	{
		licenseId = eLicenceBelgium;
		return true;
	}
	if (*this == LicenceBitmask_Spain)
	{
		licenseId = eLicenceSpain;
		return true;
	}
	if (*this == LicenceBitmask_Denmark)
	{
		licenseId = eLicenceDenmark;
		return true;
	}
	if (*this == LicenceBitmask_EU)
	{
		licenseId = eLicenceEU;
		return true;
	}
	if (*this == LicenceBitmask_FB)
	{
		licenseId = eLicenceFB;
		return true;
	}
	if (*this == LicenceBitmask_USNJ)
	{
		licenseId = eLicenceUSNJ;
		return true;
	}
	if (*this == LicenceBitmask_UK)
	{
		licenseId = eLicenceUK;
		return true;
	}
	if (*this == LicenceBitmask_BG)
	{
		licenseId = eLicenceBG;
		return true;
	}
	if (*this == LicenceBitmask_DESH)
	{
		licenseId = eLicenceDESH;
		return true;
	}
	if (*this == LicenceBitmask_Portugal_Old)
	{
		licenseId = eLicencePortugal_Old;
		return true;
	}
	if (*this == LicenceBitmask_Romania)
	{
		licenseId = eLicenceRomania;
		return true;
	}
	if (*this == LicenceBitmask_Portugal)
	{
		licenseId = eLicencePortugal;
		return true;
	}
	if (*this == LicenceBitmask_CZ)
	{
		licenseId = eLicenceCZ;
		return true;
	}
	if (*this == LicenceBitmask_India)
	{
		licenseId = eLicenceIndia;
		return true;
	}
	if (*this == LicenceBitmask_Asia)
	{
		licenseId = eLicenceAsia;
		return true;
	}
	if (*this == LicenceBitmask_RU)
	{
		licenseId = eLicenceRU;
		return true;
	}
	if (*this == LicenceBitmask_USPA)
	{
		licenseId = eLicenceUSPA;
		return true;
	}
	if (*this == LicenceBitmask_SE)
	{
		licenseId = eLicenceSE;
		return true;
	}
	if (*this == LicenceBitmask_CH)
	{
		licenseId = eLicenceCH;
		return true;
	}
	if (*this == LicenceBitmask_ARBA)
	{
		licenseId = eLicenceARBA;
		return true;
	}
	if (*this == LicenceBitmask_USCO)
	{
		licenseId = eLicenceUSCO;
		return true;
	}
	if (*this == LicenceBitmask_DE)
	{
		licenseId = eLicenceDE;
		return true;
	}
	if (*this == LicenceBitmask_GR)
	{
		licenseId = eLicenceGR;
		return true;
	}
	/*
	add new masks as follows: (see licensemask.h)
	if (*this == LicenceBitmask_LicenseNew)
	{
	licenseId = eLicenceNew;
	return true;
	}
	also, you'll need to update the function getLicenseMask() in coreserver\dbm\BitmaskUdf\BitmaskUdf\BitmaskUdf.c and possibly the #define's at the top of that file
	*/
	return false;
}

// PYR-114529 Moved from LicenseRules.
/*static*/ bool PLicenseMask::maskFromLicenseId(LicenceId licenseId, PLicenseMask& bitmask)
{
	switch (licenseId)
	{
	case eLicenceDefault:
	case _LicenceDefaultBit_ForMaskUseOnly:
	{
		bitmask = LicenceBitmask_Default;
		return true;
	}

	case eLicenceItaly:
	case eLicenceFrance:
	case eLicenceEstonia:
	case eLicenceBelgium:
	case eLicenceSpain:
	case eLicenceDenmark:
	case eLicenceEU:
	case eLicenceFB:
	case eLicenceUSNJ:
	case eLicenceUK:
	case eLicenceBG:
	case eLicenceDESH:
	case eLicencePortugal_Old:
	case eLicenceRomania:
	case eLicencePortugal:
	case eLicenceCZ:
	case eLicenceIndia:
	case eLicenceAsia:
	case eLicenceRU:
	case eLicenceUSPA:
	case eLicenceSE:
	case eLicenceCH:
	{
		bitmask = PLicenseMask::fromUINT64(licenseId);
		return true;
	}

	case eLicenceARBA:
	{
		bitmask = LicenceBitmask_ARBA;
		return true;
	}
	case eLicenceUSCO:
	{
		bitmask = LicenceBitmask_USCO;
		return true;
	}
	case eLicenceDE:
	{
		bitmask = LicenceBitmask_DE;
		return true;
	}
	case eLicenceGR:
	{
		bitmask = LicenceBitmask_GR;
		return true;
	}
	/*
	add new masks as follows: (see licensemask.h)
	case eLicenceNew:
	{
		bitmask = LicenceBitmask_LicenseNew;
		return true;
	}
	*/

	default:
		return false;
	}
}
//	==== @license2-all-lic-mask: Bitmask for all licenses @ ==>>


const char * PLicenseMask::toLicenseListStr(PString& licStr, const char* separator) const
{
	for (auto it = begin(); it != end(); ++it)
	{
		const LicenceId& licId = *it;
		if (licStr.length() > 0)
		{
			licStr.append(separator);
		}
		licStr.append(getLicenseIdCode(licId));
	}
	return licStr.c_str();
}

// PYR-114529 Moved from LicenseRules.
bool PLicenseMask::fromLicenseListStr(const char * licStr, const char* separator)
{
	*this = zero;
	if (!licStr)
	{
		PLog("Error: licenseList is undefined");
		return false;
	}

	if (!*licStr)
	{
		PLog("licenseList is empty string");
		return false;
	}

	vector<PString> tmpVector, tokens;
	StringVectorConvert::toStringVector(licStr, tmpVector, separator);
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
			*this = LicenceBitmask_All;
			return true;
		}

		LicenceId licenseId;
		if (isdigit(token[0]))
		{
			// This is a license number.
			licenseId = (LicenceId)strtoul(token.c_str(), nullptr, 10);
		}
		else
		{
			// We assume this is a license code.
			if (!licenseNameToId(token, licenseId))
			{
				PLog("Error: invalid licenseId %s in licenseList", token.c_str());
				continue;
			}
		}

		PLicenseMask licenseIdMask;
		if (!maskFromLicenseId(licenseId, licenseIdMask))
		{
			PLog("Error: cannot convert licenseId=%d to PLicenseMask", licenseId);
			continue;
		}

		*this |= licenseIdMask;
	}

	return true;
}

PLicenseMask::PLicenseMaskIterator::PLicenseMaskIterator(const PLicenseMask& mask, INT32 bit)
	: licenseMask(&mask)
	, currentBit(bit)
{}

PLicenseMask::PLicenseMaskIterator::PLicenseMaskIterator(const PLicenseMaskIterator& other)
{
	*this = other;
}

PLicenseMask::PLicenseMaskIterator& PLicenseMask::PLicenseMaskIterator::operator=(const PLicenseMaskIterator& other)
{
	if (this != &other)
	{
		licenseMask = other.licenseMask;
		currentBit = other.currentBit;
	}

	return *this;
}

LicenceId PLicenseMask::PLicenseMaskIterator::operator*() const
{
	PLicenseMask currentBitmask;
	currentBitmask.setBit(currentBit, true);
	LicenceId currentLicense = eLicenceUNDEFINED;
	currentBitmask.getLicenseId(currentLicense);
	return currentLicense;
}

PLicenseMask::PLicenseMaskIterator& PLicenseMask::PLicenseMaskIterator::operator++()
{
	if (currentBit != EndBit)
	{
		// Advance to next bit.
		++currentBit;
		while (currentBit < static_cast<INT32>(licenseMask->size()) && !licenseMask->testBit(currentBit))
		{
			// Current bit is not set or needs to be skipped.
			++currentBit;
		}

		if (currentBit >= static_cast<INT32>(licenseMask->size()))
		{
			// We reached the end of the mask.
			currentBit = EndBit;
		}
	}
	return *this;
}

PLicenseMask::const_iterator PLicenseMask::begin() const
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

PLicenseMask::const_iterator PLicenseMask::end() const
{
	return const_iterator(*this, PLicenseMask::PLicenseMaskIterator::EndBit);
}

PLicenseMask PLicenseMask::fromUINT64(UINT64 n)
{
	PLicenseMask b;
	if (n == 0x7FFFFFFF)
	{
		b = LicenceBitmask_All;
	}
	else
	{
		b.setFromUINT64(n);
	}
	return b;
}

/* static */ PLicenseMask PLicenseMask::fromDbStringS(const char* hexStr)
{
	PLicenseMask mask;
	mask.fromDbString ( hexStr );
	return mask;
}

const char* PLicenseMask::toDbString(PString& hexStr) const
{
	if (*this == LicenceBitmask_All)
	{
		hexStr.append(eternityAllStr());	//	Append and not assign, see PBitmask::toJSONString
		return hexStr.c_str();
	}
	return PBitmask::toDbString(hexStr);
}

void PLicenseMask::fromDbString(const char* hexStr)
{
	if (!strcmp(hexStr, eternityAllStr()))
	{
		*this = LicenceBitmask_All;
		return;
	}
	PBitmask::fromDbString(hexStr);
}

void PLicenseMask::compose(CommMsgBody& body) const
{
	if (*this == LicenceBitmask_All)
	{
		static const PBitmask eternityAll(eternityAllStr());
		return eternityAll.compose(body);
	}
	PBitmask::compose(body);
}

void PLicenseMask::parse(CommMsgParser& parser)
{
	PBitmask::parse(parser);
	static const PBitmask eternityAll(eternityAllStr());
	if (eternityAll == *this)
	{
		*this = LicenceBitmask_All;
	}
}

PLicenseMask& PLicenseMask::operator|=(const PLicenseMask& other)
{
	PBitmask::operator|=(other);
	return *this;
}

PLicenseMask& PLicenseMask::operator^=(const PLicenseMask& other)
{
	PBitmask::operator^=(other);
	return *this;
}

PLicenseMask& PLicenseMask::operator&=(const PLicenseMask& other)
{
	PBitmask::operator&=(other);
	return *this;
}

PLicenseMask& PLicenseMask::operator<<=(INT32 n)
{
	PBitmask::operator<<=(n);
	return *this;
}

PLicenseMask& PLicenseMask::operator>>=(INT32 n)
{
	PBitmask::operator>>=(n);
	return *this;
}

PLicenseMask& PLicenseMask::andNot(const PLicenseMask& other)
{
	PBitmask::andNot(other);
	return *this;
}

bool PLicenseMask::operator==(const PLicenseMask& other) const
{
	return PBitmask::operator==(other);
}

bool PLicenseMask::operator!=(const PLicenseMask& other) const
{
	return PBitmask::operator!=(other);
}

bool PLicenseMask::intersects(const PLicenseMask& other) const
{
	return PBitmask::intersects(other);
}
