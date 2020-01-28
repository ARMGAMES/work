#include "ppinclude.h"
#include "ppwinwnd.h"
#include "ppstring.h"
#include "ppi18n.h"

#ifdef PWIN32

#include <commctrl.h>
#include <shellapi.h>

ATOM _PWin32RegisterClassEx( const WCHAR* lpClassName, UINT uClassStyle, WNDPROC lpWindowProc,
	HINSTANCE hInstance, HCURSOR hCursor, HBRUSH hbrBackground, HICON hIcon )
{
#ifndef PWIDE_ONLY
	if( !_PWinIsUnicode() )
	{
		WNDCLASSEXA wcx;
		memset( &wcx, 0, sizeof(wcx) );
		wcx.cbSize = sizeof(wcx);
		wcx.style = uClassStyle;
		wcx.lpfnWndProc = lpWindowProc;
		wcx.hInstance = hInstance;
		wcx.hCursor = hCursor;
		wcx.hbrBackground = hbrBackground;
		wcx.hIcon = hIcon;
		PBlock buf;
		_PWin32WStringToA( buf, lpClassName );
		wcx.lpszClassName = (const char*)buf.ptr();
		return ::RegisterClassExA( &wcx );
	}
	else
#endif
	{
		WNDCLASSEXW wcx;
		memset( &wcx, 0, sizeof(wcx) );
		wcx.cbSize = sizeof(wcx);
		wcx.style = uClassStyle;
		wcx.lpfnWndProc = lpWindowProc;
		wcx.hInstance = hInstance;
		wcx.hCursor = hCursor;
		wcx.hbrBackground = hbrBackground;
		wcx.hIcon = hIcon;
		wcx.lpszClassName = lpClassName;
		return ::RegisterClassExW( &wcx );
	}
}

ATOM _PWin32RegisterSuperClassEx( const WCHAR* lpClassName, WNDPROC lpWindowProc,
	const WCHAR* lpBaseClassName, WNDPROC& lpBaseWindowProc, HINSTANCE hInstance )
{
#ifndef PWIDE_ONLY
	if( !_PWinIsUnicode() )
	{
		WNDCLASSEXA wcx;
		memset( &wcx, 0, sizeof(wcx) );
		wcx.cbSize = sizeof(wcx);
		PBlock buf;
		_PWin32WStringToA( buf, lpBaseClassName );
		if( ::GetClassInfoExA( NULL, (const char*)buf.ptr(), &wcx ) )
		{
			wcx.style &= ~CS_GLOBALCLASS;
			lpBaseWindowProc = wcx.lpfnWndProc;
			wcx.lpfnWndProc = lpWindowProc;
			wcx.hInstance = hInstance;
			_PWin32WStringToA( buf, lpClassName );
			wcx.lpszClassName = (const char*)buf.ptr();
			return ::RegisterClassExA( &wcx );
		}
	}
	else
#endif
	{
		WNDCLASSEXW wcx;
		memset( &wcx, 0, sizeof(wcx) );
		wcx.cbSize = sizeof(wcx);
		if( ::GetClassInfoExW( NULL, lpBaseClassName, &wcx ) )
		{
			wcx.style &= ~CS_GLOBALCLASS;
			lpBaseWindowProc = wcx.lpfnWndProc;
			wcx.lpfnWndProc = lpWindowProc;
			wcx.hInstance = hInstance;
			wcx.lpszClassName = lpClassName;
			return ::RegisterClassExW( &wcx );
		}
	}
	return 0;
}

HWND _PWin32CreateWindowEx( DWORD dwExStyle, const WCHAR* lpClassName, const WCHAR* lpWindowName, DWORD dwStyle,
	int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam )
{
#ifndef PWIDE_ONLY
	if( !_PWinIsUnicode() )
	{
		PBlock className, windowName;
		_PWin32WStringToA( className, lpClassName );
		_PWin32WStringToA( windowName, lpWindowName );
		return ::CreateWindowExA( dwExStyle, (const char*)className.ptr(), (const char*)windowName.ptr(),
			dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam );
	}
	else
#endif
	{
		return ::CreateWindowExW( dwExStyle, lpClassName, lpWindowName, dwStyle,
			x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam );
	}
}

LRESULT _PWin32DefWindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
#ifndef PWIDE_ONLY
	if( !_PWinIsUnicode() )
		return ::DefWindowProcA( hWnd, uMsg, wParam, lParam );
	else
#endif
		return ::DefWindowProcW( hWnd, uMsg, wParam, lParam );
}

BOOL _PWin32GetMessage( MSG* lpMsg, HWND hWnd, UINT uMsgFilterMin, UINT uMsgFilterMax )
{
#ifndef PWIDE_ONLY
	if( !_PWinIsUnicode() )
		return ::GetMessageA( lpMsg, hWnd, uMsgFilterMin, uMsgFilterMax );
	else
#endif
		return ::GetMessageW( lpMsg, hWnd, uMsgFilterMin, uMsgFilterMax );
}

LRESULT _PWin32DispatchMessage( const MSG* lpMsg )
{
#ifndef PWIDE_ONLY
	if( !_PWinIsUnicode() )
		return ::DispatchMessageA( lpMsg );
	else
#endif
		return ::DispatchMessageW( lpMsg );
}

BOOL _PWin32PeekMessage( MSG* lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg )
{
#ifndef PWIDE_ONLY
	if( !_PWinIsUnicode() )
		return ::PeekMessageA( lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg );
	else
#endif
		return ::PeekMessageW( lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg );
}

LRESULT _PWin32SendMessage( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
#ifndef PWIDE_ONLY
	if( !_PWinIsUnicode() )
		return ::SendMessageA( hWnd, uMsg, wParam, lParam );
	else
#endif
		return ::SendMessageW( hWnd, uMsg, wParam, lParam );
}

BOOL _PWin32PostMessage( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
#ifndef PWIDE_ONLY
	if( !_PWinIsUnicode() )
	{
		if( ::PostMessageA( hWnd, uMsg, wParam, lParam ) )
			return TRUE;

		PASSERT4( 0 );
		return FALSE;
	}
	else
#endif
	{
		if( ::PostMessageW( hWnd, uMsg, wParam, lParam ) )
			return TRUE;

		PASSERT4( 0 );
		return FALSE;
	}
}

BOOL _PWin32PostThreadMessage( DWORD idThread, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
#ifndef PWIDE_ONLY
	if( !_PWinIsUnicode() )
	{
		if( ::PostThreadMessageA( idThread, uMsg, wParam, lParam ) )
			return TRUE;

		PASSERT4( 0 );
		return FALSE;
	}
	else
#endif
	{
		if( ::PostThreadMessageW( idThread, uMsg, wParam, lParam ) )
			return TRUE;

		PASSERT4( 0 );
		return FALSE;
	}
}

LONG _PWin32GetWindowLong( HWND hWnd, int nIndex )
{
#ifndef PWIDE_ONLY
	if( !_PWinIsUnicode() )
		return ::GetWindowLongA( hWnd, nIndex );
	else
#endif
		return ::GetWindowLongW( hWnd, nIndex );
}

LONG _PWin32SetWindowLong( HWND hWnd, int nIndex, LONG dwNewLong )
{
#ifndef PWIDE_ONLY
	if( !_PWinIsUnicode() )
		return ::SetWindowLongA( hWnd, nIndex, dwNewLong );
	else
#endif
		return ::SetWindowLongW( hWnd, nIndex, dwNewLong );
}

int _PWin32GetWindowText( HWND hWnd, PString& rString, const PStringEncoding* encString, bool convertCrLf )
{
#ifndef PWIDE_ONLY
	if( encString == NULL )
	{
		if( int len = ::GetWindowTextLengthA( hWnd ) )
		{
			char* str = (char*)alloca( len + 1 );
			::GetWindowTextA( hWnd, str, len + 1 );
			rString.assign( str );
		}
		return rString.length();
	}
#else
	PASSERT( encString );
#endif

	PBlock wStr;
#ifndef PWIDE_ONLY
	if( !_PWinIsUnicode() )
	{
		if( int len = ::GetWindowTextLengthA( hWnd ) )
		{
			char* str = (char*)alloca( len + 1 );
			::GetWindowTextA( hWnd, str, len + 1 );
			_PWin32AStringToW( wStr, str );
		}
	}
	else
#endif
	{
		if( int len = ::GetWindowTextLengthW( hWnd ) )
		{
			WCHAR* wBuf = (WCHAR*)wStr.alloc( ( len + 1 ) * sizeof(WCHAR) );
			::GetWindowTextW( hWnd, wBuf, len + 1 );
		}
	}

	if( rString.length() )
		rString.cut( 0 );
	if( wStr.ptr() )
	{
		if( convertCrLf )
			_PWin32WStringToPStringEncodingC( rString, (const WCHAR*)wStr.ptr(), encString );
		else
			_PWin32WStringToPStringEncoding( rString, (const WCHAR*)wStr.ptr(), encString );
	}
	return static_cast< int >( rString.length() );
}

int _PWin32GetWindowTextLength( HWND hWnd )
{
#ifndef PWIDE_ONLY
	if( !_PWinIsUnicode() )
		return ::GetWindowTextLengthA( hWnd );
	else
#endif
		return ::GetWindowTextLengthW( hWnd );
}

BOOL _PWin32SetWindowText( HWND hWnd, const char* lpString, const PStringEncoding* encString, bool convertLf )
{
	if( encString == NULL )
#ifdef PWIDE_ONLY
	{
		PBlock wStr;
		_PWin32AStringToW( wStr, lpString );
		return ::SetWindowTextW( hWnd, (const WCHAR*)wStr.ptr() );
	}
#else
	{
		return ::SetWindowTextA( hWnd, lpString );
	}
#endif

	PBlock wStr;
	if( lpString )
	{
		if( convertLf )
			_PWin32AppendWStringFromPStringEncodingC( wStr, lpString, encString );
		else
			_PWin32AppendWStringFromPStringEncoding( wStr, lpString, encString );
	}
	_PWin32NullTerminateWString( wStr );
#ifndef PWIDE_ONLY
	if( !_PWinIsUnicode() )
	{
		PBlock buf;
		if( lpString )
			_PWin32WStringToA( buf, (const WCHAR*)wStr.ptr() );
		return ::SetWindowTextA( hWnd, (const char*)buf.ptr() );
	}
	else
#endif
	{
		return ::SetWindowTextW( hWnd, (const WCHAR*)wStr.ptr() );
	}
}

HWND _PWin32CreateDialogIndirectParam( HINSTANCE hInstance, LPCDLGTEMPLATE lpDialogTemplate, HWND hWndParent, DLGPROC lpDialogProc, LPARAM lParamInit )
{
#ifndef PWIDE_ONLY
	if( !_PWinIsUnicode() )
		return ::CreateDialogIndirectParamA( hInstance, lpDialogTemplate,
		hWndParent, lpDialogProc, lParamInit );
	else
#endif
		return ::CreateDialogIndirectParamW( hInstance, lpDialogTemplate,
		hWndParent, lpDialogProc, lParamInit );
}

LRESULT _PWin32CallWindowProc( WNDPROC lpWindowProc, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
#ifndef PWIDE_ONLY
	if( !_PWinIsUnicode() )
		return ::CallWindowProcA( lpWindowProc, hWnd, uMsg, wParam, lParam );
	else
#endif
		return ::CallWindowProcW( lpWindowProc, hWnd, uMsg, wParam, lParam );
}

#define MAX_TOOLTIP_LEN		0x03ff
#define MAX_TOOLTIP_SIZE	((MAX_TOOLTIP_LEN)*sizeof(WCHAR))

void _PWin32AddToolTip( HWND hWndToolTip, HWND hWnd, const char* lpString, const PStringEncoding* encString, bool convertLf )
{
	PBlock wStr;
	if( lpString && lpString != LPSTR_TEXTCALLBACKA )
	{
		if( convertLf )
			_PWin32AppendWStringFromPStringEncodingC( wStr, lpString, encString );
		else
			_PWin32AppendWStringFromPStringEncoding( wStr, lpString, encString );
		if( wStr.size() > MAX_TOOLTIP_SIZE )
			wStr.cut( MAX_TOOLTIP_SIZE );
	}
	_PWin32NullTerminateWString( wStr );
#ifndef PWIDE_ONLY
	if( !_PWinIsUnicode() )
	{
		TOOLINFOA ti;
		::ZeroMemory( &ti, sizeof(ti) );
		ti.cbSize = sizeof(ti);
		ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
		ti.uId = (UINT_PTR)hWnd;
		if( lpString != LPSTR_TEXTCALLBACKA )
		{
			PBlock buf;
			_PWin32WStringToA( buf, (const WCHAR*)wStr.ptr() );
			ti.lpszText = (char*)buf.ptr();
			::SendMessageA( hWndToolTip, TTM_ADDTOOLA, 0, (LPARAM)&ti );
		}
		else
		{
			ti.lpszText = (char*)lpString;
			ti.hwnd = hWnd;
			::SendMessageA( hWndToolTip, TTM_ADDTOOLA, 0, (LPARAM)&ti );
		}
	}
	else
#endif
	{
		TOOLINFOW ti;
		::ZeroMemory( &ti, sizeof(ti) );
		ti.cbSize = sizeof(ti);
		ti.uFlags = TTF_IDISHWND | TTF_SUBCLASS;
		ti.uId = (UINT_PTR)hWnd;
		if( lpString != LPSTR_TEXTCALLBACKA )
		{
			ti.lpszText = (WCHAR*)wStr.ptr();
		}
		else
		{
			ti.lpszText = (WCHAR*)lpString;
			ti.hwnd = hWnd;
		}
		::SendMessageW( hWndToolTip, TTM_ADDTOOLW, 0, (LPARAM)&ti );
	}
}

void _PWin32SetToolTip( HWND hWndToolTip, HWND hWnd, const char* lpString, const PStringEncoding* encString, bool convertLf )
{
	PBlock wStr;
	if( lpString )
	{
		if( convertLf )
			_PWin32AppendWStringFromPStringEncodingC( wStr, lpString, encString );
		else
			_PWin32AppendWStringFromPStringEncoding( wStr, lpString, encString );
		if( wStr.size() > MAX_TOOLTIP_SIZE )
			wStr.cut( MAX_TOOLTIP_SIZE );
	}
	_PWin32NullTerminateWString( wStr );
#ifndef PWIDE_ONLY
	if( !_PWinIsUnicode() )
	{
		TOOLINFOA ti;
		::ZeroMemory( &ti, sizeof(ti) );
		ti.cbSize = sizeof(ti);
		ti.uFlags = TTF_IDISHWND;
		ti.uId = (UINT_PTR)hWnd;
		char buff[ MAX_TOOLTIP_LEN + 1 ];
		ti.lpszText = buff;
		if( ::SendMessageA( hWndToolTip, TTM_GETTOOLINFOA, 0, (LPARAM)&ti ) )
		{
			PBlock buf;
			_PWin32WStringToA( buf, (const WCHAR*)wStr.ptr() );
			if( PString::compare( (const char*)buf.ptr(), buff ) )
			{
				ti.lpszText = (char*)buf.ptr();
				::SendMessageA( hWndToolTip, TTM_SETTOOLINFOA, 0, (LPARAM)&ti );
			}
		}
	}
	else
#endif
	{
		TOOLINFOW ti;
		::ZeroMemory( &ti, sizeof(ti) );
		ti.cbSize = sizeof(ti);
		ti.uFlags = TTF_IDISHWND;
		ti.uId = (UINT_PTR)hWnd;
		WCHAR wBuf[ MAX_TOOLTIP_LEN + 1 ];
		ti.lpszText = wBuf;
		if( ::SendMessageW( hWndToolTip, TTM_GETTOOLINFOW, 0, (LPARAM)&ti ) )
		{
			if( ::lstrcmpW( (const WCHAR*)wStr.ptr(), wBuf ) )
			{
				ti.lpszText = (WCHAR*)wStr.ptr();
				::SendMessageW( hWndToolTip, TTM_SETTOOLINFOW, 0, (LPARAM)&ti );
			}
		}
	}
}

int _PWin32ComboBoxAddString( HWND hWnd, const char* lpString, const PStringEncoding* encString )
{
	if( encString == NULL )
#ifdef PWIDE_ONLY
	{
		PBlock wStr;
		_PWin32AStringToW( wStr, lpString );
		return (int)::SendMessageW( hWnd, CB_ADDSTRING, 0, (LPARAM)wStr.ptr() );
	}
#else
	{
		return (int)::SendMessageA( hWnd, CB_ADDSTRING, 0, (LPARAM)lpString );
	}
#endif

	PBlock wStr;
	if( lpString )
		_PWin32AppendWStringFromPStringEncoding( wStr, lpString, encString );
	_PWin32NullTerminateWString( wStr );
#ifndef PWIDE_ONLY
	if( !_PWinIsUnicode() )
	{
		PBlock buf;
		_PWin32WStringToA( buf, (const WCHAR*)wStr.ptr() );
		return (int)::SendMessageA( hWnd, CB_ADDSTRING, 0, (LPARAM)buf.ptr() );
	}
	else
#endif
	{
		return (int)::SendMessageW( hWnd, CB_ADDSTRING, 0, (LPARAM)wStr.ptr() );
	}
}

int _PWin32ListBoxAddString( HWND hWnd, const char* lpString, const PStringEncoding* encString )
{
	if( encString == NULL )
#ifdef PWIDE_ONLY
	{
		PBlock wStr;
		_PWin32AStringToW( wStr, lpString );
		return (int)::SendMessageW( hWnd, LB_ADDSTRING, 0, (LPARAM)wStr.ptr() );
	}
#else
	{
		return (int)::SendMessageA( hWnd, LB_ADDSTRING, 0, (LPARAM)lpString );
	}
#endif

	PBlock wStr;
	if( lpString )
		_PWin32AppendWStringFromPStringEncoding( wStr, lpString, encString );
	_PWin32NullTerminateWString( wStr );
#ifndef PWIDE_ONLY
	if( !_PWinIsUnicode() )
	{
		PBlock buf;
		_PWin32WStringToA( buf, (const WCHAR*)wStr.ptr() );
		return (int)::SendMessageA( hWnd, LB_ADDSTRING, 0, (LPARAM)buf.ptr() );
	}
	else
#endif
	{
		return (int)::SendMessageW( hWnd, LB_ADDSTRING, 0, (LPARAM)wStr.ptr() );
	}
}

BOOL _PWin32AppendMenu( HMENU hMenu, UINT uFlags, UINT_PTR uId, const char* lpString, const PStringEncoding* encString )
{
	if( ( uFlags & MF_OWNERDRAW ) == MF_OWNERDRAW )
	{
#ifndef PWIDE_ONLY
		if( !_PWinIsUnicode() )
		{
			return ::AppendMenuA( hMenu, uFlags, uId, lpString );
		}
		else
#endif
		{
			return ::AppendMenuW( hMenu, uFlags, uId, (const WCHAR*)lpString );
		}
	}

	if( encString == NULL )
#ifdef PWIDE_ONLY
	{
		PBlock wStr;
		_PWin32AStringToW( wStr, lpString );
		return ::AppendMenuW( hMenu, uFlags, uId, (const WCHAR*)wStr.ptr() );
	}
#else
	{
		return ::AppendMenuA( hMenu, uFlags, uId, lpString );
	}
#endif

	PBlock wStr;
	if( lpString )
		_PWin32AppendWStringFromPStringEncoding( wStr, lpString, encString );
	_PWin32NullTerminateWString( wStr );
#ifndef PWIDE_ONLY
	if( !_PWinIsUnicode() )
	{
		PBlock buf;
		_PWin32WStringToA( buf, (const WCHAR*)wStr.ptr() );
		return ::AppendMenuA( hMenu, uFlags, uId, (const char*)buf.ptr() );
	}
	else
#endif
	{
		return ::AppendMenuW( hMenu, uFlags, uId, (const WCHAR*)wStr.ptr() );
	}
}

BOOL _PWin32GetModuleFileName( HMODULE hModule, PString& rPath, const PStringEncoding* encString )
{
	if( encString == NULL )
	{
		char buf[ MAX_PATH + 1 ];
		::GetModuleFileNameA( hModule, buf, sizeof(buf) );
		rPath.assign( buf );
		return static_cast< BOOL >( rPath.length() );
	}

	PBlock wStr;
#ifndef PWIDE_ONLY
	if( !_PWinIsUnicode() )
	{
		char buf[ MAX_PATH + 1 ];
		::GetModuleFileNameA( hModule, buf, sizeof(buf) );
		_PWin32AStringToW( wStr, buf );
	}
	else
#endif
	{
		WCHAR* wBuf = (WCHAR*)wStr.alloc( ( MAX_PATH + 1 ) * sizeof(WCHAR) );
		::GetModuleFileNameW( hModule, wBuf, MAX_PATH + 1 );
	}

	if( rPath.length() )
		rPath.cut( 0 );
	if( wStr.ptr() )
		_PWin32WStringToPStringEncoding( rPath, (const WCHAR*)wStr.ptr(), encString );
	return static_cast< BOOL >( rPath.length() );
}

BOOL _PWin32GetWindowsDirectory( PString& rPath, const PStringEncoding* encString )
{
	if( encString == NULL )
	{
		char buf[ MAX_PATH + 1 ];
		::GetWindowsDirectoryA( buf, sizeof(buf) );
		rPath.assign( buf );
		return static_cast< BOOL >( rPath.length() );
	}

	PBlock wStr;
#ifndef PWIDE_ONLY
	if( !_PWinIsUnicode() )
	{
		char buf[ MAX_PATH + 1 ];
		::GetWindowsDirectoryA( buf, sizeof(buf) );
		_PWin32AStringToW( wStr, buf );
	}
	else
#endif
	{
		WCHAR* wBuf = (WCHAR*)wStr.alloc( ( MAX_PATH + 1 ) * sizeof(WCHAR) );
		::GetWindowsDirectoryW( wBuf, MAX_PATH + 1 );
	}

	if( rPath.length() )
		rPath.cut( 0 );
	if( wStr.ptr() )
		_PWin32WStringToPStringEncoding( rPath, (const WCHAR*)wStr.ptr(), encString );
	return static_cast< BOOL >( rPath.length() );
}

BOOL _PWin32GetSystemDirectory( PString& rPath, const PStringEncoding* encString )
{
	if( encString == NULL )
	{
		char buf[ MAX_PATH + 1 ];
		::GetSystemDirectoryA( buf, sizeof(buf) );
		rPath.assign( buf );
		return static_cast< BOOL >( rPath.length() );
	}

	PBlock wStr;
#ifndef PWIDE_ONLY
	if( !_PWinIsUnicode() )
	{
		char buf[ MAX_PATH + 1 ];
		::GetSystemDirectoryA( buf, sizeof(buf) );
		_PWin32AStringToW( wStr, buf );
	}
	else
#endif
	{
		WCHAR* wBuf = (WCHAR*)wStr.alloc( ( MAX_PATH + 1 ) * sizeof(WCHAR) );
		::GetSystemDirectoryW( wBuf, MAX_PATH + 1 );
	}

	if( rPath.length() )
		rPath.cut( 0 );
	if( wStr.ptr() )
		_PWin32WStringToPStringEncoding( rPath, (const WCHAR*)wStr.ptr(), encString );
	return static_cast< BOOL >( rPath.length() );
}

BOOL _PWin32GetCurrentDirectory( PString& rPath, const PStringEncoding* encString )
{
	if( encString == NULL )
	{
		char buf[ MAX_PATH + 1 ];
		DWORD n = ::GetCurrentDirectoryA( sizeof(buf), buf );
		if( 0 < n && n < sizeof(buf) )
			rPath.assign( buf );
		return static_cast< BOOL >( rPath.length() );
	}

	if( rPath.length() )
		rPath.cut( 0 );
#ifndef PWIDE_ONLY
	if( ! _PWinIsUnicode() )
	{
		char buf[ MAX_PATH + 1 ];
		DWORD n = ::GetCurrentDirectoryA( sizeof(buf), buf );
		if( 0 < n && n < sizeof(buf) )
		{
			PBlock wBuf;
			_PWin32AStringToW( wBuf, buf );
			_PWin32WStringToPStringEncoding( rPath, (const WCHAR*)wBuf.ptr(), encString );
		}
	}
	else
#endif
	{
		WCHAR wBuf[ MAX_PATH + 1 ];
		DWORD n = ::GetCurrentDirectoryW( sizeof(wBuf)/sizeof(*wBuf), wBuf );
		if( 0 < n && n < sizeof(wBuf)/sizeof(*wBuf) )
			_PWin32WStringToPStringEncoding( rPath, wBuf, encString );
	}

	return static_cast< BOOL >( rPath.length() );
}

BOOL _PWin32SetCurrentDirectory( const char* lpPath, const PStringEncoding* encString )
{
	if( encString == NULL )
		return ::SetCurrentDirectoryA( lpPath );

	PBlock wPath;
	if( lpPath )
		_PWin32AppendWStringFromPStringEncoding( wPath, lpPath, encString );
	_PWin32NullTerminateWString( wPath );
#ifndef PWIDE_ONLY
	if( ! _PWinIsUnicode() )
	{
		PBlock aPath;
		_PWin32WStringToA( aPath, (const WCHAR*)wPath.ptr() );
		return ::SetCurrentDirectoryA( (const char*)aPath.ptr() );
	}
	else
#endif
	{
		return ::SetCurrentDirectoryW( (const WCHAR*)wPath.ptr() );
	}
}

int _PWin32AddFontResourceEx( const char* lpPath, const PStringEncoding* encString, DWORD flags )
{
	typedef int ( WINAPI *ADDFONTRESOURCEEXA )( LPCSTR, DWORD, PVOID );
	static PAutoProcPtr< ADDFONTRESOURCEEXA > pAddFontResourceExA( "AddFontResourceExA", "gdi32" );
	if( !pAddFontResourceExA )
		return 0;

	if( encString == NULL )
		return pAddFontResourceExA( lpPath, flags, NULL );

	PBlock wPath;
	if( lpPath )
		_PWin32AppendWStringFromPStringEncoding( wPath, lpPath, encString );
	_PWin32NullTerminateWString( wPath );
#ifndef PWIDE_ONLY
	if( ! _PWinIsUnicode() )
	{
		PBlock aPath;
		_PWin32WStringToA( aPath, (const WCHAR*)wPath.ptr() );
		return pAddFontResourceExA( (const char*)aPath.ptr(), flags, NULL );
	}
	else
#endif
	{
		typedef int ( WINAPI *ADDFONTRESOURCEEXW )( LPCWSTR, DWORD, PVOID );
		static PAutoProcPtr< ADDFONTRESOURCEEXW > pAddFontResourceExW( "AddFontResourceExW", "gdi32" );
		if( pAddFontResourceExW )
			return pAddFontResourceExW( (const WCHAR*)wPath.ptr(), flags, NULL );
		return 0;
	}
}

BOOL _PWin32CreateProcess( const char* lpApplicationName, const char* lpCommandLine, const char* lpCurrentDirectory,
	const PStringEncoding* encString, LPPROCESS_INFORMATION lpProcessInformation )
{
	const DWORD CREATIONFLAGS = DETACHED_PROCESS;
	if( encString == NULL )
	{
		STARTUPINFOA startupInfo;
		memset( &startupInfo, 0, sizeof(startupInfo) );
		startupInfo.cb = sizeof(startupInfo);
		return ::CreateProcessA( lpApplicationName, (char*)lpCommandLine, NULL, NULL,
			FALSE, CREATIONFLAGS, NULL, lpCurrentDirectory, &startupInfo, lpProcessInformation );
	}

	PBlock wApplicationName, wCommandLine, wCurrentDirectory;
	if( lpApplicationName )
	{
		_PWin32AppendWStringFromPStringEncoding( wApplicationName, lpApplicationName, encString );
		_PWin32NullTerminateWString( wApplicationName );
	}
	if( lpCommandLine )
	{
		_PWin32AppendWStringFromPStringEncoding( wCommandLine, lpCommandLine, encString );
		_PWin32NullTerminateWString( wCommandLine );
	}
	if( lpCurrentDirectory )
	{
		_PWin32AppendWStringFromPStringEncoding( wCurrentDirectory, lpCurrentDirectory, encString );
		_PWin32NullTerminateWString( wCurrentDirectory );
	}
#ifndef PWIDE_ONLY
	if( !_PWinIsUnicode() )
	{
		PBlock applicationName, commandLine, currentDirectory;
		if( lpApplicationName )
			_PWin32WStringToA( applicationName, (const WCHAR*)wApplicationName.ptr() );
		if( lpCommandLine )
			_PWin32WStringToA( commandLine, (const WCHAR*)wCommandLine.ptr() );
		if( lpCurrentDirectory )
			_PWin32WStringToA( currentDirectory, (const WCHAR*)wCurrentDirectory.ptr() );
		STARTUPINFOA startupInfo;
		memset( &startupInfo, 0, sizeof(startupInfo) );
		startupInfo.cb = sizeof(startupInfo);
		return ::CreateProcessA( (const char*)applicationName.ptr(), (char*)commandLine.ptr(), NULL, NULL,
			FALSE, CREATIONFLAGS, NULL, (const char*)currentDirectory.ptr(), &startupInfo, lpProcessInformation );
	}
	else
#endif
	{
		STARTUPINFOW startupInfo;
		memset( &startupInfo, 0, sizeof(startupInfo) );
		startupInfo.cb = sizeof(startupInfo);
		return ::CreateProcessW( (const WCHAR*)wApplicationName.ptr(), (WCHAR*)wCommandLine.ptr(), NULL, NULL,
			FALSE, CREATIONFLAGS, NULL, (const WCHAR*)wCurrentDirectory.ptr(), &startupInfo, lpProcessInformation );
	}
}

HINSTANCE _PWin32ShellExecute( HWND hWnd, const char* lpOperation, const char* lpFile, const char* lpParameters,
	const char* lpDirectory, const PStringEncoding* encString, INT nShowCmd )
{
	typedef HINSTANCE (WINAPI *SHELLEXECUTEEXA)(LPSHELLEXECUTEINFOA);
	typedef HINSTANCE (WINAPI *SHELLEXECUTEEXW)(LPSHELLEXECUTEINFOW);

	typedef HINSTANCE (WINAPI *SHELLEXECUTEA)(HWND, LPCSTR, LPCSTR, LPCSTR, LPCSTR, INT);
	typedef HINSTANCE (WINAPI *SHELLEXECUTEW)(HWND, LPCWSTR, LPCWSTR, LPCWSTR, LPCWSTR, INT);

	if( encString == NULL )
	{
		SHELLEXECUTEINFOA execInfo;
		::ZeroMemory( &execInfo, sizeof(execInfo) );
		execInfo.cbSize = sizeof(execInfo);
		execInfo.hwnd = hWnd;
		execInfo.lpVerb = lpOperation;
		execInfo.lpFile = lpFile;
		execInfo.lpParameters = lpParameters;
		execInfo.lpDirectory = lpDirectory;
		execInfo.nShow = nShowCmd;
		if( PAutoProcPtr< SHELLEXECUTEEXA > _ShellExecuteExA = PAutoProcPtr< SHELLEXECUTEEXA >( "ShellExecuteExA", "shell32" ) )
		{
			if( _ShellExecuteExA( &execInfo ) )
				return execInfo.hInstApp;
			return ( HINSTANCE )::GetLastError();
		}
		if( PAutoProcPtr< SHELLEXECUTEA > _ShellExecuteA = PAutoProcPtr< SHELLEXECUTEA >( "ShellExecuteA", "shell32" ) )
			return _ShellExecuteA( hWnd, lpOperation, lpFile, lpParameters, lpDirectory, nShowCmd );
		return NULL;
	}

	PBlock wOperation, wFile, wParameters, wDirectory;
	if( lpOperation )
	{
		_PWin32AppendWStringFromPStringEncoding( wOperation, lpOperation, encString );
		_PWin32NullTerminateWString( wOperation );
	}
	if( lpFile )
	{
		_PWin32AppendWStringFromPStringEncoding( wFile, lpFile, encString );
		_PWin32NullTerminateWString( wFile );
	}
	if( lpParameters )
	{
		_PWin32AppendWStringFromPStringEncoding( wParameters, lpParameters, encString );
		_PWin32NullTerminateWString( wParameters );
	}
	if( lpDirectory )
	{
		_PWin32AppendWStringFromPStringEncoding( wDirectory, lpDirectory, encString );
		_PWin32NullTerminateWString( wDirectory );
	}
#ifndef PWIDE_ONLY
	if( !_PWinIsUnicode() )
	{
		PBlock operation, file, parameters, directory;
		if( lpOperation )
			_PWin32WStringToA( operation, (const WCHAR*)wOperation.ptr() );
		if( lpFile )
			_PWin32WStringToA( file, (const WCHAR*)wFile.ptr() );
		if( lpParameters )
			_PWin32WStringToA( parameters, (const WCHAR*)wParameters.ptr() );
		if( lpDirectory )
			_PWin32WStringToA( directory, (const WCHAR*)wDirectory.ptr() );
		SHELLEXECUTEINFOA execInfo;
		::ZeroMemory( &execInfo, sizeof(execInfo) );
		execInfo.cbSize = sizeof(execInfo);
		execInfo.hwnd = hWnd;
		execInfo.lpVerb = (const char*)operation.ptr();
		execInfo.lpFile = (const char*)file.ptr();
		execInfo.lpParameters = (const char*)parameters.ptr();
		execInfo.lpDirectory = (const char*)directory.ptr();
		execInfo.nShow = nShowCmd;
		if( PAutoProcPtr< SHELLEXECUTEEXA > _ShellExecuteExA = PAutoProcPtr< SHELLEXECUTEEXA >( "ShellExecuteExA", "shell32" ) )
		{
			if( _ShellExecuteExA( &execInfo ) )
				return execInfo.hInstApp;
			return ( HINSTANCE )::GetLastError();
		}
		if( PAutoProcPtr< SHELLEXECUTEA > _ShellExecuteA = PAutoProcPtr< SHELLEXECUTEA >( "ShellExecuteA", "shell32" ) )
		{
			return _ShellExecuteA( hWnd, (const char*)operation.ptr(), (const char*)file.ptr(),
				(const char*)parameters.ptr(), (const char*)directory.ptr(), nShowCmd );
		}
		return NULL;
	}
	else
#endif
	{
		SHELLEXECUTEINFOW execInfo;
		::ZeroMemory( &execInfo, sizeof(execInfo) );
		execInfo.cbSize = sizeof(execInfo);
		execInfo.hwnd = hWnd;
		execInfo.lpVerb = (const WCHAR*)wOperation.ptr();
		execInfo.lpFile = (const WCHAR*)wFile.ptr();
		execInfo.lpParameters = (const WCHAR*)wParameters.ptr();
		execInfo.lpDirectory = (const WCHAR*)wDirectory.ptr();
		execInfo.nShow = nShowCmd;
		if( PAutoProcPtr< SHELLEXECUTEEXW > _ShellExecuteExW = PAutoProcPtr< SHELLEXECUTEEXW >( "ShellExecuteExW", "shell32" ) )
		{
			if( _ShellExecuteExW( &execInfo ) )
				return execInfo.hInstApp;
			return ( HINSTANCE )::GetLastError();
		}
		if( PAutoProcPtr< SHELLEXECUTEW > _ShellExecuteW = PAutoProcPtr< SHELLEXECUTEW >( "ShellExecuteW", "shell32" ) )
		{
			return _ShellExecuteW( hWnd, (const WCHAR*)wOperation.ptr(), (const WCHAR*)wFile.ptr(),
				(const WCHAR*)wParameters.ptr(), (const WCHAR*)wDirectory.ptr(), nShowCmd );
		}
		return NULL;
	}
}

BOOL _PWin32PlaySound( LPCSTR lpszSound, HMODULE hModule, DWORD dwSound )
{
	typedef BOOL (WINAPI *PLAYSOUNDA)(LPCSTR, HMODULE, DWORD);
	static PAutoProcPtr< PLAYSOUNDA > _PlaySoundA( "PlaySoundA", "winmm" );
	if( _PlaySoundA )
		return _PlaySoundA( lpszSound, hModule, dwSound );
	return 0;
}

#if !defined( PNOVERSIONAPI )

BOOL _PWin32GetFileVersionInfo( const char* lpFileName, const PStringEncoding* encString, DWORD dwHandle, DWORD dwLen, LPVOID lpData )
{
	if( encString == NULL )
		return ::GetFileVersionInfoA( (char*)lpFileName, dwHandle, dwLen, lpData );

	PBlock wFileName;
	if( lpFileName )
	{
		_PWin32AppendWStringFromPStringEncoding( wFileName, lpFileName, encString );
		_PWin32NullTerminateWString( wFileName );
	}
#ifndef PWIDE_ONLY
	if( !_PWinIsUnicode() )
	{
		PBlock fileName;
		if( lpFileName )
			_PWin32WStringToA( fileName, (const WCHAR*)wFileName.ptr() );
		return ::GetFileVersionInfoA( (char*)fileName.ptr(), dwHandle, dwLen, lpData );
	}
	else
#endif
	{
		return ::GetFileVersionInfoW( (WCHAR*)wFileName.ptr(), dwHandle, dwLen, lpData );
	}
}

DWORD _PWin32GetFileVersionInfoSize( const char* lpFileName, const PStringEncoding* encString, LPDWORD lpdwHandle )
{
	if( encString == NULL )
		return ::GetFileVersionInfoSizeA( (char*)lpFileName, lpdwHandle );

	PBlock wFileName;
	if( lpFileName )
	{
		_PWin32AppendWStringFromPStringEncoding( wFileName, lpFileName, encString );
		_PWin32NullTerminateWString( wFileName );
	}
#ifndef PWIDE_ONLY
	if( !_PWinIsUnicode() )
	{
		PBlock fileName;
		if( lpFileName )
			_PWin32WStringToA( fileName, (const WCHAR*)wFileName.ptr() );
		return ::GetFileVersionInfoSizeA( (char*)fileName.ptr(), lpdwHandle );
	}
	else
#endif
	{
		return ::GetFileVersionInfoSizeW( (WCHAR*)wFileName.ptr(), lpdwHandle );
	}
}

#endif // !defined( PNOVERSIONAPI )

int _PWin32MessageBox( HWND hWnd, const char* lpText, const char* lpCaption, const PStringEncoding* encString, UINT uType )
{
	if( encString == NULL )
#ifdef PWIDE_ONLY
	{
		PBlock wText, wCaption;
		_PWin32AStringToW( wText, lpText );
		_PWin32AStringToW( wCaption, lpCaption );
		return ::MessageBoxW( hWnd, (const WCHAR*)wText.ptr(), (const WCHAR*)wCaption.ptr(), uType );
	}
#else
	{
		return ::MessageBoxA( hWnd, lpText, lpCaption, uType );
	}
#endif

	PBlock wText, wCaption;
	if( lpText )
	{
		_PWin32AppendWStringFromPStringEncoding( wText, lpText, encString );
		_PWin32NullTerminateWString( wText );
	}
	if( lpCaption )
	{
		_PWin32AppendWStringFromPStringEncoding( wCaption, lpCaption, encString );
		_PWin32NullTerminateWString( wCaption );
	}
#ifndef PWIDE_ONLY
	if( !_PWinIsUnicode() )
	{
		PBlock text, caption;
		if( lpText )
			_PWin32WStringToA( text, (const WCHAR*)wText.ptr() );
		if( lpCaption )
			_PWin32WStringToA( caption, (const WCHAR*)wCaption.ptr() );
		return ::MessageBoxA( hWnd, (const char*)text.ptr(), (const char*)caption.ptr(), uType );
	}
	else
#endif
	{
		return ::MessageBoxW( hWnd, (const WCHAR*)wText.ptr(), (const WCHAR*)wCaption.ptr(), uType );
	}
}

int _PWin32DrawText( HDC hDC, const char* lpString, const PStringEncoding* encString, LPRECT lpRect, UINT uFormat )
{
	if( lpString == NULL )
		return 0;

	if( encString == NULL )
#ifdef PWIDE_ONLY
	{
		PBlock wStr;
		_PWin32AStringToW( wStr, lpString );
		return ::DrawTextW( hDC, (const WCHAR*)wStr.ptr(), -1, lpRect, uFormat );
	}
#else
	{
		return ::DrawTextA( hDC, lpString, -1, lpRect, uFormat );
	}
#endif

	PBlock wStr;
	_PWin32AppendWStringFromPStringEncodingC( wStr, lpString, encString );
	_PWin32NullTerminateWString( wStr );
#ifndef PWIDE_ONLY
	if( !_PWinIsUnicode() )
	{
		PBlock buf;
		_PWin32WStringToA( buf, (const WCHAR*)wStr.ptr() );
		return ::DrawTextA( hDC, (const char*)buf.ptr(), -1, lpRect, uFormat );
	}
	else
#endif
	{
		return ::DrawTextW( hDC, (const WCHAR*)wStr.ptr(), -1, lpRect, uFormat );
	}
}

int _PWin32DrawText( HDC hDC, LPCWSTR lpString, LPRECT lpRect, UINT uFormat )
{
	if( lpString == NULL )
		return 0;

#ifndef PWIDE_ONLY
	if( !_PWinIsUnicode() )
	{
		PBlock buf;
		_PWin32WStringToA( buf, lpString );
		return ::DrawTextA( hDC, (const char*)buf.ptr(), -1, lpRect, uFormat );
	}
	else
#endif
	{
		return ::DrawTextW( hDC, lpString, -1, lpRect, uFormat );
	}
}

void _PWin32CopyToClipboard( const char* lpString, const PStringEncoding* encString )
{
	if( encString == NULL )
	{
		if( ::OpenClipboard( NULL ) )
		{
			::EmptyClipboard();
			const size_t len = strlen( lpString ) + 1;
			if( HGLOBAL h = ::GlobalAlloc( GMEM_MOVEABLE, len ) )
			{
				if( LPVOID lp = ::GlobalLock( h ) )
				{
					memcpy( lp, lpString, len );
					::SetClipboardData( CF_TEXT, h );
					::GlobalUnlock( h );
				}
			}
			::CloseClipboard();
		}
		return;
	}

	if( ::OpenClipboard( NULL ) )
	{
		PBlock wStr;
		if( lpString )
		{
			_PWin32AppendWStringFromPStringEncodingC( wStr, lpString, encString );
			_PWin32NullTerminateWString( wStr );
		}
		::EmptyClipboard();
#ifndef PWIDE_ONLY
		if( !_PWinIsUnicode() )
		{
			PBlock buf;
			if( lpString )
				_PWin32WStringToA( buf, (const WCHAR*)wStr.ptr() );
			if( HGLOBAL h = ::GlobalAlloc( GMEM_MOVEABLE, buf.size() ) )
			{
				if( LPVOID lp = ::GlobalLock( h ) )
				{
					memcpy( lp, buf.ptr(), buf.size() );
					::SetClipboardData( CF_TEXT, h );
					::GlobalUnlock( h );
				}
			}
		}
		else
#endif
		{
			if( HGLOBAL h = ::GlobalAlloc( GMEM_MOVEABLE, wStr.size() ) )
			{
				if( LPVOID lp = ::GlobalLock( h ) )
				{
					memcpy( lp, wStr.ptr(), wStr.size() );
					::SetClipboardData( CF_UNICODETEXT, h );
					::GlobalUnlock( h );
				}
			}
		}
		::CloseClipboard();
	}
}

bool _PWin32CheckCtrlAltShift( bool ctrlIsPressed, bool altIsPressed, bool shiftIsPressed )
{
	if( ( ( ::GetKeyState( VK_CONTROL ) & 0x8000 ) == 0 ) == ctrlIsPressed )
		return false;

	if( ( ( ::GetKeyState( VK_MENU ) & 0x8000 ) == 0 ) == altIsPressed )
		return false;

	if( ( ( ::GetKeyState( VK_SHIFT ) & 0x8000 ) == 0 ) == shiftIsPressed )
		return false;

	return true;
}


static bool __PWin32ModifyStyle( HWND hWnd, int nStyle, DWORD dwRemove, DWORD dwAdd, UINT nFlags )
{
	PASSERT4( hWnd != NULL );

	const DWORD dwStyle = _PWin32GetWindowLong( hWnd, nStyle );
	const DWORD dwNewStyle = ( dwStyle & ~dwRemove ) | dwAdd;

	if( dwStyle == dwNewStyle )
		return false;

	_PWin32SetWindowLong( hWnd, nStyle, dwNewStyle );

	if( nFlags != 0 )
	{
		::SetWindowPos( hWnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER
			| SWP_NOACTIVATE | SWP_NOCOPYBITS | SWP_NOREDRAW | nFlags );
	}

	return true;
}

bool _PWin32ModifyStyle( HWND hWnd, DWORD dwRemove, DWORD dwAdd, UINT nFlags )
{
	return __PWin32ModifyStyle( hWnd, GWL_STYLE, dwRemove, dwAdd, nFlags );
}

bool _PWin32ModifyStyleEx( HWND hWnd, DWORD dwRemove, DWORD dwAdd, UINT nFlags )
{
	return __PWin32ModifyStyle( hWnd, GWL_EXSTYLE, dwRemove, dwAdd, nFlags );
}

void _PWin32EditReplaceSel( HWND hWnd, const char* lpReplace, const PStringEncoding* encString, BOOL bCanBeUndone )
{
	if( encString == NULL )
#ifdef PWIDE_ONLY
	{
		PBlock wStr;
		_PWin32AStringToW( wStr, lpReplace );
		::SendMessageW( hWnd, EM_REPLACESEL, (WPARAM)bCanBeUndone, (LPARAM)wStr.ptr() );
		return;
	}
#else
	{
		::SendMessageA( hWnd, EM_REPLACESEL, (WPARAM)bCanBeUndone, (LPARAM)lpReplace );
		return;
	}
#endif

	PBlock wStr;
	if( lpReplace )
		_PWin32AppendWStringFromPStringEncoding( wStr, lpReplace, encString );
	_PWin32NullTerminateWString( wStr );
#ifndef PWIDE_ONLY
	if( !_PWinIsUnicode() )
	{
		PBlock buf;
		if( lpReplace )
			_PWin32WStringToA( buf, (const WCHAR*)wStr.ptr() );
		::SendMessageA( hWnd, EM_REPLACESEL, (WPARAM)bCanBeUndone, (LPARAM)buf.ptr() );
	}
	else
#endif
	{
		::SendMessageW( hWnd, EM_REPLACESEL, (WPARAM)bCanBeUndone, (LPARAM)wStr.ptr() );
	}
}

HHOOK _PWin32SetWindowsHookEx( int idHook, HOOKPROC lpHookProc, HINSTANCE hInstance, DWORD dwThreadId )
{
#ifndef PWIDE_ONLY
	if( !_PWinIsUnicode() )
		return ::SetWindowsHookExA( idHook, lpHookProc, hInstance, dwThreadId );
	else
#endif
		return ::SetWindowsHookExW( idHook, lpHookProc, hInstance, dwThreadId );
}

HICON _PWin32LoadIcon( HINSTANCE hInstance, LPCWSTR lpszIconName, bool bSmallIcon )
{
	return static_cast< HICON >( ::LoadImageW( hInstance, lpszIconName, IMAGE_ICON,
		::GetSystemMetrics( bSmallIcon ? SM_CXSMICON : SM_CXICON ),
		::GetSystemMetrics( bSmallIcon ? SM_CYSMICON : SM_CYICON ),
		LR_DEFAULTCOLOR ) );
}

_POsVersionInfo::_POsVersionInfo()
{
	dwOSVersionInfoSize = sizeof( OSVERSIONINFO );

	if( !::GetVersionEx( this ) )
	{
		::ZeroMemory( this, dwOSVersionInfoSize );
	}
}

_POsVersionInfoEx::_POsVersionInfoEx()
{
	dwOSVersionInfoSize = sizeof( OSVERSIONINFOEX );

	if( !::GetVersionEx( reinterpret_cast< LPOSVERSIONINFO >( this ) ) )
	{
		::ZeroMemory( this, dwOSVersionInfoSize );
	}
}

CompatibleBitmap::CompatibleBitmap()
	: dcBmp( NULL ), hBmpPr( NULL ), hBmp( NULL )
{
}

CompatibleBitmap::~CompatibleBitmap()
{
	if( dcBmp )
	{
		try
		{
			destroyCompatibleBitmap(true);
		}
		catch (...)
		{
		}
	}
}

void CompatibleBitmap::createCompatibleBitmap( HDC dc, HDC dcSource, int cx, int cy )
{
	if( dcBmp )
	{
		destroyCompatibleBitmap();
		PASSERT4( 0 );
	}

	PASSERT( dc );
	PASSERT( dcSource );
	PASSERT( cx && cy );

	hBmp = ::CreateCompatibleBitmap( dcSource, cx, cy );
	PASSERT( hBmp );

	hBmpPr = ( HBITMAP )::SelectObject( dc, hBmp );
	dcBmp = dc;
}

void CompatibleBitmap::destroyCompatibleBitmap( bool noThrow )
{
	if( dcBmp )
	{
		PASSERT4( noThrow || hBmp );

		HBITMAP bmp = ( HBITMAP )::SelectObject( dcBmp, hBmpPr );
		PASSERT4( noThrow || ( bmp == hBmp ) );
		::DeleteObject( bmp );

		dcBmp = NULL;
		hBmpPr = NULL;
		hBmp = NULL;
	}
	else
	{
		PASSERT4( noThrow );
	}
}

void CompatibleBitmap::bitCopyTo( BYTE* buf, BITMAPINFO& bmi ) const
{
	HBITMAP bmp = ( HBITMAP )::SelectObject( dcBmp, hBmpPr );
	PASSERT4( bmp == hBmp );

	const int r = ::GetDIBits( dcBmp, hBmp, 0, bmi.bmiHeader.biHeight, buf, &bmi, DIB_RGB_COLORS );

	bmp = ( HBITMAP )::SelectObject( dcBmp, hBmp );
	PASSERT4( bmp == hBmpPr );

	PASSERT( r == bmi.bmiHeader.biHeight );
}

void CompatibleBitmap::bitCopyFrom( const BYTE* buf, BITMAPINFO& bmi )
{
	HBITMAP bmp = ( HBITMAP )::SelectObject( dcBmp, hBmpPr );
	PASSERT4( bmp == hBmp );

	const int r = ::SetDIBits( dcBmp, hBmp, 0, bmi.bmiHeader.biHeight, buf, &bmi, DIB_RGB_COLORS );

	bmp = ( HBITMAP )::SelectObject( dcBmp, hBmp );
	PASSERT4( bmp == hBmpPr );

	PASSERT( r == bmi.bmiHeader.biHeight );
}

#endif
