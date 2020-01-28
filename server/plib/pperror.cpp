#include "ppinclude.h"
#include "pperror.h"
#include "ppstring.h"
#include "ppthread.h"

//****************************************************************************

static PError::Callback* _constructorCallback = 0;
int PSystemExceptionError::numberOfExceptions = 0;
int PSystemMemoryError::numberOfExceptions = 0;
int PAssert::numberOfAsserts = 0;
void PErrorInitCallback( /* new */ PError::Callback* callback )//thread-unsafe!
{
	if( _constructorCallback )
		delete _constructorCallback;
	_constructorCallback = callback;
}

//****************************************************************************

PError::PError()
{
	msg = new PString();
}

PError::PError( PString&& msg_, bool logError )
{
	msg = new PString( move( msg_ ) );
	if( logError && _constructorCallback )
		_constructorCallback->error( why() );
}

PError::PError( const char* msg_, bool logError )
{
	msg = new PString( msg_ );
	if( logError && _constructorCallback )
		_constructorCallback->error( why() );
}

PError::PError( const PError& other )
{
	msg = new PString( other.why() );
}

PError& PError::operator =( const PError& other )
{
	if( this == &other )
		return *this;
	delete msg;
	msg = new PString( other.why() );
	return *this;
}

const char* PError::why() const
{
	return *msg;
}

inline PString& PError::_msg()
{
	return *msg;
}

PError::~PError()
{
	delete msg;
}

PSystemError::PSystemError( const char* msg, const char* errMsg )
{
	_msg().append( msg ).append( ": " ).append( errMsg );
	if( _constructorCallback )
		_constructorCallback->error( why() );
}

PSystemError::PSystemError( const char* msg, int errCode )
{
	_msg().append( msg ).append( ": " );
	_appendErrMsg( _msg(), errCode );
	if( _constructorCallback )
		_constructorCallback->error( why() );
}

/* static */ void PSystemError::errorMessage( PString& ret, int errCode )
{
	ret = "";
	_appendErrMsg( ret, errCode );
}

PSystemMemoryError::PSystemMemoryError( const char* msg, size_t numBytes )
{
	numberOfExceptions++;
	_msg().append( msg ).append( ": 0x" ).appendHexInt64( numBytes );
	if( _constructorCallback )
		_constructorCallback->error( why() );
}

PSystemExceptionError::PSystemExceptionError( const char* msg )
{
	numberOfExceptions++;
	_msg().append( msg );
	if( _constructorCallback )
		_constructorCallback->error( why() );
}

#ifdef PWIN32

/* static */ void PSystemError::_appendErrMsg( PString& ret, int errCode )
{
	char buf[ 256 ];
	if( ::FormatMessageA( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
		errCode, MAKELANGID( LANG_ENGLISH, SUBLANG_DEFAULT ), buf, sizeof( buf ), NULL ) )
	{
		for( char* s = buf ; *s ; ++s )
			if( *s == '\n' || *s == '\r' )
				*s = ' ';

		ret.append( buf ).append( " (" ).appendInt( errCode ).append( "=0x" ).appendHexInt( errCode ).append( ')' );
		return;
	}

	ret.append( "Unknown error (" ).appendInt( errCode ).append( "=0x" ).appendHexInt( errCode ).append( ')' );
}

#elif defined( PPOSIX )
/* static */ void PSystemError::_appendErrMsg( PString& ret, int errCode )
{
	ret.append( strerror( errCode ) ).append( " (" ).appendInt( errCode ).append( "=0x" ).appendHexInt( errCode ).append( ')' );
}
#else
/* static */ void PSystemError::_appendErrMsg( PString& ret, int errCode )
{
	ret.appendInt( errCode ).append( "=0x" ).appendHexInt( errCode );
}
#endif

PAssert::PAssert( const char* cond, const char* file, int line )
{
	numberOfAsserts++;
	_msg().append( "Assertion failed: " ).append( cond ).append( ", file " ).append( file ).append( ", line " ).appendInt( line );
	if( _constructorCallback )
		_constructorCallback->error( why() );
}

//****************************************************************************
#ifdef PWIN32
#include "ppstacktrace.h"

void PError::_translate( unsigned int code, _EXCEPTION_POINTERS* xptr )
{
	char buf[ 8192 ];
	char* p = buf;
	int n = sizeof(buf);
	int nw = snprintf( p, n, "Win32 SEH Exception %08X ", code );
	p += nw;
	n -= nw;

	EXCEPTION_RECORD* xrec = (xptr ? xptr->ExceptionRecord : 0);
	if( xrec && n > 0 )
	{
		nw = snprintf(p, n, "@%p", xrec->ExceptionAddress);
		p += nw;
		n -= nw;

		for( DWORD i=0; i < xrec->NumberParameters && n > 0; ++i )
		{
			if( i > 10 )
				break;
			nw = snprintf(p, n, ", %p", (void*)xrec->ExceptionInformation[i]);
			p += nw;
			n -= nw;
		}
	}

#if !defined(COMM_CLIENT_ONLY)
	if (n > 0)
	{
		nw = snprintf(p, n, "\r\n");
		p += nw;
		n -= nw;
		if (n > 0)
		{
			StackTrace::getStackTrace(p, n, (xptr ? xptr->ContextRecord : nullptr));
		}
	}
#endif

	if( _constructorCallback )
		_constructorCallback->error( buf );
	throw PSystemExceptionError( buf );
}

#if !defined(COMM_CLIENT_ONLY)
void logStackTrace(const char* explanation, const char* logFileSuffix)
{
	static PCriticalSection cs;
	PLock lock(cs);
	char buf[8192];
	memset(buf, 0, sizeof(buf));
	StackTrace::getStackTrace(buf, sizeof(buf));
	
	if (_constructorCallback)
	{
		_constructorCallback->error(explanation);
		_constructorCallback->error(buf);
	}
	else
	{
		char name[MAX_PATH];
		char* nameEnd = name + GetModuleFileNameA(GetModuleHandleA(0), name, sizeof(name));
		SYSTEMTIME t;
		GetSystemTime(&t);
		sprintf(nameEnd - strlen(".exe"), "_%4d%02d%02d_%02d%02d%02d_%s.log", t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond, logFileSuffix);
		HANDLE h = CreateFileA(name, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, NULL);
		LONG n = 0;
		SetFilePointer(h, 0, &n, FILE_END);
		DWORD res;
		WriteFile(h, explanation, strlen(explanation), &res, NULL);
		WriteFile(h, buf, sizeof(buf), &res, NULL);
		CloseHandle(h);
	}
}

void PError::_terminate()
{
	logStackTrace("Terminate called for process", "term");
	exit(17);
}

void PError::sigHandler(int sig)
{
	char explanation[256];
	memset(explanation, 0, sizeof(explanation));
	sprintf(explanation, "Received signal %d%s\r\n", sig, (sig != SIGTERM ? " process terminated" : ""));
	logStackTrace(explanation, "sig");
	if (sig != SIGTERM)
	{
		_exit(sig);
	}
}
#endif

#endif

void PErrorHandler::execute()
{
#if defined( PWIN32 )
	void(*oldTranslator)(unsigned int, _EXCEPTION_POINTERS*);
	oldTranslator = _set_se_translator( PError::_translate );
#if !defined(COMM_CLIENT_ONLY)
	set_terminate(PError::_terminate);
#endif
	try
	{
		run();
	}
	catch( ... )
	{
		_set_se_translator(oldTranslator);
		return;
	}
	_set_se_translator( oldTranslator );
#else
	try
	{
		run();
	}
	catch( ... )
	{
		return;
	}
#endif
}

#ifdef PPOSIX
PErrorHandler::PErrorHandler()
{
	sigset_t set_s;
	sigemptyset(&set_s);
	sigaddset(&set_s, PLIB_CONTROL_SIGNAL);
	sigprocmask(SIG_UNBLOCK,&set_s,0);
}
#endif

