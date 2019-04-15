#pragma once

#include "sharedstructs.h"

void testSorting();

// Quicksort
//time: 2N*lgN comparison, NlgN average
// not stabel sort
void quickSort(int a[], int l, int r);
void quickSortNonRecursive(int a[], int l, int r);
void quickSortDupKeys(int a[], int l, int r);
int partition(int a[], int l, int r);

// Mergesort
// Time: NlgN guaranteed, stable sort
// Space: extra space proportional to N
void mergeAB(int c[], int a[], int N, int b[], int M);
// in-place merge, use aux array
void merge(int a[], int l, int m, int r);
// top-down mergesort. time NlgN, space N
void mergeSort(int a[], int l, int r);
// bottom-up mergesort
void mergeSortBU(int a[], int l, int r);
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
void heapSort(int a[], int l, int r);