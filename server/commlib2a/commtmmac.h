#ifndef commtmmac_h_included
#define commtmmac_h_included


#if defined( P_MAC10X ) && !defined( P_IOS )

#include <CoreFoundation/CoreFoundation.h>

#include "commmsgbody.h"
#include "commqueue.h"
#include "commrtable.h"

class CommMac10XThreadManager;
class CommMac10XInterface : public _CommClientOnlyInterface
{
private:
	friend class CommMac10XThreadManager;
	CommMac10XThreadManager* mgr;
	_CommPhysicalConnection* physConn;

public:
	CommMac10XInterface()
	{
		mgr = 0;
		physConn = 0;
	}
	void init(_CommPhysicalConnection * physConn_)
	{
		physConn = physConn_;
	}

public:
	int _getOutQueueSize() const override { return 0; }
	void _post( _CommMsg& msg ) override;
	UINT32 _postClientTimer( _CommMsg& msg, int msec ) override;
	void _internalPost(_CommMsg& msg ) override;
};

class CommMac10XThreadManager : public CommThreadManagerWithTimerThread
{
private:
	typedef CommThreadManager _Parent;

public:
	CommMac10XThreadManager( CommRoutingTable& routingTable_ );
	void localPush(_CommPhysicalConnection* physConn, _CommMsg& m );
	void virtualPush( _CommPhysicalConnection* physConn, /*non-const!*/ _CommMsg& m ) override;
	void virtualPushRead( _CommPhysicalConnection* physConn, /*non-const!*/ _CommMsg& m ) override;

	void forceDisconnect( _CommPhysicalConnection* physConn ) override;// { PASSERT3( 0 ); }
	void start() override;
	void shutdown() override;
	bool terminated() override;
	/* virtual */ ~CommMac10XThreadManager();

public:
	void createMac10XTransport( CommMac10XInterface& inter, const char * portName, SInt32 msgId_ );
	void destroyTransport( CommMac10XInterface& inter );

public:
	static UINT32 translateMacEvent( _CommMsg& ret, CFDataRef datatRef);//returns message counter

private://helpers
	void _createTransport( CommMac10XInterface& inter,const char * name, SInt32 msgId_);
	void _pushToThread( _CommManageableThread* dummyThread, _CommMsg& m ) override;
};

#endif // defined( P_MAC10X ) && !defined( P_IOS )

#endif
