#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"

#include "commmsgbody.h"
#include "commqueue.h"
#include "commcli.h"
#include "commgrdrsasimple.h"

CommClientRsaSimpleGuard::CommClientRsaSimpleGuard( const char* user_, const char * pwd_,  const CommMsgBody& extra_)
: CommClientGuard( _COMM_RSA_SIMPLE_GUARD_NAME ), 
user(user_),
pwd(pwd_)
{
	firstConnection = true;
	extra.copyFrom(extra_);
	callback = 0;
}

CommClientRsaSimpleGuard::CommClientRsaSimpleGuard( const char* user_,  const PBlock & securityToken_Random_, const CommMsgBody& extra_)
: CommClientGuard( _COMM_RSA_SIMPLE_GUARD_NAME ), 
user(user_),
securityToken_Random(securityToken_Random_)
{
	firstConnection = false;
	extra.copyFrom(extra_);
	callback = 0;
}

CommClientRsaSimpleGuard::CommClientRsaSimpleGuard( CommClientAuthCallback & callback_)
: CommClientGuard( _COMM_RSA_SIMPLE_GUARD_NAME ) 
{
	firstConnection = true;
	callback = &callback_;
}

CommClientRsaSimpleGuard::CommClientRsaSimpleGuard( CommClientAuthCallback & callback_, const PBlock & securityToken_Random_)
: CommClientGuard( _COMM_RSA_SIMPLE_GUARD_NAME ), 
securityToken_Random(securityToken_Random_)
{
	firstConnection = false;
	callback = &callback_;
}

CommClientRsaSimpleGuard::~CommClientRsaSimpleGuard()
{
}

/* virtual */ void CommClientRsaSimpleGuard::composeFirstRequest( CommMsgBody& msgBody )
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
		msgBody._composeVarBlock(securityToken_Random.ptr(),securityToken_Random.size());
	}
	else
	{
		msgBody.composeString( pwd );

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

/* virtual */ bool CommClientRsaSimpleGuard::processResponse( CommMsgBody& msgBody, CommMsgParser& parser )
{
	PASSERT3(0);
	return false;
}

/* virtual */ bool CommClientRsaSimpleGuard::processGranted( CommMsgParser& parser )
{
	return true;
}

/* virtual */ void CommClientRsaSimpleGuard::processOutBlock( CommMsgBody& dst, CommMsgBody& src )
{
	dst._composeVarBlock( src._readPtr(), src._size() );
}

/* virtual */ void CommClientRsaSimpleGuard::processInBlock( CommMsgBody& dst, CommMsgParser& src )
{
	const BYTE* p;
	size_t sz;
	src._parseVarBlock( p, sz );
	memcpy( dst._alloc( sz ), p, sz );

}

//****************************************************************************


CommClientRsaSimpleGuardFactory::CommClientRsaSimpleGuardFactory()
{
	_hasAuth = false;
	callback = 0;
}


void CommClientRsaSimpleGuardFactory::setAuth( const char* user_, const char* pwd_,  const CommMsgBody* extra_ )
{
	PASSERT3(!callback);
	PTRACE3( "CommClientRsaSimpleGuardFactory::setAuth( '%s', '*' )", user_ );
	user = user_;
	pwd = pwd_;
	if( extra_ )
		extra.copyFrom( *extra_ );
	else
		extra.clear();
	_hasAuth = true;
}

void CommClientRsaSimpleGuardFactory::setAuth( CommClientAuthCallback & callback_ )
{
	user.clear();
	pwd.clear();
	memset(extra._writePtr(),0,extra._size());
	extra.clear();
	callback = &callback_;
	_hasAuth = true;
}

void CommClientRsaSimpleGuardFactory::setAuthSecurityToken( CommMsgParser& parser)
{
	securityToken_Random.clear();
	INT32 ver;
	parser.parseINT32(ver);
	if (ver != CommClientRsaSimpleGuard::getVersion())
	{
		PTRACE3("CommClientRsaSimpleGuardFactory wrong peer version %d, current %d",ver, CommClientRsaSimpleGuard::getVersion());
	}
	parser.parsePBlock(securityToken_Random);
	PASSERT3(securityToken_Random.size());
}

void CommClientRsaSimpleGuardFactory::resetAuth()
{
	PTRACE3( "CommClientRsaSimpleGuardFactory::resetAuth()" );
	user.clear();
	pwd.clear();
	memset(extra._writePtr(),0,extra._size());
	extra.clear();

	securityToken_Random.clear();
	_hasAuth = false;
}

bool 	CommClientRsaSimpleGuardFactory::hasAuth() const
{
	return _hasAuth;
}

/* virtual */ void 	CommClientRsaSimpleGuardFactory::composeGuardNameList( CommMsgBody& msgBody ) const
{
	//NOTE: order is essential here
	msgBody.composeString( _COMM_RSA_SIMPLE_GUARD_NAME );
}

/* virtual */ /* new */ CommClientGuardAuth* CommClientRsaSimpleGuardFactory::auth()
{
	_CommClientRsaSimpleGuardAuth* ret = new _CommClientRsaSimpleGuardAuth();
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

/* virtual */ /* new */ CommClientGuard* CommClientRsaSimpleGuardFactory::createGuard( const char* guardName, CommClientGuardAuth* auth_ )
{
	_CommClientRsaSimpleGuardAuth* auth = (_CommClientRsaSimpleGuardAuth*)auth_;
	PASSERT3( auth );
	if (auth->callback)
	{
		if (securityToken_Random.size())
			return new CommClientRsaSimpleGuard( *auth->callback, securityToken_Random );
		else
			return new CommClientRsaSimpleGuard( *auth->callback);
	}
	else
	{
		PASSERT3( *auth->user );
		if (securityToken_Random.size())
			return new CommClientRsaSimpleGuard(  auth->user.c_str(), securityToken_Random, auth->extra );
		else
			return new CommClientRsaSimpleGuard( auth->user.c_str(), auth->pwd.c_str(), auth->extra);
	}
}
