#ifndef ppnethost_h_included
#define ppnethost_h_included

////////////////////////////////////////////////////////////////////////////////

#include "ppincludebase.h"

#if defined(_MSC_VER) && (_MSC_VER >= 1400)
#pragma intrinsic (_byteswap_ushort, _byteswap_ulong, _byteswap_uint64)
#endif

////////////////////////////////////////////////////////////////////////////////

static inline void _writeUINT16( BYTE* p, UINT16 i )
{
#ifdef P_BIG_ENDIAN
	*( UINT16 * )p = i;
#elif defined(_MSC_VER) && (_MSC_VER >= 1400)
	*( UINT16* )p = _byteswap_ushort( i );
#else
	*p++ = ( BYTE )( i >> 8 );
	*p   = ( BYTE )i;
#endif
}


static inline UINT16 _readUINT16( const BYTE* p )
{
#ifdef P_BIG_ENDIAN
	return *( UINT16 * )p;
#elif defined(_MSC_VER) && (_MSC_VER >= 1400)
	return _byteswap_ushort( *( UINT16 const* )p );
#else
	return ((( UINT16 )p[ 0 ] ) << 8 ) |
	       ( UINT16 )p[ 1 ];
#endif
}


static inline void _writeUINT32( BYTE* p, UINT32 i )
{
#ifdef P_BIG_ENDIAN
	*( UINT32 * )p = i;
#elif defined(_MSC_VER) && (_MSC_VER >= 1400)
	*( UINT32* )p = _byteswap_ulong( i );
#else
	*p++ = ( BYTE )( i >> 24 );
	*p++ = ( BYTE )( i >> 16 );
	*p++ = ( BYTE )( i >> 8 );
	*p   = ( BYTE )i;
#endif
}


static inline UINT32 _readUINT32( const BYTE* p )
{
#ifdef P_BIG_ENDIAN
	return *( UINT32 * )p;
#elif defined(_MSC_VER) && (_MSC_VER >= 1400)
	return _byteswap_ulong( *( UINT32 const* )p );
#else
	return ((( UINT32 )p[ 0 ] ) << 24 ) |
	       ((( UINT32 )p[ 1 ] ) << 16 ) |
	       ((( UINT32 )p[ 2 ] ) <<  8 ) |
	       ( UINT32 )p[ 3 ];
#endif
}


static inline void _writeUINT64( BYTE* p, UINT64 i )
{
#ifdef P_BIG_ENDIAN
	*( UINT64 * )p = i;
#elif defined(_MSC_VER) && (_MSC_VER >= 1400)
	*( UINT64* )p = _byteswap_uint64( i );
#else
	*p++ = ( BYTE )( i >> 56 );
	*p++ = ( BYTE )( i >> 48 );
	*p++ = ( BYTE )( i >> 40 );
	*p++ = ( BYTE )( i >> 32 );
	*p++ = ( BYTE )( i >> 24 );
	*p++ = ( BYTE )( i >> 16 );
	*p++ = ( BYTE )( i >> 8 );
	*p   = ( BYTE )i;
#endif
}


static inline UINT64 _readUINT64( const BYTE* p )
{
#ifdef P_BIG_ENDIAN
	return *( UINT64 * )p;
#elif defined(_MSC_VER) && (_MSC_VER >= 1400)
	return _byteswap_uint64( *( UINT64 const* )p );
#else
	return ((( UINT64 )p[ 0 ] ) << 56 ) |
	       ((( UINT64 )p[ 1 ] ) << 48 ) |
	       ((( UINT64 )p[ 2 ] ) << 40 ) |
	       ((( UINT64 )p[ 3 ] ) << 32 ) |
	       ((( UINT64 )p[ 4 ] ) << 24 ) |
	       ((( UINT64 )p[ 5 ] ) << 16 ) |
	       ((( UINT64 )p[ 6 ] ) <<  8 ) |
	       ( UINT64 )p[ 7 ];
#endif
}

////////////////////////////////////////////////////////////////////////////////

#endif // ppnethost_h_included
