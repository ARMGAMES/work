#include "ppinclude.h"
#include "ppsocket.h"
#include "pperror.h"

static void _translateIp( struct in_addr& ipAddr, const char* hostName )
{
	if( hostName && *hostName )
	{
		bool askDNS = false;
		for( const char* s = hostName; *s ; ++s )
			if( !isdigit( *s ) && *s != '.' )
			{
				askDNS = true;
				break;
			}

		if( askDNS )
		{
			struct hostent* host = gethostbyname( hostName );
			if( host == 0 )
				throw PHostNameError( "Host name not found", "gethostbyname() failed" );
			memcpy( &ipAddr, host->h_addr, host->h_length );
		}
		else
		{
			//0.0.0.0 form
			ipAddr.s_addr = inet_addr( hostName );
			if( ipAddr.s_addr == INADDR_NONE )
				throw PSystemError( "invalid IP address", "inet_addr() failed" );
		}
	}
	else
		ipAddr.s_addr = htonl( INADDR_LOOPBACK );
}

/* static */ void PIPSocket::hostByName( PString& retIp, const char* hostName )
{
	struct in_addr ipAddr;
	_translateIp( ipAddr, hostName );
	_address( retIp, ipAddr );
}

void PIPSocket::connect( const char* hostName, int port )
{
#ifdef P_IOS
	
	struct addrinfo *aip = NULL;

	// getaddrinfo is used for hostname resolution only, port will be assigned directly to the resulting address info
	int error = ::getaddrinfo(hostName, NULL, NULL, &aip);
	if (error != 0 || !aip) {
		throw PSystemError( "Unable to get server address info", error != 0 ? gai_strerror(error) : "empty address info list"  );
	}
	
	// port assignment
	if (aip->ai_family == PF_INET)
		((struct sockaddr_in *) aip->ai_addr)->sin_port = htons(port);
	else
		((struct sockaddr_in6 *) aip->ai_addr)->sin6_port = htons(port);

#else // P_IOS

	struct in_addr ipAddr;
	_translateIp( ipAddr, hostName );
	/*
	if( hostName && *hostName )
	{
		bool askDNS = false;
		for( const char* s = hostName; *s ; ++s )
			if( !isdigit( *s ) && *s != '.' )
			{
				askDNS = true;
				break;
			}

		if( askDNS )
		{
			struct hostent* host = gethostbyname( hostName );
			if( host == 0 )
			throw PSystemError( "Host name not found", "gethostbyname() failed" );
			memcpy( &ipAddr, host->h_addr, host->h_length );
		}
		else
		{
			//0.0.0.0 form
			ipAddr.s_addr = inet_addr( hostName );
			if( ipAddr.s_addr == INADDR_NONE )
			throw PSystemError( "invalid IP address", "inet_addr() failed" );
		}
	}
	else
		ipAddr.s_addr = htonl( INADDR_LOOPBACK );
	*/

	memset( &addr, 0, sizeof(struct sockaddr_in) );
	((struct sockaddr_in *)&addr)->sin_family = AF_INET;
	((struct sockaddr_in *)&addr)->sin_port = htons( static_cast< u_short >( port ) );
	((struct sockaddr_in *)&addr)->sin_addr = ipAddr;
#endif // P_IOS
	
#ifdef PPOSIX
retr_intr:
#endif
	
#ifdef P_IOS
	int err = ::connect( sock, aip->ai_addr, aip->ai_addrlen );
#else // P_IOS
	int err = ::connect( sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in) );
#endif	// P_IOS
	
	if( err )
	{
#ifdef PPOSIX
		if (errno == EINTR)
			goto retr_intr;
#endif

#ifdef P_IOS
		freeaddrinfo(aip);
#endif
		throw PSystemError( "Unable to connect", SOCKETERRNO() );
	}
#ifdef P_IOS
	freeaddrinfo(aip);
#endif
}

#ifdef PPOSIX
void PUnixSocket::connect(const char * addr_ )
{
	memset( &addr, 0, sizeof(struct sockaddr_un) );
	((struct sockaddr_un *)&addr)->sun_family = AF_UNIX;
	strcpy(((struct sockaddr_un *)&addr)->sun_path,addr_);
	int len = sizeof(struct sockaddr_un) - sizeof( ((struct sockaddr_un *)&addr)->sun_path) + strlen(((struct sockaddr_un *)&addr)->sun_path);

#if defined(P_MAC10X) || defined(P_AIX64) || defined(P_AIX32)
	((struct sockaddr_un *)&addr)->sun_len = len + 1;
#endif
retr_intr:
	int err = ::connect( sock, (struct sockaddr *)&addr, len );
	if( err )
	{
		if (errno == EINTR)
			goto retr_intr;
		throw PSystemError( "Unable to connect", SOCKETERRNO() );
	}
}
#endif





#ifdef PPOSIX
bool PIPSocket::connect( const char* hostName, int port, int msecTimeout )
{
	if (msecTimeout == 0)
	{
		connect(  hostName, port);
		return true;
	}

#ifdef P_IOS
	
	struct addrinfo *aip = NULL;
	
	// getaddrinfo is used for hostname resolution only, port will be assigned directly to the resulting address info
	int error = ::getaddrinfo(hostName, NULL, NULL, &aip);
	if (error != 0 || !aip) {
		throw PSystemError( "Unable to get server address info", error != 0 ? gai_strerror(error) : "empty address info list"  );
	}
	
	// port assignment
	if (aip->ai_family == PF_INET)
		((struct sockaddr_in *) aip->ai_addr)->sin_port = htons(port);
	else
		((struct sockaddr_in6 *) aip->ai_addr)->sin6_port = htons(port);
	
#else	// P_IOS
	
	struct in_addr ipAddr;
	_translateIp( ipAddr, hostName );
	memset( &addr, 0, sizeof(struct sockaddr_in) );
	((struct sockaddr_in *)&addr)->sin_family = AF_INET;
	((struct sockaddr_in *)&addr)->sin_port = htons( port );
	((struct sockaddr_in *)&addr)->sin_addr = ipAddr;
#endif	// P_IOS

	int flags = ::fcntl( sock, F_GETFL, 0 );
	::fcntl( sock, F_SETFL, flags | O_NONBLOCK );

#ifdef P_IOS
	int err = ::connect( sock, aip->ai_addr, aip->ai_addrlen );
#else // P_IOS
	int err = ::connect( sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in) );
#endif	// P_IOS

	if( err )
	{
		if (errno != EINPROGRESS)
#ifdef P_IOS
		{
			freeaddrinfo(aip);
#endif	// P_IOS
			throw PSystemError( "Unable to connect", SOCKETERRNO() );
#ifdef P_IOS
		}
#endif	// P_IOS
	}

#ifdef P_IOS
	freeaddrinfo(aip);
#endif	// P_IOS

	fd_set wrset;
	FD_ZERO( &wrset );
	FD_SET( sock, &wrset );

	struct timeval timeout;
	timeout.tv_sec = msecTimeout / 1000;
	timeout.tv_usec = ( msecTimeout % 1000 ) * 1000;
	int sel = ::select( sock + 1, 0,&wrset, 0, &timeout );

	if( sel == 0 )
		return false;
	else if( sel < 0 )
		throw PSystemError( "Error in ::select()", SOCKETERRNO() );

	flags = ::fcntl( sock, F_GETFL, 0 );
	::fcntl( sock, F_SETFL, flags & (~O_NONBLOCK) );
	return true;

}
#else
bool PIPSocket::connect( const char* hostName, int port, int msecTimeout )
{
	if (msecTimeout == 0)
	{
		connect(  hostName,  port);
		return true;
	}
	struct in_addr ipAddr;
	_translateIp( ipAddr, hostName );
	memset( &addr, 0, sizeof(struct sockaddr_in) );
	((struct sockaddr_in *)&addr)->sin_family = AF_INET;
	((struct sockaddr_in *)&addr)->sin_port = htons( static_cast< u_short >( port ) );
	((struct sockaddr_in *)&addr)->sin_addr = ipAddr;

	u_long cmd = 1;
	int err = ::ioctlsocket(sock, FIONBIO, &cmd);
	if( err )
	{
		throw PSystemError( "Connect: Unable to set nonblocking mode", SOCKETERRNO() );
	}
	err = ::connect( sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in) );
	if( err )
	{
		if (SOCKETERRNO() != WSAEWOULDBLOCK) 
			throw PSystemError( "Unable to connect", SOCKETERRNO() );
	}

	fd_set wrset;
	FD_ZERO( &wrset );
	FD_SET( sock, &wrset );

	struct timeval timeout;
	timeout.tv_sec = msecTimeout / 1000;
	timeout.tv_usec = ( msecTimeout % 1000 ) * 1000;
	int sel = ::select( 1, 0,&wrset, 0, &timeout );

	if( sel == 0 )
		return false;
	else if( sel < 0 )
		throw PSystemError( "Error in ::select()", SOCKETERRNO() );
	cmd = 0;
	err = ::ioctlsocket(sock, FIONBIO, &cmd);
	if( err )
	{
		throw PSystemError( "Connect: Unable to set blocking mode", SOCKETERRNO() );
	}
	return true;

}
#endif



