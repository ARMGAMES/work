#ifndef basegatewayserverobject_h_included
#define basegatewayserverobject_h_included

#include "commsrv.h"
#include "commgrdnull.h"
#include "commmonitorlogger.h"

#include "ppinifile.h"
#include "heLogger.h"
#include "counters.h"

#include "dbm.h"
#include "timezone.h"
#include "basestructs.h"
#include "admincommon.h"
#include "baseconnection.h"

#define SETTINGS_SECTION_NAME  "SETTINGS"
#define DYNASETTINGS_SECTION_NAME "DYNAMICSETTINGS"
#define LOGGER_SECTION_NAME "LOGGER"
#define TRANSPORT_SECTION_NAME "TRANSPORT"
#define MAINSTORAGE_SECTION_NAME "MAINSTORAGE"


class BaseGatewayServerObject;

class AppFactoryClientConnection : public CommClientConnection
{
	BaseGatewayServerObject* obj;
public:
	AppFactoryClientConnection(CommClientGuardFactory& guardFactory_) : CommClientConnection(guardFactory_)
	{
		obj = NULL;
	}
	void init(BaseGatewayServerObject* obj_)
	{
		obj = obj_;
	}
	// OVERRIDES
	void processMessage( UINT32 msgId, const CommMsgBody& body ) override; // only COMMAPPFACTORY_CREATEOBJECT_REPLY
	void connected() override; // calls obj->appFactoryConnected()
	bool closedOrDisconnected( int errCode, const char* errMsg ) override
	{
		PLog( "AppFactoryClientConnection::closed: %d '%s'", errCode, errMsg );
		return CommClientConnection::closedOrDisconnected(errCode, errMsg);
	}
};

class TransportConnection;

class ILoad
{
public:
	virtual UINT32	getLoad( const TransportConnection& tc ) const = 0;
	virtual void	setLoad( TransportConnection& tc, const UINT32 load ) const = 0;

	virtual ~ILoad()
	{
	}
};

class LoadHandlerByRequest : public ILoad
{
	UINT32 getLoad( const TransportConnection& tc ) const override;
	void setLoad( TransportConnection& tc, const UINT32 load ) const override;
};

class LoadHandlerByLoad : public ILoad
{
	UINT32 getLoad( const TransportConnection& tc ) const override;
	void setLoad( TransportConnection& tc, const UINT32 load ) const override;
};

class TransportConnection : public BaseOutgoingGConnectionWithReadableName<BaseGatewayServerObject>
{
	UINT32 numRequests;
	UINT32 loadCounter;
public:
	const PString instanceName;
	const INT32 instanceIdx;

	TransportConnection( CommClientGuardFactory& guardFactory, 
		const char* instanceName_, 
		INT32 instanceIdx_, 
		const char* traceMarker_, 
		const char* readableName_ ) 
		: 
		BaseOutgoingGConnectionWithReadableName(guardFactory, 
			PString(traceMarker_).appendInt(instanceIdx_), PString(readableName_).appendInt(instanceIdx_)), 
		numRequests(0),
		loadCounter(0),
		instanceName(instanceName_),
		instanceIdx(instanceIdx_)
	{
		PLog("TransportConnection::ctor %p idx=%u instance='%s' trace='%s' readableName='%s'", this, instanceIdx, getInstanceName(), getTrace(), getReadableName());
	}

	UINT32 getNumRequests() const { return numRequests; }
	const char* getInstanceName() const { return instanceName; }
	UINT32 getLoadCounter() const { return loadCounter; }
	void setLoadCounter( const UINT32 load ) { loadCounter = load; }

	// OVERRIDES
	virtual void connected() override; // calls virtual obj->transportModuleConnected()
	virtual bool closedOrDisconnected(int errCode, const char* errMsg) override; // calls virtual obj->transportModuleDisconnected()

	virtual void processGMessage(UINT32 reqId, UINT32 msgId, const CommMsgBody& body, AsyncCall* call) override; // calls virtual obj->processCallback()

	UINT32 postX( UINT32 msgId, /*non-const!*/ CommMsgBody& body, /* new */ AsyncCallRequestId* call)
	{
		++numRequests;
		if (call && call->getTransportKey())
		{
			PString transportKeyStr;
			transportKeyStr.append("k=").appendUint(call->getTransportKey());
			PLog("%s", transportKeyStr.c_str());
		}
		return BaseOutgoingGConnectionWithReadableName::postX(msgId, body, call);
	}
};

// abstract
class BaseGatewayServerConnection : public CommServerConnection
{
protected:
	BaseGatewayServerObject* obj;
public:
	BaseGatewayServerConnection(BaseGatewayServerObject* obj_) : obj(obj_)
	{
		PLog("BaseGatewayServerConnection::ctor th=%p obj=%p", this, obj);
	}
	void connected() override
	{
		PLog("%d-%s-%s connected th=%p id=%08X", getType(), getDescription(), getTraceMarker(), this, id());
	}
	// flow: conn.processMessage -> obj.processGenericConnectionMessage -> conn.processMessage0 -> getObj().process_Specific_Message
	virtual void processMessage (UINT32 msgId, const CommMsgBody& body) /*final*/ override;
	virtual void processMessage0(UINT32 msgId, const CommMsgBody& body) = 0;

	void post(UINT32 msgId, CommMsgBody& body) override
	{
		PLog(">%s %u", getTraceMarker(), msgId);
		CommServerConnection::post(msgId, body);
	}

	virtual int getType() const = 0;
	virtual const char* getDescription() const = 0;
	virtual const char* getTraceMarker() const = 0;

	void replyGenericError(UINT32 replyMsgId, UINT32 reqId, INT16 errCode, const char* errDescr)
	{
		PLog("gErr=%d '%s'", errCode, errDescr);
		CommMsgBody reply;
		reply
			.composeUINT32(reqId)
			.composeINT16 (errCode)
			.composeString(errDescr);
		post(replyMsgId, reply);
	}
	void replyOk(UINT32 replyMsgId, UINT32 reqId)
	{
		CommMsgBody reply;
		reply
			.composeUINT32(reqId)
			.composeINT16 (DBM_NO_ERROR);
		post(replyMsgId, reply);
	}
};

class BaseGatewayServerConnFactory : public CommServerConnectionFactory // PYR-78900
{
protected:
	BaseGatewayServerObject& obj;
public:
	BaseGatewayServerConnFactory(CommServerGuardFactory& guardFactory_, BaseGatewayServerObject& obj_)
		: CommServerConnectionFactory(guardFactory_), obj(obj_) {}
};

class TrustedAdminServerConnection : public BaseGatewayServerConnection // PYR-19884
{
public:
    TrustedAdminServerConnection(BaseGatewayServerObject* obj_) : BaseGatewayServerConnection(obj_) {}
	void processMessage0( UINT32 msgId, const CommMsgBody& body ) override;

	int getType() const override {return CoreGatewayServerConnectionType_TrustedAdmin;}
	const char* getDescription() const override {return "TrustedAdmin";}
	const char* getTraceMarker() const override {return "A";}
};

class TrustedAdminServerConnFactory : public CommServerConnectionFactory
{
    BaseGatewayServerObject* obj;
public:
    TrustedAdminServerConnFactory(CommServerGuardFactory& guardFactory_) : CommServerConnectionFactory(guardFactory_), obj(NULL)
    {
        //#17188
        ++normalPriority;
        ++authPriority;
    }
	void init(BaseGatewayServerObject* obj_)
	{
		obj = obj_;
	}
    CommServerConnection* createConnection() override
    {
        return new TrustedAdminServerConnection( obj );
    }
};

//////////////////////////////////////////////////////////////////////////////////
class BaseGatewayServerObject : public CommServerObject
{
public:
	static CommServerNullGuardFactory nullGuardFactoryServer;
	static CommClientNullGuardFactory nullGuardFactoryClient;

	BaseGatewayServerObject(_CommInterface& inter_);
	virtual ~BaseGatewayServerObject()
	{
		PLog("BaseGatewayServerObject::dtor %p", this);
		heLogger = NULL;
	}


	//// CONNECTIONS
	CommServerConnectionPool srvPool;
	CommClientConnectionPool cliPool;
protected:
	AppFactoryClientConnection appFactoryConn;
	TrustedAdminServerConnFactory trustedAdminConnFactory;
	bool hasTransportConnections;

	void connectToServer(CommClientConnection& conn, const char* server, const char* serverObject, const char* serverConnType)
	{
		SrvObjectConnUtils::connectToServer(cliPool, conn, server, serverObject, serverConnType);
	}

public:
	vector<TransportConnection*> transportConnections;

	void appFactoryConnected(); // called from AppFactoryClientConnection::connected()
	virtual void createTransportModule(INT32 instanceIdx, bool createConnection) = 0; // called from appFactoryConnected()

	// is called from overriddden createTransportModule()
	void createTransportModuleImpl(const CommMsgBody& moduleMsg, INT32 instanceIdx, bool createConnection, const char* qualifierPrefix,
		const char* connectionName,  // transport server connection factory type = transport module srvPool.registerFactory()
		const char* dispatcherName); // transport exe registration  factory type = config param name for transport in dispatcher.ini [exe]

	void transportModuleConnected(); // called from TransportConnection::connected()
	virtual void registerServerConnFactories() = 0; // called from transportModuleConnected()

	void transportModuleDisconnected(TransportConnection& tc); // called from TransportConnection::closedOrDisconnected

	CommServerConnection* findConnection(UINT32 handle)
    {
		CommServerConnection* conn = srvPool.findConnection(handle);
		if (!conn)
		{
			PLog("!findConn(%X)", handle);
		}
        return conn;
    }


	//// MESSAGE PROCESSING

	// processCallback() is called from TransportConnection::processGMessage()
	// override it if some extras (e.g. db) should be done before and/or after processReply()
	virtual void processCallback(UINT32 reqId, UINT32 msgId, const CommMsgBody& body, CommClientGConnection::AsyncCall* call, TransportConnection& tc);

	// processGenericConnectionMessage() is called from BaseGatewayServerConnection::processMessage()
	// override it if some extras (e.g. db) should be done before and/or after processMessage0()
	virtual void processGenericConnectionMessage(UINT32 msgId, const CommMsgBody& body, BaseGatewayServerConnection& conn);

	virtual bool isBasePrologueEpilogue() { return true; } //PYR-115287

	// OVERRIDES
	void processMessage( _CommMsg& msg ) /*final*/ override
	{
		srvPool._processMessage( msg );
		cliPool._processMessage( msg );
	}
	void processTimerMessage( UINT32 handle, UINT32 msgId, CommMsgBody& body ) override;
    // specific timer processing in the children
    virtual bool doProcessTimerMessage( UINT32 handle, UINT32 msgId, CommMsgBody& body ) { return false; }

	//// INI
	void init(CommMsgBody& initMsg) override;
	virtual const char* getIniFileName() const = 0;
	virtual void dynamicInit();
	virtual void processRereadIni(UINT32 requestId, TrustedAdminServerConnection* conn);
	static void loadAndLogIniFile(PIniFile& iniFile_, const char* fullIniFileName_);

	void initBaseSettings ();
	void initBaseTransport();
	PString initDir;
	PString fullIniFileName;
	PIniFile iniFile;
	UINT32 overspanLimit;
	UINT32 numModules; // transport modules
	PString transportModuleSuffix;
	PString appFactoryServer;
	bool emulationMode;
	INT32 emulationDelay;


	//// PERFORMANCE - LOGGER
	bool useLogger;
	PString loggerServerAddress;
	PString loggerStationName;
	HeLogger* heLogger;
	UINT32 heLoggerTimerHandle;
	TicksAccumulator ticksAccumulator;
	QueueAccumulator inQueueAccumulator;
	const char* getOverspanStr(UINT32 cnt) const 
	{
		return (overspanLimit > 0 && cnt > overspanLimit) ? " overspan" : "";
	}

	void startHeLogger(const char* strServerAddress, const char* strStationName);
	void stopHeLogger();
	INT32 calculateNumRequestsForReport() const;
	INT32 calculateNumOfPendingRequests() const;
	void reportBasePerformance(CommMsgBody& reportBody);
	virtual void reportPerformance();

	//// SHUTDOWN: [AN 2010-03-30] PYR-14365
	bool isPreparingForShutdown;
	bool isReadyForShutdown;
    UINT32 shutdownTimerHandle;
	void prepareForShutdown();
	void trackTransportConnectionLoadsBeforeShutdown();
	bool isNoMoreRequests() const;
	bool isRepeatedRequestForShutdown( UINT32 msgId, const CommMsgBody& body, TrustedAdminServerConnection& conn ) const;
	bool ignoreWhilePreparingForShutdown(UINT32 msgId, UINT32 requestId, TrustedAdminServerConnection& conn, UINT32 replyMsgId) const;
	void shutdown() override;


	//// Transport Connections Management ////
	//mapping some key to transport index
	//To prevent sending transactions for one user or session or whatever via different transport modules.
	//Used for financially sensitive (deposit, bonus, etc.) or order sensitive transactions
	typedef map<UINT32, UINT32> TransportKeyToTransportId;
	TransportKeyToTransportId transportKeyToTransportIdMap;
	UINT32 findBestTransportConnection(ILoad* loadHandler = NULL ) const;
    UINT32 findBestTransportConnectionByKey( UINT32 key, ILoad* loadHandler = NULL );
	void decreaseLoad(UINT32 key, ILoad* loadHandler = NULL );

	// PYR-51017 Outgoing connections management
	PStringMap<OutgoingGConnWithTrace*> traceToGConnMap; // PYR-51017, key: traceMarker, value: OutgoingGConnWithTrace*
	PStringSet outConnReadableNames; // PYR-51017, used only to enforce out connection readable names uniqueness

	//// TRUSTED ADMIN
	virtual const char* getAdminRightForShutdown() const = 0;
    static bool checkAdminRight( const char* rightName, UINT32 replyMsgId, UINT32 reqId, const vector<PString>& adminRights, TrustedAdminServerConnection& conn ); // #19591
	void processTrustedAdminMessage(UINT32 msgId, const CommMsgBody& body, TrustedAdminServerConnection& conn);
    // specific trusted admin messages processing in the children
    virtual bool doProcessTrustedAdminMessage( UINT32 reqId, UINT32 msgId, const char* adminId, const vector<PString>& adminRights, const CommMsgBody& body, TrustedAdminServerConnection& conn ) { return false; }
};

#endif // basegatewayserverobject_h_included
