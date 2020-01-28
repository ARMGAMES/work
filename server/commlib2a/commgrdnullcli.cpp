#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"

#include "commmsgbody.h"
#include "commqueue.h"
#include "commcli.h"
#include "commgrdnull.h"

CommClientNullGuard::CommClientNullGuard(const CommMsgBody& extra_)
: CommClientGuard( _COMM_NULL_GUARD_NAME )
{
	extra.copyFrom( extra_ );
}

/* virtual */ void CommClientNullGuard::composeFirstRequest( CommMsgBody& msgBody )
{
	if (extra._size())
		msgBody.composeMsgBody( extra );
}

/* virtual */ bool CommClientNullGuard::processResponse( CommMsgBody& msgBody, CommMsgParser& parser )
{
	PASSERT3( 0 );
	return false;
}

/* virtual */ bool CommClientNullGuard::processGranted( CommMsgParser& parser )
{
	return true;
}

/* virtual */ void CommClientNullGuard::processOutBlock( CommMsgBody& dst, CommMsgBody& src )
{
	dst._composeVarBlock( src._readPtr(), src._size() );
}

/* virtual */ void CommClientNullGuard::processInBlock( CommMsgBody& dst, CommMsgParser& src )
{
	const BYTE* p;
	size_t sz;
	src._parseVarBlock( p, sz );
	memcpy( dst._alloc( sz ), p, sz );
}

//****************************************************************************

class _CommClientNullGuardAuth : public CommClientGuardAuth
{
public:
	CommMsgBody extra;

public:

	/* virtual */ ~_CommClientNullGuardAuth() {}
};

void CommClientNullGuardFactory::setAuth(const CommMsgBody & extra_)
{
	extra.copyFrom( extra_ );
}


/* virtual */ /* new */ CommClientGuardAuth* CommClientNullGuardFactory::auth()
{
	_CommClientNullGuardAuth* ret = new _CommClientNullGuardAuth();
	ret->extra.copyFrom( extra );
	return ret;
}


/* virtual */ void CommClientNullGuardFactory::composeGuardNameList( CommMsgBody& msgBody ) const
{
	msgBody.composeString( _COMM_NULL_GUARD_NAME );
}

/* virtual */ /* new */ CommClientGuard* CommClientNullGuardFactory::createGuard( const char* guardName, CommClientGuardAuth* auth_)
{
	_CommClientNullGuardAuth* auth = (_CommClientNullGuardAuth*)auth_;
	PASSERT3( strcmp( guardName, _COMM_NULL_GUARD_NAME ) == 0 );
	return new CommClientNullGuard(auth->extra);
}

