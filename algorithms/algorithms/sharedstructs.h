#pragma once

struct tnode
{
	int value;
	tnode *l, *r;
	tnode(int v)
	{
		value = v;
		l = 0;
		r = 0;
	}
};

typedef tnode* tlink;

struct node
{
	int value;
	node *next;
	node (int v)
	{
		value = v;
		next = 0;
	}
};

typedef node* link;