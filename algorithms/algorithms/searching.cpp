#include "pch.h"
#include "searching.h"

using namespace std;

int STHashing::searchSCR(link h, int key)
{
	link l = h;
	while (l != 0)
	{
		if (l->value == key)
		{
			return key;
		}
			// found match
		l = l->next;
	}
	return -1;
}


void testStringTrie()
{
	std::cout << "TestStringTrie\n";
	StringTrie stringTrie1;

	stringTrie1.insertNonRecursive("a");
	stringTrie1.insertNonRecursive("this");
	stringTrie1.insertNonRecursive("little");
	stringTrie1.insertNonRecursive("longgest");
	stringTrie1.insertNonRecursive("the");
	stringTrie1.insertNonRecursive("there");

	_ASSERT(stringTrie1.searchNonRecursive("the"));
	_ASSERT(!stringTrie1.searchNonRecursive("thee"));
	_ASSERT(!stringTrie1.searchNonRecursive(""));
	_ASSERT(!stringTrie1.searchNonRecursive("t"));

	StringTrie stringTrie2;
	stringTrie2.insert("a");
	stringTrie2.insert("this");
	stringTrie2.insert("little");
	stringTrie2.insert("longgest");
	stringTrie2.insert("the");
	stringTrie2.insert("there");

	_ASSERT(stringTrie2.search("the"));
	_ASSERT(!stringTrie2.search("thee"));
	_ASSERT(!stringTrie2.search(""));
	_ASSERT(!stringTrie2.search("t"));

}

void testStringTST()
{
	std::cout << "testStringTST\n";
	StringTST stringTST1;
	
	stringTST1.insert("a");
	stringTST1.insert("aa");
	stringTST1.insert("ab");
	stringTST1.insert("this");
	stringTST1.insert("little");
	stringTST1.insert("longgest");
	stringTST1.insert("the");
	stringTST1.insert("there");

	_ASSERT(stringTST1.search("the"));
	_ASSERT(stringTST1.search("ab"));
	_ASSERT(!stringTST1.search("thee"));
	_ASSERT(!stringTST1.search(""));
	_ASSERT(!stringTST1.search("t"));
}

//////////////////////////////////////////////////////////////////////////////

void computeLPSArray(const char* pat, int M, int* lps)
{
	// length of the previous longest prefix suffix 
	int len = 0;

	lps[0] = 0; // lps[0] is always 0 

	// the loop calculates lps[i] for i = 1 to M-1 
	int i = 1;
	while (i < M) {
		if (pat[i] == pat[len]) {
			len++;
			lps[i] = len;
			i++;
		}
		else // (pat[i] != pat[len]) 
		{
			// This is tricky. Consider the example. 
			// AAACAAAA and i = 7. The idea is similar 
			// to search step. 
			if (len != 0) {
				len = lps[len - 1];

				// Also, note that we do not increment 
				// i here 
			}
			else // if (len == 0) 
			{
				lps[i] = 0;
				i++;
			}
		}
	}
}

void KMPSearch(const char* pat, const char* txt)
{
	int M = strlen(pat);
	int N = strlen(txt);

	int *lps = new int[M];
	memset(lps, 0, sizeof(int)*M);

	computeLPSArray(pat, M, lps);

	int i = 0; // index for txt[] 
	int j = 0; // index for pat[] 
	while (i < N) {
		if (pat[j] == txt[i]) {
			j++;
			i++;
		}

		if (j == M) {
			printf("Found pattern at index %d \n", i - j);
			j = lps[j - 1];
		}

		// mismatch after j matches 
		else if (i < N && pat[j] != txt[i]) {
			// Do not match lps[0..lps[j-1]] characters, 
			// they will match anyway 
			if (j != 0)
				j = lps[j - 1];
			else
				i = i + 1;
		}
	}
	delete[]lps;
}

void testKMPAlgorithm()
{
	std::cout << "testKMPAlgorithm\n";
	const char* pat = "ABABCABAB";
	const char* txt = "ABABDABACDABABCABAB";
	
	
	KMPSearch(pat, txt);
}

////////////////////////////////////////////////////////////////////////////////////

void badCharHeuristic(const char* pat, int size, int badChar[NO_OF_CHARS])
{
	for (int i = 0; i < NO_OF_CHARS; ++i)
		badChar[i] = -1;

	// Fill the actual value of last occurrence of a character  
	for (int i = 0; i < size; ++i)
		badChar[(int)*(pat + i)] = i;

}

void badCharacterSearch(const char* pat, const char* txt)
{
	int m = strlen(pat);
	int n = strlen(txt);

	int badchar[NO_OF_CHARS];

	// Fill the bad character array
	badCharHeuristic(pat, m, badchar);

	// s is shift of the pattern with respect to text 
	int s = 0;
	while (s < n - m)
	{
		int j =  m -1;
		/* Keep reducing index j of pattern while characters of pattern and text are matching at this shift s */
		while (j >= 0 && *(txt + s + j) == *(pat + j))
		{
			j--;
		}

		if (j < 0)
		{
			cout << "pattern occurs at shift = " << s << endl;
			/* Shift the pattern so that the next character in text aligns with the last
			occurrence of it in pattern. The condition s+m < n is necessary for
			the case when pattern occurs at the end	of text */
			s += (s + m < n) ? (m - badchar[(int)*(txt + s + m)]) : 1;
		}
		else
		{
			/* Shift the pattern so that the bad character in text aligns with the last occurrence of
			it in pattern. The max function is used to make sure that we get a positive shift.
			We may get a negative shift if the last	occurrence of bad character in pattern
			is on the right side of the current	character. */
			s += max(1, j - badchar[(int)*(txt + s + j)]);
		}
	}
}

void testBadCharacterMatching()
{
	std::cout << "testBadCharacterMatching\n";
	const char* pat = "ABCAB";
	const char* txt = "ABABDABACDABABCABAB";

	badCharacterSearch(pat, txt);
}

////////////////////////////////////////////////////////////////////////////////////

void testSearching()
{
	testStringTrie();
	testStringTST();
	testKMPAlgorithm();
	testBadCharacterMatching();
}