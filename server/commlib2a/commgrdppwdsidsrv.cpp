#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"

#include "commmsgbody.h"
#include "commqueue.h"
#include "commsrv.h"
#include "commgrdppwdsid.h"
#include "commssl.h"

CommServerPlainTextPasswordSidGuard::CommServerPlainTextPasswordSidGuard( bool emulatePPWD_ )
: CommServerGuard( _COMM_PPWDSID_GUARD_NAME )
{
	emulatePPWD = emulatePPWD_;
	authState = AuthStateNotYet;
}

/* virtual */ void CommServerPlainTextPasswordSidGuard::processFirstRequest( CommMsgParser& parser )
{
	const char* pwd;
	const char* sid;
	parser.parseStringP( user ).parseString( pwd );
	pwdReceived = pwd;
	if( !emulatePPWD )
	{
		parser.parseString( sid );
		sidReceived = sid;
	}
	else
		sidReceived = "";

	CommMsgBody request;
	if( !parser.parseEnded() )//--- temporary; for compatibility
		parser.parseMsgBody( clientExtra );
	conn->getUserAuth( _COMM_PPWDSID_GUARD_NAME, user, clientExtra, request );
}

/* virtual */ void CommServerPlainTextPasswordSidGuard::processAddRequest( CommMsgParser& parser )
{
	PASSERT3( 0 );
}

/* virtual */ void CommServerPlainTextPasswordSidGuard::processOutBlock( CommMsgBody& dst, CommMsgBody& src )
{
	dst._composeVarBlock( src._readPtr(), src._size() );
}

/* virtual */ void CommServerPlainTextPasswordSidGuard::processInBlock( CommMsgBody& dst, CommMsgParser& src )
{
	const BYTE* p;
	size_t sz;
	src._parseVarBlock( p, sz );
	memcpy( dst._alloc( sz ), p, sz );
}

/* virtual */ bool CommServerPlainTextPasswordSidGuard::gotUserAuth( CommMsgParser* parser, UINT16 errCode, const char* errMsg, const CommMsgBody* errMsgBody )
{
	UINT16 internalErrCode = _COMM_ERROR_AUTHFAILED;
	const char * msg = "Authentication failed: no such user or invalid password"; 
	if( parser )
	{
		const char* encrPassword;
		const char* sid;
		bool ok = false;
		parser->parseString( encrPassword );
		parser->parseString( sid );
		ok = CommSSLPasswordHash::checkHash(encrPassword,pwdReceived);
		if( ok  )
		{
			if( sidReceived.equals( sid ) )
			{
				authState = AuthStateOk;
				CommMsgBody body;
				_postGuardRequestGranted( body,errMsgBody );
				return true;
			}
			else
			{
				if( *sidReceived == 0 )
				{
					authState = AuthStatePwdOkNoSid;
					if (!emulatePPWD)
					{
						if (!parser->parseEnded())
						{
							parser->parseUINT16( internalErrCode );
						}
						else
						{
							internalErrCode = _COMM_ERROR_AUTHFAILED_PARTIAL;
						}
					}
				}
				else if( *sid == 0 )
				{
					authState = AuthStatePwdOkNoSidInDatabase;
					if( !emulatePPWD )
					{
						internalErrCode = _COMM_ERROR_AUTHFAILED_EXTRA;
						msg = "SID supplied but not required";
					}
				}
				else
				{
					authState = AuthStatePwdOkSidFailed;
					if( !emulatePPWD )
						internalErrCode = _COMM_ERROR_AUTHFAILED_SEVERE;
				}
			}
		}
		else
		{
			authState = AuthStatePwdFailed;
			internalErrCode = _COMM_ERROR_AUTHFAILED;
		}
	}

	_postGuardError( (errCode ? errCode : internalErrCode), (errMsg ? errMsg : msg), errMsgBody );
	return false;
}
/* virtual */ void CommServerPlainTextPasswordSidGuard::delegateUserAuth(  UINT16 errCode, const char* errMsg, const CommMsgBody* errMsgBody )
{
	if (errCode)
	{
		_postGuardError(  errCode , errMsg , errMsgBody );
	}
	else
	{
		authState = AuthStateOk;
		CommMsgBody body;
		_postGuardRequestGranted( body,errMsgBody );
	}
}


/* virtual */ bool CommServerPlainTextPasswordSidGuard::checkUserAuth( CommMsgParser* parser ) const
{
	if( parser )
	{
		const char* encrPassword;
		const char* sid;
		bool ok = false;
		parser->parseString( encrPassword );
		parser->parseString( sid );
		ok = CommSSLPasswordHash::checkHash(encrPassword,pwdReceived);
		if( ok )
		{
			if( sidReceived.equals( sid ) )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	return false;
}


//****************************************************************************

/* virtual */ /* new */ CommServerGuard* CommServerPlainTextPasswordSidGuardFactory::createGuard( const vector< const char* >& guardNames )
{
	int n = guardNames.size();
	for( int i=0; i < n ; ++i )
	{
		if( strcmp( guardNames[ i ], _COMM_PPWDSID_GUARD_NAME ) == 0 )
			return new CommServerPlainTextPasswordSidGuard( false );
		else if( strcmp( guardNames[ i ], _COMM_PPWD_GUARD_NAME ) == 0 )
			return new CommServerPlainTextPasswordSidGuard( true );
	}
	return 0;
}
