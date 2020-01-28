#ifndef FppStoreOrder_h
#define FppStoreOrder_h

////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2009 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "commlib.h"
#include "pyrtime.h"
#include "VipStoreInventorySharedCli.h"

////////////////////////////////////////////////////////////////////////////////

struct FppStoreOrder
{
	UINT32	orderId;
	UINT32	price;
	UINT32	itemId; 
	UINT32	itemQuantity;
	PString userId;
	PString itemDescr;
	PString trackingId;
	PString status;
	PString comment;
	SrvTime when;
	vector<PString> specs;

	// [AO 2009-11-23] PYR-12626
	PString shipper;
	SrvDate shipDate;
	PString trackingUrl;

	UINT32 cartId, fulfillmentId; // [AO 2010-09-16] #15895

	VipStoreAlternateClientAddress altAddr; // PYR-15672: VipStore alternate delivery address

	UINT32 flags;		// PYR-23743

	FppStoreOrder() :	orderId(0),
						price(0),
						itemId(0),
						itemQuantity(1), // for compatibility with older versions
						cartId(0),
						fulfillmentId(0),
						flags(0)	// PYR-23743
	{
	}

	void compose(CommMsgBody& body) const;
	void parse(CommMsgParser& parser);
	static void composeVector(const vector<FppStoreOrder>& orders, CommMsgBody& body);
	static void parseVector(vector<FppStoreOrder>& orders, CommMsgParser& parser);

	static void composeForClientVector(const vector<FppStoreOrder>& orders, CommMsgBody& body);
	static void parseForClientVector(vector<FppStoreOrder>& orders, CommMsgParser& parser);
};

////////////////////////////////////////////////////////////////////////////////

#endif // FppStoreOrder_h
