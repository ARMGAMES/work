#pragma once


#include "commsrv.h"

class MainDbmServerObject;

enum ServerConnectionType
{
	LowerGuard = 0,
	TrustedAdmin = 1,
	Admin = 2,
	ExampleSrv = 3,
	UpperGuard
};

struct CONN_TYPE_STRUCT
{
	int connectionType;
	const char* marker;
};

static const CONN_TYPE_STRUCT CONN_TYPES[] =
{
	{ LowerGuard, "" }
	, { TrustedAdmin, "TA" }
	, { Admin, "A" }
	, { ExampleSrv, "EX" }
	, { UpperGuard, "" }
};

const char* getConnMarker(int connType);

////////////////////////////////////////////////////////////////////////////////

class GenericSrvConnection : public CommServerConnection
{
protected:
	MainDbmServerObject& obj;
	ServerConnectionType connectionType;

public:
	GenericSrvConnection(MainDbmServerObject& obj_, ServerConnectionType connectionType_);

	ServerConnectionType getConnType() const;
	const char* getConnMarker() const;
	virtual void processMessage( UINT32 msgId, const CommMsgBody& body );
};

////////////////////////////////////////////////////////////////////////////////

class GenericPrivilegedSrvConnection : public GenericSrvConnection
{
protected:
	GenericPrivilegedSrvConnection(MainDbmServerObject& obj_, ServerConnectionType connectionType_);

public:
	const char* getAdminId() const;

	UINT32 getUserIntId() const;
	void setUserIntId(UINT32 intId);

	void addAdminRight(const char* right);
	bool checkAdminRight(const char* right) const;
	void getAdminRights(vector<PString>& rights) const;

protected:
	PString adminId;
	UINT32 userIntId;
	PStringSet adminRights;
};

////////////////////////////////////////////////////////////////////////////////

class AdminSrvConnection : public GenericPrivilegedSrvConnection
{
public:
	explicit AdminSrvConnection(MainDbmServerObject& obj_);

	void connected() override;
	void closedOrDisconnected(int errCode, const char* errMsg) override;
	void saveAuthError(INT16 errCode);
	void getUserAuth(const char* guardType, const char* user, CommMsgBody& clientExtra, CommMsgBody& authBody) override;

private:
	INT16 authError;
};

////////////////////////////////////////////////////////////////////////////////

class TrustedAdminSrvConnection : public GenericPrivilegedSrvConnection
{
public:
	explicit TrustedAdminSrvConnection(MainDbmServerObject& obj_);

	void parseInfo(CommMsgParser& parser);
};

////////////////////////////////////////////////////////////////////////////////

class GenericServerConnFactory : public CommServerConnectionFactory
{
public:
	GenericServerConnFactory(MainDbmServerObject& obj_, CommServerGuardFactory& guardFactory, ServerConnectionType connectionType_ )
		: CommServerConnectionFactory( guardFactory ), obj(obj_), connectionType(connectionType_)
	{}

	/* virtual */ /* new */ CommServerConnection* createConnection()
	{
		return new GenericSrvConnection( obj, connectionType );
	}

public:
	MainDbmServerObject& obj;

private:
	ServerConnectionType connectionType;
};

class AdminServerConnFactory : public CommServerConnectionFactory
{
public:
	AdminServerConnFactory(MainDbmServerObject& obj_, CommServerGuardFactory& guardFactory )
		: CommServerConnectionFactory( guardFactory ), obj(obj_)
	{
		++normalPriority;
		++authPriority;
	}

	/* new */ CommServerConnection* createConnection() override
	{
		return new AdminSrvConnection( obj );
	}

public:
	MainDbmServerObject& obj;
};

class TrustedAdminServerConnFactory : public CommServerConnectionFactory
{
public:
	TrustedAdminServerConnFactory(MainDbmServerObject& obj_, CommServerGuardFactory& guardFactory )
		: CommServerConnectionFactory( guardFactory ), obj(obj_)
	{
		++normalPriority;
		++authPriority;
	}

	/* new */ CommServerConnection* createConnection() override
	{
		return new TrustedAdminSrvConnection( obj );
	}

public:
	MainDbmServerObject& obj;
};

////////////////////////////////////////////////////////////////////////////////

class GenericOutgoingConnection : public CommClientGConnection
{
public:
	GenericOutgoingConnection(MainDbmServerObject& obj_, CommClientGuardFactory& guardFactory, const char* name_, char symbol_)
		: CommClientGConnection(guardFactory), 
		obj(obj_), 
		name(name_), 
		symbol(symbol_),
		isConnected(0)
	{}

	const char* getName() const { return name; }
	int getConnected() const { return isConnected; }
	UINT32 getConnId() const;

	virtual void connected() override;
	virtual bool closedOrDisconnected(int errCode, const char* errMsg) override;
	virtual void processGMessage(UINT32 reqId, UINT32 msgId, const CommMsgBody& body, AsyncCall* call) override;
	virtual UINT32 post(UINT32 msgId, CommMsgBody& body, AsyncCall* call) override;

protected:
	MainDbmServerObject& obj;
	PString name;
	char symbol;
	int isConnected;
};

