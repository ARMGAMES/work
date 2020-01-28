#ifndef Notify_Message_Defs_h_included
#define Notify_Message_Defs_h_included

#include "gamedefs.h"
#include "composecolls.h"
#include "misc.h"
#include "licenserules.h"

// PotStruct enum moved to gamedefs.h

// PYR-23129
// used as 'structure' param in the NotifyMessage
inline UINT32 potStructFlag(PotStruct tp)
{
	switch(tp)
	{
	case StructLimit:
	case StructPotLimit:
	case StructNoLimit:
	case StructPotNoLimit:
		return 1<<(tp-1);
	case StructMultiGameMixed:
		return 1<<(tp-StructMultiGameMixed+7); //-V2007
	default:
		return 0;
	}
}

// PYR-107722
enum eNotifyMessageFlags
{
	eNotifyMessageFlags_NONE = 0,
	eNotifyMessageFlags_FORCE_TO_TOP = (1 << 0), // 0x1 PYR-107722
	eNotifyMessageFlags_FORCE_TO_TOP_STICKY = (1 << 1), // 0x2 PYR-107722
	eNotifyMessageFlags_SEND_ADMIN_EMAIL = (1 << 2) // 0x4 PYR-107722
};

// PYR-23129
inline UINT32 potStructFlag(BYTE tp)
{
	return potStructFlag((PotStruct) tp);
}

// PYR-23129
inline bool isPotStructSet(UINT32 flags, PotStruct tp)
{
	return ( ( flags & potStructFlag(tp) ) != 0 ) ;
}

// PYR-23129
// used as 'format' param in the NotifyMessage
enum eNotifyMsgFormats
{
	eNotifyMsgRingGame =	1<<0,
	eNotifyMsgRingZoom =	1<<1,
	eNotifyMsgTournament =	1<<2,
	eNotifyMsgSitNGo =		1<<3,
	eNotifyMsgSAG =			1<<4,
	eNotifyMsgSportbooks =	1<<5, // PYR-44486

	eNotifyMsgPopUp =		1<<7
};

//PYR-21560
#define TABLE_STAKES_NL_PL_HIGH			500
#define TABLE_STAKES_NL_PL_MEDIUM		100
#define TABLE_STAKES_NL_PL_LOW			25

#define TABLE_STAKES_FL_HIGH			1500
#define TABLE_STAKES_FL_MEDIUM			500
#define TABLE_STAKES_FL_LOW				50

#define TOURN_STAKES_HIGH				10000
#define TOURN_STAKES_MEDIUM				2000
#define TOURN_STAKES_LOW				500

#define TOURN_STAKES_PLAYMONEY_HIGH		2000000
#define TOURN_STAKES_PLAYMONEY_MEDIUM	200000

enum StakesMask
{
	StakePlayMoney =	0x01,
	StakeMicro =		0x02,
	StakeLow =			0x04,
	StakeMedium =		0x08,
	StakeHigh =			0x10,

	StakeRealMoney_All = (StakeMicro | StakeLow | StakeMedium | StakeHigh),
	Stake_All = (StakeRealMoney_All | StakePlayMoney)
};


inline StakesMask calculateTournStake(UINT32 buyIn, BYTE playMoney)
{
	// prepare this tournament data to compare
	if ( 0 != playMoney ) return StakePlayMoney;

	if(buyIn >= TOURN_STAKES_HIGH) return StakeHigh;
	if(buyIn >= TOURN_STAKES_MEDIUM) return StakeMedium;
	if(buyIn >= TOURN_STAKES_LOW) return StakeLow;

	return StakeMicro;
}

inline StakesMask calculateTableStake(UINT32 loBet, BYTE playMoney, BYTE structure)
{
	// prepare this tournament data to compare
	if ( 0 != playMoney ) return StakePlayMoney;

	if( StructLimit == structure)
	{
		if(loBet >= TABLE_STAKES_FL_HIGH) return StakeHigh;
		if(loBet >= TABLE_STAKES_FL_MEDIUM) return StakeMedium;
		if(loBet >= TABLE_STAKES_FL_LOW) return StakeLow;

		return StakeMicro;
	}

	if(loBet >= TABLE_STAKES_NL_PL_HIGH) return StakeHigh;
	if(loBet >= TABLE_STAKES_NL_PL_MEDIUM) return StakeMedium;
	if(loBet >= TABLE_STAKES_NL_PL_LOW) return StakeLow;

	return StakeMicro;

}

enum eUserNotifyFlags
{
	eUserNotifyFlags_FreemiumPlayer		= 1<<0, // 1
	eUserNotifyFlags_NotFreemiumPlayer	= 1<<1, // 2
	eUserNotifyFlags_RmPlayer			= 1<<2, // 4
	eUserNotifyFlags_NotRmPlayer		= 1<<3, // 8
};

enum eGameTypeFlags
{
	eGameTypeFlags_HandTypeNone				= 0,	// 0
	// eGameTypeFlags_not used				= 1<<0,	// 1
	eGameTypeFlags_HandTypeBadugi			= 1<<1,	// 2
	eGameTypeFlags_HandTypeHoldem			= 1<<2,	// 4
	eGameTypeFlags_HandType5Omaha_hilo_0	= 1<<3, // 8
	eGameTypeFlags_HandType5Omaha_hilo_1	= 1<<4, // 16
	eGameTypeFlags_HandTypeOmaha_hilo_0		= 1<<5, // 32
	eGameTypeFlags_HandTypeOmaha_hilo_1		= 1<<6, // 64
	eGameTypeFlags_HandTypeDraw				= 1<<7,	// 128
	eGameTypeFlags_HandTypeSingleDraw27		= 1<<8,	// 256
	eGameTypeFlags_HandTypeStud_hilo_0		= 1<<9,	// 128
	eGameTypeFlags_HandTypeStud_hilo_1		= 1<<10, // 1024
	eGameTypeFlags_HandTypeRazz				= 1<<11, // 2048
	eGameTypeFlags_HandType3Draw27			= 1<<12, // 4096
	eGameTypeFlags_HandType3DrawA5			= 1<<13, // 8192
	eGameTypeFlags_HandTypeCourchevel_hilo_0= 1<<14, // 16384
	eGameTypeFlags_HandTypeCourchevel_hilo_1= 1<<15, // 32768
	eGameTypeFlags_MultiGameHose			= 1<<16, // 65536
	eGameTypeFlags_MultiGameHorse			= 1<<17, // 131072
	eGameTypeFlags_MultiGameEightGame		= 1<<18, // 262144
	eGameTypeFlags_MultiGameHoldemMixed		= 1<<19, // 524288
	eGameTypeFlags_MultiGameOmahaHiLoMixed	= 1<<20, // 1048576
	eGameTypeFlags_MultiGameHoldemOmahaPl	= 1<<21, // 2097152
	eGameTypeFlags_MultiGameTripleStud		= 1<<22, // 4194304
	eGameTypeFlags_MultiGameHoldemNlOmaha	= 1<<23, // 8388608
	eGameTypeFlags_MultiGameOmahaMixed		= 1<<24, // 16777216
	eGameTypeFlags_HandType6Omaha_hilo_0	= 1<<25, // 33554432
	eGameTypeFlags_HandType6Omaha_hilo_1	= 1<<26, // 67108864
};

inline eGameTypeFlags calculateGameTypeFlag(BYTE game, BYTE isHiLo)
{
	switch(game)
	{
	case HandTypeBadugi:
		return eGameTypeFlags_HandTypeBadugi;
	case HandTypeHoldem:
		return eGameTypeFlags_HandTypeHoldem;
	case HandType5Omaha:
		return isHiLo ? eGameTypeFlags_HandType5Omaha_hilo_1 : eGameTypeFlags_HandType5Omaha_hilo_0;
	case HandType6Omaha:
		return isHiLo ? eGameTypeFlags_HandType6Omaha_hilo_1 : eGameTypeFlags_HandType6Omaha_hilo_0;
	case HandTypeOmaha:
		return isHiLo ? eGameTypeFlags_HandTypeOmaha_hilo_1 : eGameTypeFlags_HandTypeOmaha_hilo_0;
	case HandTypeDraw:
		return eGameTypeFlags_HandTypeDraw;
	case HandTypeSingleDraw27:
		return eGameTypeFlags_HandTypeSingleDraw27;
	case HandTypeStud:
		return isHiLo ? eGameTypeFlags_HandTypeStud_hilo_1 : eGameTypeFlags_HandTypeStud_hilo_0;
	case HandTypeRazz:
		return eGameTypeFlags_HandTypeRazz;
	case HandType3Draw27:
		return eGameTypeFlags_HandType3Draw27;
	case HandType3DrawA5:
		return eGameTypeFlags_HandType3DrawA5;
	case HandTypeCourchevel:
		return isHiLo ? eGameTypeFlags_HandTypeCourchevel_hilo_1 : eGameTypeFlags_HandTypeCourchevel_hilo_0;
	case MultiGameHose:
		return eGameTypeFlags_MultiGameHose;
	case MultiGameHorse:
		return eGameTypeFlags_MultiGameHorse;
	case MultiGameEightGame:
		return eGameTypeFlags_MultiGameEightGame;
	case MultiGameHoldemMixed:
		return eGameTypeFlags_MultiGameHoldemMixed;
	case MultiGameOmahaHiLoMixed:
		return eGameTypeFlags_MultiGameOmahaHiLoMixed;
	case MultiGameHoldemOmahaPl:
		return eGameTypeFlags_MultiGameHoldemOmahaPl;
	case MultiGameTripleStud:
		return eGameTypeFlags_MultiGameTripleStud;
	case MultiGameHoldemNlOmaha:
		return eGameTypeFlags_MultiGameHoldemNlOmaha;
	case MultiGameOmahaMixed:
		return eGameTypeFlags_MultiGameOmahaMixed;
	default:
		return eGameTypeFlags_HandTypeNone;
	}
}

inline bool doesTournFormatFit(UINT32 formatFlags, bool isSitNGo, bool isSAG)
{
	PTRACE5("doesTournFormatFit %u %c%c", formatFlags, boolAsPlusMinus(isSitNGo), boolAsPlusMinus(isSAG) );
	PASSERT5( !isSAG || isSitNGo ); // NOTE: any SAG must be SnG as well

	formatFlags &= ~(eNotifyMsgPopUp | eNotifyMsgSportbooks); // ignore the popup/sprotsbooks flag 

	if( 0 == formatFlags ) return true;

	// is SAG but this option is not specified
	if( !(formatFlags & eNotifyMsgSAG) && isSAG ) 
	{
		PTRACE5("!eNotifyMsgSAG");
		return false;
	}

	// is SnG but this option is not specified
	// NOTE: any SAG is SnG as well
	if( !(formatFlags & eNotifyMsgSitNGo) && isSitNGo && !isSAG) 
	{
		PTRACE5("!eNotifyMsgSitNGo");
		return false;
	}

	// is regular (= not SAG and not SnG ) tournament but this option is not specified
	// NOTE: any SAG is SnG as well
	if( !(formatFlags & eNotifyMsgTournament) && !isSitNGo ) 
	{
		PTRACE5("!eNotifyMsgTournament");
		return false;
	}

	return true;
}

inline bool doesZoomFormatFit(UINT32 formatFlags)
{
	PTRACE5("doesZoomFormatFit %u", formatFlags);

	formatFlags &= ~(eNotifyMsgPopUp | eNotifyMsgSportbooks); // ignore the popup/sprotsbooks flag 

	if( 0 == formatFlags ) return true;

	// is zoom but this option is not specified
	if( !(formatFlags & eNotifyMsgRingZoom) ) 
	{
		PTRACE5("!eNotifyMsgRingZoom");
		return false;
	}
		
	return true;
}

inline bool doesTableFormatFit(UINT32 formatFlags, bool isZoom)
{
	PTRACE5("doesTableFormatFit %u %c", formatFlags, boolAsPlusMinus(isZoom) );

	formatFlags &= ~(eNotifyMsgPopUp | eNotifyMsgSportbooks); // ignore the popup/sprotsbooks flag 

	if( 0 == formatFlags ) return true;

	// is zoom but this option is not specified
	if( !(formatFlags & eNotifyMsgRingZoom) && isZoom ) 
	{
		PTRACE5("!eNotifyMsgRingZoom");
		return false;
	}

	// is reg game but this option is not specified
	if( !(formatFlags & eNotifyMsgRingGame) && !isZoom ) 
	{
		PTRACE5("!eNotifyMsgRingGame");
		return false;
	}

	return true;
}

inline bool doesSiteFit(const PSiteMask& tournOrTableSite, const PSiteMask& siteMask, BYTE isPlayMoney)
{
	PString hexStr1, hexStr2;
	PTRACE5("doesSiteFit 0x%s 0x%s pm=%d", tournOrTableSite.toDbString(hexStr1), siteMask.toDbString(hexStr2), isPlayMoney );

	if( !( tournOrTableSite & siteMask ) )
	{
		PTRACE5("!site m");
		return false;
	}
		
	// do no check segregation for play money
	if(isPlayMoney)
		return true;

	// segregated site for Real Money
	// must be exact fit
	if ( SiteRules::isSegregatedSiteMask( tournOrTableSite | siteMask ) )
	{
		if(tournOrTableSite != siteMask)
		{
			PTRACE5("!site");
			return false;
		}
	}

	// shared liquidity pool 
	if ( SiteRules::isSharedLiquidityPoolMask( tournOrTableSite | siteMask ) )
	{
		if( !SiteRules::belongsToSharedLiquidityPool( tournOrTableSite ) || !SiteRules::belongsToSharedLiquidityPool( siteMask ) )
		{
			PTRACE5("!site pl");
			return false;
		}
	}

	return true;
}

inline bool doesGameTypeFit(UINT64 gameFlags, BYTE game, BYTE isHiLo)
{		
	PTRACE5("doesGameTypeFit %" PRIu64 " %u %u", gameFlags, game, isHiLo);

	if( 0 != gameFlags && !(gameFlags & calculateGameTypeFlag(game, isHiLo) ))
	{
		PTRACE5("!gameFlags");
		return false;
	}

	return true;
}

inline bool doesStructureFit(UINT32 structFlags, BYTE structure)
{
	PTRACE5("doesStructureFit %u %u", structFlags, structure);

	if( 0 != structFlags && !(structFlags & potStructFlag(structure) ))
	{
		PTRACE5("!structFlags");
		return false;
	}

	return true;
}

inline bool doesTableStakeFit(UINT32 stakeFlags, UINT32 loBet, BYTE playMoney, BYTE structure)
{
	PTRACE5("doesTableStakeFit %u %u %u %u", stakeFlags, loBet, playMoney, structure);

	if( 0 != stakeFlags && !(stakeFlags & calculateTableStake(loBet, playMoney, structure )) )
	{
		PTRACE5("!stakeFlags");
		return false;
	}

	return true;
}

inline bool doesTournStakeFit(UINT32 stakeFlags, UINT32 buyIn, BYTE playMoney)
{
	PTRACE5("doesTournStakeFit %u %u %u", stakeFlags, buyIn, playMoney);

	if( 0 != stakeFlags && !(stakeFlags & calculateTournStake(buyIn, playMoney )) )
	{
		PTRACE5("!stakeFlags");
		return false;
	}

	return true;
}

inline bool doesBrandFit(INT32 brandMask, INT32 brandId)
{
	PTRACE5("doesBrandFit %d %d", brandMask, brandId);

	if( (brandMask & brandId) == 0 )
	{
		PTRACE5("!doesBrandFit");
		return false;
	}

	return true;
}

struct UserSpecificNotifyParams
{
	//!!! Important: Please also modify the P_UserSpecificNotifyParams struct in srvshared/table/tableserverprot.txt 
	//               when this struct is modified.

	// UINT32 site32Obsolete;  // not existing, though parsed/composed
	UINT32 vipFlags;
	UINT32 userFlags;
	INT32 lifetimeVppMin;
	INT32 lifetimeVppMax;
	INT32 yearlyVppMin;
	INT32 yearlyVppMax;
	INT32 monthlyVppMin;
	INT32 monthlyVppMax;
	INT64 pmBalanceMin;
	INT64 pmBalanceMax;
	INT32 rmBalanceMin;
	INT32 rmBalanceMax;
	SrvTime firstDepositDateMin;
	SrvTime firstDepositDateMax;
	SrvTime registeredDateMin;
	SrvTime registeredDateMax;
	vector<PString> countryList;
	vector<PString> trackCodes;
	UINT32 brandMask; // PYR-41795
	UINT32 appTypeMask; // PYR-47691
	set<UINT8> lastUserDigit; // PYR-47020
	PSiteMask siteMask;

	
	UserSpecificNotifyParams() : 
		vipFlags(0),
		userFlags(0),
		lifetimeVppMin(0),
		lifetimeVppMax(0),
		yearlyVppMin(0),
		yearlyVppMax(0),
		monthlyVppMin(0),
		monthlyVppMax(0),
		pmBalanceMin(0),
		pmBalanceMax(0),
		rmBalanceMin(0),
		rmBalanceMax(0),
		brandMask(BrandType_ALLMask), // PYR-41795
		appTypeMask(0) // PYR-47691
	{}

	UserSpecificNotifyParams(CommMsgParser& parser)
	{
		parse(parser);
	}

	bool  isSet() const
	{
		// NOTE brandMask is excluded from the condition
		return (!countryList.empty() || !trackCodes.empty()
			|| !firstDepositDateMin.isNull() || !firstDepositDateMax.isNull()
			|| !registeredDateMin.isNull() || !registeredDateMax.isNull()
			|| siteMask || vipFlags || userFlags
			|| lifetimeVppMin || lifetimeVppMax
			|| yearlyVppMin || yearlyVppMax
			|| monthlyVppMin || monthlyVppMax
			|| pmBalanceMin || pmBalanceMax
			|| rmBalanceMin || rmBalanceMax
			|| appTypeMask // PYR-47691
			|| !lastUserDigit.empty() // PYR-47020
			);
	}

	void parse(CommMsgParser& parser)
	{
		CommMsgBody body;
		parser.parseMsgBody(body);
		CommMsgParser bodyParser(body);
		UINT32 lastUserDigitLen;
		UINT8 lastUserDigitNum;
		parseWithoutMsgBody(bodyParser);
		bodyParser.parseUINT32(appTypeMask) // PYR-47691
				.parseUINT32(lastUserDigitLen); // PYR-47020

		// PYR-47020
		for(int i = 0; i < lastUserDigitLen; ++i)
		{
			bodyParser.parseBYTE(lastUserDigitNum);
			lastUserDigit.insert(lastUserDigitNum);
		}
		bodyParser.parseObject(siteMask);
	}

	// PYR-47691
	void parseWithoutMsgBody(CommMsgParser& parser)
	{
		UINT32 site32Dummy;
		parser
			.parseUINT32(site32Dummy)
			.parseUINT32(vipFlags)
			.parseUINT32(userFlags)
			.parseINT32(lifetimeVppMin)
			.parseINT32(lifetimeVppMax)
			.parseINT32(yearlyVppMin)
			.parseINT32(yearlyVppMax)
			.parseINT32(monthlyVppMin)
			.parseINT32(monthlyVppMax)
			.parseINT64(pmBalanceMin)
			.parseINT64(pmBalanceMax)
			.parseINT32(rmBalanceMin)
			.parseINT32(rmBalanceMax)
			.parseSrvTime(firstDepositDateMin)
			.parseSrvTime(firstDepositDateMax)
			.parseSrvTime(registeredDateMin)
			.parseSrvTime(registeredDateMax)
			;

		parseVectorWithOverride(parser, countryList, ComposerPString());
		parseVectorWithOverride(parser, trackCodes, ComposerPString());
		// PYR-41795
		parser.parseUINT32(brandMask);
		appTypeMask = 0;  // PYR-47691
	}
	
	void compose( CommMsgBody& body ) const
	{
		CommMsgBody paramBody;
		paramBody
			.composeUINT32(0) // siteID 32 bits preserved for compatibility
			.composeUINT32(vipFlags)
			.composeUINT32(userFlags)
			.composeINT32(lifetimeVppMin)
			.composeINT32(lifetimeVppMax)
			.composeINT32(yearlyVppMin)
			.composeINT32(yearlyVppMax)
			.composeINT32(monthlyVppMin)
			.composeINT32(monthlyVppMax)
			.composeINT64(pmBalanceMin)
			.composeINT64(pmBalanceMax)
			.composeINT32(rmBalanceMin)
			.composeINT32(rmBalanceMax)
			.composeSrvTime(firstDepositDateMin)
			.composeSrvTime(firstDepositDateMax)
			.composeSrvTime(registeredDateMin)
			.composeSrvTime(registeredDateMax)
			;

		composeVector(paramBody, countryList, ComposerPString());
		composeVector(paramBody, trackCodes, ComposerPString());

		paramBody
			.composeUINT32(brandMask)
			.composeUINT32(appTypeMask) // PYR-47691
			.composeUINT32(static_cast<UINT32>(lastUserDigit.size())) // PYR-47020
			;

		// PYR-47020
		set<UINT8>::const_iterator intIter;
		for( intIter = lastUserDigit.begin(); intIter != lastUserDigit.end(); ++intIter )
		{
			paramBody.composeBYTE(*intIter);
		}
		paramBody.composeObject(siteMask);

		body.composeMsgBody(paramBody);
	}

	void trace5() const
	{
#if PTRACELVL >= 5
		print();
#endif
	}

	void print() const
	{
		PString  t1, t2, t3, t4, s1, s2, s3;

		bool first = true;
		for( const UINT8 dg : lastUserDigit )
		{
			if( !first )
			{
				s3.append( ',' );
			}
			else
			{
				first = false;
			}
			
			s3.appendUint( dg );
		}
		
		PString strBuf;
		PLog( "{st=0x%s vipf=%u uf=%u lfvmn=%d lfvmx=%d yvmn=%d yvmx=%d mvmn=%d mvmx=%d, pmmn=%lld pmmx=%lld rmmn=%d rmmx=%d fdmn=%s fdmx=%s rgmn=%s rgmx=%s cntr=(%s) trk=(%s) brnd=%u app=%u lds=(%s)}",
			siteMask.toDbString(strBuf), vipFlags, userFlags, lifetimeVppMin, lifetimeVppMax, yearlyVppMin, yearlyVppMax, monthlyVppMin, monthlyVppMax, pmBalanceMin, pmBalanceMax, rmBalanceMin, rmBalanceMax,
			srvTimeToString(firstDepositDateMin, t1), srvTimeToString(firstDepositDateMax, t2), srvTimeToString(registeredDateMin, t3), srvTimeToString(registeredDateMax, t4),
			strVectorToStr(s1, countryList), strVectorToStr(s2, trackCodes), brandMask, appTypeMask, s3.c_str() );
	}
};

#endif	//Notify_Message_Defs_h_included
