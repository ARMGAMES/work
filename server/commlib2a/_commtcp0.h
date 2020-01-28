#ifndef _commtcp0_h_included
#define _commtcp0_h_included

#include "ppinclude.h"
#include "ppstring.h"
#include "pplogfile.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "_commcomm.h"
#include "commstreamencryptionpike.h"

#ifdef PROTOCOLLOGGING
#define _log1  _log
#else
#define _log1( ... ) 
#endif

#ifndef COMM_CLIENT_ONLY
#define DEFAULT_PROTOCOL 0x03
#define DEFAULT_CLIENT_ENCRYPTION false
#else
#define DEFAULT_PROTOCOL 0x04
#define DEFAULT_CLIENT_ENCRYPTION true
#endif



#define MIN_SUPPORTED_PROTOCOL 0x03
#define MAX_SUPPORTED_PROTOCOL 0x05

#define DEFAULT_FRAME_SIZE 64000
#define DEFAULT_FRAME_SIZE_VER5 512000
#define MAX_ALLOWED_FRAME_SIZE 65535
#define MAX_ALLOWED_FRAME_SIZE_VER5 0x7fffffff
#define DEFAULT_COMPRESS_BITS_SIZE 14
#define MAX_COMPRESS_BITS_SIZE 15
#define ALLOWED_COMPRESS_BITS_SIZE_FIRST 14
#define ALLOWED_COMPRESS_BITS_SIZE_SECOND 15

#define FRAME_DATA_TYPE 0x01
#define FRAME_INTERNAL_TYPE 0x02
#define MAX_SENT_HISTORY    60
#define CUT_DELTA_TIME      2000

#define ENCRYPTION_KEY_SIZE 5


#define ROTR( x, y ) ( ( (x) >> (y) ) | ( (x) << (32-(y)) ) )

class PBlockSmart : public PBlock
{
public:
	size_t head;
	size_t end;
	size_t blocksize;

	PBlockSmart()
	: PBlock()
	{
		blocksize=head=end=0;
	}
	PBlockSmart( size_t sz_ )
	: PBlock( sz_)
	{
		blocksize=head=end=0;
	}
	void alloc(size_t sz_)
	{
		PBlock::alloc(sz_);
		blocksize=head=end=0;
	}
	void shrink()
	{
		if(blocksize == 0 && head && head==end)
			blocksize=head=end=0;
	}
	BYTE * headptr()
	{
		return ptr() + head;
	}
	BYTE * endptr()
	{
		return ptr() + end;
	}

	BYTE * append(size_t sz_)
	{
		end+=sz_;
		if (!(end<=size()))
			throw PInternalError( "PBlockS: invalid size");

		return endptr();
	}
	void update_end(const BYTE *p_)
	{
		end=p_-ptr();
		if (!(end<=size()))
			throw PInternalError( "PBlockS: invalid size");

	}

	size_t rest() const
	{
		return size()-end;
	}
	size_t occupied() const
	{
		return end-head;
	}
	void block()
	{
		blocksize=end;
	}
	bool blocked() const
	{
		return blocksize > 0;
	}

	void deblock()
	{
		head=blocksize;
		blocksize=0;
	}
	void clear()
	{
		blocksize=head=end=0;
		PBlock::clear();
	}
};


template< class _Communicator, class _MsgHandler >
//	_Communicator prereqs:
//	static bool hasCompression( int type )
//	void setCompression( int type, size_t BuffSize  )
//	void setEncryption( CommStreamEncryption * encryption_ ) 
//  bool initConnection(UINT32 & checkSumm)
//	bool generateRandom(UINT32 * secureRandom, int size)
//	size_t write( const BYTE* p, size_t sz );
//	void flush();
//	size_t read( BYTE* p, size_t sz );
//	_CommAsyncRet asyncWriteOk();
//	_CommAsyncRet asyncReadOk();

//	_MsgHandler prereq:
//  void msg( int mux, UINT32 muxChannelId, _CommMsg& )
//  void serviceCall(UINT32 id, CommMsgBody * msg_body)

class _CommStream0ProtocolHandler
{
	private:
	UINT32 _MaxBlockSize;
	UINT32 _MaxBlockSizeStored;
	UINT32 _MaxBlockSizeStoredVer5;
	UINT32 sizeOfHeader;
	enum { _QualityRecovery = 100 /* 1 to 10000 */ };
	enum { _SizeOfShortHeader=3,_SizeOfShortHeaderVer5=5};
	struct _Write
	{
		_CommMsg msg;
		int offset;
	};
	struct _Read
	{
		_CommMsg msg;
		size_t size;
	};

	private:
	BYTE supported;
	_Communicator* comm;
	_MsgHandler* msgHandler;
	_CommProtocolLog *_logger;
	bool _init;
	bool _inbound;
	bool _conn;

	private:
	bool muxEnabled;
	UINT32 lastTimeInQueue;
	CommMsgSimpleTimeControlQueue wrQueue;
	int wrPriority;
	_Write wrBlocks[ _COMM_MSGPRIORITIES ];
	PBlock wrData;

	PBlockSmart wrDatasm; 




	private:
	int rdState;//0 - sz, 1 - body
	BYTE rdSz[ 7 ];
	PBlock rdData; // Only for old protocol. 

	_Read rdMsgs[ _COMM_MSGPRIORITIES ];

	private:


/*
#pragma pack(1)

    // Structures for information purposes only. 
	// Never use them even for casting.

	struct HandshakeHeader
	{
		BYTE    MaxSupportedProtocol;
		BYTE    MinSupportedProtocol;
		UINT32  KeepAliveWrite;
		UINT32  KeepAliveRead;
		UINT32  FrameSize;
		UINT32  CompressBitsSize;
		BYTE    muxEnabled;
		BYTE    reserved[3];
		UINT32  Reserved[5];  // Should be zeroed
	}; 

	struct ProtocolHeader
	{
		UINT16 FrameSize;
		BYTE   PacketType; // 1 - data packet, 2 internal protocol packet (like KeepAlve message)
		                   // 0x80 and 0x40 - First and last frame in the message
	};

	struct ProtocolHeaderVer5
	{
		UINT32 FrameSize;
		BYTE   PacketType; // 1 - data packet, 2 internal protocol packet (like KeepAlve message)
		                   // 0x80 and 0x40 - First and last frame in the message
	};

	struct ExtendedProtocolHeader
	{
		ProtocolHeader hd;
		UINT32         MsgSize;
	};

	struct KeepAliveMessage
	{
		BYTE    Counter;
		UINT32  SenderTimeStamp;
		UINT32  PeerTimeStamp;
	};

#pragma pack ()
*/

	BYTE hello[ 52 ];//client - 1, server - 4 + 4 + 1
	int helloRdState;//0 - version, 1 - rest

	private:
	PString ip_address;
	int _keepAlivePoll;
	BYTE keepAliveStub[ 2 ];
	int _keepAliveWrite;
	int _keepAliveRead;
	bool _keepAliveIgnoreDataPackets;

	int _keepAliveWriteStored;
	int _keepAliveReadStored;


	bool _beenLogged;

	UINT32 _lastWrite; 
	UINT32 _lastRead;
	UINT32 _lastReadWhenForced;
	UINT32 _countSentBytes;
	UINT32 _countReceivedBytes;
	UINT32 _receivedPackets;
	UINT32 _sentPackets;
	UINT32 _maxRead;
	UINT32 _maxWrite;
	UINT32 _lastDelta;
	UINT32 _whenStored;
	UINT32 _numberOfDelays;
	time_t _timeOfConnect;
	UINT32 _random;



	int _quality;//0-10000; meaningful only if keepAlive enabled
	int _LastGottenQuality;
	UINT32 lastQuality;

	BYTE   _MaxProtocolSupported;
	BYTE   _MinProtocolSupported;
	BYTE   _ProtocolSelected;
	BYTE   _ProtocolSelectedStored;
	BYTE   _clientMinimum;


	UINT32 _CompressBitsSize;   // Number of bits. Real size is power of 2
	UINT32 _CompressBitsSizeStored;

	UINT32 _LastRoundTrip;
	UINT32 _AvgRoundTrip;
	UINT32 _MaxRoundTrip;
	BYTE   _LastPacketSent;
	BYTE   _LastPacketReceived;
	bool   quality_required;




	UINT32 _checkSumm;
	UINT32 _peerPublicKey[ENCRYPTION_KEY_SIZE];


#ifndef COMM_CLIENT_ONLY
	UINT32 _maximumMessageSize;
	UINT32 _maximumMessageRate;
	UINT32 _maximumMessageRatePeriod;
	bool   _maxMessageSizeDebug;
	bool   _maxMessageRateDebug;
	UINT32 _mRateCounter;
	UINT32 _mRateTimer;
	bool   _mRateCounterInit;

	BYTE   _LastKeepAlivedForced;
	UINT32 _LastAckTimeReceived;
	UINT32 _LastAckTimeSent;
	bool   keep_alive_forced;
	bool   _flag_keep_alive_set;
	UINT32 _SentAckTimeHistory[MAX_SENT_HISTORY];
	int _currentAckSent;


	UINT32 _publicKey[ENCRYPTION_KEY_SIZE];
	UINT32 _privateKey[ENCRYPTION_KEY_SIZE];
	bool   _enableEncryptionServer;
	bool   _strictParametersEnforcement;
	UINT32 _minBlockEnforced;
	UINT32 _maxBlockEnforced;
	UINT32 _protectionCounter;
	UINT32 _protectionRandom;


#endif
	UINT32 _secureRandom;
	UINT32 _peerSecureRandom;
	bool   _enableEncryptionClient;
	bool   _useEncryption;
	CommPikeStream * encryption;
    UINT32 _netStatsLogThreshold;

    protected:
	BYTE * _prepareFrameHeader(size_t &packetsize, BYTE packettype, bool &isLast, bool isFirst);


	public:
	_CommStream0ProtocolHandler()
		: encryption( NULL )
	{
	}
	~_CommStream0ProtocolHandler()
	{
		if (encryption)
			delete encryption;
	}

	void _log(const char * fmt, ...)
	{
		if(_logger)
		{
			PString st;
			st.appendHexInt(THREADID());
			st += "-";
			st += ip_address;
			va_list args;
        	va_start( args, fmt );
	    	_logger->_CommLog(st,fmt, args);
		    va_end(args);
		}
	}
	void _vlog(const char * fmt, va_list args)
	{
		if(_logger)
		{
			PString st;
			st.appendHexInt(THREADID());
			st += "-";
			st += ip_address;
	    	_logger->_CommLog(st,fmt, args);
		}
	}

	void construct( _Communicator& comm_, _MsgHandler& msgHandler_,_CommProtocolLog * logging_ );
	void enableMux() { muxEnabled = true; }
	void setKeepAlive( int msecPoll, int msecWrite, int msecRead, bool ignoreDataPackets )
		//For inbound (server-side) connections only. 
		//Outbound (client-side) connections will get keepAlive from handshake
	{
		_keepAlivePoll = msecPoll;
		_keepAliveWriteStored = msecWrite;
		_keepAliveReadStored = msecRead;
		_keepAliveIgnoreDataPackets = ignoreDataPackets;
	}
	void setFirstRead();
	void setDefaultFrameSize(UINT32 FrameSize)
	{
		if(FrameSize <= MAX_ALLOWED_FRAME_SIZE && FrameSize > 0)
			_MaxBlockSizeStored = FrameSize;
	}
	void setDefaultLargeFrameSize(UINT32 FrameSize)
	{
		if (FrameSize > 0 && FrameSize <= MAX_ALLOWED_FRAME_SIZE_VER5)
			_MaxBlockSizeStoredVer5 = FrameSize;
	}


	void setDefaultCompressSize(UINT32 CompressBitsSize)
	{
		if(supported == 2)
		{
			if(CompressBitsSize == ALLOWED_COMPRESS_BITS_SIZE_FIRST || CompressBitsSize == ALLOWED_COMPRESS_BITS_SIZE_SECOND || CompressBitsSize==0)
			    _CompressBitsSizeStored = CompressBitsSize;
		}
	}
	void setDefaultProtocol(UINT32 DefaultProtocol)
	{
		if(DefaultProtocol>=_MinProtocolSupported && DefaultProtocol<=_MaxProtocolSupported)
		{
			_ProtocolSelected =DefaultProtocol;
			_ProtocolSelectedStored=DefaultProtocol;
		}
	}
#ifndef COMM_CLIENT_ONLY
	void setMinimumProtocol(UINT32 minimumProtocol)
	{
		if(minimumProtocol>=MIN_SUPPORTED_PROTOCOL && minimumProtocol<=MAX_SUPPORTED_PROTOCOL)
		{
			_MinProtocolSupported = minimumProtocol;
		}
	}
	void setServerEncryption(UINT32 * publicKey_, UINT32 * privateKey_)
	{
		_enableEncryptionServer = true;
		for (int i=0;i<ENCRYPTION_KEY_SIZE; i++)
		{
			_publicKey[i] = publicKey_[i];
			_privateKey[i] = privateKey_[i];
		}
	}
	void setParametersEnforced(UINT32 minBlock,UINT32 maxBlock, UINT32 protectionRandom)
	{
		_strictParametersEnforcement = true;
		_minBlockEnforced = minBlock;
		_maxBlockEnforced = maxBlock;
		_protectionRandom = protectionRandom;

	}
	void setMaximumMessageSize(UINT32 maximumMessageSize, bool dbg)
	{
		_maximumMessageSize = maximumMessageSize;
		_maxMessageSizeDebug = dbg;
	}
	void setMaximumMessageRate(UINT32 maximumMessageRate, UINT32 maximumMessageRatePeriod, bool dbg)
	{
		_maximumMessageRate = maximumMessageRate;
		_maximumMessageRatePeriod = maximumMessageRatePeriod;
		_maxMessageRateDebug = dbg;
	}

#endif
	void setClientEncryption(bool enable)
	{
		_enableEncryptionClient = enable;
	}
    void setNetworkStatsLogThreshold(UINT32 netStatsLogThreshold)
    {
        _netStatsLogThreshold = netStatsLogThreshold;
    }

	void init( bool inbound );
	void deinit();

	bool isConnected() const
	{
		if (!_init)
			throw PInternalError( "Not initialized");

		return _conn;
	}
	void writeMsg( int mux, UINT32 muxChannelId, _CommMsg& msg );
    void writeLowLevel(const BYTE * data, size_t size);
	void asyncWriteOk();
	void asyncReadOk();

	bool serviceCallTransport(UINT32 id, CommMsgBody *msg_body);

	void storeMsg( int mux, UINT32 muxChannelId, _CommMsg& msg);
	void flushStoredMsgs();

	private://helpers

	bool keepAlive( UINT32& readFreeze );
	bool _writeFirstMsg( int mux, UINT32 muxChannelId, _CommMsg& msg );
	bool _writeFirstMsg( int priority );
    bool _writeNextMsg(int priority);
    bool _writeLowLevel(const BYTE * data, size_t size);
	void _flushMsg();
	void _doneRead()
	{
		_done( -1 );
	}
	void _doneWrite()
	{
		_done( 1 );
	}
	void _done( int done );
    void printStatistic(BYTE level);
	void _print_connect_info();
	
	void prepareClientHello();
	bool readServerHello();
	void writeClientKeyExchange();
	void readServerKeyExchange();
#ifndef COMM_CLIENT_ONLY
	void prepareServerHello();
	void readClientHello();
	void readClientKeyExchange();
	void writeServerKeyExchange();
	void _print_binary_data(const BYTE * ptr, int size);
#endif

	void startHandshake();

	inline BYTE _localRand()
	{
		_random = 22695477 * _random + 1;
		return (( _random >> 8 ) & 0xff) ^ (_random &0xff);
	}
public:
	int outQueueSize() const
	{
		return wrQueue.size();
	}
};

template< class _Communicator, class _MsgHandler >
void _CommStream0ProtocolHandler< _Communicator, _MsgHandler >::construct( _Communicator& comm_, _MsgHandler& msgHandler_, _CommProtocolLog * logger_ )
{
	_logger = logger_;
	comm = &comm_;
	msgHandler = &msgHandler_;
	_init = false;
	muxEnabled = false;

	_keepAliveWrite = _keepAliveWriteStored = 0;
	_keepAliveRead = _keepAliveReadStored = 0;
	_keepAlivePoll = 0;
    _lastReadWhenForced=0;
	_keepAliveIgnoreDataPackets = false;
	quality_required=false;
#ifndef COMM_CLIENT_ONLY
	keep_alive_forced=false;
	_flag_keep_alive_set = false;
	_LastAckTimeReceived = 0;
	_LastAckTimeSent = 0;
	memset(_SentAckTimeHistory,0,sizeof(_SentAckTimeHistory));
	_currentAckSent=-1;
    _LastKeepAlivedForced=0;
	_strictParametersEnforcement=false;
	_minBlockEnforced=0;
	_maxBlockEnforced=0;
	_protectionCounter=0;
	_protectionRandom=0;
	_maximumMessageSize = 0;
	_maximumMessageRate = 0;
	_maximumMessageRatePeriod = 0;
	_maxMessageSizeDebug = false;
	_maxMessageRateDebug = true;
	_mRateCounter= 0;
	_mRateTimer = 0;
	_mRateCounterInit = false;

#endif
    _beenLogged=false;
	_countSentBytes = 0;
	_countReceivedBytes = 0;
	_receivedPackets = 0;
	_sentPackets = 0;
	_lastDelta = 0;
	_maxRead = 0;
	_maxWrite = 0;

	_whenStored = 0;
	_numberOfDelays = 0;
	_timeOfConnect = 0;
	helloRdState = 0;
	_useEncryption = false;


	CommMsgBody::writeUINT16( keepAliveStub, 0 );

	supported = 1;

	_ProtocolSelectedStored=_ProtocolSelected=DEFAULT_PROTOCOL;
	_MaxProtocolSupported = MAX_SUPPORTED_PROTOCOL;
	_MinProtocolSupported = MIN_SUPPORTED_PROTOCOL;
	_MaxBlockSizeStored = DEFAULT_FRAME_SIZE;
	sizeOfHeader = _ProtocolSelected == 5 ? _SizeOfShortHeaderVer5:_SizeOfShortHeader;
	_MaxBlockSizeStoredVer5 = DEFAULT_FRAME_SIZE_VER5;
	_MaxBlockSize = _ProtocolSelected == 5 ? _MaxBlockSizeStoredVer5:_MaxBlockSizeStored;
	_CompressBitsSizeStored = _CompressBitsSize = DEFAULT_COMPRESS_BITS_SIZE;


	lastTimeInQueue=0;
	_LastRoundTrip = 0;
	_AvgRoundTrip = 0;
	_LastPacketSent = 0;
	_LastPacketReceived = 0;

    _MaxRoundTrip=0;
	_clientMinimum = 0;
	encryption = 0;
    _netStatsLogThreshold = 60000;

	if( _Communicator::hasCompression( _CommCompressedCommunicatorTypes::compressionLzhl ) )
	{
		supported = 2;
#ifndef COMM_CLIENT_ONLY
		_enableEncryptionServer = false;
		_enableEncryptionClient = false;
#else
		_enableEncryptionClient = true;
#endif
	}
	else
	{
		_CompressBitsSize=0;
        _CompressBitsSizeStored=0;
		_enableEncryptionClient = false;
#ifndef COMM_CLIENT_ONLY
		_enableEncryptionServer = false;
#endif
	}
}


template< class _Communicator, class _MsgHandler >
void _CommStream0ProtocolHandler< _Communicator, _MsgHandler >::prepareClientHello( )
{
	if(_ProtocolSelected==3)
	{
		_useEncryption = false;
		memset(hello,0,sizeof(hello));
		hello[0]=_ProtocolSelected;
		hello[1]=_MinProtocolSupported;
    	CommMsgBody::writeUINT32( hello + 2,  _keepAliveWrite );
		CommMsgBody::writeUINT32( hello + 6,  _keepAliveRead );
		CommMsgBody::writeUINT32( hello + 10, _MaxBlockSize);
		CommMsgBody::writeUINT32( hello + 14, _CompressBitsSize);
		hello[18] = muxEnabled;
	}
	else if(_ProtocolSelected>3)
	{
		int i;
		for (i=0;i<42;i++)
			hello[i] = rand();
		hello[0]=_ProtocolSelected;
		CommMsgBody::writeUINT32( hello + 1,  _random );
		hello[5]=_MinProtocolSupported;
		CommMsgBody::writeUINT32( hello + 6,  _keepAliveWrite );
		CommMsgBody::writeUINT32( hello + 10, _keepAliveRead );
		CommMsgBody::writeUINT32( hello + 14, _MaxBlockSize);
		CommMsgBody::writeUINT32( hello + 18, _CompressBitsSize);
		CommMsgBody::writeUINT32( hello + 22, _checkSumm);
		hello[26] = (BYTE) muxEnabled;
		if ( _useEncryption )
		{
			hello[27] = 1;
			comm->generateRandom(&_secureRandom, 1);
			CommMsgBody::writeUINT32( hello + 28, _secureRandom);
		}
		else
		{
			hello[27] = 0;
		}
		UINT32 c=0;
		for(i=0;i<38;i++)
		{
			c ^= hello[i];
			c = ROTR(c,8);
		}
		CommMsgBody::writeUINT32( hello + 38, c);
		for(i=5;i<42;i++)
		{
			hello[i] ^= _localRand();
		}
	}
	else
		throw PInternalError( "Client CommTCP0: invalid protocol");
}

template< class _Communicator, class _MsgHandler >
bool _CommStream0ProtocolHandler< _Communicator, _MsgHandler >::readServerHello()
{
	if(_ProtocolSelected==3)
	{
		_useEncryption = false;
		_keepAliveWrite = CommMsgBody::readUINT32( hello+2 );
		_keepAliveRead  = CommMsgBody::readUINT32( hello+6 );
		_MaxBlockSize   = CommMsgBody::readUINT32( hello+10 );
		_CompressBitsSize = CommMsgBody::readUINT32( hello+14 );
		muxEnabled = hello[18] == 0 ? false: true;
		if(_MaxBlockSize == 0 ||  _MaxBlockSize > MAX_ALLOWED_FRAME_SIZE)
			throw PInternalError( "Client CommTCP0: invalid _MaxBlockSize " );
		if(_CompressBitsSize && _CompressBitsSize!=ALLOWED_COMPRESS_BITS_SIZE_FIRST && _CompressBitsSize!=ALLOWED_COMPRESS_BITS_SIZE_SECOND )
			throw PInternalError( "Client CommTCP0: invalid CompressBitsSize");
		return true;
	}

	if (_ProtocolSelected>3)
	{
		int i;
		for(i=1;i<48;i++)
		{
			hello[i] ^= _localRand();
		}
		UINT32 c=0;
		for(i=0;i<44;i++)
		{
			c ^= hello[i];
			c = ROTR(c,8);
		}
		if (c != CommMsgBody::readUINT32( hello + 44 ))
		{
			throw PInternalError( "Client CommTCP0: checksum is invalid" );
		}

		_keepAliveWrite = CommMsgBody::readUINT32( hello+1 );
		_keepAliveRead  = CommMsgBody::readUINT32( hello+5 );
		_MaxBlockSize   = CommMsgBody::readUINT32( hello+9 );
		_CompressBitsSize = CommMsgBody::readUINT32( hello+13 );
		muxEnabled = hello[17] == 0 ? false: true;
		if(_MaxBlockSize == 0 ||  _MaxBlockSize > (_ProtocolSelected == 5? MAX_ALLOWED_FRAME_SIZE_VER5:MAX_ALLOWED_FRAME_SIZE))
			throw PInternalError( "Client CommTCP0: invalid _MaxBlockSize " );
		if(_CompressBitsSize && _CompressBitsSize!=ALLOWED_COMPRESS_BITS_SIZE_FIRST && _CompressBitsSize!=ALLOWED_COMPRESS_BITS_SIZE_SECOND )
			throw PInternalError( "Client CommTCP0: invalid CompressBitsSize");
		_useEncryption = hello[18] == 0 ? false: true;
		if (_useEncryption)
		{
			_peerSecureRandom = CommMsgBody::readUINT32( hello+20 );
			_peerPublicKey[0] = CommMsgBody::readUINT32( hello+24 );
			_peerPublicKey[1] = CommMsgBody::readUINT32( hello+28 );
			_peerPublicKey[2] = CommMsgBody::readUINT32( hello+32 );
			_peerPublicKey[3] = CommMsgBody::readUINT32( hello+36 );
			_peerPublicKey[4] = CommMsgBody::readUINT32( hello+40 );
			return false;
		}

		return true;
	}

	throw PInternalError( "Client CommTCP0: invalid protocol");
}

template< class _Communicator, class _MsgHandler >
void _CommStream0ProtocolHandler< _Communicator, _MsgHandler >::writeClientKeyExchange()
{
	if(_ProtocolSelected==3)
	{
		throw PInternalError( "Client CommTCP0: invalid handshake state" );
	}
	else if (_ProtocolSelected>3)
	{
		if (!_useEncryption)
			throw PInternalError( "Client CommTCP0: invalid handshake state" );
		int i;
		for (i=0;i<52;i++)
			hello[i] = rand();

		encryption = new CommPikeStream(_inbound);
		UINT32 masterKey[5];
		comm->generateRandom( masterKey, 5);
		masterKey[4] &= 0x7fffffff;
		encryption->setKey(_peerSecureRandom ^ _secureRandom, masterKey, 5);
		CommRSA::encrypt(masterKey, _peerPublicKey,  hello, ENCRYPTION_KEY_SIZE);
		UINT32 c=0;
		for(i=0;i<20;i++)
		{
			c ^= hello[i];
			c = ROTR(c,8);
		}
		CommMsgBody::writeUINT32( hello + 20, c);
		for(i=0;i<24;i++)
		{
			hello[i] ^= _localRand();
		}
		for(i=24;i<52;i++)
		{
			hello[i] = _localRand() ^ encryption->getWrite();
		}
	}
	else
		throw PInternalError( "Client CommTCP0: invalid protocol");
}

template< class _Communicator, class _MsgHandler >
void _CommStream0ProtocolHandler< _Communicator, _MsgHandler >::readServerKeyExchange()
{
	if(_ProtocolSelected==3)
	{
		throw PInternalError( "Client CommTCP0: invalid handshake state" );
	}
	else if (_ProtocolSelected>3)
	{
		if (!_useEncryption)
			throw PInternalError( "Client CommTCP0: invalid handshake state" );

		for(int i=0;i<24;i++)
		{
			if (_localRand() != (hello[i] ^ encryption->getRead()))
			{
				throw PInternalError( "Client CommTCP0: encryption is invalid" );
			}
		}
	}
	else
		throw PInternalError( "Client CommTCP0: invalid protocol");

}



#ifndef COMM_CLIENT_ONLY

template< class _Communicator, class _MsgHandler >
void _CommStream0ProtocolHandler< _Communicator, _MsgHandler >::readClientHello( )
{
	if(_ProtocolSelected==3)
	{
#ifdef COMM_PTR_TRACE
		_log("Client hello:");
		_print_binary_data(hello,42);
#endif

		_useEncryption = false;
		_clientMinimum = hello[1];
		UINT32 keepAliveClientWrite = CommMsgBody::readUINT32( hello+2 );
		UINT32 keepAliveClientRead  = CommMsgBody::readUINT32( hello+6 );
		UINT32 MaxBlockClient = CommMsgBody::readUINT32( hello+10 );
		UINT32 BitCompressClient = CommMsgBody::readUINT32( hello+14 );
		muxEnabled = hello[18] == 0 ? false : true;
		_log(" client proposal: protocol %u, min protocol %u, BlockSize %d, Compression %d, keepAlives - %d(%d), muxEnabled = %s",
			_ProtocolSelected,_clientMinimum, MaxBlockClient,BitCompressClient, keepAliveClientWrite, keepAliveClientRead,(muxEnabled? "yes":"no"));
		if( MaxBlockClient == 0 || MaxBlockClient > MAX_ALLOWED_FRAME_SIZE)
			 throw PInternalError( "Server CommTCP0: client BlockSize is invalid" );
		_MaxBlockSize = _MaxBlockSize<MaxBlockClient?_MaxBlockSize:MaxBlockClient;
		if(BitCompressClient && BitCompressClient!=ALLOWED_COMPRESS_BITS_SIZE_FIRST && BitCompressClient!=ALLOWED_COMPRESS_BITS_SIZE_SECOND )
   			 throw PInternalError( "Server CommTCP0: client CompressBitsSize is invalid" );
		if (BitCompressClient == 0)
			_CompressBitsSize = 0;

	}
	else if(_ProtocolSelected>3)
	{
#ifdef COMM_PTR_TRACE
		_log("Client hello:");
		_print_binary_data(hello,42);
#endif
		int i;
		_random = CommMsgBody::readUINT32( hello+1 );
		for(i=5;i<42;i++)
		{
			hello[i] ^= _localRand();
		}
#ifdef COMM_PTR_TRACE
		_log("Client hello decrypted:");
		_print_binary_data(hello,42);
#endif

		UINT32 c=0;
		for(i=0;i<38;i++)
		{
			c ^= hello[i];
			c = ROTR(c,8);
		}
		if (c != CommMsgBody::readUINT32( hello + 38 ))
		{
			throw PInternalError( "Server CommTCP0: checksum is invalid" );
		}
		if (_checkSumm != CommMsgBody::readUINT32( hello + 22 ))
		{
			throw PInternalError( "Server CommTCP0: SSL checksum is invalid" );
		}
		_clientMinimum = hello[5];
		UINT32 keepAliveClientWrite = CommMsgBody::readUINT32( hello+6 );
		UINT32 keepAliveClientRead  = CommMsgBody::readUINT32( hello+10 );
		UINT32 MaxBlockClient = CommMsgBody::readUINT32( hello+14 );
		UINT32 BitCompressClient = CommMsgBody::readUINT32( hello+18 );
		muxEnabled = hello[26] == 0 ? false : true;
		bool clientEncryption = hello[27] == 0 ? false : true;
		_log(" client proposal: protocol %u(%s), min protocol %u, BlockSize %d, Compression %d, keepAlives - %d(%d), muxEnabled = %s",
			_ProtocolSelected,(clientEncryption?"e":"ne"),_clientMinimum, MaxBlockClient,BitCompressClient, keepAliveClientWrite, keepAliveClientRead,(muxEnabled? "yes":"no"));

		if( MaxBlockClient == 0 || MaxBlockClient > (_ProtocolSelected == 5? MAX_ALLOWED_FRAME_SIZE_VER5:MAX_ALLOWED_FRAME_SIZE))
			 throw PInternalError( "Server CommTCP0: client BlockSize is invalid" );
		_MaxBlockSize = _MaxBlockSize<MaxBlockClient?_MaxBlockSize:MaxBlockClient;
		if(BitCompressClient && BitCompressClient!=ALLOWED_COMPRESS_BITS_SIZE_FIRST && BitCompressClient!=ALLOWED_COMPRESS_BITS_SIZE_SECOND )
   			 throw PInternalError( "Server CommTCP0: client CompressBitsSize is invalid" );
		if (_strictParametersEnforcement)
		{
			bool ok=true;
			if (clientEncryption != _enableEncryptionServer)
			{
				ok = false;
				_log("Server CommTCP0: strict parameter enforcement: clientEncryption (%s) != server encryption (%s)",(clientEncryption?"true":"false"),(_enableEncryptionServer?"true":"false"));
			}
			if (MaxBlockClient < _minBlockEnforced)
			{
				ok = false;
				_log("Server CommTCP0: strict parameter enforcement: client block (%d) < min block (%d)",MaxBlockClient,_minBlockEnforced);
			}
			if (MaxBlockClient > _maxBlockEnforced)
			{
				ok = false;
				_log("Server CommTCP0: strict parameter enforcement: client block (%d) > max block (%d)",MaxBlockClient,_maxBlockEnforced);
			}
			if (BitCompressClient != _CompressBitsSize)
			{
				ok = false;
				_log("Server CommTCP0: strict parameter enforcement: client compress (%d) != server compress (%d)",BitCompressClient,_CompressBitsSize);
			}
			if (!ok)
			{
				if (_protectionRandom == 0)
				{
					 throw PInternalError( "Server CommTCP0: strict parameter enforcement" );
				}
				else
				{
					_protectionCounter = rand() % _protectionRandom + 2;
				}
			}
		}
		if (BitCompressClient == 0)
			_CompressBitsSize = 0;

		if (clientEncryption && _enableEncryptionServer && _CompressBitsSize)
		{
			_useEncryption =true;
			_peerSecureRandom = CommMsgBody::readUINT32( hello+28 );
		}
		else
		{
			_useEncryption =false;
		}
	}
	else
		throw PInternalError( "Server CommTCP0: invalid protocol");
}

template< class _Communicator, class _MsgHandler >
void _CommStream0ProtocolHandler< _Communicator, _MsgHandler >::prepareServerHello( )
{
	if(_ProtocolSelected==3)
	{
	     memset(hello,0,42);
		hello[0]=_ProtocolSelected;
		hello[1]=_MinProtocolSupported;
		CommMsgBody::writeUINT32( hello + 2,  _keepAliveWrite);
		CommMsgBody::writeUINT32( hello + 6,  _keepAliveRead);
		CommMsgBody::writeUINT32( hello + 10, _MaxBlockSize);
		CommMsgBody::writeUINT32( hello + 14, _CompressBitsSize);
		hello[18]=muxEnabled;
	}
	else if(_ProtocolSelected>3)
	{
		int i;
		for (i=0;i<52;i++)
			hello[i] = rand();
		hello[0]=_ProtocolSelected;
		CommMsgBody::writeUINT32( hello + 1,  _keepAliveWrite);
		CommMsgBody::writeUINT32( hello + 5,  _keepAliveRead);
		CommMsgBody::writeUINT32( hello + 9,  _MaxBlockSize);
		CommMsgBody::writeUINT32( hello + 13, _CompressBitsSize);
		hello[17]=muxEnabled;
		if (_useEncryption)
		{
			comm->generateRandom(&_secureRandom, 1);
			hello[18]= 1;
			CommMsgBody::writeUINT32( hello + 20, _secureRandom );
			CommMsgBody::writeUINT32( hello + 24, _publicKey[0] );
			CommMsgBody::writeUINT32( hello + 28, _publicKey[1] );
			CommMsgBody::writeUINT32( hello + 32, _publicKey[2] );
			CommMsgBody::writeUINT32( hello + 36, _publicKey[3] );
			CommMsgBody::writeUINT32( hello + 40, _publicKey[4] );
		}
		else
		{
			hello[18] = 0;
		}

		UINT32 c=0;
		for(i=0;i<44;i++)
		{
			c ^= hello[i];
			c = ROTR(c,8);
		}
		CommMsgBody::writeUINT32( hello + 44, c);
		for(i=1;i<48;i++)
		{
			hello[i] ^= _localRand();
		}
	}
	else
		throw PInternalError( "Server CommTCP0: invalid protocol");
}





template< class _Communicator, class _MsgHandler >
void _CommStream0ProtocolHandler< _Communicator, _MsgHandler >::readClientKeyExchange()
{
	if(_ProtocolSelected==3)
	{
		throw PInternalError( "Server CommTCP0: invalid handshake state" );
	}
	else if (_ProtocolSelected>3)
	{
		if (!_useEncryption)
			throw PInternalError( "Server CommTCP0: invalid handshake state" );

		int i;
		for(i=0;i<24;i++)
		{
			hello[i] ^= _localRand();
		}
#ifdef COMM_PTR_TRACE
		_log("Client key exchange:");
		_print_binary_data(hello,52);
#endif

		UINT32 c=0;
		for(i=0;i<20;i++)
		{
			c ^= hello[i];
			c = ROTR(c,8);
		}
		if (c != CommMsgBody::readUINT32( hello + 20 ))
		{
			throw PInternalError( "Server CommTCP0: checksum is invalid" );
		}

		encryption = new CommPikeStream(_inbound);
		UINT32 masterKey[5];
		CommRSA::decrypt(masterKey, _publicKey, _privateKey,  hello, ENCRYPTION_KEY_SIZE);
		encryption->setKey(_peerSecureRandom ^ _secureRandom, masterKey, 5);
		for(i=24;i<52;i++)
		{
unsigned char c = encryption->getRead();
			if (_localRand() != (hello[i] ^ c /*encryption->getRead()*/ ))
			{
				throw PInternalError( "Server CommTCP0: encryption is invalid" );
			}
		}
	}
	else
		throw PInternalError( "Server CommTCP0: invalid protocol");

}

template< class _Communicator, class _MsgHandler >
void _CommStream0ProtocolHandler< _Communicator, _MsgHandler >::writeServerKeyExchange()
{
	if(_ProtocolSelected==3)
	{
		throw PInternalError( "Server CommTCP0: invalid handshake state" );
	}
	else if (_ProtocolSelected>3)
	{
		if (!_useEncryption)
			throw PInternalError( "Server CommTCP0: invalid handshake state" );

		int i;
		for(i=0;i<24;i++)
		{
			hello[i] = _localRand() ^ encryption->getWrite();
		}
	}
	else
		throw PInternalError( "Server CommTCP0: invalid protocol");

}

#endif
template< class _Communicator, class _MsgHandler >
void _CommStream0ProtocolHandler< _Communicator, _MsgHandler >::init( bool inbound )
{
	if (_init)
		throw PInternalError( "Initialized");

	ip_address = PString("");
	_init = true;
	_inbound = inbound;
	_conn = false;
	_keepAliveWrite = _keepAliveWriteStored;
	_keepAliveRead = _keepAliveReadStored;
    _beenLogged=false;

    _lastReadWhenForced=0;
	quality_required=false;


	wrPriority = -1;
	for( int i=0; i < _COMM_MSGPRIORITIES ; ++i )
		{
		wrBlocks[ i ].offset = -1;
		wrBlocks[ i ].msg.body._dealloc();
		rdMsgs[i].size = 0;
		rdMsgs[i].msg.body._dealloc();
		}


	rdData.clear();
	wrData.clear();
	wrDatasm.clear();
	wrQueue.clear();
#ifndef COMM_CLIENT_ONLY
	_flag_keep_alive_set = false;
	keep_alive_forced=false;
	_LastAckTimeSent=_LastAckTimeReceived =0;
    _LastKeepAlivedForced=0;
	memset(_SentAckTimeHistory,0,sizeof(_SentAckTimeHistory));
	_currentAckSent=-1;
	_strictParametersEnforcement=false;
	_minBlockEnforced=0;
	_maxBlockEnforced=0;
	_protectionCounter=0;
	_mRateCounter= 0;
	_mRateCounterInit = false;
	_mRateTimer = 0;
#endif
	_lastWrite = _lastRead = SYSTEMTICKS();
	_MaxRoundTrip=0;
	_LastRoundTrip = 0;
	_AvgRoundTrip = 0;
	_LastPacketSent = 0;
	_LastPacketReceived = 0;

	_countSentBytes = 0;
	_countReceivedBytes = 0;
	_receivedPackets = 0;
	_sentPackets = 0;
	_lastDelta=0;
	_maxRead=0;
	_maxWrite=0;
	lastTimeInQueue=0;

	_checkSumm = 0;
	_whenStored = SYSTEMTICKS();
	_clientMinimum=0;

	memset(_peerPublicKey,0,sizeof(_peerPublicKey));
	_quality = 10000;
    _LastGottenQuality=10000;

	_ProtocolSelected = _ProtocolSelectedStored;
	_MaxBlockSize = _ProtocolSelected == 5 ? _MaxBlockSizeStoredVer5:_MaxBlockSizeStored;
	sizeOfHeader = _ProtocolSelected == 5 ? _SizeOfShortHeaderVer5:_SizeOfShortHeader;
	_CompressBitsSize = _CompressBitsSizeStored;
	_numberOfDelays = 0;
	helloRdState=0;
	_timeOfConnect = plib_time(0);
	_random ^= (rand()<<16) | rand();
	_random ^= _timeOfConnect;
	_random ^= _lastRead;
	_random ^= PID();
#ifndef COMM_CLIENT_ONLY
	if (inbound)
	{
		if (_CompressBitsSize && _enableEncryptionServer && _ProtocolSelected == 4)
			_useEncryption = true;
		else
			_useEncryption = false;
	}
	else
#endif
	{
		if (_CompressBitsSize && _enableEncryptionClient && _ProtocolSelected == 4)
			_useEncryption = true;
		else
			_useEncryption = false;
	}
	helloRdState = 0;
	if (encryption)
		delete encryption;
	encryption = 0;
	if (comm->initConnection( _checkSumm ))
		startHandshake();
}

template< class _Communicator, class _MsgHandler >
void _CommStream0ProtocolHandler< _Communicator, _MsgHandler >::startHandshake()
{
   	helloRdState=1;
	if( !_inbound )
	{//client

		prepareClientHello();
		size_t wr = comm->write( hello, 42 );
		_countSentBytes+=wr;
		if( wr != 0 )
			_doneWrite();
	}
	else
	{//server
		size_t rd = comm->read( hello, 42 );
		_countReceivedBytes+=rd;
		if( rd != 0 )
			_doneRead();
	}

}


template< class _Communicator, class _MsgHandler >
void _CommStream0ProtocolHandler< _Communicator, _MsgHandler >::deinit()
{
	UINT32 now = SYSTEMTICKS();
	int cont;
#ifndef COMM_CLIENT_ONLY
	cont = _currentAckSent;
    if(_conn && _LastAckTimeSent)
	{
		if (_currentAckSent>=0)
		{
			now = now - _SentAckTimeHistory[0];
		}
		else
		{
			now = _LastRoundTrip;
		}
	}
	else
#endif
	{
		cont = -1;
		now = now - _lastRead;
	}
	_log("Disconnected. Connection time=%d, number of delays = %d, Timeout=%u, Last trip=%u, counter=%d, Avg=%u", (int)(plib_time(0) - _timeOfConnect), _numberOfDelays, now,_LastRoundTrip,cont,_AvgRoundTrip); 
	_init = false;
	if (encryption)
		delete encryption;
	encryption = 0;
}


template< class _Communicator, class _MsgHandler >
void _CommStream0ProtocolHandler< _Communicator, _MsgHandler >::asyncWriteOk()
{
	if ( !_init)
		throw PInternalError( "Not initialized");

	if (!_conn && helloRdState == 0)
	{
		if (comm->initConnection( _checkSumm ))
			startHandshake();
		return;
	}
	_CommAsyncRet ret = comm->asyncWriteOk();
	_countSentBytes += ret.written;
	_countReceivedBytes += ret.read;
	if( ret.read )
		_doneRead();
	if( ret.written )
		_doneWrite();
}

template< class _Communicator, class _MsgHandler >
void _CommStream0ProtocolHandler< _Communicator, _MsgHandler >::asyncReadOk()
	{
	if ( !_init)
		throw PInternalError( "Not initialized");

	if (!_conn && helloRdState == 0)
	{
		if (comm->initConnection( _checkSumm ))
			startHandshake();
		return;
	}

	_CommAsyncRet ret = comm->asyncReadOk();
	_countSentBytes += ret.written;
	_countReceivedBytes += ret.read;

	if( ret.read )
		_doneRead();
	if( ret.written )
		_doneWrite();
	}

template< class _Communicator, class _MsgHandler >
void _CommStream0ProtocolHandler< _Communicator, _MsgHandler >::setFirstRead()
{

	size_t rd;
	if(_ProtocolSelected >= 3)
	{
		rdState = 0;
        if(!rdData.size())
		{
       	    rdData.alloc(_MaxBlockSize);
		}
		rd = comm->read( rdData.ptr(), sizeOfHeader);  // read next standard header
		_countReceivedBytes+=rd;
		if( rd != 0 )
			_doneRead();

	}
	else
	{
		throw PInternalError( "Invalid protocol");
	}
}



template< class _Communicator, class _MsgHandler >
void _CommStream0ProtocolHandler< _Communicator, _MsgHandler >::_done( int done_ )
{
	if ( !_init)
		throw PInternalError( "Not initialized");

	if(_ProtocolSelected < 3)
	{
		throw PInternalError( "Invalid protocol");
	}

	int done = done_>0 ? 1 : 2;
	_log1("enter done() - %d",done);
	for(; done != 0 ;)
	{
	   _log1("for in done = %d",done);
		if( done&0x01)
		{
			//written
			
			UINT32 maxWrite = SYSTEMTICKS() - _lastWrite ;
			if (maxWrite > _maxWrite )
				_maxWrite = maxWrite;
			_lastWrite = SYSTEMTICKS();


			if( !_conn )
			{
				done &= ~0x01;
				if( !_inbound )
				{//client
					switch (helloRdState)
					{
					case 1: // just sent first hello
						{
							helloRdState = 2;
				           _log1("done() comm hello read");
							size_t rd;
						   if (_ProtocolSelected == 3 )
						   {
							   rd = comm->read( hello, 42);
						   }
						   else
						   {
								rd = comm->read( hello, 48);
						   }
							_countReceivedBytes+=rd;
							if( rd != 0 )
								done |= 0x02;
						}
						break;
					case 3:
						{
							if(_ProtocolSelected==3)
							{
								throw PInternalError( "Client CommTCP0: invalid handshake state" );
							}
							helloRdState = 4;
							size_t rd = comm->read( hello, 24);
							_countReceivedBytes+=rd;
							if( rd != 0 )
								done |= 0x02;
						}
						break;
					default:
						{
							throw PInternalError( "Client CommTCP0: invalid handshake state" );
						}

					}
				}
				else
#ifndef COMM_CLIENT_ONLY
				{//server
					switch(helloRdState)
					{
					case 2:
						{
							if (_ProtocolSelected == 3 || !_useEncryption)
							{
								if(_CompressBitsSize)
								{
									comm->setCompression( _CompressBitsSize == ALLOWED_COMPRESS_BITS_SIZE_SECOND ? _CommCompressedCommunicatorTypes::compressionLzhl:_CommCompressedCommunicatorTypes::compressionLzhl1,_MaxBlockSize);
								}
							}
							else
							{
								helloRdState = 3;
							   _log1("done() comm hello read");
								size_t rd = comm->read( hello, 52);
								_countReceivedBytes+=rd;
								if( rd != 0 )
									done |= 0x02;
								continue;
							}
							break;
						}
					case 4:
						{
							if(_ProtocolSelected==3)
							{
								throw PInternalError( "Server CommTCP0: invalid handshake state" );
							}
							helloRdState = 5;
							if(!_CompressBitsSize)
							{
								throw PInternalError( "Server CommTCP0: invalid handshake state" );
							}
							comm->setEncryption(encryption);
 							comm->setCompression( _CompressBitsSize == ALLOWED_COMPRESS_BITS_SIZE_SECOND ? _CommCompressedCommunicatorTypes::compressionLzhl:_CommCompressedCommunicatorTypes::compressionLzhl1,_MaxBlockSize);
							break;
						}

					default:
						{
							throw PInternalError( "Server CommTCP0: invalid handshake state" );
						}
					}
					_print_connect_info();
					_conn = true;
					rdState = 0;
       				if(!rdData.size())
					{
						rdData.alloc(_MaxBlockSize);
					}
				   _log1("done() - read handshake done");

					// Handshake done. Start to receive data packets
				}
#else
				throw PInternalError( "Client CommTCP0: invalid handshake state" );
#endif
				continue;//for( done )
			}

			if ( !wrDatasm.blocked())
				throw PInternalError( "Blocked");

			wrDatasm.deblock();
			wrDatasm.shrink();

			int wrPr;
			done &= ~0x01;

			if(wrDatasm.occupied())
			{
	             wrDatasm.block(); // block content of the writng buffer
   	             size_t wr = comm->write( wrDatasm.headptr(), wrDatasm.occupied() );
				 _countSentBytes+=wr;
		         if(wr != 0)
				 {
				   done |=0x01;
	               _log1("done() = initial write OK, occupied = %d",wrDatasm.occupied());
				   continue; // Once again to the beginning of the loop
				 }
				 else
				 {
	               _log1("done() = initial write failed, flash, occupied = %d",wrDatasm.occupied());
				   continue;
				 }
			}
			else
			{
                 _log1("done() - nothing to write initially");
			}

			for(wrPr=_COMM_MSGPRIORITIES-1; wrPr >= 0 ; --wrPr )
			{
				if(wrBlocks[wrPr].offset==0)
				{
					bool Ok = _writeFirstMsg( wrPr );
					if( Ok == false)
					{
						_log1("done writefirst - offset == 0, prior = %d, false",wrPr);
						break;
					}
					else
					{
						_log1("done writefirst - offset == 0, prior = %d, OK",wrPr);
					}

				}

				if(wrBlocks[wrPr].offset>0)
				{
					bool Ok = _writeNextMsg( wrPr );
					if( Ok == false)
					{
						_log1("done writenext - offset > 0, prior = %d, false",wrPr);
						break;
					}
					else
					{
						_log1("done writefirst - offset > 0, prior = %d, OK",wrPr);
					}
				}

				bool Flag = true;
				while( wrQueue.size()>0 && wrPr == wrQueue.peek().priority )
				{//queue has higher priority message
					_CommMsg& msg = wrQueue.peek();
					Flag = _writeFirstMsg( msg.internal.mux, msg.internal.muxChannelId, msg );
					wrQueue.pop(lastTimeInQueue);
					if( Flag ==false)
					{
						_log1("done writefirst - offset < 0, prior = %d, size = %d, false",wrPr,wrQueue.size());
						break;
					}
					else
					{
						_log1("done writefirst - offset < 0, prior = %d, size = %d, OK",wrPr,wrQueue.size());
					}
				}
				if (Flag == false)
					break;
			}

			if(wrDatasm.occupied())
			{
	             wrDatasm.block(); // block content of the writng buffer
   	             size_t wr = comm->write( wrDatasm.headptr(), wrDatasm.occupied() );
				 _countSentBytes+=wr;
		         if(wr != 0)
				 {
				   done |=0x01;
	               _log1("done() = write OK, occupied = %d",wrDatasm.occupied());
				 }
				 else
				 {
	               _log1("done() = write false, occupied = %d",wrDatasm.occupied());
				 }
			}
			else
			{
                _log1("done() - nothing to write - flushing");
				comm->flush();
			}

		} //written 
		if(done&0x02)
		{//read
//PTRACE( "_DBG: TCP0:doneRead" );
			UINT32 maxRead = SYSTEMTICKS() - _lastRead ;
			if (maxRead > _maxRead )
				_maxRead = maxRead;
			_lastRead = SYSTEMTICKS();
			
			if( !_conn )
			{
    			done &= ~0x02;
				if( !_inbound )
				{//client
					switch(helloRdState)
					{
					case 2:
						{
							if( hello[0] < _MinProtocolSupported)
							{
							    // This client doesn't support such an old protocol
						        throw PInternalError( "Client CommTCP0: server protocol is not supported" );
							}
						    else if(hello[0] > _MaxProtocolSupported)
							{
						        throw PInternalError( "Client CommTCP0: server protocol is not supported");
							}
						    else
							{
							    _ProtocolSelected=hello[0];
							}
							helloRdState = 3;
							if (readServerHello())
							{
    							if(_CompressBitsSize)
								{
    								// if 0 it means that we don't need compression at all
    								comm->setCompression( _CompressBitsSize == ALLOWED_COMPRESS_BITS_SIZE_SECOND ? _CommCompressedCommunicatorTypes::compressionLzhl:_CommCompressedCommunicatorTypes::compressionLzhl1,_MaxBlockSize);
								}
	   							// Handshake done. Start to receive data packets
								_print_connect_info();
    							_conn = true;
    							rdState = 0;
                        		if(!rdData.size())
								{
            						rdData.alloc(_MaxBlockSize);
								}
								_log1("done() - write handshake done");
							}
							else
							{
								writeClientKeyExchange();
				    			size_t wr = comm->write( hello, 52 );
								_countSentBytes+=wr;
					    		if( wr != 0 )
						    		done |= 0x01;
							}
						}
						break;
					case 4:
						{
							if(_ProtocolSelected==3)
							{
								throw PInternalError( "Client CommTCP0: invalid handshake state" );
							}
							helloRdState = 5;
							readServerKeyExchange();
							if(!_CompressBitsSize)
							{
								throw PInternalError( "Client CommTCP0: invalid handshake state" );
							}

							comm->setEncryption(encryption);
 							comm->setCompression( _CompressBitsSize == ALLOWED_COMPRESS_BITS_SIZE_SECOND ? _CommCompressedCommunicatorTypes::compressionLzhl:_CommCompressedCommunicatorTypes::compressionLzhl1,_MaxBlockSize);
   							// Handshake done. Start to receive data packets
							_print_connect_info();
   							_conn = true;
   							rdState = 0;
                       		if(!rdData.size())
							{
           						rdData.alloc(_MaxBlockSize);
							}
							_log1("done() - write handshake done");
						}
						break;
					default:
						{
							throw PInternalError( "Client CommTCP0: invalid handshake state" );
						}
					}
				}
				else
#ifndef COMM_CLIENT_ONLY
				{//server
					switch(helloRdState)
					{
					case 1:
						{
    						if( hello[0] < _MinProtocolSupported)
							{
							    // This server doesn't support such an old protocol
						        throw PInternalError( "Server CommTCP0: client protocol is not supported" );
							}
						    else if(hello[0] > _MaxProtocolSupported)
							{
						        throw PInternalError( "Server CommTCP0: client protocol is not supported");
							}
							_ProtocolSelected=hello[0];
							_MaxBlockSize = _ProtocolSelected==5 ? _MaxBlockSizeStoredVer5:_MaxBlockSizeStored;
							sizeOfHeader = _ProtocolSelected == 5 ? _SizeOfShortHeaderVer5:_SizeOfShortHeader;
							readClientHello();
							prepareServerHello();
							size_t wr;
							if (_ProtocolSelected > 3)
							{
								_log1("done() - write hello 48");
			    				wr = comm->write( hello, 48 );
							}
							else
							{
								_log1("done() - write hello 42");
			    				wr = comm->write( hello, 42 );
							}
							_countSentBytes+=wr;
				    		if( wr != 0 )
					    		done |= 0x01;
    						helloRdState = 2;
						}
						break;
					case 3:
						{
							if(_ProtocolSelected==3)
							{
								throw PInternalError( "Server CommTCP0: invalid handshake state" );
							}
							readClientKeyExchange();
							writeServerKeyExchange();
								_log1("done() - write hello 24");
			    			size_t wr = comm->write( hello, 24 );
							_countSentBytes+=wr;
				    		if( wr != 0 ) 
					    		done |= 0x01;
    						helloRdState = 4;
						}
						break;

					default:
						{
							throw PInternalError( "Server CommTCP0: invalid handshake state" );
						}
					} // switch
				} // else -server
#else
				throw PInternalError( "Client CommTCP0: invalid handshake state" );
#endif

				continue;//for( done )
			} // if (!_conn)

//PTRACE( "Read" );

			done &= ~0x02;
			switch(rdState)
			{
			case 0:
				{
					_receivedPackets++;
#ifndef COMM_CLIENT_ONLY
					if (_protectionCounter && _protectionCounter < _receivedPackets)
						throw PInternalError( "Stream0ProtocolHandler: Read - protection counter" );				
#endif
					UINT32 sz;
					const BYTE *p = rdData.ptr();
					if (_ProtocolSelected == 5)
					{					
    					sz = CommMsgBody::readUINT32( p );
	    				p+=4;
					}
					else
					{
    					sz = CommMsgBody::readUINT16( p );
	    				p+=2;
					}
	    			if( sz < sizeOfHeader || sz > _MaxBlockSize)
					{

						_log("sz = %d, max = %d",sz,_MaxBlockSize);
						throw PInternalError( "Stream0ProtocolHandler: Read - invalid block size" );				
					}
	//PTRACE( "_DBG: TCP0:read( %d )", sz );
					size_t rd = comm->read( rdData.ptr()+ sizeOfHeader, sz-sizeOfHeader );
					rdState = 1;
					_countReceivedBytes+=rd;
	    			if( rd != 0 )
					{
	                    _log1("done() - read, state 0, read - OK");
		    			done |= 0x02;
					}
					else
					{
	                    _log1("done() - read, state 0, read - false");
					}
				}
				break;
			case 1:
				{
					_log1("done() - read, state 1");
					UINT32 sz;
					const BYTE *p = rdData.ptr();
					if (_ProtocolSelected == 5)
					{					
    					sz = CommMsgBody::readUINT32( p );
	    				p+=4;
					}
					else
					{
    					sz = CommMsgBody::readUINT16( p );
	    				p+=2;
					}

   					bool   isFirst = ( *p & 0x80 ) != 0;
    				bool   isLast  = ( *p & 0x40 ) != 0;
	    			BYTE   type    = ( *p & 0x3f );
    				UINT32 szMsg = 0;
	   				p++;
	    			if(isFirst)
					{
						if (!isLast)
						{
							if ( p - rdData.ptr() +  4 > sz)
							{
								throw PInternalError( "Stream0ProtocolHandler: End of first block of message" );
							}
    						szMsg   = CommMsgBody::readUINT32(p);
       						p+=4;
						}
						else
						{
							szMsg = sz - sizeOfHeader;
						}
					}
#ifndef COMM_CLIENT_ONLY
					if (_maximumMessageSize && szMsg > _maximumMessageSize)
					{
						if (_maxMessageSizeDebug)
						{
							PLog("Stream0ProtocolHandler: Message size exceeds limit %u, msg size = %u",_maximumMessageSize,szMsg);
						}
						else
						{
							PString er = "Stream0ProtocolHandler: Message size exceeds limit ";
							er.appendInt(_maximumMessageSize).append(", msg size = ").appendInt(szMsg);
							throw PInternalError( er );
						}
					}
					if (_maximumMessageRate && _maximumMessageRatePeriod)
					{
						bool exceed = false;
						UINT32 diff = 0;
						if (!_mRateCounterInit)
						{
							_mRateTimer = SYSTEMTICKS();
							_mRateCounter = 1;
							_mRateCounterInit = true;
						}
						else
						{
							diff = SYSTEMTICKS() - _mRateTimer;
							if (diff > _maximumMessageRatePeriod)
							{
								_mRateTimer =  SYSTEMTICKS();
								_mRateCounter = 1;
							}
							else
							{
								_mRateCounter++;
								exceed = _mRateCounter > _maximumMessageRate;
							}
						}
						if (exceed)
						{
							if (_maxMessageRateDebug)
							{
								PLog("Stream0ProtocolHandler: Message rate exceeds limit %u per %u ms: %u messages in %u ms", _maximumMessageRate, _maximumMessageRatePeriod, _mRateCounter, diff);
								_mRateTimer =  SYSTEMTICKS();
								_mRateCounter = 1;
							}
							else
							{
								PString er = "Stream0ProtocolHandler: Message rate exceeds limit ";
								er.appendInt(_maximumMessageRate).append(" per ").appendInt(_maximumMessageRatePeriod).
								   append(" ms: ").appendInt(_mRateCounter).append(" messages in ").appendInt(diff).append(" ms");
								throw PInternalError( er );
							}
						}
					}
#endif
					if (type == FRAME_DATA_TYPE)
					{
						const BYTE *dataStart = p;
						if ( p - rdData.ptr() + 1 > sz)
						{
							throw PInternalError( "Stream0ProtocolHandler: End of data frame header" );
						}
						BYTE b = *p++;
    					BYTE priority = b & 0x0F;
	    				if( priority >= _COMM_MSGPRIORITIES )
    	    				throw PInternalError( "Stream0ProtocolHandler: Invalid received msg priority" );

    					if( isFirst )
						{
		    				_Read& msg = rdMsgs[ priority ];
							if(msg.size)
								throw PInternalError( "Stream0ProtocolHandler: Unexpected first block");

							if ( p - rdData.ptr() + 3 > sz)
							{
								throw PInternalError( "Stream0ProtocolHandler: End of data frame header of the first block" );
							}
						
    						UINT16 flags = CommMsgBody::readUINT16( p );
    						p += 2;
	    					BYTE type = *p++;
    						msg.msg.priority = priority;
    						msg.msg.flags = flags;
	    					msg.msg.type = type;
    						if( muxEnabled )
							{
								if ( p - rdData.ptr() + 6 > sz)
								{
									throw PInternalError( "Stream0ProtocolHandler: End of data frame header of the first block (mux)" );
								}
								msg.msg.internal.mux = CommMsgBody::readUINT16( p );
								p += 2;
 								msg.msg.internal.muxChannelId = CommMsgBody::readUINT32( p );
								p += 4;
							}
							else
							{
								msg.msg.internal.mux = 0;
								msg.msg.internal.muxChannelId = 0;
							}
							if (szMsg <= p - dataStart)
								throw PInternalError( "Stream0ProtocolHandler: Invalid message size");
							msg.size = szMsg - (p - dataStart);
							msg.msg.body._alloc(msg.size);
							msg.msg.body.clear();
							sz -= p - rdData.ptr();
							if(msg.msg.body._size() + sz > msg.size)
								throw PInternalError( "Stream0ProtocolHandler: Message exceeds declared size");
							memcpy( msg.msg.body._append(sz), p, sz);
						}
    					else
						{
							sz -=  p - rdData.ptr();
    						_Read& msg = rdMsgs[ priority ];
							if(!msg.size)
								throw PInternalError( "Stream0ProtocolHandler: Unexpected next block");
							if(msg.msg.body._size() + sz > msg.size)
								throw PInternalError( "Stream0ProtocolHandler: Message exceeds declared size");
	    					memcpy( msg.msg.body._append( sz ), p, sz );
						}
	
						if( isLast )
						{
							_Read& msg = rdMsgs[ priority ];
							_log1("message sent to server type %x",msg.msg.type);
							if(msg.msg.body._size() != msg.size)
							{
								PLog("Stream0ProtocolHandler: Actual message size %u differs from declared size %u",(UINT32)msg.msg.body._size(),(UINT32) msg.size);
							}
							msgHandler->msg( msg.msg.internal.mux, msg.msg.internal.muxChannelId, msg.msg );
							msg.msg.body._dealloc();
							msg.size=0;
						}
					}
					else
					{
						// Now only keepalive messages here
						if ( p - rdData.ptr() + 6 > sz)
						{
							throw PInternalError( "Stream0ProtocolHandler: End of keepalive message" );
						}

						const BYTE *pold = p;
						BYTE count = *p++;
						( void )count;
						BYTE inb = *p++;
						( void )inb;
						UINT32 tm = CommMsgBody::readUINT32( p );
						( void )tm;
						if(_inbound /* && inb == 1 || !_inbound && inb==0*/)
						{
#ifndef COMM_CLIENT_ONLY
							if (!inb)
								throw PInternalError( "Stream0ProtocolHandler: Invalid keepalive header" );
							// our message
							_LastAckTimeReceived = SYSTEMTICKS();
							_LastPacketReceived = count;
							_LastRoundTrip=_LastAckTimeReceived-tm;
							if (_currentAckSent < 0)
							{
								_log("!!! Acknowledge received but counter -1, round trip=%u, Avg=%u",_LastRoundTrip,_AvgRoundTrip); 
								throw PInternalError( "Stream0ProtocolHandler: Unexpected keepalive acknowledge" );
							}
							else if (tm != _SentAckTimeHistory[0])
							{
								_log("!!! Acknowledge received but sent time %u doesn't correspond history %u, counter %d, round trip=%u, Avg=%u",tm,_SentAckTimeHistory[0],_currentAckSent,_LastRoundTrip,_AvgRoundTrip); 
								throw PInternalError( "Stream0ProtocolHandler: Invalid keepalive acknowledge" );
							}
							else if (--_currentAckSent >=0)
							{
								memmove(_SentAckTimeHistory,_SentAckTimeHistory+1,(_currentAckSent+1) * sizeof(INT32));
							}

							if (_beenLogged)
							{
								if(_MaxRoundTrip <_LastRoundTrip)
								{
									_log1("The latest round trip too large=%u, counter=%d, Avg=%u", _LastRoundTrip,_currentAckSent,_AvgRoundTrip); 
									_MaxRoundTrip = _LastRoundTrip;
								}
								if( _LastRoundTrip <= CUT_DELTA_TIME)
								{
									_log1("Connection restored, latest round trip %u, counter=%d, Avg=%u, Max=%u", _LastRoundTrip,_currentAckSent,_AvgRoundTrip,_MaxRoundTrip); 
									_beenLogged=false;
									_MaxRoundTrip=0;
									if (quality_required)
									{
										CommMsgBody msg_body_;
										msg_body_.composeINT32(_TRANSPORT_KEEPALIVE_RESTORED);
										msgHandler->serviceCall(_SERVICE_REQUEST_QUALITY,&msg_body_);
									}
								}
							}
							else
							{
								if(_LastRoundTrip>_keepAliveWrite)
								{
									_log1("The latest round trip too large=%u, logging lost, counter=%d, Avg=%u", _LastRoundTrip,_currentAckSent,_AvgRoundTrip); 
								}
							}

							if(keep_alive_forced)
							{
								if(count == _LastKeepAlivedForced)
								{
									_log("Answer on keepAlive request #%u received, round trip=%u, Avg=%u", count, _LastRoundTrip,_AvgRoundTrip); 
									msgHandler->serviceCall(_SERVICE_REQUEST_KEEP_ALIVE,0);
									keep_alive_forced=false;
								}
							}
							_AvgRoundTrip = _AvgRoundTrip ? (_AvgRoundTrip + _LastRoundTrip)/2 : _LastRoundTrip;
#endif
						}
						else
						{
							if(_writeLowLevel(pold,6))
							{
								_log1("keep alive replay - OK");
								done|=0x01;
							}
							else
							{
								_log1("keep alive replay - false");
							}

						}
					}

					rdState = 0;
					size_t rd = comm->read( rdData.ptr(), sizeOfHeader ); // Read standart message header
					_countReceivedBytes+=rd;
					if( rd != 0 )
					{
						done |= 0x02;
						_log1("done() - read, state 1, read - OK");
					}
					else
					{
						_log1("done() - read, state 1, read - false");
					}

					break;
				}
			}
		}//else read
	}//forever
	_log1("leave done()");
}

template< class _Communicator, class _MsgHandler >
bool _CommStream0ProtocolHandler< _Communicator, _MsgHandler >::_writeFirstMsg( int mux, UINT32 muxChannelId, _CommMsg& msg )
{
	_Write& wr = wrBlocks[ msg.priority ];
	if ( !(wr.offset < 0))
		throw PInternalError( "Offset");
	if(_ProtocolSelected < 3)
		throw PInternalError( "Invalid protocol");

    wr.msg.moveFrom( msg );
    wr.offset = 0;
    wr.msg.internal.mux = mux;
	wr.msg.internal.muxChannelId = muxChannelId;
	return _writeFirstMsg(msg.priority);
}



template< class _Communicator, class _MsgHandler >
bool _CommStream0ProtocolHandler< _Communicator, _MsgHandler >::_writeFirstMsg(int priority)
{
	_Write& wr = wrBlocks[ priority ];
	if ( !(wr.offset >= 0))
		throw PInternalError( "Offset");

	if(_ProtocolSelected < 3)
		throw PInternalError( "Invalid protocol");

	bool isLast = true;
	if(wr.msg.type == _COMM_MSGTYPE_LOWLEVEL)
	{
		size_t size = wr.msg.body._size();
    	BYTE* p = _prepareFrameHeader(size,FRAME_INTERNAL_TYPE,isLast,true);
    	if(p)
		{
    		memcpy(p,wr.msg.body._readPtr(),wr.msg.body._size());
			p+=wr.msg.body._size();
	        wrDatasm.update_end(p);
    		wr.msg.body._dealloc();
	    	wr.offset = -1;
			return true;
		}
		else
			return false;
	}

    size_t headerSize = 1 + 2 + 1;  // Prority + Flags + message type
	if( muxEnabled )
		headerSize += 6;
	size_t packetSize = headerSize + wr.msg.body._size();

	BYTE* p = _prepareFrameHeader(packetSize,FRAME_DATA_TYPE,isLast,true);
	if(!p)
	{
		// not enough space in write buffer
		// leave this message in wrBlocks
		// and try later, from done()
//		wrPriority=-1;
		return false;
	}

	*p++ = wr.msg.priority & 0x0f;
	CommMsgBody::writeUINT16( p, wr.msg.flags );
	p += 2;
	*p++ = wr.msg.type;

	if( muxEnabled )
	{
		int mux = wr.msg.internal.mux;
		UINT32 muxChannelId = wr.msg.internal.muxChannelId; 
		CommMsgBody::writeUINT16( p, mux );
		p += 2;
		CommMsgBody::writeUINT32( p, muxChannelId );
		p += 4;
	}

	size_t bodySz = packetSize - headerSize;
	if( wr.msg.internal.flags & _CommMsg::Patch8 )
	{
		if ( !(bodySz >= 8))
			throw PInternalError( "Body size");

		CommMsgBody::writeUINT32( p, wr.msg.internal.patch0 );
		p += 4;
		CommMsgBody::writeUINT32( p, wr.msg.internal.patch1 );
		p += 4;
		memcpy( p, wr.msg.body._readPtr() + 8, bodySz - 8 );
		p+=bodySz -8;
	}
	else
	{
		memcpy( p, wr.msg.body._readPtr(), bodySz );
		p+=bodySz;
	}
	wrDatasm.update_end(p);
	

	if( isLast )
	{
//		wrPriority = -1; // we can try to write the next message
		wr.msg.body._dealloc();
		wr.offset = -1;
	}
	else
	{
//	    wrPriority = wr.msg.priority;
		wr.offset = static_cast< int >( bodySz );
		return false;
	}
	return true;
}

template< class _Communicator, class _MsgHandler >
BYTE * _CommStream0ProtocolHandler< _Communicator, _MsgHandler >::_prepareFrameHeader(size_t &packetsize, BYTE packettype, bool &isLast, bool isFirst)
{
	if(!wrDatasm.size())
	{
		wrDatasm.alloc(_MaxBlockSize);
	}

	int hdrSize = sizeOfHeader;
	if(isFirst)
	{
		// Use extended header to pass the size of the whole message
		hdrSize+=4;
	}
	size_t size = packetsize;
	size_t sz = size + hdrSize;
	isLast = true;
	if( sz> _MaxBlockSize )
	{
		sz = _MaxBlockSize;
		packetsize = _MaxBlockSize - hdrSize;
		isLast = false;
	}
	else
	{
		if( isFirst)
		{
			// adjust 
		    hdrSize-=4;
			sz-=4;
		}
	}

	if(sz > wrDatasm.rest())
		return 0;

	BYTE* p = wrDatasm.endptr();
	if (_ProtocolSelected == 5)
	{
		CommMsgBody::writeUINT32( p, static_cast< UINT32 >( sz ) );
		p += 4;
	}
	else
	{
		CommMsgBody::writeUINT16( p, static_cast< UINT16 >( sz ) );
		p += 2;
	}

	*p++ = (isFirst ? 0x80 : 0x00) | (isLast ? 0x40 : 0x00) | (packettype & 0x3f);
	if(isFirst && !isLast)
	{
		CommMsgBody::writeUINT32( p, static_cast< UINT32 >( size ) );
		p+=4;
	}
	wrDatasm.update_end(p);
	_sentPackets++;
	return p;
}


template< class _Communicator, class _MsgHandler >
bool _CommStream0ProtocolHandler< _Communicator, _MsgHandler >::_writeNextMsg(int priority)
{
	_Write& wr = wrBlocks[ priority ];

	if ( !(wr.offset > 0 ))
		throw PInternalError( "Offset");

	if ( !(wr.msg.body._size() >= wr.offset ))
		throw PInternalError( "Offset");
				
	if(_ProtocolSelected < 3)
		throw PInternalError( "Invalid protocol");

   	bool isLast = true;
    size_t left = wr.msg.body._size() - wr.offset + 1;

	BYTE *p = _prepareFrameHeader(left,FRAME_DATA_TYPE,isLast,false);
	if(!p)
	{
		return false;
	}

	*p++ = wr.msg.priority & 0x0f;
	memcpy( p, wr.msg.body._readPtr() + wr.offset, left-1 );
	p+=(left-1);
	wrDatasm.update_end(p);
	if( isLast )
	{
		wr.msg.body._dealloc();
		wr.offset = -1;
	}
	else
	{
		wr.offset += (left-1);
		return false;
	}
	return true;
}

template< class _Communicator, class _MsgHandler >
void _CommStream0ProtocolHandler< _Communicator, _MsgHandler >::writeMsg( int mux, UINT32 muxChannelId, _CommMsg& msg )
{
//PTRACE( "WRITEMSG" );
	if ( !(_init && _conn ))
		throw PInternalError( "Not initialized");

	if(_ProtocolSelected < 3)
		throw PInternalError( "Invalid protocol");

	if ( !(msg.priority < _COMM_MSGPRIORITIES ))
		throw PInternalError( "Priority");

	_log1("write msg new, type =%x",msg.type);
#ifndef COMM_CLIENT_ONLY
	if(_flag_keep_alive_set)
	{
	    _log1("write msg new - flag keepalive set, sending packet");
        UINT32 now = SYSTEMTICKS();
		BYTE packet[6];
	    packet[0] = _LastPacketSent++;
		packet[1] = _inbound ? 1:0;
	    CommMsgBody::writeUINT32( packet+2, now);
		if (_currentAckSent >= (MAX_SENT_HISTORY-1))
		{
			_log("_writeMsg  keepAlive - sent history exceeded! counter=%d, now=%u Timeout = %u, lastReceived=%u, last round trip=%u, Avg=%u",
				        _currentAckSent, now, now-_SentAckTimeHistory[0], _lastRead, _LastRoundTrip,_AvgRoundTrip);
			throw PInternalError( "Stream0ProtocolHandler: keepalive timeout exceeds limits");
		}
		else
		{
			_LastAckTimeSent = _SentAckTimeHistory[++_currentAckSent]= now;
		}

		_flag_keep_alive_set = false;
		if(_writeLowLevel(packet,6))
		{ // deblock datas in writing buffer
			wrDatasm.deblock();
			wrDatasm.shrink();
		}
	}
#endif

	if(wrBlocks[msg.priority].offset>=0)
	{
		msg.internal.mux = mux;
		msg.internal.muxChannelId = muxChannelId;
		wrQueue.push( msg );
		_log1("new message push message");
	}
	else
	{
		bool done;
    	_CommMsg msg2;

    	if( wrQueue.size() == 0 || msg.priority > wrQueue.peek().priority )
		{
			msg2.moveFrom(msg);
			_log1("new message copy message, prior = %d",msg.priority);
		}
	    else
		{
			int prior;
		    msg.internal.mux = mux;
		    msg.internal.muxChannelId = muxChannelId;
		    wrQueue.push( msg );
			if(wrBlocks[prior=wrQueue.peek().priority].offset <0)
			{
				msg2.moveFrom(wrQueue.peek());
			    wrQueue.pop(lastTimeInQueue);
			    _log1("new message pop message, prior = %d, size = %d",prior,wrQueue.size());
			}
			else
			{
			    _log1("new message - wait for high priority message");
				return;
			}

		}
		while(1)
		{
			if( _writeFirstMsg( msg2.internal.mux, msg2.internal.muxChannelId, msg2 ) == false)
			{
			   _log1("new message _writeFirst = false, prior = %d",msg2.priority);
				break;
			}
			if(wrQueue.size() && wrBlocks[wrQueue.peek().priority].offset <0)
			{
			    _log1("new message next message, prior = %d, size = %d",wrQueue.peek().priority,wrQueue.size());
				msg2.moveFrom(wrQueue.peek());
	   			wrQueue.pop(lastTimeInQueue);
			}
			else
				break;
		}
		if(wrDatasm.blocked()==false && wrDatasm.occupied())
		{
			wrDatasm.block(); // block content of the writng buffer
      		size_t wr = comm->write( wrDatasm.headptr(), wrDatasm.occupied() );
			_countSentBytes+=wr;
			done = wr != 0;
			if(done)
			{
				_log1("new message write = OK, occupied = %d",wrDatasm.occupied());
				_doneWrite();
			}
			else
				_log1("new message write -false , occupied = %d",wrDatasm.occupied());

		}
		else
			_log1("new message data blocked or empty : occupied = %d",wrDatasm.occupied());
	}
}

template< class _Communicator, class _MsgHandler >
void _CommStream0ProtocolHandler< _Communicator, _MsgHandler >::storeMsg( int mux, UINT32 muxChannelId, _CommMsg& msg )
{
	if ( !(_init && _conn ))
		throw PInternalError( "Not initialized");
	if(_ProtocolSelected < 3)
		throw PInternalError( "Invalid protocol");
	if ( !(msg.priority < _COMM_MSGPRIORITIES ))
		throw PInternalError( "Priority");
    msg.internal.mux = mux;
    msg.internal.muxChannelId = muxChannelId;
    wrQueue.push( msg );
	_log1("new message stored for delivery, queue size=%d ",wrQueue.size());
}

template< class _Communicator, class _MsgHandler >
void _CommStream0ProtocolHandler< _Communicator, _MsgHandler >::flushStoredMsgs()
{
//PTRACE( "WRITEMSG" );
	if ( !(_init && _conn ))
		throw PInternalError( "Not initialized");
	if(_ProtocolSelected < 3)
		throw PInternalError( "Invalid protocol");

	if (wrQueue.size())
	{
    	_CommMsg msg;
#ifndef COMM_CLIENT_ONLY
		if(_flag_keep_alive_set)
		{
			_log1("flush msg - flag keepalive set, sending packet");
			UINT32 now = SYSTEMTICKS();
			BYTE packet[6];
			packet[0] = _LastPacketSent++;
			packet[1] = _inbound ? 1:0;
			CommMsgBody::writeUINT32( packet+2, now);
			if (_currentAckSent >= (MAX_SENT_HISTORY-1))
			{
				_log("_flushMsg keepAlive - sent history exceeded! counter=%d, Now=%u, Timeout = %u, lastReceived=%u, last round trip=%u, Avg=%u",
							_currentAckSent, now, now-_SentAckTimeHistory[0], _lastRead, _LastRoundTrip,_AvgRoundTrip);
				throw PInternalError( "Stream0ProtocolHandler: keepalive timeout exceeds limits");
			}
			else
			{
				_LastAckTimeSent = _SentAckTimeHistory[++_currentAckSent]= now;
			}

			_flag_keep_alive_set = false;
			if(_writeLowLevel(packet,6))
			{ // deblock datas in writing buffer
				wrDatasm.deblock();
				wrDatasm.shrink();
			}
		}
#endif
		if(wrBlocks[wrQueue.peek().priority].offset <0)
		{
			msg.moveFrom(wrQueue.peek());
		    wrQueue.pop(lastTimeInQueue);
			while(1)
			{
				if( _writeFirstMsg( msg.internal.mux, msg.internal.muxChannelId, msg ) == false)
				{
				   _log1("flush _writeFirst = false, prior = %d",msg.priority);
					break;
				}
				if(wrQueue.size() && wrBlocks[wrQueue.peek().priority].offset <0)
				{
					_log1("flush next message, prior = %d, size = %d",wrQueue.peek().priority,wrQueue.size());
					msg.moveFrom(wrQueue.peek());
	   				wrQueue.pop(lastTimeInQueue);
				}
				else
					break;
			}
			if(wrDatasm.blocked()==false && wrDatasm.occupied())
			{
				wrDatasm.block(); // block content of the writng buffer
      			size_t wr = comm->write( wrDatasm.headptr(), wrDatasm.occupied() );
				_countSentBytes+=wr;
				if(wr != 0)
				{
					_log1("flush write = OK, occupied = %d",wrDatasm.occupied());
					_doneWrite();
				}
				else
					_log1("flush write -false , occupied = %d",wrDatasm.occupied());

			}
			else
				_log1("flush data blocked or empty : occupied = %d",wrDatasm.occupied());
		}
	}
}


template< class _Communicator, class _MsgHandler >
bool _CommStream0ProtocolHandler< _Communicator, _MsgHandler >::_writeLowLevel(const BYTE * data, size_t size)
{
//PTRACE( "WRITEMSG" );
	if ( !(_init && _conn ))
		throw PInternalError( "Not initialized");

	if ( _ProtocolSelected < 3 )
		throw PInternalError( "Invalid protocol");

	if ( !(size<=sizeOfHeader+_MaxBlockSize ))
		throw PInternalError( "Size");

	bool isLast;
	BYTE* p = _prepareFrameHeader(size,FRAME_INTERNAL_TYPE,isLast,true);
	if(p)
	{
		memcpy(p,data,size);
		p+=size;
	    wrDatasm.update_end(p);
	    if(wrDatasm.blocked() == false)
		{
		    wrDatasm.block(); // block content of the writng buffer

			size_t wr = comm->write( wrDatasm.headptr(), wrDatasm.occupied() );
			_countSentBytes+=wr;
			if (wr != 0)
			{
				_log1("low level write OK");
				return true;
			}
			else
			{
				_log1("low level write false");
				return false;
			}
		}
		else
		{
            _log1("low level data blocked");
		}
	}
	else
	{
		_CommMsg msg(_COMM_MSGTYPE_LOWLEVEL,_COMM_MSGPRIORITIES-1);
		memcpy(msg.body._append(size),data,size);
	    wrQueue.push( msg );
		_log1("low level push message, size=%d",wrQueue.size());
	}
	return false;
}


template< class _Communicator, class _MsgHandler >
void _CommStream0ProtocolHandler< _Communicator, _MsgHandler >::writeLowLevel(const BYTE * data, size_t size)
{
//PTRACE( "WRITEMSG" );
	if ( !(_init && _conn ))
		throw PInternalError( "Not initialized");

	if ( _ProtocolSelected < 3 )
		throw PInternalError( "Invalid protocol");
	if (_writeLowLevel(data,size))
		_doneWrite();
}



template< class _Communicator, class _MsgHandler >
bool _CommStream0ProtocolHandler< _Communicator, _MsgHandler >::keepAlive ( UINT32& readFreeze )
	{
	if ( !_init)
		throw PInternalError( "Not initialized");
	if(_ProtocolSelected < 3)
		throw PInternalError( "Invalid protocol");

	UINT32 now = SYSTEMTICKS();
	if(_inbound && _keepAliveWrite != 0)
	{
#ifndef COMM_CLIENT_ONLY
		if(_flag_keep_alive_set)
		{
			_log1("keepAlive - flag has been set, send keep_alive");
			BYTE packet[6];
			if (_currentAckSent >= (MAX_SENT_HISTORY-1))
			{
				// Very long time out;
				_log("keepAlive_new: Sent ack history exceeded. Now=%u, Timeout = %u, LastReceived=%u, last round trip=%u, Avg=%u",
					        now, now-_SentAckTimeHistory[0], _lastRead, _LastRoundTrip,_AvgRoundTrip);
				return false;
			}

			_LastAckTimeSent=_SentAckTimeHistory[++_currentAckSent] = now;
		    packet[0] = _LastPacketSent++;
			packet[1] = _inbound ? 1:0;
		    CommMsgBody::writeUINT32( packet+2, now);
			writeLowLevel(packet,6);
			_flag_keep_alive_set = false;
		}
		else if( now - _LastAckTimeSent >= _keepAliveWrite * TICKS_PER_SECOND / 1000 )
		{
			_log1("keepAlive - set the flag");
			_flag_keep_alive_set = true;
		}
#endif

	}

	if( _keepAliveRead != 0 )
	{
		UINT32 delta;
		UINT32 mx = _keepAliveRead * TICKS_PER_SECOND / 1000;
		UINT32 freezeBase;
		UINT32 mn;
#ifndef COMM_CLIENT_ONLY
		if(_inbound)
		{
			if(_conn && _LastAckTimeSent)
			{
				// Connection is established, we have sent at least one keep alive
				if (_currentAckSent==-1)
				{
					//We have acknowledged keepalive)
					delta = _LastAckTimeReceived - _LastAckTimeSent;
				}
				else
				{
					delta = now - _SentAckTimeHistory[0];
				}
			}
			else
			{
				// Just established new connection, 
				delta = now - _lastRead;
			}
			delta = delta*TICKS_PER_SECOND / 1000;
			if( delta > mx )
			{
				if ( !_keepAliveIgnoreDataPackets && (now - _lastRead) < _keepAliveRead )
				{
					// Protection against slow connections. We don't want to break connection
					// if we are still receiving something from client.
					return true;
				}
				_log("Timeout %u, expired. Counter=%d, last round trip=%u, Avg=%u",delta,_currentAckSent,_LastRoundTrip,_AvgRoundTrip);
				return false;
			}

			if (_beenLogged == false && delta > CUT_DELTA_TIME && _currentAckSent >= 0)
			{
				_beenLogged = true;
				_MaxRoundTrip=0;
                _log1("KeepAlive is delayed on %u, last=%u, Avg=%u", delta,_LastRoundTrip,_AvgRoundTrip); 
				_numberOfDelays++;
				if (quality_required)
				{
					CommMsgBody msg_body_;
					msg_body_.composeINT32(_TRANSPORT_KEEPALIVE_DELAYED);
				    msgHandler->serviceCall(_SERVICE_REQUEST_QUALITY,&msg_body_);
				}
			}
			freezeBase = _keepAliveWrite /*sic! write*/ * TICKS_PER_SECOND / 1000;
			mn = ( _keepAlivePoll + _keepAliveWrite ) * TICKS_PER_SECOND / 1000;

		}
		else
#endif
		{
			delta = now - _lastRead;
			delta = delta*TICKS_PER_SECOND / 1000;
			if( delta > mx )
			{
				_log("Timeout %u, expired. Last round trip=%u, Avg=%u",delta, _LastRoundTrip,_AvgRoundTrip);
				return false;
			}

			freezeBase = (_keepAliveWrite +_keepAlivePoll)/*sic! write*/ * TICKS_PER_SECOND / 1000;
			mn = ( _keepAlivePoll*2 + _keepAliveWrite ) * TICKS_PER_SECOND / 1000;
		}

		_lastDelta = delta;
		int currentQuality;//assuming that peer _keepAlive* parameters are the same as ours

		if( delta < freezeBase )
			readFreeze = 0;
		else
			readFreeze = delta - freezeBase;

		if( delta < mn )
			currentQuality = 10000;
		else
		{
			if (mx <= mn)
			{
				currentQuality = 10000 - delta * 10000 / mx;
			}
			else
			{
				UINT32 x = delta - mn;
				currentQuality = 10000 - x * 10000 / ( mx - mn );
			}
		}

		if( currentQuality < _quality )
			_quality = currentQuality;
		else
		{
			int recovery = _QualityRecovery * ( now - lastQuality ) / TICKS_PER_SECOND;
			if( recovery < 0 )
				recovery = 0;
			else if( recovery > _QualityRecovery )
				recovery = _QualityRecovery;
			_quality = ( ( _quality * ( 10000 - recovery ) ) + ( currentQuality * recovery ) ) / 10000;
		}
		if (_quality < 0)
			_quality =0;
		else if (_quality>10000)
			_quality = 10000;
		if(quality_required && (_quality/100) != (_LastGottenQuality/100))
		{
			CommMsgBody msg_body_;
    		msg_body_.composeINT32(_quality/100);
            msgHandler->serviceCall(_SERVICE_REQUEST_QUALITY,&msg_body_);
		}
		_LastGottenQuality = _quality;

		lastQuality = now;
//PTRACE( "CurrentQuality/Quality: %d/%d", currentQuality, quality );
	}
	else
		readFreeze = 0;

	return true;
}

template< class _Communicator, class _MsgHandler >
bool _CommStream0ProtocolHandler< _Communicator, _MsgHandler >::serviceCallTransport(UINT32 id, CommMsgBody *msg_body)
{

	switch(id)
	{
	case _SERVICE_REQUEST_QUALITY:
		{
		quality_required=true;
		CommMsgBody msg_body_;
		msg_body_.composeINT32(_quality/100);
        msgHandler->serviceCall(_SERVICE_REQUEST_QUALITY,&msg_body_);
		}
		break;
	case  _SERVICE_REQUEST_KEEP_ALIVE:
		{
			BYTE level=0;
			if (msg_body)
			{
				CommMsgParser parser(*msg_body);
				if (parser.parseEnded() == false)
					parser.parseBYTE(level);
			}
			
			printStatistic(level);
			UINT32 readFreeze;
			bool Ok = keepAlive(readFreeze);
			if (Ok == false)
				return false;
			if(msg_body)
			{
				msg_body->composeUINT32(readFreeze);
			}
			return true;
		}
		break;
	case  _SERVICE_REQUEST_STATISTIC:
		{
			if(msg_body)
			{
				msg_body->composeUINT32( static_cast< UINT32 >( wrQueue.size() ) ).
					      composeUINT32(lastTimeInQueue);
				lastTimeInQueue = 0;

			}
			return true;
		}
		break;

	case _SERVICE_REQUEST_FORCE_KEEP_ALIVE:
#ifndef COMM_CLIENT_ONLY
		_lastReadWhenForced = SYSTEMTICKS();

		if(_ProtocolSelected>=3)
		{
			BYTE packet[6];
		    _LastKeepAlivedForced = packet[0] = _LastPacketSent++;
			packet[1] = _inbound ? 1:0;
		    CommMsgBody::writeUINT32( packet+2, _lastReadWhenForced);
			if (_currentAckSent >= (MAX_SENT_HISTORY-1))
			{
				_log("Force keepAlive - sent history exceeded! count =%d, now=%u,  Timeout=%u, lastReceived=%u, last round trip=%u, Avg=%u",
					        _currentAckSent, _lastReadWhenForced, _lastReadWhenForced -_SentAckTimeHistory[0], _lastRead, _LastRoundTrip,_AvgRoundTrip);
				break;
			}

		    _LastAckTimeSent = _SentAckTimeHistory[++_currentAckSent]= _lastReadWhenForced;
			_log("KeepAlive request #%u sent", _LastKeepAlivedForced); 
			writeLowLevel(packet,6);
		}
		else
		{
			_log("KeepAlive request set"); 
		}
		keep_alive_forced=true;
#endif
		break;

	case _SERVICE_REQUEST_SETIPADDRESS:

		if ( !msg_body)
			throw PInternalError( "Body");

		const char * ret;
		char buf[20];
		int slot_number;
		CommMsgParser parser(*msg_body);
		parser.parseINT32(slot_number);
        parser.parseString(ret);
		ip_address = ret;
		ip_address +="-";
		sprintf(buf,"%d",slot_number);
		ip_address +=buf;
		ip_address +="-";
	    _log(" client connecting: protocol %u(%s), BlockSize %d, Compression %d, keepAlives - %d(%d), muxEnabled = %s",
			_ProtocolSelected,(_useEncryption?"e":"ne"),_MaxBlockSize,_CompressBitsSize, _keepAliveWrite, _keepAliveRead,(muxEnabled? "yes":"no"));
        _MaxRoundTrip=0;
		_lastWrite = _lastRead = SYSTEMTICKS();
		_whenStored = SYSTEMTICKS();

#ifndef COMM_CLIENT_ONLY
	    _LastAckTimeSent=_LastAckTimeReceived = 0;
		memset(_SentAckTimeHistory,0,sizeof(_SentAckTimeHistory));
		_currentAckSent=-1;
#endif
		break;

	}
	return true;
}


template< class _Communicator, class _MsgHandler >
void _CommStream0ProtocolHandler< _Communicator, _MsgHandler >::printStatistic(BYTE level)
{
	if ( !_init)
		throw PInternalError( "Not initialized");

	if (level || _keepAliveWrite == 0)
	{
		UINT32 now = SYSTEMTICKS();
		if (_whenStored == 0)
			_whenStored = now;
		else
		{
			if (now-_whenStored >= _netStatsLogThreshold)
			{
				if (!_CompressBitsSize)
					_log("Received packets(bytes) %u(%u), sent %u(%u), keepalive %u, read inactivity %u, write inactivity %u, queue %u, time in queue %u", 
					     _receivedPackets,_countReceivedBytes,_sentPackets, _countSentBytes,_lastDelta,
						 _maxRead, _maxWrite, wrQueue.size(),lastTimeInQueue);
				else
				{
					UINT32 compressRec, compressSent, packetsSent;
					comm->getStatistics(compressRec,compressSent,packetsSent);
					UINT32 sizeP=0,coeff1=0,coeff2=0;
					if (compressRec)
					{
						coeff1 = (_countReceivedBytes*10)/compressRec;
					}
					if (compressSent)
					{
						coeff2 = (_countSentBytes*10)/compressSent;
					}
					if (packetsSent)
					{
						sizeP = compressSent/packetsSent;
					}

					_log("Received %u packets, (compressed,ratio)= %u(%u, %u.%1.1u), sent %u packets = %u(%u, %u.%1.1u), compressed packets sent %u, average size %u, keepalive delta %u, max read inactivity %u, write inactivity %u, queue %u, time in queue %u", 
						  _receivedPackets, _countReceivedBytes,compressRec,coeff1/10,coeff1%10,
						  _sentPackets, _countSentBytes,compressSent,coeff2/10,coeff2%10,
						  packetsSent,sizeP,_lastDelta,_maxRead,_maxWrite,wrQueue.size(),lastTimeInQueue);
				}
				_whenStored = now;
				_countReceivedBytes=_countSentBytes=_receivedPackets = _sentPackets = _maxRead = _maxWrite = 0;
			}
		}
	}
}

template< class _Communicator, class _MsgHandler >
void _CommStream0ProtocolHandler< _Communicator, _MsgHandler >::_print_connect_info()
{
    _log(" client connected (%s): protocol %u(%s), BlockSize %d, Compression %d, keepAlives - %d(%d), muxEnabled = %s",
		(_inbound?"server":"client"),_ProtocolSelected,(_useEncryption?"e":"ne"),_MaxBlockSize,_CompressBitsSize, _keepAliveWrite, _keepAliveRead,(muxEnabled? "yes":"no"));
}

#ifndef COMM_CLIENT_ONLY
template< class _Communicator, class _MsgHandler >
void _CommStream0ProtocolHandler< _Communicator, _MsgHandler >::_print_binary_data(const BYTE * ptr, int size)
{
	const BYTE table[]="0123456789ABCDEF";
	int pos = 0;
	while (pos < size)
	{
		char outBuffer[16*3 + 1 + 16 + 1];
		memset(outBuffer,' ',sizeof(outBuffer));
		outBuffer[sizeof(outBuffer)-1]=0;
		int i;
		int pos2=pos;
		for (i=0; i < 16 && pos2 < size; i++,pos2++)
		{
			outBuffer[i*3]=table[ptr[pos2]>>4];
			outBuffer[i*3+1]=table[ptr[pos2]&0xf];
		}
		for (i=0; i < 16 && pos < size; i++,pos++)
		{
			outBuffer[16*3 + 1 + i]= isprint(ptr[pos]) ? ptr[pos] : '.';
		}
		_log("T: %s",  outBuffer);	
	}

}
#endif
#endif
