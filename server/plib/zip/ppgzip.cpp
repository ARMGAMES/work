#include "ppzip.h"
#include "ppstring.h"
#include "pperror.h"

#include "zlib.h"
#include "gzlib2.h"

struct _PGZipImpl
{
	gzFile f;
};

static inline _PGZipImpl* _gzImpl( void* p )
{
	return (_PGZipImpl*)p;
}

PGZip::PGZip( POutputStream& stream, int level )
{
	impl = new _PGZipImpl();
	if( level > 9 )
		level = 9;
	PString mode( "wb" );
	if( level >= 0 )
		mode.append( (char)('0' + level) );
	_gzImpl( impl )->f = gz2open( &stream, 0, mode );
	if( _gzImpl( impl )->f == 0 )
		throw PInternalError( "GZIP: Unable to open file" );
}

void PGZip::write( const BYTE* p, size_t sz )
{
	int wr = gz2write( _gzImpl( impl )->f, (void*)p, sz );
	if( wr != static_cast< int >( sz ) )
		throw PInternalError( "GZIP: write error" );
}

void PGZip::close()
{
	if( _gzImpl( impl )->f )
	{
		gz2close( _gzImpl( impl )->f );
		_gzImpl( impl )->f = 0;
	}
}

PGZip::~PGZip()
{
	close();
	delete _gzImpl( impl );
}

//****************************************************************************

PUnGZip::PUnGZip( PInputStream& stream )
{
	impl = new _PGZipImpl();
	_gzImpl( impl )->f = gz2open( 0, &stream, "rb" );
	if( _gzImpl( impl )->f == 0 )
		throw PInternalError( "GZIP: Unable to open file" );
}

size_t PUnGZip::read( BYTE* p, size_t sz )
{
	int rd = gz2read( _gzImpl( impl )->f, p, sz );
	if( rd < 0 )
		throw PInternalError( "GZIP: read error" );
	return rd;
}

void PUnGZip::close()
{
	if( _gzImpl( impl )->f )
	{
		gz2close( _gzImpl( impl )->f );
		_gzImpl( impl )->f = 0;
	}
}

PUnGZip::~PUnGZip()
{
	close();
	delete _gzImpl( impl );
}
