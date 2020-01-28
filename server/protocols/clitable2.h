#ifndef clitable_h_included
#define clitable_h_included

#include "ppinclude.h"
#include "ppimage.h"
#include "commcli.h"
#include "handhistory.h"
#include "i18n.strmap.h"
#include "tabledef.h"
#include "clifppfactor.h"

enum
{
	_chatChanged			= 0x01,
	_tableChanged			= 0x02,
	_gameTypeChanged		= 0x04,
	_dealOptionChanged		= 0x08,
	_tableFppFactorChanged	= 0x10,
	_tableDynamicChanged	= 0x20,
};

struct PowerOnCard
{
	// M(41) - M(powerId,powerPlayer)
	INT32 powerId;	//alias pI
	BYTE powerPlayer;
};

class TableClientData2004 : public CommClientSubscriber
{
public:
	enum {	MaxPlayers = MAX_TABLE_PLAYERS,
		MaxBoard = MAX_TABLE_BOARD_CARDS,
		MaxPlayerCards = MAX_TABLE_PLAYER_CARDS };

	void synchronized() override;

	struct Player
	{
		PString user;
		PString city;
		PString country;
		BYTE showCountry;
		UINT32 cartoonName;
		UINT32 timeBank;
		UINT32 siteId;
		UINT32 hallOfFame; //PYR-23062
		VipStatusLevel vipStatus;
		BYTE indicator;
		BYTE publPropFlag;
		INT16 nAllIns;
		INT8 connQuality; // in percentage 0-100%
		UINT32 bounty;
		UINT32 headBounty; //PYR-25199
		UINT32 totalBet; //PYR-16708
		UINT32 mileStoneAward; //PYR-25930
		PString promoImageName;
		UINT32 clientDevice; // PYR-27126

		INT64 chips;
		INT64 chipsBet;
		INT64 capN;		// Bug #14309
		int sittingOut; // 1 - sitOut, 0 - sitIn
		bool connected;
		bool inHand;    // Have to be added to interface protocol!!!
		bool isAllIn;   // Is All-In as no money or by default (timeout), both 
		bool isCapped;	// Bug #14309 (for circle publication)

		int nCards;
		BYTE cardsShown[ MaxPlayerCards ];
		Card cards[ MaxPlayerCards ];
		INT16 visibilityMasks[ MaxPlayerCards ];
		vector<PowerOnCard> powers[ MaxPlayerCards ];

		bool isTeamPro() const { return ( publPropFlag & ClientPublPropTeamPro ) == ClientPublPropTeamPro; }
		bool hasNoChatTooltip() const { return ( publPropFlag & ClientPublPropShowNoChatToolTip ) == ClientPublPropShowNoChatToolTip; }

		void clear();
	};

private:
	struct _Attachment
	{
		UINT32 id;
		CommMsgBody body;
	};

private:
	//{ transaction state
	PNewPtrVector< _Attachment > attachments;
	UINT32 updateFlags;//0x01 - chatChanged, 0x02 - tableChanged, 0x04 - gameTypeChanged, 0x08 - dealOptionChanged, 0x10 - tableFppFactorChanged, 0x20 - tableDynamicChanged
	int nChatAdded, nChatDeleted;
	UINT32 playerUpdateFlags[ MaxPlayers ];//0x01 - changed
	//} transaction state

	PString _protocolVer, _tableName, _nameSuffix/*, _advertiseMsg*/, _multiGameDescr, _currency, _itSessionId;
	I18nPString _advertiseMsgI18n;
	UINT16 _chatLang;
	UINT64 _handId64, _handIdLast64, _milestoneHandId;
	UINT32 _tableId, _activeTimeBank, _tableBits, _clubId, _accessMask, _tableServFlags;
	INT32 _scalePM, _nSubscribers;
	bool _showCity;
	BYTE _handType, _potStruct, _tournamentType, _maxTablePlayers, _activeTBstate, _gameType, _dealOptions;
	int _loBet, _hiBet, _ante, _round, _multiGameHands, _loBetLM, _hiBetLM, _multiGameMultiplier;
	bool _isHiLo, _isPlayMoney, _isOneToOne;
	ClientMsgStrMap _msgStrId;
	PString _superstarShowdownResults;	// #18541

	// PYR-26697 (chapter 6)
	BYTE _actionTimeType;
	UINT32 _ActionTimeTotal, _actionTimeRemaining;

	PString _createdBy;
	PString _cancelledBy;

	// VOIP (PYR-18513)
	PString _voiceObjectCommandAddress;
	PString _voiceObjectDataAddress;
	PString _voiceObjectName;

	int	_dealer, _activePlayer, _rake;
	int _pot[ MaxPlayers ];
	int _nPots, _tempPot; 

	int _nColumns;						// Number of money's columns
	INT64 _column[ MAX_TABLE_COLUMNS ];  // Money's columns in front of player

	int _minBuyIn, _maxBuyIn, _cap, _capX, _capY, _banner;
	int _nBoard[ 3 ];
	Card _board[ 3 ][ MaxBoard ];

	Player players[ MaxPlayers ];

	vector< LobbyListSubscriber_FppFactor > tableFppFactor;

public:
	TableClientData2004();
	TableClientData2004( const TableClientData2004& other );
	TableClientData2004& operator=(const TableClientData2004&) = delete;

public:
	size_t chatSize() const;
	const CommMsgBody& eventAt( int i ) const;
	int loadEvent( int i, TableHistoryItem& event, CommMsgBody& extra ) const;
	int parseEvent( const CommMsgBody& commMsgBody, TableHistoryItem& event, CommMsgBody& extra ) const;
	virtual void chatChanged( int nAdded, int nDeleted ) {}//( -1, -1) means 'changed, re-read'
	virtual void dealOptionChanged() {}

	const char* protocolVerString() const { return _protocolVer; }
	const char* tableNameString() const { return _tableName; }
	const char* nameSuffixString() const { return _nameSuffix; }
	const ClientMsgStrMap& advertisementMessage() const { return _msgStrId; }
	//const char* advertisementString() const { return _advertiseMsg; }
	const char* multiGameDescrString() const { return _multiGameDescr; }
	const char* currency() const { return _currency; }
	const char* itSessionId() const { return _itSessionId; }
	const I18nPString& advertisementI18NString() const { return _advertiseMsgI18n; }
	const char* superstarShowdownResults() const { return _superstarShowdownResults; }	// #18541

	INT64 cap() const { return scaleAmount( _cap ); }
	UINT32 cap_unscaled() const { return _cap; }
	INT64 capX() const { return scaleAmount( _capX ); }
	INT64 capY() const { return scaleAmount( _capY ); }
	INT64 minBuyIn() const { return scaleAmount( _minBuyIn ); }
	INT64 maxBuyIn() const { return scaleAmount( _maxBuyIn ); }
	INT64 loBet() const { return scaleAmount( _loBet ); }
	INT64 hiBet() const { return scaleAmount( _hiBet ); }
	INT64 loBetLimit() const { return scaleAmount( _loBetLM ); }
	INT64 hiBetLimit() const { return scaleAmount( _hiBetLM ); }
	INT64 ante() const { return scaleAmount( _ante ); }
	int multiGameHands() const { return _multiGameHands; }
	int multiGameMultiplier() const { return _multiGameMultiplier; }
	RoundOrdinal round() const { return ( RoundOrdinal )_round; }
	eShowDealOption dealOptions() const { return ( eShowDealOption )_dealOptions; }
	int starterBanner() const { return _banner; }
	INT64 bigBlind() const;

	UINT16 chatLang() const { return _chatLang; }
	UINT32 tableId() const { return _tableId; }
	UINT32 clubId() const { return _clubId; }
	UINT32 accessMask() const { return _accessMask; }
	UINT32 tableServFlags() const { return _tableServFlags; }
	INT32 nSubscribers() const { return _nSubscribers; }

	UINT32 scalePM() const { return _scalePM; }
	INT64 scaleAmount( UINT32 amount ) const
	{
		if( _scalePM > 1 )
			return amount * INT64( _scalePM );
		else
			return INT64( amount );
	}
	UINT32 descaleAmount( INT64 amount ) const
	{
		if( _scalePM > 1 )
		{
			PASSERT4( amount % _scalePM == 0 );
			amount /= _scalePM;
		}
		PASSERT4( 0 <= amount && amount <= PUINT32_MAX );
		return static_cast< UINT32 >( amount );
	}

	bool showCity() const { return _showCity; }
	UINT32 tableBits() const { return _tableBits; }
	UINT64 handId64() const { return _handId64; }
	UINT64 handIdLast64() const { return _handIdLast64; }
	UINT64 milestoneHandId() const { return _milestoneHandId; }
	UINT32 timeBankValue() const { return _activeTimeBank; }
	TimeBankState timeBankState() const { return ( TimeBankState )_activeTBstate; }

	BYTE actionTimeType() const { return _actionTimeType; }
	UINT32 actionTimeTotal() const { return _ActionTimeTotal; }
	UINT32 actionTimeRemaining() const { return _actionTimeRemaining; }

	HandType handType() const { return ( HandType )_handType; }
	HandType multiGameType() const { return ( HandType )_gameType; }
	HandType gameType() const { return _gameType ? multiGameType() : handType(); }

	const char* createdBy() { return _createdBy.length() ? _createdBy.c_str() : NULL; }
	const char* cancelledBy() { return _cancelledBy.length() ? _cancelledBy.c_str() : NULL; }

	PotStruct potStruct() const { return ( PotStruct )_potStruct; }
	Tournament tournamentType() const { return ( Tournament )_tournamentType; }

	bool isHiLo() const { return _isHiLo; }
	bool isPlayMoney() const { return _isPlayMoney; }
	bool isOneToOne() const { return _isOneToOne; }

	int dealer() const { return _dealer; }
	int maxTablePlayers() const { return _maxTablePlayers; }
	int activePlayer() const { return _activePlayer; }
	INT64 rake() const { return scaleAmount( _rake ); }
	int nBoard() const { return _nBoard[ 0 ]; }
	int nColumns() const { return _nColumns; }
	INT64 tempPot() const { return scaleAmount( _tempPot ); }
	const INT64* columns ( ) const { return _column; }
	const Card* board() const { return _board[ 0 ]; }
	int nBoard( size_t index ) const { return _nBoard[ index ]; }
	const Card* board( size_t index ) const { PASSERT4( index < MAX_TABLE_BOARDS ); return _board[ index ]; }
	bool runningItTwice() const { return _nBoard[ 1 ] > 0; }
	int nPots() const { return _nPots; }
	INT64 pot( int i ) const { return scaleAmount( _pot[ i ] ); }
	const vector< LobbyListSubscriber_FppFactor >& fppFactor() const { return tableFppFactor; }

	virtual void tableChanged() {}
	virtual void gameTypeChanged() {}
	virtual void tableDynamicChanged() {}
	virtual void tableFppFactorChanged() {}
	virtual void tableBitChanged( UINT32 oldValue, UINT32 newValue ) {}

	const Player& player( int i ) const
	{
		return players[ i ];
	}
	virtual void playerChanged( int i ) {}

private:
	void beginTrans() override;
	void commitTrans() override;
	void onlineUpdateAttachment( UINT32 id, CommMsgBody& attachment ) override;

public:
	bool isReplayTable() const { return ( _tableBits & RingGameReplayTable ) == RingGameReplayTable; }

public:
	bool isFastTable() const { return ( _tableBits & TABLESCRIPT_FLAG_FASTTABLE ) == TABLESCRIPT_FLAG_FASTTABLE; }
	bool isNoAllIn() const { return ( _tableBits & TABLESCRIPT_FLAG_NOALLIN ) == TABLESCRIPT_FLAG_NOALLIN; }
	bool isShowCardEnabled() const { return ( _tableBits & TABLESCRIPT_FLAG_SHOW_ONE_CARD ) == TABLESCRIPT_FLAG_SHOW_ONE_CARD; }

public:
	virtual void userBeginTrans() {}
	virtual void userCommitTrans() {}
	virtual void userOnlineUpdateAttachment( UINT32 id, CommMsgBody& attachment ) {}

	void leafAppended( const Path& path, const CommMsgBody& msg ) override;
	void leafRemoved( const Path& ) override;
	void leafUpdated( const Path& path, const CommMsgBody& msg ) override;
	void nodeChanged( const Path& path ) override;//also called with pathLen() == 0 for 'global update'

public:
	static void convertImage( PImage& img, const PBlock& block )
	{
		if( block.size() )
		{
			PGifImageSource src( block.ptr(), block.size() );
			img.load( src );
		}
		else
		{
			img.init( 0, 0 );
		}
	}

	const char* voiceObjectCommandAddress() const { return _voiceObjectCommandAddress.length() ? _voiceObjectCommandAddress.c_str() : NULL; }
	const char* voiceObjectDataAddress() const { return _voiceObjectDataAddress.length() ? _voiceObjectDataAddress.c_str() : NULL; }
	const char* voiceObjectName() const { return _voiceObjectName.length() ? _voiceObjectName.c_str() : NULL; }

public:
	void clear();

private:
	void _loadStatic();
	void _loadHand();
	void _loadActive();
	void _loadDynamic();
	void _loadBoardRit();

	void _loadPlayerChips( int player );
	void _loadPlayerUser( int player );
	void _loadPlayerCards( int player );

	void _resolveRunItTwice();
};

class TableClientDataParser : public CommMsgParser
{
	const TableClientData2004& tableClient;

public:
	TableClientDataParser( const CommMsgBody& msgBody, const TableClientData2004& tableClient_ )
		: CommMsgParser( msgBody ), tableClient( tableClient_ )
	{
	}

	TableClientDataParser& parseAmount( INT64& res )
	{
		UINT32 amt;
		parseUINT32( amt );
		res = ( amt == -1 ) ? -1 : tableClient.scaleAmount( amt ); // take care of -1 case!
		return *this;
	}
};

#endif
