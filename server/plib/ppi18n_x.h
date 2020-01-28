#ifndef ppi18n_x_h_included
#define ppi18n_x_h_included

#include "ppincludebase.h"
#include "ppstring.h"
#include "ppi18n.h"
#include "ppi18n_bcustring.h"
#include "ppi18n_utf8.h"
#include "ppi18n_codepage.h"

struct _PStringX_Props
{
	enum { _utf8Prefix = 0x10 };
	static int _staticReadChars( PUNICHAR*& dst, PUNICHAR* dstEnd, const BYTE*& src, const BYTE* srcEnd )
	{
		return PStreamEncodingWin1252::_staticReadChars( dst, dstEnd, src, srcEnd );
	}
	static int _staticWriteChar( PUNICHAR_32 c )
	{
		return PStreamEncodingWin1252::_staticWriteChar( c );
	}
};

typedef PBackwardCompatibleUniString< _PStringX_Props > PStringX;

class PStringEncodingPStringX : public PStringEncoding
{
	/* abstract */ class _PStringXStringParserImpl : public PStringEncoding::_StringParserImpl
	{
	private:
		PStringX::StringParser parser;

	private:
		friend class PStringEncodingPStringX;
		_PStringXStringParserImpl()
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
			//~ PASSERT4( dynamic_cast< const _PStringXStringParserImpl* >( &other_ ) != 0 );
			//~ #endif
			const _PStringXStringParserImpl& other = static_cast< const _PStringXStringParserImpl& >( other_ );
			if( parser == other.parser )
				return 0;
			return parser < other.parser ? -1 : 1;
		}
		void clone( PStringEncoding::StringParser& dst ) const override
		{
			void* p = PStringEncodingPStringX::_allocParserImpl( dst, sizeof(_PStringXStringParserImpl) );
#include "ppmemtracedisable.h"
			new( p ) _PStringXStringParserImpl( *this );
#include "ppmemtraceenable.h"
		}
	};

public:
	friend class _PStringXStringParserImpl;
	PStringEncodingPStringX( BYTE flags_ = 0, PUNICHAR defaultChar_ = 0 )//NOTE: Flag*OnUnknownChar are irrelevant and will be ignored
		: PStringEncoding( 0x0F, flags_, defaultChar_ )
	{
		//props = 0x0F;
		//PString::compare and PString::compareIgnoreCase stand for PStringX; see explanation in PStringX
		//DOES represent ASCII strings 'as is' and DOES allow appending ASCII strings 'as is'
	}
	void createNewParser( StringParser& dst ) const override
	{
		void* p = _allocParserImpl( dst, sizeof(_PStringXStringParserImpl) );
#include "ppmemtracedisable.h"
		new( p ) _PStringXStringParserImpl();
#include "ppmemtraceenable.h"
	}
	int charLength( const char* s ) const override
	{
		return PStringX::charLength( s );
	}

public://overridden
	/* new */ PStringEncoding* clone() const override
	{
		return new PStringEncodingPStringX( *this );
	}

	void _append( PString& s, PUNICHAR_32 c ) const override;
	void preparePMsg(PString& ret, const char* msg, int argc, const char* argv[]) const override;
};

#endif
