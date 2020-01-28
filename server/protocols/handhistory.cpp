//handhistory.cpp

#include "handhistory.h"
#include "lobby.h"
#include "commssl.h"
#include "sitemasks.h"

void Hand::compose(CommMsgBody& body) const
{
//Always add new items to the END!
	// of the special msg body - PYR-29549
////// compose hand itself
	body.
		composeUINT32( (UINT32)tableId). // PYR-29549 - for old code
		composeUINT64( handId64).
		composeINT32( rakeSize ).
		composeINT32( potSize ).
		composeINT8( dealer ).
		composeSrvTime( started ).
		composeSrvTime( finished ).
		composeBYTE( newGameFlag ).
		composeINT32( loBet ).
		composeINT32( hiBet ).
		composeINT32( smallBlindSize ).
		composeINT32( ante ).
		composeINT32( bringIn ).
		composeBYTE( isOneOnOne ).
		composeBYTE( chipsType ).
		composeBYTE( game ).
		composeBYTE( isHiLo ).
		composeBYTE( structure ).
		composeBYTE( tournType ).
		composeUINT32( tournId ).
		composeUINT16( tournLevel ).
		composeINT32( tournBuyIn ).
		composeINT32( tournFppBuyIn ).
		composeINT32( tournRake ).
		composeINT8( tournIsPlayMoney ).
		composeString( board ).
		composeString( tableName ).
		composeString( currency );
		
//compose users:
	body.composeBYTE( (BYTE)users.size() );
	for ( int userIdx = 0; userIdx < users.size(); userIdx++ )
	{
		users[userIdx].compose( body );
	}
//compose history:
	body.composeUINT32( history.size() );
	for ( int historyIdx = 0; historyIdx < history.size(); historyIdx++ )
	{
		history[historyIdx].compose(body);
	}
//continue composing some additional stuff:
	body.
		composeINT32( tournBounty ).
		composeUINT32( cap ).
		composeString( clubName ).
		composeBYTE( speedDisplay ).
		composeBYTE( awardSchema );

	body.composeString( board2 );

	// now populate the user2 struct 
	// for non-RIT hand, users2 list might be empty
	body.composeBYTE( (BYTE)users2.size() );
	for ( int userIdx = 0; userIdx < users2.size(); userIdx++ )
	{
		CommMsgBody ritUser2Msg;
		users2[userIdx].compose( ritUser2Msg );
		body.composeMsgBody( ritUser2Msg );
	}
	body.composeUINT32( scalePM ) //PYR-23052
		.composeUINT32( handFlags )
		.composeString( aamsId )  //PYR-24738
		.composeUINT32( clubId );

	// All new members of Hand struct have to be composed inside this body!
	CommMsgBody additionalParamsBody;
	additionalParamsBody
		.composeUINT64( tableId ) // PYR-29549
		.composeUINT32( ordinalOfHandStart ) // PYR-20821
		.composeINT32(carryOverFromLastHand) // PYR-34850
		.composeINT32(carryOverToNextHand) // PYR-34850
		.composeINT32(dealerSuppliedFreeChips) // PYR-34850
		.composeINT64( buyIn64 ) // PYR-37127
		.composeINT64( rake64 ) // PYR-37127
		.composeINT32( tournPlaceDelta )//PYR-37049
		;

	// PYR-38782 Sending starting mana and powers.
	additionalParamsBody.composeBYTE( (BYTE)users.size() );
	for ( INT32 userIdx = 0; userIdx < users.size(); ++userIdx )
	{
		CommMsgBody userStormBody;
		userStormBody.composeINT32(users[userIdx].startingMana);
		userStormBody.composeBYTE((BYTE)users[userIdx].startingPowers.size());
		for(INT32 powerIdx = 0; powerIdx < users[userIdx].startingPowers.size(); ++powerIdx)
			userStormBody.composeUINT32(users[userIdx].startingPowers[powerIdx]);
		additionalParamsBody.composeMsgBody( userStormBody );
	}

	// PYR-65734 unfold game
	CommMsgBody extraPotBody;
	handExtraPot.compose(extraPotBody);
	composeVector(extraPotBody, usersExtraPot);
	additionalParamsBody.composeMsgBody(extraPotBody);
	
	// PYR-123658 Progressive+
	additionalParamsBody.composeUINT32( pppId );

	body.composeMsgBody( additionalParamsBody );
}

void Hand::parse(CommMsgParser& parser)
{ //Always add new items to the END!
	// of the special msg body at the end with conditional parsing.
	// PYR-29549 tableId32 not used anymore.
	UINT32 tableId32; // PYR-26400: temporary - until we move all tableIds to 64-bit
	parser.
		parseUINT32( tableId32 ).
		parseUINT64( handId64 ).
		parseINT32( rakeSize ).
		parseINT32( potSize ).
		parseINT8( dealer ).
		parseSrvTime( started ).
		parseSrvTime( finished ).
		parseBYTE( newGameFlag ).
		parseINT32( loBet ).
		parseINT32( hiBet ).
		parseINT32( smallBlindSize ).
		parseINT32( ante ).
		parseINT32( bringIn ).
		parseBYTE( isOneOnOne ).
		parseBYTE( chipsType ).
		parseBYTE( game ).
		parseBYTE( isHiLo ).
		parseBYTE( structure ).
		parseBYTE( tournType ).
		parseUINT32( tournId ).
		parseUINT16( tournLevel ).
		parseINT32( tournBuyIn ).
		parseINT32( tournFppBuyIn ).
		parseINT32( tournRake ).
		parseINT8( tournIsPlayMoney ).
		parseStringP( board ).
		parseStringP( tableName ).
		parseStringP( currency );
	
	tableId = tableId32; // PYR-26400 - temporary until all tableIds are 64-bit
	BYTE numUsers;
	parser.parseBYTE( numUsers );
	users.clear();
	for( BYTE userIdx = 0; userIdx < numUsers; userIdx++ )
	{
		UserInHand* user = &( *users.insert( users.end() ,UserInHand()) );
		user->parse( parser );
	}
	UINT32 numHistoryItems;
	parser.parseUINT32( numHistoryItems );
	history.clear();
	for( UINT32 itemIdx = 0; itemIdx < numHistoryItems; itemIdx++ )
	{
		TableHistoryItem* item = &( *history.insert( history.end(),TableHistoryItem() ) );
		item->parse( parser );
	}
	parser.
		parseINT32( tournBounty ).
		parseUINT32( cap ).
		parseStringP( clubName ).
		parseBYTE( speedDisplay ).
		parseBYTE( awardSchema );

	users2.clear();
	users2.resize( numUsers );

	parser.parseStringP( board2 );

	BYTE numUsers2;
	parser.parseBYTE( numUsers2 );

	if ( numUsers2 > numUsers )  // not possible, more players in board2 than in board1
	{
		char s[64];
		char s2[32]; // PYR-26400
		PLog(LOG_TRACE_FAULTY_LOGIC ": !Hand::parse: tableId=%s, handId=%s, numUsers2(%d) > numUsers(%d)", p_u64toa(tableId, s2), p_u64toa(handId64, s), numUsers2, numUsers );
		PASSERT5(0);
		return;  // severe error: do not continue
	}

	for( BYTE userIdx = 0; userIdx < numUsers2; userIdx++ )
	{
		CommMsgBody ritUser2Msg;
		parser.parseMsgBody( ritUser2Msg );
			
		CommMsgParser ritUser2MsgParser( ritUser2Msg );

		UserInHand2 u2;
		u2.parse( ritUser2MsgParser );

		if( u2.name.equals( users[ userIdx ].name ) )
		{
			users2[ userIdx ] = u2;
		}
		else
		{
			for( int k = 0; k < numUsers; ++k )
			{
				if( u2.name.equals( users[ k ].name ) )
				{
					users2[ k ] = u2;
					break;
				}
			}
		}
	}

	CommMsgBody additionalParamsBody; // PYR-29549 - added to hold 64-bit tableId and all future Hand members
	parser
		.parseUINT32( scalePM )	//PYR-23052
		.parseUINT32( handFlags )
		.parseStringP( aamsId )	// PYR-24738
		.parseUINT32( clubId )
		.parseMsgBody( additionalParamsBody ); // PYR-29549
	CommMsgParser additionalParamsParser( additionalParamsBody );
	additionalParamsParser
		.parseUINT64( tableId ) // PYR-29549
		.parseUINT32( ordinalOfHandStart ) // PYR-20821
		.parseINT32(carryOverFromLastHand)
		.parseINT32(carryOverToNextHand)
		.parseINT32(dealerSuppliedFreeChips)
		.parseINT64( buyIn64 ) // PYR-37127
		.parseINT64( rake64 ) // PYR-37127
		.parseINT32( tournPlaceDelta ) //PYR-37049 
		;

	// PYR-38782 Parsing starting mana and powers.
	BYTE numUsersStorm;
	additionalParamsParser.parseBYTE( numUsersStorm );
	PASSERT(numUsersStorm == numUsers);
	for ( INT32 userIdx = 0; userIdx < numUsersStorm; ++userIdx )
	{
		CommMsgBody userStormBody;
		additionalParamsParser.parseMsgBody(userStormBody);
		CommMsgParser userStormParser(userStormBody);
		userStormParser.parseINT32(users[userIdx].startingMana);
		BYTE numPowers;
		userStormParser.parseBYTE(numPowers);
		for(INT32 powerIdx = 0; powerIdx < numPowers; ++powerIdx)
		{
			UINT32 powerId;
			userStormParser.parseUINT32(powerId);
			users[userIdx].startingPowers.push_back(powerId);
		}
	}

	// PYR-65734, unfold game
	CommMsgBody extraPotBody;
	additionalParamsParser.parseMsgBody(extraPotBody);

	CommMsgParser extraPotParser(extraPotBody);
	handExtraPot.parse(extraPotParser);
	parseVector(extraPotParser, usersExtraPot);

	if( !additionalParamsParser.parseEnded() )
	{
		additionalParamsParser.parseUINT32( pppId ); // PYR-123658
	}
	
	// new Hand members should be added at the end of additionalParamsBody with conditional parsing
	// Please notify Game and Payment security team of changes and additions
}

// PYR-26400
// New parse/compose methods used for new message DBM_Q_HAND and DBM_Q_HAND_OLAP_NEW
void UserInHand::composeByTable( CommMsgBody& body ) const
{
	CommMsgBody userBody;
	// compose new members at the end!
	userBody
		.composeString( name )
		.composeBYTE( position )
		.composeINT32( startAmount )
		.composeINT32( delta )
		.composeINT32( result)
		.composeString( cards )
		.composeBYTE( bestHand )
		.composeString( combHi )
		.composeString( combLo )
		.composeINT32( fpp )
		.composeINT32( fractionFpp )
		.composeINT32( potChips )
		.composeINT32( dbStartAmount )
		.composeString( playerIP )
		.composeUINT32( licenseId )
		.composeINT32( unusedStacks )
		.composeINT32( unusedStacksHandStart)
		.composeUINT32( userIntId )
		.composeINT32( startingMana )
		.composeBYTE( (BYTE)startingPowers.size() );
	for(UINT32 powerId : startingPowers)
		userBody.composeUINT32(powerId);
	userBody
		.composeUINT64( buyInTransId ) // PYR-38802
		.composeUINT32( clientDevice ) // PYR-38802
		.composeUINT64(appLoginId) //PYR-41940
		.composeUINT32(clientBrand) //PYR-41940
		;

	// PYR-95110
	userBody
		.composeBOOL( hasCashedOut )
		.composeINT32( equityClientView )
		.composeINT32( equityServerView )
		.composeINT32( cashoutAmountOffered )
		.composeINT32( cashoutRakeCalculated )
		.composeINT32( houseWinningAmount )
		;

	body.composeMsgBody( userBody );
}

void UserInHand::parseFromTable( CommMsgParser& parser )
{
	CommMsgBody userBody;
	parser.parseMsgBody( userBody );
	CommMsgParser userParser( userBody );
	// parse new members at the end with guard!
	userParser
		.parseStringP( name )
		.parseBYTE( position )
		.parseINT32( startAmount )
		.parseINT32( delta )
		.parseINT32( result )
		.parseStringP( cards )
		.parseBYTE( bestHand )
		.parseStringP( combHi )
		.parseStringP( combLo )
		.parseINT32( fpp )
		.parseINT32( fractionFpp )
		.parseINT32( potChips )
		.parseINT32( dbStartAmount )
		.parseStringP( playerIP )
		.parseUINT32( licenseId )
		.parseINT32( unusedStacks )
		.parseINT32( unusedStacksHandStart)
		.parseUINT32( userIntId );
	// PYR-38782 - Adding new fields for user's starting mana and starting powers.
	BYTE numPowers = 0;
	userParser
		.parseINT32( startingMana )
		.parseBYTE( numPowers );
	for( INT32 i = 0; i < numPowers; ++i )
	{
		UINT32 powerId;
		userParser.parseUINT32( powerId );
		startingPowers.push_back( powerId );
	}
	userParser
		.parseUINT64( buyInTransId ) // PYR-38802
		.parseUINT32( clientDevice ) // PYR-38802
		.parseUINT64(appLoginId) // PYR-41940
		.parseUINT32(clientBrand) // PYR-41940
		.parseBOOL( hasCashedOut )
		.parseINT32( equityClientView )
		.parseINT32( equityServerView )
		.parseINT32( cashoutAmountOffered )
		.parseINT32( cashoutRakeCalculated )
		.parseINT32( houseWinningAmount )
		;

	// Please notify Game and Payment security team of changes and additions
}

void UserInHand2::composeByTable( CommMsgBody& body ) const
{
	CommMsgBody user2Body;
	// add additional members at the end!
	user2Body
		.composeString( name )
		.composeBYTE( bestHand2 )
		.composeString( combHi2 )
		.composeString( combLo2 )
		;

	body.composeMsgBody( user2Body );
}

void UserInHand2::parseFromTable( CommMsgParser& parser )
{
	CommMsgBody user2Body;
	parser.parseMsgBody( user2Body );
	CommMsgParser user2Parser( user2Body );
	// add additional members at the end with the guard!
	user2Parser
		.parseStringP( name )
		.parseBYTE( bestHand2 )
		.parseStringP( combHi2 )
		.parseStringP( combLo2 )
		;

	// Please notify Game and Payment security team of changes and additions
}

void UserInHandExtra::composeByTable( CommMsgBody& body ) const
{
	CommMsgBody userExtraBody;
	// add additional members at the end!
	userExtraBody
		.composeString( arjelUserHash )
		;

	body.composeMsgBody( userExtraBody );
}

void UserInHandExtra::parseFromTable( CommMsgParser& parser )
{
	CommMsgBody userExtraBody;
	parser.parseMsgBody( userExtraBody );
	CommMsgParser userExtraParser( userExtraBody );
	// add additional members at the end with the guard!
	userExtraParser
		.parseStringP( arjelUserHash )
		;

	// Please notify Game and Payment security team of changes and additions
}

void Hand::parseFromTable( CommMsgParser& parser )
{
	UINT32 siteMaskInHandOld;
	//Always add new items to the END with the parseEnded() condition!
	parser
		.parseUINT64(tableId)		// changed to 64-bit!
		.parseStringP(tableName)
		.parseUINT64(handId64)
		.parseINT32(rakeSize)
		.parseINT32(potSize)
		.parseINT8(dealer)			// DEALERPOS in DB
		.parseBYTE(newGameFlag)		// ISNEWGAME in DB
		.parseINT32(loBet)
		.parseINT32(hiBet)
		.parseINT32(smallBlindSize)
		.parseINT32(ante)
		.parseINT32(bringIn)
		.parseBYTE(isOneOnOne)		// max number of players
		.parseBYTE(chipsType)		// ISPLAYMONEY in DB
		.parseUINT32(handFlags)
		.parseBYTE(game)			// game and isHiLo translated into GAME column in DB
		.parseBYTE(isHiLo)
		.parseBYTE(structure)
		.parseBYTE(tournType)
		.parseUINT32(tournId)
		.parseUINT16(tournLevel)
		.parseStringP(board)
		.parseSrvTime(started)
		.parseSrvTime(finished)
		.parseStringP(currency)
		.parseUINT32(siteMaskInHandOld) // #13901
		.parseUINT32(cap) // #17335 (composed as INT32)
		.parseUINT32(clubId)
		.parseUINT32( scalePM ); //PYR-23052
	parseHandHistory( parser, tableId, history );
	parseHandHistoryZip( parser, tableId, historyZip);

	BYTE num_users;
	parser.parseBYTE( num_users );
	// get information about each user in this hand
	users.resize( num_users );
	usersExtra.resize( num_users );
	for( int n = 0; n < num_users; ++n )
	{
		users[n].parseFromTable( parser );
		usersExtra[n].parseFromTable( parser );
	}

	// now parse the second board info if RIT
	CommMsgBody ritSecondBoardInfo;
	parser.parseMsgBody( ritSecondBoardInfo );

	if( (handFlags & hFlagRunItTwice) != 0 )   // RIT enabled hand
	{
		CommMsgParser ritParser( ritSecondBoardInfo );
		ritParser.parseStringP( board2 );

		BYTE num;
		ritParser.parseBYTE( num );
		PASSERT( num == num_users );

		users2.resize( num );
		for( int n = 0; n < num; ++n )
		{
			users2[n].parseFromTable( ritParser );

			// the players in users2 must be in the same sequence as the vector users
			PASSERT5( users2[n].name.equals( users[n].name ) );
		}
	}

	parser
		.parseUINT32(finalFlightTournId)
		.parseINT32(carryOverFromLastHand)  // PYR-34850
		.parseINT32(carryOverToNextHand)
		.parseINT32(dealerSuppliedFreeChips)
		.parseINT8(nextButtonPos)  // PYR-32587
		;

	// PYR-65734, unfold game
	CommMsgBody extraPotBody;
	parser.parseMsgBody(extraPotBody);

	CommMsgParser extraPotParser(extraPotBody);
	handExtraPot.parse(extraPotParser);
	parseVector(extraPotParser, usersExtraPot);
	
	parser
		.parseObject( siteMaskInHand ) // PYR-100668
		;

	// MDS TODO - remove conditional parsing after both NJ and PA are on 2020.Build.02
	if( !parser.parseEnded() )
	{
		parser.parseUINT32( pppId ); // PYR-123658
	}

	// Please notify Game and Payment security team of changes and additions
}


////////////////////////////////////////////////////////////////////////////////

void UserInHand::compose(CommMsgBody& body) const
{
	body.
		composeString( name ).
		composeBYTE( position ).
		composeINT32( startAmount ).
		composeINT32( delta ).
		composeINT32( result).
		composeString(cards ).
		composeBYTE(bestHand).
		composeString(combHi).
		composeString(combLo).
		composeINT32(fpp).
		composeINT32(fractionFpp).
		composeINT32(potChips).
		composeINT32(dbStartAmount);
}

void UserInHand::parse(CommMsgParser& parser)
{
	parser.
		parseStringP( name ).
		parseBYTE( position ).
		parseINT32( startAmount ).
		parseINT32( delta ).
		parseINT32( result ).
		parseStringP( cards ).
		parseBYTE(bestHand).
		parseStringP(combHi).
		parseStringP(combLo).
		parseINT32(fpp).
		parseINT32(fractionFpp).
		parseINT32(potChips).
		parseINT32(dbStartAmount);

	// Please notify Game and Payment security team of changes and additions
}

void UserInHand2::compose(CommMsgBody& body) const
{
	// add additional members at the end!
	body.
		composeString( name ).
		composeBYTE( bestHand2 ).
		composeString( combHi2 ).
		composeString( combLo2 );
}

void UserInHand2::parse(CommMsgParser& parser)
{
	// add additional members at the end with the parseEnded() condition!
	parser.
		parseStringP( name ).
		parseBYTE( bestHand2 ).
		parseStringP( combHi2 ).
		parseStringP( combLo2 );

	// Please notify Game and Payment security team of changes and additions
}

void TableHistoryItem::compose(CommMsgBody& body) const
{
	body.
		composeUINT32(ordinal).
		composeSrvTime(when).
		composeUINT16(eventCode).
		composeINT8(position).
		composeINT8(action).
		composeUINT32(amount).
		composeString( userId ).
		composeString( contents );
}

void TableHistoryItem::parse(CommMsgParser& parser)
{
	parser.
		parseUINT32(ordinal).
		parseSrvTime(when).
		parseUINT16(eventCode).
		parseINT8(position).
		parseINT8(action).
		parseUINT32(amount).
		parseStringP( userId ).
		parseStringP( contents );

	// Please notify Game and Payment security team of changes and additions
}

////////////////////////////////////////////////////////////////////////////////
// [AO 2010-11-16] Extracted from DBM

void parseHandHistory(CommMsgParser& parser, UINT64 tableId, TableHistory& history)
{
	UINT16 numFrames;
	parser.parseUINT16(numFrames);

	for (int frameIdx = 0; frameIdx < numFrames; frameIdx++)
	{
		//F = frame: 42<h> hand_id, num_items, [HistoryItem...] //// hand_Id = 0 means no_hand history Frame
		UINT64 frameHandId64;
		UINT16 numItems;
		parser.
			parseUINT64(frameHandId64).
			parseUINT16(numItems);

		for (int itemIdx = 0; itemIdx < numItems; itemIdx++)
		{
			UINT32 tmp_when; // hack, 1 of 3
			TableHistory::iterator it = history.insert(history.end(),TableHistoryItem());
			TableHistoryItem& item = *it;
			item.tableId = tableId;
			item.handId64 = frameHandId64;
			parser.
				parseUINT32(item.ordinal).
				parseUINT32(tmp_when). // hack, 2 of 3
				parseUINT16(item.eventCode).
				parseINT8(item.position).
				parseINT8(item.action).
				parseUINT32(item.amount).
				parseStringP(item.userId).
				parseStringP(item.contents);

			item.when.decode((time_t)tmp_when); //hack, 3 of 3
		}
	}
}


void parseHandHistoryZip(CommMsgParser& parser, UINT64 tableId, TableHistoryZip& zippedItems)
{
	UINT16 numFrames;
	parser.parseUINT16(numFrames);
	zippedItems.resize(numFrames);

	for (int i = 0; i < numFrames; ++i)
	{
		TableHistoryItemZip& itemZip = zippedItems[i];
		itemZip.tableId = tableId;
		parser.
			parseUINT64(itemZip.handId64).	// frameHandId64
			parseStringP(itemZip.encoded).	// encodedHistory
			parseUINT32(itemZip.ordinal);	// maxOrdinal
	}
}

////////////////////////////////////////////////////////////////////////////////
// PYR-74625: When using this method, please ensure the message being decompressed DOESN'T come from the end users, due to known vulnerabilities in zlib.
static void unzipBody(CommMsgBody& histBody, PBlock& block, int zipMethod)
{
    histBody.clear();
    switch( zipMethod )
    {
    case 0:
        memcpy( histBody._append( block.size() ), block.ptr(), block.size() ); //lint !e864 ... depends on order of evaluation
        break;
    case 1:
        {
            PMemInputStream stream( block );
            PInflate inflator( stream );
            for(;;)
            {
                BYTE buf[ 1024 ];
                size_t rd = inflator.read( buf, sizeof(buf) );
                if( rd == 0 )
                    break;//forever
                memcpy( histBody._append( rd ), buf, rd );
            }
            inflator.close();
        }
        break;
    default:
        PASSERT( 0 );
        break;
    }
}

// [AO 2010-11-27] PYR-16750
void extractHistoryZip(const char* bodyStr, UINT64 handId, UINT64 tableId, TableHistory& table)
{
    PBlock		zippedBlock;
    CommMsgBody historyBody;

    int zipMethod = *bodyStr - '0';
    CommSSLBase64::decode( zippedBlock, (bodyStr) + 1);
    unzipBody(historyBody, zippedBlock, zipMethod);

    UINT16 nEvents;
    CommMsgParser parser( historyBody );
    parser.parseUINT16(nEvents);

    UINT32	baseOrdinal = 0;
    time_t	timeBase_t = 0;
    bool isFirst = true;  // first event has base SrvTime and base Ordinal
    // all others have delta from the first
    for (int n = 0; n < nEvents; n++)
    {
        TableHistoryItem item;
        UINT32			 timeDelta;
        item.handId64  =  handId;
        item.tableId = tableId;

        if (isFirst)                 
        {
            parser.parseUINT32(item.ordinal);
            item.when.parse(parser);
            parser
                .parseUINT16(item.eventCode)
                .parseINT8(item.position)
                .parseINT8(item.action)
                .parseUINT32(item.amount)
                .parseStringP(item.userId )
                .parseStringP(item.contents);
            timeBase_t  = item.when.encode();
            baseOrdinal = item.ordinal;
            isFirst     = false;
        }
        else
        {
            UINT32 deltaOrdinal;
            parser	
                .parseUINT32(deltaOrdinal)
                .parseUINT32(timeDelta)
                .parseUINT16(item.eventCode)
                .parseINT8(item.position)
                .parseINT8(item.action)
                .parseUINT32(item.amount)
                .parseStringP(item.userId )
                .parseStringP(item.contents);
            item.when.decode(timeBase_t + timeDelta);
            timeBase_t   = timeBase_t + timeDelta;
            item.ordinal = baseOrdinal + deltaOrdinal;
            baseOrdinal  = item.ordinal;
        }
        table.emplace_back(move(item));
    }
}

void getMaxOrdinal(const TableHistory& history, const TableHistoryZip& historyZip, UINT32& maxOrdinal, UINT64& tableId)
{
	tableId = 0;
	maxOrdinal = 0;
	int n;
	for (n = 0; n < history.size(); n++ )
	{
		const TableHistoryItem& item = history[n];
		tableId = item.tableId;
		if( item.ordinal > maxOrdinal )
			maxOrdinal = item.ordinal;
	}
	for (n = 0; n < historyZip.size(); n++ )
	{
		const TableHistoryItemZip& item = historyZip[n];
		tableId = item.tableId;
		if( item.ordinal > maxOrdinal )
			maxOrdinal = item.ordinal;
	}
}

////////////////////////////////////////////////////////////////////////////////

void HandExtraPotInfo::compose(CommMsgBody& body) const
{
	// add additional members at the end!
	CommMsgBody m;
	m
		.composeINT32(potSize)
		.composeINT32(rake)
		.composeUINT32(potFlags)
		;
	body.composeMsgBody(m);
}

void HandExtraPotInfo::parse(CommMsgParser& parser)
{
	// add additional members at the end with the guard!
	CommMsgBody m;
	parser.parseMsgBody(m);
	CommMsgParser mParser(m);
	mParser
		.parseINT32(potSize)
		.parseINT32(rake)
		.parseUINT32(potFlags)
		;

	// Please notify Game and Payment security team of changes and additions
}


void UserInHandExtraPot::compose(CommMsgBody& body) const
{
	// add additional members at the end!
	CommMsgBody m;
	m
		.composeString(userId)
		.composeINT32(potChips)
		.composeINT32(delta)
		;
	body.composeMsgBody(m);
}

void UserInHandExtraPot::parse(CommMsgParser& parser)
{
	// add additional members at the end with the guard!
	CommMsgBody m;
	parser.parseMsgBody(m);
	CommMsgParser mParser(m);
	mParser
		.parseStringP(userId)
		.parseINT32(potChips)
		.parseINT32(delta)
		;

	// Please notify Game and Payment security team of changes and additions
}


