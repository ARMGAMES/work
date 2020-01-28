#ifndef cards_h_included
#define cards_h_included

//#include "lobby.h"
//#include "pplib.h"
#include "ppincludebase.h"
#include <vector>

enum Statistic {
				NoFlag =				0x0,
				HandFlag =				0x1,
				LeftFlag =				0x2,
				FoldFlag =				0x4,
				//FoldBeforeFlop =		0x8, //artefact
				SeenFlop =				0x10,
				MuckFlag =				0x20,
				ShownCards =			0x40,
				SeenShowdown =			0x80,
				Street_4th =			0x100,
				Street_5th =			0x200,
				Street_6th =			0x400,
				BeenWinner =			0x800, 
				MoreThanAnte_posted =	0x1000,
				BB_posted =				0x2000,
				SB_posted =				0x4000,
				Been_sitOut =			0x8000,
				Been_OutOfHand =		0x10000,
				Been_Dealer =			0x20000,
				BeenWinnerNoShowdown =	0x40000,
				CardsDrawn =			0x80000,
				//Reserved =			0x100000,
				ShownSomeCards =		0x200000,		// #15261
				FoldAndShow =           0x400000,		// #15261
				RIT_Check_Passed =      0x800000,       // #16658 
				Street_7th =			0x1000000,		// PYR-49457, represent ocean round for Stretch power play
				UnfoldFlag =			0x2000000,		// PYR-67163
				GB_posted  =			0x4000000,		// PYR-82834, giant blind posted
				Been_allin_cashedout =	0x8000000,      // PYR-95110, player cashed out 
				Disconn_ected =			0x40000000,
				Been_SittingIn =		0x80000000
			   };

#define Cards52 52
#define Cards51 51

#define MAX_TABLE_BOARDS		3

#ifdef _POWERUPS
#define MAX_TABLE_BOARD_CARDS	7
#else
#define MAX_TABLE_BOARD_CARDS	5
#endif

#define MAX_TABLE_PLAYER_CARDS	7
#define MAX_EXTRA_KICKERS		3

#define SPECIAL_CARD_RANK_ASSASSINATED	15


enum HandRanking { HIGHCARD = 1, ONEPAIR, TWOPAIRS, THREEOFAKIND, STRAIGHT, FLUSH, FULLHOUSE, FOUROFAKIND, STRAIGHTFLUSH, ROYALFLUSH /*place holder, not used in server side, only in client msg*/ };


struct ExtraKickers
{
	int rank[ MAX_EXTRA_KICKERS ];
	void clear()
	{
		for( int i = 0; i < MAX_EXTRA_KICKERS; ++i )
			rank[ i ] = 0;
	}

	ExtraKickers()
	{
		clear();
	}
};

struct Card
{
	int rank;//2-10 - as is, 11-J, 12-Q, 13-K, 14-A
	char suit;//'s'/'h'/'d'/'c'. ' ' means 'unknown'
	void clear()
	{
		rank = 0;
		suit = 0;
	}
	void hide()
	{
		rank = -1;
		suit = 0;
	}
	Card( int rank_, char suit_ )
	{
		rank = rank_;
		suit = suit_;
	}
	Card()
	{ // added due to Bugs#: 3871 /3872 / 3097 (too much assertions on Client side)
		clear();
	}
	bool eq( const Card& other ) const { return ( rank == other.rank ) && ( suit == other.suit ); }
	bool hidden() const { return rank == -1; }

	static INT8 cardToBitIndex( const Card& card );
	static Card bitIndexToCard( INT8 bitIndex );
};

int _compareBestHighHands( Card& aKicker, Card& bKicker, int a, const Card* aCards, int b, const Card* bCards, ExtraKickers& extraKickers, int n = 5 );
int _compareBestRazzHands( Card& aKicker, Card& bKicker, int a, const Card* aCards, int b, const Card* bCards, ExtraKickers& extraKickers, int n = 5 );
int _compareBestTriple27Hands( Card& aKicker, Card& bKicker, int a, const Card* aCards, int b, const Card* bCards, ExtraKickers& extraKickers, int n = 5 );
int _compareBestBadugiHands( Card& aKicker, Card& bKicker, int a, const Card* aCards, int b, const Card* bCards );

bool _decodeHandStrengthInteger( INT64 handStrengthCode, int& handRank, std::vector<Card>& hand );

static const int cardsArray[ ] = 
{
	-1, -1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, -1, 
	-1, 14, -1, -1, -1, -1, -1, -1, -1, -1, 11, 13, -1, -1, -1, -1, 
	-1, 12, -1, -1, 10
};

void strToCards( const char* comb, Card* bestCards );

#endif

