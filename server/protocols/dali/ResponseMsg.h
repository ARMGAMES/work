#ifndef __DALI_RESPONSE_MSG_H__
#define __DALI_RESPONSE_MSG_H__

#include "CommandMsg.h"

struct ResponseMsg // dali command response data
	: public BaseMsg
{

	struct Key
		: public BaseMsg::Key
	{

		string ref_id;

		Key( uint32_t uid = svclib::NULL_U32 )
			: BaseMsg::Key( uid )
			, ref_id ( svclib::NULL_STR )
		{}

		Key( const Key& that )
			: BaseMsg::Key( that )
			, ref_id( that.ref_id )
		{}

		Key& operator=( const Key& that )
		{
			BaseMsg::Key::operator=( that );
			if ( this != &that )
			{
				ref_id = that.ref_id;
			}
			return *this;
		}

		void format() override
		{
			BaseMsg::Key::format();
			dali::cut( ref_id, REF_ID_SIZE );
		}

		dali::MsgParser& parse( dali::MsgParser& parser ) override
		{
			return BaseMsg::Key::parse( parser )
				.parseString( ref_id, REF_ID_SIZE )
				;

		}

		dali::MsgComposer& compose( dali::MsgComposer& composer ) const override
		{
			return BaseMsg::Key::compose( composer )
				.composeString( ref_id )
				;
		}

		string toString() const override
		{
			return BaseMsg::Key::toString()
				.append( " ref_id=" ).append( ref_id )
				;
		}

	};

	string ref_id;
	string response_expr;

	ResponseMsg()
	{}

	ResponseMsg( const CommandMsg& cmd )
		: BaseMsg( cmd.user_id )
		, ref_id( cmd.ref_id )
	{}

	ResponseMsg( const Key & key )
		: BaseMsg( key )
		, ref_id( key.ref_id )
	{}

	void format() override
	{
		BaseMsg::format();
		dali::cut( ref_id, REF_ID_SIZE );
		dali::cut( response_expr, RESPONSE_EXPR_SIZE );
	}

	dali::MsgParser& parse( dali::MsgParser& parser ) override
	{
		BaseMsg::parse( parser )
			.parseString( ref_id, 2 * REF_ID_SIZE, "2 * REF_ID_SIZE" )
			.parseString( response_expr, 2 * RESPONSE_EXPR_SIZE, "2 * RESPONSE_EXPR_SIZE" )
			;
		format();
		return parser;
	}

	dali::MsgComposer& compose( dali::MsgComposer& composer ) const override
	{
		return BaseMsg::compose( composer )
			.composeString( ref_id )
			.composeString( response_expr )
			;
	}

	string toString() const override
	{
		return BaseMsg::toString()
			.append( " ref=" ).append( ref_id )
			.append( " response=" ).append( response_expr )
			;
	}

	typedef TimeMsg::Query< Key > Query;

};

#endif
