////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2017 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef colutilserver_h_included
#define colutilserver_h_included

////////////////////////////////////////////////////////////////////////////////

#define COLUTILSRV_INTERNAL_CONNTYPE						"colutilserver"
#define COLUTILSRV_LOGINAUTH_CONNTYPE						"colutilserver"
#define COLUTILSRV_WEB_CONNTYPE								"web"
#define COLUTILSRV_ADMIN_CONNTYPE							"admin"
#define COLUTILSRV_TRUSTEDADMIN_CONNTYPE					"trustedAdmin"

#define COLUTILSRV_INIDATA_OBJ_NAME							"colossus"
#define COLUTILSRV_INIDATA_CCDA_SECTION						"colutil.ccda"
#define COLUTILSRV_INIDATA_CCDA_EMAIL_ITEM					"email"

////////////////////////////////////////////////////////////////////////////////

#define COLSRV_Q_SHD_FREQPARTNERS_REPORT	85001			// 4* - reqId, {ShdFreqPartnersInputs}
#define COLSRV_A_SHD_FREQPARTNERS_REPORT	85002			// 42[s|4<*>] - reqId, errCode, [errBuf|numItems, <{ShdAggPPSRecord}>]

#define COLSRV_Q_STATS_FOR_HAND				85003			// 4* - reqId, {GetHandStatsInputs}
#define COLSRV_A_STATS_FOR_HAND				85004			// 42[s|4<*>] - reqId, errCode, [errBuf|numItems, <{RawHandDataEx}>]

#define COLSRV_Q_AMPS_SHD_HANDS				85005			// 4* - reqId, {ColAmps::GetShdHandsInputs}
#define COLSRV_A_AMPS_SHD_HANDS				85006			// 42[s|*] - reqId, errCode, [errBuf|{ColAmps::GetShdHandsOutputs}]

//#define COLSRV_Q_AMPS_TOP_PARTNERS			85007			// 4* - reqId, {ColAmps::TopPartnersInputs}
//#define COLSRV_A_AMPS_TOP_PARTNERS			85008			// 42[s|4<*>4<*>] - reqId, errCode, [errBuf|numItems, <{ColAmps::TopSngPartnersRecord}>, numItems, <{ColAmps::TopRingPartnersRecord}>]
//
//#define COLSRV_Q_AMPS_GAME_ACTIVITY			85009			// 4* - reqId, {ColAmps::GameActivityInputs}
//#define COLSRV_A_AMPS_GAME_ACTIVITY			85010			// 42[s|4<*>4<*>] - reqId, errCode, [errBuf|numItems, <{ColAmps::SngGameActivityRecord}>, numItems, <{ColAmps::RingGameActivityRecord}>]

#define COLSRV_Q_AMPS_TOP_PARTNERS			85007			// 4* - reqId, {ColAmps::TopPartnersInputs}
#define COLSRV_A_AMPS_TOP_PARTNERS			85008			// 42[s|4<*>4<*>dddd**] - reqId, errCode, [errBuf|numItems, <{ColAmps::TopSngPartnersRecord}>, numItems, <{ColAmps::TopRingPartnersRecord}>], sngStartDate, sngEndDate, ringStartDate, ringEndDate, ColAmps::TopSngPartners_RootUser, ColAmps::TopRingPartners_RootUser

#define COLSRV_Q_AMPS_GAME_ACTIVITY			85009			// 4* - reqId, {ColAmps::GameActivityInputs}
#define COLSRV_A_AMPS_GAME_ACTIVITY			85010			// 42[s|4<*>4<s4>4<*>4<s4>dddd] - reqId, errCode, [errBuf|numItems, <{ColAmps::SngGameActivityRecord}>, numItems, <userId, numTotalSNGTourns>, numItems, <{ColAmps::RingGameActivityRecord}>, numItems, <userId, numTotalRingHands>, sngStartDate, sngEndDate, ringStartDate, ringEndDate]

#define COLSRV_Q_AMPS_GAMES_TOGETHER		85011			// 4* - reqId, {ColAmps::GamesTogetherInputs}
#define COLSRV_A_AMPS_GAMES_TOGETHER		85012			// 42[s|**] - reqId, errCode, [errBuf|{ColAmps::SngGamesTogetherOutputs}, {ColAmps::RingGamesTogetherOutputs}]

#define COLSRV_Q_AMPS_SHD_HANDS2			85013			// 4* - reqId, {ColAmps::GetShdHandsInputs}
#define COLSRV_A_AMPS_SHD_HANDS2			85014			// 42[s|*] - reqId, errCode, [errBuf|{ColAmps::GetShdHandsOutputs}]

#define COLSRV_Q_WHITELIST_CREATE			85015			// 4* - reqId, {ColWhitelist.Create}
#define COLSRV_A_WHITELIST_CREATE			85016			// 42[s|*] - reqId, errCode, [errBuf|ColWhitelist]

#define COLSRV_Q_WHITELIST_MODIFY			85017			// 4* - reqId, {ColWhitelist.Modify}
#define COLSRV_A_WHITELIST_MODIFY			85018			// 42[s|*] - reqId, errCode, [errBuf|ColWhitelist]

#define COLSRV_Q_WHITELIST_DELETE			85019			// 4* - reqId, {ColWhitelist.Delete}
#define COLSRV_A_WHITELIST_DELETE			85020			// 42[s|*] - reqId, errCode, [errBuf|ColWhitelist]

#define COLSRV_Q_WHITELIST_GET_CURR_LIST	85021			// 444 - reqId, ModuleIds, WhitelistTypes, state
#define COLSRV_A_WHITELIST_GET_CURR_LIST	85022			// 42[s|4<*>] - reqId, errCode, [errBuf|num<ColWhitelist>]

#define COLSRV_Q_WHITELIST_GET_HISTORY		85023			// 44 - reqId, recordId
#define COLSRV_A_WHITELIST_GET_HISTORY		85024			// 42[s|4<*>] - reqId, errCode, [errBuf|num<ColWhitelist>]

#define COLSRV_Q_WHITELIST_GET_ENTRY		85025			// 444 - reqId, recordId, auditId
#define COLSRV_A_WHITELIST_GET_ENTRY		85026			// 42[s|*] - reqId, errCode, [errBuf|{ColWhiteList}]

//PYR-87500
#define COLSRV_Q_NORMALIZE_USERIDS			85027			// 44<s> - reqId, numUsers <userid>
#define COLSRV_A_NORMALIZE_USERIDS			85028			// 42[s|4<s>] - reqId, errCode, numUsers <userid>

#define COLSRV_Q_GET_CDH_EVENTS				85029			// 4 - reqId, {}
#define COLSRV_A_GET_CDH_EVENTS				85030			// 42[s|4<*>] - reqId, errCode, [errBuf|num<>]

// PYR-89537
#define COLSRV_Q_GET_NORMAL_BPS_REPORT		85031			// 4* - reqId, BPS::BPSNormalFilter
#define COLSRV_A_GET_NORMAL_BPS_REPORT		85032			// 42[s|*] - reqId, errCode, [errBuf|BPS::BPSOutputReport]

// PYR-89537
#define COLSRV_Q_GET_XSTAKES_BPS_REPORT		85033			// 4* - reqId, BPS::BPSXStakesFilter
#define COLSRV_A_GET_XSTAKES_BPS_REPORT		85034			// 42[s|*] - reqId, errCode, [errBuf|BPS::BPSOutputReport]

// PYR-89537
#define COLSRV_Q_GET_USER_STAKES_PLAYED		85035			// 4* - reqId, BPS::UserStakesPlayedInputs
#define COLSRV_A_GET_USER_STAKES_PLAYED		85036			// 42[s|*] - reqId, errCode, [errBuf|BPS::FilteredUserStakesPlayed]

// PYR-89537
#define COLSRV_Q_GET_AGG_TABLE_EXTENTS		85037			// 4* - reqId, ColUtils::MaintJobSetupQueryParams
#define COLSRV_A_GET_AGG_TABLE_EXTENTS		85038			// 42[s|4<*>] - reqId, errCode, [errBuf|sz<MaintJobSetupEntry>]

// PYR-106157
#define COLSRV_Q_GET_USER_RAKE_PAID			85039			// 4stt44 - reqId, userId, startTime, endTime, sinceBeginningOfTime, toCurrentTime
#define COLSRV_A_GET_USER_RAKE_PAID			85040			// 42[s|4<M(s88)>] - reqId, errCode, [errBuf|sz<M(currency, rawValue, convToUsdValue)>]

////////////////////////////////////////////////////////////////////////////////

#endif // colutilserver_h_included