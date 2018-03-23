#pragma once

#include "stdafx.h"

using namespace std;

class Analysis
{
public:
  Analysis()
  {
  }
  ~Analysis(){}

  bool init(string file_name);

  void analyzeOpenClose();
  void analyzeContinous();
  void analyzeLargeChangeTrading(int lastDays, float enterPert, float stopPert);
  void analyzeLittleChangeTrading(int lastDays, float enterPert, float limitPert);

private:
  vector<MarketData> market_data;
  float offset;

};