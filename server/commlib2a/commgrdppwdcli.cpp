#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"

#include "commmsgbody.h"
#include "commqueue.h"
#include "commcli.h"
#include "commgrdppwd.h"

CommClientPlainTextPasswordGuard::CommClientPlainTextPasswordGuard( const char* user_, const char* pwd_, const CommMsgBody& extra_ )
: CommClientGuard( _COMM_PPWD_GUARD_NAME ), 
user( user_ ),
pwd( pwd_ )
{
	extra.copyFrom( extra_ );
}

/* virtual */ void CommClientPlainTextPasswordGuard::composeFirstRequest( CommMsgBody& msgBody )
{
	msgBody.composeString( user ).composeString( pwd ).composeMsgBody( extra );
}

/* virtual */ bool CommClientPlainTextPasswordGuard::processResponse( CommMsgBody& msgBody, CommMsgParser& parser )
{
	PASSERT3( 0 );
	return false;
}

/* virtual */ bool CommClientPlainTextPasswordGuard::processGranted( CommMsgParser& parser )
{
	return true;
}

/* virtual */ void CommClientPlainTextPasswordGuard::processOutBlock( CommMsgBody& dst, CommMsgBody& src )
{
	dst._composeVarBlock( src._readPtr(), src._size() );
}

/* virtual */ void CommClientPlainTextPasswordGuard::processInBlock( CommMsgBody& dst, CommMsgParser& src )
{
	const BYTE* p;
	size_t sz;
	src._parseVarBlock( p, sz );
	memcpy( dst._alloc( sz ), p, sz );
}

//****************************************************************************

class _CommClientPlainTextPasswordGuardAuth : public CommClientGuardAuth
{
public:
	PString user;
	PString pwd;
	CommMsgBody extra;

public:

	/* virtual */ ~_CommClientPlainTextPasswordGuardAuth() {}
};

CommClientPlainTextPasswordGuardFactory::CommClientPlainTextPasswordGuardFactory()
{
	_hasAuth = false;
}

CommClientPlainTextPasswordGuardFactory::CommClientPlainTextPasswordGuardFactory( const char* user_, const char* pwd_ )
: user( user_ ),
pwd( pwd_ )
{
	_hasAuth = false;
}

void CommClientPlainTextPasswordGuardFactory::setAuth( const char* user_, const char* pwd_, const CommMsgBody* extra_ )
{
	PTRACE3( "CommClientPlainTextPasswordGuardFactory::setAuth( '%s', '*' )", user_ );
	user = user_;
	pwd = pwd_;
	if( extra_ )
		extra.copyFrom( *extra_ );
	else
		extra.clear();
	_hasAuth = true;
}

void CommClientPlainTextPasswordGuardFactory::resetAuth()
{
	PTRACE3( "CommClientPlainTextPasswordGuardFactory::resetAuth()" );
	user = "";
	pwd = "";
	_hasAuth = false;
}

bool CommClientPlainTextPasswordGuardFactory::hasAuth() const
{
	return _hasAuth;
}

/* virtual */ void CommClientPlainTextPasswordGuardFactory::composeGuardNameList( CommMsgBody& msgBody ) const
{
	msgBody.composeString( _COMM_PPWD_GUARD_NAME );
}

/* virtual */ /* new */ CommClientGuardAuth* CommClientPlainTextPasswordGuardFactory::auth()
{
	PASSERT3( *user );
	_CommClientPlainTextPasswordGuardAuth* ret = new _CommClientPlainTextPasswordGuardAuth();
	ret->user = user;
	ret->pwd = pwd;
	ret->extra.copyFrom( extra );
	return ret;
}

/* virtual */ /* new */ CommClientGuard* CommClientPlainTextPasswordGuardFactory::createGuard( const char* guardName, CommClientGuardAuth* auth_ )
{
	PASSERT3( strcmp( guardName, _COMM_PPWD_GUARD_NAME ) == 0 );
	_CommClientPlainTextPasswordGuardAuth* auth = (_CommClientPlainTextPasswordGuardAuth*)auth_;
	PASSERT3( auth );
	PASSERT3( *auth->user );
	return new CommClientPlainTextPasswordGuard( auth->user, auth->pwd, auth->extra );
}
