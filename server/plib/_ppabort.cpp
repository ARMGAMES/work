//------------------------------------------------------------------------------

#include "ppincludebase.h"
#include "pperror.h"

//------------------------------------------------------------------------------

// This file should NOT included into plib.lib
// To activate it, it should be linked into executable explicitly 

//------------------------------------------------------------------------------

// It's only purpose is to replace default runtime abort(), 
// so third-party calls to abort() [and may be assert(...)] 
// will result in exception - rather than in program abort

extern "C" void abort()
{
	throw PInternalError( "abort() called" );
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#ifdef PWIN32

#include "pplogfile.h"

// disable 'Pure Virtual Call' error box

extern "C" int __cdecl _purecall()
{
	PTRACE3( "*** PureCall ***" );
	PASSERT5( 0 );
	return 0;
}

//------------------------------------------------------------------------------
//
//	Fix for Windows OS compatibility issues for Visual C++ 2005
//		Standard library implementation of _invoke_watson & __report_gsfailure 
//		functions calls IsDebuggerPresent which requires Windows Vista, Windows XP, 
//		or Windows 2000 Professional.
//
//------------------------------------------------------------------------------

#if _MSC_VER >= 1400

extern "C" void __cdecl _invoke_watson( const wchar_t *pszExpression, 
	const wchar_t *pszFunction, const wchar_t *pszFile, unsigned int nLine, 
	uintptr_t pReserved )
{
	PTRACE3( "*** InvokeWatson ***" );
	PASSERT5( 0 );
}

#ifdef  _M_IX86

extern "C" void __cdecl __report_gsfailure( void )
{
	PTRACE3( "*** GsFailure ***" );
	PASSERT5( 0 );
}

extern "C" void __cdecl __report_rangecheckfailure( void )
{
	PTRACE3( "*** RangeCheckFailure ***" );
	PASSERT5( 0 );
}

#else

extern "C" void __cdecl __report_gsfailure( uintptr_t )
{
	PTRACE3( "*** GsFailure ***" );
	PASSERT5( 0 );
}

extern "C" void __cdecl __report_rangecheckfailure( void )
{
	PTRACE3( "*** RangeCheckFailure ***" );
	PASSERT5( 0 );
}

#endif

#endif

//------------------------------------------------------------------------------

#endif

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
