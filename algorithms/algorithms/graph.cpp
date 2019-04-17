#include "pch.h"
#include "graph.h"

using namespace std;

AdjListGraph::AdjListGraph(int V_)
{
	V = V_;
	adj = new list<int>[V];
	adjSet = new set<int>[V];
	adjHash = new unordered_set<int>[V];
}

AdjListGraph::~AdjListGraph()
{
	delete[] adj;
	delete[] adjSet;
	delete[] adjHash;
}

void AdjListGraph::addEdge(int v, int w)
{
	adj[v].push_back(w);
}

void AdjListGraph::addEdgeUnDir(int v, int w)
{
	adj[v].push_back(w);
	adj[w].push_back(v);
}

void AdjListGraph::BFS(int s)
{
	// mark all the vertices as not visited
	vector<bool> visited(V, false);

	list<int> queue;
	visited[s] = true;
	queue.push_back(s);

	while (!queue.empty())
	{
		int vertex = queue.front();
		queue.pop_front();

		cout << "vertice: " << vertex << endl;

		for (auto it : adj[vertex])
		{
			if (!visited[it])
			{
				visited[it] = true;
				queue.push_back(it);
			}
		}
	}
}

void AdjListGraph::DFSUtil(int v, vector<bool>& visited)
{
	// Mark the current node as visited and print it 
	visited[v] = true;
	cout << "vertice: " << v << endl;

	for (auto it : adj[v])
	{
		if (!visited[it])
			DFSUtil(it, visited);
	}
}

bool AdjListGraph::isCyclicUtil(int v, vector<bool>& visited, vector<bool>& recStack)
{
	if (visited[v] == false)
	{
		// Mark the current node as visited and part of recursion stack 
		visited[v] = true;
		recStack[v] = true;

		// Recur for all the vertices adjacent to this vertex 
		for (auto it : adj[v])
		{
			if (!visited[it] && isCyclicUtil(it, visited, recStack))
				return true;
			else if (recStack[it])
				return true;
		}

	}
	recStack[v] = false;  // remove the vertex from recursion stack 
	return false;
}

bool AdjListGraph::isCyclicUtilUndir(int v, vector<bool>& visited, int parent)
{
	visited[v] = true;

	for (auto currV : adj[v])
	{
		if (!visited[currV])
		{
			if (isCyclicUtilUndir(currV, visited, v))
				return true;
		}
		// If an adjacent is visited and not parent of current vertex,  then there is a cycle. 
		else if (currV != parent)
			return true;
	}
	return false;
}

void AdjListGraph::DFS(int v)
{
	// mark all the vertices as not visited
	vector<bool> visited(V, false);

	DFSUtil(v, visited);
}

void AdjListGraph::DFSNonRecursive(int s)
{
	// Initially mark all verices as not visited 
	vector<bool> visited(V, false);

	// Create a stack for DFS, Push the current source node. 
	stack<int> stack;
	stack.push(s);

	while (!stack.empty())
	{
		// Pop a vertex from stack and print it 
		int vertex = stack.top();
		stack.pop();

		if (!visited[vertex])
		{
			cout << vertex << " ";
			visited[vertex] = true;
		}

		for (auto i = adj[vertex].begin(); i != adj[vertex].end(); ++i)
			if (!visited[*i])
				stack.push(*i);
	}
}

int AdjListGraph::findMother()
{
	vector<bool> visited(V, false);

	// To store last finished vertex (or mother vertex)
	int v = 0;

	// Do a DFS traversal and find the last finished vertex   
	for (int i = 0; i < V; i++)
	{
		if (visited[i] == false)
		{
			DFSUtil(i, visited);
			v = i;
		}
	}

	// If there exist mother vertex (or vetices) in given graph, then v must be one (or one of them) 
	// Now check if v is actually a mother vertex (or graph has a mother vertex).  
	// We basically check if every vertex is reachable from v or not
	fill(visited.begin(), visited.end(), false);
	DFSUtil(v, visited);
	for (int i = 0; i < V; i++)
		if (visited[i] == false)
			return -1;

	return v;
}

int AdjListGraph::findNodeNumByLevel(int v, int l)
{
	// Initially mark all verices as not visited 
	vector<bool> visited(V, false);
	int nodeNum = 0;
	int currLevel = 0;

	list<int> queue;
	queue.push_back(v);
	visited[v] = true;

	while (!queue.empty())
	{
		nodeNum = queue.size();
		if (currLevel == l)
		{
			return nodeNum;
		}

		while (nodeNum--)
		{
			int vertex = queue.front();
			queue.pop_front();

			for (auto i : adj[vertex])
			{
				if (!visited[i])
				{
					visited[i] = true;
					queue.push_back(i);
				}
			}
		}
		
		currLevel++;
	}
	return -1;
}

bool AdjListGraph::isCyclic()
{
	vector<bool> visited(V, false);
	vector<bool> recStack(V, false);

	// Call the recursive helper function to detect cycle in different DFS trees 
	for (int i = 0; i < V; i++)
		if (isCyclicUtil(i, visited, recStack))
			return true;

	return false;
}

bool AdjListGraph::isCyclicUndir()
{
	vector<bool> visited(V, false);

	// Call the recursive helper function to detect cycle in different DFS trees 
	for (int u = 0; u < V; u++)
		if (!visited[u]) // Don't recur for u if it is already visited 
			if (isCyclicUtilUndir(u, visited, -1))
				return true;
	return false;
}

void testAdjGraph()
{
	cout << "testAdjGraph" << endl;

	AdjListGraph g1(5);
	g1.addEdge(0, 1);
	g1.addEdge(0, 2);
	g1.addEdge(1, 2);
	g1.addEdge(2, 0);
	g1.addEdge(2, 3);
	g1.addEdge(3, 4);
	g1.addEdge(4, 1);

	cout << "Following is Breadth First Traversal "
		<< "(starting from vertex 2) \n";
	g1.BFS(2);

	cout << "Following is Depth First Traversal "
		<< "(starting from vertex 2) \n";
	g1.DFS(2);

	AdjListGraph g2(7);
	g2.addEdge(0, 1);
	g2.addEdge(0, 2);
	g2.addEdge(1, 3);
	g2.addEdge(4, 1);
	g2.addEdge(5, 6);
	g2.addEdge(5, 2);
	g2.addEdge(6, 4);
	g2.addEdge(6, 0);

	cout << "Following is Breadth First Traversal "
		<< "(starting from vertex 0) \n";
	g2.DFS(0);

	AdjListGraph g3(7);
	g3.addEdge(0, 1);
	g3.addEdge(0, 2);
	g3.addEdge(1, 3);
	g3.addEdge(2, 5);
	g3.addEdge(2, 6);
	g3.addEdge(4, 1);
	g3.addEdge(5, 6);
	g3.addEdge(6, 4);
	g3.addEdge(6, 0);
	_ASSERT(g3.findNodeNumByLevel(0, 2) == 3);

	AdjListGraph g4(3);
	g4.addEdge(0, 1);
	//g4.addEdge(0, 1);
	g4.addEdge(1, 2);
	g4.addEdge(2, 1);
	//g4.addEdge(3, 1);

	_ASSERT(g4.isCyclic());

	AdjListGraph g5(5);
	g5.addEdgeUnDir(1, 0);
	g5.addEdgeUnDir(0, 2);
	g5.addEdgeUnDir(0, 3);
	g5.addEdgeUnDir(3, 4);
	//g5.addEdgeUnDir(1, 0);
	_ASSERT(!g5.isCyclicUndir());

	g5.addEdgeUnDir(1, 2);
	_ASSERT(g5.isCyclicUndir());
}

/////////////////////////////////////////////////////////////////////////////////
// Edge graph
EdgeGraph::EdgeGraph(int V_)
{
	V = V_;
}

void EdgeGraph::addEdge(const Edge & e)
{
	edge.push_back(e);
}

bool EdgeGraph::isCycleBasic()
{
	vector<int> parent(V);
	for (int i = 0; i < V; ++i)
	{
		parent[i] = i;
	}

	for (auto curr : edge)
	{
		int x = find(parent, curr.src);
		int y = find(parent, curr.dest);

		if (x == y)
			return true;
		else
			parent[x] = y;
	}

	return false;
}

bool EdgeGraph::isCycleRank()
{
	vector<subset> subsets(V);
	for (int i = 0; i < V; ++i)
	{
		subsets[i].parent = i;
	}

	for (auto curr : edge)
	{
		int x = find(subsets, curr.src);
		int y = find(subsets, curr.dest);

		if (x == y)
			return true;

		// Attach smaller rank tree under root of high rank tree  (Union by Rank) 
		if (subsets[x].rank < subsets[y].rank)
			subsets[x].parent = y;
		else if (subsets[x].rank > subsets[y].rank)
			subsets[y].parent = x;

		// If ranks are same, then make one as root and increment its rank by one 
		else
		{
			subsets[y].parent = x;
			subsets[x].rank++;
		}

	}

	return false;
}

int EdgeGraph::find(vector<int> parent, int i)
{
	if (parent[i] != i)
		return find(parent, parent[i]);
	return i;
}

int EdgeGraph::find(vector<subset> subsets, int i)
{
	// find root and make root as parent of i (path compression) 
	if (subsets[i].parent != i)
		subsets[i].parent = find(subsets, subsets[i].parent);

	return subsets[i].parent;
}

void testEdgeGraph()
{
	cout << "testEdgeGraph" << endl;
	EdgeGraph g1(6);
	g1.addEdge(EdgeGraph::Edge(0, 1));
	g1.addEdge(EdgeGraph::Edge(1, 2));
	g1.addEdge(EdgeGraph::Edge(2, 3));
	g1.addEdge(EdgeGraph::Edge(3, 4));
	g1.addEdge(EdgeGraph::Edge(4, 5));
	g1.addEdge(EdgeGraph::Edge(5, 0));

	_ASSERT(g1.isCycleBasic());

	_ASSERT(g1.isCycleRank());

}

/////////////////////////////////////////////////////////////////////////////////
void testGraph()
{
	testAdjGraph();
	testEdgeGraph();
}

