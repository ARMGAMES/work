#include "ProtocolMsg.h"

ProtocolMsg::ProtocolMsg()
	: ver_num( DALI_PROTOCOL_CURRENT ), user_id( 0 )
{}

ProtocolMsg::ProtocolMsg( uint32_t user_id_, const string& user_sid_ )
	: ver_num( DALI_PROTOCOL_CURRENT ), user_id( user_id_ ), user_sid( user_sid_ )
{}

ProtocolMsg::ProtocolMsg( const ProtocolMsg& msg )
	: ver_num( msg.ver_num ), user_id( msg.user_id ), user_sid( msg.user_sid ), ip_addr( msg.ip_addr )
{
	content.copyFrom( msg.content );
}

ProtocolMsg::ProtocolMsg( ProtocolMsg&& msg )
	: ver_num( msg.ver_num ), user_id( msg.user_id ), user_sid( msg.user_sid ), ip_addr( msg.ip_addr )
{
	content.moveFrom( msg.content );
}

ProtocolMsg& ProtocolMsg::operator= ( const ProtocolMsg& msg )
{
	ver_num = msg.ver_num;
	user_id = msg.user_id;
	user_sid = msg.user_sid;
	ip_addr = msg.ip_addr;
	content.copyFrom( msg.content );
	return *this;
}

ProtocolMsg& ProtocolMsg::operator= ( ProtocolMsg&& msg )
{
	ver_num = msg.ver_num;
	user_id = msg.user_id;
	user_sid = msg.user_sid;
	ip_addr = msg.ip_addr;
	content.moveFrom( msg.content );
	return *this;
}

ProtocolMsg::~ProtocolMsg()
{}

void ProtocolMsg::format()
{
	dali::cut( user_sid, USER_SID_SIZE );
	dali::cut( ip_addr, IP_ADDR_SIZE );
	if ( content._size() > MSG_CONTENT_SIZE )
		content.clear();
}

dali::MsgParser& ProtocolMsg::parse( dali::MsgParser& parser )
{
	parseHeader( parser );
	if ( ver_num == DALI_PROTOCOL_CURRENT )
		parseFooter( parseContent( parser ) );
	return parser;
}

dali::MsgComposer& ProtocolMsg::compose( dali::MsgComposer& composer ) const
{
	return composeFooter( composeContent( composeHeader( composer ) ) );
}

string ProtocolMsg::toString() const
{
	return string( "ver=" ).append( dali::intString( ver_num ) )
		.append( " user=" ).append( dali::intString( user_id ) )
		.append( " " ).append( user_sid )
		.append( " content size=" ).append( dali::intString( content._size() ) )
		.append( " ip=" ).append( ip_addr )
		;
}

dali::MsgParser& ProtocolMsg::parseHeader( dali::MsgParser& parser )
{
	parser.parseUint32( ver_num );
	if ( ver_num == DALI_PROTOCOL_CURRENT )
		parser.parseUint32( user_id ).parseString( user_sid, 2 * USER_SID_SIZE, "2 * USER_SID_SIZE" );
	dali::cut( user_sid, USER_SID_SIZE );
	return parser;
}

dali::MsgComposer& ProtocolMsg::composeHeader( dali::MsgComposer& composer ) const
{
	return composer
		.composeUint32( ver_num )
		.composeUint32( user_id )
		.composeString( user_sid )
		;
}

dali::MsgParser& ProtocolMsg::parseContent( dali::MsgParser& parser )
{
	vector<uint8_t> enc;
	parser.parseBinary( enc, 0 );
	auto dec = toDecryptedBinary( enc.data(), enc.size() );
	content.clear();
	if ( dec.size() <= MSG_CONTENT_SIZE )
	{
		dali::MsgComposer composer;
		composer.composeBytes( dec );
		content.moveFrom( composer.getMsgBody() );
	}
	return parser;
}

dali::MsgComposer& ProtocolMsg::composeContent( dali::MsgComposer& composer ) const
{
	return composer.composeBinary( toEncryptedBinary( content._readPtr(), content._size() ) );
}

dali::MsgParser& ProtocolMsg::parseFooter( dali::MsgParser& parser )
{
	if ( !parser.parseEnded() )
	{
		CommMsgBody ip_msg;
		parser.parseMsgBody( ip_msg );
		dali::MsgParser ip_parser( ip_msg );
		ip_parser.parseString( ip_addr, 2 * IP_ADDR_SIZE, "2 *  IP_ADDR_SIZE" );
		dali::cut( ip_addr, IP_ADDR_SIZE );
	}
	return parser;
}

dali::MsgComposer& ProtocolMsg::composeFooter( dali::MsgComposer& composer ) const
{
	if ( ip_addr.length() )
	{
		dali::MsgComposer ip_msg;
		composer.composeMsgBody( ip_msg.composeString( ip_addr ).getMsgBody() );
	}
	return composer;
}

static const BYTE CYPHER_BLOCK_MASK[] = { ( BYTE ) 0x42, ( BYTE ) 0x71, ( BYTE ) 0xb5, ( BYTE ) 0xe9, ( BYTE ) 0x39, ( BYTE ) 0x59, ( BYTE ) 0x92, ( BYTE ) 0xab };
static const size_t CYPHER_BLOCK_SIZE = 8;
static const size_t CYPHER_BLOCK_SIZE_EXP = 3; // base 2 exponent of block size

vector<uint8_t> ProtocolMsg::toEncryptedBinary( const BYTE* buffer, size_t sz )
{ // fast encryption by 8 bytes at a time
	vector<uint8_t> encblk;
	size_t plain_buf_sz = sz;
	size_t cypher_buf_sz = sz + CYPHER_BLOCK_SIZE;
	if ( plain_buf_sz < 1 )
		return encblk;
	encblk.resize( cypher_buf_sz, 0 );
	// initialize pointers
	const uint64_t* plain_buf_word_ptr = reinterpret_cast<const uint64_t*>( buffer );
	uint64_t* cypher_buf_word_ptr = reinterpret_cast<uint64_t*>( encblk.data() );
	// en-mask cypher
	srand( static_cast<unsigned int>( time( NULL ) ) );
	uint64_t rnd = 0;
	for ( size_t i = 0; i < 64; i += 8 )
		rnd = dali::rotateLeft( rnd, i ) ^ static_cast<uint64_t>( rand() & 0xFFF );
	const uint64_t cypher_key_word = rnd; // pseudo random 8 bytes
	const BYTE* cypher_key_byte_ptr = reinterpret_cast<const BYTE*>( &cypher_key_word );
	*cypher_buf_word_ptr++ = dali::rotateLeft( cypher_key_word, CYPHER_BLOCK_SIZE_EXP ) ^ *( reinterpret_cast<const uint64_t*>( CYPHER_BLOCK_MASK ) ); // rotate then mask
	// encrypt
	const size_t word_count = plain_buf_sz >> CYPHER_BLOCK_SIZE_EXP;
	if ( word_count > 0 )
	{
		*cypher_buf_word_ptr++ = *plain_buf_word_ptr++ ^ cypher_key_word; // first word
		for ( size_t wi = 1; wi < word_count; ++wi, ++plain_buf_word_ptr )
			*cypher_buf_word_ptr++ = *( plain_buf_word_ptr - 1 ) ^ *plain_buf_word_ptr ^ cypher_key_word;
		// tail
		const BYTE* plain_buf_byte_ptr = reinterpret_cast<const BYTE*>( plain_buf_word_ptr );
		BYTE* cypher_buf_byte_ptr = reinterpret_cast<BYTE*>( cypher_buf_word_ptr );
		for ( size_t bi = word_count << CYPHER_BLOCK_SIZE_EXP; bi < plain_buf_sz; ++bi, ++plain_buf_byte_ptr )
			*cypher_buf_byte_ptr++ = static_cast<BYTE>( *( plain_buf_byte_ptr - CYPHER_BLOCK_SIZE ) ^ *plain_buf_byte_ptr ^ *cypher_key_byte_ptr++ );
	}
	else
	{
		const BYTE* plain_buf_byte_ptr = reinterpret_cast<const BYTE*>( plain_buf_word_ptr );
		BYTE* cypher_buf_byte_ptr = reinterpret_cast<BYTE*>( cypher_buf_word_ptr );
		for ( size_t bi = 0; bi < plain_buf_sz; ++bi )
			*cypher_buf_byte_ptr++ = static_cast<BYTE>( *plain_buf_byte_ptr++ ^ *cypher_key_byte_ptr++ );
	}
	return encblk;
}

vector<uint8_t> ProtocolMsg::toDecryptedBinary( const BYTE* buffer, size_t sz )
{ // fast decryption by 8 bytes at a time
	vector<uint8_t> decblk;
	size_t plain_buf_sz = sz - CYPHER_BLOCK_SIZE;
	if ( plain_buf_sz < 1 )
		return decblk;
	decblk.resize( plain_buf_sz, 0 );
	// initialize pointers
	uint64_t* plain_buf_word_ptr = reinterpret_cast<uint64_t*>( decblk.data() );
	const uint64_t *cypher_buf_word_ptr = reinterpret_cast<const uint64_t*>( buffer );
	// de-mask cypher
	const uint64_t cypher_key_word = dali::rotateRight( *cypher_buf_word_ptr++ ^ *( reinterpret_cast<const uint64_t*>( CYPHER_BLOCK_MASK ) ), CYPHER_BLOCK_SIZE_EXP ); // mask then rotate
	const BYTE* cypher_key_byte_ptr = reinterpret_cast<const BYTE*>( &cypher_key_word );
	// decrypt
	const size_t word_count = plain_buf_sz >> CYPHER_BLOCK_SIZE_EXP;
	if ( word_count > 0 )
	{
		*plain_buf_word_ptr++ = *cypher_buf_word_ptr++ ^ cypher_key_word; // first word
		for ( size_t wi = 1; wi < word_count; ++wi, ++plain_buf_word_ptr )
			*plain_buf_word_ptr = *( plain_buf_word_ptr - 1 ) ^ *cypher_buf_word_ptr++ ^ cypher_key_word;
		// tail
		BYTE* plain_buf_byte_ptr = reinterpret_cast<BYTE*>( plain_buf_word_ptr );
		const BYTE* cypher_buf_byte_ptr = reinterpret_cast<const BYTE*>( cypher_buf_word_ptr );
		for ( size_t bi = word_count << CYPHER_BLOCK_SIZE_EXP; bi < plain_buf_sz; ++bi, ++plain_buf_byte_ptr )
			*plain_buf_byte_ptr = static_cast<BYTE>( *( plain_buf_byte_ptr - CYPHER_BLOCK_SIZE ) ^ *cypher_buf_byte_ptr++ ^ *cypher_key_byte_ptr++ );
	}
	else
	{
		BYTE* plain_buf_byte_ptr = reinterpret_cast<BYTE*>( plain_buf_word_ptr );
		const BYTE* cypher_buf_byte_ptr = reinterpret_cast<const BYTE*>( cypher_buf_word_ptr );
		for ( size_t bi = 0; bi < plain_buf_sz; ++bi )
			*plain_buf_byte_ptr++ = static_cast<BYTE>( *cypher_buf_byte_ptr++ ^ *cypher_key_byte_ptr++ );
	}
	return decblk;
}

inline bool flagExpressionLineSeparator( char c )
{
	return c == '\r' || c == '\n' || ( c >= 28 && c < 32 );
}

inline bool flagExpressionWordSeparator( char c )
{
	return c == ':';
}

ProtocolMsg::DirectiveEnum ProtocolMsg::parseDirective( const string& str )
{
	auto dir = dali::toUpper( str );
	if ( !dir.compare( "CATALOG" ) )			return DIRECTIVE_CATALOG;
	if ( !dir.compare( "ACCOUNT" ) )			return DIRECTIVE_ACCOUNT;
	if ( !dir.compare( "DEVICE" ) )				return DIRECTIVE_DEVICE;
	if ( !dir.compare( "DISPLAY" ) )			return DIRECTIVE_DISPLAY;
	if ( !dir.compare( "PROCESS" ) )			return DIRECTIVE_PROCESS;
	if ( !dir.compare( "TAG" ) )				return DIRECTIVE_TAG;
	if ( !dir.compare( "FILE" ) )				return DIRECTIVE_FILE;
	if ( !dir.compare( "PLAY" ) )				return DIRECTIVE_PLAY;
	if ( !dir.compare( "PLAYS_15D" ) )			return DIRECTIVE_PLAYS_15D;
	if ( !dir.compare( "PLAYS_30D" ) )			return DIRECTIVE_PLAYS_30D;
	if ( !dir.compare( "PLAYS_60D" ) )			return DIRECTIVE_PLAYS_60D;
	if ( !dir.compare( "BHSM" ) )				return DIRECTIVE_PLAY;
	if ( !dir.compare( "TURN" ) )				return DIRECTIVE_TURN;
	if ( !dir.compare( "TURNS_15D" ) )			return DIRECTIVE_TURNS_15D;
	if ( !dir.compare( "TURNS_30D" ) )			return DIRECTIVE_TURNS_30D;
	if ( !dir.compare( "TURNS_60D" ) )			return DIRECTIVE_TURNS_60D;
	if ( !dir.compare( "TABLE" ) )				return DIRECTIVE_TABLE;			// return table info
	if ( !dir.compare( "NOTIFY" ) )				return DIRECTIVE_NOTIFY;		// notify user in the app
	if ( !dir.compare( "CONSENT" ) )			return DIRECTIVE_CONSENT;		// conditionally block sit-in if user does not consent by selecting yes in dialog box
	if ( !dir.compare( "BLOCK" ) )				return DIRECTIVE_BLOCK;			// block sit-in
	if ( !dir.compare( "UNBLOCK" ) )			return DIRECTIVE_UNBLOCK;		// unblock sit-in
	if ( !dir.compare( "LOGOUT" ) )				return DIRECTIVE_LOGOUT;		// force logout
	if ( !dir.compare( "SELECT" ) )				return DIRECTIVE_SELECT;		// select image test
	if ( !dir.compare( "CHAT" ) )				return DIRECTIVE_CHAT;
	if ( !dir.compare( "WINDOW" ) )				return DIRECTIVE_WINDOW;
	if ( !dir.compare( "VIEW" ) )				return DIRECTIVE_VIEW;
	if ( !dir.compare( "AREA" ) )				return DIRECTIVE_AREA;
	if ( !dir.compare( "CARD" ) )				return DIRECTIVE_CARD;
	if ( !dir.compare( "DECK" ) )				return DIRECTIVE_DECK;
	if ( !dir.compare( "CHIP" ) )				return DIRECTIVE_CHIP;
	if ( !dir.compare( "LOG" ) )				return DIRECTIVE_LOG;			// log
	if ( !dir.compare( "SAMPLING" ) )			return DIRECTIVE_SAMPLING;		// sampling level -1, 1
	if ( !dir.compare( "REPORTING" ) )			return DIRECTIVE_REPORTING;		// reporting level -1, 1
	if ( !dir.compare( "REVIEW" ) )				return DIRECTIVE_REVIEW;		// create review
	if ( !dir.compare( "CASE" ) )				return DIRECTIVE_CASE;			// create case
	if ( !dir.compare( "EMAIL" ) )				return DIRECTIVE_EMAIL;			// send email to user
	if ( !dir.compare( "COMMENT" ) )			return DIRECTIVE_COMMENT;		// comment account
	if ( !dir.compare( "CLOSE" ) )				return DIRECTIVE_CLOSE;			// close account
	if ( !dir.compare( "REOPEN" ) )				return DIRECTIVE_REOPEN;		// reopen account
	if ( !dir.compare( "SUSPEND" ) )			return DIRECTIVE_SUSPEND;		// suspend real money play
	if ( !dir.compare( "RESUME" ) )				return DIRECTIVE_RESUME;		// resume real money play
	if ( !dir.compare( "REACTION_DAILY" ) )		return DIRECTIVE_REACTION_DAILY;// reaction daily limit
	if ( !dir.compare( "GEO_LOCATION" ) )		return DIRECTIVE_GEO_LOCATION;	// geo location msg
	if ( !dir.compare( "REVIEW_EVENT" ) )		return DIRECTIVE_REVIEW_EVENT;
	return DIRECTIVE_UNKNOWN;
}

const char*  ProtocolMsg::stringDirective( ProtocolMsg::DirectiveEnum  dir )
{
	switch ( dir )
	{
		case DIRECTIVE_CATALOG:				return "CATALOG";
		case DIRECTIVE_ACCOUNT:				return "ACCOUNT";
		case DIRECTIVE_DEVICE:				return "DEVICE";
		case DIRECTIVE_PROCESS:				return "PROCESS";
		case DIRECTIVE_DISPLAY:				return "DISPLAY";
		case DIRECTIVE_TAG:					return "TAG";
		case DIRECTIVE_FILE:				return "FILE";
		case DIRECTIVE_PLAY:				return "PLAY";
		case DIRECTIVE_PLAYS_15D:			return "PLAYS_15D";
		case DIRECTIVE_PLAYS_30D:			return "PLAYS_30D";
		case DIRECTIVE_PLAYS_60D:			return "PLAYS_60D";
		case DIRECTIVE_TURN:				return "TURN";
		case DIRECTIVE_TURNS_15D:			return "TURNS_15D";
		case DIRECTIVE_TURNS_30D:			return "TURNS_30D";
		case DIRECTIVE_TURNS_60D:			return "TURNS_60D";
		case DIRECTIVE_TABLE:				return "TABLE";			// return table info
		case DIRECTIVE_NOTIFY:				return "NOTIFY";		// notify user in the app
		case DIRECTIVE_CONSENT:				return "CONSENT";		// conditionally block sit-in if user does not consent by selecting yes in dialog box
		case DIRECTIVE_BLOCK:				return "BLOCK";			// block sit-in
		case DIRECTIVE_UNBLOCK:				return "UNBLOCK";		// unblock sit-in
		case DIRECTIVE_LOGOUT:				return "LOGOUT";		// force logout
		case DIRECTIVE_SELECT:				return "SELECT";		// select image test
		case DIRECTIVE_CHAT:				return "CHAT";
		case DIRECTIVE_WINDOW:				return "WINDOW";
		case DIRECTIVE_VIEW:				return "VIEW";
		case DIRECTIVE_AREA:				return "AREA";
		case DIRECTIVE_CARD:				return "CARD";
		case DIRECTIVE_DECK:				return "DECK";
		case DIRECTIVE_CHIP:				return "CHIP";
		case DIRECTIVE_LOG:					return "LOG";			// log
		case DIRECTIVE_SAMPLING:			return "SAMPLING";		// sampling level -1, 1
		case DIRECTIVE_REPORTING:			return "REPORTING";		// reporting level -1, 1
		case DIRECTIVE_REVIEW:				return "REVIEW";		// create review
		case DIRECTIVE_CASE:				return "CASE";			// create case
		case DIRECTIVE_EMAIL:				return "EMAIL";			// send email to user
		case DIRECTIVE_COMMENT:				return "COMMENT";		// comment account
		case DIRECTIVE_CLOSE:				return "CLOSE";			// close account
		case DIRECTIVE_REOPEN:				return "REOPEN";		// reopen account
		case DIRECTIVE_SUSPEND:				return "SUSPEND";		// suspend real money play
		case DIRECTIVE_RESUME:				return "RESUME";		// resume real money play
		case DIRECTIVE_REACTION_DAILY:		return "REACTION_DAILY";// reaction daily limit
		case DIRECTIVE_GEO_LOCATION:		return "GEO_LOCATION";	// geo location msg
		case DIRECTIVE_REVIEW_EVENT:		return "REVIEW_EVENT";
		case DIRECTIVE_UNKNOWN:
		default:							return NULL;
	}
}

vector< pair< ProtocolMsg::DirectiveEnum, string > > ProtocolMsg::parseExpression( const char* expr )
{
	vector< pair< DirectiveEnum, string > > retval;
	for ( const char* p = expr; *p; ++p )
	{
		if ( flagExpressionWordSeparator( *p ) || flagExpressionLineSeparator( *p ) )
			continue; // expression line starting with word separator is ignored
		const char* dirp = p;
		while ( !flagExpressionWordSeparator( *p ) && !flagExpressionLineSeparator( *p ) && *p )
			++p; // iterate to the end of word or line or expr
		string dir( dirp, p );
		DirectiveEnum d = parseDirective( dali::trim( dir ) );
		if ( flagExpressionWordSeparator( *p ) )
		{
			const char* prmp = ++p;
			while ( !flagExpressionLineSeparator( *p ) && *p )
				++p; // iterate to the end of line or expr
			string prm( prmp, p );
			retval.emplace_back( d, dali::trim( prm ) );
		}
		else
			retval.emplace_back( d, string() );
		if ( !*p )
			break;
	}
	return retval;
}

string ProtocolMsg::stringExpression( const vector< pair< DirectiveEnum, string > > vec )
{
	string retval;
	for ( const auto& e : vec )
	{
		auto dir = stringDirective( e.first );
		if ( dir )
		{
			retval.append( dir );
			if ( e.second.length() )
				retval.append( 1, ':' ).append( e.second );
			retval.append( 1, '\n' );
		}
	}
	return retval;
}
