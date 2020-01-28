#include "ppinclude.h"
#include "ppstring.h"
#include "pplogfile.h"
#include "commmsgbody.h"
#include "commconn.h"

/* static */ void _CommConnection::_wrapUserMsg( CommMsgBody& body, UINT32 msgId )
{
	BYTE* p = body._createPrefix( 4 + 1 );
	CommMsgBody::writeUINT32( p, msgId );
	p[4] = CommMsgBody::_NoFormatChar;
}

/* static */ void _CommConnection::_unwrapUserMsg( UINT32& msgId, CommMsgBody& body )
{
	const BYTE* p = body._readPtr();
	if( body._size() < 4 + 1 )
		throw PInternalError( "USERMSG: insufficient space for prefix" );
	msgId = CommMsgBody::readUINT32( p );
	p += 4;
	if( *p != CommMsgBody::_NoFormatChar )
		throw PInternalError( "USERMSG: format string is too long" );
	++p;

	body._skipPrefix( p - body._readPtr() );
}
