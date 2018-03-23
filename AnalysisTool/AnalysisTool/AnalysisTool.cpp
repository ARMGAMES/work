// AnalysisTool.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "analysis.h"

int _tmain(int argc, _TCHAR* argv[])
{
	string filename;
	int testMode;
	int lastDays = 0;
	float enterPert = 0;
	float stopPert = 0;
	float limitPert = 0;

	if (argc > 3)
	{
		wstring file(argv[1]);
		filename = string(file.begin(), file.end());
		testMode = stoi(argv[2]);
		lastDays = stoi(argv[3]);

		printf("Read file: %s \n", filename.c_str());
		printf("testMode: %d \n", testMode);
		printf("lastDays: %d \n", lastDays);

		if (testMode == 1)
		{
			enterPert = stof(argv[4]);
			stopPert = stof(argv[5]);
			printf("enterPert: %f \n", enterPert);
			printf("stopPert: %f \n", stopPert);
		}
		else if( testMode == 2)
		{
			enterPert = stof(argv[4]);
			limitPert = stof(argv[5]);
			printf("enterPert: %f \n", enterPert);
			printf("limitPert: %f \n", limitPert);
		}
	}
	else
	{
		return -1;
	}

	Analysis analysis;
	analysis.init(filename);
	//analysis.analyzeOpenClose();
	//analysis.analyzeContinous();
	if (testMode == 1)
	{
		analysis.analyzeLargeChangeTrading(lastDays, enterPert, stopPert);
	}
	else if (testMode == 2)
	{
		analysis.analyzeLittleChangeTrading(lastDays, enterPert, limitPert);	  
	}
	return 0;
}

