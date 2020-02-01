// ===========================================================
// Copyright (C) 2017 PYR Software Ltd. All rights reserved.
// ===========================================================

#include "commlocalapp.h"
#include "ColTCADbmobject.h"
#include "ColTCADbmstart.h"

int main( int argc, char** argv )
{
	PSocket::startup();
 	CommSSL::startup();

	ColTCADbmServerObjectFactory factory;
	int ret = CommLocalAppSharedMemMain( argc, argv, factory, PYRBUILD );

	CommSSL::cleanup();
 	PSocket::cleanup();
	return ret;
}
