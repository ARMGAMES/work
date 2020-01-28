#include "ppinclude.h"
#include "ppstring.h"
#include "ppnethost.h"
#include "commmsgbody.h"
#include "commtime.h"

namespace
{

size_t getAllocationSizeCheckOverflow(size_t n1, size_t n2, const char* callingFuncName)
{
    size_t allocSize = n1 + n2;
    if (allocSize < n1 || allocSize < n2)
    {
        PString excp;
        excp.append(callingFuncName).append(" - allocation size overflow");
        throw PSystemMemoryError( excp.c_str(), allocSize );
    }
    return allocSize;
}

} // unnamed namespace

inline PInterlocked* CommMsgBody::_cs()
{
	PASSERT3( refCounted );
	return (PInterlocked*)msg.ptr();
}
/*
inline UINT32* CommMsgBody::_count()
{
PASSERT3( refCounted );
return (UINT32*)( msg.ptr() + sizeof(PCriticalSection) );
}
*/
inline void CommMsgBody::_ensureInit()
{
	PASSERT3( !refCounted );
	if( offset == 0 )
	{
		offset = _Offset + _DefaultPrefix;
		msg.alloc( offset, _DefaultReserve );
	}
	else
	{
		PASSERT3( offset >= _Offset );
	}
}

inline BYTE* CommMsgBody::_ensureAppend( size_t sz_ )
{
	PASSERT3( !refCounted );
	if( offset == 0 )
	{
		offset = _Offset + _DefaultPrefix;
		msg.alloc( getAllocationSizeCheckOverflow(offset, sz_, __FUNCTION__), _DefaultReserve );
		return msg.ptr() + offset;
	}
	else
	{
		PASSERT3( offset >= _Offset );
		return msg.append( sz_ );
	}
}

CommMsgBody::CommMsgBody()
{
	offset = 0;
	refCounted = false;
}

CommMsgBody::CommMsgBody( size_t defPrefix )
	: msg( _Offset + defPrefix, _DefaultReserve )
{
	offset = _Offset + defPrefix;
	refCounted = false;
}

BYTE* CommMsgBody::_alloc( size_t sz )
{
	PASSERT3( !refCounted );
	offset = _Offset + _DefaultPrefix;
	return msg.alloc( getAllocationSizeCheckOverflow(offset, sz, __FUNCTION__) ) + offset;
}

BYTE* CommMsgBody::_append( size_t sz )
{
	return _ensureAppend( sz );
}

void CommMsgBody::_setRefCounted()
{
	PASSERT3( !refCounted );
	PASSERT3( offset >= _RefCountSz );
	refCounted = true;
#include "ppmemtracedisable.h"
	new( _cs() ) PInterlocked(1);
#include "ppmemtraceenable.h"
}

CommMsgBody::CommMsgBody( /*non-const!*/ CommMsgBody& other, const _RefCountFlag& )
{
	PASSERT3( other.refCounted );

	offset = other.offset;
	refCounted = true;
	msg._rawCopy( other.msg );
	_cs()->increment();
}

CommMsgBody::CommMsgBody( /*non-const!*/ CommMsgBody& other, bool )//"moving" constructor
: msg( other.msg, true )
{
	offset = other.offset;
	refCounted = other.refCounted;
	other.offset = 0;
	other.refCounted = false;
}

CommMsgBody::CommMsgBody( CommMsgBody&& other )//"moving" constructor
: msg( other.msg, true )
{
	offset = other.offset;
	refCounted = other.refCounted;
	other.offset = 0;
	other.refCounted = false;
}

void CommMsgBody::moveFrom( /*non-const!*/ CommMsgBody& other )
{
	PASSERT3( !refCounted );

	msg.moveFrom( other.msg );
	offset = other.offset;
	refCounted = other.refCounted;
	other.offset = 0;
	other.refCounted = false;
}

void CommMsgBody::_moveFromConstBody( const CommMsgBody& other )
{
	PASSERT3( !refCounted );

	msg.moveFrom( (PBlock &) other.msg );
	offset = other.offset;
	refCounted = other.refCounted;
	((CommMsgBody&) other).offset = 0;
	((CommMsgBody&) other).refCounted = false;
}

void CommMsgBody::moveFrom( /*non-const!*/ PBlock& other )
{
	PASSERT3( !refCounted );

	msg.moveFrom( other );
	offset = 0;
	refCounted = false;
}

void CommMsgBody::copyFrom( const CommMsgBody& other )
{
	PASSERT3( !refCounted );
	msg = other.msg;
	offset = other.offset;
}

void CommMsgBody::copyFromSkipReqId( const CommMsgBody& other )
{
	PASSERT3( !refCounted );
	PASSERT3(  other._size() >= sizeof(UINT32) );
	msg._free();
	msg._init();
	size_t sz = other._size() - sizeof(UINT32);
	BYTE* p = _alloc( sz );
	memcpy( p, other._readPtr() + sizeof(UINT32), sz ); 
}

void CommMsgBody::_destruct()
{
	if( refCounted )
	{
		if (_cs()->decrement())
		{
			(*_cs()).PInterlocked::~PInterlocked();
			msg._free();
		}
	}
	else
		msg._free();
}

void CommMsgBody::_dealloc()
{
	_destruct();
	msg._init();
	offset = 0;
	refCounted = false;
}

BYTE* CommMsgBody::_createPrefix( size_t prefixSz )
{
	_ensureInit();

	if( _Offset + prefixSz > offset )
	{
		PTRACE3("_createPrefix prefixSz=%d, offset=%d",prefixSz,offset);
		size_t delta = _Offset + prefixSz - offset;
		msg.insert( _Offset, delta );
		offset += delta;
	}
	PASSERT3( _Offset + prefixSz <= offset );
	offset -= prefixSz;
	return msg.ptr() + offset;
}

void CommMsgBody::_skipPrefix( size_t prefixSz )
{
	PASSERT3( offset >= _Offset );
	PASSERT3( offset + prefixSz <= msg.size() );
	offset += prefixSz;
}

/*
void CommMsgBody::_cut( size_t offset_ )
{
PASSERT3( !refCounted );
PASSERT3( offset + offset_ <= msg.size() );
PASSERT3( !_hasFormat );
msg.cut( offset + offset_ );
}
*/

void CommMsgBody::_shift( size_t offset_ )
{
	PASSERT3( !refCounted );
	PASSERT3( offset + offset_ <= msg.size() );
	offset += offset_;
}

//****************************************************************************

/* static */ void CommMsgBody::writeUINT16( BYTE* p, UINT16 i )
{
	_writeUINT16( p, i );
}

/* static */ UINT16 CommMsgBody::readUINT16( const BYTE* p )
{
	return _readUINT16( p );
}

/* static */ void CommMsgBody::writeUINT32( BYTE* p, UINT32 i )
{
	_writeUINT32( p, i );
}

/* static */ UINT32 CommMsgBody::readUINT32( const BYTE* p )
{
	return _readUINT32( p );
}

/* static */ void CommMsgBody::writeUINT64( BYTE* p, UINT64 i )
{
	_writeUINT64( p, i );
}

/* static */ UINT64 CommMsgBody::readUINT64( const BYTE* p )
{
	return _readUINT64( p );
}

void CommMsgBody::clear()
{
	_ensureInit();
	msg.cut( offset );
}

void CommMsgBody::merge( const CommMsgBody& other )
{
	BYTE* p = _ensureAppend( other._size() );
	memcpy( p, other._readPtr(), other._size() ); 
}

void CommMsgBody::mergeSkipReqId( const CommMsgBody& other )
{
	PASSERT3( other._size() >= sizeof(UINT32) );
	BYTE* p = _ensureAppend( other._size() - sizeof(UINT32) );
	memcpy( p, other._readPtr() + sizeof(UINT32), other._size() - sizeof(UINT32) ); 
}

CommMsgBody& CommMsgBody::composeBYTE( BYTE b )
{
	*_ensureAppend( 1 ) = b;
	return *this;
}

CommMsgBody& CommMsgBody::composeBOOL( bool b )
{
	*_ensureAppend( 1 ) = b ? 1 : 0;
	return *this;
}

CommMsgBody& CommMsgBody::composeINT8( INT8 b )
{
	*_ensureAppend( 1 ) = (BYTE)b;
	return *this;
}

CommMsgBody& CommMsgBody::composeUINT16( UINT16 i )
{
	_writeUINT16( _ensureAppend( 2 ), i );
	return *this;
}

CommMsgBody& CommMsgBody::composeINT16( INT16 i )
{
	_writeUINT16( _ensureAppend( 2 ), (UINT16)i );
	return *this;
}

CommMsgBody& CommMsgBody::composeUINT32( UINT32 i )
{
	_writeUINT32( _ensureAppend( 4 ), i );
	return *this;
}

CommMsgBody& CommMsgBody::composeVectorSize( UINT32 i, size_t & shift )
{
	_writeUINT32( _ensureAppend( 4 ), i );
	shift = msg.size() - 4;
	return *this;
}

void CommMsgBody::updateVectorSize( UINT32 i, size_t shift )
{
	PASSERT3(shift <= msg.size() + 4);
	BYTE* p = msg.ptr() + shift;
	_writeUINT32( p, i );
}

CommMsgBody& CommMsgBody::composeINT32( INT32 i )
{
	_writeUINT32( _ensureAppend( 4 ), (UINT32)i );
	return *this;
}

CommMsgBody& CommMsgBody::composeUINT64( UINT64 i )
{
	_writeUINT64( _ensureAppend( 8 ), i );
	return *this;
}

CommMsgBody& CommMsgBody::composeINT64( INT64 i )
{
	_writeUINT64( _ensureAppend( 8 ), (UINT64) i );
	return *this;
}

/* static */ void CommMsgBody::writeString( BYTE*& p, const char* s )
{
	strcpy( (char*)p, s );
	p += strlen( (char*)p ) + 1;
}

CommMsgBody& CommMsgBody::composeString( const char* s )
{
	size_t len = strlen( s );
	strcpy( (char*)_ensureAppend( len + 1 ), s );
	return *this;
}

CommMsgBody& CommMsgBody::composeMsgBody( const CommMsgBody& m )
{
	size_t sz = m._size();
	BYTE* p = _ensureAppend( 1 + 4 + sz );
	*p++ = _NoFormatChar;
	_writeUINT32( p, static_cast< UINT32 >( sz ) );
	memcpy( p + 4, m._readPtr(), sz );
	return *this;
}

CommMsgBody& CommMsgBody::composeMsgBodySkipReqId( const CommMsgBody& m )
{
	PASSERT3( m._size() >= sizeof(UINT32) );

	size_t sz = m._size() - sizeof( UINT32 );
	BYTE* p = _ensureAppend( 1 + 4 + sz );
	*p = _NoFormatChar;
	_writeUINT32( p + 1, static_cast< UINT32 >( sz ) );
	memcpy( p + 5, m._readPtr() + sizeof( UINT32 ), sz );
	return *this;
}

CommMsgBody& CommMsgBody::_composeFixedBlock( const BYTE* ptr, size_t sz )
{
	memcpy( _ensureAppend( sz ), ptr, sz );
	return *this;
}

CommMsgBody& CommMsgBody::_composeVarBlock( const BYTE* ptr, size_t sz )
{
	BYTE* p = _ensureAppend( 4 + sz );
	_writeUINT32( p, static_cast< UINT32 >( sz ) );
	memcpy( p + 4, ptr, sz );
	return *this;
}

CommMsgBody& CommMsgBody::copyMsgBodyFromParser( const CommMsgParser & parser )
{
	const CommMsgBody & other = parser.getMsgBody();
	int shift = static_cast< int >( parser._shift() );
	PASSERT3( shift <= other.msg.size() - other.offset );
	const BYTE* pStart = other._readPtr() + shift;
	const BYTE* pEnd = other.msg.ptr() + other.msg.size();
	size_t sz = pEnd - pStart;
	BYTE* p = _ensureAppend( sz );
	memcpy( p, pStart, sz ); 
	return *this;
}

CommMsgBody& CommMsgBody::composeSrvTime( const CommSrvTime& srvTime )
{
	BYTE* p = _ensureAppend( 8 );
	_writeUINT16( p, srvTime._year );
	p[2] = srvTime._month;
	p[3] = srvTime._day;
	p[4] = srvTime._hour;
	p[5] = srvTime._minute;
	p[6] = srvTime._second;
	p[7] = srvTime._wday;
	return *this;
}

CommMsgBody& CommMsgBody::composeSrvDate( const CommSrvDate& srvDate )
{
	BYTE* p = _ensureAppend( 4 );
	_writeUINT16( p, srvDate._year );
	p[2] = srvDate._month;
	p[3] = srvDate._day;
	return *this;
}

static inline const BYTE* _findZero( const BYTE* p, const BYTE* pEnd )
{
	for( const BYTE* pp = p ; pp < pEnd ; ++pp )
		if( *pp == 0 )
			return pp;
	return 0;
}

void CommMsgBody::_throwNoSpace( unsigned int line, size_t shift ) const
{
	char descr[256];
	sprintf( descr, "CommMsgBody::parse(): Insufficient space. line=%u, msg.size=%u, shift=%u, offset=%u", line, (unsigned int)msg.size(), (unsigned int)shift, (unsigned int)offset );
	throw PParserError( descr );
}

inline bool CommMsgBody::_parseEnded( size_t shift ) const
{
	if ( shift > msg.size() - offset )
		_throwNoSpace( __LINE__, shift );

	return shift == msg.size() - offset;
}

inline void CommMsgBody::_parseINT8( size_t& shift, INT8& ret ) const
{
	if ( shift > msg.size() - offset )
		_throwNoSpace( __LINE__, shift );

	if( msg.size() - offset - shift < 1 )
		_throwNoSpace( __LINE__, shift );
	ret = (INT8)*( _readPtr() + shift );
	++shift;
}

inline void CommMsgBody::_parseBYTE( size_t& shift, BYTE& ret ) const
{
	if ( shift > msg.size() - offset )
		_throwNoSpace( __LINE__, shift );

	if( msg.size() - offset - shift < 1 )
		_throwNoSpace( __LINE__, shift );
	ret = *( _readPtr() + shift );
	++shift;
}

inline void CommMsgBody::_parseINT16( size_t& shift, INT16& ret ) const
{
	if ( shift > msg.size() - offset )
		_throwNoSpace( __LINE__, shift );

	if( msg.size() - offset - shift < 2 )
		_throwNoSpace( __LINE__, shift );
	ret = (INT16)_readUINT16( _readPtr() + shift );
	shift += 2;
}

inline void CommMsgBody::_parseUINT16( size_t& shift, UINT16& ret ) const
{
	if ( shift > msg.size() - offset )
		_throwNoSpace( __LINE__, shift );

	if( msg.size() - offset - shift < 2 )
		_throwNoSpace( __LINE__, shift );
	ret = _readUINT16( _readPtr() + shift );
	shift += 2;
}

inline void CommMsgBody::_parseINT32( size_t& shift, INT32& ret ) const
{
	if ( shift > msg.size() - offset )
		_throwNoSpace( __LINE__, shift );

	if( msg.size() - offset - shift < 4 )
		_throwNoSpace( __LINE__, shift );
	ret = (INT32)_readUINT32( _readPtr() + shift );
	shift += 4;
}

inline void CommMsgBody::_parseUINT32( size_t& shift, UINT32& ret ) const
{
	if ( shift > msg.size() - offset )
		_throwNoSpace( __LINE__, shift );

	if( msg.size() - offset - shift < 4 )
		_throwNoSpace( __LINE__, shift );
	ret = _readUINT32( _readPtr() + shift );
	shift += 4;
}

inline void CommMsgBody::_parseUINT64( size_t& shift, UINT64& ret ) const
{
	if ( shift > msg.size() - offset )
		_throwNoSpace( __LINE__, shift );

	if( msg.size() - offset - shift < 8 )
		_throwNoSpace( __LINE__, shift );
	ret = _readUINT64( _readPtr() + shift );
	shift += 8;
}

inline void CommMsgBody::_parseINT64( size_t& shift, INT64& ret ) const
{
	if ( shift > msg.size() - offset )
		_throwNoSpace( __LINE__, shift );

	if( msg.size() - offset - shift < 8 )
		_throwNoSpace( __LINE__, shift );
	ret = (INT64) _readUINT64( _readPtr() + shift );
	shift += 8;
}

inline void CommMsgBody::_shiftParser( size_t& shift, size_t add ) const
{
	if ( shift > msg.size() - offset )
		_throwNoSpace( __LINE__, shift );
	if( msg.size() - offset - shift < add )
		_throwNoSpace( __LINE__, shift );
	shift += add;
}

inline void CommMsgBody::_shiftBackParser( size_t& shift, size_t sub ) const
{
	if ( shift > msg.size() - offset )
		_throwNoSpace( __LINE__, shift );
	if( shift < sub )
		_throwNoSpace( __LINE__, shift );
	shift -= sub;
}

/* static */ const char* CommMsgBody::readString( const BYTE*& p, const BYTE* pEnd )
{
	const char* ret = (const char*)p;
	const BYTE* pp = _findZero( p, pEnd );
	if( !pp )
		return 0;
	p = pp + 1;
	return ret;
}

void CommMsgBody::_parseStringN( size_t& shift, const char*& ret, size_t maxStringLengthAllowed, const char *fieldDescr ) const
{
	if(!fieldDescr || !*fieldDescr)
	{
		fieldDescr = "CommMsgBody::_parseString()";
	}

	if ( shift > msg.size() - offset )
		_throwNoSpace( __LINE__, shift );

	const BYTE* p = _readPtr() + shift;
	const BYTE* pEnd = msg.ptr() + msg.size();
	const BYTE* pp = _findZero( p, pEnd );
	if( pp == 0 )
	{
		char descr[256];
		sprintf( descr, "%.80s: String is too long. msg.size=%u, shift=%u, offset=%u", fieldDescr, (unsigned int)msg.size(), (unsigned int)shift, (unsigned int)offset );
		throw PParserError( descr );
	}
	ret = (const char*)p;
	shift = pp + 1 - _readPtr();

	ptrdiff_t len = pp - p;
	if(maxStringLengthAllowed > 0 && (size_t)len > maxStringLengthAllowed)
	{
		char descr[256];
		sprintf( descr, "%.80s: String is too long: strlen=%u, maxlen=%u (msg.size=%u, shift=%u, offset=%u)", fieldDescr, (unsigned int)len, (unsigned int)maxStringLengthAllowed, (unsigned int)msg.size(), (unsigned int)shift, (unsigned int)offset );
		throw PParserError( descr );
	}
}

void CommMsgBody::_parseString( size_t& shift, const char*& ret ) const
{
	_parseStringN(shift, ret, 0, "");
}

void CommMsgBody::_parseMsgBodyN( size_t& shift, CommMsgBody& ret, size_t maxMsgBodySizeAllowed, const char *fieldDescr ) const
{
	if(!fieldDescr || !*fieldDescr)
	{
		fieldDescr = "CommMsgBody::_parseMsgBody()";
	}

	if ( shift > msg.size() - offset )
		_throwNoSpace( __LINE__, shift );

	const BYTE* p = _readPtr() + shift;
	const BYTE* pEnd = msg.ptr() + msg.size();

	if( pEnd - p < 1 )
		_throwNoSpace( __LINE__, shift );
	if( *p != _NoFormatChar )
	{
		char descr[256];
		sprintf( descr, "%.80s: _NoFormatChar %u msg.size=%u, shift=%u, offset=%u", fieldDescr, *p, (unsigned int)msg.size(), (unsigned int)shift, (unsigned int)offset );
		throw PParserError( descr );
	}
	++p;
	if( pEnd - p < 4 )
		_throwNoSpace( __LINE__, shift );
	size_t sz = _readUINT32( p );
	p += 4;
	if( pEnd - p < static_cast< ptrdiff_t >( sz ) )
		_throwNoSpace( __LINE__, shift );

	if( maxMsgBodySizeAllowed > 0 && sz > maxMsgBodySizeAllowed )
	{
		char descr[256];
		sprintf( descr, "%.80s: msgbody is too big: sz=%u, maxsize=%u (msg.size=%u, shift=%u, offset=%u)", fieldDescr, (unsigned int)sz, (unsigned int)maxMsgBodySizeAllowed, (unsigned int)msg.size(), (unsigned int)shift, (unsigned int)offset );
		throw PParserError( descr );
	}

	memcpy( ret._alloc( sz ), p, sz );
	shift = p + sz - _readPtr();
}

void CommMsgBody::_parseMsgBody( size_t& shift, CommMsgBody& ret ) const
{
	_parseMsgBodyN(shift, ret, 0, "");
}

size_t CommMsgBody::_skipMsgBody( size_t& shift ) const  // return the skipped msgbody size
{
	if ( shift > msg.size() - offset )
		_throwNoSpace( __LINE__, shift );

	const BYTE* p = _readPtr() + shift;
	const BYTE* pEnd = msg.ptr() + msg.size();

	if( pEnd - p < 1 )
		_throwNoSpace( __LINE__, shift );
	if( *p != _NoFormatChar )
	{
		char descr[256];
		sprintf( descr, "CommMsgBody::_skipMsgBody(): _NoFormatChar %u msg.size=%u, shift=%u, offset=%u", *p, (unsigned int)msg.size(), (unsigned int)shift, (unsigned int)offset );
		throw PParserError( descr );
	}
	++p;
	if( pEnd - p < 4 )
		_throwNoSpace( __LINE__, shift );
	size_t sz = _readUINT32( p );
	p += 4;
	if( pEnd - p < static_cast< ptrdiff_t >( sz ) )
		_throwNoSpace( __LINE__, shift );
	shift = p + sz - _readPtr();

	return sz;
}


void CommMsgBody::_parseFixedBlock( size_t& shift, const BYTE*& ret, size_t sz ) const
{
	if ( shift > msg.size() - offset )
		_throwNoSpace( __LINE__, shift );

	if( msg.size() - offset - shift < sz )
		_throwNoSpace( __LINE__, shift );
	ret = _readPtr() + shift;
	shift += sz;
}

void CommMsgBody::_parseVarBlock( size_t& shift, const BYTE*& ret, size_t& retSz ) const
{
	if ( shift > msg.size() - offset )
		_throwNoSpace( __LINE__, shift );

	const BYTE* p = _readPtr() + shift;
	const BYTE* pEnd = msg.ptr() + msg.size();

	if( pEnd - p < 4 )
		_throwNoSpace( __LINE__, shift );
	retSz = _readUINT32( p );
	p += 4;
	if( pEnd - p < static_cast< ptrdiff_t >( retSz ) )
		_throwNoSpace( __LINE__, shift );
	ret = p;
	shift = p + retSz - _readPtr();
}


//****************************************************************************

bool CommMsgParser::parseEnded() const
{
	return msgBody._parseEnded( shift );
}

CommMsgParser& CommMsgParser::parseINT8( INT8& ret )
{
	msgBody._parseINT8( shift, ret );
	return *this;
}

CommMsgParser& CommMsgParser::parseBYTE( BYTE& ret )
{
	msgBody._parseBYTE( shift, ret );
	return *this;
}

CommMsgParser& CommMsgParser::parseBOOL( bool& ret )
{
	BYTE ret_;
	msgBody._parseBYTE( shift, ret_ );
	if ( !(ret_ == 1 || ret_ == 0) )
	{
		char descr[256];
		sprintf( descr, "CommMsgParser::parseBOOL( %u ): msg.size=%u, shift=%u, offset=%u", ret_, (unsigned int)msgBody.msg.size(), (unsigned int)shift, (unsigned int)msgBody.offset );
		throw PParserError( descr );
	}
	ret = !!ret_;
	return *this;
}

CommMsgParser& CommMsgParser::parseINT16( INT16& ret )
{
	msgBody._parseINT16( shift, ret );
	return *this;
}

CommMsgParser& CommMsgParser::parseUINT16( UINT16& ret )
{
	msgBody._parseUINT16( shift, ret );
	return *this;
}

CommMsgParser& CommMsgParser::parseINT32( INT32& ret )
{
	msgBody._parseINT32( shift, ret );
	return *this;
}

CommMsgParser& CommMsgParser::parseUINT32( UINT32& ret )
{
	msgBody._parseUINT32( shift, ret );
	return *this;
}

CommMsgParser& CommMsgParser::parseUINT64( UINT64& ret )
{
	msgBody._parseUINT64( shift, ret );
	return *this;
}

CommMsgParser& CommMsgParser::parseINT64( INT64& ret )
{
	msgBody._parseINT64( shift, ret );
	return *this;
}

CommMsgParser& CommMsgParser::parseString( const char*& ret )
{
	msgBody._parseString( shift, ret );
	return *this;
}

CommMsgParser& CommMsgParser::parseStringN( const char*& ret, size_t maxStringLengthAllowed, const char *fieldDescr )
{
	msgBody._parseStringN( shift, ret, maxStringLengthAllowed, fieldDescr );
	return *this;
}

CommMsgParser& CommMsgParser::parseStringP( PString & ret )
{
	const char * s;
	parseString(s);
	ret.assign(s);
	return *this;
}

CommMsgParser& CommMsgParser::parseStringPN( PString& ret, size_t maxStringLengthAllowed, const char *fieldDescr )
{
	const char * s;
	parseStringN( s, maxStringLengthAllowed, fieldDescr );
	ret.assign(s);
	return *this;
}

CommMsgParser& CommMsgParser::parseMsgBody( CommMsgBody& ret )
{
	msgBody._parseMsgBody( shift, ret );
	return *this;
}

CommMsgParser& CommMsgParser::parseMsgBodyN( CommMsgBody& ret, size_t maxMsgBodySizeAllowed, const char *fieldDescr )
{
	msgBody._parseMsgBodyN( shift, ret, maxMsgBodySizeAllowed, fieldDescr );
	return *this;
}

CommMsgParser& CommMsgParser::_parseFixedBlock( const BYTE*& ret, size_t sz )
{
	msgBody._parseFixedBlock( shift, ret, sz );
	return *this;
}

CommMsgParser& CommMsgParser::_parseVarBlock( const BYTE*& ret, size_t& retSz )
{
	msgBody._parseVarBlock( shift, ret, retSz );
	return *this;
}

CommMsgParser& CommMsgParser::parsePBlock( PBlock & ret )
{
	return parsePBlockN( ret, 0, "" );
}

CommMsgParser& CommMsgParser::parsePBlockN( PBlock & ret, size_t maxBlockSizeAllowed, const char *fieldDescr )
{
	if(!fieldDescr || !*fieldDescr)
	{
		fieldDescr = "CommMsgParser::parsePBlock()";
	}

	const BYTE * p;
	size_t size;
	msgBody._parseVarBlock( shift, p, size );

	if( maxBlockSizeAllowed > 0 && size > maxBlockSizeAllowed )
	{
		char descr[256];
		sprintf( descr, "%.80s: block is too big: sz=%u, maxsize=%u (msg.size=%u, shift=%u, offset=%u)", fieldDescr, (unsigned int)size, (unsigned int)maxBlockSizeAllowed, (unsigned int)msgBody.msg.size(), (unsigned int)shift, (unsigned int)msgBody.offset );
		throw PParserError( descr );
	}

	ret.clear();
	if (size > 0)
		memcpy( ret.alloc( size ), p, size );
	return *this;
}

CommMsgParser& CommMsgParser::parseSrvTime( CommSrvTime & ret )
{
	msgBody._parseUINT16( shift, ret._year );
	msgBody._parseBYTE( shift, ret._month );
	msgBody._parseBYTE( shift, ret._day );
	msgBody._parseBYTE( shift, ret._hour );
	msgBody._parseBYTE( shift, ret._minute );
	msgBody._parseBYTE( shift, ret._second );
	msgBody._parseBYTE( shift, ret._wday );
	return *this;
}

CommMsgParser& CommMsgParser::parseSrvDate( CommSrvDate & ret )
{
	msgBody._parseUINT16( shift, ret._year );
	msgBody._parseBYTE( shift, ret._month );
	msgBody._parseBYTE( shift, ret._day );
	return *this;
}

CommMsgParser& CommMsgParser::skip(size_t sz)
{
	msgBody._shiftParser( shift, sz );
	return *this;
}

CommMsgParser& CommMsgParser::shiftBack(size_t sz)
{
	msgBody._shiftBackParser( shift, sz );
	return *this;
}


CommMsgParser& CommMsgParser::skipMsgBody()
{
	msgBody._skipMsgBody( shift );
	return *this;
}

CommMsgParser& CommMsgParser::skipMsgBody( size_t& msgBodySizeRet )
{
	msgBodySizeRet = msgBody._skipMsgBody( shift );
	return *this;
}
