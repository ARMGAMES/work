#include "ppinclude.h"
#include "commcli.h"

#include "table.h"
#include "clitable2.h"
#include "climulticurr.h"
#include "i18n.locales.h"

#include "cliutils.h"
#include "storm\TableClientProtocols.h"

TableClientData2004::TableClientData2004() //@@@ Michaelp consider notifying client team (client code?)
{
	_tableId = _tableBits = _accessMask = 0;
	_handId64 = _handIdLast64 = _milestoneHandId = 0;
	_loBet = _hiBet = _ante = _round = _multiGameHands = 0;
	_multiGameMultiplier = 2;
	_gameType = _handType = _potStruct = _tournamentType = 0;
	_maxTablePlayers = 0;
	_activeTBstate = _activeTimeBank = 0;
	_dealer = _rake = _tempPot = 0;
	_nPots = _nColumns = 0;
	memset( _nBoard, 0, sizeof( _nBoard ) );
	_chatLang = _loBetLM = _hiBetLM = 0; 
	_minBuyIn = _maxBuyIn = _cap = _capX = _capY = 0;
	_isHiLo = _isPlayMoney = _isOneToOne = false;
	_clubId = _tableServFlags = 0;
	_nSubscribers = -1;
	_banner = -1;
	_showCity = true;
	_scalePM = 1;

	_actionTimeType = 0;
	_ActionTimeTotal = _actionTimeRemaining = 0;


	int i;
	for( i = 0; i < MaxPlayers; ++i )
	{
		Player& player = players[ i ];
		player.cartoonName = 0;
		player.connQuality = 0;
		player.indicator = 0;
		player.bounty = 0;
		player.headBounty = 0;
		player.mileStoneAward = 0;
		player.chips = 0;
		player.chipsBet = 0;
		player.totalBet = 0;
		player.capN = 0;
		player.sittingOut = 1;
		player.connected = false;
		player.inHand = false;
		player.isAllIn = false;
		player.isCapped = false;
		player.vipStatus = eVipUndefined;
		player.hallOfFame = 0;
		player.publPropFlag = 0;
		player.nCards = 0;
		for( int j = 0; j < MaxPlayerCards; ++j ) 
		{
			player.cards[ j ].suit = 0x0;
			player.cards[ j ].rank = 0x0;
			player.cardsShown[ j ] = Closed;
		}

		playerUpdateFlags[ i ] = 0;

		_pot[ i ] = 0;
	}

	updateFlags = 0;
	nChatAdded = nChatDeleted = 0;

	for( i = 0; i < MAX_TABLE_COLUMNS; ++i )
		_column[ i ] = 0;
}

TableClientData2004::TableClientData2004( const TableClientData2004& other ) //@@@ Michaelp consider fixing (client code?)
{
	_tableId = _tableBits = 0;
	_handId64 = _milestoneHandId = 0;
	_handIdLast64 = other._handIdLast64;
	_loBet = other._loBet;
	_hiBet = other._hiBet;
	_ante = other._ante;
	_round = _multiGameHands = 0;
	_multiGameMultiplier = 2;
	_gameType = other._gameType;
	_handType = other._handType;
	_potStruct = other._potStruct;
	_tournamentType = other._tournamentType;
	_maxTablePlayers = other._maxTablePlayers;
	_activeTBstate = _activeTimeBank = 0;
	_dealer = _rake = _tempPot = 0;
	_nPots = _nColumns = 0;
	memset( _nBoard, 0, sizeof( _nBoard ) );
	_chatLang = other._chatLang;
	_loBetLM = other._loBetLM;
	_hiBetLM = other._hiBetLM;
	_minBuyIn = other._minBuyIn;
	_maxBuyIn = other._maxBuyIn;
	_cap = other._cap;
	_capX = other._capX;
	_capY = other._capY;
	_isHiLo = other._isHiLo;
	_isPlayMoney = other._isPlayMoney;
	_isOneToOne = other._isOneToOne;
	_clubId = other._clubId;
	_showCity = other._showCity;

	_protocolVer = other._protocolVer;
	_tableName = other._tableName;
	_nameSuffix = other._nameSuffix;
	//_advertiseMsg = other._advertiseMsg;
	_multiGameDescr = other._multiGameDescr;
	_currency = other._currency;
	_itSessionId = other._itSessionId;
	_advertiseMsgI18n = other._advertiseMsgI18n;

	_createdBy = other._createdBy;
	_cancelledBy = other._cancelledBy;

	_voiceObjectCommandAddress = other._voiceObjectCommandAddress;
	_voiceObjectDataAddress = other._voiceObjectDataAddress;
	_voiceObjectName = other._voiceObjectName;
	_accessMask = other._accessMask;
	_scalePM = other._scalePM;
	_nSubscribers = other._nSubscribers;
	_tableServFlags = other._tableServFlags;

	int i;
	for( i = 0; i < MaxPlayers; ++i )
	{
		Player& player = players[ i ];
		player.cartoonName = 0;
		player.connQuality = 0;
		player.indicator = 0;
		player.bounty = 0;
		player.headBounty = 0;
		player.mileStoneAward = 0;
		player.chips = 0;
		player.chipsBet = 0;
		player.totalBet = 0;
		player.capN = 0;
		player.sittingOut = 1;
		player.connected = false;
		player.inHand = false;
		player.isAllIn = false;
		player.isCapped = false;
		player.vipStatus = eVipUndefined;
		player.hallOfFame = 0;
		player.publPropFlag = 0;

		player.nCards = 0;
		for( int j = 0; j < MaxPlayerCards; ++j ) 
		{
			player.cards[ j ].suit = 0x0;
			player.cards[ j ].rank = 0x0;
			player.cardsShown[ j ] = Closed;
		}

		playerUpdateFlags[ i ] = 0;

		_pot[ i ] = 0;
	}

	updateFlags = 0;
	nChatAdded = nChatDeleted = 0;

	for( i = 0; i < MAX_TABLE_COLUMNS; ++i )
		_column[ i ] = 0;
}

void TableClientData2004::beginTrans()
{
	//	PTRACE2( "beginTrans {" );
	userBeginTrans();
	attachments.clear();

	updateFlags = 0;
	nChatAdded = nChatDeleted = 0;
	for( int i = 0; i < MaxPlayers; ++i )
		playerUpdateFlags[ i ] = 0;
	//	PTRACE2( "beginTrans }" );
}

void TableClientData2004::commitTrans()
{
	//	PTRACE2( "commitTrans {" );
	int i, n = attachments.size();
	for( i = 0; i < n; ++i )
		userOnlineUpdateAttachment( attachments[ i ]->id, attachments[ i ]->body );
	attachments.clear();

	if( updateFlags & _dealOptionChanged )
		dealOptionChanged();
	if( updateFlags & _chatChanged )
		chatChanged( -1, -1 );
	else if( nChatAdded > 0 || nChatDeleted > 0 )
		chatChanged( nChatAdded, nChatDeleted );

	if( updateFlags & _tableChanged )
		tableChanged();

	if( updateFlags & _gameTypeChanged )
		gameTypeChanged();

	if( updateFlags & _tableFppFactorChanged )
		tableFppFactorChanged();

	if( updateFlags & _tableDynamicChanged )
		tableDynamicChanged();

	for( i = 0; i < MaxPlayers; ++i )
		if( playerUpdateFlags[ i ] & 0x01 )
			playerChanged( i );
	userCommitTrans();
	//	PTRACE2( "commitTrans }" );
}

void TableClientData2004::onlineUpdateAttachment( UINT32 id, CommMsgBody& body )
{
	_Attachment* att = new _Attachment();
	att->id = id;
	att->body.moveFrom( body );
	attachments.push_back( att );
}

void TableClientData2004::leafAppended( const Path& path, const CommMsgBody& msg )
{
	//	PTRACE2( "leafAppended" );
	if( path.pathLen() == 1 && path.path()[ 0 ] == 0 )
		++nChatAdded;
	//		updateFlags |= _chatChanged;
}

void TableClientData2004::leafRemoved( const Path& path )
{
	if( path.pathLen() == 2 && path.path()[ 0 ] == 0 )
	{
		if( path.path()[ 1 ] == 0 )
			++nChatDeleted;
		else//should not happen, just in case
			updateFlags |= _chatChanged;
	}
}

void TableClientData2004::leafUpdated( const Path& path, const CommMsgBody& msg )
{
	//	PTRACE2( "leafUpdated" );
	if( path.pathLen() == 1 )
	{
//int nLeaf = path.path()[ 0 ];
		switch( path.path()[ 0 ] )
		{
		case 1: 
			_loadStatic();
			updateFlags |= ( _tableChanged | _gameTypeChanged | _tableFppFactorChanged );
			break;
		case 2:
			_loadHand();
			updateFlags |= _tableChanged;
			break;
		case 3:
			_loadActive();
			updateFlags |= _tableChanged;
			break;
		//case 4: former _loadBoard() free to use after Mobile client finally released
		//case 5: Player
		case 6:
			_loadBoardRit();
			updateFlags |= _tableChanged;
			break;
		case 7:
			_loadDynamic(); 
			updateFlags |= _tableDynamicChanged;
			break;
		}
//PTRACE( "......leafUpdated: %d, _nSubscribers=%d", nLeaf, _nSubscribers );
	}
	else if( path.pathLen() == 3 && path.path()[ 0 ] == 5 )
	{
		int player = path.path()[ 1 ];
		if( path.path()[ 2 ] == 0 )
			_loadPlayerChips( player );
		else if( path.path()[ 2 ] == 1 )
			_loadPlayerUser( player );
		else if( path.path()[ 2 ] == 2 )
			_loadPlayerCards( player );
		playerUpdateFlags[ player ] |= 0x01;
	}
}

void TableClientData2004::nodeChanged( const Path& path )//also called with pathLen() == 0 for 'global update'
{
	if( path.pathLen() == 1 && path.path()[ 0 ] == 0 )
		updateFlags |= _chatChanged;
}

size_t TableClientData2004::chatSize() const
{
	const int chatPath[] = { 0, 0 };
	return CommClientSubscriber::size( chatPath, 1 );
}

const CommMsgBody& TableClientData2004::eventAt( int i ) const
{
	const int chatPath[] = { 0, i };
	return CommClientSubscriber::leaf( chatPath, 2 );
}

int TableClientData2004::loadEvent( int i, TableHistoryItem& event, CommMsgBody& extra ) const
{
	return parseEvent( eventAt( i ), event, extra );
}

int TableClientData2004::parseEvent( const CommMsgBody& commMsgBody, TableHistoryItem& event, CommMsgBody& extra ) const
{
	CommMsgParser parser( commMsgBody );
	
	INT32 amount2;
	parser.
		parseUINT32( event.ordinal ).
		parseUINT64( event.handId64 ).
		parseUINT16( event.eventCode ).
		parseINT8( event.position ).
		parseINT8( event.action ).
		parseUINT32( event.amount ).
		parseStringP( event.userId ).
		parseStringP( event.contents ).
		parseINT32( amount2 ).
		parseMsgBody( extra );

	return amount2;
}

void TableClientData2004::synchronized()
{
	_loadStatic();
	_loadHand();
	_loadActive();
	_loadDynamic();
	_loadBoardRit();

	for( int i = 0; i < MaxPlayers; ++i )
	{
		_loadPlayerChips( i );
		_loadPlayerUser( i );
		_loadPlayerCards( i );

		playerUpdateFlags[ i ] |= 0x01;
	}

	updateFlags |= _chatChanged|_tableChanged;
}

void TableClientData2004::_loadStatic()
{
	const int staticPath[] = { 1 };
	const CommMsgBody& msg = leaf( staticPath, 1 );

	const char* _serverVer;
	BYTE isHiLo, isPlayMoney, isOneToOne;

	CommMsgParser parser( msg );
	parser.
		parseString( _serverVer ).parseStringP( _protocolVer ).parseUINT32( _tableId ).
		parseBYTE( _handType ).parseBYTE( _potStruct ).parseBYTE( _tournamentType ).
		parseBYTE( isHiLo ).parseBYTE( isPlayMoney ).parseBYTE( isOneToOne ).
		parseBYTE( _maxTablePlayers ).parseStringP( _tableName ).
		parseStringP( _nameSuffix ).parseBYTE( _gameType ).parseStringP( _multiGameDescr ).
		parseINT32( _loBetLM ).parseINT32( _hiBetLM ).parseUINT16( _chatLang ).
		parseINT32( _minBuyIn ).parseINT32( _maxBuyIn );

	_parseCurrency( parser, _currency );
	BYTE save = _dealOptions;
	parser.
		parseINT32( _cap ).
		parseStringP( _itSessionId ).
		parseUINT32( _clubId ).
		parseBYTE( _dealOptions ).
		parseBOOL( _showCity ).
		parseStringP( _createdBy ).
		parseStringP( _cancelledBy ).
		parseStringP( _voiceObjectCommandAddress ).
		parseStringP( _voiceObjectDataAddress ).
		parseStringP( _voiceObjectName ).
		parseUINT32( _accessMask );

	if( !parser.parseEnded() )
	{
		UINT32 temp;
		parser.parseUINT32( temp );
		_scalePM = temp;
	}
	else
		_scalePM = 1;

	if( save != _dealOptions )
		updateFlags |= _dealOptionChanged;

	_isHiLo = isHiLo != 0;
	_isPlayMoney = isPlayMoney != 0;
	_isOneToOne = isOneToOne != 0;

	vector< LobbyListSubscriber_FppFactor > _tableFppFactor; // PYR-24398
	if( !parser.parseEnded() )
		LobbyListSubscriber_FppFactor::parseFppFactorBody( parser, _tableFppFactor );
	tableFppFactor.swap( _tableFppFactor );

	if( !parser.parseEnded() )
		parser.parseUINT32( _tableServFlags );
}

void TableClientData2004::_loadHand()
{
	const int dealerPath[] = { 2 };
	const CommMsgBody& msg = leaf( dealerPath, 1 );
	CommMsgParser parser( msg );

	UINT32 loBet, hiBet, ante;
	UINT32 tableBits = 0;
	BYTE dealer;
	INT8 round;
	INT32 multiGameHands = 0;
	UINT16 msgStrId = 0;
	const char* _advertiseMsg_dummy;

	parser.
		parseUINT64( _handId64 ).
		parseUINT32( loBet ).
		parseUINT32( hiBet ).
		parseBYTE( dealer ).
		parseUINT64( _handIdLast64 ).
		parseString( _advertiseMsg_dummy ).
		parseINT8( round ).
		parseUINT32( ante ).
		parseUINT32( tableBits ).
		parseINT32( multiGameHands ).
		parseUINT16( msgStrId ).
		parseObject( _advertiseMsgI18n ).
		parseUINT64( _milestoneHandId ).
		parseStringP( _superstarShowdownResults ); // #18541

	if( !parser.parseEnded() )
		parser.parseINT32( _banner );
	else
		_banner = -1;

	if( _tableBits != tableBits )
	{
		UINT32 oldVal = _tableBits;
		_tableBits = tableBits;
		tableBitChanged( oldVal, tableBits );
	}

	_round = round;
	_loBet = loBet;
	_hiBet = hiBet;
	_ante = ante;
	_dealer = dealer;
	_multiGameHands = multiGameHands;
	_msgStrId = (ClientMsgStrMap) msgStrId;
}

void TableClientData2004::_loadActive()
{
	const int activePath[] = { 3 };
	const CommMsgBody& msg = leaf( activePath, 1 );
	CommMsgParser parser( msg );
	BYTE activePlayer;
	UINT32 rake, tempPot;
	BYTE nPots;
	BYTE nColumns;

	parser.parseBYTE( activePlayer ).parseUINT32( rake ).parseBYTE( nPots );
	_activePlayer = activePlayer;
	_rake = rake;
	_nPots = nPots;
	int i;
	for( i = 0; i < nPots; ++i )
	{
		UINT32 amount;
		parser.parseUINT32( amount );
		_pot[ i ] = amount;
	}

	parser.parseBYTE( nColumns );
	PASSERT ( nColumns <= MAX_TABLE_COLUMNS );
	_nColumns = nColumns;
	for( i = 0; i < nColumns; ++i )
	{
		UINT32 column;
		parser.parseUINT32( column );
		_column[ i ] = scaleAmount( column );
	}

	parser.
		parseUINT32( tempPot ).
		parseBYTE( _activeTBstate ).
		parseUINT32( _activeTimeBank ).
		parseINT32( _capX ).
		parseINT32( _capY );

	if( !parser.parseEnded() )
	{
		// PYR-26697 (chapter 6)
		parser.
			parseBYTE( _actionTimeType ).
			parseUINT32( _ActionTimeTotal ).
			parseUINT32( _actionTimeRemaining );
	}

	_tempPot = tempPot;
}

#if !defined( SZARR )
#define SZARR(a) (sizeof(a)/sizeof(*(a)))
#endif

void TableClientData2004::_loadDynamic()
{ //PYR-22393:
	if( root.children.size() < 8 ) // to make it work with current production server LE 2013/03/01
		return;
	const int dynamicPath[] = { 7 };
	const CommMsgBody& msg = leaf( dynamicPath, 1 );
	CommMsgParser parser( msg );
	parser.parseINT32( _nSubscribers );
}

void TableClientData2004::_loadBoardRit()
{
	if( root.children.size() < 7 )
		return;

	const int boardPath[] = { 6 };
	const CommMsgBody& msg = leaf( boardPath, 1 );
	CommMsgParser parser( msg );

	memset( _nBoard, 0, sizeof( _nBoard ) );
	memset( _board, 0, sizeof( _board ) );

	BYTE numOfRITBoards;
	parser.parseBYTE( numOfRITBoards );

	PASSERT( numOfRITBoards <= 2 );

	BYTE nBoard, suit, rank;

	if( numOfRITBoards == 1 ) // regular deal
	{
		BYTE nBoard;
		parser.parseBYTE( nBoard );
		PASSERT( nBoard <= SZARR( _board[ 0 ] ) );
		_nBoard[ 0 ] = nBoard;
		for( int i = 0; i < nBoard; ++i )
		{
			BYTE suit, rank;
			parser.parseBYTE( suit ).parseBYTE( rank );
			_board[ 0 ][ i ].suit = suit;
			_board[ 0 ][ i ].rank = rank;
		}
	}
	else
	{
		for( int i = 1 ; i < 3 && !parser.parseEnded() ; i ++ )
		{	
			parser.parseBYTE( nBoard );
			_nBoard[ i ] = nBoard;
			PASSERT( nBoard <= SZARR( _board[ 0 ] ) );
			for( int j = 0 ; j < nBoard ; ++j )
			{
				parser.parseBYTE( suit ).parseBYTE( rank );
				_board[ i ][ j ].suit = suit;
				_board[ i ][ j ].rank = rank;
			}
		}

		_resolveRunItTwice();
	}
}

void TableClientData2004::_resolveRunItTwice()
{
	if( !_nBoard[ 1 ] )
		return;

	for( int i = 0 ; i < _nBoard[ 1 ] ; i++ )
	{
		if( _board[ 1 ][ i ].eq( _board[ 2 ][ i ] ) )
		{
			_nBoard[ 0 ] = i + 1;
			_board[ 0 ][ i ] = _board[ 1 ][ i ];
			_board[ 1 ][ i ].hide(); _board[ 2 ][ i ].hide();
		}
		else
		{
			_board[ 0 ][ i ].hide();
		}
	}
}

void TableClientData2004::_loadPlayerChips( int player )
{
	const int playerPath[] = { 5, player, 0 };
	const CommMsgBody& msg = leaf( playerPath, 3 );
	TableClientDataParser parser( msg, *this );

	//UINT32 chips, chipsBet;
	BYTE sittingIn, connected;
	BYTE inHand, isAllIn; 
	parser.
		parseAmount( players[ player ].chips ).
		parseAmount( players[ player ].chipsBet ).
		parseBYTE( sittingIn ).
		parseBYTE( connected ).
		parseBYTE( isAllIn ).
		parseBYTE( inHand ).
		parseBYTE( players[ player ].indicator ).
		parseINT8( players[ player ].connQuality ).
		parseUINT32( players[ player ].timeBank ).
		parseINT16( players[ player ].nAllIns );
	parser.
		parseAmount( players[ player ].capN ).
		parseUINT32( players[ player ].totalBet );

	//players[ player ].chips = chips;
	//players[ player ].chipsBet = chipsBet;

	players[ player ].sittingOut = sittingIn ? 0 : 1;
	players[ player ].connected = ( connected != 0 );
	players[ player ].inHand = ( inHand != 0 );
	players[ player ].isAllIn = ( ( isAllIn & PlayerAllIn ) != 0 );
	players[ player ].isCapped = ( ( isAllIn & PlayerCapped ) != 0 );
}

void TableClientData2004::_loadPlayerUser( int player )
{
	UINT32 vipStatusLevel;

	const int playerPath[] = { 5, player, 1 };
	const CommMsgBody& msg = leaf( playerPath, 3 );
	CommMsgParser parser( msg );
	parser.
		parseStringP( players[ player ].user ).
		parseStringP( players[ player ].city ).
		parseUINT32( players[ player ].cartoonName ).
		parseUINT32( players[ player ].siteId ).
		parseUINT32( vipStatusLevel ).
		parseBYTE( players[ player ].publPropFlag ).
		parseUINT32( players[ player ].bounty ).
		parseStringP( players[ player ].promoImageName ).
		parseStringP( players[ player ].country ).
		parseBYTE( players[ player ].showCountry ).
		parseUINT32( players[ player ].headBounty ).
		parseUINT32( players[ player ].hallOfFame );

		parser.parseUINT32( players[ player ].mileStoneAward );
//	kludge: make preliminary award in cents to agree with final
//		if( !( players[ player ].mileStoneAward & 0x80000000 ) )
//			players[ player ].mileStoneAward *= 100;
		PTRACE4( "player=%d mileStoneAward=%d", player, players[ player ].mileStoneAward );

	players[ player ].vipStatus = ( VipStatusLevel )vipStatusLevel;

	if( !parser.parseEnded() )
		parser.parseUINT32( players[ player ].clientDevice );
	else
		players[ player ].clientDevice = 0;
}

void TableClientData2004::_loadPlayerCards( int player )
{
	const int playerPath[] = {5, player, 3};
	const CommMsgBody& msg = leaf(playerPath, 3);
	CommMsgParser parser(msg);

	TableClient::publication::TableSubscr_PlayerCards_Dict cards;
	cards.parseMsg(parser);

	players[player].nCards = cards.playerCards.size();
	for(int i = 0; i < players[player].nCards; ++i)
	{
		players[player].cardsShown[i] = cards.playerCards[i].showFlag;
		players[player].cards[i].suit = cards.playerCards[i].cardOrIndex.suit;
		players[player].cards[i].rank = cards.playerCards[i].cardOrIndex.rank;
		players[player].visibilityMasks[i] = cards.playerCards[i].visibilityMask;

		if( !cards.playerCards[i].powers.empty() )
		{
			players[player].powers[i].resize(cards.playerCards[i].powers.size());
		}

		for( INT32 j = 0; j < cards.playerCards[i].powers.size(); ++j )
		{
			players[player].powers[i][j].powerId = cards.playerCards[i].powers[j].powerId;
			players[player].powers[i][j].powerPlayer = cards.playerCards[i].powers[j].powerPlayer;
		}
	}
}

void TableClientData2004::Player::clear()
{
	user.cut( 0 );
	city.cut( 0 );
	country.cut( 0 );
	showCountry = 0;
	promoImageName.cut( 0 );
	clientDevice = 0;

	nAllIns = indicator = sittingOut = -1;
	cartoonName = 0;
	hallOfFame = 0;
	vipStatus = eVipUndefined;
	siteId = -1;
	chips = chipsBet = capN = nCards = connQuality = 0;
	connected = inHand = sittingOut = isAllIn = isCapped = false;
	bounty = 0;
	headBounty = 0;
	mileStoneAward = 0;
	totalBet = 0;
	timeBank = 0;
	publPropFlag = 0;

	memset( cardsShown, 0, sizeof( cardsShown ) );
	memset( cards, 0, sizeof( cards ) );
}

void TableClientData2004::clear()
{
	for( int i = 0; i < MaxPlayers; ++i )
		players[ i ].clear();

	attachments.clear();
	updateFlags = 0;
	nChatAdded = nChatDeleted = 0;
	memset( playerUpdateFlags, 0, sizeof( playerUpdateFlags ) );

	_dealer = _activePlayer = _rake = 0;
	memset( _pot, 0, sizeof( _pot ) );
	_nPots = _tempPot = 0; 

	_nColumns = 0;
	memset( _column, 0, sizeof( _column ) );
	memset( _nBoard, 0, sizeof( _nBoard ) );
	memset( _board, 0, sizeof( _board ) );
}

INT64 TableClientData2004::bigBlind() const
{
	return ::bigBlind( potStruct(), loBet(), hiBet() );
}
