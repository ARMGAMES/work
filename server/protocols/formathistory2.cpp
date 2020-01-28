// i18n-d version of formathistory.cpp and tablefmt.cpp
// questions: igors@pyrsoftware.com
#include "handhistory.h"
#include "handhistory2.h"
#include "table.h"
#include "pplib.h"
#include "ppmsg.h"
#include "i18n.locales.h"
#ifndef PROTOCOL_CLIENT_ONLY
#include "i18n.msg_int.h"
#include "i18n.msg_hhy.h"
#endif
#include "i18n.localutil.h"
#include "i18n.utils.h"
#include "ppi18n_x.h"
#include "ppcheck.h"
#include "chatfilter.h"
#include "handhistorycontent.h"
#include "storm/stormhh_thin.cpp"  // for storm table, include cpp file here to avoid the multiple project files change
#include "atfgenerated/tablehh_thin.cpp"  // for non-storm table, include cpp file here to avoid the multiple project files change
#include "strtonumber.h"

#define MAX_NUM_POTS  MAX_TABLE_PLAYERS


// Some power-ups are not yet in production, for them we have no translations

static inline void PMsgNoTranslate( PString& buf, const PMsgLocaleTable& localeTable, unsigned int locale, const char* msgStr,
	const char* p0 = "", const char* p1 = "", const char* p2 = "", const char* p3 = "", const char* p4 = "" )
{
	PMsg5( buf, localeTable, locale, msgStr, p0, p1, p2, p3, p4 );
}

#define __PMsg_Debug__ PMsgNoTranslate

//------------------------------------------------------------------------------

template< size_t N >
inline const PMsgId& __name_msg__( const PMsgId( &names )[ N ], int i )
{
	if( ( 0 <= i ) && ( i < N ) )
	{
		return names[ i ];
	}

	static const PMsgId empty( P_TXT( TXTHHY_EMPTY_STUB ) );
	return empty;
}

//------------------------------------------------------------------------------

static const PMsgId& actionName( int action )
{
	static const PMsgId actionNames[] =
	{
		P_TXT( TXTHHY_EMPTY_STUB ),  // index 0
		P_TXT( TXTHHY_EMPTY_STUB ),  // index 1
		P_TXT( TXTHHY_EMPTY_STUB ),  // index 2
		P_TXT( TXTHHY_EMPTY_STUB ),  // index 3
		P_TXT( TXTHHY_EMPTY_STUB ),  // index 4
		P_TXT( TXTHHY_EMPTY_STUB ),  // index 5
		P_TXT( TXTHHY_EMPTY_STUB ),  // index 6
		P_TXT( TXTHHY_EMPTY_STUB ),  // index 7
		P_TXT( TXTHHY_EMPTY_STUB ),  // index 8
		P_TXT( TXTHHY_EMPTY_STUB ),  // index 9
		P_TXT( TXTHHY_EMPTY_STUB ),  // index 10
		P_TXT( TXTHHY_EMPTY_STUB ),  // index 11
		P_TXT( TXTHHY_EMPTY_STUB ),  // index 12
		P_TXT( TXTHHY_EMPTY_STUB ),  // index 13
		P_TXT( TXTHHY_EMPTY_STUB ),  // index 14
		P_TXT( TXTHHY_EMPTY_STUB ),  // index 15
		P_TXT( TXTHHY_EMPTY_STUB ),  // index 16
		P_TXT( TXTHHY_EMPTY_STUB ),  // index 17
		P_TXT( TXTHHY_EMPTY_STUB ),  // index 18
		P_TXT( TXTHHY_EMPTY_STUB ),  // index 19
		P_TXT( TXTHHY_EMPTY_STUB ),  // index 20
		P_TXT( TXTHHY_EMPTY_STUB ),  // index 21
		P_TXT( TXTHHY_EMPTY_STUB ),  // index 22
		P_TXT( TXTHHY_EMPTY_STUB ),  // index 23
		P_TXT( TXTHHY_EMPTY_STUB ),  // index 24
		P_TXT( TXTHHY_EMPTY_STUB ),  // index 25
		P_TXT( TXTHHY_EMPTY_STUB ),  // index 26
		P_TXT( TXTHHY_EMPTY_STUB ),  // index 27
		P_TXT( TXTHHY_EMPTY_STUB ),  // index 28
		P_TXT( TXTHHY_EMPTY_STUB ),  // index 29
		P_TXT( TXTHHY_EMPTY_STUB ),  // index 30
		P_TXT( TXTHHY_EMPTY_STUB ),  // index 31
		P_TXT( TXTHHY_is_sitting_out ),  // ' '
		P_TXT( TXTHHY_EMPTY_STUB ),  // '!'
		P_TXT( TXTHHY_EMPTY_STUB ),  // '"'
		P_TXT( TXTHHY_EMPTY_STUB ),  // '#'
		P_TXT( TXTHHY_EMPTY_STUB ),  // '$'
		P_TXT( TXTHHY_EMPTY_STUB ),  // '%'
		P_TXT( TXTHHY_EMPTY_STUB ),  // '&'
		P_TXT( TXTHHY_EMPTY_STUB ),  // '\''
		P_TXT( TXTHHY_EMPTY_STUB ),  // '('
		P_TXT( TXTHHY_EMPTY_STUB ),  // ')'
		P_TXT( TXTHHY_EMPTY_STUB ),  // '*'
		P_TXT( TXTHHY_EMPTY_STUB ),  // '+'
		P_TXT( TXTHHY_EMPTY_STUB ),  // ','
		P_TXT( TXTHHY_EMPTY_STUB ),  // '-'
		P_TXT( TXTHHY_EMPTY_STUB ),  // '.'
		P_TXT( TXTHHY_EMPTY_STUB ),  // '/'
		P_TXT( TXTHHY_EMPTY_STUB ),  // '0'
		P_TXT( TXTHHY_EMPTY_STUB ),  // '1'
		P_TXT( TXTHHY_EMPTY_STUB ),  // '2'
		P_TXT( TXTHHY_EMPTY_STUB ),  // '3'
		P_TXT( TXTHHY_EMPTY_STUB ),  // '4'
		P_TXT( TXTHHY_EMPTY_STUB ),  // '5'
		P_TXT( TXTHHY_EMPTY_STUB ),  // '6'
		P_TXT( TXTHHY_EMPTY_STUB ),  // '7'
		P_TXT( TXTHHY_EMPTY_STUB ),  // '8'
		P_TXT( TXTHHY_EMPTY_STUB ),  // '9'
		P_TXT( TXTHHY_EMPTY_STUB ),  // ':'
		P_TXT( TXTHHY_EMPTY_STUB ),  // ';'
		P_TXT( TXTHHY_EMPTY_STUB ),  // '<'
		P_TXT( TXTHHY_EMPTY_STUB ),  // '='
		P_TXT( TXTHHY_EMPTY_STUB ),  // '>'
		P_TXT( TXTHHY_EMPTY_STUB ),  // '?'
		P_TXT( TXTHHY_EMPTY_STUB ),  // '@'
		P_TXT( TXTHHY_posts_the_ante ),  // 'A'
		P_TXT( TXTHHY_bets ),        // 'B'
		P_TXT( TXTHHY_calls ),       // 'C'
		P_TXT( TXTHHY_discards ),    // 'D'
		P_TXT( TXTHHY_raises ),      // 'E'
		P_TXT( TXTHHY_folds ),       // 'F'
		P_TXT( TXTHHY_EMPTY_STUB ),    // 'G'  PYR-65734  entry will not be used as we do full sentence messages for unfold - the proper way
		P_TXT( TXTHHY_EMPTY_STUB ),    // 'H'  PYR-65734  entry will not be used as we do full sentence messages for unfold - the proper way
		P_TXT( TXTHHY_EMPTY_STUB ),  // 'I'
		P_TXT( TXTHHY_EMPTY_STUB ),  // 'J'
		P_TXT( TXTHHY_EMPTY_STUB ),  // 'K'
		P_TXT( TXTHHY_EMPTY_STUB ),  // 'L'  // Load unused stacks in NStack tournament, the entry will not be used as we do full sentence messages
		P_TXT( TXTHHY_mucks_hand ),  // 'M'
		P_TXT( TXTHHY_brings_in_for ),  // 'N'
		P_TXT( TXTHHY_EMPTY_STUB ),  // 'O'
		P_TXT( TXTHHY_posts_big_blind ),  // 'P'
		P_TXT( TXTHHY_posts_big_blind ),  // 'Q'
		P_TXT( TXTHHY_raises ),      // 'R'
		P_TXT( TXTHHY_shows ),       // 'S'
		P_TXT( TXTHHY_EMPTY_STUB ),    // 'T'  PYR-65734  entry will not be used as we do full sentence messages for unfold - the proper way
		P_TXT( TXTHHY_EMPTY_STUB ),  // 'U'  PYR-65734 entry will not be used as we do full sentence messages for unfold - the proper way
		P_TXT( TXTHHY_EMPTY_STUB ),  // 'V'
		P_TXT( TXTHHY_shows ),       // 'W'
		P_TXT( TXTHHY_EMPTY_STUB ),  // 'X'
		P_TXT( TXTHHY_EMPTY_STUB ),  // 'Y'
		P_TXT( TXTHHY_shows ),	     // 'Z' #15261
		P_TXT( TXTHHY_EMPTY_STUB ),  // '['
		P_TXT( TXTHHY_EMPTY_STUB ),  // '\\'
		P_TXT( TXTHHY_EMPTY_STUB ),  // ']'
		P_TXT( TXTHHY_EMPTY_STUB ),  // '^'
		P_TXT( TXTHHY_sits_out ),    // '_'
		P_TXT( TXTHHY_EMPTY_STUB ),  // '`'
		P_TXT( TXTHHY_X040is_being_treated_as_all_in ),  // 'a'
		P_TXT( TXTHHY_bets ),        // 'b'
		P_TXT( TXTHHY_checks ),      // 'c'
		P_TXT( TXTHHY_stands_pat ),  // 'd'
		P_TXT( TXTHHY_raises ),      // 'e'
		P_TXT( TXTHHY_folds ),		 // 'f' #15261
		P_TXT( TXTHHY_posts_giant_blind ),  // 'g' PYR-82834
		P_TXT( TXTHHY_EMPTY_STUB ),  // 'h'
		P_TXT( TXTHHY_EMPTY_STUB ),  // 'i'
		P_TXT( TXTHHY_EMPTY_STUB ),  // 'j'
		P_TXT( TXTHHY_EMPTY_STUB ),  // 'k'
		P_TXT( TXTHHY_EMPTY_STUB ),  // 'l'
		P_TXT( TXTHHY_posts ),       // 'm' PYR-82834 post missed blinds
		P_TXT( TXTHHY_EMPTY_STUB ),  // 'n'
		P_TXT( TXTHHY_EMPTY_STUB ),  // 'o'
		P_TXT( TXTHHY_posts_small_blind ),  // 'p'
		P_TXT( TXTHHY_posts_small_X_big_blinds ),  // 'q'
		P_TXT( TXTHHY_raises ),      // 'r'
		P_TXT( TXTHHY_shows ),       // 's'
		P_TXT( TXTHHY_EMPTY_STUB ),  // 't'
		P_TXT( TXTHHY_EMPTY_STUB ),  // 'u'
		P_TXT( TXTHHY_EMPTY_STUB ),  // 'v'
		P_TXT( TXTHHY_doesnXt_show_hand ),  // 'w'
		P_TXT( TXTHHY_EMPTY_STUB ),  // 'x'	// swap action uses full string translation
		P_TXT( TXTHHY_EMPTY_STUB ),  // 'y'
		P_TXT( TXTHHY_shows ),		 // 'z' #15261
		P_TXT( TXTHHY_EMPTY_STUB ),  // '{'
		P_TXT( TXTHHY_EMPTY_STUB ),  // '|'
		P_TXT( TXTHHY_EMPTY_STUB ),  // '}'
		P_TXT( TXTHHY_EMPTY_STUB ),  // '~'
		P_TXT( TXTHHY_EMPTY_STUB ),  // DEL index 127
	};

	return __name_msg__( actionNames, action );
}

//------------------------------------------------------------------------------

static const PMsgId& roundName( int round, bool upper = false )
{
	if( upper )
	{
		static const PMsgId roundNames[] =
		{
			P_TXT( TXTHHY_HOLE_CARDS ),
			P_TXT( TXTHHY_FLOP ),
			P_TXT( TXTHHY_TURN ),
			P_TXT( TXTHHY_RIVER ),
			P_TXT( TXTHHY_SEA ),
			P_TXT( TXTHHY_OCEAN ),
			P_TXT( TXTHHY_XX_SUMMARY_XX ),
		};

		return __name_msg__( roundNames, round );
	}

	static const PMsgId roundNames[] =
	{
		P_TXT( TXTHHY_X040Pre_Flop ),
		P_TXT( TXTHHY_X040Flop ),
		P_TXT( TXTHHY_X040Turn ),
		P_TXT( TXTHHY_X040River ),
		P_TXT( TXTHHY_X040Sea ),
		P_TXT( TXTHHY_X040Ocean ),
	};

	return __name_msg__( roundNames, round );
}

//------------------------------------------------------------------------------

static const PMsgId& tripleDrawRoundSuffix( int round )
{
	static const PMsgId tripleDrawRoundSuffixes[] =
	{
		P_TXT( TXTHHY_st ),
		P_TXT( TXTHHY_st ),
		P_TXT( TXTHHY_nd ),
		P_TXT( TXTHHY_nd ),
		P_TXT( TXTHHY_rd ),
		P_TXT( TXTHHY_rd ),
	};

	return __name_msg__( tripleDrawRoundSuffixes, round );
}

//------------------------------------------------------------------------------

static const PMsgId& boardName( int board )
{
	static const PMsgId boardNames[] =
	{
		P_TXT( TXTHHY_FIRST ),
		P_TXT( TXTHHY_SECOND ),
	};

	return __name_msg__( boardNames, board );
}

//------------------------------------------------------------------------------

#include "romannum.h"

//------------------------------------------------------------------------------

static const PMsgId& rankName( int rank, int rankOffset )
{
	static const PMsgId rankNames[] =
	{
		P_TXT( TXTHHY_Deuce ),
		P_TXT( TXTHHY_Deuces ),
		P_TXT( TXTHHY_2 ),
		P_TXT( TXTHHY_Three ),
		P_TXT( TXTHHY_Threes ),
		P_TXT( TXTHHY_3 ),
		P_TXT( TXTHHY_Four ),
		P_TXT( TXTHHY_Fours ),
		P_TXT( TXTHHY_4 ),
		P_TXT( TXTHHY_Five ),
		P_TXT( TXTHHY_Fives ),
		P_TXT( TXTHHY_5 ),
		P_TXT( TXTHHY_Six ),
		P_TXT( TXTHHY_Sixes ),
		P_TXT( TXTHHY_6 ),
		P_TXT( TXTHHY_Seven ),
		P_TXT( TXTHHY_Sevens ),
		P_TXT( TXTHHY_7 ),
		P_TXT( TXTHHY_Eight ),
		P_TXT( TXTHHY_Eights ),
		P_TXT( TXTHHY_8 ),
		P_TXT( TXTHHY_Nine ),
		P_TXT( TXTHHY_Nines ),
		P_TXT( TXTHHY_9 ),
		P_TXT( TXTHHY_Ten ),
		P_TXT( TXTHHY_Tens ),
		P_TXT( TXTHHY_T ),
		P_TXT( TXTHHY_Jack ),
		P_TXT( TXTHHY_Jacks ),
		P_TXT( TXTHHY_J ),
		P_TXT( TXTHHY_Queen ),
		P_TXT( TXTHHY_Queens ),
		P_TXT( TXTHHY_Q ),
		P_TXT( TXTHHY_King ),
		P_TXT( TXTHHY_Kings ),
		P_TXT( TXTHHY_K ),
		P_TXT( TXTHHY_Ace ),
		P_TXT( TXTHHY_Aces ),
		P_TXT( TXTHHY_A ),
	};

	return __name_msg__( rankNames, ( ( rank - 2 ) * 3 ) + rankOffset );
}

//------------------------------------------------------------------------------

#define RANK_OFFSET		0
#define RANK_S_OFFSET	1
#define RANK_N_OFFSET	2

//------------------------------------------------------------------------------

void _rankName( PString& val, int locale, int rank )
{
	PASSERT( rank >= 2 && rank <= 14 );
	PMsg( val, LocalesTable, locale, rankName( rank, RANK_OFFSET ) );
}

//------------------------------------------------------------------------------

void _rankName_s( PString& val, int locale, int rank )
{
	PASSERT( rank >= 2 && rank <= 14 );
	PMsg( val, LocalesTable, locale, rankName( rank, RANK_S_OFFSET ) );
}

//------------------------------------------------------------------------------

void _rankName_n( PString& val, int locale, int rank )
{
	PASSERT( rank >= 2 && rank <= 14 );
	PMsg( val, LocalesTable, locale, rankName( rank, RANK_N_OFFSET ) );
}

/*---------------------------------------------------------------------------------------------
_powerName
---------------------------------------------------------------------------------------------*/

static const PMsgId& powerName( int power )
{
	static vector< PMsgId > msgIdsVec;

	if( !msgIdsVec.size() )
	{
		msgIdsVec.resize( PokerGamePowerLastSlot + 1 );

		msgIdsVec[ PokerGamePowerChaos ] = P_TXT( TXTHHY_Chaos );
		msgIdsVec[ PokerGamePowerClone ] = P_TXT( TXTHHY_Clone );
		msgIdsVec[ PokerGamePowerDeploy ] = P_TXT( TXTHHY_Deploy );
		msgIdsVec[ PokerGamePowerDisintegrate ] = P_TXT( TXTHHY_Disintegrate );
		msgIdsVec[ PokerGamePowerEngineer ] = P_TXT( TXTHHY_Engineer );
		msgIdsVec[ PokerGamePowerEMP ] = P_TXT( TXTHHY_EMP );
		msgIdsVec[ PokerGamePowerIntel ] = P_TXT( TXTHHY_Intel );
		msgIdsVec[ PokerGamePowerSyphon ] = P_TXT( TXTHHY_Syphon );
		msgIdsVec[ PokerGamePowerMorph ] = P_TXT( TXTHHY_Morph );
		msgIdsVec[ PokerGamePowerPacify ] = P_TXT( TXTHHY_Pacify );
		msgIdsVec[ PokerGamePowerReload ] = P_TXT( TXTHHY_Reload );
		msgIdsVec[ PokerGamePowerScanner ] = P_TXT( TXTHHY_Scanner );
		msgIdsVec[ PokerGamePowerUpgrade ] = P_TXT( TXTHHY_Upgrade );
		msgIdsVec[ PokerGamePowerXRay ] = P_TXT( TXTHHY_X_Ray );
		msgIdsVec[ PokerGamePowerMint ] = P_TXT( TXTHHY_Mint );

		// the following is not translated because it's not yet in production

		// 	msgIdsVec[PokerGamePowerStealth			] = 
		// 	msgIdsVec[PokerGamePowerDisarm			] = 
		// 	msgIdsVec[PokerGamePowerDetect			] = 
		// 	msgIdsVec[PokerGamePowerMindControl		] = 
		// 	msgIdsVec[PokerGamePowerStretch			] = 
		// 	msgIdsVec[PokerGamePowerParasite		] = 
		// 	msgIdsVec[PokerGamePowerRunItTwice		] = 
		// 	msgIdsVec[PokerGamePowerZombie			] = 
		// 	msgIdsVec[PokerGamePowerLeeroyJenkins	] = 
		// 	msgIdsVec[PokerGamePowerTeleport		] = 
		// 	msgIdsVec[PokerGamePowerSwap			] = 
		// 	msgIdsVec[PokerGamePowerLowBlow			] = 
		// 	msgIdsVec[PokerGamePowerAnteUp			] = 
		// 	msgIdsVec[PokerGamePowerExtraLife		] = 
		// 	msgIdsVec[PokerGamePowerSplashPot		] = 
		// 	msgIdsVec[PokerGamePowerSuitUp			] = 
		// 	msgIdsVec[PokerGamePowerTakeBack		] = 
		// 	msgIdsVec[PokerGamePowerDummySpecimen	] = 
		// 	msgIdsVec[PokerGamePowerFreeze	        ] = 
		// 	msgIdsVec[PokerGamePowerSecretPlaceholder] =
		// 	msgIdsVec[PokerGamePowerSabotage	    ] = 
		// 	msgIdsVec[PokerGamePowerCounterSpell	] = 
		// 	msgIdsVec[PokerGamePowerReboot          ] = 
		// 	msgIdsVec[PokerGamePowerEqualizer       ] = 
		// 	msgIdsVec[PokerGamePowerDarkBet 		] = 
	}

	if( ( power < 0 ) || ( power > PokerGamePowerLastSlot ) )
	{
		power = PokerGamePowerLastSlot;
	}

	return msgIdsVec[ power ];
}

// the following works only as a 'backup' for development purposes. The actual production names are provided by powerName() function 
static const map<int, PString> generateEnglishPowerNames()
{
	map<int,PString> namesMap;
	// Powers
	namesMap[PokerGamePowerStealth			] = "Stealth";
	namesMap[PokerGamePowerDisarm			] = "Disarm";
	namesMap[PokerGamePowerDetect			] = "Detect";
	namesMap[PokerGamePowerXRay				] = "X-Ray";
	namesMap[PokerGamePowerPacify			] = "Pacify";
	namesMap[PokerGamePowerMindControl		] = "Mind Control";
	namesMap[PokerGamePowerStretch			] = "Stretch";
	namesMap[PokerGamePowerParasite			] = "Parasite";
	namesMap[PokerGamePowerUpgrade			] = "Upgrade";
	namesMap[PokerGamePowerReload			] = "Reload";
	namesMap[PokerGamePowerRunItTwice		] = "Run It Twice";
	namesMap[PokerGamePowerIntel			] = "Intel";
	namesMap[PokerGamePowerMorph			] = "Morph";
	namesMap[PokerGamePowerZombie			] = "Zombie";
	namesMap[PokerGamePowerLeeroyJenkins	] = "Leeroy Jenkins";
	namesMap[PokerGamePowerClone			] = "Clone";
	namesMap[PokerGamePowerChaos			] = "Chaos";
	namesMap[PokerGamePowerTeleport			] = "Teleport";
	namesMap[PokerGamePowerMint				] = "Mint";
	namesMap[PokerGamePowerSwap				] = "Swap";
	namesMap[PokerGamePowerLowBlow			] = "Low Blow";
	namesMap[PokerGamePowerAnteUp			] = "Ante Up";
	namesMap[PokerGamePowerDisintegrate		] = "Disintegrate";
	namesMap[PokerGamePowerScanner			] = "Scanner";
	namesMap[PokerGamePowerExtraLife		] = "Extra Life";
	namesMap[PokerGamePowerEngineer			] = "Engineer";
	namesMap[PokerGamePowerSplashPot		] = "Splash The Pot";
	namesMap[PokerGamePowerSuitUp			] = "Suit Up";
	namesMap[PokerGamePowerTakeBack			] = "Take It Back";
	namesMap[PokerGamePowerDummySpecimen	] = "DummySpecimen";
	namesMap[PokerGamePowerFreeze	        ] = "Freeze";
	namesMap[PokerGamePowerEMP		        ] = "EMP";
	namesMap[PokerGamePowerSecretPlaceholder] = "Secret";
	namesMap[PokerGamePowerSabotage	        ] = "Sabotage";
	namesMap[PokerGamePowerSyphon			] = "Syphon";
	namesMap[PokerGamePowerCounterSpell		] = "Counter Spell";
	namesMap[PokerGamePowerReboot           ] = "Reboot";
	namesMap[PokerGamePowerDeploy			] = "Deploy";
	namesMap[PokerGamePowerEqualizer        ] = "Equalizer";
	namesMap[PokerGamePowerDarkBet 			] = "Dark Bet";

	return namesMap;
}

static const map<int,PString> powerEnglishNamesMap = generateEnglishPowerNames(); // names without translation

const char* _powerName(int powerEvtCode, int locale, PString& out)
{
	if (powerEvtCode >= 0 && powerEvtCode < PokerGamePowerLastSlot && !powerName(powerEvtCode).isNull())
	{
		out.clear();
		PMsg(out, LocalesTable, locale, powerName(powerEvtCode));
		return out.c_str();
	}

	PLog("formatHistory2 could not find powerId %d in powerMsgIdMap", powerEvtCode);

// as a second chance, look into the table of non-translated power names (for development/QA purposes only)
	if (powerEnglishNamesMap.find(powerEvtCode) == powerEnglishNamesMap.end())
	{
		PLog("formatHistory2 could not find powerId %d in powerEnglishNamesMap", powerEvtCode);
	}
#if 0
	if (_powerNamesMap.find(powerEvtCode) == _powerNamesMap.end()) // AQA-756
		throw PError(PString("AQA-756 fault powerEvtCode=").appendInt(powerEvtCode));
#endif

	PASSERT( powerEnglishNamesMap.find(powerEvtCode) != powerEnglishNamesMap.end() );
	out = powerEnglishNamesMap.at(powerEvtCode);
	return out.c_str();
}

static void insertCommaInCardsString(const PString& cardsStringWithoutComma, PString& cardsStringWithComma, const char *comma = ", ")
{
	int numCards = cardsStringWithoutComma.length() / 2;
	for (int i = 0; i < numCards; ++i)
	{
		if (i != 0)
		{
			cardsStringWithComma.append(comma);
		}
		cardsStringWithComma.append(cardsStringWithoutComma[2*i]).append(cardsStringWithoutComma[2*i+1]);
	}
}


/*---------------------------------------------------------------------------------------------
_bestLoCombination
---------------------------------------------------------------------------------------------*/
void _bestLoComb( int locale, const Card* bestLowCards, PString& comb, bool separator )
{
	//comb = "";
	for ( int i = 4; i >= 0; --i )
	{
		_rankName_n( comb, locale, bestLowCards[ i ].rank );
		if( i > 0 )
			PMsg(comb, LocalesTable, locale, separator ? P_TXT( TXTHHY_X040 ) : P_TXT( TXTHHY_X_1 ) );
	}
}

/*---------------------------------------------------------------------------------------------
_bestRazzSwtch 
---------------------------------------------------------------------------------------------*/
void _bestRazzSwtch( int locale, BYTE bestLow, const Card* bestLowCards, PString& comb, bool separator )
{
	PMsg( comb, LocalesTable, locale, P_TXT( TXTHHY_LoXX040 ) );

	for( int i = 0; i < 5; ++i )
	{
		if( !bestLowCards[ i ].rank )
			break; // added due to Razz game appear
		if( i > 0 )
			PMsg( comb, LocalesTable, locale, separator ? P_TXT( TXTHHY_X040 ) : P_TXT( TXTHHY_X_1 ) );

		_rankName_n( comb, locale, bestLowCards[ i ].rank );
	}
	// commented by Isai: don't name the combinations at all, just show the 5 (worst) cards ordered.
	//if( bestLow > 1 )
	//	comb.append( " - " );

	switch( bestLow )
	{
		//case 9://Straight Flush
		//case 6://Flush
		//case 5://Straight
		//	break; // Triple Draw lowBall 
	case 8://4-of-a-Kind
	case 7://Full House
	case 4://3-of-a-Kind
	case 3://Two Pair
	case 2://Pair
	case 1://High Card
		break; // do nothing for razz case!
	default:
		{
			//PASSERT( 0 ); //return 0; unreachable
			char buf[ 80 ];
			sprintf( buf, "_bestRazzSwtch::eRrOR! bestLow=%d", bestLow );
			throw PError( buf );
		}
	}
}

/*---------------------------------------------------------------------------------------------
_bestHandSwtch 
---------------------------------------------------------------------------------------------*/
void _bestHandSwtch( int locale, BYTE bestHigh, const Card* bestHighCards, PString& comb )
{
	//	char suit;//'s'/'h'/'d'/'c'. ' ' means 'unknown'
	//	int rank;//2-9 - as is, 10-T, 11-J, 12-Q, 13-K, 14-A
	PString one, two;
	switch( bestHigh )
	{
	case 9://Straight Flush
		{
			if ( bestHighCards[ 0 ].rank == 14) // if Royal flush
				PMsg( comb, LocalesTable, locale, P_TXT( TXTHHY_a_Royal_Flush ) );
			else 
			{
				_rankName( one, locale, bestHighCards[ 4 ].rank );
				_rankName( two, locale, bestHighCards[ 0 ].rank );
				PMsg2( comb, LocalesTable, locale, P_TXT( TXTHHY_a_straight_flushX_N0_to_N1 ), one, two );
			}
		}
		break;

	case 8://4-of-a-Kind
		{
			_rankName_s( one, locale, bestHighCards[ 0 ].rank );
			PMsg1( comb, LocalesTable, locale, P_TXT( TXTHHY_four_of_a_kindX_N0 ), one );
		}
		break;

	case 7://Full House
		{
			_rankName_s( one, locale, bestHighCards[ 0 ].rank );
			_rankName_s( two, locale, bestHighCards[ 3 ].rank );
			PMsg2( comb, LocalesTable, locale, P_TXT( TXTHHY_a_full_houseX_N0_full_of_N1 ), one, two );
		}
		break;

	case 6://Flush
		{
			_rankName( one, locale, bestHighCards[ 0 ].rank );
			PMsg1( comb, LocalesTable, locale, P_TXT( TXTHHY_a_flushX_N0_high ), one );
		}
		break;

	case 5://Straight
		{
			_rankName( one, locale, bestHighCards[ 4 ].rank );
			_rankName( two, locale, bestHighCards[ 0 ].rank );
			PMsg2( comb, LocalesTable, locale, P_TXT( TXTHHY_a_straightX_N0_to_N1 ), one, two );
		}
		break;

	case 4://3-of-a-Kind
		{
			_rankName_s( one, locale, bestHighCards[ 0 ].rank );
			PMsg1( comb, LocalesTable, locale, P_TXT( TXTHHY_three_of_a_kindX_N0 ), one );
		}
		break;

	case 3://Two Pair
		{
			_rankName_s( one, locale, bestHighCards[ 0 ].rank );
			_rankName_s( two, locale, bestHighCards[ 2 ].rank );
			PMsg2( comb, LocalesTable, locale, P_TXT( TXTHHY_two_pairX_N0_and_N1 ), one, two );
		}
		break;

	case 2://Pair
		{
			_rankName_s( one, locale, bestHighCards[ 0 ].rank );
			PMsg1( comb, LocalesTable, locale, P_TXT( TXTHHY_a_pair_of_N0 ), one );
		}
		break;

	case 1://High Card
		{
			_rankName( one, locale, bestHighCards[ 0 ].rank );
			PMsg1( comb, LocalesTable, locale, P_TXT( TXTHHY_high_card_N0 ), one );
		}
		break;

	default:
		PASSERT5(0);
		if (bestHighCards[4].rank != 0)
		{
			PLog(LOG_TRACE_FAULTY_LOGIC ": bestHigh == 0");
		}
		break;
	}
}

void _bestHandSwtch2( int locale, BYTE bestHigh2, const Card* bestHighCards2, PString& comb )
{
	if ( bestHigh2 )
		_bestHandSwtch( locale, bestHigh2, bestHighCards2, comb );
	//else the second board information is not available in the database
}

/*---------------------------------------------------------------------------------------------
_bestTripleDrawSwtch
---------------------------------------------------------------------------------------------*/
void _bestTripleDrawSwtch( int locale, BYTE bestLow, const Card* bestLowCards, PString& comb, bool separator )
{
	PMsg( comb, LocalesTable, locale, P_TXT( TXTHHY_LoXX040 ) );
	PString one, two;

	switch( bestLow )
	{
	case 9://Straight Flush
		{
			if ( bestLowCards[ 0 ].rank == 14) // if Royal flush
				PMsg( comb, LocalesTable, locale, P_TXT( TXTHHY_a_Royal_Flush ) );
			else 
			{
				_rankName( one, locale, bestLowCards[ 4 ].rank );
				_rankName( two, locale, bestLowCards[ 0 ].rank );
				PMsg2( comb, LocalesTable, locale, P_TXT( TXTHHY_a_straight_flushX_N0_to_N1 ), one, two );
			}
		}
		break;

	case 8://4-of-a-Kind
		{
			_rankName_s( one, locale, bestLowCards[ 0 ].rank );
			PMsg1( comb, LocalesTable, locale, P_TXT( TXTHHY_four_of_a_kindX_N0 ), one );
		}
		break;

	case 7://Full House
		{
			_rankName_s( one, locale, bestLowCards[ 0 ].rank );
			_rankName_s( two, locale, bestLowCards[ 3 ].rank );
			PMsg2( comb, LocalesTable, locale, P_TXT( TXTHHY_a_full_houseX_N0_full_of_N1 ), one, two );
		}
		break;

	case 6://Flush
		{
			_rankName( one, locale, bestLowCards[ 0 ].rank );
			PMsg1( comb, LocalesTable, locale, P_TXT( TXTHHY_a_flushX_N0_high ), one );
		}
		break;

	case 5://Straight
		{
			_rankName( one, locale, bestLowCards[ 4 ].rank );
			_rankName( two, locale, bestLowCards[ 0 ].rank );
			PMsg2( comb, LocalesTable, locale, P_TXT( TXTHHY_a_straightX_N0_to_N1 ), one, two );
		}
		break;

	case 4://3-of-a-Kind
		{
			_rankName_s( one, locale, bestLowCards[ 0 ].rank );
			PMsg1( comb, LocalesTable, locale, P_TXT( TXTHHY_three_of_a_kindX_N0 ), one );
		}
		break;

	case 3://Two Pair
		{
			_rankName_s( one, locale, bestLowCards[ 0 ].rank );
			_rankName_s( two, locale, bestLowCards[ 2 ].rank );
			PMsg2( comb, LocalesTable, locale, P_TXT( TXTHHY_two_pairX_N0_and_N1 ), one, two );
		}
		break;

	case 2://Pair
		{
			_rankName_s( one, locale, bestLowCards[ 0 ].rank );
			PMsg1( comb, LocalesTable, locale, P_TXT( TXTHHY_a_pair_of_N0 ), one );
		}
		break;

	case 1://High Card
		{
			//PMsg1( comb, msgTable, P_TXT( TXT_DEALERCHAT_HIGHCARD ), _rankName( msgTable, bestLowCards[ 0 ].rank ) );
			for( int i = 0; i < 5; ++i )
			{
				if( !bestLowCards[ i ].rank )
					PASSERT( 0 );
				if( i > 0 )
					PMsg( comb, LocalesTable, locale, separator ? P_TXT( TXTHHY_X040 ) : P_TXT( TXTHHY_X_1 ) );

				_rankName_n( comb, locale, bestLowCards[ i ].rank );
			}
		}
		break;

	default:
		PASSERT( 0 );
		break;
	}
}

/*---------------------------------------------------------------------------------------------
_bestBadugiSwtch
---------------------------------------------------------------------------------------------*/
void _bestBadugiSwtch( int locale, BYTE bestLow, const Card* bestLowCards, bool isPhraseComplete, PString& comb, bool separator )
{
	//comb.append( PMsgStrPtr( msgTable, P_TXT( TXT_DEALERCHAT_LO ) ) );
	static const char* const _cards[] = {"0","1","2","3"};
	PString rankName;
	for( int i = 0; i < bestLow; ++i )
	{
		if( !bestLowCards[ i ].rank )
			PASSERT( 0 );
		if( i > 0 )
			PMsg( rankName, LocalesTable, locale, separator ? P_TXT( TXTHHY_X040 ) : P_TXT( TXTHHY_X_1 ) );

		_rankName_n( rankName, locale, bestLowCards[ i ].rank );
	}

	switch( bestLow )
	{
	case 4://Badugi:
		{
			PMsg1( comb, LocalesTable, locale, isPhraseComplete ? P_TXT( TXTHHY_a_BadugiX_N0 ) : P_TXT( TXTHHY_BadugiX_N0 ), rankName );
		}
		break;
	case 3: // 1/2/3 card combination:
	case 2:
	case 1:
		{
			PMsg2( comb, LocalesTable, locale, isPhraseComplete ? P_TXT( TXTHHY_a_N0_cardX_N1 ) : P_TXT( TXTHHY_N0_cardX_N1 ), _cards[ bestLow ], rankName );
		}
		break;
	default:
		PASSERT( 0 );
		break;
	}
}

/*---------------------------------------------------------------------------------------------
Feel the 'Best Combination' PString 
---------------------------------------------------------------------------------------------*/
static void fillBestCombPstr( const Hand& hand, PString& buff, int i, int locale )
{
	Card bestCards[ 5 ];
	strToCards( hand.users[ i ].combHi, bestCards );

	//HISTORY_HI HI:
	if( hand.isHiLo )
		PMsg( buff, LocalesTable, locale, P_TXT( TXTHHY_HIXX040 ) );

	_bestHandSwtch( locale, hand.users[ i ].bestHand, bestCards, buff );

	//if( historyStr[ historyStr.length() - 1 ] != '\n' )
	if( *(hand.users[ i ].combLo) )
	{
		PMsg( buff, LocalesTable, locale, P_TXT( TXTHHY_X_LOXX040 ) );
		strToCards( hand.users[ i ].combLo, bestCards );
		_bestLoComb( locale, bestCards, buff );
	}
}

static void fillBestCombPstr2( const Hand& hand, PString& buff, int i, int locale )
{
	Card bestCards[ 5 ];
	strToCards( hand.users2[ i ].combHi2, bestCards );

	//HISTORY_HI HI:
	if( hand.isHiLo )
		PMsg( buff, LocalesTable, locale, P_TXT( TXTHHY_HIXX040 ) );

	_bestHandSwtch2( locale, hand.users2[ i ].bestHand2, bestCards, buff );

	//if( historyStr[ historyStr.length() - 1 ] != '\n' )
	if( *(hand.users2[ i ].combLo2 ) )
	{
		PMsg( buff, LocalesTable, locale, P_TXT( TXTHHY_X_LOXX040 ) );
		strToCards( hand.users2[ i ].combLo2, bestCards );
		_bestLoComb( locale, bestCards, buff );
	}		
}


/*---------------------------------------------------------------------------------------------
subtractCardsPstr
---------------------------------------------------------------------------------------------*/
static void subtractCardsPstr( PString& minuend, const char* subtrahend, int numCards, int locale )
{
	PASSERT( PString::length( subtrahend ) == numCards * 2 );
	PASSERT( minuend.length() <= 10 );

	char tmp[ 10 ] = {'\x0','\x0','\x0','\x0','\x0','\x0','\x0','\x0','\x0','\x0'};
	PString tmp_buff;

	int i;
	for( i = 0; i < numCards; ++i, subtrahend += 2 )
		for( int j = 0; j < minuend.length() / 2; ++j )
		{
			if( !( PString::compare( minuend.c_str() + 2 * j, subtrahend, 2 ) ) )
			{
				tmp[ 2 * j ] = '*';
				tmp[ 2 * j + 1 ] = '*';
				break; //for(j)
			}
		}

		subtrahend = minuend;

		for( i = 0; i < minuend.length(); ++i, subtrahend++ )
			if( tmp[ i ] != '*' )
				i18nAppendChar( tmp_buff, *subtrahend );

		minuend = tmp_buff;
}

/*---------------------------------------------------------------------------------------------
formatSecondString 
---------------------------------------------------------------------------------------------*/
static void _formatTimeString( UINT32 requestContents, PString& ret, int locale )
{
	PString week, day, hour, min, sec;
	char str_buff[ 16 ] = "";
	// min
	int seconds = requestContents % 60;
	requestContents /= 60;

	int minutes = requestContents % 60;
	requestContents /= 60;

	int hours = requestContents % 24;
	requestContents /= 24;

	int days = requestContents % 7;
	requestContents /= 7;

	if( requestContents )
	{
		sprintf( str_buff, "%d", requestContents ); 
		if( requestContents == 1 )
			PMsg1( week, LocalesTable, locale, P_TXT( TXTHHY_X040N0_week ), str_buff );
		else
			PMsg1( week, LocalesTable, locale, P_TXT( TXTHHY_X040N0_weeks ), str_buff );
	}

	if( days )
	{
		sprintf( str_buff, "%d", days ); 
		if( days == 1 )
			PMsg1( day, LocalesTable, locale, P_TXT( TXTHHY_X040N0_day ), str_buff );
		else
			PMsg1( day, LocalesTable, locale, P_TXT( TXTHHY_X040N0_days ), str_buff );
	}
	if( hours )
	{
		sprintf( str_buff, "%d", hours ); 
		if( hours == 1 )
			PMsg1( hour, LocalesTable, locale, P_TXT( TXTHHY_X040N0_hour ), str_buff );
		else
			PMsg1( hour, LocalesTable, locale, P_TXT( TXTHHY_X040N0_hours ), str_buff );
	}
	if( minutes )
	{
		sprintf( str_buff, "%d", minutes ); 
		if( minutes == 1 )
			PMsg1( min, LocalesTable, locale, P_TXT( TXTHHY_X040N0_minute ), str_buff );
		else
			PMsg1( min, LocalesTable, locale, P_TXT( TXTHHY_X040N0_minutes ), str_buff );
	}
	if( seconds )
	{
		sprintf( str_buff, "%d", seconds ); 
		if( seconds == 1 )
			PMsg1( sec, LocalesTable, locale, P_TXT( TXTHHY_X040N0_second ), str_buff );
		else
			PMsg1( sec, LocalesTable, locale, P_TXT( TXTHHY_X040N0_seconds ), str_buff );
	}
	PMsg5( ret, LocalesTable, locale, P_TXT( TXTHHY_lastN0N1N2N3N4_of_play ), week, day, hour, min, sec );
}

/*---------------------------------------------------------------------------------------------
_formatDateTimeTZ
---------------------------------------------------------------------------------------------*/
static void _formatDateTimeTZ(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, const SrvTime& time, UINT32 tzf)
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
	p_utoa(localTime._hour, hour);
	sprintf(minute, "%02u", (UINT32)localTime._minute);
	sprintf(second, "%02u", (UINT32)localTime._second);
	PString timeStr;
	PMsg4(timeStr, table, localeId, P_TXT( TXTINT_FORMAT_TimeWithSecTZ ), hour, minute, second, tzStr);

	PMsg2(ret, table, localeId, P_TXT( TXTINT_FORMAT_Date_Time ), dateStr, timeStr);
}
/*---------------------------------------------------------------------------------------------
_formatDateTimeAltTZ
---------------------------------------------------------------------------------------------*/
static void _formatDateTimeAltTZ(PString& ret, const PMsgLocaleTable& table, unsigned int localeId, const SrvTime& time, UINT32 tzf)
{
	if(getTimezone(tzf) == eTzET)
	{
		_formatDateTimeTZ(ret, table, localeId, time, tzf);
	}
	else	//alternate format
	{
		PString dtStr1;
		_formatDateTimeTZ(dtStr1, table, localeId, time, tzf);
		PString dtStr2;
		_formatDateTimeTZ(dtStr2, table, localeId, time, makeTimezoneFormat(eTzET, 0));
		PMsg2(ret, table, localeId, P_TXT( TXTINT_FORMAT_DateTimeAlt ), dtStr1, dtStr2);
	}
}
/*---------------------------------------------------------------------------------------------
formatHistorySubject 
---------------------------------------------------------------------------------------------*/
void formatHistorySubject( int locale, BYTE requestType, UINT64 requestContents, const char* brandName, PString& subject )
{
	try
	{
		char str_buff[ 32 ];
		PMsg1( subject, LocalesTable, locale, P_TXT( TXTHHY_N0_Hand_History_Request__X040 ), brandName ); // PYR-40976
		switch( requestType )
		{
		case HAND_HISTORY_LAST_HANDS:
			// HISTORY_HEADER_LASTN last %0%1
			p_utoa( ( UINT32 )requestContents, str_buff );
			PMsg1( subject, LocalesTable, locale, P_TXT( TXTHHY_your_last_N0_hands ), str_buff );
			break;

		case HAND_HISTORY_LAST_SECONDS:
			_formatTimeString( ( UINT32 )requestContents, subject, locale );
			break;

		case HAND_HISTORY_HAND_IDS:
			p_u64toa( requestContents, str_buff );
			PMsg1( subject, LocalesTable, locale, P_TXT( TXTHHY_Hand_N0 ), str_buff );
			break;

		case HAND_HISTORY_TOURNAMENT:
			p_utoa( ( UINT32 )requestContents, str_buff );
			PMsg1( subject, LocalesTable, locale, P_TXT( TXTHHY_Tournament_N0 ), str_buff );
			break;

		case HAND_HISTORY_LAST_HANDS_BEFORE:
			// do nothing
			break;

		default:
			PLog( "formatHistory.cpp::formatHistorySubject:ERROR:Unknown REQUEST_TYPE: %d - ignored", requestType );
		}
	}
	catch( const PError& er )
	{
		PLog( "FormatHistorySubject::%s", er.why() );
	}
}

/*---------------------------------------------------------------------------------------------
formatHistoryHeader 
---------------------------------------------------------------------------------------------*/
void formatHistoryHeader( int locale, const char* userId, const char* userEmail, BYTE requestType, UINT64 requestContents, UINT32 requestReturned, PString& history )
{
	try
	{
		// HAND_HISTORY_LAST_HANDS	 0
		// HAND_HISTORY_LAST_SECONDS 1
		// HAND_HISTORY_HAND_IDS	 2
		// HAND_HISTORY_TOURNAMENT	 3
		// HAND_HISTORY_LAST_HANDS_BEFORE	4
		// HAND_HISTORY_LAST_HANDS_BEFORE_DATE 5
		// HAND_HISTORY_TABLE			6
		PString str_hands, str_type;
		char str_buff[ 32 ];
		bool nonconsistent = false;

		switch( requestType )
		{
		case HAND_HISTORY_LAST_HANDS:
			// HISTORY_HEADER_LASTN last %0%1
			p_utoa( ( UINT32 )requestContents, str_buff );
			PMsg( str_hands, LocalesTable, locale, P_TXT( TXTHHY_X040hands ) );
			PMsg1( str_type, LocalesTable, locale, P_TXT( TXTHHY_your_last_N0_hands ), str_buff );
			if( requestReturned != requestContents )
				nonconsistent = true;
			break;

		case HAND_HISTORY_LAST_SECONDS:
			_formatTimeString( ( UINT32 )requestContents, str_type, locale );
			break;

		case HAND_HISTORY_HAND_IDS:
			p_u64toa( requestContents, str_buff );
			PMsg1( str_type, LocalesTable, locale, P_TXT( TXTHHY_hand_XN0 ), str_buff );
			break;

		case HAND_HISTORY_TOURNAMENT:
			p_utoa( ( UINT32 )requestContents, str_buff );
			PMsg1( str_type, LocalesTable, locale, P_TXT( TXTHHY_tournament_XN0 ), str_buff );
			break;

		case HAND_HISTORY_LAST_HANDS_BEFORE:
			// do nothing, perhaps TBD something later
			break;

		case HAND_HISTORY_LAST_HANDS_BEFORE_DATE:
			// do nothing, perhaps TBD something later
			break;

		case HAND_HISTORY_TABLE:
			{
				//"Transcript for last 200 games on Table #51154905"
				p_utoa( requestReturned, str_buff );
				PMsg( str_hands, LocalesTable, locale, P_TXT( TXTHHY_X040hands ) );
				PMsg1( str_type, LocalesTable, locale, P_TXT( TXTHHY_your_last_N0_hands ), str_buff );
				p_u64toa( requestContents, str_buff );
				i18nAppend( str_type, " on the Table #" );
				i18nAppend( str_type, str_buff );
			}
			break;

		default:
			PLog( "formatHistory.cpp::formatHistoryHeader:ERROR:Unknown REQUEST_TYPE: %d - ignored", requestType );
		}
		//HISTORY_HEADER Transcript for %0 requested by %1(%2)
		PMsg3( history, LocalesTable, locale, P_TXT( TXTHHY_Transcript_for_N0_requested_by_ELL ), str_type, userId, userEmail );
		if( nonconsistent )
		{ //HISTORY_HEADER_REQUESTED . Your request was %0, however you have only played %1 
			char str_buff1[ 10 ];
			sprintf( str_buff1, "%u", requestReturned ); 
			PMsg3( history, LocalesTable, locale, P_TXT( TXTHHY__Your_request_was_for_N0X_howe_ELL ), str_buff, str_buff1, str_hands );
		}
	}
	catch( const PError& er )
	{
		PLog( "FormatHistoryHeader::%s", er.why() );
	}
}

/*---------------------------------------------------------------------------------------------
formatHandHistory 
---------------------------------------------------------------------------------------------*/
static void _formatScaledChips( UINT32 value, UINT32 scale, UINT32 denominator, char* str_out )
{
	PASSERT(denominator);
	UINT64 scaled_value = ( (UINT64)value * scale ) / denominator;
	UINT32 rest = (UINT32)( ( (UINT64)value * scale ) % denominator );

	char buff[ 32 ];
	if( rest )
		sprintf( str_out, "%s.%02u", p_u64toa( scaled_value, buff ), rest );
	else
		p_u64toa( scaled_value, str_out );
}

static void _formatScaledChipsLoHi( UINT32 lo, UINT32 hi, UINT32 scale, UINT32 denominator, char* str_out_lo, char* str_out_hi )
{
	PASSERT(denominator);
	UINT64 scaled_value_lo = ( (UINT64)lo * scale ) / denominator;
	UINT64 scaled_value_hi = ( (UINT64)hi * scale ) / denominator;
	UINT32 rest_lo = (UINT32)( ( (UINT64)lo * scale ) % denominator );
	UINT32 rest_hi = (UINT32)( ( (UINT64)hi * scale ) % denominator );
	if( rest_lo || rest_hi )
	{
		char buff[ 32 ];
		sprintf( str_out_lo, "%s.%02u", p_u64toa( scaled_value_lo, buff ), rest_lo );
		sprintf( str_out_hi, "%s.%02u", p_u64toa( scaled_value_hi, buff ), rest_hi );
	}
	else 
	{
		p_u64toa( scaled_value_lo, str_out_lo );
		p_u64toa( scaled_value_hi, str_out_hi );
	}
}

static const char* _formatChips( PString& ret, int locale, const Hand& hand, const char* currSign, 
								const char* amountStr, bool isTournHeader = false )
{
	if( ( hand.chipsType == REAL_CHIPS ) || 
		( hand.chipsType == BLITZ_REAL_CHIPS ) || 
		( hand.chipsType == POKER_DUEL_CHIPS && (hand.handFlags & hFlagPlayMoney) == 0 ) || 
		( isTournHeader && !hand.tournIsPlayMoney ) )
	{
		ret.cut( 0 );
		if( currSign && *currSign )
		{
			PMsg2( ret, LocalesTable, locale, P_TXT( TXTINT_Format_Money_Sign ), currSign, amountStr );
		}
		else
		{
			PString dollar;
			PMsg( dollar, LocalesTable, locale, P_TXT( TXTHHY_X ) );
			PMsg2( ret, LocalesTable, locale, P_TXT( TXTINT_Format_Money_Sign ), dollar, amountStr );
		}
	}
	else
	{
		ret = amountStr;
	}

	return ret;
}

struct CurrSignMapItem
{
	const char* code;
	PUNICHAR sign;

	bool operator < ( const CurrSignMapItem& other ) const
	{ return PString::compareIgnoreCase( code, other.code ) < 0; }
};

static const CurrSignMapItem _currSignMap[] = // must be sorted by code
{
	"CAD",    '$',
	"EUR", 0x20AC,
	"GBP", 0x00A3,
	"INR", 0x20B9,
	"USD",    '$',
};
static const CurrSignMapItem* _currSignMapEnd = _currSignMap + sizeof(_currSignMap)/sizeof(*_currSignMap);


static void appendOneLineHandHistoryEvent( bool admin, const SrvTime& when, PString& historyStr, PString& eventHistoryStr )
{
	if( eventHistoryStr.length() > 0 )
	{
		if( admin )
		{ //SrvTime when;
			char str[ 32 ];
			sprintf( str, "%02d:%02d:%02d  ", when._hour, when._minute, when._second );
			i18nAppend( historyStr, str );
		}

		i18nAppend( historyStr, eventHistoryStr );
		eventHistoryStr.cut( 0 );   // clear this string

		if( historyStr[ historyStr.length() - 1 ] != '\n' )
			i18nAppendChar( historyStr, '\n' );
	}
}

static const char* convertChipsStringToScaledChipsString( const char* src, const UINT32 scalePM, PString& strWinnings) // PYR-25238
{
	char* pEnd;
	double winnings = strtod( src, &pEnd );
	UINT64 winningsCents = (UINT64)(winnings * 100); // actual value is 32 bit but use 64 bit container to avoid casting in the next statement //-V2004
	strWinnings.assign("").appendUint64( ( winningsCents * scalePM) / 100 ); // converts dollars to cents, to UINT64 and apply the rest of the scale

	return strWinnings.c_str();
}

static void _formatScaledUINT32( UINT32 value, UINT32 scale, UINT32 denominator, char* str_out ) // PYR-25238 
{
	if( denominator == 0 )
	{
		PLog (LOG_TRACE_FAULTY_LOGIC ": denominator 0 set to 1"); 
		PASSERT5(0);
		denominator = 1;
	}
	UINT64 scaled_value = ( (UINT64)value * scale ) / denominator;

	p_u64toa( scaled_value, str_out );
}

// PYR-45395
static const char *_formatHandStartBounty(PString& bountyStr, int headBountyAmount, UINT32 scalePM, UINT32 denominator, int locale, const Hand& hand, const PStringX& currSign)
{
	bountyStr.cut(0);

	if( headBountyAmount > 0 && hand.isProgessiveKnockoutTournHand() )  // only show starting head bounty in progressive knockout
	{
		char str_stakes[ 32 ];
		PString str_chips_buf;

		i18nAppend(bountyStr, ", ");

		if(hand.tournIsPlayMoney)
		{
			_formatScaledChips( headBountyAmount, scalePM, denominator, str_stakes );
		}
		else
		{
			UINT32 rest = headBountyAmount % 100;
			if( rest )
				sprintf( str_stakes, "%u.%02u", headBountyAmount / 100, rest );
			else
				sprintf( str_stakes, "%u", headBountyAmount / 100 );
		}

		PMsg1( bountyStr, LocalesTable, locale, P_TXT( TXTHHY_N0_bounty ), _formatChips( str_chips_buf, locale, hand, currSign, str_stakes, true ) );
	}

	return bountyStr;
}

static void _fillCurrSign(const PString& handCurrency, PString& currString, PStringX& currSign)
{
	if( handCurrency.length() )
	{
		const CurrSignMapItem cmp = { handCurrency };
		_DEBUG_CHECK_TABLE_ORDER( _currSignMap, _currSignMapEnd );
		const CurrSignMapItem* iter = lower_bound( _currSignMap, _currSignMapEnd, cmp );
		if( iter != _currSignMapEnd && handCurrency.equalsIgnoreCase( (*iter).code ) )
		{
			currSign.append( (*iter).sign );
			currString.assign( " " );
			i18nAppend( currString, (*iter).code ); //Bug #11510: Item 4.4
		}
	}
	else
	{
		currString.assign(" USD" ); //default
	}
}


void formatPowerEvent( bool isServer, const Hand& hand, const TableHistoryItem& historyItem, const PString& playerName, const PString& targetName, int locale, const char* myUserAlias, bool admin, PString &eventHistoryStr, int& powerIdAffectingBoard );
void formatPowerEffect( bool isServer, const Hand& hand, const TableHistoryItem& historyItem, const PString& playerName, int locale, const char* myUserAlias, PString * boardCardsPstr, bool runItTwice, bool admin, PString &eventHistoryStr, int& powerIdAffectingBoard );
void formatPowerEvent2( bool isServer, const Hand& hand, const TableHistoryItem& historyItem, const PString& playerName, const PString& targetName, int locale, const char* myUserAlias, bool admin, PString &eventHistoryStr, bool &hasStretchRound, int& powerIdAffectingBoard );

// PYR-106133
const char* getUserName( const PString& name, const PString (&userAlias)[MAX_TABLE_PLAYERS], INT8 position, const char* myUserAlias, bool admin, bool showStealthTableAlias, const char* myRealUserId )
{
	// if name is empty then return immediately since there's no valid name or alias to get
	if( !*name.c_str() )
	{
		return name;
	}
	// if looking at your own history then return your name. If myRealUserId is set then return that since this is passed in by the client when retrieving hand histories locally for stealth tables
	else if( name.equals( myUserAlias ) )
	{
		return *myRealUserId ? myRealUserId : name.c_str();
	}
	// return the name if in admin mode or showStealthTableAlias is false
	else if( admin || !showStealthTableAlias )
	{
		return name;
	}
	// return the alias name if we have a valid position
	else if( position >= 0 && position < MAX_TABLE_PLAYERS )
	{
		return userAlias[position];
	}
	// shouldn't get here, log a faulty logic if we do
	else
	{
		PLog( LOG_TRACE_FAULTY_LOGIC ": getUserName() returning fallback name: n = %s, p = %d, uId = %s, a = %d, s = %d, rUID = %s", 
			  name.c_str(), position, myUserAlias, admin, showStealthTableAlias, myRealUserId );

		return name;
	}
}

void formatHandHistory( int locale, const Hand& hand, const char* myUserAlias, UINT32 timezone, const vector< PString >& myPattern, UINT32 brandId, PString& historyStr, PString& handSummary, bool admin, bool closedCards, ChatFilterHelper* pChatFilterHelper, bool isServer, const char* myRealUserId )
{
	// PYR-25238 - for tournaments the hand is in chips and is not scaled but we need to use the actual scale for tourn buy-in
	const UINT32 effectiveScalePM = hand.getEffectiveScalePM();
	const UINT32 storedScalePM = hand.getStoredScalePM();

	if( myUserAlias == nullptr )
	{
		myUserAlias = "";
	}

	if( myRealUserId == nullptr )
	{
		myRealUserId = "";
	}

	SrvTime when;
	bool beenInHand = false;

	int round = -1;
	bool hasStretchRound = false;
	int sidePot = 0, sidePotUnfold = 0;
	PString str_game, str_tourn;
	PString str_dealer;

	char str_stakes[ 32 ] = "", str_hand[ 32 ] = "", str_nTourn[ 32 ] = "";
	char str_date_0[ 16 ] = "";
	//	int stakeSmall = ( _potStruct == StructLimit || _handType == HandTypeStud ) ?  _loBet : _sBlind;
	//	int stakeBig = ( _potStruct == StructLimit || _handType == HandTypeStud ) ? _hiBet : _loBet;

	const UINT32 Denominator = ( hand.tournType == DenominatedTableTour || hand.chipsType == POKER_DUEL_CHIPS ) ? 1 : 100;  // PYR-36544

	PString currString; // table currency string
	PStringX currSign; // table currency sign
	_fillCurrSign(hand.currency, currString, currSign);

	PString str_chips_buf, str_chips_buf_2, str_chips_buf_3; // for _formatChips( ... )

	if( hand.structure == StructLimit )
	{
		_formatScaledChipsLoHi( hand.loBet, hand.hiBet, effectiveScalePM, Denominator, str_stakes, str_nTourn );
	}
	else
	{
		_formatScaledChipsLoHi( hand.smallBlindSize, hand.loBet, effectiveScalePM, Denominator, str_stakes, str_nTourn );
		if( hand.cap )
			_formatScaledChips( hand.cap, effectiveScalePM, Denominator, str_hand );
	}

	_formatChips( str_chips_buf_2, locale, hand, currSign, str_nTourn );
	if( hand.isGiantBlindHand() )
	{
		char str_giantblind[ 32 ] = "";
		_formatScaledChips( hand.hiBet, effectiveScalePM, Denominator, str_giantblind );
		PString str_giant_blind_buf;
		_formatChips( str_giant_blind_buf, locale, hand, currSign, str_giantblind );
		i18nAppend( str_chips_buf_2, "/" );
		i18nAppend( str_chips_buf_2, str_giant_blind_buf );
	}


	if( hand.isButtonBlindHand() )  // PYR-74693
	{
		// not a cap game, not a mixed game
		PASSERT5( !hand.cap );

		//(Button Blind $5 - Ante $5 USD)
		_formatScaledChips( hand.ante, effectiveScalePM, Denominator, str_stakes );
		PMsg3( str_dealer, LocalesTable, locale, P_TXT( TXTHHY_X040XButton_Blind_N0___Ante_N1_ELL ),
			str_chips_buf_2,    // big blind
			_formatChips( str_chips_buf, locale, hand, currSign, str_stakes ),  // ante
			hand.chipsType ? "" : currString.c_str() );  // currency
	}
	else
	{
		if( hand.newGameFlag >= MultiGameTypeShift && hand.tournType == NoTableTour )
			//P_TXT( TXTHHY_X_N0N1_N2N3 )	{ ", %0%1/%2%3", 0x0 },
			PMsg4( str_dealer, LocalesTable, locale, P_TXT( TXTHHY_X_N0N1_N2N3 ), 
				_formatChips( str_chips_buf, locale, hand, currSign, str_stakes ),
				"", 
				str_chips_buf_2,
				hand.chipsType ? "" : currString.c_str() ); 
		else if( hand.cap ) //(%0/%1 - %2 Cap - %3)
			PMsg4( str_dealer, LocalesTable, locale, P_TXT( TXTHHY_X040XN0_N1___N2_Cap___N3X ), 
				_formatChips( str_chips_buf, locale, hand, currSign, str_stakes ),
				str_chips_buf_2,
				_formatChips( str_chips_buf_3, locale, hand, currSign, str_hand ), 
				hand.chipsType ? "" : currString.c_str() );
		else //P_TXT( TXTHHY_X040XN0N1_N2N3X )	{ "\040(%0%1/%2%3)", 0x0 },
			PMsg4( str_dealer, LocalesTable, locale, P_TXT( TXTHHY_X040XN0N1_N2N3X ), 
				_formatChips( str_chips_buf, locale, hand, currSign, str_stakes ), 
				"", 
				str_chips_buf_2,
				hand.chipsType ? "" : currString.c_str() );
	}

	i18nAppendChar( str_game, ' ' ); //PYR-25497
	if( hand.game == HandTypeHoldem ) 
		PMsg( str_game, LocalesTable, locale, P_TXT( TXTHHY_X040HoldXem ) );
	else if( hand.game == HandTypeSplitHoldem ) 
		PMsg( str_game, LocalesTable, locale, P_TXT( TXTHHY_Split_HoldXem ) );
	else if( hand.game == HandTypeShowtimeHoldem )
		PMsg( str_game, LocalesTable, locale, P_TXT( TXTHHY_Showtime_HoldXem ) );
	else if( hand.game == HandTypeOmaha )
		PMsg( str_game, LocalesTable, locale, P_TXT( TXTHHY_X040Omaha ) );
	else if( hand.game == HandTypeSplitOmaha ) 
		PMsg( str_game, LocalesTable, locale, P_TXT( TXTHHY_Split_Omaha ) );
	else if( hand.game == HandTypeShowtimeOmaha )
		PMsg( str_game, LocalesTable, locale, P_TXT( TXTHHY_Showtime_Omaha ) );
	else if( hand.game == HandTypeFusion )
		PMsg( str_game, LocalesTable, locale, P_TXT( TXTHHY_Fusion ) );
	else if (hand.game == HandTypeUnfoldClassic)
		PMsg( str_game, LocalesTable, locale, P_TXT( TXTHHY_Unfold_Holdem ) );
	else if( hand.game == HandTypeStud )
		PMsg( str_game, LocalesTable, locale, P_TXT( TXTHHY_X0407_Card_Stud ) );
	else if( hand.game == HandTypeRazz )
		PMsg( str_game, LocalesTable, locale, P_TXT( TXTHHY_X040Razz ) );
	else if( hand.game == HandTypeDraw )
		PMsg( str_game, LocalesTable, locale, P_TXT( TXTHHY_X0405_Card_Draw ) );
	else if( hand.game == HandType3Draw27 )
		PMsg( str_game, LocalesTable, locale, P_TXT( TXTHHY_X040Triple_Draw_2_7_Lowball ) );
	else if( hand.game == HandTypeSingleDraw27 )
		PMsg( str_game, LocalesTable, locale, P_TXT( TXTHHY_X040Single_Draw_2_7_Lowball ) );
	else if( hand.game == HandType3DrawA5 )
		PMsg( str_game, LocalesTable, locale, P_TXT( TXTHHY_X040Triple_Draw_A_5 ) );
	else if( hand.game == HandTypeBadugi )
		PMsg( str_game, LocalesTable, locale, P_TXT( TXTHHY_X040Badugi ) );
	else if( hand.game == HandType5Omaha )
		PMsg( str_game, LocalesTable, locale, P_TXT( TXTHHY_X0405_Card_Omaha ) );
	else if( hand.game == HandType6Omaha )
		PMsg( str_game, LocalesTable, locale, P_TXT( TXTHHY_6_Card_Omaha ) );
	else if( hand.game == HandTypeCourchevel )
		PMsg( str_game, LocalesTable, locale, P_TXT( TXTHHY_X040Courchevel ) );
	else if( hand.game == HandTypeSixPlusHoldem )
		PMsg( str_game, LocalesTable, locale, P_TXT( TXTHHY_6X_HoldXem ) );
	else if (hand.game == HandTypeDeepWaterHoldem)  // PYR-82834
		PMsg(str_game, LocalesTable, locale, P_TXT(TXTHHY_Deep_Water_HoldXem));
	else if (hand.game == HandTypeDeepWaterOmaha)  // PYR-82834
		PMsg(str_game, LocalesTable, locale, P_TXT(TXTHHY_Deep_Water_Omaha));
	else if (hand.game == HandTypeSwapHoldem)
		PMsg( str_game, LocalesTable, locale, P_TXT(TXTHHY_Swap_HoldXem));
	else if( hand.game == HandTypeTempestHoldem )  // PYR-115738
		PMsg( str_game, LocalesTable, locale, P_TXT( TXTHHY_Tempest_HoldXem ) );

	PString str_hilo;
	if( hand.isHiLo ) //Courchevel is always Hi/Lo
		PMsg( str_hilo, LocalesTable, locale, P_TXT( TXTHHY_X040Hi_Lo ) );

	PString str_limit;
	if( hand.structure == StructLimit )
		PMsg( str_limit, LocalesTable, locale, P_TXT( TXTHHY_X040Limit ) );
	else if( hand.structure == StructPotLimit )
		PMsg( str_limit, LocalesTable, locale, P_TXT( TXTHHY_X040Pot_Limit ) );
	else if( hand.structure == StructNoLimit )
		PMsg( str_limit, LocalesTable, locale, P_TXT( TXTHHY_X040No_Limit ) );
	else if( hand.structure == StructPotNoLimit )
		PMsg( str_limit, LocalesTable, locale, P_TXT( TXTHHY_X040Pot_Limit_Pre_FlopX_No_Lim_ELL ) );
	else
		str_limit = "";

	p_u64toa( hand.handId64, str_hand );

	PString str_multiGame;
	i18nAppendChar( str_multiGame, ' ' ); //PYR-25497

	if( hand.newGameFlag == MultiGameHose )
		PMsg( str_multiGame, LocalesTable, locale, P_TXT( TXTHHY_X040HOSE ) );
	else if( hand.newGameFlag == MultiGameHorse )
		PMsg( str_multiGame, LocalesTable, locale, P_TXT( TXTHHY_X040HORSE ) );
	else if( hand.newGameFlag == MultiGameEightGame )
		PMsg( str_multiGame, LocalesTable, locale, P_TXT( TXTHHY_X040EightGame ) );
	else if( hand.newGameFlag == MultiGameHoldemMixed )
		PMsg( str_multiGame, LocalesTable, locale, P_TXT( TXTHHY_X040HoldXemMixed ) );
	else if( hand.newGameFlag == MultiGameOmahaHiLoMixed )
		PMsg( str_multiGame, LocalesTable, locale, P_TXT( TXTHHY_X040OmahaHiLoMixed ) );
	else if( hand.newGameFlag == MultiGameHoldemOmahaPl )
		PMsg( str_multiGame, LocalesTable, locale, P_TXT( TXTHHY_X040Mixed_PLHXPLO ) );
	else if( hand.newGameFlag == MultiGameTripleStud )
		PMsg( str_multiGame, LocalesTable, locale, P_TXT( TXTHHY_X040Triple_Stud ) );
	else if( hand.newGameFlag == MultiGameHoldemNlOmaha )
		PMsg( str_multiGame, LocalesTable, locale, P_TXT( TXTHHY_X040Mixed_NLH_PLO ) );
	else if( hand.newGameFlag == MultiGameOmahaMixed )
		PMsg( str_multiGame, LocalesTable, locale, P_TXT( TXTHHY_X040Mixed_Omaha ) );
	else
		str_multiGame = "";

	if( hand.tournType != NoTableTour || hand.chipsType == POKER_DUEL_CHIPS )	//if( hand.tournId )  // PYR-36544
	{
		PString nTourn;
		const char *str_p = strchr( hand.tableName, ' ' ); // find a space

		if( str_p ) 
			nTourn.assign( hand.tableName, str_p );

		if(hand.chipsType == POKER_DUEL_CHIPS)
		{
			PMsg1( str_tourn, LocalesTable, locale, P_TXT( TXTHHY_Poker_Duel_XN0 ), nTourn ); 
		}
		else if( hand.isZoomTournHand() )
		{
			// PYR-40976
			PString zoomName;
			i18nFormatBrandedName( zoomName, LocalesTable, locale, brandId, eBrandedName_Zoom, false );
			PMsg2( str_tourn, LocalesTable, locale, P_TXT( TXTHHY_N1_Tournament_XN0 ), nTourn, zoomName ); 
		}
		else
		{
			PMsg1( str_tourn, LocalesTable, locale, P_TXT( TXTHHY_Tournament_XN0 ), nTourn ); 
		}

		PMsg( str_tourn, LocalesTable, locale, P_TXT( TXTHHY_X_1 ) ); 

		if(hand.chipsType == POKER_DUEL_CHIPS)    // PYR-37127
		{
			str_stakes [ 0 ] = '\x0';
			PString moneyStr = " ";

			// no FPP, only real money or play money
			if( !( hand.buyIn64 || hand.rake64 ) )
			{
				PMsg( str_tourn, LocalesTable, locale, P_TXT( TXTHHY_X040Freeroll ) );
			}
			else
			{
				bool isPlayMoney = (hand.handFlags & hFlagPlayMoney) != 0;
				UINT32 buyInRest = hand.buyIn64 % 100;
				UINT32 rakeRest = hand.rake64 % 100;

				if(!isPlayMoney && (buyInRest || rakeRest))
				{
					sprintf( str_stakes, "%" PRIu64 ".%02u", UINT64(hand.buyIn64 / 100), buyInRest );
					i18nAppend( moneyStr, _formatChips( str_chips_buf, locale, hand, currSign, str_stakes ) );
					i18nAppendChar( moneyStr, '+' );
					sprintf( str_stakes, "%" PRIu64 ".%02u", UINT64(hand.rake64 / 100), rakeRest );
					i18nAppend( moneyStr, _formatChips( str_chips_buf, locale, hand, currSign, str_stakes ) );
				}
				else
				{
					sprintf( str_stakes, "%" PRIu64, UINT64(hand.buyIn64 / 100) );
					i18nAppend( moneyStr, _formatChips( str_chips_buf, locale, hand, currSign, str_stakes ) );
					i18nAppendChar( moneyStr, '+' );
					sprintf( str_stakes, "%" PRIu64, UINT64(hand.rake64 / 100) );
					i18nAppend( moneyStr, _formatChips( str_chips_buf, locale, hand, currSign, str_stakes ) );
				}

				if( !isPlayMoney )
					i18nAppend( moneyStr, currString ); //Bug #11510: Item 4.4
			}

			i18nAppend( str_tourn, moneyStr );
		}
		else if( hand.tournIsPlayMoney >= 0 ) // -1 means default, i.e. no data available
		{ // Bug #3526-II: 
			// ...Tournament #19571119, $200+$15 Hold'em... 
			// ...Tournament #20877014, 1500FPP Hold'em No Limit... 
			str_stakes [ 0 ] = '\x0';
			PString moneyStr = " ";

			if( hand.tournFppBuyIn )
			{
				sprintf( str_stakes, "%u SC", UINT32(hand.tournFppBuyIn) );
				i18nAppend( moneyStr, str_stakes );
			}
			else
			{
				if( !( hand.tournBuyIn || hand.tournRake ) )
				{
					if( !hand.pppId )
					{
						PMsg( str_tourn, LocalesTable, locale, P_TXT( TXTHHY_X040Freeroll ) );
					}
				}
				else
				{
					// [AO 2010-01-26] PYR-12954: The buy-in in the script includes knockout bounty (if any)
					UINT32 tournBuyIn = hand.tournBuyIn - hand.tournBounty;

					UINT32 rest = tournBuyIn%100;
					UINT32 rest0 = hand.tournRake%100;
					UINT32 rest1 = hand.tournBounty%100;

					if( !hand.tournIsPlayMoney && ( rest || rest0 || rest1 ) )
					{
						//sprintf( str_stakes, " $%u.%02u+$%u.%02u", hand.tournBuyIn/100, rest, hand.tournRake/100, rest0 );
						sprintf( str_stakes, "%u.%02u", tournBuyIn/100, rest );
						i18nAppend( moneyStr, _formatChips( str_chips_buf, locale, hand, currSign, str_stakes, true ) );
						i18nAppendChar( moneyStr, '+' );
						if( hand.tournBounty )
						{
							sprintf( str_stakes, "%u.%02u", UINT32(hand.tournBounty/100), rest1 );
							i18nAppend( moneyStr, _formatChips( str_chips_buf, locale, hand, currSign, str_stakes, true ) );
							i18nAppendChar( moneyStr, '+' );
						}
						sprintf( str_stakes, "%u.%02u", UINT32(hand.tournRake/100), rest0 );
						i18nAppend( moneyStr, _formatChips( str_chips_buf, locale, hand, currSign, str_stakes, true ) );
					}
					else // PYR-25238 - play money tourn - use scaled buy-in/rake/bounty
					{
						_formatScaledUINT32( tournBuyIn, storedScalePM, 100, str_stakes ); // PYR-25238 
						//sprintf( str_stakes, "%u", tournBuyIn/100 );
						i18nAppend( moneyStr, _formatChips( str_chips_buf, locale, hand, currSign, str_stakes, true ) );
						i18nAppendChar( moneyStr, '+' );
						if( hand.tournBounty )
						{
							_formatScaledUINT32( hand.tournBounty, storedScalePM, 100, str_stakes ); // PYR-25238 
							//sprintf( str_stakes, "%u", hand.tournBounty/100 );
							i18nAppend( moneyStr, _formatChips( str_chips_buf, locale, hand, currSign, str_stakes, true ) );
							i18nAppendChar( moneyStr, '+' );
						}
						_formatScaledUINT32( hand.tournRake, storedScalePM, 100, str_stakes ); // PYR-25238 
						//sprintf( str_stakes, "%u", hand.tournRake/100 );
						i18nAppend( moneyStr, _formatChips( str_chips_buf, locale, hand, currSign, str_stakes, true ) );
					}

					if( !hand.tournIsPlayMoney )
						i18nAppend( moneyStr, currString ); //Bug #11510: Item 4.4
				}
			}
			i18nAppend( str_tourn, moneyStr );
		}
	}
	//HISTORY_POKERSTARS PokerStars 
	//PMsg( historyStr, LocalesTable, locale, P_TXT( TXTHHY_PokerStarsX040 ) );
	//PMsg1( historyStr, LocalesTable, locale, P_TXT( TXTHHY_Game_XN0XX040 ), str_hand );

	PString brandName;
	i18nFormatBrandedName( brandName, LocalesTable, locale, brandId, eBrandedName_Brand, false );

	if( *hand.clubName )
	{ //PYR-16681:
		PMsg2( historyStr, LocalesTable, locale, P_TXT( TXTHHY_N1_Home_Game_Hand_XN0XX040 ), str_hand, brandName );
		i18nAppendChar( historyStr, '{' );
		i18nAppend( historyStr, hand.clubName );
		i18nAppendChar( historyStr, '}' );
		i18nAppendChar( historyStr, ' ' );
	}
	else if( hand.chipsType == BLITZ_REAL_CHIPS || hand.chipsType == BLITZ_PLAY_CHIPS )
	{
		// PYR-40976
		PString zoomName;
		i18nFormatBrandedName( zoomName, LocalesTable, locale, brandId, eBrandedName_Zoom, false );
		PMsg3( historyStr, LocalesTable, locale, P_TXT( TXTHHY_N1_N2_Hand_XN0XX040 ), str_hand, brandName, zoomName );
	}
	else
	{
		// PYR-40976
		PMsg2( historyStr, LocalesTable, locale, P_TXT( TXTHHY_N1_Hand_XN0XX040 ), str_hand, brandName );
	}

	i18nAppend( str_tourn, str_multiGame );

	if( hand.newGameFlag >= MultiGameTypeShift )
	{
		if( *str_game ) // remove a leading space:
			//str_game.assign( str_game.c_str() + 1 );
				str_game = PString::ltrim( str_game );

		if( hand.tournType == NoTableTour )
		{//P_TXT( TXTHHY_N0_XN1N2N3N4X )  { "%0 (%1%2%3%4)", 0x0 },
			PMsg5( historyStr, LocalesTable, locale, P_TXT( TXTHHY_N0_XN1N2N3N4X ), str_tourn, str_game, str_hilo, str_limit, str_dealer );
			str_dealer = "";
		}
		else
		{
			const char *dummy = "";
			PMsg5(historyStr, LocalesTable, locale, P_TXT( TXTHHY_N0_XN1N2N3N4X ), str_tourn, str_game, str_hilo, str_limit, dummy);
		}
	}
	else //HISTORY_LABEL Game #%0: %1%2%3%4  //P_TXT( TXTHHY_N0N1N2N3 ) { "%0%1%2%3", 0x0 },
	{
		PMsg4(historyStr, LocalesTable, locale, P_TXT(TXTHHY_N0N1N2N3), str_tourn, str_game, str_hilo, str_limit);
	}

	//if( hand.speedDisplay == SpeedDisplayTurbo )
	//	PMsg( historyStr, LocalesTable, locale, P_TXT( TXTHHY_X040Turbo ) );
	//else if( hand.speedDisplay == SpeedDisplayHyper )
	//	PMsg( historyStr, LocalesTable, locale, P_TXT( TXTHHY_X040Hyper_Turbo ) );

	// The patterns from specs:  Bugs#: 3872, 2764
	//PokerStars Game #5749537151: Tournament #28909810, $20+$2 Omaha Pot Limit - Level IV (50/100) - 2006/07/31 - 16:55:41 (ET)
	//PokerStars Game #7006091899: Tournament #24502577, $2+$1 HORSE (7 Card Stud Hi/Lo Limit) - Level XV (40000/80000)...

	//PokerStars Game #5749532153:  7 Card Stud Hi/Lo Limit ($5/$10) - 2006/07/31 - 16:55:13 (ET)
	//PokerStars Game #4301007835:  HORSE (Omaha Hi/Lo Limit, 20/40) - 2006/07/26 - 14:54:58 (ET)

	// printf "- Level III (150/300)- 2001/06/10-17:13:25 (EST)" :
	str_game = "";
	str_tourn = "";

	if( hand.tournType != NoTableTour )
		//if( hand.tournId )
	{
		int tLevel = hand.tournLevel % 1000;
		int tRound = hand.tournLevel / 1000;

		if( tRound )
			if( tRound > 100 )                              //-V547
			{ //HISTORY_ROUND  Match Round %0
				sprintf( str_nTourn, "%u", tRound );
				PMsg1( str_game, LocalesTable, locale, P_TXT( TXTHHY_X040Match_Round_N0X ), str_nTourn ); 
			}
			else
				PMsg1( str_game, LocalesTable, locale, P_TXT( TXTHHY_X040Match_Round_N0X ), numRome[ tRound ] ); 

		if( tLevel > 100 )
		{ //HISTORY_LEVEL  - Level %0
			sprintf( str_nTourn, "%u", tLevel );
			PMsg2( str_tourn, LocalesTable, locale, P_TXT( TXTHHY_X040_N0_Level_N1 ), str_game, str_nTourn ); 
		}
		else
			PMsg2( str_tourn, LocalesTable, locale, P_TXT( TXTHHY_X040_N0_Level_N1 ), str_game, numRome[ tLevel ] ); 
	}
	i18nAppend( historyStr, str_tourn );
	i18nAppend( historyStr, str_dealer );
	if( *hand.aamsId ) //PYR-16339:
		PMsg1( historyStr, LocalesTable, locale, P_TXT( TXTHHY_AAMS_ID ), hand.aamsId );

	//Bug #9230:
	when._year = hand.started._year;
	when._month = hand.started._month;
	when._day = hand.started._day;
	when._hour = hand.started._hour;
	when._minute = hand.started._minute;
	when._second = hand.started._second;
	i18nAppendChar( historyStr, ' ' );
	i18nAppendChar( historyStr, '-' );
	i18nAppendChar( historyStr, ' ' );
	_formatDateTimeAltTZ( historyStr, LocalesTable, locale, when, timezone );
	i18nAppendChar( historyStr, '\n' );

	// printf "Table '9 51': (Play Money)- The button is at the seat #2"
	PString str_one2one;
	if( hand.isOneOnOne )
	{
		if( hand.isOneOnOne == 1 )
			PMsg( str_one2one, LocalesTable, locale, P_TXT( TXTHHY_X040One_on_One ) );
		else if( hand.isOneOnOne <= MAX_TABLE_PLAYERS )
		{ // Bug #3526-I: n-max' 
			sprintf( str_date_0, " %u-max", hand.isOneOnOne ); 
			str_one2one = str_date_0;
		}
	}
	//Table 'Melpomene IV' 8-max H.O.S.E Seat #1 is the button
	PString str_playMoney;
	if( hand.chipsType == PLAY_CHIPS )
		PMsg( str_playMoney, LocalesTable, locale, P_TXT( TXTHHY_X040XPlay_MoneyX ) );
	//else
	//	;//PMsg( str_playMoney, msgTable, P_TXT( TXT_HISTORY_REALMONEY ) );

	char str_number[ 5 ] = "";
	str_dealer = "";
	if( hand.dealer >= 0 )
	{
		sprintf( str_number, "%d", hand.dealer + 1 ); 
		PMsg1( str_dealer, LocalesTable, locale, P_TXT( TXTHHY_X040Seat_XN0_is_the_button ), str_number );
		if(hand.handFlags & hFlagDealerButtonWonFromLastHand)  // PYR-32587
		{
			i18nAppend( str_dealer, " (");
			PMsg( str_dealer, LocalesTable, locale, P_TXT( TXTHHY_won_the_previous_hand ) );
			i18nAppendChar( str_dealer, ')' );
		}
	}

	// PYR-106133: Support Stealth tables
	bool isStealthTable = hand.isStealthTableHand();
	bool showStealthTableAlias = false;
	
	PString str_tableDesc( str_one2one );
	if( isStealthTable )
	{
		str_tableDesc = " (Stealth)";	// no internationalization required

		str_tableDesc.append( str_one2one ); 

		time_t timeNow = plib_time( 0 );
		
		// if the hand finished less than 24 hours ago then show aliases on stealth tables
		if( timeNow - hand.finished.encode() < SECONDS_IN_A_DAY )
		{
			showStealthTableAlias = true;
		}
	}

	//HISTORY_TABLE Table '%0'%1%2%3
	PMsg4( historyStr, LocalesTable, locale, P_TXT( TXTHHY_Table_XN0XN1N2N3 ), hand.tableName, str_tableDesc, str_playMoney, str_dealer );
	i18nAppendChar( historyStr, '\n' );

	int userSeats     [ MAX_TABLE_PLAYERS ]; //function( usersSeat, usersInHand )
	int roundFold     [ MAX_TABLE_PLAYERS ]; //function( roundFold, usersInHand )
	INT32 userNet    [ MAX_TABLE_PLAYERS ];
	INT32 userNet2   [ MAX_TABLE_PLAYERS ];
	INT32 pot[MAX_NUM_POTS];
	INT32 potUnfold[MAX_NUM_POTS];
	int playerChipsBet[ MAX_TABLE_PLAYERS ];
	int playerHeadBountyBeforeHand[ MAX_TABLE_PLAYERS ];
	int maxBet = 0; // for RaiseTo calculation
	int tableCap = hand.cap; // 0; //Bug #14309
	int userCashoutIndexInEvents[MAX_TABLE_PLAYERS]; //PYR-79935
	PString userAlias[MAX_TABLE_PLAYERS]; // PYR-106133

	int i;
	for( i = 0; i < MAX_NUM_POTS; ++i )
	{
		pot[ i ] = 0;
		potUnfold[i] = 0;
	}
	for( i = 0; i < MAX_TABLE_PLAYERS; ++i )
	{ // init array of userSeats and userNets:
		playerChipsBet[ i ] = 0;
		userSeats[ i ] = -1; 
		roundFold[ i ] = -1;
		userNet[ i ] = 0;
		userNet2[ i ] = 0;
		playerHeadBountyBeforeHand[ i ] = 0;
		userCashoutIndexInEvents[ i ] = -1;
	}

	int eventsNum = hand.history.size();
	if( !eventsNum )
	{
		char buf[ 256 ];
		sprintf( buf, "Problematic handID=%s! Contact support, please!", str_hand );
		throw PError( buf );
	}

	int usersNum = hand.users.size();
	PASSERT( usersNum <= MAX_TABLE_PLAYERS );

	beenInHand = false;
	for( i = 0; i < usersNum; ++i )
	{
		userSeats[ hand.users[ i ].position ] = i; // for having number of user in vector to be associated with seat's number
		if( hand.users[ i ].name.equals( myUserAlias ) )
			beenInHand = true;

		// PYR-106133: By default, set the alias names to the users' names so that the client can generate the hand histories for stealth tables
		// If the TableHistoryExtraPlayerAliasPubEvent was fired, as it is on the server, then we'll update these later to the players' true alias values
		userAlias[hand.users[i].position] = hand.users[i].name;
	}

	PString str_action;
	PString bestCombPstr[ MAX_TABLE_PLAYERS ];
	PString allCardsPstr[ MAX_TABLE_PLAYERS ];
	PString shownCardsPstr[ MAX_TABLE_PLAYERS ];
	PString foldAndShowCards[ MAX_TABLE_PLAYERS ];	// #15261
	PString boardCardsPstr[2];
	const char* str_p = "";

	BYTE _bestHigh = 0;
	Card _bestHighHand[ 5 ];

	//PTRACE("Format::hand=%s", str_hand );
	// Main Loop of History Events here:

	PString bestCombPstr2[ MAX_TABLE_PLAYERS ]; 
	bool runItTwice = (hand.game == HandTypeSplitHoldem || hand.game == HandTypeSplitOmaha) ? true : false;
	bool freezeBoardCardsWhenAllIn = false;

	int boardCounter = 0;

	// This is needed for properly reporting cards dealt to the board because of a power. Zero means regular dealing.
	int powerIdAffectingBoard = 0;

	PString eventHistoryStr;
	PString powerNameStr;
	PString unfoldAnteReturnSummaryStr;

	for( int step = 0; step < 2; ++step ) // PYR-20821  - need 2 passes
	{
		if (step==1) 
		{ // Dump Seat info
			struct UserOutOfHand
			{
				PString name;
				UINT32	startAmount;
				bool printed;
				UserOutOfHand()
				{
					startAmount = 0;
					printed = false;
				}
			} sitOutUsers[ MAX_TABLE_PLAYERS ];

			if( admin )
			{
				bool beenInHand2 = false;
				for( i = 0; i < eventsNum; ++i )
				{
					if( hand.history[ i ].eventCode == TABLE_HISTORY_DEALING && hand.history[ i ].action == 0 )
					{
						beenInHand2 = true;
						continue;
					}
					else if( hand.history[ i ].eventCode == TABLE_HISTORY_BEENSITOUT )
					{
						if( hand.history[ i ].position >= 0 )  // just for code protection
						{
							// PYR-106133: OK to directly use the player's name since we're in admin mode here
							sitOutUsers[ hand.history[ i ].position ].name.assign( hand.history[ i ].userId );
							sitOutUsers[ hand.history[ i ].position ].startAmount = hand.history[ i ].amount;
						}
						else
						{
#ifndef PROTOCOL_CLIENT_ONLY
							PLog( LOG_TRACE_FAULTY_LOGIC "TABLE_HISTORY_BEENSITOUT hand.history[ %d ].position = %d", i, hand.history[ i ].position );
#endif
						}
					}
					else if( beenInHand2 )
					{
						break;
					}
				}
			}
			//	if ( hand.chipsType == REAL_CHIPS )
			//		PMsg( str_dealer, msgTable, P_TXT( TXT_DEALERCHAT_DOLLAR ) ); 
			//	else 
			//		str_dealer = "";

			for( i = 0; i < usersNum; ++i )
			{ // List of Players in Hand:
				PASSERT( hand.users[ i ].position >= 0 && hand.users[ i ].position < MAX_TABLE_PLAYERS );

				if( admin )
				{
					for( int j = 0; j < hand.users[ i ].position; ++j )
					{
						if( *sitOutUsers[ j ].name && !sitOutUsers[ j ].printed )
						{
							sprintf( str_number, "%d", j + 1 );
							_formatScaledChips( sitOutUsers[ j ].startAmount, effectiveScalePM, Denominator, str_stakes );

							PString bountyStr;
							PMsg5( historyStr, LocalesTable, locale, P_TXT( TXTHHY_Seat_N0X_N1_XN2N3_in_chipsN4XX_ELL ), str_number, sitOutUsers[ j ].name, 
								_formatChips( str_chips_buf, locale, hand, currSign, str_stakes ), "",
								_formatHandStartBounty(bountyStr, playerHeadBountyBeforeHand[ j ], storedScalePM, 100, locale, hand, currSign) );

							PMsg( historyStr, LocalesTable, locale, P_TXT( TXTHHY_is_sitting_out ) );
							i18nAppendChar( historyStr, '\n' );

							sitOutUsers[ j ].printed = true;
						}
					}
				}

				sprintf( str_number, "%d", hand.users[ i ].position + 1 ); // itoa( Id, str_, 10 );

				_formatScaledChips( hand.users[ i ].startAmount, effectiveScalePM, Denominator, str_stakes );

				// PYR-106133: For Stealth tables we set the player's name to their alias
				PString str_userName;

				if( isStealthTable )
				{
					str_userName = userAlias[ hand.users[ i ].position ];

					// AQA-26869 client side only: client may have replaced the Hero's alias with his real user Id, but we still want to show his alias in the beginning
					if( !isServer && *myRealUserId && *myUserAlias && strcmp( myRealUserId, myUserAlias ) != 0 && hand.users[ i ].name.equals( myRealUserId ) && str_userName.equals( myRealUserId ) )
					{
						str_userName.assign( myUserAlias ).append( " (" ).append( myRealUserId ).append( ")" );
					}
					// if it's the player them self requesting the hand history then include their myRealUserId
					else if( hand.users[ i ].name.equals( myUserAlias ) )
					{
						const char *myName = *myRealUserId ? myRealUserId : myUserAlias;  // in server side HH formating, myRealUserId is usually empty and myUserAlias may be the real user id
						if( !str_userName.equals( myName ) )
						{
							str_userName.append( " (" ).append( myName ).append( ")" );
						}
					}
					// include the real userId if in admin mode or if it's time to show them 
					else if( admin || !showStealthTableAlias )
					{
						if( !str_userName.equals( hand.users[ i ].name ) )
						{
							str_userName.append( " (" ).append( hand.users[ i ].name ).append( ")" );
						}
					}
				}
				else
				{
					str_userName = hand.users[i].name;
				}

				//HISTORY_PLAYERS Seat %0: %1 (%2%3 in chips) 
				if(hand.isStormHand() && (hand.isManaCost() || admin || hand.users[ i ].name.equals( myUserAlias )))
				{
					PString startingPowers;
					for(size_t powerIdx = 0; powerIdx < hand.users[i].startingPowers.size(); ++powerIdx)
					{
						if(powerIdx > 0)
						{
							i18nAppend( startingPowers, ", " );
						}
						startingPowers.append(_powerName(hand.users[i].startingPowers[powerIdx], locale, powerNameStr));
					}
					// If we have no powers, state so.
					if(hand.users[i].startingPowers.size() == 0)
					{
						PMsg( startingPowers, LocalesTable, locale, P_TXT( TXTHHY_no_powers ) );
					}

					PString startingMana;
					startingMana.appendInt(hand.users[i].startingMana);

					// Substitute the regular sentence.
					if(hand.isManaCost() && (admin || hand.users[ i ].name.equals( myUserAlias )))
					{
						// Show both starting mana and starting powers.
						PMsg5( historyStr, LocalesTable, locale, P_TXT( TXTHHY_Seat_N0X_N1_XN2_in_chipsX_N3_e_ELL ), str_number, str_userName,
							_formatChips( str_chips_buf, locale, hand, currSign, str_stakes ), startingMana, startingPowers );
					}
					else if(hand.isManaCost())
					{
						// Show only starting mana.
						PMsg4( historyStr, LocalesTable, locale, P_TXT( TXTHHY_Seat_N0X_N1_XN2_in_chipsX_N3_e_ELL_1 ), str_number, str_userName,
							_formatChips( str_chips_buf, locale, hand, currSign, str_stakes ), startingMana );
					}
					else
					{
						// Show only starting powers.
						PMsg4( historyStr, LocalesTable, locale, P_TXT( TXTHHY_Seat_N0X_N1_XN2_in_chipsX_N3XX_ELL ), str_number, str_userName,
							_formatChips( str_chips_buf, locale, hand, currSign, str_stakes ), startingPowers );
					}
				}
				else
				{
					PString bountyStr;
					PMsg5( historyStr, LocalesTable, locale, P_TXT( TXTHHY_Seat_N0X_N1_XN2N3_in_chipsN4XX_ELL ), str_number, str_userName,
						_formatChips( str_chips_buf, locale, hand, currSign, str_stakes ), "", 
						_formatHandStartBounty(bountyStr, playerHeadBountyBeforeHand[ hand.users[ i ].position ], storedScalePM, 100, locale, hand, currSign) );
				}

				if( Been_sitOut & hand.users[ i ].result )
					PMsg( historyStr, LocalesTable, locale, P_TXT( TXTHHY_is_sitting_out ) );

				if( Been_OutOfHand & hand.users[ i ].result )
					PMsg( historyStr, LocalesTable, locale, P_TXT( TXTHHY_out_of_hand_Xmoved_from_anothe_ELL ) );
				//out of hand (moved from another table into small blind)
				i18nAppendChar( historyStr, '\n' );

			} // end for( i = 0; i < usersNum; ++i )

			if( admin )
			{
				for( int j = 0; j < MAX_TABLE_PLAYERS; ++j )
				{
					if( *sitOutUsers[ j ].name && !sitOutUsers[ j ].printed )
					{
						sprintf( str_number, "%d", j + 1 );
						_formatScaledChips( sitOutUsers[ j ].startAmount, effectiveScalePM, Denominator, str_stakes );

						PString bountyStr;
						PMsg5( historyStr, LocalesTable, locale, P_TXT( TXTHHY_Seat_N0X_N1_XN2N3_in_chipsN4XX_ELL ), str_number, sitOutUsers[ j ].name, 
							_formatChips( str_chips_buf, locale, hand, currSign, str_stakes ), "", 
							_formatHandStartBounty(bountyStr, playerHeadBountyBeforeHand[ j ], storedScalePM, 100, locale, hand, currSign) );

						PMsg( historyStr, LocalesTable, locale, P_TXT( TXTHHY_is_sitting_out ) );
						i18nAppendChar( historyStr, '\n' );
					}
				}
			}
		} // End Dump Seat info

		// PYR-35726
		bool isAnteUpPowerStarted = false;
		int anteUpPlayer = -1;

		for( i = 0; i < eventsNum; ++i )
		{
			// step == 0 is for printing some special events that happened before the hand started (rebuys, addons, auto deal results)
			// step == 1 is for priting events of the hand
			if (step==0)
			{
				// on the first pass we are interested only in full processing of auto deal results (TABLE_HISTORY_EXTRA event code with specific parameters)
				// and addons and re-buys (TABLE_HISTORY_ALLIN event code with specific parameters)
				// all other events should be skipped (continue;)

				if( hand.history[ i ].eventCode == TABLE_HISTORY_EXTRA )
				{
					// collect context from bounty before hand events before continue
					if( hand.history[i].action == TH_EX_Act_Player_Bounty_Before_Hand )  // PYR-45395
					{
						int seat = hand.history[i].position;
						if( seat >= 0 && seat < MAX_TABLE_PLAYERS )
						{
							playerHeadBountyBeforeHand[seat] = hand.history[i].amount;
						}
					}
					// set the userAlias values so that we can map an alias to a player's userName (this is only fired on the server)
					else if( hand.history[i].action == TH_EX_Act_Player_User_Name_Alias ) // PYR-106133
					{
						int seat = hand.history[i].position;
						if( seat >= 0 && seat < MAX_TABLE_PLAYERS )
						{
							userAlias[seat] = hand.history[i].contents;
						}
					}

					if( hand.history[i].action != TH_EX_Act_Tourn_Deal_Attempt_Completed )
					{
						continue;
					}
				}
				else if ( hand.history[ i ].eventCode == TABLE_HISTORY_ALLIN ) 
				{
					if( hand.history[i].action != '!'
						&& hand.history[i].action != '#'
						&& hand.history[i].action != '&'
						&& hand.history[i].action != '%'
						)
					{
						continue; // on the first pass we are interested only in addons/re-buys
					}
				}
				else // all other event codes
				{
					continue;
				}
			}

			// only process events before hand start on pass==0
			if(step==0 && hand.history[i].ordinal >= hand.ordinalOfHandStart)
			{
				continue;
			}

			// only process events from hand start onwards on pass==1
			if(step==1 && hand.history[i].ordinal < hand.ordinalOfHandStart)
			{
				continue;
			}

			str_action = "";
			str_dealer = "";
			str_stakes [ 0 ] = '\x0';//str_stakes = "";
			str_number [ 0 ] = '\x0';
			eventHistoryStr.cut( 0 );
			PString str_userName = getUserName( hand.history[i].userId, userAlias, hand.history[i].position, myUserAlias, admin, showStealthTableAlias, myRealUserId );

			// Bug #992: Resolving here:
			switch( hand.history[ i ].eventCode )
			{
			case TABLE_HISTORY_ACTION:
				{
					if (hand.game == HandTypeUnfoldClassic && round == UnfoldRound)
					{
						if(hand.history[i].action != 'U' && hand.history[i].action != 'G')  // only Unfold and Decline action could be in UnfoldRound, all others should be in Flop round
						{
							round = SecondRound;  // PYR-65734 actually in flop round. TODO: we need a better way to indicate the Flop round starts in Unfold game!
						}
					}

					if(isAnteUpPowerStarted && hand.history[ i ].position == anteUpPlayer)
					{
						isAnteUpPowerStarted = false;
					}

					// The assert below was commented out because some powers use fake actions that have byte values above 127.
					// In this case, the history item will be skipped.
					//PASSERT( hand.history[ i ].action >= 0 );
					if( hand.history[ i ].action < 0 )
						break;

					if( hand.history[ i ].action == 'y' || hand.history[ i ].action == 'Y' || hand.history[ i ].action == 'X' )
						break; // no HandHistory for this type of action now!

					// for unfold classic game all action related text is composed separately; note: PMsg will assert if used on empty msgid this is done to avoid it
					if( !actionName( hand.history[ i ].action ).equals( P_TXT( TXTHHY_EMPTY_STUB ) ) )
					{
						if( hand.isButtonBlindHand() && hand.history[ i ].action == 'P' )
						{
							PMsg( str_action, LocalesTable, locale, P_TXT( TXTHHY_posts_button_blind ) );  // PYR-74693
						}
						else
						{
							PMsg( str_action, LocalesTable, locale, actionName( hand.history[ i ].action ) );
						}
					}
					else if( !strchr( "LTUGHhox", hand.history[ i ].action ) )  // some action names from NStack tournament and HandTypeUnfoldClassic are empty. They are used in full sentence messages later!
					{
						PLog( LOG_TRACE_FAULTY_LOGIC ": empty action string for non-unfold game action: '%d'", hand.history[ i ].action );
						PASSERT5( 0 );  // stopped in debug
					}

					if( hand.history[ i ].action == 'L' )  // PYR-25226
					{
						sprintf( str_number, "%d", (INT32)hand.history[ i ].amount );

						PString totalChipsAdded;
						PString oneStackChips;
						PString stacksRemaining;

						const char *p = hand.history[ i ].contents.find(',');
						if(p)
						{
							totalChipsAdded.assign(hand.history[ i ].contents.c_str(), p);
							const char *p2 = strchr(++p, ',');
							if(p2)
							{
								oneStackChips.assign(p, p2);
								stacksRemaining.assign(p2+1);
							}
							else
							{
								oneStackChips.assign(p);
							}
						}
						else
						{
							PASSERT5(false);
							totalChipsAdded = hand.history[ i ].contents;
						}

						PMsg5( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_adds_N1_chips_XN2_stackXsX__ELL ), str_userName, totalChipsAdded, str_number, oneStackChips, stacksRemaining );
						appendOneLineHandHistoryEvent( admin, hand.history[ i ].when, historyStr, eventHistoryStr );
						break;
					}
					else if( hand.history[ i ].action == 'D' )
					{
						sprintf( str_number, "%d", (INT32)hand.history[ i ].amount );
						str_dealer.assign( str_number );

						if( hand.history[ i ].amount == 1 )
							PMsg( str_dealer, LocalesTable, locale, P_TXT( TXTHHY_X040card ) );
						else
							PMsg( str_dealer, LocalesTable, locale, P_TXT( TXTHHY_X040cards ) );

						if( !closedCards && ( hand.history[ i ].userId.equals( myUserAlias ) || admin ) )
						{ // add discarded card_str here:
							if( *hand.history[ i ].contents && ( *hand.history[ i ].contents != '?' ) )
							{
								str_p = hand.history[ i ].contents;
								//Bug #4754:
								//int position = hand.history[ i ].position;
								//PString temp = shownCardsPstr[ position ];
								//int temp_amt = hand.history[ i ].amount;
								//subtractCardsPstr();
								subtractCardsPstr( shownCardsPstr[ hand.history[ i ].position ], str_p, hand.history[ i ].amount, locale );
							}

							i18nAppend( str_dealer, " [");
							while( *str_p )
							{
								i18nAppendChar( str_dealer, *str_p );
								if( *( ++str_p ) )
									i18nAppendChar( str_dealer, *str_p );

								if( *( ++str_p ) )
									PMsg( str_dealer, LocalesTable, locale, P_TXT( TXTHHY_X040 ) );
							}
							i18nAppendChar( str_dealer, ']' );
						}
					}
					else if( hand.history[ i ].action == 'd' )
					{ //Bug #4754:
						if( !closedCards && ( hand.history[ i ].userId.equals( myUserAlias ) || admin ) )
							if( *shownCardsPstr[ hand.history[ i ].position ] && ( *shownCardsPstr[ hand.history[ i ].position ] != '?' ) )
							{
								str_p = shownCardsPstr[ hand.history[ i ].position ];
								PMsg( str_dealer, LocalesTable, locale, P_TXT( TXTHHY_on ) );
								i18nAppend( str_dealer, " [" );
								while( *str_p )
								{
									i18nAppendChar( str_dealer, *str_p );
									if( *( ++str_p ) )
										i18nAppendChar( str_dealer, *str_p );

									if( *( ++str_p ) )
										PMsg( str_dealer, LocalesTable, locale, P_TXT( TXTHHY_X040 ) );
								}
								i18nAppendChar( str_dealer, ']' );
							}
					}
					else if( hand.history[i].action == 'x' ) // swap action
					{
						sprintf( str_number, "%d", (INT32)hand.history[ i ].amount );

						if( !closedCards && ( hand.history[ i ].userId.equals( myUserAlias ) || admin ) )
						{ // add discarded card_str here:
							if( *hand.history[ i ].contents && ( *hand.history[ i ].contents != '?' ) )
							{
								str_p = hand.history[ i ].contents;
								subtractCardsPstr( shownCardsPstr[ hand.history[ i ].position ], str_p, hand.history[ i ].amount, locale );
							}

							i18nAppend( str_dealer, " [");
							while( *str_p )
							{
								i18nAppendChar( str_dealer, *str_p );
								if( *( ++str_p ) )
									i18nAppendChar( str_dealer, *str_p );

								if( *( ++str_p ) )
									PMsg( str_dealer, LocalesTable, locale, P_TXT( TXTHHY_X040 ) );
							}
							i18nAppendChar( str_dealer, ']' );
						}

						if( hand.history[i].amount == 1 )
						{
							PMsg2( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0X_swaps_1_cardN1 ), str_userName, str_dealer );
						}
						else
						{
							PMsg3( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0X_swaps_N1_cardsN2 ), str_userName, str_number, str_dealer );
						}

						appendOneLineHandHistoryEvent( admin, hand.history[ i ].when, historyStr, eventHistoryStr );
						break;
					}
					else if( hand.history[ i ].action == 'Z' || hand.history[ i ].action == 'f' )
					{
						// #15261
						bool cardsKnown = false; // ( hand.history[ i ].userId.equals( myUserAlias ) );  // PYR-20538
						if( !cardsKnown && admin && hand.history[ i ].action == 'f' ) //-V560
							cardsKnown = true;

						if( cardsKnown )
							str_p = hand.users[ userSeats[ hand.history[ i ].position ] ].cards;
						else
							str_p = hand.history[ i ].contents;

						i18nAppendChar( str_dealer, '[' );
						while( *str_p )
						{
							i18nAppendChar( str_dealer, *str_p );
							if( *( ++str_p ) )
								i18nAppendChar( str_dealer, *str_p );

							if( *( ++str_p ) )
								PMsg( str_dealer, LocalesTable, locale, P_TXT( TXTHHY_X040 ) );
						}
						i18nAppendChar( str_dealer, ']' );
						allCardsPstr[ userSeats[ hand.history[ i ].position ] ] = str_dealer;

						if( hand.history[ i ].action == 'f' )
						{
							roundFold[ userSeats[ hand.history[ i ].position ] ] = round;
							if( admin )
								foldAndShowCards[ userSeats[ hand.history[ i ].position ] ] = hand.history[ i ].contents;
						}
					}
					else if( hand.history[ i ].action == 'z' )
					{
						// #15261
						str_p = hand.history[ i ].contents;	// holds the cards to show
						int nbCards =  hand.history[ i ].contents.length() / 2;
						bool cardsKnown = false;  // ( hand.history[ i ].userId.equals( myUserAlias ) );  // PYR-20538

						int cardMask = hand.history[ i ].amount;

						i18nAppendChar( str_dealer, '[' );
						bool requiresSpace = false;
						for( int j = 0; j < nbCards; ++j )
						{
							if( !cardsKnown && !( cardMask & ( 1 << j ) ) ) //-V560
							{
								str_p += 2;
								continue;
							}	

							if( requiresSpace )
								PMsg( str_dealer, LocalesTable, locale, P_TXT( TXTHHY_X040 ) );

							i18nAppendChar( str_dealer, *str_p );
							if( *( ++str_p ) )
								i18nAppendChar( str_dealer, *str_p );

							if( *( ++str_p ) )
								requiresSpace = true;
						}	
						i18nAppendChar( str_dealer, ']' );
						allCardsPstr[ userSeats[ hand.history[ i ].position ] ] = str_dealer;
					}
					else if( hand.history[ i ].amount ) 
					{
						int actionCost;

						if( hand.history[ i ].action == 'E' || hand.history[ i ].action == 'e' )
						{
							actionCost = hand.history[ i ].amount - playerChipsBet[ userSeats[ hand.history[ i ].position ] ];
							PString str_raise;
							// calculate net here:
							int net = actionCost - maxBet + playerChipsBet[ userSeats[ hand.history[ i ].position ] ];
							_formatScaledChips( net, effectiveScalePM, Denominator, str_stakes );

							i18nAppendChar( str_raise, ' ' );
							i18nAppend( str_raise, _formatChips( str_chips_buf, locale, hand, currSign, str_stakes ) );

							//DEALERCHAT_RAISES_TO  %0 to
							PMsg1( str_action, LocalesTable, locale, P_TXT( TXTHHY_N0_to ), str_raise );
						}
						else
						{
							actionCost = hand.history[i].amount;
						}

						playerChipsBet[ userSeats[ hand.history[ i ].position ] ] += actionCost;

						if( hand.history[ i ].action == 'q' || ( hand.history[ i ].action == 'p' && !( hand.users[ userSeats[ hand.history[ i ].position ] ].result & SB_posted ) ) )
						{ //Bug #2393:
							actionCost = max( playerChipsBet[ userSeats[ hand.history[ i ].position ] ] - int( hand.smallBlindSize ), 0x0 );
							playerChipsBet[ userSeats[ hand.history[ i ].position ] ] = actionCost;
						}
						else
						{
							actionCost = playerChipsBet[userSeats[hand.history[i].position]];
						}

						if (maxBet < actionCost && hand.history[i].action != 'U')  // unfold bet amount not counted
						{
							maxBet = actionCost;
						}
						/*
						if(	action == 'q' || ( action == 'p' && tableData.smallBlindPayer > -1  &&  num != tableData.smallBlindPayer ) )
						{ // move SB to Main pot!	
						int pot_amount = ( result < tableData.sBlind() ) ? result : tableData.sBlind();
						tableData.moveToPot( num, pot_amount, 0x0, update );
						tableData.statisticalPot += pot_amount;
						result = tableData.player( num ).chipsBet;
						}
						*/
						_formatScaledChips( hand.history[ i ].amount, effectiveScalePM, Denominator, str_stakes );

						i18nAppend( str_dealer, _formatChips( str_chips_buf, locale, hand, currSign, str_stakes ) );
						//Bug #2410:
						if( hand.history[ i ].contents.equals( " and is all-in" ) || hand.history[ i ].contents.equals( "@" ) )
						{
							PMsg( str_dealer, LocalesTable, locale, P_TXT( TXTHHY_X040and_is_all_in ) );
							if( freezeBoardCardsWhenAllIn )  // PYR-39214
							{
								i18nAppend( str_dealer, " (" );
								PMsg( str_dealer, LocalesTable, locale, P_TXT( TXTHHY_and_freezes_the_current_board__ELL ) );
								i18nAppend( str_dealer, ")" );
							}
						}
						else if( hand.history[ i ].contents.equals( "!" ) )
						{
							_formatScaledChips( tableCap, effectiveScalePM, Denominator, str_stakes );
							PMsg1( str_dealer, LocalesTable, locale, P_TXT( TXTHHY_X040and_has_reached_the_N0_cap ), _formatChips( str_chips_buf, locale, hand, currSign, str_stakes ) );
						}
						else if( !( hand.chipsType == POKER_DUEL_CHIPS && hand.history[ i ].contents.startsWith("{") && hand.history[ i ].contents.endsWith("}") ) )  // PYR-36544, in Duel, a json string stored here
						{
							i18nAppend( str_dealer, hand.history[ i ].contents );
						}
					}
					else if( hand.history[ i ].action == 'W' || hand.history[ i ].action == 's' || hand.history[ i ].action == 'S' )
					{
						PString str_badugi;
						if( round != Showdown && hand.history[ i ].action != 'W' && !isAnteUpPowerStarted )
						{ 
							round = Showdown;  // show down round
							if( !runItTwice )
							{
								PMsg( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_XX_SHOW_DOWN_XX ) ); 
								appendOneLineHandHistoryEvent( admin, hand.history[ i ].when, historyStr, eventHistoryStr );
							}
						}

						// Player cards:
						str_p = hand.users[ userSeats[ hand.history[ i ].position ] ].cards; 

						i18nAppendChar( str_dealer, '[' );
						while( *str_p )
						{
							i18nAppendChar( str_dealer, *str_p );
							if( *( ++str_p ) )
								i18nAppendChar( str_dealer, *str_p );

							if( *( ++str_p ) )
								PMsg( str_dealer, LocalesTable, locale, P_TXT( TXTHHY_X040 ) );
							//eventHistoryStr.append( "," );
						}
						i18nAppendChar( str_dealer, ']' );
						allCardsPstr[ userSeats[ hand.history[ i ].position ] ] = str_dealer;


						Card bestUserHigh[ 5 ];
						for( int j = 0; j < 5; ++j )
							bestUserHigh[ j ].suit = bestUserHigh[ j ].rank = 0x0;

						PString str_1;

						strToCards(
							boardCounter ? hand.users2[ userSeats[ hand.history[ i ].position ] ].combHi2 : hand.users[ userSeats[ hand.history[ i ].position ] ].combHi,
							bestUserHigh );

						if( hand.isHiLo )
							PMsg( str_1, LocalesTable, locale, P_TXT( TXTHHY_HIXX040 ) );

						if( hand.game == HandType3Draw27 || hand.game == HandType3DrawA5 || hand.game == HandTypeSingleDraw27 )
							_bestTripleDrawSwtch( locale, hand.users[ userSeats[ hand.history[ i ].position ] ].bestHand, bestUserHigh, str_1 );
						else if( hand.game == HandTypeRazz )
							_bestRazzSwtch( locale, hand.users[ userSeats[ hand.history[ i ].position ] ].bestHand, bestUserHigh, str_1 );
						else if( hand.game == HandTypeBadugi )
						{	
							_bestBadugiSwtch( locale, hand.users[ userSeats[ hand.history[ i ].position ] ].bestHand, bestUserHigh, true, str_1 );
							_bestBadugiSwtch( locale, hand.users[ userSeats[ hand.history[ i ].position ] ].bestHand, bestUserHigh, false, str_badugi );
						}
						else
						{
							if( boardCounter )
								_bestHandSwtch2( locale, hand.users2[ userSeats[ hand.history[ i ].position ] ].bestHand2, bestUserHigh, str_1 );
							else
								_bestHandSwtch( locale,	hand.users[ userSeats[ hand.history[ i ].position ] ].bestHand, bestUserHigh, str_1 );
						}

						boardCounter ?
							bestCombPstr2[ userSeats[ hand.history[ i ].position ] ].assign( str_1 ) : 
							bestCombPstr[ userSeats[ hand.history[ i ].position ] ].assign( str_1 );

						if( _bestHigh ) // if there is any winner( best hand holder )
						{
							Card kicker, wKicker;
							ExtraKickers extraKickers;
							PString str_2, kickerRank;

							int cmp;
							if( hand.game == HandType3Draw27 || hand.game == HandTypeSingleDraw27 )
								cmp = _compareBestTriple27Hands( kicker, wKicker,
								hand.users[ userSeats[ hand.history[ i ].position ] ].bestHand, 
								bestUserHigh, _bestHigh, _bestHighHand, extraKickers );
							else if( hand.game == HandTypeRazz || hand.game == HandType3DrawA5 )
								cmp = _compareBestRazzHands( kicker, wKicker,
								hand.users[ userSeats[ hand.history[ i ].position ] ].bestHand, 
								bestUserHigh, _bestHigh, _bestHighHand, extraKickers );
							else if( hand.game == HandTypeBadugi )
								cmp = _compareBestBadugiHands( kicker, wKicker,
								hand.users[ userSeats[ hand.history[ i ].position ] ].bestHand,
								bestUserHigh, _bestHigh, _bestHighHand );
							else
								cmp = _compareBestHighHands( kicker, wKicker, 
								boardCounter ? hand.users2[ userSeats[ hand.history[ i ].position ] ].bestHand2 : hand.users[ userSeats[ hand.history[ i ].position ] ].bestHand, 
								bestUserHigh, _bestHigh, _bestHighHand, extraKickers );

							if( cmp > 0 )
							{
								_bestHigh = boardCounter ? hand.users2[ userSeats[ hand.history[ i ].position ] ].bestHand2 : hand.users[ userSeats[ hand.history[ i ].position ] ].bestHand;

								for( int j = 0; j < 5; ++j )
								{ //_bestHighHand = bestUserHigh
									_bestHighHand[ j ].rank = bestUserHigh[ j ].rank;
									_bestHighHand[ j ].suit = bestUserHigh[ j ].suit;
								}

								if( kicker.suit != 0 )
								{ // Bug #1462:
									if( _bestHigh != 6 ) // Not Flash
										for( int j = 0; j < MAX_EXTRA_KICKERS; ++j )
										{ //...has High card Ace, Queen+Ten+Seven kicker
											if( extraKickers.rank[ j ] ) 
											{
												_rankName( kickerRank, locale, extraKickers.rank[ j ] );
												i18nAppendChar( kickerRank, '+' );
											}
											else
												break; // for(j)
										}
										_rankName( kickerRank, locale, kicker.rank );
										PMsg( str_2, LocalesTable, locale, P_TXT( TXTHHY_X040higher ) );
								}
							}

							else if( cmp < 0 )
							{
								if( wKicker.suit != 0 )
								{
									PMsg( kickerRank, LocalesTable, locale, P_TXT( TXTHHY_lower ) );
									// DEALERCHAT_ACTIONTIMEOUT_SHOW  cards shown
									PMsg( str_2, LocalesTable, locale, P_TXT( TXTHHY_X040cards ) );
								}
							}

							if( *kickerRank )
							{
								PMsg( str_1, LocalesTable, locale, P_TXT( TXTHHY_X040_X040 ) );
								i18nAppend( str_1, kickerRank );
								if( _bestHigh == 6 )// Flash
									i18nAppend( str_1, str_2 );
								else
									PMsg( str_1, LocalesTable, locale, P_TXT( TXTHHY_X040kicker ) );
							}
						}
						else
						{
							_bestHigh = boardCounter ? hand.users2[ userSeats[ hand.history[ i ].position ] ].bestHand2 : hand.users[ userSeats[ hand.history[ i ].position ] ].bestHand;
							strToCards( boardCounter ? hand.users2[ userSeats[ hand.history[ i ].position ] ].combHi2 : hand.users[ userSeats[ hand.history[ i ].position ] ].combHi, _bestHighHand );
						}

						if( boardCounter == 0 )
						{
							if( *( hand.users[ userSeats[ hand.history[ i ].position ] ].combLo ) )
							{// if qualified as Low
								PMsg( str_1, LocalesTable, locale, P_TXT( TXTHHY_X_LOXX040 ) );
								PMsg( bestCombPstr[ userSeats[ hand.history[ i ].position ] ], LocalesTable, locale, P_TXT( TXTHHY_X_LOXX040 ) );
								strToCards( hand.users[ userSeats[ hand.history[ i ].position ] ].combLo, bestUserHigh );
								_bestLoComb( locale, bestUserHigh, str_1 );
								_bestLoComb( locale, bestUserHigh, bestCombPstr[ userSeats[ hand.history[ i ].position ] ] );
							}
						}
						else
						{
							if( *( hand.users2[ userSeats[ hand.history[ i ].position ] ].combLo2 ) )
							{// if qualified as Low
								PMsg( str_1, LocalesTable, locale, P_TXT( TXTHHY_X_LOXX040 ) );
								PMsg( bestCombPstr2[ userSeats[ hand.history[ i ].position ] ], LocalesTable, locale, P_TXT( TXTHHY_X_LOXX040 ) );
								strToCards( hand.users2[ userSeats[ hand.history[ i ].position ] ].combLo2, bestUserHigh );
								_bestLoComb( locale, bestUserHigh, str_1 );
								_bestLoComb( locale, bestUserHigh, bestCombPstr2[ userSeats[ hand.history[ i ].position ] ] );
							}		
						}

						// append best hand (HI, LO both )  // Call strToCards() the second time!! Is not good!!! TBD
						//fillBestCombPstr( hand, bestCombPstr[ userSeats[ hand.history[ i ].position ] ], userSeats[ hand.history[ i ].position ] );
						i18nAppend( str_dealer, " (" );
						i18nAppend( str_dealer, ( hand.game == HandTypeBadugi ) ? str_badugi : str_1 );
						i18nAppendChar( str_dealer, ')' );
					}
					else if (hand.history[i].action == 'F')
					{
						roundFold[userSeats[hand.history[i].position]] = round;
					}

					if(isAnteUpPowerStarted)
					{
						PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "Ante Up - " );
					}

					// below actions processed separately to use correctly internationalized string
					if (hand.game == HandTypeUnfoldClassic)
					{
						if (hand.history[i].action == 'T') // post unfold antes
						{
							PMsg2(eventHistoryStr, LocalesTable, locale, P_TXT(TXTHHY_N0X_posts_the_Unfold_ante_N1), str_userName, str_dealer);
							break;
						}
						else if (hand.history[i].action == 'U') // unfolds
						{
							PMsg2(eventHistoryStr, LocalesTable, locale, P_TXT(TXTHHY_N0X_unfolds_N1), str_userName, str_dealer);
							break;
						}
						else if (hand.history[i].action == 'G') // discards unfold
						{
							PMsg1(eventHistoryStr, LocalesTable, locale, P_TXT(TXTHHY_N0X_passes_on_unfolding), str_userName );
							break;
						}
						else if (hand.history[i].action == 'H') // unfold showdown
						{
							if (round != UnfoldShowDownRound)
							{
								PMsg(eventHistoryStr, LocalesTable, locale, P_TXT(TXTHHY_XX_UNFOLD_SHOWDOWN_XX));
								appendOneLineHandHistoryEvent(admin, hand.history[i].when, historyStr, eventHistoryStr);
								round = UnfoldShowDownRound;
							}
							// code below is intentionaly copy pasted from above and modified for Holdem and single board; 
							// changing the code above seems risky at the moment
							PString handCardsStr;
							str_p = hand.history[i].contents;
							while (*str_p)
							{
								i18nAppendChar(handCardsStr, *str_p);
								if (*(++str_p))
									i18nAppendChar(handCardsStr, *str_p);

								if (*(++str_p))
									PMsg(handCardsStr, LocalesTable, locale, P_TXT(TXTHHY_X040));
							}

							PString handBestStr;
							Card bestUserHigh[5];
							for (int j = 0; j < 5; ++j)
								bestUserHigh[j].suit = bestUserHigh[j].rank = 0x0;
							strToCards(hand.users[userSeats[hand.history[i].position]].combHi, bestUserHigh);
							_bestHandSwtch(locale, hand.users[userSeats[hand.history[i].position]].bestHand, bestUserHigh, handBestStr);
							PMsg3(eventHistoryStr, LocalesTable, locale, P_TXT(TXTHHY_N0X_unfolded_XN1X_XN2X), str_userName, handCardsStr, handBestStr);
							break;
						}
					}
					
					// PYR-79935 
					if ( hand.history[i].action == 'h' ) // agreed to All In cashout --- cashed out the hand for AMOUNT
					{
						userCashoutIndexInEvents[userSeats[hand.history[i].position]] = i;
						break;
					}
					if ( hand.history[i].action == 'o' ) // didn't take cashout, no need to log anything here
					{
						break;
					}

					//HISTORY_PLAYER_ACTION %0: %1 %2
					PMsg3( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0X_N1_N2 ), str_userName, str_action, str_dealer );
				}
				break;

			case TABLE_HISTORY_UNFOLD_ANTERETURNED:
			{
				_formatScaledChips(hand.history[i].amount, effectiveScalePM, Denominator, str_stakes);
				i18nAppend(str_dealer, _formatChips(str_chips_buf, locale, hand, currSign, str_stakes));
				i18nAppend(str_dealer, hand.history[i].contents);
				PMsg2(eventHistoryStr, LocalesTable, locale, P_TXT(TXTHHY_N0X_Unfold_ante_N1_returned), str_userName, str_dealer);
			}
			break;

			case TABLE_HISTORY_DEALING:
				{
					// Position = seat, = -1 if Board, Action = HandRound, Amount = Cards_number
					if( round != hand.history[ i ].action || hand.history[ i ].position < 0 )
					{
						round = hand.history[ i ].action;

						if( hand.game != HandTypeStud && hand.game != HandTypeRazz )  
						{
							if( hand.game == HandTypeDraw || hand.game == HandTypeBadugi || hand.game == HandType3Draw27 || hand.game == HandType3DrawA5  || hand.game == HandTypeSingleDraw27 )
							{ // Draw:
								if( !round )
								{
									PMsg( str_action, LocalesTable, locale, P_TXT( TXTHHY_DEALING_HANDS ) );
									PMsg1( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_XX_N0_XX ), str_action );
								}
							}
							else //Holdem/Omaha:
							{
								if( runItTwice && hand.history[ i ].position < 0 )   //Run It Twice is running
								{
									PASSERT(hand.history[ i ].position == -2 || hand.history[ i ].position == -3);
									PMsg( str_action, LocalesTable, locale, boardName( -2 - hand.history[ i ].position ) );
									PMsg( str_action, LocalesTable, locale, P_TXT( TXTHHY_X040 ) );
								}

								// Check if this dealing is regular or caused by a power.
								if(powerIdAffectingBoard == PokerGamePowerRunItTwice && round == 0)  // only by power
								{
									// If Run it twice is played pre-flop and actually duplicates cards, we use the word "BOARD" (which will write "SECOND BOARD").
									PASSERT5(hand.history[ i ].position < 0);
									PMsg( str_action, LocalesTable, locale, P_TXT( TXTHHY_BOARD ) );
								}
								else if(powerIdAffectingBoard == PokerGamePowerRunItTwice || powerIdAffectingBoard == 0)
								{
									if (hand.game == HandTypeUnfoldClassic && round == UnfoldRound)
									{
										round = SecondRound;  // PYR-65734 actually in flop round. TODO: we need a better way to indicate the Flop round starts in Unfold game!
									}
									// If this is regular dealing, or RIT on the flop or after, we use the proper round name.
									if( !roundName( round, true ).equals( P_TXT( TXTHHY_EMPTY_STUB ) ) )
									{
										PMsg( str_action, LocalesTable, locale, roundName( round, true ) );
									}
								}
								else
								{
									// If the current dealing was caused by a power (not RunItTwice power), use its name instead.
									str_action.append(_powerName(powerIdAffectingBoard, locale, powerNameStr));
									str_action.toUpper();
								}
								// Reset the power affecting board.
								powerIdAffectingBoard = 0;

								PMsg1( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_XX_N0_XX ), str_action );
							}

							if( hand.history[ i ].position < 0 ) // Board dealing
							{
								if(runItTwice)
								{
									PASSERT(hand.history[ i ].position == -2 || hand.history[ i ].position == -3);
									str_p = boardCardsPstr[-2 - hand.history[ i ].position];
								}
								else
								{
									str_p = boardCardsPstr[0];
								}

								if( strlen(str_p) ) //if the board already contains cards
								{ // filling the previous dealt cards
									i18nAppend( eventHistoryStr, " [" );
									while( *str_p )
									{
										i18nAppendChar( eventHistoryStr, *str_p );

										if( *( ++str_p ) )
											i18nAppendChar( eventHistoryStr, *(str_p++) );
										else break;

										if( *str_p )
											PMsg( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_X040 ) );
									}
									i18nAppendChar( eventHistoryStr, ']' ); 
								}

								i18nAppend( eventHistoryStr, " [" );
								//if( hand.game == HandTypeCourchevel && round == 1 )  // comment out per PYR-51433
								//{//if FLOP in Courchevel, show PRE-FLOP first:
								//	if(runItTwice)
								//	{
								//		PASSERT(hand.history[ i ].position == -2 || hand.history[ i ].position == -3);
								//		str_p = boardCardsPstr[-2 - hand.history[ i ].position];
								//	}
								//	else
								//		str_p = boardCardsPstr[0];
								//	if( *str_p )
								//		i18nAppendChar( eventHistoryStr, *str_p );
								//	if( *( ++str_p ) )
								//		i18nAppendChar( eventHistoryStr, *str_p );
								//	if( *( ++str_p ) )
								//		PMsg( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_X040 ) );
								//}
								str_p = hand.history[ i ].contents;

								// Add the new cards to the current board state.
								if(runItTwice)
								{
									PASSERT(hand.history[ i ].position == -2 || hand.history[ i ].position == -3);
									boardCardsPstr[-2 - hand.history[ i ].position].append(hand.history[ i ].contents);
								}
								else
									boardCardsPstr[0].append(hand.history[ i ].contents);

								while( *str_p )
								{
									i18nAppendChar( eventHistoryStr, *str_p );
									if( *( ++str_p ) )
										i18nAppendChar( eventHistoryStr, *str_p );

									if( *( ++str_p ) )
										PMsg( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_X040 ) );
								}
								i18nAppendChar( eventHistoryStr, ']' );
							}
						}
						else // Stud:
						{
							if( round != 4 )
							{ 
								sprintf( str_number, "%d", round + 3 );
								str_action = str_number;
								if( round )
									PMsg( str_action, LocalesTable, locale, P_TXT( TXTHHY_th ) ); 
								else 			
									PMsg( str_action, LocalesTable, locale, P_TXT( TXTHHY_rd ) ); 

								PMsg( str_action, LocalesTable, locale, P_TXT( TXTHHY_X040STREET ) ); 

								// do nothig TBD later! ( Looks like ***  *** which is not good)
								//PMsg( str_action, msgTable, P_TXT( TXT_HISTORY_3STREETSTUD ) ); 
							}
							else
								PMsg( str_action, LocalesTable, locale, P_TXT( TXTHHY_RIVER ) ); 

							PMsg1( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_XX_N0_XX ), str_action );

							if( hand.history[ i ].position < 0 ) // Board dealing
							{
								i18nAppend( eventHistoryStr, " [" );
								i18nAppend( eventHistoryStr, hand.history[ i ].contents );
								i18nAppendChar( eventHistoryStr, ']' );
							}
						}
					}

					if( hand.history[ i ].position >= 0 )    // Player Dealing
					{ //HISTORY_PLAYER_DEALT Dealt to %0 %1[%2]
						str_action = "";

						if( *shownCardsPstr[ hand.history[ i ].position ] )
						{// add already shown cards:
							str_p = shownCardsPstr[ hand.history[ i ].position ];
							i18nAppendChar( str_dealer, '[');
							while( *str_p )
							{
								i18nAppendChar( str_dealer, *str_p );
								if( *( ++str_p ) )
									i18nAppendChar( str_dealer, *str_p );

								if( *( ++str_p ) )
									PMsg( str_dealer, LocalesTable, locale, P_TXT( TXTHHY_X040 ) );
							}
							i18nAppend(str_dealer, "] "); 
						}

						if( !closedCards && ( hand.history[ i ].userId.equals( myUserAlias ) || admin ) )
						{
							PString temp;
							if( *hand.history[ i ].contents && ( *hand.history[ i ].contents != '?' ) )
								str_p = hand.history[ i ].contents;
							else
							{ // needed for Client HandHistory and just for any other cases:
								if( hand.game == HandTypeStud || hand.game == HandTypeRazz )
								{
									if( round )
										temp.assign( hand.users[ userSeats[ hand.history[ i ].position ] ].cards.c_str() + 4 + 2 * round, 2 );
									else
										temp.assign( hand.users[ userSeats[ hand.history[ i ].position ] ].cards, 6 );

									str_p = temp;
								}
								else if( hand.game == HandTypeFusion )
								{
									if( round )
									{
										int cardsOffset = 2 + round - 1;	// players start with two cards, and get one new card per round
										cardsOffset *= 2;					// each card is two characters
										int cardsDealt = hand.history[ i ].amount;	// in a variant of fusion with DealTwoOnFlop option set, two cards could be dealt on the flop
										PASSERT( cardsDealt == 1 || cardsDealt == 2 );
										PASSERT( hand.users[ userSeats[ hand.history[ i ].position ] ].cards.length() >= cardsOffset + (2*cardsDealt) ); // ensure there is another card at the end
										temp.assign( hand.users[ userSeats[ hand.history[ i ].position ] ].cards.c_str() + cardsOffset, 2*cardsDealt );
									}
									else
									{
										temp.assign( hand.users[ userSeats[ hand.history[ i ].position ] ].cards, 4 ); // first two cards take four characters
									}
									str_p = temp;
								}
								else
								{
									str_p = hand.users[ userSeats[ hand.history[ i ].position ] ].cards; 
								}
							}

							if( hand.game != HandTypeDraw && hand.game != HandTypeBadugi && hand.game != HandType3Draw27 && hand.game != HandType3DrawA5 && hand.game != HandTypeSingleDraw27 )
								i18nAppend( shownCardsPstr[ hand.history[ i ].position ], str_p );
							else //Bug #4754: add dealed cards:
								i18nAppend( shownCardsPstr[ hand.history[ i ].position ], hand.history[ i ].contents );

							while( *str_p )
							{
								i18nAppendChar( str_action, *str_p );
								if( *( ++str_p ) )
									i18nAppendChar( str_action, *str_p );

								if( *( ++str_p ) )
									PMsg( str_action, LocalesTable, locale, P_TXT( TXTHHY_X040 ) );
							}
						}
						else if( ( hand.game == HandTypeStud || hand.game == HandTypeRazz ) && round < 4 )
						{
							if( round > 0 )
								str_action = hand.history[ i ].contents;
							else // if 3-rd Street(two are hidden, one is public)
								str_action.assign( hand.history[ i ].contents.c_str() + 4 );

							i18nAppend( shownCardsPstr[ hand.history[ i ].position ], str_action );
						}

						if( *str_action )
						{
							appendOneLineHandHistoryEvent( admin, hand.history[ i ].when, historyStr, eventHistoryStr );
							PMsg3( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_Dealt_to_N0_N1XN2X ), str_userName, str_dealer, str_action );
						}
					}
					else if( hand.game == HandTypeCourchevel && !hand.history[ i ].action )
					{ //PYR-24197: special case of Courchevel board on zero round:
						str_action.clear();       // PYR-51433
						eventHistoryStr.clear();  // PYR-51433
						PMsg( str_action, LocalesTable, locale, P_TXT( TXTHHY_PRE_FLOP ) );
						PMsg1( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_XX_N0_XX ), str_action );
						i18nAppend( eventHistoryStr, " [" );
						i18nAppend( eventHistoryStr, hand.history[ i ].contents );
						i18nAppendChar( eventHistoryStr, ']' );
					}

				}
				break;

			case TABLE_HISTORY_TIMEOUT:
				{
					if(isAnteUpPowerStarted && hand.history[ i ].position != anteUpPlayer)
					{
						PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "Ante Up - " );
					}

					PMsg1( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_has_timed_out ), str_userName );
					if( hand.history[ i ].contents.equals( "!") )
						PMsg( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_X040while_being_disconnected ) );
					else if( hand.history[ i ].contents.equals( "") )
						PMsg( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_X040while_disconnected ) );
				}
				break;

			case TABLE_HISTORY_MOVETOPOT: // temporary frozen, TBD reproduced side pots from History (by supervisor)
				// Action /or Seat/ = Pot_number TBD later
				{
					int j;
					if( hand.history[ i ].position == -1 )
					{ //Bug #4070: special case of moving an ante only 
						for( j = 0; j < MAX_TABLE_PLAYERS; ++j )
							if( playerChipsBet[ j ] >= hand.ante ) 
								playerChipsBet[ j ] -= hand.ante;

						if( maxBet >= hand.ante ) 
							maxBet -= hand.ante; // for RaiseTo calculation:

						if( admin )
							i18nAppend( eventHistoryStr, "Moving Antes to Pot" );
					}
					else
					{
						maxBet = 0; // for RaiseTo calculation:
						for( j = 0; j < MAX_TABLE_PLAYERS; ++j )
							playerChipsBet[ j ] = 0;

						if( admin )
							i18nAppend( eventHistoryStr, "Moving Bets to Pot" );
					}	
					/*
					pot[ Action ] += hand.history[ i ].amount; //?? how to implement

					int rest = hand.history[ i ].amount%Denominator;
					if( rest )
					sprintf( str_stakes, " %u.%02u", hand.history[ i ].amount/Denominator, rest );   
					else
					sprintf( str_stakes, " %u", hand.history[ i ].amount/Denominator );   

					PMsg1( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_Total_pot_N0 ), str_stakes );
					*/
					//round = hand.history[ i ].action;
				}
				break;

			case TABLE_HISTORY_HOUSE_TAKEFROMPOT: //lint -fallthrough
			case TABLE_HISTORY_TAKEFROMPOT:
			{
				PASSERT(hand.history[i].position >= 0 && hand.history[i].position < MAX_TABLE_PLAYERS);
				PASSERT(hand.history[i].action >= 0 && hand.history[i].action < MAX_NUM_POTS);

				if (boardCounter)
					userNet2[userSeats[hand.history[i].position]] += hand.history[i].amount;
				else
					userNet[userSeats[hand.history[i].position]] += hand.history[i].amount;
				pot[hand.history[i].action] += hand.history[i].amount; //?? how to implement

				if (hand.history[i].action > sidePot) // just for indication( main, side, side-2 )
					sidePot = hand.history[i].action;

				_formatScaledChips(hand.history[i].amount, effectiveScalePM, Denominator, str_stakes);
				i18nAppend(str_dealer, _formatChips(str_chips_buf, locale, hand, currSign, str_stakes));

				if (hand.history[i].action)//if side pot
				{
					if (sidePot > 1) // Number of side Pot
					{
						sprintf(str_number, "-%d ", hand.history[i].action);
					}

					PMsg(str_action, LocalesTable, locale, P_TXT(TXTHHY_X040side));
				}
				else if (sidePot)
				{
					PMsg(str_action, LocalesTable, locale, P_TXT(TXTHHY_X040main));
				}

				if ( hand.history[i].eventCode != TABLE_HISTORY_HOUSE_TAKEFROMPOT ) // no message for cashout take from pot event
				{
					//HISTORY_MOVEFROMPOT %0 collected %1 from %2 pot%3
					PMsg4(eventHistoryStr, LocalesTable, locale, P_TXT(TXTHHY_N0_collected_N1_fromN2_potN3), str_userName, str_dealer, str_action, str_number);
				}
			}
			break;

			case TABLE_HISTORY_UNFOLD_TAKEFROMPOT:
				{
					PASSERT( hand.history[ i ].position >= 0 && hand.history[ i ].position < MAX_TABLE_PLAYERS );
					PASSERT( hand.history[ i ].action >= 0 && hand.history[ i ].action < MAX_NUM_POTS );
					if (hand.game == HandTypeUnfoldClassic)
					{
						if (boardCounter)
							userNet2[userSeats[hand.history[i].position]] += hand.history[i].amount;
						else
							userNet[userSeats[hand.history[i].position]] += hand.history[i].amount;
						potUnfold[hand.history[i].action] += hand.history[i].amount; //?? how to implement

						if (hand.history[i].action > sidePotUnfold) // just for indication( main, side, side-2 )
							sidePotUnfold = hand.history[i].action;

						_formatScaledChips(hand.history[i].amount, effectiveScalePM, Denominator, str_stakes);
						i18nAppend(str_dealer, _formatChips(str_chips_buf, locale, hand, currSign, str_stakes));

						if (hand.history[i].action)//if side pot
						{
							if (sidePotUnfold > 1) // Number of side Pot
							{
								sprintf(str_number, "-%d ", hand.history[i].action);
							}

							PMsg(str_action, LocalesTable, locale, P_TXT(TXTHHY_X040side));
						}
						else if (sidePotUnfold)
						{
							PMsg(str_action, LocalesTable, locale, P_TXT(TXTHHY_X040main));
						}

						if (sidePotUnfold > 1)
						{
							PMsg3(eventHistoryStr, LocalesTable, locale, P_TXT(TXTHHY_N0X_collected_N1_from_Unfold_s_ELL), str_userName, str_dealer, str_number);
						}
						else
						{
							PMsg2(eventHistoryStr, LocalesTable, locale, P_TXT(TXTHHY_N0X_collected_N1_from_Unfold_p_ELL), str_userName, str_dealer);
						}
						
					}
				}
				break;			

			case TABLE_HISTORY_ALLIN:
				{
					if( hand.history[ i ].action == '!' )
					{
						sprintf( str_stakes, "%u", hand.history[ i ].amount/Denominator );
						PMsg3( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_re_buys_and_receives_N1_chi_ELL_2 ), str_userName, str_stakes, _formatChips( str_chips_buf, locale, hand, currSign, hand.history[ i ].contents, true ) );
					}
					else if( hand.history[ i ].action == '#' )
					{
						sprintf( str_stakes, "%u", hand.history[ i ].amount/Denominator );
						PMsg3( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_takes_the_add_on_and_receiv_ELL_2 ), str_userName, str_stakes, _formatChips( str_chips_buf, locale, hand, currSign, hand.history[ i ].contents, true ) );
					}
					else if( hand.history[ i ].action == '&' )
					{
						sprintf( str_stakes, "%u", hand.history[ i ].amount/Denominator );
						PMsg3( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_re_buys_and_receives_N1_chi_ELL_3 ), str_userName, str_stakes, hand.history[ i ].contents );
					}
					else if( hand.history[ i ].action == '%' )
					{
						sprintf( str_stakes, "%u", hand.history[ i ].amount/Denominator );
						PMsg3( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_takes_the_add_on_and_receiv_ELL_3 ), str_userName, str_stakes, hand.history[ i ].contents );
					}

					else if( hand.history[ i ].action == '>' || hand.history[ i ].action == '<' )
					{ //PYR-22532:
						if( admin )
						{
							_formatScaledChips( hand.history[ i ].amount, effectiveScalePM, Denominator, str_stakes );

							i18nAppend( eventHistoryStr, str_userName );
							( hand.history[ i ].action == '>' ) ?
								i18nAppend( eventHistoryStr, " reserve rebuy allocated " ):
								i18nAppend( eventHistoryStr, " rebuys from reserve "	);
							i18nAppend( eventHistoryStr, _formatChips( str_chips_buf, locale, hand, currSign, str_stakes ) );
						}
					}

					else if( hand.history[ i ].action == '$' )
					{
						if( admin )
						{
							_formatScaledChips( hand.history[ i ].amount, effectiveScalePM, Denominator, str_stakes );
							_formatChips( str_chips_buf, locale, hand, currSign, str_stakes );
							PMsg2( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_rebuys_N1 ), str_userName, str_chips_buf );
						}
					}
					else if( !( hand.chipsType == POKER_DUEL_CHIPS && hand.history[ i ].action == 'S' ) )  // PYR-36544 in Duel, the action is 'S'
					{
						i18nAppend( eventHistoryStr, str_userName );
						PMsg( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_X040is_being_treated_as_all_in ) );
					}
				}
				break;

			case TABLE_HISTORY_JOINS:
				{
					if( hand.history[ i ].action )
					{
						sprintf( str_number, "%d ", hand.history[ i ].position + 1 );   
						PMsg2( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_joins_the_table_at_seat_XN1 ), str_userName, str_number );
					}
					else
						PMsg1( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_leaves_the_table ), str_userName );
				}
				break;

			case TABLE_HISTORY_SITIN:
				{
					if( hand.history[ i ].action )
						PMsg1( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_has_returned ), str_userName );
					else
						PMsg1( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_is_sitting_out ), str_userName );
				}
				break;

			case TABLE_HISTORY_CONNECT:
				{
					if( hand.history[ i ].action )
						PMsg1( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_is_connectedX040 ), str_userName );
					else
						PMsg1( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_is_disconnectedX040 ), str_userName );
				}
				break;

			case TABLE_HISTORY_KICKED:
				{
					if(hand.history[ i ].action != TABLE_KICK_REASON_GEOIP_UNKNOWN)
						PMsg1( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_was_removed_from_the_table__ELL ), str_userName );
				}
				break;

			case TABLE_HISTORY_THROW:
				{ //-V1037
					// PYR-100733: ignore this event in hand history!
				}
				break;

			case TABLE_HISTORY_EXTRA:
				{
					switch ( hand.history[ i ].action )
					{
					case TH_EX_Act_Hand_Cancelled_Bets_Returned:
						PMsg( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_Hand_cancelled ) );
						break;
					case TH_EX_Act_Betting_is_Capped:
						PMsg( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_Betting_is_capped ) );
						break;
					case TH_EX_Act_Player_Allowed_Play_After_Button:
						PMsg1( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_will_be_allowed_to_play_aft_ELL ), str_userName );
						break;
					case TH_EX_Act_Pair_On_Board_Double_Bet_Allowed:
						PMsg( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_Pair_on_board___a_double_bet_i_ELL ) );
						break;
					case TH_EX_Act_No_Low_Hand:
						PMsg( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_No_low_hand_qualified ) );
						break;
					case TH_EX_Act_Deck_Is_Reshuffled:
						//if( admin )//Bug #8187:
						//i18nAppend( eventHistoryStr, "The deck is reshuffled" );
						PMsg( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_The_deck_is_reshuffled ) );
						break;
					case TH_EX_Act_Tourn_Deal_Attempt_Completed:
						{
							if( hand.history[i].amount != 0 )
							{
								PMsg( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_AdministratorX_Deal_AgreedX ) );
							}
							else
							{
								PMsg( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_AdministratorX_Deal_CancelledX ) );
							}
						}
						break;
					case TH_EX_Act_Draw_Ordinal:
						{
							if( round != hand.history[ i ].position )
								round = hand.history[ i ].position;
							if( hand.game == HandType3Draw27 || hand.game == HandType3DrawA5 || hand.game == HandTypeBadugi )
							{
								if( round == 1 )
									PMsg( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_XX_FIRST_DRAW_XX ) );
								else if( round == 3 )
									PMsg( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_XX_SECOND_DRAW_XX ) );
								else if( round == 5 )
									PMsg( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_XX_THIRD_DRAW_XX ) );
								else
									PLog( "formatHistory.cpp::ERROR:Unknown TripleDraw round: round=%d, handId=%s", round, str_hand );
							}
						}		
						break;
					case TH_EX_Act_Uncalled_Bet_Returned_To_Player:
						{ //Bug #8192:
							_formatScaledChips( hand.history[ i ].amount, effectiveScalePM, Denominator, str_stakes );

							i18nAppend( str_dealer, _formatChips( str_chips_buf, locale, hand, currSign, str_stakes ) );
							PMsg2( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_Uncalled_bet_XN0X_returned_to__ELL ), str_dealer, str_userName );
							//PTRACE( "Moving chips to pocket!" );
						}
						break;
					case TH_EX_Act_All_In_Show: //AllInShow
						break;

					case TH_EX_Act_Win_Bounty_For_Eliminating: //Bounty
						{
							PString winner;
							const char* p, *p_start;
							int numWinners = 0;

							for( p = hand.history[ i ].contents.c_str(), p_start = hand.history[ i ].contents.c_str(); *p; ++p )
							{
								if( *p == '\v' )
								{
									winner.assign( p_start, p );
									p_start = p + 1;
									numWinners++;

									if( numWinners > 1 )
									{
										i18nAppendChar( str_dealer, ',' );
										i18nAppendChar( str_dealer, ' ' );
									}
									i18nAppend( str_dealer, winner );
								}
							}

							if( hand.tournIsPlayMoney)
							{ // PYR-25238:
								_formatScaledChips( hand.history[ i ].amount, storedScalePM, 100, str_stakes ); // not a Denominator! As it refers to prise award not 'tournament chips', and always is in cents.
							}
							else
							{
								UINT32 rest = hand.history[ i ].amount%100; // not a Denominator! As it refers to prise award not 'tournament chips', and always is in cents.
								if( rest )
									sprintf( str_stakes, "%u.%02u", hand.history[ i ].amount/100, rest );
								else
									sprintf( str_stakes, "%u", hand.history[ i ].amount/100 );
							}

							if( numWinners > 1 ) //%0 split the %1 bounty for eliminating %2
								PMsg3( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_split_the_N1_bounty_for_eli_ELL ), str_dealer, _formatChips( str_chips_buf, locale, hand, currSign, str_stakes, true ), str_userName );
							else				//%0 wins the %1 bounty for eliminating %2
								PMsg3( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_wins_the_N1_bounty_for_elim_ELL ), str_dealer, _formatChips( str_chips_buf, locale, hand, currSign, str_stakes, true ), str_userName );
						}
						break;

					case TH_EX_Act_Win_Bounty_In_Progressive_Knockout:
						{ //PYR-25199:
							char headBounty[ 32 ] = "", bountyToHead[ 32 ] = "";
							const char* p, *p_start;
							UINT32 bounty[ 2 ] = {0,0};
							int b_num = 0;// index for bounty[]
							for( p = hand.history[ i ].contents.c_str(), p_start = hand.history[ i ].contents.c_str(); *p; ++p )
							{
								if( *p == '\v' )
								{// VT:
									str_dealer.assign( p_start, p );
									p_start = p + 1;
								}
								else if( *p == '\a' )
								{// BELL:
									if( b_num < 2 ) //just protection from SEH
									{
										char* dummy;
										bounty[ b_num++ ] = strtoul( p_start, &dummy, 10 );
									}
									p_start = p + 1;
								}
							}

							if( hand.tournIsPlayMoney)
							{ // PYR-25238:  not a Denominator! As it refers to prise award not 'tournament chips', and always is in cents.
								_formatScaledChips( hand.history[ i ].amount, storedScalePM, 100, str_stakes );
								_formatScaledChips( bounty[ 0 ], storedScalePM, 100, headBounty );
								_formatScaledChips( bounty[ 1 ], storedScalePM, 100, bountyToHead );
							}
							else
							{
								UINT32 rest = hand.history[ i ].amount%100; // not a Denominator! As it refers to prise award not 'tournament chips', and always is in cents.
								if( rest )
									sprintf( str_stakes, "%u.%02u", hand.history[ i ].amount/100, rest );
								else
									sprintf( str_stakes, "%u", hand.history[ i ].amount/100 );

								rest = bounty[ 0 ]%100;
								if( rest ) //headBounty
									sprintf( headBounty, "%u.%02u", bounty[ 0 ]/100, rest );
								else
									sprintf( headBounty, "%u", bounty[ 0 ]/100 );

								rest = bounty[ 1 ]%100;
								if( rest ) //bountyToHead
									sprintf( bountyToHead, "%u.%02u", bounty[ 1 ]/100, rest );
								else
									sprintf( bountyToHead, "%u", bounty[ 1 ]/100 );
							}

							if( hand.history[ i ].position > 1 ) //%0 wins %1 for splitting the elimination of %2 and their own bounty increases by %3 to %4
								PMsg5( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_wins_N1_for_splitting_the_e_ELL ),
								str_dealer,
								_formatChips( str_chips_buf, locale, hand, currSign, str_stakes, true ),
								str_userName,
								_formatChips( str_chips_buf_2, locale, hand, currSign, headBounty, true ),
								_formatChips( str_chips_buf_3, locale, hand, currSign, bountyToHead, true ) );
							else //%0 wins %1 for eliminating %2 and their own bounty increases by %3 to %4
								PMsg5( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_wins_N1_for_eliminating_N2__ELL ),
								str_dealer,
								_formatChips( str_chips_buf, locale, hand, currSign, str_stakes, true ),
								str_userName,
								_formatChips( str_chips_buf_2, locale, hand, currSign, headBounty, true ),
								_formatChips( str_chips_buf_3, locale, hand, currSign, bountyToHead, true ) );
						}
						break;

					case TH_EX_Act_Bounty_In_Golden_SNG_Reward:
						{ //PYR-25768:
							_formatScaledChips( hand.history[ i ].amount, 1, 100, str_stakes ); // not a Denominator! As it refers to prise award not 'tournament chips', and always is in cents.
							PMsg2( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_will_also_receive_a_Golden__ELL ), str_userName, _formatChips( str_chips_buf, locale, hand, currSign, str_stakes, true ) );
						}
						break;

					case TH_EX_Act_FinishedTourn:
						{ //Bug 14315: finished

							//PYR-37049
							INT32 place = hand.history[ i ].amount;
							if( place > DELAYED_PLACE_CUTOFF )
							{
								if( hand.tournPlaceDelta > 0 ) //place is defined
								{
									place -= hand.tournPlaceDelta;
								}
								else //place not defined
								{
									PMsg1( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_finished_the_tournament ), str_userName );
									break;
								}
							}

							i18nFormatNth( str_action, LocalesTable, locale, place );
							if( *hand.history[ i ].contents )
							{
								if( hand.history[ i ].position )
								{
									str_dealer = "";
									PMsg1( str_dealer, LocalesTable, locale, P_TXT( TXTHHY_N0_FPP ), hand.history[ i ].contents );
								}
								else
								{
									PString strWinnings; // PYR-25238
									if( hand.tournIsPlayMoney && storedScalePM > 1 )
									{
										convertChipsStringToScaledChipsString( hand.history[ i ].contents, storedScalePM, strWinnings );
									}
									else
									{
										strWinnings = hand.history[ i ].contents;
									}
									str_dealer.assign( _formatChips( str_chips_buf, locale, hand, currSign, strWinnings, true ) );
								}

								PMsg3( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_finished_the_tournament_in__ELL_1 ), str_userName, str_action, str_dealer );
							}
							else
								PMsg2( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_finished_the_tournament_in__ELL ), str_userName, str_action );
						}
						break;

					case TH_EX_Act_WonTourn:
						{ //Bug 14315: won as 1st place:
							if( *hand.history[ i ].contents )
							{
								if( hand.history[ i ].position )
								{
									str_dealer = "";
									PMsg1( str_dealer, LocalesTable, locale, P_TXT( TXTHHY_N0_FPP ), hand.history[ i ].contents );
								}
								else
								{
									PString strWinnings; // PYR-25238
									if( hand.tournIsPlayMoney && storedScalePM > 1 )
									{
										convertChipsStringToScaledChipsString( hand.history[ i ].contents, storedScalePM, strWinnings );
									}
									else
									{
										strWinnings = hand.history[ i ].contents;
									}
									str_dealer.assign( _formatChips( str_chips_buf, locale, hand, currSign, strWinnings, true ) );
								}
								PMsg2( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_wins_the_tournament_and_N1__ELL ), str_userName, str_dealer );
							}
							else
								PMsg1( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_wins_the_tournament___congr_ELL ), str_userName );
						}
						break;

					case TH_EX_Act_WonAward:
						{
							if( *hand.history[ i ].contents )
							{ //admission ticket:
								PMsg1( str_action, LocalesTable, locale, P_TXT( TXTHHY_a_XN0X_ticket ), hand.history[ i ].contents );
							}

							if( hand.history[ i ].amount )
							{//satellite winner:
								if( *str_action )
								{
									i18nAppendChar( str_action, ',' );
									i18nAppendChar( str_action, ' ' );
								}

								p_utoa( hand.history[ i ].amount, str_stakes );
								PMsg1( str_action, LocalesTable, locale, P_TXT( TXTHHY_an_entry_to_tournament_XN0 ), str_stakes );
							}

							PMsg2( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_wins_N1 ), str_userName, str_action );
						}
						break;

						/*case TH_EX_Act_BuyInReward:
						{ //PYR-24382:
						_formatScaledChips( hand.history[ i ].amount, 1, Denominator, str_stakes );
						PMsg2( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_received_a_Golden_SitXGo_bu_ELL ), str_userName, _formatChips( str_chips_buf, locale, hand, currSign, str_stakes, true ) );
						}
						break;
						*/
					case TH_EX_Act_PrizePoolReward:
						{ //PYR-24382:
							_formatScaledChips( hand.history[ i ].amount, 1, 100, str_stakes ); // not a Denominator! As it refers to prise award not 'tournament chips', and always is in cents.
							PMsg2( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_also_received_a_Golden_SitX_ELL ), str_userName, _formatChips( str_chips_buf, locale, hand, currSign, str_stakes, true ) );
						}
						break;

					case TH_EX_Set_Cap_Value:
						{
							tableCap = hand.history[ i ].amount;
						}
						break;

					case TH_EX_Act_HideCards_After_FoldAndShow:
						{
							// #15261
							if( admin )
							{
								PString cardsStr;
								const char* str_p = foldAndShowCards[ userSeats[ hand.history[ i ].position ] ].c_str();
								while( *str_p )
								{
									i18nAppendChar( cardsStr, *str_p );
									if( *( ++str_p ) )
										i18nAppendChar( cardsStr, *str_p );

									if( *( ++str_p ) )
										PMsg( cardsStr, LocalesTable, locale, P_TXT( TXTHHY_X040 ) );
								}

								PMsg2(eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_Folding_shown_cards_N0_showsX__ELL ), str_userName, cardsStr);
							}
						}
						break;

					case TH_EX_Act_Start_Run_It_Twice:
						{
							PASSERT( boardCounter == 0 && !runItTwice );
							runItTwice = true;

							int numShardBoardCards = hand.history[ i ].amount;
							if(numShardBoardCards > 0)
							{
								// Copy the previous cards from the first board into the second.
								boardCardsPstr[1] = boardCardsPstr[0];
								boardCardsPstr[1].cutAtChar(numShardBoardCards * 2);
							}
						}
						break;

					case TH_EX_Act_Run_It_Twice_Next_Board:
						{
							PASSERT( runItTwice );
							++boardCounter;
							round = NonDetermined;  // reset round
						}
						break;
					case TH_EX_Act_Run_It_Twice_Evaluate_First_Board:
						{
							boardCounter = 0;

							PString showDownString;
							PMsg( showDownString, LocalesTable, locale, boardName( boardCounter ) );
							PMsg( showDownString, LocalesTable, locale, P_TXT( TXTHHY_X040 ) );
							PMsg( showDownString, LocalesTable, locale, P_TXT( TXTHHY_SHOW_DOWN ) );
							PMsg1( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_XX_N0_XX ), showDownString );
						}
						break;
					case TH_EX_Act_Run_It_Twice_Evaluate_Second_Board:
						{
							boardCounter = 1;

							PString showDownString;
							PMsg( showDownString, LocalesTable, locale, boardName( boardCounter ) );
							PMsg( showDownString, LocalesTable, locale, P_TXT( TXTHHY_X040 ) );
							PMsg( showDownString, LocalesTable, locale, P_TXT( TXTHHY_SHOW_DOWN ) );
							PMsg1( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_XX_N0_XX ), showDownString );

							_bestHigh = 0;  // reset this local variable to start the second board evaluation
						}
						break;
					case TH_EX_Act_Pot_CarryOver:
						{
							sprintf( str_number, "%u", hand.history[ i ].amount );
							PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "Starting Pot: %0 chips.", str_number );
						}
						break;
					case TH_EX_Act_Break_Skipped_And_Game_Resuming:
						{
							PMsg(eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_All_players_have_agreed_to_ski_ELL ));
						}
						break;
					case TH_EX_Act_Unfold_Ante_To_Be_Returned:
						{							
							if (hand.board.length()) // hand didn't end preflop
							{
								if (hand.history[i].amount == 0) // No players folded preflop
								{
									PMsg(unfoldAnteReturnSummaryStr, LocalesTable, locale, P_TXT(TXTHHY_Unfold_potX_No_players_folded__ELL));
								}
								else if (hand.history[i].amount == 1) // only 1 player unfolded
								{
									PMsg(unfoldAnteReturnSummaryStr, LocalesTable, locale, P_TXT(TXTHHY_Unfold_potX_Only_1_player_fold_ELL));
								}
								else // no players unfolded case
								{
									PMsg(unfoldAnteReturnSummaryStr, LocalesTable, locale, P_TXT(TXTHHY_Unfold_potX_No_players_unfolde_ELL));
								}
							}
						}
						break;
					case TH_EX_Act_Set_Variable_Ante:
						{
							_formatScaledChips(hand.history[i].amount, effectiveScalePM, Denominator, str_stakes);
							_formatChips(str_chips_buf, locale, hand, currSign, str_stakes);

							if( hand.history[ i ].position ) // position == 1 means there was a showdown
							{
								if( *hand.history[ i ].contents && hand.history[ i ].contents[ 0 ] == '=' )
								{
									PMsg1( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_Showdown_in_previous_hand_keep_ELL ), str_chips_buf );
								}
								else
								{
									PMsg1( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_Showdown_in_previous_hand_rese_ELL ), str_chips_buf );
								}
							}
							else
							{
								if( *hand.history[ i ].contents && hand.history[ i ].contents[ 0 ] == '=' )
								{
									PMsg1( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_No_showdown_in_previous_hand_k_ELL ), str_chips_buf );
								}
								else if( *hand.history[ i ].contents && hand.history[ i ].contents[ 0 ] == '<' )
								{
									PMsg1( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_No_showdown_in_previous_hand_r_ELL ), str_chips_buf );
								}
								else
								{
									PMsg1( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_No_showdown_in_previous_hand_e_ELL ), str_chips_buf );
								}
							}
						}
						break;
					default:
						{
							//if( hand.history[ i ].action > 0xF )
							//	PLog( "formatHistory.cpp::ERROR:Unknown HISTORY_EVENT: Code=%03X, Action=%02X - ignored", hand.history[ i ].eventCode, hand.history[ i ].action );
						}
					}
				}
				break;

			case TABLE_HISTORY_CHAT:
				{
					i18nAppend( eventHistoryStr, str_userName );

					if( hand.history[ i ].position < 0 )
						PMsg( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_X040XobserverX ) ); // str.append( "[observer]" )
					//HISTORY_CHAT  said, "%1"
					// void _filterChat( PString& chat, vector< PString >& myPattern )
					str_action = hand.history[ i ].contents;

#ifndef PROTOCOL_CLIENT_ONLY
					if( !admin )
					{
						_filterChat( str_action, myPattern, pChatFilterHelper );
					}
#endif

					PMsg1( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_X040saidX_XXN0XX ), str_action );
					//PMsg1( eventHistoryStr, msgTable, P_TXT( TXT_HISTORY_CHAT ), hand.history[ i ].contents );
				}
				break;

			case TABLE_HISTORY_TIMEBANK:
				{
					if( admin )
					{
						if( hand.history[ i ].action == 'T' )
							PMsg1( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_Player_N0_has_requested_TIME ), str_userName );
						else if( hand.history[ i ].action == 't' )
							PMsg1( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_Regular_time_for_player_N0_has_ELL ), str_userName );
						else if( hand.history[ i ].action == 'A' )
						{
							sprintf( str_number, "%u", hand.history[ i ].amount );
							PMsg1( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_seconds_added_to_timebank_o_ELL ), str_number );
						}
						else if( hand.history[ i ].action == 'M' )
						{
							sprintf( str_number, "%u", hand.history[ i ].amount );
							PMsg1( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_Players_maximum_timebank_has_b_ELL ), str_number );
						}
					}
				}
				break;

			case TABLE_HISTORY_BEENSITOUT:
				{
					// do nothing
				}
				break;

			case TABLE_HISTORY_STORM_POWER:
				{
					// Only get the player's name if there is a player.
					PString playerName;
					if(hand.history[ i ].position >= 0)
						playerName = hand.users[userSeats[ hand.history[ i ].position ]].name;

					// Only get the target's name if there is a target.
					PString targetName;
					if(hand.history[ i ].action >= 0)
						targetName = hand.users[userSeats[ hand.history[ i ].action ]].name;

					if(hand.history[ i ].amount == PokerGamePowerAnteUp)
					{
						PASSERT5(!isAnteUpPowerStarted);
						isAnteUpPowerStarted = true;
						anteUpPlayer = hand.history[ i ].position;
						PASSERT5(anteUpPlayer >= 0);
					}

					formatPowerEvent2( isServer, hand, hand.history[ i ], playerName, targetName, locale, myUserAlias, admin, eventHistoryStr, hasStretchRound, powerIdAffectingBoard);
				}
				break;

			case TABLE_HISTORY_STORM_EFFECT:
				{
					if(hand.history[ i ].amount == PokerGamePowerEffectFreezeBoardCardsByPlayerAllIn)
					{
						freezeBoardCardsWhenAllIn = true;
					}
					else
					{
						// Only get player seat if the effect is on a player.
						PString playerName;
						if(hand.history[ i ].position >= 0)
						{
							playerName = hand.users[userSeats[ hand.history[ i ].position ]].name;
						}
						formatPowerEffect( isServer, hand, hand.history[ i ], playerName, locale, myUserAlias, boardCardsPstr, runItTwice, admin, eventHistoryStr, powerIdAffectingBoard );
					}
				}
				break;

			default:
				//PLog( "formatHistory.cpp::ERROR:Unknown HISTORY_EVENT: Code=%03X - ignored", hand.history[ i ].eventCode );
				break;
			}

			appendOneLineHandHistoryEvent( admin, hand.history[ i ].when, historyStr, eventHistoryStr );
		}  // End of Main Loop for History Events
	} // end of 'for(int step=0; step<2;++step)'

	//PYR-79935 after showdown show users who cashed out
	if ( round == Showdown )
	{
		for ( int iCashoutUserNum = 0; iCashoutUserNum < usersNum; ++iCashoutUserNum )
		{
			if ( userCashoutIndexInEvents[iCashoutUserNum] >= 0 )
			{
				const auto& historyItem = hand.history[userCashoutIndexInEvents[iCashoutUserNum]];

				PASSERT(historyItem.action == 'h' && userSeats[historyItem.position] == iCashoutUserNum);

				char cashoutAmtHelper[32] = "";
				PString cashoutAmt;
				PString outStr;
				
				_formatScaledChips(historyItem.amount, effectiveScalePM, Denominator, cashoutAmtHelper);
				_formatChips(cashoutAmt, locale, hand, currSign, cashoutAmtHelper);
				
				PString str_userName = getUserName( historyItem.userId, userAlias, historyItem.position, myUserAlias, admin, showStealthTableAlias, myRealUserId );

				PMsg2(outStr, LocalesTable, locale, P_TXT(TXTHHY_N0_cashed_out_the_hand_for_N1), str_userName, cashoutAmt);
				INT64 fee = 0;
				if ( historyItem.contents.length() > 0
					&& strToNumber(fee, historyItem.contents, nullptr, /*base*/ 10)
					&& fee != 0 )
				{
					char feeStr[32];
					_formatScaledChips(fee, effectiveScalePM, Denominator, feeStr);

					PString outChips;
					i18nAppend(outStr, " | ");
					PMsg1(outStr, LocalesTable, locale, P_TXT(TXTHHY_Cash_Out_Fee_N0), _formatChips(outChips, locale, hand, currSign, feeStr));
				}

				appendOneLineHandHistoryEvent(admin, historyItem.when, historyStr, outStr);
			}
		}
	}

	//*** SUMMARY ***
	PMsg( historyStr, LocalesTable, locale, roundName( 6, true ) ); //Printf "*** Summary ***"
	i18nAppendChar( historyStr, '\n' );
	str_action = "";
	str_dealer = "";

	// hand.potSize and rakeSize contain the game pot + unfold pot; following is done to get onely gamePot and rake
	INT32 gamePotSize = hand.potSize - hand.handExtraPot.potSize;
	INT32 gameRakeSize = hand.rakeSize - hand.handExtraPot.rake;

	if(gamePotSize > 0) // precaution, must always be positive
	{//(add) Pot 76  | Rake 2
		//if( !hand.isPlayMoney ) 
		//if ( hand.chipsType == REAL_CHIPS )
		//	PMsg( str_dealer, msgTable, P_TXT( TXT_DEALERCHAT_DOLLAR ) ); 
		_formatScaledChips(gamePotSize, effectiveScalePM, Denominator, str_stakes );

		i18nAppend( str_dealer, _formatChips( str_chips_buf, locale, hand, currSign, str_stakes ) );
		PMsg1( historyStr, LocalesTable, locale, P_TXT( TXTHHY_Total_pot_N0 ), str_dealer );

		if( sidePot )
			for( i = 0; i < MAX_NUM_POTS; ++i ) //means MAX_POT_NUMBERS
			{
				str_action = "";
				str_dealer = "";
				//(add) Pot xxx.  Side pot 1 xxx.  Side pot 2 xxx.  Side pot 3 xxx
				if( pot[ i ] )
				{
					//if( !hand.isPlayMoney ) 
					//if ( hand.chipsType == REAL_CHIPS )
					//	PMsg( str_dealer, msgTable, P_TXT( TXT_DEALERCHAT_DOLLAR ) ); 
					_formatScaledChips( pot[ i ], effectiveScalePM, Denominator, str_stakes );
					i18nAppend( str_dealer, _formatChips( str_chips_buf, locale, hand, currSign, str_stakes ) );

					if( i > 0 && sidePot > 1 ) // if more than one side pot
						sprintf( str_number, "-%u", i );
					else 
						str_number[ 0 ] = '\x0'; // ""

					if( i )
						PMsg( str_action, LocalesTable, locale, P_TXT( TXTHHY_X040Side ) );
					else
						PMsg( str_action, LocalesTable, locale, P_TXT( TXTHHY_X040Main ) );

					//HISTORY_POTSUM %0 pot%1 %2.
					PMsg3( historyStr, LocalesTable, locale, P_TXT( TXTHHY_N0_potN1_N2X ), str_action, str_number, str_dealer );
				}
			}

			if( gameRakeSize >= 0 ) // rake !All cases( including rake == 0 ); precaution - must be positive
			{
				str_dealer = "";
				//if( !hand.isPlayMoney ) 
				//if ( hand.chipsType == REAL_CHIPS )
				//	PMsg( str_dealer, msgTable, P_TXT( TXT_DEALERCHAT_DOLLAR ) ); 
				_formatScaledChips( gameRakeSize, effectiveScalePM, Denominator, str_stakes );

				i18nAppend( str_dealer, _formatChips( str_chips_buf, locale, hand, currSign, str_stakes ) );
				PMsg1( historyStr, LocalesTable, locale, P_TXT( TXTHHY_X040X_Rake_N0X040 ), str_dealer );
			}
	}

	// AQA-1095 - Adding parasited chips information.
	if( hand.carryOverToNextHand )
	{
		str_dealer = "";
		_formatScaledChips( hand.carryOverToNextHand, effectiveScalePM, Denominator, str_stakes );

		i18nAppend( str_dealer, _formatChips( str_chips_buf, locale, hand, currSign, str_stakes ) );
		PMsgNoTranslate( historyStr, LocalesTable, locale, " | Parasite %0", str_dealer );
	}

	if( historyStr[ historyStr.length() - 1 ] != '\n' )
		i18nAppendChar( historyStr, '\n' );

	int totalBoardNumber = 1;

	if( runItTwice )
	{
		if( !(hand.game == HandTypeSplitHoldem || hand.game == HandTypeSplitOmaha) )
		{
			PMsg( historyStr, LocalesTable, locale, P_TXT( TXTHHY_Hand_was_run_twice ) );
			i18nAppendChar( historyStr, '\n' );
		}
		totalBoardNumber = 2;
	}

	// unfold pot if applies
	if (hand.game == HandTypeUnfoldClassic)
	{
		// ante return reason not set and board cards empty => hand ended preflop
		// intentionally separated under a different if;
		if (!unfoldAnteReturnSummaryStr.length() && hand.handExtraPot.potSize && !hand.board.length())
		{
			PMsg(unfoldAnteReturnSummaryStr, LocalesTable, locale, P_TXT(TXTHHY_Unfold_potX_Hand_ended_pre_flo_ELL));
		}

		if (!unfoldAnteReturnSummaryStr.length())
		{
			if (hand.handExtraPot.potSize)
			{
				// Unfold Pot 76  | Rake 2
				char str_stakes_unfold[32] = "";
				PString str_potsize_unfold, str_rake_unfold;
				// pot
				_formatScaledChips(hand.handExtraPot.potSize, effectiveScalePM, Denominator, str_stakes_unfold);
				i18nAppend(str_potsize_unfold, _formatChips(str_chips_buf, locale, hand, currSign, str_stakes_unfold));

				// rake
				_formatScaledChips(hand.handExtraPot.rake, effectiveScalePM, Denominator, str_stakes_unfold);
				i18nAppend(str_rake_unfold, _formatChips(str_chips_buf, locale, hand, currSign, str_stakes_unfold));

				if (sidePotUnfold)
				{
					PString historyStrLocal;
					for (i = 0; i < MAX_NUM_POTS; ++i) //means MAX_POT_NUMBERS
					{
						str_action = "";
						str_dealer = "";
						//(add) Unfold Pot xxx.  Unfold Side pot1 xxx.  Unfold Side pot2 xxx.  Unfold Side pot3 xxx
						if (potUnfold[i])
						{ 
							_formatScaledChips(potUnfold[i], effectiveScalePM, Denominator, str_stakes);
							i18nAppend(str_dealer, _formatChips(str_chips_buf, locale, hand, currSign, str_stakes));

							if (i > 0 && sidePotUnfold > 1) // if more than one side pot
								sprintf(str_number, "-%u", i);
							else
								str_number[0] = '\x0'; // ""

							if (i)
							{
								i18nAppend(historyStrLocal, " ");
								PMsg2(historyStrLocal, LocalesTable, locale, P_TXT(TXTHHY_Unfold_side_potN0_N1X), str_number, str_dealer);
							}
							else // main pot
							{
								PMsg2(historyStrLocal, LocalesTable, locale, P_TXT(TXTHHY_Total_Unfold_pot_N0_Unfold_mai_ELL), str_potsize_unfold, str_dealer);
							}
						}
					}
					//HISTORY_POTSUM %0 pot%1 %2.
					PMsg2(historyStr, LocalesTable, locale, P_TXT(TXTHHY_N0_X_Rake_N1), historyStrLocal, str_rake_unfold);
				}
				else
				{
					PMsg2(historyStr, LocalesTable, locale, P_TXT(TXTHHY_Unfold_pot_N0_X_Rake_N1), str_potsize_unfold, str_rake_unfold);
				}
				i18nAppendChar(historyStr, '\n');
			}
		}
		else if (hand.handExtraPot.potSize)
		{
			i18nAppend(historyStr, unfoldAnteReturnSummaryStr.c_str());
			i18nAppendChar(historyStr, '\n');
		}
	}

	//(add) Board [Kh Kd 5d 7c 2h]
	for( int m = 0; m < totalBoardNumber; ++m )
	{
		if( m ? hand.board2[0] : hand.board[0] )
		{
			str_p = m ? hand.board2 : hand.board;
			str_action = "";

			while( *str_p )
			{
				i18nAppendChar( str_action, *str_p );
				if( *( ++str_p ) )
					i18nAppendChar( str_action, *str_p );

				if( *( ++str_p ) )
					PMsg( str_action, LocalesTable, locale, P_TXT( TXTHHY_X040 ) );
			}

			if( runItTwice )
			{
				PMsg( historyStr, LocalesTable, locale, boardName( m ) );
				PMsg( historyStr, LocalesTable, locale, P_TXT( TXTHHY_X040 ) );
			}

			PMsg1( historyStr, LocalesTable, locale, P_TXT( TXTHHY_Board_XN0X ), str_action );
		}

		if( historyStr[ historyStr.length() - 1 ] != '\n' )
			i18nAppendChar( historyStr, '\n' );
	}


	// User's **SUMMARY**
	for( i = 0; i < usersNum; ++i )
	{
		if( hand.users[ i ].result & Been_OutOfHand ) // if out of hand
			continue;			

		str_dealer = "";
		PString str_userName = getUserName( hand.users[i].name, userAlias, hand.users[i].position, myUserAlias, admin, showStealthTableAlias, myRealUserId );
		sprintf( str_number, "%d", hand.users[ i ].position + 1 ); // Seat 2: NickName  
		PMsg2( historyStr, LocalesTable, locale, P_TXT( TXTHHY_Seat_N0X_N1 ), str_number, str_userName );

		if( hand.isButtonBlindHand() )  // PYR-74693
		{
			if( hand.users[i].position == hand.dealer )
				PMsg( historyStr, LocalesTable, locale, P_TXT( TXTHHY_X040Xbutton_blindX ) );
		}
		else
		{
			if( hand.users[i].position == hand.dealer )
				PMsg( historyStr, LocalesTable, locale, P_TXT( TXTHHY_X040XbuttonX ) );

			if( SB_posted & hand.users[i].result )
				PMsg( historyStr, LocalesTable, locale, P_TXT( TXTHHY_X040Xsmall_blindX ) );

			if( BB_posted & hand.users[i].result )
				PMsg( historyStr, LocalesTable, locale, P_TXT( TXTHHY_X040Xbig_blindX ) );

			if( GB_posted & hand.users[i].result )
				PMsg( historyStr, LocalesTable, locale, P_TXT( TXTHHY_X040Xgiant_blindX ) );
		}

		// { NoFlag = 0x0, HandFlag = 0x01, LeftFlag = 0x02, FoldFlag = 0x4, FoldBeforeFlop = 0x8,
		// SeenFlop = 0x10, MuckFlag = 0x20, ShownCards = 0x40, SeenShowdown = 0x80,
		// Street_4th = 0x100, Street_5th = 0x200, Street_6th = 0x400, BeenWinner = 0x800, 
		// MoreThanAnte_posted = 0x1000, BB_posted = 0x2000, SB_posted = 0x4000, Been_sitOut = 0x8000, 
		// Been_OutOfHand = 0x10000 };

		if( ( hand.users[ i ].result & FoldFlag ) || ( hand.users[ i ].result & FoldAndShow ) ) // if folded
		{
			if(	hand.game == HandTypeStud || hand.game == HandTypeRazz )
				round = 4;
			else if( hand.game == HandType3Draw27 || hand.game == HandType3DrawA5 || hand.game == HandTypeBadugi )
				round = 6;
			else if( hand.game == HandTypeDraw || hand.game == HandTypeSingleDraw27 )
				round = 2;
			else if( hand.isStormHand() || hasStretchRound ) // Storm stretched hands can add up to 2 new streets.
				round = 5;
			else // Holdem/Omaha
				round = 3;
			
			if( roundFold[ i ] < 0 || roundFold[ i ] > round /*maxRound*/ )
			{
				char buf[ 256 ];
				sprintf( buf, "Problematic handId=%s! Contact support, please", str_hand );
				throw PError( buf );
			}

			PMsg( historyStr, LocalesTable, locale, P_TXT( TXTHHY_X040folded ) );
			// on round:
			if( hand.game == HandTypeStud || hand.game == HandTypeRazz )
			{// n-Street, River, rounds [0-4]
				PMsg( historyStr, LocalesTable, locale, P_TXT( TXTHHY_X040on_the ) );
				if( roundFold[ i ] < 4 )
				{
					//char str_rnd[ 3 ];
					sprintf( str_number, " %d", roundFold[ i ] + 3 );
					i18nAppend( historyStr, str_number );

					if( roundFold[ i ] )
						PMsg( historyStr, LocalesTable, locale, P_TXT( TXTHHY_th ) );
					else
						PMsg( historyStr, LocalesTable, locale, P_TXT( TXTHHY_rd ) );

					PMsg( historyStr, LocalesTable, locale, P_TXT( TXTHHY_X040Street ) );
				}
				else 
					PMsg( historyStr, LocalesTable, locale, P_TXT( TXTHHY_X040River ) );
			}
			else if( hand.game == HandTypeDraw )
			{
				if( roundFold[ i ] )
					PMsg( historyStr, LocalesTable, locale, P_TXT( TXTHHY_X040after ) );
				else // before draw
					PMsg( historyStr, LocalesTable, locale, P_TXT( TXTHHY_X040before ) );

				PMsg( historyStr, LocalesTable, locale, P_TXT( TXTHHY_X040the ) );
				PMsg( historyStr, LocalesTable, locale, P_TXT( TXTHHY_X040Draw ) );
			}
			else if( hand.game == HandType3Draw27 || hand.game == HandType3DrawA5 || hand.game == HandTypeBadugi )
			{
				if( roundFold[ i ] )
				{
					PMsg( historyStr, LocalesTable, locale, P_TXT( TXTHHY_X040after ) );
					PMsg( historyStr, LocalesTable, locale, P_TXT( TXTHHY_X040the ) );
					//2-4-6 num even rounds in TripleDraw:
					round = roundFold[ i ];
					sprintf( str_number, " %d", 1 + round/2);
					i18nAppend( historyStr, str_number );
					PMsg( historyStr, LocalesTable, locale, tripleDrawRoundSuffix( round ) ); 
				}
				else // before draw
				{
					PMsg( historyStr, LocalesTable, locale, P_TXT( TXTHHY_X040before ) );
					PMsg( historyStr, LocalesTable, locale, P_TXT( TXTHHY_X040the ) );
				}

				PMsg( historyStr, LocalesTable, locale, P_TXT( TXTHHY_X040Draw ) );
			}
			else // holdem & Omaha, rounds [0-3]
			{// flop, turn, river
				if( roundFold[ i ] )
				{
					if( !roundName( roundFold[ i ] ).equals( P_TXT( TXTHHY_EMPTY_STUB ) ) )
					{
						PMsg( historyStr, LocalesTable, locale, P_TXT( TXTHHY_X040on_the ) );
						PMsg( historyStr, LocalesTable, locale, roundName( roundFold[ i ] ) );
					}
				}
				else // before flop
				{
					PMsg( historyStr, LocalesTable, locale, P_TXT( TXTHHY_X040before ) );
					PMsg( historyStr, LocalesTable, locale, roundName( 1 ) );  // round 1 is flop
				}
			}

			if( !( hand.users[ i ].result & MoreThanAnte_posted ) )// if did not bet
				PMsg( historyStr, LocalesTable, locale, P_TXT( TXTHHY_X040XdidnXt_betX ) );

			if (hand.users[i].result & UnfoldFlag)
			{
				if (!(*bestCombPstr[i]))
				{
					fillBestCombPstr(hand, bestCombPstr[i], i, locale);
				}

				PString localHistoryStr;
				PString potSizeStr;
				if (userNet[i] > 0)
				{
					_formatScaledChips(userNet[i], effectiveScalePM, Denominator, str_stakes);

					i18nAppend(potSizeStr, _formatChips(str_chips_buf, locale, hand, currSign, str_stakes));
					PMsg3(localHistoryStr, LocalesTable, locale, P_TXT(TXTHHY_N0X_unfolded_on_the_flop_and_w_ELL_2), historyStr, potSizeStr, bestCombPstr[i]);
				}
				else
				{
					if (!unfoldAnteReturnSummaryStr.length()) // lost:
					{
						PMsg2(localHistoryStr, LocalesTable, locale, P_TXT(TXTHHY_N0X_unfolded_on_the_flop_and_l_ELL_2), historyStr, bestCombPstr[i]);
					}
					else // ante returned
					{
						i18nAppendChar(historyStr, '\n');
						continue;
					}
				}

				historyStr.assign(localHistoryStr);
			}
		}

		else if( ( hand.users[ i ].result & MuckFlag ) || ( hand.users[ i ].result & ShownSomeCards ) ) // if mucked
		{
			PMsg( historyStr, LocalesTable, locale, P_TXT( TXTHHY_X040mucked ) ); // TBD: Add case, when user muck & requested hand (his own cards and best hand)
			//Seat 9: Mark mucked [As Qd] - pair of Kings
			//if( hand.history[ i ].userId.equals( myUserAlias ) || admin )
			if( ( beenInHand || admin ) && *hand.users[ i ].cards && ( *hand.users[ i ].cards != '?' ) )
			{	
				str_p = hand.users[ i ].cards; 
				i18nAppend( str_dealer, " [");
				while( *str_p )
				{
					i18nAppendChar( str_dealer, *str_p );
					if( *( ++str_p ) )
						i18nAppendChar( str_dealer, *str_p );

					if( *( ++str_p ) )
						PMsg( str_dealer, LocalesTable, locale, P_TXT( TXTHHY_X040 ) );
					//historyStr.append( "," );
				}
				i18nAppendChar( str_dealer, ']');

				if( *bestCombPstr[ i ] )
				{
					// append best hand (HI, LO both )
					fillBestCombPstr( hand, bestCombPstr[ i ], i, locale );
					i18nAppend( str_dealer, " - ");
					i18nAppend( str_dealer, bestCombPstr[ i ] );
				}

				if( runItTwice && *bestCombPstr2[ i ] )
				{
					// append best hand (HI, LO both )
					i18nAppend( str_dealer, ", " );
					fillBestCombPstr2( hand, bestCombPstr2[ i ], i, locale );
					i18nAppend( str_dealer, bestCombPstr2[ i ] );
				}
			}
			else if( hand.users[ i ].result & ShownSomeCards )	// #15261 - show some cards for observers
			{
				i18nAppend( str_dealer, " " );
				i18nAppend( str_dealer, allCardsPstr[ i ] );
			}
			i18nAppend( historyStr, str_dealer );
		}

		else if( hand.users[ i ].result & ShownCards ) // if shown
		{// showd
			//Seat 4: bot0853 (big blind) showed [5c 2d]  lost with two pair, Kings and Fives
			//Seat 2: Mark showed [Qd 6c 3d Qc 4h 6s 9d] and wins (73) with two pair, Queens and Sixes
			PMsg1( historyStr, LocalesTable, locale, P_TXT( TXTHHY_X040showed_N0 ), allCardsPstr[ i ] );

			for( int m = 0; m < totalBoardNumber; ++m )
			{
				str_dealer = "";

				if( m > 0 )
					PMsg( historyStr, LocalesTable, locale, P_TXT( TXTHHY_X_1 ) );

				// if wins with open cards:
				//if( hand.users[ i ].result & BeenWinner )

				INT32 userNetValue = m ? userNet2[ i ] : userNet[ i ];
				if( userNetValue > 0 ) 
				{
					//if( !hand.isPlayMoney ) 
					//if ( hand.chipsType == REAL_CHIPS )
					//	PMsg( str_dealer, msgTable, P_TXT( TXT_DEALERCHAT_DOLLAR ) ); 
					_formatScaledChips( userNetValue, effectiveScalePM, Denominator, str_stakes );

					i18nAppend( str_dealer, _formatChips( str_chips_buf, locale, hand, currSign, str_stakes ) );
					PMsg( historyStr, LocalesTable, locale, P_TXT( TXTHHY_X040and_won ) );
					//HISTORY_POTNET  (%0)
					PMsg1( historyStr, LocalesTable, locale, P_TXT( TXTHHY_X040XN0X ), str_dealer );

					if( handSummary.length() )
						i18nAppendChar( handSummary, '\n' ); // split pot situation
					i18nAppend( handSummary, str_userName );
					PMsg1( handSummary, LocalesTable, locale, P_TXT( TXTHHY_X040showed_N0 ), allCardsPstr[ i ] );
					PMsg( handSummary, LocalesTable, locale, P_TXT( TXTHHY_X040and_won ) );
					PMsg1( handSummary, LocalesTable, locale, P_TXT( TXTHHY_X040XN0X ), str_dealer );
				}
				else // lost:
				{
					PMsg( historyStr, LocalesTable, locale, P_TXT( TXTHHY_X040and_lost ) );
				}

				if( m == 0 )
				{
					if( !( *bestCombPstr[ i ] ) ) // just for any case, TBD: remove it
						fillBestCombPstr( hand, bestCombPstr[ i ], i, locale );
					// Add best combination:
					PMsg1( historyStr, LocalesTable, locale, P_TXT( TXTHHY_X040with_N0 ), bestCombPstr[ i ] );
					if( userNetValue > 0 )
					{
						PMsg1( handSummary, LocalesTable, locale, P_TXT( TXTHHY_X040with_N0 ), bestCombPstr[ i ] );
					}
				}		
				else
				{
					if( !( *bestCombPstr2[ i ] ) ) // just for any case, TBD: remove it
						fillBestCombPstr2( hand, bestCombPstr2[ i ], i, locale );
					// Add best combination:
					PMsg1( historyStr, LocalesTable, locale, P_TXT( TXTHHY_X040with_N0 ), bestCombPstr2[ i ] );
					if( userNetValue > 0 )
					{
						PMsg1( handSummary, LocalesTable, locale, P_TXT( TXTHHY_X040with_N0 ), bestCombPstr2[ i ] );
					}
				}

				// PYR-79935
				if ( userNetValue > 0 && userCashoutIndexInEvents[i] >= 0 ) //if won but cashed out
				{
					// Won: Pot not awarded as player cashed out.
					i18nAppend(historyStr, " ");
					PMsg(historyStr, LocalesTable, locale, P_TXT(TXTHHY_Xpot_not_awarded_as_player_cas_ELL));
				}
				else if ( userCashoutIndexInEvents[i] >= 0 )
				{
					//  Lost: (cashed out)
					i18nAppend(historyStr, " ");
					PMsg(historyStr, LocalesTable, locale, P_TXT(TXTHHY_Xcashed_outXX));
				}
			}
		}

		//else if( hand.users[ i ].result & Been_OutOfHand ) // if out of hand
		//	{
		//	; // do nothing TBD maybe later
		//	}
		else // if won pot /all folded/
		{
			//if ( hand.chipsType == REAL_CHIPS )
			//	PMsg( str_dealer, msgTable, P_TXT( TXT_DEALERCHAT_DOLLAR ) ); 

			for( int m = 0; m < totalBoardNumber; ++m )
			{
				INT32 userNetValue = m ? userNet2[ i ] : userNet[ i ];
				if( userNetValue == 0 )  // AQA-13027 possible in split holdem
				{
					continue;
				}

				if( m > 0 )
				{
					PMsg( historyStr, LocalesTable, locale, P_TXT( TXTHHY_X_1 ) );
				}

				_formatScaledChips( userNetValue, effectiveScalePM, Denominator, str_stakes );
				i18nAppend( str_dealer, _formatChips( str_chips_buf, locale, hand, currSign, str_stakes ) );
				//Seat 8: NickName (button) (big blind) collected 10
				PMsg( historyStr, LocalesTable, locale, P_TXT( TXTHHY_X040collected ) );
				PMsg1( historyStr, LocalesTable, locale, P_TXT( TXTHHY_X040XN0X ), str_dealer );

				i18nAppend( handSummary, str_userName );
				PMsg( handSummary, LocalesTable, locale, P_TXT( TXTHHY_X040collected ) );
				PMsg1( handSummary, LocalesTable, locale, P_TXT( TXTHHY_X040XN0X ), str_dealer );

				str_dealer.clear();
			}
		}

		i18nAppendChar( historyStr, '\n' );
	}
}

/*---------------------------------------------------------------------------------------------
formatHistoryFooter
---------------------------------------------------------------------------------------------*/
void formatHistoryFooter( int locale, PString& ret, const char* site )
{
	PMsg1( ret, LocalesTable, locale, P_TXT( TXTHHY__If_you_have_any_questionsX_pl_ELL ), site );
}


static bool showPrivateInfo(bool wantShowPrivate, bool privateInfoPresent, bool isServerSide)
{
	if (!wantShowPrivate)
		return false;
	if (wantShowPrivate && privateInfoPresent) //-V560
		return true;
	PASSERT(wantShowPrivate && !privateInfoPresent);

	if (isServerSide)
	{
		PLog(LOG_TRACE_FAULTY_LOGIC ": No history private info on server side!");
		PASSERT(0);
	}
	return false; // a case of the client which has not got private info
}

static PString suitName(char const suitCode, int locale)
{
	PString retval;

	if (suitCode == 's')
		PMsgNoTranslate( retval, LocalesTable, locale, "spades");
	else if (suitCode == 'c')
		PMsgNoTranslate( retval, LocalesTable, locale, "clubs");
	else if (suitCode == 'd')
		PMsgNoTranslate( retval, LocalesTable, locale, "diamonds");
	else if (suitCode == 'h')
		PMsgNoTranslate( retval, LocalesTable, locale, "hearts");
	else 
		retval = "***";

	return retval;
}

/*---------------------------------------------------------------------------------------------
formatPowerEvent
---------------------------------------------------------------------------------------------*/
void formatPowerEvent( bool isServer, const Hand& hand, const TableHistoryItem& historyItem, const PString& playerName, const PString& targetName, int locale, const char* myUserAlias, bool admin, PString &eventHistoryStr, int& powerIdAffectingBoard )
{
	bool dummy; // This won't be used by dealer chat.
	formatPowerEvent2( isServer, hand, historyItem, playerName, targetName, locale, myUserAlias, admin, eventHistoryStr, dummy, powerIdAffectingBoard );
}


static void _invokeSecretPower(bool isServer, const PString& playerName, const char* myUserAlias, bool admin, int locale, int secretPowerId, const PString& powerCostCurrency, const PString& cost, PString &eventHistoryStr)
{
	bool const doShowPrivateInfo = showPrivateInfo(admin || playerName.equals( myUserAlias ), secretPowerId != 0 && secretPowerId != PokerGamePowerSecretPlaceholder, isServer );
	PString powerNameStr;

	if (doShowPrivateInfo)
	{
		PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "%0 plays a Secret Power: %1 (%2%3 cost)"
			, playerName
			, _powerName(secretPowerId, locale, powerNameStr)
			, cost
			, powerCostCurrency );
	}
	else
	{
		// a case of instant history when the played power id is still a secret
		PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "%0 plays a Secret Power (%1%2 cost)"
			, playerName
			, cost
			, powerCostCurrency );
	}
}


void formatPowerEvent2( bool isServer, const Hand& hand, const TableHistoryItem& historyItem, const PString& playerName, const PString& targetName, int locale, const char* myUserAlias, bool admin, PString &eventHistoryStr, bool &hasStretchRound, int& powerIdAffectingBoard )
{
	// PYR-36799: Determine whether to use "chips" or not for cost.
	PString powerCostCurrency;
	PString powerNameStr;
	PString powerNameStr2;

	if( !hand.isManaCost() )
	{
		PMsg( powerCostCurrency, LocalesTable, locale, P_TXT( TXTHHY_X040chips ) );
	}

	// Storm codes are stored in the amount field.
	switch (historyItem.amount)
	{
	case PokerGamePowerStealth:
		{
			PString cost;
			StormHHThin::invoke::BaseData data;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, data);
			cost.appendInt64(data.header.cost);

			PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "%0 plays %1 (%2%3) and is now the last player to act on each street this hand." // I18NTAG <player> plays <power> (X chips) - or  <player> plays <power> (X)
				, playerName
				, _powerName(historyItem.amount, locale, powerNameStr)
				, cost
				, powerCostCurrency );
		}
		break;

	case PokerGamePowerDisarm:
		{
			PString cost;
			StormHHThin::invoke::BaseData data;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, data);
			cost.appendInt64(data.header.cost);

			PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "%0 plays %1 (%2%3); all players redraw one random hole card."
				, playerName
				, _powerName(historyItem.amount, locale, powerNameStr)
				, cost
				, powerCostCurrency );
		}
		break;

	case PokerGamePowerDetect:
		{
			PString cost;
			StormHHThin::invoke::BaseData data;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, data);
			cost.appendInt64(data.header.cost);

			if(historyItem.action >= 0)
			{
				PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "%0 plays %1 (%2%3) on %4"
					, playerName
					, _powerName(historyItem.amount, locale, powerNameStr)
					, cost
					, powerCostCurrency
					, targetName );
			}
			else
			{
				PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "%0 plays %1 (%2%3)"
					, playerName
					, _powerName(historyItem.amount, locale, powerNameStr)
					, cost
					, powerCostCurrency );
			}
		}
		break;

	case PokerGamePowerXRay:
		{
			PString cost; //-V1037
			StormHHThin::invoke::BaseData data;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, data);
			cost.appendInt64(data.header.cost);

			PMsg4( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_plays_N1_XN2N3XX )
				, playerName
				, _powerName(historyItem.amount, locale, powerNameStr)
				, cost
				, powerCostCurrency );
		}
		break;

	case PokerGamePowerPacify:
		{
			bool cardUnfreezePossible = false;
			PString cost;

			StormHHThin::invoke::Pacify leapData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, leapData);
			cost.appendInt64(leapData.header.cost);
			cardUnfreezePossible = leapData.pubData.cardUnfreezePossible;

			PMsg4( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_plays_N1_XN2N3X_returning_a_ELL ),
				playerName, _powerName(historyItem.amount, locale, powerNameStr), cost, powerCostCurrency );

			if (cardUnfreezePossible)
			{
				PMsg( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_X040Xand_unfreezes_the_current_ELL ) );
			}
		}
		break;

	case PokerGamePowerMindControl:
		{
			INT32 stolenPowerId; // private
			vector<INT32> leftPowers; // private
			INT32 leftPowersSize = 0; // private
			PString cost;
			bool hasSecretPart = false;

			StormHHThin::invoke::MindControl mindControlData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, mindControlData);
			cost.appendInt64(mindControlData.header.cost);
			stolenPowerId = mindControlData.privData.stolenPowerId;
			leftPowers.swap(mindControlData.privData.powersLeft);
			leftPowersSize = leftPowers.size();
			hasSecretPart = mindControlData.header.exposePrivacy;

			// Only the player who played the power, the affected player (and admin) may see the specific power information.
			if (showPrivateInfo(admin || playerName.equals( myUserAlias ) || targetName.equals( myUserAlias ), hasSecretPart, isServer))
			{
				if(historyItem.action >= 0)
				{
					PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "%0 plays %1 (%2%3) on %4"
						, playerName
						, _powerName(historyItem.amount, locale, powerNameStr)
						, cost
						, powerCostCurrency
						, targetName );
				}
				else
				{
					PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "%0 plays %1 (%2%3)"
						, playerName
						, _powerName(historyItem.amount, locale, powerNameStr)
						, cost
						, powerCostCurrency );
				}
				// Prepare a string with the powers that were not chosen.
				PString otherPowers;
				for (INT32 i = 0; i < leftPowersSize; ++i)
				{
					if (i != 0)
						otherPowers.append(", ");
					otherPowers.append(_powerName(leftPowers[i], locale, powerNameStr));
				}

				// If the stolen power Id is -1, it means either the player cancelled, or timed out.
				if(stolenPowerId == -1)
					PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "\040but steals no power-up (%0 not selected).", otherPowers);
				else
					if(leftPowersSize > 0)
						PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "\040and steals %0 (%1 not selected).", _powerName(stolenPowerId, locale, powerNameStr), otherPowers);
					else
						PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "\040and steals %0.", _powerName(stolenPowerId, locale, powerNameStr));
			}
			else
			{
				if(historyItem.action >= 0)
				{
					PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "%0 plays %1 (%2%3) on %4 and steals a power-up."
						, playerName
						, _powerName(historyItem.amount, locale, powerNameStr)
						, cost
						, powerCostCurrency
						, targetName );
				}
				else
				{
					PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "%0 plays %1 (%2%3) and steals a power-up."
						, playerName
						, _powerName(historyItem.amount, locale, powerNameStr)
						, cost
						, powerCostCurrency );
				}
			}
		}
		break;

	case PokerGamePowerParasite:
		{
			PString cost;
			StormHHThin::invoke::BaseData data;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, data);
			cost.appendInt64(data.header.cost);

			PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "%0 plays %1 (%2%3); a portion of this pot will be pushed to the next hand."
				, playerName
				, _powerName(historyItem.amount, locale, powerNameStr)
				, cost
				, powerCostCurrency );
		}
		break;

	case PokerGamePowerUpgrade:
		{
			PString cost;
			StormHHThin::invoke::BaseData data;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, data);
			cost.appendInt64(data.header.cost);

			PMsg4( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_plays_N1_XN2N3XX )
				, playerName
				, _powerName(historyItem.amount, locale, powerNameStr)
				, cost
				, powerCostCurrency );
		}
		break;

	case PokerGamePowerReload:
		{
			PString cost;
			StormHHThin::invoke::Reload reloadData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, reloadData);
			cost.appendInt64(reloadData.header.cost);

			if(reloadData.pubData.numCards == 1)
			{
				PMsg4( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_plays_N1_XN2N3X_and_redraws_ELL )
					, playerName
					, _powerName(historyItem.amount, locale, powerNameStr)
					, cost
					, powerCostCurrency );
			}
			else
			{
				PString numCards;
				numCards.appendInt(reloadData.pubData.numCards);
				PMsg5( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_plays_N1_XN2N3X_and_redraws_ELL_1 )
					, playerName
					, _powerName(historyItem.amount, locale, powerNameStr)
					, cost
					, powerCostCurrency
					, numCards );
			}
		}
		break;

	case PokerGamePowerRunItTwice:
		{
			PString cost;
			StormHHThin::invoke::BaseData data;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, data);
			cost.appendInt64(data.header.cost);
			
			PMsg4( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_plays_N1_XN2N3XX_deals_a_se_ELL )
				, playerName
				, _powerName(historyItem.amount, locale, powerNameStr)
				, cost
				, powerCostCurrency );

			// Set as the power causing the next dealing.
			powerIdAffectingBoard = PokerGamePowerRunItTwice;
		}
		break;

	case PokerGamePowerIntel:
		{
			PString cost;
			bool hasSecretPart = false;

			StormHHThin::invoke::Intel senseData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, senseData);
			cost.appendInt64(senseData.header.cost);
			hasSecretPart = senseData.header.exposePrivacy;

			PASSERT(senseData.privData.sensedCards.length() % 2 == 0);

			PString numSensedCardsString;
			numSensedCardsString.appendInt(senseData.pubData.numOfSensedNextCards);

			// Only the player who played the power (and admin) may see the specific power information.
			if (showPrivateInfo(admin || playerName.equals( myUserAlias ), hasSecretPart, isServer))
			{
				// Assemble the cards string.
				PString cardsString;
				insertCommaInCardsString(senseData.privData.sensedCards, cardsString);

				PMsg5( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_plays_N1_XN2N3X_and_peeks_a_ELL )
					, playerName
					, _powerName(historyItem.amount, locale, powerNameStr)
					, cost
					, powerCostCurrency
					, cardsString );
			}
			else
			{
				if( senseData.pubData.numOfSensedNextCards == 1)
				{
					PMsg4( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_plays_N1_XN2N3X_and_peeks_a_ELL_1 )
					, playerName
					, _powerName(historyItem.amount, locale, powerNameStr)
					, cost
					, powerCostCurrency );
				}
				else
				{
					PMsg5( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_plays_N1_XN2N3X_and_peeks_a_ELL_2 )
						, playerName
						, _powerName(historyItem.amount, locale, powerNameStr)
						, cost
						, powerCostCurrency
						, numSensedCardsString );
				}
			}
		}
		break;

	case PokerGamePowerMorph:
		{
			PString cost;
			StormHHThin::invoke::BaseData data;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, data);
			cost.appendInt64(data.header.cost);

			PMsg4( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_plays_N1_XN2N3X_and_alters__ELL )
				, playerName
				, _powerName(historyItem.amount, locale, powerNameStr)
				, cost
				, powerCostCurrency );
		}
		break;

	case PokerGamePowerZombie:
		{
			PString cost;
			StormHHThin::invoke::BaseData data;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, data);
			cost.appendInt64(data.header.cost);

			PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "%0 plays %1 (%2%3) and returns to the hand."
				, playerName
				, _powerName(historyItem.amount, locale, powerNameStr)
				, cost
				, powerCostCurrency );
		}
		break;

	case PokerGamePowerLeeroyJenkins:
		{
			PString cost;
			StormHHThin::invoke::BaseData data;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, data);
			cost.appendInt64(data.header.cost);

			PMsg4( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_plays_N1_XN2N3X ),
				playerName, _powerName(historyItem.amount, locale, powerNameStr), cost, powerCostCurrency );
		}
		break;

	case PokerGamePowerClone:
		{
			PString cost;
			INT32 publicClonedPowerId;
			INT32 privateClonedPowerId;
			bool hasSecretePart = false;

			StormHHThin::invoke::Clone cloneData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, cloneData);
			cost.appendInt64(cloneData.header.cost);
			publicClonedPowerId = cloneData.pubData.powerIdToPlay;
			privateClonedPowerId = cloneData.privData.powerIdToPlay;
			hasSecretePart = cloneData.header.exposePrivacy;

			int finalClonedPowerId = -1;

			if (publicClonedPowerId != PokerGamePowerSecretPlaceholder)
			{
				finalClonedPowerId = publicClonedPowerId;
			}
			else if (showPrivateInfo(admin || playerName.equals( myUserAlias ) || targetName.equals( myUserAlias ), hasSecretePart, isServer))
			{
				finalClonedPowerId = privateClonedPowerId;
			}

			if (finalClonedPowerId != -1)
			{
				PMsg5( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_plays_N1_XN2N3X_and_clones__ELL_3 )
					, playerName
					, _powerName(historyItem.amount, locale, powerNameStr)
					, cost
					, powerCostCurrency
					, _powerName(finalClonedPowerId, locale, powerNameStr2) );
			}
			else
			{
				PMsg4( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_plays_N1_XN2N3X_and_clones__ELL_1 )
					, playerName
					, _powerName(historyItem.amount, locale, powerNameStr)
					, cost
					, powerCostCurrency );
			}
		}
		break;

	case PokerGamePowerChaos:
		{
			PString numBombs, cost;
			StormHHThin::invoke::Chaos chaosData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, chaosData);
			cost.appendInt64(chaosData.header.cost);
			numBombs.appendInt(chaosData.pubData.numBombs);

			PMsg5( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_plays_N1_XN2N3X_redrawing_N_ELL )
				, playerName
				, _powerName(historyItem.amount, locale, powerNameStr)
				, cost
				, powerCostCurrency
				, numBombs );
		}
		break;

	case PokerGamePowerTeleport:
		{
			PString cost;
			StormHHThin::invoke::BaseData data;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, data);
			cost.appendInt64(data.header.cost);

			PMsg4( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_plays_N1_XN2N3X ),
				playerName, _powerName(historyItem.amount, locale, powerNameStr), cost, powerCostCurrency );
		}
		break;

	case PokerGamePowerMint:
		{
			PString cost, mintChipsStr;
			StormHHThin::invoke::Mint mintData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, mintData);
			cost.appendInt64(mintData.header.cost);
			mintChipsStr.appendInt64(mintData.pubData.chipsMinted);

			PMsg4( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_plays_N1_XN2N3XX ),
				playerName, _powerName(historyItem.amount, locale, powerNameStr), cost, powerCostCurrency );

			i18nAppendChar( eventHistoryStr, ' ' );

			PMsg1( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_chips_have_been_minted_and__ELL ), mintChipsStr );
		}
		break;

	case PokerGamePowerSwap:
		{
			PString holeCard, boardCard, cost;

			StormHHThin::invoke::Swap swapData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, swapData);
			cost.appendInt64(swapData.header.cost);
			holeCard.moveFrom(swapData.pubData.holeCard);
			boardCard.moveFrom(swapData.pubData.boardCard);

			PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "%0 plays %1 (%2%3); and swaps a hole card with a board card.\040"
				, playerName
				, _powerName(historyItem.amount, locale, powerNameStr)
				, cost
				, powerCostCurrency	);
			PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "%0 took the %1 and placed %2 on the board."
				, playerName
				, boardCard
				, holeCard	);
		}
		break;

	case PokerGamePowerDisintegrate:
		{
			PString cost;
			StormHHThin::invoke::BaseData data;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, data);
			cost.appendInt64(data.header.cost);

			PMsg4( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_plays_N1_XN2N3XX )
				, playerName
				, _powerName(historyItem.amount, locale, powerNameStr)
				, cost
				, powerCostCurrency );
		}
		break;

	case PokerGamePowerEngineer:
		{
			PString cost;
			StormHHThin::invoke::Engineer engineerData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, engineerData);
			cost.appendInt64(engineerData.header.cost);

			PString discardedCardsList;
			insertCommaInCardsString(engineerData.pubData.discardedCards, discardedCardsList);

			PString totalCardsList;
			if(engineerData.pubData.cardsCandidate.length() == 0)  // old hands, we need to compose a string for total card list
			{
				totalCardsList.append(engineerData.pubData.chosenCard).append(", ").append(discardedCardsList);
			}
			else  // new hands
			{
				insertCommaInCardsString(engineerData.pubData.cardsCandidate, totalCardsList);
			}

			PMsg5( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_plays_N1_XN2N3X_revealing_N_ELL )
				, playerName
				, _powerName(historyItem.amount, locale, powerNameStr)
				, cost
				, powerCostCurrency
				, totalCardsList );
			i18nAppendChar( eventHistoryStr, '\n' );
			PMsg3( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_selects_N1_and_discards_N2 )
				, playerName
				, engineerData.pubData.chosenCard 
				, discardedCardsList);
		}
		break;

	case PokerGamePowerScanner:
		{
			PString numberOfCards;
			PString cost;
			bool hasSecretePart = false;

			StormHHThin::invoke::Scanner changeFutureData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, changeFutureData);
			cost.appendInt64(changeFutureData.header.cost);
			numberOfCards.appendInt(changeFutureData.pubData.numCards);
			hasSecretePart = changeFutureData.header.exposePrivacy;

			PMsg5( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_plays_N1_XN2N3X_seeing_the__ELL )
				, playerName
				, _powerName(historyItem.amount, locale, powerNameStr)
				, cost
				, powerCostCurrency
				, numberOfCards );

			i18nAppendChar( eventHistoryStr, '\n' );

			// Only the player who played the power (and admin) may see the specific power information.
			if (showPrivateInfo(admin || playerName.equals( myUserAlias ), hasSecretePart, isServer))
			{
				// We need to list the cards.
				PString cardsList;
				insertCommaInCardsString(changeFutureData.privData.cards, cardsList);

				if(changeFutureData.pubData.keepOrDiscard.equals("k"))  // keep
					PMsg3( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_keeps_the_next_N1_cards_on__ELL )
					, playerName
					, numberOfCards
					, cardsList );
				else  // discard
					PMsg3( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_discards_the_next_N1_cards__ELL )
					, playerName
					, numberOfCards
					, cardsList );
			}
			else
			{
				if(changeFutureData.pubData.keepOrDiscard.equals("k"))  // keep
					PMsg2( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_keeps_the_next_N1_cards_on__ELL_1 )
					, playerName
					, numberOfCards );
				else  // discard
					PMsg2( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_discards_the_next_N1_cards__ELL_1 )
					, playerName
					, numberOfCards );
			}
		}
		break;

	case PokerGamePowerReboot:
		{
			PString cardsNumber;
			PString cost;

			StormHHThin::invoke::Reboot rebootData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, rebootData);
			cost.appendInt64(rebootData.header.cost);
			cardsNumber.appendInt(rebootData.pubData.numPowersDrawn);

			PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "%0 plays %1; discards all Power Cards and draws %2 Power Cards",
				playerName, _powerName(historyItem.amount, locale, powerNameStr), cardsNumber);
		}
		break;

	case PokerGamePowerDeploy:
		{
			PString cost;
			StormHHThin::invoke::BaseData data;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, data);
			cost.appendInt64(data.header.cost);

			PMsg4( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_plays_N1_XN2N3X ),
				playerName, _powerName(historyItem.amount, locale, powerNameStr), cost, powerCostCurrency );
		}
		break;

	case PokerGamePowerEqualizer:
		{
			PString manaCopied;
			PString cost;

			StormHHThin::invoke::Equalizer EqualizerData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, EqualizerData);
			cost.appendInt64(EqualizerData.header.cost);
			manaCopied.appendInt64(EqualizerData.pubData.manaCopied);

			PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "%0 has used %1 (%2%3) and clones the energy total of a random opponent (%4)",
				               playerName, _powerName(historyItem.amount, locale, powerNameStr), cost, powerCostCurrency, manaCopied );
		}
		break;

	case PokerGamePowerDarkBet:
		{
			PString amount; // private
			PString cost;
			bool hasSecretePart = false;

			StormHHThin::invoke::DarkBet darkBetData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, darkBetData);
			cost.appendInt64(darkBetData.header.cost);
			amount.appendInt64(darkBetData.privData.amount);
			hasSecretePart = darkBetData.header.exposePrivacy;

			if (showPrivateInfo(admin || playerName.equals( myUserAlias ), hasSecretePart, isServer))
			{
				PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "%0 plays %1 (%2%3) and bets %4 chips.",
					playerName, _powerName(historyItem.amount, locale, powerNameStr), cost, powerCostCurrency, amount);
			}
			else
			{
				PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "%0 plays %1 (%2%3) and bets a hidden amount.",
					playerName, _powerName(historyItem.amount, locale, powerNameStr), cost, powerCostCurrency );
			}
		}
		break;

	case PokerGamePowerDummySpecimen:
		{
			/*
			// As an example, we show how to parse the 'contents'. You do not need it if (by default) it contains only the cost
			const char* split = historyItem.contents.find('|');
			if (!split)
			{
				PLog( "formatHistory.cpp::ERROR:DummySpecimen missing splitter. Contents %s - ignored", historyItem.contents.c_str() );
				break;
			}
			PString dummyParam(historyItem.contents, split);
			PString cost(split+1);
			PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "%0 plays %1 (%2%3) dummyParam=%4."
				, playerName
				, _powerName(historyItem.amount, locale, powerNameStr)
				, cost
				, powerCostCurrency
				, dummyParam );
			*/
		}
		break;

	case PokerGamePowerFreeze:
		{
			PString frozenCards, cost;

			StormHHThin::invoke::Freeze freezeData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, freezeData);
			cost.appendInt64(freezeData.header.cost);
			frozenCards.moveFrom(freezeData.pubData.frozenCards);

			PASSERT(frozenCards.length()%2==0);
			int cardsSize = frozenCards.length()/2;

			PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "%0 plays %1 (%2%3); existing board cards cannot be changed by powers: "
				, playerName
				, _powerName(historyItem.amount, locale, powerNameStr)
				, cost
				, powerCostCurrency );

			for (int i=0; i<cardsSize;++i)
			{
				if (i!=0)
					eventHistoryStr.append(", ");
				eventHistoryStr.append(frozenCards[2*i]).append(frozenCards[2*i+1]);
			}
		}
		break;

	case PokerGamePowerEMP:
		{
			PString cost;
			StormHHThin::invoke::BaseData data;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, data);
			cost.appendInt64(data.header.cost);

			PMsg4( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_plays_N1_XN2N3X )
				, playerName
				, _powerName(historyItem.amount, locale, powerNameStr)
				, cost
				, powerCostCurrency );
		}
		break;

	case PokerGamePowerSyphon:
		{
			PString stolenMana, manaReceived, cost;
			StormHHThin::invoke::Syphon manaDrainData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, manaDrainData);

			cost.appendInt64(manaDrainData.header.cost);
			stolenMana.appendInt64(manaDrainData.pubData.manaStolen);
			if(manaDrainData.pubData.manaReceived == -1)  // old hand
			{
				manaReceived.appendInt64(manaDrainData.pubData.manaStolen);
			}
			else
			{
				manaReceived.appendInt64(manaDrainData.pubData.manaReceived);
			}

			PString powerNameCostString = _powerName(historyItem.amount, locale, powerNameStr);
			i18nAppend( powerNameCostString, " (" );
			i18nAppend( powerNameCostString, cost );
			if(*powerCostCurrency)
			{
				i18nAppend( powerNameCostString, powerCostCurrency );
			}
			i18nAppend( powerNameCostString, ")" );

			PMsg5( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_plays_N1_on_N2_removing_N3__ELL )
				, playerName
				, powerNameCostString
				, targetName
				, stolenMana
				, manaReceived );
		}
		break;

	case PokerGamePowerExtraLife:
		{
			PString cost;
			StormHHThin::invoke::BaseData data;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, data);
			cost.appendInt64(data.header.cost);

			PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "%0 plays %1 (%2%3) and goes All-In. If %0 loses the pot, a portion of the bet will be refunded."
				, playerName
				, _powerName(historyItem.amount, locale, powerNameStr)
				, cost
				, powerCostCurrency );
		}
		break;

	case PokerGamePowerAnteUp:
		{
			PString strAnte;
			StormHHThin::invoke::AnteUp anteUpData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, anteUpData);
			//cost.appendInt64(anteUpData.header.cost);
			strAnte.appendInt64(anteUpData.pubData.ante);

			PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "Player %0 used Ante Up. All active opponents must put %1 chips into the pot or fold", playerName, strAnte );
		}
		break;

	case PokerGamePowerSecretPlaceholder: // Any secret power is both played and revealed with this powerId.
		{
			// new format: only secret power invocation gets here
			PString cost;
			StormHHThin::invoke::BaseData data;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, data);
			cost.appendInt64(data.header.cost);

			_invokeSecretPower(isServer, playerName, myUserAlias, admin, locale, PokerGamePowerSecretPlaceholder, powerCostCurrency, cost, eventHistoryStr);
		}
		break;

	// =============================== secret powers begin ==================================

	case PokerGamePowerStretch:
		{
			PString cost;
			StormHHThin::invoke::Stretch stretchData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, stretchData);
			cost.appendInt64(stretchData.header.cost);

			if(stretchData.pubData.isRevealStage)
			{
				hasStretchRound = true;

				if(stretchData.pubData.isSecretPlay)
				{
					PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "%0's secret has been revealed as %1"
						, playerName
						, _powerName(stretchData.privData.mySecretPowerId, locale, powerNameStr) );
				}
				else
				{
					PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "%0 plays %1 (%2%3); an additional street is added to the board."
						, playerName
						, _powerName(historyItem.amount, locale, powerNameStr)
						, cost
						, powerCostCurrency );
				}
			}
			else
			{
				_invokeSecretPower(isServer, playerName, myUserAlias, admin, locale, stretchData.privData.mySecretPowerId, powerCostCurrency, cost, eventHistoryStr);
			}
		}
		break;

	case PokerGamePowerLowBlow:
		{
			PString cost;
			StormHHThin::invoke::Lowblow lowblowData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, lowblowData);
			cost.appendInt64(lowblowData.header.cost);

			if(lowblowData.pubData.isRevealStage)
			{
				if(lowblowData.pubData.isSecretPlay)
				{
					PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "%0's secret has been revealed as %1"
						, playerName
						, _powerName(lowblowData.privData.mySecretPowerId, locale, powerNameStr) );
				}
				else
				{
					PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "%0 plays %1 (%2%3); the next two board cards higher than 9 will be redrawn."
						, playerName
						, _powerName(historyItem.amount, locale, powerNameStr)
						, cost
						, powerCostCurrency );
				}
			}
			else
			{
				_invokeSecretPower(isServer, playerName, myUserAlias, admin, locale, lowblowData.privData.mySecretPowerId, powerCostCurrency, cost, eventHistoryStr);
			}
		}
		break;

	case PokerGamePowerSuitUp:
		{
			PString cost;
			StormHHThin::invoke::SuitUp suitUpData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, suitUpData);
			cost.appendInt64(suitUpData.header.cost);

			if(suitUpData.pubData.isRevealStage)
			{
				PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "%0's secret has been revealed as %1 for %2"
					, playerName
					, _powerName(suitUpData.privData.mySecretPowerId, locale, powerNameStr)
					, suitName(suitUpData.privData.suit, locale));
			}
			else
			{
				bool const doShowPrivateInfo = showPrivateInfo(admin || playerName.equals( myUserAlias ), suitUpData.header.exposePrivacy, isServer );

				if (doShowPrivateInfo)
				{
					PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "%0 plays a Secret Power: %1 for %2 (%3%4 cost)"
						, playerName
						, _powerName(suitUpData.privData.mySecretPowerId, locale, powerNameStr)
						, suitName(suitUpData.privData.suit, locale)
						, cost
						, powerCostCurrency );
				}
				else
				{
					// a case of instant history when the played power id is still a secret
					PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "%0 plays a Secret Power (%1%2 cost)"
						, playerName
						, cost
						, powerCostCurrency );
				}
			}
		}
		break;

	case PokerGamePowerSplashPot:
		{
			PString cost, splashSize;
			StormHHThin::invoke::SplashPot splashPotData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, splashPotData);
			cost.appendInt64(splashPotData.header.cost);
			splashSize.appendInt64(splashPotData.privData.splashSize);

			if(splashPotData.pubData.isRevealStage)
			{
				PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "%0's secret has been revealed as %1 and %2 chips have been added to the main pot."
					, playerName
					, _powerName(splashPotData.privData.mySecretPowerId, locale, powerNameStr)
					, splashSize );
			}
			else
			{
				_invokeSecretPower(isServer, playerName, myUserAlias, admin, locale, splashPotData.privData.mySecretPowerId, powerCostCurrency, cost, eventHistoryStr);
			}
		}
		break;

	case PokerGamePowerSabotage:
		{
			PString cost, hit;
			StormHHThin::invoke::Sabotage sabotageData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, sabotageData);
			cost.appendInt64(sabotageData.header.cost);
			hit.appendInt64(sabotageData.privData.hitPayment);

			if(sabotageData.pubData.isRevealStage)
			{
				if(sabotageData.privData.isManaHit)
				{
					PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "%0's secret has been revealed as %1. %2 is destroyed and %3 incurs %4 energy hit",
						playerName, _powerName(sabotageData.privData.mySecretPowerId, locale, powerNameStr),
						_powerName(sabotageData.privData.destroyedPowerId, locale, powerNameStr2), targetName, hit);
				}
				else
				{
					PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "%0's secret has been revealed as %1. %2 is destroyed and %3 incurs %4 chip hit",
						playerName, _powerName(sabotageData.privData.mySecretPowerId, locale, powerNameStr),
						_powerName(sabotageData.privData.destroyedPowerId, locale, powerNameStr2), targetName, hit);
				}
			}
			else
			{
				_invokeSecretPower(isServer, playerName, myUserAlias, admin, locale, sabotageData.privData.mySecretPowerId, powerCostCurrency, cost, eventHistoryStr);
			}
		}
		break;

	case PokerGamePowerCounterSpell:
		{
			PString cost;
			StormHHThin::invoke::CounterSpell counterSpellData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, counterSpellData);
			cost.appendInt64(counterSpellData.header.cost);

			if(counterSpellData.pubData.isRevealStage)
			{
				PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "%0's secret has been revealed as %1. %2 plays %3, which is nullified by %1.",
					playerName,
					_powerName(counterSpellData.privData.mySecretPowerId, locale, powerNameStr),
					targetName,
					_powerName(counterSpellData.privData.destroyedPowerId, locale, powerNameStr2));
			}
			else
			{
				_invokeSecretPower(isServer, playerName, myUserAlias, admin, locale, counterSpellData.privData.mySecretPowerId, powerCostCurrency, cost, eventHistoryStr);
			}
		}
		break;

	// =============================== secret powers end ==================================

	default:
		PLog( "formatHistory.cpp::ERROR:Unknown Power Code %03X - ignored", historyItem.amount );
		break;
	}
}

/*---------------------------------------------------------------------------------------------
formatPowerEffect
---------------------------------------------------------------------------------------------*/
void formatPowerEffect( bool isServer, const Hand& hand, const TableHistoryItem& historyItem, const PString& playerName, int locale, const char* myUserAlias, PString * boardCardsPstr, bool runItTwice, bool admin, PString &eventHistoryStr, int& powerIdAffectingBoard )
{
	PString powerNameStr;
	
	// Storm codes are stored in the amount field.
	switch (historyItem.amount)
	{
	case PokerGamePowerEffectReplaceHoleCards:
		{
			PString outCard, inCard; // private
			INT32 powerId;
			bool hasPrivateData = false;

			StormHHThin::effect::ReplaceHoleCard replaceHoleCardData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, replaceHoleCardData);
			powerId = replaceHoleCardData.header.powerId;
			outCard.moveFrom(replaceHoleCardData.privData.oldCard);
			inCard.moveFrom(replaceHoleCardData.privData.newCard);
			hasPrivateData = replaceHoleCardData.header.exposePrivacy;

			switch(powerId)
			{
			case PokerGamePowerSwap:
				// Do nothing, this has been reported by the PokerGamePowerSwap event.
				break;
			case PokerGamePowerReload:
			case PokerGamePowerDisarm:
				// Only the player who played the power (and admin) may see the card information.
				if (showPrivateInfo(admin || playerName.equals( myUserAlias ), hasPrivateData, isServer))
					PMsg3( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_receives_N1_and_discards_N2 )
					, playerName
					, inCard
					, outCard );
				break;
			case PokerGamePowerUpgrade:
				// Only the affected player (and admin) may see the card information.
				if (showPrivateInfo(admin || playerName.equals( myUserAlias ), hasPrivateData, isServer))
					PMsg3( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_receives_the_N1_and_discard_ELL )
					, playerName
					, inCard
					, outCard );
				break;
			default:
				break;
			}
		}
		break;

	case PokerGamePowerEffectReplaceBoardCard:
	case PokerGamePowerEffectReplaceMultiBoardCards:
		{
			// The public data we have is the cards.
			INT32 powerId = 0;
			BYTE suit = 0;  // for suit up only
			vector<PString> outCards;
			vector<PString> inCards;

			if(historyItem.amount == PokerGamePowerEffectReplaceBoardCard)
			{
				StormHHThin::effect::ReplaceBoardCard replaceBoardCardData;
				decodeDataStructFromHandHistoryContentString(historyItem.contents, replaceBoardCardData);
				powerId = replaceBoardCardData.header.powerId;
				outCards.push_back(replaceBoardCardData.pubData.oldCard);
				inCards.push_back(replaceBoardCardData.pubData.newCard);
				suit = replaceBoardCardData.pubData.suit;
			}
			else
			{
				StormHHThin::effect::ReplaceMultiBoardCards replaceMultiBoardCardsData;
				decodeDataStructFromHandHistoryContentString(historyItem.contents, replaceMultiBoardCardsData);
				powerId = replaceMultiBoardCardsData.header.powerId;
				outCards = std::move(replaceMultiBoardCardsData.pubData.oldCards);
				inCards = std::move(replaceMultiBoardCardsData.pubData.newCards);
			}

			int sz = min(outCards.size(), inCards.size());
			if(sz == 0)
			{
				PASSERT5(0);  
				break;  // do nothing!
			}

			for(int i = 0; i < sz; ++i)
			{
				PString& inCard = inCards[i];
				PString& outCard = outCards[i];

				if(inCard.length() == 0)
				{
					inCard = "--";   // unfortunately the old code may cause the empty string from Disintegrate power effect
				}

				// Replace the card on the affected board(s). If it was dealt in any round before Run It Twice was played, the card will belong to both boards.
				const char* cardToRemove = boardCardsPstr[0].find(outCard);
				if(cardToRemove)
				{
					PString newBoard;
					int removePosition = cardToRemove - boardCardsPstr[0].c_str();
					for(int charPos = 0; charPos < boardCardsPstr[0].length(); charPos++)
					{
						if(charPos < removePosition || charPos > removePosition + 1)
						{
							newBoard.append(boardCardsPstr[0].charAt(charPos));
						}
						else if (charPos == removePosition)
						{
							newBoard.append(inCard.charAt(0));
						} 
						else
						{
							newBoard.append(inCard.charAt(1));
						}
					}

					boardCardsPstr[0] = newBoard;
				}
				if(runItTwice)
				{
					cardToRemove = boardCardsPstr[1].find(outCard);
					if(cardToRemove)
					{
						PString newBoard;
						int removePosition = cardToRemove - boardCardsPstr[1].c_str();
						for(int charPos = 0; charPos < boardCardsPstr[1].length(); charPos++)
						{
							if(charPos < removePosition || charPos > removePosition + 1)
							{
								newBoard.append(boardCardsPstr[1].charAt(charPos));
							}
							else if (charPos == removePosition)
							{
								newBoard.append(inCard.charAt(0));
							} 
							else
							{
								newBoard.append(inCard.charAt(1));
							}
						}

						boardCardsPstr[1] = newBoard;
					}
				}
			}

			// Finally, append the string to the history.
			switch(powerId)
			{
			case PokerGamePowerSwap:
				// Do nothing, this has been reported by the PokerGamePowerSwap event.
				break;
			case PokerGamePowerDisintegrate:
				PMsg2( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_destroys_N1X )
					, _powerName(powerId, locale, powerNameStr)
					, outCards[0] );
				break;
			case PokerGamePowerMorph:
				PMsg2( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_Replacing_N0_with_N1X )
					, outCards[0]
					, inCards[0] );
				break;
			case PokerGamePowerChaos:
				{
					for(int i = 0; i < sz; ++i)
					{
						PMsg2( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_redraws_to_N1 ), outCards[i], inCards[i] );
					}
				}
				break;
			case PokerGamePowerLowBlow:
				{
					PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "%0's %1 redraws the %2. The new card is %3."
						, playerName
						, _powerName(powerId, locale, powerNameStr)
						, outCards[0]
						, inCards[0] );
				}
				break;
			case PokerGamePowerSuitUp:
				{
					PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "%0's %1 for %2 redraws the %3. The new card is %4."
						, playerName
						, _powerName(powerId, locale, powerNameStr)
						, suitName(suit, locale)
						, outCards[0]
						, inCards[0] );
				}
				break;
			default:
				PMsg3( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_replaces_N1_with_N2X )
					, _powerName(powerId, locale, powerNameStr)
					, outCards[0]
					, inCards[0] );
				break;
			}

			// PYR-40943 - Show full board when a power changes it.
			if(powerId != PokerGamePowerSwap)
			{
				// We need a break line. The only exception is Swap, that doesn't print anything here, which would leave us with a blank line.
				i18nAppendChar( eventHistoryStr, '\n' );
			}
			//PString uppercasePowerName = _powerName(powerId, locale, powerNameStr);
			//uppercasePowerName.toUpper();

			//PMsg1( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_XX_N0_XX ), uppercasePowerName );
			PString newBoardStr;
			PMsg( newBoardStr, LocalesTable, locale, P_TXT( TXTHHY_NEW_BOARD ) );
			PMsg1( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_XX_N0_XX ), newBoardStr );

			// Build the board strings with spaces between cards, and using brackets.
			i18nAppend( eventHistoryStr, " [" );
			const char* str_p = boardCardsPstr[0];
			while( *str_p )
			{
				i18nAppendChar( eventHistoryStr, *str_p );

				if( *( ++str_p ) )
					i18nAppendChar( eventHistoryStr, *(str_p++) );
				else break;

				if( *str_p )
					PMsg( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_X040 ) );
			}
			i18nAppendChar( eventHistoryStr, ']' );
			if(runItTwice)
			{
				i18nAppend( eventHistoryStr, ", [" );
				const char* str_p = boardCardsPstr[1];
				while( *str_p )
				{
					i18nAppendChar( eventHistoryStr, *str_p );

					if( *( ++str_p ) )
						i18nAppendChar( eventHistoryStr, *(str_p++) );
					else break;

					if( *str_p )
						PMsg( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_X040 ) );
				}
				i18nAppendChar( eventHistoryStr, ']' );
			}
		}
		break;

	case PokerGamePowerEffectDiscardHoleCards:
		{
			PString cards;
			int powerId;
			bool hasPrivateData = false;

			StormHHThin::effect::DiscardHoleCards discardHoleCardData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, discardHoleCardData);
			powerId = discardHoleCardData.header.powerId;
			cards.moveFrom(discardHoleCardData.privData.cards);
			hasPrivateData = discardHoleCardData.header.exposePrivacy;

			PASSERT(cards.length()%2==0);
			int cardsSize = cards.length()/2;

			switch(powerId)
			{
			case PokerGamePowerUpgrade:
				if(showPrivateInfo(admin || playerName.equals( myUserAlias ), hasPrivateData, isServer))
				{
					PString cardsStr;
					for (int i = 0; i < cardsSize; ++i)
					{
						if (i != 0)
							cardsStr.append(", ");
						cardsStr.append(cards[2*i]).append(cards[2*i+1]);
					}

					PMsg2( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_discards_N1X ), playerName, cardsStr );
				}
				break;
			}
		}
		break;

	case PokerGamePowerEffectReceiveHoleCards:
		{
			PString cards; // private
			int powerId;
			bool hasPrivateData = false;

			StormHHThin::effect::ReceiveHoleCards receiveHoleCardData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, receiveHoleCardData);
			powerId = receiveHoleCardData.header.powerId;
			cards.moveFrom(receiveHoleCardData.privData.cards);
			hasPrivateData = receiveHoleCardData.header.exposePrivacy;

			PASSERT(cards.length()%2==0);
			int cardsSize = cards.length()/2;

			switch(powerId)
			{
			case PokerGamePowerUpgrade:
				// Only the affected player (and admin) may see the card information.
				if (showPrivateInfo(admin || playerName.equals( myUserAlias ), hasPrivateData, isServer))
				{
					PString cardsStr;
					for (int i = 0; i < cardsSize; ++i)
					{
						if (i != 0)
							cardsStr.append(", ");
						cardsStr.append(cards[2*i]).append(cards[2*i+1]);
					}

					PMsg2( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_receives_N1X ), playerName, cardsStr );
				}
				break;
			}
		}
		break;

	case PokerGamePowerEffectShowOneHoleCard:
		{
			// We should have just one card, followed by the powerId.
			if(historyItem.contents.length() < 3) // We expect <card><power id> (for example, As5)
				break;

			// First retrieve the card in the event string.
			PString shownCard;
			int powerId;

			StormHHThin::effect::ShowOneHoleCard showOneCardData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, showOneCardData);
			powerId = showOneCardData.header.powerId;
			shownCard.moveFrom(showOneCardData.pubData.card);

			switch(powerId)
			{
			case PokerGamePowerXRay:
				PMsg2( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_shows_N1 )
					, historyItem.userId
					, shownCard );
				break;
			}
		}
		break;

	case PokerGamePowerEffectFrozenCardHit:
		{
			PASSERT(historyItem.contents.length() > 3);

			// First retrieve the card in the event string.
			PString frozenCard;
			int powerId;

			StormHHThin::effect::FrozenCardHit frozenCardHitData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, frozenCardHitData);
			powerId = frozenCardHitData.header.powerId;
			frozenCard.moveFrom(frozenCardHitData.pubData.card);

			switch(powerId)
			{
			case PokerGamePowerChaos:
				PMsg2( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_cannot_be_redrawn_due_to_N1_ELL )
					, frozenCard
					, _powerName(powerId, locale, powerNameStr) );
				break;
			}
		}
		break;

	case PokerGamePowerEffectFreeChipsReceived:
		{
			// The public data we have is the amount of free chips received.
			PString freeChipsReceived;
			INT32 powerId;

			StormHHThin::effect::GetFreeChips getFreeChipsData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, getFreeChipsData);
			powerId = getFreeChipsData.header.powerId;
			freeChipsReceived.appendInt64(getFreeChipsData.pubData.freeChipsReceived);

			switch(powerId)
			{
			case PokerGamePowerExtraLife:
				PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "%0 collects %1 chips from %2."
					, playerName
					, freeChipsReceived
					, _powerName(powerId, locale, powerNameStr) );
				break;
			}
		}
		break;

	case PokerGamePowerEffectMindControlDrawNewPower:
		{
			vector<INT32> newPowerIds; // private
			PString numPowers;
			int powerId;
			bool hasPrivateData = false;

			StormHHThin::effect::MindControlDrawNewPowersEffect mindControlData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, mindControlData);
			powerId = mindControlData.header.powerId;
			numPowers.appendInt(mindControlData.pubData.numNewPowers);
			newPowerIds = mindControlData.privData.newPowerIds;
			hasPrivateData = mindControlData.header.exposePrivacy;
			
			PString powers;
			for(size_t i = 0; i < newPowerIds.size(); ++i)
			{
				if(i > 0) powers.append(", ");
				powers.append(_powerName(newPowerIds[i], locale, powerNameStr));
			}

			switch(powerId)
			{
			case PokerGamePowerMindControl:
				{
					// Only the affected player (and admin) may see the actual power received information.
					if (showPrivateInfo(admin || playerName.equals( myUserAlias ), hasPrivateData, isServer))
						PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "%0 draws %1.", playerName, powers );
					else
						PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "%0 draws %1 replacement Power Card(s).", playerName, numPowers );
				}
				break;
			}
		}
		break;

	case PokerGamePowerEffectExchangePowers:
		{
			int powerId = 0;
			bool added = false;
			int changedPowerId = 0;
			bool hasPrivateData = false;

			StormHHThin::effect::ExchangePowerEffect ExchangePowerEffect;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, ExchangePowerEffect);
			powerId = ExchangePowerEffect.header.powerId;
			added = ExchangePowerEffect.privData.isAdded;
			changedPowerId = ExchangePowerEffect.privData.changedPowerId;
			hasPrivateData = ExchangePowerEffect.header.exposePrivacy;

			if (showPrivateInfo(admin || playerName.equals( myUserAlias ), hasPrivateData, isServer))
			{
				if (added)
				{ // added
					PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "%0 draws %1", playerName, _powerName(changedPowerId, locale, powerNameStr) );
				}
				else
				{ // removed
					PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "%0 discards %1 automatically", playerName, _powerName(changedPowerId, locale, powerNameStr) );
				}
			}
			else
			{
				// nothing is printed
			}
		}
		break;
	case PokerGamePowerEffectChipsCarryOver:
		{
			StormHHThin::effect::CarryOverChipsToNextHand chipsCarryOverData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, chipsCarryOverData);
			// Store the amount of chips being carried over to the next hand, we'll use this in the summary.
			//#pragma warning(suppress: 4244)  // ignore the compilation warning C4244 for next line
			hand.carryOverToNextHand = static_cast<INT32>(chipsCarryOverData.pubData.chipsCarriedOver);  // here it's safe to assign INT64 -> INT32
		}
		break;
	case PokerGamePowerEffectRebootPowers:
		{
			INT32 powerId;
			PString discardedPowersString;
			PString reloadedPowersString;
			bool exposePrivData = false;

			StormHHThin::effect::RebootEffect rebootData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, rebootData);
			powerId = rebootData.header.powerId;

			exposePrivData = rebootData.header.exposePrivacy;

			if(exposePrivData)
			{
				for (size_t i=0; i < rebootData.privData.discardedPowers.size(); ++i)
				{
					if (i > 0)
					{
						discardedPowersString.append(", ");
					}
					discardedPowersString.append(_powerName(rebootData.privData.discardedPowers[i], locale, powerNameStr));
				}

				for (size_t i=0; i < rebootData.privData.reloadedPowers.size(); ++i)
				{
					if (i > 0)
					{
						reloadedPowersString.append(", ");
					}
					reloadedPowersString.append(_powerName(rebootData.privData.reloadedPowers[i], locale, powerNameStr));
				}
			}

			if (showPrivateInfo(admin || playerName.equals( myUserAlias ), exposePrivData, isServer))
			{
				if (discardedPowersString.length() != 0)
				{
					PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "%0 discards (%1) and draws (%2)",
						playerName, discardedPowersString, reloadedPowersString);
				}
				else
				{
					PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "%0 discards no Power Cards and draws (%1)",
						playerName, reloadedPowersString);
				}
			}
			else
			{
				// output nothing
			}
		}
		break;
	case PokerGamePowerEffectAddBoardCard:
		{
			INT32 powerId;
			PString card;

			StormHHThin::effect::AddBoardCard addBoardCardData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, addBoardCardData);
			powerId = addBoardCardData.header.powerId;
			card.moveFrom(addBoardCardData.pubData.card);

			// Determine if the card is in the first board or the second.
			if(boardCardsPstr[1].find(card))
			{
				PMsg1( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_The_N0_has_been_added_to_the_s_ELL ), card);
			}
			else
			{
				PMsg1( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_The_N0_has_been_added_to_the_b_ELL ), card);
			}

			// Set the effect's causing power as the power affecting the board, so that the card dealing effect can be properly reported.
			powerIdAffectingBoard = powerId;
		}
		break;

	case PokerGamePowerEffectDarkBetCall:
		{
			INT32 powerId;

			StormHHThin::effect::DarkBetCall darkBetCallData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, darkBetCallData);
			powerId = darkBetCallData.header.powerId;

			PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "%0 calls %1 and puts in a hidden amount.",
				playerName, _powerName(powerId, locale, powerNameStr) );
		}
		break;

	case PokerGamePowerEffectDarkBetEnd:
		{
			INT32 powerId;
			PString amount;

			StormHHThin::effect::DarkBetEnd darkBetEndData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, darkBetEndData);
			powerId = darkBetEndData.header.powerId;
			amount.appendInt64(darkBetEndData.pubData.amount);

			PMsgNoTranslate( eventHistoryStr, LocalesTable, locale, "%0's %1 is revealed as %2 chips",
				playerName, _powerName(powerId, locale, powerNameStr), amount );
		}
		break;

	case PokerGamePowerEffectIntelCard:
		{
			int numSensedCards; // public
			PString sensedCards; // private
			int powerId;
			bool hasPrivateData = false;

			StormHHThin::effect::IntelEffect intelEffectData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, intelEffectData);
			powerId = intelEffectData.header.powerId;
			numSensedCards = intelEffectData.pubData.numCardsSensed;
			sensedCards.moveFrom(intelEffectData.privData.sensedCards);
			hasPrivateData = intelEffectData.header.exposePrivacy;

			PASSERT(sensedCards.length()%2==0);

			PString cardsSizeString;
			cardsSizeString.appendInt(numSensedCards);

			// Only the affected player (and admin) may see the actual card sensed information.
			if (showPrivateInfo(admin || playerName.equals( myUserAlias ), hasPrivateData, isServer))
			{
				// Assemble the cards string.
				PString cardsString;
				for (int i = 0; i < numSensedCards; ++i)
				{
					if (i != 0)
					{
						cardsString.append(", ");
					}
					cardsString.append(sensedCards[2*i]).append(sensedCards[2*i+1]);
				}

				PMsg3( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_peeks_at_the_N1_on_top_of_t_ELL ), playerName, sensedCards, _powerName(powerId, locale, powerNameStr) );
			}
			else
			{
				if( numSensedCards == 1)
				{
					PMsg2( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_peeks_at_the_next_card_from_ELL ), playerName, _powerName(powerId, locale, powerNameStr) );
				}
				else
				{
					PMsg3( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_peeks_at_the_next_N1_cards__ELL ), playerName, cardsSizeString, _powerName(powerId, locale, powerNameStr) );
				}
			}
		}
		break;

	case PokerGamePowerEffectListUpdate:
	case PokerGamePowerEffectConfigUpdate:
	case PokerGamePowerEffectPlayerPowerListUpdated:
	case PokerGamePowerEffectPlayerPowerStatusUpdate:
	case PokerGamePowerEffectPlayerUnselectPower:
		break;
	}	 
}






//==============================================================================================================================================
//                                                          FORMATTING POWER EVENT BAR 
//==============================================================================================================================================


static void _formatPowerInvokeItemForEventBar( const Hand& hand, const TableHistoryItem& historyItem, int powerId, const PString& playerName, int locale, const char* myUserAlias, PString &eventHistoryStr, int& powerCardUniqueId )
{
	PString powerNameStr;
	
	PMsg2( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_plays_N1X ), playerName, _powerName(powerId, locale, powerNameStr) );

	// Storm power id are stored in the amount field.
	PASSERT5(historyItem.amount == powerId);

	switch (powerId)
	{
	case PokerGamePowerScanner:
		{
			StormHHThin::invoke::Scanner changeFutureData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, changeFutureData);
			powerCardUniqueId = changeFutureData.header.powerCardUniqueId;

			i18nAppendChar( eventHistoryStr, '\n' );

			bool isCardKept = changeFutureData.pubData.keepOrDiscard.equals("k");

			PString numCardsStr;
			numCardsStr.appendInt(changeFutureData.pubData.numCards);

			// Only the player who played the power (and admin) may see the specific power information.
			if (showPrivateInfo(playerName.equals( myUserAlias ), changeFutureData.header.exposePrivacy, false))
			{
				// We need to list the cards.
				INT32 numCards = changeFutureData.privData.cards.length() / 2;
				PASSERT5(changeFutureData.pubData.numCards == numCards);

				PString cardsList;
				for (INT32 i=0; i < numCards; ++i)
				{
					if (i != 0)
					{
						cardsList.append(", ");
					}
					cardsList.append(changeFutureData.privData.cards[2*i]).append(changeFutureData.privData.cards[2*i+1]);
				}

				if(isCardKept)
				{
					PMsg1( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_are_returned_to_the_top_of__ELL ), cardsList );
				}
				else
				{
					PMsg1( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_are_discardedX ), cardsList );
				}
			}
			else
			{
				if(isCardKept)
				{
					if(changeFutureData.pubData.numCards == 2)  // most of cases
					{
						PMsg( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_The_two_cards_are_returned_to__ELL ) );
					}
					else
					{
						PMsg1( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_The_N0_cards_are_returned_to_t_ELL ), numCardsStr );
					}
				}
				else
				{
					if(changeFutureData.pubData.numCards == 2)  // most of cases
					{
						PMsg( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_The_two_cards_are_discardedX ) );
					}
					else
					{
						PMsg1( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_The_N0_cards_are_discardedX ), numCardsStr );
					}
				}
			}
		}
		break;

	case PokerGamePowerClone:
		{
			StormHHThin::invoke::Clone cloneData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, cloneData);
			powerCardUniqueId = cloneData.header.powerCardUniqueId;

			if (cloneData.pubData.powerIdToPlay != PokerGamePowerSecretPlaceholder)
			{
				i18nAppendChar( eventHistoryStr, '\n' );
				PMsg2( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_receives_N1X ), playerName, _powerName(cloneData.pubData.powerIdToPlay, locale, powerNameStr) );
			}
		}
		break;

	case PokerGamePowerEMP:
		{
			StormHHThin::invoke::EMP empData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, empData);
			powerCardUniqueId = empData.header.powerCardUniqueId;

			i18nAppendChar( eventHistoryStr, '\n' );

			int startStreet = empData.pubData.startStreet;
			if(startStreet == 0) // 0 is preflop
			{
				PMsg( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_No_more_powers_can_be_played_p_ELL ) );
				++startStreet;
			}

			PString streetNames;
			for(int i = startStreet; i <= empData.pubData.endStreet; ++i)
			{
				if( !roundName( i ).equals( P_TXT( TXTHHY_EMPTY_STUB ) ) )
				{
					if(*streetNames)
					{
						i18nAppendChar( eventHistoryStr, ',' );
					}
					PMsg( streetNames, LocalesTable, locale, roundName( i ) );
				}
			}

			if(*streetNames)
			{
				PMsg1( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_No_more_powers_can_be_played_o_ELL ), streetNames );
			}
		}
		break;

	case PokerGamePowerEngineer:
		{
			StormHHThin::invoke::Engineer engineerData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, engineerData);
			powerCardUniqueId = engineerData.header.powerCardUniqueId;

			i18nAppendChar( eventHistoryStr, '\n' );
			PMsg1( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_The_N0_is_selected_and_returne_ELL ), engineerData.pubData.chosenCard );

			PString discardedCardsList;
			insertCommaInCardsString(engineerData.pubData.discardedCards, discardedCardsList);

			i18nAppendChar( eventHistoryStr, '\n' );
			PMsg1( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_The_N0_are_discardedX ), discardedCardsList );
		}
		break;

	case PokerGamePowerSyphon:
		{
			StormHHThin::invoke::Syphon mainDrain;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, mainDrain);
			powerCardUniqueId = mainDrain.header.powerCardUniqueId;

			i18nAppendChar( eventHistoryStr, '\n' );

			PString manDrained, manaReceived;
			manDrained.appendInt64(mainDrain.pubData.manaStolen);
			manaReceived.appendInt64(mainDrain.pubData.manaReceived);

			PMsg3( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_removes_N1_energy_from_N2X ), playerName, manDrained, mainDrain.pubData.targetUserId );
			i18nAppendChar( eventHistoryStr, '\n' );
			PMsg2( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_receives_N1_energyX ), playerName, manaReceived );
		}
		break;

	case PokerGamePowerReload:
		{
			StormHHThin::invoke::Reload reloadData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, reloadData);
			powerCardUniqueId = reloadData.header.powerCardUniqueId;

			// only display this info for opponents, not the player himself
			if (!showPrivateInfo(playerName.equals( myUserAlias ), reloadData.header.exposePrivacy, false))
			{
				i18nAppendChar( eventHistoryStr, '\n' );

				if(reloadData.pubData.numCards == 1)
				{
					PMsg1( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_redraws_1_cardX ), playerName );
				}
				else
				{
					PString numCards;
					numCards.appendInt(reloadData.pubData.numCards);
					PMsg2( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_redraws_N1_cardsX ), playerName, numCards );
				}
			}
		}
		break;

	case PokerGamePowerPacify:
		{
			StormHHThin::invoke::Pacify leapData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, leapData);
			powerCardUniqueId = leapData.header.powerCardUniqueId;

			i18nAppendChar( eventHistoryStr, '\n' );
			PMsg( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_Prevents_all_future_bets_this__ELL ) );
			i18nAppendChar( eventHistoryStr, '\n' );
			PMsg( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_All_bets_from_this_street_retu_ELL ) );
		}
		break;

	case PokerGamePowerMint:
		{
			StormHHThin::invoke::Mint mintData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, mintData);
			powerCardUniqueId = mintData.header.powerCardUniqueId;

			i18nAppendChar( eventHistoryStr, '\n' );

			PString mintChipsStr;
			mintChipsStr.appendInt64(mintData.pubData.chipsMinted);
			PMsg1( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_chips_have_been_minted_and__ELL ), mintChipsStr );
		}
		break;

	case PokerGamePowerChaos:
		{
			StormHHThin::invoke::Chaos chaosData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, chaosData);
			powerCardUniqueId = chaosData.header.powerCardUniqueId;

			i18nAppendChar( eventHistoryStr, '\n' );

			PString numBombs;
			numBombs.appendInt(chaosData.pubData.numBombs);
			PMsg3( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_plays_N1_redrawing_N2_rando_ELL )
				, playerName
				, _powerName(historyItem.amount, locale, powerNameStr)
				, numBombs );
		}
		break;

		// new power invoked event added here for event bar!

	default:
		{
			StormHHThin::invoke::BaseData baseData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, baseData);
			powerCardUniqueId = baseData.header.powerCardUniqueId;
		}
		break;
	}
}


void _formatPowerEffectItemForEventBar( const Hand& hand, const TableHistoryItem& historyItem, int powerId, int powerCardUniqueId, const PString& playerName, int locale, const char* myUserAlias, PString &eventHistoryStr )
{
	// Storm effect codes are stored in the amount field.
	switch (historyItem.amount)
	{
		case PokerGamePowerEffectAddBoardCard:
		{
			StormHHThin::effect::AddBoardCard addBoardCardData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, addBoardCardData);

			if(addBoardCardData.header.powerId == powerId && addBoardCardData.header.powerCardUniqueId == powerCardUniqueId)
			{
				switch(powerId)
				{
				case PokerGamePowerDeploy:
					i18nAppendChar( eventHistoryStr, '\n' );
					PMsg1( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_The_N0_is_dealt_to_the_boardX ), addBoardCardData.pubData.card);
					break;
				default:
					break;
				}
			}
		}
		break;

	case PokerGamePowerEffectReplaceBoardCard:
		{
			StormHHThin::effect::ReplaceBoardCard replaceBoardCardData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, replaceBoardCardData);

			if(replaceBoardCardData.header.powerId == powerId && replaceBoardCardData.header.powerCardUniqueId == powerCardUniqueId)
			{
				switch(powerId)
				{
				case PokerGamePowerDisintegrate:
					i18nAppendChar( eventHistoryStr, '\n' );
					PMsg1( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_The_N0_is_destroyedX ), replaceBoardCardData.pubData.oldCard );
					break;
				case PokerGamePowerMorph:
					i18nAppendChar( eventHistoryStr, '\n' );
					PMsg2( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_The_N0_is_replaced_with_N1X ), replaceBoardCardData.pubData.oldCard, replaceBoardCardData.pubData.newCard );
					break;
				default:
					break;
				}
			}
		}
		break;

	case PokerGamePowerEffectReplaceMultiBoardCards:
		{
			StormHHThin::effect::ReplaceMultiBoardCards replaceMultiBoardCardsData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, replaceMultiBoardCardsData);

			if(replaceMultiBoardCardsData.header.powerId == powerId && replaceMultiBoardCardsData.header.powerCardUniqueId == powerCardUniqueId)
			{
				switch(powerId)
				{
				case PokerGamePowerChaos:
					{
						int sz = min(replaceMultiBoardCardsData.pubData.oldCards.size(), replaceMultiBoardCardsData.pubData.newCards.size());
						for(int i = 0; i < sz; ++i)
						{
							i18nAppendChar( eventHistoryStr, '\n' );
							PMsg2( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_redraws_to_N1 ), replaceMultiBoardCardsData.pubData.oldCards[i], replaceMultiBoardCardsData.pubData.newCards[i] );
						}
					}
					break;
				default:
					break;
				}
			}
		}
		break;

	case PokerGamePowerEffectReplaceHoleCards:
		{
			StormHHThin::effect::ReplaceHoleCard replaceHoleCardData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, replaceHoleCardData);

			if(replaceHoleCardData.header.powerId == powerId && replaceHoleCardData.header.powerCardUniqueId == powerCardUniqueId)
			{
				bool isTextForPlayer = showPrivateInfo(playerName.equals( myUserAlias ), replaceHoleCardData.header.exposePrivacy, false);

				switch(powerId)
				{
				case PokerGamePowerUpgrade:
					if (isTextForPlayer)
					{
						i18nAppendChar( eventHistoryStr, '\n' );
						PMsg3( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_receives_the_XN1X_and_disca_ELL )
						, playerName
						, replaceHoleCardData.privData.newCard
						, replaceHoleCardData.privData.oldCard );
					}
					break;
				case PokerGamePowerReload:
					if (isTextForPlayer)
					{
						i18nAppendChar( eventHistoryStr, '\n' );
						PMsg3( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_redraws_N1_to_N2X )
						, playerName
						, replaceHoleCardData.privData.oldCard
						, replaceHoleCardData.privData.newCard );
					}
					break;
				default:
					break;
				}
			}
		}
		break;

	case PokerGamePowerEffectDiscardHoleCards:
		{
			StormHHThin::effect::DiscardHoleCards discardHoleCardData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, discardHoleCardData);

			if(discardHoleCardData.header.powerId == powerId && discardHoleCardData.header.powerCardUniqueId == powerCardUniqueId)
			{
				bool isTextForPlayer = showPrivateInfo(playerName.equals( myUserAlias ), discardHoleCardData.header.exposePrivacy, false);

				switch(powerId)
				{
				case PokerGamePowerUpgrade:
					// Only the affected player may see the card information.
					if (isTextForPlayer)
					{
						PString cardsStr;
						int cardsSize = discardHoleCardData.privData.cards.length() / 2;

						for (int i=0; i < cardsSize; ++i)
						{
							if (i!=0)
								cardsStr.append(" ");
							cardsStr.append(discardHoleCardData.privData.cards[2*i]).append(discardHoleCardData.privData.cards[2*i+1]);
						}
						
						i18nAppendChar( eventHistoryStr, '\n' );
						PMsg2( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_discards_XN1XX ), playerName, cardsStr );
					}
					break;
				default:
					break;
				}
			}
		}
		break;

	case PokerGamePowerEffectReceiveHoleCards:
		{
			StormHHThin::effect::ReceiveHoleCards receiveHoleCardData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, receiveHoleCardData);

			if(receiveHoleCardData.header.powerId == powerId && receiveHoleCardData.header.powerCardUniqueId == powerCardUniqueId)
			{
				bool isTextForPlayer = showPrivateInfo(playerName.equals( myUserAlias ), receiveHoleCardData.header.exposePrivacy, false);

				switch(powerId)
				{
				case PokerGamePowerUpgrade:
					// Only the affected player (and admin) may see the card information.
					if (isTextForPlayer)
					{
						PString cardsStr;
						int cardsSize = receiveHoleCardData.privData.cards.length() / 2;

						for (int i=0; i < cardsSize; ++i)
						{
							if (i!=0)
								cardsStr.append(" ");
							cardsStr.append(receiveHoleCardData.privData.cards[2*i]).append(receiveHoleCardData.privData.cards[2*i+1]);
						}

						i18nAppendChar( eventHistoryStr, '\n' );
						PMsg2( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_receives_XN1XX ), playerName, cardsStr );
					}
					break;
				default:
					break;
				}
			}
		}
		break;

	case PokerGamePowerEffectShowOneHoleCard:
		{
			StormHHThin::effect::ShowOneHoleCard showOneCardData;
			decodeDataStructFromHandHistoryContentString(historyItem.contents, showOneCardData);

			if(showOneCardData.header.powerId == powerId && showOneCardData.header.powerCardUniqueId == powerCardUniqueId)
			{
				switch(powerId)
				{
				case PokerGamePowerXRay:
					{
						i18nAppendChar( eventHistoryStr, '\n' );
						PMsg2( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_shows_N1X )
							, historyItem.userId
							, showOneCardData.pubData.card );
					}
					break;
				default:
					break;
				}
			}
		}
		break;

	default:
		{
			// so far we don't have the general effect
			// just ignore it
		}
		break;
	}
}


static int _formatPowerEventForClientEventBar0( int start, int end, const Hand& hand, int locale, const char* userId, PString& eventHistoryStr, bool& hasMoreEffectEvents )
{
	PASSERT(start >= 0 && start < static_cast< int >( hand.history.size() ));
	PASSERT(end <= static_cast< int >( hand.history.size() ));

	PASSERT(hand.history[ start ].eventCode == TABLE_HISTORY_STORM_POWER);

	int powerId = hand.history[ start ].amount;
	int playerSeat = hand.history[ start ].position;
	PString playerName = hand.history[ start ].userId;
	PASSERT5(powerId > 0);
	PASSERT5(playerSeat >= 0);

	int powerCardUniqueId = -1;
	_formatPowerInvokeItemForEventBar(hand, hand.history[ start ], powerId, playerName, locale, userId, eventHistoryStr, powerCardUniqueId);
	PASSERT5(powerCardUniqueId > 0);

	bool done = false;
	for(int i = start + 1; i < end && !done; ++i)
	{
		const TableHistoryItem& historyItem = hand.history[ i ];

		switch(historyItem.eventCode)
		{
		case TABLE_HISTORY_STORM_EFFECT:
			{
				PASSERT5(powerId > 0);
				PASSERT5(powerCardUniqueId > 0);
				_formatPowerEffectItemForEventBar(hand, historyItem, powerId, powerCardUniqueId, playerName, locale, userId, eventHistoryStr);
			}
			break;

		case TABLE_HISTORY_STORM_POWERPLAY_END:
			{
				PASSERT5(powerCardUniqueId > 0);
				if(historyItem.amount == powerCardUniqueId)
				{
					done = true;
				}
			}
			break;

		default:
			break;
		}
	}

	hasMoreEffectEvents = !done;
	return powerId;
}


/*-----------------------------------------------------------------------------------------------------------
formatPowerEventForClientEventBar
powerEventSeq starts from 0, could also be negative, -1 means the last power played in the current hand
return the corresponding powerId, -1 means the power with powerEventSeq does not exist 
------------------------------------------------------------------------------------------------------------*/
int formatPowerEventForClientEventBar( const Hand& hand, int locale, const char* userId, int powerEventSeq, PString& eventHistoryStr, bool& hasMoreEffectEvents )
{
	// find the power it specified in powerEventSeq
	int start = -1;

	if(powerEventSeq >= 0)
	{
		for(size_t i = 0; i < hand.history.size(); ++i)
		{
			if(hand.history[ i ].eventCode == TABLE_HISTORY_STORM_POWER)
			{
				if(powerEventSeq-- == 0)
				{
					start = i;
					break;
				}
			}
		}
	}
	else
	{
		for(int i = hand.history.size() - 1; i >= 0; --i)
		{
			if(hand.history[ i ].eventCode == TABLE_HISTORY_STORM_POWER)
			{
				if(++powerEventSeq == 0)
				{
					start = i;
					break;
				}
			}
		}
	}

	if(start < 0)
	{
		return -1;  // the power was not found
	}

	return _formatPowerEventForClientEventBar0( start, hand.history.size(), hand, locale, userId, eventHistoryStr, hasMoreEffectEvents );
}

// the alternative interface to format the power specified by powerCardUniqueId
// return the corresponding powerId, -1 means the power with powerCardUniqueId does not exist 
int formatPowerEventByUniqueIdForClientEventBar( int powerCardUniqueId, const Hand& hand, int locale, const char* userId, PString& eventHistoryStr )
{
	int start = -1;
	int end = hand.history.size();

	for(int i = hand.history.size() - 1; i >= 0; --i)
	{
		if(hand.history[ i ].eventCode == TABLE_HISTORY_STORM_POWERPLAY_END)
		{
			if(hand.history[ i ].amount == powerCardUniqueId)
			{
				end = i + 1;  // 'end' is not inclusive!
			}
		}
		else if(hand.history[ i ].eventCode == TABLE_HISTORY_STORM_POWER)
		{
			StormHHThin::invoke::BaseData baseData;
			decodeDataStructFromHandHistoryContentString(hand.history[ i ].contents, baseData);
			if(powerCardUniqueId == baseData.header.powerCardUniqueId)
			{
				start = i;
				break;
			}
		}
	}

	if(start < 0)
	{
		return -1;  // the power was not found
	}

	bool hasMoreEffectEvents = false;
	return _formatPowerEventForClientEventBar0( start, end, hand, locale, userId, eventHistoryStr, hasMoreEffectEvents );
}


void formatStormHandSummaryForClientEventBar( const Hand& hand, int locale, PString& eventHistoryStr )
{
	if(hand.game != HandTypeHoldem)
	{
		return;  // only Holdem is supported now, no Hi/Lo type hand!
	}

	const UINT32 effectiveScalePM = hand.getEffectiveScalePM();
	const UINT32 denominator = ( hand.tournType == DenominatedTableTour || hand.chipsType == POKER_DUEL_CHIPS ) ? 1 : 100;

	bool hasShowDown = false;
	bool runItTwice = false;
	int boardCounter = 0;
	int sidePot = 0;

	bool isAnteUpPowerStarted = false;
	int anteUpPlayer = -1;

	PString currString; // table currency string
	PStringX currSign;  // table currency sign
	_fillCurrSign(hand.currency, currString, currSign);

	PString playerNames[MAX_TABLE_PLAYERS];
	PString playerHandValueStrings[2][MAX_TABLE_PLAYERS];   // two boards
	bool isWinners[2][MAX_TABLE_PLAYERS];                // two boards

	// initialization
	for(size_t j = 0; j < MAX_TABLE_PLAYERS; ++j)
	{
		isWinners[0][j] = false;
		isWinners[1][j] = false;

		if(j < hand.users.size())
		{
			if(hand.users[j].position >= 0 && hand.users[j].position < MAX_TABLE_PLAYERS) //-V560
			{
				playerNames[ hand.users[j].position ] = hand.users[j].name;
			}
			else
			{
				PASSERT5(0);
			}
		}
	}
	
	for(size_t i = 0; i < hand.history.size(); ++i)
	{
		switch( hand.history[ i ].eventCode )
		{
		case TABLE_HISTORY_ACTION:
			{
				int seat = hand.history[ i ].position;
				PASSERT(seat >= 0 && seat < MAX_TABLE_PLAYERS);

				if(isAnteUpPowerStarted && seat == anteUpPlayer)
				{
					isAnteUpPowerStarted = false;
				}

				if(!isAnteUpPowerStarted && (hand.history[ i ].action == 's' || hand.history[ i ].action == 'S' || hand.history[ i ].action == 'M'))
				{
					if(!hasShowDown)
					{
						if(runItTwice)
						{
							PMsg( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_First_Board ) );
							i18nAppendChar( eventHistoryStr, ':' );
							i18nAppendChar( eventHistoryStr, '\n' );
						}
						hasShowDown = true;
					}

					// find the index of hand.users
					if(boardCounter == 0)
					{
						size_t k;
						for(k = 0; k < hand.users.size(); ++k)
						{
							if(hand.users[k].position == seat)
							{
								break;
							}
						}
						PASSERT5(k < hand.users.size());
						if(k < hand.users.size()) //-V547
						{
							fillBestCombPstr( hand, playerHandValueStrings[boardCounter][seat], k, locale );
						}
					}
					else  // RIT second board
					{
						size_t k;
						for(k = 0; k < hand.users2.size(); ++k)
						{
							if(hand.users2[k].name.equals(hand.history[ i ].userId))
							{
								break;
							}
						}
						PASSERT5(k < hand.users2.size());
						if(k < hand.users2.size()) //-V547
						{
							fillBestCombPstr2( hand, playerHandValueStrings[boardCounter][seat], k, locale );
						}
					}
				}
			}
			break;

		case TABLE_HISTORY_STORM_POWER:
			{
				if(hand.history[ i ].amount == PokerGamePowerAnteUp)
				{
					PASSERT5(!isAnteUpPowerStarted);
					isAnteUpPowerStarted = true;
					anteUpPlayer = hand.history[ i ].position;
					PASSERT5(anteUpPlayer >= 0);
				}
			}
			break;

		case TABLE_HISTORY_TAKEFROMPOT:
			{
				PASSERT( hand.history[ i ].position >= 0 && hand.history[ i ].position < MAX_TABLE_PLAYERS );
				PASSERT( hand.history[ i ].action >= 0 && hand.history[ i ].action < MAX_NUM_POTS );

				if( hand.history[ i ].action > sidePot ) // just for indication( main, side, side-2 )
				{
					sidePot = hand.history[ i ].action; 
				}

				PString potAmountStr, str_chips_buf;
				char str_stakes[ 32 ] = "";
				_formatScaledChips( hand.history[ i ].amount, effectiveScalePM, denominator, str_stakes );
				i18nAppend( potAmountStr, _formatChips( str_chips_buf, locale, hand, currSign, str_stakes ) );

				if(hasShowDown)
				{
					isWinners[boardCounter][hand.history[ i ].position] = true;

					PString potNumStr;
					if( hand.history[ i ].action )//if side pot
					{
						PMsg( potNumStr, LocalesTable, locale, P_TXT( TXTHHY_side_pot ) );
						if( sidePot > 1 ) // Number of side Pot
						{
							char str_number[ 32 ] = "";
							sprintf( str_number, " %d", hand.history[ i ].action );   
							i18nAppend( potNumStr, str_number );
						}
					}
					else if( sidePot )
					{
						PMsg( potNumStr, LocalesTable, locale, P_TXT( TXTHHY_main_pot ) );
					}
					else
					{
						PMsg( potNumStr, LocalesTable, locale, P_TXT( TXTHHY_pot ) );
					}

					// user wins pot str (pot amount) with hand value
					// e.g. kitty wins side pot 2 ($25) with a pair of Threes
					PMsg4( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_wins_N1_XN2X_with_N3 ), hand.history[ i ].userId, potNumStr, potAmountStr, playerHandValueStrings[boardCounter][hand.history[ i ].position]);
				}
				else
				{
					PMsg2( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_wins_pot_XN1X ), hand.history[ i ].userId, potAmountStr);
				}

				i18nAppendChar( eventHistoryStr, '\n' );
			}
			break;

		case TABLE_HISTORY_EXTRA:
			{
				if(hand.history[ i ].action == TH_EX_Act_Start_Run_It_Twice)
				{
					PASSERT5(boardCounter == 0);
					runItTwice = true;
				}
				else if(hand.history[ i ].action == TH_EX_Act_Run_It_Twice_Evaluate_Second_Board)
				{
					PASSERT5(runItTwice);
					boardCounter = 1;

					for(int j = 0; j < MAX_TABLE_PLAYERS; ++j)
					{
						if(!isWinners[0][j] && *playerNames[j] && *playerHandValueStrings[0][j])
						{
							PMsg2( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_has_N1 ), playerNames[j], playerHandValueStrings[0][j] );
							i18nAppendChar( eventHistoryStr, '\n' );
						}
					}

					PMsg( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_Second_Board ) );
					i18nAppendChar( eventHistoryStr, ':' );
					i18nAppendChar( eventHistoryStr, '\n' );
				}
			}
			break;
		}
	}

	// print the hand values for all losers
	for(int j = 0; j < MAX_TABLE_PLAYERS; ++j)
	{
		if(!isWinners[boardCounter][j] && *playerNames[j] && *playerHandValueStrings[boardCounter][j])
		{
			PMsg2( eventHistoryStr, LocalesTable, locale, P_TXT( TXTHHY_N0_has_N1 ), playerNames[j], playerHandValueStrings[boardCounter][j] );
			i18nAppendChar( eventHistoryStr, '\n' );
		}
	}
}
