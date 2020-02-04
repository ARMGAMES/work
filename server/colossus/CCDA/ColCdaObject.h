#pragma once

#include "Connection.h"
#include "commgrdnull.h"
#include "commgrdppwdsid.h"
#include "counters.h"
#include "GenericAsyncCall.h"
#include "ColCdaCacheTypes.h"
#include "ColCdashared.h"

////////////////////////////////////////////////////////////////////////////////

// TIMER messages
#define SERVER_LOGGER_TIMER				1
#define SERVER_CLEANUP_TIMER			2

// timer wait time
#define TIMER_LOGGER_MSEC					5000
#define TIMER_CLEANUP_MSEC					10000

// server config files
#define SERVER_CONFIG_FILE "ColCda.ini"

using AdminAuthAsyncCall = NoReplyAsyncCall<ColCdaServerObject, UINT32 /* connId */>;

//////////////////////////////////////////////////////////////////////////////////
class ColCdaServerObject : public CommServerObject
{
private:
	static CommServerNullGuardFactory guardFactory;
	static CommServerPlainTextPasswordSidGuardFactory authGuardFactory;
	static CommClientNullGuardFactory clientNullGuardFactory;
	

	CommServerConnectionPool srvPool;
	CommClientConnectionPool cliPool;

	static const UINT32 fileVer = 1;

	// Incoming connections
	AdminServerConnFactory			connAdminFactory;
	TrustedAdminServerConnFactory	connTrustedAdminFactory;
	GenericServerConnFactory		connExampleSrvFactory;

	// Outgoing connections
	GenericOutgoingConnection		loginConnection;

	PString							fullFileName;
	PString							initDir;

	TicksAccumulator ticksAccumulator;

	// Logger
	bool							useLogger;
	HeLogger*						heLogger;
	PString							loggerStationName;
	UINT32							heLoggerTimerHandle;
	UINT32							cleanupTimerHandle;

	// Server config settings
	bool							moreTrace;
	PString							dumpFile;


public:
	explicit ColCdaServerObject(_CommInterface& inter);
	~ColCdaServerObject();

	// server start and shutdown
	void init(CommMsgBody& initMsg) override;
	void dynamicInit();
	bool registerAllFactories();
	void startup();
	void shutdown() override;

	// Helpers
	void replyMsg(UINT32 connId, UINT32 msgId, CommMsgBody& body);
	void setReply(CommMsgBody& reply, INT16 errCode, const char* errDesc = "");
	void postReply(GenericSrvConnection* conn, UINT32 msgId, CommMsgBody& body);

	void processTimerMessage(UINT32 handle, UINT32 msgId, CommMsgBody& body) override;
	void processCommonServerMessage(UINT32 msgId, const CommMsgBody& body, ServerConnectionType connType, GenericSrvConnection* conn);

	// Callbacks
	void processCallback(UINT32 reqId, UINT32 msgId, const CommMsgBody& body, CommClientGConnection::AsyncCall* pCall);
	void receiveAdminAuth(UINT32 /*replyId*/, CommMsgParser& parser, AdminAuthAsyncCall& callState);

	void processMessage(_CommMsg& msg) override
	{
		srvPool._processMessage(msg);
		cliPool._processMessage(msg);	
	}

	CommServerConnection* findConnection(UINT32 connId)
	{
		return srvPool.findConnection(connId);
	}	

	UINT32 postToLogin(UINT32 msgId, CommMsgBody& body, CommClientGConnection::AsyncCall* pCall = 0)
	{
		PLog(">%s %u", loginConnection.getName(), msgId);
		return loginConnection.post(msgId, body, pCall);
	}

private:
	// Process server messages
	void processAdminMessage(UINT32 msgId, const CommMsgBody & body, GenericSrvConnection* conn);
	void processTrustedAdminMessage(UINT32 msgId, const CommMsgBody& body, GenericSrvConnection* conn);
	void processExampleSrvMessage(UINT32 msgId, const CommMsgBody & body, GenericSrvConnection* conn);
	
	// Handlers for Admin messages
	bool checkAdminRight(const char* right, const GenericPrivilegedSrvConnection* pConn, CommMsgBody& reply);
	void reconnect(CommClientGConnection& conn, const char* const serverName, const char* const serverObject, const char* const serverConnectionType);
	void processKillServer(const char* adminId);
	void processRereadSettings(const char* adminId, CommMsgBody& reply);

	void startHeLogger(const char* serverAddress, const char* stationName);
	void stopHeLogger();
	void reportPerformance();
};

////////////////////////////////////////////////////////////////////////////////

class ColCdaServerObjectFactory : public CommServerObjectFactory
{
public:
	CommServerObject* createServerObject(_CommInterface& inter) const override
	{
		return new ColCdaServerObject(inter);
	}
};

////////////////////////////////////////////////////////////////////////////////
