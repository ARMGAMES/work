#pragma once

using namespace std;


int min(int a, int b, int c);

////////////////////////////////////////////////////////////////////
// sliding window technique
// Returns maximum sum in a subarray of size k. 
int findSum(int arr[], int n, int k, int sum);

void testSlidingWindow();

////////////////////////////////////////////////////////////////////

#define R 6 
#define C 5 
void printMaxSubSquare(int matrix[R][C]);

void testMaxSqureMatrix();

////////////////////////////////////////////////////////////////////
void testArray();