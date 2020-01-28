#ifndef pudttimer_h_included
#define pudttimer_h_included
#include "ppinclude.h"
#ifdef UDT_DEVELOPMENT
#include "pperror.h"
#include "ppthread.h"
#include "pplogfile.h"

class PUDTTimerQueue
{
private:
	struct _Item
	{
		UINT64 timeStamp;
		UINT32 connId;
	};



	struct _CmpItem : public binary_function< const _Item&, const _Item&, bool >
	{
	public:
		bool operator() ( const _Item& a, const _Item& b ) const
		{
			UINT64 diff = a.timeStamp - b.timeStamp;
			return diff > 0x7fffffffffffffff;
		}
	};
	typedef deque< _Item > _Vector; //maintained sorted by _CmpItem
	_Vector v;


public:

	static UINT64 getTimestamp();
	PUDTTimerQueue();
	void addTimer( UINT32 connId, INT64  usec );
	bool delTimer( UINT32 connId );
	bool usecUntilFirst(INT64 & usec);
	UINT32 pop();
};

class PUDTTimerQueueMap
{
private:

	typedef  map<UINT32, UINT64> _ConnIdMap; 
	typedef _ConnIdMap::iterator ConnIdMapIterator;

	_ConnIdMap    connIdMap;

	struct _Item
	{
		UINT64 timeStamp;
		UINT32 connId;
	};

	struct _CmpItem : public binary_function< const _Item&, const _Item&, bool >
	{
	public:
		bool operator() ( const _Item& a, const _Item& b ) const
		{
			return a.timeStamp < b.timeStamp;
		}
	};
	typedef deque< _Item > _Vector; //maintained sorted by _CmpItem
	_Vector v;

	/* mutable */ PCriticalSection cs;
	_PWin32Event _event;
	bool _shutdown;
	

public:

	enum { waitPopOk = 0, waitPopShutdown = 1, waitPopTimeout = 2, waitPopTimer = 3 };
	static UINT64 getTimestamp();
	PUDTTimerQueueMap();
	bool shuttingDown() const;
	size_t haveMoreAndReset() ;
	SOCKET getSocket() const; //for interaction with PMultiSocketRW
	void addTimer(UINT32 connId, INT64  usec, bool reschedule);
	int waitPop( UINT32 & connId, UINT64 & usec, int timeout );
	void shutdown();
private:
	bool _addConnection(UINT32 connId, INT64  usec, bool reschedule);
	bool _delTimer( UINT32 connId );
	bool _usecFirst(UINT64 & usec);
	bool _haveMore(UINT64 current);
	UINT32 _pop(UINT64 & usec );
	bool _eraseTimer( UINT32 connId );
	void _addTimer( UINT32 connId, INT64  usec );
private://prohibited
	PUDTTimerQueueMap( const PUDTTimerQueueMap& );
	PUDTTimerQueueMap& operator =( const PUDTTimerQueueMap& );


};





#endif
#endif
