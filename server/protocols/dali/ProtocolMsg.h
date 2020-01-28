#ifndef __DALI_CONTENT_MSG_H__
#define __DALI_CONTENT_MSG_H__

#include "IMsg.h"

#define USER_SID_SIZE								20
#define IP_ADDR_SIZE								45
#define MSG_CONTENT_SIZE							67108864 // 64MiB

struct ProtocolMsg
	: IMsg
{

	enum ContentEnum
	{
		CONTENT_UNKNOWN			= 0
		, CONTENT_COMMAND		= 100
		, CONTENT_RESPONSE		= 110
		, CONTENT_TABLE			= 300
		, CONTENT_WAITLIST		= 330
		, CONTENT_RESERVATION   = 350
		, CONTENT_DEVICE		= 400
		, CONTENT_GEO_LOCATION  = 404 // GPSI-74
		, CONTENT_PROCESS		= 410
		, CONTENT_DISPLAY		= 430
		, CONTENT_TAG			= 440
		, CONTENT_FILE			= 450
		, CONTENT_EXEC			= 490
		, CONTENT_PLAY			= 500
		, CONTENT_TURN			= 510
	};

	enum DirectiveEnum
	{
		DIRECTIVE_UNKNOWN = 0
		, DIRECTIVE_CATALOG
		, DIRECTIVE_ACCOUNT
		, DIRECTIVE_TABLE
		, DIRECTIVE_DEVICE
		, DIRECTIVE_PROCESS
		, DIRECTIVE_DISPLAY
		, DIRECTIVE_TAG 
		, DIRECTIVE_FILE
		, DIRECTIVE_PLAY
		, DIRECTIVE_PLAYS_15D
		, DIRECTIVE_PLAYS_30D
		, DIRECTIVE_PLAYS_60D
		, DIRECTIVE_TURN
		, DIRECTIVE_TURNS_15D
		, DIRECTIVE_TURNS_30D
		, DIRECTIVE_TURNS_60D
		, DIRECTIVE_NOTIFY				// notify user in the app
		, DIRECTIVE_CONSENT				// conditionally block sit-in if user does not consent by selecting yes in dialog box
		, DIRECTIVE_BLOCK				// block sit-in
		, DIRECTIVE_UNBLOCK				// unblock sit-in
		, DIRECTIVE_LOGOUT				// force logout
		, DIRECTIVE_SELECT				// select image test
		, DIRECTIVE_CHAT				// SILENT, NORMAL, or ALL
		, DIRECTIVE_WINDOW				// -1, 1
		, DIRECTIVE_VIEW				// -1, 1
		, DIRECTIVE_AREA				// -1, 1
		, DIRECTIVE_CARD				// -1, 1
		, DIRECTIVE_DECK				// SMP, STD, ALT, or COLOR
		, DIRECTIVE_CHIP				// POS
		, DIRECTIVE_LOG					// log
		, DIRECTIVE_SAMPLING			// -x, x
		, DIRECTIVE_REPORTING			// -x, x
		, DIRECTIVE_REVIEW				// create review
		, DIRECTIVE_CASE				// create case
		, DIRECTIVE_EMAIL				// send email to user
		, DIRECTIVE_COMMENT				// comment account
		, DIRECTIVE_CLOSE				// close account
		, DIRECTIVE_REOPEN				// reopen account
		, DIRECTIVE_SUSPEND				// suspend real money play
		, DIRECTIVE_RESUME				// resume real money play
		, DIRECTIVE_REACTION_DAILY		// reaction daily limit
		, DIRECTIVE_GEO_LOCATION		// geo location msg
		, DIRECTIVE_REVIEW_EVENT
	};

	uint32_t ver_num;
	uint32_t user_id;
	string user_sid;
	CommMsgBody content;
	string ip_addr;

	ProtocolMsg();
	ProtocolMsg( uint32_t user_id_, const string& user_sid_ );
	ProtocolMsg( const ProtocolMsg& msg );
	ProtocolMsg( ProtocolMsg&& msg );
	ProtocolMsg& operator= ( const ProtocolMsg& msg );
	ProtocolMsg& operator= ( ProtocolMsg&& msg );
	virtual ~ProtocolMsg();

	void format() override;
	dali::MsgParser& parse( dali::MsgParser& parser ) override;
	dali::MsgComposer& compose( dali::MsgComposer& composer ) const override;
	string toString() const override;
	dali::MsgParser& parseHeader( dali::MsgParser& parser );
	dali::MsgComposer& composeHeader( dali::MsgComposer& composer ) const;
	dali::MsgParser& parseContent( dali::MsgParser& parser );
	dali::MsgComposer& composeContent( dali::MsgComposer& composer ) const;
	dali::MsgParser& parseFooter( dali::MsgParser& parser );
	dali::MsgComposer& composeFooter( dali::MsgComposer& composer ) const;

	template< typename T >
	inline void appendContent( ContentEnum contype, T& coll )
	{
		dali::MsgComposer composer( content );
		composer.composeUint32( static_cast< uint32_t >( contype ) ).composeIterable( dali::formatIterable( coll ) );
		content.moveFrom( composer.getMsgBody() );
	}

	static vector<uint8_t> toEncryptedBinary( const BYTE* buffer, size_t sz );
	static vector<uint8_t> toDecryptedBinary( const BYTE* buffer, size_t sz );
	static DirectiveEnum parseDirective( const string& dir );
	static const char* stringDirective( DirectiveEnum dir );
	static vector< pair< DirectiveEnum, string > > parseExpression( const char* expr );
	static string stringExpression( const vector< pair< DirectiveEnum, string > > vec );

};

#endif
