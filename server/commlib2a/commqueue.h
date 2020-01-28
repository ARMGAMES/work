#ifndef commqueue_h_included
#define commqueue_h_included

#include "ppinclude.h"
#include "ppblock.h"
#include "ppthread.h"
#include "pperror.h"
#include "pptime.h"
#include "pplogfile.h"

#include "commmsgbody.h"
#include <unordered_map>


//****************************************************************************	

#define _COMM_MSGPRIORITIES							  16	//fundamental constant. MUST NOT be changed afer deployment
#define _COMM_MSGPRIORITY_INITCONNECT				   6
#define _COMM_MSGPRIORITY_INITSUBSCR				   6
#define _COMM_MSGPRIORITY_HIGHPRIORITY_DISCONNECT	  15
#define _COMM_MSGPRIORITY_INVALID					0xFF

//****************************************************************************	

#define _COMM_MAXHOPS								32

//****************************************************************************	

#define _COMM_MSGTYPE_NONE								0x00//invalid value

#define _COMM_MSGTYPE_PHYSICAL_CONNECT					0x01
//->Pools ""
#define _COMM_MSGTYPE_PHYSICAL_DISCONNECT				0x02
//->Pools ""
#define _COMM_MSGTYPE_LOCAL_CREATESERVEROBJECT			0x03
//->ServerObjectThread "sm" serverObjectName, initMsg
#define _COMM_MSGTYPE_REGISTER_SERVEROBJECT				0x04
//ServerObjectThread->RoutingTable "4s" rqId, serverObjectName
#define _COMM_MSGTYPE_REGISTER_SERVEROBJECT_REPLY		0x05
//RoutingTable->ServerObjectThread "41" rqId, errCode
#define _COMM_MSGTYPE_UNREGISTER_SERVEROBJECT			0x06
//Only for multiplexed connections
//ServerObjectThread->RoutingTable
//NO REPLY
#define _COMM_MSGTYPE_SERVICEREQUEST                    0x07
// from RouteTable to threads
#define _COMM_MSGTYPE_DESTROY_SERVEROBJECT		        0x08

#define _COMM_MSGTYPE_CONNPOOL_TIMER					0x0E
//Timer->ConnPool "4" connId
#define _COMM_MSGTYPE_SUBSCRPOOL_TIMER					0x0F
//Timer->SubscrPool "4" subscrId

//NOTE: srvId/cliId cannot be 0 unless otherwise specified
#define _COMM_MSGTYPE_CONNECT_REQUEST					0x10
//C->S "4sss" ("s")*  ("s")* cliId, server, serverObject, serverObjectConnectionType, supportedGuardList(ends with empty string), physicalAddressList(ends with empty string)
#define _COMM_MSGTYPE_CONNECT_REQUEST_ACCEPTED			0x11
//S->C "44" cliId, srvId
//#define _COMM_MSGTYPE_CONNECT_REQUEST_PENDING			0x12
//attempt of physical connection failed - will be repeated
//S->C "44s" cliId, srvId, failedAddress
#define _COMM_MSGTYPE_CONNECT_REQUEST_GRANTED			0x13
//S->C "441s" cliId, srvId, priority, guardName

#define _COMM_MSGTYPE_CONNECT_GUARD_REQUEST				0x14
//C->S "444"+ srvId, cliId, sessionId +guard-dependant
#define _COMM_MSGTYPE_CONNECT_GUARD_ADD_RESPONSE		0x15
//S->C "444"+ cliId, srvId, sessionId +guard-dependant
#define _COMM_MSGTYPE_CONNECT_GUARD_ADD_REQUEST			0x16
//C->S "444"+ srvId, cliId, sessionId +guard-dependant
#define _COMM_MSGTYPE_CONNECT_GUARD_REQUEST_GRANTED		0x17
//S->C "4441"+ cliId, srvId, sessionId, priority +guard-dependant

#define _COMM_MSGTYPE_CONNECT_CLIENT_USERMSG			0x1E
//C->S "4444?"+ srvId, cliId, sessionId, msgId, format +msg
#define _COMM_MSGTYPE_CONNECT_SERVER_USERMSG			0x1F
//S->C "4444?"+ cliId, srvId, sessionId, msgId, format +msg

#define _COMM_MSGTYPE_CONNECT_CLIENT_DISCONNECT			0x20
//C->S "444" srvId, cliId, sessionId
#define _COMM_MSGTYPE_CONNECT_SERVER_DISCONNECT			0x21
//S->C "444" cliId, srvId, sessionId
#define _COMM_MSGTYPE_CONNECT_CLIENT_ERROR				0x22
//C->S "442s" srvId, cliId, errCode, errMsg
//special case: srvId==0 allowed for breaking non-accepted connections
#define _COMM_MSGTYPE_CONNECT_SERVER_ERROR				0x23
//S->C "442s" cliId, srvId, errCode, errMsg
//special case: srvId==0 allowed for breaking non-accepted connections
#define _COMM_MSGTYPE_CONNECT_SERVER_FATALERROR			0x24
//S->C "442s" cliId, srvId, errCode, errMsg
//special case: srvId==0 allowed for breaking non-accepted connections
//NOTE: DISCONNECT means 'graceful disconnect', i.e. peer is no longer interested in communication on this connection
//		ERROR means 'recoverable error', handling should be similar to physical disconnect handling
//		FATAL_ERROR means 'non-recoverable error' (e.g. authentication failure) - does not make sense to repeat without changes
#define _COMM_MSGTYPE_CONNECT_CLIENT_TRANSITDISCONNECT	0x25
//->S "44"* srvId, cliId
//special case: srvId==0 allowed for breaking non-accepted connections
#define _COMM_MSGTYPE_CONNECT_SERVER_TRANSITDISCONNECT	0x26
//->C "44"* cliId, srvId

#define _COMM_MSGTYPE_SUBSCRIPTION_REQUEST				0x30
//C->S "4sss"+ cliId, server, serverObject, serverObjectChannel, +request
#define _COMM_MSGTYPE_SUBSCRIPTION_REQUEST_ACCEPTED		0x31
//S->C "44" cliId, srvId
#define _COMM_MSGTYPE_SUBSCRIPTION_RESPONSE				0x32
//S->C "441"+ cliId, srvId, priority, +response
#define _COMM_MSGTYPE_SUBSCRIPTION_ONLINE_UPDATE		0x33
//S->C "44"+ cliId, srvId, +update
#define _COMM_MSGTYPE_SUBSCRIPTION_UNSUBSCRIBE			0x34
//C->S "44" srvId, cliId
//special case: srvId==0 allowed for breaking non-accepted subscriptions
#define _COMM_MSGTYPE_SUBSCRIPTION_FATALERROR			0x35
//S->C "442s" cliId, srvId, errCode, errMsg
//special case: srvId==0 allowed for breaking non-accepted subscriptions
#define _COMM_MSGTYPE_SUBSCRIPTION_TRANSITDISCONNECT	0x36
//->C "44" cliId, srvId
#define _COMM_MSGTYPE_SUBSCRIPTION_RESYNC				0x37
//S->C "44"+ cliId, srvId, +response

#define _COMM_MSGTYPE_SUBSCRIPTION_REQUEST_EX_PHASE1    0x38

#define _COMM_MSGTYPE_SUBSCRIPTION_RESPONSE_EX_PHASE1   0x39

#define _COMM_MSGTYPE_SUBSCRIPTION_REQUEST_EX_PHASE2    0x3A

#define _COMM_MSGTYPE_SUBSCRIPTION_RESPONSE_EX_PHASE2   0x3B


#define _COMM_MSGTYPE_CONNQUALITY_CLIMONITOR			0x40
//C-> "44" srvId, cliId
#define _COMM_MSGTYPE_CONNQUALITY_CLIUPDATE				0x41
//->C "441" cliId, srvId, quality(0-100)
#define _COMM_MSGTYPE_CONNQUALITY_SRVMONITOR			0x42
//S-> "44" cliId, srvId
#define _COMM_MSGTYPE_CONNQUALITY_SRVUPDATE				0x43
//->S "441" srvId, cliId, quality(0-100)
#define _COMM_MSGTYPE_CONNQUALITY_SRVREQISALIVE			0x44
//S-> "44" cliId, srvId
#define _COMM_MSGTYPE_CONNQUALITY_SRVISALIVE			0x45
//->S "44" srvId, cliId

#define _COMM_MSGTYPE_SUBSCRIPTION_DEFROST				0x4B
//C->S "4sss"+ cliId, server, serverObject, serverObjectChannel, +request

#define _COMM_MSGTYPE_SUBSCRIPTION_FREEZE				0x4C
//->S srvId, cliId

#define _COMM_MSGTYPE_SUBSCRIPTION_COUNT_REQUEST		0x4D
//S-> peerId, thisId 0/1

#define _COMM_MSGTYPE_SUBSCRIPTION_COUNT_UPDATE 		0x4E
//C->S thisId, peerId, UIN32

#define _COMM_MSGTYPE_CONNECT_CLIENT_HIGHPRIORITY_TRANSITDISCONNECT	0x4F
//->S "44"* srvId, cliId
//special case: srvId==0 allowed for breaking non-accepted connections
#define _COMM_MSGTYPE_CONNECT_SERVER_HIGHPRIORITY_TRANSITDISCONNECT	0x50
//->C "44"* cliId, srvId

#define _COMM_MSGTYPE_CLICONNPOOL_TIMER_CONNECTION_EX	0x51
#define _COMM_MSGTYPE_SRVCONNPOOL_TIMER_CONNECTION_EX	0x52



#define _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_REQUEST				0x53
//C->S "4sss"+ cliId, server, serverObject, serverObjectChannel, +request
#define _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_REQUEST_ACCEPTED		0x54
//S->C "44" cliId, srvId
#define _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_RESPONSE				0x55
//S->C "441"+ cliId, srvId, priority, +response
#define _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_ONLINE_UPDATE			0x56
//S->C "44"+ cliId, srvId, +update
#define _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_UNSUBSCRIBE			0x57
//C->S "44" srvId, cliId
//special case: srvId==0 allowed for breaking non-accepted subscriptions
#define _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_FATALERROR				0x58
//S->C "442s" cliId, srvId, errCode, errMsg
//special case: srvId==0 allowed for breaking non-accepted subscriptions
#define _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_TRANSITDISCONNECT		0x59
//->C "44" cliId, srvId
#define _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_COUNT_REQUEST			0x5A
//S-> peerId, thisId 0/1

#define _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_COUNT_UPDATE 			0x5B
//C->S thisId, peerId, UIN32

#define _COMM_MSGTYPE_SUBSCRLIGHTPOOL_TIMER						0x5C
//Timer->SubscrPool "4" subscrId


#define _COMM_MSGTYPE_ANDROID_INTERNAL							0x81
#define _COMM_MSGTYPE_INTERNAL_COMMUNICATION					0x82
#define _COMM_MSGTYPE_INTERNAL_COMMUNICATION_CLOSE				0x83

#define _COMM_MSGTYPE_LOWLEVEL              			0xff

//****************************************************************************

// ID of service requests for transport level

#define _SERVICE_REQUEST_FORCE_KEEP_ALIVE               0x01
#define _SERVICE_REQUEST_KEEP_ALIVE                     0x02
#define _SERVICE_REQUEST_QUALITY                        0x03
#define _SERVICE_REQUEST_SETIPADDRESS                   0x04
#define _SERVICE_REQUEST_PROCESS_INIT                   0x05
#define _SERVICE_REQUEST_PROCESS_CONNECT                0x06
#define _SERVICE_REQUEST_PROCESS_DISCONNECT             0x07
#define _SERVICE_REQUEST_SERVICE_CALL                   0x08
#define _SERVICE_REQUEST_PROCESS_CONNECT_FAILED         0x09
#define _SERVICE_REQUEST_DUMP					        0x0a
#define _SERVICE_REQUEST_STATISTIC                      0x0b

#define _TRANSPORT_KEEPALIVE_DELAYED                    110
#define _TRANSPORT_KEEPALIVE_RESTORED                   111
#define _TRANSPORT_KEEPALIVE_CONNECT                    112


// Errors - disconnects reasons:

#define _SOCKET_TIMEOUT_EXPIRED			                0xf001
#define _SOCKET_HANDSHAKE_TIMEOUT_EXPIRED               0xf002
#define _SOCKET_EXCEPTION   			                0xf003
#define _SOCKET_SSLCERTIFICATEFAILED	                0xf004



//****************************************************************************

#define _COMM_ERROR_CLOSEDBYPEER					0x0001
#define _COMM_ERROR_AUTHFAILED						0x0002
#define _COMM_ERROR_NOLOCALOBJECT					0x0003
#define _COMM_ERROR_NOROUTETOHOST					0x0004
#define _COMM_ERROR_NOCONNTYPE						0x0005
#define _COMM_ERROR_NOACCEPTABLEGUARD				0x0006
//#define _COMM_ERROR_SERVERSHUTDOWN					0x0007
#define _COMM_ERROR_NOPUBLISHER						0x0008
#define _COMM_ERROR_INBOUNDCONNCLOSED				0x0009
#define _COMM_ERROR_ROUTINGLOOP						0x000A
#define _COMM_ERROR_SUBSCRIPTION_UNPUBLISHED		0x000B
#define _COMM_ERROR_PHYSICALMULTIPLEFAILURES		0x000C
#define _COMM_ERROR_AUTHFAILED_PARTIAL				0x000D
#define _COMM_ERROR_AUTHFAILED_SEVERE				0x000E
#define _COMM_ERROR_CHANGEPROTOCOL					0x000F
#define _COMM_ERROR_PROTOCOLNOTSUPPORTED			0x0010
#define _COMM_ERROR_PHYSICAL_DISCONNECTED			0x0011
#define _COMM_ERROR_INCORRECT_PARAMETERS			0x0012
#define _COMM_ERROR_INCOMPATIBLE_TYPE				0x0013
#define _COMM_ERROR_SSLCERTIFICATEFAILED			0x0014
#define _COMM_ERROR_AUTHFAILED_EXTRA				0x0015
#define _COMM_ERROR_AUTHFAILED_EXPIRED				0x0016
#define _COMM_ERROR_AUTHFAILED_PARTIAL_TEMPPIN		0x0017

#define _COMM_ERROR_USERERROR						0x0F00

//****************************************************************************

#define _COMM_MSGFLAGS_CONNECTION_EX                0x01
#define _COMM_MSGFLAGS_CONNECTION_EX_RECONNECT      0x02

class _CommMsg
{
public:
	enum { NPriorities = _COMM_MSGPRIORITIES };
	enum InternalFlags { Patch8 = 0x01 };
	struct _Internal
	{
		UINT16 flags; // enum InternalFlags
		UINT16 mux;
		UINT32 muxChannelId;
		UINT16 slot;
		UINT32 channelId;
		UINT32 patch0;//has special meaning (msgId) for _CommTimer msgs
		UINT32 patch1;//has special meaning (msec) for _CommTimerThread msgs
	};

public:
	_Internal internal;
	BYTE priority;
	UINT16 flags;//right now - 0
	BYTE type;
	CommMsgBody body;

public:
	_CommMsg();
	_CommMsg( BYTE type_, BYTE priority_ );
	_CommMsg( BYTE type_, BYTE priority_, /*non-const!*/ CommMsgBody& body_ );
	_CommMsg( BYTE type_, BYTE priority_, /*non-const!*/ CommMsgBody& body_, const CommMsgBody::_RefCountFlag& );
	_CommMsg( _CommMsg& other, bool /*true*/ );//'moving' constructor

	void moveFrom( _CommMsg& other );

private:
	/* inline */ void _copyHdr( const _CommMsg& other );

private://prohibited
	_CommMsg( const _CommMsg& other );
	_CommMsg& operator =( const _CommMsg& other );
};

template< class T >
class CommPriorityQueue
	//Deque array-based priority queue for PQueue
	//T prereqs: NPriorities enum && priority() func with priority() >= 0 && priority() < NPriorities
	//priority() == 0 is the lowest one
{
private:
	size_t sz;
	/* mutable */ int maxPriority;
	PQDeque< T > qs[ T::NPriorities ];

public:
	typedef T value_type;

public:
	CommPriorityQueue()
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
		qs[ p ].push( t );
		if( p > maxPriority )
			maxPriority = p;
		++sz;
	}
	const T& peek() const
	{
		PASSERT3( sz > 0 );
		for(;;)
		{
			PASSERT3( maxPriority >= 0 );
			const PQDeque< T >& q = qs[ maxPriority ];
			if( q.size() )
				return q.peek();
			--((CommPriorityQueue< T >*)this)->maxPriority;//should be just --maxPriority if it is mutable
		}
	}
	void pop()
	{
		PASSERT3( sz > 0 );
		for(;;)
		{
			PASSERT3( maxPriority >= 0 );
			PQDeque< T >& q = qs[ maxPriority ];
			if( q.size() )
			{
				q.pop();
				--sz;
				return;
			}
			--maxPriority;
		}
	}
};

struct _CommMsgQueueItem
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
		return (const _CommMsg*)_data;
	}

public://CommPriorityQueue requirements
	enum { NPriorities = _CommMsg::NPriorities };
	int priority() const
	{
		return ((const _CommMsg*)_data)->priority;
	}
};

// Warning: implementation of the CommMsgSimpleQueue is tricky, designed to minimize unnecessary data copying
// Functionally it is almost equivalent to CommPriorityQueue< CommMsg >
class CommMsgSimpleQueue : private CommPriorityQueue< _CommMsgQueueItem >
{
private:
	typedef CommPriorityQueue< _CommMsgQueueItem > _Parent;

public:
	CommMsgSimpleQueue() {}
	~CommMsgSimpleQueue();

public:
	void push( /*non-const!*/ _CommMsg& msg );
	const _CommMsg& peek() const
	{
		PASSERT3( size() > 0 );
		return (const _CommMsg&)_Parent::peek();
	}
	_CommMsg& peek()
	{
		PASSERT3( size() > 0 );
		return (_CommMsg&)_Parent::peek();
	}
	void pop();
	void clear();

public:
	using _Parent::size;

private://prohibited
	CommMsgSimpleQueue( const CommMsgSimpleQueue& other );
	CommMsgSimpleQueue& operator =( const CommMsgSimpleQueue& other );
};





// Warning: implementation of the CommMsgQueue is tricky, designed to minimize unnecessary data copying
// Functionally it is almost equivalent to PQueue< CommPriorityQueue< _CommMsg > >
class CommMsgQueue : private PQueue< CommPriorityQueue< _CommMsgQueueItem > >
{
private:
	typedef PQueue< CommPriorityQueue< _CommMsgQueueItem > > _Parent;

public:
	enum { waitPopOk = _Parent::waitPopOk, waitPopTimeout = _Parent::waitPopTimeout, waitPopShutdown = _Parent::waitPopShutdown };

public:
	CommMsgQueue() {}
	~CommMsgQueue();

	PCriticalSection& getCriticalSection()
	{
		return _Parent::getCriticalSection();
	}

public:
	void push( /*non-const!*/ _CommMsg& msg );
	void push( /*non-const!*/ _CommMsg& msg, UINT32 * tickCounterMsg_ );
	int waitPop( _CommMsg& ret, int timeout );
	int waitPop( _CommMsg& ret );//infinite
	bool pop( _CommMsg& ret );

public:
	using _Parent::shuttingDown;
	using _Parent::size;
	using _Parent::sizeAndReset;
	using _Parent::getSocket;
	using _Parent::shutdown;
	using _Parent::getCounters;
	using _Parent::increaseAutoResetCounter;

private://prohibited
	CommMsgQueue( const CommMsgQueue& other );
	CommMsgQueue& operator =( const CommMsgQueue& other );
};



struct _CommMsgQueueTimeControlItem
{
private:
	BYTE _data[ sizeof( _CommMsg ) + sizeof(UINT32) ];

public:
	_CommMsg* msg()
	{
		return (_CommMsg*)_data;
	}
	const _CommMsg* msg() const
	{
		return (const _CommMsg*)_data;
	}
	UINT32 *ticks() 
	{
		return (UINT32*)(_data + sizeof(_CommMsg));
	}


public://CommPriorityQueue requirements
	enum { NPriorities = _CommMsg::NPriorities };
	int priority() const
	{
		return ((const _CommMsg*)_data)->priority;
	}
};

class CommMsgSimpleTimeControlQueue : private CommPriorityQueue< _CommMsgQueueTimeControlItem >
{
private:
	typedef CommPriorityQueue< _CommMsgQueueTimeControlItem > _Parent;

public:
	CommMsgSimpleTimeControlQueue() {}
	~CommMsgSimpleTimeControlQueue();

public:
	void push( /*non-const!*/ _CommMsg& msg );
	const _CommMsg& peek() const
	{
		PASSERT3( size() > 0 );
		return (const _CommMsg&)_Parent::peek();
	}
	_CommMsg& peek()
	{
		PASSERT3( size() > 0 );
		return (_CommMsg&)_Parent::peek();
	}
	void pop(UINT32 & ticks_);
	void clear();

public:
	using _Parent::size;

private://prohibited
	CommMsgSimpleTimeControlQueue( const CommMsgSimpleTimeControlQueue& other );
	CommMsgSimpleTimeControlQueue& operator =( const CommMsgSimpleTimeControlQueue& other );
};

class CommMsgTimeControlQueue : private PQueue< CommPriorityQueue< _CommMsgQueueTimeControlItem > >
{
private:
	typedef PQueue< CommPriorityQueue< _CommMsgQueueTimeControlItem > > _Parent;

public:
	enum { waitPopOk = _Parent::waitPopOk, waitPopTimeout = _Parent::waitPopTimeout, waitPopShutdown = _Parent::waitPopShutdown };

public:
	CommMsgTimeControlQueue() {}
	~CommMsgTimeControlQueue();

public:
	void push( /*non-const!*/ _CommMsg& msg );
	void push( /*non-const!*/ _CommMsg& msg, UINT32 * tickCounterMsg_ );
	int waitPop( _CommMsg& ret, int timeout );
	int waitPop( _CommMsg& ret );//infinite
	bool pop( _CommMsg& ret );
	bool pop( _CommMsg& ret, UINT32 & ticks_ );

public:
	using _Parent::shuttingDown;
	using _Parent::size;
	using _Parent::sizeAndReset;
	using _Parent::getSocket;
	using _Parent::shutdown;
	using _Parent::getCounters;
	using _Parent::increaseAutoResetCounter;

private://prohibited
	CommMsgTimeControlQueue( const CommMsgTimeControlQueue& other );
	CommMsgTimeControlQueue& operator =( const CommMsgTimeControlQueue& other );
};




//****************************************************************************

/* abstract */ class _CommInterface
{
public:
	virtual ~_CommInterface() {}
	virtual void _post( _CommMsg& msg ) = 0;
	virtual UINT32 _postClientTimer( _CommMsg& msg, int msec ) = 0;//MUST be implemented for both client and server

public://server-side only
	virtual UINT32 _addServerTimer( UINT32 msgId, CommMsgBody& body, int msec ) = 0;//returns handle; may be empty for clients-only
	virtual bool _delServerTimer( UINT32 handle ) = 0;//returns false if handle is not found; may be empty for clients-only
	virtual const char * _getIdent() const = 0;

public://optional
	virtual int _getInQueueSize() const { return -1; }//returns -1 if information is not available
	virtual UINT64 _getWaitInQueueTime(UINT64 *readTime , UINT64 *writeTime ) { return (UINT64) -1; }//returns -1 if information is not available
	virtual void _internalPost(_CommMsg& msg ){return;}
	virtual int _getOutQueueSize() const = 0;
};

/* abstract */ class _CommClientOnlyInterface : public _CommInterface
	//defines server-side function stubs that just throw an exception when called
{
private:
	UINT32 _addServerTimer( UINT32 msgId, CommMsgBody& body, int msec ) override;
	bool _delServerTimer( UINT32 handle ) override;
	const char * _getIdent() const override;
};

//****************************************************************************

class _CommTimer
{
private:
	struct _Item
	{
		UINT32 handle;
		PTime time;
		_CommMsg msg;
	};
	struct _CmpItem
	{
		bool operator() ( const _Item* a, const _Item* b ) const
		{
			return a->time.compare( b->time ) < 0;
		}
	};

	using TimersSet = multiset < /* new */ _Item*, _CmpItem >;
	using HandlesMap = unordered_map< UINT32 /*handle*/, TimersSet::const_iterator >;

private:
	UINT32 nextHandle;
	TimersSet timers;
	HandlesMap handles;
	PCriticalSection* cs;

private:
	UINT32 _addMsg( /*non-const!*/ _CommMsg& msg, int msec);//returns timer msg handle
	bool _delMsg(UINT32 handle);//returns false if handle is not found
	int _msecUntilFirst() const;
	int _msecUntilFirstOrParam(int untilMsec) const;
	UINT32 _pop(_CommMsg& msg);
	void _delAllForSlot(int slot);

public:
	_CommTimer();
	_CommTimer(PCriticalSection& cs_);
	~_CommTimer();

	UINT32 addMsg( /*non-const!*/ _CommMsg& msg, int msec );//returns timer msg handle
	bool delMsg( UINT32 handle );//returns false if handle is not found
	int msecUntilFirst() const;
	int msecUntilFirstOrParam(int untilMsec) const;
	UINT32 pop( _CommMsg& msg );
	void delAllForSlot( int slot );

	_CommTimer( const _CommTimer& other ) = delete;
	_CommTimer& operator =( const _CommTimer& other ) = delete;
};


#endif
