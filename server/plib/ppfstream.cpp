#include "ppstream.h"
#include "ppstring.h"
#include "pperror.h"

PFileOutputStream::PFileOutputStream( FILE* f_ )
: f( f_ )
{
}

/* virtual */ void PFileOutputStream::write( const unsigned char* buf, size_t sz )
{
	size_t wr = ::fwrite( buf, 1, sz, f );
	if( wr != sz )
		throw PInternalError( "Error writing to file" );
}

/* virtual */ bool PFileOutputStream::supportSeekTell() const
{
	return true;
}

/* virtual */ void PFileOutputStream::seek( int offset, int origin )
{
	int err = fseek( f, offset, origin );
	if( err )
		throw PInternalError( "Error in fseek" );
}

/* virtual */ unsigned long PFileOutputStream::tell() const
{
	long ret = ftell( f );
	if( ret < 0 )
		throw PInternalError( "Error in ftell" );
	return ret;
}

PFileInputStream::PFileInputStream( FILE* f_ )
: f( f_ )
{
}

/* virtual */ size_t PFileInputStream::read( unsigned char* buf, size_t sz )
{
	size_t rd = ::fread( buf, 1, sz, f );
	return rd;
}

