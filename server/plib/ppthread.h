#ifndef ppthread_h_included
#define ppthread_h_included

#include "ppinclude.h"
#include "pperror.h"


#ifndef PNOTHREADS

#ifdef PWIN32

#define PTHREAD_WIN32_PRIORITY_MINUS2	"-2"
#define PTHREAD_WIN32_PRIORITY_MINUS1	"-1"
#define PTHREAD_WIN32_PRIORITY_NORMAL	"0"
#define PTHREAD_WIN32_PRIORITY_DEFAULT	""
#define PTHREAD_WIN32_PRIORITY_PLUS1	"+1"
#define PTHREAD_WIN32_PRIORITY_PLUS2	"+2"
#define PTHREAD_WIN32_PRIORITY_15		"15"


class PThread
{
private:
	HANDLE handle;
	UINT32 _thrId;

public:
	
	PThread();
	void start();
	void join();
	bool terminated();
	virtual ~PThread();
	void setTimers() {}
	bool getTimers(UINT64 & kernelT, UINT64 & userT )
	{
		FILETIME creationTime;
		FILETIME exitTime;
		FILETIME kernelTime;
		FILETIME userTime;
		if (::GetThreadTimes(handle, &creationTime,&exitTime,&kernelTime,&userTime) == FALSE)
			return false;
		else
		{
			kernelT = kernelTime.dwHighDateTime;
			kernelT = (kernelT<<32) + kernelTime.dwLowDateTime;
			userT = userTime.dwHighDateTime;
			userT = (userT<<32) + userTime.dwLowDateTime;
			return true;
		}
	}
	UINT32 getThreadId() const 
	{
		return _thrId;
	}

public://overridable
	virtual void run() {}

public:
	void setThreadPriority( const char * priority);
	static void sleep( UINT32 msec );
	static void terminateCurrentProcess()//never exits
	{
		FATALEXIT();
	}
	static void setCurrentThreadPriority( const char* priority /* PTHREAD_WIN32_PRIORITY_* */ );
	//prototype is system-independent; priority values are system-dependent

private:
	static unsigned __stdcall _threadProc( void* parm );
	static void _initGcpn();

private://prohibited
	PThread( const PThread& );
	PThread& operator =( const PThread& );
};

typedef BOOL (WINAPI *_TryEnterCriticalSection)(
	LPCRITICAL_SECTION lpCs 
	);

class PCriticalSection
{
private:
	static int tecsInited;
	static _TryEnterCriticalSection tecs;

private:
	CRITICAL_SECTION cs;
#ifndef SUPPORT95
	UINT32 oldContentionCounter;
#endif

public:
	PCriticalSection()
	{
		
		if( tecsInited == 0 )
			_initTecs();
		::InitializeCriticalSection( &cs );
#ifndef SUPPORT95

		if ( cs.DebugInfo && cs.DebugInfo != (void*)(-1) )
		{
			oldContentionCounter = cs.DebugInfo->ContentionCount;
		}
		else
		{
			oldContentionCounter = 0;
		}
#endif
	}
	~PCriticalSection()
	{
		::DeleteCriticalSection( &cs );
	}

public://mostly for class PLock
	UINT32 getContentions()
	{
#ifndef SUPPORT95
		UINT32 oldCounter = oldContentionCounter;
		if ( cs.DebugInfo && cs.DebugInfo != (void*)(-1) )
		{
			oldContentionCounter = cs.DebugInfo->ContentionCount;
		}
		else
		{
			oldContentionCounter = 0;
		}

		return (oldContentionCounter - oldCounter);
#else
		return 0;
#endif
	}
	void _lock()
	{
		::EnterCriticalSection( &cs );
	}
	static bool tryLockSupported();//'false' means that 'lock' will be called instead
	bool _tryLock()
	{
		if( tecs )
			return tecs( &cs ) != 0;
		else
		{
			::EnterCriticalSection( &cs );
			return true;
		}
		//return ::TryEnterCriticalSection( &cs );
	}
	void _unlock()
	{
		::LeaveCriticalSection( &cs );
	}

private:
	static void _initTecs();

private://prohibited
	PCriticalSection( const PCriticalSection& );
	PCriticalSection& operator =( const PCriticalSection& );
};

class PInterlocked
{
private:
	UINT32 count;
public:
	PInterlocked(UINT32 initialCount)
	{
		count = initialCount;
	}
	~PInterlocked()
	{
	}

public:
	inline UINT32 increment()
	{
		return (UINT32) InterlockedIncrement((long*)&count);
	}
	inline bool decrement()
	{
		return InterlockedDecrement((long*)&count) == 0;
	}
};

class _PWin32Event//no Unix analog
{
private:
	HANDLE handle;

public:
	_PWin32Event( BOOL manualReset, BOOL set /*, const char* name = 0 */ );
	~_PWin32Event();

public:
	void setEvent();
	void pulseEvent();
	void resetEvent();

	HANDLE getHandle() const
	{
		return handle;
	}

private://prohibited
	_PWin32Event( const _PWin32Event& );
	_PWin32Event& operator =( const _PWin32Event& );
};

class _PWin32Semaphore//no Unix analog
{
private:
	HANDLE handle;

public:
	_PWin32Semaphore( INT32 initialCount, INT32 maximumCount );
	~_PWin32Semaphore();

public:
	void releaseSemaphore(INT32 count);

	HANDLE getHandle() const
	{
		return handle;
	}

private://prohibited
	_PWin32Semaphore( const _PWin32Semaphore& );
	_PWin32Semaphore& operator =( const _PWin32Semaphore& );
};


class PLock
{
private:
	PCriticalSection& cs;
	bool locked;

public:
	PLock( PCriticalSection& cs_ )
		: cs( cs_ )
	{
		lock();
	}
	~PLock()
	{
		if( locked )
			cs._unlock();
	}

	void lock()
	{
		cs._lock();
		locked = true;
	}
	void unlock()
	{
		PASSERT5( locked );
		locked = false;
		cs._unlock();
	}

private://prohibited
	PLock( const PLock& );
	PLock& operator =( const PLock& );
};

#define PQUEUE_AUTORESET 1

template< class Container >
class PQueue
	// IMPORTANT: current implementation supports only single waiting thread
	// Container requirements: value_type/size/push/peek/pop
{
public:
	typedef TYPENAME Container::value_type value_type;
	enum { waitPopOk = 0, waitPopShutdown = 1, waitPopTimeout = 2 };
	
	PCriticalSection& getCriticalSection()
	{
		return cs;
	}

private:
	/* mutable */ PCriticalSection cs;
	_PWin32Event _event;
	Container _queue;
	bool _shutdown;

	UINT32 autoReset;
	UINT32 manualReset;
	UINT32 setEvent;

public:
	PQueue()
#ifdef PQUEUE_AUTORESET
		: _event( FALSE, FALSE )//auto reset, non-available
#else
		: _event( TRUE, FALSE )//manual reset, non-available
#endif
	{
		_shutdown = false;
		autoReset=0;
		manualReset=0;
		setEvent=0;
	}

	Container& _getContainer() { return _queue; }

public:
	bool shuttingDown() const
	{
		return _shutdown;
	}
	void increaseAutoResetCounter()
	{
		autoReset++;
	}
	void getCounters(UINT32 & autoReset_, UINT32 & manualReset_, UINT32 & setEvent_, UINT32 & contentions)
	{
		autoReset_ = autoReset;
		manualReset_ = manualReset;
		setEvent_ = setEvent;
		contentions = cs.getContentions();
		autoReset = 0;
		manualReset = 0;
		setEvent = 0;
	}

	size_t size() const
	{
		//		PLock lock( const_cast<PCriticalSection&>( cs ) );//cs is mutable
		return _queue.size();
	}
	size_t sizeAndReset() 
	{
#ifdef PQUEUE_AUTORESET
		PLock lock( cs );
		size_t sz = _queue.size();
		if (sz == 0)
		{
			manualReset++;
			_event.resetEvent();
		}
		return sz;
#else
		PLock lock( cs );
		return _queue.size();
#endif
	}


	SOCKET getSocket() const//for interaction with PMultiSocketRW
	{
		return (SOCKET)_event.getHandle();
	}

	void push( const value_type& item );
	void push( const value_type& item, UINT32 * tickCounterMsg_ );
	int waitPop( value_type& item, int timeout );
	int waitPop( value_type& item )//infinite
	{
		return waitPop( item, INFINITE );
	}
	bool pop( value_type& item );

	void shutdown();

private:
	inline void _pop( value_type& item );
	//MUST be locked before call

private://prohibited
	PQueue( const PQueue< Container >& );
	PQueue< Container >& operator =( const PQueue< Container >& );
};

#ifdef PQUEUE_AUTORESET

template< class Container >
void PQueue< Container >::push(const TYPENAME PQueue::value_type& item )
{
	if( _shutdown )
		return;
	bool _needSetEvent;
	{
		PLock lock( cs );
		_needSetEvent = _queue.size() == 0;
		_queue.push( item );
	}
	if (_needSetEvent)
	{
		setEvent++;
		_event.setEvent();
	}
}

template< class Container >
void PQueue< Container >::push( const TYPENAME PQueue::value_type& item, UINT32 * tickCounterMsg_  )
{
	if( _shutdown )
		return;
	bool _needSetEvent;
	{
		PLock lock( cs );
		tickCounterMsg_[0] = SYSTEMTICKS();
		_needSetEvent = _queue.size() == 0;
		_queue.push( item );
		tickCounterMsg_[1] = SYSTEMTICKS();
	}
	if (_needSetEvent)
	{
		setEvent++;
		_event.setEvent();
	}
	tickCounterMsg_[2] = SYSTEMTICKS();
}

template< class Container >
inline void PQueue< Container >::_pop( TYPENAME PQueue::value_type& item )
{
	PASSERT3( _queue.size() != 0 );
	item = _queue.peek();
	_queue.pop();
}

template< class Container >
int PQueue< Container >::waitPop( TYPENAME PQueue::value_type& item, int timeout )
{
	for(;;)
	{
		if( _shutdown )
			return waitPopShutdown;

		PLock lock( cs );
		if( _queue.size() == 0 )
		{
			manualReset++;
			_event.resetEvent();
			lock.unlock();
			DWORD result = ::WaitForSingleObject( _event.getHandle(), timeout );
			if( result == WAIT_FAILED )
				throw PSystemError( "WaitForSingleObject failed", GetLastError() );
			else if( result == WAIT_TIMEOUT )
				return waitPopTimeout;
			autoReset++;
			//there is no need to decrease timeout, as 
			// a) _queue is not empty now and 
			// b) only one thread (this one) allowed to extract item,
			// so on the next for(;;) iteration _queue will not be empty
		}
		else
		{
			_pop( item );
			return waitPopOk;
		}
	}
}

#else //PQUEUE_AUTORESET

template< class Container >
void PQueue< Container >::push(const TYPENAME PQueue::value_type& item )
{
	PLock lock( cs );
	if( _shutdown )
		return;

	_queue.push( item );
	_event.setEvent();
}

template< class Container >
void PQueue< Container >::push( const TYPENAME PQueue::value_type& item, UINT32 * tickCounterMsg_  )
{
	PLock lock( cs );
	tickCounterMsg_[0] = SYSTEMTICKS();
	if( _shutdown )
		return;
	_queue.push( item );
	tickCounterMsg_[1] = SYSTEMTICKS();
	_event.setEvent();
	tickCounterMsg_[2] = SYSTEMTICKS();
}

template< class Container >
inline void PQueue< Container >::_pop( TYPENAME PQueue::value_type& item )
{
	PASSERT5( _queue.size() != 0 );
	item = _queue.peek();
	_queue.pop();
	if( _queue.size() == 0 )
		_event.resetEvent();
}

template< class Container >
int PQueue< Container >::waitPop( TYPENAME PQueue::value_type& item, int timeout )
{
	for(;;)
	{
		PLock lock( cs );
		if( _shutdown )
			return waitPopShutdown;

		if( _queue.size() == 0 )
		{
			lock.unlock();
			DWORD result = ::WaitForSingleObject( _event.getHandle(), timeout );
			if( result == WAIT_FAILED )
				throw PSystemError( "WaitForSingleObject failed", GetLastError() );
			else if( result == WAIT_TIMEOUT )
				return waitPopTimeout;
			//there is no need to decrease timeout, as 
			// a) _queue is not empty now and 
			// b) only one thread (this one) allowed to extract item,
			// so on the next for(;;) iteration _queue will not be empty
		}
		else
		{
			_pop( item );
			return waitPopOk;
		}
	}
}

#endif //PQUEUE_AUTORESET

template< class Container >
bool PQueue< Container >::pop( TYPENAME PQueue::value_type& item )
{

	if( _queue.size() == 0 )
		return false;
	PLock lock( cs );
	_pop( item );
	return true;
}

template< class Container >
void PQueue< Container >::shutdown()
{
	PLock lock( cs );
	_shutdown = true;
	_event.setEvent();
}

#elif defined ( PPOSIX )
class PCriticalSection
{
private:
	pthread_mutex_t mutex;

public:
	PCriticalSection()
	{
		INITRECURSIVEMUTEX( &mutex );
	}
	~PCriticalSection()
	{
		pthread_mutex_destroy( &mutex );
	}

public://mostly for class PLock
	UINT32 getContentions() const
	{
		return 0;
	}

	void _lock()
	{
		int ret = pthread_mutex_lock( &mutex );
		if( ret )
		{
			throw PSystemError( "Unable to lock mutex", ret );
		}
	}
	static bool tryLockSupported();//'false' means that 'lock' will be called instead
	bool _tryLock()
	{
		_lock();
		return true;
	}
	void _unlock()
	{
		int ret = pthread_mutex_unlock( &mutex );
		if( ret )
		{
			throw PSystemError( "Unable to unlock mutex", ret );
		}
	}

private://prohibited
	PCriticalSection( const PCriticalSection& );
	PCriticalSection& operator =( const PCriticalSection& );
};

class PThread
{
private:
	pthread_t thread;
	bool joinDetachHandled;
	pthread_mutex_t joinDetachMutex;
	UINT32 _thrId;
#if defined(LINUX)
	UINT64 kernelT;
	UINT64 userT;
	PCriticalSection performanceLock;
#endif //LINUX

public:

	PThread();
	void start();
	void join();
	bool terminated();//no implementation yet
	virtual ~PThread();
	bool getTimers(UINT64 & kernelT, UINT64 & userT );
	void setTimers();

	UINT32 getThreadId() const 
	{
		return _thrId;
	}

public://overridable
	virtual void run() {}

public:
	void setThreadPriority( const char * priority) {/* not supported yet */}
	static void sleep( UINT32 msec );
	static void terminateCurrentProcess()//never exits
	{
		FATALEXIT();
	}
	static void setCurrentThreadPriority( const char* priority ) { /* not supported yet */ }
	//prototype is system-independent; priority values are system-dependent

private:
	static void* _threadProc( void* parm );

private://prohibited
	PThread( const PThread& );
	PThread& operator =( const PThread& );
};


class PLock
{
private:
	PCriticalSection& cs;
	bool locked;

public:
	PLock( PCriticalSection& cs_ )
		: cs( cs_ )
	{
		lock();
	}
	~PLock()
	{
		if( locked )
			cs._unlock();
	}

	void lock()
	{
		cs._lock();
		locked = true;
	}
	void unlock()
	{
		PASSERT5( locked );
		locked = false;
		cs._unlock();
	}

private://prohibited
	PLock( const PLock& );
	PLock& operator =( const PLock& );
};

#if defined(LINUX)
class PUnixEvent
{
public:
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	bool raised;
	PUnixEvent()
	{
		pthread_mutex_init(&mutex,NULL);
		pthread_cond_init(&cond,NULL);
		raised=false;
	}
	~PUnixEvent()
	{
		pthread_mutex_destroy(&mutex);
		pthread_cond_destroy(&cond);
	}
	void setEvent();
	void wait();
	int wait(int timeout);
};
class PCriticalSectionSpin
{
private:
	pthread_spinlock_t spin;

public:
	PCriticalSectionSpin()
	{
		pthread_spin_init(&spin,PTHREAD_PROCESS_PRIVATE);
	}
	~PCriticalSectionSpin()
	{
		pthread_spin_destroy( &spin );
	}

public://mostly for class PLock
	UINT32 getContentions() const
	{
		return 0;
	}

	void _lock()
	{
		int ret = pthread_spin_lock( &spin );
		if( ret )
		{
			throw PSystemError( "Unable to lock spin", ret );
		}
	}
	static bool tryLockSupported();//'false' means that 'lock' will be called instead
	bool _tryLock()
	{
		_lock();
		return true;
	}
	void _unlock()
	{
		int ret = pthread_spin_unlock( &spin );
		if( ret )
		{
			throw PSystemError( "Unable to unlock spin", ret );
		}
	}

private://prohibited
	PCriticalSectionSpin( const PCriticalSectionSpin& );
	PCriticalSectionSpin& operator =( const PCriticalSectionSpin& );
};

class PInterlocked
{
public:
	INT32 count;
public:
	PInterlocked(UINT32 initialCount = 0)
	{
		count = initialCount;
	}
	~PInterlocked()
	{
	}

public:
	inline UINT32 increment()
	{
		return __sync_add_and_fetch(&count,1);
	}
	inline bool decrement()
	{
		return __sync_sub_and_fetch(&count,1) == 0;
	}
};

template< class Container >
class PQueue
	// IMPORTANT: current implementation supports only single waiting thread
	// Container requirements: value_type/size/push/peek/pop
{
public:
	typedef TYPENAME Container::value_type value_type;
	enum { waitPopOk = 0, waitPopShutdown = 1, waitPopTimeout = 2 };

private:
	int event;
	PCriticalSection cs;
	int ePoll;

	Container _queue;
	bool _shutdown;

public:
	PQueue();
	~PQueue();

	PCriticalSection& getCriticalSection()
	{
		return cs;
	}

public:
	bool shuttingDown() const
	{
		return _shutdown;
	}
	void getCounters(UINT32 & autoReset_, UINT32 & manualReset_, UINT32 & setEvent_, UINT32 & contentions)
	{
		autoReset_ = 0;
		manualReset_ = 0;
		setEvent_ = 0;
		contentions = 0;
	}
	void increaseAutoResetCounter() const
	{
	}
	size_t size() const
	{
		//		PLock lock( const_cast<PCriticalSection&>( cs ) );//cs is mutable
		return _queue.size();
	}
	size_t sizeAndReset()
	{
		//		PLock lock( const_cast<PCriticalSection&>( cs ) );//cs is mutable
		return _queue.size();
	}

	SOCKET getSocket() const//for interaction with PMultiSocketRW
	{
		return (SOCKET) event;
	}

	void push( const value_type& item );
	void push( const value_type& item, UINT32 * tickCounterMsg_ );
	int waitPop( value_type& item, int timeout );
	int waitPop( value_type& item )//infinite
	{
		return waitPop( item, -1 );
	}
	bool pop( value_type& item );

	void shutdown();

private:
	inline void _pop( value_type& item );
	//MUST be locked before call

private://prohibited
	PQueue( const PQueue< Container >& );
	PQueue< Container >& operator =( const PQueue< Container >& );
};

template< class Container >
PQueue< Container >::PQueue()
{
	event = eventfd(0,O_NONBLOCK|FD_CLOEXEC);
	if( event ==-1 )
		throw PSystemError( "Unable to create eventfd", errno );
	_shutdown = false;
#if defined(P_USE_EPOLL)
	ePoll=-1;
#endif
}

template< class Container >
PQueue< Container >::~PQueue()
{
	::close( event );
#if defined (P_USE_EPOLL)
	::close(ePoll);
#endif
}

template< class Container >
void PQueue< Container >::push( const TYPENAME PQueue::value_type& item )
{
	PLock lock( cs );
	if( _shutdown )
		return;
	bool _needWrite = _queue.size() == 0;

	_queue.push( item );

	if (_needWrite)
	{
		UINT64 a = 1;
		auto rd = ::write( event, &a, sizeof(a) );
		if( rd < 0 )
		{
			throw PSystemError( "Error writing to eventd", errno );
		}

	}
}

template< class Container >
void PQueue< Container >::push( const TYPENAME PQueue::value_type& item, UINT32 * tickCounterMsg_  )
{
	PLock lock( cs );
	tickCounterMsg_[0] = SYSTEMTICKS();
	if( _shutdown )
		return;
	bool _needWrite = _queue.size() == 0;
	_queue.push( item );
	tickCounterMsg_[1] = SYSTEMTICKS();

	if (_needWrite)
	{
		UINT64 a = 1;
		auto rd = ::write( event, &a, sizeof(a) );
		if( rd < 0 )
		{
			throw PSystemError( "Error writing to eventd", errno );
		}
	}
	tickCounterMsg_[2] = SYSTEMTICKS();
}

template< class Container >
inline void PQueue< Container >::_pop( TYPENAME PQueue::value_type& item )
{
	PASSERT5( _queue.size() != 0 );
	item = _queue.peek();
	_queue.pop();

	if( _queue.size() == 0 )
	{
#if defined (P_MAC10X)
		retry:
#endif
			 UINT64 b;
			 int rd = ::read( event, &b, sizeof(b) );
			 if( rd < 0 )
			 {
#if defined (P_MAC10X)
				 if( errno == EAGAIN ) // Temp unavail
				 {
					 PThread::sleep( 1 );
					 goto retry;
				 }
#endif
				 throw PSystemError( "Error reading from eventd", errno );
			 }
	}
}

template< class Container >
int PQueue< Container >::waitPop( TYPENAME PQueue::value_type& item, int timeout )
{
	for(;;)
	{
		PLock lock( cs );
		if( _shutdown )
			return waitPopShutdown;
#if defined(P_USE_EPOLL)
		if (ePoll == -1)
		{
			ePoll=epoll_create1(EPOLL_CLOEXEC);
			if( ePoll ==-1 )
				throw PSystemError( "Unable to create epoll", errno );
			struct epoll_event polls;
			polls.data.fd = event;
			polls.events = POLLIN;
			if (epoll_ctl(ePoll,EPOLL_CTL_ADD,polls.data.fd,&polls)==-1)
				throw PSystemError( "Unable to ctl epoll", errno );
		}
#endif


		if( _queue.size() == 0 )
		{
			lock.unlock();
#if defined (P_USE_EPOLL)
			struct epoll_event poll;
		retr_intr:
			int res = ::epoll_wait( ePoll,&poll,1,timeout );
#else
			struct pollfd poll;
			poll.fd = event;
			poll.events = POLLIN;

		retr_intr:
			int res = ::poll( &poll, 1, timeout );
#endif
			if( res < 0 )
			{
				if (errno == EINTR)
					goto retr_intr;
				throw PSystemError( "Error waiting for eventd data", errno );
			}
			else if( res == 0 )
				return waitPopTimeout;
		}
		else
		{
			_pop( item );
			return waitPopOk;
		}
	}
}


template< class Container >
bool PQueue< Container >::pop( TYPENAME PQueue::value_type& item )
{
	PLock lock( cs );
	if( _queue.size() == 0 )
		return false;
	_pop( item );
	return true;
}

template< class Container >
void PQueue< Container >::shutdown()
{
	PLock lock( cs );
	_shutdown = true;

	lock.unlock();

	UINT64 b = 1;
	auto rd = ::write( event, &b, sizeof(b));
	( void )rd;
}



#else  //LINUX


class PInterlocked
{
private:
	PCriticalSection sec;
	INT32 count;
public:
	PInterlocked(UINT32 initialCount)
	{
		count = initialCount;
	}
	~PInterlocked()
	{
	}

public:
	inline UINT32 increment()
	{
		sec._lock();
		UINT32 ret = ++count;
		sec._unlock();
		return ret;
	}
	inline bool decrement()
	{
		sec._lock();
		bool  result = --count == 0;
		sec._unlock();
		return result;
	}
};




template< class Container >
class PQueue
	// IMPORTANT: current implementation supports only single waiting thread
	// Container requirements: value_type/size/push/peek/pop
{
public:
	typedef TYPENAME Container::value_type value_type;
	enum { waitPopOk = 0, waitPopShutdown = 1, waitPopTimeout = 2 };

private:
	int pipeRd, pipeWr;//not protected by cs

	PCriticalSection cs;
	Container _queue;
	bool _shutdown;

public:
	PQueue();
	~PQueue();

	PCriticalSection& getCriticalSection()
	{
		return cs;
	}

public:
	bool shuttingDown() const
	{
		return _shutdown;
	}
	void getCounters(UINT32 & autoReset_, UINT32 & manualReset_, UINT32 & setEvent_, UINT32 & contentions)
	{
		autoReset_ = 0;
		manualReset_ = 0;
		setEvent_ = 0;
		contentions = 0;
	}
	void increaseAutoResetCounter() const
	{
	}
	size_t size() const
	{
		PLock lock( const_cast<PCriticalSection&>( cs ) );//cs is mutable
		return _queue.size();
	}
	size_t sizeAndReset() 
	{
		PLock lock( const_cast<PCriticalSection&>( cs ) );//cs is mutable
		return _queue.size();
	}

	SOCKET getSocket() const//for interaction with PMultiSocketRW
	{
		return (SOCKET)pipeRd;
	}

	void push( const value_type& item );
	void push( const value_type& item, UINT32 * tickCounterMsg_ );
	int waitPop( value_type& item, int timeout );
	int waitPop( value_type& item )//infinite
	{
		return waitPop( item, -1 );
	}
	bool pop( value_type& item );

	void shutdown();

private:
	inline void _pop( value_type& item );
	//MUST be locked before call

private://prohibited
	PQueue( const PQueue< Container >& );
	PQueue< Container >& operator =( const PQueue< Container >& );
};

template< class Container >
PQueue< Container >::PQueue()
{
	int fd[ 2 ];
	int err = pipe( fd );
	if( err )
		throw PSystemError( "Unable to create pipe", errno );
	pipeWr = fd[ 1 ];
	pipeRd = fd[ 0 ];
	int flags = ::fcntl( pipeRd, F_GETFL, 0 );
	::fcntl( pipeRd, F_SETFL, flags | O_NONBLOCK );
	flags = ::fcntl( pipeWr, F_GETFL, 0 );
	::fcntl( pipeWr, F_SETFL, flags | O_NONBLOCK );

	flags = ::fcntl( pipeRd, F_GETFD, 0 );
	::fcntl( pipeRd, F_SETFD, flags | FD_CLOEXEC );
	flags = ::fcntl( pipeWr, F_GETFD, 0 );
	::fcntl( pipeWr, F_SETFD, flags | FD_CLOEXEC );

	_shutdown = false;
}

template< class Container >
PQueue< Container >::~PQueue()
{
	::close( pipeWr );
	::close( pipeRd );
}

template< class Container >
void PQueue< Container >::push( const TYPENAME PQueue::value_type& item )
{
	PLock lock( cs );
	if( _shutdown )
		return;
	bool _needWrite = _queue.size() == 0;

	_queue.push( item );

	if (_needWrite)
	{
		BYTE b = 0;
		auto rd = ::write( pipeWr, &b, 1 );
		if( rd < 0 )
		{
			throw PSystemError( "Error writing to pipe", errno );
		}

	}

}

template< class Container >
void PQueue< Container >::push( const TYPENAME PQueue::value_type& item, UINT32 * tickCounterMsg_  )
{
	PLock lock( cs );
	tickCounterMsg_[0] = SYSTEMTICKS();
	if( _shutdown )
		return;
	bool _needWrite = _queue.size() == 0;
	_queue.push( item );
	tickCounterMsg_[1] = SYSTEMTICKS();

	if (_needWrite)
	{
		BYTE b = 0;
		auto rd = ::write( pipeWr, &b, 1 );
		if( rd < 0 )
		{
			throw PSystemError( "Error writing to pipe", errno );
		}
	}

	lock.unlock();
	tickCounterMsg_[2] = SYSTEMTICKS();
}

template< class Container >
inline void PQueue< Container >::_pop( TYPENAME PQueue::value_type& item )
{
	PASSERT5( _queue.size() != 0 );
	item = _queue.peek();
	_queue.pop();

	if( _queue.size() == 0 )
	{
#if defined (P_MAC10X) 
retry:
#endif
		BYTE b;
		int rd = ::read( pipeRd, &b, 1 );
		if( rd < 0 )
		{
#if defined (P_MAC10X) 
			if( errno == EAGAIN ) // Temp unavail
			{
				PThread::sleep( 1 );
				goto retry;
			}
#endif
			throw PSystemError( "Error reading from pipe", errno );
		}
	}
}

template< class Container >
int PQueue< Container >::waitPop( TYPENAME PQueue::value_type& item, int timeout )
{
	for(;;)
	{
		PLock lock( cs );
		if( _shutdown )
			return waitPopShutdown;

		if( _queue.size() == 0 )
		{
			lock.unlock();

			struct pollfd poll;
			poll.fd = pipeRd;
			poll.events = POLLIN;

retr_intr:			
			int res = ::poll( &poll, 1, timeout );
			if( res < 0 )
			{
				if (errno == EINTR)
					goto retr_intr;
				throw PSystemError( "Error waiting for pipe data", errno );
			}
			else if( res == 0 )
				return waitPopTimeout;
		}
		else
		{
			_pop( item );
			return waitPopOk;
		}
	}
}

template< class Container >
bool PQueue< Container >::pop( TYPENAME PQueue::value_type& item )
{
	PLock lock( cs );
	if( _queue.size() == 0 )
		return false;
	_pop( item );
	return true;
}

template< class Container >
void PQueue< Container >::shutdown()
{
	PLock lock( cs );
	_shutdown = true;

	lock.unlock();

	BYTE b = 0xFF;
	auto rd = ::write( pipeWr, &b, 1 );
	( void )rd;
}
#endif //LINUX
#endif //POSIX


template< class T >
class PQDeque : private deque< T >
	//Deque-based container for PQueue
{
private:
	typedef deque< T > _Parent;

public:
	typedef TYPENAME _Parent::value_type value_type;


public:
	size_t size() const
	{
		return _Parent::size();
	}
	void push( const T& t )
	{
		_Parent::push_back( t );
	}
	const T& peek() const
	{
		PASSERT4( size() > 0 );
		return *_Parent::begin();
	}
	void pop()
	{
		_Parent::pop_front();
	}
};




#if defined(P_USE_READWRITE_LOCK_VERSION_2)

#if defined (WIN32)

inline UINT32 PInterlockedExchangeAdd(UINT32 volatile *Addend, UINT32 Value)
{
	return (UINT32) InterlockedExchangeAdd((long int volatile *) Addend, Value);
}
inline UINT32 PInterlockedIncrement(UINT32 volatile *Addend)
{
	return InterlockedIncrement((long int volatile *)Addend);
}

class PWLock;
class PRLock;

class PReadWriteLock 
{
private:
	volatile UINT32 _counterRead;
	volatile UINT32 _counter;
	volatile UINT32 contentions;

	UINT32 recursion;
	_PWin32Event     _evnt;
	_PWin32Semaphore _sem;
	PCriticalSection _cs;

	enum {waitMask = 0x80000000};

public:
	PReadWriteLock(): _evnt(FALSE,FALSE),_sem(0,0x7fffffff)
	{
		_counterRead = 0;
		_counter = 0;
		recursion = 0;
		contentions = 0;
	}
	UINT32 getContentions()
	{
		UINT32 tmp = contentions + _cs.getContentions();
		contentions = 0;
		return tmp;
	}
	friend class PWLock;
	friend class PRLock;
private://prohibited
	PReadWriteLock( const PReadWriteLock& );
	PReadWriteLock& operator =( const PReadWriteLock& );

};

class PRLock
{
private:
	PReadWriteLock& cs;
	bool locked;
	void _unlock();

public:
	PRLock( PReadWriteLock& cs_ );
	~PRLock()
	{
		if (locked)
			_unlock();
	}

	void unlock()
	{
		PASSERT3( locked );
		locked = false;
		_unlock();
	}
private://prohibited
	PRLock( const PRLock& );
	PRLock& operator =( const PRLock& );
};

class PWLock
{
private:
	PReadWriteLock& cs;
	bool locked;
	void _unlock();

public:
	PWLock( PReadWriteLock& cs_);
	~PWLock()
	{
		if (locked)
			_unlock();
	}

	void unlock()
	{
		PASSERT3( locked );
		locked = false;
		_unlock();
	}
private://prohibited
	PWLock( const PWLock& );
	PWLock& operator =( const PWLock& );
};



#elif defined(LINUX)
class PWLock;
class PRLock;

class PReadWriteLock
{
private:
	pthread_rwlock_t rwlock;
	pthread_t wrOwner;
	int recursive;

public:
	PReadWriteLock();
	UINT32 getContentions()
	{
		return 0;
	}
	~PReadWriteLock()
	{
		pthread_rwlock_destroy(&rwlock);
	}
	friend class PWLock;
	friend class PRLock;
private://prohibited
	PReadWriteLock( const PReadWriteLock& );
	PReadWriteLock& operator =( const PReadWriteLock& );

};

class PRLock
{
private:
	PReadWriteLock& cs;
	bool locked;
	void _unlock();

public:
	PRLock( PReadWriteLock& cs_ );
	~PRLock();
	void unlock();
private://prohibited
	PRLock( const PRLock& );
	PRLock& operator =( const PRLock& );
};

class PWLock
{
private:
	PReadWriteLock& cs;
	bool locked;
	void _unlock();

public:
	PWLock( PReadWriteLock& cs_);
	~PWLock();
	void unlock();
private://prohibited
	PWLock( const PWLock& );
	PWLock& operator =( const PWLock& );
};

UINT32 PInterlockedExchangeAdd(UINT32 volatile *Addend, UINT32 Value);
UINT32 PInterlockedIncrement(UINT32 volatile *Addend);

#else //LINUX-WIN32
typedef PLock PRLock;
typedef PLock PWLock;
typedef PCriticalSection PReadWriteLock;
UINT32 PInterlockedExchangeAdd(UINT32 volatile *Addend, UINT32 Value);
UINT32 PInterlockedIncrement(UINT32 volatile *Addend);
#endif //LINUX-WIN32

#endif //P_USE_READWRITE_LOCK_VERSION_2


#endif //PNOTHREADS
#endif //ppthread_h_included
