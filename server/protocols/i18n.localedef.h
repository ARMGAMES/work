#ifndef i18n_localedef_h_included
#define i18n_localedef_h_included

#define LOCALE_DEFAULT					0			// Backward-compatible formatting logic is allowed for this locale
#define LOCALE_INVALID					0xDEEDBEEF

#define LOCALE_EN		LOCALE_DEFAULT
#define LOCALE_EN_GB	(LOCALE_DEFAULT + 1)
#define LOCALE_EN_US	(LOCALE_DEFAULT + 2)
#define LOCALE_FR		(LOCALE_DEFAULT + 3)
#define LOCALE_DE		(LOCALE_DEFAULT + 4)
#define LOCALE_ES		(LOCALE_DEFAULT + 5)
#define LOCALE_RU		(LOCALE_DEFAULT + 6)
#define LOCALE_PT		(LOCALE_DEFAULT + 7)
#define LOCALE_NL		(LOCALE_DEFAULT + 8)
#define LOCALE_IT		(LOCALE_DEFAULT + 9)
#define LOCALE_PL		(LOCALE_DEFAULT + 10)
#define LOCALE_HU		(LOCALE_DEFAULT + 11)
#define LOCALE_DA		(LOCALE_DEFAULT + 12)
#define LOCALE_SV		(LOCALE_DEFAULT + 13)
#define LOCALE_FI		(LOCALE_DEFAULT + 14)
#define LOCALE_NO		(LOCALE_DEFAULT + 15)
#define LOCALE_JA		(LOCALE_DEFAULT + 16)
#define LOCALE_ZH_CN	(LOCALE_DEFAULT + 17)
#define LOCALE_ZH_TW	(LOCALE_DEFAULT + 18)
#define LOCALE_BG		(LOCALE_DEFAULT + 19)
#define LOCALE_CS		(LOCALE_DEFAULT + 20)
#define LOCALE_RO		(LOCALE_DEFAULT + 21)
#define LOCALE_SL		(LOCALE_DEFAULT + 22)
#define LOCALE_UK		(LOCALE_DEFAULT + 23)
#define LOCALE_EL		(LOCALE_DEFAULT + 24)
#define LOCALE_ET		(LOCALE_DEFAULT + 25)
#define LOCALE_LT		(LOCALE_DEFAULT + 26)
#define LOCALE_LV		(LOCALE_DEFAULT + 27)
#define LOCALE_PT_BR	(LOCALE_DEFAULT + 28)
#define LOCALE_HR		(LOCALE_DEFAULT + 29)
#define LOCALE_KO		(LOCALE_DEFAULT + 30)
#define LOCALE_ES_419	(LOCALE_DEFAULT + 31)
#define LOCALE_SK		(LOCALE_DEFAULT + 32)
#define LOCALE_EN_IN	(LOCALE_DEFAULT + 33)

#define MAX_LOCALES		LOCALE_EN_IN				// last assigned locale


#define CLIENT_LOCALES_ORDER \
enum\
{\
	LORDER_EN,\
	LORDER_CS,\
	LORDER_DA,\
	LORDER_DE,\
	LORDER_EL,\
	LORDER_ES,\
	LORDER_ET,\
	LORDER_FR,\
	LORDER_HR,\
	LORDER_IT,\
	LORDER_LV,\
	LORDER_LT,\
	LORDER_HU,\
	LORDER_NL,\
	LORDER_NO,\
	LORDER_PL,\
	LORDER_PT,\
	LORDER_RO,\
	LORDER_SL,\
	LORDER_SK,\
	LORDER_SV,\
	LORDER_FI,\
	LORDER_BG,\
	LORDER_RU,\
	LORDER_UK,\
	LORDER_JA,\
	LORDER_ZH,\
	LORDER_KO,\
	LORDER_MAX,\
};

#define SUPPORTED_LOCALES_TABLE \
static const PMsgLocale supportedLocales[] = \
{\
	{ LOCALE_EN,	"English",					"en",		"",		&file1252,	LORDER_EN,	LFLAG_PRODUCTION																},\
	{ LOCALE_EN,	"English (United Kingdom)",	"en-GB",	"",		&file1252,	LORDER_EN,	LFLAG_HIDDEN																	},\
	{ LOCALE_EN,	"English (United States)",	"en-US",	"",		&file1252,	LORDER_EN,	LFLAG_HIDDEN																	},\
	{ LOCALE_EN,	"French",					"fr",		"",		&file1252,	LORDER_FR,	LFLAG_PRODUCTION																},\
	{ LOCALE_EN,	"German",					"de",		"",		&file1252,	LORDER_DE,	LFLAG_PRODUCTION																},\
	{ LOCALE_EN,	"Spanish",					"es",		"",		&file1252,	LORDER_ES,	LFLAG_PRODUCTION																},\
	{ LOCALE_EN,	"Russian",					"ru",		"",		&fileUTF8,	LORDER_RU,	LFLAG_PRODUCTION							| LFLAG_UTF8						},\
	{ LOCALE_EN,	"Portuguese",				"pt",		"",		&file1252,	LORDER_PT,	LFLAG_PRODUCTION																},\
	{ LOCALE_EN,	"Dutch",					"nl",		"",		&file1252,	LORDER_NL,	LFLAG_PRODUCTION																},\
	{ LOCALE_EN,	"Italian",					"it",		"",		&file1252,	LORDER_IT,	LFLAG_PRODUCTION																},\
	{ LOCALE_EN,	"Polish",					"pl",		"",		&fileUTF8,	LORDER_PL,	LFLAG_PRODUCTION							| LFLAG_UTF8						},\
	{ LOCALE_EN,	"Hungarian",				"hu",		"",		&fileUTF8,	LORDER_HU,	LFLAG_PRODUCTION							| LFLAG_UTF8						},\
	{ LOCALE_EN,	"Danish",					"da",		"",		&file1252,	LORDER_DA,	LFLAG_PRODUCTION																},\
	{ LOCALE_EN,	"Swedish",					"sv",		"",		&file1252,	LORDER_SV,	LFLAG_PRODUCTION																},\
	{ LOCALE_EN,	"Finnish",					"fi",		"",		&file1252,	LORDER_FI,	LFLAG_PRODUCTION																},\
	{ LOCALE_EN,	"Norwegian",				"no",		"",		&file1252,	LORDER_NO,	LFLAG_PRODUCTION																},\
	{ LOCALE_EN,	"Japanese",					"ja",		"",		&fileUTF8,	LORDER_JA,	LFLAG_PRODUCTION	| LFLAG_NO_SUPPORT		| LFLAG_UTF8	| LFLAG_LOGOGRAPHIC	},\
	{ LOCALE_EN,	"Simplified Chinese",		"zh-CN",	"",		&fileUTF8,	LORDER_ZH,	LFLAG_PRODUCTION							| LFLAG_UTF8	| LFLAG_LOGOGRAPHIC	},\
	{ LOCALE_EN,	"Traditional Chinese",		"zh-TW",	"",		&fileUTF8,	LORDER_ZH,	LFLAG_PRODUCTION							| LFLAG_UTF8	| LFLAG_LOGOGRAPHIC	},\
	{ LOCALE_EN,	"Bulgarian",				"bg",		"",		&fileUTF8,	LORDER_BG,	LFLAG_PRODUCTION							| LFLAG_UTF8						},\
	{ LOCALE_EN,	"Czech",					"cs",		"",		&fileUTF8,	LORDER_CS,	LFLAG_PRODUCTION							| LFLAG_UTF8						},\
	{ LOCALE_EN,	"Romanian",					"ro",		"",		&fileUTF8,	LORDER_RO,	LFLAG_PRODUCTION							| LFLAG_UTF8						},\
	{ LOCALE_EN,	"Slovenian",				"sl",		"",		&fileUTF8,	LORDER_SL,	LFLAG_PRODUCTION							| LFLAG_UTF8						},\
	{ LOCALE_EN,	"Ukrainian",				"uk",		"",		&fileUTF8,	LORDER_UK,	LFLAG_PRODUCTION							| LFLAG_UTF8						},\
	{ LOCALE_EN,	"Greek",					"el",		"",		&fileUTF8,	LORDER_EL,	LFLAG_PRODUCTION							| LFLAG_UTF8						},\
	{ LOCALE_EN,	"Estonian",					"et",		"",		&fileUTF8,	LORDER_ET,	LFLAG_PRODUCTION							| LFLAG_UTF8						},\
	{ LOCALE_EN,	"Lithuanian",				"lt",		"",		&fileUTF8,	LORDER_LT,	LFLAG_PRODUCTION							| LFLAG_UTF8						},\
	{ LOCALE_EN,	"Latvian",					"lv",		"",		&fileUTF8,	LORDER_LV,	LFLAG_PRODUCTION							| LFLAG_UTF8						},\
	{ LOCALE_PT,	"Portuguese (Brazil)",		"pt-BR",	"",		&file1252,	LORDER_PT,	LFLAG_PRODUCTION																},\
	{ LOCALE_EN,	"Croatian",					"hr",		"",		&fileUTF8,	LORDER_HR,	LFLAG_PRODUCTION							| LFLAG_UTF8						},\
	{ LOCALE_EN,	"Korean",					"ko",		"",		&fileUTF8,	LORDER_KO,	LFLAG_PRODUCTION	| LFLAG_NO_SUPPORT		| LFLAG_UTF8	| LFLAG_LOGOGRAPHIC	},\
	{ LOCALE_ES,	"Spanish (Latin America)",	"es-419",	"",		&file1252,	LORDER_ES,	LFLAG_PRODUCTION																},\
	{ LOCALE_EN,	"Slovak",					"sk",		"",		&fileUTF8,	LORDER_SK,	LFLAG_HIDDEN								| LFLAG_UTF8						},\
	{ LOCALE_EN,	"English (India)",			"en-IN",	"",		&file1252,	LORDER_EN,	LFLAG_PRODUCTION																},\
	{ 0,			NULL,						NULL,		"",		&file1252,	LORDER_MAX,	0																				},\
};

#define SUPPORTED_LOCALES_TAGS \
static const PMsgId localeName[] =\
{\
	P_TXT( TXTINT_Lang_English ),\
	P_TXT( TXTINT_Lang_English_UK ),\
	P_TXT( TXTINT_Lang_English_US ),\
	P_TXT( TXTINT_Lang_French ),\
	P_TXT( TXTINT_Lang_German ),\
	P_TXT( TXTINT_Lang_Spanish ),\
	P_TXT( TXTINT_Lang_Russian ),\
	P_TXT( TXTINT_Lang_Portuguese ),\
	P_TXT( TXTINT_Lang_Dutch ),\
	P_TXT( TXTINT_Lang_Italian ),\
	P_TXT( TXTINT_Lang_Polish ),\
	P_TXT( TXTINT_Lang_Hungarian ),\
	P_TXT( TXTINT_Lang_Danish ),\
	P_TXT( TXTINT_Lang_Swedish ),\
	P_TXT( TXTINT_Lang_Finnish ),\
	P_TXT( TXTINT_Lang_Norwegian ),\
	P_TXT( TXTINT_Lang_Japanese ),\
	P_TXT( TXTINT_Lang_Simplified_Chinese ),\
	P_TXT( TXTINT_Lang_Traditional_Chinese ),\
	P_TXT( TXTINT_Lang_Bulgarian ),\
	P_TXT( TXTINT_Lang_Czech ),\
	P_TXT( TXTINT_Lang_Romanian ),\
	P_TXT( TXTINT_Lang_Slovenian ),\
	P_TXT( TXTINT_Lang_Ukrainian ),\
	P_TXT( TXTINT_Lang_Greek ),\
	P_TXT( TXTINT_Lang_Estonian ),\
	P_TXT( TXTINT_Lang_Lithuanian ),\
	P_TXT( TXTINT_Lang_Latvian ),\
	P_TXT( TXTINT_Lang_Portuguese_XBrazilX ),\
	P_TXT( TXTINT_Lang_Croatian ),\
	P_TXT( TXTINT_Lang_Korean ),\
	P_TXT( TXTINT_Lang_Spanish_419 ),\
	P_TXT( TXTINT_Lang_Slovak ),\
	P_TXT( TXTINT_Lang_English_IN ),\
};

#define SUPPORTED_LOCALES_TAGS_INS \
static const PMsgId localeName[] =\
{\
	P_TXT( TXTINS_Lang_English ),\
	P_TXT( TXTINS_Lang_English_UK ),\
	P_TXT( TXTINS_Lang_English_US ),\
	P_TXT( TXTINS_Lang_French ),\
	P_TXT( TXTINS_Lang_German ),\
	P_TXT( TXTINS_Lang_Spanish ),\
	P_TXT( TXTINS_Lang_Russian ),\
	P_TXT( TXTINS_Lang_Portuguese ),\
	P_TXT( TXTINS_Lang_Dutch ),\
	P_TXT( TXTINS_Lang_Italian ),\
	P_TXT( TXTINS_Lang_Polish ),\
	P_TXT( TXTINS_Lang_Hungarian ),\
	P_TXT( TXTINS_Lang_Danish ),\
	P_TXT( TXTINS_Lang_Swedish ),\
	P_TXT( TXTINS_Lang_Finnish ),\
	P_TXT( TXTINS_Lang_Norwegian ),\
	P_TXT( TXTINS_Lang_Japanese ),\
	P_TXT( TXTINS_Lang_Simplified_Chinese ),\
	P_TXT( TXTINS_Lang_Traditional_Chinese ),\
	P_TXT( TXTINS_Lang_Bulgarian ),\
	P_TXT( TXTINS_Lang_Czech ),\
	P_TXT( TXTINS_Lang_Romanian ),\
	P_TXT( TXTINS_Lang_Slovenian ),\
	P_TXT( TXTINS_Lang_Ukrainian ),\
	P_TXT( TXTINS_Lang_Greek ),\
	P_TXT( TXTINS_Lang_Estonian ),\
	P_TXT( TXTINS_Lang_Lithuanian ),\
	P_TXT( TXTINS_Lang_Latvian ),\
	P_TXT( TXTINS_Lang_Portuguese_XBrazilX ),\
	P_TXT( TXTINS_Lang_Croatian ),\
	P_TXT( TXTINS_Lang_Korean ),\
	P_TXT( TXTINS_Lang_Spanish_419 ),\
	P_TXT( TXTINS_Lang_Slovak ),\
	P_TXT( TXTINS_Lang_English_IN ),\
};

#endif // i18n_localedef_h_included
