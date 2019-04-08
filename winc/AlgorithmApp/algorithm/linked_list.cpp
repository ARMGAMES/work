

#include "constants.h"
#include "linked_list.h"

LinkedList::LinkedList()
{
	head = NULL;
	curr = NULL;

	int size = sizeof(*head);
	for (int i = 0; i < 10; i++)
	{
		//node* newnode = new node(i, NULL); 
		node* newnode = new node(1000 * (1.0*rand() / RAND_MAX), NULL);
		if (head == NULL)
		{
			head = newnode;
			curr = newnode;
		}
		else
		{
			curr->next = newnode;
			curr = curr->next;
		}
	}
}

LinkedList::~LinkedList()
{
	while (head != NULL)
	{
		node *temp = head;
		head = head->next;
		delete temp;
	}

}

void LinkedList::reverse()
{
	node *next, *curr, *prev = NULL;
	next = curr = head;

	while (curr != 0)
	{
		next = curr->next;
		curr->next = prev;
		prev = curr;
		curr = next;
	}
	head = prev;
}

void LinkedList::selection_sort()
{

}