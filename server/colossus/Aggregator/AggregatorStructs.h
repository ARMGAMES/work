#ifndef AggregatorStructs_h_included
#define	AggregatorStructs_h_included

////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2012 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "CopyablePStringMap.h"
#include "pyrtime.h"
#include "licenseid.h"
#include "../utils.h"
#include "../AggregatorShared.h"

class PIOFile;

////////////////////////////////////////////////////////////////////////////////

class SrvTimeCmpLess
{
public:
	bool operator() (SrvTime const& left, SrvTime const& right) const
	{
		return left.compareTo(right) < 0;
	}

	bool operator() (pair<UINT64, SrvTime> const& left, pair<UINT64, SrvTime> const& right) const
	{
		return left.second.compareTo(right.second) < 0;
	}
};

////////////////////////////////////////////////////////////////////////////////

typedef pair<time_t, time_t> TimeInterval;
typedef vector<TimeInterval> TimeIntervalVec;
typedef set<SrvTime, SrvTimeCmpLess> EventTimeSet;
typedef map<UINT64 /* sessionId (tableId, tournId, blitzPoolId) */, EventTimeSet> SessionValMap;
typedef vector< pair<UINT64 /* sessionId */, SrvTime /* handFinishedTime */ > > SessionHandFinishedPairVec;

////////////////////////////////////////////////////////////////////////////////

class TimeIntervalCmpLess
{
public:
	bool operator() (const TimeInterval& left, const TimeInterval& right) const
	{
		return left.first < right.first;
	}
};

////////////////////////////////////////////////////////////////////////////////

// Map sessionId to a vector of time intervals
class SessionDataMap : private map<UINT64, TimeIntervalVec>
{ //lint --e{1509}
private:
	typedef map<UINT64, TimeIntervalVec> Parent_;

public:
	using Parent_::clear;

	void getEventTimes(EventTimeSet& eventTimeSet) const;

	// gets the maximum number of instances that the user played in over the entire map
	UINT32 getMaxNumInst(EventTimeSet const& eventTimes) const;

	// Adds a hand to the map.  
	// Depending on the current contents of the map, "maxRingSessionBreakTime", and "finished",
	// this hand may either extend one of the intervals in the map or create a new interval in the map
	void addHand(UINT64 sessionId, UINT32 maxRingSessionBreakTime, UINT32 sessionType, SrvTime const& finished);

	// Appends the intervals contained in the map to the given intervals
	void appendIntervals(TimeIntervalVec& sessionIntervals) const;
};

////////////////////////////////////////////////////////////////////////////////

//lint -esym(1509, SessionKey) -esym(1512, SessionKey)
class SessionKey
{ //lint --e{1925}
public:
	UINT32 sessionType;
	PString userId;
	UINT32 structure;
	PString currency;
	UINT32 maxPerTable;
	UINT32 hostId;

	UINT32 maxSessionBreak;
	UINT32 maxSessionTableBreak;

	virtual AggregateSessionGameType getAggSessionGameType() const = 0;

protected:
	SessionKey(UINT32 maxSessionBreak_, UINT32 maxSessionTableBreak_) :
		sessionType(0), structure(0), maxPerTable(0), hostId(eHost_Unknown),
		maxSessionBreak(maxSessionBreak_), maxSessionTableBreak(maxSessionTableBreak_)
	{}

	int compare(SessionKey const& other) const;
};

////////////////////////////////////////////////////////////////////////////////

class TournsSessionKey : public SessionKey
{ //lint --e{1925}
public:
	UINT32 tournGameType;
	INT32 buyIn;
	UINT32 tournSchedulingType;
	UINT32 tournMaxEntry;
	UINT64 flags;
	UINT32 tournActualEntries;

	TournsSessionKey(UINT32 maxSessionBreak_, UINT32 maxSessionTableBreak_) :
		SessionKey(maxSessionBreak_, maxSessionTableBreak_),
		tournGameType(0), buyIn(-1), tournSchedulingType(0), tournMaxEntry(0), flags(0), tournActualEntries(0) 
	{}

	bool operator==(TournsSessionKey const& other) const	{ return compare(other) == 0; }
	bool operator<(TournsSessionKey const& other) const		{ return compare(other) < 0; }

	void toString(UINT32 maxInstances, PString& ret) const;

	virtual AggregateSessionGameType getAggSessionGameType() const;

private:
	int compare(TournsSessionKey const& other) const; //lint !e1511 : hiding
};

////////////////////////////////////////////////////////////////////////////////

// Used for RING and ZOOM/BLITZ
class HandsSessionKey : public SessionKey
{ //lint --e{1925}
public:
	UINT32 gameType;
	UINT32 bigBlind;
	UINT32 smallBlind;

	HandsSessionKey(UINT32 maxSessionBreak_, UINT32 maxSessionTableBreak_) :
	SessionKey(maxSessionBreak_, maxSessionTableBreak_),
		gameType(0), bigBlind(0), smallBlind(0) 
	{}

	bool operator==(HandsSessionKey const& other) const		{ return compare(other) == 0; }
	bool operator<(HandsSessionKey const& other) const		{ return compare(other) < 0; }

	void toString(UINT32 maxInstances, PString& ret) const;

	virtual AggregateSessionGameType getAggSessionGameType() const;

private:
	int compare(HandsSessionKey const& other) const; //lint !e1511 : hiding
};

////////////////////////////////////////////////////////////////////////////////

// A collection of data required to write a single session
struct SingleSessionData
{
	UINT32 maxInstances;
	SrvInterval sessionStartEndTimes;

	SingleSessionData() : maxInstances(0) { }
};

typedef vector<SingleSessionData> SingleSessionDataVec;
typedef map<TournsSessionKey, SingleSessionDataVec> TournsSingleSessionMap;
typedef map<HandsSessionKey, SingleSessionDataVec> HandsSingleSessionMap;

////////////////////////////////////////////////////////////////////////////////

template <typename KeyType> // Implementation in AggregatorTemplates.h
class SessionMap : private map<KeyType, SessionValMap>
{
private:
	typedef map<KeyType, SessionValMap> Parent_;

public:
	using Parent_::clear;

public:
	void addHand(KeyType const& key, UINT64 sessionId, SrvTime const& finished);

	void calcSingleSessionData(const KeyType& key, const SessionDataMap& sessionDataMap, SingleSessionDataVec& singleSessions) const;

	// Calculates the session data to write to file
	void calcSessionData(map<KeyType, SingleSessionDataVec>& keyToSingleSessions, TimeIntervalVec aggIntervals[]);

private:
	// transforms the map into a "SessionDataMap" that can be used to write session data to file
	void populateSessionDataMaps(KeyType const& key, SessionValMap const& valMap, vector<SessionDataMap>& sessionsVec) const;
};


typedef SessionMap<TournsSessionKey> TournSessionMap;
typedef SessionMap<HandsSessionKey> HandSessionMap;

////////////////////////////////////////////////////////////////////////////////

struct HandPartnerData
{
	UINT32 numHandsWithPartner;
	INT32 totalProfitWithPartner;
	UINT32 numHandsProfitFromPartner;
	INT32 totalProfitFromPartner;
	UINT32 allInConfrontations;
	UINT32 showdownAllInConfrontations;
	UINT32 hostId;

	HandPartnerData() :
		numHandsWithPartner(0), totalProfitWithPartner(0), numHandsProfitFromPartner(0), totalProfitFromPartner(0),
		allInConfrontations(0), showdownAllInConfrontations(0), hostId(eHost_Unknown)
	{}
};


class HandPartnersMap : public PStringMap<HandPartnerData>
{
public:
	HandPartnerData& getData(char const* name);
};

////////////////////////////////////////////////////////////////////////////////

struct TournPartnerData
{
	UINT32 allInConfrontations;
	UINT32 showdownAllInConfrontations;

	TournPartnerData() : allInConfrontations(0), showdownAllInConfrontations(0){}
};


class TournPartnerDataMap : public CopyablePStringMap<TournPartnerData>
{
public:
	TournPartnerData& getData(char const* name);
};


typedef map<UINT32 /*tournId*/, TournPartnerDataMap> TournPartnersMap;

////////////////////////////////////////////////////////////////////////////////

struct ConsistencyKey
{
	PString	currency;
	UINT32	maxPerTable;
	UINT32	gameType;
	UINT32	structure;
	UINT32	bigBlind;
	UINT32	position;
	PString	holeCards;

	ConsistencyKey() : maxPerTable(0), gameType(0), structure(0), bigBlind(0), position(0) {}

	bool operator==(ConsistencyKey const& other) const		{ return compare(other) == 0; }
	bool operator<(ConsistencyKey const& other) const		{ return compare(other) < 0; }

private:
	int compare(ConsistencyKey const& other) const;
};


struct ConsistencyData
{
	UINT32	numHands;
	UINT32	numVpip;
	UINT32	numHandsRaisedPreflop;
	UINT32	numColdCallOpportunities;
	UINT32	numColdCalls;

	ConsistencyData() :
		numHands(0), numVpip(0), numHandsRaisedPreflop(0), numColdCallOpportunities(0), numColdCalls(0)
	{}
};


class ConsistencyMap : public map<ConsistencyKey, ConsistencyData>
{
	typedef map<ConsistencyKey, ConsistencyData> Parent_;

public:
	ConsistencyMap() : numHands_(0)	{}
	void clear()					{ numHands_ = 0; Parent_::clear(); } //lint !e1511
	void addHand()					{ ++numHands_; }
	UINT32 numHands() const			{ return numHands_; }

private:
	UINT32 numHands_;
};

////////////////////////////////////////////////////////////////////////////////

// A sorted vector seems more efficient than a set
// TODO: Consider changing to unordered_set for VC11
class CompletedTourns : private vector<UINT32>
{
	typedef vector<UINT32> Parent_;

public:
	using Parent_::size;

	void add(UINT32 tournId)		{ push_back(tournId); }
	bool find(UINT32 tournId) const	{ return std::binary_search(begin(), end(), tournId); }
	void sort()						{ std::sort(begin(), end()); }
};

////////////////////////////////////////////////////////////////////////////////

#endif // AggregatorStructs_h_included
