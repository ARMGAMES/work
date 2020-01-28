/*
*  LZH-Light algorithm implementation v 1.0
*  Copyright (C) Sergey Ignatchenko 1998
*  This  software  is  provided  "as is"  without  express  or implied 
*  warranty.
*
*  Permission to use, copy, modify, distribute and sell this  software 
*  for any purpose is hereby  granted  without  fee,  subject  to  the 
*  following restrictions:
*  1. this notice may not be removed or altered;
*  2. altered source versions must be plainly marked as such, and must 
*     not be misrepresented as being the original software.
*
*/

#ifndef ___lz1_h
#define ___lz1_h

#include "_lzhl1.h"
#include "_huff1.h"

#define LZPOS UINT32
#define LZBUFMASK1 ( (LZBUFSIZE1) - 1 )

#define LZTABLEINT UINT16
typedef LZTABLEINT LZTableItem;

#define LZHASH UINT32

//*****************************************************************************

class LZBuffer1
{
protected:
	BYTE* buf;
	LZPOS bufPos;

protected:
	inline LZBuffer1();
	inline ~LZBuffer1();

protected:
	inline static int _wrap( LZPOS pos );
	inline static int _distance( int diff );

	inline void _toBuf( BYTE );
	inline void _toBuf( const BYTE*, size_t sz );
	inline void _bufCpy( BYTE* dst, int pos, size_t sz );
	inline int _nMatch( int pos, const BYTE* p, int nLimit );
};

//*****************************************************************************

class LZHLCompressor1 : private LZBuffer1
{
private:
	LZHLEncoderStat1 stat;
	LZTableItem* table;
	CommStreamEncryption * encryption;

public:
	LZHLCompressor1(CommStreamEncryption * encryption_);
	~LZHLCompressor1();

public:
	static size_t calcMaxBuf( size_t rawSz )
	{
		return LZHLEncoder1::calcMaxBuf( rawSz );
	}
	size_t compress( BYTE* dst, const BYTE* src, size_t sz );

private:
	void _wrapTable();
	LZHASH _updateTable( LZHASH hash, const BYTE* src, LZPOS pos, int len );
};

class LZHLDecompressor1 : private LZBuffer1, private LZHLDecoderStat1
{
private:
	UINT32 bits;
	int nBits;
	CommStreamEncryption * encryption;

public:
	LZHLDecompressor1(CommStreamEncryption * encryption_);
	~LZHLDecompressor1();
	BOOL decompress( BYTE* dst, size_t* dstSz, const BYTE* src, size_t* srcSz );

private:
	inline int _get( const BYTE*& src, const BYTE* srcEnd, int n );
};

//*****************************************************************************

inline LZBuffer1::LZBuffer1()
{
	buf= new BYTE[ LZBUFSIZE1 ];
	bufPos = 0;
}

inline LZBuffer1::~LZBuffer1()
{
	delete [] buf;
}

inline /* static */ int LZBuffer1::_wrap( LZPOS pos )
{
	return pos & LZBUFMASK1;
}

inline /* static */ int LZBuffer1::_distance( int diff )
{
	return diff & LZBUFMASK1;
}

inline void LZBuffer1::_toBuf( BYTE c )
{
	buf[ _wrap( bufPos++ ) ] = c;
}

inline void LZBuffer1::_toBuf( const BYTE* src, size_t sz )
{
	assert( sz < LZBUFSIZE1 );
	if (!(sz < LZBUFSIZE1))
		throw PInternalError( "Encoder");

	int begin = _wrap( bufPos );
	int end = begin + sz;
	if( end > LZBUFSIZE1 )
	{
		size_t left = LZBUFSIZE1 - begin;
		memcpy( buf + begin, src, left );
		memcpy( buf, src + left, sz - left );
	}
	else
		memcpy( buf + begin, src, sz );
	bufPos += sz;
}

inline void LZBuffer1::_bufCpy( BYTE* dst, int pos, size_t sz )
{
	assert( sz < LZBUFSIZE1 );
	if (!(sz < LZBUFSIZE1))
		throw PInternalError( "Encoder");

	int begin = _wrap( pos );
	int end = begin + sz;
	if( end > LZBUFSIZE1 )
	{
		size_t left = LZBUFSIZE1 - begin;
		memcpy( dst, buf + begin, left );
		memcpy( dst + left, buf, sz - left );
	}
	else
		memcpy( dst, buf + begin, sz );
}

#endif
