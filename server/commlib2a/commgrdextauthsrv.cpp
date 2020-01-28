#include "commgrdextauth.h"

void CommServerExternAuthGuard::processFirstRequest( CommMsgParser& parser )
{
    parser.parseINT32(peerVersion);
    if ( peerVersion > _version  )
    {
        PTRACE3("%s::processFirstRequest peer version %d differs from current version %d", whoAmI(), peerVersion, _version);
    }
	encr.processFirstRequest(parser);

    CommMsgBody request;
	encr.composeResponse(request);
	_postGuardAddResponse(request);
}

void CommServerExternAuthGuard::processAddRequest( CommMsgParser& parser )
{
	PBlock userRet;
	const BYTE * encryptedUser;
	size_t encrLen;
	parser.parseStringP(user);
	parser._parseVarBlock(encryptedUser, encrLen);
	encr.decryptData(encryptedUser, encrLen, userRet);
	if (userRet.size() != user.length() || memcmp(userRet.ptr(), user.c_str(), user.length()))
	{
		_postGuardError(_COMM_ERROR_INCORRECT_PARAMETERS, "Authentication failed: incorrect parameters", 0);
		return;
	}
	PBlock pwd;
	const BYTE * encryptedPwd;
	size_t encrPwdLen;
	parser._parseVarBlock(encryptedPwd, encrPwdLen);
	encr.decryptData(encryptedPwd, encrPwdLen, pwd);
	pwdReceived.assignFromBlock(pwd.ptr(), pwd.size());

	if (!parser.parseEnded())//--- temporary; for compatibility
		parser.parseMsgBody(clientExtra);

	CommMsgBody request;
	request.composeINT32(_version);
	conn->getUserAuth(_name, user, clientExtra, request);
}

void CommServerExternAuthGuard::processOutBlock( CommMsgBody& dst, CommMsgBody& src )
{
    dst._composeVarBlock( src._readPtr(), src._size() );
}

void CommServerExternAuthGuard::processInBlock( CommMsgBody& dst, CommMsgParser& src )
{
    const BYTE* p;
    size_t sz;
    src._parseVarBlock( p, sz );
    memcpy( dst._alloc( sz ), p, sz );
}

bool CommServerExternAuthGuard::gotUserAuth( CommMsgParser* parser, UINT16 errCode, const char* errMsg, const CommMsgBody* errMsgBody )
{
	if (parser)
	{
		delegateUserAuth( errCode, errMsg, errMsgBody );
		return !errCode;
	}

	_postGuardError( errCode ? errCode : _COMM_ERROR_AUTHFAILED, errMsg ? errMsg : "Authentication failed: no such user or invalid password", errMsgBody );
	return false;
}


void CommServerExternAuthGuard::delegateUserAuth(UINT16 errCode, const char* errMsg, const CommMsgBody* errMsgBody)
{
	if (errCode)
	{
		_postGuardError(errCode, errMsg, errMsgBody);
	}
	else
	{
		CommMsgBody body;
		PBlock encryptedUser;
		encr.encryptData((const BYTE *)user.c_str(), user.length(), encryptedUser);
		body._composeVarBlock(encryptedUser.ptr(), encryptedUser.size());
		_postGuardRequestGranted(body, errMsgBody);
	}
}

bool CommServerExternAuthGuard::checkUserAuth( CommMsgParser* parser ) const
{
	return true;
}

void CommServerExternAuthGuard::encryptMsg(const CommMsgBody& src, PBlock& dst) const
{
	encr.encryptData(src, dst);
}
void CommServerExternAuthGuard::decryptMsg(const BYTE * block, size_t len, CommMsgBody& dst) const
{
	encr.decryptData(block, len, dst);
}

/* new */ CommServerGuard* CommServerExternAuthGuardFactory::createGuard( const vector< const char* >& guardNames )
{
    for( int i=0; i < guardNames.size() ; ++i )
    {
        if( strcmp( guardNames[ i ], _COMM_EXTERN_AUTH_GUARD_NAME ) == 0 )
            return new CommServerExternAuthGuard;
    }
    return nullptr;
}

/* new */ CommServerGuard* CommServerExternAuthOperatorTokenGuardFactory::createGuard(const vector< const char* >& guardNames)
{
	for (int i = 0; i < guardNames.size(); ++i)
	{
		if (strcmp(guardNames[i], _COMM_EXTERN_AUTH_OPERATOR_TOKEN_GUARD_NAME) == 0)
			return new CommServerExternAuthOperatorTokenGuard;
	}
	return nullptr;
}
