#pragma once

#include "pyrtime.h"
#include "ppcontainer.h"

struct User
{
	UINT32		userIntId;
	PString		userId;
	PString		email;
	SrvTime		registered;
	PString		firstName;
	PString		middleName;
	PString		lastName;
	SrvDate		birthday;
	PString		addr_1;
	PString		addr_2;
	PString		city;
	PString		state;
	PString		country;
	PString		zipCode;
	PString		phone;
	PString		sex;
	UINT64		privileges;
	UINT64		flags;
	PString		comments;
	PString		encrPwd;
	UINT32		licenseId;
	UINT32		siteId;
	PString		defaultCurrency;

	// local variable
	time_t		lastAccessed;
	User();

	void compose(CommMsgBody& body) const;
	void parse(CommMsgParser& parser);

};

struct LoginRecord
{
	UINT64  loginId;
	PString userId;
	UINT32  userIntId;
	SrvTime loginTime;
	UINT32  clientDevice;
	UINT32  sessionFlags;
	UINT32  loginAuthMethod;
	UINT32  siteId;
	PString installId;
	PString ipAddress;
	UINT32  ipPort;
	PString ipCountry;
	PString ipState;
	PString macAddress;
	PString routerMacAddr;

	LoginRecord();

	void compose(CommMsgBody& body) const;
	void parse(CommMsgParser& parser);
};

struct LogoutRecord
{
	UINT64  loginId;
	PString userId;
	UINT32  userIntId;
	SrvTime loginTime;
	SrvTime logoutTime;
	UINT32  logoutType;
	UINT32  clientDevice;
	UINT32  sessionFlags;
	UINT32  loginAuthMethod;
	UINT32  siteId;
	PString installId;
	PString ipAddress;
	UINT32  ipPort;
	PString ipCountry;
	PString ipState;
	PString macAddress;
	PString routerMacAddr;

	LogoutRecord();

	void compose(CommMsgBody& body) const;
	void parse(CommMsgParser& parser);
};