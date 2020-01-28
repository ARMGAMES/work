#ifndef commtmandroid_h_included
#define commtmandroid_h_included

#ifdef P_ANDROID

#include "commmsgbody.h"
#include "commqueue.h"
#include "commrtable.h"

class CommAndroidThreadManager;
class CommAndroidInterface : public _CommClientOnlyInterface
{
private:
	friend class CommAndroidThreadManager;
	CommAndroidThreadManager* mgr;
	_CommPhysicalConnection* physConn;

public:
	CommAndroidInterface()
	{
		mgr = 0;
		physConn = 0;
	}

public:
	void _post( _CommMsg& msg ) override;
	UINT32 _postClientTimer( _CommMsg& msg, int msec ) override;
	int _getOutQueueSize() const override { return 0; }
	void _internalPost( _CommMsg& msg ) override;
};

class CommAndroidThreadManager : public CommThreadManagerWithTimerThread
{
private:
	typedef CommThreadManager _Parent;

public:
	CommAndroidThreadManager( CommRoutingTable& routingTable_ );
	void virtualPush( _CommPhysicalConnection* physConn, /*non-const!*/ _CommMsg& m ) override;
	void virtualPushRead( _CommPhysicalConnection* physConn, /*non-const!*/ _CommMsg& m ) override;
	void forceDisconnect( _CommPhysicalConnection* physConn ) override;// { PASSERT3( 0 ); }
	void start() override;
	void shutdown() override;
	bool terminated() override;
	/* virtual */ ~CommAndroidThreadManager();

public:
	void createTransport( CommAndroidInterface& inter, CommMsgQueue & queue);
	void destroyTransport( CommAndroidInterface& inter );

public:
	static void translateAndroidMsg( _CommMsg& ret, _CommMsg* androidMsg);

private://helpers
	void _pushToThread( _CommManageableThread* dummyThread, _CommMsg& m ) override;
};

#endif
#endif
