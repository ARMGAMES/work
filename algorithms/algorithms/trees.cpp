
#include "pch.h"
#include "trees.h"

using namespace std;

BT::BT()
{
	head = 0;
}

bool BT::find(link root, int k)
{
	if (root == NULL) return false;

	// If key is present at root, or in left subtree or right subtree,  return true; 
	if (root->key == k || find(root->left, k) || find(root->right, k))
		return true;

	// Else return false 
	return false;
}

bool BT::findPath(link root, vector<int>& path, int k)
{
	if (root == NULL)
		return false;

	// Store this node in path vector. The node will be removed if 
	// not in path from root to k 
	path.push_back(root->key);

	// See if the k is same as root's key 
	if (root->key == k)
		return true;

	// Check if k is found in left or right sub-tree 
	if ((root->left && findPath(root->left, path, k)) ||
		(root->right && findPath(root->right, path, k)))
		return true;

	// If not present in subtree rooted with root, remove root from path[] and return false 
	path.pop_back();
	return false;
}

void BT::insert(link root, int key)
{
	list<link> q;
	q.push_back(root);

	// Do level order traversal until we find 
	// an empty place.  
	while (!q.empty()) {
		link temp = q.front();
		q.pop_front();

		if (!temp->left) {
			temp->left = new node(key);
			break;
		}
		else
			q.push_back(temp->left);

		if (!temp->right) {
			temp->right = new node(key);
			break;
		}
		else
			q.push_back(temp->right);
	}
}

int BT::findLCA1(link root, int n1, int n2)
{
	// to store paths to n1 and n2 from the root 
	vector<int> path1, path2;

	// Find paths from root to n1 and root to n1. If either n1 or n2 
	// is not present, return -1 
	if (!findPath(root, path1, n1) || !findPath(root, path2, n2))
		return -1;

	/* Compare the paths to get the first different value */
	int i;
	for (i = 0; i < path1.size() && i < path2.size(); i++)
		if (path1[i] != path2[i])
			break;
	return path1[i - 1];
}

BT::link BT::findLCA2(link root, int n1, int n2)
{
	// Base case 
	if (root == NULL) return NULL;

	// If either n1 or n2 matches with root's key, report 
	// the presence by returning root (Note that if a key is 
	// ancestor of other, then the ancestor key becomes LCA 
	if (root->key == n1 || root->key == n2)
		return root;

	// Look for keys in left and right subtrees 
	link left_lca = findLCA2(root->left, n1, n2);
	link right_lca = findLCA2(root->right, n1, n2);

	// If both of the above calls return Non-NULL, then one key 
	// is present in once subtree and other is present in other, 
	// So this node is the LCA 
	if (left_lca && right_lca)  return root;

	// Otherwise check if left subtree or right subtree is LCA 
	return (left_lca != NULL) ? left_lca : right_lca;
}

BT::link BT::findLCAUtil(link root, int n1, int n2, bool & v1, bool & v2)
{
	// Base case 
	if (root == NULL) return NULL;

	// If either n1 or n2 matches with root's key, report the presence 
	// by setting v1 or v2 as true and return root (Note that if a key 
	// is ancestor of other, then the ancestor key becomes LCA) 
	if (root->key == n1)
	{
		v1 = true;
		return root;
	}
	if (root->key == n2)
	{
		v2 = true;
		return root;
	}

	// Look for keys in left and right subtrees 
	link left_lca = findLCAUtil(root->left, n1, n2, v1, v2);
	link right_lca = findLCAUtil(root->right, n1, n2, v1, v2);

	// If both of the above calls return Non-NULL, then one key 
	// is present in once subtree and other is present in other, 
	// So this node is the LCA 
	if (left_lca && right_lca)  return root;

	// Otherwise check if left subtree or right subtree is LCA 
	return (left_lca != NULL) ? left_lca : right_lca;
}

BT::link BT::findLCA3(link root, int n1, int n2)
{
	// Initialize n1 and n2 as not visited 
	bool v1 = false, v2 = false;

	// Find lca of n1 and n2 using the technique discussed above 
	link lca = findLCAUtil(root, n1, n2, v1, v2);

	// Return LCA only if both n1 and n2 are present in tree 
	if (v1 && v2 || v1 && find(lca, n2) || v2 && find(lca, n1))
		return lca;

	// Else return NULL 
	return NULL;
}

BT::link BT::buildTree(char inOrder[], char preOrder[], int inStrt, int inEnd)
{
	static int preIndex = 0;

	if (inStrt > inEnd)
		return NULL;

	/* Pick current node from Preorder traversal using preIndex and increment preIndex */
	node* tNode = new node(preOrder[preIndex++]);

	/* If this node has no children then return */
	if (inStrt == inEnd)
		return tNode;

	/* Else find the index of this node in Inorder traversal */
	int inIndex = search(inOrder, inStrt, inEnd, tNode->key);

	/* Using index in Inorder traversal, construct left and
	right subtress */
	tNode->left = buildTree(inOrder, preOrder, inStrt, inIndex - 1);
	tNode->right = buildTree(inOrder, preOrder, inIndex + 1, inEnd);

	return tNode;

}

int BT::search(char arr[], int strt, int end, char value)
{
	int i;
	for (i = strt; i <= end; i++)
	{
		if (arr[i] == value)
			return i;
	}
}

void testBT()
{
	std::cout << "testBT\n";

	BT bt1;
	bt1.head = new BT::node(1);
	bt1.head->left = new BT::node(2);
	bt1.head->right = new BT::node(3);
	bt1.head->left->left = new BT::node(4);
	bt1.head->left->right = new BT::node(5);
	bt1.head->right->left = new BT::node(6);
	bt1.head->right->right = new BT::node(7);

	_ASSERT(bt1.findLCA1(bt1.head, 4, 5) == 2);
	_ASSERT(bt1.findLCA3(bt1.head, 4, 5)->key == 2);
	_ASSERT(bt1.findLCA3(bt1.head, 4, 6)->key == 1);
}

///////////////////////////////////////////////////////////////////////////////////
BST::BST()
{
	head = 0;
}

void BST::inorder(link h)
{
	if (!h)
		return;

	inorder(h->left);
	cout << h->key << " ";
	inorder(h->right);
}

void BST::insertR(link& h, int v)
{
	if (h == 0)
	{
		h = new node(v);
		return;
	}
	++h->N;
	if (v < h->key)
		insertR(h->left, v);
	else
		insertR(h->right, v);
}

void BST::insertBSTNonRecursive(int v)
{
	link p = head;
	
	for (link q = p; q != 0; p = (q ? q : p))
	{
		if (q->key > v)
			q = q->left;
		else
			q = q->right;
	}

	if (p->key > v)
		p->left = new node(v);
	else
		p->right = new node(v);
}

BST::link BST::deleteNode(link root, int key)
{
	// base case 
	if (root == NULL) return root;

	// If the key to be deleted is smaller than the root's key, then it lies in left subtree 
	if (key < root->key)
	{
		root->left = deleteNode(root->left, key);
		return root;
	}

	// If the key to be deleted is greater than the root's key, then it lies in right subtree 
	else if (key > root->key)
	{
		root->right = deleteNode(root->right, key);
		return root;
	}

	// node with only one child or no child 
	if (root->left == NULL)
	{
		link temp = root->right;
		free(root);
		return temp;
	}
	else if (root->right == NULL)
	{
		link temp = root->left;
		free(root);
		return temp;
	}

	// If both children exist 
	else 
	{
		link succParent = root->right;

		// Find successor 
		link succ = root->right;
		while (succ->left != NULL) {
			succParent = succ;
			succ = succ->left;
		}

		// Delete successor.  Since successor is always left child of its parent 
		// we can safely make successor's right child as left of its parent. 
		if (succParent != succ) {
			succParent->left = succ->right;
		}
		else {
			root->right = succ->right;
		}

		// Copy Successor Data to root 
		root->key = succ->key;

		// Delete Successor and return root 
		delete succ;
		return root;
	}
}

BST::link BST::buildTree(link h)
{
	// Store nodes of given BST in sorted order
	vector<link> nodes;
	storeBSTNodes(h, nodes);

	// Constucts BST from nodes[]
	int n = nodes.size();
	return buildTreeUtil(nodes, 0, n - 1);
}

void BST::storeBSTNodes(link root, vector<link>& nodes)
{
	// Base case
	if (root == NULL)
		return;

	// Store nodes in Inorder (which is sorted order for BST)
	storeBSTNodes(root->left, nodes);
	nodes.push_back(root);
	storeBSTNodes(root->right, nodes);
}

BST::link BST::buildTreeUtil(vector<link> nodes, int start, int end)
{
	// base case
	if (start > end)
		return NULL;

	/* Get the middle element and make it root */
	int mid = (start + end) / 2;
	link root = nodes[mid];

	/* Using index in Inorder traversal, construct left and right subtress */
	root->left = buildTreeUtil(nodes, start, mid - 1);
	root->right = buildTreeUtil(nodes, mid + 1, end);

	return root;
}

void BST::rotL(link& h)
{
	link x = h->right;
	h->right = x->left;
	x->left = h;
	h = x;
}

void BST::rotR(link& h)
{
	link x = h->left;
	h->left = x->right;
	x->right = h;
	h = x;
}

void BST::insertRoot_(link& h, int v)
{
	if (h == 0) 
	{
		h = new node(v); 
		return;
	}

	if (v < h->key)
	{
		insertRoot_(h->left, v); rotR(h);
	}
	else
	{
		insertRoot_(h->right, v); rotL(h);
	}
}

int BST::selectR(link h, int k)
{
	if (h == 0) return -1;
	int t = (h->left==0) ? 0 : h->left->N; 
	if (t < k) return selectR(h->left, k);
	if (t > k) return selectR(h->right, k - t - 1);
	return h->key;
}

void BST::partR(link&h, int k)
{
	int t = (h->left == 0) ? 0 : h->left->N;
	if (t > k)
	{
		partR(h->left, k); rotR(h);
	}
	if (t < k)
	{
		partR(h->right, k - t - 1); rotL(h);
	}
}

BST::link BST::joinR(link a, link b)
{
	if (a == 0) return b;
	if (b == 0) return a;
	insertRoot_(b, a->key);
	b->left = joinR(a->left, b->left);
	b->right = joinR(a->right, b->right);
	delete a; 
	return b;
}

BST::link BST::minValueNode(link p)
{
	link curr = p;
	while (curr->left)
	{
		curr = curr->left;
	}
	return curr;
}

bool BST::isBST(link root, link left = NULL, link right = NULL)
{
	if (root == NULL)
		return true;

	// if left node exist then check it has correct data or not i.e. left node's data 
   // should be less than root's data 
	if (left != NULL && root->key < left->key)
		return false;

	// if right node exist then check it has  correct data or not i.e. right node's data 
	// should be greater than root's data 
	if (right != NULL && root->key > right->key)
		return false;

	// check recursively for every node. 
	return isBST(root->left, left, root) &&
		isBST(root->right, root, right);
}

void testBST()
{
	std::cout << "testBST\n";

	BST bst;
	bst.insertR(20);
	bst.insertR(10);
	bst.insertR(30);
	bst.insertR(25);
	bst.insertR(40);
	bst.insertR(35);
	bst.insertR(50);

	bst.inorder(bst.head);

	bst.head = bst.buildTree(bst.head);

	//bst.insertRoot(50);

	bst.deleteNode(bst.head, 30);

	bst.isBST(bst.head);
}

//////////////////////////////////////////////////////////////////
// AVL

AVLTree::AVLTree()
{
	head = 0;
}

AVLTree::link AVLTree::rotLeft(link x)
{
	link y = x->right;
	link T2 = y->left;

	// Perform rotation  
	y->left = x;
	x->right = T2;

	// Update heights  
	x->height = max(height(x->left),height(x->right)) + 1;
	y->height = max(height(y->left),height(y->right)) + 1;

	// Return new root  
	return y;
}

AVLTree::link AVLTree::rotRight(link y)
{
	link x = y->left;
	link T2 = x->right;

	// Perform rotation  
	x->right = y;
	y->left = T2;

	// Update heights  
	y->height = max(height(y->left), height(y->right)) + 1;
	x->height = max(height(x->left), height(x->right)) + 1;

	// Return new root  
	return x;
}

// Recursive function to insert a key in the subtree rooted with node and 
// returns the new root of the subtree. 
AVLTree::link AVLTree::insert(link node, int key)
{
	/* 1. Perform the normal BST insertion */
	if (node == NULL)
		return(new AVLTree::node(key));

	if (key < node->key)
		node->left = insert(node->left, key);
	else if (key > node->key)
		node->right = insert(node->right, key);
	else // Equal keys are not allowed in BST  
		return node;

	/* 2. Update height of this ancestor node */
	node->height = 1 + max(height(node->left), height(node->right));

	/* 3. Get the balance factor of this ancestor node to check whether this node became  unbalanced */
	int balance = getBalance(node);

	// If this node becomes unbalanced, then there are 4 cases  
	// Left Left Case  
	if (balance > 1 && key < node->left->key)
		return rotRight(node);
	// Right Right Case  
	if (balance < -1 && key > node->right->key)
		return rotLeft(node);
	// Left Right Case  
	if (balance > 1 && key > node->left->key)
	{
		node->left = rotLeft(node->left);
		return rotRight(node);
	}
	// Right Left Case  
	if (balance < -1 && key < node->right->key)
	{
		node->right = rotRight(node->right);
		return rotLeft(node);
	}

	return node;
}

void testAVL()
{
	std::cout << "testAVL\n";

	AVLTree t1;
	t1.head = t1.insert(t1.head, 10);
	t1.head = t1.insert(t1.head, 8);
	t1.head = t1.insert(t1.head, 5);

}
//////////////////////////////////////////////////////////////////

void Trie::insertR(link & h, const char * word)
{
	if (!h)
		h = new node();

	if (!*word)
	{
		h->isEndOfWord = true;
		return;
	}

	int index = *word - 'a';
	insertR(h->children[index], word + 1);
}

bool Trie::searchR(link h, const char * word)
{
	if (!h)
		return false;
	if (!*word)
		return h->isEndOfWord;
	int index = *word - 'a';
	return searchR(h->children[index], word + 1);
}

void Trie::insertNonRecursive(const char * word)
{
	link p = head;

	for (int i = 0; i < (int)strlen(word); ++i)
	{
		int index = word[i] - 'a';
		if (!p->children[index])
			p->children[index] = new node();
		p = p->children[index];
	}

	// mark last node as leaf
	p->isEndOfWord = true;
}

bool Trie::searchNonRecursive(const char * word)
{
	link p = head;

	for (int i = 0; i < (int)strlen(word); ++i)
	{
		int index = word[i] - 'a';
		if (!p->children[index])
		{
			return false;
		}
		p = p->children[index];
	}

	return (p != nullptr && p->isEndOfWord);
}

Trie::link Trie::remove(link root, const char * word, int depth = 0)
{
	// If tree is empty 
	if (!root)
		return NULL;

	// If last character of key is being processed 
	if (depth == strlen(word)) 
	{
		// This node is no more end of word after removal of given key 
		if (root->isEndOfWord)
			root->isEndOfWord = false;

		// If given is not prefix of any other word 
		if (isEmpty(root)) {
			delete (root);
			root = NULL;
		}
		return root;
	}

	// If not last character, recur for the child obtained using ASCII value 
	int index = *(word + depth) - 'a';
	root->children[index] =
		remove(root->children[index], word, depth + 1);

	// If root does not have any child (its only child got deleted), and it is not end of another word. 
	if (isEmpty(root) && root->isEndOfWord == false) {
		delete (root);
		root = NULL;
	}
}

void testTrie()
{
	std::cout << "testTrie\n";
	Trie trie1;

	trie1.insertNonRecursive("a");
	trie1.insertNonRecursive("this");
	trie1.insertNonRecursive("little");
	trie1.insertNonRecursive("longgest");
	trie1.insertNonRecursive("the");
	trie1.insertNonRecursive("there");

	_ASSERT(trie1.searchNonRecursive("the"));
	_ASSERT(!trie1.searchNonRecursive("thee"));
	_ASSERT(!trie1.searchNonRecursive(""));
	_ASSERT(!trie1.searchNonRecursive("t"));

	Trie trie2;
	trie2.insert("a");
	trie2.insert("this");
	trie2.insert("little");
	trie2.insert("longgest");
	trie2.insert("the");
	trie2.insert("there");

	_ASSERT(trie2.search("the"));
	_ASSERT(!trie2.search("thee"));
	_ASSERT(!trie2.search(""));
	_ASSERT(!trie2.search("t"));

}
//////////////////////////////////////////////////////////////////

void TST::insertR(link & h, const char * word)
{
	if (!h)
		h = new node(*word);

	if (*word < h->c)
		insertR(h->l, word);
	else if (*word > h->c)
		insertR(h->r, word);
	else
	{
		if (*(word + 1))
			insertR(h->m, word + 1);
		else
			h->isEndOfString = 1;
	}
}

bool TST::searchR(link h, const char * word)
{
	if (!h)
		return false;
	if (*word < h->c)
		return searchR(h->l, word);
	else if (*word > h->c)
		return searchR(h->r, word);
	else
	{
		if (*(word + 1))
			return searchR(h->m, word + 1);
		else
			return h->isEndOfString;
	}
}

void TST::partialMartch(link h, const char *word, int i)
{
	if (h == 0) return;

	if (*word == '*' || *word == h->c)
	{
		matchWord[i] = (h->c);

		if (*(word+1) == 0 && h->isEndOfString)
		{
			matchWord[i + 1] = 0;
			cout << "find " << matchWord << endl;
		}
		else
		{
			partialMartch(h->m, word + 1, i + 1);
		}
	}
	if (*word == '*' || *word < h->c)
	{
		partialMartch(h->l, word, i);
	}
	if (*word == '*' || *word > h->c)
	{
		partialMartch(h->r, word, i);
	}
}

void TST::partialMartch(const char * word)
{
	string matchWord;
	partialMartch(head, word, 0);
}

void testTST()
{
	std::cout << "testTST\n";
	TST tst1;

	tst1.insert("a");
	tst1.insert("aa");
	tst1.insert("ab");
	tst1.insert("this");
	tst1.insert("little");
	tst1.insert("longgest");
	tst1.insert("the");
	tst1.insert("there");

	_ASSERT(tst1.search("the"));
	_ASSERT(tst1.search("ab"));
	_ASSERT(!tst1.search("thee"));
	_ASSERT(!tst1.search(""));
	_ASSERT(!tst1.search("t"));

	TST tst2;
	tst2.insert("abc");
	tst2.insert("abda");
	tst2.insert("abdda");
	tst2.insert("acda");
	tst2.insert("acbda");

	tst2.partialMartch("a**a");
}
//////////////////////////////////////////////////////////////////

void testTree()
{
	testBST();
	testBT();
	testAVL();
	testTrie();
	testTST();
}


