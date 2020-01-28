#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"

#include "commmsgbody.h"
#include "commqueue.h"
#include "commsrv.h"
#include "commgrdnull.h"

CommServerNullGuard::CommServerNullGuard()
: CommServerGuard( _COMM_NULL_GUARD_NAME )
{
}

/* virtual */ void CommServerNullGuard::processFirstRequest( CommMsgParser& parser )
{
	CommMsgBody clientExtra;
	if( !parser.parseEnded() )
	{
		parser.parseMsgBody( clientExtra );
	}
	CommMsgBody request;
	conn->getUserAuth( _COMM_NULL_GUARD_NAME, "", clientExtra, request );
}

/* virtual */ void CommServerNullGuard::processAddRequest( CommMsgParser& parser )
{
	PASSERT3( 0 );
}

/* virtual */ void CommServerNullGuard::processOutBlock( CommMsgBody& dst, CommMsgBody& src )
{
	dst._composeVarBlock( src._readPtr(), src._size() );
}

/* virtual */ void CommServerNullGuard::processInBlock( CommMsgBody& dst, CommMsgParser& src )
{
	const BYTE* p;
	size_t sz;
	src._parseVarBlock( p, sz );
	memcpy( dst._alloc( sz ), p, sz );
}

/* virtual */ bool CommServerNullGuard::gotUserAuth( CommMsgParser* parser, UINT16 errCode, const char* errMsg, const CommMsgBody* errMsgBody )
{
	delegateUserAuth( errCode, errMsg, errMsgBody);
	return !errCode;
}
/* virtual */ void CommServerNullGuard::delegateUserAuth(  UINT16 errCode, const char* errMsg, const CommMsgBody* errMsgBody )
{
	if (errCode)
	{
		_postGuardError(  errCode , errMsg , errMsgBody );
	}
	else
	{
		CommMsgBody body;
		_postGuardRequestGranted( body,errMsgBody );
	}

}


//****************************************************************************

/* virtual */ /* new */ CommServerGuard* CommServerNullGuardFactory::createGuard( const vector< const char* >& guardNames )
{
	int n = guardNames.size();
	for( int i=0; i < n ; ++i )
	{
		if( strcmp( guardNames[ i ], _COMM_NULL_GUARD_NAME ) == 0 )
			return new CommServerNullGuard();
	}
	return 0;
}

