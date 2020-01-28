// EtcDbmBDUACommon.h 
////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef EtcDbmBDUACommon_h_included
#define EtcDbmBDUACommon_h_included

#include "ppincludebase.h"
#include "commmsgbody.h"

class BDUAData
{
public:
	INT32 attrType;
	mutable INT32 attrValue;

	BDUAData() 
		: attrType(0)
		, attrValue(0)
	{}

	explicit BDUAData(INT32 attrType_) 
		: attrType(attrType_)
		, attrValue(0)
	{}

	BDUAData(INT32 attrType_, INT32 attrValue_) 
		: attrType(attrType_)
		, attrValue(attrValue_)
	{}

	CommMsgBody& compose( CommMsgBody& body ) const
	{
		body.composeINT32( attrType )
			.composeINT32( attrValue );

		return body;
	}

	CommMsgParser& parse( CommMsgParser& parser )
	{
		parser.parseINT32( attrType )
			.parseINT32( attrValue );

		return parser;
	}
};

class BusinessDefinedUserAttributeData : public BDUAData
{

public:
	UINT32 userIntId;

	BusinessDefinedUserAttributeData() : userIntId(0)
	{}

	CommMsgBody& compose( CommMsgBody& body ) const
	{
		body.composeUINT32( userIntId );
		BDUAData::compose(body);

		return body;
	}

	CommMsgParser& parse( CommMsgParser& parser )
	{
		parser.parseUINT32( userIntId );
		BDUAData::parse(parser);

		return parser;
	}
};

enum eBusinessUserAttributeTypes
{
	eBusinessUserAttributeTypeUnkown				= 0,

	// CVL attributes

	eBusinessUserAttributeType_CVL_PlayerScore		= 1,
	eBusinessUserAttributeType_CVL_AffinityScore	= 2,
	eBusinessUserAttributeType_CVL_BoostScore		= 3,
	eBusinessUserAttributeType_CVL_TestingScore		= 4,
	eBusinessUserAttributeType_CVL_StartingTier		= 5,
	
	eBusinessUserAttributeType_XSellButtonConfig	= 20, // processed as a CVL attribute
	eBusinessUserAttributeType_CVL_PokerMiniGame	= 21, // PYR-61457 Used to display the appropriate mini game to players via help_support_portal
	eBusinessUserAttributeType_PokerHubPersona		= 22, // PYR-121398 Used to target different widget contents for players viewing the Hub

	// NOTE: 
	// Special BDUAs 
	// bigger then eBusinessUserAttributeType_CVL_Last
	// Will not be stored in database
	eBusinessUserAttributeType_CVL_UsedPlayerScore = 50, // PYR-92260

	// Update Last this when new CVL attributes are added
	eBusinessUserAttributeType_CVL_Last				= eBusinessUserAttributeType_PokerHubPersona, // the last BDUA loaded from the DB
	eBusinessUserAttributeType_CVL_First			= eBusinessUserAttributeType_CVL_PlayerScore, // the first BDUA loaded from the DB
	
	// please leave a gap for future CVL attributes
};

#endif // EtcDbmBDUACommon_h_included
