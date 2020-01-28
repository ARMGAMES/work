#ifndef ppzip_h_included
#define ppzip_h_included

#include "ppinclude.h"
#include "ppstream.h"

/*
class PGZip
{
private:
void* impl;//pimpl

public:
PGZip();
void openRead( const char* fName );
void openWrite( const char* fName, int compLevel = -1 );
size_t read( BYTE* p, size_t sz );
void write( const BYTE* p, size_t sz );
void close();
~PGZip();
};*/

class PDeflate
{
private:
	void* impl;//pimpl

public:
	PDeflate( POutputStream& stream, int compLevel = -1 );
	PDeflate( POutputStream& stream, int compLevel, int wBits, int memLevel );//wBits MUST be in range 8 - 15; memLevel MUST be in range 1 - 9

	void write( const BYTE* p, size_t sz );
	void close();
	~PDeflate();
};

// PYR-74625: When using this class, please ensure the message being decompressed DOESN'T come from the end users, due to known vulnerabilities in zlib.
class PInflate
{
private:
	void* impl;//pimpl

public:
	PInflate( PInputStream& stream );
	PInflate( PInputStream& stream, int wBits );//wBits MUST be in range 8 - 15
	size_t read( BYTE* p, size_t sz );
	void close();
	~PInflate();
};

class PGZip
{
private:
	void* impl;//pimpl

public:
	PGZip( POutputStream& stream, int compLevel = -1 );
	void write( const BYTE* p, size_t sz );
	void close();
	~PGZip();
};

class PUnGZip
{
private:
	void* impl;//pimpl

public:
	PUnGZip( PInputStream& stream );
	size_t read( BYTE* p, size_t sz );
	void close();
	~PUnGZip();
};

#ifdef OLDZIP
class PZip
{
private:
	void* impl;//pimpl

public:
	PZip();
	/*
	void openRead( const char* fName );
	size_t read( BYTE* p, size_t sz );
	void openReadSubFile( const char* fName );
	void closeReadSubFile( const char* fName );
	*/

	void openWrite( const char* fName, int compLevel = -1 );
	void openWriteSubFile( const char* subFName, const struct tm* subFTime );
	void write( const BYTE* p, size_t sz );
	void closeWriteSubFile();
	void close();
	~PZip();
};
#else
class PZip
{
private:
	void* impl;//pimpl

public:
	PZip( POutputStream& stream, int compLevel = -1 );
	/*
	void openRead( const char* fName );
	size_t read( BYTE* p, size_t sz );
	void openReadSubFile( const char* fName );
	void closeReadSubFile( const char* fName );
	*/

	//void openWrite( const char* fName, int compLevel = -1 );
	void openSubFile( const char* subFName, const struct tm* subFTime = 0 /* 0 means 'now' */ );
	void write( const BYTE* p, size_t sz );
	void closeSubFile();
	void close();
	~PZip();
};
#endif

#endif
