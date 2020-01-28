#ifndef commtmwin_h_included
#define commtmwin_h_included

#ifdef PWIN32
#include "commmsgbody.h"
#include "commqueue.h"
#include "commrtable.h"

#include <memory>

class CommWin32WinThreadManager;
class CommWin32WinInterface : public _CommClientOnlyInterface
{
private:
	friend class CommWin32WinThreadManager;
	CommWin32WinThreadManager* mgr;
	_CommPhysicalConnection* physConn;

public:
	CommWin32WinInterface()
	{
		mgr = 0;
		physConn = 0;
	}

public:
	void _post( _CommMsg& msg ) override;
	UINT32 _postClientTimer( _CommMsg& msg, int msec ) override;
	void _internalPost(_CommMsg& msg ) override;
	int _getOutQueueSize() const override { return 0; }
};

class CommWin32WinThreadManager : public CommThreadManagerWithTimerThread
{
private:
	typedef CommThreadManager _Parent;

private:
	UINT winMsg;

public:
	CommWin32WinThreadManager( UINT winMsg_, CommRoutingTable& routingTable_ );
	void virtualPush( _CommPhysicalConnection* physConn, /*non-const!*/ _CommMsg& m ) override;
	void virtualPushRead( _CommPhysicalConnection* physConn, /*non-const!*/ _CommMsg& m ) override;
	void forceDisconnect( _CommPhysicalConnection* physConn ) override;// { PASSERT3( 0 ); }
	void start() override;
	void shutdown() override;
	bool terminated() override;
	/* virtual */ ~CommWin32WinThreadManager();

public:
	void createWindowTransport( CommWin32WinInterface& inter, HWND wnd );
	void createThreadTransport( CommWin32WinInterface& inter, DWORD threadId );
	void destroyTransport( CommWin32WinInterface& inter );

public:
	static UINT32 translateWinMsg( _CommMsg& ret, WPARAM w, LPARAM l );//returns message counter
	static unique_ptr< _CommMsg > translateWinMsg( WPARAM w, LPARAM l );
	static const _CommMsg& peekTranslateWinMsg( WPARAM w, LPARAM l );

private://helpers
	void _createTransport( CommWin32WinInterface& inter, bool isWnd, HWND wnd, DWORD threadId );
	void _pushToThread( _CommManageableThread* dummyThread, _CommMsg& m ) override;
};

#endif

#endif
