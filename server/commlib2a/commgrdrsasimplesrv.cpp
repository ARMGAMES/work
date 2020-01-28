#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"

#include "commmsgbody.h"
#include "commqueue.h"
#include "commsrv.h"
#include "commgrdrsasimple.h"

CommServerRsaSimpleGuard::CommServerRsaSimpleGuard(  )
: CommServerGuard( _COMM_RSA_SIMPLE_GUARD_NAME )
{
	firstConnection = true;
	peerVersion = 0;
}

CommServerRsaSimpleGuard::~CommServerRsaSimpleGuard(  )
{
}

/* virtual */ void CommServerRsaSimpleGuard::processOutBlock( CommMsgBody& dst, CommMsgBody& src )
{
	dst._composeVarBlock( src._readPtr(), src._size() );
}

/* virtual */ void CommServerRsaSimpleGuard::processInBlock( CommMsgBody& dst, CommMsgParser& src )
{
	const BYTE* p;
	size_t sz;
	src._parseVarBlock( p, sz );
	memcpy( dst._alloc( sz ), p, sz );
}


/* virtual */ void CommServerRsaSimpleGuard::processFirstRequest( CommMsgParser& parser )
{
	parser.parseINT32(peerVersion)
		.parseStringP(user)
		.parseBOOL(firstConnection);
	if ( peerVersion > _version )
	{
		PTRACE3("CommServerRsaSimpleGuard::processFirstRequest peer version %d greater than current %d ", peerVersion, _version);
	}

	if (!firstConnection)
	{
		parser.parsePBlock(securityToken);
	}
	else
	{
		parser.parseStringP(pwdReceived);
	}

	if( !parser.parseEnded() )//--- temporary; for compatibility
		parser.parseMsgBody( clientExtra );

	CommMsgBody request;
	request.composeINT32(_version)
		.composeBOOL(firstConnection);

	conn->getUserAuth( _COMM_RSA_SIMPLE_GUARD_NAME, user, clientExtra, request );
}

/* virtual */ void CommServerRsaSimpleGuard::processAddRequest( CommMsgParser& parser )
{
	PASSERT3( 0 );
}


/* virtual */ bool CommServerRsaSimpleGuard::gotUserAuth( CommMsgParser* parser, UINT16 errCode, const char* errMsg, const CommMsgBody* errMsgBody )
{

	if (!parser || errCode)
	{
		_postGuardError( errCode ? errCode : _COMM_ERROR_AUTHFAILED, errMsg ? errMsg : "Authentication failed: no such user or invalid password", errMsgBody );
		return false;
	}

	INT32 ver;
	parser->parseINT32(ver);
	if ( ver != _version  )
	{
		PTRACE3("CommServerRsaSimpleGuard::gotUserAuth peer version %d differs from current version %d", ver, _version);
	}
	if (firstConnection)
	{
		const char* encrPassword;
		bool ok = false;
		parser->parseString( encrPassword );
		ok = CommSSLPasswordHash::checkHash(encrPassword,pwdReceived);
		if( !ok  )
		{
			_postGuardError(_COMM_ERROR_AUTHFAILED, "Authentication failed: no such user or invalid password", errMsgBody );
			return false;
		}
	}
	else
	{
		const BYTE * token_;
		size_t size;
		parser->_parseVarBlock(token_,size);
		if( size != securityToken.size() || memcmp(securityToken.ptr(),token_,size))
		{
			_postGuardError( _COMM_ERROR_AUTHFAILED, "Session key does not match", errMsgBody );
			return false;
		}
	}
	CommMsgBody body;
	_postGuardRequestGranted( body,errMsgBody );
	return true;
}


/* virtual */ bool CommServerRsaSimpleGuard::checkUserAuth( CommMsgParser* parser ) const
{

	if (!parser)
		return false;

	INT32 ver;
	parser->parseINT32(ver);
	if ( ver != _version  )
	{
		PTRACE3("CommServerRsaSimpleGuard::checkUserAuth peer version %d differs from current version %d", ver, _version);
	}
	if (firstConnection)
	{
		const char* encrPassword;
		bool ok = false;
		parser->parseString( encrPassword );
		ok = CommSSLPasswordHash::checkHash(encrPassword,pwdReceived);
		if( !ok  )
		{
			return false;
		}
	}
	else
	{
		const BYTE * token_;
		size_t size;
		parser->_parseVarBlock(token_,size);
		if( size != securityToken.size() || memcmp(securityToken.ptr(),token_,size))
		{
			return false;
		}
	}
	return true;
}

//****************************************************************************

/* virtual */ /* new */ CommServerGuard* CommServerRsaSimpleGuardFactory::createGuard( const vector< const char* >& guardNames )
{
	int n = guardNames.size();
	for( int i=0; i < n ; ++i )
	{
		if( strcmp( guardNames[ i ], _COMM_RSA_SIMPLE_GUARD_NAME ) == 0 )
			return new CommServerRsaSimpleGuard();
	}
	return 0;
}
