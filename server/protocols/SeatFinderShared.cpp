// SeatFinderShared.cpp - SeatFinder and Blitz definitions

#include "SeatFinderShared.h"
#include "composecolls.h"
#include "brands.h"
#include "dbm.h"

SeatFinderBase::SeatFinderBase()
	: id(0)
	, game(0)
	, structure(0)
	, isHiLo(0)
	, maxPlayersPerTable(0)
	, loBet(0)
	, hiBet(0)
	, ante(0)
	, cap(0)
	, minChipsLimit(0)
	, maxBuyIn(0)
	, visibilityMask(0)
	, defaultBuyIn(0)
	, countryRestrictionInverted(0)
	, isPlayMoney(false)
	, morphLobbyMask(0)
	, scalePM(1)
	, handsPerHour(0)
	, avgPotSize(0)
	, avgStackSize(0)
	, numPlayers(0)
{
}

void ShadowSeatFinder::parseFromLobby( CommMsgParser& parser ) // override
{
	parser
		.parseUINT32( id )
		.parseBYTE( game )
		.parseBYTE( structure )
		.parseBYTE( isHiLo )
		.parseBYTE( maxPlayersPerTable )
		.parseINT32( loBet )
		.parseINT32( hiBet )
		.parseINT32( ante )
		.parseINT32( cap )
		.parseStringP( currency )
		.parseINT32( minChipsLimit )
		.parseINT32( maxBuyIn )
		.parseObject( visibilityMaskEx ) // PYR-100652
		.parseINT32( defaultBuyIn )
		.parseStringP( specialName )
		.parseINT8( minPlayersPerTable )
		.parseBOOL( isPasswordProtected )
		.parseBOOL( countryRestrictionInverted )
		;
	parseVectorOrList( parser, restrictedCountries );
	parser
		.parseStringP( replayerAddress )
		.parseStringP( replayerInstance )
		.parseINT32( poolBlockMinHands )
		;
	parseVectorOrList( parser, poolBlockTimePenalty );
	parser
		.parseUINT32( flags )
		.parseUINT64( flags2 )
		.parseBYTE( isPlayMoney )
		.parseBYTE( morphLobbyMask )
		.parseUINT32( scalePM )
		;
	tableStakes.parseMsg( parser );
	parseVectorOrList( parser, variableAntes );
	parser
		.parseINT32( handsPerHour )
		.parseINT32( avgPotSize )
		.parseINT32( avgStackSize )
		.parseINT32( numTables )
		.parseINT32( numPlayers )
		.parseINT32( numFullTables )
		;
	visibilityMask = visibilityMaskEx.getLower32(); // PYR-100652
}

void ShadowSeatFinder::parseFromLobbyDyna( CommMsgParser& parser ) // override
{
	parser
		.parseUINT32( id )
		.parseBYTE( morphLobbyMask ) //PYR-64785
		.parseINT32( handsPerHour )
		.parseINT32( avgPotSize )
		.parseINT32( avgStackSize )
		.parseINT32( numTables )
		.parseINT32( numPlayers )
		.parseINT32( numFullTables )
		;
}

void ShadowSeatFinder::composeStatMsg( CommMsgBody& msg ) const
{
	msg
		.composeUINT32( BrandType_ALLMask ) // for now brand is not part of SeatFinder hash key
		.composeUINT32( visibilityMaskEx.getLower32() )
		.composeBOOL( countryRestrictionInverted )
		;
	composeVectorOrList( msg, restrictedCountries );
	msg
		.composeUINT32( id )
		.composeBYTE( game )
		.composeBYTE( structure )
		.composeBYTE( isHiLo )
		.composeBYTE( maxPlayersPerTable )
		.composeINT32( loBet )
		.composeINT32( hiBet )
		.composeINT32( ante )
		.composeINT32( cap )
		.composeString( currency )
		.composeINT32( minChipsLimit )
		.composeINT32( maxBuyIn )
		.composeINT32( defaultBuyIn )
		.composeString( specialName )
		.composeString( replayerAddress )
		.composeString( replayerInstance )
		.composeINT8( minPlayersPerTable )
		.composeBOOL( isPasswordProtected )
		.composeINT32( poolBlockMinHands )
		;
	composeVectorOrList( msg, poolBlockTimePenalty );
	msg
		.composeUINT32( flags )
		.composeUINT64( flags2 )
		.composeBYTE( isPlayMoney )
		.composeBOOL( morphLobbyMask != 0 )
		.composeUINT32( scalePM )
		;

	tableStakes.composeMsg( msg );
	composeVectorOrList( msg, variableAntes );
	msg
		.composeObject( visibilityMaskEx ) // PYR-100652
		;
}

void ShadowSeatFinder::composeDynaMsg( CommMsgBody& msg ) const // override
{
	msg
		.composeINT32( handsPerHour )
		.composeINT32( avgPotSize )
		.composeINT32( avgStackSize )
		.composeINT32( numTables )
		.composeINT32( numPlayers )
		.composeINT32( numFullTables )
		;
}

void ShadowSeatFinder::composeForMobile( CommMsgBody& msg ) const // override
{
	CommMsgBody statBody, dynaBody;
	composeStatMsg( statBody );
	composeDynaMsg( dynaBody );
	msg.composeMsgBody( statBody );
	msg.composeMsgBody( dynaBody );
}

void ShadowSeatFinder::parseForMobile( CommMsgParser& parser ) // override
{
	CommMsgBody statBody, dynaBody;
	parser.parseMsgBody( statBody );
	parser.parseMsgBody( dynaBody );

	CommMsgParser statParser( statBody );
	UINT32 brandMaskDummy;
	statParser
		.parseUINT32( brandMaskDummy ) // for now brand is not part of SeatFinder hash key
		.parseUINT32( visibilityMask )
		.parseBOOL( countryRestrictionInverted )
		;
	parseVectorOrList( statParser, restrictedCountries );
	statParser
		.parseUINT32( id )
		.parseBYTE( game )
		.parseBYTE( structure )
		.parseBYTE( isHiLo )
		.parseBYTE( maxPlayersPerTable )
		.parseINT32( loBet )
		.parseINT32( hiBet )
		.parseINT32( ante )
		.parseINT32( cap )
		.parseStringP( currency )
		.parseINT32( minChipsLimit )
		.parseINT32( maxBuyIn )
		.parseINT32( defaultBuyIn )
		.parseStringP( specialName )
		.parseStringP( replayerAddress )
		.parseStringP( replayerInstance )
		.parseINT8( minPlayersPerTable )
		.parseBOOL( isPasswordProtected )
		.parseINT32( poolBlockMinHands )
		;
	parseVectorOrList( statParser, poolBlockTimePenalty );
	statParser
		.parseUINT32( flags )
		.parseUINT64( flags2 )
		.parseBYTE( isPlayMoney )
		.parseBYTE( morphLobbyMask )
		.parseUINT32( scalePM )
		;

	// PYR-85614, PYR-83034, PYR-82834
	tableStakes.parseMsg( statParser );
	parseVectorOrList( statParser, variableAntes );
	statParser
		.parseObject( visibilityMaskEx ) // PYR-100652
		;

	CommMsgParser dynaParser( dynaBody );
	dynaParser
		.parseINT32( handsPerHour )
		.parseINT32( avgPotSize )
		.parseINT32( avgStackSize )
		.parseINT32( numTables )
		.parseINT32( numPlayers )
		.parseINT32( numFullTables )
		;
}

bool ShadowSeatFinder::dynaEqualForUpdate( const ShadowSeatFinder& other) const
{
	if( handsPerHour != other.handsPerHour	) return false;
	if( avgPotSize	 != other.avgPotSize	) return false;
	if( avgStackSize != other.avgStackSize	) return false;
	if( numTables	 != other.numTables		) return false;
	if( numPlayers	 != other.numPlayers	) return false;
	if( numFullTables!= other.numFullTables	) return false;
	return true;
}

bool ShadowSeatFinder::statEqualForUpdate( const ShadowSeatFinder& other) const
{
	if( defaultBuyIn != other.defaultBuyIn ) return false;
	if( minPlayersPerTable != other.minPlayersPerTable ) return false;
	if( !replayerAddress.equals( other.replayerAddress ) ) return false;
	if( !replayerInstance.equals( other.replayerInstance ) ) return false;
	if( poolBlockMinHands != other.poolBlockMinHands ) return false;
	if( poolBlockTimePenalty != other.poolBlockTimePenalty ) return false;
	if( flags != other.flags ) return false;
	if( flags2 != other.flags2 ) return false;
	return true;
}

ShadowBlitzForMobile::ShadowBlitzForMobile() 
	: isOneOnOne(0)
	, tableFlags(0)
	, tableFlags2(0)
	, chatLang(0)
	, quickLobbyVisible(false)
	, blitzFlags(0)
	, brandMask(0)
	, publProp(0)
	, avgPlayers(0)
	, avgVpip(0)
{
}

ShadowBlitzForMobile::ShadowBlitzForMobile( const ShadowBlitzForMobile& rh )
	: SeatFinderBase( rh )
	, server( rh.server )
	, name( rh.name )
	, replayerInstance( rh.replayerInstance )
	, isOneOnOne( rh.isOneOnOne )
	, tableFlags( rh.tableFlags )
	, tableFlags2( rh.tableFlags2 )
	, chatLang( rh.chatLang )
	, quickLobbyVisible( rh.quickLobbyVisible )
	, blitzFlags( rh.blitzFlags )
	, brandMask( rh.brandMask )
	, publProp( rh.publProp )
	, avgPlayers( rh.avgPlayers )
	, avgVpip( rh.avgVpip )
{
	fppFactorsBody.copyFrom( rh.fppFactorsBody );
	fppFactorsBodyEx.copyFrom( rh.fppFactorsBodyEx );
}

void ShadowBlitzForMobile::parseFromLobby( CommMsgParser& parser ) // override
{
	PLog( LOG_TRACE_FAULTY_LOGIC ": ShadowBlitzForMobile::parseFromLobby() should not be called" );
}

void ShadowBlitzForMobile::composeStatMsg( CommMsgBody& msg ) const // override
{
	msg
		.composeUINT32( id )
		.composeString( name )
		.composeString( server )
		.composeUINT32( tableFlags )
		.composeBYTE( isOneOnOne )
		.composeBYTE( isPlayMoney )
		.composeBYTE( maxPlayersPerTable )
		.composeBYTE( game )
		.composeBYTE( isHiLo )
		.composeINT32( loBet )
		.composeINT32( hiBet )
		.composeBYTE( structure )
		.composeINT32( cap )
		.composeBOOL( quickLobbyVisible ) // #22042
		.composeString( specialName )
		.composeUINT32( visibilityMaskEx.getLower32() )
		.composeUINT16( chatLang )
		.composeString( currency )
		.composeINT32( minChipsLimit )
		.composeINT32( maxBuyIn )
		.composeINT32( ante )
		;

	msg.composeUINT32( static_cast< UINT32 >( restrictedCountries.size() ) );
	if( restrictedCountries.size() )
	{
		msg.composeBOOL( countryRestrictionInverted );
		for( int i = 0; i < restrictedCountries.size(); ++i )
			msg.composeString( restrictedCountries[i] );
	}
	msg
		.composeUINT32( scalePM ) // PYR-24190
		.composeINT32( defaultBuyIn ) // PYR-24823
		.composeString(replayerInstance)	//PYR-24004
		.composeUINT32(blitzFlags)
		.composeUINT32(brandMask); // PYR-40190
	tableStakes.composeMsg( msg );
	msg.composeObject( visibilityMaskEx ); // PYR-100652

	msg.composeUINT64( tableFlags2 ); // PYR-106114
}

void ShadowBlitzForMobile::composeDynaMsg( CommMsgBody& msg ) const // override
{
	msg
		.composeINT32( numPlayers )
		.composeBYTE( avgPlayers )
		.composeINT32( avgPotSize )
		.composeINT32( handsPerHour )
		.composeBYTE( publProp )
		.composeINT32( avgStackSize )
		.composeBYTE( avgVpip )
		.composeMsgBody( fppFactorsBody )
		.composeMsgBody( fppFactorsBodyEx )
		;
}

void ShadowBlitzForMobile::composeForMobile( CommMsgBody& msg ) const // override
{
	CommMsgBody statBody, dynaBody;
	ShadowBlitzForMobile::composeStatMsg( statBody );
	ShadowBlitzForMobile::composeDynaMsg( dynaBody );
	msg.composeMsgBody( statBody );
	msg.composeMsgBody( dynaBody );
}

void ShadowBlitzForMobile::parseForMobile( CommMsgParser& parser ) // override
{
	CommMsgBody statBody, dynaBody;
	parser.parseMsgBody( statBody );
	parser.parseMsgBody( dynaBody );

	CommMsgParser statParser( statBody );
	statParser
		.parseUINT32( id )
		.parseStringP( name )
		.parseStringP( server )
		.parseUINT32( tableFlags )
		.parseBYTE( isOneOnOne )
		.parseBYTE( isPlayMoney )
		.parseBYTE( maxPlayersPerTable )
		.parseBYTE( game )
		.parseBYTE( isHiLo )
		.parseINT32( loBet )
		.parseINT32( hiBet )
		.parseBYTE( structure )
		.parseINT32( cap )
		.parseBOOL( quickLobbyVisible ) // #22042
		.parseStringP( specialName )
		.parseUINT32( visibilityMask )
		.parseUINT16( chatLang )
		.parseStringP( currency )
		.parseINT32( minChipsLimit )
		.parseINT32( maxBuyIn )
		.parseINT32( ante )
		;

	INT32 restrictedSz;
	statParser.parseINT32( restrictedSz );
	restrictedCountries.resize( restrictedSz );
	if( restrictedSz )
	{
		statParser.parseBOOL( countryRestrictionInverted );
		for( int i = 0; i < restrictedSz; ++i )
			statParser.parseStringP( restrictedCountries[i] );
	}
	statParser
		.parseUINT32( scalePM ) // PYR-24190
		.parseINT32( defaultBuyIn )	// PYR-24823
		.parseStringP( replayerInstance )	//PYR-24004
		.parseUINT32( blitzFlags )
		.parseUINT32( brandMask ) // PYR-40975
		;
	tableStakes.parseMsg( statParser );
	statParser.parseObject( visibilityMaskEx ); // PYR-100652
	statParser.parseUINT64( tableFlags2 ); // PYR-106114

	CommMsgParser dynaParser( dynaBody );
	dynaParser
		.parseINT32( numPlayers )
		.parseBYTE( avgPlayers )
		.parseINT32( avgPotSize )
		.parseINT32( handsPerHour )
		.parseBYTE( publProp )
		.parseINT32( avgStackSize )
		.parseBYTE( avgVpip )
		.parseMsgBody( fppFactorsBody )
		.parseMsgBody( fppFactorsBodyEx )
		;
}

SortedSeatFindersAndBlitzes::~SortedSeatFindersAndBlitzes()
{
	if( !ownsFinders )
		return;

	for( int i = 0; i < finders.size(); ++i )
	{
		delete finders[i];
	}
}

void SortedSeatFindersAndBlitzes::parse( CommMsgParser& parser )
{
	INT32 size;
	parser
		.parseINT32( totalNum )
		.parseINT32( size )
		;
	finders.reserve( size );
	for( int i = 0; i < size; ++i )
	{
		bool isBlitz;
		CommMsgBody body;
		parser
			.parseBOOL( isBlitz )
			.parseMsgBody( body )
			;

		SeatFinderBase* base = isBlitz ? (SeatFinderBase*) new ShadowBlitzForMobile() : (SeatFinderBase*) new ShadowSeatFinder();
		CommMsgParser parser1( body );
		base->parseForMobile( parser1 );
		finders.push_back( base );
	}
}

void SortedSeatFindersAndBlitzes::compose( CommMsgBody& msg ) const
{
	msg
		.composeINT32( totalNum )
		.composeUINT32( static_cast< UINT32 >( finders.size() ) )
		;
	for( int i = 0; i < finders.size(); ++i )
	{
		const SeatFinderBase* sf = finders[i];
		CommMsgBody body;
		sf->composeForMobile( body );
		msg
			.composeBOOL( sf->isBlitz() )
			.composeMsgBody( body )
			;
	}
}

void SortedSeatFindersAndBlitzes::clear()
{
	totalNum = 0;
	
	if( ownsFinders )
	{
		for( int i = 0; i < finders.size(); ++i )
		{
			delete finders[i];
		}
	}

	finders.clear();
}