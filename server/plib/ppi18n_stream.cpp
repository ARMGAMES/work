#include "ppinclude.h"
#include "ppi18n.h"
#include "ppi18n_x.h"

PUniInputStreamFromStream::PUniInputStreamFromStream( PInputStream& s_, const PStreamEncoding& enc_, int inputBufSz_, int inputBufEncodedSz_ )
: s( s_ ), enc( enc_ )
{
	inputBufSz = inputBufSz_;
	inputBufEncodedSz = inputBufEncodedSz_;
	PASSERT3( inputBufSz >= PStreamEncoding::MaxPossibleCharSizeInBytes );

	inputBuf = new BYTE[ inputBufSz ];
	inputBufEncoded = new PUNICHAR[ inputBufEncodedSz ];

	inputBufPtr = inputBufEnd = inputBuf;
	inputBufEncodedPtr = inputBufEncodedEnd = inputBufEncoded;
}

PUniInputStreamFromStream::~PUniInputStreamFromStream()
{
	delete[] inputBufEncoded;
	delete[] inputBuf;
}

/* virtual */ size_t PUniInputStreamFromStream::read( PUNICHAR* buf, size_t sz )
{
	if( sz == 0 )
		return 0;

	PASSERT4( inputBufEncodedEnd >= inputBufEncodedPtr );
	if( inputBufEncodedPtr == inputBufEncodedEnd )
	{
		PASSERT4( inputBufEnd >= inputBufPtr );
		if( inputBufEnd >= inputBufPtr ) //IgorS
		{
			int sz2 = 0;
			if( inputBufEnd > inputBufPtr )
			{
				sz2 = inputBufEnd - inputBufPtr;
				if( inputBufPtr != inputBuf )
					memmove( inputBuf, inputBufPtr, sz2 );
			}

			size_t rd = s.read( inputBuf + sz2, inputBufSz - sz2 );
			if( rd == 0 )
			{
				//sFailed = true;
				//+++warn if sz2 != 0
				return 0;
			}
			inputBufPtr = inputBuf;
			inputBufEnd = inputBufPtr + sz2 + rd;
		}

		PUNICHAR* dst = inputBufEncoded;
		int nEnc = enc._readChars( dst, inputBufEncoded + inputBufEncodedSz, const_cast<const BYTE *&>(inputBufPtr), const_cast<const BYTE*>(inputBufEnd) );
		if( nEnc == 0 )
		{
			//+++warn
			return 0;
		}

		inputBufEncodedPtr = inputBufEncoded;
		inputBufEncodedEnd = inputBufEncodedPtr + nEnc;
	}

	PASSERT4( inputBufEncodedPtr < inputBufEncodedEnd );
	int sz3 = inputBufEncodedEnd - inputBufEncodedPtr;
	if( sz3 > sz )
		sz3 = sz;
	PASSERT4( sz3 );
	if( sz3 == 1 )
		*buf = *inputBufEncodedPtr++;
	else
	{
		memcpy( buf, inputBufEncodedPtr, sz3 * sizeof(PUNICHAR) );
		inputBufEncodedPtr += sz3;
	}
	return sz3;
}

//****************************************************************************

PUniInputStreamFromString::PUniInputStreamFromString( const char* s, const PStringEncoding& enc_ )
: enc( enc_ )
{
	enc.createNewParser( parser );
	parser.init( s );
}

PUniInputStreamFromString::PUniInputStreamFromString( const char* s, const char* eos, const PStringEncoding& enc_ )
: enc( enc_ )
{
	enc.createNewParser( parser );
	parser.init( s, eos );
}

PUniInputStreamFromString::~PUniInputStreamFromString()
{
}

/* virtual */ size_t PUniInputStreamFromString::read( PUNICHAR* buf, size_t sz )
{
	PUNICHAR* p = buf;
	for( size_t i=0; i < sz ; ++i, ++p )
	{
		PUNICHAR_RET ch = parser.nextChar();
		if( ch < 0 )
			throw PInternalError( "Wrong string format" );
		else if( ch == 0 )
			return i;
		*p = static_cast< PUNICHAR >( ch );
	}
	return sz;
}

//****************************************************************************

PUniInputStreamFromUniString::PUniInputStreamFromUniString( const PUNICHAR* s_ )
: s( s_ )
{
}

PUniInputStreamFromUniString::~PUniInputStreamFromUniString()
{
}

/* virtual */ size_t PUniInputStreamFromUniString::read( PUNICHAR* buf, size_t sz )
{
	size_t n = 0;
	while( *s && n < sz )
	{
		*buf++ = *s++;
		++n;
	}
	return n;
}

//****************************************************************************

PUniCharInputStream::PUniCharInputStream( PUniInputStream& s_ )
: s( s_ )
{
}

PUniCharInputStream::~PUniCharInputStream()
{
}

PUNICHAR_RET PUniCharInputStream::get()
{
	if( ungetBuf.size() > 0 )
	{
		PASSERT4( ungetBuf.size() % sizeof(PUNICHAR) == 0 );
		size_t sz = ungetBuf.size() / sizeof(PUNICHAR);
		PASSERT4( sz > 0 );
		PUNICHAR ret = *( ( (const PUNICHAR*)ungetBuf.ptr() ) + sz - 1 );
		ungetBuf.cut( ( sz - 1 ) * sizeof(PUNICHAR) );
		return ret;
	}

	PUNICHAR ch;
	size_t rd = s.read( &ch, 1 );
	if( rd == 0 )
		return -1;
	return ch;
}

void PUniCharInputStream::unget( PUNICHAR_RET b )
{
	if( b < 0 )
		return;
	*( PUNICHAR* )ungetBuf.append( sizeof( PUNICHAR ) ) = static_cast< PUNICHAR >( b );
}

void PUniOutputStreamToPStringX::write( const PUNICHAR* buf, size_t sz )
{
	for( int ch = 0; ch < sz; ++ch, ++buf)
	{
		PStringX::staticAppend(pStr, *buf);
	}
}
