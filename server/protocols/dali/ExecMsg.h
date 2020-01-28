#ifndef __DALI_EXEC_MSG_H__
#define __DALI_EXEC_MSG_H__

#include "SampleMsg.h"

#define EXEC_MSG_HEADER SAMPLE_MSG_HEADER "|Exec Name|Exec State|Content"

struct ExecMsg // dali process data
	: public SampleMsg
{

	struct Key
		: public SampleMsg::Key
	{

		string exec_name;

		Key( uint32_t uid = svclib::NULL_U32, uint64_t did = svclib::NULL_U64 )
			: SampleMsg::Key( uid, did )
			, exec_name( svclib::NULL_STR )
		{}

		Key( const Key& that )
			: SampleMsg::Key( that )
			, exec_name( that.exec_name )
		{}

		Key& operator=( const Key& that )
		{
			SampleMsg::Key::operator=( that );
			if ( this != &that )
			{
				exec_name = that.exec_name;
			}
			return *this;
		}

		void format() override
		{
			SampleMsg::Key::format();
			dali::cut( exec_name, EXEC_NAME_SIZE );
		}

		dali::MsgParser& parse( dali::MsgParser& parser ) override
		{
			SampleMsg::Key::parse( parser )
				.parseString( exec_name, 2 * EXEC_NAME_SIZE, "2 * EXEC_NAME_SIZE" )
				;
			return parser;

		}

		dali::MsgComposer& compose( dali::MsgComposer& composer ) const override
		{
			return SampleMsg::Key::compose( composer )
				.composeString( exec_name )
				;
		}

		string toString() const override
		{
			return SampleMsg::Key::toString()
				.append( " name=" ).append( exec_name )
				;
		}

	};

	enum ExecStateEnum
	{
		STATE_NONE = 0,
		STATE_START_SUCCESS = 1,
		STATE_START_FAILURE = 2,
		STATE_SUSPENSION = 3,
		STATE_USER_TERMINATION = 4,
		STATE_SYSTEM_TERMINATION = 5,
	};

	string exec_name;				// en
	uint16_t exec_state;			// es
	string content;					// ec

	ExecMsg()
		: exec_state( 0 )
	{}

	ExecMsg( const string& name, uint16_t state, const string& txt )
		: exec_name( name )
		, exec_state( state )
		, content( txt )
	{}

	ExecMsg( const Key & key )
		: SampleMsg( key )
		, exec_name( key.exec_name )
		, exec_state( 0 )
	{}

	void format() override
	{
		SampleMsg::format();
		dali::cut( exec_name, EXEC_NAME_SIZE );
		dali::cut( content, EXEC_CONTENT_SIZE );
	}

	dali::MsgParser& parse( dali::MsgParser& parser ) override
	{
		SampleMsg::parse( parser )
			.parseString( exec_name, 2 * EXEC_NAME_SIZE, "2 * EXEC_NAME_SIZE" )
			.parseUint16( exec_state )
			.parseString( content, 2 * EXEC_CONTENT_SIZE, "2 * EXEC_CONTENT_SIZE" )
			;
		format();
		return parser;
	}

	dali::MsgComposer& compose( dali::MsgComposer& composer ) const override
	{
		return SampleMsg::compose( composer )
			.composeString( exec_name )
			.composeUint16( exec_state )
			.composeString( content )
			;
	}

	string toString() const override
	{
		return SampleMsg::toString()
			.append( " name=" ).append( exec_name )
			.append( " state=" ).append( dali::intString( exec_state ) )
			.append( " content=" ).append( content )
			;
	}

	string toLine() const override
	{
		return SampleMsg::toLine()
			.append( "|" ).append( dali::quoteString( exec_name ) )
			.append( "|" ).append( dali::intString( exec_state ) )
			.append( "|" ).append( dali::quoteString( content ) )
			;
	}

	static std::string header()
	{
		return EXEC_MSG_HEADER;
	}

	typedef TimeMsg::Query< Key > Query;

};

#endif