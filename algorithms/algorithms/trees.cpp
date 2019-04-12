
#include "pch.h"
#include "trees.h"

void testTrees()
{
	std::cout << "Test trees\n";

	BST bst;
	bst.insertR(10);
	bst.insertR(5);
	bst.insertR(56);
	bst.insertR(74);
	bst.insertR(127);
	bst.insertR(3);

	bst.insertRoot(50);

}

BST::BST()
{
	head = 0;
}

void BST::insert(tlink& h, int v)
{
	if (h == 0)
	{
		h = new tnode(v);
		return;
	}
	if (v < h->value)
		insert(h->l, v);
	else
		insert(h->r, v);
}

void BST::insertBSTNonRecursive(int v)
{
	tlink p = head;
	
	for (tlink q = p; q != 0; p = (q ? q : p))
	{
		if (q->value > v)
			q = q->l;
		else
			q = q->r;
	}

	if (p->value > v)
		p->l = new tnode(v);
	else
		p->r = new tnode(v);
}

void BST::rotL(tlink& h)
{
	tlink x = h->r;
	h->r = x->l;
	x->l = h;
	h = x;
}

void BST::rotR(tlink& h)
{
	tlink x = h->l;
	h->l = x->r;
	x->r = h;
	h = x;
}

void BST::insertRoot_(tlink& h, int v)
{
	if (h == 0) 
	{
		h = new tnode(v); 
		return;
	}

	if (v < h->value)
	{
		insertRoot_(h->l, v); rotR(h);
	}
	else
	{
		insertRoot_(h->r, v); rotL(h);
	}
}