#if defined(_WIN32) && !defined(COMM_CLIENT_ONLY)

#include "ppstacktrace.h"
#include <cstdio>
#include <list>
#include <regex>
#include <Psapi.h>

#ifdef _WIN64
#	if defined(_DEBUG)
#		define SKIP 2
#	else // !_DEBUG
#		define SKIP 1
#	endif // _DEBUG
#else
#	if defined(_DEBUG)
#		define SKIP 1
#	else // !_DEBUG
#		define SKIP 0
#	endif // _DEBUG
#endif

namespace
{

struct Initializer
{
	Initializer() : m_inited(SymInitialize(GetCurrentProcess(), NULL, TRUE)) {}
	~Initializer()
	{
		if (m_inited)
		{
			SymCleanup(GetCurrentProcess());
		}
	}
	bool m_inited;
};
const Initializer dummy;
CRITICAL_SECTION stackTraceMutex;

bool initStackTraceMutex()
{
	InitializeCriticalSection(&stackTraceMutex);
	return true;
}

} // unnamed namespace

bool StackTrace::captureStackTrace(DWORD64* frame_ptrs, size_t count, size_t skip, CONTEXT* ctx)
{
	if (!dummy.m_inited)
	{
		return false;
	}

	CONTEXT tmp, *current_context = nullptr;
	if (ctx)
	{
		current_context = ctx;
	}
	else
	{
		RtlCaptureContext(&tmp);
		current_context = &tmp;
	}

	DWORD machine_type;
	STACKFRAME64 frame;
	ZeroMemory(&frame, sizeof(frame));
	frame.AddrPC.Mode = AddrModeFlat;
	frame.AddrFrame.Mode = AddrModeFlat;
	frame.AddrStack.Mode = AddrModeFlat;
#ifdef _WIN64
	frame.AddrPC.Offset = current_context->Rip;
	frame.AddrFrame.Offset = current_context->Rbp;
	frame.AddrStack.Offset = current_context->Rsp;
	machine_type = IMAGE_FILE_MACHINE_AMD64;
#else
	frame.AddrPC.Offset = current_context->Eip;
	frame.AddrFrame.Offset = current_context->Ebp;
	frame.AddrStack.Offset = current_context->Esp;
	machine_type = IMAGE_FILE_MACHINE_I386;
#endif  
	for (size_t i = 0; i < count + skip; ++i)
	{
		if (!StackWalk64(machine_type, GetCurrentProcess(), GetCurrentThread(), &frame, current_context, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL))
		{
			break;
		}
		if (i >= skip)
		{
			frame_ptrs[i - skip] = frame.AddrPC.Offset;
		}
	}
	return true;
}

bool StackTrace::getSymbolInfo(DWORD64 addr, char*& dst, int& n)
{
	if (!dummy.m_inited || n <= 0)
	{
		return false;
	}
	DWORD64 displacement64;
	MEMORY_BASIC_INFORMATION info;
	
	char symbolBuffer[sizeof(SYMBOL_INFO) + 256];
	SYMBOL_INFO* symbol = reinterpret_cast<SYMBOL_INFO*>(symbolBuffer);
	symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
	symbol->MaxNameLen = 255;
	
	bool haveVirtualQuery = VirtualQuery((void*)addr, &info, sizeof(MEMORY_BASIC_INFORMATION));
	char moduleName[MAX_PATH];
	bool haveModuleName = (haveVirtualQuery && GetModuleBaseNameA(GetCurrentProcess(), (HMODULE)info.AllocationBase, moduleName, sizeof(moduleName)));

	int nw = snprintf(dst, n, "%llX", addr);
	dst += nw;
	n -= nw;
	
	if (n > 0 && haveVirtualQuery)
	{
#ifdef _WIN64
		nw = snprintf(dst, n, " %llX", (UINT64)info.AllocationBase);
#else
		nw = snprintf(dst, n, " %X", (UINT32)info.AllocationBase);
#endif
		dst += nw;
		n -= nw;
	}
	
	if (n > 0 && haveModuleName)
	{
		nw = snprintf(dst, n, " (%s)", moduleName);
		dst += nw;
		n -= nw;
	}

	if (n > 0 && SymFromAddr(GetCurrentProcess(), addr, &displacement64, symbol))
	{
		nw = snprintf(dst, n, " %s", symbol->Name);
		dst += nw;
		n -= nw;

		DWORD displacement;
		IMAGEHLP_LINE64 line;
		line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
		if (n > 0 && SymGetLineFromAddr64(GetCurrentProcess(), addr, &displacement, &line))
		{
			nw = snprintf(dst, n, " %s:%ld", line.FileName, line.LineNumber);
			dst += nw;
			n -= nw;
		}
	}
	if (n > 0)
	{
		nw = snprintf(dst, n, "\r\n");
		dst += nw;
		n -= nw;
	}
	return n > 0;
}

void StackTrace::getStackTrace(char* dst, int n, CONTEXT* ctx)
{
	static const bool stackTraceMutexInited = initStackTraceMutex();
	(void)stackTraceMutexInited;
	static constexpr size_t max_frame_ptrs = 16;
	DWORD64 m_frame_ptrs[max_frame_ptrs];
	ZeroMemory(m_frame_ptrs, sizeof(m_frame_ptrs));
	EnterCriticalSection(&stackTraceMutex);
	if (captureStackTrace(m_frame_ptrs, max_frame_ptrs, SKIP, ctx))
	{
		for (size_t i = 0; i < max_frame_ptrs && m_frame_ptrs[i]; ++i)
		{
			if (!getSymbolInfo(m_frame_ptrs[i], dst, n))
			{
				break;
			}
		}
	}
	LeaveCriticalSection(&stackTraceMutex);
}

void StackTrace::decodeStackTrace(const char* file)
{
	struct InternalModuleInfo
	{
		MODULEINFO moduleInfo;
		char modulePath[MAX_PATH];
	};

	std::list<InternalModuleInfo> moduleList;
	HMODULE hMod[1024];
	DWORD cbNeeded;
	if (EnumProcessModules(GetCurrentProcess(), hMod, sizeof(hMod), &cbNeeded))
	{
		if (cbNeeded > sizeof(hMod))
		{
			printf("Too many loaded modules");
			return;
		}
		for (size_t i = 0, numModulesFound = cbNeeded / sizeof(HMODULE); i < numModulesFound; ++i)
		{
			InternalModuleInfo tempInfo;
			if (GetModuleInformation(GetCurrentProcess(), hMod[i], &tempInfo.moduleInfo, sizeof(tempInfo.moduleInfo)) && GetModuleFileNameExA(GetCurrentProcess(), hMod[i], tempInfo.modulePath, sizeof(tempInfo.modulePath)))
			{
				moduleList.emplace_back(tempInfo);
			}
		}

		FILE* f = fopen(file, "r");
		if (!f)
		{
			printf("Could not open file %s for reading", file);
			return;
		}

		char line[512];
		while (fgets(line, sizeof(line), f))
		{
			if (!line[0] || line[0] == '\r' || line[0] == '\n')
			{
				continue;
			}
			line[strlen(line) - 1] = 0;

			std::cmatch match;

			static const std::regex frameLineRegex("([0-9A-F]*) ([0-9A-F]*) \\((.*)\\).*");
			if (std::regex_match(line, match, frameLineRegex))
			{
				auto& mmn = match[3];
				char moduleName[MAX_PATH + 1];
				memset(moduleName, 0, sizeof(moduleName));
				strncpy(moduleName, mmn.first, min(MAX_PATH, mmn.length()));
				auto it = std::find_if(moduleList.cbegin(), moduleList.cend(), [moduleName](const InternalModuleInfo& imi){return std::strstr(imi.modulePath, moduleName);});
				if (it != moduleList.cend())
				{
					auto& ma = match[1];
					char addrStr[32];
					memset(addrStr, 0, sizeof(addrStr));
					strncpy(addrStr, ma.first, min(31, ma.length()));
#ifdef _WIN64
					DWORD64 addr = strtoull(addrStr, nullptr, 16);
#else
					DWORD32 addr = strtoul(addrStr, nullptr, 16);
#endif
					
					auto& maba = match[2];
					char allocationBaseAddrStr[32];
					memset(allocationBaseAddrStr, 0, sizeof(allocationBaseAddrStr));
					strncpy(allocationBaseAddrStr, maba.first, min(31, maba.length()));
#ifdef _WIN64
					UINT64 allocationBaseAddr = strtoull(allocationBaseAddrStr, nullptr, 16);
#else
					UINT32 allocationBaseAddr = strtoul(allocationBaseAddrStr, nullptr, 16);
#endif
					
					auto funcOffset = addr - allocationBaseAddr;
					decltype(funcOffset) funcAddr = reinterpret_cast<decltype(funcOffset)>((*it).moduleInfo.lpBaseOfDll) + funcOffset;
					DWORD64 displacement64;
					char symbolBuffer[sizeof(SYMBOL_INFO) + 256];
					SYMBOL_INFO* symbol = reinterpret_cast<SYMBOL_INFO*>(symbolBuffer);
					symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
					symbol->MaxNameLen = 255;
					if (SymFromAddr(GetCurrentProcess(), funcAddr, &displacement64, symbol))
					{
						DWORD displacement;
						IMAGEHLP_LINE64 line;
						line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
						printf("%s", symbol->Name);
						if (SymGetLineFromAddr64(GetCurrentProcess(), addr, &displacement, &line))
						{
							printf(" %s:%ld", line.FileName, line.LineNumber);
						}
						printf("\r\n");
					}
					else
					{
						printf("%s\r\n", line);
					}
				}
			}
		}

		fclose(f);
	}
}

#endif // _WIN32
