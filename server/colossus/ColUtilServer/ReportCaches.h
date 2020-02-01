////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef ReportCaches_h_included
#define ReportCaches_h_included

////////////////////////////////////////////////////////////////////////////////

#include "ReportCacheTemplates.h"
#include "../shared/AmpsStructs.h"
#include "../shared/BPSShared.h"
#include "ColSrvStructs.h"

////////////////////////////////////////////////////////////////////////////////

// PYR-64045
class ReportCaches
{
	ReportCache<ColAmps::GetShdHandsInputs, ColAmps::GetShdHandsOutputs, ColAmps::GetShdHandsInputsCmpLess> ampsShdHandsCache;
	ReportCache<ColAmps::TopPartnersInputs, ColAmps::TopSngPartnersCachedOutputs, ColAmps::TopPartnersInputsCmpLess> ampsTopSngPartnersCache;
	ReportCache<ColAmps::TopPartnersInputs, ColAmps::TopRingPartnersCachedOutputs, ColAmps::TopPartnersInputsCmpLess> ampsTopRingPartnersCache;
	ReportCache<ColAmps::TopPartnersInputs, ColAmps::TopSngPartners_RootUser, ColAmps::TopPartnersInputsCmpLess> ampsTopSngPartnersRootUserCache;
	ReportCache<ColAmps::TopPartnersInputs, ColAmps::TopRingPartners_RootUser, ColAmps::TopPartnersInputsCmpLess> ampsTopRingPartnersRootUserCache;
	ReportCache<ColAmps::GameActivityInputs, ColAmps::SngGameActivityCachedOutputs, ColAmps::GameActivityInputsCmpLess> ampsSngGameActivityCache;
	ReportCache<ColAmps::GameActivityInputs, ColAmps::RingGameActivityCachedOutputs, ColAmps::GameActivityInputsCmpLess> ampsRingGameActivityCache;
	ReportCache<ColAmps::GamesTogetherInputs, ColAmps::SngGamesTogetherOutputs, ColAmps::GamesTogetherInputsCmpLess> ampsSngGamesTogetherCache;
	ReportCache<ColAmps::GamesTogetherInputs, ColAmps::RingGamesTogetherOutputs, ColAmps::GamesTogetherInputsCmpLess> ampsRingGamesTogetherCache;
	ReportCache<BPS::UserStakesPlayedInputs, BPS::UserStakesPlayedOutputs, BPS::UserStakesPlayedInputsCmpLess> bpsUserStakesPlayedCache;
	ReportCache<ColUtils::ColDBMUserBPSRequest, BPS::BPSReportCacheData, ColUtils::ColDBMUserBPSRequestCmpLess> bpsReportCache;
	ReportCache<ColUtils::MaintJobSetupQueryParams, ColUtils::MaintJobSetupEntries, ColUtils::MaintJobSetupQueryParamsCmpLess> aggTableExtentsCache;
	ReportCache<ColUtils::UserWageringLifetimeAggKey, ColUtils::UserWageringLifetimeAggData, ColUtils::UserWageringLifetimeAggKeyCmpLess> userWageringLifetimeRequestCache;

public:

	ReportCaches();

	void sweep();
	void logStats() const;
	
	bool getIfPresent(const ColAmps::GetShdHandsInputs& inputs, ColAmps::GetShdHandsOutputs& outputs);
	void putIfAbsent(const ColAmps::GetShdHandsInputs& inputs, const ColAmps::GetShdHandsOutputs& outputs);

	bool getIfPresent(const ColAmps::TopPartnersInputs& inputs, ColAmps::TopSngPartnersCachedOutputs& outputs);
	void putIfAbsent(const ColAmps::TopPartnersInputs& inputs, const ColAmps::TopSngPartnersCachedOutputs& outputs);

	bool getIfPresent(const ColAmps::TopPartnersInputs& inputs, ColAmps::TopRingPartnersCachedOutputs& outputs);
	void putIfAbsent(const ColAmps::TopPartnersInputs& inputs, const ColAmps::TopRingPartnersCachedOutputs& outputs);

	bool getIfPresent(const ColAmps::TopPartnersInputs& inputs, ColAmps::TopSngPartners_RootUser& outputs);
	void putIfAbsent(const ColAmps::TopPartnersInputs& inputs, const ColAmps::TopSngPartners_RootUser& outputs);

	bool getIfPresent(const ColAmps::TopPartnersInputs& inputs, ColAmps::TopRingPartners_RootUser& outputs);
	void putIfAbsent(const ColAmps::TopPartnersInputs& inputs, const ColAmps::TopRingPartners_RootUser& outputs);

	bool getIfPresent(const ColAmps::GameActivityInputs& inputs, ColAmps::SngGameActivityCachedOutputs& outputs);
	void putIfAbsent(const ColAmps::GameActivityInputs& inputs, const ColAmps::SngGameActivityCachedOutputs& outputs);

	bool getIfPresent(const ColAmps::GameActivityInputs& inputs, ColAmps::RingGameActivityCachedOutputs& outputs);
	void putIfAbsent(const ColAmps::GameActivityInputs& inputs, const ColAmps::RingGameActivityCachedOutputs& outputs);

	bool getIfPresent(const ColAmps::GamesTogetherInputs& inputs, ColAmps::SngGamesTogetherOutputs& outputs);
	void putIfAbsent(const ColAmps::GamesTogetherInputs& inputs, const ColAmps::SngGamesTogetherOutputs& outputs);

	bool getIfPresent(const ColAmps::GamesTogetherInputs& inputs, ColAmps::RingGamesTogetherOutputs& outputs);
	void putIfAbsent(const ColAmps::GamesTogetherInputs& inputs, const ColAmps::RingGamesTogetherOutputs& outputs);

	bool getIfPresent(const BPS::UserStakesPlayedInputs& inputs, BPS::UserStakesPlayedOutputs& outputs);
	void putIfAbsent(const BPS::UserStakesPlayedInputs& inputs, const BPS::UserStakesPlayedOutputs& outputs);

	bool getIfPresent(const BPS::BPSCommonFilter& inputs, BPS::BPSReportCacheData& outputs);
	void putIfAbsent(const BPS::BPSCommonFilter& inputs, const BPS::BPSReportCacheData& outputs);

	bool getIfPresent(const ColUtils::MaintJobSetupQueryParams& inputs, ColUtils::MaintJobSetupEntries& outputs);
	void putIfAbsent(const ColUtils::MaintJobSetupQueryParams& inputs, const ColUtils::MaintJobSetupEntries& outputs);

	bool getIfPresent(const ColUtils::UserWageringLifetimeAggKey& inputs, ColUtils::UserWageringLifetimeAggData& outputs);
	void putIfAbsent(const ColUtils::UserWageringLifetimeAggKey& inputs, const ColUtils::UserWageringLifetimeAggData& outputs);
};

////////////////////////////////////////////////////////////////////////////////

#endif // ReportCaches_h_included
