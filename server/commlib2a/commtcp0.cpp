#include "ppinclude.h"
#include "ppstring.h"
#include "pplogfile.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "commrtable.h"

#include "commproto.h"

/* static */ void CommTcp0ProtocolHandler::parseSocketAddr( PString& retAddr, int& port, const char* address )
{
	PASSERT3( PString::startsWith( address, _COMMTCP0PREFIX ) );
	const char* p = address + _COMMTCP0PREFIXLEN;
	const char* col = strchr( p, ':' );
	if( col )
	{
		retAddr.assign( p, col );
		port = atoi( col + 1 );
	}
	else
		retAddr = p;
}

/* static */ void CommTcp0ProtocolHandler::composeSocketAddr( PString& retAddr, const char* address, int port )
{
	retAddr.append( _COMMTCP0PREFIX ).append( address ).append( ':' ).appendInt( port );
}

#ifdef UDT_DEVELOPMENT
/* static */ void CommUdt0ProtocolHandler::parseSocketAddr( PString& retAddr, int& port, const char* address )
{
	PASSERT3( PString::startsWith( address, _COMMUDT0PREFIX ) );
	const char* p = address + _COMMUDT0PREFIXLEN;
	const char* col = strchr( p, ':' );
	if( col )
	{
		retAddr.assign( p, col );
		port = atoi( col + 1 );
	}
	else
		retAddr = p;
}

/* static */ void CommUdt0ProtocolHandler::composeSocketAddr( PString& retAddr, const char* address, int port )
{
	retAddr.append( _COMMUDT0PREFIX ).append( address ).append( ':' ).appendInt( port );
}
/* static */ void CommUdtSSL0ProtocolHandler::parseSocketAddr( PString& retAddr, int& port, const char* address )
{
	PASSERT3( PString::startsWith( address, _COMMUDTSSL0PREFIX ) );
	const char* p = address + _COMMUDTSSL0PREFIXLEN;
	const char* col = strchr( p, ':' );
	if( col )
	{
		retAddr.assign( p, col );
		port = atoi( col + 1 );
	}
	else
		retAddr = p;
}

/* static */ void CommUdtSSL0ProtocolHandler::composeSocketAddr( PString& retAddr, const char* address, int port )
{
	retAddr.append( _COMMUDTSSL0PREFIX ).append( address ).append( ':' ).appendInt( port );
}

#endif