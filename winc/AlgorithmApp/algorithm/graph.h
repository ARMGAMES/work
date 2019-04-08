
#pragma once

#include "constants.h"
#include "radix_search.h"

using namespace std;
using namespace RadixSearch;

namespace GRAPH
{

	struct Edge
	{
		int v, w;
		Edge(int v = -1, int w = -1) :v(v), w(w) {}
	};

	/************************************************************************/
	/* For dense graphs, when E and V*V are comparable, we prefer the matrix
	/*
	/************************************************************************/
	class DenseGraph
	{
	private:
		int Vcnt, Ecnt;
		bool digraph;
		vector<vector <bool>> adj;

	public:
		/*
		int - maximum possible number of vertices
		bool - directed or not
		*/
		DenseGraph(int V, bool digraph = false) :adj(V), Vcnt(V), Ecnt(0), digraph(digraph)
		{
			for (int i = 0; i < V; ++i)
			{
				adj[i].assign(V, false);
			}
		}
		~DenseGraph() {}

		int V() const { return Vcnt; }
		int E() const { return Ecnt; }

		bool directed() const { return digraph; }
		void insert(Edge e)
		{
			int v = e.v, w = e.w;
			if (adj[v][w] == false) Ecnt++;
			adj[v][w] = true;
			if (!digraph) adj[w][v] = true;
		}

		void remove(Edge e)
		{
			int v = e.v, w = e.w;
			if (adj[v][w] == true) Ecnt--;
			adj[v][w] = false;
			if (!digraph) adj[w][v] = false;
		}

		bool edge(int v, int w)
		{
			return adj[v][w];
		}

		class adjIterator
		{
		private:
			const DenseGraph& G;
			int i, v;
		public:
			adjIterator(const DenseGraph& G, int v) : G(G), v(v), i(-1)
			{}

			int beg()
			{
				i = -1; return nxt();
			}
			int nxt()
			{
				for (i++; i < G.V(); i++)
					if (G.adj[v][i] == true) return i;
				return -1;
			}
			bool end()
			{
				return i >= G.V();
			}
		};
	};

	/************************************************************************/
	/* For  sparse  graphs,  when V*V is huge compared to V + E,  we prefer the lists
	/************************************************************************/
	class SparseMultiGraph
	{
		struct node
		{
			int v;
			node* next;
			node(int x, node* t)
			{
				v = x;
				next = t;
			}
		};
		typedef node* link;
		vector<link> adj;

	private:
		int Vcnt, Ecnt;
		bool digraph;

	public:
		SparseMultiGraph(int V, bool digraph = false) : adj(V), Vcnt(V), Ecnt(0), digraph(digraph)
		{
			adj.assign(V, 0);
		}

		int V() const { return Vcnt; }
		int E() const { return Ecnt; }

		void insert(Edge e)
		{
			int v = e.v, w = e.w;
			adj[v] = new node(w, adj[v]);
			if (!digraph) adj[w] = new node(v, adj[w]);
			Ecnt++;
		}

		void remove(Edge e)
		{
			int v = e.v, w = e.w;
			if (adj[v] == NULL) return;

			if (adj[v]->v == w)
			{
				link t = adj[v];
				adj[v] = t->next;
				delete t;
			}
			else
			{
				link t = adj[v];
				while (t->next != NULL)
				{
					if (t->next->v == w)
					{
						link del = t->next;
						t->next = del->next;
						delete del;
					}
					t = t->next;
				}
			}

			if (!digraph)
			{
				// add v to w
			}
		}

		bool edge(int v, int w) const
		{
			if (adj[v] == NULL) return false;
			link t = adj[v];
			while (t != NULL)
			{
				if (t->v == w)
					return true;
				t = t->next;
			}

		}

		class adjIterator
		{
			const SparseMultiGraph &G;
			int v;
			link t;
		public:
			adjIterator(const SparseMultiGraph &G, int v) : G(G), v(v) { t = 0; }

			int beg()
			{
				t = G.adj[v]; return t ? t->v : -1;
			}
			int nxt()
			{
				if (t) t = t->next; return t ? t->v : -1;
			}
			bool end()
			{
				return t == 0;
			}
		};
	};

	/************************************************************************/
	/* Graph IO interface                                                   */
	/************************************************************************/
	template <class Graph>
	class IO
	{
	public:
		static void show(const Graph& g)
		{
			typedef typename Graph::adjIterator Iterator;

			for (int v = 0; v < g.V(); v++)
			{
				cout.width(2);
				cout << v << ":";
				Iterator a(g, v);
				for (int t = a.beg(); !a.end(); t = a.nxt())
				{
					cout.width(2);
					cout << t << " ";
				}
				cout << endl;
			}
		}
		static void scanEZ(Graph &);
		static void scan(Graph& g)
		{
			string v, w;
			StringST st;
			while (cin >> v >> w)
				g.insert(Edge(st.index(v), st.index(w)));
		}
	};

	/************************************************************************/
	/* graph connectivity interface                                      */
	/************************************************************************/
	template <class Graph>
	class CC
	{
		const Graph &G;
		int ccnt;
		vector <int> id;

		void ccR(int w)
		{
			id[w] = ccnt;
			typedef typename Graph::adjIterator Iterator;
			Iterator A(G, w);
			for (int v = A.beg(); !A.end(); v = A.nxt())
				if (id[v] == -1) ccR(v);
		}
	public:
		CC(const Graph &G) : G(G), ccnt(0), id(G.V(), -1)
		{
			for (int v = 0; v < G.V(); v++)
				if (id[v] == -1) { ccR(v); ccnt++; }
		}
		int count() const { return ccnt; }
		bool connect(int s, int t) const
		{
			return id[s] == id[t];
		}
	};


	/************************************************************************/
	/*   Simple search for graph                                                                   */
	/************************************************************************/
	template <class Graph>
	class sPATH
	{
		const Graph &G;
		vector <bool> visited;
		bool found;
		bool searchR(int v, int w)
		{
			if (v == w) return true;
			visited[v] = true;
			typedef typename Graph::adjIterator Iterator;
			Iterator A(G, v);
			for (int t = A.beg(); !A.end(); t = A.nxt())
				if (!visited[t])
					if (searchR(t, w)) return true;
			return false;
		}
	public:
		sPATH(const Graph &G, int v, int w) : G(G), visited(G.V(), false)
		{
			found = searchR(v, w);
		}
		bool exists() const
		{
			return found;
		}
	};

	/************************************************************************/
	/* Depth First Search                                                                     */
	/************************************************************************/
	template <class Graph>
	class cDFS
	{
		int cnt;
		const Graph& g;
		vector<int> ord;
		void searchC(int v)
		{
			ord[v] = cnt++;
			typedef typename Graph::adjIterator Iterator;
			Iterator A(g, v);
			for (int t = A.beg(); !A.end(); t = A.nxt())
			{
				if (ord[t] == -1) searchC(t);
			}
		}

	public:
		cDFS(const Graph& g, int v = 0) :g(g), cnt(0), ord(g.V(), -1)
		{
			searchC(v);
		}
		int operator[](int v) const { return ord[v]; }
		int count() const { return cnt; }
	};



	/************************************************************************/
	/* Graph search ADT                                                                     */
	/************************************************************************/
	template <class Graph>
	class SEARCH
	{
	protected:
		const Graph &G;
		int cnt;
		vector <int> ord;  // order of each vertex visited
		virtual void searchC(Edge) = 0;
		void search()
		{
			for (int v = 0; v < G.V(); v++)
				if (ord[v] == -1)
					searchC(Edge(v, v));
		}
	public:
		SEARCH(const Graph &G) : G(G), ord(G.V(), -1), cnt(0) { }
		int operator[](int v) const { return ord[v]; }
	};

	/************************************************************************/
	/*  DFS implements spanning tree(st) with parent links                                                                   */
	/************************************************************************/
	template <class Graph>
	class DFS : public SEARCH<Graph>
	{
		vector<int> st; // construct a spanning tree, points to parent links.  If index and value are same, its root vertex
		void searchC(Edge e)
		{
			int w = e.w;
			ord[w] = cnt++;
			st[e.w] = e.v;
			typedef typename Graph::adjIterator Iterator;
			Iterator A(G, w);
			for (int t = A.beg(); !A.end(); t = A.nxt())
				if (ord[t] == -1)
					searchC(Edge(w, t));
		}
	public:
		DFS(const Graph& G) :SEARCH<Graph>(G), st(G.V(), -1)
		{
			//search();
		}
		int ST(int v) const { return st[v]; }
	};
	/************************************************************************/
	/*  Two-way Euler tour                                                                    */
	/************************************************************************/
	template <class Graph>
	class EULER : public SEARCH<Graph>
	{
		void searchC(Edge e)
		{
			int v = e.v, w = e.w;
			ord[w] = cnt++;
			cout << "-" << w;
			typedef typename Graph::adjIterator Iterator;
			Iterator A(G, w);
			for (int t = A.beg(); !A.end(); t = A.nxt())
				if (ord[t] == -1)
					searchC(Edge(w, t));
				else if (ord[t] < ord[v])
					cout << "-" << t << "-" << w;

			if (v != w)
				cout << "-" << v;
			else
				cout << endl;
		}
	public:
		EULER(const Graph &G) : SEARCH<Graph>(G)
		{
			//search();
		}
	};
	/************************************************************************/
	/*   Edge connectivity (bridge problem)                                                                   */
	/************************************************************************/
	template <class Graph>
	class EC : public SEARCH<Graph>
	{
		int bcnt;
		vector<int> low;  // keep track of the lowest preoder number referenced by any back edge in the subtree rooted at the vertex
		void searchC(Edge e)
		{
			int w = e.w;
			ord[w] = cnt++;
			low[w] = ord[w];
			typedef typename Graph::adjIterator Iterator;
			Iterator A(G, w);
			for (int t = A.beg(); !A.end(); t = A.nxt())
			{
				if (ord[t] == -1)
				{
					searchC(Edge(w, t));
					if (low[w] > low[t])
						low[w] = low[t];
					if (low[t] == ord[t])
						bcnt++;  // w-t is a bridge
				}
				else if (t != e.v)
				{
					if (low[w] > ord[t])
						low[w] = ord[t];
				}
			}
		}
	public:
		EC(const Graph& G) :SEARCH<Graph>(G), bcnt(0), low(G.V(), -1)
		{
			//search();
		}
		int count() const { return bcnt; }
	};

	/************************************************************************/
	/* Breadth first search                                                                     */
	/************************************************************************/
	template <class Graph>
	class BFS : public SEARCH<Graph>
	{
		vector<int> st;  // spanning tree
		void searchC(Edge e)
		{
			std::queue<Edge> Q;
			Q.push(e);
			while (!Q.empty())
			{
				Edge t = Q.front();
				Q.pop();
				if (ord[t.v] == -1)
				{
					int v = t.v, w = t.w;
					ord[w] = cnt++; st[w] = v;
					typedef typename Graph::adjIterator Iterator;
					Iterator A(G, w);
					for (int t = A.beg(); !A.end(); t = A.nxt())
					{
						if (ord[t] == -1) Q.push(Edge(w, t));
					}
				}
			}
		}
	public:
		BFS(Graph& G) : SEARCH<GRAPH>(G), st(G.V(), -1)
		{
			//search();
		}
		int ST(int v) const { return st[v]; }
	};

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	template <class Graph>
	vector <Edge> edges(Graph &G);
	/************************************************************************/
	/* Random graph generator                                                                     */
	/************************************************************************/
	void randE(DenseGraph& G, int E);

	void randG(DenseGraph &G, int E);

} // namespace GRAPH

