#ifndef commtmios_h_included
#define commtmios_h_included

#if defined( P_IOS )

#include "commmsgbody.h"
#include "commqueue.h"
#include "commrtable.h"

class CommIOSThreadManager;
class CommIOSInterface : public _CommClientOnlyInterface
{
private:
	friend class CommIOSThreadManager;
	CommIOSThreadManager* mgr;
	_CommPhysicalConnection* physConn;

public:
	CommIOSInterface()
	{
		mgr = 0;
		physConn = 0;
	}

public:
	void _post( _CommMsg& msg ) override;
	UINT32 _postClientTimer( _CommMsg& msg, int msec ) override;
	int _getOutQueueSize() const override { return 0; }
};

class CommIOSThreadManager : public CommThreadManagerWithTimerThread
{
private:
	typedef CommThreadManager _Parent;

public:
	CommIOSThreadManager( CommRoutingTable& routingTable_ );
	void virtualPush( _CommPhysicalConnection* physConn, /*non-const!*/ _CommMsg& m ) override;
	void virtualPushRead( _CommPhysicalConnection* physConn, /*non-const!*/ _CommMsg& m ) override;
	void forceDisconnect( _CommPhysicalConnection* physConn ) override;// { PASSERT3( 0 ); }
	void start() override;
	void shutdown() override;
	bool terminated() override;
	/* virtual */ ~CommIOSThreadManager();

public:
	void createTransport( CommIOSInterface& inter, CommMsgQueue & queue );
	void destroyTransport( CommIOSInterface& inter );

private://helpers
	void _pushToThread( _CommManageableThread* dummyThread, _CommMsg& m ) override;

};

#endif // defined( P_IOS )

#endif
