////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2017 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "ColSrvObject.h"
#include "commtmwebsocket.h"

#define	DEFAULT_PORT		5998
#define	DEFAULT_WEB_PORT	6998
#define DEFAULT_INSTANCE	"ColSrvInstance"

//****************************************************************************

class Daemon : public PDaemon
{
private:
	CommRoutingTable theRoutingTable;
	CommRWThreadManager rw;
	CommLocalThreadManager local;
	ColUtilServerObjectFactory factory;
	CommThreadManagerWebSocket * _ws_thread_manager;

public:
	Daemon();
	virtual ~Daemon()
	{
		if (_ws_thread_manager)
			delete _ws_thread_manager;
	}

	/* virtual */ void start( const char* dir, int nParms, char** parms );
	/* virtual */ void stop();
};

Daemon::Daemon()
	: rw( theRoutingTable ),
	local( theRoutingTable ),
	_ws_thread_manager( NULL )
{
}

//
// ColDbm.exe [port] [instance name]
//
void Daemon::start( const char* dir, int nParms, char** parms )
{
	PSocket::startup();

	int portNo = 0;
	if (nParms)
	{
		const char* portstr = parms[0];
		portNo = atoi(portstr);
	}
	int realPortNo = portNo ? portNo : DEFAULT_PORT;
	PLog("Listening port %d", realPortNo);

	rw.listenPort( realPortNo );

	PString alias("ip:*:");
	alias.appendInt(realPortNo);
	theRoutingTable.addAlias(alias);

	CommRule rule;
	rule.mask = "ip:*:*";
	rule.conns.push_back( "pyrtcp0://$0:$1" );
	theRoutingTable.addRule( rule );

	local.registerServerObjectFactory( "ColDbm", factory );

	CommMsgBody initBody;
	initBody.composeString( "" );

	const char* instanceName = nParms > 1 ? parms[1] : DEFAULT_INSTANCE;
	local.createServerObject( "ColDbm", instanceName, &initBody );

	theRoutingTable.start();

	PLog( "[%s] <I> %s listening web port %d", __FUNCTION__, instanceName, DEFAULT_WEB_PORT );
	_ws_thread_manager = new CommThreadManagerWebSocket( theRoutingTable );
	_ws_thread_manager->listenPort( DEFAULT_WEB_PORT );
	_ws_thread_manager->start();

}

void Daemon::stop()
{
	theRoutingTable.shutdown();
	bool Ok = theRoutingTable.waitUntilTerminated( 5 );
	if( Ok )
		PLog( "Daemon::stop(): Threads terminated" );
	else
		PLog( "Daemon::stop(): Threads not terminated - exiting anyway" );

	PSocket::cleanup();
}

//*****************************************************************************

int main( int argc, char** argv )
{
	Daemon daemon;
	return PWin32ServiceDaemonMain( daemon, argc, argv, "colSrv.log", "PYRCOLSRV" );
}
