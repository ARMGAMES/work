////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2018 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#ifndef ReportCacheTemplates_h_included
#define ReportCacheTemplates_h_included

////////////////////////////////////////////////////////////////////////////////

#include "ppinclude.h"

////////////////////////////////////////////////////////////////////////////////

template <typename ReportType>
struct ReportCacheValue
{
	time_t timeLastAccessed;
	ReportType outputs;

	ReportCacheValue() : timeLastAccessed(time(nullptr))
	{ }

	ReportCacheValue(const ReportType& outputs_) : timeLastAccessed(time(nullptr)), outputs(outputs_)
	{ }

	bool shouldRemove(int maxTimeInCacheSeconds)
	{
		time_t now = time(nullptr);
		INT64 timeInCacheSecs = now - timeLastAccessed;
		return timeInCacheSecs > maxTimeInCacheSeconds;
	}

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

template <typename ReportInputsType, typename ReportType, typename ReportInputsPred = less<ReportInputsType>>
struct ReportCache
{
	static const int MAX_TIME_IN_CACHE_SECONDS = 3600;
	static const int MAX_REPORT_CACHE_SIZE_MEDIUM = 15;
	static const int MAX_REPORT_CACHE_SIZE_SMALL = 5;

	int cacheHits;
	int cacheMisses;
	PString cacheName;
	map<ReportInputsType, ReportCacheValue<ReportType>, ReportInputsPred> cache;

public:
	ReportCache(const char* cacheName_) : cacheHits(0), cacheMisses(0), cacheName(cacheName_)
	{ }

	void sweep()
	{
		PLog("cache %s sweep(numItems: %u)", cacheName.c_str(), (UINT32)cache.size());

		auto it = cache.begin();
		while (it != cache.end())
		{
			ReportCacheValue<ReportType>& reportCacheValue = it->second;
			if (reportCacheValue.shouldRemove(MAX_TIME_IN_CACHE_SECONDS))
			{
				auto eraseIt = it++;
				cache.erase(eraseIt);
			}
			else
			{
				++it;
			}
		}

		PLog("cache %s #sweep(numItems: %u)", cacheName.c_str(), (UINT32)cache.size());
	}

	void logStats() const
	{
		PLog("cache %s logStats(hits: %d, misses: %d)", cacheName.c_str(), cacheHits, cacheMisses);
	}

	bool get(const ReportInputsType& inputs, ReportType& outputs)
	{
		PString logStr;
		inputs.appendLog(logStr);

		PLog("cache %s get(inputs: %s)", cacheName.c_str(), logStr.c_str());

		auto it = cache.find(inputs);

		if (it == cache.end())
		{
			++cacheMisses;
			PLog("Not found");

			return false;
		}
		else
		{
			++cacheHits;
			PLog("Found");

			ReportCacheValue<ReportType> reportCacheValue = it->second;
			reportCacheValue.getOutputs(outputs);

			return true;
		}
	}

	void putIfAbsent(const ReportInputsType& inputs, const ReportType& outputs)
	{
		PString logStr;
		inputs.appendLog(logStr);

		PLog("cache %s putIfAbsent(inputs: %s)", cacheName.c_str(), logStr.c_str());

		auto it = cache.find(inputs);

		if (it == cache.end())
		{
			auto res = cache.insert(make_pair(inputs, ReportCacheValue<ReportType>()));
			ReportCacheValue<ReportType>& reportCacheValue = res.first->second;
			reportCacheValue.outputs = outputs;
			PLog("Added new");
			ensureCacheNotTooBig();
		}
		else
		{
			ReportCacheValue<ReportType>& reportCacheValue = it->second;
			reportCacheValue.updateTimeLastAccessed();
			PLog("Already in cache");
		}
	}

	void clear()
	{
		PLog("cache %s clear(numItems: %u)", cacheName.c_str(), (UINT32)cache.size());
		cache.clear();
	}

private: // Helper methods

	void ensureCacheNotTooBig()
	{
		PLog("cache %s ensureCacheNotTooBig(numItems: %u)", cacheName.c_str(), (UINT32)cache.size());

		while (cache.size() > MAX_REPORT_CACHE_SIZE_MEDIUM)
		{
			time_t oldestLastAccessTime = time(nullptr);
			auto oldestCacheEntrySoFar = cache.end();
			for (auto it = cache.begin(); it != cache.end(); ++it)
			{
				ReportCacheValue<ReportType>& reportCacheValue = it->second;
				if (reportCacheValue.timeLastAccessed < oldestLastAccessTime)
				{
					oldestLastAccessTime = reportCacheValue.timeLastAccessed;
					oldestCacheEntrySoFar = it;
				}
			}

			if (oldestCacheEntrySoFar != cache.end())
			{
				cache.erase(oldestCacheEntrySoFar);
			}
		}
		PLog("cache %s #ensureCacheNotTooBig(numItems: %u)", cacheName.c_str(), (UINT32)cache.size());
	}
};

////////////////////////////////////////////////////////////////////////////////

#endif // ReportCacheTemplates_h_included
