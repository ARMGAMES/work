
#include "ColTCADbmobject.h"
#include "AuthRedirector.h"
#include "admincommon.h"
#include "connUtils.h"


/* static */ CommServerNullGuardFactory ColTCADbmServerObject::guardFactory;
/* static */ CommServerPlainTextPasswordSidGuardFactory ColTCADbmServerObject::authGuardFactory;
/* static */ CommClientNullGuardFactory ColTCADbmServerObject::clientNullGuardFactory;

ColTCADbmServerObject::ColTCADbmServerObject(_CommInterface& inter) : 
	DbmServerObjectCommon( inter ), 
	srvPool( inter ), cliPool( inter),
	connAdminFactory(*this, authGuardFactory),
	connTrustedAdminFactory(*this, guardFactory),
	connExampleSrvFactory(*this, guardFactory, ExampleSrv),
	loginConnection(*this, clientNullGuardFactory, "LOG", 'L'),
	heLoggerTimerHandle(0), heLogger(NULL),	useLogger(false),
	cleanupTimerHandle(0), moreTrace(false)
{

}

ColTCADbmServerObject::~ColTCADbmServerObject()
{
	if (heLogger)
	{
		stopHeLogger();
		delete heLogger;
		heLogger = NULL;
	}
}

bool ColTCADbmServerObject::registerAllFactories() // override
{
	if( !Parent::registerAllFactories() )
	{
		// Id generator not ready yet
		return false;
	}
	srvPool.registerFactory(COLTCADBM_SRV_ADMIN_CONN_NAME, connAdminFactory );
	srvPool.registerFactory(COLTCADBM_SRV_TRUSTEDADMIN_CONN_NAME, connTrustedAdminFactory );
	srvPool.registerFactory(COLTCADBM_SRV_EXAMPLE_CONN_NAME, connExampleSrvFactory);

	return true;
}

void ColTCADbmServerObject::init(CommMsgBody& initMsg)
{
	CommMsgParser parser(initMsg);
	const char* initDir_;
	parser.parseString(initDir_);

	if (!parser.parseEnded())
	{
		parser.parseStringP(loggerStationName);
		PLog("logger station name [%s] was provided at startup and will override correspondent logger's INI property", loggerStationName.c_str());
	}

	initDir.assign(initDir_);
	fullFileName.assign(initDir_);
	fullFileName.append(SERVER_CONFIG_FILE);
	PIniFile ini(fullFileName);

	dbManager.init( this );
	PString sectionName("MAINSTORAGE");
	if (!dbManager.openDataStorage(fullFileName, sectionName))
	{
		throw PError("ERROR: Main data storage not open");
	}

	dynamicInit();
	initGenerator( &dbManager, &srvPool, &cliPool, &clientNullGuardFactory, ini );
	registerAllFactories();

	if (cleanupTimerHandle)
	{
		deleteTimerMessage(cleanupTimerHandle);
		cleanupTimerHandle = 0;
	}
	CommMsgBody timerBody;
	postTimerMessage(SERVER_CLEANUP_TIMER, timerBody, TIMER_CLEANUP_MSEC);

	startup();
}

void ColTCADbmServerObject::dynamicInit()
{
	PIniFile ini(fullFileName);
	
	PIniFile::Section * pSection = ini.getSection("DYNAMICCONNECT");
	if( pSection )
	{
		const char* server = pSection->getProperty("LoginDbmServer");
		const char* object = pSection->getProperty("LoginDbmObject", "LoginDbmServerInstance");
		const char* connType = pSection->getProperty("LoginDbmConnectionType", "LOGINSERVER");
		if (server && *server)
		{
			reconnect(loginConnection, server, object, connType);
		}
	}
	else
	{
		PLog("Warning : no DYNAMICCONNECT in config");
	}

	pSection = ini.getSection("DYNAMICSETTINGS");
	if( pSection )
	{
		moreTrace = pSection->getIntProperty("moreTrace", 0) != 0;
	}
	else
	{
		PTRACE("No DYNAMICSETTINGS configured");
	}

	PLog("-moreTrace=%s", moreTrace ? "T" : "F");

	pSection = ini.getSection("LOGGER");
	if (pSection)
	{
		useLogger = (0 != pSection->getIntProperty("useLogger", 0));
		const char* loggerServerAddress = pSection->getProperty("LoggerAddress", "");
		if (!loggerStationName.length())
		{
			loggerStationName = pSection->getProperty("LogStationName", "");
		}

		PLog("useLogger: %s", useLogger ? "YES" : "NO");
		PLog("loggerServerAddress: %s", loggerServerAddress);
		PLog("loggerStationName: %s", loggerStationName.c_str());

		if (useLogger && *loggerServerAddress && loggerStationName.length())
		{
			startHeLogger(loggerServerAddress, loggerStationName);
		}
		else
		{
			useLogger = false;
			PTRACE("No LOGGER started");
		}
	}
	else
	{
		useLogger = false;
		PTRACE("No LOGGER configured");
	}

	Parent::dynamicInit( ini );
}

void ColTCADbmServerObject::reconnect(GenericOutgoingConnection& conn, const char* const serverName, const char* const serverObject, const char* const serverConnectionType)
{
	PLog("reconnect(serverName: %s, serverObject: %s, serverConnectionType: %s)"
		, (serverName && *serverName) ? serverName : "none"
		, (serverObject && *serverObject) ? serverObject : "none"
		, (serverConnectionType && *serverConnectionType) ? serverConnectionType : "none"
	);

	ConnUtils::createClientConnection(cliPool, conn, serverName, serverObject, serverConnectionType);

	// Based on assumption that isConnected() returns logical status, but not status of phisical connection let's check that it got 'connected'
	if (!conn.underlyingConnection().isConnected())
	{
		PString err("Failed to connect to ");
		err
			.append(serverName)
			.append("/")
			.append(serverObject)
			.append("/")
			.append(serverConnectionType)
			;
		
		PLog(LOG_TRACE_FAULTY_LOGIC ": %s", err.c_str());
	}
}

void ColTCADbmServerObject::processCommonServerMessage(UINT32 msgId, const CommMsgBody& body, ServerConnectionType connType, GenericSrvConnection* conn)
{
	if (connType <= LowerGuard || connType >= UpperGuard)
	{
		throw PError((PString("Unsupported eServerConnection value: ").appendInt(connType)));
	}

	PLog("{%s %u %08x", conn->getConnMarker(), msgId, conn->id());

	HiresCounter counter;
	dbManager.setAutoCommitTrue();

	bool Ok = true;
	for (int n = 0; n < dbManager.getMaxDbReconnectAttempts(); n++)
	{
		try
		{
			if (Ok)
			{
				switch (connType)
				{
				case TrustedAdmin:
				{
					processTrustedAdminMessage(msgId, body, conn);
					break;
				}
				case Admin:
				{
					processAdminMessage(msgId, body, conn);
					break;
				}
				case ExampleSrv:
				{
					processExampleSrvMessage(msgId, body, conn);
					break;
				}
				}

				break; // out of the loop
			}
		}

		catch (const PDbDisconnectError& er)
		{
			PLog("processColTCADbmServerMessage...Disconnect from database detected, reason: '%s', trying to reconnect, attempt #%d,", er.why(), n);
		}

		PThread::sleep(dbManager.getDbReconnectInterval());
		PString sectionName("MAINSTORAGE");
		if (!dbManager.openDataStorage(fullFileName, sectionName))
		{
			PLog("processColTCADbmServerMessage error ...attempt #%d failed", n);
			Ok = false;
		}
		else
		{
			PLog("processColTCADbmServerMessage...reconnect OK");
			Ok = true;
		}
	}

	counter.stop();
	UINT32 cnt = counter.getElapsedTick();
	PLog("}%u%s", cnt, (cnt > TICKS_PER_SECOND) ? " overspan" : "");
	ticksAccumulator.addMessageTicks(counter);
}

void ColTCADbmServerObject::processCallback(UINT32 reqId, UINT32 msgId, const CommMsgBody& body, CommClientGConnection::AsyncCall* pCall)
{
	HiresCounter counter;
	PLog("{ASC %u", msgId);

	pCall->processReply(reqId, msgId, body);

	counter.stop();
	UINT32 cnt = counter.getElapsedTick();
	PLog("}%04u %05u%s", msgId, cnt, (cnt > TICKS_PER_SECOND) ? " overspan" : "");
	ticksAccumulator.addMessageTicks(counter);
}

void ColTCADbmServerObject::postReply(GenericSrvConnection* pConn, UINT32 msgId, CommMsgBody& body)
{
	if (pConn)
		pConn->post(msgId, body);
	else
		PLog("Message #%u not sent (no connection)", msgId);
}

void ColTCADbmServerObject::setReply(CommMsgBody& reply, INT16 errCode, const char* errDesc /*= ""*/)
{
	reply.composeINT16(errCode);

	if (errCode)
		reply.composeString(errDesc);
}

void ColTCADbmServerObject::processExampleSrvMessage(UINT32 msgId, const CommMsgBody & body, GenericSrvConnection* conn)
{
	CommMsgParser parser(body);
	switch (msgId)
	{

		default:
		{
			PLog("ColTCADbmServerObject::processTCAServerMessage error ... unhandled message ID=%u", msgId);
			break;
		}
	}
}


void ColTCADbmServerObject::replyMsg(UINT32 connId, UINT32 msgId, CommMsgBody& body)
{
	if (!connId)
	{
		PLog("Warning - Message #%u failed to send, no connId.", msgId);
		return;
	}

	CommServerConnection* conn = srvPool.findConnection(connId);
	if (!conn)
	{
		PLog("Warning - Message #%u failed to send, lost connection.", msgId);
		return;
	}

	conn->post(msgId, body);
}

//
// HeLogger and performance reporting
//

void ColTCADbmServerObject::startHeLogger(const char * strServerAddress, const char * strStationName )
{
	PLog("ColTCADbmServerObject::startHeLogger()");
	PLog("ServerAddress - %s", strServerAddress);
	PLog("StationName - %s", strStationName);

	if (heLogger)
	{
		stopHeLogger();
		delete heLogger;
		heLogger = NULL;
	}

	if( !heLogger )
	{
		heLogger = new HeLogger;
		heLogger->init( cliPool, strServerAddress, strStationName, HE_LOGGER_LOCAL_TIME_ANSWERING );
	}

	if( !heLoggerTimerHandle )
	{
		CommMsgBody timerBody;
		heLoggerTimerHandle = postTimerMessage(SERVER_LOGGER_TIMER, timerBody, TIMER_LOGGER_MSEC );
	}
}

void ColTCADbmServerObject::stopHeLogger()
{
	PLog("ColTCADbmServerObject::stopHeLogger()");

	if( heLoggerTimerHandle )
	{
		deleteTimerMessage(heLoggerTimerHandle);
		heLoggerTimerHandle = 0;
	}
}

void ColTCADbmServerObject::reportPerformance()
{
	PTRACE("ColTCADbmServerObject::reportPerformance()");

	if( useLogger )
	{
		try
		{
			ticksAccumulator.stop();

			CommMsgBody reportBody;
			PString strValue;

			UINT32 messagesPerSec = ticksAccumulator.messagesPerSec();
			strValue.assign("").appendInt(messagesPerSec);
			heLogger->compose (reportBody, "messagesPerSec", strValue);

			UINT32 avgProcessingMsec = ticksAccumulator.avgProcessingMkSec();
			strValue.assign("").appendInt(avgProcessingMsec);
			heLogger->compose (reportBody, "avgProcessingMksec", strValue);
			strValue.assign("").appendInt(ticksAccumulator.avgProcessingMkSecTick());
			heLogger->compose (reportBody, "avgProcessingMksecTick", strValue);

			strValue.assign("").appendInt(loginConnection.getConnected());
			heLogger->compose(reportBody, "connection.login", strValue);

			UINT32 loadPercentCnt = ticksAccumulator.loadPercentCntTick();
			strValue.assign("").appendInt(loadPercentCnt);
			heLogger->compose (reportBody, "loadPercentCounter", strValue);

			ticksAccumulator.reset();

			HeLoggerParameters dbManagerParameters;
			dbManager.composeHeLoggerParameters(dbManagerParameters);
			for( HeLoggerParameters::const_iterator it=dbManagerParameters.begin(); it!=dbManagerParameters.end(); ++it )
			{
				heLogger->compose (reportBody, it->first, it->second);
			}

			heLogger->post(reportBody);
		}
		catch( const PError& er )
		{
			PLog( "ColTCADbmServerObject::reportPerformance() error: '%s'", er.why() );
		}
	}
}

//
// Timer message processing
//

void ColTCADbmServerObject::processTimerMessage( UINT32 /*handle*/, UINT32 msgId, CommMsgBody& body )
{
	UINT32 ticks = SYSTEMTICKS();
	PTRACE("{timer msgId=%d", msgId );

	CommMsgParser parser(body);

	switch( msgId )
	{
		case SERVER_LOGGER_TIMER:
		{
			CommMsgBody timerBody;
			heLoggerTimerHandle = postTimerMessage(SERVER_LOGGER_TIMER, timerBody, TIMER_LOGGER_MSEC );
			reportPerformance();
			break;
		}
		case TIMER_CLEANUP_MSEC:
		{
			CommMsgBody timerBody;
			heLoggerTimerHandle = postTimerMessage(SERVER_CLEANUP_TIMER, timerBody, TIMER_CLEANUP_MSEC);
			break;
		}
	}
	ticks = SYSTEMTICKS() - ticks;
	PTRACE("}%04d %05d%s", msgId, ticks, ( ticks > TICKS_PER_SECOND ) ? " overspan" : "");
}

void ColTCADbmServerObject::startup()
{
	if (dumpFile.length())
	{
		try
		{
			PLog("Startup - read file=%s", dumpFile.c_str());
			PIOFile fp;
			fp.openFile(dumpFile, PLIB_FILE_READONLY);

			UINT32 bytesCount = (UINT32)fp.getSize();
			CommMsgBody body;
			BYTE* ptr = body._alloc(bytesCount);
			fp.readFile(ptr, bytesCount);

			CommMsgParser parser(body);
			UINT32 nVer;
			parser.parseUINT32(nVer);

			fp.closeFile();
			
		}
		catch (PError& err)
		{
			PLog("startup: cannot read from %s: %s", dumpFile.c_str(), err.why());
		}
	}

}

/*virtual*/ void ColTCADbmServerObject::shutdown() // override
{
	if (dumpFile.length())
	{
		try
		{
			PLog("shutdown - write file=%s", dumpFile.c_str());
			PIOFile fp;
			fp.openFile(dumpFile, PLIB_FILE_CREATE_OR_OPEN | PLIB_FILE_FLAG_TRUNCATE);

			CommMsgBody body;
			body.composeUINT32(fileVer);

			fp.writeFile((BYTE*)body._readPtr(), body._size());
			fp.closeFile();
		}
		catch (PError& err)
		{
			PLog("shutdown: cannot write to %s: %s", dumpFile.c_str(), err.why());
		}
	}
}



