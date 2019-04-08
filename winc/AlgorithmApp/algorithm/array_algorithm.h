
#pragma once

#include "constants.h"

/*
Simple string search algorithm.  Use two loops to compare every char
*/
void string_search(const char a[], const char p[]);

void test_string_search();

/*
Find the most repeated number in a array.  
Return the repeat number * repeat count
*/
int MostRepeatedNumber(int intarray[], int len);

/*
Find first non-repeated char in an array
WARNNING: This is wrong implementation by using map.  Map is ordered by key
*/
char FirstNonRepeat(char* str, int len);

/*
Remove a list of characters from original char array
*/
void RemoveChars (char str [] , char remove []);

/*
Convert string to an int
*/
int StrToInt (char str[]);

/*
Convert int to a string
*/
void IntToStr(int num, char str[]);

/*
Find if int array contains any two ints adds up to sum
*/
bool FindSum(int intarr[], int size, int sum);

/*
Given an array of integers A[1...n], compute the array B[1...n] such that B[k] is the product of all the elements of A, except A[k]
*/
void ArrayProduct(int a[], int b[], int size);

void largestSum();
