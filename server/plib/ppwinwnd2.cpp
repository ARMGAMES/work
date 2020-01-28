#include "ppinclude.h"
#include "ppwinwnd2.h"
#include "ppstring.h"
#include "ppi18n.h"

#ifdef PWIN32

#include <shlobj.h>

typedef LPITEMIDLIST (WINAPI *tpSHBrowseForFolderW)(LPBROWSEINFOW lpbi);
typedef BOOL (WINAPI *tpSHGetPathFromIDListW)(LPCITEMIDLIST pidl, LPWSTR pszPath);
typedef HICON (WINAPI *tpExtractIconW)(HINSTANCE hInst, LPCWSTR lpszExeFileName, UINT nIconIndex);

tpSHBrowseForFolderW wSHBrowseForFolder=0;
tpSHGetPathFromIDListW wSHGetPathFromIDList=0;
tpExtractIconW wExtractIcon=0;

HMODULE hModShell32 = 0;

void _PWin32FreeWin2Libraries()
{
	if(hModShell32) 
		::FreeLibrary(hModShell32);

	hModShell32 = 0;
}

int _PWin32InitWin2Libraries()
{

#ifndef PWIDE_ONLY

	if( !_PWinIsUnicode() )
		return 0;

#endif

	hModShell32 = ::LoadLibraryA("shell32.dll");

	if(hModShell32)
	{
		atexit( _PWin32FreeWin2Libraries );
		wSHBrowseForFolder = (tpSHBrowseForFolderW)GetProcAddress(hModShell32, "SHBrowseForFolderW");
		wSHGetPathFromIDList = (tpSHGetPathFromIDListW)GetProcAddress(hModShell32, "SHGetPathFromIDListW");
		wExtractIcon = (tpExtractIconW)GetProcAddress(hModShell32, "ExtractIconW");
	}

	return 1;
}

#ifndef BIF_NEWDIALOGSTYLE
#define BIF_NEWDIALOGSTYLE 0x0040
#endif

LONG _PWin32SHBrowseForFolder(HWND hwnd, const char *title, const char *browsePath, BFFCALLBACK lpfn, LPARAM callBackData, PString& rString, const PStringEncoding* encString)
{
	LONG ret = -1;

	PBlock wStrTitle;
	_PWin32AppendWStringFromPStringEncoding( wStrTitle, title, encString );
	_PWin32NullTerminateWString( wStrTitle );

	PBlock wStrBrowsePath;
	wStrBrowsePath.ensure( sizeof(WCHAR) * MAX_PATH );
	_PWin32AppendWStringFromPStringEncoding( wStrBrowsePath, browsePath, encString );
	_PWin32NullTerminateWString( wStrBrowsePath );

	//const UINT flags = BIF_STATUSTEXT | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
	const UINT flags = BIF_NEWDIALOGSTYLE | BIF_RETURNONLYFSDIRS;

#ifndef PWIDE_ONLY
	if( !_PWinIsUnicode() )
	{
		PBlock bufTitle;
		_PWin32WStringToA( bufTitle, (const WCHAR*)wStrTitle.ptr() );

		PBlock bufBrowsePath;
		bufBrowsePath.ensure( MAX_PATH );
		_PWin32WStringToA( bufBrowsePath, (const WCHAR*)wStrBrowsePath.ptr() );

		BROWSEINFO bi;
		
		bi.hwndOwner = hwnd;
		bi.pidlRoot = NULL;
		bi.pszDisplayName = (char*)bufBrowsePath.ptr();
		bi.lpszTitle = (char*)bufTitle.ptr();
		bi.ulFlags = flags;
		bi.lpfn = lpfn;
		bi.lParam = callBackData;
		bi.iImage = 0;
		
		LPSHELLFOLDER sf;
		ULONG eaten;
		LPITEMIDLIST root;
		SHGetDesktopFolder(&sf);

		sf->ParseDisplayName(hwnd, NULL, L"", &eaten, &root, NULL);
		bi.pidlRoot = root;
		sf->Release();

		LPITEMIDLIST pResult = ::SHBrowseForFolderA(&bi);
		if( pResult )
		{
			ret = 0;
			_PWin32SHGetPathFromIDList( (LPITEMIDLIST)pResult, rString, encString );
		}
		LPMALLOC pMalloc;
		if( !SHGetMalloc( &pMalloc ) )
			pMalloc->Free( pResult );
	}
	else
#endif
	{
		if(!wSHBrowseForFolder)
			return -1;

		BROWSEINFOW bi;
		
		bi.hwndOwner = hwnd;
		bi.pidlRoot = NULL;
		bi.pszDisplayName = (WCHAR*)wStrBrowsePath.ptr();
		bi.lpszTitle = (WCHAR*)wStrTitle.ptr();
		bi.ulFlags = flags;
		bi.lpfn = lpfn;
		bi.lParam = callBackData;
		bi.iImage = 0;
		
		LPSHELLFOLDER sf;
		ULONG eaten;
		LPITEMIDLIST root;

		SHGetDesktopFolder(&sf);
		sf->ParseDisplayName(hwnd, NULL, L"", &eaten, &root, NULL);
		bi.pidlRoot = root;
		sf->Release();

		LPITEMIDLIST pResult = wSHBrowseForFolder(&bi);
		if (pResult)
		{
			ret = 0;
			_PWin32SHGetPathFromIDList((LPITEMIDLIST)pResult, rString, encString);
		}
		LPMALLOC pMalloc;
		if (!SHGetMalloc(&pMalloc)) 
			pMalloc->Free(pResult);
	}
	return 0;
}

LONG _PWin32SHBFFSetSelection(HWND hwnd, const char *folder, const PStringEncoding* encString)
{
	if(!folder)
		return 0;
	if(!strlen(folder))
		return 0;

	LONG ret = ERROR_SUCCESS;
	PBlock wStrFolder;

	_PWin32AppendWStringFromPStringEncoding( wStrFolder, folder, encString );
	_PWin32NullTerminateWString( wStrFolder );

#ifndef PWIDE_ONLY
	if( !_PWinIsUnicode() )
	{
		PBlock buf;
		_PWin32WStringToA( buf, (const WCHAR*)wStrFolder.ptr() );
		return ::SendMessageA(hwnd, BFFM_SETSELECTIONA, TRUE, (LPARAM)buf.ptr());
	}
	else
#endif
	{
		return ::SendMessageW(hwnd, BFFM_SETSELECTIONW, TRUE, (LPARAM)wStrFolder.ptr());
	}
}

BOOL _PWin32SHGetPathFromIDList(LPCITEMIDLIST pidl, PString& rString, const PStringEncoding* encString)
{
	PBlock wStr;
	int len = MAX_PATH; 
#ifndef PWIDE_ONLY
	if( !_PWinIsUnicode() )
	{
		char* str = (char*)alloca( len + 1 );
		if( !::SHGetPathFromIDListA( pidl, str ) )
			return FALSE;

		_PWin32AStringToW( wStr, str );
	}
	else
#endif
	{
		if( wSHGetPathFromIDList )
		{
			WCHAR* wBuf = (WCHAR*)wStr.alloc( ( len + 1 ) * sizeof(WCHAR) );
			if(!wSHGetPathFromIDList( pidl, wBuf ))
				return FALSE;
		}
	}

	if( wStr.ptr() )
		_PWin32WStringToPStringEncoding( rString, (const WCHAR*)wStr.ptr(), encString );
	encString->charLength( rString );

	return TRUE;
}

LONG _PWin32RegSetStringValue(HKEY hKey, const char *lpValueName, const char *lpData, const PStringEncoding* encString)
{
	if(!lpValueName)
		return 0;
	if(!lpData)
		return 0;

	LONG ret = ERROR_SUCCESS;
	PBlock wStrName;
	PBlock wStrValue;

	_PWin32AppendWStringFromPStringEncoding( wStrName, lpValueName, encString );
	_PWin32NullTerminateWString( wStrName );
	_PWin32AppendWStringFromPStringEncoding( wStrValue, lpData, encString );
	_PWin32NullTerminateWString( wStrValue );

#ifndef PWIDE_ONLY
	if( !_PWinIsUnicode() )
	{
		PBlock bufname;
		PBlock bufvalue;
		_PWin32WStringToA( bufname, (const WCHAR*)wStrName.ptr() );
		_PWin32WStringToA( bufvalue, (const WCHAR*)wStrValue.ptr() );

		DWORD cbData=bufvalue.size();

		ret = ::RegSetValueExA(hKey, (const char *)bufname.ptr(), 0, REG_SZ, (const BYTE*)bufvalue.ptr(), bufvalue.size());
	}
	else
#endif
	{
		DWORD cbData=0;
		ret = ::RegSetValueExW(hKey, (const WCHAR*)wStrName.ptr(), NULL, REG_SZ, (const BYTE*) wStrValue.ptr(), wStrValue.size());
		//if ( ret != ERROR_SUCCESS)
	}
	return ret;
}

LONG _PWin32RegQueryStringValue(HKEY hKey, const char *lpValueName, PString& rString, const PStringEncoding* encString)
{
	DWORD type;
	LONG ret = ERROR_SUCCESS;
	PBlock wStr;
	if( lpValueName )
		_PWin32AppendWStringFromPStringEncoding( wStr, lpValueName, encString );
	_PWin32NullTerminateWString( wStr );

	PBlock wStr1;
#ifndef PWIDE_ONLY
	if( !_PWinIsUnicode() )
	{
		PBlock buf;
		_PWin32WStringToA( buf, (const WCHAR*)wStr.ptr() );

		DWORD cbData=0;

		ret = RegQueryValueExA(hKey, (const char*)buf.ptr(), NULL, NULL, 0, &cbData);
		if ( ret != ERROR_SUCCESS)
			return ret;

		if(!cbData)
			return ret;

		char* str = (char*)alloca( cbData + 1 );
		ret = RegQueryValueExA(hKey, (const char*)buf.ptr(), NULL, &type, (LPBYTE)str, &cbData);

		if ( ret != ERROR_SUCCESS)
			return ret;

		if( (type == REG_SZ || type == REG_EXPAND_SZ) && cbData>0)
			;
		else
			return ret;

		_PWin32AStringToW( wStr1, str );
	}
	else
#endif
	{
		DWORD cbData=0;

		ret = RegQueryValueExW(hKey, (const WCHAR*)wStr.ptr(), NULL, NULL, 0, &cbData);
		if ( ret != ERROR_SUCCESS)
			return ret;

		if(!cbData)
			return ret;
		cbData++;

		WCHAR* wBuf = (WCHAR*)wStr1.alloc( cbData * sizeof(WCHAR) );

		ret = RegQueryValueExW(hKey, (const WCHAR*)wStr.ptr(), NULL, &type, (LPBYTE)wBuf, &cbData);

		if ( ret != ERROR_SUCCESS)
		{
			/*
			LPVOID lpMsgBuf;
			FormatMessage( 
				FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM | 
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				ret,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				(LPTSTR) &lpMsgBuf,
				0,
				NULL 
			);
			// Process any inserts in lpMsgBuf.
			// ...
			// Display the string.
			MessageBox( NULL, (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );
			// Free the buffer.
			LocalFree( lpMsgBuf );
			*/
			return ret;
		}

		if( (type == REG_SZ || type == REG_EXPAND_SZ) && cbData>0)
			;
		else
			return ret;
	}

	if( wStr1.ptr() )
		_PWin32WStringToPStringEncoding( rString, (const WCHAR*)wStr1.ptr(), encString );
	//return encString->charLength( rString );
	return ret;
}

HRESULT _PWin32CreateLink( const PStringEncoding* encString, LPCSTR filePath, LPCSTR lnkPath, LPCSTR workDir, LPCSTR args, LPCSTR iconPath, int iconIdx )
{
	PBlock wStrFilePath;
	PBlock wStrLnkPath;
	PBlock wStrWorkDir;
	PBlock wStrArgs;
	PBlock wStrIconPath;

	_PWin32AppendWStringFromPStringEncoding( wStrFilePath, filePath, encString );
	_PWin32NullTerminateWString( wStrFilePath );
	_PWin32AppendWStringFromPStringEncoding( wStrLnkPath, lnkPath, encString );
	_PWin32NullTerminateWString( wStrLnkPath );
	_PWin32AppendWStringFromPStringEncoding( wStrWorkDir, workDir, encString );
	_PWin32NullTerminateWString( wStrWorkDir );
	if( args )
	{
		_PWin32AppendWStringFromPStringEncoding( wStrArgs, args, encString );
		_PWin32NullTerminateWString( wStrArgs );
	}
	if( iconPath && *iconPath )
	{
		_PWin32AppendWStringFromPStringEncoding( wStrIconPath, iconPath, encString );
		_PWin32NullTerminateWString( wStrIconPath );
	}

	HRESULT hres;
	IPersistFile* ppf;

#ifndef PWIDE_ONLY
	if( !_PWinIsUnicode() )
	{
		PBlock bufFilePath;
		PBlock bufLnkPath;
		PBlock bufWorkDir;
		PBlock bufArgs;
		PBlock bufIconPath;

		_PWin32WStringToA( bufFilePath, (const WCHAR*)wStrFilePath.ptr() );
		_PWin32WStringToA( bufLnkPath, (const WCHAR*)wStrLnkPath.ptr() );
		_PWin32WStringToA( bufWorkDir, (const WCHAR*)wStrWorkDir.ptr() );
		_PWin32WStringToA( bufArgs, (const WCHAR*)wStrArgs.ptr() );
		_PWin32WStringToA( bufIconPath, (const WCHAR*)wStrIconPath.ptr() );

		IShellLinkA* psl;
		hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLinkA, (LPVOID *) &psl);
		if( SUCCEEDED( hres ) )
		{ 
			psl->SetPath((char*)bufFilePath.ptr());
			psl->SetWorkingDirectory((char*)bufWorkDir.ptr());

			if( args )
				psl->SetArguments((char*)bufArgs.ptr());

			if( iconPath && *iconPath )
				psl->SetIconLocation( (char*)bufIconPath.ptr(), iconIdx );

			hres = psl->QueryInterface(IID_IPersistFile,(LPVOID*)&ppf);
			if( SUCCEEDED( hres ) )
			{
				hres = ppf->Save( (WCHAR*)wStrLnkPath.ptr(), TRUE );
				ppf->Release();
			}
			psl->Release();
		}
	}
	else
#endif
	{
		IShellLinkW* psl;
		hres = CoCreateInstance( CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLinkW, (LPVOID *) &psl );
		if( SUCCEEDED( hres ) )
		{
			psl->SetPath( (WCHAR*)wStrFilePath.ptr() );
			psl->SetWorkingDirectory( (WCHAR*)wStrWorkDir.ptr() );

			if( args )
				psl->SetArguments( (WCHAR*)wStrArgs.ptr() );

			if( iconPath && *iconPath )
				psl->SetIconLocation( (WCHAR*)wStrIconPath.ptr(), iconIdx );

			hres = psl->QueryInterface(IID_IPersistFile,(LPVOID*)&ppf);
			if( SUCCEEDED( hres ) )
			{
				hres = ppf->Save((WCHAR*)wStrLnkPath.ptr(), TRUE);
				ppf->Release();
			}
			psl->Release();
		}
	}

	return hres;
}

HICON _PWin32ExtractIcon( HINSTANCE inst, LPCSTR exeFileName, UINT iconIndex, const PStringEncoding* encString)
{
	if(!exeFileName)
		return 0;
	if(!strlen(exeFileName))
		return 0;

	LONG ret = ERROR_SUCCESS;
	PBlock wStrFileName;

	_PWin32AppendWStringFromPStringEncoding( wStrFileName, exeFileName, encString );
	_PWin32NullTerminateWString( wStrFileName );

#ifndef PWIDE_ONLY
	if( !_PWinIsUnicode() )
	{
		PBlock buf;
		_PWin32WStringToA( buf, (const WCHAR*)wStrFileName.ptr() );
		return ::ExtractIcon(inst, (const char *)buf.ptr(), iconIndex);
	}
	else
#endif
	{
		if(wExtractIcon)
			return ::wExtractIcon(inst, (const WCHAR*)wStrFileName.ptr(), iconIndex);
	}
	return 0;
}

int getTimeZoneBias()
{
	TIME_ZONE_INFORMATION tmzoneinfo;
	GetTimeZoneInformation(&tmzoneinfo);

	time_t ltime;
	struct tm *today;
	time( &ltime );
	struct tm *gmt = gmtime(&ltime);
	gmt->tm_isdst = 0;
	today = localtime( &ltime );

	if(today->tm_isdst)
		return (tmzoneinfo.Bias + tmzoneinfo.DaylightBias)*60;
	else
		return tmzoneinfo.Bias*60;
}

#endif
