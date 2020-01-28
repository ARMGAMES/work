#include "ppzip.h"
#include "ppstring.h"
#include "pperror.h"
#include "pplogfile.h"

#include "zlib.h"

struct _PDeflateImpl
{
	z_stream z;
	POutputStream* stream;
	BYTE buf[ 4096 ];
};

static inline _PDeflateImpl* _deflateImpl( void* p )
{
	return (_PDeflateImpl*)p;
}

PDeflate::PDeflate( POutputStream& stream, int level )
{
	if( level > 9 )
		level = 9;

	impl = new _PDeflateImpl();
	_PDeflateImpl* p = _deflateImpl( impl );
	p->stream = &stream;

	p->z.zalloc = (alloc_func)0;
	p->z.zfree = (free_func)0;
	p->z.opaque = (voidpf)0;
	p->z.next_in = Z_NULL;
	p->z.avail_in = 0;
	p->z.next_out = p->buf;
	p->z.avail_out = sizeof(p->buf);
	int zerr = deflateInit( &p->z, level );
	if( zerr != Z_OK )
		throw PInternalError( PString( "Deflate: unable to init stream; err=" ).appendInt( zerr ) );
}

PDeflate::PDeflate( POutputStream& stream, int level, int wBits, int memLevel )
//wBits MUST be in range 8 - 15
//memLevel MUST be in range 1 - 9
{
	if( level > 9 )
		level = 9;

	impl = new _PDeflateImpl();
	_PDeflateImpl* p = _deflateImpl( impl );
	p->stream = &stream;

	p->z.zalloc = (alloc_func)0;
	p->z.zfree = (free_func)0;
	p->z.opaque = (voidpf)0;
	p->z.next_in = Z_NULL;
	p->z.avail_in = 0;
	p->z.next_out = p->buf;
	p->z.avail_out = sizeof(p->buf);
	int zerr = deflateInit2( &p->z, level, Z_DEFLATED, wBits, memLevel, Z_DEFAULT_STRATEGY );
	if( zerr != Z_OK )
		throw PInternalError( PString( "Deflate: unable to init stream; err=" ).appendInt( zerr ) );
}

void PDeflate::write( const BYTE* data, size_t sz )
{
	_PDeflateImpl* p = _deflateImpl( impl );
	p->z.next_in = (BYTE*)data;
	p->z.avail_in = sz;
	for(;;)
	{
		int zerr = deflate( &p->z, Z_NO_FLUSH );
		if( zerr != Z_OK )
			throw PInternalError( PString( "Deflate: unable to deflate; err=" ).appendInt( zerr ) );

		if( p->z.avail_out == 0 )//output buffer full
		{
			p->stream->write( p->buf, sizeof(p->buf) );
			p->z.next_out = p->buf;
			p->z.avail_out = sizeof(p->buf);
		}
		//no 'else' - both conditions may happen simultaneously
		if( p->z.avail_in == 0 )//input buffer empty
			break;//forever
	}
	p->z.next_in = Z_NULL;
	p->z.avail_in = 0;
}

void PDeflate::close()
{
	_PDeflateImpl* p = _deflateImpl( impl );
	if( p->stream == 0 )
		return;

	PASSERT4( p->z.next_in == Z_NULL );
	PASSERT4( p->z.avail_in == 0 );
	for(;;)
	{
		int zerr = deflate( &p->z, Z_FINISH );
		if( zerr == Z_STREAM_END )
			break;//forever
		else if( zerr != Z_OK )
			throw PInternalError( PString( "Deflate: unable to flush; err=" ).appendInt( zerr ) );
		PASSERT4( p->z.avail_out == 0 );
		p->stream->write( p->buf, sizeof(p->buf) );
		p->z.next_out = p->buf;
		p->z.avail_out = sizeof(p->buf);
	}
	p->stream->write( p->buf, sizeof(p->buf) - p->z.avail_out );

	p->stream = 0;
}

PDeflate::~PDeflate()
{
	try
	{
		close();
	}
	catch( PError& err )
	{
		PLog( "Caught error in PDeflate::~PDeflate(): %s", err.why() );
	}

	_PDeflateImpl* p = _deflateImpl( impl );
	int zerr = deflateEnd( &p->z );
	if( zerr != Z_OK )
	{//no exception, just log
		PLog( "Deflate: error in deflateEnd: err=%d", zerr );
	}
	delete p;
}
