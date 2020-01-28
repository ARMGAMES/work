#include "timezone.h"

//some data
static const UINT32 yearsS[] =
{
	((TZ_START_YEAR-1970)*365+(TZ_START_YEAR-1969)/4)*24*60*60,		//2000
	((TZ_START_YEAR+1-1970)*365+(TZ_START_YEAR+1-1969)/4)*24*60*60,	//2001
	((TZ_START_YEAR+2-1970)*365+(TZ_START_YEAR+2-1969)/4)*24*60*60,
	((TZ_START_YEAR+3-1970)*365+(TZ_START_YEAR+3-1969)/4)*24*60*60,
	((TZ_START_YEAR+4-1970)*365+(TZ_START_YEAR+4-1969)/4)*24*60*60,
	((TZ_START_YEAR+5-1970)*365+(TZ_START_YEAR+5-1969)/4)*24*60*60,
	((TZ_START_YEAR+6-1970)*365+(TZ_START_YEAR+6-1969)/4)*24*60*60,
	((TZ_START_YEAR+7-1970)*365+(TZ_START_YEAR+7-1969)/4)*24*60*60,
	((TZ_START_YEAR+8-1970)*365+(TZ_START_YEAR+8-1969)/4)*24*60*60,
	((TZ_START_YEAR+9-1970)*365+(TZ_START_YEAR+9-1969)/4)*24*60*60,
	((TZ_START_YEAR+10-1970)*365+(TZ_START_YEAR+10-1969)/4)*24*60*60,
	((TZ_START_YEAR+11-1970)*365+(TZ_START_YEAR+11-1969)/4)*24*60*60,	//2011
	((TZ_START_YEAR+12-1970)*365+(TZ_START_YEAR+12-1969)/4)*24*60*60,	//2012
	((TZ_START_YEAR+13-1970)*365+(TZ_START_YEAR+13-1969)/4)*24*60*60,
	((TZ_START_YEAR+14-1970)*365+(TZ_START_YEAR+14-1969)/4)*24*60*60,
	((TZ_START_YEAR+15-1970)*365+(TZ_START_YEAR+15-1969)/4)*24*60*60,
	((TZ_START_YEAR+16-1970)*365+(TZ_START_YEAR+16-1969)/4)*24*60*60,
	((TZ_START_YEAR+17-1970)*365+(TZ_START_YEAR+17-1969)/4)*24*60*60,
	((TZ_START_YEAR+18-1970)*365+(TZ_START_YEAR+18-1969)/4)*24*60*60,
	((TZ_START_YEAR+19-1970)*365+(TZ_START_YEAR+19-1969)/4)*24*60*60,
	((TZ_START_YEAR+20-1970)*365+(TZ_START_YEAR+20-1969)/4)*24*60*60,	//2020
	((TZ_START_YEAR+21-1970)*365+(TZ_START_YEAR+21-1969)/4)*24*60*60,
	((TZ_START_YEAR+22-1970)*365+(TZ_START_YEAR+22-1969)/4)*24*60*60,
	((TZ_START_YEAR+23-1970)*365+(TZ_START_YEAR+23-1969)/4)*24*60*60,
	((TZ_START_YEAR+24-1970)*365+(TZ_START_YEAR+24-1969)/4)*24*60*60,
	((TZ_START_YEAR+25-1970)*365+(TZ_START_YEAR+25-1969)/4)*24*60*60,
	((TZ_START_YEAR+26-1970)*365+(TZ_START_YEAR+26-1969)/4)*24*60*60,
	((TZ_START_YEAR+27-1970)*365+(TZ_START_YEAR+27-1969)/4)*24*60*60,
	((TZ_START_YEAR+28-1970)*365+(TZ_START_YEAR+28-1969)/4)*24*60*60,
	((TZ_START_YEAR+29-1970)*365+(TZ_START_YEAR+29-1969)/4)*24*60*60,
	((TZ_START_YEAR+30-1970)*365+(TZ_START_YEAR+30-1969)/4)*24*60*60,	//2030
};

static const UINT32 monthS[] =
{
	0,
	31*24*60*60,
	(31+28)*24*60*60,
	(31+28+31)*24*60*60,
	(31+28+31+30)*24*60*60,
	(31+28+31+30+31)*24*60*60,
	(31+28+31+30+31+30)*24*60*60,
	(31+28+31+30+31+30+31)*24*60*60,
	(31+28+31+30+31+30+31+31)*24*60*60,
	(31+28+31+30+31+30+31+31+30)*24*60*60,
	(31+28+31+30+31+30+31+31+30+31)*24*60*60,
	(31+28+31+30+31+30+31+31+30+31+30)*24*60*60,
	(31+28+31+30+31+30+31+31+30+31+30+31)*24*60*60
};

static const UINT32 monthLeapS[] =
{
	0,
	31*24*60*60,
	(31+29)*24*60*60,
	(31+29+31)*24*60*60,
	(31+29+31+30)*24*60*60,
	(31+29+31+30+31)*24*60*60,
	(31+29+31+30+31+30)*24*60*60,
	(31+29+31+30+31+30+31)*24*60*60,
	(31+29+31+30+31+30+31+31)*24*60*60,
	(31+29+31+30+31+30+31+31+30)*24*60*60,
	(31+29+31+30+31+30+31+31+30+31)*24*60*60,
	(31+29+31+30+31+30+31+31+30+31+30)*24*60*60,
	(31+29+31+30+31+30+31+31+30+31+30+31)*24*60*60
};

struct TzSpan
{
	time_t utcStart;
	time_t utcEnd;
//	UINT32 localStart;
//	UINT32 localEnd;
	INT32 offset;
	int isDst;
};

struct TzSpanTable
{
	const TzSpan *table;
	size_t size;
};

//perl-unix = 62135683200 secs = 719163 days
//	utcStart	utcEnd	localStart	localEnd	offset		isDst		shortName
#include "tz_data.h"
#include "tz_data2.h" // PYR-22234

static const TzSpanTable tzSpans[] =
{
	{ tzSpan_UTC, sizeof(tzSpan_UTC) / sizeof(*tzSpan_UTC) },
	{ tzSpan_ET, sizeof(tzSpan_ET) / sizeof(*tzSpan_ET) },
	{ tzSpan_CET, sizeof(tzSpan_CET) / sizeof(*tzSpan_CET) },
	{ tzSpan_CST, sizeof(tzSpan_CST) / sizeof(*tzSpan_CST) },
	{ tzSpan_PT, sizeof(tzSpan_PT) / sizeof(*tzSpan_PT) },
	{ tzSpan_MT, sizeof(tzSpan_MT) / sizeof(*tzSpan_MT) },
	{ tzSpan_CT, sizeof(tzSpan_CT) / sizeof(*tzSpan_CT) },
	{ tzSpan_AT, sizeof(tzSpan_AT) / sizeof(*tzSpan_AT) },
	{ tzSpan_BRT, sizeof(tzSpan_BRT) / sizeof(*tzSpan_BRT) },
	{ tzSpan_EET, sizeof(tzSpan_EET) / sizeof(*tzSpan_EET) },
	{ tzSpan_WET, sizeof(tzSpan_WET) / sizeof(*tzSpan_WET) },
	{ tzSpan_MSK, sizeof(tzSpan_MSK) / sizeof(*tzSpan_MSK) },
	{ tzSpan_WST, sizeof(tzSpan_WST) / sizeof(*tzSpan_WST) },
	{ tzSpan_EST, sizeof(tzSpan_EST) / sizeof(*tzSpan_EST) },
	{ tzSpan_NZT, sizeof(tzSpan_NZT) / sizeof(*tzSpan_NZT) },
	{ tzSpan_CCT, sizeof(tzSpan_CCT) / sizeof(*tzSpan_CCT) },
	{ tzSpan_JST, sizeof(tzSpan_JST) / sizeof(*tzSpan_JST) },
	{ tzSpan_ACST, sizeof(tzSpan_ACST) / sizeof(*tzSpan_ACST) },
	{ tzSpan_ART, sizeof(tzSpan_ART) / sizeof(*tzSpan_ART) },
	{ tzSpan_AKT, sizeof(tzSpan_AKT) / sizeof(*tzSpan_AKT) },
	{ tzSpan_HST, sizeof(tzSpan_HST) / sizeof(*tzSpan_HST) },
	{ tzSpan_NT, sizeof(tzSpan_NT) / sizeof(*tzSpan_NT) },
	{ tzSpan_IST, sizeof(tzSpan_IST) / sizeof(*tzSpan_IST) },

    // PYR-22234: start of customized timezones
    { tzSpan_CustM1200, sizeof(tzSpan_CustM1200) / sizeof(*tzSpan_CustM1200) },
    { tzSpan_CustM1145, sizeof(tzSpan_CustM1145) / sizeof(*tzSpan_CustM1145) },
    { tzSpan_CustM1130, sizeof(tzSpan_CustM1130) / sizeof(*tzSpan_CustM1130) },
    { tzSpan_CustM1115, sizeof(tzSpan_CustM1115) / sizeof(*tzSpan_CustM1115) },
    { tzSpan_CustM1100, sizeof(tzSpan_CustM1100) / sizeof(*tzSpan_CustM1100) },
    { tzSpan_CustM1045, sizeof(tzSpan_CustM1045) / sizeof(*tzSpan_CustM1045) },
    { tzSpan_CustM1030, sizeof(tzSpan_CustM1030) / sizeof(*tzSpan_CustM1030) },
    { tzSpan_CustM1015, sizeof(tzSpan_CustM1015) / sizeof(*tzSpan_CustM1015) },
    { tzSpan_CustM1000, sizeof(tzSpan_CustM1000) / sizeof(*tzSpan_CustM1000) },
    { tzSpan_CustM0945, sizeof(tzSpan_CustM0945) / sizeof(*tzSpan_CustM0945) },
    { tzSpan_CustM0930, sizeof(tzSpan_CustM0930) / sizeof(*tzSpan_CustM0930) },
    { tzSpan_CustM0915, sizeof(tzSpan_CustM0915) / sizeof(*tzSpan_CustM0915) },
    { tzSpan_CustM0900, sizeof(tzSpan_CustM0900) / sizeof(*tzSpan_CustM0900) },
    { tzSpan_CustM0845, sizeof(tzSpan_CustM0845) / sizeof(*tzSpan_CustM0845) },
    { tzSpan_CustM0830, sizeof(tzSpan_CustM0830) / sizeof(*tzSpan_CustM0830) },
    { tzSpan_CustM0815, sizeof(tzSpan_CustM0815) / sizeof(*tzSpan_CustM0815) },
    { tzSpan_CustM0800, sizeof(tzSpan_CustM0800) / sizeof(*tzSpan_CustM0800) },
    { tzSpan_CustM0745, sizeof(tzSpan_CustM0745) / sizeof(*tzSpan_CustM0745) },
    { tzSpan_CustM0730, sizeof(tzSpan_CustM0730) / sizeof(*tzSpan_CustM0730) },
    { tzSpan_CustM0715, sizeof(tzSpan_CustM0715) / sizeof(*tzSpan_CustM0715) },
    { tzSpan_CustM0700, sizeof(tzSpan_CustM0700) / sizeof(*tzSpan_CustM0700) },
    { tzSpan_CustM0645, sizeof(tzSpan_CustM0645) / sizeof(*tzSpan_CustM0645) },
    { tzSpan_CustM0630, sizeof(tzSpan_CustM0630) / sizeof(*tzSpan_CustM0630) },
    { tzSpan_CustM0615, sizeof(tzSpan_CustM0615) / sizeof(*tzSpan_CustM0615) },
    { tzSpan_CustM0600, sizeof(tzSpan_CustM0600) / sizeof(*tzSpan_CustM0600) },
    { tzSpan_CustM0545, sizeof(tzSpan_CustM0545) / sizeof(*tzSpan_CustM0545) },
    { tzSpan_CustM0530, sizeof(tzSpan_CustM0530) / sizeof(*tzSpan_CustM0530) },
    { tzSpan_CustM0515, sizeof(tzSpan_CustM0515) / sizeof(*tzSpan_CustM0515) },
    { tzSpan_CustM0500, sizeof(tzSpan_CustM0500) / sizeof(*tzSpan_CustM0500) },
    { tzSpan_CustM0445, sizeof(tzSpan_CustM0445) / sizeof(*tzSpan_CustM0445) },
    { tzSpan_CustM0430, sizeof(tzSpan_CustM0430) / sizeof(*tzSpan_CustM0430) },
    { tzSpan_CustM0415, sizeof(tzSpan_CustM0415) / sizeof(*tzSpan_CustM0415) },
    { tzSpan_CustM0400, sizeof(tzSpan_CustM0400) / sizeof(*tzSpan_CustM0400) },
    { tzSpan_CustM0345, sizeof(tzSpan_CustM0345) / sizeof(*tzSpan_CustM0345) },
    { tzSpan_CustM0330, sizeof(tzSpan_CustM0330) / sizeof(*tzSpan_CustM0330) },
    { tzSpan_CustM0315, sizeof(tzSpan_CustM0315) / sizeof(*tzSpan_CustM0315) },
    { tzSpan_CustM0300, sizeof(tzSpan_CustM0300) / sizeof(*tzSpan_CustM0300) },
    { tzSpan_CustM0245, sizeof(tzSpan_CustM0245) / sizeof(*tzSpan_CustM0245) },
    { tzSpan_CustM0230, sizeof(tzSpan_CustM0230) / sizeof(*tzSpan_CustM0230) },
    { tzSpan_CustM0215, sizeof(tzSpan_CustM0215) / sizeof(*tzSpan_CustM0215) },
    { tzSpan_CustM0200, sizeof(tzSpan_CustM0200) / sizeof(*tzSpan_CustM0200) },
    { tzSpan_CustM0145, sizeof(tzSpan_CustM0145) / sizeof(*tzSpan_CustM0145) },
    { tzSpan_CustM0130, sizeof(tzSpan_CustM0130) / sizeof(*tzSpan_CustM0130) },
    { tzSpan_CustM0115, sizeof(tzSpan_CustM0115) / sizeof(*tzSpan_CustM0115) },
    { tzSpan_CustM0100, sizeof(tzSpan_CustM0100) / sizeof(*tzSpan_CustM0100) },
    { tzSpan_CustM0045, sizeof(tzSpan_CustM0045) / sizeof(*tzSpan_CustM0045) },
    { tzSpan_CustM0030, sizeof(tzSpan_CustM0030) / sizeof(*tzSpan_CustM0030) },
    { tzSpan_CustM0015, sizeof(tzSpan_CustM0015) / sizeof(*tzSpan_CustM0015) },
    { tzSpan_CustP0000, sizeof(tzSpan_CustP0000) / sizeof(*tzSpan_CustP0000) },
    { tzSpan_CustP0015, sizeof(tzSpan_CustP0015) / sizeof(*tzSpan_CustP0015) },
    { tzSpan_CustP0030, sizeof(tzSpan_CustP0030) / sizeof(*tzSpan_CustP0030) },
    { tzSpan_CustP0045, sizeof(tzSpan_CustP0045) / sizeof(*tzSpan_CustP0045) },
    { tzSpan_CustP0100, sizeof(tzSpan_CustP0100) / sizeof(*tzSpan_CustP0100) },
    { tzSpan_CustP0115, sizeof(tzSpan_CustP0115) / sizeof(*tzSpan_CustP0115) },
    { tzSpan_CustP0130, sizeof(tzSpan_CustP0130) / sizeof(*tzSpan_CustP0130) },
    { tzSpan_CustP0145, sizeof(tzSpan_CustP0145) / sizeof(*tzSpan_CustP0145) },
    { tzSpan_CustP0200, sizeof(tzSpan_CustP0200) / sizeof(*tzSpan_CustP0200) },
    { tzSpan_CustP0215, sizeof(tzSpan_CustP0215) / sizeof(*tzSpan_CustP0215) },
    { tzSpan_CustP0230, sizeof(tzSpan_CustP0230) / sizeof(*tzSpan_CustP0230) },
    { tzSpan_CustP0245, sizeof(tzSpan_CustP0245) / sizeof(*tzSpan_CustP0245) },
    { tzSpan_CustP0300, sizeof(tzSpan_CustP0300) / sizeof(*tzSpan_CustP0300) },
    { tzSpan_CustP0315, sizeof(tzSpan_CustP0315) / sizeof(*tzSpan_CustP0315) },
    { tzSpan_CustP0330, sizeof(tzSpan_CustP0330) / sizeof(*tzSpan_CustP0330) },
    { tzSpan_CustP0345, sizeof(tzSpan_CustP0345) / sizeof(*tzSpan_CustP0345) },
    { tzSpan_CustP0400, sizeof(tzSpan_CustP0400) / sizeof(*tzSpan_CustP0400) },
    { tzSpan_CustP0415, sizeof(tzSpan_CustP0415) / sizeof(*tzSpan_CustP0415) },
    { tzSpan_CustP0430, sizeof(tzSpan_CustP0430) / sizeof(*tzSpan_CustP0430) },
    { tzSpan_CustP0445, sizeof(tzSpan_CustP0445) / sizeof(*tzSpan_CustP0445) },
    { tzSpan_CustP0500, sizeof(tzSpan_CustP0500) / sizeof(*tzSpan_CustP0500) },
    { tzSpan_CustP0515, sizeof(tzSpan_CustP0515) / sizeof(*tzSpan_CustP0515) },
    { tzSpan_CustP0530, sizeof(tzSpan_CustP0530) / sizeof(*tzSpan_CustP0530) },
    { tzSpan_CustP0545, sizeof(tzSpan_CustP0545) / sizeof(*tzSpan_CustP0545) },
    { tzSpan_CustP0600, sizeof(tzSpan_CustP0600) / sizeof(*tzSpan_CustP0600) },
    { tzSpan_CustP0615, sizeof(tzSpan_CustP0615) / sizeof(*tzSpan_CustP0615) },
    { tzSpan_CustP0630, sizeof(tzSpan_CustP0630) / sizeof(*tzSpan_CustP0630) },
    { tzSpan_CustP0645, sizeof(tzSpan_CustP0645) / sizeof(*tzSpan_CustP0645) },
    { tzSpan_CustP0700, sizeof(tzSpan_CustP0700) / sizeof(*tzSpan_CustP0700) },
    { tzSpan_CustP0715, sizeof(tzSpan_CustP0715) / sizeof(*tzSpan_CustP0715) },
    { tzSpan_CustP0730, sizeof(tzSpan_CustP0730) / sizeof(*tzSpan_CustP0730) },
    { tzSpan_CustP0745, sizeof(tzSpan_CustP0745) / sizeof(*tzSpan_CustP0745) },
    { tzSpan_CustP0800, sizeof(tzSpan_CustP0800) / sizeof(*tzSpan_CustP0800) },
    { tzSpan_CustP0815, sizeof(tzSpan_CustP0815) / sizeof(*tzSpan_CustP0815) },
    { tzSpan_CustP0830, sizeof(tzSpan_CustP0830) / sizeof(*tzSpan_CustP0830) },
    { tzSpan_CustP0845, sizeof(tzSpan_CustP0845) / sizeof(*tzSpan_CustP0845) },
    { tzSpan_CustP0900, sizeof(tzSpan_CustP0900) / sizeof(*tzSpan_CustP0900) },
    { tzSpan_CustP0915, sizeof(tzSpan_CustP0915) / sizeof(*tzSpan_CustP0915) },
    { tzSpan_CustP0930, sizeof(tzSpan_CustP0930) / sizeof(*tzSpan_CustP0930) },
    { tzSpan_CustP0945, sizeof(tzSpan_CustP0945) / sizeof(*tzSpan_CustP0945) },
    { tzSpan_CustP1000, sizeof(tzSpan_CustP1000) / sizeof(*tzSpan_CustP1000) },
    { tzSpan_CustP1015, sizeof(tzSpan_CustP1015) / sizeof(*tzSpan_CustP1015) },
    { tzSpan_CustP1030, sizeof(tzSpan_CustP1030) / sizeof(*tzSpan_CustP1030) },
    { tzSpan_CustP1045, sizeof(tzSpan_CustP1045) / sizeof(*tzSpan_CustP1045) },
    { tzSpan_CustP1100, sizeof(tzSpan_CustP1100) / sizeof(*tzSpan_CustP1100) },
    { tzSpan_CustP1115, sizeof(tzSpan_CustP1115) / sizeof(*tzSpan_CustP1115) },
    { tzSpan_CustP1130, sizeof(tzSpan_CustP1130) / sizeof(*tzSpan_CustP1130) },
    { tzSpan_CustP1145, sizeof(tzSpan_CustP1145) / sizeof(*tzSpan_CustP1145) },
    { tzSpan_CustP1200, sizeof(tzSpan_CustP1200) / sizeof(*tzSpan_CustP1200) },
    { tzSpan_CustP1215, sizeof(tzSpan_CustP1215) / sizeof(*tzSpan_CustP1215) },
    { tzSpan_CustP1230, sizeof(tzSpan_CustP1230) / sizeof(*tzSpan_CustP1230) },
    { tzSpan_CustP1245, sizeof(tzSpan_CustP1245) / sizeof(*tzSpan_CustP1245) },
    { tzSpan_CustP1300, sizeof(tzSpan_CustP1300) / sizeof(*tzSpan_CustP1300) },
    { tzSpan_CustP1315, sizeof(tzSpan_CustP1315) / sizeof(*tzSpan_CustP1315) },
    { tzSpan_CustP1330, sizeof(tzSpan_CustP1330) / sizeof(*tzSpan_CustP1330) },
    { tzSpan_CustP1345, sizeof(tzSpan_CustP1345) / sizeof(*tzSpan_CustP1345) },
    { tzSpan_CustP1400, sizeof(tzSpan_CustP1400) / sizeof(*tzSpan_CustP1400) },
    // end of customized timezones

};

static int _tzFindLocalSpan(UINT32 localTime, eTimeZone tz, int isDst)
{
	for(int tzInd = 0; tzInd < tzSpans[tz].size; ++tzInd)
	{
		if(localTime < tzSpans[tz].table[tzInd].utcEnd + tzSpans[tz].table[tzInd].offset)
		{
			if(localTime < tzSpans[tz].table[tzInd].utcStart + tzSpans[tz].table[tzInd].offset)
			{
				PTRACE3("_tzFindLocalSpan: invalid localStart, time %u, zone %u, dst %d, ind %d", localTime, tz, isDst, tzInd);
				return tzInd;
			}

			if(tzInd < tzSpans[tz].size - 1 &&
				localTime >= tzSpans[tz].table[tzInd+1].utcStart + tzSpans[tz].table[tzInd+1].offset &&		//ambiguous time
				isDst >= 0 &&
				isDst != tzSpans[tz].table[tzInd].isDst)		//first of repeated hours by default
			{
				return (tzInd+1);
			}
			return tzInd;
		}
	}
	//shouldn't happen - invalid timezone spans table
	PTRACE5("_tzFindLocalSpan: invalid span, time %u, zone %u, dst %d", localTime, tz, isDst);
	return 0;
}

static int _tzFindUtcSpan(time_t utcTime, eTimeZone tz)
{
	for(int tzInd = 0; tzInd < tzSpans[tz].size; ++tzInd)
	{
		if(utcTime < tzSpans[tz].table[tzInd].utcEnd)
		{
			if(utcTime < tzSpans[tz].table[tzInd].utcStart)
			{
				PTRACE5("_tzFindUtcSpan: invalid utcStart, time %u, zone %u, ind %d", (UINT32)utcTime, tz, tzInd);
				return tzInd;
			}

			return tzInd;
		}
	}
	//shouldn't happen - invalid timezone spans table
	PTRACE5("_tzFindUtcSpan: invalid span, time %u, zone %u", (UINT32)utcTime, tz);
	return 0;
}

//returns UTC
time_t tzSrvTimeToUtc(const SrvTime& srvTime, eTimeZone tz, int isDst)
{
	if(srvTime._year <= TZ_START_YEAR || srvTime._year >= TZ_END_YEAR)
	{//out of range (unsupported time) - return unconverted
		PTRACE5("tzSrvTimeToUtc: unsupported year %u - return unconverted time", srvTime._year);
		return srvTime.encode();
	}

	if(!isValidTimezone(tz))
	{//unsupported timezone - assume ET
		PTRACE5("tzSrvTimeToUtc: unsupported timezone %u - assume ET", tz);
		tz = eTzET;
	}

	UINT32 yearFromStart = srvTime._year - TZ_START_YEAR;
	//946684800 = 2000/01/01-00:00:00
	UINT32 secs = yearsS[yearFromStart];
	secs += ((srvTime._year % 4) == 0 ?		//works till 2100, change then
			monthLeapS[srvTime._month-1] :
			monthS[srvTime._month-1]);
	secs += (srvTime._day-1) * 24*60*60 +
			srvTime._hour * 60*60 +
			srvTime._minute * 60 +
			srvTime._second;

	UINT32 tzInd = _tzFindLocalSpan(secs, tz, isDst);
	return (time_t)(secs - tzSpans[tz].table[tzInd].offset);
}

//returns isDst
int tzUtcToLocalSrvTime(time_t utcTime, SrvTime& localSrv, eTimeZone tz, bool* p_isAmb)
{
	if(utcTime < yearsS[1] || yearsS[TZ_END_YEAR-TZ_START_YEAR-1] < utcTime)
	{//out of range (unsupported time) - return unconverted
		PTRACE5("tzUtcToLocalSrvTime: unsupported time %u - return unconverted time", (UINT32)utcTime);
		localSrv.decode(utcTime);
		if(p_isAmb)
			*p_isAmb = false;
		return 0;
	}

	if(!isValidTimezone(tz))
	{//unsupported timezone - assume ET
		PTRACE5("tzUtcToLocalSrvTime: unsupported timezone %u - assume ET", tz);
		tz = eTzET;
	}

	UINT32 tzInd = _tzFindUtcSpan(utcTime, tz);
	UINT32 secs = (UINT32)utcTime + tzSpans[tz].table[tzInd].offset;
	if(p_isAmb)
		*p_isAmb = ((tzInd > 0) ? (secs < tzSpans[tz].table[tzInd-1].utcEnd + tzSpans[tz].table[tzInd-1].offset) : false);	//second of repeated hours
	localSrv._wday = (secs / (24*60*60) + 4) % 7; //-V112
	UINT32 year;
	for(year = 1; year < TZ_END_YEAR-TZ_START_YEAR; ++year)
	{
		if(secs < yearsS[year])
		{
			localSrv._year = year + TZ_START_YEAR-1;
			secs -= yearsS[year-1];
			break;
		}
	}
	if((localSrv._year % 4) == 0)	//works till 2100, change then
	{
		for(UINT32 month = 1; month <= 12; ++month)
		{
			if(secs < monthLeapS[month])
			{
				localSrv._month = month;
				secs -= monthLeapS[month-1];
				break;
			}
		}
	}
	else
	{
		for(UINT32 month = 1; month <= 12; ++month)
		{
			if(secs < monthS[month])
			{
				localSrv._month = month;
				secs -= monthS[month-1];
				break;
			}
		}
	}
	localSrv._day = secs / (24*60*60) + 1;
	secs %= (24*60*60);
	localSrv._hour = secs / (60*60);
	secs %= (60*60);
	localSrv._minute = secs / 60;
	localSrv._second = secs % 60;
	return tzSpans[tz].table[tzInd].isDst;
}

//returns isDst
int tzLocalToLocalSrvTime(const SrvTime& srvSrc, eTimeZone tzSrc, int isDstSrc, SrvTime& srvDest, eTimeZone tzDest, bool* p_isAmb)
{
	if(srvSrc._year <= TZ_START_YEAR || srvSrc._year >= TZ_END_YEAR)
	{//out of range (unsupported time) - return unconverted
		PTRACE5("tzLocalToLocalSrvTime: unsupported year %u - return unconverted time", srvSrc._year);
		srvDest = srvSrc;
		if(p_isAmb)
			*p_isAmb = false;
		return 0;
	}

	if(!isValidTimezone(tzSrc))
	{//unsupported timezone - assume ET
		PTRACE5("tzLocalToLocalSrvTime: unsupported timezone %u - assume ET", tzSrc);
		tzSrc = eTzET;
	}

	if(!isValidTimezone(tzDest))
	{//unsupported timezone - assume ET
		PTRACE5("tzLocalToLocalSrvTime: unsupported timezone %u - assume ET", tzDest);
		tzDest = eTzET;
	}

	time_t utcTime = tzSrvTimeToUtc(srvSrc, tzSrc, isDstSrc);
	return tzUtcToLocalSrvTime(utcTime, srvDest, tzDest, p_isAmb);
}

bool isValidTimezone(UINT32 tz)
{
	return((tz&0xFFFF) < eTzNumber);
}

eTimeZone getTimezone(UINT32 tzf)
{
	return((eTimeZone)(tzf & 0xFFFF));
}

bool getTimeFormat(UINT32 tzf)
{
	return((tzf >> 16) > 0);
}

UINT32 makeTimezoneFormat(eTimeZone tz, UINT16 tf)
{
	return((UINT32)tz | (tf << 16));
}

UINT32 makeTimezoneFormatEx(eTimeZone tz, bool tf)
{
	return((UINT32)tz | (tf ? 0x10000 : 0));
}

bool nonCustomTzStringToEnum(const char* tzStr, eTimeZone& tz)
{
	tz = eTzET;

	if ( !tzStr || !*tzStr )
		return false;

	if ( !PString::compareIgnoreCase(tzStr,  "UTC", 3) ) { tz = eTzUTC ; return true; }
	if ( !PString::compareIgnoreCase(tzStr,   "ET", 2) ) { tz = eTzET  ; return true; }
	if ( !PString::compareIgnoreCase(tzStr,  "CET", 3) ) { tz = eTzCET ; return true; }
	if ( !PString::compareIgnoreCase(tzStr,  "CST", 3) ) { tz = eTzCST ; return true; }
	if ( !PString::compareIgnoreCase(tzStr,   "PT", 2) ) { tz = eTzPT  ; return true; }
	if ( !PString::compareIgnoreCase(tzStr,   "MT", 2) ) { tz = eTzMT  ; return true; }
	if ( !PString::compareIgnoreCase(tzStr,   "CT", 2) ) { tz = eTzCT  ; return true; }
	if ( !PString::compareIgnoreCase(tzStr,   "AT", 2) ) { tz = eTzAT  ; return true; }
	if ( !PString::compareIgnoreCase(tzStr,  "BRT", 3) ) { tz = eTzBRT ; return true; }
	if ( !PString::compareIgnoreCase(tzStr,  "EET", 3) ) { tz = eTzEET ; return true; }
	if ( !PString::compareIgnoreCase(tzStr,  "WET", 3) ) { tz = eTzWET ; return true; }
	if ( !PString::compareIgnoreCase(tzStr,  "MSK", 3) ) { tz = eTzMSK ; return true; }
	if ( !PString::compareIgnoreCase(tzStr,  "WST", 3) ) { tz = eTzWST ; return true; }
	if ( !PString::compareIgnoreCase(tzStr,  "EST", 3) ) { tz = eTzEST ; return true; }
	if ( !PString::compareIgnoreCase(tzStr,  "NZT", 3) ) { tz = eTzNZT ; return true; }
	if ( !PString::compareIgnoreCase(tzStr,  "CCT", 3) ) { tz = eTzCCT ; return true; }
	if ( !PString::compareIgnoreCase(tzStr,  "JST", 3) ) { tz = eTzJST ; return true; }
	if ( !PString::compareIgnoreCase(tzStr, "ACST", 4) ) { tz = eTzACST; return true; }
	if ( !PString::compareIgnoreCase(tzStr,  "ART", 3) ) { tz = eTzART ; return true; }
	if ( !PString::compareIgnoreCase(tzStr,  "AKT", 3) ) { tz = eTzAKT ; return true; }
	if ( !PString::compareIgnoreCase(tzStr,  "HST", 3) ) { tz = eTzHST ; return true; }
	if ( !PString::compareIgnoreCase(tzStr,   "NT", 2) ) { tz = eTzNT  ; return true; }
	if ( !PString::compareIgnoreCase(tzStr,  "IST", 3) ) { tz = eTzIST ; return true; }
	
	return false;
}

// begin of customized timezones (PYR-22234)
const UINT32 tzCustStart    =  eTzCustM1200;
const UINT32 tzCustEnd      =  eTzCustP1400;
const int tzCustStartHour   =  -12;
const int tzCustEndHour     =  +14;
const int tzCustIntervalMin =  15;

bool isValidStdTimezone(UINT32 tz)
{
    eTimeZone etz = getTimezone(tz);
    return (etz < tzCustStart);
}

bool isValidCustTimezone(UINT32 tz)
{
    eTimeZone etz = getTimezone(tz);
    return (etz >= tzCustStart && etz <= tzCustEnd);
}

bool isValidUtcOffsetMin(int minute)
{
    return (minute>=tzCustStartHour*60 && minute<=tzCustEndHour*60 && abs(minute)%tzCustIntervalMin==0);
}

// return false if input is not a valid offset of customized timezone
bool tzUtcOffsetToCustTimezone(int offsetmin, eTimeZone& tz)
{
    tz = eTzUTC;
    if (!isValidUtcOffsetMin(offsetmin))
        return false;
    int interval = (offsetmin - tzCustStartHour*60) / tzCustIntervalMin;
    tz = (eTimeZone)(tzCustStart + interval); //lint !e838 (Previously assigned value to variable 'tz' has not been used)
    return true;
}

// return false if input is not a valid customized timezone
bool tzCustTimezoneToUtcOffset(eTimeZone tz, int& offsetmin)
{
    offsetmin = 0;
    if (!isValidCustTimezone(tz))
        return false;
    offsetmin = (tz - tzCustStart) * tzCustIntervalMin + tzCustStartHour * 60;     //lint !e838 (Previously assigned value to variable 'offsetmin' has not been used)
    return true;
}
// end of customized timezones (PYR-22234)