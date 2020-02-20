// ===========================================================
// Copyright (C) 2017 PYR Software Ltd. All rights reserved.
// ===========================================================

#include "commlocalapp.h"
#include "SrvTemplateDbmobject.h"
#include "SrvTemplateDbmstart.h"

int main( int argc, char** argv )
{
	PSocket::startup();
 	CommSSL::startup();

	SrvTemplateDbmServerObjectFactory factory;
	int ret = CommLocalAppSharedMemMain( argc, argv, factory, PYRBUILD );

	CommSSL::cleanup();
 	PSocket::cleanup();
	return ret;
}
