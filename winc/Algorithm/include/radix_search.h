
#ifndef RADIX_SEARCH_H
#define RADIX_SEARCH_H

#include "constants.h"

namespace RadixSearch
{

// Ternary Symbol Table for string
class TST
{
private:
  struct node{
    char d;
    node *l, *m, *r;
    node(char k)
    {
      d = k; l = 0; m = 0; r = 0;
    }
  };

  typedef node* link;
  link head;
	char word[maxWord];

  bool searchR(link h, const string& v, int d)
  {
    if (h ==  NULL)
      return false;
    if (v[d] == 0)
    {
			return (h->d == 0);
    }
    if (v[d] < h->d)
      return searchR(h->l, v, d);
    if (v[d] == h->d)
      return searchR(h->m, v, d+1);
    if (v[d] > h->d)
      return searchR(h->r, v, d);
  }

  void insertR(link& h, const string& v, int d)
  {
    if (h == NULL) 
    {
      h = new node(v[d]);
    }
		// to allow substring
		if(v[d] == 0 && h->d == 0)
		{
			return;
		}

		if (v[d] < h->d)
			insertR(h->l, v, d);
		if (v[d] == h->d)
			insertR(h->m, v, d+1);
		if (v[d] > h->d)
			insertR(h->r, v, d);
  }

	/*
	 partial-match search in TST
	*/
	void matchR(link h, char* v, int i)
	{
		if (h == 0) return;
		if (*v == 0 && h->d == 0)
		{
			word[i] = 0; cout << word << endl;
		}

		if (*v == '*' || *v == h->d)
		{
			word[i] = h->d; matchR(h->m, v+1, i+1);
		}
		if (*v == '*' || *v < h->d)
			matchR(h->l, v, i);
		if (*v == '*' || *v > h->d)
			matchR(h->r, v, i);
	}

public:
  TST(int maxN)
  {
    head = 0;
  }
  bool search(const string& v)
  { return searchR(head, v, 0); }
  void insert(const string& v)
  { insertR(head, v, 0); }
	void match(char* v)
	{ matchR(head, v, 0);}
};

// use val for string index 
class StringST
{
private:
	struct node
	{
		int val, key;
		node *l, *m, *r;
		node(int k):val(-1), key(k), l(0), m(0),r(0){}
	};

	typedef node* link;
	link head;
	int N, val;
	void indexR(link& h, const string& key, int d)
	{
		int i = key[d];
		if (h == 0)
		{
			h = new node(i);
		}
		if (i == 0)
		{
			if (h->val == -1)
				h->val = N++;
			val = h->val;
			return;
		}
		if (i < h->key)
			indexR(h->l, key, d);
		if (i == h->key)
			indexR(h->m, key, d+1);
		if (i > h->key)
			indexR(h->r, key, d);
	}

public:
	StringST():head(0), N(0){}
	int index(const string& key)
	{
		indexR(head, key, 0);
		return val;
	}

};
}

#endif