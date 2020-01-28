#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"

#include "commmsgbody.h"
#include "commqueue.h"
#include "commsrv.h"
#include "commgrdaesencrypted.h"

/* static */ const char* CommServerAesEncryptedGuard::userAuthHandled()
{
	return _COMM_AES_GUARD_NAME;
}

/* static */ void CommServerAesEncryptedGuard::processUserAuth( CommMsgBody& ret, CommMsgParser& parser, const BYTE * encryptionKey, int sizeEncryptionKey, const BYTE * encryptionKey2, int sizeEncryptionKey2 )
{
	INT32 peerVersion;
	parser.parseINT32(peerVersion);
	if (peerVersion != _version)
	{
		PTRACE3(" CommServerAesEncryptedGuard::processUserAuth: requested version %d differs from current %d", peerVersion, _version);

	}
	ret.composeINT32(_version);
	ret._composeVarBlock( encryptionKey,sizeEncryptionKey);
	if (sizeEncryptionKey2)
		ret._composeVarBlock( encryptionKey2,sizeEncryptionKey2);
}


/*static*/  void CommServerAesEncryptedGuard::composeSecurityToken( CommMsgBody& ret, const BYTE * encryptionKey, int sizeEncryptionKey)
{
	ret.composeINT32(_version)._composeVarBlock( encryptionKey,sizeEncryptionKey);
}