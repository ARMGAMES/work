#ifndef commconcli_h_included
#define commconcli_h_included

#include "ppinclude.h"
#include "ppthread.h"
#include "commmsgbody.h"
#include "commidmap.h"
#include "commqueue.h"
#include "commrtable.h"

class CommConsoleClientThreadManager;
class CommConsoleClientInterface : public _CommClientOnlyInterface
{
private:
	friend class CommConsoleClientThreadManager;
	int objIdx;
	CommConsoleClientThreadManager* mgr;
	_CommPhysicalConnection* physConn;

public:
	CommConsoleClientInterface( CommConsoleClientThreadManager& mgr_ );
	void _init( _CommPhysicalConnection* physConn_ );

public:
	void _post( _CommMsg& msg ) override;
	UINT32 _postClientTimer( _CommMsg& msg, int msec ) override;
	int _getOutQueueSize() const override
	{
		return 0;
	}
};

/* abstract */ class CommConsoleClientObject
{
private:
	friend class CommConsoleClientThreadManager;
	CommConsoleClientInterface* inter;

public:
	CommConsoleClientObject( CommConsoleClientInterface& inter_ );

public:
	virtual void processCommand( const char* cmd ) {}
	virtual void processMessage( _CommMsg& msg ) = 0;
};

class CommConsoleClientThreadManager : public CommThreadManagerWithTimerThread
{
private:
	typedef CommThreadManager _Parent;

private:
	struct _Object
	{
		int threadIdx;
		CommConsoleClientObject* obj;
		CommConsoleClientInterface* inter;
	};

private:
	vector< _Object > objs;

public:
	CommConsoleClientThreadManager( CommRoutingTable& routingTable_ );
	CommConsoleClientInterface& createInterface();
	void registerObject( CommConsoleClientObject& obj_ );
	void start() override;
	void shutdown() override;
	bool terminated() override;
	/* virtual */ ~CommConsoleClientThreadManager();
	void forceDisconnect( _CommPhysicalConnection* physConn ) override {} //ignore
	const char * whoAreYou() const override 
	{
		return "CommConsoleClientThreadManager";
	} 
	void loggerInformation(CommMsgBody &body) override {}
	void pushCommand( const char* s );

public:
	void _pushToThread(_CommManageableThread* dummyThread_, _CommMsg& m) override;
};

#endif
