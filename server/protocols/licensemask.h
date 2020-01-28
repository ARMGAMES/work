// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Parts of this file are being maintained using automated scripts.
// The script maintenance parts are surrounded by language-specific
// comment lines that begin with <<== and end with ==>> markers.
// When required, manual changes can made to this file as usual. 
// Such changes should be communicated to the current maintainer 
// of GIT module in ADMIN/asppages/admin/_licenses. 
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#ifndef licensemasks_h_included
#define licensemasks_h_included

#include "PBitmask.h"
#include "licenseid.h"

#define LIC_UNKNOWN_CODE "UKN"

class PLicenseMask : public PBitmask
{
	class PLicenseMaskIterator : public std::iterator<forward_iterator_tag, LicenceId>
	{
		INT32 currentBit; // -1 indicates the end
		const PLicenseMask* licenseMask;

	public:
		PLicenseMaskIterator(const PLicenseMask& mask, INT32 bit);
		PLicenseMaskIterator(const PLicenseMaskIterator& other);
		PLicenseMaskIterator& operator=(const PLicenseMaskIterator& other);
		LicenceId operator*() const;
		PLicenseMaskIterator& operator++();
		bool operator==(const PLicenseMaskIterator& other) const
		{
			if (currentBit == EndBit && other.currentBit == EndBit)
			{
				return true;
			}

			return (licenseMask == other.licenseMask) && (currentBit == other.currentBit);
		}

		bool operator!=(const PLicenseMaskIterator& other) const
		{
			return !operator==(other);
		}

		static const INT32 EndBit = -1;
	};

public:
	static const PLicenseMask zero;

	PLicenseMask(int) = delete;
	
	explicit PLicenseMask(const char* str)
		: PBitmask(str)
	{}

	PLicenseMask()
		: PBitmask()
	{}

	PLicenseMask(const PLicenseMask& other)
		: PBitmask(other)
	{}

	// PYR-114529 - Temporary constructor, to be removed when all usage of PBitmask for licenses is changed in order to use PLicenseMask instead.
	PLicenseMask(const PBitmask& other)
		: PBitmask(other)
	{}

	PLicenseMask& operator=(const PLicenseMask& other)
	{
		PBitmask::operator=(other);
		return *this;
	}

	PLicenseMask(PLicenseMask&& other)
		: PBitmask(std::move(other))
	{}

	PLicenseMask& operator=(PLicenseMask&& other)
	{
		PBitmask::operator=(std::move(other));
		return *this;
	}

	bool getLicenseId(LicenceId& licenseId) const;

	const char* toLicenseListStr(PString& licStr, const char* separator = ",") const;
	
	// PYR-114529 Moved from LicenseRules.
	bool fromLicenseListStr(const char* licStr, const char* separator = ",");

	typedef PLicenseMaskIterator const_iterator;
	const_iterator begin() const;
	const_iterator end() const;

	static PLicenseMask fromUINT64(UINT64 n);
	static PLicenseMask fromDbStringS(const char* hexStr);
	static const char* eternityAllStr() {return "7FFFFFFF";}

	virtual const char* toDbString(PString& hexStr) const override;
	virtual void fromDbString(const char* hexStr) override;
	using PBitmask::fromDbString; // for the `void fromDbString(const char* dbString, UINT64 integerMaskFallback)` overload

	virtual void compose(CommMsgBody& body) const override;
	virtual void parse(CommMsgParser& parser) override;

	PLicenseMask& operator|=(const PLicenseMask& other);
	PLicenseMask& operator^=(const PLicenseMask& other);
	PLicenseMask& operator&=(const PLicenseMask& other);
	PLicenseMask& operator<<=(INT32 n);
	PLicenseMask& operator>>=(INT32 n);
	bool operator==(const PLicenseMask& other) const;
	bool operator!=(const PLicenseMask& other) const;
	bool intersects(const PLicenseMask& other) const;
	PLicenseMask& andNot(const PLicenseMask& other);

	// PYR-114529 Moved from LicenseRules.
	static const char* getLicenseIdCode(LicenceId licenseId)
	{
		switch (licenseId)
		{
//	<<== @license2-id-code: License to code @ ====
		case eLicenceDefault:
			return "COM";
		case eLicenceItaly:
			return "IT";
		case eLicenceFrance:
			return "FR";
		case eLicenceEstonia:
			return "EE";
		case eLicenceBelgium:
			return "BE";
		case eLicenceSpain:
			return "ES";
		case eLicenceDenmark:
			return "DK";
		case eLicenceEU:
			return "EU";
		case eLicenceFB:
			return "FB";
		case eLicenceUSNJ:
			return "USNJ";
		case eLicenceUK:
			return "UK";
		case eLicenceBG:
			return "BG";
		case eLicenceDESH:
			return "DESH";
		case eLicenceRomania:
			return "RO";
		case eLicencePortugal:
			return "PT";
		case eLicenceCZ:
			return "CZ";
		case eLicenceIndia:
			return "IN";
		case eLicenceAsia:
			return "ASIA";
		case eLicenceRU:
			return "RU";
		case eLicenceUSPA:
			return "USPA";
		case eLicenceSE:
			return "SE";
		case eLicenceCH:
			return "CH";
		case eLicenceARBA:
			return "ARBA";
		case eLicenceUSCO:
			return "USCO";
		case eLicenceDE:
			return "DE";
		case eLicenceGR:
			return "GR";
//	==== @license2-id-code: License to code @ ==>>
		default:
			;
		}
		return LIC_UNKNOWN_CODE;
	}

	// PYR-114529 Moved from LicenseRules.
//	<<== @license2-name-to-id: License id by name @ ====
	static bool licenseNameToId(const char* licenseName, LicenceId& outVal)
	{
		if (PString::compareIgnoreCase(licenseName, "COM") == 0 || PString::compareIgnoreCase(licenseName, "DEF") == 0)
		{
			outVal = eLicenceDefault;
		}
		else if (PString::compareIgnoreCase(licenseName, "IT") == 0)
		{
			outVal = eLicenceItaly;
		}
		else if (PString::compareIgnoreCase(licenseName, "FR") == 0)
		{
			outVal = eLicenceFrance;
		}
		else if (PString::compareIgnoreCase(licenseName, "EE") == 0)
		{
			outVal = eLicenceEstonia;
		}
		else if (PString::compareIgnoreCase(licenseName, "BE") == 0)
		{
			outVal = eLicenceBelgium;
		}
		else if (PString::compareIgnoreCase(licenseName, "ES") == 0)
		{
			outVal = eLicenceSpain;
		}
		else if (PString::compareIgnoreCase(licenseName, "DK") == 0)
		{
			outVal = eLicenceDenmark;
		}
		else if (PString::compareIgnoreCase(licenseName, "EU") == 0)
		{
			outVal = eLicenceEU;
		}
		else if (PString::compareIgnoreCase(licenseName, "FB") == 0)
		{
			outVal = eLicenceFB;
		}
		else if (PString::compareIgnoreCase(licenseName, "USNJ") == 0)
		{
			outVal = eLicenceUSNJ;
		}
		else if (PString::compareIgnoreCase(licenseName, "UK") == 0)
		{
			outVal = eLicenceUK;
		}
		else if (PString::compareIgnoreCase(licenseName, "BG") == 0)
		{
			outVal = eLicenceBG;
		}
		else if (PString::compareIgnoreCase(licenseName, "DESH") == 0)
		{
			outVal = eLicenceDESH;
		}
		else if (PString::compareIgnoreCase(licenseName, "RO") == 0)
		{
			outVal = eLicenceRomania;
		}
		else if (PString::compareIgnoreCase(licenseName, "PT") == 0)
		{
			outVal = eLicencePortugal;
		}
		else if (PString::compareIgnoreCase(licenseName, "CZ") == 0)
		{
			outVal = eLicenceCZ;
		}
		else if (PString::compareIgnoreCase(licenseName, "IN") == 0)
		{
			outVal = eLicenceIndia;
		}
		else if (PString::compareIgnoreCase(licenseName, "ASIA") == 0)	// PYR-51749
		{
			outVal = eLicenceAsia;
		}
		else if (PString::compareIgnoreCase(licenseName, "RU") == 0)
		{
			outVal = eLicenceRU;
		}
		else if (PString::compareIgnoreCase(licenseName, "USPA") == 0)
		{
			outVal = eLicenceUSPA;
		}
		else if (PString::compareIgnoreCase(licenseName, "SE") == 0)
		{
			outVal = eLicenceSE;
		}
		else if (PString::compareIgnoreCase(licenseName, "CH") == 0)
		{
			outVal = eLicenceCH;
		}
		else if (PString::compareIgnoreCase(licenseName, "ARBA") == 0)	// PYR-114693 PYR-114733
		{
			outVal = eLicenceARBA;
		}
		else if (PString::compareIgnoreCase(licenseName, "USCO") == 0)	// PYR-124292
		{
			outVal = eLicenceUSCO;
		}
		else if (PString::compareIgnoreCase(licenseName, "DE") == 0)	// PYR-124555
		{
			outVal = eLicenceDE;
		}
		else if (PString::compareIgnoreCase(licenseName, "GR") == 0)	// PYR-120936
		{
			outVal = eLicenceGR;
		}
		else if (PString::compareIgnoreCase(licenseName, "NIL") == 0)  //PYR-30560 - this is NOT a valid licence, 
		{	                                                           //            internal Majordomo use only
			outVal = LicenceId((1 << 31));
		}
		else
		{
			return false; // not found
		}
		return true; // success
	}
//	==== @license2-name-to-id: License id by name @ ==>>

	// PYR-114529 Moved from LicenseRules.
	static bool maskFromLicenseId(LicenceId licenseId, PLicenseMask& bitmask);
};

inline PLicenseMask operator|(const PLicenseMask& left, const PLicenseMask& right)
{
	PLicenseMask answer = left;
	return answer.operator|=(right);
}

inline PLicenseMask operator&(const PLicenseMask& left, const PLicenseMask& right)
{
	PLicenseMask answer = left;
	return answer.operator&=(right);
}

// PYR-114529 - Temporary operator, to be removed when all usage of PBitmask for licenses is changed in order to use PLicenseMask instead.
inline PLicenseMask operator&(const PBitmask& left, const PLicenseMask& right)
{
	PLicenseMask answer = left;
	return answer.operator&=(right);
}

inline PLicenseMask operator^(const PLicenseMask& left, const PLicenseMask& right)
{
	PLicenseMask answer = left;
	return answer.operator^=(right);
}

inline PLicenseMask operator>>(const PLicenseMask& left, INT32 shift)
{
	PLicenseMask answer = left;
	return answer.operator>>=(shift);
}

inline PLicenseMask operator<<(const PLicenseMask& left, INT32 shift)
{
	PLicenseMask answer = left;
	return answer.operator<<=(shift);
}

inline PLicenseMask andNot(const PLicenseMask& left, const PLicenseMask& right)
{
	PLicenseMask answer = left;
	return answer.andNot(right);
}

//	<<== @license2-list-full: All license bitmasks @ ====
extern const PLicenseMask LicenceBitmask_Default;
extern const PLicenseMask LicenceBitmask_Italy;
extern const PLicenseMask LicenceBitmask_France;
extern const PLicenseMask LicenceBitmask_Estonia;
extern const PLicenseMask LicenceBitmask_Belgium;
extern const PLicenseMask LicenceBitmask_Spain;
extern const PLicenseMask LicenceBitmask_Denmark;
extern const PLicenseMask LicenceBitmask_EU;
extern const PLicenseMask LicenceBitmask_FB;
extern const PLicenseMask LicenceBitmask_USNJ;
extern const PLicenseMask LicenceBitmask_UK;
extern const PLicenseMask LicenceBitmask_BG;
extern const PLicenseMask LicenceBitmask_DESH;
extern const PLicenseMask LicenceBitmask_Portugal_Old;
extern const PLicenseMask LicenceBitmask_Romania;
extern const PLicenseMask LicenceBitmask_Portugal;
extern const PLicenseMask LicenceBitmask_CZ;
extern const PLicenseMask LicenceBitmask_India;
extern const PLicenseMask LicenceBitmask_Asia;
extern const PLicenseMask LicenceBitmask_RU;
extern const PLicenseMask LicenceBitmask_USPA;
extern const PLicenseMask LicenceBitmask_SE;
extern const PLicenseMask LicenceBitmask_CH;
extern const PLicenseMask LicenceBitmask_ARBA;
extern const PLicenseMask LicenceBitmask_USCO;
extern const PLicenseMask LicenceBitmask_DE;
extern const PLicenseMask LicenceBitmask_GR;
//	==== @license2-list-full: All license bitmasks @ ==>>

extern const PLicenseMask LicenceBitmask_Last;

extern const PLicenseMask LicenceBitmask_UNDEFINED;

extern const PLicenseMask LicenceBitmask_SharedLiquidityPool;

extern const PLicenseMask LicenceBitmask_All; // PYR-99717

#endif // licensemasks_h_included
