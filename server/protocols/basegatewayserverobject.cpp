#include	"basegatewayserverobject.h"
#include    "dbm.h"
#include    "aamsgateway.h"
#include    "admincommon.h"
#include    "AuthRedirector.h"
#include    "shared.h"

#define TIMER_LOGGER_MSEC               5000
#define DBM_TIMER_LOGGER                4
#define TIMER_SHUTDOWN_LOGGER_MSEC      5000
#define DBM_SHUTDOWN_TIMER_LOGGER       5

/* static */ CommServerNullGuardFactory BaseGatewayServerObject::nullGuardFactoryServer;
/* static */ CommClientNullGuardFactory BaseGatewayServerObject::nullGuardFactoryClient;

BaseGatewayServerObject::BaseGatewayServerObject(_CommInterface& inter_)
		: CommServerObject (inter_)
		, srvPool          (inter_)
		, cliPool          (inter_)
		, appFactoryConn          (nullGuardFactoryClient)
        , trustedAdminConnFactory (nullGuardFactoryServer)
{
	PLog("BaseGatewayServerObject::ctor %p", this);

	appFactoryConn.init(this);
	trustedAdminConnFactory.init(this);

	emulationMode = false;
	emulationDelay = 0;
	numModules = 0;
	hasTransportConnections = false;

	useLogger = false;
	heLogger = NULL;
	heLoggerTimerHandle = 0;

	isPreparingForShutdown = false;
	isReadyForShutdown = false;
	shutdownTimerHandle = 0;
	overspanLimit = 0;

    srvPool.registerFactory( "trustedAdmin", trustedAdminConnFactory );
}

void BaseGatewayServerObject::init(CommMsgBody& initMsg) // override
{
	CommMsgParser parser(initMsg);
	parser.parseStringP(initDir);
	fullIniFileName.assign(initDir).append(getIniFileName());
	PLog("BaseGatewayServerObject::init(%s)", fullIniFileName.c_str());
	loadAndLogIniFile(iniFile, fullIniFileName); // static

	initBaseSettings();
}

void BaseGatewayServerObject::initBaseSettings() // read SETTINGS and TRANSPORT sections
{
	// LOGGER
    const PIniFile::Section* loggerSection = iniFile.getSection(LOGGER_SECTION_NAME);
    if(loggerSection)
    {
	    useLogger           = loggerSection->getIntProperty("useLogger", 0) != 0;
	    loggerServerAddress = loggerSection->getProperty("LoggerAddress", "");
	    loggerStationName   = loggerSection->getProperty("LogStationName", "");
    }
    else
    {
        PLog("[" LOGGER_SECTION_NAME "] section is missing");
    }
	if(useLogger && loggerServerAddress.length() && loggerStationName.length())
	{
		startHeLogger(loggerServerAddress, loggerStationName);
	}
	else
	{
		useLogger = false;
	}
	PLog("useLogger: '%s'", useLogger ? "yes" : "no");
	PLog("LoggerAddress: '%s'", loggerServerAddress.c_str());
	PLog("LogStationName: '%s'", loggerStationName.c_str());

    // TODO: eventually remove [SETTINGS] section and move everything over
	// to [STATICSETTINGS] and [DYNAMICSETTINGS] depending on parameter
	const PIniFile::Section* settingsSection = iniFile.getSection(SETTINGS_SECTION_NAME);
	if (settingsSection)
	{
		emulationMode = 0 != settingsSection->getIntProperty("emulationMode", 0); // don't want Emulation by default (#19436)
		emulationDelay = settingsSection->getIntProperty("emulationDelay", 0); // don't want EmulationDelay by default (#19436)
	}
	PLog("emulationMode: %s", emulationMode ? "yes" : "no");
	PLog("emulationDelay: %d", emulationDelay);

	if(!emulationMode)
	{
		initBaseTransport(); // read [TRANSPORT]
	}
	else
	{	// make sure Gateway still receives messages even if no real transports defined
		transportModuleConnected(); // To register server factory
	}

	dynamicInit();
}

// virtual
void BaseGatewayServerObject::dynamicInit()
{
	PLog("dynamicInit");
	const PIniFile::Section* settingsSection = iniFile.getSection(DYNASETTINGS_SECTION_NAME);
	if(settingsSection)
    {
    	overspanLimit = settingsSection->getIntProperty("overspan", 1000); // 1 second
    }
	PLog("overspanLimit: %d", overspanLimit);
}

void BaseGatewayServerObject::initBaseTransport()
{
	if (emulationMode)
		return;

	PLog("Reading [" TRANSPORT_SECTION_NAME "]");
	const PIniFile::Section* section = iniFile.getSection(TRANSPORT_SECTION_NAME);
	if (!section)
	{
		throw PError("ERROR: no [" TRANSPORT_SECTION_NAME "] section in ini file" );
	}
	transportModuleSuffix = section->getProperty("transportModuleSuffix", "");
	PLog("transportModuleSuffix: '%s'",  transportModuleSuffix.c_str());
	appFactoryServer      = section->getProperty("factory", "");
	PLog("appFactoryServer: '%s'", appFactoryServer.c_str());
	if(appFactoryServer.length() == 0)
	{
		throw PError("ERROR: factory must be specified in section " TRANSPORT_SECTION_NAME);
	}
	numModules = section->getIntProperty("numModules", 0);
	PLog("numModules = %d", numModules);
	if(numModules == 0 || numModules > 50)
	{
		throw PError("ERROR: must be {0 < numModules <= 50} in section " TRANSPORT_SECTION_NAME);
	}

	cliPool.connect(appFactoryConn, appFactoryServer, "AppFactory");
}

void AppFactoryClientConnection::connected() // override
{
	PLog("Connected to s='%s' o='%s' t='%s'", getServer(), getServerObject(), getServerConnType());
	obj->appFactoryConnected();
}

void BaseGatewayServerObject::appFactoryConnected() // called from AppFactoryClientConnection::connected()
{
	PLog("Creating %d transport module instances...", numModules);
	for( INT32 i = 0; i < numModules; ++i )
	{
		createTransportModule(i, true /*createConnection*/); // calls createTransportModuleImpl()
	}
}

void BaseGatewayServerObject::createTransportModuleImpl( // is called from overriddden createTransportModule()
	const CommMsgBody& moduleMsg,
	INT32 instanceIdx, 
	bool createConnection, 
	const char* qualifierPrefix,
	const char* connectionName, // transport server connection factory type - must match transport module srvPool.registerFactory()
	const char* dispatcherName) // transport exe registration  factory type - must match config parameter name for transport in dispatcher.ini [exe] section
	// dispatcherName must be given to config administrator to set it in dispatcher.ini
{
	if (instanceIdx < 0)
		instanceIdx = transportConnections.size();

	PString qualifier;
	qualifier.assign(qualifierPrefix).append(transportModuleSuffix).append('.').appendUint(instanceIdx);

	CommMsgBody initBody;
	initBody
		.composeUINT32(instanceIdx)    // requestId - dispatcher, thinking it might be G-connection, will return it back on reply
		.composeString(dispatcherName) // exe factory type
		.composeString(qualifier)      // transport server object instance name
		.composeMsgBody(moduleMsg);    // initMsgBody

	// COMMAPPFACTORY_CREATEOBJECT: 4ssB = reqId, objectType, serverObjectInstanceName, initMsg
	appFactoryConn.post( COMMAPPFACTORY_CREATEOBJECT, initBody );
	PLog("COMMAPPFACTORY_CREATEOBJECT: instanceIdx=%d instanceName='%s'", instanceIdx, qualifier.c_str());

	if(createConnection)
	{
		TransportConnection* tc = new TransportConnection(nullGuardFactoryClient, qualifier, instanceIdx, "T", connectionName);
		tc->init(this);
        // Transport requests should have higher priority
        // than normal requests coming in to this gateway
        tc->underlyingConnection().setPriority(_COMM_MSGPRIORITY_INITCONNECT + 1);
		cliPool.connect(tc->underlyingConnection(), appFactoryServer, qualifier, connectionName); 
		transportConnections.push_back(tc);
		PLog("New %s connection created", dispatcherName);
	}
}

void AppFactoryClientConnection::processMessage( UINT32 msgId, const CommMsgBody& body ) // override
{
	CommMsgParser parser( body );
	switch( msgId )
	{
		case COMMAPPFACTORY_CREATEOBJECT_REPLY:
		{
			UINT32 reqId;
			INT16  err_code;
			parser.
				parseUINT32( reqId ). // instanceIdx
				parseINT16(err_code);
			if(err_code)
			{
				const char* errDescr;
				parser.parseString(errDescr);
				PLog("COMMAPPFACTORY_CREATEOBJECT_REPLY instanceIdx=%d error %d: '%s'", reqId, err_code, errDescr);
			}
			else
			{
				PLog("COMMAPPFACTORY_CREATEOBJECT_REPLY instanceIdx=%d Ok", reqId);
			}
			break;
		}
		default:
		{
			PLog("AppFactoryClientConnection::processMessage error ... unexpected message id = %d", msgId );
			break;
		}
	}
}

void TransportConnection::connected() // override
{
	BaseOutgoingGConnectionWithReadableName::connected();
    obj->transportModuleConnected();
}

void BaseGatewayServerObject::transportModuleConnected()
{
	if(!hasTransportConnections)
	{
		hasTransportConnections = true;
		registerServerConnFactories();
	}
}


//////////////////////////////////////////////////////////
//// process reply message from TransportConnection client
void TransportConnection::processGMessage(UINT32 reqId, UINT32 msgId, const CommMsgBody& body, AsyncCall* call) // override
{
	--numRequests;
	obj->processCallback(reqId, msgId, body, call, *this); // virtual
}

// virtual - override it if some extras (e.g. db) should be done in addition
void BaseGatewayServerObject::processCallback(
	UINT32 reqId, UINT32 msgId, const CommMsgBody& body, CommClientGConnection::AsyncCall* call, TransportConnection& tc)
{
	HiresCounter counter;
	if (isBasePrologueEpilogue())
	{
		PLog("{T%d %d %X", tc.instanceIdx, msgId, reqId);
	}

	if (call)
	{
		call->processReply(reqId, msgId, body);
	}
	else
	{
		CommMsgParser parser(body);
		switch (msgId)
		{
			case MSG_ADMIN_REREAD_INI_REPLY: // reply from Transport
			{
				INT16 err_code;
				parser.parseINT16(err_code);
				if ( 0 != err_code )
				{
					const char* errDescr;
					parser.parseString( errDescr );
					PLog("error messageID=%d, errCode=%d, errDescr='%s'", msgId, err_code, errDescr);
				}
				break;
			}
			default:
			{
				PLog(" error: unhandled message ID=%u", msgId);
				break;
			}
		}
	}

	if (isBasePrologueEpilogue())
	{
		counter.stop();
		UINT32 cnt = counter.getElapsedTick();
		ticksAccumulator.addMessageTicks(counter);
		PLog("}T%d %u %u%s", tc.instanceIdx, msgId, cnt, getOverspanStr(cnt));
	}
}


////////////////////////////////////////////////////////
//// process request message to GatewayServerConnection
void BaseGatewayServerConnection::processMessage(UINT32 msgId, const CommMsgBody& body) // final override
{
	// [AN 2010-03-30] PYR-14365
    // PYR-19591 - admin requests should never be ignored to prevent hanging of the admin tool
    // Non-admin connections are not expected to receive "prepare for shutdown" message
	bool isAdminConn = (dynamic_cast<TrustedAdminServerConnection*>(this) != NULL);
	if ( obj->isPreparingForShutdown && !isAdminConn)
	{
        PLog("Ignored - %s for Shutdown", obj->isReadyForShutdown ? "*** READY ***" : "Preparing");
		return;
	}

	obj->processGenericConnectionMessage(msgId, body, *this); // its implementation must call conn->processMessage0
}

// virtual
void BaseGatewayServerObject::processGenericConnectionMessage(UINT32 msgId, const CommMsgBody& body, BaseGatewayServerConnection& conn)
{
	// override it if some extras (e.g. db) should be done in addition to processMessage0()
	UINT32 inQuSize = 0;
	HiresCounter counter;

	if (isBasePrologueEpilogue())
	{
		PLog("{%s %d", conn.getTraceMarker(), msgId);
		inQuSize = getInQueueSize();
	}

	conn.processMessage0(msgId, body); // virtual

	if (isBasePrologueEpilogue())
	{
		counter.stop();
		UINT32 cnt = counter.getElapsedTick();
		ticksAccumulator.addMessageTicks(counter);
		inQueueAccumulator.addQueue(inQuSize);
		PLog("}%s %d %u%s", conn.getTraceMarker(), msgId, cnt, getOverspanStr(cnt));
	}
}


/////////////////////////////////////////////////////////////////////////
//// HeLogger and performance reporting
void BaseGatewayServerObject::startHeLogger(const char * strServerAddress, const char * strStationName )
{
	if( !heLogger )
	{
		heLogger = new HeLogger;
		heLogger->init( cliPool, strServerAddress, strStationName, HE_LOGGER_LOCAL_TIME_ANSWERING );
	}
	if( !heLoggerTimerHandle )
	{
		CommMsgBody timerBody;
		heLoggerTimerHandle = postTimerMessage(DBM_TIMER_LOGGER, timerBody, TIMER_LOGGER_MSEC);
	}
}

void BaseGatewayServerObject::stopHeLogger()
{
	if(heLoggerTimerHandle)
	{
		deleteTimerMessage(heLoggerTimerHandle);
		heLoggerTimerHandle = 0;
	}
}

// virtual
void BaseGatewayServerObject::reportPerformance()
{
	CommMsgBody reportBody;
	reportBasePerformance(reportBody);
	heLogger->post(reportBody);
}

void BaseGatewayServerObject::reportBasePerformance(CommMsgBody& reportBody)
{
	if (!useLogger || !heLogger)
		return;

	try
	{
		ticksAccumulator.stop();

		PString strValue;

		UINT32 messagesPerSec = ticksAccumulator.messagesPerSec();
		strValue.assign("").appendInt(messagesPerSec);
		heLogger->compose (reportBody, "messagesPerSec", strValue);
		
		UINT32 avgProcessingMsec = ticksAccumulator.avgProcessingMkSec();
		strValue.assign("").appendInt(avgProcessingMsec);
		heLogger->compose (reportBody, "avgProcessingMksec", strValue);
		strValue.assign("").appendInt(ticksAccumulator.avgProcessingMkSecTick());
		heLogger->compose (reportBody, "avgProcessingMksecTick", strValue);

		UINT32 loadPercentCnt = ticksAccumulator.loadPercentCntTick();
		strValue.assign("").appendInt(loadPercentCnt);
		heLogger->compose (reportBody, "loadPercentCounter", strValue);

		strValue = emulationMode ? "emulation" : "real";
		heLogger->compose(reportBody, "mode", strValue);

		strValue.assign("").appendInt(inQueueAccumulator.getAverage());
		heLogger->compose (reportBody, "avgInQueueSize", strValue);

		strValue.assign("").appendInt(inQueueAccumulator.getMax());
		heLogger->compose (reportBody, "maxInQueueSize", strValue);
		PLog("QA %u QM %u", inQueueAccumulator.getAverage(), inQueueAccumulator.getMax());

		strValue.assign("").appendInt(calculateNumRequestsForReport());
		heLogger->compose (reportBody, "avgQueueSize", strValue);

		ticksAccumulator.reset();
		inQueueAccumulator.reset();
	}
	catch( const PError& er )
	{
		PLog("reportPerformance() error: '%s'", er.why());
	}
}

INT32 BaseGatewayServerObject::calculateNumRequestsForReport() const
{
	UINT32 numConnectedTM = 0;
	INT32 queuedMessages = 0;
	for(UINT32 i = 0; i < transportConnections.size(); ++i)
	{
		if( transportConnections[i]->isActuallyConnected())
		{
			++numConnectedTM;
			queuedMessages += transportConnections[i]->getNumRequests();
		}
	}
	if(numConnectedTM)
		return (queuedMessages / numConnectedTM);
	
	return 0;
}

INT32 BaseGatewayServerObject::calculateNumOfPendingRequests() const
{
    INT32 pendingRequests = 0;
	for(UINT32 i = 0; i < transportConnections.size(); ++i)
	{
		if( transportConnections[i]->isActuallyConnected())
		{
			pendingRequests += transportConnections[i]->_size();
		}
	}
	return pendingRequests;
}

///////////////////////////////////////
UINT32 BaseGatewayServerObject::findBestTransportConnection( ILoad* loadHandler /* = NULL */) const
{
	LoadHandlerByRequest	lbr;
	if( !loadHandler )
	{
		loadHandler = &lbr;
	}
	UINT32 idx = transportConnections.size();
	UINT32 bestLoad = 0xFFFFFFFF;
	for(UINT32 i = 0; i < transportConnections.size(); ++i)
	{
		const TransportConnection& tc = *transportConnections[i];
		if(tc.isActuallyConnected())
		{
			const UINT32 load = loadHandler->getLoad( tc );

			PLog(" idx %d, %s, reqs: %d load: %d", i, tc.getInstanceName(), tc.getNumRequests(), load );
			if( load == 0 || load < bestLoad )
			{
				idx = i;
				bestLoad = load;
				if(bestLoad == 0)
				{
					break;
				}
			}
		}
	}
	return idx;
}

// Find best connection for account transactions based on key 
// We must send transactions associated with the same key through one transport connection
// otherwise they might be received out of order
UINT32 BaseGatewayServerObject::findBestTransportConnectionByKey(UINT32 key, ILoad* loadHandler /* = NULL */)
{
	LoadHandlerByRequest	lbr;
	if( !loadHandler )
	{
		loadHandler = &lbr;
	}
    UINT32 idx;

	TransportKeyToTransportId::const_iterator it = transportKeyToTransportIdMap.find(key);
    if( it == transportKeyToTransportIdMap.end() )
    {
        idx = findBestTransportConnection(loadHandler);
        if( idx < transportConnections.size() )
        {
            transportKeyToTransportIdMap.insert(make_pair(key, idx));
			TransportConnection& tc = *transportConnections[idx];
			loadHandler->setLoad( tc, loadHandler->getLoad( tc ) + 1 );
        }
    }
    else
    {
        // found transport already used for this key
        idx = it->second;
    }
    return idx;
}

void BaseGatewayServerObject::decreaseLoad(UINT32 key, ILoad* loadHandler /* = NULL */)
{
	LoadHandlerByRequest	lbr;
	if( !loadHandler )
	{
		loadHandler = &lbr;
	}

	PLog("DLD: k=%u", key);
	TransportKeyToTransportId::const_iterator it = transportKeyToTransportIdMap.find(key);
	if( it != transportKeyToTransportIdMap.end() )
	{
		UINT32 idx = it->second;
		TransportConnection& tc = *transportConnections[idx];

		UINT32 load = loadHandler->getLoad(tc);
		if( load > 0 )
		{
			PLog("-l=%u idx=%u", load, idx);
			loadHandler->setLoad(  tc, load - 1 );
		}
	}
}


//////////////////////////////////////////////////////////////////////////
//// ADMIN  ///////////////
void TrustedAdminServerConnection::processMessage0( UINT32 msgId, const CommMsgBody& body ) // override
{
	obj->processTrustedAdminMessage(msgId, body, *this);
}

void BaseGatewayServerObject::processTrustedAdminMessage(UINT32 msgId, const CommMsgBody& body, TrustedAdminServerConnection& conn)
{
    if( isRepeatedRequestForShutdown( msgId, body, conn ) )
    {
        return; // reply is sent already
    }
    // 4s4<s>M - reqId, adminId, numRights<right>, msgBody
    CommMsgParser wrapperparser( body );
    UINT32 requestId;

    wrapperparser.parseUINT32( requestId );
    MsgAuthRedirectorToGateway msg( wrapperparser );
    //CommMsgParser msgParser( msg.msgBody );
    PLog( "adminId='%s'", msg.adminId.c_str() );

    switch( msgId )
    {
	    case MSG_ADMIN_REREAD_INI:
        {
            if( ignoreWhilePreparingForShutdown( msgId, requestId, conn, MSG_ADMIN_REREAD_INI_REPLY ) )
            {
                break;
            }
            if( !checkAdminRight( SYSADMIN_STR, MSG_ADMIN_REREAD_INI_REPLY, requestId, msg.adminRights, conn ) )
            {
                break;
            }
            PLog("reloading configuration.");
            processRereadIni(requestId, &conn); // may be overridden
            break;
        }
		case MSG_ADMIN_PREPARE_FOR_SHUTDOWN:
        {
            // check for repeated "prepare for shutdown" requests is already made at the beginning
            if( !checkAdminRight(getAdminRightForShutdown(), MSG_ADMIN_PREPARE_FOR_SHUTDOWN_REPLY, requestId, msg.adminRights, conn ) )
            {
                break;
            }
            prepareForShutdown();

            CommMsgBody reply;
            reply
                .composeUINT32( requestId )
                .composeINT16( 0 );
            conn.post( MSG_ADMIN_PREPARE_FOR_SHUTDOWN_REPLY, reply );
            break;
        }
		case MSG_ADMIN_KILL_ITSELF:
        {
            if( ignoreWhilePreparingForShutdown( msgId, requestId, conn, MSG_ADMIN_KILL_ITSELF_REPLY ) )
            {
                break;
            }
            if( !checkAdminRight( HANDLE_KILL_ITSELF, MSG_ADMIN_KILL_ITSELF_REPLY, requestId, msg.adminRights, conn ) )
            {
                break;
            }
            // No need to reply since the connection may be destroyed by exit()
            shutdown();
            exit();
            break;
        }
		default:
        {
            if( doProcessTrustedAdminMessage( requestId, msgId, msg.adminId, msg.adminRights, msg.msgBody, conn ) )
            {
                break;
            }
            PLog("processTrustedAdminMessage ... unhandled message msgId=%u reqId=%u", msgId, requestId );
            break;
        }
    }
}

bool BaseGatewayServerObject::checkAdminRight( const char* rightName, UINT32 replyMsgId, UINT32 reqId, const vector<PString>& adminRights, TrustedAdminServerConnection& conn )
{
	bool hasRight = isStringInVector(rightName, adminRights);
	PLog("chAdmRt '%s' %s", rightName, hasRight ? "ok" : "rejected");
	if (hasRight)
		return true;
    CommMsgBody reply;
    reply
        .composeUINT32( reqId )
        .composeINT16( NOT_ALLOWED )
        .composeString( "NOT_ENOUGH_RIGHTS" );
    conn.post( replyMsgId, reply );
    PTRACE( " checkAdminRight - NOT_ENOUGH_RIGHTS [%u:%08X]", replyMsgId, reqId );
    return false;
}

// virtual
void BaseGatewayServerObject::processRereadIni(UINT32 requestId, TrustedAdminServerConnection* conn)
{
	loadAndLogIniFile(iniFile, fullIniFileName);
	dynamicInit();

	if (!conn)
		return;
	CommMsgBody response;
    response
        .composeUINT32(requestId)
        .composeINT16(DBM_NO_ERROR);
    conn->post(MSG_ADMIN_REREAD_INI_REPLY, response);
}

// static
void BaseGatewayServerObject::loadAndLogIniFile(PIniFile& ini, const char* fileName)
{
	PLog("=================\n" "loadAndLogIniFile(%s)", fileName);
	ini.load(fileName);
	for (int sIdx = 0; sIdx < ini.sections.size(); ++sIdx)
	{
		const PIniFile::Section& s = ini.sections[sIdx];
		PLog("[%s]", s.name.c_str());
		if (s.name.startsWith(";"))
		{
			PLog(" -section ignored");
			continue;
		}
		for (int itemIdx = 0; itemIdx < s.items.size(); ++itemIdx)
		{
			const PIniFile::Item& item = s.items[itemIdx];
			bool isPwdRelated = item.name.findIgnoreCase("password") || item.name.findIgnoreCase("pwd");
			PLog("%s=%s|", item.name.c_str(), isPwdRelated ? "__hidden__" : item.value.c_str());
		}
	}
	PLog("loadAndLogIniFile end" "\n=================");
}

////////////////////////////////////////////////
//// SHUTDOWN ////////////
void BaseGatewayServerObject::prepareForShutdown()
{
	PLog("Gateway Preparing For Shutdown");
	isPreparingForShutdown = true;

	if( !shutdownTimerHandle)
	{
		CommMsgBody timerBody;
		shutdownTimerHandle = postTimerMessage(DBM_SHUTDOWN_TIMER_LOGGER, timerBody, TIMER_SHUTDOWN_LOGGER_MSEC);
	}
}

// check if we're already preparing for shutdown and if so, whether the admin request we received is
// a repeated shutdown request, in which case instead of ignoring it we MUST reject it with an error
// in order to avoid it being resubmitted by a client's G-Connection once a new instance of AAMS Gateway starts
bool BaseGatewayServerObject::isRepeatedRequestForShutdown( UINT32 msgId, const CommMsgBody& body, TrustedAdminServerConnection& conn ) const
{
	bool isRepeatedRequest = false;

	if( isPreparingForShutdown && msgId == MSG_ADMIN_PREPARE_FOR_SHUTDOWN)
	{
        isRepeatedRequest = true;
        PLog("admin - repeated shutdown request");

        CommMsgParser parser(body);
        UINT32 reqId;
        parser.parseUINT32(reqId);
		CommMsgBody replyBody;
        replyBody
            .composeUINT32(reqId)
            .composeINT16(AAMS_REPEATED_SHUTDOWN_REQUEST)
            .composeString("Gateway is already preparing for shutdown")
            ;
        conn.post(MSG_ADMIN_PREPARE_FOR_SHUTDOWN_REPLY, replyBody);
	}
	return isRepeatedRequest;
}

bool BaseGatewayServerObject::ignoreWhilePreparingForShutdown(UINT32 msgId, UINT32 requestId, TrustedAdminServerConnection& conn, UINT32 replyMsgId) const
{
    // PYR-19591 - kill itself message must be allowed when component is ready for shutdown
    // but others will be ignored and error reply must be sent to Admin tool to keep it from hanging.
    if( isPreparingForShutdown && (!isReadyForShutdown || msgId != MSG_ADMIN_KILL_ITSELF) )
    {
        PLog( " ignored - preparing for shutdown" );
        CommMsgBody reply;
        reply
            .composeUINT32( requestId )
            .composeINT16( NOT_ALLOWED )
            .composeString( "Gateway is preparing for shutdown" )
            ;
        conn.post( replyMsgId, reply );
        return true;
    }
    return false;
}

void BaseGatewayServerObject::trackTransportConnectionLoadsBeforeShutdown()
{
	// do not execute unless preparing for shutdown
	if (!isPreparingForShutdown)
		return;

	PLog("Transport connection load status:");
	bool noMoreRequests = isNoMoreRequests();

	// update gateway state
	isReadyForShutdown = noMoreRequests;
	PLog("Gateway %s", noMoreRequests ? "IS *** READY FOR SHUTDOWN ***" : "Is Still Processing Requests");
}

// [AN 2010-03-30] PYR-14365
// enumerate through all transport connections and check if all of them have no more requests left
// in which case the gateway is ready for shutdown
bool BaseGatewayServerObject::isNoMoreRequests() const
{
	bool noMoreRequests = true;
	// iterate through all transport modules and log their request counts
	for(UINT32 i = 0; i < transportConnections.size(); ++i)
	{
		const TransportConnection& tc = *transportConnections[i];
		if( tc.isActuallyConnected())
		{
			PLog(" idx %d, %s, load: %d", i, tc.getInstanceName(), tc.getNumRequests());
			// until we have a first transport connection with non-zero request count
			// keep checking request count
			if (noMoreRequests)
				noMoreRequests = (tc.getNumRequests() == 0);
		}
	}
	return noMoreRequests;
}

bool TransportConnection::closedOrDisconnected(int errCode, const char* errMsg) // override
{
	obj->transportModuleDisconnected(*this);
	return BaseOutgoingGConnectionWithReadableName::closedOrDisconnected(errCode, errMsg);
}

void BaseGatewayServerObject::transportModuleDisconnected(TransportConnection& tc)
{
	if(!tc._size())
	{
		return;
	}
	PLog("tcMsgMapSize=%u", tc._size());

	set<UINT32> clearedKeys;
	for(CommClientGConnection::const_iterator it = tc._begin(); it != tc._end(); ++it)
	{
		CommClientGConnection::AsyncCall* ac = CommClientGConnection::_getCall(it);
		if (!ac) // without callback
			continue;
		AsyncCallRequestId* tcAcyncCall = dynamic_cast<AsyncCallRequestId*>(ac);
		if (!tcAcyncCall) // should never happen (aka fubar)
		{
			PLog(LOG_TRACE_FAULTY_LOGIC " BaseGatewayServerObject::transportModuleDisconnected - !tcAcyncCall");
			continue;
		}

		const UINT32 key = tcAcyncCall->getTransportKey();
		UINT32 i = 0;
		if( key != 0 )
		{
			// first time we find a particular key - erase the old transport index associated with it
			// subsequent call to findBestTransportConnectionByKey will add a new transport index for that key
			if( clearedKeys.find( key ) == clearedKeys.cend() )
			{
				PLog("found key %d", key);
				transportKeyToTransportIdMap.erase( key );
				clearedKeys.insert( key );
			}
			i = findBestTransportConnectionByKey( key );
		}
		else
		{
			i = findBestTransportConnection();
		}
		if(i >= transportConnections.size())
		{
			PLog("WARNING!!! switch to alternative: alternative server not found. Ignored.");
			return;
		}
		PLog("key=%d switch to alternate: reqId %u connId %u -> %s", key, tcAcyncCall->getReqId(), tcAcyncCall->getConnId(), transportConnections[i]->getInstanceName());
		CommMsgBody body;
		CommClientGConnection::_getMsgBody(it, body);
		transportConnections[i]->postX(CommClientGConnection::_getMsgId(it), body, tcAcyncCall->clone() );
	}
	tc._clearAll();
}

void BaseGatewayServerObject::shutdown() // override
{
	PLog("BaseGatewayServerObject::shutdown");
	if(useLogger)
	{
		stopHeLogger();
		delete heLogger;
	}
}


//////////////////////////
//// TIMER processing ////
void BaseGatewayServerObject::processTimerMessage( UINT32 handle, UINT32 msgId, CommMsgBody& body ) // override
{
	UINT32 ticks = SYSTEMTICKS();
	PTRACE("{ timer Q=%d msgId=%d", getInQueueSize(), msgId );

    switch( msgId )
    {
	    case DBM_TIMER_LOGGER:
	    {
		    // report performance data to Monitor
		    reportPerformance();
		    CommMsgBody timerBody;
		    heLoggerTimerHandle = postTimerMessage(DBM_TIMER_LOGGER, timerBody, TIMER_LOGGER_MSEC);
		    break;
	    }
	    case DBM_SHUTDOWN_TIMER_LOGGER:
	    {
		    // [AN 2010-03-30] PYR-14365
		    // does nothing unless preparing for shutdown
		    // otherwise logs connection request counts
		    trackTransportConnectionLoadsBeforeShutdown();
		    CommMsgBody timerBody;
		    shutdownTimerHandle = postTimerMessage(DBM_SHUTDOWN_TIMER_LOGGER, timerBody, TIMER_SHUTDOWN_LOGGER_MSEC);
		    break;
	    }
	    default:
	    {
			// do not process any more messages if preparing for shutdown
			if( isPreparingForShutdown )
			{
				PLog("isPreparingForShutdown timer handle=%u msgId=%u ignored", handle, msgId);
				break;
			}

            if( doProcessTimerMessage( handle, msgId, body ) )
                break;

		    PLog("processTimerMessage error ... unexpected message id = %d", msgId );
		    break;
	    }
    }
	ticks = SYSTEMTICKS() - ticks;
	PTRACE("}I %04d %05d%s", msgId, ticks, ( ticks > TICKS_PER_SECOND ) ? " overspan" : "");
}

UINT32 LoadHandlerByRequest::getLoad( const TransportConnection& tc ) const 
{
	return tc.getNumRequests();
}
void LoadHandlerByRequest::setLoad( TransportConnection& tc, const UINT32 load ) const
{
	// when working with load by request do not set load
}

UINT32 LoadHandlerByLoad::getLoad( const TransportConnection& tc ) const 
{
	return tc.getLoadCounter();
}
void LoadHandlerByLoad::setLoad( TransportConnection& tc, const UINT32 load ) const
{
	tc.setLoadCounter( load );
}
