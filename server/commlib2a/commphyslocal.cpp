#include "ppinclude.h"
#include "ppstring.h"
#include "pplogfile.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "commrtable.h"

#include "commproto.h"

/* static */ void CommCompositeProtocolHandler::parseSocketAddr( PString& retAddr, const char* address )
{
	PASSERT3( PString::startsWith( address, _COMMCOMPOSITPREFIX ) );
	const char* p = address + _COMMCOMPOSITREFIXLEN;
	retAddr = p;
}

/* static */ void CommCompositeProtocolHandler::composeSocketAddr( PString& retAddr, const char* address )
{
	retAddr.append( _COMMCOMPOSITPREFIX ).append( address );
}
