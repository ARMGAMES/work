#include "pperror.h"
#include "cards.h"
//#include "pyrtime.h"
//#include "dbm.h"
/*
static const int cardsArray[ ] = 
	{
	-1, -1, 2, 3, 4, 5, 6, 7, 8, 9, -1, -1, -1, -1, -1, -1, 
	-1, 14, -1, -1, -1, -1, -1, -1, -1, -1, 11, 13, -1, -1, -1, -1, 
	-1, 12, -1, -1, 10 	};
---------------------------------------------------------------------------------------------
				PString to Cards, converter 
---------------------------------------------------------------------------------------------*/
void strToCards( const char* comb, Card* bestCards )
	{
	/*struct Card
	{
	char suit;//'s'/'h'/'d'/'c'. ' ' means 'unknown'
	int rank;//2-9 - as is, 10-T, 11-J, 12-Q, 13-K, 14-A
	bool eq( const Card& other ) const { return ( rank == other.rank ) && ( suit == other.suit ); }
	};*/
	for( int i = 0; *comb && i < 10; ++comb, ++i )
		if( i % 2 )
			bestCards[ i/2 ].suit = *comb;
		else 
			bestCards[ i/2 ].rank = cardsArray[ *comb - 0x30 ];
	}

/*---------------------------------------------------------------------------------------------
				compareBestHighRanks
---------------------------------------------------------------------------------------------*/
static int _cmpRanks( Card& aKicker, Card& bKicker, const Card* aCards, const Card* bCards, ExtraKickers& extraKickers, int n )
	{
	for( int i = 0; i < n; ++i )
		{
		int cmp = aCards[ i ].rank - bCards[ i ].rank;
		if( cmp )
			{
			aKicker = aCards[ i ];
			bKicker = bCards[ i ];
			return cmp;
			}
		else if( i < MAX_EXTRA_KICKERS ) // Just protection instead of PASSERT(i < )
			extraKickers.rank[ i ] = aCards[ i ].rank;
		}
	return 0;
	}

/*---------------------------------------------------------------------------------------------
				compareBestHighHands 
---------------------------------------------------------------------------------------------*/
int _compareBestHighHands( Card& aKicker, Card& bKicker, int a, const Card* aCards, int b, const Card* bCards, ExtraKickers& extraKickers, int n )
	{
	aKicker.suit = 0;
	bKicker.suit = 0;

	if( a != b )
		return a - b;

	switch( a )
		{
		case 9://Straight Flush
		case 5://Straight
			PASSERT( n == 5 );
			return aCards[ 0 ].rank - bCards[ 0 ].rank;

		case 8://4-of-a-Kind
			{
			PASSERT( n >= 4 );
			int cmp = aCards[ 0 ].rank - bCards[ 0 ].rank;
			if( cmp )
				return cmp;
			}
			if( n == 4 )
				return 0;
			aKicker = aCards[ 4 ];
			bKicker = bCards[ 4 ];
			return aCards[ 4 ].rank - bCards[ 4 ].rank;

		case 7://Full House
			{
			PASSERT( n == 5 );
			int cmp = aCards[ 0 ].rank - bCards[ 0 ].rank;
			if( cmp )
				return cmp;
			}
			return aCards[ 3 ].rank - bCards[ 3 ].rank;

		case 6://Flush
			{
			PASSERT( n == 5 );
			int cmp = aCards[ 0 ].rank - bCards[ 0 ].rank;
			if( cmp )
				return cmp;
			}
			return _cmpRanks( aKicker, bKicker, aCards + 1, bCards + 1, extraKickers, 4 );

		case 4://3-of-a-Kind
			{
			PASSERT( n >= 3 );
			int cmp = aCards[ 0 ].rank - bCards[ 0 ].rank;
			if( cmp )
				return cmp;
			}
			return _cmpRanks( aKicker, bKicker, aCards + 3, bCards + 3, extraKickers, n - 3 );

		case 3://Two Pair
			{
			PASSERT( n >= 4 );
			int cmp = aCards[ 0 ].rank - bCards[ 0 ].rank;
			if( cmp )
				return cmp;
			cmp = aCards[ 2 ].rank - bCards[ 2 ].rank;
			if( cmp )
				return cmp;
			}
			if( n == 4 )
				return 0;
			aKicker = aCards[ 4 ];
			bKicker = bCards[ 4 ];
			return aCards[ 4 ].rank - bCards[ 4 ].rank;

		case 2://Pair
			{
			PASSERT( n >= 2 );
			int cmp = aCards[ 0 ].rank - bCards[ 0 ].rank;
			if( cmp )
				return cmp;
			}
			return _cmpRanks( aKicker, bKicker, aCards + 2, bCards + 2, extraKickers, n - 2 );

		case 1://High Card
			{
			PASSERT( n >= 1 );
			int cmp = aCards[ 0 ].rank - bCards[ 0 ].rank;
			if( cmp )
				return cmp;
			}
			return _cmpRanks( aKicker, bKicker, aCards + 1, bCards + 1, extraKickers, n - 1 );

		default:
			PASSERT( 0 );
			//return 0; unreachable
		}
	}

/*---------------------------------------------------------------------------------------------
				compareBestRazzRanks
---------------------------------------------------------------------------------------------*/
static int _cmpRazzRanks( /*Card& aKicker, Card& bKicker,*/ const Card* aCards, const Card* bCards, /*ExtraKickers& extraKickers, */ int n )
	{
	for( int i = 0; i < n; ++i )
		{
		int aRank = aCards[ i ].rank == 14 ? 1 : aCards[ i ].rank;
		int bRank = bCards[ i ].rank == 14 ? 1 : bCards[ i ].rank;

		int cmp = bRank - aRank;
		if( cmp )
			{
			//aKicker = aCards[ i ];
			//bKicker = bCards[ i ];
			return cmp;
			}
		//else if( i < MAX_EXTRA_KICKERS ) // Just protection instead of PASSERT(i < )
		//	extraKickers.rank[ i ] = aCards[ i ].rank;
		}
	return 0;
	}

/*---------------------------------------------------------------------------------------------
				compareBestRazzHands 
---------------------------------------------------------------------------------------------*/
int _compareBestRazzHands( Card& aKicker, Card& bKicker, int a, const Card* aCards, int b, const Card* bCards, ExtraKickers& /*extraKickers*/, int n )
	{
	aKicker.suit = 0;
	bKicker.suit = 0;

	if( a != b )
		return b - a;

	return _cmpRazzRanks( aCards, bCards, n ); // No any kickers at this time
	}

/*---------------------------------------------------------------------------------------------
				compareBestBadugiHands 
---------------------------------------------------------------------------------------------*/
int _compareBestBadugiHands( Card& aKicker, Card& bKicker, int a, const Card* aCards, int b, const Card* bCards )
	{
	aKicker.suit = 0;
	bKicker.suit = 0;

	if( a != b )
		return a - b; // this is a difference/!/ from _compareBestRazzHands()

	return _cmpRazzRanks( aCards, bCards, 4 ); // No kickers
	}

/*---------------------------------------------------------------------------------------------
				compareBestTriple27Hands 
---------------------------------------------------------------------------------------------*/
int _compareBestTriple27Hands( Card& aKicker, Card& bKicker, int a, const Card* aCards, int b, const Card* bCards, ExtraKickers& /*extraKickers*/, int n )
	{
	aKicker.suit = 0;
	bKicker.suit = 0;
	if( a != b )
		return b - a;

	for( int cmp, i = 0; i < n; ++i )
		{
		cmp = bCards[ i ].rank - aCards[ i ].rank;
		if( cmp )
			return cmp;
		}
	return 0;
	}

/*---------------------------------------------------------------------------------------------
				decode a hand (5 cards) from an INT64 integer 
---------------------------------------------------------------------------------------------*/
// please see this document about the encoded integer format
// https://confluence.pyrsoftware.ca/confluence/display/JP/BRD-35599+-+PokerDuel+-+Hand+Strength+Details

bool _decodeHandStrengthInteger( INT64 handStrengthCode, int& handRank, std::vector<Card>& handCards )
{
	if(handStrengthCode <= 0)
	{
		return false;   // must be positive number, the highest bit is always 0
	}

	UINT32 upper = (UINT32)((UINT64)handStrengthCode >> 32);
	UINT32 lower = (UINT32) handStrengthCode;

	handRank = upper >> 27;  // 4 bits

	if(handRank < 1 || handRank > 9)
	{
		return false;
	}

	for(int i = 0; i < 5; ++i)   // always 5 cards
	{
		int upperShift = 23 - 4 * i;
		int lowerShift = 30 - 2 * i;

		int rank = (upper >> upperShift) & 0x0F;
		int suitIndex = (lower >> lowerShift) & 0x3;

		if(rank < 2 || rank > 14)
		{
			return false;
		}

		Card card;
		card.rank = rank;
		card.suit = "cdhs"[ suitIndex ];

		handCards.push_back(card);
	}

	//TODO: verify the hand cards

	return true;
}


/* static */ INT8 Card::cardToBitIndex( const Card& card )
{
	if( card.rank > 14 || card.rank < 2 )
	{
		return -1;  // invalid card rank
	}

	BYTE i = card.rank - 2;

	switch( card.suit )
	{
	case 'c': break;
	case 'd': i += 13; break;
	case 'h': i += 26; break;
	case 's': i += 39; break;
	default: return -1;  // invalid card suit
	}

	return i;  // valid range is [0, 51]
}

/* static */ Card Card::bitIndexToCard( INT8 bitIndex )
{
	if(bitIndex < 0 || bitIndex > 51)
	{
		return Card(0,0);  // invalid card
	}

	if(bitIndex < 13)
	{
		return Card(bitIndex + 2, 'c');
	}
	else if(bitIndex < 26)
	{
		return Card(bitIndex - 13 + 2, 'd');
	}
	else if(bitIndex < 39)
	{
		return Card(bitIndex - 26 + 2, 'h');
	}
	else
	{
		return Card(bitIndex - 39 + 2, 's');
	}
}


