////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2017 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "ColSrvStructs.h"
#include "composecolls.h"
#include "shared.h"
#include "ColSrvObject.h"

using ColUtils::UserWageringLifetimeAggKey;
using ColUtils::UserWageringLifetimeAggKeyCmpLess;
using ColUtils::UserWageringLifetimeAggData;

static UINT32 NEXT_FAST_RPT_QUEUE_ID = 1;

////////////////////////////////////////////////////////////////////////////////

// PYR-106157
UserWageringLifetimeAggKey::UserWageringLifetimeAggKey(const PString& userId_)
	: userId(userId_)
{
}

// PYR-106157
void UserWageringLifetimeAggKey::appendLog(PString& logStr) const
{
	logStr.append("[uid: '").append(userId).append("']");
}

// PYR-106157
bool ColUtils::UserWageringLifetimeAggKeyCmpLess::operator()(const UserWageringLifetimeAggKey& lhs, const UserWageringLifetimeAggKey& rhs) const
{
	static PStringCmp stringComparatorLess;

	if (stringComparatorLess(lhs.userId, rhs.userId))
	{
		return true;
	}
	if (stringComparatorLess(rhs.userId, lhs.userId))
	{
		return false;
	}

	// Both user ids equal

	return false;
}

// PYR-106157
UserWageringLifetimeAggData::UserWageringLifetimeAggData()
{
}

// PYR-106157
UserWageringLifetimeAggData::UserWageringLifetimeAggData(const SrvDate& lastDateCashGames_, const SrvDate& lastDateTournaments_, const UserWageringEx& userWagering_)
	: lastDateCashGames(lastDateCashGames_)
	, lastDateTournaments(lastDateTournaments_)
	, userWagering(userWagering_)
{
}

////////////////////////////////////////////////////////////////////////////////

// PYR-64045
FastReportItemBase::~FastReportItemBase()
{
	try
	{
		PLog("~FastReportItemBase [uniqueId: %u]", uniqueId);
	}
	catch (...)
	{

	}
}

void FastReportItemBase::log() const
{
	PLog("FastReportItemBase [uniqueId: %u, reqId: %u, connId: %08X, replyId: %u, adminId: %s]", uniqueId, reqId, connId, replyId, adminId.c_str());
}

////////////////////////////////////////////////////////////////////////////////

void FastReportQueue::clear()
{
	PLog("FastReportQueue::clear()");

	reportQueue.clear();
}

bool FastReportQueue::empty() const
{
	return reportQueue.empty();
}

// Returns true if a job with the given uniqueId was erased, false otherwise.
bool FastReportQueue::eraseByUniqueId(UINT32 uniqueId)
{
	for (Iterator it = reportQueue.begin(); it != reportQueue.end(); ++it)
	{
		unique_ptr<FastReportItemBase>& reportParams = *it;
		if (reportParams->uniqueId == uniqueId)
		{
			reportQueue.erase(it);
			return true;
		}
	}

	return false;
}

// Returns true if a job was added to the queue, false otherwise
bool FastReportQueue::pushBack(unique_ptr<FastReportItemBase> reportParams)
{
	if (size() >= maxQueueSize)
	{
		return false;
	}

	reportParams->uniqueId = NEXT_FAST_RPT_QUEUE_ID;
	++NEXT_FAST_RPT_QUEUE_ID;

	reportQueue.push_back(std::move(reportParams));

	return true;
}

UINT32 FastReportQueue::size() const
{
	return reportQueue.size();
}

void FastReportQueue::configure(UINT32 maxQueueSize_, UINT32 maxNumProcessing_)
{
	PLog("FastReportQueue::configure(maxQueueSize: %u, maxNumProcessing: %u)", maxQueueSize_, maxNumProcessing_);
	
	clear();

	maxQueueSize = maxQueueSize_;
	maxNumProcessing = maxNumProcessing_;
}

// Returns true if a job was "submitted", false otherwise
bool FastReportQueue::submitNext()
{
	PLog("FastReportQueue::submitNext");

	if (numNowInProcess >= maxNumProcessing)
	{
		PLog("Too many submitted");
		return false;
	}

	if (empty())
	{
		PLog("Nothing to submit");
		return false;
	}

	Iterator it = reportQueue.begin();
	unique_ptr<FastReportItemBase>& reportParams = *it;

	++numNowInProcess;

	addRunningReport(reportParams->uniqueId);

	reportParams->process();

	eraseByUniqueId(reportParams->uniqueId);

	return true;
}

void FastReportQueue::reportJobComplete(UINT32 uniqueId)
{
	--numNowInProcess;

	log();

	eraseRunningReport(uniqueId);
	submitNext();
}

void FastReportQueue::log() const
{
	PString buf("FastReportQueue [");
	buf.append("maxQueueSize: ").appendUint(maxQueueSize);
	buf.append(", maxNumProcessing: ").appendUint(maxNumProcessing);
	buf.append(", numNowInProcess: ").appendUint(numNowInProcess);
	buf.append(", numInQueue: ").appendUint(reportQueue.size());
	buf.append("]");
	PLog("%s", buf.c_str());

	if (reportQueue.empty())
	{
		return;
	}

	PLog("FastReportQueue items:");
	for (const unique_ptr<FastReportItemBase>& reportItem : reportQueue)
	{
		if (!reportItem)
		{
			PLog(LOG_TRACE_FAULTY_LOGIC " this item does not exist");
			continue;
		}
			
		reportItem->log();
	}
}

void FastReportQueue::addRunningReport(UINT32 uniqueId)
{
	PLog("[+]RR-%u", uniqueId);

	SrvTime now;
	now.currentLocalTime();
	runningReportQueue.insert(RunningReportMap::value_type(uniqueId, now.encode()));
}

void FastReportQueue::eraseRunningReport(UINT32 uniqueId)
{
	PLog("[-]RR-%u", uniqueId);
	runningReportQueue.erase(uniqueId);
}

UINT32 FastReportQueue::getOvertimeReportNum()
{
	UINT32 count = 0;
	SrvTime now;
	now.currentLocalTime();
	UINT64 thresholdTime = now.encode() - REPORT_OVERTIME_SECONDS;
	for (auto& entry : runningReportQueue)
	{
		UINT64 reportStartTime = entry.second;
		if (reportStartTime < thresholdTime)
		{
			++count;
		}
	}

	return count;
}

////////////////////////////////////////////////////////////////////////////////

void ColShdFreqPartnersFastReport::process()
{
	serverObj.processGetShdFreqPartnersReport_Submitted(*this);
}

void ColShdFreqPartnersFastReport::log() const
{
	FastReportItemBase::log();

	PLog("Job type: ColShdFreqPartnersFastReport");

	inputs.log();
}

unique_ptr<FastReportItemBase> ColShdFreqPartnersFastReport::cloneAsBase() const
{
	return make_unique<ColShdFreqPartnersFastReport>(*this);
}

void ColShdFreqPartnersFastReport::compose(CommMsgBody& body) const
{
	inputs.compose(body);
}

////////////////////////////////////////////////////////////////////////////////

void ColGetHandStatsFastReport::process()
{
	serverObj.processGetStatsForHand_Submitted(*this);
}

void ColGetHandStatsFastReport::log() const 
{
	FastReportItemBase::log();

	PLog("Job type: ColGetHandStatsFastReport");

	PString buf("Hand Id: ");
	buf.appendUint64(inputs.handId);
	PLog("%s", buf.c_str());
}

unique_ptr<FastReportItemBase> ColGetHandStatsFastReport::cloneAsBase() const
{
	return make_unique<ColGetHandStatsFastReport>(*this);
}

void ColGetHandStatsFastReport::compose(CommMsgBody& body) const
{
	inputs.compose(body);
}

////////////////////////////////////////////////////////////////////////////////

// PYR-64045
void ColGetShdHandsForAmpsFastReport::process()
{
	serverObj.processGetShdHandsForAmps_Submitted(*this);
}

// PYR-64045
void ColGetShdHandsForAmpsFastReport::log() const 
{
	FastReportItemBase::log();

	PLog("Job type: ColGetShdHandsForAmpsFastReport");
	jobState.inputs.log();
}

unique_ptr<FastReportItemBase> ColGetShdHandsForAmpsFastReport::cloneAsBase() const
{
	return make_unique<ColGetShdHandsForAmpsFastReport>(*this);
}

////////////////////////////////////////////////////////////////////////////////

// PYR-64045
void AmpsTopPartnersFastReport::process()
{
	serverObj.processAmpsTopPartners_Submitted(*this);
}

// PYR-64045
void AmpsTopPartnersFastReport::log() const 
{
	FastReportItemBase::log();

	PLog("Job type: AmpsTopPartnersFastReport");
	jobState.aggInputs.log();
}

unique_ptr<FastReportItemBase> AmpsTopPartnersFastReport::cloneAsBase() const
{
	return make_unique<AmpsTopPartnersFastReport>(*this);
}

// PYR-64045
void AmpsTopPartnersFastReport::composeJobState(CommMsgBody& body) const
{
	jobState.compose(body);
}

// PYR-64045
bool AmpsTopPartnersFastReport::isTimeout() const
{
	return jobState.isTimeout();
}

////////////////////////////////////////////////////////////////////////////////

// PYR-64045
void AmpsGameActivityFastReport::process()
{
	serverObj.processAmpsGameActivity_Submitted(*this);
}

// PYR-64045
void AmpsGameActivityFastReport::log() const 
{
	FastReportItemBase::log();

	PLog("Job type: AmpsGameActivityFastReport");
	jobState.aggInputs.log();
}

unique_ptr<FastReportItemBase> AmpsGameActivityFastReport::cloneAsBase() const
{
	return make_unique<AmpsGameActivityFastReport>(*this);
}

// PYR-64045
void AmpsGameActivityFastReport::composeJobState(CommMsgBody& body) const
{
	jobState.compose(body);
}

// PYR-64045
bool AmpsGameActivityFastReport::isTimeout() const
{
	return jobState.isTimeout();
}

////////////////////////////////////////////////////////////////////////////////

// PYR-64045
void AmpsGamesTogetherFastReport::process()
{
	serverObj.processAmpsGamesTogether_Submitted(*this);
}

// PYR-64045
void AmpsGamesTogetherFastReport::log() const 
{
	FastReportItemBase::log();

	PLog("Job type: AmpsGamesTogetherFastReport");
	jobState.log();
}

unique_ptr<FastReportItemBase> AmpsGamesTogetherFastReport::cloneAsBase() const
{
	return make_unique<AmpsGamesTogetherFastReport>(*this);
}

// PYR-64045
void AmpsGamesTogetherFastReport::composeJobState(CommMsgBody& body) const
{
	jobState.compose(body);
}

// PYR-64045
bool AmpsGamesTogetherFastReport::isTimeout() const
{
	return jobState.isTimeout();
}

////////////////////////////////////////////////////////////////////////////////

// PYR-82878
void ColCdhEventFastReport::process()
{
	serverObj.processGetCdhEventReport_Submitted(*this);
}

// PYR-82878
void ColCdhEventFastReport::log() const
{
	FastReportItemBase::log();

	PLog("Job type: ColCdhEventFastReport");
	inputs.log();
}

// PYR-82878
std::unique_ptr<FastReportItemBase> ColCdhEventFastReport::cloneAsBase() const
{
	return make_unique<ColCdhEventFastReport>(*this);
}

// PYR-82878
void ColCdhEventFastReport::compose(CommMsgBody& body) const
{
	inputs.compose(body);
}

////////////////////////////////////////////////////////////////////////////////

void ColNormalBPSFastReport::process()
{
	serverObj.processGetNormalBPSReport_Submitted(*this);

}

void ColNormalBPSFastReport::log() const
{
	FastReportItemBase::log();

	PLog("Job type: ColNormalBPSFastReport");
	inputs.log();
}

std::unique_ptr<FastReportItemBase> ColNormalBPSFastReport::cloneAsBase() const
{
	return make_unique<ColNormalBPSFastReport>(*this);
}

void ColNormalBPSFastReport::compose(CommMsgBody& body) const
{
	inputs.compose(body);
}

void ColXStakesBPSFastReport::process()
{
	serverObj.processGetXStakesBPSReport_Submitted(*this);
}

void ColXStakesBPSFastReport::log() const
{
	FastReportItemBase::log();

	PLog("Job type: ColXStakesBPSFastReport");
	inputs.log();
}

std::unique_ptr<FastReportItemBase> ColXStakesBPSFastReport::cloneAsBase() const
{
	return make_unique<ColXStakesBPSFastReport>(*this);
}

void ColXStakesBPSFastReport::compose(CommMsgBody& body) const
{
	inputs.compose(body);
}
////////////////////////////////////////////////////////////////////////////////

// PYR-89537
void ColBpsUserStakesPlayedFastReport::process()
{
	serverObj.processGetBpsUserStakedPlayedReport_Submitted(*this);
}

// PYR-89537
void ColBpsUserStakesPlayedFastReport::log() const
{
	FastReportItemBase::log();

	PLog("Job type: ColBpsUserStakesPlayedFastReport");
	inputsEx.log();
}

// PYR-89537
std::unique_ptr<FastReportItemBase> ColBpsUserStakesPlayedFastReport::cloneAsBase() const
{
	return make_unique<ColBpsUserStakesPlayedFastReport>(*this);
}

// PYR-89537
void ColBpsUserStakesPlayedFastReport::compose(CommMsgBody& body) const
{
	inputsEx.compose(body);
}

////////////////////////////////////////////////////////////////////////////////

// PYR-89537
ColGetAggTableExtentsFastReport::ColGetAggTableExtentsFastReport(UINT32 reqId, UINT32 connId, UINT32 replyId, const char* adminId, ColUtilServerObject& serverObj, const ColUtils::MaintJobSetupQueryParamsEx& inputs_) 
	: FastReportItemBase(reqId, connId, replyId, adminId, serverObj)
	, requestProcessorFn(&ColUtilServerObject::processGetAggTableExtentsReport_Submitted)
	, inputsEx(inputs_)
{
	
}

// PYR-89537
void ColGetAggTableExtentsFastReport::process()
{
	(serverObj.*requestProcessorFn)(*this);
}

// PYR-89537
void ColGetAggTableExtentsFastReport::log() const
{
	FastReportItemBase::log();

	PLog("Job type: ColGetAggTableExtentsFastReport");
	inputsEx.log();
}

// PYR-89537
std::unique_ptr<FastReportItemBase> ColGetAggTableExtentsFastReport::cloneAsBase() const
{
	return make_unique<ColGetAggTableExtentsFastReport>(*this);
}

// PYR-89537
void ColGetAggTableExtentsFastReport::compose(CommMsgBody& body) const
{
	inputsEx.compose(body);
}

////////////////////////////////////////////////////////////////////////////////
