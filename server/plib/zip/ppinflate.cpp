#include "ppzip.h"
#include "ppstring.h"
#include "pperror.h"
#include "pplogfile.h"

#include "zlib.h"

struct _PInflateImpl
{
	z_stream z;
	PInputStream* stream;
	BYTE buf[ 4096 ];
	bool streamEof;
	bool inflateEof;
};

static inline _PInflateImpl* _inflateImpl( void* p )
{
	return (_PInflateImpl*)p;
}

PInflate::PInflate( PInputStream& stream )
{
	impl = new _PInflateImpl();
	_PInflateImpl* p = _inflateImpl( impl );
	p->stream = &stream;
	p->streamEof = p->inflateEof = false;

	p->z.zalloc = (alloc_func)0;
	p->z.zfree = (free_func)0;
	p->z.opaque = (voidpf)0;
	p->z.next_out = Z_NULL;
	p->z.avail_out = 0;

	p->z.next_in = p->buf;
	p->z.avail_in = stream.read( p->buf, sizeof(p->buf) );
	if( p->z.avail_in == 0 )
		p->streamEof = true;
	int zerr = inflateInit( &p->z );
	if( zerr != Z_OK )
		throw PInternalError( PString( "Inflate: unable to init stream; err=" ).appendInt( zerr ) );
}

PInflate::PInflate( PInputStream& stream, int wBits )
//wBits MUST be in range 8 - 15
{
	impl = new _PInflateImpl();
	_PInflateImpl* p = _inflateImpl( impl );
	p->stream = &stream;
	p->streamEof = p->inflateEof = false;

	p->z.zalloc = (alloc_func)0;
	p->z.zfree = (free_func)0;
	p->z.opaque = (voidpf)0;
	p->z.next_out = Z_NULL;
	p->z.avail_out = 0;

	p->z.next_in = p->buf;
	p->z.avail_in = stream.read( p->buf, sizeof(p->buf) );
	if( p->z.avail_in == 0 )
		p->streamEof = true;
	int zerr = inflateInit2( &p->z, wBits );
	if( zerr != Z_OK )
		throw PInternalError( PString( "Inflate: unable to init stream; err=" ).appendInt( zerr ) );
}

size_t PInflate::read( BYTE* data, size_t sz )
{
	_PInflateImpl* p = _inflateImpl( impl );
	if( p->inflateEof )
		return 0;
	p->z.next_out = data;
	p->z.avail_out = sz;
	for(;;)
	{
		int zerr = inflate( &p->z, Z_NO_FLUSH );
		if( zerr == Z_STREAM_END )
		{
			size_t ret = sz - p->z.avail_out;
			p->z.next_out = Z_NULL;
			p->z.avail_out = 0;
			p->inflateEof = true;
			return ret;
		}
		//		else if( zerr == Z_NEED_DICT )
		//			{
		//			inflateSetDictionary( &p->z, _dict, sizeof(_dict) );
		//			continue;//forever
		//			}
		else if( zerr != Z_OK )
			throw PInternalError( PString( "Inflate: unable to inflate; err=" ).appendInt( zerr ) );

		if( p->z.avail_in == 0 )//input buffer empty
		{
			p->z.next_in = p->buf;
			if( p->streamEof )
				throw PInternalError( "Inflate: unexpected end of stream" );
			p->z.avail_in = p->stream->read( p->buf, sizeof(p->buf) );
			if( p->z.avail_in == 0 )
				p->streamEof = true;
		}
		//no 'else' - both conditions may happen simultaneously
		if( p->z.avail_out == 0 )//output buffer full
		{
			p->z.next_out = Z_NULL;
			p->z.avail_out = 0;
			return sz;
		}
	}
}

void PInflate::close()
{
	/*
	_PInflateImpl* p = _inflateImpl( impl );
	if( p->stream == 0 )
	return;
	//do nothing
	p->stream = 0;
	*/
}

PInflate::~PInflate()
{
	//	close();
	_PInflateImpl* p = _inflateImpl( impl );
	int zerr = inflateEnd( &p->z );
	if( zerr != Z_OK )
	{//no exception, just log
		PLog( "Inflate: error in inflateEnd: err=%d", zerr );
	}
	delete p;
}
