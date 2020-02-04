#ifndef ColCdaCacheTypes_h_included
#define ColCdaCacheTypes_h_included

////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "ColossusConstants.h"
#include "CopyablePStringMap.h"
#include "MsgBody.h"
#include "..\ColossusSHD\ColCdhEvent.h"
#include "ColossusCommon.h"
#include "ColCdaRules.h"
#include "wagerMeas.h"
#include "..\shared\CCDAShared.h"

struct ColossusPPTInfo;
struct ColossusHand;
struct RawHandDataEx;

using namespace ColShd;

////////////////////////////////////////////////////////////////////////////////

#define CCDA_LAST_DEPOSIT_NA_RULE		"NA"

namespace ColCDA
{
	struct BankerRingData;
	struct CDHHandCache;
	struct RingRuleData;
	struct TournRuleData;
	struct AggBankerTournData;

	// GPSI-259
	struct CCDAAlert
	{
		PString bankerUserId;
		PString dumperUserId;
		INT32 autoActions;

		CCDAAlert();
		CCDAAlert(const char* bankerUserId_, const char* dumperUserId_, INT32 autoActions_);
	};

	struct CDHAlertInfo
	{
		SrvTime when;
		PString dumperUserId;
		UINT32 bankerChipsEndingCents;

		CDHAlertInfo(const CdhEvent& cdhEvent);
		void toString(UINT64 handId, PString& cdhAlertString) const;
	};

	struct CCDAAlertData
	{
		INT32 autoActions;
		PString parameter;
		map<UINT64 /*HandId*/, CDHAlertInfo> cdhMap;
		CopyablePStringMap<UINT32> relatedPlayerMap;

		void appendRuleInfo(UINT32 ruleId, const char* ruledescription);
		void removeCDHDumper(const char* dumperId);
		CCDAAlertData();
	};

	//////////////////////////////////////////////////////////////////////////////////////

	// aggregated dumper ring game data
	struct AggDumperRingData
	{
		INT32 totalLostToBankerCents;
		INT32 bbLostToBankerScaled100;
		UINT32 totalHandsWithBanker;
		UINT32 cdhHandNum;
		INT32 matachedWagerCents_USD;
		INT32 bankerDeltaCents_USD;

		INT32 totalDeltaCents_USD;	// dumper total delta, played with all players
		INT32 totalWagerCents_USD;	// dumper total wager, played with all players

		AggDumperRingData();

		INT32 getBBPer100LostToBankerScaled100() const;
		INT32 getBBLostToBanker() const;
		INT32 getNetAmountLostToBanker() const;
		void aggregateHand(INT32 amount, UINT32 bbSize, ColGameStructFlags structureFlags);
		void aggDeltaWager(INT32 matchedWager, INT32 delta);
	};

	struct AggBankerRingData
	{
		INT32 bbWonScaled100;
		UINT32 totalHands;
		UINT32 totalPlayersDealtIn;
		UINT32 cdhHandNum;
		INT32 totalDeltaCents_USD;
		INT32 totalWagerCents_USD;

		typedef CopyablePStringMap<AggDumperRingData> DumperDataMap;
		typedef DumperDataMap::iterator DumperDataMapIt;
		typedef DumperDataMap::const_iterator DumperDataMapConstIt;
		DumperDataMap aggDumperData;

		AggBankerRingData();

		INT32 getBBWon() const;
		INT32 getBBWonPer100HandsScaled100() const;
		UINT32 getAvgPlayersDealtInScaled100() const;
		UINT32 getNumDumpers() const;
		INT64 getAllDumpersLostToMeCents() const;
		INT32 getAllDumpersLostToMeBBPer100Scaled100() const;

		void clear();
		void aggregateHand(const PString& bankerName, const RingHand& hand);
		void evalDumperRingHands(const RingGameRule& ringRule, bool moreTrace);
		void evalDumperTotalData(const RingGameRule& ringRule, bool moreTrace);
		void addRingGameCDHHandCount(const CdhEvent& cdhEvent);

	private:
		DumperDataMapIt ensureDumper(const PString& dumperName);
	};

	struct RingHandCache
	{
		typedef map<UINT64 /* handId */, RingHand> ContainerMap;
		typedef PStringMap<set<UINT64>> PlayerAuxCacheMap;
		
		void add(const time_t& threshOldestTime, UINT64 handId, const RingHand& hand);
		void cull(time_t oldestAllowed, UINT32 maxNumHandsAllowed);
		UINT32 size() const { return handsCacheMap.size(); }

		bool isRingRuleQualified(const PString& bankerName, const RingGameRule& ringRule,
								 RingRuleData& ringRuleData, bool moreTrace);

	private:
		ContainerMap handsCacheMap;
		PlayerAuxCacheMap playerAuxCacheMap;

		void addToPlayerAuxCache(UINT64 handId, const PString& playerName);
		void erasePlayersFromAuxCache(UINT64 handId, const vector<UserInRingHand>& playersVec);
		void eraseFromPlayerAuxCache(UINT64 handId, const PString& playerName);

		void aggregateForRule(const PString& playerName, const RingGameRule& rule, RingRuleData& ringRuleData, bool moreTrace);
		void aggregateDumpersTotalData(const PString& playerName, const RingGameRule& rule, RingRuleData& ringRuleData, bool moreTrace);
	};

	// See sharedstmt.cpp - SaveTransactionStmt
	// Or DBM_Q_OLAP_INSERT_TRANSACT
	struct Transact
	{
		UINT64 transId;
		PString userId;
		UINT32 transType;
		UINT32 objectId32;
		INT32 chips;
		INT32 playChips32;
		INT32 fpp;
		INT32 chipsAfter;
		INT32 playChipsAfter32;
		INT32 fppAfter;
		UINT32 adminIntId;
		PString comment;
		INT32 tChips;
		INT32 wChips;
		SrvTime when;
		UINT32 userIntId;
		INT32 tChipsAfter;
		INT32 wChipsAfter;
		INT32 fppCents;
		INT32 fppCentsAfter;
		INT32 baseFpp;
		INT32 vpp1;
		INT32 vpp1After;
		INT32 vpp2;
		INT32 vpp2After;
		bool isPlayMoney;
		PString currency;
		INT16 baseFppCents;
		INT16 vpp1Cents;
		INT16 vpp1AfterCents;
		INT16 vpp2Cents;
		INT16 vpp2AfterCents;
		INT64 playChips;
		INT64 playChipsAfter;
		UINT64 objectId;
		UINT64 appLoginId;

		explicit Transact(CommMsgParser& parser);

		void parse(CommMsgParser& parser);
		void compose(CommMsgBody& body) const;
		bool isRelevant() const;
		bool isRingTableEvent() const;
		bool isBuyinEvent() const;
		bool isLeaveEvent() const;
	};

	struct UserOnGame
	{
		time_t newestEventTime;
		INT32 chips;

		UserOnGame();

		void addTransact(const Transact& transact);
		bool isWinner() const;

		UINT32 memSize() const { return sizeof(UserOnGame);}
	};

	struct PlayerGameCache
	{
		typedef map<UINT64, UserOnGame> UserGameMap;		 // objectId = tableId for ring game, sessionId for zoom game
		typedef PStringMap<UserGameMap> PlayerMap;   // <userId, <objectID, UserOnGame>>
																		   
		void addTransaction(const Transact& transact, bool& shouldTriggerEval);
		void cull(time_t oldestAllowed);
		UINT32 size() const {return (UINT32)playerMap.size();}
	private:
		PlayerMap playerMap;
	};

	/////////////////////////////////////////////////////////////////
	struct CDHHandCache
	{
		typedef PStringMap<list<CdhEvent>> ContainerMap;  // <banker, vector<CDHHand>>
		ContainerMap cdhCacheMap;

		void add(const CdhEvent& cdhEvent);
		void cull(time_t oldestAllowed, UINT32 maxUsersAllowed);
		UINT32 size() const {return (UINT32)cdhCacheMap.size();}

		void fillCDHAlertData(const CdhEvent& cdhEvent, CCDAAlertData& alertData);
		bool checkCommonCDHFilters(const CommonRule& cdaRule, const CdhEvent& cdhEvent, bool moreTrace);
		bool isRingRuleQualified(const PString& bankerName, const RingGameRule& ringRule, RingRuleData& ringRuleData, bool moreTrace);
		bool isTournRuleQualified(const PString& bankerName, const TournRule& sngRule, TournRuleData& tournRuleData, bool moreTrace);
		bool checkDumperLastDepositFilters(const CdhEvent& cdhEvent, const vector<LastDepositParam>& lastDepositParamVec);
		bool checkBankerLastDepositFilters(const CdhEvent& cdhEvent, const vector<LastDepositParam>& lastDepositParamVec);
	};

	struct NewTourn
	{
		time_t createdTime;
		UINT32 numHands;

		NewTourn();
		NewTourn(const SrvTime& when);
	};

	struct NewTournCache
	{
		typedef map<UINT64 /* tournId */, NewTourn> ContainerMap;
		
		void add(UINT64 tournId, const SrvTime& when);
		void erase(UINT64 tournId);
		void cull(time_t oldestAllowed);
		void addTournHand(const TournHand& tournHand);
		bool find(UINT64 tournId, NewTourn& newTourn);
		UINT32 size() const {return (UINT32)newTournCache.size();}

	private:
		ContainerMap newTournCache;
	};

	struct TournPlayer
	{
		PString playerId;
		UINT32 totalInvested_USD;
		UINT32 rakePaid_USD;
		UINT32 payout_USD;

		TournPlayer();
		bool isWinner() const;
		UINT32 memSize() const;
	};

	struct TournData
	{
		time_t finished;
		ColGameTypeFlags gameTypeFlags;
		ColGameStructFlags structureFlags;
		ColGameFlags gameFlags;
		ColGameSpeedFlags tournSpeedFlags;
		ColGameTourneyFlags tournFlags;
		UINT32 buyIn_RuleCurrency;
		PString currency;
		UINT32 numHands;

		vector<TournPlayer> players;

		TournData(const ColossusPPTInfo& info, UINT32 numHands_);
		UINT32 memSize() const;
		void updateTournFlags2(UINT64 tournFlags2);
	};

	struct AggDumperTournData
	{
		UINT32 totalInvested_USD;
		UINT32 rakePaid_USD;
		UINT32 payout_USD;
		UINT32 totalWonTournNum;
		UINT32 totalTournNum;
		UINT32 cdhHandNum;

		AggDumperTournData();

		UINT32 getDumperNumCDHHands() const;
		UINT32 getDumperWinRate_scaled100() const;
		INT32 getDumperROI_scaled100() const;
	};

	struct AggBankerTournData
	{
		UINT32 totalInvested_USD;
		UINT32 rakePaid_USD;
		UINT32 payout_USD;
		UINT32 totalWonTournNum;
		UINT32 totalTournNum;
		UINT32 totalTournHandNum;
		UINT32 cdhHandNum;

		UINT32 getBankerNumCDHHands() const;
		UINT32 getBankerNumTourns() const;
		UINT32 getBankerWinRate_scaled100() const;
		INT32 getBankerROI_scaled100() const;
		UINT32 getBankerAvgHandsPerTourn_scaled100() const;

		UINT32 getAllDumperCount() const;
		UINT32 getAllDumperWinRate_scaled100() const;
		INT32 getAllDumperROI_scaled100() const;

		typedef CopyablePStringMap<AggDumperTournData> DumperDataMap;
		typedef DumperDataMap::iterator DumperDataMapIt;
		typedef DumperDataMap::const_iterator DumperDataMapConstIt;
		DumperDataMap aggDumperData;

		AggBankerTournData();
		void clear();

		void aggregateTourn(const PString& bankerName, const TournData& tournData);
		void evalDumperTournRule(const TournRule& tournRule);

		void addTournCDHHandCount(const CdhEvent& cdhEvent);

	private:
		DumperDataMapIt ensureDumper(const PString& dumperName);
	};

	struct TournDataCache
	{
		typedef map<UINT64 /* tournId */, TournData> ContainerMap;
		typedef PStringMap<set<UINT64>> PlayerAuxCacheMap;

		void add(UINT64 tournId, const TournData& tournData);
		void cull(time_t oldestAllowed, UINT32 maxTournAllowed);
		UINT32 size() const {return (UINT32)tournDataCache.size();}

		bool isTournRuleQualified(const PString& bankerName, const TournRule& tournRule,
								  TournRuleData& tournRuleData, bool moreTrace);
		void updateTournFlags2(UINT64 tournId, UINT64 tournFlags2);

	private:
		ContainerMap tournDataCache;
		PlayerAuxCacheMap playerAuxCacheMap;

		void addToPlayerAuxCache(UINT64 tournId, const PString& playerName);
		void erasePlayersFromAuxCache(UINT64 tournId, const vector<TournPlayer>& playersVec);
		void eraseFromPlayerAuxCache(UINT64 tournId, const PString& playerName);
		     
		void aggregateForRule(const PString& playerName, const TournRule& rule, TournRuleData& tournRuleData);

	};

	/////////////////////////////////////////////////////////////////////////////////
	// Rule evaluation data
	struct CommonRuleData
	{
		time_t evalTime;
		CCDAAlertData alertData;

		CommonRuleData(time_t evalTime_);
	};

	// rule evaluation data for a banker
	struct RingRuleData : public CommonRuleData
	{
		AggBankerRingData aggBankerRingData;

		RingRuleData(time_t evalTime);
	};

	typedef map<RingGameRule, RingRuleData> RingRuleEvalMap;

	struct BankerRingGameEvalData
	{
		PString bankerName;
		RingRuleEvalMap ringRuleEvalMap;

		bool hasQualifiedRules() const; 
	};

	////////////////////////////////////////////////////////////////////////////////
	// Tourn rule data
	struct TournRuleData : public CommonRuleData
	{
		AggBankerTournData aggBankerTournData;
		TournRuleData(time_t evalTime);
	};

	typedef map<TournRule, TournRuleData> TournRuleEvalMap;
	struct BankerTournRuleEvalData
	{
		PString bankerName;
		TournRuleEvalMap tournRuleEvalMap;

		bool hasQualifiedRules() const {return tournRuleEvalMap.size() > 0;}
	};

	////////////////////////////////////////////////////////////////////////////////
	// Velocity Control cache

	struct UserEvaluationCache
	{
		struct UserEvaluation
		{
			PString userId;
			UINT32 evalType;
			SrvTime evalTime;

			UserEvaluation(const char* userId_, UINT32 evalType_, const SrvTime& evalTime_);
		};

		typedef list<UserEvaluation> UserEvaluationList;

		void add(const char* userId_, UINT32 evalType_, const SrvTime& evalTime_);
		UINT32 size() const;

		UserEvaluationList container;
	};
	

	struct SRMMonitorCache
	{
		struct SRMStats
		{
			time_t srmTime;
			PString userId;

			SRMStats(const SrvTime& srmTime_, const char* userId_);

			INT64 compare(const SRMStats& other) const;
			bool operator==(const SRMStats& other) const { return compare(other) == 0; }
			bool operator<(const SRMStats& other) const { return compare(other) < 0; }
		};

		typedef set<SRMStats> SRMStatsSet;

		void add(const SrvTime& srmTime_, const char* userId_);
		void cull(UINT32 lookbackMinutes);
		bool isOverThreshold(UINT32 srmThreshold);
		void clear();
	private:
		SRMStatsSet container;
	};

	
}

////////////////////////////////////////////////////////////////////////////////

#endif // ColCdaCacheTypes_h_included
