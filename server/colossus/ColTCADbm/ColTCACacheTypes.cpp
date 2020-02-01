// ===========================================================
// Copyright (C) 2018 PYR Software Ltd. All rights reserved.
// ===========================================================

#include "ColTCACacheTypes.h"

void TopPartnerTournSet::insertPartnerTourn(UINT32 totalTourn)
{
	// rule is disabled. no need to insert partner data.
	if (maxSize <= 0)
	{
		return;
	}

	insert(totalTourn);
	if (!empty() && size() >  maxSize)
	{
		erase(--end());
	}
}

double TopPartnerTournSet::getMedian() const
{
	double median = 0;
	if (size() == 0)
	{
		PLog("Warning - Top partner size is empty.");
		return PINT32_MAX;
	}

	iterator it = begin();
	UINT32 count = 0;
	while (it != end())
	{
		if (count >= size() / 2)
			break;
		++count;
		++it;
	}

	if (size() % 2 == 0)
	{
		UINT32 midr = *it;
		UINT32 midl = *(--it);
		median = (double)(midr + midl) / 2;
	}
	else
	{
		median = *it;
	}

	return median;
}

void UserColDataMap::insertPlayerData(const UserPartnerPair& pair, const ColPairData& colData, INT32 topPartnerNum)
{
	iterator it = find(pair.userId);
	if (it == end())
	{
		it = insert(value_type(pair.userId, UserColData(topPartnerNum, colData))).first;
	}
	else
	{
		it->second.numAllInConfrontations += colData.numAllInConfrontations;
		it->second.numPairTotalTourns += colData.totalTourns;
	}

	if (it != end())
	{
		it->second.topPartnerSet.insertPartnerTourn(colData.totalTourns);
	}

}

TCAColTaskData::TCAColTaskData()
	: reqId(0), connId(0)
{
	startRunningTime.currentLocalTime();
}

bool TCAColTaskData::isTaskRunOverTime()
{
	SrvTime now;
	now.currentLocalTime();

	if ((now.encode() - startRunningTime.encode()) > tcaConfig.maxRunTimeMinutes * SECONDS_IN_A_MINUTE)
	{
		return true;
	}
	return false;
}

bool TCAColTaskCache::add(UINT32 taskId, const TCAConfig& tcaConfig)
{
	auto taskIt = container.find(taskId);
	if (taskIt != container.end())
	{
		PLog("Error: task already in cache. Id=%u", taskId);
		return false;
	}

	TCAColTaskData tcaColTaskData;
	tcaColTaskData.tcaConfig = tcaConfig;

	container.insert(make_pair(taskId, tcaColTaskData));
	PLog("Added taskId=%u", taskId);
	return true;
}

void TCAColTaskCache::remove(UINT32 taskId)
{
	container.erase(taskId);
	PLog("Removed taskId=%u", taskId);
}