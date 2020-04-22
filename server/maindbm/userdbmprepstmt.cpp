
#include "userdbmprepstmt.h"

/////////////////////////////////////////////////////////////////////////////////////

void UserStmtBase::bindAllParams()
{
	bindFirstParam(userIntId);
	bindNextParam(userId);
	bindNextParam(email);
	bindNextParam(registered);
	bindNextParam(firstName);
	bindNextParam(middleName);
	bindNextParam(lastName);
	bindNextParam(birthday);
	bindNextParam(addr_1);
	bindNextParam(addr_2);
	bindNextParam(city);
	bindNextParam(state);
	bindNextParam(country);
	bindNextParam(zipCode);
	bindNextParam(phone);
	bindNextParam(sex);
	bindNextParam(privileges);
	bindNextParam(flags);
	bindNextParam(comments);
	bindNextParam(encrPwd);
	bindNextParam(licenseId);
	bindNextParam(siteId);
	bindNextParam(defaultCurrency);
}

void UserStmtBase::bindAllColumns()
{
	bindFirstCol(userIntId);
	bindNextCol(userId);
	bindNextCol(email);
	bindNextCol(registered);
	bindNextCol(firstName);
	bindNextCol(middleName);
	bindNextCol(lastName);
	bindNextCol(birthday);
	bindNextCol(addr_1);
	bindNextCol(addr_2);
	bindNextCol(city);
	bindNextCol(state);
	bindNextCol(country);
	bindNextCol(zipCode);
	bindNextCol(phone);
	bindNextCol(sex);
	bindNextCol(privileges);
	bindNextCol(flags);
	bindNextCol(comments);
	bindNextCol(encrPwd);
	bindNextCol(licenseId);
	bindNextCol(siteId);
	bindNextCol(defaultCurrency);
}

void UserStmtBase::appendAllColumns(PString& query)
{
	query
		.append("USERINTID,USERID,EMAIL,REGISTERED,FIRSTNAME,")		// 5
		.append("MIDDLENAME,LASTNAME,BIRTHDAY,ADDR_1,ADDR_2,")		// 10
		.append("CITY,STATE,COUNTRY,ZIPCODE,PHONE,")				// 15
		.append("SEX,PRIVILEGES,FLAGS,COMMENTS,ENCRPWD,")			// 20
		.append("LICENSEID,SITEID,DEFAULTCURRENCY");				// 23
}

void UserStmtBase::setUser(const User& user)
{
	userIntId = user.userIntId;
	userId.initCut(user.userId);
	email.initCut(user.email);
	registered = user.registered;
	firstName.initCut(user.firstName);
	middleName.initCut(user.middleName);
	lastName.initCut(user.lastName);
	birthday = user.birthday;
	addr_1.initCut(user.addr_1);
	addr_2.initCut(user.addr_2);
	city.initCut(user.city);
	state.initCut(user.state);
	country.initCut(user.country);
	zipCode.initCut(user.zipCode);
	phone.initCut(user.phone);
	sex.initCut(user.sex);
	privileges = user.privileges;
	flags = user.flags;
	comments.initCut(user.comments);
	encrPwd.initCut(user.encrPwd);
	licenseId = user.licenseId;
	siteId = user.siteId;
	defaultCurrency.initCut(user.defaultCurrency);
}

void UserStmtBase::getUser(User& user)
{
	user.userIntId = userIntId.value;
	user.userId = userId.c_str();
	user.email = email.c_str();
	registered.getValue(user.registered);
	user.firstName = firstName.c_str();
	user.middleName = middleName.c_str();
	user.lastName = lastName.c_str();
	birthday.getValue(user.birthday);
	user.addr_1 = addr_1.c_str();
	user.addr_2 = addr_2.c_str();
	user.city = city.c_str();
	user.state = state.c_str();
	user.country = country.c_str();
	user.zipCode = zipCode.c_str();
	user.phone = phone.c_str();
	user.sex = sex.c_str();
	user.privileges = privileges.value;
	user.flags = flags.value;
	user.comments = comments.c_str();
	user.encrPwd = encrPwd.c_str();
	user.licenseId = licenseId.value;
	user.siteId = siteId.value;
	user.defaultCurrency = defaultCurrency.c_str();
}

InsertUserStmt::InsertUserStmt(DatabaseManagerCommon& man)
	: UserStmtBase(man)
{
	prepareStmt();
}

void InsertUserStmt::prepareStmt()
{
	PString query;
	query
		.assign("INSERT INTO " DB_TABLE_USERS " (");

	appendAllColumns(query);

	query
		.append(") VALUES(")
		.append("?,?,?,?,?,")	// 5
		.append("?,?,?,?,?,")	// 10
		.append("?,?,?,?,?,")	// 15
		.append("?,?,?,?,?,")	// 20
		.append("?,?,?")	// 23
		.append(")");

	prepare(query);

	bindAllParams();
}

void InsertUserStmt::exec(const User& user)
{
	setUser(user);
	execute();
}

GetUserByUserIdStmt::GetUserByUserIdStmt(DatabaseManagerCommon& man)
	: UserStmtBase(man)
{
	prepareStmt();
}

void GetUserByUserIdStmt::prepareStmt()
{
	PString query;
	query.assign("SELECT ");

	appendAllColumns(query);

	query
		.append(" FROM " DB_TABLE_USERS " WHERE USERID = ?");

	prepare(query);

	bindFirstParam(inputUserId);
	bindAllColumns();
}

bool GetUserByUserIdStmt::execGet(const char* userId_, User& user)
{
	inputUserId.initCut(userId_);

	bool found = false;
	execute();
	if (fetch())
	{
		getUser(user);
		found = true;
	}

	closeCursor();
	return found;
}