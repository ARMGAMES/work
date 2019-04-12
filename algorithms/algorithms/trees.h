#pragma once

void testTrees();

class BST
{
private:
	void rotL(tlink& h);
	void rotR(tlink& h);
	void insert(tlink& h, int v);
	void insertRoot_(tlink& h, int v);

public:
	tlink head;

	BST();
	void insertR(int v) { insert(head, v); }
	void insertBSTNonRecursive(int v);
	void insertRoot(int v) { insertRoot_(head, v); }


};

// 12.5

