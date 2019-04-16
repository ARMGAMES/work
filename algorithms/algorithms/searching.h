#pragma once

#include "sharedstructs.h"

////////////////////////////////////////////////////////////////////
// Hashing
// seperate chaining
class STHashing
{
private:
	struct node
	{
		int value;
		node *next;
		node(int v)
		{
			value = v;
			next = 0;
		}
		node(int v, node* next_)
		{
			value = v;
			next = next_;
		}
	};
	typedef node* link;
	link* heads;
	int N, M;
	int searchSCR(link h, int v);

public:
	STHashing(int maxN)
	{
		N = 0; M = maxN / 5;
		heads = new link[M];
		for (int i = 0; i < M; i++) heads[i] = 0;
	}
	int searchSC(int v)
	{
		return searchSCR(heads[hash1(v, M)], v);
	}
	void insert(int item)
	{
		int i = hash1(item, M);
		heads[i] = new node(item, heads[i]); 
		N++;
	}
};

////////////////////////////////////////////////////////////////////
// Radix Search
// Digital search trees(DSTs)
//
template <class Item, class Key>
class STDSTs
{
private:
	struct node
	{
		Item item; node *l, *r;
		node(Item x)
		{
			item = x; l = 0; r = 0;
		}
	};
	typedef node *link;
	link head;
	Item nullItem;

	Item searchR(link h, Key v, int d)
	{
		if (h == 0)
			return nullItem;
		if (v == h->item.key())
			return h->item;
		if (digit(v, d) == 0)
			return searchR(h->l, v, d + 1);
		else return searchR(h->r, v, d + 1);
	}
public:
	STDSTs(int maxN)
	{
		head = 0;
	}
	Item search(Key v) { return searchR(head, v, 0); }
};

////////////////////////////////////////////////////////////////////
// Ternary Search Trie (TST)
template <class Item, class Key>
class TST
{
private:
	struct node
	{
		Item item;
		int d;
		node*l, *m, *r;
		node(int k)
		{
			d = k;
			l = 0; m = 0; r = 0;
		}
	};
	typedef node* link;
	link head;
	Item nullItem;

	Item searchR(link h, Key v, int d)
	{
		int i = digit(v, d);
		if (h == 0) return nullItem;
		if (i == NULLdigit)
		{
			Item dummy(v); return dummy;
		}
		if (i < h->d) return searchR(h->l, v, d);
		if (i == h->d) return searchR(h->m, v, d + 1);
		if (i > h->d) return searchR(h->r, v, d);

	}
	void insertR(link& h, Item x, int d)
	{
		int i = digit(x.key(), d);
		if (h == 0) h = new node(i);
		if (i == NULLdigit) return;
		if (i < h->d) insertR(h->l, x, d);
		if (i == h->d) insertR(h->m, x, d + 1);
		if (i > h->d) insertR(h->r, x, d);
	}

public:
	TST()
	{
		head = 0;
	}
	Item search(Key v) { return searchR(head, v, 0); }
	void insert(Item x) { insertR(head, x, 0); };
};

////////////////////////////////////////////////////////////////

const int ALPHABET_SIZE = 26;

class StringTrie
{
private:
	struct node
	{
		node* children[ALPHABET_SIZE];
		bool isEndOfWord;	

		node()
		{
			for (int i = 0; i < ALPHABET_SIZE; ++i)
			{
				children[i] = nullptr;
			}
			isEndOfWord = false;
		}
	};

	typedef node* link;
	link head;

	void insertR(link& h, const char* word)
	{
		if (!h)
			h = new node();

		if (!*word)
		{
			h->isEndOfWord = true;
			return;
		}

		int index = *word - 'a';
		insertR(h->children[index], word + 1);
	}

	bool searchR(link h, const char* word)
	{
		if (!h)
			return false;
		if (!*word)
			return h->isEndOfWord;
		int index = *word - 'a';
		return searchR(h->children[index], word + 1);
	}
	
public:
	StringTrie()
	{}

	void insertNonRecursive(const char* word)
	{
		if (!head)
		{
			head = new node();
		}
		link p = head;

		for (int i = 0; i < (int)strlen(word); ++i)
		{
			int index = word[i] - 'a';
			if (!p->children[index])
				p->children[index] = new node();
			p = p->children[index];
		}

		// mark last node as leaf
		p->isEndOfWord = true;
	}
	bool searchNonRecursive(const char* word)
	{
		link p = head;

		for (int i = 0; i < (int)strlen(word); ++i)
		{
			int index = word[i] - 'a';
			if (! p->children[index])
			{
				return false;
			}
			p = p->children[index];
		}

		return (p != nullptr && p->isEndOfWord);
	}
	void insert(const char* word) { insertR(head, word); }
	bool search(const char* word) { return searchR(head, word); }
};

void testStringTrie();

//////////////////////////////////////////////////////////////////////////////
class StringTST
{
private:
	struct node
	{
		char c;
		bool isEndOfString;
		node *l, *m, *r;
		node(char c_)
		{
			c = c_; l = 0; m = 0; r = 0;
			isEndOfString = false;
		}
	};
	typedef node* link;
	link head;

	void insertR(link& h, const char* word)
	{
		if (!h)
			h = new node(*word);

		if (*word < h->c)
			insertR(h->l, word);
		else if (*word > h->c)
			insertR(h->r, word);
		else
		{
			if (*(word + 1))
				insertR(h->m, word + 1);
			else
				h->isEndOfString = 1;
		}
	}
	bool searchR(link h, const char* word)
	{
		if (!h)
			return false;
		if (*word < h->c)
			return searchR(h->l, word);
		else if (*word > h->c)
			return searchR(h->r, word);
		else
		{
			if (*(word + 1))
				return searchR(h->m, word + 1);
			else
				return h->isEndOfString;
		}
	}
public:
	StringTST(){}

	void insert(const char* word) { insertR(head, word); }
	bool search(const char* word) { return searchR(head, word); }
};

void testStringTST();

///////////////////////////////////////////////////////////////////////////////////
// KMP Algorithm for Pattern Searching
// 
// Fills lps[] for given patttern pat[0..M-1] 
void computeLPSArray(const char* pat, int M, int* lps);
void KMPSearch(const char* pat, const char* txt);
void testKMPAlgorithm();

/////////////////////////////////////////////////////////////////////////////////////
//  Bad Character Heuristic of Boyer Moore String Matching Algorithm 
const int NO_OF_CHARS = 256;

void badCharHeuristic(const char* pat, int size, int badChar[NO_OF_CHARS]);
void badCharacterSearch(const char* pat, const char* txt);
void testBadCharacterMatching();

/////////////////////////////////////////////////////////////////////////////////////

void testSearching();