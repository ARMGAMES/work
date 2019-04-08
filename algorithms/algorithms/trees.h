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

	link head;

public:
	void insert(link h, int v);
	void insertBSTNonRecursive(int v);


};

// 12.5

