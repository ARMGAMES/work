// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Parts of this file are being maintained using automated scripts.
// The script maintenance parts are surrounded by language-specific
// comment lines that begin with <<== and end with ==>> markers.
// When required, manual changes can made to this file as usual. 
// Such changes should be communicated to the current maintainer 
// of GIT module in ADMIN/asppages/admin/_licenses. 
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#include "licenserules.h"
#include "licensemask.h"
#include "sitemasks.h"
#include "vectorutils.h"

//	PYR-100863
/* static */ 
//	<<== @license2-central-host: Licenses hosted at IOM DC @ ====
const PLicenseMask& LicenseRules::getCentralHostLicensesMask()
{
	static const PLicenseMask centralHostLicensesMask =
			LicenceBitmask_Default |
			LicenceBitmask_Italy |
			LicenceBitmask_France |
			LicenceBitmask_Estonia |
			LicenceBitmask_Belgium |
			LicenceBitmask_Spain |
			LicenceBitmask_Denmark |
			LicenceBitmask_EU |
			LicenceBitmask_FB |
			LicenceBitmask_UK |
			LicenceBitmask_BG |
			LicenceBitmask_DESH |
			LicenceBitmask_Romania |
			LicenceBitmask_Portugal |
			LicenceBitmask_CZ |
			LicenceBitmask_India |
			LicenceBitmask_Asia |
			LicenceBitmask_RU |
			LicenceBitmask_SE |
			LicenceBitmask_CH |
			LicenceBitmask_ARBA |
			LicenceBitmask_DE |
			LicenceBitmask_GR;
	return centralHostLicensesMask;
}
//	==== @license2-central-host: Licenses hosted at IOM DC @ ==>>

/* static */  
//	<<== @license2-peripheral-hosts: Licenses hosted at Peripheral DC @ ====
const PLicenseMask& LicenseRules::getPeripheralHostLicensesMask()
{
	static const PLicenseMask peripheralHostLicensesMask =
		LicenceBitmask_USNJ | LicenceBitmask_USPA |
		LicenceBitmask_USCO;

	return peripheralHostLicensesMask;
}
//	==== @license2-peripheral-hosts: Licenses hosted at Peripheral DC @ ==>>

bool LicenseRules::maskContainsLicenseId(UINT32 licenseId, const PLicenseMask& bitmask)
{
	PLicenseMask licenseBitmask;
	if (!PLicenseMask::maskFromLicenseId((LicenceId)licenseId, licenseBitmask))
	{
		PTRACE(LOG_TRACE_FAULTY_LOGIC ": maskContainsLicenseId: invalid licenseId %u", licenseId);
		return false;
	}
	return (licenseBitmask & bitmask) != PBitmask::zero;
}

// PYR-109751 - deprecated, use the PLicenseMask version instead.
/*static*/ bool LicenseRules::addToMask(const UINT32 licenseId, PBitmask& licenseMask)
{
	PLicenseMask licMask = licenseMask;
	bool result = addToMask(licenseId, licMask);
	licenseMask = std::move(licMask);
	return result;
}

bool LicenseRules::addToMask( const UINT32 licenseId, PLicenseMask& licenseMask )
{
	PLicenseMask maskToAdd;
	if(PLicenseMask::maskFromLicenseId( (LicenceId)licenseId, maskToAdd ) )
	{
		licenseMask |= maskToAdd;
		return true;
	}
	return false;
}

// PYR-109751 - deprecated, use PLicenseMask::maskFromLicenseId version instead.
bool LicenseRules::maskFromLicenseId(UINT32 licenseId, PBitmask& bitmask)
{
	PLicenseMask licMask;
	bool result = PLicenseMask::maskFromLicenseId((LicenceId)licenseId, licMask);
	bitmask = std::move(licMask);
	return result;
}

//	<<== @site2-sitemask-by-license: Get site mask by licenseid @ ====
	const PSiteMask& SiteRules::getSiteMaskByLicenseId( UINT32 licenseId )
{
	switch( licenseId )
	{
	case eLicenceItaly:
		return SiteBitmask_Ita;
	case eLicenceFrance:
		return SiteBitmask_Fra;
	case eLicenceEstonia:
		return SiteBitmask_Est;
	case eLicenceBelgium:
		return SiteBitmask_Bel;
	case eLicenceSpain:
		return SiteBitmask_Spa;
	case eLicenceDenmark:
		return SiteBitmask_Den;
	case eLicenceEU:
		return SiteBitmask_EU;
	case eLicenceFB:
		return SiteBitmask_FB;
	case eLicenceUSNJ:
		return SiteBitmask_US_NJ;
	case eLicenceUK:
		return SiteBitmask_UK;
	case eLicenceBG:
		return SiteBitmask_Bul;
	case eLicenceDESH:
		return SiteBitmask_DE_SH;
	case eLicenceRomania:
		return SiteBitmask_Rom;
	case eLicencePortugal:
		return SiteBitmask_Por;
	case eLicenceCZ:
		return SiteBitmask_Cze;
	case eLicenceIndia:
		return SiteBitmask_Ind;
	case eLicenceAsia:
		return SiteBitmask_Asia;
	case eLicenceRU:
		return SiteBitmask_RU;
	case eLicenceUSPA:
		return SiteBitmask_US_PA;
	case eLicenceSE:
		return SiteBitmask_Swe;
	case eLicenceCH:
		return SiteBitmask_CH;
	case eLicenceARBA:
		return SiteBitmask_ARBA;
	case eLicenceUSCO:
		return SiteBitmask_US_CO;
	case eLicenceDE:
		return SiteBitmask_DE;
	case eLicenceGR:
		return SiteBitmask_GR;
	default:
		return SiteBitmask_Com;
	}
}
//	==== @site2-sitemask-by-license: Get site mask by licenseid @ ==>>

//	<<== @license2-bonus-mask: Licenses eligible for bonus @ ====
const PLicenseMask& LicenseRules::getBonusLicensesMaskIfZero()
	{ // All new licences are to be added
	static const PLicenseMask mask = LicenceBitmask_Default
			| LicenceBitmask_EU | LicenceBitmask_UK | LicenceBitmask_BG
			| LicenceBitmask_DESH | LicenceBitmask_Romania | LicenceBitmask_Portugal
			| LicenceBitmask_CZ | LicenceBitmask_India | LicenceBitmask_Asia
			| LicenceBitmask_RU | LicenceBitmask_SE | LicenceBitmask_CH
			| LicenceBitmask_DE | LicenceBitmask_GR
		;

	return mask;
}
//	==== @license2-bonus-mask: Licenses eligible for bonus @ ==>>

//	<<== @license2-single-currency-masks2: Masks for licenses with single currency wallet @ ====
const char* LicenseRules::getCommonCurrencyOrNull( const PLicenseMask& licenseMask )
{
	static const PLicenseMask pmMask = // not a RM license
		LicenceBitmask_FB;
	if( (licenseMask & pmMask) != PBitmask::zero )
		return nullptr;

	static const PLicenseMask usdMask = LicenceBitmask_Default
			| LicenceBitmask_Estonia | LicenceBitmask_Belgium | LicenceBitmask_Denmark | LicenceBitmask_EU
			| LicenceBitmask_USNJ | LicenceBitmask_UK | LicenceBitmask_BG | LicenceBitmask_DESH
			| LicenceBitmask_Romania | LicenceBitmask_CZ | LicenceBitmask_Asia | LicenceBitmask_USPA
			| LicenceBitmask_SE | LicenceBitmask_CH | LicenceBitmask_ARBA | LicenceBitmask_USCO
			| LicenceBitmask_DE | LicenceBitmask_GR;
	static const PLicenseMask eurMask = LicenceBitmask_Italy | LicenceBitmask_France | LicenceBitmask_Spain | LicenceBitmask_Portugal;
	static const PLicenseMask inrMask = LicenceBitmask_India;
	static const PLicenseMask rubMask = LicenceBitmask_RU;

	const char* commonCurrency = nullptr;
	if( licenseMask == PBitmask::zero )
	{
		PLog(LOG_TRACE_FAULTY_LOGIC "LicenseRules::getCommonCurrencyOrNull: zero licenseMask");
		commonCurrency = "USD";
	}
	if( (licenseMask & usdMask) != PBitmask::zero )
	{
		commonCurrency = "USD";
	}
	if( (licenseMask & eurMask) != PBitmask::zero )
	{
		if( commonCurrency )
			return nullptr;
		commonCurrency = "EUR";
	}
	if( (licenseMask & inrMask) != PBitmask::zero )
	{
		if( commonCurrency )
			return nullptr;
		commonCurrency = "INR";
	}
	if( (licenseMask & rubMask) != PBitmask::zero )
	{
		if( commonCurrency )
			return nullptr;
		commonCurrency = "RUB";
	}
	return commonCurrency;
}
//	==== @license2-single-currency-masks2: Masks for licenses with single currency wallet @ ==>>

//	<<== @license2-shared-liquidity-masks: Shared liquidity license masks @ ====
PLicenseMask LicenseRules::getRealMoneySharedLiquidityLicensesMask(UINT32 licenseId) // PYR-46696
{
	static const PLicenseMask defMask = LicenceBitmask_Default | LicenceBitmask_Estonia | LicenceBitmask_Belgium
			| LicenceBitmask_Denmark | LicenceBitmask_EU | LicenceBitmask_UK
			| LicenceBitmask_BG | LicenceBitmask_DESH | LicenceBitmask_Romania
			| LicenceBitmask_CZ | LicenceBitmask_Asia | LicenceBitmask_SE
			| LicenceBitmask_CH | LicenceBitmask_ARBA | LicenceBitmask_GR;

	if (maskContainsLicenseId(licenseId, LicenceBitmask_SharedLiquidityPool))
	{
		return LicenceBitmask_SharedLiquidityPool;
	}

	switch (licenseId)
	{
		case eLicenceItaly		: return LicenceBitmask_Italy		;
		case eLicenceFB			: return LicenceBitmask_FB			; // NOT a RM license
		case eLicenceUSNJ		: return LicenceBitmask_USNJ		;
		case eLicenceIndia		: return LicenceBitmask_India		;
		case eLicenceRU			: return LicenceBitmask_RU			;
		case eLicenceUSPA		: return LicenceBitmask_USPA		;
		case eLicenceUSCO		: return LicenceBitmask_USCO		;
		case eLicenceDE			: return LicenceBitmask_DE			;

	default:
		return defMask;
	}
}
//	==== @license2-shared-liquidity-masks: Shared liquidity license masks @ ==>>

//	<<== @site2-peripheral-host-masks: Peripheral host site masks @ ====

const PSiteMask& SiteRules::getPeripheralHostSitesMask()
{
	static const PSiteMask peripheralHostSitesMask =
			SiteBitmask_US_NJ |
			SiteBitmask_US_PA |
			SiteBitmask_US_CO
	;
	return peripheralHostSitesMask;
}
//	==== @site2-peripheral-host-masks: Peripheral host site masks @ ==>>

//	<<== @license2-central-host-site-mask2: Mask of all sites on central host @ ====
const PSiteMask& SiteRules::getCentralHostSitesMask()
{
	static const PSiteMask centralHostSitesMask =
			SiteBitmask_Com |
			SiteBitmask_Net |
			SiteBitmask_RUSO |
			SiteBitmask_RUSOPM |
			SiteBitmask_Ita |
			SiteBitmask_Fra |
			SiteBitmask_Est |
			SiteBitmask_Bel |
			SiteBitmask_Spa |
			SiteBitmask_Den |
			SiteBitmask_EU |
			SiteBitmask_FB |
			SiteBitmask_UK |
			SiteBitmask_Bul |
			SiteBitmask_DE_SH |
			SiteBitmask_Por_Old |
			SiteBitmask_Rom |
			SiteBitmask_Por |
			SiteBitmask_Cze |	//	PYR-47735
			SiteBitmask_Ind |	//	PYR-52215
			SiteBitmask_Asia |
			SiteBitmask_RU |
			SiteBitmask_Swe |
			SiteBitmask_CH |
			SiteBitmask_ARBA |	//	PYR-114693 PYR-114733
			SiteBitmask_DE |	//	PYR-124555
			SiteBitmask_GR;	//	PYR-120936
	// note the missing: SiteBitmask_Lite, SiteBitmask_ESPN, SiteBitmask_FOX, HoyleSierra_Com
	return centralHostSitesMask;
}
//	==== @license2-central-host-site-mask2: Mask of all sites on central host @ ==>>

PLicenseMask LicenseRulesConfig::getLicensesMaskByHostId(const UINT32 hostId) const
{
	if (hostId > eHost_Maximum)
	{
		PLog(LOG_TRACE_FAULTY_LOGIC "LicenseRulesConfig::doHostAndLicenseMatch: hostId(%u) > eHost_Maximum", hostId);
		// no return
	}
	if (hostId == eHost_Central)
	{
		return LicenseRules::getCentralHostLicensesMask();
	}
	if (_peripheralLicenseToHostMap.empty())
	{
		// using default values if no configuration is loaded
		// to be removed after Poker task is done: PYR-99612
		PLog("LicenseRulesConfig::getLicensesMaskByHostId: no configuration loaded, using default values (to be removed in 2019.Build.05)");
		if (hostId == 2)
		{
			return LicenceBitmask_USNJ;
		}
		if (hostId == 3)
		{
			return LicenceBitmask_USPA;
		}
		// TODO: #vadimp remove the above in 2019.Build.05
		//PLog(LOG_TRACE_FAULTY_LOGIC "LicenseRulesConfig not initialized!");
		// no return
	}

	PLicenseMask licenseMask;
	for (const LicenseHostMap::value_type& e : _peripheralLicenseToHostMap)
	{
		if (e.second == hostId)
		{
			PLicenseMask tmpMask;
			if (LicenseRules::maskFromLicenseId(e.first, tmpMask))
				licenseMask |= tmpMask;
			else
				PLog( LOG_TRACE_FAULTY_LOGIC "LicenseRulesConfig::getLicensesMaskByHostId: invalid licenseId: %u", e.first);
		}
	}
	return licenseMask;
}

// PYR-100668 - moved from licenserules.h
bool LicenseRulesConfig::isCrossHostSiteMask(PSiteMask siteMask ) const // PYR-28055, formerly known as 'isConflictingHostsSiteMask'
{
	UINT32 hostId = eHost_Unknown;
	PSiteMask siteIdMask = SiteBitmask_Com; // PYR-100668
	while( siteMask != PBitmask::zero )
	{
		if( siteMask & SiteBitmask_Com ) // PYR-100668
		{
			UINT32 siteId;
			SiteRules::siteNumberFromMask( siteIdMask, siteId );

			if( hostId == eHost_Unknown )
			{
				hostId = getHostBySite( siteId );
			}
			else
			{
				if( hostId != getHostBySite( siteId ) )
					return true;
			}
		}
		siteIdMask <<= 1;
		siteMask >>= 1;
	}
	return false;
}

// based on the original getSitesMaskByHostId(), this function does not consider SiteBitmask_FOX | SiteBitmask_ESPN a part of central host
PSiteMask LicenseRulesConfig::getSitesMaskByHostId(const UINT32 hostId) const
{
	if (hostId > eHost_Maximum)
	{
		PLog(LOG_TRACE_FAULTY_LOGIC "LicenseRulesConfig::getSitesMaskByHostId: hostId(%u) > eHost_Maximum", hostId);
		// no return
	}
	if (hostId == eHost_Central)
	{
		return SiteRules::getCentralHostSitesMask();
	}
	if (_peripheralLicenseToHostMap.empty())
	{
		// using default values if no configuration is loaded
		// to be removed after Poker task is done: PYR-99612
		PLog("LicenseRulesConfig::getSitesMaskByHostId: no configuration loaded, using default values (to be removed in 2019.Build.05)");
		if (hostId == 2)
		{
			return SiteBitmask_US_NJ;
		}
		if (hostId == 3)
		{
			return SiteBitmask_US_PA;
		}
		// TODO: #vadimp remove the above in 2019.Build.05
		//PLog(LOG_TRACE_FAULTY_LOGIC "LicenseRulesConfig not initialized!");
		// no return
	}
	PSiteMask siteMask = PSiteMask::zero;
	for (const LicenseHostMap::value_type& e : _peripheralLicenseToHostMap)
	{
		if (e.second == hostId)
		{
			PSiteMask siteMaskfromSiteNumber;
			SiteRules::maskFromSiteNumber(SiteRules::getSiteIdByLicenseId(e.first), siteMaskfromSiteNumber);
			siteMask |= siteMaskfromSiteNumber;
		}
	}
	return siteMask;
}

// checks if ANY site in the mask matches the host
// this function considers PokerStars_FOX and PokerStars_ESPN a part of central host
bool LicenseRulesConfig::siteMaskMatchHost(const UINT32 hostId, const PSiteMask& siteMask) const // PYR-28055
{
	PSiteMask hostSites = getSitesMaskByHostId(hostId);
	if (hostId == eHost_Central)
	{
		hostSites = hostSites | SiteBitmask_FOX | SiteBitmask_ESPN;
	}
	return (hostSites & siteMask) != PSiteMask::zero;
}

bool LicenseRulesConfig::isCrossHostLicenseMask(const PLicenseMask& licenseMaskToCheck) const // PYR-28055, formerly known as 'isConflictingHostsLicenseMask'
{
	UINT32 hostId = eHost_Unknown;
	static const PLicenseMask licenseMaskEnd = LicenceBitmask_Last << 1;

	for (PLicenseMask singleLicenseMask = LicenceBitmask_Default; singleLicenseMask != licenseMaskEnd; singleLicenseMask <<= 1)
	{
		UINT32 licenseId = 0;
		if (!LicenseRules::licenseIdFromMask(singleLicenseMask, licenseId))
		{
			continue;
		}

		if ((licenseMaskToCheck & singleLicenseMask) == PBitmask::zero)
		{
			continue;
		}

		if (hostId == eHost_Unknown)
		{
			hostId = getHostByLicense(licenseId);
		}
		else
		{
			if (hostId != getHostByLicense(licenseId))
				return true;
		}
	}

	return false;
}

// PYR-109751 - deprecated, use PSiteMask::maskFromSiteNumber() instead.
bool SiteRules::maskFromSiteNumber(UINT32 siteNum, PBitmask& bitmask)
{
	PSiteMask siteMask;
	bool result = maskFromSiteNumber(siteNum, siteMask);
	bitmask = std::move(siteMask);
	return result;
}

bool SiteRules::doSiteAndLicenseMatch( UINT32 licenseId, const PSiteMask& siteMask )
{
	if( licenseId == eLicenceDefault )
	{
		return ( siteMask & SiteBitmask_Segregated ) == PBitmask::zero;
	}

	PSiteMask siteBitmaskForLicenseId;
	if( maskFromSiteNumber( getSiteIdByLicenseId( licenseId ), siteBitmaskForLicenseId ) )
	{
		return ( siteMask & siteBitmaskForLicenseId ) != PBitmask::zero;
	}
		
	return false;
}

bool SiteRules::showCityBySiteMask( const PSiteMask& siteMask )
{
//	<<== @site2-site-mask-for-city: (E) Get mask for sites that require city display @ ====
	return siteMask == SiteBitmask_Ita
		|| siteMask == SiteBitmask_Fra
		|| siteMask == SiteBitmask_Est
		|| siteMask == SiteBitmask_Bel
		|| siteMask == SiteBitmask_Spa
		|| siteMask == SiteBitmask_Den	// #21043
		|| siteMask == SiteBitmask_US_NJ	// PYR-27853
		|| siteMask == SiteBitmask_UK
		|| siteMask == SiteBitmask_Bul
		|| siteMask == SiteBitmask_Rom
		|| siteMask == SiteBitmask_Por
		|| siteMask == SiteBitmask_Ind	// PYR-30559
		|| siteMask == SiteBitmask_RU
		|| siteMask == SiteBitmask_US_PA
		|| siteMask == SiteBitmask_US_CO
//	==== @site2-site-mask-for-city: (E) Get mask for sites that require city display @ ==>>
		;
}

const PSiteMask& SiteRules::getSiteMaskCantBeAddedWithDeltaTool()
{
	static const PSiteMask cantBeAdded( SiteBitmask_Ita | SiteBitmask_Fra );
	return cantBeAdded;
}

bool SiteRules::belongsToSharedLiquidityPool( const PSiteMask& siteMask )
{
	return ((SiteBitmask_SharedLiquidityPool & siteMask) == siteMask) && (PSiteMask::zero != siteMask);
}

bool SiteRules::isSharedLiquidityPoolMask( const PSiteMask& siteMask )
{
	return (SiteBitmask_SharedLiquidityPool & siteMask) != PBitmask::zero;
}

bool SiteRules::isConflictingSharedLiquidityPoolMask( const PSiteMask& siteMask )
{
	// the mask has sites from within the shared liquidity pool
	// and from outside of the pool
	return	isSharedLiquidityPoolMask( siteMask ) &&
		(!belongsToSharedLiquidityPool( siteMask ));
}

bool SiteRules::isSegregatedSiteMask( const PSiteMask& siteMask )
{
	return (siteMask & SiteBitmask_Segregated) != PBitmask::zero;
}

bool SiteRules::isConflictingSegregatedSiteMask( const PSiteMask& siteMask )
{
	return (siteMask & SiteBitmask_Segregated) != PBitmask::zero
		&& !siteMask.isOnlyOneBitSet();  // checks whether more than 1 bits are set
}

bool SiteRules::isConflictingSiteMask( const PSiteMask& siteMask )
{
	if( isConflictingSharedLiquidityPoolMask( siteMask ) )
	{
		return true;
	}

	return isConflictingSegregatedSiteMask( siteMask );
}