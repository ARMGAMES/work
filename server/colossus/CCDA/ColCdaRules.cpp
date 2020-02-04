////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "ColCdaRules.h"
#include "pplogfile.h"
#include "ppcontainer.h"
#include "../utils.h"
#include "stringroutines.h"
#include "licenserules.h"
#include "ColCdaCacheTypes.h"

using namespace ColCDA;

////////////////////////////////////////////////////////////////////////////////

const char* CountriesRulePart::toString(PString & out) const
{
	out.clear();
	if (!positiveCountryCodes.empty())
	{
		out.append("[");
		for (int i = 0; i < positiveCountryCodes.size(); ++i)
		{
			if (i > 0)
			{
				out.append(",");
			}
			out.append(positiveCountryCodes[i]);
		}
		out.append("]");
	}
	if (!negativeCountryCodes.empty())
	{
		out.append("-[");
		for (int i = 0; i < negativeCountryCodes.size(); ++i)
		{
			if (i > 0)
			{
				out.append(", ");
			}
			out.append(negativeCountryCodes[i]);
		}
		out.append("]");
	}
	return out.c_str();
}

const char* IntParamRange::toString(PString & out)const
{
	out.assign("(").appendInt(min).append("-").appendInt(max).append(")");
	return out.c_str();
}

CommonRule::CommonRule()
	: orderId(0), ruleId(0), auditId(0), isActive(Rule_INACTIVE), lookbackPeriodMins(0)
	, lastStreetReached(eStreetFlags_NONE), gameType(eColGameTypeFlags_None)
	, gameStructure(eColGameStruct_None), gameFlags(eColGameFlags_NONE)	
	, cdhGameFlags(eColGameFlags_NONE), cdhGameTypes(eColGameTypeFlags_None)
	, cdhGameStructures(eColGameStruct_None), cdhMaxPerTable(eColMaxPerTableFlags_NONE)
	, cdhTournFlags(eColGameTourneyFlags_NONE), cdhDumperLastActions(0)
	, cdhDumperFilters(0), autoActions(0)
	, cdhBankerStartingHandStrength(), cdhDumperStartingHandStrength()
	, cdhBankerFinalHandStrength(), cdhDumperFinalHandStrength()
{
}

void CommonRule::parseLastDepositRules(const vector<CCDADepositRule>& lastDepsitRuleVec, vector<LastDepositParam>& lastDepositParamVec)
{
	for (int i = 0; i < lastDepsitRuleVec.size(); ++i)
	{
		const CCDADepositRule& lastDepositRule = lastDepsitRuleVec[i];

		LastDepositParam lastDepositParam;

		vector<PString> paySystemGateWay;
		readStrVectorFromString(lastDepositRule.cdhLastDepositTypes, paySystemGateWay, '.');

		if (paySystemGateWay.size() != 2)
		{
			PLog("Error: wrong deposit type config. '%s'", lastDepositRule.cdhLastDepositTypes.c_str());
			continue;
		}

		readStrVectorFromString(paySystemGateWay[0], lastDepositParam.paySystemVec, ',');
		readStrVectorFromString(paySystemGateWay[1], lastDepositParam.gateWayVec, ',');

		lastDepositParam.lastDepositHours.init(lastDepositRule.cdhLastDepositHoursMin, lastDepositRule.cdhLastDepositHoursMax);
		lastDepositParam.lastDepositCents.init(lastDepositRule.cdhLastDepositCentsMin, lastDepositRule.cdhLastDepositCentsMax);

		lastDepositParamVec.push_back(lastDepositParam);
	}
}

void CommonRule::parseLicCountryFilter(const char* str, LicenseToCountriesMap& licenseCountryMap)
{
	// license country filter should be validated in CCDARule::isValidLicCountryFilter
	vector<PString> licCountryVec;
	readStrVectorFromString(str, licCountryVec, ';');

	for (const PString& licenseCountryEntry : licCountryVec)
	{
		vector<PString> licCountry;
		readStrVectorFromString(licenseCountryEntry, licCountry, '.');
		if (licCountry.size() != 2)
		{
			PLog("Error: wrong license country str '%s'", licenseCountryEntry.c_str());
			continue;
		}

		vector<PString> licVec;
		vector<PString> countryVec;

		readStrVectorFromString(licCountry[0], licVec, ',');
		readStrVectorFromString(licCountry[1], countryVec, ',');

		if (licVec.empty() || countryVec.empty())
		{
			PLog("Error: wrong license country str '%s'", licenseCountryEntry.c_str());
			continue;
		}

		bool isValidLicCountryFilter = true;
		PLicenseMask licenseMask = PLicenseMask::zero;
		for (const PString& licenseStr : licVec)
		{
			if (!isValidLicCountryFilter)
			{
				break;
			}

			if (licenseStr.equals("*"))
			{
				licenseMask = LicenceBitmask_All;
			}
			else if (licenseStr.startsWith("-"))
			{
				const char* licStr = licenseStr.c_str();
				if (!licStr || !*licStr)
				{
					isValidLicCountryFilter = false;
					break;
				}

				++licStr;

				PLicenseMask negLicenseMask = PLicenseMask::zero;
				if (!ColUtils::licenseNameToMask(licStr, negLicenseMask))
				{
					isValidLicCountryFilter = false;
					break;
				}

				licenseMask.andNot(negLicenseMask);
			}
			else
			{
				PLicenseMask posLicenseMask = PLicenseMask::zero;
				if (!ColUtils::licenseNameToMask(licenseStr, posLicenseMask))
				{
					isValidLicCountryFilter = false;
					break;
				}

				licenseMask |= posLicenseMask;
			}
		}

		CountriesRulePart countriesRulePart;
		for (const PString& country : countryVec)
		{
			if (!isValidLicCountryFilter)
			{
				break;
			}

			if (country.equals("*"))
			{
				countriesRulePart.positiveCountryCodes.push_back("*");
			}
			else if (country.startsWith("-"))
			{
				const char* countryStr = country.c_str() + 1;
				countriesRulePart.negativeCountryCodes.push_back(countryStr);
			}
			else
			{
				countriesRulePart.positiveCountryCodes.push_back(country);
			}
		}

		if (isValidLicCountryFilter)
		{
			licenseCountryMap.insert(make_pair(licenseMask, countriesRulePart));
		}
		else
		{
			PTRACE("Warning: Invalid license/country filter found: %s", licenseCountryEntry.c_str());
		}
	}
}

bool CommonRule::isLicCountryValid(UINT32 licenseId, const char* country, const LicenseToCountriesMap& licenseCountryMap) const
{
	for (const auto& licCountry : licenseCountryMap)
	{
		const PLicenseMask& licenseIdMask = licCountry.first;
		const CountriesRulePart& countriesRulePart = licCountry.second;

		if (LicenseRules::maskContainsLicenseId(licenseId, licenseIdMask))
		{
			if (isStringInVector(country, countriesRulePart.negativeCountryCodes, true))
			{
				continue;
			}
			else if (isStringInVector("*", countriesRulePart.positiveCountryCodes) ||
					 isStringInVector(country, countriesRulePart.positiveCountryCodes, true))
			{
				return true;
			}
		}
	}

	return false;
}

void CommonRule::initCommonRule(const CCDARule& rule)
{
	orderId = rule.orderId;
	ruleId = rule.ruleId;
	auditId = rule.auditId;
	isActive = rule.isActive;
	ruleName = rule.name;

	// generic
	lookbackPeriodMins = rule.lookbackPeriodMins;
	parseLicCountryFilter(rule.bankerLicenseCountry, bankerLicCountries);
	parseLicCountryFilter(rule.dumperLicenseCountry, dumperLicCountries);
	bankerAccountAge.init(rule.bankerAcctAgeDaysMin, rule.bankerAcctAgeDaysMax);
	dumperAccountAge.init(rule.dumperAcctAgeDaysMin, rule.dumperAcctAgeDaysMax);
	bankerMPWCents.init(rule.bankerMPWMinCents, rule.bankerMPWMaxCents);
	dumperMPWCents.init(rule.dumperMPWMinCents, rule.dumperMPWMaxCents);
	bankerTotalProfitCents.init(rule.bankerProfitMin, rule.bankerProfitMax);
	bankerTotalCDHNum.init(rule.bankerTotalCDHMin, rule.bankerTotalCDHMax);
	readStrVectorFromString(rule.currencies, currencies, ',');

	// CDH specific
	cdhDumperLossPct_scaled100.init(rule.cdhDumperLossPCTMin_scaled100, rule.cdhDumperLossPCTMax_scaled100);
	lastStreetReached = rule.cdhLastStreetReached;
	cdhBankerTotalHands.init(rule.cdhBankerTotalHandsMin, rule.cdhBankerTotalHandsMax);

	// CDH specific PYR-84380
	cdhGameFlags = rule.cdhGameFlags;
	cdhGameTypes = rule.cdhGameTypes;
	cdhGameStructures = rule.cdhGameStructures;
	cdhMaxPerTable = rule.cdhMaxPerTable;
	cdhTournFlags = rule.cdhTournFlags;
	cdhBBCents.init(rule.cdhBBCentsMin, rule.cdhBBCentsMax);
	cdhBBWon.init(rule.cdhBBWonMin, rule.cdhBBWonMax);
	cdhPlayersDealtin.init(rule.cdhPlayersDealtinMin, rule.cdhPlayersDealtinMax);
	cdhDumperAccountAge.init(rule.cdhDumperAccountAgeMin, rule.cdhDumperAccountAgeMax);
	cdhDumperLTPokerMPWCents.init(rule.cdhDumperLTPokerMPWCentsMin, rule.cdhDumperLTPokerMPWCentsMax);
	cdhDumperLTFpp.init(rule.cdhDumperLTFppMin, rule.cdhDumperLTFppMax);
	readStrVectorFromString(rule.cdhRuleIds, cdhRuleIds, ',');
	cdhDumperLastActions = rule.cdhDumperLastActions;
	cdhTurnFlopPotRatio_scaled100.init(rule.cdhTurnFlopPotRatioMin_scaled100, rule.cdhTurnFlopPotRatioMax_scaled100);
	cdhRiverTurnPotRatio_scaled100.init(rule.cdhRiverTurnPotRatioMin_scaled100, rule.cdhRiverTurnPotRatioMax_scaled100);
	cdhFinalRiverPotRatio_scaled100.init(rule.cdhFinalRiverPotRatioMin_scaled100, rule.cdhFinalRiverPotRatioMax_scaled100);
	cdhBankerStartingHandStrength.startHandStrengthFlags = rule.cdhBankerStartingHandStrength;
	cdhDumperStartingHandStrength.startHandStrengthFlags = rule.cdhDumperStartingHandStrength;
	cdhNumbBets.init(rule.cdhNumbBetsMin, rule.cdhNumbBetsMax);
	cdhDumperPreFlopActions.init(rule.cdhDumperPreFlopActionsMin, rule.cdhDumperPreFlopActionsMax);
	cdhDumperFlopActions.init(rule.cdhDumperFlopActionsMin, rule.cdhDumperFlopActionsMax);
	cdhDumperTurnActions.init(rule.cdhDumperTurnActionsMin, rule.cdhDumperTurnActionsMax);
	cdhDumperRiverActions.init(rule.cdhDumperRiverActionsMin, rule.cdhDumperRiverActionsMax);
	cdhFlopSizeBBRatio_scaled100.init(rule.cdhFlopSizeBBRatioMin_scaled100, rule.cdhFlopSizeBBRatioMax_scaled100);
	cdhBankerFinalHandStrength.normalHandStrengthFlags  = rule.cdhBankerFinalHandStrengthNormal;
	cdhBankerFinalHandStrength.low8HandStrengthFlags    = rule.cdhBankerFinalHandStrengthLow8;
	cdhBankerFinalHandStrength.low2To7HandStrengthFlags = rule.cdhBankerFinalHandStrength2To7;
	cdhBankerFinalHandStrength.badugiHandStrengthFlags  = rule.cdhBankerFinalHandStrengthBadugi;
	cdhBankerFinalHandStrength.sixPlusHandStrengthFlags = rule.cdhBankerFinalHandStrength6Plus;
	cdhDumperFinalHandStrength.normalHandStrengthFlags  = rule.cdhDumperFinalHandStrengthNormal;
	cdhDumperFinalHandStrength.low8HandStrengthFlags    = rule.cdhDumperFinalHandStrengthLow8;
	cdhDumperFinalHandStrength.low2To7HandStrengthFlags = rule.cdhDumperFinalHandStrength2To7;
	cdhDumperFinalHandStrength.badugiHandStrengthFlags  = rule.cdhDumperFinalHandStrengthBadugi;
	cdhDumperFinalHandStrength.sixPlusHandStrengthFlags = rule.cdhDumperFinalHandStrength6Plus;
	cdhDumperCount.init(rule.cdhDumperCountMin, rule.cdhDumperCountMax);
	cdhDumperFilters = rule.cdhDumperFilters;
	cdhDumperFirstMacDays.init(rule.cdhDumperFirstMacDaysMin, rule.cdhDumperFirstMacDaysMax);
	autoActions = rule.autoActions;
	parseLastDepositRules(rule.bankerLastDepositRules, bankerLastDepositParams);
	parseLastDepositRules(rule.dumperLastDepositRules, dumperLastDepositParams);

	// Game specific
	gameType = rule.gameType;
	gameStructure = rule.gameStructure;
	gameFlags = rule.gameFlags;

	dumperNumCDH.init(rule.dumperNumCDHMin, rule.dumperNumCDHMax);
	dumperCount.init(rule.numDumpersMin, rule.numDumpersMax);
}

void CommonRule::logLicenseToCountriesMap(const LicenseToCountriesMap& licenseToCountriesMap) const
{
	for (const auto& licCountry : licenseToCountriesMap)
	{
		const PLicenseMask& licenseMask = licCountry.first;
		PString hexStr;
		PString countryStr;
		PLog("Lic=0x%s,countries=%s", licenseMask.toDbString(hexStr), licCountry.second.toString(countryStr));
	}
}

void CommonRule::getRuleCurrency(PString& ruleCur) const
{
	if (currencies.size() == 1 && currencies[0].equalsIgnoreCase("INR"))
	{
		ruleCur = "INR";
	}
	else
	{
		ruleCur = "USD";
	}
}

void CommonRule::logCommonRule() const
{
	PString dummy;
	PLog("-----");
	PLog("orderId=%u", orderId); 
	PLog("ruleId=%u", ruleId); 
	PLog("auditId=%u", auditId); 
	PLog("isActive=%u", isActive); 
	PLog("lookbackPeriodMins=%u", lookbackPeriodMins);
	PLog("bankerLicCountries:");
	logLicenseToCountriesMap(bankerLicCountries);
	PLog("dumperLicCountries:");
	logLicenseToCountriesMap(dumperLicCountries);
	PLog("bankerAccountAge=%s", bankerAccountAge.toString(dummy));
	PLog("dumperAccountAge=%s", dumperAccountAge.toString(dummy));
	PLog("bankerMPWCents=%s", bankerMPWCents.toString(dummy));
	PLog("dumperMPWCents=%s", dumperMPWCents.toString(dummy));
	PLog("bankerTotalProfitCents=%s", bankerTotalProfitCents.toString(dummy));
	PLog("bankerTotalCDHNum=%s", bankerTotalCDHNum.toString(dummy));
	PLog("cdhDumperLossPct_scaled100=%s", cdhDumperLossPct_scaled100.toString(dummy));
	PLog("lastStreetReached=%u", lastStreetReached); 
	PLog("cdhBankerTotalHands=%s", cdhBankerTotalHands.toString(dummy));
	PLog("cdhGameFlags=%u", cdhGameFlags);
	PLog("cdhGameTypes=%llu", cdhGameTypes);
	PLog("cdhGameStructures=%u", cdhGameStructures);
	PLog("cdhMaxPerTable=%u", cdhMaxPerTable);
	PLog("cdhTournFlags=%u", cdhTournFlags);
	PLog("cdhBBCents=%s", cdhBBCents.toString(dummy));
	PLog("cdhBBWon=%s", cdhBBWon.toString(dummy));
	PLog("cdhPlayersDealtin=%s", cdhPlayersDealtin.toString(dummy));
	PLog("cdhDumperAccountAge=%s", cdhDumperAccountAge.toString(dummy));
	PLog("cdhDumperLTPokerMPWCents=%s", cdhDumperLTPokerMPWCents.toString(dummy));
	PLog("cdhDumperLTFpp=%s", cdhDumperLTFpp.toString(dummy));
	PLog("cdhRuleIds=%s", strVectorToStr(dummy, cdhRuleIds));
	PLog("cdhDumperLastActions=%d", cdhDumperLastActions);
	PLog("cdhTurnFlopPotRatio_scaled100=%s", cdhTurnFlopPotRatio_scaled100.toString(dummy));
	PLog("cdhRiverTurnPotRatio_scaled100=%s", cdhRiverTurnPotRatio_scaled100.toString(dummy));
	PLog("cdhFinalRiverPotRatio_scaled100=%s", cdhFinalRiverPotRatio_scaled100.toString(dummy));
	PLog("cdhNumbBets=%s", cdhNumbBets.toString(dummy));
	PLog("cdhDumperPreFlopActions=%s", cdhDumperPreFlopActions.toString(dummy));
	PLog("cdhDumperFlopActions=%s", cdhDumperFlopActions.toString(dummy));
	PLog("cdhDumperTurnActions=%s", cdhDumperTurnActions.toString(dummy));
	PLog("cdhDumperRiverActions=%s", cdhDumperRiverActions.toString(dummy));
	PLog("cdhFlopSizeBBRatio_scaled100=%s", cdhFlopSizeBBRatio_scaled100.toString(dummy));
	PLog("cdhDumperCount=%s", cdhDumperCount.toString(dummy));
	PLog("gameFlags=%d", cdhDumperFilters);
	PLog("cdhDumperFirstMacDays=%s", cdhDumperFirstMacDays.toString(dummy));
	PLog("autoActions=%d", autoActions);
	PLog("currencies=%s", strVectorToStr(dummy, currencies));
	PLog("gameType=%llu", gameType); 
	PLog("gameStructure=%u", gameStructure); 
	PLog("gameFlags=%u", gameFlags); 
	PLog("dumperNumCDH=%s", dumperNumCDH.toString(dummy));
	PLog("dumperCount=%s", dumperCount.toString(dummy));
}

/////////////////////////////////////////////////////////////////////////////////

RingGameRule::RingGameRule()
	:CommonRule(), maxSeats(0)
{

}

void RingGameRule::initRule(const CCDARule& rule)
{
	initCommonRule(rule);

	bankerTotalHands.init(rule.bankerTotalHandsMin, rule.bankerTotalHandsMax);

	cdhDumperLoss.init(rule.cdhDumperLossMin, rule.cdhDumperLossMax);
	cdhFinalPotSizeCents.init(rule.cdhFinalPotSizeMin, rule.cdhFinalPotSizeMax);
	cdhFlopPotSizeCents.init(rule.cdhFlopPotSizeMin, rule.cdhFlopPotSizeMax);
	cdhTurnPotSizeCents.init(rule.cdhTurnPotSizeMin, rule.cdhTurnPotSizeMax);
	cdhRiverPotSizeCents.init(rule.cdhRiverPotSizeMin, rule.cdhRiverPotSizeMax);
	cdh7thStreetPotSizeCents.init(rule.cdh7thStreetPotSizeMin, rule.cdh7thStreetPotSizeMax);

	maxSeats = rule.maxSeats;
	bbSize.init(rule.bbSizeMin, rule.bbSizeMax);
	bankerBBWon.init(rule.bankerBBWonMin, rule.bankerBBWonMax);
	bankerBBPer100_scaled100.init(rule.bankerBBper100Min_scaled100, rule.bankerBBper100Max_scaled100);
	avgPlayersDelatin_scaled100.init(rule.avgPlayersDealtinMin_scaled100, rule.avgPlayersDealtinMax_scaled100);

	dumperBBLost.init(rule.dumperBBLostMin, rule.dumperBBLostMax);
	dumperBBper100Lost_scaled100.init(rule.dumperBBper100LostMin_scaled100, rule.dumperBBper100LostMax_scaled100);
	dumperAmountLost.init(rule.dumperAmountLostMin, rule.dumperAmountLostMax);

	allDumpersAmountLost.init(rule.allDumpersAmountLostMin, rule.allDumpersAmountLostMax);
	allDumpersBBper100Lost_scaled100.init(rule.allDumpersBBper100LostMin_scaled100, rule.allDumpersBBper100LostMax_scaled100);

	rgBankerDeltaRatio_scaled100.init(rule.rgBankerDeltaRatioMin_scaled100, rule.rgBankerDeltaRatioMax_scaled100);
	rgBankerDeltaWagerRatio_scaled100.init(rule.rgBankerDeltaWagerRatioMin_scaled100, rule.rgBankerDeltaWagerRatioMax_scaled100);
	rgBankerWagerRatio_scaled100.init(rule.rgBankerWagerRatioMin_scaled100, rule.rgBankerWagerRatioMax_scaled100);
	rgDumperDeltaRatio_scaled100.init(rule.rgDumperDeltaRatioMin_scaled100, rule.rgDumperDeltaRatioMax_scaled100);
	rgDumperWagerRatio_scaled100.init(rule.rgDumperWagerRatioMin_scaled100, rule.rgDumperWagerRatioMax_scaled100);
}

bool RingGameRule::doesHandMatchForRule(const RingHand& ringHand, time_t evalTime) const
{
	if (isActive == Rule_INACTIVE)
	{
		return false;
	}

	if (!bbSize.isInRange(ringHand.blindSzHandCurrencyCents))
	{
		return false;
	}

	if (!(ringHand.gameTypeFlags & gameType))
	{
		return false;
	}

	if (!(ringHand.structureFlags & gameStructure))
	{
		return false;
	}

	if (!(ringHand.maxPerTableFlags & maxSeats))
	{
		return false;
	}

	if (!(ringHand.gameFlags & gameFlags))
	{
		return false;
	}

	if (!isStringInVector(ringHand.currency, currencies, true))
	{
		return false;
	}

	// skip old ring hand for the rule
	time_t lookbackTime = evalTime - lookbackPeriodMins * SECONDS_IN_A_MINUTE;
	if (ringHand.finishTime < lookbackTime)
	{
		return false;
	}

	return true;
}

void RingGameRule::logRule() const
{
	PString dummy;
	logCommonRule();

	PLog("bankerTotalHands=%s", bankerTotalHands.toString(dummy));

	PLog("cdhFinalPotSizeCents=%s", cdhFinalPotSizeCents.toString(dummy));
	PLog("cdhFlopPotSizeCents=%s", cdhFlopPotSizeCents.toString(dummy));
	PLog("cdhTurnPotSizeCents=%s", cdhTurnPotSizeCents.toString(dummy));
	PLog("cdhRiverPotSizeCents=%s", cdhRiverPotSizeCents.toString(dummy));
	PLog("cdh7thStreetPotSizeCents=%s", cdh7thStreetPotSizeCents.toString(dummy));

	PLog("maxSeats=%u", maxSeats); 
	PLog("bbSize=%s", bbSize.toString(dummy));
	PLog("bankerBBWon=%s", bankerBBWon.toString(dummy));
	PLog("bankerBBPer100_scaled100=%s", bankerBBPer100_scaled100.toString(dummy));
	PLog("avgPlayersDelatin_scaled100=%s", avgPlayersDelatin_scaled100.toString(dummy));

	PLog("dumperBBLost=%s", dumperBBLost.toString(dummy));
	PLog("dumperBBper100Lost_scaled100=%s", dumperBBper100Lost_scaled100.toString(dummy));
	PLog("dumperAmountLost=%s", dumperAmountLost.toString(dummy));

	PLog("allDumpersAmountLost=%s", allDumpersAmountLost.toString(dummy));
	PLog("allDumpersBBper100Lost_scaled100=%s", allDumpersBBper100Lost_scaled100.toString(dummy));

	PLog("rgBankerDeltaRatio_scaled100=%s", rgBankerDeltaRatio_scaled100.toString(dummy));
	PLog("rgBankerDeltaWagerRatio_scaled100=%s", rgBankerDeltaWagerRatio_scaled100.toString(dummy));
	PLog("rgBankerWagerRatio_scaled100=%s", rgBankerWagerRatio_scaled100.toString(dummy));
	PLog("rgDumperDeltaRatio_scaled100=%s", rgDumperDeltaRatio_scaled100.toString(dummy));
	PLog("rgDumperWagerRatio_scaled100=%s", rgDumperWagerRatio_scaled100.toString(dummy));
}


/////////////////////////////////////////////////////////////////////////////////
TournRule::TournRule()
	:CommonRule(), tournFlags(eColGameTourneyFlags_NONE), tournSpeed(eColGameSpeed_None)
{

}

void TournRule::initRule( const CCDARule& rule)
{
	initCommonRule(rule);

	bankerTotalBuyin.init(rule.bankerTotalBuyinMin, rule.bankerTotalBuyinMax);

	tournFlags = rule.tournFlags;
	tournSpeed = rule.tournSpeed;
	tournBuyInStake.init(rule.tournBuyinStakeMin, rule.tournBuyinStakeMax);
	tournNum.init(rule.tournNumMin, rule.tournNumMax);
	tournWinRate_scaled100.init(rule.tournWinrateMin_scaled100, rule.tournWinrateMax_scaled100);
	tournROI_scaled100.init(rule.tournROIMin_scaled100, rule.tournROIMax_scaled100);
	tournAveHands_scaled100.init(rule.tournAveHandsMin_scaled100, rule.tournAveHandsMax_scaled100);

	dumperWinrate_scaled100.init(rule.dumperWinrateMin_scaled100, rule.dumperWinrateMax_scaled100);
	dumperTournROI_scaled100.init(rule.dumperTournROIMin_scaled100, rule.dumperTournROIMax_scaled100);

	allDumpersWinrate_scaled100.init(rule.allDumpersWinrateMin_scaled100, rule.allDumpersWinrateMax_scaled100);
	allDumpersROI_scaled100.init(rule.allDumpersTournROIMin_scaled00, rule.allDumpersTournROIMax_scaled00);

}

void TournRule::logRule() const
{
	PString dummy;
	logCommonRule();

	PLog("bankerTotalBuyin=%s", bankerTotalBuyin.toString(dummy));

	PLog("tournFlags=%u", tournFlags); 
	PLog("tournSpeed=%u", tournSpeed); 
	PLog("tournBuyInStake=%s", tournBuyInStake.toString(dummy));
	PLog("tournNum=%s", tournNum.toString(dummy));
	PLog("tournWinRate_scaled100=%s", tournWinRate_scaled100.toString(dummy));
	PLog("tournROI_scaled100=%s", tournROI_scaled100.toString(dummy));
	PLog("tournAveHands_scaled100=%s", tournAveHands_scaled100.toString(dummy));

	PLog("dumperWinrate_scaled100=%s", dumperWinrate_scaled100.toString(dummy));
	PLog("dumperTournROI_scaled100=%s", dumperTournROI_scaled100.toString(dummy));

	PLog("allDumpersWinrate_scaled100=%s", allDumpersWinrate_scaled100.toString(dummy));
	PLog("allDumpersROI_scaled100=%s", allDumpersROI_scaled100.toString(dummy));
}

bool TournRule::doesTournMatch(const TournData& tournData, time_t evalTime) const
{
	if (isActive == Rule_INACTIVE)
	{
		return false;
	}

	if (!(tournData.gameTypeFlags & gameType))
	{
		return false;
	}

	if (!(tournData.structureFlags & gameStructure))
	{
		return false;
	}

	if (!(tournData.gameFlags & gameFlags))
	{
		return false;
	}

	if (!(tournData.tournFlags & tournFlags))
	{
		return false;
	}

	if (!(tournData.tournSpeedFlags & tournSpeed))
	{
		return false;
	}

	if (!tournBuyInStake.isInRange(tournData.buyIn_RuleCurrency))
	{
		return false;
	}

	if (!isStringInVector(tournData.currency, currencies, true))
	{
		return false;
	}

	// skip old tourn for the rule
	time_t lookbackTime = evalTime - lookbackPeriodMins * SECONDS_IN_A_MINUTE;
	if ((tournData.finished < lookbackTime) ||
		(tournData.finished > evalTime))
	{
		return false;
	}

	return true;
}
////////////////////////////////////////////////////////////////////////////////
