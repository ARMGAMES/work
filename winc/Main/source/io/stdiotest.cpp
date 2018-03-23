
#include "stdafx.h"

using namespace std;

void write(const void *buf, size_t nbytes, FILE* m_file)
{
    size_t n;
    if(!nbytes)
        return;
    n = fwrite(buf, 1, nbytes, m_file);
}

const int kSize(2*1024*1024);
char buffer[kSize];

void write_test()
{
  char md5buf[16] = "012345678912345";
  size_t md5sz(16);

  char crcbuf[4] = "crc";
  size_t crcsz(4);

  FILE* file = fopen("C:\\test.txt", "w");
  //setvbuf (file, buffer, _IOFBF,  kSize);

  for(size_t index = 0;  index < 33554432; ++index) 
  {
    write(md5buf, md5sz, file);
    write(crcbuf, crcsz, file);
  }

  fclose(file);
}

void test_fwrite()
{
  clock_t clk;
  size_t count = clock();

  write_test();

  count = clock() - count;

  printf ("Total time spent: %f\n", ((float)count)/CLOCKS_PER_SEC);
  printf ("\n");
}