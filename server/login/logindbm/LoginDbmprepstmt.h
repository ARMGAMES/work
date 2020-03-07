#pragma once

#include "dbmstmt.h"
#include "dbm.h"
#include "pyrtime.h"
#include "maindbmstruct.h"
class ColPairMap;

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

class SampleTestingStmt : public DbmStatement
{
	PSqlString<OBJECT_NAME_LEN> objectName;
	PSqlBigInt					objectId;

	void prepareStmt();

public:
	SampleTestingStmt(DatabaseManagerCommon& man) : DbmStatement(man)
	{
		prepareStmt();
	}
	void init(const char* name, UINT64 object_id);
};


/////////////////////////////////////////////////////////////////////////////////////
class LoggedinStmtBase : public DbmStatement
{
protected:
	explicit LoggedinStmtBase(DatabaseManagerCommon& man) : DbmStatement(man)
	{}

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
	void setUser(const Loggedin& loginRecord);
	void getUser(Loggedin& loginRecord);
};


/////////////////////////////////////////////////////////////////////////////////////
class LoggedoutStmtBase : public DbmStatement
{
protected:
	explicit LoggedoutStmtBase(DatabaseManagerCommon& man) : DbmStatement(man)
	{}

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
};
