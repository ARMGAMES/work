#include "ppinclude.h"
#include "ppthread.h"
#include "ppsocket.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "commrtable.h"
#include "commsharedmem.h"
#include "commsharedmemloop.h"
#include "commtmlm.h"
#include "_commtcp0.h"
#include "_commtmlm.h"
#include "commoption.h"
#include "commmonitorlogger.h"



class _CommLMAcceptThread : public PThread
{
private:

	CommLMThreadManager& mgr;
	PString sharedResourceName; 
	CommSharedMemory comm;
	bool shutdownFlag;
	bool exitFlag;

public:
	_CommLMAcceptThread( CommLMThreadManager& mgr_, const char * sharedResourceName_ );
	void run() override;
	void shutdown();
	void threadState(CommMsgBody &body)
	{
		composeLoggerStat(body, sharedResourceName.c_str(), exitFlag);
	}

	//	bool terminated() const
	//		{
	//		return exited;
	//		}
};

class _CommLMConnectThread : public PThread
{
public:
	typedef PQueue< PQDeque< _CommPhysicalConnection* > > Queue;

private:
	//	bool exited;
	CommLMThreadManager& mgr;
	Queue q;
	PString sharedResourceName; 
	//	CommSharedMemory comm;

public:
	_CommLMConnectThread( CommLMThreadManager& mgr_ );
	void pushConnection( _CommPhysicalConnection* conn )
	{
		q.push( conn );
	}

public:
	void run() override;
	void shutdown()
	{
		q.shutdown();
	}
	//	bool terminated() const
	//		{
	//		return exited;
	//		}

private:
	void _connect( _CommPhysicalConnection* conn );
};

//****************************************************************************

_CommLMAcceptThread::_CommLMAcceptThread( CommLMThreadManager& mgr_, const char * sharedResourceName_ )
: mgr( mgr_ )
{
	sharedResourceName.assign(sharedResourceName_);
	shutdownFlag=false;
	exitFlag=false;
}

/* virtual */ void _CommLMAcceptThread::run()
{
	try
	{
		PTRACE3( "CommLMAcceptThread: create accepting communication point (%s)", sharedResourceName.c_str() );
		comm.Accept(sharedResourceName.c_str());
		for(;;)
		{
			int n = PWaitForObjects::waitSingle(comm.getReadHandle(),PWaitForObjects::infinite);
			if (shutdownFlag)
			{
				PTRACE3("CommLMAccept shutdown %s",comm.getResourceName().c_str());
				exitFlag = true;
				return;
			}
			switch(n)
			{
			case 0:
				{
					char buffer[256];
					while(comm.acceptConnectionMessage(buffer)>0)
					{
						PString addr2;
						mgr.composeSocketAddr( addr2, buffer,0);
						PLog( "Incoming connection from '%s'", addr2.c_str() );
						mgr._pushInSocket(buffer, addr2 );
					}

				}
				break;
			default:
				PASSERT(0);
				break;
			}
		}
	}
	catch( PError& err )
	{
		PLog( "CommLMAcceptThread exception: %s", err.why() );
	}
	exitFlag = true;
}

void _CommLMAcceptThread::shutdown()
{
	if (shutdownFlag == false)
	{
		shutdownFlag=true;
		comm.forceDisconnectSharedMemory();
	}
}

//****************************************************************************

_CommLMConnectThread::_CommLMConnectThread( CommLMThreadManager& mgr_ )
: mgr( mgr_ )
{
	//	exited = false;
}

/* virtual */ void _CommLMConnectThread::run()
{
	try
	{
		for(;;)
		{
			_CommPhysicalConnection* conn;
			int ok = q.waitPop( conn );
			switch( ok )
			{
			case Queue::waitPopOk:
				_connect( conn );
				break;
			case Queue::waitPopShutdown:
				//					exited = true;
				PTRACE3("CommLMConnect shutdown");
				return;
			default:
				PASSERT3( 0 );
				break;
			}
		}
	}
	catch( PError& err )
	{
		PLog( "LMConnectThread exception: %s", err.why() );
	}
	//	exited = true;
}

void _CommLMConnectThread::_connect( _CommPhysicalConnection* conn )
{
	try
	{

		PString addr;
		int port;
		mgr.parseSocketAddr( addr, port, conn->address );
		CommLMThreadManager::CommunicationPoint lmPoint;
		lmPoint.physConn = conn;
		lmPoint.incoming = false;
		char tmp[256];
		sprintf(tmp,"pyr_%4.4x",port);
		strncpy(lmPoint.sharedResourceName,tmp,255);
		mgr._pushOutSocket( lmPoint );
	}
	catch( PError& err )
	{
		PLog( "LMConnect to '%s' failed: %s", conn->address.c_str(), err.why() );
		mgr.routingTable.physicalConnectFailed( conn );
	}
}

//****************************************************************************



_CommLMThreadBase::_CommLMThreadBase( CommLMThreadManager& mgr_, int mgrIdx_, bool muxEnable_ )
: mgr( mgr_ )
{
	mgrIdx = mgrIdx_;
	muxEnable = muxEnable_;
	maxActiveSlots = 0;
	_statInit();
}

void _CommLMThreadBase::pushSocket( const CommLMThreadManager::CommunicationPoint& point )
{
	_CommMsg m( _COMM_MSGTYPE_NONE, _COMM_MSGPRIORITY_INITCONNECT );
	m.internal.slot = 0xFFFF;
	m.body._composeFixedBlock( (const BYTE*)&point, sizeof(CommLMThreadManager::CommunicationPoint) );
	q.push( m );
}

void _CommLMThreadBase::pushForceDisconnect( int slot, UINT32 channelId )
{
	_CommMsg m( _COMM_MSGTYPE_NONE, _COMM_MSGPRIORITIES - 1 );
	PASSERT3( slot != 0xFFFF );
	m.internal.slot = slot;
	m.internal.channelId = channelId;
	q.push( m );
}

//****************************************************************************

inline /* static */ _CommLMThreadBase* CommLMThreadManager::_rwThread( Thread& thread )
{
	return static_cast< _CommLMThreadBase* >( thread.thread );//safe, because all RWThreadManager threads are in fact RWThreads
}

_CommLMThreadBase* CommLMThreadManager::_createNewThread( int threadIdx )
{
	return new _CommLMThreadBase( *this, threadIdx, muxEnabled);
}


CommLMThreadManager::CommLMThreadManager( CommRoutingTable& routingTable_, bool muxEnabled_ )
: CommThreadManager( routingTable_ )
{
	started = false;
	muxEnabled = muxEnabled_;
	connThread = new _CommLMConnectThread( *this );
	comm_logging = 0;
}

void CommLMThreadManager::listenResource( const char * resource )
{
	PWLock lock( cs );
	_CommLMAcceptThread* acceptThread = new _CommLMAcceptThread( *this, resource );
	acceptThreads.push_back( acceptThread );
	listenResources.push_back(PString(resource));
}

void CommLMThreadManager::listenPort( int port )
{
	char tmp[256];
	sprintf(tmp,"pyr_%4.4x",port);
	listenResource(tmp);
}

/* virtual */ void CommLMThreadManager::setOption( const char* optName, const char* optValue )
{
	if ( strcmp( optName, COMM_OPTION_PROTOCOL_LOG_FILE ) == 0 && comm_logging == 0)
		comm_logging = new _CommProtocolLog(optValue,PLogFile::perDate|PLogFile::printTimePerLine);
	else 
		CommThreadManager::setOption( optName, optValue );
}

void CommLMThreadManager::start()
{
	PWLock lock( cs );
	PASSERT3( !started );
	connThread->start();
	int n = acceptThreads.size();
	for( int i=0; i < n ; ++i )
		acceptThreads[ i ]->start();
	started = true;
}

void CommLMThreadManager::shutdown()
{
	PWLock lock( cs );
	_Parent::shutdown();
	connThread->shutdown();
	int n = acceptThreads.size();
	for( int i=0; i < n ; ++i )
		acceptThreads[ i ]->shutdown();
}

bool CommLMThreadManager::terminated()
{
	PWLock lock( cs );
	if( !_Parent::terminated() )
	{
		return false;
	}
	if( !connThread->terminated() )
	{
		return false;
	}
	int n = acceptThreads.size();
	for( int i=0; i < n ; ++i )
		if( !acceptThreads[ i ]->terminated() )
		{
			return false;
		}
	return true;
}

void CommLMThreadManager::acceptCancel()
{
	PWLock lock( cs );
	PTRACE3( "%s acceptCancel() - acceptState=%s",whoAreYou(),inStateOfClosedAccept?"true":"false");
	if (inStateOfClosedAccept)
		return;
	int n = acceptThreads.size();
	for( int i=0; i < n ; ++i )
	{
		acceptThreads[ i ]->shutdown();
	}
	inStateOfClosedAccept = true;
}

bool CommLMThreadManager::acceptReinstate()
{
	PWLock lock( cs );
	PTRACE3( "%s acceptReinstate() - acceptState=%s",whoAreYou(),inStateOfClosedAccept?"true":"false");
	if (inStateOfClosedAccept == false)
		return true;
	int n = listenResources.size();
	int i;
	for( i=0; i < n ; ++i )
	{
		if (acceptThreads[ i ]->terminated()== false)
		{
			PTRACE3( "%s acceptReinstate() - thread not terminated",whoAreYou());
			return false;
		}
	}

	for( i=0; i < n ; ++i )
	{
		delete acceptThreads[ i ];
	}

	acceptThreads.clear();

	for( i=0; i < n ; ++i )
	{
		_CommLMAcceptThread* acceptThread = new _CommLMAcceptThread( *this, listenResources[i].c_str() );
		acceptThreads.push_back( acceptThread );
		acceptThread->start();
	}
	inStateOfClosedAccept = false;
	return true;
}

CommLMThreadManager::~CommLMThreadManager()
{
	PWLock lock( cs );
	delete connThread;
	int n = acceptThreads.size();
	for( int i=0; i < n ; ++i )
		delete acceptThreads[ i ];
	if(comm_logging)
		delete comm_logging;
}

void CommLMThreadManager::_pushInSocket( const char * sockDescr, const char* addr )
{
	CommLMThreadManager::CommunicationPoint point;
	strncpy(point.sharedResourceName,sockDescr,255);
	point.incoming=true;
	point.physConn = routingTable.createIncomingPhysicalConnection( addr, this );
	_pushSocket( point );
}

void CommLMThreadManager::_pushSocket( const CommLMThreadManager::CommunicationPoint& point )
{
	PWLock lock( cs );
	int threadIdx = _findSlot();
	if( threadIdx >= 0 )
	{
		Thread& thread = threads[ threadIdx ];
		_rwThread( thread )->pushSocket( point );
		--thread.slotsLeft;
		PTRACE3( "Point pushed into existing thread #%d; --slotsLeft=%d", threadIdx, thread.slotsLeft );
		return;
	}

	threadIdx = _allocThread();
	Thread& thread = threads[ threadIdx ];
	thread.thread = _createNewThread( threadIdx );
	thread.slotsLeft = thread.maxSlots = _rwThread( thread )->maxLMPoints();
	thread.thread->start();
	if (*baseThreadPriority.c_str())
	{
		thread.thread->setThreadPriority(baseThreadPriority.c_str());
	}
	PTRACE3( "Started LM thread #%d", threadIdx );

	_rwThread( thread )->pushSocket( point );
	--thread.slotsLeft;
	PTRACE3( "Point pushed into thread #%d", threadIdx );
}

bool CommLMThreadManager::_freeSocket( int threadIdx )//no thread operations allowed after freeSocket returned true//no thread operations allowed after freeSocket returned true
{
	PWLock lock( cs );
	Thread& thread = threads[ threadIdx ];
	if( ++thread.slotsLeft != thread.maxSlots )
		return false;

	//{+++ to preserve some threads for the future use - return false
	//}

	//	delete thread.thread;
	//	thread.thread = 0;
	thread.thread->exit();
	return true;
}

void CommLMThreadManager::connect( _CommPhysicalConnection* physConn )
{
	connThread->pushConnection( physConn );
}

void CommLMThreadManager::forceDisconnect( _CommPhysicalConnection* physConn )
{
	PWLock lock( cs );
	Thread& thread = threads[ physConn->thread ];
	_rwThread( thread )->pushForceDisconnect( physConn->slot, physConn->channelId );
}

void CommLMThreadManager::parseSocketAddr( PString& retAddr, int& port, const char* address )
{
	PASSERT3( PString::startsWith( address, _COMMSHAREDPREFIX ) );
	const char* p = address + _COMMSHAREDPREFIXLEN;
	const char* col = strchr( p, ':' );
	if( col )
	{
		retAddr.assign( p, col );
		port = atoi( col + 1 );
	}
	else
		retAddr = p;
}

void CommLMThreadManager::composeSocketAddr( PString& retAddr, const char* address, int port )
{
	retAddr.append( _COMMSHAREDPREFIX ).append( address ).append( ':' ).appendInt( port );
}

void CommLMThreadManager::loggerInformation(CommMsgBody &body)
{
	PWLock lock( cs );
	vector< /* new */ _CommLMAcceptThread* > :: iterator it;
	for (it=acceptThreads.begin(); it != acceptThreads.end();++it)
	{
		(*it)->threadState(body);
	}

	if (isGatherStatistic())
	{
		PString s;
		s.assign("Max queue size(").append(managerType.c_str()).append(")");
		composeLoggerStat(body, s.c_str(), qSize);

		s.assign("Max time in queue (").append(managerType.c_str()).append(")");
		composeLoggerStat(body, s.c_str(), qTime);
		qSize=0;
		qTime=0;
		if (isGatherStatistic()>1)
		{
			s.assign("Max slot queue (").append(managerType.c_str()).append(")");
			composeLoggerStat(body, s.c_str(), qSlotSize);
			s.assign("Max time in slot queue (").append(managerType.c_str()).append(")");
			composeLoggerStat(body, s.c_str(), qSlotTime);
			qSlotSize=0;
			qSlotTime=0;
		}
	}

}

