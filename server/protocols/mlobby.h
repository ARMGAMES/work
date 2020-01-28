//mlobby.h - mobile lobby support

#ifndef mlobby_h_included
#define mlobby_h_included

#include "commmsgbody.h"
#include "dbm.h"
#include "lobby.h"
#include "sites.h"

// selection filter for both ring games and tournaments
struct SelectionFilter
{
	// client locale
	UINT32 locale;

	// client country
	PString country;

	// player's siteId
	UINT32 siteId; // see Sites in sites.h

	BYTE game; // see HandType in dbm.h
	BYTE hiLo; // hiLo flag where applicable
	BYTE limit; // see PotStruct in lobby.h

	// game stake range, 0 if not used
	UINT32 minStake, maxStake;

	// buy-in range (0=unused). 
	// For ring games, means the number of big blinds
	// For tournaments, means cash
	UINT32 minBuyIn, maxBuyIn;

	// tournament buy-in options
	enum BuyInMask
	{
		Cash = 1<<1,
		FPP = 1<<2,
		PlayMoney = 1<<3,

		// Don't forget to keep BuyInMask_Last updated
		BuyInMask_Last = PlayMoney,
	};
	UINT32 buyInMask; // mask
	
	// maximum players at the table
	enum MaxPlayers
	{
		// ring games 
		OneOnOne = 1<<2,
		FiveMax = 1<<5,
		SixMax = 1<<6,
		ShortRing = 1<<3 | 1<<4 | 1<<5 | 1<<6,
		FullRing = 1<<9 | 1<<10,

		// tournaments
		FourMax = 1<<4,
		SevenToNine = 1<<7 | 1<<8 | 1<<9,
		TenMax = 1<<10,

		// !!! keep MaxPlayers_Mask_Max updated
		MaxPlayers_Mask_Max = OneOnOne | FullRing | SevenToNine | ShortRing,
	};
	UINT32 maxPlayers; // mask

	// minimum players seated, 0=unused
	BYTE minSeated;

	// minimum available seats, 0=unused
	BYTE minAvailable;

	// table speed
	enum TableSpeed
	{
		NormalSpeed = 1<<0,
		FastSpeed	= 1<<1,

		// !!! keep TableSpeed_Last updated
		TableSpeed_Last = FastSpeed,
	};
	enum TourneySpeed
	{
		RegularSpeed= 1<<0,
		TurboSpeed	= 1<<1,
		HyperSpeed	= 1<<2,
		SlowSpeed	= 1<<3,

		// !!! keep TableSpeed_Last updated
		TourneySpeed_Last = SlowSpeed,
	};
	BYTE speed; // mask

	// true for real money games
	bool realMoney;

	// currencies used for Real Money
	enum Currency
	{
		USD = 1<<0, // US dollars
		EUR = 1<<1, // Euro Currency
		GBP = 1<<2, // British Pounds
		CAD = 1<<3, // Canadian Dollars
		INR = 1<<4, // Indian Rupees

		// !!! keep Currency_Last updated
		Currency_Last = INR,
	};
	UINT32 currencies; // currency mask, 0=any
	bool checkCurrency( const char* currency ) const; // PYR-30558 - refactored currency check

	// Tourn Format/Variant - indicates clients screen, ideally should be mutually exclusive
	enum Format
	{
		  Regular		= 1<<0	//default
		, Rebuy			= 1<<1	//not used yet
		, Knockout		= 1<<2
		, Shootout		= 1<<3	//not used yet
		, DoubleOrNothing	= 1<<4	//not used yet
		, Fifty50		= 1<<5	//not used yet
		, NotSatellite	= 1<<6	//modifier for Regular and SitNGo
		, Steps			= 1<<7	//not used yet
		, SitNGo		= 1<<8
		, BeatTheClock	= 1<<9

		// !!! keep Format_Last updated
		, Format_Last = BeatTheClock
	};
	UINT32 format; // mask

	// starting soon in minutes, or 0 if not used
	UINT32 startingSoon;

	// Tournament state
	enum State
	{
		Announced	= 1<<0,
		Registering	= 1<<1,
		Running		= 1<<2,
		Completed	= 1<<3,
		Replays		= 1<<4,

		// !!! keep State_Last updated
		State_Last = Replays,
	};
	UINT32 state; // mask

	// tables order
	enum TableOrder
	{
		TableOrder_Default = 0,
		AvailSeats=1, 
		StakesAsc=2,
		StakesDesc=3,
		AveragePot=4,
		HandsPerHour=5,
		PlayersSeatedAsc=6,
		PlayersSeatedDesc=7,

		// !!! keep TableOrder_Last updated
		TableOrder_Last = PlayersSeatedDesc
	};

	// tournaments order
	enum TourneyOrder
	{
		StartDate=1,
		BuyInAsc=2,
		BuyInDesc=3,
		Enrolled=4,
	};
	BYTE order; // sorting order

	// tournament size for Sit&Go
	UINT32 minEntrants, maxEntrants;

	// ZOOM Tournaments
	enum ZoomTournaments
	{
		zoom_Exclude	= 1<<0,
		zoom_Include	= 1<<1,
		zoom_Only		= 1<<2,

		// !!! keep ZoomTournaments_Last updated
		ZoomTournaments_Last = zoom_Only,
	};
	UINT32 zoomTournaments; // mask

	// playmoney multiplier for high stakes tables and tournaments
	UINT32 multiplierPM;

	// features supported in the client
	enum ClientFeaturesMobile
	{
		feature_None = 0,
		feature_ScaledTablesTournaments = 1<<0,		// display high-stake play money tournaments
		feature_TournamentReEntry = 1<<1,		// display re-entry tournaments
		//feature_NStackTournament = 1<<2,        // display nStack tournaments

		// !!! ClientFeaturesMobile_Last must be updated
		ClientFeaturesMobile_Last = feature_TournamentReEntry,
	};
	UINT32 features;

	PString nameCriteria;		// search tournaments by name

	// mlobby special search modes
	enum SpecialSearchMode
	{
		specialSearchMode_None = 0,			// normal search
		specialSearchMode_tournByNameOnly = 1,		// search tournaments by name ignoring other criteria

		// !!! ClientFeaturesMobile_Last must be updated
		SpecialSearchMode_Last = specialSearchMode_tournByNameOnly,
	};
	UINT32 specialSearchMode;
	UINT32 clientSortingSeed; //PYR-32929
	UINT32 brandMask; // PYR-40768
	bool includePsng; //PYR-39272

	enum SeatFindersAndBlitzes
	{
		SeatFindersAndBlitzes_Both = 0,
		SeatFindersAndBlitzes_SeatFindersOnly = 1,
		SeatFindersAndBlitzes_BlitzesOnly = 2,

		// !!! SeatFindersAndBlitzes_Last must be updated
		SeatFindersAndBlitzes_Last = SeatFindersAndBlitzes_BlitzesOnly,
	};
	INT8 seatFindersAndBlitzes; // PYR-43098

	// default constructor
	SelectionFilter()
	{
		locale = LOCALE_DEFAULT;
		siteId = PokerStars_SiteUnknown; 
		game = HandTypeHoldem; 
		hiLo = false; // hiLo flag //-V601
		limit = StructNoLimit; 
		minStake = 0; // not used
		maxStake = 0; // not used
		buyInMask = PlayMoney; 
		minBuyIn = 0; // not used
		maxBuyIn = 0; // not used
		maxPlayers = 0; // not used
		minSeated = 0; // not used
		minAvailable = 1; // hide full tables
		speed = 0; // not used
		currencies = 0; // not used
		format = 0; // not used
		startingSoon = 0; // not used
		state = 0; // not used
		order = 0; // not used
		realMoney = false; // play money
		minEntrants = 0; // not used
		maxEntrants = 0; // not used
		zoomTournaments = zoom_Exclude;
		multiplierPM = 1;
		features = feature_None;
		specialSearchMode = specialSearchMode_None; // mlobby special search mode
		clientSortingSeed = 0; //PYR-32929, 0 means not used
		brandMask = BrandType_PokerStars; // PYR-40768 default to PokerStars
		includePsng = false; //PYR-39272 old clients don't see PSNGs
		seatFindersAndBlitzes = SeatFindersAndBlitzes_Both;
	}

#if defined( PROTOCOL_CLIENT_ONLY )
	void compose(CommMsgBody& body) const
	{
		body
        	.composeBYTE(MLOBBY_CLIENT_PROTOCOL_VERSION)
			.composeUINT32(locale)
			.composeString(country)
			.composeUINT32(siteId)
			.composeBYTE(game)
			.composeBYTE(hiLo)
			.composeBYTE(limit)
			.composeUINT32(minStake)
			.composeUINT32(maxStake)
			.composeUINT32(buyInMask)
			.composeUINT32(minBuyIn)
			.composeUINT32(maxBuyIn)
			.composeUINT32(maxPlayers)
			.composeBYTE(minSeated)
			.composeBYTE(minAvailable)
			.composeBYTE(speed)
			.composeUINT32(currencies)
			.composeUINT32(format)
			.composeUINT32(startingSoon)
			.composeUINT32(state)
			.composeBYTE(order)
			.composeBOOL(realMoney)
			.composeUINT32(minEntrants)
			.composeUINT32(maxEntrants)
			.composeUINT32(zoomTournaments)
			.composeUINT32(multiplierPM)
			.composeUINT32(features)
			.composeBOOL(false /*tableStarters*/)
			.composeString(nameCriteria)
			.composeUINT32(specialSearchMode)
			.composeUINT32(clientSortingSeed) //PYR-32929
			.composeUINT32(brandMask) // PYR-40768
			.composeINT8(seatFindersAndBlitzes)
			;
	}
#endif

	void parse(CommMsgParser& parser)
	{
		BYTE version;
		parser.parseBYTE(version)
			.parseUINT32(locale)
			.parseStringP(country)
			.parseUINT32(siteId)
			.parseBYTE(game)
			.parseBYTE(hiLo)
			.parseBYTE(limit)
			.parseUINT32(minStake)
			.parseUINT32(maxStake)
			.parseUINT32(buyInMask)
			.parseUINT32(minBuyIn)
			.parseUINT32(maxBuyIn)
			.parseUINT32(maxPlayers)
			.parseBYTE(minSeated)
			.parseBYTE(minAvailable)
			.parseBYTE(speed)
			.parseUINT32(currencies)
			.parseUINT32(format)
			.parseUINT32(startingSoon)
			.parseUINT32(state)
			.parseBYTE(order)
			;

		if( version>=2 )
			parser.parseBOOL(realMoney);
		if( version >=3 )
		{
			parser
				.parseUINT32(minEntrants)
				.parseUINT32(maxEntrants)
				;
		}
		if( version>=4 )
		{
			parser
				.parseUINT32(zoomTournaments)
				.parseUINT32(multiplierPM)
				.parseUINT32(features)
				;
		}
		if( version>=5 )
		{
			bool dummyTableStarters;
			parser.parseBOOL(dummyTableStarters);
		}
		if( version>=6 )
		{
			parser
				.parseStringP(nameCriteria)
				.parseUINT32(specialSearchMode)
				;
		}
		//PYR-32929
		if( version>=7 )
		{
			parser
				.parseUINT32(clientSortingSeed)
				;
		}

		// PYR-40768
		if( version>=8 )
		{
			parser
				.parseUINT32(brandMask)
				;
		}

		if( version>=9 )
		{
			includePsng = true;
		}

		if( version>=10 )
		{
			parser
				.parseINT8(seatFindersAndBlitzes)
				;
			if( seatFindersAndBlitzes < SeatFindersAndBlitzes_Both ||
				seatFindersAndBlitzes > SeatFindersAndBlitzes_BlitzesOnly)
			{
				seatFindersAndBlitzes = SeatFindersAndBlitzes_Both;
			}
		}

		//corrections
		if( !format )	//PYR-39944, remove when old clients are dead
		{
			format = Regular;
		}
	}

	void dump() const
	{
		if( locale != 0 ) 
			PLog("Locale=%d", locale);
		if( country.length()>0 ) 
			PLog("Country=%s", country.c_str());
		if( siteId != 0 )
			PLog("SiteId=%d", siteId);
		if( game != 0 ) 
			PLog("Game=%d", game);
		if( hiLo != 0 ) 
			PLog("HiLo=%d", hiLo);
		if( limit != 0 ) 
			PLog("Limit=%d", limit);
		if( minStake != 0 ) 
			PLog("MinStake=%d", minStake);
		if( maxStake != 0 ) 
			PLog("MaxStake=%d", maxStake);
		if( buyInMask != 0 ) 
			PLog("buyInMask=%d", buyInMask);
		if( minBuyIn != 0 ) 
			PLog("minBuyIn=%d", minBuyIn);
		if( maxBuyIn != 0 ) 
			PLog("maxBuyIn=%d", maxBuyIn);
		if( maxPlayers != 0 ) 
			PLog("maxPlayers=%d", maxPlayers);
		if( minSeated != 0 ) 
			PLog("minSeated=%d", minSeated);
		if( minAvailable != 0 ) 
			PLog("minAvailable=%d", minAvailable);
		if( speed != 0 ) 
			PLog("speed=%d", speed);
		if( realMoney )
			PLog("realMoney=true");
		if( currencies != 0 ) 
			PLog("currencies=%d", currencies);
		if( format != 0 ) 
			PLog("format=%d", format);
		if( startingSoon != 0 ) 
			PLog("startingSoon=%d", startingSoon);
		if( state != 0 )
			PLog("state=%d", state);
		if( zoomTournaments != 0 )
			PLog("zoomTournaments=%d", zoomTournaments);
		if( multiplierPM != 1 )
			PLog("multiplierPM=%d", multiplierPM);
		if( order != 0 )
			PLog("order=%d", order);
		if( features != feature_None )
			PLog("features=%d", features);
		if( nameCriteria.length() > 0)
			PLog("nameCriteria=%s", nameCriteria.c_str());
		if( specialSearchMode != 0)
			PLog("specialSearchMode=%d", specialSearchMode);
		if( clientSortingSeed != 0) //PYR-32929
			PLog("clientSortingSeed=%d", clientSortingSeed);
		PLog("brandMask=%u", brandMask);
		if( includePsng ) //PYR-39272
			PLog("includePsng=1");
		if( seatFindersAndBlitzes > 0 )
			PLog("seatFindersAndBlitzes=%u", seatFindersAndBlitzes);
	}
};


enum class MobileGameTypes : UINT64
{
	MobileGame_None					= 0ull,
	MobileGame_NLHoldem				= 1ull << 0,
	MobileGame_PLOmaha				= 1ull << 1,
	MobileGame_EightGame			= 1ull << 2,
	MobileGame_HORSE				= 1ull << 3,
	MobileGame_HOSE					= 1ull << 4,
	MobileGame_NLHoldem_PLOmaha		= 1ull << 5,
	MobileGame_PLHoldem_PLOmaha		= 1ull << 6,
	MobileGame_MixedHoldem			= 1ull << 7,
	MobileGame_MixedOmahaHL			= 1ull << 8,
	MobileGame_MixedOmaha			= 1ull << 9,
	MobileGame_TripleStud			= 1ull << 10,
	MobileGame_LimitHoldem			= 1ull << 11,
	MobileGame_PLHoldem				= 1ull << 12,
	MobileGame_NLOmaha				= 1ull << 13,
	MobileGame_LimitOmaha			= 1ull << 14,
	MobileGame_NLOmahaHL			= 1ull << 15,
	MobileGame_PLOmahaHL			= 1ull << 16,
	MobileGame_LimitOmahaHL			= 1ull << 17,
	MobileGame_PL5Omaha				= 1ull << 18,
	MobileGame_NL5Omaha				= 1ull << 19,
	MobileGame_NL5OmahaHL			= 1ull << 20,
	MobileGame_PL5OmahaHL			= 1ull << 21,
	MobileGame_Limit5OmahaHL		= 1ull << 22,
	MobileGame_NLCourchevel			= 1ull << 23,
	MobileGame_PLCourchevel			= 1ull << 24,
	MobileGame_NLCourchevelHL		= 1ull << 25,
	MobileGame_PLCourchevelHL		= 1ull << 26,
	MobileGame_LimitCourchevelHL	= 1ull << 27,
	MobileGame_7Stud				= 1ull << 28,
	MobileGame_7StudHL				= 1ull << 29,
	MobileGame_Razz					= 1ull << 30,
	MobileGame_NLSixPlusHoldem		= 1ull << 31,
	MobileGame_NLFusion				= 1ull << 32,
	MobileGame_PLFusion				= 1ull << 33,
	MobileGame_NLShowtimeHoldem		= 1ull << 34,
	MobileGame_PLShowtimeHoldem		= 1ull << 35,
	MobileGame_NLShowtimeOmaha		= 1ull << 36,
	MobileGame_PLShowtimeOmaha		= 1ull << 37,
	MobileGame_NLSwapHoldem			= 1ull << 38,
	MobileGame_PLSwapHoldem			= 1ull << 39,
	MobileGame_FLSwapHoldem			= 1ull << 40,
	MobileGame_NLTempestHoldem		= 1ull << 41,
	MobileGame_PL6Omaha				= 1ull << 42,
	MobileGame_Last = MobileGame_PL6Omaha, // !!!please keep up to date
};

// tournament buy-in options
enum MobileBuyInTypes
{
	MobileBuyIn_None = 0,
	MobileBuyIn_Cash = 1<<1,
	MobileBuyIn_FPP = 1<<2,
	MobileBuyIn_PlayMoney = 1<<3,
	MobileBuyIn_FreeRoll = 1<<4,
	MobileBuyIn_Last = MobileBuyIn_FreeRoll, // !!!please keep up to date
};

// maximum players at the table
enum MobileMaxPlayerTypes
{
	// tournaments
	MobileMaxPlayer_TwoMax = 1<<1,
	MobileMaxPlayer_ThreeMax = 1<<2,
	MobileMaxPlayer_FourMax = 1<<3,
	MobileMaxPlayer_FiveMax = 1<<4,
	MobileMaxPlayer_SixMax = 1<<5,
	MobileMaxPlayer_SevenMax = 1<<6,
	MobileMaxPlayer_EightMax = 1<<7,
	MobileMaxPlayer_NineMax = 1<<8,
	MobileMaxPlayer_TenMax = 1<<9,
	
	MobileMaxPlayer_Last = MobileMaxPlayer_TenMax, // !!!please keep up to date
	MobileMaxPlayer_ThreeToSix = MobileMaxPlayer_ThreeMax | MobileMaxPlayer_FourMax | MobileMaxPlayer_FiveMax | MobileMaxPlayer_SixMax,
	MobileMaxPlayer_SevenToTen = MobileMaxPlayer_SevenMax | MobileMaxPlayer_EightMax | MobileMaxPlayer_NineMax | MobileMaxPlayer_TenMax,
};

enum MobileTournSpeedTypes
{
	MobileTournSpeed_Regular	= 1<<0,
	MobileTournSpeed_Turbo		= 1<<1,
	MobileTournSpeed_Hyper		= 1<<2,
	MobileTournSpeed_Slow		= 1<<3,
	MobileTournSpeed_Last = MobileTournSpeed_Slow, // !!!please keep up to date
};

// currencies used for Real Money
enum MobileCurrencyTypes
{
	MobileCurrency_None = 0, 
	MobileCurrency_USD = 1<<0, // US dollars
	MobileCurrency_EUR = 1<<1, // Euro Currency
	MobileCurrency_GBP = 1<<2, // British Pounds
	MobileCurrency_CAD = 1<<3, // Canadian Dollars
	MobileCurrency_INR = 1<<4, // Indian Rupees
	MobileCurrency_Last = MobileCurrency_INR, // !!!please keep up to date
};


// Game Format/Variant
enum MobileGameVariantTypes
{
	MobileGameVariant_None		= 0,	
	MobileGameVariant_Regular		= 1<<0,	//default
	MobileGameVariant_Knockout		= 1<<1, 
	MobileGameVariant_NotSatellite	= 1<<2,	//modifier for Regular and SitNGo
	MobileGameVariant_SitNGo		= 1<<3,
	MobileGameVariant_BeatTheClock	= 1<<4,
	MobileGameVariant_Zoom			= 1<<5,
	MobileGameVariant_SpinAndGo		= 1<<6,
	MobileGameVariant_Last = MobileGameVariant_SpinAndGo, // !!!please keep up to date
};


// tourn table size
enum MobileTournTableSizeTypes
{
	MobileTournTableSize_None = 0,
	MobileTournTableSize_HeadsUp = 1<<0,
	MobileTournTableSize_SingleTable = 1<<1,
	MobileTournTableSize_MultiTable = 1<<2,

	MobileTournTableSize_Last = MobileTournTableSize_MultiTable, // !!!please keep up to date
};


enum MobileSortOrderTypes
{
	MobileSortOrder_None = 0, 
	// table order
	MobileSortOrder_AvailSeats = 1, 
	MobileSortOrder_StakesAsc = 2,
	MobileSortOrder_StakesDesc = 3,
	MobileSortOrder_AveragePot = 4,
	MobileSortOrder_HandsPerHour = 5,
	MobileSortOrder_PlayersSeatedAsc = 6,
	MobileSortOrder_PlayersSeatedDesc = 7,
	// tournament order
	MobileSortOrder_StartDate = 8,
	MobileSortOrder_BuyInAsc = 9,
	MobileSortOrder_BuyInDesc = 10,
	MobileSortOrder_Enrolled = 11,
	MobileSortOrder_Last = MobileSortOrder_Enrolled, // !!!please keep up to date
};


enum MobileHybridLobbyGameTypes
{
	MobileHybridLobbyGameTypes_None = 0,
	MobileHybridLobbyGameTypes_SeatFinders = 1,
	MobileHybridLobbyGameTypes_Blitzes = 2,
	MobileHybridLobbyGameTypes_TableTypes = 4,

	// !!! MobileHybridLobbyGameTypes_Last must be updated
	MobileHybridLobbyGameTypes_Last = MobileHybridLobbyGameTypes_TableTypes,
};


// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_MLOBBY_FIND_RINGGAMES				1 // M4 - filterBody, maxTables
#define MSG_MLOBBY_FIND_RINGGAMES_REPLY			2 // 6[s|44<MM>] - errCode[errInfo|totalTables,returnedTables<statBody,dynaBody>]
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_MLOBBY_FIND_TOURNAMENTS				3 // M4 - filterBody, maxTournaments
#define MSG_MLOBBY_FIND_TOURNAMENTS_REPLY		4 // 6[s|44<MM>] - errCode[errInfo|totalTournaments,returnedTournaments<statBody,dynaBody>]
#define MSG_MLOBBY_FIND_TABLE_OLD				5 // M4<4> - filterBody,exludedSize<TableId32>
#define MSG_MLOBBY_FIND_TABLE_OLD_REPLY			6 // 6[s|MM] - errCode[errInfo|statBody,dynaBody]
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_MLOBBY_FIND_BLITZ					7 // M4 - filterBody, maxTables
#define MSG_MLOBBY_FIND_BLITZ_REPLY				8 // 6[s|44<MM>] - errCode[errInfo|totalBlitzTables,returnedBlitzTables<statBody,dynaBody>]
// !!! WARNING !!! If you modify this message, please let testautomation team know.
#define MSG_MLOBBY_FIND_TABLE64					9 // M4<8> - filterBody,exludedSize<TableId64>
#define MSG_MLOBBY_FIND_TABLE64_REPLY			10 // 6[s|MM] - errCode[errInfo|statBody,dynaBody]
#define MSG_MLOBBY_FIND_SEATFINDERS_AND_BLITZES	11 // M44<4> - filterBody,maxReturnedNum,excludedSeatFindersNum<seatFinderId>
#define MSG_MLOBBY_FIND_SEATFINDERS_AND_BLITZES_REPLY	12 // 6[s|44<bM>] - errCode[errInfo|totalNum,returnedNum<isBlitz,body>]


#define MSG_MLOBBY_FIND_RINGGAMES2						13 // see Protocol_MSG_MLOBBY_FIND_RINGGAMES2
#define MSG_MLOBBY_FIND_RINGGAMES2_REPLY				14 // see Protocol_MSG_MLOBBY_FIND_RINGGAMES2_REPLY
#define MSG_MLOBBY_FIND_TOURNAMENTS2					15 // see Protocol_MSG_MLOBBY_FIND_TOURNAMENTS2
#define MSG_MLOBBY_FIND_TOURNAMENTS2_REPLY				16 // see Protocol_MSG_MLOBBY_FIND_TOURNAMENTS2_REPLY
#define MSG_MLOBBY_FIND_SEATFINDERS_AND_BLITZES2		19 // see Protocol_MSG_MLOBBY_FIND_SEATFINDERS_AND_BLITZES2
#define MSG_MLOBBY_FIND_SEATFINDERS_AND_BLITZES2_REPLY	20 // see Protocol_MSG_MLOBBY_FIND_SEATFINDERS_AND_BLITZES2_REPLY
#define MSG_MLOBBY_FIND_TABLE64_2						21 // see Protocol_MSG_MLOBBY_FIND_TABLE64_2
#define MSG_MLOBBY_FIND_TABLE64_2_REPLY					22 // see Protocol_MSG_MLOBBY_FIND_TABLE64_2_REPLY

// PYR-58846
#define MSG_MLOBBY_FIND_MORPH_LOBBIES					23 // see Protocol_MSG_MLOBBY_FIND_MORPH_LOBBIES
#define MSG_MLOBBY_FIND_MORPH_LOBBIES_REPLY				24 // see Protocol_MSG_MLOBBY_FIND_MORPH_LOBBIES_REPLY

// PYR-97056
#define MSG_MLOBBY_FIND_HYBRID_GAMES					25 // see Protocol_MSG_MLOBBY_FIND_HYBRID_GAMES
#define MSG_MLOBBY_FIND_HYBRID_GAMES_REPLY				26 // see Protocol_MSG_MLOBBY_FIND_HYBRID_GAMES_REPLY

#define MSG_MLOBBY_FIND_RING_GAME_BY_TYPE				27 // see Protocol_MSG_MLOBBY_FIND_RING_GAME_BY_TYPE
#define MSG_MLOBBY_FIND_RING_GAME_BY_TYPE_REPLY			28 // see Protocol_MSG_MLOBBY_FIND_RING_GAME_BY_TYPE_REPLY


#endif	//mlobby_h_included
