#ifndef ColCdaObject_h_included
#define ColCdaObject_h_included

////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "commgrdnull.h"
#include "commgrdppwdsid.h"
#include "counters.h"
#include "ColCdaCacheTypes.h"
#include "ColCdaRules.h"
#include "ColCda.h"
#include "coldbm.h"
#include "sharedconversion.h"
#include "../shared/ColWhitelist.h"

using namespace ColCDA;

class ColCdaServerObject;
struct TournRetryData;
struct ColossusPPTInfo;
class UserWMDataMap;

namespace WagerMeas
{
	struct WageringPerCurr;
}

using WageringPerCurrVec = vector<WagerMeas::WageringPerCurr>;

////////////////////////////////////////////////////////////////////////////////

#define TIMER_LOGGER_MSEC					5000  // 5 seconds
#define TIMER_CONVERTER_MSEC				(60*60*1000) // 1 HOUR
#define TIMER_SWEEPER_MSEC					10*1000  // 10 seconds
#define TIMER_EVAL_CACHE_USERS_MSEC			5000  // 5 seconds

#define DEFAULT_HAND_TOO_OLD_THRESH_SECS	86400	// 24 hours
#define DEFAULT_MAX_NUM_HANDS_ALLOWED		10000000
#define DEFAULT_MAX_NUM_CDHUSER_ALLOWED		500000
#define DEFAULT_MAX_NUM_TOURN_ALLOWED		500000
#define DEFAULT_FIRST_TOURN_RETRY_DELAY		60		//One minute
#define DEFAULT_TOURN_RETRY_DELAY			(15*60)   //15 minutes
#define DEFAULT_MAX_TOURN_RETRIES			7
#define DEFAULT_MAX_NUM_CACHE_USERS_EVAL	50

// Server defaults
enum
{
	TIMER_LOGGER		= 1,
	TIMER_CONVERTER		= 2,
	TIMER_TOURN_DELAY	= 3,
	TIMER_SWEEPER		= 4,
	TIMER_EVAL_CACHE_USERS = 5,
};

enum COLCDA_CONN_TYPE
{
	eColCdaConnTypeTrustedAdmin = 1,
	eColCdaConnTypeAdmin,
	eColCdaConnTypeOlapMD,
	eColCdaConnTypeColossus,
	eColCdaConnTypeColUtil,
};

////////////////////////////////////////////////////////////////////////////////

class TrustedAdminConnection : public CommServerConnection
{
protected:
	ColCdaServerObject* obj;

public:
	explicit TrustedAdminConnection(ColCdaServerObject* obj_) : obj(obj_) { }

	void processMessage(UINT32 msgId, const CommMsgBody& body) override;
	void connected() override;
	void closedOrDisconnected(int errCode, const char* errMsg) override;
};

class TrustedAdminConnFactory : public CommServerConnectionFactory
{
public:
	ColCdaServerObject* obj;

public:
	explicit TrustedAdminConnFactory(CommServerGuardFactory& guardFactory)
		: CommServerConnectionFactory(guardFactory)
	{
		obj = 0;
		++normalPriority;
		++authPriority;
	}

	/* new */ CommServerConnection* createConnection() override
	{
		return new TrustedAdminConnection(obj);
	}
};

////////////////////////////////////////////////////////////////////////////////

class AdminConnection : public CommServerConnection
{
protected:
	ColCdaServerObject* obj;
	UINT32 userIntId;
	PString userId;
	PStringSet adminRights;

public:
	explicit AdminConnection(ColCdaServerObject* obj_) : userIntId(0), authError(0)
	{
		obj = obj_;
	}

	void processMessage(UINT32 msgId, const CommMsgBody& body) override;
	void connected() override;
	void closedOrDisconnected(int errCode, const char* errMsg) override;
	void getUserAuth(const char* guardType, const char* user_, CommMsgBody& clientExtra, CommMsgBody& request) override;
	bool checkAdminRight(char const* rightName) const { return adminRights.find(rightName) != adminRights.end(); }
	void receiveAdminAuth(CommMsgParser& parser);
	const char* getUser() const { return userId.c_str(); }
	UINT32 getUserIntId() const { return userIntId; }
	static const char* tryGetAdminId(CommServerConnection* conn);

private:
	INT16 authError;
};

class AdminConnFactory : public CommServerConnectionFactory
{
public:
	ColCdaServerObject* obj;

public:
	explicit AdminConnFactory(CommServerGuardFactory& guardFactory)
		: CommServerConnectionFactory(guardFactory)
	{
		obj = 0;
		++normalPriority;
		++authPriority;
	}

	/* new */ CommServerConnection* createConnection() override
	{
		return new AdminConnection(obj);
	}
};

////////////////////////////////////////////////////////////////////////////////

class OlapMDServerConnection : public CommServerConnection
{
protected:
	ColCdaServerObject* obj;

public:
	explicit OlapMDServerConnection(ColCdaServerObject* obj_) : obj(obj_) { }

	void processMessage(UINT32 msgId, const CommMsgBody& body) override;
	void connected() override;
	void closedOrDisconnected(int errCode, const char* errMsg) override;
};

class OlapMDServerConnFactory : public CommServerConnectionFactory
{
public:
	ColCdaServerObject* obj;

	explicit OlapMDServerConnFactory(CommServerGuardFactory& guardFactory)
		: CommServerConnectionFactory(guardFactory)
	{
		obj = 0;
	}

	/* new */ CommServerConnection* createConnection() override
	{
		return new OlapMDServerConnection(obj);
	}
};

////////////////////////////////////////////////////////////////////////////////

class ColossusSrvConn : public CommServerConnection
{
protected:
	ColCdaServerObject* obj;

public:
	explicit ColossusSrvConn(ColCdaServerObject* obj_) : obj(obj_) { }

	void processMessage(UINT32 msgId, const CommMsgBody& body) override;
	void connected() override;
	void closedOrDisconnected(int errCode, const char* errMsg) override;
};

class ColossusSrvConnFactory : public CommServerConnectionFactory
{
public:
	ColCdaServerObject* obj;

	explicit ColossusSrvConnFactory(CommServerGuardFactory& guardFactory)
		: CommServerConnectionFactory(guardFactory)
	{
		obj = 0;
	}

	/* new */ CommServerConnection* createConnection() override
	{
		return new ColossusSrvConn(obj);
	}
};

////////////////////////////////////////////////////////////////////////////////

class ColUtilSrvConn : public CommServerConnection
{
protected:
	ColCdaServerObject* obj;

public:
	explicit ColUtilSrvConn(ColCdaServerObject* obj_) : obj(obj_) { }

	void processMessage(UINT32 msgId, const CommMsgBody& body) override;
	void connected() override;
	void closedOrDisconnected(int errCode, const char* errMsg) override;
};

class ColUtilSrvConnFactory : public CommServerConnectionFactory
{
public:
	ColCdaServerObject* obj;

	explicit ColUtilSrvConnFactory(CommServerGuardFactory& guardFactory)
		: CommServerConnectionFactory(guardFactory)
	{
		obj = 0;
	}

	/* new */ CommServerConnection* createConnection() override
	{
		return new ColUtilSrvConn(obj);
	}
};

////////////////////////////////////////////////////////////////////////////////

class ColCdaClientConn : public CommClientGConnection
{
protected:
	int isconnected;
	char name;

public:
	ColCdaServerObject* obj;

public:
	ColCdaClientConn(CommClientGuardFactory& guardFactoryClient, char name_)
		: CommClientGConnection(guardFactoryClient), isconnected(0), name (name_), obj(0)
	{
	}

public:
	void connected() override;
	bool closedOrDisconnected(int errCode, const char* errMsg) override;
	UINT32 post(UINT32 msgId, CommMsgBody& body, AsyncCall* call);
	void processGMessage(UINT32 requestId, UINT32 msgId, const CommMsgBody& body, AsyncCall* call) override;
	bool isTheSame(const char* server, const char* serverObject, const char* connectionType)
	{
		return
			PString::equals(underlyingConnection().getServer(), server) &&
			PString::equals(underlyingConnection().getServerObject(), serverObject) &&
			PString::equals(underlyingConnection().getServerConnType(), connectionType)
			;
	}

	int isConnected() const
	{
		return isconnected;
	}
};

////////////////////////////////////////////////////////////////////////////////

class ColCdaServerObject : public CommServerObject
{
	static CommServerNullGuardFactory guardFactory;
	static CommClientNullGuardFactory guardFactoryClient;
	static CommServerPlainTextPasswordSidGuardFactory guardPwdFactory;

	CommServerConnectionPool srvPool;
	CommClientConnectionPool cliPool;

	AdminConnFactory adminFactory;
	TrustedAdminConnFactory adminTrustedFactory;
	OlapMDServerConnFactory olapMDConnFactory;
	ColossusSrvConnFactory colossusSrvConnFactory;
	ColUtilSrvConnFactory colUtilSrvConnFactory;

	ColCdaClientConn loginAuthConn;
	ColCdaClientConn colReaderConn;
	ColCdaClientConn colDbmWriterConn;
	ColCdaClientConn colDbmConn;
	ColCdaClientConn colRawDbmConn;
	ColCdaClientConn wagerMeasConn;
	ColCdaClientConn collusionConn;
	ColCdaClientConn olapConn;
	ColCdaClientConn templateSrvConn;

	PString fullFileName;
	PString initDir;

	TicksAccumulator ticksAccumulator;

	UINT32 timerConverterHandle;
	UINT32 sweeperTimerHandle;
	UINT32 heLoggerTimerHandle;
	
	PString loggerStationName;
    HeLogger* heLogger;

	bool serverEnabled;
	bool handleRing;
	bool handleTourn;
	bool moreTrace;
	UINT32 handTimeTooOldSecs;
	UINT32 maxNumHandsAllowed;
	UINT32 maxNumCDHUserAllowed;
	UINT32 maxNumTournAllowed;
	UINT32 firstTournRetryDelay;
	UINT32 tournRetryDelay;
	UINT32 maxTournRetries;
	bool enableAlertAutoAction;
	UINT32 evaluationStatus;
	UINT32 maxNumUsersToEval;

	CurrencyRatesVec					currencies;

	// CCDA rule containers
	RuleContainer<RingGameRule> ringGameRules;
	RuleContainer<TournRule>  sngHUGameRules;
	CCDARule	velocityControlRule;

	// CCDA Caches
	RingHandCache ringHandCache;
	CDHHandCache  cdhHandCache;
	NewTournCache newTournCache;
	TournDataCache tournDataCache;
	PlayerGameCache playerGameCache;
	SRMMonitorCache srmMonitorCache;
	UserEvaluationCache userEvaluationCache;

	// whitelist
	ColWhitelistCache colWhitelistCache;

	// Timer methods
	void sweepCaches();

	// Rule methods
	void addCCDARule(const CCDARule& rule);
	void deleteCCDARule(const CCDARule& rule);
	void reorderCCDARule(const vector<CCDARule>& ccdaRuleVec);
	void getCurrentActiveRingRule(BankerRingGameEvalData& bankerRingGameEvalData, const SrvTime& evalTime);
	void getCurrentActiveTournRule(BankerTournRuleEvalData& tournRuleEvalData, const SrvTime& evalTime);

	bool isRingHandQualifiedAnyRules(const RingHand& ccdaRingHand);
	bool isTournQualifiedAnyRules(const ColossusTournInfo& tournInfo);

public:
	explicit ColCdaServerObject(_CommInterface& inter);
	~ColCdaServerObject();

	void init(CommMsgBody& initMsg) override;
	void shutdown() override;

	void processCallback(UINT32 reqId, UINT32 msgId, const CommMsgBody& body, CommClientGConnection::AsyncCall* pCall);

	void addMessageTicks(HiresCounter& counter);
	void processMessage(_CommMsg& msg) override;
	void processCommonServerMessage(COLCDA_CONN_TYPE connType, UINT32 msgId, const CommMsgBody& body, CommServerConnection* conn);
	void processClientGMessage(UINT32 requestId, UINT32 msgId, const CommMsgBody& body);

	void processTimerMessage(UINT32 handle, UINT32 msgId, CommMsgBody& body) override;

	void processRates(const CommMsgBody& body );

	INT64 convertCurrencyM64(INT64 amtSrc, const char* curSrc, const char* curDst) const;
	INT64 convertUserWagerAmt(const WageringPerCurrVec& wagerCurVec, const char* desCur);
	INT64 convertAmountCurr(const AmountPerCurrMap& amountPerCurrMap, const char* desCur);
	void convertCDHEventCurrency(ColShd::CdhEvent& cdhEvent);
	void convertRingHandCurrency(RingHand& ringHand);
	void convertTournDataCurrency(TournData& tournData);

	// process Colossus message
	void processRingHand(CommMsgParser& parser);
	void processTournHand(CommMsgParser& parser);
	void processCDHHand(CommMsgParser& parser);

	// process admin messages
	void gotCreateRuleReply(CommMsgParser& parser, UINT32 connId_, UINT32 reqId);
	void gotModifyRuleReply(CommMsgParser& parser, UINT32 connId_, UINT32 reqId);
	void gotActivateRuleReply(CommMsgParser& parser, UINT32 connId_, UINT32 reqId);
	void gotReorderRuleReply(CommMsgParser& parser, UINT32 connId_, UINT32 reqId);
	void gotDeleteRuleReply(CommMsgParser& parser, UINT32 connId_, UINT32 reqId);
	void gotReadRuleReply(CommMsgParser& parser, UINT32 connId_, UINT32 reqId);
	void gotReadCurrRuleListReply(CommMsgParser& parser, UINT32 connId_, UINT32 reqId);
	void gotReadFullRuleListReply(CommMsgParser& parser, UINT32 connId_, UINT32 reqId);
	void gotRuleHistoryReply(CommMsgParser& parser, UINT32 connId_, UINT32 reqId);
	
	// ColWhitelist messages
	void gotCurrColWhitelist(CommMsgParser& parser, UINT32 connId_, UINT32 reqId);

	// Tourn methods
	void processTournNew(CommMsgParser& parser, bool isTournCreateMessage);
	void processTournClosed(CommMsgParser& parser, UINT32 hostId);
	void processGetTournInfo(CommMsgParser& parser);
	void processReceiveTournData(const CommMsgBody& body, TournRetryData& tournRetryData);
	void processUpdateTournStructure(CommMsgParser& parser);

	// Transact methods
	void processTransact(CommMsgParser& parser);

	// Ring game rule evaluation methods
	void triggerRingEval(const PString& userId, const SrvTime& evalTime);
	void evalRingGameCDHRules(BankerRingGameEvalData& bankerRingGameEvalData);
	void evalRingGameHandRules(BankerRingGameEvalData& bankerRingGameEvalData);

	void requestRingGameDBMData(BankerRingGameEvalData& bankerRingGameEvalData);
	void gotRingGameDBMData(CommMsgParser& parser, BankerRingGameEvalData& bankerRingGameEvalData);
	void evalRingGameDBMData(const CCDAUserDBMDataMap& userDBMDataMap, BankerRingGameEvalData& bankerRingGameEvalData);
	
	bool doesRingGameRuleNeedWM(const RingGameRule& ringRule);
	void requestRingGameWMData(BankerRingGameEvalData& bankerRingGameEvalData);
	void gotRingGameWMData(CommMsgParser& parser, BankerRingGameEvalData& bankerRingGameEvalData);
	void evalRingGameWMData(const UserWMDataMap& userWMDataMap, BankerRingGameEvalData& bankerRingGameEvalData);

	bool doesRingGameRuleNeedColRaw(const RingGameRule& ringRule);
	void requestRingGamColRawData(BankerRingGameEvalData& bankerRingGameEvalData);
	void gotRingGameColRawData(CommMsgParser& parser, BankerRingGameEvalData& bankerRingGameEvalData);
	void evalRingGameColRawData(const RingGameRuleColDataMap& ringGameRuleColDataMap, BankerRingGameEvalData& bankerRingGameEvalData);

	void prepareCCDARingGameAlert(BankerRingGameEvalData& bankerRingGameEvalData);

	// Tourn rule evaluation methods
	void triggerTournEval(const PString& userId, const SrvTime& evalTime);
	void evalTournCDHRules(BankerTournRuleEvalData& bankerTournRuleEvalData);
	void evalTournGameRules(BankerTournRuleEvalData& bankerTournRuleEvalData);

	void requestTournGameDBMData(BankerTournRuleEvalData& bankerTournRuleEvalData);
	void gotTournGameDBMData(CommMsgParser& parser, BankerTournRuleEvalData& bankerTournRuleEvalData);
	void evalTournGameDBMData(const CCDAUserDBMDataMap& userDBMDataMap, BankerTournRuleEvalData& bankerTournRuleEvalData);

	void requestTournGameWMData(BankerTournRuleEvalData& bankerTournRuleEvalData);
	void gotTournGameWMData(CommMsgParser& parser, BankerTournRuleEvalData& bankerTournRuleEvalData);
	void evalTournGameWMData(const UserWMDataMap& userWMDataMap, BankerTournRuleEvalData& bankerTournRuleEvalData);

	void requestTournGamColRawData(BankerTournRuleEvalData& bankerTournRuleEvalData);
	void gotTournGameColRawData(CommMsgParser& parser, BankerTournRuleEvalData& bankerTournRuleEvalData);
	void evalTournGameColRawData(const TournRuleColDataMap& tournGameRuleColDataMap, BankerTournRuleEvalData& bankerTournRuleEvalData);

	void prepareCCDATournAlert(BankerTournRuleEvalData& bankerTournRuleEvalData);

	void sendCCDAAlert(const PString& userId, const CCDAAlertData& alertData, CCDARuleType ruleType, const vector<PString>& dumpers);
	void gotCreateAlertReply(CommMsgParser& parser, const ColCDA::CCDAAlert& alert);

	// Velocity control methods
	void startEvaluation();
	void addSRMStats(UINT32 autoAction, const SrvTime& srmTime, const char* userId, const char* dumperId);
	bool shouldRunEvaluation() const;
	void processVelocityControlNotification();
	void processGotIniDataReply(CommMsgParser& parser);
	void processGotEmailSentReply(CommMsgParser& parser);

	// Helpers
	void logServerMsg(const char* prefix, UINT32 msgId, CommServerConnection* conn, const char* userId);
	void replyMsg(UINT32 connId, UINT32 msgId, CommMsgBody& body);
	void setReply(UINT32 reqId, INT16 errCode, char const* errDesc, CommMsgBody& reply);
	void postReply(CommServerConnection* conn, UINT32 msgId, CommMsgBody& body);
	// C
	UINT32 postToLoginAuth(UINT32 msgId, CommMsgBody& body, CommClientGConnection::AsyncCall* call)
	{
		return loginAuthConn.post(msgId, body, call);
	}
	UINT32 postToReader(UINT32 msgId, CommMsgBody& body, CommClientGConnection::AsyncCall* pCall = 0)
	{
		return colReaderConn.post(msgId, body, pCall);
	}
	UINT32 postToColDbm(UINT32 msgId, CommMsgBody& body, CommClientGConnection::AsyncCall* call)
	{
		return colDbmConn.post(msgId, body, call);
	}
	UINT32 postToColRawDbm(UINT32 msgId, CommMsgBody& body, CommClientGConnection::AsyncCall* call)
	{
		return colRawDbmConn.post(msgId, body, call);
	}
	UINT32 postToColDbmWriter(UINT32 msgId, CommMsgBody& body, CommClientGConnection::AsyncCall* call)
	{
		return colDbmWriterConn.post(msgId, body, call);
	}
	UINT32 postToWagerMeas(UINT32 msgId, CommMsgBody& body, CommClientGConnection::AsyncCall* call)
	{
		return wagerMeasConn.post(msgId, body, call);
	}
	UINT32 postToCollusion(UINT32 msgId, CommMsgBody& body, CommClientGConnection::AsyncCall* call)
	{
		return collusionConn.post(msgId, body, call);
	}
	UINT32 postToOlap(UINT32 msgId, CommMsgBody& body, CommClientGConnection::AsyncCall* pCall = 0)
	{
		return olapConn.post(msgId, body, pCall);
	}
	UINT32 postToTemplateSrv(UINT32 msgId, CommMsgBody& body, CommClientGConnection::AsyncCall* pCall = 0)
	{
		return templateSrvConn.post(msgId, body, pCall);
	}

	void reportPerformance();
	void dynamicInit();
	void dynamicInit_Logger(const PIniFile::Section& section);
	void dynamicInit_DynamicConnect(const PIniFile::Section& section);
	void reconnect(CommClientGConnection& conn, const char* const serverName, const char* const serverObject, const char* const serverConnectionType);
	void stopHeLogger();
	void startHeLogger(const char* serverAddress, const char* stationName);

	// Process server messages
	void processAdminMessage(UINT32 msgId, const CommMsgBody & body, CommServerConnection* conn);
	void processTrustedAdminMessage(UINT32 msgId, const CommMsgBody& body, CommServerConnection* conn);
	void processOlapMDMessage(UINT32 msgId, const CommMsgBody& body, CommServerConnection* conn);
	void processColossusMessage(UINT32 msgId, const CommMsgBody& body, CommServerConnection* conn);
	void processColUtilMessage(UINT32 msgId, const CommMsgBody& body, CommServerConnection* conn);

	// Whitelist handlers
	void processColWhitelistCreate(CommMsgParser& parser);
	void processColWhitelistModify(CommMsgParser& parser);
	void processColWhitelistDelete(CommMsgParser& parser);

	// velocity control handlers
	void processGetEvaluationStatus(UINT32 reqId, CommServerConnection& conn);
	void processStartEvaluation(UINT32 reqId, CommServerConnection& conn);
	void gotGetVelocityControlReply(CommMsgParser& parser, UINT32 connId_, UINT32 reqId);
	void gotSetVelocityControlReply(CommMsgParser& parser, UINT32 connId_, UINT32 reqId);

	bool checkAdminRight(const char* rightName, const vector<PString> adminRights) const;
	void requestUserAuth(CommMsgBody& request, AdminConnection* conn);
	void receiveAuthFromDbm(CommMsgParser& parser, UINT32 connId);
	bool checkAdminRight(const char* right, const AdminConnection& pConn);
};

////////////////////////////////////////////////////////////////////////////////

class ColCdaServerObjectFactory : public CommServerObjectFactory
{
public:
	CommServerObject* createServerObject(_CommInterface& inter) const override
	{
		return new ColCdaServerObject(inter);
	}
};

////////////////////////////////////////////////////////////////////////////////

class ColCdaAuthAsyncCall : public CommClientGConnection::AsyncCall
{
	ColCdaServerObject* serverObj;
	UINT32 connId;

	virtual void processReply(UINT32 /* requestId */, UINT32 /* msgId */, const CommMsgBody& body)
	{
		// DBM_A_USER_AUTH
		CommMsgParser parser(body);
		serverObj->receiveAuthFromDbm(parser, connId);
	}

public:
	ColCdaAuthAsyncCall(ColCdaServerObject* obj_, UINT32 connId_)
	{
		serverObj = obj_;
		connId = connId_;
	}
};

////////////////////////////////////////////////////////////////////////////////

class ColUtilSrvCall : public CommClientGConnection::AsyncCall
{
	ColCdaServerObject* obj;
	UINT32 connId;
	UINT32 reqId;

public:
	ColUtilSrvCall(ColCdaServerObject* obj_, UINT32 connId_, UINT32 reqId_)
		: obj(obj_), connId(connId_), reqId(reqId_)
	{
	}

	void processReply(UINT32 /*reqId*/, UINT32 msgId, const CommMsgBody& body) override
    {
        CommMsgParser parser(body);
		switch (msgId)
		{
		case COL_A_CCDA_CREATE_RULE:
			obj->gotCreateRuleReply(parser, connId, reqId);
			break;
		case COL_A_CCDA_MODIFY_RULE:
			obj->gotModifyRuleReply(parser, connId, reqId);
			break;
		case COL_A_CCDA_ACTIVATE_RULE:
			obj->gotActivateRuleReply(parser, connId, reqId);
			break;
		case COL_A_CCDA_REORDER_RULE:
			obj->gotReorderRuleReply(parser, connId, reqId);
			break;
		case COL_A_CCDA_DELETE_RULE:
			obj->gotDeleteRuleReply(parser, connId, reqId);
			break;
		case COL_A_CCDA_READ_RULE:
			obj->gotReadRuleReply(parser, connId, reqId);
			break;
		case COL_A_CCDA_READ_CURRENT_RULE_LIST:
			obj->gotReadCurrRuleListReply(parser, connId, reqId);
			break;
		case COL_A_CCDA_READ_FULL_RULE_LIST:
			obj->gotReadFullRuleListReply(parser, connId, reqId);
			break;
		case COL_A_CCDA_GET_RULE_HISTORY:
			obj->gotRuleHistoryReply(parser, connId, reqId);
			break;
		case COL_A_COLWHITELIST_GET_CURR_LIST:
			obj->gotCurrColWhitelist(parser, connId, reqId);
			break;
		case COL_A_CCDA_GET_VELOCITY_CONTROL_RULE:
			obj->gotGetVelocityControlReply(parser, connId, reqId);
			break;
		case COL_A_CCDA_SET_VELOCITY_CONTROL_RULE:
			obj->gotSetVelocityControlReply(parser, connId, reqId);
			break;
		default:
			PLog("ColUtilSrvCall: Unknown msgId=%u", msgId);
			break;
		}
	}
};

//////////////////////////////////////////////////////////////////////////////////////////

#endif // ColCdaObject_h_included
