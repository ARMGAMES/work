#ifndef __DALI_FILE_MSG_H__
#define __DALI_FILE_MSG_H__

#include "SampleMsg.h"

struct FileMsg // dali device file data
	: public SampleMsg
{

	struct Key
		: public SampleMsg::Key
	{

		string file_name;

		Key( uint32_t uid = svclib::NULL_U32, uint64_t did = svclib::NULL_U64 )
			: SampleMsg::Key( uid, did )
			, file_name( svclib::NULL_STR )
		{}

		Key( const Key& that )
			: SampleMsg::Key( that )
			, file_name( that.file_name )
		{}

		Key& operator=( const Key& that )
		{
			SampleMsg::Key::operator=( that );
			if ( this != &that )
			{
				file_name = that.file_name;
			}
			return *this;
		}

		dali::MsgParser& parse( dali::MsgParser& parser ) override
		{
			return SampleMsg::Key::parse( parser )
				.parseString( file_name, 2 * FILE_NAME_SIZE, "2 * FILE_NAME_SIZE" )
				;
		}

		dali::MsgComposer& compose( dali::MsgComposer& composer ) const override
		{
			return SampleMsg::Key::compose( composer )
				.composeString( file_name )
				;
		}

		virtual void format() override
		{
			SampleMsg::Key::format();
			dali::cut( file_name, FILE_NAME_SIZE );
		}

		string toString() const override
		{
			return SampleMsg::Key::toString()
				.append( " name=" ).append( file_name )
				;
		}

	};

	string file_name;				// fn
	uint64_t file_size;				// fs
	uint16_t file_type;				// ft
	vector<uint8_t> file_content;		// fc

	FileMsg()
		: file_size( 0 )
		, file_type( FileEnum::FILE_BINARY )
	{}

	FileMsg( const Key & key )
		: SampleMsg( key )
		, file_name( key.file_name )
		, file_size( 0 )
		, file_type( FileEnum::FILE_BINARY )
	{}

	void format() override
	{
		SampleMsg::format();
		dali::cut( file_name, FILE_NAME_SIZE );
		dali::cut( file_content, FILE_CONTENT_SIZE );
	}

	dali::MsgParser& parse( dali::MsgParser& parser ) override
	{
		SampleMsg::parse( parser )
			.parseString( file_name, 2 * FILE_NAME_SIZE, "2 * FILE_NAME_SIZE" )
			.parseUint64( file_size )
			.parseUint16( file_type )
			.parseBinary( file_content, 2 * FILE_CONTENT_SIZE, "2 * FILE_CONTENT_SIZE" )
			;
		format();
		return parser;
	}
		
	dali::MsgComposer& compose( dali::MsgComposer& composer ) const override
	{
		return SampleMsg::compose( composer )
			.composeString( file_name )
			.composeUint64( file_size )
			.composeUint16( file_type )
			.composeBinary( file_content )
			;
	}

	string toString() const override
	{
		return SampleMsg::toString()
			.append( " name=" ).append( file_name )
			.append( " size=" ).append( dali::intString( file_size ) )
			.append( " type=" ).append( dali::intString( file_type ) )
			;
	}

	typedef TimeMsg::Query< Key > Query;

};

#endif