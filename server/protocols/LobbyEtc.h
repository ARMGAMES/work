// LobbyEtc.h
#ifndef LobbyEtc_h_included
#define LobbyEtc_h_included

#define ERR_IMAGE_GALLERY_INIT_NOT_DONE 1000

// PYR-58657, PYR-53668
#define URI_DELIMETER	"/"


//#define MSG_LOBBYETC_GET_GUEST_ACCOUNT						1000		//	4M44	- clientSiteId,extraInfo,locale,timezone
//#define MSG_LOBBYETC_GET_GUEST_ACCOUNT_REPLY				1001		//	6[s|ss8] - errCode[errDescr | userId,userPass,startChips]

//	Authenticated message to release a guest account
// !!! WARNING !!! If you modify this message, please let testautomation team know.
//#define MSG_LOBBYETC_GUEST_ACCOUNT_RELEASE					1002		//	
//#define MSG_LOBBYETC_GUEST_ACCOUNT_RELEASE_REPLY			1003		//	6[s] - errCode[errDesc]



//	Available
//#define MSG_LOBBYETC_CLIENT_VISIBILITY_CHANGE				1004		//	44<4> - reqId, arraySize<disabledSiteId>

// renamed from MSG_LOBBYETC_RATHOLE_MANAGER_CONFIG
#define MSG_LOBBYETC_RATHOLE_AND_POOLBLOCK_MANAGER_CONFIG	1005		//44<224>4<224>4<4<4>4<4>> - reqId=0,rh_cfg_size,<rh_threshold, rh_minutes, rh_sites>, zoom_cfg_size,<zoom_threshold, zoom_minutes, zoom_sites>, poolblock_cfg_size, <licenses_size, <licenseId>, vip_levels_size, <vipLevel>>

//	PYR-27341, Authenticated client message
// !!! WARNING !!! If you modify this message, please let testautomation team know.
//#define MSG_LOBBYETC_GUEST_ACCOUNT_GET_CHIPS				1006		//	
//#define MSG_LOBBYETC_GUEST_ACCOUNT_GET_CHIPS_REPLY			1007		//	6[s|88] - errCode[errDesc|totalPMChips,allocatedPMChips]

//DRC <--> LobbyEtc
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBYETC_GET_TABLE_MIN_BUYIN					1200		//4M - reqId,<RatholeManTableData from rhmantable.h>
#define MSG_LOBBYETC_GET_TABLE_MIN_BUYIN_REPLY				1201		//4M(4) - reqId,M(minBuyin)

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBYETC_USER_TABLE_SIT							1202		//4<rec>4 - reqId,<RatholeManTableData from rhmantable.h>,maxTablesAllowed
#define MSG_LOBBYETC_USER_TABLE_SIT_REPLY					1203		//46[s|4] - reqId,errCode,[errDesc|obligationId]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBYETC_USER_TABLE_OUT							1204		//4<rec> - reqId,<RatholeManTableData from rhmantable.h>
#define MSG_LOBBYETC_USER_TABLE_OUT_REPLY					1205		//46s - reqId,errCode=0,errDesc

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBYETC_USER_TABLE_UNSIT						1206		//4<rec>4 - reqId,<RatholeManTableData from rhmantable.h>
#define MSG_LOBBYETC_USER_TABLE_UNSIT_REPLY					1207		//46s - reqId,errCode=0,errDesc

#define MSG_LOBBYETC_USER_TABLE_POOLBLOCK_SITOUT			1208		//4<rec>4 - reqId,<PoolBlockTableData from rhmantable.h>
#define MSG_LOBBYETC_USER_TABLE_POOLBLOCK_SITOUT_REPLY		1209		//46[s] - reqId,errCode,errDesc


//	Admin message - dynamically add new guest accounts
// !!! WARNING !!! If you modify this message, please let testautomation team know.
//#define MSG_LOBBYETC_ADMIN_ADD_NEW_GUEST_ACCOUNTS			2000		//	44<s4> - reqId, arraySize<userid,licenseId>
//#define MSG_LOBBYETC_ADMIN_ADD_NEW_GUEST_ACCOUNTS_REPLY		2001		//	46[s] - reqId, errCode[errDesc]


//	Admin message used for automation testing - return guest account info
//#define MSG_LOBBYETC_ADMIN_GET_GUEST_ACCOUNT_INFO			2002		//	4 - reqId
//#define MSG_LOBBYETC_ADMIN_GET_GUEST_ACCOUNT_INFO_REPLY		2003		//	46[s|44] - reqId, errCode[errDesc|totalNumAccountsAvail,totalNumAccountsInUse]

// PYR-24001
#define MSG_LOBBYETC_LOCALE_SET								2004 // 4 locale #
#define MSG_LOBBYETC_LOCALE_SET_REPLY						2005 // 6[s|44] errCode[ errDescr|locale, emLocale]

// PYR-24001
#define MSG_LOBBYETC_EMLOCALE_SET							2006 // 444 emLocale, emLocale2, emLocale3
#define MSG_LOBBYETC_EMLOCALE_SET_REPLY						2007 // 6[s|4444] errCode[ errDescr|locale, emLocale, emLocale2, emLocale3]

// PYR-24001
#define MSG_LOBBYETC_EMLOCALE_GET							2008
#define MSG_LOBBYETC_EMLOCALE_GET_REPLY						2009 // 6[s|4444] errCode[ errDescr|locale, emLocale, emLocale2, emLocale3]

// Admin message - get ratholong user status
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBYETC_ADMIN_GET_RATHOLING_INFO				2010 // 4s - reqId, userId
#define MSG_LOBBYETC_ADMIN_GET_RATHOLING_INFO_REPLY			2011 // 46[s|s4<ObligationReport>4<ObligationReport>] - reqId, errCode[errDesc|userId, numOfObligations,<ObligationVector> numOfTags, <TagVector>] ObligationReport=4844444TB

// Admin message - delete ratholong user obligation
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBYETC_ADMIN_DELETE_OBLIGATION				2012 // 4s4<4> - reqId, userId, numOfObligations, <obligationId>
#define MSG_LOBBYETC_ADMIN_DELETE_OBLIGATION_REPLY			2013 // 46[s|s4<46s>] - reqId, errCode[errDesc|userId, numOfObligations, <obligationId, oblErrCode, oblErrDescr>]

//	PYR-27341
//	Admin message for automation testing
// !!! WARNING !!! If you modify this message, please let testautomation team know.
//#define MSG_LOBBYETC_ADMIN_GUEST_ACCOUNT_GET_CHIPS			2014	//	4s - reqId, userId
//#define MSG_LOBBYETC_ADMIN_GUEST_ACCOUNT_GET_CHIPS_REPLY	2015	//	46[s|88] - reqId,errCode[errDesc|totalPMChips,allocatedPMChips]

// GeoComply
#define MSG_LOBBYETC_GEOCOMPLY_LOCATE						2016	//	ssss - license key, reason, user, phone ( server -> client ) !!! all strings are Win1252 encoded !!!
#define MSG_LOBBYETC_GEOCOMPLY_LOCATE_REPLY					2017	//	6[s|s] - errCode [ errDesc | location ] ( client -> server ) !!! reply string is UTF-8 encoded !!!

#define MSG_LOBBYETC_CLI_RECONNECT							2018	//	( server -> client )

// lobbyetc -> GeoComplyGateway
#define MSG_LOBBYETC_LOCAID_REGISTER_PHONE_NUM				2020	// 4ss - reqid, mobilePhoneNum, token
#define MSG_LOBBYETC_LOCAID_REGISTER_PHONE_NUM_REPLY		2021	//	6[s|s] - errCode [ errDesc | response ] 

// GeoComplyGateway -> lobbyetc (notification)
#define MSG_LOBBYETC_NOTIF_LOCAID_REG_STATUS				2022	// 4sss - reqId(0), regStatus, mobilePhoneNum, token

#define MSG_LOBBYETC_CLI_START_MOBILE_VERIFICATION			2024	// ssb - mobilePhoneCountry, mobilePhoneNumber, verifyPhoneOrNot ( server -> client )

#define MSG_LOBBYETC_CLI_REQUEST_USER_LOCAID_CONSENT		2026	// ( server -> client )
#define MSG_LOBBYETC_CLI_REQUEST_USER_LOCAID_CONSENT_REPLY	2027	// 6[s] - errCode [ errDesc ] ( client -> server )

#define MSG_LOBBYETC_CLI_NOTIF_MOBILE_VERIFIED				2028	// ( client -> server )

#define MSG_LOBBYETC_CLI_NOTIF_MOBILE_REQUIRED_STATUS		2030	// 4s - status, message (see MobileRequiredStatus definition below) ( lobbyetc -> client )

// lobbyetc -> GeoComplyGateway
#define MSG_LOBBYETC_GEOCOMPLY_GET_LICENSE					2032	// empty
#define MSG_LOBBYETC_GEOCOMPLY_GET_LICENSE_REPLY			2033	// 6[s|s] - errCode [ errDesc | response ] 

#define MSG_LOBBYETC_CLI_NOTIF_GEOIP_RESULT					2034	// 4s - status, message (see GeoLocationResult definition below) ( lobbyetc -> client )


//////////////////////////////////////////////////////////////////////////
// image gallery
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBYETC_ADMIN_SAVE_IMAGE_CAT					2035	// 4O - reqId, ImageCatItem
#define MSG_LOBBYETC_ADMIN_SAVE_IMAGE_CAT_REPLY				2036	// 46[s] - reqId, errCode[errDescr|catId]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBYETC_ADMIN_SAVE_IMAGE_GALLERY_ENTRY			2037	// 4O - reqId, ImageGalleryItem
#define MSG_LOBBYETC_ADMIN_SAVE_IMAGE_GALLERY_ENTRY_REPLY	2038	// 46[s] - reqId, errCode[errDescr]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBYETC_ADMIN_DELETE_IMAGE_GALLERY_ENTRY		2039	// 444 - reqId, imageId, catId
#define MSG_LOBBYETC_ADMIN_DELETE_IMAGE_GALLERY_ENTRY_REPLY	2040	// 46[s] - reqId, errCode[errDescr]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBYETC_ADMIN_SAVE_IMAGE_APPROVAL				2041	// 4O - reqId, ImageApprovalItem
#define MSG_LOBBYETC_ADMIN_SAVE_IMAGE_APPROVAL_REPLY		2042	// 46[s] - reqId, errCode[errDescr]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBYETC_ADMIN_DELETE_IMAGE_APPROVAL			2043	// 444 - reqId, imageId, userIntId
#define MSG_LOBBYETC_ADMIN_DELETE_IMAGE_APPROVAL_REPLY		2044	// 46[s] - reqId, errCode[errDescr]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBYETC_ADMIN_DELETE_IMAGE_CAT					2045	// 44 - reqId, catId
#define MSG_LOBBYETC_ADMIN_DELETE_IMAGE_CAT_REPLY			2046	// 46[s] - reqId, errCode[errDescr]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBYETC_ADMIN_GALLERY_PUBLICATION				2047	// 44 - reqId, freeze(0)/clear_and_freeze(1)/republish(2)
#define MSG_LOBBYETC_ADMIN_GALLERY_PUBLICATION_REPLY		2048	// 46[s] - reqId, errCode[errDescr]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBYETC_CLI_SET_USER_GALLERY_IMAGE				2049	// 4 - imageId
#define MSG_LOBBYETC_CLI_SET_USER_GALLERY_IMAGE_REPLY		2050	// 6[s] errCode[errDescr]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBYETC_CLI_GET_IMAGE_USER_APPROVALS			2051	// 
#define MSG_LOBBYETC_CLI_GET_IMAGE_USER_APPROVALS_REPLY		2052	// 6[s|4<M(O)>44b] - errCode[errDescr|numItems<M(ImageApprovalItem)>nRemaining,currentImageId,isGallery]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBYETC_CLI_DELETE_IMAGE_APPROVAL				2053	// 4 - imageId
#define MSG_LOBBYETC_CLI_DELETE_IMAGE_APPROVAL_REPLY		2054	// 6[s] - errCode[errDescr]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBYETC_REGULATOR_IS_GALLERY_IMAGE				2055	// 444 - reqId,imageId,brandId
#define MSG_LOBBYETC_REGULATOR_IS_GALLERY_IMAGE_REPLY		2056	// 46[s|bb] - errCode[errDescr|isGalleryImage, isBrandAvailable]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBYETC_DELETE_PRIVATE_IMAGE					2057	// 44b - reqId,imageId
#define MSG_LOBBYETC_DELETE_PRIVATE_IMAGE_REPLY				2058	// 46[s|b] - errCode[errDescr]

#define MSG_LOBBYETC_LOBBY_SET_USER_GALLERY_IMAGE			2059	// 4s444 - reqId,userId,userIntId,imageId,adminIntId
#define MSG_LOBBYETC_LOBBY_SET_USER_GALLERY_IMAGE_REPLY		2060	// 46[s|b] - errCode[errDescr]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBYETC_ADMIN_GET_PUBLICATION_MODE				2061	// 4 - reqId
#define MSG_LOBBYETC_ADMIN_GET_PUBLICATION_MODE_REPLY		2062	// 46[s|4] - reqId,errCode[errDescr|publicationMode - 0 frozen, 1 frozen and empty, 2 normal, 3 not initialized ]

#define MSG_LOBBYETC_ADMIN_SET_USER_GALLERY_IMAGE			2063	// 4s444 - reqId,userId,userIntId,imageId
#define MSG_LOBBYETC_ADMIN_SET_USER_GALLERY_IMAGE_REPLY		2064	// 46[s|b] - errCode[errDescr]

#define MSG_LOBBYETC_REGULATOR_GET_IMG_USER_APPROVALS		2065	// 444484 - reqId, userIntId, localeId, priv, priv2
#define MSG_LOBBYETC_REGULATOR_GET_IMG_USER_APPROVALS_REPLY	2066	// 46[s|4<M(O)>44b] - reqId,errCode[errDescr|numItems<M(ImageApprovalItem)>nRemaining,currentImageId,isGallery]

#define MSG_LOBBYETC_CLI_GET_DEFAULT_GALLERY_IMAGEID		2067	// 4M4s - cliSiteId, cliParams, locale, country code
#define MSG_LOBBYETC_CLI_GET_DEFAULT_GALLERY_IMAGEID_REPLY	2068	// 6[s|4] - errCode[errDescr|imageId]


// admin -> lobbyetc -> CRMDBM
#define MSG_LOBBYETC_ADMIN_ADD_IMAGE_CRITERIA				2069	// 44s - reqId, imageId, criteria
#define MSG_LOBBYETC_ADMIN_ADD_IMAGE_CRITERIA_REPLY			2070	// 46[s] - errCode[errDesc]

#define MSG_LOBBYETC_ADMIN_UPDATE_IMAGE_CRITERIA			2071	// 44s - reqId, imageId, criteria
#define MSG_LOBBYETC_ADMIN_UPDATE_IMAGE_CRITERIA_REPLY		2072	// 46[s] - errCode[errDesc]

// image gallery
//////////////////////////////////////////////////////////////////////////

// lobbyetc -> client
#define MSG_LOBBYETC_CLI_NOTIFY_ICE_NUDGE					2073	// 44s - numChanges,flags,toolTip

// client -> lobbyetc
#define MSG_LOBBYETC_CLI_ICE_GET_UPDATES					2074	//
#define MSG_LOBBYETC_CLI_ICE_GET_UPDATES_REPLY				2075	// 6[s|44s] errCode[errDescr|numChanges,flags,toolTip]

// lobby -> lobbyetc group registration
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBYETC_TOURN_GROUP_REG						2076	// 4s4tb - reqId, groupName, tournId, start, active
#define MSG_LOBBYETC_TOURN_GROUP_REG_REPLY					2077	// 6

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBYETC_TOURN_GROUP_UNREG						2078	// 4s4 - reqId, groupName, tournId
#define MSG_LOBBYETC_TOURN_GROUP_UNREG_REPLY				2079	// 6

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBYETC_TOURN_GROUP_ACTIVE						2080	// 4s4b - reqId, groupName, tournId, active
#define MSG_LOBBYETC_TOURN_GROUP_ACTIVE_REPLY				2081	// 6

//#define MSG_LOBBYETC_TOURN_GROUP_BESTFIT					2082	// 4st - reqId, groupName, offset
//#define MSG_LOBBYETC_TOURN_GROUP_BESTFIT_REPLY				2083	// 6[s|4t] err[descr|tournId offset]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBYETC_TOURN_GROUP_TIME_UPDATE				2084	// 4s4t - reqId, groupName, tournId, start
#define MSG_LOBBYETC_TOURN_GROUP_TIME_UPDATE_REPLY			2085	// 6

// PYR-29167
#define MSG_LOBBYETC_ADX_EVENT								2086	// st4444sss  - userWebId, when, eventType, license, platform, device, deviceId, network, reference
#define MSG_LOBBYETC_ADX_EVENT_REPLY						2087	// 6

#define MSG_LOBBYETC_GET_CASINO_LIMITS				2088 // empty
#define MSG_LOBBYETC_GET_CASINO_LIMITS_REPLY		2089 // 6[s|4<{8s1}>4<{s}>44b444] - errCode [errDescr|numLimits=3<Body{limitAmount64,currency,limitType}> numBalances<Body{currency}> autoApprovalHours waitingIncreaseBlackjackLimitDay hasPrevLimitUK  waitingIncreaseRouletteLimitDay  waitingIncreaseSlotsLimitDay  waitingIncreaseOthersLimitDay]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBYETC_SET_GAME_LIMITS2				2090 // 4<{8s1}> vecSize <Body{limAmount64 limCurrency eTableLimitType}>
#define MSG_LOBBYETC_SET_GAME_LIMITS2_REPLY			2091 // 6[s|4<{16[s44|b444]}>] globalErrCode [globalErrDescr | vecSize<Body{limitType errCode[errDescr, autoApprovalHours, waitingIncreaseLimitDays | inEffect, autoApprovalHours, waitingIncreaseLimitDays, limAmount32]}>]

// admin -> lobbyetc group registration
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_ADMIN_LOBBYETC_TOURN_DEAL_SAVE					2092	// 4(CashierDealData) - reqId, CashierDealData cashierdealdata.h
#define MSG_ADMIN_LOBBYETC_TOURN_DEAL_SAVE_REPLY			2093	// 46[s|4] - reqId,errCode[errDescr|dealId]

// web client (web cashier) -> lobbyetc 
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_WEB_CLIENT_LOBBYETC_GET_DEALS					2094	// 44- reqId, sideId
#define MSG_WEB_CLIENT_LOBBYETC_GET_DEALS_REPLY				2095	// 46[s|4<44sI1tts>] - reqId,errCode[errDescr|dealNum<dealId, price, currency, displayName, dealType, start, end, dealName>]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_WEB_CLIENT_LOBBYETC_REGISTER_USER				2096	// 448- reqId, dealId, cashTransId
#define MSG_WEB_CLIENT_LOBBYETC_REGISTER_USER_REPLY			2097	// 46[s|81] - reqId,errCode[errDescr|tournOrTableId, dealType]

#define MSG_ADMIN_LOBBYETC_RELOAD_DEAL_DATA					2098	// 4 - reqId
#define MSG_ADMIN_LOBBYETC_RELOAD_DEAL_DATA_REPLY			2099	// 46[s] - reqId,errCode[errDescr]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_ADMIN_LOBBYETC_TOURN_DEAL_PROP_SAVE				2100	// 44144s - reqId, dealId, action, prop, locale, val 
#define MSG_ADMIN_LOBBYETC_TOURN_DEAL_PROP_SAVE_REPLY		2101	// 46[s] - reqId,errCode[errDescr]

// PYR-27091 lobby -> lobbyetc ring games registration
#define MSG_LOBBYETC_RING_GAME_MASS_REG						2102	// 44<s44>4 - reqId, num,<cashierDeal, tabTypeId, siteId>, poolBlockResetMin
#define MSG_LOBBYETC_RING_GAME_MASS_REG_REPLY				2103	// 4

#define MSG_LOBBYETC_LOGOUT_USER_INFO   					2104    // PYR-30463 via "frozen"- empty
#define MSG_LOBBYETC_LOGOUT_USER_INFO_REPLY      			2105    // 6[s|M4<4M>] - errCode[errDescr|CommBody, num requests<enum eLogoutActionRequest + specificBody>]

#define MSG_LOBBYETC_GET_SPORT_LIMITS						2106    // empty
#define MSG_LOBBYETC_GET_SPORT_LIMITS_REPLY					2107    // 6[s|4<{18s44b}>] - errCode [errDescr|numLimits=1<Body{limitType,limitAmount64,currency,autoApprovalHours,waitingIncreaseLimitDays,hasPrevLimitUK}>]

// PYR-32120 web client -> lobbyetc
#define MSG_WEB_CLIENT_LOBBYETC_EXCLUDE_PLAYER				2108	//4448b1 - reqId,days_to_exclude,flags,flags2,groupSelfExclusion, version
#define MSG_WEB_CLIENT_LOBBYETC_EXCLUDE_PLAYER_REPLY		2109	//46[s|4T4] - reqId, errCode [errDescr|days excluded, until, days requested]

//	PYR-33506. Main lobby -> lobbyetc
#define MSG_LOBBYETC_SET_NETVERIFY_CFG						2110	//	4b4<s>4<s>4<4> b4<s>4<s>4<4> - reqId,jumioEnabled,n<userFilter>,n<countryFilter>,n<licenseFilter> IDCEnabled,n<userFilter>,n<countryFilter>,n<licenseFilter>
#define MSG_LOBBYETC_SET_NETVERIFY_CFG_REPLY				2111	//	46[s] - reqId,errCode[errDescr]

//	PYR-33416: webapp -> lobbyetc
#define MSG_LOBBYETC_GET_USER_IMAGE							2112	//	4<s> - numUsers<userId>
#define MSG_LOBBYETC_GET_USER_IMAGE_REPLY					2113	//	6[s|4<s64b>] - errCode[errDescr|numUsers<userId,errCode,imageid,isTeamPro>]

#define MSG_ADMIN_LOBBYETC_ICELOBBY_RESET_TASK				2114	// 444486s - reqId, userIntId, seasonId, taskId, acceptedInstanceId, stateId, descr
#define MSG_ADMIN_LOBBYETC_ICELOBBY_RESET_TASK_REPLY		2115	// 46[s] - reqId,errCode[errDescr]

//	PYR-30754 admin -> lobbyetc
#define MSG_LOBBYETC_ADMIN_GEOCOMPLY_REQ					2116	//	4s - reqId,userId
#define MSG_LOBBYETC_ADMIN_GEOCOMPLY_REQ_REPLY				2117	//	46[s] - reqId,errCode[errDescr]

//	PYR-33799 webapp -> lobbyEtc
#define MSG_LOBBYETC_CONVERT_USERINTIDS						2118	//	4<4> - numUsers<userIntId>
#define MSG_LOBBYETC_CONVERT_USERINTIDS_REPLY				2119	//	6[s|4<4s>] - errCode[errDesc|numUsers<userIntId,userId>]

// client -> lobbyetc -> MSG_ICELOBBY_SET_ALL_USER_BOARD_PREF to ice lobby
#define MSG_LOBBYETC_CLI_ICE_SET_ALL_USER_BOARD_PREF		2120	// b - eligible
#define MSG_LOBBYETC_CLI_ICE_SET_ALL_USER_BOARD_PREF_REPLY	2121	// 6[s] errCode[errDescr]

// PYR-34388 client -> lobbyetc
#define MSG_LOBBYETC_GET_SPORT_ACCUMULATED_LIMIT			2122	// empty
#define MSG_LOBBYETC_GET_SPORT_ACCUMULATED_LIMIT_REPLY		2123	// 6[s|4<{18s44b}>] - errCode [errDescr|numLimits=1<Body{limitType,limitAmount64,currency,autoApprovalHours,waitingIncreaseLimitDays,hasPrevLimitUK}>]

//	PYR-34652. RAM client -> LobbyEtc
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBYETC_GET_USER_PROPS							2124	//	4 - reqId
#define MSG_LOBBYETC_GET_USER_PROPS_REPLY					2125	//	46[s|444s844488M(8888)4s] - reqId,errCode[errDescr|privileges,flags,locale,country,privileges2,vipStatus,timezone,userIntId,flags2,privileges3,M(featureMaskOn,featureMaskOff,featureMaskOn2,featureMaskOff2),originalUserIntId,fiscalCode]

//	PYR-34652. Lobby -> LobbyEtc
#define MSG_LOBBYETC_SET_INIDATA							2126	//	4 - reqId,blockedCountryLicenseInfo
#define MSG_LOBBYETC_SET_INIDATA_REPLY						2127	//	46[s] - reqId,errCode[errDesc]

// admin -> lobbyetc
#define MSG_ADMIN_LOBBYETC_SAVE_FPP_BUNDLES					2128	// 4o - reqId, FppBundlesScriptData
#define MSG_ADMIN_LOBBYETC_SAVE_FPP_BUNDLES_REPLY			2129	// 46[s] - reqId,errCode[errDescr]

// admin -> lobbyetc
#define MSG_ADMIN_LOBBYETC_SAVE_FPP_BUNDLES_FROM_FILE		2130	// 4s - reqId, fileName
#define MSG_ADMIN_LOBBYETC_SAVE_FPP_BUNDLES_FROM_FILE_REPLY	2131	// 46[s] - reqId,errCode[errDescr]

//	BlitzLobby/SeatFinder -> lobbyetc
#define MSG_LOBBYETC_GET_STEALTH_IMAGES						2132	//	44 - reqId, imagesNum
#define MSG_LOBBYETC_GET_STEALTH_IMAGES_REPLY				2133	//	44<4> - reqId, imagesNum<imageId>

// lobbyetc -> client
#define MSG_LOBBYETC_CLI_NOTIFY_FPP_BUNDLE_VPP_CHANGE		2134	// o - UserFppBundleVppNotifyData

#define MSG_LOBBYETC_GET_FPP_BUNDLES_FOR_USER				2135	//	
#define MSG_LOBBYETC_GET_FPP_BUNDLES_FOR_USER_REPLY			2136	//	6[s|o] - errCode[errDescr|UserFppBundlesForVipLevel]

// PYR-35077
#define MSG_LOBBYETC_FIND_TOURN_IT							2137 //s userId
#define MSG_LOBBYETC_FIND_TOURN_IT_REPLY					2138 //2[s|4<4M(TournData)>4<4M(TableData 8)>4<s4>] errCode[errDesc, numTourn<tournId, M(TournData)>, numTable<tableId32, M(TableData, tableId64)>, numSportsbook<aamsTiketId, amount>]

// PYR-35875
#define MSG_LOBBYETC_USER_TOURNTICKETS2						2139 // s user 
#define MSG_LOBBYETC_USER_TOURNTICKETS2_REPLY				2140 // 6[s|4<4sb4tt>] err_code[errDescr| numTickets,<M(ticketid, description, multitourn, price, issued, exp)>]

// PYR-52382: Deprecated
// PYR-35875
//#define MSG_LOBBYETC_DUEL_CLI_TOURN_STATS					2141 //1s4 requestType(see dbm.h for details), userName, tournIdOrNumTourns
//#define MSG_LOBBYETC_DUEL_CLI_TOURN_STATS_REPLY			2142 //6[s] errCode, [errDescr|email]

// PYR-52382: Deprecated
// PYR-35875
//#define MSG_LOBBYETC_DUEL_CLI_HISTORY_REQUEST				2143 //s14[<4>]	userId, requestType, requestContents[<handId>] 
//#define MSG_LOBBYETC_DUEL_CLI_HISTORY_REQUEST_REPLY		2144 //6s	error_code[errDescr|userEmail]
//#define MSG_LOBBYETC_DUEL_CLI_HISTORY_REQUEST_ACCEPTED	2145

// PYR-35875
// Lobby -> LobbyEtc
#define MSG_LOBBYETC_CLIENT_CURR_CONV_OPT					2146 // s8bb - userId, appLogindId, doConvertBuyIn, doConvertWinnings
#define MSG_LOBBYETC_CLIENT_CURR_CONV_OPT_REPLY				2147 // 46[s|4848] - reqId, errCode[errDescr|flags, flags2, privs, privs2]

// PYR-35875
// this message is only used by RAM now (see PYR-34497)
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBYETC_WEBCLIENT_CURR_CONV_OPT				2148 // sbb - userId, doConvertBuyIn, doConvertWinnings
#define MSG_LOBBYETC_WEBCLIENT_CURR_CONV_OPT_REPLY			2149 // 6[s|4848] - errCode[errStr|flags, flags2, privs, privs2]

// PYR-89275
// LobbyEtc --> SeatFinder
#define MSG_LOBBYETC_SFN_REMOVE_RM_WAITING_USER				2250 // 4s4 - reqId, userId, userIntId
#define MSG_LOBBYETC_SFN_REMOVE_RM_WAITING_USER_REPLY		2251 // 4 - reqId

//	PYR-36169: webapp -> lobbyetc
#define MSG_LOBBYETC_GET_AUX_HAND_HISTORY					2152	//	4444b1488 - reqId,hostId,locale,timezone,sentByEmail,requestType,numOfHands,numOfSeconds,handId
#define MSG_LOBBYETC_GET_AUX_HAND_HISTORY_REPLY				2153	//	46[s|s] - reqId,errCode[errDescr|handHistory]

// PYR-52382: Deprecated
// PYR-36593
//#define MSG_LOBBYETC_DUEL_CLI_DUEL_HISTORY_REQUEST		2154 // 14 - requestType,numOfLastDuels
//#define MSG_LOBBYETC_DUEL_CLI_DUEL_HISTORY_REQUEST_REPLY	2155 // 6s	error_code[errDescr|userEmail]

#define MSG_LOBBYETC_GET_ALL_APPROVED_GAME_LIMITS			2156 // empty
#define MSG_LOBBYETC_GET_ALL_APPROVED_GAME_LIMITS_REPLY		2157 // 6[s|4<{18s44b}>4<{s}>] - errCode [errDescr|numLimits<Body{limitType,limitAmount64,currency,autoApprovalHours,waitingIncreaseLimitDays,hasPrevLimitUK}>,numBalances<Body{currency}>]

// PYR-35275
#define MSG_LOBBYETC_CLIENT_REPORT							2158 // 4M(dds4b) - reportType, M(retportstartDate, endDate, password, eReportFormat, notifyClient)
#define MSG_LOBBYETC_CLIENT_REPORT_REPLY					2159 // 6[s] errCode, [errDescr]

// PYR-35623, PYR-39764, PYR-85575
#define MSG_LOBBYETC_SIGNAL_EVENT_INSTALLATION				2160  // ss444sss[s][M][b] - installIdDeprecated,promo,siteId,platformId,clientVersion,tiVer,tinfoText,refDomain,mobileAppId,loginExtraData,lat
#define MSG_LOBBYETC_SIGNAL_EVENT_INSTALLATION_REPLY		2161  // 6[s] - errCode[errDescr]

// PYR-37887
// Lobby -> LobbyEtc
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBYETC_CLIENT_NOTIFICATION					2162 // 4s4M() - reqId, userId, lobbyToUserNotificationMsgId, M()
#define MSG_LOBBYETC_CLIENT_NOTIFICATION_REPLY				2163 // 46[s] - reqId, errCode[errDescr]

// PYR-37887
// WebApp -> LobbyEtc
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBYETC_GET_NOTIFICATIONS						2164 // 4t[4] - reqId, afterThisTime[,protocolVersion]
#define MSG_LOBBYETC_GET_NOTIFICATIONS_REPLY				2165 // 46[s|t4<[4]4tM()>] - reqId, errCode[errDescr|replyTimeStamp,vectorSize<[uniqueId],userNotificationMsgId, notificationTimeStamp, M(notificationBody)>]

// PYR-38415
#define MSG_LOBBYETC_TO_CLIENT_INFO_NOTIFICATION			2166	// s-userId,4-imageId,4-priv,8-priv2,8-priv3,4-flags,8-flags2,8-flags3,8-playMoney, 4-numOfAccts <MsgBody{s-currency, 8-chips(might be negative), 8-tChips}>, 8-fppInCents, 8-totalBalanceInPreferredCurrency, 8-totalBalanceInUSD, 8-totalTChipsInSelectedCurrency, s-tChipsCurrency

// PYR-36336
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBYETC_GET_LOGIN_CONFIRMATION 				2167
#define MSG_LOBBYETC_GET_LOGIN_CONFIRMATION_REPLY			2168 

// PYR-38484
#define MSG_LOBBYETC_GET_LOGIN_HISTORY 						2169	//
#define MSG_LOBBYETC_GET_LOGIN_HISTORY_REPLY				2170 	// 6[s|4<tsss>] - errCode[errDescr| numOfItems<logingtime, isp, country, ipAddress>]

// PYR-33958
#define MSG_ADMIN_LOBBYETC_ICELOBBY_COMPLETE_TASK			2171	// 4448bs - reqId, userIntId, taskId, acceptedInstanceId, isCompleteSuccess, descr
#define MSG_ADMIN_LOBBYETC_ICELOBBY_COMPLETE_TASK_REPLY		2172	// 46[s] - reqId,errCode[errDescr]

// PYR-37900
// WebApp -> LobbyEtc
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBYETC_GET_USER_NOTIFICATIONS					2173 // 44st[4] - reqId, userIntId, sUserTokenString,afterThisTime[,protocolVersion]
#define MSG_LOBBYETC_GET_USER_NOTIFICATIONS_REPLY			2174 // 46[s|t4<[4]4tM()>] - reqId, errCode[errDescr|replyTimeStamp,vectorSize<[uniqueId],userNotificationMsgId, notificationTimeStamp, M(notificationBody)>]

// PYR-35364
// client -> LobbyEtc
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBYETC_SET_REALITY_CHECK_FREQ					2175 // 4 - minutes
#define MSG_LOBBYETC_SET_REALITY_CHECK_FREQ_REPLY			2176 // 6[s] - errCode[errDescr]

// PYR-35364
// client -> LobbyEtc
#define MSG_LOBBYETC_GET_REALITY_CHECK_FREQ					2177 //
#define MSG_LOBBYETC_GET_REALITY_CHECK_FREQ_REPLY			2178 // 6[s|b[44st|4]] - errCode[errDescr|found[propType,propInt,propStr,when|propType]]

// PYR-41099 security server -> lobbyetc
#define MSG_LOBBYETC_GEOCOMPLY_REQ							2179	//	4s - reqId,userId
#define MSG_LOBBYETC_GEOCOMPLY_REQ_REPLY					2180	//	46[s] - reqId,errCode[errDescr]

// PYR-40329
#define MSG_LOBBYETC_SAVE_TABLES_SCRIPT						2181 // 4ss - reqId,userId,tableScript
#define MSG_LOBBYETC_SAVE_TABLES_SCRIPT_REPLY				2182 // 4 - reqId

// PYR-40428 
// MGLobby,Lobby -> LobbyEtc
#define MSG_LOBBYETC_CLIENT_NOTIFICATION_MESSAGE            2183 // 4s44<s>444 - reqId,userId,messageType (see eClientNotificationMessageType definition below),numMessageParameters<messageParameters>,i18nMsgId,appTypeIdMask,eClientNotificationCode
#define MSG_LOBBYETC_CLIENT_NOTIFICATION_MESSAGE_REPLY		2184 // 46[s] - reqId,errCode[errDescr]

// PYR-41760 
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBYETC_FPP_HISTORY_AUDIT						2185 // sdd4sbb - userId, startDate, endDate, eReportFormat, password, notifyClient, stepsOnly
#define MSG_LOBBYETC_FPP_HISTORY_AUDIT_REPLY				2186 // 6[s] - err_code, [errDescr]

// PYR-42970
#define MSG_LOBBYETC_FR_GET_LOGIN_CONFIRMATION 				2187 // b - doAccVal
#define MSG_LOBBYETC_FR_GET_LOGIN_CONFIRMATION_REPLY		2188 // 6[s|M()M()]- errCode[errDescr|M(validation status)M(user props)]

// PYR-42791 - notification to client
#define MSG_LOBBYETC_U_POOL_BLOCK_TIME_PENALTY_UPDATE		2190 // 84 - blockedUntil (time_t), nextInfractionNum

#define MSG_LOBBYETC_ADMIN_GET_USER_POOL_BLOCK_STATUS 		2191 // 44 - reqId, userIntId
#define MSG_LOBBYETC_ADMIN_GET_USER_POOL_BLOCK_STATUS_REPLY	2192 // 46t44t8244444bs - reqId, errCode, [errDescr|blockedUntil, activeInfractionsNum, totalTimePenaltyMin, lastInfractionTime, lastInfractionTableId, lastTableMinHands,
																 //												lastPlayedHandsNum, lastTableSize, lastTableHandType, lastTablePotStruct, lastTableAnte, lastTableHiLo, lastTableCurrency]

#define MSG_LOBBYETC_ADMIN_RESET_USER_POOL_BLOCK_STATUS 		2193 // 44 - reqId, userIntId
#define MSG_LOBBYETC_ADMIN_RESET_USER_POOL_BLOCK_STATUS_REPLY	2194 // 46 - reqId, errCode, [errDescr]

// PYR-40839
// Renamed from MSG_LOBBYETC_GET_EMAIL_FLAGS
#define MSG_LOBBYETC_BOUNCED_EMAIL							2195 // 1[b] doReset [sendValidationCode]
#define MSG_LOBBYETC_BOUNCED_EMAIL_REPLY					2196 // 6[s] - errCode, [errDescr]

// PYR-42114
#define MSG_LOBBYETC_CROSSLINK_BUTTONS_CREATE				2197 // 4<M(4<M(s)>4<M(s)>M(sss4<4s>4<4s>))> - buttonCount<M(userCriteriaCount<M(userCriteria)>,gameCriteriaCount<M(gameCriteria)>,M(buttonName,c2lLink,iconName,labelCount<locale,label>,tooltipCount<locale,tooltip>,type))>
#define MSG_LOBBYETC_CROSSLINK_BUTTONS_CREATE_REPLY			2198 // 6[s] - errCode, [errDescr]

// PYR-43143
#define MSG_LOBBYETC_RAM_RO_SESSION_DURATION_NOTIFICATION	2199 // 4 sessionDurationSeconds

// PYR-43054
// LobbyEtc --> SeatFinder
#define MSG_LOBBYETC_SFN_POOL_BLOCK_UPDATE					2200 // 44<4t> - reqId, usersNum<userIntId, blockedUntil>
#define MSG_LOBBYETC_SFN_POOL_BLOCK_UPDATE_REPLY			2201 // 46[s] - reqId, errCode, [errDescr]

#define MSG_LOBBYETC_SFN_POOL_BLOCK_RESET					2202 // 4 - reqId
#define MSG_LOBBYETC_SFN_POOL_BLOCK_RESET_REPLY				2203 // 46[s] - reqId, errCode, [errDescr]

// PYR-43054
// SeatFinder --> LobbyEtc
#define MSG_SFN_LOBBYETC_POOL_BLOCK_INIT_DATA				2204 // 4 - reqId
#define MSG_SFN_LOBBYETC_POOL_BLOCK_INIT_DATA_REPLY			2205 // 444<4t> - reqId, errCode, [usersNum<userIntId, blockedUntil>]

// PYR-45757
// Lobby --> LobbyEtc
#define MSG_LOBBYETC_LOBBY_HISTORY_REQUEST					2206 // 4s4844s4ffM(s14<8>) - reqId,userId,flags,flags2,locale,clientSiteId,email,brandId,throttleRequests,sendAcceptNotification,M(userId,requestType,requestContents,[<handId>]
#define MSG_LOBBYETC_LOBBY_HISTORY_REQUEST_REPLY			2207 // 46s - reqId,errCode,[errDescr|email]

/*
// PYR-45757
requestType					requestContents
------------------------------------------------------
HAND_HISTORY_LAST_HANDS		number of last hands
HAND_HISTORY_LAST_SECONDS	number of last seconds 
HAND_HISTORY_HAND_IDS		number of hands requested, followed by list of hands84>
*/
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBYETC_CLIENT_HISTORY_REQUEST				2208	//s1[8|4<8>]4	userId, requestType, [requestContents|numHands<handId>], locale
#define MSG_LOBBYETC_CLIENT_HISTORY_REQUEST_REPLY		2209	//6s	error_code[errDescr|userEmail]

// PYR-46617
#define MSG_LOBBYETC_ADMIN_REREAD_INIDATA				2210	//4 - reqId
#define MSG_LOBBYETC_ADMIN_REREAD_INIDATA_REPLY			2211	// 46[s] reqId, errCode[errDescr]

// PYR-47874
#define MSG_LOBBYETC_W2G_AUDIT							2212	// 4ss4 - reqId, userId, password, year
#define MSG_LOBBYETC_W2G_AUDIT_REPLY					2213	//reqId, 6[s] errCode, [errDescr]

// PYR-33126
#define MSG_LOBBYETC_CLI_REPORT_WEBCLIENT_INFO			2214	// ssst44 - browser, osdetail, playlang, localtime, scrnsizex, scrnsizey
#define MSG_LOBBYETC_CLI_REPORT_WEBCLIENT_INFO_REPLY	2215	// 6[s] errCode, [errDescr]

// PYR-47770
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBYETC_ADMIN_GET_ALL_APPROVED_GAME_LIMITS			2216 // 44s - reqId, userIntId, userId
#define MSG_LOBBYETC_ADMIN_GET_ALL_APPROVED_GAME_LIMITS_REPLY	2217 // 46[s|4<{18s44bt4}>4<{s}>] - reqId,errCode [errDescr|numLimits<Body{limitType,limitAmount64,currency,autoApprovalHours,waitingIncreaseLimitDays,hasPrevLimitUK,scheduled,remaining}>,numBalances<Body{currency}>]

// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBYETC_ADMIN_SET_GAME_LIMITS2				2218 // 4s4<{8s1}> reqId,userId,vecSize <Body{limAmount64 limCurrency eTableLimitType}>
#define MSG_LOBBYETC_ADMIN_SET_GAME_LIMITS2_REPLY		2219 // 46[s|4<{16[s44|b444]}>] reqId,globalErrCode [globalErrDescr | vecSize<Body{limitType errCode[errDescr, autoApprovalHours, waitingIncreaseLimitDays | inEffect, autoApprovalHours, waitingIncreaseLimitDays, limAmount32]}>]

// PYR-47770 client -> lobbyetc
#define MSG_LOBBYETC_GET_LOGIN_LIMIT_INFO				2220 // empty
#define MSG_LOBBYETC_GET_LOGIN_LIMIT_INFO_REPLY			2221 // 6[s|4444t] - errCode [errDescr|loginTotal, loginTotalLimit, loginTimeTotalSec, loginTimeLimitMin, userSessionStartTime]

// PYR-47770
// obsolete
// #define MSG_LOBBYETC_SESSION_DURATION_NOTIFICATION	2222 // 4 sessionDurationMilliSeconds

// PYR-47770 admin -> lobbyetc
#define MSG_LOBBYETC_ADMIN_GET_LOGIN_LIMIT_INFO				2224 // 4s - reqId, userId
#define MSG_LOBBYETC_ADMIN_GET_LOGIN_LIMIT_INFO_REPLY		2225 // 46[s|4444t] - reqId, errCode [errDescr|loginTotal, loginTotalLimit, loginTimeTotalSec, loginTimeLimitMin, userSessionStartTime]

// PYR-45659
// obsolete
// #define MSG_LOBBYETC_CVL_CONFIG							2226		//4 - reqId=0,CvlConfig

// PYR-45639 
// defined in protocols/cvl/: CvlClientUserBarNotification, CvlClientUserBar
// lobbyetc -> client
#define MSG_LOBBYETC_CLI_NOTIFY_CVL_STATE				2227 // o - CvlClientUserBarNotification
// client <-> lobbyetc
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBYETC_GET_CVL_STATE_FOR_USER				2228 //    
#define MSG_LOBBYETC_GET_CVL_STATE_FOR_USER_REPLY		2229 //    6[s|o] - errCode[errDescr|CvlClientUserBar]

// PYR-45550
// defined in srvshared/cvl/
// mglobby <-> lobbyetc
#define MSG_LOBBYETC_MG_NOMINATE_COMPLETED_BAR			2230 // 44 - reqId,userIntId
#define MSG_LOBBYETC_MG_NOMINATE_COMPLETED_BAR_REPLY	2231 // 42[s|81] - reqId,errCode[errString|barId,barTierId]
// mglobby <-> lobbyetc
#define MSG_LOBBYETC_MG_GET_BAR_AND_PLAYER_ATTRS		2232 // 448 - reqId,userIntId,barId
#define MSG_LOBBYETC_MG_GET_BAR_AND_PLAYER_ATTRS_REPLY	2233 // 42[s|14<M(44)>] - reqId,errCode[errString|barTierId,numBDUserAttrs<M(attrType,attrValue)>]
// mglobby <-> lobbyetc
#define MSG_LOBBYETC_MG_START_BAR_OPENING				2234 // 4884 - reqId,barId,minigamePlayId,userIntId
#define MSG_LOBBYETC_MG_START_BAR_OPENING_REPLY			2235 // 42[s] - reqId,errCode[errString]
// mglobby <-> lobbyetc
#define MSG_LOBBYETC_MG_FINISH_BAR_OPENING				2236 // 4884 - reqId,barId,minigamePlayId,userIntId
#define MSG_LOBBYETC_MG_FINISH_BAR_OPENING_REPLY		2237 // 42[s] - reqId,errCode[errString]
// mglobby <-> lobbyetc
#define MSG_LOBBYETC_MG_PAY_REWARD_POINTS				2238 // 4484<4> - reqId,userIntId,barId,numRewardPointBatches<rewardPointBatches>
#define MSG_LOBBYETC_MG_PAY_REWARD_POINTS_REPLY			2239 // 442[s|4<M(4<M(44444444)>)>] - reqId,errCode[errString|numClientBarStates<M(numProperties<M(tierId,flags,currentBoostRemaining,nextBoostTime,barProgress,barTarget,doneBarCount,tierUpEnd)>)>]
// 6s
#define MSG_LOBBYETC_GENERAL_CLIENT_ERROR_NOTIF			2240
// 46s
#define MSG_LOBBYETC_GENERAL_CLIENT_ERROR_WITH_CLIENT_ID	2241

// PYR-49481 admin <-> lobbyetc
// tierData = 4<M(4)>, scoreCount<M(BoostLimt)>
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBYETC_ADMIN_SAVE_CVL_BOOST_CONFIG		2242 // 44444<M(tierData) - reqId, licenseId,countDownTimer,testingScore,tierCount<TierData>
#define MSG_LOBBYETC_ADMIN_SAVE_CVL_BOOST_CONFIG_REPLY	2243 // 46[s] - reqId, errCode [errDescr]

// client <-> lobbyetc
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBYETC_CVL_OPTED_IN						2244 // b - opted-in/refused   
#define MSG_LOBBYETC_CVL_OPTED_IN_REPLY					2245 // 6[s] - errCode[errDescr]

// PYR-49459 admin -> lobbyetc
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBYETC_ADMIN_SET_CVL_BAR					2246 // 444 - reqId, userIntId,tierId
#define MSG_LOBBYETC_ADMIN_SET_CVL_BAR_REPLY			2247 // 46[s] - reqId, errCode [errDescr]

// obsolete
// replaced by MSG_LOBBYETC_ADMIN_GET_CVL_COUNTERS/MSG_LOBBYETC_ADMIN_GET_CVL_COUNTERS_REPLY
// #define MSG_LOBBYETC_ADMIN_ASSIGN_VIP_MANAGER		2248 // 444ss - reqId, userIntId, oldVipManagerId, newVipManagerId
// #define MSG_LOBBYETC_ADMIN_ASSIGN_VIP_MANAGER_REPLY	2249 // 46[s] - reqId, errCode [errDescr]
// PYR-50566 admin -> lobbyetc
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBYETC_ADMIN_GET_RP_COUNTERS				2248 // 44 - reqId, userIntId
#define MSG_LOBBYETC_ADMIN_GET_RP_COUNTERS_REPLY		2249 // 46[s|888] - reqId, errCode[errDescr|ltPokerRpsInCents,ltCasinoRpsInCents,ltSportRpsInCents]

//	PYR-50915 Lobby -> LobbyEtc
#define MSG_LOBBYETC_ADD_PERSONAL_MSG					2250	//	4sssb - reqId, adminUserId, userId, message, addAdminChatRecord
#define MSG_LOBBYETC_ADD_PERSONAL_MSG_REPLY				2251	//	46[s] - reqId, errCode [errDescr]

// PYR-50736
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBYETC_CVL_GET_USER_INFO					2252 // 44 - reqId, userIntId
#define MSG_LOBBYETC_CVL_GET_USER_INFO_REPLY			2253 // 46[s|4] - reqId, errCode [errDescr|doneBarCount]

// PYR-51065 admin <-> lobbyetc
// tierData = 4<M(4)>, scoreCount<M(BarTarget)>
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBYETC_ADMIN_SAVE_CVL_BARS_CONFIG			2254 // 44444<M(tierData) - reqId,licenseId,threshold,testingScore,tierCount<TierData>
#define MSG_LOBBYETC_ADMIN_SAVE_CVL_BARS_CONFIG_REPLY	2255 // 46[s] - reqId, errCode [errDescr]

//	PYR-51006 Client -> LobbyEtc (unauthenticated)
#define MSG_LOBBYETC_CLI_GET_IMAGE_GALLERY				2256	//	444 - siteId, brand, locale
#define MSG_LOBBYETC_CLI_GET_IMAGE_GALLERY_REPLY		2257	//	6[s|4<M(4s)>4<M(s4<4>4<M(s4<4>)>)>] - errCode [errDescr|imageCount<M(imageId,imageDescr)>,catCount<M(catName,numImages<imageId>,numSubCats<M(subCatName,numImages<imageId>)>)>]

// PYR-51576
// automation->lobbyetc
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBYETC_BDUA_UPDATE_NOTIFICATION			2258 // 4- reqId
#define MSG_LOBBYETC_BDUA_UPDATE_NOTIFICATION_REPLY		2259 // 46[s] - reqId, errCode [errDescr]

// PYR-51534 admin -> lobbyetc
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBYETC_ADMIN_EXCLUDE_CVL_USER				2262 // 44b - reqId, userIntId,exclude
#define MSG_LOBBYETC_ADMIN_EXCLUDE_CVL_USER_REPLY		2263 // 46[s] - reqId, errCode [errDescr]

// PYR-52641 - security server -> lobbyetc
#define MSG_LOBBYETC_PRESENT_CRM_OFFER					2264 // 444M(ss) - reqId, userIntId, CRMOfferType, M(offercode, starCRMZone)
#define MSG_LOBBYETC_PRESENT_CRM_OFFER_REPLY			2265 // 46[s] - reqId, errCode [errDescr]

// PYR-52641 - notification to client
#define MSG_LOBBYETC_U_PRESENT_CRM_OFFER				2266 // M(ss) - M(starcode, starCRMZone)

// PYR-53553
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_LOBBYETC_ADMIN_REMOVE_CVL_USER_CACHE		2267 // 44<4> - reqId, num<userIntId>
#define MSG_LOBBYETC_ADMIN_REMOVE_CVL_USER_CACHE_REPLY	2268 // 46[s] - reqId, errCode [errDescr]

// PYR-51109
#define MSG_LOBBYETC_CLI_DISPLAY_ADS					2269 //
#define MSG_LOBBYETC_CLI_DISPLAY_ADS_REPLY				2270 // 6[s|b]

// PYR-51947
#define MSG_LOBBYETC_GET_BDUAS_FOR_USER					2271 //    
#define MSG_LOBBYETC_GET_BDUAS_FOR_USER_REPLY			2272 //    6[s|4<M(4,4)>] - errCode[errDescr|num<M(type,value)>]

//	PYR-52886. LobbyEtc -> Client.
//	If the client is a Poker App, the client should log itself out. If the client is a Non-Poker App, the client should logout of all Poker tables.
#define MSG_LOBBYETC_CLI_NOTIFY_LOGOUT_APC				2273	//	empty

// PYR-55173
#define MSG_LOBBYETC_SFN_FRESH_TABLE_CAP_UPDATE			2274 // 44t - reqId,userIntId,freshTableCapAppliedUntil
#define MSG_LOBBYETC_SFN_FRESH_TABLE_CAP_UPDATE_REPLY	2275 // 46 - reqId,errCode(always 0)

#define MSG_LOBBYETC_SFN_FRESH_TABLE_CAP_RESET			2276 // 4 - reqId
#define MSG_LOBBYETC_SFN_FRESH_TABLE_CAP_RESET_REPLY	2277 // 46 - reqId,errCode(always 0)

// PYR-52022
// LobbyEtc -> Client
#define MSG_LOBBYETC_U_UPDATE_FLAGS_PRIV				2278 // 488488 - flags, flags2, flags3, priv, priv2, priv3

// PYR-53961
#define MSG_LOBBYETC_ADMIN_REMOVE_CVL_BARS_CONFIG		2279 // 444 - reqId,licenseId,testingScore
#define MSG_LOBBYETC_ADMIN_REMOVE_CVL_BARS_CONFIG_REPLY	2280 // 46[s] - reqId, errCode [errDescr]

// PYR-53961
#define MSG_LOBBYETC_ADMIN_REMOVE_CVL_BOOST_CONFIG			2281 // 444 - reqId,licenseId,testingScore
#define MSG_LOBBYETC_ADMIN_REMOVE_CVL_BOOST_CONFIG_REPLY	2282 // 46[s] - reqId, errCode [errDescr]

#define MSG_LOBBYETC_CLI_SET_GAME_AVATAR					2283 // o - atf protocols\atfgenerated\lobbyetc_atf.txt
#define MSG_LOBBYETC_CLI_SET_GAME_AVATAR_REPLY				2284 // o - atf protocols\atfgenerated\lobbyetc_atf.txt

// PYR-55718
#define MSG_LOBBYETC_ADMIN_GET_CVL_STATE_FOR_USER			2285 // 44 reqId, userIntId
#define MSG_LOBBYETC_ADMIN_GET_CVL_STATE_FOR_USER_REPLY		2286 // 46[s|CvlClientUserBar] - reqId, errCode [errDescr| CvlClientUserBar]

// PYR-57931
#define MSG_LOBBYETC_LOBBY_SET_CLI_LOCALE					2287 // 4s44 reqId, userId, locale, emLocale
#define MSG_LOBBYETC_LOBBY_SET_CLI_LOCALE_REPLY				2288 // 46[s] reqId, errCode[errDescr]

// PYR-58784 admin -> lobbyetc
#define MSG_LOBBYETC_ADMIN_SAVE_MPCT						2289 // 4s		- reqId, MpcTypeData NOTE MpcTypeData.id == 0 creates new type, MpcTypeData.id != 0 updates existing type
#define MSG_LOBBYETC_ADMIN_SAVE_MPCT_REPLY					2290 // 46[s|4] - reqId, errCode [errDescr|MpcTypeData]

// PYR-58784 ice -> lobbyetc
#define MSG_LOBBYETC_GET_MPCT_LIST							2291 // 44<s>	- reqId, mpcTypeNameCount,<mpcTypeName> NOTE: mpcTypeNameCount==0 brings all active Mpc types  
#define MSG_LOBBYETC_GET_MPCT_LIST_REPLY					2292 // 46[s|4<s>4<MpcTypeData>] - reqId, errCode [errDescr|notFoundMpcTypeNameCount,<notFoundMpcTypeName>, foundMpcTypeNameCount<MpcTypeData>]

// PYR-58784 lobbyetc -> client
// defined in protocols/cvl/clientprotocol.txt: ClientChestData
#define MSG_LOBBYETC_CLI_NOTIFY_MPC_ISSUED					2293 // b4o - isUserOptedIn,chestNum,ClientChestData

// PYR-58784 admin/ice/automation -> lobbyetc
#define MSG_LOBBYETC_ISSUE_MPC_TO_USER						2294 // 444	- reqId, userIntId, typeId
#define MSG_LOBBYETC_ISSUE_MPC_TO_USER_REPLY				2295 // 46[s] - reqId, errCode[errString]

// PYR-58784 admin/ice/automation -> lobbyetc
#define MSG_LOBBYETC_OPEN_MPC_FOR_USER						2296 // 448 - reqId, userIntId,chestId
#define MSG_LOBBYETC_OPEN_MPC_FOR_USER_REPLY				2297 // 46[s] - reqId, errCode[errString]

// PYR-55410
// defined in srvshared/cvl/ Replaces MSG_LOBBYETC_MG_NOMINATE_COMPLETED_BAR / REPLY
// mglobby <-> lobbyetc
#define MSG_LOBBYETC_MG_NOMINATE_COMPLETED_BARINFO			2298 // 44 - reqId,userIntId
#define MSG_LOBBYETC_MG_NOMINATE_COMPLETED_BARINFO_REPLY	2299 // 4[s|4<M(811ss)>] - errCode[errString|numNominatedBars<M(barId,barTierId,barType,miniGameName,logoName)>]

// PYR-55410
// defined in srvshared/cvl/ Replaces MSG_LOBBYETC_MG_GET_BAR_AND_PLAYER_ATTRS / REPLY
// mglobby <-> lobbyetc
#define MSG_LOBBYETC_MG_GET_BARINFO_AND_PLAYER_ATTRS		2300 // 448 - reqId,userIntId,barId
#define MSG_LOBBYETC_MG_GET_BARINFO_AND_PLAYER_ATTRS_REPLY	2301 // 42[s|M(811ss)4<M(44)>] - errCode[errString|M(barId,barTierId,barType,miniGameName,logoName),numBDUserAttrs<M(attrType,attrValue)>]

// PYR-58235
#define MSG_LOBBYETC_GET_USER_SPORT_BOOK_INFO				2302	//	
#define MSG_LOBBYETC_GET_USER_SPORT_BOOK_INFO_REPLY			2303	//	6[s|4] freeBetCount

// PYR-58577
// automation->lobbyetc
#define MSG_LOBBYETC_EXPIRE_CHESTS_FOR_USERS				2304	// 44tb4<4> reqId, adminIntId, srvNow, isMPC, numOfUsers<userIntId>
#define MSG_LOBBYETC_EXPIRE_CHESTS_FOR_USERS_REPLY			2305	// 44<42s> reqId, numOfUsers<userIntId,errCode,errString>

// PYR-55099
#define MSG_LOBBYETC_CVL_GET_USER_STATUS					2306 // 44 - reqId, userIntId
#define MSG_LOBBYETC_CVL_GET_USER_STATUS_REPLY				2307 // 46[s|44] - reqId, errCode [errDescr|barTier,playScore]

// PYR-58657
#define MSG_LOBBYETC_PLAYING_HISTORY_AUDIT2					2308 // o - atf protocols\atfgenerated\lobbyetc_atf.txt
#define MSG_LOBBYETC_PLAYING_HISTORY_AUDIT2_REPLY			2309 // o - atf protocols\atfgenerated\lobbyetc_atf.txt

// PYR-58657
#define MSG_LOBBYETC_U_CLI_NOTIFY_PLAY_HISTORY_REPORT		2310 // 4ddsb4b4 - reqType, [startDate, endDate, password, includeFpp, reportFormat, isHtmlForClient, reportFlags, url | (hand history MD to be defined)]

// PYR-58657, PYR-53668
#define MSG_LOBBYETC_GET_LAST_PLAYING_AUDIT_REPORT			2311 // o - atf protocols\atfgenerated\lobbyetc_atf.txt
#define MSG_LOBBYETC_GET_LAST_PLAYING_AUDIT_REPORT_REPLY	2312 // o - atf protocols\atfgenerated\lobbyetc_atf.txt

// PYR-58657
#define MSG_LOBBYETC_PLAYING_AUDIT_CLEAR_NUDGE				2313 // o - atf protocols\atfgenerated\lobbyetc_atf.txt
#define MSG_LOBBYETC_PLAYING_AUDIT_CLEAR_NUDGE_REPLY		2314 // o - atf protocols\atfgenerated\lobbyetc_atf.txt

// PYR-53668
#define MSG_LOBBYETC_CLIENT_HAND_HISTORY_REQUEST			2315 // o - atf protocols\atfgenerated\lobbyetc_atf.txt
#define MSG_LOBBYETC_CLIENT_HAND_HISTORY_REQUEST_REPLY		2316 // o - atf protocols\atfgenerated\lobbyetc_atf.txt

// PYR-53668
#define MSG_LOBBYETC_GET_LAST_HAND_HISTORY_REPORT			2317 // o - atf protocols\atfgenerated\lobbyetc_atf.txt
#define MSG_LOBBYETC_GET_LAST_HAND_HISTORY_REPORT_REPLY		2318 // o - atf protocols\atfgenerated\lobbyetc_atf.txt

// PYR-53668
#define MSG_LOBBYETC_HAND_HISTORY_CLEAR_NUDGE				2319 // o - atf protocols\atfgenerated\lobbyetc_atf.txt
#define MSG_LOBBYETC_HAND_HISTORY_CLEAR_NUDGE_REPLY			2320 // o - atf protocols\atfgenerated\lobbyetc_atf.txt

// PYR-62106
#define MSG_LOBBYETC_GET_USER_BARS							2321 //    
#define MSG_LOBBYETC_GET_USER_BARS_REPLY					2322 //    6[s|4<o>] - errCode[errDescr|num<ClientMpChestData)]
// PYR-62106
#define MSG_LOBBYETC_GET_USER_BARS_NOTIFICATION				2323 //    44<o> - reqId,num<ClientMpChestData)

// PYR-64363
#define MSG_LOBBYETC_PREDICTOR_GET_TICKETS					2324 // o - atf protocols\atfgenerated\lobbyetc_atf.txt
#define MSG_LOBBYETC_PREDICTOR_GET_TICKETS_REPLY			2325 //     same as MSG_LOBBYETC_U_CLI_NOTIFY_PREDICTOR_TICKETS below

// PYR-64363
#define MSG_LOBBYETC_U_CLI_NOTIFY_PREDICTOR_TICKETS			2326 // o - atf protocols\atfgenerated\lobbyetc_atf.txt

// PYR-67196 cashier or regulator ->lobbyEtc: request to forward notification to user
#define MSG_LOBBYETC_FORWARD_NOTIFICATION_TO_USER			2327 // 4s44M  reqId userId lobbyToUserMsgId ForwardNotificationOptions msgBody
#define MSG_LOBBYETC_FORWARD_NOTIFICATION_TO_USER_REPLY		2328 // 46[s] reqId errCode[errDescr]

// PYR-61146
#define MSG_LOBBYETC_GET_LAST_AUX_HAND_HISTORY_REPORT		2329 // o - atf protocols\atfgenerated\lobbyetc_atf.txt
#define MSG_LOBBYETC_GET_LAST_AUX_HAND_HISTORY_REPORT_REPLY	2330 // o - atf protocols\atfgenerated\lobbyetc_atf.txt

// PYR-61146
#define MSG_LOBBYETC_AUX_HAND_HISTORY_CLEAR_NUDGE			2331 // o - atf protocols\atfgenerated\lobbyetc_atf.txt
#define MSG_LOBBYETC_AUX_HAND_HISTORY_CLEAR_NUDGE_REPLY		2332 // o - atf protocols\atfgenerated\lobbyetc_atf.txt

// PYR-61146
#define MSG_LOBBYETC_CLIENT_AUX_HAND_HISTORY_REQUEST		2333 // o - atf protocols\atfgenerated\lobbyetc_atf.txt
#define MSG_LOBBYETC_CLIENT_AUX_HAND_HISTORY_REQUEST_REPLY	2334 // o - atf protocols\atfgenerated\lobbyetc_atf.txt

// PYR-70859
#define MSG_LOBBYETC_CLI_TOURN_STATS						2335 // o - atf protocols\atfgenerated\lobbyetc_atf.txt
#define MSG_LOBBYETC_CLI_TOURN_STATS_REPLY					2336 // o - atf protocols\atfgenerated\lobbyetc_atf.txt

// PYR-62644
#define MSG_LOBBYETC_VERIFY_PWD								2337 //  o - atf protocols\atfgenerated\lobbyetc_atf.txt
#define MSG_LOBBYETC_VERIFY_PWD_REPLY						2338 //  o - atf protocols\atfgenerated\lobbyetc_atf.txt

// PYR-65894 admin -> lobbyetc
#define MSG_LOBBYETC_CVL_OPT_IN_USER						2339 // 44 - reqId, userIntId
#define MSG_LOBBYETC_CVL_OPT_IN_USER_REPLY					2340 // 46[s] - reqId, errCode[errString]

// PYR-73672
#define MSG_LOBBYETC_GEOCOMPLY_LOCATE_REPLY_CONFIRM			2341 // o - atf protocols\atfgenerated\lobbyetc_atf.txt

//PYR-65428
#define MSG_LOBBYETC_MG_VALIDATE_MPC_TYPES           2342 //  44<s> - reqId,numMpcTypes<typeNames>
#define MSG_LOBBYETC_MG_VALIDATE_MPC_TYPES_REPLY     2343 //  42[s] - reqId,errCode[errString]

//PYR-77065
#define MSG_LOBBYETC_CLI_SE_SESSION_NOTIF					2344 // o - atf protocols\atfgenerated\lobbyetc_atf.txt

#define MSG_LOBBYETC_CLI_GET_SESSION_POPUP_INTERVAL			2345 // o - atf protocols\atfgenerated\lobbyetc_atf.txt
#define MSG_LOBBYETC_CLI_GET_SESSION_POPUP_INTERVAL_REPLY	2346 // o - atf protocols\atfgenerated\lobbyetc_atf.txt

#define MSG_LOBBYETC_CLI_SET_SESSION_POPUP_INTERVAL			2347 // o - atf protocols\atfgenerated\lobbyetc_atf.txt
#define MSG_LOBBYETC_CLI_SET_SESSION_POPUP_INTERVAL_REPLY	2348 // o - atf protocols\atfgenerated\lobbyetc_atf.txt

//PYR-67865
#define MSG_LOBBYETC_ADMIN_VOID_MPCS						2349 // 4<M(44<8>)> - numMpcVoidEntries<M(userIntId,numMpcIds<mpcIds>)>
#define MSG_LOBBYETC_ADMIN_VOID_MPCS_REPLY					2350 // 2[s4<M(482s)>] - errCode[errString,numErrors<M(userIntId,mpcId,errCode,errDescr)>]

// PYR-77653
// automation->lobbyetc
#define MSG_LOBBYETC_RESET_SR_BAR_FOR_USERS					2351	// 444<4s4> reqId, adminIntId, numOfUsers<userIntId,userId,licenseId>
#define MSG_LOBBYETC_RESET_SR_BAR_FOR_USERS_REPLY			2352	// 44<42s> reqId, numOfUsers<userIntId,errCode,errString>

// PYR-78513
// client->lobbyetc
#define MSG_LOBBYETC_GET_SESSION_WINLOSS_DATA				2353 // o - atf protocols\atfgenerated\lobbyetc_atf.txt
#define MSG_LOBBYETC_GET_SESSION_WINLOSS_DATA_REPLY			2354 // o - atf protocols\atfgenerated\lobbyetc_atf.txt

// PYR-49377
// bet server->lobbyetc
#define MSG_LOBBYETC_GET_RP_ESTIMATION						2355 // 448 reqId, userIntId, sppEarnedMillicents
#define MSG_LOBBYETC_GET_RP_ESTIMATION_REPLY				2356 // 46[s|8] - reqId, errCode[errString|rpEstimationCents]

//	PYR-83061
//	Lobby -> LobbyEtc
#define MSG_LOBBYETC_NOT_ALLOWED_TO_PLAY					2357 // 4o - reqId, LoginDbm::Protocol_LDBM_N_USER_NOT_ALLOWED_TO_PLAY
#define MSG_LOBBYETC_NOT_ALLOWED_TO_PLAY_REPLY				2358 // 46[s] - reqId, errCode[errString]

// PYR-82345, PYR-85575
// client -> lobbyEtc
#define MSG_LOBBYETC_SIGNAL_EVENT_LOGIN						2359 // ss[b] - appsFlyerId, mobileAppId, lat (see LobbyEtc::cli::Protocol_MSG_LOBBYETC_SIGNAL_EVENT_LOGIN)
#define MSG_LOBBYETC_SIGNAL_EVENT_LOGIN_REPLY				2360 // 46[s] - reqId, errCode[errString]

//PYR-79304
// admin-> lobbyetc
#define MSG_LOBBYETC_ADMIN_UPLOAD_CVL_BAR_CONFIG				2361	// ss4 - reqId,scriptFilename,script,token
#define MSG_LOBBYETC_ADMIN_UPLOAD_CVL_BAR_CONFIG_REPLY			2362	// 42[s|44<M(s44444<4>4<4>4<4>4)>4<s>] - reqId,errCode[errDesc|token,numConflictData<M(conflictingConfigName,removedLicenses,addedLicenses,removedSites,addedSites,numChangedTiers<changedTiers>,numRemovedTiers<removedTiers>,numAddedTiers<addedTiers>,numTestingScoreDelta)>,numWarnings<warnings>]

#define MSG_LOBBYETC_ADMIN_RETRIEVE_PENDING_CVL_BAR_CONFIG			2363	// 4 - reqId
#define MSG_LOBBYETC_ADMIN_RETRIEVE_PENDING_CVL_BAR_CONFIG_REPLY	2364	// 42[s|4s4<M(s44444<4>4<4>4<4>4)>4<s>] - reqId,errCode[errDesc|token,submitterId,numConflictData<M(conflictingConfigName,removedLicenses,addedLicenses,removedSites,addedSites,numChangedTiers<changedTiers>,numRemovedTiers<removedTiers>,numAddedTiers<addedTiers>,numTestingScoreDelta)>,numWarnings<warnings>]

#define MSG_LOBBYETC_ADMIN_REVIEW_PENDING_CVL_BAR_CONFIG			2365	// 44b - reqId,token,approved
#define MSG_LOBBYETC_ADMIN_REVIEW_PENDING_CVL_BAR_CONFIG_REPLY		2366	// 42[s] - reqId,errCode[errDesc]

// PYR-79304 AQA-21665
#define MSG_LOBBYETC_EXCHANGE_CURRENT_CVL_BAR						2367 //    
#define MSG_LOBBYETC_EXCHANGE_CURRENT_CVL_BAR_REPLY					2368 // 6[s|] - errCode[errDescr|num<ClientMpChestData)]

// PYR-77110 ice -> lobbyetc
#define MSG_LOBBYETC_UNLOCK_USER_MPC						2369 // 444	- reqId, userIntId, typeId
#define MSG_LOBBYETC_UNLOCK_USER_MPC_REPLY					2370 // 42[s|8] - reqId,errCode[errString|chestId]

// PYR-91136 automation -> lobbyetc
#define MSG_LOBBYETC_REPORT_AGE_NOT_VERIFIED						2371 // 44444 - reqId, userIntId, brandsUsed, platformsUsed, sitesVisited
#define MSG_LOBBYETC_REPORT_AGE_NOT_VERIFIED_REPLY					2372 // 46[s] - reqId, errCode[errString]

// PYR-92319
#define MSG_LOBBYETC_ADMIN_DOWNLOAD_PENDING_CVL_BAR_SCRIPT			2373	// 4b - reqId
#define MSG_LOBBYETC_ADMIN_DOWNLOAD_PENDING_CVL_BAR_SCRIPT_REPLY	2374	// 42[s|M(4s)] - reqId,errCode[errDesc|configId,script]

// PYR-92319
#define MSG_LOBBYETC_ADMIN_GET_PENDING_CVL_BAR_INFO					2375	// 4b - reqId
#define MSG_LOBBYETC_ADMIN_GET_PENDING_CVL_BAR_INFO_REPLY			2376	// 42[s|4s4tts] - reqId,errCode[errDesc|scriptId,configName,licenseMask,activationTime,expirationTime,submittedBy]

// PYR-92436
// automation->lobbyetc
#define MSG_LOBBYETC_VOID_CHESTS_FOR_USERS							2377	// 4444<4> reqId, adminIntId, chestType, numOfUsers<userIntId>
#define MSG_LOBBYETC_VOID_CHESTS_FOR_USERS_REPLY					2378	// 44<42s> reqId, numOfUsers<userIntId,errCode,errString>

// PYR-95957 ice -> lobbyetc
#define MSG_LOBBYETC_VOID_USER_MPC									2379 // 444	- reqId, userIntId, typeId
#define MSG_LOBBYETC_VOID_USER_MPC_REPLY							2380 // 42[s|8] - reqId,errCode[errString|chestId]

// PYR-100911 lobbyetc -> player (generic unsol notif)
#define MSG_LOBBYETC_CLI_UNSOLICITED_NOTIFICATION					2381 // see LobbyEtcClientProtocols.h

//	PYR-96179. Lobby -> LobbyEtc
#define MSG_LOBBYETC_PLAY_TIMELIMIT_WARNING							2382 // 4o - reqId, LoginDbm::Protocol_LDBM_N_LDBM_N_PLAYING_TIME_LIMIT
#define MSG_LOBBYETC_PLAY_TIMELIMIT_WARNING_REPLY					2383 // 46[s] - reqId, errCode[errString]

// PYR-106825 StarsCrm -> LobbyEtc
#define MSG_LOBBYETC_REQ_RM_BLOCKED_COUNTRIES_STATES				2384 // 4 - reqId
#define MSG_LOBBYETC_REQ_RM_BLOCKED_COUNTRIES_STATES_REPLY			2385 // 4o - reqId, starsCrm::Protocol_MSG_LOBBYETC_REQ_RM_BLOCKED_COUNTRIES_REPLY

// PYR-108724
// client -> LobbyEtc
#define MSG_LOBBYETC_CLI_CHECK_TWITCH_CONNECTION_STATUS				2386 // o - atf protocols\lobbyetc\lobbyEtcClientProtocols.txt
#define MSG_LOBBYETC_CLI_CHECK_TWITCH_CONNECTION_STATUS_REPLY		2387 // o - atf protocols\lobbyetc\lobbyEtcClientProtocols.txt

#define MSG_LOBBYETC_CLI_TWITCH_CONNECT								2388 // o - atf protocols\lobbyetc\lobbyEtcClientProtocols.txt
#define MSG_LOBBYETC_CLI_TWITCH_CONNECT_REPLY						2389 // o - atf protocols\lobbyetc\lobbyEtcClientProtocols.txt

#define MSG_LOBBYETC_CLI_TWITCH_DISCONNECT							2390 // o - atf protocols\lobbyetc\lobbyEtcClientProtocols.txt
#define MSG_LOBBYETC_CLI_TWITCH_DISCONNECT_REPLY					2391 // o - atf protocols\lobbyetc\lobbyEtcClientProtocols.txt

// PYR-106819
#define MSG_LOBBYETC_CLI_MG_SIDEBET_HISTORY_AUDIT					2392 // o - atf protocols\lobbyetc\lobbyEtcClientProtocols.txt
#define MSG_LOBBYETC_CLI_MG_SIDEBET_HISTORY_AUDIT_REPLY				2393 // o - atf protocols\lobbyetc\lobbyEtcClientProtocols.txt

// PYR-107053
// cashier->lobbyetc
#define MSG_CASHIER_TO_LOBBYETC_NOTIF_USERROLLS_DATA				2394 // 44sM - reqId, userIntId,userId,msgBody with data from MSG_CASHIER_TO_CLIENT_USERROLLS_GAMETICKETS_DATA_NOTIFICATION
#define MSG_CASHIER_TO_LOBBYETC_NOTIF_USERROLLS_DATA_REPLY			2395 // 46[s|4M] reqId, errCode[errString|gtClientItemsNum,M(gtBodyForClient)]

// PYR-112602
#define MSG_LOBBYETC_GET_TICKETS_WITH_TYPE							2396 // o - atf protocols\lobbyetc\lobbyEtcClientProtocols.txt
#define MSG_LOBBYETC_GET_TICKETS_WITH_TYPE_REPLY					2397 // o - atf protocols\lobbyetc\lobbyEtcClientProtocols.txt

// PYR-97802
#define MSG_LOBBYETC_GET_MY_STARS_COUNTERS							2398 // o - atf protocols\lobbyetc\lobbyEtcClientProtocols.txt
#define MSG_LOBBYETC_GET_MY_STARS_COUNTERS_REPLY					2399 // o - atf protocols\lobbyetc\lobbyEtcClientProtocols.txt

// PYR-107053
// cashier->lobbyetc
#define MSG_CASHIER_TO_LOBBYETC_NOTIF_USERROLL_CLEARED				2400 // 4444	- reqId, userIntId, rollId, rollType
#define MSG_CASHIER_TO_LOBBYETC_NOTIF_USERROLL_CLEARED_REPLY		2401 // 46[s]

//PYR-117862
//admin->lobbyetc
#define MSG_LOBBYETC_ADMIN_OUTBOUND_BOLDCHAT						2402 // 414		- reqId, chatType, userIntId
#define MSG_LOBBYETC_ADMIN_OUTBOUND_BOLDCHAT_REPLY					2403 // 46[s]	- reqId, errCode [errDescr]

//PYR-117862
//lobbyetc->client
#define MSG_LOBBYETC_CLI_OUTBOUND_BOLDCHAT_NOTIFICATION				2404 // 1	- chatType

// PYR-123307
#define MSG_LOBBYETC_ADMIN_GET_ASSETS								2405 // 4o - reqId,WebUxMsgAdminGetAssetsRequest
#define MSG_LOBBYETC_ADMIN_GET_ASSETS_REPLY							2406 // 46[s|o] - reqId,WebUxMsgAdminGetAssetsReply

#define MSG_LOBBYETC_ADMIN_SAVE_ASSET								2407 // 4o - reqId, WebUxMsgAdminSaveAssetRequest // ID - 0 new asset, reply message returns ID
#define MSG_LOBBYETC_ADMIN_SAVE_ASSET_REPLY							2408 // 46[s] - reqId,errCode,[errDescr]

#define MSG_LOBBYETC_ADMIN_ASSET_LINK								2409 // 4o - reqId, WebUxMsgAdminLinkAssetRequest
#define MSG_LOBBYETC_ADMIN_ASSET_LINK_REPLY							2410 // 46[s] - reqId,errCode,[errDescr]

#define MSG_LOBBYETC_ADMIN_ASSET_UNLINK								2411 // 4o - reqId, WebUxMsgAdminUnlinkAssetRequest
#define MSG_LOBBYETC_ADMIN_ASSET_UNLINK_REPLY						2412 // 46[s] - reqId,errCode,[errDescr]

#define MSG_LOBBYETC_ADMIN_SAVE_PUBRULE								2413 // 4o> - reqId, n<assetId>, WebUxMsgAdminSavePubruleRequest
#define MSG_LOBBYETC_ADMIN_SAVE_PUBRULE_REPLY						2414 // 46[s] - reqId,errCode,[errDescr]

#define MSG_LOBBYETC_ADMIN_PUBRULE_ASSETS_ADD						2415 // 4o - reqId, WebUxMsgAdminPubruleAssetsAddRequest
#define MSG_LOBBYETC_ADMIN_PUBRULE_ASSETS_ADD_REPLY					2416 // 46[s] - reqId,errCode,[errDescr]

#define MSG_LOBBYETC_ADMIN_PUBRULE_ASSETS_REMOVE					2417 // 4o - reqId, WebUxMsgAdminPubruleAssetsRemoveRequest
#define MSG_LOBBYETC_ADMIN_PUBRULE_ASSETS_REMOVE_REPLY				2418 //  46[s] - reqId,errCode,[errDescr]

#define MSG_LOBBYETC_CLI_GET_ASSETS									2419 // 4o - reqId, WebUxMsgClientGetAssetsRequest
#define MSG_LOBBYETC_CLI_GET_ASSETS_REPLY							2420 // 46[s|o] - reqId, errCode, [ errDescr | WebUxMsgClientGetAssetsReply ]

#define MSG_LOBBYETC_ADMIN_GET_RULE_CONFLICTS						2421 // 4 - reqId
#define MSG_LOBBYETC_ADMIN_GET_RULE_CONFLICTS_REPLY					2422 // 46[s|s] - reqId, errCode, [ errDescr | conflicts ]

// PYR-120600 
// mg->lobbyetc
#define MSG_LOBBYETC_MG_FIXED_PRIZE_DRAW_RESULT						2423 // o - atf srvshared\minigames\minigamesserverprot.txt
#define MSG_LOBBYETC_MG_FIXED_PRIZE_DRAW_RESULT_REPLY				2424 // o - atf srvshared\minigames\minigamesserverprot.txt

// PYR-123376
#define MSG_LOBBYETC_CLI_GET_MG_MY_STARS_DATA						2425 // o - atf protocols\lobbyetc\lobbyEtcClientProtocols.txt
#define MSG_LOBBYETC_CLI_GET_MG_MY_STARS_DATA_REPLY					2426 // o - atf protocols\lobbyetc\lobbyEtcClientProtocols.txt

//PYR-77065
#define DEFAULT_SESSION_POPUP_INTERVAL_MIN 30

enum GeoIPResult 
{
	GeoLocateResultInvalid				= 0,
	GeoLocateResultInBoundary			= 1, 
	GeoLocateResultNotInBoundary		= 2, 
	GeoLocateResultMobileRequired		= 3,
	GeoLocateResultUnknown				= 4,
	GeoLocateResultBlockedSoftware		= 5,
	GeoLocateResultUsingProxy			= 6,
	GeoIPReqSentToClient				= 7,
	GeoIPReqByAdmin						= 8,	// PYR-30754
	GeoIPReqBySecServer					= 9,	// PYR-41099
	GeoLocateResultInExclusionBoundary	= 10,	// PYR-74823
};

enum MobileRequiredStatus 
{ 
	WAIT_MOBILE_VERIFIED = 0,
	WAIT_USER_CONSENT,
	LOCAID_REG_STARTED,
	LOCAID_REG_SMS_DELIVERED,
	LOCAID_REG_COMPLETED,
	LOCAID_REG_DENIED,
	LOCAID_REG_TIMEOUT,
};

// PYR-29167
enum MobileEventType
{
	eMobileEventTypeUnknown = 0,
	eMobileEventTypeAccountCreate = 1,
	eMobileEventTypeFirstDeposit = 2,

	eMobileEventType_LAST = eMobileEventTypeFirstDeposit,		//	must be kept up to date
};

enum eLogoutActionRequest
{
	eLogoutActionRequest_None = 0,	
	eLogoutActionRequest_GreekTax = 1,	// PYR-30463 specificMsgBody: 8 - dailyProfit, 8 - profitTaxBoundary, s - currency, T - taxCutOffTime
};

enum eClientNotificationMessageType
{
	eClientNotificationMessageType_TheDealPrizeWinner = 0,
	eClientNotificationMessageType_Generic = 1,					//	PYR-49332
};

// PYR-53668, PYR-58657
enum ePlayHistoryRequestType
{
	ePlayHistoryRequestType_Undefined			= 0,
	ePlayHistoryRequestType_PlayHistoryAudit	= 1,
	ePlayHistoryRequestType_HandHistory			= 2,
	ePlayHistoryRequestType_AuxHandHistory		= 3,
};

// PYR-53668, PYR-58657
enum ePlayHistoryRequestStatus
{
	ePlayHistoryRequestStatus_Undefined		= 0,
	ePlayHistoryRequestStatus_InProgress	= 1,
	ePlayHistoryRequestStatus_Done			= 2,
	ePlayHistoryRequestStatus_Error			= 3,
};

// PYR-53668, PYR-58657
enum eReportDeliveryMethod
{
	eReportDeliveryMethod_Undefined		= 0,
	eReportDeliveryMethod_CommLib		= 1,
	eReportDeliveryMethod_Http			= 2,
};

//	 PYR-68327. Options for MSG_LOBBYETC_FORWARD_NOTIFICATION_TO_USER
enum ForwardNotificationOptions
{
	ForwardNotificationOptions_lobbyEtcConn = 1 << 0,	// 1
	ForwardNotificationOptions_rcs			= 1 << 1,	// 2
	ForwardNotificationOptions_rcsReplace	= 1 << 2,	// 4 PYR-76207

	ForwardNotificationOptions_sendToAllConns = ForwardNotificationOptions_lobbyEtcConn | ForwardNotificationOptions_rcs,
};

// PYR-100911 msg type for generic unsolicited client notifications
enum UnsolicitedNotificationType
{
	UnsolicitedNotificationType_Throwables		= 1,
	UnsolicitedNotificationType_WebNotification	= 2, // PYR-107053
};

//PYR-117862
enum BoldChatType
{
	BoldChatOutboundResponsibleGaming = 1,
	BoldChatOutboundRescue = 2,
};

#endif //LobbyEtc_h_included
