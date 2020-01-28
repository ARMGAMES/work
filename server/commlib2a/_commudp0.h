#ifndef _commudp0_h_included
#define _commudp0_h_included

#include "ppinclude.h"
#include "ppstring.h"
#include "pplogfile.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "_commcomm.h"



//#define PROTOCOLLOGGING

#ifdef PROTOCOLLOGGING
#define _log1  _log
#else
#define _log1 
#endif


#define UDP_NUMSLOTS 100
#define UDP_NUMBUFFS 5
#define UDP_NUMTHREADS 5
#define UDP_NUMCONCURRENT 5
#define UDP_DEFAULT_FRAME_SIZE 512
#define UDP_MINIMUM_FRAME_SIZE 400
#define UDP_MAXIMUM_FRAME_SIZE 1000
#define UDP_DEFAULT_KEEP_ALIVE_READ 30000
#define UDP_DEFAULT_KEEP_ALIVE_WRITE 5000
#define UDP_DEFAULT_KEEP_ALIVE_POLL 200
#define UDP_RESENT_TIMEOUT 500


#define UDP_HEADER_SIZE 50
#define UDP_MSGHEADER_SIZE 4
#define UDP_MSGPATCH_SIZE 8


#define UDP_BROADCAST_HEADER_SIZE 9
#define UDP_CONFIRMED_HEADER_SIZE 11
#define UDP_CONFIRMATION_HEADER_SIZE 13


#define UDP_TOTAL_MAXIMUM_PACKET_SIZE (UDP_MAXIMUM_FRAME_SIZE + UDP_CONFIRMATION_HEADER_SIZE + UDP_MSGHEADER_SIZE)

// protocol 
// For confirmed packets
// 1 - packet type , 0x80 - ack follows
// 4 - peer ID
// 4 - my   Id
// 2 - packet number
// 2 - if 0x80 in type - packet number received or nothing
//

// For broadcast packets
// 1 - packet type
// 4 - peer ID
// 4 - my   Id



// Communicator:
// BYTE * asyncRead( size_t & size) - return buffer with data
// struct 
// {
//   size_t size;
//   BYTE * buf;
// } _UDPBuff; 
// size_t send(_UDPBuff *, int num, void * key);
// void * asyncWrite(size_t &size); - return key and size of written data
// _UDPBuff * getSendBuffer();





//#ifdef WSABUF
//#define _UDPBuff WSABUF
//#else
//struct 
//{
//	size_t len;
//	char * buf;
//} _UDPBuff; 
//#endif



#define ROTR( x, y ) ( ( (x) >> (y) ) | ( (x) << (32-(y)) ) )

template< class _Communicator, class _MsgHandler >
class _CommUDP0ProtocolHandler
{
	private:

	enum {_HelloFirstClientPacket = 1, _HelloFirstServerPacket = 2,    
		  _ToBeConfirmedPacket = 3, _KeepAlivePAcket = 4, _UDPPacket = 5, _ackNumber = 0x80 };


	UINT32 frameSize;
	UINT32 defaultFrameSize;
	BYTE protocolVersion;

	UINT16 sentNumber;
	UINT16 receivedNumber;
	UINT16 confirmedNumber;

	int helloRdState;
	UINT32 _random;

	INT32 _keepAliveWrite;
	INT32 _keepAliveRead;
	INT32 _keepAliveWriteStored;
	INT32 _keepAliveReadStored;

	UINT32 _lastKeepAliveSent;
	UINT32 _lastKeepAliveReceived;

	UINT32 _LastKeepAliveTimeSent;
	UINT32 _LastKeepAliveTimeReceived;

	UINT32 _resendTimeout;

	UINT32 _myId; 
	UINT32 _peerId; 

	PString ip_address;
	bool _beenLogged;
	bool _needToConfirm;

	UINT32 _lastWrite; 
	UINT32 _lastRead;
	UINT32 _countSentBytes;
	UINT32 _countReceivedBytes;
	UINT32 _countSentMsg;
	UINT32 _countReceivedMsg;
	UINT32 _whenStored;


	time_t _timeOfConnect;




	_Communicator* comm;
	_MsgHandler* msgHandler;
	_CommProtocolLog *_logger;

	bool _init;
	bool _inbound;
	bool _conn;

//	CommMsgSimpleQueue wrQueue;

	struct SendItem
	{
		int type;
		int inProcess;
		BYTE header[UDP_HEADER_SIZE];
		int headerSize;
		BYTE msgHeader[UDP_MSGHEADER_SIZE];
		int msgHeaderSize;
		BYTE msgPatch[UDP_MSGPATCH_SIZE];

		BYTE data[sizeof(_CommMsg)];

		_CommMsg &msg()
		{
			return * ((_CommMsg*) data);
		}

		SendItem( int type_)
		{
#include "ppmemtracedisable.h"
			new( (data) ) _CommMsg();
#include "ppmemtraceenable.h"
			type = type_;
			inProcess = 0;
			headerSize=0;
			msgHeaderSize=0;
		}
		~SendItem()
		{
			msg()._CommMsg::~_CommMsg();
		}
	};

	struct SendConfirmedItem : public SendItem
	{
		UINT16 num;
		UINT32 lastSent;
		SendConfirmedItem(UINT16 num_, int type_) : SendItem(type_)
		{
			num = num_;
		}
	};


	typedef deque<SendConfirmedItem> SendToBeConfirmed;
	SendToBeConfirmed sendList;

	struct ReceivedItem
	{
		UINT16 num;
		BYTE data[sizeof(_CommMsg)];
		_CommMsg & msg()
		{
			return *((_CommMsg *)data);
		}
		ReceivedItem()
		{
#include "ppmemtracedisable.h"
			new( (data) ) _CommMsg();
#include "ppmemtraceenable.h"
		}
		~ReceivedItem()
		{
			msg()._CommMsg::~_CommMsg();
		}


	};

	typedef list<ReceivedItem> ReceivedToBeConfirmed;
	ReceivedToBeConfirmed receivedList;

	typedef list<SendItem> SendBroadcast;
	SendBroadcast broadcastQueue;

	bool  send(SendConfirmedItem & it)
	{
		_log1("send SendConfirmedItem %d", it.num);
		if (it.inProcess == 1)
		{
			_log1("send SendConfirmedItem inProcess, return");
			return true;
		}
		WSABUF * buf = comm->getSendBuffer();
		it.inProcess = 0;
		if (buf)
		{
			_log1("send SendConfirmedItem buff found");
			int c = 0;
			buf[c].buf = (char *)it.header;
			buf[c].len = it.headerSize;
			c++;
			if (it.msgHeaderSize)
			{
				buf[c].buf = (char *)it.msgHeader;
				buf[c].len = it.msgHeaderSize;
				c++;
			}
			if (it.msg().body._size())
			{
				if( it.msg().internal.flags & _CommMsg::Patch8 )
				{
					PASSERT3(it.msg().body._size() >= 8);
					CommMsgBody::writeUINT32( it.msgPatch, it.msg().internal.patch0 );
					CommMsgBody::writeUINT32( it.msgPatch + 4, it.msg().internal.patch1 );
					buf[c].buf = (char *) it.msgPatch;
					buf[c].len = 8;
					c++;
					if (it.msg().body._size() > 8)
					{
						buf[c].buf = (char *)it.msg().body._readPtr() + 8 ;
						buf[c].len = it.msg().body._size() - 8;
						c++;
					}
				}
				else
				{
					buf[c].buf = (char *)it.msg().body._readPtr();
					buf[c].len = it.msg().body._size();
					c++;
				}
			}
			it.inProcess = 1;
			it.lastSent=SYSTEMTICKS();
			size_t wr = comm->send(buf,c,&it);
			if (wr)
			{
				_log1("send SendConfirmedItem: comm->send return %d, it->type=%d", wr,it.type);
				_countSentBytes+=wr;
				_countSentMsg++;
				_lastWrite = SYSTEMTICKS();
				it.inProcess = 2;
				return true;
			}
			return false;
		}
		else
		{
			_log1("send SendConfirmedItem buff not found");
		}
		return false;
	}
	bool  send(SendItem & it)
	{
		_log1("send SendItem");
		if (it.inProcess == 1)
		{
			_log1("send SendItem inProcess, return");
			return true;
		}
		WSABUF * buf = comm->getSendBuffer();
		it.inProcess = 0;
		if (buf)
		{
			_log1("send SendItem buff found");
			int c = 0;
			buf[c].buf = (char *)it.header;
			buf[c].len = it.headerSize;
			c++;
			if (it.msgHeaderSize)
			{
				buf[c].buf = (char *)it.msgHeader;
				buf[c].len = it.msgHeaderSize;
				c++;
			}
			if (it.msg().body._size())
			{
				if( it.msg().internal.flags & _CommMsg::Patch8 )
				{
					PASSERT3(it.msg().body._size() >= 8);
					CommMsgBody::writeUINT32( it.msgPatch, it.msg().internal.patch0 );
					CommMsgBody::writeUINT32( it.msgPatch + 4, it.msg().internal.patch1 );
					buf[c].buf = (char *) it.msgPatch;
					buf[c].len = 8;
					c++;
					if (it.msg().body._size() > 8)
					{
						buf[c].buf = (char *)it.msg().body._readPtr() + 8 ;
						buf[c].len = it.msg().body._size() - 8;
						c++;
					}
				}
				else
				{
					buf[c].buf = (char *)it.msg().body._readPtr();
					buf[c].len = it.msg().body._size();
					c++;
				}
			}
			it.inProcess = 1;
			size_t wr = comm->send(buf,c,&it);
			if (wr)
			{
				_log1("send SendItem: comm->send return %d", wr);
				_countSentBytes+=wr;
				_countSentMsg++;
				_lastWrite = SYSTEMTICKS();
				it.inProcess = 2;
				return true;
			}
			return false;
		}
		else
		{
			_log1("send SendItem buff not found");
		}
		return false;
	}

	void resend(UINT16 num)
	{
		_log1("resend %d",num);
		SendToBeConfirmed::iterator it;
		for (it=sendList.begin(); it != sendList.end(); ++it)
		{
			if ((*it).num == num)
			{
				if ((*it).inProcess != 1 && (*it).inProcess != 4)
				{
					send((*it));
				}
				return;
			}
		}
	}

	void remove(UINT16 num)
	{
		_log1("remove %d",num);
		SendToBeConfirmed::iterator it = sendList.begin();

		while (it != sendList.end())
		{
			if (  (INT16)(num - (*it).num) >= 0 )
			{

				SendConfirmedItem & i = (*it);
				if (i.inProcess != 1)
				{
					_log1("remove %d from sendList ",i.num);
					sendList.pop_front();
				}
				else
				{
					i.inProcess = 4;
					return;
				}
				it  = sendList.begin();
			}
			else
				break;
		}
	}

	void removeBroadcast(SendItem * i)
	{
		_log1("removing broadcast");
		SendBroadcast::iterator it = broadcastQueue.begin();

		while (it != broadcastQueue.end())
		{
			if (  i == &(*it) )
			{
				_log1("removed broadcast");
				broadcastQueue.erase(it);
				return;
			}
			++it;
		}
	}


	SendConfirmedItem & getConfirmedItem()
	{
		sendList.push_back(SendConfirmedItem(++sentNumber, 1));
		_log1("getConfirmedItem %d",sentNumber);
		return sendList.back();
	}

	SendItem & getBroadcastItem()
	{
		broadcastQueue.push_back(SendItem(2));
		return broadcastQueue.back();
	}
	void sendBroadcast()
	{
		SendBroadcast::iterator it = broadcastQueue.begin();
		while (it != broadcastQueue.end())
		{
			if ( (*it).inProcess != 1)
			{
				if (send((*it)))
				{
					SendBroadcast::iterator it1 = it;
					++it;
					broadcastQueue.erase(it1);
				}
				else
					break;
			}
			else
				++it;
		}
	}

	public:
	_CommUDP0ProtocolHandler()
	{

	}
	~_CommUDP0ProtocolHandler()
	{
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
	void _log(const char * fmt, va_list args)
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

	void construct( _Communicator& comm_, _MsgHandler& msgHandler_, _CommProtocolLog * logging_ );

	void init( bool inbound, UINT32 myId_ );
	void deinit();

	void writeMsg( _CommMsg& msg );
	void asyncWriteOk();
	void asyncReadOk(bool repeated = false);

	bool serviceCallTransport(UINT32 id, CommMsgBody *msg_body);
	bool isConnected() const
	{
		if (!_init)
			throw PInternalError( "Not initialized");

		return _conn;
	}
	void setDefaultFrameSize(UINT32 frameSize_)
	{
		defaultFrameSize = frameSize_;
	}
	void setKeepAlive( int msecWrite, int msecRead )
		//For inbound (server-side) connections only. 
		//Outbound (client-side) connections will get keepAlive from handshake
	{
		_keepAliveWriteStored = msecWrite;
		_keepAliveReadStored = msecRead;
	}
	void setResendTimeout( UINT32 resend_)
	{
		_resendTimeout = resend_;
	}



	private://helpers

	void _startHandshake();
	void _prepareClientHello(SendConfirmedItem & it);
	void _prepareServerHello(SendConfirmedItem & it);
	void _readClientHello(BYTE * p, size_t size);
	void _readServerHello(BYTE * p, size_t size);
	void _checkClientHello(BYTE * p, size_t size);
	void _checkServerHello(BYTE * p, size_t size);
	void _checkClientConfirmPacket(BYTE * buffer, size_t size);


	void _processClientHello(BYTE * p, size_t size);
	void _processServerHello(BYTE * p, size_t size);
	void _confirmPacket();
	bool _parseMsg(_CommMsg &msg, BYTE * buffer, size_t size);
	void _processMsg(BYTE * buffer, size_t size);
	void _sendKeepAlive();
	void _processKeepAlive(BYTE * buffer, size_t size);
	bool _keepAlive();




	size_t _composeMsg(BYTE * p, _CommMsg & msg);
	void _sendNextPacket();
	void printStatistic();
	void _print_connect_info();

	inline BYTE _localRand(UINT32 & _random_)
	{
		_random_ = 22695477 * _random_ + 1;
		return (( _random_ >> 8 ) & 0xff) ^ (_random_ &0xff);
	}


};

template< class _Communicator, class _MsgHandler >
void _CommUDP0ProtocolHandler< _Communicator, _MsgHandler >::construct( _Communicator& comm_, _MsgHandler& msgHandler_,_CommProtocolLog * logger_ )
{
	_logger = logger_;
	comm = &comm_;
	msgHandler = &msgHandler_;

	_init = false;
	_conn = false;
    _beenLogged=false;
	protocolVersion = 1;

	_countSentBytes = 0;
	_countReceivedBytes = 0;
	_timeOfConnect = 0;
	_lastWrite = 0; 
	_lastRead = 0;
	_countSentMsg = 0;
	_countReceivedMsg = 0;
	helloRdState = 0;
	frameSize = defaultFrameSize = UDP_DEFAULT_FRAME_SIZE;
	_keepAliveWrite = _keepAliveWriteStored = UDP_DEFAULT_KEEP_ALIVE_WRITE;
	_keepAliveRead = _keepAliveReadStored = UDP_DEFAULT_KEEP_ALIVE_READ;
	_resendTimeout = UDP_RESENT_TIMEOUT; 
	_lastKeepAliveSent = 0;
	_lastKeepAliveReceived = 0;
	_myId = 0; 
	_peerId = 0; 
	sentNumber = 0;
	receivedNumber = 0;
	confirmedNumber = 0;
	_whenStored = 0;
	_needToConfirm = false;
}

template< class _Communicator, class _MsgHandler >
void _CommUDP0ProtocolHandler< _Communicator, _MsgHandler >::init( bool inbound, UINT32 myId_ )
{
	if (_init)
		throw PInternalError( "Initialized");

	ip_address = PString("");
	protocolVersion = 1;
	_init = true;
	_conn = false;
	_inbound = inbound;
    _beenLogged=false;
	_countSentBytes = 0;
	_countReceivedBytes = 0;
	_lastWrite = _lastRead = SYSTEMTICKS();
	_countSentMsg = 0;
	_countReceivedMsg = 0;
	_timeOfConnect = plib_time(0);
	helloRdState = 0;
	_random ^= (rand()<<16) | rand();
	_random ^= _timeOfConnect;
	_random ^=  SYSTEMTICKS();
	_random ^= PID();
	frameSize = defaultFrameSize;
	_keepAliveWrite = _keepAliveWriteStored;
	_keepAliveRead = _keepAliveReadStored;
	_lastKeepAliveSent = 0;
	_lastKeepAliveReceived = 0;
	_LastKeepAliveTimeReceived = SYSTEMTICKS();
	_peerId = 0;
	if (myId_)
		_myId = myId_; 
	else
		_myId = _localRand(_random);

	sentNumber = _localRand(_random);
	sentNumber = ( sentNumber << 8 ) | _localRand(_random);
	confirmedNumber = sentNumber;
	receivedNumber = 0;
	_whenStored = 0;
	_needToConfirm = false;
	_startHandshake();

}


template< class _Communicator, class _MsgHandler >
void _CommUDP0ProtocolHandler< _Communicator, _MsgHandler >::_startHandshake()
{
	_log1("start handshake");
   	helloRdState=1;
	if( !_inbound )
	{//client

		SendConfirmedItem & it = getConfirmedItem();
		_prepareClientHello(it);
		send(it);
	}
	else
	{//server
	 // 'init' for sever is called only if incoming packet is available;
		size_t size;
		BYTE * p = comm->asyncReadOk(size);
		_processClientHello(p,size);
	}

}


template< class _Communicator, class _MsgHandler >
void _CommUDP0ProtocolHandler< _Communicator, _MsgHandler >::_prepareClientHello(SendConfirmedItem & it)
{
	_log1("_prepareClientHello %d", it.num);
	int i;
	for (i=0;i<UDP_HEADER_SIZE;i++)
		it.header[i] = rand();
	it.header[0]=_HelloFirstClientPacket;	

	CommMsgBody::writeUINT32( it.header+1, _random );
	CommMsgBody::writeUINT16( it.header+5, it.num );
	CommMsgBody::writeUINT32( it.header+7, _myId );
	it.header[14]=protocolVersion;
	CommMsgBody::writeUINT32( it.header + 15, frameSize );
	UINT32 c=0;
	for(i=0;i<UDP_HEADER_SIZE - 4;i++)
	{
		c ^= it.header[i];
		c = ROTR(c,8);
	}
	CommMsgBody::writeUINT32( it.header + UDP_HEADER_SIZE - 4, c);
	for(i=5;i<UDP_HEADER_SIZE;i++)
	{
		it.header[i] ^= _localRand(_random);
	}
	it.headerSize = UDP_HEADER_SIZE;
	it.msgHeaderSize = 0;
}


template< class _Communicator, class _MsgHandler >
void _CommUDP0ProtocolHandler< _Communicator, _MsgHandler >::_readClientHello( BYTE * buffer, size_t size )
{
	_log1("_readClientHello");
	int i;
	BYTE *p = buffer;

	if (size != UDP_HEADER_SIZE)
		throw PInternalError( "Server CommUDP0: illegal first packet size" );
	BYTE packet = p[0];
	if (packet != _HelloFirstClientPacket)
		throw PInternalError( "Server CommUDP0: illegal first packet" );

	UINT32 _clientRandom = CommMsgBody::readUINT32( p + 1 );
	for(i=5;i<UDP_HEADER_SIZE;i++)
	{
		p[i] ^= _localRand(_clientRandom);
	}

	UINT32 c=0;
	for(i=0;i<UDP_HEADER_SIZE - 4;i++)
	{
		c ^= p[i];
		c = ROTR(c,8);
	}
	if (c != CommMsgBody::readUINT32( p + UDP_HEADER_SIZE - 4 ))
	{
		throw PInternalError( "Server CommUDP0: checksum is invalid" );
	}


	receivedNumber = CommMsgBody::readUINT16( p + 5);
	_peerId = CommMsgBody::readUINT32( p + 7);

//	BYTE _clientProtocolVersion = p[14];

	UINT32 clientFrameSize = CommMsgBody::readUINT32( p + 15);
	if (clientFrameSize < UDP_MINIMUM_FRAME_SIZE || clientFrameSize > UDP_MAXIMUM_FRAME_SIZE)
		throw PInternalError( "Server CommUDP0: incorrect client frame size");


	if (clientFrameSize < frameSize) 
		frameSize = clientFrameSize;
	_log1("_readClientHello end, receivedNumber=%d",receivedNumber);
	
	_random = _clientRandom; // accept client 
}

template< class _Communicator, class _MsgHandler >
void _CommUDP0ProtocolHandler< _Communicator, _MsgHandler >::_checkClientHello( BYTE * buffer, size_t size )
{
	_log1("_checkClientHello");
	// check retransmitted first packet
	int i;
	BYTE *p = buffer;

	if (size != UDP_HEADER_SIZE)
		throw PInternalError( "Server CommUDP0: illegal first packet size" );
	BYTE packet = p[0];
	if (packet != _HelloFirstClientPacket)
		throw PInternalError( "Server CommUDP0: illegal first packet" );

	UINT32 _clientRandom = CommMsgBody::readUINT32( p + 1 );
	for(i=5;i<UDP_HEADER_SIZE;i++)
	{
		p[i] ^= _localRand(_clientRandom);
	}

	UINT32 c=0;
	for(i=0;i<UDP_HEADER_SIZE - 4;i++)
	{
		c ^= p[i];
		c = ROTR(c,8);
	}
	if (c != CommMsgBody::readUINT32( p + UDP_HEADER_SIZE - 4 ))
	{
		throw PInternalError( "Server CommUDP0: checksum is invalid" );
	}

	if (receivedNumber != CommMsgBody::readUINT16( p + 5))
	{
		throw PInternalError( "Server CommUDP0: received number not valid");
	}
	if (_peerId != CommMsgBody::readUINT16( p + 7))
	{
		throw PInternalError( "Server CommUDP0: illegal id");
	}

	UINT32 clientFrameSize = CommMsgBody::readUINT32( p + 15);
	if (clientFrameSize < UDP_MINIMUM_FRAME_SIZE || clientFrameSize > UDP_MAXIMUM_FRAME_SIZE)
		throw PInternalError( "Server CommUDP0: incorrect client frame size");
	_log1("_checkClientHello end, receivedNumber=%d",receivedNumber);
}


template< class _Communicator, class _MsgHandler >
void _CommUDP0ProtocolHandler< _Communicator, _MsgHandler >::_prepareServerHello(SendConfirmedItem & it)
{
	_log1("_prepareServerHello %d, received=%d", it.num,receivedNumber);
	int i;
	for (i=0;i<UDP_HEADER_SIZE;i++)
		it.header[i] = rand();
	
	it.header[0]=_HelloFirstServerPacket | _ackNumber;	
	CommMsgBody::writeUINT32( it.header+1, _peerId );
	CommMsgBody::writeUINT16( it.header+5, it.num );
	CommMsgBody::writeUINT32( it.header+7, _myId );
	CommMsgBody::writeUINT16( it.header+11, receivedNumber );

	it.header[14]=protocolVersion;
	CommMsgBody::writeUINT32( it.header + 15, frameSize );
	CommMsgBody::writeUINT32( it.header + 19, _keepAliveWrite );
	CommMsgBody::writeUINT32( it.header + 23, _keepAliveRead );

	UINT32 c=0;
	for(i=0;i<UDP_HEADER_SIZE - 4;i++)
	{
		c ^= it.header[i];
		c = ROTR(c,8);
	}
	CommMsgBody::writeUINT32( it.header + UDP_HEADER_SIZE - 4, c);
	for(i=5;i<UDP_HEADER_SIZE;i++)
	{
		it.header[i] ^= _localRand(_random);
	}
	it.headerSize = UDP_HEADER_SIZE;
	it.msgHeaderSize = 0;
}

template< class _Communicator, class _MsgHandler >
void _CommUDP0ProtocolHandler< _Communicator, _MsgHandler >::_readServerHello( BYTE * buffer, size_t size)
{
	_log1("_readServerHello %d", *buffer);
	int i;
	BYTE *p = buffer;

	if (size != UDP_HEADER_SIZE)
		throw PInternalError( "Client CommUDP0: illegal first packet size" );

	BYTE packet = p[0]& ~_ackNumber;
	if (packet != _HelloFirstServerPacket)
		throw PInternalError( "Client CommUDP0: illegal first packet" );

	UINT32 _r = _random;
	for(i=5;i<UDP_HEADER_SIZE;i++)
	{
		p[i] ^= _localRand(_r);
	}

	UINT32 c=0;
	for(i=0;i<UDP_HEADER_SIZE - 4;i++)
	{
		c ^= p[i];
		c = ROTR(c,8);
	}
	_log1("_readServerHello confirmed=%d, our sent=%d",CommMsgBody::readUINT16( p + 11),sentNumber);
	if (c != CommMsgBody::readUINT32( p + UDP_HEADER_SIZE - 4 ))
	{
		throw PInternalError( "Client CommUDP0: checksum is invalid" );
	}

	if ( !( p[0] & _ackNumber) || CommMsgBody::readUINT16( p + 11) != sentNumber)
		throw PInternalError( "Client CommUDP0: illegal confirmation number");

	
	if (_myId != CommMsgBody::readUINT32( p + 1))
		throw PInternalError( "Client CommUDP0: illegal id");
	
	receivedNumber = CommMsgBody::readUINT16( p + 5);
	frameSize = CommMsgBody::readUINT32( p + 15);
	if (frameSize < UDP_MINIMUM_FRAME_SIZE || frameSize > UDP_MAXIMUM_FRAME_SIZE)
		throw PInternalError( "Client CommUDP0: incorrect frame size");

	_keepAliveWrite = CommMsgBody::readUINT32( p + 19);
	_keepAliveRead = CommMsgBody::readUINT32( p + 23);
	_peerId = CommMsgBody::readUINT32( p + 7);
	_log1("_readServerHello end, receivedNumber=%d",receivedNumber);
}

template< class _Communicator, class _MsgHandler >
void _CommUDP0ProtocolHandler< _Communicator, _MsgHandler >::_checkServerHello( BYTE * buffer, size_t size)
{
	_log1("_checkServerHello");
	int i;
	BYTE *p = buffer;

	if (size != UDP_HEADER_SIZE)
		throw PInternalError( "Client CommUDP0: illegal first packet size" );

	BYTE packet = p[0]& ~_ackNumber;
	if (packet != _HelloFirstServerPacket)
		throw PInternalError( "Client CommUDP0: illegal first packet" );

	UINT32 _r = _random;
	for(i=5;i<UDP_HEADER_SIZE;i++)
	{
		p[i] ^= _localRand(_r);
	}

	UINT32 c=0;
	for(i=0;i<UDP_HEADER_SIZE - 4;i++)
	{
		c ^= p[i];
		c = ROTR(c,8);
	}
	_log1("_checkServerHello confirmed=%d, our sent=%d",CommMsgBody::readUINT16( p + 11),sentNumber);
	_log1("_checkServerHello received=%d, our received=%d",CommMsgBody::readUINT16( p + 5),receivedNumber);
	if (c != CommMsgBody::readUINT32( p + UDP_HEADER_SIZE - 4 ))
	{
		throw PInternalError( "Client CommUDP0: checksum is invalid" );
	}
	
	if (_myId != CommMsgBody::readUINT32( p + 1))
		throw PInternalError( "Client CommUDP0: illegal id");

	if (receivedNumber != CommMsgBody::readUINT16( p + 5))
	{
		throw PInternalError( "Server CommUDP0: received number not valid");
	}


	frameSize = CommMsgBody::readUINT32( p + 15);
	if (frameSize < UDP_MINIMUM_FRAME_SIZE || frameSize > UDP_MAXIMUM_FRAME_SIZE)
		throw PInternalError( "Client CommUDP0: incorrect frame size");

	if ( _keepAliveWrite != CommMsgBody::readUINT32( p + 19))
		throw PInternalError( "Client CommUDP0: incorrect keepAlive Write");
	if ( _keepAliveRead != CommMsgBody::readUINT32( p + 23))
		throw PInternalError( "Client CommUDP0: incorrect keepAlive Read");
	if (_peerId != CommMsgBody::readUINT32( p + 7))
		throw PInternalError( "Client CommUDP0: incorrect peer Id");
	_log1("_checkServerHello end, receivedNumber=%d",receivedNumber);

}


template< class _Communicator, class _MsgHandler >
void _CommUDP0ProtocolHandler< _Communicator, _MsgHandler >::_confirmPacket()
{
	if (_needToConfirm == false)
	{
		_needToConfirm = true;
		_log1("_confirmPacket, set flag sent=%d, received= %d",sentNumber,receivedNumber);
		return;
	}
	_log1("_confirmPacket sent=%d, received= %d",sentNumber,receivedNumber);
	_needToConfirm = true;
	SendItem & it = getBroadcastItem();
	it.header[0] = _ackNumber;
	CommMsgBody::writeUINT32( it.header+1, _peerId );
	CommMsgBody::writeUINT32( it.header+5, _myId );
	CommMsgBody::writeUINT16( it.header+9, sentNumber );
	CommMsgBody::writeUINT16( it.header+11, receivedNumber );
	it.headerSize = 13;
	it.msgHeaderSize = 0;
	sendBroadcast();
}

template< class _Communicator, class _MsgHandler >
void _CommUDP0ProtocolHandler< _Communicator, _MsgHandler >::_checkClientConfirmPacket(BYTE * p, size_t size)
{
	_log1("_checkClientConfirmPacket confirmed=%d, our sent=%d",CommMsgBody::readUINT16( p + 11),sentNumber);
	if (size < 13)
		throw PInternalError( "Server CommUDP0: illegal  packet size" );

	if ( !( p[0] & _ackNumber) || CommMsgBody::readUINT16( p + 11) != sentNumber)
		throw PInternalError( "Server CommUDP0: illegal confirmation number");

	if ( CommMsgBody::readUINT32( p + 5) != _peerId)
		throw PInternalError( "Server CommUDP0: illegal peer id");
}


template< class _Communicator, class _MsgHandler >
void _CommUDP0ProtocolHandler< _Communicator, _MsgHandler >::_processClientHello(BYTE * p, size_t size)
{
	_log1("_processClientHello, packet=%d, state=%d", *p,helloRdState);
	if ((*p & ~_ackNumber) == _HelloFirstClientPacket)
	{
		// First packet
		if (helloRdState == 1)
		{
			// first packet
			_readClientHello(p,size);
			SendConfirmedItem & it = getConfirmedItem();
			_prepareServerHello(it);
			helloRdState = 2;
			send(it);
		}
		else if (helloRdState == 2)
		{
			// resend server hello
			_checkClientHello(p,size);
			resend(sentNumber);
		}
	}
	else 
	{
		PASSERT3(helloRdState == 2);
		_checkClientConfirmPacket(p,size);
		helloRdState = 3;
		_log1("connected");
		_conn=true;
	}
}


template< class _Communicator, class _MsgHandler >
void  _CommUDP0ProtocolHandler< _Communicator, _MsgHandler >::_processServerHello(BYTE * p, size_t size)
{
	_log1("_processServerHello, packet=%d, state=%d", *p,helloRdState);
	if ( (*p & ~_ackNumber) == _HelloFirstServerPacket)
	{
		// First packet
		if (helloRdState == 1)
		{
			// first packet
			_readServerHello(p,size);
			helloRdState = 2;
			_conn = true;
			_log1("connected");
//			_confirmPacket();
			remove(sentNumber);
			return;
		}
	}
	throw PInternalError( "Client CommUDP0: illegal state in _processServerHello" );
}



template< class _Communicator, class _MsgHandler >
void _CommUDP0ProtocolHandler< _Communicator, _MsgHandler >::deinit()
{
	UINT32 now = SYSTEMTICKS();
	_log("Disconnected. Connection time=%d", (int)(plib_time(0) - _timeOfConnect)); 
}


template< class _Communicator, class _MsgHandler >
void _CommUDP0ProtocolHandler< _Communicator, _MsgHandler >::asyncWriteOk()
{
	_log1("asyncWriteOk");
	if ( !_init)
		throw PInternalError( "Not initialized");
	size_t size;
	SendItem * it = (SendItem*) comm->asyncWriteOk(size);
	if (!it)
	{
		_log1("asyncWriteOk - SendItem 0");
		return;
	}

	if (it->type == 1)
	{
		PASSERT3(it->inProcess == 1 || it->inProcess == 4);
		if (it->inProcess == 1)
			it->inProcess = 2;
		else
			remove( ((SendConfirmedItem *)it)->num);
	}
	else
		removeBroadcast(it);

	_countSentBytes+=size;
	_countSentMsg++;
	_lastWrite = SYSTEMTICKS();
	if (_conn)
		_sendNextPacket();

}
template< class _Communicator, class _MsgHandler >
bool  _CommUDP0ProtocolHandler< _Communicator, _MsgHandler >::_parseMsg(_CommMsg &msg, BYTE * p, size_t size)
{
	_log1("_parseMsg size=%d",size);
	if (size < 4)
		return false;

	BYTE b = *p++;
	BYTE priority = b & 0x0F;
	if( priority >= _COMM_MSGPRIORITIES )
		throw PInternalError( "UDP0ProtocolHandler: Invalid received msg priority" );
	UINT16 flags = CommMsgBody::readUINT16( p );
	p += 2;
	BYTE type = *p++;

	_log1("_parseMsg type=%d size=%d", type, size - 4);

	msg.priority = priority;
	msg.flags = flags;
	msg.type = type;
	msg.internal.mux = 0;
	msg.internal.muxChannelId = 0;
	if (size > 4)
	{
		msg.body._alloc( size - 4);
		msg.body.clear();
		memcpy( msg.body._append(size - 4), p, size - 4);
	}
	return true;
}


template< class _Communicator, class _MsgHandler >
void  _CommUDP0ProtocolHandler< _Communicator, _MsgHandler >::_processMsg(BYTE * buffer, size_t size)
{
	_log1("_processMsg size=%d",size);
	BYTE *p = buffer;

	if (size < UDP_BROADCAST_HEADER_SIZE)
		return;

	BYTE type = p[0];
	_log1("_processMsg size=%d, type=%d",size,type);

	if ((type & ~_ackNumber) == _HelloFirstServerPacket)
	{
		PASSERT3(!_inbound);
		_checkServerHello(p,size);
		_confirmPacket();
		return;
	}

	UINT32 myId_ = CommMsgBody::readUINT32( p + 1);
	UINT32 peerId_ = CommMsgBody::readUINT32( p + 5);
	if (myId_ != _myId || peerId_ != _peerId)
		return;


	if  ( type & _ackNumber)
	{
		if (size < UDP_CONFIRMATION_HEADER_SIZE)
			return;
		UINT16 confirmed = CommMsgBody::readUINT16( p + 11 );
		_log1("_processMsg  confirmed_=%d, our sent=%d",confirmed, sentNumber);
		remove(confirmed);
	}

	if ((type & ~_ackNumber) == _UDPPacket)
	{
		_log1("_processMsg  UDP received");
		p+=9;
		_CommMsg msg;
		if (_parseMsg(msg,p,size - (p-buffer)))
		{
			msgHandler->msg(  msg );
			msg.body._dealloc();
		}
	}
	else if ((type & ~_ackNumber) == _ToBeConfirmedPacket)
	{
		if (size < UDP_CONFIRMED_HEADER_SIZE)
			return;
		UINT16 packetReceived_ = CommMsgBody::readUINT16( p + 9 );
		_log1("_processMsg tobeconfirmed received=%d, our receivedNumber=%d",packetReceived_, receivedNumber);
		
		p+=13;

		_CommMsg msg;
		if (_parseMsg(msg,p,size - (p-buffer)))
		{
			if ( (UINT16) (receivedNumber + 1) == packetReceived_)
			{
				_log1("_processMsg msgHandler %d", packetReceived_);
				msgHandler->msg(  msg );
				msg.body._dealloc();
				receivedNumber++;
				ReceivedToBeConfirmed :: iterator it = receivedList.begin();
				while(it != receivedList.end())
				{
					if ( (*it).num == (UINT16) (receivedNumber + 1))
					{
						_log1("_processMsg loop msgHandler %d", (*it).num);
						msgHandler->msg(  (*it).msg() );
						receivedList.pop_front();
						receivedNumber++;
						it = receivedList.begin();
					}
					else
						break;
				}
				_confirmPacket();
			}
			else if ( (INT16) ( packetReceived_ - receivedNumber) <= 0 )
			{
				// do nothing 
			}
			else
			{
				ReceivedToBeConfirmed :: iterator it;
				for (it = receivedList.begin(); it != receivedList.end();++it)
				{
					if ( (*it).num == packetReceived_)
					{
						// already inserted
						break;
					}
					else if ( (INT16) ((*it).num - packetReceived_) > 0)
					{
						it = receivedList.insert(it,ReceivedItem());
						(*it).num = packetReceived_;
						(*it).msg().moveFrom(msg);
						break;
					}
				}
			}
		}

	}
	else if ((type & ~_ackNumber) == _KeepAlivePAcket)
	{
		if (size < UDP_CONFIRMED_HEADER_SIZE)
			return;
		p+=13;
		_processKeepAlive(p, size - (p-buffer));
	}
}


template< class _Communicator, class _MsgHandler >
void _CommUDP0ProtocolHandler< _Communicator, _MsgHandler >::asyncReadOk(bool repeated)
{
	if ( !_init)
		throw PInternalError( "Not initialized");

	size_t size;
	BYTE * p = comm->asyncReadOk(size);
	if (!p) 
	{
		_log1("asyncReadOk return 0");
		return;
	}
	_log1("asyncReadOk size=%d", size);
	if (repeated == false)
	{
		_countReceivedBytes+=size;
		_countReceivedMsg++;
	}
	_lastRead = SYSTEMTICKS();

	if (_conn == false)
	{
		if (_inbound)
		{
			// server
			_log1("server received hello, state=%d", helloRdState);
			_processClientHello(p,size);
		}
		else
		{
			_log1("client received hello, state=%d", helloRdState);
			_processServerHello(p,size);
			return;
		}
	}
	else
	{
		_processMsg(p,size);
	}
}


template< class _Communicator, class _MsgHandler >
void _CommUDP0ProtocolHandler< _Communicator, _MsgHandler >::_sendKeepAlive()
{
	SendItem & it = getBroadcastItem();
	_needToConfirm = false;
	_LastKeepAliveTimeSent = SYSTEMTICKS();
	BYTE *p = it.header;
	*p =_KeepAlivePAcket;
	*p|= _ackNumber;
	p++;
	CommMsgBody::writeUINT32( p, _peerId );
	p+=sizeof(UINT32);
	CommMsgBody::writeUINT32( p, _myId );
	p+=sizeof(UINT32);
	CommMsgBody::writeUINT16( p, sentNumber);
	p+=sizeof(UINT16);
	CommMsgBody::writeUINT16( p, receivedNumber );
	p+=sizeof(UINT16);
	CommMsgBody::writeUINT32( p, _lastKeepAliveSent++);
	p+=sizeof(UINT32);
	CommMsgBody::writeUINT32( p, _lastKeepAliveReceived);
	p+=sizeof(UINT32);

	it.headerSize = p-it.header;
	it.msgHeaderSize = 0;
	sendBroadcast();
}

template< class _Communicator, class _MsgHandler >
void _CommUDP0ProtocolHandler< _Communicator, _MsgHandler >::_processKeepAlive(BYTE * p, size_t size)
{
	_LastKeepAliveTimeReceived = SYSTEMTICKS();
	_lastKeepAliveReceived = CommMsgBody::readUINT32( p );
//	UINT32 lastKASend = CommMsgBody::readUINT32( p+4 );
	if (!_inbound)
	{
		_sendKeepAlive();
	}
}





template< class _Communicator, class _MsgHandler >
void _CommUDP0ProtocolHandler< _Communicator, _MsgHandler >::_sendNextPacket()
{
	_log1("_sendNextPacket" );
	if ( !_init)
		throw PInternalError( "Not initialized");
	if(_conn == false)
	{
		sendBroadcast();
		return;
	}

	// check resend list
	SendToBeConfirmed::iterator it;
	UINT32 t = SYSTEMTICKS();
	for (it = sendList.begin(); it != sendList.end(); ++it)
	{
		if ((*it).inProcess == 0 || ( (t - (*it).lastSent)>_resendTimeout  && (*it).inProcess != 1))
		{
			(*it).lastSent = t;
			_log1("_sendNextPacket: resend tobeconfirmed %d, %s", (*it).num, ((*it).inProcess == 0 ?"resending!":"") );
			(*it).header[0] |= _ackNumber;
			CommMsgBody::writeUINT16( (*it).header+11, receivedNumber );
			(*it).headerSize = 13;
			_needToConfirm = false;
			if (send(*it)==false)
				return;
		}
	}
	sendBroadcast();
}

template< class _Communicator, class _MsgHandler >
void _CommUDP0ProtocolHandler< _Communicator, _MsgHandler >::writeMsg(_CommMsg& msg )
{
//PTRACE( "WRITEMSG" );
	if ( !_init )
		throw PInternalError( "Not initialized");

	if ( !_conn )
		throw PInternalError( "Not connected");

	if ( !(msg.priority < _COMM_MSGPRIORITIES ))
		throw PInternalError( "Incorrect priority");

	if ( msg.body._size() > frameSize)
	{
		PString err;
		err.append("Message size ").appendInt(msg.body._size()).append(" exceeds limit ").appendInt(frameSize);
		throw PInternalError( err.c_str());
	}

	_log1("write msg new, type =%x, size=%d",msg.type,msg.body._size());

	if (msg.type == _COMM_MSGTYPE_CONNECT_CLIENT_USERMSG || 
		msg.type == _COMM_MSGTYPE_CONNECT_SERVER_USERMSG || 
		msg.type == _COMM_MSGTYPE_SUBSCRIPTION_ONLINE_UPDATE || 
		msg.type == _COMM_MSGTYPE_SUBSCRIPTION_LIGHT_ONLINE_UPDATE
	   )
	{
		SendItem & it = getBroadcastItem();
		BYTE *p = it.header;
		*p =_UDPPacket;
		p++;
		CommMsgBody::writeUINT32( p, _peerId );
		p+=sizeof(UINT32);
		CommMsgBody::writeUINT32( p, _myId );
		p+=sizeof(UINT32);
		it.headerSize = p-it.header;
		p = it.msgHeader;
		*p++ = msg.priority & 0x0f;
		CommMsgBody::writeUINT16( p, msg.flags );
		p += 2;
		*p++ = msg.type;
		it.msgHeaderSize = p-it.msgHeader;
		it.msg().moveFrom(msg);
		sendBroadcast();
	}
	else
	{
		SendConfirmedItem & it = getConfirmedItem();
		_needToConfirm = false;
		BYTE *p = it.header;
		*p =_ToBeConfirmedPacket|_ackNumber;
		p++;
		CommMsgBody::writeUINT32( p, _peerId );
		p+=sizeof(UINT32);
		CommMsgBody::writeUINT32( p, _myId );
		p+=sizeof(UINT32);
		CommMsgBody::writeUINT16( p, it.num );
		p+=sizeof(UINT16);
		CommMsgBody::writeUINT16( p, receivedNumber );
		p+=sizeof(UINT16);
		it.headerSize = p-it.header;
		p = it.msgHeader;
		*p++ = msg.priority & 0x0f;
		CommMsgBody::writeUINT16( p, msg.flags );
		p += 2;
		*p++ = msg.type;
		it.msgHeaderSize = p-it.msgHeader;
		it.msg().moveFrom(msg);
		send(it);
	}
}

template< class _Communicator, class _MsgHandler >
bool _CommUDP0ProtocolHandler< _Communicator, _MsgHandler >::_keepAlive()
{

	if ( !_init)
		throw PInternalError( "Not initialized");

	UINT32 now = SYSTEMTICKS();
	if(_inbound )
	{
		if( now - _LastKeepAliveTimeSent >= _keepAliveWrite * TICKS_PER_SECOND / 1000 )
		{
			_sendKeepAlive();
		}
	}
	UINT32 delta = now - _lastRead;
	if ( delta < _keepAliveRead )
	{
		return true;
	}
	else
	{
		_log("Timeout %u, expired.",delta);
		return false;
	}
}


template< class _Communicator, class _MsgHandler >
bool _CommUDP0ProtocolHandler< _Communicator, _MsgHandler >::serviceCallTransport(UINT32 id, CommMsgBody *msg_body)
{

	switch(id)
	{
	case  _SERVICE_REQUEST_STATISTIC:
		{
			if(msg_body)
			{
				msg_body->composeUINT32(0);
			}
			return true;
		}
		break;

	case  _SERVICE_REQUEST_KEEP_ALIVE:
		{
			bool Ok = _keepAlive();
			_sendNextPacket();
			printStatistic();
			return Ok;
		}
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
	    _log(" client connecting");
		_lastWrite = _lastRead = SYSTEMTICKS();
		break;
	}
	return true;
}


template< class _Communicator, class _MsgHandler >
void _CommUDP0ProtocolHandler< _Communicator, _MsgHandler >::printStatistic()
{
	if ( !_init)
		throw PInternalError( "Not initialized");

	{
		UINT32 now = SYSTEMTICKS();
		if (_whenStored == 0)
			_whenStored = now;
		else
		{
			if (now-_whenStored > 60000)
			{
				_log("Received = %u Bytes, sent = %u Bytes", _countReceivedBytes,_countSentBytes);
				_countReceivedBytes=_countSentBytes=0;
				_whenStored = now;
			}
		}
	}
}

template< class _Communicator, class _MsgHandler >
void _CommUDP0ProtocolHandler< _Communicator, _MsgHandler >::_print_connect_info()
{
    _log(" client connected (%s): protocol %u(%s), BlockSize %d, Compression %d, keepAlives - %d(%d), muxEnabled = %s",
		(_inbound?"server":"client"),_ProtocolSelected,(_useEncryption?"e":"ne"),_MaxBlockSize,_CompressBitsSize, _keepAliveWrite, _keepAliveRead,(muxEnabled? "yes":"no"));
}

#endif
