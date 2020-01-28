#include "ppinclude.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "commrtable.h"
#include "commsrv.h"
#include "commsrvcomposite.h"
#include "commtmcomposite.h"
#include "commoption.h"

#define _MAXLOCALTHREADOBJECTS 32
#define MAX_COMPOSITE_MESSAGE_SIZE 1000000
//  Composite local thread manager 


struct _CommMsgQueueCompositeItem
{
private:
	BYTE _data[ sizeof( _CommMsg ) ];
public:
	_CommMsg* msg()
	{
		return (_CommMsg*)_data;
	}
	const _CommMsg* msg() const
	{
		return (_CommMsg*)_data;
	}

public://CommPriorityQueue requirements
	enum { NPriorities = _CommMsg::NPriorities * 2 };
	bool physConnection() const
	{
		// Following msg types are processed in thread and doesn't go to the redundant object.
		// we don't want them to be append to other messages
		// msg()->internal.slot < _MAXLOCALTHREADOBJECTS means that message belongs to the 
		// logical connection and already joined
		if ( msg()->type == _COMM_MSGTYPE_NONE || 
			msg()->type == _COMM_MSGTYPE_LOCAL_CREATESERVEROBJECT || 
			msg()->type == _COMM_MSGTYPE_REGISTER_SERVEROBJECT_REPLY || 
			msg()->type == _COMM_MSGTYPE_SERVICEREQUEST ||
			msg()->internal.slot < _MAXLOCALTHREADOBJECTS)
			return false;
		return true;
	}
	int priority() const // Logical connections have higher priority
	{
		if (msg()->internal.slot < _MAXLOCALTHREADOBJECTS)
			return (((_CommMsg*)_data)->priority*2);
		else
			return ((_CommMsg*)_data)->priority;
	}

};

template< class T > class CommCompositePriorityQueue
{
private:
	size_t sz;
	/* mutable */ int maxPriority;
	deque < T > qs[ T::NPriorities ];

public:
	typedef T value_type;

public:
	CommCompositePriorityQueue()
	{
		sz = 0;
		maxPriority = -1;
	}
	size_t size() const
	{
		return sz;
	}
	void push( const T& t )
	{
		int p = t.priority();
		PASSERT3( p >= 0 && p < T::NPriorities );
		deque< T >& q = qs[ p ];
		if (t.physConnection())
		{
			if (q.empty() || (q[q.size()-1]).msg()->body._size() >= MAX_COMPOSITE_MESSAGE_SIZE)
			{
				// Create new message and append incoming message
				T newT;
#include "ppmemtracedisable.h"
				new( newT.msg() ) _CommMsg(t.msg()->type,t.msg()->priority);
#include "ppmemtraceenable.h"
				newT.msg()->internal = t.msg()->internal;
				newT.msg()->internal.flags=0; // just in case
				newT.msg()->internal.patch1 = SYSTEMTICKS(); // This is safe because during very short life of 
				// this _CommMsg internal structure won't be used

				// Prepare space for msgId, format, counter. It will save us one copy of this message 
				// when we will post it to logical connection 
				// Counter will be set when message will be moved to nonpriority queue of outgoing messages
				// in redundant object
				newT.msg()->body.composeUINT32(0).composeBYTE(0).composeUINT32(0);
				q.push_back( newT );
				if( p > maxPriority )
					maxPriority = p;
				++sz;
			}
			_CommMsg * msg = (q[q.size()-1]).msg();
			msg->body.composeBYTE(t.msg()->priority)
				.composeUINT16(t.msg()->flags)
				.composeBYTE(t.msg()->type)
				.composeUINT32(t.msg()->body._size());

			BYTE * data = msg->body._append(t.msg()->body._size());
			if( t.msg()->internal.flags & _CommMsg::Patch8 )
			{
				PASSERT3(t.msg()->body._size() >=8 );
				CommMsgBody::writeUINT32( data, t.msg()->internal.patch0 );
				data += 4;
				CommMsgBody::writeUINT32( data, t.msg()->internal.patch1 );
				data += 4;
				memcpy( data, t.msg()->body._readPtr() + 8, t.msg()->body._size() - 8 );
			}
			else
			{
				memcpy( data, t.msg()->body._readPtr(), t.msg()->body._size() );
			}
			t.msg()->_CommMsg::~_CommMsg(); //!!! destructor must be called for initial message. 
		}
		else
		{
			q.push_back( t );
			if( p > maxPriority )
				maxPriority = p;
			++sz;
		}
	}
	const T& peek() const
	{
		PASSERT3( sz > 0 );
		for(;;)
		{
			PASSERT3( maxPriority >= 0 );
			const deque< T >& q = qs[ maxPriority ];
			if( q.size() )
				return *q.begin();
			--((CommCompositePriorityQueue< T >*)this)->maxPriority;//should be just --maxPriority if it is mutable
		}
	}
	void pop()
	{
		PASSERT3( sz > 0 );
		for(;;)
		{
			PASSERT3( maxPriority >= 0 );
			deque< T >& q = qs[ maxPriority ];
			if( q.size() )
			{
				q.pop_front();
				--sz;
				return;
			}
			--maxPriority;
		}
	}
};

class CommCompositeMsgQueue : private PQueue< CommCompositePriorityQueue< _CommMsgQueueCompositeItem > >
{
private:
	typedef PQueue< CommCompositePriorityQueue< _CommMsgQueueCompositeItem > > _Parent;

public:
	enum { waitPopOk = _Parent::waitPopOk, waitPopTimeout = _Parent::waitPopTimeout, waitPopShutdown = _Parent::waitPopShutdown };

public:
	CommCompositeMsgQueue() {}
	~CommCompositeMsgQueue();

public:
	void push( /*non-const!*/ _CommMsg& msg );
	void push( /*non-const!*/ _CommMsg& msg, UINT32 * tickCounterMsg_ );
	int waitPop( _CommMsg& ret, int timeout );
	int waitPop( _CommMsg& ret );//infinite
	bool pop( _CommMsg& ret );

public:
	_Parent::shuttingDown;
	_Parent::size;
	_Parent::getSocket;
	_Parent::shutdown;

private://prohibited
	CommCompositeMsgQueue( const CommMsgQueue& other );
	CommCompositeMsgQueue& operator =( const CommMsgQueue& other );
};

CommCompositeMsgQueue::~CommCompositeMsgQueue()
{
	_CommMsgQueueCompositeItem item;
	while( _Parent::pop( item ) )
		item.msg()->_CommMsg::~_CommMsg();
}

void CommCompositeMsgQueue::push( /*non-const!*/ _CommMsg& msg )//NOTE: PBlock/PString fields in msg will be destroyed
{
	_CommMsgQueueCompositeItem item;
#include "ppmemtracedisable.h"
	new( item.msg() ) _CommMsg( msg, true );//"moving" constructor
#include "ppmemtraceenable.h"
	_Parent::push( item );
}

void CommCompositeMsgQueue::push( /*non-const!*/ _CommMsg& msg, UINT32 * tickCounterMsg_ )//NOTE: PBlock/PString fields in msg will be destroyed
{
	_CommMsgQueueCompositeItem item;
#include "ppmemtracedisable.h"
	new( item.msg() ) _CommMsg( msg, true );//"moving" constructor
#include "ppmemtraceenable.h"
	tickCounterMsg_[0] = SYSTEMTICKS();  
	_Parent::push( item, tickCounterMsg_ + 1 );
}

int CommCompositeMsgQueue::waitPop( _CommMsg& ret, int timeout )
{
	_CommMsgQueueCompositeItem item;
	int retVal = _Parent::waitPop( item, timeout );
	if( retVal == waitPopOk )
		ret.moveFrom( *item.msg() );
	return retVal;
}

int CommCompositeMsgQueue::waitPop( _CommMsg& ret )//infinite
{
	_CommMsgQueueCompositeItem item;
	int retVal = _Parent::waitPop( item );
	if( retVal == waitPopOk )
		ret.moveFrom( *item.msg() );
	return retVal;
}

bool CommCompositeMsgQueue::pop( _CommMsg& ret )
{
	_CommMsgQueueCompositeItem item;
	if( _Parent::pop( item ) )
	{
		ret.moveFrom( *item.msg() );
		return true;
	}
	return false;
}






class _CommCompositeLocalThreadInterface : public _CommCompositeInterface
{
private:
	// from _CommLocalThreadInterface
	CommRoutingTable* routingTable;
	_CommTimer* timer;
	_CommPhysicalConnection* physConn;
	CommCompositeMsgQueue* q;

	// new for external phys connection
	friend class _CommCompositeLocalThread;

	_CommPhysicalConnection* externalPhysConn;
	CommThreadManager * mgr;
	bool externalConnected;
	bool inbound;
	UINT32 channelId;
	int slot;
	int threadId;


public:
	_CommCompositeLocalThreadInterface()
	{
		routingTable = 0;
		physConn = 0;
		q = 0;
		externalPhysConn=0;
		externalConnected=false;
		inbound=false;
	}
	void _construct( CommRoutingTable& routingTable_, _CommTimer& timer_, CommCompositeMsgQueue& q_,
		CommThreadManager &mgr_, int slot_, int threadId_)
	{
		routingTable = &routingTable_;
		timer = &timer_;
		q = &q_;
		mgr = &mgr_;
		slot = slot_ + _MAXLOCALTHREADOBJECTS;
		threadId = threadId_;
	}
	void _init( _CommPhysicalConnection* physConn_ )
	{
		physConn = physConn_;
	}
	void _deinit()
	{
		physConn = 0;
	}

public://_CommInterface
	void _post( _CommMsg& msg ) override
	{
		routingTable->routeIncomingMessage( physConn, 0, 0, msg );
	}
	UINT32 _postClientTimer( _CommMsg& msg, int msec ) override
	{
		msg.internal.channelId = physConn->channelId;
		msg.internal.slot = physConn->slot;
		return timer->addMsg( msg, msec );
	}
	UINT32 _addServerTimer( UINT32 msgId, CommMsgBody& body, int msec ) override
	{
		_CommMsg msg( _COMM_MSGTYPE_NONE, _COMM_MSGPRIORITY_INVALID );
		msg.body.moveFrom( body );
		msg.internal.channelId = physConn->channelId;
		msg.internal.slot = physConn->slot;
		msg.internal.patch0 = msgId;
		return timer->addMsg( msg, msec );
	}
	bool _delServerTimer( UINT32 handle ) override
	{
		return timer->delMsg( handle );
	}
	int _getInQueueSize() const override
	{
		return q ? q->size() : -1;
	}
	int _getOutQueueSize() const override
	{
		return 0;
	}


	void _setOutgoingExternalConnection( _CommPhysicalConnection* physConn_ )
	{
		externalPhysConn = physConn_;
		externalConnected = false;
		inbound = false;
	}
	void _postToPhysConnection(_CommMsg & msg, int srcMux, UINT32 srcMuxChannelId) override
	{
		if (externalPhysConn && externalConnected)
		{
			routingTable->routeIncomingMessage( externalPhysConn, srcMux, srcMuxChannelId, msg );
		}
		else
		{
			PTRACE3("_CommCompositeLocalThreadInterface:_postToPhysConnection - physical connection doesn't exist");
		}
	}
	void _disconnectPhysConnection() override
	{
		// our request for disconnect
		if (externalPhysConn)
		{
			if (externalConnected)
			{
				routingTable->physicallyDisconnected( externalPhysConn );
			}
			else
			{
				routingTable->physicalConnectFailed( externalPhysConn );
			}
			externalPhysConn= 0;
			externalConnected=false;
		}
	}
	void _physConnectionConnected(int isLocal) override
	{
		channelId = mgr->nextChannelId();
		routingTable->physicallyConnected( externalPhysConn, threadId, slot, channelId, isLocal );
		externalConnected = true;
	}
	void _createIncomingPhysConnection(const char * addr) override
	{
		if (externalPhysConn)
		{
			_physConnectionDisconnected();
		}
		externalPhysConn = routingTable->createIncomingPhysicalConnection( addr, mgr );
		externalConnected = false;
	}
	void _physConnectionDisconnected()
	{
		// reply on forceDisconnect from routingTable
		if (externalPhysConn)
		{
			routingTable->physicallyDisconnected( externalPhysConn );
			externalPhysConn= 0;
			externalConnected=false;
		}
	}
	const char * _getIdent() const 
	{
		return routingTable->_getIdent();
	}

};

struct _CommCompositeLocalThreadObject
{
	PString name;
	bool nameRegistered;
	_CommPhysicalConnection* physConn;
	_CommCompositeLocalThreadInterface inter;

	CommCompositeServerObject* obj;
	int channelId;
};

class _CommCompositeLocalThreadObjects
{
private:
	_CommCompositeLocalThreadObject servers[ _MAXLOCALTHREADOBJECTS ];

public:
	_CommCompositeLocalThreadObjects( CommRoutingTable& routingTable, _CommTimer& timer, CommCompositeMsgQueue& q, CommThreadManager &mgr_, int threadId_ );
	void shutdown();
	bool terminated();
	~_CommCompositeLocalThreadObjects();

public:
	_CommCompositeLocalThreadObject& operator[] ( int i )
	{
		PASSERT3( i >= 0 && i < _MAXLOCALTHREADOBJECTS );
		return servers[ i ];
	}
};

class _CommCompositeConnectThread : public PThread
{
public:
	typedef PQueue< PQDeque<_CommPhysicalConnection* > > Queue;

private:
	CommCompositeLocalThreadManager& mgr;
	Queue q;

public:
	_CommCompositeConnectThread( CommCompositeLocalThreadManager& mgr_ );
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
};
_CommCompositeConnectThread::_CommCompositeConnectThread( CommCompositeLocalThreadManager& mgr_ )
: mgr( mgr_ )
{
}

/* virtual */ void _CommCompositeConnectThread::run()
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
				mgr.pushOutConnection( conn );
				break;
			case Queue::waitPopShutdown:
				return;
			default:
				PASSERT3( 0 );
				break;
			}
		}
	}
	catch( PError& err )
	{
		PLog( "_CommCompositeConnectThread exception: %s", err.why() );
	}
}


class _CommCompositeLocalThread : public _CommManageableThread
{

private:
	friend class CommCompositeLocalThreadManager;
	CommCompositeLocalThreadManager & mgr;
	const CommCompositeServerObjectFactory* factory;
	CommCompositeMsgQueue qc;

	bool   slotActive[_MAXLOCALTHREADOBJECTS];
	UINT32 commQueueSize[_MAXLOCALTHREADOBJECTS];
	UINT32 queueSize;
	UINT32 timeInWait;


	int mgrIdx;

public:
	enum { _KeepAlivePoll = 1000 };

	void pushForceDisconnect( int slot, UINT32 channelId );
	_CommCompositeLocalThread( CommCompositeLocalThreadManager& mgr_, int mgrIdx_, const CommCompositeServerObjectFactory& factory_ );

	void run() override;
	UINT32 printCounters(int n,UINT32 & qSize, UINT32 & qTime, UINT32 & qSlotSize, UINT32 & qSlotTime) override
	{
		UINT32 _max = _CommManageableThread::printCounters(n,qSize,qTime,qSlotSize,qSlotTime);
		if (mgr.isGatherStatistic())
		{
			PLog("   QueueSize = %u, timeInWait = %u",queueSize,timeInWait);
			for (int i = 0;i<_MAXLOCALTHREADOBJECTS;i++)
			{
				if (slotActive[i])
					PLog("   slot %d - unconfirmed Queue Size = %u",i,commQueueSize[i]);
			}
		}
		return _max;
	}

private:
	bool _destroyServerObject( _CommTimer& timer, _CommCompositeLocalThreadObjects& servers, int slot );//returns mustExit
};

void _CommCompositeLocalThread::pushForceDisconnect( int slot, UINT32 channelId )
{
	_CommMsg m( _COMM_MSGTYPE_NONE, _COMM_MSGPRIORITIES - 1 );
	m.internal.slot = slot;
	m.internal.channelId = channelId;
	if (slot >= _MAXLOCALTHREADOBJECTS)
	{
		// this is tunneled phys connection, 
		m.internal.patch0=2; // Safe because patch0 is used for timer messages only.
	}
	qc.push( m );
}


_CommCompositeLocalThread::_CommCompositeLocalThread( CommCompositeLocalThreadManager& mgr_, int mgrIdx_, const CommCompositeServerObjectFactory& factory_ )
: mgr( mgr_ ),
factory( &factory_ )
{
	mgrIdx = mgrIdx_;
	timeInWait = 0;
	queueSize = 0;
	for (int i=0;i<_MAXLOCALTHREADOBJECTS;i++)
	{
		slotActive[i]=false;
		commQueueSize[i]=0;
	}
}


_CommCompositeLocalThreadObjects::_CommCompositeLocalThreadObjects( CommRoutingTable& routingTable, _CommTimer& timer, CommCompositeMsgQueue& q, 
																   CommThreadManager &mgr_, int threadId_)
{
	for( int i=0; i < _MAXLOCALTHREADOBJECTS ; ++i )
	{
		servers[ i ].physConn = 0;
		servers[ i ].inter._construct( routingTable, timer, q, mgr_, i , threadId_ );
		servers[ i ].obj = 0;
	}
}

void _CommCompositeLocalThreadObjects::shutdown()
{
	for( int i=0; i < _MAXLOCALTHREADOBJECTS ; ++i )
		if( servers[ i ].obj )
			servers[ i ].obj->_safeShutdown();
}

bool _CommCompositeLocalThreadObjects::terminated()
{
	for( int i=0; i < _MAXLOCALTHREADOBJECTS ; ++i )
		if( servers[ i ].obj && servers[ i ].obj->_safeTerminated() == 0 )
			return false;
	return true;
}

_CommCompositeLocalThreadObjects::~_CommCompositeLocalThreadObjects()
{
	for( int i=_MAXLOCALTHREADOBJECTS - 1; i >= 0; --i )
		delete servers[ i ].obj;
}

bool _CommCompositeLocalThread::_destroyServerObject( _CommTimer& timer, _CommCompositeLocalThreadObjects& servers, int slot )
{
	_CommCompositeLocalThreadObject& server = servers[ slot ];
	timer.delAllForSlot( slot );
	server.inter._deinit();
	mgr.routingTable.physicallyDisconnected( server.physConn );
	server.physConn = 0;
	server.inter._disconnectPhysConnection();
	delete server.obj;
	server.obj = 0;
	slotActive[slot]=false;
	mgr.removeServerObject(server.name);
	int saveMgrIdx = mgrIdx;
	if( mgr._freeObject( mgrIdx ) )
	{//no thread operations allowed after _freeObject returned true
		PTRACE3( "Exiting CompositeLocalThread #%d", saveMgrIdx );
		//exit(); - neither needed nor allowed as this is already deleted
		return true;
	}
	return false;
}

/* virtual */ void _CommCompositeLocalThread::run()
{
	try
	{
		_CommTimer timer;
		_CommCompositeLocalThreadObjects servers( mgr.routingTable, timer, qc, mgr, mgrIdx );
		UINT32 _timeInWait = 0 ;
		UINT32 _queueSize  = 0 ;
		UINT32 lastPoll = SYSTEMTICKS();
		for(;;)
		{
			int timeout;
			for(;;)
			{
				UINT32 now = SYSTEMTICKS();
				UINT32 diff = now - lastPoll;
				if( diff >= _KeepAlivePoll)
				{
					if (mgr.isGatherStatistic())
					{
						timeInWait = _timeInWait;
						queueSize = _queueSize;
						_timeInWait=0;
						_queueSize = 0;
					}
					for (int i=0;i<_MAXLOCALTHREADOBJECTS;i++)
					{
						if (slotActive[i] && servers[i].obj)
							commQueueSize[i]=servers[i].obj->getInternalQueueSize();
						else
							commQueueSize[i] = 0;
					}
					lastPoll = now;
					diff=0;
				}
				timeout = diff < _KeepAlivePoll ? _KeepAlivePoll - diff : 0;
				UINT32 timeoutTmp = timer.msecUntilFirst();
				timeout = timeout < timeoutTmp ? timeout : timeoutTmp;

				if( timeoutTmp == 0 )
				{
					_CommMsg msg;
					UINT32 timerHandle = timer.pop( msg );
					int slot = msg.internal.slot;

					if( msg.internal.channelId != servers[ slot ].channelId )
					{// Critical check. Protects from using wrong channel
						PLog( "CompositeLocalThread[ %d ]: timer message for obsolete channelId=%08X - ignored", mgrIdx, msg.internal.channelId );
						continue;//forever timer
					}

					_CommCompositeLocalThreadObject& server = servers[ slot ];

					bool destroy = false;
					if( msg.type == _COMM_MSGTYPE_NONE )
						destroy = server.obj->_safeProcessTimerMessage( timerHandle, msg.internal.patch0, msg.body );
					else
					{
						PASSERT3( msg.type == _COMM_MSGTYPE_CONNPOOL_TIMER || msg.type == _COMM_MSGTYPE_SUBSCRPOOL_TIMER ||
							msg.type == _COMM_MSGTYPE_CLICONNPOOL_TIMER_CONNECTION_EX || msg.type == _COMM_MSGTYPE_SRVCONNPOOL_TIMER_CONNECTION_EX);

						destroy = server.obj->_safeProcessMessage( msg );
					}

					if( destroy )
					{
						bool mustExit = _destroyServerObject( timer, servers, slot );
						if( mustExit )
							return;
					}
				}
				else
					break;
			}

			int w;
			_CommMsg msg;
			if (mgr.isGatherStatistic())
			{
				UINT32 tmp = SYSTEMTICKS();
				w = qc.waitPop( msg, timeout );
				_timeInWait += (SYSTEMTICKS() - tmp); 
				tmp = qc.size();
				_queueSize = max(_queueSize,tmp);
			}
			else
			{
				w = qc.waitPop( msg, timeout );

			}

			switch( w )
			{
			case CommCompositeMsgQueue::waitPopOk:
				{
					switch( msg.type )
					{
					case _COMM_MSGTYPE_NONE://forceDisconnect
						{
							int slot = msg.internal.slot;

							if (slot<_MAXLOCALTHREADOBJECTS)
							{
								PLog( "CompositeLocalThread[ %d ]: ServerObject( %d ) - forceDisconnect received", mgrIdx, slot );
								PASSERT3( slot >= 0 && slot < _MAXLOCALTHREADOBJECTS );
								if( msg.internal.channelId != servers[ slot ].channelId )
								{// Critical check. Protects from closing wrong channel
									PLog( "LocalThread[ %d ]: forceDisconnect for obsolete channelId=%08X - ignored", mgrIdx, msg.internal.channelId );
									break;
								}

								_CommCompositeLocalThreadObject& server = servers[ slot ];
								if( !server.nameRegistered )//no name yet and forceDisconnect? - destroy!
								{
									PLog( "CompositeLocalThread[ %d ]: ServerObject( %d ) has no registered name yet and forceDisconnect called - will be destroyed", mgrIdx, slot );
									bool mustExit = _destroyServerObject( timer, servers, slot );
									if( mustExit )
										return;
									break;//switch
								}

								_CommMsg m( _COMM_MSGTYPE_PHYSICAL_DISCONNECT, _COMM_MSGPRIORITY_INVALID );
								bool destroy = server.obj->_safeProcessMessage( m );
								if( destroy )
								{
									bool mustExit = _destroyServerObject( timer, servers, slot );
									if( mustExit )
										return;
									else
										break;
								}

								PLog( "CompositeLocalThread[ %d ]: ServerObject( %d ) - forceDisconnect: disconnecting...", mgrIdx, slot );
								server.inter._deinit();
								mgr.routingTable.physicallyDisconnected( server.physConn );
								server.physConn = 0;
								PLog( "CompositeLocalThread[ %d ]: ServerObject( %d ) - forceDisconnect: reconnecting...", mgrIdx, slot );
								server.physConn = mgr.routingTable.createIncomingPhysicalConnection( "pyrlocal://", &mgr );
								server.channelId = mgr.nextChannelId();
								mgr.routingTable.physicallyConnected( server.physConn, mgrIdx, slot, server.channelId, 3 );
								server.inter._init( server.physConn );

								//inform routing table about server object connected
								_CommMsg m2( _COMM_MSGTYPE_REGISTER_SERVEROBJECT, _COMM_MSGPRIORITIES - 1 );
								m2.body.composeUINT32( server.channelId ).composeString( server.name );
								server.inter._post( m2 );
								server.nameRegistered = false;
							}
							else 
							{
								slot = slot - _MAXLOCALTHREADOBJECTS;

								if (msg.internal.patch0 == 1)
								{
									PLog( "CompositeLocalThread[ %d ]: ServerObject( %d ) - connection request received", mgrIdx, slot );
									PASSERT3( slot >= 0 && slot < _MAXLOCALTHREADOBJECTS );

									_CommCompositeLocalThreadObject& server = servers[ slot ];

									// new outgoing connection 
									_CommPhysicalConnection* physConn;
									const BYTE * ptr;
									CommMsgParser parser( msg.body );
									parser._parseFixedBlock( ptr, sizeof(physConn) );
									physConn = *(_CommPhysicalConnection **) ptr;
									if (server.inter.externalPhysConn)
									{
										// New connection comes but we already have old connection
										server.inter._disconnectPhysConnection();
										bool destroy = server.obj->_safeProcessPhysDisconnect( );
										if( destroy )
										{
											bool mustExit = _destroyServerObject( timer, servers, slot );
											if( mustExit )
												return;
											else
												break;
										}

									}
									server.inter._setOutgoingExternalConnection(physConn);
									bool destroy = server.obj->_safeProcessOutgoingPhysConnect(physConn->address);
									if( destroy )
									{
										bool mustExit = _destroyServerObject( timer, servers, slot );
										if( mustExit )
											return;
										else
											break;
									}

								}
								else if (msg.internal.patch0 == 2)
								{
									PLog( "CompositeLocalThread[ %d ]: ServerObject( %d ) - forceDisconnect for external connection received", mgrIdx, slot );
									PASSERT3( slot >= 0 && slot < _MAXLOCALTHREADOBJECTS );
									if( msg.internal.channelId != servers[ slot ].inter.channelId )
									{// Critical check. Protects from closing wrong channel
										PLog( "LocalThread[ %d ]: forceDisconnect for obsolete channelId=%08X - ignored", mgrIdx, msg.internal.channelId );
										break;
									}
									_CommCompositeLocalThreadObject& server = servers[ slot ];

									server.inter._physConnectionDisconnected();
									bool destroy = server.obj->_safeProcessPhysDisconnect( );
									if( destroy )
									{
										bool mustExit = _destroyServerObject( timer, servers, slot );
										if( mustExit )
											return;
										else
											break;
									}
								}
							}
						}
						break;

					case _COMM_MSGTYPE_LOCAL_CREATESERVEROBJECT:
						{
							const char* name;
							CommMsgBody initMsg;
							CommMsgParser parser( msg.body );
							parser.parseString( name ).parseMsgBody( initMsg );
							PLog( "CompositeLocalThread[ %d ]: Creating Server '%s'", mgrIdx, name );

							int slot = -1;
							for( int i=0; i < _MAXLOCALTHREADOBJECTS ; ++i )
								if( servers[ i ].physConn == 0 )
								{
									slot = i;
									break;
								}
							PASSERT3( slot >= 0 );
							_CommCompositeLocalThreadObject& server = servers[ slot ];

							server.physConn = mgr.routingTable.createIncomingPhysicalConnection( "pyrlocal://", &mgr );
							server.channelId = mgr.nextChannelId();
							mgr.routingTable.physicallyConnected( server.physConn, mgrIdx, slot, server.channelId, 3 );
							server.inter._init( server.physConn );
							server.obj = factory->createServerObject( server.inter ); 


							bool destroy = server.obj->_safeInit( initMsg );
							if( destroy )
							{
								bool mustExit = _destroyServerObject( timer, servers, slot );
								if( mustExit )
									return;
								else
									break;
							}
							slotActive[slot]=true;

							//inform routing table about server object connected
							_CommMsg m( _COMM_MSGTYPE_REGISTER_SERVEROBJECT, _COMM_MSGPRIORITIES - 1 );
							m.body.composeUINT32( server.channelId ).composeString( name );
							server.inter._post( m );
							server.nameRegistered = false;
							server.name = name;

							PLog( "LocalThread[ %d ]: ServerObject '%s' created in slot %d", mgrIdx, name, slot );
						}
						break;

					case _COMM_MSGTYPE_REGISTER_SERVEROBJECT_REPLY:
						{
							BYTE err;
							UINT32 reqId;
							CommMsgParser parser( msg.body );
							parser.parseUINT32( reqId ).parseBYTE( err );

							int slot = -1;
							for( int i=0; i < _MAXLOCALTHREADOBJECTS ; ++i )
								if( servers[ i ].channelId == reqId )
								{
									slot = i;
									break;//for( i )
								}
							if( slot < 0 )
							{
								PLog( "CompositeLocalThread[ %d ]: _COMM_MSGTYPE_REGISTER_SERVEROBJECT_REPLY with unknown id=%08X - ignored", mgrIdx, reqId );
								break;//switch
							}

							_CommCompositeLocalThreadObject& server = servers[ slot ];
							if( err )
							{
								PLog( "CompositeLocalThread[ %d ]: ServerObject( %d ): Unable to register name with CommRoutingTable (err=%02X) - object destroyed", mgrIdx, slot, err );
								timer.delAllForSlot( slot );
								bool mustExit = _destroyServerObject( timer, servers, slot );
								if( mustExit )
									return;
								break;//switch
							}

							PLog( "CompositeLocalThread[ %d ]: ServerObject( %d ) registered", mgrIdx, slot );

							server.nameRegistered = true;
							//inform server object about connection
							_CommMsg m( _COMM_MSGTYPE_PHYSICAL_CONNECT, _COMM_MSGPRIORITY_INVALID );
							bool destroy = server.obj->_safeProcessMessage( m );
							if( destroy )
							{
								bool mustExit = _destroyServerObject( timer, servers, slot );
								if( mustExit )
									return;
								//else
								//	break;
							}
							mgr.addServerObject(server.name,mgrIdx,slot);
						}
						break;
					case  _COMM_MSGTYPE_SERVICEREQUEST:
						{
							CommMsgParser parser( msg.body );
							UINT32 id;
							parser.parseUINT32(id);
							if (id == _SERVICE_REQUEST_DUMP)
							{
								rundump();
								break;
							}
						}
						break;

					default:
						{
							int slot = msg.internal.slot;

							if (slot < _MAXLOCALTHREADOBJECTS)
							{
								PASSERT3( slot >= 0 && slot < _MAXLOCALTHREADOBJECTS );
								if( msg.internal.channelId != servers[ slot ].channelId )
								{// Critical check. Protects from dispatching the message to the wrong channel
									// when connection was deleted and then another connection created in the same slot.
									// Also protects from using failed connection: in case of close
									// servers[ slot ].channelId must be changed
									PTRACE3( "CompositeLocalThread[ %d ]: message to obsolete channelId=%08X - ignored", mgrIdx, msg.internal.channelId );
									break;
								}

								_CommCompositeLocalThreadObject& server = servers[ slot ];
								bool destroy;
								if( msg.internal.flags & _CommMsg::Patch8 )
								{
									_CommMsg m;
									CommThreadManager::_copyPatchMsg( m, msg );
									destroy = server.obj->_safeProcessMessage( m );
								}
								else
									destroy = server.obj->_safeProcessMessage( msg );
								if( destroy )
								{
									bool mustExit = _destroyServerObject( timer, servers, slot );
									if( mustExit )
										return;
									else
										break;
								}
							}
							else 
							{
								slot = slot - _MAXLOCALTHREADOBJECTS;
								PASSERT3( slot >= 0 && slot < _MAXLOCALTHREADOBJECTS );
								if( msg.internal.channelId != servers[ slot ].inter.channelId )
								{// Critical check. Protects from dispatching the message to the wrong channel
									// when connection was deleted and then another connection created in the same slot.
									// Also protects from using failed connection: in case of close
									// servers[ slot ].channelId must be changed
									PTRACE3( "CompositeLocalThread[ %d ]: phys connection message to obsolete channelId=%08X - ignored", mgrIdx, msg.internal.channelId );
									break;
								}
								_CommCompositeLocalThreadObject& server = servers[ slot ];
								bool destroy = server.obj->_safeProcessPhysMessage( msg );
								if( destroy )
								{
									bool mustExit = _destroyServerObject( timer, servers, slot );
									if( mustExit )
										return;
									//else
									//	break;
								}

							}
						}
						break;
					}
				}
				break;

			case CommCompositeMsgQueue::waitPopShutdown:
				PTRACE3( "CompositeLocalThread #%d shutdown - exiting", mgrIdx );

				servers.shutdown();
				_CommWaitUntilTerminated( servers );
				PLog( "CompositeLocalThread: objects terminated" );

				//					exit();
				return;

			case CommCompositeMsgQueue::waitPopTimeout:
				//do nothing - action will be done in the beginning of the forever cycle
				break;
			}
		}
	}
	catch( PError& err )
	{
		PLog( "CompositeLocalThread[ %d ] exception: %s", mgrIdx, err.why() );
	}
	//	exit();
}

//****************************************************************************

CommCompositeLocalThreadManager::CommCompositeLocalThreadManager( CommRoutingTable& routingTable_ )
: CommThreadManager( routingTable_,CommThreadManager::_flagVirtualPush )
{
	started = false;
	connThread = new _CommCompositeConnectThread( *this );
}

CommCompositeLocalThreadManager::~CommCompositeLocalThreadManager( ) 
{
	PWLock lock( cs );
	delete connThread;
}
void CommCompositeLocalThreadManager::virtualPush( _CommPhysicalConnection* physConn, /*non-const!*/ _CommMsg& m )
{
	PWLock lock( cs );
	_CommCompositeLocalThread* thread = static_cast< _CommCompositeLocalThread* > (threads[ physConn->thread ].thread);
	if( !thread )
	{
		PLog( "CommCompositeLocalThreadManager: Thread #%d is already deleted - msg ignored", physConn->thread );
		return;
	}
	thread->qc.push( m );
}
void CommCompositeLocalThreadManager::virtualPushRead( _CommPhysicalConnection* physConn, /*non-const!*/ _CommMsg& m )
{
	_CommCompositeLocalThread* thread = static_cast< _CommCompositeLocalThread* > (threads[ physConn->thread ].thread);
	if( !thread )
	{
		PLog( "CommCompositeLocalThreadManager: Thread #%d is already deleted - msg ignored", physConn->thread );
		return;
	}
	thread->qc.push( m );
}


void CommCompositeLocalThreadManager::pushOutConnection( _CommPhysicalConnection* physConn )
{
	PString ret;
	int dummy;
	parseSocketAddr(ret,dummy,physConn->address);

	PWLock lock( cs );
	_Slots::iterator found = slots.find( ret );

	if (found == slots.end())
	{
		lock.unlock();
		routingTable.physicalConnectFailed(physConn);
	}
	else
	{
		_CommMsg m( _COMM_MSGTYPE_NONE, _COMM_MSGPRIORITY_INITCONNECT );
		m.internal.slot = (*found).second.slotId + _MAXLOCALTHREADOBJECTS;
		m.internal.patch0 = 1;  // patch0 used for timer messages, it is safe to use it here =1 - connect, =2 disconnect

		m.body._composeFixedBlock( (const BYTE*)&physConn, sizeof(physConn) );
		Thread& thread = threads[ (*found).second.threadId ];
		(static_cast< _CommCompositeLocalThread* >(thread.thread))->qc.push( m );
	}

}

void CommCompositeLocalThreadManager::connect( _CommPhysicalConnection* physConn )
{
	connThread->pushConnection( physConn );
}


bool CommCompositeLocalThreadManager::createServerObject( const char* type, const char* name, const CommMsgBody* initMsg )
{
	_CommMsg m( _COMM_MSGTYPE_LOCAL_CREATESERVEROBJECT, _COMM_MSGPRIORITY_INITCONNECT );
	m.body.composeString( name );
	if( initMsg )
		m.body.composeMsgBody( *initMsg );
	else
	{
		CommMsgBody dummy;
		m.body.composeMsgBody( dummy );
	}

	PWLock lock( cs );
	_FactoriesType::iterator found = factories.find( type );
	if( found == factories.end() )
		return false;

	_Factory f = (*found).second;

	int threadIdx = -1;
	int n = threads.size();
	for( int i=0; i < n ; ++i )
	{
		Thread& thread = threads[ i ];
		if( thread.thread != 0 )
		{
			_CommCompositeLocalThread* localThread = static_cast< _CommCompositeLocalThread* >(thread.thread);//typecast is safe - always a _CommCompositeLocalThread*
			if( localThread->factory ==  f.factory  && thread.slotsLeft > 0 )
			{
				threadIdx = i;
				break;//for( i )
			}
		}
	}

	if( threadIdx < 0 )
	{
		threadIdx = _allocThread();
		Thread& thread = threads[ threadIdx ];
		thread.thread = new _CommCompositeLocalThread( *this, threadIdx, (*f.factory) );
		thread.maxSlots = thread.slotsLeft = f.maxObjectsPerThread;
		thread.thread->start();
		PTRACE3( "Started Local thread #%d - type '%s'", threadIdx, type );
	}

	Thread& thread = threads[ threadIdx ];
	PASSERT3( thread.slotsLeft > 0 );
	--thread.slotsLeft;
	PTRACE3( "Sending CREATESERVER msg to local thread #%d", threadIdx );
	(static_cast< _CommCompositeLocalThread* >(thread.thread))->qc.push( m );
	return true;
}

void CommCompositeLocalThreadManager::registerServerObjectFactory( const char* typeName, CommCompositeServerObjectFactory& factory, int maxObjectsPerThread )
{
	PWLock lock( cs );
	_Factory f;
	f.factory = &factory;
	f.maxObjectsPerThread = maxObjectsPerThread;
	factories.insert( _FactoriesType::value_type( typeName, f ) );
}

void CommCompositeLocalThreadManager::addServerObject(const char * name, int threadId, int slotId)
{
	PWLock lock( cs );
	slots.insert(_Slots::value_type(name,_Slot(threadId,slotId)));
}

void CommCompositeLocalThreadManager::removeServerObject(const char * name)
{
	PWLock lock( cs );
	_Slots::iterator found = slots.find( name );
	if( found != slots.end() )
	{
		slots.erase(found);
	}
}

void CommCompositeLocalThreadManager::forceDisconnect( _CommPhysicalConnection* physConn )
{
	PWLock lock( cs );
	Thread& th = threads[ physConn->thread ];
	_CommCompositeLocalThread* thread = static_cast< _CommCompositeLocalThread* >( th.thread );//safe because th.thread is always created by CommCompositeLocalThreadManager as _CommCompositeLocalThread
	thread->pushForceDisconnect( physConn->slot, physConn->channelId );
}


bool CommCompositeLocalThreadManager::_freeObject( int threadIdx )//no thread operations allowed after freeSocket returned true//no thread operations allowed after _freeObject returned true
{
	PWLock lock( cs );
	Thread& thread = threads[ threadIdx ];
	if( ++thread.slotsLeft != thread.maxSlots )
		return false;

	//{+++ to preserve some threads for the future use - return false
	//}

	delete thread.thread;
	thread.thread = 0;
	return true;
}

void CommCompositeLocalThreadManager::start()
{
	PWLock lock( cs );
	PASSERT3( !started );
	connThread->start();
	started = true;
}

void CommCompositeLocalThreadManager::shutdown()
{
	PWLock lock( cs );
	connThread->shutdown();
	int n = threads.size();
	for( int i=0; i < n ; ++i )
	{
		Thread& thread = threads[ i ];
		if( thread.thread != 0 )
		{
			_CommCompositeLocalThread* th = static_cast< _CommCompositeLocalThread* >( thread.thread );
			th->qc.shutdown();
		}
	}
}

bool CommCompositeLocalThreadManager::terminated()
{
	PWLock lock( cs );
	if( !CommThreadManager::terminated() )
		return false;
	if( !connThread->terminated() )
		return false;
	return true;
}


/* virtual */ void CommCompositeLocalThreadManager::parseSocketAddr( PString& retAddr, int& port, const char* address )
{
	CommCompositeProtocolHandler::parseSocketAddr( retAddr, address );
	port=0;
}

/* virtual */ void CommCompositeLocalThreadManager::composeSocketAddr( PString& retAddr, const char* address, int port )
{
	CommCompositeProtocolHandler::composeSocketAddr( retAddr, address );
}


/* virtual */ void CommCompositeLocalThreadManager::setOption( const char* optName, const char* optValue )
{
	if( strcmp( optName, COMM_OPTION_CLIENTDISCONNECT ) == 0 )
		forceClientDisconnect = atoi( optValue ) != 0;
	else if( strcmp( optName, COMM_OPTION_ALLOWCLIENTDISCONNECT ) == 0 )
		allowClientDisconnect = atoi( optValue ) != 0;
	else
		CommThreadManager::setOption( optName, optValue );
}
