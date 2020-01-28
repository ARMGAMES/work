//////////////////////////////////////////////////////////////////////////

#include "DbTestingObject.h"

//////////////////////////////////////////////////////////////////////////

/* static */ CommServerNullGuardFactory DbTestingObject::guardFactory;

//////////////////////////////////////////////////////////////////////////

void DbTestingObject::init(CommMsgBody& initMsg)
{
	CommMsgParser parser(initMsg);
	const char* initDir_;
	parser.parseString(initDir_);

	if (!parser.parseEnded())
	{
		parser.parseStringP(loggerStationName);
		PLog("DBM logger station name [%s] was provided at startup and will override correspondent logger's INI property", loggerStationName.c_str());
	}

	initDir.assign(initDir_);
	fullFileName.assign(initDir_);
	fullFileName.append("dbtesting.ini");
	PIniFile ini(fullFileName);

	dbManagerMain.readDynamicConfiguration(ini);

	dbManagerMain.init(this);
	if (!dbManagerMain.openDataStorage(fullFileName, "MAINSTORAGE"))
		throw PError("ERROR: Main data storage not open");

	srvPool.registerFactory("test", connFactory);
}

//////////////////////////////////////////////////////////////////////////
