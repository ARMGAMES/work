#include "stdafx.h"
#include "analysis.h"

using namespace std;


bool Analysis::init(std::string file_name)
{
  ifstream input_stream(file_name);

  string line;

  if(!input_stream.is_open())
  {
    return false;
  }
    
  while (getline(input_stream, line))
  {
    if(line.find("Date") != string::npos)
      continue;

    vector<string> tokens;
    splitbychar(line, kSeparator, tokens);

    MarketData data;
    data.date = tokens[0];
    data.open = stof(tokens[1]);
    data.high = stof(tokens[2]);
    data.low = stof(tokens[3]);
    data.close = stof(tokens[4]);

    market_data.push_back(data);
  }
  input_stream.close();
  return true;
}

void Analysis::analyzeContinous()
{
  map<int, int> continousUp;
  map<int, int> continousDown;

  int currContUp=0;
  int currContDown=0;

  for (int i = market_data.size()- 2; i >= 0; --i)
  {
    // last day's index
    int prev = i + 1;

    float prevClose = market_data[prev].close;
    float prevOpen = market_data[prev].open;

    float todayOpen = market_data[i].open;
    float todyaClose = market_data[i].close;

    bool prevUp = prevClose > prevOpen ? true : false;
    bool todayUp = todyaClose > todayOpen ? true : false;

    if (prevUp)
    {
      if (todayUp)
      {
        currContUp++;
      }
      else
      {
        if (continousUp.find(currContUp) == continousUp.end())
          continousUp[currContUp] = 1;
        else
          continousUp[currContUp] = continousUp[currContUp] + 1;

        currContUp = 0;
        currContDown = 1;
      }
    }
    else
    {
      if (todayUp)
      {
        if (continousDown.find(currContDown) == continousDown.end())
          continousDown[currContDown] = 1;
        else
          continousDown[currContDown] = continousDown[currContDown] + 1;

        currContDown = 0;
        currContUp = 1;
      }
      else
      {
        currContDown++;
      }
    }
  }

  int total_up = 0;
  int total_down = 0;
  
  int countUP = 0;
  int countDown = 0;
  for (map<int, int>::iterator it = continousUp.begin(); it != continousUp.end(); ++it)
  {
    total_up = total_up + it->second;
    countUP = countUP + it->first * it->second;
  }
  for (map<int, int>::iterator it = continousDown.begin(); it != continousDown.end(); ++it)
  {
    total_down = total_down + it->second;
    countDown = countDown + it->first * it->second;
  }
  printf("***************************\n");
  printf("Total up days = %d\n", countUP);
  printf("Total down days = %d\n", countDown);
  printf("***************************\n");
  for (map<int, int>::iterator it = continousUp.begin(); it != continousUp.end(); ++it)
  {
    printf("continous number of up %d occurred %d, percentage = %.2f \n", it->first, it->second, (float)it->second/total_up);
  }

  printf("***************************\n");
  for (map<int, int>::iterator it = continousDown.begin(); it != continousDown.end(); ++it)
  {
    printf("continous number of down %d occurred %d, percentage = %.2f\n", it->first, it->second, (float)it->second/total_down);
  }
}

void Analysis::analyzeOpenClose()
{
  int positive = 0;
  int nochange = 0;
  int negagive = 0;

  int totalCount=0;

  for (int i = market_data.size()- 2; i >= 0; --i)
  {
    // last day's index
    int prev = i + 1;

    float prevClose = market_data[prev].close;
    float prevOpen = market_data[prev].open;

    float todayOpen = market_data[i].open;
    float todyaClose = market_data[i].close;


    // prev day down
    if (prevOpen > prevClose)
    {
      // open high
      if (todayOpen > prevClose)
      {
        if (todyaClose > todayOpen)
        {
          positive++;
        }
      }
      // open low
      else if (todayOpen < prevClose)
      {
        if (todyaClose < todayOpen)
        {
          positive++;
        }
      }
      // open =
      else
      {
        if (todyaClose > todayOpen)
        {
          positive++;
        }
        //nochange++;
      }
    }
    // prev day up
    else if (prevOpen < prevClose)
    {
      // open high
      if (todayOpen > prevClose)
      {
        if (todyaClose > todayOpen)
        {
          positive++;
        }
      }
      // open low
      else if (todayOpen < prevClose)
      {
        if (todyaClose < todayOpen)
        {
          positive++;
        }
      }
      // open =
      else
      {
        if (todyaClose < todayOpen)
        {
          positive++;
        }
        //nochange++;
      }
    }
    else
    {
      nochange++;
    }
  }

  printf("Total market days: %d\n", market_data.size()-1);
  printf("Positive days: %d, percentage: %f\n", positive, (float)positive/(market_data.size()-1-nochange));
  printf("No change days: %d\n", nochange);
}

void Analysis::analyzeLargeChangeTrading(int lastDays, float enterPert, float stopPert)
{
  int openHighCount = 0;
  int openHigh_closeHigh = 0;
  int openLowCount = 0;
  int openLow_closeLow = 0;

  float averagLongGain = 0;
  float averagShortGain = 0;

  int unchanged=0;

  int index = 0;
  if (lastDays == 0)
  {
    index = market_data.size()- 2;
  }
  else
  {
    index = lastDays;
  }

  for (int i = index; i >= 0; --i)
  {
    // last day's index
    int prevDay = i + 1;

    float prevClose = market_data[prevDay].close;

    float todayOpen = market_data[i].open;
    float todayClose = market_data[i].close;
    float todayLow = market_data[i].low;
    float todayHigh = market_data[i].high;

    // open high
    if (todayOpen/prevClose > (1+enterPert))
    {
      openHighCount++;

      if (todayClose > todayOpen && todayLow/todayOpen > (1-stopPert))
      {
        averagLongGain += todayClose/todayOpen;
        //printf("Date:%s - PreClose:%f, Open:%f, Close:%f\n", market_data[i].date.c_str(), prevClose, todayOpen, todayClose);
        openHigh_closeHigh++;
      }
    }
    // open low
    else if (todayOpen/prevClose < (1-enterPert))
    {
      openLowCount++;

      if (todayClose < todayOpen && todayHigh/todayOpen < (1 + stopPert))
      {
        averagShortGain += todayOpen/todayClose;
        openLow_closeLow++;
      }
    }
    else
    {
      unchanged++;
    }
  }

  printf("Total market days: %d\n", index);
  printf("----------------------------\n");
  printf("Open high: %d, Close high: %d, percentage: %f, average gain: %f \n", openHighCount, openHigh_closeHigh, (float)openHigh_closeHigh/openHighCount, averagLongGain/openHigh_closeHigh);
  printf("Open Low: %d, Close Low: %d, percentage: %f, average gain: %f \n", openLowCount, openLow_closeLow, (float)openLow_closeLow/openLowCount, averagShortGain/openLow_closeLow);
  printf("----------------------------\n");
  printf("No change days: %d\n", unchanged);
}

void Analysis::analyzeLittleChangeTrading(int lastDays, float enterPert, float limitPert)
{
	int openlittlehigh = 0;
	int openlittlehigh_win = 0;
	int openlittlehigh_loss = 0;

	int openlittlelow = 0;
	int openlittlelow_win = 0;
	int openlittlelow_loss = 0;

	float LittleHigh_PeakHigh = 0;
	float LittleHigh_PeakLow = 100;
	float averagLittleHighWin = 0;
	float averagLittleHighLoss = 0;

	float LittleLow_PeakHigh = 0;
	float LittleLow_PeakLow = 100;
	float averagLittleLowLWin = 0;
	float averagLittleLowLoss = 0;

	int unchanged=0;

	int index = 0;
	if (lastDays == 0)
	{
		index = market_data.size()- 2;
	}
	else
	{
		index = lastDays;
	}

	for (int i = index; i >= 0; --i)
	{
		// last day's index
		int prevDay = i + 1;

		float prevClose = market_data[prevDay].close;

		float todayOpen = market_data[i].open;
		float todayClose = market_data[i].close;
		float todayLow = market_data[i].low;
		float todayHigh = market_data[i].high;

		// open little high
		if ( todayOpen > prevClose && todayOpen/prevClose > (1+enterPert))
		{
			openlittlehigh++;
			/*averagLittleHighWin += todayHigh / prevClose;
			averagLittleHighLoss += todayLow / prevClose;
			if (todayHigh / prevClose > LittleHigh_PeakHigh)
			{
				LittleHigh_PeakHigh = todayHigh / prevClose;
			}
			if (todayLow / prevClose < LittleHigh_PeakLow)
			{
				LittleHigh_PeakLow = todayLow / prevClose;
			}*/
			if (todayClose > todayOpen)
			{
				openlittlehigh_win++;
				//averagLittleHighWin += todayClose/todayOpen;
			}

		}
		// open little low
		else if ( todayOpen < prevClose && todayOpen/prevClose < (1-enterPert))
		{
			openlittlelow++;
			/*averagLittleLowLWin += prevClose / todayLow;
			averagLittleLowLoss += prevClose / todayHigh;
			if (prevClose / todayLow > LittleLow_PeakHigh)
			{
				LittleLow_PeakHigh = prevClose / todayLow;
			}
			if (prevClose / todayHigh < LittleLow_PeakLow)
			{
				LittleLow_PeakLow = prevClose / todayHigh;
			}*/
			if (todayClose < todayOpen)
			{
				openlittlelow_win++;
				//averagLittleLowLWin += todayOpen/todayClose;
			}

		}
		else
		{
			unchanged++;
		}
	}

	printf("Total market days: %d\n", index);
	printf("----------------------------\n");
	printf("Open Little high: %d, win: %d, percentage: %f, average win: %f\n", openlittlehigh, openlittlehigh_win, (float)openlittlehigh_win/openlittlehigh, averagLittleHighWin/openlittlehigh_win);
	printf("Open Little low: %d, win: %d, percentage: %f, average win: %f\n", openlittlelow, openlittlelow_win, (float)openlittlelow_win/openlittlelow, averagLittleLowLWin/openlittlelow_win);
	//printf("Open Little high: %d, average win: %f, average loss: %f, peakhigh: %f, peaklow: %f\n", openlittlehigh,averagLittleHighWin/openlittlehigh, averagLittleHighLoss/openlittlehigh, LittleHigh_PeakHigh, LittleHigh_PeakLow);
	//printf("Open Little low: %d, average win: %f, average loss: %f, peakhigh: %f, peaklow: %f\n", openlittlelow, averagLittleLowLWin/openlittlelow, averagLittleLowLoss/openlittlelow, LittleLow_PeakHigh, LittleLow_PeakLow);
	
	printf("----------------------------\n");
	printf("No change days: %d\n", unchanged);
}