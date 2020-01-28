// ===========================================================
// Copyright (C) 2012 PYR Software Ltd. All rights reserved.
// ===========================================================

#ifndef spinandgo_h_included
#define spinandgo_h_included

#include "dbm.h"
#include "sites.h"
#include "brands.h"
#include "pplogfile.h"
#include "commmsgbody.h"
#include "sitemasks.h"

#ifndef PROTOCOL_CLIENT_ONLY
#include "licenserules.h"
#endif

#define MAX_SAG_LEVELS 20
#define SAG_FULL_PROBABILITY 100000000
#define NUM_SAG_MAX_SPINNERS 3


//<-- TO BE REMOVED----------------------------------
#define SAG_PUBL_OLD_DATA_PREFIX "sag."
#define SAG_PUBL_DATA_PREFIX "sag1."
#define SAG_PUBL_DATA_PREFIX2 "sag2."
#define SAG_PUBL_DATA_PREFIX3 "sag3."
#define SAG_PUBL_DATA_PREFIX4 "sag4."
//-- TO BE REMOVED---------------------------------->

//PYR-59442
#define SAG_PUB_0 "sag0."

class SagDefs
{
public:
	//Used as default for gaming objects, consult VadimSh if you change anything here
	enum eMarket
	{
		eMarketUnknown = 0,
		eMarketSharedLiquidity = 1,
		eMarketIta = 2,
		eMarketFra = 3,
		eMarketSpa = 4,
		eMarketSharedPlayMoney = 5,
		eMarketUS_NJ = 6,
		eMarketPor = 7,
		eMarketInd = 8, // PYR-52215
		eMarketUS_PA = 9,
	
		eMarketLast = 10, 
		eMarketSharedLiquidityPool = eMarketLast //not going to iterate through it
	};

	// PYR-52215 - refactored market->site->string functionality
	struct Market
	{
		eMarket market;
		PSiteMask site;
		const char* marketStr;
	};

#ifndef PROTOCOL_CLIENT_ONLY

	static const Market* getMarketSiteStringMap( INT32& size )
	{
		static const Market marketMap[] =
		{
			// should be listed in the same order as in eMarket enum
			{ eMarketSharedLiquidity,	SiteBitmask_SharedLiquidity,	"COM"  },
			{ eMarketIta,				SiteBitmask_Ita,				"IT"   },
			{ eMarketFra,				SiteBitmask_Fra,				"FR"   },
			{ eMarketSpa,				SiteBitmask_Spa,				"ES"   },
			{ eMarketSharedPlayMoney,	SiteBitmask_SiteUnknown,		"NET"  },
			{ eMarketUS_NJ,				SiteBitmask_US_NJ,			"USNJ" },
			{ eMarketPor,				SiteBitmask_Por,				"PT"   },
			{ eMarketInd,				SiteBitmask_Ind,				"IN"   }, // PYR-52215
			{ eMarketUS_PA,				SiteBitmask_US_PA,			"USPA" },
		};
		size = sizeof(marketMap)/sizeof(marketMap[0]);
		return marketMap;
	}

	static bool isOneBit( UINT32 x ) { return ( x && (( x & x-1 ) == 0)); }

	static PSiteMask getFullSiteMaskByMarket( eMarket market )
	{
		INT32 size;
		const Market* const marketMap = getMarketSiteStringMap( size );
		for( INT32 i = 0; i < size; ++i )
		{
			if( market ==  marketMap[i].market )
			{
				return market == eMarketSharedPlayMoney ? SiteBitmask_AllValidMask : marketMap[i].site;
			}
		}

		return PSiteMask::zero;
	}

	static eMarket getMarketTypeBySiteMask ( const PSiteMask& siteMask, bool isPlayMoney )
	{
		if( isPlayMoney )
			return eMarketSharedPlayMoney;

		//PYR-59442 special case
		//PYR-65488 - return eMarketSharedLiquidityPool if >1 shared liquidity pool bits are set, and no conflicts are specified
	    //            this excludes from publication in legacy publications
		//            clients that support the euro shared liquidity pool use a different mechanism to determine availability so this is OK
		if(    ! siteMask.isOnlyOneBitSet()
			&& SiteRules::isSharedLiquidityPoolMask( siteMask )
			&& !SiteRules::isConflictingSharedLiquidityPoolMask( siteMask ) )
		{
			return eMarketSharedLiquidityPool;
		}

		eMarket result = eMarketUnknown;
		// PYR-52215 - refactored
		INT32 size;
		const Market* const marketMap = getMarketSiteStringMap( size );

		//we can't do as the following because .NET conflicting with other sites
		//for( INT32 i = 0; i < size; ++i )
		//{
		//	UINT32 intersection = siteMask & marketMap[i].site;
		//	if( intersection )
		//	{
		//		if( intersection == siteMask )
		//			return marketMap[i].market;
		//		else
		//			return eMarketUnknown;
		//	}
		//}
		//return eMarketUnknown;

		// restored previous logic due to AQA-12034
		for( INT32 i = 0; i < size; ++i )
		{
			if( siteMask & marketMap[i].site )
			{
				if( result )
					return eMarketUnknown;

				result = marketMap[i].market;
			}
		}
		return result;
	}

	static  const char * getMarketStringById( eMarket id ) 
	{
		switch( id )
		{
		case eMarketUnknown:
			return "unknown";

		default:
			{
				// PYR-52215 - refactored
				INT32 size;
				const Market* const marketMap = getMarketSiteStringMap( size );
				for( INT32 i = 0; i < size; ++i )
				{
					if( id == marketMap[i].market )
						return marketMap[i].marketStr;
				}
			}
			return "invalid";
		}
	}

	static  eMarket getMarketIdByString( const char * strCode ) 
	{
		if( !strCode || !*strCode )
			return eMarketUnknown;
		// PYR-52215 - refactored
		INT32 size;
		const Market* const marketMap = getMarketSiteStringMap( size );
		for( INT32 i = 0; i < size; ++i )
		{
			if( !strcmp( marketMap[i].marketStr, strCode ) )
				return marketMap[i].market;
		}
		return eMarketUnknown;
	}

	static bool isMarketOk( eMarket market ) { return market > SagDefs::eMarketUnknown && market < SagDefs::eMarketLast;  }

#endif // PROTOCOL_CLIENT_ONLY

	enum eCurrency
	{
		eCurrency_UNK = 0,	//unknown
		eCurrency_PM = 1,
		//eCurrency_FPP = (1 << 1),	//reserved
		eCurrency_USD = (1 << 2),
		eCurrency_EUR = (1 << 3),
		eCurrency_GBP = (1 << 4),
		eCurrency_CAD = (1 << 5),
		eCurrency_INR = (1 << 6), // PYR-30558
	};
	// PYR-30558 - refactored currency check
	struct Currency
	{
		eCurrency currBit;
		const char* currName;
	};
	static const Currency* getRealMoneyCurrencyMap( INT32& size )
	{
		static const Currency currMap[] =
		{
			{ eCurrency_USD, ""    },
			{ eCurrency_USD, "USD" },
			{ eCurrency_EUR, "EUR" },
			{ eCurrency_CAD, "CAD" },
			{ eCurrency_GBP, "GBP" },
			{ eCurrency_INR, "INR" }, // PYR-30558
		};
		size = sizeof(currMap)/sizeof(currMap[0]);
		return currMap;
	}

	static bool isPlayMoney( eCurrency val )
	{
		switch(val)
		{
		case eCurrency_PM: 
			return true;
		default:
			{
				INT32 size;
				const Currency* const currMap = getRealMoneyCurrencyMap( size );
				for( INT32 i = 0; i < size; ++i )
				{
					if( currMap[i].currBit == val )
						return false;
				}
			}
			PTRACE( LOG_TRACE_FAULTY_LOGIC ": invalid currency %u", val );
			return false;
		}
	}

	static  eCurrency getSagCurrencyByString( const char * currencyString )
	{
		if( !currencyString )
			return eCurrency_UNK;

		INT32 size;
		const Currency* const currMap = getRealMoneyCurrencyMap( size );
		for( INT32 i = 0; i < size; ++i )
		{
			if( !PString::compareIgnoreCase( currMap[i].currName, currencyString ) )
				return currMap[i].currBit;
		}
		return eCurrency_UNK; 
	}

	static  const char* getCurrencyStringById( INT32 id )
	{
		switch( id )
		{
		case eCurrency_UNK:
			return "unknown";

		case eCurrency_PM:
			return "play money";

		default:
			{
				INT32 size;
				const Currency* const currMap = getRealMoneyCurrencyMap( size );
				for( INT32 i = 0; i < size; ++i )
				{
					if( currMap[i].currBit == id && *currMap[i].currName )
						return currMap[i].currName;
				}
			}
			return "invalid";
		}
	}
};

enum eSagBannerParamFlags
{
	sbpfCountup =		( 1 << 0 ),
	sbpfRunningTourn =	( 1 << 1 ),
	sbpfExBuyIn =		( 1 << 2 ),
	sbpfExPrizePool =	( 1 << 3 ),
	sbpfExNumPlayed =	( 1 << 4 ),
	sbpExCurrency =		( 1 << 5 ),
};

enum eSagTicketOptions
{ 
	stoNoTicketRequired = 0, 
	stoTicketAndBuyIn = 1, 
	stoTicketOrBuyIn = 2, 
	stoTicketOnly = 3, 
	stoTicketFreeRoll = 4 
};

// PYR-43478
enum ePayoutTicketFlags
{ 
	ePayoutTicketFlags_No						= 0, 
	ePayoutTicketFlags_CanUnregFromTarget		= ( 1 << 0 ), 
	ePayoutTicketFlags_CannotUnregFromTarget	= ( 1 << 1 ), 
	ePayoutTicketFlags_GiveTargetTicket			= ( 1 << 2 ), 
	ePayoutTicketFlags_RegisterTotarget			= ( 1 << 3 ), 
};

enum eSpinnerPrizeType
{ 
	sptUnknown = 0,
	sptMoney = 1,
	sptCustomPrizeTicket = 2,
	sptCustomPrizeEntry = 3,
};


struct SpinnerPrize
{
	UINT32	value;	//for money it's total prize pool, for custom prizes it's ticket type ID for 1st place award
	BYTE	level;
	BYTE	type;
	PString targetRef;

	SpinnerPrize( BYTE level_ = 0, BYTE type_ = 0, UINT32 value_ = 0, const char* targetRef_= "" )
		:   value(value_), level(level_), type(type_), targetRef(targetRef_) {}
	bool operator ==( const SpinnerPrize& other) const
	{
		return level == other.level 
			&& type == other.type
			&& value == other.value
			&& targetRef.equals( other.targetRef )
			;
	}

	void compose( CommMsgBody& body ) const
	{
		CommMsgBody itemBody;
		itemBody
			.composeBYTE( level )
			.composeBYTE( type )
			.composeUINT32( value )
			.composeString( targetRef )
			;
		body.composeMsgBody( itemBody );
	}
	
	void parse( CommMsgParser& parser )
	{
		CommMsgBody itemBody;
		parser.parseMsgBody( itemBody );
		CommMsgParser itemParser( itemBody );
		itemParser
			.parseBYTE( level )
			.parseBYTE( type )
			.parseUINT32( value )
			.parseStringP( targetRef )
			;
	}
};

typedef vector< SpinnerPrize > SpinnerPrizes;

struct SagLevelPlace
{
	static UINT32 encode( UINT32 level, UINT32 place ) { return level * 1000 + place; }
	static void decode( UINT32 ticketId, UINT32& level, UINT32& place ) { level = ticketId / 1000; place = ticketId - level * 1000; }
};

#define DEFAULT_SAG_TICKET_TEMPLATE_ID "DEFAULT_TICKET"
#define DEFAULT_SAG_TARGETREF_TEMPLATE_ID "DEFAULT_TARGETREF"
#define SAG_TOURN_NAME_SEARCH_STRING "%TOURN_NAME%"
#define SAG_TICKET_NAME_SEARCH_STRING "%TICKET_NAME%"


enum eSpinAndGoPublicationFlags
{ 
	espfPaused		= (1 << 0),
	espfSagMax		= (1 << 1),
	espfFixedPayout = (1 << 2),
	espfPromoMode	= (1 << 3),
	espfVariants	= (1 << 4),
};

enum eSpinAndGoMaxFinalDrawAction
{ 
	efdaNone = 0,
	efda1stSpinner = 1,
	efda2ndSpinner = 2,
	efda3rdSpinner = 3,
	efdaCashout = 4,
	efdaLast = 5
};

enum eSpinAndGoAnimationFlags
{ 
	esafNone				= 0,
	esafPrizePoolDraw		= (1 << 0),
	esafPlayersNumber		= (1 << 1),
	esafWinnerPrize			= (1 << 2),
	esafWinnerPrizeDisplay	= (1 << 3),
};

enum eSpinAndGoVariants
{
	esvBase = 0,
	esvFlash = 1,
	esvLast = 2,
};

#define SAG_ANIMATION_EXTRA_TIME_SEC 5					// extra time
#define SAG_ANIMATE_NPLAYERS_TIME_SEC 8					// SAG MAX number of users animation
#define SAG_ANIMATE_REG_DRAW_TIME_SEC 10				// regular SAG animation for prizepool draw
#define SAG_ANIMATE_3SPINNER_DRAW_TIME_SEC 13			// SAG MAX animation for triple spinner
#define SAG_ANIMATE_3SPINNER_MERGE_TIME_SEC 15			// SAG MAX animation for top prize merge
#define SAG_ANIMATE_WINNER_PRIZE_TIME_SEC 5				// SAG MAX prompt animation for winner to pick the prize
#define SAG_ANIMATE_3SPINNER_WINNER_PRIZE_DISPLAY_TIME_SEC 7	// SAG MAX animation showing the winner's pick
#define SAG_ANIMATE_TOP_PRIZE_WINNER_PRIZE_DISPLAY_TIME_SEC 9	// SAG MAX animation showing top prize
#define SAG_MAX_FINAL_DRAW_DELAY_SEC 45 
#define SAG_MAX_WINNER_POPUP_DELAY_SEC 3

#endif
