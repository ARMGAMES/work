#ifndef ColCdaRules_h_included
#define ColCdaRules_h_included

////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "ppinifile.h"
#include "licensemask.h"
#include "ColossusConstants.h"
#include "ColCda.h"
#include "../ColossusSHD/ColShdHandStrengthFilters.h"

////////////////////////////////////////////////////////////////////////////////

using namespace ColShd;

namespace ColCDA
{
	struct RingHand;
	struct TournData;

	struct IntParamRange
	{
		INT32 min;
		INT32 max;

		IntParamRange()
		{
			min = 0;
			max = 0;
		}

		IntParamRange(INT32 min_, INT32 max_) : min(min_), max(max_)
		{}

		void init(INT32 min_, INT32 max_)
		{
			min = min_;
			max = max_;
		}

		bool isInRange(INT32 value) const
		{
			if (min == CCDA_INVALID_RULE_VALUE)
			{
				return true;
			}
			return value >= min && value <= max;
		}

		bool isInRange(UINT32 value) const
		{
			if (min == CCDA_INVALID_RULE_VALUE)
			{
				return true;
			}
			return (INT32)value >= min && (INT32)value <= max;
		}

		bool isInRange(INT64 value) const
		{
			if (min == CCDA_INVALID_RULE_VALUE)
			{
				return true;
			}
			return value >= (UINT64)min && value <= (UINT64)max;
		}

		bool isEnabled() const
		{
			return min != CCDA_INVALID_RULE_VALUE;
		}

		const char* toString(PString & out) const;
	};

	struct LastDepositParam
	{
		vector<PString> paySystemVec;
		vector<PString> gateWayVec;
		IntParamRange lastDepositHours;
		IntParamRange lastDepositCents;

	};

	struct CountriesRulePart
	{
		vector<PString> positiveCountryCodes;
		vector<PString> negativeCountryCodes;

		const char* toString(PString & out)const;
	};

	typedef map<PLicenseMask, CountriesRulePart> LicenseToCountriesMap;

	struct CommonRule
	{
		UINT32 orderId;  // key
		UINT32 ruleId;
		UINT32 auditId;
		UINT16 isActive;
		PString ruleName;

		// generic
		UINT32 lookbackPeriodMins; 
		LicenseToCountriesMap bankerLicCountries;
		LicenseToCountriesMap dumperLicCountries;
		IntParamRange bankerAccountAge;
		IntParamRange dumperAccountAge;
		IntParamRange bankerMPWCents;
		IntParamRange dumperMPWCents;
		IntParamRange bankerTotalProfitCents;
		IntParamRange bankerTotalCDHNum;
		vector<PString> currencies;

		// CDH specific
		IntParamRange cdhDumperLossPct_scaled100;
		UINT32 lastStreetReached;
		IntParamRange cdhBankerTotalHands;

		// CDH specific PYR-84380
		UINT32  cdhGameFlags;
		UINT64  cdhGameTypes;
		UINT32  cdhGameStructures;
		UINT32  cdhMaxPerTable;
		UINT32  cdhTournFlags;
		IntParamRange cdhBBCents;
		IntParamRange cdhBBWon;
		IntParamRange cdhPlayersDealtin;
		IntParamRange cdhDumperAccountAge;
		IntParamRange cdhDumperLTPokerMPWCents;
		IntParamRange cdhDumperLTFpp;
		vector<PString> cdhRuleIds;
		INT32   cdhDumperLastActions;
		IntParamRange cdhTurnFlopPotRatio_scaled100;
		IntParamRange cdhRiverTurnPotRatio_scaled100;
		IntParamRange cdhFinalRiverPotRatio_scaled100;
		StartHandStrengthFilters  cdhBankerStartingHandStrength;
		StartHandStrengthFilters  cdhDumperStartingHandStrength;
		IntParamRange cdhNumbBets;
		IntParamRange cdhDumperPreFlopActions;
		IntParamRange cdhDumperFlopActions;
		IntParamRange cdhDumperTurnActions;
		IntParamRange cdhDumperRiverActions;
		IntParamRange cdhFlopSizeBBRatio_scaled100;
		HandStrengthFilters cdhBankerFinalHandStrength;
		HandStrengthFilters cdhDumperFinalHandStrength;
		IntParamRange cdhDumperCount;
		INT32   cdhDumperFilters; 
		IntParamRange cdhDumperFirstMacDays;
		INT32   autoActions;
		vector<LastDepositParam> bankerLastDepositParams;
		vector<LastDepositParam> dumperLastDepositParams;

		// Game specific
		UINT64 gameType;
		UINT32 gameStructure;
		UINT32 gameFlags;
		
		// Dumper criteria
		IntParamRange dumperNumCDH;
		
		// Dumper agg
		IntParamRange dumperCount; 

		void parseLastDepositRules(const vector<CCDADepositRule>& lastDepsitRuleVec, vector<LastDepositParam>& lastDepositParamVec);
		void parseLicCountryFilter(const char* str, LicenseToCountriesMap& licenseCountryMap);
		bool isLicCountryValid(UINT32 licenseId, const char* country, 
							   const LicenseToCountriesMap& licenseCountryMap) const;

		CommonRule();
		void initCommonRule( const CCDARule& rule);
		void logCommonRule() const;
		void logLicenseToCountriesMap(const LicenseToCountriesMap& licenseToCountriesMap) const;
		void getRuleCurrency(PString& ruleCur) const;
	};

	struct RingGameRule : public CommonRule
	{
		IntParamRange bankerTotalHands;

		// CDH
		IntParamRange cdhDumperLoss;
		IntParamRange cdhFinalPotSizeCents;
		IntParamRange cdhFlopPotSizeCents;
		IntParamRange cdhTurnPotSizeCents;
		IntParamRange cdhRiverPotSizeCents;
		IntParamRange cdh7thStreetPotSizeCents;

		// Game specific
		UINT32 maxSeats;
		IntParamRange bbSize;
		IntParamRange bankerBBWon;
		IntParamRange bankerBBPer100_scaled100;
		IntParamRange avgPlayersDelatin_scaled100;

		// Dumper criteria
		IntParamRange dumperBBLost;
		IntParamRange dumperBBper100Lost_scaled100;
		IntParamRange dumperAmountLost;

		// Dumper agg
		IntParamRange allDumpersAmountLost;
		IntParamRange allDumpersBBper100Lost_scaled100;

		// GPSI-285
		IntParamRange rgBankerDeltaRatio_scaled100;
		IntParamRange rgBankerDeltaWagerRatio_scaled100;
		IntParamRange rgBankerWagerRatio_scaled100;
		IntParamRange rgDumperDeltaRatio_scaled100;
		IntParamRange rgDumperWagerRatio_scaled100;

		RingGameRule();
		void initRule( const CCDARule& rule);
		bool doesHandMatchForRule(const RingHand& ringHand, time_t evalTime) const;
		void logRule() const;
		bool operator<(const RingGameRule& otherRule) const { return orderId < otherRule.orderId;}
	};
	
	struct TournRule : public CommonRule
	{
		IntParamRange bankerTotalBuyin;

		// Game specific
		UINT32 tournFlags;
		UINT32 tournSpeed;
		IntParamRange tournBuyInStake;
		IntParamRange tournNum;
		IntParamRange tournWinRate_scaled100;
		IntParamRange tournROI_scaled100;
		IntParamRange tournAveHands_scaled100;

		// Dumper criteria
		IntParamRange dumperWinrate_scaled100;
		IntParamRange dumperTournROI_scaled100;

		// Dumper agg
		IntParamRange allDumpersWinrate_scaled100;
		IntParamRange allDumpersROI_scaled100;

		TournRule();
		void initRule( const CCDARule& rule);
		bool doesTournMatch(const TournData& tournData, time_t evalTime) const;
		void logRule() const;
		bool operator<(const TournRule& otherRule) const { return orderId < otherRule.orderId;}
	};

	template <class T>
	class RuleContainer
	{
	public:
		typedef set<T> Container;
		typedef typename set<T>::iterator Iterator;
		Iterator begin() {return rules.begin();}
		Iterator end() {return rules.end();}

		void addRule(const CCDARule& rule)
		{
			removeRule(rule.ruleId);

			T newRule;
			newRule.initRule(rule);
			rules.insert(newRule);

			newRule.logRule();
		}

		void removeRule(Iterator it)
		{
			rules.erase(it);
		}

		void removeRule(UINT32 ruleId)
		{
			for (Container::iterator it = rules.begin(); it != rules.end(); ++it)
			{
				if (it->ruleId == ruleId)
				{
					rules.erase(it);
					return;
				}
			}
		}

		void deleteRule (const CCDARule& rule)
		{
			Container newRules;

			for (Container::iterator it = rules.begin(); it != rules.end(); ++it)
			{
				if (it->ruleId == rule.ruleId)
				{
					continue;
				}
				T newRule = *it;
				if (newRule.orderId > rule.orderId)
				{
					--newRule.orderId;
				}
				newRules.insert(newRule);
			}
			rules.swap(newRules);
		}

		void reorderRules(const vector<CCDARule>& ccdaRuleVec)
		{
			Container::iterator it = rules.begin();
			while(it != rules.end())
			{
				Container::iterator removeIt = it++;
				for (int i = 0; i < ccdaRuleVec.size(); ++i)
				{
					if (removeIt->ruleId == ccdaRuleVec[i].ruleId)
					{
						rules.erase(removeIt);
						break;
					}
				}
			}

			for (int i = 0; i < ccdaRuleVec.size(); ++i)
			{
				T newRule;
				newRule.initRule(ccdaRuleVec[i]);
				rules.insert(newRule);

				newRule.logRule();
			}
		}

		void clear()
		{
			rules.clear();
		}

		UINT32 size() const
		{
			return rules.size();
		}

	private:
		Container rules;
	};

}

////////////////////////////////////////////////////////////////////////////////

#endif // ColCdaRules_h_included
