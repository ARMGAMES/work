#ifndef commtthread_h_included
#define commtthread_h_included

#include "ppinclude.h"
#include "ppstring.h"
#include "pplogfile.h"
#include "commmsgbody.h"
#include "commqueue.h"

template<typename T>
struct DummyContainer
{
	using value_type = T;

	void push(const T&) { ++_size; }
	void pop() { --_size; }
	const T& peek() const { static T val; return val; }
	size_t size() const { return _size; }

private:
	size_t _size = 0;
};

/* abstract */ class _CommTimerThread : public PThread
{
private:
	// NOTE: Order of members is important!
	PQueue< DummyContainer<int> > dummyQueue;
	PCriticalSection& cs;
	_CommTimer commTimer;

public:
	_CommTimerThread() : cs(dummyQueue.getCriticalSection()), commTimer(cs) {}

public:
	UINT32 push( /*non-const!*/ _CommMsg& msg, int msec );//NOTE: implementation uses msg.internal.patch1 - it will not be preserved

	void shutdown();

public://overridable
	virtual void timer( _CommMsg& msg ) = 0;

public://overridden
	void run() override;
};

#endif
