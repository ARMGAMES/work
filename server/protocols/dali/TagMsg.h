#ifndef __DALI_TAG_MSG_H__
#define __DALI_TAG_MSG_H__

#include "SampleMsg.h"

#define TAG_MSG_HEADER SAMPLE_MSG_HEADER "|Tag Key|Tag Make|Tag Model|Tag Serial"

struct TagMsg // dali device tag data
	: public SampleMsg
{

	struct Key
		: public SampleMsg::Key
	{

		string tag_key;
		string tag_make;
		string tag_model;
		string tag_serial;

		Key( uint32_t uid = svclib::NULL_U32, uint64_t did = svclib::NULL_U64 )
			: SampleMsg::Key( uid, did )
			, tag_key( svclib::NULL_STR )
			, tag_make( svclib::NULL_STR )
			, tag_model( svclib::NULL_STR )
			, tag_serial( svclib::NULL_STR )
		{}

		Key( const Key& that )
			: SampleMsg::Key( that )
			, tag_key( that.tag_key )
			, tag_make( that.tag_make )
			, tag_model( that.tag_model )
			, tag_serial( that.tag_serial )
		{}

		Key& operator=( const Key& that )
		{
			SampleMsg::Key::operator=( that );
			if ( this != &that )
			{
				tag_key = that.tag_key;
				tag_make = that.tag_make;
				tag_model = that.tag_model;
				tag_serial = that.tag_serial;
			}
			return *this;
		}

		void format() override
		{
			SampleMsg::Key::format();
			dali::cut( tag_key, TAG_KEY_SIZE );
			dali::cut( tag_make, TAG_MAKE_SIZE );
			dali::cut( tag_model, TAG_MODEL_SIZE );
			dali::cut( tag_serial, TAG_SERIAL_SIZE );
		}

		dali::MsgParser& parse( dali::MsgParser& parser ) override
		{
			SampleMsg::Key::parse( parser )
				.parseString( tag_key, 2 * TAG_KEY_SIZE, "2 * TAG_KEY_SIZE" )
				.parseString( tag_make, 2 * TAG_MAKE_SIZE, "2 * TAG_MAKE_SIZE" )
				.parseString( tag_model, 2 * TAG_MODEL_SIZE, "2 * TAG_MODEL_SIZE" )
				.parseString( tag_serial, 2 * TAG_SERIAL_SIZE, "2 * TAG_SERIAL_SIZE" )
				;
			return parser;

		}

		dali::MsgComposer& compose( dali::MsgComposer& composer ) const override
		{
			return SampleMsg::Key::compose( composer )
				.composeString( tag_key )
				.composeString( tag_make )
				.composeString( tag_model )
				.composeString( tag_serial )
				;
		}

		string toString() const override
		{
			return SampleMsg::Key::toString()
				.append( "|" ).append( dali::quoteString( tag_key ) )
				.append( "|" ).append( dali::quoteString( tag_make ) )
				.append( "|" ).append( dali::quoteString( tag_model ) )
				.append( "|" ).append( dali::quoteString( tag_serial ) )
				;
		}

	};

	string tag_key;				// key - values: OS, VM, BIOS, MB, CPUx, RAMx, NVMx, GPUx, MONx, PRNx, KBDx, MOUSEx, APP
	string tag_make;				// make
	string tag_model;				// model
	string tag_serial;				// serial

	TagMsg()
	{}

	TagMsg( const Key & key )
		: SampleMsg( key )
		, tag_key( key.tag_key )
		, tag_make( key.tag_make )
		, tag_model( key.tag_model )
		, tag_serial( key.tag_serial )
	{}

	void format() override
	{
		SampleMsg::format();
		dali::cut( tag_key, TAG_KEY_SIZE );
		dali::cut( tag_make, TAG_MAKE_SIZE );
		dali::cut( tag_model, TAG_MODEL_SIZE );
		dali::cut( tag_serial, TAG_SERIAL_SIZE );
	}

	dali::MsgParser& parse( dali::MsgParser& parser ) override
	{
		SampleMsg::parse( parser )
			.parseString( tag_key, 2 * TAG_KEY_SIZE, "2 * TAG_KEY_SIZE" )
			.parseString( tag_make, 2 * TAG_MAKE_SIZE, "2 * TAG_MAKE_SIZE" )
			.parseString( tag_model, 2 * TAG_MODEL_SIZE, "2 * TAG_MODEL_SIZE" )
			.parseString( tag_serial, 2 * TAG_SERIAL_SIZE, "2 * TAG_SERIAL_SIZE" )
			;
		format();
		return parser;
	}

	dali::MsgComposer& compose( dali::MsgComposer& composer ) const override
	{
		return SampleMsg::compose( composer )
			.composeString( tag_key )
			.composeString( tag_make )
			.composeString( tag_model )
			.composeString( tag_serial )
			;
	}

	string toString() const override
	{
		return SampleMsg::toString()
			.append( " key=" ).append( tag_key )
			.append( " make=" ).append( tag_make )
			.append( " model=" ).append( tag_model )
			.append( " serial=" ).append( tag_serial )
			;
	}

	string toLine() const override
	{
		return SampleMsg::toLine()
			.append( "|" ).append( dali::quoteString( tag_key ) )
			.append( "|" ).append( dali::quoteString( tag_make ) )
			.append( "|" ).append( dali::quoteString( tag_model ) )
			.append( "|" ).append( dali::quoteString( tag_serial ) )
			;
	}

	static std::string header()
	{
		return TAG_MSG_HEADER;
	}

	typedef TimeMsg::Query< Key > Query;
	
	struct DeltaKey
		: public BaseMsg::Key
	{

		string tag_key;

		DeltaKey( uint32_t uid = svclib::NULL_U32 )
			: BaseMsg::Key( uid )
			, tag_key( svclib::NULL_STR )
		{}

		DeltaKey( const DeltaKey& that )
			: BaseMsg::Key( that )
			, tag_key( that.tag_key )
		{}

		DeltaKey& operator=( const DeltaKey& that )
		{
			BaseMsg::Key::operator=( that );
			if ( this != &that )
			{
				tag_key = that.tag_key;
			}
			return *this;
		}

		void format() override
		{
			BaseMsg::Key::format();
			dali::cut( tag_key, TAG_KEY_SIZE );
		}

		dali::MsgParser& parse( dali::MsgParser& parser ) override
		{
			BaseMsg::Key::parse( parser )
				.parseString( tag_key, 2 * TAG_KEY_SIZE, "2 * TAG_KEY_SIZE" )
				;
			return parser;

		}

		dali::MsgComposer& compose( dali::MsgComposer& composer ) const override
		{
			return BaseMsg::Key::compose( composer )
				.composeString( tag_key )
				;
		}

		string toString() const override
		{
			return BaseMsg::Key::toString()
				.append( "|" ).append( dali::quoteString( tag_key ) )
				;
		}

	};

	typedef TimeMsg::DeltaQuery< DeltaKey > DeltaQuery;

};

#endif
