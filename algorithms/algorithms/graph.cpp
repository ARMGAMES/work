#include "pch.h"
#include "graph.h"

using namespace std;

AdjListGraph::AdjListGraph(int V_)
{
	V = V_;
	adj = new list<int>[V];
}

AdjListGraph::~AdjListGraph()
{
	delete[] adj;
}

void AdjListGraph::addEdge(int v, int w)
{
	adj[v].push_back(w);
}

void AdjListGraph::BFS(int s)
{
	// mark all the vertices as not visited
	vector<bool> visited(V);
	for (int i = 0; i < V; ++i)
		visited[i] = false;

	list<int> queue;
	visited[s] = true;
	queue.push_back(s);

	while (!queue.empty())
	{
		int vertex = queue.front();
		queue.pop_front();

		cout << "vertice: " << vertex << endl;

		for (auto it = adj[vertex].begin(); it != adj[vertex].end(); ++it)
		{
			if (!visited[*it])
			{
				visited[*it] = true;
				queue.push_back(*it);
			}
		}
	}
}

void AdjListGraph::DFSUtil(int v, vector<bool>& visited)
{
	// Mark the current node as visited and print it 
	visited[v] = true;
	cout << "vertice: " << v << endl;

	for (auto it = adj[v].begin(); it != adj[v].end(); ++it)
	{
		if (!visited[*it])
			DFSUtil(*it, visited);
	}
}

void AdjListGraph::DFS(int v)
{
	// mark all the vertices as not visited
	vector<bool> visited(V);
	for (int i = 0; i < V; ++i)
		visited[i] = false;

	DFSUtil(v, visited);
}

void testBFS()
{
	cout << "testBFS" << endl;

	AdjListGraph g(5);
	g.addEdge(0, 1);
	g.addEdge(0, 2);
	g.addEdge(1, 2);
	g.addEdge(2, 0);
	g.addEdge(2, 3);
	g.addEdge(3, 4);
	g.addEdge(4, 1);

	cout << "Following is Breadth First Traversal "
		<< "(starting from vertex 2) \n";
	g.BFS(2);

	cout << "Following is Depth First Traversal "
		<< "(starting from vertex 2) \n";
	g.DFS(2);
}

/////////////////////////////////////////////////////////////////////////////////
void testGraph()
{
	testBFS();
}


