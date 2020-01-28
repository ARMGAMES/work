#include "initdatetime.h"

void initTimeStruct(SQL_TIMESTAMP_STRUCT& ts, const SrvTime& st )
{
	ts.year			= st._year;
	ts.month		= st._month;
	ts.day			= st._day;
	ts.hour			= st._hour;
	ts.minute		= st._minute;
	ts.second		= st._second;
	ts.fraction		= 0;
}
// for convenience when we have CommUtcTime instead of SrvTime
void initTimeStruct(SQL_TIMESTAMP_STRUCT& ts, const CommUtcTime& st )
{
	ts.year			= st._year;
	ts.month		= st._month;
	ts.day			= st._day;
	ts.hour			= st._hour;
	ts.minute		= st._minute;
	ts.second		= st._second;
	ts.fraction		= 0;
}

void initDateStruct(SQL_DATE_STRUCT& ds, const SrvDate& sd )
{
	ds.year	 = sd._year;
	ds.month = sd._month;
	ds.day   = sd._day;
}

void initSrvTime(SrvTime& srvTime, const SQL_TIMESTAMP_STRUCT* sqlTime)
{
	srvTime._year	= sqlTime->year;
	srvTime._month	= sqlTime->month;
	srvTime._day	= sqlTime->day;
	srvTime._hour	= sqlTime->hour;
	srvTime._minute	= sqlTime->minute;
	srvTime._second	= sqlTime->second;
	//srvTime._wday	= 0;
}
// for convenience when we have CommUtcTime instead of SrvTime
void initSrvTime(CommUtcTime& srvTime, const SQL_TIMESTAMP_STRUCT* sqlTime)
{
	srvTime._year	= sqlTime->year;
	srvTime._month	= sqlTime->month;
	srvTime._day	= sqlTime->day;
	srvTime._hour	= sqlTime->hour;
	srvTime._minute	= sqlTime->minute;
	srvTime._second	= sqlTime->second;
	//srvTime._wday	= 0;
}

void initSrvDate(SrvDate& srvDate, const SQL_DATE_STRUCT* sqlDate)
{
	srvDate._year	= sqlDate->year;
	srvDate._month	= sqlDate->month;
	srvDate._day	= sqlDate->day;
}

void initTimeStructDateHMS(
	SQL_TIMESTAMP_STRUCT& ts, 
	const SrvDate& sd,
	BYTE	_hour,
	BYTE	_minute,
	BYTE	_second
	)
{
	ts.year			= sd._year;
	ts.month		= sd._month;
	ts.day			= sd._day;
	ts.hour			= _hour;
	ts.minute		= _minute;
	ts.second		= _second;
	ts.fraction		= 0;
}