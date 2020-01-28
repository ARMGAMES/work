#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"

#include "commmsgbody.h"
#include "commqueue.h"
#include "commcli.h"
#include "commgrdpftppwd.h"

CommClientFtpPasswordGuard::CommClientFtpPasswordGuard( const char* user_, const char* pwd_, const CommMsgBody& extra_ )
: CommClientGuard( _COMM_PFTPPWD_GUARD_NAME ), 
user( user_ ),
pwd( pwd_ )
{
	extra.copyFrom( extra_ );
}

/* virtual */ void CommClientFtpPasswordGuard::composeFirstRequest( CommMsgBody& msgBody )
{
	msgBody.composeString( user ).composeString( pwd ).composeMsgBody( extra );
}

/* virtual */ bool CommClientFtpPasswordGuard::processResponse( CommMsgBody& msgBody, CommMsgParser& parser )
{
	PASSERT3( 0 );
	return false;
}

/* virtual */ bool CommClientFtpPasswordGuard::processGranted( CommMsgParser& parser )
{
	return true;
}

/* virtual */ void CommClientFtpPasswordGuard::processOutBlock( CommMsgBody& dst, CommMsgBody& src )
{
	dst._composeVarBlock( src._readPtr(), src._size() );
}

/* virtual */ void CommClientFtpPasswordGuard::processInBlock( CommMsgBody& dst, CommMsgParser& src )
{
	const BYTE* p;
	size_t sz;
	src._parseVarBlock( p, sz );
	memcpy( dst._alloc( sz ), p, sz );
}

//****************************************************************************

class _CommClientFtpPasswordGuardAuth : public CommClientGuardAuth
{
public:
	PString user;
	PString pwd;
	CommMsgBody extra;

public:

	/* virtual */ ~_CommClientFtpPasswordGuardAuth() {}
};

CommClientFtpPasswordGuardFactory::CommClientFtpPasswordGuardFactory()
{
	_hasAuth = false;
}

CommClientFtpPasswordGuardFactory::CommClientFtpPasswordGuardFactory( const char* user_, const char* pwd_ )
: user( user_ ),
pwd( pwd_ )
{
	_hasAuth = false;
}

void CommClientFtpPasswordGuardFactory::setAuth( const char* user_, const char* pwd_, const CommMsgBody* extra_ )
{
	PTRACE3( "CommClientFtpPasswordGuardFactory::setAuth( '%s', '*' )", user_ );
	user = user_;
	pwd = pwd_;
	if( extra_ )
		extra.copyFrom( *extra_ );
	else
		extra.clear();
	_hasAuth = true;
}

void CommClientFtpPasswordGuardFactory::resetAuth()
{
	PTRACE3( "CommClientFtpPasswordGuardFactory::resetAuth()" );
	user = "";
	pwd = "";
	_hasAuth = false;
}

bool CommClientFtpPasswordGuardFactory::hasAuth() const
{
	return _hasAuth;
}

/* virtual */ void CommClientFtpPasswordGuardFactory::composeGuardNameList( CommMsgBody& msgBody ) const
{
	msgBody.composeString( _COMM_PFTPPWD_GUARD_NAME );
}

/* virtual */ /* new */ CommClientGuardAuth* CommClientFtpPasswordGuardFactory::auth()
{
	PASSERT3( *user );
	_CommClientFtpPasswordGuardAuth* ret = new _CommClientFtpPasswordGuardAuth();
	ret->user = user;
	ret->pwd = pwd;
	ret->extra.copyFrom( extra );
	return ret;
}

/* virtual */ /* new */ CommClientGuard* CommClientFtpPasswordGuardFactory::createGuard( const char* guardName, CommClientGuardAuth* auth_ )
{
	PASSERT3( strcmp( guardName, _COMM_PFTPPWD_GUARD_NAME ) == 0 );
	_CommClientFtpPasswordGuardAuth* auth = (_CommClientFtpPasswordGuardAuth*)auth_;
	PASSERT3( auth );
	PASSERT3( *auth->user );
	return new CommClientFtpPasswordGuard( auth->user, auth->pwd, auth->extra );
}
