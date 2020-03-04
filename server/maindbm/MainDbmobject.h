#pragma once

#include "Connection.h"

#include "ppstring.h"
#include "commlib.h"
#include "commsrv.h"
#include "commgrdnull.h"
#include "counters.h"
#include "GenericAsyncCall.h"
#include "dbmserverobjectcommon.h"
#include "MainDbmmanager.h"
#include "MainCacheTypes.h"
#include "MainDbmshared.h"

// TIMER messages
#define SERVER_LOGGER_TIMER				1
#define SERVER_CLEANUP_TIMER			2

// timer wait time
#define TIMER_LOGGER_MSEC					5000
#define TIMER_CLEANUP_MSEC					10000

// server config files
#define SERVER_CONFIG_FILE "MainDbm.ini"

using AdminAuthAsyncCall = NoReplyAsyncCall<MainDbmServerObject, UINT32 /* connId */>;

//////////////////////////////////////////////////////////////////////////////////
class MainDbmServerObject : public DbmServerObjectCommon
{
private:
	typedef DbmServerObjectCommon		Parent;

	static CommServerNullGuardFactory					guardFactory;
	static CommServerPlainTextPasswordSidGuardFactory	authGuardFactory;
	static CommClientNullGuardFactory					clientNullGuardFactory;

	static const UINT32 fileVer = 1;

	CommServerConnectionPool		srvPool;
	CommClientConnectionPool		cliPool;
	MainDbmManager				dbManager;

	// Incoming connections
	AdminServerConnFactory			connAdminFactory;
	TrustedAdminServerConnFactory	connTrustedAdminFactory;
	GenericServerConnFactory		connLobbySrvFactory;

	// Outgoing connections
	GenericOutgoingConnection		loginConnection;

	
	PString							fullFileName;
	PString							initDir;

	TicksAccumulator				ticksAccumulator;

	// Logger
	bool							useLogger;
	HeLogger*						heLogger;
	PString							loggerStationName;
	UINT32							heLoggerTimerHandle;
	UINT32							cleanupTimerHandle;

	// Server config settings
	bool							moreTrace;
	PString							dumpFile;

	// server cache

public:
	MainDbmServerObject( _CommInterface& inter );
	~MainDbmServerObject();

	// server start and shutdown
	void init(CommMsgBody& initMsg) override;
	void dynamicInit();
	bool registerAllFactories() override;
	void startup();
	void shutdown() override;

	// helpers
	void replyMsg(UINT32 connId, UINT32 msgId, CommMsgBody& body);
	void postReply(GenericSrvConnection* pConn, UINT32 msgId, CommMsgBody& body);
	void setReply(CommMsgBody& reply, INT16 errCode, const char* errDesc = "");

	void processTimerMessage( UINT32 handle, UINT32 msgId, CommMsgBody& body ) override;
	void processCommonServerMessage(UINT32 msgId, const CommMsgBody& body, ServerConnectionType connType, GenericSrvConnection* conn);

	// Callbacks
	void processCallback(UINT32 reqId, UINT32 msgId, const CommMsgBody& body, CommClientGConnection::AsyncCall* pCall);
	void receiveAdminAuth(UINT32 /*replyId*/, CommMsgParser& parser, AdminAuthAsyncCall& callState);

	void processMessage( _CommMsg& msg ) override
	{
		srvPool._processMessage( msg );
		cliPool._processMessage( msg );
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
	// process server messages
	void processAdminMessage(UINT32 msgId, const CommMsgBody& body, GenericSrvConnection* conn);
	void processTrustedAdminMessage(UINT32 msgId, const CommMsgBody& body, GenericSrvConnection* conn);
	void processLobbyMessage(UINT32 msgId, const CommMsgBody & body, GenericSrvConnection* conn);

	// Handlers for Admin messages
	bool checkAdminRight(const char* right, const GenericPrivilegedSrvConnection* pConn, CommMsgBody& reply);
	void reconnect(GenericOutgoingConnection& conn, const char* const serverName, const char* const serverObject, const char* const serverConnectionType);
	void processKillServer(const char* adminId);
	void processRereadSettings(const char* adminId, CommMsgBody& reply);

	void startHeLogger(const char * strServerAddress, const char * strStationName);
	void stopHeLogger();
	void reportPerformance();
	
	// process messages
	void processInsertUser(CommMsgParser& parser, GenericSrvConnection* conn);
	void processGetUsers(CommMsgParser& parser, GenericSrvConnection* conn);
};

class MainDbmServerObjectFactory : public CommServerObjectFactory
{
public:
	/* new */ CommServerObject* createServerObject( _CommInterface& inter ) const override
	{
		return new MainDbmServerObject( inter );
	}
};

