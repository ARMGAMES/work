#pragma once

using namespace std;

// Adjacency matrix
// int adj[V][V]
// pros: checking and removing an edge takes O(1) time
// cons: comesumes more space O(V*V), adding a vertex takes O(V*V)



// Adjacency list
// vector<int> adj[V]
// pros: save space, adding vertex is easy
// cons: query edge takes O(V)

class AdjListGraph
{
private:
	int V;
	list<int> *adj;

	void DFSUtil(int v, vector<bool>& visited);

public:
	AdjListGraph(int V_);
	~AdjListGraph();
	void addEdge(int v, int w);

	// prints BFS traversal from a given source s
	void BFS(int s);
	void DFS(int v);
};

void testBFS();

///////////////////////////////////////////////////////////////////////////////
void testGraph();
