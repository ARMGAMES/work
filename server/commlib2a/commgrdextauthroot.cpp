#include "commgrdextauth.h"

namespace
{

void processUserAuthImpl(CommMsgBody& ret, CommMsgParser& parser, INT32 version, const char* guardName)
{
	INT32 peerVersion;
	parser.parseINT32(peerVersion);
	if (peerVersion != version)
	{
		PTRACE3(" %s::processUserAuth: requested version %d differs from current %d", guardName, peerVersion, version);
	}
	ret.composeINT32(version);
}

}

/* static */ const char* CommServerExternAuthGuard::userAuthHandled()
{
    return _COMM_EXTERN_AUTH_GUARD_NAME;
}

/* static */ void CommServerExternAuthGuard::processUserAuth( CommMsgBody& ret, CommMsgParser& parser)
{
	processUserAuthImpl(ret, parser, _version, "CommServerExternAuthGuard");
}

/* static */ const char* CommServerExternAuthOperatorTokenGuard::userAuthHandled()
{
	return _COMM_EXTERN_AUTH_OPERATOR_TOKEN_GUARD_NAME;
}

/* static */ void CommServerExternAuthOperatorTokenGuard::processUserAuth(CommMsgBody& ret, CommMsgParser& parser)
{
	processUserAuthImpl(ret, parser, _version, "CommServerExternAuthOperatorTokenGuard");
}
