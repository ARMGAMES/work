#ifndef commredundant_h_included
#define commredundant_h_included

#include "commsrvcomposite.h"
#include "commgrdnull.h"
#include "ppinifile.h"

// clientPhysConnectionId - unique identifier of physical connection on client side (one per object)
//							Must be changed every time when new phys connection created

// serverPhysConnectionId - unique identifier of physical connection on server side (one per object)
//							Must be changed every time when new phys connection created

// clientLogicalConnectionId - unique identifier of client logical connection.
// serverLogicalConnectionId - unique identifier of server logical connection.


#define COMMMCOMPOSITE_INIT_MSG			1   // c->s, init logical connection 44 clientPhysConnectionId, clientLogicalConnectionId
#define COMMMCOMPOSITE_INIT_REPLY_MSG	2   // s->c, confirmation on init logical connection
#define COMMMCOMPOSITE_CLOSE_MSG		3   // s->c, c->s, close phys connection, last message 
#define COMMMCOMPOSITE_DATA_MSG			4   // s->c, c->s, data message
#define COMMMCOMPOSITE_DATA_ACK_MSG		5   // s->c, c->s, ack data message
#define COMMMCOMPOSITE_ERROR_MSG		6   // s->c, c->s, error message


#define COMMCOMPOSITE_ERROR_WRONG_SERVER_PHYSCONNECTION 1
#define COMMCOMPOSITE_ERROR_WRONG_CLIENT_PHYSCONNECTION 2


#define REDUNDANT_CONNECT_TIMEOUT 10000

class CommRedundantConnectionsObject;
struct _RedundantItem
{
	UINT32 orderNum;
	UINT32 connectionMask;
	UINT32 ticksSent;
	UINT32 ticksCreated;
	CommMsgBody body;
	_RedundantItem(){connectionMask=0;}
	_RedundantItem(const _RedundantItem &other){connectionMask=0;}

};

struct _RedundantIncomingItem
{
	UINT32 orderNum;
	CommMsgBody body;
	_RedundantIncomingItem(){}
	_RedundantIncomingItem(const _RedundantIncomingItem &other){}

};

typedef list <_RedundantItem > OutgoingQueueList;
typedef OutgoingQueueList::iterator OutQueueListIterator;
//////////////////////////////////////////////////////////////////////////////////

class CommRedundantServerConnection : public CommServerConnection
{
protected:
	CommRedundantConnectionsObject* obj;
public:
	UINT32 srvId;
	UINT32 clnId;
	INT32  clnNum;
	UINT32 lastSent;
	UINT32 lastReceived;
	UINT32 lastAck;
	UINT32 outstandingNumber;
	UINT32 outstandingSize;
	bool lastIteratorValid;
	OutQueueListIterator itr;


	CommRedundantServerConnection( CommRedundantConnectionsObject* obj_ );
	void processMessage( UINT32 msgId, const CommMsgBody& body ) override;
	void connected() override;
	void closedOrDisconnected( int errCode, const char* errMsg ) override;
	void postCloseConnection();
	void postRedundant(const CommMsgBody& body);

};


class CommRedundantServerConnFactory : public CommServerConnectionFactory
{
public:
	CommRedundantConnectionsObject* obj;

public:

	CommRedundantServerConnFactory( CommServerGuardFactory& guardFactory )
		: CommServerConnectionFactory( guardFactory )
	{
		obj = 0;
		authPriority +=3;
		normalPriority +=3;
	}

public:
	/* new */ CommServerConnection* createConnection() override
	{
		return new CommRedundantServerConnection( obj );
	}
};


class CommRedundantClientConnection : public CommClientConnection
{
private:
	CommRedundantConnectionsObject* obj;

public:

	PString serverAddress;
	PString serverObject;
	bool established;
	INT32  clnNum;
	UINT32 srvId;
	UINT32 clnId;
	UINT32 lastSent;
	UINT32 lastReceived;
	UINT32 lastAck;
	UINT32 outstandingNumber;
	UINT32 outstandingSize;
	bool lastIteratorValid;
	OutQueueListIterator itr;



	CommRedundantClientConnection( CommRedundantConnectionsObject& obj_, int num_, CommClientGuardFactory& guardFactory_, const char * serverAddress_, const char * serverObject_ );
	void reinit();
	void postCloseConnection();
	void postRedundant(const CommMsgBody& body);

	void processMessage(UINT32 msgId, const CommMsgBody& body) override;
	void connected() override;
	bool closedOrDisconnected( int errCode, const char* errMsg ) override;
};


//////////////////////////////////////////////////////////////////////////////////
class CommRedundantConnectionsObject : public CommCompositeServerObject
{
private:
	static CommServerNullGuardFactory guardFactory;
	static CommClientNullGuardFactory guardFactoryC;
	CommServerConnectionPool srvPool;
	CommClientConnectionPool cliPool;





	typedef list <_RedundantIncomingItem > IncomingPhysList;
	OutgoingQueueList outQueue;
	IncomingPhysList physQueue;
	typedef IncomingPhysList::iterator InPhysQueueListIterator;


	UINT32 uniqueId;


	UINT32 currentMsgSend;
	UINT32 currentPhysConnMsg;
	UINT32 currentMsgReceived;
	UINT32 queueSize;

	typedef vector <CommRedundantClientConnection *> _ClientConnections;

	_ClientConnections	clientConnections;

	CommRedundantServerConnFactory	serverConnectionFactory;


	PString serverObject;


	bool outgoingConnection;
	bool physConnectionExist;
	bool physConnectionEstablished;
	UINT32 timeOut;
	UINT32 connectTimeOut;
	UINT32 printStatTime;

	UINT32 stat_maxRoundTrip;
	UINT32 stat_maxLive;
	UINT32 stat_averageRoundTrip;
	UINT32 stat_averageLive;
	UINT32 stat_numMessages;
	UINT32 stat_distribution[10];


public:
	enum TypeOfFlow 
	{
		neverDropRedundancy = 0, // normal type of message flow:
		dropRedundancy = 1,
		nonRedundant = 2
	};
	enum TypeOfFlow flowType;
	UINT32 uniqueClientPhysConnId;
	UINT32 uniqueServerPhysConnId;
	UINT32 perConnectionThreshold; // allowed total size of outstanding unconfirmed messages per connection. 
	// If exceeded stop sending messages to connection. 0 - not used
	UINT32 globalThreshold;	       // allowed size of outstanding queue. 
	// If exceeded drop phys connection. 0 - not used

	UINT32 totalSize; // size of all messages in uncofirmed queue;

	CommRedundantConnectionsObject( _CommCompositeInterface& inter ) : 
	CommCompositeServerObject( inter ),
		srvPool( inter ),
		cliPool( inter ),
		serverConnectionFactory(guardFactory)
	{
		serverConnectionFactory.obj = this;
		outgoingConnection = false;
		physConnectionExist = false;
		physConnectionEstablished = false;
		uniqueId = RANDOMTIME();
		uniqueClientPhysConnId=0;
		uniqueServerPhysConnId=0;
		currentMsgSend=0;
		currentPhysConnMsg=0;
		currentMsgReceived=0;
		timeOut = 0;
		totalSize = 0;
		queueSize = 0;
		flowType = neverDropRedundancy;
		perConnectionThreshold = 0;
		globalThreshold = 0;
		connectTimeOut = REDUNDANT_CONNECT_TIMEOUT;
		printStatTime = 0;
		stat_maxRoundTrip = 0;
		stat_maxLive = 0;
		stat_averageRoundTrip = 0;
		stat_averageLive = 0;
		stat_numMessages = 0;
		memset(stat_distribution,0,sizeof(stat_distribution));

	}

	~CommRedundantConnectionsObject() 
	{
		for(int i =0 ;i<clientConnections.size();i++)
		{
			delete clientConnections[i];
		}
	}

	UINT32 nextUniqueId()
	{
		if( ++uniqueId == 0 )
			uniqueId = 1;
		return uniqueId;
	}

	void reinitObject(CommRedundantServerConnection * conn = 0);
	void closePhysicalConnection(CommRedundantServerConnection * excludedServerConnection = 0);

	void init(CommMsgBody& initMsg) override;
	void processTimerMessage( UINT32 handle, UINT32 msgId, CommMsgBody& body ) override;
	void processMessage( _CommMsg& msg ) override
	{
		srvPool._processMessage( msg );
		cliPool._processMessage( msg );
	}
	UINT32 getInternalQueueSize() override
	{
		return queueSize;
	}

	void processServerMessage( UINT32 msgId, const CommMsgBody& body, CommRedundantServerConnection* conn );
	void processClientMessage( UINT32 msgId, const CommMsgBody& body, CommRedundantClientConnection* conn );

	void processOutgoingPhysConnection (const PString & address) override;
	void processPhysDisconnect() override;
	void processPhysMessage(_CommMsg & msg) override;

	bool isAnyClientConnectionsExist();
	bool isAnyServerConnectionsExist();

	void moveIterator(OutQueueListIterator it);
	void setNewIterator(OutQueueListIterator it);
	void connectedClient(CommRedundantClientConnection * conn);
	void disconnectedClient(CommRedundantClientConnection * conn);
	void connectedServer(CommRedundantServerConnection* conn);
	void disconnectedServer(CommRedundantServerConnection* conn);
	void postServerError(CommRedundantServerConnection *conn, UINT32 physId, UINT32 cliId, UINT32 errCode);
	void initNewServerConnection(CommMsgParser & parser,CommRedundantServerConnection * conn);
	void replyOnInitClientConnection(CommMsgParser & parser,CommRedundantClientConnection * conn);
	bool clearQueue(UINT32 counter);
	void postQueue(CommRedundantServerConnection * conn, bool upTreshold);
	void postQueue(CommRedundantClientConnection * conn, bool upTreshold);
	void checkPhysQueue();
	void printStatistic();
	void calculateStatistic(const struct _RedundantItem &item);
	OutQueueListIterator cleanMask(UINT32 clnNum);
};

class CommRedundantConnectionsObjectFactory : public CommCompositeServerObjectFactory
{
public:
	/* new */ CommCompositeServerObject* createServerObject( _CommCompositeInterface& inter ) const override
	{
		return new CommRedundantConnectionsObject( inter );
	}
};

#endif
