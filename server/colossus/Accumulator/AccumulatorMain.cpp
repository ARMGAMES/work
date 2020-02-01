////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2016 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "AccumulatorStart.h"
#include "AccumulatorDbManager.h"
#include "../utils.h" 

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[]) 
{
	if (argc == 2 && !strcmp(argv[1], "-v"))
	{
		printf("%s", PYRBUILD);
		return 0;
	}

	PLogFile log("accumulator.log", PLogFile::alsoToStdout | PLogFile::printTimePerLine | PLogFile::perDate); 
	PLogInit(&log);
	PLog("Starting %s %s [%s] ...", argv[0], argv[1], PYRBUILD " - " __DATE__ ", " __TIME__);

	
	PString usageStr;
	usageStr.append("Usage1: ").append(argv[0]).append(" [day|week] [YYYY-MM-DD]\n");
	usageStr.append("Usage2: ").append(argv[0]).append(" fix [YYYY-MM-DD] [fileName]\n");

	PError usage(usageStr);

	try
	{
		AccumulatorDbManager dbManager;
		
		if (argc == 3) 
		{
			AccumulationType type;
			SrvDate date;
			if (strcmp(argv[1], "day") == 0)
			{
				type = ACCUM_DAILY;
			}
			else if (strcmp(argv[1], "week") == 0)
			{
				type = ACCUM_WEEKLY;
			}
			else
			{
				throw usage;
			}

			if (!strToSrvDate(argv[2], date))
			{
				throw usage;
			}

			dbManager.initAccumMode(type, date);
			dbManager.startAccumWithSQLAccum();
		}
		else if (argc == 4)
		{
			SrvDate startDate;
			PString fileName;
			if (strcmp(argv[1], "fix") != 0)
			{
				throw usage;
			}

			if (!strToSrvDate(argv[2], startDate))
			{
				throw usage;
			}

			fileName = argv[3];

			dbManager.initAccumFixMode(startDate, fileName);
			dbManager.startAccumulationFix();
		}
		else
		{
			throw usage;
		}

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
