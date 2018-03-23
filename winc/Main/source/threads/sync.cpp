#include "stdafx.h"

volatile LONG g_Count(0);
CRITICAL_SECTION g_cs;

const int COUNT = 10;
int g_nSum = 0;

DWORD WINAPI thread_proc(void* data)
{
  //InterlockedExchangeAdd(&g_Count, 2);

  EnterCriticalSection(&g_cs);
  g_nSum = 0;
  for (int n = 1; n <= COUNT; n++) 
  {
    g_nSum += n;
  }
  printf("total sum = %d\n", g_nSum);
  LeaveCriticalSection(&g_cs);

  return 1;
}

void test_synchronization()
{
  const UINT32 kThreadNum(2);
  std::vector<HANDLE> handles(kThreadNum);

  InitializeCriticalSection(&g_cs);

  for (int i(0); i < kThreadNum; ++i)
  {
    handles[i] = CreateThread(NULL, 0, &thread_proc, NULL, 0, NULL);
  }

  DWORD ret(WaitForMultipleObjects(kThreadNum, &(handles[0]), true, INFINITE));

  switch (ret)
  {
  case WAIT_OBJECT_0 :
  case WAIT_ABANDONED_0:
    printf("all threads finished\n");
    break;
  case WAIT_FAILED:
  case WAIT_TIMEOUT:
    printf("error: failed to wait!");
    break;
  default:
    break;
  }

  DeleteCriticalSection(&g_cs);

  printf("global count is: %Ld\n", g_Count);

}
