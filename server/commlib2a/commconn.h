#ifndef commconn_h_included
#define commconn_h_included

#include "ppinclude.h"
#include "ppstring.h"
#include "pplogfile.h"
#include "commmsgbody.h"

// definitions for extended connection
#define CONN_EX_DISCONNECT_TIME_OUT 30000
#define CONN_EX_POLL_TIME 100

class _CommConnection
{
public:
	enum { _StateConnDisc = 0, _StateConnRequestSent = 1, _StateConnPeerAssigned = 2, _StateConnGuardNegotiationNoSession = 3, _StateConnGuardNegotiation = 4, _StateConnOk = 5, _StateConnServerClosed = -1, _StateConnServerFatalError = -2, _StateConnClientFatalError = -3 };

public:
	static void _wrapUserMsg( CommMsgBody& body, UINT32 msgId );
	static void _unwrapUserMsg( UINT32& msgId, CommMsgBody& body );
};

#endif
