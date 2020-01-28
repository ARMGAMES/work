#include "i18n.utils.h"
#include "i18n.localutil.h"
#include "brands.h"

#include "commonmacros.h"
#include "ppcheck.h"

//-----------------------------------------------------------------------------
#ifndef PROTOCOL_CLIENT_ONLY
//-----------------------------------------------------------------------------

#include "i18n.msg_int.h"

struct IntTableInitializer 
{
	IntTableInitializer()
	{
		i18nInitIntTable();
	}
};

static IntTableInitializer intTableInitializer_;

//-----------------------------------------------------------------------------
#endif // PROTOCOL_CLIENT_ONLY
//-----------------------------------------------------------------------------

void i18nFormatShortTimezoneName(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, eTimeZone tz)
{
	if( isValidCustTimezone( tz ) )
		tz = eTzCUSTOM_start;
	
	static const PMsgId tzShortNamesId[] =
	{
		P_TXT( TXTINT_TimeZone_UTC_Short ),
		P_TXT( TXTINT_TimeZone_ET_Short ),
		P_TXT( TXTINT_TimeZone_CET_Short ),
		P_TXT( TXTINT_TimeZone_CST_Short ),
		P_TXT( TXTINT_TimeZone_PT_Short ),
		P_TXT( TXTINT_TimeZone_MT_Short ),
		P_TXT( TXTINT_TimeZone_CT_Short ),
		P_TXT( TXTINT_TimeZone_AT_Short ),
		P_TXT( TXTINT_TimeZone_BRT_Short ),
		P_TXT( TXTINT_TimeZone_EET_Short ),
		P_TXT( TXTINT_TimeZone_WET_Short ),
		P_TXT( TXTINT_TimeZone_MSK_Short ),
		P_TXT( TXTINT_TimeZone_WST_Short ),
		P_TXT( TXTINT_TimeZone_EST_Short ),
		P_TXT( TXTINT_TimeZone_NZT_Short ),
		P_TXT( TXTINT_TimeZone_CCT_Short ),
		P_TXT( TXTINT_TimeZone_JST_Short ),
		P_TXT( TXTINT_TimeZone_ACST_Short ),
		P_TXT( TXTINT_TimeZone_ART_Short ),
		P_TXT( TXTINT_TimeZone_AKT_Short ),
		P_TXT( TXTINT_TimeZone_HST_Short ),
		P_TXT( TXTINT_TimeZone_NT_Short ),
		P_TXT( TXTINT_TimeZone_IST_Short ),
        P_TXT( TXTINT_TimeZone_Custom_Short ), // PYR-22234
	};

	PASSERT5(tz < sizeof(tzShortNamesId)/sizeof(*tzShortNamesId));
	if(tz < sizeof(tzShortNamesId)/sizeof(*tzShortNamesId))
		PMsg(ret, table, localeId, tzShortNamesId[tz]);
}

void i18nFormatFullTimezoneName(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, eTimeZone tz)
{
	if( isValidCustTimezone( tz ) )
		tz = eTzCUSTOM_start;

	static const PMsgId tzFullNamesId[] =
	{
		P_TXT( TXTINT_TimeZone_UTC_Full ),
		P_TXT( TXTINT_TimeZone_ET_Full ),
		P_TXT( TXTINT_TimeZone_CET_Full ),
		P_TXT( TXTINT_TimeZone_CST_Full ),
		P_TXT( TXTINT_TimeZone_PT_Full ),
		P_TXT( TXTINT_TimeZone_MT_Full ),
		P_TXT( TXTINT_TimeZone_CT_Full ),
		P_TXT( TXTINT_TimeZone_AT_Full ),
		P_TXT( TXTINT_TimeZone_BRT_Full ),
		P_TXT( TXTINT_TimeZone_EET_Full ),
		P_TXT( TXTINT_TimeZone_WET_Full ),
		P_TXT( TXTINT_TimeZone_MSK_Full ),
		P_TXT( TXTINT_TimeZone_WST_Full ),
		P_TXT( TXTINT_TimeZone_EST_Full ),
		P_TXT( TXTINT_TimeZone_NZT_Full ),
		P_TXT( TXTINT_TimeZone_CCT_Full ),
		P_TXT( TXTINT_TimeZone_JST_Full ),
		P_TXT( TXTINT_TimeZone_ACST_Full ),
		P_TXT( TXTINT_TimeZone_ART_Full ),
		P_TXT( TXTINT_TimeZone_AKT_Full ),
		P_TXT( TXTINT_TimeZone_HST_Full ),
		P_TXT( TXTINT_TimeZone_NT_Full ),
		P_TXT( TXTINT_TimeZone_IST_Full ),
        P_TXT( TXTINT_TimeZone_Custom_Full ), // PYR-22234
	};
	PASSERT5(tz < sizeof(tzFullNamesId)/sizeof(*tzFullNamesId));
	if(tz < sizeof(tzFullNamesId)/sizeof(*tzFullNamesId))
		PMsg(ret, table, localeId, tzFullNamesId[tz]);
}

void i18nFormatDate(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, const SrvDate& date)
{
	char year[16], month[16], day[16];
	p_utoa(date._year, year);
	sprintf(month, "%02u", (UINT32) date._month);
	sprintf(day, "%02u", (UINT32) date._day);
	PMsg3(ret, table, localeId, P_TXT( TXTINT_FORMAT_ShortDate ), year, month, day);
}

void i18nFormatDateTZ(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, const SrvTime& time, UINT32 tzfDest)
{
	SrvTime timeDest;
	tzLocalToLocalSrvTime(time, eTzET, -1, timeDest, getTimezone(tzfDest));
	SrvDate dateDest(timeDest);
	i18nFormatDate(ret, table, localeId, dateDest);
}

void i18nFormatDateTZ(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, time_t time, UINT32 tzfDest)
{
	SrvTime timeDest;
	tzUtcToLocalSrvTime(time, timeDest, getTimezone(tzfDest));
	SrvDate dateDest(timeDest);
	i18nFormatDate(ret, table, localeId, dateDest);
}

void i18nFormatTime(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, const SrvTime& time, bool imperial)
{
	char hour[16], minute[16];
	sprintf(minute, "%02u", (UINT32) time._minute);
	if(imperial)
	{
		PString ampm;
		if(time._hour == 0)		//12am
		{
			p_utoa(12, hour);
			PMsg(ampm, table, localeId, P_TXT( TXTINT_am ));
		}
		else if(time._hour > 12)	//1-11pm
		{
			p_utoa(time._hour - 12, hour);
			PMsg(ampm, table, localeId, P_TXT( TXTINT_pm ));
		}
		else if(time._hour == 12)	//12pm
		{
			p_utoa(12, hour);
			PMsg(ampm, table, localeId, P_TXT( TXTINT_pm ));
		}
		else					//1-11am
		{
			p_utoa(time._hour, hour);
			PMsg(ampm, table, localeId, P_TXT( TXTINT_am ));
		}
		PMsg3(ret, table, localeId, P_TXT( TXTINT_FORMAT_Time_12 ), hour, minute, ampm);
	}
	else
	{
        sprintf(hour, "%02u", (UINT32) time._hour);  // PYR-15655
		PMsg2(ret, table, localeId, P_TXT( TXTINT_FORMAT_Time ), hour, minute);
	}
}

void i18nFormatTimeTZ(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, const SrvTime& time, eTimeZone tz, bool withTZ, bool imperial)
{
	SrvTime localTime;
	bool isAmb = false;
	tzLocalToLocalSrvTime(time, eTzET, -1, localTime, tz, &isAmb);
	PString dtStr;
	i18nFormatTime(dtStr, table, localeId, localTime, imperial);
	if(isAmb)
	{
		PString dtStrAmb;
		PMsg1(dtStrAmb, table, localeId, P_TXT( TXTINT_FORMAT_TimeAmb ), dtStr);
		dtStr.assign(dtStrAmb);
	}
	if(withTZ)
	{
		PString tzStr;
		i18nFormatShortTimezoneName(tzStr, table, localeId, tz);
		PMsg2(ret, table, localeId, P_TXT( TXTINT_DateTimeWithTZ ), dtStr, tzStr);
	}
	else
	{
		i18nAppend(ret, dtStr);
	}
}

void i18nFormatTimeTZ(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, const SrvTime& time, UINT32 tzf, bool withTZ)
{
	i18nFormatTimeTZ(ret, table, localeId, time, getTimezone(tzf), withTZ, getTimeFormat(tzf));
}

void i18nFormatTimeTZ(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, time_t time, eTimeZone tz, bool withTZ, bool imperial)
{
	SrvTime localTime;
	bool isAmb = false;
	tzUtcToLocalSrvTime(time, localTime, tz, &isAmb);
	PString dtStr;
	i18nFormatTime(dtStr, table, localeId, localTime, imperial);
	if(isAmb)
	{
		PString dtStrAmb;
		PMsg1(dtStrAmb, table, localeId, P_TXT( TXTINT_FORMAT_TimeAmb ), dtStr);
		dtStr.assign(dtStrAmb);
	}
	if(withTZ)
	{
		PString tzStr;
		i18nFormatShortTimezoneName(tzStr, table, localeId, tz);
		PMsg2(ret, table, localeId, P_TXT( TXTINT_DateTimeWithTZ ), dtStr, tzStr);
	}
	else
	{
		i18nAppend(ret, dtStr);
	}
}

void i18nFormatTimeTZ(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, time_t time, UINT32 tzf, bool withTZ)
{
	i18nFormatTimeTZ(ret, table, localeId, time, getTimezone(tzf), withTZ, getTimeFormat(tzf));
}

void i18nFormatLongTime(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, const SrvTime& time, bool showMin)
{
	char hour[16], minute[16];
	if(time._hour && (time._minute || showMin))
	{
		sprintf(minute, "%02u", (UINT32) time._minute);
		p_utoa(time._hour, hour);
		PMsg2(ret, table, localeId, P_TXT( TXTINT_FORMAT_LongTime ), hour, minute);
	}
	else if(time._hour)
	{
		p_utoa(time._hour, hour);
		PMsg1(ret, table, localeId, P_TXT( TXTINT_FORMAT_LongTimeHour ), hour);
	}
	else // always show at least "0 min"
	{
		p_utoa(time._minute, minute);
		PMsg1(ret, table, localeId, P_TXT( TXTINT_FORMAT_LongTimeMin ), minute);
	}
}

void i18nFormatDateTime(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, const SrvTime& time, bool imperial)
{
	PString ampm;
	char year[16], month[16], day[16];
	p_utoa(time._year, year);
	sprintf(month, "%02u", (UINT32) time._month);
	sprintf(day, "%02u", (UINT32) time._day);
	char hour[16], minute[16];
	if(imperial)
	{
		if(time._hour == 0)		//12am
		{
			p_utoa(12, hour);
			PMsg(ampm, table, localeId, P_TXT( TXTINT_AM ));
		}
		else if(time._hour > 12)	//1-11pm
		{
			p_utoa(time._hour - 12, hour);
			PMsg(ampm, table, localeId, P_TXT( TXTINT_PM ));
		}
		else if(time._hour == 12)	//12pm
		{
			p_utoa(12, hour);
			PMsg(ampm, table, localeId, P_TXT( TXTINT_PM ));
		}
		else					//1-11am
		{
			p_utoa(time._hour, hour);
			PMsg(ampm, table, localeId, P_TXT( TXTINT_AM ));
		}
	}
	else
	{
        sprintf(hour, "%02u", (UINT32) time._hour);  // PYR-15655
	}
	sprintf(minute, "%02u", (UINT32) time._minute);
	PString dtStr;
	PMsg5(dtStr, table, localeId, P_TXT( TXTINT_FORMAT_DateTime ), year, month, day, hour, minute);
	if(imperial)
	{
		PMsg2(ret, table, localeId, P_TXT( TXTINT_FORMAT_Date_Time ), dtStr, ampm);
	}
	else
	{
		i18nAppend(ret, dtStr);
	}
}

void i18nFormatDateTimeTZ(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, const SrvTime& time, eTimeZone tz, bool withTZ, bool imperial)
{
	SrvTime localTime;
	bool isAmb = false;
	tzLocalToLocalSrvTime(time, eTzET, -1, localTime, tz, &isAmb);
	PString dtStr;
	i18nFormatDateTime(dtStr, table, localeId, localTime, imperial);
	if(isAmb)
	{
		PString dtStrAmb;
		PMsg1(dtStrAmb, table, localeId, P_TXT( TXTINT_FORMAT_TimeAmb ), dtStr);
		dtStr.assign(dtStrAmb);
	}
	if(withTZ)
	{
		PString tzStr;
		i18nFormatShortTimezoneName(tzStr, table, localeId, tz);
		PMsg2(ret, table, localeId, P_TXT( TXTINT_DateTimeWithTZ ), dtStr, tzStr);
	}
	else
	{
		i18nAppend(ret, dtStr);
	}
}

void i18nFormatDateTimeTZ(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, const SrvTime& time, UINT32 tzf, bool withTZ)
{
	i18nFormatDateTimeTZ(ret, table, localeId, time, getTimezone(tzf), withTZ, getTimeFormat(tzf));
}

void i18nFormatDateTimeAltTZ(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, const SrvTime& time, UINT32 tzf)
{
	if(getTimezone(tzf) == eTzET)
	{
		i18nFormatDateTimeTZ(ret, table, localeId, time, getTimezone(tzf), true, getTimeFormat(tzf));
	}
	else
	{
		PString dtStr;
		i18nFormatDateTimeTZ(dtStr, table, localeId, time, getTimezone(tzf), true, getTimeFormat(tzf));
		PString dtStrET;
		i18nFormatDateTimeTZ(dtStrET, table, localeId, time, eTzET, true, false);
		PMsg2(ret, table, localeId, P_TXT( TXTINT_FORMAT_DateTimeAlt ), dtStr, dtStrET);
	}
}

void i18nFormatDateTimeTZ(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, time_t time, eTimeZone tz, bool withTZ, bool imperial)
{
	SrvTime localTime;
	bool isAmb = false;
	tzUtcToLocalSrvTime(time, localTime, tz, &isAmb);
	PString dtStr;
	i18nFormatDateTime(dtStr, table, localeId, localTime, imperial);
	if(isAmb)
	{
		PString dtStrAmb;
		PMsg1(dtStrAmb, table, localeId, P_TXT( TXTINT_FORMAT_TimeAmb ), dtStr);
		dtStr.assign(dtStrAmb);
	}
	if(withTZ)
	{
		PString tzStr;
		i18nFormatShortTimezoneName(tzStr, table, localeId, tz);
		PMsg2(ret, table, localeId, P_TXT( TXTINT_DateTimeWithTZ ), dtStr, tzStr);
	}
	else
	{
		i18nAppend(ret, dtStr);
	}
}

void i18nFormatDateTimeTZ(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, time_t time, UINT32 tzf, bool withTZ)
{
	i18nFormatDateTimeTZ(ret, table, localeId, time, getTimezone(tzf), withTZ, getTimeFormat(tzf));
}

void i18nFormatDateTimeAltTZ(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, time_t time, UINT32 tzf)
{
	if(getTimezone(tzf) == eTzET)
	{
		i18nFormatDateTimeTZ(ret, table, localeId, time, getTimezone(tzf), true, getTimeFormat(tzf));
	}
	else
	{
		PString dtStr;
		i18nFormatDateTimeTZ(dtStr, table, localeId, time, getTimezone(tzf), true, getTimeFormat(tzf));
		PString dtStrET;
		i18nFormatDateTimeTZ(dtStrET, table, localeId, time, eTzET, true, false);
		PMsg2(ret, table, localeId, P_TXT( TXTINT_FORMAT_DateTimeAlt ), dtStr, dtStrET);
	}
}

void i18nFormatDateTimeWithSecTZ(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, const SrvTime& time, UINT32 tzf)
{
	eTimeZone tz = getTimezone(tzf);

	PString tzStr;

	i18nFormatShortTimezoneName(tzStr, table, localeId, tz);

	SrvTime localTime;
	bool isAmb = false;

	tzLocalToLocalSrvTime(time, eTzET, -1, localTime, tz, &isAmb);

	char year[16], month[16], day[16], hour[16], minute[16], second[16];

	p_utoa(localTime._year, year);
	sprintf(month, "%02u", (UINT32)localTime._month);
	sprintf(day, "%02u", (UINT32)localTime._day);
	PString dateStr;
	PMsg3(dateStr, table, localeId, P_TXT( TXTINT_FORMAT_ShortDate ), year, month, day);

	//ignore time format, always 24, no ambiguous
    sprintf(hour, "%02u", (UINT32)localTime._hour); // PYR-15655
	sprintf(minute, "%02u", (UINT32)localTime._minute);
	sprintf(second, "%02u", (UINT32)localTime._second);
	PString timeStr;
	PMsg4(timeStr, table, localeId, P_TXT( TXTINT_FORMAT_TimeWithSecTZ ), hour, minute, second, tzStr);

	PMsg2(ret, table, localeId, P_TXT( TXTINT_FORMAT_Date_Time ), dateStr, timeStr);
}

/// Convert time to its ISO 8601 string representation
/// @param ret     - result of conversion
/// @param srcTime - time to convert
/// @param tzSrc   - timezone of 'srcTime'
/// @param isDst   - 'daylight savings time', valid values: -1 for auto detection, 0 if no DST, 1 if DST.
///                  Used only in rare cases when srcTime is ambiguous
/// @param tzDest  - destination timezone
void i18nFormatDateTimeISO8601(PString& ret, const SrvTime& srcTime, eTimeZone tzSrc, int isDst, eTimeZone tzDest)
{
	SrvTime dstTime;
	int isDstDst = tzLocalToLocalSrvTime(srcTime, tzSrc, isDst, dstTime, tzDest);

	time_t utcTimeTZ = tzSrvTimeToUtc(dstTime, tzDest, isDstDst);
	time_t utcTime   = tzSrvTimeToUtc(dstTime, eTzUTC, isDstDst);

	time_t diff = utcTime - utcTimeTZ;
	char sign = '+';
	if (diff < 0)
	{
		diff = -diff;
		sign = '-';
	}

#ifndef SECONDS_IN_AN_HOUR
#define SECONDS_IN_AN_HOUR	3600
#endif

#ifndef SECONDS_IN_A_MINUTE
#define SECONDS_IN_A_MINUTE	60
#endif

	int TZH = static_cast< int >( diff ) / SECONDS_IN_AN_HOUR;
	int TZM = (diff % SECONDS_IN_AN_HOUR) / SECONDS_IN_A_MINUTE;

	char buff[100];
	sprintf(buff, "%04u-%02u-%02uT%02u:%02u:%02u%c%02u:%02u",
		dstTime._year, dstTime._month, dstTime._day,
		dstTime._hour, dstTime._minute, dstTime._second,
		sign, TZH, TZM);
	ret.assign(buff);
}


void i18nFormatFloat(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, UINT64 number, short places, bool showTrailZeros, bool showTriplets)
{
	PASSERT5( 0 <= places );
	if( places < 0 )
		places = 0;

	char str[32];
	p_u64toa( number, str );
	const int l = strlen( str );

	char *ptrFraction = &str[ l - places ];
	char *ptrValue = &str[ l - min( places, (short)1 ) ];
	while( *ptrValue == '0' && ptrValue != str && ptrValue != ptrFraction )
		ptrValue--;

	if( str < ptrFraction )
	{
		for( char *ptr = str; ptr < ptrFraction ; ptr++ )
		{
			i18nAppendChar(ret, *ptr );
			if( showTriplets )
			{
				if( localeId == LOCALE_EN_IN )
				{
					const int i = ( ptrFraction - ptr - 2 );

					// xx,xx,xxx.xx - lakh, crore
					if( i > 0 && ( i % 2 ) == 0 )
						PMsg( ret, table, localeId, P_TXT( TXTINT_DigitsSeparator ) );
				}
				else
				{
					const int i = ( ptrFraction - ptr - 1 );

					if( i && 0 == ( i % 3 ) )
						PMsg( ret, table, localeId, P_TXT( TXTINT_DigitsSeparator ) );
				}
			}
		}
	}
	else
	{
		i18nAppendChar(ret, '0' );
	}

	if( places )
	{
		if( *ptrValue != '0' || showTrailZeros )
		{
			PMsg(ret, table, localeId, P_TXT( TXTINT_DecimalPoint ));
			for( char *ptr = ptrFraction; ; ptr++ )
			{
				if( ptr < str )
				{
					i18nAppendChar(ret, '0' );
				}
				else
				{
					if( 0 == *ptr )
						break;
					i18nAppendChar(ret, *ptr );
					if( false == showTrailZeros && ptr == ptrValue )
						break;
				}
			}
		}
	}
}

void i18nFormatMoney(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, UINT64 cents_, const char* currSign, bool showZeroCents, bool showTriplets)
{
	PString moneyStr;
	i18nFormatFloat(moneyStr, table, localeId, cents_, 2, (showZeroCents || cents_ % 100), showTriplets);
	PMsg2(ret, table, localeId, P_TXT( TXTINT_Format_Money_Sign ), currSign, moneyStr);
}

void i18nFormatMoneyEx(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, const char* currStr, eFormatCurrType currType, const char* moneyStr)
{
	switch(currType)
	{
	case eFormatCurrTypeSign:
		PMsg2(ret, table, localeId, P_TXT( TXTINT_Format_Money_Sign ), currStr, moneyStr);
		break;
	case eFormatCurrTypeCode:
		PMsg2(ret, table, localeId, P_TXT( TXTINT_Format_Money_Code ), currStr, moneyStr);
		break;
	case eFormatCurrTypeSignWithCode:
		PMsg2(ret, table, localeId, P_TXT( TXTINT_Format_Money_Sign_With_Code ), currStr, moneyStr);
		break;
	case eFormatCurrTypeName:
		PMsg2(ret, table, localeId, P_TXT( TXTINT_Format_Money_Name ), currStr, moneyStr);
		break;
	default:
		PASSERT5(0);
		PMsg2(ret, table, localeId, P_TXT( TXTINT_Format_Money_Sign ), currStr, moneyStr);
		break;
	}
}

void i18nFormatMoneyEx(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, UINT64 cents_, const char* currStr, eFormatCurrType currType, bool showZeroCents, bool showTriplets)
{
	PString moneyStr;
	i18nFormatFloat(moneyStr, table, localeId, cents_, 2, (showZeroCents || cents_ % 100), showTriplets);
	i18nFormatMoneyEx(ret, table, localeId, currStr, currType, moneyStr);
}

struct i18nCurrMapItem
{
	const char*	value;
	PMsgId	code;
	PMsgId	name;
	PMsgId	sign;

	bool operator<( const i18nCurrMapItem& other ) const
	{
		return PString::compare( value, other.value ) < 0;
	}
};

#define _CurrMapItem( code )		{ #code, P_TXT( TXTINT_CurrCode_##code ), P_TXT( TXTINT_CurrNameC_pl_##code ), P_TXT( TXTINT_CurrSign_##code ) }
#define _CurrMapItem_NoSign( code )	{ #code, P_TXT( TXTINT_CurrCode_##code ), P_TXT( TXTINT_CurrNameC_pl_##code ) }

static const i18nCurrMapItem* _i18nFindCurrencyInfo( const char* value )
{
	static const i18nCurrMapItem _currMap[] = // must be sorted by value
	{
		_CurrMapItem( BGN ),
		_CurrMapItem( BRL ),
		_CurrMapItem( CAD ),
		_CurrMapItem( CHF ),
		_CurrMapItem( CZK ),
		_CurrMapItem( DKK ),
		_CurrMapItem( EUR ),
		_CurrMapItem( GBP ),
		_CurrMapItem( INR ),
		_CurrMapItem( NOK ),
		_CurrMapItem_NoSign( RON ),
		_CurrMapItem( SEK ),
		_CurrMapItem( USD ),
	};
	const static i18nCurrMapItem* _currMapEnd = _currMap + SZARR( _currMap );
	_DEBUG_CHECK_TABLE_ORDER( _currMap, _currMapEnd );

	const i18nCurrMapItem cmp = { value[ 0 ] == '\0' ? "USD" : value };
	auto iter = lower_bound( _currMap, _currMapEnd, cmp );
	if( iter < _currMapEnd && !( cmp < *iter ) )
		return iter;
	return nullptr;
}

static void _i18nFormatCurrencySign(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, const char* currency, eMoneyType moneyType)
{
	PString currSign;
	const i18nCurrMapItem* currInfo = _i18nFindCurrencyInfo(currency);
 	if( currInfo && !currInfo->sign.isNull() )
 		PMsg(currSign, table, localeId, currInfo->sign);
 	else
		i18nAppend(currSign, currency);

	switch(moneyType)
	{
	case eMoneyTypeCurr:
		i18nAppend(ret, currSign);
		break;
	case eMoneyTypeT:
		PMsg1(ret, table, localeId, P_TXT( TXTINT_TN0 ), currSign);
		break;
	case eMoneyTypeW:
		PMsg1(ret, table, localeId, P_TXT( TXTINT_WN0 ), currSign);
		break;
	default:
		PASSERT5(0);
		i18nAppend(ret, currSign);
		break;
	}
}

static void _i18nFormatCurrencyCode(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, const char* currency, eMoneyType moneyType)
{
	PString currCode;
 	const i18nCurrMapItem* currInfo = _i18nFindCurrencyInfo(currency);
 	if( currInfo && !currInfo->code.isNull() )
 		PMsg(currCode, table, localeId, currInfo->code);
 	else
		i18nAppend(currCode, currency);

	switch(moneyType)
	{
	case eMoneyTypeCurr:
		i18nAppend(ret, currCode);
		break;
	case eMoneyTypeT:
		PMsg1(ret, table, localeId, P_TXT( TXTINT_Format_Money_T_Code ), currCode);
		break;
	case eMoneyTypeW:
		PMsg1(ret, table, localeId, P_TXT( TXTINT_Format_Money_W_Code ), currCode);
		break;
	default:
		PASSERT5(0);
		i18nAppend(ret, currCode);
		break;
	}
}

static void _i18nFormatCurrencyName(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, const char* currency, eMoneyType moneyType)
{
	PString currName;
 	const i18nCurrMapItem* currInfo = _i18nFindCurrencyInfo( currency);
 	if( currInfo && !currInfo->name.isNull() )
 		PMsg(currName, table, localeId, currInfo->name);
 	else
		i18nAppend(currName, currency);

	switch(moneyType)
	{
	case eMoneyTypeCurr:
		i18nAppend(ret, currName);
		break;
	case eMoneyTypeT:
		PMsg1(ret, table, localeId, P_TXT( TXTINT_Format_Money_T_Code ), currName);
		break;
	case eMoneyTypeW:
		PMsg1(ret, table, localeId, P_TXT( TXTINT_Format_Money_W_Code ), currName);
		break;
	default:
		PASSERT5(0);
		i18nAppend(ret, currName);
		break;
	}
}

void i18nFormatCurrency(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, const char* currency, eMoneyType moneyType, eFormatCurrType currType)
{
	switch(currType)
	{
	case eFormatCurrTypeSign:
		{
			_i18nFormatCurrencySign(ret, table, localeId, currency, moneyType);
			break;
		}
	case eFormatCurrTypeCode:
		{
			_i18nFormatCurrencyCode(ret, table, localeId, currency, moneyType);
			break;
		}
	case eFormatCurrTypeSignWithCode:
		{
			_i18nFormatCurrencyCode(ret, table, localeId, currency, moneyType);
			_i18nFormatCurrencySign(ret, table, localeId, currency, moneyType);
			break;
		}
	case eFormatCurrTypeName:
		{
			_i18nFormatCurrencyName(ret, table, localeId, currency, moneyType);
			break;
		}
	default:
		{
			PASSERT5(0);
			_i18nFormatCurrencySign(ret, table, localeId, currency, moneyType);
			break;
		}
	}
}

void i18nFormatMoneyWithCurrency(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, UINT64 cents_, const char* currency, eMoneyType moneyType, eFormatCurrType currType, bool showZeroCents, bool showTriplets)
{
	PString currStr;
	switch(currType)
	{
	case eFormatCurrTypeSign:
		{
			PString currSign;
			_i18nFormatCurrencySign(currSign, table, localeId, currency, moneyType);
			i18nFormatMoneyEx(ret, table, localeId, cents_, currSign, currType, showZeroCents, showTriplets);
			break;
		}
	case eFormatCurrTypeCode:
		{
			PString currCode;
			_i18nFormatCurrencyCode(currCode, table, localeId, currency, moneyType);
			i18nFormatMoneyEx(ret, table, localeId, cents_, currCode, currType, showZeroCents, showTriplets);
			break;
		}
	case eFormatCurrTypeSignWithCode:
		{
			PString currSign;
			PString currCode;
			_i18nFormatCurrencySign(currSign, table, localeId, currency, moneyType);
			PString moneyStr;
			i18nFormatMoneyEx(moneyStr, table, localeId, cents_, currSign, eFormatCurrTypeSign, showZeroCents, showTriplets);
			_i18nFormatCurrencyCode(currCode, table, localeId, currency, moneyType);
			i18nFormatMoneyEx(ret, table, localeId, currCode, eFormatCurrTypeSignWithCode, moneyStr);
			break;
		}
	case eFormatCurrTypeName:
		{
			PString currName;
			_i18nFormatCurrencyName(currName, table, localeId, currency, moneyType);
			i18nFormatMoneyEx(ret, table, localeId, cents_, currName, currType, showZeroCents, showTriplets);
			break;
		}
	default:
		{
			PASSERT5(0);
			PString currSign;
			_i18nFormatCurrencySign(currSign, table, localeId, currency, moneyType);
			i18nFormatMoneyEx(ret, table, localeId, cents_, currSign, currType, showZeroCents, showTriplets);
			break;
		}
	}
}

void i18nFormatWeekday(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, int idx /* 0 = Monday, 1 = Tuesday... etc */, bool shortForm)
{
	static const PMsgId weekDay[] =
	{
		P_TXT( TXTINT_Monday ),
		P_TXT( TXTINT_Tuesday ),
		P_TXT( TXTINT_Wednesday ),
		P_TXT( TXTINT_Thursday ),
		P_TXT( TXTINT_Friday ),
		P_TXT( TXTINT_Saturday ),
		P_TXT( TXTINT_Sunday ),
		P_TXT( TXTINT_Mon ),
		P_TXT( TXTINT_Tue ),
		P_TXT( TXTINT_Wed ),
		P_TXT( TXTINT_Thu ),
		P_TXT( TXTINT_Fri ),
		P_TXT( TXTINT_Sat ),
		P_TXT( TXTINT_Sun ),
	};
	PASSERT5(0 <= idx && idx < 7);
	if (0 <= idx && idx < 7)
	{
		if (shortForm)
			idx += 7;
		PMsg(ret, table, localeId, weekDay[idx]);
	}
}

void i18nFormatMonth(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, int idx /* 0 = January, 1 = February... etc */, bool shortForm)
{
	static const PMsgId monthName[] =
	{
		P_TXT( TXTINT_January ),
		P_TXT( TXTINT_February ),
		P_TXT( TXTINT_March ),
		P_TXT( TXTINT_April ),
		P_TXT( TXTINT_May_1 ), // long tag
		P_TXT( TXTINT_June ),
		P_TXT( TXTINT_July ),
		P_TXT( TXTINT_August ),
		P_TXT( TXTINT_September ),
		P_TXT( TXTINT_October ),
		P_TXT( TXTINT_November ),
		P_TXT( TXTINT_December ),
		P_TXT( TXTINT_Jan ),
		P_TXT( TXTINT_Feb ),
		P_TXT( TXTINT_Mar ),
		P_TXT( TXTINT_Apr ),
		P_TXT( TXTINT_May ), // short tag
		P_TXT( TXTINT_Jun ),
		P_TXT( TXTINT_Jul ),
		P_TXT( TXTINT_Aug ),
		P_TXT( TXTINT_Sep ),
		P_TXT( TXTINT_Oct ),
		P_TXT( TXTINT_Nov ),
		P_TXT( TXTINT_Dec ),
	};
	PASSERT5(0 <= idx && idx < 12);
	if (0 <= idx && idx < 12) 
	{
		if (shortForm)
			idx += 12;
		PMsg(ret, table, localeId, monthName[idx]);
	}
}

void i18nFormatQuarter(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, int idx /* 0 = January, 1 = February... etc */)
{
	static const PMsgId quarterName[] =
	{
		P_TXT( TXTINT_Winter ),
		P_TXT( TXTINT_Winter ),
		P_TXT( TXTINT_Winter ),
		P_TXT( TXTINT_Spring ),
		P_TXT( TXTINT_Spring ),
		P_TXT( TXTINT_Spring ),
		P_TXT( TXTINT_Summer ),
		P_TXT( TXTINT_Summer ),
		P_TXT( TXTINT_Summer ),
		P_TXT( TXTINT_Fall ),
		P_TXT( TXTINT_Fall ),
		P_TXT( TXTINT_Fall ),
	};
	PASSERT5(0 <= idx && idx < 12);
	if (0 <= idx && idx < 12) 
	{
		PMsg(ret, table, localeId, quarterName[idx]);
	}
}

void i18nFormatLongDate(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, const SrvDate& date, bool shortMonth, bool showYear)
{
	char year[16], day[16];
	p_utoa(date._year, year);
	p_utoa(date._day, day);
	PString month;
	i18nFormatMonth(month, table, localeId, date._month - 1, shortMonth);
	if (showYear)
		PMsg3(ret, table, localeId, P_TXT( TXTINT_FORMAT_LongDate ), month, day, year);
	else
		PMsg2(ret, table, localeId, P_TXT( TXTINT_FORMAT_LongDateNoYear ), month, day);
}

void i18nFormatLongDateTZ(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, const SrvTime& time, UINT32 tzfDest, bool shortMonth, bool showYear)
{
	SrvTime timeDest;
	tzLocalToLocalSrvTime(time, eTzET, -1, timeDest, getTimezone(tzfDest));
	SrvDate dateDest(timeDest);
	i18nFormatLongDate(ret, table, localeId, dateDest, shortMonth, showYear);
}

void i18nFormatLongDateTZ(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, time_t time, UINT32 tzfDest, bool shortMonth, bool showYear)
{
	SrvTime timeDest;
	tzUtcToLocalSrvTime(time, timeDest, getTimezone(tzfDest));
	SrvDate dateDest(timeDest);
	i18nFormatLongDate(ret, table, localeId, dateDest, shortMonth, showYear);
}

void i18nFormatLocaleName(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, int idx)
{
	//array from i18n.localedef.h
	SUPPORTED_LOCALES_TAGS

	PASSERT5(0 <= idx && idx < sizeof(localeName)/sizeof(localeName[0]));
	if (0 <= idx && idx < sizeof(localeName)/sizeof(localeName[0]))
		PMsg(ret, table, localeId, localeName[idx]);
}

void i18nFormatNth(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, int num)
{
	// we assume "teen" rule for all locales, as it is true even for Russian 
	int teen = num % 100;
	if (teen > 10 && teen < 20)
	{
		char str[32];
		PMsg1(ret, table, localeId, P_TXT( TXTINT_FORMAT_Nth ), p_itoa(num, str));
		return;
	}
	int n = num / 10;
	if (n > 0)
	{
		char str[32];
		i18nAppend(ret, p_itoa(n, str));
	}
	switch (num % 10)
	{
		case 1:
			PMsg(ret, table, localeId, P_TXT( TXTINT_FORMAT_1st ));
			break;
		case 2:
			PMsg(ret, table, localeId, P_TXT( TXTINT_FORMAT_2nd ));
			break;
		case 3:
			PMsg(ret, table, localeId, P_TXT( TXTINT_FORMAT_3rd ));
			break;
		default:
		{
			char str[4];
			PMsg1(ret, table, localeId, P_TXT( TXTINT_FORMAT_Nth ), p_itoa(num % 10, str));
			break;
		}
	}
}

void i18nFormatBrandedName( PString& ret, const PMsgLocaleTable& table, unsigned int localeId, unsigned int brandId, eBrandedNames nameId, bool isCapital )
{
	switch( nameId )
	{
	case eBrandedName_Brand:
		switch( brandId )
		{
		case BrandType_StarsCasino:
			PMsg( ret, table, localeId, isCapital ? P_TXT( TXTINT_BRAND_CASINO ) : P_TXT( TXTINT_BRAND_Casino ) );
			break;

		case BrandType_BetStars:
			PMsg( ret, table, localeId, isCapital ? P_TXT( TXTINT_BRAND_BETSTARS ) : P_TXT( TXTINT_BRAND_BetStars ) );
			break;

		case BrandType_StarsDraft:
			PMsg( ret, table, localeId, isCapital ? P_TXT( TXTINT_BRAND_STARSDRAFT ) : P_TXT( TXTINT_BRAND_StarsDraft ) );
			break;

		case BrandType_JackPotPoker:
			PMsg( ret, table, localeId, isCapital ? P_TXT( TXTINT_BRAND_JACKPOT ) : P_TXT( TXTINT_BRAND_Jackpot ) );
			break;

		case BrandType_FullTilt:
			PMsg( ret, table, localeId, isCapital ? P_TXT( TXTINT_BRAND_FULL_TILT ) : P_TXT( TXTINT_BRAND_Full_Tilt ) );
			break;

		case BrandType_Unknown:
		case BrandType_ALLMask:
			PMsg( ret, table, localeId, isCapital ? P_TXT( TXTINT_BRAND_STARS_ACCOUNT ) : P_TXT( TXTINT_BRAND_Stars_Account ) );
			break;

		case BrandType_PokerStarsVegas:
			PMsg( ret, table, localeId, isCapital ? P_TXT( TXTINT_BRAND_POKERSTARSVEGAS ) : P_TXT( TXTINT_BRAND_PokerStarsVegas ) );
			break;

		case BrandType_SkyBet:
			PMsg(ret, table, localeId, isCapital ? P_TXT(TXTINT_BRAND_SKY_BET) : P_TXT(TXTINT_BRAND_Sky_Bet));
			break;

		case BrandType_SkyPoker:
			PMsg(ret, table, localeId, isCapital ? P_TXT(TXTINT_BRAND_SKY_POKER) : P_TXT(TXTINT_BRAND_Sky_Poker));
			break;

		case BrandType_SkyCasino:
			PMsg(ret, table, localeId, isCapital ? P_TXT(TXTINT_BRAND_SKY_CASINO) : P_TXT(TXTINT_BRAND_Sky_Casino));
			break;

		case BrandType_SkyVegas:
			PMsg(ret, table, localeId, isCapital ? P_TXT(TXTINT_BRAND_SKY_VEGAS) : P_TXT(TXTINT_BRAND_Sky_Vegas));
			break;

		case BrandType_FoxBet:
			PMsg(ret, table, localeId, isCapital ? P_TXT(TXTINT_BRAND_FOX_BET) : P_TXT(TXTINT_BRAND_FOX_Bet));
			break;

		default:
			PASSERT5( BrandType_PokerStars == brandId );
			PMsg( ret, table, localeId, isCapital ? P_TXT( TXTINT_BRAND_POKERSTARS ) : P_TXT( TXTINT_BRAND_PokerStars ) );
		}
		break;

	case eBrandedName_PokerBrand:
		{
			if( brandId == BrandType_FullTilt )
				PMsg( ret, table, localeId, isCapital ? P_TXT( TXTINT_BRAND_FULL_TILT ) : P_TXT( TXTINT_BRAND_Full_Tilt ) );
			else
				PMsg( ret, table, localeId, isCapital ?  P_TXT( TXTINT_BRAND_POKERSTARS ) : P_TXT( TXTINT_BRAND_PokerStars ) );
		}
		break;

	case eBrandedName_CasinoBrand:
		{
			if( brandId == BrandType_FullTilt )
				PMsg( ret, table, localeId, isCapital ? P_TXT( TXTINT_BRAND_FULL_TILT_CASINO ) : P_TXT( TXTINT_BRAND_Full_Tilt_Casino ) );
			else
				PMsg( ret, table, localeId, isCapital ?  P_TXT( TXTINT_BRAND_POKERSTARS_CASINO ) : P_TXT( TXTINT_BRAND_PokerStars_Casino ) );
		}
		break;

	case eBrandedName_SpinGo:
		PMsg( ret, table, localeId, isCapital ? P_TXT( TXTINT_BRAND_SPIN_X_GO ) : P_TXT( TXTINT_BRAND_Spin_X_Go ) );
		break;

	case eBrandedName_Zoom:
		PMsg( ret, table, localeId, isCapital ? P_TXT( TXTINT_BRAND_ZOOM ) : P_TXT( TXTINT_BRAND_Zoom ) );
		break;
	}
}

// *IMPORTANT*:	please use an overloaded implementations of these function that uses a "const IFormatErrorAction& action" last parameter
//				see an example in the lobby. the new implementation sends the information about a generated internal error to a DB so we
//				can collect statistics and also the client can map such errors on CSR defined overrides.
//
void i18nFormatInternalError(PString& ret, const PMsgLocaleTable& table, unsigned int localeId,  INT16 commonErrorCode, int errorLocationMark, bool addDefaultLocale /*= true*/)
{
	PString strCommonErr;
	PString strLocation;
	strCommonErr.appendInt(commonErrorCode);
	strLocation.appendInt(errorLocationMark);
	PMsg2(ret, table, localeId, P_TXT( TXTINT_INTERNAL_ERR ), strCommonErr, strLocation);
	if( addDefaultLocale && localeId != LOCALE_DEFAULT )
	{
		PString tmp;
		PMsg2(tmp, table, LOCALE_DEFAULT, P_TXT( TXTINT_INTERNAL_ERR ), strCommonErr, strLocation);
		i18nAppendChar(tmp, '\n');
		i18nAppend(tmp, ret);
		ret = tmp;
	}
	PLog("formatInternalError ... Internal Error (%d/%d)", (int) commonErrorCode, errorLocationMark);
}

// PYR-35048
void i18nFormatInternalErrorNamespace(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, INT16 commonErrorCode, int errorLocationMark, ErrorOrigin errorNameSpace, bool addDefaultLocale /*= true*/)
{
	if (errorNameSpace == eErrorOriginDefaultPokerstars)
	{
		i18nFormatInternalError(ret, table, localeId, commonErrorCode, errorLocationMark, addDefaultLocale);
		return;
	}
	PString strCommonErr;
	PString strLocation;
	PString strNamespace;
	strCommonErr.appendInt(commonErrorCode);
	strLocation.appendInt(errorLocationMark);
	strNamespace.appendUint(static_cast<UINT32>(errorNameSpace));
	PMsg3(ret, table, localeId, P_TXT( TXTINT_Internal_Error_XN0_N1_N2XX__Pl_ELL ), strCommonErr, strLocation, strNamespace);
	if( addDefaultLocale && localeId != LOCALE_DEFAULT )
	{
		PString tmp;
		PMsg3(tmp, table, LOCALE_DEFAULT, P_TXT( TXTINT_Internal_Error_XN0_N1_N2XX__Pl_ELL ), strCommonErr, strLocation, strNamespace);
		i18nAppendChar(tmp, '\n');
		i18nAppend(tmp, ret);
		ret = tmp;
	}
	PLog("formatInternalErrorNamespace ... Internal Error (%d/%d/%d)", (int) commonErrorCode, errorLocationMark, errorNameSpace);
}

// *IMPORTANT*:	please use an overloaded implementations of these function that uses a "const IFormatErrorAction& action" last parameter
//				see an example in the lobby. the new implementation sends the information about a generated internal error to a DB so we
//				can collect statistics and also the client can map such errors on CSR defined overrides.
//
void i18nFormatInternalErrorEx( PString& ret, const PMsgLocaleTable& table, unsigned int localeId, INT16 commonErrorCode, int errorLocationMark, const char* domain, bool addDefaultLocale /*= true*/ )
{
	PString strCommonErr;
	PString strLocation;
	strCommonErr.appendInt(commonErrorCode);
	strLocation.appendInt(errorLocationMark);
	PMsg3(ret, table, localeId, P_TXT( TXTINT_Internal_Error_XN0_N1XX_Please_ELL ), strCommonErr, strLocation, domain);
	if( addDefaultLocale && localeId != LOCALE_DEFAULT )
	{
		PString tmp;
		PMsg3(tmp, table, LOCALE_DEFAULT, P_TXT( TXTINT_Internal_Error_XN0_N1XX_Please_ELL ), strCommonErr, strLocation, domain);
		i18nAppendChar(tmp, '\n');
		i18nAppend(tmp, ret);
		ret = tmp;
	}
	PLog("formatInternalErrorEx ... Internal Error (%d/%d) '%s'", (int) commonErrorCode, errorLocationMark, domain);
}

// PYR-35048
void i18nFormatInternalErrorExNamespace( PString& ret, const PMsgLocaleTable& table, unsigned int localeId, INT16 commonErrorCode, int errorLocationMark, ErrorOrigin errorNameSpace, const char* domain, bool addDefaultLocale /*= true*/ )
{
	if (errorNameSpace == eErrorOriginDefaultPokerstars)
	{
		i18nFormatInternalErrorEx(ret, table, localeId, commonErrorCode, errorLocationMark, domain, addDefaultLocale);
		return;
	}
	PString strCommonErr;
	PString strLocation;
	PString strNamespace;
	strCommonErr.appendInt(commonErrorCode);
	strLocation.appendInt(errorLocationMark);
	strNamespace.appendUint(static_cast<UINT32>(errorNameSpace));
	PMsg4(ret, table, localeId, P_TXT( TXTINT_Internal_Error_XN0_N1_N2XX_Ple_ELL ), strCommonErr, strLocation, strNamespace, domain);
	if( addDefaultLocale && localeId != LOCALE_DEFAULT )
	{
		PString tmp;
		PMsg4(tmp, table, LOCALE_DEFAULT, P_TXT( TXTINT_Internal_Error_XN0_N1_N2XX_Ple_ELL ), strCommonErr, strLocation, strNamespace, domain);
		i18nAppendChar(tmp, '\n');
		i18nAppend(tmp, ret);
		ret = tmp;
	}
	PLog("formatInternalErrorExNamespace ... Internal Error (%d/%d/%d) '%s'", (int) commonErrorCode, errorLocationMark, errorNameSpace, domain);
}

//-----------------------------------------------------------------------------
#ifndef PROTOCOL_CLIENT_ONLY
//-----------------------------------------------------------------------------

void i18nFormatInternalError(PString& ret, const PMsgLocaleTable& table, unsigned int localeId,  INT16 commonErrorCode, int errorLocationMark, const IFormatErrorAction& action )
{
	i18nFormatInternalError(ret, table, localeId, commonErrorCode, errorLocationMark);
	action.doAction( localeId, commonErrorCode, errorLocationMark );
}

// PYR-35048
void i18nFormatInternalErrorNamespace(PString& ret, const PMsgLocaleTable& table, unsigned int localeId,  INT16 commonErrorCode, int errorLocationMark, ErrorOrigin errorNameSpace, const IFormatErrorAction& action )
{
	if (errorNameSpace != eErrorOriginDefaultPokerstars)
	{
		i18nFormatInternalErrorNamespace(ret, table, localeId, commonErrorCode, errorLocationMark, errorNameSpace);
		action.doAction( localeId, commonErrorCode, errorLocationMark, static_cast<UINT32>(errorNameSpace) );
	}
	else
	{
		i18nFormatInternalError(ret, table, localeId, commonErrorCode, errorLocationMark);
		action.doAction( localeId, commonErrorCode, errorLocationMark );
	}
}

void i18nFormatInternalErrorEx( PString& ret, const PMsgLocaleTable& table, unsigned int localeId, INT16 commonErrorCode, int errorLocationMark, const char* domain, const IFormatErrorAction& action )
{
	i18nFormatInternalErrorEx(ret, table, localeId, commonErrorCode, errorLocationMark, domain);
	action.doAction( localeId, commonErrorCode, errorLocationMark );
}

// PYR-35048
void i18nFormatInternalErrorExNamespace( PString& ret, const PMsgLocaleTable& table, unsigned int localeId, INT16 commonErrorCode, int errorLocationMark, ErrorOrigin errorNameSpace, const char* domain, const IFormatErrorAction& action )
{
	if (errorNameSpace != eErrorOriginDefaultPokerstars)
	{
		i18nFormatInternalErrorExNamespace(ret, table, localeId, commonErrorCode, errorLocationMark, errorNameSpace, domain);
		action.doAction( localeId, commonErrorCode, errorLocationMark, static_cast<UINT32>(errorNameSpace) );
	}
	else
	{
		i18nFormatInternalErrorEx(ret, table, localeId, commonErrorCode, errorLocationMark, domain);
		action.doAction( localeId, commonErrorCode, errorLocationMark );
	}
}

//-----------------------------------------------------------------------------
#endif // PROTOCOL_CLIENT_ONLY
//-----------------------------------------------------------------------------
