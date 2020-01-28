///counters.h
#ifndef include_counters_h
#define include_counters_h

#ifdef _DEBUG
#include "pperror.h"
#endif
#include "HeLogger.h"
#include "commmsgbody.h"

class HiresCounter
{
	friend class TicksAccumulator;
	UINT64	_counterStart;
	INT64	_counterResult;
	UINT32 _counterTick;
#ifdef _DEBUG
	bool stopped;
#endif

public:
	HiresCounter()
	{
#ifdef _DEBUG
		stopped = false;
#endif
		start();
	}
	void start() { _counterStart = getHighResCounter(); _counterTick = SYSTEMTICKS(); _counterResult = 0; } 
	void stop()
	{
		_counterResult = getHighResCounter() - _counterStart;
		_counterTick = SYSTEMTICKS() - _counterTick;
#ifdef _DEBUG
		stopped = true;
#endif
	}
	INT32 getElapsed32() const
	{
#ifdef _DEBUG
		PASSERT( stopped );
#endif
		return ( INT32 )( _counterResult >> 16 ); //lint !e704
	}
	UINT32 getElapsedTick() const
	{
		return _counterTick;
	}
    // PYR-32351 - to pass this structure between user and main thread in dbm
    void compose( CommMsgBody& body ) const;
    void parse( CommMsgParser& parser );
};

class TicksAccumulator
{
	UINT32 startTicks;
	UINT32 messagesProcessed;
	UINT64 startCounter; 
	INT64  countersSpent;
	UINT32 countersSpentTick;

	UINT64 stopCounter;
	UINT32 stopTicks;

public:
	void addMessageTicks( const HiresCounter& counter, bool countMessage = true )
	{
		if( countMessage )
			messagesProcessed++;
		countersSpent += counter._counterResult;
		countersSpentTick += counter._counterTick;
	}
	void reset();
	void stop();
	TicksAccumulator() { reset(); }
	INT32 getAccumulated32() const { return ( INT32 )( countersSpent >> 16 );  } //lint !e704
	INT32 getAccumulatedTicks() const { return ( INT32 )countersSpentTick;  }
	UINT32 getMessagesProcessed() const { return messagesProcessed; }
	UINT32 messagesPerSec() const;
	double messagesPerSecF() const;
	UINT32 avgProcessingMkSec() const;
	double avgProcessingMkSecF() const;
	UINT32 avgProcessingMkSecTick() const;
	UINT32 loadPercentCnt() const;
	double loadPercentCntF() const;
	UINT32 loadPercentCntTick() const;
	UINT32 mSecondsElapsed() const { return stopTicks - startTicks; }
	UINT64 getCounterElapsed() const { return stopCounter - startCounter; }
	// To pass this structure between user and main thread in dbm
	void composeForAdjust( CommMsgBody& body ) const;
	void parseForAdjust( CommMsgParser& parser );
	void adjust( const TicksAccumulator& tempCopy );
};

class TicksAccumulatorStopResetWrapper
{
private:
	TicksAccumulator& ticksAccumulator;
public:
	TicksAccumulatorStopResetWrapper( TicksAccumulator& ticksAccumulator_ );
	~TicksAccumulatorStopResetWrapper();

public:
	void reportTicks( const char* nameSuffix, HeLogger* heLogger, CommMsgBody& reportBody );
};

class QueueAccumulator
{
	UINT32 n, total, hwm;

public:
	QueueAccumulator() { reset(); }
	void reset() { n = total = hwm = 0; }
	void addQueue( UINT32 sz )
	{
		n++;
		total += sz;
		if( sz > hwm )
			hwm = sz;
	}
	UINT32 getAverage() const { return n ? ( total / n ) : 0; }
	double getAverageF() const { return n ? ( double(total) / n ) : 0.0; }
	UINT32 getMax() const { return hwm; }
};

class MessageMonitor;
class StepMonitor;

class MonitorForMessages
{

private:
	friend class MessageMonitor;

	struct MonitorForStep
	{
		PString name;
		TicksAccumulator stepAccumulator;
		// To pass this structure between user and main thread in dbm
		void composeForAdjust( CommMsgBody& body ) const;
		void parseForAdjust( CommMsgParser& parser );
	};

	typedef list<MonitorForStep> MonitorForSteps;

	struct MonitorForMessage
	{
		PString name;
		TicksAccumulator messageAccumulator;
		MonitorForSteps stepMonitors;
		// To pass this structure between user and main thread in dbm
		void composeForAdjust( CommMsgBody& body ) const;
		void parseForAdjust( CommMsgParser& parser );
	};

	map<UINT32,MonitorForMessage> monitorsForMessages;

	MonitorForMessage* activeMonitorForMessage;	

public:
	MonitorForMessages()
	{
		activeMonitorForMessage = NULL;
	}
	
	MonitorForMessages(const MonitorForMessages& other) = delete;
	MonitorForMessages& operator=(const MonitorForMessages& other) = delete;

	void registerMonitor( const UINT32 msgId, const char* name );
	void startMonitor( const UINT32 msgId, MessageMonitor& msgMonitor );
	void startStep( const char* name, StepMonitor& stepMonitor );
	void reportPerformance( const HeLogger* helogger, CommMsgBody& reportBody );
	// To pass this structure between user and main thread in dbm
	void composeForAdjust( CommMsgBody& body ) const;
	void parseForAdjust( CommMsgParser& parser );
	void reset(); // to reset without reporting to monitor
	void adjust( const MonitorForMessages& tempCopy );
};

class MessageMonitor
{

private:
	friend class MonitorForMessages;
	HiresCounter counter;
	TicksAccumulator* tickAccumulator;
	MonitorForMessages* monitorForMessages;

public:
	MessageMonitor()
	{
		tickAccumulator = NULL;
		monitorForMessages = NULL;
		counter.start();
	}

	MessageMonitor(const MessageMonitor& other) = delete;
	MessageMonitor& operator=(const MessageMonitor& other) = delete;

	void stop()
	{
		if ( tickAccumulator != NULL )
		{
			counter.stop();
			tickAccumulator->addMessageTicks( counter );
			tickAccumulator = NULL;
			if ( monitorForMessages != NULL )
				monitorForMessages->activeMonitorForMessage = NULL;
			monitorForMessages = NULL;
		}
	}

	void reset()
	{
		counter.start();
	}

	~MessageMonitor() //lint -e{1551}
	{
		//PTRACE5( "cleaning up the message monitor." );
		stop();
	}
};

class StepMonitor
{

private:
	friend class MonitorForMessages;
	HiresCounter counter;
	TicksAccumulator* tickAccumulator;

public:

	StepMonitor()
	{
		tickAccumulator = NULL;
		counter.start();
	}

	StepMonitor(const StepMonitor& other) = delete;
	StepMonitor& operator=(const StepMonitor& other) = delete;

	void stop()
	{
		//PTRACE5( "stopping the step monitor..." );
		if ( tickAccumulator != NULL )
		{
			counter.stop();
			tickAccumulator->addMessageTicks( counter );
		}
	}

	void reset()
	{
		counter.start();
	}

	~StepMonitor() //lint -e{1551}
	{
		//PTRACE5( "cleaning up the message step monitor." );
		stop();
	}
};

// PYR-40275
class RequestDelayMonitor
{
	typedef map<UINT32/*reqId*/, UINT32/*tickCount*/> RequestMapType;
	RequestMapType pendingRequests;
	
	UINT64 totalDelay;
	UINT32 maxDelay;
	UINT32 count;
	
public:
	RequestDelayMonitor()
	{
		clear();
	}

	void clear()
	{
		totalDelay = 0;
		count = 0;
		maxDelay = 0;
	}

	void requestOut( UINT32 reqId )
	{
		pendingRequests.insert( make_pair( reqId, SYSTEMTICKS() ) );
	}
	void requestIn( UINT32 reqId )
	{
		RequestMapType::iterator it = pendingRequests.find( reqId );
		if( pendingRequests.end() == it )
			return;
		UINT32 elapced = SYSTEMTICKS() - it->second;
		totalDelay += elapced;
		++count;
		if( maxDelay < elapced )
			maxDelay = elapced;

		pendingRequests.erase( it );
	}
	UINT32 getAveDelay() const
	{
		if( !count )
			return 0;

		return totalDelay / count;
	}
	UINT32 getMaxDelay() const
	{
		return maxDelay;
	}
	UINT32 getPendingSize() const
	{
		return pendingRequests.size();
	}
};

#endif //include_counters_h
