/*
*  LZH-Light algorithm implementation v 1.0
*  Copyright (C) Sergey Ignatchenko 1998
*  This  software  is  provided  "as is"  without  express  or implied 
*  warranty.
*
*  Permission to use, copy, modify, distribute and sell this  software 
*  for any purpose is hereby  granted  without  fee,  subject  to  the 
*  following restrictions:
*  1. this notice may not be removed or altered;
*  2. altered source versions must be plainly marked as such, and must 
*     not be misrepresented as being the original software.
*
*/

#ifndef ___lzhl1_h
#define ___lzhl1_h

#include <stdlib.h>
#include <memory.h>
#include <assert.h>


#ifndef BYTE
#define BYTE unsigned char
#endif

#ifndef INT16
#define INT16 signed short
#endif

#ifndef INT32
#define INT32 signed int
#endif

#ifndef UINT16
#define UINT16 unsigned short
#endif

#ifndef UINT32
#define UINT32 unsigned int
#endif

#ifndef BOOL
#define BOOL int
#endif
#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

#ifndef min
#define min( a, b ) ( (a) < (b) ? (a) : (b) )
#endif
#ifndef max
#define max( a, b ) ( (a) > (b) ? (a) : (b) )
#endif

#ifdef _MSC_VER
#define __CDECL __cdecl
#pragma intrinsic( memcpy, memset, _rotl )
#define ROTL( x, y ) _rotl( x, y )
#if _MSC_VER >= 1400
//Visual Studio 2005+
#pragma warning ( disable:4267 )//conversion from size_t to 32-bit
#pragma warning ( disable:4244 )//conversion; possible loss of data
#endif
#else
#define __CDECL 
#define ROTL( x, y ) ( ( (x) << (y) ) | ( (x) >> (32-(y)) ) )
#endif

#define LZMIN 4

//{{{{{{{{{{{{{{{{{ USER - TUNABLE ********************************************

//Affect format
#define LZBUFBITS1 14
//LZBUFBITS is a log2(LZBUFSIZE) and must be in range 10 - 16

//NOT affect format
#define LZMATCH 5

#define LZSLOWHASH
#define LZTABLEBITS1 13
//LZTABLEBITS is a log2(LZTABLESIZE) and should be in range 9 - 17

#define LZOVERLAP
//#define LZBACKWARDMATCH
//#define LZLAZYMATCH
#define LZSKIPHASH 1024

#define HUFFRECALCLEN 4096
//HUFFRECALCLEN should be <= 16384

//}}}}}}}}}}}}}}}}} USER - TUNABLE ********************************************

#define LZTABLESIZE1 (1<<(LZTABLEBITS1))
#define LZBUFSIZE1 (1<<(LZBUFBITS1))

#endif
