////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2013 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "commlocalapp.h"
#include "LoopbackObject.h"
#include "LoopbackStart.h"

int main( int argc, char** argv )
{
	LoopbackServerObjectFactory factory;
	return CommLocalAppSharedMemMain( argc, argv, factory, PYRBUILD );
}

////////////////////////////////////////////////////////////////////////////////
