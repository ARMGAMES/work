
#include "SrvTemplateDbmobject.h"

#define DEFAULT_PORT 8000
#define DEFAULT_INSTANCE "SrvTemplateDbmInstance"

//****************************************************************************

class Daemon : public PDaemon
{
	CommRoutingTable theRoutingTable;
	CommRWThreadManager rw;
	CommLocalThreadManager local;
	SrvTemplateDbmServerObjectFactory factory;

public:
	Daemon();
	/* virtual */ void start( const char* dir, int nParms, char** parms );
	/* virtual */ void stop();
};

Daemon::Daemon() : rw( theRoutingTable ), local( theRoutingTable ) {}

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

	local.registerServerObjectFactory( "SrvTemplateDbm", factory );
	CommMsgBody initBody;
	initBody.composeString( "" );

	const char* instanceName = nParms > 1 ? parms[1] : DEFAULT_INSTANCE;
	local.createServerObject("SrvTemplateDbm", instanceName, &initBody);

	theRoutingTable.start();
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
	return PWin32ServiceDaemonMain( daemon, argc, argv, "pyr.log", "PYRGFSL" );
}

