#pragma once

using namespace std;


int min(int a, int b, int c);

////////////////////////////////////////////////////////////////////
// Array
// sliding window technique
// Returns maximum sum in a subarray of size k. 
int findSum(int arr[], int n, int k, int sum);

//THREE sum
vector<vector<int> > threeSum(vector<int> &num, int sum);

//A Juggling Algorithm - rotate array
/*Fuction to get gcd of a and b*/
int gcd(int a, int b);
/*Function to left rotate arr[] of siz n by d*/
void leftRotate(int arr[], int d, int n);


////////////////////////////////////////////////////////////////////
// merge sorted interval
void mergePair(vector<pair<int, int> > &res, const pair<int, int> &toMerge);
void mergeIntervals(const vector<pair<int, int> > &l1, const vector<pair<int, int> > &l2, vector<pair<int, int>>& result);

void testMergeIntervals();

////////////////////////////////////////////////////////////////////

class MyCircularQueue {
	vector<int> q;
	int head, tail, N;
	bool empty;
public:
	/** Initialize your data structure here. Set the size of the queue to be k. */
	MyCircularQueue(int k) {
		head = 0;
		tail = 0;
		N = k;
		q = vector<int>(k);
		empty = true;
	}

	/** Insert an element into the circular queue. Return true if the operation is successful. */
	bool enQueue(int value) {
		if (isFull()) return false;
		empty = false;
		q[tail] = value;
		tail = (tail + 1) % N;
		return true;
	}

	/** Delete an element from the circular queue. Return true if the operation is successful. */
	bool deQueue() {
		if (isEmpty()) return false;
		q[head] = 0;
		head = (head + 1) % N;
		if (head == tail) empty = true;
		return true;
	}

	/** Get the front item from the queue. */
	int Front() {
		if (isEmpty()) return -1;
		return q[head];
	}

	/** Get the last item from the queue. */
	int Rear() {
		if (isEmpty()) return -1;
		return q[(tail - 1 + N) % N];
	}

	/** Checks whether the circular queue is empty or not. */
	bool isEmpty() {
		return empty;
	}

	/** Checks whether the circular queue is full or not. */
	bool isFull() {
		return (empty) ? false : head == tail;
	}
};

////////////////////////////////////////////////////////////////////
// MATRIX

// Function to print the matrix 
void displayMatrix(vector<vector<int>> mat);

#define R 6 
#define C 5 
void printMaxSubSquare(int matrix[R][C]);
void testMaxSqureMatrix();

// Find number of island
// DFS
#define ROW 5
#define COL 5
int isSafe(int M[][COL], int row, int col, bool visited[][COL]);
void DFS(int M[][COL], int row, int col, bool visited[][COL]);
int countIslands(int M[][COL]);

// An Inplace function to rotate a N x N matrix 
// by 90 degrees in anti-clockwise direction  
void rotateMatrix(vector<vector<int>>& mat);

////////////////////////////////////////////////////////////////////


void testArray();