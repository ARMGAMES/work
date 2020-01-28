// ===========================================================
// Copyright (C) 2014 PYR Software Ltd. All rights reserved.
// ===========================================================

#include	"PROJECTobject.h"
#include	"PROJECT.h"
#include "AuthRedirector.h"
#include "admincommon.h"

/* static */	CommServerNullGuardFactory PROJECTServerObject::guardFactory;
/*static*/		CommClientNullGuardFactory PROJECTServerObject::guardFactoryClient;
/*static*/		CommServerPlainTextPasswordSidGuardFactory PROJECTServerObject::guardPwdFactory;


PROJECTServerObject::PROJECTServerObject(_CommInterface& inter)
	: 
	DbmServerObjectCommon( inter ), // PYR-27418
	srvPool( inter ),
	cliPool( inter),
	clientConnection(guardFactoryClient),
	clientGConnection(guardFactoryClient),
	connPROJECTFactory(guardFactory),
	connPROJECTAuthFactory(guardPwdFactory),
	adminTrustedFactory(guardFactory),
	heLoggerTimerHandle(0),
	heLogger(NULL),
	useLogger(false)
{
	connPROJECTFactory.obj = this;
	connPROJECTAuthFactory.obj = this;
	adminTrustedFactory.obj = this;
	clientConnection.obj = this;
	clientGConnection.obj = this;
	connectionToAuthDbm = 0;
	// registering factories is done in virtual method registerAllFactories()
}

void PROJECTServerConnection::processMessage( UINT32 msgId, const CommMsgBody& body )
{
	obj->processCommonServerMessage(msgId, body, this, SomeTypeDbm);
}

void PROJECTServerConnection::connected()
{
	PTRACE("PROJECTServerConnection::connected()");
}

void PROJECTServerConnection::closedOrDisconnected( int errCode, const char* errMsg )
{
	PTRACE( "PROJECTServerConnection::closedOrDisconnected: %d '%s'", errCode, errMsg );
}

void AdminConnection::processMessage( UINT32 msgId, const CommMsgBody& body )
{
	obj->processCommonServerMessage(msgId, body, this, TrustedAdmin);
}

void AdminConnection::connected()
{
	PTRACE("AdminConnection::connected()");
}

void AdminConnection::closedOrDisconnected( int errCode, const char* errMsg )
{
	PTRACE( "AdminConnection::closedOrDisconnected: %d '%s'", errCode, errMsg );
}

void PROJECTServerAuthConnection::processMessage( UINT32 msgId, const CommMsgBody& body )
{
	obj->processServerAuthMessage( msgId, body, this );
}


void PROJECTClientConnection::processMessage( UINT32 msgId, const CommMsgBody& body )
{
	obj->processClientMessage(msgId, body );
}

void PROJECTClientGConnection::processGMessage( UINT32 requestId, UINT32 msgId, const CommMsgBody& body, AsyncCall* call  )
{
	if( call )
	{
		call->processReply( requestId, msgId, body ); 
		return;
	}
	obj->processClientGMessage( requestId, msgId, body );
}

void PROJECTClientAuthConnection::processGMessage( UINT32 requestId, UINT32 msgId, const CommMsgBody& body, AsyncCall* call  )
{
	if( call )
	{
		call->processReply( requestId, msgId, body ); 
		return;
	}
	obj->processClientAuthMessage( requestId, msgId, body );
}


void PROJECTServerAuthConnection::connected()
{
}

void PROJECTServerAuthConnection::closedOrDisconnected( int errCode, const char* errMsg )
{
}


void PROJECTClientConnection::connected()
{
	PLog("PROJECTClientConnection::connected()");
}
bool PROJECTClientConnection::closedOrDisconnected( int errCode, const char* errMsg )
{
	PLog( "PROJECTClientConnection::closed: %d '%s'", errCode, errMsg );
	return CommClientConnection::closedOrDisconnected(errCode, errMsg);
}

void PROJECTClientGConnection::connected()
{
	PLog("PROJECTClientGConnection::connected()");
	isconnected = 1;
}

bool PROJECTClientGConnection::closedOrDisconnected( int errCode, const char* errMsg )
{
	isconnected = 0;
	PLog( "PROJECTClientGConnection::closed: %d '%s'", errCode, errMsg );
	return CommClientGConnection::closedOrDisconnected(errCode, errMsg);
}

void PROJECTClientAuthConnection::connected()
{
}

bool PROJECTClientAuthConnection::closedOrDisconnected( int errCode, const char* errMsg )
{
	PLog( "PROJECTClientAuthConnection::closed: %d '%s'", errCode, errMsg );
	return CommClientGConnection::closedOrDisconnected(errCode, errMsg);
}

void PROJECTServerAuthConnection::getUserAuth( const char* guardType, const char* user_, CommMsgBody& clientExtra, CommMsgBody& authBody )
{
	user.assign(user_);
	CommMsgBody request;
	request.
		composeString(user_).
		composeMsgBody(authBody).
		composeMsgBody(clientExtra);
	obj->requestUserAuth(request, this);
}

void PROJECTServerObject::init(CommMsgBody& initMsg)
{
	CommMsgParser parser(initMsg);
	const char* initDir_;
	parser.parseString(initDir_);
	initDir.assign(initDir_);
	fullFileName.assign(initDir_);
	fullFileName.append("PROJECT.ini");
	PIniFile ini(fullFileName);

	PIniFile::Section* sec = ini.getSection( "----" );
	if( sec )
	{
		int i;
		int n = sec->items.size();
		for( i=0; i < n ; ++i )
		{
			PIniFile::Item& item = sec->items[ i ];
			PIniFile::Section* section = ini.getSection( item.value );
			if (section)
			{
				const char* server = section->getProperty( "server" );
				PASSERT(server);
				const char* serverObject = section->getProperty( "serverobject" );
				PASSERT( serverObject );
				const char* serverType = section->getProperty( "servertype", "" );
				PASSERT( serverType );
			}
		}
	}

	dbManager.init( this );
	PString sectionName("MAINSTORAGE");
	if (!dbManager.openDataStorage(fullFileName, sectionName))
	{
		throw PError("ERROR: Main data storage not open");
	}

	dynamicInit();
	initGenerator( &dbManager, &srvPool, &cliPool, &guardFactoryClient, ini ); // PYR-27418
	registerAllFactories();
}

void PROJECTServerObject::dynamicInit()
{
	PIniFile ini(fullFileName);
	PIniFile::Section* sec;
	sec = ini.getSection("authdbm");
	if( sec )
	{
		const char* server = sec->getProperty( "server" );
		PASSERT( server );
		const char* serverObject = sec->getProperty( "serverobject" );
		PASSERT( serverObject );
		const char* serverType = sec->getProperty( "servertype", "" );
		if (!connectionToAuthDbm || !connectionToAuthDbm->isTheSame(server,serverObject,serverType))
		{
			if (connectionToAuthDbm)
			{
				connectionToAuthDbm->closedOrDisconnected(0,"");
				cliPool.disconnect(connectionToAuthDbm->underlyingConnection());
				delete connectionToAuthDbm;
				connectionToAuthDbm = NULL;
			}
			connectionToAuthDbm = new PROJECTClientAuthConnection(*this,guardFactoryClient);
			cliPool.connect( connectionToAuthDbm->underlyingConnection(), server, serverObject, serverType );
		}
	}

	sec = ini.getSection("LOGGER");
	if (sec)
	{
		useLogger = (0 != sec->getIntProperty("useLogger", 0));
		PString loggerServerAddress = sec->getProperty("LoggerAddress", "");
		PString loggerStationName = sec->getProperty("LogStationName", "");
		if (useLogger && !loggerServerAddress.equals("") && !loggerStationName.equals(""))
		{
			startHeLogger(loggerServerAddress, loggerStationName);
		}
		else
		{
			PTRACE("No LOGGER started");
		}
	}
	else
	{
		useLogger = false;
		PTRACE("No LOGGER configured");
	}
	// PYR-27418
	Parent::dynamicInit( ini );
}

void PROJECTServerObject::requestUserAuth( CommMsgBody & request,PROJECTServerAuthConnection * conn)
{
	PASSERT3(connectionToAuthDbm);
	connectionToAuthDbm->post( REQUEST_AUTH, request, new _PROJECTAuthAsyncCall( this, conn->id() ) );
}

void PROJECTServerObject::receiveAuthFromDbm( CommMsgParser& parser, UINT32 connId )
{
	INT16 err_code;

	CommMsgBody authBody;
	CommServerConnection* conn_ = srvPool.findConnection(connId);

	PROJECTServerAuthConnection * conn = dynamic_cast<PROJECTServerAuthConnection*>(conn_);
	parser.parseINT16( err_code );
	if (conn)
	{
		if ( 0 != err_code )
		{
			const char * errDescr;
			parser.parseString( errDescr );
			conn->gotUserAuth( 0, err_code, errDescr );
		}
		else
		{
			parser.parseMsgBody(authBody);
			CommMsgParser parser2( authBody );
			if (conn->gotUserAuth(&parser2))
			{
				PTRACE3("receiveAuthFromDbm - auth granted");
			}
			else
			{
				PTRACE3("receiveAuthFromDbm - auth not granted");			
			}
		}
	}
}

void PROJECTServerObject::processCommonServerMessage(UINT32 msgId, const CommMsgBody& body, CommServerConnection* conn_, PROJECTConnectionType type)
{
	HiresCounter counter;
	PTRACE("{");

	switch (type)
	{

	case SomeTypeDbm:
		{
			PROJECTServerConnection* conn  = dynamic_cast<PROJECTServerConnection*>(conn_);
			processServerMessage(msgId, body, conn);
			break;
		}

	case TrustedAdmin:
		{
			AdminConnection* conn = dynamic_cast<AdminConnection*>(conn_);
			processAdminMessage(msgId, body, conn);
			break;
		}
	default:
		{
			// Only if not developed properly
			throw PError("Unsupported connection type.");
		}
	}

	counter.stop();
	UINT32 ticks = counter.getElapsedTick();
	ticksAccumulator.addMessageTicks(counter);

	PTRACE("}E %04d %05d%s", msgId, ticks, ( ticks > TICKS_PER_SECOND ) ? " overspan" : "");
}

void PROJECTServerObject::processServerMessage( UINT32 msgId, const CommMsgBody & body, PROJECTServerConnection* conn )
{
	bool connected = true;
	for (int n = 0; n < dbManager.getMaxDbReconnectAttempts(); n++)
	{
		try
		{
			if (connected)
			{
				processServerDbmMessage(msgId, body, conn);
				break;
			}
		}
		catch (const PDbDisconnectError& er)
		{
			PLog("processServerMessage...Disconnect from database detected, reason: '%s', trying to reconnect, attempt #%d,", er.why(), n);
		}

		PThread::sleep(dbManager.getDbReconnectInterval());
		PString sectionName("MAINSTORAGE");
		if (dbManager.openDataStorage(fullFileName, sectionName))
		{
			PLog("processServerMessage...reconnect OK");
			connected = true;
		}
		else
		{
			PLog("processServerMessage error ...attempt #%d failed", n);
			connected = false;
		}
	}
}

void PROJECTServerObject::processServerDbmMessage( UINT32 msgId, const CommMsgBody & body, PROJECTServerConnection* conn )
{
	UINT32 reqId;
	CommMsgParser parser(body);
	parser.
		parseUINT32(reqId);
	CommMsgBody reply;
	reply.composeUINT32(reqId);
	conn->post(SOMETHING_ANSWER, reply);
}

bool PROJECTServerObject::checkAdminRight(const char* rightName, UINT32 replyMsgId, UINT32 reqId, const vector<PString>& adminRights, CommServerConnection* conn) const
{
	for (vector<PString>::const_iterator it = adminRights.begin(); it != adminRights.end(); ++it)
	{
		if (it->equals(rightName))
		{
			return true;
		}
	}
	CommMsgBody reply;
	reply
		.composeUINT32( reqId )
		.composeINT16( NOT_ALLOWED )
		.composeString( "NOT_ENOUGH_RIGHTS" )
		;
	conn->post(replyMsgId, reply);
	PLog("checkAdminRight - NOT_ENOUGH_RIGHTS [%u:%08X]", replyMsgId, reqId);
	return false;
}

void PROJECTServerObject::processAdminMessage(UINT32 msgId, const CommMsgBody& body, AdminConnection* conn)
{
	UINT32 reqId;
	CommMsgParser wrapperParser( body );
	wrapperParser.parseUINT32( reqId );
	MsgAuthRedirectorToGateway msg( wrapperParser );
	PLog("msgId = %d from %s", msgId, msg.adminId.c_str());

	CommMsgParser msgParser( msg.msgBody );

	switch (msgId)
	{

	case MSG_ADMIN_REREAD_INI:
		{
			PLog("[PROJECTServerObject::processAdminMessage] MSG_ADMIN_REREAD_INI has been received");

			if (!checkAdminRight(SYSADMIN_STR, MSG_ADMIN_REREAD_INI_REPLY, reqId, msg.adminRights, conn))
			{
				break;
			}

			try
			{
				dynamicInit();
				CommMsgBody reply;
				reply
					.composeUINT32(reqId)
					.composeINT16(0);
				conn->post(MSG_ADMIN_REREAD_INI_REPLY, reply);

				PLog("[PROJECTServerObject::processAdminMessage] Dynamic section of INI File has been successfully reloaded");
			}
			catch(const PError& er)
			{
				CommMsgBody reply;
				reply
					.composeUINT32(reqId)
					.composeINT16(1)
					.composeString(er.why());
				conn->post(MSG_ADMIN_REREAD_INI_REPLY, reply);

				PLog("[PROJECTServerObject::processAdminMessage] Exception: %s", er.why());
			}

			break;
		}

	case MSG_ADMIN_KILL_ITSELF:
		{
			PLog("[PROJECTServerObject::processAdminMessage] MSG_ADMIN_KILL_ITSELF has been received");

			if (!checkAdminRight(HANDLE_KILL_ITSELF, MSG_ADMIN_KILL_ITSELF_REPLY, reqId, msg.adminRights, conn))
			{
				break;
			}

			CommMsgBody reply;
			reply
				.composeUINT32(reqId)
				.composeINT16(0);
			conn->post(MSG_ADMIN_KILL_ITSELF_REPLY, reply);

			PLog("[PROJECTServerObject::processAdminMessage] MSG_ADMIN_KILL_ITSELF");
			shutdown();
			exit();
			break;
		}

	default:
		{
			PLog("PROJECTServerObject::processAdminMessage =====> Unsupported message msgId=%d", msgId);
			break;
		}
	}
}

void PROJECTServerObject::processServerAuthMessage( UINT32 msgId, const CommMsgBody & body, PROJECTServerAuthConnection* conn )
{
	UINT32 reqId;

	CommMsgParser parser(body);
	parser.
		parseUINT32(reqId);

	CommMsgBody reply;
	reply.composeUINT32(reqId);
	conn->post(SOMETHING_ANSWER, reply);
}


void PROJECTServerObject::processClientGMessage( UINT32 requestId, UINT32 msgId, const CommMsgBody & body )
{
	switch( msgId )
	{
	case 1:
		{
			CommMsgParser parser( body );
			break;
		}
	}
}

void PROJECTServerObject::processClientMessage( UINT32 msgId, const CommMsgBody & body )
{
	switch( msgId )
	{
	case 1:
		{
			CommMsgParser parser( body );
			UINT reqId;
			parser.parseUINT32(reqId);
			break;
		}
	}
}


void PROJECTServerObject::processClientAuthMessage( UINT32 requestId, UINT32 msgId, const CommMsgBody & body)
{
	switch( msgId )
	{
	case 1:
		{
			CommMsgParser parser( body );
			UINT reqId;
			parser.parseUINT32(reqId);
			break;
		}
	}
}

void PROJECTAsyncCall::processReply(UINT32 requestId, UINT32 msgId_, const CommMsgBody& body)
{
	HiresCounter counter;
	PTRACE("{");

	CommMsgParser parser(body);
	switch (msgId_)
	{
	case SOMETHING_ANSWER:
		{
			// obj->processReply_SOMETHING_ANSWER(parser);
			break;
		}
	default:
		{
			PLog("PROJECTAsyncCall - unsupported msgId: %d", msgId_);
			break;
		}
	}

	counter.stop();
	UINT32 ticks = counter.getElapsedTick();
	obj->ticksAccumulator.addMessageTicks(counter);
	PTRACE("}E %04d %05d%s", msgId_, ticks, ( ticks > TICKS_PER_SECOND ) ? " overspan" : "");
}

//
// HeLogger and performance reporting
//

void PROJECTServerObject::startHeLogger(const char * strServerAddress, const char * strStationName )
{
	PLog("PROJECTServerObject::startHeLogger()");
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
		heLoggerTimerHandle = postTimerMessage( DBM_TIMER_LOGGER, timerBody, TIMER_LOGGER_MSEC );
	}
}

void PROJECTServerObject::stopHeLogger()
{
	PLog("PROJECTServerObject::stopHeLogger()");

	if( heLoggerTimerHandle )
	{
		deleteTimerMessage(heLoggerTimerHandle);
		heLoggerTimerHandle = 0;
	}
}

void PROJECTServerObject::reportPerformance()
{
	PTRACE("PROJECTServerObject::reportPerformance()");

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

			strValue.assign("").appendInt(inQueueAccumulator.getAverage());
			heLogger->compose (reportBody, "avgInQueueSize", strValue);

			strValue.assign("").appendInt(inQueueAccumulator.getMax());
			heLogger->compose (reportBody, "maxInQueueSize", strValue);

			reportToHeLogger( *heLogger, reportBody ); // PYR-27418 - inserted this call to parent to send generator data to Monitor

			inQueueAccumulator.reset();

			heLogger->post(reportBody);
		}
		catch( const PError& er )
		{
			PLog( "PROJECTServerObject::reportPerformance() error: '%s'", er.why() );
		}
	}
}

//
// Timer message processing
//

void PROJECTServerObject::processTimerMessage( UINT32 /*handle*/, UINT32 msgId, CommMsgBody& body )
{
	UINT32 ticks = SYSTEMTICKS();
	PTRACE("{ timer Q=%d msgId=%d", getInQueueSize(), msgId );

	CommMsgParser parser(body);

	switch( msgId )
	{
	case DBM_TIMER_LOGGER:
		{
			reportPerformance();
			CommMsgBody timerBody;
			heLoggerTimerHandle = postTimerMessage( DBM_TIMER_LOGGER, timerBody, TIMER_LOGGER_MSEC );
			break;
		}
	}
	ticks = SYSTEMTICKS() - ticks;
	PTRACE("}I %04d %05d%s", msgId, ticks, ( ticks > TICKS_PER_SECOND ) ? " overspan" : "");
}

/*virtual*/ void PROJECTServerObject::shutdown() // override
{
	if(heLogger)
	{
		stopHeLogger();
		delete heLogger;
		heLogger = NULL;
	}
}

// PYR-27418
// NB!!! This method may be called several times by various frameworks, but only the last time must succeed!
// If your Dbm needs other conditions to be met before factories can be opened, then you would have to
// add those conditions to your implementation of this method.
// In this generic example there's only one condition - Id generator must be fully initialized.
bool PROJECTServerObject::registerAllFactories() // override
{
	if( !Parent::registerAllFactories() )
	{
		// Id generator not ready yet
		return false;
	}
	srvPool.registerFactory( "SomeType", connPROJECTFactory );
	srvPool.registerFactory( "auth", connPROJECTAuthFactory );
	srvPool.registerFactory( "trustedAdmin", adminTrustedFactory );

	return true;
}