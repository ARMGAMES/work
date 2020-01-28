// ===========================================================
// Copyright (C) 2013 PYR Software Ltd. All rights reserved.
// ===========================================================
#ifndef dbmserverobjectcommon_h_included
#define dbmserverobjectcommon_h_included

#include "commgrdnull.h"
#include "dbcommon.h"

// PYR-27418
class DbmServerObjectCommon;
class CommMonitorLoggerObject;

#pragma warning( push )
#pragma warning( disable: 4355 )

#define GENERATOR_CONNECTION	"generator_connection"	// PYR-26980

class DbmCommonServerConnection : public CommServerConnection
{
protected:
	DbmServerObjectCommon* obj;

public:
	DbmCommonServerConnection( DbmServerObjectCommon* obj_ )
		: obj( obj_ )
	{}
	void processMessage( UINT32 msgId, const CommMsgBody& body ) override;
	void connected() override
	{
		PLog( "DbmCommonServerConnection: connected: %X", id() );
	}
	void closedOrDisconnected( int errCode, const char* errMsg ) override
	{
		PLog( "DbmCommonServerConnection::closed: %d '%s'", errCode, errMsg );
		return CommServerConnection::closedOrDisconnected( errCode, errMsg );
	}
};

class DbmCommonServerConnFactory : public CommServerConnectionFactory
{
private:
	DbmServerObjectCommon* obj;

public:
	DbmCommonServerConnFactory( CommServerGuardFactory& guardFactory, DbmServerObjectCommon* obj_ )
		: CommServerConnectionFactory( guardFactory )
		, obj( obj_ )
	{
	}

	/* new */ CommServerConnection* createConnection() override
	{
		return new DbmCommonServerConnection( obj );
	}
};

class IdRangeClientConnection;

class DbmServerObjectCommon : public CommServerObject
{
private:
	typedef CommServerObject Parent;

	static CommServerNullGuardFactory serverGuardFactoryCommon;
	CommServerConnectionPool* srvPoolCommon;
	CommClientConnectionPool* cliPoolCommon;
	DbmCommonServerConnFactory connDbmCommonFactory;
	DatabaseManagerCommon* dbManagerCommon;

	IdRangeClientConnection* idRangeConn; // connection (if needed to IOM site to get ID ranges)
	class InitGeneratorCallback : public DbmGenerator::DbmGeneratorCallback // to register factories upon generator init
	{
		DbmServerObjectCommon* obj;

	public:
		InitGeneratorCallback( DbmServerObjectCommon* obj_ )
			: obj( obj_ )
		{}

		void process() override;
	};
	void connectToMasterGenerator( const PIniFile& iniFile );

protected:
	DbmServerObjectCommon( _CommInterface& inter_ )
		: CommServerObject( inter_ )
		, srvPoolCommon( 0 )
		, cliPoolCommon( 0 )
		, connDbmCommonFactory( serverGuardFactoryCommon, this )
		, dbManagerCommon( 0 )
		, idRangeConn( 0 )
		, generatorReady( false )
		, useMasterGenerator( false ) // by default do not use generator functionality
		, generatorFactoryRegistered( false ) // to detect double-registering of generator factory
	{
	}

	virtual ~DbmServerObjectCommon();

	// Must be called once from static init of server object after dbManager initialization
	void initGenerator(
			DatabaseManagerCommon* dbManagerCommon_
			, CommServerConnectionPool* srvPool_
			, CommClientConnectionPool* cliPool_
			, CommClientNullGuardFactory* clientGuardFactory_
			, const PIniFile& iniFile
			);

	virtual void reportToHeLogger( const CommMonitorLoggerObject& heLogger, CommMsgBody& reportBody );

	bool generatorReady;
	bool useMasterGenerator;
	bool generatorFactoryRegistered;

public:
	void processDbmCommonServerMessage( UINT32 msgId, const CommMsgBody& body, DbmCommonServerConnection& conn );
	// !!!NB: the overriding implementation must call the parent implementation.
	// If there's no overriding then this method must be called from dbm-specific dynamic init function.
	virtual void dynamicInit( const PIniFile& iniFile );
	// !!!NB: should be overridden to register dbm-specific factories and must call the parent implementation.
	virtual bool registerAllFactories();
	virtual void reportGeneratorConnStatus( bool connected ) {}// PYR-26980
};

class IdRangeClientConnection : protected CommClientGConnection
{
	DbmServerObjectCommon* obj;
public:
	INT32 idRangeRequestsSent;

	IdRangeClientConnection( CommClientGuardFactory& guardFactory_, DbmServerObjectCommon* obj_ )
		: CommClientGConnection( guardFactory_ )
		, obj( obj_ )
		, idRangeRequestsSent( 0 )
		, isConnected( false )
	{}

	UINT32 postX( UINT32 msgId, CommMsgBody& body, AsyncCall* call )
	{
		++idRangeRequestsSent;
		UINT32 reqId = post( msgId, body, call );
		PLog(">IDR%c %u %X", isConnected ? '+' : '-', msgId, reqId);
		return reqId;
	}

	CommClientGConnection::underlyingConnection;
	bool closedOrDisconnected( int errCode, const char* errMsg ) override
	{
		PLog( "idRangeConn disconnected: %X", underlyingConnection().id() );
		isConnected = false;
		obj->reportGeneratorConnStatus( false );
		return CommClientGConnection::closedOrDisconnected( errCode, errMsg );
	}
	bool getIsConnected() const { return isConnected; }

private:
	bool isConnected;

	void connected() override
	{
		PLog( "idRangeConn connected: %X", underlyingConnection().id() );
		isConnected = true;
		obj->reportGeneratorConnStatus( true );
	}

	virtual void processGMessage( UINT32 requestId, UINT32 msgId, const CommMsgBody& body, AsyncCall* call ) override;
};

#pragma warning( pop )

#endif // dbmserverobjectcommon_h_included
