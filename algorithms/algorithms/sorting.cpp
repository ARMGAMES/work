
#include "pch.h"
#include "sorting.h"

using namespace std;


//////////////////////////////////////////////////////////////
void selectionSort(vector<int>& a)
{
	int n = a.size();
	for (int i = 0; i < n - 1; ++i)
	{
		int min = i;
		for (int j = i + 1; j < n; ++j)
		{
			if (a[min] > a[j])
				min = j;
		}
		exch(a[i], a[min]);
	}
}

/* Function to sort an array using insertion sort*/
void insertionSort(vector<int>& arr)
{
	int n = arr.size();

	int i, key, j;
	for (i = 1; i < n; i++)
	{
		key = arr[i];
		j = i - 1;

		/* Move elements of arr[0..i-1], that are
		greater than key, to one position ahead
		of their current position */
		while (j >= 0 && arr[j] > key)
		{
			arr[j + 1] = arr[j];
			j = j - 1;
		}
		arr[j + 1] = key;
	}
}

//////////////////////////////////////////////////////////////
// Quicksort

int partition(vector<int>& a, int l, int r)
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

void quickSort(vector<int>& a, int l, int r)
{
	if (l >= r) return;
	int i = partition(a, l, r);
	quickSort(a, l, i - 1);
	quickSort(a, i+1, r);
}

void quickSortNonRecursive(vector<int>& a, int l, int r)
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

void quickSortDupKeys(vector<int>& a, int l, int r)
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
void merge(vector<int>& a, int l, int m, int r)
{
	int i, j;
	static int aux[maxN];
	for (i = l; i <= m; ++i) 
		aux[i] = a[i];
	for (j = m + 1; j <= r; j++) 
		aux[r + m + 1 - j] = a[j];

	i = l;
	j = r;

	for (int k = l; k <= r; ++k)
	{
		if (aux[j] < aux[i])
			a[k] = aux[j--]; 
		else 
			a[k] = aux[i++];
	}
}

void merge2(vector<int>& a, int l, int m, int r)
{
	vector<int> ln;
	vector<int> rn;

	for (int i = l; i <= m; ++i)
	{
		ln.push_back(a[i]);
	}
	for (int i = m+1; i <= r; ++i)
	{
		rn.push_back(a[i]);
	}

	int i = 0;
	int j = 0;
	int k = l;
	while (i < (int)ln.size() && j < (int)rn.size())
	{
		if (ln[i] <= rn[j])
		{
			a[k] = ln[i++];
		}
		else
		{
			a[k] = rn[j++];
		}
		k++;
	}

	while (i < (int)ln.size())
	{
		a[k++] = ln[i++];
	}

	while (j < (int)rn.size())
	{
		a[k++] = rn[j++];
	}
}

void mergeSort(vector<int>& a, int l, int r)
{
	if (l >= r) return;
	int m = (l + r) / 2;
	mergeSort(a, l, m);
	mergeSort(a, m + 1, r);
	merge(a, l, m, r);
}

void mergeSortBU(vector<int>& a, int l, int r)
{
	for (int m = 1; m <= r - l; m = m + m)
		for (int i = l; i <= r - m; i += m + m)
			merge2(a, i, i + m - 1, min(i + m + m - 1, r));
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

void heapSort2(int a[], int l, int r)
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
void heapify(vector<int>& arr, int n, int i)
{
	int largest = i; // Initialize largest as root 
	int l = 2 * i + 1; // left = 2*i + 1 
	int r = 2 * i + 2; // right = 2*i + 2 

	// If left child is larger than root 
	if (l < n && arr[l] > arr[largest])
		largest = l;

	// If right child is larger than largest so far 
	if (r < n && arr[r] > arr[largest])
		largest = r;

	// If largest is not root 
	if (largest != i)
	{
		swap(arr[i], arr[largest]);

		// Recursively heapify the affected sub-tree 
		heapify(arr, n, largest);
	}
}

void heapSort(vector<int>& arr, int n)
{
	// Build heap (rearrange array) 
	for (int i = n / 2 - 1; i >= 0; i--)
		heapify(arr, n, i);

	// One by one extract an element from heap 
	for (int i = n - 1; i > 0; i--)
	{
		// Move current root to end 
		swap(arr[0], arr[i]);

		// call max heapify on the reduced heap 
		heapify(arr, i, 0);
	}
}

/////////////////////////////////////////////////////////

void countSort(vector <int>& arr)
{
	static int MIN_RANGE = 0;
	static int MAX_RANGE = 20;

	int range = MAX_RANGE - MIN_RANGE + 1;

	vector<int> count(range, 0);
	vector<int> output(arr.size());

	for (int i = 0; i < arr.size(); ++i)
	{
		count[arr[i] - MIN_RANGE]++;
	}

	for (int i = 1; i < count.size(); ++i)
	{
		count[i] += count[i - 1];
	}

	for (int i = 0; i < arr.size(); ++i)
	{
		output[count[arr[i] - MIN_RANGE] - 1] = arr[i];
		count[arr[i] - MIN_RANGE]--;
	}

	for (int i = 0; i < arr.size(); i++)
		arr[i] = output[i];
}

void countSort(vector<int>& arr, int exp)
{
	int n = arr.size();
	vector<int> output(n); // output array 
	int i, count[10] = { 0 };

	// Store count of occurrences in count[] 
	for (i = 0; i < n; i++)
		count[(arr[i] / exp) % 10]++;

	// Change count[i] so that count[i] now contains actual 
	//  position of this digit in output[] 
	for (i = 1; i < 10; i++)
		count[i] += count[i - 1];

	// Build the output array 
	for (i = n - 1; i >= 0; i--)
	{
		output[count[(arr[i] / exp) % 10] - 1] = arr[i];
		count[(arr[i] / exp) % 10]--;
	}

	// Copy the output array to arr[], so that arr[] now 
	// contains sorted numbers according to current digit 
	for (i = 0; i < n; i++)
		arr[i] = output[i];
}

void radixSort(vector<int>& arr)
{
	// Find the maximum number to know number of digits 
	int m = *std::max_element(arr.begin(), arr.end());

	// Do counting sort for every digit. Note that instead 
	// of passing digit number, exp is passed. exp is 10^i 
	// where i is current digit number 
	for (int exp = 1; m / exp > 0; exp *= 10)
	{
		countSort(arr, exp);
	}
}


/////////////////////////////////////////////////////////
void testSorting()
{
	std::cout << "Test sorting\n";

	vector<int> a = { 32, 29, 43, 3, 98, 17, 6 };
	selectionSort(a);

	vector<int> b = { 32, 29, 43, 3, 98, 17, 6 };
	mergeSort(b, 0, b.size() - 1);

	vector<int> c = { 32, 29, 43, 3, 98, 17, 6 };
	mergeSortBU(c, 0, b.size() - 1);

	vector<int> d = { 32, 29, 43, 3, 98, 17, 6 };
	quickSort(d, 0, b.size() - 1);

	vector<int> e = { 32, 29, 43, 3, 98, 17, 6 };
	quickSortNonRecursive(e, 0, b.size() - 1);

	vector<int> f = { 32, 29, 43, 3, 98, 17, 6 };
	heapSort(f, f.size());

	vector<int> g = { 3, 1, 4, 6, 4, 6, 10 };
	countSort(g);

	vector<int> h = { 170, 45, 75, 90, 802, 24, 2, 66 };
	radixSort(h);

	std::sort(h.begin(), h.end(), greater<int>());
	std::cout << "Test sorting completed\n";
}

