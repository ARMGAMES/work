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
			// This is tricky. Consider the example. AAACAAAA and i = 7. 
			// The idea is similar to search step. 
			if (len != 0) {
				len = lps[len - 1];

				// Also, note that we do not increment i here 
			}
			else // if (len == 0) 
			{
				lps[i] = 0;
				i++;
			}
		}
	}
}

// O(n) in the worst case
void KMPSearch(const char* pat, const char* txt)
{
	int M = (int)strlen(pat);
	int N = (int)strlen(txt);

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
	std::cout << "test KMP searching\n";
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
	int m = (int)strlen(pat);
	int n = (int)strlen(txt);

	int badchar[NO_OF_CHARS];

	// Fill the bad character array
	badCharHeuristic(pat, m, badchar);

	// s is shift of the pattern with respect to text 
	int s = 0;
	while (s <= n - m)
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
	const char* pat1 = "ABCAB";
	const char* txt1 = "ABABDABACDABABCABAB";

	badCharacterSearch(pat1, txt1);

	const char* pat2 = "EDG";
	const char* txt2 = "EDGABDH";
	badCharacterSearch(pat2, txt2);
}

////////////////////////////////////////////////////////////////////////////////////
bool regularExpressionMatching(string s, string p)
{
	/**
		 * f[i][j]: if s[0..i-1] matches p[0..j-1]
		 * if p[j - 1] != '*'
		 *      f[i][j] = f[i - 1][j - 1] && s[i - 1] == p[j - 1]
		 * if p[j - 1] == '*', denote p[j - 2] with x
		 *      f[i][j] is true iff any of the following is true
		 *      1) "x*" repeats 0 time and matches empty: f[i][j - 2]
		 *      2) "x*" repeats >= 1 times and matches "x*x": s[i - 1] == x && f[i - 1][j]
		 * '.' matches any single character
		 */
	int m = s.size(), n = p.size();
	vector<vector<bool>> dp(m + 1, vector<bool>(n + 1, false));

	dp[0][0] = true;
	for (int i = 1; i <= m; i++)
		dp[i][0] = false;
	// p[0.., j - 3, j - 2, j - 1] matches empty iff p[j - 1] is '*' and p[0..j - 3] matches empty
	for (int j = 1; j <= n; j++)
		dp[0][j] = j > 1 &&  (p[j - 1] == '*') && dp[0][j - 2];

	for (int i = 1; i <= m; i++)
		for (int j = 1; j <= n; j++)
			if (p[j - 1] != '*')
				dp[i][j] = dp[i - 1][j - 1] && (p[j - 1] == s[i - 1] || p[j - 1] == '.');
			else
				// p[0] cannot be '*' so no need to check "j > 1" here
				dp[i][j] = dp[i][j - 2] || ((p[j - 2] == s[i - 1] || p[j - 2] == '.') && dp[i - 1][j]);

	return dp[m][n];
}

bool wildCardMatching(string s, string p) 
{
	int m = s.length(), n = p.length();
	int i = 0, j = 0, asterisk = -1, match;
	while (i < m) {
		if (j < n && p[j] == '*') {
			match = i;
			asterisk = j++;
		}
		else if (j < n && (s[i] == p[j] || p[j] == '?')) {
			i++;
			j++;
		}
		else if (asterisk >= 0) {
			i = ++match;
			j = asterisk + 1;
		}
		else return false;
	}
	while (j < n && p[j] == '*') j++;
	return j == n;
	string l;
}

void testRegularExpressionMatching()
{
	string s = "aab";
	string p = "a*b*";
	_ASSERT(regularExpressionMatching(s, p));
};

////////////////////////////////////////////////////////////////////////////////////

bool isValidNumber(string str)
{
	vector<vector<int>> transTable =
	{ //+/-, 0-9, ., e, ' '
		{1, 2, 3, -1, 0}, // s0
		{-1, 2, -1, -1, -1}, // s1
		{-1, 2, 4, 5, -1}, // s2

	};

	int state = 0;
	for (int i = 0; i < str.length(); i++)
	{
		if (str[i] == '+' || str[i] == '-')
		{
			if (state == 0 || state == 5)
				state++;
			else return false;
		}
		else if (str[i] >= '0' && str[i] <= '9')
		{
			if (state <= 2)
				state = 2;
			else if (state == 3 || state == 4)
				state = 4;
			else if (state == 5 || state == 6 || state == 7)
				state = 7;
			else
				return false;
		}
		else if (str[i] == '.')
		{
			if (state == 0)
				state = 3;
			else if (state == 2)
				state == 4;
			else return false;
		}
		else if (str[i] == 'e')
		{
			if (state == 2 || state == 4)
				state = 5;
			else return false;
		}
		else if (str[i] == ' ')
		{
			if (state != 0)
				return false;
		}
	}
	return (state == 2 || state == 4 || state == 7);
}

void testIsValidNumber()
{
	string s1 = ".0";
	_ASSERT(isValidNumber(s1));

	string s2 = ".";
	_ASSERT(!isValidNumber(s2));

	string s3 = " -123";
	_ASSERT(isValidNumber(s3));

	string s4 = "+23e.";
	_ASSERT(!isValidNumber(s4));

}

////////////////////////////////////////////////////////////////////////////////////
void testSearching()
{
	std::cout << "Test testSearching\n";

	testKMPAlgorithm();

	testBadCharacterMatching();

	testRegularExpressionMatching();

	testIsValidNumber();

	std::cout << "Test testSearching completed\n";
}