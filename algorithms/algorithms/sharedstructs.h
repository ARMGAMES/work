#pragma once
#include "pch.h"

using namespace std;

const int maxN = 100;
const int bitsword = 32;
const int bitsbyte = 8;
const int bytesword = bitsword / bitsbyte;
const int R = 1 << bitsbyte;
const int NULLdigit = -1;

// int =  0 1 2 3 bytes
inline int digit(int A, int b)
{
	return (A >> bitsbyte * (bytesword - b - 1) & (R - 1));
}

inline int digit(char* A, int b)
{
	return A[b];
}

inline int min(int a, int b)
{
	return (a < b) ? a : b;
}

inline int max(int a, int b)
{
	return (a > b) ? a : b;
}

inline void exch(int& a, int& b)
{
	int temp = b;
	b = a;
	a = temp;
}

inline int hash1(int value, int M)
{
	int h = value % M;
	return (h < 0) ? (h + M) : h;
}

inline int hash1(char *v, int M)
{
	int h = 0, a = 127;
	for (; *v != 0; v++)
		h = (a*h + *v) % M;
	return h;
}

inline int hashU(char *v, int M)
{
	int h, a = 31415, b = 27183;
	for (h = 0; *v != 0; v++, a = a * b % (M - 1))
		h = (a*h + *v) % M;
	return (h < 0) ? (h + M) : h;
}


struct node
{
	int value;
	node *next;
	node(int v)
	{
		value = v;
		next = 0;
	}
	node(int v, node* next_)
	{
		value = v;
		next = next_;
	}
};
typedef node* link;

/* Standard Binary Search function*/
int binarySearch(const vector<int>& arr, int low, int high, int key);
