#pragma once

void testSorting();

// 
void exch(int& a, int& b);

// Quicksort
void quickSort(int a[], int l, int r);
void quickSortNonRecursive(int a[], int l, int r);
void quickSortDupKeys(int a[], int l, int r);
int partition(int a[], int l, int r);


