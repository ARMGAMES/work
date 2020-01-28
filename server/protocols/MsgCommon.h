#ifndef MsgCommon_h_included
#define MsgCommon_h_included

#include "commsrv.h"

// generic message response class
class MsgGenericResponse
{
public:
	UINT16 resultCode;
	const char* errorText;

	// constructor
	MsgGenericResponse( UINT16 _resultCode = 0, const char* _errorText = "" )
	: resultCode( _resultCode )
	, errorText( _errorText )
	{
	}

	// copy constructor
	MsgGenericResponse( const MsgGenericResponse& source )
	: resultCode( source.resultCode )
	, errorText( source.errorText ) //lint !e1554 : copying a pointer
	{}

	// construct from parser
	MsgGenericResponse( CommMsgParser& parser )
	: resultCode( 0 )
	, errorText( "" )
	{
		parse( parser );
	}

	MsgGenericResponse& operator = (const MsgGenericResponse&) = delete;

	// parse the generic response
	CommMsgParser& parse( CommMsgParser& parser )
	{
		parser.parseUINT16(resultCode);
		if( resultCode != 0 )
			parser.parseString(errorText);
		return parser;
	}

	// compose the generic response
	CommMsgBody& compose( CommMsgBody& msg ) const
	{
		msg.composeUINT16(resultCode);
		if( resultCode != 0 )
			msg.composeString(errorText);
		return msg;
	}

};

// similar to MsgGenericResponse but can be thrown as an exception
// (carries error text in PString)
class MsgError
{
public:
	UINT16 resultCode;
	PString errorText;

	// default constructor
	MsgError()
		: resultCode( 0 )
	{}

	MsgError( CommMsgParser& parser )
	{
		parser.parseUINT16(resultCode);
		if( resultCode != 0 )
			parser.parseStringP(errorText);
	}

	MsgError( UINT16 _resultCode, const char* _errorText )
		: resultCode( _resultCode )
		, errorText( _errorText )
	{
	}

	CommMsgBody& compose( CommMsgBody& msg ) const
	{
		msg.composeUINT16(resultCode);
		if( resultCode != 0 )
			msg.composeString(errorText);
		return msg;
	}
};


// class for messages with empty body
class MsgEmptyBody
{
public:
	// default constructor
	MsgEmptyBody()
	{}

	// constructor from a parser
	MsgEmptyBody( const CommMsgParser& parser )
	{
		PASSERT3( parser.parseEnded() );
	}

	CommMsgBody& compose( CommMsgBody& msg ) const
	{
		return msg;
	}
};

// generic user request
class MsgUserRequest 
{
public:
	const char* userId;

	// construct from incoming message
	MsgUserRequest( CommMsgParser& parser )
	{
		parser.parseString(userId);
	}

	// construct for outgoing message
	MsgUserRequest( const char* _userId )
	{
		userId = _userId;
	}

	// compose outgoing message
	CommMsgBody& compose( CommMsgBody& msg ) const
	{
		return msg.composeString(userId);
	}
};


// unit test to validate message composition/parsing integrity
class MsgGenericResponseTest
{
public:
	static void run()
	{
		CommMsgBody body;
		MsgGenericResponse msg1(5,"Error message");
		msg1.compose(body);
		CommMsgParser parser(body);
		MsgGenericResponse msg2(parser);
		PASSERT( msg1.resultCode == msg2.resultCode );
		PASSERT( strcmp(msg1.errorText, msg2.errorText)==0 );
	}
};


#endif
