#ifndef	stathistory2_h_included
#define	stathistory2_h_included

#include "commmsgbody.h"
#include "table.h"
#include "pyrtime.h"
#include "statbase.h"
#include "i18n.localutil.h"
#include "tournFreeBet.h"

enum statsRequestType
{
	Non_defined = 0,
	Hands_stats,
	N_tourns_stats,
	ID_tourn_stats
};

//*********** struct TournParticipant **********************************************
struct TournParticipant
{
	PString user;
	PString city;
	PString country;
	bool showCity;
	INT32 amount;
	UINT32 nextTournID; // if got a ticket to the next 
	UINT32 place;
	PString ticketAdmission;
	UINT32 tournFlags; // PYR-59874

	struct Reentry // PYR-24444
	{
		UINT16 numEntry;
		UINT32 place;
		SrvTime eliminated;

		Reentry() : numEntry( 0 ), place( 0 ) {}
		Reentry( UINT16 numEntry_, UINT32 place_ ) : numEntry( numEntry_ ), place( place_ ) {}
	};
	vector<Reentry> reentries;

	TournParticipant() 
		: showCity(false)
		, amount(0)
		, nextTournID(0)
		, place(0)
		, tournFlags(0) 
	{}

	static void composeVector( CommMsgBody& body, const vector<TournParticipant>& v );
	static void parseVector( vector<TournParticipant>& v, CommMsgParser& parser );
};

struct TournParticipantStat
{
	const TournParticipant* tournParticipant;
	const int entryInd;

	TournParticipantStat(const TournParticipant* tournParticipant_, const int entryInd_)
		: tournParticipant(tournParticipant_), entryInd(entryInd_)
	{}
};

struct TournParticipantStatLess
{
	bool operator()(const TournParticipantStat& a, const TournParticipantStat& b) const;
};

class TournParticipantStats : public set<TournParticipantStat, TournParticipantStatLess>
{
public:
	void insertParticipants(const vector<TournParticipant>& tournParticipants);
};

struct OneParticipantStatLess
{
	bool operator()(const TournParticipantStat& a, const TournParticipantStat& b) const;
};

typedef set<TournParticipantStat, OneParticipantStatLess> OneParticipantStat;

//*********** struct TournLeaderPoints **********************************************
struct TournLeaderPoints
{
	UINT32 tournId;
	BYTE gameType;
	BYTE isHiLo;
	BYTE structure;
	UINT32 place;
	UINT32 points;
	UINT32 entrants;
	UINT32 buyIn;
	UINT32 fppBuyIn;
	SrvTime start;

	TournLeaderPoints()
	{
		tournId = 0;
		gameType = 0;
		isHiLo = 0;
		structure = 0;
		place = 0;
		points = 0;
		entrants = 0;
		buyIn = 0;
		fppBuyIn = 0;
	}

	static void composeVector(CommMsgBody& body, const vector<TournLeaderPoints>& v)
	{
		body.composeUINT32( v.size() );
		for( size_t i = 0; i < v.size(); ++i )
		{
			body.
				composeUINT32( v[ i ].tournId ).
				composeBYTE( v[ i ].gameType ).
				composeBYTE( v[ i ].isHiLo ).
				composeBYTE( v[ i ].structure ).
				composeUINT32( v[ i ].place ).
				composeUINT32( v[ i ].points ).
				composeUINT32( v[ i ].entrants ).
				composeUINT32( v[ i ].buyIn ).
				composeUINT32( v[ i ].fppBuyIn ).
				composeSrvTime( v[ i ].start );
		}
	}

	static void parseVector(vector<TournLeaderPoints>& v, CommMsgParser& parser)
	{
		UINT32 nTourneys;
		parser.parseUINT32( nTourneys );
		v.reserve( nTourneys + v.size() );
		for( size_t i = 0; i < nTourneys; ++i )
		{	
			v.insert( v.end(),TournLeaderPoints() );
			parser.
				parseUINT32( v[ i ].tournId ).
				parseBYTE( v[ i ].gameType ).
				parseBYTE( v[ i ].isHiLo ).
				parseBYTE( v[ i ].structure ).
				parseUINT32( v[ i ].place ).
				parseUINT32( v[ i ].points ).
				parseUINT32( v[ i ].entrants ).
				parseUINT32( v[ i ].buyIn ).
				parseUINT32( v[ i ].fppBuyIn ).
				parseSrvTime( v[ i ].start );
		}
	}
};

//*********** struct TournLeadersBoard **********************************************
struct TournLeadersBoard
	{
	PString player;
	UINT32 points;
	UINT32 intId;
	TournLeadersBoard()
		{
		points = 0;
		intId = 0;
		}

	static void parseVector( vector<TournLeadersBoard>& v, CommMsgParser& parser );
	static void composeVector( const vector<TournLeadersBoard>& v, CommMsgBody& msg );
	};

//*************** class TournHeader **********************************************
class TournHeader
 {
 public:
	BYTE 
		game, 
		hiLo, 
		structure,
		isPlayMoney;

	UINT32 
		tournID;
	INT32 // PYR-38436
		buyIn,
		rake,
		knockout, // [AO 2010-03-24] PYR-12954
		prizeAddOn;
	UINT32 satelliteTarget;
	INT32 // PYR-38436
		satelliteBuyIn,
		satelliteRake,
		prizePool;
	UINT32 players;
	INT32 fppBuyIn;
	UINT32
		awardSchema,
		tournFlags;
	INT32 // PYR-38436
		displayBuyIn, //PYR-20351
		displayRake;  //PYR-20351	

	PString	currency;
	PString satelliteCurrency;
	SrvTime	started;
	SrvTime	finished;
	INT32	scalePM; // PYR-25238
	UINT32 totalNextTourn;

	bool askTicket;
	INT64 ticketPrice;
	PString customPrizes;
	INT32 playerKnockoutAddOn; // PYR-42834
	INT32 totalKnockoutAddOn; // PYR-42834
	FreeBetPropData freeBetReward;  // PYR-59874

	TournHeader()
		{
		game = 0; 
		hiLo = 0; 
		structure = 0;
		isPlayMoney = 0;

		tournID = 0;
		buyIn = 0;
		knockout = 0;
		fppBuyIn = 0;
		rake = 0; 
		prizeAddOn = 0;
		satelliteTarget = 0; 
		satelliteBuyIn = 0; 
		satelliteRake = 0;

		players = 0;
		prizePool = 0;
		awardSchema = 0;
		tournFlags = 0x0;

		displayBuyIn = 0;
		displayRake = 0;
		scalePM = 1; // PYR-25238
		totalNextTourn = 0;

		askTicket = false;
		ticketPrice = 0;
		playerKnockoutAddOn = 0; // PYR-42834
		totalKnockoutAddOn = 0; // PYR-42834
		}
	void compose(CommMsgBody& body) const;
	void parse(CommMsgParser& parser);
 };

// PYR-25830
class AddonRebuyTournStatsInfo
{
public:
	UINT32	numAddons;
	UINT32	numRebuys;
	INT32	addonPriceWithRake;
	INT32	rebuyPriceWithRake;
	INT32   totalBountiesNumber,  // PYR-20821
		    totalBountiesMoney;

	AddonRebuyTournStatsInfo() :  numAddons( 0 )
								, numRebuys( 0 )
								, addonPriceWithRake( 0 )
								, rebuyPriceWithRake( 0 )
								, totalBountiesNumber( 0 )
								, totalBountiesMoney( 0 )

	{
	}
	AddonRebuyTournStatsInfo( CommMsgParser& parser )
	{
		parse( parser );
	}

	CommMsgBody& compose( CommMsgBody& body ) const
	{
		body.composeUINT32( numAddons )
			.composeUINT32( numRebuys )
			.composeINT32( addonPriceWithRake )
			.composeINT32( rebuyPriceWithRake )
			.composeINT32 ( totalBountiesNumber )
			.composeINT32 ( totalBountiesMoney )
			;
		return body;
	}
	CommMsgParser& parse( CommMsgParser& parser )
	{
		parser
			.parseUINT32( numAddons )
			.parseUINT32( numRebuys )
			.parseINT32( addonPriceWithRake )
			.parseINT32( rebuyPriceWithRake )
			.parseINT32 ( totalBountiesNumber )
			.parseINT32 ( totalBountiesMoney )
			;
		return parser;
	}
};

/*
void formatTournStatHeader( 
	const char* userId, 
	const char* userEmail, 
	PString& stat,			// string to format
	BYTE requestType,		// particular tournId or N last tourns
	UINT32 tournIdOrNumTourns,	// Id of particular tournament OR number of tourns requested( if N last tourns )
	UINT32 numTourns		// number of tourns returned( in any case )
	);
*/

struct HeaderFormatOption
{
	HeaderFormatOption()
		: ignoreFreeBet( false )
	{}
	bool ignoreFreeBet;
};

void formatTournStats( PString& ret, UINT32 locale, UINT32 brandId, UINT32 timezone, const TournHeader& hdr, const Statistics& stats, UINT64 handId64, BYTE flags, bool noPlayerStats, const char* name, const char* email, const vector< TournParticipant >& players, bool showTournInProgress = true );
void formatTournStatFooter( UINT32 locale, PString& ret, const char* site = NULL );

void formatStatSubject( PString& ret, UINT32 locale, UINT32 brandId, BYTE requestType, UINT32 requestContents );
void formatGameHistory( PString& ret, UINT32 locale, const Statistics& stats);
void formatStatHistory( PString& ret, UINT32 locale, UINT32 brandId, const Statistics& stats, UINT32 numRequestedHands, const char* name, const char* email, const char* site );
void formatTournStatHeader( PString& ret, UINT32 locale, UINT32 brandId, const char* userId, const char* userEmail, BYTE requestType, UINT32 numRequested, UINT32 requestContents );

void formatTournHeaderEx( PString& ret, UINT32 locale, UINT32 brandId, UINT32 timezone, const TournHeader& hdr, const HeaderFormatOption& info );
void formatStatHeader( PString& ret, UINT32 locale, UINT32 brandId, const char* userId, const char* userEmail, UINT32 numRequested, UINT32 requestContents, statsRequestType requestType);

inline void formatTournHeader( PString& ret, UINT32 locale, UINT32 brandId, UINT32 timezone, const TournHeader& hdr )
{
	HeaderFormatOption defaultInfo;
	formatTournHeaderEx( ret, locale, brandId, timezone, hdr, defaultInfo );
}

//------------------------------------------------------------------------------
// Old definitions with default brand for admin/cgi/cgi.vcxproj
//------------------------------------------------------------------------------
inline void formatTournStats( UINT32 locale, UINT32 timezone, PString& ret, const TournHeader& hdr, const Statistics& stats, UINT64 handId64, BYTE flags, bool noPlayerStats, const char* name, const char* email, const vector< TournParticipant >& players, bool showTournInProgress = true )
{
	formatTournStats( ret, locale, BrandType_PokerStars, timezone, hdr, stats, handId64, flags, noPlayerStats, name, email, players, showTournInProgress );
}

inline void formatStatHistory( UINT32 locale, PString& ret, const Statistics& stats, UINT32 numRequestedHands, const char* name, const char* email, const char* site )
{
	formatStatHistory( ret, locale, BrandType_PokerStars, stats, numRequestedHands, name, email, site );
}

inline void formatTournStatHeader( const char* userId, const char* userEmail, UINT32 locale, PString& stat,  BYTE requestType, UINT32 numRequested, UINT32 requestContents )
{
	formatTournStatHeader( stat, locale, BrandType_PokerStars, userId, userEmail, requestType, numRequested, requestContents );
}
//------------------------------------------------------------------------------

#endif	//stathistory2_h_included
