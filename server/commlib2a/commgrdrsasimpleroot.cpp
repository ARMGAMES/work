#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"

#include "commmsgbody.h"
#include "commqueue.h"
#include "commsrv.h"
#include "commgrdrsasimple.h"


/* static */ const char* CommServerRsaSimpleGuard::userAuthHandled()
{
	return _COMM_RSA_SIMPLE_GUARD_NAME;
}

void CommServerRsaSimpleGuard::processUserAuth( CommMsgBody& ret, CommMsgParser& parser,
										              const BYTE * randomToken, int randomTokenSize,
										              const BYTE * randomToken2, int randomTokenSize2)
{
	INT32 peerVersion;
	parser.parseINT32(peerVersion);
	if (peerVersion != _version)
	{
		PTRACE3(" CommServerRsaSimpleSimpleGuard::processUserAuth: requested version %d differs from current %d ", peerVersion, _version);
	}

	bool firstConnection;
	parser.parseBOOL(firstConnection);
	if (firstConnection)
	{
		PTRACE3(" CommServerRsaSimpleGuard::processUserAuth: requested for first connection");
		PASSERT3(0);
	}
	ret.composeINT32(_version);
	ret._composeVarBlock( randomToken,randomTokenSize);
	if (randomTokenSize2)
	{
		ret._composeVarBlock( randomToken2,randomTokenSize2);
	}
}


/*static*/  void CommServerRsaSimpleGuard::composeSecurityToken( CommMsgBody& ret, const BYTE * randomToken, int randomTokenSize)
{
	ret.composeINT32(_version).
		_composeVarBlock( randomToken,randomTokenSize);
}

void CommServerRsaSimpleGuard::processUserAuth( CommMsgBody& ret, CommMsgParser& parser, const char* user, const char* encrPassword )
{
	INT32 peerVersion;
	parser.parseINT32(peerVersion);
	if (peerVersion != _version)
	{
		PTRACE3(" CommServerAesEncryptedGuard::processUserInfo: requested version %d differs from current %d", peerVersion, _version);
	}
	bool firstConnection;
	parser.parseBOOL(firstConnection);
	if (!firstConnection)
	{
		PTRACE3(" CommServerRsaSimpleGuard::processUserInfo: requested for reconnect");
		PASSERT3(0);
	}

	ret.composeINT32(_version);
	ret.composeString( encrPassword );
}