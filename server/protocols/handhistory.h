//handhistory.h
#ifndef	handhistory_h_included
#define	handhistory_h_included

#include "ppinclude.h"
#include "ppstring.h"
#include "commmsgbody.h"
#include "pyrtime.h"
#include "dbm.h"
#include "BrandImages.h"
#include "brands.h"
#include "sitemasks.h"


struct UserInHand
{
	PString	name;
	UINT32  userIntId;		// PYR-30164
	BYTE	position;
	INT32	startAmount;	// not saved to DB after the hand, but returned by hand history
	INT32	delta;
	INT32	result;			//(collected, tied, folded, lost, etc.)
	PString	cards;			//(n cards that were dealt)
	BYTE	bestHand;		//(high card, pair, two pairs, three, etc...)
	PString	combHi;			//(5 cards that make the combination (not that were dealt!!!))
	PString	combLo;			//(5 cards that make the combination (not that were dealt!!!))
	INT32	potChips;		// #19697. Player's pot contribution.
	// The below members ARE NOT returned by hand history query (but are saved into DB)
	INT32	fpp;			// amount of fpp earned by user in hand
	INT32	fractionFpp;	// amount of 'fpp cents' earned by user
	BYTE	fppMethod; // not used anywhere. Remove?
	INT32	dbStartAmount;	// the "real" start amount - how much was available for betting during the play. That's what we should save to database, not the startAmount, which might be affected by rebuys etc. For detailed explanation see bug #923
	// PYR-41650 - we save this member to DB now
	UINT32	licenseId;

	// The following members only used in the client:
	UINT32	cartoonId;
	UINT32	siteId;
	PString	city;
	UINT32	bounty;
	PString country;
	BrandImages	brandImages;  // PYR-41693
	// End of client-only members.

	// PYR-26400 - sent at the end of the hand, but not saved to DB or returned by hand history
	PString playerIP;

	UINT32  clientDevice; // PYR-38802 - now used not only in the client, but in the DBM_Q_HAND message as well

	//PYR-25226
	INT32 unusedStacks;
	INT32 unusedStacksHandStart;

	// PYR-38782 - Storm-related user data saved to DB.
	INT32 startingMana;
	vector<UINT32> startingPowers;

	UINT64 buyInTransId; // PYR-38802 - buyIn transId for ring game and first buyIn transId for blitz (used for DK ring/blitz reporting)

	UINT64 appLoginId; // PYR-41940
	UINT32 clientBrand;  // PYR-41940

	// PYR-95110
	bool hasCashedOut = false;
	INT32 equityClientView = 0;   // x10000
	INT32 equityServerView = 0;   // x10000
	INT32 cashoutAmountOffered = 0;
	INT32 cashoutRakeCalculated = 0;
	INT32 houseWinningAmount = 0;

	// Please notify Game and Payment security team of changes and additions

	UserInHand() //just zero the fields
		: licenseId( 0 )
		, buyInTransId( 0 ) // PYR-38802
	{
		userIntId = 0;
		position = 0;
		cartoonId = 0;
		siteId = 0;
		startAmount = 0;
		delta = 0;
		result = 0;
		bestHand = 0;
		fpp = 0;
		fractionFpp = 0;
		fppMethod = 0;
		potChips = 0;
		dbStartAmount = 0;
		bounty = 0;
		clientDevice = 0;
		unusedStacks = 0;
		unusedStacksHandStart = 0;
		startingMana = 0;
		startingPowers.clear();
		appLoginId = 0; // PYR-41940
		clientBrand = BrandType_Unknown;  // PYR-41940
	}

	void compose(CommMsgBody& body) const;
	void parse(CommMsgParser& parser);
	// PYR-26400
	void composeByTable( CommMsgBody& body ) const;
	void parseFromTable( CommMsgParser& parser );
};

// PYR-16558, for the second board info
struct UserInHand2   
{
	PString	name;
	BYTE  bestHand2;	//(high card, pair, two pairs, three, etc...)
	PString	combHi2;	//(5 cards that make the combination (not that were dealt!!!))
	PString	combLo2;	//(5 cards that make the combination (not that were dealt!!!))

		// Please notify Game and Payment security team of changes and additions

	UserInHand2() //just zero the fields
	{
		bestHand2 = 0;
	}

	void compose(CommMsgBody& body) const;
	void parse(CommMsgParser& parser);
	// PYR-26400
	void composeByTable( CommMsgBody& body ) const;
	void parseFromTable( CommMsgParser& parser );
};

// PYR-65734, for unfold pot
struct HandExtraPotInfo
{
	INT32 potSize;
	INT32 rake;
	UINT32 potFlags;

	// Please notify Game and Payment security team of changes and additions

	HandExtraPotInfo()
	{
		clear();
	}

	void clear()
	{
		potSize = 0;
		rake = 0;
		potFlags = 0;
	}
	void compose(CommMsgBody& body) const;
	void parse(CommMsgParser& parser);
};

struct UserInHandExtraPot
{
	PString	userId;  // i.e. "name" in other structs
	INT32 potChips;  // chips contributed to this pot only  
	INT32 delta;     // winning or loss amount from this pot only

	// Please notify Game and Payment security team of changes and additions

	UserInHandExtraPot() //just zero the fields
	{
		potChips = 0;
		delta = 0;
	}

	void compose(CommMsgBody& body) const;
	void parse(CommMsgParser& parser);
};


// PYR-26400 - some extra data sent at the end of the hand. Usually needed for license-specific reporting.
// Not saved to DB.
struct UserInHandExtra
{
	PString arjelUserHash;

	// Please notify Game and Payment security team of changes and additions

	void composeByTable( CommMsgBody& body ) const;
	void parseFromTable( CommMsgParser& parser );
};

struct TableHistoryItem
{
	UINT64		tableId; // PYR-29549
	UINT64		handId64;
	UINT32		ordinal;
	SrvTime		when;
	UINT16		eventCode;
	INT8		position;
	INT8		action;
	UINT32		amount;
	PString		userId;
	PString		contents;

	// Please notify Game and Payment security team of changes and additions

	TableHistoryItem()
	{
		tableId = 0;
		handId64 = 0;
		ordinal = 0;
		eventCode = 0;
		position = 0;
		action = 0;
		amount = 0;
	}

	TableHistoryItem( const TableHistoryItem& other ) = default;
	TableHistoryItem& operator=( const TableHistoryItem& other ) = default;

	TableHistoryItem( TableHistoryItem&& other )
	{
		operator=( move( other ) );
	}

	TableHistoryItem& operator=( TableHistoryItem&& other )
	{
		if( this != &other )
		{
			tableId = other.tableId;
			handId64 = other.handId64;
			ordinal = other.ordinal;
			when = other.when;
			eventCode = other.eventCode;
			position = other.position;
			action = other.action;
			amount = other.amount;
			userId = move( other.userId );
			contents = move( other.contents );
		}

		return *this;
	}

	void compose(CommMsgBody& body) const;
	void parse(CommMsgParser& parser);

	//this function defines sort order 
	bool operator < ( const TableHistoryItem& right ) const
	{
		/// order of sorting:
		//1) tableId      
		//2) ordinal
		if(tableId != right.tableId)
		{
			return tableId < right.tableId;
		}
		return ordinal < right.ordinal;
	}

};

struct TableHistoryItemZip
{
	UINT64		tableId; // PYR-29549
	UINT64		handId64;
	UINT32		ordinal;
	PString		encoded;

	// Please notify Game and Payment security team of changes and additions

	TableHistoryItemZip()
	{
		tableId = 0;
		handId64 = 0;
		ordinal = 0;
	}
};

struct HistoryCollusionItem
{
	UINT16	collusionCode;
	vector<PString> colluders;
	explicit HistoryCollusionItem( UINT16 code )
	{
		collusionCode = code;
	}
};
struct TableHistoryCollusions
{
	UINT64 handId64;
	vector<HistoryCollusionItem> items;
	explicit TableHistoryCollusions( UINT32 hand_id)
	{
		handId64 = hand_id;
	}
	TableHistoryCollusions()
	{
		handId64 = 0;
	}
	void clear()
	{
		handId64 = 0;
		items.clear();
	}
};


typedef vector<UserInHand> UsersInHand;
typedef vector<UserInHand2> UsersInHand2;
typedef vector<UserInHandExtraPot> UsersInHandExtraPot;
typedef vector<UserInHandExtra> UsersInHandExtra; // PYR-26400
typedef vector<TableHistoryItem> TableHistory;
typedef vector<TableHistoryItemZip> TableHistoryZip;

struct Hand
{
	// !!!NB!!! Any changes to this structure must be communicated to the client team!!!
	// The client uses its own parsing!
	UINT64			handId64;
	UINT64			tableId; // PYR-26400 - 64-bit now
	// game and isHiLo are saved as one "GAME" field in DB
	BYTE			game;
	BYTE			isHiLo;
	INT8			dealer;
	SrvTime			started;
	SrvTime			finished;
	INT32			rakeSize;
	INT32			potSize;
	INT32			loBet;
	INT32			hiBet;
	INT32			smallBlindSize;
	INT32			ante;
	INT32			bringIn;
	BYTE			isOneOnOne;	// max players on the table
	BYTE			chipsType;	// saved as "ISPLAYMONEY" field in DB
	BYTE			structure;
	BYTE			tournType;
	UINT32			tournId;
	UINT16			tournLevel;
	BYTE			newGameFlag;	// saved as "ISNEWGAME" field in DB
	PString			board;
	PString			tableName;
	PString			currency;
	UINT32          handFlags;
	UsersInHand		users;
	PString			board2;		// PYR-16558
	UsersInHand2	users2;		// PYR-16558
	UINT32			scalePM;	// PYR-23052
	TableHistory	history;
	TableHistoryZip	historyZip;
	UINT32			cap;	//PYR-17335: kind of fiction, is not going to Hands DB table; HH continue to get Cap from TabTypeProps.
	UINT32			clubId;

	// PYR-26400
	// The below members are sent at the end of the hand, but not saved to DB or returned by hand history
	PSiteMask		siteMaskInHand; // PYR-100668
	UsersInHandExtra usersExtra;

	// The below members are not sent at the end of the hand - only returned by HH
	INT32			tournBuyIn;
	INT32			tournFppBuyIn;
	INT32			tournRake;
	INT8			tournIsPlayMoney;
	INT32			tournBounty;
	PString			aamsId;
	PString			clubName;
	UINT32          ordinalOfHandStart;
	INT64			buyIn64; // PYR-37127
	INT64			rake64; // PYR-37127

	// The following members only used in the client:
	BYTE			speedDisplay; //PYR-20162
	BYTE			awardSchema;  //PYR-20162
	// End of client-only members.

	UINT32 finalFlightTournId; // PYR-26094 
	INT32 tournPlaceDelta;

	// The following members are needed for Storm hands (PYR-34850)
	INT32 carryOverFromLastHand;
	mutable INT32 carryOverToNextHand; // AQA-1095 - we need to inject a value into this during formatHandHistory, hence the mutable.
	INT32 dealerSuppliedFreeChips;
	// !!!NB!!! Any changes to this structure must be communicated to the client team!!!
	// The client uses its own parsing!

	INT8 nextButtonPos;  // PYR-32587

	// PYR-65734 for unfold game
	HandExtraPotInfo handExtraPot;
	UsersInHandExtraPot usersExtraPot;

	UINT32 pppId; // PYR-123658 - progressive plus pool id

	// Please notify Game and Payment security team of changes and additions

	Hand()
	{
		clear(); 
		chipsType = 0;
	}


	void clear()
	{
		handId64 = 0;
		tableId = 0;
		game = 0; //HandTypeNone:: dbm.h
		isHiLo = 0;
		dealer = -1;
		rakeSize = 0;
		potSize = 0;
		loBet = 0;
		hiBet = 0;
		smallBlindSize = 0;
		ante = 0;
		bringIn = 0;
		isOneOnOne = 0;
		//chipsType = 0;  // AQA-28025 keep chipsType as it should not be changed between hands!
		structure = 0; //StructNonDefined:: lobby.h
		tournType = 0;
		tournLevel = 0;
		tournBuyIn = 0;
		tournFppBuyIn = 0;
		tournRake = 0;
		tournIsPlayMoney = -1;
		newGameFlag = 0;
		tournId = 0;
		tournBounty = 0;
		ordinalOfHandStart = 0;
		cap = 0;
		speedDisplay = 0;
		awardSchema = 0;
		scalePM = 1;
		handFlags = 0;
		finalFlightTournId = 0;
		tournPlaceDelta = 0;
		carryOverFromLastHand = 0;
		carryOverToNextHand = 0;
		dealerSuppliedFreeChips = 0;
		siteMaskInHand = PSiteMask::zero; // PYR-100668
		clubId = 0;
		buyIn64 = 0;
		rake64 = 0;

		board = "";
		tableName = "";
		currency = "";
		aamsId = "";
		clubName = "";
		users.clear();
		history.clear();
		historyZip.clear();
		board2 = "";
		users2.clear();
		usersExtra.clear();

		started.setNull();
		finished.setNull();

		nextButtonPos = -1;

		handExtraPot.clear();
		usersExtraPot.clear();

		pppId = 0;
	}

	// PYR-25238 - for tournaments the hand is in chips and is not scaled but we need to use the actual scale for tourn buy-in
	// so effective scalePM for tournament is always 1 and for table - whatever is stored in scalePM member i.e. 'stored scalePM'
	// 
	UINT32 getEffectiveScalePM() const { return ( tournType == 0 /*NON_TOURNAMENT - in lobby.h*/ ) ? scalePM : 1; }
	UINT32 getStoredScalePM() const { return scalePM; }
	void setScalePM( UINT32 val ) { scalePM = val; }

	bool isRunItTwiceHand() const { return ( handFlags & hFlagRunItTwice ) != 0; }
	bool isNStackTournHand() const { return ( handFlags & hFlagNStackTournHand ) != 0; }
	bool isSagTournHand() const { return ( handFlags & hFlagSagTournHand ) != 0; }
	bool isBlitzModeTournHand() const { return ( handFlags & hFlagBlitzModeTourn ) != 0; }
	bool isZoomTournHand() const { return ( handFlags & hFlagZoomTourn ) != 0; }  // for both blitz mode and normal mode
	bool isProgessiveKnockoutTournHand() const { return ( handFlags & hFlagProgressiveKnockoutTourn ) != 0; }
	bool isTournHand() const { return chipsType == TOURN_CHIPS && tournId; }		//	PYR-32627
	bool isManaCost() const { return ( handFlags & hFlagStormMana ) != 0; } // PYR-36799
	bool isStormHand() const { return ( handFlags & hFlagStorm ) != 0; } // PYR-38782
	bool isButtonBlindHand() const { return ( handFlags & hFlagButtonBlind ) != 0; }  // PYR-74693
	bool isGiantBlindHand() const { return ( handFlags & hFlagGiantBlind ) != 0; }  // PYR-82834
	bool isStealthTableHand() const { return ( handFlags & hFlagStealthTable ) != 0; }  // PYR-106120

	void compose(CommMsgBody& body) const;
	void parse( CommMsgParser& parser );

	// table code does the composing for this parseFromTable()
	void parseFromTable( CommMsgParser& parser ); // PYR-26400
};

// [AO 2010-11-16] Extracted from DBM
void parseHandHistory(CommMsgParser& parser, UINT64 tableId, TableHistory& history);
void parseHandHistoryZip(CommMsgParser& parser, UINT64 tableId, TableHistoryZip& zippedItems);

// PYR-23115: moved from handhistorydbmprepstmt.h/cpp to remove dependency on DB2
void extractHistoryZip(const char* bodyStr, UINT64 handId, UINT64 tableId, TableHistory& table); // [AO 2010-11-27] PYR-16750

void getMaxOrdinal(const TableHistory& history, const TableHistoryZip& historyZip, UINT32& maxOrdinal, UINT64& tableId);

#endif	//handhistory_h_included
