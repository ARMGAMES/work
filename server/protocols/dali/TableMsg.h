#ifndef __DALI_TABLE_MSG_H__
#define __DALI_TABLE_MSG_H__

#include "SampleMsg.h"

struct TableMsg // dali table data
	: public SampleMsg
{

	uint16_t game_schedule;			// sch
	uint16_t game_variant;			// var
	uint16_t game_speed;				// spd
	uint16_t game_size;				// gsz
	uint64_t min_buy_in;				// minbi
	uint64_t max_buy_in;				// tmaxbi
	uint64_t lo_bet;					// lb
	uint64_t hi_bet;					// hb
	uint64_t tourn_id;
	uint64_t table_id;
	string table_name;
	string table_theme;
	int32_t window_x;					// wx
	int32_t window_y;					// wy
	uint32_t window_width;			// ww
	uint32_t window_height;			// wh
	uint16_t deck_setting;			// ds
	uint16_t chat_setting;			// cs
	int16_t seat_num;					// sn - zero based, -1 if sitting out

	TableMsg()
		: game_schedule( 0 )
		, game_variant( 0 )
		, game_speed( 0 )
		, game_size( 0 )
		, min_buy_in( 0 )
		, max_buy_in( 0 )
		, lo_bet( 0 )
		, hi_bet( 0 )
		, tourn_id( 0 )
		, table_id( 0 )
		, window_x( 0 )
		, window_y( 0 )
		, window_width( 0 )
		, window_height( 0 )
		, deck_setting( 0 )
		, chat_setting( 0 )
		, seat_num( -1 )
	{}

	void format() override
	{
		dali::cut( table_name, TABLE_NAME_SIZE );
		dali::cut( table_theme, TABLE_THEME_SIZE );
	}

	dali::MsgParser& parse( dali::MsgParser& parser ) override
	{
		SampleMsg::parse( parser )
			.parseUint16( game_schedule )
			.parseUint16( game_variant )
			.parseUint16( game_speed )
			.parseUint16( game_size )
			.parseUint64( min_buy_in )
			.parseUint64( max_buy_in )
			.parseUint64( lo_bet )
			.parseUint64( hi_bet )
			.parseUint64( tourn_id )
			.parseUint64( table_id )
			.parseString( table_name, 2 * TABLE_NAME_SIZE, "2 * TABLE_NAME_SIZE" )
			.parseString( table_theme, 2 * TABLE_THEME_SIZE, "2 * TABLE_THEME_SIZE" )
			.parseInt32( window_x )
			.parseInt32( window_y )
			.parseUint32( window_width )
			.parseUint32( window_height )
			.parseUint16( deck_setting )
			.parseUint16( chat_setting )
			.parseInt16( seat_num )
			;
		format();
		return parser;
	}

	dali::MsgComposer& compose( dali::MsgComposer& composer ) const override
	{
		return SampleMsg::compose( composer )
			.composeUint16( game_schedule )
			.composeUint16( game_variant )
			.composeUint16( game_speed )
			.composeUint16( game_size )
			.composeUint64( min_buy_in )
			.composeUint64( max_buy_in )
			.composeUint64( lo_bet )
			.composeUint64( hi_bet )
			.composeUint64( tourn_id )
			.composeUint64( table_id )
			.composeString( table_name )
			.composeString( table_theme )
			.composeInt32( window_x )
			.composeInt32( window_y )
			.composeUint32( window_width )
			.composeUint32( window_height )
			.composeUint16( deck_setting )
			.composeUint16( chat_setting )
			.composeInt16( seat_num )
			;
	}

	string toString() const override
	{
		return SampleMsg::toString()
			.append( " game_schedule=" ).append( dali::intString( game_schedule ) )
			.append( " game_variant=" ).append( dali::intString( game_variant ) )
			.append( " game_speed=" ).append( dali::intString( game_speed ) )
			.append( " game_size=" ).append( dali::intString( game_size ) )
			.append( " min_buy_in=" ).append( dali::intString( min_buy_in ) )
			.append( " max_buy_in=" ).append( dali::intString( max_buy_in ) )
			.append( " lo_bet=" ).append( dali::intString( lo_bet ) )
			.append( " hi_bet=" ).append( dali::intString( hi_bet ) )
			.append( " tourn_id=" ).append( dali::intString( tourn_id ) )
			.append( " table_id=" ).append( dali::intString( table_id ) )
			.append( " table_name=" ).append( table_name )
			.append( " table_theme=" ).append( table_theme )
			.append( " window_x=" ).append( dali::intString( window_x ) )
			.append( " window_y=" ).append( dali::intString( window_y ) )
			.append( " window_width=" ).append( dali::intString( window_width ) )
			.append( " window_height=" ).append( dali::intString( window_height ) )
			.append( " deck_setting=" ).append( dali::intString( deck_setting ) )
			.append( " chat_setting=" ).append( dali::intString( chat_setting ) )
			.append( " seat_num=" ).append( dali::intString( seat_num ) )
			;
	}

};

#endif
