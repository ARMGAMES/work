#ifndef TimeUtils_h_included
#define	TimeUtils_h_included

////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2014 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "commtime.h"
#include "pyrtime.h"
#include "dbm.h"

template < class TTime, class TDate >
struct TimeUtilsTpl
{
	static void makeStartMonth(TTime& date)
	{
		date._day = 1;
		date._hour = 0;
		date._minute = 0;
		date._second = 0;
	}

	static void makeStartOfNextMonth(TDate& date)
	{
		BYTE m = date._month + 1;
		if (m < 13)
		{
			date._month = m;
		}
		else
		{
			date._month = 1;
			date._year += 1;
		}
		date._day = 1;
	}

	static TDate getNextDay(const TDate& date)
	{
		TDate nextDay;
		nextDay.decode(date.encode() + 36 * SECONDS_IN_AN_HOUR);
		return nextDay;
	}

	static void makeStartOfNextMonth(TTime& date)
	{
		BYTE m = date._month + 1;
		if (m < 13)
		{
			date._month = m;
		}
		else
		{
			date._month = 1;
			date._year += 1;
		}
		makeStartMonth(date);
	}

	static void makeStartOfPrevMonth(TTime& date)
	{
		if (--date._month < 1)
		{
			date._month = 12;
			date._year -= 1;
		}
		makeStartMonth(date);
	}

	static void makeStartOfNextDay(TTime& date)
	{
		date._hour = 0;
		date._minute = 0;
		date._second = 0;
		time_t nextDayEncoded = date.encode() + 36*60*60;
		date.decode(nextDayEncoded);
		date._hour = 0;
		date._minute = 0;
		date._second = 0;
	}

	static void makeStartOfPrevDay(TTime& date)
	{
		date._hour = 23;
		date._minute = 59;
		date._second = 59;
		time_t prevDayEncoded = date.encode() - 36*60*60;
		date.decode(prevDayEncoded);
		date._hour = 0;
		date._minute = 0;
		date._second = 0;
	}

	static void makeStartOfNextHour(TTime& date)
	{
		date._minute = 0;
		date._second = 0;
		time_t nextHourEncoded = date.encode() + 90*60;
		date.decode(nextHourEncoded);
		date._minute = 0;
		date._second = 0;
	}

	static void makeStartOfPrevHour(TTime& date)
	{
		date._minute = 59;
		date._second = 59;
		time_t prevHourEncoded = date.encode() - 90*60;
		date.decode(prevHourEncoded);
		date._minute = 0;
		date._second = 0;
	}

	// IntForDb for 2013-12-23 is 20131223: YYYYMMDD
	static INT32 dateToIntForDb(const TDate& d)
	{
		return (INT32)d._year * 10000 + (INT32)d._month * 100 + (INT32)d._day;
	}

	static TDate intForDbToDate(INT32 intForDb)
	{
		TDate d; 
		d._year   = intForDb / 10000;
		d._month  =(intForDb / 100) % 100;
		d._day    = intForDb % 100;
		return d;
	}

	// Int64ForDb for 2013-12-23-11.20.15 is 20131223112015: YYYYMMDDhhmmss
	static INT64 timeToInt64ForDb(const TTime& t)
	{
		return (INT64)t._year * 10000000000LL + (INT64)t._month * 100000000 + (INT64)t._day * 1000000 + (INT64)t._hour * 10000 + (INT64)t._minute * 100 + t._second;
	}

	static TTime int64ForDbToTime(INT64 int64ForDb)
	{
		TTime t; 
		t._year   = int64ForDb / 10000000000LL;
		t._month  = (int64ForDb / 100000000LL) % 100;
		t._day    = (int64ForDb / 1000000LL) % 100; 
		t._hour   = (int64ForDb / 10000LL) % 100; 
		t._minute = (int64ForDb / 100LL) % 100; 
		t._second = int64ForDb % 100; 
		return t; 
	}

	struct TimeCmpLess
	{
		bool operator()(TTime const& left, TTime const& right) const
		{
			return left.compareTo(right) < 0;
		}
	};
};

struct TimeUtils : TimeUtilsTpl<SrvTime,SrvDate>
{
	static const SrvTime& set1990(SrvTime& t)
	{
		t.setNull();
		t._year = 1990;
		t._month= 1;
		t._day  = 1;
		return t;
	}
	static const SrvTime get1990()
	{
		SrvTime t;
		return set1990( t ); // return will copy 't'
	}
	static bool is1990(const SrvTime& t)
	{
		SrvTime time1990;
		set1990( time1990 );
		return time1990.equals(t);
	}

	static SrvTime mapUtcToSrvTime(const CommUtcTime& utcTime)
	{
		SrvTime srvTime;
		srvTime._year = utcTime._year;
		srvTime._month = utcTime._month;
		srvTime._day = utcTime._day;
		srvTime._hour = utcTime._hour;
		srvTime._minute = utcTime._minute;
		srvTime._second = utcTime._second;
		srvTime._wday = utcTime._wday;
		return srvTime;
	}

	static CommUtcTime mapSrvToUtcTime(const SrvTime& srvTime)
	{
		CommUtcTime utcTime;
		utcTime._year = srvTime._year;
		utcTime._month = srvTime._month;
		utcTime._day = srvTime._day;
		utcTime._hour = srvTime._hour;
		utcTime._minute = srvTime._minute;
		utcTime._second = srvTime._second;
		utcTime._wday = srvTime._wday;
		return utcTime;
	}

	// helper function that takes a UTC timestamp, converts it to server time (typically eastern time) and returns the utc offset
	static INT16 convertUtcToSrvTimeWithOffset(const CommUtcTime& utcTime, SrvTime& localTime)
	{
		localTime.decode( utcTime.encode() );	// convert to SrvTime with timezone consideration

		const CommUtcTime mappedTime = mapSrvToUtcTime( localTime );	// just copies day/month/etc over: no conversion done
		const time_t time_mapped = mappedTime.encode();
		const time_t time_utc = utcTime.encode();

		const time_t difference = time_mapped - time_utc;

		return (INT16)(difference / SECONDS_IN_A_MINUTE);
	}

	static SrvTime getCurrentLocalTime()
	{
		SrvTime now;
		now.currentLocalTime();
		return now;
	}

	typedef TimeCmpLess SrvTimeCmpLess; // deprecated, for compatibility only
};

struct TimeUtilsUtc : TimeUtilsTpl<CommUtcTime,CommUtcDate>
{
	static CommUtcTime getCurrentTime()
	{
		CommUtcTime now;
		now.currentTime();
		return now;
	}
};

#endif // TimeUtils_h_included

