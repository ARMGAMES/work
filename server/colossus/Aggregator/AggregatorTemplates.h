#ifndef AggregatorTemplates_h_included
#define AggregatorTemplates_h_included

////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2012 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

template <typename KeyType>
void SessionMap<KeyType>::addHand(KeyType const& key, UINT64 sessionId, SrvTime const& finished)
{
	(*this)[key][sessionId].insert(finished);
}


template <typename KeyType>
void SessionMap<KeyType>::calcSingleSessionData(const KeyType& key, const SessionDataMap& sessionDataMap, 
												SingleSessionDataVec& singleSessions) const
{
	SingleSessionData& lastSession = *singleSessions.insert(singleSessions.end(), SingleSessionData());

	EventTimeSet eventTimes;
	sessionDataMap.getEventTimes(eventTimes);

	lastSession.sessionStartEndTimes.timeFrom = *(eventTimes.begin());
	EventTimeSet::iterator lastEventTimeIt = eventTimes.end();
	lastSession.sessionStartEndTimes.timeTo = *(--lastEventTimeIt);

	lastSession.maxInstances = (key.sessionType == COLOSSUS_SESSIONTYPE_BLITZ) ? 0 : sessionDataMap.getMaxNumInst(eventTimes);
}


template <typename KeyType>
void SessionMap<KeyType>::calcSessionData(map<KeyType, SingleSessionDataVec>& keyToSingleSessions, 
										  TimeIntervalVec aggIntervals[])
{
	for (iterator mapIt = begin(); mapIt != end(); ++mapIt)
	{
		vector<SessionDataMap> sessionsVec; // Aggregate all of these over all keys to get aggregate maxInst result
		const KeyType& key = mapIt->first;
		populateSessionDataMaps(key, mapIt->second, sessionsVec);

		keyToSingleSessions.insert(make_pair(key, SingleSessionDataVec()));
		SingleSessionDataVec& singleSessions = keyToSingleSessions[key];

		for (UINT32 i = 0; i < sessionsVec.size(); ++i)
		{
			// Single session data calculations
			SessionDataMap& sessionDataMap = sessionsVec[i];
			calcSingleSessionData(key, sessionDataMap, singleSessions);

			// Aggregate session data calculations
			AggregateSessionGameType aggSessionGameType = key.getAggSessionGameType();
			if (aggSessionGameType == COLOSSUS_AGGSESSIONGAMETYPE_INVALID)
				throw PError("Invalid session game type"); // TODO cleanup

			TimeIntervalVec& intervals = aggIntervals[aggSessionGameType];
			sessionDataMap.appendIntervals(intervals);
		}
	}
}


template <typename KeyType>
void SessionMap<KeyType>::populateSessionDataMaps(KeyType const& key, SessionValMap const& valMap,
												  vector<SessionDataMap>& sessionsVec) const
{
	UINT32 const maxSessionBreakSecs = key.maxSessionBreak * 60;

	// Pairs of (session id, hand finished time)
	SessionHandFinishedPairVec handPairVec;

	for (SessionValMap::const_iterator valMapIt = valMap.begin(); valMapIt != valMap.end(); ++valMapIt)
	{
		EventTimeSet const& eventTimeSet = valMapIt->second;

		for (EventTimeSet::const_iterator timeIt = eventTimeSet.begin(); timeIt != eventTimeSet.end(); ++timeIt)
			handPairVec.push_back(make_pair(valMapIt->first, *timeIt));
	}

	// Ensure that all hands are sorted in order by finished time
	sort(handPairVec.begin(), handPairVec.end(), SrvTimeCmpLess());

	SrvTime prevFinished;
	for (size_t i = 0; i < handPairVec.size(); ++i)
	{
		UINT64 sessionId = handPairVec[i].first;
		SrvTime const& finished = handPairVec[i].second;

		if (finished.encode() - prevFinished.encode() > maxSessionBreakSecs)
			sessionsVec.push_back(SessionDataMap());

		SessionDataMap& sessionDataMap = *(sessionsVec.end() - 1);
		sessionDataMap.addHand(sessionId, key.maxSessionTableBreak,	key.sessionType, finished);

		prevFinished = finished;
	}
}

////////////////////////////////////////////////////////////////////////////////


#endif // AggregatorTemplates_h_included
