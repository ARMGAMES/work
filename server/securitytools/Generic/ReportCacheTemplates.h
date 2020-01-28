////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2019 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef ReportCacheTemplates_h_included
#define ReportCacheTemplates_h_included

////////////////////////////////////////////////////////////////////////////////

#include "pplogfile.h"
#include "genericutils.h"

////////////////////////////////////////////////////////////////////////////////

template <typename ReportType>
struct ReportCacheValue
{
	time_t timeLastAccessed;
	ReportType outputs;

	ReportCacheValue() : timeLastAccessed(time(nullptr))
	{ }

	explicit ReportCacheValue(const ReportType& outputs_) : timeLastAccessed(time(nullptr)), outputs(outputs_)
	{ }

	void updateTimeLastAccessed()
	{
		timeLastAccessed = time(nullptr);
	}

	void getOutputs(ReportType& outputs_) const
	{
		outputs_ = outputs;
	}
};

////////////////////////////////////////////////////////////////////////////////

struct ReportCacheOptions
{
	ReportCacheOptions(bool moreTrace_, int maxCacheSize_, int maxTimeInCacheSeconds_, const char* cacheName_) 
		: moreTrace(moreTrace_) 
		, maxCacheSize(maxCacheSize_)
		, maxTimeInCacheSeconds(maxTimeInCacheSeconds_)
		, cacheName(cacheName_)
	{ }

	void log() const
	{
		PLog("cache %s config(moreTrace: %s, maxCacheSize: %d, maxTimeInCacheSeconds: %d)",
			cacheName.c_str(), moreTrace ? "YES" : "NO", maxCacheSize, maxTimeInCacheSeconds);
	}

	bool moreTrace;
	int maxCacheSize;
	int maxTimeInCacheSeconds;
	PString cacheName;
};

////////////////////////////////////////////////////////////////////////////////

template <typename ReportInputsType, typename ReportType, typename ReportInputsPred = less<ReportInputsType>>
struct ReportCache
{
	static const int DEFAULT_MAX_TIME_IN_CACHE_SECONDS = 3600;
	static const int DEFAULT_MAX_REPORT_CACHE_SIZE_MEDIUM = 15;
	static const int DEFAULT_MAX_REPORT_CACHE_SIZE_SMALL = 5;
	static const int DEFAULT_MAX_REPORT_CACHE_SIZE_ONE = 1;

	int cacheHits;
	int cacheMisses;
	ReportCacheOptions options;

	using CacheMap = map<ReportInputsType, ReportCacheValue<ReportType>, ReportInputsPred>;
	using CacheMapIt = typename CacheMap::iterator;
	using CacheMapConstIt = typename CacheMap::const_iterator;

	CacheMap mainCache;
	map<time_t, vector<CacheMapConstIt>> auxCache;

public:
	ReportCache(bool moreTrace_, int maxCacheSize_, int maxTimeInCacheSeconds_, const char* cacheName_)
		: cacheHits(0)
		, cacheMisses(0)
		, options(moreTrace_, maxCacheSize_, maxTimeInCacheSeconds_, cacheName_)
	{ }

	void sweep()
	{
		PLog("cache %s sweep(numItems: %d)", options.cacheName.c_str(), getNumItems());

		time_t now = time(nullptr);
		auto it = auxCache.begin();
		while (it != auxCache.end())
		{
			time_t timeLastAccessed = it->first;

			INT64 timeInCacheSecs = now - timeLastAccessed;
			if (timeInCacheSecs <= options.maxTimeInCacheSeconds)
			{
				break;
			}

			vector<CacheMapConstIt>& itemVec = it->second;
			SecUtils::for_each(itemVec, [this](const CacheMapConstIt& mainCacheIt) {
				mainCache.erase(mainCacheIt);
			});

			it = auxCache.erase(it);
		}

		PLog("cache %s #sweep(numItems: %d)", options.cacheName.c_str(), getNumItems());
	}

	void logStats() const
	{
		PLog("cache %s logStats(hits: %d, misses: %d, numItems: %d)", 
			options.cacheName.c_str(), cacheHits, cacheMisses, getNumItems());
	}

	void logConfig() const
	{
		options.log();
	}

	bool get(const ReportInputsType& inputs, ReportType& outputs)
	{
		PString logStr;
		inputs.appendLog(logStr);

		if (options.moreTrace)
		{
			PLog("cache %s get(inputs: %s)", options.cacheName.c_str(), logStr.c_str());
		}

		auto mainCacheIt = mainCache.find(inputs);

		if (mainCacheIt == mainCache.end())
		{
			++cacheMisses;
			PLog("Not found");

			return false;
		}
		else
		{
			++cacheHits;
			PLog("Found");

			ReportCacheValue<ReportType>& reportCacheValue = mainCacheIt->second;
			
			// Get a *copy* of what is in the cache
			reportCacheValue.getOutputs(outputs);

			updateLastAccessTimeInAuxCache(inputs, reportCacheValue);

			return true;
		}
	}

	void putIfAbsent(const ReportInputsType& inputs, const ReportType& outputs)
	{
		PString logStr;
		inputs.appendLog(logStr);

		if (options.moreTrace)
		{
			PLog("cache %s putIfAbsent(inputs: %s)", options.cacheName.c_str(), logStr.c_str());
		}

		auto mainCacheIt = mainCache.find(inputs);

		if (mainCacheIt == mainCache.end())
		{
			static ReportCacheValue<ReportType> EMPTY;
			auto res = mainCache.emplace(make_pair(inputs, EMPTY));
			mainCacheIt = res.first;
			ReportCacheValue<ReportType>& reportCacheValue = mainCacheIt->second;
			reportCacheValue.outputs = outputs;
			updateLastAccessTimeInAuxCache(inputs, reportCacheValue);

			if (options.moreTrace)
			{
				PLog("Added new");
			}

			ensureCacheNotTooBig();
		}
		else
		{
			ReportCacheValue<ReportType>& reportCacheValue = mainCacheIt->second;
			updateLastAccessTimeInAuxCache(inputs, reportCacheValue);

			if (options.moreTrace)
			{
				PLog("Already in cache");
			}
		}
	}

	void clear()
	{
		PLog("cache %s clear(numItems: %d)", options.cacheName.c_str(), getNumItems());
		auxCache.clear();
		mainCache.clear();
	}

	void setMoreTrace(bool moreTrace)
	{
		options.moreTrace = moreTrace;
	}

private: // Helper methods

	void ensureCacheNotTooBig()
	{
		if (options.moreTrace)
		{
			PLog("cache %s ensureCacheNotTooBig(numItems: %d)", options.cacheName.c_str(), getNumItems());
		}

		while (getNumItems() > options.maxCacheSize && !auxCache.empty())
		{
			auto auxCacheIt = auxCache.begin();
			vector<CacheMapConstIt>& auxCacheItemVec = auxCacheIt->second;

			if (auxCacheItemVec.empty())
			{
				// Log warning?
				auxCache.erase(auxCacheIt);
				continue;
			}

			auto auxCacheItemVecIt = auxCacheItemVec.rbegin();
			const CacheMapConstIt& mainCacheIt = *auxCacheItemVecIt;

			mainCache.erase(mainCacheIt);
			auxCacheItemVec.pop_back();

			if (auxCacheItemVec.empty())
			{
				auxCache.erase(auxCacheIt);
			}
		}

		if (options.moreTrace)
		{
			PLog("cache %s #ensureCacheNotTooBig(numItems: %d)", options.cacheName.c_str(), getNumItems());
		}
	}

	void updateLastAccessTimeInAuxCache(const ReportInputsType& inputs, ReportCacheValue<ReportType>& reportCacheValue)
	{
		time_t prevTimeLastAccessed = reportCacheValue.timeLastAccessed;
		reportCacheValue.updateTimeLastAccessed();
		removeFromAuxCache(prevTimeLastAccessed, inputs);
		addToAuxCache(reportCacheValue.timeLastAccessed, inputs);
	}

	void removeFromAuxCache(time_t timeLastAccessed, const ReportInputsType& inputs)
	{
		auto it = auxCache.find(timeLastAccessed);

		if (it == auxCache.end())
		{
			// Log warning?
			return;
		}

		vector<CacheMapConstIt>& auxCacheItemVec = it->second;
		auto auxCacheItemVecIt = SecUtils::find_if(auxCacheItemVec, [&inputs](CacheMapConstIt cacheMapConstIt) {
			const ReportInputsType& inputs_ = cacheMapConstIt->first;
			return inputs_ == inputs;
		});

		if (auxCacheItemVecIt != auxCacheItemVec.end())
		{
			auxCacheItemVec.erase(auxCacheItemVecIt);
		}

		if (auxCacheItemVec.empty())
		{
			auxCache.erase(it);
		}
	}

	void addToAuxCache(time_t timeLastAccessed, const ReportInputsType& inputs)
	{
		const auto mainCacheIt = mainCache.find(inputs);
		if (mainCacheIt == mainCache.end())
		{
			// Log warning?
			return;
		}

		auto auxCacheIt = auxCache.find(timeLastAccessed);
		if (auxCacheIt == auxCache.end())
		{
			static const vector<CacheMapConstIt> EMPTY;
			auxCacheIt = auxCache.insert(make_pair(timeLastAccessed, EMPTY)).first;
		}

		vector<CacheMapConstIt>& auxCacheItemVec = auxCacheIt->second;
		auxCacheItemVec.push_back(mainCacheIt);
	}

	int getNumItems() const 
	{
		return mainCache.size();
	}
};

////////////////////////////////////////////////////////////////////////////////

#endif // ReportCacheTemplates_h_included
