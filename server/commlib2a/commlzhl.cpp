#include "ppinclude.h"

#include "pperror.h"
#include "commlzhl.h"
#include "lzhl/_lz.h"
#include "lzhl1/_lz1.h"


/* static */ size_t CommLZHL::calcMaxBuf( size_t sz )
{
	return LZHLCompressor::calcMaxBuf( sz );
}

struct _CommLZHLImpl
{
	LZHLCompressor comp;
	LZHLDecompressor decomp;
	_CommLZHLImpl(CommStreamEncryption * encryption) :
	comp(encryption), decomp(encryption)
	{}
};


struct _CommLZHLImpl1
{
	LZHLCompressor1 comp;
	LZHLDecompressor1 decomp;
	_CommLZHLImpl1(CommStreamEncryption * encryption) :
	comp(encryption), decomp(encryption)
	{}

};


static inline _CommLZHLImpl* _impl( void* p )
{
	return (_CommLZHLImpl*)p;
}

static inline _CommLZHLImpl1* _impl1( void* p )
{
	return (_CommLZHLImpl1*)p;
}


CommLZHL::CommLZHL(int level_, CommStreamEncryption * encryption)
{
	_level=level_;
	if(level_ == 16)
		impl = new _CommLZHLImpl(encryption);
	else
		impl = new _CommLZHLImpl1(encryption);
}

size_t CommLZHL::compress( BYTE* dst, const BYTE* src, size_t srcSz )
{
	//FILE* f = fopen( "lzhlcsrc", "wb" );
	//fprintf( f, "\r\n**BLOCK** %d\r\n", srcSz );
	//fwrite( src, 1, srcSz, f );
	//fclose( f );

	size_t ret =  _level==16 ? _impl( impl )->comp.compress( dst, src, srcSz ) : _impl1( impl )->comp.compress( dst, src, srcSz );
	//FILE* f2 = fopen( "lzhlcdst", "wb" );
	//fprintf( f2, "\r\n**BLOCK** %d\r\n", ret );
	//fwrite( dst, 1, ret, f2 );
	//fclose( f2 );
	return ret;
}

size_t CommLZHL::decompress( BYTE* dst, size_t dstSz, const BYTE* src, size_t srcSz )
{
	//FILE* f = fopen( "lzhldsrc", "wb" );
	//fprintf( f, "\r\n**BLOCK** %d\r\n", srcSz );
	//fwrite( src, 1, srcSz, f );
	//fclose( f );
	size_t dstSz0 = dstSz;
	bool Ok = (_level ==16 ? _impl( impl )->decomp.decompress( dst, &dstSz, src, &srcSz ) : _impl1( impl )->decomp.decompress( dst, &dstSz, src, &srcSz )) != 0;
	if( !Ok )
		throw PInternalError( "CommLZHL: error decompressing frame" );
	size_t ret = dstSz0 - dstSz;
	//FILE* f2 = fopen( "lzhlddst", "wb" );
	//fprintf( f2, "\r\n**BLOCK** %d\r\n", ret );
	//fwrite( dst, 1, ret, f2 );
	//fclose( f2 );
	return ret;
}

CommLZHL::~CommLZHL()
{
	if(_level==16)
		delete _impl( impl );
	else
		delete _impl1( impl );
}
