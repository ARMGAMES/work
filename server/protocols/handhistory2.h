// handhistory2.h - i18n version of handhistory.h and tablefmt.h
#ifndef	handhistory2_h_included
#define	handhistory2_h_included

#include "cards.h"
#include "chatfilter.h"
//
// NB: ONLY THE FUNCTIONS WITH NEW SIGNATURES FOR TRANSITIONAL PERIOD!
//
void _rankName( PString& val, int locale, int rank );
void _rankName_s( PString& val, int locale, int rank );
void _rankName_n( PString& val, int locale, int rank );

void _bestLoComb( int locale, const Card* bestLowCards, PString& comb, bool separator = false  );
void _bestRazzSwtch( int locale, BYTE bestLow, const Card* bestLowCards, PString& comb, bool separator = false );
void _bestHandSwtch( int locale, BYTE bestHigh, const Card* bestHighCards, PString& comb );
void _bestTripleDrawSwtch( int locale, BYTE bestLow, const Card* bestLowCards, PString& comb, bool separator = false );
void _bestBadugiSwtch( int locale, BYTE bestLow, const Card* bestLowCards, bool isPhraseComplete, PString& comb, bool separator = false );
/*
requestType - one of the constants HAND_HISTORY_* in "lobby.h"
requestContents depends on requestType (see "lobby.h")
*/

struct Hand;

void formatHistorySubject( int locale, BYTE requestType, UINT64 requestContents, const char* brandName, PString& ret );
void formatHistoryHeader( int locale, const char* userId, const char* userEmail, BYTE requestType, UINT64 requestContents, UINT32 requestReturned, PString& history );
void formatHandHistory( int locale, const Hand& hand, const char* myUserAlias, UINT32 timezone, const vector< PString >& pattern, UINT32 brandId, PString& ret, PString& handSummary, bool admin = false, bool closedCard = false, ChatFilterHelper* pChatFilterHelper = NULL, bool isServer = true, const char* myRealUserId = "" );
void formatHistoryFooter( int locale, PString& ret, const char* site );

// PYR-40128
//powerEventSeq starts from 0, could also be negative, -1 means the last power played in the current hand
//return the corresponding powerId, -1 means the power with the power play sequence number powerEventSeq is not existing 
int formatPowerEventForClientEventBar( const Hand& hand, int locale, const char* userId, int powerEventSeq, PString& eventHistoryStr, bool& hasMoreEffectEvents );
// the alternative interface to format the power specified by powerCardUniqueId
int formatPowerEventByUniqueIdForClientEventBar( int powerCardUniqueId, const Hand& hand, int locale, const char* userId, PString& eventHistoryStr );
// provide the hand finished event text
void formatStormHandSummaryForClientEventBar( const Hand& hand, int locale, PString& eventHistoryStr );

#endif	//handhistory2_h_included
