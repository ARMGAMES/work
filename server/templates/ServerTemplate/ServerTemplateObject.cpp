
#include "ServerTemplateObject.h"
#include "connUtils.h"
#include "AuthRedirector.h"
#include "admincommon.h"
#include "dbm.h"

////////////////////////////////////////////////////////////////////////////////

/*static*/ CommServerNullGuardFactory ServerTemplateServerObject::guardFactory;
/*static*/ CommServerPlainTextPasswordSidGuardFactory ServerTemplateServerObject::authGuardFactory;
/*static*/ CommClientNullGuardFactory ServerTemplateServerObject::clientNullGuardFactory;

////////////////////////////////////////////////////////////////////////////////

ServerTemplateServerObject::ServerTemplateServerObject(_CommInterface& inter) : 
	CommServerObject(inter),
	srvPool(inter), cliPool(inter),
	connAdminFactory(*this, authGuardFactory),
	connTrustedAdminFactory(*this, guardFactory),
	connExampleSrvFactory(*this, guardFactory, ExampleSrv),
	loginConnection(*this, clientNullGuardFactory, "LOG", 'L'),
	heLoggerTimerHandle(0), heLogger(NULL), useLogger(false),
	cleanupTimerHandle(0), moreTrace(false)
{

}

ServerTemplateServerObject::~ServerTemplateServerObject()
{
	if (heLogger)
	{
		stopHeLogger();
		delete heLogger;
		heLogger = NULL;
	}
}

bool ServerTemplateServerObject::registerAllFactories() // override
{

	srvPool.registerFactory(COLCDA_SRV_ADMIN_CONN_NAME, connAdminFactory);
	srvPool.registerFactory(COLCDA_SRV_TRUSTEDADMIN_CONN_NAME, connTrustedAdminFactory);
	srvPool.registerFactory(COLCDA_SRV_EXAMPLE_CONN_NAME, connExampleSrvFactory);

	return true;
}

void ServerTemplateServerObject::init(CommMsgBody& initMsg)
{
	CommMsgParser parser(initMsg);
	char const* initDir_;
	parser.parseString(initDir_);

	if (!parser.parseEnded())
	{
		parser.parseStringP(loggerStationName);
		PLog("DBM logger station name [%s] was provided at startup and will override correspondent logger's INI property", loggerStationName.c_str());
	}

	initDir.assign(initDir_);
	fullFileName.assign(initDir_);
	fullFileName.append(SERVER_CONFIG_FILE);

	dynamicInit();

	if (cleanupTimerHandle)
	{
		deleteTimerMessage(cleanupTimerHandle);
		cleanupTimerHandle = 0;
	}
	CommMsgBody timerBody;
	postTimerMessage(SERVER_CLEANUP_TIMER, timerBody, TIMER_CLEANUP_MSEC);

	startup();
	registerAllFactories();
}

void ServerTemplateServerObject::dynamicInit()
{
	PLog("dynamicInit");

	PIniFile ini(fullFileName);
	const PIniFile::Section* pSection = nullptr;

	pSection = ini.getSection("DYNAMICCONNECT");
	if (pSection)
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
	if (pSection)
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

}

void ServerTemplateServerObject::reconnect(CommClientGConnection& conn,
	const char* const serverName,
	const char* const serverObject,
	const char* const serverConnectionType)
{
	PLog("reconnect(serverName: %s, serverObject: %s, serverConnectionType: %s)"
		, (serverName && *serverName) ? serverName : "none"
		, (serverObject && *serverObject) ? serverObject : "none"
		, (serverConnectionType && *serverConnectionType) ? serverConnectionType : "none"
	);

	ConnUtils::createClientConnection(cliPool, conn, serverName, serverObject, serverConnectionType);

	// Based on assumption that isConnected() returns logical status, but not status of physical connection, let's check that it got 'connected'
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

void ServerTemplateServerObject::startup()
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

/*virtual*/ void ServerTemplateServerObject::shutdown() // override
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

////////////////////////////////////////////////////////////////////////////////

void ServerTemplateServerObject::setReply(CommMsgBody& reply, INT16 errCode, const char* errDesc/* = ""*/)
{
	reply.composeINT16(errCode);
	if (errCode)
	{
		reply.composeString(errDesc);
	}
}

void ServerTemplateServerObject::postReply(GenericSrvConnection* conn, UINT32 msgId, CommMsgBody& body)
{
	if (conn)
	{
		conn->post(msgId, body);
	}
	else
	{
		PLog("Message #%u not sent (no connection)", msgId);
	}
}

void ServerTemplateServerObject::replyMsg(UINT32 connId, UINT32 msgId, CommMsgBody& body)
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

////////////////////////////////////////////////////////////////////////////////

void ServerTemplateServerObject::processCallback(UINT32 reqId, UINT32 msgId, const CommMsgBody& body, CommClientGConnection::AsyncCall* pCall)
{
	HiresCounter counter;

	PLog("{ASC %u", msgId);

	pCall->processReply(reqId, msgId, body);

	counter.stop();
	ticksAccumulator.addMessageTicks(counter);

	UINT32 ticks = counter.getElapsedTick();
	PLog("}%u%s", ticks, (ticks > TICKS_PER_SECOND) ? " overspan" : "");
}


void ServerTemplateServerObject::processCommonServerMessage(UINT32 msgId, const CommMsgBody& body, ServerConnectionType connType, GenericSrvConnection* conn)
{
	if (connType <= LowerGuard || connType >= UpperGuard)
	{
		throw PError((PString("Unsupported eServerConnection value: ").appendInt(connType)));
	}

	PLog("{%s %u %08x", conn->getConnMarker(), msgId, conn->id());

	HiresCounter counter;

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
		default:
		{
			throw PError("Unsupported connType value.");
		}
	}

	counter.stop();
	ticksAccumulator.addMessageTicks(counter);

	UINT32 ticks = counter.getElapsedTick();
	PLog("}%u%s", ticks, (ticks > TICKS_PER_SECOND) ? " overspan" : "");
}

void ServerTemplateServerObject::processExampleSrvMessage(UINT32 msgId, const CommMsgBody & body, GenericSrvConnection* conn)
{
	CommMsgParser parser(body);
	switch (msgId)
	{

		default:
		{
			PLog(__FUNCTION__" error ... unhandled message ID=%u", msgId);
			break;
		}
	}

}

void ServerTemplateServerObject::processTimerMessage(UINT32 handle, UINT32 msgId, CommMsgBody& body)
{
	HiresCounter counter;
	PTRACE("{timer msgId=%d", msgId);

	CommMsgParser parser(body);

	switch (msgId)
	{
		case SERVER_LOGGER_TIMER:
		{
			CommMsgBody timerBody;
			heLoggerTimerHandle = postTimerMessage(SERVER_LOGGER_TIMER, timerBody, TIMER_LOGGER_MSEC);
			reportPerformance();
			break;
		}
		case SERVER_CLEANUP_TIMER:
		{
			CommMsgBody timerBody;
			heLoggerTimerHandle = postTimerMessage(SERVER_CLEANUP_TIMER, timerBody, TIMER_CLEANUP_MSEC);
			break;
		}
		default:
		{
			PLog("Unknown timer msg %d", msgId);
			break;
		}
	}

	counter.stop();
	ticksAccumulator.addMessageTicks(counter);

	UINT32 ticks = counter.getElapsedTick();
	PLog("}%u%s", ticks, (ticks > TICKS_PER_SECOND) ? " overspan" : "");
}

void ServerTemplateServerObject::reportPerformance()
{
	PLog(__FUNCTION__);

	try
	{
		ticksAccumulator.stop();

		if (heLogger)
		{
			CommMsgBody reportBody;
			PString strValue;

			strValue.assign("").appendUint(ticksAccumulator.avgProcessingMkSec());
			heLogger->compose(reportBody, "All:avgProcessingMksec", strValue);

			strValue.assign("").appendUint(ticksAccumulator.messagesPerSec());
			heLogger->compose(reportBody, "All:messagesPerSec", strValue);

			strValue.assign("").appendUint(ticksAccumulator.loadPercentCntTick());
			heLogger->compose(reportBody, "All:loadPercentCounter", strValue);

			strValue.assign("").appendUint(ticksAccumulator.avgProcessingMkSecTick());
			heLogger->compose(reportBody, "All:avgProcessingMksecTick", strValue);

			strValue.assign("").appendInt(loginConnection.getConnected());
			heLogger->compose(reportBody, "connection.loginConnection", strValue);

			UINT32 loadPercentCnt = ticksAccumulator.loadPercentCntTick();
			strValue.assign("").appendInt(loadPercentCnt);
			heLogger->compose(reportBody, "loadPercentCounter", strValue);

			heLogger->post(reportBody);
		}

		ticksAccumulator.reset();
	}
	catch (const PError& er)
	{
		PLog(__FUNCTION__" error: '%s'", er.why());
	}

}


void ServerTemplateServerObject::stopHeLogger()
{
	PLog(__FUNCTION__);

	if (heLogger)
	{
		delete heLogger;
		heLogger = NULL;
	}

	if (heLoggerTimerHandle)
	{
		deleteTimerMessage(heLoggerTimerHandle);
		heLoggerTimerHandle = 0;
	}
}

void ServerTemplateServerObject::startHeLogger(const char* serverAddress, const char* stationName)
{
	PLog(__FUNCTION__);
	PLog("- serverAddress: %s", serverAddress);
	PLog("- stationName: %s", stationName);

	stopHeLogger();

	if (!heLogger)
	{
		heLogger = new HeLogger;
		heLogger->init(cliPool, serverAddress, stationName, HE_LOGGER_LOCAL_TIME_ANSWERING);
	}

	if (!heLoggerTimerHandle)
	{
		CommMsgBody emptyBody;
		heLoggerTimerHandle = postTimerMessage(SERVER_LOGGER_TIMER, emptyBody, TIMER_LOGGER_MSEC);
	}
}

//////////////////////////////////////////////////////////////////////////////

