#ifndef __DALI_BASE_MSG_H__
#define __DALI_BASE_MSG_H__

#include "TimeMsg.h"

#define BASE_MSG_HEADER TIME_MSG_HEADER "|Send Time|User Id"

struct BaseMsg
	: public TimeMsg
{

	struct Key
		: public IMsg::Key
	{

		uint32_t user_id;

		Key( uint32_t uid = svclib::NULL_U32 )
			: user_id( uid )
		{}

		Key( const Key& that )
			: user_id( that.user_id )
		{}

		Key & operator=( const Key& that )
		{
			if ( this != &that )
			{
				user_id = that.user_id;
			}
			return *this;
		}

		void format() override
		{}

		dali::MsgParser& parse( dali::MsgParser& parser ) override
		{
			return parser.parseUint32( user_id );
		}

		dali::MsgComposer& compose( dali::MsgComposer& composer ) const override
		{
			return composer.composeUint32( user_id );
		}

		string toString() const override
		{
			return string()
				.append( " user=" ).append( dali::intString( user_id ) )
				;
		}

	};

	int64_t send_time;				// sendt
	uint32_t user_id;				// uid - user int id

	BaseMsg()
		: send_time( 0 )
		, user_id( 0 )
	{}

	BaseMsg( const Key & key )
		: TimeMsg( key )
		, send_time( 0 )
		, user_id( key.user_id )
	{}

	BaseMsg( uint32_t user_id_ )
		: send_time( dali::getTime() )
		, user_id( user_id_ )
	{}

	virtual dali::MsgParser& parse( dali::MsgParser& parser ) override
	{
		return TimeMsg::parse( parser )
			.parseInt64( send_time )
			.parseUint32( user_id )
			;
	}

	virtual dali::MsgComposer& compose( dali::MsgComposer& composer ) const override
	{
		return TimeMsg::compose( composer )
			.composeInt64( send_time )
			.composeUint32( user_id )
			;
	}

	virtual string toString() const override
	{
		return  TimeMsg::toString()
			.append( " sent=" ).append( dali::toString( send_time ) )
			.append( " user=" ).append( dali::intString( user_id ) )
			;
	}

	virtual string toLine() const override
	{
		return TimeMsg::toLine()
			.append( "|" ).append( dali::toString( send_time ) )
			.append( "|" ).append( dali::intString( user_id ) )
			;
	}

	static std::string header()
	{
		return BASE_MSG_HEADER;
	}

	enum CommandIndexEnum: size_t
	{
		COMMAND_INDEX_SENDER = 0
		, COMMAND_INDEX_RECIPIENT
		, COMMAND_INDEX_TITLE
		, COMMAND_INDEX_CONTENT
		, COMMAND_INDEX_MAX
	};

	enum VmFlagEnum: uint32_t
	{
		VM_EMPTY					= 0x0000
		, VM_GENERIC				= 0x0001		// generic vm
		, VM_QEMU					= 0x000010		// 000010 to 00FFFF - NO BRAND VM / OPEN SOURCE VM
		, VM_WINE					= 0x000020
		, VM_VMWARE					= 0x010000		// 010000 ( generic ) to 01FFFF - VMWARE
		, VM_VMWARE_ESX_SERVER
		, VM_VMWARE_GSX_SERVER
		, VM_VMWARE_WORKSTATION
		, VM_VMWARE_EXPRESS
		, VM_MICROSOFT				= 0x020000		// 020000 ( generic ) to 02FFFF - MICROSOFT
		, VM_MICROSOFT_HYPER_V
		, VM_MICROSOFT_VIRTUAL_PC
		, VM_ORACLE					= 0x030000		// 030000 ( generic ) to 03FFFF - ORACLE
		, VM_ORACLE_VIRTUALBOX
		, VM_PARALLELS				= 0x040000		// 040000 ( generic ) to 04FFFF - PARALLELS
		, VM_NESTED					= 0x80000000	// highest bit - nested vm flag
	};

	enum CapFlagEnum: uint32_t
	{
		CAP_EMPTY					= 0
		, CAP_ADMIN					= 1		// legacy user account is admin
		, CAP_VOICE					= 2		// legacy device is voice capable
		, CAP_ADMIN_ACC_DEF			= 4		// admin account defined
		, CAP_ADMIN_ACC_VAL			= 8		// admin account
		, CAP_ADMIN_APP_DEF			= 16	// admin client defined
		, CAP_ADMIN_APP_VAL			= 32	// admin client
		, CAP_VOICE_DEV_DEF			= 64	// voice device capability defined
		, CAP_VOICE_DEV_VAL			= 128	// voice device capability
	};

	enum NetworkFlagEnum: uint32_t
	{
		NETWORK_EMPTY				= 0x0000
		, NETWORK_WIRED				= 0x0001
		, NETWORK_WIFI				= 0x0002
		, NETWORK_CELL				= 0x0004
		, NETWORK_GPS				= 0x0008
	};

	enum FileEnum: uint16_t
	{
		FILE_BINARY					= 0
		, FILE_TEXT					= 1
	};

	enum ScheduleEnum: uint16_t
	{
		SCHEDULE_UNKNOWN			= 0
		, SCHEDULE_PMBL				= 0x1100 // Play Money Bet Limit
		, SCHEDULE_PMBL_RNG
		, SCHEDULE_PMBL_SNG
		, SCHEDULE_PMBL_MTT
		, SCHEDULE_PMBL_STEALTH     = 0x1180
		, SCHEDULE_PMBL_STEALTH_RNG
		, SCHEDULE_PMBL_OTHER		= 0x11FF
		, SCHEDULE_PMPL				= 0x1200 // Play Money Pot Limit
		, SCHEDULE_PMPL_RNG
		, SCHEDULE_PMPL_SNG
		, SCHEDULE_PMPL_MTT
		, SCHEDULE_PMPL_STEALTH     = 0x1280
		, SCHEDULE_PMPL_STEALTH_RNG
		, SCHEDULE_PMPL_OTHER		= 0x12FF
		, SCHEDULE_PMNL				= 0x1400 // Play Money No Limit
		, SCHEDULE_PMNL_RNG
		, SCHEDULE_PMNL_SNG
		, SCHEDULE_PMNL_MTT
		, SCHEDULE_PMNL_STEALTH     = 0x1480
		, SCHEDULE_PMNL_STEALTH_RNG
		, SCHEDULE_PMNL_OTHER		= 0x14FF
		, SCHEDULE_RMBL				= 0x4100 // Real Money Bet Limit
		, SCHEDULE_RMBL_RNG
		, SCHEDULE_RMBL_SNG
		, SCHEDULE_RMBL_MTT
		, SCHEDULE_RMBL_STEALTH		= 0x4180
		, SCHEDULE_RMBL_STEALTH_RNG
		, SCHEDULE_RMBL_OTHER		= 0x41FF
		, SCHEDULE_RMPL				= 0x4200 // Real Money Pot Limit
		, SCHEDULE_RMPL_RNG
		, SCHEDULE_RMPL_SNG
		, SCHEDULE_RMPL_MTT
		, SCHEDULE_RMPL_STEALTH		= 0x4280
		, SCHEDULE_RMPL_STEALTH_RNG
		, SCHEDULE_RMPL_OTHER		= 0x42FF
		, SCHEDULE_RMNL				= 0x4400 // Real Money No Limit
		, SCHEDULE_RMNL_RNG
		, SCHEDULE_RMNL_SNG
		, SCHEDULE_RMNL_MTT
		, SCHEDULE_RMNL_STEALTH		= 0x4480
		, SCHEDULE_RMNL_STEALTH_RNG
		, SCHEDULE_RMNL_OTHER		= 0x44FF
		, SCHEDULE_OTHER			= 0x7FFF
	};

	enum VariantEnum: uint16_t
	{
		VARIANT_UNDEFINED				= 0
		, VARIANT_HOLDEM
		, VARIANT_27SD
		, VARIANT_27TD
		, VARIANT_5CD
		, VARIANT_BADUGI
		, VARIANT_COURCHEVEL
		, VARIANT_COURCHEVEL8
		, VARIANT_OMAHA
		, VARIANT_OMAHA5C
		, VARIANT_OMAHA8
		, VARIANT_OMAHA85C
		, VARIANT_RAZZ
		, VARIANT_STUD
		, VARIANT_STUD8
		, VARIANT_HOLDEM6
		, VARIANT_SPINGO			= 0x0100
		, VARIANT_SPINGO_HOLDEM
		, VARIANT_SPINGO_27SD
		, VARIANT_SPINGO_27TD
		, VARIANT_SPINGO_5CD
		, VARIANT_SPINGO_BADUGI
		, VARIANT_SPINGO_COURCHEVEL
		, VARIANT_SPINGO_COURCHEVEL8
		, VARIANT_SPINGO_OMAHA
		, VARIANT_SPINGO_OMAHA5C
		, VARIANT_SPINGO_OMAHA8
		, VARIANT_SPINGO_OMAHA85C
		, VARIANT_SPINGO_RAZZ
		, VARIANT_SPINGO_STUD
		, VARIANT_SPINGO_STUD8
		, VARIANT_SPINGO_HOLDEM6
		, VARIANT_SPINGO_OTHER		= 0x01FF
		, VARIANT_ZOOM				= 0x0200
		, VARIANT_ZOOM_HOLDEM
		, VARIANT_ZOOM_27SD
		, VARIANT_ZOOM_27TD
		, VARIANT_ZOOM_5CD
		, VARIANT_ZOOM_BADUGI
		, VARIANT_ZOOM_COURCHEVEL
		, VARIANT_ZOOM_COURCHEVEL8
		, VARIANT_ZOOM_OMAHA
		, VARIANT_ZOOM_OMAHA5C
		, VARIANT_ZOOM_OMAHA8
		, VARIANT_ZOOM_OMAHA85C
		, VARIANT_ZOOM_RAZZ
		, VARIANT_ZOOM_STUD
		, VARIANT_ZOOM_STUD8
		, VARIANT_ZOOM_HOLDEM6
		, VARIANT_ZOOM_OTHER		= 0x02FF
		, VARIANT_5050				= 0x0300
		, VARIANT_5050_HOLDEM
		, VARIANT_5050_27SD
		, VARIANT_5050_27TD
		, VARIANT_5050_5CD
		, VARIANT_5050_BADUGI
		, VARIANT_5050_COURCHEVEL
		, VARIANT_5050_COURCHEVEL8
		, VARIANT_5050_OMAHA
		, VARIANT_5050_OMAHA5C
		, VARIANT_5050_OMAHA8
		, VARIANT_5050_OMAHA85C
		, VARIANT_5050_RAZZ
		, VARIANT_5050_STUD
		, VARIANT_5050_STUD8
		, VARIANT_5050_HOLDEM6
		, VARIANT_5050_OTHER		= 0x03FF
		, VARIANT_POWERUP			= 0x0400
		, VARIANT_POWERUP_HOLDEM
		, VARIANT_POWERUP_27SD
		, VARIANT_POWERUP_27TD
		, VARIANT_POWERUP_5CD
		, VARIANT_POWERUP_BADUGI
		, VARIANT_POWERUP_COURCHEVEL
		, VARIANT_POWERUP_COURCHEVEL8
		, VARIANT_POWERUP_OMAHA
		, VARIANT_POWERUP_OMAHA5C
		, VARIANT_POWERUP_OMAHA8
		, VARIANT_POWERUP_OMAHA85C
		, VARIANT_POWERUP_RAZZ
		, VARIANT_POWERUP_STUD
		, VARIANT_POWERUP_STUD8
		, VARIANT_POWERUP_HOLDEM6
		, VARIANT_POWERUP_OTHER		= 0x04FF
		, VARIANT_OTHER				= 0x7FFF
	};

	enum SpeedEnum: uint16_t
	{
		SPEED_UNKNOWN				= 0
		, SPEED_TIME				= 0x0100
		, SPEED_TIME_SLOW			= 0x0110
		, SPEED_TIME_REGULAR		= 0x0120
		, SPEED_TIME_TURBO			= 0x0130
		, SPEED_TIME_HYPER			= 0x0140
		, SPEED_TIME_OTHER			= 0x01FF
		, SPEED_HAND				= 0x0400
		, SPEED_HAND_SLOW			= 0x0410
		, SPEED_HAND_REGULAR		= 0x0420
		, SPEED_HAND_TURBO			= 0x0430
		, SPEED_HAND_HYPER			= 0x0440
		, SPEED_HAND_OTHER			= 0x04FF
		, SPEED_OTHER				= 0x7FFF
	};

	enum TurnEnum: uint16_t
	{
		TURN_UNKNOWN = 0
		, TURN_CLICK_FORCED					= 0x0001
		, TURN_CLICK						= 0x0100
		, TURN_CLICK_BET
		, TURN_CLICK_CALL
		, TURN_CLICK_CHECK
		, TURN_CLICK_FOLD
		, TURN_CLICK_FAST_FOLD
		, TURN_CLICK_RAISE
		, TURN_CLICK_ADV_CALL				= 0x0180
		, TURN_CLICK_ADV_CALL_ANY
		, TURN_CLICK_ADV_CALL_ANY_CHECK
		, TURN_CLICK_ADV_CHECK
		, TURN_CLICK_ADV_CHECK_FOLD
		, TURN_CLICK_ADV_FOLD
		, TURN_CLICK_ADV_FOLD_ANY
		, TURN_CLICK_OTHER					= 0x01FF
		, TURN_HOTKEY						= 0x0200
		, TURN_HOTKEY_BET
		, TURN_HOTKEY_CALL
		, TURN_HOTKEY_CHECK
		, TURN_HOTKEY_FOLD
		, TURN_HOTKEY_FAST_FOLD
		, TURN_HOTKEY_RAISE
		, TURN_HOTKEY_ADV_CALL				= 0x0280
		, TURN_HOTKEY_ADV_CALL_ANY
		, TURN_HOTKEY_ADV_CALL_ANY_CHECK
		, TURN_HOTKEY_ADV_CHECK
		, TURN_HOTKEY_ADV_CHECK_FOLD
		, TURN_HOTKEY_ADV_FOLD
		, TURN_HOTKEY_ADV_FOLD_ANY
		, TURN_HOTKEY_OTHER					= 0x02FF
		, TURN_OTHER						= 0x7FFF
	};

	enum CurrencyEnum : uint16_t {
		CURRENCY_NONE = 0
		, CURRENCY_USD = 840  // as per ISO 4217
		, CURRENCY_EUR = 978
		, CURRENCY_GBP = 826
		, CURRENCY_CAD = 124
		, CURRENCY_INR = 356
		, CURRENCY_STARS_COIN = 0x7000
		, CURRENCY_OTHER = 0x7FFF
	};

};

#define REF_ID_SIZE									24
#define COMMAND_CONTENTS_SIZE						256
#define COMMAND_CONTENT_SIZE						16777216 // 16MiB
#define COMMAND_EXPR_SIZE							1024
#define RESPONSE_EXPR_SIZE							1024
#define DEVICE_SID_SIZE								24
#define DEVICE_LANG_SIZE							32
#define DEVICE_OS_VER_SIZE							64
#define DEVICE_OS_SID_SIZE							64
#define DEVICE_OS_ACCOUNT_SIZE						64
#define DEVICE_APP_VER_SIZE							32
#define DEVICE_APP_SID_SIZE							24
#define DEVICE_TIME_ZONE_SIZE						32
#define DEVICE_NETWORK_PROVIDER_SIZE				32
#define DEVICE_COUNTRY_CODE_SIZE					2
#define DEVICE_REGION_CODE_SIZE						3
#define DEVICE_CITY_SIZE							32
#define DEVICE_DEGREE_SCALE_FACTOR					1000000
#define TAG_KEY_SIZE								16
#define TAG_MAKE_SIZE								32
#define TAG_MODEL_SIZE								64
#define TAG_SERIAL_SIZE								32
#define FILE_HASH_SIZE								16
#define FILE_NAME_SIZE								256
#define FILE_DIR_SIZE								256
#define FILE_CONTENT_SIZE							16777216 // 16MiB
#define TABLE_NAME_SIZE								32
#define TABLE_THEME_SIZE							32
#define EXEC_NAME_SIZE								256
#define EXEC_CONTENT_SIZE							512

#endif
