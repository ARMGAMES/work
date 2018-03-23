
#ifndef SORTING_H 
#define SORTING_H

#include "stdafx.h"

const int bitsbyte = 8;
const int bitsword = 32;
const int bytesword = bitsword/bitsbyte;
const int R = 1 << bitsbyte;
const int X=0xff;

inline int digit(long  A, int B)
{
	return (A >> bitsbyte*(bytesword - B - 1)) & (R-1);
}

inline int digit(int A, int B)
{
  return (A >> B) & 1;
}

template<class Item>
void exch(Item &A, Item &B)
{
	Item t = A; A = B; B = t;
}

template<class Item>
void compexch(Item &A, Item &B)
{
	if (B < A)
		exch (A, B);
}

/*
Selection sort. Each loop finds the min number and move to left
Comparison N*N/2, exchange N
*/
template<class Item>
void selection_sort(Item a[], int left, int right)
{
	for(int i = left; i < right; i++)
	{
		int min = i;
		for (int j = i+1; j <= right; j++)
		{
			if (a[min] > a[j]) 
				min = j;
		}
		exch(a[i], a[min]);
	}
}

/*
Insertion sort. exchange base
Comparison N*N/4, exchange N*N/4
*/
template<class Item>
void insertion_sort_exch(Item a[], int left, int right)
{
  for (int i = left+1; i<= right; i++)
  {
    Item temp = a[i];
    int j = i;
    for (; j > left; j--)
    {
      if(a[j-1] <= temp)
        break;
      a[j] = a[j-1];
    }
    a[j] = temp;
  }
}
/*
Insertion sort. no exchange
Comparison N*N/4, exchange N*N/4
*/
template<class Item>
void insertion_sort(Item a[], int left, int right)
{
  // find min number and put it in front as sentinel
  int i;
  for (i = right; i > left; i--) compexch(a[i-1], a[i]);

  for (int i = left+1; i<= right; i++)
  {
    int j = i; Item temp = a[i];
    while(temp < a[j-1])
    {
      a[j] = a[j-1]; j--;
    }
    a[j] = temp;
  }
}

/*
Bubble sort: twice slower than insertion and selection sort generally
Comparison N*N/2, exchange N*N/2
*/
template<class Item>
void bubble_sort(Item a[], int left, int right)
{
  for (int i = left; i < right; i++)
  {
    for (int j = right; j > i; j--)
    {
      compexch(a[j-1], a[j]);
    }
  }
}

/*
Shell sort, modified based on insertion sort
*/
template <class Item>
void shell_sort(Item a[], int l, int r)
{ 
  int h;
  for (h = 1; h <= (r-l)/9; h = 3*h+1) 
    ;

  for ( ; h > 0; h /= 3)
  {
    for (int i = l+h; i <= r; i++)
    { 
      int j = i; 
      Item v = a[i];
      while (j >= l+h && v < a[j-h])
      { 
        a[j] = a[j-h]; 
        j -= h; 
      }
      a[j] = v;
    }
  }
}


/*
partition to move duplicate keys in middle
*/
template <class Item>
int partition_dk(Item a[], int left, int right)
{
  if (right <= left) return;

  int temp = a[right];
  int i = left - 1, j = right, p = left-1, q = right;
  for(;;)
  {
    while (a[++i] < temp);
    while (a[--j] > temp)
    {
      if (j == 1) break;
    }
    if (i >= j) break;
    exch(a[i], a[j]);
    if (a[i] == temp) { exch(a[++p],a[i]); }
    if (temp == a[j]) { exch(a[--q],a[j]); }
  }
  exch(a[i], a[right]);
  j = i - 1; i = i + 1;
  for (int k = left ; k <= p; k++, j--) exch(a[k],a[j]);
  for (int k = right-1; k >= q; k--, i++) exch(a[k],a[i]);

  partition_dk(a, left, j);
  partition_dk(a, i, right);

}


/*

*/
template <class Item>
int partition(Item a[], int left, int right)
{
  int temp = a[right];
  int i = left - 1, j = right;
  for(;;)
  {
    while (a[++i] < temp);
    while (a[--j] > temp)
    {
      if (j == 1)
        break;
    }
    if (i >= j)
      break;
    exch(a[i], a[j]);
  }
  exch(a[i], a[right]);
  return i;
}


/*
Quick sort.  Resursive impl, split to two sub arrays
Best case - always partition arrays in middle.
Search time NlgN to N*N/2
*/
template <class Item>
void quicksort(Item a[], int left, int right)
{
  if (left >= right) return;
  int i = partition(a, left, right);
  quicksort(a, left, i-1);
  quicksort(a, i, right);
}

/*
Quick sort implemented with stack. Sort smaller sub arrays first to make sure stack
size is no more than NlgN
*/
template <class Item>
void quicksort_stack(Item a[], int left, int right)
{
  int size = right - left;

  std::stack<int> s;
  s.push(right);
  s.push(left);

  while (!s.empty())
  {
    int l = s.top(); s.pop();
    int r = s.top(); s.pop();

    if (l >= r) continue;
    int i = partition(a, l, r);

    if (i-l > r-i)
    {
      // push left sub list
      s.push(i-1); s.push(l);
      //push right sub list.  Right sub lists is smaller, we want to sort it first
      s.push(r); s.push(i);
    }
    else
    {
      //push right sub list
      s.push(r); s.push(i);
      // push left sub list. Left sub lists is smaller, we want to sort it first
      s.push(i-1); s.push(l);
    }
  }
}




template <class Item>
void mergesortAB(Item a[], int l, int r)
{ 
  static Item aux[SORT_MAX];
  for (int i = l; i <= r; i++) aux[i] = a[i];
  mergesortABr(a, aux, l, r);
}
/*
Merge sore with cutoff and no copy
*/
template <class Item>
void mergesortABr(Item a[], Item b[], int l, int r)
{ 
  //if (r-l <= 10) { insertion_sort(a, l, r); return; }
  if (left >= right) return;

  int m = (l+r)/2;
  mergesortABr(b, a, l, m);
  mergesortABr(b, a, m+1, r);
  mergeAB(a+l, b+l, m-l+1, b+m+1, r-m);
}
template <class Item>
void mergeAB(Item c[], Item a[], int N,
             Item b[], int M )
{
  for (int i = 0, j = 0, k = 0; k < N+M; k++)
  {
    if (i == N) { c[k] = b[j++]; continue; }
    if (j == M) { c[k] = a[i++]; continue; }
    c[k] = (a[i] < b[j]) ? a[i++] : b[j++];
  }
}


/**
Mergesort requires about N lg N comparisons to sort any file of N elements
Mergesort uses extra space proportional to N
*/
template <class Item>
void mergesort(Item a[], int left, int right)
{
  if (left >= right) return;
  int m = (right + left)/2;
  mergesort(a, left, m);
  mergesort(a, m+1, right);
  merge(a, left, m, right);
}

/*
Merge takes extra space
Mergesort is stable, if the merge is stable
*/
template <class Item>
void merge(Item a[], int left, int m, int right)
{
  int i, j;
  static Item aux[SORT_MAX];
  // copy 
  for (i = m+1; i > left; i--) 
    aux[i-1] = a[i-1];

  for (j = m; j < right; j++) 
    aux[right+m-j] = a[j+1]; 

  for (int k = left; k <= right; k++)
  {
    if (aux[j] < aux[i])
      a[k] = aux[j--]; 
    else 
      a[k] = aux[i++];
  }
}

/*
Priority queue sort- insert into priority queue, and get max from pq
*/
template <class Item>
void PQsort(Item a[], int l, int r)
{ 
  int k;
  PQ<Item> pq(r-l+1);
  // N * LgN
  for (k = l; k <= r; k++) 
    pq.insert(a[k]);
  
  // 2N * LgN
  for (k = r; k >= l; k--) 
    a[k] = pq.getmax();
}

template<class Item>
void fixDown(Item a[], int k, int N)
{
  while (2*k <= N)
  { 
    int j = 2*k;
    if (j < N && a[j] < a[j+1]) 
      j++;
    if (!(a[k] < a[j])) 
      break;
    exch(a[k], a[j]); 
    k = j;
  }
}

template<class Item>
void heapsort(Item a[], int l, int r)
{
  // eg, l = 0 r = 8, N = 9. this equal from 1 to 9
  int N = r - l + 1, k;
  Item *pq = a+l-1;
  for (k = N/2; k>=1; k++)
  {
    fixDown(pq, k, N);    
  }
  while (N > 1)
  {
    exch(pq[1], pq[N]);
    fixDown(pq, 1, --N);
  }
}

/*
Binary quick sort
*/
template <class Item>
void quicksortB(Item a[], int l, int r, int d)
{ 
  int i = l, j = r;
  if (r <= l || d > bitsword) 
    return;

  while (j != i)
  {
    while (digit(a[i], d) == 0 && (i < j)) i++;
    while (digit(a[j], d) == 1 && (j > i)) j--;
    exch(a[i], a[j]);
  }
  if (digit(a[r], d) == 0) j++;
  quicksortB(a, l, j-1, d+1);
  quicksortB(a, j, r, d+1);
}


#endif