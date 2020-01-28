#ifndef __DALI_PROTOCOL_H__
#define __DALI_PROTOCOL_H__

#include "svclib/svclib.h"

#define MSG_DALI_SERVER_TO_CLIENT					3550
#define MSG_DALI_SERVER_TO_CLIENT_REPLY				3551
#define MSG_DALI_SERVER_TO_SERVER					4550
#define MSG_DALI_SERVER_TO_SERVER_REPLY				4551
#define MSG_DALI_CLIENT_TO_SERVER					5550
#define MSG_DALI_CLIENT_TO_SERVER_REPLY				5551

#define DALI_PROTOCOL_20170200						0
#define DALI_PROTOCOL_20170400						20170400	// YYYY-MM-RR ( YEAR-MONTH-REVISION )
#define DALI_PROTOCOL_20180400						20180400	// YYYY-MM-RR ( YEAR-MONTH-REVISION )
#define DALI_PROTOCOL_CURRENT						DALI_PROTOCOL_20180400

#define NUM_SCALE_FACTOR							1000

namespace dali
{
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

	string trim( const char* s );
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
	string quoteString( const char* str, char quoteChar = '"', const char escapeChar = '\\' );
	inline string quoteString( const string& str, const char quoteChar = '"', const char escapeChar = '\\' ) { return quoteString( str.c_str(), quoteChar, escapeChar ); };

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
	inline size_t sizeOfBase41Binary( size_t sz ) { return static_cast< size_t >( ceil( 2 * sz / 3.0 ) ); }
	inline size_t sizeOfBase85String( size_t sz ) { return ( ( sz & 0x3 ) ? 1 : 0 ) + sz + ( sz >> 2 ); }		// ceil( 5 * sz / 4 )
	inline size_t sizeOfBase85Binary( size_t sz ) { return static_cast< size_t >( ceil( 4 * sz / 5.0 ) ); }

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

	// iterable functions

	template< typename T >
	inline T& formatIterable( T& coll )
	{
		for( typename T::iterator i = coll.begin(); i != coll.end(); ++i )
			i->format();
		return coll;
	}

	// Composer class

	class MsgComposer
	{

		CommMsgBody msgBody;

	public:

		MsgComposer() {}
		MsgComposer( const MsgComposer& other ) { msgBody.copyFrom( other.getMsgBody() ); }
		MsgComposer( MsgComposer&& other ) : msgBody( move( other.getMsgBody() ) ) {}
		MsgComposer( const CommMsgBody& other ) { msgBody.copyFrom( other ); }
		MsgComposer( CommMsgBody&& other ) : msgBody( move( other ) ) {}

		// Implicit conversion operator for classes that only implement CommMsgParser& parse(CommMsgParser&) and cannot depend on dali/protocol.
		operator CommMsgBody&( ) { return msgBody; }

		inline CommMsgBody& getMsgBody() { return msgBody; }
		inline const CommMsgBody& getMsgBody() const { return msgBody; }

		inline MsgComposer& composeBool( bool b ) { msgBody.composeBOOL( b ); return *this; }
		inline MsgComposer& composeInt8( int8_t i ) { msgBody.composeINT8( i ); return *this; }
		inline MsgComposer& composeByte( uint8_t b ) { msgBody.composeBYTE( b ); return *this; }
		inline MsgComposer& composeInt16( int16_t i ) { msgBody.composeINT16( i ); return *this; }
		inline MsgComposer& composeUint16( uint16_t i ) { msgBody.composeUINT16( i ); return *this; }
		inline MsgComposer& composeInt32( int32_t i ) { msgBody.composeINT32( i ); return *this; }
		inline MsgComposer& composeUint32( uint32_t i ) { msgBody.composeUINT32( i ); return *this; }
		inline MsgComposer& composeInt64( int64_t i ) { msgBody.composeINT64( i ); return *this; }
		inline MsgComposer& composeUint64( uint64_t i ) { msgBody.composeUINT64( i ); return *this; }
		inline MsgComposer& composeString( const char* s ) { msgBody.composeString( s ); return *this; }
		inline MsgComposer& composeString( const string& s ) { msgBody.composeString( s.c_str() ); return *this; }
		inline MsgComposer& composeBinary( const vector<uint8_t>& bin ) { msgBody._composeVarBlock( bin.data(), bin.size() ); return *this; }
		inline MsgComposer& composeBytes( const vector<uint8_t>& bin ) { msgBody._composeFixedBlock( bin.data(), bin.size() ); return *this; }

		inline MsgComposer& composeMsgBody( const CommMsgBody& m ) { msgBody.composeMsgBody( m ); return *this; }
		inline MsgComposer& composeMsgBodySkipReqId( const CommMsgBody& m ) { msgBody.composeMsgBodySkipReqId( m ); return *this; }
		inline MsgComposer& copyMsgBody( const CommMsgBody& msg, size_t i=0 ) { msgBody._composeFixedBlock( msg._readPtr()+i, msg._size()-i ); return *this; }

		inline MsgComposer& composeSrvTime( const CommSrvTime& srvTime ) { msgBody.composeSrvTime( srvTime ); return *this; }
		inline MsgComposer& composeSrvDate( const CommSrvDate& srvDate ) { msgBody.composeSrvDate( srvDate ); return *this; }

		inline MsgComposer& composeUtcTime( const CommUtcTime& utcTime ) { utcTime.compose( msgBody ); return *this; }
		inline MsgComposer& composeUtcDate( const CommUtcDate& utcDate ) { utcDate.compose( msgBody ); return *this; }

		template< typename T > inline MsgComposer& composeObject( const T& obj ) { obj.compose( *this ); return *this; }
		
		MsgComposer& composeVector( const vector<uint32_t>& vec );
		MsgComposer& composeVector( const vector<uint64_t>& vec );
		MsgComposer& composeVector( const vector<string>& vec );
		template< typename T >
		inline MsgComposer& composeVector( const vector<T>& vec )
		{
			composeUint32( (uint32_t)vec.size() );
			for ( size_t i = 0; i < vec.size(); ++i )
			{
				vec[i].compose( *this );
			}
			return *this;
		}

		template< typename T >
		inline MsgComposer& composeIterable( const T& coll )
		{
			composeUint32( static_cast< uint32_t >( coll.size() ) );
			for( typename T::const_iterator i = coll.begin(); i != coll.end(); ++i )
			{
				MsgComposer composer;
				composeMsgBody( i->compose( composer ).getMsgBody() );
			}
			return *this;
		}

		template< typename T >
		inline MsgComposer& composeOptional( const shared_ptr<T>& o )
		{
			MsgComposer composer;
			if ( o )
				composeBool( true ).composeMsgBody( o->compose( composer ).getMsgBody() );
			else
				composeBool( false );
			return *this;
		}

		inline void copyFrom( const CommMsgBody& other ) { msgBody.copyFrom( other ); }
		inline void copyFrom( const MsgComposer& other ) { copyFrom( other.getMsgBody() ); }
		inline void moveFrom( CommMsgBody& other ) { msgBody.moveFrom( other ); }
		inline void moveFrom( MsgComposer& other ) { moveFrom( other.getMsgBody() ); }

		inline const uint8_t* data() const { return msgBody._readPtr(); }
		inline size_t size() const { return msgBody._size(); }
		inline void clear() { msgBody.clear(); }

	private: // prohibit default assignment operator

		MsgComposer& operator=( const MsgComposer& other );

	};

	// Parser class

	class MsgParser
	{

		CommMsgParser parser;

	public:

		MsgParser( const CommMsgBody& other ) : parser( other ) {}

		// Implicit conversion operator for classes that only implement CommMsgParser& parse(CommMsgParser&) and cannot depend on dali/protocol.
		operator CommMsgParser&( ) { return parser; }

		inline MsgParser& parseBool( bool& ret ) { parser.parseBOOL( ret ); return *this; }
		inline MsgParser& parseInt8( int8_t& ret ) { parser.parseINT8( ret );  return *this; }
		inline MsgParser& parseByte( uint8_t& ret ) { parser.parseBYTE( ret );  return *this; }
		inline MsgParser& parseInt16( int16_t& ret ) { parser.parseINT16( ret );  return *this; }
		inline MsgParser& parseUint16( uint16_t& ret ) { parser.parseUINT16( ret );  return *this; }
		inline MsgParser& parseInt32( int32_t& ret )   { parser.parseINT32( ret );  return *this; }
		inline MsgParser& parseUint32( uint32_t& ret ) { parser.parseUINT32( ret );  return *this; }
		inline MsgParser& parseInt64( int64_t& ret )   { parser.parseINT64( ret );  return *this; }
		inline MsgParser& parseUint64( uint64_t& ret ) { parser.parseUINT64( ret );  return *this; }

		MsgParser& parseString( string& str, size_t maxStringLengthAllowed, const char *fieldDescr = "" );
		MsgParser& parseBinary( vector<uint8_t>& block, size_t maxBlockSizeAllowed, const char *fieldDescr = "" );

		inline MsgParser& parseMsgBody( CommMsgBody& ret, size_t maxMsgBodySizeAllowed = 0, const char *fieldDescr = "" )
		{
			parser.parseMsgBodyN( ret, maxMsgBodySizeAllowed, fieldDescr );
			return *this;
		}

		inline MsgParser& parseSrvDate( CommSrvDate & ret ) { parser.parseSrvDate( ret ); return *this; }
		inline MsgParser& parseSrvTime( CommSrvTime & ret ) { parser.parseSrvTime( ret ); return *this; }
		MsgParser& parseSrvTime( int64_t & ret );

		MsgParser& parseVector( vector<uint32_t>& vec );
		MsgParser& parseVector( vector<uint64_t>& vec );
		MsgParser& parseVector( vector<string>& vec );
		template< typename T >
		inline MsgParser& parseVector( vector<T>& vec )
		{
			uint32_t size;
			parseUint32( size );
			vec.resize( size );
			for ( uint32_t i = 0; i < size; ++i )
			{
				vec[i].parse( *this );
			}
			return *this;
		}

		template< typename T >
		MsgParser& parseIterable( T& coll )
		{
			uint32_t size = 0;
			parseUint32( size );
			coll.resize( size );
			for( typename T::iterator i = coll.begin(); i != coll.end(); ++i )
			{
				CommMsgBody b;
				parseMsgBody( b );
				MsgParser p( b );
				i->parse( p );
			}
			return *this;
		}

		template< typename T >
		MsgParser& parseOptional( shared_ptr<T>& o )
		{
			bool has_obj = false;
			parseBool( has_obj );
			if ( has_obj )
			{
				CommMsgBody b;
				parseMsgBody( b );
				MsgParser p( b );
				o.reset( new T );
				o->parse( p );
			}
			else
				o.reset();
			return *this;
		}

		inline bool parseEnded() { return parser.parseEnded(); }

	private: // prohibit default assignment operator
		
		MsgParser( const MsgParser & other );
		MsgParser& operator=( const MsgParser& other );

	};

}

// DALI LEGACY PROTOCOL 20170400

#define MSG_DALI_SERVER								2550		// legacy
#define MSG_DALI_SERVER_REPLY						2551		// legacy
#define MSG_DALI_CLIENT								5550		// legacy
#define MSG_DALI_CLIENT_REPLY						5551		// legacy
#define MSG_DALI_ERROR								1000000		// legacy
#define MSG_DALI_ERROR_REPLY						1000001		// legacy

// payload
#define MSG_DALI_SERVER_SESSION						2560 // TBD
#define MSG_DALI_SERVER_SESSION_REPLY				2561
#define MSG_DALI_BH_SAMPLE_REQUEST					2602 // UserMsg
#define MSG_DALI_BH_SAMPLE_REQUEST_REPLY			2603
#define MSG_DALI_EX_SAMPLE_REQUEST					2604 // UserMsg
#define MSG_DALI_EX_SAMPLE_REQUEST_REPLY			2605
#define MSG_DALI_DV_SAMPLE_REQUEST					2606 // UserMsg
#define MSG_DALI_DV_SAMPLE_REQUEST_REPLY			2607
#define MSG_DALI_FILE_SAMPLE_REQUEST				2608 // UserMsg
#define MSG_DALI_FILE_SAMPLE_REQUEST_REPLY			2609
#define MSG_DALI_USER_DIGEST_REQUEST				2620 // UserMsg
#define MSG_DALI_USER_DIGEST_REQUEST_REPLY			2621
#define MSG_DALI_USER_NOTIFICATION					2640 // UserNotification
#define MSG_DALI_USER_NOTIFICATION_REPLY			2641
#define MSG_DALI_IMAGE_MUTATION						2642 // ImageMutation
#define MSG_DALI_IMAGE_MUTATION_REPLY				2643
#define MSG_DALI_IMAGE_SELECTION					2644 // ImageSelection
#define MSG_DALI_IMAGE_SELECTION_REPLY				2645
// payload
#define MSG_DALI_CLIENT_SESSION						5560 // TBD
#define MSG_DALI_CLIENT_SESSION_REPLY				5561
#define MSG_DALI_BH_SAMPLE							5602 // BhSample
#define MSG_DALI_BH_SAMPLE_REPLY					5603
#define MSG_DALI_EX_SAMPLE							5604 // ExSample
#define MSG_DALI_EX_SAMPLE_REPLY					5605
#define MSG_DALI_DV_SAMPLE							5606 // DvSample
#define MSG_DALI_DV_SAMPLE_REPLY					5607
#define MSG_DALI_FILE_SAMPLE						5608 // FileSample
#define MSG_DALI_FILE_SAMPLE_REPLY					5609
#define MSG_DALI_USER_DIGEST						5620 // UserDigest
#define MSG_DALI_USER_DIGEST_REPLY					5621
#define MSG_DALI_USER_NOTIFICATION_RESULT			5640 // UserNotification
#define MSG_DALI_USER_NOTIFICATION_RESULT_REPLY		5641
#define MSG_DALI_IMAGE_MUTATION_RESULT				5642 // ImageMutation
#define MSG_DALI_IMAGE_MUTATION_RESULT_REPLY		5643
#define MSG_DALI_IMAGE_SELECTION_RESULT				5644 // ImageSelection
#define MSG_DALI_IMAGE_SELECTION_RESULT_REPLY		5645

// DALI LEGACY PROTOCOL 20170200

#define MSG_DALI_CYPHER					5550
#define MSG_DALI_CYPHER_REPLY			5551
#define MSG_DALI_BH_SAMPLE_SAVE			5000
#define MSG_DALI_BH_SAMPLE_SAVE_REPLY	5001
#define MSG_DALI_EX_SAMPLE_SAVE			5004
#define MSG_DALI_EX_SAMPLE_SAVE_REPLY	5005
#define MSG_DALI_HW_SAMPLE_SAVE			5008
#define MSG_DALI_HW_SAMPLE_SAVE_REPLY	5009
#define MSG_DALI_SW_SAMPLE_SAVE			5012
#define MSG_DALI_SW_SAMPLE_SAVE_REPLY	5013
#define MSG_DALI_FILE_SAMPLE_SAVE		5016
#define MSG_DALI_FILE_SAMPLE_SAVE_REPLY	5017

#endif
