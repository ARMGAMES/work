#include "ppinclude.h"
#include "ppstring.h"
#include "pplogfile.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "commrtable.h"

#include "commproto.h"

/* static */ void CommSsl0ProtocolHandler::parseSocketAddr( PString& retAddr, int& port, const char* address )
{
	PASSERT3( PString::startsWith( address, _COMMSSL0PREFIX ) );
	const char* p = address + _COMMSSL0PREFIXLEN;
	const char* col = strchr( p, ':' );
	if( col )
	{
		retAddr.assign( p, col );
		port = atoi( col + 1 );
	}
	else
		retAddr = p;
}

/* static */ void CommSsl0ProtocolHandler::composeSocketAddr( PString& retAddr, const char* address, int port )
{
	retAddr.append( _COMMSSL0PREFIX ).append( address ).append( ':' ).appendInt( port );
}
