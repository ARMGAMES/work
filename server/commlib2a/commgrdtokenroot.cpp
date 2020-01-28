#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"

#include "commmsgbody.h"
#include "commqueue.h"
#include "commsrv.h"
#include "commgrdtoken.h"

/* static */ const char* CommServerTokenGuard::userAuthHandled()
{
	return _COMM_TOKEN_GUARD_NAME;
}


/* static */ void CommServerTokenGuard::processUserAuth( CommMsgBody& ret, CommMsgParser& parser, const BYTE * token, int tokenSize, const BYTE * token2, int tokenSize2 )
{
	INT32 peerVersion;
	parser.parseINT32(peerVersion);
	if (peerVersion != _version)
	{
		PTRACE3(" CommServerTokenGuard::processUserAuth: requested version %d differs from current %d", peerVersion, _version);

	}
	ret.composeINT32(_version);
	ret._composeVarBlock( token,tokenSize);
	if (tokenSize2)
		ret._composeVarBlock( token2,tokenSize2);
}

/*static*/  void CommServerTokenGuard::composeSecurityToken( CommMsgBody& ret, const BYTE * token, int tokenSize)
{
	ret.composeINT32(_version)._composeVarBlock( token,tokenSize);
}
