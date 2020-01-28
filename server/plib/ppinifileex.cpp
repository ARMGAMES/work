#include "ppinifile.h"
#include "ppi18n_string.h"
#include "ppi18n_x.h"

const char* UTF8_BOM = "\xEF\xBB\xBF";

const char* PIniFileEx::Section::getProperty( const char* name_ ) const
{
	size_t n = items.size();
	for( size_t i = 0; i < n ; ++i )
	{
		const Item& item = items[ i ];
		if( parent->encString->compareIgnoreCase( name_, item.name ) == 0 )
			return item.value;
	}
	return 0;
}

void PIniFileEx::Section::setProperty( const char* name_, const char* value )
{
	size_t n = items.size();
	for( size_t i = 0; i < n ; ++i )
	{
		Item& item = items[ i ];
		if( parent->encString->compareIgnoreCase( name_, item.name ) == 0 )
		{
			item.value = value;
			return;
		}
	}
	items.push_back( Item( name_, value ) );
}

void PIniFileEx::Section::eraseProperty( const char* name_ )
{
	vector< Item >::iterator iter;
	for( iter = items.begin(); iter != items.end(); )
	{
		if( parent->encString->compareIgnoreCase( name_, (*iter).name ) == 0 )
		{
			iter = items.erase( iter );
			continue;
		}
		++iter;
	}
}

int PIniFileEx::Section::getIntProperty( const char* name_, int defaultValue ) const
{
	const char* s = getProperty( name_ );
	if( s == 0 )
		return defaultValue;
	int radix = 10;
	if( *s != 0 && s[ 0 ] == 'x' )
	{
		radix = 16;
		s += 1;
	}

	char* dummy;
	return (int)strtol( s, &dummy, radix );
}

INT64 PIniFileEx::Section::getInt64Property( const char* name_, INT64 defaultValue ) const
{
	const char* s = getProperty( name_ );
	if( s == 0 )
		return defaultValue;
	return p_atoi64( s );
}

PIniFileEx::PIniFileEx()
{
	encString = new PStringEncodingPStringX();
}

PIniFileEx::PIniFileEx( const PIniFileEx& from_ )
{
	encString = from_.encString->clone();
	sections = from_.sections;
	for(vector<Section>::iterator iter = sections.begin();
		iter != sections.end();
		++iter)
	{
		iter->parent = this;
	}
}

PIniFileEx::Section* PIniFileEx::getSection( const char* secName )
{
	size_t n = sections.size();
	for( size_t i = 0; i < n ; ++i )
	{
		Section& sec = sections[ i ];
		if( encString->compareIgnoreCase( secName, sec.name ) == 0 )
			return &sec;
	}
	return 0;
}

const PIniFileEx::Section* PIniFileEx::getSection( const char* secName ) const
{
	size_t n = sections.size();
	for( size_t i = 0; i < n ; ++i )
	{
		const Section& sec = sections[ i ];
		if( encString->compareIgnoreCase( secName, sec.name ) == 0 )
			return &sec;
	}
	return 0;
}

PIniFileEx::Section* PIniFileEx::addSection( const char* secName )
{
	size_t n = sections.size();
	for( size_t i = 0; i < n ; ++i )
	{
		Section& sec = sections[ i ];
		if( encString->compareIgnoreCase( secName, sec.name ) == 0 )
			return &sec;
	}
	return &*sections.insert( sections.end(), Section( *this, secName ) );
}

void PIniFileEx::eraseSection( const char* secName )
{
	vector< Section >::iterator iter;
	for( iter = sections.begin(); iter != sections.end(); )
	{
		if( encString->compareIgnoreCase( secName, (*iter).name ) == 0 )
		{
			iter = sections.erase( iter );
			continue;
		}
		++iter;
	}
}

void PIniFileEx::setSection( const Section& sec )
{
	PASSERT4( sec.parent == 0 );
	Section* found = getSection( sec.name );
	if( found )
	{
		*found = sec;
		(*found).parent = this;
	}
	else
	{
		Section& s = *sections.insert( sections.end(), sec );
		s.parent = this;
	}
}

static inline void _appendChar( PBlock& b, PUNICHAR ch )
{
	*(PUNICHAR*)b.append( sizeof(PUNICHAR) ) = ch;
}

static inline size_t _charLen( const PBlock& b )
{
	size_t sz = b.size();
	PASSERT4( sz % sizeof(PUNICHAR) == 0 );
	return sz / sizeof(PUNICHAR);
}

static bool _readLine( PBlock& ret /* of PUNICHAR */, PUniCharInputStream& f )
{
	ret.clear();
	for(;;)
	{
		int c = f.get();
		if( c < 0 )
			return ret.size() ? true : false;
		if( c == '\n' )
			return true;
		else if( c != '\r' && c != 0 )
			_appendChar( ret, static_cast< PUNICHAR >( c ) );
	}
}

class _PUniCharStringParser
{
private:
	const PUNICHAR* p;
	const PUNICHAR* pEnd;

public:
	_PUniCharStringParser()
	{
		p = pEnd = NULL;
	}
	_PUniCharStringParser( const PUNICHAR* p_, const PUNICHAR* pEnd_ )
	{
		p = p_;
		pEnd = pEnd_;
	}

public:
	PUNICHAR_RET nextChar()
	{
		if( p >= pEnd )
			return 0;
		return *p++;
	}
	bool operator <( const _PUniCharStringParser& other ) const
	{
		PASSERT4( pEnd == other.pEnd );
		return p < other.p;
	}
	bool operator ==( const _PUniCharStringParser& other ) const
	{
		PASSERT4( pEnd == other.pEnd );
		return p == other.p;
	}
	bool isValid() const
	{
		return p < pEnd;
	}

public:
	const PUNICHAR* _ptr() const
	{
		return p;
	}
};

static void _copyStr( PBlock& b, const _PUniCharStringParser& begin, const _PUniCharStringParser& end )
{
	if( !( begin < end ) )
		return;
	_PUniCharStringParser s = begin;
	while( !( s == end ) )
	{
		PUNICHAR_RET ch = s.nextChar();
		if( ch <= 0 )
			return;
		_appendChar( b, static_cast< PUNICHAR >( ch ) );
	}
}

void PIniFileEx::_loadLine( vector< Section >::iterator& section, int line, const PUNICHAR* ln, const PUNICHAR* lnEnd )
{
	_PUniCharStringParser lnParser( ln, lnEnd );
	PBlock s_;
	_copyStr( s_, PI18N_ltrim( lnParser ), PI18N_rtrim( lnParser ) );
	const PUNICHAR* s = (const PUNICHAR*)s_.ptr();
	size_t sLen = _charLen( s_ );

	bool Ok = true;
	if( sLen == 0 || *s == ';' || *s == '#' )
		return;
	else if( *s == '[' && s[ sLen - 1 ] == ']' )
	{
		section = sections.insert( sections.end(), Section( *this ) );
		encString->append( (*section).name, s + 1, s + sLen - 1 );
	}
	else
	{
		if( section == sections.end() )
			section = sections.insert( sections.end(), Section( *this ) );//empty section

		_PUniCharStringParser sParser( s, s + sLen );
		_PUniCharStringParser eq = PI18N_strchr( sParser, '=' );
		if( !eq.isValid() )
			Ok = false;
		else
		{
			Item& item = *(*section).items.insert( (*section).items.end(), Item() );
			encString->append( item.name, sParser._ptr(), eq._ptr() );
			eq.nextChar();
			encString->append( item.value, eq._ptr(), s + sLen );
		}
	}
	if( !Ok )
	{
		throw PInternalError( PString( "PIniFileEx: Error in line #" ).appendInt( line ) );
	}
}

void PIniFileEx::load( PUniInputStream& f_ )
{
	PUniCharInputStream f( f_ );

	sections.erase( sections.begin(), sections.end() );
	vector< Section >::iterator section = sections.end();
	int line = 0;
	for(;;)
	{
		PBlock ln;
		bool Ok = _readLine( ln, f );
		if( !Ok )
			return;
		++line;

		_loadLine( section, line, (const PUNICHAR*)ln.ptr(), ( (const PUNICHAR*)ln.ptr() ) + _charLen( ln ) );
	}
}

void PIniFileEx::load( PInputStream& f, const PStreamEncoding& enc )
{
	PUniInputStreamFromStream uf( f, enc );
	load( uf );
}

void PIniFileEx::load( const char* fileName )
{
	FILE* f = fopen( fileName, "rt" );
	if( !f )
	{
		PString msg;
		msg.append( "Unable to open ini file '" ).append( fileName ).append( "'" );
		throw PInternalError( msg );
	}
	bool isUtf8File = false;
	//read BOM
	char buf_char[4];
	buf_char[3] = '\0';

	if(fread(&buf_char, 1, 3, f) == 3)
	{
		if(!strcmp(buf_char, UTF8_BOM))
			isUtf8File = true;
	}
	if( !isUtf8File )
		rewind( f );
	PFileInputStream inS( f );
	if( isUtf8File )
	{
		PStreamEncodingUtf8 encStr;
		PUniInputStreamFromStream inStr( inS, encStr );
		load( inStr );
	}
	else
	{
		PStreamEncodingWin1252 encStr;
		PUniInputStreamFromStream inStr( inS, encStr );
		load( inStr );
	}
	fclose( f );
}

static inline void _writeChar( PUniOutputStream& f, PUNICHAR c )
{
	f.write( &c, 1 );
}

static void _writeString( PUniOutputStream& f, PStringEncoding::StringParser& parser )
{
	for(;;)
	{
		PUNICHAR_RET ch = parser.nextChar();
		if( ch <= 0 )
			return;
		_writeChar( f, static_cast< PUNICHAR >( ch ) );
	}
}

void PIniFileEx::save( PUniOutputStream& f ) const
{
	size_t n = sections.size();
	PStringEncoding::StringParser parser;
	encString->createNewParser( parser );
	for( size_t i = 0; i < n ; ++i )
	{
		const Section& sec = sections[ i ];
		_writeChar( f, '[' );
		parser.init( sec.name );
		_writeString( f, parser );
		_writeChar( f, ']' );
		_writeChar( f, '\n' );
		size_t nj = sec.items.size();
		for( int j = 0; j < nj ; ++j )
		{
			const Item& item = sec.items[ j ];
			parser.init( item.name );
			_writeString( f, parser );
			_writeChar( f, '=' );
			parser.init( item.value );
			_writeString( f, parser );
			_writeChar( f, '\n' );
		}
		_writeChar( f, '\n' );
	}
}

void PIniFileEx::save( const char* fileName ) const
{
	FILE* f = fopen( fileName, "wt" );
	if( !f )
		throw PInternalError( PString( "Unable to open ini file '" ).append( fileName ).append( "' for writing" ) );
	PString scriptStr;
	PUniOutputStreamToPStringX outStr(scriptStr);
	save(outStr);
	if( PStringX::isUtf8( scriptStr ) )
	{
		fwrite( UTF8_BOM, 1, 3, f );
		fwrite( scriptStr.c_str() + 1, 1, scriptStr.length() - 1, f );
	}
	else
	{
		fwrite( scriptStr.c_str(), 1, scriptStr.length(), f );
	}
	fclose( f );
}
