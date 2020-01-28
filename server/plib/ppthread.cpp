#include "ppinclude.h"
#include "ppthread.h"
#include "pperror.h"
#include "pplogfile.h"

#ifndef PNOTHREADS

#ifdef PWIN32
/* static */ int PCriticalSection::tecsInited = 0;
/* static */ _TryEnterCriticalSection PCriticalSection::tecs = 0;

/* static */ bool PCriticalSection::tryLockSupported()
{
	if( tecsInited == 0 )
		_initTecs();
	return tecs != 0;
}

/* static */ void PCriticalSection::_initTecs()
{
	HMODULE hl = ::LoadLibraryA( "Kernel32.dll" );
	if( hl )
	{
		_TryEnterCriticalSection tecs_ = (_TryEnterCriticalSection)(void*)GetProcAddress( hl, "TryEnterCriticalSection" );
		tecs = tecs_;
	}
	tecsInited = 1;
}

PThread::PThread()
{
	handle = NULL;
	_thrId=0;
}

/* virtual */ PThread::~PThread()
{
	if( handle != NULL )
		CloseHandle( handle );
}

void PThread::start()
{
	PASSERT4( handle == NULL );
	handle = (HANDLE)_beginthreadex( NULL, 0, _threadProc, this, 0, &_thrId );
	if( handle == NULL )
		throw PSystemError( "Unable to create thread", GetLastError() );
}

/* static */ unsigned __stdcall PThread::_threadProc( void* parm )
{
	void(*oldTranslator)(unsigned int, _EXCEPTION_POINTERS*);
	oldTranslator = _set_se_translator( PError::_translate );
#if !defined(COMM_CLIENT_ONLY)
	set_terminate(PError::_terminate);
#endif
	try
	{
		((PThread*)parm)->run();
	}
	catch( ... )
	{
		_set_se_translator( oldTranslator );
		return 13;
	}
	_set_se_translator( oldTranslator );
	return 0;
}

void PThread::join()
{
	PASSERT4( handle != NULL );
	WaitForSingleObject( handle, INFINITE );
}

bool PThread::terminated()
{
	if( handle == NULL )//never started
		return true;
	DWORD res = WaitForSingleObject( handle, 0 );
	return res != WAIT_TIMEOUT;
}

/* static */ void PThread::sleep( UINT32 msec )
{
	::Sleep( msec );
}

/* static */ void PThread::setCurrentThreadPriority( const char* priority )
{
	if( strcmp( priority, PTHREAD_WIN32_PRIORITY_DEFAULT ) == 0 )
		return;

	int p = THREAD_PRIORITY_NORMAL;
	if( strcmp( priority, PTHREAD_WIN32_PRIORITY_MINUS2 ) == 0 )
		p = THREAD_PRIORITY_LOWEST;
	else if( strcmp( priority, PTHREAD_WIN32_PRIORITY_MINUS1 ) == 0 )
		p = THREAD_PRIORITY_BELOW_NORMAL;
	else if( strcmp( priority, PTHREAD_WIN32_PRIORITY_PLUS1 ) == 0 )
		p = THREAD_PRIORITY_ABOVE_NORMAL;
	else if( strcmp( priority, PTHREAD_WIN32_PRIORITY_PLUS2 ) == 0 )
		p = THREAD_PRIORITY_HIGHEST;
	else if( strcmp( priority, PTHREAD_WIN32_PRIORITY_15 ) == 0 )
		p = THREAD_PRIORITY_TIME_CRITICAL;

	::SetThreadPriority( GetCurrentThread(), p );
}
void PThread::setThreadPriority( const char* priority )
{
	if( strcmp( priority, PTHREAD_WIN32_PRIORITY_DEFAULT ) == 0 )
		return;
	if (handle == NULL)
		return;
	int p;
	if( strcmp( priority, PTHREAD_WIN32_PRIORITY_NORMAL ) == 0 )
		p = THREAD_PRIORITY_NORMAL;
	else if( strcmp( priority, PTHREAD_WIN32_PRIORITY_MINUS2 ) == 0 )
		p = THREAD_PRIORITY_LOWEST;
	else if( strcmp( priority, PTHREAD_WIN32_PRIORITY_MINUS1 ) == 0 )
		p = THREAD_PRIORITY_BELOW_NORMAL;
	else if( strcmp( priority, PTHREAD_WIN32_PRIORITY_PLUS1 ) == 0 )
		p = THREAD_PRIORITY_ABOVE_NORMAL;
	else if( strcmp( priority, PTHREAD_WIN32_PRIORITY_PLUS2 ) == 0 )
		p = THREAD_PRIORITY_HIGHEST;
	else if( strcmp( priority, PTHREAD_WIN32_PRIORITY_15 ) == 0 )
		p = THREAD_PRIORITY_TIME_CRITICAL;
	else return;
	::SetThreadPriority( handle, p );
}

_PWin32Event::_PWin32Event( BOOL manualReset, BOOL set /*, const char* name */ )
{
	handle = ::CreateEvent( 0, manualReset, set, NULL /*name*/ );
	if( handle == NULL )
		throw PSystemError( "Unable to create event", GetLastError() );
}

_PWin32Event::~_PWin32Event()
{
	if( handle != NULL )
		::CloseHandle( handle );
}

void _PWin32Event::setEvent()
{
	BOOL Ok = ::SetEvent( handle );
	if( !Ok )
		throw PSystemError( "Unable to set event", GetLastError() );
}

void _PWin32Event::pulseEvent()
{
	BOOL Ok = ::PulseEvent( handle );
	if( !Ok )
		throw PSystemError( "Unable to pulse event", GetLastError() );
}

void _PWin32Event::resetEvent()
{
	BOOL Ok = ::ResetEvent( handle );
	if( !Ok )
		throw PSystemError( "Unable to reset event", GetLastError() );
}

_PWin32Semaphore::_PWin32Semaphore( INT32 initialCount,INT32 maximumCount)
{
	handle = ::CreateSemaphore( 0, initialCount, maximumCount, NULL /*name*/ );
	if( handle == NULL )
		throw PSystemError( "Unable to create semaphore", GetLastError() );
}

_PWin32Semaphore::~_PWin32Semaphore()
{
	if( handle != NULL )
		::CloseHandle( handle );
}

void _PWin32Semaphore::releaseSemaphore(INT32 count)
{
	BOOL Ok = ::ReleaseSemaphore( handle, count, 0);
	if( !Ok )
		throw PSystemError( "Unable to release semaphore", GetLastError() );
}

#elif defined ( PPOSIX )

/* static */ bool PCriticalSection::tryLockSupported()
{
	return false;
}


PThread::PThread()
	: thread( 0 ), joinDetachHandled( false )
{
	_thrId=0;
}

bool PThread::getTimers( UINT64 & kernelT_, UINT64 & userT_ )
{
	kernelT_ = userT_ = 0;
	return false;
}

void PThread::start()
{
	PASSERT4( !joinDetachHandled );
	pthread_mutex_init( &joinDetachMutex, NULL );

	int err;
#if defined(P_AIX32) || defined(P_AIX64)
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	struct rlimit limit;
	getrlimit( RLIMIT_STACK, &limit );
	size_t stacksize = limit.rlim_cur;
	if( stacksize > PTHREAD_STACK_MIN )
	{
		if( stacksize > 8192000 )
			stacksize = 8192000;
		if( (err = pthread_attr_setstacksize (&attr, stacksize)) )
			throw PSystemError( "Unable to create stack", err );
	}
	err = pthread_create( &thread, &attr, _threadProc, this );
	pthread_attr_destroy ( &attr );
#else
	err = pthread_create( &thread, NULL, _threadProc, this );
#endif
	if( err )
		throw PSystemError( "Unable to create thread", err );

}

/* virtual */ PThread::~PThread()
{
	if( thread )
	{
		PTRACE5( "Thread %u started destructor of %u (joinDetachHandled = %d)", pthread_self(), thread, (int) joinDetachHandled );
		pthread_mutex_lock( &joinDetachMutex );
		if ( !joinDetachHandled )
		{
			joinDetachHandled = true;
			PTRACE5( "Thread %u entered pthread_detach of %u", pthread_self(), thread );
#if !defined( P_MAC10X )
			pthread_detach( thread );
#endif
			PTRACE5( "Thread %u exited pthread_detach of %u", pthread_self(), thread );
		}
		pthread_mutex_unlock( &joinDetachMutex );
		PTRACE5( "Thread %u ended destructor of %u", pthread_self(), thread );
		pthread_mutex_destroy( &joinDetachMutex );
	}
}
#if defined(LINUX)
void  PThread::setTimers( )
{
	PLock lock(performanceLock);
	struct rusage r;
	if (!getrusage(RUSAGE_THREAD,&r))
	{
		kernelT = (UINT64)r.ru_stime.tv_sec * 10000000 + (r.ru_stime.tv_usec * 10);
		userT =   (UINT64)r.ru_utime.tv_sec * 10000000 + (r.ru_utime.tv_usec * 10);
	}
}

bool PThread::getTimers(UINT64 & kernelT_, UINT64 & userT_ )
{
	PLock lock(performanceLock);
	kernelT_ = kernelT;
	userT_ = userT;
	return true;
}
#else //LINUX
void  PThread::setTimers( )
{
}
bool getTimers(UINT64 & kernelT, UINT64 & userT ) // not supported yet
{
	kernelT = 0;
	userT = 0;
	return true;
}
#endif

/* static */ void* PThread::_threadProc( void* parm )
{
	try
	{
		((PThread*)parm)->run();
	}
	catch( const PError& err )
	{
		printf( "caught '%s' in thread id %u\n", err.why(), THREADID() ); // Send to console
		try
		{
			if( PLogIsInit() )
			{
				PLog( "caught '%s' in thread id %u\n", err.why(), THREADID() );
			}
		}
		catch( ... ) {} // Ignore
	}
	catch(  exception& exc  )
	{
		printf( "caught exception '%s' in thread id %u\n", exc.what(), THREADID() ); // Send to console
		try
		{
			if( PLogIsInit() )
			{
				PLog( "caught exception '%s' in thread id %u", exc.what(), THREADID() );
			}
		}
		catch( ... ) {} // Ignore
	}

	catch( ... )
	{
		printf( "caught ... in thread id %u\n", THREADID() ); // Send to console
		try
		{
			if( PLogIsInit() )
			{
				PLog( "caught ... in thread id %u\n", THREADID() );
			}
		}
		catch( ... ) {} // Ignore
	}
	return NULL;
}

void PThread::join()
{
	PTRACE5("Thread %u started to join %u (joinDetachHandled = %d)", pthread_self(), thread, (int) joinDetachHandled);
	pthread_mutex_lock( &joinDetachMutex );
	if( !joinDetachHandled )
	{
		joinDetachHandled = true;
		PTRACE5( "Thread %u entered pthread_join to %u", pthread_self(), thread );
		pthread_join( thread, NULL );
		PTRACE5( "Thread %u exited pthread_join to %u", pthread_self(), thread );
	}
	pthread_mutex_unlock( &joinDetachMutex );
	PTRACE5( "Thread %u ended to join %u", pthread_self(), thread );
}

bool PThread::terminated()
{
	return !thread || pthread_kill( thread, 0 ) != 0;
}

/* static */ void PThread::sleep( UINT32 msec )
{
#if 1
	usleep( msec * 1000 );
#else
	pthread_mutex_t sleepMutex;
	pthread_cond_t sleepCond;

	struct timespec timeToWait;
	struct timeval now;
	int rt;

	gettimeofday( &now, NULL );

	timeToWait.tv_sec = now.tv_sec + msec / 1000;
	timeToWait.tv_nsec = (now.tv_usec + (msec % 1000) * 1000) * 1000;

	pthread_mutex_init( &sleepMutex, NULL );
	pthread_cond_init( &sleepCond, NULL );

	pthread_mutex_lock( &sleepMutex );
	rt = pthread_cond_timedwait( &sleepCond, &sleepMutex, &timeToWait );
	pthread_mutex_unlock( &sleepMutex );

	pthread_cond_destroy( &sleepCond );
	pthread_mutex_destroy( &sleepMutex );
#endif
}

#endif


#if defined(P_USE_READWRITE_LOCK_VERSION_2)


#if defined(PWIN32)

void PRLock::_unlock()
{
	if (InterlockedDecrement((volatile LONG *)&cs._counter) & PReadWriteLock::waitMask)
	{
		if (!InterlockedDecrement((volatile LONG *)&cs._counterRead))
		{
			cs._evnt.setEvent();
		}
	}
}

void PWLock::_unlock()
{
	if (!--cs.recursion)
	{
		INT32 ret = InterlockedExchangeAdd((volatile LONG *)&cs._counter, -PReadWriteLock::waitMask) - PReadWriteLock::waitMask;
		if (ret)
		{
			cs._sem.releaseSemaphore(ret);
		}
	}
	cs._cs._unlock();
}



PRLock::PRLock( PReadWriteLock& cs_)
: cs( cs_ )
{
	if (InterlockedIncrement((volatile LONG *) &cs._counter ) & PReadWriteLock::waitMask)
	{
		InterlockedIncrement((volatile LONG *)& cs.contentions);
		::WaitForSingleObject(cs._sem.getHandle(),INFINITE);
	}
	locked = true;
	return;
}

PWLock::PWLock( PReadWriteLock& cs_)
: cs( cs_ )
{
	cs._cs._lock();
	if (!cs.recursion++)
	{
		PASSERT3(!(cs._counter & PReadWriteLock::waitMask));
		UINT32 ret = (UINT32) InterlockedExchangeAdd((volatile LONG *)&cs._counter, PReadWriteLock::waitMask);
		if (ret)
		{
			if (InterlockedExchangeAdd((volatile LONG *)&cs._counterRead, ret) + ret)
			{
				InterlockedIncrement((volatile LONG *)& cs.contentions);
				::WaitForSingleObject(cs._evnt.getHandle(),INFINITE);
			}
		}
		PASSERT3(cs._counter & PReadWriteLock::waitMask);
	}
	locked = true;
}
#elif defined(LINUX)

PReadWriteLock::PReadWriteLock()
{
//	PTRACE5("Constructor %p ", this);
	recursive=0;
	wrOwner=0;
	pthread_rwlock_init(&rwlock,0);
}

void PRLock::_unlock()
{
//	PTRACE5(" read _unlocking %p",&cs.rwlock);
	int ret = pthread_rwlock_unlock(&cs.rwlock);
//	PTRACE5(" read _unlocking %p done, ret=%d",&cs.rwlock,ret);
}

PRLock::~PRLock()
{
//	PTRACE5(" destructor read %p, locked=%s",&cs.rwlock,(locked ? "true":"false"));
	if (locked)
		_unlock();
}

void PRLock::unlock()
{
//	PTRACE5(" unlock read %p, locked=%s",&cs.rwlock,(locked ? "true":"false"));
	PASSERT3( locked );
	locked = false;
	_unlock();
}

void PWLock::_unlock()
{
//	PTRACE5(" write _unlocking %p, recursive=%d",&cs.rwlock,cs.recursive);
	if (--cs.recursive > 0)
	{
//		PTRACE3(" write _unlocking %p - still have it",&cs.rwlock);
		return;
	}
	cs.wrOwner = 0;
	int ret = pthread_rwlock_unlock(&cs.rwlock);
//	PTRACE5(" write _unlocking %p done, ret =%d",&cs.rwlock,ret);
}


PWLock::~PWLock()
{
//	PTRACE5(" destructor write %p, locked=%s",&cs.rwlock,(locked ? "true":"false"));
	if (locked)
		_unlock();
}

void PWLock::unlock()
{
//	PTRACE5(" unlock write %p, locked=%s",&cs.rwlock,(locked ? "true":"false"));
	PASSERT3( locked );
	locked = false;
	_unlock();
}

PRLock::PRLock( PReadWriteLock& cs_):cs(cs_)
{
//	PTRACE5(" locking read %p",&cs.rwlock);
	int ret = pthread_rwlock_rdlock(&cs.rwlock);
	locked = true;
//	PTRACE5(" locking read %p done, ret = %d",&cs.rwlock,ret);
}

PWLock::PWLock( PReadWriteLock& cs_):cs(cs_)
{
	if(cs.wrOwner == pthread_self())
	{
//		PTRACE5(" locking write %p - we already have it",&cs.rwlock);
		locked = true;
		cs.recursive++;
		return;
	}
//	PTRACE5(" locking write %p",&cs.rwlock);
	int ret = pthread_rwlock_wrlock(&cs.rwlock);
	locked = true;
	cs.wrOwner = pthread_self();
	cs.recursive=1;
//	PTRACE5(" locking write %p done, ret = %d",&cs.rwlock,ret);

}


UINT32 PInterlockedExchangeAdd(UINT32 volatile *Addend, UINT32 Value)
{
	return __sync_fetch_and_add(Addend,Value);
}
UINT32 PInterlockedIncrement(UINT32 volatile *Addend)
{
	return __sync_add_and_fetch(Addend,1);
}

#else

UINT32 PInterlockedExchangeAdd(UINT32 volatile *Addend, UINT32 Value)
{
	UINT32 oldValue = *Addend;
	*Addend+=Value;
	return oldValue;
}
UINT32 PInterlockedIncrement(UINT32 volatile *Addend)
{
	return ++(*Addend);
}
#endif //linux
#endif //P_USE_READWRITE_LOCK_VERSION_2

#if defined(LINUX)


	void PUnixEvent::setEvent()
	{
//		PTRACE3("In set %p", this);
		pthread_mutex_lock(&mutex);
		raised=true;
		pthread_mutex_unlock(&mutex);
		pthread_cond_signal(&cond);
//		PTRACE3("Out set %p", this);
	}
	void PUnixEvent::wait()
	{
//		PTRACE3("In wait %p", this);
		pthread_mutex_lock(&mutex);
		while(!raised)
		{
			pthread_cond_wait(&cond,&mutex);
		}
		raised=false;
		pthread_mutex_unlock(&mutex);
//		PTRACE3("Out wait %p",this);
	}
	int PUnixEvent::wait(int timeout)
	{
//		PTRACE3("In timewait %p",this);
		pthread_mutex_lock(&mutex);
		struct timespec tm;
		timespec_get(&tm,TIME_UTC);
//		clock_gettime(CLOCK_REALTIME,&tm);

//		gettimeofday(&tvl,NULL);
		tm.tv_sec+=timeout/1000;
		tm.tv_nsec+=(timeout%1000)*1000000;
		int ret = 0;
		while(!raised && ret == 0)
		{
			ret = pthread_cond_timedwait(&cond,&mutex,&tm);
		}
		if (raised)
		{
			raised=false;
			ret=0;
		}
		pthread_mutex_unlock(&mutex);
//		PTRACE3("Out timewait %p  : ret=%d ", this,ret);
		if (ret)
			return 1;
		else
			return 0;
	}


#endif


#endif
