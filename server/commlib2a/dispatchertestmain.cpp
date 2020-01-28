#include "commdispatcher.h"
#include "dispatcherteststart.h"

int main( int argc, char** argv )
{
	if( argc == 2 && !strcmp( argv[1], "-v" ) )
	{
		printf( PYRBUILD );
		return 0;
	}

	CommDispatcherDaemon daemon;

#ifdef PWIN32
	return PWin32ServiceDaemonMain( daemon, argc, argv, "dispatcher.log", "PYRTEST" );
#elif defined (PPOSIX)
	return PUnixDaemonMain( daemon, argc, argv, "dispatcher.log");
#endif
}
