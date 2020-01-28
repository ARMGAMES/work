#include "ppinclude.h"
#include "ppstring.h"
#include "pperror.h"
#include "pplogfile.h"
#include "commmsgbody.h"
#include "commidmap.h"
#include "commqueue.h"
#include "commrtable.h"
#include "commoption.h"

_CommManageableThread::_CommManageableThread()
{
	exited = false;
	cleanup = false;
	userCpuUsage = (UINT64)(-1);
	kernelCpuUsage = (UINT64)(-1);
	routingTableTime = 0;
	afterLockTime = 0;

}

void _CommManageableThread::exit(bool cleanup_)
{
	exited = true;
	cleanup=cleanup_;
}

void _CommManageableThread::shutdown()
{
	q.shutdown();
}

void _CommManageableThread::dump()
{
	if (inStateOfExit())
	{
		rundump();
	}
	else
	{
		_CommMsg msg( _COMM_MSGTYPE_SERVICEREQUEST, _COMM_MSGPRIORITIES-1 );
		msg.body.composeUINT32( _SERVICE_REQUEST_DUMP );
		push(msg);
	}
}

void _CommManageableThread::rundump()
{
	PLog("Dump=== size of queue = %d",q.size());
}


//****************************************************************************

CommThreadManager::CommThreadManager( CommRoutingTable& routingTable_, BYTE flags_)
: routingTable( routingTable_ ),
cs( routingTable_.cs )
{
	flags = flags_;
	routingTable._registerThreadManager( *this );
	channelIdGen = RANDOMTIME();
	maxNumberOfConnectionsPerThread=0;
	forceClientDisconnect = false;
	allowClientDisconnect = false;
	inStateOfClosedAccept = false;
	gatherStatistic = 0;
    networkStatsLogThreshold = 0;
	connectAttemptsDDOSProtection = 0;
	exitOnFatalError = false;
	qSize=0;
	qTime=0;
	qSlotSize=0;
	qSlotTime=0;

}

UINT32 CommThreadManager::nextChannelId()
{
	PWLock lock( cs );
	if( ++channelIdGen == 0 )
		channelIdGen = 1;
	return channelIdGen;
}

/* virtual */ void CommThreadManager::setOption( const char* optName, const char* optValue )
{
	if( strcmp( optName, COMM_OPTION_EXITONFATALERROR ) == 0 )
		exitOnFatalError = atoi( optValue ) != 0;
	else if( strcmp( optName, COMM_OPTION_NUMBEROFCONNECTIONSPERTHREAD ) == 0 )
		maxNumberOfConnectionsPerThread =  atoi( optValue );
	else if( strcmp( optName, COMM_OPTION_GATHERSTATISTIC ) == 0 )
		gatherStatistic =  atoi( optValue );
    else if( strcmp( optName, COMM_OPTION_NETWORKSTATSLOGTHRESHOLD ) == 0 )
        networkStatsLogThreshold =  atoi( optValue );
	else if( strcmp( optName, COMM_OPTION_CONNECTATTEMPTSDDOSPROTECTION ) == 0 )
		connectAttemptsDDOSProtection =  atoi( optValue );
	else
		PLog( "CommThreadManager: Unknown option '%s' - ignored", optName );
}

/* virtual */ void CommThreadManager::dynamicInit(PIniFile& iniFile)
{
	PIniFile::Section* section = iniFile.getSection( managerType.c_str() );
	if( section )
	{
		setOption(COMM_OPTION_GATHERSTATISTIC,section->getProperty(COMM_OPTION_GATHERSTATISTIC,""));
		const char *priority = section->getProperty(COMM_OPTION_BASETHREADPRIORITY);
		if (priority)
		{
			PWLock lock( cs );
			int n = static_cast< int >( threads.size() );
			for( int i=0; i < n ; ++i )
				threads[ i ].thread->setThreadPriority(priority);
			baseThreadPriority.assign(priority);
		}
	}
}

void CommThreadManager::push( _CommPhysicalConnection* physConn, /*non-const!*/ _CommMsg& m )
{
	if( ( flags & _flagPrepatch ) && ( m.internal.flags & _CommMsg::Patch8 ) )
	{
		_CommMsg m2;
		_copyPatchMsg( m2, m );
		m2.internal.slot = static_cast< UINT16 >( physConn->slot );
		m2.internal.channelId = physConn->channelId;
		if( flags & _flagVirtualPush )
			virtualPush( physConn, m2 );
		else
			_push( physConn, m2 );
	}
	else
	{
		m.internal.slot = static_cast< UINT16 >( physConn->slot );
		m.internal.channelId = physConn->channelId;
		if( flags & _flagVirtualPush )
			virtualPush( physConn, m );
		else
			_push( physConn, m );
	}
}

void CommThreadManager::pushRead( _CommPhysicalConnection* physConn, /*non-const!*/ _CommMsg& m )
{
	if( ( flags & _flagPrepatch ) && ( m.internal.flags & _CommMsg::Patch8 ) )
	{
		_CommMsg m2;
		_copyPatchMsg( m2, m );
		m2.internal.slot = static_cast< UINT16 >( physConn->slot );
		m2.internal.channelId = physConn->channelId;
		if( flags & _flagVirtualPush )
			virtualPushRead( physConn, m2 );
		else
			_pushRead( physConn, m2 );
	}
	else
	{
		m.internal.slot = static_cast< UINT16 >( physConn->slot );
		m.internal.channelId = physConn->channelId;
		if( flags & _flagVirtualPush )
			virtualPushRead( physConn, m );
		else
			_pushRead( physConn, m );
	}
}

void CommThreadManager::push( _CommPhysicalConnection* physConn, /*non-const!*/ _CommMsg& m, UINT32 * tickCounterMsg_ )
{
	if( ( flags & _flagPrepatch ) && ( m.internal.flags & _CommMsg::Patch8 ) )
	{
		_CommMsg m2;
		_copyPatchMsg( m2, m );
		m2.internal.slot = static_cast< UINT16 >( physConn->slot );
		m2.internal.channelId = physConn->channelId;
		if( flags & _flagVirtualPush )
			virtualPush( physConn, m2 );
		else
		{
			tickCounterMsg_[0] = SYSTEMTICKS();
			_push( physConn, m2, tickCounterMsg_+1   );
		}
	}
	else
	{
		m.internal.slot = static_cast< UINT16 >( physConn->slot );
		m.internal.channelId = physConn->channelId;
		if( flags & _flagVirtualPush )
			virtualPush( physConn, m );
		else
		{
			tickCounterMsg_[0] = SYSTEMTICKS();
			_push( physConn, m, tickCounterMsg_+1 );
		}
	}
}
void CommThreadManager::pushRead( _CommPhysicalConnection* physConn, /*non-const!*/ _CommMsg& m, UINT32 * tickCounterMsg_ )
{
	if( ( flags & _flagPrepatch ) && ( m.internal.flags & _CommMsg::Patch8 ) )
	{
		_CommMsg m2;
		_copyPatchMsg( m2, m );
		m2.internal.slot = static_cast< UINT16 >( physConn->slot );
		m2.internal.channelId = physConn->channelId;
		if( flags & _flagVirtualPush )
			virtualPushRead( physConn, m2 );
		else
		{
			tickCounterMsg_[0] = SYSTEMTICKS();
			_pushRead( physConn, m2, tickCounterMsg_+1   );
		}
	}
	else
	{
		m.internal.slot = static_cast< UINT16 >( physConn->slot );
		m.internal.channelId = physConn->channelId;
		if( flags & _flagVirtualPush )
			virtualPushRead( physConn, m );
		else
		{
			tickCounterMsg_[0] = SYSTEMTICKS();
			_pushRead( physConn, m, tickCounterMsg_+1 );
		}
	}
}


void CommThreadManager::_push( _CommPhysicalConnection* physConn, /*non-const!*/ _CommMsg& m )
{
	PWLock lock( cs );
	_CommManageableThread* thread = threads[ physConn->thread ].thread;
	if( !thread )
	{
		PLog( "CommRoutingTable: Thread #%d is already deleted - msg ignored", physConn->thread );
		return;
	}

	thread->push( m );
}
void CommThreadManager::_pushRead( _CommPhysicalConnection* physConn, /*non-const!*/ _CommMsg& m )
{
	_CommManageableThread* thread = threads[ physConn->thread ].thread;
	if( !thread )
	{
		PLog( "CommRoutingTable: Thread #%d is already deleted - msg ignored", physConn->thread );
		return;
	}

	thread->push( m );
}

void CommThreadManager::_push( _CommPhysicalConnection* physConn, /*non-const!*/ _CommMsg& m, UINT32 * tickCounterMsg_ )
{
	PWLock lock( cs );
	_CommManageableThread* thread = threads[ physConn->thread ].thread;
	if( !thread )
	{
		PLog( "CommRoutingTable: Thread #%d is already deleted - msg ignored", physConn->thread );
		return;
	}
	tickCounterMsg_[0] = SYSTEMTICKS();
	thread->push( m, tickCounterMsg_ + 1 );
}
void CommThreadManager::_pushRead( _CommPhysicalConnection* physConn, /*non-const!*/ _CommMsg& m, UINT32 * tickCounterMsg_ )
{
	_CommManageableThread* thread = threads[ physConn->thread ].thread;
	if( !thread )
	{
		PLog( "CommRoutingTable: Thread #%d is already deleted - msg ignored", physConn->thread );
		return;
	}
	tickCounterMsg_[0] = SYSTEMTICKS();
	thread->push( m, tickCounterMsg_ + 1 );
}

void CommThreadManager::dump()
{
	PWLock lock( cs );
	int n = static_cast< int >( threads.size() );
	PLog("Dump=== '%s' Number of threads = %d",whoAreYou(),n);
	for( int i=0; i < n ; ++i )
	{
		PLog("Dump=== Thread %d, maxSlots=%d,slotLeft=%d, thread=%p",i,threads[i].maxSlots,
			threads[i].slotsLeft,threads[i].thread);
		if (threads[ i ].thread)
			threads[ i ].thread->dump();
	}
}


UINT32 CommThreadManager::printCounters()
{
	PWLock lock( cs );
	int n = static_cast< int >( threads.size() );
	UINT32 _max = 0;

	for( int i=0; i < n ; ++i )
	{
		if (threads[ i ].thread && !threads[ i ].thread->inStateOfExit())
		{
			UINT32 qs,qt,qss,qst;
			UINT32 _max1 = threads[ i ].thread->printCounters(i,qs,qt,qss,qst);
			if (_max1 > _max)
				_max = _max1;
			if (qs > qSize)
				qSize=qs;
			if (qt > qTime)
				qTime=qt;
			if (qss > qSlotSize)
				qSlotSize=qss;
			if (qst > qSlotTime)
				qSlotTime=qst;
		}
	}
	return _max;
}

void CommThreadManager::setStatistic( _CommPhysicalConnection* physConn, UINT32 totalTime_,UINT32 afterLockTime_)
{
	// do not need PLock, function is called from routing table being under PLock
	if (physConn->thread >=0 && physConn->thread < threads.size())
	{	
		_CommManageableThread* thread = threads[ physConn->thread ].thread;
		if( thread && !thread->inStateOfExit())
		{
			thread->setStatistic( totalTime_, afterLockTime_ );
		}
	}
}

/* virtual */ void CommThreadManager::start()
{
}

/* virtual */ void CommThreadManager::shutdown()
{
	PWLock lock( cs );
	int n = static_cast< int >( threads.size() );
	for( int i=0; i < n ; ++i )
	{
		Thread& thread = threads[ i ];
		if( thread.thread != 0 )
			thread.thread->shutdown();
	}
}

/* virtual */ bool CommThreadManager::terminated()
{
	PWLock lock( cs );
	int n = static_cast< int >( threads.size() );
	for( int i=0; i < n ; ++i )
	{
		Thread& thread = threads[ i ];
		if( thread.thread != 0 && !thread.thread->terminated() )
			return false;
	}
	return true;
}

/* virtual */ CommThreadManager::~CommThreadManager()
{
	PWLock lock( cs );
	int n = static_cast< int >( threads.size() );
	for( int i=0; i < n ; ++i )
		delete threads[ i ].thread;
}


void CommThreadManager::_checkExitingThreads()
{
	int n = static_cast< int >( threads.size() );
	for( int i=0; i < n ; ++i )
	{
		Thread& thread = threads[ i ];
		if( thread.thread != 0 && thread.thread->inStateOfExit() == true && thread.thread->terminated())
		{
			if (thread.thread->isCleanupNeeded())
			{
				routingTable.cleanMgrConnections(this,i);
			}
			delete thread.thread;
			thread.thread = 0;
		}
	}
}


int CommThreadManager::_findSlot()//returns threadIdx. MUST be locked before call
{
	_checkExitingThreads();
	int n = static_cast< int >( threads.size() );
	for( int i=0; i < n ; ++i )
	{
		Thread& thread = threads[ i ];
		if( thread.thread != 0 && thread.thread->inStateOfExit() == false)
		{ 
			if(maxNumberOfConnectionsPerThread)
			{
				if (thread.slotsLeft > (thread.maxSlots - maxNumberOfConnectionsPerThread) )
					return i;
			}
			else
			{
				if (thread.slotsLeft > 0)
					return i;
			}

		}
	}
	return -1;
}

int CommThreadManager::_allocThread()//returns threadIdx. MUST be locked before call
{
	int n = static_cast< int >( threads.size() );
	for( int i=0; i < n ; ++i )
	{
		Thread& thread = threads[ i ];
		if( thread.thread == 0)
			return i;
	}

	threads.push_back( Thread() );
	return static_cast< int >( threads.size() ) - 1;
}

/* static */ void CommThreadManager::_copyPatchMsg( _CommMsg& m, const _CommMsg& srcMsg )
{
	PASSERT3( srcMsg.internal.flags & _CommMsg::Patch8 );
	m.internal = srcMsg.internal;
	m.internal.flags &= ~_CommMsg::Patch8;
	m.priority = srcMsg.priority;
	m.type = srcMsg.type;
	m.flags = srcMsg.flags;
	m.body.copyFrom( srcMsg.body );
	PASSERT3( m.body._size() >= 8 );
	BYTE* p = m.body._writePtr();
	CommMsgBody::writeUINT32( p, srcMsg.internal.patch0 );
	p += 4;
	CommMsgBody::writeUINT32( p, srcMsg.internal.patch1 );
	//p += 4;
}

CommThreadManagerWithTimerThread::CommThreadManagerWithTimerThread(CommRoutingTable& routingTable, BYTE flags_) :
	CommThreadManager(routingTable, flags_), timerThread(nullptr)
{
}

void CommThreadManagerWithTimerThread::_timer(_CommMsg& msg)
{
	PWLock lock(cs);

	int threadIdx = msg.internal.slot;
	msg.internal.slot = 0;

	Thread& thread = threads[threadIdx];
	_pushToThread(thread.thread, msg);
}

UINT32 CommThreadManagerWithTimerThread::_postTimer(_CommPhysicalConnection* physConn, _CommMsg& msg, int msec)
{
	PWLock lock(cs);

	msg.internal.slot = physConn->thread;
	msg.internal.channelId = physConn->channelId;
	return timerThread->push(msg, msec);
}
