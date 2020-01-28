#include "stathistory2.h"

#include "ppmsg.h"
#include "i18n.locales.h"
#include "i18n.utils.h"
#include "i18n.localutil.h"
#ifndef PROTOCOL_CLIENT_ONLY
#include "i18n.msg_int.h"
#include "i18n.msg_hhy.h"
#endif
#include "currency.h"
#include "stringroutines.h"
#include "dbm.h"

#define MAX_NUM_STATS 10

static void _formatScaledTournChips( UINT32 value, UINT32 scale, UINT32 denominator, char* str_out ) // PYR-25238
{
	if( denominator == 0 )
	{
		PLog (LOG_TRACE_FAULTY_LOGIC ": denominator 0 set to 1"); 
		PASSERT5(0);
		denominator = 1;
	}
	UINT64 scaled_value = ( (UINT64)value * scale ) / denominator;
	UINT32 rest = (UINT32)( ( (UINT64)value * scale ) % denominator );

	char buff[ 32 ];
	sprintf( str_out, "%s.%02u", p_u64toa( scaled_value, buff ), rest );
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

/*---------------------------------------------------------------------------------------------
				percentageCalc 
---------------------------------------------------------------------------------------------*/

static void utoaStr(UINT32 val, PString& str)
{
	str.clear();
	str.appendUint(val);
}

static void percentageCalc(UINT32 times, UINT32 hands, PString& str_times, PString& str_hands, PString& str_percent)
{
	utoaStr(times, str_times);
	utoaStr(hands, str_hands);

	UINT32 percent = 0;
	if(hands)
		percent = (times*100 + hands/2) / hands; // Check %100 TBD later!!!

	utoaStr(percent, str_percent);
}

/*---------------------------------------------------------------------------------------------
				formatGameHeader 
---------------------------------------------------------------------------------------------*/
static void formatGameHeader(int handType, bool isOneToOne, bool hiLo, int chipsType, Statistics::StructureType structureType, int locale, PString& ret)
{
	// TODO: this array most likely belongs elsewhere
	const PMsgId msg_Id[ UniqueHandType_NextAfterLast ] =
	{
		P_TXT( TXTHHY_EMPTY_STUB ), // None
		P_TXT( TXTHHY_X040Badugi ),
		P_TXT( TXTHHY_X040HoldXem ), // P_TXT( TXT_HISTORY_HOLDEM )
		P_TXT( TXTHHY_X0405_Card_Omaha ),
		P_TXT( TXTHHY_X040Omaha ), // P_TXT( TXT_HISTORY_OMAHA )
		P_TXT( TXTHHY_X0405_Card_Draw ), // P_TXT( TXT_HISTORY_DRAW )
		P_TXT( TXTHHY_X040Single_Draw_2_7_Lowball ),//" Single Draw 2-7 Lowball",
		P_TXT( TXTHHY_X0407_Card_Stud ), // P_TXT( TXT_HISTORY_STUD )
		P_TXT( TXTHHY_X040Razz ), // P_TXT( TXT_HISTORY_RAZZ )
		P_TXT( TXTHHY_X040Triple_Draw_2_7_Lowball ), // P_TXT( TXT_HISTORY_TRIPLEDRAW_27 )
		P_TXT( TXTHHY_X040Triple_Draw_A_5 ),
		P_TXT( TXTHHY_X040Courchevel ),
		P_TXT( TXTHHY_EMPTY_STUB ), // 6-card omaha - no copy for this yet
		P_TXT( TXTHHY_Split_HoldXem ),
		P_TXT( TXTHHY_Split_Omaha ),
		P_TXT( TXTHHY_Showtime_HoldXem ),
		P_TXT( TXTHHY_Showtime_Omaha ),
		P_TXT( TXTHHY_Fusion ),
		P_TXT( TXTHHY_Unfold_Holdem ),
		P_TXT( TXTHHY_EMPTY_STUB ), // unfold max - no copy for this yet
		P_TXT( TXTHHY_6X_HoldXem ),
		P_TXT( TXTHHY_Deep_Water_HoldXem ),
		P_TXT( TXTHHY_Deep_Water_Omaha ),
		P_TXT( TXTHHY_Swap_HoldXem ),
		P_TXT( TXTHHY_Tempest_HoldXem ),
	};
	PASSERT( handType >= 0 && handType < UniqueHandType_NextAfterLast );

	PMsg( ret, LocalesTable, locale, P_TXT( TXTHHY_XX ) );// *** // P_TXT( TXT_STATISTIC_STARSDELIM )
	i18nAppendChar( ret, '\n' );
	i18nAppendChar( ret, ' ' ); //PYR-25497

	PMsg( ret, LocalesTable, locale, msg_Id[ handType ] ); //Hold'em | Omaha | Stud
	if( hiLo )
	{
		PMsg( ret, LocalesTable, locale, P_TXT( TXTHHY_X040Hi_Lo ) ); // P_TXT( TXT_HISTORY_HILO )
	}

	if( structureType == Statistics::Structure_ButtonBlind )
	{
		PMsg( ret, LocalesTable, locale, P_TXT( " with button blind" ) ); // TODO: localization
	}
	else if( structureType == Statistics::Structure_GiantBlind )
	{
		PMsg( ret, LocalesTable, locale, P_TXT( " with giant blind" ) ); // TODO: localization
	}

	PString oneOnOne;
	if( isOneToOne )	// bool isOneOnOne
	{
		PMsg( oneOnOne, LocalesTable, locale, P_TXT( TXTHHY_1_on_1 ) ); // P_TXT( TXT_STATISTIC_ONEONONE )
	}

	PString money;
	if( chipsType == PLAY_CHIPS )	//bool isPlayMoney
		PMsg( money, LocalesTable, locale, P_TXT( TXTHHY_Play_Money ) ); // P_TXT( TXT_STATISTIC_PLAYMONEY )
	else if( chipsType == 0 )	// real money
		PMsg( money, LocalesTable, locale, P_TXT( TXTHHY_Real_Money ) ); // P_TXT( TXT_STATISTIC_REALMONEY )
	//else if( chipsType == 2 ) // any tournament: no specific text for now, perhaps TBD...  
	//	money = "";  do nothing
	if( *money && *oneOnOne )
	{
		PMsg( money, LocalesTable, locale, P_TXT( TXTHHY_X_1 ) ); // P_TXT( TXT_DEALERCHAT_COMMA )
		PMsg( money, LocalesTable, locale, P_TXT( TXTHHY_X040 ) ); // P_TXT( TXT_DEALERCHAT_SPACE )
	}
	//STATISTIC_PLAYMONEY Play Money
	//STATISTIC_REALMONEY Real Money
	//STATISTIC_ONEONONE 1-on-1
	//STATISTIC_MONEY1ON1  (%0%1):
	if( *money || *oneOnOne )
	{
		PMsg2( ret, LocalesTable, locale, P_TXT( TXTHHY_X040XN0N1XX ), money, oneOnOne ); // P_TXT( TXT_STATISTIC_MONEY1ON1 )
	}
	else // Bug #4500:
	{
		i18nAppendChar( ret, ':' );
	}

	i18nAppend( ret, "\n\n" );
}

/*---------------------------------------------------------------------------------------------
				formatHoldemHistory 
---------------------------------------------------------------------------------------------*/
static void formatHoldemHistory(const Statistics& stats, int handType, bool oneToOne, bool hiLo, int chipsType, Statistics::StructureType structureType, UINT32 locale, PString& ret, bool printHeader = false)
{
	const Statistics::HoldemStats* stat = stats.getStatHoldem( static_cast< BYTE >( handType ), hiLo, oneToOne, static_cast< BYTE >( chipsType ), structureType );

	if( !( stat->HandFlag ) ) 
		return;

	if( printHeader )
		formatGameHeader(handType, oneToOne, hiLo, chipsType, structureType, locale, ret);

	PString str_times, str_hands, str_percent;
	PString sawflop;
		
	utoaStr(stat->HandFlag, str_hands);
	PMsg(sawflop, LocalesTable, locale, P_TXT( TXTHHY_X040and_saw_flop )); // P_TXT( TXT_STATISTIC_SAWFLOP )
	PMsg2(ret, LocalesTable, locale, P_TXT( TXTHHY_N0_hands_playedN1X ), str_hands, sawflop); // P_TXT( TXT_STATISTIC_HANDSPLAYED )
	i18nAppendChar(ret, '\n');

	if( structureType == Statistics::Structure_ButtonBlind )
	{
		percentageCalc( stat->SeenFlop_BB_pos, stat->BB_posted, str_times, str_hands, str_percent );
		PMsg2( ret, LocalesTable, locale, P_TXT( "\040- %0 times out of %1 while in blind" ), str_times, str_hands ); // P_TXT( TXT_STATISTIC_BLIND )
		// Calculate and add needed spaces: TBD
		i18nAppend( ret, "      " );
		PMsg1( ret, LocalesTable, locale, P_TXT( TXTHHY_XN0NNX ), str_percent ); // P_TXT( TXT_STATISTIC_PERCENTAGE )
		i18nAppendChar( ret, '\n' );
	}
	else
	{
		percentageCalc( stat->SeenFlop_SB_pos, stat->SB_posted, str_times, str_hands, str_percent );
		PMsg2( ret, LocalesTable, locale, P_TXT( TXTHHY_X040__N0_times_out_of_N1_while_ELL ), str_times, str_hands ); // P_TXT( TXT_STATISTIC_SMALLBLIND )
		// Calculate and add needed spaces: TBD
		i18nAppend( ret, "      " );
		PMsg1( ret, LocalesTable, locale, P_TXT( TXTHHY_XN0NNX ), str_percent ); // P_TXT( TXT_STATISTIC_PERCENTAGE )
		i18nAppendChar( ret, '\n' );

		percentageCalc( stat->SeenFlop_BB_pos, stat->BB_posted, str_times, str_hands, str_percent );
		PMsg2( ret, LocalesTable, locale, P_TXT( TXTHHY_X040__N0_times_out_of_N1_while_ELL_1 ), str_times, str_hands ); // P_TXT( TXT_STATISTIC_BIGBLIND )
		// Calculate and add needed spaces: TBD
		i18nAppend( ret, "      " );
		PMsg1( ret, LocalesTable, locale, P_TXT( TXTHHY_XN0NNX ), str_percent ); // P_TXT( TXT_STATISTIC_PERCENTAGE )
		i18nAppendChar( ret, '\n' );
	
		if( structureType == Statistics::Structure_GiantBlind )
		{
			percentageCalc( stat->SeenFlop_GB_pos, stat->GB_posted, str_times, str_hands, str_percent );
			PMsg2( ret, LocalesTable, locale, P_TXT( "\040 - %0 times out of %1 while in giant blind" ), str_times, str_hands ); // P_TXT( TXT_STATISTIC_GIANTBLIND )
			// Calculate and add needed spaces: TBD
			i18nAppend( ret, "      " );
			PMsg1( ret, LocalesTable, locale, P_TXT( TXTHHY_XN0NNX ), str_percent ); // P_TXT( TXT_STATISTIC_PERCENTAGE )
			i18nAppendChar( ret, '\n' );
		}
	}

	percentageCalc( stat->SeenFlop_Other_pos, stat->Other_Pos, str_times, str_hands, str_percent );
	PMsg2(ret, LocalesTable, locale, P_TXT( TXTHHY_X040__N0_times_out_of_N1_in_ot_ELL ), str_times, str_hands); // P_TXT( TXT_STATISTIC_OTHERPOS )
	// Calculate and add needed spaces: TBD
	i18nAppend(ret, "      ");
	PMsg1(ret, LocalesTable, locale, P_TXT( TXTHHY_XN0NNX ), str_percent); // P_TXT( TXT_STATISTIC_PERCENTAGE )
	i18nAppendChar(ret, '\n');

	percentageCalc( stat->SeenFlop, stat->HandFlag, str_times, str_hands, str_percent );
	PMsg2(ret, LocalesTable, locale, P_TXT( TXTHHY_X040__a_total_of_N0_times_out__ELL ), str_times, str_hands); // P_TXT( TXT_STATISTIC_TOTAL )
	// Calculate and add needed spaces: TBD
	i18nAppend(ret, "      ");
	PMsg1(ret, LocalesTable, locale, P_TXT( TXTHHY_XN0NNX ), str_percent); // P_TXT( TXT_STATISTIC_PERCENTAGE )
	i18nAppendChar(ret, '\n');

	i18nAppendChar(ret, '\n');
	percentageCalc( stat->BeenWinner, stat->SeenShowdown, str_times, str_hands, str_percent );
	PMsg2(ret, LocalesTable, locale, P_TXT( TXTHHY_X040Pots_won_at_showdown___N0__ELL ), str_times, str_hands); // P_TXT( TXT_STATISTIC_WINSPOT )

	// Calculate and add needed spaces: TBD
	i18nAppend(ret, "      ");
	PMsg1(ret, LocalesTable, locale, P_TXT( TXTHHY_XN0NNX ), str_percent); // P_TXT( TXT_STATISTIC_PERCENTAGE )
	i18nAppendChar(ret, '\n');
		
	utoaStr(stat->BeenWinnerNoShowdown, str_times);
	PMsg1(ret, LocalesTable, locale, P_TXT( TXTHHY_X040Pots_won_without_showdown__ELL ), str_times); // P_TXT( TXT_STATISTIC_WINSPOT_NOSHOWDOWN )
	i18nAppendChar(ret, '\n');
}

/*---------------------------------------------------------------------------------------------
				formatDrawHistory 
---------------------------------------------------------------------------------------------*/
static void formatDrawHistory(const Statistics& stats, int handType, bool oneToOne, bool hiLo, int chipsType, Statistics::StructureType structureType, UINT32 locale, PString& ret, bool printHeader = false)
{
	const Statistics::DrawStats* stat = stats.getStatDraw( static_cast< BYTE >( handType ), hiLo, oneToOne, static_cast< BYTE >( chipsType ), structureType );

	if( !( stat->HandFlag ) ) 
		return;

	if( printHeader )
		formatGameHeader( handType, oneToOne, hiLo, chipsType, structureType, locale, ret );

	PString str_times, str_hands, str_percent;

	utoaStr(stat->HandFlag, str_hands);
	PMsg2(ret, LocalesTable, locale, P_TXT( TXTHHY_N0_hands_playedN1X ), str_hands, ""); // P_TXT( TXT_STATISTIC_HANDSPLAYED )
	i18nAppendChar(ret, '\n');

	percentageCalc( stat->CardsDrawn, stat->HandFlag, str_times, str_hands, str_percent );
	PMsg1(ret, LocalesTable, locale, P_TXT( TXTHHY_X040__cards_drawn_N0_times ), str_times); // P_TXT( TXT_STATISTIC_CARDSDRAWN )
	// Calculate and add needed spaces: TBD
	i18nAppend(ret, "      ");
	PMsg1(ret, LocalesTable, locale, P_TXT( TXTHHY_XN0NNX ), str_percent); // P_TXT( TXT_STATISTIC_PERCENTAGE )
	i18nAppendChar(ret, '\n');

	percentageCalc( stat->BeenWinnerIfDrawn, stat->CardsDrawn, str_times, str_hands, str_percent );
	PMsg1(ret, LocalesTable, locale, P_TXT( TXTHHY_X040__won_when_drawing_N0_time_ELL ), str_times); // P_TXT( TXT_STATISTIC_WON_IFDRAWING )
	// Calculate and add needed spaces: TBD
	i18nAppend(ret, "      ");
	PMsg1(ret, LocalesTable, locale, P_TXT( TXTHHY_XN0NNX ), str_percent); // P_TXT( TXT_STATISTIC_PERCENTAGE )
	i18nAppendChar(ret, '\n');

	i18nAppendChar(ret, '\n');
	percentageCalc( stat->BeenWinner, stat->SeenShowdown, str_times, str_hands, str_percent );
	PMsg2(ret, LocalesTable, locale, P_TXT( TXTHHY_X040Pots_won_at_showdown___N0__ELL ), str_times, str_hands); // P_TXT( TXT_STATISTIC_WINSPOT )

	// Calculate and add needed spaces: TBD
	i18nAppend(ret, "      ");
	PMsg1(ret, LocalesTable, locale, P_TXT( TXTHHY_XN0NNX ), str_percent); // P_TXT( TXT_STATISTIC_PERCENTAGE )
	i18nAppendChar(ret, '\n');

	utoaStr(stat->BeenWinnerNoShowdown, str_times);
	PMsg1(ret, LocalesTable, locale, P_TXT( TXTHHY_X040Pots_won_without_showdown__ELL ), str_times); // P_TXT( TXT_STATISTIC_WINSPOT_NOSHOWDOWN )
	i18nAppendChar(ret, '\n');
}

/*---------------------------------------------------------------------------------------------
				formatStudHistory 
---------------------------------------------------------------------------------------------*/
static void formatStudHistory(const Statistics& stats, int handType, bool oneToOne, bool hiLo, int chipsType, Statistics::StructureType structureType, UINT32 locale, PString& ret, bool printHeader = false)
{
	const Statistics::StudStats* stat = stats.getStatStud( static_cast< BYTE >( handType ), hiLo, oneToOne, static_cast< BYTE >( chipsType ), structureType );

	if( !( stat->HandFlag ) ) 
		return;

	if( printHeader )
		formatGameHeader(handType, oneToOne, hiLo, chipsType, structureType, locale, ret);

	PString str_times, str_hands, str_percent;
		
	utoaStr(stat->HandFlag, str_hands);
	PMsg2(ret, LocalesTable, locale, P_TXT( TXTHHY_N0_hands_playedN1X ), str_hands, ""); // P_TXT( TXT_STATISTIC_HANDSPLAYED )
	i18nAppendChar(ret, '\n');

	percentageCalc( stat->Street_4th, stat->HandFlag, str_times, str_hands, str_percent );
	PMsg1(ret, LocalesTable, locale, P_TXT( TXTHHY_X040__saw_fourth_street_N0_tim_ELL ), str_times); // P_TXT( TXT_STATISTIC_4STREET )
	// Calculate and add needed spaces: TBD
	i18nAppend(ret, "      ");
	PMsg1(ret, LocalesTable, locale, P_TXT( TXTHHY_XN0NNX ), str_percent); // P_TXT( TXT_STATISTIC_PERCENTAGE )
	i18nAppendChar(ret, '\n');

	percentageCalc( stat->Street_5th, stat->HandFlag, str_times, str_hands, str_percent );
	PMsg1(ret, LocalesTable, locale, P_TXT( TXTHHY_X040__saw_fifth_street_N0_time_ELL ), str_times); // P_TXT( TXT_STATISTIC_5STREET )
	// Calculate and add needed spaces: TBD
	i18nAppend(ret, "      ");
	PMsg1(ret, LocalesTable, locale, P_TXT( TXTHHY_XN0NNX ), str_percent); // P_TXT( TXT_STATISTIC_PERCENTAGE )
	i18nAppendChar(ret, '\n');

	percentageCalc( stat->Street_6th, stat->HandFlag, str_times, str_hands, str_percent );
	PMsg1(ret, LocalesTable, locale, P_TXT( TXTHHY_X040__saw_sixth_street_N0_time_ELL ), str_times); // P_TXT( TXT_STATISTIC_6STREET )
	// Calculate and add needed spaces: TBD
	i18nAppend(ret, "      ");
	PMsg1(ret, LocalesTable, locale, P_TXT( TXTHHY_XN0NNX ), str_percent); // P_TXT( TXT_STATISTIC_PERCENTAGE )
	i18nAppendChar(ret, '\n');

	percentageCalc( stat->SeenShowdown, stat->HandFlag, str_times, str_hands, str_percent );
	PMsg1(ret, LocalesTable, locale, P_TXT( TXTHHY_X040__reached_showdown_N0_time_ELL ), str_times); // P_TXT( TXT_STATISTIC_REACHED )
	// Calculate and add needed spaces: TBD
	i18nAppend(ret, "      ");
	PMsg1(ret, LocalesTable, locale, P_TXT( TXTHHY_XN0NNX ), str_percent); // P_TXT( TXT_STATISTIC_PERCENTAGE )
	i18nAppendChar(ret, '\n');

	i18nAppendChar(ret, '\n');
	percentageCalc( stat->BeenWinner, stat->SeenShowdown, str_times, str_hands, str_percent );
	PMsg2(ret, LocalesTable, locale, P_TXT( TXTHHY_X040Pots_won_at_showdown___N0__ELL ), str_times, str_hands); // P_TXT( TXT_STATISTIC_WINSPOT )

	// Calculate and add needed spaces: TBD
	i18nAppend(ret, "      ");
	PMsg1(ret, LocalesTable, locale, P_TXT( TXTHHY_XN0NNX ), str_percent); // P_TXT( TXT_STATISTIC_PERCENTAGE )
	i18nAppendChar(ret, '\n');
		
	utoaStr(stat->BeenWinnerNoShowdown, str_times);
	PMsg1(ret, LocalesTable, locale, P_TXT( TXTHHY_X040Pots_won_without_showdown__ELL ), str_times); // P_TXT( TXT_STATISTIC_WINSPOT_NOSHOWDOWN )
	i18nAppendChar(ret, '\n');
}

/*---------------------------------------------------------------------------------------------
				formatStatHeader 
---------------------------------------------------------------------------------------------*/
void formatStatHeader( PString& stat, UINT32 locale, UINT32 brandId, const char* userId, const char* userEmail, UINT32 numRequested, UINT32 requestContents, statsRequestType requestType)
{
	PString str_games, str_type, str_gameortour ;
	PString str_buff;
	bool nonconsistent = false;
	switch( requestType )
	{
		case Hands_stats:
		{
			if( numRequested == 1 )
				PMsg( str_gameortour, LocalesTable, locale, P_TXT( TXTHHY_X040hand ) ); // P_TXT( TXT_STATISTIC_HEADER_GAME )
			else
				PMsg( str_gameortour, LocalesTable, locale, P_TXT( TXTHHY_X040hands ) ); // P_TXT( TXT_HISTORY_HEADER_GAMES )

			if( !numRequested )
			{
				PMsg( str_type, LocalesTable, locale, P_TXT( TXTHHY_your_all ) ); // P_TXT( TXT_STATISTIC_HEADER_ALL )
				i18nAppend(str_type, str_gameortour);
			}
			else
			{
				utoaStr( requestContents, str_games );
				// HISTORY_HEADER_LASTHANDS last %0 games 
				PMsg2( str_type, LocalesTable, locale, P_TXT( TXTHHY_your_last_N0N1 ), str_games, str_gameortour ); // P_TXT( TXT_HISTORY_HEADER_LASTN )
				if( numRequested != requestContents ) 
					nonconsistent = true;
			}

			PString brandName;
			i18nFormatBrandedName( brandName, LocalesTable, locale, brandId, eBrandedName_Brand, false );
			PMsg4( stat, LocalesTable, locale, P_TXT( TXTHHY_Here_are_your_N3_Game_Statisti_ELL ), str_type, userId, userEmail, brandName ); // P_TXT( TXT_STATISTIC_HEADER )
		}
		break;

		case ID_tourn_stats:
		{ // Tournament #number
			utoaStr( numRequested, str_buff );
			//HISTORY_HEADER_TOURN Tournament #%0,
			PMsg1( str_type, LocalesTable, locale, P_TXT( TXTHHY_Tournament_XN0 ), str_buff ); // P_TXT( TXT_HISTORY_HEADER_TOURN )
			PMsg3( stat, LocalesTable, locale, P_TXT( TXTHHY_Tournament_History_for_N0_requ_ELL ), str_type, userId, userEmail ); // P_TXT( TXT_TOURNSTAT_HEADER )
		}
		break;

		case N_tourns_stats:
		{ // n Tournaments
			if( requestContents == 1 )
				PMsg( str_gameortour, LocalesTable, locale, P_TXT( TXTHHY_X040tournament ) ); // P_TXT( TXT_STATISTIC_HEADER_TOURNAMRNT )
			else
				PMsg( str_gameortour, LocalesTable, locale, P_TXT( TXTHHY_X040tournaments ) ); // P_TXT( TXT_STATISTIC_HEADER_TOURNAMRNTS )

			if( !requestContents )
			{
				PMsg( str_type, LocalesTable, locale, P_TXT( TXTHHY_your_all ) ); // P_TXT( TXT_STATISTIC_HEADER_ALL )
				i18nAppend(str_type, str_gameortour);
			}
			else
			{
				utoaStr( requestContents, str_games );
				// HISTORY_HEADER_LASTIN last %0 tournaments 
				PMsg2( str_type, LocalesTable, locale, P_TXT( TXTHHY_your_last_N0N1 ), str_games, str_gameortour ); // P_TXT( TXT_HISTORY_HEADER_LASTN )
				if( numRequested != requestContents ) 
					nonconsistent = true;
			}
			PMsg3( stat, LocalesTable, locale, P_TXT( TXTHHY_Tournament_History_for_N0_requ_ELL ), str_type, userId, userEmail ); // P_TXT( TXT_TOURNSTAT_HEADER )
		}
		break;

		default:
			PLog( "formatStatHeader::Nondefined statistic request-'%d'", requestType );
	}

	// HISTORY_HEADER PokerStars Game Statistics for %0 requested by %1(%2)
	//PMsg3( stat, msgTable, P_TXT( TXT_STATISTIC_HEADER ), str_type, userId, userEmail ); // P_TXT( TXTHHY_Here_are_your_PokerStars_Game__ELL )
	//PMsg3( stat, msgTable, P_TXT( TXT_TOURNSTAT_HEADER ), str_type, userId, userEmail ); // P_TXT( TXTHHY_Tournament_History_for_N0_requ_ELL )
	if( nonconsistent )
	{ //HISTORY_HEADER_REQUESTED . Your request was %0, however you have only played %1%2  
		PString str_buff1;
		utoaStr( numRequested, str_buff1 );
		PMsg3( stat, LocalesTable, locale, P_TXT( TXTHHY__Your_request_was_for_N0X_howe_ELL ), str_buff1, str_games, str_gameortour ); // P_TXT( TXT_HISTORY_HEADER_REQUESTED )
		//if( ! requestContents ) // #2602
		//	stat.append( "\nPlease note that play money hands are not stored to the database, and therefore statistics cannot be computed or retrieved for play money games.\n" );
	}
	i18nAppendChar(stat, '\n');
}
/*
static enum statsRequestType
	{
	Non_defined = 0,
	Hands_stats,
	N_tourns_stats,
	ID_tourn_stats
	};
//---------------------------------------------------------------------------------------------
//				formatTournStatHeader 
//---------------------------------------------------------------------------------------------
void formatTournStatHeader( 
	const char* userId, 
	const char* userEmail, 
	PString& stat,			// string to format
	BYTE requestType,		// particular tournId or N last tourns
	UINT32 tournIdOrNumTourns,	// Id of particular tournament OR number of tourns requested( if N last tourns )
	UINT32 numTourns		// number of tourns returned( in any case )
	);
*/
void formatTournStatHeader( PString& stat, UINT32 locale, UINT32 brandId, const char* userId, const char* userEmail, BYTE requestType, UINT32 numRequested, UINT32 requestContents )
{
	try
	{ // dbm.h:
		statsRequestType type;

		switch( requestType )
		{
			case TOURN_STATS_TOURN_ID:
			case TOURN_STATS_TOURN_ID_NOSTAT:
				type = ID_tourn_stats;
			break;
			case TOURN_STATS_NUM_TOURN:
			case TOURN_STATS_NUM_TOURN_NOSTAT:
				type = N_tourns_stats;
			break;
			default:
				type = Non_defined;
		}
		formatStatHeader( stat, locale, brandId, userId, userEmail, numRequested, requestContents, type /*BYTE requestType*/ );
	}
	catch( const PError& er )
	{
		PLog( "FormatTournStatHeader::%s", er.why() );
	}
}

/*---------------------------------------------------------------------------------------------
				formatStatSubject 
---------------------------------------------------------------------------------------------*/
void formatStatSubject( PString& subject, UINT32 locale, UINT32 brandId, BYTE requestType, UINT32 requestContents )
	{
	try
		{
		//statsRequestType type;
		PString str_buff;

		PString brandName;
		i18nFormatBrandedName( brandName, LocalesTable, locale, brandId, eBrandedName_Brand, false );
		PMsg1( subject, LocalesTable, locale, P_TXT( TXTHHY_N0_Tournament_History_Request__ELL ), brandName ); // P_TXT( TXT_STATISTIC_SUBJECT )

		switch( requestType )
			{
			case TOURN_STATS_NUM_TOURN:
			case TOURN_STATS_NUM_TOURN_NOSTAT:
				{
				PString str_tourns;
				//HISTORY_HEADER_LASTN last %0%1
				//STATISTIC_HEADER_TOURNAMRNT  tournament
				//STATISTIC_HEADER_TOURNAMRNTS  tournaments
				if( requestContents != 1 )
					{
					utoaStr( requestContents, str_buff );
					PMsg1( subject, LocalesTable, locale, P_TXT( TXTHHY_Last_N0_tournaments ), str_buff );
					}
				else
					PMsg( subject, LocalesTable, locale, P_TXT( TXTHHY_Last_1_tournament ) );
				}
			break;

			case TOURN_STATS_TOURN_ID:
			case TOURN_STATS_TOURN_ID_NOSTAT:
				utoaStr( requestContents, str_buff );
				PMsg1( subject, LocalesTable, locale, P_TXT( TXTHHY_Tournament_N0 ), str_buff ); // P_TXT( TXT_HISTORY_SUBJECT_TOURNID )
			break;

			default:
				PLog( "statHistory.cpp::ERROR:Unknown REQUEST_TYPE: %d - ignored", requestType );
//			type = Non_defined;
			}
		}
	catch( const PError& er )
		{
		PLog( "FormatStatSubject::%s", er.why() );
		}
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
				formatTournHeader 
---------------------------------------------------------------------------------------------*/

static void formatCurrencyShort( PString& res, const char* currencyCode )
{
	res = "";
	PUNICHAR uc = 0;
	if( ( 0 == currencyCode ) || ( 0 == *currencyCode ) )
		uc = '$';
	else if( PString::equals( currencyCode, "USD" ) )
		uc = '$';
	else if( PString::equals( currencyCode, "EUR" ) )
		uc = getEuroSignUni();
	else if( PString::equals( currencyCode, "GBP" ) )
		uc = getPoundSignUni();
	else if( PString::equals( currencyCode, "CAD" ) )
		uc = '$';
	else if( PString::equals( currencyCode, "INR" ) ) // PYR-60629
		uc = getINRSignUni();

	if( uc )
		i18nAppendChar( res, uc );
}

static void formatCurrencyLong( PString& res, const char* currencyCode )
{
	res = "";
	if( ( 0 == currencyCode ) || ( 0 == *currencyCode ) )
		i18nAppend( res, "USD" );
	else
		i18nAppend( res, currencyCode );
}

void formatTournHeaderEx( PString& ret, UINT32 locale, UINT32 brandId, UINT32 timezone, const TournHeader& hdr, const HeaderFormatOption& info )
{ // Header:
	char str_buff[ 32 ] = "";
	PString buff_str; 

	i18nFormatBrandedName( ret, LocalesTable, locale, brandId, eBrandedName_Brand, false );
	i18nAppendChar( ret, ' ' );

	p_utoa( hdr.tournID, str_buff );
	PMsg1( ret, LocalesTable, locale, P_TXT( TXTHHY_Tournament_XN0 ), str_buff );  // P_TXT( TXT_HISTORY_HEADER_TOURN )
	PMsg( ret, LocalesTable, locale, P_TXT( TXTHHY_X_1 ) ); // P_TXT( TXT_DEALERCHAT_COMMA )

	if( hdr.game < MultiGameTypeShift )
	{
		if( hdr.structure == StructLimit )
			PMsg( ret, LocalesTable, locale, P_TXT( TXTHHY_X040Limit ) ); // P_TXT( TXT_HISTORY_LIMIT )
		else if( hdr.structure == StructPotLimit )
			PMsg( ret, LocalesTable, locale, P_TXT( TXTHHY_X040Pot_Limit ) ); // P_TXT( TXT_HISTORY_POTLIMIT )
		else if( hdr.structure == StructNoLimit )
			PMsg( ret, LocalesTable, locale, P_TXT( TXTHHY_X040No_Limit ) ); // P_TXT( TXT_HISTORY_NOLIMIT )
		else if( hdr.structure == StructPotNoLimit )
			PMsg( ret, LocalesTable, locale, P_TXT( TXTHHY_X040Pot_Limit_Pre_FlopX_No_Lim_ELL ) );
		else 
			PASSERT( 0 );
	}
	i18nAppendChar( ret, ' ' ); //PYR-25497
	if( hdr.game == HandTypeHoldem )
		PMsg( ret, LocalesTable, locale, P_TXT( TXTHHY_X040HoldXem ) ); // P_TXT( TXT_HISTORY_HOLDEM )
	else if( hdr.game == HandTypeOmaha )
		PMsg( ret, LocalesTable, locale, P_TXT( TXTHHY_X040Omaha ) ); // P_TXT( TXT_HISTORY_OMAHA )
	else if( hdr.game == HandTypeStud )
		PMsg( ret, LocalesTable, locale, P_TXT( TXTHHY_X0407_Card_Stud ) ); // P_TXT( TXT_HISTORY_STUD )
	else if( hdr.game == HandTypeRazz )
		PMsg( ret, LocalesTable, locale, P_TXT( TXTHHY_X040Razz ) ); // P_TXT( TXT_HISTORY_RAZZ )
	else if( hdr.game == HandTypeDraw )
		PMsg( ret, LocalesTable, locale, P_TXT( TXTHHY_X0405_Card_Draw ) ); // P_TXT( TXT_HISTORY_DRAW )
	else if( hdr.game == HandType3Draw27 )
		PMsg( ret, LocalesTable, locale, P_TXT( TXTHHY_X040Triple_Draw_2_7_Lowball ) ); // P_TXT( TXT_HISTORY_TRIPLEDRAW_27 )
	else if( hdr.game == HandTypeSingleDraw27 )
		PMsg( ret, LocalesTable, locale, P_TXT( TXTHHY_X040Single_Draw_2_7_Lowball ) ); // P_TXT( TXT_HISTORY_TRIPLEDRAW_27 )
	else if( hdr.game == HandTypeBadugi )
		PMsg( ret, LocalesTable, locale, P_TXT( TXTHHY_X040Badugi ) ); // P_TXT( TXT_HISTORY_BADUGI )
	else if( hdr.game == HandTypeSplitHoldem )
		PMsg( ret, LocalesTable, locale, P_TXT( TXTHHY_Split_HoldXem ) );
	else if( hdr.game == HandTypeSplitOmaha )
		PMsg( ret, LocalesTable, locale, P_TXT( TXTHHY_Split_Omaha ) );
	else if( hdr.game == HandTypeShowtimeHoldem )
		PMsg( ret, LocalesTable, locale, P_TXT( TXTHHY_Showtime_HoldXem ) );
	else if( hdr.game == HandTypeShowtimeOmaha )
		PMsg( ret, LocalesTable, locale, P_TXT( TXTHHY_Showtime_Omaha ) );
	else if( hdr.game == HandTypeFusion )
		PMsg( ret, LocalesTable, locale, P_TXT( TXTHHY_Fusion ) );
	else if( hdr.game == HandTypeSixPlusHoldem )
		PMsg( ret, LocalesTable, locale, P_TXT( TXTHHY_6X_HoldXem ) );
	else if( hdr.game == HandTypeSwapHoldem )
		PMsg( ret, LocalesTable, locale, P_TXT( TXTHHY_Swap_HoldXem ) );
	else if( hdr.game == HandTypeTempestHoldem )
		PMsg( ret, LocalesTable, locale, P_TXT( TXTHHY_Tempest_HoldXem ) );
	else if( hdr.game == MultiGameHose )
		PMsg( ret, LocalesTable, locale, P_TXT( TXTHHY_X040HOSE ) ); // P_TXT( TXT_HISTORY_HOSE )
	else if( hdr.game == MultiGameHorse )
		PMsg( ret, LocalesTable, locale, P_TXT( TXTHHY_X040HORSE ) ); // P_TXT( TXT_HISTORY_HORSE )
	else if( hdr.game == MultiGameEightGame )
		PMsg( ret, LocalesTable, locale, P_TXT( TXTHHY_X040EightGame ) );
	else if( hdr.game == MultiGameHoldemMixed )
		PMsg( ret, LocalesTable, locale, P_TXT( TXTHHY_X040HoldXemMixed ) );
	else if( hdr.game == MultiGameOmahaHiLoMixed )
		PMsg( ret, LocalesTable, locale, P_TXT( TXTHHY_X040OmahaHiLoMixed ) );
	else if( hdr.game == MultiGameHoldemOmahaPl )
		PMsg( ret, LocalesTable, locale, P_TXT( TXTHHY_X040Mixed_PLHXPLO ) );
	else if( hdr.game == MultiGameTripleStud )
		PMsg( ret, LocalesTable, locale, P_TXT( TXTHHY_X040Triple_Stud ) );
	else if( hdr.game == MultiGameHoldemNlOmaha )
		PMsg( ret, LocalesTable, locale, P_TXT( TXTHHY_X040Mixed_NLH_PLO ) );
	else if( hdr.game == HandType5Omaha )
		PMsg( ret, LocalesTable, locale, P_TXT( TXTHHY_X0405_Card_Omaha ) );
	else if( hdr.game == HandType6Omaha )
		PMsg( ret, LocalesTable, locale, P_TXT( TXTHHY_6_Card_Omaha ) );
	else if( hdr.game == HandTypeCourchevel )
		PMsg( ret, LocalesTable, locale, P_TXT( TXTHHY_X040Courchevel ) );
	else if( hdr.game == MultiGameOmahaMixed )
		PMsg( ret, LocalesTable, locale, P_TXT( TXTHHY_X040Mixed_Omaha ) );
	else 
		PASSERT( 0 );

	if( hdr.game < MultiGameTypeShift )
		if( hdr.hiLo )
			PMsg( ret, LocalesTable, locale, P_TXT( TXTHHY_X040Hi_Lo ) ); // P_TXT( TXT_HISTORY_HILO )

	i18nAppendChar(ret, '\n');

	PString currency, moneyStr, currString;
	formatCurrencyShort( currency, hdr.currency );	
	formatCurrencyLong( currString, hdr.currency );	
		 
	if( hdr.fppBuyIn )
		{
		sprintf( str_buff, "%u SC", (UINT32)hdr.fppBuyIn );
		PMsg1( buff_str, LocalesTable, locale, P_TXT( TXTHHY_Buy_InX_N0 ), str_buff ); // P_TXT( TXT_TOURNSTAT_BUYIN )
		}
	else
		{
		if( !( hdr.buyIn || hdr.rake 
			 || hdr.displayBuyIn || hdr.displayRake ) ) //PYR-20531
			{
			PMsg( ret, LocalesTable, locale, P_TXT( TXTHHY_FreerollX040 ) ); // P_TXT( TXT_TOURNSTAT_FREEROLL )
			}
		else
			{
			if( hdr.isPlayMoney )
				{
				_formatScaledUINT32( (hdr.buyIn - hdr.knockout), hdr.scalePM, 100, str_buff ); // PYR-25238
				//sprintf( str_buff, "%u", (hdr.buyIn - hdr.knockout) / 100 );
				moneyStr.append( str_buff );

				if( hdr.knockout )
					{
					_formatScaledUINT32( hdr.knockout, hdr.scalePM, 100, str_buff ); // PYR-25238
					//sprintf( str_buff, "%u", hdr.knockout / 100 );
					moneyStr.append("/").append( str_buff );
					}

				if( hdr.rake )
					{
					_formatScaledUINT32( hdr.rake, hdr.scalePM, 100, str_buff ); // PYR-25238
					//sprintf( str_buff, "%u", hdr.rake / 100 );
					moneyStr.append("/").append( str_buff );
					}
				}

			else // not play money - no scaling needed in this block
				{

				//PYR-20531 showing rake and buyin of initial flight for target flights
				UINT32 rake = hdr.rake;
				UINT32 buyIn = hdr.buyIn;
				if( !( buyIn || rake ) )
				{
					buyIn = hdr.displayBuyIn;
					rake = hdr.displayRake;
				}

				UINT32 buyInOnly = buyIn - hdr.knockout;  //PYR-20531 hdr. removed
				i18nAppend( moneyStr, currency );
				sprintf( str_buff, "%u.%02u", buyInOnly / 100, buyInOnly % 100 );
				i18nAppend( moneyStr, str_buff );

				if( hdr.knockout )
					{
					i18nAppendChar( moneyStr, '/' );
					i18nAppend( moneyStr, currency );
					sprintf( str_buff, "%u.%02u", UINT32(hdr.knockout / 100), UINT32(hdr.knockout % 100) );
					i18nAppend( moneyStr, str_buff );
					}

				if( rake ) //PYR-20531 hdr. removed
					{
					i18nAppendChar( moneyStr, '/' );
					i18nAppend( moneyStr, currency );
					sprintf( str_buff, "%u.%02u", rake / 100, rake % 100 ); //PYR-20531 hdr. removed
					i18nAppend( moneyStr, str_buff );
					}

				i18nAppendChar( moneyStr, ' ' );
				i18nAppend( moneyStr, currString );
				}

			PMsg1( buff_str, LocalesTable, locale, P_TXT( TXTHHY_Buy_InX_N0 ), moneyStr ); // P_TXT( TXT_TOURNSTAT_BUYIN )
			}
		}

	if( hdr.satelliteTarget )
		PMsg( ret, LocalesTable, locale, P_TXT( TXTHHY_Super_Satellite ) ); // P_TXT( TXT_TOURNSTAT_SUPERSATELLITE )

	if( ret[ ret.length() - 1 ] != '\n' ) 
		i18nAppendChar(ret, '\n');

	i18nAppend( ret, buff_str ); // Buy-In: $%0/$%1
	if( ret[ ret.length() - 1 ] != '\n' ) 
		i18nAppendChar(ret, '\n');

	if( hdr.players ) 
		{
		p_utoa(hdr.players, str_buff);
		PMsg1( ret, LocalesTable, locale, P_TXT( TXTHHY_N0_players ), str_buff ); // %0 players // P_TXT( TXT_TOURNSTAT_PLAYERS )
		i18nAppendChar(ret, '\n');
		}

	if( (hdr.prizeAddOn && ( hdr.awardSchema != PRIZE_ANOTHER_TOURN_FPP )) || (!hdr.isPlayMoney && (hdr.playerKnockoutAddOn > 0 || hdr.totalKnockoutAddOn > 0)) )
		{ //%0 added to the prize pool by PokerStars
		moneyStr = "";
		i18nAppend( moneyStr, currency );
		INT32 amount = 0; // PYR-42834

		if( hdr.prizeAddOn && ( hdr.awardSchema != PRIZE_ANOTHER_TOURN_FPP ) )
		{
			amount += hdr.prizeAddOn;
		}

		if( !hdr.isPlayMoney && (hdr.playerKnockoutAddOn > 0 || hdr.totalKnockoutAddOn > 0) )
		{
			amount += hdr.playerKnockoutAddOn * hdr.players + hdr.totalKnockoutAddOn;
		}

		_formatScaledTournChips( amount, hdr.scalePM, 100, str_buff ); // PYR-25238
		//sprintf( str_buff, "%u.%02u", hdr.prizeAddOn/100, hdr.prizeAddOn%100 ); 
		i18nAppend( moneyStr, str_buff );
		i18nAppendChar( moneyStr, ' ' );
		i18nAppend( moneyStr, currString );

		PString brandName;
		i18nFormatBrandedName( brandName, LocalesTable, locale, brandId, eBrandedName_Brand, false );
		PMsg2( ret, LocalesTable, locale, P_TXT( TXTHHY_N0_added_to_the_prize_pool_by__ELL_1 ), moneyStr, brandName );  // P_TXT( TXT_TOURNSTAT_PRIZEADDON )
		}

	if( ret[ ret.length() - 1 ] != '\n' ) 
		i18nAppendChar( ret, '\n' );

	//UINT32 prizePool = hdr.prizeAddOn + hdr.buyIn * hdr.players;

	if( *hdr.customPrizes.c_str() ) //PYR-33574
	{
		vector<PString> customPrizes; //PYR-33574
		splitString( hdr.customPrizes, customPrizes, '|', true, true );

		PString commaSeparatedString;
		for( UINT32 i = 0; i < customPrizes.size(); ++i )
		{
			commaSeparatedString.append( customPrizes[i] ).append( (i < customPrizes.size() - 1) ? ", ": "" );
		}

		PMsg1( ret, LocalesTable, locale, P_TXT( TXTHHY_Total_Prize_PoolX_N0X040 ), commaSeparatedString.c_str() );   // P_TXT( TXT_TOURNSTAT_PRIZEPOOL )
	}
	else
	{
		UINT32 _prizePool = hdr.prizePool;
		if( _prizePool )
			{
			moneyStr = "";

			if( hdr.awardSchema == PRIZE_ANOTHER_TOURN_FPP )
				{
				sprintf( str_buff, "%u SC", _prizePool );
				moneyStr.append( str_buff );
				}
			else if( hdr.isPlayMoney )
				{
				_formatScaledUINT32( _prizePool, hdr.scalePM, 100, str_buff ); // PYR-25238
				//p_utoa( _prizePool/100, str_buff );
				moneyStr.append( str_buff );
				}
			else
				{
				i18nAppend( moneyStr, currency );
				sprintf( str_buff, "%u.%02u", _prizePool/100, _prizePool%100 );
				i18nAppend( moneyStr, str_buff );
				i18nAppendChar( moneyStr, ' ' );
				i18nAppend( moneyStr, currString );
				}
			
			PMsg1( ret, LocalesTable, locale, P_TXT( TXTHHY_Total_Prize_PoolX_N0X040 ), moneyStr );   // P_TXT( TXT_TOURNSTAT_PRIZEPOOL )
			}
	}
	
	if( ret[ ret.length() - 1 ] != '\n' ) 
		i18nAppendChar( ret, '\n' );

	// PYR-59874
	if( !info.ignoreFreeBet && !hdr.freeBetReward.base.isNull() )
	{
		PString fbCampaignStr, fbAwardStr;
		fbCampaignStr.appendUint64(hdr.freeBetReward.base.campaignId);
				
		INT32 stake = hdr.freeBetReward.awarded.amount ? hdr.freeBetReward.awarded.amount : hdr.freeBetReward.base.stake;
		PString fbCurrency = hdr.freeBetReward.awarded.amount ? hdr.freeBetReward.awarded.currency : hdr.freeBetReward.base.currency;

		i18nFormatMoneyWithCurrency(fbAwardStr, LocalesTable, locale, stake, fbCurrency,
			eMoneyTypeCurr, eFormatCurrTypeSignWithCode, true /*showZeroCents*/, true/*showTriplets*/);

		// FreeBet #45684 Value: $50.00 USD
		PMsg2( ret, LocalesTable, locale, P_TXT( TXTHHY_FreeBet_XN0_ValueX_N1 ), fbCampaignStr, fbAwardStr );

		if( ret[ ret.length() - 1 ] != '\n' ) 
			i18nAppendChar( ret, '\n' );
	}

	if( hdr.satelliteTarget )
		{ //Target Tournament #%0
		moneyStr = "";

		p_utoa( hdr.satelliteTarget, str_buff );
		PMsg1( ret, LocalesTable, locale, P_TXT( TXTHHY_Target_Tournament_XN0X040 ), str_buff ); // P_TXT( TXT_TOURNSTAT_TARGET )

		if( ( hdr.satelliteBuyIn || hdr.satelliteRake ) && ( hdr.awardSchema != PRIZE_ANOTHER_TOURN_FPP ) )
			{
			if( *hdr.satelliteCurrency )
				{ //Bug #12750:
				formatCurrencyShort( currency, hdr.satelliteCurrency );
				formatCurrencyLong( currString, hdr.satelliteCurrency );
				}

			i18nAppend( moneyStr, currency );
			if( hdr.satelliteRake )
				{
				// working on the assumption that satellite scale is the same as target
				_formatScaledTournChips( hdr.satelliteBuyIn, hdr.scalePM, 100, str_buff ); // PYR-25238
				//sprintf( str_buff, "%u.%02u/", hdr.satelliteBuyIn/100, hdr.satelliteBuyIn%100 ); 
				i18nAppend( moneyStr, str_buff );
				i18nAppend( moneyStr, "/" );
				_formatScaledTournChips( hdr.satelliteRake, hdr.scalePM, 100, str_buff ); // PYR-25238
				//sprintf( str_buff, "%u.%02u", hdr.satelliteRake/100, hdr.satelliteRake%100 ); 
				}
			else
				{
				_formatScaledTournChips( hdr.satelliteBuyIn, hdr.scalePM, 100, str_buff ); // PYR-25238
				//sprintf( str_buff, "%u.%02u", hdr.satelliteBuyIn/100, hdr.satelliteBuyIn%100 );
				}
			i18nAppend( moneyStr, str_buff );
			i18nAppendChar( moneyStr, ' ' );
			i18nAppend( moneyStr, currString );
			PMsg1( ret, LocalesTable, locale, P_TXT( TXTHHY_Buy_InX_N0 ), moneyStr ); // P_TXT( TXT_TOURNSTAT_BUYIN )
			}

		if( ret[ ret.length() - 1 ] != '\n' )
			i18nAppendChar(ret, '\n');
		// calculate number of tickets:

		if( hdr.totalNextTourn )
			{ //%0 tickets to the target tournament
			p_utoa(hdr.totalNextTourn, str_buff);
			PMsg1( ret, LocalesTable, locale, P_TXT( TXTHHY_N0_tickets_to_the_target_tourn_ELL ), str_buff ); // P_TXT( TXT_TOURNSTAT_TICKETS )
			}
		}

	if( ret[ ret.length() - 1 ] != '\n' ) 
		i18nAppend(ret, "\n\n");
	/*
	char str_buff1[ 15 ] = "", str_buff2[ 15 ] = "";
	//Tournament started%0
	sprintf( str_buff, "%4d", hdr.started._year ); 
	sprintf( str_buff1, "%02d", hdr.started._month ); 
	sprintf( str_buff2, "%02d", hdr.started._day ); 

	buff_str = "";
	PMsg3( buff_str, LocalesTable, locale, P_TXT( TXTHHY_X040__N0_N1_N2 ), str_buff, str_buff1, str_buff2 ); // P_TXT( TXT_HISTORY_DATE )
	//PMsg1( ret, LocalesTable, locale, P_TXT( TXTHHY_Tournament_startedN0 ), buff_str ); // P_TXT( TXT_TOURNSTAT_STARTED )

	sprintf( str_buff, "%02d", hdr.started._hour ); 
	sprintf( str_buff1, "%02d", hdr.started._minute ); 
	sprintf( str_buff2, "%02d", hdr.started._second ); 
	PMsg3( ret, LocalesTable, locale, P_TXT( TXTHHY_X040__N0XN1XN2_XETX ), str_buff, str_buff1, str_buff2 ); // P_TXT( TXT_HISTORY_TIME )
	*/
	// PYR-28924 for started tournaments only
	if(!hdr.started.isNull())
	{
		buff_str = "";
		i18nAppendChar( buff_str, ' ' );
		PMsg1( ret, LocalesTable, locale, P_TXT( TXTHHY_Tournament_startedN0 ), buff_str ); // P_TXT( TXT_TOURNSTAT_STARTED )

		SrvTime when;
			when._year = hdr.started._year;
			when._month = hdr.started._month;
			when._day = hdr.started._day;
			when._hour = hdr.started._hour;
			when._minute = hdr.started._minute;
			when._second = hdr.started._second;
		_formatDateTimeAltTZ( ret, LocalesTable, locale, when, timezone );
	}

	i18nAppendChar(ret, '\n');
}

/*---------------------------------------------------------------------------------------------
				formatStatFooter
---------------------------------------------------------------------------------------------*/
void formatStatFooter(UINT32 locale, PString& ret, const char* site )
{
	PMsg1( ret, LocalesTable, locale, P_TXT( TXTHHY__Please_note_that_play_money_h_ELL ), site ); // P_TXT( TXT_STATISTIC_FOOTER )
}

/*---------------------------------------------------------------------------------------------
				formatGameHistory 
---------------------------------------------------------------------------------------------*/
void formatGameHistory(PString& ret, UINT32 locale, const Statistics& stats)
{
	try
	{
		for( int handType = 0; handType < UniqueHandType_NextAfterLast; handType++ )
		{
			for( int oneToOne = 0; oneToOne < 2; ++oneToOne )
			{
				for( int hiLo = 0; hiLo < 2; ++hiLo )
				{
					for( int chipsType = 0; chipsType < 2; ++chipsType )	// skips tourn_chips
					{
						for( int structureTypeLoop = 0; structureTypeLoop < Statistics::Structure_Count; ++structureTypeLoop )
						{
							Statistics::StructureType structureType = (Statistics::StructureType)structureTypeLoop;
							if( stats.isHoldemStat( static_cast< BYTE >( handType ) ) )
							{
								formatHoldemHistory( stats, handType, oneToOne != 0, hiLo != 0, chipsType, structureType, locale, ret, true );
							}
							else if( stats.isStudStat( static_cast< BYTE >( handType ) ) )
							{
								formatStudHistory( stats, handType, oneToOne != 0, hiLo != 0, chipsType, structureType, locale, ret, true );
							}
							else if( stats.isDrawStat( static_cast< BYTE >( handType ) ) )
							{
								formatDrawHistory( stats, handType, oneToOne !=0, hiLo !=0, chipsType, structureType, locale, ret, true );
							}
						}
					}
				}
			}
		}
	}
	catch( const PError& er )
	{
		PLog( "FormatGameHistory::%s", er.why() );
	}
}

/*---------------------------------------------------------------------------------------------
				formatStatHistory 
---------------------------------------------------------------------------------------------*/
void formatStatHistory( PString& ret, UINT32 locale, UINT32 brandId, const Statistics& stats, UINT32 numRequestedHands, const char* name, const char* email, const char* site )
{
	try
	{ /*enum statsRequestType
		{
		Non_defined,
		Hands_stats,
		N_tourns_stats,
		ID_tourn_stats
		}
		*/
		formatStatHeader( ret, locale, brandId, name, email, numRequestedHands, stats._numHands, Hands_stats );
		i18nAppendChar(ret, '\n');

		formatGameHistory(ret, locale, stats);

		formatStatFooter(locale, ret, site );
		//formatTournStatFooter( ret );
	}
	catch( const PError& er )
	{
		PLog( "FormatStatHistory::%s", er.why() );
	}
}

bool TournParticipantStatLess::operator()(const TournParticipantStat& a, const TournParticipantStat& b) const
{
	UINT32 placeA = (a.entryInd == -1 ? a.tournParticipant->place : a.tournParticipant->reentries[a.entryInd].place);
	UINT32 placeB = (b.entryInd == -1 ? b.tournParticipant->place : b.tournParticipant->reentries[b.entryInd].place);
	if(placeA != placeB)
		return (placeA < placeB);
	int nameComp = strcmp(a.tournParticipant->user, b.tournParticipant->user);
	if(nameComp)
		return (nameComp < 0);
	UINT32 numEntryA = (a.entryInd == -1 ? a.tournParticipant->reentries.size() : a.tournParticipant->reentries[a.entryInd].numEntry);
	UINT32 numEntryB = (b.entryInd == -1 ? b.tournParticipant->reentries.size() : b.tournParticipant->reentries[b.entryInd].numEntry);
	return (numEntryA > numEntryB);
}

void TournParticipantStats::insertParticipants(const vector<TournParticipant>& tournParticipants)
{
	for(size_t iP = 0; iP < tournParticipants.size(); ++iP)
	{
		insert(TournParticipantStat(&tournParticipants[iP], -1));
		for(size_t iE = 0; iE < tournParticipants[iP].reentries.size(); ++ iE)
			insert(TournParticipantStat(&tournParticipants[iP], iE));
	}
}

bool OneParticipantStatLess::operator()(const TournParticipantStat& a, const TournParticipantStat& b) const
{
	UINT32 numEntryA = (a.entryInd == -1 ? a.tournParticipant->reentries.size() : a.tournParticipant->reentries[a.entryInd].numEntry);
	UINT32 numEntryB = (b.entryInd == -1 ? b.tournParticipant->reentries.size() : b.tournParticipant->reentries[b.entryInd].numEntry);
	return (numEntryA > numEntryB);
}

/*---------------------------------------------------------------------------------------------
				formatTournStats
---------------------------------------------------------------------------------------------*/
void formatTournStats( PString& ret, UINT32 locale, UINT32 brandId, UINT32 timezone, const TournHeader& hdr, const Statistics& stats,
					  UINT64 handId64, BYTE flags, bool noPlayerStats, const char* name, const char* email,
					  const vector< TournParticipant >& players, bool showTournInProgress /*= true*/ )
{
	try
	{
		formatTournHeader(ret, locale, brandId, timezone, hdr );

		vector<PString> customPrizes; //PYR-33574
		if( *hdr.customPrizes.c_str() )
		{
			splitString( hdr.customPrizes, customPrizes, '|', true, true );
		}

		if( hdr.finished.isNull() )
		{
			if( showTournInProgress )
				PMsg( ret, LocalesTable, locale, P_TXT( TXTHHY_Tournament_is_still_in_progres_ELL ) );
		}
		else
		{
			PString timeStr(" ");
			_formatDateTimeAltTZ( timeStr, LocalesTable, locale, hdr.finished, timezone );
			PMsg1( ret, LocalesTable, locale, P_TXT( TXTHHY_Tournament_finishedN0 ), timeStr );
		}
		i18nAppendChar(ret, '\n');

		PString currency;
		formatCurrencyShort(currency, hdr.currency);

		TournParticipantStats tournParticipantStats;
		tournParticipantStats.insertParticipants(players);

		OneParticipantStat oneParticipantStats;

		int lineNum = 1;
		for(TournParticipantStats::const_iterator itP = tournParticipantStats.begin(); itP != tournParticipantStats.end(); ++itP, ++lineNum)
		{	//"  2: PRIZM [2] (Canada), $33.00 (30%) + $22.00"
			PString lineNumStr("  ");
			lineNumStr.appendInt(lineNum).append(": ");	//"  2: "

			PString nameStr(itP->tournParticipant->user);	//"PRIZM"
			UINT32 numEntry = (itP->entryInd == -1 ? itP->tournParticipant->reentries.size() : itP->tournParticipant->reentries[itP->entryInd].numEntry);
			if(numEntry)
			{
				i18nAppend(nameStr, " [");
				i18nAppendUint(nameStr, numEntry + 1);
				i18nAppendChar(nameStr, ']');	//"PRIZM [2]"
			}

			PString cityOrCountry(itP->tournParticipant->showCity ? itP->tournParticipant->city : itP->tournParticipant->country);	//"Canada"

			PString moneyStr;
			PString commentStr;
			if(itP->entryInd == -1)		//only for last entries
			{
				if(itP->tournParticipant->amount)	//got something
				{
					// //PYR-31192
					if(hdr.awardSchema == PRIZE_ANOTHER_TOURN_FPP)
					{
						i18nFormatMoneyWithCurrency(moneyStr, LocalesTable, locale, itP->tournParticipant->amount*100, "FPP", 
							eMoneyTypeCurr, eFormatCurrTypeName, false /*showZeroCents*/, true/*showTriplets*/);
					}
					else if(hdr.isPlayMoney)
					{	// PYR-25238
						i18nFormatFloat(moneyStr, LocalesTable, locale, (UINT64)itP->tournParticipant->amount * hdr.scalePM, 2, false, true);
					}
					else
					{	// not play money - no scaling
						moneyStr.assign(currency);
						i18nFormatFloat(moneyStr, LocalesTable, locale, itP->tournParticipant->amount, 2, true, true);	//"$33.00"
					}

					if( !hdr.satelliteTarget && hdr.prizePool )
					{
						INT32 percent = PMulDiv(100000, itP->tournParticipant->amount, hdr.prizePool);
						commentStr.assign(" (");
						i18nFormatFloat(commentStr, LocalesTable, locale, percent, 3, false, false);
						i18nAppend(commentStr, "%)");	//" (30%)"
					}
				}
				
				if( hdr.satelliteTarget && itP->tournParticipant->nextTournID )
					PMsg(commentStr, LocalesTable, locale, P_TXT( TXTHHY_X040Xqualified_for_the_target__ELL ));
				else if( *(itP->tournParticipant->ticketAdmission) )
				{
					if( !customPrizes.empty() ) //PYR-33574
					{
						if( lineNum <= static_cast< int >( customPrizes.size() ) )
						{
							moneyStr = customPrizes[lineNum - 1];
						}
					}
					else
						PMsg1(commentStr, LocalesTable, locale, P_TXT( TXTHHY_Tournament_Ticket_XN0X ), itP->tournParticipant->ticketAdmission);
				}
				else if( itP->tournParticipant->place == 0 )
					PMsg(commentStr, LocalesTable, locale, P_TXT( TXTHHY_still_playing ));

				// PYR-59874
				// only for eligible users, last entries and if tournament has FreeBet
				if( !hdr.freeBetReward.base.isNull() && 0 != (itP->tournParticipant->tournFlags & eTournUserFlags_WonSAGFreeBetReward) )
				{
					PString fbAwardStr;
					fbAwardStr.assign(" + ");
										
					INT32 stake = hdr.freeBetReward.awarded.amount ? hdr.freeBetReward.awarded.amount : hdr.freeBetReward.base.stake;
					PString fbCurrency = hdr.freeBetReward.awarded.amount ? hdr.freeBetReward.awarded.currency : hdr.freeBetReward.base.currency;

					i18nFormatMoneyWithCurrency(fbAwardStr, LocalesTable, locale, stake, fbCurrency,
						eMoneyTypeCurr, eFormatCurrTypeSignWithCode, true /*showZeroCents*/, true/*showTriplets*/);

					// + $22.00
					commentStr.append(fbAwardStr);
				}
			}

			PMsg5(ret, LocalesTable, locale, P_TXT( TXTHHY_N0N1_XN2XX_N3N4 ), lineNumStr, nameStr, cityOrCountry, moneyStr, commentStr);
			i18nAppendChar(ret, '\n');

			if( itP->tournParticipant->user.equals(name) )	//save for future
				oneParticipantStats.insert(TournParticipantStat(itP->tournParticipant, itP->entryInd));
		}

		if(oneParticipantStats.size() == 0)
		{
			if((flags&eTournStatsFlags_Details)!=0) // PYR-33674
			{
				PMsg(ret, LocalesTable, locale, P_TXT( TXTHHY__There_are_no_personal_statist_ELL ));
			}
		}
		else for(OneParticipantStat::const_iterator itP = oneParticipantStats.begin(); itP != oneParticipantStats.end(); ++itP)
		{
			UINT32 nPlace = itP->tournParticipant->place;
			if( itP->entryInd == -1 && nPlace == 0 )
			{
				PMsg(ret, LocalesTable, locale, P_TXT( TXTHHY__You_are_still_playing_in_this_ELL ));
				continue;
			}

			PString elimComment;
			if(itP->entryInd == -1)
			{
				if(handId64)
				{	// (eliminated at hand #%1)
					PString handId64Str;
					handId64Str.appendUint64(handId64);
					PMsg1(elimComment, LocalesTable, locale, P_TXT( TXTHHY_X040Xeliminated_at_hand_XN0X ), handId64Str);
				}
			}
			else
			{
				nPlace = itP->tournParticipant->reentries[itP->entryInd].place;

				if(!itP->tournParticipant->reentries[itP->entryInd].eliminated.isNull())
				{
					PString timeStr;
					_formatDateTimeAltTZ(timeStr, LocalesTable, locale, itP->tournParticipant->reentries[itP->entryInd].eliminated, timezone);
					PMsg1(elimComment, LocalesTable, locale, P_TXT( TXTHHY_Xeliminated_at_N0X ), timeStr);
				}
			}

			if( nPlace > DELAYED_PLACE_CUTOFF )//PYR-37049
			{
				PMsg1(ret, LocalesTable, locale, P_TXT( TXTHHY_You_finished_the_tournamentN0X ), elimComment);
			}
			else
			{
				PString placeStr;
				i18nFormatNth(placeStr, LocalesTable, locale, nPlace);
				PMsg2(ret, LocalesTable, locale, P_TXT( TXTHHY__You_finished_in_N0_placeN1X ), placeStr, elimComment);
			}


		}

		if(oneParticipantStats.size())
		{
			if( !noPlayerStats ) // if provide statistics as it worked before:
			{
				i18nAppend(ret, "\n\n");

				if( !handId64 )
				{
					if ((flags&eTournStatsFlags_Hands)!=0) // PYR-33674
					{
						PMsg( ret, LocalesTable, locale, P_TXT( TXTHHY__Statistics_for_this_tournamen_ELL ) );
					}
				}
				else
				{ // copy/paste from formatStatHistory():
					for( int handType = 0; handType < UniqueHandType_NextAfterLast; ++handType )
					{
						for( int oneToOne = 0; oneToOne < 2; ++oneToOne )
						{
							for( int hiLo = 0; hiLo < 2; ++hiLo )
							{
								for( int structureTypeLoop = 0; structureTypeLoop < Statistics::Structure_Count; ++structureTypeLoop )
								{
									Statistics::StructureType structureType = (Statistics::StructureType)structureTypeLoop;
									if( stats.isHoldemStat( static_cast< BYTE >( handType ) ) )
									{
										formatHoldemHistory( stats, handType, oneToOne != 0 , hiLo != 0 , TOURN_CHIPS, structureType, locale, ret, hdr.game >= MultiGameTypeShift );
									}
									else if( stats.isStudStat( static_cast< BYTE >( handType ) ) )
									{
										formatStudHistory( stats, handType, oneToOne != 0 , hiLo != 0 , TOURN_CHIPS, structureType, locale, ret, hdr.game >= MultiGameTypeShift );
									}
									else if( stats.isDrawStat( static_cast< BYTE >( handType ) ) )
									{
										formatDrawHistory( stats, handType, oneToOne != 0 , hiLo != 0 , TOURN_CHIPS, structureType, locale, ret, hdr.game >= MultiGameTypeShift );
									}
								}
							}
						}
					}
				}
			}
			else
			{
				i18nAppendChar(ret, '\n');
			}

			if(oneParticipantStats.size() > 1)
			{
				PString numEntrStr;
				numEntrStr.appendUint(oneParticipantStats.size() - 1);
				UINT32 totalBuyin = (hdr.buyIn + hdr.rake) * (oneParticipantStats.size() - 1);
				PString moneyStr;
				if(hdr.isPlayMoney)
				{
					i18nFormatFloat(moneyStr, LocalesTable, locale, (UINT64)totalBuyin * hdr.scalePM, 2, false, true);
				}
				else
				{	// not play money - no scaling
					moneyStr.assign(currency);
					i18nFormatFloat(moneyStr, LocalesTable, locale, totalBuyin, 2, true, true);
				}
				PMsg2(ret, LocalesTable, locale, P_TXT( TXTHHY__You_made_N0_re_entries_for_a__ELL ), numEntrStr, moneyStr);
			}
		}	//end if
		i18nAppendChar(ret, '\n');
	}
	catch( const PError& er )
	{
		PLog( "FormatTournStats::%s", er.why() );
	}
}

/*---------------------------------------------------------------------------------------------
				formatFooter
---------------------------------------------------------------------------------------------*/
void formatTournStatFooter(UINT32 locale, PString& ret, const char* site )
{
	if( ! site )
		site = "pokerstars.com";
	PMsg1( ret, LocalesTable, locale, P_TXT( TXTHHY__If_you_have_any_questionsX_pl_ELL ), site ); // P_TXT( TXT_EMAILS_FOOTER )
}
