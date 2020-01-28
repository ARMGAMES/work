#ifndef _commcomp_h_included
#define _commcomp_h_included

#include "ppinclude.h"
#include "ppstring.h"
#include "pplogfile.h"

#include "commlzhl.h"
#include "_commcomm.h"
#include "commstreamencryption.h"

//#define _COMMLZHLBUF 2048 //MUST be <= 65535

template< class _Communicator >
class _CommCompressedCommunicator : public _Communicator
{
private:
	int mode;//compression*
	CommLZHL* lzhl;

	size_t _BuffSize;
	size_t compSz;
	BYTE* wrBuf;
	size_t wrBufOffset;
	BYTE* wrBufComp;
	size_t wrBufCompSz;
	size_t wrBufCompOffset;
	const BYTE* wrOnHold;
	size_t wrOnHoldSz;
	BYTE* rdBuf;
	size_t rdBufOffset;
	size_t rdBufSz;
	BYTE* rdBufDecomp;
	size_t rdBufDecompOffset;
	size_t rdBufDecompSz;
	BYTE* rdOnHold;
	size_t rdOnHoldSz;
	bool  flushDelayed;
	UINT32 _countReceivedBytes;
	UINT32 _countSentBytes;
	UINT32 _packets;
	CommStreamEncryption * encryption;
	UINT32 sizeLength;

public:
	_CommCompressedCommunicator();
	~_CommCompressedCommunicator();
	void init();

	static bool hasCompression( int type ) { return type == _CommCompressedCommunicatorTypes::compressionLzhl; }
	void setCompression( int mode_,size_t BuffSize);//_CommCompressedCommunicatorTypes::compression*
	void setEncryption( CommStreamEncryption * encryption_ );
	bool initConnection(UINT32 & checkSumm )
	{
		return _Communicator::initConnection(checkSumm);
	}
	bool generateRandom(UINT32 * secureRandom, int size)
	{
		return _Communicator::generateRandom( secureRandom,  size);
	}
	void getStatistics(UINT32 & readBytes,UINT32 & writeBytes,UINT32 &packets)
	{
		readBytes = _countReceivedBytes;
		writeBytes = _countSentBytes;
		packets = _packets;
		_packets=0;
		_countReceivedBytes = 0;
		_countSentBytes = 0;
	}
public:
	size_t writeIncomplete( const BYTE* p, size_t sz );
	void flush();
	_CommAsyncRet asyncWriteOk();
	size_t readIncomplete( BYTE* p, size_t sz );
	_CommAsyncRet asyncReadOk();

private://helpers
	void _asyncOk( _CommAsyncRet& ret );
	size_t _write( const BYTE* p, size_t sz );
	size_t _read( BYTE* p, size_t sz );
};

template< class _Communicator >
_CommCompressedCommunicator< _Communicator >::_CommCompressedCommunicator()
{
	mode = _CommCompressedCommunicatorTypes::compressionNone;
	lzhl = 0;
	wrBuf = 0;
	wrBufComp = 0;
	rdBuf = 0;
	rdBufDecomp = 0;
	_countReceivedBytes = 0;
	_countSentBytes = 0;
	_packets = 0;
	encryption = 0;
	sizeLength = 2;
}

template< class _Communicator >
_CommCompressedCommunicator< _Communicator >::~_CommCompressedCommunicator()
{
	delete lzhl;
	delete [] wrBuf;
	delete [] wrBufComp;
	delete [] rdBuf;
	delete [] rdBufDecomp;
}

template< class _Communicator >
void _CommCompressedCommunicator< _Communicator >::init()
{
	mode = _CommCompressedCommunicatorTypes::compressionNone;
	delete lzhl;
	lzhl = 0;
	delete [] wrBuf;
	wrBuf = 0;
	delete [] wrBufComp;
	wrBufComp = 0;
	delete [] rdBuf;
	rdBuf = 0;
	delete [] rdBufDecomp;
	rdBufDecomp = 0;
	_countReceivedBytes = 0;
	_countSentBytes = 0;
	_packets = 0;
	encryption = 0;
	sizeLength = 2;
}
template< class _Communicator >
void _CommCompressedCommunicator< _Communicator >::setEncryption( CommStreamEncryption * encryption_ )
{
	encryption = encryption_;
}

template< class _Communicator >
void _CommCompressedCommunicator< _Communicator >::setCompression( int mode_,size_t BuffSize)
{
	if (!( mode == _CommCompressedCommunicatorTypes::compressionNone   ))
		throw PInternalError( "Compression");

	switch( mode_ )
	{
	case _CommCompressedCommunicatorTypes::compressionLzhl1:
	case _CommCompressedCommunicatorTypes::compressionLzhl:
		{
			sizeLength =  BuffSize > 65535  ? 4 : 2;
			_BuffSize = BuffSize;
			mode = _CommCompressedCommunicatorTypes::compressionLzhl;
			lzhl = new CommLZHL((mode_ == _CommCompressedCommunicatorTypes::compressionLzhl1 ? 14 :16), encryption);
			compSz = CommLZHL::calcMaxBuf( BuffSize );
			wrBuf = new BYTE[ BuffSize + 5 ];//+5 - temp fix
			//wrBuf = new BYTE[ _COMMLZHLBUF ];
			wrBufComp = new BYTE[ sizeLength + compSz ];
			rdBuf = new BYTE[ BuffSize ];
			rdBufDecomp = new BYTE[ sizeLength + compSz ];
			wrBufOffset = 0;
			wrBufCompOffset = 0;
			wrBufCompSz = 0;
			wrOnHold = 0;
			wrOnHoldSz = 0;
			rdBufOffset = 0;
			rdBufSz = 0;
			rdBufDecompOffset = 0;
			rdBufDecompSz = 0;
			rdOnHold = 0;
			rdOnHoldSz = 0;
			flushDelayed = false;
		}
		break;
	}
}

template< class _Communicator >
size_t _CommCompressedCommunicator< _Communicator >::_write( const BYTE* p, size_t sz )
{
	size_t ret = 0;
	for(;;)
	{
		size_t wr = _Communicator::writeIncomplete( p, sz );
		_packets++;
		_countSentBytes += wr;
		if( wr == 0 )
			return ret;
		ret += wr;
		sz -= wr;
		p += wr;
		if( sz == 0 )
			return ret;
	}
}

template< class _Communicator >
size_t _CommCompressedCommunicator< _Communicator >::_read( BYTE* p, size_t sz )
{
	size_t ret = 0;
	for(;;)
	{
		size_t rd = _Communicator::readIncomplete( p, sz );
		_countReceivedBytes += rd;
		if( rd == 0 )
			return ret;
		ret += rd;
		sz -= rd;
		p += rd;
		if( sz == 0 )
			return ret;
	}
}

template< class _Communicator >
size_t _CommCompressedCommunicator< _Communicator >::writeIncomplete( const BYTE* p, size_t sz )
{
	//PTRACE( "WRITE" );
	if (!(sz != 0))
		throw PInternalError( "Compression");
	switch( mode )
	{
	case _CommCompressedCommunicatorTypes::compressionNone:
	default:
		{
			size_t wr =  _Communicator::writeIncomplete( p, sz );
			_countSentBytes += wr;
			_packets++;
			return wr;
		}

	case _CommCompressedCommunicatorTypes::compressionLzhl:
		{
			if (!(wrOnHold == 0))
				throw PInternalError( "Compression");

			if( wrBufCompSz != 0 )//smth pending
			{
				if (!(wrBufOffset == 0))
					throw PInternalError( "Compression");

				wrOnHold = p;
				wrOnHoldSz = sz;
				return 0;
			}

			if (!(wrBufOffset < _BuffSize))
				throw PInternalError( "Compression");

			size_t len = sz;
			if( len > _BuffSize - wrBufOffset )
				len = _BuffSize - wrBufOffset;
			memcpy( wrBuf + wrBufOffset, p, len );
			wrBufOffset += len;
			if (!(wrBufOffset <= _BuffSize))
				throw PInternalError( "Compression");

			if( wrBufOffset == _BuffSize )
			{
				if (!(wrBufCompSz == 0))
					throw PInternalError( "Compression");


				wrBufCompSz = lzhl->compress( wrBufComp + sizeLength, wrBuf, wrBufOffset );
				wrBufOffset = 0;
				if ( sizeLength == 4 )
				{
					CommMsgBody::writeUINT32( wrBufComp, static_cast< UINT32 >( wrBufCompSz ) );
				}
				else
				{
					CommMsgBody::writeUINT16( wrBufComp, static_cast< UINT16 >( wrBufCompSz ) );
				}
				wrBufCompSz += sizeLength;
				wrBufCompOffset = 0;

				size_t wr = _write( wrBufComp + wrBufCompOffset, wrBufCompSz - wrBufCompOffset );
				wrBufCompOffset += wr;
				if (!(wrBufCompOffset <= wrBufCompSz))
					throw PInternalError( "Compression");

				if( wrBufCompOffset == wrBufCompSz )
					wrBufCompSz = 0;
			}

			//PASSERT3( _CrtCheckMemory() );
			return len;
		}
	}
}

template< class _Communicator >
void _CommCompressedCommunicator< _Communicator >::flush()
//NOTE: current implementation assumes that there are no more flushable _Communicators down the road
{
	//PTRACE( "FLUSH" );
	switch( mode )
	{
	case _CommCompressedCommunicatorTypes::compressionNone:
		break;

	case _CommCompressedCommunicatorTypes::compressionLzhl:
		{
			if( wrOnHold )
			{
				//				PLog( "_CommCompressedCommunicator< _Communicator >::flush(): Unexpected flush when wrOnHold (wrOnHoldSz=%d) - ignored", wrOnHoldSz );
				flushDelayed = true;
				return;
			}
			if( wrBufOffset == 0 )
				return;

			if (!(wrBufCompSz == 0))
				throw PInternalError( "Compression");

			wrBufCompSz = lzhl->compress( wrBufComp + sizeLength, wrBuf, wrBufOffset );
			wrBufOffset = 0;
			if ( sizeLength == 4 )
			{
				CommMsgBody::writeUINT32( wrBufComp, static_cast< UINT32 >( wrBufCompSz ) );
			}
			else
			{
				CommMsgBody::writeUINT16( wrBufComp, static_cast< UINT16 >( wrBufCompSz ) );
			}
			wrBufCompSz += sizeLength;
			wrBufCompOffset = 0;

			size_t wr = _write( wrBufComp + wrBufCompOffset, wrBufCompSz - wrBufCompOffset );
			wrBufCompOffset += wr;
			if (!(wrBufCompOffset <= wrBufCompSz))
				throw PInternalError( "Compression");

			if( wrBufCompOffset == wrBufCompSz )
				wrBufCompSz = 0;
			//PASSERT3( _CrtCheckMemory() );
			return;
		}
	}
}

template< class _Communicator >
_CommAsyncRet _CommCompressedCommunicator< _Communicator >::asyncWriteOk()
{
	_CommAsyncRet ret = _Communicator::asyncWriteOk();
	_countReceivedBytes += ret.read;
	_countSentBytes += ret.written;
	switch( mode )
	{
	case _CommCompressedCommunicatorTypes::compressionNone:
	default:
		return ret;
	case _CommCompressedCommunicatorTypes::compressionLzhl:
		{
			_asyncOk( ret );
			//PASSERT3( _CrtCheckMemory() );
			return ret;
		}
	}
}

template< class _Communicator >
void _CommCompressedCommunicator< _Communicator >::_asyncOk( _CommAsyncRet& ret )
{
	if( ret.written )
	{
		//PASSERT3( _CrtCheckMemory() );
		if (!(wrBufOffset == 0))
			throw PInternalError( "Compression");

		wrBufCompOffset += ret.written;
		if (!(wrBufCompOffset <= wrBufCompSz))
			throw PInternalError( "Compression");

		if( wrBufCompOffset != wrBufCompSz )
		{
			size_t wr = _write( wrBufComp + wrBufCompOffset, wrBufCompSz - wrBufCompOffset );
			wrBufCompOffset += wr;
		}

		if( wrBufCompOffset == wrBufCompSz )
		{
			if (!( wrBufOffset == 0 ))
				throw PInternalError( "Compression");

			wrBufCompSz = 0;
			size_t len = wrOnHoldSz;
			if( len > _BuffSize )
				len = _BuffSize;
			if(wrOnHoldSz)
			{
				memcpy( wrBuf, wrOnHold, len );
				wrBufOffset = len;
				wrOnHold = 0;
				wrOnHoldSz = 0;
				if(flushDelayed || wrBufOffset == _BuffSize)
				{
					flushDelayed = false;
					flush();
				}
			}
			ret.written = len;
		}
		else
			ret.written = 0;
		//PASSERT3( _CrtCheckMemory() );
	}

	if( ret.read )
	{
		rdBufDecompOffset += ret.read;
		ret.read = 0;
		for(;;)
		{
			//PASSERT3( _CrtCheckMemory() );
			if( rdBufDecompOffset == sizeLength )
			{

				if (!( rdBufDecompSz == sizeLength  ))
					throw PInternalError( "Compression");

				if (sizeLength == 4)
				{
					rdBufDecompSz += CommMsgBody::readUINT32( rdBufDecomp );
				}
				else
				{
					rdBufDecompSz += CommMsgBody::readUINT16( rdBufDecomp );
				}
				if( rdBufDecompSz > sizeLength + compSz )
					throw PInternalError( "_CommCompressedCommunicator: LZHL frame is too long" );
				//PASSERT3( _CrtCheckMemory() );
				size_t rd = _read( rdBufDecomp + rdBufDecompOffset, rdBufDecompSz - rdBufDecompOffset );
				//PASSERT3( _CrtCheckMemory() );
				rdBufDecompOffset += rd;
				if( rdBufDecompOffset != rdBufDecompSz )
					break;//forever
			}
			else if( rdBufDecompOffset == rdBufDecompSz )
			{
				//PTRACE( "READOK" );
				//PASSERT3( _CrtCheckMemory() );
				rdBufSz = lzhl->decompress( rdBuf, _BuffSize, rdBufDecomp + sizeLength, rdBufDecompSz - sizeLength );
				//PASSERT3( _CrtCheckMemory() );
				rdBufOffset = 0;

				size_t len = rdOnHoldSz;
				if( len > rdBufSz - rdBufOffset )
					len = rdBufSz - rdBufOffset;
				//PASSERT3( _CrtCheckMemory() );
				memcpy( rdOnHold, rdBuf + rdBufOffset, len );
				//PASSERT3( _CrtCheckMemory() );
				rdBufOffset += len;
				rdOnHold = 0;
				rdOnHoldSz = 0;
				ret.read = len;
				break;//forever
			}
			else
			{
				//PASSERT3( _CrtCheckMemory() );
				size_t rd = _read( rdBufDecomp + rdBufDecompOffset, rdBufDecompSz - rdBufDecompOffset );
				//PASSERT3( _CrtCheckMemory() );
				rdBufDecompOffset += rd;
				if( rdBufDecompOffset != rdBufDecompSz )
					break;//forever
			}
			//PASSERT3( _CrtCheckMemory() );
		}
		//PASSERT3( _CrtCheckMemory() );
	}
	//PASSERT3( _CrtCheckMemory() );
}

template< class _Communicator >
size_t _CommCompressedCommunicator< _Communicator >::readIncomplete( BYTE* p, size_t sz )
{
	switch( mode )
	{
	case _CommCompressedCommunicatorTypes::compressionNone:
	default:
		{
			size_t rd = _Communicator::readIncomplete( p, sz );
			_countReceivedBytes += rd;
			return rd;
		}


	case _CommCompressedCommunicatorTypes::compressionLzhl:
		{
			if( rdBufOffset == rdBufSz )
			{
				rdBufDecompSz = sizeLength;
				rdBufDecompOffset = 0;
				size_t rd = _read( rdBufDecomp + rdBufDecompOffset, rdBufDecompSz - rdBufDecompOffset );
				rdBufDecompOffset += rd;
				if( rdBufDecompOffset == sizeLength )
				{
					if (sizeLength == 4)
					{
						rdBufDecompSz += CommMsgBody::readUINT32( rdBufDecomp );
					}
					else
					{
						rdBufDecompSz += CommMsgBody::readUINT16( rdBufDecomp );
					}
					rd = _read( rdBufDecomp + rdBufDecompOffset, rdBufDecompSz - rdBufDecompOffset );
					rdBufDecompOffset += rd;
					if( rdBufDecompOffset == rdBufDecompSz )
					{
						rdBufSz = lzhl->decompress( rdBuf, _BuffSize, rdBufDecomp + sizeLength, rdBufDecompSz - sizeLength );
						rdBufOffset = 0;
					}
				}
			}

			if( rdBufOffset < rdBufSz )
			{
				size_t len = sz;
				if( len > rdBufSz - rdBufOffset )
					len = rdBufSz - rdBufOffset;
				memcpy( p, rdBuf + rdBufOffset, len );
				rdBufOffset += len;
				//PASSERT3( _CrtCheckMemory() );
				return len;
			}
			else
			{
				rdOnHold = p;
				rdOnHoldSz = sz;
				//PASSERT3( _CrtCheckMemory() );
				return 0;
			}
		}
	}
}

template< class _Communicator >
_CommAsyncRet _CommCompressedCommunicator< _Communicator >::asyncReadOk()
{
	//PASSERT3( _CrtCheckMemory() );
	_CommAsyncRet ret = _Communicator::asyncReadOk();
	_countReceivedBytes += ret.read;
	_countSentBytes += ret.written;

	switch( mode )
	{
	case _CommCompressedCommunicatorTypes::compressionNone:
	default:
		return ret;
	case _CommCompressedCommunicatorTypes::compressionLzhl:
		{
			//PASSERT3( _CrtCheckMemory() );
			_asyncOk( ret );
			//PASSERT3( _CrtCheckMemory() );
			return ret;
		}
	}
}

#endif
