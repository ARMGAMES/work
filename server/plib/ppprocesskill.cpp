#include "ppinclude.h"
#include "ppstring.h"
#include "pplogfile.h"
#include "ppprocess.h"


#ifdef PWIN32

/* static */ PProcessKillHandler* PProcessKillHandler::instance = 0;

static BOOL WINAPI _HandlerRoutine( DWORD ctrlType )
{
#ifndef _DEBUG
	return TRUE;//yes - we ignore all Win32 system events (including infamous CTRL_LOGOFF_EVENT)
#else
	return PProcessKillHandler::instance->_safeSignal();
#endif
}

class _PProcessKillThread : public PThread
{
public:
	PProcessKillHandler* handler;
	HANDLE event;
	UINT32 pPid;

public:
	_PProcessKillThread( PProcessKillHandler* handler_, HANDLE event_, UINT32 pPid_ );
	void run() override;
};

_PProcessKillThread::_PProcessKillThread( PProcessKillHandler* handler_, HANDLE event_, UINT32 pPid_ )
{
	handler = handler_;
	event = event_;
	pPid = pPid_;
}

void _PProcessKillThread::run()
{
	if (pPid)
	{
		HANDLE pH = OpenProcess(SYNCHRONIZE,FALSE,pPid);
		if (pH)
		{
			HANDLE hnds[2];
			hnds[0]=pH;
			hnds[1]=event;
			int n = ::WaitForMultipleObjects(2,hnds,FALSE,INFINITE);
            UNUSED(n);
			PTRACE5("_PProcessKillThread (pid %d) wait %d, %d", pPid, n, ::GetLastError());
		}
		else
		{
			int n = ::WaitForSingleObject( event, INFINITE );	 
            UNUSED(n);
			PTRACE5("_PProcessKillThread (pid %d, no pH) wait %d, %d", pPid, n, ::GetLastError());
		}
	}
	else
	{
		int n = ::WaitForSingleObject( event, INFINITE );
        UNUSED(n);
		PTRACE5("_PProcessKillThread wait %d, %d", n, ::GetLastError());
	}
	handler->_safeSignal();
}

PProcessKillHandler::PProcessKillHandler(const char * pyrHandler, UINT32 pPid)
{
	PASSERT4( instance == 0 );
	instance = this;
	eventThread = 0;
	shutdown = false;


	const char* env;
	if (pyrHandler)
		env = pyrHandler;
	else
		env = getenv( PYRKILLEVENTHANDLE );
	if( env )
	{
		/*		char* endPtr;
		event = (HANDLE)strtoul( env, &endPtr, 16 );
		*/
		event.open( false, env );
		eventThread = new _PProcessKillThread( this, event.getHandle(), pPid );
		eventThread->start();
	}

	SetConsoleCtrlHandler( _HandlerRoutine, TRUE );
}

PProcessKillHandler::~PProcessKillHandler()
{
	if (!instance) return;
	shutdown = true;
	SetConsoleCtrlHandler( _HandlerRoutine, FALSE );

	if( eventThread )
	{
		event.set();
		eventThread->join();
		delete eventThread;
	}

	instance = 0;
}

bool PProcessKillHandler::_safeSignal()
{
	PLock lock( cs );
	if( shutdown )
		return true;
	return signal();
}

#elif defined ( PPOSIX )

/* static */ PProcessKillHandler* PProcessKillHandler::instance = 0;

static void signalhandler(int sigNum)
{
	PProcessKillHandler::instance->_safeSignal();
}

static void defaultsignalhandler(int sigNum)
{
	if (PLogIsInit())
		PLog("Signal %d received",sigNum);
}


PProcessKillHandler::PProcessKillHandler(const char * pyrHandler, UINT32 pPid)
{
	PASSERT4( instance == 0 );
	instance = this;
	shutdown = false;
	struct sigaction ss;
	memset(&ss,0,sizeof(ss));
	ss.sa_handler = signalhandler;
	int err = ::sigaction(PLIB_CONTROL_SIGNAL, &ss,0);
	if (err)
		throw PSystemError( "Unable to set sigaction", errno );
#if defined(P_AIX32) || defined(P_AIX64)
	PSIGHANDLER_INIT(PSIGHANDLER_NO_TRACE_DUMP);
	for (int i=1; i<64; i++)
	{
		if (i != SIGKILL && i != PLIB_CONTROL_SIGNAL && 
			i != SIGSEGV && i != SIGBUS &&i != SIGFPE &&i != SIGSYS &&i != SIGILL )
		{
			struct sigaction ssDef;
			memset(&ssDef,0,sizeof(ssDef));
			ssDef.sa_handler = defaultsignalhandler;
			::sigaction(i, &ssDef,0);
		}
	}
#elif defined (P_MAC10X) && defined(P_IOS)
	for (int i=1; i<32; i++)
	{
		if (i != SIGKILL && i != PLIB_CONTROL_SIGNAL && 
			i != SIGSEGV && i != SIGBUS &&i != SIGFPE &&i != SIGSYS &&i != SIGILL )
		{
			struct sigaction ssDef;
			memset(&ssDef,0,sizeof(ssDef));
			ssDef.sa_handler = defaultsignalhandler;
			::sigaction(i, &ssDef,0);
		}
	}
#endif

}

PProcessKillHandler::~PProcessKillHandler()
{
	if (!instance) return;
	shutdown = true;
	::signal(PLIB_CONTROL_SIGNAL, SIG_IGN);
	instance = 0;
}

bool PProcessKillHandler::_safeSignal()
{
	if( shutdown )
		return true;
	return signal();
}

#endif
