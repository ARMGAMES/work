
#include "stdafx.h"

void test_crc()
{
  const uint32_t kSize(512);
  CRC* crc = crc_create(CRC_DMS1);

  char buf[kSize] = "123123";
  CRCVALUE test = crc_calculate(crc, buf, kSize, 0);

  if (crc)
  {
    crc_destroy(crc);
  }
}