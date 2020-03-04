#pragma once

#include "dbmstmt.h"
#include "dbm.h"
#include "maindbmstruct.h"

/////////////////////////////////////////////////////////////////////
// USERS table
class UserStmtBase : public DbmStatement
{
protected:
	explicit UserStmtBase(DatabaseManagerCommon& man) : DbmStatement(man)
	{}

	PSqlInt								userIntId;
	PSqlString<USERS_USERID_LEN>		userId;
	PSqlString<USERS_EMAIL_LEN>			email;
	PSqlTimestamp						registered;
	PSqlString<USERS_FIRSTNAME_LEN>		firstName;
	PSqlString<USERS_MIDDLENAME_LEN>	middleName;
	PSqlString<USERS_LASTNAME_LEN>		lastName;
	PSqlDateNullable					birthday;
	PSqlString<USERS_ADDR_1_LEN>		addr_1;
	PSqlString<USERS_ADDR_2_LEN>		addr_2;
	PSqlString<USERS_CITY_LEN>			city;
	PSqlString<USERS_STATE_LEN>			state;
	PSqlString<USERS_COUNTRY_LEN>		country;
	PSqlString<USERS_ZIPCODE_LEN>		zipCode;
	PSqlString<USERS_PHONE_LEN>			phone;
	PSqlString<USERS_SEX_LEN>			sex;
	PSqlBigInt							privileges;
	PSqlBigInt							flags;
	PSqlString<USERS_COMMNETS_LEN>		comments;
	PSqlString<USERS_ENCRPWD_LEN>		encPwd;
	PSqlInt								licenseId;
	PSqlInt								siteId;
	PSqlString<USERS_CURRENCY_LEN>		defaultCurrency;

	void bindAllParams();
	void bindAllColumns();
	void appendAllColumns(PString& query);
	void setUsers(const User& user);
	void getUsers(User& user);
};

class InsertUserStmt : public UserStmtBase
{
	void prepareStmt();

public:
	explicit InsertUserStmt(DatabaseManagerCommon& man);
	void exec(const User& user);
};