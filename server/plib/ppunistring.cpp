#include "ppinclude.h"
#include "ppunistring.h"
#include "ppi18n_casefolding.h"

const PUNICHAR PUniString::_emptyString[] = { 0 };

static inline bool _isSpace( PUNICHAR c )
{
	return c == ' ' || c == '\t';
}

//****************************************************************************

PUniString::PUniString( const PUNICHAR* s )
: PBlock( s && *s ? ( length( s ) + 1 ) * sizeof(PUNICHAR) : 0 )
{
	if( s && *s )
		memcpy( _p(), s, size() );
}

static inline size_t limited_cpy( PUNICHAR* dst, const PUNICHAR* src, size_t sz_ )
{
	size_t n;
	for( n = 0; n < sz_ && src[n]; n++ )
		dst[n] = src[n];
	dst[n] = 0;
	return n + 1;
}

PUniString::PUniString( const PUNICHAR* s, PSTR_SIZE_T sz_ )
: PBlock( s && sz_ > 0 ? ( sz_ + 1 ) * sizeof(PUNICHAR) : 0 )
{
	if( s && sz_ > 0 )
	{
		_cut( limited_cpy( _p(), s, sz_ ) * sizeof(PUNICHAR) );
		PASSERT5( length( _p() ) == length() );
	}
}

PUniString::PUniString( const PUNICHAR* begin, const PUNICHAR* end )
: PBlock( end > begin ? ( end - begin + 1 ) * sizeof(PUNICHAR) : 0 )
{
	if( end > begin )
	{
		_cut( limited_cpy( _p(), begin, end - begin ) * sizeof(PUNICHAR) );
		PASSERT5( length( _p() ) == length() );
	}
}

PUniString& PUniString::assign( const PUNICHAR* s )
{
	if( s == 0 || *s == 0 )
	{
		PBlock::alloc( 0 );
	}
	else
	{
		PUniString tmp( s );//to solve problems with assign of (this->p() + shift)
		PBlock::moveFrom( tmp );
	}
	return *this;
}

PUniString& PUniString::assign( const PUNICHAR* s, PSTR_SIZE_T sz_ )
{
	if( s && sz_ > 0 )
	{
		PUniString tmp( s, sz_ );//to solve problems with assign of (this->p() + shift)
		PBlock::moveFrom( tmp );
	}
	else
	{
		PBlock::alloc( 0 );
	}

	return *this;
}
/*
PUniString& PUniString::appendInt( int i )
{
	PUNICHAR buf[ 16 ];
	_append( p_itoa(i, buf) );
	return *this;
}

PUniString& PUniString::appendUint( UINT32 i )
{
	PUNICHAR buf[ 16 ];
	_append( p_utoa(i, buf) );
	return *this;
}

PUniString& PUniString::appendUint64( UINT64 i )
{
	PUNICHAR buf[ 32 ];
	_append( p_u64toa(i, buf) );
	return *this;
}

PUniString& PUniString::appendInt64( INT64 i )
{
	PUNICHAR buf[ 32 ];
	_append( p_i64toa(i, buf) );
	return *this;
}

PUniString& PUniString::appendHexInt( int i )
{
	PUNICHAR buf[ 16 ];
	_append( p_hextoa(i, buf) );
	return *this;
}

PUniString& PUniString::appendHexInt64( UINT64 i )
{
	PUNICHAR buf[ 32 ];
	_append( p_hex64toa(i, buf) );
	return *this;
}
*/
/* static */ const PUNICHAR* PUniString::ltrim( const PUNICHAR* s )
{
	for(;;)
		if( _isSpace( *s ) )
			++s;
		else
			break;
	return s;
}

/* static */ const PUNICHAR* PUniString::rtrim( const PUNICHAR* s )
{
	const PUNICHAR* ret = s + length( s );
	for(;;)
		if( ret > s && _isSpace( ret[ -1 ] ) )
			--ret;
		else
			break;
	return ret;
}

void PUniString::toUpper()//may NOT be changed to affect non-ASCII or it will break i18n implementations
{
	if( _p() )
		for( PUNICHAR* pp = _p(); *pp ; ++pp )
			*pp = i18nCharToUpperCS( *pp );
}

void PUniString::toLower()//may NOT be changed to affect non-ASCII or it will break i18n implementations
{
	if( _p() )
		for( PUNICHAR* pp = _p(); *pp ; ++pp )
			*pp = i18nCharToLowerCS( *pp );
}

void PUniString::i18nToUpper()
{
	if( _p() )
		for( PUNICHAR* pp = _p(); *pp ; ++pp )
			*pp = i18nCharToUpperCS( *pp );
}

void PUniString::i18nToLower()
{
	if( _p() )
		for( PUNICHAR* pp = _p(); *pp ; ++pp )
			*pp = i18nCharToLowerCS( *pp );
}

PSTR_SIZE_T PUniString::length( const PUNICHAR* s )
{
	const PUNICHAR *s_ = s;
	while( *s_ )
		++s_;
	return s_ - s;
}

int PUniString::compare( const PUNICHAR* s, const PUNICHAR* s2 )//NOT lexicographical order, just _some_ strict weak order
{
	while( *s && *s == *s2 )
		++s, ++s2;
	return *s - *s2;
}

int PUniString::compare( const PUNICHAR* s, const PUNICHAR* s2, PSTR_SIZE_T n )//NOT lexicographical order, just _some_ strict weak order
{
	if( !n )
		return 0;
	while( --n && *s && *s == *s2 )
		++s, ++s2;
	return *s - *s2;
}

void PUniString::_append( const PUNICHAR* s )
{
	if( s == 0 )
		return;
	PASSERT5( ( 0 == size() ) || ( 0 == *( _p() + size() / sizeof(PUNICHAR) - 1 ) ) );
	size_t sLen = length( s );
	if( size() )
		limited_cpy( (PUNICHAR*)PBlock::append( sLen * sizeof(PUNICHAR) ) - 1, s, sLen ); // instead of strcpy - allows self append
	else
		memcpy( (char*)PBlock::append( ( sLen + 1 ) * sizeof(PUNICHAR) ), s, ( sLen + 1 ) * sizeof(PUNICHAR) );
	PASSERT5( length( _p() + length() - sLen ) == sLen );
}

void PUniString::_append( const PUNICHAR* s, PSTR_SIZE_T n )
{
	if( n <= 0 || s == 0 || *s == 0 )
		return;
	PASSERT5( ( 0 == size() ) || ( 0 == *( _p() + size() / sizeof(PUNICHAR) - 1 ) ) );
	size_t prevLen = length();
	PBlock::append( ( size() ? n : n + 1 ) * sizeof(PUNICHAR) );
	_cut( ( limited_cpy( _p() + prevLen, s, n ) + prevLen ) * sizeof(PUNICHAR) );
	PASSERT5( length( _p() + prevLen ) + prevLen == length() );
}

void PUniString::_append( PUNICHAR c )
{
	if( ! c )
		return;
	PASSERT5( ( 0 == size() ) || ( 0 == *( _p() + size() / sizeof(PUNICHAR) - 1 ) ) );
	PUNICHAR* pp;
	if( size() )
		pp = (PUNICHAR*)PBlock::append( sizeof(PUNICHAR) ) - 1;
	else
		pp = (PUNICHAR*)PBlock::append( 2 * sizeof(PUNICHAR) );
	pp[ 0 ] = c;
	pp[ 1 ] = 0;
}

/* static */ bool PUniString::startsWith( const PUNICHAR* s, const PUNICHAR* s2, PSTR_SIZE_T n )
{
	if( length( s ) < n )
		return false;
	return memcmp( s, s2, n * sizeof(PUNICHAR) ) == 0;
}

/* static */ bool PUniString::endsWith( const PUNICHAR* s, const PUNICHAR* s2, PSTR_SIZE_T n )
{
	PSTR_SIZE_T sLen = length( s );
	if( sLen < n )
		return false;
	return memcmp( s + sLen - n, s2, n * sizeof(PUNICHAR) ) == 0;
}

/* static */ int PUniString::compareIgnoreCase( const PUNICHAR* src, const PUNICHAR* dst )
//may NOT be changed to ignore case for non-ASCII or it will break i18n implementations
{
	for( ;; ++src, ++dst )
	{
		PUNICHAR c1 = i18nCharToUpperCS( *src );
		PUNICHAR c2 = i18nCharToUpperCS( *dst );
		int ret = c1 - c2;
		if( ret || c2 == 0 )
			return ret;
	}
}

/* static */ int PUniString::compareIgnoreCase( const PUNICHAR* src, const PUNICHAR* dst, PSTR_SIZE_T n )
//may NOT be changed to ignore case for non-ASCII or it will break i18n implementations
{
	const PUNICHAR* srcEnd = src + n;
	for( ; src < srcEnd ; ++src, ++dst )
	{
		PUNICHAR c1 = i18nCharToUpperCS( *src );
		PUNICHAR c2 = i18nCharToUpperCS( *dst );
		int ret = c1 - c2;
		if( ret || c2 == 0 )
			return ret;
	}
	return 0;
}
