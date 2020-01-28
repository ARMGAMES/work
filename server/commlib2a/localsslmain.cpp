#include "commdispatcher.h"
#include "commlocalapp.h"
#include "localsslstart.h"

int main( int argc, char** argv )
{
	if( argc == 2 && !strcmp( argv[1], "-v" ) )
	{
		printf( PYRBUILD );
		return 0;
	}

	return CommLocalTcpAppMain( argc, argv );
}
