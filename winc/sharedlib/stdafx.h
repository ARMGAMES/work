// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers



// TODO: reference additional headers your program requires here

#if !defined(_WINSOCKAPI_)
#  define _WINSOCKAPI_
#  include <windows.h>
#  undef _WINSOCKAPI_
#else
#  include <windows.h>
#endif

#include <stdint.h>
#include <stdio.h>
#include <tchar.h>

#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <fstream>
#include <process.h>
#include <time.h>

#include "data/crc.h"
#include "mis/stopwatch.h"