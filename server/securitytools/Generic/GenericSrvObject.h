////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef GenericSrvObject_h_included
#define GenericSrvObject_h_included

////////////////////////////////////////////////////////////////////////////////

#include "commgrdnull.h"
#include "commgrdppwdsid.h"
#include "GenericConnections.h"
#include "counters.h"
#include "GenericAsyncCall.h"

struct MsgAuthRedirectorToGateway;

static constexpr UINT32 LOGGER_TIMER = 100;
static constexpr UINT32 TIMER_LOGGER_MSEC = 5000;

////////////////////////////////////////////////////////////////////////////////

class GenericServerObjectBase : public CommServerObject
{
protected:
	static CommServerNullGuardFactory guardFactory;
	static CommServerPlainTextPasswordSidGuardFactory authGuardFactory;
	static CommClientNullGuardFactory clientNullGuardFactory;

	CommServerConnectionPool srvPool;
	CommClientConnectionPool cliPool;

	GenericTrustedAdminSrvConnFactory connTrustedAdminFactory;

	PString fullFileName;
	PString initDir;

	TicksAccumulator ticksAccumulator;

	bool useLogger;
	PString loggerServerAddress;
	PString loggerStationName;

	HeLogger* heLogger;
	UINT32 heLoggerTimerHandle;

public:

	explicit GenericServerObjectBase(_CommInterface& inter_);
	virtual ~GenericServerObjectBase();

	virtual void init(CommMsgBody& initMsg) override;
	virtual void dynamicInit();
	virtual void reconnect(CommClientGConnection& conn, const char* const serverName, const char* const serverObject, const char* const serverConnectionType);
	virtual void shutdown() override;
	

	virtual void processMessage(_CommMsg& msg) override;
	virtual void processCommonServerMessage(UINT32 msgId, const CommMsgBody& body, ServerConnectionType srvConnType, GenericSrvConn* conn);
	virtual void processTimerMessage(UINT32 handle, UINT32 msgId, CommMsgBody& body) override;
	virtual void processTrustedAdminMessage(UINT32 msgId, const CommMsgBody& body, CommServerConnection* conn);

	virtual void startHeLogger(const char* strServerAddress, const char* strStationName);
	virtual void stopHeLogger();
	virtual void reportPerformance();

	// Handlers for Admin messages
	virtual void processRefreshSettings(CommMsgBody& reply);
	virtual void processKillServer(const char* adminId);

	// Callbacks
	virtual void processCallback(UINT32 reqId, UINT32 replyId, const CommMsgBody& body, CommClientGConnection::AsyncCall* pCall);

	virtual CommServerConnection* findConnection(UINT32 connId);
	virtual void addMessageTicks(const HiresCounter& counter);

	// Utilities
	void postReply(CommServerConnection* pConn, UINT32 msgId, CommMsgBody& body);
	void postReply(UINT32 connId, UINT32 msgId, CommMsgBody& body);
	void setReply(CommMsgBody& reply, INT16 errCode, const char* errDesc);
	bool checkAdminRight(const char* right, const GenericPrivilegedSrvConn* pConn, CommMsgBody& reply);

protected:
	// "Must" overrides
	virtual const char* getConfigFileName() const = 0;

	// Override as required.
	virtual bool isConnTypeSupported(int connType) const;

	// "Life cycle events". Override as required
	virtual void startup() {}
	virtual void doInitRegisterFactories() { }
	virtual void doPostInit() { }
	virtual void doPreDynamicInit(const PIniFile& /*ini*/) { }
	virtual void doDynamicInitReconnect(const PIniFile::Section& /*section*/) { }
	virtual void doDynamicInitReadSettings(const PIniFile::Section& /*section*/) { }
	virtual void doDynamicInitLogSettings() const { }
	virtual void doPostDynamicInit(const PIniFile& /*iniFile*/) { }
	virtual void doPreShutdown() { }
	virtual void doPostShutdown() { }
	virtual void doProcessCommonServerMessage(UINT32 msgId, const CommMsgBody& body, ServerConnectionType srvConnType, GenericSrvConn* conn) { }
	virtual void doProcessTimerMessage(UINT32 handle, UINT32 msgId, CommMsgBody& body, bool& wasHandled);
	virtual void doProcessTrustedAdminMessage(UINT32 reqId, UINT32 msgId, MsgAuthRedirectorToGateway& msg, TrustedAdminSrvConn* adminConn, bool& wasHandled);
	virtual void doReportPerformanceToFile() const { }
	virtual void doComposePerformanceStats(CommMsgBody& /*reportBody*/) { }
};

////////////////////////////////////////////////////////////////////////////////

using AdminAuthAsyncCall = NoReplyAsyncCall<GenericServerWithLoginAuthObjectBase, UINT32 /* connId */>;

class GenericServerWithLoginAuthObjectBase : public GenericServerObjectBase
{
	using Parent_ = GenericServerObjectBase;

protected:
	unique_ptr<GenericAdminSrvConnBaseFactory> connAdminFactory;

	GenericClientGConnection loginAuthConn;

public:

	explicit GenericServerWithLoginAuthObjectBase(_CommInterface& inter_);
	virtual ~GenericServerWithLoginAuthObjectBase() { }

	virtual void processAdminMessage(UINT32 msgId, const CommMsgBody& body, CommServerConnection* conn);

	// Callbacks
	virtual void receiveAdminAuth(UINT32 /*replyId*/, CommMsgParser& parser, AdminAuthAsyncCall& callState);

	virtual UINT32 postToLoginAuth(UINT32 msgId, CommMsgBody& body, CommClientGConnection::AsyncCall* pCall = nullptr);

protected:
	// "Must" overrides
	virtual const char* getLoginAuthDbmConnType() const = 0;

	// Life cycle events overridden from parent
	virtual void doDynamicInitReconnect(const PIniFile::Section& section) override;
	virtual void doInitRegisterFactories() override;
	virtual void doProcessCommonServerMessage(UINT32 msgId, const CommMsgBody& body, ServerConnectionType srvConnType, GenericSrvConn* conn) override;
	virtual void doComposePerformanceStats(CommMsgBody& reportBody) override;

	// "Life cycle events". Override as required
	virtual void doProcessAdminMessage(UINT32 reqId, UINT32 msgId, CommMsgParser& parser, GenericPrivilegedSrvConn* adminConn) { }
};

////////////////////////////////////////////////////////////////////////////////

#endif // GenericSrvObject_h_included
