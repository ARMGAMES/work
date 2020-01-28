// ===========================================================
// Copyright (C) 2012 PYR Software Ltd. All rights reserved.
// ===========================================================

#ifndef SeatFinderSharedCli_h_included
#define SeatFinderSharedCli_h_included

#include "commmsgbody.h"
#include "composecolls.h"

#include "cliutils.h"

//------------------------------------------------------------------------------

#include "storm/TableCommonStructs.h"

typedef TableCommon::TableStakes TableStakes;

//------------------------------------------------------------------------------

// PYR-43054
enum SeatFinderRemoveReason // reason why user was removed from seat finder
{
	eSeatFinderCancelled				= 1, // seat finder dissolved
	eSeatFinderNotEnoughChips			= 2, // not enough chips
	eSeatFinderNewRatHolingObligation	= 3, // new rat holing obligation created
	eSeatFinderNotAllowedToPlayRM		= 4, // (loginFlags & eUserExLoginFlags_AllLimitedModeGeoIPFlags) == 0
	eSeatFinderNotLoggedIn				= 5, // user disconnected
	eSeatFinderDbmAtTooManyTables		= 6, // DBM failed to add user to table
	eSeatFinderDbmAtTooManyHuTables		= 7, // DBM failed to add user to table
	eSeatFinderDbmSqlError				= 8, // DBM failed to add user to table
	eSeatFinderLobbyCannotSitUser		= 9, // lobby failed to sit user at table
	eSeatFinderUserPoolBlocked			= 10, // pool block penalty applied
	eSeatFinderDealingStopped			= 11, // dealing stopped on one of sites
	eSeatFinderAamsError				= 12, // AAMS error
	eSeatFinderNotMatch					= 100, // user is no longer allowed to be on seat finder for any other reason
	eSeatFinderRemoveReasonMax			= 0xff
};

struct SeatFinderCli
{
	UINT32 id;
	PString currency;
	INT8 isHiLo;
	INT32 loBet;
	INT32 hiBet;
	INT32 cap;
	INT8 game;
	INT8 limit;
	INT32 minBuyIn;
	INT32 maxBuyIn;
	INT32 ante;
	bool isFast;
	INT8 maxPlayers;
	INT32 chipsQuantum;
	INT32 scalePM;
	TableStakes tableStakes;
	vector<INT64> variableAntes;

	SeatFinderCli()
		: id(0)
		, isHiLo(0)
		, loBet(0)
		, hiBet(0)
		, cap(0)
		, game(0)
		, limit(0)
		, minBuyIn(0)
		, maxBuyIn(0)
		, ante(0)
		, isFast(false)
		, maxPlayers(0)
		, chipsQuantum(0)
		, scalePM(1)
	{}

	SeatFinderCli( CommMsgParser& parser ) { parse( parser ); }

	SeatFinderCli( const SeatFinderCli& source )
		: id( source.id )
		, currency( source.currency )
		, isHiLo( source.isHiLo )
		, loBet( source.loBet )
		, hiBet( source.hiBet )
		, cap( source.cap )
		, game( source.game )
		, limit( source.limit )
		, minBuyIn( source.minBuyIn )
		, maxBuyIn( source.maxBuyIn )
		, ante( source.ante )
		, isFast( source.isFast )
		, maxPlayers( source.maxPlayers )
		, chipsQuantum( source.chipsQuantum )
		, scalePM( source.scalePM )
		, tableStakes( source.tableStakes )
		, variableAntes( source.variableAntes )
	{}
	SeatFinderCli& operator =(const SeatFinderCli&) = delete;

	CommMsgParser& parse( CommMsgParser& parser )
	{
		parser
			.parseUINT32( id )
			.parseStringP( currency )
			.parseINT8( isHiLo )
			.parseINT32( loBet )
			.parseINT32( hiBet )
			.parseINT32( cap )
			.parseINT8( game )
			.parseINT8( limit )
			.parseINT32( minBuyIn )
			.parseINT32( maxBuyIn )
			.parseINT32( ante )
			.parseBOOL( isFast )
			.parseINT8( maxPlayers )
			.parseINT32( chipsQuantum )
			.parseINT32( scalePM )
			;

		tableStakes.parseMsg( parser );
		parseVectorOrList( parser, variableAntes );
		return parser;
	}

	static CommMsgBody& compose(  UINT32 id, const char* currency, INT8 isHiLo, INT32 loBet, INT32 hiBet
								, UINT32 cap, INT8 game, INT8 limit, UINT32 minBuyIn, UINT32 maxBuyIn
								, UINT32 ante, bool isFast, INT8 maxPlayers, INT32 chipsQuantum
								, INT32 scalePM, const TableStakes& tableStakes, const vector<INT64>& variableAntes, CommMsgBody& body )
	{
		body
			.composeUINT32( id )
			.composeString( currency )
			.composeINT8( isHiLo )
			.composeINT32( loBet )
			.composeINT32( hiBet )
			.composeINT32( cap )
			.composeINT8( game )
			.composeINT8( limit )
			.composeINT32( minBuyIn )
			.composeINT32( maxBuyIn )
			.composeINT32( ante )
			.composeBOOL( isFast )
			.composeINT8( maxPlayers )
			.composeINT32( chipsQuantum )
			.composeINT32( scalePM )
			;

		tableStakes.composeMsg( body );
		composeVectorOrList( body, variableAntes );
		return body;
	}

	INT64 bigBlind() const { return ::bigBlind( (PotStruct)limit, loBet, hiBet ); }
};

#endif // SeatFinderSharedCli_h_included
