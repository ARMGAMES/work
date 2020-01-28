#ifndef pperror_h_included
#define pperror_h_included

#include "ppincludebase.h"
#ifdef PWIN32
#	include <signal.h>
#endif // PWIN32

class PString;

class PError
{
public:
	/* abstract */ class Callback
	{
	public:
		virtual void error( const char* msg ) = 0;
		virtual ~Callback(){}
	};

private:
	PString* msg;

public:
	PError( PString&& msg_, bool logError = true );
	PError( const char* msg_, bool logError = true );
	PError( const PError& other );
	PError& operator =( const PError& other );
	const char* why() const;
	~PError();

#ifdef PWIN32
	static void __cdecl _translate( unsigned int code, _EXCEPTION_POINTERS* xptr );

#if !defined(COMM_CLIENT_ONLY)
	static void _terminate();
	static void sigHandler(int sig);
#endif
#endif

protected:
	PError();
	/* inline */ PString& _msg();
};

void PErrorInitCallback( /* new */ PError::Callback* callback );//mostly for debug purposes; thread-unsafe!

class PInternalError : public PError
{
public:
	PInternalError()
	{
	}
	PInternalError( const char* msg, bool logError = true )
		: PError( msg, logError)
	{
	}
};

class PParserError : public PInternalError
{
public:
	PParserError()
	{
	}
	PParserError( const char* msg )
		: PInternalError( msg )
	{
	}
};

class PSSLError : public PInternalError
{
public:
	PSSLError()
	{
	}
	PSSLError( const char* msg )
		: PInternalError( msg )
	{
	}
};

class PFatalError : public PInternalError
{
protected:
	PFatalError( )
	{
	}
};


class PSystemError : public PInternalError
{
public:
	PSystemError( const char* msg, const char* errMsg );
	PSystemError( const char* msg, int errCode );

public:
	static void errorMessage( PString& ret, int errCode );

private:
	static void _appendErrMsg( PString& ret, int errCode );
protected:
	PSystemError()
	{
	}
};

class PHostNameError : public PSystemError
{
public:
	PHostNameError( const char* msg, const char* errMsg)
		:PSystemError(msg,errMsg)
	{}
};



class PSystemMemoryError : public PFatalError
{
public:
	PSystemMemoryError( const char* msg, size_t numBytes );
	static int getNumberOfExceptions()
	{
		return numberOfExceptions;
	}
	static void clearNumberOfExceptions()
	{
		numberOfExceptions = 0;
	}
private:
	static int numberOfExceptions;
};

class PSystemExceptionError : public PFatalError
{
public:
	PSystemExceptionError( const char* msg);
	static int getNumberOfExceptions()
	{
		return numberOfExceptions;
	}
	static void clearNumberOfExceptions()
	{
		numberOfExceptions = 0;
	}
private:
	static int numberOfExceptions;
};

class PAssert : public PInternalError
{
public:
	PAssert( const char* cond, const char* file, int line );
	static int getNumberOfAsserts()
	{
		return numberOfAsserts;
	}
	static void clearNumberOfAsserts()
	{
		numberOfAsserts = 0;
	}
private:
	static int numberOfAsserts;

};

//PASSERTLVL - PLIB debug level
//level 1 - application level
//level 2 - application level
//level 3 - application level
//level 4 - PLIB call parameters check
//level 5 - internal PLIB low-level

#ifndef PASSERTLVL
#define PASSERTLVL 5
#endif

#ifdef CONCISE_ASSERT

#if PASSERTLVL >= 1
#define PASSERT(cond) (void)( (cond) || (throw PAssert("", __FILE__, __LINE__), 0) )
#else
#define PASSERT(cond)
#endif

#if PASSERTLVL >= 2
#define PASSERT2(cond) (void)( (cond) || (throw PAssert("", __FILE__, __LINE__), 0) )
#else
#define PASSERT2(cond)
#endif

#if PASSERTLVL >= 3
#define PASSERT3(cond) (void)( (cond) || (throw PAssert("", __FILE__, __LINE__), 0) )
#else
#define PASSERT3(cond)
#endif

#if PASSERTLVL >= 4
#define PASSERT4(cond) (void)( (cond) || (throw PAssert("", __FILE__, __LINE__), 0) )
#else
#define PASSERT4(cond)
#endif

#if PASSERTLVL >= 5
#define PASSERT5(cond) (void)( (cond) || (throw PAssert("", __FILE__, __LINE__), 0) )
#else
#define PASSERT5(cond)
#endif

#else // CONCISE_ASSERT

#if PASSERTLVL >= 1
#define PASSERT(cond) (void)( (cond) || (throw PAssert(#cond, __FILE__, __LINE__), 0) )
#else
#define PASSERT(cond)
#endif

#if PASSERTLVL >= 2
#define PASSERT2(cond) (void)( (cond) || (throw PAssert(#cond, __FILE__, __LINE__), 0) )
#else
#define PASSERT2(cond)
#endif

#if PASSERTLVL >= 3
#define PASSERT3(cond) (void)( (cond) || (throw PAssert(#cond, __FILE__, __LINE__), 0) )
#else
#define PASSERT3(cond)
#endif

#if PASSERTLVL >= 4
#define PASSERT4(cond) (void)( (cond) || (throw PAssert(#cond, __FILE__, __LINE__), 0) )
#else
#define PASSERT4(cond)
#endif

#if PASSERTLVL >= 5
#define PASSERT5(cond) (void)( (cond) || (throw PAssert(#cond, __FILE__, __LINE__), 0) )
#else
#define PASSERT5(cond)
#endif

#endif // CONCISE_ASSERT

//****************************************************************************

class PErrorHandler
{
public:
#ifdef PPOSIX
	PErrorHandler();
#endif 
	void execute();//calls run() trying to convert all system errors to PError's and catch all
	virtual void run() = 0;
};


#endif
