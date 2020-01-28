#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"

#include "commmsgbody.h"
#include "commqueue.h"
#include "commcli.h"
#include "commgrdwebtokenint.h"

CommClientWebTokenIntGuard::CommClientWebTokenIntGuard( UINT32 userIntId_, const char * token_, const CommMsgBody& extra_ )
: CommClientGuard( _COMM_WEBTOKENINT_GUARD_NAME ) 
{
	callback = 0;
	token = token_;
	userIntId =userIntId_;
	extra.copyFrom( extra_ );
}

CommClientWebTokenIntGuard::CommClientWebTokenIntGuard(CommClientAuthCallback & callback_)
: CommClientGuard( _COMM_WEBTOKENINT_GUARD_NAME ) 
{
	callback = &callback_;
}

/* virtual */ void CommClientWebTokenIntGuard::composeFirstRequest( CommMsgBody& msgBody )
{
	if (callback)
	{
		PString sid;
		callback->auth(userIntId,token,sid,extra);
		sid.clear();
	}

	msgBody.composeUINT32(_version);
	msgBody.composeUINT32(userIntId).composeString(token);
	msgBody.composeMsgBody( extra );
	encr.composeFirstRequest(msgBody);
	if (callback)
	{
		memset(extra._writePtr(),0,extra._size());
		extra.clear();
		token.clear();
		userIntId = 0;
	}

}

/* virtual */ bool CommClientWebTokenIntGuard::processResponse( CommMsgBody& msgBody, CommMsgParser& parser )
{
	PASSERT3( 0 );
	return false;
}

/* virtual */ bool CommClientWebTokenIntGuard::processGranted( CommMsgParser& parser )
{
	if (!parser.parseEnded())
		encr.processResponse(parser);
	return true;
}

/* virtual */ void CommClientWebTokenIntGuard::processOutBlock( CommMsgBody& dst, CommMsgBody& src )
{
	dst._composeVarBlock( src._readPtr(), src._size() );
}

/* virtual */ void CommClientWebTokenIntGuard::processInBlock( CommMsgBody& dst, CommMsgParser& src )
{
	const BYTE* p;
	size_t sz;
	src._parseVarBlock( p, sz );
	memcpy( dst._alloc( sz ), p, sz );
}

void CommClientWebTokenIntGuard::encryptMsg(const CommMsgBody& src, PBlock& dst) const
{
	if (encr.isInit())
		encr.encryptData(src,dst);
	else
		CommClientGuard::encryptMsg(src,dst);
}
void CommClientWebTokenIntGuard::decryptMsg(const BYTE * block, size_t len, CommMsgBody& dst) const
{
	if (encr.isInit())
		encr.decryptData(block,len,dst);
	else
		CommClientGuard::decryptMsg(block,len,dst);
}

//****************************************************************************

class _CommClientWebTokenIntGuardAuth : public CommClientGuardAuth
{
public:
	PString token;
	UINT32 userIntId;
	CommMsgBody extra;
	CommClientAuthCallback * callback;

public:
	_CommClientWebTokenIntGuardAuth()
	{
		callback = 0;
	}

	/* virtual */ ~_CommClientWebTokenIntGuardAuth() {}
};

CommClientWebTokenIntGuardFactory::CommClientWebTokenIntGuardFactory()
{
	callback = 0;
}


void CommClientWebTokenIntGuardFactory::setAuth( UINT32 userIntId_, const char * token_,  const CommMsgBody* extra_ )
{
	PASSERT3(!callback);
	PTRACE3( "CommClientWebTokenIntGuardFactory::setAuth( '%u' )",userIntId_);
	token = token_;
	userIntId = userIntId_;
	if( extra_ )
		extra.copyFrom( *extra_ );
	else
		extra.clear();
}

void CommClientWebTokenIntGuardFactory::setAuth(  CommClientAuthCallback & callback_ )
{
	PTRACE3( "CommClientWebTokenIntGuardFactory::setAuth( 'callback' )");
	callback = &callback_;
	memset(extra._writePtr(),0,extra._size());
	extra.clear();
	token.clear();
	userIntId=0;
}


void CommClientWebTokenIntGuardFactory::resetAuth()
{
	PTRACE3( "CommClientWebTokenIntGuardFactory::resetAuth()" );
	token.clear();
	userIntId=0;
	memset(extra._writePtr(),0,extra._size());
	extra.clear();
	callback = 0;

}

bool CommClientWebTokenIntGuardFactory::hasAuth() const
{
	return callback || (token.length() > 0 && userIntId != 0); 
}

/* virtual */ void CommClientWebTokenIntGuardFactory::composeGuardNameList( CommMsgBody& msgBody ) const
{
	msgBody.composeString( _COMM_WEBTOKENINT_GUARD_NAME );
}

/* virtual */ /* new */ CommClientGuardAuth* CommClientWebTokenIntGuardFactory::auth()
{
	PASSERT3( *token || callback );
	_CommClientWebTokenIntGuardAuth* ret = new _CommClientWebTokenIntGuardAuth();
	if (callback)
		ret->callback = callback;
	else
	{
		ret->token = token;
		ret->userIntId = userIntId;
		ret->callback = 0;
		ret->extra.copyFrom( extra );
	}
	return ret;
}

/* virtual */ /* new */ CommClientGuard* CommClientWebTokenIntGuardFactory::createGuard( const char* guardName, CommClientGuardAuth* auth_ )
{
	PASSERT3( strcmp( guardName, _COMM_WEBTOKENINT_GUARD_NAME ) == 0 );
	_CommClientWebTokenIntGuardAuth* auth = (_CommClientWebTokenIntGuardAuth*)auth_;
	PASSERT3( auth );

	if (callback)
	{
		return new CommClientWebTokenIntGuard( *auth->callback );
	}
	else
	{
		PASSERT3( *auth->token );
		PASSERT3( auth->userIntId );
		return new CommClientWebTokenIntGuard( auth->userIntId, auth->token, auth->extra );
	}
}
