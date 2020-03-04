
#include "maindbmstruct.h"

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