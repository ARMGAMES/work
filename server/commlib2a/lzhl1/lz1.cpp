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
*/
#include <pperror.h>
#include "../commstreamencryption.h"
#include "_huff1.h"
#include "_lz1.h"

#ifdef LZSLOWHASH
#define LZHASHSHIFT 5
#define UPDATE_HASH( hash, c )          \
	{                                   \
	hash ^= (c);                        \
	hash = ROTL( hash, LZHASHSHIFT );   \
	}
#define UPDATE_HASH_EX( hash, src )                     \
	{                                                   \
	hash ^= ROTL( (src)[ 0 ], LZHASHSHIFT * LZMATCH );  \
	hash ^= (src)[ LZMATCH ];                           \
	hash = ROTL( hash, LZHASHSHIFT );                   \
	}

#define HASH_POS(hash) ((( (hash) * 214013 + 2531011) >> (32-LZTABLEBITS1)) )

#else

#define LZHASHSHIFT (((LZTABLEBITS1)+(LZMATCH)-1)/(LZMATCH))

#define UPDATE_HASH( hash, c ) { hash = ( hash << LZHASHSHIFT ) ^ (c); }
#define UPDATE_HASH_EX( hash, src )  { hash = ( hash << LZHASHSHIFT ) ^ (src)[ LZMATCH ]; }
#define LZHASHMASK1 ((LZTABLESIZE1)-1)
#define HASH_POS( hash ) ( (hash) & LZHASHMASK1 )
#endif

static inline LZHASH _calcHash( const BYTE* src )
{
	LZHASH hash = 0;
	const BYTE* pEnd = src + LZMATCH;
	for( const BYTE* p=src; p < pEnd ; )
	{
		UPDATE_HASH( hash, *p++ );
	}
	return hash;
}

//*****************************************************************************

inline int LZBuffer1::_nMatch( int pos /*already wrapped*/, const BYTE* p, int nLimit )
{
	assert( nLimit < LZBUFSIZE1 );
	if (!(nLimit < LZBUFSIZE1))
		throw PInternalError( "Encoder");

	int begin = pos;
	if( LZBUFSIZE1 - begin >= nLimit )
	{
		for( int i=0; i < nLimit ; ++i )
			if( buf[ begin + i ] != p[ i ] )
				return i;
		return nLimit;
	}
	else
	{
		int i;
		for( i=begin; i < LZBUFSIZE1 ; ++i )
			if( buf[ i ] != p[ i - begin ] )
				return i - begin;
		int shift = LZBUFSIZE1 - begin;
		int n = nLimit - ( LZBUFSIZE1 - begin );
		for( i=0; i < n ; ++i )
			if( buf[ i ] != p[ shift + i ] )
				return shift + i;
		return nLimit;
	}
}

//*****************************************************************************

LZHLCompressor1::LZHLCompressor1(CommStreamEncryption * encryption_)
{
	encryption = encryption_;
	table = new LZTableItem[ LZTABLESIZE1 ];
	for( int i=0; i < LZTABLESIZE1 ; ++i )
		table[ i ] = (LZTABLEINT)(-1);
}

LZHLCompressor1::~LZHLCompressor1()
{
	delete [] table;
}

inline LZHASH LZHLCompressor1::_updateTable( LZHASH hash, const BYTE* src, LZPOS pos, int len )
{
	if( len <= 0 )
		return 0;

	if( len > LZSKIPHASH )
	{
		++src;
		hash = 0;
		const BYTE* pEnd = src + len + LZMATCH;
		for( const BYTE* p=src+len; p < pEnd ; )
		{
			UPDATE_HASH( hash, *p++ );
		}
		return hash;
	}

	UPDATE_HASH_EX( hash, src );
	++src;

	for( int i=0; i < len ; ++i )
	{
		table[ HASH_POS( hash ) ] = (LZTableItem)_wrap( pos + i );
		UPDATE_HASH_EX( hash, src + i );
	}

	return hash;
}

size_t LZHLCompressor1::compress( BYTE* dst, const BYTE* src, size_t sz )
{
	LZHLEncoder1 coder( &stat, dst, encryption );
	const BYTE* srcEnd = src + sz;

	LZHASH hash = 0;
	if( sz >= LZMATCH )
	{
		const BYTE* pEnd = src + LZMATCH;
		for( const BYTE* p=src; p < pEnd ; )
		{
			UPDATE_HASH( hash, *p++ );
		}
	}

	for(;;)
	{
		int srcLeft = srcEnd - src;
		if( srcLeft < LZMATCH )
		{
			if( srcLeft )
			{
				_toBuf( src, srcLeft );
				coder.putRaw( src, srcLeft );
				src += srcLeft;
			}
			break;//forever
		}

		int nRaw = 0;
		int maxRaw = min( srcLeft - LZMATCH, LZHLEncoder1::maxRaw );
#ifdef LZLAZYMATCH
		int    lazyMatchLen = 0;
		int    lazyMatchHashPos;
		LZPOS  lazyMatchBufPos;
		int    lazyMatchNRaw;
		LZHASH lazyMatchHash;
		BOOL   lazyForceMatch = FALSE;//needed only for LZBACKWARDMATCH
#endif
		for(;;)
		{
			LZHASH hash2 = HASH_POS( hash );

			int hashPos = table[ hash2 ];
			int wrapBufPos = _wrap( bufPos );
			table[ hash2 ] = (LZTableItem)wrapBufPos;

			int matchLen = 0;
			if( hashPos != (LZTABLEINT)(-1) && hashPos != wrapBufPos )
			{
				int matchLimit = min( min( _distance( wrapBufPos - hashPos ), srcLeft - nRaw ), LZMIN + LZHLEncoder1::maxMatchOver );
				matchLen = _nMatch( hashPos, src + nRaw, matchLimit );

#ifdef LZOVERLAP
				if( _wrap( hashPos + matchLen ) == wrapBufPos )
				{
					assert( matchLen != 0 );
					if (!(matchLen != 0))
						throw PInternalError( "Encoder");

					int xtraMatchLimit = min( LZMIN + LZHLEncoder1::maxMatchOver - matchLen, srcLeft - nRaw - matchLen );
					int xtraMatch;
					for( xtraMatch = 0; xtraMatch < xtraMatchLimit ; ++xtraMatch )
					{
						if( src[ nRaw + xtraMatch ] != src[ nRaw + xtraMatch + matchLen ] )
							break;//for( xtraMatch )
					}
					matchLen += xtraMatch;
				}
#endif

#ifdef LZBACKWARDMATCH
				if( matchLen >= LZMIN - 1 )//to ensure that buf will be overwritten
				{
					int xtraMatchLimit = min( LZMIN + LZHLEncoder::maxMatchOver - matchLen, nRaw );
					int d = (int)_distance( bufPos - hashPos );
					xtraMatchLimit = min( min( xtraMatchLimit, d - matchLen ), LZBUFSIZE1 - d );
					for( int xtraMatch = 0; xtraMatch < xtraMatchLimit ; ++xtraMatch )
					{
						if( buf[ _wrap( hashPos - xtraMatch - 1 ) ] != src[ nRaw - xtraMatch - 1 ] )
							break;//for( xtraMatch )
					}
					if( xtraMatch > 0 )
					{
						assert( matchLen + xtraMatch >= LZMIN );
						if (!(matchLen + xtraMatch >= LZMIN))
							throw PInternalError( "Encoder");

						assert( matchLen + xtraMatch <= _distance( bufPos - hashPos ) );
						if (!(matchLen + xtraMatch <= _distance( bufPos - hashPos )))
							throw PInternalError( "Encoder");


						nRaw -= xtraMatch;
						bufPos -= xtraMatch;
						hashPos -= xtraMatch;
						matchLen += xtraMatch;
						wrapBufPos = _wrap( bufPos );
						hash = _calcHash( src + nRaw );

#ifdef LZLAZYMATCH
						lazyForceMatch = TRUE;
#endif
					}
				}
#endif
			}

#ifdef LZLAZYMATCH
			if( lazyMatchLen >= LZMIN )
			{
				if( matchLen > lazyMatchLen )
				{
					coder.putMatch( src, nRaw, matchLen - LZMIN, _distance( wrapBufPos - hashPos ) );
					hash = _updateTable( hash, src + nRaw, bufPos + 1, min( matchLen - 1, srcEnd - (src + nRaw + 1) ) );
					_toBuf( src + nRaw, matchLen );
					src += nRaw + matchLen;
					break;//for( nRaw )
				}
				else
				{
					nRaw = lazyMatchNRaw;
					bufPos = lazyMatchBufPos;

					hash = lazyMatchHash;
					UPDATE_HASH_EX( hash, src + nRaw );

					coder.putMatch( src, nRaw, lazyMatchLen - LZMIN, _distance( bufPos - lazyMatchHashPos ) );
					hash = _updateTable( hash, src + nRaw + 1, bufPos + 2, min( lazyMatchLen - 2, srcEnd - (src + nRaw + 2) ) );
					_toBuf( src + nRaw, lazyMatchLen );
					src += nRaw + lazyMatchLen;
					break;//for( nRaw )
				}
			}
#endif

			if( matchLen >= LZMIN )
			{
#ifdef LZLAZYMATCH
				if( !lazyForceMatch )
				{
					lazyMatchLen = matchLen;
					lazyMatchHashPos = hashPos;
					lazyMatchNRaw = nRaw;
					lazyMatchBufPos = bufPos;
					lazyMatchHash = hash;
				}
				else
#endif
				{
					coder.putMatch( src, nRaw, matchLen - LZMIN, _distance( wrapBufPos - hashPos ) );
					hash = _updateTable( hash, src + nRaw, bufPos + 1, min( matchLen - 1, srcEnd - (src + nRaw + 1) ) );
					_toBuf( src + nRaw, matchLen );
					src += nRaw + matchLen;
					break;//for( nRaw )
				}
			}

#ifdef LZLAZYMATCH
			assert( !lazyForceMatch );
			if (lazyForceMatch)
				throw PInternalError( "Encoder");

#endif

			if( nRaw + 1 > maxRaw )
			{
#ifdef LZLAZYMATCH
				if( lazyMatchLen >= LZMIN )
				{
					coder.putMatch( src, nRaw, lazyMatchLen - LZMIN, _distance( bufPos - lazyMatchHashPos ) );
					hash = _updateTable( hash, src + nRaw, bufPos + 1, min( lazyMatchLen - 1, srcEnd - (src + nRaw + 1) ) );
					_toBuf( src + nRaw, lazyMatchLen );
					src += nRaw + lazyMatchLen;
					break;//for( nRaw )
				}
#endif

				if( nRaw + LZMATCH >= srcLeft && srcLeft <= LZHLEncoder1::maxRaw )
				{
					_toBuf( src + nRaw, srcLeft - nRaw );
					nRaw = srcLeft;
				}

				coder.putRaw( src, nRaw );
				src += nRaw;
				break;//for( nRaw )
			}

			UPDATE_HASH_EX( hash, src + nRaw );
			_toBuf( src[ nRaw++ ] );
		}//for( nRaw )
	}//forever

	return coder.flush();
}
