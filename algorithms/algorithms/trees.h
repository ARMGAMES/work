#pragma once

void testTrees();

struct node
{
	int value;
	node *l, *r;
	node(int v)
	{
		value = v;
		l = 0;
		r = 0;
	}

};

class BST
{
private:
	typedef node* link;

	
	void rotL(link& h);
	void rotR(link& h);
	void insert(link& h, int v);
	void insertRoot_(link& h, int v);

public:
	link head;

	BST();
	void insertR(int v) { insert(head, v); }
	void insertBSTNonRecursive(int v);
	void insertRoot(int v) { insertRoot_(head, v); }


};

// 12.5

