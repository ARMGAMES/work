// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Parts of this file are being maintained using automated scripts.
// The script maintenance parts are surrounded by language-specific
// comment lines that begin with <<== and end with ==>> markers.
// When required, manual changes can made to this file as usual. 
// Such changes should be communicated to the current maintainer 
// of GIT module in ADMIN/asppages/admin/_licenses. 
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#ifndef licenserules_h_included
#define licenserules_h_included

#include "dbm.h"
#include "licenseid.h"
#include "sites.h"
#include "clientplatforms.h"
#include "brandutils.h"
#include "shared.h"
#include "licensemask.h"
#include "sitemasks.h"


class LicenseRules
{
public:
	//	PYR-100863
	static const PLicenseMask& getCentralHostLicensesMask();
	static const PLicenseMask& getPeripheralHostLicensesMask();

	[[deprecated("use PLicenseMask::getlicenseId() instead")]]
	static bool licenseIdFromMask(const PLicenseMask& bitmask, UINT32& licenseId)
	{
		return bitmask.getLicenseId((LicenceId&)licenseId);
	}

	// PYR-109751 - deprecated, use PLicenseMask::maskFromLicenseId instead.
	[[deprecated("use PLicenseMask::maskFromLicenseId() instead")]]
	static bool maskFromLicenseId(UINT32 licenseId, PBitmask& bitmask);

	// PYR-109751 - deprecated, use PLicenseMask::maskFromLicenseId instead.
	[[deprecated("use PLicenseMask::maskFromLicenseId() instead")]]
	static bool maskFromLicenseId(UINT32 licenseId, PLicenseMask& bitmask)
	{
		return PLicenseMask::maskFromLicenseId((LicenceId)licenseId, bitmask);
	}

	static bool maskContainsLicenseId(UINT32 licenseId, const PLicenseMask& bitmask);
	// PYR-109751 - deprecated, use the PLicenseMask version instead.
	static bool addToMask( const UINT32 licenseId, PBitmask& licenseMask );
	static bool addToMask( const UINT32 licenseId, PLicenseMask& licenseMask );
	
	// PYR-114529 - deprecated, use PLicenseMask::fromLicenseListStr instead.
	[[deprecated("use PLicenseMask::fromLicenseListStr instead")]]
	static PLicenseMask getLicenseMaskFromCommaSeparatedList(const char* licenseList, const char* separators = ",")
	{
		PLicenseMask mask;
		mask.fromLicenseListStr(licenseList, separators);
		return mask;
	}

	//
	// Common
	//
	// PYR-114529 Moved to PLicenseMask
	[[deprecated("use PLicenseMask::getLicenseIdCode instead")]]
	static const char* getLicenseIdCode(UINT32 licenseId)
	{
		return PLicenseMask::getLicenseIdCode((LicenceId)licenseId);
	}

	// PYR-19131
	// getLicenseIdByName was removed, use PLicenseMask::licenseNameToId instead

	// PYR-114529 Moved to PLicenseMask
	[[deprecated("use PLicenseMask::licenseNameToId instead")]]
	static bool licenseNameToId(const char* licenseName, LicenceId& outVal)
	{
		return PLicenseMask::licenseNameToId(licenseName, outVal);
	}

	static bool isValidLicense(UINT32 licenseId)
	{
		switch(licenseId)
		{
//	<<== @license2-case-enum2: Valid license enumeration @ ====
			case eLicenceDefault:
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
			case eLicenceRomania:
			case eLicencePortugal:
			case eLicenceCZ:
			case eLicenceIndia:
			case eLicenceAsia:	//	PYR-51749
			case eLicenceRU:
			case eLicenceUSPA:
			case eLicenceSE:
			case eLicenceCH:
			case eLicenceARBA:	//	PYR-114693 PYR-114733
			case eLicenceUSCO:	//	PYR-124292
			case eLicenceDE:	//	PYR-124555
			case eLicenceGR:	//	PYR-120936
//	==== @license2-case-enum2: Valid license enumeration @ ==>>
				return true;
		}
		return false;
	}

	static bool wordRebateRequired(UINT32 licenseId)
	{
//	<<== @license2-rebate: License rebate required @ ====
		return licenseId == eLicenceSpain
			|| licenseId == eLicenceDenmark
			|| licenseId == eLicenceBG
			;
//	==== @license2-rebate: License rebate required @ ==>>
	}

	static const PLicenseMask& getBonusLicensesMaskIfZero();

	static bool isEligibleForBonus( const PLicenseMask& bonusLicensesBitMask, UINT32 userLicenseId )
	{
		return maskContainsLicenseId(userLicenseId, bonusLicensesBitMask);
	}

	static bool needSelfLimitApproval( UINT32 licenseId )
	{
//	<<== @license2-self-limit: Licenses that require self limit approval @ ====
		return licenseId == eLicenceDefault	//	PYR-122809 
			|| licenseId == eLicenceItaly 
			|| licenseId == eLicenceFrance 
			|| licenseId == eLicenceEstonia	//	PYR-61458 
			|| licenseId == eLicenceBelgium	//	PYR-113183 
			|| licenseId == eLicenceSpain 
			|| licenseId == eLicenceDenmark 
			|| licenseId == eLicenceEU	//	PYR-122809 
			|| licenseId == eLicenceUSNJ 
			|| licenseId == eLicenceUK	//	PYR-32389 
			|| licenseId == eLicenceDESH	//	PYR-33649 
			|| licenseId == eLicenceRomania 
			|| licenseId == eLicencePortugal 
			|| licenseId == eLicenceUSPA	//	PYR-72414 
			|| licenseId == eLicenceSE	//	PYR-76896 
			|| licenseId == eLicenceARBA	//	PYR-122809 
			|| licenseId == eLicenceUSCO 
			|| licenseId == eLicenceDE 
			|| licenseId == eLicenceGR	//	PYR-122809 
//	==== @license2-self-limit: Licenses that require self limit approval @ ==>>
			;
	}

	static bool has3SelfLimits( UINT32 licenseId )
	{
//	<<== @license2-3self-limits: Licenses that have 3 self-limits @ ====
		return licenseId == eLicenceSpain 
			|| licenseId == eLicenceDenmark 
			|| licenseId == eLicenceEU	//	PYR-103866 
			|| licenseId == eLicenceUSNJ 
			|| licenseId == eLicenceUK	//	PYR-35363 
			|| licenseId == eLicenceDESH 
			|| licenseId == eLicenceRomania 
			|| licenseId == eLicencePortugal 
			|| licenseId == eLicenceUSPA	//	PYR-72414 
			|| licenseId == eLicenceSE	//	PYR-76896 
			|| licenseId == eLicenceARBA	//	PYR-114693 PYR-114733 
			|| licenseId == eLicenceUSCO	//	PYR-124292 
			|| licenseId == eLicenceDE	//	PYR-124555 
			|| licenseId == eLicenceGR	//	PYR-103866 
			|| licenseId == eLicenceDefault	//	PYR-103866 
//	==== @license2-3self-limits: Licenses that have 3 self-limits @ ==>>
			;
	}

	static const char* getTheOnlyCurrencyOrNull(UINT32 licenseId) // introduced in PYR-58378, returns NULL for multi-currency licenses
	{
		switch (licenseId)
		{
//	<<== @license2-single-currency: Single currency licenses @ ====

		case eLicenceItaly:
		case eLicenceFrance:
		case eLicenceSpain:
		case eLicencePortugal:
			return "EUR";


		case eLicenceUSNJ:
		case eLicenceAsia:
		case eLicenceUSPA:
		case eLicenceCH:
		case eLicenceUSCO:
			return "USD";


		case eLicenceIndia:
			return "INR";


		case eLicenceRU:
			return "RUB";

//	==== @license2-single-currency: Single currency licenses @ ==>>
		default:
			return nullptr; // the function returns NULL for multi-currency licenses
		}
	}

	static bool isSingleCurrency(UINT32 licenseId)
	{
		return (getTheOnlyCurrencyOrNull(licenseId) != nullptr);
	}

	static bool canHaveCurrency(UINT32 licenseId, const char* currency)
	{
		const char* singleCurrency = getTheOnlyCurrencyOrNull(licenseId);
		return (!singleCurrency || compareCurrencies(singleCurrency, currency));
	}

	static bool hasUsdCurrency(UINT32 licenseId)
	{
		return canHaveCurrency(licenseId, "USD");
	}

	// precondition: licenseMask != PBitmask::zero
	static const char* getCommonCurrencyOrNull(const PLicenseMask& licenseMask);

//	<<== @license2-has-only-eur: Licenses that allow only EUR currency @ ====
	static bool hasOnlyEurAccount( UINT32 licenseId )
	{
		return licenseId == eLicenceItaly || licenseId == eLicenceFrance || licenseId == eLicenceSpain || licenseId == eLicencePortugal;
	}
//	==== @license2-has-only-eur: Licenses that allow only EUR currency @ ==>>

	static PLicenseMask getRealMoneySharedLiquidityLicensesMask(UINT32 licenseId); // PYR-46696

	static bool mustHaveEurAccount( UINT32 licenseId )
	{
//	<<== @license2-mandatory-eur: Licenses that require EUR wallet (such as for upsell) @ ====
		return hasOnlyEurAccount(licenseId) ||
			licenseId == eLicenceBelgium ||
			licenseId == eLicenceDESH ||
			licenseId == eLicenceDE ||
			licenseId == eLicenceGR;
//	==== @license2-mandatory-eur: Licenses that require EUR wallet (such as for upsell) @ ==>>
	}

	static const char* getMustHaveCurrency( UINT32 licenseId )
	{
		const char* singleCurrency = getTheOnlyCurrencyOrNull(licenseId);
		return (singleCurrency != nullptr ? singleCurrency : "USD");
	}

	static const char* getSelfDepositLimitCurrency(UINT32 licenseId) // self-imposed deposit limit currency
	{
//	<<== @license2-limit-currency: Licenses with explicit wallet for self imposed limit @ ====
		if (licenseId == eLicenceSE)
			return "SEK";
//	==== @license2-limit-currency: Licenses with explicit wallet for self imposed limit @ ==>>
		return getMustHaveCurrency(licenseId);
	}

	static const char* getGameNacCurrency( UINT32 licenseId, const char* gameCurrency )
	{
//	<<== @license2-nac-currency: Licenses with explicit gameCurrency NAC wallet @ ====
		return (licenseId == eLicenceRomania) ? "USD" : gameCurrency;	// PYR-33529
//	==== @license2-nac-currency: Licenses with explicit gameCurrency NAC wallet @ ==>>
	}

	static const char* getUserNacCurrency( UINT32 licenseId, const char* userDefaultCurrency )
	{
//	<<== @license2-nac-currency: Licenses with explicit userDefaultCurrency NAC wallet @ ====
		return (licenseId == eLicenceRomania) ? "USD" : userDefaultCurrency;	// PYR-33529
//	==== @license2-nac-currency: Licenses with explicit userDefaultCurrency NAC wallet @ ==>>
	}

	static const char* getTournTicketTotalCurrency(UINT32 licenseId)
	{
		return getMustHaveCurrency(licenseId); // PYR-88255 USD for CH, singleCurrency for others
	}

//	<<== @license2-limit-confirm: (E) Licenses that require self-imposed limit confirmation @ ====
	static bool newGameLimitsNeedReconfirmOnLogin(UINT32 licenseId)
	{
  return licenseId == eLicenceUSNJ	// PYR-27779 PYR-27819
   || licenseId == eLicenceUSPA	// PYR-73719
   || licenseId == eLicenceUSCO;
	}
//	==== @license2-limit-confirm: (E) Licenses that require self-imposed limit confirmation @ ==>>


	//
	// TLB and BOP
	//
	static TlbPlanetKeys getTlbPlanetKey(UINT32 licenseId)
	{
		// [MJ 2010-08-19: PYR-15925]
		switch (licenseId)
		{
		case eLicenceItaly: return eKeyItRegularTlb; //-V525
		case eLicenceFrance: return eKeyFrRegularTlb;
		case eLicenceUSNJ: return eKeyUSNJRegularTlb;
		case eLicenceUSPA: return eKeyUSNJRegularTlb; // PYR-107460
		// TODO add eLicenceCZ
		default: return eKeyRegularTlb;
		}
	}

	//
	// Stellar Reward
	//
	static bool stellarRewardsSupported(UINT32 licenseId)
	{
//	<<== @license2-srs: (E) Licenses that support Stellar Rewards @ ====
  if( licenseId == eLicenceDefault
   || licenseId == eLicenceItaly
   || licenseId == eLicenceFrance
   || licenseId == eLicenceEstonia
   || licenseId == eLicenceBelgium
   || licenseId == eLicenceSpain	// #20993
   || licenseId == eLicenceDenmark	// #17012
   || licenseId == eLicenceEU	// #21486
   || licenseId == eLicenceUSNJ	// PYR-27228
   || licenseId == eLicenceUK
   || licenseId == eLicenceBG
   || licenseId == eLicenceDESH
   || licenseId == eLicencePortugal
   || licenseId == eLicenceCZ
   || licenseId == eLicenceIndia
   || licenseId == eLicenceSE
   || licenseId == eLicenceCH
   || licenseId == eLicenceDE	// PYR-124555
   || licenseId == eLicenceGR	// PYR-120936
			)
		{
			return true;
		}
//	==== @license2-srs: (E) Licenses that support Stellar Rewards @ ==>>
		return false;
	}

	static bool stellarRewardsGroupIdSupported(UINT32 groupId)
	{
//	<<== @license2-srsg: (E??) Licenses Stellar Rewards group support @ ====
		if(groupId == eLicenceDefault
		|| groupId == eLicenceItaly
		|| groupId == eLicenceFrance
		|| groupId == eLicenceBelgium
		|| groupId == eLicenceSpain	// #20993
		|| groupId == eLicenceDenmark	// #17012
		|| groupId == eLicenceEU	// #21486
		|| groupId == eLicenceUSNJ	// PYR-27228
		|| groupId == eLicenceUK	// PYR-31071
		|| groupId == eLicenceBG	// PYR-31070
		|| groupId == eLicenceDESH
		|| groupId == eLicencePortugal
		|| groupId == eLicenceCZ
		|| groupId == eLicenceIndia
		|| groupId == eLicenceSE
		|| groupId == eLicenceCH
		|| groupId == eLicenceDE	// PYR-124555
		|| groupId == eLicenceGR	// PYR-120936
			)
		{
			return true;
		}
//	==== @license2-srsg: (E??) Licenses Stellar Rewards group support @ ==>>
		// eLicenceEstonia cannot be a groupId because it shares the same groupId as eLicenceDefault
		return false;
	}

	static UINT32 getStellarGroupId(UINT32 licenseId)
	{
//	<<== @license2-srsgid: (E) Stellar Rewards group id @ ====
		if(licenseId == eLicenceEstonia)
		{
			return eLicenceDefault;	// PYR-15612 Estonian License - behavior matches .COM
		}
//	==== @license2-srsgid: (E) Stellar Rewards group id @ ==>>
		return licenseId;
	}

//	<<== @license2-srcc: (E) Stellar Rewards currency @ ====
	static const char* getStellarCurrency(UINT32 licenseId)
	{
		switch(licenseId)
		{
			case eLicenceDefault:
			case eLicenceEstonia:
			case eLicenceDenmark:	// #17012
			case eLicenceEU:	// #21486
			case eLicenceUSNJ:	// PYR-27228
			case eLicenceUK:
			case eLicenceBG:
			case eLicenceDESH:
			case eLicenceCZ:
			case eLicenceSE:
			case eLicenceCH:
			case eLicenceDE:	// PYR-124555
			case eLicenceGR:	// PYR-120936
			{
				return "USD";
			}
			case eLicenceIndia:
			{
				return "INR";
			}
			// Explicit listing of Stellar Reqards licenses with EUR (see default below)
			case eLicenceItaly:
			case eLicenceFrance:
			case eLicenceBelgium:
			case eLicenceSpain:
			case eLicencePortugal:
			{
				return "EUR";
			}
		}
		return "EUR"; // Return EUR even for licenses that do not support Stellar Rewards
	}
//	==== @license2-srcc: (E) Stellar Rewards currency @ ==>>

//	<<== @license2-sr-tmoney: (E) Stellar Rewards payments via Tmoney @ ====
	static INT32 getStellarAmount(UINT32 licenseId, INT32 amount)
	{
		return licenseId == eLicenceItaly ? 0 : amount;
	}

	static INT32 getStellarTAmount(UINT32 licenseId, INT32 tAmount)
	{
		return licenseId == eLicenceItaly ? tAmount : 0;
	}
//	==== @license2-sr-tmoney: (E) Stellar Rewards payments via Tmoney @ ==>>

	//
	// Login
	//

	//PYR-17095, returns true if city should be shown
	static bool showCityByLicenses(UINT32 license1, UINT32 license2)
	{
//	<<== @license2-show-city: (E) License that require to show city @ ====
		if(license1 == license2)
		{
			if(license1 == eLicenceItaly
			|| license1 == eLicenceFrance
			|| license1 == eLicenceEstonia
			|| license1 == eLicenceBelgium
			|| license1 == eLicenceSpain// #21043
			|| license1 == eLicenceDenmark// #21042
			|| license1 == eLicenceUSNJ// PYR-27853
			|| license1 == eLicenceUK
			|| license1 == eLicenceBG
			|| license1 == eLicenceDESH// PYR-33562
			|| license1 == eLicencePortugal
			|| license1 == eLicenceIndia
			|| license1 == eLicenceRU
			|| license1 == eLicenceUSPA
			|| license1 == eLicenceUSCO
				)
				return true;
		}
//	==== @license2-show-city: (E) License that require to show city @ ==>>
		return false;
	}

	static bool interLicenseRmTransferAllowed(UINT32 licenseFrom, UINT32 licenseTo)
	{
		if(licenseFrom == licenseTo)
			return true;

//	<<== @license2-rm-transfer: (E) Licenses that allow RM inter-license transfers  @ ====
		// BE -> IOM and IOM -> BE
		return	(licenseFrom == eLicenceBelgium && licenseTo == eLicenceDefault)
			 ||	(licenseFrom == eLicenceDefault && licenseTo == eLicenceBelgium)
		// EU -> IOM and IOM -> EU
			 ||	(licenseFrom == eLicenceEU && licenseTo == eLicenceDefault)
			 ||	(licenseFrom == eLicenceDefault && licenseTo == eLicenceEU)
		// EU -> BE and BE -> EU
			 ||	(licenseFrom == eLicenceEU && licenseTo == eLicenceBelgium)
			 ||	(licenseFrom == eLicenceBelgium && licenseTo == eLicenceEU)
		// UK -> IOM and IOM -> UK
			 ||	(licenseFrom == eLicenceUK && licenseTo == eLicenceDefault)
			 ||	(licenseFrom == eLicenceDefault && licenseTo == eLicenceUK)
		// UK -> EU and EU -> UK// PYR-32914
			 ||	(licenseFrom == eLicenceUK && licenseTo == eLicenceEU)
			 ||	(licenseFrom == eLicenceEU && licenseTo == eLicenceUK)
		// UK -> BE and BE -> UK// PYR-34871
			 ||	(licenseFrom == eLicenceUK && licenseTo == eLicenceBelgium)
			 ||	(licenseFrom == eLicenceBelgium && licenseTo == eLicenceUK)
			 ;
//	==== @license2-rm-transfer: (E) Licenses that allow RM inter-license transfers  @ ==>>
	}

	static bool okToPlayFreerollWithoutRmOK(UINT32 licenseId)
	{
//	<<== @license2-freeroll-ok: (E) Licenses that allow Freeroll play w/o RmOK @ ====
		switch(licenseId)
		{
			case eLicenceDefault: 	// #30113
			case eLicenceBelgium: 	// #20082
			case eLicenceSpain: 	// #21282
			case eLicenceDenmark: 	// #21282
			case eLicenceEU: 	// #30113
			case eLicenceRomania:
			case eLicenceIndia:
			case eLicenceDE: 	// PYR-124555
			case eLicenceGR: 	// PYR-120936
			{
				return true;
			}
		}
//	==== @license2-freeroll-ok: (E) Licenses that allow Freeroll play w/o RmOK @ ==>>
		return false;
	}

	// Buy-In Limit
	static bool buyInLimitSupported(UINT32 licenseId)
	{
//	<<== @license2-buyin-limit: (E) Licenses that support buy-in limit @ ====
		switch(licenseId)
		{
			case eLicenceItaly: 	// #21307
			case eLicenceFrance:
			{
				return true;
			}
		}
//	==== @license2-buyin-limit: (E) Licenses that support buy-in limit @ ==>>
		return false;
	}

	static bool buyInLimitMustBeSet(UINT32 licenseId)
	{
//	<<== @license2-set-buyin-limit: (E) Licenses that require buy-in limit to be set @ ====
		switch(licenseId)
		{
			case eLicenceFrance:
			{
				return true;
			}
		}
//	==== @license2-set-buyin-limit: (E) Licenses that require buy-in limit to be set @ ==>>
		return false;
	}

	//PYR-27249
	static bool buyInLimitExSupported(UINT32 licenseId)
	{
//	<<== @license2-buyin-limit-ex: (E) Licenses that support buy-in limit self-exclusion @ ====
		switch(licenseId)
		{
			case eLicenceUSNJ:
			case eLicencePortugal: 	// PYR-38889
			case eLicenceUSPA:
			case eLicenceCH: 	// PYR-96610
			case eLicenceUSCO:
			{
				return true;
			}
		}
//	==== @license2-buyin-limit-ex: (E) Licenses that support buy-in limit self-exclusion @ ==>>
		return false;
	}

	static bool buyInLimitExMustBeSet(UINT32 licenseId)
	{
//	<<== @license2-set-buyin-limit-ex: (E) Licenses that require setting buy-in limit self-exclusion @ ====
		switch(licenseId)
		{
			case eLicenceCH: 	// PYR-96610
			{
				return true;
			}
		default:
			{
				return false;
			}
		}
//	==== @license2-set-buyin-limit-ex: (E) Licenses that require setting buy-in limit self-exclusion @ ==>>
	}

	//	PYR-100863 Deprecated.
	static void addToMask( const UINT32 licenseId, UINT32& licenseMask /*in-out parameter*/)
	{
		licenseMask |= ( licenseId == eLicenceDefault ? _LicenceDefaultBit_ForMaskUseOnly : licenseId );
	}

	// PYR-90648 For configuration matters ONLY. DO NOT use for business logic.
	static bool isPeripheralHostLicense(UINT32 licenseId)
	{
//	<<== @license2-peripheral-host: (E) Licenses hosted in non-IOM DC (peripheral hosts) @ ====
		switch(licenseId)
		{
			case eLicenceUSNJ:
			case eLicenceUSPA:
			case eLicenceUSCO:
			return true;
		default:
			return false;
		}
//	==== @license2-peripheral-host: (E) Licenses hosted in non-IOM DC (peripheral hosts) @ ==>>
	}

	static bool areChallengeQuestionsOnRMF( UINT32 licenseId )		//	PYR-31158
//	<<== @license2-rmf-question: (E) licenses that provide challenge questions in RMF @ ====
	{
		return licenseId == eLicenceItaly
		    || licenseId == eLicenceUSNJ
		    || licenseId == eLicenceUSPA 	// PYR-70708
		    || licenseId == eLicenceUSCO
			;
	}
//	==== @license2-rmf-question: (E) licenses that provide challenge questions in RMF @ ==>>


	static bool canMigrateToCom(const UINT32 licenseId)
	{
//	<<== @license2-allow-migration-com: (E) Licenses that can migrate to COM @ ====
		switch(licenseId)
		{
			case eLicenceEstonia:
			case eLicenceBelgium:
			case eLicenceDenmark:
			case eLicenceEU:
			case eLicenceUK:
			case eLicenceGR:
			{
				return true;
			}
		}
//	==== @license2-allow-migration-com: (E) Licenses that can migrate to COM @ ==>>
		return false;
	}

	static bool canMigrateToEu(const UINT32 licenseId)
	{
//	<<== @license2-allow-migration-eu: (E) Licenses that can migrate to EU @ ====
		switch(licenseId)
		{
			case eLicenceEstonia: 	// PYR-118031
			case eLicenceBelgium: 	// PYR-118031
			case eLicenceDenmark: 	// PYR-118031
			case eLicenceUK: 	// PYR-118031
			{
				return true;
			}
		}
//	==== @license2-allow-migration-eu: (E) Licenses that can migrate to EU @ ==>>
		return false;
	}

	static INT32 getConversionUserPropertyType(UINT32 licenseId)
	{
		switch (licenseId)
		{
			case eLicenceDefault: 
				return eUserPropertyConvertedToIom;
//	<<== @license2-cup-suffix: (E) Conversion user property by license  @ ====
			case eLicenceFrance:
				return eUserPropertyDotFrConverted; 	// #14432
			case eLicenceEstonia:
				return eUserPropertyDotEeConverted; 	// PYR-15751
			case eLicenceBelgium:
				return eUserPropertyDotBeConverted; 	// PYR-18397
			case eLicenceSpain:
				return eUserPropertyDotEsConverted; 	// int - origLicenseId
			case eLicenceDenmark:
				return eUserPropertyDotDkConverted; 	// int - origLicenseId
			case eLicenceEU:
				return eUserPropertyDotEuConverted; 	// int - origLicenseId, str - source SiteIdCode for silent migration only
			case eLicenceUSNJ:
				return eUserPropertyUsConverted; 	// int - origLicenseId
			case eLicenceUK:
				return eUserPropertyDotUkConverted; 	// int - origLicenseId
			case eLicenceBG:
				return eUserPropertyDotBgConverted; 	// int - origLicenseId
			case eLicenceDESH:
				return eUserPropertyToDESHConverted; 	// PYR-33564. SH - Desktop migration, int - origLicenseId
			case eLicenceRomania:
				return eUserPropertyDotRoConverted; 	// PYR-43114. int - origLicenseId
			case eLicencePortugal:
				return eUserPropertyDotPtConverted; 	// PYR-38563. int - origLicenseId
			case eLicenceCZ:
				return eUserPropertyDotCzConverted; 	// PYR-47567 int - origLicenseId
			case eLicenceIndia:
				return eUserPropertyDotInConverted; 	// PYR-30559 int - origLicenseId
			case eLicenceUSPA:
				return eUserPropertyUsConverted; 	// int - origLicenseId
			case eLicenceSE:
				return eUserPropertyDotSeConverted; 	// PYR-78227. int - origLicenseId
			case eLicenceCH:
				return eUserPropertyDotChConverted; 	// PYR-89650. int - origLicenseId
			case eLicenceARBA:
				return eUserPropertyDotArbaConverted; 	// PYR-114733. int - origLicenseId
			case eLicenceUSCO:
				return eUserPropertyUsConverted;
			case eLicenceGR:
				return eUserPropertyDotGrConverted; 	// int - origLicenseId
//	==== @license2-cup-suffix: (E) Conversion user property by license  @ ==>>
			default:
				break;
		};
		return -1;
	}

	static bool supportsRegulatoryNac(UINT32 licenseId) // PYR-39019, Regulatory NAC: IT/PT/RO
	{
//	<<== @license2-regulatory-nac: (E) licenses that impose regulatory NAC @ ====
	
		return licenseId == eLicenceItaly
		    || licenseId == eLicenceRomania
		    || licenseId == eLicencePortugal 	// PYR-39019
			;

//	==== @license2-regulatory-nac: (E) licenses that impose regulatory NAC @ ==>>
	}

	static bool supportsTTicketTotal(UINT32 licenseId) // PYR-43008
	{
		// The function applicable for single currency licenses only!!!
		// Update function when eLicenceAsia or eLicenceCH becomes multi-currency.
//	<<== @license2-ttt-accounting: (E) Licenses that need to keep TTT (Tournament Ticket Total) value in account total  @ ====
	
		return licenseId == eLicenceItaly
		    || licenseId == eLicenceFrance 	// PYR-47250
		    || licenseId == eLicenceSpain 	// PYR-65747
		    || licenseId == eLicencePortugal
		    || licenseId == eLicenceAsia 	// PYR-64516  // TTT value calculated in USD.
		    || licenseId == eLicenceCH 	// PYR-88255  // TTT value calculated in USD.
			;

//	==== @license2-ttt-accounting: (E) Licenses that need to keep TTT (Tournament Ticket Total) value in account total  @ ==>>
	}
	static bool buyinLimitIncludesTicketsTChips(UINT32 licenseId)		//	PYR-49018
	{
//	<<== @license2-buyin-thips: (E) Licenses that account TChips in buyin limits @ ====	
		return licenseId == eLicenceCZ;
//	==== @license2-buyin-thips: (E) Licenses that account TChips in buyin limits @ ==>>
	}

	static bool emailRequired(UINT32 licenseId)	//	PYR-52518
	{
//	<<== @license2-no-email: (E) Licenses that do not require account email @ ====	
		return licenseId != eLicenceAsia;
//	==== @license2-no-email: (E) Licenses that do not require account email @ ==>>
	}

	//	PYR-83061
	static bool forceLogoutOnLimitedSessionExpire( UINT32 licenseId )
	{
//	<<== @license2-session-timeout: (E) Licenses that require session timeout message to player @ ====	
		return licenseId == eLicenceCZ
		    || licenseId == eLicenceSE;
//	==== @license2-session-timeout: (E) Licenses that require session timeout message to player @ ==>>
	}

	//	PYR-89430
	static bool sendLoginNotificationEmail(UINT32 licenseId)
	{
//	<<== @license2-notify-login: (E) Licenses that notify account holder via different channel on account login event @ ====	
		return licenseId == eLicenceUSNJ
		    || licenseId == eLicenceUSPA
		    || licenseId == eLicenceUSCO;
//	==== @license2-notify-login: (E) Licenses that notify account holder via different channel on account login event @ ==>>
	}

	// PYR-96119 For business logic ONLY. NOT to be used for configuration / host-related matters.
	static bool isUsaLicense(UINT32 licenseId)
	{
//	<<== @license2-is-usa-license: (E) USA licenses @ ====	
		return licenseId == eLicenceUSNJ
		    || licenseId == eLicenceUSPA
		    || licenseId == eLicenceUSCO;
//	==== @license2-is-usa-license: (E) USA licenses @ ==>>
	}

	// PYR-92691
	static const char* getUsaState(UINT32 licenseId)
	{
		switch (licenseId)
		{
//	<<== @license2-usa-state-from-license: (E) get USA state from a license @ ====
		case eLicenceUSNJ:
			return "NJ";
		case eLicenceUSPA:
			return "PA";
		case eLicenceUSCO:
			return "CO";
//	==== @license2-usa-state-from-license: (E) get USA state from a license @ ==>>
		default:
			break;
		}
		return LIC_UNKNOWN_CODE;
	}

	// PYR-47572 PYR-88284
//	<<== @license2-verify-cc: (E) Licenses that require CC verification @ ====
	static bool isCardVerificationRequired(UINT32 licenseId)
	{	
		return licenseId == eLicencePortugal 	// PYR-88284
		    || licenseId == eLicenceCZ; 	// PYR-47572
	}
//	==== @license2-verify-cc: (E) Licenses that require CC verification @ ==>>
};

// client site - see Sites.h
class SiteRules
{
public:
	static const PSiteMask& getPeripheralHostSitesMask();
	static const PSiteMask& getCentralHostSitesMask();

	[[deprecated("use PSiteMask::getSiteNumber() instead")]]
	static bool siteNumberFromMask(const PSiteMask& bitmask, UINT32& siteNum)
	{
		return bitmask.getSiteNumber((Sites&)siteNum);
	}

	// PYR-109216 - deprecated, use PSiteMask::maskFromSiteNumber() instead.
	static bool maskFromSiteNumber(UINT32 siteNum, PBitmask& bitmask);

	// PYR-109216 - deprecated, use PSiteMask::maskFromSiteNumber() instead.
	static bool maskFromSiteNumber(UINT32 siteNum, PSiteMask& bitmask)
	{
		return PSiteMask::maskFromSiteNumber((Sites)siteNum, bitmask);
	}

	// PYR-109216 - deprecated, use PSiteMask::containsSiteId() instead.
	static bool maskContainsSiteId(UINT32 siteNum, const PSiteMask& bitmask)
	{
		return bitmask.containsSiteId((Sites)siteNum);
	}

	[[deprecated("use PSiteMask::fromSiteListStr() instead")]]
	static PSiteMask getSiteMaskFromCommaSeparatedList(const char* siteList, const char* separators = ",")
	{
		PSiteMask mask;
		mask.fromSiteListStr(siteList, separators);

		return mask;
	}

	//
	// Common
	//
	// PYR-114529 Moved to PLicenseMask
	[[deprecated("use PSiteMask::getSiteIdCode instead")]]
	static const char* getSiteIdCode(UINT32 siteId)
	{
		return PSiteMask::getSiteIdCode((Sites)siteId);
	}

	[[deprecated("use PSiteMask::getSiteNumber instead")]]
	static bool siteIdFromMask(const PSiteMask& bitmask, UINT32& siteId)
	{
		return bitmask.getSiteNumber((Sites&)siteId);
	}

	// PYR-114529 Moved to PSiteMask.
	//The name getSiteIdCodeWithNet is misleading. In reality - can recognize NET, RUSO, RUSOPM.
	[[deprecated("use PSiteMask::getSiteIdCodeWithNet() instead")]]
	static const char* getSiteIdCodeWithNet(UINT32 siteId)
	{
		return PSiteMask::getSiteIdCodeWithNet((Sites)siteId);
	}

	// PYR-114529 Moved to PSiteMask.
	[[deprecated("use PSiteMask::getSiteId() instead")]]
	static UINT32 getSiteId(const char* siteCode)
	{
		return PSiteMask::getSiteId(siteCode);
	}
	
	// PYR-114529 Moved to PSiteMask.
	//The name getSiteIdWithNet is misleading. In reality - can recognize NET, RUSO, RUSOPM.
	static UINT32 getSiteIdWithNet(const char* siteCode)
	{
		return PSiteMask::getSiteIdWithNet(siteCode);
	}


	// Client sends siteId shifted right by one bit.
	static LicenceId getLicenseIdByClientShiftedSiteId(UINT32 siteId)
	{
		switch(siteId)
		{
//	<<== @site2-license-by-shited-site-id: (E) Get license id from client-provided site id (shifted by 1 bit) @ ====
//---------------------------------
//	will not work for new siteid
//---------------------------------
//	==== @site2-license-by-shited-site-id: (E) Get license id from client-provided site id (shifted by 1 bit) @ ==>>
		case (PokerStars_Ita >> 1):
			return eLicenceItaly;
		case (PokerStars_Fra >> 1):
			return eLicenceFrance;
		case (PokerStars_Est >> 1):
			return eLicenceEstonia;
		case (PokerStars_Bel >> 1):
			return eLicenceBelgium;
		case (PokerStars_Spa >> 1):
			return eLicenceSpain;
		case (PokerStars_Den >> 1):
			return eLicenceDenmark;
		case (PokerStars_EU >> 1):
			return eLicenceEU;
		case (PokerStars_FB >> 1):
			return eLicenceFB;
		case (PokerStars_US_NJ >> 1):
			return eLicenceUSNJ;
		case (PokerStars_UK >> 1):
			return eLicenceUK;
		case (PokerStars_Bul >> 1):
			return eLicenceBG;
		case (PokerStars_DE_SH >> 1):
			return eLicenceDESH;
		case (PokerStars_Rom >> 1):
			return eLicenceRomania;
		case (PokerStars_Por >> 1):
			return eLicencePortugal;
		case (PokerStars_Cze >> 1):
			return eLicenceCZ;
		case (PokerStars_Ind >> 1):
			return eLicenceIndia;
		case (PokerStars_Asia >> 1):
			return eLicenceAsia;
		case (PokerStars_RU >> 1):
			return eLicenceRU;
		case (PokerStars_US_PA >> 1):
			return eLicenceUSPA;
		case (PokerStars_Swe >> 1):
			return eLicenceSE;
		case (PokerStars_CH >> 1):
			return eLicenceCH;
		}
		return eLicenceDefault;
	}

	static LicenceId getLicenseIdByClientSiteId(UINT32 siteId)
	{
		switch(siteId)
		{
//	<<== @site2-license-by-site-id: Get license id from site id @ ====
		case PokerStars_Ita:
			return eLicenceItaly;
		case PokerStars_Fra:
			return eLicenceFrance;
		case PokerStars_Est:
			return eLicenceEstonia;
		case PokerStars_Bel:
			return eLicenceBelgium;
		case PokerStars_Spa:
			return eLicenceSpain;
		case PokerStars_Den:
			return eLicenceDenmark;
		case PokerStars_EU:
			return eLicenceEU;
		case PokerStars_FB:
			return eLicenceFB;
		case PokerStars_US_NJ:
			return eLicenceUSNJ;
		case PokerStars_UK:
			return eLicenceUK;
		case PokerStars_Bul:
			return eLicenceBG;
		case PokerStars_DE_SH:
			return eLicenceDESH;
		case PokerStars_Rom:
			return eLicenceRomania;
		case PokerStars_Por:
			return eLicencePortugal;
		case PokerStars_Cze:
			return eLicenceCZ;
		case PokerStars_Ind:
			return eLicenceIndia;
		case PokerStars_Asia:
			return eLicenceAsia;
		case PokerStars_RU:
			return eLicenceRU;
		case PokerStars_US_PA:
			return eLicenceUSPA;
		case PokerStars_Swe:
			return eLicenceSE;
		case PokerStars_CH:
			return eLicenceCH;
		case PokerStars_ARBA:
			return eLicenceARBA;
		case PokerStars_US_CO:
			return eLicenceUSCO;
		case PokerStars_DE:
			return eLicenceDE;
		case PokerStars_GR:
			return eLicenceGR;
//	==== @site2-license-by-site-id: Get license id from site id @ ==>>
		}
		return eLicenceDefault;
	}

	// PYR-16191
	static UINT32 getShiftedSiteIdByLicenseId(UINT32 licenseId)
	{
		return (getSiteIdByLicenseId(licenseId) >> 1);
	}

	// #21113
	static UINT32 getSiteIdByLicenseId(UINT32 licenseId)
	{
		switch(licenseId)
		{
//	<<== @site2-site-id-from-license-id: Get site id from license id @ ====
		case eLicenceItaly:
			return PokerStars_Ita;
		case eLicenceFrance:
			return PokerStars_Fra;
		case eLicenceEstonia:
			return PokerStars_Est;
		case eLicenceBelgium:
			return PokerStars_Bel;
		case eLicenceSpain:
			return PokerStars_Spa;
		case eLicenceDenmark:
			return PokerStars_Den;
		case eLicenceEU:
			return PokerStars_EU;
		case eLicenceFB:
			return PokerStars_FB;
		case eLicenceUSNJ:
			return PokerStars_US_NJ;
		case eLicenceUK:
			return PokerStars_UK;
		case eLicenceBG:
			return PokerStars_Bul;
		case eLicenceDESH:
			return PokerStars_DE_SH;
		case eLicenceRomania:
			return PokerStars_Rom;
		case eLicencePortugal:
			return PokerStars_Por;
		case eLicenceCZ:
			return PokerStars_Cze;
		case eLicenceIndia:
			return PokerStars_Ind;
		case eLicenceAsia:
			return PokerStars_Asia;
		case eLicenceRU:
			return PokerStars_RU;
		case eLicenceUSPA:
			return PokerStars_US_PA;
		case eLicenceSE:
			return PokerStars_Swe;
		case eLicenceCH:
			return PokerStars_CH;
		case eLicenceARBA:
			return PokerStars_ARBA;
		case eLicenceUSCO:
			return PokerStars_US_CO;
		case eLicenceDE:
			return PokerStars_DE;
		case eLicenceGR:
			return PokerStars_GR;
//	==== @site2-site-id-from-license-id: Get site id from license id @ ==>>
		default:
			return PokerStars_Com;
		}
	}

	static const PSiteMask& getSiteMaskByLicenseId( UINT32 licenseId );

	//
	// Stellar Reward
	//
	static UINT32 getStellarGrouIdBySite(UINT32 siteId)
	{
		switch(siteId)
		{
//	<<== @license2-srsg-by-site: Stellar rewards license by site id @ ====
//------------------------------------------------------------------
//	The list below doesn't match 'stellarRewardsGroupIdSupported'
//------------------------------------------------------------------
		case PokerStars_Ita:
			return eLicenceItaly;
		case PokerStars_Fra:
			return eLicenceFrance;
		case PokerStars_Bel:
			return eLicenceBelgium;
		case PokerStars_Spa:
			return eLicenceSpain;	// #20993
		case PokerStars_Den:
			return eLicenceDenmark;	// #17012
		case PokerStars_EU:
			return eLicenceEU;	// #21486
		case PokerStars_US_NJ:
			return eLicenceUSNJ;	// PYR-27228
		case PokerStars_UK:
			return eLicenceUK;	// PYR-31071
		case PokerStars_Bul:
			return eLicenceBG;	// PYR-31070
		case PokerStars_DE_SH:
			return eLicenceDESH;
		case PokerStars_Por:
			return eLicencePortugal;
		case PokerStars_Cze:
			return eLicenceCZ;
		case PokerStars_Ind:
			return eLicenceIndia;
		case PokerStars_Swe:
			return eLicenceSE;
		case PokerStars_CH:
			return eLicenceCH;
		case PokerStars_DE:
			return eLicenceDE;	// PYR-124555
		case PokerStars_GR:
			return eLicenceGR;	// PYR-120936
//	==== @license2-srsg-by-site: Stellar rewards license by site id @ ==>>
		}
		// #PYR-15618 Estonian License - behavior matches .COM
		return eLicenceDefault;
	}

	//
	// Tournaments
	//
	static const char* getUserTournSiteSpecificPart(UINT32 siteId)
//	<<== @license2-tourn-site-parts: Tournament site-specific parts @ ====
// --------------------------------------------------------
// Check for consistency with definitions in sitemasks.cpp:
// - SiteBitmask_SharedLiquidity
// - SiteBitmask_Segregated
// - SiteBitmask_IOM_Segregated
// - SiteBitmask_SharedLiquidityPool
// --------------------------------------------------------
	{
#define ALL_SHARED_LIQUIDITY_SITES_STRING	"Site=PokerStars.com,PokerStars.ee,PokerStars.be,PokerStars.dk,PokerStars.eu,PokerStars.uk,PokerStars.bg,PokerStars.DESH,PokerStars.ro,PokerStars.cz,PokerStars.RuSo,PokerStars.se,PokerStars.ch,PokerStars.arba,pokerstars.gr\n"
#define ALL_SHARED_LIQUIDITY_POOL_SITES_STRING "Site=PokerStars.fr,PokerStars.es,PokerStars.pt\n" // PYR-52938

		switch(siteId)
		{
		case PokerStars_Ita:
			return	"Site=PokerStars.it\n"
					"Currency=EUR\n"
					"ChatLang=Italian\n"
					"RequiredApprovalFromAAMS=Yes\n";
		case PokerStars_Fra:
			return	ALL_SHARED_LIQUIDITY_POOL_SITES_STRING
					"Currency=EUR\n"
					"ChatLang=French\n";
		case PokerStars_Spa:
			return	ALL_SHARED_LIQUIDITY_POOL_SITES_STRING
					"Currency=EUR\n"
					"ChatLang=Spanish\n";
		case PokerStars_Est:
			return	ALL_SHARED_LIQUIDITY_SITES_STRING
					"ChatLang=Estonian\n";
		case PokerStars_Den:
			return	ALL_SHARED_LIQUIDITY_SITES_STRING
					"ChatLang=Danish\n";
		case PokerStars_FB:
			return	"Site=PokerStars.fb\n";
		case PokerStars_US_NJ:
			return	"Site=PokerStars.usnj\n";
		case PokerStars_Bul:
			return	ALL_SHARED_LIQUIDITY_SITES_STRING
					"ChatLang=Bulgarian\n";
		case PokerStars_DE_SH:
			return	ALL_SHARED_LIQUIDITY_SITES_STRING
					"ChatLang=German\n";
		case PokerStars_Por:
			return	ALL_SHARED_LIQUIDITY_POOL_SITES_STRING
					"Currency=EUR\n"
					"ChatLang=Portuguese (Portugal)\n";
		case PokerStars_Rom:
			return	ALL_SHARED_LIQUIDITY_SITES_STRING
					"ChatLang=Romanian\n";
		case PokerStars_Cze:
			return	ALL_SHARED_LIQUIDITY_SITES_STRING
					"ChatLang=Czech\n";
		case PokerStars_Ind:
			return	"Site=PokerStars.in\n"
					"Currency=INR\n";
		case PokerStars_US_PA:
			return	"Site=PokerStars.uspa\n";
		case PokerStars_Swe:
			return	ALL_SHARED_LIQUIDITY_SITES_STRING
					"ChatLang=Swedish\n";
		case PokerStars_US_CO:
			return	"Site=Pokerstars.usco\n";
		case PokerStars_DE:
			return	"Site=Pokerstars.de\n";
		// case PokerStars_CH: // PYR-85408 no specific customization for CH, fall into default ALL_SHARED_LIQUIDITY_SITES_STRING case below
		}

		return ALL_SHARED_LIQUIDITY_SITES_STRING;
#undef ALL_SHARED_LIQUIDITY_SITES_STRING
#undef ALL_SHARED_LIQUIDITY_POOL_SITES_STRING
	}
//	==== @license2-tourn-site-parts: Tournament site-specific parts @ ==>>


	//
	// Login
	//
	static bool isClientSoftwareLicensed(UINT32 siteId)
	{
		if(siteId < PokerStars_Ita
//	<<== @license2-is-client-licensed: Sites that use unlicensed client @ ====
			|| siteId == PokerStars_ESPN
			|| siteId == PokerStars_FOX
			|| siteId == PokerStars_FB
			|| siteId == PokerStars_RUSO
			|| siteId == PokerStars_RUSOPM
//	==== @license2-is-client-licensed: Sites that use unlicensed client @ ==>>
			)
		{
			// PokerStars_SiteUnknown, PokerStars_Com, PokerStars_Net, PokerStars_Lite, HoyleSierra_Com
			// PokerStars_ESPN, PokerStars_FOX, PokerStars_FB, PokerStars_RUSO, PokerStars_RUSOPM
			return false;
		}
		return true; // PokerStars_Ita, PokerStars_Fra, PokerStars_Est, PokerStars_Bel, PokerStars_Spa, PokerStars_Den
	}

	static UINT32 shiftedSiteIdToNewsSiteId(UINT32 lobbySiteId) // moved from templateServer (EmailProcessing.cpp)
	{
		switch(lobbySiteId)
		{
		case (PokerStars_Com >> 1):		return PokerStars_Com;		//lint !e572	//Excessive shift value
		case (PokerStars_Ita >> 1):		return PokerStars_Ita;
		case (PokerStars_Fra >> 1):		return PokerStars_Fra;
		case (PokerStars_Est >> 1):		return PokerStars_Est;
		case (PokerStars_ESPN >> 1):	return PokerStars_ESPN;
		case (PokerStars_FOX >> 1):		return PokerStars_FOX;
		case (PokerStars_Bel >> 1):		return PokerStars_Bel;
		case (PokerStars_Spa >> 1):		return PokerStars_Spa;
		case (PokerStars_Den >> 1):		return PokerStars_Den;
		case (PokerStars_EU >> 1):		return PokerStars_EU;
		case (PokerStars_FB >> 1):		return PokerStars_FB; // PSP-3348, PYR-25723
		case (PokerStars_US_NJ >> 1):	return PokerStars_US_NJ;
		case (PokerStars_UK >> 1):	    return PokerStars_UK;
		case (PokerStars_Bul >> 1):	    return PokerStars_Bul;
		case (PokerStars_DE_SH >> 1):	return PokerStars_DE_SH;
		case (PokerStars_Por >> 1):		return PokerStars_Por;
		case (PokerStars_Rom >> 1):		return PokerStars_Rom;
		case (PokerStars_Cze >> 1):		return PokerStars_Cze;
		case (PokerStars_RUSO >> 1):	return PokerStars_RUSO;
		case (PokerStars_RUSOPM >> 1):	return PokerStars_RUSOPM;
		case (PokerStars_Ind >> 1):		return PokerStars_Ind;
		case (PokerStars_Asia >> 1):	return PokerStars_Asia;
		case (PokerStars_RU >> 1):		return PokerStars_RU;
		case (PokerStars_US_PA >> 1):	return PokerStars_US_PA;
		case (PokerStars_Swe >> 1):		return PokerStars_Swe;
		case (PokerStars_CH >> 1):		return PokerStars_CH;
		default:						return PokerStars_Net;
		}
	}

	static bool doSiteAndLicenseMatch( UINT32 licenseId, const PSiteMask& siteMask );

	//PYR-17095, returns true if city should be shown
	static bool showCityBySiteMask( const PSiteMask& siteMask );

	//PYR-22119, returns mask for sites that are forbidden to insert with delta or clone admin tool
	static const PSiteMask& getSiteMaskCantBeAddedWithDeltaTool();

	// ALL sites are within the shared liquidity pool
	static bool belongsToSharedLiquidityPool( const PSiteMask& siteMask ); // PYR-52935

	// SOME sites are within the shared liquidity pool
	static bool isSharedLiquidityPoolMask( const PSiteMask& siteMask ); // PYR-52935

	static bool isConflictingSharedLiquidityPoolMask( const PSiteMask& siteMask );

	static bool isSegregatedSiteMask( const PSiteMask& siteMask ); // PYR-52935

	static bool isConflictingSegregatedSiteMask( const PSiteMask& siteMask ); // PYR-34073

	// conflicting segregated or shared pool
	static bool isConflictingSiteMask( const PSiteMask& siteMask );
};

// License rules that depend on host configuration
// Also includes site rules that depend on license rules that depend on host configuration
class LicenseRulesConfig
{
public:
	typedef map<UINT32 /*enum LicenceId*/, UINT32 /*enum eHost*/> LicenseHostMap;
	LicenseHostMap _peripheralLicenseToHostMap; // LicenseId => HostId, should not be accessed directly!

	LicenseRulesConfig ()
	{
		// initialized either by DbmLicenseRules::readAndPrintLicenseHostConfig or parse
	}

	void compose(CommMsgBody& envelopeBody) const
	{
		CommMsgBody payloadBody;
		payloadBody.composeUINT32(_peripheralLicenseToHostMap.size());
		for (const LicenseHostMap::value_type& e : _peripheralLicenseToHostMap)
		{
			CommMsgBody elementBody;
			elementBody
				.composeUINT32(e.first)
				.composeUINT32(e.second);
			payloadBody.composeMsgBody(elementBody);
		}
		envelopeBody.composeMsgBody(payloadBody);
	}

	void parse(CommMsgParser& envelopeParser)
	{
		if (!_peripheralLicenseToHostMap.empty())
		{
			PString logStr;
			PLog("LicenseRulesConfig::parse: re-init attempt ignored, current peripheralLicenseToHostMap=%s", toString(logStr));
			// no FL here
			envelopeParser.skipMsgBody();
			return;
		}

		CommMsgBody payloadBody;
		envelopeParser.parseMsgBody(payloadBody);
		CommMsgParser payloadParser(payloadBody);

		UINT32 size = 0;
		payloadParser.parseUINT32(size);
		for (UINT32 i = 0; i < size; ++i)
		{
			CommMsgBody elementBody;
			payloadParser.parseMsgBody(elementBody);
			UINT32 licenseId;
			UINT32 hostId;
			CommMsgParser elementParser(elementBody);
			elementParser
				.parseUINT32(licenseId)
				.parseUINT32(hostId);
			_addLicenseHost(licenseId, hostId);
		}
	}

	const char* toString(PString& str) const
	{
		str.assign("{");
		const char* delimiter = "";
		for (const LicenseHostMap::value_type& e : _peripheralLicenseToHostMap)
		{
			str.append(delimiter).appendUint(e.first).append(':').appendUint(e.second);
			delimiter = ",";
		}
		str.append('}');
		return str.c_str();
	}

	bool inititalized() const
	{
		return !_peripheralLicenseToHostMap.empty();
	}

	//
	// Licenses
	//

	// invariant: never returns eHost_Unknown
	UINT32 getHostByLicense(const UINT32 licenseId) const
	{
		// using default values if no configuration is loaded
		// to be removed after Payment Security tasks are done: PYR-99591, PYR-96996, PYR-99890, PYR-99593, PYR-95088
		if (_peripheralLicenseToHostMap.empty())
		{
			PLog(LOG_TRACE_FAULTY_LOGIC "LicenseRulesConfig::getHostByLicense: no configuration loaded, using default values (to be removed in 2019.Build.06)");
			switch (licenseId)
			{
//	<<== @license2-host-by-license: Retrieve host id by license @ ====
			case eLicenceUSNJ: return 2;
			case eLicenceUSPA: return 3;
			case eLicenceUSCO: return 3;
//	==== @license2-host-by-license: Retrieve host id by license @ ==>>
			default: return eHost_Central;
			}
		}
		// TODO: #vadimp remove the above in 2019.Build.06
		const LicenseHostMap::const_iterator i = _peripheralLicenseToHostMap.find(licenseId);
		if (i != _peripheralLicenseToHostMap.end())
		{
			if (!LicenseRules::isPeripheralHostLicense(licenseId))
			{
				PLog(LOG_TRACE_FAULTY_LOGIC "LicenseRulesConfig::getHostByLicense: non-peripheral licenseId(%u) in peripheralLicenseToHostMap", i->first);
				// no return
			}
			if (i->second == eHost_Central)
			{
				PLog(LOG_TRACE_FAULTY_LOGIC "LicenseRulesConfig::getHostByLicense: peripheral licenseId(%u) configured with central hostId(%u)", i->first, i->second);
				// no return
			}
			return i->second;
		}

		if (LicenseRules::isPeripheralHostLicense(licenseId))
		{
			PLog(LOG_TRACE_FAULTY_LOGIC "LicenseRulesConfig::getHostByLicense: peripheral licenseId(%u) needs to be in peripheralLicenseToHostMap", licenseId);
			// fl only, assuming that configuration takes precedence over hardcoded isPeripheralHostLicense
		}
		return eHost_Central;
	}

	bool doHostAndLicenseMatch(const UINT32 hostId, const UINT32 licenseId) const
	{
		if (hostId > eHost_Maximum)
		{
			PLog(LOG_TRACE_FAULTY_LOGIC "LicenseRulesConfig::doHostAndLicenseMatch: hostId(%u) > eHost_Maximum", hostId);
			// no return
		}
		return hostId == getHostByLicense(licenseId);
	}

	//	PYR-100863
	PLicenseMask getLicensesMaskByHostId(const UINT32 hostId) const;

	// checks whether the license mask provided maps to more than one host
	bool isCrossHostLicenseMask(const PLicenseMask& licenseIdMask) const; // PYR-28055, formerly known as 'isConflictingHostsLicenseMask'

	//
	// Sites
	//

	// invariant: never returns eHost_Unknown
	// this function considers PokerStars_FOX and PokerStars_ESPN a part of central host
	UINT32 getHostBySite(const UINT32 siteId) const
	{
		PSiteMask siteMask;
		SiteRules::maskFromSiteNumber(siteId, siteMask);

		UINT32 hostId = getHostByLicense(SiteRules::getLicenseIdByClientSiteId(siteId));
		if (hostId == eHost_Central && (SiteRules::getPeripheralHostSitesMask() & siteMask))
		{
			PLog(LOG_TRACE_FAULTY_LOGIC "LicenseRulesConfig::getHostBySite: peripheral siteId(%u) maps to central hostId(%u)", siteId, hostId);
			// no return
		}
		if (hostId != eHost_Central && (SiteRules::getCentralHostSitesMask() & siteMask))
		{
			PLog(LOG_TRACE_FAULTY_LOGIC "LicenseRulesConfig::getHostBySite: central siteId(%u) maps to non-central hostId(%u)", siteId, hostId);
			// no return
		}
		return hostId;
	}

	// based on the original getSitesMaskByHostId(), this function does not consider SiteBitmask_FOX | SiteBitmask_ESPN a part of central host
	PSiteMask getSitesMaskByHostId(const UINT32 hostId) const;

	// this function considers PokerStars_FOX and PokerStars_ESPN a part of central host
	// siteMask is passed by value on purpose.
	bool isCrossHostSiteMask(PSiteMask siteMask ) const; // PYR-28055, formerly known as 'isConflictingHostsSiteMask'

	// checks if ANY site in the mask matches the host
	// this function considers PokerStars_FOX and PokerStars_ESPN a part of central host
	bool siteMaskMatchHost(const UINT32 hostId, const PSiteMask& siteMask) const; // PYR-28055

	// do not use, should be used only in parse and DbmLicenseRules::readAndPrintLicenseHostConfig
	void _addLicenseHost(const UINT32 licenseId, const UINT32 hostId)
	{
		auto inserted = _peripheralLicenseToHostMap.emplace(licenseId, hostId);
		if (!inserted.second) // not inserted
		{
			PLog(LOG_TRACE_FAULTY_LOGIC "LicenseRulesConfig insert: duplicate LicenseId=%u ignored (HostId=%u)", licenseId, hostId);
			return;
		}
		
		if (!LicenseRules::isPeripheralHostLicense(licenseId))
		{
			PLog(LOG_TRACE_FAULTY_LOGIC "LicenseRulesConfig insert: non-peripheral LicenseId=%u (HostId=%u)", licenseId, hostId);
			// fl only, assuming that configuration takes precedence over hardcoded isPeripheralHostLicense
		}
		if (hostId <= eHost_Central || hostId > eHost_Maximum)
		{
			PLog(LOG_TRACE_FAULTY_LOGIC "LicenseRulesConfig insert: invalid hostId for LicenseId=%u (HostId=%u)", licenseId, hostId);
			// fl only
		}
	}
};

class PlatformRules
{
public:
	//
	// Common
	//
	static const char* getPlatformIdCode(UINT32 platformId)
	{
		switch(platformId)
		{
		case Client_Win:	return "WIN";
		case Client_Mac:	return "MAC";
        case Client_iOS:    return "IOS";  // PYR-21920
        case Client_Android: return "AND"; // PYR-21920
		case Client_Web:	return "WEB";  // PYR-22051
		case Client_CORE:	return "CORE";  // PYR-23463
		}
		return LIC_UNKNOWN_CODE;
	}

	static UINT32 getPlatformId(const char* platformCode)
	{
		if(!platformCode || !*platformCode)
		{
			return Client_UnknownPlatform;
		}
		if(PString::equals(platformCode, "WIN"))
		{
			return Client_Win;
		}
		if(PString::equals(platformCode, "MAC"))
		{
			return Client_Mac;
		}
        if(PString::equals(platformCode, "IOS")) // PYR-21920
        {
            return Client_iOS;
        }
        if(PString::equals(platformCode, "AND")) // PYR-21920
        {
            return Client_Android;
        }
		if(PString::equals(platformCode, "WEB")) // PYR-22051
		{
			return Client_Web;
		}
		if(PString::equals(platformCode, "CORE")) // PYR-23463
		{
			return Client_CORE;
		}
		return Client_UnknownPlatform;
	}

	static bool isMobilePlatform(UINT32 platform)
	{
		return platform == Client_iOS || platform == Client_Android;
	}
	static bool isDesktopPlatform(UINT32 platform)
	{
		return platform == Client_Win || platform == Client_Mac;
	}
	static UINT32 getMobilePlatformMask() // PYR-23460
	{
		return platform2mask(Client_iOS) | platform2mask(Client_Android);
	}
	static UINT32 platform2mask( ClientPlatform cp )
	{
		if( cp >= 32 ) //-V547
		{
			PLog(LOG_TRACE_FAULTY_LOGIC ": cp(%d) >= data size(32)", cp);
			return 0;
		}
		return ( 1 << cp );
	}
};

#endif // licenserules_h_included
