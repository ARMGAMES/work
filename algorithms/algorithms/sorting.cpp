
#include "sorting.h"
#include "pch.h"


void exch(int& a, int& b)
{
	int temp = b;
	b = a;
	a = temp;
}

int min(int a, int b)
{
	return (a < b) ? a : b;
}
//////////////////////////////////////////////////////////////
// Quicksort

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

//////////////////////////////////////////////////////////
// mergesort

void mergeAB(int c[], int a[], int N, int b[], int M)
{
	for (int i = 0, j = 0, k = 0; k < N+M; ++k)
	{
		if (i == N)
		{
			c[k] = b[j++];
			continue;
		}
		if (j == M)
		{
			c[k] = a[i++];
			continue;
		}
		c[k] = a[i] < b[j] ? a[i++] : b[j++];
	}
}

// in-place merge
void merge(int a[], int l, int m, int r)
{
	int i, j;
	static int aux[maxN];
	for (i = l; i <= m; ++i)
		aux[i] = a[i];
	for (j = m; j <= r; ++j)
		aux[r + m - j] = a[j];
	i = l; j = r;
	for (int k = l; k <= r; ++k)
	{
		if (aux[j] < aux[i])
			a[k] = aux[j--];
		else
			a[k] = aux[i++];
	}
}

void mergeSort(int a[], int l, int r)
{
	if (l >= r) return;
	int m = (l + r) / 2;
	mergeSort(a, l, m);
	mergeSort(a, m + 1, r);
	merge(a, l, m, r);
}

void mergeSortBU(int a[], int l, int r)
{
	for (int m = 1; m <= r - l; m = m + m)
		for (int i = l; i <= r - m; i += m + m)
			merge(a, i, i + m - 1, min(i + m + m - 1, r));
}

link mergeLinkedList(link a, link b)
{
	node dummy(0); link head = &dummy, c = head;
	while ((a != 0) && (b != 0))
	{
		if (a->value < b->value)
		{
			c->next = a; c = a; a = a->next;
		}
		else
		{
			c->next = b; c = b; b = b->next;
		}
	}
	c->next = (a == 0) ? b : a;
	return head->next;
}

link mergeSortLinkedList(link c)
{
	if (c == 0 || c->next == 0) return c;
	link a = c, b = c->next;
	while ((b != 0) && (b->next != 0))
	{
		c = c->next; b = b->next->next;
	}
	b = c->next; c->next = 0;
	return mergeLinkedList(mergeSortLinkedList(a), mergeSortLinkedList(b));
}

/////////////////////////////////////////////////////////
// Priority Queue
void fixUp(int a[], int k)
{
	while (k > 1 && a[k / 2] < a[k])
	{
		exch(a[k], a[k / 2]); k = k / 2;
	}
}

void fixDown(int a[], int k, int N)
{
	while (2 * k <= N)
	{
		int j = 2 * k;
		if (j < N && a[j] < a[j + 1]) j++;
		if (!(a[k] < a[j])) break;
		exch(a[k], a[j]); k = j;
	}
}

void PQSort(int a[], int l, int r)
{
	int k;
	PQ pq(r - l + 1);
	for (k = l; k <= r; k++) pq.insert(a[k]);
	for (k = r; k >= l; k--) a[k] = pq.getmax();
}

void heapSort(int a[], int l, int r)
{
	int k, N = r - l + 1;
	int *pq = a + l - 1;
	for (k = N / 2; k >= 1; k--)
		fixDown(pq, k, N);
	while (N > 1)
	{
		exch(pq[1], pq[N]);
		fixDown(pq, 1, --N);
	}
}

/////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////

void testSorting()
{
	std::cout << "Test sorting\n";

	int a[] = { 32, 29, 43, 3, 98, 17, 6 };

	//quickSort(a, 0, sizeof(a) / sizeof(int) - 1);

	quickSortNonRecursive(a, 0, sizeof(a) / sizeof(int) - 1);
}