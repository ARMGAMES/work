#ifndef ppstacktrace_h_included
#define ppstacktrace_h_included

#if defined(COMM_CLIENT_ONLY)

class StackTrace
{
	static bool captureStackTrace(DWORD64* frame_ptrs, size_t count, size_t skip, CONTEXT* ctx) {return false;}
	static bool getSymbolInfo(DWORD64 addr, char*& dst, int& n) {return false;}

public:
	static void getStackTrace(char* dst, int n, CONTEXT* ctx = nullptr) {}
	static void decodeStackTrace(const char* file) {}
};

#else

#include <windows.h>
#include <DbgHelp.h>

class StackTrace
{
	static bool captureStackTrace(DWORD64* frame_ptrs, size_t count, size_t skip, CONTEXT* ctx);
	static bool getSymbolInfo(DWORD64 addr, char*& dst, int& n);
	
public:
	static void getStackTrace(char* dst, int n, CONTEXT* ctx = nullptr);
	static void decodeStackTrace(const char* file);
};

#endif // COMM_CLIENT_ONLY

#endif // ppstacktrace_h_included
