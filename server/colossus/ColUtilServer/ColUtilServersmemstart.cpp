////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2017 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "commlocalapp.h"
#include "ColSrvObject.h"
#include "ColUtilServerStart.h"

int main(int argc, char** argv)
{
	ColUtilServerObjectFactory factory;
	return CommLocalAppSharedMemMain(argc, argv, factory, PYRBUILD);
}
