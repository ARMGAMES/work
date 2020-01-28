#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"

#include "commmsgbody.h"
#include "commqueue.h"
#include "commcli.h"
#include "commgrdwebtoken.h"

CommClientWebTokenGuard::CommClientWebTokenGuard( const char * user_, const char * token_, const CommMsgBody& extra_ )
: CommClientGuard( _COMM_WEBTOKEN_GUARD_NAME ) 
{
	callback = 0;
	token = token_;
	user =user_;
	extra.copyFrom( extra_ );
}

CommClientWebTokenGuard::CommClientWebTokenGuard(CommClientAuthCallback & callback_)
: CommClientGuard( _COMM_WEBTOKEN_GUARD_NAME ) 
{
	callback = &callback_;
}

/* virtual */ void CommClientWebTokenGuard::composeFirstRequest( CommMsgBody& msgBody )
{
	if (callback)
	{
		PString sid;
		callback->auth(user,token,sid,extra);
		sid.clear();
	}

	msgBody.composeUINT32(_version);
	msgBody.composeString(user).composeString(token);
	msgBody.composeMsgBody( extra );
	encr.composeFirstRequest(msgBody);
	if (callback)
	{
		memset(extra._writePtr(),0,extra._size());
		extra.clear();
		token.clear();
		user.clear();
	}

}

/* virtual */ bool CommClientWebTokenGuard::processResponse( CommMsgBody& msgBody, CommMsgParser& parser )
{
	PASSERT3( 0 );
	return false;
}

/* virtual */ bool CommClientWebTokenGuard::processGranted( CommMsgParser& parser )
{
	if (!parser.parseEnded())
		encr.processResponse(parser);
	return true;
}

/* virtual */ void CommClientWebTokenGuard::processOutBlock( CommMsgBody& dst, CommMsgBody& src )
{
	dst._composeVarBlock( src._readPtr(), src._size() );
}

/* virtual */ void CommClientWebTokenGuard::processInBlock( CommMsgBody& dst, CommMsgParser& src )
{
	const BYTE* p;
	size_t sz;
	src._parseVarBlock( p, sz );
	memcpy( dst._alloc( sz ), p, sz );
}

void CommClientWebTokenGuard::encryptMsg(const CommMsgBody& src, PBlock& dst) const
{
	if (encr.isInit())
		encr.encryptData(src,dst);
	else
		CommClientGuard::encryptMsg(src,dst);
}
void CommClientWebTokenGuard::decryptMsg(const BYTE * block, size_t len, CommMsgBody& dst) const
{
	if (encr.isInit())
		encr.decryptData(block,len,dst);
	else
		CommClientGuard::decryptMsg(block,len,dst);
}

//****************************************************************************

class _CommClientWebTokenGuardAuth : public CommClientGuardAuth
{
public:
	PString token;
	PString user;
	CommMsgBody extra;
	CommClientAuthCallback * callback;

public:
	_CommClientWebTokenGuardAuth()
	{
		callback = 0;
	}

	/* virtual */ ~_CommClientWebTokenGuardAuth() {}
};

CommClientWebTokenGuardFactory::CommClientWebTokenGuardFactory()
{
	callback = 0;
}


void CommClientWebTokenGuardFactory::setAuth( const char * user_, const char * token_,  const CommMsgBody* extra_ )
{
	PASSERT3(!callback);
	PTRACE3( "CommClientWebTokenGuardFactory::setAuth( '%s' )",user_);
	token = token_;
	user = user_;
	if( extra_ )
		extra.copyFrom( *extra_ );
	else
		extra.clear();
}

void CommClientWebTokenGuardFactory::setAuth(  CommClientAuthCallback & callback_ )
{
	PTRACE3( "CommClientWebTokenGuardFactory::setAuth( 'callback' )");
	callback = &callback_;
	memset(extra._writePtr(),0,extra._size());
	extra.clear();
	token.clear();
	user.clear();
}


void CommClientWebTokenGuardFactory::resetAuth()
{
	PTRACE3( "CommClientTokenGuardFactory::resetAuth()" );
	token.clear();
	user.clear();
	memset(extra._writePtr(),0,extra._size());
	extra.clear();
	callback = 0;

}

bool CommClientWebTokenGuardFactory::hasAuth() const
{
	return callback || (token.length() > 0 && user.length() > 0); 
}

/* virtual */ void CommClientWebTokenGuardFactory::composeGuardNameList( CommMsgBody& msgBody ) const
{
	msgBody.composeString( _COMM_WEBTOKEN_GUARD_NAME );
}

/* virtual */ /* new */ CommClientGuardAuth* CommClientWebTokenGuardFactory::auth()
{
	PASSERT3( *token );
	_CommClientWebTokenGuardAuth* ret = new _CommClientWebTokenGuardAuth();
	if (callback)
		ret->callback = callback;
	else
	{
		ret->token = token;
		ret->user = user;
		ret->callback = 0;
		ret->extra.copyFrom( extra );
	}
	return ret;
}

/* virtual */ /* new */ CommClientGuard* CommClientWebTokenGuardFactory::createGuard( const char* guardName, CommClientGuardAuth* auth_ )
{
	PASSERT3( strcmp( guardName, _COMM_WEBTOKEN_GUARD_NAME ) == 0 );
	_CommClientWebTokenGuardAuth* auth = (_CommClientWebTokenGuardAuth*)auth_;
	PASSERT3( auth );

	if (callback)
	{
		return new CommClientWebTokenGuard( *auth->callback );
	}
	else
	{
		PASSERT3( *auth->token );
		PASSERT3( *auth->user );
		return new CommClientWebTokenGuard( auth->user, auth->token, auth->extra );
	}
}
