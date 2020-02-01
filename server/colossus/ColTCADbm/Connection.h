// ===========================================================
// Copyright (C) 2018 PYR Software Ltd. All rights reserved.
// ===========================================================

#ifndef Connection_h_included
#define Connection_h_included

#include "CommonAsyncCall.h"

class ColTCADbmServerObject;

enum ServerConnectionType
{
	LowerGuard = 0,
	TrustedAdmin = 1,
	Admin = 2,
	TCADetector = 3,
	UpperGuard
};

static const char* ConnectionNames[] = 
{
	"", "TA", "A", "TCA"
};

////////////////////////////////////////////////////////////////////////////////

class ColTCADbmConnection : public CommServerConnection
{
protected:
	ColTCADbmServerObject* obj;
	ServerConnectionType connectionType;

public:
	ColTCADbmConnection( ColTCADbmServerObject* obj_, ServerConnectionType connectionType_ )
	{
		obj = obj_;
		connectionType = connectionType_;
	}
	ServerConnectionType getConnType() const { return connectionType;}
	virtual void processMessage( UINT32 msgId, const CommMsgBody& body );
};

////////////////////////////////////////////////////////////////////////////////

class ColTCADbmPrivilegedConnection : public ColTCADbmConnection
{
protected:
	ColTCADbmPrivilegedConnection(ColTCADbmServerObject* obj_, ServerConnectionType connectionType_) :
		ColTCADbmConnection(obj_, connectionType_), userIntId(0)
	{}

public:
	const char* getAdminId() const					{ return adminId.c_str(); }

	UINT32 getUserIntId() const						{ return userIntId; }
	void setUserIntId(UINT32 intId)					{ userIntId = intId; }

	void addAdminRight(const char* right)			{ adminRights.insert(right); }
	bool checkAdminRight(const char* right) const	{ return adminRights.find(right) != adminRights.end(); }

protected:
	PString adminId;
	UINT32 userIntId;
	PStringSet adminRights;
};

////////////////////////////////////////////////////////////////////////////////

class ColTCADbmAdminServerConnection : public ColTCADbmPrivilegedConnection
{
public:
	explicit ColTCADbmAdminServerConnection( ColTCADbmServerObject* obj_ ) :
		ColTCADbmPrivilegedConnection(obj_, Admin), authError(0)
	{}

	void connected() override;
	void closedOrDisconnected(int errCode, const char* errMsg) override;
	void saveAuthError(INT16 errCode) { authError = errCode; }
	void getUserAuth(const char* guardType, const char* user, CommMsgBody& clientExtra, CommMsgBody& authBody) override;

private:
	INT16 authError;
};

////////////////////////////////////////////////////////////////////////////////

class ColTCADbmTrustedAdminServerConnection : public ColTCADbmPrivilegedConnection
{
public:
	explicit ColTCADbmTrustedAdminServerConnection( ColTCADbmServerObject* obj_ ) :
		ColTCADbmPrivilegedConnection(obj_, TrustedAdmin)
	{}

	void parseInfo(CommMsgParser& parser);
};

////////////////////////////////////////////////////////////////////////////////

class ColTCADbmServerConnFactory : public CommServerConnectionFactory
{
public:
	ColTCADbmServerConnFactory( CommServerGuardFactory& guardFactory, ServerConnectionType connectionType_ )
		: CommServerConnectionFactory( guardFactory ), obj(nullptr), connectionType(connectionType_)
	{}

	/* virtual */ /* new */ CommServerConnection* createConnection()
	{
		return new ColTCADbmConnection( obj, connectionType );
	}

	void init(ColTCADbmServerObject* obj_)
	{
		obj = obj_;
	}

public:
	ColTCADbmServerObject* obj;

private:
	ServerConnectionType connectionType;
};

class ColTCADbmAdminServerConnFactory : public CommServerConnectionFactory
{
public:
	ColTCADbmAdminServerConnFactory(CommServerGuardFactory& guardFactory )
		: CommServerConnectionFactory( guardFactory ), obj(nullptr)
	{
		++normalPriority;
		++authPriority;
	}

	void init(ColTCADbmServerObject* obj_)
	{
		obj = obj_;
	}

	/* new */ CommServerConnection* createConnection() override
	{
		return new ColTCADbmAdminServerConnection( obj );
	}

public:
	ColTCADbmServerObject* obj;
};

class ColTCADbmTrustedAdminServerConnFactory : public CommServerConnectionFactory
{
public:
	ColTCADbmTrustedAdminServerConnFactory( CommServerGuardFactory& guardFactory )
		: CommServerConnectionFactory( guardFactory ), obj(nullptr)
	{
		++normalPriority;
		++authPriority;
	}

	void init(ColTCADbmServerObject* obj_)
	{
		obj = obj_;
	}

	/* new */ CommServerConnection* createConnection() override
	{
		return new ColTCADbmTrustedAdminServerConnection( obj );
	}

public:
	ColTCADbmServerObject* obj;
};

////////////////////////////////////////////////////////////////////////////////
class AdminAuthAsyncCall : public BaseAsyncCallT<ColTCADbmServerObject>
{
public:
	AdminAuthAsyncCall(ColTCADbmServerObject* pObject, const ColTCADbmAdminServerConnection* pConn) :
		BaseAsyncCallT<ColTCADbmServerObject>(pObject, pConn)
	{}

	void processReply(UINT32 requestId, UINT32 msgId, const CommMsgBody& body) override;
};

////////////////////////////////////////////////////////////////////////////////

class ColTCADbmOutgoingConnection : public CommClientGConnection
{
public:
	ColTCADbmOutgoingConnection(CommClientGuardFactory& guardFactory, const char* name_, char symbol_)
		: CommClientGConnection(guardFactory), obj(nullptr), name(name_), symbol(symbol_), isConnected(0)
	{}

	void init(ColTCADbmServerObject* obj_)
	{
		obj = obj_;
	}

	const char* getName() const { return name; }
	int getConnected() const { return isConnected; }

	/* virtual */ void connected();
	/* virtual */ bool closedOrDisconnected(int errCode, const char* errMsg);
	/* virtual */ void processGMessage(UINT32 reqId, UINT32 msgId, const CommMsgBody& body, AsyncCall* call);
	/* virtual */ UINT32 post(UINT32 msgId, CommMsgBody& body, AsyncCall* call);

protected:
	ColTCADbmServerObject* obj;
	PString name;
	char symbol;
	int isConnected;
};

#endif // Connection_h_included
