////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2009 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "VipStoreCommon.h"
#include "commmsgbody.h"
#include "shared.h"

////////////////////////////////////////////////////////////////////////////////

void VipStoreDeal::compose(CommMsgBody& msg) const
{
	msg.composeString(name)
	   .composeString(description)
	   .composeBOOL(isPerAccount)
	   .composeBOOL(isPercentage)
	   .composeUINT32(value)
	   .composeUINT32(maxAmount);

	expiry.compose(msg);
	UINT32 size = trackCodes.size();

	msg.composeUINT32(clientSites)
	   .composeUINT32(vipLevel)
	   .composeString(""/*itemCategory*/) // PYR-29268
	   .composeUINT32(itemId)
	   .composeUINT32(affiliateId)
	   .composeUINT32(size);

	for (UINT32 i = 0; i < size; ++i)
		msg.composeString(trackCodes[i]);

	// PYR-29268
	composeStringVector(msg, itemCategories);

	// PYR-119394
	msg.composeObject(clientSites_ex);
}

void VipStoreDeal::parse(CommMsgParser& parser)
{
	parser.parseStringP(name)
		  .parseStringP(description)
		  .parseBOOL(isPerAccount)
		  .parseBOOL(isPercentage)
		  .parseUINT32(value)
		  .parseUINT32(maxAmount);

	expiry.parse(parser);
	UINT32 size;

	const char* itemCategory; // PYR-29268
	parser.parseUINT32(clientSites)
		  .parseUINT32(vipLevel)
		  .parseString(itemCategory)
		  .parseUINT32(itemId)
		  .parseUINT32(affiliateId)
		  .parseUINT32(size);

	trackCodes.resize(size);
	for (UINT32 i = 0; i < size; ++i)
		parser.parseStringP(trackCodes[i]);

	// PYR-29268
	parseStringVector(parser, itemCategories);

	// PYR-119394 Only used by core server modules
	parser.parseObject(clientSites_ex);
	if (clientSites_ex == PLicenseMask::zero)
	{
		clientSites_ex = PLicenseMask::fromUINT64(clientSites); // approved usage of PLicenseMask::fromUINT64()
	}
}

void VipStoreDeal::print() const
{
	PTRACE5(	"VipStoreDeal: name='%s' descr='%s' perAcct=%d isPect=%d val=%d max=%d", 
				name.c_str(), 
				description.c_str(), 
				isPerAccount, 
				isPercentage, 
				value, 
				maxAmount);

	PTRACE5(	"sites=%d level=%d item=%d aff=%d",
				clientSites, vipLevel, itemId, affiliateId);

	PTRACE5("TrackCodes=%d", trackCodes.size());
	for( UINT32 i = 0; i < trackCodes.size(); ++i )
	{
		PTRACE5("%s", trackCodes[i].c_str());
	}

	PTRACE5("NumCats=%d", itemCategories.size());
	for( UINT32 i = 0; i < itemCategories.size(); ++i )
	{
		PTRACE5("%s", itemCategories[i].c_str());
	}

	PString tmp;
	PTRACE5("clientSites_ex=%s", clientSites_ex.toDbString(tmp));
}
////////////////////////////////////////////////////////////////////////////////

void VipStoreGenerateCodes::compose(CommMsgBody& msg) const
{
	msg.composeUINT32(dealIntId)
	   .composeBOOL(isUnique)
	   .composeUINT32(numCodes)
	   .composeString(prefix);
}


void VipStoreGenerateCodes::parse(CommMsgParser& parser)
{
	parser.parseUINT32(dealIntId)
		  .parseBOOL(isUnique)
		  .parseUINT32(numCodes)
		  .parseStringP(prefix);
}

//////////////////////////////////////////////////////////////////////////

// PYR-26304
void VipStoreShippingCompany::compose(CommMsgBody& msg) const
{
	msg.composeString(shipper)
		.composeString(url)
		.composeUINT32(flags);
}

// PYR-26304
void VipStoreShippingCompany::parse(CommMsgParser& parser)
{
	parser.parseStringP(shipper)
		.parseStringP(url)
		.parseUINT32(flags);
}

// PYR-26304
void VipStoreShippingCompany::print(const char* prefix) const
{
	PLog("%s: '%s' '%s' %X", prefix, shipper.c_str(), url.c_str(), flags);
}

