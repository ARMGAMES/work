#ifndef ppwinwnd_h_included
#define ppwinwnd_h_included

#include "pperror.h"

#ifdef PWIN32

#include "eh.h"

ATOM _PWin32RegisterClassEx( const WCHAR* lpClassName, UINT uClassStyle, WNDPROC lpWindowProc,
	HINSTANCE hInstance, HCURSOR hCursor = 0, HBRUSH hbrBackground = 0, HICON hIcon = 0 );

ATOM _PWin32RegisterSuperClassEx( const WCHAR* lpClassName, WNDPROC lpWindowProc,
	const WCHAR* lpBaseClassName, WNDPROC& lpBaseWindowProc, HINSTANCE hInstance );

HWND _PWin32CreateWindowEx( DWORD dwExStyle, const WCHAR* lpClassName, const WCHAR* lpWindowName, DWORD dwStyle,
	int x, int y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam );

LRESULT _PWin32DefWindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

BOOL _PWin32GetMessage( MSG* lpMsg, HWND hWnd, UINT uMsgFilterMin, UINT uMsgFilterMax );
LRESULT _PWin32DispatchMessage( const MSG* lpMsg );

BOOL _PWin32PeekMessage( MSG* lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg );

LRESULT _PWin32SendMessage( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
BOOL _PWin32PostMessage( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
BOOL _PWin32PostThreadMessage( DWORD idThread, UINT uMsg, WPARAM wParam, LPARAM lParam );

LONG _PWin32GetWindowLong( HWND hWnd, int nIndex );
LONG _PWin32SetWindowLong( HWND hWnd, int nIndex, LONG dwNewLong );

int _PWin32GetWindowText( HWND hWnd, PString& rString, const PStringEncoding* encString, bool convertCrLf = false );
BOOL _PWin32SetWindowText( HWND hWnd, const char* lpString, const PStringEncoding* encString, bool convertLf = false );
int _PWin32GetWindowTextLength( HWND hWnd );

HWND _PWin32CreateDialogIndirectParam( HINSTANCE hInstance, LPCDLGTEMPLATE lpDialogTemplate,
	HWND hWndParent, DLGPROC lpDialogProc, LPARAM lParamInit );

LRESULT _PWin32CallWindowProc( WNDPROC lpWindowProc, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

void _PWin32AddToolTip( HWND hWndToolTip, HWND hWnd, const char* lpString, const PStringEncoding* encString, bool convertLf = false );
void _PWin32SetToolTip( HWND hWndToolTip, HWND hWnd, const char* lpString, const PStringEncoding* encString, bool convertLf = false );

int _PWin32ComboBoxAddString( HWND hWnd, const char* lpString, const PStringEncoding* encString );
int _PWin32ListBoxAddString( HWND hWnd, const char* lpString, const PStringEncoding* encString );

BOOL _PWin32AppendMenu( HMENU hMenu, UINT uFlags, UINT_PTR uId, const char* lpString, const PStringEncoding* encString );

BOOL _PWin32GetModuleFileName( HMODULE hModule, PString& rPath, const PStringEncoding* encString );
BOOL _PWin32GetWindowsDirectory( PString& rPath, const PStringEncoding* encString );
BOOL _PWin32GetSystemDirectory( PString& rPath, const PStringEncoding* encString );

BOOL _PWin32GetCurrentDirectory( PString& rPath, const PStringEncoding* encString );
BOOL _PWin32SetCurrentDirectory( const char* lpPath, const PStringEncoding* encString );

int _PWin32AddFontResourceEx( const char* lpPath, const PStringEncoding* encString, DWORD flags );

BOOL _PWin32CreateProcess( const char* lpApplicationName, const char* lpCommandLine, const char* lpCurrentDirectory,
	const PStringEncoding* encString, LPPROCESS_INFORMATION lpProcessInformation );

HINSTANCE _PWin32ShellExecute( HWND hWnd, const char* lpOperation, const char* lpFile, const char* lpParameters,
	const char* lpDirectory, const PStringEncoding* encString, INT nShowCmd );

BOOL _PWin32PlaySound( LPCSTR lpszSound, HMODULE hModule, DWORD dwSound );

#if !defined( PNOVERSIONAPI )

BOOL _PWin32GetFileVersionInfo( const char* lpFileName, const PStringEncoding* encString, DWORD dwHandle, DWORD dwLen, LPVOID lpData );
DWORD _PWin32GetFileVersionInfoSize( const char* lpFileName, const PStringEncoding* encString, LPDWORD lpdwHandle );

#endif

int _PWin32MessageBox( HWND hWnd, const char* lpText, const char* lpCaption, const PStringEncoding* encString, UINT uType );

int _PWin32DrawText( HDC hDC, const char* lpString, const PStringEncoding* encString, LPRECT lpRect, UINT uFormat );
int _PWin32DrawText( HDC hDC, LPCWSTR lpString, LPRECT lpRect, UINT uFormat );

void _PWin32CopyToClipboard( const char* lpString, const PStringEncoding* encString );

bool _PWin32CheckCtrlAltShift( bool ctrlIsPressed, bool altIsPressed, bool shiftIsPressed );

bool _PWin32ModifyStyle( HWND hWnd, DWORD dwRemove, DWORD dwAdd, UINT nFlags = 0 );
bool _PWin32ModifyStyleEx( HWND hWnd, DWORD dwRemove, DWORD dwAdd, UINT nFlags = 0 );

void _PWin32EditReplaceSel( HWND hWnd, const char* lpReplace, const PStringEncoding* encString, BOOL bCanBeUndone = TRUE );

HHOOK _PWin32SetWindowsHookEx( int idHook, HOOKPROC lpHookProc, HINSTANCE hInstance, DWORD dwThreadId );

HICON _PWin32LoadIcon( HINSTANCE hInstance, LPCWSTR lpszIconName, bool bSmallIcon = false );

inline void ClientToScreen( HWND hWnd, LPRECT r )
{
	::ClientToScreen( hWnd, (LPPOINT)r );
	::ClientToScreen( hWnd, (LPPOINT)r + 1 );
}

inline void ScreenToClient( HWND hWnd, LPRECT r )
{
	::ScreenToClient( hWnd, (LPPOINT)r );
	::ScreenToClient( hWnd, (LPPOINT)r + 1 );
}

class PWin32ExceptionTranslator
{
	const _se_translator_function initial_se_translator;

public:
	PWin32ExceptionTranslator( _se_translator_function se_translator = PError::_translate )
		: initial_se_translator( ::_set_se_translator( se_translator ) )
	{
	}

	~PWin32ExceptionTranslator()
	{
		::_set_se_translator( initial_se_translator );
	}

private: // prohibit default copy constructor & assignment operator
	PWin32ExceptionTranslator( const PWin32ExceptionTranslator& other );
	PWin32ExceptionTranslator& operator=( const PWin32ExceptionTranslator& other );
};

class PLibrary
{
protected:
	HMODULE hModule;

public:
	PLibrary( const char* moduleName )
		: hModule( ::LoadLibraryA( moduleName ) )
	{
	}

	PLibrary( PLibrary&& other )
	{
		hModule = other.hModule;
		other.hModule = NULL;
	}

	~PLibrary()
	{
		if( hModule )
		{
			::FreeLibrary( hModule );
		}
	}

public:
	PLibrary& operator=( PLibrary&& other )
	{
		hModule = other.hModule;
		other.hModule = NULL;
		return *this;
	}

public:
	operator HMODULE() { return hModule; }

private: // prohibit default copy constructor & assignment operator
	PLibrary( const PLibrary& other );
	PLibrary& operator=( const PLibrary& other );
};

template< typename T >
class PProcPtr
{
	T ptr;

public:
	PProcPtr( const char* procName, const char* moduleName = "kernel32" )
		: ptr( nullptr )
	{
		if( HMODULE hModule = ::GetModuleHandle( moduleName ) )
		{
			ptr = reinterpret_cast< T >( ::GetProcAddress( hModule, procName ) );
		}
	}

public:
	operator bool() const { return ptr; }

public:
	operator T() const { return ptr; }
};

template< typename T >
class PAutoProcPtr : protected PLibrary
{
	T ptr;

public:
	PAutoProcPtr( const char* procName, const char* moduleName ) 
		: PLibrary( moduleName ), ptr( nullptr )
	{
		if( hModule )
		{
			ptr = reinterpret_cast< T >( ::GetProcAddress( hModule, procName ) );
		}
	}

	PAutoProcPtr( PAutoProcPtr&& other )
		: PLibrary( move( other ) )
	{
		ptr = other.ptr;
		other.ptr = nullptr;
	}

	~PAutoProcPtr()
	{
	}

public:
	PAutoProcPtr& operator=( PAutoProcPtr&& other )
	{
		PLibrary::operator=( move( other ) );
		ptr = other.ptr;
		other.ptr = nullptr;
		return *this;
	}

public:
	operator bool() const { return ptr; }

public:
	operator T() const { return ptr; }

private: // prohibit default copy constructor & assignment operator
	PAutoProcPtr( const PAutoProcPtr& other );
	PAutoProcPtr& operator=( const PAutoProcPtr& other );
};

struct PWin32WindowEnumerator
{
	const HWND hWnd;

	PWin32WindowEnumerator( HWND hWnd_ )
		: hWnd( hWnd_ )
	{
	}

	struct iterator
	{
		HWND hWnd;

		iterator( HWND hWnd_ )
			: hWnd( hWnd_ )
		{
		}

		bool operator==( const iterator& other ) const
		{
			return hWnd == other.hWnd;
		}

		bool operator!=( const iterator& other ) const
		{
			return hWnd != other.hWnd;
		}

		iterator& operator++()
		{
			hWnd = ::GetWindow( hWnd, GW_HWNDNEXT );
			return *this;
		}

		HWND operator*()
		{
			return hWnd;
		}
	};

	iterator begin() const
	{
		return iterator( hWnd );
	}

	iterator end() const
	{
		return iterator( NULL );
	}
};

struct PWin32ChildWindowEnumerator : PWin32WindowEnumerator
{
	PWin32ChildWindowEnumerator( HWND hWnd_ )
		: PWin32WindowEnumerator( ::GetWindow( hWnd_, GW_CHILD ) )
	{
	}
};

struct PWin32TopWindowEnumerator : PWin32WindowEnumerator
{
	PWin32TopWindowEnumerator( HWND hWnd_ = NULL )
		: PWin32WindowEnumerator( ::GetTopWindow( hWnd_ ) )
	{
	}
};

struct _POsVersionInfo : public OSVERSIONINFO
{
	_POsVersionInfo();

	DWORD osVersion() const
	{
		PASSERT4( HIWORD( dwMinorVersion ) == 0 );
		PASSERT4( HIWORD( dwMajorVersion ) == 0 );
		return MAKELONG( LOWORD( dwMinorVersion ), LOWORD( dwMajorVersion ) );
	}
};
struct _POsVersionInfoEx : public OSVERSIONINFOEX
{
	_POsVersionInfoEx();

	DWORD osVersion() const
	{
		PASSERT4( HIWORD( dwMinorVersion ) == 0 );
		PASSERT4( HIWORD( dwMajorVersion ) == 0 );
		return MAKELONG( LOWORD( dwMinorVersion ), LOWORD( dwMajorVersion ) );
	}
};

class CompatibleBitmap
{
	HDC dcBmp;
	HBITMAP hBmpPr;
	HBITMAP hBmp;

public:
	CompatibleBitmap();
	~CompatibleBitmap();

public:
	void createCompatibleBitmap( HDC dc, HDC dcSource, int cx, int cy );

	void createCompatibleBitmap( HDC dc, HDC dcSource, const RECT& r )
	{
		createCompatibleBitmap( dc, dcSource, r.right - r.left, r.bottom - r.top );
	}

	void createCompatibleBitmap( HDC dc, HDC dcSource, const SIZE& size )
	{
		createCompatibleBitmap( dc, dcSource, size.cx, size.cy );
	}

public:
	void destroyCompatibleBitmap( bool noThrow = false );

public:
	void bitCopyTo( BYTE* buf, BITMAPINFO& bmi ) const;
	void bitCopyFrom( const BYTE* buf, BITMAPINFO& bmi );

public:
	operator bool() const
	{
		return dcBmp && hBmp;
	}
};

#endif
#endif
