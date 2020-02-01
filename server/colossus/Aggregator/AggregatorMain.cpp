////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2011 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "AggregatorStart.h"
#include "AggregatorDbManager.h"
#include "../utils.h" 

#define INI_FILE "aggregator.ini"

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) //lint -esym(818, argv)
{
	if (argc == 2 && !strcmp(argv[1], "-v"))
	{
		printf("%s", PYRBUILD);
		return 0;
	}

	PLogFile log("aggregator.log", PLogFile::alsoToStdout | PLogFile::printTimePerLine | PLogFile::perDate); //lint !e578 (hiding)
	PLogInit(&log);
	PLog("Starting %s [%s] ...", argv[0], PYRBUILD " - " __DATE__ ", " __TIME__);

	//lint -esym(1793, PString::append)
	PError usage(PString("Usage: ").append(argv[0]).append(" [YYYY-MM-DD]\nUses yesterday's date by default"));

	try
	{
		SrvDate date;

		if (argc == 1) // No date specified
		{
			date.currentLocalDate();
			ColUtils::addDays(date, -1); 
		}

		else if (argc == 2) // Parse date
		{
			UINT32 day, month, year;
			if (sscanf(argv[1], "%4u-%2u-%2u", &year, &month, &day) != 3)
				throw usage;

			if (year < 2011 || month > 12 || day > 31)
				throw usage;

			date._day = day; date._month = month; date._year = year;
		}

		else
			throw usage;

		AggregatorDbManager dbManager;
		dbManager.init(INI_FILE, date);

		dbManager.aggregateRawHands(date);
		return 0;
	}
	
	catch (PSqlError const& err)
	{
		PLog("Error: %s", err.why());
		return err.code();
	}

	catch (PError const& err)
	{
		PLog("Error: %s", err.why());
	}
 
	catch (...) 
	{
		PLog("Unknown error"); 
	}
 
	return -1; // Error 
}

////////////////////////////////////////////////////////////////////////////////
