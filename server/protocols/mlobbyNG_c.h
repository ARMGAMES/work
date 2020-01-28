// ===========================================================
// Copyright (C) 2020 Stars Technologies Services  All rights reserved.
// =========================================================== 

//mlobbyNG_c.h - poker NG mobile lobby support

#ifndef mlobbyNG_c_h_included
#define mlobbyNG_c_h_included

// tournament buy-in options
enum NGBuyInTypes
{
	NGBuyIn_None = 0,
	NGBuyIn_Cash = 1<<0,
	NGBuyIn_FPP = 1<<1,
	NGBuyIn_FreeRoll = 1<<2,
	NGBuyIn_Last = NGBuyIn_FreeRoll, // !!!please keep up to date, update NUM_NG_BUYIN_TYPES when adding/deleting game types
};

#define NUM_NG_BUYIN_TYPES 4

enum NGTournSpeedTypes
{
	NGTournSpeed_Regular	= 1<<0,
	NGTournSpeed_Turbo		= 1<<1,
	NGTournSpeed_Hyper		= 1<<2,
	NGTournSpeed_Slow		= 1<<3,
	NGTournSpeed_Last = NGTournSpeed_Slow, // !!!please keep up to date, update NUM_NG_SPEED_TYPES when adding/deleting game types
};

#define NUM_NG_SPEED_TYPES 4

// Tourn Type
enum NGTournTypeTypes
{
	NGTournType_None	= 0,
	NGTournType_MTT		= 1<<0,
	NGTournType_SNG		= 1<<1,
	NGTournType_Last = NGTournType_SNG, // !!!please keep up to date, update NUM_NG_TYPE_TYPES when adding/deleting tourn types
};

#define NUM_NG_TOURN_TYPES 2

// currencies used for Real Money
enum NGCurrencyTypes
{
	NGCurrency_None = 0,
	NGCurrency_USD = 1<<0, // US dollars
	NGCurrency_EUR = 1<<1, // Euro Currency
	NGCurrency_GBP = 1<<2, // British Pounds
	NGCurrency_CAD = 1<<3, // Canadian Dollars
	NGCurrency_INR = 1<<4, // Indian Rupees
	NGCurrency_Last = NGCurrency_INR, // !!!please keep up to date, update NUM_NG_CURRENCY_TYPES when adding/deleting game types
};

#define NUM_NG_CURRENCY_TYPES 5

// maximum players at the table
enum NGMaxTableSizeTypes
{
	// tournaments
	NGMaxTableSize_TwoMax = 1<<0,
	NGMaxTableSize_ThreeMax = 1<<1,
	NGMaxTableSize_FourMax = 1<<2,
	NGMaxTableSize_FiveMax = 1<<3,
	NGMaxTableSize_SixMax = 1<<4,
	NGMaxTableSize_SevenMax = 1<<5,
	NGMaxTableSize_EightMax = 1<<6,
	NGMaxTableSize_NineMax = 1<<7,
	NGMaxTableSize_TenMax = 1<<8,

	NGMaxTableSize_Last = NGMaxTableSize_TenMax, // !!!please keep up to date, update NUM_NG_TABLE_SIZE_TYPES when adding/deleting game types
};

#define NUM_NG_TABLE_SIZE_TYPES 9


// Tourn Variant
enum NGTournVariantTypes
{
	NGTournVariant_None			= 0,
	NGTournVariant_Regular		= 1<<0,	//default
	NGTournVariant_Rebuy		= 1<<1,
	NGTournVariant_Knockout		= 1<<2,
	NGTournVariant_Shootout		= 1<<3,
	NGTournVariant_Phase		= 1<<4,
	NGTournVariant_Time			= 1<<5,
	NGTournVariant_Zoom			= 1<<6,
	NGTournVariant_Satellite	= 1<<7,
	NGTournVariant_WinTheButton	= 1<<8,
	NGTournVariant_ReEntry		= 1<<9,
	NGTournVariant_NStack		= 1<<10,
	NGTournVariant_SAGMax		= 1<<11, // only used for SAG's
	NGTournVariant_LiveSatellite = 1<<12,
	NGTournVariant_Fifty50		= 1<<13,
	NGTournVariant_SAGFlash		= 1<<14, // only used for SAG's
	NGTournVariant_Last = NGTournVariant_SAGFlash, // !!!please keep up to date, update NUM_NG_VARIANT_TYPES when adding/deleting game types.  You probably also want to modify TournVariantTypes in personalization.h
};

#define NUM_NG_VARIANT_TYPES 15

enum NGTournStateTypes
{
	NGTournState_Completed	= 1<<0,
	NGTournState_Running	= 1<<1,
	NGTournState_LateReg	= 1<<2,
	NGTournState_AllReg		= 1<<3,
	NGTournState_Announced	= 1<<4,
	NGTournState_Last = NGTournState_Announced, // !!!please keep up to date, update NUM_NG_TOURN_STATES when adding/deleting
};


enum NGTournSortTypes
{
	NGTournSort_BuyInAsc = 0,
	NGTournSort_StartTimeAsc = 1,
	NGTournSort_Last = NGTournSort_StartTimeAsc, // !!!please keep up to date
};


enum NGTableSortTypes
{
	NGTableSort_StakesAsc = 0,
	NGTableSort_Last = NGTableSort_StakesAsc, // !!!please keep up to date
};


enum NGTableNumTypes
{
	NGTableNum_HeadToHead = 1<<0,
	NGTableNum_Single = 1<<1,
	NGTableNum_Multi = 1<<2,
	NGTableNum_Last = NGTableNum_Multi, // !!!please keep up to date, update NUM_NG_TABLE_NUM_TYPES when adding/deleting table nums
};

#define NUM_NG_TABLE_NUM_TYPES 3

enum NGFilterTypes
{
	NGFilter_GameType = 1,
	NGFilter_TableSize = 2,
	NGFilter_Speed = 3,
	NGFilter_TableNum = 4,
	NGFilter_Last = NGFilter_TableNum, // !!!please keep up to date
};

enum NGTableHasCapTypes
{
	NGTableHasCap_Yes = 1,
	NGTableHasCap_No = 2,
	NGTableHasCap_Last = NGTableHasCap_No, // !!!please keep up to date
};

enum NGTableHasAnteTypes
{
	NGTableHasAnte_Yes = 1,
	NGTableHasAnte_No = 2,
	NGTableHasAnte_Last = NGTableHasAnte_No, // !!!please keep up to date
};

enum NGTournRestrictedToTypes
{
	RestrictedTo_None = 0,
	RestrictedTo_Male = 1,
	RestrictedTo_Female = 2,
	RestrictedTo_Last = RestrictedTo_Female, // !!!please keep up to date
};

enum NGTournDisplayTypes
{
	NGTournDisplay_None = 0,
	NGTournDisplay_Regular = 1,
	NGTournDisplay_Featured = 2,
	NGTournDisplay_Last = NGTournDisplay_Featured, // !!!please keep up to date
};

// SAG Variant
enum NGSAGFlagTypes
{
	NGSAGFlag_Regular = 0,	//default
	NGSAGFlag_SAGFlash = 1 << 0,
	NGSAGFlag_Last = NGSAGFlag_SAGFlash, // !!!please keep up to date,
};

// Table Variant
enum NGTableVariantTypes
{
	NGTableVariant_None = 0,
	NGTableVariant_Regular = 1 << 0,	//default
	NGTableVariant_Stealth = 1 << 1,
	NGTableVariant_Last = NGTableVariant_Stealth, // !!!please keep up to date, update NUM_NG_TABLE_VARIANT_TYPES when adding/deleting game types.  You probably also want to modify TournVariantTypes in personalization.h
};

#define NUM_NG_TABLE_VARIANT_TYPES 2

#endif	//mlobbyNG_c_h_included
