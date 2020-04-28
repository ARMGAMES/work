#pragma once

#include "pch.h"
#include "sharedstructs.h"

using namespace std;

void testSorting();

// basic sorting
void selectionSort(vector<int>& a);
void insertionSort(vector<int>& a);

// Quicksort
//time: 2N*lgN comparison, NlgN average
// not stabel sort
void quickSort(vector<int>& a, int l, int r);
void quickSortNonRecursive(vector<int>& a, int l, int r);
void quickSortDupKeys(vector<int>& a, int l, int r);
int partition(vector<int>& a, int l, int r);

// Mergesort
// Time: NlgN guaranteed, stable sort
// Space: extra space proportional to N
void mergeAB(int c[], int a[], int N, int b[], int M);
// in-place merge, use aux array
void merge(vector<int>& a, int l, int m, int r);
void merge2(vector<int>& a, int l, int m, int r);

// top-down mergesort. time NlgN, space N
void mergeSort(vector<int>& a, int l, int r);
// bottom-up mergesort
void mergeSortBU(vector<int>& a, int l, int r);
// linked list merge
link mergeLinkedList(link a, link b);
link mergeSortLinkedList(link a);

// Heapsort
// Time: NlgN guaranteed, not stable sort

void fixUp(int a[], int k);
void fixDown(int a[], int k, int N);

class PQ
{
private:
	int * pq;
	int N;
public:
	PQ(int max)
	{
		pq = new int[max + 1];
		N = 0;
	}
	int empty() const
	{
		return N == 0;
	}
	void insert(int item)
	{
		pq[++N] = item; fixUp(pq, N);
	}
	int getmax()
	{
		exch(pq[1], pq[N]);
		fixDown(pq, 1, N - 1);
		return pq[N--];
	}
};

void PQSort(int a[], int l, int r);
void heapSort2(int a[], int l, int r);

void heapify(vector<int>& arr, int n, int i);
void heapSort(vector<int>& arr, int n);

void countSort(vector <int>& arr);

// A function to do counting sort of arr[] according to the digit represented by exp. 
void countSort(vector<int>& arr, int exp);

// The main function to that sorts arr[] of size n using Radix Sort 
void radixSort(vector<int>& arr);


