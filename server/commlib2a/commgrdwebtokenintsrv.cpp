#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"

#include "commmsgbody.h"
#include "commqueue.h"
#include "commsrv.h"
#include "commgrdwebtokenint.h"


CommServerWebTokenIntGuard::CommServerWebTokenIntGuard()
: CommServerGuard( _COMM_WEBTOKENINT_GUARD_NAME )
{
	peerVersion = 0;
}

/* virtual */ void CommServerWebTokenIntGuard::processFirstRequest( CommMsgParser& parser )
{

	parser.parseINT32(peerVersion);
	if ( peerVersion > _version  )
	{
		PTRACE3("CommServerWebTokenIntGuard::processFirstRequest peer version %d differs from current version %d", peerVersion, _version);
	}
	UINT32 userIntId;
	const char * token;

	parser.parseUINT32( userIntId).parseString(token);

	CommMsgBody request;
	CommMsgBody clientExtra;
	if( !parser.parseEnded() )//--- temporary; for compatibility
		parser.parseMsgBody( clientExtra );
	if (peerVersion >= 2)
	{
		encr.processFirstRequest(parser);
	}

	request.composeINT32(_version).composeString(token);
	conn->getUserAuth( _COMM_WEBTOKENINT_GUARD_NAME, userIntId, clientExtra, request );
}

/* virtual */ void CommServerWebTokenIntGuard::processAddRequest( CommMsgParser& parser )
{
	PASSERT3( 0 );
}

/* virtual */ void CommServerWebTokenIntGuard::processOutBlock( CommMsgBody& dst, CommMsgBody& src )
{
	dst._composeVarBlock( src._readPtr(), src._size() );
}

/* virtual */ void CommServerWebTokenIntGuard::processInBlock( CommMsgBody& dst, CommMsgParser& src )
{
	const BYTE* p;
	size_t sz;
	src._parseVarBlock( p, sz );
	memcpy( dst._alloc( sz ), p, sz );
}

/* virtual */ bool CommServerWebTokenIntGuard::gotUserAuth( CommMsgParser* parser, UINT16 errCode, const char* errMsg, const CommMsgBody* errMsgBody )
{
	if( parser && !errCode)
	{
		INT32 ver;
		parser->parseINT32(ver);
		if ( ver != _version  )
		{
			PTRACE3("CommServerWebTokenIntGuard::gotUserAuth peer version %d differs from current version %d", ver, _version);
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

/* virtual */ bool CommServerWebTokenIntGuard::checkUserAuth( CommMsgParser* parser ) const
{
	return true;
}

void CommServerWebTokenIntGuard::encryptMsg(const CommMsgBody& src, PBlock& dst) const
{
	if (encr.isInit())
		encr.encryptData(src,dst);
	else
		CommServerGuard::encryptMsg(src,dst);
}
void CommServerWebTokenIntGuard::decryptMsg(const BYTE * block, size_t len, CommMsgBody& dst) const
{
	if (encr.isInit())
		encr.decryptData(block,len,dst);
	else
		CommServerGuard::decryptMsg(block,len,dst);
}


//****************************************************************************

/* virtual */ /* new */ CommServerGuard* CommServerWebTokenIntGuardFactory::createGuard( const vector< const char* >& guardNames )
{
	int n = guardNames.size();
	for( int i=0; i < n ; ++i )
	{
		if( strcmp( guardNames[ i ], _COMM_WEBTOKENINT_GUARD_NAME ) == 0 )
			return new CommServerWebTokenIntGuard();
	}
	return 0;
}
