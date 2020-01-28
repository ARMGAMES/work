#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"

#include "commmsgbody.h"
#include "commqueue.h"
#include "commcli.h"
#include "commgrdppwdsid.h"
#include "commgrdencryptedpwd.h"


CommClientEncryptedPwdGuard::CommClientEncryptedPwdGuard( const char* user_, const char * pwd_,  const char* sid_, const CommMsgBody& extra_,  bool useSid_)
: CommClientGuard( _COMM_ENCRYPTEDPASSWORDSID_GUARD_NAME ), 
user(user_),
pwd(pwd_),
sid( sid_ )
{
	extra.copyFrom(extra_);
	useSid = useSid_;
	callback = 0;
}


CommClientEncryptedPwdGuard::CommClientEncryptedPwdGuard( CommClientAuthCallback & callback_, bool useSid_)
: CommClientGuard( _COMM_ENCRYPTEDPASSWORDSID_GUARD_NAME ) 
{
	useSid = useSid_;
	callback = &callback_;
}


CommClientEncryptedPwdGuard::~CommClientEncryptedPwdGuard()
{
}

/* virtual */ void CommClientEncryptedPwdGuard::composeFirstRequest( CommMsgBody& msgBody )
{
	msgBody.composeINT32(_version);
	encr.composeFirstRequest(msgBody);

}

/* virtual */ bool CommClientEncryptedPwdGuard::processResponse( CommMsgBody& msgBody, CommMsgParser& parser )
{

	if (callback)
	{
		callback->auth(user,pwd,sid,extra);
	}
	encr.processResponse(parser);
	msgBody.composeString(user);
	PBlock encryptedUser;
	encr.encryptData(( const BYTE *)user.c_str(),user.length(),encryptedUser);
	msgBody._composeVarBlock( encryptedUser.ptr(), encryptedUser.size() );
	PBlock encryptedPwd;
	encr.encryptData(( const BYTE *)pwd.c_str(),pwd.length(),encryptedPwd);
	msgBody._composeVarBlock( encryptedPwd.ptr(), encryptedPwd.size() );
	if (useSid)
	{
		PBlock encryptedSid;
		encr.encryptData(( const BYTE *)sid.c_str(),sid.length(),encryptedSid);
		msgBody._composeVarBlock( encryptedSid.ptr(), encryptedSid.size() );
	}
	msgBody.composeMsgBody( extra );
	if (callback)
	{
		user.clear();
		pwd.clear();
		sid.clear();
		memset(extra._writePtr(),0,extra._size());
		extra.clear();
	}
	return true;
}

/* virtual */ bool CommClientEncryptedPwdGuard::processGranted( CommMsgParser& parser )
{
	const BYTE* p;
	size_t sz;
	parser._parseVarBlock( p, sz );
	PBlock ret;
	encr.decryptData(p,sz,ret);

	if (callback)
	{
		callback->auth(user,pwd,sid,extra);
	}

	bool ok = ret.size() == user.length() &&!memcmp(ret.ptr(),user.c_str(), ret.size() );
	if (callback)
	{
		user.clear();
		pwd.clear();
		sid.clear();
		memset(extra._writePtr(),0,extra._size());
		extra.clear();
	}
	return ok;
}

/* virtual */ void CommClientEncryptedPwdGuard::processOutBlock( CommMsgBody& dst, CommMsgBody& src )
{
	dst._composeVarBlock( src._readPtr(), src._size() );
}

/* virtual */ void CommClientEncryptedPwdGuard::processInBlock( CommMsgBody& dst, CommMsgParser& src )
{
	const BYTE* p;
	size_t sz;
	src._parseVarBlock( p, sz );
	memcpy( dst._alloc( sz ), p, sz );

}
void CommClientEncryptedPwdGuard::encryptMsg(const CommMsgBody& src, PBlock& dst) const
{
	encr.encryptData(src,dst);
}
void CommClientEncryptedPwdGuard::decryptMsg(const BYTE * block, size_t len, CommMsgBody& dst) const
{
	encr.decryptData(block,len,dst);
}


//****************************************************************************


CommClientEncryptedPwdGuardFactory::CommClientEncryptedPwdGuardFactory()
{
	_hasAuth = false;
	callback = 0;
}


void CommClientEncryptedPwdGuardFactory::setAuth( const char* user_, const char* pwd_,  const char* sid_, const CommMsgBody* extra_ )
{
	PASSERT3(!callback);
	PTRACE3( "CommClientEncryptedPwdGuardFactory::setAuth( '%s', '*' )", user_ );
	user = user_;
	pwd = pwd_;
	sid = sid_;
	setExtra(extra_);
	_hasAuth = true;
}

void CommClientEncryptedPwdGuardFactory::setAuth( CommClientAuthCallback & callback_ )
{
	user.clear();
	pwd.clear();
	sid.clear();
	memset(extra._writePtr(),0,extra._size());
	extra.clear();
	callback = &callback_;
	_hasAuth = true;
}


void CommClientEncryptedPwdGuardFactory::resetAuth()
{
	PTRACE3( "CommClientEncryptedPwdGuardFactory::resetAuth()" );
	user.clear();
	pwd.clear();
	sid.clear();
	memset(extra._writePtr(),0,extra._size());
	extra.clear();
	_hasAuth = false;
}

bool 	CommClientEncryptedPwdGuardFactory::hasAuth() const
{
	return _hasAuth;
}

/* virtual */ void 	CommClientEncryptedPwdGuardFactory::composeGuardNameList( CommMsgBody& msgBody ) const
{
	//NOTE: order is essential here
	msgBody.composeString( _COMM_ENCRYPTEDPASSWORDSID_GUARD_NAME );
	msgBody.composeString( _COMM_ENCRYPTEDPASSWORD_GUARD_NAME );
}

/* virtual */ /* new */ CommClientGuardAuth* CommClientEncryptedPwdGuardFactory::auth()
{
	_CommClientPlainTextPasswordSidGuardAuth* ret = new _CommClientPlainTextPasswordSidGuardAuth();
	if (callback)
	{
		ret->callback = callback;
	}
	else
	{
		PASSERT3( *user );
		ret->callback = 0;
		ret->user = user;
		ret->pwd = pwd;
		ret->sid = sid;
		ret->extra.copyFrom( extra );
	}
	return ret;
}

/* virtual */ /* new */ CommClientGuard* CommClientEncryptedPwdGuardFactory::createGuard( const char* guardName, CommClientGuardAuth* auth_ )
{
	if( strcmp( guardName, _COMM_ENCRYPTEDPASSWORDSID_GUARD_NAME ) == 0 )
	{
		_CommClientPlainTextPasswordSidGuardAuth* auth = (_CommClientPlainTextPasswordSidGuardAuth*)auth_;
		PASSERT3( auth );
		if (auth->callback)
			return new CommClientEncryptedPwdGuard( *auth->callback, true);
		else
		{
			PASSERT3( *auth->user );
			return new CommClientEncryptedPwdGuard( auth->user, auth->pwd, auth->sid, auth->extra, true );
		}
	}
	else if (strcmp( guardName, _COMM_ENCRYPTEDPASSWORD_GUARD_NAME ) == 0)
	{
		_CommClientPlainTextPasswordSidGuardAuth* auth = (_CommClientPlainTextPasswordSidGuardAuth*)auth_;//sic! - auth still has Sid-type here; we just won't use sid
		PASSERT3( auth );
		if (auth->callback)
			return new CommClientEncryptedPwdGuard( *auth->callback, false);
		else
		{
			PASSERT3( *auth->user );
			return new CommClientEncryptedPwdGuard( auth->user, auth->pwd, auth->sid, auth->extra, false );
		}
	}
	else
		PASSERT3(0);

}
