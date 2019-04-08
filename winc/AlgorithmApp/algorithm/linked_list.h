#pragma once

#include "constants.h"

class LinkedList
{
	struct node
	{
		node() { value = -1; next = NULL; }
		node(int v, node* t) { value = v; next = t; }

		int value;
		node* next;
	};

public:
	LinkedList();
	~LinkedList();

	void reverse();

	void selection_sort();

private:
	node * head;
	node* curr;

};

