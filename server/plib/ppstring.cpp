#include "ppincludebase.h"
#include "ppstring.h"

static inline bool _isSpace( char c )
{
    return c == ' ' || c == '\t';
}

//****************************************************************************

PString::PString( _In_opt_z_ const char* s )
: PBlock( s && *s ? strlen( s ) + 1 : 0 )
{
	if( s && *s )
		strcpy( _p(), s );
}

static inline size_t limited_cpy( _Out_writes_z_(sz_+1) char* dst, _In_reads_or_z_(sz_) const char* src, size_t sz_ )
{
	size_t n;
	for( n = 0; n < sz_ && src[n]; n++ )
		dst[n] = src[n];
	dst[n] = 0;
	return n + 1;
}

PString::PString( _In_reads_or_z_(sz_) const char* s, PSTR_SIZE_T sz_ )
: PBlock( s && sz_ > 0 ? sz_ + 1 : 0 )
{
	if( s && sz_ > 0 )
	{
		_cut( limited_cpy( _p(), s, sz_ ) );
		PASSERT5( length( _p() ) == length() );
	}
}

PString::PString( _In_reads_or_z_(end-begin) const char* begin, _In_ const char* end )
: PBlock( end > begin ? end - begin + 1 : 0 )
{
	if( end > begin )
	{
		_cut( limited_cpy( _p(), begin, end - begin ) );
		PASSERT5( length( _p() ) == length() );
	}
}

void PString::moveFrom( PBlock& other )
{
	if (other.size() == 0 )
	{
		clear();
	}
	else
	{
		const unsigned char * s = other.ptr();
		int length = 0;
		size_t i = other.size();
		while(i--)
		{
			length++;
			if (!*s)
			{
				if (i)
				{
					other.cut(length);
					break;
				}
			}
			s++;
		}
		if (other.ptr()[length - 1])
		{
			*other.append(1) = 0;
		}
		PBlock::moveFrom( other );
	}
}

static inline void _directional_strcpy( _Out_writes_z_(_String_length_(src) + 1) char* dst, _In_z_ const char* src )
{
	while( ( *dst++ = *src++ ) != 0 )
		;
}

PString& PString::assign( _In_opt_z_ const char* s )
{
	if( s == 0 || *s == 0 )
	{
		PBlock::alloc( 0 );
	}
	else
	{
		auto len = length( s );
		if( _fitSize( len + 1 ) )
		{
			PASSERT5( ( s >= _p() ) || ( s + len + 1 < _p() ) ); // assume no overlap when s < _p()
			_directional_strcpy( _p(), s ); // in case of overlap - behavior of libc strcpy undefined
			_cut( len + 1 );
			PASSERT5( len == length() );
			PASSERT5( length( _p() ) == length() );
		}
		else
		{
			PString tmp( s, len );
			PBlock::moveFrom( tmp );
		}
	}
	return *this;
}

PString& PString::assign( _In_reads_or_z_(sz_) const char* s, PSTR_SIZE_T sz_ )
{
	if( s == 0 || sz_ <= 0 )
	{
		PBlock::alloc( 0 );
	}
	else if( _fitSize( sz_ + 1 ) )
	{
		_cut( limited_cpy( _p(), s, sz_ ) );
		PASSERT5( length( _p() ) == length() );
	}
	else
	{
		PString tmp( s, sz_ );
		PBlock::moveFrom( tmp );
	}
	return *this;
}

PString& PString::appendInt( INT32 i )
{
	char buf[ 16 ];
	_append( p_itoa(i, buf) );
	return *this;
}

PString& PString::appendUint( UINT32 i )
{
	char buf[ 16 ];
	_append( p_utoa(i, buf) );
	return *this;
}

PString& PString::appendUint64( UINT64 i )
{
	char buf[ 32 ];
	_append( p_u64toa(i, buf) );
	return *this;
}

PString& PString::appendInt64( INT64 i )
{
	char buf[ 32 ];
	_append( p_i64toa(i, buf) );
	return *this;
}

PString& PString::appendHexInt( UINT32 i )
{
	char buf[ 16 ];
	_append( p_hextoa(i, buf) );
	return *this;
}

PString& PString::appendHexInt64( UINT64 i )
{
	char buf[ 32 ];
	_append( p_hex64toa(i, buf) );
	return *this;
}

PString& PString::appendDouble(  double d )
{
    char buf[ 256 ];
#if _MSC_VER && (_MSC_VER < 1900)
    _snprintf(buf, sizeof(buf), "%.2f", d);
#else
    snprintf(buf, sizeof(buf), "%.2f", d);
#endif
    _append( buf );
    return *this;
}

_Ret_maybenull_
_When_(return != NULL, _Ret_range_(s, s+_String_length_(s)-1))
const char* PString::findIgnoreCase( _In_z_ const char* s, char c )
{
	c = toUpper( c );
	while( *s && toUpper( *s ) != c )
		++s;
	return toUpper( *s ) == c ? s : 0;
}

_Ret_maybenull_
_When_(return != NULL, _Ret_range_(s, s+_String_length_(s)-1))
const char* PString::findIgnoreCase( _In_z_ const char* s, _In_z_ const char* s2 )
{
	if( !*s2 )
		return s;
	while( *s )
	{
		const char* p = s;
		const char* p2 = s2;
		while( *p && *p2 && ( toUpper( *p ) == toUpper( *p2 ) ) )
			++p, ++p2;
		if( !*p2 )
			return s;
		++s;
	}
	return 0;
}

_Check_return_ _Ret_z_ _Ret_range_(s, s+_String_length_(s)-1)
const char* PString::ltrim( _In_z_ const char* s )
{
	for(;;)
		if( _isSpace( *s ) )
			++s;
		else
			break;
	return s;
}

_Check_return_ _Ret_z_ _Ret_range_(s, s+_String_length_(s)-1)
const char* PString::rtrim( _In_z_ const char* s )
{
	const char* ret = s + strlen( s );
	for(;;)
		if( ret > s && _isSpace( ret[ -1 ] ) )
			--ret;
		else
			break;
	return ret;
}

bool PString::compareWithWildChar( _In_z_ const char * pattern, _In_z_ const char * value)
{
	if (!*value)
	{
		while(*pattern)
		{
			if (*pattern == '*' || *pattern == '?')
			{
				pattern++;
			}
			else
				break;
		}
	}
	while(*value && *pattern )
	{
		if (*pattern == '*')
		{
			pattern++;
			if (!*pattern)
			{
				return true;
			}
			while(1)
			{
				while (*value && toLower(*value) != toLower(*pattern))
					value++;
				if (!*value)
					return false;
				bool ok = compareWithWildChar(pattern,value);
				if (ok)
					return true;
				value++;
			}

		}
		else if (*pattern == '?')
		{
			pattern++;
			value++;
		}
		else
		{
			if (toLower(*pattern++) != toLower(*value++))
				return false;
		}
	}
	if (!*value)
	{
		while(*pattern)
		{
			if (*pattern == '*' || *pattern == '?')
			{
				pattern++;
			}
			else
				break;
		}
	}
	return (!*value && !*pattern);
}

void PString::toUpper()//may NOT be changed to affect non-ASCII or it will break i18n implementations
{
	if( _p() )
		for( char* pp = _p(); *pp ; ++pp )
			*pp = toUpper( *pp );
}

void PString::toLower()//may NOT be changed to affect non-ASCII or it will break i18n implementations
{
	if( _p() )
		for( char* pp = _p(); *pp ; ++pp )
			*pp = toLower( *pp );
}

void PString::_append( _In_opt_z_ const char* s )
{
	if( s == 0 || *s == 0 )
		return;
	PASSERT5( ( 0 == size() ) || ( 0 == *( _p() + size() - 1 ) ) );
	auto sLen = length( s );
	if( size() )
		limited_cpy( (char*)PBlock::append( sLen ) - 1, s, sLen ); // instead of strcpy - allows self append
	else
		strcpy( (char*)PBlock::append( sLen + 1 ), s );
	PASSERT5( length( _p() + length() - sLen ) == sLen );
}

void PString::_append( _In_reads_or_z_(n) const char* s, PSTR_SIZE_T n )
{
	if( n <= 0 || s == 0 || *s == 0 )
		return;
	PASSERT5( ( 0 == size() ) || ( 0 == *( _p() + size() - 1 ) ) );
	auto prevLen = length();
	PBlock::append( size() ? n : n + 1 );
	_cut( limited_cpy( _p() + prevLen, s, n ) + prevLen );
	PASSERT5( length( _p() + prevLen ) + prevLen == length() );
}

void PString::_append( char c )
{
	if( ! c )
		return;
	PASSERT5( ( 0 == size() ) || ( 0 == *( _p() + size() - 1 ) ) );
	char* pp;
	if( size() )
		pp = (char*)PBlock::append( 1 ) - 1;
	else
		pp = (char*)PBlock::append( 2 );
	pp[ 0 ] = c;
	pp[ 1 ] = 0;
}

/* static */ bool PString::startsWith( _In_z_ const char* s, _In_reads_(n) const char* s2, PSTR_SIZE_T n )
{
	if( length( s ) < n )
		return false;
	return memcmp( s, s2, n ) == 0;
}

/* static */ bool PString::endsWith( _In_z_ const char* s, _In_reads_(n) const char* s2, PSTR_SIZE_T n )
{
	PSTR_SIZE_T sLen = length( s );
	if( sLen < n )
		return false;
	return memcmp( s + sLen - n, s2, n ) == 0;
}

/* static */int PString::compareIgnoreCase( _In_z_ const char* s, _In_z_ const char* s2 )
//may NOT be changed to ignore case for non-ASCII or it will break i18n implementations
{
	for( ;; ++s, ++s2 )
	{
		unsigned char c1 = toUpper( *s );
		unsigned char c2 = toUpper( *s2 );
		int ret = c1 - c2;
		if( ret || c2 == 0 )
			return ret;
	}
}

int PString::compareIgnoreCase( _In_z_ const char* s, _In_z_ const char* s2, PSTR_SIZE_T n )
//may NOT be changed to ignore case for non-ASCII or it will break i18n implementations
{
	const char* srcEnd = s + n;
	for( ; s < srcEnd ; ++s, ++s2 )
	{
		unsigned char c1 = toUpper( *s );
		unsigned char c2 = toUpper( *s2 );
		int ret = c1 - c2;
		if( ret || c2 == 0 )
			return ret;
	}
	return 0;
}
