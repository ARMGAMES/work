#ifndef ppi18n_bcustring_h_included
#define ppi18n_bcustring_h_included

#include "ppinclude.h"
#include "ppstring.h"
#include "ppi18n.h"
#include "ppi18n_utf8.h"
#include "ppi18n_casefolding.h"

template< class Props >
//Props requirements: 
//enum { _utf8Prefix }
//static int _staticReadChars() - for default encoding
//static int _staticWriteChar() - for default encoding
class PBackwardCompatibleUniString : private PString
	//special string which either has specific encoding, or starts with prefix byte and is UTF-8 afterwards
	//always normalizes content to 'canonical' representation, which is without _utf8Prefix whenever possible
{
private:
	typedef PString _Parent;

public:
	class StringParser
	{
		friend class PBackwardCompatibleUniString< Props >;

	private:
		bool mode;//true if UTF-8
		const char* p;
		const char* endP;

	public:
		StringParser()
		{
			mode = false;
			p = endP = NULL;
		}
		StringParser( const char* p_, const char* endP_ = 0 )//endP == 0 means that p is null-terminated
		{
			init( p_, endP_ );
		}
		void init( const char* p_, const char* endP_ = 0 )//endP == 0 means that p is null-terminated
		{
			p = p_;
			endP = endP_ ? endP_ : p + strlen( p );
			if( *p == Props::_utf8Prefix )
			{
				mode = true;
				++p;
			}
			else
				mode = false;
		}
		PUNICHAR_RET nextChar()//returns 0 if end of string found (just like when encountered terminating 0), -1 for incomplete char, -2 in case of error
		{
			if( mode )
				return PUtf8String::_nextChar( p, endP );
			else
			{
				PASSERT4( p <= endP );
				if( p == endP )
					return 0;

				PUNICHAR uni;
				PUNICHAR* dst = &uni;
				const BYTE* src = (const BYTE*)p;//cast from char to BYTE reflects codepage single-symbol-per-byte encoding
				const BYTE* srcEnd = (const BYTE*)endP;//cast from char to BYTE reflects codepage single-symbol-per-byte encoding
				int n = Props::_staticReadChars( dst, &uni + 1, src, srcEnd );
				p = (const char*)src;
				if( n == 0 )
					return -2;
				else
				{
					PASSERT5( n == 1 );
					return uni;
				}
			}
		}

		bool operator <( const StringParser& other ) const
		{
			PASSERT4( mode == other.mode );
			PASSERT4( endP == other.endP );
			return p < other.p;
		}
		bool operator ==( const StringParser& other ) const
		{
			PASSERT4( mode == other.mode );
			PASSERT4( endP == other.endP );
			return p == other.p;
		}
	};

public:
	//IMPORTANT: when feeding a PString/const char* to any of PBackwardCompatibleUniString's constructors/functions, 
	//			 they MUST be valid PBackwardCompatibleUniString strings (which is always valid for ASCII-only strings though)
	//           otherwise PASSERT4 will happen; to validate strings, use isValidString()
	PBackwardCompatibleUniString()
	{
	}
	PBackwardCompatibleUniString( const PBackwardCompatibleUniString& other )
		: PString( other )
	{
	}
	PBackwardCompatibleUniString( const PString& other )
	{
		assign( other );
	}
	PBackwardCompatibleUniString( const char* s )
	{
		assign( s );
	}
	PBackwardCompatibleUniString( const char* s, PSTR_SIZE_T sz )
	{
		assign( s, sz );
	}
	PBackwardCompatibleUniString( const char* begin, const char* end )
	{
		assign( begin, end );
	}
	PBackwardCompatibleUniString( pair< bool, const char* > begin, pair< bool, const char* > end )
	{
		PASSERT4( begin.first == end.first );
		PASSERT4( end.second != NULL );//to enforce compatibility with PString
		_assign( begin, end, begin.first );
	}
	PBackwardCompatibleUniString& operator=( const PBackwardCompatibleUniString& other )
	{
		return assign( other );
	}
	PBackwardCompatibleUniString& operator=( const PString& other )
	{
		return assign( other );
	}
	PBackwardCompatibleUniString& operator=( const char* s )
	{
		return assign( s );
	}
	void moveFrom( PBackwardCompatibleUniString& other )
	{
		_Parent::moveFrom( other );
	}

	PBackwardCompatibleUniString& assign( const PBackwardCompatibleUniString& other )
	{
		_Parent::assign( other );
		return *this;
	}
	PBackwardCompatibleUniString& assign( const PString& other )
	{
		return _assign( other.c_str(), NULL );
	}
	PBackwardCompatibleUniString& assign( const char* s )
	{
		return _assign( s, NULL );
	}
	PBackwardCompatibleUniString& assign( const char* s, PSTR_SIZE_T sz )
	{
		return _assign( s, s + sz );
	}
	PBackwardCompatibleUniString& assign( const char* begin, const char* end )
	{
		PASSERT4( end != NULL );//to enforce compatibility with PString
		return _assign( begin, end );
	}

	PBackwardCompatibleUniString& append( const PBackwardCompatibleUniString& other )
	{//cannot use _Parent::append here
		return _append( other.c_str(), NULL );
	}
	PBackwardCompatibleUniString& append( const PString& other )
	{
		return _append( other.c_str(), NULL );
	}
	PBackwardCompatibleUniString& append( const char* s )
	{
		return _append( s, NULL );
	}
	PBackwardCompatibleUniString& append( const char* begin, const char* end )
	{
		return _append( begin, end );
	}
	PBackwardCompatibleUniString& append( const char* s, PSTR_SIZE_T sz )
	{
		return _append( s, s + sz );
	}
	PBackwardCompatibleUniString& append( PUNICHAR c )
	{
		staticAppend( *this, c );
		return *this;
	}

	PBackwardCompatibleUniString& appendInt( int i )
	{
		_Parent::appendInt( i );
		return *this;
	}
	PBackwardCompatibleUniString& appendUint( UINT32 i )
	{
		_Parent::appendUint( i );
		return *this;
	}
	PBackwardCompatibleUniString& appendInt64( INT64 i )
	{
		_Parent::appendInt64( i );
		return *this;
	}
	PBackwardCompatibleUniString& appendUint64( UINT64 i )
	{
		_Parent::appendUint64( i );
		return *this;
	}
	PBackwardCompatibleUniString& appendHexInt( int i )
	{
		_Parent::appendHexInt( i );
		return *this;
	}
	PBackwardCompatibleUniString& appendHexInt64( UINT64 i )
	{
		_Parent::appendHexInt64( i );
		return *this;
	}

	//no cut() to avoid confusion; use cutAtChar() instead
	void cutAtChar( PSTR_SIZE_T len );

	void toUpper()//affects ASCII only
	{
		_Parent::toUpper();//sic - will work for both 'default' strings and UTF-8 (see comment in PUtf8String re. UTF-8)
	}
	void toLower()//affects ASCII only
	{
		_Parent::toLower();//sic - will work for both 'normal' strings and UTF-8 (see comment in PUtf8String re. UTF-8)
	}

	using _Parent::c_str;

	operator const char*() const
	{
		return c_str();
	}

	//no _Parent::operator[] to avoid confusion; use charAt() instead:
	PUNICHAR charAt( PSTR_SIZE_T idx ) const
	{
		const char* p = _Parent::c_str();
		if( *p == Props::_utf8Prefix )
			return PUtf8String::charAt( p + 1, idx );
		else
			return _Parent::charAt( idx );
	}
	int charLength() const
	{
		return charLength( c_str() );
	}

	bool equals( const char* s ) const//non-lexicographical
	{
		return charCompare( c_str(), s ) == 0;
	}
	bool equalsIgnoreCase( const char* s ) const
	{
		return charCompareIgnoreCase( c_str(), s ) == 0;
	}

	bool startsWith( const char* s ) const
	{
		return charStartsWith( c_str(), s );//seems Ok
	}
	bool endsWith( const char* s ) const
	{
		return charEndsWith( c_str(), s );//seems Ok
	}

	pair< bool, const char* > ltrim() const
	{
		return charLtrim( c_str() );
	}
	pair< bool, const char* > rtrim() const
	{
		return charRtrim( c_str() );
	}

public:
	static bool isValidString( const char* s, const char* end = 0 );//end == 0 means that s is null-terminated
	static int charLength( const char* s )
	{
		if( *s == Props::_utf8Prefix )
			return PUtf8String::charLength( s + 1 );
		else
			return static_cast< int >( strlen( s ) );
	}
	static const char* charPtr( const char* s, PSTR_SIZE_T charNum )//NOT always returns s + charNum :-)
	{
		if( *s == Props::_utf8Prefix )
			return PUtf8String::charPtr( s + 1, charNum );
		else
			return s + charNum;
	}

	static int charCompare( const char* s, const char* s2 )//NOT lexicographical order, just _some_ strict weak order
	{
		return _Parent::compare( s, s2 );//stands for non-lexicographical comparison; assumes no non-canonical strings on input
	}
	static int charCompareIgnoreCase( const char* s, const char* s2 )//ignores case for ASCII only
	{
		return _Parent::compareIgnoreCase( s, s2 );//sic - will work for both 'default' and UTF-8 strings; see comment in PUtf8String for UTF-8; assumes no non-canonical strings on input
	}

	static bool charStartsWith( const char* s, const char* s2 )
	{//assumes no non-canonical strings on input
		if( *s == Props::_utf8Prefix )
			return PUtf8String::charStartsWith( s + 1, s2 );
		else
			return _Parent::startsWith( s, s2 );
	}
	static bool charEndsWith( const char* s, const char* s2 )
	{//assumes no non-canonical strings on input
		if( *s == Props::_utf8Prefix )
			return PUtf8String::charEndsWith( s + 1, s2 );
		else
			return _Parent::endsWith( s, s2 );
	}

	static pair< bool, const char* > charLtrim( const char* s )
	{
		if( *s == Props::_utf8Prefix )
			return pair< bool, const char* >( true, PUtf8String::charLtrim( s + 1 ) );
		else
			return pair< bool, const char* >( false, _Parent::ltrim( s ) );
	}
	static pair< bool, const char* > charRtrim( const char* s )
	{
		if( *s == Props::_utf8Prefix )
			return pair< bool, const char* >( true, PUtf8String::charRtrim( s + 1 ) );
		else
			return pair< bool, const char* >( false, _Parent::rtrim( s ) );
	}
	static bool isUtf8( const char* s )
	{
		return ( *s == Props::_utf8Prefix );
	}

	static void staticAppend( PString& s, PUNICHAR_32 c );    // PYR-50664 change PUNICHAR to PUNICHAR_32

	static void staticAppendUtf8( PString& s, const char* begin, const char* end = nullptr );

	static void i18nToUpper( PString& s );
	static void i18nToLower( PString& s );

private://helpers
	PBackwardCompatibleUniString& _assign( const char* begin, const char* end, bool enforceBeginAsUtf8 = false );
	PBackwardCompatibleUniString& _append( const char* begin, const char* end, bool enforceBeginAsUtf8 = false );
	void _convertToUtf8()
	{
		_staticConvertToUtf8( *this );
	}
	static void _staticConvertToUtf8( PString& s );
};

template< class Props >
/* static */ bool PBackwardCompatibleUniString< Props >::isValidString( const char* s, const char* end )//end == 0 means that s is null-terminated
{
	if( *s != Props::_utf8Prefix )
		return true;
	else
		return PUtf8String::isValidString( s + 1, end );
}

template< class Props >
PBackwardCompatibleUniString< Props >& PBackwardCompatibleUniString< Props >::_assign( const char* begin, const char* end, bool enforceBeginAsUtf8 )
{
	if( _Parent::length() )
		_Parent::cut( 0 );
	return _append( begin, end, enforceBeginAsUtf8 );
}

template< class Props >
PBackwardCompatibleUniString< Props >& PBackwardCompatibleUniString< Props >::_append( const char* begin, const char* end, bool enforceBeginAsUtf8 )
{
	if( end && end - begin <= 0 )
		return *this;
	else if( !end && *begin == 0 )
		return *this;

	bool ok = isValidString( begin, end );
	PASSERT4( ok );
	UNUSED( ok );

	char c0 = *_Parent::c_str();
	if( c0 == Props::_utf8Prefix )
	{
		if( *begin == Props::_utf8Prefix )
		{//both are UTF-8
			PUtf8String::_staticAppend( *this, begin + 1, end );
		}
		else if( enforceBeginAsUtf8 )
		{//both are UTF-8
			PUtf8String::_staticAppend( *this, begin, end );
		}
		else
		{//convert to UTF-8
			if( end == NULL )
				end = begin + strlen( begin );
			while( begin < end )
			{
				//from char to PUNICHAR
				PUNICHAR uni;
				PUNICHAR* dst = &uni;
				const BYTE* src = (const BYTE*)begin;//cast from char to BYTE reflects codepage single-symbol-per-byte encoding
				const BYTE* srcEnd = (const BYTE*)end;//cast from char to BYTE reflects codepage single-symbol-per-byte encoding
				int n = Props::_staticReadChars( dst, &uni + 1, src, srcEnd );
				begin = (const char*)src;
				PASSERT5( n == 1 );
				UNUSED( n );
				//from PUNICHAR to UTF-8
				PUtf8String::staticAppend( *this, uni );
			}
		}
	}
	else
	{
		if( *begin != Props::_utf8Prefix && !enforceBeginAsUtf8 )
		{//none is UTF-8
			if( end == NULL )
				_Parent::append( begin );
			else
				_Parent::append( begin, end );
		}
		else
		{
			const char* begin1 = begin;
			if( *begin == Props::_utf8Prefix )
				begin1 = begin + 1;

			//check whether src can be 'canonized'
			bool isDefaultEncoding = true;
			const char* _p = begin1;
			for(;;)
			{
				PUNICHAR_RET ch = PUtf8String::_nextChar( _p, end );
				PASSERT4( ch >= 0 );
				if( ch <= 0 )
					break;
				int ch2 = Props::_staticWriteChar( ch );
				if( ch2 < 0 )
				{
					isDefaultEncoding = false;
					break;
				}
			}

			if( isDefaultEncoding )
			{
				_p = begin1;
				for(;;)
				{
					PUNICHAR_RET ch = PUtf8String::_nextChar( _p, end );
					PASSERT4( ch >= 0 );
					if( ch <= 0 )
						break;
					int ch2 = Props::_staticWriteChar( ch );
					PASSERT5( ch2 > 0 );
					PASSERT5( ch2 <= 255 );
					_Parent::append( (char)ch2 );//cast is valid, see PASSERT's above
				}
			}
			else
			{
				//converting *this to UTF-8
				_convertToUtf8();

				//appending UTF-8 to UTF-8
				PUtf8String::_staticAppend( *this, begin1, end );
			}
		}
	}

	return *this;
}

template< class Props >
void PBackwardCompatibleUniString< Props >::_staticConvertToUtf8( PString& s )
{
	//converting *this to UTF-8
	PString tmp;
	tmp.append( (char)Props::_utf8Prefix );
	const char* p = s.c_str();
	const char* pEnd = p + s.length();
	while( p < pEnd )
	{
		//from char to PUNICHAR
		PUNICHAR uni;
		PUNICHAR* dst = &uni;
		const BYTE* src = (const BYTE*)p;//cast from char to BYTE reflects codepage single-symbol-per-byte encoding
		const BYTE* srcEnd = (const BYTE*)pEnd;//cast from char to BYTE reflects codepage single-symbol-per-byte encoding
		int n = Props::_staticReadChars( dst, &uni + 1, src, srcEnd );
        UNUSED(n);
		p = (const char*)src;
		PASSERT5( n == 1 );

		//from PUNICHAR to UTF-8
		PUtf8String::staticAppend( tmp, uni );
	}

	s.moveFrom( tmp );
}

template< class Props >
/* static */ void PBackwardCompatibleUniString< Props >::staticAppend( PString& s, PUNICHAR_32 c )    // PYR-50664 change PUNICHAR to PUNICHAR_32
{
	PASSERT4( c );
	char c0 = *s.c_str();
	if( c0 == Props::_utf8Prefix )
	{
		PUtf8String::staticAppend( s, c );
	}
	else
	{
		//check whether c fits into default encoding
		int ch2 = Props::_staticWriteChar( c );
		if( ch2 >= 0 )
		{
			PASSERT5( ch2 > 0 );
			PASSERT5( ch2 <= 255 );
			s.append( (char)ch2 );//cast is valid, see PASSERT's above
		}
		else
		{
			_staticConvertToUtf8( s );
			PUtf8String::staticAppend( s, c );
		}
	}
}

template< class Props >
/* static */ void PBackwardCompatibleUniString< Props >::staticAppendUtf8( PString& s, const char* begin, const char* end )
{
	char c0 = *s.c_str();
	if( c0 != Props::_utf8Prefix )
	{
		_staticConvertToUtf8( s );
	}

	PUtf8String::_staticAppend( s, begin, end );
}

template< class Props >
void PBackwardCompatibleUniString< Props >::cutAtChar( PSTR_SIZE_T len )
{
	PASSERT4( len >= 0 );
	const char* p0 = c_str();
	const char* p = charPtr( p0, len );
	_Parent::cut( p - p0 );
}

template< class Props >
/* static */ void PBackwardCompatibleUniString< Props >::i18nToUpper( PString& s )
{
	PString tmp;
	StringParser parser;
	parser.init(s.c_str(), 0);
	for(PUNICHAR_RET c = parser.nextChar(); c > 0; c = parser.nextChar())
		staticAppend(tmp, i18nCharToUpperCS(c));
	s.moveFrom( tmp );
}

template< class Props >
/* static */ void PBackwardCompatibleUniString< Props >::i18nToLower( PString& s )
{
	PString tmp;
	StringParser parser;
	parser.init(s.c_str(), 0);
	for(PUNICHAR_RET c = parser.nextChar(); c > 0; c = parser.nextChar())
		staticAppend(tmp, i18nCharToLowerCS(c));
	s.moveFrom( tmp );
}

#endif
