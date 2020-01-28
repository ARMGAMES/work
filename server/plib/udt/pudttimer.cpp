#include "ppinclude.h"
#ifdef UDT_DEVELOPMENT
#include "pudttimer.h"

UINT64 PUDTTimerQueue::getTimestamp()
{
	static LARGE_INTEGER ccf = {0,0};
	if (!ccf.QuadPart)
		QueryPerformanceFrequency(&ccf);
	LARGE_INTEGER cc;
	QueryPerformanceCounter(&cc);
	return ((cc.QuadPart * 1000000ULL) / ccf.QuadPart);
}

PUDTTimerQueue::PUDTTimerQueue()
{
}
void PUDTTimerQueue::addTimer( UINT32 connId, INT64  usec )
{
	PASSERT3(usec > 0);
	_Item item;
	item.timeStamp = getTimestamp() + usec;
	item.connId = connId;
	_Vector::iterator iter = lower_bound( v.begin(), v.end(), item, _CmpItem() );//sic! lower_bound not upper_bound
	v.insert( iter, item );
}
bool PUDTTimerQueue::delTimer( UINT32 connId )
{
	int n = v.size();
	for( int i = n-1; i >= 0 ; --i )
		if( v[ i ].connId == connId )
		{
			v.erase( v.begin() + i );
			return true;
		}
	return false;
}
bool PUDTTimerQueue::usecUntilFirst(INT64 & usec)
{
	if( !v.size() )
		return false;
	usec = v.front().timeStamp - getTimestamp(); 
	return true;
}
UINT32 PUDTTimerQueue::pop()
{
	PASSERT3( v.size() );
	UINT32 s = v.front().connId;
	v.pop_front();
	return s;
}

UINT64 PUDTTimerQueueMap::getTimestamp()
{
	static LARGE_INTEGER ccf = {0,0};
	if (!ccf.QuadPart)
		QueryPerformanceFrequency(&ccf);
	LARGE_INTEGER cc;
	QueryPerformanceCounter(&cc);
	return ((cc.QuadPart * 1000000ULL) / ccf.QuadPart);
}

PUDTTimerQueueMap::PUDTTimerQueueMap()
: _event( FALSE, FALSE )//auto reset, non-available
{
	_shutdown = false;
}

bool PUDTTimerQueueMap::shuttingDown() const
{
	return _shutdown;
}

size_t PUDTTimerQueueMap::haveMoreAndReset() 
{
	PLock lock( cs );
	bool reset = false;
	if (v.size() == 0)
	{
		reset=true;
	}
	else
	{
		UINT64 current =  getTimestamp();
		if (!_haveMore(current))
		{
			reset=true;
		}
	}
	if (reset)
	{
		_event.resetEvent();
		return false;
	}
	return true;
}


SOCKET PUDTTimerQueueMap::getSocket() const//for interaction with PMultiSocketRW
{
	return (SOCKET)_event.getHandle();
}



void PUDTTimerQueueMap::addTimer(UINT32 connId, INT64  usec, bool reschedule)
{
	if( _shutdown )
		return;
	bool _needSetEvent = false;
	bool empty;
	bool nothingprocessing=false;
	bool replaced=false;
	UINT64 current;
	{
		PLock lock( cs );
		current=getTimestamp();
		empty = !v.size();
		if (empty)
		{
			_addConnection(connId,usec, reschedule);
			if(usec < current)
				_needSetEvent = true;
		}
		else
		{
			nothingprocessing = !_haveMore(current);
			replaced = _addConnection(connId,usec, reschedule);
			if (replaced)
			{
				_needSetEvent = nothingprocessing == true && usec < current;
			}
		}
	}
//		PTRACE3("Timer event %s set: empty=%s,  nothingprocessing=%s, replaced=%s, reschedule=%s, u=%d",(_needSetEvent?"":"not"),(empty?"t":"f"),(nothingprocessing?"t":"f"),(replaced?"t":"f"),(reschedule?"t":"f"),(int)(usec==1?usec:(usec-current)));
	if (_needSetEvent)
	{
			
		_event.setEvent();
	}
}
int PUDTTimerQueueMap::waitPop( UINT32 & connId, UINT64 & usec, int timeout )
{
    UINT32 ticks = SYSTEMTICKS();
	DWORD result = WAIT_FAILED;
    for(;;)
    {
        if( _shutdown )
            return waitPopShutdown;
 
        PLock lock( cs );
 
        UINT64 current =  getTimestamp();
        if (_haveMore(current))
        {
            connId = _pop( usec );
            return waitPopTimer;
        }
        if (result == WAIT_TIMEOUT)
        {
            if (timeout != INFINITE)
            {
                timeout -=  SYSTEMTICKS() - ticks;
                if (timeout <= 0)
                    return waitPopTimeout;
                ticks = SYSTEMTICKS();
            }
        }
        int timeoutT = INFINITE;
        UINT64 ts;
        if (_usecFirst(ts))
        {
            timeoutT = (int) ((ts - current) / 1000);
            if (timeoutT <= 0)
                timeoutT = 1;
            if (timeout > 0 && timeout < timeoutT)
                timeoutT = timeout;
        }
        else
        {
            timeoutT = timeout;
        }
        _event.resetEvent();
        lock.unlock();
//UINT32 st =SYSTEMTICKS();
        result = ::WaitForSingleObject( _event.getHandle(), timeoutT );
        if( result == WAIT_FAILED )    
            throw PSystemError( "WaitForSingleObject failed", GetLastError() );
/*
else if(result == WAIT_TIMEOUT)
PTRACE3("After wait: waited for %d msec:, %d msec per ticks, %d usec",timeoutT,SYSTEMTICKS()-st, PUDTTimerQueue::getTimestamp() - current);
else
{
PTRACE3("After wait: waited for %d msec: set by EVENT, %d msec per ticks, %d usec",timeoutT,SYSTEMTICKS()-st, PUDTTimerQueue::getTimestamp() - current);
}
*/
    }
}






void PUDTTimerQueueMap::shutdown()
{
	PLock lock( cs );
	_shutdown = true;
	_event.setEvent();
}

bool PUDTTimerQueueMap::_addConnection(UINT32 connId, INT64  usec, bool reschedule)
{
	auto p = connIdMap.emplace(connId, usec);
	if (!p.second)
	{
		if (p.first->second > 0)
		{
			if (reschedule)
			{
				_eraseTimer(connId);
			}
			else
			{
				return false;
			}
		}
		p.first->second = usec;
	}
	_addTimer(connId,usec);
	return true;
}

bool PUDTTimerQueueMap::_delTimer( UINT32 connId )
{
	ConnIdMapIterator it = connIdMap.find(connId);
	if (it == connIdMap.end() || it->second == 0)
	{
		return false;
	}
	it->second = 0;
	return _eraseTimer(connId);
}
bool PUDTTimerQueueMap::_usecFirst(UINT64 & usec)
{
	if( !v.size() )
		return false;
	usec = v.front().timeStamp; 
	return true;
}
bool PUDTTimerQueueMap::_haveMore(UINT64 current)
{
	if( !v.size() )
		return false;
	UINT64 usec = v.front().timeStamp; 
	return usec <= current;
}

UINT32 PUDTTimerQueueMap::_pop(UINT64 & usec )
{
	PASSERT3( v.size() );
	UINT32 connId = v.front().connId;
	ConnIdMapIterator it = connIdMap.find(connId);
	it->second = 0;
	v.pop_front();
	return connId;
}


bool PUDTTimerQueueMap::_eraseTimer( UINT32 connId )
{
	int n = v.size();
	for( int i = n-1; i >= 0 ; --i )
		if( v[ i ].connId == connId )
		{
			v.erase( v.begin() + i );
			return true;
		}
	return false;
}
void PUDTTimerQueueMap::_addTimer( UINT32 connId, INT64  usec )
{
	PASSERT3(usec > 0);
	_Item item;
	item.timeStamp = usec;
	item.connId = connId;
	_Vector::iterator iter = lower_bound( v.begin(), v.end(), item, _CmpItem() );
	v.insert( iter, item );
}

#endif

