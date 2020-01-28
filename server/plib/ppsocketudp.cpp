#include "ppinclude.h"
#include "ppsocket.h"
#include "pperror.h"
#include "pplogfile.h"

void PUDPSocket::connect( const char* hostName, int port )
{
	struct in_addr ipAddr;
	ipAddr.s_addr = inet_addr( hostName );
	if( ipAddr.s_addr == INADDR_NONE )
		throw PSystemError( "invalid IP address", "inet_addr() failed" );

	memset( &addr, 0, sizeof(struct sockaddr_in) );
	((struct sockaddr_in *)&addr)->sin_family = AF_INET;
	((struct sockaddr_in *)&addr)->sin_port = htons( port );
	((struct sockaddr_in *)&addr)->sin_addr = ipAddr;
#ifdef PPOSIX
retr_intr:
#endif
	int err = ::connect( sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in) );
	if( err )
	{
#ifdef PPOSIX
		if (errno == EINTR)
			goto retr_intr;
#endif
		throw PSystemError( "Unable to connect", SOCKETERRNO() );
	}
}

PUDPSocket::PUDPSocket()
{
	sock = ::socket( PF_INET, SOCK_DGRAM, 0 );
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



/* static */ void PUDPSocket::_address( PString& ret, const struct in_addr& inaddr )
{
	ret = "";
	UINT32 addr = ntohl(inaddr.s_addr);
	ret.appendInt(  (addr >>24 ) & 0xFF ).append( '.' ).appendInt( ( addr >> 16 ) & 0xFF ).append( '.' ).appendInt( ( addr >> 8 ) & 0xFF ).append( '.' ).appendInt( addr & 0xFF  );//cannot use inet_ntoa because of reentrability problems
}

/* static */ int PUDPSocket::_port( const struct sockaddr_in& addr )
{
	return ntohs( addr.sin_port );
}

void PUDPSocket::bind( int port )
{
	int err;
//	int one = 1;
//	err = ::setsockopt( sock, SOL_SOCKET, SO_REUSEADDR, (char*)&one, sizeof(one) );
//	if (err)
//		throw PSystemError( "Unable to reuse socket", SOCKETERRNO() );

	memset( &addr, 0, sizeof(struct sockaddr_in) );
	((struct sockaddr_in *)&addr)->sin_family = AF_INET;
	((struct sockaddr_in *)&addr)->sin_port = htons( port );

	err = ::bind( sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
	if( err )
		throw PSystemError( "Unable to bind socket", SOCKETERRNO() );
}

bool PUDPSocket::udpSetSendBuffer(int size)
{
	return setBuffer( sock, size,1 );
}
bool PUDPSocket::udpSetRecvBuffer(int size)
{
	return setBuffer( sock, size, 0);
}


#ifdef PWIN32
size_t PUDPSocket::recv( PIOBUF* b, int bufCount, Descriptor * addr_) const
{
	DWORD flags = 0;
	DWORD retb;
	int addrSize = sizeof(struct sockaddr);
  	int ret = ::WSARecvFrom( sock, (WSABUF*)b, bufCount, &retb, &flags, (struct sockaddr *)&addr_->addr, &addrSize, NULL, NULL );
	if (ret == SOCKET_ERROR)
		throw PSystemError( "Error reading from socket", SOCKETERRNO() );
	return retb; 
}

void PUDPSocket::settimeout(int t)
{
      DWORD ot = t; //milliseconds
      ::setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&ot, sizeof(DWORD));
}

int PUDPSocket::recvnoexc( PIOBUF* b, int bufCount, Descriptor * addr_) const
{
	DWORD flags = 0;
	DWORD retb;
	int addrSize = sizeof(struct sockaddr);
	int ret = ::WSARecvFrom( sock, (WSABUF*)b, bufCount, &retb, &flags, &addr_->addr, &addrSize, NULL, NULL );
	if (ret == SOCKET_ERROR)
	{
		int  code = SOCKETERRNO();
		if (code == WSAETIMEDOUT)
		{
			retb = 0;
		}
		else if (code == WSAECONNRESET)
		{
			retb = -1;
			PTRACE3("Connection reset by peer: %d",code);
		}
		else
		{
			throw PSystemError( "Error reading from socket", SOCKETERRNO() );
		}
	}
	return retb; 
}



size_t PUDPSocket::send( PIOBUF* b, int bufCount, size_t toSend, const Descriptor * addr_) const
{
	DWORD flags = 0;
	DWORD retb;
	int addrSize = sizeof(struct sockaddr);
	int ret = ::WSASendTo( sock, (WSABUF*)b, bufCount, &retb, flags, (const struct sockaddr *)&addr_->addr, addrSize, NULL, NULL );
	if (ret == SOCKET_ERROR)
		throw PSystemError( "Error sending to socket", SOCKETERRNO() );
	return retb; 
}

int PUDPSocket::sendnoexc( PIOBUF* b, int bufCount, size_t toSend, const Descriptor * addr_) const
{
	DWORD flags = 0;
	DWORD retb;
	int addrSize = sizeof(struct sockaddr);
	int ret = ::WSASendTo( sock, (WSABUF*)b, bufCount, &retb, flags, (const struct sockaddr *)&addr_->addr, addrSize, NULL, NULL );
	if (ret == SOCKET_ERROR)
	{
		int  code = SOCKETERRNO();
		if (code == WSAETIMEDOUT)
		{
			retb = 0;
		}
		else if (code == WSAECONNRESET)
		{
			retb = -1;
			PTRACE3("Connection reset by peer: %d",code);
		}
		else
		{
			throw PSystemError( "Error sending to socket", SOCKETERRNO() );
		}
	}
	return retb; 
}
#endif
