// This file should NOT included into plib.lib
// To activate it, it should be linked into executable explicitly 

// It's only purpose is to replace default VC++ runtime __heap_select()
// to enforce using VC6 heap instead of default one (Win2k heap on Win2k computers)
#if defined( _WIN32 ) && !defined( _WIN64 )

#include <windows.h>

#define USE_LFH
// #define USE_VC6

#ifdef USE_LFH

typedef BOOL (WINAPI *HSI)(
	HANDLE HeapHandle,
	int HeapInformationClass,
	PVOID HeapInformation,
	SIZE_T HeapInformationLength
);
#endif

extern "C"
{

extern HANDLE _crtheap;

int __cdecl __heap_select(void)
{
#ifdef USE_VC6
	return 3;
#elif defined USE_LFH
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if( GetVersionEx(&osvi) )
	{
		if( osvi.dwPlatformId != VER_PLATFORM_WIN32_NT )
			return 1;
		if( osvi.dwMajorVersion < 5 )
			return 1;
		if( ( osvi.dwMajorVersion == 5 ) && ( osvi.dwMinorVersion < 2 ) )
			return 1;
	}
	HMODULE hl = LoadLibraryA( "Kernel32.dll" );
	if( hl )
	{
		HSI fp = (HSI)GetProcAddress( hl, "HeapSetInformation" );
		if( fp )
		{
			ULONG u = 2; // LFH
			fp( _crtheap, 0, &u, sizeof( u ) );
		}
		FreeLibrary( hl );
	}
	return 1;
#else
	return 1;
#endif
}

}
#endif