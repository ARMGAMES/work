#include "ppinclude.h"
#include "pperror.h"
#include "ppstring.h"
#include "pplogfile.h"
#include "ppprocess.h"

#ifdef PWIN32

namespace
{

void CALLBACK onProcessExitedCallback(_In_ PVOID   lpParameter, _In_ BOOLEAN TimerOrWaitFired)
{
	ProcessExitedCallback* callback = reinterpret_cast<ProcessExitedCallback*>(lpParameter);
	if (callback)
	{
		callback->onProcessExited();
	}
}

} // unnamed namespace

/* static */ PCriticalSection PProcess::cs;
/* static */ unsigned short PProcess::globalCounter=0;

PProcess::PProcess()
{
	//groupId = 0;
	handle = onTerminateCallbackHandle = INVALID_HANDLE_VALUE;
	pId = -1;
}

PProcess::~PProcess()
{
	if( handle != INVALID_HANDLE_VALUE )
		::CloseHandle( handle );
}


void PProcess::start( const char* exeName, int nParms, const char* const* parms )
{
	closeHandles();
	PString cmdLine;
	cmdLine.append( '\"' ).append( exeName ).append( '\"' );
	for( int i=0; i < nParms ; ++i )
		cmdLine.append( " \"" ).append( parms[ i ] ).append( '\"' );
	PArrayAutoPtr< char > cmd = new char[ cmdLine.length() + 1 ];
	strcpy( cmd, cmdLine );


	char eventName[256];
	unsigned short globalCounterCopy;
	{
		PLock lock(cs);
		globalCounterCopy=++globalCounter;
	}

	sprintf(eventName,"pyrsftevnt%08lx-%04x-%08lx",::GetCurrentProcessId(), globalCounterCopy,GetTickCount());

	/*
	SECURITY_ATTRIBUTES sa;//only to inherit this event
	sa.nLength = sizeof( sa );
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = TRUE;
	event = ::CreateEvent( &sa, TRUE, FALSE , NULL );

	PASSERT5( event != NULL );

	PString env( PYRKILLEVENTHANDLE "=" );
	{
	char buf[ 32 ];
	sprintf( buf, "%08X", event );
	env.append( buf );
	}
	*/
	event.create( false, eventName, true );
	PString env( PYRKILLEVENTHANDLE "=" );
	env.append( eventName );

	STARTUPINFOA startup;
	memset( &startup, 0, sizeof(startup) );
	startup.cb = sizeof(startup);
	PROCESS_INFORMATION pi;

	BOOL Ok;
	{
		PLock lock( cs );//to protect environment variables from changing between putenv - CreateProcess calls
		putenv( env );
		//	Ok = ::CreateProcess( NULL, cmd, NULL /*SD*/, NULL/*SD*/, TRUE /*inherit handles - to inherit 'event'*/, CREATE_NEW_PROCESS_GROUP, NULL/*inherit environment*/, NULL/*inherit cwd*/, &startup, &pi );
		//	Ok = ::CreateProcess( NULL, cmd, NULL /*SD*/, NULL/*SD*/, TRUE /*inherit handles - to inherit 'event'*/, DETACHED_PROCESS, NULL/*inherit environment*/, NULL/*inherit cwd*/, &startup, &pi );
		Ok = ::CreateProcessA( NULL, cmd, NULL /*SD*/, NULL/*SD*/, FALSE/* don't inherit handles - to inherit 'event'*/, DETACHED_PROCESS, NULL/*inherit environment*/, NULL/*inherit cwd*/, &startup, &pi );
	}//unlock

	//FILE* f = fopen( "ppprocess", "at" );
	//fprintf( f, "'%s': %d %d %d\n", cmd, Ok, err, pi.dwProcessId );
	//fclose( f );
	if( !Ok )
	{
		event.close();
		throw PSystemError( "Unable to create process", GetLastError() );
	}
	::CloseHandle( pi.hThread );
	handle = pi.hProcess;
	pId = pi.dwProcessId;
	//groupId = pi.dwProcessId;
}

bool PProcess::getExitCode(long& exitCode) const
{
	DWORD code;
	if (GetExitCodeProcess(getHandle(), &code))
	{
		if (code == STILL_ACTIVE)
		{
			PTRACE3("Unable to obtain process exit code - PID %u is still running", pId);
			exitCode = 0;
			return false;
		}
		exitCode = static_cast<long>(code);
		return true;
	}
	PString errMsg;
	PSystemError::errorMessage(errMsg, GetLastError());
	PTRACE3("Failed to obtain process exit code for PID %u: %s", pId, errMsg.c_str());
	exitCode = -1;
	return false;
}

bool PProcess::setOnExitedCallback(ProcessExitedCallback* callback)
{
	return (callback && handle != INVALID_HANDLE_VALUE && RegisterWaitForSingleObject(&onTerminateCallbackHandle, handle, onProcessExitedCallback, callback, INFINITE, WT_EXECUTEONLYONCE));
}

void PProcess::clearOnExitedCallback()
{
	if (onTerminateCallbackHandle != INVALID_HANDLE_VALUE)
	{
		if (!UnregisterWait(onTerminateCallbackHandle))
		{
			PString err;
			PSystemError::errorMessage(err, GetLastError());
			PLog("Error: UnregisterWait failed for PID %u: %s", pId, err.c_str());
		}
		onTerminateCallbackHandle = INVALID_HANDLE_VALUE;
	}
}

/*
bool PProcess::isRunning() const
{
PASSERT4( handle != INVALID_HANDLE_VALUE );
DWORD res = ::WaitForSingleObject( handle, 0 );
return res != WAIT_TIMEOUT;
}
*/
void PProcess::kill()
{
	//::GenerateConsoleCtrlEvent( CTRL_BREAK_EVENT, groupId );
	event.set();
}

void PProcess::kill9()
{
	::TerminateProcess( handle, 13 );
}

void PProcess::join()
{
	::WaitForSingleObject( handle, INFINITE );
}
void  PProcess::setenv(const char * name, const char * value)
{
	PString env;
	env.append(name).append("=").append(value);
	::putenv(env.c_str());
}


#elif defined ( PPOSIX )

PProcess::PProcess()
{
	processPid = 0;
}

PProcess::~PProcess()
{

}

void PProcess::start( const char* exeName, int nParms, const char* const* parms )
{
	if (!(processPid = ::fork()))
	{
		char * * argv = new char * [nParms + 2];
		argv[0]=(char *) exeName;
		for( int i=1; i <= nParms ; ++i )
		{
			argv[i] = (char *)parms[i-1];
		}
		argv[nParms+1]=0;
		if (::execv(exeName,argv) == -1)
		{
			::exit(-1);
		}
		delete [] argv;
	}
}

bool PProcess::getExitCode(long& exitCode) const
{
	int status;
	PASSERT3(processPid);
	if (::waitpid(processPid, &status, WNOHANG | WCONTINUED) != -1)
	{
		if (WIFEXITED(status))
		{
			exitCode = WEXITSTATUS(status);
			return true;
		}
		if (WIFSIGNALED(status))
		{
			exitCode = WTERMSIG(status);
			return true;
		}
		if (WIFSTOPPED(status))
		{
			exitCode = WSTOPSIG(status);
			return true;
		}
		PTRACE3("Unable to obtain process exit code - PID %u is still running", processPid);
		exitCode = 0;
		return false;
	}
	PTRACE3("Failed to obtain process exit code for PID %u: %s", processPid, strerror(errno));
	exitCode = -1;
	return false;
}

void PProcess::kill()
{
	if (processPid)
	{
		PTRACE3("kill process  %d",processPid);
		::kill(processPid, PLIB_CONTROL_SIGNAL);
	}
}
void PProcess::kill9()
{
	if (processPid)
	{
		PASSERT3(processPid);
		::kill(processPid, SIGKILL);
	}
}
void  PProcess::join()
{
	int status;
	PASSERT3(processPid);
	//	::wait4(processPid,&status,WNOHANG,0);
	::waitpid(processPid,&status,0);
}

void  PProcess::setenv(const char * name, const char * value)
{
	::setenv(name,value,1);
}


#endif
