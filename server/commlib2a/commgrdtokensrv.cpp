#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"

#include "commmsgbody.h"
#include "commqueue.h"
#include "commsrv.h"
#include "commgrdtoken.h"


CommServerTokenGuard::CommServerTokenGuard()
: CommServerGuard( _COMM_TOKEN_GUARD_NAME )
{
	peerVersion = 0;
}

/* virtual */ void CommServerTokenGuard::processFirstRequest( CommMsgParser& parser )
{
	const char * user;

	parser.parseINT32(peerVersion);
	if ( peerVersion > _version  )
	{
		PTRACE3("CommServerTokenGuard::processFirstRequest peer version %d differs from current version %d", peerVersion, _version);
	}

	parser.parseString( user );
	parser.parsePBlock(tokenReceived);

	CommMsgBody request;
	CommMsgBody clientExtra;
	if( !parser.parseEnded() )//--- temporary; for compatibility
		parser.parseMsgBody( clientExtra );
	if (peerVersion >= 2)
	{
		encr.processFirstRequest(parser);
	}
	request.composeINT32(_version);
	conn->getUserAuth( _COMM_TOKEN_GUARD_NAME, user, clientExtra, request );
}

/* virtual */ void CommServerTokenGuard::processAddRequest( CommMsgParser& parser )
{
	PASSERT3( 0 );
}

/* virtual */ void CommServerTokenGuard::processOutBlock( CommMsgBody& dst, CommMsgBody& src )
{
	dst._composeVarBlock( src._readPtr(), src._size() );
}

/* virtual */ void CommServerTokenGuard::processInBlock( CommMsgBody& dst, CommMsgParser& src )
{
	const BYTE* p;
	size_t sz;
	src._parseVarBlock( p, sz );
	memcpy( dst._alloc( sz ), p, sz );
}

/* virtual */ bool CommServerTokenGuard::gotUserAuth( CommMsgParser* parser, UINT16 errCode, const char* errMsg, const CommMsgBody* errMsgBody )
{
	if( parser && !errCode)
	{
		const BYTE * token_;
		size_t size;
		INT32 ver;
		parser->parseINT32(ver);
		if ( ver != _version  )
		{
			PTRACE3("CommServerTokenGuard::gotUserAuth peer version %d differs from current version %d", ver, _version);
		}

		parser->_parseVarBlock(token_,size);
		if( size == tokenReceived.size() && !memcmp(tokenReceived.ptr(),token_,size))
		{
			CommMsgBody body;
			if ( peerVersion >= 2)
				encr.composeResponse(body);
			_postGuardRequestGranted( body,errMsgBody );
			return true;
		}
		if (!parser->parseEnded())
		{
			parser->_parseVarBlock(token_,size);
			if( size == tokenReceived.size() && !memcmp(tokenReceived.ptr(),token_,size))
			{
				CommMsgBody body;
				if ( peerVersion >= 2)
					encr.composeResponse(body);
				_postGuardRequestGranted( body,errMsgBody );
				return true;
			}
		}
	}
	_postGuardError( errCode ? errCode : _COMM_ERROR_AUTHFAILED, errMsg ? errMsg : "Authentication failed: no such user or invalid password", errMsgBody );
	return false;
}

/* virtual */ bool CommServerTokenGuard::checkUserAuth( CommMsgParser* parser ) const
{
	return true;
}

void CommServerTokenGuard::encryptMsg(const CommMsgBody& src, PBlock& dst) const
{
	if (encr.isInit())
		encr.encryptData(src,dst);
	else
		CommServerGuard::encryptMsg(src,dst);
}
void CommServerTokenGuard::decryptMsg(const BYTE * block, size_t len, CommMsgBody& dst) const
{
	if (encr.isInit())
		encr.decryptData(block,len,dst);
	else
		CommServerGuard::decryptMsg(block,len,dst);
}

//****************************************************************************

/* virtual */ /* new */ CommServerGuard* CommServerTokenGuardFactory::createGuard( const vector< const char* >& guardNames )
{
	int n = guardNames.size();
	for( int i=0; i < n ; ++i )
	{
		if( strcmp( guardNames[ i ], _COMM_TOKEN_GUARD_NAME ) == 0 )
			return new CommServerTokenGuard();
	}
	return 0;
}
