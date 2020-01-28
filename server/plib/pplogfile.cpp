#include "ppinclude.h"
#include "pplogfile.h"
#include "ppi18n.h"//PStringEncoding
#include "ppsystem.h"
//#include "ppdir.h"

#if defined(_WIN32) && !defined(COMM_CLIENT_ONLY)
#include "ppstacktrace.h"
#endif

#ifndef NOLOCK_LOG
#ifndef PNOTHREADS
static PCriticalSection* _cs = 0;
#endif
#endif

#if defined(P_ANDROID)
#include <android/log.h>
#endif

#ifdef PWIN32
typedef enum PP_HEAP_INFORMATION_CLASS // Win32 _HEAP_INFORMATION_CLASS not defined in some versions
{
	PPHeapCompatibilityInformation

} PPHEAP_INFORMATION_CLASS;

typedef BOOL (WINAPI *HSI)(
						   HANDLE HeapHandle,
						   PPHEAP_INFORMATION_CLASS HeapInformationClass,
						   PVOID HeapInformation,
						   SIZE_T HeapInformationLength
						   );

typedef BOOL (WINAPI *HQI)(
						   HANDLE HeapHandle,
						   PPHEAP_INFORMATION_CLASS HeapInformationClass,
						   PVOID HeapInformation,
						   SIZE_T HeapInformationLength,
						   PSIZE_T ReturnLength
						   );

#if 1
static void* getBaseAddress()
{
	return GetModuleHandle(0);
}
#else
#include <TlHelp32.h>
static void* getBaseAddress()
{
	HANDLE h = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,0);
	MODULEENTRY32 s; 
	Module32First(h,&s);
	CloseHandle(h);
	return s.modBaseAddr;
}
#endif
#endif

static void _logNameByDate( PString& ret, const char* baseFileName, int version, const struct tm* tm )
{
	ret = baseFileName;
	char buf[ 64 ];
	sprintf( buf, "%04d%02d%02d", tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday );
	if (version)
		ret.append( '.' ).appendInt(version).append('.').append( buf );
	else
		ret.append( '.' ).append( buf );
}

static void _logNameByDateLimited( PString& ret, const char* baseFileName )
{
	ret = baseFileName;
	ret.append(".0");
}

static void _bakFile1( const char* fileName, const PStringEncoding* fileNameEncoding_ )
{
	PString currFileName( fileName );
	PString bakFileName( fileName );
	currFileName.append( ".0" );
	bakFileName.append( ".1" );
//	seems that the following should be replaced with corresponding PDirectory calls; requires testing
	::remove( bakFileName );
	::rename( currFileName, bakFileName );
//	PDirectory::removeFile( bakFileName, fileNameEncoding_ );
//	PDirectory::renameFile( currFileName, bakFileName, fileNameEncoding_ );
}

#if defined( NOLOCK_LOG ) && defined( PWIN32 )

static LOGFILE_T _openLogFile( const char* fileName, const char* mode, const PStringEncoding* fileNameEncoding_ )
{
	PASSERT5( mode );
	DWORD cMode = 0;
	if( *mode == 'w' )
		cMode = CREATE_ALWAYS;
	else if( *mode == 'a' )
		cMode = OPEN_ALWAYS;
	PASSERT5( cMode );

	PBlock wfName;//of WCHAR
	_PWin32AppendWStringFromPStringEncoding( wfName, fileName, fileNameEncoding_ );
	_PWin32NullTerminateWString( wfName );
	HANDLE h;
#if !defined( PWIDE_ONLY )
	if( ! _PWinIsUnicode() )
	{
		PBlock afName;
		_PWin32WStringToA( afName, (const WCHAR*)wfName.ptr() );
		h = ::CreateFileA( (const char*)afName.ptr(), GENERIC_WRITE|GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, cMode, 0, NULL );
	}
	else
#endif
	{
		h = ::CreateFileW( (const WCHAR*)wfName.ptr(), GENERIC_WRITE|GENERIC_READ, FILE_SHARE_READ|FILE_SHARE_WRITE, NULL, cMode, 0, NULL );
	}
	LONG n=0;
	::SetFilePointer( h, 0, &n, FILE_END );
	return h;
}

static inline LOGFILE_T _invalidLogFile()
{
	return INVALID_HANDLE_VALUE;
}

static inline void _writeLogFile( LOGFILE_T f, const void* buff, size_t n )
{
	DWORD res;
	::WriteFile( f, buff, n, &res, NULL );
}

static inline void _closeLogFile( LOGFILE_T f )
{
	::CloseHandle( f );
}

static inline void _flushLogFile( LOGFILE_T f )
{
}

#else

static LOGFILE_T _openLogFile( const char* fileName, const char* mode, const PStringEncoding* fileNameEncoding_ )
{
#ifdef NOLOCK_LOG
#error NOT tested configuration: NOLOCK_LOG && ! WIN32
	PString md( mode );
	md.append( 'b' );
#else
	const char* const md = mode;
#endif
	return plib_fopen( fileName, md, fileNameEncoding_ );
}

static inline LOGFILE_T _invalidLogFile()
{
	return NULL;
}

#ifdef NOLOCK_LOG
static inline void _writeLogFile( LOGFILE_T f, const void* buff, size_t n )
{
	if( f )
		::fwrite( buff, 1, n, f );
}
#endif

static void _closeLogFile( LOGFILE_T f )
{
	if( f )
		::fclose( f );
}

static void _flushLogFile( LOGFILE_T f )
{
	if( f )
		::fflush( f );
}

#endif

static inline bool _isValidLogFile( LOGFILE_T f )
{
	return f != _invalidLogFile();
}

PLogFile::PLogFile( const char* fileName, int flags_ /* = 0 */, const PStringEncoding* fileNameEncoding_ /* = NULL */ )
{
	flags = flags_;
	fileNameEncoding = NULL;
	version = 0;
	if( fileNameEncoding_ )
	{
		PASSERT4( fileNameEncoding_->appendsAsciiAsIs() );
		fileNameEncoding = fileNameEncoding_->clone();
	}

	if( fileName )
	{
		baseFileName = fileName;
		if( flags_ & perDate )
		{
			flags_ &= ~( perDateLimited | perSession );

			//time_t now = time( 0 );
			//struct tm* tt = localtime( &now );
			//PASSERT3( tt );
			struct tm tm;
			PCurrentLocalTime( &tm );
			const struct tm* tt = &tm;

			lastLogTm = *tt;
			PString dateFileName;
			_logNameByDate( dateFileName, baseFileName, version, tt );
			file = _openLogFile( dateFileName, "a", fileNameEncoding );
		}
		else if( flags_ & perDateLimited )
		{
			flags_ &= ~perSession;

			//time_t now = time( 0 );
			//struct tm* tt = localtime( &now );
			//PASSERT3( tt );
			struct tm tm;
			PCurrentLocalTime( &tm );
			const struct tm* tt = &tm;

			lastLogTm = *tt;

			PString currFileName;
			_logNameByDateLimited( currFileName, baseFileName );
			bool backup = true;
			FILE* f = plib_fopen( currFileName, "rt", fileNameEncoding );
			if( f )
			{//NOTE: following code relies on data format below [1]
				char buf[ 32 ];
				char sample[ 32 ];
				if( ::fread( buf, 1, 12, f ) == 12 )
				{
					buf[ 12 ] = 0;

					sprintf( sample, "[%04d/%02d/%02d ", tt->tm_year + 1900, tt->tm_mon + 1, tt->tm_mday );
					if( strcmp( buf, sample ) == 0 )
						backup = false;
				}
				::fclose( f );
			}

			if( backup )
			{
				_bakFile1( baseFileName, fileNameEncoding );
				file = _openLogFile( currFileName, "w", fileNameEncoding );
			}
			else
			{
				file = _openLogFile( currFileName, "a", fileNameEncoding );
			}
		}
		else if( flags_ & perSession )
		{
			_bakFile1( fileName, fileNameEncoding );
			PString currFileName;
			_logNameByDateLimited( currFileName, baseFileName );
			file = _openLogFile( currFileName, "w", fileNameEncoding );
		}
		else
		{
			file = _openLogFile( fileName, "a", fileNameEncoding );
		}
		//--- PASSERT4( isValidLogFile( file ) );
		// proceed anyway
	}
	else
	{
		file = _invalidLogFile();
	}
	//lastLogTime = 0;
	memset( &lastLogTime, 0, sizeof(lastLogTime) );
}

void PLogFile::addCallBack(Callback* _callback)
{
#ifndef NOLOCK_LOG
#ifndef PNOTHREADS
	PASSERT4( _cs );
	PLock lock( *_cs );
#endif
#endif

	_plogCallBacks.push_back(_callback);
}

void PLogFile::_callCallback()
{
	list<Callback *>::iterator it;
	for (it=_plogCallBacks.begin();it != _plogCallBacks.end(); ++it)
	{
		(*it)->opened();
	}
}



void PLogFile::updateFlags( int updateMask, int newFlags )
{
#ifdef NOLOCK_LOG
#ifndef PNOTHREADS
	PLock lock( _cs );
#endif
#endif
	updateMask &= ( alsoToStdout | printProcessId | printThreadId | noFlush );	//sanity
	flags = ( flags & ~updateMask ) | ( newFlags & updateMask );
}

PLogFile::~PLogFile()
{
	_closeLogFile( file );
	delete fileNameEncoding;
	list<Callback *>::iterator it;
	for (it=_plogCallBacks.begin();it != _plogCallBacks.end(); ++it)
	{
		delete (*it);
	}
	_plogCallBacks.clear();
}

void PLogFile::_print( _In_z_ _Printf_format_string_ const char* fmt, ... )
{
	va_list args;
	va_start( args, fmt );
	_vprint( "", fmt, args );
	va_end( args );
}

void PLogFile::_vprint( _In_z_ const char* prefix, _In_z_ _Printf_format_string_ const char* fmt, va_list args )
{
	const bool needStdOut = ( flags & alsoToStdout );

#ifdef NOLOCK_LOG
	const unsigned SZ = 1024;
	char buff[SZ+16];
	int n;
	for( n = 0; n < SZ; n++ )
		if( prefix[n] )
			buff[n] = prefix[n];
		else
			break;
	int n0 = ::_vsnprintf( buff + n, SZ - n, fmt, args );
	if (n0 == -1)
		n0 = SZ - n;
	n += n0;
	buff[n++] = '\r';
	buff[n++] = '\n';
	_writeLogFile( file, buff, n );
	if( needStdOut )
		::fwrite( buff, 1, n, stdout );
#else
	va_list argsStdOut;
#ifdef va_copy
	va_copy(argsStdOut,args);
#else
	argsStdOut = args;
#endif
	if( _isValidLogFile( file ) )
	{
		::fputs( prefix, file );
		::vfprintf( file, fmt, args );
		::fputs( "\n", file );
	}
	if( needStdOut )
	{
		::fputs( prefix, stdout );
		::vfprintf( stdout, fmt, argsStdOut );
		::fputs( "\n", stdout );
	}
	va_end(argsStdOut);
#endif
}

void PLogFile::log( _In_z_ _Printf_format_string_ const char* fmt, ... )
{
	va_list args;
	va_start( args, fmt );
	vlog( 0, fmt, args );
	va_end( args );
}

void PLogFile::checkRotate(bool printTime)
{
	int flags_ = flags; // use local copy
	struct tm tm;

#ifdef NOLOCK_LOG
#ifndef PNOTHREADS
	PLock lock( _cs );
#endif
#endif

	PCurrentLocalTime( &tm );
	if( PCompareLocalTime( &tm, &lastLogTime ) != 0 )
	{
		const struct tm* tt = &tm;
		bool newFile = false;
		if( _isValidLogFile( file ) && ( flags_ & perDate ) != 0 )
		{
			if( tt->tm_year != lastLogTm.tm_year || tt->tm_mon != lastLogTm.tm_mon || tt->tm_mday != lastLogTm.tm_mday )
			{
				_closeLogFile( file );
				version = 0;
				lastLogTm = *tt;
				PString dateFileName;
				_logNameByDate( dateFileName, baseFileName, version, tt );
				file = _openLogFile( dateFileName, "a", fileNameEncoding );
				newFile = true;
			}
		}

		if( _isValidLogFile( file ) && ( flags_ & perDateLimited ) != 0 )
		{
			if( tt->tm_year != lastLogTm.tm_year || tt->tm_mon != lastLogTm.tm_mon || tt->tm_mday != lastLogTm.tm_mday )
			{
				_closeLogFile( file );

				lastLogTm = *tt;
				_bakFile1( baseFileName, fileNameEncoding );
				PString currFileName;
				_logNameByDateLimited( currFileName, baseFileName );
				file = _openLogFile( currFileName, "w", fileNameEncoding );
				newFile = true;
			}
		}

		if (printTime || newFile)
		{
			//NOTE: following date format is relied on in code above [1]
			_print( "[%04d/%02d/%02d %02d:%02d:%02d]", tt->tm_year + 1900, tt->tm_mon + 1, tt->tm_mday, tt->tm_hour, tt->tm_min, tt->tm_sec );
			if (newFile &&  !(flags_ & printProcessId))
				_print( "PID=%d", (int)PID());
	#ifdef PWIN32
			if (newFile  && ( flags_ & perDate ) != 0 )
				_print( "BASEADDR=0x%p", getBaseAddress() );
	#endif
			if (newFile)
				_callCallback();
			if( ( flags_ & noFlush ) == 0 )
				_flushLogFile( file );
			lastLogTime = tm;
		}
	}
#ifdef NOLOCK_LOG
#ifndef PNOTHREADS
	lock.unlock();
#endif
#endif
}


void PLogFile::vlog( _In_opt_z_ const char* prefix, _In_z_ _Printf_format_string_ const char* fmt, va_list args )
{
	{
		int flags_ = flags; // use local copy
		checkRotate(true);
		struct tm tm;
		PCurrentLocalTime(&tm);
		char buff[1024];
		buff[0] = 0;
		int n = 0;
		if (flags_ & printTimePerLine)
			n += sprintf(buff + n, "%02d:%02d:%02d ", tm.tm_hour, tm.tm_min, tm.tm_sec);
		if (flags_ & printProcessId)
			n += sprintf(buff + n, "PID=%d ", (int)PID());
		if (flags_ & printThreadId)
			n += sprintf(buff + n, "THREADID=%d ", (int)THREADID());
		if (prefix)
		{
			if (n + strlen(prefix) < sizeof(buff) / sizeof(*buff))
				strcpy(buff + n, prefix);
		}
		_vprint(buff, fmt, args);
		if ((flags_ & noFlush) == 0)
			_flushLogFile(file);
#if defined(P_ANDROID)
		if (flags & alsoToStdout)
			__android_log_vprint(ANDROID_LOG_DEBUG, "PLogFile::vLog", fmt, args);
#endif
	}

#if defined(_WIN32) && !defined(COMM_CLIENT_ONLY)
	bool logStackTrace = true;
	const char *p1 = fmt, *p2 = "faulty logic";
	for (; *p1 && *p2; ++p1, ++p2)
	{
		if (*p1 != *p2)
		{
			logStackTrace = false;
			break;
		}
	}
	logStackTrace &= !*p2;
	if (logStackTrace)
	{
		char buf[1024];
		StackTrace::getStackTrace(buf, sizeof(buf));
		size_t len = strlen(buf);
		_writeLogFile(file, buf, len);
		if (flags & alsoToStdout)
			::fwrite(buf, 1, len, stdout);
	}
#endif // _WIN32 && !COMM_CLIENT_ONLY
}

void PLogFile::dump( _In_z_ const char* s )
{
	const bool needStdOut = ( flags & alsoToStdout );
#ifdef NOLOCK_LOG
	size_t len = strlen( s );
	_writeLogFile( file, s, len );
	if( needStdOut )
		::fwrite( s, 1, len, stdout );
#else
	if( _isValidLogFile( file ) )
		::fputs( s, file );
	if( needStdOut )
		::fputs( s, stdout );
#endif
}

bool PLogFile::reopen()
{
	int flags_ = flags; // use local copy
	struct tm tm;

#ifdef NOLOCK_LOG
#ifndef PNOTHREADS
	PLock lock( _cs );
#endif
#endif

	PCurrentLocalTime( &tm );
	if( ( flags_ & perDate ) != 0 )
	{
		if  (_isValidLogFile( file ) )
			_closeLogFile( file );
		const struct tm* tt = &tm;
		if( tt->tm_year != lastLogTm.tm_year || tt->tm_mon != lastLogTm.tm_mon || tt->tm_mday != lastLogTm.tm_mday )
		{
			lastLogTm = *tt;
			version = 0;
		}
		else
		{
			version++;
		}
		PString dateFileName;
		_logNameByDate( dateFileName, baseFileName, version, tt );
		file = _openLogFile( dateFileName, "a", fileNameEncoding );

		//NOTE: following date format is relied on in code above [1]
		_print( "[%04d/%02d/%02d %02d:%02d:%02d]", tt->tm_year + 1900, tt->tm_mon + 1, tt->tm_mday, tt->tm_hour, tt->tm_min, tt->tm_sec );
		if ( !(flags_ & printProcessId))
			_print( "PID=%d", (int)PID());
#ifdef PWIN32
		_print( "BASEADDR=0x%p", getBaseAddress() );
#endif
		_callCallback();

		lastLogTime = tm;
		return true;
	}
	return false;
}


//****************************************************************************

static PLogFile* _logFile = 0;

bool PLogIsInit()
{
	return _logFile != 0;
}

const char * PLogGetFileName()
{
	if (_logFile)
		return _logFile->getBaseFileName();
	else
		return "";
}
bool PLogReopen()
{
	if (_logFile)
		return _logFile->reopen();
	else
		return false;

}
void PLogCheckRotate()
{
	if (_logFile)
		_logFile->checkRotate(false);
}


#ifdef PWIN32//you can name it a win32 hack :-)
#ifndef NDEBUG

static int _ourReportingFunction( int reportType, char *userMessage, int *retVal )
{
	const char* crtType = "UNKNOWN";
	switch( reportType )
	{
	case _CRT_WARN:
		crtType = "WARN";
		break;
	case _CRT_ERROR:
		crtType = "ERROR";
		break;
	case _CRT_ASSERT:
		crtType = "ASSERT";
		break;
	}
	PLog( "_CRT_%s: %s", crtType, userMessage );
	*retVal = 0;
	return TRUE;
}

#endif
#endif

class _PLogErrorCallback : public PError::Callback
{
public:
	void error( const char* msg ) override
	{
		PLog( "PError( '%s' )", msg );
	}
};

#ifdef PWIN32
#ifndef PWIN64
extern "C" int __active_heap;
#endif
// Doesn't exist in VS2017 runtime and not needed anyway
//extern "C" HANDLE _crtheap;
#endif

#ifdef PWIN32
int Plib_handle_program_memory( size_t bytes )
{
	throw PSystemMemoryError("'new', number of bytes requested",(int)bytes);
}

#if !defined(COMM_CLIENT_ONLY) && !defined(PPYCL)
void initSigHandlers()
{
	signal(SIGABRT, PError::sigHandler);
	signal(SIGFPE, PError::sigHandler);
	signal(SIGILL, PError::sigHandler);
	signal(SIGINT, PError::sigHandler);
	signal(SIGSEGV, PError::sigHandler);
	signal(SIGTERM, PError::sigHandler);
}
#endif
#endif

void PLogInit( /* new */ PLogFile* logFile )//thread-unsafe!
{
#ifdef PWIN32
	_set_new_handler(Plib_handle_program_memory); 
#if !defined(COMM_CLIENT_ONLY) && !defined(PPYCL)
	initSigHandlers();
#endif
#endif
#ifndef NOLOCK_LOG
#ifndef PNOTHREADS
	PASSERT4( _cs == 0 );
	_cs = new PCriticalSection();
	LONGSYSTEMTICKS(0);
#endif
#endif
	PASSERT4( _logFile == 0 );
	_logFile = logFile;

#ifdef PWIN32
	if( logFile->flags & PLogFile::catchSystemDbgOutput )
	{
#ifndef NDEBUG
		_CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_DEBUG );//just in case
		_CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_DEBUG );//just in case
		_CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_DEBUG );//just in case
		_CrtSetReportHook( _ourReportingFunction );
#endif
	}
#endif

	if( logFile->baseFileName && ( logFile->flags & PLogFile::catchStdErr ) )
	{
		PString fName;
		fName.append( logFile->baseFileName ).append( ".stderr" );
		FILE* dummy = ::freopen( fName, "a", stderr );
		if( dummy )
		{
#ifdef PPOSIX
			int flags = ::fcntl(fileno(dummy),F_GETFD,0);
			::fcntl(fileno(dummy),F_SETFD,flags | FD_CLOEXEC);
#endif			
			setvbuf( dummy, 0, _IONBF, 0 );

			//time_t t = time( 0 );
			//struct tm* tt = localtime( &t );
			//PASSERT3( tt );
			struct tm tm;
			PCurrentLocalTime( &tm );
			const struct tm* tt = &tm;

			fprintf( dummy, "[%04d/%02d/%02d %02d:%02d:%02d]\n", tt->tm_year + 1900, tt->tm_mon + 1, tt->tm_mday, tt->tm_hour, tt->tm_min, tt->tm_sec );
			fprintf( dummy, "stderr reopened Ok\n" );
			setvbuf( stderr, 0, _IONBF, 0 );
			fprintf( stderr, "stderr reopened - testing stderr\n" );
		}

		//according to doc, should fclose( dummy ), but...
	}

	if( logFile->flags & PLogFile::catchPErrors )
		PErrorInitCallback( new _PLogErrorCallback() );

#ifdef PWIN32
#ifndef PWIN64
#if _MSC_VER > 1400
	int __active_heap = 1;
#endif
	PLog( "VC++ __active_heap = %d", __active_heap );
#endif
// Not needed and not compilable on VS2017
//#ifndef COMM_CLIENT_ONLY
//	try
//	{
//		HMODULE hl = ::LoadLibraryA( "Kernel32.dll" );
//		if( hl )
//		{
//			ULONG u = 255;
//			HQI fq = (HQI)GetProcAddress( hl, "HeapQueryInformation" );
//			SIZE_T sz = 0;
//			if( fq )
//				fq( _crtheap, PPHeapCompatibilityInformation, &u, sizeof( u ), &sz );
//			PLog( "HeapQueryInformation=%u", u );
//			FreeLibrary( hl );
//		}
//	}
//	catch( ... )
//	{
//	}
//#endif

	DWORD_PTR pa = 0, sa = 0;
	GetProcessAffinityMask( GetCurrentProcess(), &pa, &sa );
	PLog( "GetProcessAffinityMask=0x%X", pa );
#endif

	void* p = realloc( 0, 48 );
	if( p )
	{
		free( p );
	}
	else
	{
		PLog( "Substandard realloc" );
		throw PSystemMemoryError( "Substandard realloc", 48 );
	}

	PLog( "tryLock %s", PCriticalSection::tryLockSupported() ? "supported" : "not supported" );
	if( ! ( logFile->flags & PLogFile::printProcessId ) )
		PLog("PID=%d", (int)PID());
#ifdef PWIN32
	PLog( "BASEADDR=0x%p", getBaseAddress() );
#if !defined(COMM_CLIENT_ONLY)
	logProcessHeapInfo(); // only for Server side
	logCompilerInfo();
#endif // COMM_CLIENT_ONLY
#endif
	logFile->_callCallback();
}

void PLogFree()//thread-unsafe!
{
	PASSERT4( _logFile );
	delete _logFile;
	_logFile = NULL;
}

void PLogUpdateFlags( int updateMask, int newFlags )
{
#ifndef NOLOCK_LOG
#ifndef PNOTHREADS
	PASSERT4( _cs );
	PLock lock( *_cs );
#endif
#endif
	PASSERT4( _logFile );
	_logFile->updateFlags( updateMask, newFlags );
}

void PLog( _In_z_ _Printf_format_string_ const char* fmt, ... )//thread-safe
{
#ifndef NOLOCK_LOG
#ifndef PNOTHREADS
	if (_cs == 0)
		return;
	PLock lock( *_cs );
#endif
#endif

	if (_logFile == 0)
		return;
	va_list args;
	va_start( args, fmt );
	_logFile->vlog( 0, fmt, args );
	va_end( args );
}

void PLogError( _In_z_ _Printf_format_string_ const char* fmt, ... )//thread-safe
{
#ifndef NOLOCK_LOG
#ifndef PNOTHREADS
	PASSERT4( _cs );
	PLock lock( *_cs );
#endif
#endif

	PASSERT4( _logFile );
	va_list args;
	va_list argsStdErr;
	va_start( args, fmt );
#ifdef va_copy
	va_copy(argsStdErr,args);
#else
	argsStdErr = args;
#endif
	_logFile->vlog( 0, fmt, args );
	va_end( args );
	if (_logFile->isStderror())
	{
		struct tm tm;
		PCurrentLocalTime( &tm );
		fprintf( stderr, "[%04d/%02d/%02d %02d:%02d:%02d]\n", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec );
		vfprintf(stderr,fmt,argsStdErr);
		fprintf(stderr,"\n");
	}
	va_end( args );
}

void PVLog( _In_z_ _Printf_format_string_ const char* fmt, va_list args )//thread-safe
{
#ifndef NOLOCK_LOG
#ifndef PNOTHREADS
	PASSERT4( _cs );
	PLock lock( *_cs );
#endif
#endif

	PASSERT4( _logFile );
	_logFile->vlog( 0, fmt, args );
}

void PDump( _In_z_ const char* s )
{
#ifndef NOLOCK_LOG
#ifndef PNOTHREADS
	PASSERT4( _cs );
	PLock lock( *_cs );
#endif
#endif

	if( _logFile )
		_logFile->dump( s );
}

void PVLogWithPrefix( _In_opt_z_ const char* prefix, _In_z_ _Printf_format_string_ const char* fmt, va_list args )//thread-safe
{
#ifndef NOLOCK_LOG
#ifndef PNOTHREADS
	PASSERT4( _cs );
	PLock lock( *_cs );
#endif
#endif
	PASSERT4( _logFile );
	_logFile->vlog( prefix, fmt, args );
}

void PLogSetCallback(PLogFile::Callback *_callBack)
{
#ifndef NOLOCK_LOG
#ifndef PNOTHREADS
	PASSERT4( _cs );
	PLock lock( *_cs );
#endif
#endif
	PASSERT4( _logFile );
	_logFile->addCallBack( _callBack);

}
