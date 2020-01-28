//------------------------------------------------------------------------------

#ifndef seatmanstruct_cli_h_included
#define seatmanstruct_cli_h_included

//------------------------------------------------------------------------------

#include "commmsgbody.h"

//------------------------------------------------------------------------------

enum eSeatManSpeed
{
	eSeatManSpeed_Fast = 1,
	eSeatManSpeed_Normal = (1 << 1),

	// !!! keep eSeatManSpeed_Last up to date
	eSeatManSpeed_Last = eSeatManSpeed_Normal,
};

//------------------------------------------------------------------------------

enum eSeatManCurrency
{
	eSeatManCurrency_UNK = 0,	//unknown
	eSeatManCurrency_PM = 1,
	//eSeatManCurrency_FPP = (1 << 1),	//reserved
	eSeatManCurrency_USD = (1 << 2),
	eSeatManCurrency_EUR = (1 << 3),
	eSeatManCurrency_GBP = (1 << 4),
	eSeatManCurrency_CAD = (1 << 5),
	eSeatManCurrency_INR = (1 << 6), // PYR-30558
	// !!! maximum value is (1 << 7) (see enum QuickStartTournFlags)
	// !!! keep eSeatManCurrency_Last up to date

	eSeatManCurrency_Last = eSeatManCurrency_INR,
};

//update mask when changing eSeatManCurrency
#define SEAT_MAN_CURRENCY_MASK 0x3F

//------------------------------------------------------------------------------

enum eSeatManFlags
{
	eSeatManFlags_None = 0,
	eSeatManFlags_AddToWaitingList = (1 << 0),
	eSeatManFlags_AddToInterestList = (1 << 1),
	eSeatManFlags_DecrementingSearch = (1 << 2),

	// !!! Warning - DO NOT forget to change eSeatManFlags_Last when adding new enum members
	eSeatManFlags_Last = eSeatManFlags_DecrementingSearch,
};

//------------------------------------------------------------------------------

enum QuickStartTournFlags
{
	//start with offset to allow currency - eSeatManCurrency
	qsftRegular =	(1 << 8),
	qsftRebuy =		(1 << 9),
	qsftKnockout =	(1 << 10),
	qsftFifty50 =	(1 << 11),
	qsftBTC =		(1 << 12),

	qsftSpeedDisplayRegular =	(1 << 16),
	qsftSpeedDisplayTurbo =		(1 << 17),
	qsftSpeedDisplayHyper =		(1 << 18),
	qsftSpeedDisplaySlow =		(1 << 19),
	qsftSpeedDisplayNone =		(1 << 20),

    // !!! always keep qsftSpeedDisplayLast up to date
	qsftSpeedDisplayLast = qsftSpeedDisplayNone,
};

#define QUICK_START_TOURN_FORMAT_MASK 0xF00
#define QUICK_START_SPEED_DISPLAY_MASK 0x1F0000

//------------------------------------------------------------------------------
#endif //seatmanstruct_cli_h_included
//------------------------------------------------------------------------------
