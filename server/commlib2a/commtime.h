#ifndef	commtime_h_included
#define	commtime_h_included

#include    <pptime.h>
#include	"commmsgbody.h"

struct CommSrvTime
{
	UINT16	_year;
	BYTE	_month;
	BYTE	_day;
	BYTE	_hour;
	BYTE	_minute;
	BYTE	_second;
	BYTE	_wday;
private:
	void _assign( const struct tm& str_tm )
	{
		_year	= static_cast< UINT16 >( str_tm.tm_year + 1900 );
		_month	= static_cast< BYTE >( str_tm.tm_mon + 1 );
		_day	= static_cast< BYTE >( str_tm.tm_mday );
		_hour	= static_cast< BYTE >( str_tm.tm_hour );
		_minute	= static_cast< BYTE >( str_tm.tm_min );
		_second	= static_cast< BYTE >( str_tm.tm_sec );
		_wday	= static_cast< BYTE >( str_tm.tm_wday );
	}

	void _mk_tm( struct tm& str_tm ) const
	{
		str_tm.tm_year	= _year - 1900;
		str_tm.tm_mon	= _month - 1;
		str_tm.tm_mday	= _day;
		str_tm.tm_hour	= _hour;
		str_tm.tm_min	= _minute;
		str_tm.tm_sec	= _second;
		str_tm.tm_wday	= _wday;
		str_tm.tm_isdst = -1;
	}

public:
	void parse( CommMsgParser& parser )
	{
		parser.
			parseUINT16(_year).
			parseBYTE(_month).
			parseBYTE(_day).
			parseBYTE(_hour).
			parseBYTE(_minute).
			parseBYTE(_second).
			parseBYTE(_wday);
	}
	void compose(CommMsgBody& msg) const
	{
		msg.
			composeUINT16(_year).
			composeBYTE(_month).
			composeBYTE(_day).
			composeBYTE(_hour).
			composeBYTE(_minute).
			composeBYTE(_second).
			composeBYTE(_wday);
	}

	void decode(time_t a_time)
	{
		struct tm str_tm;
		if( plib_localtime_ext( a_time, &str_tm ) )
			_assign( str_tm );
		else
			setNull();
	}

	time_t encode() const
	{
		struct tm str_tm;
		_mk_tm( str_tm );
		return plib_mktime(&str_tm);
	}

	CommSrvTime()
	{
		setNull();
	}

	explicit CommSrvTime( time_t a_time )
	{
		decode( a_time );
	}

	explicit CommSrvTime( const struct tm& str_tm )
	{
		_assign( str_tm );
	}

	void assign( const struct tm& str_tm )
	{
		_assign( str_tm );
	}

	void mk_tm(struct tm& str_tm ) const
	{
		_mk_tm( str_tm );
	}

	bool equals( const CommSrvTime& dst ) const
	{
		if( _year != dst._year )
			return false;
		if( _month != dst._month )
			return false;
		if( _day != dst._day )
			return false;
		if( _hour != dst._hour )
			return false;
		if( _minute != dst._minute )
			return false;
		if( _second != dst._second )
			return false;
		return true;
	}

	int compareTo( const CommSrvTime& dst ) const
	{
		if( _year != dst._year )
			return (int)_year - (int)dst._year;
		if( _month != dst._month )
			return (int)_month - (int)dst._month;
		if( _day != dst._day )
			return (int)_day - (int)dst._day;
		if( _hour != dst._hour )
			return (int)_hour - (int)dst._hour;
		if( _minute != dst._minute )
			return (int)_minute - (int)dst._minute;
		if( _second != dst._second )
			return (int)_second - (int)dst._second;
		return 0;
	}

	bool isNull() const { return ( 0 == _year ); }
	void setNull() { memset(this, 0, sizeof(*this)); }
	void currentLocalTime()
	{
		struct tm lt;
		PCurrentLocalTime( &lt );
		_assign( lt );
	}
};

struct CommSrvDate
{
	UINT16	_year;
	BYTE	_month;
	BYTE	_day;
private:
	void _assign( const struct tm& str_tm )
	{
		_year	= static_cast< UINT16 >( str_tm.tm_year + 1900 );
		_month	= static_cast< BYTE >( str_tm.tm_mon + 1 );
		_day	= static_cast< BYTE >( str_tm.tm_mday );
	}
public:
	void parse( CommMsgParser& parser )
	{
		parser.
			parseUINT16(_year).
			parseBYTE(_month).
			parseBYTE(_day);
	}
	void compose(CommMsgBody& msg) const
	{
		msg.
			composeUINT16(_year).
			composeBYTE(_month).
			composeBYTE(_day);
	}

	void decode(time_t a_time)
	{
		struct tm str_tm;
		if( plib_localtime_ext( a_time, &str_tm  ) )
			_assign( str_tm );
		else
			setNull();
	}

	time_t encode() const
	{
		struct tm str_tm;
		memset( &str_tm, 0, sizeof( str_tm ) );
		str_tm.tm_year	= _year - 1900;
		str_tm.tm_mon	= _month - 1;
		str_tm.tm_mday	= _day;
		str_tm.tm_isdst = -1;
		return plib_mktime(&str_tm);
	}

	CommSrvDate()
	{
		setNull();
	}

	explicit CommSrvDate( time_t a_time )
	{
		decode( a_time );
	}

	explicit CommSrvDate( const CommSrvTime& t )
	{
		_year = t._year;
		_month = t._month;
		_day = t._day;
	}

	bool equals( const CommSrvDate& dst ) const
	{
		if( _year != dst._year )
			return false;
		if( _month != dst._month )
			return false;
		if( _day != dst._day )
			return false;
		return true;
	}

	int compareTo( const CommSrvDate& dst ) const
	{
		if( _year != dst._year )
			return _year - dst._year;
		if( _month != dst._month )
			return _month - dst._month;
		if( _day != dst._day )
			return _day - dst._day;

		return 0;
	}

	bool equals( const CommSrvTime& dst ) const
	{
		if( _year != dst._year )
			return false;
		if( _month != dst._month )
			return false;
		if( _day != dst._day )
			return false;
		return true;
	}

	int compareTo( const CommSrvTime& dst ) const
	{
		if( _year != dst._year )
			return _year - dst._year;
		if( _month != dst._month )
			return _month - dst._month;
		if( _day != dst._day )
			return _day - dst._day;

		return 0;
	}

	bool isNull() const { return ( 0 == _year ); }
	void setNull() { memset(this, 0, sizeof(*this)); }
	void assign(const CommSrvTime& srvTime)
	{
		_year = srvTime._year;
		_month = srvTime._month;
		_day = srvTime._day;
	}
	void currentLocalDate()
	{
		struct tm lt;
		PCurrentLocalTime( &lt );
		_assign( lt );
	}
};

struct CommSrvInterval
{
	CommSrvTime timeFrom;
	CommSrvTime timeTo;
	void compose(CommMsgBody& body) const
	{
		timeFrom.compose(body);
		timeTo.compose(body);
	}
	void parse(CommMsgParser& parser)
	{
		timeFrom.parse(parser);
		timeTo.parse(parser);
	}
};


struct CommUtcTime
{
	UINT16	_year;
	BYTE	_month;
	BYTE	_day;
	BYTE	_hour;
	BYTE	_minute;
	BYTE	_second;
	BYTE	_wday;
private:
	void _assign( const struct tm& str_tm )
	{
		_year	= static_cast< UINT16 >( str_tm.tm_year + 1900 );
		_month	= static_cast< BYTE >( str_tm.tm_mon + 1 );
		_day	= static_cast< BYTE >( str_tm.tm_mday );
		_hour	= static_cast< BYTE >( str_tm.tm_hour );
		_minute	= static_cast< BYTE >( str_tm.tm_min );
		_second	= static_cast< BYTE >( str_tm.tm_sec );
		_wday	= static_cast< BYTE >( str_tm.tm_wday );
	}

	void _mk_tm( struct tm& str_tm ) const
	{
		str_tm.tm_year	= _year - 1900;
		str_tm.tm_mon	= _month - 1;
		str_tm.tm_mday	= _day;
		str_tm.tm_hour	= _hour;
		str_tm.tm_min	= _minute;
		str_tm.tm_sec	= _second;
		str_tm.tm_wday	= _wday;
		str_tm.tm_isdst = -1;
	}

public:
	void parse( CommMsgParser& parser )
	{
		parser.
			parseUINT16(_year).
			parseBYTE(_month).
			parseBYTE(_day).
			parseBYTE(_hour).
			parseBYTE(_minute).
			parseBYTE(_second).
			parseBYTE(_wday);
	}
	void compose(CommMsgBody& msg) const
	{
		msg.
			composeUINT16(_year).
			composeBYTE(_month).
			composeBYTE(_day).
			composeBYTE(_hour).
			composeBYTE(_minute).
			composeBYTE(_second).
			composeBYTE(_wday);
	}

	void decode(time_t a_time)
	{
		struct tm str_tm;
		if( plib_timetToStructTm( a_time, &str_tm ) )
			_assign( str_tm );
		else
			setNull();
	}

	time_t encode() const
	{
		struct tm str_tm;
		_mk_tm( str_tm );
		return plib_mkUTCtime(&str_tm);
	}

	CommUtcTime()
	{
		setNull();
	}

	explicit CommUtcTime( time_t a_time )
	{
		decode( a_time );
	}

	explicit CommUtcTime( const struct tm& str_tm )
	{
		_assign( str_tm );
	}

	void assign( const struct tm& str_tm )
	{
		_assign( str_tm );
	}

	void mk_tm(struct tm& str_tm ) const
	{
		_mk_tm( str_tm );
	}

	bool equals( const CommUtcTime& dst ) const
	{
		if( _year != dst._year )
			return false;
		if( _month != dst._month )
			return false;
		if( _day != dst._day )
			return false;
		if( _hour != dst._hour )
			return false;
		if( _minute != dst._minute )
			return false;
		if( _second != dst._second )
			return false;
		return true;
	}

	int compareTo( const CommUtcTime& dst ) const
	{
		if( _year != dst._year )
			return (int)_year - (int)dst._year;
		if( _month != dst._month )
			return (int)_month - (int)dst._month;
		if( _day != dst._day )
			return (int)_day - (int)dst._day;
		if( _hour != dst._hour )
			return (int)_hour - (int)dst._hour;
		if( _minute != dst._minute )
			return (int)_minute - (int)dst._minute;
		if( _second != dst._second )
			return (int)_second - (int)dst._second;
		return 0;
	}

	bool isNull() const { return ( 0 == _year ); }
	void setNull() { memset(this, 0, sizeof(*this)); }
	void currentTime()
	{
		struct tm lt;
		PCurrentUTCTime( &lt );
		_assign( lt );
	}
};

struct CommUtcDate
{
	UINT16	_year;
	BYTE	_month;
	BYTE	_day;
private:
	void _assign( const struct tm& str_tm )
	{
		_year	= static_cast< UINT16 >( str_tm.tm_year + 1900 );
		_month	= static_cast< BYTE >( str_tm.tm_mon + 1 );
		_day	= static_cast< BYTE >( str_tm.tm_mday );
	}
	void _mk_tm( struct tm& str_tm ) const
	{
		str_tm.tm_year	= _year - 1900;
		str_tm.tm_mon	= _month - 1;
		str_tm.tm_mday	= _day;
		str_tm.tm_hour	= 0;
		str_tm.tm_min	= 0;
		str_tm.tm_sec	= 0;
		str_tm.tm_wday	= 0; // not correct, but better than uninitialized
		str_tm.tm_isdst = -1;
	}

public:
	void parse( CommMsgParser& parser )
	{
		parser.
			parseUINT16(_year).
			parseBYTE(_month).
			parseBYTE(_day);
	}
	void compose(CommMsgBody& msg) const
	{
		msg.
			composeUINT16(_year).
			composeBYTE(_month).
			composeBYTE(_day);
	}

	void decode(time_t a_time)
	{
		struct tm str_tm;
		if( plib_timetToStructTm( a_time, &str_tm ) )
			_assign( str_tm );
		else
			setNull();
	}

	time_t encode() const
	{
		struct tm str_tm;
		_mk_tm( str_tm );
		return plib_mkUTCtime(&str_tm);
	}

	CommUtcDate()
	{
		setNull();
	}

	explicit CommUtcDate( time_t a_time )
	{
		decode( a_time );
	}

	explicit CommUtcDate( const CommUtcTime& t)
	{
		_year = t._year;
		_month = t._month;
		_day = t._day;
	}

	bool equals( const CommUtcDate& dst ) const
	{
		if( _year != dst._year )
			return false;
		if( _month != dst._month )
			return false;
		if( _day != dst._day )
			return false;
		return true;
	}

	int compareTo( const CommUtcDate& dst ) const
	{
		if( _year != dst._year )
			return _year - dst._year;
		if( _month != dst._month )
			return _month - dst._month;
		if( _day != dst._day )
			return _day - dst._day;

		return 0;
	}

	bool equals( const CommUtcTime& dst ) const
	{
		if( _year != dst._year )
			return false;
		if( _month != dst._month )
			return false;
		if( _day != dst._day )
			return false;
		return true;
	}

	int compareTo( const CommUtcTime& dst ) const
	{
		if( _year != dst._year )
			return _year - dst._year;
		if( _month != dst._month )
			return _month - dst._month;
		if( _day != dst._day )
			return _day - dst._day;

		return 0;
	}

	bool isNull() const { return ( 0 == _year ); }
	void setNull() { memset(this, 0, sizeof(*this)); }
	void assign(const CommUtcTime& srvTime)
	{
		_year = srvTime._year;
		_month = srvTime._month;
		_day = srvTime._day;
	}
	void currentLocalDate()
	{
		struct tm lt;
		PCurrentUTCTime( &lt );
		_assign( lt );
	}
};


#endif	//commtime_h_included
