#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"

#include "commmsgbody.h"
#include "commqueue.h"
#include "commsrv.h"
#include "commgrdwebtokenint.h"

/* static */ const char* CommServerWebTokenIntGuard::userAuthHandled()
{
	return _COMM_WEBTOKENINT_GUARD_NAME;
}


/* static */ void CommServerWebTokenIntGuard::processUserAuth( CommMsgBody& ret, CommMsgParser& parser)
{
	INT32 peerVersion;
	parser.parseINT32(peerVersion);
	if (peerVersion != _version)
	{
		PTRACE3(" CommServerWebTokenIntGuard::processUserAuth: requested version %d differs from current %d", peerVersion, _version);
	}
	ret.composeINT32(_version);
}

