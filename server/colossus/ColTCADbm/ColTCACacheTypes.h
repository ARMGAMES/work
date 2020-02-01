// ===========================================================
// Copyright (C) 2018 PYR Software Ltd. All rights reserved.
// ===========================================================

#ifndef ColTCACacheTypes_h_included
#define ColTCACacheTypes_h_included

#include "ppstring.h"
#include "pyrtime.h"
#include "TCADetector.h"
#include "CopyablePStringMap.h"
#include "../DateExtents.h"

class TopPartnerTournSet : public multiset<UINT32, std::greater<UINT32>>
{
	INT32 maxSize;

public:
	TopPartnerTournSet(INT32 maxSize_) : maxSize(maxSize_)
	{}

	void insertPartnerTourn(UINT32 totalTourn);
	double getMedian() const;
};

// only store col and colraw related data.
struct UserColData
{
	UINT32 numAllInConfrontations;
	UINT32 numPairTotalTourns;
	TopPartnerTournSet topPartnerSet;

	UserColData(INT32 topPartnerSize, const ColPairData& colPairData)
		: topPartnerSet(topPartnerSize),
		numAllInConfrontations(colPairData.numAllInConfrontations),
		numPairTotalTourns(colPairData.totalTourns)
	{}
};

class UserColDataMap : public PStringMap<UserColData>
{
public:
	UserColDataMap() {}

	void insertPlayerData(const UserPartnerPair& pair, const ColPairData& userData, INT32 topPartnerNum);
};

struct TCAColTaskData
{
	SrvTime startRunningTime;
	UINT32 reqId;
	UINT32 connId;
	SrvDate lastTournROIAggDate;
	SrvDate lastTournPartnerAggDate;
	GetDateExtentsOutputs tournROIExtentOutputs;
	GetDateExtentsOutputs tournPartnerExtentOutputs;
	
	PString taskDetails;
	TCAConfig tcaConfig;

	UserTournDataMap userTournDataMap;
	ColPairMap colRawPairMap;
	ColPairMap colAggPairMap;

	TCAAlertDataMap tcaAlertMap;

	TCAColTaskData();

	bool isTaskRunOverTime();
};

struct TCAColTaskCache
{
	typedef map<UINT32 /*taskId*/, TCAColTaskData> ContainerMap;

	bool add(UINT32 taskId, const TCAConfig& tcaConfig);
	void remove(UINT32 taskId);
	ContainerMap container;
};

#endif