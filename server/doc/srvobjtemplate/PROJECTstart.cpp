// ===========================================================
// Copyright (C) 2012 PYR Software Ltd. All rights reserved.
// ===========================================================

#include "commlocalapp.h"
#include "PROJECTobject.h"
#include "PROJECTstart.h"

int main( int argc, char** argv )
{
	PROJECTServerObjectFactory factory;
	// return CommLocalAppMain( argc, argv, factory, PYRBUILD );
	return CommLocalAppSharedMemMain( argc, argv, factory, PYRBUILD );
}
