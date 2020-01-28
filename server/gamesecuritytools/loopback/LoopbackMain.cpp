////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2013 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "commlib.h"
#include "LoopbackObject.h"

#define DEFAULT_PORT 8000
#define DEFAULT_INSTANCE "LoopbackInstance"

////////////////////////////////////////////////////////////////////////////////

class Daemon : public PDaemon
{
private:
	CommRoutingTable theRoutingTable;
	CommRWThreadManager rw;
	CommLocalThreadManager local;
	LoopbackServerObjectFactory factory;

public:
	Daemon() : rw(theRoutingTable), local(theRoutingTable) {}
	void start(char const* dir, int nParms, char** parms) override;
	void stop() override;
};

////////////////////////////////////////////////////////////////////////////////

void Daemon::start(char const* /*dir*/, int nParms, char** parms)
{
	PSocket::startup();

	int portNo = 0;
	if (nParms)
	{
		char const* portstr = parms[0];
		portNo = atoi(portstr);
	}

	int realPortNo = portNo ? portNo : DEFAULT_PORT;
	PLog("Listening port %d", realPortNo);

	rw.listenPort(realPortNo);

	PString alias("ip:*:");
	alias.appendInt(realPortNo);
	theRoutingTable.addAlias(alias);

	CommRule rule;
	rule.mask = "ip:*:*";
	rule.conns.push_back("pyrtcp0://$0:$1");
	theRoutingTable.addRule(rule);

	local.registerServerObjectFactory("Loopback", factory);

	CommMsgBody initBody;
	initBody.composeString("");

	char const* instanceName = nParms > 1 ? parms[1] : DEFAULT_INSTANCE;
	local.createServerObject("Loopback", instanceName, &initBody);

	theRoutingTable.start();
}

////////////////////////////////////////////////////////////////////////////////

void Daemon::stop()
{
	theRoutingTable.shutdown();
	bool Ok = theRoutingTable.waitUntilTerminated(5);
	if (Ok)
		PLog("Daemon::stop(): Threads terminated");
	else
		PLog("Daemon::stop(): Threads not terminated - exiting anyway");

	PSocket::cleanup();
}

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv)
{
	Daemon daemon;
	return PWin32ServiceDaemonMain(daemon, argc, argv, "Loopback.log", "PyrLoopback");
}

////////////////////////////////////////////////////////////////////////////////
