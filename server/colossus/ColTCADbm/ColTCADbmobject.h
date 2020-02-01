#pragma once

#include "Connection.h"

#include "ppstring.h"
#include "commlib.h"
#include "commsrv.h"
#include "commgrdnull.h"
#include "counters.h"
#include "GenericAsyncCall.h"
#include "dbmserverobjectcommon.h"
#include "ColTCADbmmanager.h"
#include "ColTCACacheTypes.h"
#include "ColTCADbmshared.h"

// TIMER messages
#define SERVER_LOGGER_TIMER				1
#define SERVER_CLEANUP_TIMER			2

// timer wait time
#define TIMER_LOGGER_MSEC					5000
#define TIMER_CLEANUP_MSEC					10000

// server config files
#define SERVER_CONFIG_FILE "ColTCADbm.ini"

using AdminAuthAsyncCall = NoReplyAsyncCall<ColTCADbmServerObject, UINT32 /* connId */>;

//////////////////////////////////////////////////////////////////////////////////
class ColTCADbmServerObject : public DbmServerObjectCommon
{
private:
	static const UINT32 fileVer = 1;

	typedef DbmServerObjectCommon		Parent;

	static CommServerNullGuardFactory					guardFactory;
	static CommServerPlainTextPasswordSidGuardFactory	authGuardFactory;
	static CommClientNullGuardFactory					clientNullGuardFactory;

	CommServerConnectionPool			srvPool;
	CommClientConnectionPool			cliPool;

	// Incoming connections
	AdminServerConnFactory				connAdminFactory;
	TrustedAdminServerConnFactory		connTrustedAdminFactory;
	GenericServerConnFactory			connExampleSrvFactory;

	// Outgoing connections
	GenericOutgoingConnection			loginConnection;

	
	PString fullFileName;
	PString initDir;

	ColTCADbmManager					dbManager;

	// Logger
	bool								useLogger;
	HeLogger*							heLogger;
	PString								loggerStationName;
	UINT32								heLoggerTimerHandle;
	UINT32								cleanupTimerHandle;

	// Server config settings
	bool								moreTrace;
	PString								dumpFile;

	// server cache

public:
	ColTCADbmServerObject( _CommInterface& inter );
	~ColTCADbmServerObject();

	void init(CommMsgBody& initMsg) override;
	void dynamicInit();
	bool registerAllFactories() override; // PYR-27418 - need to postpone registering factories till ID ranges are received
	void startup();
	void shutdown() override;

	void replyMsg(UINT32 connId, UINT32 msgId, CommMsgBody& body);

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
	void processAdminMessage(UINT32 msgId, const CommMsgBody& body, GenericSrvConnection* conn);
	void processTrustedAdminMessage(UINT32 msgId, const CommMsgBody& body, GenericSrvConnection* conn);
	void processExampleSrvMessage(UINT32 msgId, const CommMsgBody & body, GenericSrvConnection* conn);

	// Handlers for Admin messages
	bool checkAdminRight(const char* right, const GenericPrivilegedSrvConnection* pConn, CommMsgBody& reply);
	void reconnect(GenericOutgoingConnection& conn, const char* const serverName, const char* const serverObject, const char* const serverConnectionType);
	void processKillServer(const char* adminId);
	void processRereadSettings(const char* adminId, CommMsgBody& reply);

	void postReply(GenericSrvConnection* pConn, UINT32 msgId, CommMsgBody& body);
	void setReply(CommMsgBody& reply, INT16 errCode, const char* errDesc = "");

	void startHeLogger(const char * strServerAddress, const char * strStationName);
	void stopHeLogger();
	void reportPerformance();

public:

	TicksAccumulator					ticksAccumulator;
};

class ColTCADbmServerObjectFactory : public CommServerObjectFactory
{
public:
	/* new */ CommServerObject* createServerObject( _CommInterface& inter ) const override
	{
		return new ColTCADbmServerObject( inter );
	}
};

