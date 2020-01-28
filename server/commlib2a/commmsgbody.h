#ifndef commmsgbody_h_included
#define commmsgbody_h_included

#include "ppinclude.h"
#include "ppblock.h"
#include "ppthread.h"
#include "pperror.h"
#include "pplogfile.h"

struct CommSrvTime;
struct CommSrvDate;
class CommMsgParser;

class CommMsgBody
{
private:
	enum { _RefCountSz = sizeof(PInterlocked) };
	enum { _Offset = _RefCountSz, _DefaultPrefix = 28 };
	enum { _DefaultReserve = 64 /* why not? */ };

public:
	enum { _NoFormatChar = 0xFF };
	struct _RefCountFlag {};//dummy struct just to provide 'RefCount' constructor argument

private:
	size_t offset;
	bool refCounted;
	_PBlock msg;

public:
	CommMsgBody();
	CommMsgBody( size_t defPrefix ); 
	CommMsgBody( /*non-const!*/ CommMsgBody& other, const _RefCountFlag& );//'other' MUST be RefCounted
	CommMsgBody( /*non-const!*/ CommMsgBody& other, bool );//"moving" constructor
	CommMsgBody( CommMsgBody&& other );//"moving" constructor

	~CommMsgBody()
	{
		_destruct();
	}

	BYTE* _alloc( size_t sz );
	BYTE* _append( size_t sz );
	void _dealloc();

	void _setRefCounted();
	void moveFrom( /*non-const!*/ CommMsgBody& other );//this MUST NOT be RefCounted
	void moveFrom( /*non-const!*/ PBlock& other );//this MUST NOT be RefCounted
	void copyFrom( const CommMsgBody& other );//this MUST NOT be RefCounted
	void copyFromSkipReqId( const CommMsgBody& other );//this MUST NOT be RefCounted

	// Prohibited to use this function without explicit approval
	void _moveFromConstBody( const CommMsgBody& other );//this MUST NOT be RefCounted

	CommMsgBody& operator=( CommMsgBody&& other )//this MUST NOT be RefCounted
	{
		if( this != &other )
		{
			moveFrom( other );
		}

		return *this;
	}

public:
	size_t _size() const
	{
		return msg.size() - offset;
	}
	const BYTE* _readPtr() const
	{
		return msg.ptr() + offset;
	}
	BYTE* _writePtr()
	{
		PASSERT3( !refCounted );
		return msg.ptr() + offset;
	}

	BYTE* _createPrefix( size_t prefixSz );
	void _skipPrefix( size_t prefixSz );
	//void _cut( size_t offset );
	void _shift( size_t offset_ );

public:
	static void writeUINT16( BYTE* p, UINT16 i );
	static UINT16 readUINT16( const BYTE* p );
	static void writeUINT32( BYTE* p, UINT32 i );
	static UINT32 readUINT32( const BYTE* p );
	static void writeUINT64( BYTE* p, UINT64 i );
	static UINT64 readUINT64( const BYTE* p );
	static void writeString( BYTE*& p, const char* s );
	static const char* readString( const BYTE*& p, const BYTE* pEnd );

public:
	void clear();
	void merge( const CommMsgBody& other );
	void mergeSkipReqId( const CommMsgBody& other );
	CommMsgBody& composeINT8( INT8 b );
	CommMsgBody& composeBYTE( BYTE b );
	CommMsgBody& composeBOOL( bool b );
	CommMsgBody& composeINT16( INT16 i );
	CommMsgBody& composeUINT16( UINT16 i );
	CommMsgBody& composeINT32( INT32 i );
	CommMsgBody& composeUINT32( UINT32 i );
	CommMsgBody& composeINT64( INT64 i );
	CommMsgBody& composeUINT64( UINT64 i );

	CommMsgBody& composeString( const char* s );
	CommMsgBody& composeMsgBody( const CommMsgBody& m );
	CommMsgBody& composeMsgBodySkipReqId( const CommMsgBody& m );
	//message body only
	CommMsgBody& _composeFixedBlock( const BYTE* ptr, size_t sz );
	//block of well-known size
	//don't use unless you exactly know what are you doing.
	//absolutely not for structure transfer over-the-network
	CommMsgBody& _composeVarBlock( const BYTE* ptr, size_t sz );
	CommMsgBody& composePBlock( const PBlock& block )
	{
		return _composeVarBlock( block.ptr(), block.size() );
	}
	//block; size of the block is also transferred
	//don't use unless you exactly know what are you doing.
	//absolutely not for structure transfer over-the-network
	CommMsgBody& composeSrvTime( const CommSrvTime& srvTime );
	CommMsgBody& composeSrvDate( const CommSrvDate& srvTime );

	template< class T >
	CommMsgBody& composeObject( const T& obj )
	{
		obj.compose( *this );
		return *this;
	}

	// Copies only the part of the parser's message that was not parsed yet,
	// as opposed to copyFrom(parser.getMsgBody()) which copies the complete message
	CommMsgBody& copyMsgBodyFromParser( const CommMsgParser& parser );
	CommMsgBody& composeVectorSize( UINT32 i, size_t & shift );
	void updateVectorSize( UINT32 i, size_t shift );

private:
	PInterlocked* _cs();
	//	UINT32* _count();
	void _throwNoSpace( unsigned int line, size_t shift ) const;

private:
	friend class CommMsgParser;
	/* inline */ void _parseINT8( size_t& shift, INT8& ret ) const;
	/* inline */ void _parseBYTE( size_t& shift, BYTE& ret ) const;
	/* inline */ void _parseINT16( size_t& shift, INT16& ret ) const;
	/* inline */ void _parseUINT16( size_t& shift, UINT16& ret ) const;
	/* inline */ void _parseINT32( size_t& shift, INT32& ret ) const;
	/* inline */ void _parseUINT32( size_t& shift, UINT32& ret ) const;
	/* inline */ void _parseINT64( size_t& shift, INT64& ret ) const;
	/* inline */ void _parseUINT64( size_t& shift, UINT64& ret ) const;
	/* inline */ void _shiftParser( size_t& shift, size_t add ) const;
	/* inline */ void _shiftBackParser( size_t& shift, size_t sub ) const;

	void _parseString( size_t& shift, const char*& ret ) const;
	void _parseMsgBody( size_t& shift, CommMsgBody& ret ) const;
	void _parseFixedBlock( size_t& shift, const BYTE*& ret, size_t sz ) const;
	void _parseVarBlock( size_t& shift, const BYTE*& ret, size_t& retSz ) const;
	size_t _skipMsgBody( size_t& shift ) const;  // return the skipped msgbody size

	void _parseStringN( size_t& shift, const char*& ret, size_t maxStringLengthAllowed, const char *fieldDescr ) const;
	void _parseMsgBodyN( size_t& shift, CommMsgBody& ret, size_t maxMsgBodySizeAllowed, const char *fieldDescr ) const;

	/* inline */ bool _parseEnded( size_t shift ) const;

private://helper
	/* inline */ void _ensureInit();
	/* inline */ BYTE* _ensureAppend( size_t sz );
	void _destruct();

private://prohibited
	CommMsgBody( const CommMsgBody& );
	CommMsgBody& operator =( const CommMsgBody& );
};

//****************************************************************************

class CommMsgParser
{
private:
	const CommMsgBody& msgBody;
	size_t shift;

public:
	CommMsgParser( const CommMsgBody& msgBody_ )
		: msgBody( msgBody_ )
	{
		shift = 0;
	}

public:
	const CommMsgBody& getMsgBody() const { return msgBody; }
	bool parseEnded() const;
	CommMsgParser& parseINT8( INT8& ret );
	CommMsgParser& parseBYTE( BYTE& ret );
	CommMsgParser& parseBOOL( bool& ret );
	CommMsgParser& parseINT16( INT16& ret );
	CommMsgParser& parseUINT16( UINT16& ret );
	CommMsgParser& parseINT32( INT32& ret );
	CommMsgParser& parseUINT32( UINT32& ret );
	CommMsgParser& parseINT64( INT64& ret );
	CommMsgParser& parseUINT64( UINT64& ret );
	CommMsgParser& parseString( const char*& ret );
	CommMsgParser& parseStringP( PString& ret );
	CommMsgParser& parseMsgBody( CommMsgBody& ret );
	CommMsgParser& _parseFixedBlock( const BYTE*& ret, size_t sz );
	CommMsgParser& _parseVarBlock( const BYTE*& ret, size_t& retSz );
	CommMsgParser& parsePBlock( PBlock & ret);
	CommMsgParser& parseSrvTime( CommSrvTime & ret);
	CommMsgParser& parseSrvDate( CommSrvDate & ret);
	CommMsgParser& skip(size_t sz);
	CommMsgParser& shiftBack(size_t sz);
	CommMsgParser& skipMsgBody();
	CommMsgParser& skipMsgBody( size_t& msgBodySizeRet );

	// in the following parsing functions the maximum length/size is checked to avoid malicious big memory consumption.
	// when the string length or msgbody size exceeds the maximum allowed value, an PParserError exception will be thrown.
	// fieldDescr is used as a hint for developer to know on which field this PParserError exception is thrown.
	CommMsgParser& parseStringN( const char*& ret, size_t maxStringLengthAllowed, const char *fieldDescr = "" );
	CommMsgParser& parseStringPN( PString& ret, size_t maxStringLengthAllowed, const char *fieldDescr = "" );
	CommMsgParser& parseMsgBodyN( CommMsgBody& ret, size_t maxMsgBodySizeAllowed, const char *fieldDescr = "" );
	CommMsgParser& parsePBlockN( PBlock & ret, size_t maxBlockSizeAllowed, const char *fieldDescr = "" );

	template< class T >
	CommMsgParser& parseObject( T& obj )
	{
		obj.parse( *this );
		return *this;
	}

public:
	size_t _shift() const
	{
		return shift;
	}

private: // prohibit default assignment operator
	CommMsgParser& operator=( const CommMsgParser& other );
};

class _CommProtocolLog
{
private:
	PCriticalSection * _cs_prot;
	PLogFile         *_logFile_prot;
	_CommProtocolLog( const _CommProtocolLog &); // copying is prohibited for this class
	_CommProtocolLog & operator = (const _CommProtocolLog &);

public:
	_CommProtocolLog(  const char* fileName, int flags_ )
	{
#ifndef PNOTHREADS
		_cs_prot = new PCriticalSection();
#endif
		_logFile_prot = new PLogFile(fileName,flags_);
	}
	~_CommProtocolLog()
	{
#ifndef PNOTHREADS
		if (_cs_prot)
			delete _cs_prot;
#endif
		if (_logFile_prot)
			delete _logFile_prot;
	}

	void _CommLog( const char* fmt, ... )
	{
		if(_logFile_prot == 0)
			return;
#ifndef PNOTHREADS
		PASSERT4( _cs_prot );
		PLock lock( *_cs_prot );
#endif
		PASSERT4( _logFile_prot );
		va_list args;
		va_start( args, fmt );
		_logFile_prot->vlog( 0, fmt, args );
		va_end( args );
	}
	void _CommLog( const char * prefix, const char *fmt, va_list args )//thread-safe
	{
		if(_logFile_prot == 0)
			return;
#ifndef PNOTHREADS
		PASSERT4( _cs_prot );
		PLock lock( *_cs_prot );
#endif
		PASSERT4( _logFile_prot );
		_logFile_prot->vlog( prefix, fmt, args );
	}
};

#endif
