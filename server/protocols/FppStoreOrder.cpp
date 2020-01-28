////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2009 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "FppStoreOrder.h"
#include "composecolls.h"
#include "dbm.h"

////////////////////////////////////////////////////////////////////////////////

void FppStoreOrder::compose(CommMsgBody& body) const
{
	body.
		composeUINT32(orderId).
		composeUINT32(price).
		composeString(userId).
		composeString(itemDescr).
		composeString(status).
		composeString(trackingId).
		composeString(comment).
		composeSrvTime(when).
		composeUINT16(static_cast<UINT16>(specs.size()));

	for (size_t i = 0; i < specs.size(); i++)
		body.composeString(specs[i]);

	body.
		composeUINT32(itemId).
		composeString(shipper).
		composeSrvDate(shipDate).
		composeString(trackingUrl).
		composeUINT32(itemQuantity).
		composeUINT32(cartId).
		composeUINT32(fulfillmentId);

		altAddr.compose(body); // PYR-15672: VipStore alternate delivery address
	body.composeUINT32(flags); // PYR-23743
}

////////////////////////////////////////////////////////////////////////////////

void FppStoreOrder::parse(CommMsgParser& parser)
{
	UINT16 numSpecs;
	parser.
		parseUINT32(orderId).
		parseUINT32(price).
		parseStringP(userId).
		parseStringP(itemDescr).
		parseStringP(status).
		parseStringP(trackingId).
		parseStringP(comment).
		parseSrvTime(when).
		parseUINT16(numSpecs);

	specs.resize(numSpecs);
	for (int i = 0; i < numSpecs; i++)
		parser.parseStringP(specs[i]);

	parser.
		parseUINT32(itemId).
		parseStringP(shipper).
		parseSrvDate(shipDate).
		parseStringP(trackingUrl).
		parseUINT32(itemQuantity).
		parseUINT32(cartId).
		parseUINT32(fulfillmentId);

	// PYR-15672: VipStore alternate delivery address
	//if (!parser.parseEnded())
	{
		altAddr.parse(parser);
	}
	parser.parseUINT32(flags);
}

////////////////////////////////////////////////////////////////////////////////

void FppStoreOrder::composeVector(const vector<FppStoreOrder>& orders, CommMsgBody& body)
{
	composeVectorOfComposableValues(orders, body);
}

////////////////////////////////////////////////////////////////////////////////

void FppStoreOrder::composeForClientVector(const vector<FppStoreOrder>& orders, CommMsgBody& body)
{
	body.composeUINT32(orders.size());
	for(size_t i = 0; i < orders.size(); i++)
	{
		CommMsgBody msg;
		const FppStoreOrder& order = orders[i];
		order.compose(msg);
		body.composeMsgBody(msg);
	}
}

////////////////////////////////////////////////////////////////////////////////

void FppStoreOrder::parseVector(vector<FppStoreOrder>& orders, CommMsgParser& parser)
{
	parseVectorOfParsableValues(orders, parser);
}

////////////////////////////////////////////////////////////////////////////////

void FppStoreOrder::parseForClientVector(vector<FppStoreOrder>& orders, CommMsgParser& parser)
{
	UINT32 numOrders;
	parser.parseUINT32(numOrders);
	orders.resize(numOrders);
	for(size_t i = 0; i < numOrders; i++)
	{
		FppStoreOrder& order = orders[i];
		CommMsgBody msg;
		parser.parseMsgBody(msg);
		CommMsgParser parser2(msg);
		order.parse(parser2);
	}
}

////////////////////////////////////////////////////////////////////////////////
