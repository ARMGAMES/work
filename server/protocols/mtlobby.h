//------------------------------------------------------------------------------
//	mtlobby.h
//------------------------------------------------------------------------------
//
//	Copyright (c) 2001-2019 PYR Software Ltd. All rights reserved. This document
//	is for the use of licensed users only and is subject to the terms and
//	conditions of the accompanying and/or applicable license between you and
//	PYR Software Ltd, including without limitation those terms and conditions
//	prohibiting unauthorized translation, reverse engineering, decompilation,
//	disassembly, and other inappropriate use of any software provided. Any
//	unauthorized use, copying, modification, distribution, publication or
//	disclosure of information is a violation of copyright laws. No reproduction
//	in whole or in part of this document may be made without express written
//	consent of PYR Software Ltd.
//
//
//------------------------------------------------------------------------------

#ifndef	mtlobby_h_included
#define	mtlobby_h_included

//------------------------------------------------------------------------------
//these are kinds of events found in TournamentSchedule table in database:
//------------------------------------------------------------------------------

#define MTEVENT_REGISTRATION_START					1
#define MTEVENT_CREATE_TABLES						2
#define MTEVENT_SET_STAKES							3
#define MTEVENT_SET_STRUCTURE						4
#define MTEVENT_RESHUFFLE							5
#define MTEVENT_BREAK								6
#define MTEVENT_NO_REBUYS							7
#define MTEVENT_SET_SMALL_BLINDS					8
#define MTEVENT_SET_BRINGIN							9
#define MTEVENT_SET_ANTE							10

//------------------------------------------------------------------------------
//publishing constants:
//------------------------------------------------------------------------------

#define PUBL_TABLES_ROOT							0
#define PUBL_TOURNAMENT_ROOT						1
#define PUBL_USERS_ROOT								2

//------------------------------------------------------------------------------

#define	PUBL_TABLE_STATIC_LEAF						0
#define	PUBL_TABLE_DYNAMIC_LEAF						1

//------------------------------------------------------------------------------

#define	PUBL_TOURN_STATIC_LEAF						0
#define	PUBL_TOURN_DYNAMIC_LEAF						1
#define	PUBL_TOURN_FREQ_LEAF						2

//------------------------------------------------------------------------------

#define	PUBL_USER_STATIC_LEAF						0
#define PUBL_USER_DYNAMIC_LEAF						1
#define	PUBL_USER_KNOCKOUT_LEAF						2 // [AO 2010-05-13] 
#define	PUBL_USER_TICKET_LEAF						3 // PYR-24549

//------------------------------------------------------------------------------
// publishing constants for tournInfoPublisher
//------------------------------------------------------------------------------

#define PUBL_TOURN_INFO_ROOT						0

#define PUBL_TOURN_INFO_STAKES_LEAF					0	// to be deprecated in favor of PUBL_TOURN_INFO_NEWSTAKES_LEAF
#define PUBL_TOURN_INFO_BREAKS_LEAF					1
#define PUBL_TOURN_INFO_HANDBYHAND_LEAF				2
#define PUBL_TOURN_INFO_PRIZES_LEAF					3
#define PUBL_TOURN_INFO_REBUYRAKECOUNTRIES_LEAF		4
#define PUBL_TOURN_INFO_NEWSTAKES_LEAF				5	// PYR-85614

//------------------------------------------------------------------------------
// leaf numeration constants for MtLobbyServerData2004
//------------------------------------------------------------------------------

#define PUBL_TOURN2004_ROOT							0

#define PUBL_TOURN2004_STATIC_LEAF					0
#define PUBL_TOURN2004_DYNAMIC_BETTING_LEAF			1	// to be deprecated in favor of PUBL_TOURN2004_DYNAMIC_BETTING_LEAF_NEW
#define PUBL_TOURN2004_DYNAMIC_STATUS_LEAF			2
#define PUBL_TOURN2004_DYNAMIC_MSGBOARD_LEAF		3
#define PUBL_TOURN2004_DYNAMIC_BREAK_LEAF			4
#define PUBL_TOURN2004_DYNAMIC_CLOSED_LEAF			5
#define PUBL_TOURN2004_DYNAMIC_AWARDS_LEAF			6
#define PUBL_TOURN2004_DYNAMIC_ADDON_LEAF			7
#define PUBL_TOURN2004_DYNAMIC_STAKES_LEAF			8
#define PUBL_TOURN2004_DYNAMIC_PRIZEPOOL_LEAF		9
#define PUBL_TOURN2004_DYNAMIC_REGISTERED_LEAF		10
#define PUBL_TOURN2004_DYNAMIC_PRELIM_AWARDS_LEAF	11
#define PUBL_TOURN2004_DYNAMIC_TABLE_MSG_LEAF		12
#define PUBL_TOURN2004_DYNAMIC_HAPPY_HOURS_LEAF		13
#define PUBL_TOURN2004_DYNAMIC_HANDBYHAND_LEAF		14
#define PUBL_TOURN2004_DYNAMIC_BOUNTIES_LEAF		15
#define PUBL_TOURN2004_DYNAMIC_GOLDEN_LEAF			16
#define PUBL_TOURN2004_AUTOPUBLISH_LEAF				17 // PYR-27969
#define PUBL_TOURN2004_DYNAMIC_PRIZEPOOLUNIT_LEAF	18 // PYR-28498
#define PUBL_TOURN2004_DYNAMIC_OPTEARLYSTART_LEAF	19 // PYR-18141
#define PUBL_TOURN2004_DYNAMIC_MILESTONE_LEAF		20 // PYR-28689
#define PUBL_TOURN2004_DYNAMIC_SAG_LEAF				21 // PYR-28752
#define PUBL_TOURN2004_DYNAMIC_PER_HAND				22 // PYR-30443
#define PUBL_TOURN2004_DYNAMIC_BEATTHECLOCK			23 // PYR-44718
#define PUBL_TOURN2004_DYNAMIC_TRIPLE_SPINNER		24 // PYR-51310
#define PUBL_TOURN2004_DYNAMIC_BETTING_LEAF_NEW		25 // PYR-85614

//------------------------------------------------------------------------------
// publishing constants for tournLocalDataPublisher
//------------------------------------------------------------------------------

#define PUBL_TOURN_I18N_ROOT						0

#define PUBL_TOURN_I18N_MSG_BOARD_LEAF				0
#define PUBL_TOURN_I18N_NAME_LEAF					1
#define PUBL_TOURN_I18N_TARGET_NAME_LEAF			2
#define PUBL_TOURN_I18N_TARGET_NAME_MTT_LEAF		3

//------------------------------------------------------------------------------
// delay in seconds before destroying last tournament table after tournament is over
//------------------------------------------------------------------------------

#define	DEFAULT_MT_LAST_TABLE_DELAY					300
#define	DEFAULT_SG_LAST_TABLE_DELAY					120

#define BEATTHECLOCK_ANIMATION_DELAY_SEC			10

//------------------------------------------------------------------------------

// [AO 2010-10-13] #14719 - Extracted to a separate file
#include "TournWinFlags.h"

//------------------------------------------------------------------------------
// MtLobbyTournData::MtTournamentInfo::tournMessageFlags, mtclilobbydata.h
//------------------------------------------------------------------------------

enum eTableMsgFlags
{
	eTableMsgFlags_showOnLimboTables				= (1 << 0),
	eTableMsgFlags_showOnRegularTables				= (1 << 1),
	eTableMsgFlags_addToChat						= (1 << 2),
	eTableMsgFlags_goldenBackground_Obsolete		= (1 << 3),
	eTableMsgFlags_playersOnly						= (1 << 4),
	eTableMsgFlags_JackpotAnnounce					= (1 << 5),
	eTableMsgFlags_JackpotResult					= (1 << 6),
	eTableMsgFlags_EarlyPayout						= (1 << 7),
};

//------------------------------------------------------------------------------

enum eLiveEventFlags
{
	eLiveEventFlags_None							= 0,
	eLiveEventFlags_DirectSattelite					= (1 << 0),
};

//------------------------------------------------------------------------------
// PYR-51083
//------------------------------------------------------------------------------

enum class TournRegInfoFlags
{
	NoRmOkCheck										= (1 << 0), // 0x1 - do not check RMOK flag if insufficient funds
	MultipleFlightEntryConfirmed					= (1 << 1), // 0x2 - do not check for other flight entries
	eTournRegInfoFlags_Last							= MultipleFlightEntryConfirmed, // UPDATE IF NEW FLAGS ARE ADDED;
};

//------------------------------------------------------------------------------
// PYR-103079
//------------------------------------------------------------------------------

enum eClientTournRegFlags
{
	eClientTournRegFlags_IgnoreFlightEntryCheck = ( 1 << 0 ), // 0x1 - do not check for other flight entries
	eClientTournRegFlags_Last = eClientTournRegFlags_IgnoreFlightEntryCheck, // UPDATE IF NEW FLAGS ARE ADDED;
};

//------------------------------------------------------------------------------
#endif // mtlobby_h_included
//------------------------------------------------------------------------------
