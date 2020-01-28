#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"

#include "commmsgbody.h"
#include "commqueue.h"
#include "commsrv.h"
#include "commgrdrsa.h"


/* static */ const char* CommServerRsaGuard::userAuthHandled()
{
	return _COMM_RSA_GUARD_NAME;
}

void CommServerRsaGuard::processUserAuth( CommMsgBody& ret, CommMsgParser& parser,
										 const BYTE * password, int passwordSize,const BYTE * randomToken, int randomTokenSize,
										 const BYTE * password2, int passwordSize2,const BYTE * randomToken2, int randomTokenSize2)
{
	INT32 peerVersion;
	parser.parseINT32(peerVersion);
	if (peerVersion != _version)
	{
		PTRACE3(" CommServerRsaGuard::processUserAuth: requested version %d differs from current %d ", peerVersion, _version);
	}

	bool firstConnection;
	parser.parseBOOL(firstConnection);
	if (firstConnection)
	{
		PTRACE3(" CommServerRsaGuard::processUserAuth: requested for first connection");
	}
	ret.composeINT32(_version);
	ret._composeVarBlock( password,passwordSize);
	ret._composeVarBlock( randomToken,randomTokenSize);
	if (passwordSize2)
	{
		ret._composeVarBlock( password2,passwordSize2);
		ret._composeVarBlock( randomToken2,randomTokenSize2);
	}
}


/*static*/  void CommServerRsaGuard::composeSecurityToken( CommMsgBody& ret, const BYTE * password, int passwordSize,const BYTE * randomToken, int randomTokenSize)
{
	ret.composeINT32(_version).
		_composeVarBlock( password,passwordSize).
		_composeVarBlock( randomToken,randomTokenSize);
}

void CommServerRsaGuard::processUserInfo( CommMsgBody& ret, CommMsgParser& parser, const char* user, const char* encrPassword )
{
	INT32 peerVersion;
	parser.parseINT32(peerVersion);
	if (peerVersion != _version)
	{
		PTRACE3(" CommServerRsaGuard::processUserInfo: requested version %d differs from current %d", peerVersion, _version);
	}
	bool firstConnection;
	parser.parseBOOL(firstConnection);
	if (!firstConnection)
	{
		PTRACE3(" CommServerRsaGuard::processUserInfo: requested for reconnect");
	}

	ret.composeINT32(_version);
	ret.composeString( encrPassword );
}