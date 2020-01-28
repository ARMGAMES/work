#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"

#include "commmsgbody.h"
#include "commqueue.h"
#include "commsrv.h"
#include "commgrdppwd.h"
#include "commssl.h"

CommServerPlainTextPasswordGuard::CommServerPlainTextPasswordGuard()
: CommServerGuard( _COMM_PPWD_GUARD_NAME )
{
}

/* virtual */ void CommServerPlainTextPasswordGuard::processFirstRequest( CommMsgParser& parser )
{
	parser.parseStringP( user ).parseStringP( pwdReceived );

	CommMsgBody request;
	if( !parser.parseEnded() )//--- temporary; for compatibility
		parser.parseMsgBody( clientExtra );
	conn->getUserAuth( _COMM_PPWD_GUARD_NAME, user, clientExtra, request );
}

/* virtual */ void CommServerPlainTextPasswordGuard::processAddRequest( CommMsgParser& parser )
{
	PASSERT3( 0 );
}

/* virtual */ void CommServerPlainTextPasswordGuard::processOutBlock( CommMsgBody& dst, CommMsgBody& src )
{
	dst._composeVarBlock( src._readPtr(), src._size() );
}

/* virtual */ void CommServerPlainTextPasswordGuard::processInBlock( CommMsgBody& dst, CommMsgParser& src )
{
	const BYTE* p;
	size_t sz;
	src._parseVarBlock( p, sz );
	memcpy( dst._alloc( sz ), p, sz );
}

/* virtual */ bool CommServerPlainTextPasswordGuard::gotUserAuth( CommMsgParser* parser, UINT16 errCode, const char* errMsg, const CommMsgBody* errMsgBody )
{
	if( parser )
	{
		const char* encrPassword;
		bool ok = false;
		parser->parseString( encrPassword );
		ok = CommSSLPasswordHash::checkHash(encrPassword,pwdReceived);
		if( ok )
		{
			CommMsgBody body;
			_postGuardRequestGranted( body,errMsgBody );
			return true;
		}
	}

	_postGuardError( errCode ? errCode : _COMM_ERROR_AUTHFAILED, errMsg ? errMsg : "Authentication failed: no such user or invalid password", errMsgBody );
	return false;
}

/* virtual */ void CommServerPlainTextPasswordGuard::delegateUserAuth(  UINT16 errCode, const char* errMsg, const CommMsgBody* errMsgBody )
{
	if (errCode)
	{
		_postGuardError(  errCode , errMsg , errMsgBody );
	}
	else
	{
		CommMsgBody body;
		_postGuardRequestGranted( body, errMsgBody );
	}
}

/* virtual */ bool CommServerPlainTextPasswordGuard::checkUserAuth( CommMsgParser* parser ) const
{
	if( parser )
	{
		const char* encrPassword;
		bool ok = false;
		parser->parseString( encrPassword );
		ok = CommSSLPasswordHash::checkHash(encrPassword,pwdReceived);

		if( ok )
		{
			return true;
		}
	}
	return false;
}


//****************************************************************************

/* virtual */ /* new */ CommServerGuard* CommServerPlainTextPasswordGuardFactory::createGuard( const vector< const char* >& guardNames )
{
	int n = guardNames.size();
	for( int i=0; i < n ; ++i )
	{
		if( strcmp( guardNames[ i ], _COMM_PPWD_GUARD_NAME ) == 0 )
			return new CommServerPlainTextPasswordGuard();
	}
	return 0;
}
