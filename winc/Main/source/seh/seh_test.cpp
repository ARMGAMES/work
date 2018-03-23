
#include "stdafx.h"

/*
Very tricky try finally code.  Avoid use returns, continues, breaks 
in try/finally
*/
int test_tryfinal()
{
	DWORD dwTemp = 0;
	while (dwTemp < 10) 
	{
		__try {
			if (dwTemp == 2)
				continue;
			if (dwTemp == 3)
				break;
		}
		// final block always execute when exit try
		__finally {
			dwTemp++;
		}
		dwTemp++;
	}
	dwTemp += 10;
	return(dwTemp);
}

/*
Best practice for try finally to clean up all resources
*/
BOOL tryfinal_best_practice()
{
	// IMPORTANT: Initialize all variables to assume failure.
	HANDLE hFile(INVALID_HANDLE_VALUE);
	PVOID pvBuf(NULL);

	__try 
	{
		DWORD dwNumBytesRead;
		BOOL bOk;
		hFile = CreateFile(TEXT("SOMEDATA.DAT"), GENERIC_READ,
			FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE) {
			return(FALSE);
		}

		pvBuf = VirtualAlloc(NULL, 1024, MEM_COMMIT, PAGE_READWRITE);
		if (pvBuf == NULL) {
			return(FALSE);
		}

		bOk = ReadFile(hFile, pvBuf, 1024, &dwNumBytesRead, NULL);
		if (!bOk || (dwNumBytesRead != 1024)) {
			return(FALSE);
		}
		// Do some calculation on the data.
	}
	__finally {
		// Clean up all the resources.
		if (pvBuf != NULL)
			VirtualFree(pvBuf, 1024, MEM_RELEASE | MEM_DECOMMIT);
		if (hFile != INVALID_HANDLE_VALUE)
			CloseHandle(hFile);
	}
	// Continue processing.
	return(TRUE);

}

template <typename T>
int fun1(const T& p)
{
  typedef typename T::const_iterator map_iterator;

  map_iterator it(p.find("aaa"));

  return 1;
}

template
int fun1<string_set_map<true>::type >(const string_set_map<true>::type & p);

template
int fun1<string_set_map<false>::type >(const string_set_map<false>::type & p);
