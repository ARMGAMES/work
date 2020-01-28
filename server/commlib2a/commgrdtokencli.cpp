#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"

#include "commmsgbody.h"
#include "commqueue.h"
#include "commcli.h"
#include "commgrdtoken.h"

CommClientTokenGuard::CommClientTokenGuard( const char* user_, const PBlock & token_, const CommMsgBody& extra_ )
: CommClientGuard( _COMM_TOKEN_GUARD_NAME ), 
user( user_ ),
token(token_)
{
	callback = 0;
	extra.copyFrom( extra_ );
}

CommClientTokenGuard::CommClientTokenGuard( CommClientAuthCallback & callback_, const PBlock & token_)
: CommClientGuard( _COMM_TOKEN_GUARD_NAME ), 
token(token_)
{
	callback = &callback_;
}


/* virtual */ void CommClientTokenGuard::composeFirstRequest( CommMsgBody& msgBody )
{
	if (callback)
	{
		PString sid;
		PString pwd;
		callback->auth(user,pwd,sid,extra);
		sid.clear();
		pwd.clear();
	}
	msgBody.composeUINT32(_version);
	msgBody.composeString( user.c_str() );
	msgBody._composeVarBlock(token.ptr(),token.size());
	msgBody.composeMsgBody( extra );
	encr.composeFirstRequest(msgBody);
	if (callback)
	{
		user.clear();
		memset(extra._writePtr(),0,extra._size());
		extra.clear();
	}

}

/* virtual */ bool CommClientTokenGuard::processResponse( CommMsgBody& msgBody, CommMsgParser& parser )
{
	PASSERT3( 0 );
	return false;
}

/* virtual */ bool CommClientTokenGuard::processGranted( CommMsgParser& parser )
{
	if (!parser.parseEnded())
		encr.processResponse(parser);
	return true;
}

/* virtual */ void CommClientTokenGuard::processOutBlock( CommMsgBody& dst, CommMsgBody& src )
{
	dst._composeVarBlock( src._readPtr(), src._size() );
}

/* virtual */ void CommClientTokenGuard::processInBlock( CommMsgBody& dst, CommMsgParser& src )
{
	const BYTE* p;
	size_t sz;
	src._parseVarBlock( p, sz );
	memcpy( dst._alloc( sz ), p, sz );
}

void CommClientTokenGuard::encryptMsg(const CommMsgBody& src, PBlock& dst) const
{
	if (encr.isInit())
		encr.encryptData(src,dst);
	else
		CommClientGuard::encryptMsg(src,dst);
}
void CommClientTokenGuard::decryptMsg(const BYTE * block, size_t len, CommMsgBody& dst) const
{
	if (encr.isInit())
		encr.decryptData(block,len,dst);
	else
		CommClientGuard::decryptMsg(block,len,dst);

}

//****************************************************************************

class _CommClientTokenGuardAuth : public CommClientGuardAuth
{
public:
	PString user;
	CommMsgBody extra;
	CommClientAuthCallback * callback;

public:
	_CommClientTokenGuardAuth()
	{
		callback = 0;
	}

	/* virtual */ ~_CommClientTokenGuardAuth() {}
};

CommClientTokenGuardFactory::CommClientTokenGuardFactory()
{
	callback = 0;
}


void CommClientTokenGuardFactory::setAuth( const char* user_,  const CommMsgBody* extra_ )
{
	PASSERT3(!callback);
	PTRACE3( "CommClientTokenGuardFactory::setAuth( '%s', '*' )", user_ );
	user = user_;
	if( extra_ )
		extra.copyFrom( *extra_ );
	else
		extra.clear();
}
void CommClientTokenGuardFactory::setAuth(CommClientAuthCallback & callback_ )
{
	callback = &callback_;
	user.clear();
	memset(extra._writePtr(),0,extra._size());
	extra.clear();
}

void CommClientTokenGuardFactory::setAuthSecurityToken( CommMsgParser& parser)
{
	token.clear();
	INT32 ver;
	parser.parseINT32(ver);
	if (ver != CommClientTokenGuard::getVersion())
	{
		PTRACE3("CommClientTokenGuardFactory wrong peer version %d, current %d",ver, CommClientTokenGuard::getVersion());
	}
	parser.parsePBlock(token);
	PASSERT3(token.size());
}


void CommClientTokenGuardFactory::resetAuth()
{
	PTRACE3( "CommClientTokenGuardFactory::resetAuth()" );
	user.clear();
	token.clear();
	callback=0;

}

bool CommClientTokenGuardFactory::hasAuth() const
{
	return (user.length() > 0 || callback) && token.size()>0;
}

/* virtual */ void CommClientTokenGuardFactory::composeGuardNameList( CommMsgBody& msgBody ) const
{
	msgBody.composeString( _COMM_TOKEN_GUARD_NAME );
}

/* virtual */ /* new */ CommClientGuardAuth* CommClientTokenGuardFactory::auth()
{
	_CommClientTokenGuardAuth* ret = new _CommClientTokenGuardAuth();
	if (callback)
		ret->callback = callback;
	else
	{
		ret->callback = 0;
		PASSERT3( *user );
		ret->extra.copyFrom( extra );
		ret->user = user;
	}

	return ret;
}

/* virtual */ /* new */ CommClientGuard* CommClientTokenGuardFactory::createGuard( const char* guardName, CommClientGuardAuth* auth_ )
{
	PASSERT3( strcmp( guardName, _COMM_TOKEN_GUARD_NAME ) == 0 );
	_CommClientTokenGuardAuth* auth = (_CommClientTokenGuardAuth*)auth_;
	PASSERT3( auth );
	if (callback)
	{
		return new CommClientTokenGuard( *auth->callback, token );
	}
	else
	{
		PASSERT3( *auth->user );
		return new CommClientTokenGuard( auth->user, token, auth->extra );
	}
}
