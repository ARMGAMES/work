////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef GenericConnections_h_included
#define GenericConnections_h_included

////////////////////////////////////////////////////////////////////////////////

#include "commsrv.h"
#include "GenericServer.h"

class GenericServerObjectBase;
class GenericServerWithLoginAuthObjectBase;
class GenericDbmServerWithLoginAuthObjectBase;

////////////////////////////////////////////////////////////////////////////////

class GenericSrvConn : public CommServerConnection
{
protected:
	GenericServerObjectBase& obj;
	ServerConnectionType connectionType;

public:
	GenericSrvConn(GenericServerObjectBase& obj_, ServerConnectionType connectionType_);

	virtual void processMessage(UINT32 msgId, const CommMsgBody& body) override;
	virtual const char* getConnMarker() const;
	virtual void getExtraLogMarkers(PString& /*logStr*/) const { }
};

////////////////////////////////////////////////////////////////////////////////

class GenericPrivilegedSrvConn : public GenericSrvConn
{
protected:
	GenericPrivilegedSrvConn(GenericServerObjectBase& obj_, ServerConnectionType connectionType_);

public:
	const char* getAdminId() const;

	UINT32 getUserIntId() const;
	void setUserIntId(UINT32 userIntId_);

	void addAdminRight(const char* right);
	bool checkAdminRight(const char* right) const;
	void getAdminRights(vector<PString>& rights) const;

	virtual void getExtraLogMarkers(PString& logStr) const override;

protected:
	PString adminId;
	UINT32 userIntId;
	PStringSet adminRights;
};

////////////////////////////////////////////////////////////////////////////////

class AdminSrvConnBase : public GenericPrivilegedSrvConn
{
public:
	explicit AdminSrvConnBase(GenericServerWithLoginAuthObjectBase& obj_);
	virtual ~AdminSrvConnBase() { }

	void connected() override;
	void closedOrDisconnected(int errCode, const char* errMsg) override;
	void saveAuthError(INT16 errCode);
	void getUserAuth(const char* guardType, const char* user, CommMsgBody& clientExtra, CommMsgBody& authBody) override;

protected:
	virtual BYTE getSrvObjType() const = 0;
	GenericServerWithLoginAuthObjectBase& loginAuthSrvObj;

private:
	INT16 authError;
};

////////////////////////////////////////////////////////////////////////////////

class TrustedAdminSrvConn : public GenericPrivilegedSrvConn
{
public:
	explicit TrustedAdminSrvConn(GenericServerObjectBase& obj_);

	void parseInfo(CommMsgParser& parser);
};

////////////////////////////////////////////////////////////////////////////////

class GenericClientGConnection : public CommClientGConnection
{
public:
	GenericClientGConnection(GenericServerObjectBase& obj_, CommClientGuardFactory& guardFactory, const char* name_)
		: CommClientGConnection(guardFactory)
		, obj(obj_)
		, name(name_)
		, isconnected(0)
	{ }

	const char* getName() const { return name; }

	virtual void connected() override;
	virtual bool closedOrDisconnected(int errCode, const char* errMsg) override;
	virtual UINT32 post(UINT32 msgId, CommMsgBody& body, AsyncCall* call) override;
	virtual void processGMessage(UINT32 reqId, UINT32 msgId, const CommMsgBody& body, AsyncCall* call) override;

	int isConnected() const { return isconnected; }

protected:
	GenericServerObjectBase& obj;
	PString name;
	int isconnected;

	UINT32 getConnId() const;
};

////////////////////////////////////////////////////////////////////////////////

class GenericServerConnFactory : public CommServerConnectionFactory
{
public:
	GenericServerConnFactory(GenericServerObjectBase& obj_, CommServerGuardFactory& guardFactory, ServerConnectionType connectionType_)
		: CommServerConnectionFactory(guardFactory), obj(obj_), connectionType(connectionType_)
	{ }

	/* virtual */ /* new */ CommServerConnection* createConnection() override
	{
		return new GenericSrvConn(obj, connectionType);
	}

public:
	GenericServerObjectBase & obj;

private:
	ServerConnectionType connectionType;
};

////////////////////////////////////////////////////////////////////////////////

class GenericAdminSrvConnBaseFactory : public CommServerConnectionFactory
{
public:
	GenericAdminSrvConnBaseFactory(GenericServerWithLoginAuthObjectBase& obj_, CommServerGuardFactory& guardFactory)
		: CommServerConnectionFactory(guardFactory), obj(obj_)
	{
		++normalPriority;
		++authPriority;
	}
	virtual ~GenericAdminSrvConnBaseFactory() { }

	// virtual CommServerConnection* createConnection() = 0;
	///* new */ CommServerConnection* createConnection() override
	//{
	//	return new AdminSrvConBase(obj);
	//}

public:
	GenericServerWithLoginAuthObjectBase& obj;
};

////////////////////////////////////////////////////////////////////////////////

class GenericAdminDbmServerConnBaseFactory : public CommServerConnectionFactory
{
public:
	GenericAdminDbmServerConnBaseFactory(GenericDbmServerWithLoginAuthObjectBase& obj_, CommServerGuardFactory& guardFactory)
		: CommServerConnectionFactory(guardFactory), obj(obj_)
	{
		++normalPriority;
		++authPriority;
	}
	virtual ~GenericAdminDbmServerConnBaseFactory() { }

	// virtual CommServerConnection* createConnection() = 0;
	///* new */ CommServerConnection* createConnection() override
	//{
	//	return new AdminSrvConBase(obj);
	//}

public:
	GenericDbmServerWithLoginAuthObjectBase& obj;
};

////////////////////////////////////////////////////////////////////////////////

class GenericTrustedAdminSrvConnFactory : public CommServerConnectionFactory
{
public:
	GenericTrustedAdminSrvConnFactory(GenericServerObjectBase& obj_, CommServerGuardFactory& guardFactory)
		: CommServerConnectionFactory(guardFactory), obj(obj_)
	{
		++normalPriority;
		++authPriority;
	}

	/* new */ CommServerConnection* createConnection() override
	{
		return new TrustedAdminSrvConn(obj);
	}

public:
	GenericServerObjectBase & obj;
};

////////////////////////////////////////////////////////////////////////////////

#endif // GenericConnections_h_included
