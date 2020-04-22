
#include "LoginDbmprepstmt.h"
#include "initdatetime.h"

/////////////////////////////////////////////////////////////////////////////////////

void LoggedinStmtBase::bindAllParams()
{
	bindFirstParam(loginId);
	bindNextParam(userId);
	bindNextParam(userIntId);
	bindNextParam(loginTime);
	bindNextParam(clientDevice);
	bindNextParam(sessionFlags);
	bindNextParam(loginAuthMethod);
	bindNextParam(siteId);
	bindNextParam(installId);
	bindNextParam(ipAddress);
	bindNextParam(ipPort);
	bindNextParam(ipCountry);
	bindNextParam(ipState);
	bindNextParam(macAddress);
	bindNextParam(routerMacAddr);
}

void LoggedinStmtBase::bindAllColumns()
{
	bindFirstCol(loginId);
	bindNextCol(userId);
	bindNextCol(userIntId);
	bindNextCol(loginTime);
	bindNextCol(clientDevice);
	bindNextCol(sessionFlags);
	bindNextCol(loginAuthMethod);
	bindNextCol(siteId);
	bindNextCol(installId);
	bindNextCol(ipAddress);
	bindNextCol(ipPort);
	bindNextCol(ipCountry);
	bindNextCol(ipState);
	bindNextCol(macAddress);
	bindNextCol(routerMacAddr);
}

void LoggedinStmtBase::appendAllColumns(PString& query)
{
	query
		.append("LOGINID,USERID,USERINTID,LOGINTIME,CLIENTDEVICE,")				// 5
		.append("SESSIONFLAGS,LOGINAUTHMETHOD,SITEID,INSTALLID,IPADDRESS,")		// 10
		.append("IPPORT,IPCOUNTRY,IPSTATE,MACADDRESS,ROUTERMACADDR");			// 15
}

void LoggedinStmtBase::setLogin(const LoginRecord& loginRecord)
{
	loginId = loginRecord.loginId;
	userId.initCut(loginRecord.userId);
	userIntId = loginRecord.userIntId;
	loginTime = loginRecord.loginTime;
	clientDevice = loginRecord.clientDevice;
	sessionFlags = loginRecord.sessionFlags;
	loginAuthMethod = loginRecord.loginAuthMethod;
	siteId = loginRecord.siteId;
	installId.initCut(loginRecord.installId);
	ipAddress.initCut(loginRecord.ipAddress);
	ipPort = loginRecord.ipPort;
	ipCountry.initCut(loginRecord.ipCountry);
	ipState.initCut(loginRecord.ipState);
	macAddress.initCut(loginRecord.macAddress);
	routerMacAddr.initCut(loginRecord.routerMacAddr);
}

void LoggedinStmtBase::getLogin(LoginRecord& loginRecord)
{
	loginRecord.loginId = loginId.value;
	loginRecord.userId = userId.c_str();
	loginRecord.userIntId = userIntId.value;
	loginTime.getValue(loginRecord.loginTime);
	loginRecord.clientDevice = clientDevice.value;
	loginRecord.sessionFlags = sessionFlags.value;
	loginRecord.loginAuthMethod = loginAuthMethod.value;
	loginRecord.siteId = siteId.value;
	loginRecord.installId = installId.c_str();
	loginRecord.ipAddress = ipAddress.c_str();
	loginRecord.ipPort = ipPort.value;
	loginRecord.ipCountry = ipCountry.c_str();
	loginRecord.ipState = ipState.c_str();
	loginRecord.macAddress = macAddress.c_str();
	loginRecord.routerMacAddr = routerMacAddr.c_str();
}

InsertLoginStmt::InsertLoginStmt(DatabaseManagerCommon& man)
	: LoggedinStmtBase(man)
{
	prepareStmt();
}

void InsertLoginStmt::prepareStmt()
{
	PString query;
	query
		.assign("INSERT INTO " DB_TABLE_LOGGEDIN " (");

	appendAllColumns(query);

	query
		.append(") VALUES(")
		.append("?,?,?,?,?,")	// 5
		.append("?,?,?,?,?,")	// 10
		.append("?,?,?,?,?")	// 15
		.append(")");

	prepare(query);

	bindAllParams();
}

void InsertLoginStmt::exec(const LoginRecord& loginRecord)
{
	setLogin(loginRecord);
	execute();
}


GetLoginByLoginIdStmt::GetLoginByLoginIdStmt(DatabaseManagerCommon& man)
	: LoggedinStmtBase(man)
{
	prepareStmt();
}

void GetLoginByLoginIdStmt::prepareStmt()
{
	PString query;
	query.assign("SELECT ");

	appendAllColumns(query);

	query
		.append(" FROM " DB_TABLE_LOGGEDIN " WHERE LOGINID = ?");

	prepare(query);

	bindFirstParam(inputLoginId);
	bindAllColumns();
}

bool GetLoginByLoginIdStmt::execGet(UINT64 loginId, LoginRecord& loginRecord)
{
	inputLoginId = loginId;

	bool found = false;
	execute();
	if (fetch())
	{
		getLogin(loginRecord);
		found = true;
	}

	closeCursor();
	return found;
}

/////////////////////////////////////////////////////////////////////////////////////

void LoggedoutStmtBase::bindAllParams()
{
	bindFirstParam(loginId);
	bindNextParam(userId);
	bindNextParam(userIntId);
	bindNextParam(loginTime);
	bindNextParam(logoutTime);
	bindNextParam(logoutType);
	bindNextParam(clientDevice);
	bindNextParam(sessionFlags);
	bindNextParam(loginAuthMethod);
	bindNextParam(siteId);
	bindNextParam(installId);
	bindNextParam(ipAddress);
	bindNextParam(ipPort);
	bindNextParam(ipCountry);
	bindNextParam(ipState);
	bindNextParam(macAddress);
	bindNextParam(routerMacAddr);
}

void LoggedoutStmtBase::bindAllColumns()
{
	bindFirstCol(loginId);
	bindNextCol(userId);
	bindNextCol(userIntId);
	bindNextCol(loginTime);
	bindNextCol(logoutTime);
	bindNextCol(logoutType);
	bindNextCol(clientDevice);
	bindNextCol(sessionFlags);
	bindNextCol(loginAuthMethod);
	bindNextCol(siteId);
	bindNextCol(installId);
	bindNextCol(ipAddress);
	bindNextCol(ipPort);
	bindNextCol(ipCountry);
	bindNextCol(ipState);
	bindNextCol(macAddress);
	bindNextCol(routerMacAddr);
}

void LoggedoutStmtBase::appendAllColumns(PString& query)
{
	query
		.append("LOGINID,USERID,USERINTID,LOGINTIME,LOGOUTTIME,")
		.append("LOGOUTTYPE,CLIENTDEVICE,SESSIONFLAGS,LOGINAUTHMETHOD,SITEID,")
		.append("INSTALLID,IPADDRESS,IPPORT,IPCOUNTRY,IPSTATE,")
		.append("MACADDRESS,ROUTERMACADDR");
}

void LoggedoutStmtBase::setLogout(const LogoutRecord& logoutRecord)
{
	loginId = logoutRecord.loginId;
	userId.initCut(logoutRecord.userId);
	userIntId = logoutRecord.userIntId;
	loginTime = logoutRecord.loginTime;
	logoutTime = logoutRecord.logoutTime;
	logoutType = logoutRecord.logoutType;
	clientDevice = logoutRecord.clientDevice;
	sessionFlags = logoutRecord.sessionFlags;
	loginAuthMethod = logoutRecord.loginAuthMethod;
	siteId = logoutRecord.siteId;
	installId.initCut(logoutRecord.installId);
	ipAddress.initCut(logoutRecord.ipAddress);
	ipPort = logoutRecord.ipPort;
	ipCountry.initCut(logoutRecord.ipCountry);
	ipState.initCut(logoutRecord.ipState);
	macAddress.initCut(logoutRecord.macAddress);
	routerMacAddr.initCut(logoutRecord.routerMacAddr);
}

void LoggedoutStmtBase::getLogout(LogoutRecord& logoutRecord)
{
	logoutRecord.loginId = loginId.value;
	logoutRecord.userId = userId.c_str();
	logoutRecord.userIntId = userIntId.value;
	loginTime.getValue(logoutRecord.loginTime);
	logoutTime.getValue(logoutRecord.logoutTime);
	logoutRecord.logoutType = logoutType.value;
	logoutRecord.clientDevice = clientDevice.value;
	logoutRecord.sessionFlags = sessionFlags.value;
	logoutRecord.loginAuthMethod = loginAuthMethod.value;
	logoutRecord.siteId = siteId.value;
	logoutRecord.installId = installId.c_str();
	logoutRecord.ipAddress = ipAddress.c_str();
	logoutRecord.ipPort = ipPort.value;
	logoutRecord.ipCountry = ipCountry.c_str();
	logoutRecord.ipState = ipState.c_str();
	logoutRecord.macAddress = macAddress.c_str();
	logoutRecord.routerMacAddr = routerMacAddr.c_str();
}

InsertLogoutStmt::InsertLogoutStmt(DatabaseManagerCommon& man)
	: LoggedoutStmtBase(man)
{
	prepareStmt();
}

void InsertLogoutStmt::prepareStmt()
{
	PString query;
	query
		.assign("INSERT INTO " DB_TABLE_LOGGEDOUT " (");

	appendAllColumns(query);

	query
		.append(") VALUES(")
		.append("?,?,?,?,?,")	// 5
		.append("?,?,?,?,?,")	// 10
		.append("?,?,?,?,?,")	// 15
		.append("?,?")			// 17
		.append(")");

	prepare(query);

	bindAllParams();
}

void InsertLogoutStmt::exec(const LogoutRecord& logoutRecord)
{
	setLogout(logoutRecord);
	execute();
}


GetLogoutByLoginIdStmt::GetLogoutByLoginIdStmt(DatabaseManagerCommon& man)
	: LoggedoutStmtBase(man)
{
	prepareStmt();
}

void GetLogoutByLoginIdStmt::prepareStmt()
{
	PString query;
	query.assign("SELECT ");

	appendAllColumns(query);

	query
		.append(" FROM " DB_TABLE_LOGGEDOUT " WHERE LOGINID = ?");

	prepare(query);

	bindFirstParam(inputLoginId);
	bindAllColumns();
}

bool GetLogoutByLoginIdStmt::execGet(UINT64 loginId, LogoutRecord& logoutRecord)
{
	inputLoginId = loginId;

	bool found = false;
	execute();
	if (fetch())
	{
		getLogout(logoutRecord);
		found = true;
	}

	closeCursor();
	return found;
}