#include "ppinclude.h"
#include "ppstring.h"
#include "pperror.h"
#include "pplogfile.h"
#include "commurl.h"

/* static */ bool CommLine::read( PString& ret, const BYTE*& p, size_t sz )
{
	const BYTE* pEnd = p + sz;
	for(; p < pEnd ; ++p )
	{
		if( *p == '\n' )
		{
			++p;
			int len = ret.length();
			if( len && ret[ len - 1 ] == '\r' )
				ret.cut( len - 1 );
			return true;
		}
		ret.append( *p );
	}
	return false;
}

/* static */ bool CommLine::read( PString& ret, const char*& p, size_t sz )
{
	const char * pEnd = p + sz;
	for(; p < pEnd ; ++p )
	{
		if( *p == '\n' )
		{
			++p;
			int len = ret.length();
			if( len && ret[ len - 1 ] == '\r' )
				ret.cut( len - 1 );
			return true;
		}
		ret.append( *p );
	}
	return false;
}

/* static */ bool CommLine::read( PString& ret, const char*& p )
{
	for(; *p != 0 ; ++p )
	{
		if( *p == '\n' )
		{
			++p;
			int len = ret.length();
			if( len && ret[ len - 1 ] == '\r' )
				ret.cut( len - 1 );
			return true;
		}
		ret.append( *p );
	}
	return false;
}

//****************************************************************************

CommSocketLineReader::CommSocketLineReader( PIPSocket& sock_, int timeout_ )
{
	sock = &sock_;
	offset = 0;
	sz = 0;
	timeout = timeout_;
}

void CommSocketLineReader::readLine( PString& ret )
{
	PString partialLine;
	for(;;)
	{
		if( sz )
		{
			const BYTE* p = buf + offset;
			bool Ok = CommLine::read( partialLine, p, sz );
			if( Ok )
			{
				sz = buf + offset + sz - p;
				offset = p - buf;
				ret = partialLine;
				partialLine = "";
				return;
			}
			else
			{
				PASSERT3( p == buf + offset + sz );
				offset = 0;
				sz = 0;
			}
		}

		PASSERT3( sz == 0 );
		size_t rd;
		if ( timeout )
		{
			rd = sock->recv( buf, sizeof(buf), timeout );
			if (!rd)
				throw PSystemError ( "Socket time out", 0);
		}
		else
			rd = sock->recv( buf, sizeof(buf));

		offset = 0;
		sz = rd;
	}
}

//****************************************************************************

static int _fromHex( char c )
{
	if( c >= '0' && c <= '9' )
		return c - '0';
	else if( c >= 'a' && c <= 'f' )
		return c - 'a' + 10;
	else if( c >= 'A' && c <= 'F' )
		return c - 'A' + 10;
	throw PInternalError( "CommCgi URL: Unexpected symbol after '%' sign" );
}

/* static */ void CommUrl::urlEncode( PString& ret, const char* s, const char* sEnd )
{
	if( sEnd == 0 )
		sEnd = s + strlen( s );
	for(; s < sEnd ; ++s )
	{
		unsigned char c = (unsigned char)*s;
		if( c == ' ' )
			ret.append( '+' );
		else if( ( c >= 'a' && c <= 'z' ) || ( c >= 'A' && c <= 'Z' ) || ( c >= '0' && c <= '9' )
			|| c == '-' || c == '.' || c == '_' || c == '~' )	// RFC 3986, section 2.3
			ret.append( c );
		else
		{
			char buf[ 8 ];
			buf[ 0 ] = '%';
			sprintf( buf + 1, "%02X", (int)c );
			ret += buf;
		}
	}
}

/* static */ void CommUrl::urlDecode( PString& ret, const char* s, const char* sEnd )
{
	if( sEnd == 0 )
		sEnd = s + strlen( s );
	for(; s < sEnd ;)
	{
		if( *s == '+' )
		{
			ret.append( ' ' );
			++s;
		}
		else if( *s == '%' )
		{
			char c1 = *++s;
			char c2 = *++s;
			ret.append( (char)( ( _fromHex( c1 ) << 4 ) | _fromHex( c2 ) ) );
			++s;
		}
		else
		{
			ret.append( *s );
			++s;
		}
	}
}

/* static */ void CommUrl::htmlEncode( PString& ret, const char* s )
{
	for(;; ++s )
		switch( *s )
	{
		case 0:
			return;
		case '&':
			ret.append( "&amp;" );
			break;
		case '<':
			ret.append( "&lt;" );
			break;
		case '>':
			ret.append( "&gt;" );
			break;
			//case '\'':
			//	ret += "&apos;";
			//	break;
		case '\"':
			ret.append( "&quot;" );
			break;
		default:
			ret.append( *s );
			break;
	}
}
