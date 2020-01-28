#ifndef timezone_h_included
#define timezone_h_included

#include "pyrtime.h"

//2000-2030
#define TZ_START_YEAR	(2000)
#define TZ_END_YEAR		(2030)

enum eTimeZone
{
	eTzUTC = 0,		//Universal Coordinated Time
	eTzET = 1,		//Eastern Time
	eTzCET = 2,		//Central European Time
	eTzCST = 3,		//Costa Rica Standard Time
	eTzPT = 4,		//Pacific Time
	eTzMT = 5,		//Mountain Time
	eTzCT = 6,		//Central Time
	eTzAT = 7,		//Atlantic Time
	eTzBRT = 8,		//Brasilia Time
	eTzEET = 9,		//East European Time
	eTzWET = 10,	//Western European Time
	eTzMSK = 11,	//Moscow Standard Time
	eTzWST = 12,	//Australian Western Standard Time
	eTzEST = 13,	//Australian Eastern Standard Time
	eTzNZT = 14,	//New Zealand Time
	eTzCCT = 15,	//China Coast Time
	eTzJST = 16,	//Japan Standard Time
	eTzACST = 17,	//Australian Central Standard Time
	eTzART = 18,	//Argentina Time
	eTzAKT = 19,	//Alaska Time
	eTzHST = 20,	//Hawaii Standard Time
	eTzNT = 21,		//Newfoundland Time
	eTzIST = 22,	//India Standard Time

    // PYR-22234: start of customized timezones with offset from UTC (offset 0 is ignored)
    // offset range [-12, +14] in quarterly steps, 105 timezones in total
    // customized timezone values start at 23
	eTzCUSTOM_start = 23,
    eTzCustM1200 = eTzCUSTOM_start,
    eTzCustM1145 = 24,
    eTzCustM1130 = 25,
    eTzCustM1115 = 26,
    eTzCustM1100 = 27,
    eTzCustM1045 = 28,
    eTzCustM1030 = 29,
    eTzCustM1015 = 30,
    eTzCustM1000 = 31,
    eTzCustM0945 = 32,
    eTzCustM0930 = 33,
    eTzCustM0915 = 34,
    eTzCustM0900 = 35,
    eTzCustM0845 = 36,
    eTzCustM0830 = 37,
    eTzCustM0815 = 38,
    eTzCustM0800 = 39,
    eTzCustM0745 = 40,
    eTzCustM0730 = 41,
    eTzCustM0715 = 42,
    eTzCustM0700 = 43,
    eTzCustM0645 = 44,
    eTzCustM0630 = 45,
    eTzCustM0615 = 46,
    eTzCustM0600 = 47,
    eTzCustM0545 = 48,
    eTzCustM0530 = 49,
    eTzCustM0515 = 50,
    eTzCustM0500 = 51,
    eTzCustM0445 = 52,
    eTzCustM0430 = 53,
    eTzCustM0415 = 54,
    eTzCustM0400 = 55,
    eTzCustM0345 = 56,
    eTzCustM0330 = 57,
    eTzCustM0315 = 58,
    eTzCustM0300 = 59,
    eTzCustM0245 = 60,
    eTzCustM0230 = 61,
    eTzCustM0215 = 62,
    eTzCustM0200 = 63,
    eTzCustM0145 = 64,
    eTzCustM0130 = 65,
    eTzCustM0115 = 66,
    eTzCustM0100 = 67,
    eTzCustM0045 = 68,
    eTzCustM0030 = 69,
    eTzCustM0015 = 70,
    eTzCustP0000 = 71,
    eTzCustP0015 = 72,
    eTzCustP0030 = 73,
    eTzCustP0045 = 74,
    eTzCustP0100 = 75,
    eTzCustP0115 = 76,
    eTzCustP0130 = 77,
    eTzCustP0145 = 78,
    eTzCustP0200 = 79,
    eTzCustP0215 = 80,
    eTzCustP0230 = 81,
    eTzCustP0245 = 82,
    eTzCustP0300 = 83,
    eTzCustP0315 = 84,
    eTzCustP0330 = 85,
    eTzCustP0345 = 86,
    eTzCustP0400 = 87,
    eTzCustP0415 = 88,
    eTzCustP0430 = 89,
    eTzCustP0445 = 90,
    eTzCustP0500 = 91,
    eTzCustP0515 = 92,
    eTzCustP0530 = 93,
    eTzCustP0545 = 94,
    eTzCustP0600 = 95,
    eTzCustP0615 = 96,
    eTzCustP0630 = 97,
    eTzCustP0645 = 98,
    eTzCustP0700 = 99,
    eTzCustP0715 = 100,
    eTzCustP0730 = 101,
    eTzCustP0745 = 102,
    eTzCustP0800 = 103,
    eTzCustP0815 = 104,
    eTzCustP0830 = 105,
    eTzCustP0845 = 106,
    eTzCustP0900 = 107,
    eTzCustP0915 = 108,
    eTzCustP0930 = 109,
    eTzCustP0945 = 110,
    eTzCustP1000 = 111,
    eTzCustP1015 = 112,
    eTzCustP1030 = 113,
    eTzCustP1045 = 114,
    eTzCustP1100 = 115,
    eTzCustP1115 = 116,
    eTzCustP1130 = 117,
    eTzCustP1145 = 118,
    eTzCustP1200 = 119,
    eTzCustP1215 = 120,
    eTzCustP1230 = 121,
    eTzCustP1245 = 122,
    eTzCustP1300 = 123,
    eTzCustP1315 = 124,
    eTzCustP1330 = 125,
    eTzCustP1345 = 126,
    eTzCustP1400 = 127,
	eTzCUSTOM_end = eTzCustP1400,
    // end of customized timezones with offset from UTC

// ^ Append here ^
	eTzNumber
};

time_t tzSrvTimeToUtc(const SrvTime& srvTime, eTimeZone tz, int isDst = -1);
int tzUtcToLocalSrvTime(time_t utcTime, SrvTime& localSrv, eTimeZone tz, bool* p_isAmb = NULL);
int tzLocalToLocalSrvTime(const SrvTime& srvSrc, eTimeZone tzSrc, int isDstSrc, SrvTime& srvDest, eTimeZone tzDest, bool* p_isAmb = NULL);
bool isValidTimezone(UINT32 tz);
eTimeZone getTimezone(UINT32 tzf);
bool getTimeFormat(UINT32 tzf);
UINT32 makeTimezoneFormat(eTimeZone tz, UINT16 tf);
UINT32 makeTimezoneFormatEx(eTimeZone tz, bool tf);
bool nonCustomTzStringToEnum(const char* tzStr, eTimeZone& tz);

// PYR-22234
bool isValidStdTimezone(UINT32 tz);
bool isValidCustTimezone(UINT32 tz);
bool isValidUtcOffsetMin(int min);
bool tzUtcOffsetToCustTimezone(int offsetmin, eTimeZone& tz);
bool tzCustTimezoneToUtcOffset(eTimeZone tz, int& offsetmin);

#endif
