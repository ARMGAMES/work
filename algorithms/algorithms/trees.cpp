
#include "pch.h"
#include "trees.h"

void testTrees()
{
	std::cout << "Test trees\n";
}

void BST::insert(link h, int v)
{
	if (h == 0)
	{
		h = new node(v);
		return;
	}
	if (h->value < v)
		insert(h->l, v);
	else
		insert(h->r, v);
}

void BST::insertBSTNonRecursive(int v)
{

}