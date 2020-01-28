#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"

#include "commmsgbody.h"
#include "commqueue.h"
#include "commsrv.h"
#include "commgrdaesencrypted.h"

CommServerAesEncryptedGuard::CommServerAesEncryptedGuard()
: CommServerGuard( _COMM_AES_GUARD_NAME )
{
	peerVersion = 0;
	encrypt = 0;
	decrypt = 0;
}


CommServerAesEncryptedGuard::~CommServerAesEncryptedGuard(  )
{
	if (encrypt)
		delete encrypt;
	if (decrypt)
		delete decrypt;
}

/* virtual */ void CommServerAesEncryptedGuard::processOutBlock( CommMsgBody& dst, CommMsgBody& src )
{
	PBlock ret;
	encrypt->addData(src._readPtr(),src._size());
	encrypt->encrypt(ret);
	dst._composeVarBlock( ret.ptr(), ret.size() );
}

/* virtual */ void CommServerAesEncryptedGuard::processInBlock( CommMsgBody& dst, CommMsgParser& src )
{
	const BYTE* p;
	size_t sz;
	src._parseVarBlock( p, sz );

	PBlock ret;
	decrypt->addData(p,sz);
	decrypt->decrypt(ret);
	memcpy( dst._alloc( ret.size() ), ret.ptr(), ret.size() );
}

/* virtual */ void CommServerAesEncryptedGuard::processAddRequest( CommMsgParser& parser )
{
	PASSERT3( 0 );
}


/* virtual */ void CommServerAesEncryptedGuard::processFirstRequest( CommMsgParser& parser )
{
	parser.parseINT32(peerVersion);

	if ( peerVersion != _version  )
	{
		PTRACE3("CommServerAesEncryptedGuard::processFirstRequest peer version %d differs from current version %d", peerVersion, _version);
	}

	parser.parseStringP(user);
	parser.parsePBlock(encrypted);
	CommMsgBody clientExtra;	

	if( !parser.parseEnded() )//--- temporary; for compatibility
		parser.parseMsgBody( clientExtra );

	CommMsgBody request;
	request.composeINT32(_version);
	conn->getUserAuth( _COMM_AES_GUARD_NAME, user, clientExtra, request );
}



/* virtual */ bool CommServerAesEncryptedGuard::gotUserAuth( CommMsgParser* parser, UINT16 errCode, const char* errMsg, const CommMsgBody* errMsgBody )
{
	if (parser && !errCode)
	{
		const BYTE * pwd;
		size_t pwdLen;
		INT32 ver;
		parser->parseINT32(ver);
		if ( ver != _version  )
		{
			PTRACE3("CommServerAesEncryptedGuard::gotUserAuth peer version %d differs from current version %d", ver, _version);
		}

		parser->_parseVarBlock(pwd,pwdLen);

		encrypt = new CommSSLEncryptAES(pwd,pwdLen,false);
		decrypt = new CommSSLDecryptAES(pwd,pwdLen,false);
		PBlock ret;

		decrypt->addData(encrypted.ptr(),encrypted.size());
		decrypt->decrypt(ret);
		if ( ret.size() == user.length() &&
			!memcmp(ret.ptr(),user.c_str(), user.length())
			)
		{
			CommMsgBody body;
			PBlock encrypted;
			encrypt->addData((const BYTE *)user.c_str(),user.length());
			encrypt->encrypt(encrypted);
			PASSERT3(encrypted.size());
			body._composeVarBlock(encrypted.ptr(),encrypted.size());
			_postGuardRequestGranted( body,errMsgBody );
			return true;
		}

		if (!parser->parseEnded())
		{
			delete encrypt;
			delete decrypt;
			parser->_parseVarBlock(pwd,pwdLen);
			encrypt = new CommSSLEncryptAES(pwd,pwdLen,false);
			decrypt = new CommSSLDecryptAES(pwd,pwdLen,false);
			PBlock ret;
			decrypt->addData(encrypted.ptr(),encrypted.size());
			decrypt->decrypt(ret);
			if ( ret.size() == user.length() &&
				!memcmp(ret.ptr(),user.c_str(), user.length() )
				)
			{
				CommMsgBody body;
				PBlock encrypted;
				encrypt->addData((const BYTE *)user.c_str(),user.length());
				encrypt->encrypt(encrypted);
				PASSERT3(encrypted.size());
				body._composeVarBlock(encrypted.ptr(),encrypted.size());
				_postGuardRequestGranted( body,errMsgBody );
				return true;
			}
		}
	}
	_postGuardError( errCode ? errCode : _COMM_ERROR_AUTHFAILED, errMsg ? errMsg : "Authentication failed: no such user or invalid password", errMsgBody );
	return false;
}


/* virtual */ bool CommServerAesEncryptedGuard::checkUserAuth( CommMsgParser* parser ) const
{
	return true;
}

//****************************************************************************

/* virtual */ /* new */ CommServerGuard* CommServerAesEncryptedGuardFactory::createGuard( const vector< const char* >& guardNames )
{
	int n = guardNames.size();
	for( int i=0; i < n ; ++i )
	{
		if( strcmp( guardNames[ i ], _COMM_AES_GUARD_NAME ) == 0 )
			return new CommServerAesEncryptedGuard();
	}
	return 0;
}
