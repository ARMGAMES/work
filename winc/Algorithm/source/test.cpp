
#include "stdafx.h"


using namespace GRAPH;

void test_linkedlist()
{
	LinkedList mylist;

	mylist.reverse();
}

void test_bitalgorithm()
{
	char test = 0x88;
	BitAlgorithm::CountBit1(test);

	//byte test_arr[] = {0x88, 0xFF};
	//CountBit1_arr(test_arr, 2);

	int int1 = 0x89;
	int x = BitAlgorithm::bitsinint(int1);

	std::cout << x;

}

void test_stringsearch()
{
	char a[] = "abcdefghijklbcdmnopqrst";
	char p[] = "bcd";

	string_search(a, p);
}


void test_containers()
{
	test_adtobject();
}

void sort_testing()
{
	// create random array number for testing
  int testarray[SORT_MAX] = {18, 28, 8, 1, 35, 44, 3, 15, 59, 31, 21, 7, 14, 4, 9};

	/*for(int i = 0; i < SORT_MAX; i++)
	{
		testarray[i] = 1000 * (1.0*rand()/RAND_MAX);
	}*/

	// selction sort.
	//selection_sort(testarray, 0, SORT_MAX-1);

	insertion_sort_exch(testarray, 0, SORT_MAX-1);

  //bubble_sort(testarray, 0, SORT_MAX-1);

  //shell_sort(testarray, 0, SORT_MAX-1);

  //quicksort_stack(testarray, 0, SORT_MAX-1);

  //mergesort(testarray, 0, SORT_MAX-1);

  int v = R;

  for(int i = 0; i < SORT_MAX; i++)
  {
    cout << testarray[i] << endl; 
  }
}


void test_search()
{
	//bst_test();
	search::array_insertion_test();
}

/************************************************************************/
/* Test radix search (TST, multi-Trie)
/************************************************************************/
void test_radix_search()
{
	printf("Test radix search begin---\n");

	bool res;
	int index;
	RadixSearch::TST st(100);

	st.insert("ab");
	st.insert("aa");
	st.insert("aaa");
	//st.insert("happy");

	res = st.search("a");
	res = st.search("aa");
	res = st.search("aaaa");
	//st.match("*el**");

	RadixSearch::StringST sst;
	index = sst.index("ab");
	index = sst.index("aa");
	index = sst.index("aaa");
	index = sst.index("aa");
	index = sst.index("a");


	printf("Test radix search end---\n");

}


/************************************************************************/
/* Test graph functions
/************************************************************************/
void test_graph()
{
	printf("Test Graph begin---\n");

	DenseGraph dg(10);
	SparseMultiGraph sg(10);

	randE(dg, 20);
	IO<DenseGraph>::show(dg);
	//GRAPH::IO<GRAPH::DenseGraph>::scan(g);
	//sPATH<DenseGraph> found(dg, 1, 4);
	//DFS<DenseGraph> dfs(dg);
	EULER<DenseGraph> euler(dg);

	printf("Test Graph end---\n");
}


void test_entry()
{
	//test_bitalgorithm();
	//test_linkedlist();
	//test_stringsearch();
	test_containers();	
	//sort_testing();
	//test_search();
	//test_radix_search();
	//test_graph();
}