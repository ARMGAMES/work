#ifndef ppdaemon_h_included
#define ppdaemon_h_included

#include "ppinclude.h"

/* abstract */ class PDaemon
{
public:
	virtual void start( const char* dir, int nParms, char** parms ) = 0;
	virtual void stop() = 0;
	virtual ~PDaemon() {}
};

#ifdef PWIN32
class PConsoleApplication//singleton!
{
public:
	static PConsoleApplication* instance;

private:
	PDaemon* daemon;
	int nParms;
	char** parms;
	HANDLE waitEvent;
	HANDLE killHandlerEvent;

public:
	PConsoleApplication( PDaemon& daemon_, int nParms_, char** parms_ );
	void run();
	~PConsoleApplication();

public:
	BOOL _handler( DWORD ctrlType );
};

class PWin32Service//singleton!
{
public:
	static PWin32Service* instance;

private:
	PDaemon* daemon;
	PString name;

	SERVICE_STATUS_HANDLE sshStatusHandle;
	SERVICE_STATUS ssStatus;
	DWORD dwCheckPoint;
	SERVICE_TABLE_ENTRYA dispatchTable[ 2 ];
	HANDLE waitEvent;

public:
	PWin32Service( PDaemon& daemon, const char* name_ );
	void run();
	~PWin32Service();

	static void installService( const char* name,  const char * localAccount, const char * password , const char* displayName = 0, const vector< const char* >* dependencies = 0);
	static void removeService( const char* name );
	static void serviceDir( PString& ret );

public:
	void _serviceMain( DWORD dwArgc, LPSTR *lpszArgv );
	void _serviceCtrl( DWORD dwCtrlCode );

private:
	bool _reportStatusToSCMgr( DWORD dwCurrentState, DWORD dwWin32ExitCode, DWORD dwWaitHint );
};

int PWin32ServiceDaemonMain( PDaemon& daemon, int argc, char** argv, const char* logFileName, const char* serviceName, const char* serviceDisplayName = 0 );

#elif defined (PPOSIX)

class PConsoleApplication//singleton!
{
private:
	PDaemon* daemon;
	int nParms;
	char** parms;

public:
	PConsoleApplication( PDaemon& daemon_, int nParms_, char** parms_ );
	void run();
	~PConsoleApplication();

};

int PUnixDaemonMain( PDaemon& daemon, int argc, char** argv, const char* logFileName );

#endif

#endif
