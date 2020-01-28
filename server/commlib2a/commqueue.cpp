#include "ppinclude.h"
#include "ppblock.h"
#include "ppthread.h"
#include "pperror.h"
#include "pplogfile.h"

#include "commmsgbody.h"
#include "commqueue.h"

inline void _CommMsg::_copyHdr( const _CommMsg& other )
{
	internal = other.internal;

	priority = other.priority;
	flags = other.flags;
	type = other.type;
}

_CommMsg::_CommMsg()
{
	internal.flags = 0;
	type = _COMM_MSGTYPE_NONE;
	flags = 0;
	priority = _COMM_MSGPRIORITY_INVALID;
}

_CommMsg::_CommMsg( BYTE type_, BYTE priority_ )
{
	internal.flags = 0;
	type = type_;
	flags = 0;
	priority = priority_;
}

_CommMsg::_CommMsg( BYTE type_, BYTE priority_, /*non-const!*/ CommMsgBody& body_ )
: body( body_, true )
{
	internal.flags = 0;
	type = type_;
	flags = 0;
	priority = priority_;
}

_CommMsg::_CommMsg( BYTE type_, BYTE priority_, /*non-const!*/ CommMsgBody& body_, const CommMsgBody::_RefCountFlag& flag )
: body( body_, flag )
{
	internal.flags = 0;
	type = type_;
	flags = 0;
	priority = priority_;
}

_CommMsg::_CommMsg( _CommMsg& other, bool /*true*/ )//'moving' constructor
: body( other.body, true )
{
	_copyHdr( other );
}

void _CommMsg::moveFrom( _CommMsg& other )
{
	body.moveFrom( other.body );
	_copyHdr( other );
}

//****************************************************************************

CommMsgQueue::~CommMsgQueue()
{
	_CommMsgQueueItem item;
	while( _Parent::pop( item ) )
		item.msg()->_CommMsg::~_CommMsg();
}

void CommMsgQueue::push( /*non-const!*/ _CommMsg& msg )//NOTE: PBlock/PString fields in msg will be destroyed
{
	_CommMsgQueueItem item;
#include "ppmemtracedisable.h"
	new( item.msg() ) _CommMsg( msg, true );//"moving" constructor
#include "ppmemtraceenable.h"
	_Parent::push( item );
}

void CommMsgQueue::push( /*non-const!*/ _CommMsg& msg, UINT32 * tickCounterMsg_ )//NOTE: PBlock/PString fields in msg will be destroyed
{
	_CommMsgQueueItem item;
#include "ppmemtracedisable.h"
	new( item.msg() ) _CommMsg( msg, true );//"moving" constructor
#include "ppmemtraceenable.h"
	tickCounterMsg_[0] = SYSTEMTICKS();  
	_Parent::push( item, tickCounterMsg_ + 1 );
}

int CommMsgQueue::waitPop( _CommMsg& ret, int timeout )
{
	_CommMsgQueueItem item;
	int retVal = _Parent::waitPop( item, timeout );
	if( retVal == waitPopOk )
		ret.moveFrom( *item.msg() );
	return retVal;
}

int CommMsgQueue::waitPop( _CommMsg& ret )//infinite
{
	_CommMsgQueueItem item;
	int retVal = _Parent::waitPop( item );
	if( retVal == waitPopOk )
		ret.moveFrom( *item.msg() );
	return retVal;
}

bool CommMsgQueue::pop( _CommMsg& ret )
{
	_CommMsgQueueItem item;
	if( _Parent::pop( item ) )
	{
		ret.moveFrom( *item.msg() );
		return true;
	}
	return false;
}

//****************************************************************************

void CommMsgSimpleQueue::clear()
{
	while( size() )
		pop();
}

CommMsgSimpleQueue::~CommMsgSimpleQueue()
{
	clear();
}

void CommMsgSimpleQueue::push( /*non-const!*/ _CommMsg& msg )
{
	_CommMsgQueueItem item;
#include "ppmemtracedisable.h"
	new( (&item) ) _CommMsg( msg, true );//"moving" constructor
#include "ppmemtraceenable.h"
	_Parent::push( item );
}

void CommMsgSimpleQueue::pop()
{
	_CommMsg& msg = (_CommMsg&)_Parent::peek();
	msg._CommMsg::~_CommMsg();
	_Parent::pop();
}



void CommMsgSimpleTimeControlQueue::clear()
{
	UINT32 ticks_;
	while( size() )
		pop(ticks_);
}

CommMsgSimpleTimeControlQueue::~CommMsgSimpleTimeControlQueue()
{
	clear();
}

void CommMsgSimpleTimeControlQueue::push( /*non-const!*/ _CommMsg& msg )
{
	_CommMsgQueueTimeControlItem item;
#include "ppmemtracedisable.h"
	new( (&item) ) _CommMsg( msg, true );//"moving" constructor
#include "ppmemtraceenable.h"
	*item.ticks() = SYSTEMTICKS();
	_Parent::push( item );
}

void CommMsgSimpleTimeControlQueue::pop(UINT32 & ticks_)
{
	_CommMsgQueueTimeControlItem& msgItem = (_CommMsgQueueTimeControlItem&)_Parent::peek();
	ticks_ = SYSTEMTICKS() - *msgItem.ticks();
	msgItem.msg()->_CommMsg::~_CommMsg();
	_Parent::pop();
}


CommMsgTimeControlQueue::~CommMsgTimeControlQueue()
{
	_CommMsgQueueTimeControlItem item;
	while( _Parent::pop( item ) )
		item.msg()->_CommMsg::~_CommMsg();
}

void CommMsgTimeControlQueue::push( /*non-const!*/ _CommMsg& msg )//NOTE: PBlock/PString fields in msg will be destroyed
{
	_CommMsgQueueTimeControlItem item;
#include "ppmemtracedisable.h"
	new( item.msg() ) _CommMsg( msg, true );//"moving" constructor
#include "ppmemtraceenable.h"
	*item.ticks() = SYSTEMTICKS();
	_Parent::push( item );
}

void CommMsgTimeControlQueue::push( /*non-const!*/ _CommMsg& msg, UINT32 * tickCounterMsg_ )//NOTE: PBlock/PString fields in msg will be destroyed
{
	_CommMsgQueueTimeControlItem item;
#include "ppmemtracedisable.h"
	new( item.msg() ) _CommMsg( msg, true );//"moving" constructor
#include "ppmemtraceenable.h"
	tickCounterMsg_[0] = SYSTEMTICKS();
	*item.ticks() = SYSTEMTICKS();
	_Parent::push( item, tickCounterMsg_ + 1 );
}

int CommMsgTimeControlQueue::waitPop( _CommMsg& ret, int timeout )
{
	_CommMsgQueueTimeControlItem item;
	int retVal = _Parent::waitPop( item, timeout );
	if( retVal == waitPopOk )
		ret.moveFrom( *item.msg() );
	return retVal;
}

int CommMsgTimeControlQueue::waitPop( _CommMsg& ret )//infinite
{
	_CommMsgQueueTimeControlItem item;
	int retVal = _Parent::waitPop( item );
	if( retVal == waitPopOk )
		ret.moveFrom( *item.msg() );
	return retVal;
}

bool CommMsgTimeControlQueue::pop( _CommMsg& ret )
{
	_CommMsgQueueTimeControlItem item;
	if( _Parent::pop( item ) )
	{
		ret.moveFrom( *item.msg() );
		return true;
	}
	return false;
}

bool CommMsgTimeControlQueue::pop( _CommMsg& ret, UINT32 & ticks_ )
{
	_CommMsgQueueTimeControlItem item;
	if( _Parent::pop( item ) )
	{
		ret.moveFrom( *item.msg() );
		ticks_ = SYSTEMTICKS() - *item.ticks();
		return true;
	}
	return false;
}

//****************************************************************************

/* virtual */ UINT32 _CommClientOnlyInterface::_addServerTimer( UINT32 msgId, CommMsgBody& body, int msec )
{
	PASSERT3( 0 );
	return 0;
}

/* virtual */ bool _CommClientOnlyInterface::_delServerTimer( UINT32 handle )
{
	PASSERT3( 0 );
	return false;
}
/* virtual */ const char * _CommClientOnlyInterface::_getIdent() const 
{
	return "";
}


//****************************************************************************

_CommTimer::_CommTimer()
{
	nextHandle = 0;
	cs = nullptr;
}

_CommTimer::_CommTimer(PCriticalSection& cs_)
{
	nextHandle = 0;
	cs = &cs_;
}

_CommTimer::~_CommTimer()
{
	for (const _Item* item : timers)
	{
		delete item;
	}
}

UINT32 _CommTimer::_addMsg( /*non-const!*/ _CommMsg& msg, int msec )//returns handle
{
	PASSERT3( msec >= 0 );
	_Item* item = new _Item();
	if( nextHandle == 0 )
		++nextHandle;
	item->handle = nextHandle++;
	item->time = PTime::now();
	item->time.add( msec );
	item->msg.moveFrom( msg );
	handles[item->handle] = timers.insert(item);
	return item->handle;
}

UINT32 _CommTimer::addMsg( /*non-const!*/ _CommMsg& msg, int msec)//returns handle
{
	if (cs)
	{
		PLock lock(*cs);
		return _addMsg(msg, msec);
	}
	else
	{
		return _addMsg(msg, msec);
	}
}

bool _CommTimer::_delMsg(UINT32 handle)
{
	auto handleIt = handles.find(handle);
	if (handleIt == handles.end())
		return false;

	_Item* item = *handleIt->second;
	timers.erase(handleIt->second);
	handles.erase(handleIt);
	delete item;
	return true;
}

bool _CommTimer::delMsg(UINT32 handle)
{
	if (cs)
	{
		PLock lock(*cs);
		return _delMsg(handle);
	}
	else
	{
		return _delMsg(handle);
	}
}

int _CommTimer::_msecUntilFirst() const
{
	if (timers.empty())
		return -1;
	const _Item* item = *timers.cbegin();
	const PTime& t = item->time;
	int ret = t.diff(PTime::now());
	ret = (ret > 3600 * 24 * 1000) ? (3600 * 24 * 1000) : ret;
	return ret >= 0 ? ret : 0;
}

int _CommTimer::msecUntilFirst() const
{
	if (cs)
	{
		PLock lock(*cs);
		return _msecUntilFirst();
	}
	else
	{
		return _msecUntilFirst();
	}
}

int _CommTimer::_msecUntilFirstOrParam(int untilMsec) const
{
	if( timers.empty() && untilMsec < 0 )
		return -1;
	int ret = untilMsec;
	if( !timers.empty() )
	{
		const _Item* item = *timers.cbegin();
		const PTime& t = item->time;
		int ret1 = t.diff( PTime::now() );
	    ret = ( ret1 > ret ) ? ret : ret1; 
	}
	return ret >= 0 ? ret : 0;
}

int _CommTimer::msecUntilFirstOrParam(int untilMsec) const
{
	if (cs)
	{
		PLock lock(*cs);
		return _msecUntilFirstOrParam(untilMsec);
	}
	else
	{
		return _msecUntilFirstOrParam(untilMsec);
	}
}

UINT32 _CommTimer::_pop( _CommMsg& msg )
{
	PASSERT3( !timers.empty() );
	_Item* item = *timers.cbegin();
	UINT32 ret = item->handle;
	msg.moveFrom( item->msg );
	timers.erase(timers.cbegin());
	handles.erase(item->handle);
	delete item;
	return ret;
}

UINT32 _CommTimer::pop(_CommMsg& msg)
{
	if (cs)
	{
		PLock lock(*cs);
		return _pop(msg);
	}
	else
	{
		return _pop(msg);
	}
}

void _CommTimer::_delAllForSlot( int slot )
{
	for( auto it = timers.begin(); it != timers.end(); /*increment in the loop*/ )
	{
		const _Item* item = *it;
		if (item->msg.internal.slot == slot)
		{
			it = timers.erase(it);
			handles.erase(item->handle);
			delete item;
		}
		else
		{
			++it;
		}
	}
}

void _CommTimer::delAllForSlot(int slot)
{
	if (cs)
	{
		PLock lock(*cs);
		_delAllForSlot(slot);
	}
	else
	{
		_delAllForSlot(slot);
	}
}
