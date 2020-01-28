#include "ppinclude.h"
#include "ppstring.h"
#include "ppi18n.h"
#include "ppi18n_utf8.h"
#include "ppi18n_casefolding.h"

/* static */ PUNICHAR_RET PUtf8String::_nextChar( const char*& p, const char* endP )//returns 0 if end of string found (just like when encountered terminating 0), -1 in case of incomplete char, -2 in case of error
{//see http://www.ietf.org/rfc/rfc2279.txt
	PASSERT4( endP == 0 || p <= endP );
	if( p == endP )
		return 0;
	if( *p == 0 )
		return 0;  // end of string, don't advance the pointer p!
	char c = *p++;
	if( ( (UINT32)c & 0x80 ) == 0 )
		return c;
	else if( ( (UINT32)c & 0xE0 ) == 0xC0 )
	{
		if( endP && endP - p < 1 )
		{
			--p;
			return -1;
		}
		char c1 = *p++;
		if( ( (UINT32)c1 & 0xC0 ) != 0x80 )
			return -2;
		UINT32 ret = ( (UINT32)c & 0x1F ) << 6;
		ret |= ( (UINT32)c1 & 0x3F );
		return (PUNICHAR_RET)ret;
	}
	else if( ( (UINT32)c & 0xF0 ) == 0xE0 )
	{
		if( endP && endP - p < 2 )
		{
			--p;
			return -1;
		}
		char c1 = *p++;
		if( ( (UINT32)c1 & 0xC0 ) != 0x80 )
			return -2;
		char c2 = *p++;
		if( ( (UINT32)c2 & 0xC0 ) != 0x80 )
			return -2;
		UINT32 ret = ( (UINT32)c & 0x0F ) << 12;
		ret |= ( (UINT32)c1 & 0x3F ) << 6;
		ret |= ( (UINT32)c2 & 0x3F );
		return (PUNICHAR_RET)ret;
	}
	else if( ( (UINT32)c & 0xF8 ) == 0xF0 )
	{
		if( endP && endP - p < 3 )
		{
			--p;
			return -1;
		}
		char c1 = *p++;
		if( ( (UINT32)c1 & 0xC0 ) != 0x80 )
			return -2;
		char c2 = *p++;
		if( ( (UINT32)c2 & 0xC0 ) != 0x80 )
			return -2;
		char c3 = *p++;
		if( ( (UINT32)c3 & 0xC0 ) != 0x80 )
			return -2;
		UINT32 ret = ( (UINT32)c & 0x07 ) << 18;
		ret |= ( (UINT32)c1 & 0x3F ) << 12;
		ret |= ( (UINT32)c2 & 0x3F ) << 6;
		ret |= ( (UINT32)c3 & 0x3F );
		return (PUNICHAR_RET)ret;
	}
	else if( ( (UINT32)c & 0xFC ) == 0xF8 )
	{
		if( endP && endP - p < 4 )
		{
			--p;
			return -1;
		}
		char c1 = *p++;
		if( ( (UINT32)c1 & 0xC0 ) != 0x80 )
			return -2;
		char c2 = *p++;
		if( ( (UINT32)c2 & 0xC0 ) != 0x80 )
			return -2;
		char c3 = *p++;
		if( ( (UINT32)c3 & 0xC0 ) != 0x80 )
			return -2;
		char c4 = *p++;
		if( ( (UINT32)c4 & 0xC0 ) != 0x80 )
			return -2;
		UINT32 ret = ( (UINT32)c & 0x03 ) << 24;
		ret |= ( (UINT32)c1 & 0x3F ) << 18;
		ret |= ( (UINT32)c2 & 0x3F ) << 12;
		ret |= ( (UINT32)c3 & 0x3F ) << 6;
		ret |= ( (UINT32)c4 & 0x3F );
		return (PUNICHAR_RET)ret;
	}
	else if( ( (UINT32)c & 0xFE ) == 0xFC )
	{
		if( endP && endP - p < 5 )
		{
			--p;
			return -1;
		}
		char c1 = *p++;
		if( ( (UINT32)c1 & 0xC0 ) != 0x80 )
			return -2;
		char c2 = *p++;
		if( ( (UINT32)c2 & 0xC0 ) != 0x80 )
			return -2;
		char c3 = *p++;
		if( ( (UINT32)c3 & 0xC0 ) != 0x80 )
			return -2;
		char c4 = *p++;
		if( ( (UINT32)c4 & 0xC0 ) != 0x80 )
			return -2;
		char c5 = *p++;
		if( ( (UINT32)c5 & 0xC0 ) != 0x80 )
			return -2;
		UINT32 ret = ( (UINT32)c & 0x01 ) << 30;
		ret |= ( (UINT32)c1 & 0x3F ) << 24;
		ret |= ( (UINT32)c2 & 0x3F ) << 18;
		ret |= ( (UINT32)c3 & 0x3F ) << 12;
		ret |= ( (UINT32)c4 & 0x3F ) << 6;
		ret |= ( (UINT32)c5 & 0x3F );
		return (PUNICHAR_RET)ret;
	}
	else
		return -2;
}

/* static */ bool PUtf8String::_genChar( char*& p, const char* endP, PUNICHAR_32 c )//returns false if not enough space in p    // PYR-50664 change PUNICHAR to PUNICHAR_32
{//see http://www.ietf.org/rfc/rfc2279.txt
	PASSERT4( endP - p > 0 );
	if( c <= 0x7F )
	{
		*p++ = (char)c;
	}
	else if( c <= 0x7FF )
	{
		if( endP - p < 2 )
			return false;
		BYTE c1 = (BYTE)( ( ( c >> 6 ) & 0x1F ) | 0xC0 );
		BYTE c2 = (BYTE)( ( c & 0x3F ) | 0x80 );
		*p++ = c1;
		*p++ = c2;
	}
	else if( c <= 0xFFFF )
	{
		if( endP - p < 3 )
			return false;
		BYTE c1 = (BYTE)( ( ( c >> 12 ) & 0x0F ) | 0xE0 );
		BYTE c2 = (BYTE)( ( ( c >> 6 ) & 0x3F ) | 0x80 );
		BYTE c3 = (BYTE)( ( c & 0x3F ) | 0x80 );
		*p++ = c1;
		*p++ = c2;
		*p++ = c3;
	}
	else if( c <= 0x1FFFFF )
	{
		if( endP - p < 4 )
			return false;
		BYTE c1 = (BYTE)( ( ( c >> 18 ) & 0x07 ) | 0xF0 );
		BYTE c2 = (BYTE)( ( ( c >> 12 ) & 0x3F ) | 0x80 );
		BYTE c3 = (BYTE)( ( ( c >> 6 ) & 0x3F ) | 0x80 );
		BYTE c4 = (BYTE)( ( c & 0x3F ) | 0x80 );
		*p++ = c1;
		*p++ = c2;
		*p++ = c3;
		*p++ = c4;
	}
	else if( c <= 0x3FFFFFF )
	{
		if( endP - p < 5 )
			return false;
		BYTE c1 = (BYTE)( ( ( c >> 24 ) & 0x03 ) | 0xF8 );
		BYTE c2 = (BYTE)( ( ( c >> 18 ) & 0x3F ) | 0x80 );
		BYTE c3 = (BYTE)( ( ( c >> 12 ) & 0x3F ) | 0x80 );
		BYTE c4 = (BYTE)( ( ( c >> 6 ) & 0x3F ) | 0x80 );
		BYTE c5 = (BYTE)( ( c & 0x3F ) | 0x80 );
		*p++ = c1;
		*p++ = c2;
		*p++ = c3;
		*p++ = c4;
		*p++ = c5;
	}
	else if( c <= 0x7FFFFFFF )
	{
		if( endP - p < 6 )
			return false;
		BYTE c1 = (BYTE)( ( ( c >> 30 ) & 0x01 ) | 0xFC );
		BYTE c2 = (BYTE)( ( ( c >> 24 ) & 0x3F ) | 0x80 );
		BYTE c3 = (BYTE)( ( ( c >> 18 ) & 0x3F ) | 0x80 );
		BYTE c4 = (BYTE)( ( ( c >> 12 ) & 0x3F ) | 0x80 );
		BYTE c5 = (BYTE)( ( ( c >> 6 ) & 0x3F ) | 0x80 );
		BYTE c6 = (BYTE)( ( c & 0x3F ) | 0x80 );
		*p++ = c1;
		*p++ = c2;
		*p++ = c3;
		*p++ = c4;
		*p++ = c5;
		*p++ = c6;
	}
	else
	{
		PASSERT4( 0 );
	}
	return true;
}

/* static */ int PUtf8String::lengthOfChar(PUNICHAR_32 c)    // PYR-50664 change PUNICHAR to PUNICHAR_32
{
	if(c <= 0x7F)
		return 1;
	else if(c <= 0x7FF)
		return 2;
	else if(c <= 0xFFFF)
		return 3;
	else if(c <= 0x1FFFFF)
		return 4;
	else if(c <= 0x3FFFFFF)
		return 5;
	else if(c <= 0x7FFFFFFF)
		return 6;
	else
	{
		PASSERT4(0);
	}
	return 0;
}

/* static */ bool PUtf8String::isValidString( const char* s, const char* end )
{
	for(;;)
	{
		PUNICHAR_RET c = _nextChar( s, end );
		if( c < 0 )
			return false;
		if( c == 0 )
			return true;
		if( end && s == end )
			return true;
	}
}

static void _throwUtf8FormatError()
{
	throw PI18NError( "UTF-8 format error" );
}

/* static */ int PUtf8String::charLength( const char* s )
{
	int ret = 0;
	for(;;)
	{
		PUNICHAR_RET c = _nextChar( s );
		PASSERT4( c >= 0 );
		if( c <= 0 )
			return ret;
		ret++;
	}
}

/* static */ const char* PUtf8String::charPtr( const char* s, PSTR_SIZE_T charNum )
{
	PASSERT4( charNum >= 0 );
	for( PSTR_SIZE_T i=0; i < charNum ; ++i )
	{
		PUNICHAR_RET c = _nextChar( s );
		PASSERT4( c > 0 );
		if(c <= 0)
			break;
	}
	return s;
}

PUtf8String& PUtf8String::_assign( const char* begin, const char* end )
{
	if( _Parent::length() )
		_Parent::cut( 0 );
	return _append( begin, end );
}

/* static */ void PUtf8String::_staticAppend( PString& s, const char* begin, const char* end )
{
	if( end == NULL )
		s.append( begin );
	else
		s.append( begin, end );		
}

PUtf8String& PUtf8String::_append( const char* begin, const char* end )
{
	if( end && end - begin <= 0 )
		return *this;
	else if( !end && *begin == 0 )
		return *this;

	bool ok = isValidString( begin, end );
	PASSERT4( ok );

	_staticAppend( *this, begin, end );
	return *this;
}

PUtf8String& PUtf8String::append( PUNICHAR_32 c )    // PYR-50664 change PUNICHAR to PUNICHAR_32
{
	PASSERT4( c );
	staticAppend( *this, c );
	return *this;
}

/* static */ void PUtf8String::staticAppend( PString& s, PUNICHAR_32 c )    // PYR-50664 change PUNICHAR to PUNICHAR_32
{
	PASSERT4( c );
	char tmp[ 8 ];//in fact, 6 should be enough for any char
	char* p = tmp;
	bool ok = _genChar( p, p + sizeof( tmp ), c );
    UNUSED(ok);
	PASSERT5( ok );
	s.append( tmp, p );
}

/* static */ PUNICHAR_32 PUtf8String::charAt( const char* s, PSTR_SIZE_T idx )    // PYR-50664 change PUNICHAR to PUNICHAR_32
{
	const char* p = charPtr( s, idx );
	PUNICHAR_RET c = _nextChar( p );
	PASSERT4( c > 0 );
	return c > 0 ? (PUNICHAR_32)c : 0;  // return 0: might reach end of string or encounter any utf8 decoding error
}

static inline bool _isSpace( PUNICHAR_32 c )    // PYR-50664 change PUNICHAR to PUNICHAR_32
{
	return c == ' ' || c == '\t';
}

/* static */ const char* PUtf8String::charLtrim( const char* s )
{
	for(;;)
	{
		const char* s0 = s;
		PUNICHAR_RET c = _nextChar( s );
		PASSERT4( c >= 0 );
		if( !_isSpace( c ) )
			return s0;
	}
}

/* static */ const char* PUtf8String::charRtrim( const char* s )
{
	const char* firstSp = NULL;
	for(;;)
	{
		const char* s0 = s;
		PUNICHAR_RET c = _nextChar( s );
		PASSERT4( c >= 0 );
		if( c <= 0 )
			return firstSp ? firstSp : s0;
		bool sp = _isSpace( c );
		if( sp && !firstSp )
			firstSp = s0;
		else if( !sp && firstSp )
			firstSp = NULL;
		//else - ( !sp && !firstSp ) or ( sp && firstSp ) - do nothing
	}
}

void PUtf8String::cutAtChar( PSTR_SIZE_T len )
{
	PASSERT4( len >= 0 );
	const char* p0 = c_str();
	const char* pp = charPtr( p0, len );
	_Parent::cut( pp - p0 );
}

/* static */ void PUtf8String::i18nToUpper( PString& s )
{
	PUtf8String tmp;
	tmp.reserve(s.length());
	const char* p = s.c_str();
	for(;;)
	{
		PUNICHAR_RET c = _nextChar(p);
		PASSERT4( c >= 0 );
		if(c <= 0)
			break;
		staticAppend(tmp, i18nCharToUpperCS( c ));
	}
	s.moveFrom(tmp);
}

/* static */ void PUtf8String::i18nToLower( PString& s )
{
	PUtf8String tmp;
	tmp.reserve(s.length());
	const char* p = s.c_str();
	for(;;)
	{
		PUNICHAR_RET c = _nextChar(p);
		PASSERT4( c >= 0 );
		if(c <= 0)
			break;
		staticAppend(tmp, i18nCharToLowerCS( c ));
	}
	s.moveFrom(tmp);
}

/* static */ void PUtf8String::i18nToLowerNoAsserts( PString& s )
{
	PUtf8String tmp;
	tmp.reserve(s.length());
	const char* p = s.c_str();
	for(;;)
	{
		PUNICHAR_RET c = _nextChar(p);
		if(c <= 0)
			break;
		staticAppend(tmp, i18nCharToLowerCS( c ));
	}
	s.moveFrom(tmp);
}

//****************************************************************************

/* virtual */ int PStreamEncodingUtf8::_readChars( PUNICHAR*& dst, PUNICHAR* dstEnd, const BYTE*& src_, const BYTE* srcEnd_ ) const
{
	const char*& src = (const char*&)src_;
	const char* srcEnd = (const char*)srcEnd_;
	int ret = 0;
	while( dst < dstEnd && src < srcEnd )
	{
		PUNICHAR_RET c = PUtf8String::_nextChar( src, srcEnd );
		if( c == -1 )//incomplete char
			return ret;
		else if( c < 0 )
			_throwUtf8FormatError();
		//0 is Ok here
		*dst++ = (PUNICHAR)c;
		++ret; //IgorS
	}
	return ret;
}

//****************************************************************************

/* virtual */ void PStringEncodingUtf8::_append( PString& s, PUNICHAR_32 c ) const
{
	if( c == 0 )
	{
		if( flags & FlagErrorOnNullChar )
			throw PInternalError( "PStringEncodingUtf8: unexpected NULL character" );
		else if( flags & FlagDefaultCharOnNullChar )
		{
			PUtf8String::staticAppend( s, defaultChar );
			return;
		}
		else
			return;//ignore
	}

	PUtf8String::staticAppend( s, c );
}

/* virtual */ void PStringEncodingUtf8::preparePMsg(PString& ret, const char* msg, int argc, const char* argv[]) const
{
	preparePMsg(ret, nullptr, nullptr, msg, argc, argv);
}

void PStringEncodingUtf8::preparePMsg(PString& ret, const PStringEncoding* encString, const PStringEncoding* argEncString, const char* msg, int argc, const char* argv[]) const
{
	if(!encString)
		encString = this;

	if(!argEncString)
		argEncString = this;

	PUtf8String::StringParser srcParser;

	PStringEncoding::StringParser argParser;
	argEncString->createNewParser(argParser);

	srcParser.init(msg, 0);

	for(PUNICHAR_RET s = srcParser.nextChar(); s > 0; s = srcParser.nextChar())
	{
		if(s != '%')
		{
			encString->append(ret, s);
			continue;
		}
		//%
		if((s = srcParser.nextChar()) <= 0)
			break;
		if(s == '%')
		{
			encString->append(ret, '%');
			continue;
		}
		//L or l
		bool isLower = false;
		if(s == 'L' || s == 'l')
		{
			isLower = true;
			if((s = srcParser.nextChar()) <= 0)
				break;
		}
		//0-9
		if(s >= '0' && s <= '9')
		{
			int m = s - '0';
			if(m < argc)
			{
				argParser.init(argv[m]);
				for(PUNICHAR_RET t = argParser.nextChar(); t > 0; t = argParser.nextChar())
				{
					encString->append(ret, isLower ? i18nCharToLowerCS(t) : t);
				}
			}
		}
		else
		{
			//invalid msg format - ignored
		}
	}
}
