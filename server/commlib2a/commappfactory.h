#ifndef commappfactory_h_included
#define commappfactory_h_included

#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"
//#include "ppinifile.h"
#include "ppprocess.h"

#include "commmsgbody.h"
#include "commsrv.h"
#include "commgrdnull.h"
#include "commtmlocal.h"
#include "commappfactoryinterface.h"

void parseExeLine(const PIniFile::Item& item, PString& exeName, PString& logName, int& maxNumRestarts, int& minInterRestartPeriod);

#define _COMM_MAX_LOCALAPP_OBJECTS 64

#define _COMMAPPFACTORY_SUBFACTORY_CONNECTED			0x2468 //why not?
//s "s"* subFactoryName; list of objectNames, ended with ""
#define _COMMAPPFACTORY_SUBFACTORY_OBJECTDEAD			0x2469 //why not?
//ss subFactoryName; objectName

#define _COMMAPPFACTORY_SUBFACTORY_CREATEOBJECT			0x246A
//"4sm" requestId, objectName, initMsg
#define _COMMAPPFACTORY_SUBFACTORY_CREATEOBJECT_REPLY	0x246B
//"42" ["s"] requestId, errCode, [errMsg]

#define _COMMAPPFACTORY_SUBFACTORY_EXIT					0x246D

#define _COMMAPPFACTORY_SUBFACTORY_REOPENLOG			0x246E


#define _COMMAPPFACTORY_SUBFACTORY_SEH_NUMBER_UPDATE                     0x2474


//-

class CommFactoryServerBase;
class _CommFactoryServerObject;
class _CommFactoryServerConnection : public CommServerConnection
{
private:
	_CommFactoryServerObject* obj;

public:
	_CommFactoryServerConnection( _CommFactoryServerObject* obj_ );
	void processMessage( UINT32 msgId, const CommMsgBody& body ) override;
};

class _CommFactoryServerConnectionEx : public CommServerConnectionEx
{
private:
	_CommFactoryServerObject* obj;

public:
	_CommFactoryServerConnectionEx(_CommFactoryServerObject* obj_);
	void processMessage(UINT32 msgId, const CommMsgBody& body) override;
};

class _CommFactoryServerConnFactory : public CommServerConnectionFactory
{
public:
	_CommFactoryServerObject* obj;

public:
	_CommFactoryServerConnFactory( CommServerGuardFactory& guardFactory );

public:
	/* new */ CommServerConnection* createConnection() override;
};

class _CommFactoryServerConnFactoryEx : public CommServerConnectionFactory
{
public:
	_CommFactoryServerObject* obj;

public:
	_CommFactoryServerConnFactoryEx(CommServerGuardFactory& guardFactory);

public:
	/* new */ CommServerConnection* createConnection() override;
};

class _CommFactorySubFactoryServerConnection : public CommServerConnection
{
private:
	_CommFactoryServerObject* obj;

public:
	_CommFactorySubFactoryServerConnection( _CommFactoryServerObject* obj_ );
	void processMessage( UINT32 msgId, const CommMsgBody& body ) override;
	void closedOrDisconnected(int errCode, const char* errMsg) override;
	~_CommFactorySubFactoryServerConnection();
};

class _CommFactoryServerSubFactoryConnFactory : public CommServerConnectionFactory
{
public:
	_CommFactoryServerObject* obj;

public:
	_CommFactoryServerSubFactoryConnFactory( CommServerGuardFactory& guardFactory );

public:
	/* new */ CommServerConnection* createConnection() override;
};

class CommMonitorLoggerObject;

class _CommFactoryServerObject : public CommServerObject
{
private:
	CommFactoryServerBase* base;

	static CommServerNullGuardFactory guardFactory;//+++ - SHOULD be changed
	static CommClientNullGuardFactory clientGuardFactory;//MUST be the same type as guardFactory
	_CommFactoryServerConnFactory connFactory;
	_CommFactoryServerConnFactoryEx connFactoryEx;
	_CommFactoryServerSubFactoryConnFactory subFactoryConnFactory;
	CommServerConnectionPool srvPool;
	CommClientConnectionPool cliPool;
	CommClientConnection keepAliveConn;
	CommMonitorLoggerObject * hLogger;
	int numberOfSEH;
	int numberOfAsserts;
	int numberOfMemoryFailure;
public:
	_CommFactoryServerObject( _CommInterface& inter );
	void shutdown() override;
	void init( CommMsgBody& msg ) override;
	void processMessage( _CommMsg& msg ) override;
	void processTimerMessage( UINT32 handle, UINT32 msgId, CommMsgBody& body ) override;
	void _processConnMessage( CommServerConnection* conn, UINT32 msgId, const CommMsgBody& msg );
	void _processSubFactoryMessage( _CommFactorySubFactoryServerConnection* conn, UINT32 msgId, const CommMsgBody& msg );
	void _subFactoryDisconnected( _CommFactorySubFactoryServerConnection* conn, bool allowAutoRestart );
};

//****************************************************************************

class CommFactoryServerBase : public CommServerObjectFactory
{
private:
	struct _Type
	{
		_Type() : numRestarts(0), minInterRestartPeriod(0) {}

		PString localManagerTypeName;
		PString multiExeName;
		PString exeName;
		PString localPort;//for exe connection
		PString logFileDir;
		int numRestarts;
		int minInterRestartPeriod;
	};
	struct _Dispatcher
	{
		PString exeName;
		PString iniName;
		PString logName;
		PString keepAliveId;
		int keepAliveTimeout;
		UINT32 lastKeepAlive;
		/* new */ PProcess* process;
	};
	typedef PStringMap< _Type > _Types;


	struct _SubFactoryItem
	{
		int state;//0 - pending, 1 - requested, 2 - running
		UINT32 reqId;
		PString name;
		CommMsgBody initMsg;
	};
	struct _SubFactory
	{
		enum eStates
		{
			eSubFactoryNormalState,
			eSubFactoryKilledState
		};
		CommServerConnection* conn;
		PProcess *process;
		PString type;
		PString name;
		PString exeName;
		PString logFile;
		PString localPort;
		PString objectName;
		int maxObjects;
		eStates state;  // 0 - normal,  2 - closing
		bool  isObsolete;
		CommMsgBody initMsg;
		vector< /* new */ _SubFactoryItem* > objects;
	};
	typedef vector< _SubFactory > _SubFactories;

	struct _SubFactoryRestartData
	{
		_SubFactoryRestartData(_SubFactory* subFactory_) : processLastStartTime(0), numProcessRestarts(0), restartTimerHandle(0), restartTimerSubmitTime(0), numRestartsRemaining(0), subFactory(subFactory_)
		{
			subFactoryObjectName = subFactory->objectName;
		}

		UINT32 processLastStartTime;
		UINT32 numProcessRestarts;
		UINT32 restartTimerHandle;
		UINT32 restartTimerSubmitTime;
		int numRestartsRemaining;
		_SubFactory* subFactory;
		PString subFactoryObjectName;
	};
	typedef PStringMap<_SubFactoryRestartData> _SubFactoryRestartDataMap;

private:
	PCriticalSection cs;
	CommRoutingTable* routingTable;
	CommLocalThreadManager* localManager;
	_CommProtocolLog *object_logger;
	mutable _CommFactoryServerObject* factorySrvObj;

	//{ keepAlive
	PString keepAliveServer;
	PString keepAliveServerObject;
	PString keepAliveId;
	//} keepAlive

	//{ for multiExes
	PString server;
	PString serverObject;
	PString hLoggerServer;
	PString hLoggerStation;
	PString hLoggerSuffix;
	bool reportSEH;

	//} for multiExes

	_Types types;
	vector< _Dispatcher > dispatchers;
	UINT32 idGen;
	_SubFactories subFactories;
	_SubFactoryRestartDataMap subFactoryRestartDataMap;
	UINT32 lastProcessDisconnectTime;
	UINT32 numProcessesDisconnected;
	UINT32 massDisconnectDetectorTimerHandle;

protected:
	PString dir;
	PString iniFullFileName;

public:

	CommFactoryServerBase( CommRoutingTable& routingTable_, CommLocalThreadManager& localManager_ );
	void setLoggerInformation( const char* server_, const char* station_, const char* suffix_, bool reportSEH_ )
	{
		hLoggerServer.assign( server_ );
		hLoggerStation.assign( station_ );
		hLoggerSuffix.assign( suffix_ );
		reportSEH = reportSEH_;
	}

	const char * findLogFileName( CommServerConnection* conn );
	const char * findLogFileName( const char * objectName);
	void setDir( const char* dir_ );
	void setServer( const char* server_, const char* serverObject_ );
	void setKeepAlive( const char* keepAliveServer_, const char* keepAliveServerObject_, const char* appFactoryKeepAliveId_ );
	~CommFactoryServerBase();
	bool registerLocalServerObjectFactory( const char* typeName, const char* localManagerName = 0 );
	bool registerMultiExeObjectFactory( const char* typeName, const char* multiExeName, const char * logFileDir, const char * localPort, const int* maxNumRestarts = nullptr, const int* minInterRestartPeriod = nullptr );
	bool registerExeObjectFactory( const char* typeName, const char* exeName, const char * logFileDir, const char * localPort, const int* maxNumRestarts = nullptr, const int* minInterRestartPeriod = nullptr );
	bool registerMultiExeObjectFactory( const char* typeName, const char* multiExeName, const char * logFileDir, int localPort, const int* maxNumRestarts = nullptr, const int* minInterRestartPeriod = nullptr );
	bool registerExeObjectFactory( const char* typeName, const char* exeName, const char * logFileDir, int localPort, const int* maxNumRestarts = nullptr, const int* minInterRestartPeriod = nullptr );
	void registerAndStartDispatcher( const char* dispatcherExeName, const char* dispatcherIni, const char* dispatcherLog, const char* keepAliveId, int keepAliveTimeout );
	void setIniFile(const char * iniFullFileName_);
	void dynamicInit();
	void prepareInitServerObjectMsg( CommMsgBody& body );

	/* new */ CommServerObject* createServerObject( _CommInterface& inter ) const override
	{
		if (!factorySrvObj)
		{
			factorySrvObj = new _CommFactoryServerObject( inter );
		}
		return factorySrvObj;
	}

	void dump()
	{
		routingTable->dump();
	}
	void dumpEx(CommMsgParser& parser)
	{
		routingTable->dumpEx(parser);
	}
	void getPhysConnections(CommMsgBody &reply)
	{
		routingTable->getPhysConnections(reply);
	}


	int  createObject( PString& errMsg, const char* objectType, const char* objectName, const CommMsgBody& initMsg );
	void processSubFactoryConnected( CommServerConnection* conn, const char* subFactoryName, const vector< PString >& objects );
	void processSubFactoryObjectDead( CommServerConnection* conn, const char* subFactoryName, const char* objectName );
	void processSubFactoryCreateReply( CommServerConnection* conn, UINT32 reqId, int errCode );
	void processSubFactoryDisconnected( CommServerConnection* conn, bool allowAutoRestart );
	void timer();
	void gotKeepAlive( const char* keepAliveId );
	void setLogFile( const char* logFile_ );
	INT16 changeExeName( PString& errMsg, const char* objectType, const char* newExeName);
	INT16 changeLogFileDir( PString& errMsg, const char* objectType, const char* newLogFileDir);
	void tryRestartProcess(const char* objectType, const char* objectName, int numRestartsRemaining, const CommMsgBody& initMsg);
	void resetRestartParams(const char* factoryName, int numRestarts);
	void reportModuleAutoRestartStats(CommMsgBody& body) const;

	void killExes();
	void waitForExes();

protected:
	virtual PProcess * _startProcess( const char* objectName, const char* exeName, const char* logFileName, const char* localPort, const CommMsgBody& initMsg );
private:
	int  _createObject(const _Type& type, PString& errMsg, const char* objectType, const char* objectName, const CommMsgBody& initMsg, int numRestartsRemaining);
	void _startDispatcher( _Dispatcher& d );
	_SubFactory* _findSubFactory( const char* factoryName );
	void _printLogFileName(const char *fmt,...);
	void _findObjects(UINT32 pId, CommMsgBody &body);
	void _reopenObjectsLogfile(UINT32 pId, CommMsgBody &body);
	friend class _CommFactoryServerObject;
};

#endif
