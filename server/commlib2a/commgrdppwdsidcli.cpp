#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"

#include "commmsgbody.h"
#include "commqueue.h"
#include "commcli.h"
#include "commgrdppwdsid.h"

CommClientPlainTextPasswordSidGuard::CommClientPlainTextPasswordSidGuard( const char* user_, const char* pwd_, const char* sid_, const CommMsgBody& extra_, bool useSid_ )
: CommClientGuard( _COMM_PPWDSID_GUARD_NAME ), 
user( user_ ),
pwd( pwd_ ),
sid( sid_ )
{
	callback = 0;
	useSid = useSid_;
	extra.copyFrom( extra_ );
}

CommClientPlainTextPasswordSidGuard::CommClientPlainTextPasswordSidGuard( CommClientAuthCallback & callback_, bool useSid_)
: CommClientGuard( _COMM_PPWDSID_GUARD_NAME ) 
{
	callback = &callback_;
	useSid = useSid_;
}

/* virtual */ void CommClientPlainTextPasswordSidGuard::composeFirstRequest( CommMsgBody& msgBody )
{
	if (callback)
	{
		callback->auth(user,pwd,sid,extra);
	}

	if (useSid)
		msgBody.composeString( user ).composeString( pwd ).composeString( sid ).composeMsgBody( extra );
	else
		msgBody.composeString( user ).composeString( pwd ).composeMsgBody( extra );

	if (callback)
	{
		user.clear();
		pwd.clear();
		sid.clear();
		memset(extra._writePtr(),0,extra._size());
		extra.clear();
	}

}

/* virtual */ bool CommClientPlainTextPasswordSidGuard::processResponse( CommMsgBody& msgBody, CommMsgParser& parser )
{
	PASSERT3( 0 );
	return false;
}

/* virtual */ bool CommClientPlainTextPasswordSidGuard::processGranted( CommMsgParser& parser )
{
	return true;
}

/* virtual */ void CommClientPlainTextPasswordSidGuard::processOutBlock( CommMsgBody& dst, CommMsgBody& src )
{
	dst._composeVarBlock( src._readPtr(), src._size() );
}

/* virtual */ void CommClientPlainTextPasswordSidGuard::processInBlock( CommMsgBody& dst, CommMsgParser& src )
{
	const BYTE* p;
	size_t sz;
	src._parseVarBlock( p, sz );
	memcpy( dst._alloc( sz ), p, sz );
}

//****************************************************************************


CommClientPlainTextPasswordSidGuardFactory::CommClientPlainTextPasswordSidGuardFactory()
{
	callback = 0;
	_hasAuth = false;
}

CommClientPlainTextPasswordSidGuardFactory::CommClientPlainTextPasswordSidGuardFactory( const char* user_, const char* pwd_, const char* sid_ )
: user( user_ ),
pwd( pwd_ ),
sid( sid_ )
{
	callback = 0;
	_hasAuth = true;
}

CommClientPlainTextPasswordSidGuardFactory::CommClientPlainTextPasswordSidGuardFactory( CommClientAuthCallback & callback_ )
{
	callback = &callback_;
	_hasAuth = true;
}

void CommClientPlainTextPasswordSidGuardFactory::setAuth( const char* user_, const char* pwd_, const char* sid_, const CommMsgBody* extra_ )
{
	PASSERT3(!callback);
	PTRACE3( "CommClientPlainTextPasswordSidGuardFactory::setAuth( '%s', '*' )", user_ );
	user = user_;
	pwd = pwd_;
	sid = sid_;
	setExtra( extra_ );
	_hasAuth = true;
}

void CommClientPlainTextPasswordSidGuardFactory::setAuth( CommClientAuthCallback & callback_ )
{
	user.clear();
	pwd.clear();
	sid.clear();
	memset(extra._writePtr(),0,extra._size());
	extra.clear();
	callback = &callback_;
	_hasAuth = true;
}

void CommClientPlainTextPasswordSidGuardFactory::resetAuth()
{
	PTRACE3( "CommClientPlainTextPasswordSidGuardFactory::resetAuth()" );
	callback=0;
	user.clear();
	pwd.clear();
	sid.clear();
	memset(extra._writePtr(),0,extra._size());
	extra.clear();
	_hasAuth = false;
}

bool CommClientPlainTextPasswordSidGuardFactory::hasAuth() const
{
	return _hasAuth;
}

/* virtual */ void CommClientPlainTextPasswordSidGuardFactory::composeGuardNameList( CommMsgBody& msgBody ) const
{
	//NOTE: order is essential here
	msgBody.composeString( _COMM_PPWDSID_GUARD_NAME );
	msgBody.composeString( _COMM_PPWD_GUARD_NAME );//sic! - supports both
}

/* virtual */ /* new */ CommClientGuardAuth* CommClientPlainTextPasswordSidGuardFactory::auth()
{
	PASSERT3( _hasAuth );
	_CommClientPlainTextPasswordSidGuardAuth* ret = new _CommClientPlainTextPasswordSidGuardAuth();
	if (callback)
	{
		ret->callback = callback;
	}
	else
	{
		ret->callback = 0;
		ret->user = user;
		ret->pwd = pwd;
		ret->sid = sid;
		ret->extra.copyFrom( extra );
	}
	return ret;
}

/* virtual */ /* new */ CommClientGuard* CommClientPlainTextPasswordSidGuardFactory::createGuard( const char* guardName, CommClientGuardAuth* auth_ )
{
	if( strcmp( guardName, _COMM_PPWDSID_GUARD_NAME ) == 0 )
	{
		_CommClientPlainTextPasswordSidGuardAuth* auth = (_CommClientPlainTextPasswordSidGuardAuth*)auth_;
		PASSERT3( auth );
		if (auth->callback)
			return new CommClientPlainTextPasswordSidGuard( *auth->callback, true);
		else
		{
			PASSERT3( *auth->user );
			return new CommClientPlainTextPasswordSidGuard( auth->user, auth->pwd, auth->sid, auth->extra, true );
		}
	}
	else
	{
		PASSERT3( strcmp( guardName, _COMM_PPWD_GUARD_NAME ) == 0 );
		_CommClientPlainTextPasswordSidGuardAuth* auth = (_CommClientPlainTextPasswordSidGuardAuth*)auth_;//sic! - auth still has Sid-type here; we just won't use sid
		PASSERT3( auth );
		if (auth->callback)
			return new CommClientPlainTextPasswordSidGuard( *auth->callback, false);
		else
		{
			PASSERT3( *auth->user );
			return new CommClientPlainTextPasswordSidGuard( auth->user, auth->pwd, auth->sid, auth->extra, false );
		}
	}
}



