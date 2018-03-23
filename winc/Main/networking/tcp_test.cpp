
#include "stdafx.h"
#include "tcp_test.h"



#pragma comment(lib, "ws2_32.lib")

void test_tcp()
{
  WSADATA wsaData;
  int iResult;

  DWORD dwError;
  int i = 0;

  struct hostent *remoteHost;
  char *host_name = "el-win2012-2";
  struct in_addr addr;

  iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
  remoteHost = gethostbyname(host_name);
}