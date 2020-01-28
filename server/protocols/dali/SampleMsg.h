#ifndef __DALI_SAMPLE_MSG_H__
#define __DALI_SAMPLE_MSG_H__

#include "BaseMsg.h"

#define SAMPLE_MSG_HEADER BASE_MSG_HEADER "|Device Id|Session Id"

struct SampleMsg // dali sample data
	: public BaseMsg
{

	struct Key
		: public BaseMsg::Key
	{

		uint64_t device_id;
		uint64_t session_id;

		Key( uint32_t uid = svclib::NULL_U32, uint64_t did = svclib::NULL_U64 )
			: BaseMsg::Key( uid )
			, device_id( did )
			, session_id( svclib::NULL_U64 )
		{}

		Key( const Key& that )
			: BaseMsg::Key( that )
			, device_id( that.device_id )
			, session_id( that.session_id )
		{}

		Key& operator=( const Key& that )
		{
			BaseMsg::Key::operator=( that );
			if ( this != &that )
			{
				device_id = that.device_id;
				session_id = that.session_id;
			}
			return *this;
		}

		dali::MsgParser& parse( dali::MsgParser& parser ) override
		{
			return BaseMsg::Key::parse( parser )
				.parseUint64( device_id )
				.parseUint64( session_id )
				;
		}

		dali::MsgComposer& compose( dali::MsgComposer& composer ) const override
		{
			return BaseMsg::Key::compose( composer )
				.composeUint64( device_id )
				.composeUint64( session_id )
				;
		}

		virtual void format() override
		{
			BaseMsg::Key::format();
		}

		string toString() const override
		{
			return BaseMsg::Key::toString()
				.append( " device=" ).append( dali::intString( device_id ) )
				.append( " session=" ).append( dali::intString( session_id ) )
				;
		}

	};

	uint64_t device_id;				// did
	uint64_t session_id;

	SampleMsg()
		: device_id( 0 )
		, session_id( 0 )
	{}

	SampleMsg( const Key & key )
		: BaseMsg( key )
		, device_id( key.device_id )
		, session_id( key.session_id )
	{}

	virtual dali::MsgParser& parse( dali::MsgParser& parser ) override
	{
		return BaseMsg::parse( parser )
			.parseUint64( device_id )
			.parseUint64( session_id )
			;
	}

	virtual dali::MsgComposer& compose( dali::MsgComposer& composer ) const override
	{
		return BaseMsg::compose( composer )
			.composeUint64( device_id )
			.composeUint64( session_id )
			;
	}

	virtual string toString() const override
	{
		return BaseMsg::toString()
			.append( " device=" ).append( dali::intString( device_id ) )
			.append( " session=" ).append( dali::intString( session_id ) )
			;
	}

	string toLine() const override
	{
		return BaseMsg::toLine()
			.append( "|" ).append( dali::intString( device_id ) )
			.append( "|" ).append( dali::intString( session_id ) )
			;
	}

	static std::string header()
	{
		return SAMPLE_MSG_HEADER;
	}

};

#endif
