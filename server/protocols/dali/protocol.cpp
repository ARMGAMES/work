#include "protocol.h"
#pragma warning( push )
#pragma warning( disable: 4365 4625 4626 )
#include "commssl.h"
#pragma warning( pop )

namespace dali
{

	// string functions
	string floatString( double d, uint8_t p )
	{
		char buf[300];
		snprintf( buf, sizeof( buf ), "%.*f", p, d );
		return string( buf );
	}

	string toLower( const char* s )
	{
		string res( s );
		transform( res.begin(), res.end(), res.begin(), ::tolower );
		return res;
	}

	string toUpper( const char* s )
	{
		string res( s );
		transform( res.begin(), res.end(), res.begin(), ::toupper );
		return res;
	}

	string trim( const char* s )
	{
		string res;

		if( s && *s )
		{
			s = PString::ltrim( s );

			if( s && *s )
			{
				return res.assign( s, PString::rtrim( s ) );
			}
		}

		return res;
	}

	bool startsWith( const string& str, const char* substr )
	{
		if ( !substr || !*substr || str.length() < strlen( substr ) )
			return false;
		return !str.substr( 0, strlen( substr ) ).compare( substr );
	}

	bool endsWith( const string& str, const char* substr )
	{
		if ( !substr || !*substr || str.length() < strlen( substr ) )
			return false;
		return !str.substr( str.length() - strlen( substr ) ).compare( substr );
	}

	void replaceSubstring( string& str, const char* substr, const char* replacement )
	{
		for ( auto offset = str.find( substr ); offset != string::npos; offset = str.find( substr, offset ) )
		{
			str.replace( offset, strlen( substr ), replacement );
		}
	}

	size_t findEndUtf8( const string& str )
	{
		auto end = std::min<size_t>( 6, str.size() );	// max theoritical len == 6
		const char* cptr = &str.back();
		size_t num = 1;
		for ( ; num <= end; ++num, --cptr )
		{
			if ( ( *cptr & 0x80 ) == 0x00 )		// U+000000 ~ U+00007F, single character, don't need to trim
				return num-1;
			if ( ( *cptr & 0xF8 ) == 0xF0 )		// U+010000 ~ U+10FFFF, 4 bytes needed
				return num == 4 ? 0 : num;
			if ( ( *cptr & 0xF0 ) == 0xE0 )		// U+000800 ~ U+00FFFF, 3 bytes needed
				return num == 3 ? 0 : num;
			if ( ( *cptr & 0xE0 ) == 0xC0 )		// U+000080 ~ U+0007FF, 2 bytes needed
				return num == 2 ? 0 : num;
		}
		return 0;								// malformed UTF-8 character sequence.
	}

	void cut( string& str, size_t sz )
	{
		if ( str.size() > sz )
		{
			str.resize( sz );
			auto trim = findEndUtf8( str );
			if ( trim )
				str.resize( str.size() - trim );
		}
	}

	// binary functions

	PBlock toBlock( const vector<uint8_t>& bin )
	{
		PBlock blk;
		auto ptr = blk.alloc( bin.size() );
		memmove( ptr, bin.data(), bin.size() );
		return blk;
	}

	// time functions

	string toString( int64_t time, bool usec )
	{
		char str[ 64 ];
		auto tm = expandTime( time );
		if ( tm->tm_year < 0 || tm->tm_mon < 0 || tm->tm_mday < 0 || tm->tm_hour < 0 || tm->tm_min < 0 || tm->tm_sec < 0 )
			sprintf( str, "0001-01-01 00:00:00" );
		else
		{
			if ( usec )
				sprintf( str, "%d-%02d-%02d %02d:%02d:%02d.%06d"
					, tm->tm_year + 1900, tm->tm_mon + 1 , tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec
					, static_cast< int >( time % USEC_IN_SEC ) );
			else
				sprintf( str, "%d-%02d-%02d %02d:%02d:%02d"
					, tm->tm_year + 1900, tm->tm_mon + 1 , tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec );
		}
		return string( str );
	}

	string toString( const CommSrvTime& stime )
	{
		char str[ 64 ];
		sprintf( str, "%d-%02d-%02d %02d:%02d:%02d",
				stime._year, stime._month , stime._day, stime._hour, stime._minute, stime._second );
		return string( str );
	}

	string toString( const CommSrvDate& sdate )
	{
		char str[ 64 ];
		sprintf( str, "%d-%02d-%02d",
				sdate._year, sdate._month , sdate._day );
		return string( str );
	}

	unique_ptr< struct tm > expandTime( int64_t time, bool utc )
	{
		auto tm = unique_ptr< struct tm >( new struct tm );
		auto t = fromTime( time );
		if ( utc )
		{
			auto tm_ptr = gmtime( &t );
			if ( tm_ptr )
				*tm = *tm_ptr;
			else
				plib_localtime_ext( t, tm.get() ); // can't find utc time conversion
		}
		else
			plib_localtime_ext( t, tm.get() ); // can't use local_s( tm.get(), &t ); due to dst shift not propagating to windows process
		return tm;
	}

	// string functions

	vector< string > splitString( const char* str, const char* delimiters )
	{
		vector< string > vec_str;
		if ( !str )
			return vec_str;
		for ( const char* ptr = str; *ptr; ++ptr )
		{
			const char* start_ptr = ptr;
			while ( *ptr && !strchr( delimiters, *ptr ) )
				++ptr;
			if ( start_ptr < ptr )
				vec_str.emplace_back( start_ptr, ptr - start_ptr );
			if ( !*ptr )
				break;
		}
		return vec_str;
	}

	string deliniateString( const string& str, char word_sep, size_t chars_in_word, size_t words_in_line )
	{
		string str_ret;
		str_ret.reserve( str.length()
			+ ( ( word_sep && chars_in_word ) ? ( str.length() / chars_in_word ) : 0 )
			+ ( words_in_line ? ( str.length() / words_in_line ) : 0 ) );
		size_t charCount = 0;
		size_t wordCount = 0;
		for ( auto p = str.c_str(); *p; )
		{
			str_ret.append( 1, *p++ );
			if ( ++charCount == chars_in_word && *p )
			{ // word separation needed and not the end of the string
				charCount = 0; // new word
				if ( ++wordCount == words_in_line )
				{ // line separation needed
					wordCount = 0; // new line
					str_ret.append( 1, '\n' );
				}
				else
				{
					str_ret.append( 1, word_sep );
				}
			}
		}
		return str_ret;
	}

	string quoteString( const char* str, const char quoteChar, const char escapeChar )
	{
		string buf;
		buf.append( 1, quoteChar );
		if ( str )
		{
			for ( const char* p = str; *p; ++p )
			{
				if ( *p == quoteChar )
					buf.append( 1, escapeChar );
				buf.append( 1, *p );
			}
		}

		buf.append( 1, quoteChar );
		return buf;
	}

	// block functions

	string toBase16( const vector<uint8_t>& bin )
	{
		string str_ret;
		str_ret.reserve( sizeOfBase16String( bin.size() ) );
		for ( auto b : bin )
			str_ret.append( 1, toCharFromBase16( b >> 4 ) ).append( 1, toCharFromBase16( b & 0xF ) );
		return str_ret;
	}

	vector<uint8_t> fromBase16( const string& str )
	{
		vector<uint8_t> blk( sizeOfBase16Binary( ( size_t ) str.length() ), 0 );
		uint8_t* dstptr = blk.data();
		for ( auto ptr = str.c_str(), end = str.c_str() + str.length(); ptr < end; )
		{
			auto n = toBase16FromChar( *ptr++ );
			if ( n >= 0 )
			{
				uint8_t b = static_cast<uint8_t>( n );
				if ( ptr < end )
				{
					n = toBase16FromChar( *ptr++ );
					if ( n >= 0 )
						b = static_cast<uint8_t>( ( b << 4 ) | n );
				}
				*dstptr++ = b;
			}
		}
		dali::cut( blk, static_cast<size_t>( dstptr - blk.data() ) );
		return blk;
	}

	string toBase41( const vector<uint8_t> & bin )
	{
		string str( sizeOfBase41String( bin.size() ) + 1, 0 );
		size_t word_sz = 2, word_ix = 0, str_ix = 0;
		unsigned int acc = 0;
		for ( size_t j = 0; j < bin.size(); j++ )
		{
			if ( word_ix && word_ix < word_sz )
				acc = acc << CHAR_BIT;
			acc += bin[j];
			if ( ++word_ix == word_sz || j == bin.size()-1 )
			{ // last word BYTE or last block BYTE
				for ( size_t i = word_ix; i > 0; --i )
				{
					str[ str_ix + i ] = toCharFromBase41( acc % 41 );
					acc /= 41;
				}
				str[ str_ix ] = toCharFromBase41( acc );
				str_ix += word_ix + 1;
				word_ix = 0;
				acc = 0;
			}
		}
		str[ str_ix ] = 0;
		return str;
	}

	vector<uint8_t> fromBase41( const string & str )
	{
		vector<uint8_t> blk( sizeOfBase41Binary( ( size_t ) str.length() ), 0 );
		size_t code_sz = 3, code_ix = 0, blk_ix = 0;
		unsigned int acc = 0;
		for ( size_t j = 0; j < str.size(); j++ )
		{
			int base = toBase41FromChar( str[j] );
			if ( base < 0 )
				continue;
			if ( code_ix && code_ix < code_sz )
				acc *= 41;
			acc += base;
			if ( ++code_ix == code_sz || j == str.size()-1 )
			{ // last word char or last string char
				if ( code_ix == 1 )
				{ // traling single character
					blk[ blk_ix ] = ( uint8_t ) acc;
					blk_ix += code_ix;
				}
				else
				{ // full word or trailing characters
					for ( size_t i = code_ix - 2; i > 0; --i )
					{
						blk[ blk_ix + i ] = ( uint8_t ) ( acc & UCHAR_MAX );
						acc = acc >> CHAR_BIT;
					}
					blk[ blk_ix ] = ( uint8_t ) acc; // intentionally ignore overflow
					blk_ix += code_ix - 1;
				}
				code_ix = 0;
				acc = 0;
			}
		}
		dali::cut( blk, static_cast< size_t >( blk_ix ) );
		return blk;
	}

	string toBase85( const vector<uint8_t> & blk )
	{
		string str( sizeOfBase85String( blk.size() ) + 1, 0 );
		size_t word_sz = 4, word_ix = 0, str_ix = 0;
		unsigned int acc = 0;
		for ( size_t j = 0; j < blk.size(); j++ )
		{
			if ( word_ix && word_ix < word_sz )
				acc = acc << CHAR_BIT;
			acc += blk[j];
			if ( ++word_ix == word_sz || j == blk.size()-1 )
			{ // last word BYTE or last block BYTE
				for ( size_t i = word_ix; i > 0; --i )
				{
					str[ str_ix + i ] = toCharFromBase85( acc % 85 );
					acc /= 85;
				}
				str[ str_ix ] = toCharFromBase85( acc );
				str_ix += word_ix + 1;
				word_ix = 0;
				acc = 0;
			}
		}
		str[ str_ix ] = 0;
		return str;
	}

	vector<uint8_t> fromBase85( const string & str )
	{
		vector<uint8_t> blk( sizeOfBase85Binary( ( size_t ) str.length() ), 0 );
		size_t code_sz = 5, code_ix = 0, blk_ix = 0;
		unsigned int acc = 0;
		for ( size_t j = 0; j < str.size(); j++ )
		{
			int base = toBase85FromChar( str[j] );
			if ( base < 0 )
				continue;
			if ( code_ix && code_ix < code_sz )
				acc *= 85;
			acc += base;
			if ( ++code_ix == code_sz || j == str.size()-1 )
			{ // last word char or last string char
				if ( code_ix == 1 )
				{ // traling single character
					blk[ blk_ix ] = ( uint8_t ) acc;
					blk_ix += code_ix;
				}
				else
				{ // full word or trailing characters
					for ( size_t i = code_ix - 2; i > 0; --i )
					{
						blk[ blk_ix + i ] = ( uint8_t ) ( acc & UCHAR_MAX );
						acc = acc >> CHAR_BIT;
					}
					blk[ blk_ix ] = ( uint8_t ) acc; // intentionally ignore overflow
					blk_ix += code_ix - 1;
				}
				code_ix = 0;
				acc = 0;
			}
		}
		dali::cut( blk, static_cast< size_t >( blk_ix ) );
		return blk;
	}

	bool operator <( const vector<uint8_t> & lhs, const vector<uint8_t> & rhs )
	{
		const uint8_t * p1 = lhs.data(), * p2 = rhs.data();
		for ( size_t i = 0; i < lhs.size() && i < rhs.size(); ++i )
		{
			int diff = *p1++ - *p2++;
			if ( diff )
				return diff < 0;
		}
		return lhs.size() < rhs.size();
	}

	char toCharFromBase16( int n )
	{
		if ( n > -1 && n < 10 )
			return '0' + ( char ) n;
		if ( n > 9 && n < 16 )
			return 'A' + ( char ) n - 10;
		return 0;
	}

	int toBase16FromChar( char c )
	{
		if ( c >= '0' && c <= '9' )
			return c - '0';
		if ( c >= 'A' && c <= 'F' )
			return c - 'A' + 10;
		if ( c >='a' && c <= 'f' )
			return c - 'a' + 10;
		return -1;
	}

	char toCharFromBase41( unsigned int n )
	{
		if ( n < 26 )
			return 'a' + ( char ) n;
		if ( n < 41 )
			return 'L' + ( char ) n - 26;
		return 0;
	}

	int toBase41FromChar( char c )
	{
		if ( c >= 'a' && c <='z' )
			return c - 'a';
		if ( c >= 'L' && c <= 'Z' )
			return c - 'L' + 26;
		return -1;
	}

	char toCharFromBase85( unsigned int n )
	{
		if ( n < 56 )
			return '(' + ( char ) n;
		if ( n < 85 )
			return 'a' + ( char ) n - 56;
		return 0;
	}

	int toBase85FromChar( char c )
	{
		if ( c >= '(' && c <= '_' )
			return c - '(';
		if ( c >= 'a' && c <= '}' )
			return c - 'a' + 56;
		return -1;
	}

	uint64_t toHashFromBinary( const vector<uint8_t> & blk )
	{
		uint64_t hash = 0;
		size_t word_sz = 8, word_ix = 0;
		uint64_t acc = 0;
		for ( size_t j = 0; j < blk.size(); j++ )
		{
			if ( word_ix && word_ix < word_sz )
				acc = acc << CHAR_BIT;
			acc += blk[j];
			if ( ++word_ix == word_sz || j == blk.size()-1 )
			{ // last word BYTE or last block BYTE
				hash ^= acc;
				word_ix = 0;
				acc = 0;
			}
		}
		return hash;
	}

	vector<uint8_t> fromChksum( CommSSLChksumMD5& md5 )
	{
		PBlock blk;
		md5.chksum( blk );
		return dali::fromBlock( blk );
	}

	// Composer class

	MsgComposer& MsgComposer::composeVector( const vector<uint32_t>& vec )
	{
		composeUint32( (uint32_t)vec.size() );
		for ( auto n : vec )
		{
			composeUint32( n );
		}
		return *this;
	}

	MsgComposer& MsgComposer::composeVector( const vector<uint64_t>& vec )
	{
		composeUint32( (uint32_t)vec.size() );
		for ( auto n : vec )
			composeUint64( n );
		return *this;
	}

	MsgComposer& MsgComposer::composeVector( const vector<string>& vec )
	{
		composeUint32( (uint32_t)vec.size() );
		for ( size_t i = 0; i < vec.size(); ++i )
		{
			composeString( vec[i] );
		}
		return *this;
	}

	// Parser class

	MsgParser& MsgParser::parseString( string& str, size_t maxStringLengthAllowed, const char *fieldDescr )
	{
		const char * s;
		parser.parseStringN( s, maxStringLengthAllowed, fieldDescr );
		str.assign( s );
		return *this;
	}

	MsgParser& MsgParser::parseBinary( vector<uint8_t>& block, size_t maxBlockSizeAllowed, const char *fieldDescr )
	{
		if ( !fieldDescr || !*fieldDescr )
		{
			fieldDescr = "dali::MsgParser::parseBinary()";
		}

		const uint8_t * p;
		size_t size;
		parser._parseVarBlock( p, size );

		if ( maxBlockSizeAllowed > 0 && size > maxBlockSizeAllowed )
		{
			char descr[256];
			sprintf( descr, "%.80s: block is too big: sz=%u, maxsize=%u", fieldDescr, (unsigned int)size, (unsigned int)maxBlockSizeAllowed );
			throw PParserError( descr );
		}
		block.clear();
		if ( size > 0 )
			block.assign( p, p + size );
		return *this;
	}

	MsgParser& MsgParser::parseSrvTime( int64_t & ret )
	{
		CommSrvTime t;
		parser.parseSrvTime( t );
		ret = toTime( t.encode() );
		return *this;
	}

	MsgParser& MsgParser::parseVector( vector<uint32_t>& vec )
	{
		uint32_t size;
		parseUint32( size );
		for ( uint32_t i = 0; i < size; ++i )
		{
			uint32_t n;
			parseUint32( n );
			vec.push_back( n );
		}
		return *this;
	}

	MsgParser& MsgParser::parseVector( vector<uint64_t>& vec )
	{
		uint32_t size;
		parseUint32( size );
		vec.resize( size );
		for ( size_t i = 0; i < size; ++i )
			parseUint64( vec[i] );
		return *this;
	}

	MsgParser& MsgParser::parseVector( vector<string>& vec )
	{
		uint32_t size;
		parseUint32( size );
		vec.resize( size );
		for ( uint32_t i = 0; i < size; ++i )
		{
			parseString( vec[i], 0 );
		}
		return *this;
	}

}
