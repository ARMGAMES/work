#ifndef i18n_localutil_h_included
#define i18n_localutil_h_included

#include "i18n.locales.h"

#include "timezone.h"

enum eFormatCurrType
{
	eFormatCurrTypeSign,
	eFormatCurrTypeCode,
	eFormatCurrTypeSignWithCode,
	eFormatCurrTypeName,
};

enum eMoneyType
{
	eMoneyTypeCurr,
	eMoneyTypeT,
	eMoneyTypeW,
};

enum eBrandedNames
{
	eBrandedName_Brand,
	eBrandedName_PokerBrand,
	eBrandedName_CasinoBrand,
	eBrandedName_SpinGo,
	eBrandedName_Zoom,
};

// Error name spaces
enum ErrorOrigin
{
	eErrorOriginDefaultPokerstars		= 0,	// this is a default for backward compatibility
	eErrorOriginPaymentsCashier			= 1,	// 1-999 ids are for payments(cashier, regulator, gateways and so on)
	eErrorOriginPaymentsAstropay		= 2,
	eErrorOriginPayments3v				= 3,
	eErrorOriginPokerLobby				= 1000, // 1000-1999 ids are for poker(lobby, table and so on)
	eErrorOriginLicense                 = 2000, // 2000-2999 for license PYR-35191
};


extern void i18nFormatShortTimezoneName(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, eTimeZone tz);
extern void i18nFormatFullTimezoneName(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, eTimeZone tz);
extern void i18nFormatDate(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, const SrvDate& date);
extern void i18nFormatDateTZ(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, const SrvTime& time, UINT32 tzfDest);
extern void i18nFormatDateTZ(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, time_t time, UINT32 tzfDest);
extern void i18nFormatTime(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, const SrvTime& time, bool imperial = false);
extern void i18nFormatTimeTZ(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, const SrvTime& time, eTimeZone tz, bool withTZ = false, bool imperial = false);
extern void i18nFormatTimeTZ(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, const SrvTime& time, UINT32 tzf, bool withTZ = false);
extern void i18nFormatTimeTZ(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, time_t time, eTimeZone tz, bool withTZ = false, bool imperial = false);
extern void i18nFormatTimeTZ(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, time_t time, UINT32 tzf, bool withTZ = false);
extern void i18nFormatLongTime(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, const SrvTime& time, bool showMin = true);
extern void i18nFormatDateTime(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, const SrvTime& time, bool imperial = false);
extern void i18nFormatDateTimeTZ(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, const SrvTime& time, eTimeZone tz, bool withTZ = false, bool imperial = false);
extern void i18nFormatDateTimeTZ(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, const SrvTime& time, UINT32 tzf, bool withTZ = false);
extern void i18nFormatDateTimeAltTZ(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, const SrvTime& time, UINT32 tzf);
extern void i18nFormatDateTimeTZ(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, time_t time, eTimeZone tz, bool withTZ = false, bool imperial = false);
extern void i18nFormatDateTimeTZ(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, time_t time, UINT32 tzf, bool withTZ = false);
extern void i18nFormatDateTimeAltTZ(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, time_t time, UINT32 tzf);
extern void i18nFormatDateTimeWithSecTZ(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, const SrvTime& time, UINT32 tzf);
extern void i18nFormatDateTimeISO8601(PString& ret, const SrvTime& srcTime, eTimeZone tzSrc, int isDst, eTimeZone tzDest);
extern void i18nFormatFloat(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, UINT64 number, short places, bool showTrailZeros, bool showTriplets);
extern void i18nFormatMoney(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, UINT64 cents, const char* currSign, bool showZeroCents, bool showTriplets);
extern void i18nFormatMoneyEx(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, const char* currStr, eFormatCurrType currType, const char* moneyStr);
extern void i18nFormatMoneyEx(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, UINT64 cents_, const char* currStr, eFormatCurrType currType, bool showZeroCents, bool showTriplets);
extern void i18nFormatCurrency(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, const char* currency, eMoneyType moneyType, eFormatCurrType currType);
extern void i18nFormatMoneyWithCurrency(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, UINT64 cents_, const char* currency, eMoneyType moneyType, eFormatCurrType currType, bool showZeroCents, bool showTriplets);
extern void i18nFormatWeekday(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, int weekday /* 0 = Monday, 1 = Tuesday... etc */, bool shortForm = false);
extern void i18nFormatMonth(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, int idx /* 0 = January, 1 = February... etc */, bool shortForm = false);
extern void i18nFormatQuarter(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, int idx /* 0 = January, 1 = February... etc */);
extern void i18nFormatLongDate(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, const SrvDate& date, bool shortMonth = false, bool showYear = true);
extern void i18nFormatLongDateTZ(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, const SrvTime& time, UINT32 tzfDest, bool shortMonth = false, bool showYear = true);
extern void i18nFormatLongDateTZ(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, time_t time, UINT32 tzfDest, bool shortMonth = false, bool showYear = true);
extern void i18nFormatLocaleName(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, int currIdx);
extern void i18nFormatNth(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, int num);

extern void i18nFormatBrandedName( PString& ret, const PMsgLocaleTable& table, unsigned int localeId, unsigned int brandId, eBrandedNames nameId, bool isCapital );

//-----------------------------------------------------------------------------
#ifndef PROTOCOL_CLIENT_ONLY
//-----------------------------------------------------------------------------

#include "internalerroraudit.h"

// *IMPORTANT*:	please use an overloaded implementations of these function that uses a "const IFormatErrorAction& action" last parameter
//				see an example in the lobby. the new implementation sends the information about a generated internal error to a DB so we
//				can collect statistics and also the client can map such errors on CSR defined overrides.
//
extern void i18nFormatInternalError(PString& ret, const PMsgLocaleTable& table, unsigned int localeId,  INT16 commonErrorCode, int errorLocationMark, bool addDefaultLocale = true);
extern void i18nFormatInternalErrorEx(PString& ret, const PMsgLocaleTable& table, unsigned int localeId,  INT16 commonErrorCode, int errorLocationMark, const char* domain, bool addDefaultLocale = true);

extern void i18nFormatInternalError(PString& ret, const PMsgLocaleTable& table, unsigned int localeId,  INT16 commonErrorCode, int errorLocationMark, const IFormatErrorAction& action);
extern void i18nFormatInternalErrorEx(PString& ret, const PMsgLocaleTable& table, unsigned int localeId,  INT16 commonErrorCode, int errorLocationMark, const char* domain, const IFormatErrorAction& action);

// PYR-35048
extern void i18nFormatInternalErrorNamespace(PString& ret, const PMsgLocaleTable& table, unsigned int localeId,  INT16 commonErrorCode, int errorLocationMark, ErrorOrigin errorNameSpace, bool addDefaultLocale = true);
extern void i18nFormatInternalErrorExNamespace(PString& ret, const PMsgLocaleTable& table, unsigned int localeId,  INT16 commonErrorCode, int errorLocationMark, ErrorOrigin errorNameSpace, const char* domain, bool addDefaultLocale = true);

extern void i18nFormatInternalErrorNamespace(PString& ret, const PMsgLocaleTable& table, unsigned int localeId,  INT16 commonErrorCode, int errorLocationMark, ErrorOrigin errorNameSpace, const IFormatErrorAction& action);
extern void i18nFormatInternalErrorExNamespace(PString& ret, const PMsgLocaleTable& table, unsigned int localeId,  INT16 commonErrorCode, int errorLocationMark, ErrorOrigin errorNameSpace, const char* domain, const IFormatErrorAction& action);

//-----------------------------------------------------------------------------
#endif // PROTOCOL_CLIENT_ONLY
//-----------------------------------------------------------------------------

// PYR-19814 - even internationalized internal errors will have this tag
//			   so the client could see that it's internal error
//
#define INTERNAL_ERROR_TAG "Internal Error "

#endif // i18n_localutil_h_included
