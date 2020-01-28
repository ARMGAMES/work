#ifndef ppi18n_h_included
#define ppi18n_h_included

#include "ppinclude.h"
#include "ppstring.h"
#include "ppstream.h"
#include "pperror.h"

//****************************************************************************

typedef UINT16 PUNICHAR;
//PUNICHAR may need to be changed to UINT32 in the future
typedef UINT32 PUNICHAR_32;  // PYR-50664, to handle the unicode character whose code point exceeds 0xFFFF. N.B. PUNICHAR_MAX still be kept in UINT16 scope!!!
typedef INT32 PUNICHAR_RET;
//PUNICHAR_RET is used for return with possible error/eof; will stay as INT32 even if PUNICHAR is updated to UINT32; negative values are used for errors/eofs/...
#define PUNICHAR_MAX 0xFFFF

class PI18NError : public PError
{
public:
	PI18NError( const char* s )
		: PError( s )
	{
	}
};

//****************************************************************************

/* abstract */ class PStreamEncoding
{
public:
	enum { MaxPossibleCharSizeInBytes = 16 };//used as minimum BYTE buffer size when reading from BYTE streams; 16 should be far more than enough

public:
	virtual int _readChars( PUNICHAR*& dst, PUNICHAR* dstEnd, const BYTE*& src, const BYTE* srcEnd ) const = 0;
	//reads as much as possible from src to dst; returns # of PUNICHAR's read (may return 0)
	//in case of protocol errors throws PI18NError
	//*dst == 0 is a valid case, if putting to null-terminated strings need to be filtered out later

	virtual ~PStreamEncoding()
	{
	}
};

class PStreamEncodingAscii : public PStreamEncoding
{
private:
	bool isNaive;

public:
	PStreamEncodingAscii( bool isNaive_ = false )
	{
		isNaive = isNaive_;
	}

public://overrridden
	int _readChars( PUNICHAR*& dst, PUNICHAR* dstEnd, const BYTE*& src, const BYTE* srcEnd ) const override;
	//reads as much as possible from src to dst; returns # of PUNICHAR's read (may return 0)
	//in case of protocol errors throws PI18NError
	//*dst == 0 is a valid case, if putting to null-terminated strings need to be filtered out later
};

//****************************************************************************

/* abstract */ class PStringEncoding
	//PStringEncoding represents abstract interface to different string encodings
	//to be used primarily in generic library contexts where library does not know what kind of encoding will be used
{
public:
	enum//not all flags apply to all encodings
	{
		FlagErrorOnNullChar = 0x01,//ignore by default
		FlagDefaultCharOnNullChar = 0x02,//ignore by default
		FlagErrorOnUnknownChar = 0x04,//ignore by default
		FlagDefaultCharOnUnknownChar = 0x08,//ignore by default
		//IMPORTANT: 0x80 is reserved for PStringEncodingAscii::FlagNaive
	};

protected:
	BYTE props;//0x01 - use PString::compare; 0x02 - use PString::compareIgnoreCase, 0x04 - represents ASCII (32-127) strings 'as is', 0x08 - allowsAppendingAscii
	BYTE flags;//Flag*
	PUNICHAR defaultChar;

public:
	class StringParser;
	/* abstract */ class _StringParserImpl
	{
	public:
		virtual void init( const char* p, const char* endP ) = 0;//endP == nullptr means that p is null-terminated
		virtual PUNICHAR_RET nextChar() = 0;//returns 0 if end of string found (just like when encountered terminating 0), -1 for incomplete char, -2 in case of error
		virtual int compare( const _StringParserImpl& other ) const = 0;
		virtual void clone( StringParser& dst ) const = 0;
		//IMPORTANT: currently classes derived from _StringParserImpl MAY NOT have non-trivial destructors: to support it, will need to add explicit call of virtual destructor in places marked as (*) below
	};

	class StringParser
	{
	private:
		_StringParserImpl* impl;
		enum { _BufSize = 4 * sizeof(void*) };//typical size of complex parser
		BYTE _buf[ _BufSize ];//_buf is intended to save on allocations if size of specific _StringParserImpl is indeed <= _BufSize

	public:
		StringParser()
		{
			impl = 0;
		}
		StringParser( const StringParser& other )
		{
			impl = 0;
			PASSERT4( other.impl );
			other.impl->clone( *this );
		}
		~StringParser()
		{
			if( (void*)impl != (void*)_buf )
			{
				//(*)
				delete [] (BYTE*)(void*)impl;
			}
			//(*)
		}
		void operator =( const StringParser& other )
		{
			if( (void*)impl != (void*)_buf )
			{
				//(*)
				delete [] (BYTE*)(void*)impl;
			}
			//(*)
			impl = 0;
			other.impl->clone( *this );
		}

		void init( const char* p, const char* endP = nullptr )//endP == nullptr means that p is null-terminated
		{
			PASSERT4( impl );
			impl->init( p, endP );
		}
		PUNICHAR_RET nextChar()
		{
			PASSERT4( impl );
			return impl->nextChar();
		}
		bool operator ==( const StringParser& other ) const
		{
			PASSERT4( impl );
			return impl->compare( *other.impl ) == 0;
		}
		bool operator <( const StringParser& other ) const
		{
			PASSERT4( impl );
			return impl->compare( *other.impl ) < 0;
		}

	private:
		friend class PStringEncoding;
		void* _allocImpl( size_t sz )
		{
			PASSERT4( impl == 0 );
			void* p = sz <= sizeof(_buf) ? _buf : new BYTE[ sz ];
			impl = (_StringParserImpl*)p;
			return p;
		}
	};

public:
	PStringEncoding( BYTE props_, BYTE flags_, PUNICHAR defaultChar_ )
	{
		props = props_;
		flags = flags_;
		PASSERT4( ( ( flags & ( FlagDefaultCharOnNullChar | FlagDefaultCharOnUnknownChar ) ) == 0 ) || defaultChar_ != 0 );
		defaultChar = defaultChar_;
	}
	bool asciiAsIs() const
	{
		return ( props & 0x04 ) != 0;
	}
	bool appendsAsciiAsIs() const
	{
		return ( props & 0x08 ) != 0;
	}
	virtual ~PStringEncoding()
	{
	}

public://overridables
	virtual /* new */ PStringEncoding* clone() const = 0;

	virtual void createNewParser( StringParser& parser ) const = 0;
	virtual void _append( PString& s, PUNICHAR_32 c ) const = 0;

	void append( PString& s, PUNICHAR_32 c ) const
	{
		_append( s, c );
	}
	void append( PString& s, PUNICHAR_RET c ) const
	{
		_append( s, static_cast< PUNICHAR_32 >( c ) );
	}
	void append( PString& s, const PUNICHAR* begin, const PUNICHAR* end = nullptr ) const
	{
		if( end == nullptr )
		{
			for( ; *begin ; ++begin )
				_append( s, *begin );
		}
		else
		{
			for( ; begin < end ; ++begin )
				_append( s, *begin );
		}
	}
	void append( PString& s, const char* begin, const char* end = nullptr ) const
	{
		if( end == nullptr )
		{
			for( ; *begin ; ++begin )
				_append( s, (unsigned char)*begin );
		}
		else
		{
			for( ; begin < end ; ++begin )
				_append( s, (unsigned char)*begin );
		}
	}

public:
	void parseAppend( PString& s, const PStringEncoding& other, const char* begin, const char* end = nullptr ) const
	{
		if( begin && ( !end || ( begin < end ) ) )
		{
			StringParser parser;
			other.createNewParser( parser );
			parser.init( begin, end );

			for( auto c = parser.nextChar(); 0 < c; c = parser.nextChar() )
			{
				_append( s, c );
			}
		}
	}
	virtual void parseAppend( PString& s, const char* begin, const char* end = nullptr ) const
	{
		parseAppend( s, *this, begin, end );
	}

private://overridables; for internal use only
	virtual int _vCompare( const char* a, const char* b ) const { PASSERT4( 0 ); return 0; }
	virtual int _vCompareIgnoreCase( const char* a, const char* b ) const { PASSERT4( 0 ); return 0; }

public:
	virtual int charLength( const char* s ) const = 0;
	int compare( const char* a, const char* b ) const
		//non-lexicographical; just some strict weak ordering
	{
		if( props & 0x01 )
			return PString::compare( a, b );
		else
			return _vCompare( a, b );
	}
	int compareIgnoreCase( const char* a, const char* b ) const
		//non-lexicographical; just some strict weak ordering; ignoring case works only for ASCII
	{
		if( props & 0x02 )
			return PString::compareIgnoreCase( a, b );
		else
			return _vCompareIgnoreCase( a, b );
	}
	virtual void preparePMsg(PString& ret, const char* msg, int argc, const char* argv[]) const = 0;

protected:
	static void* _allocParserImpl( StringParser& dst, size_t sz )
	{
		return dst._allocImpl( sz );
	}
};

class PStringEncodingAscii : public PStringEncoding
{
public:
	enum
	{//IMPORTANT: FlagNaive shares bit space with PStringEncoding::Flag*
		FlagNaive = 0x80, //'naive' means 'assume encoding with low 256 UNICODE codes directly mapped to unsigned char values'; supported ONLY to be used ONLY in conjunction with 'naive' PStreamEncodingAscii
	};

public:
	/* abstract */ class _AsciiStringParserImpl : public PStringEncoding::_StringParserImpl
	{
	private:
		const char* p;
		const char* endP;

	private:
		friend class PStringEncodingAscii;
		_AsciiStringParserImpl()
		{
		}

	public:
		void init( const char* p_, const char* endP_ ) override;//endP_ == nullptr means that p_ is null-terminated;
		PUNICHAR_RET nextChar() override;//returns 0 if end of string found (just like when encountered terminating 0), -1 for incomplete char, -2 in case of error
		int compare( const _StringParserImpl& other_ ) const override;
		void clone( PStringEncoding::StringParser& dst ) const override;
	};

public:
	friend class _AsciiStringParserImpl;
	PStringEncodingAscii( BYTE flags_ = 0, char defaultChar_ = 0 )
		: PStringEncoding( 0x0F, flags_, defaultChar_ )
	{
		//props = 0x0F;
		//PString::compare and PString::compareIgnoreCase stand for UTF-8; see explanation in PUtf8String
		//DOES represent ASCII strings 'as is' and DOES allow appending ASCII strings 'as is'		 
	}

public:
	/* new */ PStringEncoding* clone() const override;
	void createNewParser( StringParser& dst ) const override;
	int charLength( const char* s ) const override;

	void _append( PString& s, PUNICHAR_32 c ) const override;
	void preparePMsg(PString& ret, const char* msg, int argc, const char* argv[]) const override;//not implemented
};

//****************************************************************************

/* abstract */ class PUniInputStream
{
public:
	virtual size_t read( PUNICHAR* buf, size_t sz ) = 0;
	virtual ~PUniInputStream() {};
};

/* abstract */ class PUniOutputStream
{
public:
	virtual void write( const PUNICHAR* buf, size_t sz ) = 0;
	virtual ~PUniOutputStream() {};
};

class PUniInputStreamFromStream : public PUniInputStream
{
private:
	PInputStream& s;
	const PStreamEncoding& enc;
	int inputBufSz;
	int inputBufEncodedSz;

	//	bool sFailed;
	/* new */ BYTE* inputBuf;//'linear' buffer
	BYTE* inputBufPtr;
	BYTE* inputBufEnd;

	/* new */ PUNICHAR* inputBufEncoded;//'linear' buffer
	PUNICHAR* inputBufEncodedPtr;
	PUNICHAR* inputBufEncodedEnd;

public:
	PUniInputStreamFromStream( PInputStream& s_, const PStreamEncoding& enc_, int inputBufSz_ = 256, int inputBufEncodedSz_ = 256 );
	~PUniInputStreamFromStream();

public:
	size_t read( PUNICHAR* buf, size_t sz ) override;

private: // prohibit default assignment operator
	PUniInputStreamFromStream& operator=( const PUniInputStreamFromStream& other );
};

class PUniInputStreamFromString : public PUniInputStream
{
private:
	const PStringEncoding& enc;
	PStringEncoding::StringParser parser;

public:
	PUniInputStreamFromString( const char* s, const PStringEncoding& enc_ );
	PUniInputStreamFromString( const char* s, const char* eos, const PStringEncoding& enc_ );
	~PUniInputStreamFromString();

public:
	size_t read( PUNICHAR* buf, size_t sz ) override;

private: // prohibit default assignment operator
	PUniInputStreamFromString& operator=( const PUniInputStreamFromString& other );
};

class PUniInputStreamFromUniString : public PUniInputStream
{
private:
	const PUNICHAR* s;

public:
	PUniInputStreamFromUniString( const PUNICHAR* s_ );
	~PUniInputStreamFromUniString();

public:
	size_t read( PUNICHAR* buf, size_t sz ) override;
};

class PUniCharInputStream
{
private:
	PUniInputStream& s;
	PBlock /* of PUNICHAR */ ungetBuf;//'linear' buffer

public:
	PUniCharInputStream( PUniInputStream& s_ );
	~PUniCharInputStream();

public:
	PUNICHAR_RET get();
	void unget( PUNICHAR_RET b );
	//b < 0 is Ok, will be ignored
	//if called several times, should be called in LIFO order (compared to get() order).

private: // prohibit default assignment operator
	PUniCharInputStream& operator=( const PUniCharInputStream& other );
};

template< class UniCharInputStream >
void PSkipUntil( UniCharInputStream& f, PUNICHAR c )
{
	for(;;)
	{
		PUNICHAR_RET cc = f.get();
		if( cc < 0 || cc == c )
			return;
	}
}

inline bool _PSkipUntilEq( const PUNICHAR* p, const char* pattern, int nn )//PSkipUntil implementation helper
{
	for( int i=0; i < nn ; ++i, ++p, ++pattern )
		if( *p != *pattern )
			return false;
	return true;
}

template< class UniCharInputStream >
void PSkipUntil( UniCharInputStream& f, const char* pattern )//sic - for now ASCII only
{
	int n = static_cast< int >( strlen( pattern ) );
	PBlock buf( n * sizeof(PUNICHAR) );
	PUNICHAR* ptr = (PUNICHAR*)buf.ptr();
	for( int i=0; i < n ; ++i )
	{
		PUNICHAR_RET c = f.get();
		if( c < 0 )
			return;
		ptr[ i ] = (PUNICHAR)c;
	}
	PUNICHAR* p = ptr;

	for(;;)
	{
		int nn = static_cast< int >( ptr + n - p );
		if( _PSkipUntilEq( p, pattern, nn ) &&
			_PSkipUntilEq( ptr, pattern + nn, n - nn ) )
			return;
		PUNICHAR_RET c = f.get();
		if( c < 0 )
			return;
		*p = (PUNICHAR)c;
		if( ++p >= ptr + n )
			p = ptr;
	}
}

class PUniOutputStreamToPStringX : public PUniOutputStream
{
	PString& pStr;
public:
	PUniOutputStreamToPStringX( PString& pStr_ )
		: pStr( pStr_ ) {}
	void write( const PUNICHAR* buf, size_t sz ) override;

private: // prohibit default assignment operator
	PUniOutputStreamToPStringX& operator=( const PUniOutputStreamToPStringX& other );
};

#endif
