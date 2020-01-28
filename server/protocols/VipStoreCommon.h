#ifndef VipStoreCommon_h_included
#define VipStoreCommon_h_included

////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2009 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "pplib.h"
#include "pyrtime.h"
#include "licensemask.h"

class CommMsgBody;
class CommMsgParser;

////////////////////////////////////////////////////////////////////////////////
// Message IDs

#define MSG_(x) x, x##_REPLY,

enum
{
	VIPSTORE_MESSAGE_BASE = 1000,

	MSG_(VIPSTORE_REFRESH_SETTINGS) // 1001
	MSG_(VIPSTORE_REFRESH_ITEMS)	// 1003
// !!! WARNING !!! If you modify this message, please let testautomation team know.
	MSG_(VIPSTORE_CANCEL_ORDER)		// 1005
	MSG_(VIPSTORE_GET_ORDER)		// 1007
	MSG_(VIPSTORE_ORDERS_BY_STATUS) // 1009
	MSG_(VIPSTORE_GET_ITEM)			// 1011
	MSG_(VIPSTORE_SAVE_ITEM)		// 1013
	MSG_(VIPSTORE_UPDATE_ORDER_STATUS)	// 1015
	MSG_(VIPSTORE_CREATE_DEAL)			// 1017
	MSG_(VIPSTORE_GENERATE_CODES)		// 1019
	MSG_(VIPSTORE_INV_GET_ITEM_ROUTING)	// 1021
	MSG_(VIPSTORE_INV_SET_ITEM_ROUTING)	// 1023
	MSG_(VIPSTORE_INV_GET_WAREHOUSE_ITEM)	// 1025
	MSG_(VIPSTORE_INV_SET_WAREHOUSE_ITEM)	// 1027
// !!! WARNING !!! If you modify this message, please let testautomation team know.
	MSG_(VIPSTORE_ADMIN_PURCHASE_ITEM)		// 1029
	MSG_(VIPSTORE_INV_GET_ORDER)	// 1031
	MSG_(VIPSTORE_INV_MOVE_ORDER)	// 1033
	MSG_(VIPSTORE_INV_UPDATE_WAREHOUSE_QUANTITY)	// 1035
	MSG_(VIPSTORE_INV_ADD_RETURNED_ITEM)	// 1037
	MSG_(VIPSTORE_INV_UPDATE_RETURNED_ITEM)	// 1039
	MSG_(VIPSTORE_INV_EDIT_GET_DEFAULT_ROUTING)	// 1041
	MSG_(VIPSTORE_INV_EDIT_SET_DEFAULT_ROUTING)	// 1043
// !!! WARNING !!! If you modify this message, please let testautomation team know.
	MSG_(VIPSTORE_CANCEL_ORDERS)	// 1045
	MSG_(VIPSTORE_GET_USER_ALT_ADDRESSES) // 1047 no parameters; reply 44(o) - suggestedMax, numAddr, (VipStoreAlternateClientAddress)
	MSG_(VIPSTORE_OBSOLETE_USER_ALT_ADDRESS)// 1049 4 - addrId
	MSG_(VIPSTORE_UPDATE_ORDER_EXPORT_STATUS) // 1051
	MSG_(VIPSTORE_GET_USER_ALT_ADDRESSES2) // 1053 no parameters; reply 44(M(o)) - suggestedMax, numAddr, (M(VipStoreAlternateClientAddress))
	MSG_(VIPSTORE_SAVE_SHIPPER) // 1055 PYR-26304 ss4 - shipper, url, flags
	MSG_(VIPSTORE_SET_ITEMS_OBSOLETE) // 1057 PYR-31130 4<4>b - numberOfItems,<itemId>, obsolete
									  // VIPSTORE_SET_ITEMS_OBSOLETE_REPLY 2s[4<4>4<4s>] - errCode, errDescr[,numOfSuccess<itemId>, numOfFailures<itemId,itemErrDecr>]
};

#undef MSG_

static inline UINT32 vipStoreReplyId(UINT32 msgId) { return msgId + 1; }

////////////////////////////////////////////////////////////////////////////////
// Admin rights

#define VIPSTORE_CREATE_DEAL_RIGHT		"CreateStoreDeal"
#define VIPSTORE_GENERATE_CODES_RIGHT	"GenerateDealCodes"

////////////////////////////////////////////////////////////////////////////////

struct VipStoreDeal
{
	PString name;				// 100 chars
	PString description;		// 256 chars
	bool isPerAccount = false;	// true = per account, false = per system
	bool isPercentage = false;	// true = percentage, false = fixed amount
	UINT32 value = 0;			// Fixed or percentage, depending on above
	UINT32 maxAmount = 0;		// If percentage
	SrvDate expiry;				// Deal's gone...

	// Optional filters
	UINT32 clientSites = 0;		// (0 = all) Bitmap mask, see "enum LicenseId"
	UINT32 vipLevel = 0;		// See enum VipStatusLevel in dbm.h
	UINT32 itemId = 0;			// [storeitems]
	UINT32 affiliateId = 0;		// [affiliates: userintid]
	vector<PString> trackCodes;	// [usertrackcodes]
	vector<PString> itemCategories;// [itemprops: _category], (PYR-29268)
	PLicenseMask clientSites_ex = LicenceBitmask_All;// PYR-119394 IT IS ACTUALLY LICENSE MASK!!!

	void compose(CommMsgBody& msg) const;
	void parse(CommMsgParser& parser);
	void print() const;
};

////////////////////////////////////////////////////////////////////////////////

struct VipStoreGenerateCodes
{
	UINT32 dealIntId;			// deal identifier
	bool isUnique;				// true = unique codes, false = one code
	UINT32 numCodes;			// number of codes (uses) to generate
	PString prefix;				// 4-character code prefix

	void compose(CommMsgBody& msg) const;
	void parse(CommMsgParser& parser);
};

////////////////////////////////////////////////////////////////////////////////

// PYR-26304
struct VipStoreShippingCompany
{
	enum FlagMask
	{
		eFlagsObsolete = 1,
	};

	PString shipper;
	PString url;
	UINT32 flags;

	VipStoreShippingCompany() : flags(0) {}
	VipStoreShippingCompany(const char* shipper_, const char* url_, UINT32 flags_)
	 : shipper(shipper_), url(url_), flags(flags_)
	{}

	void compose(CommMsgBody& msg) const;
	void parse(CommMsgParser& parser);
	void print(const char* prefix) const;
};

#endif // VipStoreCommon_h_included
