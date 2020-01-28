#ifndef ppiofile_h_included
#define ppiofile_h_included

#include "ppinclude.h"
#include "pperror.h"
#include "ppstring.h"

// These are not binary flags, these are types of opening files! Don't combine!

#define PLIB_FILE_READONLY			1 // open in read only mode. Write operation won't be permitted. 
#define PLIB_FILE_CREATE			2 // create new file for writing. If file exists return error.
#define PLIB_FILE_OPEN				3 // open file for writing. If file doesn't exist return error
#define PLIB_FILE_CREATE_OR_OPEN	4 // create new file. If file exists open file for writing

// These are flags, they could be used with any writing types of open: 
#define PLIB_FILE_FLAG_APPEND		0x100  // all write operations start from the end of file regardless of file pointerfile pointer
// read operations start from file pointer
// After any write operations file pointer points on the end of file.
#define PLIB_FILE_FLAG_TRUNCATE		0x200 // if file exists delete content
#define PLIB_FILE_DEFAULT	(PLIB_FILE_CREATE_OR_OPEN|PLIB_FILE_FLAG_APPEND)

#define PLIB_SEEK_FILE_BEGIN	1
#define PLIB_SEEK_FILE_CURRENT	2
#define PLIB_SEEK_FILE_END		3

#if defined( LINUX ) && !defined(P_ANDROID)

#define S_IREAD (S_IRUSR|S_IRGRP|S_IROTH)
#define S_IWRITE (S_IWUSR|S_IWGRP|S_IWOTH)

#endif


class PIOFile
{
private:
#ifdef PPOSIX
	int handle;
#else
	HANDLE handle;
#endif
	UINT32 flags;
	PIOFile(const PIOFile &other); // prohibited
	PIOFile& operator = (const PIOFile & other); // prohibited
public:
	PIOFile();
	~PIOFile();
	void openFile(const char * fileName_,UINT32 flags_ = PLIB_FILE_DEFAULT, bool exclusive = false);
	void closeFile();
	bool isOpened() const
	{
#ifdef PPOSIX
		return handle != -1;
#else	
		return handle != INVALID_HANDLE_VALUE;
#endif
	}
	UINT64 getSize();
	UINT32 readFile(BYTE* p, UINT32 size);
	void writeFile(const BYTE* p, UINT32 size);
	INT64 lseekFile(INT64 position,INT32 origin );
	void  flushFile();
};

#define PLIB_SIZE_OF_STREAM_BUFFER 4096
class PBufferedIOFile 
{
private:
	INT32 bufferSize;
	PIOFile file;
	BYTE * buffer;
	BYTE * endPointer;

	bool _isAllocated() const
	{
		return buffer != 0;
	}


	PBufferedIOFile (const PBufferedIOFile  & other); // prohibited
	PBufferedIOFile & operator = (const PBufferedIOFile  & other); // prohibited
public:
	PBufferedIOFile (INT32 bufferSize_ = PLIB_SIZE_OF_STREAM_BUFFER);
	~PBufferedIOFile ();
	void openFile(	const char *fileName);
	void closeFile(bool allowThrowExcp = true);
	UINT32 writeFile(const BYTE*p ,UINT32 size);
	void flushFile();

	bool isOpened() const
	{
		return _isAllocated() && file.isOpened();
	}

};




#endif
