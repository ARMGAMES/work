
#include "..\stdafx.h"
#include "leetcodetest.h"

using namespace std;

int lengthOfLongestSubstring(const char* str)
{
	int longestCount = 0, maxLongest = 0;
	unordered_set<char> strSet;

	while (*str)
	{
		if (strSet.find(*str) != strSet.end())
		{
			if (longestCount > maxLongest)
			{
				maxLongest = longestCount;
			}
			longestCount = 0;
			strSet.clear();
		}
		longestCount++;
		strSet.insert(*str);
		str++;
	}

	if (longestCount > maxLongest)
	{
		maxLongest = longestCount;
	}

	return maxLongest;
}

int reverseInt(int x)
{
	int result = 0;
	bool negative = false;
	if (x < 0)
	{
		negative = true;
		x = x*-1;
	}
	while (x > 0)
	{
		int remainder = x%10;
		x = x/10;
		result = result*10 + remainder;
	}

	if (negative)
	{
		result = result * -1;
	}

	return result;
}

const static char decodeTable[27] = {'*', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 
										  'h', 'i', 'j', 'k', 'l', 'm', 'n',
										  'o', 'p', 'q', 'r', 's', 't',
										  'u', 'v', 'w', 'x', 'y', 'z'};

void decodeNum(vector<string>& result, const char* p, string decodestr)
{
	if (!*p)
	{
		result.push_back(decodestr);
		return;
	}

	int first = *p - '0';
	if (first < 1 || first > 9)
		return;
	else if ((first > 2) || !*(p+1))
		decodeNum(result, ++p, decodestr + decodeTable[first]);
	else
	{
		int second = *(p+1) - '0';
		int index = first * 10 + second;
		if (second == 0)
		{
			decodeNum(result, p+2, decodestr + decodeTable[index]);
		}
		else if (index > 26)
		{
			decodeNum(result, ++p, decodestr + decodeTable[first]);
		}
		else
		{
			decodeNum(result, ++p, decodestr + decodeTable[first]);
			decodeNum(result, p+2, decodestr + decodeTable[index]);
		}
	}
}



void leetCodeTest()
{
	printf("leet code test ");

	//printf("lengthOfLongestSubstring=%d", lengthOfLongestSubstring("abcabcdadf"));

	//int result = reverseInt(-1234);
	vector<string> result;
	decodeNum(result, "126243218", "");
}