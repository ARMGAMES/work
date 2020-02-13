// ===========================================================
// Copyright (C) 2013 PYR Software Ltd. All rights reserved.
// ===========================================================

#include "dbmserverobjectcommon.h"
#include "counters.h"
#include "HeLogger.h"
#include "dbmmsg.h"
#include "dbm.h"

// PYR-27418
/*static*/ class CommServerNullGuardFactory DbmServerObjectCommon::serverGuardFactoryCommon;

void IdRangeClientConnection::processGMessage( UINT32 requestId, UINT32 msgId, const CommMsgBody& body, AsyncCall* call ) // override
{
	HiresCounter counter;
	PLog("{IDR %u %X", msgId, requestId);
	if( call )
	{
		call->processReply( requestId, msgId, body );
	}
	else
	{
		PLog("ERROR: unhandled message: !call %u", msgId);
		PASSERT5(0);
	}
	counter.stop();
	PLog("}IDR %u %X %u", msgId, requestId, counter.getElapsedTick());
}

DbmServerObjectCommon::~DbmServerObjectCommon()
{
	delete idRangeConn;
}

void DbmServerObjectCommon::reportToHeLogger( const CommMonitorLoggerObject& heLogger, CommMsgBody& reportBody )
{
	PString strValue;

	strValue.assign("").appendInt(idRangeConn && idRangeConn->getIsConnected() ? 1 : 0);
	heLogger.compose( reportBody, "generatorIdRangeConn", strValue );

	if( idRangeConn )
	{
		strValue.assign("").appendInt(idRangeConn->idRangeRequestsSent);
		heLogger.compose( reportBody, "idRangeRequestsSent", strValue );
	}
}

void DbmServerObjectCommon::processDbmCommonServerMessage( UINT32 msgId, const CommMsgBody& body, DbmCommonServerConnection& conn )
{
	switch( msgId )
	{
	case DBM_Q_GET_GENERATOR_ID_RANGES:
		{
			//47<s7> - reqId, num<objectName,rangeSize>
			INT16 errCode = DBM_NO_ERROR;
			CommMsgParser parser( body );
			UINT32 reqId;
			INT32 numObjects;
			parser
				.parseUINT32( reqId )
				.parseINT32( numObjects )
				;
			CommMsgBody reply;
			//DBM_A_GET_GENERATOR_ID_RANGES - 46[s|7<s88>] reqId, errCode[errDescr|numRanges<objectName,startId,endId>]
			reply
				.composeUINT32( reqId )
				.composeINT16( DBM_NO_ERROR )
				.composeINT32( numObjects )
				;
			PLog( "getIdRange: %d", numObjects );
			for( INT32 i = 0; i < numObjects; ++i )
			{
				const char* objectName;
				INT32 rangeSize;
				parser
					.parseString( objectName )
					.parseINT32( rangeSize )
					;
				UINT64 startId;
				UINT64 endId;
				PString sqlErr;
				PLog( " '%s' %d", objectName, rangeSize );
				errCode = dbManagerCommon->getIdRange( objectName, rangeSize, startId, endId, sqlErr );
				if( errCode )
				{
					PLog( "ERROR getting Id range" );
					reply.clear();
					reply
						.composeUINT32( reqId )
						.composeINT16( errCode )
						.composeString( sqlErr )
						;
					break;
				}
				char s1[32];
				char s2[32];
				PLog( " range: %s - %s", p_i64toa( startId, s1 ), p_i64toa( endId, s2 ) );
				reply
					.composeString( objectName )
					.composeUINT64( startId )
					.composeUINT64( endId )
					;
			}
			conn.post( DBM_A_GET_GENERATOR_ID_RANGES, reply );
			break;
		}
	default:
		PLog( "DbmServerObjectCommon::processDbmCommonServerMessage: unhandled message: %u", msgId );
		break;
	}
}

// This method will be called after slave generator is fully initialized (received all shared ranges)
void DbmServerObjectCommon::InitGeneratorCallback::process() // override
{
	obj->generatorReady = true;
	PLog( "Slave generator ready" );
	obj->registerAllFactories();
}

void DbmServerObjectCommon::initGenerator(
		DatabaseManagerCommon* dbManagerCommon_
		, CommServerConnectionPool* srvPool_
		, CommClientConnectionPool* cliPool_
		, CommClientNullGuardFactory* clientGuardFactory_
		, const PIniFile& iniFile
		)
{
	srvPoolCommon = srvPool_;
	cliPoolCommon = cliPool_;
	dbManagerCommon = dbManagerCommon_;
	generatorReady = true;
	useMasterGenerator = dbManagerCommon->isUseMasterGenerator();
	if( useMasterGenerator && dbManagerCommon->useSharedIds() )
	{
		idRangeConn = new IdRangeClientConnection( *clientGuardFactory_, this );
		connectToMasterGenerator( iniFile );
		InitGeneratorCallback* initFinishedCallback = new InitGeneratorCallback( this );
		generatorReady = dbManagerCommon->initConnectionToMasterGenerator( idRangeConn, initFinishedCallback );
	}
	else // PYR-26980
	{
		reportGeneratorConnStatus(true/*connected*/);
	}
}

void DbmServerObjectCommon::dynamicInit( const PIniFile& iniFile )
{
	if( useMasterGenerator && dbManagerCommon->useSharedIds() )
	{
		connectToMasterGenerator( iniFile );
	}
}

void DbmServerObjectCommon::connectToMasterGenerator( const PIniFile& iniFile )
{
	const PIniFile::Section* deplSection = iniFile.getSection( DEPLOYMENT_SECTION_NAME );
	const char* idRangeAddress = deplSection->getProperty( "idrangeaddress", "" );
	const char* idRangeObject = deplSection->getProperty( "idrangeobject", "" );
	const char* idRangeConnType = deplSection->getProperty( "idrangeconntype", "generator" );
	if( !*idRangeAddress || !*idRangeObject )
	{
		PLog("ERROR: Empty idrangeaddress or idrangeobject in [%s] section", DEPLOYMENT_SECTION_NAME);
		return;
	}
	if( idRangeConn->underlyingConnection().isConnected()
		&& ( !PString::equals( idRangeAddress, idRangeConn->underlyingConnection().getServer() )
		|| !PString::equals( idRangeObject, idRangeConn->underlyingConnection().getServerObject() )
		|| !PString::equals( idRangeConnType, idRangeConn->underlyingConnection().getServerConnType() )
		)
		)
	{
		PLog( "Disconnecting idRangeConn: %s/%s/%s", idRangeConn->underlyingConnection().getServer(), idRangeConn->underlyingConnection().getServerObject(), idRangeConn->underlyingConnection().getServerConnType() );
		idRangeConn->closedOrDisconnected( 0, "" );
		cliPoolCommon->disconnect( idRangeConn->underlyingConnection() );
	}
	if( !idRangeConn->underlyingConnection().isConnected() )
	{
		PLog( "Connecting idRangeConn: %s/%s/%s", idRangeAddress, idRangeObject, idRangeConnType );
		cliPoolCommon->connect( idRangeConn->underlyingConnection(), idRangeAddress, idRangeObject, idRangeConnType );
	}
}

// virtual
bool DbmServerObjectCommon::registerAllFactories()
{
	if( !generatorReady )
	{
		PLog( "registerAllFactories - generator not ready yet" );
		return false;
	}
	if( dbManagerCommon->useSharedIds() ) // IOM dbm is the recipient of the messages
	{
		if( generatorFactoryRegistered )
		{
			PLog( LOG_TRACE_FAULTY_LOGIC ": repeated call to registerAllFactories()!" );
		}
		else
		{
			srvPoolCommon->registerFactory( "generator", connDbmCommonFactory );
			PLog( "'generator' factory opened" );
			generatorFactoryRegistered = true;
		}
	}
	return true;
}

void DbmCommonServerConnection::processMessage( UINT32 msgId, const CommMsgBody& body ) // override
{
	PLog( "{CSC %u", msgId );
	HiresCounter counter;
	obj->processDbmCommonServerMessage( msgId, body, *this );
	counter.stop();
	UINT32 cnt = counter.getElapsedTick();
	PLog( "}CSC %u %u", msgId, cnt );
}
