#ifndef chipsdbm_h_included
#define chipsdbm_h_included

#define CHIPSDBM_MIN					8000
#define CHIPSDBM_DELTA					1

inline UINT32 reqChipsDbmReplyId(UINT32 msgId) 
{ 
	return msgId + CHIPSDBM_DELTA; 
}

#define PM_PURCHASE_PENDING_STATUS	"P"
#define PM_PURCHASE_SUCCESS_STATUS	"A"
#define PM_PURCHASE_REJECT_STATUS	"R"
#define PM_PURCHASE_FAIL_STATUS		"F"

#define REQUEST_AUTH 0x8003
#define ANSWER_AUTH  4

#define CHIPSDBM_Q_GET_CHIPS_TRANS                  8001 // 44 - reqId, purchaseId
#define CHIPSDBM_A_GET_CHIPS_TRANS                  8002 // 46[s|struct PlayChipsTrans] - reqId, errCode, [errStr | struct PlayChipsTrans]

#define CHIPSDBM_Q_GET_AVAILABLE_PLAYCHIPSCONFIG    8003 // 4 - reqId
#define CHIPSDBM_A_GET_AVAILABLE_PLAYCHIPSCONFIG    8004 // 46[s|4<struct PlayChipsConfig>4ss] reqId, errCode, [errStr | total, <struct PlayChipsConfig>, locale, country, email]

#define CHIPSDBM_Q_INIT_PLAYCHIPS_PURCHASE		    8005 // 4s44 - reqId, userId, Amount, PM Amount, gateway
#define CHIPSDBM_A_INIT_PLAYCHIPS_PURCHASE		    8006 // 46[s|<struct PlayChipsTrans>] reqId, errCode, [errStr | PlayChipsTrans]

#define CHIPSDBM_Q_UPDATE_PLAYCHIPS_EXTREF			8007 // 44 - reqId, purchaseid
#define CHIPSDBM_A_UPDATE_PLAYCHIPS_EXTREF			8008 // 46[s] reqId, errCode, [errStr]

#define CHIPSDBM_Q_FINALIZE_PLAYCHIPS_PURCHASE	    8009 // 4<struct PlayChipsConfig> - reqId, PlayChipsTrans
#define CHIPSDBM_A_FINALIZE_PLAYCHIPS_PURCHASE	    8010 // 46[s|<struct PlayChipsTrans>] reqId, errCode, [errStr | PlayChipsTrans]

#define CHIPSDBM_Q_GET_STORE_AVAILABLE_PLAYCHIPSCONFIG    8011 // 44 - reqId, StoreId
#define CHIPSDBM_A_GET_STORE_AVAILABLE_PLAYCHIPSCONFIG    8012 // 46[s|4<struct PlayChipsConfig>] reqId, errCode, [errStr | total, <struct PlayChipsConfig>]

#define CHIPSDBM_Q_GOOGLE_PLAY_PURCHASE				8013 // 4sssb - reqId, userid, payload, signature, useNewPaywall
#define CHIPSDBM_A_GOOGLE_PLAY_PURCHASE				8014 // 46[s|8] reqId, errCode, [errStr | purchaseToken]

#define CHIPSDBM_Q_APPLE_STORE_PURCHASE				8015 // 4sb - reqId, data, useNewPaywall
#define CHIPSDBM_A_APPLE_STORE_PURCHASE				8016 // 46[s|8] reqId, errCode, [errStr | purchaseToken]

#define CHIPSDBM_Q_GET_USER_BUNDLES			8017 // 4 - parentAppTypeId
#define CHIPSDBM_A_GET_USER_BUNDLES			8018 // 6[s|4<M(4s2s884<M(s88)>4<M(4s)>22)>44422t] - errCode[errStr|numBundles<M(bundleID, name, type, imageName, chipAmount, regularChipAmount, numPrices<M(currency, regPrice, price)>, numProductIDs<M(appStoreEnumValue, appStoreProductID)>, bannerType, bundleVersionId)>, paywallID, segmentID, sortOrder, paywallVersionId, segmentVersionId, deactivateTime]

// enum ClientPlayChipsStore was MOVED to dbm.h

#define CHIPSDBM_ERR_PUBLIC_KEY_NOT_FOUND			-1000
#define CHIPSDBM_ERR_VERIFICATION_FAILED			-1001
#define CHIPSDBM_ERR_JSON_PARSE_FAILED				-1002
#define CHIPSDBM_ERR_NOT_A_PURCHASE					-1003
#define CHIPSDBM_ERR_PURCHASE_ALREADY_INITIATED		-1004
#define CHIPSDBM_ERR_PRODUCT_NOT_FOUND				-1005
#define CHIPSDBM_PURCHASE_ALREADY_COMPLETED			-1006

#define CHIPSDBM_GATEWAY_GOOGLE_PLAY				"GOOGLEPLAY"
#define CHIPSDBM_GATEWAY_APPLE_STORE				"APPLESTORE"


// PYR-46043
enum ePaywallBundleType
{
	ePaywallBundle_Regular		= 0,
	ePaywallBundle_Discount		= 1,
	ePaywallBundle_Booster		= 2,
};

// PYR-46043
enum ePaywallBannerType
{
	ePaywallBanner_None			= 0,
	ePaywallBanner_BestValue	= 1,
	ePaywallBundle_MostPopular	= 2,
};

// PYR-46043
enum ePaywallObjectType
{
	ePaywallObject_Paywall		= 0,
	ePaywallObject_Segment		= 1,
	ePaywallObject_Bundle		= 2,
};

// PYR-46043
enum ePaywallParamType
{
	ePaywallParam_Country				= 0,
	ePaywallParam_TrackCode				= 1,
	ePaywallParam_UserIntId				= 2,
	ePaywallParam_AppStoreProduct		= 3,
	ePaywallParam_CurrencyPrice			= 4,
	ePaywallParam_RegCurrencyPrice		= 5,
};

// PYR-46043
enum ePaywallStatusType
{
	ePaywallStatus_Inactive		= 0,
	ePaywallStatus_Active		= 1,
	ePaywallStatus_Deleted		= 2,
	ePaywallStatus_Obsolete		= 3,
	ePaywallStatus_Default		= 4,
};

// PYR-46043
enum ePaywallBundleSortOrder
{
	ePaywallBundleSort_None			= 0,
	ePaywallBundleSort_USDPriceAsc	= 1,
	ePaywallBundleSort_USDPriceDesc	= 2,
};

#endif // chipsdbm_h_included
