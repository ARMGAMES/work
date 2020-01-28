#include "commdispatcher.h"
#include "dispatcherstart.h"


int main( int argc, char** argv )
{
	if( argc == 2 && !strcmp( argv[1], "-v" ) )
	{
		printf( PYRBUILD );
		return 0;
	}

	CommDispatcherDaemon daemon;
	return PWin32ServiceDaemonMain( daemon, argc, argv, "dispatcher.log", 0 );
}
