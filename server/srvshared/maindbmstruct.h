#pragma once

#include "pyrtime.h"

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
	PString		encPwd;
	UINT32		licenseId;
	UINT32		siteId;
	PString		defaultCurrency;

	User();

	void compose(CommMsgBody& body) const;
	void parse(CommMsgParser& parser);

};