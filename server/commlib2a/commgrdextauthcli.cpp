#include "commgrdextauth.h"

CommClientExternAuthGuard::CommClientExternAuthGuard(const char* guardName_, const char* user_, const char* pwd_, const CommMsgBody& extra_)
	: CommClientGuard( guardName_ ), user( user_ ), pwd( pwd_ ), callback(nullptr)
{
	extra.copyFrom( extra_ );
}

CommClientExternAuthGuard::CommClientExternAuthGuard( const char* guardName_, CommClientAuthCallback& callback_)
	: CommClientGuard( guardName_ ), callback(&callback_)
{
}

void CommClientExternAuthGuard::composeFirstRequest( CommMsgBody& msgBody )
{
	msgBody.composeINT32(_version);
	encr.composeFirstRequest(msgBody);
}

bool CommClientExternAuthGuard::processResponse( CommMsgBody& msgBody, CommMsgParser& parser )
{
	if (callback)
	{
		PString sid;
		callback->auth(user, pwd, sid, extra);
		sid.clear();
	}

	encr.processResponse(parser);
	msgBody.composeString(user);
	PBlock encryptedUser;
	encr.encryptData((const BYTE *)user.c_str(), user.length(), encryptedUser);
	msgBody._composeVarBlock(encryptedUser.ptr(), encryptedUser.size());
	PBlock encryptedPwd;
	encr.encryptData((const BYTE *)pwd.c_str(), pwd.length(), encryptedPwd);
	msgBody._composeVarBlock(encryptedPwd.ptr(), encryptedPwd.size());
	msgBody.composeMsgBody(extra);
	if (callback)
	{
		user.clear();
		pwd.clear();
		memset(extra._writePtr(), 0, extra._size());
		extra.clear();
	}
	return true;
}

bool CommClientExternAuthGuard::processGranted( CommMsgParser& parser )
{
	const BYTE* p;
	size_t sz;
	parser._parseVarBlock(p, sz);
	PBlock ret;
	encr.decryptData(p, sz, ret);

	if (callback)
	{
		PString sid;
		callback->auth(user, pwd, sid, extra);
		sid.clear();
	}

	bool ok = ret.size() == user.length() && !memcmp(ret.ptr(), user.c_str(), ret.size());
	if (callback)
	{
		user.clear();
		pwd.clear();
		memset(extra._writePtr(), 0, extra._size());
		extra.clear();
	}
	return ok;
}

void CommClientExternAuthGuard::composeGuardNameList( CommMsgBody& msgBody ) const
{
    msgBody.composeString( _COMM_EXTERN_AUTH_GUARD_NAME );
}

void CommClientExternAuthGuard::processOutBlock( CommMsgBody& dst, CommMsgBody& src )
{
    dst._composeVarBlock( src._readPtr(), src._size() );
}

void CommClientExternAuthGuard::processInBlock( CommMsgBody& dst, CommMsgParser& src )
{
    const BYTE* p;
    size_t sz;
    src._parseVarBlock( p, sz );
    memcpy( dst._alloc( sz ), p, sz );
}

void CommClientExternAuthGuard::encryptMsg(const CommMsgBody& src, PBlock& dst) const
{
	encr.encryptData(src, dst);
}

void CommClientExternAuthGuard::decryptMsg(const BYTE * block, size_t len, CommMsgBody& dst) const
{
	encr.decryptData(block, len, dst);
}

struct _CommClientExternAuthGuardAuth : public CommClientGuardAuth
{
	_CommClientExternAuthGuardAuth() : callback(nullptr) {}

    PString user;
    PString pwd;
	CommClientAuthCallback* callback;
    CommMsgBody extra;
};

//------------------------------------------------------------------------------

CommClientExternAuthGuardFactory::CommClientExternAuthGuardFactory()
	: _hasAuth( false ), callback( nullptr )
{
}

CommClientExternAuthGuardFactory::CommClientExternAuthGuardFactory( const char* user_, const char* pwd_ )
	: user( user_ ), pwd( pwd_ ), _hasAuth( false ), callback( nullptr )
{
}

void CommClientExternAuthGuardFactory::setAuth( CommClientAuthCallback& callback_ )
{
	user.clear();
	pwd.clear();
	memset( extra._writePtr(), 0, extra._size() );
	extra.clear();
	callback = &callback_;
	_hasAuth = true;
}

void CommClientExternAuthGuardFactory::setAuth( const char* user_, const char* pwd_, const CommMsgBody* extra_)
{
	PASSERT3( !callback );
    PTRACE3( "%s::setAuth( '%s', '*' )", whoAmI(), user_ );
    user = user_;
    pwd = pwd_;
    if( extra_ )
        extra.copyFrom( *extra_ );
    else
        extra.clear();
    _hasAuth = true;
}

void CommClientExternAuthGuardFactory::resetAuth()
{
    PTRACE3( "%s::resetAuth()", whoAmI());
	user.clear();
	pwd.clear();
	memset( extra._writePtr(), 0, extra._size() );
	extra.clear();
	callback = nullptr;
    _hasAuth = false;
}

void CommClientExternAuthGuardFactory::composeGuardNameList( CommMsgBody& msgBody ) const
{
    msgBody.composeString( _COMM_EXTERN_AUTH_GUARD_NAME );
}

/* new */ CommClientGuardAuth* CommClientExternAuthGuardFactory::auth()
{
    PASSERT3( _hasAuth );
    _CommClientExternAuthGuardAuth* ret = new _CommClientExternAuthGuardAuth;
	if( callback )
	{
		ret->callback = callback;
	}
	else
	{
		PASSERT3( *user );
		ret->callback = nullptr;
		ret->user = user;
		ret->pwd = pwd;
		ret->extra.copyFrom( extra );
	}
    return ret;
}

/* new */ CommClientGuard* CommClientExternAuthGuardFactory::createGuard( const char* guardName, CommClientGuardAuth* auth_ )
{
    PASSERT3( strcmp( guardName, _COMM_EXTERN_AUTH_GUARD_NAME ) == 0 );
    _CommClientExternAuthGuardAuth* auth = (_CommClientExternAuthGuardAuth*)auth_;
    PASSERT3( auth );

	if( auth->callback )
		return new CommClientExternAuthGuard( *( auth->callback ));

    PASSERT3( *auth->user );
    return new CommClientExternAuthGuard( auth->user, auth->pwd, auth->extra);
}

struct _CommClientExternAuthOperatorTokenGuardAuth : public CommClientGuardAuth
{
	_CommClientExternAuthOperatorTokenGuardAuth() : callback(nullptr) {}

	PString user;
	PString token;
	CommClientAuthCallback* callback;
	CommMsgBody extra;
};

void CommClientExternAuthOperatorTokenGuardFactory::composeGuardNameList( CommMsgBody& msgBody ) const
{
	msgBody.composeString(_COMM_EXTERN_AUTH_OPERATOR_TOKEN_GUARD_NAME);
}

/* new */ CommClientGuardAuth* CommClientExternAuthOperatorTokenGuardFactory::auth()
{
	PASSERT3( _hasAuth );
	_CommClientExternAuthOperatorTokenGuardAuth* ret = new _CommClientExternAuthOperatorTokenGuardAuth;
	if (callback)
	{
		ret->callback = callback;
	}
	else
	{
		PASSERT3(*user);
		ret->callback = nullptr;
		ret->user = user;
		ret->token = pwd;
		ret->extra.copyFrom(extra);
	}
	return ret;
}

/* new */ CommClientGuard* CommClientExternAuthOperatorTokenGuardFactory::createGuard(const char* guardName, CommClientGuardAuth* auth_)
{
	PASSERT3(strcmp(guardName, _COMM_EXTERN_AUTH_OPERATOR_TOKEN_GUARD_NAME) == 0);
	_CommClientExternAuthOperatorTokenGuardAuth* auth = (_CommClientExternAuthOperatorTokenGuardAuth*)auth_;
	PASSERT3(auth);

	if (auth->callback)
		return new CommClientExternAuthOperatorTokenGuard(*(auth->callback));

	PASSERT3(*auth->user);
	return new CommClientExternAuthOperatorTokenGuard(auth->user, auth->token, auth->extra);
}
