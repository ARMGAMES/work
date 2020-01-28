////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2019 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "commlocalapp.h"
#include "pdserverSrvObject.h"
#include "pdserverStart.h"

int main(int argc, char** argv)
{
	PDServerObjectFactory factory;
	return CommLocalAppSharedMemMain(argc, argv, factory, PYRBUILD);
}
