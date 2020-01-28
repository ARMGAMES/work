//------------------------------------------------------------------------------
//	SeatFinderShared.h - SeatFinder and Blitz definitions
//------------------------------------------------------------------------------
//
//	Copyright (c) 2001-2019 PYR Software Ltd. All rights reserved. This document
//	is for the use of licensed users only and is subject to the terms and
//	conditions of the accompanying and/or applicable license between you and
//	PYR Software Ltd, including without limitation those terms and conditions
//	prohibiting unauthorized translation, reverse engineering, decompilation,
//	disassembly, and other inappropriate use of any software provided. Any
//	unauthorized use, copying, modification, distribution, publication or
//	disclosure of information is a violation of copyright laws. No reproduction
//	in whole or in part of this document may be made without express written
//	consent of PYR Software Ltd.
//
//
//------------------------------------------------------------------------------

#if !defined( __Seat_Finder_Shared_h_included__ )
#define __Seat_Finder_Shared_h_included__

//------------------------------------------------------------------------------

#include "commmsgbody.h"

//------------------------------------------------------------------------------
#if defined( PROTOCOL_CLIENT_ONLY )
//------------------------------------------------------------------------------

#include "storm/TableCommonStructs.h"

//------------------------------------------------------------------------------
#else // defined( PROTOCOL_CLIENT_ONLY )
//------------------------------------------------------------------------------

#include "storm/TableCommonStructs_thin.h"

//------------------------------------------------------------------------------
#endif // defined( PROTOCOL_CLIENT_ONLY )
//------------------------------------------------------------------------------

struct EarlyBirdMultiplier
{
	UINT32 licenseId = 0;
	UINT32 multiplier = 0;

	void parse(CommMsgParser& parser)
	{
		parser.parseUINT32(licenseId)
			.parseUINT32(multiplier);
	}

	void compose(CommMsgBody& msg) const
	{
		msg.composeUINT32(licenseId)
			.composeUINT32(multiplier);
	}
};

struct SeatFinderBase
{
	virtual ~SeatFinderBase() {}

	// static data
	UINT32			id;
	BYTE			game;
	BYTE			structure;
	BYTE			isHiLo;
	BYTE			maxPlayersPerTable;
	INT32			loBet;
	INT32			hiBet;
	INT32			ante;
	INT32			cap;
	PString			currency;
	INT32			minChipsLimit;
	INT32			maxBuyIn;
	UINT32			visibilityMask;
	PSiteMask		visibilityMaskEx;
	INT32			defaultBuyIn;
	PString			specialName;
	bool			countryRestrictionInverted;
	vector<PString>	restrictedCountries;
	BYTE			isPlayMoney;
	UINT8			morphLobbyMask;
	UINT32			scalePM;
#if defined( PROTOCOL_CLIENT_ONLY )
	TableCommon::TableStakes		tableStakes;
#else
	TableCommonThin::TableStakes	tableStakes;
#endif

	// dynamic data
	INT32			handsPerHour;
	INT32			avgPotSize;
	INT32			avgStackSize;
	INT32			numPlayers;

	SeatFinderBase();

	virtual bool isBlitz() const = 0;
	virtual void parseFromLobby( CommMsgParser& parser ) = 0;
	virtual void parseFromLobbyDyna( CommMsgParser& parser ) = 0;
	virtual void composeStatMsg( CommMsgBody& msg ) const = 0;
	virtual void composeDynaMsg( CommMsgBody& msg ) const = 0;

	virtual void composeForMobile( CommMsgBody& body ) const = 0;
	virtual void parseForMobile( CommMsgParser& parser ) = 0;
};

struct ShadowSeatFinder : public SeatFinderBase
{
	ShadowSeatFinder()
		: minPlayersPerTable(0), isPasswordProtected(false), poolBlockMinHands(0)
		, numTables(0), numFullTables(0), flags(0), flags2(0) {}

	// static data
	PString replayerAddress;
	PString replayerInstance;
	INT8 minPlayersPerTable;
	bool isPasswordProtected;
	INT32 poolBlockMinHands;
	vector<INT32> poolBlockTimePenalty;
	UINT32 flags;
	UINT64 flags2;
	vector<INT64>	variableAntes; // scaled by scalePM already

	// dynamic data
	INT32 numTables;
	INT32 numFullTables;

	bool dynaEqualForUpdate( const ShadowSeatFinder& other ) const;
	bool statEqualForUpdate( const ShadowSeatFinder& other ) const;

	bool isBlitz() const override { return false; }
	void parseFromLobby( CommMsgParser& parser ) override;
	void parseFromLobbyDyna( CommMsgParser& parser ) override;
	void composeStatMsg( CommMsgBody& msg ) const override;
	void composeDynaMsg( CommMsgBody& msg ) const override;

	// for mobile
	void composeForMobile( CommMsgBody& body ) const override;
	void parseForMobile( CommMsgParser& parser ) override;
};

struct ShadowBlitzForMobile : public SeatFinderBase
{
	ShadowBlitzForMobile();
	ShadowBlitzForMobile( const ShadowBlitzForMobile& rh );
	ShadowBlitzForMobile& operator =(const ShadowBlitzForMobile&) = delete;

	// static data
	PString server;
	PString name;
	PString replayerInstance;
	BYTE isOneOnOne;
	UINT32 tableFlags;
	UINT64 tableFlags2;
	UINT16 chatLang;
	bool quickLobbyVisible; // #22042
	UINT32 blitzFlags;
	UINT32 brandMask; // PYR-40190

	// dynamic data
	BYTE publProp;
	BYTE avgPlayers;
	BYTE avgVpip;
	CommMsgBody fppFactorsBody;
	CommMsgBody fppFactorsBodyEx;
	INT32 earlyBirdMins = 0; // PYR-114793
	vector<EarlyBirdMultiplier> earlyBirdMultipliers; // PYR-114793

	bool isBlitz() const override { return true; }
	void parseFromLobby( CommMsgParser& parser ) override;
	void parseFromLobbyDyna( CommMsgParser& parser ) override {}
	void composeStatMsg( CommMsgBody& msg ) const override;
	void composeDynaMsg( CommMsgBody& msg ) const override;

	// for mobile
	void composeForMobile( CommMsgBody& msg ) const override;
	void parseForMobile( CommMsgParser& parser ) override;
};

class SortedSeatFindersAndBlitzes
{
	// object is non-copyable
	SortedSeatFindersAndBlitzes( const SortedSeatFindersAndBlitzes& );
	SortedSeatFindersAndBlitzes& operator=( const SortedSeatFindersAndBlitzes& );

	vector<const SeatFinderBase*> finders;
	bool ownsFinders;
	INT32 totalNum;

public:
	// for mobile client
	~SortedSeatFindersAndBlitzes();
	SortedSeatFindersAndBlitzes() : ownsFinders( true ), totalNum( 0 ) {}
	SortedSeatFindersAndBlitzes( CommMsgParser& parser ) : ownsFinders( true ), totalNum( 0 ) { parse(parser); }

	void parse( CommMsgParser& parser );
	void parseMsg( CommMsgParser& parser ) { parse( parser ); }

	const vector<const SeatFinderBase*>& getFinders() const { return finders; }
	INT32 getTotalNum() const { return totalNum; }

	// for server use only
	SortedSeatFindersAndBlitzes( const vector<const SeatFinderBase*>& finders_, INT32 totalNum_ )
		: finders( finders_ ), ownsFinders( false ), totalNum( totalNum_ ) {}

	void compose( CommMsgBody& body ) const;

	// note: default parameter here is a hack to make this ATF compatible with both thick and thin protocols
	void composeMsg( CommMsgBody& body, bool _ignoreJSON = false ) const { compose( body ); }

	void clear();
};

//------------------------------------------------------------------------------
#endif // !defined( __Seat_Finder_Shared_h_included__ )
//------------------------------------------------------------------------------
