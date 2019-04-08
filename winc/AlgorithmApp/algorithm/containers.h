
#pragma once

#include "constants.h"

/*
Priority Queue - implemented by heap
*/
template <class Item>
class PQ
{
public:
	PQ(int maxN)
	{
		pq = new Item[maxN + 1];
		N = 0;
	}
	~PQ()
	{
		delete[] pg;
	}

	int empty() const
	{
		return N == 0;
	}

	void insert(Item item)
	{
		pq[++N] = item;
		fixUp(pq, N);
	}

	Item getmax()
	{
		exch(pq[1], pq[N]);
		fixDown(pq, 1, N - 1);
		return pq[N--];
	}

private:
	void fixUp(Item a[], int k)
	{
		while (k > 1 && a[k / 2] < a[k])
		{
			exch(a[k], a[k / 2]);
			k = k / 2;
		}
	}
	void fixDown(Item a[], int k, int N)
	{
		while (2 * k <= N)
		{
			int j = 2 * k;
			if (j < N && a[j] < a[j + 1])
				j++;
			if (!(a[k] < a[j]))
				break;
			exch(a[k], a[j]);
			k = j;
		}
	}

private:
	Item * pq;
	int N;
};
