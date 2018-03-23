
#include "stdafx.h"

using namespace std;

namespace GRAPH
{

template <class Graph>
vector <Edge> edges(Graph &G)
{ 
  int E = 0;
	vector <Edge> a(G.E());
	for (int v = 0; v < G.V(); v++)
	{
		typename Graph::adjIterator A(G, v);
		for (int w = A.beg(); !A.end(); w = A.nxt())
			if (G.directed() ||v<w)
				a[E++] = Edge(v, w);
	}
	return a;
}

void randE(DenseGraph& G, int E)
{
	for (int i = 0;i<E; i++)
	{
		int v = int(G.V()*rand()/(1.0+RAND_MAX));
		int w = int(G.V()*rand()/(1.0+RAND_MAX));
		G.insert(Edge(v,w));
	}
}

void randG(DenseGraph &G, int E)
{ 
	double p = 2.0*E/G.V()/(G.V()-1);
	for (int i = 0; i < G.V(); i++)
		for (int j = 0; j < i; j++)
			if (rand() < p*RAND_MAX)
				G.insert(Edge(i, j));
}

}