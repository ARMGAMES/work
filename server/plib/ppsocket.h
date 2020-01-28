#ifndef ppsocket_h_included
#define ppsocket_h_included

#include "ppinclude.h"
#include "ppthread.h"
#include "ppstring.h"
#include "pperror.h"

typedef struct
{
	unsigned long len;
	char * buf;
} PIOBUF;



class PSocket
{
public:
	static void startup();
	static void cleanup();

private://prohibited. Prohibits also copying of all derived classes
	PSocket( const PSocket& );
	PSocket& operator =( const PSocket& );
};

class PIPListeningSocket;

#ifdef PPOSIX
class PUnixListeningSocket;
#endif

class PBaseSocket
{
public:
	struct Descriptor
	{
		SOCKET sock;
#ifdef PPOSIX
		struct sockaddr_storage addr;
#else
		struct sockaddr addr;
#endif
	};


private:
#ifdef PPOSIX
	struct sockaddr_storage addr;
#else
	struct sockaddr addr;
#endif
	SOCKET sock;

public:
	PBaseSocket()
	{
		memset( &addr, 0, sizeof(addr) );
	}
	~PBaseSocket();

public:
	void extractDescriptor( Descriptor& ret )
	{
		ret.addr = addr;
		ret.sock = sock;
		sock = INVALID_SOCKET;
	}
	void copyDescriptor( Descriptor& ret )
	{
		ret.addr = addr;
		ret.sock = sock;
	}


	void close();
	size_t send( const BYTE* p, size_t sz );
	void sendComplete( const BYTE* p, size_t sz );
	size_t recv( BYTE* p, size_t sz );//cannot return 0 - throws an exception on gracefully closed connection
	size_t recv( BYTE* p, size_t sz, int msecTimeout );//returns 0 on timeout; throws an exception on gracefully closed connection
	size_t recvStream( BYTE* p, size_t sz, int msecTimeout ); // returns -1 when timed out, 0 - gracefully closed connection, throws on errors
	void recvComplete( BYTE* p, size_t sz );

	static void address( PString& ret, const Descriptor& descr );
	static bool isLocal( const Descriptor& descr );
	static bool setBuffer( SOCKET sock, int size, int type );




	friend class PIPListeningSocket;
	friend class PIPSocket;
	friend class PUDPSocket;
#ifdef PPOSIX
	friend class PUnixListeningSocket;
	friend class PUnixSocket;
#endif 
};


class PIPSocket : public PBaseSocket//IPv4 socket
{
public:
private:

public:
	PIPSocket();
	PIPSocket( const Descriptor& );
	~PIPSocket(){}

public:

	static bool enableKeepAlive( SOCKET sock );
	bool enableKeepAlive();//usually 2 hours
	static bool tcpNoDelay( SOCKET sock );

	bool tcpNoDelay();//TCP_NODELAY
	bool tcpSetSendBuffer(int size);
	bool tcpSetRecvBuffer(int size);
	bool tcpSetBuffer(int size);

	void connect( const char* addr, int port );//accepts either numeric or symbolic address
	bool connect( const char* hostName, int port, int msecTimeout );
	void bind( int port );
	void address( PString& ret ) const
	{
		_address( ret, ((struct sockaddr_in *)&addr)->sin_addr );
	}
	int port() const
	{
		return _port( *(struct sockaddr_in *)&addr );
	}


public:
	static void address( PString& ret, const Descriptor& descr )
	{
		_address( ret, ((struct sockaddr_in *)&descr.addr)->sin_addr );
	}
	static void ownAddress( PString& ret );//only one address - uses gethostname/gethostbyname pair
	static int port( const Descriptor& descr )
	{
		return _port( *(struct sockaddr_in *)&descr.addr );
	}
	static void hostByName( PString& retIp, const char* hostName );
	static void composeDescriptor( Descriptor& ret, int port, const char * addr )
	{
		memset( &ret.addr, 0, sizeof(ret.addr) );
		((struct sockaddr_in *)&ret.addr)->sin_family = AF_INET;
		((struct sockaddr_in *)&ret.addr)->sin_port = htons( static_cast< u_short >( port ) );
		((struct sockaddr_in *)&ret.addr)->sin_addr.s_addr = inet_addr( addr);
	}


private:
#ifdef P_IOS
	int getSocketDomain();
#endif
	static void _address( PString& ret, const struct in_addr& addr );
	static int _port( const struct sockaddr_in& addr );
	friend class PIPListeningSocket;
};

class PIPListeningSocket : public PIPSocket//IPv4 socket
{
private:
#ifdef PPOSIX
	int fd [ 2 ];
#endif
public:
	PIPListeningSocket();
	~PIPListeningSocket();
#if defined(LINUX)
	void bind( int port );
#endif
	void listen( int queueSize = 5 );
	void accept( Descriptor& ret );
	void close();
};

#ifdef PPOSIX
class PUnixSocket: public PBaseSocket//Unix socket
{

public:
	PUnixSocket();
	PUnixSocket( const Descriptor& );
	~PUnixSocket(){}

public:
	void extractDescriptor( Descriptor& ret )
	{
		ret.addr = addr;
		ret.sock = sock;
		sock = INVALID_SOCKET;
	}
	void copyDescriptor( Descriptor& ret )
	{
		ret.addr = addr;
		ret.sock = sock;
	}

	void connect( const char* addr_ );
	void bind( const char* addr_ );
	void address( PString& ret ) const
	{
		_address( ret, ((struct sockaddr_un *)&addr)->sun_path );
	}

public:
	static void address( PString& ret, const Descriptor& descr )
	{
		_address( ret, ((struct sockaddr_un *)&descr.addr)->sun_path );
	}

private:
	static void _address( PString& ret, const char * path)
	{
		ret="";
		ret.append(path);
	}
	friend class PUnixListeningSocket;
};

class PUnixListeningSocket : public PUnixSocket//Unix socket
{
private:
	int fd [ 2 ];
public:
	PUnixListeningSocket();
	~PUnixListeningSocket();
	void listen( int queueSize = 5 );
	void accept( Descriptor& ret );
	void close();
};
#endif


#ifdef PWIN32
class PMultiSocketRW
{
public:
	enum { MaxSockets = 32 };//maximum 64/2 = 32
	enum { waitQueue = 0, waitRead = 1, waitWrite = 2, waitTimeout = 3, waitFailed = 4 };

private:
	struct _Buf
	{
		BYTE* buf;
		size_t sz;
		size_t offset;
	};
	struct _Slot
	{
		PString _failedMsg;
		PString _failedErrMsg;
		int     _failedErrCode;

		_Buf readBuf;
		_Buf writeBuf;

		HANDLE sock;
		OVERLAPPED readOver;
		OVERLAPPED writeOver;
	};

private:
	bool haveFailedSlot;

	int maxActiveSockP1;
	int startSocket;
	BYTE *sockFlags;
	//0x01 - active, 0x02 - readPending, 0x04 - writePending, 0x08 - readIncomplete, 0x10 - writeIncomplete, 0x20 - failed
	_Slot *slots;

	int nWaitHandles;//-1 means unknown
	int waitExpectedOp;//if next operation will be as expected, it is still possible to reuse cached waitHandles
	HANDLE *waitHandles;
	int *waitHandlesMap;
	int numberOfSlots;
public:
	PMultiSocketRW( SOCKET queue, int numberOfConnections_ );//queue == INVALID_SOCKET to disable queue
	~PMultiSocketRW();
	int addSocket( SOCKET sock );
	void removeSocket( int slot );//also closes socket
	void shutdownSockets();
	int maxActiveSocket() const
	{
		return maxActiveSockP1;
	}

	int wait( int& slotNum, int timeout );
	//returns wait* 
	int wait( int& slot )
	{
		return wait( slot, INFINITE );
	}

	size_t read( int slot, BYTE* buf, size_t bufSz, bool incomplete = false );
	size_t write( int slot, const BYTE* buf, size_t bufSz, bool incomplete = false );
	size_t getReadSize( int slot ) const;
	size_t getWrittenSize( int slot ) const;
	void forceSlotFailure( int slot, const char* errMsg = "", int errCode = 0 );

	bool slotActive( int slot ) const
	{
		PASSERT4( slot >= 0 && slot < MaxSockets );
		if( slot == 0 || slot >= maxActiveSockP1 )
			return false;
		return ( sockFlags[ slot ] & 0x01 ) != 0;
	}
	bool slotFailed( int slot ) const
	{
		PASSERT4( slot >= 0 && slot < maxActiveSockP1 );
		return ( sockFlags[ slot ] & 0x20 ) != 0;
	}
	bool writePending( int slot ) const
	{
		PASSERT4( slot >= 0 && slot < maxActiveSockP1 );
		return ( sockFlags[ slot ] & 0x04 ) != 0;
	}
	bool readPending( int slot ) const
	{
		PASSERT4( slot >= 0 && slot < maxActiveSockP1 );
		return ( sockFlags[ slot ] & 0x02 ) != 0;
	}

	const char* failedMsg( int slot ) const
	{
		PASSERT4( slot >= 0 && slot < maxActiveSockP1 );
		return slots[ slot ]._failedMsg;
	}
	const char* failedErrMsg( int slot ) const
	{
		PASSERT4( slot >= 0 && slot < maxActiveSockP1 );
		return slots[ slot ]._failedErrMsg;
	}
	int failedErrCode( int slot ) const
	{
		PASSERT4( slot >= 0 && slot < maxActiveSockP1 );
		return slots[ slot ]._failedErrCode;
	}

private:
	void _slotFailed( int slot, const char* msg, const char* errMsg, int errCode = 0 );
	void _slotFailed( int slot, const char* msg, int errCode );
	/*inline*/ void _completeRead(int slot,bool evnReset);
	/*inline*/ void _completeWrite(int slot,bool evnReset);
};

#elif defined( PPOSIX )


#if defined (P_USE_EPOLL)

class PMultiSocketRW
{
public:
	enum { MaxSockets = 640 };//maximum FD_SETSIZE
	enum { waitQueue = 0, waitRead = 1, waitWrite = 2, waitTimeout = 3, waitFailed = 4 };

private:
	enum { _noWait = -1 };

private:
	struct _Buf
	{
		BYTE* buf;
		size_t sz;
		size_t offset;
	};
	struct _Slot
	{
		PString _failedMsg;
		PString _failedErrMsg;
		int     _failedErrCode;

		_Buf readBuf;
		_Buf writeBuf;

		int sock;
	};

private:
	bool haveFailedSlot;
	//	int writtenSlot;
	//	int readSlot;

	BYTE *sockFlags;
	//0x01 - active, 0x02 - readPending, 0x04 - writePending, 0x08 - readIncomplete, 0x10 - writeIncomplete, 0x20 - failed
	_Slot *slots;

	struct epoll_event *epolls;

	int ePoll;
	int nPolled;
	int polledIdx;
	int maxActiveSockP1;
	int numberOfSlots;
public:
	PMultiSocketRW( SOCKET queue, int numberOfConnections_ );//queue == INVALID_SOCKET to disable queue
	~PMultiSocketRW();
	int addSocket( SOCKET sock );
	void removeSocket( int slot );//also closes socket
	void shutdownSockets();
	int maxActiveSocket() const
	{
		return maxActiveSockP1;
	}


	int wait( int& slotNum, int timeout );
	//returns wait*
	int wait( int& slot )
	{
		return wait( slot, -1 );
	}

	size_t read( int slot, BYTE* buf, size_t bufSz, bool incomplete = false );
	size_t write( int slot, const BYTE* buf, size_t bufSz, bool incomplete = false );
	size_t getReadSize( int slot ) const;
	size_t getWrittenSize( int slot ) const;

	void forceSlotFailure( int slot, const char* errMsg = "", int errCode = 0 );

	bool slotActive( int slot ) const
	{
		PASSERT4( slot >= 0 && slot < MaxSockets );
		if( slot == 0 || slot >= maxActiveSockP1 )
			return false;
		return ( sockFlags[ slot ] & 0x01 ) != 0;
	}
	bool slotFailed( int slot ) const
	{
		PASSERT4( slot >= 0 && slot < maxActiveSockP1 );
		return ( sockFlags[ slot ] & 0x20 ) != 0;
	}
	bool writePending( int slot ) const
	{
		PASSERT4( slot >= 0 && slot < maxActiveSockP1 );
		return ( sockFlags[ slot ] & 0x04 ) != 0;
	}
	bool readPending( int slot ) const
	{
		PASSERT4( slot >= 0 && slot < maxActiveSockP1 );
		return ( sockFlags[ slot ] & 0x02 ) != 0;
	}

	const char* failedMsg( int slot ) const
	{
		PASSERT4( slot >= 0 && slot < maxActiveSockP1 );
		return slots[ slot ]._failedMsg;
	}
	const char* failedErrMsg( int slot ) const
	{
		PASSERT4( slot >= 0 && slot < maxActiveSockP1 );
		return slots[ slot ]._failedErrMsg;
	}
	int failedErrCode( int slot ) const
	{
		PASSERT4( slot >= 0 && slot < maxActiveSockP1 );
		return slots[ slot ]._failedErrCode;
	}

private:
	int _addSocket( SOCKET sock );
	int _pollSlot( int & slotNum );
	int _pollSlotIn( int slotNum );
	int _pollSlotOut( int slotNum );
	void _slotFailed( int slot, const char* msg, const char* errMsg, int errCode = 0 );
	void _slotFailed( int slot, const char* msg, int errCode );
};

#else

class PMultiSocketRW
{
public:
	enum { MaxSockets = 32 };//maximum FD_SETSIZE
	enum { waitQueue = 0, waitRead = 1, waitWrite = 2, waitTimeout = 3, waitFailed = 4 };

private:
	enum { _noWait = -1 };

private:
	struct _Buf
	{
		BYTE* buf;
		size_t sz;
		size_t offset;
	};
	struct _Slot
	{
		PString _failedMsg;
		PString _failedErrMsg;
		int     _failedErrCode;

		_Buf readBuf;
		_Buf writeBuf;

		int sock;
	};

private:
	bool haveFailedSlot;
	//	int writtenSlot;
	//	int readSlot;

	BYTE * sockFlags;
	//0x01 - active, 0x02 - readPending, 0x04 - writePending, 0x08 - readIncomplete, 0x10 - writeIncomplete, 0x20 - failed
	_Slot * slots;
	struct pollfd *polls;

	int nPolled;
	int polledIdx;
	int maxActiveSockP1;
	int numberOfSlots;
public:
	PMultiSocketRW( SOCKET queue, int numberOfConnections_ );//queue == INVALID_SOCKET to disable queue
	~PMultiSocketRW();
	int addSocket( SOCKET sock );
	void removeSocket( int slot );//also closes socket
	void shutdownSockets();
	int maxActiveSocket() const
	{
		return maxActiveSockP1;
	}

	int wait( int& slotNum, int timeout );
	//returns wait*
	int wait( int& slot )
	{
		return wait( slot, -1 );
	}

	size_t read( int slot, BYTE* buf, size_t bufSz, bool incomplete = false );
	size_t write( int slot, const BYTE* buf, size_t bufSz, bool incomplete = false );
	size_t getReadSize( int slot ) const;
	size_t getWrittenSize( int slot ) const;

	void forceSlotFailure( int slot, const char* errMsg = "", int errCode = 0 );

	bool slotActive( int slot ) const
	{
		PASSERT4( slot >= 0 && slot < MaxSockets );
		if( slot == 0 || slot >= maxActiveSockP1 )
			return false;
		return ( sockFlags[ slot ] & 0x01 ) != 0;
	}
	bool slotFailed( int slot ) const
	{
		PASSERT4( slot >= 0 && slot < maxActiveSockP1 );
		return ( sockFlags[ slot ] & 0x20 ) != 0;
	}
	bool writePending( int slot ) const
	{
		PASSERT4( slot >= 0 && slot < maxActiveSockP1 );
		return ( sockFlags[ slot ] & 0x04 ) != 0;
	}
	bool readPending( int slot ) const
	{
		PASSERT4( slot >= 0 && slot < maxActiveSockP1 );
		return ( sockFlags[ slot ] & 0x02 ) != 0;
	}

	const char* failedMsg( int slot ) const
	{
		PASSERT4( slot >= 0 && slot < maxActiveSockP1 );
		return slots[ slot ]._failedMsg;
	}
	const char* failedErrMsg( int slot ) const
	{
		PASSERT4( slot >= 0 && slot < maxActiveSockP1 );
		return slots[ slot ]._failedErrMsg;
	}
	int failedErrCode( int slot ) const
	{
		PASSERT4( slot >= 0 && slot < maxActiveSockP1 );
		return slots[ slot ]._failedErrCode;
	}

private:
	int _addSocket( SOCKET sock );
	int _pollSlot( int& retSlot, int slotNum );
	int _pollSlotIn( int slotNum );
	int _pollSlotOut( int slotNum );
	void _slotFailed( int slot, const char* msg, const char* errMsg, int errCode = 0 );
	void _slotFailed( int slot, const char* msg, int errCode );
};
#endif //epoll
#endif



class PUDPSocket : public PBaseSocket//IPv4 socket
{

public:
	PUDPSocket();
	~PUDPSocket(){}

public:
	SOCKET getSocket() const
	{
		return sock;
	}

	void connect( const char* addr, int port );//accepts either numeric or symbolic address
	void bind( int port );
	bool udpSetSendBuffer(int size);
	bool udpSetRecvBuffer(int size);
#ifdef PWIN32
	size_t recv( PIOBUF* b, int bufCount, Descriptor * addr_) const;
	void settimeout(int t);
	int recvnoexc( PIOBUF* b, int bufCount, Descriptor * addr_) const;
	size_t send( PIOBUF* b, int bufCount, size_t toSend, const Descriptor * addr_) const;
	int sendnoexc( PIOBUF* b, int bufCount, size_t toSend, const Descriptor * addr_) const;
#endif
	void address( PString& ret ) const
	{
		_address( ret, ((struct sockaddr_in *)&addr)->sin_addr );
	}
	int port() const
	{
		return _port( *(struct sockaddr_in *)&addr );
	}
	static void address( PString& ret, const Descriptor& descr )
	{
		_address( ret, ((struct sockaddr_in *)&descr.addr)->sin_addr );
	}
	static int port( const Descriptor& descr )
	{
		return _port( *(struct sockaddr_in *)&descr.addr );
	}
private:
	static void _address( PString& ret, const struct in_addr& addr );
	static int _port( const struct sockaddr_in& addr );
};


#endif
