#pragma once

#define FILENAME "sp500.csv"

const char kSeparator = ',';

using namespace std;

struct MarketData
{
  string date;
  float open;
  float high;
  float low;
  float close;
  double volume;
  float adj_close;
};

// put it here temperarily 
inline void splitbychar(string s, char separator, vector<string>& tokens)
{
  stringstream ss(s);
  string item;
  while(getline(ss, item, separator)) 
  {
    tokens.push_back(item);
  }  
}