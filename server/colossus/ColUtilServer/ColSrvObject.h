////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2017 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef ColSrvobject_h_included
#define ColSrvobject_h_included

////////////////////////////////////////////////////////////////////////////////

#include "commsrv.h"
#include "CommonAsyncCall.h"
#include "ColSrvStructs.h"
#include "counters.h"
#include "TCADetector.h"
#include "ColCda.h"
#include "ReportCaches.h"
#include "sharedconversion.h"
#include "../shared/ColWhitelist.h"
#include "../shared/BPSShared.h"
#include "../shared/AccumulatorShared.h"
#include "coldbm.h"

class ColUtilServerObject;

#define LOGGER_TIMER					(100)
#define RPT_CACHES_TIMER				(101)
#define CONV_RATE_TIMER					(102)
#define TIMER_LOGGER_MSEC				(5000)
#define TIMER_RPT_CACHES_MSEC			(30000)
#define CONV_RATE_MSEC					(60*60*1000) // 1 Hour
#define CONV_RATE_DEFAULT_CURRENCY		"USD"

//////////////////////////////////////////////////////////////////////////////////

enum ServerConnectionType
{
	LowerGuard = 0,
	Admin, TrustedAdmin, Internal,
	UpperGuard
};

////////////////////////////////////////////////////////////////////////////////

class ColUtilServerConnection : public CommServerConnection
{
protected:
	ColUtilServerObject& obj;
	ServerConnectionType connectionType;

public:
	ColUtilServerConnection(ColUtilServerObject& obj_, ServerConnectionType connectionType_)
		: obj(obj_), connectionType(connectionType_)
	{ }

	void processMessage(UINT32 msgId, const CommMsgBody& body) override;
};

////////////////////////////////////////////////////////////////////////////////

class ColUtilSrvPrivilegedConnection : public ColUtilServerConnection
{
protected:
	ColUtilSrvPrivilegedConnection(ColUtilServerObject& obj_, ServerConnectionType connectionType_) 
		: ColUtilServerConnection(obj_, connectionType_), userIntId(0)
	{ }

public:
	const char* getAdminId() const							{ return adminId.c_str(); }

	UINT32 getUserIntId() const								{ return userIntId; }
	void setUserIntId(UINT32 intId)							{ userIntId = intId; }

	void addAdminRight(char const* right)					{ adminRights.insert(right); }
	bool checkAdminRight(char const* right) const			{ return adminRights.find(right) != adminRights.end(); }

	ColUtilServerObject& getColUtilServerObject() { return obj; }

protected:
	PString adminId;
	UINT32 userIntId;
	PStringSet adminRights;
};

////////////////////////////////////////////////////////////////////////////////

class AdminSrvCon : public ColUtilSrvPrivilegedConnection
{
public:
	explicit AdminSrvCon(ColUtilServerObject& obj_) :
		ColUtilSrvPrivilegedConnection(obj_, Admin), authError(0)
	{ }

	void connected() override;
	void closedOrDisconnected(int errCode, const char* errMsg) override;
	void saveAuthError(INT16 errCode) { authError = errCode; }
	void getUserAuth(const char* guardType, const char* user, CommMsgBody& clientExtra, CommMsgBody& authBody) override;

private:
	INT16 authError;
};

////////////////////////////////////////////////////////////////////////////////

class TrustedAdminSrvConn : public ColUtilSrvPrivilegedConnection
{
public:
	explicit TrustedAdminSrvConn(ColUtilServerObject& obj_) :
		ColUtilSrvPrivilegedConnection(obj_, TrustedAdmin)
	{ }

	void parseInfo(CommMsgParser& parser);
};

////////////////////////////////////////////////////////////////////////////////

class ColUtilSrvCliGConn : public CommClientGConnection
{
public:
	ColUtilSrvCliGConn(ColUtilServerObject& obj_, CommClientGuardFactory& guardFactory, char const* name_)
		: CommClientGConnection(guardFactory)
		, obj(obj_)
		, name(name_)
		, isconnected(0)
	{ }

	const char* getName() const { return name; }

	virtual void connected() override;
	virtual bool closedOrDisconnected(int errCode, const char* errMsg) override;
	virtual UINT32 post(UINT32 msgId, CommMsgBody& body, AsyncCall* call) override;
	virtual void processGMessage(UINT32 reqId, UINT32 msgId, const CommMsgBody& body, AsyncCall* call) override;

	int isConnected() const { return isconnected; }

protected:
	ColUtilServerObject& obj;
	PString name;
	int isconnected;

	UINT32 getConnId() const;
};

////////////////////////////////////////////////////////////////////////////////

// PYR-81917
class ColCdhCliGConn : public ColUtilSrvCliGConn
{
	using Parent_ = ColUtilSrvCliGConn;

public:
	ColCdhCliGConn(ColUtilServerObject& obj_, CommClientGuardFactory& guardFactory, char const* name_)
		: ColUtilSrvCliGConn(obj_, guardFactory, name_)
	{ }

	virtual void connected() override;
};

////////////////////////////////////////////////////////////////////////////////

class ColUtilServerConnFactory : public CommServerConnectionFactory
{
public:
	ColUtilServerConnFactory(ColUtilServerObject& obj_, CommServerGuardFactory& guardFactory, ServerConnectionType connectionType_)
		: CommServerConnectionFactory(guardFactory), obj(obj_), connectionType(connectionType_)
	{ }

	/* virtual */ /* new */ CommServerConnection* createConnection()
	{
		return new ColUtilServerConnection(obj, connectionType);
	}

public:
	ColUtilServerObject& obj;

private:
	ServerConnectionType connectionType;
};

////////////////////////////////////////////////////////////////////////////////

class ColUtilSrvAdminServerConnFactory : public CommServerConnectionFactory
{
public:
	ColUtilSrvAdminServerConnFactory(ColUtilServerObject& obj_, CommServerGuardFactory& guardFactory)
		: CommServerConnectionFactory(guardFactory), obj(obj_)
	{
		++normalPriority;
		++authPriority;
	}

	/* new */ CommServerConnection* createConnection() override
	{
		return new AdminSrvCon(obj);
	}

public:
	ColUtilServerObject& obj;
};

////////////////////////////////////////////////////////////////////////////////

class ColUtilSrvTrustedAdminServerConnFactory : public CommServerConnectionFactory
{
public:
	ColUtilSrvTrustedAdminServerConnFactory(ColUtilServerObject& obj_, CommServerGuardFactory& guardFactory)
		: CommServerConnectionFactory(guardFactory), obj(obj_)
	{
		++normalPriority;
		++authPriority;
	}

	/* new */ CommServerConnection* createConnection() override
	{
		return new TrustedAdminSrvConn(obj);
	}

public:
	ColUtilServerObject& obj;
};

////////////////////////////////////////////////////////////////////////////////

class AdminAuthAsyncCall : public BaseAsyncCallT<ColUtilServerObject>
{
public:
	AdminAuthAsyncCall(ColUtilServerObject& obj, AdminSrvCon const* pConn) :
		BaseAsyncCallT<ColUtilServerObject>(&obj, pConn)
	{ }

	void processReply(UINT32 requestId, UINT32 msgId, const CommMsgBody& body) override;
};

////////////////////////////////////////////////////////////////////////////////

// PYR-64045
struct ConvRateAsyncCall : public CommClientGConnection::AsyncCall
{
	ColUtilServerObject& obj;

	ConvRateAsyncCall(ColUtilServerObject& obj_);

	virtual void processReply(UINT32 requestId, UINT32 replyId, const CommMsgBody& body) override;
};

////////////////////////////////////////////////////////////////////////////////

// PYR-64045
struct GetShdHandsForAmpsAsyncCall : public CommClientGConnection::AsyncCall
{
	static const UINT32 EVENT_BATCH_SZ_FOR_OLAP_INFO = 20;

	ColUtilServerObject& obj;
	UINT32 curEventIndex;
	ColGetShdHandsForAmpsFastReport reportItem;

	typedef map<UINT32, PString> UserInfoMap;
	typedef UserInfoMap::iterator UserInfoMapIt;
	typedef UserInfoMap::const_iterator UserInfoMapConstIt;

	UserInfoMap userInfoMap;

	GetShdHandsForAmpsAsyncCall(ColUtilServerObject& obj_, const ColGetShdHandsForAmpsFastReport& reportItem_);

	virtual void processReply(UINT32 requestId, UINT32 replyId, const CommMsgBody& body) override;

	void getNextUserIntIdBatch(vector<UINT32>& userIntIdsBatch);
	void updateOlapUserInfo(vector<ColossusUserInfo>& userInfoVec);
	void applyOlapUserInfoBatch();
	UINT32 getLastEventIndex(UINT32 numEvents) const;
	void postResultReply(INT16 errCode, const char* errDesc) const;
	CommServerConnection* findConnection();
	bool isShdEventsEmpty() const;
	bool isAdminStillConnected() const;
	void parseShdData(CommMsgParser& parser);
	void keepOnlyRelevantRecords();
	bool gotAllUserInfo() const;
};

////////////////////////////////////////////////////////////////////////////////

// PYR-64045
struct AmpsTopPartnersAsyncCall : public CommClientGConnection::AsyncCall
{
	static const INT32 PARTNER_BATCH_SZ_FOR_OLAP_INFO = 200;

	enum ProcessingState
	{
		eProcessingInvalid = 0,
		eProcessingSNG, eProcessingRing
	};

	ColUtilServerObject& obj;
	AmpsTopPartnersFastReport reportItem;
	ProcessingState processingState;

	ColAmps::TopSngPartnersAggOutputs aggSngOutputs;
	ColAmps::TopSngPartnersRawOutputs rawSngOutputs;
	ColAmps::TopSngPartnersCachedOutputs finalSngOutputs;
	INT32 curSngRecIndex;

	ColAmps::TopRingPartnersAggOutputs aggRingOutputs;
	ColAmps::TopRingPartnersRawOutputs rawRingOutputs;
	ColAmps::TopRingPartnersCachedOutputs finalRingOutputs;
	INT32 curRingRecIndex;

	typedef PStringMap<ColossusUserInfo> UserInfoMap;
	typedef UserInfoMap::iterator UserInfoMapIt;
	typedef UserInfoMap::const_iterator UserInfoMapConstIt;
	UserInfoMap userInfoMap;

	ColAmps::TopSngPartners_RootUser sngRootUserInfo;
	ColAmps::TopRingPartners_RootUser ringRootUserInfo;

	AmpsTopPartnersAsyncCall(ColUtilServerObject& obj_, const AmpsTopPartnersFastReport& reportItem_);
	virtual ~AmpsTopPartnersAsyncCall();

	virtual void processReply(UINT32 requestId, UINT32 replyId, const CommMsgBody& body) override;

	void postErrorReply(INT16 errCode, const char* errDesc) const;
	void postResultReply() const;
	bool isAdminStillConnected() const;
	void composeJobState(CommMsgBody& body) const;
	void updateOlapUserInfo(const vector<ColossusUserInfo>& userInfoVec);

	void calcSngOutputRecords();
	void getNextSngUserIdsBatch(vector<PString>& userIdsBatch) const;
	bool gotAllSngUsersInfo() const;
	void applySngOlapUserInfoBatch();

	void calcRingOutputRecords();
	void getNextRingUserIdsBatch(vector<PString>& userIdsBatch) const;
	bool gotAllRingUsersInfo() const;
	void applyRingOlapUserInfoBatch();

private: // Helpers
	void copySngRelevantRecords(const vector<ColAmps::TopSngPartnersRecord>& records, vector<ColAmps::TopSngPartnersRecord>& relevantRecords) const;
	INT32 getLastSngOutputRecIndex() const;

	void mergeRingAggAndRawOutputs(vector<ColAmps::TopRingPartnersRecord>& records);
	void copyRingRelevantRecords(const vector<ColAmps::TopRingPartnersRecord>& records, vector<ColAmps::TopRingPartnersRecord>& relevantRecords) const;
	INT32 getLastRingOutputRecIndex() const;

	CommServerConnection* findConnection() const;
};

////////////////////////////////////////////////////////////////////////////////

// PYR-64045
struct AmpsGameActivityAsyncCall : public CommClientGConnection::AsyncCall
{
	enum ProcessingState
	{
		eProcessingInvalid = 0,
		eProcessingSNG, eProcessingRing
	};

	ColUtilServerObject& obj;
	AmpsGameActivityFastReport reportItem;
	ProcessingState processingState;

	ColAmps::SngGameActivityAggOutputs aggSngOutputs;
	ColAmps::SngGameActivityRawOutputs rawSngOutputs;
	ColAmps::SngGameActivityCachedOutputs finalSngOutputs;

	ColAmps::RingGameActivityAggOutputs aggRingOutputs;
	ColAmps::RingGameActivityRawOutputs rawRingOutputs;
	ColAmps::RingGameActivityCachedOutputs finalRingOutputs;

	AmpsGameActivityAsyncCall(ColUtilServerObject& obj_, const AmpsGameActivityFastReport& reportItem_);
	virtual ~AmpsGameActivityAsyncCall();

	virtual void processReply(UINT32 requestId, UINT32 replyId, const CommMsgBody& body) override;

	void postErrorReply(INT16 errCode, const char* errDesc) const;
	void postResultReply() const;
	bool isAdminStillConnected() const;
	void composeJobState(CommMsgBody& body) const;

	void mergeSngAggAndRawOutputs();
	void mergeRingAggAndRawOutputs();
	CommServerConnection* findConnection() const;
};

////////////////////////////////////////////////////////////////////////////////

// PYR-64045
struct AmpsGamesTogetherAsyncCall : public CommClientGConnection::AsyncCall
{
	ColUtilServerObject& obj;
	AmpsGamesTogetherFastReport reportItem;

	// Intermediate outputs
	ColUtils::SngGamesTogetherAggOutputs aggSngOutputs;
	vector<ColUtils::SngGamesTogetherRecord> rawSngOutputVec;
	ColUtils::RingGamesTogetherAggOutputs aggRingOutputs;
	vector<ColUtils::RingGamesTogetherRecord> rawRingOutputVec;

	// Final outputs
	ColAmps::SngGamesTogetherOutputs sngOutputs;
	ColAmps::RingGamesTogetherOutputs ringOutputs;

	AmpsGamesTogetherAsyncCall(ColUtilServerObject& obj_, const AmpsGamesTogetherFastReport& reportItem_);
	virtual ~AmpsGamesTogetherAsyncCall();

	virtual void processReply(UINT32 requestId, UINT32 replyId, const CommMsgBody& body) override;

	void postErrorReply(INT16 errCode, const char* errDesc) const;
	void postResultReply() const;
	bool isAdminStillConnected() const;
	void composeJobState(CommMsgBody& body) const;

	void mergeSngAggAndRawOutputs();
	void mergeRingAggAndRawOutputs();
	CommServerConnection* findConnection() const;
};

////////////////////////////////////////////////////////////////////////////////

// PYR-81917
struct CdaChangeRuleAsyncCallBase : public CommClientGConnection::AsyncCall
{
	ColUtilServerObject& obj;
	UINT32 reqId;
	UINT32 connId;
	UINT32 replyToOriginId;
	vector<CCDARule> ccdaRuleVec;
	INT16 replyErrCode;
	PString replyErrDesc;

	CdaChangeRuleAsyncCallBase(ColUtilServerObject& obj_, UINT32 reqId_, UINT32 connId_, UINT32 replyId_);
	virtual ~CdaChangeRuleAsyncCallBase();

	virtual void processReply(UINT32 requestId, UINT32 msgId, const CommMsgBody& body) override;
};

////////////////////////////////////////////////////////////////////////////////

// PYR-81917
struct CdaChangeRuleAsyncCall : public CdaChangeRuleAsyncCallBase
{
	CdaChangeRuleAsyncCall(ColUtilServerObject& obj_, UINT32 reqId_, UINT32 connId_, UINT32 replyId_);
	virtual ~CdaChangeRuleAsyncCall();

	void replyToOrigin() const;
};

////////////////////////////////////////////////////////////////////////////////

// PYR-81917
struct CdaReorderRuleAsyncCall : public CdaChangeRuleAsyncCallBase
{
	CdaReorderRuleAsyncCall(ColUtilServerObject& obj_, UINT32 reqId_, UINT32 connId_, UINT32 replyId_);
	virtual ~CdaReorderRuleAsyncCall();

	void replyToOrigin() const;
};

////////////////////////////////////////////////////////////////////////////////

// PYR-92626
struct CdaStartEvalAsyncCall : public CommClientGConnection::AsyncCall
{
	ColUtilServerObject& obj;
	UINT32 reqId;
	UINT32 connId;
	UINT32 replyToOriginId;
	INT16 replyErrCode;
	PString replyErrDesc;

	CdaStartEvalAsyncCall(ColUtilServerObject& obj_, UINT32 reqId_, UINT32 connId_, UINT32 replyId_);

	virtual void processReply(UINT32 requestId, UINT32 msgId, const CommMsgBody& body) override;
};

////////////////////////////////////////////////////////////////////////////////

// PYR-106157
struct GetUserTotalRakePaidAsyncCall : public CommClientGConnection::AsyncCall
{
	static UINT32 UNIQUE_ID;

	ColUtilServerObject& obj;
	UINT32 reqId;
	UINT32 connId;
	UINT32 curIteration;
	time_t timeOfRequest;
	ColUtils::GetUserWageringRequest userWageringRequest;

	UserWageringEx userWagering;

	GetUserTotalRakePaidAsyncCall(ColUtilServerObject& obj_, UINT32 reqId_, UINT32 connId_, 
		const time_t& timeOfRequest_, const ColUtils::GetUserWageringRequest& userWageringRequest_);
	~GetUserTotalRakePaidAsyncCall();

	virtual void processReply(UINT32 requestId, UINT32 replyMsgId, const CommMsgBody& body) override;
};

////////////////////////////////////////////////////////////////////////////////

class ColUtilServerObject : public CommServerObject
{
private: // AsyncCalls

	using ShdFreqPartnersAsyncCall = GenericAsyncCallT<ColUtilServerObject, ColShdFreqPartnersFastReport, false /* HandleErrors */>;
	using GetHandStatsAsyncCall = GenericAsyncCallT<ColUtilServerObject, ColGetHandStatsFastReport, false /* HandleErrors */>;
	using TcaConfigAsyncCall = GenericAsyncCallT<ColUtilServerObject, TCAConfig, false /* HandleErrors */>;
	using CdaRuleAsyncCall = GenericAsyncCallT<ColUtilServerObject, CCDARule, false /* HandleErrors */>;
	using ColWhitelistAsyncCall = GenericAsyncCallT<ColUtilServerObject, Void, false /* HandleErrors */>;
	using ColWhitelistNoReplyAsyncCall = NoReplyAsyncCallT<ColUtilServerObject, Void, false /* HandleErrors */>;
	using NormalizeUserIdsAsyncCall = GenericAsyncCallT<ColUtilServerObject, Void, false /* HandleErrors */>;
	using ColWhitelistCheckUsersAsyncCall = GenericAsyncCallT<ColUtilServerObject, ColWhitelist, false /* HandleErrors */>;
	using GetCdhEventsAsyncCall = GenericAsyncCallT<ColUtilServerObject, ColCdhEventFastReport, false /* HandleErrors */>;
	using GetNormalBPSAsyncCall = GenericAsyncCallT<ColUtilServerObject, ColNormalBPSFastReport, false /* HandleErrors */>;
	using GetXStakesBPSAsyncCall = GenericAsyncCallT<ColUtilServerObject, ColXStakesBPSFastReport, false /* HandleErrors */>;
	using GetBpsUserStakesPlayedAsyncCall = GenericAsyncCallT<ColUtilServerObject, ColBpsUserStakesPlayedFastReport, false /* HandleErrors */>;
	using GetAggTableExtentsAsyncCall = GenericAsyncCallT<ColUtilServerObject, ColGetAggTableExtentsFastReport, false /* HandleErrors */>;
	using GetAggTableExtentsNoReplyAsyncCall = NoReplyAsyncCallT<ColUtilServerObject, ColGetAggTableExtentsFastReport, false /* HandleErrors */>;
	using ColAdminReportAsyncCall = NoReplyAsyncCallT<ColUtilServerObject, Void, false /* HandleErrors */>;

public:
	explicit ColUtilServerObject(_CommInterface& inter)
		: CommServerObject(inter)
		, srvPool(inter)
		, cliPool(inter)
		, connInternalFactory(*this, guardFactory, Internal)
		, connWebFactory(*this, guardFactory)
		, connAdminFactory(*this, authGuardFactory)
		, connTrustedAdminFactory(*this, guardFactory)
		, loginAuthConn(*this, clientNullGuardFactory, "L")
		, colRawReportDbmConn(*this, clientNullGuardFactory, "RAW")
		, colReaderDbmConn(*this, clientNullGuardFactory, "O")
		, colAggReportDbmConn(*this, clientNullGuardFactory, "AGG")
		, tcaConn(*this, clientNullGuardFactory, "TCA")
		, ccdaConn(*this, clientNullGuardFactory, "CCDA")
		, cdhConn(*this, clientNullGuardFactory, "CDH")
		, colAggDbmWriterConn(*this, clientNullGuardFactory, "AGGW")
		, olapConn(*this, clientNullGuardFactory, "OLAP")
		, templateSrvConn(*this, clientNullGuardFactory, "TPL")
		, colAggDataDbmConn(*this, clientNullGuardFactory, "DA")
		, colRawDataDbmConn(*this, clientNullGuardFactory, "DR")
		, useLogger(false)
		, heLogger(0)
		, heLoggerTimerHandle(0)
		, moreTrace(false)
		, fastReportsMaxQueueSize(0)
		, fastReportsMaxNumProcessing(0)
		, shdFreqPartnersReportTimeoutSeconds(0)
		, getHandStatsReportTimeoutSeconds(0)
		, getShdHandsForAmpsTimeoutSeconds(0)
		, getAmpsTopPartnersTimeoutSeconds(0)
		, getAmpsGameActivityTimeoutSeconds(0)
		, getAmpsGamesTogetherTimeoutSeconds(0)
		, getCdhEventReportTimeoutSeconds(0)
		, getBpsUserStakesPlayedTimeoutSeconds(0)
		, getAggTableExtentsTimeoutSeconds(0)
		, fastReportQueue(0, 0)
		, timerConverterHandle(0)
	{ }

	~ColUtilServerObject() { heLogger = nullptr; }

	virtual void init(CommMsgBody& initMsg) override;
	virtual void shutdown() override;

	void processCommonServerMessage(UINT32 msgId, const CommMsgBody& body, ServerConnectionType serverConnection, CommServerConnection* conn);

	/* virtual */ void processMessage(_CommMsg& msg) override;

	// Callbacks
	void processCallback(UINT32 reqId, UINT32 msgId, const CommMsgBody& body, CommClientGConnection::AsyncCall* pCall);
	void receiveAdminAuth(const CommMsgBody& body, AdminSrvCon* pConn);

	UINT32 postToLoginAuth(UINT32 msgId, CommMsgBody& body, CommClientGConnection::AsyncCall* pCall = 0)
	{
		return loginAuthConn.post(msgId, body, pCall);
	}

	UINT32 postToColReportingRaw(UINT32 msgId, CommMsgBody& body, CommClientGConnection::AsyncCall* pCall = 0)
	{
		return colRawReportDbmConn.post(msgId, body, pCall);
	}

	UINT32 postToColReportingAgg(UINT32 msgId, CommMsgBody& body, CommClientGConnection::AsyncCall* pCall = 0)
	{
		return colAggReportDbmConn.post(msgId, body, pCall);
	}

	UINT32 postToColReader(UINT32 msgId, CommMsgBody& body, CommClientGConnection::AsyncCall* pCall = 0)
	{
		return colReaderDbmConn.post(msgId, body, pCall);
	}

	UINT32 postToCCDA(UINT32 msgId, CommMsgBody& body, CommClientGConnection::AsyncCall* pCall = 0)
	{
		return ccdaConn.post(msgId, body, pCall);
	}

	UINT32 postToCDH(UINT32 msgId, CommMsgBody& body, CommClientGConnection::AsyncCall* pCall = 0)
	{
		return cdhConn.post(msgId, body, pCall);
	}

	UINT32 postToTCA(UINT32 msgId, CommMsgBody& body, CommClientGConnection::AsyncCall* pCall = 0)
	{
		return tcaConn.post(msgId, body, pCall);
	}

	UINT32 postToColAggWriter(UINT32 msgId, CommMsgBody& body, CommClientGConnection::AsyncCall* pCall = 0)
	{
		return colAggDbmWriterConn.post(msgId, body, pCall);
	}

	UINT32 postToOlap(UINT32 msgId, CommMsgBody& body, CommClientGConnection::AsyncCall* pCall = 0)
	{
		return olapConn.post(msgId, body, pCall);
	}

	UINT32 postToTemplateSrv(UINT32 msgId, CommMsgBody& body, CommClientGConnection::AsyncCall* pCall = 0)
	{
		return templateSrvConn.post(msgId, body, pCall);
	}

	UINT32 postToColDataAgg(UINT32 msgId, CommMsgBody& body, CommClientGConnection::AsyncCall* pCall = 0)
	{
		return colAggDataDbmConn.post(msgId, body, pCall);
	}

	UINT32 postToColDataRaw(UINT32 msgId, CommMsgBody& body, CommClientGConnection::AsyncCall* pCall = 0)
	{
		return colRawDataDbmConn.post(msgId, body, pCall);
	}

	// necessary for AsyncCalls to work
	CommServerConnection* findConnection(UINT32 connId)
	{ 	
		return srvPool.findConnection(connId); 
	}

	void addMessageTicks(const HiresCounter& counter) { ticksAccumulator.addMessageTicks(counter); }

	// Cache handlers
	void insertAmpsShdHandsCache(const ColAmps::GetShdHandsInputs& inputs, const ColAmps::GetShdHandsOutputs& outputs);
	void insertAmpsTopSngPartnersCache(const ColAmps::TopPartnersInputs& inputs, const ColAmps::TopSngPartnersCachedOutputs& outputs);
	void insertAmpsTopRingPartnersCache(const ColAmps::TopPartnersInputs& inputs, const ColAmps::TopRingPartnersCachedOutputs& outputs);
	void insertAmpsTopSngPartnersRootUserCache(const ColAmps::TopPartnersInputs& inputs, const ColAmps::TopSngPartners_RootUser& outputs);
	void insertAmpsTopRingPartnersRootUserCache(const ColAmps::TopPartnersInputs& inputs, const ColAmps::TopRingPartners_RootUser& outputs);
	void insertAmpsSngGameActivityCache(const ColAmps::GameActivityInputs& inputs, const ColAmps::SngGameActivityCachedOutputs& outputs);
	void insertAmpsRingGameActivityCache(const ColAmps::GameActivityInputs& inputs, const ColAmps::RingGameActivityCachedOutputs& outputs);
	void insertAmpsSngGamesTogetherCache(const ColAmps::GamesTogetherInputs& inputs, const ColAmps::SngGamesTogetherOutputs& outputs);
	void insertAmpsRingGamesTogetherCache(const ColAmps::GamesTogetherInputs& inputs, const ColAmps::RingGamesTogetherOutputs& outputs);

	// Currency conversions rates
	void processConvRates(CommMsgParser& parser);
	INT64 convertCurrencyM(INT64 amtSrc, const char* curSrc, const char* curDst) const;

	// Handlers for Colossus messages
	void processGetShdFreqPartnersReport(UINT32 reqId, CommMsgParser& parser, CommServerConnection* pConn);
	void processGetShdFreqPartnersReport_Submitted(const ColShdFreqPartnersFastReport& inputs);
	void processGetShdFreqPartnersReport_GotPPSData(CommMsgParser& parser, ShdFreqPartnersAsyncCall* pCall);

	void processGetStatsForHand(UINT32 reqId, CommMsgParser& parser, CommServerConnection* pConn);
	void processGetStatsForHand_Submitted(const ColGetHandStatsFastReport& reportItem);
	void processGetStatsForHand_GotData(CommMsgParser& parser, GetHandStatsAsyncCall* pCall);

	void processGetShdHandsForAmps(UINT32 reqId, CommMsgParser& parser, CommServerConnection* pConn);
	void processGetShdHandsForAmps_Submitted(const ColGetShdHandsForAmpsFastReport& reportItem);
	void processGetShdHandsForAmps_GotShdData(CommMsgParser& parser, GetShdHandsForAmpsAsyncCall& callState);
	void processGetShdHandsForAmps_GotUserInfoBatch(CommMsgParser& parser, GetShdHandsForAmpsAsyncCall& callState);
	void processGetShdHandsForAmps_GetUserInfoBatch(GetShdHandsForAmpsAsyncCall& callState);

	void processAmpsTopPartners(UINT32 reqId, CommMsgParser& parser, CommServerConnection* pConn);
	bool processAmpsTopPartners_PreProcessing(const char* logStr, const AmpsTopPartnersAsyncCall& callState);
	void processAmpsTopPartners_Submitted(const AmpsTopPartnersFastReport& reportItem);
	void processAmpsTopPartners_SNG_GotAggData(CommMsgParser& parser, AmpsTopPartnersAsyncCall& callState);
	void processAmpsTopPartners_SNG_GotRawData(CommMsgParser& parser, AmpsTopPartnersAsyncCall& callState);
	void processAmpsTopPartners_SNG_GetUserInfoBatch(AmpsTopPartnersAsyncCall& callState);
	void processAmpsTopPartners_RING_GotAggData(CommMsgParser& parser, AmpsTopPartnersAsyncCall& callState);
	void processAmpsTopPartners_RING_GotRawData(CommMsgParser& parser, AmpsTopPartnersAsyncCall& callState);
	void processAmpsTopPartners_RING_GetUserInfoBatch(AmpsTopPartnersAsyncCall& callState);
	void processAmpsTopPartners_GotUserInfoBatch(CommMsgParser& parser, AmpsTopPartnersAsyncCall& callState);

	void processAmpsGameActivity(UINT32 reqId, CommMsgParser& parser, CommServerConnection* pConn);
	bool processAmpsGameActivity_PreProcessing(const char* logStr, const AmpsGameActivityAsyncCall& callState);
	void processAmpsGameActivity_Submitted(const AmpsGameActivityFastReport& reportItem);
	void processAmpsGameActivity_SNG_GotAggData(CommMsgParser& parser, AmpsGameActivityAsyncCall& callState);
	void processAmpsGameActivity_SNG_GotRawData(CommMsgParser& parser, AmpsGameActivityAsyncCall& callState);
	void processAmpsGameActivity_RING_GotAggData(CommMsgParser& parser, AmpsGameActivityAsyncCall& callState);
	void processAmpsGameActivity_RING_GotRawData(CommMsgParser& parser, AmpsGameActivityAsyncCall& callState);

	void processAmpsGamesTogether(UINT32 reqId, CommMsgParser& parser, CommServerConnection* pConn);
	bool processAmpsGamesTogether_PreProcessing(const char* logStr, const AmpsGamesTogetherAsyncCall& callState);
	void processAmpsGamesTogether_Submitted(const AmpsGamesTogetherFastReport& reportItem);
	void processAmpsGamesTogether_SNG_GotAggData(CommMsgParser& parser, AmpsGamesTogetherAsyncCall& callState);
	void processAmpsGamesTogether_SNG_GotRawData(CommMsgParser& parser, AmpsGamesTogetherAsyncCall& callState);
	void processAmpsGamesTogether_RING_GotAggData(CommMsgParser& parser, AmpsGamesTogetherAsyncCall& callState);
	void processAmpsGamesTogether_RING_GotRawData(CommMsgParser& parser, AmpsGamesTogetherAsyncCall& callState);

	void processGetCdhEventReport(UINT32 reqId, CommMsgParser& parser, CommServerConnection* pConn);
	void processGetCdhEventReport_Submitted(const ColCdhEventFastReport& reportItem);
	void processGetCdhEventReport_GotData(CommMsgParser& parser, GetCdhEventsAsyncCall* pCall);

	void processGetNormalBPSReport(UINT32 reqId, CommMsgParser& parser, AdminSrvCon& adminConn);
	void processGetNormalBPSReport_Submitted(const ColNormalBPSFastReport& reportItem);
	void processGetNormalBPSReport_GotData(CommMsgParser& parser, GetNormalBPSAsyncCall* pCall);

	void processGetXStakesBPSReport(UINT32 reqId, CommMsgParser& parser, AdminSrvCon& adminConn);
	void processGetXStakesBPSReport_Submitted(const ColXStakesBPSFastReport& reportItem);
	void processGetXStakesBPSReport_GotData(CommMsgParser& parser, GetXStakesBPSAsyncCall* pCall);

	void processGetBpsUserStakedPlayedReport(UINT32 reqId, CommMsgParser& parser, CommServerConnection* pConn);
	void processGetBpsUserStakedPlayedReport_Submitted(const ColBpsUserStakesPlayedFastReport& reportItem);
	void processGetBpsUserStakedPlayedReport_GotData(CommMsgParser& parser, GetBpsUserStakesPlayedAsyncCall* pCall);

	void processGetAggTableExtentsReport(UINT32 reqId, CommMsgParser& parser, CommServerConnection* pConn);
	void processGetAggTableExtentsReport_Submitted(const ColGetAggTableExtentsFastReport& reportItem);
	void processGetAggTableExtentsReport_OnSchedule_Submitted(const ColGetAggTableExtentsFastReport& reportItem);
	void processGetAggTableExtentsReport_GotData(CommMsgParser& parser, GetAggTableExtentsAsyncCall* pCall);
	void processGetAggTableExtentsReport_OnSchedule_GotData(CommMsgParser& parser, GetAggTableExtentsNoReplyAsyncCall* pCall);

	// User handlers
	void processNormalizeUserIds(UINT32 reqId, CommMsgParser& parser, AdminSrvCon& conn);
	void processNormalizeUserIds_GotData(CommMsgParser& parser, NormalizeUserIdsAsyncCall* pCall);

	// ColWhitelist handlers
	void processCreateColWhitelist(UINT32 reqId, CommMsgParser& parser, AdminSrvCon& conn);
	void processModifyColWhitelist(UINT32 reqId, CommMsgParser& parser, AdminSrvCon& conn);
	void processDeleteColWhitelist(UINT32 reqId, CommMsgParser& parser, AdminSrvCon& conn);
	void processGetCurrColWhitelist(UINT32 reqId, CommMsgParser& parser, AdminSrvCon& conn);
	void processGetColWhitelistHisotry(UINT32 reqId, CommMsgParser& parser, AdminSrvCon& conn);
	void processGetColWhitelistEntry(UINT32 reqId, CommMsgParser& parser, AdminSrvCon& conn);

	void processCreateColWhitelist_GotData(CommMsgParser& parser, ColWhitelistAsyncCall* pCall);
	void processModifyColWhitelist_GotData(CommMsgParser& parser, ColWhitelistAsyncCall* pCall);
	void processDeleteColWhitelist_GotData(CommMsgParser& parser, ColWhitelistAsyncCall* pCall);
	void processGetCurrColWhitelist_GotData(CommMsgParser& parser, ColWhitelistAsyncCall* pCall);
	void processGetColWhitelistHisotry_GotData(CommMsgParser& parser, ColWhitelistAsyncCall* pCall);
	void processGetColWhitelistEntry_GotData(CommMsgParser& parser, ColWhitelistAsyncCall* pCall);

	void detectColWhitelistChange(UINT32 msgId, const CommMsgBody& body);

	void processGetCurCdhWhitelist();
	void processGetCurCdhWhitelist_GotData(CommMsgParser& parser, ColWhitelistNoReplyAsyncCall* pCall);
	void processGetCurCdhWhitelist_GotCdhReply(CommMsgParser& parser, ColWhitelistNoReplyAsyncCall* pCall);

	void processCdaChangeRule_CdaDone(CommMsgParser& parser, CdaChangeRuleAsyncCallBase* callState);
	void processCdaChangeRule_CdaDoneReorder(CommMsgParser& parser, CdaChangeRuleAsyncCallBase* callState);
	void processCdaChangeRule_GotEmailList(CommMsgParser& parser, CdaChangeRuleAsyncCallBase* callState);
	void processCdaChangeRule_EmailSent(CommMsgParser& parser, CdaChangeRuleAsyncCallBase* callState);

	void processGetUserRakePaid(UINT32 reqId, UINT32 connId, CommMsgParser& parser);
	void processGetUserRakePaid_GotAggData(CommMsgParser& parser, GetUserTotalRakePaidAsyncCall& callState);
	void processGetUserRakePaid_GotRawData(CommMsgParser& parser, GetUserTotalRakePaidAsyncCall& callState);
	bool processGetUserRakePaid_CalcRawExtents(
		const SrvDate& lastAggDate, const ColUtils::GetUserWageringRequest& userWageringRequest, const time_t& now,
		SrvTime& rawStartTime, SrvTime& rawEndTime);
	void processGetUserRakePaid_ReplyToClient(const GetUserTotalRakePaidAsyncCall& callState);
	void processGetUserRakePaid_ReplyErrorToClient(UINT32 reqId, UINT32 connId, INT16 errCode, const char* errBuf);

	void insertAdminReport(ColUtils::ColAdminReport& adminReport);
	void processInsertAdminReport_GotData(CommMsgParser& parser, ColAdminReportAsyncCall* pCall);

protected:

	void forwardAsyncMsg_CreateFwdMsgBody(UINT32 msgId, UINT32 reqId, CommMsgParser& parser, CommServerConnection* inboundConn, CommMsgBody& fwd)
	{
		PLog("[%s] <I> reqId: %u msgId: %u ", __FUNCTION__, reqId, msgId);
		AdminSrvCon* adminConn = dynamic_cast<AdminSrvCon*>(inboundConn);
		if (adminConn == nullptr)
		{
			PLog("[%s] <E> lost admin connection, aborting", __FUNCTION__);
			return;
		}

		fwd.copyFromSkipReqId(parser.getMsgBody());
	}

	// use TcaConfigAsyncCall for TCA
	// use CdaRuleAsyncCall for CCDA
	template<class C>
	void forwardAsyncMsg(UINT32 msgId, UINT32 reqId, CommMsgParser& parser, CommServerConnection* inboundConn, ColUtilSrvCliGConn& outboundConn)
	{
		CommMsgBody fwd;
		forwardAsyncMsg_CreateFwdMsgBody(msgId, reqId, parser, inboundConn, fwd);

		C* call = new C(this, inboundConn->id(), reqId, msgId+1, &ColUtilServerObject::forwardAsyncReply<C>);
		outboundConn.post(msgId, fwd, call);
	}

	// Template specialized to CdaChangeRuleAsyncCall
	template<>
	void forwardAsyncMsg<CdaChangeRuleAsyncCall>(UINT32 msgId, UINT32 reqId, CommMsgParser& parser, CommServerConnection* inboundConn, ColUtilSrvCliGConn& outboundConn)
	{
		CommMsgBody fwd;
		forwardAsyncMsg_CreateFwdMsgBody(msgId, reqId, parser, inboundConn, fwd);

		CdaChangeRuleAsyncCall* call = new CdaChangeRuleAsyncCall(*this, reqId, inboundConn->id(), msgId + 1);
		outboundConn.post(msgId, fwd, call);
	}

	// Template specialized to CdaReorderRuleAsyncCall
	template<>
	void forwardAsyncMsg<CdaReorderRuleAsyncCall>(UINT32 msgId, UINT32 reqId, CommMsgParser& parser, CommServerConnection* inboundConn, ColUtilSrvCliGConn& outboundConn)
	{
		CommMsgBody fwd;
		forwardAsyncMsg_CreateFwdMsgBody(msgId, reqId, parser, inboundConn, fwd);

		CdaReorderRuleAsyncCall* call = new CdaReorderRuleAsyncCall(*this, reqId, inboundConn->id(), msgId + 1);
		outboundConn.post(msgId, fwd, call);
	}

	template<>
	void forwardAsyncMsg<CdaStartEvalAsyncCall>(UINT32 msgId, UINT32 reqId, CommMsgParser& parser, CommServerConnection* inboundConn, ColUtilSrvCliGConn& outboundConn)
	{
		CommMsgBody fwd;
		forwardAsyncMsg_CreateFwdMsgBody(msgId, reqId, parser, inboundConn, fwd);

		CdaStartEvalAsyncCall* call = new CdaStartEvalAsyncCall(*this, reqId, inboundConn->id(), msgId + 1);
		outboundConn.post(msgId, fwd, call);
	}

	template<class C>
	void forwardAsyncReply(CommMsgParser& parser, C* call)
	{
		PLog( "[%s] <I> reqId: %u replyId: %u ", __FUNCTION__, call->getReqId(), call->getReplyId());

		INT16 errCode = 0;
		parser.parseINT16(errCode);
		if (errCode != DBM_NO_ERROR)
		{
			const char* errBuf;
			parser.parseString(errBuf);
			PLog("[%s] <E> Error: %s", __FUNCTION__, errBuf);
			call->postReply(errCode, errBuf);
			return;
		}

		CommMsgBody body;
		body.copyFrom(parser.getMsgBody()); // Copy the whole message body, not just unparsed part
		call->postReply(body);
	}

private:
	void dynamicInit();
	void reconnect(CommClientConnectionPool& cliConnPool, CommClientGConnection& conn, const char* const serverName, const char* const serverObject, const char* const serverConnectionType);

	void processTimerMessage(UINT32 handle, UINT32 msgId, CommMsgBody& body) override;
	void processAdminMessage(UINT32 msgId, const CommMsgBody& body, CommServerConnection* conn);
	void processTrustedAdminMessage(UINT32 msgId, const CommMsgBody& body, CommServerConnection* conn);
	void processInternalMessage(UINT32 msgId, const CommMsgBody& body, CommServerConnection* conn);

	void startHeLogger(const char * strServerAddress, const char * strStationName );
	void stopHeLogger();
	void reportPerformance();

	// Handlers for Admin messages
	void processRefreshSettings(CommMsgBody& reply);
	void processKillServer(const char* adminId);

	void getAggTableExtents();

	static void postReply(CommServerConnection* pConn, UINT32 msgId, CommMsgBody& body);
	static void setReply(CommMsgBody& reply, INT16 errCode, char const* errDesc = "");
	static bool checkAdminRight(const char* right, const ColUtilSrvPrivilegedConnection* pConn, CommMsgBody& reply);

	static CommServerNullGuardFactory guardFactory;
	static CommServerPlainTextPasswordSidGuardFactory authGuardFactory;
	static CommClientNullGuardFactory clientNullGuardFactory;

	CommServerConnectionPool srvPool;
	CommClientConnectionPool cliPool;	

	ColUtilServerConnFactory connInternalFactory;
	ColUtilSrvAdminServerConnFactory connWebFactory;
	ColUtilSrvAdminServerConnFactory connAdminFactory;
	ColUtilSrvTrustedAdminServerConnFactory connTrustedAdminFactory;

	ColUtilSrvCliGConn loginAuthConn;
	ColUtilSrvCliGConn colRawReportDbmConn;
	ColUtilSrvCliGConn colReaderDbmConn;
	ColUtilSrvCliGConn colAggReportDbmConn;
	ColUtilSrvCliGConn tcaConn;
	ColUtilSrvCliGConn ccdaConn;
	ColCdhCliGConn cdhConn;
	ColUtilSrvCliGConn colAggDbmWriterConn;
	ColUtilSrvCliGConn olapConn;
	ColUtilSrvCliGConn templateSrvConn;
	ColUtilSrvCliGConn colAggDataDbmConn;
	ColUtilSrvCliGConn colRawDataDbmConn;

	PString fullFileName;
	PString sectionName;
	PString initDir;

	TicksAccumulator ticksAccumulator;

	bool useLogger;
	PString loggerServerAddress;
	PString loggerStationName;

	HeLogger* heLogger;
	UINT32 heLoggerTimerHandle;

	bool moreTrace;

	UINT32 fastReportsMaxQueueSize;
	UINT32 fastReportsMaxNumProcessing;
	UINT32 shdFreqPartnersReportTimeoutSeconds;
	UINT32 getHandStatsReportTimeoutSeconds;
	UINT32 getShdHandsForAmpsTimeoutSeconds;
	UINT32 getAmpsTopPartnersTimeoutSeconds;
	UINT32 getAmpsGameActivityTimeoutSeconds;
	UINT32 getAmpsGamesTogetherTimeoutSeconds;
	UINT32 getCdhEventReportTimeoutSeconds;
	UINT32 getBpsUserStakesPlayedTimeoutSeconds;
	UINT32 getAggTableExtentsTimeoutSeconds;

	FastReportQueue fastReportQueue;
	ReportCaches reportCaches;

	UINT32 timerConverterHandle;
	CurrencyRatesVec currencies;
};

////////////////////////////////////////////////////////////////////////////////

class ColUtilServerObjectFactory : public CommServerObjectFactory
{
public:
	/* virtual */ /* new */ CommServerObject* createServerObject(_CommInterface& inter) const
	{
		return new ColUtilServerObject(inter);
	}
};

////////////////////////////////////////////////////////////////////////////////

#endif // ColSrvobject_h_included
