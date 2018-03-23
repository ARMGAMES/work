
#ifndef SEARCH_H
#define SEARCH_H

#include "stdafx.h"

using namespace std;

namespace search
{

/*
Key-indexed-array symbol table
Assume key are positive int, and less than sentinel value M
*/
template<class Item, class Key>
class ST_KeyIndex
{
public:
	ST_KeyIndex(int maxN)
  {
    M = nullItem.key();
    st = new Item[M];
  }
  ~ST_KeyIndex() {delete[] st;}

	int count()
  {
    int N = 0;
    for (int i = 0; i < M; i++)
    {
      if (!st[i].null()) N++; 
    }
    return N;
  }

	Item search(Key k)
  {
    return st[k];
  }

	void insert(Item x)
  {
    st[x.key()] = x;
  }

	void remove(Item x)
  {
    st[x.key()] = nullItem;
  }

	Item select(int k)
  {
    for (int i =0; i < M; i++)
    {
      if (!st[i].null())
      {
        if (k-- == 0) return st[i];
      }
    }
    return nullItem;
  }

	void show(ostream& os)
  { 
    for (int i = 0; i < M; i++)
      if (!st[i].null()) st[i].show(os); 
  }

private:
  Item nullItem, *st;
  int M;
};

/*
Ordered array symbol table
Use binary search
*/
template<class Item, class Key>
class ST_Array
{
public:
  ST_Array(){}
  ~ST_Array(){}

  Item search(Key v)
  {
     return searchR(0, N-1, v);
  }

private:
  /*
  Binary search
  Program 12.7
  */
  Item searchR(int l, int r, Key v)
  {
    if (l > r) return nullItem;
    int m = (l+r)/2;
    if (st[m].key() == v) return st[m];
    if (l==r) return nullItem;
    if (v < st[m].key())
      return searchR(l, m-1, v);
    else return searchR(m+1, r, v);
  }

  /*
  Binary search - interpolation
  */
  Item searchR_Ip(int l, int r, Key v)
  {
    if (l > r) return nullItem;
    int m = l + (v-st[l].key())*(r-1)/ (st[r].key() - st[l].key());
    if (st[m].key() == v) return st[m];
    if (l==r) return nullItem;
    if (v < st[m].key())
      return searchR_Ip(l, m-1, v);
    else return searchR_Ip(m+1, r, v);
  }
  Item nullItem, *st;
  int N;
};

/*
Binary search tree
program 12.8
*/
template<class Item, class Key>
class ST_BST
{
private:
  struct node
  {
    Item item;
    node *l, *r;
    int N;
    node(Item x)
    {
      item = x; l = 0; r = 0; N = 0;
    }
  };

  typedef node* link;
  link head;
  Item nullItem;
  /*
  Binary search - recursive
  */
  Item searchR(link h, Key v)
  {
    if (h == 0) return nullItem;
    Key t = h->item.key();
    if (v == t) return h->item;
    if (v < t) return searchR(h->l, v);
    else return searchR(h->r, v);
  }
  
  /*
  Binary tree insertion - recursive
  */
  void insertR(link& h, Item x)
  {
    if (h == 0) 
    {
      h = new node(x);
      return;
    }

    if (x.key() < h->item.key())
      insertR(h->l, x);
    else
      insertR(h->r, x);
  }

  void insert_nonrecursive(link h, Item x)
  {
	  if (h ==0)
		  h = new node(x);
	  link p, q;
	  for (p = h, q = p; q!=0; p = q? q:p)
		  q = (x.key() < q->item.key()) ? q->l:q->r;

	  if (x.key() < p->item.key())
		  p->l = new node(x);
	  else
		  p->r = new node(x);
  }


  void rotR(link& h)
  {
    link x = h->l;
    h->l = x->r;
    x->r = h;
    h = x;
  }

  void rotL(link& h)
  {
    link x = h->r;
    h->r = x->l;
    x->l = h;
    h = x;
  }

  void insertT(link& h, Item x)
  {
    if (head == 0) 
    {
      head = new node(x);
      return;
    }
    if (x.key()<head->item.key())
    {
      insertT(head->l);
      rotR(head);
    }
    else
    {
      insertT(head->r);
      rotL(head);
    }
  }

  Item selectR(link h, int k)
  {
    if (h == 0) return nullItem;
    int t = (h->l == 0)? 0 : h->l->N;
    if (t > k) return selectR(h->l, k);
    if (t < k) return selectR(h->r, k-t-1);
    return h->item;
  }

  void partR(link& h, int k)
  {
    if (h == 0) return;
    int t = (h->l == 0) ? 0 : h->l->N;
    if (t > k)
    {
      partR(h->l, k); 
      rotR(h);
    }
    if (t < k)
    {
      partR(h->r, k-t-1);
      rotL(h);
    }
  }

	void removeR(link& h, Key v)
	{
		if(h==0) return;
		Key t = h->item.key();

		if (v < t)
			removeR(h->l, v);
		if (v > t)
			removeR(h->r, v);
		if (t == v)
		{
			link t = h;
			h = joinLR(h->l, h->r);
			delete t;
		}
	}

	link joinLR(link& a, link& b)
	{
		if (b == 0) return a;
		partR(b, 0);
		b->l = a;
		return b;
	}

  int count(link h)
  {
    if(h == 0) return 0;
    h->N = count(h->l) + count(h->r) + 1;
    return h->N;
  }

	link joinR(link a, link b)
	{
		if (b == 0) return a;
		if (a == 0) return b;
		insertT(b, a->item);
		b->l = joinR(a->l, b->l);
		b->r = joinR(a->r, b->r);
		delete a; return b;
	}

	void balanceR(link& h)
	{
		if (h == 0 || (h->N == 1)) return;
		partR(h, h->N/2);
		count(h);
		balanceR(h->l);
		balanceR(h->r);
	}

  void showR(link h)
  {
    if (h == 0) return;
    showR(h->l);
    h->item.show();
    showR(h->r);
  }

	bool calint(link h, int k)
	{
		if (h == 0) return false;
		int v = k - h->item.key();
		if (v == 0)
		{
			cout << h->item.key()<<endl;
			return true;
		}
		if (v < 0)
		{
			return false;
		}

		if (calint(h->l, v))
		{
			cout << h->item.key()<<endl;
		//	return true;
		}
		if (calint(h->r, v))
		{
			cout << h->item.key()<<endl;
		//	return true;
		}
		//return false;

	}

public:
  ST_BST(){head = 0;}
  ~ST_BST(){}

  Item search(Key v)
  {
    return searchR(head, v);
  }

  void insert(Item x)
  {
    insertR(head, x);
  }

  void insert_nonrecursive(Item x)
  {
    insert_nonrecursive(head, x);
  }

  /*
  insert root
  */
  void insertT(Item x)
  {
    insertT(head, x);
  }

  int count()
  {
    return count(head);
  }

  Item select(int k)
  {
		int t = k;
    return selectR(head, t);
  }

  void part(int k)
  {
		int t = k;
    partR(head, t);
  }

	void remove(Item x)
	{
		removeR(head, x.key());
	}

	void join(ST_BST<Item, Key>& b)
	{
		head = joinR(head, b.head);
	}

	void balance()
	{
		balanceR(head);
	}

	void calint(int k)
	{
		calint(head, k);
	}

  void show()
  {
    showR(head);
  }
};


/*
Quick-find solution set all connected numbers to same number.  If two numbers are same, they are connected,
otherwise it change them to same number
BIG(O): 
Find - 1
Union - N*M
*/
void quickfind();

/*
quick union uses tree structure to keep track of connected numbers.
it traverses the pair and find their root to compare.  
This is weighted quick-union
BIG(O): 
Find - lgN
Union - lgN
*/
void quickunion();

void halvingquickunion();

void array_insertion_test();

} // namespace search

#endif