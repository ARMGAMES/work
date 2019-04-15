#pragma once

void testTrees();

class BST
{
private:
	struct tnode
	{
		int value;
		int N;
		tnode *l, *r;
		tnode(int v)
		{
			value = v;
			N = 1;
			l = 0;
			r = 0;
		}
	};
	typedef tnode* tlink;

	void rotL(tlink& h);
	void rotR(tlink& h);
	void insert(tlink& h, int v);
	void insertRoot_(tlink& h, int v);
	int selectR(tlink h, int k);
	void partR(tlink&h, int k);
	tlink joinR(tlink a, tlink b);

public:
	tlink head;

	BST();
	void insertR(int v) { insert(head, v); }
	void insertBSTNonRecursive(int v);
	void insertRoot(int v) { insertRoot_(head, v); }
	int select(int k) { return selectR(head, k); }

	
};

// 12.5

