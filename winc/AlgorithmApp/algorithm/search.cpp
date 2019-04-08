

#include "constants.h"
#include "search.h"

using namespace std;

namespace search
{

	void quickfind()
	{
		int p, q, t, id[SEARCH_MAX];

		for (int i = 0; i < SEARCH_MAX; i++)
		{
			id[i] = i;
		}

		while (cin >> p >> q)
		{
			if (p < 0 || p >= SEARCH_MAX || q < 0 || q >= SEARCH_MAX)
				continue;

			t = id[p];
			if (t == id[q])
			{
				cout << p << " and " << q << " already connected" << endl;
				continue;
			}

			for (int i = 0; i < SEARCH_MAX; i++)
			{
				if (id[i] == t)
					id[i] = id[q];
			}
			cout << " " << p << " " << q << endl;
		}
	}


	void quickunion()
	{
		int p, q, t, id[SEARCH_MAX], sz[SEARCH_MAX];

		for (int i = 0; i < SEARCH_MAX; i++)
		{
			id[i] = i, sz[i] = 1;
		}

		while (cin >> p >> q)
		{
			// move up the tree structure to the root 
			int i;
			int j;
			for (i = p; i != id[i]; i = id[i]);
			for (j = q; j != id[j]; j = id[j]);

			// if they are same root, means they are connected
			if (i == j)
			{
				cout << p << " and " << q << " already connected" << endl;
				continue;
			}

			// if they are not same root, compare the children of each root, and add root that has less children to root has more children to make it equally weighted
			if (sz[i] < sz[j])
			{
				id[i] = j; sz[j] += sz[i];
			}
			else
			{
				id[j] = i; sz[i] += sz[j];
			}
			cout << " " << p << " " << q << endl;
		}
	}

	void halvingquickunion()
	{
		int p, q, t, id[SEARCH_MAX], sz[SEARCH_MAX];

		for (int i = 0; i < SEARCH_MAX; i++)
		{
			id[i] = i, sz[i] = 1;
		}

		while (cin >> p >> q)
		{
			// move up the tree structure to the root 
			int i;
			int j;
			for (i = p; i != id[i]; i = id[i]);
			{
				// make tree flatten
				id[i] = id[id[i]];
			}
			for (j = q; j != id[j]; j = id[j]);
			{
				// make tree flatten
				id[j] = id[id[j]];
			}

			// if they are same root, means they are connected
			if (i == j)
			{
				cout << p << " and " << q << " already connected" << endl;
				continue;
			}

			// if they are not same root, compare the children of each root, and add root that has less children to root has more children to make it equally weighted
			if (sz[i] < sz[j])
			{
				id[i] = j; sz[j] += sz[i];
			}
			else
			{
				id[j] = i; sz[i] += sz[j];
			}
			cout << " " << p << " " << q << endl;
		}

	}

	void bst_test()
	{
	/*	ST_BST<Item, Key> st;
		st.insert(Item(10));
		st.insert(Item(7));
		st.insert(Item(3));
		st.insert(Item(18));
		st.insert(Item(15));
		st.insert(Item(13));
		st.insert(Item(16));
		st.insert(Item(28));
		st.insert(Item(23));
		st.insert(Item(29));

		//st.calint(56);
		//int cont = st.count();

		int k = 4;

		Item t = st.select(k);

		st.balance();
		//st.part(k);
		//st.remove(Item(18));
		st.count();

		st.show();
		*/
	}

	void array_insertion_test()
	{
		/*const int N = 10;
		int a[N];
		for (int i = 0; i < N; i++)
		{
			a[i] = i;
		}

		ST_BST<Item, Key> st;
		queue<int> tq;
		tq.push(0);
		tq.push(N - 1);

		while (!tq.empty())
		{
			int l = tq.front(); tq.pop();
			int r = tq.front(); tq.pop();

			if (l > r)
				continue;

			if (l == r)
			{
				st.insert(Item(a[l]));
				continue;
			}
			int m = (l + r) / 2;
			st.insert(a[m]);
			tq.push(a[l]); tq.push(a[m - 1]);
			tq.push(a[m + 1]); tq.push(a[r]);
		}*/
	}

}// namespace search