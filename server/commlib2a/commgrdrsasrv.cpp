#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"

#include "commmsgbody.h"
#include "commqueue.h"
#include "commsrv.h"
#include "commgrdrsa.h"

CommServerRsaGuard::CommServerRsaGuard( const CommMsgBody & dhParameters_ )
: CommServerGuard( _COMM_RSA_GUARD_NAME )
{
	firstConnection = true;
	peerVersion = 0;
	dhParameters.copyFrom(dhParameters_);
	dh = 0;
	encrypt = 0;
	decrypt = 0;
	connectionFailed=false;
}

CommServerRsaGuard::~CommServerRsaGuard(  )
{
	if (dh)
		delete dh;
	if (encrypt)
		delete encrypt;
	if (decrypt)
		delete decrypt;
}

/* virtual */ void CommServerRsaGuard::processOutBlock( CommMsgBody& dst, CommMsgBody& src )
{
	PBlock ret;
	encrypt->addData(src._readPtr(),src._size());
	encrypt->encrypt(ret);
	dst._composeVarBlock( ret.ptr(), ret.size() );
}

/* virtual */ void CommServerRsaGuard::processInBlock( CommMsgBody& dst, CommMsgParser& src )
{
	const BYTE* p;
	size_t sz;
	src._parseVarBlock( p, sz );

	PBlock ret;
	decrypt->addData(p,sz);
	decrypt->decrypt(ret);
	memcpy( dst._alloc( ret.size() ), ret.ptr(), ret.size() );
}


/* virtual */ void CommServerRsaGuard::processFirstRequest( CommMsgParser& parser )
{
	parser.parseINT32(peerVersion)
		.parseStringP(user)
		.parseBOOL(firstConnection);
	if ( peerVersion > _version )
	{
		PTRACE3("CommServerRsaGuard::processFirstRequest peer version %d greater than current %d ", peerVersion, _version);
	}

	if (!firstConnection)
	{
		// encrypted block
		parser.parsePBlock(encryptedToken);
	}

	if( !parser.parseEnded() )//--- temporary; for compatibility
		parser.parseMsgBody( clientExtra );

	CommMsgBody request;
	request.composeINT32(_version)
		.composeBOOL(firstConnection);

	if (firstConnection)
	{
		conn->getGuardUserInfo( _COMM_RSA_GUARD_NAME, user, clientExtra, request );
	}
	else
	{
		conn->getUserAuth( _COMM_RSA_GUARD_NAME, user, clientExtra, request );
	}
}

/* virtual */ void CommServerRsaGuard::processAddRequest( CommMsgParser& parser )
{
	PASSERT3(firstConnection);
	const BYTE * clientPublicKey;
	size_t pbLen;
	PBlock secret;
	parser._parseVarBlock(clientPublicKey,pbLen);
	dh->generateSecret(clientPublicKey,pbLen,secret);

	delete dh;
	dh=0;

	BYTE * p = secret.append(_hashPassword.length());
	memcpy (p,_hashPassword.c_str(),_hashPassword.length());

	encrypt = new CommSSLEncryptAES(secret.ptr(),secret.size(),true);
	decrypt = new CommSSLDecryptAES(secret.ptr(),secret.size(),true);


	PBlock ret;
	const BYTE * encryptedUser;
	size_t encrLen;
	parser._parseVarBlock(encryptedUser,encrLen);

	decrypt->addData(encryptedUser,encrLen);
	decrypt->decrypt(ret);
	if (ret.size() != user.length() || memcmp(ret.ptr(),user.c_str(),user.length()))
	{
		connectionFailed=true;
		//		_postGuardError( _COMM_ERROR_AUTHFAILED, "Authentication failed: no such user or invalid password" );
	}
	CommMsgBody request;
	request.composeINT32(_version)
		.composeBOOL(firstConnection);

	conn->getUserAuth( _COMM_RSA_GUARD_NAME, user, clientExtra, request );
}


/* virtual */ bool CommServerRsaGuard::gotGuardUserInfo( CommMsgParser* parser, UINT16 errCode, const char* errMsg, const CommMsgBody* errMsgBody )
{

	if (!parser || errCode)
	{
		_postGuardError( errCode ? errCode : _COMM_ERROR_AUTHFAILED, errMsg ? errMsg : "Authentication failed: no such user or invalid password", errMsgBody );
		return false;
	}

	PASSERT3(firstConnection);

	INT32 ver;
	parser->parseINT32(ver);
	if ( ver != _version  )
	{
		PTRACE3("CommServerRsaGuard::gotGuardUserInfo peer version %d differs from current version %d", ver, _version);
	}

	parser->parseStringP( _hashPassword );

	dh = new CommSSLDH();
	CommMsgParser parseParameters(dhParameters);
	dh->parseParameters(parseParameters);

	PBlock publicKey;
	dh->generateKeys(publicKey);
	CommMsgBody request;

	dh->composeParameters(request);
	request._composeVarBlock(publicKey.ptr(),publicKey.size());
	_postGuardAddResponse( request );
	return true;
}

/* virtual */ bool CommServerRsaGuard::gotUserAuth( CommMsgParser* parser, UINT16 errCode, const char* errMsg, const CommMsgBody* errMsgBody )
{
	if (!parser || errCode)
	{
		_postGuardError( errCode ? errCode : _COMM_ERROR_AUTHFAILED, errMsg ? errMsg : "Authentication failed: no such user or invalid password", errMsgBody );
		return false;
	}
	if (connectionFailed)
	{
		_postGuardError( _COMM_ERROR_AUTHFAILED, "Authentication failed: no such user or invalid password", errMsgBody );
		return false;
	}

	if (!firstConnection)
	{
		const BYTE * pwd;
		size_t pwdLen;
		const BYTE * block;
		size_t len;

		INT32 ver;
		parser->parseINT32(ver);
		if ( ver != _version )
		{
			PTRACE3("CommServerRsaGuard::gotUserAuth peer version %d differs from current version %d", ver, _version);
		}

		parser->_parseVarBlock(pwd,pwdLen);
		parser->_parseVarBlock(block,len);

		encrypt = new CommSSLEncryptAES(pwd,pwdLen,true);
		decrypt = new CommSSLDecryptAES(pwd,pwdLen,true);

		PBlock ret;
		decrypt->addData(encryptedToken.ptr(),encryptedToken.size());
		decrypt->decrypt(ret);
		if (ret.size() != len || memcmp(ret.ptr(),block,len))
		{
			delete encrypt;
			delete decrypt;
			parser->_parseVarBlock(pwd,pwdLen);
			parser->_parseVarBlock(block,len);

			encrypt = new CommSSLEncryptAES(pwd,pwdLen,true);
			decrypt = new CommSSLDecryptAES(pwd,pwdLen,true);
			PBlock ret;
			decrypt->addData(encryptedToken.ptr(),encryptedToken.size());
			decrypt->decrypt(ret);
			if (ret.size() != len || memcmp(ret.ptr(),block,len))
			{
				_postGuardError( _COMM_ERROR_AUTHFAILED, "Session key does not match", errMsgBody );
				return false;
			}
		}
	}
	CommMsgBody body;
	PBlock encryptedUser;

	encrypt->addData(( const BYTE *)user.c_str(),user.length());
	encrypt->encrypt(encryptedUser);
	body._composeVarBlock( encryptedUser.ptr(), encryptedUser.size() );
	_postGuardRequestGranted( body,errMsgBody );
	return true;
}


/* virtual */ bool CommServerRsaGuard::checkUserAuth( CommMsgParser* parser ) const
{
	return true;
}

//****************************************************************************

/* virtual */ /* new */ CommServerGuard* CommServerRsaGuardFactory::createGuard( const vector< const char* >& guardNames )
{
	int n = guardNames.size();
	for( int i=0; i < n ; ++i )
	{
		if( strcmp( guardNames[ i ], _COMM_RSA_GUARD_NAME ) == 0 )
			return new CommServerRsaGuard(dhParameters);
	}
	return 0;
}
