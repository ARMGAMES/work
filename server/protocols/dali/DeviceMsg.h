#ifndef __DALI_DEVICE_MSG_H__
#define __DALI_DEVICE_MSG_H__

#include "SampleMsg.h"

#define DEVICE_MSG_HEADER SAMPLE_MSG_HEADER "|Device SID|OS Install Time|OS Ver|OS SID|OS Account|OS Lang|App Install Time|App Ver" \
"|App SID|App Lang|User Lang|Kbd Lang|Timezone|Time offset|VM flag|Cap flag|Network flag|Network Provider|Local MAC|Gateway MAC" \
"|Public IP|Country Code|Region Code|City|Latitude|Longitude"

struct DeviceMsg // dali device data
	: public SampleMsg
{

	struct Key
		: public SampleMsg::Key
	{

		string os_account;
		string os_ver;
		uint64_t local_mac;
		uint64_t gateway_mac;
		string public_ip;

		Key( uint32_t uid = svclib::NULL_U32, uint64_t did = svclib::NULL_U64 )
			: SampleMsg::Key( uid, did )
			, os_account( svclib::NULL_STR )
			, os_ver( svclib::NULL_STR )
			, local_mac( svclib::NULL_U64 )
			, gateway_mac( svclib::NULL_U64 )
			, public_ip( svclib::NULL_STR )
		{}

		Key( const Key& that )
			: SampleMsg::Key( that )
			, os_account( that.os_account )
			, os_ver( that.os_ver )
			, local_mac( that.local_mac )
			, gateway_mac( that.gateway_mac )
			, public_ip( that.public_ip )
		{}

		Key& operator=( const Key& that )
		{
			SampleMsg::Key::operator=( that );
			if ( this != &that )
			{
				os_account = that.os_account;
				os_ver = that.os_ver;
				local_mac = that.local_mac;
				gateway_mac = that.gateway_mac;
				public_ip = that.public_ip;
			}
			return *this;
		}

		dali::MsgParser& parse( dali::MsgParser& parser ) override
		{
			return SampleMsg::Key::parse( parser )
				.parseString( os_account, 2 * DEVICE_OS_ACCOUNT_SIZE, "2 * DEVICE_OS_ACCOUNT_SIZE" )
				.parseString( os_ver, 2 * DEVICE_OS_VER_SIZE, "2 * DEVICE_OS_VER_SIZE" )
				.parseUint64( local_mac )
				.parseUint64( gateway_mac )
				.parseString( public_ip, 2 * IP_ADDR_SIZE, "2 * IP_ADDR_SIZE" )
				;
		}

		dali::MsgComposer& compose( dali::MsgComposer& composer ) const override
		{
			return SampleMsg::Key::compose( composer )
				.composeString( os_account )
				.composeString( os_ver )
				.composeUint64( local_mac )
				.composeUint64( gateway_mac )
				.composeString( public_ip )
				;
		}

		virtual void format() override
		{
			SampleMsg::Key::format();
			dali::cut( os_account, DEVICE_OS_ACCOUNT_SIZE );
			dali::cut( os_ver, DEVICE_OS_VER_SIZE );
			dali::cut( public_ip, IP_ADDR_SIZE );
		}

		string toString() const override
		{
			return SampleMsg::Key::toString()
				.append( " os_account=" ).append( os_account )
				.append( " os_ver=" ).append( os_ver )
				.append( " local_mac=" ).append( dali::hexString( local_mac ) )
				.append( " gateway_mac=" ).append( dali::hexString( gateway_mac ) )
				.append( " public_ip=" ).append( public_ip )
				;
		}

	};

	string device_sid;				// dsid
	int64_t os_install_time;		// sint - microseconds since epoch: 1970-01-01
	string os_ver;					// sver
	string os_sid;					// ssid
	string os_account;				// sacc
	string os_lang;					// slang
	int64_t app_install_time;		// aint - microseconds since epoch: 1970-01-01
	string app_ver;					// aver
	string app_sid;					// asid
	string app_lang;				// alang
	string user_lang;				// ulang
	string kbd_lang;				// klang
	string time_zone;				// tz
	int16_t time_offset;			// to
	uint32_t vm_flag;				// vm
	uint32_t cap_flag;				// cap
	uint32_t network_flag;			// net
	string network_provider;		// prov
	uint64_t local_mac;				// mac
	uint64_t gateway_mac;			// gmac
	string public_ip;				// pip
	string country_code;			// cc
	string region_code;				// rc
	string city;					// city
	int32_t latitude;				// lat - uDegree
	int32_t longitude;				// lon - uDegree

	DeviceMsg()
		: os_install_time( 0 )
		, app_install_time( 0 )
		, time_offset( 0 )
		, vm_flag( 0 )
		, cap_flag( 0 )
		, network_flag( 0 )
		, local_mac( 0 )
		, gateway_mac( 0 )
		, latitude( 0 )
		, longitude( 0 )
	{}

	DeviceMsg( const Key & key )
		: SampleMsg( key )
		, os_install_time( 0 )
		, os_ver( key.os_ver )
		, os_account( key.os_account )
		, app_install_time( 0 )
		, time_offset( 0 )
		, vm_flag( 0 )
		, cap_flag( 0 )
		, network_flag( 0 )
		, local_mac( key.local_mac )
		, gateway_mac( key.gateway_mac )
		, public_ip( key.public_ip )
		, latitude( 0 )
		, longitude( 0 )
	{}

	void format() override
	{
		SampleMsg::format();
		dali::cut( device_sid, DEVICE_SID_SIZE );
		dali::cut( os_ver, DEVICE_OS_VER_SIZE );
		dali::cut( os_sid, DEVICE_OS_SID_SIZE );
		dali::cut( os_account, DEVICE_OS_ACCOUNT_SIZE );
		dali::cut( os_lang, DEVICE_LANG_SIZE );
		dali::cut( app_ver, DEVICE_APP_VER_SIZE );
		dali::cut( app_sid, DEVICE_APP_SID_SIZE );
		dali::cut( app_lang, DEVICE_LANG_SIZE );
		dali::cut( user_lang, DEVICE_LANG_SIZE );
		dali::cut( kbd_lang, DEVICE_LANG_SIZE );
		dali::cut( time_zone, DEVICE_TIME_ZONE_SIZE );
		dali::cut( network_provider, DEVICE_NETWORK_PROVIDER_SIZE );
		dali::cut( public_ip, IP_ADDR_SIZE );
		dali::cut( country_code, DEVICE_COUNTRY_CODE_SIZE );
		dali::cut( region_code, DEVICE_REGION_CODE_SIZE );
		dali::cut( city, DEVICE_CITY_SIZE );
	}

	dali::MsgParser& parse( dali::MsgParser& parser ) override
	{
		SampleMsg::parse( parser )
			.parseString( device_sid, 2 * DEVICE_SID_SIZE, "2 * DEVICE_SID_SIZE" )
			.parseInt64( os_install_time )
			.parseString( os_ver, 2 * DEVICE_OS_VER_SIZE, "2 * DEVICE_OS_VER_SIZE" )
			.parseString( os_sid, 2 * DEVICE_OS_SID_SIZE, "2 * DEVICE_OS_SID_SIZE" )
			.parseString( os_account, 2 * DEVICE_OS_ACCOUNT_SIZE, "2 * DEVICE_OS_ACCOUNT_SIZE" )
			.parseString( os_lang, 2 * DEVICE_LANG_SIZE, "2 * DEVICE_LANG_SIZE:OS" )
			.parseInt64( app_install_time )
			.parseString( app_ver, 2 * DEVICE_APP_VER_SIZE, "2 * DEVICE_APP_VER_SIZE" )
			.parseString( app_sid, 2 * DEVICE_APP_SID_SIZE, "2 * DEVICE_APP_SID_SIZE" )
			.parseString( app_lang, 2 * DEVICE_LANG_SIZE, "2 * DEVICE_LANG_SIZE:APP" )
			.parseString( user_lang, 2 * DEVICE_LANG_SIZE, "2 * DEVICE_LANG_SIZE:USER" )
			.parseString( kbd_lang, 2 * DEVICE_LANG_SIZE, "2 * DEVICE_LANG_SIZE:KBD" )
			.parseString( time_zone, 2 * DEVICE_TIME_ZONE_SIZE, "2 * DEVICE_TIME_ZONE_SIZE" )
			.parseInt16( time_offset )
			.parseUint32( vm_flag )
			.parseUint32( cap_flag )
			.parseUint32( network_flag )
			.parseString( network_provider, 2 * DEVICE_NETWORK_PROVIDER_SIZE, "2 * DEVICE_NETWORK_PROVIDER_SIZE" )
			.parseUint64( local_mac )
			.parseUint64( gateway_mac )
			.parseString( public_ip, 2 * IP_ADDR_SIZE, "2 * IP_ADDR_SIZE" )
			.parseString( country_code, 2 * DEVICE_COUNTRY_CODE_SIZE, "2 * DEVICE_COUNTRY_CODE_SIZE" )
			.parseString( region_code, 2 * DEVICE_REGION_CODE_SIZE, "2 * DEVICE_REGION_CODE_SIZE" )
			.parseString( city, 2 * DEVICE_CITY_SIZE, "2 * DEVICE_CITY_SIZE" )
			.parseInt32( latitude )
			.parseInt32( longitude )			
			;
		format();
		return parser;
	}

	dali::MsgComposer& compose( dali::MsgComposer& composer ) const override
	{
		return SampleMsg::compose( composer )
			.composeString( device_sid )
			.composeInt64( os_install_time )
			.composeString( os_ver )
			.composeString( os_sid )
			.composeString( os_account )
			.composeString( os_lang )
			.composeInt64( app_install_time )
			.composeString( app_ver )
			.composeString( app_sid )
			.composeString( app_lang )
			.composeString( user_lang )
			.composeString( kbd_lang )
			.composeString( time_zone )
			.composeInt16( time_offset )
			.composeUint32( vm_flag )
			.composeUint32( cap_flag )
			.composeUint32( network_flag )
			.composeString( network_provider )
			.composeUint64( local_mac )
			.composeUint64( gateway_mac )
			.composeString( public_ip )
			.composeString( country_code )
			.composeString( region_code )
			.composeString( city )
			.composeInt32( latitude )
			.composeInt32( longitude )			
			;
	}

	string toString() const override
	{
		return SampleMsg::toString()
			.append( " device_sid=" ).append( device_sid.c_str() )
			.append( " os_install_time=" ).append( dali::toString( os_install_time ) )
			.append( " os_ver=" ).append( os_ver )
			.append( " os_sid=" ).append( os_sid )
			.append( " os_account=" ).append( os_account )
			.append( " os_lang=" ).append( os_lang )
			.append( " app_install_time=" ).append( dali::toString( app_install_time ) )
			.append( " app_ver=" ).append( app_ver )
			.append( " app_sid=" ).append( app_sid )
			.append( " app_lang=" ).append( app_lang )
			.append( " user_lang=" ).append( user_lang )
			.append( " kbd_lang=" ).append( kbd_lang )
			.append( " time_zone=" ).append( time_zone )
			.append( " time_offset=" ).append( dali::intString( time_offset ) )
			.append( " vm_flag=0x" ).append( dali::hexString( vm_flag ) )
			.append( " cap_flag=0x" ).append( dali::hexString( cap_flag ) )
			.append( " network_flag=0x" ).append( dali::hexString( network_flag ) )
			.append( " network_provider=" ).append( network_provider )
			.append( " local_mac=" ).append( dali::hexString( local_mac ) )
			.append( " gateway_mac=" ).append( dali::hexString( gateway_mac ) )
			.append( " public_ip=" ).append( public_ip )
			.append( " country_code=" ).append( country_code )
			.append( " region_code=" ).append( region_code )
			.append( " city=" ).append( city )
			.append( " latitude=" ).append( dali::intString( latitude ) )
			.append( " longitude=" ).append( dali::intString( longitude ) )
			;
	}

	string toLine() const override
	{
		return SampleMsg::toLine()
			.append( "|" ).append( dali::quoteString( device_sid ) )
			.append( "|" ).append( dali::toString( os_install_time ) )
			.append( "|" ).append( dali::quoteString( os_ver ) )
			.append( "|" ).append( dali::quoteString( os_sid ) )
			.append( "|" ).append( dali::quoteString( os_account ) )
			.append( "|" ).append( dali::quoteString( os_lang ) )
			.append( "|" ).append( dali::toString( app_install_time ) )
			.append( "|" ).append( dali::quoteString( app_ver ) )
			.append( "|" ).append( dali::quoteString( app_sid ) )
			.append( "|" ).append( dali::quoteString( app_lang ) )
			.append( "|" ).append( dali::quoteString( user_lang ) )
			.append( "|" ).append( dali::quoteString( kbd_lang ) )
			.append( "|" ).append( dali::quoteString( time_zone ) )
			.append( "|" ).append( dali::intString( time_offset ) )
			.append( "|" ).append( dali::hexString( vm_flag ) )
			.append( "|" ).append( dali::hexString( cap_flag ) )
			.append( "|" ).append( dali::hexString( network_flag ) )
			.append( "|" ).append( dali::quoteString( network_provider ) )
			.append( "|" ).append( dali::hexString( local_mac ) )
			.append( "|" ).append( dali::hexString( gateway_mac ) )
			.append( "|" ).append( dali::quoteString( public_ip ) )
			.append( "|" ).append( dali::quoteString( country_code ) )
			.append( "|" ).append( dali::quoteString( region_code ) )
			.append( "|" ).append( dali::quoteString( city ) )
			.append( "|" ).append( dali::intString( latitude ) )
			.append( "|" ).append( dali::intString( longitude ) )
			;
	}

	static std::string header()
	{
		return DEVICE_MSG_HEADER;
	}

	typedef TimeMsg::Query< Key > Query;
	typedef BaseMsg::Key DeltaKey ;
	typedef TimeMsg::DeltaQuery< DeltaKey > DeltaQuery;

};

#endif
