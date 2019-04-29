#include "pch.h"
#include "graph.h"

using namespace std;

AdjMatrixGraph::AdjMatrixGraph(int maxV)
{
	V = maxV;
	graph.resize(V);
	for (int i = 0; i < V; ++i)
	{
		graph[i].resize(V);
	}
}

int AdjMatrixGraph::minKey(vector<int>& key, vector<bool>& mstSet)
{
	// Initialize min value 
	int min = INT_MAX, min_index;

	for (int v = 0; v < V; v++)
		if (mstSet[v] == false && key[v] < min)
			min = key[v], min_index = v;

	return min_index;
}

int AdjMatrixGraph::minDistance(vector<int>& dist, vector<bool>& sptSet)
{
	// Initialize min value 
	int min = INT_MAX, min_index;

	for (int v = 0; v < V; v++)
		if (sptSet[v] == false && dist[v] <= min)
			min = dist[v], min_index = v;

	return min_index;

}

void AdjMatrixGraph::addEdgeUndir(int src, int dest, int weight)
{
	graph[src][dest] = weight;
	graph[dest][src] = weight;
}

void AdjMatrixGraph::primMST()
{
	// Array to store constructed MST 
	vector<int> parent;
	parent.resize(V, 0);

	// Key values used to pick minimum weight edge in cut 
	vector<int> key;
	key.resize(V, INT_MAX);

	// To represent set of vertices not yet included in MST 
	vector<bool> mstSet;
	mstSet.resize(V, false);

	// Always include first 1st vertex in MST. 
	// Make key 0 so that this vertex is picked as first vertex. 
	key[0] = 0;
	parent[0] = -1; // First node is always root of MST  

	// The MST will have V vertices 
	for (int count = 0; count < V - 1; count++)
	{
		// Pick the minimum key vertex from the set of vertices not yet included in MST 
		int u = minKey(key, mstSet);

		// Add the picked vertex to the MST Set 
		mstSet[u] = true;

		// Update key value and parent index of the adjacent vertices of the picked vertex.  
		// Consider only those vertices which are not yet included in MST 
		for (int v = 0; v < V; v++)
			// graph[u][v] is non zero only for adjacent vertices of m 
			// mstSet[v] is false for vertices not yet included in MST 
			// Update the key only if graph[u][v] is smaller than key[v] 
			if (graph[u][v] && mstSet[v] == false && graph[u][v] < key[v])
				parent[v] = u, key[v] = graph[u][v];
	}

	// print the constructed MST
	// printMST(parent, V, graph);
}

void AdjMatrixGraph::shortestpath(int src)
{
	// The output array.  dist[i] will hold the shortest distance from src to i 
	vector<int> dist(V, INT_MAX);

	// sptSet[i] will be true if vertex i is included in shortest path tree or shortest distance from src to i is finalized
	vector<bool> sptSet(V, false);

	vector<int> parent(V, -1);

	// Distance of source vertex from itself is always 0 
	dist[src] = 0;

	// Find shortest path for all vertices 
	for (int count = 0; count < V - 1; count++)
	{
		// Pick the minimum distance vertex from the set of vertices not 
		// yet processed. u is always equal to src in the first iteration. 
		int u = minDistance(dist, sptSet);

		// Mark the picked vertex as processed 
		sptSet[u] = true;

		// Update dist value of the adjacent vertices of the picked vertex. 
		for (int v = 0; v < V; v++)

			// Update dist[v] only if is not in sptSet, there is an edge from  
			// u to v, and total weight of path from src to  v through u is  
			// smaller than current value of dist[v] 
			if (!sptSet[v] && graph[u][v] && dist[u] != INT_MAX
				&& dist[u] + graph[u][v] < dist[v])
			{
				parent[v] = u;
				dist[v] = dist[u] + graph[u][v];
			}
	}
}

void testAdjMatrixGraph()
{
	cout << "testAdjMatrixGraph" << endl;
	AdjMatrixGraph g1(5);
	g1.addEdgeUndir(0, 1, 2);
	g1.addEdgeUndir(1, 2, 3);
	g1.addEdgeUndir(0, 3, 6);
	g1.addEdgeUndir(1, 3, 8);
	g1.addEdgeUndir(1, 4, 5);
	g1.addEdgeUndir(2, 4, 7);
	g1.addEdgeUndir(3, 4, 9);

	g1.primMST();
	g1.shortestpath(0);
}


AdjListGraph::AdjListGraph(int V_)
{
	V = V_;
	adj = new list<int>[V];
	adjlist.resize(V);
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

	//adjlist[V].push_back(w);
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
	queue.push_back(s);
	visited[s] = true;

	while (!queue.empty())
	{
		int vertex = queue.front();
		queue.pop_front();

		cout << "vertex: " << vertex << endl;

		for (auto it : adj[vertex])
		{
			if (!visited[it])
			{
				queue.push_back(it);
				visited[it] = true;
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
			cout << "vertex: " << vertex << endl;
			visited[vertex] = true;
		}

		for (auto it : adj[vertex])
			if (!visited[it])
				stack.push(it);
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

void AdjListGraph::topologicalSortUtil(int v, vector<bool>& visited, stack<int>& Stack)
{
	// Mark the current node as visited. 
	visited[v] = true;

	// Recur for all the vertices adjacent to this vertex 
	list<int>::iterator i;
	for (i = adj[v].begin(); i != adj[v].end(); ++i)
		if (!visited[*i])
			topologicalSortUtil(*i, visited, Stack);

	// Push current vertex to stack which stores result 
	Stack.push(v);
}

void AdjListGraph::topologicalSort()
{
	stack<int> Stack;

	// Mark all the vertices as not visited 
	vector<bool> visited(V, false);

	// Call the recursive helper function to store Topological 
	// Sort starting from all vertices one by one 
	for (int i = 0; i < V; i++)
		if (visited[i] == false)
			topologicalSortUtil(i, visited, Stack);

	// Print contents of stack 
	while (Stack.empty() == false)
	{
		cout << Stack.top() << " ";
		Stack.pop();
	}
	cout << endl;
}

// This function fidns and prints order of characer from a sorted 
// array of words. n is size of words[].  alpha is set of possible 
// alphabets. 
// For simplicity, this function is written in a way that only 
// first 'alpha' characters can be there in words array.  For 
// example if alpha is 7, then words[] should have only 'a', 'b', 
// 'c' 'd', 'e', 'f', 'g' 
void AdjListGraph::alienDictionary(string words[], int n, int alpha)
{
	// Process all adjacent pairs of words and create a graph 
	for (int i = 0; i < n - 1; i++)
	{
		// Take the current two words and find the first mismatching character 
		string word1 = words[i], word2 = words[i + 1];
		for (int j = 0; j < min(word1.length(), word2.length()); j++)
		{
			// If we find a mismatching character, then add an edge 
			// from character of word1 to that of word2 
			if (word1[j] != word2[j])
			{
				addEdge(word1[j] - 'a', word2[j] - 'a');
				break;
			}
		}
	}

	// Print topological sort of the above created graph 
	topologicalSort();
}

void testAdjGraph()
{
	cout << "testAdjGraph" << endl;

	AdjListGraph g0(4);
	g0.addEdge(0, 1);
	g0.addEdge(0, 2);
	g0.addEdge(1, 3);
	g0.addEdge(2, 3);
	g0.BFS(0);
	g0.DFSNonRecursive(0);

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

	AdjListGraph g4(4);
	g4.addEdge(0, 1);
	g4.addEdge(1, 2);
	g4.addEdge(2, 3);
	//g4.addEdge(2, 1);
	//g4.addEdge(3, 1);

	_ASSERT(!g4.isCyclic());

	AdjListGraph g5(5);
	g5.addEdgeUnDir(1, 0);
	g5.addEdgeUnDir(0, 2);
	g5.addEdgeUnDir(0, 3);
	g5.addEdgeUnDir(3, 4);
	//g5.addEdgeUnDir(1, 0);
	_ASSERT(!g5.isCyclicUndir());

	g5.addEdgeUnDir(1, 2);
	_ASSERT(g5.isCyclicUndir());

	AdjListGraph g6(6);
	g6.addEdge(5, 2);
	g6.addEdge(5, 0);
	g6.addEdge(4, 0);
	g6.addEdge(4, 1);
	g6.addEdge(2, 3);
	g6.addEdge(3, 1);
	g6.topologicalSort();

	AdjListGraph g7(4);
	string  words[] = { "baa", "abcd", "abca", "cab", "cad" };
	g7.alienDictionary(words, 5, 4);
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

int EdgeGraph::find(vector<int> parent, int i)
{
	// while(parent[i] != i) 
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

void EdgeGraph::KruskalMST()
{
	vector<Edge> result(V - 1);
	int e = 0;  // An index variable, used for result[] 
	int i = 0;  // An index variable, used for sorted edges 

	sort(edge.begin(), edge.end(), cmpEdge);

	vector<subset> subsets(V);
	for (int i = 0; i < V; ++i)
	{
		subsets[i].parent = i;
	}

	while (e < V - 1)
	{
		struct Edge nextEdge = edge[i++];

		int x = find(subsets, nextEdge.src);
		int y = find(subsets, nextEdge.dest);

		if (x != y)
		{
			result[e++] = nextEdge;

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
	}

	printf("Following are the edges in the constructed MST\n");
	for (i = 0; i < e; ++i)
		printf("%d -- %d == %d\n", result[i].src, result[i].dest,
			result[i].weight);
}

void EdgeGraph::BellmanFordSP(int src)
{
	int E = edge.size();

	vector<int> dist;
	dist.resize(V, INT_MAX);
	dist[src] = 0;

	// Relax all edges |V| - 1 times. A simple shortest path from src to any other vertex can have at-most |V| - 1  edges 
	for (int i = 1; i <= V - 1; i++)
	{
		for (int j = 0; j < E; j++)
		{
			int u = edge[j].src;
			int v = edge[j].dest;
			int weight = edge[j].weight;
			if (dist[u] != INT_MAX && dist[u] + weight < dist[v])
				dist[v] = dist[u] + weight;
		}
	}

	// check for negative-weight cycles.  The above step guarantees shortest distances if graph doesn't contain  
	// negative weight cycle.  If we get a shorter path, then there is a cycle. 
	for (int i = 0; i < E; i++)
	{
		int u = edge[i].src;
		int v = edge[i].dest;
		int weight = edge[i].weight;
		if (dist[u] != INT_MAX && dist[u] + weight < dist[v])
			printf("Graph contains negative weight cycle");
	}
}

bool cmpEdge(const EdgeGraph::Edge& a, const EdgeGraph::Edge& b)
{
	return a.weight < b.weight;
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

	EdgeGraph g2(4);
	g2.addEdge(EdgeGraph::Edge(0, 1, 10));
	g2.addEdge(EdgeGraph::Edge(0, 2, 6));
	g2.addEdge(EdgeGraph::Edge(0, 3, 5));
	g2.addEdge(EdgeGraph::Edge(1, 3, 15));
	g2.addEdge(EdgeGraph::Edge(2, 3, 4));

	g2.KruskalMST();

	EdgeGraph g3(5);
	g3.addEdge(EdgeGraph::Edge(0, 1, -1));
	g3.addEdge(EdgeGraph::Edge(0, 2, 4));
	g3.addEdge(EdgeGraph::Edge(1, 2, 3));
	g3.addEdge(EdgeGraph::Edge(1, 3, 2));
	g3.addEdge(EdgeGraph::Edge(1, 4, 2));
	g3.addEdge(EdgeGraph::Edge(3, 2, 5));
	g3.addEdge(EdgeGraph::Edge(3, 1, 1));
	g3.addEdge(EdgeGraph::Edge(4, 3, -3));
	g3.BellmanFordSP(0);

}

/////////////////////////////////////////////////////////////////////////////////
void testGraph()
{
	testAdjMatrixGraph();
	testAdjGraph();
	testEdgeGraph();
}

