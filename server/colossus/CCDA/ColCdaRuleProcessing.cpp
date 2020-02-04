// ===========================================================
// Copyright (C) 2018 PYR Software Ltd. All rights reserved.
// ===========================================================

#include "ColCda.h"
#include "ColCdaObject.h"
#include "wagerMeas.h"
#include "../shared/UserWMData.h"
#include "coldbm.h"
#include "collusions.h"
#include "secdbm.h"

using namespace ColCDA;

class RingGameRuleDataRequestCall : public CommClientGConnection::AsyncCall
{
	ColCdaServerObject* obj;
	BankerRingGameEvalData bankerRingGameEvalData;

public:
	RingGameRuleDataRequestCall(ColCdaServerObject* obj_, const BankerRingGameEvalData& bankerRingGameEvalData_)
		: obj(obj_), bankerRingGameEvalData(bankerRingGameEvalData_)
	{}

	void processReply(UINT32 /*reqId*/, UINT32 msgId, const CommMsgBody& body) override
	{
		CommMsgParser parser(body);
		switch (msgId)
		{
		case COLOSSUS_GET_CCDA_USER_DATA_REPLY:
			obj->gotRingGameDBMData(parser, bankerRingGameEvalData);
			break;
		case WM_A_GET_RM_WAGER_FR:
			obj->gotRingGameWMData(parser, bankerRingGameEvalData);
			break;
		case COLRAW_A_GET_CCDA_RINGGAME_RULE_DATA:
			obj->gotRingGameColRawData(parser, bankerRingGameEvalData);
			break;
		default:
			PLog("RingGameRuleDataRequestCall: Unknown msgId=%u", msgId);
			break;
		}
	}
};

class TournGameRuleDataRequestCall : public CommClientGConnection::AsyncCall
{
	ColCdaServerObject* obj;
	BankerTournRuleEvalData bankerTournRuleEvalData;

public:
	TournGameRuleDataRequestCall(ColCdaServerObject* obj_, const BankerTournRuleEvalData& bankerTournRuleEvalData_)
		: obj(obj_), bankerTournRuleEvalData(bankerTournRuleEvalData_)
	{}

	void processReply(UINT32 /*reqId*/, UINT32 msgId, const CommMsgBody& body) override
	{
		CommMsgParser parser(body);
		switch (msgId)
		{
		case COLOSSUS_GET_CCDA_USER_DATA_REPLY:
			obj->gotTournGameDBMData(parser, bankerTournRuleEvalData);
			break;
		case WM_A_GET_RM_WAGER_FR:
			obj->gotTournGameWMData(parser, bankerTournRuleEvalData);
			break;
		case COLRAW_A_GET_CCDA_TOURN_RULE_DATA:
			obj->gotTournGameColRawData(parser, bankerTournRuleEvalData);
			break;
			
		default:
			PLog("TournGameRuleDataRequestCall: Unknown msgId=%u", msgId);
			break;
		}
	}
};

class CommonDataRequestCall : public CommClientGConnection::AsyncCall
{
	ColCdaServerObject* obj;
	ColCDA::CCDAAlert alert;

public:
	CommonDataRequestCall(ColCdaServerObject* obj_, const ColCDA::CCDAAlert& alert_)
		: obj(obj_), alert(alert_)
	{}

	void processReply(UINT32 /*reqId*/, UINT32 msgId, const CommMsgBody& body) override
	{
		CommMsgParser parser(body);
		switch (msgId)
		{
		case COLLUSION_A_CREATE_SEC_ALERT:
			obj->gotCreateAlertReply(parser, alert);
			break;
		default:
			PLog("CommonDataRequestCall: Unknown msgId=%u", msgId);
			break;
		}
	}
};

bool ColCdaServerObject::isRingHandQualifiedAnyRules(const RingHand& ccdaRingHand)
{
	if (!ringGameRules.size())
	{
		return false;
	}

	SrvTime now;
	now.currentLocalTime();

	for (const auto& ringGameRule : ringGameRules)
	{	
		if (ringGameRule.doesHandMatchForRule(ccdaRingHand, now.encode()))
		{
			return true;
		}
	}
	return false;
}

bool ColCdaServerObject::isTournQualifiedAnyRules(const ColossusTournInfo& tournInfo)
{
	if (tournInfo.isPlayMoney)
	{
		return false;
	}

	// Only monitor SNG HU tourns
	if (!tournInfo.isSng() || tournInfo.maxEntry != 2)
	{
		return false;
	}

	if (!sngHUGameRules.size())
	{
		return false;
	}

	return true;
}


void ColCdaServerObject::triggerRingEval(const PString& userId, const SrvTime& evalTime)
{
	PString dummy;
	PLog("triggerRingEval - userId='%s',time=%s", userId.c_str(), srvTimeToString(evalTime, dummy));
	
	// PYR-92626
	if (!shouldRunEvaluation())
	{
		userEvaluationCache.add(userId, CCDA_RuleType_RingGame, evalTime);
		return;
	}

	if (colWhitelistCache.isWhitelistUser(userId, evalTime))
	{
		PLog("skip whitelist user.");
		return;
	}

	BankerRingGameEvalData bankerRingGameEvalData;
	bankerRingGameEvalData.bankerName = userId;
	getCurrentActiveRingRule(bankerRingGameEvalData, evalTime);

	if (!bankerRingGameEvalData.hasQualifiedRules())
	{
		return;
	}

	// CDH filters
	evalRingGameCDHRules(bankerRingGameEvalData);

	if (!bankerRingGameEvalData.hasQualifiedRules())
	{
		return;
	}
	
	// Ring game filter
	evalRingGameHandRules(bankerRingGameEvalData);

	if (!bankerRingGameEvalData.hasQualifiedRules())
	{
		return;
	}

	// add user evaluation data into cache
	requestRingGameDBMData(bankerRingGameEvalData);
}

void ColCdaServerObject::evalRingGameCDHRules(BankerRingGameEvalData& bankerRingGameEvalData)
{
	PLog("evalRingGameCDHRules - userId='%s'", bankerRingGameEvalData.bankerName.c_str());

	RingRuleEvalMap::iterator ruleIt = bankerRingGameEvalData.ringRuleEvalMap.begin();
	while (ruleIt != bankerRingGameEvalData.ringRuleEvalMap.end())
	{
		RingRuleEvalMap::iterator removeIt = ruleIt++;

		if (!cdhHandCache.isRingRuleQualified(bankerRingGameEvalData.bankerName, removeIt->first, removeIt->second, moreTrace))
		{
			PLog("-RingRule[%u] UID[%s] CDHHandRule", removeIt->first.ruleId, bankerRingGameEvalData.bankerName.c_str());
			bankerRingGameEvalData.ringRuleEvalMap.erase(removeIt);
		}
	}
}

void ColCdaServerObject::evalRingGameHandRules(BankerRingGameEvalData& bankerRingGameEvalData)
{
	PLog("evalRingGameHandRules - userId='%s'", bankerRingGameEvalData.bankerName.c_str());

	RingRuleEvalMap::iterator ruleIt = bankerRingGameEvalData.ringRuleEvalMap.begin();
	while (ruleIt != bankerRingGameEvalData.ringRuleEvalMap.end())
	{
		RingRuleEvalMap::iterator removeIt = ruleIt++;

		const auto& ringRule = removeIt->first;
		auto& ringRuleData = removeIt->second;

		if (!ringRule.dumperCount.isEnabled())
		{
			ringRuleData.aggBankerRingData.clear();
			if (moreTrace)
			{
				PLog("RingRule[%u] game dumper count disabled.", ringRule.ruleId);
			}
			continue;
		}

		if (!ringHandCache.isRingRuleQualified(bankerRingGameEvalData.bankerName, ringRule, ringRuleData, moreTrace))
		{
			PLog("-RingRule[%u] UID[%s] game rule", ringRule.ruleId, bankerRingGameEvalData.bankerName.c_str());
			bankerRingGameEvalData.ringRuleEvalMap.erase(removeIt);
		}
	}
}

void ColCdaServerObject::requestRingGameDBMData(BankerRingGameEvalData& bankerRingGameEvalData)
{
	PLog("requestRingGameDBMData - userId='%s'", bankerRingGameEvalData.bankerName.c_str());

	UINT32 userCount = 0;
	CommMsgBody msgUsers;

	msgUsers.composeString(bankerRingGameEvalData.bankerName);
	++userCount;

	PStringSet dumperIds;
	for (const auto& ringRuleEval : bankerRingGameEvalData.ringRuleEvalMap)
	{
		for (const auto& dumperData : ringRuleEval.second.aggBankerRingData.aggDumperData)
		{
			dumperIds.insert(dumperData.first);
		}
	}

	for (const auto& dumperId : dumperIds)
	{
		msgUsers.composeString(dumperId);
		++userCount;
	}

	PLog("total #%u DBM users", userCount);

	CommMsgBody msgBody;
	msgBody.composeUINT32(userCount);
	msgBody.merge(msgUsers);
	
	postToReader(COLOSSUS_GET_CCDA_USER_DATA, msgBody, new RingGameRuleDataRequestCall(this, bankerRingGameEvalData));
}

void ColCdaServerObject::gotRingGameDBMData(CommMsgParser& parser, BankerRingGameEvalData& bankerRingGameEvalData)
{
	PLog("gotRingGameDBMData - userId='%s'", bankerRingGameEvalData.bankerName.c_str());

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	if (errCode)
	{
		const char* errStr;
		parser.parseString(errStr);
		PLog("DBM error[%d]-%s", errCode, errStr);

		return;
	}

	CCDAUserDBMDataMap userDBMDataMap;
	userDBMDataMap.parse(parser);

	evalRingGameDBMData(userDBMDataMap, bankerRingGameEvalData);

	if (!bankerRingGameEvalData.hasQualifiedRules())
	{
		return;
	}

	requestRingGameWMData(bankerRingGameEvalData);
}

void ColCdaServerObject::evalRingGameDBMData(const CCDAUserDBMDataMap& userDBMDataMap, 
											 BankerRingGameEvalData& bankerRingGameEvalData)
{
	PLog("evalRingGameDBMRules - userId='%s'", bankerRingGameEvalData.bankerName.c_str());

	CCDAUserDBMDataMap::const_iterator bankerDBMIt = userDBMDataMap.find(bankerRingGameEvalData.bankerName);
	if (bankerDBMIt == userDBMDataMap.end())
	{
		// continue evaluation
		PLog(LOG_TRACE_FAULTY_LOGIC "Failed to find user DBM data ('%s')", bankerRingGameEvalData.bankerName.c_str());
	}

	time_t now = time(0);

	RingRuleEvalMap::iterator ruleIt = bankerRingGameEvalData.ringRuleEvalMap.begin();
	while (ruleIt != bankerRingGameEvalData.ringRuleEvalMap.end())
	{
		RingRuleEvalMap::iterator removeIt = ruleIt++;

		const RingGameRule& ringRule = removeIt->first;
		AggBankerRingData& aggBanker = removeIt->second.aggBankerRingData;

		//check banker license, country, account age
		if (bankerDBMIt != userDBMDataMap.end())
		{
			if (!ringRule.isLicCountryValid(bankerDBMIt->second.licenseId, bankerDBMIt->second.country, ringRule.bankerLicCountries))
			{
				if (moreTrace)
				{
					PLog("-failed to pass banker lic=%u, country=%s", bankerDBMIt->second.licenseId, bankerDBMIt->second.country.c_str());
				}
				bankerRingGameEvalData.ringRuleEvalMap.erase(removeIt);
				continue;
			}

			INT32 userAccountAge = (now - bankerDBMIt->second.registered.encode()) / SECONDS_IN_A_DAY;
			if (!ringRule.bankerAccountAge.isInRange(userAccountAge))
			{
				if (moreTrace)
				{
					PLog("-failed to pass banker acocunt age=%d", userAccountAge);
				}
				bankerRingGameEvalData.ringRuleEvalMap.erase(removeIt);
				continue;
			}
		}

		// check dumper DBM data
		AggBankerRingData::DumperDataMapIt dumperIt = aggBanker.aggDumperData.begin();
		while (dumperIt != aggBanker.aggDumperData.end())
		{
			AggBankerRingData::DumperDataMapIt removeDumperIt = dumperIt++;

			const char* dumperId = removeDumperIt->first;

			CCDAUserDBMDataMap::const_iterator dumperDBMIt = userDBMDataMap.find(dumperId);
			if (dumperDBMIt == userDBMDataMap.end())
			{
				// continue evaluation. consider current user passed rules
				PLog(LOG_TRACE_FAULTY_LOGIC "Failed to find user DBM data ('%s')", dumperId);
				continue;
			}

			if (!ringRule.isLicCountryValid(dumperDBMIt->second.licenseId, dumperDBMIt->second.country, ringRule.dumperLicCountries))
			{
				if (moreTrace)
				{
					PLog("-failed to pass dumper lic=%u, country=%s", dumperDBMIt->second.licenseId, dumperDBMIt->second.country.c_str());
				}
				aggBanker.aggDumperData.erase(removeDumperIt);
				continue;
			}

			INT32 userAccountAge = (now - dumperDBMIt->second.registered.encode()) / SECONDS_IN_A_DAY;
			if (!ringRule.dumperAccountAge.isInRange(userAccountAge))
			{
				if (moreTrace)
				{
					PLog("-failed to pass dumper acocunt age=%d", userAccountAge);
				}
				aggBanker.aggDumperData.erase(removeDumperIt);
				continue;
			}
		}

		if (!ringRule.dumperCount.isInRange((UINT32)aggBanker.aggDumperData.size()))
		{
			if (moreTrace)
			{
				PLog("-failed to pass dumperCount=%u", aggBanker.aggDumperData.size());
			}
			bankerRingGameEvalData.ringRuleEvalMap.erase(removeIt);
			continue;
		}
	}
}

bool ColCdaServerObject::doesRingGameRuleNeedWM(const RingGameRule& ringRule)
{
	return ringRule.bankerMPWCents.isEnabled() ||
		ringRule.dumperMPWCents.isEnabled();
}

void ColCdaServerObject::requestRingGameWMData(BankerRingGameEvalData& bankerRingGameEvalData)
{
	PLog("requestRingGameWMData - userId='%s'", bankerRingGameEvalData.bankerName.c_str());

	bool needsWMData = false;

	UINT32 userCount = 0;
	CommMsgBody msgUsers;

	msgUsers
		.composeString(bankerRingGameEvalData.bankerName)
		.composeUINT32(0)			// USERINTID, since we only need poker wm, no need for userintid
		;

	++userCount;

	PStringSet dumperIds;

	for (const auto& ringGameEval : bankerRingGameEvalData.ringRuleEvalMap)
	{
		if (doesRingGameRuleNeedWM(ringGameEval.first))
		{
			needsWMData = true;
		}
		for (const auto& dumperData : ringGameEval.second.aggBankerRingData.aggDumperData)
		{
			dumperIds.insert(dumperData.first);
		}
	}

	if (!needsWMData)
	{
		requestRingGamColRawData(bankerRingGameEvalData);
		return;
	}

	for (const auto& dumperId : dumperIds)
	{
		msgUsers
			.composeString(dumperId)
			.composeUINT32(0)			// USERINTID, since we only need poker wm, no need for userintid
			;
		++userCount;
	}

	PLog("total #%u WM users", userCount);

	// Set fromTime and toTime to null for lifetime request
	SrvTime fromTime, toTime;

	CommMsgBody msgBody;
	msgBody
		.composeUINT32(WagerMeas::eProductTypePoker)
		.composeSrvTime(fromTime)
		.composeSrvTime(toTime)
		.composeUINT32(userCount);

	msgBody.merge(msgUsers);

	postToWagerMeas(WM_Q_GET_RM_WAGER_FR, msgBody, new RingGameRuleDataRequestCall(this, bankerRingGameEvalData));
}

void ColCdaServerObject::gotRingGameWMData(CommMsgParser& parser, BankerRingGameEvalData& bankerRingGameEvalData)
{
	PLog("gotRingGameWMData - userId='%s'", bankerRingGameEvalData.bankerName.c_str());

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	if (errCode)
	{
		const char* errStr;
		parser.parseString(errStr);
		PLog("DBM error[%d]-%s", errCode, errStr);

		return;
	}

	UserWMDataMap userWMDataMap;
	userWMDataMap.parse(parser);

	PLog("WM user size=%u", (UINT32)userWMDataMap.size());

	evalRingGameWMData(userWMDataMap, bankerRingGameEvalData);

	if (!bankerRingGameEvalData.hasQualifiedRules())
	{
		return;
	}

	// col raw data
	requestRingGamColRawData(bankerRingGameEvalData);
}

void ColCdaServerObject::evalRingGameWMData(const UserWMDataMap& userWMDataMap, 
											BankerRingGameEvalData& bankerRingGameEvalData)
{
	PLog("evalRingGameWMData - userId='%s'", bankerRingGameEvalData.bankerName.c_str());

	UserWMDataMap::const_iterator bankerWMIt = userWMDataMap.find(bankerRingGameEvalData.bankerName);
	if (bankerWMIt == userWMDataMap.end())
	{
		// continue evaluation
		PLog(LOG_TRACE_FAULTY_LOGIC "Failed to find user WM data ('%s')", bankerRingGameEvalData.bankerName.c_str());
	}

	RingRuleEvalMap::iterator ruleIt = bankerRingGameEvalData.ringRuleEvalMap.begin();
	while (ruleIt != bankerRingGameEvalData.ringRuleEvalMap.end())
	{
		RingRuleEvalMap::iterator removeIt = ruleIt++;

		const RingGameRule& ringRule = removeIt->first;
		AggBankerRingData& aggBanker = removeIt->second.aggBankerRingData;

		PString ruleCur;
		ringRule.getRuleCurrency(ruleCur);

		//check banker poker WM
		if (bankerWMIt != userWMDataMap.end())
		{
			INT64 bankerPokerWM = convertUserWagerAmt(bankerWMIt->second.amtPokerPerCurr, ruleCur);
			if (!ringRule.bankerMPWCents.isInRange(bankerPokerWM))
			{
				if (moreTrace)
				{
					PLog("-failed to pass banker totWagerPoker=%llu", bankerPokerWM);
				}
				bankerRingGameEvalData.ringRuleEvalMap.erase(removeIt);
				continue;
			}
		}

		// check dumper WM data
		AggBankerRingData::DumperDataMapIt dumperIt = aggBanker.aggDumperData.begin();
		while (dumperIt != aggBanker.aggDumperData.end())
		{
			AggBankerRingData::DumperDataMapIt removeDumperIt = dumperIt++;

			const char* dumperId = removeDumperIt->first;

			UserWMDataMap::const_iterator dumperWMIt = userWMDataMap.find(dumperId);
			if (dumperWMIt == userWMDataMap.end())
			{
				// continue evaluation. consider current user passed rules
				PLog(LOG_TRACE_FAULTY_LOGIC "Failed to find user WM data ('%s')", dumperId);
				continue;
			}

			INT64 dumperPokerWM = convertUserWagerAmt(dumperWMIt->second.amtPokerPerCurr, ruleCur);
			if (!ringRule.dumperMPWCents.isInRange(dumperPokerWM))
			{
				if (moreTrace)
				{
					PLog("-failed to pass dumper totWagerPoker=%llu", dumperPokerWM);
				}
				aggBanker.aggDumperData.erase(removeDumperIt);
				continue;
			}
		}

		if (!ringRule.dumperCount.isInRange((UINT32)aggBanker.aggDumperData.size()))
		{
			if (moreTrace)
			{
				PLog("-failed to pass dumperCount=%u", aggBanker.aggDumperData.size());
			}
			bankerRingGameEvalData.ringRuleEvalMap.erase(removeIt);
			continue;
		}

		if (!ringRule.allDumpersBBper100Lost_scaled100.isInRange(aggBanker.getAllDumpersLostToMeBBPer100Scaled100()))
		{
			if (moreTrace)
			{
				PLog("-failed to pass allDumpersBBper100Lost_scaled100=%u", aggBanker.getAllDumpersLostToMeBBPer100Scaled100());
			}
			bankerRingGameEvalData.ringRuleEvalMap.erase(removeIt);
			continue;
		}

		if (!ringRule.allDumpersAmountLost.isInRange(aggBanker.getAllDumpersLostToMeCents()))
		{
			if (moreTrace)
			{
				PLog("-failed to pass allDumpersAmountLost=%llu", aggBanker.getAllDumpersLostToMeCents());
			}
			bankerRingGameEvalData.ringRuleEvalMap.erase(removeIt);
			continue;
		}
	}
}

bool ColCdaServerObject::doesRingGameRuleNeedColRaw(const RingGameRule& ringRule)
{
	return ringRule.bankerTotalHands.isEnabled() ||
		ringRule.bankerTotalProfitCents.isEnabled();
}

void ColCdaServerObject::requestRingGamColRawData(BankerRingGameEvalData& bankerRingGameEvalData)
{
	PLog("requestRingGamColRawData - userId='%s'", bankerRingGameEvalData.bankerName.c_str());

	set<UINT32> lookbackMinutesSet;

	bool needsColRawData = false;
	for (const auto& ringRuleEval : bankerRingGameEvalData.ringRuleEvalMap)
	{
		const auto& ringRule = ringRuleEval.first;
		if (doesRingGameRuleNeedColRaw(ringRule))
		{
			needsColRawData = true;
		}
		lookbackMinutesSet.insert(ringRule.lookbackPeriodMins);
	}

	if (!needsColRawData)
	{
		prepareCCDARingGameAlert(bankerRingGameEvalData);
		return;
	}

	UINT32 count = 0;
	CommMsgBody msgLookBack;

	for (const auto& lookBackMins : lookbackMinutesSet)
	{
		msgLookBack.composeUINT32(lookBackMins);
		++count;
	}

	CommMsgBody msgBody;
	msgBody
		.composeString(bankerRingGameEvalData.bankerName)
		.composeUINT32(count);

	msgBody.merge(msgLookBack);

	postToColRawDbm(COLRAW_Q_GET_CCDA_RINGGAME_RULE_DATA, msgBody, new RingGameRuleDataRequestCall(this, bankerRingGameEvalData));
}

void ColCdaServerObject::gotRingGameColRawData(CommMsgParser& parser, BankerRingGameEvalData& bankerRingGameEvalData)
{
	PLog("gotRingGameColRawData - userId='%s'", bankerRingGameEvalData.bankerName.c_str());

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	if (errCode)
	{
		const char* errStr;
		parser.parseString(errStr);
		PLog("DBM error[%d]-%s", errCode, errStr);

		return;
	}

	RingGameRuleColDataMap ringGameRuleColDataMap;
	ringGameRuleColDataMap.parse(parser);

	evalRingGameColRawData(ringGameRuleColDataMap, bankerRingGameEvalData);

	prepareCCDARingGameAlert(bankerRingGameEvalData);
}

void ColCdaServerObject::evalRingGameColRawData(const RingGameRuleColDataMap& ringGameRuleColDataMap, BankerRingGameEvalData& bankerRingGameEvalData)
{
	PLog("evalRingGameColRawData - userId='%s'", bankerRingGameEvalData.bankerName.c_str());

	RingRuleEvalMap::iterator ruleIt = bankerRingGameEvalData.ringRuleEvalMap.begin();
	while (ruleIt != bankerRingGameEvalData.ringRuleEvalMap.end())
	{
		RingRuleEvalMap::iterator removeIt = ruleIt++;

		const RingGameRule& ringRule = removeIt->first;

		RingGameRuleColDataMap::const_iterator colIt = ringGameRuleColDataMap.find(ringRule.lookbackPeriodMins);
		if (colIt == ringGameRuleColDataMap.end())
		{
			PLog(LOG_TRACE_FAULTY_LOGIC "Failed to find colraw data lookbackPeriodMins=%u", ringRule.lookbackPeriodMins);
			continue;
		}

		const RingGameRuleColData& colData = colIt->second;
		INT64 totalProfit = convertAmountCurr(colData.totalProfit, "USD");
		if (!ringRule.bankerTotalProfitCents.isInRange(totalProfit))
		{
			if (moreTrace)
			{
				PLog("-failed to pass bankerTotalProfitCents=%lld", totalProfit);
			}
			bankerRingGameEvalData.ringRuleEvalMap.erase(removeIt);
			continue;
		}

		if (!ringRule.bankerTotalHands.isInRange(colData.totalHands))
		{
			if (moreTrace)
			{
				PLog("-failed to pass totalHands=%u", colData.totalHands);
			}
			bankerRingGameEvalData.ringRuleEvalMap.erase(removeIt);
			continue;
		}
	}
}

void ColCdaServerObject::prepareCCDARingGameAlert(BankerRingGameEvalData& bankerRingGameEvalData)
{
	PLog("prepareCCDATournAlert - userId='%s'", bankerRingGameEvalData.bankerName.c_str());

	if (!bankerRingGameEvalData.hasQualifiedRules())
	{
		return;
	}

	RingRuleEvalMap::iterator it = bankerRingGameEvalData.ringRuleEvalMap.begin();
	
	const RingGameRule& ringGameRule = it->first;
	CCDAAlertData& alertData = it->second.alertData;
	alertData.appendRuleInfo(ringGameRule.ruleId, ringGameRule.ruleName);

	if (enableAlertAutoAction)
	{
		alertData.autoActions = ringGameRule.autoActions;
	}
	
	vector<PString> dumperUserIds;
	for (const auto& aggDumper : it->second.aggBankerRingData.aggDumperData)
	{
		dumperUserIds.push_back(aggDumper.first);
	}

	sendCCDAAlert(bankerRingGameEvalData.bankerName, alertData, CCDA_RuleType_RingGame, dumperUserIds);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Tourn evaluation methods
void ColCdaServerObject::triggerTournEval(const PString& userId, const SrvTime& evalTime)
{
	PLog("triggerTournEval - userId='%s'", userId.c_str());

	// PYR-92626
	if (!shouldRunEvaluation())
	{
		userEvaluationCache.add(userId, CCDA_RuleType_SNGHU, evalTime);
		return;
	}

	if (colWhitelistCache.isWhitelistUser(userId, evalTime))
	{
		PLog("skip whitelist user.");
		return;
	}

	BankerTournRuleEvalData bankerTournRuleEvalData;
	bankerTournRuleEvalData.bankerName = userId;
	getCurrentActiveTournRule(bankerTournRuleEvalData, evalTime);

	if (!bankerTournRuleEvalData.hasQualifiedRules())
	{
		return;
	}

	// CDH filters
	evalTournCDHRules(bankerTournRuleEvalData);

	if (!bankerTournRuleEvalData.hasQualifiedRules())
	{
		return;
	}

	evalTournGameRules(bankerTournRuleEvalData);

	if (!bankerTournRuleEvalData.hasQualifiedRules())
	{
		return;
	}

	requestTournGameDBMData(bankerTournRuleEvalData);
}

void ColCdaServerObject::evalTournCDHRules(BankerTournRuleEvalData& tournRuleEvalData)
{
	PLog("evalTournGameCDHRules - userId='%s'", tournRuleEvalData.bankerName.c_str());

	TournRuleEvalMap::iterator ruleIt = tournRuleEvalData.tournRuleEvalMap.begin();
	while (ruleIt != tournRuleEvalData.tournRuleEvalMap.end())
	{
		TournRuleEvalMap::iterator removeIt = ruleIt++;

		if (!cdhHandCache.isTournRuleQualified(tournRuleEvalData.bankerName, removeIt->first, removeIt->second, moreTrace))
		{
			PLog("-TournRule[%u] UID[%s] CDHHandRule", removeIt->first.ruleId, tournRuleEvalData.bankerName.c_str());
			tournRuleEvalData.tournRuleEvalMap.erase(removeIt);
		}
	}
}

void ColCdaServerObject::evalTournGameRules(BankerTournRuleEvalData& bankerTournRuleEvalData)
{
	PLog("evalTournGameRules - userId='%s'", bankerTournRuleEvalData.bankerName.c_str());

	TournRuleEvalMap::iterator ruleIt = bankerTournRuleEvalData.tournRuleEvalMap.begin();
	while (ruleIt != bankerTournRuleEvalData.tournRuleEvalMap.end())
	{
		TournRuleEvalMap::iterator removeIt = ruleIt++;

		if (!tournDataCache.isTournRuleQualified(bankerTournRuleEvalData.bankerName, removeIt->first, removeIt->second, moreTrace))
		{
			PLog("-TournRule[%u] UID[%s] game rule", removeIt->first.ruleId, bankerTournRuleEvalData.bankerName.c_str());
			bankerTournRuleEvalData.tournRuleEvalMap.erase(removeIt);
		}
	}
}

void ColCdaServerObject::requestTournGameDBMData(BankerTournRuleEvalData& bankerTournRuleEvalData)
{
	PLog("requestTournGameDBMData - userId='%s'", bankerTournRuleEvalData.bankerName.c_str());

	UINT32 userCount = 0;
	CommMsgBody msgUsers;

	msgUsers.composeString(bankerTournRuleEvalData.bankerName);
	++userCount;

	PStringSet dumperIds;
	for (const auto& tournRuleEval : bankerTournRuleEvalData.tournRuleEvalMap)
	{
		for (const auto& dumperData : tournRuleEval.second.aggBankerTournData.aggDumperData)
		{
			dumperIds.insert(dumperData.first);
		}
	}

	for (const auto& dumperId : dumperIds)
	{
		msgUsers.composeString(dumperId);
		++userCount;
	}

	PLog("total #%u DBM users", userCount);

	CommMsgBody msgBody;
	msgBody.composeUINT32(userCount);
	msgBody.merge(msgUsers);

	postToReader(COLOSSUS_GET_CCDA_USER_DATA, msgBody, new TournGameRuleDataRequestCall(this, bankerTournRuleEvalData));
}

void ColCdaServerObject::gotTournGameDBMData(CommMsgParser& parser, BankerTournRuleEvalData& bankerTournRuleEvalData)
{
	PLog("gotTournGameDBMData - userId='%s'", bankerTournRuleEvalData.bankerName.c_str());

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	if (errCode)
	{
		const char* errStr;
		parser.parseString(errStr);
		PLog("DBM error[%d]-%s", errCode, errStr);

		return;
	}

	CCDAUserDBMDataMap userDBMDataMap;
	userDBMDataMap.parse(parser);

	evalTournGameDBMData(userDBMDataMap, bankerTournRuleEvalData);

	if (!bankerTournRuleEvalData.hasQualifiedRules())
	{
		return;
	}

	requestTournGameWMData(bankerTournRuleEvalData);
}

void ColCdaServerObject::evalTournGameDBMData(const CCDAUserDBMDataMap& userDBMDataMap, 
											  BankerTournRuleEvalData& bankerTournRuleEvalData)
{
	PLog("evalTournGameDBMData - userId='%s'", bankerTournRuleEvalData.bankerName.c_str());

	CCDAUserDBMDataMap::const_iterator bankerDBMIt = userDBMDataMap.find(bankerTournRuleEvalData.bankerName);
	if (bankerDBMIt == userDBMDataMap.end())
	{
		// continue evaluation
		PLog(LOG_TRACE_FAULTY_LOGIC "Failed to find user DBM data ('%s')", bankerTournRuleEvalData.bankerName.c_str());
	}

	time_t now = time(0);

	TournRuleEvalMap::iterator ruleIt = bankerTournRuleEvalData.tournRuleEvalMap.begin();
	while (ruleIt != bankerTournRuleEvalData.tournRuleEvalMap.end())
	{
		TournRuleEvalMap::iterator removeIt = ruleIt++;

		const TournRule& tournRule = removeIt->first;
		AggBankerTournData& aggBanker = removeIt->second.aggBankerTournData;

		//check banker license, country, account age
		if (bankerDBMIt != userDBMDataMap.end())
		{
			if (!tournRule.isLicCountryValid(bankerDBMIt->second.licenseId, bankerDBMIt->second.country, tournRule.bankerLicCountries))
			{
				if (moreTrace)
				{
					PLog("-failed to pass banker lic=%u, country=%s", bankerDBMIt->second.licenseId, bankerDBMIt->second.country.c_str());
				}
				bankerTournRuleEvalData.tournRuleEvalMap.erase(removeIt);
				continue;
			}

			INT32 userAccountAge = (now - bankerDBMIt->second.registered.encode()) / SECONDS_IN_A_DAY;
			if (!tournRule.bankerAccountAge.isInRange(userAccountAge))
			{
				if (moreTrace)
				{
					PLog("-failed to pass banker acocunt age=%d", userAccountAge);
				}
				bankerTournRuleEvalData.tournRuleEvalMap.erase(removeIt);
				continue;
			}
		}

		// check dumper DBM data
		AggBankerTournData::DumperDataMapIt dumperIt = aggBanker.aggDumperData.begin();
		while (dumperIt != aggBanker.aggDumperData.end())
		{
			AggBankerTournData::DumperDataMapIt removeDumperIt = dumperIt++;

			const char* dumperId = removeDumperIt->first;

			CCDAUserDBMDataMap::const_iterator dumperDBMIt = userDBMDataMap.find(dumperId);
			if (dumperDBMIt == userDBMDataMap.end())
			{
				// continue evaluation. consider current user passed rules
				PLog(LOG_TRACE_FAULTY_LOGIC "Failed to find user DBM data ('%s')", dumperId);
				continue;
			}

			if (!tournRule.isLicCountryValid(dumperDBMIt->second.licenseId, dumperDBMIt->second.country, tournRule.dumperLicCountries))
			{
				if (moreTrace)
				{
					PLog("-failed to pass dumper lic=%u, country=%s", dumperDBMIt->second.licenseId, dumperDBMIt->second.country.c_str());
				}
				aggBanker.aggDumperData.erase(removeDumperIt);
				continue;
			}

			INT32 userAccountAge = (now - dumperDBMIt->second.registered.encode()) / SECONDS_IN_A_DAY;
			if (!tournRule.dumperAccountAge.isInRange(userAccountAge))
			{
				if (moreTrace)
				{
					PLog("-failed to pass dumper acocunt age=%d", userAccountAge);
				}
				aggBanker.aggDumperData.erase(removeDumperIt);
				continue;
			}
		}

		if (!tournRule.dumperCount.isInRange((UINT32)aggBanker.aggDumperData.size()))
		{
			if (moreTrace)
			{
				PLog("-failed to pass dumperCount=%u", aggBanker.aggDumperData.size());
			}
			bankerTournRuleEvalData.tournRuleEvalMap.erase(removeIt);
			continue;
		}

	}
}


void ColCdaServerObject::requestTournGameWMData(BankerTournRuleEvalData& bankerTournRuleEvalData)
{
	PLog("requestTournGameWMData - userId='%s'", bankerTournRuleEvalData.bankerName.c_str());

	UINT32 userCount = 0;
	CommMsgBody msgUsers;

	msgUsers
		.composeString(bankerTournRuleEvalData.bankerName)
		.composeUINT32(0)			// USERINTID, since we only need poker wm, no need for userintid
		;

	++userCount;

	PStringSet dumperIds;
	for (const auto& tournRuleEval : bankerTournRuleEvalData.tournRuleEvalMap)
	{
		for (const auto& dumperData : tournRuleEval.second.aggBankerTournData.aggDumperData)
		{
			dumperIds.insert(dumperData.first);
		}
	}


	for (const auto& dumperId : dumperIds)
	{
		msgUsers
			.composeString(dumperId)
			.composeUINT32(0)			// USERINTID, since we only need poker wm, no need for userintid
			;
		++userCount;
	}

	PLog("total #%u WM users", userCount);

	// Set fromTime and toTime to null for lifetime request
	SrvTime fromTime, toTime;

	CommMsgBody msgBody;
	msgBody
		.composeUINT32(WagerMeas::eProductTypePoker)
		.composeSrvTime(fromTime)
		.composeSrvTime(toTime)
		.composeUINT32(userCount);

	msgBody.merge(msgUsers);

	postToWagerMeas(WM_Q_GET_RM_WAGER_FR, msgBody, new TournGameRuleDataRequestCall(this, bankerTournRuleEvalData));
}

void ColCdaServerObject::gotTournGameWMData(CommMsgParser& parser, BankerTournRuleEvalData& bankerTournRuleEvalData)
{
	PLog("gotTournGameWMData - userId='%s'", bankerTournRuleEvalData.bankerName.c_str());

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	if (errCode)
	{
		const char* errStr;
		parser.parseString(errStr);
		PLog("DBM error[%d]-%s", errCode, errStr);

		return;
	}

	UserWMDataMap userWMDataMap;
	userWMDataMap.parse(parser);

	PLog("WM user size=%u", (UINT32)userWMDataMap.size());

	evalTournGameWMData(userWMDataMap, bankerTournRuleEvalData);

	if (!bankerTournRuleEvalData.hasQualifiedRules())
	{
		return;
	}

	// col raw data
	requestTournGamColRawData(bankerTournRuleEvalData);
}

void ColCdaServerObject::evalTournGameWMData(const UserWMDataMap& userWMDataMap, 
											 BankerTournRuleEvalData& bankerTournRuleEvalData)
{
	PLog("evalTournGameWMData - userId='%s'", bankerTournRuleEvalData.bankerName.c_str());

	UserWMDataMap::const_iterator bankerWMIt = userWMDataMap.find(bankerTournRuleEvalData.bankerName);
	if (bankerWMIt == userWMDataMap.end())
	{
		// continue evaluation
		PLog(LOG_TRACE_FAULTY_LOGIC "Failed to find user WM data ('%s')", bankerTournRuleEvalData.bankerName.c_str());
	}

	TournRuleEvalMap::iterator ruleIt = bankerTournRuleEvalData.tournRuleEvalMap.begin();
	while (ruleIt != bankerTournRuleEvalData.tournRuleEvalMap.end())
	{
		TournRuleEvalMap::iterator removeIt = ruleIt++;

		const TournRule& tournRule = removeIt->first;
		AggBankerTournData& aggBanker = removeIt->second.aggBankerTournData;

		PString ruleCur;
		tournRule.getRuleCurrency(ruleCur);

		//check banker poker WM
		if (bankerWMIt != userWMDataMap.end())
		{
			INT64 bankerPokerWM = convertUserWagerAmt(bankerWMIt->second.amtPokerPerCurr, ruleCur);
			if (!tournRule.bankerMPWCents.isInRange(bankerPokerWM))
			{
				if (moreTrace)
				{
					PLog("-failed to pass banker totWagerPoker=%llu", bankerPokerWM);
				}
				bankerTournRuleEvalData.tournRuleEvalMap.erase(removeIt);
				continue;
			}
		}

		// check dumper WM data
		AggBankerTournData::DumperDataMapIt dumperIt = aggBanker.aggDumperData.begin();
		while (dumperIt != aggBanker.aggDumperData.end())
		{
			AggBankerTournData::DumperDataMapIt removeDumperIt = dumperIt++;

			const char* dumperId = removeDumperIt->first;

			UserWMDataMap::const_iterator dumperWMIt = userWMDataMap.find(dumperId);
			if (dumperWMIt == userWMDataMap.end())
			{
				// continue evaluation. consider current user passed rules
				PLog(LOG_TRACE_FAULTY_LOGIC "Failed to find user WM data ('%s')", dumperId);
				continue;
			}

			INT64 dumperPokerWM = convertUserWagerAmt(dumperWMIt->second.amtPokerPerCurr, ruleCur);
			if (!tournRule.dumperMPWCents.isInRange(dumperPokerWM))
			{
				if (moreTrace)
				{
					PLog("-failed to pass dumper totWagerPoker=%llu", dumperPokerWM);
				}
				aggBanker.aggDumperData.erase(removeDumperIt);
				continue;
			}
		}

		if (!tournRule.dumperCount.isInRange((UINT32)aggBanker.aggDumperData.size()))
		{
			if (moreTrace)
			{
				PLog("-failed to pass dumperCount=%u", aggBanker.aggDumperData.size());
			}
			bankerTournRuleEvalData.tournRuleEvalMap.erase(removeIt);
			continue;
		}

		if (!tournRule.allDumpersWinrate_scaled100.isInRange(aggBanker.getAllDumperWinRate_scaled100()))
		{
			if (moreTrace)
			{
				PLog("-failed to pass allDumpersWinrate_scaled100=%u", aggBanker.getAllDumperWinRate_scaled100());
			}
			bankerTournRuleEvalData.tournRuleEvalMap.erase(removeIt);
			continue;
		}

		if (!tournRule.allDumpersROI_scaled100.isInRange(aggBanker.getAllDumperROI_scaled100()))
		{
			if (moreTrace)
			{
				PLog("-failed to pass allDumpersROI_scaled100=%d", aggBanker.getAllDumperROI_scaled100());
			}
			bankerTournRuleEvalData.tournRuleEvalMap.erase(removeIt);
			continue;
		}

	}
}


void ColCdaServerObject::requestTournGamColRawData(BankerTournRuleEvalData& bankerTournRuleEvalData)
{
	PLog("requestTournGamColRawData - userId='%s'", bankerTournRuleEvalData.bankerName.c_str());

	set<UINT32> lookbackMinutesSet;

	for (const auto& tournRuleEval : bankerTournRuleEvalData.tournRuleEvalMap)
	{
		lookbackMinutesSet.insert(tournRuleEval.first.lookbackPeriodMins);
	}

	UINT32 count = 0;
	CommMsgBody msgLookBack;

	for (const auto& lookBackMins : lookbackMinutesSet)
	{
		msgLookBack.composeUINT32(lookBackMins);
		++count;
	}

	CommMsgBody msgBody;
	msgBody
		.composeString(bankerTournRuleEvalData.bankerName)
		.composeUINT32(count);

	msgBody.merge(msgLookBack);

	postToColRawDbm(COLRAW_Q_GET_CCDA_TOURN_RULE_DATA, msgBody, new TournGameRuleDataRequestCall(this, bankerTournRuleEvalData));
}

void ColCdaServerObject::gotTournGameColRawData(CommMsgParser& parser, BankerTournRuleEvalData& bankerTournRuleEvalData)
{
	PLog("gotTournGameColRawData - userId='%s'", bankerTournRuleEvalData.bankerName.c_str());

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	if (errCode)
	{
		const char* errStr;
		parser.parseString(errStr);
		PLog("DBM error[%d]-%s", errCode, errStr);

		return;
	}

	TournRuleColDataMap tournGameRuleColDataMap;
	tournGameRuleColDataMap.parse(parser);

	evalTournGameColRawData(tournGameRuleColDataMap, bankerTournRuleEvalData);

	prepareCCDATournAlert(bankerTournRuleEvalData);
}

void ColCdaServerObject::prepareCCDATournAlert(BankerTournRuleEvalData& bankerTournRuleEvalData)
{
	PLog("prepareCCDATournAlert - userId='%s'", bankerTournRuleEvalData.bankerName.c_str());

	if (!bankerTournRuleEvalData.hasQualifiedRules())
	{
		return;
	}

	TournRuleEvalMap::iterator it = bankerTournRuleEvalData.tournRuleEvalMap.begin();

	const TournRule& tournRule = it->first;
	CCDAAlertData& alertData = it->second.alertData;
	alertData.appendRuleInfo(tournRule.ruleId, tournRule.ruleName);

	if (enableAlertAutoAction)
	{
		alertData.autoActions = tournRule.autoActions;
	}

	vector<PString> dumperUserIds;
	for (const auto& aggDumper : it->second.aggBankerTournData.aggDumperData)
	{
		dumperUserIds.push_back(aggDumper.first);
	}

	sendCCDAAlert(bankerTournRuleEvalData.bankerName, alertData, CCDA_RuleType_SNGHU, dumperUserIds);
}

void ColCdaServerObject::evalTournGameColRawData(const TournRuleColDataMap& tournRuleColDataMap, BankerTournRuleEvalData& bankerTournRuleEvalData)
{
	PLog("evalTournGameColRawData - userId='%s'", bankerTournRuleEvalData.bankerName.c_str());

	TournRuleEvalMap::iterator ruleIt = bankerTournRuleEvalData.tournRuleEvalMap.begin();
	while (ruleIt != bankerTournRuleEvalData.tournRuleEvalMap.end())
	{
		TournRuleEvalMap::iterator removeIt = ruleIt++;

		const TournRule& tournRule = removeIt->first;

		TournRuleColDataMap::const_iterator colIt = tournRuleColDataMap.find(tournRule.lookbackPeriodMins);
		if (colIt == tournRuleColDataMap.end())
		{
			PLog(LOG_TRACE_FAULTY_LOGIC "Failed to find colraw data lookbackPeriodMins=%u", tournRule.lookbackPeriodMins);
			continue;
		}

		const TournRuleColData& colData = colIt->second;
		INT64 totalProfit = convertAmountCurr(colData.totalProfit, "USD");
		if (!tournRule.bankerTotalProfitCents.isInRange(totalProfit))
		{
			if (moreTrace)
			{
				PLog("-failed to pass bankerTotalProfitCents=%lld", totalProfit);
			}
			bankerTournRuleEvalData.tournRuleEvalMap.erase(removeIt);
			continue;
		}

		INT64 totalBuyinRake = convertAmountCurr(colData.totalBuyinRake, "USD");
		if (!tournRule.bankerTotalBuyin.isInRange(totalBuyinRake))
		{
			if (moreTrace)
			{
				PLog("-failed to pass bankerTotalBuyin=%lld", totalBuyinRake);
			}
			bankerTournRuleEvalData.tournRuleEvalMap.erase(removeIt);
			continue;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ColCdaServerObject::getCurrentActiveRingRule(BankerRingGameEvalData& bankerRingGameEvalData, const SrvTime& evalTime)
{
	bankerRingGameEvalData.ringRuleEvalMap.clear();

	for (const auto& ringGameRule : ringGameRules)
	{
		if (ringGameRule.isActive == Rule_Active)
		{
			RingGameRule rule = ringGameRule;
			RingRuleData ruleData(evalTime.encode());
			bankerRingGameEvalData.ringRuleEvalMap.insert(make_pair(rule, ruleData));
		}
	}
}

void ColCdaServerObject::getCurrentActiveTournRule(BankerTournRuleEvalData& tournRuleEvalData, const SrvTime& evalTime)
{
	tournRuleEvalData.tournRuleEvalMap.clear();

	for (const auto& tournRule : sngHUGameRules)
	{
		if (tournRule.isActive == Rule_Active)
		{
			TournRule rule = tournRule;
			TournRuleData ruleData(evalTime.encode());
			tournRuleEvalData.tournRuleEvalMap.insert(make_pair(rule, ruleData));
		}
	}
}


void ColCdaServerObject::sendCCDAAlert(const PString& userId, 
									   const CCDAAlertData& alertData, 
									   CCDARuleType ruleType,
									   const vector<PString>& dumpers)
{
	PLog("sendCCDAAlert - userId='%s', CDH=%u, dumper=%u"
		, userId.c_str(), (UINT32)alertData.cdhMap.size(), (UINT32)dumpers.size());

	vector<SecAlertsAux2> alertAux2Vec;

	for (const auto& relatedPlayer : alertData.relatedPlayerMap)
	{
		SecAlertsAux2 alertsAux2;
		alertsAux2.fieldType = eSecAux_CCDA_RELATED_PLAYER;
		alertsAux2.strVal = relatedPlayer.first;
		alertsAux2.intVal = relatedPlayer.second;

		alertAux2Vec.push_back(alertsAux2);
	}

	// PYR-87907. only create one alert for each (banker,dumper) pair
	PStringMap<UINT64> cdaAlerts;
	for (const auto& cdhAlertEvent : alertData.cdhMap)
	{
		UINT64 handId = cdhAlertEvent.first;
		const CDHAlertInfo& cdhAlertInfo = cdhAlertEvent.second;

		SecAlertsAux2 alertsAux2;
		alertsAux2.fieldType = eSecAux_CCDA_CDH_HAND;
		cdhAlertInfo.toString(handId, alertsAux2.strVal);

		alertAux2Vec.push_back(alertsAux2);

		auto cdaAlertsIt = cdaAlerts.find(cdhAlertInfo.dumperUserId);
		if (cdaAlertsIt == cdaAlerts.end())
		{
			cdaAlerts.insert(make_pair(cdhAlertInfo.dumperUserId, handId));
		}
		else
		{
			cdaAlertsIt->second = handId;
		}
	}

	PLog("CDA alert num=%u", (UINT32)cdaAlerts.size());

	SrvTime now;
	now.currentLocalTime();

	if (cdaAlerts.size())
	{
		for (const auto& cdaAlert : cdaAlerts)
		{
			const char* dumperUserId = cdaAlert.first;
			UINT64 handId = cdaAlert.second;

			if (colWhitelistCache.isWhitelistPair(userId, dumperUserId, now))
			{
				PLog("skip whitelist pair(%s-%s)", userId.c_str(), dumperUserId);
				continue;
			}

			ColCDA::CCDAAlert alert(userId, dumperUserId, alertData.autoActions);

			CommMsgBody body;
			body.
				composeUINT32(eSecurityAlertColCDA).
				composeString(userId).
				composeString(dumperUserId).
				composeString(alertData.parameter).
				composeString("").
				composeUINT64(handId).
				composeUINT32(ruleType).
				composeINT32(alertData.autoActions).
				composeINT32(0);

			composeVector(body, alertAux2Vec);
			postToCollusion(COLLUSION_Q_CREATE_SEC_ALERT, body, new CommonDataRequestCall(this, alert));

		}
	}
	else
	{
		for (const auto& dumperId : dumpers)
		{
			if (colWhitelistCache.isWhitelistPair(userId, dumperId, now))
			{
				PLog("skip whitelist pair(%s-%s)", userId.c_str(), dumperId.c_str());
				continue;
			}

			ColCDA::CCDAAlert alert(userId, dumperId, alertData.autoActions);

			CommMsgBody body;
			body.
				composeUINT32(eSecurityAlertColCDA).
				composeString(userId).
				composeString(dumperId).
				composeString(alertData.parameter).
				composeString("").
				composeUINT64(0).
				composeUINT32(ruleType).
				composeINT32(alertData.autoActions).
				composeINT32(0);

			composeVector(body, alertAux2Vec);
			postToCollusion(COLLUSION_Q_CREATE_SEC_ALERT, body, new CommonDataRequestCall(this, alert));
		}
	}
}

void ColCdaServerObject::gotCreateAlertReply(CommMsgParser& parser, const ColCDA::CCDAAlert& alert)
{
	PLog("gotCreateAlertReply - userId='%s', dumperUserId='%s', aa=%d", 
		alert.bankerUserId.c_str(), alert.dumperUserId.c_str(), alert.autoActions);

	INT16 errCode = 0;
	parser.parseINT16(errCode);

	if (errCode)
	{
		const char* errStr;
		parser.parseString(errStr);
		PLog("DBM error[%d]-%s", errCode, errStr);

		return;
	}

	UINT32 groupId = 0;
	parser.parseUINT32(groupId);

	PLog("created groupId=%u", groupId);

	// GPSI-259
	SrvTime now;
	now.currentLocalTime();
	if (groupId)
	{
		addSRMStats(alert.autoActions, now, alert.bankerUserId, alert.dumperUserId);
	}
}

void ColCdaServerObject::addSRMStats(UINT32 autoActions, const SrvTime& srmTime, const char* userId, const char* dumperId)
{
	if (!velocityControlRule.isVelocityControlEnabled())
	{
		return;
	}

	if (autoActions & CCDA_AA_BANKER_SRM)
	{
		srmMonitorCache.add(srmTime, userId);
	}
	if (autoActions & CCDA_AA_DUMPER_SRM)
	{
		srmMonitorCache.add(srmTime, dumperId);
	}

	if (srmMonitorCache.isOverThreshold(velocityControlRule.getVelocityControlMaxSRMUsers()))
	{
		evaluationStatus = CCDA_EVAL_STATUS_STOPPED;
		processVelocityControlNotification();
	}
}

void ColCdaServerObject::startEvaluation()
{
	PLog("startEvaluation");

	srmMonitorCache.clear();
	CommMsgBody body;
	postTimerMessage(TIMER_EVAL_CACHE_USERS, body, 1);
}

bool ColCdaServerObject::shouldRunEvaluation() const
{
	return evaluationStatus == CCDA_EVAL_STATUS_RUNNING;
}