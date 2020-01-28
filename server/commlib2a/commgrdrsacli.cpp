#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"

#include "commmsgbody.h"
#include "commqueue.h"
#include "commcli.h"
#include "commgrdrsa.h"

CommClientRsaGuard::CommClientRsaGuard( const char* user_, const char * pwd_,  const CommMsgBody& extra_)
: CommClientGuard( _COMM_RSA_GUARD_NAME ), 
user(user_),
pwd(pwd_)
{
	firstConnection = true;
	extra.copyFrom(extra_);
	encrypt = 0;
	decrypt = 0;
	callback = 0;
}

CommClientRsaGuard::CommClientRsaGuard( const char* user_, const PBlock & securityToken_Password_, const PBlock & securityToken_Random_, const CommMsgBody& extra_)
: CommClientGuard( _COMM_RSA_GUARD_NAME ), 
user(user_),
securityToken_Random(securityToken_Random_)
{
	firstConnection = false;
	extra.copyFrom(extra_);
	callback = 0;
	encrypt = new CommSSLEncryptAES(securityToken_Password_.ptr(),securityToken_Password_.size(),true);
	decrypt = new CommSSLDecryptAES(securityToken_Password_.ptr(),securityToken_Password_.size(),true);
}

CommClientRsaGuard::CommClientRsaGuard( CommClientAuthCallback & callback_)
: CommClientGuard( _COMM_RSA_GUARD_NAME ) 
{
	firstConnection = true;
	encrypt = 0;
	decrypt = 0;
	callback = &callback_;
}

CommClientRsaGuard::CommClientRsaGuard( CommClientAuthCallback & callback_, const PBlock & securityToken_Password_, const PBlock & securityToken_Random_)
: CommClientGuard( _COMM_RSA_GUARD_NAME ), 
securityToken_Random(securityToken_Random_)
{
	firstConnection = false;
	callback = &callback_;
	encrypt = new CommSSLEncryptAES(securityToken_Password_.ptr(),securityToken_Password_.size(),true);
	decrypt = new CommSSLDecryptAES(securityToken_Password_.ptr(),securityToken_Password_.size(),true);
}

CommClientRsaGuard::~CommClientRsaGuard()
{
	if (encrypt)
		delete encrypt;
	if (decrypt)
		delete decrypt;
}

/* virtual */ void CommClientRsaGuard::composeFirstRequest( CommMsgBody& msgBody )
{
	if (callback)
	{
		PString sid;
		callback->auth(user,pwd,sid,extra);
		sid.clear();
	}

	msgBody.composeINT32(_version).composeString( user ).composeBOOL(firstConnection);
	if (!firstConnection)
	{
		PBlock encrypted;
		encrypt->addData(securityToken_Random.ptr(),securityToken_Random.size());
		encrypt->encrypt(encrypted);
		msgBody._composeVarBlock(encrypted.ptr(),encrypted.size());
	}
	msgBody.composeMsgBody( extra );
	if (callback)
	{
		user.clear();
		pwd.clear();
		memset(extra._writePtr(),0,extra._size());
		extra.clear();
	}

}

/* virtual */ bool CommClientRsaGuard::processResponse( CommMsgBody& msgBody, CommMsgParser& parser )
{
	PASSERT3(firstConnection);

	if (callback)
	{
		PString sid;
		callback->auth(user,pwd,sid,extra);
		sid.clear();
	}


	CommSSLDH *	dh = new CommSSLDH();
	dh->parseParameters(parser);
	const BYTE * serverPublicKey;
	size_t len;
	parser._parseVarBlock(serverPublicKey,len);

	PBlock publicKey;
	dh->generateKeys(publicKey);

	PBlock secret;
	dh->generateSecret(serverPublicKey,len,secret);
	delete dh;
	dh=0;

#ifdef USE_RSA_VERSION_2
	PString encPwd;
	CommSSLPasswordHash::generateHash(encPwd,pwd.c_str());
	BYTE * p = secret.append(encPwd.length());
	memcpy (p,encPwd.c_str(),encPwd.length());

#else
	BYTE * p = secret.append(pwd.length());
	memcpy (p,pwd.c_str(),pwd.length());
#endif


	encrypt = new CommSSLEncryptAES(secret.ptr(),secret.size(),true);
	decrypt = new CommSSLDecryptAES(secret.ptr(),secret.size(),true);

	PBlock encryptedUser;

	encrypt->addData(( const BYTE *)user.c_str(),user.length());
	encrypt->encrypt(encryptedUser);
	msgBody._composeVarBlock(publicKey.ptr(),publicKey.size());
	msgBody._composeVarBlock( encryptedUser.ptr(), encryptedUser.size() );
	if (callback)
	{
		user.clear();
		pwd.clear();
		memset(extra._writePtr(),0,extra._size());
		extra.clear();
	}
	return true;
}

/* virtual */ bool CommClientRsaGuard::processGranted( CommMsgParser& parser )
{
	const BYTE* p;
	size_t sz;
	parser._parseVarBlock( p, sz );

	PBlock ret;
	decrypt->addData(p,sz);
	decrypt->decrypt(ret);

	if (callback)
	{
		PString sid;
		callback->auth(user,pwd,sid,extra);
		sid.clear();
	}

	bool ok = ret.size() == user.length() &&!memcmp(ret.ptr(),user.c_str(), ret.size() );
	if (callback)
	{
		user.clear();
		pwd.clear();
		memset(extra._writePtr(),0,extra._size());
		extra.clear();
	}
	return ok;
}

/* virtual */ void CommClientRsaGuard::processOutBlock( CommMsgBody& dst, CommMsgBody& src )
{
	PBlock ret;
	encrypt->addData(src._readPtr(),src._size());
	encrypt->encrypt(ret);
	dst._composeVarBlock( ret.ptr(), ret.size() );
}

/* virtual */ void CommClientRsaGuard::processInBlock( CommMsgBody& dst, CommMsgParser& src )
{
	const BYTE* p;
	size_t sz;
	src._parseVarBlock( p, sz );

	PBlock ret;
	decrypt->addData(p,sz);
	decrypt->decrypt(ret);
	memcpy( dst._alloc( ret.size() ), ret.ptr(), ret.size() );
}

//****************************************************************************


CommClientRsaGuardFactory::CommClientRsaGuardFactory()
{
	_hasAuth = false;
	callback = 0;
}


void CommClientRsaGuardFactory::setAuth( const char* user_, const char* pwd_,  const CommMsgBody* extra_ )
{
	PASSERT3(!callback);
	PTRACE3( "CommClientRsaGuardFactory::setAuth( '%s', '*' )", user_ );
	user = user_;
	pwd = pwd_;
	if( extra_ )
		extra.copyFrom( *extra_ );
	else
		extra.clear();
	_hasAuth = true;
}

void CommClientRsaGuardFactory::setAuth( CommClientAuthCallback & callback_ )
{
	user.clear();
	pwd.clear();
	memset(extra._writePtr(),0,extra._size());
	extra.clear();
	callback = &callback_;
	_hasAuth = true;
}

void CommClientRsaGuardFactory::setAuthSecurityToken( CommMsgParser& parser)
{
	securityToken_Password.clear();
	securityToken_Random.clear();
	INT32 ver;
	parser.parseINT32(ver);
	if (ver != CommClientRsaGuard::getVersion())
	{
		PTRACE3("CommClientRsaGuardFactory wrong peer version %d, current %d",ver, CommClientRsaGuard::getVersion());
	}
	parser.parsePBlock(securityToken_Password);
	PASSERT3(securityToken_Password.size());
	parser.parsePBlock(securityToken_Random);
	PASSERT3(securityToken_Random.size());
}

void CommClientRsaGuardFactory::resetAuth()
{
	PTRACE3( "CommClientRsaGuardFactory::resetAuth()" );
	user.clear();
	pwd.clear();
	memset(extra._writePtr(),0,extra._size());
	extra.clear();

	securityToken_Password.clear();
	securityToken_Random.clear();
	_hasAuth = false;
}

bool 	CommClientRsaGuardFactory::hasAuth() const
{
	return _hasAuth;
}

/* virtual */ void 	CommClientRsaGuardFactory::composeGuardNameList( CommMsgBody& msgBody ) const
{
	//NOTE: order is essential here
	msgBody.composeString( _COMM_RSA_GUARD_NAME );
}

/* virtual */ /* new */ CommClientGuardAuth* CommClientRsaGuardFactory::auth()
{
	_CommClientRsaGuardAuth* ret = new _CommClientRsaGuardAuth();
	if (callback)
	{
		ret->callback = callback;
	}
	else
	{
		ret->callback = 0;
		PASSERT3( *user );
		ret->user = user;
		ret->pwd = pwd;
		ret->extra.copyFrom( extra );
	}
	return ret;
}

/* virtual */ /* new */ CommClientGuard* CommClientRsaGuardFactory::createGuard( const char* guardName, CommClientGuardAuth* auth_ )
{
	_CommClientRsaGuardAuth* auth = (_CommClientRsaGuardAuth*)auth_;
	PASSERT3( auth );
	if (auth->callback)
	{
		if (securityToken_Password.size())
			return new CommClientRsaGuard( *auth->callback, securityToken_Password, securityToken_Random );
		else
			return new CommClientRsaGuard( *auth->callback);
	}
	else
	{
		PASSERT3( *auth->user );
		if (securityToken_Password.size())
			return new CommClientRsaGuard(  auth->user.c_str(), securityToken_Password, securityToken_Random, auth->extra );
		else
			return new CommClientRsaGuard( auth->user.c_str(), auth->pwd.c_str(), auth->extra);
	}
}
