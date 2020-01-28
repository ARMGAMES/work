#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"

#include "commmsgbody.h"
#include "commqueue.h"
#include "commsrv.h"
#include "commgrdencryptedpwd.h"

CommServerEncryptedPwdGuard::CommServerEncryptedPwdGuard( bool useSid_ )
: CommServerGuard( _COMM_ENCRYPTEDPASSWORDSID_GUARD_NAME )
{
	useSid = useSid_;
	peerVersion = 0;
}

CommServerEncryptedPwdGuard::~CommServerEncryptedPwdGuard(  )
{
}

/* virtual */ void CommServerEncryptedPwdGuard::processOutBlock( CommMsgBody& dst, CommMsgBody& src )
{
	dst._composeVarBlock( src._readPtr(), src._size() );
}

/* virtual */ void CommServerEncryptedPwdGuard::processInBlock( CommMsgBody& dst, CommMsgParser& src )
{
	const BYTE* p;
	size_t sz;
	src._parseVarBlock( p, sz );
	memcpy( dst._alloc( sz ), p, sz );
}


/* virtual */ void CommServerEncryptedPwdGuard::processFirstRequest( CommMsgParser& parser )
{
	parser.parseINT32(peerVersion);
	if ( peerVersion > _version )
	{
		PTRACE3("CommServerEncryptedPwdGuard::processFirstRequest peer version %d greater than current %d ", peerVersion, _version);
	}
	encr.processFirstRequest(parser);
	CommMsgBody request;

	encr.composeResponse(request);
	_postGuardAddResponse( request );
	return;
}

/* virtual */ void CommServerEncryptedPwdGuard::processAddRequest( CommMsgParser& parser )
{
	PBlock userRet;
	const BYTE * encryptedUser;
	size_t encrLen;
	parser.parseStringP(user);
	parser._parseVarBlock(encryptedUser,encrLen);
	encr.decryptData(encryptedUser,encrLen,userRet);
	if (userRet.size() != user.length() || memcmp(userRet.ptr(),user.c_str(),user.length()))
	{
		_postGuardError( _COMM_ERROR_INCORRECT_PARAMETERS,  "Authentication failed: incorrect parameters", 0);
		return;
	}
	PBlock pwd;
	const BYTE * encryptedPwd;
	size_t encrPwdLen;
	parser._parseVarBlock(encryptedPwd,encrPwdLen);
	encr.decryptData(encryptedPwd,encrPwdLen,pwd);
	pwdReceived.assignFromBlock(pwd.ptr(),pwd.size());

	if (useSid)
	{
		PBlock sid;
		const BYTE * encryptedSid;
		size_t encrSidLen;
		parser._parseVarBlock(encryptedSid,encrSidLen);
		encr.decryptData(encryptedSid,encrSidLen,sid);
		sidReceived.assignFromBlock(sid.ptr(),sid.size());
	}
	else
	{
		sidReceived="";
	}
	if( !parser.parseEnded() )//--- temporary; for compatibility
		parser.parseMsgBody( clientExtra );


	CommMsgBody request;
	request.composeINT32(_version);
	conn->getUserAuth( _COMM_ENCRYPTEDPASSWORDSID_GUARD_NAME, user, clientExtra, request );
}



/* virtual */ bool CommServerEncryptedPwdGuard::gotUserAuth( CommMsgParser* parser, UINT16 errCode, const char* errMsg, const CommMsgBody* errMsgBody )
{
	if (!parser || errCode)
	{
		_postGuardError( errCode ? errCode : _COMM_ERROR_AUTHFAILED, errMsg ? errMsg : "Authentication failed: no such user or invalid password", errMsgBody );
		return false;
	}

	UINT16 internalErrCode = _COMM_ERROR_AUTHFAILED;
	const char * msg = "Authentication failed: no such user or invalid password"; 
	if( parser )
	{
		INT32 ver;
		parser->parseINT32(ver);
		if ( ver != _version )
		{
			PTRACE3("CommServerEncryptedPwdGuard::gotUserAuth peer version %d differs from current version %d", ver, _version);
		}

		const char* encrPassword;
		const char* sid;
		bool ok = false;
		parser->parseString( encrPassword );
		parser->parseString( sid );
		ok = CommSSLPasswordHash::checkHash(encrPassword,pwdReceived);
		if( ok  )
		{
			if( sidReceived.equals( sid ) )
			{
				CommMsgBody body;
				PBlock encryptedUser;
				encr.encryptData(( const BYTE *)user.c_str(),user.length(),encryptedUser);
				body._composeVarBlock( encryptedUser.ptr(), encryptedUser.size() );
				_postGuardRequestGranted( body,errMsgBody );
				return true;
			}
			else
			{
				if( *sidReceived == 0 )
				{
					if (useSid)
					{
						if ( !parser->parseEnded() )
						{
							parser->parseUINT16( internalErrCode );
						}
						else
						{
							internalErrCode = _COMM_ERROR_AUTHFAILED_PARTIAL;
						}
					}
				}
				else if( *sid == 0 )
				{
					if( useSid )
					{
						internalErrCode = _COMM_ERROR_AUTHFAILED_EXTRA;
						msg = "SID supplied but not required";
					}
				}
				else
				{
					if( useSid )
						internalErrCode = _COMM_ERROR_AUTHFAILED_SEVERE;
				}
			}
		}
		else
		{
			internalErrCode = _COMM_ERROR_AUTHFAILED;
		}
	}
	_postGuardError( (errCode ? errCode : internalErrCode), (errMsg ? errMsg : msg), errMsgBody );
	return false;
}

/* virtual */ void CommServerEncryptedPwdGuard::delegateUserAuth(  UINT16 errCode, const char* errMsg, const CommMsgBody* errMsgBody )
{
	if (errCode)
	{
		_postGuardError(  errCode , errMsg , errMsgBody );
	}
	else
	{
		CommMsgBody body;
		_postGuardRequestGranted( body,errMsgBody );
	}
}

/* virtual */ bool CommServerEncryptedPwdGuard::checkUserAuth( CommMsgParser* parser ) const
{
	if( parser )
	{
		INT32 ver;
		parser->parseINT32(ver);
		if ( ver != _version )
		{
			PTRACE3("CommServerEncryptedPwdGuard::gotUserAuth peer version %d differs from current version %d", ver, _version);
		}
		const char* encrPassword;
		const char* sid;
		bool ok = false;
		parser->parseString( encrPassword );
		parser->parseString( sid );
		ok = CommSSLPasswordHash::checkHash(encrPassword,pwdReceived);
		if( ok )
		{
			if( sidReceived.equals( sid ) )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	return false;

}

void CommServerEncryptedPwdGuard::encryptMsg(const CommMsgBody& src, PBlock& dst) const
{
	encr.encryptData(src,dst);
}
void CommServerEncryptedPwdGuard::decryptMsg(const BYTE * block, size_t len, CommMsgBody& dst) const
{
	encr.decryptData(block,len,dst);
}

//****************************************************************************

/* virtual */ /* new */ CommServerGuard* CommServerEncryptedPwdGuardFactory::createGuard( const vector< const char* >& guardNames )
{
	int n = guardNames.size();
	for( int i=0; i < n ; ++i )
	{
		if( strcmp( guardNames[ i ], _COMM_ENCRYPTEDPASSWORDSID_GUARD_NAME ) == 0 )
			return new CommServerEncryptedPwdGuard(true);
		else if( strcmp( guardNames[ i ], _COMM_ENCRYPTEDPASSWORD_GUARD_NAME ) == 0 )
			return new CommServerEncryptedPwdGuard(false);
	}
	return 0;
}
