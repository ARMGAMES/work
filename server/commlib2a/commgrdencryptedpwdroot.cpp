#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"

#include "commmsgbody.h"
#include "commqueue.h"
#include "commsrv.h"
#include "commgrdencryptedpwd.h"

/* static */ const char* CommServerEncryptedPwdGuard::userAuthHandled()
{
	return _COMM_ENCRYPTEDPASSWORDSID_GUARD_NAME;
}

/* static */ void CommServerEncryptedPwdGuard::processUserAuth( CommMsgBody& ret, CommMsgParser& parser, const char* user, const char* encrPassword, const char* sid, INT16 sidErrorCode )
{
	INT32 peerVersion;
	parser.parseINT32(peerVersion);
	if (peerVersion != _version)
	{
		PTRACE3(" CommServerEncryptedPwdGuard::processUserInfo: requested version %d differs from current %d", peerVersion, _version);
	}
	ret.composeINT32(_version).composeString(encrPassword).composeString(sid);
	if (sidErrorCode)
		ret.composeINT16(sidErrorCode);
}
