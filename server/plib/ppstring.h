#ifndef ppstring_h_included
#define ppstring_h_included

#include "ppincludebase.h"
#include "ppblock.h"

#if SIZE_MAX > 0xFFFFFFFF
typedef INT64 PSTR_SIZE_T;
#elif _MSC_VER >= 1400
typedef INT32 __w64 PSTR_SIZE_T;
#else
typedef INT32 PSTR_SIZE_T;
#endif

class PString : private PBlock
{
public:
	PString()
	{
	}
	PString( const PString& other )
		: PBlock( other )
	{
	}
	PString( _In_opt_z_ const char* s );
	PString( _In_reads_or_z_(sz) const char* s, PSTR_SIZE_T sz );
	PString( _In_reads_or_z_(end-begin) const char* begin, _In_ const char* end );
	PString& operator=( const PString& other )
	{
		return assign( other );
	}
	PString& operator=( _In_opt_z_ const char* s )
	{
		return assign( s );
	}
#ifdef MOVE_CONSTRUCTOR_SUPPORTED
	PString( PString&& other ) : PBlock( std::move( other ) ) {}
	PString& operator=( PString&& other ) { moveFrom( other ); return *this; }
#endif

	void moveFrom( PString& other )
	{
		PBlock::moveFrom( other );
	}
	void moveFrom( PBlock& other );

	void operator+=( const PString& other )
	{
		append( other );
	}
	void operator+=( _In_opt_z_ const char* s )
	{
		_append( s );
	}
	~PString()
	{
	}

	PString& assign( const PString& other )
	{
		assign( other.c_str(), other.length() );
		return *this;
	}
	PString& assign( _In_opt_z_ const char* s );
	PString& assign( _In_reads_or_z_(sz) const char* s, PSTR_SIZE_T sz );

	//--------------------------------------------------------------------------
	//	!!! PLEASE NOTE !!!
	//--------------------------------------------------------------------------
	//	The 'assign' method below copies no characters when 'end' equals to 'nullptr';
	//		contarary to e.g. 'PStringEncoding::append' which copies characters up
	//		to the null terminator.
	//--------------------------------------------------------------------------

	PString& assign( _In_reads_or_z_(end-begin) const char* begin, const char* end )
	{
		return assign( begin, end - begin );
	}

	//--------------------------------------------------------------------------

	PString& assignFromBlock( _In_reads_or_z_(sz_) const BYTE * ptr, size_t sz_ )
	{
		return assign( (const char*)ptr, sz_ );
	}

	PString& append( const PString& other )
	{
		_append( other.c_str(), other.length() );
		return *this;
	}
	PString& append( _In_opt_z_ const char* s )
	{
		_append( s );
		return *this;
	}
	PString& append( _In_reads_or_z_(end-begin) const char* begin, _In_ const char* end )
	{
		_append( begin, end - begin );
		return *this;
	}
	PString& append( _In_reads_or_z_(sz_) const char* s, PSTR_SIZE_T sz_ )
	{
		_append( s, sz_ );
		return *this;
	}
	PString& append( char c )
	{
		_append( c );
		return *this;
	}
	PString& append( unsigned char c ) // PYR-58229, created so devs cannot mistakenly call append with integers
	{
		return append((char)c);
	}
	PString& append( signed char c ) // PYR-58229, created so devs cannot mistakenly call append with integers
	{
		return append((char)c);
	}

	PString& appendInt(  INT32 i );
	PString& appendInt( UINT32 i ) { return appendInt( static_cast< INT32 >( i ) );         }
	PString& appendInt(  INT16 i ) { return appendInt( static_cast< INT32 >( i ) );         }
	PString& appendInt( UINT16 i ) { return appendInt( static_cast< INT32 >( i ) );         }
	PString& appendInt(  INT64 i ) { return appendInt64( i );                               }
	PString& appendInt( UINT64 i ) { return appendInt64( static_cast< INT64 >( i ) );       }
#ifndef P_ANDROID_64
	PString& appendInt(   long i ) { return appendInt64( i );                               }
	PString& appendInt( unsigned long i ) { return appendInt64( static_cast< long >( i ) ); }
#endif

	PString& appendUint( UINT32 i );
	PString& appendUint(  INT32 i ) { return appendUint( static_cast< UINT32 >( i ) );          }
	PString& appendUint( UINT16 i ) { return appendUint( static_cast< UINT32 >( i ) );          }
	PString& appendUint(  INT16 i ) { return appendUint( static_cast< UINT32 >( i ) );          }
	PString& appendUint( UINT64 i ) { return appendUint64( i );                                 }
	PString& appendUint(  INT64 i ) { return appendUint64( static_cast< UINT64 >( i ) );        }
#ifndef P_ANDROID_64
	PString& appendUint( unsigned long i ) { return appendUint64( i );                          }
	PString& appendUint(   long i ) { return appendUint64( static_cast< unsigned long >( i ) ); }
#endif

	PString& appendHexInt( UINT32 i );
	PString& appendHexInt(  INT32 i ) { return appendHexInt( static_cast< UINT32 >( i ) );          }
	PString& appendHexInt( UINT16 i ) { return appendHexInt( static_cast< UINT32 >( i ) );          }
	PString& appendHexInt(  INT16 i ) { return appendHexInt( static_cast< UINT32 >( i ) );          }
	PString& appendHexInt( UINT64 i ) { return appendHexInt64( i );                                 }
	PString& appendHexInt(  INT64 i ) { return appendHexInt64( static_cast< UINT64 >( i ) );        }
	PString& appendHexInt(   char i ) { return appendHexInt( static_cast< UINT32 >( i ) );          }
#ifndef P_ANDROID_64
	PString& appendHexInt( unsigned long i ) { return appendHexInt64( i );                          }
	PString& appendHexInt(   long i ) { return appendHexInt64( static_cast< unsigned long >( i ) ); }
#endif

	PString& appendInt64( INT64 i ); // PYR-62256, obsolete
	PString& appendUint64( UINT64 i ); // PYR-62256, obsolete
	PString& appendHexInt64( UINT64 i ); // PYR-62256, obsolete

    PString& appendDouble(  double d );

	void reserve( size_t reserved_ )
	{
		if( reserved_ == 0 )
			return;
		if( size() == 0 )
		{
			_PBlock::reserve( reserved_ + 1 );
			*_p() = 0;
		}
		else
		{
			_PBlock::reserve( reserved_ + 1 );
		}
	}

	void cut( PSTR_SIZE_T len )
	{
		if( _p() && ( 0 <= len ) && ( (size_t)len < size() ) )
		{
			_p()[ len ] = 0;
			PBlock::cut( len + 1 );
		}
	}
	void cutAtChar( PSTR_SIZE_T len )//same as cut() for PString, but different for i18n
	{
		cut( len );
	}

	void toUpper();//affects ASCII only; may NOT be changed to affect non-ASCII or it will break i18n implementations
	void toLower();//affects ASCII only; may NOT be changed to affect non-ASCII or it will break i18n implementations

	_Ret_z_
	const char* c_str() const
	{
#ifndef	PROTOCOL_CLIENT_ONLY
		PASSERT5( !_p() || !*(_p() + length()) );//??? - to PASSERT5_1?
#endif
		return _p() ? _p() : "";
	}
	_Ret_z_
	operator const char*() const
	{
		return c_str();
	}
#if defined (P_MAC10X) 
	char operator[]( size_t idx ) const
	{
		PASSERT5( idx <= (size_t)length() );
		return c_str()[ idx ];
	}
#endif
	char operator[]( UINT64 idx ) const
	{
		PASSERT5( idx <= (UINT64)length() );
		return c_str()[ idx ];
	}
	char operator[]( UINT32 idx ) const
	{
		PASSERT5( (UINT64)idx <= (UINT64)length() );
		return c_str()[ idx ];
	}
	char operator[]( INT64 idx ) const
	{
		PASSERT5( idx >= 0 );
		PASSERT5( idx <= length() );
		return c_str()[ idx ];
	}
	char operator[]( INT32 idx ) const
	{
		PASSERT5( idx >= 0 );
		PASSERT5( idx <= length() );
		return c_str()[ idx ];
	}
	char charAt( PSTR_SIZE_T idx ) const//same as operator[]() for PString, but different for i18n
	{
		PASSERT5( idx >= 0 );
		PASSERT5( idx <= length() );
		return c_str()[ idx ];
	}
	PSTR_SIZE_T length() const
	{
		return size() ? size() - 1 : 0;
	}
	void clear()
	{
		if( _p() )
		{
			memset(_p(),0,size()); //ok
		}
		PBlock::clear();
	}

	bool equals( _In_z_ const char* s ) const
	{
		return compare( c_str(), s ) == 0;
	}
	bool equalsIgnoreCase( _In_z_ const char* s ) const//ignores case for ASCII only; may NOT be changed to ignore case for non-ASCII or it will break i18n implementations
	{
		return compareIgnoreCase( c_str(), s ) == 0;
	}

	bool startsWith( _In_z_ const char* s ) const
	{
		return startsWith( c_str(), s, length( s ) );
	}
	bool endsWith( _In_z_ const char* s ) const
	{
		return endsWith( c_str(), s, length( s ) );
	}

	_Ret_maybenull_z_
	const char* find( char c ) const
	{
		return find( c_str(), c );
	}
	_Ret_maybenull_z_
	const char* findIgnoreCase( char c ) const
	{
		return findIgnoreCase( c_str(), c );
	}

	_Ret_maybenull_z_
	const char* find( _In_z_ const char* s ) const
	{
		return find( c_str(), s );
	}
	_Ret_maybenull_z_
	const char* findIgnoreCase( _In_z_ const char* s ) const
	{
		return findIgnoreCase( c_str(), s );
	}

	_Check_return_ _Ret_z_
	const char* ltrim() const
	{
		return ltrim( c_str() );
	}
	_Check_return_ _Ret_z_
	const char* rtrim() const
	{
		return rtrim( c_str() );
	}

public:
	static PSTR_SIZE_T length( _In_z_ const char* s )
	{
		return strlen( s );
	}

	static int compare( _In_z_ const char* s, _In_z_ const char* s2 )//NOT lexicographical order, just _some_ strict weak order
	{
		return strcmp( s, s2 );
	}
	static int compare( _In_reads_or_z_(n) const char* s, _In_reads_or_z_(n) const char* s2, PSTR_SIZE_T n )//NOT lexicographical order, just _some_ strict weak order
	{
		return strncmp( s, s2, n );
	}
	static bool equals( _In_z_ const char* s, _In_z_ const char* s2 )
	{
		return compare( s, s2 ) == 0;
	}

	static int compareIgnoreCase( _In_z_ const char* s, _In_z_ const char* s2 );//ignores case for ASCII only; may NOT be changed to ignore case for non-ASCII or it will break i18n implementations
	static int compareIgnoreCase( _In_z_ const char* s, _In_z_ const char* s2, PSTR_SIZE_T n );//ignores case for ASCII only; may NOT be changed to ignore case for non-ASCII or it will break i18n implementations

	static bool startsWith( _In_z_ const char* s, _In_reads_(n) const char* s2, PSTR_SIZE_T n );
	static bool startsWith( _In_z_ const char* s, _In_z_ const char* s2 )
	{
		return startsWith( s, s2, length( s2 ) );
	}
	static bool endsWith( _In_z_ const char* s, _In_reads_(n) const char* s2, PSTR_SIZE_T n );
	static bool endsWith( _In_z_ const char* s, _In_z_ const char* s2 )
	{
		return endsWith( s, s2, length( s2 ) );
	}

	_Ret_maybenull_
	_When_(return != NULL, _Ret_range_(s, s+_String_length_(s)-1))
	static const char* find( _In_z_ const char* s, char c )
	{
		return strchr( s, c );
	}
	_Ret_maybenull_
	_When_(return != NULL, _Ret_range_(s, s+_String_length_(s)-1))
	static const char* findIgnoreCase( _In_z_ const char* s, char c );

	_Ret_maybenull_
	_When_(return != NULL, _Ret_range_(s, s+_String_length_(s)-1))
	static const char* find( _In_z_ const char* s, _In_z_ const char* s2 )
	{
		return strstr( s, s2 );
	}
	_Ret_maybenull_
	_When_(return != NULL, _Ret_range_(s, s+_String_length_(s)-1))
	static const char* findIgnoreCase( _In_z_ const char* s, _In_z_ const char* s2 );

	_Check_return_ _Ret_z_ _Ret_range_(s, s+_String_length_(s)-1)
	static const char* ltrim( _In_z_ const char* s );
	_Check_return_ _Ret_z_ _Ret_range_(s, s+_String_length_(s)-1)
	static const char* rtrim( _In_z_ const char* s );
	static bool compareWithWildChar( _In_z_ const char * pattern, _In_z_ const char * value );

	static unsigned char toUpper( unsigned char c )
	//may NOT be changed to affect non-ASCII or it will break i18n implementations
	{
		if( c >= 'a' && c <= 'z' )
			return c + 'A' - 'a';
		else
			return c;
	}

	static unsigned char toLower( unsigned char c )
	//may NOT be changed to affect non-ASCII or it will break i18n implementations
	{
		if( c >= 'A' && c <= 'Z' )
			return c + 'a' - 'A';
		else
			return c;
	}

private:
	void _append( _In_opt_z_ const char* s );
	void _append( _In_reads_or_z_(n) const char* s, PSTR_SIZE_T n );
	void _append( char c );
	char* _p()
	{
		return (char*)ptr();
	}
	const char* _p() const
	{
		return (const char*)ptr();
	}
};

// !! comparison operators below are prohibited !! - use PString::compare() or PString::equals() instead
void operator==( const PString& s1, const PString& s2 );
void operator==( const PString& s1, const char* s2 );
void operator==( const char* s1, const PString& s2 );
void operator!=( const PString& s1, const PString& s2 );
void operator!=( const PString& s1, const char* s2 );
void operator!=( const char* s1, const PString& s2 );
void operator<( const PString& s1, const PString& s2 );
void operator<( const PString& s1, const char* s2 );
void operator<( const char* s1, const PString& s2 );
void operator>( const PString& s1, const PString& s2 );
void operator>( const PString& s1, const char* s2 );
void operator>( const char* s1, const PString& s2 );
void operator<=( const PString& s1, const PString& s2 );
void operator<=( const PString& s1, const char* s2 );
void operator<=( const char* s1, const PString& s2 );
void operator>=( const PString& s1, const PString& s2 );
void operator>=( const PString& s1, const char* s2 );
void operator>=( const char* s1, const PString& s2 );
//size_t strlen( const PString& s ); // use s.length() instead

//****************************************************************************

#endif
