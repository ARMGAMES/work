#ifndef __DALI_DISPLAY_MSG_H__
#define __DALI_DISPLAY_MSG_H__

#include "SampleMsg.h"

#define DISPLAY_MSG_HEADER SAMPLE_MSG_HEADER "|Display Num|Display X|Display Y|Display Width|Display Height|Colour Depth"

struct DisplayMsg // dali device display data
	: public SampleMsg
{

	struct Key
		: public SampleMsg::Key
	{

		uint32_t display_width;
		uint32_t display_height;

		Key( uint32_t uid = svclib::NULL_U32, uint64_t did = svclib::NULL_U64 )
			: SampleMsg::Key( uid, did )
			, display_width( svclib::NULL_U32 )
			, display_height( svclib::NULL_U32 )
		{}

		Key( const Key& that )
			: SampleMsg::Key( that )
			, display_width( that.display_width )
			, display_height( that.display_height )
		{}

		Key& operator=( const Key& that )
		{
			SampleMsg::Key::operator=( that );
			if ( this != &that )
			{
				display_width = that.display_width;
				display_height = that.display_height;
			}
			return *this;
		}

		dali::MsgParser& parse( dali::MsgParser& parser ) override
		{
			return SampleMsg::Key::parse( parser )
				.parseUint32( display_width )
				.parseUint32( display_height )
				;
		}

		dali::MsgComposer& compose( dali::MsgComposer& composer ) const override
		{
			return SampleMsg::Key::compose( composer )
				.composeUint32( display_width )
				.composeUint32( display_height )
				;
		}

		virtual void format() override
		{
			SampleMsg::Key::format();
		}

		string toString() const override
		{
			return SampleMsg::Key::toString()
				.append( " width=" ).append( dali::intString( display_width ) )
				.append( " height=" ).append( dali::intString( display_height ) )
				;
		}

	};

	uint16_t display_num;				// num
	int32_t display_x;				// dx
	int32_t display_y;				// dy
	uint32_t display_width;			// dw
	uint32_t display_height;			// dh
	uint16_t colour_depth;			// cd

	DisplayMsg()
		: display_num( 0 )
		, display_x( 0 )
		, display_y( 0 )
		, display_width( 0 )
		, display_height( 0 )
		, colour_depth( 0 )
	{}

	DisplayMsg( const Key & key )
		: SampleMsg( key )
		, display_num( 0 )
		, display_x( 0 )
		, display_y( 0 )
		, display_width( key.display_width )
		, display_height( key.display_height )
		, colour_depth( 0 )
	{}

	void format() override
	{
		SampleMsg::format();
	}

	dali::MsgParser& parse( dali::MsgParser& parser ) override
	{
		return SampleMsg::parse( parser )
			.parseUint16( display_num )
			.parseInt32( display_x )
			.parseInt32( display_y )
			.parseUint32( display_width )
			.parseUint32( display_height )
			.parseUint16( colour_depth )
			;
	}

	dali::MsgComposer& compose( dali::MsgComposer& composer ) const override
	{
		return SampleMsg::compose( composer )
			.composeUint16( display_num )
			.composeInt32( display_x )
			.composeInt32( display_y )
			.composeUint32( display_width )
			.composeUint32( display_height )
			.composeUint16( colour_depth )
			;
	}

	string toString() const override
	{
		return SampleMsg::toString()
			.append( " num=" ).append( dali::intString( display_num ) )
			.append( " x=" ).append( dali::intString( display_x ) )
			.append( " y=" ).append( dali::intString( display_y ) )
			.append( " width=" ).append( dali::intString( display_width ) )
			.append( " height=" ).append( dali::intString( display_height ) )
			.append( " colour_depth=" ).append( dali::intString( colour_depth ) )
			;
	}

	string toLine() const override
	{
		return SampleMsg::toLine()
			.append( "|" ).append( dali::intString( display_num ) )
			.append( "|" ).append( dali::intString( display_x ) )
			.append( "|" ).append( dali::intString( display_y ) )
			.append( "|" ).append( dali::intString( display_width ) )
			.append( "|" ).append( dali::intString( display_height ) )
			.append( "|" ).append( dali::intString( colour_depth ) )
			;
	}

	static std::string header()
	{
		return DISPLAY_MSG_HEADER;
	}

	typedef TimeMsg::Query< Key > Query;
	typedef BaseMsg::Key DeltaKey;
	typedef TimeMsg::DeltaQuery< DeltaKey > DeltaQuery;

};

#endif
