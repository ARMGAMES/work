#include "ppinclude.h"
#include "ppstring.h"
#include "pplogfile.h"
#include "ppdaemon.h"

#ifdef PWIN32

#include "ppstacktrace.h"

/* static */ PWin32Service* PWin32Service::instance = 0;

static void WINAPI _service_main( DWORD dwArgc, LPSTR *lpszArgv );
static void WINAPI _service_ctrl( DWORD dwCtrlCode );

/* static */ void PWin32Service::serviceDir( PString& ret )
{
	char szPath[ 512 ];
	if( GetModuleFileNameA( NULL, szPath, 512 ) == 0 )
	{
		ret = ".\\";
		return;
	}

	char drive[ _MAX_DRIVE ];
	char dir[ _MAX_DIR ];

	_splitpath( szPath, drive, dir, NULL, NULL );
	char path[ _MAX_PATH ];
	_makepath( path, drive, dir, NULL, NULL );
	ret = path;
}

PWin32Service::PWin32Service( PDaemon& daemon_, const char* name_ )
: name( name_ )
{
	PASSERT4( instance == 0 );
	instance = this;
	daemon = &daemon_;
	dwCheckPoint = 1;
	waitEvent = CreateEvent( NULL, TRUE, FALSE, NULL );
}

PWin32Service::~PWin32Service()
{
	if ( !instance ) return;
	CloseHandle( waitEvent );
	instance = 0;
}

void PWin32Service::run()
{
	dispatchTable[ 0 ].lpServiceName = (char*)name.c_str();
	dispatchTable[ 0 ].lpServiceProc = _service_main;
	dispatchTable[ 1 ].lpServiceName = NULL;
	dispatchTable[ 1 ].lpServiceProc = NULL;
	if( !StartServiceCtrlDispatcherA( dispatchTable ) )
	{
		PString errMsg;
		PSystemError::errorMessage( errMsg, GetLastError() );
		PLog( "PWin32Service: StartServiceCtrlDispatcher failed: %s", errMsg.c_str() );
	}
}

static void WINAPI _service_main( DWORD dwArgc, LPSTR *lpszArgv )
{
	PASSERT5( PWin32Service::instance );
	PWin32Service::instance->_serviceMain( dwArgc, lpszArgv );
}

void PWin32Service::_serviceMain( DWORD dwArgc, LPSTR *lpszArgv )
{
	// register our service control handler:
	//
	sshStatusHandle = RegisterServiceCtrlHandlerA( name, _service_ctrl );

	if( sshStatusHandle )
	{
		// SERVICE_STATUS members that don't change in example
		//
		ssStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
		ssStatus.dwServiceSpecificExitCode = 0;

		// report the status to the service control manager.
		//
		if( _reportStatusToSCMgr( SERVICE_START_PENDING, NO_ERROR, 3000 /* wait hint */ ) )
		{
			try
			{
				PString dir;
				serviceDir( dir );
				daemon->start( dir, dwArgc - 1, lpszArgv + 1 );
			}
			catch( const PError& err )
			{
				PLog( "PWin32Service: error in start(): '%s'", err.why() );
				_reportStatusToSCMgr( SERVICE_STOPPED, ERROR_INVALID_FUNCTION, 0 );
			}

			if( _reportStatusToSCMgr(
				SERVICE_RUNNING,       // service state
				NO_ERROR,              // exit code
				0) )                   // wait hint
			{
				WaitForSingleObject( waitEvent, INFINITE );

				try
				{
					daemon->stop();
				}
				catch( const PError& err )
				{
					PLog( "PWin32Service: error in stop(): '%s'", err.why() );
				}
			}
		}
	}

	// try to report the stopped status to the service control manager.
	//
	if( sshStatusHandle )
		_reportStatusToSCMgr( SERVICE_STOPPED, NO_ERROR, 0 );
}

static void WINAPI _service_ctrl( DWORD dwCtrlCode )
{
	PASSERT5( PWin32Service::instance );
	PWin32Service::instance->_serviceCtrl( dwCtrlCode );
}

void PWin32Service::_serviceCtrl( DWORD dwCtrlCode )
{
	switch( dwCtrlCode )
	{
		// Stop the service.
		//
		// SERVICE_STOP_PENDING should be reported before
		// setting the Stop Event - hServerStopEvent - in
		// ServiceStop().  This avoids a race condition
		// which may result in a 1053 - The Service did not respond...
		// error.
	case SERVICE_CONTROL_STOP:
		_reportStatusToSCMgr( SERVICE_STOP_PENDING, NO_ERROR, 0 );
		SetEvent( waitEvent );
		return;

		// System is going to shutdown, stop service;
	case SERVICE_CONTROL_SHUTDOWN:
		_reportStatusToSCMgr( SERVICE_STOP_PENDING, NO_ERROR, 0 );
		SetEvent( waitEvent );
		return;

		// Update the service status.
		//
	case SERVICE_CONTROL_INTERROGATE:
		break;

		// invalid control code
		//
	default:
		break;
	}
	_reportStatusToSCMgr( ssStatus.dwCurrentState, NO_ERROR, 0 );
}

bool PWin32Service::_reportStatusToSCMgr( DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint )
{
	if( dwCurrentState == SERVICE_START_PENDING )
		ssStatus.dwControlsAccepted = 0;
	else
		ssStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;

	ssStatus.dwCurrentState = dwCurrentState;
	ssStatus.dwWin32ExitCode = dwWin32ExitCode;
	ssStatus.dwWaitHint = dwWaitHint;

	if ( ( dwCurrentState == SERVICE_RUNNING ) ||
		( dwCurrentState == SERVICE_STOPPED ) )
		ssStatus.dwCheckPoint = 0;
	else
		ssStatus.dwCheckPoint = dwCheckPoint++;

	// Report the status of the service to the service control manager.
	//
	bool Ok = SetServiceStatus( sshStatusHandle, &ssStatus ) != 0;
	if( !Ok )
		PLog( "PWin32Service: Service error: SetServiceStatus failed" );
	return Ok;
}

/* static */ void PWin32Service::installService( const char* name,  const char * localAccount, const char * password, const char* displayName_, const vector< const char* >* dependencies)
{
	const char* displayName = displayName_ ? displayName_ : name;

	char szPath[ 512 ];
	if( GetModuleFileNameA( NULL, szPath, 512 ) == 0 )
	{
		PString errMsg;
		PSystemError::errorMessage( errMsg, GetLastError() );
		PLog( "Unable to install %s - %s", name, errMsg.c_str() );
		return;
	}

	SC_HANDLE schSCManager = OpenSCManager(
		NULL,                   // machine (NULL == local)
		NULL,                   // database (NULL == default)
		SC_MANAGER_ALL_ACCESS   // access required
		);
	if( schSCManager )
	{
		char* deps = 0;
		if( dependencies && dependencies->size() )
		{
			int i, n = dependencies->size(), l = 0;
			for( i=0; i < n ; ++i )
				l += strlen( (*dependencies)[ i ] ) + 1;
			deps = (char*)malloc( l + 1 );
			char* p = deps;
			for( i=0; i < n ; ++i )
			{
				strcpy( p, (*dependencies)[ i ] );
				p += strlen( p ) + 1;
			}
			*p = 0;
		}

		SC_HANDLE schService = CreateServiceA(
			schSCManager,               // SCManager database
			name,						// name of service
			displayName,				// name to display
			SERVICE_ALL_ACCESS,         // desired access
			SERVICE_WIN32_OWN_PROCESS,  // service type
			SERVICE_DEMAND_START,       // start type
			SERVICE_ERROR_NORMAL,       // error control type
			szPath,                     // service's binary
			NULL,                       // no load ordering group
			NULL,                       // no tag identifier
			deps,						// dependencies
			localAccount,               // LocalSystem account
			password);                  // no password

		if( deps )
			free( deps );

		if( schService )
		{
			PLog( "%s installed.", name );
			CloseServiceHandle( schService );
		}
		else
		{
			PString errMsg;
			PSystemError::errorMessage( errMsg, GetLastError() );
			PLog( "CreateService failed - %s", errMsg.c_str() );
		}

		CloseServiceHandle(schSCManager);
	}
	else
	{
		PString errMsg;
		PSystemError::errorMessage( errMsg, GetLastError() );
		PLog( "OpenSCManager failed - %s", errMsg.c_str() );
	}
}

void PWin32Service::removeService( const char* name )
{
	SC_HANDLE schSCManager = OpenSCManager(
		NULL,                   // machine (NULL == local)
		NULL,                   // database (NULL == default)
		SC_MANAGER_ALL_ACCESS   // access required
		);
	if( schSCManager )
	{
		SC_HANDLE schService = OpenServiceA( schSCManager, name, SERVICE_ALL_ACCESS );
		SERVICE_STATUS ssStatus;

		if( schService )
		{
			// try to stop the service
			if( ControlService( schService, SERVICE_CONTROL_STOP, &ssStatus ) )
			{
				PLog( "Stopping %s...", name );
				Sleep( 1000 );

				while ( QueryServiceStatus( schService, &ssStatus ) )
					if ( ssStatus.dwCurrentState == SERVICE_STOP_PENDING )
						Sleep( 1000 );
					else
						break;

				if ( ssStatus.dwCurrentState == SERVICE_STOPPED )
					PLog( "%s stopped.", name );
				else
					PLog( "%s failed to stop.", name );

			}

			// now remove the service
			if( DeleteService( schService ) )
				PLog( "%s removed.", name );
			else
			{
				PString errMsg;
				PSystemError::errorMessage( errMsg, GetLastError() );
				PLog( "DeleteService failed - %s", errMsg.c_str() );
			}

			CloseServiceHandle(schService);
		}
		else
		{
			PString errMsg;
			PSystemError::errorMessage( errMsg, GetLastError() );
			PLog( "OpenService failed - %s", errMsg.c_str() );

			CloseServiceHandle(schSCManager);
		}
	}
	else
	{
		PString errMsg;
		PSystemError::errorMessage( errMsg, GetLastError() );
		PLog( "OpenSCManager failed - %s", errMsg.c_str() );
	}
}

int PWin32ServiceDaemonMain( PDaemon& daemon, int argc, char** argv, const char* logFileName, const char* serviceName, const char* serviceDisplayName )
{
	if( argc >= 2 && strcmp( argv[ 1 ], "-install" ) == 0 )
	{
		PLogInit( new PLogFile( 0, PLogFile::alsoToStdout | PLogFile::catchAll /* no thread id */ ) );

		vector< const char* > deps;
		int i;
		for( i=2; i < argc ; ++i )
		{
			if( strcmp( argv[ i ], "-dependency" ) == 0 )
			{
				++i;
				if( i < argc )
					deps.push_back( argv[ i ] );
			}
			else if( strcmp( argv[ i ], "-servicename" ) == 0 )
			{
				++i;
				if( i < argc )
				{
					serviceName =  argv[ i ];
				}
			}
#if defined(PWIN32) && !defined(COMM_CLIENT_ONLY)
			else if (strcmp(argv[i], "-recoverstacktrace") == 0)
			{
				++i;
				if (i < argc)
				{
					StackTrace::decodeStackTrace(argv[i]);
				}
			}
#endif // PWIN32 && !COMM_CLIENT_ONLY
		}
		if (!serviceName)
		{
			PLog("Service name not provided, exiting");
			return 1;
		}
		char buffer_usr[100];
		printf("User:");
		if (gets_s(buffer_usr,100) && *buffer_usr)
		{
			char buffer_pwd[100];
			printf("Password:");
			int ch;
			for (i=0; i<99 && (ch=getch()) != 0 && ch != 13;i++)
				buffer_pwd[i] = ch;
			buffer_pwd[i]=0;
			PString user;
			if (strchr(buffer_usr,'\\') == 0)
			{
				user.assign(".\\");
			}
			user.append(buffer_usr);

			PWin32Service::installService( serviceName, user.c_str(), buffer_pwd, serviceDisplayName, &deps );
		}
		else
		{
			PWin32Service::installService( serviceName, 0, 0, serviceDisplayName, &deps );

		}
		return 0;
	}
	else if( argc >= 2 && strcmp( argv[ 1 ], "-remove" ) == 0 )
	{
		PLogInit( new PLogFile( 0, PLogFile::alsoToStdout | PLogFile::catchAll /* no thread id */ ) );
		int i;
		for( i=2; i < argc ; ++i )
		{
			if( strcmp( argv[ i ], "-servicename" ) == 0 )
			{
				++i;
				if( i < argc )
				{
					serviceName =  argv[ i ];
					break;
				}
			}
		}
		if (!serviceName)
		{
			PLog("Service name not provided, exiting");
			return 1;
		}

		PWin32Service::removeService( serviceName );
		return 0;
	}
	else if( argc >= 2 && strcmp( argv[ 1 ], "-debug" ) == 0 )
	{
		int skip = 2;
		if( argc >= 3 && strcmp( argv[ 2 ], "-logfile" ) == 0 )
		{
			logFileName = argv[ 3 ];
			skip = 4;
		}

		PLogInit( new PLogFile( logFileName, PLogFile::alsoToStdout | PLogFile::printThreadId | PLogFile::perDate | PLogFile::catchAll ) );

		PConsoleApplication app( daemon, argc - skip, argv + skip );
		app.run();
		return 0;
	}
	else if( argc == 1 )
	{
		PString logName;
		PWin32Service::serviceDir( logName );
		logName.append( logFileName );

		PLogInit( new PLogFile( logName, PLogFile::alsoToStdout | PLogFile::printThreadId | PLogFile::perDate | PLogFile::catchAll ) );
		PLog( "Attempting to start service..." );

		PWin32Service svc( daemon, serviceName );
		svc.run();
		return 0;
	}

	PLogInit( new PLogFile( 0, PLogFile::alsoToStdout | PLogFile::catchAll /* no thread id */ ) );
	PLog( "Usage:\n"
		"%s -install [-servicename <serviceName> [-dependency <dependencyName> [-dependency <dependencyName>]...]]\n"
		"\tinstalls WinNT service\n"
		"%s -remove [-servicename <serviceName>]\n"
		"\tremoves WinNT service\n"
		"%s -debug [params]\n"
		"\truns daemon in debug mode\n",
		argv[ 0 ], argv[ 0 ], argv[ 0 ] );
	return 1;
}

#elif defined (PPOSIX)



int PUnixDaemonMain( PDaemon& daemon, int argc, char** argv, const char* logFileName)
{
	int skip = 1;
	if( argc >= 2 && strcmp( argv[ 1 ], "-logfile" ) == 0 )
	{
		logFileName = argv[ 2 ];
		skip = 3;
	}

	PLogInit( new PLogFile( logFileName, PLogFile::alsoToStdout | PLogFile::printThreadId | PLogFile::perDate | PLogFile::catchAll ) );
	PConsoleApplication app( daemon, argc - skip, argv + skip );
	app.run();
	return 1;
}



#endif
