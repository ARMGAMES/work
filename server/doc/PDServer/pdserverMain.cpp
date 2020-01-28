////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2019 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "ppstring.h"
#include "ppdaemon.h"
#include "commtmrw.h"
#include "commtmlocal.h"
#include "pdserverSrvObject.h"

#define DEFAULT_PORT 8602
#define DEFAULT_INSTANCE "pdserverInstance"

//****************************************************************************

class Daemon : public PDaemon
{
private:
	CommRoutingTable theRoutingTable;
	CommRWThreadManager tm;
	CommLocalThreadManager local;
	PDServerObjectFactory factory;

public:
	Daemon();
	/* virtual */ void start(const char* dir, int nParms, char** parms) override;
	/* virtual */ void stop() override;
};

Daemon::Daemon()
	: tm(theRoutingTable), local(theRoutingTable)
{

}

void Daemon::start(const char* dir, int nParms, char** parms)
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
	tm.listenPort(realPortNo);

	PString alias("ip:*:");
	alias.appendInt(realPortNo);

	theRoutingTable.addAlias(alias);
	CommRule rule;
	rule.mask = "ip:*:*";
	rule.conns.push_back("pyrtcp0://$0:$1");
	theRoutingTable.addRule(rule);

	local.registerServerObjectFactory("PDServer", factory);
	CommMsgBody initBody;
	initBody.composeString("");

	const char* instanceName = nParms > 1 ? parms[1] : DEFAULT_INSTANCE;
	local.createServerObject("PDServer", instanceName, &initBody);

	theRoutingTable.start();
}

void Daemon::stop()
{
	theRoutingTable.shutdown();
	bool Ok = theRoutingTable.waitUntilTerminated(5);
	if (Ok)
	{
		PLog("Daemon::stop(): Threads terminated");
	}
	else
	{
		PLog("Daemon::stop(): Threads not terminated - exiting anyway");
	}

	PSocket::cleanup();
}

//*****************************************************************************

int main(int argc, char** argv)
{
	Daemon daemon;
	return PWin32ServiceDaemonMain(daemon, argc, argv, "col_pdserver.log", "PYRPDSERVER");
}
