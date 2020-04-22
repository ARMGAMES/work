
#include "maindbmstruct.h"

/////////////////////////////////////////////////////////////////////////////
User::User()
{
	userIntId = 0;
	privileges = 0;
	flags = 0;
	licenseId = 0;
	siteId = 0;

	lastAccessed = 0;
}

void User::compose(CommMsgBody& body) const
{
	body
		.composeUINT32(userIntId)
		.composeString(userId)
		.composeString(email)
		.composeSrvTime(registered)
		.composeString(firstName)
		.composeString(middleName)
		.composeString(lastName)
		.composeSrvDate(birthday)
		.composeString(addr_1)
		.composeString(addr_2)
		.composeString(city)
		.composeString(state)
		.composeString(country)
		.composeString(zipCode)
		.composeString(phone)
		.composeString(sex)
		.composeUINT64(privileges)
		.composeUINT64(flags)
		.composeString(comments)
		.composeString(encrPwd)
		.composeUINT32(licenseId)
		.composeUINT32(siteId)
		.composeString(defaultCurrency);
}

void User::parse(CommMsgParser& parser)
{
	parser
		.parseUINT32(userIntId)
		.parseStringP(userId)
		.parseStringP(email)
		.parseSrvTime(registered)
		.parseStringP(firstName)
		.parseStringP(middleName)
		.parseStringP(lastName)
		.parseSrvDate(birthday)
		.parseStringP(addr_1)
		.parseStringP(addr_2)
		.parseStringP(city)
		.parseStringP(state)
		.parseStringP(country)
		.parseStringP(zipCode)
		.parseStringP(phone)
		.parseStringP(sex)
		.parseUINT64(privileges)
		.parseUINT64(flags)
		.parseStringP(comments)
		.parseStringP(encrPwd)
		.parseUINT32(licenseId)
		.parseUINT32(siteId)
		.parseStringP(defaultCurrency);
}

LoginRecord::LoginRecord()
{
	loginId = 0;
	userIntId = 0;
	clientDevice = 0;
	sessionFlags = 0;
	loginAuthMethod = 0;
	siteId = 0;
	ipPort = 0;
}

void LoginRecord::compose(CommMsgBody& body) const
{
	body
		.composeUINT64(loginId)
		.composeString(userId)
		.composeUINT32(userIntId)
		.composeSrvTime(loginTime)
		.composeUINT32(clientDevice)
		.composeUINT32(sessionFlags)
		.composeUINT32(loginAuthMethod)
		.composeUINT32(siteId)
		.composeString(installId)
		.composeString(ipAddress)
		.composeUINT32(ipPort)
		.composeString(ipCountry)
		.composeString(ipState)
		.composeString(macAddress)
		.composeString(routerMacAddr)
		;

}

/////////////////////////////////////////////////////////////////////////////

void LoginRecord::parse(CommMsgParser& parser)
{
	parser
		.parseUINT64(loginId)
		.parseStringP(userId)
		.parseUINT32(userIntId)
		.parseSrvTime(loginTime)
		.parseUINT32(clientDevice)
		.parseUINT32(sessionFlags)
		.parseUINT32(loginAuthMethod)
		.parseUINT32(siteId)
		.parseStringP(installId)
		.parseStringP(ipAddress)
		.parseUINT32(ipPort)
		.parseStringP(ipCountry)
		.parseStringP(ipState)
		.parseStringP(macAddress)
		.parseStringP(routerMacAddr)
		;
}

LogoutRecord::LogoutRecord()
{
	loginId = 0;
	userIntId = 0;
	logoutType = 0;
	clientDevice = 0;
	sessionFlags = 0;
	loginAuthMethod = 0;
	siteId = 0;
	ipPort = 0;
}

void LogoutRecord::compose(CommMsgBody& body) const
{
	body
		.composeUINT64(loginId)
		.composeString(userId)
		.composeUINT32(userIntId)
		.composeSrvTime(loginTime)
		.composeSrvTime(logoutTime)
		.composeUINT32(logoutType)
		.composeUINT32(clientDevice)
		.composeUINT32(sessionFlags)
		.composeUINT32(loginAuthMethod)
		.composeUINT32(siteId)
		.composeString(installId)
		.composeString(ipAddress)
		.composeUINT32(ipPort)
		.composeString(ipCountry)
		.composeString(ipState)
		.composeString(macAddress)
		.composeString(routerMacAddr)
		;

}

void LogoutRecord::parse(CommMsgParser& parser)
{
	parser
		.parseUINT64(loginId)
		.parseStringP(userId)
		.parseUINT32(userIntId)
		.parseSrvTime(loginTime)
		.parseSrvTime(logoutTime)
		.parseUINT32(logoutType)
		.parseUINT32(clientDevice)
		.parseUINT32(sessionFlags)
		.parseUINT32(loginAuthMethod)
		.parseUINT32(siteId)
		.parseStringP(installId)
		.parseStringP(ipAddress)
		.parseUINT32(ipPort)
		.parseStringP(ipCountry)
		.parseStringP(ipState)
		.parseStringP(macAddress)
		.parseStringP(routerMacAddr)
		;
}