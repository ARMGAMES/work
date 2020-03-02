// ===========================================================
// Copyright (C) 2017 PYR Software Ltd. All rights reserved.
// ===========================================================

#include "commlocalapp.h"
#include "MainDbmobject.h"
#include "MainDbmstart.h"

int main( int argc, char** argv )
{
	PSocket::startup();
 	CommSSL::startup();

	MainDbmServerObjectFactory factory;
	int ret = CommLocalAppSharedMemMain( argc, argv, factory, PYRBUILD );

	CommSSL::cleanup();
 	PSocket::cleanup();
	return ret;
}
