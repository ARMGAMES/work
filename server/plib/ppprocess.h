#ifndef ppprocess_h_included
#define ppprocess_h_included

#include "ppinclude.h"
#include "pperror.h"
#include "ppthread.h"
#include "ppipc.h"

/* abstract */ struct ProcessExitedCallback
{
	virtual ~ProcessExitedCallback() {}
	virtual void onProcessExited() = 0;
};


#ifdef PWIN32

#define PYRKILLEVENTHANDLE "PYRKILLEVENTHANDLE"
class PProcess
{
private:
	//DWORD groupId;
	HANDLE handle;
	HANDLE onTerminateCallbackHandle;
	PNamedEvent event;
	UINT32 pId;

private:
	static PCriticalSection cs;//implementation: to protect environment variables from changing between putenv - CreateProcess calls
	static unsigned short globalCounter;

public:
	PProcess();
	~PProcess();

public:
	void start( const char* exeName, int nParms = 0, const char* const* parms = 0 );
	void startElevated( const WCHAR* exeName, int nParms = 0, const char* const* parms = 0, bool forceElevated = false );
	bool getExitCode(long& exitCode) const;
	bool setOnExitedCallback(ProcessExitedCallback* callback);
	void clearOnExitedCallback();

	//	bool isRunning() const;
	void kill();
	void kill9();
	void join();
	static void setenv(const char * name, const char * value);

	HANDLE getHandle() const
	{
		return handle;
	}
	UINT32 getPID() const
	{
		return pId;
	}
	void closeHandles() 
	{
		if( handle != INVALID_HANDLE_VALUE )
		{
			::CloseHandle( handle );
			handle = INVALID_HANDLE_VALUE;
		}
		event.close();
	}

private://prohibited
	PProcess( const PProcess& other );
	PProcess& operator =( const PProcess& other );
};


//****************************************************************************

class _PProcessKillThread;
class PProcessKillHandler//singleton!
{
public:
	static PProcessKillHandler* instance;
	PCriticalSection cs;
	bool shutdown;
	_PProcessKillThread* eventThread;
	PNamedEvent event;

public:
	PProcessKillHandler(const char * pyrHandler = 0, UINT32 pPid = 0);
	virtual ~PProcessKillHandler();

public:
	bool _safeSignal();
	virtual bool signal() { return false; }//returns true if processed signal
};

#elif defined ( PPOSIX )


class PProcess
{
private:
	//DWORD groupId;
	pid_t processPid;

public:
	PProcess();
	~PProcess();

public:
	void start( const char* exeName, int nParms = 0, const char* const* parms = 0 );
	void startElevated( const char* exeName, int nParms = 0, const char* const* parms = 0 );
	bool getExitCode(long& exitCode) const;
	bool setOnExitedCallback(ProcessExitedCallback* callback) {return false;}
	void clearOnExitedCallback() {}

	UINT32 getPID() const
	{
		return processPid;
	}

	//	bool isRunning() const;
	void kill();
	void kill9();
	void join();
	static void setenv(const char * name, const char * value);

private://prohibited
	PProcess( const PProcess& other );
	PProcess& operator =( const PProcess& other );
};


//****************************************************************************

class PProcessKillHandler//singleton!
{
public:
	static PProcessKillHandler* instance;
	bool shutdown;

public:
	PProcessKillHandler(const char * pyrHandler = 0, UINT32 pPid = 0);
	~PProcessKillHandler();

public:
	bool _safeSignal();
	virtual bool signal() { return false; }//returns true if processed signal
};


#endif

#endif
