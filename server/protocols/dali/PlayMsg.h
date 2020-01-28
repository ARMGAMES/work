#ifndef __DALI_PLAY_MSG_H__
#define __DALI_PLAY_MSG_H__

#include "GameMsg.h"

#define PLAY_MSG_HEADER GAME_MSG_HEADER "|Min Buy In|Max Buy In|Lo Bet|Hi Bet|Tourn Bet|Table Id|Table Name|Table Theme|Table Delay" \
"|Window X|Window Y|Window Width|Window Height|Window Drag Count|Window Resize Count|Chat Setting|Chat Line Count|Note Line Count" \
"|Hand Replay Count|Hand Reskin Count|Slider Change Count|Tab Change Count|Play Duration|Total Duration" \
"|Response Timeout Count|Inj Press Count|Press Count|Press Duration|Press Interval|Inj Click Count|Click Count|Alt Click Count|Touch Count" \
"|Hotkey Focus Count|Hotkey Action Count|Hotkey Amount Count|Hand Count|Turn Count"

struct PlayMsg // dali play behaviour sample
	: public GameMsg
{

	typedef GameMsg::Key Key;

	uint64_t min_buy_in;					// minbi
	uint64_t max_buy_in;					// maxbi
	uint64_t lo_bet;						// lb
	uint64_t hi_bet;						// hb
	uint64_t tourn_id;
	uint64_t table_id;
	string table_name;
	string table_theme;
	uint32_t table_delay;					// tdel
	int32_t window_x;						// wx
	int32_t window_y;						// wy
	uint32_t window_width;				// ww
	uint32_t window_height;				// wh
	uint32_t window_drag_count;			// wdc
	uint32_t window_resize_count;			// wrc
	uint16_t chat_setting;				// cs
	uint32_t chat_line_count;				// clc
	uint32_t note_line_count;				// nlc
	uint32_t hand_replay_count;			// hrpc
	uint32_t hand_reskin_count;			// hrsc
	uint32_t slider_change_count;			// scc
	uint32_t tab_change_count;			// tcc
	uint32_t play_duration;				// playdur
	uint32_t total_duration;				// totaldur
	uint32_t response_timeout_count;		// rtc
	uint32_t inj_press_count;				// ikc
	uint32_t press_count;					// pc
	uint32_t press_duration;				// pdur
	uint32_t press_interval;				// pint
	uint32_t inj_click_count;				// icc
	uint32_t click_count;					// cc
	uint32_t alt_click_count;				// acc
	uint32_t touch_count;					// tc
	uint32_t hotkey_focus_count;			// hfocc
	uint32_t hotkey_action_count;			// hactc
	uint32_t hotkey_amount_count;			// hamtc
	uint32_t hand_count;					// handc
	uint32_t turn_count;					// turnc

	PlayMsg()
		: min_buy_in( 0 )
		, max_buy_in( 0 )
		, lo_bet( 0 )
		, hi_bet( 0 )
		, tourn_id( 0 )
		, table_id( 0 )
		, table_delay( 0 )
		, window_x( 0 )
		, window_y( 0 )
		, window_width( 0 )
		, window_height( 0 )
		, window_drag_count( 0 )
		, window_resize_count( 0 )
		, chat_setting( 0 )
		, chat_line_count( 0 )
		, note_line_count( 0 )
		, hand_replay_count( 0 )
		, hand_reskin_count( 0 )
		, slider_change_count( 0 )
		, tab_change_count( 0 )
		, play_duration( 0 )
		, total_duration( 0 )
		, response_timeout_count( 0 )
		, inj_press_count( 0 )
		, press_count( 0 )
		, press_duration( 0 )
		, press_interval( 0 )
		, inj_click_count( 0 )
		, click_count( 0 )
		, alt_click_count( 0 )
		, touch_count( 0 )
		, hotkey_focus_count( 0 )
		, hotkey_action_count( 0 )
		, hotkey_amount_count( 0 )
		, hand_count( 0 )
		, turn_count( 0 )
	{}

	PlayMsg( const Key & key )
		: GameMsg( key )
		, min_buy_in( 0 )
		, max_buy_in( 0 )
		, lo_bet( 0 )
		, hi_bet( 0 )
		, tourn_id( 0 )
		, table_id( 0 )
		, table_delay( 0 )
		, window_x( 0 )
		, window_y( 0 )
		, window_width( 0 )
		, window_height( 0 )
		, window_drag_count( 0 )
		, window_resize_count( 0 )
		, chat_setting( 0 )
		, chat_line_count( 0 )
		, note_line_count( 0 )
		, hand_replay_count( 0 )
		, hand_reskin_count( 0 )
		, slider_change_count( 0 )
		, tab_change_count( 0 )
		, play_duration( 0 )
		, total_duration( 0 )
		, response_timeout_count( 0 )
		, inj_press_count( 0 )
		, press_count( 0 )
		, press_duration( 0 )
		, press_interval( 0 )
		, inj_click_count( 0 )
		, click_count( 0 )
		, alt_click_count( 0 )
		, touch_count( 0 )
		, hotkey_focus_count( 0 )
		, hotkey_action_count( 0 )
		, hotkey_amount_count( 0 )
		, hand_count( 0 )
		, turn_count( 0 )
	{}


	void format() override
	{
		GameMsg::format();
		dali::cut( table_name, TABLE_NAME_SIZE );
		dali::cut( table_theme, TABLE_THEME_SIZE );
	}

	dali::MsgParser& parse( dali::MsgParser& parser ) override
	{
		GameMsg::parse( parser )
			.parseUint64( min_buy_in )
			.parseUint64( max_buy_in )
			.parseUint64( lo_bet )
			.parseUint64( hi_bet )
			.parseUint64( tourn_id )
			.parseUint64( table_id )
			.parseString( table_name, 2 * TABLE_NAME_SIZE, "2 * TABLE_NAME_SIZE" )
			.parseString( table_theme, 2 * TABLE_THEME_SIZE, "2 * TABLE_THEME_SIZE" )
			.parseUint32( table_delay )
			.parseInt32( window_x )
			.parseInt32( window_y )
			.parseUint32( window_width )
			.parseUint32( window_height )
			.parseUint32( window_drag_count )
			.parseUint32( window_resize_count )
			.parseUint16( chat_setting )
			.parseUint32( chat_line_count )
			.parseUint32( note_line_count )
			.parseUint32( hand_replay_count )
			.parseUint32( hand_reskin_count )
			.parseUint32( slider_change_count )
			.parseUint32( tab_change_count )
			.parseUint32( play_duration )
			.parseUint32( total_duration )
			.parseUint32( response_timeout_count )
			.parseUint32( inj_press_count )
			.parseUint32( press_count )
			.parseUint32( press_duration )
			.parseUint32( press_interval )
			.parseUint32( inj_click_count )
			.parseUint32( click_count )
			.parseUint32( alt_click_count )
			.parseUint32( touch_count )
			.parseUint32( hotkey_focus_count )
			.parseUint32( hotkey_action_count )
			.parseUint32( hotkey_amount_count )
			.parseUint32( hand_count )
			.parseUint32( turn_count )
			;
		format();
		return parser;
	}

	dali::MsgComposer& compose( dali::MsgComposer& composer ) const override
	{
		return GameMsg::compose( composer )
			.composeUint64( min_buy_in )
			.composeUint64( max_buy_in )
			.composeUint64( lo_bet )
			.composeUint64( hi_bet )
			.composeUint64( tourn_id )
			.composeUint64( table_id )
			.composeString( table_name )
			.composeString( table_theme )
			.composeUint32( table_delay )
			.composeInt32( window_x )
			.composeInt32( window_y )
			.composeUint32( window_width )
			.composeUint32( window_height )
			.composeUint32( window_drag_count )
			.composeUint32( window_resize_count )
			.composeUint16( chat_setting )
			.composeUint32( chat_line_count )
			.composeUint32( note_line_count )
			.composeUint32( hand_replay_count )
			.composeUint32( hand_reskin_count )
			.composeUint32( slider_change_count )
			.composeUint32( tab_change_count )
			.composeUint32( play_duration )
			.composeUint32( total_duration )
			.composeUint32( response_timeout_count )
			.composeUint32( inj_press_count )
			.composeUint32( press_count )
			.composeUint32( press_duration )
			.composeUint32( press_interval )
			.composeUint32( inj_click_count )
			.composeUint32( click_count )
			.composeUint32( alt_click_count )
			.composeUint32( touch_count )
			.composeUint32( hotkey_focus_count )
			.composeUint32( hotkey_action_count )
			.composeUint32( hotkey_amount_count )
			.composeUint32( hand_count )
			.composeUint32( turn_count )
			;
	}

	string toString() const override
	{
		return GameMsg::toString()
			.append( " min_buy_in=" ).append( dali::intString( min_buy_in ) )
			.append( " max_buy_in=" ).append( dali::intString( max_buy_in ) )
			.append( " lo_bet=" ).append( dali::intString( lo_bet ) )
			.append( " hi_bet=" ).append( dali::intString( hi_bet ) )
			.append( " tourn_id=" ).append( dali::intString( tourn_id ) )
			.append( " table_id=" ).append( dali::intString( table_id ) )
			.append( " table_name=" ).append( table_name )
			.append( " table_theme=" ).append( table_theme )
			.append( " table_delay=" ).append( dali::intString( table_delay ) )
			.append( " window_x=" ).append( dali::intString( window_x ) )
			.append( " window_y=" ).append( dali::intString( window_y ) )
			.append( " window_width=" ).append( dali::intString( window_width ) )
			.append( " window_height=" ).append( dali::intString( window_height ) )
			.append( " window_drag_count=" ).append( dali::intString( window_drag_count ) )
			.append( " window_resize_count=" ).append( dali::intString( window_resize_count ) )
			.append( " chat_setting=" ).append( dali::intString( chat_setting ) )
			.append( " chat_line_count=" ).append( dali::intString( chat_line_count ) )
			.append( " note_line_count=" ).append( dali::intString( note_line_count ) )
			.append( " hand_replay_count=" ).append( dali::intString( hand_replay_count ) )
			.append( " hand_reskin_count=" ).append( dali::intString( hand_reskin_count ) )
			.append( " slider_change_count=" ).append( dali::intString( slider_change_count ) )
			.append( " tab_change_count=" ).append( dali::intString( tab_change_count ) )
			.append( " play_duration=" ).append( dali::intString( play_duration ) )
			.append( " total_duration=" ).append( dali::intString( total_duration ) )
			.append( " response_timeout_count=" ).append( dali::intString( response_timeout_count ) )
			.append( " inj_press_count=" ).append( dali::intString( inj_press_count ) )
			.append( " press_count=" ).append( dali::intString( press_count ) )
			.append( " press_duration=" ).append( dali::intString( press_duration ) )
			.append( " press_interval=" ).append( dali::intString( press_interval ) )
			.append( " inj_click_count=" ).append( dali::intString( inj_click_count ) )
			.append( " click_count=" ).append( dali::intString( click_count ) )
			.append( " alt_click_count=" ).append( dali::intString( alt_click_count ) )
			.append( " touch_count=" ).append( dali::intString( touch_count ) )
			.append( " hotkey_focus_count=" ).append( dali::intString( hotkey_focus_count ) )
			.append( " hotkey_action_count=" ).append( dali::intString( hotkey_action_count ) )
			.append( " hotkey_amount_count=" ).append( dali::intString( hotkey_amount_count ) )
			.append( " hand_count=" ).append( dali::intString( hand_count ) )
			.append( " turn_count=" ).append( dali::intString( turn_count ) )
			;
	}

	string toLine() const override
	{
		return GameMsg::toLine()
			.append( "|" ).append( dali::intString( min_buy_in ) )
			.append( "|" ).append( dali::intString( max_buy_in ) )
			.append( "|" ).append( dali::intString( lo_bet ) )
			.append( "|" ).append( dali::intString( hi_bet ) )
			.append( "|" ).append( dali::intString( tourn_id ) )
			.append( "|" ).append( dali::intString( table_id ) )
			.append( "|" ).append( dali::quoteString( table_name ) )
			.append( "|" ).append( dali::quoteString( table_theme ) )
			.append( "|" ).append( dali::intString( table_delay ) )
			.append( "|" ).append( dali::intString( window_x ) )
			.append( "|" ).append( dali::intString( window_y ) )
			.append( "|" ).append( dali::intString( window_width ) )
			.append( "|" ).append( dali::intString( window_height ) )
			.append( "|" ).append( dali::intString( window_drag_count ) )
			.append( "|" ).append( dali::intString( window_resize_count ) )
			.append( "|" ).append( dali::intString( chat_setting ) )
			.append( "|" ).append( dali::intString( chat_line_count ) )
			.append( "|" ).append( dali::intString( note_line_count ) )
			.append( "|" ).append( dali::intString( hand_replay_count ) )
			.append( "|" ).append( dali::intString( hand_reskin_count ) )
			.append( "|" ).append( dali::intString( slider_change_count ) )
			.append( "|" ).append( dali::intString( tab_change_count ) )
			.append( "|" ).append( dali::intString( play_duration ) )
			.append( "|" ).append( dali::intString( total_duration ) )
			.append( "|" ).append( dali::intString( response_timeout_count ) )
			.append( "|" ).append( dali::intString( inj_press_count ) )
			.append( "|" ).append( dali::intString( press_count ) )
			.append( "|" ).append( dali::intString( press_duration ) )
			.append( "|" ).append( dali::intString( press_interval ) )
			.append( "|" ).append( dali::intString( inj_click_count ) )
			.append( "|" ).append( dali::intString( click_count ) )
			.append( "|" ).append( dali::intString( alt_click_count ) )
			.append( "|" ).append( dali::intString( touch_count ) )
			.append( "|" ).append( dali::intString( hotkey_focus_count ) )
			.append( "|" ).append( dali::intString( hotkey_action_count ) )
			.append( "|" ).append( dali::intString( hotkey_amount_count ) )
			.append( "|" ).append( dali::intString( hand_count ) )
			.append( "|" ).append( dali::intString( turn_count ) )
			;
	}

	static std::string header()
	{
		return PLAY_MSG_HEADER;
	}

	typedef TimeMsg::Query< Key > Query;

};

#endif
