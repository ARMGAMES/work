#ifndef HgDbmSharedCli_h_included
#define HgDbmSharedCli_h_included

////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2010 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////


#include "commmsgbody.h"
#include "composecolls.h"
#include "PlayChipsScaler.h"
#include "commonmacros.h"
#include "PBitmask.h"
#include "sitemasks.h"

//////////////////////////////////////////////////////////////////////////

// PYR-40177 Symbols for Home Games client protocol versions
#define HG_CLI_VER_1	1	// Not scalePM aware
#define HG_CLI_VER_2	2	// scalePM aware

enum HomeGamesClubStatus
{
	eHgClubStatusActive		= 0,
	eHgClubStatusSuspended	= 1,
	eHgClubStatusClosed		= 2,
};

enum HomeGamesClubSeasons
{
	eHgClubSeasonsYearly	= 0,
	eHgClubSeasonsQuarterly	= 1,
	eHgClubSeasonsMonthly	= 2,
};

enum HomeGamesClubFlags
{
	eHgClubFlagStandingsEnabled	= 0x00000001,
	eHgClubFlagStatsEnabled		= 0x00000002,
	eHgClubFlagImageApproved	= 0x00000004,
	eHgClubFlagNameApproved		= 0x00000008,
	eHgClubFlagImageDisapproved	= 0x00000010,
	eHgClubFlagNameDisapproved	= 0x00000020,
	eHgClubFlagShowFounderName  = 0x00000040,
	eHgClubFlagHeaderApproved	= 0x00000080,
	eHgClubFlagHeaderDisapproved= 0x00000100,
	eHgClubFlagPublicClub		= 0x00000200,
	eHgClubFlagVoiceEnabled		= 0x00000400,
	eHgClubFlagArchived			= 0x00000800,		//	PYR-24340
};

//////////////////////////////////////////////////////////////////////////

enum HomeGamesMembershipStatus
{
	eHgMembershipStatusApproved	= 0,
	eHgMembershipStatusApplied	= 1,
	eHgMembershipStatusRejected	= 2,
	eHgMembershipStatusBlocked	= 3,
	eHgMembershipStatusSuspended= 4,
	eHgMembershipStatusResigned	= 5,
	eHgMembershipStatusGuest	= 6,
	eHgMembershipStatusBanned   = 7,
	// add all states before here. the eHgMembershipStatusLast
	// is used by int2MembershipStatus as a marker to determine
	// the last valid integer value
	eHgMembershipStatusLastAndInvalid,
	// 'did not exist' status - this is not a valid status and its 
	// purpose is to be used for the audit, when a membership is created
	// it should never be used by the client code and int2MembershipStatus
	// does not map it at any valid number
	//
	eHgMembershipStatusDidNotExist   = 0xFFFF,

};

enum HgMemberRole
{
	eHrMemberRoleMember			= 0,
	eHrMemberRoleFounder		= 1,
	eHrMemberRoleAdmin			= 2,
	// fake flags begin. 
	// these flags are not stored in the database. 
	// they're used to communicate special information to the client. 
	eHrMemberRoleShadowFounder	= 3,	
	eHrMemberRoleObserver		= 4,
	// fake flags 5,6,7 had to be added to basically pass 
	// HomeGamesMembershipStatus information inside HgMemberRole
	// of the HgClubInfo object. This is not something to be proud of
	// but had to be done under the circumstances.
	eHrMemberRoleApplied        = 5,    // PYR-17781 
	eHrMemberRoleGuest          = 6,    // PYR-17781
	eHrMemberRoleSuspended      = 7,    // PYR-17781
	// 	fake flags end
};

inline bool isHomeClubManager( const UINT32 memberRole )
{
	return	(	eHrMemberRoleFounder		== memberRole	||
				eHrMemberRoleShadowFounder	== memberRole	);
}

inline bool isHomeClubAdmin( const UINT32 memberRole )
{
	return	(	eHrMemberRoleFounder		== memberRole	||
				eHrMemberRoleShadowFounder	== memberRole	||
				eHrMemberRoleAdmin			== memberRole	);
}

inline bool isHomeClubObserver( const UINT32 memberRole )
{
	return (eHrMemberRoleObserver == memberRole);
}

inline bool isHomeClubApplicant( const UINT32 memberRole )
{
	return (eHrMemberRoleApplied == memberRole);
}

inline int homeClubStatusOrder( UINT32 memberStatus )
{
	switch( memberStatus )
	{
	case eHgMembershipStatusApproved:
		return 0;

	case eHgMembershipStatusGuest:
		return 1;
	}

	return -1;
}

enum HgMemberFlags
{
	HgMemberFlagIgnoreClubEmails		= 0x00000001,
	HgMemberFlagFakeIgnoreAllClubEmails	= 0x00000002,
	HgMemberFlagFakeLiveSupportEnabled  = 0x00000004,
	HgMemberFlagFavoriteClub            = 0x00000008,    // PYR-17781
	HgMemberFlagRemovedApplication      = 0x00000010,    // PYR-17781
	HgMemberFlagNoActivityNotification	= 0x00000020,	 // PYR-18183
	HgMemberFlagMute					= 0x00000040,	 // PYR-18198
	HgMemberFlagShowFullName			= 0x00000080,	 // PYR-18154
	HgMemberFlagOnlineIfCheckedIn		= 0x00000100,	 // PYR-18154

	ENUM_LAST_VALUE(HgMemberFlags) // defines HgMemberFlags_Last & HgMemberFlags_NextAfterLast
};

// PYR-17781 - use for not setting these flags in DB
const UINT32 HgMemberFakeFlags = ( HgMemberFlagFakeIgnoreAllClubEmails | HgMemberFlagFakeLiveSupportEnabled );

enum HgTournamentFormat
{
	eHgTournFormatRegular	= 0,
	eHgTournFormatRebuy		= 1,
	eHgTournFormatShootout	= 2,
	eHgTournFormatKnockout	= 3,
	eHgTournFormatRebuy1R1A = 4,	// PYR-18533, 1 rebuy and 1 add-on
	eHgTournFormatRebuy2R1A = 5,	// PYR-18533, 2 rebuys with 1 add-on
	eHgTournFormatRebuy2RnoA = 6,	// PYR-18533, 2 rebuys with no add-on
};

enum HgFavTournFlags
{
	eHgFavTournFlagsNone			= 0,
	eHgFavTournFlagsVoiceEnabled	= 1,
};

//////////////////////////////////////////////////////////////////////////
class HgPrintVector
{
public:
	template<typename T>
	static void printNumVector(const vector<T>& v, const char* descr = "")
	{
		PString strOut;
		for(int i = 0; i < v.size(); ++i)
		{
			if( is_signed<T>::value )
			{
				strOut.appendInt64((INT64)v[i]).append(",");
			}
			else
			{
				strOut.appendUint64((UINT64)v[i]).append(",");
			}
		}
		PLog("%s: %s", descr, strOut.c_str());
	}
	static void printStrVector(const vector<PString>& v, const char* descr = "")
	{
		PString strOut;
		for(int i = 0; i < v.size(); ++i)
		{
			strOut.append(v[i]).append(",");
		}
		PLog("%s: %s", descr, strOut.c_str());
	}
	template <class T>
	static void printPrintableVector(const vector<T>& v, const char* descr = "")
	{
		PLog("%s", descr);
		for(int i = 0; i < v.size(); ++i)
		{
			v[i].print();
		}
	}
};
//////////////////////////////////////////////////////////////////////////

class HgUint32Pair
{
public:
	UINT32 first;
	UINT32 second;
public:
	HgUint32Pair() : first(0), second(0)
	{
	}
	HgUint32Pair(const UINT32 first_, const UINT32 second_) : first(first_), second(second_)
	{
	}
	HgUint32Pair( CommMsgParser& parser)
	{
		parse(parser);
	}
	void compose(CommMsgBody& body) const
	{
		body.composeUINT32(first)
			.composeUINT32(second);
	}
	void parse(CommMsgParser& parser)
	{
		parser.parseUINT32(first)
			.parseUINT32(second);
	}
	void print() const
	{
		PLog("HgUint32Pair< first=%u second=%u >", first, second);
	}
};
//////////////////////////////////////////////////////////////////////////
//  PYR-40177
class HgUint64Pair
{
public:
	UINT64 first;
	UINT64 second;
public:
	HgUint64Pair() : first(0), second(0)
	{
	}
	HgUint64Pair(const UINT64 first_, const UINT64 second_) : first(first_), second(second_)
	{
	}
	HgUint64Pair( CommMsgParser& parser)
	{
		parse(parser);
	}
	void compose(CommMsgBody& body) const
	{
		body.composeUINT64(first)
			.composeUINT64(second);
	}
	void parse(CommMsgParser& parser)
	{
		parser.parseUINT64(first)
			.parseUINT64(second);
	}
	void print() const
	{
		PLog("HgU64 %" PRIu64 "/%" PRIu64 "", first, second);
	}
};

//////////////////////////////////////////////////////////////////////////
// 	PYR-17810
class HgGlobalClubPublicationData
{
	UINT32 clubId;
	PString clubName;
	UINT32 numMembers;
	UINT32 ordinal;
	PSiteMask visibilityMask;
	UINT32 clubNameColor;

public:
	HgGlobalClubPublicationData(UINT32 clubId_, 
								const char* clubName_, 
								const UINT32 numMembers_, 
								const UINT32 ordinal_,
								const PSiteMask& visibilityMask_,
								UINT32 clubNameColor_)
		:
		clubId(clubId_),
		clubName(clubName_),
		numMembers(numMembers_),
		ordinal(ordinal_),
		visibilityMask( visibilityMask_ ),
		clubNameColor(clubNameColor_)
	{
	}
	HgGlobalClubPublicationData(CommMsgParser& parser)
	{
		parse(parser);
	}
	void compose(CommMsgBody& body) const
	{
		body.composeUINT32(clubId)
			.composeUINT32(numMembers)
			.composeUINT32(ordinal)
			.composeString(clubName)
			.composeUINT32(visibilityMask.getLower32())
			.composeUINT32(clubNameColor)
			.composeObject( visibilityMask )
			;
	}
	void parse(CommMsgParser& parser)
	{
		UINT32 visibilityMask32;
		parser.parseUINT32(clubId)
			  .parseUINT32(numMembers)
			  .parseUINT32(ordinal)
			  .parseStringP(clubName)
			  .parseUINT32(visibilityMask32)
			  .parseUINT32(clubNameColor);
		if (parser.parseEnded()) return;
		parser.parseObject(visibilityMask)
			;
	}
	const UINT32	getClubId()				const { return clubId; }
	const PString	getClubName()			const { return clubName; }
	const UINT32	getNumMembers()			const { return numMembers; }
	const UINT32	getOrdinal()			const { return ordinal; }
	const UINT32	siteVisibilityMask()	const { return visibilityMask.getLower32(); }
	const UINT32	getClubNameColor()		const { return clubNameColor; }
	const PSiteMask&	siteVisibilityMaskEx()	const { return visibilityMask; }

	void print() const
	{
		PString hexStr;
		PTRACE5("HGGPD: c=%u n='%s' nm=%u o=%u v=0x%s clr=%x", clubId, clubName.c_str(), numMembers, ordinal, visibilityMask.toDbString(hexStr), clubNameColor);
	}
};
//////////////////////////////////////////////////////////////////////////

struct HgClubInfo
{
	UINT32 clubId;
	PString clubName;
	UINT32 nbMembers;
	UINT32 roleForUser;
	UINT32 memberFlags;
	UINT32 ordinal;       // PYR-16627 - Showcase clubs
	bool favoriteClub;    // PYR-17781
	static const UINT32 noColor = 0x80000000;
	UINT32 clubNameColor; // PYR-20121 - how club name appears in the home games main lobby

	HgClubInfo()
	{
		clear();
	}
	HgClubInfo( UINT32 clubId_, const char *clubName_, 
		UINT32 nbMembers_, UINT32 roleForUser_, UINT32 memberFlags_, UINT32 ordinal_, bool favoriteClub_, UINT32 clubNameColor_ )
	{
		clubId = clubId_;
		clubName = clubName_;
		nbMembers = nbMembers_;
		roleForUser = roleForUser_;
		memberFlags = memberFlags_;
		ordinal = ordinal_;
		favoriteClub = favoriteClub_;
		clubNameColor = clubNameColor_;
	}

	void clear()
	{
		clubId			= 0;
		clubName		= "";
		nbMembers		= 0;
		roleForUser		= 0;
		memberFlags		= 0;
		ordinal			= 0;
		favoriteClub	= false;
		clubNameColor	= noColor;
	}

	void parse( CommMsgParser& parser )
	{
		parser.parseUINT32( clubId )
			  .parseStringP( clubName )
			  .parseUINT32( nbMembers )
			  .parseUINT32( roleForUser )
			  .parseUINT32( memberFlags )
			  .parseUINT32( ordinal )		// PYR-16627 - Showcase clubs
			  .parseBOOL( favoriteClub );	// PYR-17781;

		// PYR-20121 TODO: remove after Jan 2012
		if( !parser.parseEnded())
		{
			parser.parseUINT32( clubNameColor ); // PYR-20121
		}
		else
		{
			clubNameColor = noColor;
		}
	}
	void compose( CommMsgBody& body ) const
	{
		body.composeUINT32( clubId )
			.composeString( clubName )
			.composeUINT32( nbMembers )
			.composeUINT32( roleForUser )
			.composeUINT32( memberFlags )
			.composeUINT32( ordinal )
			.composeBOOL( favoriteClub )
			.composeUINT32( clubNameColor )
			;
	}

	void print() const
	{
		PTRACE5( "ClubId=%u, name=%s, Number of members=%u, Role=%u, Membership flags=%u, Ordinal=%u, FavoriteClub=%u nameColor=%x", 
			clubId, clubName.c_str(), nbMembers, roleForUser, memberFlags, ordinal, favoriteClub ? 1 : 0, clubNameColor );
	}
};

/////////////////////////////////////////////////////////////////////////////

struct HgClubInfoList
{
	vector<HgClubInfo> clubs;

	HgClubInfoList()
	{
		clear();
	}
	HgClubInfoList( CommMsgParser& parser )
	{
		parse( parser );
	}

	void clear()
	{
		clubs.clear();
	}

	void parse( CommMsgParser& parser )
	{
		parseVectorOfParsableMessageBodies( clubs, parser );
	}
	void compose( CommMsgBody& body ) const
	{
		composeVectorOfMessageBodiesOfComposableValues( clubs, body );
	}

	size_t size() const
	{
		return clubs.size();
	}
	const HgClubInfo& operator[]( size_t i ) const
	{
		return clubs[ i ];
	}

	void print() const
	{
		for( vector<HgClubInfo>::const_iterator c = clubs.begin(); c != clubs.end(); ++c )
		{
			PTRACE5("");
			c->print();
		}
	}
};

//////////////////////////////////////////////////////////////////////////

class HgRingStake
{
public:
	UINT32 low;
	UINT32 high;
public:
	HgRingStake() : low(0), high(0)
	{
	}
	HgRingStake(const UINT32 low_, const UINT32 high_) : low(low_), high(high_)
	{
	}
	HgRingStake( CommMsgParser& parser)
	{
		parse(parser);
	}
	void compose(CommMsgBody& body) const
	{
		body.composeUINT32(low)
			.composeUINT32(high)
			;
	}
	void parse(CommMsgParser& parser)
	{
		parser.parseUINT32(low)
			.parseUINT32(high)
			;
	}
	void print() const
	{
		PLog("HgRingStake< low=%u high=%u>", low, high);
	}

};
//////////////////////////////////////////////////////////////////////////
class HgTournBuyIn
{
public:
	UINT32 buyIn;
	UINT32 rake;
	UINT32 bounty;
	INT32  scalePM;	// PYR-40177
public:
	HgTournBuyIn() : buyIn(0), rake(0), bounty(0), scalePM(1)
	{
	}
	virtual ~HgTournBuyIn()
	{
	}
	HgTournBuyIn(	const UINT32 buyIn_, 
					const UINT32 rake_, 
					const UINT32 bounty_,
					const INT32  scalePM_)
					: 
					buyIn(buyIn_), 
					rake(rake_), 
					bounty(bounty_),
					scalePM(scalePM_)
	{
	}
	HgTournBuyIn( CommMsgParser& parser, const INT32 verNum )
	{
		parse(parser, verNum);
	}
	INT64 getExpandedTo64( INT32 val32 ) const
	{
		if( scalePM != 1 )
		{
			return PlayChipsScaler::getExpandedTo64( val32, scalePM );
		}
		return static_cast< INT64 >( val32 );
	}
	void compose(CommMsgBody& body, const INT32 verNum) const
	{
		// use of throw agreed with AlexN 16Jan2017. 
		if( verNum != HG_CLI_VER_1 && verNum != HG_CLI_VER_2)
		{
			throw PError(PString("Faulty logic. Do not understand verNum: ").appendInt(verNum));
		}

		body.composeUINT32(buyIn)
			.composeUINT32(rake)
			.composeUINT32(bounty);

		if( verNum == HG_CLI_VER_2 )
		{
			body.composeINT32(scalePM);
		}
	}
	void parse(CommMsgParser& parser, const INT32 verNum)
	{
		// use of throw agreed with AlexN 16Jan2017. 
		if( verNum != HG_CLI_VER_1 && verNum != HG_CLI_VER_2)
		{
			throw PError(PString("Faulty logic. Do not understand verNum: ").appendInt(verNum));
		}

		parser.parseUINT32(buyIn)
			.parseUINT32(rake)
			.parseUINT32(bounty);

		if( verNum == HG_CLI_VER_2 )
		{
			parser.parseINT32(scalePM);
		}
		else
		{
			scalePM = 1;
		}
	}
	void print() const
	{
		PLog("HgTournBuyIn< buyIn=%u rake=%u bounty=%u scalePM=%d>", buyIn, rake, bounty, scalePM);
	}
};
//////////////////////////////////////////////////////////////////////////
class HgGameParams
{
public:
	BYTE			handType;
	bool			isHiLo;
	BYTE			potStruct;
	UINT32			tableSize;
	bool			isPlayMoney;
	PString			currency;
	bool			isFastDealing;

	HgGameParams()
		:	handType(0),
			isHiLo(false),
			potStruct(0),
			tableSize(0),
			isPlayMoney(false),
			currency(""),
			isFastDealing(false)
	{
	}
	virtual ~HgGameParams()
	{
	}
	INT64 getExpandedTo64( INT32 val32, INT32 scalePM ) const
	{
		if( isPlayMoney && scalePM != 1 )
		{
			return PlayChipsScaler::getExpandedTo64( val32, scalePM );
		}
		return static_cast< INT64 >( val32 );
	}
	void compose(CommMsgBody& body) const
	{

		body.composeBOOL(isHiLo)
			.composeUINT32(tableSize)
			.composeBOOL(isFastDealing)
			.composeBOOL(isPlayMoney)
			.composeString(currency)
			.composeBYTE(handType)
			.composeBYTE(potStruct)
			;
	}
	void parse(CommMsgParser& parser)
	{
		parser.parseBOOL(isHiLo)
			.parseUINT32(tableSize)
			.parseBOOL(isFastDealing)
			.parseBOOL(isPlayMoney)
			.parseStringP(currency)
			.parseBYTE(handType)
			.parseBYTE(potStruct)
			;
	}
	void print() const
	{
		PLog("HgGameParams<hand=%u hl=%u pot=%u numpl=%u money=%s curr=%s fast=%u>", 
			 (UINT32)handType, 
			 (UINT32)isHiLo, 
			 (UINT32)potStruct, 
			 (UINT32)tableSize,
			 isPlayMoney ? "PM" : "RM",
			 currency.c_str(),
			 (UINT32)isFastDealing);
	}
};
//////////////////////////////////////////////////////////////////////////
class HgTableParams : public HgGameParams
{
public:
	UINT32			minBuyIn;
	UINT32			maxBuyIn;
	UINT32			cap;
	UINT32			stakesLow;
	UINT32			stakesHigh;
	UINT32			ante;
	PString			tableName;
	INT32			scalePM;	// PYR-40177

	HgTableParams()
		:	minBuyIn(0),
			maxBuyIn(0),
			cap(0),
			stakesLow(0),
			stakesHigh(0),
			ante(0),
			scalePM(1)			// PYR-40177
	{
	}
	HgTableParams(CommMsgParser& parser)
	{
		parse(parser);
	}
	void compose(CommMsgBody& body, const char* tableNameStr = NULL) const
	{
		HgGameParams::compose(body);

		body.composeUINT32(ante)
			.composeUINT32(cap)
			.composeUINT32(minBuyIn)
			.composeUINT32(maxBuyIn)
			.composeUINT32(stakesLow)
			.composeUINT32(stakesHigh)
			.composeString(tableNameStr ? tableNameStr : tableName.c_str())
			.composeINT32(scalePM)	// PYR-40177
			;
	}
	void parse(CommMsgParser& parser)
	{
		HgGameParams::parse(parser);

		parser.parseUINT32(ante)
			.parseUINT32(cap)
			.parseUINT32(minBuyIn)
			.parseUINT32(maxBuyIn)
			.parseUINT32(stakesLow)
			.parseUINT32(stakesHigh)
			.parseStringP(tableName)
			;

		if( !parser.parseEnded() )	// PYR-40177
		{
			parser.parseINT32(scalePM);
		}
	}
	void print() const
	{
		PLog("HgTableParams<");
		HgGameParams::print();
		PLog(	"ante=%u cap=%u sLow=%u sHigh=%u minBuyIn=%u maxBuyIn=%u tblName='%s' scalePM=%d", 
				ante, cap, stakesLow, stakesHigh, minBuyIn, maxBuyIn, tableName.c_str(), scalePM);
		PLog(">HgTableParams");
	}
};

class HgTableParamsVariations
{
public:
	vector<HgTableParams>	params;
	bool					isVoiceEnabled;
	
	HgTableParamsVariations() : isVoiceEnabled(false)
	{
	}
	HgTableParamsVariations(CommMsgParser& parser)
	{
		parse(parser);
	}
	HgTableParamsVariations( HgTableParamsVariations& other, bool ) // moving constructor
	{
		moveFrom( other );
	}
	void moveFrom( HgTableParamsVariations& other )
	{
		params.swap( other.params );
		isVoiceEnabled = other.isVoiceEnabled;
	}
	void compose(CommMsgBody& body) const
	{
		composeVectorOfMessageBodiesOfComposableValues(params, body);
		
		body.composeBOOL(isVoiceEnabled);
	}
	void parse(CommMsgParser& parser)
	{
		parseVectorOfParsableMessageBodies(params, parser);
		
		parser.parseBOOL(isVoiceEnabled);
	}
	void print() const
	{
		PLog("HgTableParamsVariations< size=%u %s", params.size(), isVoiceEnabled ? "voice" : "no voice");
		HgPrintVector::printPrintableVector(params);
		PLog(">HgTableParamsVariations");
	}
};
//////////////////////////////////////////////////////////////////////////
class HgTournParams : public HgGameParams, public HgTournBuyIn
{
public:

	// tourn specific stuff
	bool			isSyncBreaksOn;
	UINT32			tournFormat;
	UINT32			payout;
	UINT32			startingChips;
	UINT32			levelLength;
	UINT32			tournKnockoutPrize;
	UINT32			tournMaxPlayers;

	UINT16			year;
	BYTE			month;
	BYTE			day;
	BYTE			hour;
	BYTE			minute;

	HgTournParams() :	isSyncBreaksOn(false),
						tournFormat(eHgTournFormatRegular),
						payout(0),
						startingChips(0),
						levelLength(0),
						tournKnockoutPrize(0),
						tournMaxPlayers(0),
						year(0),
						month(0),
						day(0),
						hour(0),
						minute(0)
	{
	}
	HgTournParams( CommMsgParser& parser, const INT32 verNum)	// PYR-40177 Now versioned.
	{
		parse(parser, verNum);
	}
	void compose(CommMsgBody& body, const INT32 verNum) const	// PYR-40177 Now versioned.
	{
		// use of throw agreed with AlexN 16Jan2017. 
		if( verNum != HG_CLI_VER_1 && verNum != HG_CLI_VER_2)
		{
			throw PError(PString("Faulty logic. Do not understand verNum: ").appendInt(verNum));
		}

		HgGameParams::compose(body);

		body.composeUINT32	(buyIn				)
			.composeBOOL	(isSyncBreaksOn		)
			.composeUINT32	(tournFormat		)
			.composeUINT32	(payout				)
			.composeUINT32	(startingChips		)
			.composeUINT32	(levelLength		)
			.composeUINT32	(tournKnockoutPrize	)
			.composeUINT32	(tournMaxPlayers	)
			.composeUINT16	(year				)
			.composeBYTE	(month				)
			.composeBYTE	(day				)
			.composeBYTE	(hour				)
			.composeBYTE	(minute				)
			;

		if( verNum == HG_CLI_VER_2 )
		{
			body.composeINT32(scalePM);
		}
	}
	void parse(CommMsgParser& parser, const INT32 verNum)
	{
		// use of throw agreed with AlexN 16Jan2017. 
		if( verNum != HG_CLI_VER_1 && verNum != HG_CLI_VER_2)
		{
			throw PError(PString("Faulty logic. Do not understand verNum: ").appendInt(verNum));
		}

		HgGameParams::parse(parser);

		parser.parseUINT32	(buyIn				)
			.parseBOOL		(isSyncBreaksOn		)
			.parseUINT32	(tournFormat		)
			.parseUINT32	(payout				)
			.parseUINT32	(startingChips		)
			.parseUINT32	(levelLength		)
			.parseUINT32	(tournKnockoutPrize	)
			.parseUINT32	(tournMaxPlayers	)
			.parseUINT16	(year				)
			.parseBYTE		(month				)
			.parseBYTE		(day				)
			.parseBYTE		(hour				)
			.parseBYTE		(minute				)
			;

		if( verNum == HG_CLI_VER_2 )
		{
			parser.parseINT32(scalePM);	
		}
		else
		{
			scalePM = 1;
		}
	}
	void print() const
	{
		PLog( "HgTournParams< "
			" buyIn=%d"	
			" isSyncBreaksOn=%d"	
			" tournFormat=%d"	
			" payout=%d"	
			" startingChips=%d"	
			" levelLength=%d"
			" tournKnockoutPrize=%d"	
			" tournMaxPlayers=%d"
			" year=%d"
			" month=%d"	
			" day=%d"	
			" hour=%d"	
			" minute=%d"
			" scalePM=%d >"
			, buyIn				
			, isSyncBreaksOn		
			, tournFormat		
			, payout				
			, startingChips		
			, levelLength		
			, tournKnockoutPrize	
			, tournMaxPlayers	
			, year				
			, month				
			, day				
			, hour				
			, minute
			, scalePM
			);
		HgGameParams::print();
		
		// we do not compose parse full structure so only buyIn will have a correct value the rest will be 0
		HgTournBuyIn::print();
	}
};
//////////////////////////////////////////////////////////////////////////

class HgCreateTournParams : public HgTournParams
{
public:
	UINT32	clubId;
	PString	tournamentLobbyText;
	PString gameName;
	bool	countTowardsClubStandings;

	HgCreateTournParams() :	clubId(0),
							tournamentLobbyText(""),
							gameName(""),
							countTowardsClubStandings(false)
	{
	}
	HgCreateTournParams(CommMsgParser& parser, const INT32 verNum)	// PYR-40177 Now versioned.
	{
		parse(parser, verNum);
	}
	void compose(CommMsgBody& body, const INT32 verNum) const		// PYR-40177 Now versioned.
	{
		body.composeUINT32(clubId)
			.composeString(tournamentLobbyText)
			.composeString(gameName)
			.composeBOOL(countTowardsClubStandings)
			;
		HgTournParams::compose(body, verNum);

	}
	void parse(CommMsgParser& parser, const INT32 verNum)			// PYR-40177 Now versioned.
	{
		parser.parseUINT32(clubId)
			  .parseStringP(tournamentLobbyText)
			  .parseStringP(gameName)
			  .parseBOOL(countTowardsClubStandings)
			  ;
		HgTournParams::parse(parser, verNum);
	}
	virtual bool isVoiceEnabled() const
	{
		return false;
	}
};

class HgCreateTournParams2 : public HgCreateTournParams
{
public:
	bool voiceEnabled;

	HgCreateTournParams2() : voiceEnabled(false)
	{
	}
	HgCreateTournParams2(CommMsgParser& parser, const INT32 verNum)
	{
		parse(parser, verNum);
	}
	HgCreateTournParams2( const HgCreateTournParams& v1, bool voiceEnabled_ ) 
		: HgCreateTournParams( v1 ), voiceEnabled(voiceEnabled_)
	{
	}
	void compose(CommMsgBody& body, const INT32 verNum) const	// PYR-40177 Now versioned.
	{
		HgCreateTournParams::compose(body, verNum);
		body.composeBOOL(voiceEnabled);
	}
	void parse(CommMsgParser& parser, const INT32 verNum)		// PYR-40177 Now versioned.
	{
		HgCreateTournParams::parse(parser, verNum);
		parser.parseBOOL(voiceEnabled);
	}
	virtual bool isVoiceEnabled() const
	{
		return voiceEnabled;
	}
};

class HgCreateTableParams : public HgTableParams
{
public:
	UINT32	clubId;
	bool	voiceEnabled;
	
	HgCreateTableParams() :	clubId(0), voiceEnabled(false)
	{
	}
	HgCreateTableParams(CommMsgParser& parser)
	{
		parse(parser);
	}
	void compose(CommMsgBody& body, const char* tableNameStr = NULL) const
	{
		body.composeUINT32(clubId);

		HgTableParams::compose(body, tableNameStr);

		body.composeBOOL(voiceEnabled);
	}
	void parse(CommMsgParser& parser)
	{
		parser.parseUINT32(clubId);

		HgTableParams::parse(parser);

		if( !parser.parseEnded() )
		{
			parser.parseBOOL( voiceEnabled );
		}
	}
};

//////////////////////////////////////////////////////////////////////////
class HgGameStructure
{
public:
	BYTE				handType;
	bool				isHiLo;
	vector<BYTE>		potTypes;
	vector<UINT32>		tableSizes;
	PString				description;
	vector<bool>		moneyTypes;		// ( false = real, true = play)*		// script - PlayMoney yes/no
	UINT32				siteRmTableVisibilityMask;
	PSiteMask			siteRmTableVisibilityMaskEx;

public:
	HgGameStructure( const BYTE handType_ = HandTypeNone ) 
		: handType( handType_ ), isHiLo( false ), siteRmTableVisibilityMask( 0 ) {}

	HgGameStructure( CommMsgParser& parser) { parse(parser); }

	void compose(CommMsgBody& body) const
	{
		body.composeBYTE(handType)
			.composeBOOL(isHiLo)
			.composeString(description)
			;

		composeVectorOrList(body, potTypes);
		composeVectorOrList(body, tableSizes);
		composeVectorOrList(body, moneyTypes);

		body.composeUINT32(siteRmTableVisibilityMaskEx.getLower32())
			.composeObject(siteRmTableVisibilityMaskEx)
			;
	}

	void parse(CommMsgParser& parser)
	{
		parser.parseBYTE(handType)
			  .parseBOOL(isHiLo)
			  .parseStringP(description)
			  ;

		parseVectorOrList(parser, potTypes);
		parseVectorOrList(parser, tableSizes);
		parseVectorOrList(parser, moneyTypes);

		parser.parseUINT32(siteRmTableVisibilityMask)
			  .parseObject(siteRmTableVisibilityMaskEx)
			  ;
	}

	void print() const
	{
		PLog("HgGameStructure<\r\n %d %s - %s", 
			 handType, isHiLo ? "hi/lo" : "", description.c_str());

		HgPrintVector::printNumVector(potTypes,		"potTypes");
		HgPrintVector::printNumVector(tableSizes,	"tableSizes");
		HgPrintVector::printNumVector(moneyTypes,	"moneyTypes");
		PLog(">HgGameStructure");
	}
};
//////////////////////////////////////////////////////////////////////////
class HgTournGameStructure : public HgGameStructure
{
};

//////////////////////////////////////////////////////////////////////////

class HgTournCurrencyBuyIns
{
public:
	PString currency;
	vector<HgTournBuyIn> tournBuyIns;
	HgTournCurrencyBuyIns(){}
	HgTournCurrencyBuyIns(const char* currency_) : currency(currency_)
	{
	}
	void compose(CommMsgBody& body, const INT32 verNum) const	// PYR-40177 Now versioned.
	{
		// use of throw agreed with AlexN 16Jan2017. 
		if( verNum != HG_CLI_VER_1 && verNum != HG_CLI_VER_2)
		{
			throw PError(PString("Faulty logic. Do not understand verNum: ").appendInt(verNum));
		}

		body.composeString(currency)
			;

		// PYR-40177 For version 2 including scalePMs we use versioned compose with message bodies.
		if( verNum == HG_CLI_VER_2 )
		{
			composeVectorOfMessageBodiesOfComposableValuesVer(tournBuyIns, body, verNum);
		}
		else
		{
			composeVectorOfComposableValuesVer(tournBuyIns, body, verNum);
		}
	}
	void parse(CommMsgParser& parser, const INT32 verNum)		// PYR-40177 Now versioned.
	{
		// use of throw agreed with AlexN 16Jan2017. 
		if( verNum != HG_CLI_VER_1 && verNum != HG_CLI_VER_2)
		{
			throw PError(PString("Faulty logic. Do not understand verNum: ").appendInt(verNum));
		}

		parser.parseStringP(currency)
			;

		// PYR-40177 For version 2 including scalePMs we use versioned parse with message bodies.
		if( verNum == HG_CLI_VER_2 )
		{
			parseVectorOfParsableMessageBodiesVer(tournBuyIns, parser, verNum);
		}
		else
		{
			parseVectorOfParsableValuesVer(tournBuyIns, parser, verNum);
		}
	}
	void print() const
	{
		PLog("HgTournCurrencyBuyIns<\r\n - currency=%s", currency.c_str());

		HgPrintVector::printPrintableVector( tournBuyIns, "tournBuyIns");

		PLog(">HgTournCurrencyBuyIns");
	}
};

class HgTournStructureLimits
{
public:
	UINT32					tournKnockoutPrize;		// script - Knockout (it's a value if not 0)
	UINT32					tournMaxPlayers;		// script - MaxPlayersPerTable
	vector<HgTournBuyIn>	tournBuyInsPM;			// from global club props via category
	vector<bool>			tournFastDealing;		// (normal,fast)*	// script - FastDealing yes/no (not using delay parameters)
	vector<bool>			tournSyncBreaks;		// (off,on)			// script - SyncBreaks yes/no
	vector<UINT32>			tournPayouts;			// (top15,top10,top20,winnerTakeAll)
	vector<UINT32>			tournStartingChips;		// (150000,100000,200000,300000,500000) // script - InitialChips
	vector<UINT32>			tournLevelLengths;		// (10,3,5,15,20)	// script - Level
	vector<UINT32>			tournFormats;			// regular[,rebuy][,shootout][,knockout]
	vector<HgTournGameStructure>	gameStructures;
	vector<HgTournCurrencyBuyIns> currencyBuyIns;
	bool					isVoiceEnabled;
	
public:
	HgTournStructureLimits() 
		: tournKnockoutPrize(0), tournMaxPlayers(0), isVoiceEnabled(false)
	{
	}
	HgTournStructureLimits(CommMsgParser& parser, const INT32 verNum)	// PYR-40177 Now versioned.
		: tournKnockoutPrize(0), tournMaxPlayers(0), isVoiceEnabled(false)
	{
		parse(parser, verNum);
	}
	HgTournStructureLimits( HgTournStructureLimits& other, bool ) // moving constructor
		: tournKnockoutPrize(0), tournMaxPlayers(0), isVoiceEnabled(false)
	{
		moveFrom( other );
	}
	void moveFrom( HgTournStructureLimits& other )
	{
		tournKnockoutPrize = other.tournKnockoutPrize;
		tournMaxPlayers = other.tournMaxPlayers;

		tournBuyInsPM.swap( other.tournBuyInsPM );
		tournFastDealing.swap( other.tournFastDealing );
		tournSyncBreaks.swap( other.tournSyncBreaks );
		tournPayouts.swap( other.tournPayouts );
		tournStartingChips.swap( other.tournStartingChips );
		tournLevelLengths.swap( other.tournLevelLengths );
		tournFormats.swap( other.tournFormats );
		currencyBuyIns.swap( other.currencyBuyIns );

		gameStructures.swap( other.gameStructures );

		isVoiceEnabled = other.isVoiceEnabled;
	}
	void compose(CommMsgBody& body, const INT32 verNum) const	// PYR-40177 Now versioned.
	{
		// use of throw agreed with AlexN 16Jan2017. 
		if( verNum != HG_CLI_VER_1 && verNum != HG_CLI_VER_2)
		{
			throw PError(PString("Faulty logic. Do not understand verNum: ").appendInt(verNum));
		}

		body.composeUINT32(tournKnockoutPrize)
			.composeUINT32(tournMaxPlayers);

		composeVector(body, tournFastDealing,		ComposerBOOL());
		composeVector(body, tournSyncBreaks,		ComposerBOOL());
		composeVector(body, tournPayouts,			ComposerUINT32());
		composeVector(body, tournStartingChips,		ComposerUINT32());
		composeVector(body, tournLevelLengths,		ComposerUINT32());
		composeVector(body, tournFormats,			ComposerUINT32());
		
		composeVectorOfMessageBodiesOfComposableValues(gameStructures, body);
		
		// PYR-40177 For version 2 including scalePMs we use versioned compose with message bodies.
		if( verNum == HG_CLI_VER_2 )
		{
			composeVectorOfMessageBodiesOfComposableValuesVer(tournBuyInsPM, body, verNum);
			composeVectorOfMessageBodiesOfComposableValuesVer(currencyBuyIns, body, verNum);
		}
		else
		{
			composeVectorOfComposableValuesVer(tournBuyInsPM, body, verNum);
			composeVectorOfComposableValuesVer(currencyBuyIns, body, verNum);
		}

		body.composeBOOL(isVoiceEnabled);
	}
	void parse(CommMsgParser& parser, const INT32 verNum)	// PYR-40177 Now versioned.
	{
		// use of throw agreed with AlexN 16Jan2017. 
		if( verNum != HG_CLI_VER_1 && verNum != HG_CLI_VER_2)
		{
			throw PError(PString("Faulty logic. Do not understand verNum: ").appendInt(verNum));
		}

		parser.parseUINT32(tournKnockoutPrize)
			.parseUINT32(tournMaxPlayers);
				
		parseVectorWithOverride(parser, tournFastDealing,	ComposerBOOL());
		parseVectorWithOverride(parser, tournSyncBreaks,	ComposerBOOL());
		parseVectorWithOverride(parser, tournPayouts,		ComposerUINT32());
		parseVectorWithOverride(parser, tournStartingChips,	ComposerUINT32());
		parseVectorWithOverride(parser, tournLevelLengths,	ComposerUINT32());
		parseVectorWithOverride(parser, tournFormats,		ComposerUINT32());
		parseVectorOfParsableMessageBodies(gameStructures, parser);

		// PYR-40177 For version 2 including scalePMs we use versioned parse with message bodies.
		if( verNum == HG_CLI_VER_2 )
		{
			parseVectorOfParsableMessageBodiesVer(tournBuyInsPM, parser, verNum);
			parseVectorOfParsableMessageBodiesVer(currencyBuyIns, parser, verNum);
		}
		else
		{
			parseVectorOfParsableValuesVer(tournBuyInsPM, parser, verNum);
			parseVectorOfParsableValuesVer(currencyBuyIns, parser, verNum);
		}

		parser.parseBOOL(isVoiceEnabled);
	}
	const vector< HgTournBuyIn >& tournBuyIn( const char* currency, bool playMoney ) const
	{
		if( playMoney )
			return tournBuyInsPM;

		for( size_t i = 0; i < currencyBuyIns.size(); ++i )
			if( currencyBuyIns[ i ].currency.equals( currency ) )
				return currencyBuyIns[ i ].tournBuyIns;

		PASSERT( 0 );
	}
	void print() const
	{
		PLog("HgTournStructureLimits< \r\ntkoPrize=%u maxPlrs=%u %s", tournKnockoutPrize, tournMaxPlayers, isVoiceEnabled ? "voice" : "no voice" );

		HgPrintVector::printNumVector(tournFastDealing,		"tournFastDealing");
		HgPrintVector::printNumVector(tournSyncBreaks,		"tournSyncBreaks");
		HgPrintVector::printNumVector(tournPayouts,			"tournPayouts");
		HgPrintVector::printNumVector(tournStartingChips,	"tournStartingChips");
		HgPrintVector::printNumVector(tournLevelLengths,	"tournLevelLengths");
		HgPrintVector::printNumVector(tournFormats,			"tournFormats");
		HgPrintVector::printPrintableVector( tournBuyInsPM,	"tournBuyInsPM");
		HgPrintVector::printPrintableVector(gameStructures,	"gameStructures");
		HgPrintVector::printPrintableVector(currencyBuyIns,	"currencyBuyIns");
		PLog(">HgTournStructureLimits");
	}

private: // prohibit default copy constructor & assignment operator
	HgTournStructureLimits( const HgTournStructureLimits& other );
	HgTournStructureLimits& operator=( const HgTournStructureLimits& other );
};
//////////////////////////////////////////////////////////////////////////

class HgClubMemberListItem
{
	PString 					userId;
	PString 					userFullName;
	PString 					userCity;
	UINT32						userAvatar;
	UINT32						flags;
	HomeGamesMembershipStatus	status;
	HgMemberRole				role;
	SrvTime						memberFrom;
	
public:
	HgClubMemberListItem( CommMsgParser& parser )
	{
		parse( parser );
	}
	HgClubMemberListItem(	const char*		userId_,
							const char*		userFullName_,
							const char*		userCity_,
							const UINT32	userAvatar_,
							const UINT32	flags_,
							const SrvTime&	memberFrom_,
							const HomeGamesMembershipStatus status_,
							const HgMemberRole	role_)
							:
							userId( userId_ ),
							userFullName( userFullName_ ),
							userCity( userCity_ ),
							userAvatar( userAvatar_ ),
							flags( flags_ ),
							status( status_ ),
							role(role_),
							memberFrom( memberFrom_ )
	{
	}
	HgClubMemberListItem() : userAvatar( 0 ), flags( 0 ), status( eHgMembershipStatusApproved ), role(eHrMemberRoleMember)
	{
	}
	void compose(CommMsgBody& body) const
	{
		body.composeString(userId)
			.composeString(userFullName)
			.composeString(userCity)
			.composeUINT32(userAvatar)
			.composeUINT32(flags)
			;
			composeEnum32asUINT32(body, status);
			composeEnum32asUINT32(body, role);

		body.composeSrvTime(memberFrom)
			;
	}
	void parse(CommMsgParser& parser)
	{
		parser.parseStringP(userId)
			  .parseStringP(userFullName)
			  .parseStringP(userCity)
			  .parseUINT32(userAvatar)
			  .parseUINT32(flags)
			  ;
		parseEnum32asUINT32(parser, status);
		parseEnum32asUINT32(parser, role);

		parser.parseSrvTime(memberFrom)
			  ;
	}
	const char* getUserId() const { return userId; }
	const char* getUserFullName() const { return userFullName; }
	const char*  getUserCity() const { return userCity; }
	UINT32 getUserAvatar() const { return userAvatar; }
	UINT32 getFlags() const { return flags; }
	HgMemberRole getRole() const { return role; }
	const SrvTime& getMemberFrom() const { return memberFrom; }
	HomeGamesMembershipStatus getStatus() const { return status; }
	void print() const
	{
		PLog(	"MLI< usrId='%s' fn='%s' cty='%s' avt=%u flg=%u sts=%u>", 
				userId.c_str(), userFullName.c_str(), userCity.c_str(), userAvatar, flags, status);
	}
};

//////////////////////////////////////////////////////////////////////////
// clubId, seasonId
class HgClubSeasonUserStats
{
	PString userId;
	PString userCity;
	UINT32 userAvatar;
	UINT32 qualifiedGames;
	UINT32 bestTournPlace;
	UINT32 bestTournPlaceTo;
	UINT32 playersInbestTourn;
	UINT32 worstTournPlace;
	UINT32 worstTournPlaceTo;
	UINT32 playersInWorstTourn;
	UINT32 inTheMoney;
	UINT64 points;
	UINT32 goldFinishes;
	UINT32 silverFinishes;
	UINT32 bronzeFinishes;
	UINT32 averagePlace;
	UINT32 rank;
	UINT32 knockouts;
	SrvTime memberFrom;

public:
	HgClubSeasonUserStats(	const char* userId_, 
							const UINT32 qualifiedGames_,
							const UINT32 bestTournPlace_,
							const UINT32 bestTournPlaceTo_,
							const UINT32 playersInBestTourn_,
							const UINT32 worstTournPlace_,
							const UINT32 worstTournPlaceTo_,
							const UINT32 playersInWorstTourn_,
							const UINT32 inTheMoney_,
							const UINT64 points_,
							const UINT32 goldFinishes_,
							const UINT32 silverFinishes_,
							const UINT32 bronzeFinishes_,
							const UINT32 averagePlace_,
							const UINT32 rank_,
							const UINT32 knockouts_,
							const SrvTime& memberFrom_)
							: 
							userId(userId_),
							userAvatar(0),
							qualifiedGames(qualifiedGames_),
							bestTournPlace(bestTournPlace_),
							bestTournPlaceTo(bestTournPlaceTo_),
							playersInbestTourn(playersInBestTourn_),
							worstTournPlace(worstTournPlace_),
							worstTournPlaceTo(worstTournPlaceTo_),
							playersInWorstTourn(playersInWorstTourn_),
							inTheMoney(inTheMoney_),
							points(points_),
							goldFinishes(goldFinishes_),
							silverFinishes(silverFinishes_),
							bronzeFinishes(bronzeFinishes_),
							averagePlace(averagePlace_),
							rank(rank_),
							knockouts(knockouts_),
							memberFrom(memberFrom_)
	{
	}
	HgClubSeasonUserStats( CommMsgParser& parser)
	{
		parse(parser);
	}
	HgClubSeasonUserStats() :
		  userAvatar			(0)
		, qualifiedGames		(0)
		, bestTournPlace		(0)
		, bestTournPlaceTo		(0)
		, playersInbestTourn	(0)
		, worstTournPlace		(0)
		, worstTournPlaceTo		(0)
		, playersInWorstTourn	(0)
		, inTheMoney			(0)
		, points				(0)
		, goldFinishes			(0)
		, silverFinishes		(0)
		, bronzeFinishes		(0)
		, averagePlace			(0)
		, rank					(0)
		, knockouts				(0)
	{
	}
	void compose(CommMsgBody& body) const
	{
		body.composeString(userId)
			.composeString(userCity)
			.composeUINT32(userAvatar)
			.composeUINT32(qualifiedGames)
			.composeUINT32(bestTournPlace)
			.composeUINT32(bestTournPlaceTo)
			.composeUINT32(playersInbestTourn)
			.composeUINT32(worstTournPlace)
			.composeUINT32(worstTournPlaceTo)
			.composeUINT32(playersInWorstTourn)
			.composeUINT32(inTheMoney)
			.composeUINT64(points)
			.composeUINT32(goldFinishes)
			.composeUINT32(silverFinishes)
			.composeUINT32(bronzeFinishes)
			.composeUINT32(averagePlace)
			.composeUINT32(rank)
			.composeUINT32(knockouts)
			.composeSrvTime(memberFrom)
			;
	}
	void parse(CommMsgParser& parser)
	{
		parser.parseStringP(userId)
			.parseStringP(userCity)
			.parseUINT32(userAvatar)
			.parseUINT32(qualifiedGames)
			.parseUINT32(bestTournPlace)
			.parseUINT32(bestTournPlaceTo)
			.parseUINT32(playersInbestTourn)
			.parseUINT32(worstTournPlace)
			.parseUINT32(worstTournPlaceTo)
			.parseUINT32(playersInWorstTourn)
			.parseUINT32(inTheMoney)
			.parseUINT64(points)
			.parseUINT32(goldFinishes)
			.parseUINT32(silverFinishes)
			.parseUINT32(bronzeFinishes)
			.parseUINT32(averagePlace)
			.parseUINT32(rank)
			.parseUINT32(knockouts)
			.parseSrvTime(memberFrom)
			;
	}
	const char* getUserId()			const { return userId; }
	UINT32 getQualifiedGames()		const { return qualifiedGames; }
	UINT32 getBestTournPlace()		const { return bestTournPlace; }
	UINT32 getBestTournPlaceTo()	const { return bestTournPlaceTo; }
	UINT32 getBestTournPlayers()	const { return playersInbestTourn; }
	UINT32 getWorstTournPlace()		const { return worstTournPlace; }
	UINT32 getWorstTournPlaceTo()	const { return worstTournPlaceTo; }
	UINT32 getWorstTournPlayers()	const { return playersInWorstTourn; }
	UINT32 getInTheMoney()			const { return inTheMoney; }
	UINT64 getPoints()				const { return points; }
	UINT32 getGoldFinishes()		const { return goldFinishes; }
	UINT32 getSilverFinishes()		const { return silverFinishes; }
	UINT32 getBronzeFinishes()		const { return bronzeFinishes; }
	UINT32 getAveragePlace()		const { return averagePlace; }
	UINT32 getRank()				const { return rank; }
	UINT32 getKnockouts()			const { return knockouts; }
	const SrvTime& getMemberFrom()	const { return memberFrom; }

	const char* getUserCity() const { return userCity; }
	void setUserCity(const char* val) { userCity = val; }

	UINT32 getUserAvatar() const { return userAvatar; }
	void setUserAvatar(UINT32 val) { userAvatar = val; }

	void print() const
	{
		PLog(	"SUS< usrId='%s' rnk=%u pts=%u, gms=%u bstTrn=%u-%u/%u wstTrn=%u-%u/%u avg=%u inMny=%u g=%u s=%u b=%u city='%s' uAvtr=%u kkos=%u>", 
				userId.c_str(), rank, (UINT32)points, qualifiedGames, 
				bestTournPlace, bestTournPlaceTo, playersInbestTourn,
				worstTournPlace, worstTournPlaceTo, playersInWorstTourn, 
				averagePlace, inTheMoney, goldFinishes, silverFinishes, bronzeFinishes, userCity.c_str(), userAvatar, knockouts);
	}
};
//////////////////////////////////////////////////////////////////////////

class HgClubSeasonStats
{
public:
	vector<HgClubSeasonUserStats> stats;

	HgClubSeasonStats()
	{
	}
	HgClubSeasonStats(CommMsgParser& parser)
	{
		parse(parser);
	}
	void compose(CommMsgBody& body) const
	{
		composeVectorOfMessageBodiesOfComposableValues(stats, body);
	}
	void parse(CommMsgParser& parser)
	{
		parseVectorOfParsableMessageBodies(stats, parser);
	}
	size_t size() const
	{
		return stats.size();
	}
	const HgClubSeasonUserStats& operator[]( size_t i ) const
	{
		return stats[ i ];
	}
	const bool hasQualifiedGames()
	{
		for(int i = 0; i < stats.size(); ++i)
		{
			if(stats[i].getQualifiedGames() > 0)
				return true;
		}
		return false;
	}
	void print() const
	{
		PLog("HgClubSeasonStats<");
		for(int i = 0; i < stats.size(); ++i)
		{
			stats[i].print();
		}
		PLog(">HgClubSeasonStats");
	}
};
//////////////////////////////////////////////////////////////////////////
class HgClubRecentTournamentSummary
{
	UINT32	tournId;
	PString tournName;
	PString	currency;
	SrvTime	started;
	SrvTime	closed;
	UINT32	numPlayers;
	UINT64	prizePool;
	UINT32	seasonId;
	bool	playMoney;
	UINT32	rebuys;
	UINT32	rebuyRake;
	bool    countingToStats;   // PYR-17842
	INT32	scalePM;           // PYR-40177
public:
	HgClubRecentTournamentSummary() :	tournId(0),
										numPlayers(0), 
										prizePool(0), 
										seasonId(0),
										playMoney(false),
										rebuys(0),
										rebuyRake(0),
										countingToStats(false),
										scalePM(1)						// PYR-40177
	{
	}
	HgClubRecentTournamentSummary(	const UINT32	tournId_,
									const char*		tournName_,
									const SrvTime&	started_,
									const SrvTime&	closed_,
									const UINT32	numPlayers_,
									const UINT64	prizePool_,
									const UINT32	seasonId_,
									const bool		playMoney_,
									const char*		currency_,
									const UINT32	rebuys_,
									const UINT32	rebuyRake_,
									const bool      countingToStats_,
									const INT32		scalePM_)			// PYR-40177 
									:
									tournId(tournId_),
									tournName(tournName_),
									currency(currency_),
									started(started_),
									closed(closed_),
									numPlayers(numPlayers_), 
									prizePool(prizePool_), 
									seasonId(seasonId_),
									playMoney(playMoney_),
									rebuys(rebuys_),
									rebuyRake(rebuyRake_),
									countingToStats(countingToStats_),
									scalePM(scalePM_)					// PYR-40177
	{
	}
	HgClubRecentTournamentSummary(CommMsgParser& parser)
	{
		parse(parser);
	}
	void compose(CommMsgBody& body) const
	{
		body.composeUINT32(tournId)
			.composeString(tournName)
			.composeSrvTime(started)
			.composeSrvTime(closed)
			.composeUINT32(numPlayers)
			.composeUINT64(prizePool)
			.composeUINT32(seasonId)
			.composeBOOL(playMoney)
			.composeString(currency)
			.composeUINT32(rebuys)
			.composeUINT32(rebuyRake)
			.composeBOOL(countingToStats)
			.composeINT32(scalePM)										// PYR-40177
			;
	}
	void parse(CommMsgParser& parser)
	{
		parser.parseUINT32(tournId)
			.parseStringP(tournName)
			.parseSrvTime(started)
			.parseSrvTime(closed)
			.parseUINT32(numPlayers)
			.parseUINT64(prizePool)
			.parseUINT32(seasonId)
			.parseBOOL(playMoney)
		    .parseStringP(currency)
		    .parseUINT32(rebuys)
		    .parseUINT32(rebuyRake);

		if( !parser.parseEnded() )
		{
			parser.parseBOOL(countingToStats);
		}
		else
		{
			countingToStats = false;
		}
		if( !parser.parseEnded() )										// PYR-40177
		{
			parser.parseINT32(scalePM);
		}
		else
		{
			scalePM = 1;
		}
	}
	INT64 getExpandedTo64( INT32 val32 ) const
	{
		if( scalePM != 1 )
		{
			return PlayChipsScaler::getExpandedTo64( val32, scalePM );
		}
		return static_cast< INT64 >( val32 );
	}
	const char* getTournName()		 const { return tournName; }
	const char* getCurrency()		 const { return currency; }
	const SrvTime& getStarted()		 const { return started; }
	const SrvTime& getClosed()		 const { return closed; }
	UINT32	getNumPlayers()			 const { return numPlayers; }
	INT64	getPrizePool()			 const { return getExpandedTo64( static_cast< INT32 >( prizePool ) ); }
	UINT32	getSeasonId()			 const { return seasonId; }
	UINT32	getTournId()			 const { return tournId; }
	UINT32	getRebuys()				 const { return rebuys; }
	UINT32	getRebuyRake()			 const { return rebuyRake; }
	bool	isPlayMoney()			 const { return playMoney; }
	bool    isCountingTowardsStats() const { return countingToStats; }
	INT32	getScalePM()			 const { return scalePM; }			// PYR-40177
	void print() const 
	{
		PString strPrizePool;
		strPrizePool.appendUint64(prizePool);
		PLog("HgClubRecentTournamentSummary< tournId=%u name='%s' %s numPlayers=%u prizePool=%s-%s rebuys=%u rebuyRake=%u scalePM=%d seasonId=%u, countingStats=%u >",
			tournId, tournName.c_str(), playMoney ? "PM" : "RM", numPlayers, strPrizePool.c_str(), currency.c_str(), rebuys, rebuyRake, scalePM, seasonId, countingToStats ? 1 : 0);
	}
};

class HgClubRecentTournamentSummaries
{
public:
	vector<HgClubRecentTournamentSummary> tournSummaries;

	HgClubRecentTournamentSummaries()
	{
	}
	HgClubRecentTournamentSummaries(CommMsgParser& parser)
	{
		parse(parser);
	}
	void compose(CommMsgBody& body) const
	{
		composeVectorOfMessageBodiesOfComposableValues(tournSummaries, body);
	}
	void parse(CommMsgParser& parser)
	{
		parseVectorOfParsableMessageBodies(tournSummaries, parser);
	}
	size_t size() const
	{
		return tournSummaries.size();
	}
	const HgClubRecentTournamentSummary& operator[]( size_t i ) const
	{
		return tournSummaries[ i ];
	}
	void print() const
	{
		PLog("HgClubRecentTournaments<");
		for(int i = 0; i < tournSummaries.size(); ++i)
		{
			tournSummaries[i].print();
		}
		PLog(">HgClubRecentTournaments");
	}
};

//////////////////////////////////////////////////////////////////////////
class HgClubRecentTournPlayerResult
{
	UINT32	tournRank;
	PString	userId;
	UINT32	place;
	UINT32	placeTo;
	UINT32	prize;
	UINT32	pointsEarned;
	UINT32	knockouts;
	INT32   scalePM;												// PYR-40177
public:
	HgClubRecentTournPlayerResult() :	tournRank(0), 
										place(0), 
										placeTo(0), 
										prize(0),
										pointsEarned(0),
										knockouts(0),
										scalePM(1)					// PYR-40177
	{
	}
	HgClubRecentTournPlayerResult(	UINT32	tournRank_,
									const char*	userId_,
									UINT32	place_,
									UINT32	placeTo_,
									UINT32	prize_,
									UINT32	pointsEarned_,
									UINT32	knockouts_,
									INT32   scalePM_)				// PYR-40177 
									: 
									tournRank(tournRank_),
									userId(userId_),
									place(place_), 
									placeTo(placeTo_), 
									prize(prize_),
									pointsEarned(pointsEarned_),
									knockouts(knockouts_),
									scalePM(scalePM_)				// PYR-40177
	{
	}
	HgClubRecentTournPlayerResult(CommMsgParser& parser)
	{
		parse(parser);
	}
	void compose(CommMsgBody& body) const
	{
		body.composeString(userId)
			.composeUINT32(place)
			.composeUINT32(placeTo)
			.composeUINT32(prize)
			.composeUINT32(tournRank)
			.composeUINT32(pointsEarned)
			.composeUINT32(knockouts)
			.composeINT32(scalePM)									// PYR-40177
			;
	}
	void parse(CommMsgParser& parser)
	{
		parser.parseStringP(userId)
			.parseUINT32(place)
			.parseUINT32(placeTo)
			.parseUINT32(prize)
			.parseUINT32(tournRank)
			.parseUINT32(pointsEarned)
			.parseUINT32(knockouts)
			;

		// PYR-40177
		if( !parser.parseEnded() )
		{
			parser.parseINT32(scalePM);
		}
		else
		{
			scalePM = 1;
		}
	}
	const char*	getUserId()	const { return userId; }
	UINT32	getPlace()			const { return place; }
	UINT32	getPlaceTo()		const { return placeTo; }
	UINT32	getPrize()			const { return prize; }
	UINT32	getTournRank()		const { return tournRank; }
	UINT32	getPointsEarned()	const { return pointsEarned; }
	UINT32	getKnockouts()		const { return knockouts; }
	INT32   getScalePM()		const { return scalePM; }			// PYR-40177

	void setTournRank(const UINT32 tournRank_)
	{
		tournRank = tournRank_;
	}
	
	void print() const
	{
		PLog("HgClubRecentTournPlayerData< userId='%s', tournRank=%u place=%u placeTo=%u prize=%u scalePM=%d points=%u kkos=%u>",
			 userId.c_str(), tournRank, place, placeTo, prize, scalePM, pointsEarned, knockouts);
	}
};

class HgClubRecentTournResults
{
public:
	vector<HgClubRecentTournPlayerResult> playerResults;
	bool isTournCountingToStats;		// PYR-17842

	HgClubRecentTournResults() : isTournCountingToStats(false)
	{
	}
	HgClubRecentTournResults(CommMsgParser& parser)
	{
		parse(parser);
	}
	void compose(CommMsgBody& body) const
	{
		composeVectorOfMessageBodiesOfComposableValues(playerResults, body);
		body.composeBOOL(isTournCountingToStats);
	}
	void parse(CommMsgParser& parser)
	{
		parseVectorOfParsableMessageBodies(playerResults, parser);
		if( !parser.parseEnded() )
		{
			parser.parseBOOL( isTournCountingToStats );
		}
		else
		{
			isTournCountingToStats = false;
		}
	}
	size_t size() const
	{
		return playerResults.size();
	}
	const HgClubRecentTournPlayerResult& operator[]( size_t i ) const
	{
		return playerResults[ i ];
	}
	void print() const
	{
		PLog("HgClubRecentTournResults<");
		for(int i = 0; i < playerResults.size(); ++i)
		{
			playerResults[i].print();
		}
		PLog("isCounting=%u >HgClubRecentTournResults", isTournCountingToStats ? 1 : 0);
	}
};

//////////////////////////////////////////////////////////////////////////

class HgClubSeasonUserRank
{
	PString	userId;
	UINT64	pointsTotal;
	UINT32	gamesPlayed;
	UINT32	rank;

public:
	HgClubSeasonUserRank(	const char* userId_, 
							const UINT64 pointsTotal_, 
							const UINT32 gamesPlayed_, 
							const UINT32 rank_)
							: 
							userId(userId_), 
							pointsTotal(pointsTotal_), 
							gamesPlayed(gamesPlayed_), 
							rank(rank_)
	{
	}
	HgClubSeasonUserRank( CommMsgParser& parser)
	{
		parse(parser);
	}
	HgClubSeasonUserRank() : pointsTotal(0), gamesPlayed(0), rank(0)
	{
	}
	void compose(CommMsgBody& body) const
	{
		body.composeString(userId)
			.composeUINT64(pointsTotal)
			.composeUINT32(gamesPlayed)
			.composeUINT32(rank)
			;
	}
	void parse(CommMsgParser& parser)
	{
		parser.parseStringP(userId)
			.parseUINT64(pointsTotal)
			.parseUINT32(gamesPlayed)
			.parseUINT32(rank)
			;
	}
	const char* getUserId() const { return userId; }
	UINT64 getPointsTotal() const { return pointsTotal; }
	UINT32 getGamesPlayed() const { return gamesPlayed; }
	UINT32 getRank()		const { return rank; }

	void print() const
	{
		PString strPointsTotal;
		strPointsTotal.appendInt64(pointsTotal);
		PLog("CSUR< u='%s' prs=%s gms=%u rnk=%u>", userId.c_str(), strPointsTotal.c_str(), gamesPlayed, rank);
	}
};

//////////////////////////////////////////////////////////////////////////
class HgClubSeasonRanks
{
public:
	vector<HgClubSeasonUserRank> ranks;

	HgClubSeasonRanks()
	{
	}
	HgClubSeasonRanks(CommMsgParser& parser)
	{
		parse(parser);
	}
	void compose(CommMsgBody& body) const
	{
		composeVectorOfMessageBodiesOfComposableValues(ranks, body);
	}
	void parse(CommMsgParser& parser)
	{
		parseVectorOfParsableMessageBodies(ranks, parser);
	}
	void print() const
	{
		PLog("HgClubSeasonRanks<");
		for(int i = 0; i < ranks.size(); ++i)
		{
			ranks[i].print();
		}
		PLog(">HgClubSeasonRanks");
	}
};
//////////////////////////////////////////////////////////////////////////

class HgClubSeason
{
	UINT32	seasonId;
	SrvTime	timeFrom;
	SrvTime	timeTo;
	bool	closed;
	HomeGamesClubSeasons seasonType;
	PString	seasonName;

public:
	HgClubSeason(	const UINT32 seasonId_, 
					const SrvTime& from_, 
					const SrvTime& to_, 
					const bool closed_,
					const HomeGamesClubSeasons seasonType_,
					const char* seasonName_)
					: 
					seasonId(seasonId_), 
					timeFrom(from_), 
					timeTo(to_), 
					closed(closed_),
					seasonType(seasonType_),
					seasonName(seasonName_)
	{
	}
	HgClubSeason( CommMsgParser& parser) : seasonId(0), closed(false), seasonType(eHgClubSeasonsYearly)
	{
		parse(parser);
	}
	HgClubSeason() : seasonId(0), closed(false), seasonType(eHgClubSeasonsYearly)
	{
	}
	void compose(CommMsgBody& body) const
	{
		body.composeUINT32(seasonId)
			.composeSrvTime(timeFrom)
			.composeSrvTime(timeTo)
			.composeBOOL(closed)
			.composeUINT32(seasonType)
			.composeString(seasonName)
			;
	}
	void parse(CommMsgParser& parser)
	{
		parser.parseUINT32(seasonId)
			  .parseSrvTime(timeFrom)
			  .parseSrvTime(timeTo)
			  .parseBOOL(closed)
			  ;
		parseEnum32asUINT32(parser, seasonType);

		parser.parseStringP(seasonName);
	}
	UINT32 getSeasonId()					const { return seasonId; }
	bool isClosed()							const { return closed; }
	const SrvTime& getTimeTo()				const { return timeTo; }
	const SrvTime& getTimeFrom()			const { return timeFrom; }
	HomeGamesClubSeasons getSeasonType()	const { return seasonType; }
	const char* getSeasonName()				const { return seasonName; }
	void setSeasonName(const char* seasonName_)
	{
		seasonName = seasonName_;
	}
	void print() const
	{
		PLog("HGCS< sid=%u %s type=%u name='%s'>", seasonId, (closed ? "closed" : "open"), (UINT32)seasonType, seasonName.c_str());
	}
};

//////////////////////////////////////////////////////////////////////////
class HgClubSeasons
{
public:
	vector<HgClubSeason> seasons;

	HgClubSeasons()
	{
	}
	HgClubSeasons(CommMsgParser& parser)
	{
		parse(parser);
	}
	void compose(CommMsgBody& body) const
	{
		composeVectorOfMessageBodiesOfComposableValues(seasons, body);
	}
	void parse(CommMsgParser& parser)
	{
		parseVectorOfParsableMessageBodies(seasons, parser);
	}
	size_t size() const
	{
		return seasons.size();
	}
	const HgClubSeason& operator[]( size_t i ) const
	{
		return seasons[ i ];
	}
	void print() const
	{
		PLog("HgClubSeasons<");
		for(int i = 0; i < seasons.size(); ++i)
		{
			seasons[i].print();
		}
		PLog(">HgClubSeasons");
	}
};

//////////////////////////////////////////////////////////////////////////
// HgLobby objects
struct HgChangeClubOptionsParams
{
	UINT32 clubId;
	HomeGamesClubSeasons seasonLength;
	bool showClubStandings;
	bool showPlayerStats;
	PString currency;
	bool showFounderFullName;
	bool isSaveHeader;

	HgChangeClubOptionsParams()
	{
		clear();
	}
	HgChangeClubOptionsParams( CommMsgParser& parser ) 
		: isSaveHeader(false)
	{
		parse( parser ); 
	}
	HgChangeClubOptionsParams(	UINT32 clubId_, 
								HomeGamesClubSeasons seasonLength_,
								bool showClubStandings_, 
								bool showPlayerStats_, 
								const char* currency_, 
								bool showFounderFullName_ )
								:
								clubId(clubId_),
								seasonLength(seasonLength_),
								showClubStandings(showClubStandings_),
								showPlayerStats(showPlayerStats_),
								currency(currency_),
								showFounderFullName(showFounderFullName_),
								isSaveHeader(false)
								{}
	void clear()
	{
		clubId				= 0;
		seasonLength		= eHgClubSeasonsYearly;
		showClubStandings	= false;
		showPlayerStats		= false;
		currency			= "";
		showFounderFullName	= false;
		isSaveHeader		= false;
	}
	void parse( CommMsgParser& parser )
	{
		parser.parseUINT32( clubId );

		parseEnum32asUINT32( parser, seasonLength );

		parser.parseBOOL( showClubStandings )
			  .parseBOOL( showPlayerStats )
			  .parseStringP( currency )
			  .parseBOOL( showFounderFullName )
			  ;
	}

	void compose( CommMsgBody& body ) const
	{
		body.composeUINT32( clubId )
			.composeUINT32( seasonLength )
			.composeBOOL( true )
			.composeBOOL( true )
			.composeString( currency )
			.composeBOOL( showFounderFullName )
			;
	}

	void print() const
	{
		PLog( "Club=%u, Seasons=%u, ShowStandings=%s, ShowPlayerStats=%s, Currency=%s, ShowManager=%s isSaveHeader=%s",
			clubId, 
			seasonLength, 
			showClubStandings	? "true" : "false", 
			showPlayerStats		? "true" : "false",
			currency.c_str(), 
			showFounderFullName	? "true" : "false",
			isSaveHeader ? "yes" : "no" );
	}
};

//////////////////////////////////////////////////////////////////////////

// PYR-17813 - recurring tournaments

enum HgRecurringTournPatternType
{
	eHgNotRecurring     = 0,
	eHgRecurringDaily   = 1,
	eHgRecurringWeekly  = 2,
	eHgRecurringMonthly = 3
};

enum HgRecurringDay
{
	eHgRdNone            = 0x000000,

	eHgRdSunday          = 0x0001,
	eHgRdMonday          = 0x0002,
	eHgRdTuesday         = 0x0004,
	eHgRdWednesday       = 0x0008,
	eHgRdThursday        = 0x0010,
	eHgRdFriday          = 0x0020,
	eHgRdSaturday        = 0x0040,

	eHgRdEveryday        = 0x007F,  // all days mask
	eHgRdEveryWeekday    = 0x003E,  // Monday to Friday
	eHgRdEveryWeekendDay = 0x0041,  // Saturday/Sunday
};

enum HgWhichRecurringDay  // specified which recurring day of the week 
{
	eHgWrdNone    = 0,
	eHgWrdFirst   = 1,
	eHgWrdSecond  = 2,
	eHgWrdThird   = 3,
	eHgWrdFourth  = 4,
	eHgWrdLast    = 5
};

class HgRecurringTournament
{
public:
	//////////////////////////////////////////////////////////////////////////
	class Daily
	{
	public:
		UINT32 numberOfDays;
		bool everyWeekday;

		Daily(	UINT32 numberOfDays_ = 0, bool everyWeekday_ = false ) : numberOfDays(numberOfDays_), everyWeekday(everyWeekday_)
		{
		}
		void clear()
		{
			numberOfDays = 0;
			everyWeekday = false;
		}
		void compose( CommMsgBody& body ) const
		{
			body.composeUINT32( numberOfDays )
				.composeBOOL( everyWeekday )
				;
		}
		void parse( CommMsgParser& parser )
		{
			parser.parseUINT32( numberOfDays )
				  .parseBOOL( everyWeekday )
				  ;
		}
		void print() const
		{
			PTRACE5( "Daily: every %u days, everyWeekday=%u", numberOfDays, everyWeekday ? 1 : 0 );
		}
	};
	class Weekly
	{
	public:
		UINT32 numberOfWeeks;
		UINT32 recurringDays;

		Weekly(	const UINT32 numberOfWeeks_ = 0, const UINT32 recurringDays_ = 0) : numberOfWeeks(numberOfWeeks_), recurringDays( recurringDays_ )
		{
		}
		void clear()
		{
			numberOfWeeks = 0;
			recurringDays = 0;
		}
		void compose( CommMsgBody& body ) const
		{
			body.composeUINT32( numberOfWeeks )
				.composeUINT32( recurringDays );
		}
		void parse( CommMsgParser& parser )
		{
			parser.parseUINT32( numberOfWeeks )
				  .parseUINT32( recurringDays );
		}
		void print() const
		{
			PString recurrStr;
			PTRACE5( "Weekly: every %u weeks, days=%s", numberOfWeeks, getDaysStr( recurringDays, recurrStr ) );
		}
	};
	class Monthly
	{
	public:
		BYTE dayOfMonth;
		HgWhichRecurringDay  whichRecurringDay;
		HgRecurringDay recurringDay;
		BYTE numberMonths;

		Monthly(	BYTE dayOfMonth_						= 0, 
					HgWhichRecurringDay  whichRecurringDay_	= eHgWrdNone, 
					HgRecurringDay recurringDay_			= eHgRdNone, 
					BYTE numberMonths_						= 0 )
					: 
					dayOfMonth( dayOfMonth_ ), 
					whichRecurringDay( whichRecurringDay_ ),
					recurringDay( recurringDay_ ), 
					numberMonths( numberMonths_ )
		{
		}
		void clear()
		{
			dayOfMonth			= 0;
			whichRecurringDay	= eHgWrdNone;
			recurringDay		= eHgRdNone;
			numberMonths		= 0;
		}
		void compose( CommMsgBody& body ) const
		{
			body.composeBYTE( dayOfMonth )
				.composeBYTE( numberMonths )
				.composeUINT32( whichRecurringDay )
				.composeUINT32( recurringDay )
				;
		}
		void parse( CommMsgParser& parser )
		{
			parser.parseBYTE( dayOfMonth )
				  .parseBYTE( numberMonths )
				  ;
			parseEnum32asUINT32( parser, whichRecurringDay );
			parseEnum32asUINT32( parser, recurringDay );
		}
		void print() const
		{
			PString recurrStr1, recurrStr2;
			PTRACE5("Monthly: day %u, which week=%s, which day=%s, every %u months", 
					dayOfMonth, getWhichRecurringDayStr( whichRecurringDay, recurrStr1 ), 
					getDaysStr( recurringDay, recurrStr2 ), numberMonths );
		}

		static bool isValidWhichRecurringDay( UINT32 whichRecurringDay_ )
		{
			switch( whichRecurringDay_ )
			{
			case eHgWrdFirst:
			case eHgWrdSecond:
			case eHgWrdThird:
			case eHgWrdFourth:
			case eHgWrdLast:
				return true;

			default:
				return false;
			}
		}

		static bool isValidRecurringDay( UINT32 recurringDay_ )
		{
			switch( recurringDay_ )
			{
			case eHgRdSunday:
			case eHgRdMonday:
			case eHgRdTuesday:
			case eHgRdWednesday:
			case eHgRdThursday:
			case eHgRdFriday:
			case eHgRdSaturday:
			case eHgRdEveryday:
			case eHgRdEveryWeekday:
			case eHgRdEveryWeekendDay:
				return true;

			default:
				return false;
			}

		}
	};
	//////////////////////////////////////////////////////////////////////////
	HgRecurringTournPatternType	recurringPatternType;
	UINT32						nbOccurences;
    UINT32                      nbConsecutiveCancellation; // PYR-22229: number of consecutive times the tournament has been cancelled
	SrvDate						endDate;
	Daily						daily;
	Weekly						weekly;
	Monthly						monthly;

	HgRecurringTournament(	HgRecurringTournPatternType recurringType_	= eHgNotRecurring,
							UINT32 nbOccurences_						= 0,
							const SrvTime& endDate_						= SrvTime(), 
							const Daily& daily_							= Daily(), 
							const Weekly& weekly_						= Weekly(),
							const Monthly& monthly_						= Monthly(),
							UINT32 nbCancelled_                         = 0 // PYR-22229
							) 
							:
							recurringPatternType( recurringType_ ),
							nbOccurences( nbOccurences_ ),
							nbConsecutiveCancellation( nbCancelled_ ),  // PYR-22229
                            endDate( endDate_ ),
							daily( daily_ ),
							weekly( weekly_ ),
							monthly( monthly_ )
	{
	}
	void clear()
	{
		recurringPatternType = eHgNotRecurring;
		nbOccurences = 0;
        nbConsecutiveCancellation = 0;  // PYR-22229
		endDate.setNull();
		daily.clear();
		weekly.clear();
		monthly.clear();
	}
	void compose( CommMsgBody& body ) const
	{
		body.composeUINT32(recurringPatternType)
			.composeUINT32(nbOccurences)
			.composeSrvDate(endDate)
			;
		switch( recurringPatternType )
		{
		case eHgNotRecurring:
			break;
		case eHgRecurringDaily:
			composeComposableObjectAsSeparateMessageBody(daily, body);
			break;
		case eHgRecurringWeekly:
			composeComposableObjectAsSeparateMessageBody(weekly, body );
			break;
		case eHgRecurringMonthly:
			composeComposableObjectAsSeparateMessageBody(monthly, body);
			break;
		default:
			PLog( "RecurringTournament::compose, invalid type=%u", recurringPatternType );
		}
		body.composeUINT32(nbConsecutiveCancellation); // PYR-22229
	}
	void parse( CommMsgParser& parser, bool fromClient = false )
	{
		daily.clear();
		weekly.clear();
		monthly.clear();

		parseEnum32asUINT32(parser, recurringPatternType);
		parser.parseUINT32(nbOccurences)
			  .parseSrvDate(endDate)
			  ;
		switch( recurringPatternType )
		{
		case eHgNotRecurring:
			break;
		case eHgRecurringDaily:
			parseParsableObjectFromSeparateMessageBody(daily, parser);
			break;
		case eHgRecurringWeekly:
			parseParsableObjectFromSeparateMessageBody(weekly, parser);
			break;
		case eHgRecurringMonthly:
			parseParsableObjectFromSeparateMessageBody(monthly, parser);
			break;
		default:
			PLog( "RecurringTournament::parse, invalid type=%u", recurringPatternType );
		}
		if( fromClient ) // TODO: remove this condition after June 2012 build and do unconditional parse
		{
			nbConsecutiveCancellation = 0;
			return;
		}
		parser.parseUINT32(nbConsecutiveCancellation); // PYR-22229
	}
	void print() const
	{
		PTRACE5(	"recurringType=%u, nbOccurences=%u, nbCancelled=%u, endDate=%u/%02u/%02u",
					recurringPatternType, 
					nbOccurences, 
                    nbConsecutiveCancellation, // PYR-22229
					endDate._year, 
					endDate._month, 
					endDate._day );

		switch( recurringPatternType )
		{
		case eHgRecurringDaily:
			daily.print();
			break;
		case eHgRecurringWeekly:
			weekly.print();
			break;
		case eHgRecurringMonthly:
			monthly.print();
			break;
		case eHgNotRecurring:
			PTRACE5("Not Recurring");
			break;
		}
	}

	static const char* getDaysStr( UINT32 dayMask, PString& daysStr )
	{
		if( !dayMask )
		{
			daysStr = "None";
			return daysStr.c_str();
		}

		UINT32 oldDayMask = 0;

		daysStr = "";
		while( dayMask && dayMask != oldDayMask )
		{
			oldDayMask = dayMask;
			if( *daysStr )
			{
				daysStr.append(", ");
			}

			if( ( dayMask & eHgRdEveryday ) == eHgRdEveryday )
			{
				daysStr.append("Everyday");
				dayMask &= ~eHgRdEveryday;
			}
			else if( ( dayMask & eHgRdEveryWeekday ) == eHgRdEveryWeekday )
			{
				daysStr.append("Every weekday");
				dayMask &= ~eHgRdEveryWeekday;
			}
			else if( ( dayMask & eHgRdEveryWeekendDay ) == eHgRdEveryWeekendDay )
			{
				daysStr.append("Every weekend day");
				dayMask &= ~eHgRdEveryWeekendDay;
			}
			else if( ( dayMask & eHgRdSunday ) )
			{
				daysStr.append( "Sunday" );
				dayMask &= ~eHgRdSunday;
			}
			else if( ( dayMask & eHgRdMonday ) )
			{
				daysStr.append( "Monday" );
				dayMask &= ~eHgRdMonday;
			}
			else if( ( dayMask & eHgRdTuesday ) )
			{
				daysStr.append( "Tuesday" );
				dayMask &= ~eHgRdTuesday;
			}
			else if( ( dayMask & eHgRdWednesday ) )
			{
				daysStr.append( "Wednesday" );
				dayMask &= ~eHgRdWednesday;
			}
			else if( ( dayMask & eHgRdThursday ) )
			{
				daysStr.append( "Thursday" );
				dayMask &= ~eHgRdThursday;
			}
			else if( ( dayMask & eHgRdFriday ) )
			{
				daysStr.append( "Friday" );
				dayMask &= ~eHgRdFriday;
			}
			else if( ( dayMask & eHgRdSaturday ) )
			{
				daysStr.append( "Saturday" );
				dayMask &= ~eHgRdSaturday;
			}
			else
			{
				daysStr.append( "None=" ).appendHexInt( dayMask );
			}
		}

		return daysStr.c_str();
	}

	static const char* getWhichRecurringDayStr( UINT32 whichRecurringDay, PString& recurDayStr )
	{
		switch( whichRecurringDay )
		{
		case eHgWrdNone:
			recurDayStr = "none";
			break;

		case eHgWrdFirst:
			recurDayStr = "first";
			break;

		case eHgWrdSecond:
			recurDayStr = "second";
			break;

		case eHgWrdThird:
			recurDayStr = "third";
			break;

		case eHgWrdFourth:
			recurDayStr = "fourth";
			break;

		case eHgWrdLast:
			recurDayStr = "last";
			break;

		default:
			recurDayStr = "unknown=";
			recurDayStr.appendUint( whichRecurringDay );
			break;
		}

		return recurDayStr.c_str();
	}
};

//////////////////////////////////////////////////////////////////////////

struct ScheduledTournament
{
//IMPORTANT: please also update the ATF protocol definition P_ScheduledTournament in protocols/storm/TableCommonStructs.txt!!!
	SrvTime startTime;
	UINT32 clubId;
	PString clubName;
	UINT32 tournId;
	PString tournName;
	BYTE gameType;
	bool isHiLo;
	UINT32 buyin;
	UINT32 rake;
	UINT32 knockoutBounty;
	UINT32 numPlayers;
	bool isRegistered;
	BYTE limit;      
	PString currency;
	bool isPlayMoney;
	UINT32 maxPlayers;
	UINT32 tournState; 
	PString server;			// PYR-18302
	PString serverObject;	// PYR-18302
	UINT32	flags;
	INT32   scalePM;		// PYR-40177

	// Not part of message, just for internal handling
	UINT32 statusBits;

	ScheduledTournament()
	{
		clear();
	}
	ScheduledTournament(const SrvTime& startTime_, 
						UINT32 clubId_, 
						const char* clubName_, 
						UINT32 tournId_, 
						const char* tournName_, 
						BYTE gameType_, 
						bool isHiLo_, 
						UINT32 buyin_, 
						UINT32 rake_, 
						UINT32 knockoutBounty_, 
						UINT32 numPlayers_, 
						bool isRegistered_, 
						BYTE limit_,
						const char* currency_, 
						bool isPlayMoney_, 
						UINT32 maxPlayers_, 
						UINT32 tournState_,
						UINT32 statusBits_,
						const char* server_,
						const char* serverObject_,
						UINT32 flags_,
						INT32 scalePM_)					// PYR-40177
						:
						startTime( startTime_ ),
						clubId( clubId_ ),
						clubName( clubName_ ),
						tournId( tournId_ ),
						tournName( tournName_ ),
						gameType( gameType_ ),
						isHiLo( isHiLo_ ),
						buyin( buyin_ ),
						rake( rake_ ),
						knockoutBounty( knockoutBounty_ ),
						numPlayers( numPlayers_ ),
						isRegistered( isRegistered_ ),
						limit( limit_ ),
						currency( currency_ ),
						isPlayMoney( isPlayMoney_ ),
						maxPlayers( maxPlayers_ ),
						tournState( tournState_ ),
						server( server_ ),				// PYR-18302
						serverObject( serverObject_ ),  // PYR-18302
						flags( flags_ ),
						scalePM( scalePM_ ),			// PYR-40177
						statusBits( statusBits_ )
						{
						}

	void clear()
	{
		startTime.setNull();
		clubId			= 0;
		clubName		= "";
		tournId			= 0;
		tournName		= "";
		gameType		= 0;
		isHiLo			= false;
		buyin			= 0;
		rake			= 0;
		knockoutBounty	= 0;
		numPlayers		= 0;
		isRegistered	= false;
		limit			= 0;
		currency		= "";
		isPlayMoney		= false;
		maxPlayers		= 0;
		tournState		= 0;
		statusBits		= 0;
		server          = "";
		serverObject    = "";
		flags			= 0;
		scalePM			= 1;
	}
	void parse( CommMsgParser& parser )
	{
		parser.parseSrvTime( startTime )
			  .parseUINT32( clubId )
			  .parseStringP( clubName )
			  .parseUINT32( tournId )
			  .parseStringP( tournName )
			  .parseBYTE( gameType )
			  .parseBOOL( isHiLo )
			  .parseUINT32( buyin )
			  .parseUINT32( rake )
			  .parseUINT32( knockoutBounty )
			  .parseUINT32( numPlayers )
			  .parseBOOL( isRegistered )
			  .parseBYTE( limit )
			  .parseStringP( currency )
			  .parseBOOL( isPlayMoney )
			  .parseUINT32( maxPlayers )
			  .parseUINT32( tournState )
			  .parseStringP( server )		// PYR-18302
			  .parseStringP( serverObject ) // PYR-18302
			  ;

		// PYR-18513
		if( !parser.parseEnded() )
		{
			parser.parseUINT32( flags );
		}
		else
		{
			flags = 0;
		}
		// PYR-40177
		if( !parser.parseEnded() )
		{
			parser.parseINT32( scalePM );
		}
		else
		{
			scalePM = 1;
		}
	}
	void compose( CommMsgBody& body ) const
	{
		body.composeSrvTime( startTime )
			.composeUINT32( clubId )
			.composeString( clubName )
			.composeUINT32( tournId )
			.composeString( tournName )
			.composeBYTE( gameType )
			.composeBOOL( isHiLo )
			.composeUINT32( buyin )
			.composeUINT32( rake )
			.composeUINT32( knockoutBounty )
			.composeUINT32( numPlayers )
			.composeBOOL( isRegistered )
			.composeBYTE( limit )
			.composeString( currency )
			.composeBOOL( isPlayMoney )
			.composeUINT32( maxPlayers )
			.composeUINT32( tournState )
			.composeString( server )
			.composeString( serverObject )
			.composeUINT32( flags )
			.composeINT32( scalePM )
			;
	}
	void print() const
	{
		PTRACE5(	"Start time=%u-%02u-%02u %02u-%02u, club id=%u, club name=%s, tournId=%u, tourn name=%s, "
					"game=%u, isHiLo=%s, "
					"buyin=%u, rake=%u, knockoutBounty=%u, #players=%u, isRegistered=%s, "
					"limit=%u, currency=%s, isPlayMoney=%s, max players=%u, tournState=%u "
					"server=%s, serverObject=%s voice %s, scalePM=%d",
					startTime._year, 
					startTime._month, 
					startTime._day, 
					startTime._hour, 
					startTime._minute, 
					clubId, 
					clubName.c_str(), 
					tournId, 
					tournName.c_str(), 
					gameType, 
					isHiLo ? "true" : "false", 
					buyin, 
					rake, 
					knockoutBounty,
					numPlayers, 
					isRegistered ? "true" : "false",
					limit, 
					currency.c_str(), 
					isPlayMoney ? "true" : "false",
					maxPlayers, 
					tournState,
					server.c_str(),
					serverObject.c_str(),
					( flags & eHgFavTournFlagsVoiceEnabled ) ? "enabled" : "not enabled",
					scalePM
					);
	}
};
//////////////////////////////////////////////////////////////////////////
struct ScheduledRingGame
{
	UINT32 clubId;
	PString clubName;
	UINT64 tableId;
	PString tableName;
	BYTE gameType;
	bool isHiLo;
	UINT32 buyin;
	UINT32 numPlayers;
	bool isSeated;
	BYTE limit;
	PString currency;
	bool isPlayMoney;
	UINT32 lowStake;
	UINT32 highStake;
	PString tableObject;	// PYR-18302
	UINT32 maxNumPlayers;	// PYR-18302 - now part of message
	UINT32 playersSeated;	// PYR-18183
	UINT32 flags;
	INT32 scalePM;			// PYR-40177

	ScheduledRingGame()
	{
		clear();
	}
	ScheduledRingGame(	UINT32 clubId_, 
						const char* clubName_, 
						UINT64 tableId_, 
						const char* tableName_, 
						BYTE gameType_, 
						bool isHiLo_, 
						UINT32 buyin_, 
						UINT32 numPlayers_, 
						bool isSeated_,
						BYTE limit_, 
						const char* currency_, 
						bool isPlayMoney_,
						UINT32 lowStake_, 
						UINT32 highStake_,
						const char* tableObject_,	// PYR-18302
						UINT32 maxNumPlayers_,
						UINT32 playersSeated_,		// PYR-18183
						UINT32 flags_,				// PYR-18513
						INT32 scalePM_)				// PYR-40177
						:
						clubId( clubId_ ),
						clubName( clubName_ ),
						tableId( tableId_ ),
						tableName( tableName_ ),
						gameType( gameType_ ),
						isHiLo( isHiLo_ ),
						buyin( buyin_ ),
						numPlayers( numPlayers_ ),
						isSeated( isSeated_ ),
						limit( limit_ ),
						currency( currency_ ),
						isPlayMoney( isPlayMoney_ ),
						lowStake( lowStake_ ),
						highStake( highStake_ ),
						tableObject( tableObject_ ),	// PYR-18302
						maxNumPlayers( maxNumPlayers_ ),
						playersSeated( playersSeated_ ),// PYR-18183
						flags( flags_ ),
						scalePM( scalePM_ )
	{
	}
	void clear()
	{
		clubId			= 0;
		clubName		= "";
		tableId			= 0;
		tableName		= "";
		gameType		= 0;
		isHiLo			= false;
		buyin			= 0;
		numPlayers		= 0;
		isSeated		= false;
		limit			= 0;
		currency		= "";
		isPlayMoney		= false;
		lowStake		= 0;
		highStake		= 0;
		maxNumPlayers	= 0;
		tableObject     = "";
		playersSeated	= 0;
		flags			= 0;
		scalePM			= 1;
	}
	void parse( CommMsgParser& parser )
	{
		parser.parseUINT32( clubId )
			  .parseStringP( clubName );
		UINT32 tableId32;
		parser.parseUINT32( tableId32 )
			  .parseStringP( tableName )
			  .parseBYTE( gameType )
			  .parseBOOL( isHiLo )
			  .parseUINT32( buyin )
			  .parseUINT32( numPlayers )
			  .parseBOOL( isSeated )
			  .parseBYTE( limit )
			  .parseStringP( currency )
			  .parseBOOL( isPlayMoney )
			  .parseUINT32( lowStake )
			  .parseUINT32( highStake )
			  .parseStringP( tableObject ) // PYR-18302, PYR-18183
			  .parseUINT32( maxNumPlayers )
			  .parseUINT32( playersSeated )
			  .parseUINT32(flags) // PYR-18513
			  ;
		
		if( !parser.parseEnded() ) // PYR-29494 TODO: remove conditional parsing some time after Aug 2014.
		{
			parser.parseUINT64(tableId);
		}
		else
		{
			tableId = tableId32;
		}
		if( !parser.parseEnded() ) // PYR-40177 Composed only in getScheduledUserGamesImpl() which is called by getUserClubsReply2() and getScheduledUserGames() and parsed by client. 
		{                          // TODO: remove conditional parsing around July 2017
			parser.parseINT32(scalePM);
		}
		else
		{
			scalePM = 1;
		}
	}
	void compose( CommMsgBody& body ) const
	{
		body.composeUINT32( clubId )
			.composeString( clubName )
			.composeUINT32( static_cast< UINT32 >( tableId ) )
			.composeString( tableName )
			.composeBYTE( gameType )
			.composeBOOL( isHiLo )
			.composeUINT32( buyin )
			.composeUINT32( numPlayers )
			.composeBOOL( isSeated )
			.composeBYTE( limit )
			.composeString( currency )
			.composeBOOL( isPlayMoney )
			.composeUINT32( lowStake )
			.composeUINT32( highStake )
			.composeString( tableObject )	// PYR-18302
			.composeUINT32( maxNumPlayers )	// PYR-18302
			.composeUINT32( playersSeated )	// PYR-18183
			.composeUINT32(flags)			// PYR-18513
			.composeUINT64(tableId)			// PYR-29494
			.composeINT32(scalePM)			// PYR-40177
			;
	}
	void print() const
	{
		PTRACE5(	"club id=%u, club name=%s, tableId=%" PRIu64 ", table name=%s, "
					"game=%u, isHiLo=%s, "
					"buyin=%u, #players=%u, isSeated=%s, "
					"limit=%u, currency=%s, isPlayMoney=%s, lowStake=%u, highStake=%u, "
					"tableObject=%s, maxNumPlayers=%u, playersSeated=%u flags=%u, scalePM=%d",
					clubId, 
					clubName.c_str(), 
					tableId, 
					tableName.c_str(), 
					gameType, 
					isHiLo ? "true" : "false", 
					buyin, 
					numPlayers, 
					isSeated ? "true" : "false", 
					limit, 
					currency.c_str(), 
					isPlayMoney ? "true" : "false",
					lowStake, 
					highStake,
					tableObject.c_str(),
					maxNumPlayers,
					playersSeated,
					flags,
					scalePM);
	}
	bool isActiveForMe( bool seatAvailOnly ) const
	{
		return !isSeated && ( 0 < playersSeated ) &&
			( ( playersSeated < maxNumPlayers ) || !seatAvailOnly );
	}
};
//////////////////////////////////////////////////////////////////////////
struct ScheduledGames
{
	vector<ScheduledTournament> tourns;
	vector<ScheduledRingGame> ringGames;
	PString msgBoard;

	ScheduledGames()
	{
		clear();
	}
	ScheduledGames( CommMsgParser& parser )
	{
		parse( parser );
	}
	void clear()
	{
		tourns.clear();
		ringGames.clear();
	}
	void parse( CommMsgParser& parser )
	{
		parseVectorOfParsableMessageBodies( tourns, parser );
		parseVectorOfParsableMessageBodies( ringGames, parser );
		parser.parseStringP( msgBoard );
	}
	void compose( CommMsgBody& body ) const
	{
		composeVectorOfMessageBodiesOfComposableValues( tourns, body );
		composeVectorOfMessageBodiesOfComposableValues( ringGames, body );
		body.composeString( msgBoard );
	}
	void print() const
	{
		PTRACE5( "Tournaments" );
		PTRACE5( "-----------" );

		for( vector<ScheduledTournament>::const_iterator tournPtr = tourns.begin();
			tournPtr != tourns.end();
			++tournPtr )
		{
			PTRACE5( "" );
			tournPtr->print();
		}

		PTRACE5( "" );
		PTRACE5( "Ring games" );
		PTRACE5( "----------" );

		for( vector<ScheduledRingGame>::const_iterator ringGamePtr = ringGames.begin();
			ringGamePtr != ringGames.end();
			++ringGamePtr )
		{
			PTRACE5( "" );
			ringGamePtr->print();
		}
	}
};
// HgLobby objects done
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
enum HgMessageType
{
	eHgMessageTypeWall = 1,
	eHgMessageTypeAnnouncement = 2,
	eHgMessageTypeLast
};

class HgClubMessageData
{
public:
	HgMessageType	type;
	UINT64			id;
	UINT32			clubId;
	UINT32			pinnedPos;
	UINT64			parentId;
	SrvTime			created;
	bool			obsolete;
	PString			userId;
	UINT32			userIntId;
	PString			content;
	UINT32			roleFlags;
	UINT32			flags;
	static const UINT32 pinPosIgnoreAllPreviousMessages = 0xFFFFFFFF;

protected:
	// to be used by the client
	HgClubMessageData(	HgMessageType	type_
					 ,	UINT32			clubId_
					 ,	UINT64			parentId_
					 ,	UINT64			id_
					 ,	const char*		content_
					 ,	bool			obsolete_
					 ,	UINT32			pinnedPos_ = 0)
					 :  
						type		(	type_		)
					 ,	id			(	id_			)
					 ,	clubId		(	clubId_		)
					 ,	pinnedPos	(	pinnedPos_	)
					 ,	parentId	(	parentId_	)
					 ,	obsolete	(	obsolete_	)
					 ,	userIntId	(	0			)
					 ,	content		(	content_	)
					 ,	roleFlags	(	0			)
					 ,	flags		(	0			)
	{
	}
public:
	HgClubMessageData()	: type			( eHgMessageTypeLast )
						, id			( 0 )
						, clubId		( 0 )
						, pinnedPos		( 0 )
						, parentId		( 0 )
						, obsolete		( false )
						, userIntId		( 0 )
						, roleFlags		( 0 )
						, flags			( 0 )
	{
	}
	// to be used by the server
	HgClubMessageData(	HgMessageType	type_,
						UINT64			id_,
						UINT32			clubId_,
						UINT32			pinnedPos_,
						UINT64			parentId_,
						const SrvTime&	created_,
						const char*		userId_,
						UINT32			userIntId_,
						const char*		content_,
						bool			obsolete_,
						UINT32			roleFlags_,
						UINT32			flags_) 
						:
						type(type_),
						id(id_),
						clubId(clubId_),
						pinnedPos(pinnedPos_),
						parentId(parentId_),
						created(created_),
						obsolete(obsolete_),
						userId(userId_),
						userIntId(userIntId_),
						content(content_),
						roleFlags(roleFlags_),
						flags(flags_)
	{
	}
	HgClubMessageData( CommMsgParser& parser )
	{
		parse(parser);
	}
	void compose(CommMsgBody& body) const
	{
		body.composeUINT32(type)
			.composeUINT64(id)
			.composeUINT32(clubId)
			.composeUINT32(pinnedPos)
			.composeUINT64(parentId)
			.composeUINT32(userIntId)
			.composeSrvTime(created)
			.composeString(userId)
			.composeString(content)
			.composeBOOL(obsolete)
			.composeUINT32(roleFlags)
			.composeUINT32(flags)
			;

	}
	void parse(CommMsgParser& parser)
	{
		parseEnum32asUINT32(parser, type);
		parser.parseUINT64(id)
			.parseUINT32(clubId)
			.parseUINT32(pinnedPos)
			.parseUINT64(parentId)
			.parseUINT32(userIntId)
			.parseSrvTime(created)
			.parseStringP(userId)
			.parseStringP(content)
			.parseBOOL(obsolete)
			.parseUINT32(roleFlags)
			.parseUINT32(flags)
			;
	}
	void print() const
	{
		PString strMsgId, strParentId;
		strMsgId.appendUint64(id);
		strParentId.appendUint64(parentId);
		PTRACE5("HgMessageData: id=%s t=%d c=%d pt=%s o=%d pn=%d rf=%d f=%d", 
				strMsgId.c_str(), type, clubId, strParentId.c_str(), obsolete ? 1 : 0, pinnedPos, roleFlags, flags);
		PTRACE5("'%s'", content.c_str());
	}
	void printNoContent() const
	{
		PString strMsgId, strParentId;
		strMsgId.appendUint64(id);
		strParentId.appendUint64(parentId);
		PTRACE5("HgMessageData: id=%s t=%d c=%d pt=%s o=%d pn=%d rf=%d f=%d", 
				strMsgId.c_str(), type, clubId, strParentId.c_str(), obsolete ? 1 : 0, pinnedPos, roleFlags, flags);
	}
};

//------------------------------------------------------------------------------
// client message helpers

class HomeClubWallMessage : public HgClubMessageData
{
public:
	HomeClubWallMessage( UINT32 homeClubId, const char* msgBody )
		: HgClubMessageData( eHgMessageTypeWall, homeClubId, 0, 0, msgBody, false )
	{
	}
};

class HomeClubWallMessageReply : public HgClubMessageData
{
public:
	HomeClubWallMessageReply( UINT32 homeClubId, UINT64 replyTo, const char* msgBody )
		: HgClubMessageData( eHgMessageTypeWall, homeClubId, replyTo, 0, msgBody, false )
	{
	}
};

class HomeClubWallMessageEraser : public HgClubMessageData
{
public:
	HomeClubWallMessageEraser( UINT32 homeClubId, UINT64 msgId )
		: HgClubMessageData( eHgMessageTypeWall, homeClubId, 0, msgId, NULL, true )
	{
	}
};

class HomeClubWallEraser : public HgClubMessageData
{
public:
	HomeClubWallEraser( UINT32 homeClubId )
		: HgClubMessageData( eHgMessageTypeWall, homeClubId, 0, 0, NULL, false, pinPosIgnoreAllPreviousMessages )
	{
	}
};

//------------------------------------------------------------------------------

class HomeClubWhiteboardMessage : public HgClubMessageData
{
public:
	HomeClubWhiteboardMessage( UINT32 homeClubId, const char* msgBody )
		: HgClubMessageData( eHgMessageTypeAnnouncement, homeClubId, 0, 0, msgBody, false )
	{
	}
};

class HomeClubWhiteboardMessageEraser : public HgClubMessageData
{
public:
	HomeClubWhiteboardMessageEraser( UINT32 homeClubId )
		: HgClubMessageData( eHgMessageTypeAnnouncement, homeClubId, 0, 0, NULL, false )
	{
	}
};

//------------------------------------------------------------------------------

class HgClubMessages
{
public: 
	vector<HgClubMessageData> data;

	HgClubMessages()
	{
	}
	HgClubMessages(CommMsgParser& parser)
	{
		parse(parser);
	}
	void compose(CommMsgBody& body) const
	{
		composeVectorOfMessageBodiesOfComposableValues(data, body);
	}
	void parse(CommMsgParser& parser)
	{
		parseVectorOfParsableMessageBodies(data, parser);
	}
	void print() const
	{
		PLog("HgMessages< size=%zu", data.size());
		HgPrintVector::printPrintableVector(data);
		PLog(">HgMessages");
	}
};


class HgClubMessagePublicationItem
{
public:
	UINT64			id;
	SrvTime			created;
	PString			userId;
	PString			content;
	UINT32			pinnedPos;
	UINT32			roleFlags;
	UINT32			flags;
	PString			fullName;


	HgClubMessagePublicationItem()
		: id(0), pinnedPos(0), roleFlags(0), flags(0)
	{
	}
	// to be used by the server
	HgClubMessagePublicationItem( CommMsgParser& parser)
		: id(0), pinnedPos(0), roleFlags(0), flags(0)
	{
		parse(parser);
	}
	static void compose(CommMsgBody& body, const HgClubMessageData& d, const char* fullName)
	{
		body.composeUINT64(d.id)
			.composeSrvTime(d.created)
			.composeString(d.userId)
			.composeString(d.content)
			.composeUINT32(d.pinnedPos)
			.composeUINT32(d.roleFlags)
			.composeUINT32(d.flags)
			.composeString(fullName)
			;

	}
	void parse(CommMsgParser& parser)
	{
		parser.parseUINT64(id)
			  .parseSrvTime(created)
			  .parseStringP(userId)
			  .parseStringP(content)
			  .parseUINT32(pinnedPos)
			  .parseUINT32(roleFlags)
			  .parseUINT32(flags)
			  .parseStringP(fullName)
			  ;

	}
	void print() const
	{
		PString strMsgId;
		strMsgId.appendUint64(id);
		PTRACE5("HgMessageData: id=%s pn=%d rf=%u f=%u", strMsgId.c_str(), pinnedPos, roleFlags, flags);
		PTRACE5("'%s'", content.c_str());
	}
};

//////////////////////////////////////////////////////////////////////////
#endif //HgDbmSharedCli_h_included
