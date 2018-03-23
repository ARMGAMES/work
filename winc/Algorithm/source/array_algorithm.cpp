
#include "stdafx.h"

using namespace std;

#define MAX_DIGITS_INT 10

int MostRepeatedNumber(int intarray[], int len)
{
	int sum = 0;
	map<int, int> arraycount;
	map<int, int>::iterator it;

	for (int i = 0; i < len; i++)
	{
		if (arraycount.find(intarray[i]) != arraycount.end())
		{
			arraycount[intarray[i]] = arraycount[intarray[i]]  + 1;
		}
		else
		{
			arraycount.insert(std::pair<int, int>(intarray[i], 1));
		}
	}

	int maxcount = 0;
	for (map<int, int>::iterator ii = arraycount.begin(); ii != arraycount.end(); ii++)
	{
		if (ii->second > maxcount)
		{
			maxcount = ii->second;
			it = ii;
		}
	}
	return it->first * it->second;
}

char FirstNonRepeat(char* str, int len)
{
	map<char, int> mymap;

	for (int i = 0; i < len; i++)
	{
		mymap[*(str+i)] = mymap[*(str+i)]  + 1;
	}

	for (map<char, int>::iterator it = mymap.begin(); it!=mymap.end(); it++)
	{
		if (it->second == 1)
		{
			return it->first;
		}
	}
	return 0;
}

void RemoveChars (char str [] , char remove []) 
{
	//	char* newstr = new char[strlen(str)];
	int dest = 0;
	bool found = false;
	for (int i = 0; i < strlen(str); i++)
	{
		for (int j = 0; j < strlen(remove); j++)
		{
			if ( str[i] == remove[j])
			{
				found = true;
				break;
			}
		}

		if (found)
		{
			found = false;
		}
		else
		{
			str[dest++] = str[i];
		}
	}
	str[dest] = 0;

}

int StrToInt (char str[])
{
	int result = 0, i = 0;
	bool neg = false;

	if (str[0] == '-')
	{
		neg = true;
		i = 1;
	}

	while( str[i])
	{
		result = result*10  + str[i++] - '0';
	}

	if (neg)
	{
		result = result * (-1);
	}

	return result;
}

void IntToStr(int num, char str[])
{
	int i = 0, j = 0;
	bool isneg = false;

	/* Buffer big enough for largest int, - sign and NUL */
	char temp[MAX_DIGITS_INT + 2];

	if (num < 0)
	{
		num = num *(-1);
		isneg = true;
	}

	/* Fill buffer with digit characters in reverse order */ 
	while (num) 
	{
		temp[i++] = (num % 10) + '0' ;
		num /= 10;
	}
		
	if (isneg)
			temp [i++] = '-' ;

	/* Reverse the characters */ 
	while (i > 0)
			str[j++] = temp[--i];
		
	/* NUL terminate the string */ 
	str[j] = '\0';
}


bool FindSum(int intarr[], int size, int sum)
{
	typedef set<int> int_set;
	int_set myset;

	for (int i = 0; i < size; i++)
	{
		myset.insert(intarr[i]);
	}

	for(int_set::iterator it = myset.begin(); it != myset.end(); it++)
	{
		int sec = sum - *it;
		if (myset.find(sec) != myset.end())
		{
			return true;
		}
	}

	return false;
}

void ArrayProduct(int a[], int b[], int size)
{
	int totalproduct = 1;
	for (int i = 0; i < size; i++)
	{
		if (i == 0)
		{
			b[i] = 1;
		}
		else
		{
			totalproduct *= a[i-1];
			b[i] = totalproduct;
		}
	}

	int reverstproduct = 1;
	for (int i = (size-1); i >=0; i--)
	{
		b[i] *= reverstproduct;	
		
		reverstproduct *= a[i];
	}

}

void string_search(const char a[], const char p[])
{
	for (int i = 0; i < strlen(a); i++)
	{
		int j;
		for (j = 0; j < strlen(p); j++)
			if (a[i+j] != p[j]) break;

		if (p[j] == 0)
			cout << "found match at index:" << i << endl;
	}

}

void largestSum()
{
	const int SIZE = 8;
	int a[SIZE]={-5, 3,-6,7,-4,12, -5, 8};
	int curr_sum = 0;
	int curr_start = 0;
	int curr_end = 0;

	int largest_sum = 0, largest_start = 0, largest_end = 0;

	for(int i = 0; i < SIZE; ++i)
	{
		if (curr_sum < 0)
		{
			curr_sum = a[i];
			curr_start = i;
		}
		else
		{
			curr_sum += a[i];
		}

		if (curr_sum > largest_sum)
		{
			largest_sum = curr_sum;
			largest_start = curr_start;
			largest_end = i;
		}
	}

	std::cout << largest_sum << " " << largest_start << " " << largest_end << std::endl;
	printf("end\n");
}