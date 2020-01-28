#include "ppzip.h"
#include "ppstring.h"
#include "pperror.h"

#include "zlib.h"
#ifdef OLDZIP
#include "zip.h"
#else
#include "zip2.h"
#endif
#include "unzip.h"

struct _PZipImpl
{
	int isWrite;
	zipFile fw;
	int wCompLevel;
	//unzFile fr;
};

static inline _PZipImpl* zipImpl( void* p )
{
	return (_PZipImpl*)p;
}

#ifdef OLDZIP
PZip::PZip()
{
	impl = new _PZipImpl();
	zipImpl( impl )->isWrite = -1;
}

/*	
void PZip::openRead( const char* fName )
{
zipImpl( impl )->fr = unzOpen( fName );
if( zipImpl( impl )->fr == 0 )
throw PInternalError( "ZIP: Unable to open file" );
}
*/
void PZip::openWrite( const char* fName, int level )
{
	if( level > 9 )
		level = 9;
	zipImpl( impl )->fw = zipOpen( fName, 0 /*no append*/ );
	if( zipImpl( impl )->fw == 0 )
		throw PInternalError( "ZIP: Unable to open file" );
	zipImpl( impl )->isWrite = 1;
	zipImpl( impl )->wCompLevel = level;
}

void PZip::openWriteSubFile( const char* subFName, const struct tm* subFTime )
{
	zip_fileinfo zi;

	zi.tmz_date.tm_sec = zi.tmz_date.tm_min = zi.tmz_date.tm_hour = 
		zi.tmz_date.tm_mday = zi.tmz_date.tm_mon = zi.tmz_date.tm_year = 0;
	zi.dosDate = 0;
	zi.internal_fa = 0;
	zi.external_fa = 0;
	//	struct tm* tt = localtime( &subFTime );
	const struct tm* tt = subFTime;
	zi.tmz_date.tm_sec = tt->tm_sec;
	zi.tmz_date.tm_min = tt->tm_min;
	zi.tmz_date.tm_hour = tt->tm_hour;
	zi.tmz_date.tm_mday = tt->tm_mday;
	zi.tmz_date.tm_mon = tt->tm_mon;
	zi.tmz_date.tm_year = tt->tm_year;

	int err = zipOpenNewFileInZip(zipImpl( impl )->fw,subFName,&zi,
		NULL,0,NULL,0,NULL /*comment*/,
		(zipImpl( impl )->wCompLevel != 0) ? Z_DEFLATED : 0,
		zipImpl( impl )->wCompLevel < 0 ? Z_DEFAULT_COMPRESSION : zipImpl( impl )->wCompLevel );
	if( err != ZIP_OK )
		throw PInternalError( "ZIP: error opening subFile" );
}

/*
size_t PZip::read( BYTE* p, size_t sz )
{
int rd = gzread( zipImpl( impl )->f, p, sz );
if( rd < 0 )
throw PInternalError( "GZIP: read error" );
return rd;
}
*/
void PZip::write( const BYTE* p, size_t sz )
{
	PASSERT5( zipImpl( impl )->isWrite == 1 );
	int err = zipWriteInFileInZip( zipImpl( impl )->fw, (void*)p, sz );
	if( err != ZIP_OK )
		throw PInternalError( "ZIP: write error" );
}

void PZip::closeWriteSubFile()
{
	PASSERT5( zipImpl( impl )->isWrite == 1 );
	int err = zipCloseFileInZip( zipImpl( impl )->fw );
	if( err != ZIP_OK )
		throw PInternalError( "ZIP: error closing file" );
}

void PZip::close()
{
	if( zipImpl( impl )->isWrite == 1 )
	{
		int err = zipClose( zipImpl( impl )->fw, NULL );
		if( err != ZIP_OK )
			throw PInternalError( "ZIP: error closing file" );
		zipImpl( impl )->isWrite = -1;
	}
}
#else
PZip::PZip( POutputStream& stream, int level )
{
	impl = new _PZipImpl();

	if( level > 9 )
		level = 9;
	zipImpl( impl )->fw = zip2Open( &stream );
	if( zipImpl( impl )->fw == 0 )
		throw PInternalError( "ZIP: Unable to open ZIP file" );
	zipImpl( impl )->isWrite = 1;
	zipImpl( impl )->wCompLevel = level;
}

void PZip::openSubFile( const char* subFName, const struct tm* subFTime )
{
	struct tm tmnow;
	if( subFTime == 0 )
	{
		time_t now = time( 0 );
		tmnow = *localtime( &now );
		subFTime = &tmnow;
	}

	zip_fileinfo zi;

	zi.tmz_date.tm_sec = zi.tmz_date.tm_min = zi.tmz_date.tm_hour = 
		zi.tmz_date.tm_mday = zi.tmz_date.tm_mon = zi.tmz_date.tm_year = 0;
	zi.dosDate = 0;
	zi.internal_fa = 0;
	zi.external_fa = 0;
	//	struct tm* tt = localtime( &subFTime );
	const struct tm* tt = subFTime;
	zi.tmz_date.tm_sec = tt->tm_sec;
	zi.tmz_date.tm_min = tt->tm_min;
	zi.tmz_date.tm_hour = tt->tm_hour;
	zi.tmz_date.tm_mday = tt->tm_mday;
	zi.tmz_date.tm_mon = tt->tm_mon;
	zi.tmz_date.tm_year = tt->tm_year;

	int err = zip2OpenNewFileInZip(zipImpl( impl )->fw,subFName,&zi,
		NULL,0,NULL,0,NULL /*comment*/,
		(zipImpl( impl )->wCompLevel != 0) ? Z_DEFLATED : 0,
		zipImpl( impl )->wCompLevel < 0 ? Z_DEFAULT_COMPRESSION : zipImpl( impl )->wCompLevel );
	if( err != ZIP_OK )
		throw PInternalError( "ZIP: error opening subFile" );
}

void PZip::write( const BYTE* p, size_t sz )
{
	PASSERT5( zipImpl( impl )->isWrite == 1 );
	int err = zip2WriteInFileInZip( zipImpl( impl )->fw, (void*)p, sz );
	if( err != ZIP_OK )
		throw PInternalError( "ZIP: write error" );
}

void PZip::closeSubFile()
{
	PASSERT5( zipImpl( impl )->isWrite == 1 );
	int err = zip2CloseFileInZip( zipImpl( impl )->fw );
	if( err != ZIP_OK )
		throw PInternalError( "ZIP: error closing file" );
}

void PZip::close()
{
	if( zipImpl( impl )->isWrite == 1 )
	{
		int err = zip2Close( zipImpl( impl )->fw, NULL );
		if( err != ZIP_OK )
			throw PInternalError( "ZIP: error closing file" );
		zipImpl( impl )->isWrite = -1;
	}
}
#endif

PZip::~PZip()
{
	try
	{
		close();
	}
	catch (...)
	{
	}
	delete zipImpl( impl );
}
