#ifndef _commcomm_h_included
#define _commcomm_h_included

#include "ppinclude.h"
#include "ppstring.h"
#include "pplogfile.h"

struct _CommAsyncRet
{
public:
	size_t read;
	size_t written;

public:
	_CommAsyncRet()
	{
		read = 0;
		written = 0;
	}
};



template< class _Communicator >
//	_Communicator prereqs:
//	size_t writeIncomplete( const BYTE* p, size_t sz );
//	size_t readIncomplete( BYTE* p, size_t sz );
//	_CommAsyncRet asyncWriteOk();
//	_CommAsyncRet asyncReadOk();
//	_CommCompleteCommunicator provides:
//	size_t write( const BYTE* p, size_t sz );
//	size_t read( BYTE* p, size_t sz );
class _CommCompleteUDPCommunicator : public _Communicator
{
private:
	const BYTE* wrBuf;
	size_t wrSz;
	size_t wrInitSz;
	BYTE* rdBuf;
	size_t rdSz;
	size_t rdInitSz;

public:
	_CommCompleteUDPCommunicator();

public:
	void init();
//	size_t write( const BYTE* p, size_t sz );
//	size_t read( BYTE* p, size_t sz );

//	size_t _write( const BYTE* p, size_t sz );
//	size_t _read( BYTE* p, size_t sz );
};

template< class _Communicator >
_CommCompleteUDPCommunicator< _Communicator >::_CommCompleteUDPCommunicator()
{
	init();
}


template< class _Communicator >
void _CommCompleteUDPCommunicator< _Communicator >::init()
{
}
/*
template< class _Communicator >
size_t _CommCompleteUDPCommunicator< _Communicator >::write( const BYTE* p, size_t sz )
{
	//FILE* f = fopen( "wr", "ab" );
	//fwrite( p, 1, sz, f );
	//fclose( f );
	size_t wr = _write( p, sz );
	PASSERT3( wr <= sz );
	return wr;
}

template< class _Communicator >
size_t _CommCompleteUDPCommunicator< _Communicator >::read( BYTE* p, size_t sz )
{
	size_t rd = _read( p, sz );
	PASSERT3( rd <= sz );
	return rd;
}


template< class _Communicator >
size_t _CommCompleteUDPCommunicator< _Communicator >::_write( const BYTE* p, size_t sz )
{
	size_t wr = _Communicator::writeIncomplete( p, sz );
	return wr;
}

template< class _Communicator >
size_t _CommCompleteUDPCommunicator< _Communicator >::_read( BYTE* p, size_t sz )
{
	size_t rd = _Communicator::readIncomplete( p, sz );
	return rd;
}
*/


struct _CommCompressedCommunicatorTypes
{
	enum { compressionNone = 0, compressionLzhl = 1, compressionLzhl1 = 2 };
};

//****************************************************************************

template< class _Communicator >
//	_Communicator prereqs:
//	size_t writeIncomplete( const BYTE* p, size_t sz );
//	size_t readIncomplete( BYTE* p, size_t sz );
//	_CommAsyncRet asyncWriteOk();
//	_CommAsyncRet asyncReadOk();
//	_CommCompleteCommunicator provides:
//	size_t write( const BYTE* p, size_t sz );
//	size_t read( BYTE* p, size_t sz );
class _CommCompleteCommunicator : public _Communicator
{
private:
	const BYTE* wrBuf;
	size_t wrSz;
	size_t wrInitSz;
	BYTE* rdBuf;
	size_t rdSz;
	size_t rdInitSz;

public:
	_CommCompleteCommunicator();

public:
	void init();
	size_t write( const BYTE* p, size_t sz );
	size_t read( BYTE* p, size_t sz );
	_CommAsyncRet asyncWriteOk();
	_CommAsyncRet asyncReadOk();

	size_t _write( const BYTE* p, size_t sz );
	size_t _read( BYTE* p, size_t sz );
	void _asyncOk( _CommAsyncRet& ret );
};

template< class _Communicator >
_CommCompleteCommunicator< _Communicator >::_CommCompleteCommunicator()
{
	init();
}

template< class _Communicator >
void _CommCompleteCommunicator< _Communicator >::init()
{
	wrBuf = 0;
	wrSz = 0;
	wrInitSz = 0;
	rdBuf = 0;
	rdSz = 0;
	rdInitSz = 0;
}

template< class _Communicator >
size_t _CommCompleteCommunicator< _Communicator >::write( const BYTE* p, size_t sz )
{
	//FILE* f = fopen( "wr", "ab" );
	//fwrite( p, 1, sz, f );
	//fclose( f );
	PASSERT3( wrBuf == 0 );
	size_t wr = _write( p, sz );
	PASSERT3( wr <= sz );
	if( wr == sz )
		return sz;
	else
	{
		wrBuf = p + wr;
		wrSz = sz - wr;
		wrInitSz = sz;
		return 0;
	}
}

template< class _Communicator >
size_t _CommCompleteCommunicator< _Communicator >::read( BYTE* p, size_t sz )
{
	PASSERT3( rdBuf == 0 );
	size_t rd = _read( p, sz );
	PASSERT3( rd <= sz );
	if( rd == sz )
	{
		//FILE* f = fopen( "rd", "ab" );
		//fwrite( p, 1, sz, f );
		//fclose( f );
		return sz;
	}
	else
	{
		rdBuf = p + rd;
		rdSz = sz - rd;
		rdInitSz = sz;
		return 0;
	}
}

template< class _Communicator >
_CommAsyncRet _CommCompleteCommunicator< _Communicator >::asyncWriteOk()
{
	_CommAsyncRet ret = _Communicator::asyncWriteOk();
	_asyncOk( ret );
	return ret;
}

template< class _Communicator >
_CommAsyncRet _CommCompleteCommunicator< _Communicator >::asyncReadOk()
{
	_CommAsyncRet ret = _Communicator::asyncReadOk();
	_asyncOk( ret );
	return ret;
}

template< class _Communicator >
size_t _CommCompleteCommunicator< _Communicator >::_write( const BYTE* p, size_t sz )
{
	size_t ret = 0;
	for(;;)
	{
		size_t wr = _Communicator::writeIncomplete( p, sz );
		if( wr == 0 )
			return ret;
		ret += wr;
		sz -= wr;
		p += wr;
		if( sz == 0 )
			return ret;
	}
}

template< class _Communicator >
size_t _CommCompleteCommunicator< _Communicator >::_read( BYTE* p, size_t sz )
{
	size_t ret = 0;
	for(;;)
	{
		size_t rd = _Communicator::readIncomplete( p, sz );
		if( rd == 0 )
			return ret;
		ret += rd;
		sz -= rd;
		p += rd;
		if( sz == 0 )
			return ret;
	}
}

template< class _Communicator >
void _CommCompleteCommunicator< _Communicator >::_asyncOk( _CommAsyncRet& ret )
{
	if( ret.written )
	{
		//PTRACE( "_DBG: CompleteComm:written" );
		size_t wr = ret.written;
		PASSERT3( wrBuf );
		wrBuf += wr;
		wrSz -= wr;
		if( wrSz == 0 )
		{
			ret.written = wrInitSz;
			wrBuf = 0;
			//wrSz = 0;
			wrInitSz = 0;
		}
		else
		{
			//PTRACE( "_DBG: CompleteComm:write( %d )", wrSz );
			size_t wr = _write( wrBuf, wrSz );
			PASSERT3( wr <= wrSz );
			if( wr == wrSz )
			{
				ret.written = wrInitSz;
				wrBuf = 0;
				wrSz = 0;
				wrInitSz = 0;
			}
			else
			{
				ret.written = 0;
				wrBuf += wr;
				wrSz -= wr;
			}
		}
	}
	if( ret.read )
	{
		//PTRACE( "_DBG: CompleteComm:read" );
		size_t rd = ret.read;
		PASSERT3( rdBuf );
		rdBuf += rd;
		rdSz -= rd;
		if( rdSz == 0 )
		{
			ret.read = rdInitSz;
			rdBuf = 0;
			//rdSz = 0;
			rdInitSz = 0;
		}
		else
		{
			//PTRACE( "_DBG: CompleteComm:read( %d )", rdSz );
			size_t rd = _read( rdBuf, rdSz );
			PASSERT3( rd <= rdSz );
			if( rd == rdSz )
			{
				ret.read = rdInitSz;
				rdBuf = 0;
				rdSz = 0;
				rdInitSz = 0;
			}
			else
			{
				ret.read = 0;
				rdBuf += rd;
				rdSz -= rd;
			}
		}
	}
}

//****************************************************************************

/* abstract */ class _CommVirtualCommunicator
{
public:
	virtual ~_CommVirtualCommunicator() {}
	virtual size_t writeIncomplete( const BYTE* p, size_t sz ) = 0;
	virtual size_t readIncomplete( BYTE* p, size_t sz ) = 0;
	virtual _CommAsyncRet asyncWriteOk() = 0;
	virtual _CommAsyncRet asyncReadOk() = 0;
};

//****************************************************************************

template< class _Communicator >
class _CommDebugCommunicator : public _Communicator
	//transparent filter communicator. logs all the traffic
{
private:
	const BYTE* rdBuf;
	const BYTE* wrBuf;
	FILE* wr;
	FILE* rd;

public:
	_CommDebugCommunicator()
	{
		rd = wr = 0;
	}
	void initDebug( const char* rdLogFilename, const char* wrLogFilename )
	{
		rd = fopen( rdLogFilename, "wb" );
		PASSERT3( rd );
		wr = fopen( wrLogFilename, "wb" );
		PASSERT3( wr );
	}
	~_CommDebugCommunicator()
	{
		if( wr )
			fclose( wr );
		if( rd )
			fclose( rd );
	}

public:
	size_t writeIncomplete( const BYTE* p, size_t sz )
	{
		wrBuf = p;
		size_t ret = _Communicator::writeIncomplete( p, sz );
		if( ret )
			fwrite( p, 1, ret, wr );
		return ret;
	}
	size_t readIncomplete( BYTE* p, size_t sz )
	{
		rdBuf = p;
		size_t ret = _Communicator::readIncomplete( p, sz );
		if( ret )
			fwrite( p, 1, ret, rd );
		return ret;
	}
	_CommAsyncRet asyncWriteOk()
	{
		_CommAsyncRet ret = _Communicator::asyncWriteOk();
		if( ret.read )
			fwrite( rdBuf, 1, ret.read, rd );
		if( ret.written )
			fwrite( wrBuf, 1, ret.written, wr );
		return ret;
	}
	_CommAsyncRet asyncReadOk()
	{
		_CommAsyncRet ret = _Communicator::asyncReadOk();
		if( ret.read )
			fwrite( rdBuf, 1, ret.read, rd );
		if( ret.written )
			fwrite( wrBuf, 1, ret.written, wr );
		return ret;
	}
};

#endif
