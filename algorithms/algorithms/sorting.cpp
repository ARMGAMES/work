
#include "sorting.h"
#include "pch.h"


void exch(int& a, int& b)
{
	int temp = b;
	b = a;
	a = temp;
}

int partition(int a[], int l, int r)
{
	int i = l - 1;
	int j = r;
	int v = a[r];

	while (1)
	{
		while (a[++i] < v);
		while (a[--j] > v) if (j == l) break;
		if (i >= j) break;
		exch(a[i], a[j]);
	}

	exch(a[i], a[r]);
	return i;
}

void quickSort(int a[], int l, int r)
{
	if (l >= r) return;
	int i = partition(a, l, r);
	quickSort(a, l, i - 1);
	quickSort(a, i+1, r);
}

void quickSortNonRecursive(int a[], int l, int r)
{
	std::stack<int> s;
	s.push(r);
	s.push(l);

	while (!s.empty())
	{
		l = s.top();
		s.pop();
		r = s.top();
		s.pop();
		if (l >= r) continue;
		int i = partition(a, l, r);
		s.push(i - 1);
		s.push(l);
		s.push(r);
		s.push(i + 1);
	}
}

void quickSortDupKeys(int a[], int l, int r)
{
	int k; int v = a[r];
	if (l >= r) return;
	int i = l - 1, p = l - 1, j = r, q = r;
	while (1)
	{
		while (a[++i] < v);
		while (a[--j] > v) if (j == l) break;
		if (i >= j) break;
		exch(a[i], a[j]);
		if (a[i] == v) 
		{
			p++; exch(a[p], a[i]);
		}
		if (a[j] == v)
		{
			q--; exch(a[q], a[j]);
		}
	}
	exch(a[i], a[r]); j = i - 1; i = i + 1;
	for (k = l; k <= p; k++, j--) exch(a[k], a[j]);
	for (k = r - 1; k >= q; k--, i++) exch(a[k], a[i]);
	quickSortDupKeys(a, l, j);
	quickSortDupKeys(a, i, r);
}

void testSorting()
{
	std::cout << "Test sorting\n";

	int a[] = { 32, 29, 43, 3, 98, 17, 6 };

	//quickSort(a, 0, sizeof(a) / sizeof(int) - 1);

	quickSortNonRecursive(a, 0, sizeof(a) / sizeof(int) - 1);
}