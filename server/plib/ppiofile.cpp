#include "ppiofile.h"


#ifdef PPOSIX
PIOFile::PIOFile()
{
	handle = -1;
	flags=0;
}

PIOFile::~PIOFile()
{
	if (isOpened())
		closeFile();
}

void PIOFile::openFile(const char * fileName_, UINT32 flags_, bool exclusive)
{
	if (isOpened())
		closeFile();
	flags = flags_;
	int oflag=0;
	switch(flags&0xff)
	{
	case PLIB_FILE_READONLY:
		oflag |= O_RDONLY;
		break;
	case PLIB_FILE_CREATE:
		oflag |= (O_CREAT|O_EXCL|O_RDWR|(flags&PLIB_FILE_FLAG_APPEND?O_APPEND:0));
		break;
	case PLIB_FILE_OPEN:
		oflag |= (O_RDWR|(flags&PLIB_FILE_FLAG_APPEND?O_APPEND:0)|(flags&PLIB_FILE_FLAG_TRUNCATE?O_TRUNC:0));
		break;
	case PLIB_FILE_CREATE_OR_OPEN:
		oflag |= (O_CREAT|O_RDWR|(flags&PLIB_FILE_FLAG_APPEND?O_APPEND:0)|(flags&PLIB_FILE_FLAG_TRUNCATE?O_TRUNC:0));
		break;
	default:
		throw PSystemError( "Error in ::openfile() - wrong type of file", -1);
	}
	handle = ::open(fileName_,oflag,S_IREAD | S_IWRITE);
	if (handle < 0)
		throw PSystemError( "Error in ::openfile()", FILEERRNO() );
}

void PIOFile::closeFile()
{
	if (isOpened())
	{
		::close(handle);
		handle = -1;
		flags=0;
	}
}

UINT64 PIOFile::getSize()
{
	UINT64 current = lseekFile( 0, PLIB_SEEK_FILE_CURRENT );
	UINT64 size = lseekFile( 0 , PLIB_SEEK_FILE_END );
	lseekFile( current, PLIB_SEEK_FILE_BEGIN );
	return size;
}

UINT32 PIOFile::readFile(BYTE* p, UINT32 size)
{
	int ret = static_cast< int >( ::read(handle, p, size) );
	if (ret < 0)
		throw PSystemError( "Error in ::readfile()", FILEERRNO() );
	return static_cast< UINT32 >( ret );
}

void PIOFile::writeFile(const BYTE* p, UINT32 size)
{
	while(size)
	{
		int ret= static_cast< int >( ::write(handle, p, size) );
		if (ret < 0)
			throw PSystemError( "Error in ::writefile()", FILEERRNO() );
		size -= ret;
		p += ret;
	}
	return;
}

INT64 PIOFile::lseekFile(INT64 position,INT32 origin )
{
	int orf=0;
	switch (origin)
	{
	case PLIB_SEEK_FILE_BEGIN:
		orf = SEEK_SET;
		break;
	case PLIB_SEEK_FILE_CURRENT:
		orf = SEEK_CUR;
		break;
	case PLIB_SEEK_FILE_END:
		orf = SEEK_END;
		break;
	default:
		throw PSystemError( "Error in ::lseek() - wrong parameter", -1);
	}
	INT64 ret = ::lseek(handle,position,orf);
	if (ret < 0)
		throw PSystemError( "Error in ::lseek()", FILEERRNO() );
	return ret;
}

void PIOFile::flushFile(  )
{
	if( fsync( handle ) == -1 )
		throw PSystemError( "Error in flushFile", FILEERRNO() );		
}


#else

#ifdef PWIN32
#ifndef INVALID_SET_FILE_POINTER 
#define INVALID_SET_FILE_POINTER ((UINT32)-1)
#endif
#endif

PIOFile::PIOFile()
{
	handle = INVALID_HANDLE_VALUE;
	flags=0;
}

PIOFile::~PIOFile()
{
	if (isOpened())
	{
		::CloseHandle(handle);
	}
}

void PIOFile::openFile(const char * fileName_, UINT32 flags_, bool exclusive)
{
	if (isOpened())
		closeFile();

	flags = flags_;
	UINT32 accessFlag=GENERIC_READ;
	UINT32 createFlag=0;
	switch(flags&0xff)
	{
	case PLIB_FILE_READONLY:
		createFlag |= OPEN_EXISTING;
		break;
	case PLIB_FILE_CREATE:
		accessFlag |= GENERIC_WRITE;
		createFlag = CREATE_NEW;
		break;
	case PLIB_FILE_OPEN:
		accessFlag |= GENERIC_WRITE;
		if (flags&PLIB_FILE_FLAG_TRUNCATE)
			createFlag = TRUNCATE_EXISTING;
		else
			createFlag = OPEN_EXISTING;
		break;
	case PLIB_FILE_CREATE_OR_OPEN:
		accessFlag |= GENERIC_WRITE;
		if (flags&PLIB_FILE_FLAG_TRUNCATE)
			createFlag = CREATE_ALWAYS;
		else
			createFlag = OPEN_ALWAYS;
		break;
	default:
		throw PSystemError( "Error in ::openfile() - wrong type of file", -1);
	}
	handle = ::CreateFileA(fileName_,accessFlag, ( exclusive ? 0 : FILE_SHARE_READ) ,0,createFlag,FILE_ATTRIBUTE_NORMAL,0);
	if (handle == INVALID_HANDLE_VALUE)
		throw PSystemError( "Error in ::openfile()", FILEERRNO() );
}

void PIOFile::closeFile()
{
	if (isOpened())
	{
		if (!::CloseHandle(handle))
		{
			throw PSystemError( "Error in ::closeFile()", FILEERRNO() );
		}
		handle = INVALID_HANDLE_VALUE;
		flags=0;
	}
}

UINT64 PIOFile::getSize()
{
	ULARGE_INTEGER pos;
	pos.QuadPart=0;
	pos.LowPart = GetFileSize( handle, &pos.HighPart );
	if ((pos.LowPart == INVALID_FILE_SIZE) && (FILEERRNO() != NO_ERROR))
		throw PSystemError( "Error in ::getSize", FILEERRNO() );
	return pos.QuadPart;
}

UINT32 PIOFile::readFile(BYTE* p, UINT32 size)
{
	UINT32 readed;
	BOOL ret = ::ReadFile(handle,p,size,(unsigned long *)&readed,0);
	if (!ret)
		throw PSystemError( "Error in ::readfile()", FILEERRNO() );
	return readed;
}

void PIOFile::writeFile(const BYTE *p, UINT32 size)
{
	if (flags&PLIB_FILE_FLAG_APPEND)
	{
		lseekFile(0,PLIB_SEEK_FILE_END);
	}
	while(size)
	{
		UINT32 written;
		BOOL ret = ::WriteFile(handle,p,size,(unsigned long *)&written,0);
		if (!ret )
			throw PSystemError( "Error in ::writefile()", FILEERRNO() );
		p += written;
		size -= written;
	}
	return;
}

INT64 PIOFile::lseekFile(INT64 position,INT32 moveType )
{
	UINT32 mt=0;
	switch (moveType)
	{
	case PLIB_SEEK_FILE_BEGIN:
		mt = FILE_BEGIN;
		break;
	case PLIB_SEEK_FILE_CURRENT:
		mt = FILE_CURRENT;
		break;
	case PLIB_SEEK_FILE_END:
		mt = FILE_END;
		break;
	default:
		throw PSystemError( "Error in ::lseek() - wrong parameter", -1);
	}
	LARGE_INTEGER pos;
	pos.QuadPart=position;
	pos.LowPart = ::SetFilePointer(handle,pos.LowPart,&pos.HighPart,mt);
	if ((pos.LowPart == INVALID_SET_FILE_POINTER) && (FILEERRNO() != NO_ERROR))
		throw PSystemError( "Error in ::lseek()", FILEERRNO() );
	return pos.QuadPart;
}


void PIOFile::flushFile()
{
	if( FlushFileBuffers( handle ) == FALSE )
		throw PSystemError( "Error in flushFile()", FILEERRNO() );
}


#endif





PBufferedIOFile::PBufferedIOFile(INT32 bufferSize_)
{
	endPointer = 0;
	buffer = 0;
	bufferSize = bufferSize_;
}

PBufferedIOFile::~PBufferedIOFile()
{
	closeFile(false);
}

void PBufferedIOFile::openFile(	const char *fileName)
{
	closeFile(); // just in case.
	file.openFile(fileName);
	buffer=(BYTE *)malloc(bufferSize);
	endPointer = buffer;
}

void PBufferedIOFile::closeFile(bool allowThrowExcp)
{
	try
	{
		flushFile();
	}
	catch (...)
	{
		if (allowThrowExcp)
		{
			throw;
		}
	}
	try
	{
		file.closeFile();
	}
	catch (...)
	{
		if (allowThrowExcp)
		{
			throw;
		}
	}
	if (_isAllocated())
	{
		::free(buffer);
		buffer=0;
		endPointer = 0;
	}
}

UINT32 PBufferedIOFile::writeFile(const BYTE*p ,UINT32 size)
{
	UINT32 sz = size;
	const BYTE * pcur = p;
	while (sz)
	{
		UINT32 rest = static_cast< UINT32 >(bufferSize - (endPointer - buffer));
		if (rest >= sz)
		{
			// this is last write
			memcpy(endPointer,pcur,sz);
			endPointer += sz;
			if ( rest == sz )
				flushFile(); // don't want to keep full buffer
			sz=0;
		}
		else
		{
			if( rest == static_cast< UINT32 >( bufferSize ) )
			{
				// buffer empty, and we don't have enough size to keep data anyway - write everything directly on disk
				file.writeFile(pcur,sz);
				sz=0;
			}
			else
			{
				memcpy(endPointer,pcur,rest);
				endPointer += rest;
				flushFile(); 
				sz	 -= rest;
				pcur += rest;
			}
		}
	}
	return size;
}

void PBufferedIOFile::flushFile()
{
	if (isOpened())
	{
		UINT32 size = static_cast< UINT32 >(endPointer - buffer);
		if (size)
		{
			file.writeFile(buffer,size);
			endPointer = buffer;
		}
	}
}

