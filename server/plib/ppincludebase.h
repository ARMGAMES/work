#ifndef ppincludebase_h_included
#define ppincludebase_h_included

// PLIB - Platform LIBrary
// Platform macros: LINUX, _WIN32, P_AIX32, P_AIX64, P_MAC10X
// Additional macros:
//		PBIG_ENDIAN/PLITTLE_ENDIAN (default - none defined)
//		PNOTHREADS (default=OFF)

// ************** PLATFORM-SPECIFIC DEFINITIONS

#if defined(LINUX) && !defined(P_ANDROID)
// ************** PLATFORM-SPECIFIC DEFINITIONS: LINUX

#define _GNU_SOURCE 1 //main reason is to enable recursive mutexes via __USE_UNIX98
#define _THREAD_SAFE 1


#include <limits.h> // PTHREAD_STACK_MIN
#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <wchar.h>
#include <math.h>//requires libm.a (-lm switch for gcc)
#include <stdarg.h>
#include <signal.h>

#include <fcntl.h>

#include <netdb.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/times.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/eventfd.h>
#include <sys/epoll.h>
#include <sys/resource.h>

typedef unsigned char UINT8;
typedef signed char INT8;
typedef unsigned char BYTE;
typedef signed short INT16;
typedef unsigned short UINT16;
typedef signed int INT32;
typedef unsigned int UINT32;
typedef unsigned long long UINT64;
typedef long long INT64;

//#error Please ensure the following defines work properly on this platform
#define PRId64 "lld"
#define PRIu64 "llu"
#define PRIx64 "llx"
#define PRIX64 "llX"

#define P_USE_READWRITE_LOCK_VERSION_2
#define P_USE_EPOLL

#if defined( __LP64__ )
#define P64BITPLATFORM 1
#endif

#define SOCKET int
#define INVALID_SOCKET -1
#define VALID_SOCKET( s ) ( s >= 0 )
#define SOCKETERRNO() errno
#define CLOSESOCKET( s ) close( s )
#define FILESEPARATOR "/"
#define FILESEPARATORCHAR '/'
#define _SETSTDINBINARYMODE()
#define _SETSTDOUTBINARYMODE()
#define ONE64 (UINT64(1))

void INITRECURSIVEMUTEX( pthread_mutex_t* mutex );

time_t TIMEMSEC( int* msec );
UINT32 RANDOMTIME();
#define TIMEZONE() timezone
UINT32 SYSTEMTICKS();
UINT32  LONGSYSTEMTICKS(int * msec);
#define TICKS_PER_SECOND 1000
//NOTE: to ensure correct wrap-around handling, we need to use system-dependent TICKS_PER_SECOND
#define TIME( t ) time( t )

#define PID() getpid()

#if defined(P64BITPLATFORM) 
#define THREADID() (UINT32)(UINT64)pthread_self()
#define THREADIDREAL() (UINT64)pthread_self()
#else
#define THREADID() (UINT32) pthread_self()
#define THREADIDREAL() (UINT32) pthread_self()
#endif

#define FATALEXIT()	_exit( 1 )

#define PPOSIX 1
#define TYPENAME typename



#elif defined (P_ANDROID)
// ************** PLATFORM-SPECIFIC DEFINITIONS: ANDROID

#define _GNU_SOURCE 1 //main reason is to enable recursive mutexes via __USE_UNIX98
#define _THREAD_SAFE 1

#include <limits.h> // PTHREAD_STACK_MIN
#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <wchar.h>
#include <math.h>//requires libm.a (-lm switch for gcc)
#include <stdarg.h>
#include <signal.h>

#include <fcntl.h>

#include <netdb.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/times.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <sys/wait.h>

#include <linux/stat.h>
#include <sys/mman.h>
#define S_IREAD S_IRUSR
#define S_IWRITE S_IWUSR



typedef uint8_t UINT8;
typedef int8_t INT8;
typedef unsigned char BYTE;
typedef int16_t INT16;
typedef uint16_t UINT16;
typedef int32_t INT32;
typedef uint32_t UINT32;
typedef uint64_t UINT64;
typedef int64_t INT64;

#if defined( __LP64__ )
#define P64BITPLATFORM 1
#define P_ANDROID_64
#endif

#define PRId64 "lld"
#define PRIu64 "llu"
#define PRIx64 "llx"
#define PRIX64 "llX"

#define SOCKET int
#define INVALID_SOCKET -1
#define VALID_SOCKET( s ) ( s >= 0 )
#define SOCKETERRNO() errno
#define CLOSESOCKET( s ) close( s )
#define FILESEPARATOR "/"
#define FILESEPARATORCHAR '/'
#define _SETSTDINBINARYMODE()
#define _SETSTDOUTBINARYMODE()
#define ONE64 (UINT64(1))

void INITRECURSIVEMUTEX( pthread_mutex_t* mutex );

time_t TIMEMSEC( int* msec );
UINT32 RANDOMTIME();
#define TIMEZONE() timezone
UINT32 SYSTEMTICKS();
UINT32  LONGSYSTEMTICKS(int * msec);
#define TICKS_PER_SECOND 1000
//NOTE: to ensure correct wrap-around handling, we need to use system-dependent TICKS_PER_SECOND
#define TIME( t ) time( t )

#define PID() getpid()

#if defined(P64BITPLATFORM) 
#define THREADID() (UINT32)(UINT64)pthread_self()
#define THREADIDREAL() (UINT64)pthread_self()
#else
#define THREADID() (UINT32) pthread_self()
#define THREADIDREAL() (UINT32) pthread_self()
#endif

#define FATALEXIT()	_exit( 1 )

#define PPOSIX 1
#define TYPENAME typename

#define P_USE_READWRITE_LOCK_VERSION_2

#elif defined (P_MAC10X)
// ************** PLATFORM-SPECIFIC DEFINITIONS: Mac OS X

#define _GNU_SOURCE 1 //main reason is to enable recursive mutexes via __USE_UNIX98
#define _THREAD_SAFE 1

#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <sys/malloc.h> // Not needed - not avail for iPad/iPhone
#include <time.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <wchar.h>
#include <math.h>//requires libm.a (-lm switch for gcc)
#include <stdarg.h>
#include <signal.h>

#include <fcntl.h>

#include <netdb.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/times.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <limits.h>

#ifndef SIZE_MAX
#define SIZE_MAX	  ULONG_MAX
#endif

typedef unsigned char UINT8;
typedef signed char INT8;
typedef unsigned char BYTE;
typedef signed short INT16;
typedef unsigned short UINT16;
typedef signed int INT32;
typedef unsigned int UINT32;
typedef unsigned long long UINT64;
typedef long long INT64;

#ifndef PRId64
#define PRId64 "lld"
#endif

#ifndef PRIu64
#define PRIu64 "llu"
#endif

#ifndef PRIx64
#define PRIx64 "llx"
#endif

#ifndef PRIX64
#define PRIX64 "llX"
#endif

#if defined( __LP64__ )
#define P64BITPLATFORM 1
#endif


#define SOCKET int
#define INVALID_SOCKET -1
#define VALID_SOCKET( s ) ( s >= 0 )
#define SOCKETERRNO() errno
#define CLOSESOCKET( s ) close( s )
#define FILESEPARATOR "/"
#define FILESEPARATORCHAR '/'
#define _SETSTDINBINARYMODE()
#define _SETSTDOUTBINARYMODE()
#define ONE64 (UINT64(1))

void INITRECURSIVEMUTEX( pthread_mutex_t* mutex );

time_t TIMEMSEC( int* msec );
UINT32 RANDOMTIME();
int TIMEZONE();
UINT32 SYSTEMTICKS();
UINT32  LONGSYSTEMTICKS(int * msec);
#define TICKS_PER_SECOND 1000 //CLOCKS_PER_SEC
//NOTE: to ensure correct wrap-around handling, we need to use system-dependent TICKS_PER_SECOND
#define TIME( t ) time( t )

#define PID() getpid()

#if defined(P64BITPLATFORM) 
#define THREADID() (UINT32)(UINT64)pthread_self()
#define THREADIDREAL() (UINT64)pthread_self()
#else
#define THREADID() (UINT32) pthread_self()
#define THREADIDREAL() (UINT32) pthread_self()
#endif

#define FATALEXIT()	_exit( 1 )

#define PPOSIX 1
#define TYPENAME typename

#if defined(__BIG_ENDIAN__)
#define P_BIG_ENDIAN 1
#endif

#define P_USE_READWRITE_LOCK_VERSION_2

#elif defined( P_AIX32 )
// ************** PLATFORM-SPECIFIC DEFINITIONS: AIX GCC 32 bits

#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <wchar.h>
#include <math.h>//requires libm.a (-lm switch for gcc)
#include <stdarg.h>

#include <fcntl.h>

#include <netdb.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/times.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#include <limits.h>

typedef unsigned char UINT8;
typedef signed char INT8;
typedef unsigned char BYTE;
typedef signed short INT16;
typedef unsigned short UINT16;
typedef signed int INT32;
typedef unsigned int UINT32;
typedef u_int64 UINT64;
typedef int64 INT64;

#error Please ensure the following defines work properly on this platform
#define PRId64 "lld"
#define PRIu64 "llu"
#define PRIx64 "llx"
#define PRIX64 "llX"


#define SOCKET int
#define INVALID_SOCKET -1
#define VALID_SOCKET( s ) ( s >= 0 )
#define SOCKETERRNO() errno
#define CLOSESOCKET( s ) close( s )
#define FILESEPARATOR "/"
#define FILESEPARATORCHAR '/'
#define _SETSTDINBINARYMODE()
#define _SETSTDOUTBINARYMODE()
#define ONE64 (UINT64(1))

void INITRECURSIVEMUTEX( pthread_mutex_t* mutex );

time_t TIMEMSEC( int* msec );
UINT32 RANDOMTIME();
#define TIMEZONE() timezone
UINT32 SYSTEMTICKS();
UINT32  LONGSYSTEMTICKS(int * msec);
#define TICKS_PER_SECOND 1000
//NOTE: to ensure correct wrap-around handling, we need to use system-dependent TICKS_PER_SECOND
#define TIME( t ) time( t )

#define PID() getpid()
#define THREADID() pthread_self()
#define THREADIDREAL() pthread_self()

#define FATALEXIT()	_exit( 1 )

#define PPOSIX 1
#define P_BIG_ENDIAN 1
#define TYPENAME typename


#elif defined( P_AIX64 )
// ************** PLATFORM-SPECIFIC DEFINITIONS: AIX XLC 64 bits

#include <pthread.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <time.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <wchar.h>
#include <math.h>//requires libm.a (-lm switch for gcc)
#include <stdarg.h>

#include <fcntl.h>

#include <netdb.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/times.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#include <limits.h>

typedef unsigned char UINT8;
typedef signed char INT8;
typedef unsigned char BYTE;
typedef signed short INT16;
typedef unsigned short UINT16;
typedef signed int INT32;
typedef unsigned int UINT32;
typedef u_int64 UINT64;
typedef int64 INT64;

#error Please ensure the following defines work properly on this platform
#define PRId64 "lld"
#define PRIu64 "llu"
#define PRIx64 "llx"
#define PRIX64 "llX"

#define P64BITPLATFORM 1

#define SOCKET int
#define INVALID_SOCKET -1
#define VALID_SOCKET( s ) ( s >= 0 )
#define SOCKETERRNO() errno
#define CLOSESOCKET( s ) close( s )
#define FILESEPARATOR "/"
#define FILESEPARATORCHAR '/'
#define _SETSTDINBINARYMODE()
#define _SETSTDOUTBINARYMODE()
#define ONE64 (UINT64(1))

void INITRECURSIVEMUTEX( pthread_mutex_t* mutex );

time_t TIMEMSEC( int* msec );
UINT32 RANDOMTIME();
#define TIMEZONE() timezone
UINT32 SYSTEMTICKS();
UINT32  LONGSYSTEMTICKS(int * msec);
#define TICKS_PER_SECOND 1000
//NOTE: to ensure correct wrap-around handling, we need to use system-dependent TICKS_PER_SECOND
#define TIME( t ) time( t )

#define PID() getpid()
#define THREADID() (UINT32) pthread_self()
#define THREADIDREAL() pthread_self()

#define FATALEXIT()	_exit( 1 )

#define PPOSIX 1
#define P_BIG_ENDIAN 1
#define TYPENAME typename

#elif defined( _WIN64 )

#if !defined( COMM_CLIENT_ONLY )
#define UDT_DEVELOPMENT
#endif

#define PWIDE_ONLY

// ************** PLATFORM-SPECIFIC DEFINITIONS: WIN64/Visual Studio 2005+

#ifdef _MSC_VER
#  if _MSC_VER >= 1400
#    ifndef NDEBUG
#      define _CRTDBG_MAP_ALLOC
#      include <crtdbg.h>
#    endif
#  endif
#endif

#define P_USE_READWRITE_LOCK_VERSION_2

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <time.h>
#include <ctype.h>
#include <process.h>
#include <errno.h>
#include <direct.h>
#include <conio.h>
#include <math.h>

#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>
#include <sys\types.h>
#include <sys\timeb.h>
#include <new.h>
#include <limits.h>

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <winsock2.h>
#include <windows.h>
#include "ppmemtrace.h"

typedef unsigned char UINT8;
typedef signed char INT8;
typedef unsigned char BYTE;
typedef signed short INT16;
typedef unsigned short UINT16;
typedef signed int INT32;
typedef unsigned int UINT32;
typedef unsigned __int64 UINT64;
typedef __int64 INT64;


#define PRId64 "I64d"
#define PRIu64 "I64u"
#define PRIx64 "I64x"
#define PRIX64 "I64X"

#define P64BITPLATFORM 1

#define VALID_SOCKET( s ) ( s != INVALID_SOCKET )
#define SOCKETERRNO() WSAGetLastError()
#define CLOSESOCKET( s ) closesocket( s )
typedef int socklen_t;
#define FILESEPARATOR "\\"
#define FILESEPARATORCHAR '\\'
#define _SETSTDINBINARYMODE() _setmode( 0, O_BINARY )
#define _SETSTDOUTBINARYMODE() _setmode( 1, O_BINARY )
#define FILEERRNO() GetLastError()
#define ONE64 (UINT64(1))

#include <intrin.h>
#pragma intrinsic(__rdtsc)
#pragma intrinsic(_InterlockedIncrement)
#pragma intrinsic(_InterlockedDecrement)
#pragma intrinsic(_InterlockedExchangeAdd)
#define InterlockedIncrement _InterlockedIncrement
#define InterlockedDecrement _InterlockedDecrement
#define InterlockedExchangeAdd _InterlockedExchangeAdd

inline UINT64 getHighResCounter() { return __rdtsc(); }

time_t TIMEMSEC( int* msec );
#define RANDOMTIME() GetTickCount()
//#define TIMEZONE() timezone
int TIMEZONE();
#define SYSTEMTICKS() GetTickCount()
UINT32  LONGSYSTEMTICKS(int * msec);
#define TICKS_PER_SECOND 1000
//NOTE: to ensure correct wrap-around handling, we need to use system-dependent TICKS_PER_SECOND
#define TIME( t ) time( t )

#define PID() GetCurrentProcessId()
#define THREADID() GetCurrentThreadId()
#define THREADIDREAL() GetCurrentThreadId()

inline INT32 PMulDiv( INT32 number, INT32 numerator, INT32 denominator )
{
	return (INT32)( Int32x32To64( number, numerator ) / denominator );
}

inline UINT32 PUMulDiv( UINT32 number, UINT32 numerator, UINT32 denominator )
{
	return (UINT32)( UInt32x32To64( number, numerator ) / denominator );
}

#define FATALEXIT() TerminateProcess( GetCurrentProcess(), 1 )

void _PWin32SystemTimeToStructTm(const SYSTEMTIME * systime, struct tm * timetm);
void _PWin32StructTmToSystemTime(const struct tm * timetm, SYSTEMTIME * systime);

#ifdef _MSC_VER
#  if _MSC_VER >= 1400
#    ifndef NDEBUG
#      define _CRTDBG_MAP_ALLOC
#      include <crtdbg.h>
#    endif
#  endif

# pragma hdrstop
# ifndef _MT
#   define PNOTHREADS
# endif
#endif

#define PWIN64 1
#define PWIN32 1

#ifdef _MSC_VER
#if _MSC_VER >= 1400
//Visual Studio 2005+
#define TYPENAME typename
#else
#define TYPENAME
#endif
#else
#define TYPENAME
#endif

#elif defined( _WIN32 )
// ************** PLATFORM-SPECIFIC DEFINITIONS: WIN32/Visual Studio 6 or WIN32/Visual Studio 2005+ (Visual Studio 2003 is not supported)
// ************** Visual Studio 6 has _MSC_VER 12XX, Visual Studio 2003 - _MSC_VER 13XX, Visual Studio 2005 - _MSC_VER 14XX, Visual Studio 2012 - _MSC_VER 17XX

#if !defined( COMM_CLIENT_ONLY )
#define UDT_DEVELOPMENT
#endif

#ifdef _MSC_VER
#  if _MSC_VER >= 1400
#    ifndef NDEBUG
#      define _CRTDBG_MAP_ALLOC
#      include <crtdbg.h>
#    endif
#	 if _MSC_VER >= 1700
#	   ifndef WINVER
		  #define _WIN32_WINNT  0x0500 
		  #define WINVER  0x0501
		  #define _WIN32_IE 0x0501
#      endif
#    endif
#  endif
#endif

#ifndef SUPPORT95
#define P_USE_READWRITE_LOCK_VERSION_2
#elif !defined( COMM_CLIENT_ONLY )
#error Please define COMM_CLIENT_ONLY in your project
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <time.h>
#include <ctype.h>
#include <process.h>
#include <errno.h>
#include <direct.h>
#include <conio.h>
#include <math.h>

#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>
#include <sys\types.h>
#include <sys\timeb.h>
#include <new.h>
#include <limits.h>

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <winsock2.h>
#include <windows.h>
#include "ppmemtrace.h"

typedef unsigned char UINT8;
typedef signed char INT8;
typedef unsigned char BYTE;
typedef signed short INT16;
typedef unsigned short UINT16;
typedef signed int INT32;
typedef unsigned int UINT32;
typedef unsigned __int64 UINT64;
typedef __int64 INT64;

#if _MSC_VER < 1800
#define PRId64 "I64d"
#define PRIu64 "I64u"
#define PRIx64 "I64x"
#define PRIX64 "I64X"
#else
#include <inttypes.h>
#endif

#define VALID_SOCKET( s ) ( s != INVALID_SOCKET )
#define SOCKETERRNO() WSAGetLastError()
#define CLOSESOCKET( s ) closesocket( s )
typedef int socklen_t;
#define FILESEPARATOR "\\"
#define FILESEPARATORCHAR '\\'
#define _SETSTDINBINARYMODE() _setmode( 0, O_BINARY )
#define _SETSTDOUTBINARYMODE() _setmode( 1, O_BINARY )
#define FILEERRNO() GetLastError()
#define ONE64 (UINT64(1))

#ifdef _MSC_VER
#if _MSC_VER >= 1400 // VS2005+

#pragma warning( 3:4701 ) // enable at warning level 3 : 'potentially uninitialized local variable'

#include <intrin.h>
#pragma intrinsic(__rdtsc)
#pragma intrinsic(_InterlockedIncrement)
#pragma intrinsic(_InterlockedDecrement)
#pragma intrinsic(_InterlockedExchangeAdd)
#define InterlockedIncrement _InterlockedIncrement
#define InterlockedDecrement _InterlockedDecrement
#define InterlockedExchangeAdd _InterlockedExchangeAdd
inline UINT64 getHighResCounter() { return __rdtsc(); }

# else // not VS2005+

#pragma warning( push )
#pragma warning( disable:4035 ) // no return value
inline UINT64 getHighResCounter() // Much higher overhead for inline asm
{
	__asm rdtsc;
}
#pragma warning( pop )

#endif // VS2005+
#else // not MSC

UINT64 getHighResCounter();

#endif // _MSC_VER

time_t TIMEMSEC( int* msec );
#define RANDOMTIME() GetTickCount()
//#define TIMEZONE() timezone
int TIMEZONE();
#define SYSTEMTICKS() GetTickCount()
UINT32  LONGSYSTEMTICKS(int * msec);
#define TICKS_PER_SECOND 1000
//NOTE: to ensure correct wrap-around handling, we need to use system-dependent TICKS_PER_SECOND
#define TIME( t ) time( t )

#define PID() GetCurrentProcessId()
#define THREADID() GetCurrentThreadId()
#define THREADIDREAL() GetCurrentThreadId()

inline INT32 PMulDiv( INT32 number, INT32 numerator, INT32 denominator )
{
	return (INT32)( Int32x32To64( number, numerator ) / denominator );
}

inline UINT32 PUMulDiv( UINT32 number, UINT32 numerator, UINT32 denominator )
{
	return (UINT32)( UInt32x32To64( number, numerator ) / denominator );
}

#define FATALEXIT() TerminateProcess( GetCurrentProcess(), 1 )

void _PWin32SystemTimeToStructTm(const SYSTEMTIME * systime, struct tm * timetm);
void _PWin32StructTmToSystemTime(const struct tm * timetm, SYSTEMTIME * systime);

#ifdef _MSC_VER

# if _MSC_VER < 1400
#  ifndef NDEBUG
#   define _CRTDBG_MAP_ALLOC
#   include <crtdbg.h>
#  endif
# endif

# pragma hdrstop
# ifndef _MT
#   define PNOTHREADS
# endif

#endif

#define PWIN32 1

#ifdef _MSC_VER
#if _MSC_VER >= 1400
//Visual Studio 2005+
#define TYPENAME typename
#else
#define TYPENAME
#endif
#else
#define TYPENAME
#endif


#else
#error Unknown platform
#endif


#ifdef PPOSIX

#define stricmp(a,b) strcasecmp(a,b)
#define strnicmp(a,b, c) strncasecmp(a,b,c)
#define PLIB_CONTROL_SIGNAL SIGUSR1
#define FILEERRNO() errno

inline INT32 PMulDiv( INT32 number, INT32 numerator, INT32 denominator )
{
	return (INT32)  ( ( (INT64)number * (INT64) numerator ) / denominator );
}

inline UINT32 PUMulDiv( UINT32 number, UINT32 numerator, UINT32 denominator )
{
	return (UINT32) ( ( (UINT64) number * (UINT64) numerator ) / denominator );
}

UINT64 getHighResCounter();
#define _O_RDWR O_RDWR
#define _O_CREAT O_CREAT
#define _O_TRUNC O_TRUNC
#define _O_BINARY 0
#define _S_IREAD S_IREAD
#define _S_IWRITE S_IWRITE
#define _O_RDONLY O_RDONLY

#elif defined ( PWIN32 )

#ifdef PWIDE_ONLY
 // prohibit PostMessageA
#undef PostMessage
#define PostMessage		C_ASSERT( 0 )
#define PostMessageA	C_ASSERT( 0 )
// prohibit SendMessageA
#undef SendMessage
#define SendMessage		C_ASSERT( 0 )
#define SendMessageA	C_ASSERT( 0 )
#endif

class PBlock;
class PString;
class PStringEncoding;
void _PWin32AppendWStringFromPStringEncoding( PBlock& ret /* of WCHAR's */, const char* s, const PStringEncoding* enc = NULL );//IMPORTANT: produces unterminated string; use _PWin32NullTerminateWString to terminate
void _PWin32WStringToPStringEncoding( PString& ret, const WCHAR* ws, const PStringEncoding* enc );
void _PWin32AppendWStringFromPStringEncodingC( PBlock& ret /* of WCHAR's */, const char* s, const PStringEncoding* enc = NULL );//IMPORTANT: converts lf to cr-lf & produces unterminated string; use _PWin32NullTerminateWString to terminate
void _PWin32WStringToPStringEncodingC( PString& ret, const WCHAR* ws, const PStringEncoding* enc );//IMPORTANT: converts cr-lf to lf
void _PWin32NullTerminateWString( PBlock& ret /* of WCHAR's */ );
#ifndef PWIDE_ONLY
bool _PWinIsUnicode();
void _PWinSetUnicode( INT32 isUnicode );//for debugging purposes only!
void _PWin32WStringToA( PBlock& s, const WCHAR* ws );//for use on !_PWinIsUnicode() (9x) ONLY
#endif
void _PWin32AStringToW( PBlock& ws, const char* s );

#if !defined( override ) && ( !defined( _MSC_VER ) || ( _MSC_VER < 1400 ) )
#define override
#endif

#endif

#if ( _MSC_VER >= 1700 )
#define MOVE_CONSTRUCTOR_SUPPORTED
#endif

#if !defined( __ATTR_SAL ) || !defined( _SAL_VERSION ) || ( _SAL_VERSION < 20 )
#include "no_sal2.h"
#endif

// ************** END OF PLATFORM-SPECIFIC DEFINITIONS

#define PINT16_MIN	(-32768)
#define PINT16_MAX	32767
#define PUINT16_MAX	0xFFFF

#define PINT32_MIN	(-2147483647 - 1)
#define PINT32_MAX	2147483647
#define PUINT32_MAX	0xFFFFFFFF

#ifdef LLONG_MAX
#define PINT64_MIN	LLONG_MIN
#define PINT64_MAX	LLONG_MAX
#define PUINT64_MAX	ULLONG_MAX
#else
#define PINT64_MIN	(-9223372036854775807 - 1)
#define PINT64_MAX	9223372036854775807
#define PUINT64_MAX	0xFFFFFFFFFFFFFFFF
#endif

#ifdef INFINITE
#	if ( INFINITE != -1 ) && ( INFINITE != 0xFFFFFFFF )
#		error INFINITE expected to be -1
#	endif
#else
#	define INFINITE (-1)
#endif

void PCurrentLocalTime( struct tm* tm );//NOTE: always sets tm->tm_yday to 0, tm->tm_isdst to -1
void PCurrentUTCTime( struct tm* tm );
int PCompareLocalTime( const struct tm* a, const struct tm* b );

char * p_itoa( _In_ INT32 val, _Pre_notnull_ _Post_z_ char * str );
char * p_utoa( _In_ UINT32 val, _Pre_notnull_ _Post_z_ char * str );
char * p_hextoa( _In_ UINT32 val, _Pre_notnull_ _Post_z_ char * str );
char * p_i64toa( _In_ INT64 val, _Pre_notnull_ _Post_z_ char * str );
char * p_u64toa( _In_ UINT64 val, _Pre_notnull_ _Post_z_ char * str );
char * p_hex64toa( _In_ UINT64 val, _Pre_notnull_ _Post_z_ char * str );
UINT64 p_atou64( _In_opt_z_ const char * str );
INT64 p_atoi64( _In_opt_z_ const char * str );
UINT64 p_atohex64( _In_opt_z_ const char * str );

class PStringEncoding;
FILE* plib_fopen( const char* fName, const char* mode, const PStringEncoding* fNameEncoding = NULL );

#define UNUSED(var) (void)var

#endif
