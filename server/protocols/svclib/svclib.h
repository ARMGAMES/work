#pragma once

#pragma warning( push )
#pragma warning( disable: 4242 4244 4365 4625 4626 4996 )
#include "pplib.h"
#include "commtime.h"
#pragma warning( pop )
#pragma warning( push )
#pragma warning( disable: 4365 4774 )
#include <memory>
#include <chrono>
#include <ctime>
#include <string>
#pragma warning( pop )

class CommSSLChksumMD5;

#define USEC_IN_MSEC		1000
#define USEC_IN_SEC			1000000
#define USEC_IN_HOUR		3600000000
#define USEC_IN_DAY			86400000000
#define USEC_EPOCH_OFFSET	2177452800000000

#define CATCH_LOG( _FUNC_NAME_ ) \
	catch( const PError& exc ) { PLog( "[%s] <F> framework exception=%s", _FUNC_NAME_, exc.why() ); }\
	catch( const exception& exc ) { PLog( "[%s] <F> general exception=%s", _FUNC_NAME_, exc.what() ); }\
	catch( ... ) { PLog( "[%s] <F> unknown exception", _FUNC_NAME_ ); }

#define CATCH_LOG_MSG( _FUNC_NAME_, _MSG_ID_ ) \
	catch( const PError& exc ) { PLog( "[%s] <F> msg=%u, framework exception=%s", _FUNC_NAME_, _MSG_ID_, exc.why() ); }\
	catch( const exception& exc ) { PLog( "[%s] <F> msg=%u, general exception=%s", _FUNC_NAME_, _MSG_ID_, exc.what() ); }\
	catch( ... ) { PLog( "[%s] <F> msg=%u, unknown exception", _FUNC_NAME_, _MSG_ID_ ); }

#define CATCH_LOG_MSG_USER( _FUNC_NAME_, _MSG_ID_, _USER_NAME_ ) \
	catch( const PError& exc ) { PLog( "[%s] <F> msg=%u, user=%s, framework exception=%s", _FUNC_NAME_, _MSG_ID_, _USER_NAME_, exc.why() ); }\
	catch( const exception& exc ) { PLog( "[%s] <F> msg=%u, user=%s, general exception=%s", _FUNC_NAME_, _MSG_ID_, _USER_NAME_, exc.what() ); }\
	catch( ... ) { PLog( "[%s] <F> msg=%u, user=%s, unknown exception", _FUNC_NAME_, _MSG_ID_, _USER_NAME_ ); }

namespace svclib
{
	static constexpr uint64_t NULL_U64 = UINT64_MAX;
	static constexpr uint32_t NULL_U32 = UINT32_MAX;
	static constexpr uint16_t NULL_U16 = UINT16_MAX;
	static constexpr uint8_t NULL_U8 = UINT8_MAX;
	static constexpr int64_t NULL_I64 = INT64_MIN;
	static constexpr int32_t NULL_I32 = INT32_MIN;
	static constexpr int16_t NULL_I16 = INT16_MIN;
	static constexpr int8_t NULL_I8 = INT8_MIN;
	static constexpr double NULL_DBL = NAN;
	static const string NULL_STR = "\xFF";
	static const vector< uint8_t > NULL_BIN = { 0xff, 0x00 };

	inline bool isNull( const uint64_t & val ) { return val == NULL_U64; }
	inline bool isNull( const uint32_t & val ) { return val == NULL_U32; }
	inline bool isNull( const uint16_t & val ) { return val == NULL_U16; }
	inline bool isNull( const uint8_t & val ) { return val == NULL_U8; }
	inline bool isNull( const int64_t & val ) { return val == NULL_I64; }
	inline bool isNull( const int32_t & val ) { return val == NULL_I32; }
	inline bool isNull( const int16_t & val ) { return val == NULL_I16; }
	inline bool isNull( const int8_t & val ) { return val == NULL_I8; }
	inline bool isNull( const double & val ) { return isnan( val ); }
	inline bool isNull( const string & val ) { return val == NULL_STR; }
	inline bool isNull( const vector< uint8_t > & val ) { return val == NULL_BIN; }

	template< typename T, typename = typename std::enable_if< std::is_unsigned< T >::value >::type, typename = void >
	inline string intString( T n )
	{
		char buf[ 0x20 ];
		return string( p_u64toa( n, buf ) );
	}

	template< typename T, typename = typename std::enable_if< std::is_signed< T >::value || std::is_enum< T >::value >::type >
	inline string intString( T n )
	{
		char buf[ 0x20 ];
		return string( p_i64toa( n, buf ) );
	}

	template< typename T >
	inline string hexString( T n )
	{
		char buf[ 0x20 ];
		return string( p_hex64toa( static_cast< typename make_unsigned< T >::type >( n ), buf ) );
	}

	string floatString( double d, uint8_t p = 2 );

	string toLower( const char* s );
	string toUpper( const char* s );
	inline string toLower( const string& s ) { return toLower( s.c_str() ); }
	inline string toUpper( const string& s ) { return toUpper( s.c_str() ); }
	string toUtf8String( const char* str );

	string ltrim( const char* s );
	string rtrim( const char* s );
	string trim( const char* s );
	inline string ltrim( const string& s ) { return ltrim( s.c_str() ); }
	inline string rtrim( const string& s ) { return rtrim( s.c_str() ); }
	inline string trim( const string& s ) { return trim( s.c_str() ); }

	inline const char* findIgnoreCase( const string& str, const char* s ) { return PString::findIgnoreCase( str.c_str(), s ); }
	inline bool equalsIgnoreCase( const string& str, const char* s ) { return PString::compareIgnoreCase( str.c_str(), s ) == 0; }
	bool startsWith( const string& str, const char* substr );
	bool endsWith( const string& str, const char* substr );
	void replaceSubstring( string& str, const char* substr, const char* replacement );

	template<typename T>
	inline void cut( T& s, size_t sz ) { if ( s.size() > sz ) s.resize( sz ); }

	void cut( string& s, size_t sz );

	// binary functions

	PBlock toBlock( const vector<uint8_t>& bin );
	inline string toString( const vector<uint8_t>& bin ) { return string( bin.data(), bin.data() + bin.size() ); }
	inline vector<uint8_t> fromBlock( const PBlock& blk ) { return vector<uint8_t>( blk.ptr(), blk.ptr() + blk.size() ); }
	inline vector<uint8_t> fromString( const string& str ) { return vector<uint8_t>( str.c_str(), str.c_str() + str.size() ); }

	// time functions

	string toString( int64_t time, bool usec = false );
	string toString( const CommSrvTime& stime );
	string toString( const CommSrvDate& sdate );

	inline int64_t getTime( bool usec = false )
	{
		using namespace std::chrono;
		return usec
			? time_point_cast< microseconds >( system_clock::now() ).time_since_epoch().count() + USEC_EPOCH_OFFSET
			: time_point_cast< seconds >( system_clock::now() ).time_since_epoch().count() * USEC_IN_SEC + USEC_EPOCH_OFFSET;
	}

	inline int64_t toTime( time_t time )
	{
		using namespace std::chrono;
		return time_point_cast< seconds >( system_clock::from_time_t( time ) ).time_since_epoch().count() * USEC_IN_SEC + USEC_EPOCH_OFFSET;
	}

	inline int64_t toTime( struct tm& tm ) { return toTime( plib_mkUTCtime( &tm ) ); }

	inline time_t fromTime( int64_t time )
	{
		using namespace std::chrono;
		return system_clock::to_time_t( time_point< system_clock >( microseconds( time - USEC_EPOCH_OFFSET ) ) );
	}

	unique_ptr< struct tm > expandTime( int64_t time, bool utc = true );

	// string functions

	vector< string > splitString( const char* str, const char* delimiters );
	string deliniateString( const string& str, char word_sep = 0, size_t chars_in_word = 0, size_t words_in_line = 0 ); // string with delimiters
	string quoteString( const char* str, char quoteChar = '"' );
	inline string quoteString( const string& str, char quoteChar = '"' ) { return quoteString( str.c_str(), quoteChar ); };
	bool isUtf8String( const string& str );

	// binary functions

	string toBase16( const vector<uint8_t>& bin );
	vector<uint8_t> fromBase16( const string& str );
	string toBase41( const vector<uint8_t>& bin );
	vector<uint8_t> fromBase41( const string& str );
	string toBase85( const vector<uint8_t>& bin );
	vector<uint8_t> fromBase85( const string& str );

	inline size_t sizeOfBase16String( size_t sz ) { return sz << 1; }
	inline size_t sizeOfBase16Binary( size_t sz ) { return ( sz & 0x1 ) + ( sz >> 1 ); }						// ceil( sz / 2 )
	inline size_t sizeOfBase41String( size_t sz ) { return ( sz & 0x1 ) + sz + ( sz >> 1 ); }					// ceil( 3 * sz / 2 )
	inline size_t sizeOfBase41Binary( size_t sz ) { return static_cast< size_t >( ceil( 2 * sz / 3.0 ) ); }                             //-V113 //-V203 //-V2004
	inline size_t sizeOfBase85String( size_t sz ) { return ( ( sz & 0x3 ) ? 1 : 0 ) + sz + ( sz >> 2 ); }		// ceil( 5 * sz / 4 )
	inline size_t sizeOfBase85Binary( size_t sz ) { return static_cast< size_t >( ceil( 4 * sz / 5.0 ) ); }                             //-V113 //-V203 //-V2004

	bool operator <( const vector<uint8_t> & lhs, const vector<uint8_t> & rhs );
	char toCharFromBase16( int n );
	int toBase16FromChar( char c );
	char toCharFromBase41( unsigned int n );
	int toBase41FromChar( char c );
	char toCharFromBase85( unsigned int n );
	int toBase85FromChar( char c );
	uint64_t toHashFromBinary( const vector<uint8_t>& bin );

	// binary functions

	template<typename T>
	T toIntegerFromBinary( const vector<uint8_t>& bin, bool bigEndian = true ) // binary to any integer
	{
		T val = 0;
		const uint8_t* p = bin.data();
		size_t sz = min( sizeof( T ), bin.size() );
		if ( bigEndian )
			for ( size_t i = 0; i < sz; ++i )
				val = ( val << CHAR_BIT ) | *( p + i );
		else
			for ( size_t i = sz; i > 0; --i )
				val = ( val << CHAR_BIT ) | *( p + i - 1 );
		return val;
	}

	template<typename T>
	vector<uint8_t> toBinaryFromInteger( const T val, bool bigEndian = true ) // any integer to binary
	{
		size_t sz = sizeof( T );
		vector<uint8_t> blk( sz, 0 );
		uint8_t* p = blk.data();
		for ( size_t i = 0; i < sz; ++i )
			*( p++ ) = ( val >> ( CHAR_BIT * ( bigEndian ? ( sz - 1 - i ) : i ) ) ) & 0xff;
		return blk;
	}

	template<typename T>
	T rotateLeft( T value, size_t shift )
	{
		return ( T ) ( ( value << shift ) | ( value >> ( ( sizeof( T ) * CHAR_BIT ) - shift ) ) );
	}

	template<typename T>
	T rotateRight( T value, size_t shift )
	{
		return ( T ) ( ( value >> shift ) | ( value << ( ( sizeof( T ) * CHAR_BIT ) - shift ) ) );
	}

	vector<uint8_t> fromChksum( CommSSLChksumMD5& md5 );

}
