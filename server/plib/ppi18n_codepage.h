#ifndef ppi18n_codepage_h_included
#define ppi18n_codepage_h_included

#include "ppincludebase.h"
#include "ppstring.h"
#include "ppi18n.h"

class PStreamEncodingCodePage : public PStreamEncoding //'1-symbol' encodings with all lower part corresponding to ASCII; cover all the Latin and Cyrillic encodings
{
public:
	struct ReverseTableItem
	{
		PUNICHAR_32 uni;
		BYTE c;

		bool operator<( const ReverseTableItem& other ) const;
	};

public:
	static int _staticReadChars( const PUNICHAR* table, PUNICHAR*& dst, PUNICHAR* dstEnd, const BYTE*& src, const BYTE* srcEnd );
	static int _staticWriteChar( const ReverseTableItem* begin, const ReverseTableItem* end, PUNICHAR_32 c )//returns -1 if unsuccessful, or (0-255) if successful
	{
		return c <= 127 ? c : _staticWriteCharPrivate( begin, end, c );
	}

private:
	static int _staticWriteCharPrivate( const ReverseTableItem* begin, const ReverseTableItem* end, PUNICHAR_32 c );//returns -1 if unsuccessful, or (0-255) if successful
};

class PStringEncodingCodePage : public PStringEncoding
	//just ignore unknown symbols; resulting PString is not even MBCS, but a string in some codepage
{
public:
	/* abstract */ class _CodePageStringParserImpl : public PStringEncoding::_StringParserImpl
	{
	private:
		const PUNICHAR* table;
		const char* p;
		const char* endP;

	private:
		friend class PStringEncodingCodePage;
		_CodePageStringParserImpl( const PUNICHAR* table_ )
		{
			table = table_;
			p = endP = NULL;
		}

	public:
		void init( const char* p_, const char* endP_ ) override//endP == NULL means that p is null-terminated
		{
			p = p_;
			endP = endP_ ? endP_ : p + strlen( p );
		}
		PUNICHAR_RET nextChar() override//returns 0 if end of string found (just like when encountered terminating 0), -1 for incomplete char, -2 in case of error
		{
			PASSERT4( p <= endP );
			if( p == endP )
				return 0;

			PUNICHAR uni;
			PUNICHAR* dst = &uni;
			const BYTE* src = (const BYTE*)p;//cast from char to BYTE reflects codepage single-symbol-per-byte encoding
			const BYTE* srcEnd = (const BYTE*)endP;//cast from char to BYTE reflects codepage single-symbol-per-byte encoding
			int n = PStreamEncodingCodePage::_staticReadChars( table, dst, &uni + 1, src, srcEnd );
			p = (const char*)src;
			if( n == 0 )
				return -2;
			else
			{
				PASSERT5( n == 1 );
				return uni;
			}
		}
		int compare( const _StringParserImpl& other_ ) const override
		{
			//~ #ifndef PNORTTI
			//~ PASSERT4( dynamic_cast< const _CodePageStringParserImpl* >( &other_ ) != 0 );
			//~ #endif
			const _CodePageStringParserImpl& other = static_cast< const _CodePageStringParserImpl& >( other_ );
			PASSERT4( table == other.table );
			PASSERT4( endP == other.endP );
			if( p == other.p )
				return 0;
			return p < other.p ? -1 : 1;
		}
		void clone( PStringEncoding::StringParser& dst ) const override
		{
			void* tmp = PStringEncodingCodePage::_allocParserImpl( dst, sizeof(_CodePageStringParserImpl) );
#include "ppmemtracedisable.h"
			new( tmp ) _CodePageStringParserImpl( *this );
#include "ppmemtraceenable.h"
		}
	};

protected:
	friend class _CodePageStringParserImpl;
	const PUNICHAR* table;
	const PStreamEncodingCodePage::ReverseTableItem* rTable;
	const PStreamEncodingCodePage::ReverseTableItem* rTableEnd;

public:
	PStringEncodingCodePage( const PUNICHAR* table_, const PStreamEncodingCodePage::ReverseTableItem* rTable_, const PStreamEncodingCodePage::ReverseTableItem* rTableEnd_, BYTE flags_, PUNICHAR defaultChar_ )
		: PStringEncoding( 0x0F, flags_, defaultChar_ )
	{
		//props = 0x0F;
		//PString comparisons are Ok as this is just a regular string
		//DOES represent ASCII strings 'as is' and DOES allow appending ASCII strings 'as is'
		table = table_;
		rTable = rTable_;
		rTableEnd = rTableEnd_;
	}
	void createNewParser( StringParser& dst ) const override
	{
		void* p = _allocParserImpl( dst, sizeof(_CodePageStringParserImpl) );
#include "ppmemtracedisable.h"
		new( p ) _CodePageStringParserImpl( table );
#include "ppmemtraceenable.h"
	}
	int charLength( const char* s ) const override
	{
		return static_cast< int >( strlen( s ) );
	}

public://overridden
	void _append( PString& s, PUNICHAR_32 c ) const override;
	void preparePMsg(PString& ret, const char* msg, int argc, const char* argv[]) const override;
};

//**************** Win1252 ***************************************************

class PStreamEncodingWin1252 : public PStreamEncodingCodePage
{
private:
	typedef PStreamEncodingCodePage _Parent;

public:
	PStreamEncodingWin1252() {}
	static const PUNICHAR* table;

public:
	static int _staticReadChars( PUNICHAR*& dst, PUNICHAR* dstEnd, const BYTE*& src, const BYTE* srcEnd );
	static int _staticWriteChar( PUNICHAR_32 c );
	int _readChars( PUNICHAR*& dst, PUNICHAR* dstEnd, const BYTE*& src, const BYTE* srcEnd ) const override;

	//static PStreamEncodingWin1252 instance;
};

class PStringEncodingWin1252 : public PStringEncodingCodePage
{
public:
	PStringEncodingWin1252( BYTE flags_ = 0, PUNICHAR defaultChar_ = 0 );

	/* new */ PStringEncoding* clone() const override
	{
		return new PStringEncodingWin1252( *this );
	}

	//static PStringEncodingWin1252 instance;
};

#endif
