#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"

#include "commmsgbody.h"
#include "commqueue.h"
#include "commsrv.h"
#include "commgrdwebtoken.h"


CommServerWebTokenGuard::CommServerWebTokenGuard()
: CommServerGuard( _COMM_WEBTOKEN_GUARD_NAME )
{
	peerVersion = 0;
}

/* virtual */ void CommServerWebTokenGuard::processFirstRequest( CommMsgParser& parser )
{

	parser.parseINT32(peerVersion);
	if ( peerVersion > _version  )
	{
		PTRACE3("CommServerWebTokenGuard::processFirstRequest peer version %d differs from current version %d", peerVersion, _version);
	}
	const char * user;
	const char * token;

	parser.parseString( user).parseString(token);

	CommMsgBody request;
	CommMsgBody clientExtra;
	if( !parser.parseEnded() )//--- temporary; for compatibility
		parser.parseMsgBody( clientExtra );
	if (peerVersion >= 2)
	{
		encr.processFirstRequest(parser);
	}
	request.composeINT32(_version).composeString(token);
	conn->getUserAuth( _COMM_WEBTOKEN_GUARD_NAME, user, clientExtra, request );
}

/* virtual */ void CommServerWebTokenGuard::processAddRequest( CommMsgParser& parser )
{
	PASSERT3( 0 );
}

/* virtual */ void CommServerWebTokenGuard::processOutBlock( CommMsgBody& dst, CommMsgBody& src )
{
	dst._composeVarBlock( src._readPtr(), src._size() );
}

/* virtual */ void CommServerWebTokenGuard::processInBlock( CommMsgBody& dst, CommMsgParser& src )
{
	const BYTE* p;
	size_t sz;
	src._parseVarBlock( p, sz );
	memcpy( dst._alloc( sz ), p, sz );
}

/* virtual */ bool CommServerWebTokenGuard::gotUserAuth( CommMsgParser* parser, UINT16 errCode, const char* errMsg, const CommMsgBody* errMsgBody )
{
	if( parser && !errCode)
	{
		INT32 ver;
		parser->parseINT32(ver);
		if ( ver != _version  )
		{
			PTRACE3("CommServerWebTokenGuard::gotUserAuth peer version %d differs from current version %d", ver, _version);
		}
		CommMsgBody body;
		if ( peerVersion >= 2)
			encr.composeResponse(body);
		_postGuardRequestGranted( body, errMsgBody );
		return true;
	}
	_postGuardError( errCode ? errCode : _COMM_ERROR_AUTHFAILED, errMsg ? errMsg : "Authentication failed: no such user or invalid password", errMsgBody );
	return false;
}

/* virtual */ bool CommServerWebTokenGuard::checkUserAuth( CommMsgParser* parser ) const
{
	return true;
}

void CommServerWebTokenGuard::encryptMsg(const CommMsgBody& src, PBlock& dst) const
{
	if (encr.isInit())
		encr.encryptData(src,dst);
	else
		CommServerGuard::encryptMsg(src,dst);
}
void CommServerWebTokenGuard::decryptMsg(const BYTE * block, size_t len, CommMsgBody& dst) const
{
	if (encr.isInit())
		encr.decryptData(block,len,dst);
	else
		CommServerGuard::decryptMsg(block,len,dst);
}


//****************************************************************************

/* virtual */ /* new */ CommServerGuard* CommServerWebTokenGuardFactory::createGuard( const vector< const char* >& guardNames )
{
	int n = guardNames.size();
	for( int i=0; i < n ; ++i )
	{
		if( strcmp( guardNames[ i ], _COMM_WEBTOKEN_GUARD_NAME ) == 0 )
			return new CommServerWebTokenGuard();
	}
	return 0;
}
