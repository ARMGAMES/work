#include "ppinclude.h"
#include "ppstring.h"
#include "pplogfile.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "commrtable.h"

#include "commproto.h"

/* static */ void CommSslPProtocolHandler::parseSocketAddr( PString& retAddr, int& port, const char* address )
	{
	PASSERT3( PString::startsWith( address, _COMMSSLPPREFIX ) );
	const char* p = address + _COMMSSLPPREFIXLEN;
	const char* col = strchr( p, ':' );
	if( col )
		{
		retAddr.assign( p, col );
		port = atoi( col + 1 );
		}
	else
		retAddr = p;
	}

/* static */ void CommSslPProtocolHandler::composeSocketAddr( PString& retAddr, const char* address, int port )
	{
	retAddr.append( _COMMSSLPPREFIX ).append( address ).append( ':' ).appendInt( port );
	}
