#ifndef ppi18n_utf8_h_included
#define ppi18n_utf8_h_included

#include "ppincludebase.h"
#include "ppstring.h"
#include "ppi18n.h"

class PUtf8String : private PString
{
private:
	typedef PString _Parent;

public:
	class StringParser
	{
	private:
		const char* p;
		const char* endP;

	public:
		StringParser()
		{
			p = endP = NULL;
		}
		StringParser( const char* p_, const char* endP_ = NULL )
		{
			init( p_, endP_ );
		}
		void init( const char* p_, const char* endP_ = NULL )//endP == NULL means that p is null-terminated
		{
			p = p_;
			endP = endP_;
		}
		PUNICHAR_RET nextChar()//returns 0 if end of string found (just like when encountered terminating 0), -1 for incomplete char, -2 in case of error
		{
			return PUtf8String::_nextChar( p, endP );
		}

		bool operator ==( const StringParser& other ) const
		{
			PASSERT4( endP == other.endP );
			return p == other.p;
		}
		bool operator <( const StringParser& other ) const
		{
			PASSERT4( endP == other.endP );
			return p < other.p;
		}
	};

public:
	//IMPORTANT: when feeding a PString/const char* to any of PUtf8String's constructors/functions, 
	//			 they MUST be valid UTF-8 strings (which is always valid for ASCII-only strings though)
	//           otherwise PASSERT4 will happen; to validate strings, use isValidString()
	PUtf8String()
	{
	}
	PUtf8String( const PUtf8String& other )
		: PString( other )
	{
	}
	PUtf8String( const PString& other )
	{
		assign( other );
	}
	PUtf8String( const char* s )
	{
		assign( s );
	}
	PUtf8String( const char* s, PSTR_SIZE_T sz )
	{
		assign( s, sz );
	}
	PUtf8String( const char* begin, const char* end )
	{
		assign( begin, end );
	}
	PUtf8String& operator=( const PUtf8String& other )
	{
		return assign( other );
	}
	PUtf8String& operator=( const PString& other )
	{
		return assign( other );
	}
	PUtf8String& operator=( const char* s )
	{
		return assign( s );
	}
	void moveFrom( PUtf8String& other )
	{
		_Parent::moveFrom( other );
	}

	PUtf8String& assign( const PUtf8String& other )
	{
		_Parent::assign( other );
		return *this;
	}
	PUtf8String& assign( const PString& other )
	{
		return _assign( other.c_str(), NULL );
	}
	PUtf8String& assign( const char* s )
	{
		return _assign( s, NULL );
	}
	PUtf8String& assign( const char* s, PSTR_SIZE_T sz_ )
	{
		return _assign( s, s + sz_ );
	}
	PUtf8String& assign( const char* begin, const char* end )
	{
		PASSERT4( end != NULL );//to enforce compatibility with PString
		return _assign( begin, end );
	}

	PUtf8String& append( const PUtf8String& other )
	{
		_Parent::append( other.c_str() );
		return *this;
	}
	PUtf8String& append( const PString& other )
	{
		return _append( other.c_str(), NULL );
	}
	PUtf8String& append( const char* s )
	{
		return _append( s, NULL );
	}
	PUtf8String& append( const char* begin, const char* end )
	{
		return _append( begin, end );
	}
	PUtf8String& append( const char* s, PSTR_SIZE_T sz_ )
	{
		return _append( s, s + sz_ );
	}
	PUtf8String& append( PUNICHAR_32 c );    // PYR-50664 change PUNICHAR to PUNICHAR_32

	PUtf8String& appendInt( int i )
	{
		_Parent::appendInt( i );
		return *this;
	}
	PUtf8String& appendUint( UINT32 i )
	{
		_Parent::appendUint( i );
		return *this;
	}
	PUtf8String& appendInt64( INT64 i )
	{
		_Parent::appendInt64( i );
		return *this;
	}
	PUtf8String& appendUint64( UINT64 i )
	{
		_Parent::appendUint64( i );
		return *this;
	}
	PUtf8String& appendHexInt( int i )
	{
		_Parent::appendHexInt( i );
		return *this;
	}
	PUtf8String& appendHexInt64( UINT64 i )
	{
		_Parent::appendHexInt64( i );
		return *this;
	}

	//no cut() to avoid confusion; use cutAtChar() instead
	void cutAtChar( PSTR_SIZE_T len );

	void toUpper()//affects ASCII only
	{
		_Parent::toUpper();//sic - will work for UTF-8 because (a) _Parent doesn't affect non-ASCII, and (b) the way ASCII/non-ASCII represented in UTF-8
	}
	void toLower()//affects ASCII only
	{
		_Parent::toLower();//sic - will work for UTF-8 because (a) _Parent doesn't affect non-ASCII, and (b) the way ASCII/non-ASCII represented in UTF-8
	}

	using _Parent::c_str;
	using _Parent::reserve;

	operator const char*() const
	{
		return c_str();
	}

	//no _Parent::operator[] to avoid confusion; use charAt() instead:
	PUNICHAR_32 charAt( PSTR_SIZE_T idx ) const    // PYR-50664 change PUNICHAR to PUNICHAR_32
	{
		return charAt( c_str(), idx );
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
		return charStartsWith( c_str(), s );
	}
	bool endsWith( const char* s ) const
	{
		return charEndsWith( c_str(), s );
	}

	const char* ltrim() const
	{
		return charLtrim( c_str() );
	}
	const char* rtrim() const
	{
		return charRtrim( c_str() );
	}

public:
	static bool isValidString( const char* s, const char* end = 0 );//end == 0 means that s is null-terminated
	static int charLength( const char* s );
	static const char* charPtr( const char* s, PSTR_SIZE_T charNum );//NOT always returns s + charNum :-)
	static PUNICHAR_32 charAt( const char* s, PSTR_SIZE_T idx );    // PYR-50664 change PUNICHAR to PUNICHAR_32
	static int lengthOfChar( PUNICHAR_32 c );    // PYR-50664 change PUNICHAR to PUNICHAR_32

	static int charCompare( const char* s, const char* s2 )//NOT lexicographical order, just _some_ strict weak order
	{
		return _Parent::compare( s, s2 );//stands for non-lexicographical comparison
	}
	static int charCompareIgnoreCase( const char* s, const char* s2 )//ignores case for ASCII only
	{
		return _Parent::compareIgnoreCase( s, s2 );//sic - will work for UTF-8 because (a) _Parent doesn't affect non-ASCII, and (b) the way ASCII/non-ASCII represented in UTF-8
	}

	static bool charStartsWith( const char* s, const char* s2 )
	{
		return _Parent::startsWith( s, s2 );//seems Ok
	}
	static bool charEndsWith( const char* s, const char* s2 )
	{
		return _Parent::endsWith( s, s2 );//seems Ok
	}

	static const char* charLtrim( const char* s );
	static const char* charRtrim( const char* s );

	static void staticAppend( PString& s, PUNICHAR_32 c );    // PYR-50664 change PUNICHAR to PUNICHAR_32

	static PUNICHAR_RET _nextChar( const char*& p, const char* endP = 0 );
	//endP == 0 means that p is null-terminated
	//returns 0 if end of string found (just like when encountered terminating 0), -1 in case of incomplete char, -2 in case of error
	static void _staticAppend( PString& s, const char* begin, const char* end );//WARNING: assumes that string is already UTF-8 and don't check anything more

	static void i18nToUpper( PString& s );
	static void i18nToLower( PString& s );
	static void i18nToLowerNoAsserts( PString& s );

	static bool _genChar( char*& p, const char* endP, PUNICHAR_32 c );//returns false if not enough space in p    // PYR-50664 change PUNICHAR to PUNICHAR_32

private:
	PUtf8String& _assign( const char* begin, const char* end );
	PUtf8String& _append( const char* begin, const char* end );
};

class PStreamEncodingUtf8 : public PStreamEncoding //an example of 'non-1-symbol' encoding
{
public:
	PStreamEncodingUtf8() {}
	int _readChars( PUNICHAR*& dst, PUNICHAR* dstEnd, const BYTE*& src, const BYTE* srcEnd ) const override;
};

class PStringEncodingUtf8 : public PStringEncoding
{
public:
	/* abstract */ class _Utf8StringParserImpl : public PStringEncoding::_StringParserImpl
	{
	private:
		PUtf8String::StringParser parser;

	private:
		friend class PStringEncodingUtf8;
		_Utf8StringParserImpl()
		{
		}

	public:
		void init( const char* p, const char* endP ) override//endP == NULL means that p is null-terminated
		{
			parser.init( p, endP );
		}
		PUNICHAR_RET nextChar() override//returns 0 if end of string found (just like when encountered terminating 0), -1 for incomplete char, -2 in case of error
		{
			return parser.nextChar();
		}

		int compare( const _StringParserImpl& other_ ) const override
		{
			//~ #ifndef PNORTTI
			//~ PASSERT4( dynamic_cast< const _Utf8StringParserImpl* >( &other_ ) != 0 );
			//~ #endif
			const _Utf8StringParserImpl& other = static_cast< const _Utf8StringParserImpl& >( other_ );
			if( parser == other.parser )
				return 0;
			return parser < other.parser ? -1 : 1;
		}
		void clone( PStringEncoding::StringParser& dst ) const override
		{
			void* p = PStringEncodingUtf8::_allocParserImpl( dst, sizeof(_Utf8StringParserImpl) );
#include "ppmemtracedisable.h"
			new( p ) _Utf8StringParserImpl( *this );
#include "ppmemtraceenable.h"
		}
	};

public:
	friend class _Utf8StringParserImpl;
	PStringEncodingUtf8( BYTE flags_ = 0, PUNICHAR defaultChar_ = 0 )//NOTE: Flag*OnUnknownChar are irrelevant and will be ignored
		: PStringEncoding( 0x0F, flags_, defaultChar_ )
	{
		//props = 0x0F;
		//PString::compare and PString::compareIgnoreCase stand for UTF-8; see explanation in PUtf8String
		//DOES represent ASCII strings 'as is' and DOES allow appending ASCII strings 'as is'
	}
	void createNewParser( StringParser& dst ) const override
	{
		void* p = _allocParserImpl( dst, sizeof(_Utf8StringParserImpl) );
#include "ppmemtracedisable.h"
		new( p ) _Utf8StringParserImpl();
#include "ppmemtraceenable.h"
	}
	int charLength( const char* s ) const override
	{
		return PUtf8String::charLength( s );
	}

public://overridden
	/* new */ PStringEncoding* clone() const override
	{
		return new PStringEncodingUtf8( *this );
	}

	void _append( PString& s, PUNICHAR_32 c ) const override;

	using PStringEncoding::parseAppend;
	void parseAppend( PString& s, const char* begin, const char* end = nullptr ) const override
	{
		PUtf8String::_staticAppend( s, begin, end );
	}

	void preparePMsg(PString& ret, const char* msg, int argc, const char* argv[]) const override;
	void preparePMsg( PString& ret, const PStringEncoding* encString, const PStringEncoding* argEncString, const char* msg, int argc, const char* argv[] ) const;
};

#endif
