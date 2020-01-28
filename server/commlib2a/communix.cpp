#include "ppinclude.h"
#include "ppstring.h"
#include "pplogfile.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "commrtable.h"

#include "commproto.h"
#ifdef PPOSIX
/* static */ void CommUnixProtocolHandler::parseSocketAddr( PString& retAddr, const char* address )
{
	PASSERT3( PString::startsWith( address, _COMMUNIXPREFIX ) );
	const char* p = address + _COMMUNIXPREFIXLEN;
	retAddr = p;
}

/* static */ void CommUnixProtocolHandler::composeSocketAddr( PString& retAddr, const char* address )
{
	retAddr.append( _COMMUNIXPREFIX ).append( address );
}
#endif