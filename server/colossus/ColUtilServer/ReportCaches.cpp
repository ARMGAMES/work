////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "ReportCaches.h"

////////////////////////////////////////////////////////////////////////////////

// PYR-64045
ReportCaches::ReportCaches() 
	: ampsShdHandsCache("AMPS_SHD_HANDS")
	, ampsTopSngPartnersCache("AMPS_TOP_SNG_PARTNERS")
	, ampsTopRingPartnersCache("AMPS_TOP_RING_PARTNERS")
	, ampsTopSngPartnersRootUserCache("AMPS_TOP_SNG_PARTNERS_ROOT_USER")
	, ampsTopRingPartnersRootUserCache("AMPS_TOP_RING_PARTNERS_ROOT_USER")
	, ampsSngGameActivityCache("AMPS_SNG_GAME_ACTIVITY")
	, ampsRingGameActivityCache("AMPS_RING_GAME_ACTIVITY")
	, ampsSngGamesTogetherCache("AMPS_SNG_GAMES_TOGETHER")
	, ampsRingGamesTogetherCache("AMPS_RING_GAMES_TOGETHER")
	, bpsUserStakesPlayedCache("BPS_USER_STAKES_PLAYED")
	, bpsReportCache("BPS_REPORT")
	, aggTableExtentsCache("AGG_TABLE_EXTENTS")
	, userWageringLifetimeRequestCache("USERWAGER_LIFETIME_REQUESTS")
{

}

// PYR-64045
void ReportCaches::sweep()
{
	ampsShdHandsCache.sweep();
	ampsTopSngPartnersCache.sweep();
	ampsTopRingPartnersCache.sweep();
	ampsTopSngPartnersRootUserCache.sweep();
	ampsTopRingPartnersRootUserCache.sweep();
	ampsSngGameActivityCache.sweep();
	ampsRingGameActivityCache.sweep();
	ampsSngGamesTogetherCache.sweep();
	ampsRingGamesTogetherCache.sweep();
	bpsUserStakesPlayedCache.sweep();
	bpsReportCache.sweep();
	aggTableExtentsCache.sweep();
	userWageringLifetimeRequestCache.sweep();
}

// PYR-64045
void ReportCaches::logStats() const
{
	ampsShdHandsCache.logStats();
	ampsTopSngPartnersCache.logStats();
	ampsTopRingPartnersCache.logStats();
	ampsTopSngPartnersRootUserCache.logStats();
	ampsTopRingPartnersRootUserCache.logStats();
	ampsSngGameActivityCache.logStats();
	ampsRingGameActivityCache.logStats();
	ampsSngGamesTogetherCache.logStats();
	ampsRingGamesTogetherCache.logStats();
	bpsUserStakesPlayedCache.logStats();
	bpsReportCache.logStats();
	aggTableExtentsCache.logStats();
	userWageringLifetimeRequestCache.logStats();
}

// PYR-64045
bool ReportCaches::getIfPresent(const ColAmps::GetShdHandsInputs& inputs, ColAmps::GetShdHandsOutputs& outputs)
{
	return ampsShdHandsCache.get(inputs, outputs);
}

// PYR-64045
void ReportCaches::putIfAbsent(const ColAmps::GetShdHandsInputs& inputs, const ColAmps::GetShdHandsOutputs& outputs)
{
	ampsShdHandsCache.putIfAbsent(inputs, outputs);
}

// PYR-64045
bool ReportCaches::getIfPresent(const ColAmps::TopPartnersInputs& inputs, ColAmps::TopSngPartnersCachedOutputs& outputs)
{
	return ampsTopSngPartnersCache.get(inputs, outputs);
}

// PYR-64045
void ReportCaches::putIfAbsent(const ColAmps::TopPartnersInputs& inputs, const ColAmps::TopSngPartnersCachedOutputs& outputs)
{
	ampsTopSngPartnersCache.putIfAbsent(inputs, outputs);
}

// PYR-64045
bool ReportCaches::getIfPresent(const ColAmps::TopPartnersInputs& inputs, ColAmps::TopRingPartnersCachedOutputs& outputs)
{
	return ampsTopRingPartnersCache.get(inputs, outputs);
}

// PYR-64045
void ReportCaches::putIfAbsent(const ColAmps::TopPartnersInputs& inputs, const ColAmps::TopRingPartnersCachedOutputs& outputs)
{
	ampsTopRingPartnersCache.putIfAbsent(inputs, outputs);
}

// PYR-75383
bool ReportCaches::getIfPresent(const ColAmps::TopPartnersInputs& inputs, ColAmps::TopSngPartners_RootUser& outputs)
{
	return ampsTopSngPartnersRootUserCache.get(inputs, outputs);
}

// PYR-75383
void ReportCaches::putIfAbsent(const ColAmps::TopPartnersInputs& inputs, const ColAmps::TopSngPartners_RootUser& outputs)
{
	ampsTopSngPartnersRootUserCache.putIfAbsent(inputs, outputs);
}

// PYR-75383
bool ReportCaches::getIfPresent(const ColAmps::TopPartnersInputs& inputs, ColAmps::TopRingPartners_RootUser& outputs)
{
	return ampsTopRingPartnersRootUserCache.get(inputs, outputs);
}

// PYR-75383
void ReportCaches::putIfAbsent(const ColAmps::TopPartnersInputs& inputs, const ColAmps::TopRingPartners_RootUser& outputs)
{
	ampsTopRingPartnersRootUserCache.putIfAbsent(inputs, outputs);
}

// PYR-64045
bool ReportCaches::getIfPresent(const ColAmps::GameActivityInputs& inputs, ColAmps::SngGameActivityCachedOutputs& outputs)
{
	return ampsSngGameActivityCache.get(inputs, outputs);
}

// PYR-64045
void ReportCaches::putIfAbsent(const ColAmps::GameActivityInputs& inputs, const ColAmps::SngGameActivityCachedOutputs& outputs)
{
	ampsSngGameActivityCache.putIfAbsent(inputs, outputs);
}

// PYR-64045
bool ReportCaches::getIfPresent(const ColAmps::GameActivityInputs& inputs, ColAmps::RingGameActivityCachedOutputs& outputs)
{
	return ampsRingGameActivityCache.get(inputs, outputs);
}

// PYR-64045
void ReportCaches::putIfAbsent(const ColAmps::GameActivityInputs& inputs, const ColAmps::RingGameActivityCachedOutputs& outputs)
{
	ampsRingGameActivityCache.putIfAbsent(inputs, outputs);
}

// PYR-64045
bool ReportCaches::getIfPresent(const ColAmps::GamesTogetherInputs& inputs, ColAmps::SngGamesTogetherOutputs& outputs)
{
	return ampsSngGamesTogetherCache.get(inputs, outputs);
}

// PYR-64045
void ReportCaches::putIfAbsent(const ColAmps::GamesTogetherInputs& inputs, const ColAmps::SngGamesTogetherOutputs& outputs)
{
	ampsSngGamesTogetherCache.putIfAbsent(inputs, outputs);
}

// PYR-64045
bool ReportCaches::getIfPresent(const ColAmps::GamesTogetherInputs& inputs, ColAmps::RingGamesTogetherOutputs& outputs)
{
	return ampsRingGamesTogetherCache.get(inputs, outputs);
}

// PYR-64045
void ReportCaches::putIfAbsent(const ColAmps::GamesTogetherInputs& inputs, const ColAmps::RingGamesTogetherOutputs& outputs)
{
	ampsRingGamesTogetherCache.putIfAbsent(inputs, outputs);
}

// PYR-89537
bool ReportCaches::getIfPresent(const BPS::UserStakesPlayedInputs& inputs, BPS::UserStakesPlayedOutputs& outputs)
{
	return bpsUserStakesPlayedCache.get(inputs, outputs);
}

// PYR-89537
void ReportCaches::putIfAbsent(const BPS::UserStakesPlayedInputs& inputs, const BPS::UserStakesPlayedOutputs& outputs)
{
	bpsUserStakesPlayedCache.putIfAbsent(inputs, outputs);
}

// PYR-89537
bool ReportCaches::getIfPresent(const BPS::BPSCommonFilter& inputs, BPS::BPSReportCacheData& outputs)
{
	ColUtils::ColDBMUserBPSRequest key(inputs.userId, inputs.startDate, inputs.endDate, inputs.aggType);
	return bpsReportCache.get(key, outputs);
}

// PYR-89537
void ReportCaches::putIfAbsent(const BPS::BPSCommonFilter& inputs, const BPS::BPSReportCacheData& outputs)
{
	ColUtils::ColDBMUserBPSRequest key(inputs.userId, inputs.startDate, inputs.endDate, inputs.aggType);
	bpsReportCache.putIfAbsent(key, outputs);
}

// PYR-89537
bool ReportCaches::getIfPresent(const ColUtils::MaintJobSetupQueryParams& inputs, ColUtils::MaintJobSetupEntries& outputs)
{
	return aggTableExtentsCache.get(inputs, outputs);
}

// PYR-89537
void ReportCaches::putIfAbsent(const ColUtils::MaintJobSetupQueryParams& inputs, const ColUtils::MaintJobSetupEntries& outputs)
{
	aggTableExtentsCache.putIfAbsent(inputs, outputs);
}

// PYR-106157
bool ReportCaches::getIfPresent(const ColUtils::UserWageringLifetimeAggKey& inputs, ColUtils::UserWageringLifetimeAggData& outputs)
{
	return userWageringLifetimeRequestCache.get(inputs, outputs);
}

// PYR-106157
void ReportCaches::putIfAbsent(const ColUtils::UserWageringLifetimeAggKey& inputs, const ColUtils::UserWageringLifetimeAggData& outputs)
{
	userWageringLifetimeRequestCache.putIfAbsent(inputs, outputs);
}

////////////////////////////////////////////////////////////////////////////////
