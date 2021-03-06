#pragma once

using namespace std;

// Adjacency matrix
// int adj[V][V]
// pros: checking and removing an edge takes O(1) time
// cons: comesumes more space O(V*V), adding a vertex takes O(V*V)
class AdjMatrixGraph
{
private:
	int V;
	vector<vector<int>> graph;
	int minKey(vector<int>& key, vector<bool>& mstSet);
	int minDistance(vector<int>& dist, vector<bool>& sptSet);
public:
	AdjMatrixGraph(int maxV);
	void addEdgeUndir(int src, int dest, int weight);

	// directed
	void primMST();
	void shortestpath(int src);
};

void testAdjMatrixGraph();

//////////////////////////////////////////////////////////////////////////////////////
// Adjacency list
// vector<int> adj[V]
// pros: save space, adding vertex is easy
// cons: query edge takes O(V)

class AdjListGraph
{
private:
	int V;
	list<int> *adj;
	vector<list<int>> adjlist;
	// con: searchEdge takes O(lgV). pros: adding an edge takes O(log V), no duplicates
	set<int> *adjSet;
	// con: searchEdge takes O(1), adding an edge takes O(1). pros: no duplicates, edges no order
	unordered_set<int>* adjHash;

	void DFSUtil(int v, vector<bool>& visited);
	bool isCyclicUtil(int v, vector<bool>& visited, vector<bool>& recStack);
	bool isCyclicUtilUndir(int v, vector<bool>& visited, int parent);

public:
	AdjListGraph(int V_);
	~AdjListGraph();
	void addEdge(int v, int w);
	void addEdgeUnDir(int v, int w);

	// prints BFS traversal from a given source s
	void BFS(int s);
	void DFS(int v);
	void DFSNonRecursive(int s);

	// Returns a mother vertex if exists. Otherwise returns -1 
	int findMother();
	// Count the number of nodes at given level in a tree using BFS.
	int findNodeNumByLevel(int v, int l);

	bool isCyclic();
	bool isCyclicUndir();

	// requires Directed Acyclic Graph(DAG)
	void topologicalSortUtil(int v, vector<bool>& visited, stack<int>& Stack);
	void topologicalSort();
	void alienDictionary(string words[], int n, int alpha);

};

void testAdjGraph();

///////////////////////////////////////////////////////////////////////////////
// undirected graph
class EdgeGraph
{
public:
	struct Edge
	{
		int src, dest;
		int weight;
		Edge() { src = 0; dest = 0; weight = 0; }
		Edge(int src_, int dest_) : src(src_), dest(dest_) { weight = 0; }
		Edge(int src_, int dest_, int weight_) : src(src_), dest(dest_), weight(weight_) {  }
	};

	struct subset
	{
		int parent,rank;
		subset():parent(-1), rank(0){}
	};
private:
	int V;
	vector<Edge> edge;

	// A utility function to find the subset of an element i 
	int find(vector<int> parent, int i);
	int find(vector<subset> parent, int i);

public:
	EdgeGraph(int V_);

	void addEdge(const Edge& e);

	// undirected graph
	bool isCycleBasic();
	bool isCycleRank();
	// undirected and weighted graph 
	void KruskalMST();

	// directed and weighted graph 
	void BellmanFordSP(int src);
};

bool cmpEdge(const EdgeGraph::Edge& a, const EdgeGraph::Edge& b);


void testEdgeGraph();

///////////////////////////////////////////////////////////////////////////////

void testGraph();
