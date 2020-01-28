#ifndef __DALI_COMMAND_MSG_H__
#define __DALI_COMMAND_MSG_H__

#include "BaseMsg.h"

struct CommandMsg // dali command data
	: public BaseMsg
{

	struct Key
		: public BaseMsg::Key
	{

		string ref_id;

		Key( uint32_t uid = svclib::NULL_U32 )
			: BaseMsg::Key( uid )
			, ref_id( svclib::NULL_STR )
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

		dali::MsgParser& parse( dali::MsgParser& parser ) override
		{
			return BaseMsg::Key::parse( parser )
				.parseString( ref_id, 2 * REF_ID_SIZE, "2 *REF_ID_SIZE" )
				;
		}

		dali::MsgComposer& compose( dali::MsgComposer& composer ) const override
		{
			return BaseMsg::Key::compose( composer )
				.composeString( ref_id )
				;
		}

		virtual void format() override
		{
			BaseMsg::Key::format();
			dali::cut( ref_id, REF_ID_SIZE );
		}

		string toString() const override
		{
			return BaseMsg::Key::toString()
				.append( " ref=" ).append( ref_id )
				;
		}

	};

	uint32_t agent_id;
	string ref_id;
	string command_expr;
	vector< vector<uint8_t> > contents;

	CommandMsg()
		: agent_id( 0 )
	{}

	CommandMsg( const Key & key )
		: BaseMsg( key )
		, agent_id( 0 )
		, ref_id( key.ref_id )
	{}

	CommandMsg(	uint32_t user_id_
		, uint32_t agent_id_
		, const string& command_expr_
	)	: BaseMsg( user_id_ )
		, agent_id( agent_id_ )
		, command_expr( command_expr_ )
	{}

	void format() override
	{
		BaseMsg::format();
		dali::cut( ref_id, REF_ID_SIZE );
		dali::cut( command_expr, COMMAND_EXPR_SIZE );
		while ( contents.size() > COMMAND_CONTENTS_SIZE )
			contents.pop_back();
		for ( auto& e : contents )
			dali::cut( e, COMMAND_CONTENT_SIZE );
	}

	dali::MsgParser& parse( dali::MsgParser& parser ) override
	{
		uint32_t sz = 0;
		BaseMsg::parse( parser )
			.parseUint32( agent_id )
			.parseString( ref_id, 2 * REF_ID_SIZE, "2 *REF_ID_SIZE" )
			.parseString( command_expr, 2 * COMMAND_EXPR_SIZE, "2 *COMMAND_EXPR_SIZE" )
			.parseUint32( sz )
			;
		if ( sz > COMMAND_CONTENTS_SIZE )
			sz = COMMAND_CONTENTS_SIZE;
		contents.resize( sz );
		for ( uint32_t i = 0; i < sz; ++i )
			parser.parseBinary( contents[ i ], 0 );
		format();
		return parser;
	}
	
	dali::MsgComposer& compose( dali::MsgComposer& composer ) const override
	{
		uint32_t sz = static_cast< uint32_t >( contents.size() );
		BaseMsg::compose( composer )
			.composeUint32( agent_id )
			.composeString( ref_id )
			.composeString( command_expr )
			.composeUint32( sz )
			;
		for ( uint32_t i = 0; i < sz; ++i )
			composer.composeBinary( contents[ i ] );
		return composer;
	}

	string toString() const override
	{
		return BaseMsg::toString()
			.append( " agent=" ).append( dali::intString( agent_id ) )
			.append( " ref=" ).append( ref_id )
			.append( " command=" ).append( command_expr )
			.append( " contents=" ).append( dali::intString( contents.size() ) )
			;
	}

	vector< string > popStringVec( size_t size, size_t ix = 0 )
	{
		vector< string > vec;
		if ( ( size + ix ) <= contents.size() )
			for ( auto i = contents.begin() + ( int64_t ) ix; i != contents.end() && size; i = contents.erase( i ), --size )
				vec.emplace_back( dali::toString( *i ) );
		return vec;
	}

	size_t pushStringVec( const vector< string >& vec )
	{
		for ( const auto& data : vec )
		{
			contents.emplace_back( data.c_str(), data.c_str() + data.length() + 1 );
		}
		return vec.size();
	}

	vector< vector<uint8_t> > popBinaryVec( size_t size, size_t ix = 0 )
	{
		vector< vector<uint8_t> > vec;
		if ( ( size + ix ) <= contents.size() )
			for ( auto i = contents.begin() + ( int64_t ) ix; i != contents.end() && size; i = contents.erase( i ), --size )
				vec.push_back( *i );
		return vec;
	}

	size_t pushBinaryVec( const vector< vector<uint8_t> >& vec )
	{
		contents.insert( contents.end(), vec.begin(), vec.end() );
		return vec.size();
	}

	typedef TimeMsg::Query< Key > Query;

};

#endif
