// gamedefs.h
#ifndef	gamedefs_h_included
#define	gamedefs_h_included

#include "pplib.h"
#include "commonmacros.h"

// moved here from dbm.h
enum HandType 
{
	// uniqueGame type sequence:
	HandTypeNone = 0,
	HandTypeBadugi = 1,
	HandTypeHoldem = 2,
	HandType5Omaha = 3,
	HandTypeOmaha = 4,
	HandTypeDraw = 5,
	HandTypeSingleDraw27 = 6,
	HandTypeStud = 7,
	HandTypeRazz = 8,
	HandType3Draw27 = 9,
	HandType3DrawA5 = 10,
	HandTypeCourchevel = 11,
	HandType6Omaha = 12,  // PYR-92617
	HandTypeSplitHoldem = 13,  // PYR-52675
	HandTypeSplitOmaha = 14,
	HandTypeShowtimeHoldem = 15,  // PYR-64093
	HandTypeShowtimeOmaha = 16,
	HandTypeFusion = 17, // PYR-66528 - starts as hold'em, switches to omaha
	HandTypeUnfoldClassic = 18,	
	HandTypeUnfoldMax = 19,	       // not implemented yet!  don't forget to update playingaudit.cpp 
	HandTypeSixPlusHoldem = 20,    // PYR-73403 
	HandTypeDeepWaterHoldem = 21,  // PYR-82834
	HandTypeDeepWaterOmaha = 22,   // PYR-82834
	HandTypeSwapHoldem = 23,   // PYR-107599
	HandTypeTempestHoldem = 24, // PYR-115738
	//HandType: Scandinavian, Chinese, PineApple, 5-Stud... 
	// New entries here should also have the name mapping updated in stathistory2.cpp
	//////////////////////////////////////////////////////////////////////////
	// *** IMPORTANT!!!!  *** Please notify Game and Payment security team of changes and additions
	//////////////////////////////////////////////////////////////////////////

	ENUM_LAST_VALUE(UniqueHandType), // defines UniqueHandType_Last & UniqueHandType_NextAfterLast

	// multiGame type sequence:
	MultiGameTypeShift = 100,
	MultiGameHose			= MultiGameTypeShift, //lint !e849 : duplicate enum value
	MultiGameHorse			= (MultiGameTypeShift+1),
	//New set, Bug #8800:
	MultiGameEightGame		= (MultiGameTypeShift+2),
	MultiGameHoldemMixed	= (MultiGameTypeShift+3),
	MultiGameOmahaHiLoMixed = (MultiGameTypeShift+4),
	MultiGameHoldemOmahaPl	= (MultiGameTypeShift+5),
	MultiGameTripleStud		= (MultiGameTypeShift+6), // this game is removed by PYR-108504
	MultiGameHoldemNlOmaha	= (MultiGameTypeShift+7), // PYR-15855
	MultiGameOmahaMixed		= (MultiGameTypeShift+8), // PYR-24197
	//MultiGameSomeNew = (MultiGameTypeShift+nn) - SHOE, ROE, NHL etc...
	//////////////////////////////////////////////////////////////////////////
	// *** IMPORTANT!!!!  *** WHEN ADDING NEW ENTRIES HERE PLEASE MAKE SURE TO UPDATE
	// "switch(game) // HandType" in "formatGameType" function in "CRMServer\playingaudit.cpp"
	// in order to keep client version of Playing Audit consistent
	// NOTIFY BA for game name mappings in gateways
	// *** IMPORTANT!!!!*** Please notify Game and Payment security team of changes and additions
	//////////////////////////////////////////////////////////////////////////

	ENUM_LAST_VALUE(HandType) // defines HandType_Last & HandType_NextAfterLast
};

// moved here from NotifyMessageDefs.h
enum PotStruct 
{ 
	StructNonDefined = 0, 
	StructLimit = 1, 
	StructPotLimit = 2, 
	StructNoLimit = 3,
	StructPotNoLimit = 4, 

	ENUM_LAST_VALUE(UniquePotStruct), // defines UniquePotStruct_Last & UniquePotStruct_NextAfterLast

	StructMultiGameMixed = 100,

	ENUM_LAST_VALUE(PotStruct) // defines PotStruct_Last & PotStruct_NextAfterLast
};

#define NUM_MULTIGAME_TYPES 9 // HOSE, HORSE, 8GAME, Holdem MIXED, Omaha H/L MIXED, Holdem/Omaha PL, TripleStud, Holdem NL/Omaha, MixedOmaha... to be rised
#define MAX_MULTIGAME_GAMES 8 // upper limit... to be probably rised

struct GameType
{
	BYTE handType;  // 0 - None, 2 - Hold'em, 4 - Omaha, 7 - Stud, 8 - Razz, 5 - Draw, etc.
	BYTE potStruct; // 0 - Nondefined, 1 - Limit, 2 - PotLimit, 3 - NoLimit 
	bool hiLo;		// isHiLo, !isHiLo
};

struct MultiGameType
{
	const char* descr;
	UINT32 n; //- is maxNumber of GameTypes in array (4-HOSE, 5-HORSE, 3-PLO...)
	GameType games[ MAX_MULTIGAME_GAMES ];
};

extern const MultiGameType multiGameTypes[ NUM_MULTIGAME_TYPES ];

enum eGameObjectType
{
	eGameObjectType_Undefined = 0,
	eGameObjectType_RingTable = 1,
	eGameObjectType_Blitz = 2,
	eGameObjectType_Tourn = 3
};

// PYR-95132
enum eUsersInHandCashoutFlags // bits in INTEGER USERSINHANDCASHOUT.FLAGS
{
	eUsersInHandCashoutFlags_None		= 0,
	eUsersInHandCashoutFlags_Taken		= (1 << 0) // 1
};

#endif // gamedefs_h_included
