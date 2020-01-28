#include "ppi18n.h"

/* virtual */ int PStreamEncodingAscii::_readChars( PUNICHAR*& dst, PUNICHAR* dstEnd, const BYTE*& src, const BYTE* srcEnd ) const
{
	PASSERT4( src <= srcEnd );
	PASSERT4( dst <= dstEnd );

	int ret = 0;
	if( isNaive )
	{
		for( ; src < srcEnd && dst < dstEnd ; ++src, ++dst, ++ret )
			*dst = (PUNICHAR)*src;
	}
	else
	{
		for( ; src < srcEnd && dst < dstEnd ; ++src, ++dst, ++ret )
		{
			PUNICHAR c = (PUNICHAR)*src;
			if( c >= 32 && c <= 127 )
				*dst = c;
			else
				throw PInternalError( PString( "PStreamEncodingAscii: unexpected symbol 0x" ).appendHexInt( c ) );
		}
	}

	return ret;
}

//****************************************************************************

/* virtual */ void PStringEncodingAscii::_AsciiStringParserImpl::init( const char* p_, const char* endP_ )//endP_ == NULL means that p_ is null-terminated
{
	p = p_;
	endP = endP_ ? endP_ : p_ + strlen( p_ );
}

/* virtual */ PUNICHAR_RET PStringEncodingAscii::_AsciiStringParserImpl::nextChar()//returns 0 if end of string found (just like when encountered terminating 0), -1 for incomplete char, -2 in case of error
{
	if( p < endP )
		return *p++;
	else
	{
		PASSERT4( p == endP );
		return 0;
	}
}

/* virtual */ int PStringEncodingAscii::_AsciiStringParserImpl::compare( const _StringParserImpl& other_ ) const
{
	//~ #ifndef PNORTTI
	//~ PASSERT4( dynamic_cast< const _AsciiStringParserImpl* >( &other_ ) != 0 );
	//~ #endif
	const _AsciiStringParserImpl& other = static_cast< const _AsciiStringParserImpl& >( other_ );
	PASSERT4( endP == other.endP );
	if( p == other.p )
		return 0;
	return p < other.p ? -1 : 1;
}

/* virtual */ void PStringEncodingAscii::_AsciiStringParserImpl::clone( PStringEncoding::StringParser& dst ) const
{
	void* tmp = PStringEncodingAscii::_allocParserImpl( dst, sizeof(_AsciiStringParserImpl) );
#include "ppmemtracedisable.h"
	new( tmp ) _AsciiStringParserImpl( *this );
#include "ppmemtraceenable.h"
}

//****************************************************************************

/* virtual */ void PStringEncodingAscii::createNewParser( StringParser& dst ) const
{
	void* p = _allocParserImpl( dst, sizeof(_AsciiStringParserImpl) );
#include "ppmemtracedisable.h"
	new( p ) _AsciiStringParserImpl();
#include "ppmemtraceenable.h"
}

/* virtual */ int PStringEncodingAscii::charLength( const char* s ) const
{
	return static_cast< int >( strlen( s ) );
}

/* virtual */ /* new */ PStringEncoding* PStringEncodingAscii::clone() const
{
	return new PStringEncodingAscii( *this );
}

/* virtual */ void PStringEncodingAscii::_append( PString& s, PUNICHAR_32 c ) const
{
	if( c >= 32 && c <= 127 )
	{
		s.append( (char)c );
		return;
	}

	if( c == 0 )
	{
		if( flags & FlagErrorOnNullChar )
			throw PInternalError( "PStringEncodingAscii: unexpected NULL character" );
		else if( flags & FlagDefaultCharOnNullChar )
		{
			s.append( (char)defaultChar );
			return;
		}
		else
			return;//ignore
	}

	if( flags & FlagNaive )
	{
		PASSERT4( c > 0 && c <= 255 );
		s.append( (char)c );
		return;
	}

	if( flags & FlagErrorOnUnknownChar )
		throw PInternalError( "PStringEncodingAscii: unknown character" );
	else if( flags & FlagDefaultCharOnUnknownChar )
		s.append( (char)defaultChar );
	//else ignore
}

/* virtual */ void PStringEncodingAscii::preparePMsg(PString& ret, const char* msg, int argc, const char* argv[]) const
{//not implemented now and not sure whether should ever be implemented
	PASSERT( 0 );
}
