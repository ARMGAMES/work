#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"

#include "commmsgbody.h"
#include "commqueue.h"
#include "commsrv.h"
#include "commgrdwebtoken.h"

/* static */ const char* CommServerWebTokenGuard::userAuthHandled()
{
	return _COMM_WEBTOKEN_GUARD_NAME;
}


/* static */ void CommServerWebTokenGuard::processUserAuth( CommMsgBody& ret, CommMsgParser& parser)
{
	INT32 peerVersion;
	parser.parseINT32(peerVersion);
	if (peerVersion != _version)
	{
		PTRACE3(" CommServerWebTokenGuard::processUserAuth: requested version %d differs from current %d", peerVersion, _version);
	}
	ret.composeINT32(_version);
}

