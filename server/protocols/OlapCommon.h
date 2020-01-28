#ifndef olapcommon_h_included
#define	olapcommon_h_included

#include "commsrv.h"
#include "commgrdnull.h"
#include "ppinifile.h"
#include "counters.h"
#include "HeLogger.h"
#include "pyrtime.h"

#include "dbcommon.h"
#include "logmessage.h"
#include "dbmserverobjectcommon.h" // PYR-27418

class CommServerObjectWithOlapInteface;
class OlapStruct;

typedef DatabaseManagerCommon DatabaseManagerCommonWithOlap;

class DbmToOlapClientConn : public CommClientGConnection
{
	TicksAccumulator accOlapResponse;
protected:
	CommServerObjectWithOlapInteface* obj;
	OlapStruct*		olap;
public:
	DbmToOlapClientConn( CommClientGuardFactory& guardFactory, CommServerObjectWithOlapInteface* obj_, OlapStruct* olap_ )
	: CommClientGConnection( guardFactory/*, true*/ )
	{
		obj = obj_;
		olap = olap_;
	}
	void processGMessage(  UINT32 reqId, UINT32 msgId, const CommMsgBody& body, CommClientGConnection::AsyncCall* call) override;
	void connected() override;
	bool closedOrDisconnected(int errCode, const char * errStr) override;
	UINT32 getOlapId() const; 
	OlapStruct* getOlap() { return olap; }
};

// PYR-39774 - obsolete
//#define FULL_OLAP_MSG_MASK (1 << 0)
#define USER_ACCOUNT_MSG_MASK (1 << 1)

// PYR-18899: This mask denotes a message to be replicated to play site only, and not to any other OLAPS
//	PYR-39062 Obsolete
//#define OLAP_REPLICATE_TO_TEST_SITE_ONLY_MSG_MASK (1<<2) 

#define ICE_MSG_MASK (1 << 3) // 8

enum eOlapSyncMode
{ 
	eSyncNormal = 1,
	eSyncUsersOnly,
	eSyncFull,
	eSyncIce,
 };


class OlapServer;

class OlapStruct
{

friend class OlapServer;
friend class OlapManager;
friend class DbmToOlapClientConn;

private:
	UINT32	olapId;
	bool directSendToOlap;
	DbmToOlapClientConn*		connToOlapDbm;
	OlapServer*					olapServer;
	PString name;
	PString olapDbmAddress;
	PString olapDbmInstance;
	bool useOlapDbmPermitted;
	bool saveMessageOnDisconnect;
	eOlapSyncMode olapSyncMode;
	UINT32 numSavedMessagesToOlap;

	size_t getQueueSize() const { return connToOlapDbm->_size(); }
	void getToOlapQueueRange(CommClientGConnection::const_iterator& begin_, CommClientGConnection::const_iterator& end_) const;
	void clearToOlapQueue();
	void setNewOlapParameters(const unique_ptr<OlapStruct>& other);
	void connectToOlap(CommClientNullGuardFactory& guardCliFactory, CommClientConnectionPool& cliPool);
	void trace() const;
	bool haveOlapDbm() const { return *olapDbmAddress && *olapDbmInstance; }
	bool useOlapDbm() const { return 0 != useOlapDbmPermitted; }
	
	void saveQueue();
    void sendSavedMessages();
	void setDirectSendToOlap(bool doSendDirectly);

public:

	OlapStruct(OlapServer*	olapServer_)
	{
		olapId = -1;
		useOlapDbmPermitted = false;
		saveMessageOnDisconnect = false;
		olapSyncMode = eSyncNormal;
		saveMessageOnError = false;
		olapServer = olapServer_;
		connToOlapDbm = 0;
		directSendToOlap = false;
		numSavedMessagesToOlap = 0;
	}
	~OlapStruct()
	{
		olapServer = 0;
		delete connToOlapDbm;
	}
	bool saveMessageOnError;

	eHost getHostId() const;
};


///////////////////////////////////////////////////////// Olap - db 

//table MESSAGES
#define MSG_BODY_LEN		16001
#define MSG_ERRDESCR		251

class SaveMessageStmt;
class GetMessageStmt;
class GetFirstMessageStmt;

// PYR-22086
enum eOlapProtocolVersion
{
	eOlapProtocolVersionLegacy = 0,
	eOlapProtocolVersionSysInfo = 1
};

class OlapManager
{
friend class OlapServer;
	int maxMessagesForSynchr;
	eOlapProtocolVersion olapProtocolVersion;

	DatabaseManagerCommon*		manager;
	SaveMessageStmt*			saveMessageStmt;
	GetMessageStmt*				getSavedOLAPMessagesStmt;
	GetFirstMessageStmt*		getFirstMessageStmt;
	PString uniqueOlapMsgIdObjectName = "UniqueOlapMsgId";

	void _zeroStatements();
	INT16 sendSavedMessagesToOlap(OlapStruct& olap, bool& toBeContinued, PString& errDescr);
	INT16 saveMessage(UINT32 msgId, const CommMsgBody& body, int fromQueue, INT16 errCode, const char* errDescr, UINT32 olapId);
	void addUniqueIdGenerator(); // PYR-71763
	UINT64 getNextUniqueMsgId(); // PYR-71763

public:

	OlapManager()
	{
		maxMessagesForSynchr = 500;
		manager = 0;
		olapProtocolVersion = eOlapProtocolVersionSysInfo;
		_zeroStatements();
	}
	~OlapManager()
	{
		deleteStatements();
	}
	void init(DatabaseManagerCommon* manager_);
	void prepareStatements( DatabaseManagerCommon& manager );
	void deleteStatements();
	void saveQueueToOlap(OlapStruct& olap);

};

class OlapServer
{
friend class OlapStruct;
protected: // PYR-32351 - used in the child class
	vector<OlapStruct*> olapVector;
private:
	DatabaseManagerCommonWithOlap* manager;
	OlapManager* olapManager;
	bool allOlapsInDirectSend;

	void addOlap(unique_ptr<OlapStruct>& new_olap);
	void readOlapFromIniFile(const PIniFile& iniFile, const char* olapSectionName);
    
public:

	CommServerObjectWithOlapInteface*	obj;

	OlapServer()
	{
		obj = 0;
		manager = 0;
		olapManager = 0;
		allOlapsInDirectSend = false;
	}
    virtual
	~OlapServer()
	{
		obj = 0;			//pclint
		manager = 0;		//pclint
		olapManager = 0;	//pclint
		for(int n = 0; n < olapVector.size(); n++)
			delete olapVector[n];
	}

	void reportQueue(const HeLogger* heLogger, CommMsgBody& reportBody);
	void reportSavedMessage(const HeLogger* heLogger, CommMsgBody& reportBody);
	void readFromIniFile(const char* iniFileName);
    // PYR-32351 - made this method virtual
	virtual	void postToOlap(UINT32 msgId, const CommMsgBody& body, bool sendAlwaysDirect, bool /*isPlayReplicaSync*/, BYTE msgMask, const OlapMsgSysInfo& msgSysInfo);
	void postToIce(UINT32 msgId, UINT32 userIntId, const CommMsgBody& body);
	UINT32 connectToOlapAll(CommClientConnectionPool& cliPool, CommClientNullGuardFactory& guardCliFactory); // move to read settings ??
	void init(CommServerObjectWithOlapInteface*	obj_, DatabaseManagerCommonWithOlap* manager_);
	void processSendSavedMessages(OlapStruct* olap, bool& toBeContinued);
	void saveMessage(UINT32 msgId, const CommMsgBody& body, int fromQueue, INT16 errCode, const char* errDescr, UINT32 olapId); // move to private??
	void saveQueueToOlapAll(); 
	OlapStruct* getOlapById(UINT32 olapId) const;
	void olapInDirectSendMode(UINT32 olapId, bool inDirectSend);
	eHost getHostId() const;
	UINT64 getNextUniqueMsgId(); // PYR-71763
};

#define PROP_LAST_MSG_ORDINAL_OLAP 	"LastMsgOrdinalOlap"
// PYR-51852 - this is the hard-coded name of the new MESSAGES table that will be common for all dbms
#define OLAPMESSAGES_TABLE_NAME	"MESSAGES"
#define OLAPMESSAGES_OBJECT_NAME "OlapMessages"

enum ProcessGenericMessageStatus
{
	processGenericMessageNormal			= 0, 
// PYR-34930 - remove worker code
//	processGenericMessageRunIntoLock	= 1, 
//	processGenericMessagePostedToWorker	= 2, 
	processGenericMessageError			= 3,
    processGenericMessageDelayed        = 4, // PYR-32351 - message was placed into "delayed" queue
	processGenericMessagePostedToThread	= 5, // PYR-32351 - message was placed into the child thread queue for immediate processing by that thread
};

// PYR-32351
inline bool isThreadedMsgStatus( int msgStatus )
{
	return msgStatus == processGenericMessageDelayed || msgStatus == processGenericMessagePostedToThread;
}

class CommServerObjectWithOlapInteface
{
public:
	virtual ~CommServerObjectWithOlapInteface(){};
	virtual void postToOlap(UINT32 msgId, const CommMsgBody& body, bool sendAlwaysDirect, bool isPlayReplicaSync, BYTE msgMask, const OlapMsgSysInfo& msgSysInfo) = 0;
	virtual void processOlapMessage( UINT32 reqId, UINT32 msgId, const CommMsgBody& body, DbmToOlapClientConn* conn ) = 0;
	virtual bool addSpecialTicks( TicksAccumulator& acc, UINT32& t, UINT32& n, const HiresCounter& counter, int msgStatus ) const = 0;
	virtual void addMessageTicks(const HiresCounter& counter, int msgStatus) = 0;
	virtual void addInQueueSize( UINT32 sz ) = 0;
	virtual const char* getTraceModifier( int msgStatus /* processGenericMessage* */ ) = 0;
	virtual int getCommServerInQueueSize() const = 0;
};

class CommServerObjectWithOlap : public DbmServerObjectCommon, public CommServerObjectWithOlapInteface
{
	typedef DbmServerObjectCommon Parent;

protected:
	OlapServer olapServer;
//// replay stuff 
	int								replayMode;
	bool							useMsgTime;
//// replay of type 2 stuff
	SrvTime msgSrvTime;
	time_t  msgTimeT;
//// replay of type 2 stuff end
public:
	
	CommServerObjectWithOlap(_CommInterface& inter_) : DbmServerObjectCommon(inter_)
	{
		msgSrvTime.currentLocalTime();//safety net - we need this time in saveLogoutAll() before we got first message from replay
		msgTimeT = plib_time(0);
		replayMode = 0;
		useMsgTime = false;
	}
	int getCommServerInQueueSize() const override
	{
		return CommServerObject::getInQueueSize();
	}
	void dynamicInit( const PIniFile& iniFile ) override
	{
		// nothing specific to do - call parent's implementation
		Parent::dynamicInit( iniFile );
	}
};

class InTransactionUpdates
{
protected: // PYR-32351 - made protected to be used in the child
	typedef list< LogMessage > _List;
	
	_List				preparedMessages;

public:
	InTransactionUpdates(){};
	void commit(CommServerObjectWithOlapInteface*	dbmObj);
    void rollback();
    void insertMessage(const UINT32 msgId, CommMsgBody& body, BYTE msgMask, const OlapMsgSysInfo& sysInfo);
	void setSequenceEnd();
};

class OlapMessageProcessor
{
	set<UINT32>			allowedMessages;

public:
	OlapMessageProcessor(	const UINT32 *arr, UINT32 sizeArr );

	void processOlapMessage(	UINT32 msgId, 
								CommMsgParser &parser, 
								DbmToOlapClientConn* conn, 
								UINT32 reqId, 
								OlapServer& olapServer );
};

#endif //olapcommon_h_included
