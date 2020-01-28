#include "ppinclude.h"
#include "ppstring.h"
#include "pplogfile.h"
#include "ppdaemon.h"
#include "ppprocess.h"

#ifdef PWIN32

class _PConsoleKillHandler : public PProcessKillHandler
{
private:
	HANDLE event;

public:
	_PConsoleKillHandler( HANDLE event_ )
	{
		event = event_;
	}

public:
	bool signal() override
	{
		::SetEvent( event );
		return true;
	}
};

/* static */ PConsoleApplication* PConsoleApplication::instance = 0;

static BOOL WINAPI _HandlerRoutine( DWORD ctrlType )
{
	return PConsoleApplication::instance->_handler( ctrlType );
}

PConsoleApplication::PConsoleApplication( PDaemon& daemon_, int nParms_, char** parms_ )
{
	PASSERT4( instance == 0 );
	instance = this;
	daemon = &daemon_;
	nParms = nParms_;
	parms = parms_;
	waitEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	killHandlerEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
	::SetConsoleCtrlHandler( _HandlerRoutine, TRUE );
}

void PConsoleApplication::run()
{
	try
	{
		_PConsoleKillHandler killHandler( killHandlerEvent );
		daemon->start( "", nParms, parms );
		//::WaitForSingleObject( waitEvent, INFINITE );
		HANDLE handles[ 2 ] = { waitEvent, killHandlerEvent };
		::WaitForMultipleObjects( 2, handles, FALSE, INFINITE );
		daemon->stop();
	}
	catch( PError& err )
	{
		PLog( "Last hope catch in PConsoleApplication::run(): '%s'", err.why() );
	}
}

PConsoleApplication::~PConsoleApplication()
{
	if ( !instance ) return;
	::SetConsoleCtrlHandler( _HandlerRoutine, FALSE );
	::CloseHandle( killHandlerEvent );
	::CloseHandle( waitEvent );
	instance = 0;
}

BOOL PConsoleApplication::_handler( DWORD ctrlType )
{
	::SetEvent( waitEvent );
	return TRUE;
}

#elif defined (PPOSIX)


PConsoleApplication::PConsoleApplication( PDaemon& daemon_, int nParms_, char** parms_ )
{
	daemon = &daemon_;
	nParms = nParms_;
	parms = parms_;
}

void PConsoleApplication::run()
{
	try
	{
		sigset_t set_s;
		sigemptyset(&set_s);
		sigaddset(&set_s, PLIB_CONTROL_SIGNAL);
		sigprocmask(SIG_BLOCK,&set_s,0);
		PProcessKillHandler killHandler; // we need this to set signal handle,
		// otherwise PLIB_CONTROL_SIGNAL will be processed by default -exiting	
		daemon->start( "", nParms, parms );
		sigfillset(&set_s);
		int err = sigdelset(&set_s, PLIB_CONTROL_SIGNAL);
		err = ::pthread_sigmask(SIG_SETMASK,&set_s,0);
		sigsuspend(&set_s);
		daemon->stop();
	}
	catch( PError& err )
	{
		PLog( "Last hope catch in PConsoleApplication::run(): '%s'", err.why() );
	}
}

PConsoleApplication::~PConsoleApplication()
{
}


#endif
