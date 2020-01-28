#ifndef ppipc_h_included
#define ppipc_h_included


#ifdef PWIN32
class PNamedEvent
{
private:
	HANDLE handle;
//	bool   eventForWait;

public:
	enum { waitOk = 0, waitTimeout = -2, waitFailed = -1 };

	PNamedEvent();
	~PNamedEvent();
	void close();
	void create(bool eventForWait_, const char * name, bool manualReset = false);
	void open(bool eventForWait_, const char * name);
	SOCKET getSocket() const;
	HANDLE getHandle() const;
	int wait(int timeOut);
	void set();
	void reset()
	{
		// if auto reset event do nothing 
		// else
		// ::ResetEvent( handle );
	}

private://prohibited
	PNamedEvent( const PNamedEvent& );
	PNamedEvent& operator =( const PNamedEvent& );
};


class PNamedMutex
{
private:
	HANDLE handle;

public:
	PNamedMutex();
	~PNamedMutex();
	void close();
	void create(const char * name);
	void open(const char * name);
	void set();
	void release();
private://prohibited
	PNamedMutex( const PNamedMutex& );
	PNamedMutex& operator =( const PNamedMutex& );
};


class PNamedSharedMemory
{
private:
	HANDLE handle;
	void * sharedAddress;

public:
	PNamedSharedMemory();
	~PNamedSharedMemory();
	void close();
	void * create(int size, const char * name);
	void * open(const char * name);
private://prohibited
	PNamedSharedMemory( const PNamedSharedMemory& );
	PNamedSharedMemory& operator =( const PNamedSharedMemory& );
};

class PWaitForObjects
{
public:
	enum { infinite = -1 };
	enum { waitTimeout = -2, waitFailed = -1 };

	PWaitForObjects(int) // (int maxNumSlots_) on some platforms
	{
	}
	int waitMultiple(int NumSlot, SOCKET * sockets, int timeOut);
	static int waitSingle(SOCKET socket, int timeOut);
};



#else

#include <sys/ipc.h>
#ifndef P_ANDROID
#include <sys/shm.h>
#endif 
#include <semaphore.h>
#include <ppstring.h>

class PNamedEvent
{
private:
	int  handle;
	int  handlewrite;
	bool eventForWait;
	bool created;
	bool shutdown;
	PString _name;

public:
	enum { waitOk = 0, waitTimeout = -2, waitFailed = -1 };

	PNamedEvent();
	~PNamedEvent();
	void close();
	void create(bool eventForWait_, const char * name, bool manualReset = false);
	void open(bool eventForWait_, const char * name);
	void set();
	SOCKET getSocket() const;
	int getHandle() const;
	int wait(int timeOut);
	void reset();
private://prohibited
	PNamedEvent( const PNamedEvent& );
	PNamedEvent& operator =( const PNamedEvent& );
};



class PNamedMutex
{
private:
	sem_t * handle;
	bool created;
	PString _name;

public:
	PNamedMutex();
	void close();
	~PNamedMutex();
	void create(const char * name);
	void open(const char * name);
	void set();
	void release();
private://prohibited
	PNamedMutex( const PNamedMutex& );
	PNamedMutex& operator =( const PNamedMutex& );
};

#ifndef P_ANDROID
class PNamedSharedMemory
{
private:
	int handle;
	key_t key;
	void * sharedAddress;
	PString _name;
	bool created;


public:
	PNamedSharedMemory();
	void close();
	~PNamedSharedMemory();
	void * create(int size, const char * name);
	void * open(const char * name);
private://prohibited
	PNamedSharedMemory( const PNamedSharedMemory& );
	PNamedSharedMemory& operator =( const PNamedSharedMemory& );
};
#endif

class PWaitForObjects
{
	struct pollfd * polls;
	int maxNumSlots;
public:
	enum { infinite = -1};
	enum { waitTimeout = -2, waitFailed = -1 };

	PWaitForObjects(int maxNumSlots_);
	~PWaitForObjects();
	int waitMultiple(int NumSlot, SOCKET * sockets, int timeOut);
	static int waitSingle(SOCKET socket, int timeOut);
};


#endif

#endif
