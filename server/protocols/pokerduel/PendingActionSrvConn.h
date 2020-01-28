////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2012 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#ifndef PendingActionSrvConn_h_included
#define PendingActionSrvConn_h_included

#include "commsrv.h"
#include "commgrdnull.h"
#include "ppinclude.h"
#include "baseconnection.h"
#include "pokerduel/duelnotifserver.h"
#include "conndefines.h"

////////////////////////////////////////////////////////////////////////////////

/*
To add pending action support in your module, do the following steps:

1. #include "pokerduel/PendingActionSrvConn.h" in your object.h file

2. Add PendingActionSrvConn.cpp to your project file. Don't forget to change .files for the build.

3. Inherit PendingActionServerObjectInter by your object:	
	class XXXServerObject : public CommServerObject, public PendingActionServerObjectInter

4. Add to the object constructor initializing list:
	PendingActionServerObjectInter(nullGuardFactoryServer)

5. Add to the constructor:	
	initPendingActionServerObjectInter(srvPool);

6. Implement virtual function:
	INT16 getPendingActions(const char* userId, UINT32 siteId, vector<DuelPendingAction>& actions, PString& errMsg) override;

7. call the following function to inform DuelNotifServer about change of the pending action:
	void notifPendingActionChange(const char* userId, UINT32 siteIdMask, bool addNotDrop, const DuelPendingAction& action)
*/

class PendingActionServerObjectInter;

class DuelNotifSrvConn : public BaseIncomingConnection<PendingActionServerObjectInter>
{
public:
	DuelNotifSrvConn(PendingActionServerObjectInter* obj_, const char* traceMarker_)
		: BaseIncomingConnection(obj_, traceMarker_){}
	void processRequest(UINT32 msgId, const CommMsgBody& body) override;	

	void replyGetPendingActions(const char* userId, UINT32 siteId, UINT32 reqId);

	void replyGetPendingActionsCount(const char* userId, UINT32 siteId, UINT32 reqId);

	void closedOrDisconnected(int errCode, const char* errMsg) override;
};
class DuelNotifSrvConnFactory : public BaseIncomingConnectionFactory<PendingActionServerObjectInter>
{
public:
	DuelNotifSrvConnFactory(CommServerGuardFactory& guardFactory, const char* traceMarker_) 
		: BaseIncomingConnectionFactory(guardFactory, traceMarker_)
	{ }	

	/* new */ CommServerConnection* createConnection() override;
};

class PendingActionServerObjectInter
{
	DuelNotifSrvConnFactory srvConnPAFactory;	
public:
	PendingActionServerObjectInter(CommServerGuardFactory& guardFactory)
		: srvConnPAFactory(guardFactory, "PA")
		, duelNotifSrvConn(NULL)
	{
		srvConnPAFactory.init(this);
	}
	virtual ~PendingActionServerObjectInter()
	{ }

	void initPendingActionServerObjectInter(CommServerConnectionPool& srvPool)
	{
		srvPool.registerFactory(DUELNOTIFSERVER_CONN_TYPE, srvConnPAFactory);
		PLog("srvConnPendingActionFactory registered");
	}

	void replyPendingActionMsg(UINT32 msgId, UINT32 reqId, const char* userId, UINT32 siteId)
	{
		if (!duelNotifSrvConn)
			return;
		if (msgId == MSG_DNSERVER_SRV_GET_PENDING_ACTIONS)
		{	
			duelNotifSrvConn->replyGetPendingActions(userId, siteId, reqId);		
		}
		else // MSG_DNSERVER_SRV_GET_PENDING_ACTIONS_COUNT
		{	
			duelNotifSrvConn->replyGetPendingActionsCount(userId, siteId, reqId);
		}
	}
	
	// Override useAsyncCallToLoadData and return true to load pending action data from async-call.
	// When async-call is finished, call replyPendingActionMsg(). See friend server for an example.
	virtual bool useAsyncCallToLoadData(UINT32 msgId, UINT32 reqId, const char* userId, UINT32 siteId)
	{
		return false;
	}

	virtual INT16 getPendingActions(const char* userId, UINT32 siteId, vector<DuelPendingAction>& actions, PString& errMsg) = 0;

	void notifPendingActionChange(const char* userId, UINT32 siteIdMask, bool addNotDrop, const DuelPendingAction& action)
	{
		CommMsgBody body;
		body
			.composeUINT32(0/*reqId*/)
			.composeString(userId)
			.composeUINT32(siteIdMask)
			.composeBOOL(addNotDrop)
			.composeObject(action);
		postNotifMsg(MSG_DNSERVER_SRV_NOTIF_PENDING_ACTION, body);
	}

	void postNotifMsg(UINT32 msgId, CommMsgBody& body);
	DuelNotifSrvConn *duelNotifSrvConn;
};


////////////////////////////////////////////////////////////////////////////////

#endif // PendingActionSrvConn_h_included
