// udxsupport.h
// Error handling for User Defined Functions and Aggregates
//
// (C) Copyright IBM Corp. 2007, 2014  All Rights Reserved.


#ifndef __UDX_SUPPORT_H__
#define __UDX_SUPPORT_H__
#include <stdio.h>
#include <iostream>

#include <sys/types.h>
#include <stdint.h>
#include <inttypes.h>

using namespace std;

typedef unsigned int    uint;
typedef unsigned char   uchar;
typedef uint8_t         byte;

// Ideally we'd use stdint.h types here, but these types need to be
// identical to the typedefs in pg/include/c.h
typedef signed char     int8;
typedef unsigned char   uint8;
typedef signed short    int16;
typedef unsigned short  uint16;
typedef signed int      int32;
typedef unsigned int    uint32;

// For some reason, int64/uint64 are based on standard types, not POD types
typedef int64_t         int64;
typedef uint64_t        uint64;

// to avoid bool size issues between GCC intel and ppc targets
typedef uint8_t         nzbool;

// Integer types large enough to contain pointers
typedef uintptr_t       uint_ptr;
typedef intptr_t        int_ptr;

typedef uint16 vlen_t;

#define NZ_ERROR_ILLVAL 8
#define NZ_ERROR_UDX_EXCEPTION 93

extern "C" int throwError(int code, const char *buf, const char *obj);

static int throwError(int code, const char* buf, int num)
{
  char str[1024];
  sprintf(str, "%d", num);
  throwError(code, buf, str);
  return 0;

}
static void throwUdxException(const char *msg)
{
    throwError(NZ_ERROR_UDX_EXCEPTION, msg, (const char*)0);
}

#endif


