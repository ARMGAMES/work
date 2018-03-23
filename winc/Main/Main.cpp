// Main.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "data/crc_test.h"
#include "leetcode/leetcodetest.h"

#pragma comment (lib, "Ws2_32.lib")

void DedicatedThreadIO()
{
	NTSTATUS error(ERROR_SUCCESS);
	WriteThread writethread(error);
	if (error != ERROR_SUCCESS)
	{
		printf("failed to create write thread. %s", __FUNCTION__);
	}

	char buffer[128];

	for (int i = 0; i < 5; i++)
	{
		printf(buffer, "this is a test %d", i);
		WriteData writedata;
		writedata.buffer = buffer;
		writedata.length = 20;
		writedata.type = IOTYPE_COMPLETE;

		writethread.AddWriteData(&writedata);
	}

	writethread.CancelWriteThread();

	printf("Program completed.");

}


void WindowsSystemInfor()
{
	SYSTEM_INFO sysinfo;
	OSVERSIONINFOA osver;

	osver.dwOSVersionInfoSize = sizeof(osver);

	GetSystemInfo(&sysinfo);
	GetVersionExA(&osver);

	DWORD pagesize = sysinfo.dwPageSize;
}


void TestDll()
{
	//int 
	//printf("result = %d", result);

	TCHAR dllpath[] = _T("AlgorithmDLL.dll");
	HMODULE hDLL = LoadLibraryW(dllpath);

	if (hDLL == NULL)
	{
		printf ("failed to open algorithm dll");
	}

	typedef int (*pGetInt)();
	typedef int (*pAddTwo)(int, int);

	pGetInt pfnGetInt = (pGetInt)GetProcAddress(hDLL, "GetInt");
	pAddTwo pfnAddTwo = (pAddTwo)GetProcAddress(hDLL, "AddTwo");

	int result;
	if (pfnGetInt != NULL)
	{
		result = pfnGetInt();    
	}

	if (pfnAddTwo != NULL)
	{
		result = pfnAddTwo(3, 6);    
	}

	FreeLibrary(hDLL);

	printf("dll freed");
}


const char WHITE_SPACE = ' ';
const char QUOTE_MARK = '"';

bool tokenizeQuoted(const std::string& input, std::vector<std::string>& tokens, const char& delimiter)
{
  size_t i(0), start(0);
  bool quote(false);

  for (;;)
  {
    if (input.size() <= i)
    {
      if (quote)
      {
        // unclosed quote
        return false;
      }
      if (start < i)
      {
        tokens.push_back(std::string(input, start, i - start));
      }
      return true;
    }
    else if ((delimiter == input[i]) || (WHITE_SPACE == input[i]))
    {
      if (quote)
      {
        ++i;
        continue;
      }
      if (start < i)
      {
        tokens.push_back(std::string(input, start, i - start));
      }
      for (++i; (i < input.size()) && (WHITE_SPACE == input[i]); ++i)
      {
      }
      start = i;
    }
    else if (QUOTE_MARK == input[i])
    {
      if (quote)
      {
        quote = false;
        tokens.push_back(std::string(input, start, i - start));
        for (++i; (i < input.size()) && (WHITE_SPACE == input[i]); ++i)
        {
        }
      }
      else
      {
        quote = true;
        if (start < i)
        {
          tokens.push_back(std::string(input, start, i - start));
        }
        ++i;
      }
      start = i;
    }
    else
    {
      ++i;
    }
  }
  return true;
}

void WinAPITest(std::wstring param)
{
  u_int res;

  res = GetDriveTypeW(param.c_str());

  printf("%d", res);

}


void CodingActivity()
{
	// need to find largest continuous array of 0s.
	string input;

	int N;
	getline(cin, input);
	N = atoi(input.c_str());

	getline(cin, input);

	int* a = new int[N];
	for (int i = 0; i < N; i++)
	{
		if (input[2*i] == '1')
		{
			a[i] = 1;
		}
		else
		{
			a[i] = 0;
		}
	}

	int max_num = 0;
	int max_start = 0;
	int max_end = 0;

	int curr_num = 0;
	int curr_start = 0;

	for (int i = 0; i < N; ++i)
	{
		if (curr_num < 0)
		{
			curr_num = 0;
			curr_start = i;
		}

		// increment curr_num if its 0
		if (a[i] == 0)
		{
			curr_num++;
		}
		else
		{
			curr_num--;
		}

		// update max_num
		if (curr_num > max_num)
		{
			max_num = curr_num;
			max_start = curr_start;
			max_end = i;
		}

	}

	int count = 0;

	for (int i = 0; i < N; ++i)
	{
		if (i >= max_start && i <= max_end)
		{
			a[i] = (a[i] == 1? 0 : 1);
		}
		if (a[i] == 1)
		{
			count++;
		}
	}
	cout << count;

	delete[] a;

}
/************************************************************************/
/*                                                                      */
/************************************************************************/

 struct a1
{
  int a2;
  short a;
  int b2;
};

__declspec(align(32)) struct aType 
{
  int a[12]; 
};


void test_aligned()
{
  char buffer[] = "------------------------";
  void * pt = buffer;
  void * retpt(NULL);
  std::size_t space = sizeof(buffer)-1;
  while ( retpt = std::align(4,sizeof(char),pt,space) ) 
  {
    char* temp = static_cast<char*>(pt);
    *temp='*'; ++temp; space-=sizeof(char);
    pt=temp;
  }
  std::cout << buffer << '\n';

  int i = sizeof(a1);

  int j(sizeof(aType));
}
/************************************************************************/
/*                                                                      */
/************************************************************************/

void TestMemoryMappedFile()
{
  //1 create or opening a file kernel object
  LPCWCHAR filename = L"C:\\Users\\Eric\\Liu\\MyWorks\\EVault\\Acknowledgements_Quantum_RPMs.txt";
  HANDLE handle = CreateFileW(filename, 
                              GENERIC_READ | GENERIC_WRITE, 
                              FILE_SHARE_READ | FILE_SHARE_WRITE, 
                              NULL, 
                              OPEN_EXISTING, 
                              0, 
                              NULL);

  DWORD err(GetLastError());
  if (handle == INVALID_HANDLE_VALUE)
  {
    printf ("failed to open file : %s", filename);
  }

  HANDLE mapping_handle = CreateFileMapping(handle, 
                                            NULL,
                                            PAGE_WRITECOPY,
                                            0,
                                            0,
                                            NULL);

  if (mapping_handle == INVALID_HANDLE_VALUE)
  {
    printf ("failed to open file : %s", filename);
  }

  PBYTE pbfile = (PBYTE) MapViewOfFile(mapping_handle, FILE_MAP_COPY, 0, 0, 0);

  BYTE bSomeByte = pbfile[0];

  pbfile[0] = 'B';

  UnmapViewOfFile(pbfile);

  // Clean up after ourselves.
  CloseHandle(mapping_handle);
  CloseHandle(handle);

}

/************************************************************************/
/*                                                                      */
/************************************************************************/
DWORD FuncaDoodleDoo() {
 DWORD dwTemp = 0;
 while (dwTemp < 10) 
 {
  __try 
  {
    if (dwTemp == 2)
      continue;
    if (dwTemp == 3)
      break;
  }
  __finally 
  {
    dwTemp++;
  }
  dwTemp++;
 }

 dwTemp += 10;
 return(dwTemp);
}

void test_main()
{
  //int i = FuncaDoodleDoo();

  std::array<char, 10> buff;
  buff[1] = 'c';
  printf("Finished test main");
}
/************************************************************************/
/*                                                                      */
/************************************************************************/
int _tmain(int argc, _TCHAR* argv[])
{
	//DedicatedThreadIO();
	// TestEvent();

	//WindowsSystemInfor();

	//TestDll();

	//test_tryfinal();

  //int res = fun1(mymap);
	//printf("main finished");

	//BitAlgorithm::TestBitAlgorithm();

  //std::wstring param(argv[1]);
  //WinAPITest(param);

	//CodingActivity();

  //test_entry();

	//winlib::GetEnvironmentVariables();

 //test_fwrite();

  //test_aligned();
  
  //TestMemoryMappedFile();

  //test_synchronization();
  
  //test_crc();

  //test_main();
  
  //test_tcp();
  
	leetCodeTest();

	return 0;
}
