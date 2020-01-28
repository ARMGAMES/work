#include "commdispatcher.h"
#include "dispatcherstart.h"

#ifndef SERVICE_PYR
#error Service name must be defined
#endif

int main( int argc, char** argv )
{
	if( argc == 2 && !strcmp( argv[1], "-v" ) )
	{
		printf( "%s", PYRBUILD );
		return 0;
	}

	CommDispatcherDaemon daemon;
	return PWin32ServiceDaemonMain( daemon, argc, argv, "dispatcher.log", SERVICE_PYR );
}
