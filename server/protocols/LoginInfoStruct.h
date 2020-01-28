#ifndef	logininfostruct_h_included
#define	logininfostruct_h_included

// ===========================================================
// Copyright (C) 2013 PYR Software Ltd. All rights reserved.
// ===========================================================

#include "commmsgbody.h"

struct LoginInfoStruct
{
	BYTE	objectType;
	UINT64	objectId; // PYR-29549
	PString ipAddress;
	LoginInfoStruct(BYTE object_type, UINT64 object_id, const char* ipAddress_)
		: objectType(object_type), objectId(object_id), ipAddress(ipAddress_)
	{}

	bool operator < (const LoginInfoStruct& other) const
	{
		if( objectType != other.objectType )
			return ( objectType < other.objectType );
		return ( objectId < other.objectId );
	}

	void parse(CommMsgParser& parser)
	{
		parser.
			parseBYTE(objectType).
			parseUINT64(objectId). // PYR-29549
			parseStringP(ipAddress)
			;
	}

	void compose(CommMsgBody& body) const
	{
		body.
			composeBYTE(objectType).
			composeUINT64(objectId). // PYR-29549
			composeString(ipAddress)
			;
	}

private:
	LoginInfoStruct();
};


struct AppLoginInfoStruct
{
	BYTE	objectType;
	UINT64	objectId;
	SrvTime loginTime; // PYR-49061
	AppLoginInfoStruct(BYTE object_type, UINT64 object_id, const SrvTime& loginTime_)
		: objectType(object_type), objectId(object_id), loginTime(loginTime_)
	{ }

	AppLoginInfoStruct()
		: objectType(0), objectId(0)
	{ }

	bool operator < (const LoginInfoStruct& other) const
	{
		if( objectType != other.objectType )
			return ( objectType < other.objectType );
		return ( objectId < other.objectId );
	}

	void parse(CommMsgParser& parser)
	{
		parser.
			parseBYTE(objectType).
			parseUINT64(objectId).
			parseSrvTime(loginTime);
	}

	void compose(CommMsgBody& body) const
	{
		body.
			composeBYTE(objectType).
			composeUINT64(objectId).
			composeSrvTime(loginTime);
	}	
	//	PYR-72821
	bool isLoginWithinTimeRange( const SrvTime& startTime, const SrvTime& endTime ) const
	{
		return loginTime.compareTo( startTime ) >= 0 && loginTime.compareTo( endTime ) <= 0;
	}
};

// PYR-38803 - consolidated user login info from either LOGGEDIN or LOGGEDOUT tables
struct FullUserLoginInfo
{
	// LOGGEDIN/LOGGEDOUT table fields
	UINT64  loginId;
	PString userId;
	INT16   objectType;
	UINT64  objectId;
	PString ipAddress;
	INT32   ipPort;
	PString macAddress;
	PString	volumeLabel;
	PString installId;
	SrvTime loginTime;
	UINT32  userIntId;
	UINT32  siteId; 
	UINT32  platformId; 
	INT32   loginAuthMethod; 
	PString ipCountry;
	PString ipState;
	INT32	clientType;
	INT32	clientDevice;
	UINT32	sessionFlags;
	INT32	platformUsed;
	PString routerMacAddr;
	INT32   clientTypeUsed;
	INT32   brandsUsed;
	// Additional LOGGEDOUT table fields
	SrvTime logoutTime; // can be null if user is not yet logged out
	INT16   logoutType; // irrelevant when logoutTime = NULL
	
	FullUserLoginInfo()
		: loginId(0)
		, objectType(0)
		, objectId(0)
		, ipPort(0)
		, userIntId(0)
		, siteId(0)
		, platformId(0)
		, loginAuthMethod(0)
		, clientType(0)
		, clientDevice(0)
		, sessionFlags(0)
		, platformUsed(0)
		, clientTypeUsed(0)
		, brandsUsed(0)
		, logoutType(0)
	{}

	FullUserLoginInfo(CommMsgParser& parser)
	{
		parse(parser);
	}

	CommMsgParser& parse(CommMsgParser& parser)
	{
		parser
			.parseUINT64(loginId)
			.parseStringP(userId)
			.parseINT16(objectType)
			.parseUINT64(objectId)
			.parseStringP(ipAddress)
			.parseINT32(ipPort)
			.parseStringP(macAddress)
			.parseStringP(volumeLabel)
			.parseStringP(installId) 
			.parseSrvTime(loginTime)
			.parseUINT32(userIntId) 
			.parseUINT32(siteId)  
			.parseUINT32(platformId)
			.parseINT32(loginAuthMethod)
			.parseStringP(ipCountry)
			.parseStringP(ipState)	 
			.parseINT32(clientType)
			.parseINT32(clientDevice)
			.parseUINT32(sessionFlags) 
			.parseINT32(platformUsed) 
			.parseStringP(routerMacAddr)
			.parseINT32(clientTypeUsed)
			.parseINT32(brandsUsed)
			.parseSrvTime(logoutTime)
			.parseINT16(logoutType)
			;

		return parser;
	}

	CommMsgBody& compose(CommMsgBody& body) const
	{
		body
			.composeUINT64(loginId)
			.composeString(userId)
			.composeINT16(objectType)
			.composeUINT64(objectId)
			.composeString(ipAddress)
			.composeINT32(ipPort)
			.composeString(macAddress)
			.composeString(volumeLabel)
			.composeString(installId) 
			.composeSrvTime(loginTime)
			.composeUINT32(userIntId) 
			.composeUINT32(siteId)  
			.composeUINT32(platformId)
			.composeINT32(loginAuthMethod)
			.composeString(ipCountry)
			.composeString(ipState)	 
			.composeINT32(clientType)
			.composeINT32(clientDevice)
			.composeUINT32(sessionFlags) 
			.composeINT32(platformUsed) 
			.composeString(routerMacAddr)
			.composeINT32(clientTypeUsed)
			.composeINT32(brandsUsed)
			.composeSrvTime(logoutTime)
			.composeINT16(logoutType)
			;

		return body;
	}	
};

#endif
