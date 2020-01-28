#ifndef ppwinwnd2_h_included
#define ppwinwnd2_h_included
//------------------------------------------------------------------------------

#if defined( PWIN32 )
//------------------------------------------------------------------------------

#include <shlobj.h>

int _PWin32InitWin2Libraries();

LONG _PWin32RegQueryStringValue( HKEY hKey, const char *lpValueName, PString& rString, const PStringEncoding* encString );
LONG _PWin32RegSetStringValue( HKEY hKey, const char *lpValueName, const char *lpData, const PStringEncoding* encString );

BOOL _PWin32SHGetPathFromIDList( LPCITEMIDLIST pidl, PString& rString, const PStringEncoding* encString );

LONG _PWin32SHBFFSetSelection( HWND hwnd, const char *folder, const PStringEncoding* encString );
LONG _PWin32SHBrowseForFolder( HWND hwnd, const char *title, const char *browsePath, BFFCALLBACK lpfn, LPARAM callBackData,
	PString& rString, const PStringEncoding* encString );

HRESULT _PWin32CreateLink( const PStringEncoding* encString, LPCSTR filePath, LPCSTR lnkPath,
	LPCSTR workDir, LPCSTR args, LPCSTR iconPath, int iconIdx );

HICON _PWin32ExtractIcon( HINSTANCE inst, LPCSTR exeFileName, UINT iconIndex, const PStringEncoding* encString );

int getTimeZoneBias();

//------------------------------------------------------------------------------
#endif	// defined( PWIN32 )

//------------------------------------------------------------------------------
#endif	// ppwinwnd2_h_included
