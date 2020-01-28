#ifndef admincommon_h_included
#define admincommon_h_included

//
// Add generic (common among several modules) Admin messages here with ID < 100000
//

//	PYR-67119
#define MSG_ADMIN_REREAD_INIDATA				99996	//	4 - reqId
#define MSG_ADMIN_REREAD_INIDATA_REPLY			99997	//	46[s] - reqId,errCode,[errDesc] 


// #PYR-21818
#define MSG_ADMIN_REOPEN_LOG_FILE			99998 // 4 - reqId
#define MSG_ADMIN_REOPEN_LOG_FILE_REPLY	99999 // 46[s] - reqId, errCode, [errDescr]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_ADMIN_REREAD_INI					100000 //4 reqId
#define MSG_ADMIN_REREAD_INI_REPLY				100001 //46[s] reqId, err_code, [errDescr]
// PYR-19031
#define MSG_ADMIN_KILL_ITSELF					100002 //4 reqId
#define MSG_ADMIN_KILL_ITSELF_REPLY				100003 //46[s] reqId, err_code, [errDescr]

#define MSG_ADMIN_KILL_OBJECT					100004 //4s reqId,object name
#define MSG_ADMIN_KILL_OBJECT_REPLY				100005 //46[s] reqId, err_code, [errDescr]
// PYR-19591
// prepare the module for shutdown but don't shutdown it yet
#define MSG_ADMIN_PREPARE_FOR_SHUTDOWN          100006 //4 reqId
#define MSG_ADMIN_PREPARE_FOR_SHUTDOWN_REPLY    100007 //46[s] reqId, err_code, [errDescr]

// PYR-16706 Swith NEMID full check for .DK
#define MSG_ADMIN_NEMID_FULLCHECK_ON            100008 //4 reqId
#define MSG_ADMIN_NEMID_FULLCHECK_ON_REPLY      100009 //46[s] reqId, err_code, [errDescr]
#define MSG_ADMIN_NEMID_FULLCHECK_OFF           100010 //4 reqId
#define MSG_ADMIN_NEMID_FULLCHECK_OFF_REPLY     100011 //46[s] reqId, err_code, [errDescr]
#define MSG_ADMIN_NEMID_FULLCHECK_STATE         100012 //4 reqId
#define MSG_ADMIN_NEMID_FULLCHECK_STATE_REPLY   100013 //46[b|s] reqId, err_code, {state (0 - off, 1 - on)|[errDescr]}

#define MSG_ADMIN_REREAD_MSGS					100014 //4 reqId
#define MSG_ADMIN_REREAD_MSGS_REPLY				100015 //46[s] reqId, err_code, [errDescr]

#define MSG_ADMIN_NEMID_IGNORE_PIDCHK_ON                 100016 //4 reqId
#define MSG_ADMIN_NEMID_IGNORE_PIDCHK_ON_REPLY           100017 //46[s] reqId, err_code, [errDescr]
#define MSG_ADMIN_NEMID_IGNORE_PIDCHK_OFF                100018 //4 reqId
#define MSG_ADMIN_NEMID_IGNORE_PIDCHK_OFF_REPLY          100019 //46[s] reqId, err_code, [errDescr]
#define MSG_ADMIN_NEMID_IGNORE_PIDCHK_STATE              100020 //4 reqId
#define MSG_ADMIN_NEMID_IGNORE_PIDCHK_STATE_REPLY        100021 //46[b|s] reqId, err_code, {state (0 - off, 1 - on)|[errDescr]}
#define MSG_ADMIN_NEMID_IGNORE_PID_NWERROR_ON            100022 //4 reqId
#define MSG_ADMIN_NEMID_IGNORE_PID_NWERROR_ON_REPLY      100023 //46[s] reqId, err_code, [errDescr]
#define MSG_ADMIN_NEMID_IGNORE_PID_NWERROR_OFF           100024 //4 reqId
#define MSG_ADMIN_NEMID_IGNORE_PID_NWERROR_OFF_REPLY     100025 //46[s] reqId, err_code, [errDescr]
#define MSG_ADMIN_NEMID_IGNORE_PID_NWERROR_STATE         100026 //4 reqId
#define MSG_ADMIN_NEMID_IGNORE_PID_NWERROR_STATE_REPLY   100027 //46[b|s] reqId, err_code, {state (0 - off, 1 - on)|[errDescr]}

// #20516 
#define MSG_ADMIN_VOUCHER_GENERATE_BATCH                 100028 //4444s reqId,denomination,totalbatchamount,flag(processor_gw),currency
#define MSG_ADMIN_VOUCHER_GENERATE_BATCH_REPLY           100029 //42[s|s] reqId,err[errStr|filename]

// #PYR-21818
#define MSG_ADMIN_REOPEN_LOG_FILE_LDBM			100030 // 4 - reqId
#define MSG_ADMIN_REOPEN_LOG_FILE_LDBM_REPLY	100031 // 46[s] - reqId, errCode, [errDescr]

// PYR-23900
#define MSG_ADMIN_LIFT_TEMP_TABLE_STARTER_BAN            100032
#define MSG_ADMIN_LIFT_TEMP_TABLE_STARTER_BAN_REPLY      100033

// PYR-24906
#define MSG_ADMIN_IOS_PUSH_NOTIFICATION               100034//4tss4<4s> - ReqId, expirytime, notification text, url, total of tokens, <siteid, token>
#define MSG_ADMIN_IOS_PUSH_NOTIFICATION_REPLY         100035//46[s] - reqId, errCode, [errDescr] 

#define MSG_ADMIN_IOS_CHECK_NOTIFICATION_LENGTH       100036//44<4ss> - ReqId, total,<langid, notification text, url>
#define MSG_ADMIN_IOS_CHECK_NOTIFICATION_LENGTH_REPLY 100037//46[s|4<4>]  - reqId, errCode, [errDescr,total,<langid>]

// PYR-24968
#define MSG_ADMIN_ANDROID_PUSH_NOTIFICATION           100038//4tss4<s> - ReqId, expirytime, notification text, url, otal of tokens, <siteid, token>
#define MSG_ADMIN_ANDROID_PUSH_NOTIFICATION_REPLY     100039//46[s] - reqId, errCode, [errDescr] 

// PYR-25449
#define MSG_ADMIN_UPDATE_SPAIN_MD5_KEY                100040 // 4s - reqId, base64md5keystring
#define MSG_ADMIN_UPDATE_SPAIN_MD5_KEY_REPLY          100041 // 46s - reqId, errCode, errDescr

// PYR-26426
#define MSG_ADMIN_INSERT_PLAYCHIPSCONFIG              100042 // 488446 - reqId, amount, playchipsamt, paytier, platform, popular
#define MSG_ADMIN_INSERT_PLAYCHIPSCONFIG_REPLY        100043 // 46[s|4] - reqId, errCode, [errStr | configId]

#define MSG_ADMIN_GET_AVAILABLE_PLAYCHIPSCONFIG       100044 // 4 - reqID
#define MSG_ADMIN_GET_AVAILABLE_PLAYCHIPSCONFIG_REPLY 100045 // 46[s|4<struct PlayChipsConfig>] - reqId, errCode, [errStr | total, <struct PlayChipsConfig>]

#define MSG_ADMIN_REMOVE_PLAYCHIPSCONFIG              100046 // 44 - reqID, configID
#define MSG_ADMIN_REMOVE_PLAYCHIPSCONFIG_REPLY        100047 // 42[s] - reqID, errCode [errStr]

#define MSG_ADMIN_FULLTILT_ACCOUNT_VIEW               100048 // 44 - reqId, userIntId
#define MSG_ADMIN_FULLTILT_ACCOUNT_VIEW_REPLY         100049 // 4s - reqId, respData

#define MSG_ADMIN_FULLTILT_ACCOUNT_VIEW_BY_EMAIL               100050 // 4s - reqId, useremail
#define MSG_ADMIN_FULLTILT_ACCOUNT_VIEW_BY_EMAIL_REPLY         100051 // 4s - reqId, respData

#define MSG_ADMIN_FULLTILT_ACCOUNT_VIEW_BY_USERID              100052 // 4s - reqId, userId
#define MSG_ADMIN_FULLTILT_ACCOUNT_VIEW_BY_USERID_REPLY        100053 // 4s - reqId, respData

// PYR-28586
#define MSG_ADMIN_ES_Q_VERIFY_PLAYER                  100054 // 4T reqId, <struct EsNicAndBanCheckRequest in spaingateway.h>
#define MSG_ADMIN_ES_A_VERIFY_PLAYER                  100055 // 42[s|444] reqId, errCode, [errDesc | banResult, nicCheckResult, nicCheckErrorType]

// PYR-30612
#define MSG_IOS_PUSH_NOTIFICATION					  100056//4t4<4sss[s4]> - ReqId, expirytime, total of tokens, <siteid, token, text, url [,appKey] [,userIntId]>
#define MSG_IOS_PUSH_NOTIFICATION_REPLY				  100057//46[s] - reqId, errCode, [errDescr] 

// PYR-30612
#define MSG_ANDROID_PUSH_NOTIFICATION				  100058//4t4<sss[s4]> - ReqId, expirytime, total of tokens, <siteid, token, text, url [,appKey][, userIntId]>
#define MSG_ANDROID_PUSH_NOTIFICATION_REPLY			  100059//46[s] - reqId, errCode, [errDescr] 

// PYR-47770  see sportsbook\SB_protocols\Admin_Protocols.h struct SB_Admin_GetNetLossesInPeriod
#define MSG_ADMIN_GET_NET_LOSSES_IN_PERIOD			  100060//44t1<sss> - ReqId, userIntId, tStamp, flags
#define MSG_ADMIN_GET_NET_LOSSES_IN_PERIOD_REPLY	  100061//46[s] - reqId, errCode, [errDescr | placed,losses,tPrague,tToronto]

// PYR-38549
#define MSG_ADMIN_INSERT_PAYPAL_CHARGEBACK            100062 // 44ssssss invoiceid,grossamnt,grosscurrency,transactionid,transEventCode,caseType,TRRfilename,DDRfilename 
#define MSG_ADMIN_INSERT_PAYPAL_CHARGEBACK_REPLY      100063 // 46s reqId,errCode,errStr



// PYR-46043
#define MSG_ADMIN_CREATE_BUNDLE				100064 // 4s2s884<M(s88)>4<M(44<M(4s)>)> - reqID, name, type, imageName, chipAmount, regularChipAmount, numPrices<M(currency, regPrice, price)>, numProdIDs<M(productId, numAppProductIDs<M(appStoreEnumValue, appStoreProductID)>)>
#define MSG_ADMIN_CREATE_BUNDLE_REPLY		100065 // 42[s] - reqID, errCode[errDesc]
#define MSG_ADMIN_GET_BUNDLES				100066 // 4 - reqID
#define MSG_ADMIN_GET_BUNDLES_REPLY			100067 // 42[s|4<M(4s2s884<M(s88)>4<M(44<M(4s)>)>tsts)>] - reqID, errCode[errDesc|numBundles<M(bundleID, name, type, imageName, chipAmount, regularChipAmount, numPrices<M(currency, regPrice, price)>, numProdIDs<M(productId, numAppProductIDs<M(appStoreEnumValue, appStoreProductID)>)>, created, createdBy, edited, editedBy)>]
#define MSG_ADMIN_UPDATE_BUNDLE				100068 // 44s2s884<M(s88)>4<M(44<M(4s)>)> - reqID, bundleID, name, type, imageName, chipAmount, regularChipAmount, numPrices<M(currency, regPrice, price)>, numProdIDs<M(productId, numAppProductIDs<M(appStoreEnumValue, appStoreProductID)>)>
#define MSG_ADMIN_UPDATE_BUNDLE_REPLY		100069 // 42[s] - reqID, errCode[errDesc]
#define MSG_ADMIN_DELETE_BUNDLE				100070 // 44 - reqID, bundleID
#define MSG_ADMIN_DELETE_BUNDLE_REPLY		100071 // 42[s] - reqID, errCode[errDesc]

#define MSG_ADMIN_CREATE_SEGMENT			100072 // 4ss4444444<s>4<s>884<4>tt44tt44tt444444 - reqID, name, description, brandMask, siteMask, appTypeMask, osIDMask, clientDeviceMask, productMask, numCountries<country>, numTrackCodes<trackCode>, playerFlags2, playerPriv3, numIntIDFilters<intId>, registerDateRangeStart, registerDateRangeEnd, registerDateRangeOffsetStart, registerDateRangeOffsetEnd, lastLoginRangeStart, lastLoginRangeEnd, pmBalanceMin, pmBalanceMax, lastPurchaseTimeRangeStart, lastPurchaseTimeRangeEnd, lastPMPurchaseMin, lastPMPurchaseMax, last30DaySpentMin, last30DaySpentMax, avgSpendAmt, sortOrder
#define MSG_ADMIN_CREATE_SEGMENT_REPLY		100073 // 42[s] - reqID, errCode[errDesc]
#define MSG_ADMIN_GET_SEGMENTS				100074 // 4 - reqID
#define MSG_ADMIN_GET_SEGMENTS_REPLY		100075 // 42[s|4<M(4ss4444444<s>4<s>884<4>tt44tt44tt444444tsts)>] - reqID, errCode[errDesc|numSegments<M(segmentID, name, description, brandMask, siteMask, appTypeMask, osIDMask, clientDeviceMask, productMask,  numCountries<country>, numTrackCodes<trackCode>, playerFlags2, playerPriv3, numIntIDFilters<intId>, registerDateRangeStart, registerDateRangeEnd, registerDateRangeOffsetStart, registerDateRangeOffsetEnd, lastLoginRangeStart, lastLoginRangeEnd, pmBalanceMin, pmBalanceMax, lastPurchaseTimeRangeStart, lastPurchaseTimeRangeEnd, lastPMPurchaseMin, lastPMPurchaseMax, last30DaySpentMin, last30DaySpentMax, avgSpendAmt, sortOrder, created, createdBy, edited, editedBy)>]
#define MSG_ADMIN_UPDATE_SEGMENT			100076 // 44ss4444444<s>4<s>884<4>tt44tt44tt444444 - reqID, segmentID, name, description, brandMask, siteMask, appTypeMask, osIDMask, clientDeviceMask, productMask, numCountries<country>, numTrackCodes<trackCode>, playerFlags2, playerPriv3, numIntIDFilters<intId>, registerDateRangeStart, registerDateRangeEnd, registerDateRangeOffsetStart, registerDateRangeOffsetEnd, lastLoginRangeStart, lastLoginRangeEnd, pmBalanceMin, pmBalanceMax, lastPurchaseTimeRangeStart, lastPurchaseTimeRangeEnd, lastPMPurchaseMin, lastPMPurchaseMax, last30DaySpentMin, last30DaySpentMax, avgSpendAmt, sortOrder
#define MSG_ADMIN_UPDATE_SEGMENT_REPLY		100077 // 42[s] - reqID, errCode[errDesc]
#define MSG_ADMIN_DELETE_SEGMENT			100078 // 44 - reqID, segmentID
#define MSG_ADMIN_DELETE_SEGMENT_REPLY		100079 // 42[s] - reqID, errCode[errDesc]

#define MSG_ADMIN_CREATE_PAYWALL			100080 // 4ss444<M(42)>2ttb - reqID, name, description, priority, segmentID, numBundles<M(bundleID, bannerType)>, status, activateStart, activateEnd, isFallback
#define MSG_ADMIN_CREATE_PAYWALL_REPLY		100081 // 42[s] - reqID, errCode[errDesc]
#define MSG_ADMIN_GET_PAYWALLS				100082 // 4 - reqID
#define MSG_ADMIN_GET_PAYWALLS_REPLY		100083 // 42[s|4<M(M(4ss444<M(42)>2tststtb)2bb)>] - reqID, errCode[errDesc|numPaywalls<M(M(paywallID, name, description, priority, segmentID, numBundles<M(bundleID, bannerType)>, status, created, createdBy, edited, editedBy, activateStart, activateEnd, isFallback), sortOrder, hasBestValue, hasMostPopular)>]
#define MSG_ADMIN_UPDATE_PAYWALL			100084 // 44ss444<M(42)>2ttb - reqID, paywallID, name, description, priority, segmentID, numBundles<M(bundleID, bannerType)>, status, activateStart, activateEnd, isFallback
#define MSG_ADMIN_UPDATE_PAYWALL_REPLY		100085 // 42[s] - reqID, errCode[errDesc]
#define MSG_ADMIN_DELETE_PAYWALL			100086 // 44 - reqID, paywallID
#define MSG_ADMIN_DELETE_PAYWALL_REPLY		100087 // 42[s] - reqID, errCode[errDesc]
#define MSG_ADMIN_GET_PAYWALL_BUNDLES		100088 // 4 - reqID, paywallID
#define MSG_ADMIN_GET_PAYWALL_BUNDLES_REPLY	100089 // 42[s|4<M(M(4s2s884<M(s88)>4<M(44<M(4s)>)>tsts)2)>] - reqID, errCode[errDesc|numBundles<M(M(bundleID, name, type, imageName, chipAmount, regularChipAmount, numPrices<M(currency, regPrice, price)>, numProdIDs<M(productId, numAppProductIDs<M(appStoreEnumValue, appStoreProductID)>)>, created, createdBy, edited, editedBy), bannerType)>]

// PYR-83014
#define MSG_ADMIN_BROADCAST_REREAD_INI          100090 //4 reqId
#define MSG_ADMIN_BROADCAST_REREAD_INI_REPLY    100091 //46[s] reqId, err_code, [errDescr]

// PYR-105007
#define MSG_ADMIN_RESEND_ASIA_REPORT_MESSAGES       100092 //44tt - ReqId, ReqMsgId, tStampStart, tStampEnd
#define MSG_ADMIN_RESEND_ASIA_REPORT_MESSAGES_REPLY 100093 //46[s|44] reqId, err_code [ errDescr | totalMessagesCount, totalErrorMessagesCount]

#endif
