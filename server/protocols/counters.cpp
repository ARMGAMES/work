///counters.cpp
#include "ppincludebase.h"
#include "counters.h"

void TicksAccumulator::reset()
{
	startTicks = SYSTEMTICKS();
	messagesProcessed = 0;
	countersSpent = 0;
	startCounter = getHighResCounter();
	stopCounter = 0;
	stopTicks = 0;
	countersSpentTick = 0;
}

void TicksAccumulator::stop()
{
	stopCounter = getHighResCounter();
	stopTicks = SYSTEMTICKS();
}

UINT32 TicksAccumulator::messagesPerSec() const 
{
	UINT32 ticksMeasured = stopTicks - startTicks; //mseconds
	return ticksMeasured ? ( ( (UINT64)messagesProcessed * 1000 ) / ticksMeasured ) : 0;
}

double TicksAccumulator::messagesPerSecF() const
{
	UINT32 ticksMeasured = stopTicks - startTicks; //mseconds
	return ticksMeasured ? ( ( messagesProcessed * 1000.0 ) / ticksMeasured ) : 0.0;
}

UINT32 TicksAccumulator::avgProcessingMkSec() const 
{
	UINT64 totalHiResCounter = stopCounter - startCounter; //CPU ticks
	UINT32 ticksMeasured = stopTicks - startTicks; //mseconds
	UINT32 mksec = 
			( messagesProcessed && totalHiResCounter ) 
			?
			UINT32( ( ticksMeasured * ( UINT64 )countersSpent * 1000 ) / ( totalHiResCounter * messagesProcessed ) )
			: 
			0;
	return mksec;
}

double TicksAccumulator::avgProcessingMkSecF() const
{
	UINT64 totalHiResCounter = stopCounter - startCounter; //CPU ticks
	UINT32 ticksMeasured = stopTicks - startTicks; //mseconds
	double mksec =
		( messagesProcessed && totalHiResCounter )
		?
		( ticksMeasured * 1000.0 * countersSpent ) / ( (double)totalHiResCounter * messagesProcessed )
		:
		0.0;
	return mksec;
}

UINT32 TicksAccumulator::loadPercentCnt() const
{
	UINT64 totalHiResCounter = stopCounter - startCounter; //CPU ticks
	return totalHiResCounter ? ( UINT32 )( ( ( UINT64 )countersSpent * 100 ) / totalHiResCounter ) : 0;
}

double TicksAccumulator::loadPercentCntF() const
{
	UINT64 totalHiResCounter = stopCounter - startCounter; //CPU ticks
	return totalHiResCounter ? ( ( countersSpent * 100.0 ) / totalHiResCounter ) : 0.0;
}

UINT32 TicksAccumulator::avgProcessingMkSecTick() const 
{
	if( ! messagesProcessed )
		return 0;
	return PUMulDiv( countersSpentTick, 1000, messagesProcessed );
}

UINT32 TicksAccumulator::loadPercentCntTick() const
{
	UINT32 totalTickCounter = stopTicks - startTicks; //mseconds
	return totalTickCounter ? ( UINT32 )( ( ( UINT64 )countersSpentTick * 100 ) / totalTickCounter ) : 0;
}

// ===== TicksAccumulatorStopResetWrapper
TicksAccumulatorStopResetWrapper::TicksAccumulatorStopResetWrapper( TicksAccumulator& ticksAccumulator_ )
	: ticksAccumulator( ticksAccumulator_ )
{
	ticksAccumulator.stop();
}

TicksAccumulatorStopResetWrapper::~TicksAccumulatorStopResetWrapper()
{
	ticksAccumulator.reset();
}

void TicksAccumulatorStopResetWrapper::reportTicks( const char* nameSuffix, HeLogger* heLogger, CommMsgBody& reportBody )
{
	PString strValue;

	UINT32 messagesPerSec = ticksAccumulator.messagesPerSec();
	strValue.assign( "" ).appendInt( messagesPerSec );
	heLogger->compose ( reportBody, PString( "messagesPerSec" ).append( nameSuffix ), strValue );

	UINT32 avgProcessingMsec = ticksAccumulator.avgProcessingMkSec();
	strValue.assign( "" ).appendInt( avgProcessingMsec );
	heLogger->compose ( reportBody, PString( "avgProcessingMksec" ).append( nameSuffix ), strValue );
	strValue.assign( "" ).appendInt( ticksAccumulator.avgProcessingMkSecTick() );
	heLogger->compose ( reportBody, PString( "avgProcessingMksec" ).append( nameSuffix ).append( "Tick" ), strValue );

	UINT32 loadPercentCnt = ticksAccumulator.loadPercentCnt();
	strValue.assign( "" ).appendInt( loadPercentCnt );
	heLogger->compose ( reportBody, PString( "loadPercentCounter" ).append( nameSuffix ), strValue );

}

// ===== MessageMonitor

void MonitorForMessages::registerMonitor( UINT32 msgId, const char* name )
{
	PLog( "register message to monitor : %d ( %s )", msgId, name );
	MonitorForMessage monitorForMessage;
	monitorForMessage.name = name;
	monitorsForMessages[msgId] = monitorForMessage;
}

void MonitorForMessages::startMonitor( UINT32 msgId, MessageMonitor& msgMonitor )
{
	map<UINT32,MonitorForMessage>::iterator it = monitorsForMessages.find( msgId );
	if ( it == monitorsForMessages.end() )
	{
		PTRACE5( "Current message '%d' is NOT being monitored.", msgId );
		activeMonitorForMessage = NULL;
		msgMonitor.tickAccumulator = NULL;
	}
	else
	{
		activeMonitorForMessage = &it->second;
		//PTRACE5( "Current message '%d' ( %s ) is being monitored.", msgId, activeMonitorForMessage->name.c_str() );
		msgMonitor.tickAccumulator = &activeMonitorForMessage->messageAccumulator;
		msgMonitor.monitorForMessages = this;
	}
	
}

void MonitorForMessages::startStep( const char* name, StepMonitor& stepMonitor )
{
	if ( activeMonitorForMessage != NULL )
	{
		stepMonitor.stop();

		//PTRACE5( "Message is being monitored." );
		stepMonitor.tickAccumulator = NULL;

		MonitorForSteps& steps = activeMonitorForMessage->stepMonitors;
		for (MonitorForStep& step : steps)
		{
			if (step.name.equals(name))
			{
				stepMonitor.tickAccumulator = &step.stepAccumulator;
				stepMonitor.reset();
				break;
			}
		}

		// not found, create it
		if ( stepMonitor.tickAccumulator == NULL )
		{
			PLog( "step monitor not found, creating one for '%s'.", name );
			list<MonitorForStep>::iterator monitorForStep = steps.insert( steps.end(), MonitorForStep() );
			monitorForStep->name = name;
			stepMonitor.tickAccumulator = &( monitorForStep->stepAccumulator );
			stepMonitor.reset();
		}
	}
}

void MonitorForMessages::reportPerformance( const HeLogger* heLogger, CommMsgBody& reportBody )
{
	map<UINT32, MonitorForMessage>::iterator itr;
	PString strValue;
	PString strName;
	for ( itr = monitorsForMessages.begin(); itr != monitorsForMessages.end(); ++itr )
	{
		MonitorForMessage& msg = itr->second;

		msg.messageAccumulator.stop();

		// record the stats of the entire message and each step
		strName.assign( msg.name ).append( ".avg_processing.overall" );
		strValue.assign( "" ).appendInt( msg.messageAccumulator.avgProcessingMkSec() );
		heLogger->compose ( reportBody, strName, strValue );
//		PTRACE5( "performance report %s = %s", strName.c_str(), strValue.c_str() );

		strName.assign( msg.name ).append( ".msg_processed.overall" );
		strValue.assign( "" ).appendInt( msg.messageAccumulator.getMessagesProcessed() );
		heLogger->compose ( reportBody, strName, strValue );
//		PTRACE5( "performance report %s = %s", strName.c_str(), strValue.c_str() );

		MonitorForSteps& steps = msg.stepMonitors;
		for (MonitorForStep& step : steps)
		{
			step.stepAccumulator.stop();

			// record the stats of the entire message and each step
			strName.assign( msg.name ).append( ".avg_processing.step:" ).append( step.name );
			strValue.assign( "" ).appendInt( step.stepAccumulator.avgProcessingMkSec() );
			heLogger->compose ( reportBody, strName, strValue );
//			PTRACE5( "performance report %s = %s", strName.c_str(), strValue.c_str() );

			strName.assign( msg.name ).append( ".msg_processed.step:" ).append( step.name );
			strValue.assign( "" ).appendInt( step.stepAccumulator.getMessagesProcessed() );
			heLogger->compose ( reportBody, strName, strValue );
//			PTRACE5( "performance report %s = %s", strName.c_str(), strValue.c_str() );

			step.stepAccumulator.reset();
		}

		msg.messageAccumulator.reset();
	}
}
