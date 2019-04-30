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

vector<vector<int> > threeSum(vector<int> &num, int sum) {

	vector<vector<int> > res;

	std::sort(num.begin(), num.end());

	for (int i = 0; i < num.size() - 2; i++) {

		int target = sum - num[i];
		int front = i + 1;
		int back = num.size() - 1;

		while (front < back) {

			int sum = num[front] + num[back];

			// Finding answer which start from number num[i]
			if (sum < target)
				front++;

			else if (sum > target)
				back--;

			else {
				vector<int> triplet(3, 0);
				triplet[0] = num[i];
				triplet[1] = num[front];
				triplet[2] = num[back];
				res.push_back(triplet);

				// Processing duplicates of Number 2
				// Rolling the front pointer to the next different number forwards
				while (front < back && num[front] == triplet[1]) front++;

				// Processing duplicates of Number 3
				// Rolling the back pointer to the next different number backwards
				while (front < back && num[back] == triplet[2]) back--;
			}
		}
		// Processing duplicates of Number 1
		while (i < num.size() && i + 1 < num.size() && num[i + 1] == num[i])
			i++;
	}
	return res;
}

void test3Sum()
{
	vector<int> array = { 4, 5, 2, 8, 6, 11, 7, 4 };

	vector<vector<int> > resul = threeSum(array, 16);
}

////////////////////////////////////////////////////////////////////

int gcd(int a, int b)
{
	if (b == 0)
		return a;

	else
		return gcd(b, a % b);
}

void leftRotate(int arr[], int d, int n)
{
	int gcdNum = gcd(d, n);
	for (int i = 0; i < gcdNum; i++) {
		/* move i-th values of blocks */
		int temp = arr[i];
		int j = i;

		while (1) {
			int k = j + d;
			if (k >= n)
				k = k - n;

			if (k == i)
				break;

			arr[j] = arr[k];
			j = k;
		}
		arr[j] = temp;
	}
}

void testJungleAlgo()
{
	cout << "testJungleAlgo\n";

	int arr[] = { 1, 2, 3, 4, 5, 6 };
	int n = sizeof(arr) / sizeof(arr[0]);

	// Function calling 
	leftRotate(arr, 2, n);
}

////////////////////////////////////////////////////////////////////

void mergePair(vector<pair<int, int>>& res, const pair<int, int>& toMerge)
{
	if (res.empty() || toMerge.first > res.back().second) {
		res.push_back(toMerge);
	}
	else {
		res.back().first = min(res.back().first, toMerge.first);
		res.back().second = max(res.back().second, toMerge.second);
	}
}

void mergeIntervals(const vector<pair<int, int> > &l1, const vector<pair<int, int> > &l2, vector<pair<int, int>>& result)
{
	for (int i = 0, j = 0; i < l1.size() || j < l2.size();) {
		if (i == l1.size()) {
			mergePair(result, l2[j++]);
		}
		else if (j == l2.size()) {
			mergePair(result, l1[i++]);
		}
		else {
			if (l1[i].second < l2[j].first) {
				mergePair(result, l1[i++]);
			}
			else if (l2[j].second < l1[i].first) {
				mergePair(result, l2[j++]);
			}
			else {
				mergePair(result, { min(l1[i].first,l2[j].first),max(l1[i].second,l2[j].second) });
				i++;
				j++;
			}
		}
	}
}

void testMergeIntervals()
{
	vector<pair<int, int>> l1;
	l1.push_back(make_pair(1, 2));
	l1.push_back(make_pair(3, 9));

	vector<pair<int, int>> l2;
	l2.push_back(make_pair(4, 5));
	l2.push_back(make_pair(8, 10));
	l2.push_back(make_pair(11, 12));

	vector<pair<int, int>> result;
	mergeIntervals(l1, l2, result);
}

/////////////////////////////////////////////////////////////////////

void displayMatrix(vector<vector<int>> mat)
{
	for (int i = 0; i < mat.size(); i++)
	{
		for (auto val : mat[i])
			printf("%2d ", val);
		printf("\n");
	}
	printf("\n");
}

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

// A function to check if a given cell (row, col) can be included in DFS 
int isSafe(int M[][COL], int row, int col, bool visited[][COL])
{
	// row number is in range, column number is in range and value is 1  
	// and not yet visited 
	return (row >= 0) && (row < ROW) &&
		(col >= 0) && (col < COL) &&
		(M[row][col] && !visited[row][col]);
}

// A utility function to do DFS for a 2D boolean matrix. It only considers 
// the 8 neighbours as adjacent vertices 
void DFS(int M[][COL], int row, int col, bool visited[][COL])
{
	// These arrays are used to get row and column numbers of 8 neighbours  
	// of a given cell 
	static int rowNbr[] = { -1, -1, -1,  0, 0,  1, 1, 1 };
	static int colNbr[] = { -1,  0,  1, -1, 1, -1, 0, 1 };

	// Mark this cell as visited 
	visited[row][col] = true;

	// Recur for all connected neighbours 
	for (int k = 0; k < 8; ++k)
		if (isSafe(M, row + rowNbr[k], col + colNbr[k], visited))
			DFS(M, row + rowNbr[k], col + colNbr[k], visited);
}

int countIslands(int M[][COL])
{
	// Make a bool array to mark visited cells. 
	// Initially all cells are unvisited 
	bool visited[ROW][COL];
	memset(visited, 0, sizeof(visited));

	// Initialize count as 0 and travese through the all cells of 
	// given matrix 
	int count = 0;
	for (int i = 0; i < ROW; ++i)
		for (int j = 0; j < COL; ++j)
			if (M[i][j] && !visited[i][j]) // If a cell with value 1 is not 
			{                         // visited yet, then new island found 
				DFS(M, i, j, visited);     // Visit all cells in this island. 
				++count;                   // and increment island count 
			}

	return count;
}

void testIslands()
{
	int M[][COL] = { {1, 1, 0, 0, 0},
		{0, 1, 0, 0, 1},
		{1, 0, 0, 1, 1},
		{0, 0, 0, 0, 0},
		{1, 0, 1, 0, 1}
	};

	printf("Number of islands is: %d\n", countIslands(M));
}

////////////////////////////////////////////////////////////////////

void rotateMatrix(vector<vector<int>>& mat)
{
	int N = mat.size();
	// Consider all squares one by one 
	for (int x = 0; x < N / 2; x++)
	{
		// Consider elements in group of 4 in current square 
		for (int y = x; y < N - x - 1; y++)
		{
			// store current cell in temp variable 
			int temp = mat[x][y];

			// move values from right to top 
			mat[x][y] = mat[y][N - 1 - x];

			// move values from bottom to right 
			mat[y][N - 1 - x] = mat[N - 1 - x][N - 1 - y];

			// move values from left to bottom 
			mat[N - 1 - x][N - 1 - y] = mat[N - 1 - y][x];

			// assign temp to left 
			mat[N - 1 - y][x] = temp;
		}
	}
}

void testRotateMatrix()
{
	// Test Case 1 
	vector<vector<int>> mat =
	{
		{1, 2, 3, 4},
		{5, 6, 7, 8},
		{9, 10, 11, 12},
		{13, 14, 15, 16}
	};

	rotateMatrix(mat);

	// Print rotated matrix 
	displayMatrix(mat);
}

////////////////////////////////////////////////////////////////////
void testArray()
{
	testSlidingWindow();
	test3Sum();
	testJungleAlgo();
	testMergeIntervals();
	testMaxSqureMatrix();
	testIslands();
	testRotateMatrix();
}