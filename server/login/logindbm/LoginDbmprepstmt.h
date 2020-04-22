#pragma once

#include "dbmstmt.h"
#include "dbm.h"
#include "pyrtime.h"
#include "maindbmstruct.h"


template<typename T>
class NoExecStmtT : public T
{
public:
	explicit NoExecStmtT<T>(::DatabaseManagerCommon& man) : T(man) {}

protected: // Disallow invocation of execute methods
	using T::execute;
	using T::executeRet;
	using T::execUpdateDelete;
};

typedef NoExecStmtT<DbmStatement> NoExecStmt;

/////////////////////////////////////////////////////////////////////////////////////
class LoggedinStmtBase : public DbmStatement
{
protected:
	explicit LoggedinStmtBase(DatabaseManagerCommon& man) : DbmStatement(man)
	{}

	// inputs
	PSqlBigInt							inputLoginId;

	// outputs
	PSqlBigInt							loginId;
	PSqlString<USERS_USERID_LEN>		userId;
	PSqlInt								userIntId;
	PSqlTimestamp						loginTime;
	PSqlInt								clientDevice;
	PSqlInt								sessionFlags;
	PSqlInt								loginAuthMethod;
	PSqlInt								siteId;
	PSqlString<INSTALL_ID_LEN>			installId;
	PSqlString<IP_ADDR_LEN>				ipAddress;
	PSqlInt								ipPort;
	PSqlString<USERS_COUNTRY_LEN>		ipCountry;
	PSqlString<USERS_STATE_LEN>			ipState;
	PSqlString<MAC_ADDR_LEN>			macAddress;
	PSqlString<ROUTERMAC_ADDR_LEN>		routerMacAddr;

	void bindAllParams();
	void bindAllColumns();
	void appendAllColumns(PString& query);
	void setLogin(const LoginRecord& loginRecord);
	void getLogin(LoginRecord& loginRecord);
};

class InsertLoginStmt : public LoggedinStmtBase
{
	void prepareStmt();

public:
	explicit InsertLoginStmt(DatabaseManagerCommon& man);
	void exec(const LoginRecord& loginRecord);
};

class GetLoginByLoginIdStmt : public LoggedinStmtBase
{
	void prepareStmt();

public:
	explicit GetLoginByLoginIdStmt(DatabaseManagerCommon& man);
	bool execGet(UINT64 loginId, LoginRecord& loginRecord);
};


/////////////////////////////////////////////////////////////////////////////////////
class LoggedoutStmtBase : public DbmStatement
{
protected:
	explicit LoggedoutStmtBase(DatabaseManagerCommon& man) : DbmStatement(man)
	{}

	// inputs
	PSqlBigInt							inputLoginId;

	// outputs
	PSqlBigInt							loginId;
	PSqlString<USERS_USERID_LEN>		userId;
	PSqlInt								userIntId;
	PSqlTimestamp						loginTime;
	PSqlTimestamp						logoutTime;
	PSqlInt								logoutType;
	PSqlInt								clientDevice;
	PSqlInt								sessionFlags;
	PSqlInt								loginAuthMethod;
	PSqlInt								siteId;
	PSqlString<INSTALL_ID_LEN>			installId;
	PSqlString<IP_ADDR_LEN>				ipAddress;
	PSqlInt								ipPort;
	PSqlString<USERS_COUNTRY_LEN>		ipCountry;
	PSqlString<USERS_STATE_LEN>			ipState;
	PSqlString<MAC_ADDR_LEN>			macAddress;
	PSqlString<ROUTERMAC_ADDR_LEN>		routerMacAddr;

	void bindAllParams();
	void bindAllColumns();
	void appendAllColumns(PString& query);
	void setLogout(const LogoutRecord& logoutRecord);
	void getLogout(LogoutRecord& logoutRecord);
};

class InsertLogoutStmt : public LoggedoutStmtBase
{
	void prepareStmt();

public:
	explicit InsertLogoutStmt(DatabaseManagerCommon& man);
	void exec(const LogoutRecord& logoutRecord);
};

class GetLogoutByLoginIdStmt : public LoggedoutStmtBase
{
	void prepareStmt();

public:
	explicit GetLogoutByLoginIdStmt(DatabaseManagerCommon& man);
	bool execGet(UINT64 loginId, LogoutRecord& logoutRecord);
};
