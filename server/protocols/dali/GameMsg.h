#ifndef __DALI_GAME_MSG_H__
#define __DALI_GAME_MSG_H__

#include "SampleMsg.h"

#define  GAME_MSG_HEADER SAMPLE_MSG_HEADER "|Window Id|Game Schedule|Game Variant|Game Speed|Game Size"

struct GameMsg // dali game header
	: public SampleMsg
{

	struct Key
		: public SampleMsg::Key
	{

		uint16_t game_schedule;
		uint16_t game_variant;
		uint16_t game_speed;
		uint16_t game_size;

		Key( uint32_t uid = svclib::NULL_U32, uint64_t did = svclib::NULL_U64 )
			: SampleMsg::Key( uid, did )
			, game_schedule( svclib::NULL_U16 )
			, game_variant( svclib::NULL_U16 )
			, game_speed( svclib::NULL_U16 )
			, game_size( svclib::NULL_U16 )
		{}

		Key( const Key& that )
			: SampleMsg::Key( that )
			, game_schedule( that.game_schedule )
			, game_variant( that.game_variant )
			, game_speed( that.game_speed )
			, game_size( that.game_size )
		{}

		Key& operator=( const Key& that )
		{
			SampleMsg::Key::operator=( that );
			if ( this != &that )
			{
				game_schedule = that.game_schedule;
				game_variant = that.game_variant;
				game_speed = that.game_speed;
				game_size = that.game_size;
			}
			return *this;
		}

		dali::MsgParser& parse( dali::MsgParser& parser ) override
		{
			SampleMsg::Key::parse( parser )
				.parseUint16( game_schedule )
				.parseUint16( game_variant )
				.parseUint16( game_speed )
				.parseUint16( game_size )
				;
			return parser;

		}

		dali::MsgComposer& compose( dali::MsgComposer& composer ) const override
		{
			return SampleMsg::Key::compose( composer )
				.composeUint16( game_schedule )
				.composeUint16( game_variant )
				.composeUint16( game_speed )
				.composeUint16( game_size )
				;
		}

		string toString() const override
		{
			return SampleMsg::Key::toString()
				.append( " sch=" ).append( dali::intString( game_schedule ) )
				.append( " var=" ).append( dali::intString( game_variant ) )
				.append( " spd=" ).append( dali::intString( game_speed ) )
				.append( " gsz=" ).append( dali::intString( game_size ) )
				;
		}

	};

	uint32_t window_id;
	uint16_t game_schedule;				// sch
	uint16_t game_variant;				// var
	uint16_t game_speed;				// spd
	uint16_t game_size;					// gsz

	GameMsg()
		: window_id( 0 )
		, game_schedule( 0 )
		, game_variant( 0 )
		, game_speed( 0 )
		, game_size( 0 )
	{}

	GameMsg( const Key & key )
		: SampleMsg( key )
		, window_id( 0 )
		, game_schedule( key.game_schedule )
		, game_variant( key.game_variant )
		, game_speed( key.game_speed )
		, game_size( key.game_size )
	{}

	void format() override
	{
		SampleMsg::format();
	}

	dali::MsgParser& parse( dali::MsgParser& parser ) override
	{
		SampleMsg::parse( parser )
			.parseUint32( window_id )
			.parseUint16( game_schedule )
			.parseUint16( game_variant )
			.parseUint16( game_speed )
			.parseUint16( game_size )
			;
		if ( game_schedule == 0XFFFF ) game_schedule = ScheduleEnum::SCHEDULE_OTHER;
		if ( game_variant == 0XFFFF ) game_variant = VariantEnum::VARIANT_OTHER;
		if ( game_speed == 0XFFFF ) game_speed = SpeedEnum::SPEED_OTHER;
		format();
		return parser;
	}

	dali::MsgComposer& compose( dali::MsgComposer& composer ) const override
	{
		return SampleMsg::compose( composer )
			.composeUint32( window_id )
			.composeUint16( game_schedule )
			.composeUint16( game_variant )
			.composeUint16( game_speed )
			.composeUint16( game_size )
			;
	}

	string toString() const override
	{
		return SampleMsg::toString()
			.append( " window=" ).append( dali::intString( window_id ) )
			.append( " sch=" ).append( dali::intString( game_schedule ) )
			.append( " var=" ).append( dali::intString( game_variant ) )
			.append( " spd=" ).append( dali::intString( game_speed ) )
			.append( " gsz=" ).append( dali::intString( game_size ) )
			;
	}

	string toLine() const override
	{
		return SampleMsg::toLine()
			.append( "|" ).append( dali::intString( window_id ) )
			.append( "|" ).append( dali::intString( game_schedule ) )
			.append( "|" ).append( dali::intString( game_variant ) )
			.append( "|" ).append( dali::intString( game_speed ) )
			.append( "|" ).append( dali::intString( game_size ) )
			;
	}

	static std::string header()
	{
		return GAME_MSG_HEADER;
	}

	inline static uint64_t gameType( uint16_t game_sch, uint16_t game_var, uint16_t game_spd, uint16_t game_sz )
	{
		return ( ( ( ( ( static_cast< uint64_t >( game_sch ) << 16 ) + game_var ) << 16 ) + game_spd ) << 16 ) + game_sz;
	}

	inline void gameType( uint64_t gid )
	{
		game_size = static_cast< uint16_t >( gid & 0xFFFF );
		gid = gid >> 16;
		game_speed = static_cast< uint16_t >( gid & 0xFFFF );
		gid = gid >> 16;
		game_variant = static_cast< uint16_t >( gid & 0xFFFF );
		gid = gid >> 16;
		game_schedule = static_cast< uint16_t >( gid & 0xFFFF );
	}

};


#endif
