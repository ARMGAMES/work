////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2017 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef ColSrvStructs_h_included
#define ColSrvStructs_h_included

#include "ppinclude.h"
#include "ppstring.h"
#include "pyrtime.h"
#include "../ShdFreqPartners.h"
#include "HandStats.h"
#include "../shared/AmpsStructs.h"
#include "../shared/BPSShared.h"
#include "coldbm.h"

class ColUtilServerObject;

////////////////////////////////////////////////////////////////////////////////

namespace ColUtils
{
	// PYR-106157
	struct UserWageringLifetimeAggKey
	{
		PString userId;

		explicit UserWageringLifetimeAggKey(const PString& userId_);

		void appendLog(PString& logStr) const;
	};

	// PYR-106157
	struct UserWageringLifetimeAggKeyCmpLess
	{
		bool operator() (const UserWageringLifetimeAggKey& lhs, const UserWageringLifetimeAggKey& rhs) const;
	};

	// PYR-106157
	struct UserWageringLifetimeAggData
	{
		SrvDate lastDateCashGames;
		SrvDate lastDateTournaments;
		UserWageringEx userWagering;

		UserWageringLifetimeAggData();
		UserWageringLifetimeAggData(const SrvDate& lastDateCashGames_, const SrvDate& lastDateTournaments_, const UserWageringEx& userWagering_);
	};
}

////////////////////////////////////////////////////////////////////////////////

class FastReportItemBase
{
public:

	FastReportItemBase(UINT32 reqId_, UINT32 connId_, UINT32 replyId_, const char* adminId_, ColUtilServerObject& serverObj_)
		: uniqueId(PUINT32_MAX)
		, reqId(reqId_)
		, connId(connId_)
		, replyId(replyId_)
		, adminId(adminId_)
		, serverObj(serverObj_)
	{}

	virtual ~FastReportItemBase();

	virtual void process() = 0;
	virtual void log() const;
	virtual unique_ptr<FastReportItemBase> cloneAsBase() const = 0;

	UINT32 uniqueId;
	UINT32 reqId;
	UINT32 connId;
	UINT32 replyId;
	PString adminId;
	ColUtilServerObject& serverObj;
};

////////////////////////////////////////////////////////////////////////////////

class FastReportQueue
{
public:

	// PYR-79713. Report overtime 20 minutes
	const UINT32 REPORT_OVERTIME_SECONDS = 1200;

	FastReportQueue(UINT32 maxQueueSize_, UINT32 maxNumProcessing_)
		: maxQueueSize(maxQueueSize_)
		, maxNumProcessing(maxNumProcessing_)
		, numNowInProcess(0)
	{ }

	void clear();
	bool empty() const;
	bool eraseByUniqueId(UINT32 uniqueId);
	bool pushBack(unique_ptr<FastReportItemBase> reportParams);
	UINT32 size() const;
	void configure(UINT32 maxQueueSize_, UINT32 maxNumProcessing_);

	bool submitNext();
	void reportJobComplete(UINT32 uniqueId);
	void log() const;

	void addRunningReport(UINT32 uniqueId);
	void eraseRunningReport(UINT32 uniqueId);
	UINT32 getOvertimeReportNum();

private:

	typedef list<unique_ptr<FastReportItemBase>> ContainerList;
	typedef ContainerList::iterator Iterator;
	typedef ContainerList::const_iterator ConstIterator;
	typedef map<UINT32, UINT64> RunningReportMap;		// PYR-79713 <uniqueId, reportStartTime>

	// Configuration
	UINT32 maxQueueSize;
	UINT32 maxNumProcessing;

	// Current state
	UINT32 numNowInProcess;
	ContainerList reportQueue;
	RunningReportMap runningReportQueue;
};

////////////////////////////////////////////////////////////////////////////////

class ColShdFreqPartnersFastReport : public FastReportItemBase
{
public:
	ColShdFreqPartnersFastReport(UINT32 reqId, UINT32 connId, UINT32 replyId, const char* adminId, 
		ColUtilServerObject& serverObj, const ShdFreqPartnersInputs& inputs_)
		: FastReportItemBase(reqId, connId, replyId, adminId, serverObj)
		, inputs(inputs_)
	{ }
	virtual ~ColShdFreqPartnersFastReport() { }

	virtual void process() override;
	virtual void log() const override;
	virtual unique_ptr<FastReportItemBase> cloneAsBase() const override;

	void compose(CommMsgBody& body) const;

	ShdFreqPartnersInputs inputs;
};

// PYR-55192
class ColGetHandStatsFastReport : public FastReportItemBase
{
public:
	ColGetHandStatsFastReport(UINT32 reqId, UINT32 connId, UINT32 replyId, const char* adminId,
		ColUtilServerObject& serverObj, const GetHandStatsInputs& inputs_)
		: FastReportItemBase(reqId, connId, replyId, adminId, serverObj)
		, inputs(inputs_)
	{ }
	virtual ~ColGetHandStatsFastReport() { }

	virtual void process() override;
	virtual void log() const override;
	virtual unique_ptr<FastReportItemBase> cloneAsBase() const override;

	void compose(CommMsgBody& body) const;

	GetHandStatsInputs inputs;
};

// PYR-64045
struct ColGetShdHandsForAmpsFastReport : public FastReportItemBase
{
	ColGetShdHandsForAmpsFastReport(UINT32 reqId, UINT32 connId, UINT32 replyId, const char* adminId,
		ColUtilServerObject& serverObj, const ColAmps::GetShdHandsJobState& jobState_)
		: FastReportItemBase(reqId, connId, replyId, adminId, serverObj)
		, jobState(jobState_)
	{ }

	virtual ~ColGetShdHandsForAmpsFastReport() { }

	virtual void process() override;
	virtual void log() const override;
	virtual unique_ptr<FastReportItemBase> cloneAsBase() const override;

	ColAmps::GetShdHandsOutputs outputs;
	ColAmps::GetShdHandsJobState jobState;
};

// PYR-64045
struct AmpsTopPartnersFastReport : public FastReportItemBase
{
	AmpsTopPartnersFastReport(UINT32 reqId, UINT32 connId, UINT32 replyId, const char* adminId,
		ColUtilServerObject& serverObj, const ColAmps::TopPartnersJobState& jobState_)
		: FastReportItemBase(reqId, connId, replyId, adminId, serverObj)
		, jobState(jobState_)
	{ }

	virtual ~AmpsTopPartnersFastReport() { }

	virtual void process() override;
	virtual void log() const override;
	virtual unique_ptr<FastReportItemBase> cloneAsBase() const override;

	void composeJobState(CommMsgBody& body) const;
	bool isTimeout() const;

	ColAmps::TopPartnersJobState jobState;
};

// PYR-64045
struct AmpsGameActivityFastReport : public FastReportItemBase
{
	AmpsGameActivityFastReport(UINT32 reqId, UINT32 connId, UINT32 replyId, const char* adminId,
		ColUtilServerObject& serverObj, const ColAmps::GameActivityJobState& jobState_)
		: FastReportItemBase(reqId, connId, replyId, adminId, serverObj)
		, jobState(jobState_)
	{ }

	virtual ~AmpsGameActivityFastReport() { }

	virtual void process() override;
	virtual void log() const override;
	virtual unique_ptr<FastReportItemBase> cloneAsBase() const override;

	void composeJobState(CommMsgBody& body) const;
	bool isTimeout() const;

	ColAmps::GameActivityJobState jobState;
};

// PYR-64045
struct AmpsGamesTogetherFastReport : public FastReportItemBase
{
	AmpsGamesTogetherFastReport(UINT32 reqId, UINT32 connId, UINT32 replyId, const char* adminId,
		ColUtilServerObject& serverObj, const ColAmps::GamesTogetherJobState& jobState_)
		: FastReportItemBase(reqId, connId, replyId, adminId, serverObj)
		, jobState(jobState_)
	{ }

	virtual ~AmpsGamesTogetherFastReport() { }

	virtual void process() override;
	virtual void log() const override;
	virtual unique_ptr<FastReportItemBase> cloneAsBase() const override;

	void composeJobState(CommMsgBody& body) const;
	bool isTimeout() const;

	ColAmps::GamesTogetherJobState jobState;
};

// PYR-82878
struct ColCdhEventFastReport : public FastReportItemBase
{
public:
	ColCdhEventFastReport(UINT32 reqId, UINT32 connId, UINT32 replyId, const char* adminId,
		ColUtilServerObject& serverObj, const ColUtils::CdhEventReportFilterEx& inputs_)
		: FastReportItemBase(reqId, connId, replyId, adminId, serverObj)
		, inputs(inputs_)
	{ }
	virtual ~ColCdhEventFastReport() { }

	virtual void process() override;
	virtual void log() const override;
	virtual unique_ptr<FastReportItemBase> cloneAsBase() const override;

	void compose(CommMsgBody& body) const;

	ColUtils::CdhEventReportFilterEx inputs;
};

// PYR-89537
struct ColNormalBPSFastReport : public FastReportItemBase
{
public:
	ColNormalBPSFastReport(UINT32 reqId, UINT32 connId, UINT32 replyId, const char* adminId,
		ColUtilServerObject& serverObj, const BPS::BPSNormalFilter& inputs_, const ColUtils::ColAdminReport& adminReport_)
		: FastReportItemBase(reqId, connId, replyId, adminId, serverObj)
		, inputs(inputs_), adminReport(adminReport_)
	{ }
	virtual ~ColNormalBPSFastReport() { }

	virtual void process() override;
	virtual void log() const override;
	virtual unique_ptr<FastReportItemBase> cloneAsBase() const override;

	void compose(CommMsgBody& body) const;

	BPS::BPSNormalFilter inputs;
	ColUtils::ColAdminReport adminReport;
};

// PYR-89537
struct ColXStakesBPSFastReport : public FastReportItemBase
{
public:
	ColXStakesBPSFastReport(UINT32 reqId, UINT32 connId, UINT32 replyId, const char* adminId,
		ColUtilServerObject& serverObj, const BPS::BPSXStakesFilter& inputs_, const ColUtils::ColAdminReport& adminReport_)
		: FastReportItemBase(reqId, connId, replyId, adminId, serverObj)
		, inputs(inputs_), adminReport(adminReport_)
	{ }
	virtual ~ColXStakesBPSFastReport() { }

	virtual void process() override;
	virtual void log() const override;
	virtual unique_ptr<FastReportItemBase> cloneAsBase() const override;

	void compose(CommMsgBody& body) const;

	BPS::BPSXStakesFilter inputs;
	ColUtils::ColAdminReport adminReport;
};

// PYR-89537
struct ColBpsUserStakesPlayedFastReport : public FastReportItemBase
{
	ColBpsUserStakesPlayedFastReport(UINT32 reqId, UINT32 connId, UINT32 replyId, const char* adminId,
		ColUtilServerObject& serverObj, const BPS::UserStakesPlayedInputsEx& inputs_)
		: FastReportItemBase(reqId, connId, replyId, adminId, serverObj)
		, inputsEx(inputs_)
	{ }
	virtual ~ColBpsUserStakesPlayedFastReport() { }

	virtual void process() override;
	virtual void log() const override;
	virtual unique_ptr<FastReportItemBase> cloneAsBase() const override;

	void compose(CommMsgBody& body) const;

	BPS::UserStakesPlayedInputsEx inputsEx;
};

// PYR-89537
struct ColGetAggTableExtentsFastReport : public FastReportItemBase
{
	using PHandler = void (ColUtilServerObject::*)(const ColGetAggTableExtentsFastReport&);

	ColGetAggTableExtentsFastReport(UINT32 reqId, UINT32 connId, UINT32 replyId, const char* adminId,
		ColUtilServerObject& serverObj, const ColUtils::MaintJobSetupQueryParamsEx& inputs_);
	virtual ~ColGetAggTableExtentsFastReport() { }

	virtual void process() override;
	virtual void log() const override;
	virtual unique_ptr<FastReportItemBase> cloneAsBase() const override;

	void compose(CommMsgBody& body) const;

	PHandler requestProcessorFn;
	ColUtils::MaintJobSetupQueryParamsEx inputsEx;
};

////////////////////////////////////////////////////////////////////////////////

#endif // ColSrvStructs_h_included
