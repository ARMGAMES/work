/*
* Carbon copy of Mis/Crc.cpp file. Added here for the sake of benchmark.
* All comments to functions were removed - in case you need them, please refer to original.
*/

#include "stdafx.h"
#include "crc.h"

#if defined(WIN32) && !defined(EV_DEBUG)
#pragma optimize("t", on)
#endif

#if defined(__sun)
#include <stdlib.h>
#else
#include <cstdlib>
#endif

#if defined(_WIN64) 
#define kTableRows 32
#define crc_calculate_reflected crc_calculate_reflected_slide_by_32
#else
#define kTableRows 16
#define crc_calculate_reflected crc_calculate_reflected_slide_by_16
#endif


//#include "Mis/fisynchronization.h"
//static csl::FIMutex   CRC_initalize_tables_mutex;
static bool           CRC_tables_initialized(false); 
static CRCVALUE       CRC_reflected_table[kTableRows][256]; 
static CRCVALUE       CRC_non_reflected_table[256]; 

/*
 * Helper-function prototypes
 */
CRCVALUE crc_reflect(CRCVALUE, int);
void crc_initialize_global_tables();
void crc_inittab(bool reflected);

/***********************************************************************
 *  NAME: crc_create
 *    OS: all
 *  Creates and initializes a CRC structure.
 * 
 *  Argument: enum crc_t crc_type
 *  Type of CRC calculation to perform:
 *  o CRC_DMS1, CRC_AUTODIN2, CRC_SIDF
 *  o CRC_ALT_1, CRC_DMS2
 *
 *  Returns:  CRC *
 *  A pointer to a CRC structure if successful; NULL if otherwise.
 ***********************************************************************/
CRC *crc_create(enum crc_t crc_type)
{
  CRC *crcptr;

  crcptr = (CRC*)malloc(sizeof(CRC));
  //checkadr(crcptr, allocerr);

  switch (crc_type) {

  case CRC_DMS1:
  case CRC_AUTODIN2:
  case CRC_SIDF:
    crcptr->poly = CRC_POLY_AUTODIN2;
    crcptr->reflected = 1;
    break;

  case CRC_ALT_1:
  case CRC_DMS2:

    crcptr->poly = CRC_POLY_AUTODIN2;
    crcptr->reflected = 0;
    break;

  default:
    /* Requested type not supported */
    free(crcptr);
    return (0);
  }

  crc_initialize_global_tables(); // this makes sure tables are initialized only once
  return(crcptr);

/* error exits */

//allocerr:
  //log_show(0, MOD_CRC, VV_ALLOCERR, (void*)sizeof(CRC));
  //I18N_LOG1(MSG_GLOBAL__VV_ALLOCERR, MOD_CRC, F, sizeof(CRC));  
  //return 0;
}

// passing in all parameters in case we need them for a better simulation
CRCVALUE crc_calculate_simulation(CRC* /*crc*/, char* /*buf*/, size_t /*len*/, CRCVALUE /*initial_crc*/)
{
  static bool crc_simulation_started(false);
  static CRCVALUE fake_crc(1);
  if (!crc_simulation_started)
  {
    crc_simulation_started = true;
    srand(time(NULL));
    fake_crc = rand();
  }

  ++fake_crc;  // next CRC
  const CRCVALUE kWrappedAroundCRC(1);
  if ((0 == fake_crc) || (0xFFFFFFFF == fake_crc))   // we need to avoid 0 and 0xFFFFFFFF for simulated CRC after wrap around
  {
    fake_crc = kWrappedAroundCRC;
  }
  return fake_crc;
}

CRCVALUE crc_calculate_reflected_slide_by_16(const char* buffer, uint32_t length, CRCVALUE initial_crc)
{
  uint64_t next_crc;
  uint64_t crc(initial_crc);
  uint32_t align((sizeof(uint32_t) - (unsigned long)(int*)buffer) & (sizeof(uint32_t) - 1));

  align = align < length ? align : length;
  length -= align;

  for (; align; align--)
  {
    crc = CRC_reflected_table[0][(crc ^ *buffer++) & 0xFF] ^ (crc >> 8);
  }

  const uint32_t kSize(sizeof(uint64_t) * 2);

  while (length >= kSize)
  {
    crc ^= *(uint64_t*)buffer;
    next_crc  = *(uint64_t*)&buffer[8];

    crc = CRC_reflected_table[15][(crc)            & 0xFF] ^
          CRC_reflected_table[14][(crc      >>  8) & 0xFF] ^
          CRC_reflected_table[13][(crc      >> 16) & 0xFF] ^
          CRC_reflected_table[12][(crc      >> 24) & 0xFF] ^
          CRC_reflected_table[11][(crc      >> 32) & 0xFF] ^
          CRC_reflected_table[10][(crc      >> 40) & 0xFF] ^
          CRC_reflected_table[ 9][(crc      >> 48) & 0xFF] ^
          CRC_reflected_table[ 8][(crc >>      56)]        ^
          CRC_reflected_table[ 7][(next_crc)       & 0xFF] ^
          CRC_reflected_table[ 6][(next_crc >>  8) & 0xFF] ^
          CRC_reflected_table[ 5][(next_crc >> 16) & 0xFF] ^
          CRC_reflected_table[ 4][(next_crc >> 24) & 0xFF] ^
          CRC_reflected_table[ 3][(next_crc >> 32) & 0xFF] ^
          CRC_reflected_table[ 2][(next_crc >> 40) & 0xFF] ^
          CRC_reflected_table[ 1][(next_crc >> 48) & 0xFF] ^
          CRC_reflected_table[ 0][(next_crc >> 56)];
    
    buffer += kSize;
    length -= kSize;
  }

  while (length--)
  {
    crc = CRC_reflected_table[0][(crc ^ *buffer++) & 0xFF] ^ (crc >> 8);
  }

  return (CRCVALUE) crc;
}

CRCVALUE crc_calculate_reflected_slide_by_32(const char* buffer, uint32_t length, CRCVALUE initial_crc)
{
  uint64_t crc(initial_crc);
  uint32_t align((sizeof(uint32_t) - (unsigned long)(int*)buffer) & (sizeof(uint32_t) - 1));

  align = align < length ? align : length;
  length -= align;

  for (; align; align--)
  {
    crc = CRC_reflected_table[0][(crc ^ *buffer++) & 0xFF] ^ (crc >> 8);
  }

  const uint32_t kSize(sizeof(uint64_t) * 4);

  uint64_t next_crc_1;
  uint64_t next_crc_2;
  uint64_t next_crc_3;
  while (length >= kSize)
  {
    crc ^= *(uint64_t*)buffer;
    next_crc_1  = *(uint64_t*)&buffer[8];
    next_crc_2  = *(uint64_t*)&buffer[16];
    next_crc_3  = *(uint64_t*)&buffer[24];

    crc = CRC_reflected_table[31][(crc)              & 0xFF] ^
          CRC_reflected_table[30][(crc      >>  8)   & 0xFF] ^
          CRC_reflected_table[29][(crc      >> 16)   & 0xFF] ^
          CRC_reflected_table[28][(crc      >> 24)   & 0xFF] ^
          CRC_reflected_table[27][(crc      >> 32)   & 0xFF] ^
          CRC_reflected_table[26][(crc      >> 40)   & 0xFF] ^
          CRC_reflected_table[25][(crc      >> 48)   & 0xFF] ^
          CRC_reflected_table[24][(crc >>      56)]          ^
          CRC_reflected_table[23][(next_crc_1)       & 0xFF] ^
          CRC_reflected_table[22][(next_crc_1 >>  8) & 0xFF] ^
          CRC_reflected_table[21][(next_crc_1 >> 16) & 0xFF] ^
          CRC_reflected_table[20][(next_crc_1 >> 24) & 0xFF] ^
          CRC_reflected_table[19][(next_crc_1 >> 32) & 0xFF] ^
          CRC_reflected_table[18][(next_crc_1 >> 40) & 0xFF] ^
          CRC_reflected_table[17][(next_crc_1 >> 48) & 0xFF] ^
          CRC_reflected_table[16][(next_crc_1 >> 56)]        ^   
          CRC_reflected_table[15][(next_crc_2)       & 0xFF] ^
          CRC_reflected_table[14][(next_crc_2 >>  8) & 0xFF] ^
          CRC_reflected_table[13][(next_crc_2 >> 16) & 0xFF] ^
          CRC_reflected_table[12][(next_crc_2 >> 24) & 0xFF] ^
          CRC_reflected_table[11][(next_crc_2 >> 32) & 0xFF] ^
          CRC_reflected_table[10][(next_crc_2 >> 40) & 0xFF] ^
          CRC_reflected_table[ 9][(next_crc_2 >> 48) & 0xFF] ^
          CRC_reflected_table[ 8][(next_crc_2 >> 56)]        ^
          CRC_reflected_table[ 7][(next_crc_3)       & 0xFF] ^
          CRC_reflected_table[ 6][(next_crc_3 >>  8) & 0xFF] ^
          CRC_reflected_table[ 5][(next_crc_3 >> 16) & 0xFF] ^
          CRC_reflected_table[ 4][(next_crc_3 >> 24) & 0xFF] ^
          CRC_reflected_table[ 3][(next_crc_3 >> 32) & 0xFF] ^
          CRC_reflected_table[ 2][(next_crc_3 >> 40) & 0xFF] ^
          CRC_reflected_table[ 1][(next_crc_3 >> 48) & 0xFF] ^
          CRC_reflected_table[ 0][(next_crc_3 >> 56)];
    
    buffer += kSize;
    length -= kSize;
  }

  while (length--)
  {
    crc = CRC_reflected_table[0][(crc ^ *buffer++) & 0xFF] ^ (crc >> 8);
  }

  return (CRCVALUE) crc;
}

/***********************************************************************
 *  NAME: crc_calculate
 *    OS: all
 *  Calculates a CRC.
 * 
 *  Argument: CRC * crc
 *  Pointer to an initialized CRC structure.
 *  Argument: char *buf
 *  Pointer to a buffer whose CRC value to calculate.
 *  Argument: uint32_t len
 *  Number of bytes in the buffer.
 *  Argument: CRCVALUE initial_crc
 *  Input value to CRC calculation routine.
 *
 *  Returns:  CRCVALUE 
 *  Calculated CRC value.
 ***********************************************************************/
// TODO: Some callers may expect this routine to handle size_t len
//       We should change this interface to accept uint32_t only. 
//       All caller should adjust accordingly.
CRCVALUE crc_calculate(CRC* crc, char* buf, size_t len, CRCVALUE initial_crc)
{
  //
  // avoid creating a zero crc return value when call this 
  // with a zero len buffer and a non-seed crc input value
  //
  if (0 == len)
  {
    return (initial_crc == 0xFFFFFFFFL)? 0 : initial_crc;
  }

  register CRCVALUE crc_value;

  crc_value = initial_crc;

  if (crc->reflected)
  {
    crc_value = crc_calculate_reflected(buf, (uint32_t) len, initial_crc);
  }
  else 
  {
    while (len--)
    {
      crc_value = CRC_non_reflected_table[((crc_value >> 24) ^ *buf++) & 0xffL] ^ (crc_value << 8);
    }
  }

  return crc_value ^ initial_crc;
}
 
CRCVALUE crc_byte_calculate(CRC* crc, char* buf, size_t len, register CRCVALUE crc_value)
{
  if (crc->reflected)
  {
    crc_value = crc_calculate_reflected(buf, (uint32_t) len, crc_value);
  }
  else 
  {
    while (len--)
    {
      crc_value = CRC_non_reflected_table[((crc_value >> 24) ^ *buf++) & 0xffL] ^ (crc_value << 8);
    }
  }
  return crc_value;
}

/***********************************************************************
 *  NAME: crc_destroy
 *    OS: all
 *  Disposes of CRC structure.
 * 
 *  Argument: CRC *crc
 *  Pointer to an initialized CRC structure.
 *
 *  Returns:  unsigned long 
 *  0 always.
 ***********************************************************************/
CRC* crc_destroy(CRC* crc)
{
  if (crc) 
  {
    free(crc);
  }
  return 0;
}

/*
 * HELPER FUNCTIONS
 */
/***********************************************************************
 *  NAME: crc_reflect
 *    OS: all
 *  Reserves the bit order for the bottom N bits in a word.
 * 
 *  Argument: CRCVALUE crc
 *  CRC values whose bits are to be reflected.
 *  Argument: int bits
 *  Number of bottom bits to reflect.
 *
 *  Returns:  CRCVALUE reflected_crc 
 *  Resultant CRC value.
 ***********************************************************************/
CRCVALUE crc_reflect(CRCVALUE crc, int bits)
{
  CRCVALUE reflected_crc(0);

  while (bits > 0) 
  {
    reflected_crc <<= 1;
    reflected_crc = reflected_crc | (crc & 1L);
    crc >>= 1;
    --bits;
  }

  return reflected_crc;
}

/***********************************************************************
 *  NAME: crc_inittab
 *    OS: all
 *  Builds the CRC lookup table.
 * 
 *  Argument: reflected - true to initialize reflected table
 *                        false to initialize non-reflected table
 *
 *  Returns:  void 
 *  Nothing.
 ***********************************************************************/
void crc_inittab(bool reflected)
{
  const CRCVALUE kPolyCRC(CRC_POLY_AUTODIN2);
  CRCVALUE checksum;
  CRCVALUE i, j;
  
  for (i = 0; i < 256; ++i) 
  {
    if (reflected)
    {
      checksum = (crc_reflect(i, 8)) << 24;
    }
    else
    {
      checksum = i << 24;
    }

    for (j = 0; j < 8; ++j) 
    {
      if (checksum & 0x80000000L)
      {
        checksum = (checksum << 1) ^ kPolyCRC;
      }
      else
      {
        checksum <<= 1;
      }
    }

    if (reflected)
    {
      CRC_reflected_table[0][i] = crc_reflect(checksum, 32);
    }
    else
    {
      CRC_non_reflected_table[i] = checksum;
    }
  }

  if (reflected)
  {
    for (i=0; i<256; ++i)
    {
      checksum = CRC_reflected_table[0][i];
      for (j=1; j < kTableRows; ++j)
      {
        checksum = CRC_reflected_table[0][checksum & 0xFF] ^ (checksum >> 8);
        CRC_reflected_table[j][i] = checksum;
      }
    }
  }
}

// this makes sure tables are initialized only once
// Instead of initializing one table at a time, we initialize both to be simplier.
// In other words, the first caller initializes both tables for the benefit 
// of all subsequent callers.
void crc_initialize_global_tables()
{
  //csl::FIScopedLock lock(&CRC_initalize_tables_mutex);

  if (!CRC_tables_initialized)
  {
    CRC_tables_initialized = true;
    crc_inittab(true);    // initialize reflected table
    crc_inittab(false);    // initialize non-reflected table
  }
}
