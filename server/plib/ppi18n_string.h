#ifndef ppi18n_string_h_included
#define ppi18n_string_h_included

#include "ppi18n.h"

inline bool isUniSpace( PUNICHAR_32 c )
{
	return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

inline bool isUniSpace( PUNICHAR_RET c )
{
	return isUniSpace( static_cast< PUNICHAR_32 >( c ) );
}

template< class StringParser >
INT64 PI18N_strtol64( const StringParser& str, StringParser* end, int base, PUNICHAR separator = 0 )
{
	PASSERT4( base == 0 || ( base >= 2 && base <= 36 ) );

	StringParser s = str;
	PUNICHAR_RET ch = 0;
	for(;;)
	{
		if( end )
			*end = s;
		ch = s.nextChar();
		if( ch <= 0 )
			return 0;

		if( !isUniSpace( ch ) )
			break;
	}

	bool neg = false;
	if( ch == '-' )
	{
		neg = true;
		if( end )
			*end = s;
		ch = s.nextChar();
	}
	else if( ch == '+' )
	{
		if( end )
			*end = s;
		ch = s.nextChar();
	}

	if( ch <= 0 )
		return 0;

	if( base == 0 )
	{
		if( ch == '0' )
		{
			base = 8;
			if( end )
				*end = s;
			ch = s.nextChar();
		}
		else if( ch == 'x' || ch == 'X' )
		{
			base = 16;
			if( end )
				*end = s;
			ch = s.nextChar();
		}
		else
			base = 10;
	}

	if( ch <= 0 )
		return 0;

	INT64 ret = 0;
	for(;;)
	{
		if( ch >= '0' && ch < '0' + base )
			ret = ret * base + ( ch - '0' );
		else if( base > 10 && ch >= 'a' && ch < 'a' + base - 10 )
			ret = ret * base + ( ch - 'a' + 10 );
		else if( base > 10 && ch >= 'A' && ch < 'A' + base - 10 )
			ret = ret * base + ( ch - 'A' + 10 );
		else if( !separator || ( ch != separator ) )
			return neg ? -ret : ret;

		if( end )
			*end = s;
		ch = s.nextChar();
	}
}

template< class StringParser >
StringParser PI18N_strchr( const StringParser& str, PUNICHAR ch )
{
	StringParser s = str;
	StringParser ret = s;
	for(;;)
	{
		ret = s;
		PUNICHAR_RET c = s.nextChar();
		if( c <= 0 )
			return ret;

		if( c == ch )
			return ret;
	}
}

template< class StringParser, class StringEncoding >
void PI18N_strcpy( PString& ret, const StringEncoding* encString, const StringParser& s )
{
	StringParser p = s;
	for(;;)
	{
		PUNICHAR_RET ch = p.nextChar();
		if( ch <= 0 )
			return;
		encString->append( ret, ch );
	}
}

template< class StringParser, class StringEncoding >
void PI18N_strcpy( PString& ret, const StringEncoding* encString, const StringParser& s, const StringParser& end )
{
	if( end < s || end == s )
		return;
	StringParser p = s;
	for(;;)
	{
		PUNICHAR_RET ch = p.nextChar();
		if( ch <= 0 )
			return;
		encString->append( ret, ch );
		if( p == end )
			return;
	}
}

template< class StringParser >
StringParser PI18N_ltrim( const StringParser& s )
{
	StringParser p = s;
	StringParser ret;
	for(;;)
	{
		ret = p;
		PUNICHAR_RET ch = p.nextChar();
		if( !isUniSpace( ch ) )
			return ret;
	}
}

template< class StringParser >
StringParser PI18N_rtrim( const StringParser& s )
{
	StringParser p = s;
	StringParser ret = s;

	for(;;)
	{
		PUNICHAR_RET ch = p.nextChar();
		if( ch <= 0 )
			return ret;
		if( !isUniSpace( ch ) )
			ret = p;
	}
}

template< class StringParser >
PUNICHAR_RET PI18N_charAt( const StringParser& s, int n )
{
	PASSERT4( n >= 0 );
	StringParser p = s;
	for( int i=0 ;; ++i )
	{
		PUNICHAR_RET ch = p.nextChar();
		if( ch <= 0 || i == n )
			return ch;
	}
}

template< class StringParser >
PUNICHAR_RET PI18N_strlen( const StringParser& s )
{
	StringParser p = s;
	for( int i=0 ;; ++i )
	{
		PUNICHAR_RET ch = p.nextChar();
		if( ch <= 0 )
			return i;
	}
}

template< class StringParser, class StringEncoding >
void PI18N_preparePMsg( PString& ret, const StringEncoding* encString, const StringParser& srcParser_, StringParser& argParser, int argc, const char* argv[] )
{
	StringParser srcParser = srcParser_;
	for( PUNICHAR_RET s = srcParser.nextChar(); s > 0; s = srcParser.nextChar() )
	{
		if( s == '%' )
		{
			PUNICHAR_RET c = srcParser.nextChar();
			if( c == '%' )
				encString->append( ret, '%' );
			else if( c >= '0' && c <= '9' )
			{
				int m = c - '0';
				if( m < argc )
				{
					argParser.init( argv[m] );
					PI18N_strcpy( ret, encString, argParser );
				}
			}
			else
			{
				//invalid msg format - ignored
			}
		}
		else
			encString->append( ret, s );
	}
}

#endif
