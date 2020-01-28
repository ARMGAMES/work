#ifndef __DALI_TURN_MSG_H__
#define __DALI_TURN_MSG_H__

#include "GameMsg.h"

#define TURN_MSG_HEADER GAME_MSG_HEADER "|Turn Type|Turn Time|Hand ID|Action Offset X|Action Offset Y|Response Delay" \
"|Focus Duration|Shift Count|Shift Duration|Shift Distance|Shift Angle|Click Count|Click Duration|Drift Count|Drift Distance"

struct TurnMsg // dali play behaviour sample
	: public GameMsg
{

	struct Key
		: public GameMsg::Key
	{

		uint16_t turn_type;

		Key( uint32_t uid = svclib::NULL_U32, uint64_t did = svclib::NULL_U64 )
			: GameMsg::Key( uid, did )
			, turn_type( svclib::NULL_U16 )
		{}

		Key( const Key& that )
			: GameMsg::Key( that )
			, turn_type( that.turn_type )
		{}

		Key& operator=( const Key& that )
		{
			GameMsg::Key::operator=( that );
			if ( this != &that )
			{
				turn_type = that.turn_type;
			}
			return *this;
		}

		dali::MsgParser& parse( dali::MsgParser& parser ) override
		{
			GameMsg::Key::parse( parser )
				.parseUint16( turn_type )
				;
			return parser;

		}

		dali::MsgComposer& compose( dali::MsgComposer& composer ) const override
		{
			return GameMsg::Key::compose( composer )
				.composeUint16( turn_type )
				;
		}

		string toString() const override
		{
			return GameMsg::Key::toString()
				.append( " turn=" ).append( dali::intString( turn_type ) )
				;
		}

	};

	uint16_t turn_type;					// turn
	int64_t turn_time;					// tt
	uint64_t hand_id;						// hid
	int32_t action_offset_x;				// aox
	int32_t action_offset_y;				// aoy
	uint32_t response_delay;				// rdel
	uint32_t focus_duration;				// fdur
	uint32_t shift_count;					// sc
	uint32_t shift_duration;				// sdur
	uint32_t shift_distance;				// sd
	uint32_t shift_angle;					// sa
	uint32_t click_count;					// cc
	uint32_t click_duration;				// cdur
	uint32_t drift_count;					// dc
	uint32_t drift_distance;				// dd

	TurnMsg()
		: turn_type( 0 )
		, turn_time( 0 )
		, hand_id( 0 )
		, action_offset_x( 0 )
		, action_offset_y( 0 )
		, response_delay( 0 )
		, focus_duration( 0 )
		, shift_count( 0 )
		, shift_duration( 0 )
		, shift_distance( 0 )
		, shift_angle( 0 )
		, click_count( 0 )
		, click_duration( 0 )
		, drift_count( 0 )
		, drift_distance( 0 )
	{}

	TurnMsg( const Key & key )
		: GameMsg( key )
		, turn_type( key.turn_type )
		, turn_time( 0 )
		, hand_id( 0 )
		, action_offset_x( 0 )
		, action_offset_y( 0 )
		, response_delay( 0 )
		, focus_duration( 0 )
		, shift_count( 0 )
		, shift_duration( 0 )
		, shift_distance( 0 )
		, shift_angle( 0 )
		, click_count( 0 )
		, click_duration( 0 )
		, drift_count( 0 )
		, drift_distance( 0 )
	{}

	void format() override
	{
		GameMsg::format();
	}

	dali::MsgParser& parse( dali::MsgParser& parser ) override
	{
		GameMsg::parse( parser )
			.parseUint16( turn_type )
			.parseInt64( turn_time )
			.parseUint64( hand_id )
			.parseInt32( action_offset_x )
			.parseInt32( action_offset_y )
			.parseUint32( response_delay )
			.parseUint32( focus_duration )
			.parseUint32( shift_count )
			.parseUint32( shift_duration )
			.parseUint32( shift_distance )
			.parseUint32( shift_angle )
			.parseUint32( click_count )
			.parseUint32( click_duration )
			.parseUint32( drift_count )
			.parseUint32( drift_distance )
			;
		if ( turn_type == 0XFFFF ) game_schedule = TurnEnum::TURN_OTHER;
		return parser;
	}

	dali::MsgComposer& compose( dali::MsgComposer& composer ) const override
	{
		return GameMsg::compose( composer )
			.composeUint16( turn_type )
			.composeInt64( turn_time )
			.composeUint64( hand_id )
			.composeInt32( action_offset_x )
			.composeInt32( action_offset_y )
			.composeUint32( response_delay )
			.composeUint32( focus_duration )
			.composeUint32( shift_count )
			.composeUint32( shift_duration )
			.composeUint32( shift_distance )
			.composeUint32( shift_angle )
			.composeUint32( click_count )
			.composeUint32( click_duration )
			.composeUint32( drift_count )
			.composeUint32( drift_distance )
			;
	}

	string toString() const override
	{
		return GameMsg::toString()
			.append( " turn=" ).append( dali::intString( turn_type ) )
			.append( " turn_time=" ).append( dali::intString( turn_time ) )
			.append( " hand_id=" ).append( dali::intString( hand_id ) )
			.append( " action_offset_x=" ).append( dali::intString( action_offset_x ) )
			.append( " action_offset_y=" ).append( dali::intString( action_offset_y ) )
			.append( " response_delay=" ).append( dali::intString( response_delay ) )
			.append( " focus_duration=" ).append( dali::intString( focus_duration ) )
			.append( " shift_count=" ).append( dali::intString( shift_count ) )
			.append( " shift_duration=" ).append( dali::intString( shift_duration ) )
			.append( " shift_distance=" ).append( dali::intString( shift_distance ) )
			.append( " shift_angle=" ).append( dali::intString( shift_angle ) )
			.append( " click_count=" ).append( dali::intString( click_count ) )
			.append( " click_duration=" ).append( dali::intString( click_duration ) )
			.append( " drift_count=" ).append( dali::intString( drift_count ) )
			.append( " drift_distance=" ).append( dali::intString( drift_distance ) )
			;
	}

	string toLine() const override
	{
		return GameMsg::toLine()
			.append( "|" ).append( dali::intString( turn_type ) )
			.append( "|" ).append( dali::toString( turn_time, true ) )
			.append( "|" ).append( dali::intString( hand_id ) )
			.append( "|" ).append( dali::intString( action_offset_x ) )
			.append( "|" ).append( dali::intString( action_offset_y ) )
			.append( "|" ).append( dali::intString( response_delay ) )
			.append( "|" ).append( dali::intString( focus_duration ) )
			.append( "|" ).append( dali::intString( shift_count ) )
			.append( "|" ).append( dali::intString( shift_duration ) )
			.append( "|" ).append( dali::intString( shift_distance ) )
			.append( "|" ).append( dali::intString( shift_angle ) )
			.append( "|" ).append( dali::intString( click_count ) )
			.append( "|" ).append( dali::intString( click_duration ) )
			.append( "|" ).append( dali::intString( drift_count ) )
			.append( "|" ).append( dali::intString( drift_distance ) )
			;
	}

	static std::string header()
	{
		return TURN_MSG_HEADER;
	}

	typedef TimeMsg::Query< Key > Query;

};

#endif
