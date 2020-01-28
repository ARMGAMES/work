#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"
#include "commmsgbody.h"
#include "commsrv.h"
#include "commgrdnull.h"
#include "commappfactory.h"
#include "commhercules.h"
#include "commmonitorlogger.h"

#define _DISPATCHERKEEPALIVEPOLL 500
#define _TIMER_POLL		7777	

#define _DISPATCHERLOGGERPOLL 10000
#define _TIMER_LOGGER	7778	

#define _PROCESS_RESTART 7779
#define _MASS_PROCESS_DISCONNECT_MONITOR 7780
#define _RESET_PROCESS_RESTART_PARAMS 7781

void parseExeLine(const PIniFile::Item& item, PString& exeName, PString& logName, int& maxNumRestarts, int& minInterRestartPeriod)
{
	const char* sc = strchr(item.value, ';');
	if (sc)
	{
		exeName.assign(item.value, sc);
		const char* sc1 = strchr(sc + 1, ';');
		if (sc1)
		{
			logName.assign(sc + 1, sc1);
			const char* fs = strchr(sc1 + 1, '/');
			if (fs)
			{
				PString tmp;
				tmp.assign(sc1 + 1, fs);
				maxNumRestarts = atoi(tmp.c_str());
				minInterRestartPeriod = atoi(fs + 1);
			}
		}
		else
		{
			logName = sc + 1;
		}
	}
	else
	{
		exeName = item.value;
	}
}

CommFactoryServerBase::CommFactoryServerBase( CommRoutingTable& routingTable_, CommLocalThreadManager& localManager_ )
{
	routingTable = &routingTable_;
	localManager = &localManager_;
	idGen = RANDOMTIME();
	reportSEH=false;
	object_logger = 0;
	factorySrvObj = nullptr;
	lastProcessDisconnectTime = numProcessesDisconnected = massDisconnectDetectorTimerHandle = 0;
}

void CommFactoryServerBase::setDir( const char* dir_ )
{
	dir = dir_;
}

void CommFactoryServerBase::setLogFile( const char* logFile_ )
{
	object_logger = new _CommProtocolLog( logFile_, PLogFile::perDate );
}

void CommFactoryServerBase::_printLogFileName( const char *fmt, ... )
{
	if(object_logger)
	{
		va_list args;
		va_start( args, fmt );
		object_logger->_CommLog( 0, fmt, args );
		va_end( args );
	}
}

void CommFactoryServerBase::setIniFile( const char * iniFullFileName_ )
{
	iniFullFileName = iniFullFileName_;
}

void CommFactoryServerBase::setServer( const char* server_, const char* serverObject_ )
{
	server = server_ ? server_ : "";
	serverObject = serverObject_ ? serverObject_ : "";
}

void CommFactoryServerBase::setKeepAlive( const char* keepAliveServer_, const char* keepAliveServerObject_, const char* keepAliveId_ )
{
	keepAliveServer = keepAliveServer_;
	keepAliveServerObject = keepAliveServerObject_;
	keepAliveId = keepAliveId_;
}

bool CommFactoryServerBase::registerLocalServerObjectFactory( const char* typeName, const char* localManagerTypeName )
{
	PLock lock( cs );
	PASSERT3( localManager );
	_Types::iterator found = types.find( typeName );
	if( found == types.end() )
	{
		_Type t;
		t.localManagerTypeName = localManagerTypeName ? localManagerTypeName : typeName;
		types.insert( _Types::value_type( typeName, t ) );
		return true;
	}
	return false;
}
bool CommFactoryServerBase::registerExeObjectFactory( const char* typeName, const char* exeName, const char * logFileDir, int localPort, const int* maxNumRestarts, const int* minInterRestartPeriod )
{
	PString n;
	n.appendInt(localPort);
	return registerExeObjectFactory(typeName,exeName,logFileDir, n.c_str(), maxNumRestarts, minInterRestartPeriod);
}

bool CommFactoryServerBase::registerExeObjectFactory( const char* typeName, const char* exeName, const char * logFileDir, const char * localPort, const int* maxNumRestarts, const int* minInterRestartPeriod )
{
	PLock lock( cs );
	_Types::iterator found = types.find( typeName );
	if( found == types.end() )
	{
		_Type t;
		t.exeName = exeName;
		t.localPort = localPort;
		if (maxNumRestarts && minInterRestartPeriod)
		{
			t.numRestarts = *maxNumRestarts;
			t.minInterRestartPeriod = *minInterRestartPeriod;
		}
		if (logFileDir && *logFileDir)
			t.logFileDir = logFileDir;
		types.insert( _Types::value_type( typeName, t ) );
		return true;
	}
	else
	{
		_Type& t = found->second;
		if (maxNumRestarts && minInterRestartPeriod)
		{
			t.numRestarts = *maxNumRestarts;
			t.minInterRestartPeriod = *minInterRestartPeriod;
			PTRACE3("CommFactoryServerBase::registerExeObjectFactory: auto restart params for type '%s' updated: numRestarts = %d minInterRestartPeriod = %d", typeName, t.numRestarts, t.minInterRestartPeriod);
		}
		if (logFileDir && *logFileDir)
		{
			t.logFileDir = logFileDir;
			PTRACE3("CommFactoryServerBase::registerExeObjectFactory: log file dir for type '%s' updated to '%s'", typeName, t.logFileDir.c_str());
		}
	}
	return false;
}
bool CommFactoryServerBase::registerMultiExeObjectFactory( const char* typeName, const char* multiExeName, const char * logFileDir, int localPort, const int* maxNumRestarts, const int* minInterRestartPeriod )
{
	PString n;
	n.appendInt(localPort);
	return registerMultiExeObjectFactory(typeName,multiExeName,logFileDir, n.c_str(), maxNumRestarts, minInterRestartPeriod);
}

bool CommFactoryServerBase::registerMultiExeObjectFactory( const char* typeName, const char* multiExeName, const char * logFileDir, const char * localPort, const int* maxNumRestarts, const int* minInterRestartPeriod )
{
	PLock lock( cs );
	_Types::iterator found = types.find( typeName );
	if( found == types.end() )
	{
		_Type t;
		t.multiExeName = multiExeName;
		t.localPort = localPort;
		if (maxNumRestarts && minInterRestartPeriod)
		{
			t.numRestarts = *maxNumRestarts;
			t.minInterRestartPeriod = *minInterRestartPeriod;
		}
		if (logFileDir && *logFileDir)
			t.logFileDir = logFileDir;
		types.insert( _Types::value_type( typeName, t ) );
		return true;
	}
	else
	{
		_Type& t = found->second;
		if (maxNumRestarts && minInterRestartPeriod)
		{
			t.numRestarts = *maxNumRestarts;
			t.minInterRestartPeriod = *minInterRestartPeriod;
			PTRACE3("CommFactoryServerBase::registerMultiExeObjectFactory: auto restart params for type '%s' updated: numRestarts = %d minInterRestartPeriod = %d", typeName, t.numRestarts, t.minInterRestartPeriod);
		}
		if (logFileDir && *logFileDir)
		{
			t.logFileDir = logFileDir;
			PTRACE3("CommFactoryServerBase::registerMultiExeObjectFactory: log file dir for type '%s' updated to '%s'", typeName, t.logFileDir.c_str());
		}
	}
	return false;
}

void CommFactoryServerBase::_startDispatcher( _Dispatcher& d )
{
	d.process = new PProcess();

	PString exeFileName;
	exeFileName.assign( dir ).append( d.exeName );

	PString iniFileName;
	iniFileName.assign( dir ).append( d.iniName );

	PString logFileName;
	logFileName.assign( dir ).append( d.logName );

	const char* parms[ 4 ];
	parms[ 0 ] = "-debug";
	parms[ 1 ] = "-logfile";
	parms[ 2 ] = logFileName;
	parms[ 3 ] = iniFileName;
	PTRACE3( "CommFactoryServerBase: starting dispatcher '%s'( '%s', '%s', '%s', '%s' )", d.exeName.c_str(), parms[ 0 ], parms[ 1 ], parms[ 2 ], parms[ 3 ] );
	d.process->start( exeFileName, 4, parms );
	PTRACE3( "CommFactoryServerBase: dispatcher process started" );
	d.lastKeepAlive = SYSTEMTICKS();
}

void CommFactoryServerBase::registerAndStartDispatcher( const char* dispatcherExeName, const char* dispatcherIni, const char* dispatcherLog, const char* keepAliveId, int keepAliveTimeout )
{
	PLock lock( cs );
	_Dispatcher& d = *dispatchers.insert( dispatchers.end(), _Dispatcher() );
	d.exeName = dispatcherExeName;
	d.iniName = dispatcherIni;
	d.logName = dispatcherLog;
	d.keepAliveId = keepAliveId;
	d.keepAliveTimeout = keepAliveTimeout;

	_startDispatcher( d );
}

PProcess * CommFactoryServerBase::_startProcess( const char* objectName, const char* exeName, const char* logFileName, const char* localPort, const CommMsgBody& initMsg)
{
	PProcess * proc = new PProcess();

	bool isAbsPath = (strchr(exeName, '\\') || strchr(exeName, '/'));
	PString exeFileName;
	if (!isAbsPath)
	{
		exeFileName.assign(dir);
	}
	exeFileName.append(exeName);

	PString envName( "PYRCOMMLOCALAPP" );
	PString envValue;
	int sz = initMsg._size();
	const BYTE* p = initMsg._readPtr();
	for( int i=0; i < sz ; ++i )
	{
		BYTE c = *p++;
		int c0 = c >> 4;
		PASSERT3( c0 < 16 );
		int c1 = c & 0x0F;
		PASSERT3( c1 < 16 );
		if( c0 < 10 )
			envValue.append( (char)('0' + c0) );
		else
			envValue.append( (char)('A' + c0 - 10) );
		if( c1 < 10 )
			envValue.append( (char)('0' + c1) );
		else
			envValue.append( (char)('A' + c1 - 10) );
	}
	PProcess::setenv( envName,envValue );//! - potential risk of non-reentrability


	const char* parms[ 6 ];
	int nParam = 5;
	parms[ 0 ] = localPort;
	parms[ 1 ] = objectName;
	parms[ 2 ] = logFileName;
	parms[ 3 ] = "0"; // Old 'restore flag', always 0 now
	parms[ 4 ] = "PYRCOMMLOCALAPP";
	if (*routingTable->_getIdent())
	{
		parms[ 5 ] = routingTable->_getIdent();
		nParam=6;
	}



	PTRACE3( "CommFactoryServerBase: starting process '%s'( '%s' '%s' '%s' '%s' )", exeFileName.c_str(), parms[ 0 ], parms[ 1 ], parms[ 2 ], parms[ 3 ] );
	proc->start( exeFileName, nParam, parms );
	PTRACE3( "CommFactoryServerBase: process started" );
	return proc;
}

/*void CommFactoryServerBase::_replaceProcess( const char* objectName, const char* exeName, const char* exeNewName, const char* logFileName_, int localPort )
{
PProcess* proc = new PProcess();
processes.push_back( _Processes::value_type( objectName, proc ) );

PString exeFileName;
exeFileName.assign( dir ).append( exeName );

PString logFileName;
logFileName.assign( dir ).append( logFileName_ );//.append( ".log" );

PString env( "PYRCOMMLOCALAPP=" );
int sz = initMsg._size();
const BYTE* p = initMsg._readPtr();
for( int i=0; i < sz ; ++i )
{
BYTE c = *p++;
int c0 = c >> 4;
PASSERT3( c0 < 16 );
int c1 = c & 0x0F;
PASSERT3( c1 < 16 );
if( c0 < 10 )
env.append( '0' + c0 );
else
env.append( 'A' + c0 - 10 );
if( c1 < 10 )
env.append( '0' + c1 );
else
env.append( 'A' + c1 - 10 );
}
putenv( env );//! - potential risk of non-reentrability

char localPortStr[ 16 ];
sprintf( localPortStr, "%d", localPort );
const char* parms[ 4 ];
parms[ 0 ] = localPortStr;
parms[ 1 ] = objectName;
parms[ 2 ] = logFileName;
parms[ 3 ] = "PYRCOMMLOCALAPP";

PTRACE3( "CommFactoryServerBase: starting process '%s'( '%s' '%s' '%s' '%s' )", exeFileName.c_str(), parms[ 0 ], parms[ 1 ], parms[ 2 ], parms[ 3 ] );
proc->start( exeFileName, 4, parms );
PTRACE3( "CommFactoryServerBase: process started" );
}
*/
INT16 CommFactoryServerBase::changeExeName( PString& errMsg, const char* objectType, const char* newExeName)
{
	PLock lock( cs );
	errMsg = "";

	_Types::iterator found = types.find( objectType );
	if( found == types.end() )
	{
		errMsg = "Unknown object type";
		return COMMAPPFACTORY_ERROR_UNKNOWNTYPE;
	}

	_Type & type = (*found).second;
	if( *type.multiExeName )
	{
		type.multiExeName = newExeName;
		int n = subFactories.size();
		for( int i=0; i < n ; ++i )
		{
			_SubFactory& sf = subFactories[ i ];
			if(strcmp( sf.type, objectType ) == 0 )
			{
				sf.isObsolete = true;
			}
		}
	}
	else if( *type.exeName )
	{
		type.exeName = newExeName;
	}
	return 0;
}
INT16 CommFactoryServerBase::changeLogFileDir( PString& errMsg, const char* objectType, const char* newLogFileDir)
{
	PLock lock( cs );
	errMsg = "";

	_Types::iterator found = types.find( objectType );
	if( found == types.end() )
	{
		errMsg = "Unknown object type";
		return COMMAPPFACTORY_ERROR_UNKNOWNTYPE;
	}

	_Type & type = (*found).second;
	if (newLogFileDir && *newLogFileDir)
		type.logFileDir = newLogFileDir;
	return 0;
}

int CommFactoryServerBase::_createObject(const _Type& type, PString& errMsg, const char* objectType, const char* objectName, const CommMsgBody& initMsg, int numRestartsRemaining)
{
	if (routingTable->localNameExists(objectName))//moved down to avoid any risk of deadlock (cs with RoutingTable::cs) Most likely there is no risk anyway, but better safe than sorry
	{
		errMsg = "Object name already exists";
		return COMMAPPFACTORY_ERROR_DUPLICATENAME;
	}

	if (*type.localManagerTypeName)
	{
		localManager->createServerObject(type.localManagerTypeName, objectName, &initMsg);
		return COMMAPPFACTORY_ERROR_OK;
	}
	else if (*type.multiExeName)
	{
		int n = subFactories.size();
		for (int i = 0; i < n; ++i)
		{
			_SubFactory& sf = subFactories[i];
			if (sf.isObsolete == false && sf.state == _SubFactory::eSubFactoryNormalState && sf.objects.size() < sf.maxObjects && strcmp(sf.type, objectType) == 0)
			{//using existing factory
			 // and only if it is not in replacing state
				PTRACE3("CommFactoryServerBase::createObject: requesting creation of object '%s'", objectName);
				_printLogFileName("Object %s, log file name: %s", objectName, sf.logFile.c_str());
				UINT32 reqId = idGen++;
				if (sf.conn)
				{
					CommMsgBody body;
					body.composeUINT32(reqId).composeString(objectName).composeMsgBody(initMsg);
					sf.conn->post(_COMMAPPFACTORY_SUBFACTORY_CREATEOBJECT, body);
				}

				_SubFactoryItem* item = new _SubFactoryItem();
				item->state = 1;
				item->reqId = reqId;
				item->name = objectName;
				item->initMsg.copyFrom(initMsg);
				sf.objects.push_back(item);
				return COMMAPPFACTORY_ERROR_OK;
			}
		}

		//not found - creating new factory
		UINT32 factoryId = idGen++;
		char factoryName[64];
		char factoryNameForLogFile[64];
		sprintf(factoryName, "CommAppSubFactory%08X", factoryId);
		sprintf(factoryNameForLogFile, "CASF_%08X", factoryId);

		PTRACE3("CommFactoryServerBase::createObject: creating subfactory '%s' for object '%s'", factoryName, objectName);
		CommMsgBody factoryInit;
		PString _server(server);
		_server.append(type.localPort);
		factoryInit.composeString(factoryName).composeString(_server).composeString(serverObject).composeString("subfactory");
		PString logFileName;
		if (*type.logFileDir)
		{
			logFileName.assign(type.logFileDir);
			if (logFileName[logFileName.length() - 1] != '\\' &&
				logFileName[logFileName.length() - 1] != '/')
				logFileName.append(FILESEPARATORCHAR);
			logFileName.append(factoryNameForLogFile);
		}
		else
		{
			logFileName.assign(dir).append(factoryNameForLogFile);
		}

		logFileName.append('.').append(objectType).append(".log");
		PProcess * proc = _startProcess(factoryName, type.multiExeName, logFileName, type.localPort, factoryInit);

		_SubFactory& sf = *subFactories.insert(subFactories.end(), _SubFactory());
		sf.conn = 0;
		sf.type = objectType;
		sf.name = factoryName;
		sf.exeName = type.multiExeName;
		sf.logFile = logFileName;
		sf.localPort = type.localPort;
		sf.state = _SubFactory::eSubFactoryNormalState;
		sf.process = proc;
		sf.maxObjects = _COMM_MAX_LOCALAPP_OBJECTS - 1;//1 - subFactory itself
		sf.isObsolete = false;
		int newNumRestartsRemaining = (numRestartsRemaining ? numRestartsRemaining - 1 : type.numRestarts);
		if (type.minInterRestartPeriod && *objectName)
		{
			sf.objectName = objectName;
			auto sfrdIt = subFactoryRestartDataMap.find(sf.objectName);
			if (sfrdIt == subFactoryRestartDataMap.end())
			{
				sfrdIt = subFactoryRestartDataMap.insert(sf.objectName, _SubFactoryRestartData(&sf)).first;
			}
			else
			{
				sfrdIt->second.subFactory = &sf;
			}
			_SubFactoryRestartData& sfrd = sfrdIt->second;
			sfrd.numRestartsRemaining = newNumRestartsRemaining;
			sfrd.numProcessRestarts = (numRestartsRemaining ? sfrd.numProcessRestarts + 1 : 0);
			sfrd.processLastStartTime = SYSTEMTICKS();
			sf.initMsg.copyFrom(initMsg);
			if (routingTable->getAutoRestartParamsResetTimerFreq() && numRestartsRemaining && type.numRestarts && type.numRestarts != sfrd.numRestartsRemaining)
			{
				CommMsgBody restartData;
				restartData.composeString(sf.name).composeINT32(type.numRestarts);
				sfrd.restartTimerHandle = factorySrvObj->postTimerMessage(_RESET_PROCESS_RESTART_PARAMS, restartData, routingTable->getAutoRestartParamsResetTimerFreq());
				sfrd.restartTimerSubmitTime = SYSTEMTICKS();
			}
			else
			{
				sfrd.restartTimerHandle = sfrd.restartTimerSubmitTime = 0;
			}
			PTRACE3("CommFactoryServerBase::createObject: process %s is restartable: numRestartsRemaining = %d minInterRestartPeriod = %d", type.multiExeName.c_str(), sfrd.numRestartsRemaining, type.minInterRestartPeriod);
		}
		_printLogFileName("Object %s, log file name: %s", objectName, sf.logFile.c_str());
		_SubFactoryItem* item = new _SubFactoryItem();
		item->state = 0;
		item->reqId = 0;
		item->name = objectName;
		item->initMsg.copyFrom(initMsg);
		sf.objects.push_back(item);
		return COMMAPPFACTORY_ERROR_OK;
	}
	else
	{
		UINT32 factoryId = idGen++;
		char factoryName[64];
		sprintf(factoryName, "CommAppSubFactory%08X", factoryId);

		PTRACE3("CommFactoryServerBase::createObject: creating subfactory '%s' for object '%s'", factoryName, objectName);
		CommMsgBody factoryInit;
		PString _server(server);
		_server.append(type.localPort);
		factoryInit.composeString(factoryName).composeString(_server).composeString(serverObject).composeString("subfactory");

		PString logFileName;
		if (*type.logFileDir)
		{
			logFileName.assign(type.logFileDir);
			if (logFileName[logFileName.length() - 1] != '\\' &&
				logFileName[logFileName.length() - 1] != '/')
				logFileName.append(FILESEPARATORCHAR);
			logFileName.append(objectName);
		}
		else
		{
			logFileName.assign(dir).append(objectName);
		}
		logFileName.append(".log");
		PProcess *proc = _startProcess(factoryName, type.exeName, logFileName, type.localPort, factoryInit);
		_SubFactory& sf = *subFactories.insert(subFactories.end(), _SubFactory());
		sf.conn = 0;
		sf.type = objectType;
		sf.name = factoryName;
		sf.exeName = type.exeName;
		sf.logFile = logFileName;
		sf.localPort = type.localPort;
		sf.state = _SubFactory::eSubFactoryNormalState;
		sf.process = proc;
		sf.maxObjects = 2;//1 - subFactory itself + 1 object
		sf.isObsolete = false;
		int newNumRestartsRemaining = (numRestartsRemaining ? numRestartsRemaining - 1 : type.numRestarts);
		if (type.minInterRestartPeriod && *objectName)
		{
			sf.objectName = objectName;
			auto sfrdIt = subFactoryRestartDataMap.find(sf.objectName);
			if (sfrdIt == subFactoryRestartDataMap.end())
			{
				sfrdIt = subFactoryRestartDataMap.insert(sf.objectName, _SubFactoryRestartData(&sf)).first;
			}
			else
			{
				sfrdIt->second.subFactory = &sf;
			}
			_SubFactoryRestartData& sfrd = sfrdIt->second;
			sfrd.numRestartsRemaining = newNumRestartsRemaining;
			sfrd.numProcessRestarts = (numRestartsRemaining ? sfrd.numProcessRestarts + 1 : 0);
			sfrd.processLastStartTime = SYSTEMTICKS();
			sf.initMsg.copyFrom(initMsg);
			if (routingTable->getAutoRestartParamsResetTimerFreq() && numRestartsRemaining && type.numRestarts && type.numRestarts != sfrd.numRestartsRemaining)
			{
				CommMsgBody restartData;
				restartData.composeString(sf.name).composeINT32(type.numRestarts);
				sfrd.restartTimerHandle = factorySrvObj->postTimerMessage(_RESET_PROCESS_RESTART_PARAMS, restartData, routingTable->getAutoRestartParamsResetTimerFreq());
				sfrd.restartTimerSubmitTime = SYSTEMTICKS();
			}
			else
			{
				sfrd.restartTimerHandle = sfrd.restartTimerSubmitTime = 0;
			}
			PTRACE3("CommFactoryServerBase::createObject: process %s is restartable: numRestartsRemaining = %d minInterRestartPeriod = %d", type.exeName.c_str(), sfrd.numRestartsRemaining, type.minInterRestartPeriod);
		}
		_printLogFileName("Object %s, log file name: %s", objectName, sf.logFile.c_str());
		_SubFactoryItem* item = new _SubFactoryItem();
		item->state = 0;
		item->reqId = 0;
		item->name = objectName;
		item->initMsg.copyFrom(initMsg);
		sf.objects.push_back(item);
		return COMMAPPFACTORY_ERROR_OK;
	}
}

int CommFactoryServerBase::createObject( PString& errMsg, const char* objectType, const char* objectName, const CommMsgBody& initMsg )
{
	PLock lock( cs );
	errMsg = "";

	_Types::iterator found = types.find( objectType );
	if( found == types.end() )
	{
		errMsg = "Unknown object type";
		return COMMAPPFACTORY_ERROR_UNKNOWNTYPE;
	}

	_Type type = (*found).second;//make a copy of type to be able to unlock
	lock.unlock();//needed to prevent potential deadlock with localManager
	return _createObject(type, errMsg, objectType, objectName, initMsg, 0);
}

void CommFactoryServerBase::reportModuleAutoRestartStats(CommMsgBody& body) const
{
	std::for_each(subFactoryRestartDataMap.cbegin(), subFactoryRestartDataMap.cend(), [&body](const std::pair<const char*, _SubFactoryRestartData>& sfrdPair)
	{
		PString name = sfrdPair.second.subFactoryObjectName;
		if (!name.length())
		{
			return;
		}
		name.append(".NumProcessRestarts");
		body.composeBYTE(CommMonitorLoggerObject::UIntType).composeString(name).composeUINT32(sfrdPair.second.numProcessRestarts);

		name = sfrdPair.second.subFactoryObjectName;
		name.append(".NumRestartsRemaining");
		body.composeBYTE(CommMonitorLoggerObject::IntType).composeString(name).composeINT32(sfrdPair.second.numRestartsRemaining);
	});
}

void CommFactoryServerBase::tryRestartProcess(const char* objectType, const char* objectName, int numRestartsRemaining, const CommMsgBody& initMsg)
{
	int massCrashNumProcessesThreshold = routingTable->getMassCrashNumProcessesThreshold();
	if (massCrashNumProcessesThreshold && numProcessesDisconnected >= massCrashNumProcessesThreshold)
	{
		PTRACE3("Object %s wasn't restarted - mass disconnect detected", objectName);
		return;
	}

	_Type type;
	PLock lock(cs);
	_Types::iterator found = types.find(objectType);
	if (found != types.end())
	{
		type = found->second;
	}
	lock.unlock();
	
	PString errMsg;
	int err = _createObject(type, errMsg, objectType, objectName, initMsg, numRestartsRemaining);
	if (err == COMMAPPFACTORY_ERROR_OK)
	{
		PLog("Object %s restarted (num restarts remaining = %d)", objectName, numRestartsRemaining - 1);
	}
	else
	{
		PLog("Object %s wasn't restarted: error %d (%s)", objectName, err, errMsg.c_str());
	}
}

void CommFactoryServerBase::resetRestartParams(const char* factoryName, int numRestarts)
{
	for (size_t i = 0; i < subFactories.size(); ++i)
	{
		_SubFactory& sf = subFactories[i];
		if (sf.objectName.length() && sf.name.equals(factoryName))
		{
			auto sfrdIt = subFactoryRestartDataMap.find(sf.objectName);
			if (sfrdIt != subFactoryRestartDataMap.end())
			{
				_SubFactoryRestartData& sfrd = sfrdIt->second;
				sfrd.numRestartsRemaining = numRestarts;
				PTRACE3("CommFactoryServerBase::resetRestartParams: numRestartsRemaining for '%s' reset to %d", sf.name.c_str(), sfrd.numRestartsRemaining);
			}
			break;
		}
	}
}

void CommFactoryServerBase::_findObjects(UINT32 pId, CommMsgBody &body)
{
	int i;
	for (i=0; i < subFactories.size();i++)
	{
		if (subFactories[i].process->getPID() == pId)
		{
			body.composeINT16(0)
				.composeUINT32(subFactories[i].objects.size());
			for( int j=0; j < subFactories[i].objects.size() ; j++ )
			{
				body.composeString(subFactories[i].objects[j]->name);
			}
			return;
		}
	}
	body.composeINT16(1)
		.composeString("PID not found");

}

void CommFactoryServerBase::_reopenObjectsLogfile(UINT32 pId, CommMsgBody &body)
{
	int i;
	for (i=0; i < subFactories.size();i++)
	{
		if (subFactories[i].process->getPID() == pId)
		{

			CommMsgBody request;
			if (subFactories[i].conn)
			{
				body.composeINT16(0);
				subFactories[i].conn->post(_COMMAPPFACTORY_SUBFACTORY_REOPENLOG,request);
			}
			else
			{
				body.composeINT16(1)
					.composeString("Connection to object not estableshed");
			}
			return;
		}
	}
	body.composeINT16(1)
		.composeString("PID not found");

}


void CommFactoryServerBase::killExes()
{
	int n = subFactories.size();
	int i;
	for( i=0; i < n ; ++i )
	{
		_SubFactory& sf = subFactories[ i ];
		sf.state = _SubFactory::eSubFactoryKilledState;
		sf.process->kill();
	}

	n = dispatchers.size();
	for( i=0; i < n ; ++i )
	{
		_Dispatcher& d = dispatchers[ i ];
		PTRACE3( "CommFactoryServerBase: killing dispatcher #%d", i );
		d.process->kill();
	}
}

void CommFactoryServerBase::waitForExes()
{
	int n = subFactories.size();
	int i;
	for( i=0; i < n ; ++i )
	{
		_SubFactory& sf = subFactories[ i ];
		PTRACE3( "CommFactoryServerBase: waiting for process '%s' termination...", sf.exeName.c_str() );
		sf.process->join();
		PTRACE3( "CommFactoryServerBase: process '%s' terminated", sf.exeName.c_str() );
	}

	n = dispatchers.size();
	for( i=0; i < n ; ++i )
	{
		_Dispatcher& d = dispatchers[ i ];
		PTRACE3( "CommFactoryServerBase: waiting for dispatcher #%d termination...", i );
		d.process->join();
		PTRACE3( "CommFactoryServerBase: dispatcher #%d terminated", i );
	}
}

void CommFactoryServerBase::prepareInitServerObjectMsg( CommMsgBody& body )
{
	CommFactoryServerBase* _this = this;
	body._composeFixedBlock( (const BYTE*)&_this, sizeof(CommFactoryServerBase*) );
}

CommFactoryServerBase::~CommFactoryServerBase()
{
	int n = subFactories.size();
	int i;
	for( i=0; i < n ; ++i )
	{
		_SubFactory& sf = subFactories[ i ];
		int m = sf.objects.size();
		for( int j=0; j < m ; ++j )
		{
			_SubFactoryItem* item = sf.objects[ j ];
			delete item;
		}
		delete sf.process;
	}
	if (object_logger)
		delete object_logger;
}

void CommFactoryServerBase::gotKeepAlive( const char* keepAliveId )
{
	//PTRACE( "CommFactoryServerBase::gotKeepAlive( ''%s' )", keepAliveId );
	int n = dispatchers.size();
	int i;
	for( i=0; i < n ; ++i )
	{
		_Dispatcher& d = dispatchers[ i ];
		if( d.keepAliveId.equals( keepAliveId ) )
		{
			d.lastKeepAlive = SYSTEMTICKS();
			//PTRACE( "CommFactoryServerBase::lastKeepAlive[ ''%s' ] = %08X", keepAliveId, d.lastKeepAlive );
		}
	}
}

void CommFactoryServerBase::timer()
{
	int n = dispatchers.size();
	UINT32 now = SYSTEMTICKS();
	for( int i=0; i < n ; ++i )
	{
		_Dispatcher& d = dispatchers[ i ];
		if( d.keepAliveTimeout && now - d.lastKeepAlive > _DISPATCHERKEEPALIVEPOLL * 2 )
		{
			PTRACE3( "CommFactoryServerBase::timer(): now=%08X lastKeepAlive=%08X diff=%d", now, d.lastKeepAlive, now - d.lastKeepAlive );
		}

		if( d.keepAliveTimeout && now - d.lastKeepAlive > d.keepAliveTimeout )
		{
			PTRACE3( "CommFactoryServerBase::timer(): now=%08X lastKeepAlive=%08X diff=%d", now, d.lastKeepAlive, now - d.lastKeepAlive );
			PLog( "Dispatcher #%d hanged", i );
#if 1
			PLog( "Dispatcher #%d hanged - terminating...", i );
			d.process->kill9();
			PLog( "Waiting for dispatcher termination..." );
			d.process->join();
			PLog( "Dispatcher #%d terminated.", i );
			delete d.process;
			d.process = 0;

			PLog( "Restarting dispatcher #%d...", i );
			_startDispatcher( d );
#endif
		}
	}
}

CommFactoryServerBase::_SubFactory* CommFactoryServerBase::_findSubFactory( const char* factoryName )
{
	int n = subFactories.size();
	for( int i=0; i < n ; ++i )
	{
		_SubFactory& sf = subFactories[ i ];
		if( strcmp( sf.name, factoryName ) == 0 )
			return &sf;
	}
	return 0;
}

void CommFactoryServerBase::processSubFactoryConnected( CommServerConnection* conn, const char* subFactoryName, const vector< PString >& objects )
{
	// Called only when new process created
	// Never called in replacing process.
	_SubFactory* sf = _findSubFactory( subFactoryName );
	PASSERT3( sf );
	sf->conn = conn;
	int n = sf->objects.size();
	for( int i=n-1; i >= 0 ; --i )
	{
		_SubFactoryItem* sfi = sf->objects[ i ];

		bool found = false;
		int m = objects.size();
		for( int j=0; j < m ; ++j )
		{
			if( strcmp( objects[ j ], sfi->name ) == 0 )
			{
				found = true;
				break;
			}
		}

		if( !found )
		{
			switch( sfi->state )
			{
			case 0:
			case 1:
				{
					PTRACE3( "CommFactoryServerBase::processConnectedSubFactory: requesting creation of object '%s'", sfi->name.c_str() );

					UINT32 reqId = idGen++;
					CommMsgBody body;
					body.composeUINT32( reqId ).composeString( sfi->name ).composeMsgBody( sfi->initMsg );
					sf->conn->post( _COMMAPPFACTORY_SUBFACTORY_CREATEOBJECT, body );

					sfi->state = 1;
					sfi->reqId = reqId;
				}
				break;

			case 2:
				PLog( "CommFactoryServerBase::processConnectedSubFactory: object '%s' no longer exists - deleting", objects[ i ].c_str() );
				delete sfi;
				sf->objects.erase( sf->objects.begin() + i );//safe only as long as i is an int and counting down
				break;

			default:
				PASSERT3( 0 );
				break;
			}
		}
	}
}

void CommFactoryServerBase::processSubFactoryDisconnected( CommServerConnection* conn, bool allowAutoRestart )
{
	// This function called in all cases: killed object, replacing, etc.
	// Check the state!
	PTRACE3("CommFactoryServerBase::processSubFactoryDisconnected: connId=%x", conn->id());
	UINT32 now = SYSTEMTICKS();
	int massCrashTimeThreshold = routingTable->getMassCrashTimeThreshold();
	int n = subFactories.size();
	bool connFound = false;
	for( int i=0; i < n ; ++i )
	{
		_SubFactory& sf = subFactories[ i ];
		if( sf.conn == conn )
		{
			connFound = true;
			bool exitCodeObtained = false;
			long processExitCode = -1;
			if (sf.process)
			{
				exitCodeObtained = sf.process->getExitCode(processExitCode);
			}

			if (sf.state == _SubFactory::eSubFactoryKilledState || sf.state == _SubFactory::eSubFactoryNormalState )
			{
				// process was killed or dead.
				// delete everything.
				if (exitCodeObtained)
				{
					PTRACE3("CommFactoryServerBase::processSubFactoryDisconnected: object '%s/%s' killed, state = %d (process exit code = %ld)", sf.name.c_str(), sf.exeName.c_str(), sf.state, processExitCode);
				}
				else
				{
					PTRACE3("CommFactoryServerBase::processSubFactoryDisconnected: object '%s/%s' killed, state = %d", sf.name.c_str(), sf.exeName.c_str(), sf.state);
				}
			}
			else
			{
				// Just in case. It shouldn't happen.
				if (exitCodeObtained)
				{
					PTRACE3("CommFactoryServerBase::processSubFactoryDisconnected: ERROR: unexpected object state %d '%s/%s' (process exit code = %ld)", sf.state, sf.name.c_str(), sf.exeName.c_str(), processExitCode);
				}
				else
				{
					PTRACE3("CommFactoryServerBase::processSubFactoryDisconnected: ERROR: unexpected object state %d '%s/%s'", sf.state, sf.name.c_str(), sf.exeName.c_str());
				}
			}
			sf.conn = 0;
			int n = sf.objects.size();
			for (int j = 0; j < n; ++j)
			{
				_SubFactoryItem* sfi = sf.objects[j];
				delete sfi;
			}
			delete sf.process;
			auto sfrdIt = (sf.objectName.length() ? subFactoryRestartDataMap.find(sf.objectName) : subFactoryRestartDataMap.end());
			if (sfrdIt != subFactoryRestartDataMap.end())
			{
				_SubFactoryRestartData& sfrd = sfrdIt->second;
				bool restartable = sfrd.numRestartsRemaining;
				if (allowAutoRestart && restartable && factorySrvObj && !routingTable->isShuttingDown() && massCrashTimeThreshold)
				{
					if (sfrd.numProcessRestarts)
					{
						PLock lock(cs);
						_Types::iterator found = types.find(sf.type);
						restartable = (found != types.end() && now - sfrd.processLastStartTime >= found->second.minInterRestartPeriod);
						lock.unlock();
					}
					if (restartable)
					{
						CommMsgBody restartData;
						restartData.composeString(sf.type).composeString(sf.objectName).composeINT32(sfrd.numRestartsRemaining).composeMsgBody(sf.initMsg);
						factorySrvObj->postTimerMessage(_PROCESS_RESTART, restartData, massCrashTimeThreshold);
						PTRACE5("CommFactoryServerBase::processSubFactoryDisconnected: posted auto-restart message for %s/%s", sf.objectName.c_str(), sf.exeName.c_str());
					}
				}
				if (sfrd.restartTimerHandle)
				{
					factorySrvObj->deleteTimerMessage(sfrd.restartTimerHandle);
					sfrd.restartTimerHandle = sfrd.restartTimerSubmitTime = 0;
				}
			}
			subFactories.erase(subFactories.begin() + i);
			if (sfrdIt != subFactoryRestartDataMap.end())
			{
				_SubFactoryRestartData& sfrd = sfrdIt->second;
				sfrd.subFactory = nullptr;
			}
			break;
		}
	}
	
	if (allowAutoRestart && massCrashTimeThreshold && factorySrvObj && connFound && (!lastProcessDisconnectTime || now - lastProcessDisconnectTime < massCrashTimeThreshold))
	{
		lastProcessDisconnectTime = now;
		++numProcessesDisconnected;
		if (massDisconnectDetectorTimerHandle)
		{
			if (factorySrvObj->deleteTimerMessage(massDisconnectDetectorTimerHandle))
			{
				PTRACE5("CommFactoryServerBase::processSubFactoryDisconnected: deleted timer handle %u", massDisconnectDetectorTimerHandle);
			}
			else
			{
				PTRACE5("CommFactoryServerBase::processSubFactoryDisconnected: failed to delete timer handle %u", massDisconnectDetectorTimerHandle);
			}
			massDisconnectDetectorTimerHandle = 0;
		}
		CommMsgBody dummyMsg;
		massDisconnectDetectorTimerHandle = factorySrvObj->postTimerMessage(_MASS_PROCESS_DISCONNECT_MONITOR, dummyMsg, massCrashTimeThreshold);
		PTRACE5("CommFactoryServerBase::processSubFactoryDisconnected: posted process disconnect monitor timer %u", massDisconnectDetectorTimerHandle);
	}
}

void CommFactoryServerBase::processSubFactoryObjectDead( CommServerConnection* conn, const char* subFactoryName, const char* objectName )
{
	_SubFactory* sf = _findSubFactory( subFactoryName );
	PASSERT3( sf );
	PASSERT3( sf->conn == conn );

	int n = sf->objects.size();
	for( int i=0; i < n ; ++i )
	{
		_SubFactoryItem* sfi = sf->objects[ i ];
		if( strcmp( sfi->name, objectName ) == 0 )
		{
			PTRACE3( "CommFactoryServerBase::processSubFactoryObjectDead: deleting object '%s'", objectName );
			delete sfi;
			sf->objects.erase( sf->objects.begin() + i );//safe only because of return
			if (sf->objects.size())
				return;
			sf->state=_SubFactory::eSubFactoryKilledState;
			sf->process->kill();
			return;
		}
	}
	PLog( "CommFactoryServerBase::processSubFactoryObjectDead: object '%s' not found", objectName );
}

const char * CommFactoryServerBase::findLogFileName( CommServerConnection* conn )
{
	int n = subFactories.size();
	for( int i=0; i < n ; ++i )
	{
		_SubFactory& sf = subFactories[ i ];
		if( sf.conn == conn )
		{
			return sf.logFile.c_str();
		}
	}
	return 0;
}
const char * CommFactoryServerBase::findLogFileName( const char * objectName)
{
	if( !routingTable->localNameExists( objectName ) )
	{
		_printLogFileName("Object %s, log file not found in routing table",objectName);
		return 0;
	}
	int n = subFactories.size();
	for( int i=0; i < n ; ++i )
	{
		_SubFactory& sf = subFactories[ i ];
		int m = sf.objects.size();
		for( int j=0; j < m ; ++j )
		{
			_SubFactoryItem* sfi = sf.objects[ j ];
			if( sfi->name.equals(objectName))
			{
				_printLogFileName("Object %s, log file name: %s",objectName,sf.logFile.c_str());
				return sf.logFile.c_str();
			}
		}
	}
	_printLogFileName("Object %s, log file not found",objectName);
	return 0;
}

void CommFactoryServerBase::processSubFactoryCreateReply( CommServerConnection* conn, UINT32 reqId, int errCode )
{
	int n = subFactories.size();
	for( int i=0; i < n ; ++i )
	{
		_SubFactory& sf = subFactories[ i ];
		if( sf.conn == conn )
		{
			int m = sf.objects.size();
			for( int j=0; j < m ; ++j )
			{
				_SubFactoryItem* sfi = sf.objects[ j ];
				if( sfi->reqId == reqId )
				{
					if( sfi->state == 1 )
					{
						if( errCode )
						{
							PLog( "CommFactoryServerBase::processSubFactoryCreateReply: error creating object '%s'", sfi->name.c_str() );
							delete sfi;
							sf.objects.erase( sf.objects.begin() + j );//safe only because of return
							if (!sf.objects.size())
							{
								sf.state=_SubFactory::eSubFactoryKilledState;
								sf.process->kill();
							}
						}
						else
						{
							PTRACE3( "CommFactoryServerBase::processSubFactoryCreateReply: object '%s' created", sfi->name.c_str() );
							sfi->state = 2;
						}
					}
					else
						PLog( "CommFactoryServerBase::processSubFactoryCreateReply: unexpected state=%d", sfi->state );
					return;
				}
			}
			PLog( "CommFactoryServerBase::processSubFactoryCreateReply: reqId not found" );
			return;
		}
	}
	PLog( "CommFactoryServerBase::processSubFactoryCreateReply: factory not found for connection" );
}

//****************************************************************************




_CommFactorySubFactoryServerConnection::_CommFactorySubFactoryServerConnection( _CommFactoryServerObject* obj_ )
{
	obj = obj_;
}

void _CommFactorySubFactoryServerConnection::processMessage( UINT32 msgId, const CommMsgBody& body )
{
	obj->_processSubFactoryMessage( this, msgId, body );
}

_CommFactorySubFactoryServerConnection::~_CommFactorySubFactoryServerConnection()
{
	obj->_subFactoryDisconnected( this, false );
}

_CommFactoryServerSubFactoryConnFactory::_CommFactoryServerSubFactoryConnFactory( CommServerGuardFactory& guardFactory )
: CommServerConnectionFactory( guardFactory )
{
	obj = 0;
	//normalPriority = 1;//essential. See comment in commlocalapp.cpp
}

/* virtual */ /* new */ CommServerConnection* _CommFactoryServerSubFactoryConnFactory::createConnection()
{
	return new _CommFactorySubFactoryServerConnection( obj );
}

void _CommFactorySubFactoryServerConnection::closedOrDisconnected(int errCode, const char* errMsg)
{
	if (errCode == _COMM_ERROR_PHYSICAL_DISCONNECTED)
	{
		obj->_subFactoryDisconnected( this, true );
	}
	else
	{
		PLog("closedOrDisconnected - errCode=%d, do nothing",errCode);
	}
}

//****************************************************************************

_CommFactoryServerConnection::_CommFactoryServerConnection( _CommFactoryServerObject* obj_ )
:CommServerConnection()
{
	obj = obj_;
}

void _CommFactoryServerConnection::processMessage( UINT32 msgId, const CommMsgBody& body )
{
	obj->_processConnMessage( this, msgId, body );
}

_CommFactoryServerConnectionEx::_CommFactoryServerConnectionEx(_CommFactoryServerObject* obj_)
{
	obj = obj_;
}

void _CommFactoryServerConnectionEx::processMessage(UINT32 msgId, const CommMsgBody& body)
{
	switch (msgId)
	{
		case COMMAPPFACTORY_CREATEOBJECT:
		{
			obj->_processConnMessage(this, msgId, body);
		}
		break;

		default:
		{
			PLog("AppFactoryEx::processMessage: unknown msgId=%d(%x)", msgId, msgId);
		}
		break;
	}
}

_CommFactoryServerConnFactory::_CommFactoryServerConnFactory( CommServerGuardFactory& guardFactory )
: CommServerConnectionFactory( guardFactory )
{
	obj = 0;
}

/* virtual */ /* new */ CommServerConnection* _CommFactoryServerConnFactory::createConnection()
{
	return new _CommFactoryServerConnection( obj );
}

_CommFactoryServerConnFactoryEx::_CommFactoryServerConnFactoryEx(CommServerGuardFactory& guardFactory)
	: CommServerConnectionFactory(guardFactory)
{
	obj = 0;
}

/* virtual */ /* new */ CommServerConnection* _CommFactoryServerConnFactoryEx::createConnection()
{
	return new _CommFactoryServerConnectionEx(obj);
}

//****************************************************************************

/* static */ CommServerNullGuardFactory _CommFactoryServerObject::guardFactory;
/* static */ CommClientNullGuardFactory _CommFactoryServerObject::clientGuardFactory;

_CommFactoryServerObject::_CommFactoryServerObject( _CommInterface& inter )
: CommServerObject( inter ),
connFactory( guardFactory ),
connFactoryEx(guardFactory),
subFactoryConnFactory( guardFactory ),
srvPool( inter ),
cliPool( inter ),
keepAliveConn( clientGuardFactory )
{
	hLogger = 0;
	base = 0;
	numberOfSEH=0;
	numberOfAsserts=0;
	numberOfMemoryFailure=0;
	connFactory.obj = this;
	connFactoryEx.obj = this;
	subFactoryConnFactory.obj = this;
	srvPool.registerFactory( "", connFactory );
	srvPool.registerFactory( "ex", connFactoryEx );
	srvPool.registerFactory( "subfactory", subFactoryConnFactory );
}

/* virtual */ void _CommFactoryServerObject::processMessage( _CommMsg& msg )
{
	srvPool._processMessage( msg );
	cliPool._processMessage( msg );
}

/* virtual */ void _CommFactoryServerObject::init( CommMsgBody& msg )
{
	PASSERT3( base == 0 );
	CommMsgParser parser( msg );
	const BYTE* b;
	parser._parseFixedBlock( b, sizeof(CommFactoryServerBase*) );
	base = *(CommFactoryServerBase**)b;

	if( *base->keepAliveServer && *base->keepAliveServerObject )
		cliPool.connect( keepAliveConn, base->keepAliveServer, base->keepAliveServerObject );

	CommMsgBody dummy;
	postTimerMessage( _TIMER_POLL, dummy, _DISPATCHERKEEPALIVEPOLL );

	if (base->hLoggerServer.length())
	{
		hLogger = new CommMonitorLoggerObject();
		hLogger->init(cliPool,base->hLoggerServer,base->hLoggerStation);
		CommMsgBody dummy;
		postTimerMessage( _TIMER_LOGGER, dummy, _DISPATCHERLOGGERPOLL );
	}
}

/* virtual */ void _CommFactoryServerObject::processTimerMessage( UINT32 handle, UINT32 msgId, CommMsgBody& msgBody )
{
	switch( msgId )
	{
	case _TIMER_POLL:
		{
			if( keepAliveConn.isConnected() )
			{
				CommMsgBody keepAlive;
				keepAlive.composeString( base->keepAliveId );
				keepAliveConn.post( _COMMAPPFACTORY_KEEPALIVE, keepAlive );//don't care whether really connected or not
			}

			base->timer();

			CommMsgBody dummy;
			postTimerMessage( _TIMER_POLL, dummy, _DISPATCHERKEEPALIVEPOLL );
		}
		break;

	case _TIMER_LOGGER:
		{
			if (hLogger)
			{
				CommMsgBody body;
				CommMsgBody logBody;
				base->routingTable->loggerInformation(body);
				base->reportModuleAutoRestartStats(body);
				CommMsgParser parser(body);
				bool haveSomethingToReport = false;

				if (base->reportSEH)
				{
					PString realName;
					if (base->hLoggerSuffix.length())
					{
						realName.assign(base->hLoggerSuffix).append("Number of SEH");
					}
					else
					{
						realName.assign("Number of SEH");
					}
					char buffer[40];
					sprintf(buffer,"%d", numberOfSEH);
					hLogger->compose(logBody,realName,buffer);
					if (base->hLoggerSuffix.length())
					{
						realName.assign(base->hLoggerSuffix).append("Total number of Asserts");
					}
					else
					{
						realName.assign("Total number of Asserts");
					}
					sprintf(buffer,"%d", numberOfAsserts);
					hLogger->compose(logBody,realName,buffer);
					if (base->hLoggerSuffix.length())
					{
						realName.assign(base->hLoggerSuffix).append("Total number of Memory Failurs");
					}
					else
					{
						realName.assign("Total number of Memory Failurs");
					}
					sprintf(buffer,"%d", numberOfMemoryFailure);
					hLogger->compose(logBody,realName,buffer);

					haveSomethingToReport = true;
				}

				while(parser.parseEnded() == false)
				{
					const char * name;
					BYTE type;
					parser.parseBYTE(type).parseString(name);
					PString realName;
					if (base->hLoggerSuffix.length())
					{
						realName.assign(base->hLoggerSuffix).append(name);
					}
					else
					{
						realName.assign(name);
					}
					if (type == CommMonitorLoggerObject::BoolType)
					{
						bool flag;
						parser.parseBOOL(flag);
						hLogger->compose(logBody,realName,(flag?HERCULES_PARAMETER_UNAVAILABLE:HERCULES_PARAMETER_PRESENT));
						haveSomethingToReport = true;
					}
                    else if (type == CommMonitorLoggerObject::StringType)
                    {
                        const char* parameter;
                        parser.parseString(parameter);
                        hLogger->compose(logBody,realName,parameter);
                        haveSomethingToReport = true;
                    }
					else if (type == CommMonitorLoggerObject::UIntType)
					{
						UINT32 parameter;
						parser.parseUINT32(parameter);
						char buffer[40];
						sprintf(buffer,"%u", parameter);
						hLogger->compose(logBody,realName,buffer);
						haveSomethingToReport = true;
					}
					else if (type == CommMonitorLoggerObject::IntType)
					{
						INT32 parameter;
						parser.parseINT32(parameter);
						char buffer[40];
						sprintf(buffer, "%d", parameter);
						hLogger->compose(logBody, realName, buffer);
						haveSomethingToReport = true;
					}
					else
					{
						PLog("faulty logic : unexpected logger parameter type: %d", type);
					}
				}

				if (haveSomethingToReport)
				{
					hLogger->post(logBody);
				}

				CommMsgBody dummy;
				postTimerMessage( _TIMER_LOGGER, dummy, _DISPATCHERLOGGERPOLL );
			}

		}
		break;

		case _PROCESS_RESTART:
		{
			CommMsgParser parser(msgBody);
			const char *objectType, *objectName;
			int numRestartsRemaining;
			CommMsgBody initMsg;
			parser.parseString(objectType).parseString(objectName).parseINT32(numRestartsRemaining).parseMsgBody(initMsg);
			base->tryRestartProcess(objectType, objectName, numRestartsRemaining, initMsg);
		}
		break;

		case _RESET_PROCESS_RESTART_PARAMS:
		{
			CommMsgParser parser(msgBody);
			const char* factoryName;
			int numRestarts;
			parser.parseString(factoryName).parseINT32(numRestarts);
			base->resetRestartParams(factoryName, numRestarts);
		}
		break;

		case _MASS_PROCESS_DISCONNECT_MONITOR:
		{
			PTRACE5("_CommFactoryServerObject::processTimerMessage: process disconnect monitor timer %u fired", base->massDisconnectDetectorTimerHandle);
			base->massDisconnectDetectorTimerHandle = base->lastProcessDisconnectTime = base->numProcessesDisconnected = 0;
		}
		break;
	}
}

void _CommFactoryServerObject::_processConnMessage( CommServerConnection* conn, UINT32 msgId, const CommMsgBody& msg )
{
	switch( msgId )
	{
	case _COMMAPPFACTORY_KEEPALIVE:
		{
			CommMsgParser parser( msg );
			const char* keepAliveId;
			parser.parseString( keepAliveId );
			base->gotKeepAlive( keepAliveId );
		}
		break;

	case COMMAPPFACTORY_CREATEOBJECT:
		{
			CommMsgParser parser( msg );
			UINT32 reqId;
			const char* objectType;
			const char* objectName;
			CommMsgBody initMsg;
			parser.parseUINT32( reqId ).parseString( objectType ).parseString( objectName ).parseMsgBody( initMsg );

			PString errMsg;
			int err = base->createObject( errMsg, objectType, objectName, initMsg );
			CommMsgBody reply;
			reply.composeUINT32( reqId );
			if( err == COMMAPPFACTORY_ERROR_OK )
				reply.composeUINT16( err );
			else
				reply.composeUINT16( err ).composeString( errMsg );
			conn->post( COMMAPPFACTORY_CREATEOBJECT_REPLY, reply );
		}
		break;
	case COMMAPPFACTORY_LOGFILE:
		{
			CommMsgParser parser( msg );
			UINT32 reqId;
			const char* objectName;
			parser.parseUINT32( reqId ).parseString( objectName );
			const char * logFimeName = base->findLogFileName(objectName);
			CommMsgBody reply;
			reply.composeUINT32( reqId );
			if (logFimeName)
			{
				reply.composeUINT16( 0 );
				reply.composeString( logFimeName );
			}
			else
			{
				reply.composeUINT16( -1 );
				reply.composeString( "Object not found" );
			}
			conn->post( COMMAPPFACTORY_LOGFILE_REPLY, reply );
		}
		break;

	case COMMAPPFACTORY_DUMP:
		{
			CommMsgParser parser( msg );
			UINT32 reqId;
			parser.parseUINT32( reqId );
			base->dump();
			CommMsgBody reply;
			reply.composeUINT32( reqId );
			conn->post( COMMAPPFACTORY_DUMP_REPLY, reply );
		}
		break;

	case COMMAPPFACTORY_CHANGE_EXE_NAME:
		{
			CommMsgParser parser( msg );
			UINT32 reqId;
			parser.parseUINT32( reqId );
			const char * type;
			const char * name;
			parser.parseString(type)
				.parseString(name);

			PString errMsg;
			INT16 errCode = base->changeExeName(errMsg,type, name );
			CommMsgBody reply;
			reply.composeUINT32( reqId )
				.composeINT16(errCode);
			if (errCode)
				reply.composeString(errMsg);

			conn->post( COMMAPPFACTORY_CHANGE_EXE_NAME_REPLY, reply );
		}
		break;


	case COMMAPPFACTORY_DUMP_EX:
		{
			CommMsgParser parser( msg );
			UINT32 reqId;
			parser.parseUINT32( reqId );
			base->dumpEx(parser);
			CommMsgBody reply;
			reply.composeUINT32( reqId );
			conn->post( COMMAPPFACTORY_DUMP_REPLY_EX, reply );
		}
		break;

	case COMMAPPFACTORY_DYNAMIC_INIT:
		{
			CommMsgParser parser( msg );
			UINT32 reqId;
			parser.parseUINT32( reqId );
			PString errMsg;
			int err=COMMAPPFACTORY_ERROR_OK;
			try
			{
				base->dynamicInit();
				numberOfSEH = 0;
				numberOfAsserts = 0;
				numberOfMemoryFailure=0;
			}
			catch(PError &er)
			{
				PLog( "COMMAPPFACTORY_DYNAMIC_INIT caught: %s", er.why() );
				errMsg = er.why();
				err=COMMAPPFACTORY_ERROR_INITFAILED;
			}
			CommMsgBody reply;
			reply.composeUINT32( reqId )
				.composeUINT16( err);
			if (err != COMMAPPFACTORY_ERROR_OK)
			{
				reply.composeString( errMsg );
			}
			conn->post( COMMAPPFACTORY_DYNAMIC_INIT_REPLY, reply );
		}
		break;
	case COMMAPPFACTORY_LOGFILE_REOPEN:
		{
			CommMsgParser parser( msg );
			UINT32 reqId;
			parser.parseUINT32( reqId );
			INT16 ret = PLogReopen() ? 0:1;
			CommMsgBody reply;
			reply.composeUINT32( reqId )
				 .composeINT16(ret);
			conn->post( COMMAPPFACTORY_LOGFILE_REOPEN_REPLY, reply );
		}
		break;
	case COMMAPPFACTORY_REOPEN_OBJECT_LOGFILE:
		{
			CommMsgParser parser( msg );
			UINT32 reqId;
			UINT32 pId;
			parser.parseUINT32( reqId )
				  .parseUINT32(pId);
			CommMsgBody reply;
			reply.composeUINT32( reqId );
			base->_reopenObjectsLogfile(pId,reply);
			conn->post( COMMAPPFACTORY_REOPEN_OBJECT_LOGFILE_REPLY, reply );
		}
		break;

	case COMMAPPFACTORY_GET_OBJECT_BY_PID:
		{
			CommMsgParser parser( msg );
			UINT32 reqId;
			UINT32 pId;
			parser.parseUINT32( reqId )
				  .parseUINT32(pId);
			CommMsgBody reply;
			reply.composeUINT32( reqId );
			base->_findObjects(pId,reply);
			conn->post( COMMAPPFACTORY_GET_OBJECT_BY_PID_REPLY, reply );
		}
		break;
	case COMMAPPFACTORY_CHANGE_LOGFILE_DIR:
		{
			CommMsgParser parser( msg );
			UINT32 reqId;
			parser.parseUINT32( reqId );
			const char * type;
			const char * logDir;
			parser.parseString(type)
				.parseString(logDir);

			PString errMsg;
			INT16 errCode = base->changeLogFileDir(errMsg,type, logDir );
			CommMsgBody reply;
			reply.composeUINT32( reqId )
				.composeINT16(errCode);
			if (errCode)
				reply.composeString(errMsg);

			conn->post( COMMAPPFACTORY_CHANGE_LOGFILE_DIR_REPLY, reply );
		}
		break;
	case COMMAPPFACTORY_GET_PHYS_CONNECTIONS_STATUS:
		{
			CommMsgParser parser( msg );
			UINT32 reqId;
			parser.parseUINT32( reqId );
			CommMsgBody reply;
			reply.composeUINT32(reqId);
			base->getPhysConnections(reply);
			conn->post( COMMAPPFACTORY_GET_PHYS_CONNECTIONS_STATUS_REPLY, reply );
		break;
		}
	default:
		PLog("AppFactory::processMessage: unknown msgId=%d(%x)",msgId,msgId);
		break;

	}
}

void _CommFactoryServerObject::_processSubFactoryMessage( _CommFactorySubFactoryServerConnection* conn, UINT32 msgId, const CommMsgBody& msg )
{
	switch( msgId )
	{
	case _COMMAPPFACTORY_SUBFACTORY_CONNECTED:
		{
			CommMsgParser parser( msg );
			const char* subFactoryName;
			parser.parseString( subFactoryName );

			vector< PString > objects;
			for(;;)
			{
				const char* objectName;
				parser.parseString( objectName );
				if( *objectName == 0 )
					break;
				objects.push_back( objectName );
			}
			base->processSubFactoryConnected( conn, subFactoryName, objects );
		}
		break;

	case _COMMAPPFACTORY_SUBFACTORY_OBJECTDEAD:
		{
			CommMsgParser parser( msg );
			const char* subFactoryName;
			const char* objectName;
			parser.parseString( subFactoryName ).parseString( objectName );
			base->processSubFactoryObjectDead( conn, subFactoryName, objectName );
		}
		break;

	case _COMMAPPFACTORY_SUBFACTORY_CREATEOBJECT_REPLY:
		{
			CommMsgParser parser( msg );
			UINT32 reqId;
			INT16 errCode;
			parser.parseUINT32( reqId ).parseINT16( errCode );
			base->processSubFactoryCreateReply( conn, reqId, errCode );
		}
		break;
	case _COMMAPPFACTORY_SUBFACTORY_SEH_NUMBER_UPDATE:
		{
			CommMsgParser parser( msg );
			UINT32 reqId;
			INT32  number;
			INT32  numberAsserts=0;
			INT32 numberMemoryFailure=0;
			parser.parseUINT32( reqId )
				.parseINT32(number);
			if (!parser.parseEnded())
			{
				parser.parseINT32(numberAsserts);
				if (!parser.parseEnded())
				{
					parser.parseINT32(numberMemoryFailure);
				}
			}
			numberOfSEH += number;
			numberOfAsserts += numberAsserts;
			numberOfMemoryFailure += numberMemoryFailure;
			if (number || numberAsserts || numberMemoryFailure)
			{
				const char * logFimeName = base->findLogFileName(conn);
				PLog("SEH/ASSERT/MEMORY FAILURE appeared in object: SEH=%d, ASSERT=%d, Memory failure=%d, log file: '%s'",number, numberAsserts, numberMemoryFailure, (logFimeName ? logFimeName: "Unknown"));  
			}
		}
		break;


	}
}

void _CommFactoryServerObject::_subFactoryDisconnected( _CommFactorySubFactoryServerConnection* conn, bool allowAutoRestart )
{
	base->processSubFactoryDisconnected( conn, allowAutoRestart );
}

void _CommFactoryServerObject::shutdown()
{
	base->killExes();
	base->waitForExes();
}

void CommFactoryServerBase::dynamicInit()
{
	PIniFile iniFile( iniFullFileName.c_str() );

	// find mux ports
	PString unixMuxName;
	int tcpMuxPort =-1;
	int smemMuxPort = -1;

	int i,n;
	const PIniFile::Section* section = iniFile.getSection( "managers" );
	if (section)
	{
		n = section->items.size();

		for( i=0; i < n ; ++i )
		{
			const PIniFile::Item& item = section->items[ i ];
			if( item.name.equalsIgnoreCase( "manager" ) )
			{
				const PIniFile::Section* manSection = iniFile.getSection( item.value );
				if (!manSection)
				{
					PLog(" CommFactoryServerBase::dynamicInit() - section manager='%s' not found", item.value.c_str());
					return;
				}
				const  char * type = manSection->getProperty( "type");
				if (!type)
				{
					PLog(" CommFactoryServerBase::dynamicInit() - 'type' not found in section '%s'", manSection->name.c_str());
					return;
				}
				if (!stricmp(type,"unixmux") && unixMuxName.length() == 0)
				{
					unixMuxName =  manSection->getProperty("port");
				}
				else if (!stricmp(type,"tcpmux") && tcpMuxPort < 0)
				{
					tcpMuxPort =  manSection->getIntProperty("port",-1);
				}
				else if (!stricmp(type,"smemmux") && smemMuxPort < 0)
				{
					smemMuxPort =  manSection->getIntProperty("port",-1);
				}
			}
		}
	}
	else
	{
		section = iniFile.getSection( "tcpmux" );
		if( section )
		{
			tcpMuxPort =  section->getIntProperty("port",-1);
		}
		section = iniFile.getSection( "smemmux" );
		if( section )
		{
			smemMuxPort =  section->getIntProperty("port",-1);
		}
	}

	section  = iniFile.getSection( "objectfactories" );
	if (section)
	{
		n = section->items.size();

		for( i=0; i < n ; ++i )
		{
			const PIniFile::Item& item = section->items[ i ];
			if( item.name.equalsIgnoreCase( "objectfactory" ) )
			{
				const PIniFile::Section* objectSection = iniFile.getSection( item.value );
				if (!objectSection)
				{
					PLog(" CommFactoryServerBase::dynamicInit() - section objectfactory='%s' not found", item.value.c_str());
					return;
				}
				const  char * type = objectSection->getProperty( "type");
				if (!type)
				{
					PLog(" CommFactoryServerBase::dynamicInit() - 'type' not found in section '%s'", objectSection->name.c_str());
					return;
				}

				int muxPort = objectSection->getIntProperty( "port",tcpMuxPort);

#ifdef PPOSIX
				const char * muxName = objectSection->getProperty( "name" , unixMuxName.c_str());
#endif
				int nObject = objectSection->items.size();
				for( int j=0; j < nObject ; ++j )
				{
					const PIniFile::Item& item = objectSection->items[ j ];
					if (stricmp(item.name,"type") && stricmp(item.name,"port")  && stricmp(item.name,"name"))
					{
						PString exeName, logName;
						int maxNumRestarts = 0, minInterRestartPeriod = 0;
						parseExeLine(item, exeName, logName, maxNumRestarts, minInterRestartPeriod);
						if (!stricmp(type,"multiexe"))
						{
							if (muxPort<=0)
							{
								PLog(" CommFactoryServerBase::dynamicInit() - muxPort=%d in section '%s'", muxPort, objectSection->name.c_str());
								return;
							}
							if (registerMultiExeObjectFactory( item.name, exeName,logName, muxPort, (maxNumRestarts && minInterRestartPeriod ? &maxNumRestarts : nullptr), (maxNumRestarts && minInterRestartPeriod ? &minInterRestartPeriod : nullptr) ))
								PLog( "MultiObject factory '%s', muxPort=%d, executable '%s' registered", item.name.c_str(),muxPort,exeName.c_str() );
						}
						else if(!stricmp(type,"exe"))
						{
							if (muxPort<=0)
							{
								PLog(" CommFactoryServerBase::dynamicInit() - muxPort=%d in section '%s'", muxPort, objectSection->name.c_str());
								return;
							}
							if(registerExeObjectFactory( item.name, exeName, logName, muxPort, (maxNumRestarts && minInterRestartPeriod ? &maxNumRestarts : nullptr), (maxNumRestarts && minInterRestartPeriod ? &minInterRestartPeriod : nullptr) ))
								PLog( "Object factory '%s', muxPort=%d, executable '%s'  registered", item.name.c_str(),muxPort,exeName.c_str() );
						}
#ifdef PPOSIX
						else if (!stricmp(type,"unixmultiexe"))
						{
							if (strlen(muxName) == 0)
							{
								PLog(" CommFactoryServerBase::dynamicInit() - muxName is empty in section '%s'", objectSection->name.c_str());
								return;
							}
							if(registerMultiExeObjectFactory( item.name, exeName,logName, muxName ))
								PLog( "MultiObject factory '%s', muxName=%s, executable '%s' registered", item.name.c_str(),muxName,exeName.c_str() );
						}
						else if(!stricmp(type,"unixexe"))
						{
							if (strlen(muxName) == 0)
							{
								PLog(" CommFactoryServerBase::dynamicInit() - muxName is empty in section '%s'", objectSection->name.c_str());
								return;
							}
							if(registerExeObjectFactory( item.name, exeName, logName, muxName ))
								PLog( "Object factory '%s', muxName=%s, executable '%s'  registered", item.name.c_str(),muxName,exeName.c_str() );
						}
#endif
						else
						{
							PLog(" CommFactoryServerBase::dynamicInit() - unknown parameter type='%s' in section '%s'", type, objectSection->name.c_str());
							return;
						}
					}
				}
			}
		}
	}			
	else
	{
#ifdef PWIN32
		section = iniFile.getSection( "smemmultiexe" );
		if( section )
		{
			int i,n = section->items.size();
			for( i=0; i < n ; ++i )
			{
				const PIniFile::Item& item = section->items[ i ];
				if (smemMuxPort<=0)
				{
					PLog(" CommFactoryServerBase::dynamicInit() - smemMuxPort=%d",smemMuxPort);
					return;
				}
				PString exeName, logName;
				int maxNumRestarts = 0, minInterRestartPeriod = 0;
				parseExeLine(item, exeName, logName, maxNumRestarts, minInterRestartPeriod);
				if (registerMultiExeObjectFactory( item.name, exeName,logName, smemMuxPort, (maxNumRestarts && minInterRestartPeriod ? &maxNumRestarts : nullptr), (maxNumRestarts && minInterRestartPeriod ? &minInterRestartPeriod : nullptr) ))
					PLog( "SmemMultiObject factory '%s', executable '%s' registered", item.name.c_str(),exeName.c_str() );
			}
		}
#endif
		section = iniFile.getSection( "multiexe" );
		if( section )
		{
			n = section->items.size();
			for( i=0; i < n ; ++i )
			{
				const PIniFile::Item& item = section->items[ i ];
				if (tcpMuxPort<=0)
				{
					PLog(" CommFactoryServerBase::dynamicInit() - tcpMuxPort=%d",tcpMuxPort);
					return;
				}
				PString exeName, logName;
				int maxNumRestarts = 0, minInterRestartPeriod = 0;
				parseExeLine(item, exeName, logName, maxNumRestarts, minInterRestartPeriod);
				if(registerMultiExeObjectFactory( item.name, exeName,logName, tcpMuxPort, (maxNumRestarts && minInterRestartPeriod ? &maxNumRestarts : nullptr), (maxNumRestarts && minInterRestartPeriod ? &minInterRestartPeriod : nullptr) ))
					PLog( "MultiObject factory '%s', executable '%s' registered", item.name.c_str(),exeName.c_str() );
			}
		}
		section = iniFile.getSection( "exe" );
		if( section )
		{
			n = section->items.size();
			for( i=0; i < n ; ++i )
			{
				const PIniFile::Item& item = section->items[ i ];
				if (tcpMuxPort<=0)
				{
					PLog(" CommFactoryServerBase::dynamicInit() - tcpMuxPort=%d",tcpMuxPort);
					return;
				}
				PString exeName, logName;
				int maxNumRestarts = 0, minInterRestartPeriod = 0;
				parseExeLine(item, exeName, logName, maxNumRestarts, minInterRestartPeriod);
				if(registerExeObjectFactory( item.name, exeName, logName, tcpMuxPort, (maxNumRestarts && minInterRestartPeriod ? &maxNumRestarts : nullptr), (maxNumRestarts && minInterRestartPeriod ? &minInterRestartPeriod : nullptr) ))
					PLog( "Object factory '%s', executable '%s' registered", item.name.c_str(),exeName.c_str() );
			}
		}
#ifdef PPOSIX
		section = iniFile.getSection( "unixmultiexe" );
		if( section )
		{
			n = section->items.size();
			for( i=0; i < n ; ++i )
			{
				const PIniFile::Item& item = section->items[ i ];
				if (strlen(unixMuxName) == 0)
				{
					PLog(" CommFactoryServerBase::dynamicInit() - unixMuxName is empty");
					return;
				}
				PString exeName;
				PString logName;
				const char* sc = strchr( item.value, ';' );
				if( sc )
				{
					exeName.assign( item.value, sc );
					logName = sc + 1;
				}
				else
				{
					exeName = item.value;
				}

				if(registerMultiExeObjectFactory( item.name, exeName,logName, unixMuxName.c_str() ))
					PLog( "MultiObject factory '%s', executable '%s' registered", item.name.c_str(),exeName.c_str() );
			}
		}
		section = iniFile.getSection( "unixexe" );
		if( section )
		{
			n = section->items.size();
			for( i=0; i < n ; ++i )
			{
				const PIniFile::Item& item = section->items[ i ];
				if (strlen(unixMuxName) == 0)
				{
					PLog(" CommFactoryServerBase::dynamicInit() - unixMuxName is empty");
					return;
				}
				PString exeName;
				PString logName;
				const char* sc = strchr( item.value, ';' );
				if( sc )
				{
					exeName.assign( item.value, sc );
					logName = sc + 1;
				}
				else
				{
					exeName = item.value;
				}

				if(registerExeObjectFactory( item.name, exeName, logName, unixMuxName.c_str() ))
					PLog( "Object factory '%s', executable '%s' registered", item.name.c_str(),exeName.c_str() );
			}
		}
#endif 

#ifdef PWIN32
		section = iniFile.getSection( "smemexe" );
		if( section )
		{
			n = section->items.size();
			for( i=0; i < n ; ++i )
			{
				const PIniFile::Item& item = section->items[ i ];
				if (smemMuxPort<=0)
				{
					PLog(" CommFactoryServerBase::dynamicInit() - smemMuxPort=%d",smemMuxPort);
					return;
				}
				PString exeName, logName;
				int maxNumRestarts = 0, minInterRestartPeriod = 0;
				parseExeLine(item, exeName, logName, maxNumRestarts, minInterRestartPeriod);
				if(registerExeObjectFactory( item.name, exeName,logName, smemMuxPort, (maxNumRestarts && minInterRestartPeriod ? &maxNumRestarts : nullptr), (maxNumRestarts && minInterRestartPeriod ? &minInterRestartPeriod : nullptr) ))
					PLog( "Object factory '%s', executable '%s' registered", item.name.c_str(),exeName.c_str() );
			}
		}
#endif
	}

	routingTable->dynamicInit(iniFile);

	{
		PLock lock(cs);
		for (size_t i = 0; i < subFactories.size(); ++i)
		{
			_SubFactory& sf = subFactories[i];
			_Types::iterator found = types.find(sf.type);
			if (found == types.cend())
			{
				continue;
			}
			const auto& type = found->second;
			if (!sf.objectName.length())
			{
				continue;
			}
			auto sfrdIt = subFactoryRestartDataMap.find(sf.objectName);
			if (sfrdIt == subFactoryRestartDataMap.end())
			{
				continue;
			}
			_SubFactoryRestartData& sfrd = sfrdIt->second;
			sfrd.numRestartsRemaining = type.numRestarts;
			PTRACE3("CommFactoryServerBase::dynamicInit: numRestartsRemaining for '%s' reset to %d", sf.name.c_str(), sfrd.numRestartsRemaining);
			if (sfrd.restartTimerHandle)
			{
				factorySrvObj->deleteTimerMessage(sfrd.restartTimerHandle);
				int autoRestartParamsResetTimerFreq = routingTable->getAutoRestartParamsResetTimerFreq();
				if (autoRestartParamsResetTimerFreq)
				{
					CommMsgBody restartData;
					restartData.composeString(sf.name).composeINT32(type.numRestarts);
					int newTimerTimeout = std::max(static_cast<INT32>(1), static_cast<INT32>(autoRestartParamsResetTimerFreq - (SYSTEMTICKS() - sfrd.restartTimerSubmitTime)));
					sfrd.restartTimerHandle = factorySrvObj->postTimerMessage(_RESET_PROCESS_RESTART_PARAMS, restartData, newTimerTimeout);
					sfrd.restartTimerSubmitTime = SYSTEMTICKS();
					PTRACE5("CommFactoryServerBase::dynamicInit: _RESET_PROCESS_RESTART_PARAMS timer for '%s' resubmitted (set to fire in %d ms)", sf.name.c_str(), newTimerTimeout);
				}
			}
		}
	}
}
