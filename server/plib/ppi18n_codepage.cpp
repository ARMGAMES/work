#include "ppinclude.h"
#include "ppstring.h"
#include "ppi18n.h"
#include "ppi18n_codepage.h"
#include "ppcheck.h"

inline bool PStreamEncodingCodePage::ReverseTableItem::operator<( const ReverseTableItem& other ) const
{
	return uni < other.uni;
}

/* static */ int PStreamEncodingCodePage::_staticReadChars( const PUNICHAR* table, PUNICHAR*& dst, PUNICHAR* dstEnd, const BYTE*& src, const BYTE* srcEnd )
{
	int ret = 0;
	while( src < srcEnd && dst < dstEnd )
	{
		const BYTE c = *src++;
		if( c <= 0x7F )
		{
			*dst++ = ( PUNICHAR )c;
			++ret;
		}
		else if( const PUNICHAR cc = table[ c - 0x80 ] )
		{
			*dst++ = cc;
			++ret;
		}
		else
		{
			// +++ 0 means undefined symbol (what to do?)
			PASSERT5( 0 );
		}
	}
	return ret;
}

/* static */ int PStreamEncodingCodePage::_staticWriteCharPrivate( const ReverseTableItem* begin, const ReverseTableItem* end, PUNICHAR_32 c )//returns -1 if unsuccessful, or (0-255) if successful
{
	ReverseTableItem item;
	item.uni = c;
	_DEBUG_CHECK_TABLE_ORDER( begin, end );
	const ReverseTableItem* it = lower_bound( begin, end, item );
	if( it == end || (*it).uni != c )
		return -1;
	return (*it).c;
}

//****************************************************************************

/* virtual */ void PStringEncodingCodePage::_append( PString& s, PUNICHAR_32 c ) const
{
	if( c == 0 )
	{
		if( flags & FlagErrorOnNullChar )
			throw PInternalError( "PStringEncodingCodePage: unexpected NULL character" );
		else if( flags & FlagDefaultCharOnNullChar )
		{
			int cc = PStreamEncodingCodePage::_staticWriteChar( rTable, rTableEnd, defaultChar );
			PASSERT4( cc > 0 );
			s.append( (char)cc );
			return;
		}
		else
			return;//ignore
	}

	int cc = PStreamEncodingCodePage::_staticWriteChar( rTable, rTableEnd, c );
	if( cc < 0 )
	{
		if( flags & FlagErrorOnUnknownChar )
			throw PInternalError( "PStringEncodingCodePage: unknown character" );
		else if( flags & FlagDefaultCharOnUnknownChar )
		{
			cc = PStreamEncodingCodePage::_staticWriteChar( rTable, rTableEnd, defaultChar );
			PASSERT4( cc > 0 );
			s.append( (char)cc );
			return;
		}
		//else ignore
	}
	else
	{
		PASSERT4( cc > 0 );
		s.append( (char)cc );
	}
}

/* virtual */ void PStringEncodingCodePage::preparePMsg(PString& ret, const char* msg, int argc, const char* argv[]) const
{
	for(const char* s = msg; *s ; ++s)
	{
		if(*s != '%')
		{
			ret.append(*s);
			continue;
		}
		//%
		if(*(++s) == 0)
			break;
		if(*s == '%')
		{
			ret.append('%');
			continue;
		}
		//L or l
		bool isLower = false;
		if(*s == 'L' || *s == 'l')
		{
			isLower = true;
			if(*(++s) == 0)
				break;
		}
		//0-9
		if(*s >= '0' && *s <= '9')
		{
			int m = *s - '0';
			if(m < argc)
			{
				if(isLower)
				{
					PString insert(argv[m]);
					insert.toLower();
					ret.append(insert);
				}
				else
					ret.append(argv[m]);
			}
		}
		else
		{
			//invalid msg format - ignored
		}
	}
}
