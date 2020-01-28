#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"

#include "commmsgbody.h"
#include "commqueue.h"
#include "commcli.h"
#include "commgrdaesencrypted.h"



class _CommClientAesEncryptedGuardAuth : public CommClientGuardAuth
{
public:
	PString user;
	CommMsgBody extra;
	CommClientAuthCallback * callback;
public:
	_CommClientAesEncryptedGuardAuth()
	{
		callback = 0;
	}
	/* virtual */ ~_CommClientAesEncryptedGuardAuth() {}
};


CommClientAesEncryptedGuard::CommClientAesEncryptedGuard( const char * user_, PBlock &securityToken_Password,const CommMsgBody& extra_)
: CommClientGuard( _COMM_AES_GUARD_NAME ), 
user(user_)
{
	callback = 0;
	extra.copyFrom(extra_);
	encrypt = new CommSSLEncryptAES(securityToken_Password.ptr(),securityToken_Password.size(),false);
	decrypt = new CommSSLDecryptAES(securityToken_Password.ptr(),securityToken_Password.size(),false);
}

CommClientAesEncryptedGuard::CommClientAesEncryptedGuard(CommClientAuthCallback & callback_,PBlock &securityToken_Password)
: CommClientGuard( _COMM_AES_GUARD_NAME ) 
{
	callback = &callback_;
	encrypt = new CommSSLEncryptAES(securityToken_Password.ptr(),securityToken_Password.size(),false);
	decrypt = new CommSSLDecryptAES(securityToken_Password.ptr(),securityToken_Password.size(),false);
}

CommClientAesEncryptedGuard::~CommClientAesEncryptedGuard()
{
	if (encrypt)
		delete encrypt;
	if (decrypt)
		delete decrypt;
}

/* virtual */ void CommClientAesEncryptedGuard::composeFirstRequest( CommMsgBody& msgBody )
{
	if (callback)
	{
		PString sid;
		PString pwd;
		callback->auth(user,pwd,sid,extra);
		sid.clear();
		pwd.clear();
	}

	msgBody.composeUINT32(_version).composeString( user.c_str() );
	PBlock encrypted;
	encrypt->addData((const BYTE *)user.c_str(),user.length());
	encrypt->encrypt(encrypted);
	PASSERT3(encrypted.size());
	msgBody._composeVarBlock(encrypted.ptr(),encrypted.size());
	msgBody.composeMsgBody( extra );
	if (callback)
	{
		user.clear();
		memset(extra._writePtr(),0,extra._size());
		extra.clear();
	}

}

/* virtual */ bool CommClientAesEncryptedGuard::processResponse( CommMsgBody& msgBody, CommMsgParser& parser )
{	
	PASSERT3(0);
	return false;
}

/* virtual */ bool CommClientAesEncryptedGuard::processGranted( CommMsgParser& parser )
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
		PString pwd;
		callback->auth(user,pwd,sid,extra);
		sid.clear();
		pwd.clear();
	}

	bool ok = ret.size() == user.length() && !memcmp(ret.ptr(),user.c_str(), ret.size());
	if (callback)
	{
		user.clear();
		memset(extra._writePtr(),0,extra._size());
		extra.clear();
	}
	return ok;

}

/* virtual */ void CommClientAesEncryptedGuard::processOutBlock( CommMsgBody& dst, CommMsgBody& src )
{
	PBlock ret;
	encrypt->addData(src._readPtr(),src._size());
	encrypt->encrypt(ret);
	dst._composeVarBlock( ret.ptr(), ret.size() );
}

/* virtual */ void CommClientAesEncryptedGuard::processInBlock( CommMsgBody& dst, CommMsgParser& src )
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


CommClientAesEncryptedGuardFactory::CommClientAesEncryptedGuardFactory()
{
	callback=0;
}

void CommClientAesEncryptedGuardFactory::setAuth( const char* user_, const CommMsgBody* extra_ )
{
	PASSERT3(!callback);
	PTRACE3( "_CommClientAesEncryptedGuardAuth::setAuth( '%s', '*' )", user_ );
	user = user_;
	if( extra_ )
		extra.copyFrom( *extra_ );
	else
		extra.clear();
}

void CommClientAesEncryptedGuardFactory::setAuth( CommClientAuthCallback & callback_ )
{
	user.clear();
	memset(extra._writePtr(),0,extra._size());
	extra.clear();
	callback = &callback_;
}

void CommClientAesEncryptedGuardFactory::setAuthSecurityToken( CommMsgParser& parser)
{
	securityToken_Password.clear();
	INT32 ver;
	parser.parseINT32(ver);
	if (ver != CommClientAesEncryptedGuard::getVersion())
	{
		PTRACE3("CommClientAesEncryptedGuardFactory wrong peer version %d, current %d",ver, CommClientAesEncryptedGuard::getVersion());
	}
	parser.parsePBlock(securityToken_Password);
	PASSERT3(securityToken_Password.size());
}

void CommClientAesEncryptedGuardFactory::resetAuth()
{
	PTRACE3( "CommClientAesEncryptedGuardFactory::resetAuth()" );
	user.clear();
	memset(extra._writePtr(),0,extra._size());
	extra.clear();
	callback = 0;
	securityToken_Password.clear();
}

bool 	CommClientAesEncryptedGuardFactory::hasAuth() const
{
	return (user.length() > 0 || callback) && securityToken_Password.size() > 0;
}

/* virtual */ void 	CommClientAesEncryptedGuardFactory::composeGuardNameList( CommMsgBody& msgBody ) const
{
	//NOTE: order is essential here
	msgBody.composeString( _COMM_AES_GUARD_NAME );
}

/* virtual */ /* new */ CommClientGuardAuth* CommClientAesEncryptedGuardFactory::auth()
{
	_CommClientAesEncryptedGuardAuth* ret = new _CommClientAesEncryptedGuardAuth();
	if (callback)
	{
		ret->callback = callback;
	}
	else
	{
		ret->callback = 0;
		PASSERT3( *user );
		ret->user = user;
		ret->extra.copyFrom( extra );
	}
	return ret;
}

/* virtual */ /* new */ CommClientGuard* CommClientAesEncryptedGuardFactory::createGuard( const char* guardName, CommClientGuardAuth* auth_ )
{
	_CommClientAesEncryptedGuardAuth* auth = (_CommClientAesEncryptedGuardAuth*)auth_;
	PASSERT3( auth );
	if (callback)
	{
		return new CommClientAesEncryptedGuard( *callback, securityToken_Password);
	}
	else
	{
		PASSERT3( *auth->user );
		return new CommClientAesEncryptedGuard( auth->user.c_str(), securityToken_Password, auth->extra );
	}
}
