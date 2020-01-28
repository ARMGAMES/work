#include "ppinclude.h"
#include "ppsocket.h"
#include "pplogfile.h"
#include "pperror.h"

#ifdef PWIN32

/* static */ void PSocket::startup()
{
	WSADATA wsaData;
	if( WSAStartup( 0x0101, &wsaData ) != 0 )
		throw PSystemError( "Error initializing sockets", WSAGetLastError() );
}

/* static */ void PSocket::cleanup()
{
	WSACleanup();
}

#else
/* static */ void PSocket::startup() {}
/* static */ void PSocket::cleanup() {}
#endif

PIPSocket::PIPSocket()
{
#ifdef P_IOS
	sock = ::socket( getSocketDomain(), SOCK_STREAM, 0);
#else
	sock = ::socket( PF_INET, SOCK_STREAM, 0 );
#endif
	if( !VALID_SOCKET( sock ) )
		throw PSystemError( "Unable to create socket", SOCKETERRNO() );
#ifdef PPOSIX
	int flags = ::fcntl(sock,F_GETFD,0);
	::fcntl(sock,F_SETFD,flags|FD_CLOEXEC);
#endif

#ifdef P_MAC10X
	int one = 1;
	int err = ::setsockopt( sock, SOL_SOCKET, SO_NOADDRERR, (char*)&one, sizeof(one) );
//#ifdef P_IOS
	one = 1;
	err = ::setsockopt(sock, SOL_SOCKET, SO_NOSIGPIPE, (char*)&one, sizeof(one));
//#endif
#endif

}

#ifdef P_IOS
int PIPSocket::getSocketDomain()
{
	// Any IPv4 address can be used. If IPv4 connection exists, IPv4 address info will be created, otherwise, if IPv6 connection exists, IPv6 address info will be created, otherwise, socket domain wil be defaulted to IPv4 
	const char *address = "8.8.8.8";

	int domain = PF_INET;
	
	struct addrinfo *aip = NULL;
	
	if (getaddrinfo(address, NULL, NULL, &aip) == 0 && aip)
		domain = aip->ai_family;

	if (aip)
		freeaddrinfo(aip);
	
	return domain;
}
#endif

PIPListeningSocket::PIPListeningSocket()
{
#ifdef PPOSIX
	int err = pipe( fd );
	if( err )
		throw PSystemError( "Unable to create pipe", errno );
	int flags = ::fcntl(fd[0],F_GETFD,0);
	::fcntl(fd[0],F_SETFD,flags | FD_CLOEXEC);
	flags = ::fcntl(fd[1],F_GETFD,0);
	::fcntl(fd[1],F_SETFD,flags | FD_CLOEXEC);

#endif
}
#if defined(LINUX)
void PIPListeningSocket::bind(int port)
{
	int one = 1;
	int err = ::setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, (char*)&one, sizeof(one) );
	if (err)
		throw PSystemError( "Unable to reuse socket", SOCKETERRNO() );
	PIPSocket::bind(port);
}
#endif


#ifdef PPOSIX
PUnixSocket::PUnixSocket()
{
	sock = ::socket( AF_UNIX, SOCK_STREAM, 0 );
	if( !VALID_SOCKET( sock ) )
		throw PSystemError( "Unable to create socket", SOCKETERRNO() );
	int flags = ::fcntl(sock,F_GETFD,0);
	::fcntl(sock,F_SETFD,flags|FD_CLOEXEC);
}

PUnixListeningSocket::PUnixListeningSocket()
{
	int err = pipe( fd );
	if( err )
		throw PSystemError( "Unable to create pipe", errno );
	int flags = ::fcntl(fd[0],F_GETFD,0);
	::fcntl(fd[0],F_SETFD,flags | FD_CLOEXEC);
	flags = ::fcntl(fd[1],F_GETFD,0);
	::fcntl(fd[1],F_SETFD,flags | FD_CLOEXEC);
}
PUnixSocket::PUnixSocket( const PUnixSocket::Descriptor& descr )
{
	sock = descr.sock;
	addr = descr.addr;
}

#endif

PIPSocket::PIPSocket( const PIPSocket::Descriptor& descr )
{
	sock = descr.sock;
	addr = descr.addr;
}

void PBaseSocket::close()
{
	if( VALID_SOCKET( sock ) )
	{
#ifdef PPOSIX
		::shutdown(sock,2);
#endif 
		::CLOSESOCKET( sock );
		sock = INVALID_SOCKET;
	}
}

void PIPListeningSocket::close()
{
	if( VALID_SOCKET( sock ) )
	{
#ifdef PPOSIX
		BYTE b = 0;
		::write( fd[1], &b, 1 );
		//		::close( fd[0] );
		//		::close( fd[1] );
		//		fd[0]=INVALID_SOCKET;
		//		fd[1]=INVALID_SOCKET;
#endif 
		::CLOSESOCKET( sock );
		sock = INVALID_SOCKET;
	}
}


PBaseSocket::~PBaseSocket()
{
	if( VALID_SOCKET( sock ) )
	{
		::CLOSESOCKET( sock );
	}
}

PIPListeningSocket::~PIPListeningSocket()
{
#ifdef PPOSIX
	if( VALID_SOCKET( fd[0] ) )
	{
		::close( fd[0] );
	}
	if( VALID_SOCKET( fd[1] ) )
	{
		::close( fd[1] );
	}
#endif
}

#ifdef PPOSIX
void PUnixListeningSocket::close()
{
	if( VALID_SOCKET( sock ) )
	{
		BYTE b = 0;
		::write( fd[1], &b, 1 );
		::CLOSESOCKET( sock );
		sock = INVALID_SOCKET;
	}
}

PUnixListeningSocket::~PUnixListeningSocket()
{
	if( VALID_SOCKET( fd[0] ) )
	{
		::close( fd[0] );
	}
	if( VALID_SOCKET( fd[1] ) )
	{
		::close( fd[1] );
	}
}
#endif 

/* static */ bool PIPSocket::enableKeepAlive( SOCKET sock )
{
	int one = 1;
	int err = ::setsockopt( sock, SOL_SOCKET, SO_KEEPALIVE, (char*)&one, sizeof(one) );
	return err == 0;
}

bool PIPSocket::enableKeepAlive()
{
	return enableKeepAlive( sock );
}

/* static */ bool PIPSocket::tcpNoDelay( SOCKET sock )
{
	int one = 1;
	int err = ::setsockopt( sock, IPPROTO_TCP, TCP_NODELAY, (char*)&one, sizeof(one) );
	return err == 0;
}

/* static */ bool PBaseSocket::setBuffer( SOCKET sock, int size, int type )
{
	int newValue;
	socklen_t newValueSize = sizeof(newValue);
	int err;
	int so = type == 0 ? SO_RCVBUF : SO_SNDBUF;
	const char * ttype =  type == 0 ? "recv":"send";

	getsockopt( sock, SOL_SOCKET, so, (char*)&newValue, &newValueSize);
	PTRACE5("PBaseSocket::setBuffer - original %s buffer=%d, to set=%d",ttype, newValue, size);
	newValueSize = sizeof(int);
	err = ::setsockopt( sock, SOL_SOCKET, so, (char*)&size, sizeof(size) );
	if (err)
	{
		PTRACE3("PBaseSocket::setBuffer(%s) error %d, tried to set %d",ttype, err, size);
		return false;
	}
	newValueSize = sizeof(newValue);
	err = ::getsockopt( sock, SOL_SOCKET, so, (char*)&newValue, &newValueSize);
	if (err || size != newValue)
	{
		PTRACE3("PBaseSocket::setBuffer(%s) getsockopt error %d, tried to set %d, result %d",ttype, err, size, newValue);
		return false;
	}

	return true;
}

bool PIPSocket::tcpNoDelay()
{
	return tcpNoDelay( sock );
}

bool PIPSocket::tcpSetSendBuffer(int size)
{
	return setBuffer( sock, size, 1 );
}
bool PIPSocket::tcpSetRecvBuffer(int size)
{
	return setBuffer( sock, size, 0 );
}
bool PIPSocket::tcpSetBuffer(int size)
{
	return setBuffer( sock, size, 1 ) && setBuffer( sock, size, 2 );
}

size_t PBaseSocket::send( const BYTE* p, size_t sz )
{
#ifdef PPOSIX
retr_intr:
#endif
	int wr = ::send( sock, (const char*)p, static_cast< int >( sz ), 0 );//assuming cast BYTE*->char* is safe
	if( wr < 0 )
	{
#ifdef PPOSIX
		if (errno == EINTR)
			goto retr_intr;
#endif
		throw PSystemError( "Error writing to socket", SOCKETERRNO() );
	}
	return wr;
}

void PBaseSocket::sendComplete( const BYTE* p, size_t sz )
{
	for(;;)
	{
#ifdef PPOSIX
retr_intr:
#endif
		int wr = ::send( sock, (const char*)p, static_cast< int >( sz ), 0 );//assuming cast BYTE*->char* is safe
		if( wr < 0 )
		{
#ifdef PPOSIX
			if (errno == EINTR)
				goto retr_intr;
#endif

			throw PSystemError( "Error writing to socket", SOCKETERRNO() );
		}
		if( ( sz -= wr ) == 0 )
			break;
		p += wr;
	}
}



size_t PBaseSocket::recv( BYTE* p, size_t sz )
{
#ifdef PPOSIX
retr_intr:
#endif
	int rd = ::recv( sock, (char*)p, static_cast< int >( sz ), 0 );//assuming cast BYTE*->char* is safe
	if( rd < 0 )
	{
#ifdef PPOSIX
		if (errno == EINTR)
			goto retr_intr;
#endif
		throw PSystemError( "Error reading from socket", SOCKETERRNO() );
	}
	else if( rd == 0 )
		throw PSystemError( "Socket: connection closed", SOCKETERRNO() );
	return rd;
}

#ifdef PPOSIX
size_t PBaseSocket::recvStream( BYTE* p, size_t sz, int msecTimeout )
{
	struct pollfd poll;
	poll.fd = sock;
	poll.events = POLLIN;
retr_intr:
	int sel = ::poll( &poll, 1, msecTimeout );
	if( sel == 0 )
		return -1;
	else if( sel < 0 )
	{
		if (errno == EINTR)
			goto retr_intr;
		throw PSystemError( "Error in ::select()", SOCKETERRNO() );
	}

	PASSERT5( sel == 1 );
retr_intr_read:
	int rd = ::recv( sock, (char*)p, sz, 0 );//assuming cast BYTE*->char* is safe
	if( rd < 0 )
	{
		if (errno == EINTR)
			goto retr_intr_read;
		throw PSystemError( "Error reading from socket", SOCKETERRNO() );
	}
	return rd;
}
#else
size_t PBaseSocket::recvStream( BYTE* p, size_t sz, int msecTimeout )
{
	fd_set rdset;
	FD_ZERO( &rdset );
	FD_SET( sock, &rdset );

	struct timeval timeout;
	timeout.tv_sec = msecTimeout / 1000;
	timeout.tv_usec = ( msecTimeout % 1000 ) * 1000;
	int sel = ::select( 1, &rdset, 0, 0, &timeout );

	if( sel == 0 )
		return static_cast< size_t >( -1 );
	else if( sel < 0 )
		throw PSystemError( "Error in ::select()", SOCKETERRNO() );

	PASSERT5( sel == 1 );
	int rd = ::recv( sock, (char*)p, static_cast< int >( sz ), 0 );//assuming cast BYTE*->char* is safe
	if( rd < 0 )
		throw PSystemError( "Error reading from socket", SOCKETERRNO() );
	return rd;
}
#endif
size_t PBaseSocket::recv( BYTE* p, size_t sz, int msecTimeout )
{
	size_t ret = recvStream(p,sz,msecTimeout);
	if (ret == static_cast< size_t >( -1 ))
		return 0;
	if (ret == 0)
		throw PSystemError( "Socket: connection closed", SOCKETERRNO() );
	return ret;
}



void PBaseSocket::recvComplete( BYTE* p, size_t sz )
{
	for(;;)
	{
		int rd = ::recv( sock, (char*)p, static_cast< int >( sz ), 0 );//assuming cast BYTE*->char* is safe
		if( rd < 0 )
			throw PSystemError( "Error reading from socket", SOCKETERRNO() );
		else if( rd == 0 )
			throw PSystemError( "Socket: connection closed", SOCKETERRNO() );
		if( ( sz -= rd ) == 0 )
			break;
		p += rd;
	}
}

/* static */ void PIPSocket::_address( PString& ret, const struct in_addr& inaddr )
{
	ret = "";
	UINT32 addr = ntohl(inaddr.s_addr);
	ret.appendInt(  (addr >>24 ) & 0xFF ).append( '.' ).appendInt( ( addr >> 16 ) & 0xFF ).append( '.' ).appendInt( ( addr >> 8 ) & 0xFF ).append( '.' ).appendInt( addr & 0xFF  );//cannot use inet_ntoa because of reentrability problems
}

/* static */ int PIPSocket::_port( const struct sockaddr_in& addr )
{
	return ntohs( addr.sin_port );
}

/* static */ void PIPSocket::ownAddress( PString& ret )
{
	ret = "";
	char hostName[ 256 ];
	int err = gethostname( hostName, sizeof( hostName ) );
	if( !err )
	{
		struct hostent* host = gethostbyname( hostName );
		if( host )
		{
			struct in_addr ipAddr;
			memcpy( &ipAddr, host->h_addr, host->h_length );
			_address( ret, ipAddr );
		}
	}
}
/* static */ void PBaseSocket::address( PString& ret, const Descriptor& descr )
{
#ifdef PPOSIX
	if ( ((struct sockaddr_un *)&descr.addr)->sun_family == AF_UNIX)
		PUnixSocket::address(ret,descr);
	else
		PIPSocket::address(ret,descr);
#else
	PIPSocket::address(ret,descr);
#endif

}
/* static */ bool PBaseSocket::isLocal(const Descriptor& descr )
{
#ifdef PPOSIX
	if (((struct sockaddr_un *)&descr.addr)->sun_family == AF_UNIX)
		return true;
	else
	{
		PString addr;
		PIPSocket::address(addr,descr);
		if( addr.equals( "127.0.0.1" ) )
			return true;
		else 
			return false;
	}
#else
	PString addr;
	PIPSocket::address(addr,descr);
	if( addr.equals( "127.0.0.1" ) )
		return true;
	else 
		return false;
#endif
}
