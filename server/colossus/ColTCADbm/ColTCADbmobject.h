
// ===========================================================
// Copyright (C) 2018 PYR Software Ltd. All rights reserved.
// ===========================================================

#ifndef ColTCADbmobject_h_included
#define ColTCADbmobject_h_included

#include "Connection.h"

#include "ppstring.h"
#include "commlib.h"
#include "commsrv.h"
#include "commgrdnull.h"
#include "counters.h"
#include "dbmserverobjectcommon.h"
#include "ColTCADbmmanager.h"
#include "ColTCACacheTypes.h"
#include "TCADetector.h"

// TIMER messages
#define TCA_COL_TIMER_LOGGER				1
#define TCA_COL_TIMER_CHECK_TASK			2

#define TIMER_LOGGER_MSEC					5000
#define TIMER_CHECK_TASK_MSEC					60*1000

//////////////////////////////////////////////////////////////////////////////////
class ColTCADbmServerObject : public DbmServerObjectCommon
{
	typedef DbmServerObjectCommon		Parent;

	static CommServerNullGuardFactory					guardFactory;
	static CommServerPlainTextPasswordSidGuardFactory	authGuardFactory;
	static CommClientNullGuardFactory					clientNullGuardFactory;

	CommServerConnectionPool			srvPool;
	CommClientConnectionPool			cliPool;

	// Incoming connections
	ColTCADbmAdminServerConnFactory			connAdminFactory;
	ColTCADbmTrustedAdminServerConnFactory	connTrustedAdminFactory;
	ColTCADbmServerConnFactory				connTCADetectorFactory;

	// Outgoing connections
	ColTCADbmOutgoingConnection				loginConnection;
	ColTCADbmOutgoingConnection				colRawConnection;
	ColTCADbmOutgoingConnection				colossusReadConnection;  // DBM

	PString fullFileName;
	PString initDir;

	ColTCADbmManager					dbManager;

	// Logger
	bool								useLogger;
	HeLogger*							heLogger;
	UINT32								heLoggerTimerHandle;
	PString								loggerStationName;

	UINT32								checkTaskTimerHandle;

	// Server config settings
	bool								moreTrace;

	// server cache
	TCAColTaskCache						tcaColTaskCache;

public:
	TicksAccumulator					ticksAccumulator;

public:

	ColTCADbmServerObject( _CommInterface& inter );
	~ColTCADbmServerObject();


	void init(CommMsgBody& initMsg) override;
	void dynamicInit();
	bool registerAllFactories() override; // PYR-27418 - need to postpone registering factories till ID ranges are received
	void shutdown() override;

	void replyMsg(UINT32 connId, UINT32 msgId, CommMsgBody& body);

	void processTimerMessage( UINT32 handle, UINT32 msgId, CommMsgBody& body ) override;
	void processColTCADbmServerMessage( UINT32 msgId, const CommMsgBody& body, ColTCADbmConnection* conn );

	// Handlers for TCADetector messages
	void processGetTCAUserTournData(CommMsgParser& parser, CommServerConnection* conn);
	void gotColRawUserTournData(CommMsgParser& parser, UINT32 taskId);
	
	void processGetTCAPairsRaw(UINT32 taskId, TCAColTaskData& tcaColTaskData);
	void gotColRawPairsData(CommMsgParser& parser, UINT32 taskId);

	void processGetUserStartLetters(UINT32 taskId, TCAColTaskData& tcaColTaskData);
	void gotUserStartLetters(CommMsgParser& parser, UINT32 taskId);
	
	
	void processColTournPair(UINT32 taskId, TCAColTaskData& tcaColTaskData, char uidStartLetter);

	void appendTaskBreakDownDetails(const char* module, UINT32 pairNum, PString& taskDetails);

	void checkRunningTasks();

	// Callbacks
	void processCallback(UINT32 reqId, UINT32 msgId, const CommMsgBody& body, CommClientGConnection::AsyncCall* pCall);
	void receiveAdminAuth(const CommMsgBody& body, ColTCADbmAdminServerConnection* pConn);

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
		return loginConnection.post(msgId, body, pCall);
	}

	UINT32 postToColRaw(UINT32 msgId, CommMsgBody& body, CommClientGConnection::AsyncCall* pCall = 0)
	{
		return colRawConnection.post(msgId, body, pCall);
	}

	UINT32 postToColossusRead(UINT32 msgId, CommMsgBody& body, CommClientGConnection::AsyncCall* pCall = 0)
	{
		return colossusReadConnection.post(msgId, body, pCall);
	}

private:
	void processAdminMessage(UINT32 msgId, const CommMsgBody& body, ColTCADbmConnection* conn);
	void processTrustedAdminMessage(UINT32 msgId, const CommMsgBody& body, ColTCADbmConnection* conn);
	void processTCAServerMessage(UINT32 msgId, const CommMsgBody & body, ColTCADbmConnection* conn);

	// Handlers for Admin messages
	bool checkAdminRight(const char* right, const ColTCADbmPrivilegedConnection* pConn, CommMsgBody& reply);
	void reconnect(ColTCADbmOutgoingConnection& conn, const char* const serverName, const char* const serverObject, const char* const serverConnectionType);
	void processKillServer(const char* adminId);
	void processRereadSettings(const char* adminId, CommMsgBody& reply);

	void postReply(ColTCADbmConnection* pConn, UINT32 msgId, CommMsgBody& body);
	void setReply(CommMsgBody& reply, INT16 errCode, const char* errDesc = "");

	void startHeLogger(const char * strServerAddress, const char * strStationName);
	void stopHeLogger();
	void reportPerformance();

public:

	void processClientMessage(UINT32 msgId, const CommMsgBody& body );
	void processClientGMessage(UINT32 requestId, UINT32 msgId, const CommMsgBody& body );
	void processClientAuthMessage(UINT32 requestId, UINT32 msgId, const CommMsgBody& body );
	//void receiveAuthFromDbm( CommMsgParser& parser, UINT32 connId );

};

class ColTCADbmServerObjectFactory : public CommServerObjectFactory
{
public:
	/* new */ CommServerObject* createServerObject( _CommInterface& inter ) const override
	{
		return new ColTCADbmServerObject( inter );
	}
};


#endif	