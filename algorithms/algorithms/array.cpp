#include "pch.h"
#include "array.h"

using namespace std;

int min(int a, int b, int c)
{
	int m = a;
	if (m > b)
		m = b;
	if (m > c)
		m = c;
	return m;
}

////////////////////////////////////////////////////////////////////
int findSum(int arr[], int n, int k, int sum)
{
	// n must be greater 
	if (n < k)
	{
		cout << "Invalid";
		return -1;
	}

	// Compute sum of first window of size k 
	int currSum = 0;
	for (int i = 0; i < k; i++)
		currSum += arr[i];

	if (currSum == sum)
		cout << "find sum, starting index=0";
	// Compute sums of remaining windows by  removing first element of previous 
	// window and adding last element of current window. 
	int window_sum = currSum;
	for (int i = k; i < n; i++)
	{
		window_sum += arr[i] - arr[i - k];
		if (window_sum == sum)
			cout << "find sum, starting index="<<i-k+1<<endl;
	}

	return 0;
}

void testSlidingWindow()
{
	cout << "testSlidingWindow\n";

	int arr[] = { 1, 4, 2, 10, 2, 3, 1, 10, 20 };
	int k = 4;
	int sum = 16;
	int n = sizeof(arr) / sizeof(arr[0]);
	findSum(arr, n, k, sum);
}

////////////////////////////////////////////////////////////////////

void printMaxSubSquare(int matrix[R][C])
{
	int i, j;
	int S[R][C];
	int max_of_s, max_i, max_j;

	/* Set first column of S[][]*/
	for (i = 0; i < R; i++)
		S[i][0] = matrix[i][0];

	/* Set first row of S[][]*/
	for (j = 0; j < C; j++)
		S[0][j] = matrix[0][j];

	/* Construct other entries of S[][]*/
	for (i = 1; i < R; i++)
	{
		for (j = 1; j < C; j++)
		{
			if (matrix[i][j] == 1)
				S[i][j] = min(S[i][j - 1], S[i - 1][j],
					S[i - 1][j - 1]) + 1;
			else
				S[i][j] = 0;
		}
	}

	/* Find the maximum entry, and indexes of maximum entry in S[][] */
	max_of_s = S[0][0]; max_i = 0; max_j = 0;
	for (i = 0; i < R; i++)
	{
		for (j = 0; j < C; j++)
		{
			if (max_of_s < S[i][j])
			{
				max_of_s = S[i][j];
				max_i = i;
				max_j = j;
			}
		}
	}

	printf("Maximum size sub-matrix is: \n");
	for (i = max_i; i > max_i - max_of_s; i--)
	{
		for (j = max_j; j > max_j - max_of_s; j--)
		{
			printf("%d ", matrix[i][j]);
		}
		printf("\n");
	}
}

int printMaxSubSquare(int M[R][C], int m, int n, int *maxv) {
	int res;
	if (m < 0 || n < 0) return 0;
	if (M[m][n] == 1) {
		res = 1 + min(printMaxSubSquare(M, m, n - 1, maxv), min(printMaxSubSquare(M, m - 1, n, maxv), printMaxSubSquare(M, m - 1, n - 1, maxv)));
	}
	else {

		res = min(printMaxSubSquare(M, m, n - 1, maxv), min(printMaxSubSquare(M, m - 1, n, maxv), printMaxSubSquare(M, m - 1, n - 1, maxv)));
	}
	*maxv = max(*maxv, res);
	return res;

}

void testMaxSqureMatrix()
{
	cout << "testMaxSqureMatrix\n";
	int m[R][C] = { {0, 1, 1, 0, 1},
				    {1, 1, 0, 1, 0},
				    {0, 1, 1, 1, 0},
					{1, 1, 1, 1, 0},
					{1, 1, 1, 1, 1},
					{0, 0, 0, 0, 0} };

	printMaxSubSquare(m);

	int maxv = 0;
	printMaxSubSquare(m, R - 1, C - 1, &maxv);
}

////////////////////////////////////////////////////////////////////

void testArray()
{
	testMaxSqureMatrix();
	testSlidingWindow();
}