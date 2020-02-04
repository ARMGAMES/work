////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////

#include "ColCdaCacheTypes.h"
#include "shared.h"
#include "../utils.h"
#include "../ColossusStructures.h"
#include "ColossusHand.h"
#include "../shared/ColossusConstantsInfo.h"

using namespace ColCDA;
using namespace ColShd;

/////////////////////////////////////////////////////////////////////////////////////////////

CCDAAlert::CCDAAlert()
{
	autoActions = 0;
}

CCDAAlert::CCDAAlert(const char* bankerUserId_, const char* dumperUserId_, INT32 autoActions_)
{
	bankerUserId = bankerUserId_;
	dumperUserId = dumperUserId_;
	autoActions = autoActions_;
}

CDHAlertInfo::CDHAlertInfo(const CdhEvent& cdhEvent)
{
	when = cdhEvent.when;
	dumperUserId = cdhEvent.dumperUserId;
	bankerChipsEndingCents = cdhEvent.bankerChipsEndingCents;
}

void CDHAlertInfo::toString(UINT64 handId, PString& cdhAlertString) const
{
	PString dummy1, dummy2;

	cdhAlertString
		.assign("HandId:").appendUint64(handId)
		.append("  Time:").append(srvTimeToString(when, dummy1))
		.append("  Partner:").append(dumperUserId)
		.append("  Final Balance Banker:$")
		.append(centsToString(dummy2, bankerChipsEndingCents));
}

CCDAAlertData::CCDAAlertData()
{
	autoActions = 0;
}

void CCDAAlertData::appendRuleInfo(UINT32 ruleId, const char* ruleDescription)
{
	parameter.append("RuleId:").appendUint(ruleId).append("-").append(ruleDescription).append(";");
}

void CCDAAlertData::removeCDHDumper(const char* dumperId)
{
	auto cdhIt = cdhMap.begin();
	while (cdhIt != cdhMap.end())
	{
		auto removeIt = cdhIt++;
		if (removeIt->second.dumperUserId.equals(dumperId))
		{
			cdhMap.erase(removeIt);
		}
	}

	relatedPlayerMap.erase(dumperId);
}

////////////////////////////////////////////////////////////////////////////////

AggDumperRingData::AggDumperRingData()
	: totalLostToBankerCents(0)
	, bbLostToBankerScaled100(0)
	, totalHandsWithBanker(0)
	, cdhHandNum(0)
	, matachedWagerCents_USD(0)
	, bankerDeltaCents_USD(0)
	, totalDeltaCents_USD(0)
	, totalWagerCents_USD(0)
{

}

INT32 AggDumperRingData::getBBPer100LostToBankerScaled100() const
{
	if (totalHandsWithBanker == 0)
	{
		return 0;
	}

	double bbLostToBankerScaled100_ = bbLostToBankerScaled100;
	double totalHandsWithBanker_ = totalHandsWithBanker;

	double bbLostToBankerPer100HandsScaled100 = 100.0 * bbLostToBankerScaled100_ / totalHandsWithBanker_;
	return ColUtils::round<INT32>(bbLostToBankerPer100HandsScaled100);
}

INT32 AggDumperRingData::getBBLostToBanker() const
{
	double bbLostToBankerScaled100_ = bbLostToBankerScaled100;

	double bbLostToBanker = bbLostToBankerScaled100_ / 100.0;
	return ColUtils::round<INT32>(bbLostToBanker);
}

INT32 AggDumperRingData::getNetAmountLostToBanker() const
{
	return totalLostToBankerCents;
}

void AggDumperRingData::aggregateHand(INT32 amount, UINT32 bbSize, ColGameStructFlags structureFlags)
{
	totalLostToBankerCents += amount;
	
	if (bbSize != 0)
	{
		double amount_ = amount;
		double bbSize_ = bbSize;

		INT32 bbLostToBankerScaled100_ = 0;
		if (structureFlags == eColGameStruct_Limit)
		{
			bbLostToBankerScaled100_ = ColUtils::round<INT32>(100.0 * amount_ / (2 * bbSize_));
		}
		else
		{
			bbLostToBankerScaled100_ = ColUtils::round<INT32>(100.0 * amount_ / bbSize_);
		}

		bbLostToBankerScaled100 += bbLostToBankerScaled100_;
	}
	++totalHandsWithBanker;
}

void AggDumperRingData::aggDeltaWager(INT32 matchedWager, INT32 delta)
{
	matachedWagerCents_USD += matchedWager;
	bankerDeltaCents_USD += delta;
}

////////////////////////////////////////////////////////////////////////////////

AggBankerRingData::AggBankerRingData()
	: bbWonScaled100(0)
	, totalHands(0)
	, totalPlayersDealtIn(0)
	, cdhHandNum(0)
	, totalDeltaCents_USD(0)
	, totalWagerCents_USD(0)
{
}

INT32 AggBankerRingData::getBBWon() const
{
	double bbWonScaled100_ = bbWonScaled100;

	return ColUtils::round<INT32>(bbWonScaled100_ / 100.0);
}

INT32 AggBankerRingData::getBBWonPer100HandsScaled100() const
{
	if (totalHands == 0)
	{
		return 0;
	}

	double bbWonScaled100_ = bbWonScaled100;
	double totalHands_ = totalHands;

	double bankerBBWonPer100HandsScaled100 = 100.0 * bbWonScaled100_ / totalHands_;
	return ColUtils::round<INT32>(bankerBBWonPer100HandsScaled100);
}

UINT32 AggBankerRingData::getAvgPlayersDealtInScaled100() const
{
	if (totalHands == 0)
	{
		return 0;
	}

	double totalPlayersDealtIn_ = totalPlayersDealtIn;
	double totalHands_ = totalHands;

	double avgPlayersDealtInScaled100 = 100.0 * totalPlayersDealtIn_ / totalHands_;
	return ColUtils::round<UINT32>(avgPlayersDealtInScaled100);
}

UINT32 AggBankerRingData::getNumDumpers() const
{
	return aggDumperData.size();
}

INT64 AggBankerRingData::getAllDumpersLostToMeCents() const
{
	INT64 totalCents = 0;

	for (const auto& dumperData : aggDumperData)
	{
		const AggDumperRingData& dumper = dumperData.second;

		totalCents += dumper.totalLostToBankerCents;
	}

	return totalCents;
}

INT32 AggBankerRingData::getAllDumpersLostToMeBBPer100Scaled100() const
{
	double totalBBScaled100_ = 0;
	double totalHands_ = 0;

	for (const auto& dumperData : aggDumperData)
	{
		const AggDumperRingData& dumper = dumperData.second;

		totalBBScaled100_ += dumper.bbLostToBankerScaled100;
		totalHands_ += dumper.totalHandsWithBanker;
	}

	if (totalHands_ == 0.0)
	{
		return 0;
	}

	double bbPer100Scaled100_ = 100.0 * totalBBScaled100_ / totalHands_;

	return ColUtils::round<INT32>(bbPer100Scaled100_);
}

void AggBankerRingData::clear()
{
	bbWonScaled100 = 0;
	totalHands = 0;
	totalPlayersDealtIn = 0;
	cdhHandNum = 0;
	totalDeltaCents_USD = 0;
	totalWagerCents_USD = 0;
	aggDumperData.clear();
}

void AggBankerRingData::aggregateHand(const PString& bankerName, const RingHand& hand)
{
	const vector<UserInRingHand>& playersVec = hand.playersVec;

	INT32 bankerDeltaCents_USD = 0;
	INT32 bankerPotChipsCents_USD = 0;

	for (const auto& uih : playersVec)
	{
		if (uih.userId.equals(bankerName))
		{
			bankerDeltaCents_USD = uih.deltaCents_USD;
			bankerPotChipsCents_USD = uih.potChips_USD;
			break;
		}
	}

	for (int i = 0; i < playersVec.size(); ++i)
	{
		const auto& uih = playersVec[i];
		if (!uih.userId.equals(bankerName))
		{
			// aggregate matched wager and delta
			const PString& dumperName = uih.userId;
			DumperDataMapIt dumperIt = ensureDumper(dumperName);

			dumperIt->second.aggDeltaWager(min(uih.potChips_USD, bankerPotChipsCents_USD), bankerDeltaCents_USD);
			continue;
		}

		// aggregate all qualified ring hands for banker

		totalDeltaCents_USD += uih.deltaCents_USD;
		totalWagerCents_USD += uih.potChips_USD;

		if (hand.blindSzCents_USD != 0)
		{
			double deltaCents_USD_ = uih.deltaCents_USD;
			double blindSzCents_USD_ = hand.blindSzCents_USD;
			INT32 bbWonScaled100_ = 0;
			if (hand.structureFlags == eColGameStruct_Limit)
			{
				bbWonScaled100_ = ColUtils::round<INT32>(100.0 * deltaCents_USD_ / (2 * blindSzCents_USD_));
			}
			else
			{
				bbWonScaled100_ = ColUtils::round<INT32>(100.0 * deltaCents_USD_ / blindSzCents_USD_);
			}
			
			bbWonScaled100 += bbWonScaled100_;
		}

		++totalHands;
		totalPlayersDealtIn += hand.numPlayersDealtIn;

		// aggregate dumper data
		if (hand.winLossMatrix_USD.size() != playersVec.size())
		{
			PLog(LOG_TRACE_FAULTY_LOGIC " - invalid winlossmatrix size=%u", hand.winLossMatrix_USD.size());
			continue;
		}

		const WinLossRow& winLossRow = hand.winLossMatrix_USD[i];
		if (winLossRow.size() != playersVec.size())
		{
			PLog(LOG_TRACE_FAULTY_LOGIC " - invalid winLossRow size=%u", winLossRow.size());
			continue;
		}

		for (int j = 0; j < winLossRow.size(); ++j)
		{
			// we only aggregate hands that current user is banker, and partner is dumper
			// or current user is dumper, or partner is banker
			if ((uih.isBanker() && winLossRow[j] > 0) ||
				(!uih.isBanker() && winLossRow[j] < 0))
			{
				const PString& dumperName = playersVec[j].userId;
				DumperDataMapIt dumperIt = ensureDumper(dumperName);

				dumperIt->second.aggregateHand(winLossRow[j], hand.blindSzCents_USD, hand.structureFlags);
			}
		}
	}
}

AggBankerRingData::DumperDataMapIt AggBankerRingData::ensureDumper(const PString& dumperName)
{
	DumperDataMapIt dumperIt = aggDumperData.find(dumperName);
	if (dumperIt == aggDumperData.end())
	{
		static const AggDumperRingData empty;
		dumperIt = aggDumperData.insert(make_pair(dumperName, empty)).first;
	}

	return dumperIt;
}

void AggBankerRingData::evalDumperRingHands(const RingGameRule& ringRule, bool moreTrace)
{
	// remove not qualified dumpers
	if (moreTrace)
	{
		PLog("evalDumperRingHands total dumper#=%u", (UINT32)aggDumperData.size());
	}
	DumperDataMapIt dumperIt = aggDumperData.begin();
	while (dumperIt != aggDumperData.end())
	{
		const AggDumperRingData& dumperData = dumperIt->second;

		DumperDataMapIt removeIt =  dumperIt++;

		if (moreTrace)
		{
			PLog("DumperName=%s", removeIt->first);
		}
		if (!ringRule.dumperBBLost.isInRange(dumperData.getBBLostToBanker()))
		{
			if (moreTrace)
			{
				PLog("-RG dumper=%s, bblost=%d", removeIt->first, dumperData.getBBLostToBanker());
			}
			aggDumperData.erase(removeIt);
			continue;
		}

		if (!ringRule.dumperBBper100Lost_scaled100.isInRange(dumperData.getBBPer100LostToBankerScaled100()))
		{
			if (moreTrace)
			{
				PLog("-RG dumper=%s, BBper100Lost=%d", removeIt->first, dumperData.getBBPer100LostToBankerScaled100());
			}
			aggDumperData.erase(removeIt);
			continue;
		}

		if (!ringRule.dumperAmountLost.isInRange(dumperData.getNetAmountLostToBanker()))
		{
			if (moreTrace)
			{
				PLog("-RG dumper=%s, amount lost=%d", removeIt->first, dumperData.getNetAmountLostToBanker());
			}
			aggDumperData.erase(removeIt);
			continue;
		}

		if (!ringRule.dumperNumCDH.isInRange(dumperData.cdhHandNum))
		{
			if (moreTrace)
			{
				PLog("-RG dumper=%s, num cdh=%u", removeIt->first, dumperData.cdhHandNum);
			}

			aggDumperData.erase(removeIt);
			continue;
		}

		if (moreTrace)
		{
			PLog("dumper delta with banker=%d", dumperData.bankerDeltaCents_USD);
			PLog("dumper matched wager=%d", dumperData.matachedWagerCents_USD);
		}

		// GPSI-285 num 3
		INT32 bankerDeltaRatio_scaled100 = 0;
		if (totalDeltaCents_USD)
		{
			bankerDeltaRatio_scaled100 = 100 * 100 * dumperData.bankerDeltaCents_USD / totalDeltaCents_USD;
		}
		if (!ringRule.rgBankerDeltaRatio_scaled100.isInRange(bankerDeltaRatio_scaled100))
		{
			if (moreTrace)
			{
				PLog("-RG dumper=%s, banker delta ratio=%i", removeIt->first, bankerDeltaRatio_scaled100);
			}

			aggDumperData.erase(removeIt);
			continue;
		}

		// GPSI-285 num 4
		INT32 bankerDeltaWagerRatio_scaled100 = 0;
		if (totalWagerCents_USD)
		{
			bankerDeltaWagerRatio_scaled100 = 100 * 100 * dumperData.bankerDeltaCents_USD / totalWagerCents_USD;
		}
		if (!ringRule.rgBankerDeltaWagerRatio_scaled100.isInRange(bankerDeltaWagerRatio_scaled100))
		{
			if (moreTrace)
			{
				PLog("-RG dumper=%s, banker delta wager ratio=%i", removeIt->first, bankerDeltaWagerRatio_scaled100);
			}

			aggDumperData.erase(removeIt);
			continue;
		}

		// GPSI-285 num 6
		INT32 bankerWagerRatio_scaled100 = 0;
		if (totalWagerCents_USD)
		{
			bankerWagerRatio_scaled100 = 100 * 100 * dumperData.matachedWagerCents_USD / totalWagerCents_USD;
		}
		if (!ringRule.rgBankerWagerRatio_scaled100.isInRange(bankerWagerRatio_scaled100))
		{
			if (moreTrace)
			{
				PLog("-RG dumper=%s, banker wager ratio=%i", removeIt->first, bankerWagerRatio_scaled100);
			}

			aggDumperData.erase(removeIt);
			continue;
		}
	}
}

void AggBankerRingData::evalDumperTotalData(const RingGameRule& ringRule, bool moreTrace)
{
	// remove not qualified dumpers
	if (moreTrace)
	{
		PLog("evalDumperTotalData total dumper#=%u", (UINT32)aggDumperData.size());
	}
	DumperDataMapIt dumperIt = aggDumperData.begin();
	while (dumperIt != aggDumperData.end())
	{
		DumperDataMapIt removeIt = dumperIt++;
		const AggDumperRingData& dumperData = removeIt->second;

		// GPSI-285 num 5
		INT32 dumperDetlaRatio_scaled100 = 0;
		if (dumperData.totalDeltaCents_USD)
		{
			dumperDetlaRatio_scaled100 = 100 * 100 * dumperData.bankerDeltaCents_USD / dumperData.totalDeltaCents_USD;
		}
		if (!ringRule.rgDumperDeltaRatio_scaled100.isInRange(dumperDetlaRatio_scaled100))
		{
			if (moreTrace)
			{
				PLog("-RG dumper=%s, dumper delta ratio=%i", removeIt->first, dumperDetlaRatio_scaled100);
			}

			aggDumperData.erase(removeIt);
			continue;
		}

		// GPSI-285 num 7
		INT32 dumperWagerRatio_scaled100 = 0;
		if (dumperData.totalWagerCents_USD)
		{
			dumperWagerRatio_scaled100 = 100 * 100 * dumperData.matachedWagerCents_USD / dumperData.totalWagerCents_USD;
		}
		if (!ringRule.rgDumperWagerRatio_scaled100.isInRange(dumperWagerRatio_scaled100))
		{
			if (moreTrace)
			{
				PLog("-RG dumper=%s, dumper wager ratio=%i", removeIt->first, dumperWagerRatio_scaled100);
			}

			aggDumperData.erase(removeIt);
			continue;
		}
	}
}

void AggBankerRingData::addRingGameCDHHandCount(const CdhEvent& cdhEvent)
{
	++cdhHandNum;

	AggBankerRingData::DumperDataMapIt dumperIt = aggDumperData.find(cdhEvent.dumperUserId);
	if (dumperIt == aggDumperData.end())
	{
		AggDumperRingData aggDumperRingData;
		dumperIt = aggDumperData.insert(make_pair(cdhEvent.dumperUserId, aggDumperRingData)).first;
	}

	++(dumperIt->second.cdhHandNum);
}

////////////////////////////////////////////////////////////////////////////////

void RingHandCache::add(const time_t& threshOldestTime, UINT64 handId, const RingHand& hand)
{
	if (hand.finishTime< threshOldestTime)
	{
		SrvTime threshOldestTime_(threshOldestTime);
		SrvTime handFinishTime(hand.finishTime);
		PString threshOldestTimeStr, handFinishTimeStr;
		srvTimeToString(threshOldestTime_, threshOldestTimeStr);
		srvTimeToString(handFinishTime, handFinishTimeStr);
		PLog("Warning: attempt to add handId: %llu too old (threshOldestTime: %s, hand.finishTime: %s)",
			handId, threshOldestTimeStr.c_str(), handFinishTimeStr.c_str());
		return;
	}
	else
	{
		handsCacheMap.insert(make_pair(handId, hand));

		const vector<UserInRingHand>& playersVec = hand.playersVec;
		for (int i = 0; i < playersVec.size(); ++i)
		{
			addToPlayerAuxCache(handId, playersVec[i].userId);
		}

		PLog("+rhc %llu", handId);
	}
}

void RingHandCache::cull(time_t oldestAllowed, UINT32 maxNumHandsAllowed)
{
	UINT32 numRemoved = 0;
	UINT32 memUsed = 0;
	INT32 minNumHandsToRemove = 0;

	if (handsCacheMap.size() > maxNumHandsAllowed)
	{
		minNumHandsToRemove = handsCacheMap.size() - maxNumHandsAllowed;
	}

	ContainerMap::iterator handCacheIt = handsCacheMap.begin();
	while (handCacheIt != handsCacheMap.end())
	{
		UINT64 handId = handCacheIt->first;
		RingHand& hand = handCacheIt->second;
		ContainerMap::iterator eraseIt = handCacheIt++;
		
		if (hand.finishTime < oldestAllowed || numRemoved < minNumHandsToRemove)
		{
			erasePlayersFromAuxCache(handId, hand.playersVec);
			handsCacheMap.erase(eraseIt);
			++numRemoved;

			PLog("-rhc %llu", handId);
		}
		else
		{
			memUsed += hand.memSize();
		}
	}


	PLog("RHC::cull - new size [%u], numRemoved: [%u] , memUsed: %uKB, player aux size[%u]",
		(UINT32)handsCacheMap.size(), numRemoved, memUsed / 1000, (UINT32)playerAuxCacheMap.size());
}

bool RingHandCache::isRingRuleQualified(const PString& bankerName, 
										const RingGameRule& ringRule,  
										RingRuleData& ringRuleData,
										bool moreTrace)
{
	if (moreTrace)
	{
		PLog("RingHandCache::isRingRuleQualified uid=%s ruleid=%u", bankerName.c_str(), ringRule.ruleId);
	}

	aggregateForRule(bankerName, ringRule, ringRuleData, moreTrace);

	AggBankerRingData& aggBankerRingData = ringRuleData.aggBankerRingData;
	if (moreTrace)
	{
		PLog("banker total delta=%d", aggBankerRingData.totalDeltaCents_USD);
		PLog("banker total wager=%d", aggBankerRingData.totalWagerCents_USD);
	}

	// Game specific criteria for banker
	if (!ringRule.bankerBBWon.isInRange(aggBankerRingData.getBBWon()))
	{
		if (moreTrace)
		{
			PLog("-failed to pass bankerBBWon=%d", aggBankerRingData.getBBWon());
		}
		return false;
	}

	if (!ringRule.bankerBBPer100_scaled100.isInRange(aggBankerRingData.getBBWonPer100HandsScaled100()))
	{
		if (moreTrace)
		{
			PLog("-failed to pass BBWonPer100_scaled100=%d", aggBankerRingData.getBBWonPer100HandsScaled100());
		}
		return false;
	}

	if (!ringRule.avgPlayersDelatin_scaled100.isInRange(aggBankerRingData.getAvgPlayersDealtInScaled100()))
	{
		if (moreTrace)
		{
			PLog("-failed to pass avgPlayersDelatin_scaled100=%u", aggBankerRingData.getAvgPlayersDealtInScaled100());
		}
		return false;
	}

	aggBankerRingData.evalDumperRingHands(ringRule, moreTrace);

	// GPSI-285
	// if it has qualified dumpers, and num 5 or 7 is enabled, aggregate dumper total delta and wager
	if (aggBankerRingData.aggDumperData.size() &&
		(ringRule.rgDumperDeltaRatio_scaled100.isEnabled() || ringRule.rgDumperWagerRatio_scaled100.isEnabled()))
	{
		aggregateDumpersTotalData(bankerName, ringRule, ringRuleData, moreTrace);
		aggBankerRingData.evalDumperTotalData(ringRule, moreTrace);
	}

	if (!ringRule.dumperCount.isInRange((UINT32)aggBankerRingData.aggDumperData.size()))
	{
		if (moreTrace)
		{
			PLog("-failed to pass dumperCount=%u", aggBankerRingData.aggDumperData.size());
		}
		return false;
	}

	return true;
}

void RingHandCache::aggregateForRule(const PString& playerName, const ColCDA::RingGameRule& rule, RingRuleData& ringRuleData, bool moreTrace)
{
	PLog("RHC::aggForRule(banker: %s, rule: %u)", playerName.c_str(), rule.ruleId);

	AggBankerRingData& bankerData = ringRuleData.aggBankerRingData;

	PlayerAuxCacheMap::iterator it = playerAuxCacheMap.find(playerName);
	if (it == playerAuxCacheMap.end())
	{
		return;
	}

	if (moreTrace)
	{
		PLog("banker total RG hand#=%u", (UINT32)it->second.size());
	}

	for (const auto& handId : it->second)
	{
		ContainerMap::const_iterator handCacheIt = handsCacheMap.find(handId);
		if (handCacheIt == handsCacheMap.end())
		{
			PLog(LOG_TRACE_FAULTY_LOGIC " - handId %llu missing from rhc cache", handId);
			continue;
		}

		const RingHand& hand = handCacheIt->second;

		if (!rule.doesHandMatchForRule(hand, ringRuleData.evalTime))
		{
			if (moreTrace)
			{
				PLog("-RG hand=%llu, game filters", handId);
			}
			continue;
		}
		bankerData.aggregateHand(playerName, hand);
	}
}

void RingHandCache::aggregateDumpersTotalData(const PString& playerName, const RingGameRule& rule, RingRuleData& ringRuleData, bool moreTrace)
{
	PLog("RHC::aggForDumper(bankerName=%s)", playerName.c_str());

	for (auto& dumper : ringRuleData.aggBankerRingData.aggDumperData)
	{
		const PString& dumperName = dumper.first;
		AggDumperRingData& dumperData = dumper.second;

		PlayerAuxCacheMap::iterator it = playerAuxCacheMap.find(dumperName);
		if (it == playerAuxCacheMap.end())
		{
			return;
		}

		if (moreTrace)
		{
			PLog("dumper total RG hand#=%u", (UINT32)it->second.size());
		}

		for (const auto& handId : it->second)
		{
			ContainerMap::const_iterator handCacheIt = handsCacheMap.find(handId);
			if (handCacheIt == handsCacheMap.end())
			{
				PLog(LOG_TRACE_FAULTY_LOGIC " - handId %llu missing from rhc cache", handId);
				continue;
			}

			const RingHand& hand = handCacheIt->second;

			if (!rule.doesHandMatchForRule(hand, ringRuleData.evalTime))
			{
				if (moreTrace)
				{
					PLog("-RG hand=%llu, game filters", handId);
				}
				continue;
			}

			for (const auto& uih : hand.playersVec)
			{
				if (uih.userId.equals(dumperName))
				{
					dumperData.totalDeltaCents_USD += uih.deltaCents_USD;
					dumperData.totalWagerCents_USD += uih.potChips_USD;
					break;
				}
			}
		}

		if (moreTrace)
		{
			PLog("dumper(%s) total delta=%d", dumperName.c_str(), dumperData.totalDeltaCents_USD);
			PLog("dumper(%s) total wager=%d", dumperName.c_str(), dumperData.totalWagerCents_USD);
		}
	}
}

void RingHandCache::addToPlayerAuxCache(UINT64 handId, const PString& playerName)
{
	PlayerAuxCacheMap::iterator it = playerAuxCacheMap.find(playerName);

	if (it == playerAuxCacheMap.end())
	{
		static const set<UINT64> empty;
		it = playerAuxCacheMap.insert(make_pair(playerName, empty)).first;
	}

	it->second.insert(handId);
}

void RingHandCache::erasePlayersFromAuxCache(UINT64 handId, const vector<UserInRingHand>& playersVec)
{
	for (int i = 0; i < playersVec.size(); ++i)
	{
		eraseFromPlayerAuxCache(handId, playersVec[i].userId);
	}
}

void RingHandCache::eraseFromPlayerAuxCache(UINT64 handId, const PString& playerName)
{
	PlayerAuxCacheMap::iterator it = playerAuxCacheMap.find(playerName);

	if (it == playerAuxCacheMap.end())
	{
		return;
	}

	set<UINT64>& handIds = it->second;
	handIds.erase(handId);

	if (handIds.empty())
	{
		playerAuxCacheMap.erase(it);
	}
}

////////////////////////////////////////////////////////////////////////////////

Transact::Transact(CommMsgParser& parser)
{
	parse(parser);
}

void Transact::parse(CommMsgParser& parser)
{
	parser.
		parseUINT64(transId).
		parseStringP(userId).
		parseUINT32(transType).
		parseUINT32(objectId32).
		parseINT32(chips).
		parseINT32(playChips32).
		parseINT32(fpp).
		parseINT32(chipsAfter).
		parseINT32(playChipsAfter32).
		parseINT32(fppAfter).
		parseUINT32(adminIntId).
		parseStringP(comment).
		parseINT32(tChips).
		parseINT32(wChips).
		parseSrvTime(when).
		parseUINT32(userIntId).
		parseINT32(tChipsAfter).
		parseINT32(wChipsAfter).
		parseINT32(fppCents).
		parseINT32(fppCentsAfter).
		parseINT32(baseFpp).
		parseINT32(vpp1).
		parseINT32(vpp1After).
		parseINT32(vpp2).
		parseINT32(vpp2After).
		parseBOOL(isPlayMoney).
		parseStringP(currency).
		parseINT16(baseFppCents).
		parseINT16(vpp1Cents).
		parseINT16(vpp1AfterCents).
		parseINT16(vpp2Cents).
		parseINT16(vpp2AfterCents).
		parseINT64(playChips).
		parseINT64(playChipsAfter).
		parseUINT64(objectId).
		parseUINT64(appLoginId);
}

void Transact::compose(CommMsgBody& body) const
{
	body.
		composeUINT64(transId).
		composeString(userId).
		composeUINT32(transType).
		composeUINT32(objectId32).
		composeINT32(chips).
		composeINT32(playChips32).
		composeINT32(fpp).
		composeINT32(chipsAfter).
		composeINT32(playChipsAfter32).
		composeINT32(fppAfter).
		composeUINT32(adminIntId).
		composeString(comment).
		composeINT32(tChips).
		composeINT32(wChips).
		composeSrvTime(when).
		composeUINT32(userIntId).
		composeINT32(tChipsAfter).
		composeINT32(wChipsAfter).
		composeINT32(fppCents).
		composeINT32(fppCentsAfter).
		composeINT32(baseFpp).
		composeINT32(vpp1).
		composeINT32(vpp1After).
		composeINT32(vpp2).
		composeINT32(vpp2After).
		composeBOOL(isPlayMoney).
		composeString(currency).
		composeINT16(baseFppCents).
		composeINT16(vpp1Cents).
		composeINT16(vpp1AfterCents).
		composeINT16(vpp2Cents).
		composeINT16(vpp2AfterCents).
		composeINT64(playChips).
		composeINT64(playChipsAfter).
		composeUINT64(objectId).
		composeUINT64(appLoginId);
}

bool Transact::isRelevant() const
{
	if (isPlayMoney)
	{
		return false;
	}

	static const TransactionType RELEVANT_TRANSACT_TYPES[] = {
		eTableBuyIn, eTableReBuy, eTableAutoRebuy,
		eTableLeave, eTableChipsReturn, eTableForcedLeave, eCleanupObsoleteTables, eCleanupDeadTable, 
		eBlitzBuyIn, eBlitzReBuy, eBlitzAutoRebuy,
		eBlitzLeave, eBlitzCreditSinceLeft, eBlitzChipsReturn
	};
	const TransactionType* begin = RELEVANT_TRANSACT_TYPES;
	const TransactionType* end = RELEVANT_TRANSACT_TYPES + SZARR(RELEVANT_TRANSACT_TYPES);

	if (std::find(begin, end, transType) == end)
	{
		return false;
	}

	return true;
}

bool Transact::isRingTableEvent() const
{
	static const TransactionType RELEVANT_TRANSACT_TYPES[] = {
		eTableBuyIn, eTableReBuy, eTableAutoRebuy,
		eTableLeave, eTableChipsReturn,
	};
	const TransactionType* begin = RELEVANT_TRANSACT_TYPES;
	const TransactionType* end = RELEVANT_TRANSACT_TYPES + SZARR(RELEVANT_TRANSACT_TYPES);

	return (std::find(begin, end, transType) != end);
}

bool Transact::isBuyinEvent() const
{
	return transType == eTableBuyIn || transType == eBlitzBuyIn;
}

bool Transact::isLeaveEvent() const
{
	return transType == eTableLeave 
		|| transType == eBlitzLeave
		|| transType == eTableForcedLeave
		|| transType == eCleanupObsoleteTables
		|| transType == eCleanupDeadTable;
}

////////////////////////////////////////////////////////////////////////////////

UserOnGame::UserOnGame()
	: newestEventTime(time(nullptr)) 
	, chips(0)
{
}

void UserOnGame::addTransact(const Transact& transact)
{
	newestEventTime = transact.when.encode();
	chips += transact.chips;
}

bool UserOnGame::isWinner() const
{
	return chips > 0;
}

void PlayerGameCache::addTransaction(const Transact& transact, bool& shouldTriggerEval)
{
	shouldTriggerEval = false;

	PlayerMap::iterator it = playerMap.find(transact.userId);
	if (it == playerMap.end())
	{
		std::pair<PlayerMap::iterator,bool> ret;
		UserGameMap newUserGameMap;
		ret = playerMap.insert(make_pair(transact.userId, newUserGameMap));

		if (ret.second == false)
		{
			PLog("Error: addTransaction failed to add transact for user='%s'", transact.userId.c_str());
			return;
		}

		PLog("+PM %s", transact.userId.c_str());
		it = ret.first;
	}

	UserGameMap& userGameMap = it->second;
	if (transact.isBuyinEvent())
	{
		UserOnGame userOnGame;
		userOnGame.addTransact(transact);
		
		// buy-in create new objectId record
		userGameMap.insert(make_pair(transact.objectId, userOnGame));

		PLog("+UGM %llu", transact.objectId);
	}
	else
	{
		// update existing UserOnGame record
		UserGameMap::iterator userGameIt = userGameMap.find(transact.objectId);
		if (userGameIt == userGameMap.end())
		{
			// this might happen for some transtype 
			PLog("Missing objectId, transType=%u", transact.transType);
			return;
		}

		userGameIt->second.addTransact(transact);

		// Leave transact remove objectId
		if (transact.isLeaveEvent())
		{
			if (userGameIt->second.isWinner())
			{
				shouldTriggerEval = true;
			}

			userGameMap.erase(userGameIt);
			PLog("-UGM %llu", transact.objectId);
		}
	}

}

void PlayerGameCache::cull(time_t oldestAllowed)
{
	PLog("PMC::cull");
	UINT32 numRemoved = 0;
	UINT32 memUsed = 0;

	PlayerMap::iterator playerIt = playerMap.begin();
	while (playerIt != playerMap.end())
	{
		PlayerMap::iterator removePlayerIt = playerIt++;

		UserGameMap& userGameMap = removePlayerIt->second;
		UserGameMap::iterator userGameIt = userGameMap.begin();
		while (userGameIt != userGameMap.end())
		{
			UserGameMap::iterator removeUserGameIt = userGameIt++;
			if (removeUserGameIt->second.newestEventTime < oldestAllowed)
			{
				PLog("-UGM %llu", removeUserGameIt->first);
				userGameMap.erase(removeUserGameIt);
				++numRemoved;
			}
			else
			{
				memUsed += removeUserGameIt->second.memSize() + 8; // UINT64 objectId
			}
		}

		if (removePlayerIt->second.size() == 0)
		{
			PLog("-PM %s", removePlayerIt->first);
			playerMap.erase(removePlayerIt);
		}
	}

	PLog("PMC::cull - new size [%u], numRemoved: [%u], memUsed: %uKB", 
		(UINT32)playerMap.size(), numRemoved, memUsed / 1000);
}

////////////////////////////////////////////////////////////////////////////////
void CDHHandCache::add(const CdhEvent& cdhEvent)
{
	ContainerMap::iterator it = cdhCacheMap.find(cdhEvent.bankerUserId);

	if (it == cdhCacheMap.end())
	{
		list<CdhEvent> cdhHandList;
		cdhHandList.push_back(cdhEvent);
		cdhCacheMap.insert(make_pair(cdhEvent.bankerUserId, cdhHandList));
		PLog("+CDHU %s", cdhEvent.bankerUserId.c_str());
		PLog("+CDH %llu", cdhEvent.handId);
	}
	else
	{
		it->second.push_back(cdhEvent);
		PLog("+CDH %llu", cdhEvent.handId);
	}
}

void CDHHandCache::cull(time_t oldestAllowed, UINT32 maxUsersAllowed)
{
	UINT32 numRemoved = 0;
	INT32 minUsersToRemove = 0;

	if (cdhCacheMap.size() > maxUsersAllowed)
	{
		minUsersToRemove = cdhCacheMap.size() - maxUsersAllowed;
	}

	ContainerMap::iterator cdhUserIt = cdhCacheMap.begin();
	while (cdhUserIt != cdhCacheMap.end())
	{
		ContainerMap::iterator eraseUserIt = cdhUserIt++;
		
		list<CdhEvent>& cdhHandList = eraseUserIt->second;

		list<CdhEvent>::iterator handIt = cdhHandList.begin();
		while ( handIt != cdhHandList.end())
		{
			list<CdhEvent>::iterator eraseHandIt = handIt++;
			if (eraseHandIt->when.encode() < oldestAllowed)
			{
				PLog("-CDH %llu", eraseHandIt->handId);
				cdhHandList.erase(eraseHandIt);
				++numRemoved;
			}
		}

		if (cdhHandList.size() == 0 || minUsersToRemove > 0)
		{
			PLog("-CDHU %s", eraseUserIt->first);
			cdhCacheMap.erase(eraseUserIt);
			--minUsersToRemove;
		}
	}

	PLog("CDHC::cull - new size [%u], numRemoved: [%u]", 
		(UINT32)cdhCacheMap.size(), numRemoved);
}

bool CDHHandCache::checkDumperLastDepositFilters(const CdhEvent& cdhEvent, const vector<LastDepositParam>& lastDepositParamVec)
{
	// if no last deposit rule configured, keep the CDH
	if (lastDepositParamVec.size() == 0)
	{
		return true;
	}

	for (const auto& lastDepositParam : lastDepositParamVec)
	{
		if (cdhEvent.dumperLastDepAmtUSDCents == 0)
		{
			// Check NA rule
			if (isStringInVector(CCDA_LAST_DEPOSIT_NA_RULE, lastDepositParam.paySystemVec)
				&& isStringInVector("1", lastDepositParam.gateWayVec))
			{
				return true;
			}
		}
		else
		{
			if (isStringInVector("*", lastDepositParam.paySystemVec) ||
				isStringInVector(cdhEvent.dumperLastDepPaysystem, lastDepositParam.paySystemVec))
			{
				if (isStringInVector("*", lastDepositParam.gateWayVec) ||
					isStringInVector(cdhEvent.dumperLastDepGateway, lastDepositParam.gateWayVec))
				{
					double dumperLastDepAgeSecs_ = cdhEvent.dumperLastDepAgeSecs;
					INT32 lastDepHours = ColUtils::round<INT32>(dumperLastDepAgeSecs_ / SECONDS_IN_AN_HOUR);
					if (lastDepositParam.lastDepositCents.isInRange(cdhEvent.dumperLastDepAmtUSDCents) &&
						lastDepositParam.lastDepositHours.isInRange(lastDepHours))
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool CDHHandCache::checkBankerLastDepositFilters(const CdhEvent& cdhEvent, const vector<LastDepositParam>& lastDepositParamVec)
{
	// if no last deposit rule configured, keep the CDH
	if (lastDepositParamVec.size() == 0)
	{
		return true;
	}

	for (const auto& lastDepositParam : lastDepositParamVec)
	{
		if (cdhEvent.bankerLastDepAmtUSDCents == 0)
		{
			// Check NA rule
			if (isStringInVector(CCDA_LAST_DEPOSIT_NA_RULE, lastDepositParam.paySystemVec)
				&& isStringInVector("1", lastDepositParam.gateWayVec))
			{
				return true;
			}
		}
		else
		{
			if (isStringInVector("*", lastDepositParam.paySystemVec) ||
				isStringInVector(cdhEvent.bankerLastDepPaysystem, lastDepositParam.paySystemVec))
			{
				if (isStringInVector("*", lastDepositParam.gateWayVec) ||
					isStringInVector(cdhEvent.bankerLastDepGateway, lastDepositParam.gateWayVec))
				{
					double bankerLastDepAgeSecs_ = cdhEvent.bankerLastDepAgeSecs;
					INT32 lastDepHours = ColUtils::round<INT32>(bankerLastDepAgeSecs_ / SECONDS_IN_AN_HOUR);
					if (lastDepositParam.lastDepositCents.isInRange(cdhEvent.bankerLastDepAmtUSDCents) &&
						lastDepositParam.lastDepositHours.isInRange(lastDepHours))
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool CDHHandCache::checkCommonCDHFilters(const CommonRule& cdaRule, const CdhEvent& cdhEvent, bool moreTrace)
{
	// dumper loss percentage
	INT32 dumperLossPert_scaled100 = getDumperLossPertScaled100(cdhEvent);
	if (!cdaRule.cdhDumperLossPct_scaled100.isInRange(dumperLossPert_scaled100))
	{
		if (moreTrace)
		{
			PLog("-cdh handid=%llu, dumperLossPert_scaled100=%d", cdhEvent.handId, dumperLossPert_scaled100);
		}
		return false;
	}

	// last street reached
	if ((cdaRule.lastStreetReached & cdhEvent.finalStreetReached()) == 0)
	{
		if (moreTrace)
		{
			PLog("-cdh handid=%llu, lastStreetReached=%u", cdhEvent.handId, cdhEvent.finalStreetReached());
		}
		return false;
	}

	// PYR-84380 1.1.1 game flags
	UINT32 gameFlags = cdhEventRingFlagsToColGameFlags(cdhEvent.ringFlags);
	if (!(cdaRule.cdhGameFlags & gameFlags))
	{
		if (moreTrace)
		{
			PLog("-cdh handid=%llu, game type=%u", cdhEvent.handId, cdhEvent.ringFlags);
		}
		return false;
	}

	// PYR-84380 1.1.2 hand types
	UINT64 gameType = handTypeToGameTypeFlags(cdhEvent.gameType);
	if (!(cdaRule.cdhGameTypes & gameType))
	{
		if (moreTrace)
		{
			PLog("-cdh handid=%llu, game type=%d", cdhEvent.handId, cdhEvent.gameType);
		}
		return false;
	}

	// PYR-84380 1.1.3 game structures
	UINT32 gameStructure = gameStructToGameStructFlags(cdhEvent.structure);
	if (!(cdaRule.cdhGameStructures & gameStructure))
	{
		if (moreTrace)
		{
			PLog("-cdh handid=%llu, game structure=%d", cdhEvent.handId, cdhEvent.structure);
		}
		return false;
	}

	// PYR-84380 1.1.4 Max per table
	UINT32 maxPerTable = cdhEventMPTToMaxPerTableFlags(cdhEvent.mptCategory);
	if (!(cdaRule.cdhMaxPerTable & maxPerTable))
	{
		if (moreTrace)
		{
			PLog("-cdh handid=%llu, MPT=%u", cdhEvent.handId, cdhEvent.mptCategory);
		}
		return false;
	}

	// PYR-84380 1.1.5 big blind range
	if (!cdaRule.cdhBBCents.isInRange(cdhEvent.blindSzCents))
	{
		if (moreTrace)
		{
			PLog("-cdh handid=%llu, BB cents=%d", cdhEvent.handId, cdhEvent.blindSzCents);
		}
		return false;
	}

	// PYR-84380 1.1.6 big blind won
	INT32 bbWon = getBankerAmtBBWon(cdhEvent);
	if (!cdaRule.cdhBBWon.isInRange(bbWon))
	{
		if (moreTrace)
		{
			PLog("-cdh handid=%llu, BB won=%d", cdhEvent.handId, bbWon);
		}
		return false;
	}

	// PYR-84380 1.1.7 player dealt in
	if (!cdaRule.cdhPlayersDealtin.isInRange(cdhEvent.numPlayersDealtIn))
	{
		if (moreTrace)
		{
			PLog("-cdh handid=%llu, player dealt in=%u", cdhEvent.handId, cdhEvent.numPlayersDealtIn);
		}
		return false;
	}

	// PYR-84380 1.1.8 dumper account age
	if (!cdaRule.cdhDumperAccountAge.isInRange(cdhEvent.dumperAccountAgeDays))
	{
		if (moreTrace)
		{
			PLog("-cdh handid=%llu, dumper acc ages=%u", cdhEvent.handId, cdhEvent.dumperAccountAgeDays);
		}
		return false;
	}

	// PYR-84380 1.1.9 dumper LT Poker MPW
	if (!cdaRule.cdhDumperLTPokerMPWCents.isInRange(cdhEvent.dumperLtPokerMpwCents))
	{
		if (moreTrace)
		{
			PLog("-cdh handid=%llu, dumper lt poker=%lld", cdhEvent.handId, cdhEvent.dumperLtPokerMpwCents);
		}
		return false;
	}

	// PYR-84380 1.1.10 dumper LTFPP
	if (!cdaRule.cdhDumperLTFpp.isInRange(cdhEvent.dumperLtFpp))
	{
		if (moreTrace)
		{
			PLog("-cdh handid=%llu, dumper ltfpp=%d", cdhEvent.handId, cdhEvent.dumperLtFpp);
		}
		return false;
	}

	// PYR-84380 1.1.11 ruleIds
	if (cdaRule.cdhRuleIds.size())
	{
		PString ruleId;
		ruleId.appendUint(cdhEvent.ruleId);
		if (!isStringInVector(ruleId, cdaRule.cdhRuleIds))
		{
			if (moreTrace)
			{
				PLog("-cdh handid=%llu, ruleId=%u", cdhEvent.handId, cdhEvent.ruleId);
			}
			return false;
		}
	}

	// PYR-84380 1.1.12 dumper last actions
	INT32 dumperLastAction = getDumperLastAction(cdhEvent);
	if (!(cdaRule.cdhDumperLastActions & dumperLastAction))
	{
		if (moreTrace)
		{
			PLog("-cdh handid=%llu, dumper last action=%d", cdhEvent.handId, dumperLastAction);
		}
		return false;
	}

	// PYR-84380 1.1.13 turn/flop pot size
	INT32 turnFlopRatio = turnToFlopPotSzRatioScaled100(cdhEvent);
	if (!cdaRule.cdhTurnFlopPotRatio_scaled100.isInRange(turnFlopRatio))
	{
		if (moreTrace)
		{
			PLog("-cdh handid=%llu, turn/flop=%d", cdhEvent.handId, turnFlopRatio);
		}
		return false;
	}

	// PYR-84380 1.1.14 river/turn pot size
	INT32 riverTurnRatio = riverToTurnPotSzRatioScaled100(cdhEvent);
	if (!cdaRule.cdhRiverTurnPotRatio_scaled100.isInRange(riverTurnRatio))
	{
		if (moreTrace)
		{
			PLog("-cdh handid=%llu, river/turn=%d", cdhEvent.handId, riverTurnRatio);
		}
		return false;
	}

	// PYR-84380 1.1.15 final/river pot size
	INT32 finalRiverRatio = finalToRiverPotSzRatioScaled100(cdhEvent);
	if (!cdaRule.cdhFinalRiverPotRatio_scaled100.isInRange(finalRiverRatio))
	{
		if (moreTrace)
		{
			PLog("-cdh handid=%llu, final/river=%d", cdhEvent.handId, finalRiverRatio);
		}
		return false;
	}

	// PYR-84380 1.1.16 banker starting hand strength
	if (!doesBankerStartHandStrengthPassFilter(cdhEvent, cdaRule.cdhBankerStartingHandStrength))
	{
		if (moreTrace)
		{
			PLog("-cdh handid=%llu, banker starting hand", cdhEvent.handId);
		}
		return false;
	}

	// PYR-84380 1.1.17 dumper starting hand strength
	if (!doesDumperStartHandStrengthPassFilter(cdhEvent, cdaRule.cdhDumperStartingHandStrength))
	{
		if (moreTrace)
		{
			PLog("-cdh handid=%llu, dumper starting hand", cdhEvent.handId);
		}
		return false;
	}

	// PYR-84380 1.1.18 number of pot size bets
	if (!cdaRule.cdhNumbBets.isInRange(cdhEvent.numPotSizeBets))
	{
		if (moreTrace)
		{
			PLog("-cdh handid=%llu, pot size bet=%d", cdhEvent.handId, cdhEvent.numPotSizeBets);
		}
		return false;
	}

	// PYR-84380 1.1.19 number of preflop action
	INT32 dumperPreflopActions = getNumDumperActionsPreflop(cdhEvent);
	if (!cdaRule.cdhDumperPreFlopActions.isInRange(dumperPreflopActions))
	{
		if (moreTrace)
		{
			PLog("-cdh handid=%llu, preflop actions=%d", cdhEvent.handId, dumperPreflopActions);
		}
		return false;
	}

	// PYR-84380 1.1.20 number of flop action
	INT32 dumperFlopActions = getNumDumperActionsOnFlop(cdhEvent);
	if (!cdaRule.cdhDumperFlopActions.isInRange(dumperFlopActions))
	{
		if (moreTrace)
		{
			PLog("-cdh handid=%llu, flop actions=%d", cdhEvent.handId, dumperFlopActions);
		}
		return false;
	}

	// PYR-84380 1.1.21 number of turn action
	INT32 dumperTurnActions = getNumDumperActionsOnTurn(cdhEvent);
	if (!cdaRule.cdhDumperTurnActions.isInRange(dumperTurnActions))
	{
		if (moreTrace)
		{
			PLog("-cdh handid=%llu, turn actions=%d", cdhEvent.handId, dumperTurnActions);
		}
		return false;
	}

	// PYR-84380 1.1.22 number of river action
	INT32 dumperRiverActions = getNumDumperActionsOnRiver(cdhEvent);
	if (!cdaRule.cdhDumperRiverActions.isInRange(dumperRiverActions))
	{
		if (moreTrace)
		{
			PLog("-cdh handid=%llu, river actions=%d", cdhEvent.handId, dumperRiverActions);
		}
		return false;
	}

	// PYR-84380 1.1.23 flop pot size in big blinds
	INT32 flopPotBBRatio = getPotSizeBBOnFlopScaled100(cdhEvent);
	if (!cdaRule.cdhFlopSizeBBRatio_scaled100.isInRange(flopPotBBRatio))
	{
		if (moreTrace)
		{
			PLog("-cdh handid=%llu, flop pot/bb=%d", cdhEvent.handId, flopPotBBRatio);
		}
		return false;
	}

	// PYR-84380 1.1.24 banker final hand strength
	if (!doesBankerFinalHandStrengthPassFilter(cdhEvent, cdaRule.cdhBankerFinalHandStrength))
	{
		if (moreTrace)
		{
			PLog("-cdh handid=%llu, banker final hand", cdhEvent.handId);
		}
		return false;
	}

	// PYR-84380 1.1.25 dumper final hand strength
	if (!doesDumperFinalHandStrengthPassFilter(cdhEvent, cdaRule.cdhDumperFinalHandStrength))
	{
		if (moreTrace)
		{
			PLog("-cdh handid=%llu, dumper final hand", cdhEvent.handId);
		}
		return false;
	}

	// PYR-84380 1.1.26-28 dumper last deposit
	if (!checkDumperLastDepositFilters(cdhEvent, cdaRule.dumperLastDepositParams))
	{
		if (moreTrace)
		{
			PLog("-cdh handid=%llu, dumper last dep", cdhEvent.handId);
		}
		return false;
	}

	// PYR-84380 1.1.29-31 banker last deposit
	if (!checkBankerLastDepositFilters(cdhEvent, cdaRule.bankerLastDepositParams))
	{
		if (moreTrace)
		{
			PLog("-cdh handid=%llu, banker last dep", cdhEvent.handId);
		}
		return false;
	}

	// PYR-84380 1.1.34 dumper IP/COUNTRY mismatch
	if (cdaRule.cdhDumperFilters & CDHDUMPER_IP_COUNTRY_MISMATCH)
	{
		bool ipCountryMismatch = true;
		for (const PString& ipCountry : cdhEvent.dumperIpCountries)
		{
			if (ipCountry.equalsIgnoreCase(cdhEvent.dumperAccCountry))
			{
				ipCountryMismatch = false;
			}
			else
			{
				// if there is one mismatch, treat it as ip country mismatch
				ipCountryMismatch = true;
				break;
			}
		}
		if (!ipCountryMismatch)
		{
			if (moreTrace)
			{
				PLog("-cdh handid=%llu, ip country mismatch", cdhEvent.handId);
			}
			return false;
		}
	}

	// TODO: PYR-84380 1.1.35 dumper first usage of MAC

	// PYR-86936
	if (!isStringInVector(cdhEvent.currency, cdaRule.currencies, true))
	{
		if (moreTrace)
		{
			PLog("-cdh handid=%llu, currency=%s", cdhEvent.handId, cdhEvent.currency.c_str());
		}
		return false;
	}

	return true;
}

bool CDHHandCache::isRingRuleQualified(const PString& bankerName, const RingGameRule& ringRule, RingRuleData& ringRuleData, bool moreTrace)
{
	if (moreTrace)
	{
		PLog("CDHHandCache::isRingRuleQualified uid=%s ruleid=%u", bankerName.c_str(), ringRule.ruleId);
	}

	ContainerMap::iterator it = cdhCacheMap.find(bankerName);
	if (it == cdhCacheMap.end())
	{
		// check CDH related filters
		if (!ringRule.cdhBankerTotalHands.isInRange(0))
		{
			if (moreTrace)
			{
				PLog("-failed to pass cdhBankerTotalHands. No CDH");
			}
			return false;
		}
		if (!ringRule.cdhDumperCount.isInRange(0))
		{
			if (moreTrace)
			{
				PLog("-failed to pass cdhDumperCount. No CDH");
			}
			return false;
		}
		if (!ringRule.bankerTotalCDHNum.isInRange(0))
		{
			if (moreTrace)
			{
				PLog("-failed to pass bankerTotalCDHNum. No CDH");
			}
			return false;
		}

		return true;
	}

	const list<CdhEvent>& cdhEvents = it->second;

	if (moreTrace)
	{
		PLog("found #%u CDH", cdhEvents.size());
	}

	UINT32 cdhRuleHandNum = 0;
	PStringMap<UINT32> cdhDumpers;
	time_t lookbackTime = ringRuleData.evalTime - ringRule.lookbackPeriodMins * SECONDS_IN_A_DAY;
	for (const auto& cdhEvent : cdhEvents)
	{
		// skip old cdh hand for the rule
		if ((cdhEvent.when.encode() < lookbackTime) ||
			(cdhEvent.when.encode() > ringRuleData.evalTime))
		{
			continue;
		}

		// count global number of CDH hands
		ringRuleData.aggBankerRingData.addRingGameCDHHandCount(cdhEvent);

		if (cdhEvent.tournId)
		{
			continue;
		}

		// evaluate common CDH filters
		if (!checkCommonCDHFilters(ringRule, cdhEvent, moreTrace))
		{
			continue;
		}

		// evaluate ring game only CDH filters
		INT32 dumperLoss = cdhEvent.dumperChipsStartingCents - cdhEvent.dumperChipsEndingCents;
		if (!ringRule.cdhDumperLoss.isInRange(dumperLoss))
		{
			if (moreTrace)
			{
				PLog("-cdh handid=%llu, dumperLoss=%d", cdhEvent.handId, dumperLoss);
			}
			continue;
		}

		if (!ringRule.cdhFlopPotSizeCents.isInRange(cdhEvent.potSizeFlopCents))
		{
			if (moreTrace)
			{
				PLog("-cdh handid=%llu, potSizeFlopCents=%u", cdhEvent.handId, cdhEvent.potSizeFlopCents);
			}
			continue;
		}

		if (!ringRule.cdhTurnPotSizeCents.isInRange(cdhEvent.potSizeTurnCents))
		{
			if (moreTrace)
			{
				PLog("-cdh handid=%llu, potSizeTurnCents=%u", cdhEvent.handId, cdhEvent.potSizeTurnCents);
			}
			continue;
		}

		if (!ringRule.cdhRiverPotSizeCents.isInRange(cdhEvent.potSizeRiverCents))
		{
			if (moreTrace)
			{
				PLog("-cdh handid=%llu, potSizeRiverCents=%u", cdhEvent.handId, cdhEvent.potSizeRiverCents);
			}
			continue;
		}

		if (!ringRule.cdh7thStreetPotSizeCents.isInRange(cdhEvent.potSizeSeventhCents))
		{
			if (moreTrace)
			{
				PLog("-cdh handid=%llu, potSizeSeventhCents=%u", cdhEvent.handId, cdhEvent.potSizeSeventhCents);
			}
			continue;
		}

		if (!ringRule.cdhFinalPotSizeCents.isInRange(cdhEvent.potSizeFinalCents))
		{
			if (moreTrace)
			{
				PLog("-cdh handid=%llu, potSizeFinalCents=%u", cdhEvent.handId, cdhEvent.potSizeFinalCents);
			}
			continue;
		}

		// alertData used to generate CDA alert (aux2 tables)
		fillCDHAlertData(cdhEvent, ringRuleData.alertData);

		cdhRuleHandNum++;

		auto cdhDumperIt = cdhDumpers.find(cdhEvent.dumperUserId);
		if (cdhDumperIt == cdhDumpers.end())
		{
			cdhDumpers.insert(make_pair(cdhEvent.dumperUserId, 1));
		}
		else
		{
			++(cdhDumperIt->second);
		}
	}

	// PYR-84380 1.1.32 dumper count
	if (ringRule.cdhDumperCount.isEnabled())
	{
		UINT32 dumperCount = 0;
		for (const auto& cdhDumper : cdhDumpers)
		{
			const char* dumperId = cdhDumper.first;
			if (ringRule.cdhBankerTotalHands.isInRange(cdhDumper.second))
			{
				++dumperCount;
			}
			else
			{
				// PYR-87907 remove not qualified CDH dumpers
				ringRuleData.alertData.removeCDHDumper(dumperId);
			}
		}

		if (!ringRule.cdhDumperCount.isInRange(dumperCount))
		{
			if (moreTrace)
			{
				PLog("-failed to pass cdhDumperCount=%u", cdhDumpers.size());
			}
			return false;
		}
	}
	
	if (!ringRule.cdhBankerTotalHands.isInRange(cdhRuleHandNum))
	{
		if (moreTrace)
		{
			PLog("-failed to pass cdhBankerTotalHands=%u", cdhRuleHandNum);
		}
		return false;
	}

	if (!ringRule.bankerTotalCDHNum.isInRange(ringRuleData.aggBankerRingData.cdhHandNum))
	{
		if (moreTrace)
		{
			PLog("-failed to pass bankerTotalCDHNum=%u", ringRuleData.aggBankerRingData.cdhHandNum);
		}
		return false;
	}

	if (!ringRule.cdhBankerTotalHands.isEnabled() 
		&& !ringRule.cdhDumperCount.isEnabled())
	{
		// if cdh hand and cdh dumper filters are disabled, clear alert maps so alert will be created on ring game dumpers only
		if (moreTrace)
		{
			PLog("No CDH hand and dumper enabled.");
		}
		ringRuleData.alertData.cdhMap.clear();
		ringRuleData.alertData.relatedPlayerMap.clear();
	}

	return true;
}

bool CDHHandCache::isTournRuleQualified(const PString& bankerName, const TournRule& tournRule, TournRuleData& tournRuleData, bool moreTrace)
{
	if (moreTrace)
	{
		PLog("CDHHandCache::isTournRuleQualified uid=%s ruleid=%u", bankerName.c_str(), tournRule.ruleId);
	}

	ContainerMap::iterator it = cdhCacheMap.find(bankerName);
	if (it == cdhCacheMap.end())
	{
		// check CDH related filters
		if (!tournRule.cdhBankerTotalHands.isInRange(0))
		{
			if (moreTrace)
			{
				PLog("-failed to pass cdhBankerTotalHands. No CDH");
			}
			return false;
		}
		if (!tournRule.cdhDumperCount.isInRange(0))
		{
			if (moreTrace)
			{
				PLog("-failed to pass cdhDumperCount. No CDH");
			}
			return false;
		}
		if (!tournRule.bankerTotalCDHNum.isInRange(0))
		{
			if (moreTrace)
			{
				PLog("-failed to pass bankerTotalCDHNum. No CDH");
			}
			return false;
		}

		return true;
	}

	const list<CdhEvent>& cdhEvents = it->second;
	if (moreTrace)
	{
		PLog("found #%u CDH", cdhEvents.size());
	}

	UINT32 cdhRuleHandNum = 0;
	PStringMap<UINT32> cdhDumpers;
	time_t lookbackTime = tournRuleData.evalTime - tournRule.lookbackPeriodMins * SECONDS_IN_A_MINUTE;
	for (const auto& cdhEvent : cdhEvents)
	{
		// skip old cdh hand for the rule
		if ((cdhEvent.when.encode() < lookbackTime) ||
			(cdhEvent.when.encode() > tournRuleData.evalTime))
		{
			continue;
		}

		tournRuleData.aggBankerTournData.addTournCDHHandCount(cdhEvent);

		if (!cdhEvent.tournId)
		{
			continue;
		}

		// evaluate common CDH filters
		if (!checkCommonCDHFilters(tournRule, cdhEvent, moreTrace))
		{
			continue;
		}

		// alertData used to generate CDA alert (aux2 tables)
		fillCDHAlertData(cdhEvent, tournRuleData.alertData);

		cdhRuleHandNum++;

		auto cdhDumperIt = cdhDumpers.find(cdhEvent.dumperUserId);
		if (cdhDumperIt == cdhDumpers.end())
		{
			cdhDumpers.insert(make_pair(cdhEvent.dumperUserId, 1));
		}
		else
		{
			++(cdhDumperIt->second);
		}
	}

	// PYR-84380 1.1.32 dumper count
	if (tournRule.cdhDumperCount.isEnabled())
	{
		UINT32 dumperCount = 0;
		for (const auto& cdhDumper : cdhDumpers)
		{
			const char* dumperId = cdhDumper.first;
			if (tournRule.cdhBankerTotalHands.isInRange(cdhDumper.second))
			{
				++dumperCount;
			}
			else
			{
				// PYR-87907 remove not qualified CDH dumpers
				tournRuleData.alertData.removeCDHDumper(dumperId);
			}
		}

		if (!tournRule.cdhDumperCount.isInRange(dumperCount))
		{
			if (moreTrace)
			{
				PLog("-failed to pass cdhDumperCount=%u", cdhDumpers.size());
			}
			return false;
		}
	}

	if (!tournRule.cdhBankerTotalHands.isInRange(cdhRuleHandNum))
	{
		if (moreTrace)
		{
			PLog("-failed to pass cdhBankerTotalHands=%u", cdhRuleHandNum);
		}
		return false;
	}

	if (!tournRule.bankerTotalCDHNum.isInRange(tournRuleData.aggBankerTournData.cdhHandNum))
	{
		if (moreTrace)
		{
			PLog("-failed to pass bankerTotalCDHNum=%u", tournRuleData.aggBankerTournData.cdhHandNum);
		}
		return false;
	}

	if (!tournRule.cdhBankerTotalHands.isEnabled()
		&& !tournRule.cdhDumperCount.isEnabled())
	{
		// if cdh hand and cdh dumper filters are disabled, clear alert maps so alert will be created on ring game dumpers only
		if (moreTrace)
		{
			PLog("No CDH hand and dumper enabled.");
		}
		tournRuleData.alertData.cdhMap.clear();
		tournRuleData.alertData.relatedPlayerMap.clear();
	}

	return true;
}

void CDHHandCache::fillCDHAlertData(const CdhEvent& cdhEvent, CCDAAlertData& alertData)
{
	CDHAlertInfo cdhAlertInfo(cdhEvent);
	alertData.cdhMap.insert(make_pair(cdhEvent.handId, cdhAlertInfo));

	PStringMap<UINT32>::iterator it = alertData.relatedPlayerMap.find(cdhEvent.dumperUserId);
	if (it == alertData.relatedPlayerMap.end())
	{
		alertData.relatedPlayerMap.insert(make_pair(cdhEvent.dumperUserId, 1));
	}
	else
	{
		++(it->second);
	}
}

////////////////////////////////////////////////////////////////////////////////

NewTourn::NewTourn(const SrvTime& when)
	: numHands(0)
{
	createdTime = when.encode();
}

NewTourn::NewTourn()
	: createdTime(time(nullptr))
	, numHands(0)
{
}
void NewTournCache::add(UINT64 tournId, const SrvTime& when)
{
	newTournCache.insert(make_pair(tournId, NewTourn(when)));
	PLog("+NTC %llu", tournId);
}

void NewTournCache::erase(UINT64 tournId)
{
	newTournCache.erase(tournId);
	PLog("-NTC %llu", tournId);
}

void NewTournCache::addTournHand(const TournHand& tournHand)
{
	ContainerMap::iterator it = newTournCache.find(tournHand.tournId);
	if (it == newTournCache.end())
	{
		return;
	}

	++(it->second.numHands);
}

bool NewTournCache::find(UINT64 tournId, NewTourn& newTourn)
{
	ContainerMap::iterator it = newTournCache.find(tournId);
	if (it == newTournCache.end())
	{
		return false;
	}

	newTourn = it->second;
	return true;
}

void NewTournCache::cull(time_t oldestAllowed)
{
	UINT32 numRemoved = 0;

	ContainerMap::iterator tournIt = newTournCache.begin();
	while (tournIt != newTournCache.end())
	{
		ContainerMap::iterator eraseIt = tournIt++;

		if (eraseIt->second.createdTime < oldestAllowed)
		{
			PLog("-NTC %llu", eraseIt->first);
			newTournCache.erase(eraseIt);
			++numRemoved;
		}
	}

	PLog("NTC::cull - new size [%u], numRemoved: [%u]", (UINT32)newTournCache.size(), numRemoved);
}

TournPlayer::TournPlayer()
	: totalInvested_USD(0), rakePaid_USD(0), payout_USD(0)
{}

bool TournPlayer::isWinner() const
{
	INT32 roi = payout_USD - totalInvested_USD - rakePaid_USD;
	return roi > 0;
}

UINT32 TournPlayer::memSize() const
{
	return sizeof(TournPlayer) + playerId.length();
}

TournData::TournData(const ColossusPPTInfo& info, UINT32 numHands_)
{
	numHands = numHands_;

	ColossusTournInfo const& ti = info.tournInfo;

	finished = ti.closed.encode();
	buyIn_RuleCurrency = ti.buyIn;
	currency = ti.currency;

	// all game info converted to flags here. 
	// tourn game type
	UINT16 gameType = ti.isHiLo ? COLOSSUS_GAMETYPE_HILO_FLAG : 0;
	gameType += ti.gameType;
	gameTypeFlags = tournGameTypeToGameTypeFlags(gameType);

	// game structure
	structureFlags = gameStructToGameStructFlags(ti.structure);

	// game flags
	UINT32 flags;
	flags = eColGameFlags_NONE;
	flags |= (ti.isBlitzTourn() ? eColGameFlags_ZOOM : 0);
	flags |= (ti.isHomeGame ? eColGameFlags_HOMEGAME : 0);
	if (flags == eColGameFlags_NONE)
	{
		flags |= eColGameFlags_NORMAL;
	}
	gameFlags = (ColGameFlags)flags;

	// game speed
	flags = eColGameSpeed_None;
	if (ti.isBlindsIncreaseByHandsInterval())
	{
		flags |= eColGameSpeed_ByHandHyper;
	}
	flags |= (ti.turboVal() == SpeedDisplayRegular ? eColGameSpeed_Regular : 0);
	flags |= (ti.turboVal() == SpeedDisplayTurbo ? eColGameSpeed_Turbo : 0);
	flags |= (ti.turboVal() == SpeedDisplayHyper ? eColGameSpeed_Hyper : 0);
	flags |= (ti.turboVal() == SpeedDisplaySlow ? eColGameSpeed_Slow : 0);
	tournSpeedFlags = (ColGameSpeedFlags) flags;

	// tourn flags
	tournFlags = ColUtils::getTourneyFlags(
		ti.getIsTurboVal(),
		ti.isFifty50() ? 1 : 0,
		ti.isSatellite() ? 1 : 0,
		ti.isKnockout() ? 1 : 0);

	// Calculate ROI
	bool isFppBuyIn = ti.fppBuyIn > 0;
	for (int i = 0; i < info.players.size(); ++i)
	{
		const ColossusPPTPlayerInfo& player = info.players[i];
		
		bool treatAsFppFreeRoll = isFppBuyIn && (player.award + player.ticketAwards + player.bounties) > 0;

		TournPlayer tournPlayer;
		tournPlayer.playerId = player.userId;

		tournPlayer.totalInvested_USD = treatAsFppFreeRoll ? 0 : player.getInvestmentMinusRake(ti);
		tournPlayer.rakePaid_USD = treatAsFppFreeRoll ? 0 : player.getRakePaid(ti);
		tournPlayer.payout_USD = player.getPayout(ti);

		players.push_back(tournPlayer);
	}
}

UINT32 TournData::memSize() const
{
	UINT32 memSize = 0;
	for (int i = 0; i < players.size(); ++i)
	{
		memSize+= players[i].memSize();
	}

	return memSize + sizeof(TournData) + currency.length();
}

// PYR-114096
void TournData::updateTournFlags2(UINT64 tournFlags2)
{
	// BEGIN - update game speed flag
	{
		UINT32 flags = tournSpeedFlags;
		if (ColossusTournInfo::isBlindsIncreaseByHandsInterval(tournFlags2))
		{
			flags |= eColGameSpeed_ByHandHyper;
		}
		else
		{
			flags &= ~eColGameSpeed_ByHandHyper;
		}

		// We can end up with eColGameSpeed_None if we strip the Hyper-speed flag from a Hyper-speed tournament
		// In which case, just leave the game speed unchanged.
		if (flags != eColGameSpeed_None)
		{
			tournSpeedFlags = (ColGameSpeedFlags)flags;
		}
	}
	// END - update game speed flag

	// BEGIN - update tourn flag
	{
		tournFlags = ColUtils::updateTourneyFlags(tournFlags, tournFlags2);
	}
	// END - update tourn flag
}

////////////////////////////////////////////////////////////////////////////////

AggDumperTournData::AggDumperTournData()
{
	totalInvested_USD = 0;
	rakePaid_USD = 0;
	payout_USD = 0;
	totalWonTournNum = 0;
	totalTournNum = 0;
	cdhHandNum = 0;
}

UINT32 AggDumperTournData::getDumperNumCDHHands() const
{
	return cdhHandNum;
}

UINT32 AggDumperTournData::getDumperWinRate_scaled100() const
{
	if (totalTournNum == 0)
	{
		return 0;
	}

	double totalWonTournNum_ = totalWonTournNum;
	double totalTournNum_ = totalTournNum;

	double winRate_sacled100 = 100.0 * 100.0 * totalWonTournNum_ / totalTournNum_;

	return ColUtils::round<UINT32>(winRate_sacled100);
}

INT32 AggDumperTournData::getDumperROI_scaled100() const
{
	if((totalInvested_USD + rakePaid_USD) == 0)
	{
		return 0;
	}

	double payout_USD_ = payout_USD;
	double totalInvest_ = totalInvested_USD + rakePaid_USD;

	double roi_scaled100 = 100.0 * 100.0 * (payout_USD_ - totalInvest_) / totalInvest_;
	return ColUtils::round<INT32>(roi_scaled100);
}

AggBankerTournData::AggBankerTournData()
{
	totalInvested_USD = 0;
	rakePaid_USD = 0;
	payout_USD = 0;
	totalWonTournNum = 0;
	totalTournNum = 0;
	totalTournHandNum = 0;
	cdhHandNum = 0;
}

void AggBankerTournData::clear()
{
	totalInvested_USD = 0;
	rakePaid_USD = 0;
	totalWonTournNum = 0;
	totalTournNum = 0;
	totalTournHandNum = 0;
	cdhHandNum = 0;
	aggDumperData.clear();
}

UINT32 AggBankerTournData::getBankerNumCDHHands() const
{
	return cdhHandNum;
}

UINT32 AggBankerTournData::getBankerNumTourns() const
{
	return totalTournNum;
}

UINT32 AggBankerTournData::getBankerWinRate_scaled100() const
{
	if (totalTournNum == 0)
	{
		return 0;
	}

	double totalWonTournNum_ = totalWonTournNum;
	double totalTournNum_ = totalTournNum;

	double winRate_sacled100 = 100.0 * 100.0 * totalWonTournNum_ / totalTournNum;
	return ColUtils::round<UINT32>(winRate_sacled100);
}

INT32 AggBankerTournData::getBankerROI_scaled100() const
{
	if ((totalInvested_USD + rakePaid_USD) == 0)
	{
		return 0;
	}

	double payout_USD_ = payout_USD;
	double totalInvest_ = totalInvested_USD + rakePaid_USD;

	double roi_scaled100 = 100.0 * 100.0 * (payout_USD_ - totalInvest_) / totalInvest_;
	return ColUtils::round<INT32>(roi_scaled100);
}

UINT32 AggBankerTournData::getBankerAvgHandsPerTourn_scaled100() const
{
	if (totalTournNum == 0)
	{
		return 0;
	}

	double totalTournHandNum_ = totalTournHandNum;
	double totalTournNum_ = totalTournNum;

	double avgHands_scaled100 = 100.0 * totalTournHandNum_ / totalTournNum_;
	return ColUtils::round<UINT32>(avgHands_scaled100);
}

UINT32 AggBankerTournData::getAllDumperCount() const
{
	return aggDumperData.size();
}

UINT32 AggBankerTournData::getAllDumperWinRate_scaled100() const
{
	UINT32 totalWonTourns = 0;
	UINT32 totalTourn = 0;

	for (const auto& dumperData : aggDumperData)
	{
		totalWonTourns += dumperData.second.totalWonTournNum;
		totalTourn += dumperData.second.totalTournNum;
	}

	if (totalTourn == 0)
	{
		return 0;
	}

	double totalWonTourns_ = totalWonTourns;
	double totalTourn_ = totalTourn;

	double winRate_sacled100 = 100.0 * 100.0 * totalWonTourns_ / totalTourn_;
	return ColUtils::round<UINT32>(winRate_sacled100);
}

INT32 AggBankerTournData::getAllDumperROI_scaled100() const
{
	UINT32 totalInvested_Rake_USD = 0;
	UINT32 totalPayout_USD = 0;

	for (const auto& dumperData : aggDumperData)
	{
		totalInvested_Rake_USD += (dumperData.second.totalInvested_USD + dumperData.second.rakePaid_USD);
		totalPayout_USD += dumperData.second.payout_USD;
	}

	if (totalPayout_USD == 0)
	{
		return 0;
	}

	double totalPayout_USD_ = totalPayout_USD;
	double totalInvested_Rake_USD_ = totalInvested_Rake_USD;

	double roi_scaled100 = 100.0 * 100.0 * (totalPayout_USD_ - totalInvested_Rake_USD_) / totalInvested_Rake_USD_;
	return ColUtils::round<INT32>(roi_scaled100);
}

void AggBankerTournData::aggregateTourn(const PString& bankerName, const TournData& tournData)
{
	for (int i = 0; i < tournData.players.size(); ++i)
	{
		const TournPlayer& tournPlayer = tournData.players[i];

		if (tournPlayer.playerId.equals(bankerName))
		{
			// aggregate banker data
			totalInvested_USD += tournPlayer.totalInvested_USD;
			rakePaid_USD += tournPlayer.rakePaid_USD;
			payout_USD += tournPlayer.payout_USD;

			if (tournPlayer.isWinner())
			{
				totalWonTournNum++;
			}
			++totalTournNum;
			totalTournHandNum += tournData.numHands;
		}
		else
		{
			// aggregate dumper data
			DumperDataMapIt dumperIt = ensureDumper(tournPlayer.playerId);
			AggDumperTournData& dumperData = dumperIt->second;

			dumperData.totalInvested_USD += tournPlayer.totalInvested_USD;
			dumperData.rakePaid_USD += tournPlayer.rakePaid_USD;
			dumperData.payout_USD += tournPlayer.payout_USD;

			if (tournPlayer.isWinner())
			{
				++dumperData.totalWonTournNum;
			}
			++dumperData.totalTournNum;
		}
	}
}

void AggBankerTournData::evalDumperTournRule(const TournRule& tournRule)
{
	// remove not qualified dumpers
	DumperDataMapIt dumperIt = aggDumperData.begin();
	while (dumperIt != aggDumperData.end())
	{
		const AggDumperTournData& dumperData = dumperIt->second;

		DumperDataMapIt removeIt = dumperIt++;

		if (!tournRule.dumperNumCDH.isInRange(dumperData.cdhHandNum))
		{
			aggDumperData.erase(removeIt);
			continue;
		}

		if (!tournRule.dumperWinrate_scaled100.isInRange(dumperData.getDumperWinRate_scaled100()))
		{
			aggDumperData.erase(removeIt);
			continue;
		}

		if (!tournRule.dumperTournROI_scaled100.isInRange(dumperData.getDumperROI_scaled100()))
		{
			aggDumperData.erase(removeIt);
			continue;
		}

	}
}

void AggBankerTournData::addTournCDHHandCount(const CdhEvent& cdhEvent)
{
	++cdhHandNum;

	AggBankerTournData::DumperDataMapIt dumperIt = aggDumperData.find(cdhEvent.dumperUserId);
	if (dumperIt == aggDumperData.end())
	{
		AggDumperTournData aggDumperTournData;
		dumperIt = aggDumperData.insert(make_pair(cdhEvent.dumperUserId, aggDumperTournData)).first;
	}

	++(dumperIt->second.cdhHandNum);
}

AggBankerTournData::DumperDataMapIt AggBankerTournData::ensureDumper(const PString& dumperName)
{
	DumperDataMapIt dumperIt = aggDumperData.find(dumperName);
	if (dumperIt == aggDumperData.end())
	{
		AggDumperTournData empty;
		dumperIt = aggDumperData.insert(make_pair(dumperName, empty)).first;
	}

	return dumperIt;
}
////////////////////////////////////////////////////////////////////////////////

void TournDataCache::add(UINT64 tournId, const TournData& tournData)
{
	tournDataCache.insert(make_pair(tournId, tournData));

	const vector<TournPlayer>& playersVec = tournData.players;
	for (int i = 0; i < playersVec.size(); ++i)
	{
		addToPlayerAuxCache(tournId, playersVec[i].playerId);
	}

	PLog("+TDC %llu", tournId);
}

void TournDataCache::addToPlayerAuxCache(UINT64 tournId, const PString& playerName)
{
	PlayerAuxCacheMap::iterator it = playerAuxCacheMap.find(playerName);

	if (it == playerAuxCacheMap.end())
	{
		set<UINT64> empty;
		it = playerAuxCacheMap.insert(make_pair(playerName, empty)).first;
	}

	it->second.insert(tournId);
}

void TournDataCache::erasePlayersFromAuxCache(UINT64 tournId, const vector<TournPlayer>& playersVec)
{
	for (int i = 0; i < playersVec.size(); ++i)
	{
		eraseFromPlayerAuxCache(tournId, playersVec[i].playerId);
	}
}

void TournDataCache::eraseFromPlayerAuxCache(UINT64 tournId, const PString& playerName)
{
	PlayerAuxCacheMap::iterator it = playerAuxCacheMap.find(playerName);

	if (it == playerAuxCacheMap.end())
	{
		return;
	}

	set<UINT64>& tournIds = it->second;
	tournIds.erase(tournId);

	if (tournIds.empty())
	{
		playerAuxCacheMap.erase(it);
	}
}

void TournDataCache::cull(time_t oldestAllowed, UINT32 maxTournAllowed)
{
	UINT32 numRemoved = 0;
	UINT32 memUsed = 0;
	INT32 minNumTournToRemove = 0;

	if (tournDataCache.size() > maxTournAllowed)
	{
		minNumTournToRemove = tournDataCache.size() - maxTournAllowed;
	}

	ContainerMap::iterator tournCacheIt = tournDataCache.begin();
	while (tournCacheIt != tournDataCache.end())
	{
		ContainerMap::iterator eraseIt = tournCacheIt++;
		
		if (eraseIt->second.finished < oldestAllowed || numRemoved < minNumTournToRemove)
		{
			PLog("-TDC %llu", eraseIt->first);

			erasePlayersFromAuxCache(eraseIt->first, eraseIt->second.players);
			tournDataCache.erase(eraseIt);
			++numRemoved;
		}
		else
		{
			memUsed += (eraseIt->second.memSize() + 8);
		}
	}


	PLog("TDC::cull - new size [%u], numRemoved: [%u] , memUsed: %uKB, player aux size[%u]", 
		(UINT32)tournDataCache.size(), numRemoved, memUsed / 1000, (UINT32)playerAuxCacheMap.size());
}

bool TournDataCache::isTournRuleQualified(const PString& bankerName, const TournRule& tournRule,
										  TournRuleData& tournRuleData, bool moreTrace)
{
	if (moreTrace)
	{
		PLog("TournDataCache::isTournRuleQualified uid=%s ruleid=%u", bankerName.c_str(), tournRule.ruleId);
	}

	aggregateForRule(bankerName, tournRule, tournRuleData);

	AggBankerTournData& aggBankerTournData = tournRuleData.aggBankerTournData;

	// Game specific criteria for banker
	if (!tournRule.tournNum.isInRange(aggBankerTournData.getBankerNumTourns()))
	{
		if (moreTrace)
		{
			PLog("-failed to pass tournNum=%u", aggBankerTournData.getBankerNumTourns());
		}
		return false;
	}

	if (!tournRule.tournAveHands_scaled100.isInRange(aggBankerTournData.getBankerAvgHandsPerTourn_scaled100()))
	{
		if (moreTrace)
		{
			PLog("-failed to pass tournAveHands_scaled100=%u", aggBankerTournData.getBankerAvgHandsPerTourn_scaled100());
		}
		return false;
	}

	if (!tournRule.tournWinRate_scaled100.isInRange(aggBankerTournData.getBankerWinRate_scaled100()))
	{
		if (moreTrace)
		{
			PLog("-failed to pass tournWinRate_scaled100=%u", aggBankerTournData.getBankerWinRate_scaled100());
		}
		return false;
	}

	if (!tournRule.tournROI_scaled100.isInRange(aggBankerTournData.getBankerROI_scaled100()))
	{
		if (moreTrace)
		{
			PLog("-failed to pass tournROI_scaled100=%d", aggBankerTournData.getBankerROI_scaled100());
		}
		return false;
	}

	aggBankerTournData.evalDumperTournRule(tournRule);

	if (!tournRule.dumperCount.isInRange(aggBankerTournData.getAllDumperCount()))
	{
		if (moreTrace)
		{
			PLog("-failed to pass dumperCount=%u", aggBankerTournData.getAllDumperCount());
		}
		return false;
	}

	return true;
}

// PYR-114096
void TournDataCache::updateTournFlags2(UINT64 tournId, UINT64 tournFlags2)
{
	auto it = tournDataCache.find(tournId);
	if (it == tournDataCache.end())
	{
		PTRACE5("tid: %llu not in cache, aborting", tournId);
		return;
	}

	TournData& tournData = it->second;
	tournData.updateTournFlags2(tournFlags2);
}

void TournDataCache::aggregateForRule(const PString& playerName, const TournRule& rule, TournRuleData& tournRuleData)
{
	PLog("TDC::aggForRule(banker: %s, rule: %u)", playerName.c_str(), rule.ruleId);

	AggBankerTournData& bankerData = tournRuleData.aggBankerTournData;

	PlayerAuxCacheMap::iterator it = playerAuxCacheMap.find(playerName);
	if (it == playerAuxCacheMap.end())
	{
		return;
	}

	set<UINT64> tournIds = it->second;
	for (const auto& tournId : tournIds)
	{
		ContainerMap::const_iterator tournCacheIt = tournDataCache.find(tournId);
		if (tournCacheIt == tournDataCache.end())
		{
			PLog(LOG_TRACE_FAULTY_LOGIC " - tournId %llu missing from rhc cache", tournId);
			continue;
		}

		const TournData& tournData = tournCacheIt->second;

		if (!rule.doesTournMatch(tournData, tournRuleData.evalTime))
		{
			continue;
		}

		bankerData.aggregateTourn(playerName, tournData);
	}
}

/////////////////////////////////////////////////////////////////////////////////////
CommonRuleData::CommonRuleData(time_t evalTime_)
	: evalTime(evalTime_)
{
}

RingRuleData::RingRuleData(time_t evalTime)
	: CommonRuleData(evalTime)
{
}

bool BankerRingGameEvalData::hasQualifiedRules() const
{
	PLog("UID='%s' Qualified Ring Rule#=%u", bankerName.c_str(), (UINT32)ringRuleEvalMap.size());

	if (ringRuleEvalMap.size() > 0)
	{
		return true;
	}

	return false;
}

TournRuleData::TournRuleData(time_t evalTime)
	: CommonRuleData(evalTime)
{
}

UserEvaluationCache::UserEvaluation::UserEvaluation(const char* userId_, UINT32 evalType_, const SrvTime& evalTime_)
	: userId(userId_), evalType(evalType_), evalTime(evalTime_)
{}

void UserEvaluationCache::add(const char* userId_, UINT32 evalType_, const SrvTime& evalTime_)
{
	container.push_back(UserEvaluation(userId_, evalType_, evalTime_));

	PLog("UserEvaluationCache::add - uid=%s", userId_);
}

UINT32 UserEvaluationCache::size() const
{
	return container.size();
}

SRMMonitorCache::SRMStats::SRMStats(const SrvTime& srmTime_, const char* userId_)
{
	srmTime = srmTime_.encode();
	userId = userId_;
}

INT64 SRMMonitorCache::SRMStats::compare(const SRMStats& other) const
{
	INT64 res = srmTime - other.srmTime;
	if (res)
		return res;

	res = PString::compare(userId, other.userId);
	if (res)
		return res;

	return 0;
}

void SRMMonitorCache::add(const SrvTime& srmTime_, const char* userId_)
{
	container.insert(SRMStats(srmTime_, userId_));

	PLog("SRMMonitorCache::add - srmVec size=%u", (UINT32)container.size());
}

void SRMMonitorCache::cull(UINT32 lookbackMinutes)
{
	PLog("SRMMonitorCache::cull");

	SrvTime now;
	now.currentLocalTime();
	time_t oldestAllowed = now.encode() - lookbackMinutes * SECONDS_IN_A_MINUTE;
	UINT32 numRemoved = 0;

	auto it = container.begin();
	while (it != container.end())
	{
		auto removeIt = it++;

		if (removeIt->srmTime < oldestAllowed)
		{
			container.erase(removeIt);
			++numRemoved;
		}
	}

	PLog("numRemoved=[%u], srmSize=[%u]", numRemoved, (UINT32)container.size());
}

bool SRMMonitorCache::isOverThreshold(UINT32 srmThreshold)
{
	UINT32 totalSRMNum = container.size();

	bool isOverThreshold = totalSRMNum >= srmThreshold;

	if (isOverThreshold)
	{
		PLog("SRM COUNT IS OVERTHRESHOLD. TotalSRMNum=%u", totalSRMNum);
	}
	return isOverThreshold;
}

void SRMMonitorCache::clear()
{
	PLog("Clear SRMMonitorCache. total record=%u", (UINT32)container.size());
	container.clear();
}