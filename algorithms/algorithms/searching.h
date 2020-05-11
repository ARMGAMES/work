#pragma once

#include "sharedstructs.h"
using namespace std;

////////////////////////////////////////////////////////////////////
// Hashing
// seperate chaining
class STHashing
{
private:
	struct node
	{
		int value;
		node *next;
		node(int v)
		{
			value = v;
			next = 0;
		}
		node(int v, node* next_)
		{
			value = v;
			next = next_;
		}
	};
	typedef node* link;
	link* heads;
	int N, M;
	int searchSCR(link h, int v);

public:
	STHashing(int maxN)
	{
		N = 0; M = maxN / 5;
		heads = new link[M];
		for (int i = 0; i < M; i++) heads[i] = 0;
	}
	int searchSC(int v)
	{
		return searchSCR(heads[hash1(v, M)], v);
	}
	void insert(int item)
	{
		int i = hash1(item, M);
		heads[i] = new node(item, heads[i]); 
		N++;
	}
};


///////////////////////////////////////////////////////////////////////////////////
// KMP Algorithm for Pattern Searching
// 
// Fills lps[] for given pattern pat[0..M-1] 
void computeLPSArray(const char* pat, int M, int* lps);
void KMPSearch(const char* pat, const char* txt);
void testKMPAlgorithm();

/////////////////////////////////////////////////////////////////////////////////////
//  Bad Character Heuristic of Boyer Moore String Matching Algorithm 
const int NO_OF_CHARS = 256;

void badCharHeuristic(const char* pat, int size, int badChar[NO_OF_CHARS]);
void badCharacterSearch(const char* pat, const char* txt);
void testBadCharacterMatching();

/////////////////////////////////////////////////////////////////////////////////////
//Regular Expression Matching
bool regularExpressionMatching(string s, string p);

bool wildCardMatching(string s, string p);
/////////////////////////////////////////////////////////////////////////////////////
// Valid Number: DFA
bool isValidNumber(string s);

/////////////////////////////////////////////////////////////////////////////////////

void testSearching();