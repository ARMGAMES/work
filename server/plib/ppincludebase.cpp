#include "ppincludebase.h"
#include "ppthread.h"
#include "ppi18n.h"//for PStringEncoding which is necessary for plib_fopen

#ifdef PPOSIX

void INITRECURSIVEMUTEX( pthread_mutex_t* mutex )
{
	pthread_mutexattr_t attr;
	pthread_mutexattr_init( &attr );
	pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE );
	pthread_mutex_init( mutex, &attr );
	pthread_mutexattr_destroy( &attr );
}

time_t TIMEMSEC( int* msec )
{
	struct timeval tv;
	gettimeofday( &tv, 0 );
	if( msec )
		*msec = tv.tv_usec / 1000;
	return tv.tv_sec;
}

UINT32 LONGSYSTEMTICKS(int * msec)
{
	return (UINT32) TIMEMSEC(msec);
}


UINT32 RANDOMTIME()
{
	struct timeval tv;
	gettimeofday( &tv, 0 );
	return (UINT32) tv.tv_usec;
}

#if defined(LINUX)
UINT32 SYSTEMTICKS()
{
	struct timespec tm;
    clock_gettime(CLOCK_MONOTONIC_COARSE,&tm);
	return (UINT32) (((UINT64) tm.tv_sec * 1000 ) +  tm.tv_nsec / 1000000);
}

#elif	defined(P_MAC10X) || defined(P_AIX64) || defined(P_AIX32) || defined(P_ANDROID)
UINT32 SYSTEMTICKS()
{
	struct tms dummy;
	return static_cast< UINT32 >( times( &dummy ) * 10 );
}
#else
UINT32 SYSTEMTICKS()
{
	struct tms dummy;
	return times( &dummy );
}
#endif

void PCurrentLocalTime( struct tm* tm )
{
	//MULTITHREAD: may require PCriticalSection to ensure reentrability!
	time_t t = time( 0 );
	struct tm* tt = localtime( &t );
	PASSERT3( tt );
	*tm = *tt;
	tm->tm_yday = 0;
	tm->tm_isdst = -1;
}

time_t plib_time(time_t * sec)
{
	return time(sec);
}

struct tm * plib_localtime(const time_t * sec)
{
	return localtime(sec);
}
bool plib_localtime_ext(time_t sec, struct tm * timetm )
{
	*timetm = *localtime(&sec);
	return true;
}
bool plib_localtime_ext_bias(time_t sec, struct tm * timetm, INT16 * bias  )
{
	*timetm = *localtime(&sec);
	return true;
}

time_t plib_mktime(struct tm * timetm)
{
	return mktime(timetm);
} 

time_t plib_mkUTCtime(struct tm * timetm)
{
	return mktime(timetm);
} 

bool plib_timetToStructTm(time_t sec, struct tm * timetm)
{
	*timetm = *localtime(&sec);
	return true;
}

#ifdef P_MAC10X
int TIMEZONE()
{
	struct tm t;
	PCurrentLocalTime(&t);
	return (int) t.tm_gmtoff;
}
#endif
#endif

#ifdef PWIN32

time_t TIMEMSEC( int* msec )
{
	struct _timeb t;
	_ftime( &t );
	if( msec )
		*msec = t.millitm;
	return t.time;
}

struct PTicksInitializer
{
	PCriticalSection cs;
	UINT32 high;
	UINT32 low;
	PTicksInitializer()
	{
		low=SYSTEMTICKS();
		high=0;
	}
};



UINT32 LONGSYSTEMTICKS(int * msec)
{
	static PTicksInitializer *pi = 0;
	if (!pi)
	{
		static PTicksInitializer i;
		pi = &i;
		UINT32 msecLow = pi->low;
		if (msec)
			*msec = msecLow%1000;
		return msecLow/1000;
	}
	else
	{
		PLock lock(pi->cs);
		UINT32 curr = SYSTEMTICKS();
		if (curr<pi->low)
		{
			pi->high++;
		}
		pi->low = curr;
		unsigned __int64 tt = ( (unsigned __int64)pi->high << 32 ) | (unsigned __int64)pi->low;
		if (msec)
			*msec = (UINT32) (tt%1000);
		return (UINT32) (tt/1000);
	}
} 



static void _PTimetToFileTime( time_t t, FILETIME* pft )
{
	__int64 ll = t * 10000000LL + 116444736000000000LL; // PYR-64533 - "t" can exceed int32 range so must use straight 64-bit arithmetic
	pft->dwLowDateTime = (UINT32) ll;
	pft->dwHighDateTime = (UINT32) (ll >> 32);
}

static time_t _PFileTimeToTimet( const FILETIME* pft )
{
	unsigned __int64 tt = ( (unsigned __int64)pft->dwHighDateTime << 32 ) | (unsigned __int64)pft->dwLowDateTime;
	return (time_t) ((tt - 116444736000000000)/10000000);
}

static TIME_ZONE_INFORMATION _plib_tmzone_cache={-1};

time_t plib_time(time_t * sec)
{
	FILETIME ft;
	GetSystemTimeAsFileTime(&ft);
	time_t t= _PFileTimeToTimet(&ft);
	if (sec)
		*sec=t;
	return t;
}

/***********************************************************************
*  _DayLightCompareDate
*
*  Compares two dates without looking at the year
*
* RETURNS
*
*  -1 if date < compareDate
*   0 if date == compareDate
*   1 if date > compareDate
*  -2 if an error occures
*/
static int _DayLightCompareDate(
								const LPSYSTEMTIME date,                       /* [in] The date to compare. */
								const LPSYSTEMTIME compareDate,
								const int standartDayLightBias,
								const int daylightDayLightBias)                /* [in] The daylight saving begin or end date */
{
	int limit_day;

	if (compareDate->wYear != 0)
	{
		if (date->wMonth < compareDate->wMonth)
			return -1; /* We are in a year before the date limit. */

		if (date->wMonth > compareDate->wMonth)
			return 1; /* We are in a year after the date limit. */
	}

	if (date->wMonth < compareDate->wMonth)
		return -1; /* We are in a month before the date limit. */

	if (date->wMonth > compareDate->wMonth)
		return 1; /* We are in a month after the date limit. */

	if (compareDate->wDayOfWeek <= 6)
	{
		SYSTEMTIME tmp;
		FILETIME tmp_ft;

		/* compareDate->wDay is interpreted as number of the week in the month. */
		/* 5 means: the last week in the month */
		int weekofmonth = compareDate->wDay;

		/* calculate day of week for the first day in the month */
		memcpy(&tmp, date, sizeof(SYSTEMTIME));
		tmp.wDay = 1;
		tmp.wDayOfWeek = static_cast< WORD >( -1 );

		if (weekofmonth == 5)
		{
			/* Go to the beginning of the next month. */
			if (++tmp.wMonth > 12)
			{
				tmp.wMonth = 1;
				++tmp.wYear;
			}
		}

		if (!SystemTimeToFileTime(&tmp, &tmp_ft))
			return -2;

		if (weekofmonth == 5)
		{
			__int64 t, one_day;

			t = tmp_ft.dwHighDateTime;
			t <<= 32;
			t += (UINT)tmp_ft.dwLowDateTime;

			/* substract one day */
			one_day = 24*60*60;
			one_day *= 10000000;
			t -= one_day;

			tmp_ft.dwLowDateTime  = (UINT32)t;
			tmp_ft.dwHighDateTime = (UINT32)(t >> 32);
		}

		if (!FileTimeToSystemTime(&tmp_ft, &tmp))
			return -2;

		if (weekofmonth == 5)
		{
			/* calculate the last matching day of the week in this month */
			int dif = tmp.wDayOfWeek - compareDate->wDayOfWeek;
			if (dif < 0)
				dif += 7;

			limit_day = tmp.wDay - dif;
		}
		else
		{
			/* calulcate the matching day of the week in the given week */
			int dif = compareDate->wDayOfWeek - tmp.wDayOfWeek;
			if (dif < 0)
				dif += 7;

			limit_day = tmp.wDay + 7*(weekofmonth-1) + dif;
		}
	}
	else
	{
		limit_day = compareDate->wDay;
	}

	if (date->wDay < limit_day)
		return -1;

	if (date->wDay > limit_day)
		return 1;
	/* date is equal to the date limit. */

	if ((date->wHour*60 + date->wMinute) >= (compareDate->wHour*60 + compareDate->wMinute - daylightDayLightBias))
		return 1;
	else if ((date->wHour*60 + date->wMinute) < (compareDate->wHour*60 + compareDate->wMinute + standartDayLightBias))
		return -1;

	return 0;   
}

static bool _getUTCTime(
						const LPSYSTEMTIME date,                       /* [in] The date to compare. */
						const LPSYSTEMTIME compareDate,
						LPFILETIME utcTime)
{

	SYSTEMTIME tmp;
	FILETIME tmp_ft;

	/* compareDate->wDay is interpreted as number of the week in the month. */
	/* 5 means: the last week in the month */
	int weekofmonth = compareDate->wDay;

	tmp.wYear = date->wYear;
	tmp.wDay = 1;
	tmp.wHour = compareDate->wHour;
	tmp.wMinute = compareDate->wMinute;
	tmp.wSecond = 0;
	tmp.wMilliseconds = 0;
	tmp.wMonth = compareDate->wMonth;
	tmp.wDayOfWeek = static_cast< WORD >( -1 );
	if (weekofmonth == 5)
	{
		/* Go to the beginning of the next month. */
		if (++tmp.wMonth > 12)
		{
			tmp.wMonth = 1;
			++tmp.wYear;
		}
	}
	if (!SystemTimeToFileTime(&tmp, &tmp_ft))
		return false;

	if (weekofmonth == 5)
	{
		__int64 t, one_day;

		t = tmp_ft.dwHighDateTime;
		t <<= 32;
		t += (UINT)tmp_ft.dwLowDateTime;

		/* substract one day */
		one_day = 24*60*60;
		one_day *= 10000000;
		t -= one_day;

		tmp_ft.dwLowDateTime  = (UINT32)t;
		tmp_ft.dwHighDateTime = (UINT32)(t >> 32);
	}
	if (!FileTimeToSystemTime(&tmp_ft, &tmp))
		return false;

	if (weekofmonth == 5)
	{
		/* calculate the last matching day of the week in this month */
		int dif = tmp.wDayOfWeek - compareDate->wDayOfWeek;
		if (dif < 0)
			dif += 7;

		tmp.wDay -= static_cast< WORD >( dif );
	}
	else
	{
		/* calulcate the matching day of the week in the given week */
		int dif = compareDate->wDayOfWeek - tmp.wDayOfWeek;
		if (dif < 0)
			dif += 7;

		tmp.wDay += static_cast< WORD >( 7*(weekofmonth-1) + dif );
	}

	if (!SystemTimeToFileTime(&tmp, utcTime))
		return false;
	return true;   

}

/***********************************************************************
*  _GetTimezoneBias
*
*  Calculates the local time bias for a given time zone
*
* RETURNS
*
*  Returns TRUE when the time zone bias was calculated.
*/

static bool _GetTimezoneBias(
							 const LPTIME_ZONE_INFORMATION lpTimeZoneInformation, /* [in] The time zone data.            */
							 LPSYSTEMTIME                  lpSystemTime,          /* [in] The system time.               */
							 INT32*                         pBias)                 /* [out] The calulated bias in minutes */
{
	int ret;
	bool beforeStandardDate, afterDaylightDate;

	bool daylightsaving = false;
	INT32 bias = lpTimeZoneInformation->Bias;
	if (lpTimeZoneInformation->StandardDate.wMonth != 0 && lpTimeZoneInformation->DaylightDate.wMonth != 0 && 
		(lpTimeZoneInformation->DaylightBias != 0 || lpTimeZoneInformation->StandardBias != 0))
	{
		if (lpTimeZoneInformation->StandardDate.wMonth == 0 ||
			lpTimeZoneInformation->StandardDate.wDay<1 ||
			lpTimeZoneInformation->StandardDate.wDay>5 ||
			lpTimeZoneInformation->DaylightDate.wDay<1 ||
			lpTimeZoneInformation->DaylightDate.wDay>5)
		{
			SetLastError(ERROR_INVALID_PARAMETER);
			return false;
		}

		bool north = lpTimeZoneInformation->StandardDate.wMonth>lpTimeZoneInformation->DaylightDate.wMonth;
		/* check for daylight saving */
		ret = _DayLightCompareDate(lpSystemTime, &lpTimeZoneInformation->StandardDate,lpTimeZoneInformation->DaylightBias,0);
		if (ret == -2)
			return false;

		if (ret==0)
		{
			// Special case - local time in the period of changing time
			// use current bias
			FILETIME ft;
			FILETIME lft;
			GetSystemTimeAsFileTime(&ft);
			FileTimeToLocalFileTime(&ft,&lft);
			*pBias = (INT32)( (_PFileTimeToTimet(&ft)-_PFileTimeToTimet(&lft))/60 );
			return true;
		}

		beforeStandardDate = ret < 0;
		ret = _DayLightCompareDate(lpSystemTime, &lpTimeZoneInformation->DaylightDate,0,lpTimeZoneInformation->DaylightBias);
		if (ret == -2)
			return false;
		if (ret==0)
		{
			// special case - there is no such time
			return false;
		}

		afterDaylightDate = ret >= 0;

		if (north)
		{
			if (beforeStandardDate && afterDaylightDate)
				daylightsaving = true;
		}
		else
		{
			if (beforeStandardDate || afterDaylightDate)
				daylightsaving = true;
		}
	}

	if (daylightsaving)
		bias += lpTimeZoneInformation->DaylightBias;
	else if (lpTimeZoneInformation->StandardDate.wMonth != 0)
		bias += lpTimeZoneInformation->StandardBias;

	*pBias = bias;

	return true;
}

static bool _GetUTCTimezoneBias(
								const LPTIME_ZONE_INFORMATION lpTimeZoneInformation, /* [in] The time zone data.            */
								LPFILETIME                    lpUniversalTime,          /* [in] The system time.               */
								INT32*                        pBias)                 /* [out] The calulated bias in minutes */
{

	bool daylightsaving = false;

	INT32 bias = lpTimeZoneInformation->Bias;
	if (lpTimeZoneInformation->StandardDate.wMonth != 0 && lpTimeZoneInformation->DaylightDate.wMonth != 0 && 
		(lpTimeZoneInformation->DaylightBias != 0 || lpTimeZoneInformation->StandardBias != 0))
	{
		if (lpTimeZoneInformation->StandardDate.wMonth == 0 ||
			lpTimeZoneInformation->StandardDate.wDay<1 ||
			lpTimeZoneInformation->StandardDate.wDay>5 ||
			lpTimeZoneInformation->DaylightDate.wDay<1 ||
			lpTimeZoneInformation->DaylightDate.wDay>5)
		{
			SetLastError(ERROR_INVALID_PARAMETER);
			return false;
		}

		bool north = lpTimeZoneInformation->StandardDate.wMonth>lpTimeZoneInformation->DaylightDate.wMonth;
		/* check for daylight saving */

		SYSTEMTIME systemTime;
		if (!FileTimeToSystemTime(lpUniversalTime,&systemTime))
			return false;
		FILETIME ft_standart;
		if ( !_getUTCTime(&systemTime, &lpTimeZoneInformation->StandardDate,&ft_standart))
			return false;
		FILETIME ft_daylight;
		if ( !_getUTCTime(&systemTime, &lpTimeZoneInformation->DaylightDate,&ft_daylight))
			return false;


		__int64 curr,standart,daylight;

		curr = lpUniversalTime->dwHighDateTime;
		curr <<= 32;
		curr += (UINT32)lpUniversalTime->dwLowDateTime;

		standart = ft_standart.dwHighDateTime;
		standart <<= 32;
		standart += (UINT32)ft_standart.dwLowDateTime;
		standart += (__int64) (lpTimeZoneInformation->Bias + lpTimeZoneInformation->DaylightBias) * 600000000;

		daylight = ft_daylight.dwHighDateTime;
		daylight <<= 32;
		daylight += (UINT32)ft_daylight.dwLowDateTime;
		daylight += (__int64) (lpTimeZoneInformation->Bias + lpTimeZoneInformation->StandardBias) * 600000000;
		if (north)
		{
			if (curr >= daylight && curr < standart)
				daylightsaving = true;
		}
		else
		{
			if (curr < standart || curr >= daylight)
				daylightsaving = true;
		}
	}

	if (daylightsaving)
		bias += lpTimeZoneInformation->DaylightBias;
	else if (lpTimeZoneInformation->StandardDate.wMonth != 0)
		bias += lpTimeZoneInformation->StandardBias;

	*pBias = bias;

	return true;
}

/***********************************************************************
*              TzSpecificLocalTimeToSystemTime  (KERNEL32.@)
*
*  Converts a local time to a time in Coordinated Universal Time (UTC).
*
* RETURNS
*
*  Returns TRUE when the utc time was calculated.
*/

static bool _Plib_TzSpecificLocalTimeToSystemTime(
	const LPTIME_ZONE_INFORMATION lpTimeZoneInformation, /* [in] The desired time zone. */
	LPSYSTEMTIME            lpLocalTime,           /* [in] The local time. */ 
	LPFILETIME              lpUniversalTime)       /* [out] The  calculated utc time. */
{
	FILETIME ft;
	INT32 lBias;
	__int64 t, bias;

	if (!SystemTimeToFileTime(lpLocalTime, &ft))
		return false;

	t = ft.dwHighDateTime;
	t <<= 32;
	t += (UINT32)ft.dwLowDateTime;

	if (!_GetTimezoneBias(lpTimeZoneInformation, lpLocalTime, &lBias))
	{
		// error , take standart bias;
		lBias = lpTimeZoneInformation->Bias;
	}

	bias = (__int64) lBias * 600000000; /* 60 seconds per minute, 100000 [100-nanoseconds-ticks] per second */
	t += bias;

	lpUniversalTime->dwLowDateTime  = (UINT32)t;
	lpUniversalTime->dwHighDateTime = (UINT32)(t >> 32);

	return true;
}


/***********************************************************************
*              SystemTimeToTzSpecificLocalTime  (KERNEL32.@)
*
*  Converts the system time (utc) to the local time in the specified time zone.
*
* RETURNS
*
*
*  Returns TRUE when the local time was calculated.
*
*/

static bool _Plib_SystemTimeToTzSpecificLocalTime(
	const LPTIME_ZONE_INFORMATION lpTimeZoneInformation, /* [in] The desired time zone. */
	LPFILETIME            lpUniversalTime,       /* [in] The utc time to base local time on. */
	LPFILETIME            lpLocalTime,           /* [out] The local time  in the time zone. */
	INT16 * minBias)								  /* bias in minutes */
{

	INT32 lBias;
	__int64 t,bias;

	t = lpUniversalTime->dwHighDateTime;
	t <<= 32;
	t += (UINT32)lpUniversalTime->dwLowDateTime;

	if (lpTimeZoneInformation->StandardDate.wMonth == 0 || lpTimeZoneInformation->DaylightDate.wMonth == 0 || 
		(lpTimeZoneInformation->DaylightBias == 0 && lpTimeZoneInformation->StandardBias == 0))
	{
		if (minBias)
		{
			*minBias = (INT16) lpTimeZoneInformation->Bias;
		}
		t -= (__int64) lpTimeZoneInformation->Bias * 600000000;
		lpLocalTime->dwLowDateTime  = (UINT32)t;
		lpLocalTime->dwHighDateTime = (UINT32)(t >> 32);
		return true;
	}

	if (!_GetUTCTimezoneBias(lpTimeZoneInformation, lpUniversalTime, &lBias))
	{
		// error , take standart bias;
		lBias = lpTimeZoneInformation->Bias;
	}

	if (minBias)
	{
		*minBias = (INT16) lBias;
	}

	bias = (__int64) lBias * 600000000; /* 60 seconds per minute, 100000 [100-nanoseconds-ticks] per second */
	t -= bias;

	lpLocalTime->dwLowDateTime  = (UINT32)t;
	lpLocalTime->dwHighDateTime = (UINT32)(t >> 32);
	return true;
}

void _PWin32StructTmToSystemTime(const struct tm * timetm, SYSTEMTIME * systime)
{
	systime->wDayOfWeek = static_cast< WORD >( timetm->tm_wday );
	systime->wDay = static_cast< WORD >( timetm->tm_mday );
	systime->wHour = static_cast< WORD >( timetm->tm_hour );
	systime->wMilliseconds = 0;
	systime->wMinute = static_cast< WORD >( timetm->tm_min );
	systime->wMonth = static_cast< WORD >( timetm->tm_mon + 1 );
	systime->wSecond = static_cast< WORD >( timetm->tm_sec );
	systime->wYear = static_cast< WORD >( timetm->tm_year + 1900 );
}

void _PWin32SystemTimeToStructTm(const SYSTEMTIME * systime, struct tm * timetm)
{
	static const int _days[] = 
	{
		-1, 30, 58, 89, 119, 150, 180, 211, 242, 272, 303, 333, 364
	};

	timetm->tm_wday = systime->wDayOfWeek;
	timetm->tm_mday = systime->wDay;
	timetm->tm_hour = systime->wHour;
	timetm->tm_min  = systime->wMinute;
	timetm->tm_mon  = systime->wMonth-1;
	timetm->tm_sec  = systime->wSecond;
	timetm->tm_year = systime->wYear-1900;
	timetm->tm_yday = timetm->tm_mday  + _days[timetm->tm_mon];
	if ( !(timetm->tm_year & 3) && (timetm->tm_mon > 1) )
		timetm->tm_yday++;
	timetm->tm_isdst = -1;
}

bool plib_timetToStructTm(time_t sec, struct tm * timetm) // prohibited for business logic - returns struct tm in system time
{
	if ( sec == (time_t) -1 )
	{
		memset(timetm,0,sizeof(struct tm));
		return false;
	}

	FILETIME ft;
	SYSTEMTIME st;
	_PTimetToFileTime(sec,&ft);
	if (!FileTimeToSystemTime(&ft,&st))
	{
		memset(timetm,0,sizeof(struct tm));
		return false;
	}

	_PWin32SystemTimeToStructTm(&st,timetm);
	return true;
}

bool plib_localtime_ext_bias(time_t sec, struct tm * timetm, INT16 * bias )
{
	if (_plib_tmzone_cache.Bias == -1)
	{
		GetTimeZoneInformation(&_plib_tmzone_cache);
	}
	if ( sec == (time_t) -1 )
	{
		memset(timetm,0,sizeof(struct tm));
		return false;
	}

	FILETIME ftdest;
	FILETIME ft;
	SYSTEMTIME st;

	_PTimetToFileTime(sec,&ft);
	if (_Plib_SystemTimeToTzSpecificLocalTime(&_plib_tmzone_cache,&ft,&ftdest, bias) == false)
	{
		memset(timetm,0,sizeof(struct tm));
		return false;
	}
	if (!FileTimeToSystemTime(&ftdest,&st))
	{
		memset(timetm,0,sizeof(struct tm));
		return false;
	}

	_PWin32SystemTimeToStructTm(&st,timetm);
	return true;
}

bool plib_localtime_ext(time_t sec, struct tm * timetm )
{
	return plib_localtime_ext_bias(sec,timetm,0);
}

struct tm * plib_localtime(const time_t * sec)
{
	static struct tm timetm;
	plib_localtime_ext_bias(*sec,&timetm,0);
	return &timetm;
}

time_t plib_mktime(struct tm * timetm)
{
	if (_plib_tmzone_cache.Bias == -1)
	{
		GetTimeZoneInformation(&_plib_tmzone_cache);
	}
	SYSTEMTIME st;
	FILETIME ft;
	_PWin32StructTmToSystemTime(timetm,&st);
	if (_Plib_TzSpecificLocalTimeToSystemTime(&_plib_tmzone_cache,&st,&ft) == false)
		return (time_t) -1;
	return _PFileTimeToTimet(&ft);
}

time_t plib_mkUTCtime(struct tm * timetm)
{
	SYSTEMTIME st;
	FILETIME ft;
	_PWin32StructTmToSystemTime(timetm,&st);
	if (!SystemTimeToFileTime(&st,&ft))
		return (time_t) -1;
	return _PFileTimeToTimet(&ft);
} 

void PCurrentLocalTime( struct tm* tm )
{
	SYSTEMTIME lt;
	GetLocalTime( &lt );
	_PWin32SystemTimeToStructTm(&lt,tm);
}

void PCurrentUTCTime( struct tm* tm )
{
	SYSTEMTIME lt;
	GetSystemTime( &lt );
	_PWin32SystemTimeToStructTm(&lt,tm);
}

int TIMEZONE()
{
	FILETIME ft;
	FILETIME lft;
	GetSystemTimeAsFileTime(&ft);
	FileTimeToLocalFileTime(&ft,&lft);
	return (int)(_PFileTimeToTimet(&ft)-_PFileTimeToTimet(&lft));
}

bool plib_addDays(struct tm * tm, int day)
{
	static const int _days[] = 
	{
		31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
	};
	if ( day < 0 )
		return false;
	if (tm->tm_mon < 0 || tm->tm_mon >11)
		return false;
	if (tm->tm_mday <= 0 || tm->tm_mday > _days[tm->tm_mon])
	{
		if (! (tm->tm_mon == 1 && !(tm->tm_year & 3) && tm->tm_mday == 29))
			return false;
	}
	while (day--)
	{
		if (++tm->tm_wday > 6)
		{
			tm->tm_wday = 0;
		}

		if (++tm->tm_mday > _days[tm->tm_mon])
		{
			if (tm->tm_mon == 1)
			{
				if ( !(tm->tm_year & 3) && tm->tm_mday == 29)
					continue;
			}
			tm->tm_mday = 1;
			if (++tm->tm_mon > 11)
			{
				tm->tm_mon = 0;
				tm->tm_year++;
			}
		}
	}
	return true;
}
#endif

int PCompareLocalTime( const struct tm* a, const struct tm* b )
{
	if( a->tm_year != b->tm_year )
		return a->tm_year - b->tm_year;
	if( a->tm_mon != b->tm_mon )
		return a->tm_mon - b->tm_mon;
	if( a->tm_mday != b->tm_mday )
		return a->tm_mday - b->tm_mday;
	if( a->tm_hour != b->tm_hour )
		return a->tm_hour - b->tm_hour;
	if( a->tm_min != b->tm_min )
		return a->tm_min - b->tm_min;
	return a->tm_sec - b->tm_sec;
}

char * p_itoa( _In_ INT32 val_, _Pre_notnull_ _Post_z_ char * str )
{
	char buffer[64];
	char * p = buffer+63;
	*p--=0;
	bool sign = val_<0;
	UINT32 val;
	if (sign)
		val = -val_; // ATTN: 0x8000 0000
	else
		val = val_;
	if (val)
	{
		while(val)
		{
			*p--=(val%10)+'0';
			val/=10;
		}
		if (sign)
		{
			*p--='-';
		}
	}
	else
	{
		*p--='0';
	}
	p++;
	strcpy(str, p);
	return str;
}

char * p_utoa( _In_ UINT32 val, _Pre_notnull_ _Post_z_ char * str )
{
	char buffer[64];
	char * p = buffer+63;
	*p--=0;
	if (val)
	{
		while(val)
		{
			*p--=(val%10)+'0';
			val/=10;
		}
	}
	else
	{
		*p--='0';
	}
	p++;
	strcpy(str,p);
	return str;
}

char * p_hextoa( _In_ UINT32 val, _Pre_notnull_ _Post_z_ char * str )
{
	char buffer[64];
	char * p = buffer+63;
	*p--=0;
	if (val)
	{
		while(val)
		{
			int digit = (val%16)+'0';
			if (digit >'9')
				digit += 'A' - '9' - 1;
			*p-- = static_cast< char >( digit );
			val/=16;
		}
	}
	else
	{
		*p--='0';
	}
	p++;
	strcpy(str, p);
	return str;
}

char * p_i64toa( _In_ INT64 val_, _Pre_notnull_ _Post_z_ char * str )
{
	char buffer[64];
	char * p = buffer+63;
	*p--=0;
	bool sign = val_<0;
	UINT64 val;
	if (sign)
		val = -val_; // ATTN: 0x8000 0000 0000 0000
	else
		val = val_;
	if (val)
	{
		while(val)
		{
			*p--=char(val%10)+'0';
			val/=10;
		}
		if (sign)
		{
			*p--='-';
		}
	}
	else
	{
		*p--='0';
	}
	p++;
	strcpy(str,p);
	return str;
}

char * p_u64toa( _In_ UINT64 val, _Pre_notnull_ _Post_z_ char * str )
{
	char buffer[64];
	char * p = buffer+63;
	*p--=0;
	if (val)
	{
		while(val)
		{
			*p--=char(val%10)+'0';
			val/=10;
		}
	}
	else
	{
		*p--='0';
	}
	p++;
	strcpy(str,p);
	return str;
}

char * p_hex64toa( _In_ UINT64 val, _Pre_notnull_ _Post_z_ char * str )
{
	char buffer[64];
	char * p = buffer+63;
	*p--=0;
	if (val)
	{
		while(val)
		{
			char digit = char(val%16)+'0';
			if (digit >'9')
				digit += 'A' - '9' - 1;
			*p-- = digit;
			val/=16;
		}
	}
	else
	{
		*p--='0';
	}
	p++;
	strcpy(str,p);
	return str;
}

UINT64 p_atou64( _In_opt_z_ const char * str )
{
	UINT64 result64 = 0;
	if (!str)
		return 0;

	while ( isspace((int)(unsigned char)*str) )
		++str;

	while(*str && *str >= '0' && *str <= '9')
	{
		result64 = result64*10 + (*str - '0');
		str++;
	}
	return result64;
}

INT64 p_atoi64( _In_opt_z_ const char * str )
{
	INT64 result64 = 0;
	bool sign = false;
	if (!str)
		return 0;

	while ( isspace((int)(unsigned char)*str) )
		++str;

	if (*str == '-')
	{
		sign=true;
		str++;
	}
	while(*str && *str >= '0' && *str <= '9')
	{
		result64 = result64*10 + (*str - '0');
		str++;
	}
	if (sign)
	{
		result64 = -result64;
	}
	return result64;
}

UINT64 p_atohex64( _In_opt_z_ const char * str )
{
	UINT64 result64 = 0;
	if (!str)
		return 0;

	while ( isspace((int)(unsigned char)*str) )
		++str;

	while(*str && (*str >= '0' && *str <= '9' || *str>='A' && *str <='F' || *str >='a' && *str <='f'))
	{
		if (*str >= '0' && *str <= '9')
			result64 = result64*16 + (*str - '0');
		else if (*str>='A' && *str <='F')
			result64 = result64*16 + (*str - 'A' + 10);
		else if (*str>='a' && *str <='f')
			result64 = result64*16 + (*str - 'a' + 10);
		str++;
	}
	return result64;
}

#if defined( PWIN32 )

void _PWin32AppendWStringFromPStringEncoding( PBlock& ret /* of WCHAR's */, const char* s, const PStringEncoding* enc /* = NULL */ )
//IMPORTANT: produces unterminated string; use _PWin32NullTerminateWString to terminate
{
	if( enc == NULL )
	{
		for( const char* p = s; *p ; ++p )
			*(WCHAR*)ret.append( sizeof(WCHAR) ) = *p;
		return;
	}

	PStringEncoding::StringParser parser;
	enc->createNewParser( parser );
	parser.init( s );
	for(;;)
	{
		PUNICHAR_RET ch = parser.nextChar();
		if( ch <= 0 )
			break;
		WCHAR c = (WCHAR)ch;
		*(WCHAR*)ret.append( sizeof(WCHAR) ) = c;
	}
}

void _PWin32WStringToPStringEncoding( PString& ret, const WCHAR* ws, const PStringEncoding* enc )
{
	PASSERT( enc != NULL );
	if( sizeof(WCHAR) == sizeof(PUNICHAR) )
	{
		enc->append( ret, (const PUNICHAR*)ws );
	}
	else
	{
		for( const WCHAR* p = ws; *p ; ++p )
		{
			PUNICHAR c = *p;
			enc->append( ret, &c, &c + 1 );
		}
	}
}

void _PWin32AppendWStringFromPStringEncodingC( PBlock& ret /* of WCHAR's */, const char* s, const PStringEncoding* enc /* = NULL */ )
//IMPORTANT: converts lf to cr-lf & produces unterminated string; use _PWin32NullTerminateWString to terminate
{
	if( enc == NULL )
	{
		for( const char* p = s; *p ; ++p )
			switch( *p )
			{
			case '\n':
				*(WCHAR*)ret.append( sizeof(WCHAR) ) = '\r';
				*(WCHAR*)ret.append( sizeof(WCHAR) ) = '\n';
				break;
			case '\r':
				break;
			default:
				*(WCHAR*)ret.append( sizeof(WCHAR) ) = *p;
				break;
			}
		return;
	}

	PStringEncoding::StringParser parser;
	enc->createNewParser( parser );
	parser.init( s );
	for(;;)
	{
		PUNICHAR_RET ch = parser.nextChar();
		if( ch <= 0 )
			break;
		WCHAR c = (WCHAR)ch;
		switch( c )
		{
		case '\n':
			*(WCHAR*)ret.append( sizeof(WCHAR) ) = '\r';
			*(WCHAR*)ret.append( sizeof(WCHAR) ) = '\n';
			break;
		case '\r':
			break;
		default:
			*(WCHAR*)ret.append( sizeof(WCHAR) ) = c;
			break;
		}
	}
}

void _PWin32WStringToPStringEncodingC( PString& ret, const WCHAR* ws, const PStringEncoding* enc )
{
	//IMPORTANT: converts cr-lf to lf
	PASSERT4( enc != NULL );
	for( const WCHAR* p = ws; *p ; ++p )
	{
		if( *p != '\r' )
		{
			PUNICHAR c = *p;
			enc->append( ret, &c, &c + 1 );
		}
	}
}

void _PWin32NullTerminateWString( PBlock& ret /* of WCHAR's */ )
{
	*(WCHAR*)ret.append( sizeof(WCHAR) ) = 0;
}

#ifndef PWIDE_ONLY

static INT32 _isUnicode = 0;

bool _PWinIsUnicode()
{
	if( _isUnicode == 0 )
		return ( ::GetVersion() & 0x80000000 ) == 0;
	return _isUnicode > 0;
}

void _PWinSetUnicode( INT32 isUnicode )
{
	_isUnicode = isUnicode;
}

void _PWin32WStringToA( PBlock& s, const WCHAR* ws )//for use on !_PWinIsUnicode() (9x) ONLY
{
	int sz = ::WideCharToMultiByte( CP_ACP, 0, ws, -1, NULL, 0, NULL, NULL );
	s.alloc( sz );
	::WideCharToMultiByte( CP_ACP, 0, ws, -1, (char*)s.ptr(), sz, NULL, NULL );
}

#endif

void _PWin32AStringToW( PBlock& ws, const char* s )//for use on !_PWinIsUnicode() (9x) ONLY
{
	int sz = ::MultiByteToWideChar( CP_ACP, 0, s, -1, NULL, 0 );
	ws.alloc( sz * sizeof(WCHAR) );
	::MultiByteToWideChar( CP_ACP, 0, s, -1, (WCHAR*)ws.ptr(), sz );
}

#endif

#if defined(P_MAC10X)
#include <CoreFoundation/CoreFoundation.h>
bool _PMacPathToUTF8( PBlock& u, const char* s, const PStringEncoding* enc )
{
	PStringEncoding::StringParser parser;
	enc->createNewParser( parser );
	parser.init( s );

	int len = enc->charLength( s );
	PBlock b;
	b.alloc( ( len + 1 ) * sizeof(UniChar) );
	UniChar* buf = (UniChar*)b.ptr();

	for( int i = 0; i < len; ++i )
	{
		PUNICHAR_RET ch = parser.nextChar();
		PASSERT4( ch > 0 );
		if( 0 < ch )
			*buf++ = ch;
	}
	CFStringRef str = CFStringCreateWithCharacters( kCFAllocatorDefault, (UniChar*)b.ptr(), len );
	PASSERT4( str );
	CFIndex sz = CFStringGetMaximumSizeOfFileSystemRepresentation( str );
	u.alloc( sz );
	bool result = CFStringGetFileSystemRepresentation( str, (char*)u.ptr(), sz );
	CFRelease( str );
	return result;
}
#endif

#if defined(P_ANDROID) || defined(LINUX)

#include "ppi18n_utf8.h"

void _convertPStringArbitraryToUtf8(char const *src, PStringEncoding const *from, PString &dst) 
{

	dst.cut(0);

	if (!src)
	{
		return;
	}

	if (from) 
	{
		PStringEncodingUtf8 to;

		PStringEncoding::StringParser parser;
		from->createNewParser(parser);
		parser.init(src);

		int len = from->charLength(src);

		for (int i = 0; i < len; ++i) 
		{
			PUNICHAR_RET ch = parser.nextChar();
			if (ch > 0) 
			{
				to.append(dst, ch);
			}
		}
	} 
	else 
	{
		dst.append(src);
	}
}

void _convertPStringUtf8ToArbitrary(char const *src, PStringEncoding const *to, PString &dst) 
{
	dst.cut(0);

	if (!src || !*src)
	{
		return;
	}

	if (to) 
	{
		PStringEncodingUtf8 from;
		PStringEncoding::StringParser parser;
		from.createNewParser(parser);
		parser.init(src);

		int len = from.charLength(src);

		for (int i = 0; i < len; ++i) 
		{
			PUNICHAR_RET ch = parser.nextChar();
			if (ch > 0) 
			{
				to->append(dst, ch);
			}
		}
	} 
	else 
	{
		dst.append(src);
	}
}

#endif // P_ANDROID || LINUX

UINT32 plib_timeticks(UINT32 * sec)
{
	UINT32 ret=LONGSYSTEMTICKS(0);
	if (sec)
	{
		*sec = ret;
	}
	return ret;
}

FILE* plib_fopen( const char* fName, const char* mode, const PStringEncoding* fNameEncoding /* = NULL */ )
{
	if( fNameEncoding == NULL ) 
	{
		FILE * f = ::fopen( fName, mode );
#ifdef  PPOSIX  
		if (f)
		{
			int flags = ::fcntl( fileno(f),F_SETFD,0);
			::fcntl( fileno(f),F_SETFD,flags | FD_CLOEXEC);
		}
#endif	    
		return f;
	}
#if defined PWIN32
	PBlock wfName;//of WCHAR
	_PWin32AppendWStringFromPStringEncoding( wfName, fName, fNameEncoding );
	_PWin32NullTerminateWString( wfName );
#if !defined( PWIDE_ONLY )
	if( ! _PWinIsUnicode() )
	{
		PBlock afName;
		_PWin32WStringToA( afName, (const WCHAR*)wfName.ptr() );
		return ::fopen( (const char*)afName.ptr(), mode );
	}
	else
#endif
	{
		PBlock wMode;
		_PWin32AppendWStringFromPStringEncoding( wMode, mode, NULL );
		_PWin32NullTerminateWString( wMode );
		return ::_wfopen( (const WCHAR*)wfName.ptr(), (const WCHAR*)wMode.ptr() );
	}
#elif defined(P_MAC10X)

	PBlock path;
	if( _PMacPathToUTF8( path, fName, fNameEncoding ) )
		return ::fopen( (const char*)path.ptr(), mode );
	else
		return NULL;

#elif defined(P_ANDROID) || defined(LINUX)

	PString path;
	_convertPStringArbitraryToUtf8(fName, fNameEncoding, path);
	return ::fopen(path.c_str(), mode);

#else
	throw PInternalError( "filename encodings are not supported on this platform" );
#endif
}

