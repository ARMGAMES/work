#ifndef ppunistring_h_included
#define ppunistring_h_included

#include "ppincludebase.h"
#include "ppblock.h"
#include "ppi18n.h"

class PUniString : private PBlock
{
	static const PUNICHAR _emptyString[];

public:
	PUniString()
	{
	}
	PUniString( const PUniString& other )
		: PBlock( other )
	{
	}
	PUniString( const PUNICHAR* s );
	PUniString( const PUNICHAR* s, PSTR_SIZE_T sz );
	PUniString( const PUNICHAR* begin, const PUNICHAR* end );
	PUniString& operator=( const PUniString& other )
	{
		return assign( other );
	}
	PUniString& operator=( const PUNICHAR* s )
	{
		return assign( s );
	}
#ifdef MOVE_CONSTRUCTOR_SUPPORTED
	PUniString( PUniString&& other ) : PBlock( std::move( other ) ) {}
	PUniString& operator=( PUniString&& other ) { moveFrom( other ); return *this; }
#endif
	void moveFrom( PUniString& other )
	{
		PBlock::moveFrom( other );
	}

	void operator+=( const PUniString& other )
	{
		_append( other.c_str() );
	}
	void operator+=( const PUNICHAR* s )
	{
		_append( s );
	}
	~PUniString()
	{
	}

	PUniString& assign( const PUniString& other )
	{
		PBlock::operator=( other );
		return *this;
	}
	PUniString& assign( const PUNICHAR* s );
	PUniString& assign( const PUNICHAR* s, PSTR_SIZE_T sz );
	PUniString& assign( const PUNICHAR* begin, const PUNICHAR* end )
	{
		return assign( begin, end - begin );
	}

	PUniString& append( const PUniString& other )
	{
		_append( other.c_str() );
		return *this;
	}
	PUniString& append( const PUNICHAR* s )
	{
		_append( s );
		return *this;
	}
	PUniString& append( const PUNICHAR* begin, const PUNICHAR* end )
	{
		_append( begin, end - begin );
		return *this;
	}
	PUniString& append( const PUNICHAR* s, PSTR_SIZE_T sz_ )
	{
		_append( s, sz_ );
		return *this;
	}
	PUniString& append( PUNICHAR c )
	{
		_append( c );
		return *this;
	}
	PUniString& appendInt( int i );
	PUniString& appendUint( UINT32 i );
	PUniString& appendInt64( INT64 i );
	PUniString& appendUint64( UINT64 i );
	PUniString& appendHexInt( int i );
	PUniString& appendHexInt64( UINT64 i );
	void reserve( size_t reservedInChars )
	{
		PASSERT5( reservedInChars < SIZE_MAX / sizeof(PUNICHAR) );
		if( reservedInChars == 0 )
			return;
		if( size() == 0 )
		{
			_PBlock::reserve( ( reservedInChars + 1 ) * sizeof(PUNICHAR) );
			*_p() = 0;
		}
		else
		{
			_PBlock::reserve( ( reservedInChars + 1 ) * sizeof(PUNICHAR) );
		}
	}

	void cut( PSTR_SIZE_T len )
	{
		PASSERT4( len >= 0 && len + 1 <= static_cast< PSTR_SIZE_T >( size() ) );
		if( _p() )
		{
			_p()[ len ] = 0;
			PBlock::cut( ( len + 1 ) * sizeof(PUNICHAR) );
		}
	}
	void cutAtChar( PSTR_SIZE_T len )//same as cut() for PUniString, but different for i18n
	{
		cut( len );
	}

	void clear()
	{
		if (_p())
		{
			memset(_p(), 0, size()); //ok
		}
		PBlock::clear();
	}

	void toUpper();//affects ASCII only; may NOT be changed to affect non-ASCII or it will break i18n implementations
	void toLower();//affects ASCII only; may NOT be changed to affect non-ASCII or it will break i18n implementations

	void i18nToUpper();
	void i18nToLower();

	const PUNICHAR* c_str() const
	{
		PASSERT5( !_p() || length( _p() ) == length() );//??? - to PASSERT5_1?
		return _p() ? _p() : _emptyString;
	}
	const PUNICHAR* c_eos() const
	{
		PASSERT5( !_p() || length( _p() ) == length() );//??? - to PASSERT5_1?
		return _p() ? _p() + length() : _emptyString;
	}
	operator const PUNICHAR*() const
	{
		return c_str();
	}
	PUNICHAR operator[]( PSTR_SIZE_T idx ) const
	{
		PASSERT5( idx >= 0 && idx <= length() );
		return c_str()[ idx ];
	}
	PUNICHAR charAt( PSTR_SIZE_T idx ) const//same as operator[]() for PUniString, but different for i18n
	{
		return operator[]( idx );
	}
	PSTR_SIZE_T length() const
	{
		return size() ? size() / sizeof(PUNICHAR) - 1 : 0;
	}

	bool equals( const PUNICHAR* s ) const
	{
		return compare( c_str(), s ) == 0;
	}
	bool equalsIgnoreCase( const PUNICHAR* s ) const//ignores case for ASCII only; may NOT be changed to ignore case for non-ASCII or it will break i18n implementations
	{
		return compareIgnoreCase( c_str(), s ) == 0;
	}

	bool startsWith( const PUNICHAR* s ) const
	{
		return startsWith( c_str(), s, length( s ) );
	}
	bool endsWith( const PUNICHAR* s ) const
	{
		return endsWith( c_str(), s, length( s ) );
	}

	const PUNICHAR* ltrim() const
	{
		return ltrim( c_str() );
	}
	const PUNICHAR* rtrim() const
	{
		return rtrim( c_str() );
	}

public:
	static PSTR_SIZE_T length( const PUNICHAR* s );

	static int compare( const PUNICHAR* s, const PUNICHAR* s2 );//NOT lexicographical order, just _some_ strict weak order
	static int compare( const PUNICHAR* s, const PUNICHAR* s2, PSTR_SIZE_T n );//NOT lexicographical order, just _some_ strict weak order

	static int compareIgnoreCase( const PUNICHAR* s, const PUNICHAR* s2 );//ignores case for ASCII only; may NOT be changed to ignore case for non-ASCII or it will break i18n implementations
	static int compareIgnoreCase( const PUNICHAR* s, const PUNICHAR* s2, PSTR_SIZE_T n );//ignores case for ASCII only; may NOT be changed to ignore case for non-ASCII or it will break i18n implementations

	static bool startsWith( const PUNICHAR* s, const PUNICHAR* s2, PSTR_SIZE_T n );
	static bool startsWith( const PUNICHAR* s, const PUNICHAR* s2 )
	{
		return startsWith( s, s2, length( s2 ) );
	}
	static bool endsWith( const PUNICHAR* s, const PUNICHAR* s2, PSTR_SIZE_T n );
	static bool endsWith( const PUNICHAR* s, const PUNICHAR* s2 )
	{
		return endsWith( s, s2, length( s2 ) );
	}

	static const PUNICHAR* ltrim( const PUNICHAR* s );
	static const PUNICHAR* rtrim( const PUNICHAR* s );

private:
	void _append( const PUNICHAR* s );
	void _append( const PUNICHAR* s, PSTR_SIZE_T n );
	void _append( PUNICHAR c );
	PUNICHAR* _p()
	{
		return (PUNICHAR*)ptr();
	}
	const PUNICHAR* _p() const
	{
		return (const PUNICHAR*)ptr();
	}
};

// !! comparison operators below are prohibited !! - use PUniString::compare() or PUniString::equals() instead
void operator==( const PUniString& s1, const PUniString& s2 );
void operator==( const PUniString& s1, const PUNICHAR* s2 );
void operator==( const PUNICHAR* s1, const PUniString& s2 );
void operator!=( const PUniString& s1, const PUniString& s2 );
void operator!=( const PUniString& s1, const PUNICHAR* s2 );
void operator!=( const PUNICHAR* s1, const PUniString& s2 );
void operator<( const PUniString& s1, const PUniString& s2 );
void operator<( const PUniString& s1, const PUNICHAR* s2 );
void operator<( const PUNICHAR* s1, const PUniString& s2 );
void operator>( const PUniString& s1, const PUniString& s2 );
void operator>( const PUniString& s1, const PUNICHAR* s2 );
void operator>( const PUNICHAR* s1, const PUniString& s2 );
void operator<=( const PUniString& s1, const PUniString& s2 );
void operator<=( const PUniString& s1, const PUNICHAR* s2 );
void operator<=( const PUNICHAR* s1, const PUniString& s2 );
void operator>=( const PUniString& s1, const PUniString& s2 );
void operator>=( const PUniString& s1, const PUNICHAR* s2 );
void operator>=( const PUNICHAR* s1, const PUniString& s2 );

#endif    
