#pragma once

using namespace std;

///////////////////////////////////////////////////////////////////////////
// Binary Tree
class BT
{
public:
	struct node
	{
		int key;
		node *left, *right;
		node(int v)
		{
			key = v;
			left = 0;
			right = 0;
		}
	};
	typedef node* link;

	link head;

	BT();

	bool find(link root, int k);

	bool findPath(link root, vector<int>& path, int k);

	void insert(link root, int key);

	/* Returns LCA if node n1, n2 are present in the given binary tree, otherwise return -1 
	   By Storing root to n1 and root to n2 paths 
	   time:O(N), space: O(N)*/

	int findLCA1(link root, int n1, int n2);

	/* Using Single Traversal. time: O(N)
	 This function returns pointer to LCA of two given values n1 and n2. 
	 This function assumes that n1 and n2 are present in Binary Tree */
	link findLCA2(link root, int n1, int n2);

	// works if n1 and n2 not present in BT
	link findLCAUtil(link root, int n1, int n2, bool &v1, bool &v2);
	link findLCA3(link root, int n1, int n2);

	// build BT from inorder, preorder
	link buildTree(char in[], char pre[], int inStrt, int inEnd);
	int search(char arr[], int strt, int end, char value);
};

void testBT();

///////////////////////////////////////////////////////////////////////////
// Binary Search Tree
class BST
{
private:
	struct node
	{
		int key;
		int N;
		node *left, *right;
		node(int v)
		{
			key = v;
			N = 1;
			left = 0;
			right = 0;
		}
	};
	typedef node* link;

	void rotL(link& h);
	void rotR(link& h);
	void insertR(link& h, int v);
	void insertRoot_(link& h, int v);
	int selectR(link h, int k);
	void partR(link&h, int k);
	link joinR(link a, link b);
	link minValueNode(link p);
public:
	link head;

	BST();
	//Inorder traversal of BST always produces sorted output.
	void inorder(link h);
	void insertR(int v) { insertR(head, v); }
	void insertBSTNonRecursive(int v);
	void insertRoot(int v) { insertRoot_(head, v); }
	link deleteNode(link root, int key);

	int select(int k) { return selectR(head, k); }

	// build balanced tree
	link buildTree(link h);
	void storeBSTNodes(link root, vector<link>& nodes);
	link buildTreeUtil(vector<link> nodes, int start, int end);

	// isBST
	bool isBST(link root, link left, link right);
};

void testBST();

//////////////////////////////////////////////////////////////////

class AVLTree
{
	struct node
	{
		int key;
		int height;
		node *left, *right;
		node(int v)
		{
			key = v;
			height = 1;
			left = 0;
			right = 0;
		}
	};
	typedef node* link;

	int height(link N)
	{
		if (N == NULL)
			return 0;
		return N->height;
	}

	int getBalance(link N)
	{
		if (N == NULL)
			return 0;
		return height(N->left) - height(N->right);
	}

public:
	link head;
	AVLTree();

	link rotLeft(link h);
	link rotRight(link h);

	link insert(link node, int key);

};

void testAVL();
//////////////////////////////////////////////////////////////////
// Trie
// pros:insert and find strings in O(L) time
// cons:need lot of memory
const int ALPHABET_SIZE = 26;

class Trie
{
public:
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
	

	void insertR(link& h, const char* word);
	bool searchR(link h, const char* word);

	Trie()
	{
		if (!head)
		{
			head = new node();
		}
	}

	// Returns true if root has no children, else false 
	bool isEmpty(link root)
	{
		for (int i = 0; i < ALPHABET_SIZE; i++)
			if (root->children[i])
				return false;
		return true;
	}

	link head;
	void insertNonRecursive(const char* word);
	bool searchNonRecursive(const char* word);
	void insert(const char* word) { insertR(head, word); }
	bool search(const char* word) { return searchR(head, word); }
	link remove(link head, const char* word, int depth);
};

void testTrie();

////////////////////////////////////////////////////////////////

class TST
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
	char matchWord[256];
	void insertR(link& h, const char* word);
	bool searchR(link h, const char* word);
	void partialMartch(link h, const char* word, int i);
public:
	TST() {}

	void insert(const char* word) { insertR(head, word); }
	bool search(const char* word) { return searchR(head, word); }
	void partialMartch(const char* word);
};

void testTST();

////////////////////////////////////////////////////////////////

void testTree();

