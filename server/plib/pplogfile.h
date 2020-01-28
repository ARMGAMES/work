#ifndef pplogfile_h_included
#define pplogfile_h_included

#include "ppinclude.h"
#include "ppstring.h"
#include "ppthread.h"

#if defined( PWIN32 )
#define NOLOCK_LOG
#endif

#if defined( NOLOCK_LOG ) && defined( PWIN32 )
#define LOGFILE_T HANDLE
#else
#define LOGFILE_T FILE*
#endif

class PLogFile//thread-unaware
{
public:
	enum { alsoToStdout = 0x01, printProcessId = 0x02, printThreadId = 0x04, perSession = 0x08, perDate = 0x10, catchSystemDbgOutput = 0x20, catchPErrors = 0x40, catchStdErr = 0x80, perDateLimited = 0x100, noFlush = 0x200, printTimePerLine = 0x400 };
	enum { catchAll = catchSystemDbgOutput | catchPErrors | catchStdErr };

	/* abstract */ class Callback
	{
	public:
		virtual void opened() = 0;
		virtual ~Callback(){}
	};



private:
	list<Callback *> _plogCallBacks;
	friend void PLogInit( PLogFile* );
	LOGFILE_T file;
	int flags;
	int version;
	//time_t lastLogTime;
	struct tm lastLogTime;
	PString baseFileName;
	/* new */ PStringEncoding* fileNameEncoding;//storing clone as PLogFile scope is often unclear
	struct tm lastLogTm;//perDate only
#ifdef NOLOCK_LOG
#ifndef PNOTHREADS
	PCriticalSection _cs;
#endif
#endif


public:
	PLogFile( const char* fileName, int flags_ = 0, const PStringEncoding* fileNameEncoding_ = NULL );
	bool isStderror() const { return (flags & PLogFile::catchStdErr) ? true : false; }
	void updateFlags( int updateMask, int newFlags );
	//NOTE: eligible to update flags are: 
	// alsoToStdout, printProcessId, printThreadId, noFlush
	~PLogFile();
	void log( _In_z_ _Printf_format_string_ const char* fmt, ... ); //+V576, class:PLogFile,function:log, format_arg:1, ellipsis_arg:2
	void vlog( _In_opt_z_ const char* prefix, _In_z_ _Printf_format_string_ const char* fmt, va_list args );
	void dump( _In_z_ const char* s );
	const char * getBaseFileName() const
	{
		return baseFileName.c_str();
	}
	bool reopen();
	void addCallBack(Callback *);
	void checkRotate(bool printTime);
private:
	void _print( _In_z_ _Printf_format_string_ const char* fmt, ... ); //+V576, class:PLogFile,function:_print, format_arg:1, ellipsis_arg:2
	void _vprint( _In_z_ const char* prefix, _In_z_ _Printf_format_string_ const char* fmt, va_list args );

	void _callCallback();

private://prohibited
	PLogFile( const PLogFile& );
	PLogFile& operator =( const PLogFile& );
};

void PLogInit( /* new */ PLogFile* logFile );//thread-unsafe!
void PLogFree();//thread-unsafe!
void PLogUpdateFlags( int updateMask, int newFlags );//thread-safe
//NOTE: for list of eligible to update flags see PLogFile::updateFlags()
void PLog( _In_z_ _Printf_format_string_ const char* fmt, ... );//thread-safe             //+V576, function:PLog, format_arg:1, ellipsis_arg:2
void PLogError( _In_z_ _Printf_format_string_ const char* fmt, ... );//thread-safe        //+V576, function:PLogError, format_arg:1, ellipsis_arg:2
void PVLog( _In_z_ _Printf_format_string_ const char* fmt, va_list args );//thread-safe
void PVLogWithPrefix( _In_opt_z_ const char* prefix, _In_z_ _Printf_format_string_ const char* fmt, va_list args );//thread-safe
void PDump( _In_z_ const char* s );//thread-safe
//void PLogCounters(UINT32 & counterBefore,UINT32 & counterAfter, UINT32 & number);
//void PLogCountersSetZero(UINT32 & counterBefore,UINT32 & counterAfter, UINT32 & number);

bool PLogIsInit();
const char * PLogGetFileName();
bool PLogReopen();
void PLogCheckRotate();

void PLogSetCallback( /*new*/ PLogFile::Callback *_callBack);
//PTRACELVL - PLIB trace level
//level 1 - application level (HIGH)
//level 2 - application level
//level 3 - application level 
//level 4 - application level 
//level 5 - application level (LOW)

#ifndef PTRACELVL
#define PTRACELVL 5
#endif

#if PTRACELVL >= 1
#define PTRACE PLog
#else
#define PTRACE(...) ((void)0)
#endif

#if PTRACELVL >= 2
#define PTRACE2 PLog
#else
#define PTRACE2(...) ((void)0)
#endif

#if PTRACELVL >= 3
#define PTRACE3 PLog
#else
#define PTRACE3(...) ((void)0)
#endif

#if PTRACELVL >= 4
#define PTRACE4 PLog
#else
#define PTRACE4(...) ((void)0)
#endif

#if PTRACELVL >= 5
#define PTRACE5 PLog
#else
#define PTRACE5(...) ((void)0)
#endif

//PTRACEX_Y family - differs from PTRACEX family in that it doesn't leave internals of macro in the code if undefined
//the drawback is that it needs to specify Y - number of parameters in macro

#if PTRACELVL >= 1
#define PTRACE1_1( x1 ) PLog( x1 )
#define PTRACE1_2( x1, x2 ) PLog( x1, x2 )
#define PTRACE1_3( x1, x2, x3 ) PLog( x1, x2, x3 )
#define PTRACE1_4( x1, x2, x3, x4 ) PLog( x1, x2, x3, x4 )
#define PTRACE1_5( x1, x2, x3, x4, x5 ) PLog( x1, x2, x3, x4, x5 )
#else
#define PTRACE1_1( x1 )
#define PTRACE1_2( x1, x2 )
#define PTRACE1_3( x1, x2, x3 )
#define PTRACE1_4( x1, x2, x3, x4 )
#define PTRACE1_5( x1, x2, x3, x4, x5 )
#endif

#if PTRACELVL >= 2
#define PTRACE2_1( x1 ) PLog( x1 )
#define PTRACE2_2( x1, x2 ) PLog( x1, x2 )
#define PTRACE2_3( x1, x2, x3 ) PLog( x1, x2, x3 )
#define PTRACE2_4( x1, x2, x3, x4 ) PLog( x1, x2, x3, x4 )
#define PTRACE2_5( x1, x2, x3, x4, x5 ) PLog( x1, x2, x3, x4, x5 )
#else
#define PTRACE2_1( x1 )
#define PTRACE2_2( x1, x2 )
#define PTRACE2_3( x1, x2, x3 )
#define PTRACE2_4( x1, x2, x3, x4 )
#define PTRACE2_5( x1, x2, x3, x4, x5 )
#endif

#if PTRACELVL >= 3
#define PTRACE3_1( x1 ) PLog( x1 )
#define PTRACE3_2( x1, x2 ) PLog( x1, x2 )
#define PTRACE3_3( x1, x2, x3 ) PLog( x1, x2, x3 )
#define PTRACE3_4( x1, x2, x3, x4 ) PLog( x1, x2, x3, x4 )
#define PTRACE3_5( x1, x2, x3, x4, x5 ) PLog( x1, x2, x3, x4, x5 )
#else
#define PTRACE3_1( x1 )
#define PTRACE3_2( x1, x2 )
#define PTRACE3_3( x1, x2, x3 )
#define PTRACE3_4( x1, x2, x3, x4 )
#define PTRACE3_5( x1, x2, x3, x4, x5 )
#endif

#if PTRACELVL >= 4
#define PTRACE4_1( x1 ) PLog( x1 )
#define PTRACE4_2( x1, x2 ) PLog( x1, x2 )
#define PTRACE4_3( x1, x2, x3 ) PLog( x1, x2, x3 )
#define PTRACE4_4( x1, x2, x3, x4 ) PLog( x1, x2, x3, x4 )
#define PTRACE4_5( x1, x2, x3, x4, x5 ) PLog( x1, x2, x3, x4, x5 )
#else
#define PTRACE4_1( x1 )
#define PTRACE4_2( x1, x2 )
#define PTRACE4_3( x1, x2, x3 )
#define PTRACE4_4( x1, x2, x3, x4 )
#define PTRACE4_5( x1, x2, x3, x4, x5 )
#endif

#if PTRACELVL >= 5
#define PTRACE5_1( x1 ) PLog( x1 )
#define PTRACE5_2( x1, x2 ) PLog( x1, x2 )
#define PTRACE5_3( x1, x2, x3 ) PLog( x1, x2, x3 )
#define PTRACE5_4( x1, x2, x3, x4 ) PLog( x1, x2, x3, x4 )
#define PTRACE5_5( x1, x2, x3, x4, x5 ) PLog( x1, x2, x3, x4, x5 )
#else
#define PTRACE5_1( x1 )
#define PTRACE5_2( x1, x2 )
#define PTRACE5_3( x1, x2, x3 )
#define PTRACE5_4( x1, x2, x3, x4 )
#define PTRACE5_5( x1, x2, x3, x4, x5 )
#endif

#endif
