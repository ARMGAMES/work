////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2012 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "AggregatorStructs.h"
#include "ColossusConstants.h"
#include "ppiofile.h"
#include "shared.h"

////////////////////////////////////////////////////////////////////////////////

void SessionDataMap::getEventTimes(EventTimeSet& eventTimeSet) const
{
	SrvTime startTime, endTime;
	for (SessionDataMap::const_iterator mapIt = begin(); mapIt != end(); ++mapIt)
	{
		const TimeIntervalVec& intervalVec = mapIt->second;
		for (TimeIntervalVec::const_iterator it = intervalVec.begin(); it != intervalVec.end(); ++it)
		{
			const TimeInterval& interval = *it;
			startTime.decode(interval.first);
			endTime.decode(interval.second);

			eventTimeSet.insert(startTime);
			eventTimeSet.insert(endTime);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////

UINT32 SessionDataMap::getMaxNumInst(EventTimeSet const& eventTimes) const
{
	UINT32 maxNumInstances = 0;

	for (EventTimeSet::const_iterator eventIt = eventTimes.begin(); eventIt != eventTimes.end(); ++eventIt)
	{
		time_t rawEventTime = eventIt->encode();
		UINT32 numInstances = 0;

		for (const_iterator mapIt = begin(); mapIt != end(); ++mapIt)
		{
			const TimeIntervalVec& intervalVec = mapIt->second;
			for (TimeIntervalVec::const_iterator it = intervalVec.begin(); it != intervalVec.end(); ++it)
			{
				const TimeInterval& interval = *it;
				if (rawEventTime >= interval.first && rawEventTime <= interval.second)
				{
					++numInstances;
					break;
				}
			}
		}

		if (numInstances > maxNumInstances)
			maxNumInstances = numInstances;
	}

	return maxNumInstances;
}

////////////////////////////////////////////////////////////////////////////////

void SessionDataMap::addHand(UINT64 sessionId, UINT32 maxRingSessionBreakTime, UINT32 sessionType, SrvTime const& finished)
{
	time_t finishTime = finished.encode();
	TimeInterval interval(finishTime, finishTime);

	// This will create a new, empty interval if the 'sessionId' has not yet been encountered
	TimeIntervalVec& intervalVec = (*this)[sessionId];

	if (intervalVec.empty())
		intervalVec.push_back(interval);

	// Found existing interval for this session id
	else 
	{
		TimeInterval& lastInterval = *(intervalVec.end() - 1);

		// Check if this hand is the start of a new session with the same session id
		if ((sessionType == COLOSSUS_SESSIONTYPE_RING) &&
			(finishTime - lastInterval.second > static_cast<time_t>(maxRingSessionBreakTime) * SECONDS_IN_A_MINUTE))
		{
			intervalVec.push_back(interval);
		}

		// This hand is a continuation of the previous session
		else
			lastInterval.second = finishTime;
	}
}


void SessionDataMap::appendIntervals(TimeIntervalVec& sessionIntervals) const
{
	for (const_iterator it = begin(); it != end(); ++it)
	{
		const TimeIntervalVec& intervals = it->second;
		sessionIntervals.insert(sessionIntervals.end(), intervals.begin(), intervals.end());
	}
}

////////////////////////////////////////////////////////////////////////////////

int SessionKey::compare(SessionKey const& other) const
{
	int res = PString::compare(userId, other.userId);
	if (res)
	{
		return res;
	}

	res = int(structure) - int(other.structure);
	if (res)
	{
		return res;
	}

	res = PString::compare(currency, other.currency);
	if (res)
	{
		return res;
	}

	res = int(maxPerTable) - int(other.maxPerTable);
	if (res)
	{
		return res;
	}

	res = int(hostId) - int(other.hostId);
	if (res)
	{
		return res;
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////

void TournsSessionKey::toString(UINT32 maxInstances, PString& ret) const
{
	ret.
		appendUint(sessionType).
		append(',').appendUint(tournGameType).
		append(',').appendUint(structure).
		append(',').appendUint(buyIn).
		append(',').append('"').append(currency).append('"').
		append(',').appendUint(maxPerTable).
		append(',').appendUint(tournSchedulingType).
		append(',').appendUint(tournMaxEntry).
		append(',').appendUint(maxInstances).
		append(',').appendUint64(flags).
		append(',').appendUint(hostId).
		append(',').appendUint(tournActualEntries);
}


int TournsSessionKey::compare(TournsSessionKey const& other) const
{
	int res = SessionKey::compare(other);
	if (res)
	{
		return res;
	}

	res = int(tournGameType) - int(other.tournGameType);
	if (res)
	{
		return res;
	}

	res = buyIn - other.buyIn;
	if (res)
	{
		return res;
	}

	res = int(tournSchedulingType) - int(other.tournSchedulingType);
	if (res)
	{
		return res;
	}

	res = int(tournMaxEntry) - int(other.tournMaxEntry);
	if (res)
	{
		return res;
	}

	INT64 res64 = INT64(flags) - INT64(other.flags);
	if (res64)
	{
		return res64 > INT64(0) ? 1 : -1;
	}

	res = int(tournActualEntries) - int(other.tournActualEntries);
	if (res)
	{
		return res;
	}

	return 0;
}


AggregateSessionGameType TournsSessionKey::getAggSessionGameType() const
{
	if (tournSchedulingType == COLOSSUS_TOURNSCHEDULINGTYPE_SNG)
	{
		return COLOSSUS_AGGSESSIONGAMETYPE_SNG;
	}
	else if (tournSchedulingType == COLOSSUS_TOURNSCHEDULINGTYPE_MTT)
	{
		return COLOSSUS_AGGSESSIONGAMETYPE_MTT;
	}
	else
	{
		return COLOSSUS_AGGSESSIONGAMETYPE_INVALID;
	}
}

////////////////////////////////////////////////////////////////////////////////

void HandsSessionKey::toString(UINT32 maxInstances, PString& ret) const
{
	ret.
		appendUint(sessionType).
		append(',').appendUint(gameType).
		append(',').appendUint(structure).
		append(',').appendUint(bigBlind).
		append(',').append('"').append(currency).append('"').
		append(',').appendUint(maxPerTable).
		append(',').appendUint(0 /* tournSchedulingType */).
		append(',').appendUint(0 /* tournMaxEntry */).
		append(',').appendUint(maxInstances).
		append(',').appendUint64(UINT64(0) /* flags */).
		append(',').appendUint(hostId).
		append(',').appendUint(0 /* tournActualEntries*/);
}


int HandsSessionKey::compare(HandsSessionKey const& other) const
{
	int res = SessionKey::compare(other);
	if (res)
	{
		return res;
	}

	res = int(gameType) - int(other.gameType);
	if (res)
	{
		return res;
	}

	res = int(bigBlind) - int(other.bigBlind);
	if (res)
	{
		return res;
	}

	res = int(smallBlind) - int(other.smallBlind);
	if (res)
	{
		return res;
	}

	return 0;
}


AggregateSessionGameType HandsSessionKey::getAggSessionGameType() const
{
	if (sessionType == COLOSSUS_SESSIONTYPE_RING)
		return COLOSSUS_AGGSESSIONGAMETYPE_RING;
	else if (sessionType == COLOSSUS_SESSIONTYPE_BLITZ)
		return COLOSSUS_AGGSESSIONGAMETYPE_ZOOM;
	else
		return COLOSSUS_AGGSESSIONGAMETYPE_INVALID;
}

////////////////////////////////////////////////////////////////////////////////

HandPartnerData& HandPartnersMap::getData(char const* name)
{
	iterator it = find(name);
	if (it == end())
		it = insert(value_type(name, mapped_type())).first;
	return it->second;
}

////////////////////////////////////////////////////////////////////////////////

TournPartnerData& TournPartnerDataMap::getData(char const* name)
{
	iterator it = find(name);
	if (it == end())
		it = insert(value_type(name, mapped_type())).first;
	return it->second;
}

////////////////////////////////////////////////////////////////////////////////

int ConsistencyKey::compare(ConsistencyKey const& other) const
{
	int res = int(position) - int(other.position);
	if (res)
		return res;

	res = PString::compare(holeCards, other.holeCards);
	if (res)
		return res;

	res = int(bigBlind) - int(other.bigBlind);
	if (res)
		return res;

	res = int(structure) - int(other.structure);
	if (res)
		return res;

	res = int(gameType) - int(other.gameType);
	if (res)
		return res;

	res = int(maxPerTable) - int(other.maxPerTable);
	if (res)
		return res;

	res = PString::compare(currency, other.currency);
	if (res)
		return res;

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
