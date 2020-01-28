#include "ppinclude.h"
#include "pplogfile.h"
#include "ppsocket.h"
#include "pperror.h"
//#include "pplogfile.h"//DEBUG

void PIPSocket::bind( int port )
{
	memset( &addr, 0, sizeof(struct sockaddr_in) );
	((struct sockaddr_in *)&addr)->sin_family = AF_INET;
	((struct sockaddr_in *)&addr)->sin_port = htons(  static_cast< u_short >( port ) );

	int err = ::bind( sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
	if( err )
		throw PSystemError( "Unable to bind socket", SOCKETERRNO() );
}

void PIPListeningSocket::listen( int queueSize )
{
	int err = ::listen( sock, queueSize );
	if( err )
		throw PSystemError( "socket listen failed", SOCKETERRNO() );
}
#ifdef PPOSIX
void PUnixSocket::bind(const char* addr_ )
{
	memset( &addr, 0, sizeof(struct sockaddr_un) );
	((struct sockaddr_un *)&addr)->sun_family = AF_UNIX;
	strcpy(((struct sockaddr_un *)&addr)->sun_path,addr_);
	int len = sizeof(struct sockaddr_un) - sizeof( ((struct sockaddr_un *)&addr)->sun_path) + strlen(((struct sockaddr_un *)&addr)->sun_path);

#if defined(P_MAC10X) || defined(P_AIX64) || defined(P_AIX32)
	((struct sockaddr_un *)&addr)->sun_len = len + 1;
#endif

	::unlink(((struct sockaddr_un *)&addr)->sun_path);
	PTRACE5("PUnixSocket::bind - %s, length=%d, sock=%d",((struct sockaddr_un *)&addr)->sun_path,len,sock);
	int err = ::bind( sock, (struct sockaddr *)&addr, len);
	if( err )
		throw PSystemError( "Unable to bind Unix socket", SOCKETERRNO() );
}

void PUnixListeningSocket::listen( int queueSize )
{
	int err = ::listen( sock, queueSize );
	if( err )
		throw PSystemError( "socket listen failed", SOCKETERRNO() );
}

#endif
//****************************************************************************

#ifdef PWIN32

void PIPListeningSocket::accept( Descriptor& ret )
{
	socklen_t addrSz = sizeof(struct sockaddr_in);
	ret.sock = ::accept( sock, &ret.addr, &addrSz );
	if( !VALID_SOCKET( ret.sock ) )
		throw PSystemError( "socket accept failed", SOCKETERRNO() );
}


PMultiSocketRW::PMultiSocketRW( SOCKET queue, int numberOfConnections_ )
{
	PASSERT3(numberOfConnections_ < MaxSockets);
	numberOfSlots = numberOfConnections_ + 1;
	sockFlags = new BYTE[numberOfSlots];
	slots = new _Slot[numberOfSlots];
	waitHandles = new HANDLE[2 * numberOfSlots];
	waitHandlesMap = new int[2 * numberOfSlots];
	maxActiveSockP1 = 0;
	int slot = addSocket( queue );
    UNUSED(slot);
	PASSERT5( slot == 0 );

	haveFailedSlot = false;
	nWaitHandles = -1;
	waitExpectedOp = 0;
	startSocket=0;
}
PMultiSocketRW::~PMultiSocketRW()
{
	delete[] sockFlags;
	delete[] slots;
	delete[] waitHandles;
	delete[] waitHandlesMap;
}

int PMultiSocketRW::addSocket( SOCKET sock )
{
	int slotNum = -1;
	for( int i=0; i < maxActiveSockP1 ; ++i )
		if( sockFlags[ i ] == 0x00 )
		{
			slotNum = i;
			break;
		}

	if( slotNum < 0 )
	{
		PASSERT4( maxActiveSockP1 < numberOfSlots );
		slotNum = maxActiveSockP1++;
	}

	_Slot& slot = slots[ slotNum ];
	if (slotNum>0)
	{
		HANDLE ev = CreateEvent( NULL, TRUE, FALSE, NULL );
		if( ev == NULL )
			throw PSystemError( "Unable to create event", GetLastError() );
		memset( &slot.readOver, 0, sizeof(OVERLAPPED) );
		slot.readOver.hEvent = ev;
		ev = CreateEvent( NULL, TRUE, FALSE, NULL );
		if( ev == NULL )
		{ 
			CloseHandle( slot.readOver.hEvent );
			throw PSystemError( "Unable to create event", GetLastError() );
		}
		memset( &slot.writeOver, 0, sizeof(OVERLAPPED) );
		slot.writeOver.hEvent = ev;
	}
	slot.sock = (HANDLE)sock;
	sockFlags[ slotNum ] = 0x01;

	nWaitHandles = -1;
	waitExpectedOp = 0;
	return slotNum;
}

void PMultiSocketRW::removeSocket( int slotNum )
{
	PASSERT4( slotNum > 0 && slotNum < maxActiveSockP1 );
	PASSERT4( sockFlags[ slotNum ] & 0x01 );

	_Slot& slot = slots[ slotNum ];
	closesocket( (SOCKET)slot.sock );
	CloseHandle( slot.writeOver.hEvent );
	CloseHandle( slot.readOver.hEvent );
	sockFlags[ slotNum ] = 0x00;

	if (slotNum == maxActiveSockP1 - 1)
	{
		--maxActiveSockP1;
		while (maxActiveSockP1 > 0 && sockFlags[maxActiveSockP1 - 1] == 0x00)
			--maxActiveSockP1;
	}

	nWaitHandles = -1;
	waitExpectedOp = 0;
}

void PMultiSocketRW::shutdownSockets()
{
	for( int i=0; i < maxActiveSockP1 ; ++i )
		if( sockFlags[ i ] & 0x01 )
		{
			_Slot& slot = slots[ i ];
			if( i != 0 )
			{
				closesocket( (SOCKET)slot.sock );
				CloseHandle( slot.writeOver.hEvent );
				CloseHandle( slot.readOver.hEvent );
			}
			sockFlags[ i ] = 0x00;
		}
	maxActiveSockP1 = 0;
	nWaitHandles = -1;
	waitExpectedOp = 0;
}


inline void PMultiSocketRW::_completeRead( int slotNum, bool evnReset )
{
	sockFlags[ slotNum ] &= ~0x02;
	waitExpectedOp = slotNum;
	if( evnReset )
	{
		ResetEvent(slots[ slotNum ].readOver.hEvent);
	}
}

inline void PMultiSocketRW::_completeWrite( int slotNum, bool evnReset )
{
	sockFlags[ slotNum ] &= ~ 0x04;
	waitExpectedOp = -slotNum;
	if( evnReset )
	{
		ResetEvent(slots[ slotNum ].writeOver.hEvent);
	}
}


size_t PMultiSocketRW::read( int slotNum, BYTE* buf, size_t bufSz, bool incomplete )
{
	PASSERT4( slotNum > 0 && slotNum < maxActiveSockP1 );
	PASSERT4( sockFlags[ slotNum ] & 0x01 );
	if( sockFlags[ slotNum ] & 0x20 )
	{
		return 0;//failed
	}
	PASSERT4( ( sockFlags[ slotNum ] & 0x02 ) == 0 );

	if( waitExpectedOp != slotNum )
		nWaitHandles = -1;
	waitExpectedOp = 0;

	_Slot& slot = slots[ slotNum ];
	_Buf& _buf = slot.readBuf;
	_buf.buf = buf;
	_buf.sz = bufSz;
	_buf.offset = 0;
	sockFlags[ slotNum ] |= 0x02 | ( incomplete ? 0x08 : 0x00 );

	DWORD dummy;

	while( ReadFile( slot.sock, _buf.buf+_buf.offset, static_cast< DWORD >( _buf.sz-_buf.offset ),
		&dummy, &slot.readOver ) )
	{
		if(!dummy)
		{
			return 0; //  don't reset event
			//  next wait() handles this situation
		}
		_buf.offset += dummy;

		if ( incomplete || _buf.offset == _buf.sz)
		{
			_completeRead(slotNum,true);
			return _buf.offset;
		}
	}

	DWORD err = GetLastError();
	if( err != ERROR_IO_PENDING )
	{
		_slotFailed( slotNum, "ReadFile", err );
		return 0;
	}
	return 0;

}

size_t PMultiSocketRW::write( int slotNum, const BYTE* buf, size_t bufSz, bool incomplete )
{
	PASSERT4( slotNum > 0 && slotNum < maxActiveSockP1 );
	PASSERT4( slotNum < maxActiveSockP1 );
	PASSERT4( sockFlags[ slotNum ] & 0x01 );
	if( sockFlags[ slotNum ] & 0x20 )
		return 0;//failed
	PASSERT4( ( sockFlags[ slotNum ] & 0x04 ) == 0 );

	if( waitExpectedOp != -slotNum )
		nWaitHandles = -1;
	waitExpectedOp = 0;

	_Slot& slot = slots[ slotNum ];
	_Buf& _buf = slot.writeBuf;
	_buf.buf = (BYTE*)buf;//type cast is safe - WriteFile actually gets const BYTE*
	_buf.sz = bufSz;
	_buf.offset = 0;
	sockFlags[ slotNum ] |= 0x04 | ( incomplete ? 0x10 : 0x00 );

	DWORD dummy;
	while( WriteFile( slot.sock, _buf.buf + _buf.offset, static_cast< DWORD >( _buf.sz-_buf.offset ),
		&dummy, &slot.writeOver ) )
	{
		_buf.offset += dummy;
		if ( incomplete || _buf.offset == _buf.sz)
		{
			_completeWrite(slotNum,true);
			return _buf.offset;
		}
	}

	DWORD err = GetLastError();
	if( err != ERROR_IO_PENDING )
	{
		_slotFailed( slotNum, "WriteFile", err );
		return 0;
	}
	return 0;
}

int PMultiSocketRW::wait( int& retSlot, int timeout )
{
	if( haveFailedSlot )
	{
		for( int i=1; i < maxActiveSockP1 ; ++i )
			if( sockFlags[ i ] & 0x20 )
			{
				retSlot = i;
				return waitFailed;
			}
		haveFailedSlot = false;
	}

//	if( nWaitHandles < 0 || waitExpectedOp != 0 ) - commented out to disable waitHandles/waitHandlesMap caching
	{
		int i,j;
		nWaitHandles = 0;
		if( slots[ 0 ].sock != (HANDLE)INVALID_SOCKET )
		{
			waitHandles[ nWaitHandles ] = slots[ 0 ].sock;
			waitHandlesMap[ nWaitHandles++ ] = 0;
		}
		startSocket++;
		if(startSocket>=maxActiveSockP1)
			startSocket = 1;

		for( j=1,i=startSocket; j < maxActiveSockP1; j++,i++, i=i>=maxActiveSockP1?1:i )
		{
			BYTE flags = sockFlags[ i ];
			if( flags & 0x01 )
			{
				if( flags & 0x04 )
				{
					waitHandles[ nWaitHandles ] = slots[ i ].writeOver.hEvent;
					waitHandlesMap[ nWaitHandles++ ] = -i;
				}
				if( flags & 0x02 )
				{
					waitHandles[ nWaitHandles ] = slots[ i ].readOver.hEvent;
					waitHandlesMap[ nWaitHandles++ ] = i;
				}
			}
		}
		waitExpectedOp = 0;
	}

	for(;;)
	{
		DWORD result = ::WaitForMultipleObjects( nWaitHandles, waitHandles, FALSE, timeout );
		if( result == WAIT_FAILED )
			throw PSystemError( "WaitForMultipleObjects failed", GetLastError() );
		else if( result == WAIT_TIMEOUT )
			return waitTimeout;

		int waitNum = result - WAIT_OBJECT_0;
		PASSERT5( waitNum < nWaitHandles );
		int slotNum = waitHandlesMap[ waitNum ];
		if( slotNum == 0 )//queue
			return waitQueue;
		else if( slotNum < 0 )//write
		{
			slotNum = -slotNum;
			PASSERT5( slotNum > 0 && slotNum < maxActiveSockP1 );
			_Slot& slot = slots[ slotNum ];

			DWORD sz;
			if( !GetOverlappedResult( slot.sock, &slot.writeOver, &sz, FALSE ) )
			{
				_slotFailed( slotNum, "GetOverlappedResult", GetLastError() );
				retSlot = slotNum;
				return waitFailed;
			}

			_Buf& _buf = slot.writeBuf;
			_buf.offset += sz;
			PASSERT5( _buf.offset <= _buf.sz );
			if( ( sockFlags[ slotNum ] & 0x10 ) || _buf.offset == _buf.sz )
			{
				sockFlags[ slotNum ] &= ~0x04;
				waitExpectedOp = -slotNum;
				retSlot = slotNum;
				return waitWrite;
			}

			DWORD dummy;
			if( !WriteFile( slot.sock, _buf.buf + _buf.offset, static_cast< DWORD >( _buf.sz - _buf.offset ), &dummy, &slot.writeOver ) )
			{
				DWORD err = GetLastError();
				if( err != ERROR_IO_PENDING )
				{
					_slotFailed( slotNum, "WriteFile", GetLastError() );
					retSlot = slotNum;
					return waitFailed;
				}
			}

		}
		else//read
		{
			PASSERT5( slotNum < maxActiveSockP1 );
			_Slot& slot = slots[ slotNum ];
			DWORD sz;
			if( !GetOverlappedResult( slot.sock, &slot.readOver, &sz, FALSE ) )
			{
				_slotFailed( slotNum, "GetOverlappedResult", GetLastError() );
				retSlot = slotNum;
				return waitFailed;
			}
			if( sz == 0 )
			{
				_slotFailed( slotNum, "GetOverlappedResult", "Connection closed" );
				retSlot = slotNum;
				return waitFailed;
			}
			_Buf& _buf = slot.readBuf;
			_buf.offset += sz;
			PASSERT5( _buf.offset <= _buf.sz );

			if( ( sockFlags[ slotNum ] & 0x08 ) || _buf.offset == _buf.sz )
			{
				sockFlags[ slotNum ] &= ~0x02;
				waitExpectedOp = slotNum;
				retSlot = slotNum;
				return waitRead;
			}
			DWORD dummy;
			if( !ReadFile( slot.sock, _buf.buf + _buf.offset, static_cast< DWORD >( _buf.sz - _buf.offset ), &dummy, &slot.readOver ) )
			{
				DWORD err = GetLastError();
				if( err != ERROR_IO_PENDING )
				{
					_slotFailed( slotNum, "ReadFile", GetLastError() );
					retSlot = slotNum;
					return waitFailed;
				}
			}
		}//read
	}//forever
}

void PMultiSocketRW::_slotFailed( int slotNum, const char* msg, const char* errMsg, int errCode )
{
	if( sockFlags[ slotNum ] & 0x20 )
		return;//already failed
	haveFailedSlot = true;
	sockFlags[ slotNum ] |= 0x20;
	slots[ slotNum ]._failedMsg = msg;
	slots[ slotNum ]._failedErrMsg = errMsg;
	slots[ slotNum ]._failedErrCode = errCode;
}

void PMultiSocketRW::_slotFailed( int slotNum, const char* msg, int errCode )
{
	if( sockFlags[ slotNum ] & 0x20 )
		return;//already failed
	haveFailedSlot = true;
	sockFlags[ slotNum ] |= 0x20;
	slots[ slotNum ]._failedMsg = msg;
	PSystemError::errorMessage( slots[ slotNum ]._failedErrMsg, errCode );
	slots[ slotNum ]._failedErrCode = errCode;
}

void PMultiSocketRW::forceSlotFailure( int slot, const char* errMsg, int errCode )
{
	_slotFailed( slot, "forceSlotFailure", errMsg, errCode );
}

size_t PMultiSocketRW::getReadSize( int slotNum ) const
{
	PASSERT4( ( sockFlags[ slotNum ] & 0x02 ) == 0 );
	return slots[ slotNum ].readBuf.offset;
}

size_t PMultiSocketRW::getWrittenSize( int slotNum ) const
{
	PASSERT4( ( sockFlags[ slotNum ] & 0x04 ) == 0 );
	return slots[ slotNum ].writeBuf.offset;
}

//****************************************************************************

#elif defined( PPOSIX )


void PIPListeningSocket::accept( Descriptor& ret )
{
	struct pollfd polls[ 2 ];
	polls[ 0 ].fd = fd[0];
	polls[ 0 ].events = POLLIN;
	polls[ 1 ].fd = sock;
	polls[ 1 ].events = POLLIN;
retr_intr:
	int err = ::poll(polls,2,-1);
	if (err>0)
	{
		if (polls[1].revents & POLLIN)
		{
			socklen_t addrSz = sizeof(struct sockaddr);
			ret.sock = ::accept( sock, (struct sockaddr *)&ret.addr, &addrSz );
			if( !VALID_SOCKET( ret.sock ) )
			{
				if (errno == ECONNABORTED || errno == ECONNRESET)
				{
					PTRACE3("Accept socket: errno =%d, retry",errno);
					goto retr_intr;
				}
				throw PSystemError( "socket accept failed", errno );
			}
			int flags = ::fcntl(ret.sock,F_GETFD,0);
			::fcntl(ret.sock,F_SETFD,flags|FD_CLOEXEC);

			return;
		}
		else
			throw PSystemError( "socket accept closed", 0 );

	}
	else
	{
		if (errno == EINTR)
			goto retr_intr;
		throw PSystemError( "socket accept failed", errno );
	}
}

void PUnixListeningSocket::accept( Descriptor& ret )
{
	struct pollfd polls[ 2 ];
	polls[ 0 ].fd = fd[0];
	polls[ 0 ].events = POLLIN;
	polls[ 1 ].fd = sock;
	polls[ 1 ].events = POLLIN;
retr_intr:
	int err = ::poll(polls,2,-1);
	if (err>0)
	{
		if (polls[1].revents & POLLIN)
		{
			socklen_t addrSz = sizeof(ret.addr);
			ret.sock = ::accept( sock, (struct sockaddr*)&ret.addr, &addrSz );
			if( !VALID_SOCKET( ret.sock ) )
				throw PSystemError( "socket accept failed", errno );
			int flags = ::fcntl(ret.sock,F_GETFD,0);
			::fcntl(ret.sock,F_SETFD,flags|FD_CLOEXEC);

			return;
		}
		else
			throw PSystemError( "socket accept closed", 0 );

	}
	else
	{
		if (errno == EINTR)
			goto retr_intr;

		throw PSystemError( "socket accept failed", errno );
	}
}

#if defined (P_USE_EPOLL)

PMultiSocketRW::PMultiSocketRW( SOCKET queue, int numberOfConnections_ )
{
	PASSERT3(numberOfConnections_ < MaxSockets);
	numberOfSlots = numberOfConnections_ + 1;
	sockFlags = new BYTE[numberOfSlots];
	slots = new _Slot[numberOfSlots];
	epolls = new epoll_event[ numberOfSlots];

	maxActiveSockP1 = 0;
	polledIdx = nPolled = 0;
	haveFailedSlot = false;
	ePoll=::epoll_create1(EPOLL_CLOEXEC);
	if( ePoll == -1 )
		throw PSystemError( "Unable to create epoll", errno );
	int slotNum = _addSocket( queue );
	PASSERT5( slotNum == 0 );
}

PMultiSocketRW::~PMultiSocketRW()
{
	delete[] sockFlags;
	delete[] slots;
	delete[] epolls;
}

int PMultiSocketRW::_addSocket( SOCKET sock )
{
	int slotNum = -1;
	for( int i=0; i < maxActiveSockP1 ; ++i )
		if( sockFlags[ i ] == 0x00 )
		{
			slotNum = i;
			break;
		}
	if( slotNum < 0 )
	{
		PASSERT4( maxActiveSockP1 < numberOfSlots );
		slotNum = maxActiveSockP1++;
	}
	_Slot& slot = slots[ slotNum ];
	slot.sock = sock;
	sockFlags[ slotNum ] = 0x01;
	struct epoll_event polls;
	polls.data.u32 = slotNum;
	polls.events = (slotNum == 0) ? EPOLLIN : EPOLLRDHUP;
	if (::epoll_ctl(ePoll,EPOLL_CTL_ADD,sock,&polls)==-1)
		throw PSystemError( "Unable to add socket to epoll", errno );

	return slotNum;
}

int PMultiSocketRW::addSocket( SOCKET sock )
{
	int flags = ::fcntl( sock, F_GETFL, 0 );
	::fcntl( sock, F_SETFL, flags | O_NONBLOCK );
	return _addSocket( sock );
}

void PMultiSocketRW::removeSocket( int slotNum )
{
	PASSERT4( slotNum > 0 && slotNum < maxActiveSockP1 );
	PASSERT4( sockFlags[ slotNum ] & 0x01 );

	_Slot& slot = slots[ slotNum ];
	struct epoll_event polls;
	polls.data.u32 = slotNum;
	polls.events = 0;
	if(::epoll_ctl(ePoll,EPOLL_CTL_DEL,slot.sock,&polls)==-1)
		throw PSystemError( "Unable to delete socket from epoll", errno );
	::shutdown(slot.sock,2);
	::close( slot.sock );
	sockFlags[ slotNum ] = 0x00;
	if (slotNum == maxActiveSockP1 - 1)
	{
		--maxActiveSockP1;
		while (maxActiveSockP1 > 0 && sockFlags[maxActiveSockP1 - 1] == 0x00)
			--maxActiveSockP1;
	}
	
}

void PMultiSocketRW::shutdownSockets()
{

	for( int i=0; i < maxActiveSockP1 ; ++i )
		if( sockFlags[ i ] & 0x01 )
		{
			_Slot& slot = slots[ i ];
			if ( i != 0)
			{
				::shutdown(slot.sock,2);
				::close( slot.sock );
			}
			sockFlags[ i ] = 0x00;
		}
	maxActiveSockP1 = 0;
	nPolled = 0;
	::close(ePoll);
}

size_t PMultiSocketRW::read( int slotNum, BYTE* buf, size_t bufSz, bool incomplete )
{
	//PTRACE5( "%scomplete read: %d bytes from slot %d", incomplete ? "in" : "", bufSz, slotNum );
	//PTRACE5( "read: slot %d: flags=%X", slotNum, sockFlags[ slotNum ] );

	PASSERT4( slotNum > 0 && slotNum < maxActiveSockP1 );
	PASSERT4( sockFlags[ slotNum ] & 0x01 );
	if( sockFlags[ slotNum ] & 0x20 )
		return 0;//failed
	PASSERT4( ( sockFlags[ slotNum ] & 0x02 ) == 0 );

	_Slot& slot = slots[ slotNum ];
	_Buf& _buf = slot.readBuf;
	_buf.buf = buf;
	_buf.sz = bufSz;
	_buf.offset = 0;

	sockFlags[ slotNum ] |= 0x02 | ( incomplete ? 0x08 : 0x00 );

	int rd = ::recv( slot.sock, buf, bufSz, 0 );
	if( rd < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
		rd = 0;
	else if( rd < 0 )
	{
		_slotFailed( slotNum, "recv", errno );
		return 0;
	}
	else if( rd == 0 )
	{
		_slotFailed( slotNum, "recv", "Connection closed by peer",0);
		return 0;
	}

	_buf.offset += rd;
	//PTRACE5( "slot %d recv: %d read, offset=%d, sz=%d", slotNum, rd, _buf.offset, _buf.sz );

	if( _buf.offset == _buf.sz || (incomplete && rd > 0) )
	{
		sockFlags[ slotNum ] &= ~0x02;
		//PTRACE5( "slot %d recv complete op", slotNum );

		return _buf.offset;
	}
	else
	{
		struct epoll_event polls;
		polls.data.u32 = slotNum;
		polls.events = (sockFlags[ slotNum ] & 0x04) ? (EPOLLOUT|EPOLLIN|EPOLLRDHUP) : (EPOLLIN|EPOLLRDHUP);
		if (::epoll_ctl(ePoll,EPOLL_CTL_MOD,slot.sock,&polls)==-1)
			throw PSystemError( "Read: Unable to modify socket in epoll", errno );
		return 0;
		//PTRACE5( "read: slot %d: flags=%X", slotNum, sockFlags[ slotNum ] );
	}
}

size_t PMultiSocketRW::write( int slotNum, const BYTE* buf, size_t bufSz, bool incomplete )
{
	//PTRACE5( "write: %d bytes to slot %d", bufSz, slotNum );
	PASSERT4( slotNum > 0 && slotNum < maxActiveSockP1 );
	PASSERT4( sockFlags[ slotNum ] & 0x01 );
	if( sockFlags[ slotNum ] & 0x20 )
		return 0;//failed
	PASSERT4( ( sockFlags[ slotNum ] & 0x04 ) == 0 );

	_Slot& slot = slots[ slotNum ];
	_Buf& _buf = slot.writeBuf;
	_buf.buf = (BYTE*)buf;//type cast is safe - ::send actually gets const BYTE*
	_buf.sz = bufSz;
	_buf.offset = 0;

	sockFlags[ slotNum ] |= 0x04 | ( incomplete ? 0x10 : 0x00 );

	int wr = ::send( slot.sock, buf, bufSz, MSG_NOSIGNAL);
	if( wr < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
		wr = 0;
	else if( wr < 0 )
	{
		_slotFailed( slotNum, "send", errno );
		return 0;
	}

	_buf.offset += wr;
	//PTRACE5( "slot %d write: %d written, offset=%d, sz=%d", slotNum, wr, _buf.offset, _buf.sz );

	if( _buf.offset == _buf.sz || (incomplete && wr > 0))
	{
		sockFlags[ slotNum ] &= ~0x04;
		//PTRACE5( "slot %d write complete op", slotNum );
		return _buf.offset;
	}
	else
	{
		struct epoll_event polls;
		polls.data.u32 = slotNum;
		polls.events = (sockFlags[ slotNum ] & 0x02) ? (EPOLLOUT|EPOLLIN|EPOLLRDHUP) : (EPOLLOUT|EPOLLRDHUP);
		if (::epoll_ctl(ePoll,EPOLL_CTL_MOD,slot.sock,&polls)==-1)
			throw PSystemError( "Read: Unable to modify socket in epoll", errno );
		return 0;
		//PTRACE5( "write: slot %d: flags=%X", slotNum, sockFlags[ slotNum ] );
	}
}

int PMultiSocketRW::_pollSlotIn( int slotNum )
{

	PASSERT5( sockFlags[ slotNum ] & 0x02 );

	_Slot& slot = slots[ slotNum ];
	_Buf& buf = slot.readBuf;


	int rd = ::recv( slot.sock, buf.buf + buf.offset, buf.sz - buf.offset, 0 );
	if( rd < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
		rd = 0;
	else if( rd < 0 )
	{
		_slotFailed( slotNum, "recv", errno );
		return waitFailed;
	}
	else if( rd == 0 )
	{
		_slotFailed( slotNum, "recv", "Connection closed by peer",0);
		return waitFailed;
	}

	buf.offset += rd;
	//PTRACE5( "pollSlotIn %d recv: %d read, offset=%d, sz=%d", slotNum, rd, buf.offset, buf.sz );

	if( buf.offset == buf.sz || ( (sockFlags[ slotNum ] & 0x08) && rd >0 ) )
	{
		sockFlags[ slotNum ] &= ~0x02;
		PASSERT5( ( sockFlags[ slotNum ] & 0x02 ) == 0 );
		struct epoll_event polls;
		polls.data.u32 = slotNum;
		polls.events = (sockFlags[ slotNum ] & 0x04) ? (EPOLLOUT|EPOLLRDHUP) : (EPOLLRDHUP);
		if (::epoll_ctl(ePoll,EPOLL_CTL_MOD,slot.sock,&polls)==-1)
			throw PSystemError( "Read: Unable to modify socket in epoll", errno );

		//PTRACE5( "pollSlotIn: slot %d: flags=%X", slotNum, sockFlags[ slotNum ] );
		return waitRead;
	}
	return _noWait;
}

int PMultiSocketRW::_pollSlotOut( int slotNum )
{
	PASSERT5( sockFlags[ slotNum ] & 0x04 );

	_Slot& slot = slots[ slotNum ];
	_Buf& buf = slot.writeBuf;

	int wr = ::send( slot.sock, buf.buf + buf.offset, buf.sz - buf.offset, MSG_NOSIGNAL );
	if( wr < 0 && (errno == EAGAIN || errno == EWOULDBLOCK))
		wr = 0;
	else if( wr < 0 )
	{
		_slotFailed( slotNum, "send", errno );
		return waitFailed;
	}
	buf.offset += wr;
	//PTRACE5( "pollSlotOut slot %d write: %d written, offset=%d, sz=%d", slotNum, wr, buf.offset, buf.sz );

	if( buf.offset == buf.sz || ( ( sockFlags[ slotNum ] & 0x10 ) && wr > 0 ) )
	{
		sockFlags[ slotNum ] &= ~0x04;
		//PTRACE5( "pollSlotOut: slot %d: flags=%X", slotNum, sockFlags[ slotNum ] );
		struct epoll_event polls;
		polls.data.u32 = slotNum;
		polls.events = (sockFlags[ slotNum ] & 0x02) ? (EPOLLIN|EPOLLRDHUP) : (EPOLLRDHUP);
		if (::epoll_ctl(ePoll,EPOLL_CTL_MOD,slot.sock,&polls)==-1)
			throw PSystemError( "Read: Unable to modify socket in epoll", errno );
		return waitWrite;
	}
	return _noWait;
}

int PMultiSocketRW::_pollSlot(int &slotNum)
{
	PASSERT5( polledIdx >= 0 && polledIdx < numberOfSlots );
	int revents = epolls[ polledIdx ].events;
	slotNum = epolls[ polledIdx ].data.u32;
	if ( slotNum >= maxActiveSockP1 || !(sockFlags[ slotNum ] & 0x01) )
		return _noWait;

	if( revents == 0 )
		return _noWait;

	if(slotNum == 0)
	{
		if( revents & ( EPOLLERR | EPOLLHUP ) )
			throw PSystemError( "pipe failed", "EPOLLERR" );
		PASSERT3( revents == EPOLLIN);
		return waitQueue;
	}
	if( revents & ( EPOLLERR  ) )
	{
		_slotFailed( slotNum, "EPOLLERR", "Connection error" );
		return waitFailed;
	}
	else if( revents & (EPOLLHUP|EPOLLRDHUP) )
	{
		_slotFailed( slotNum, "EPOLLHUP", "Connection closed" );
		return waitFailed;
	}
	else if( revents & POLLOUT )
	{
		//PTRACE5( "slot %d POLLOUT: flags=%X", slotNum, sockFlags[ slotNum ] );
		return _pollSlotOut( slotNum );
	}
	else if( revents & POLLIN )
	{
		//PTRACE5( "slot %d POLLIN: flags=%X", slotNum, sockFlags[ slotNum ] );
		return _pollSlotIn( slotNum );

	}

	else
	{
		PASSERT( 0 );
		return _noWait;
	}
}

int PMultiSocketRW::wait( int& retSlot, int timeout )
{
	if( haveFailedSlot )
	{
		for( int i=1; i < maxActiveSockP1 ; ++i )
			if( sockFlags[ i ] & 0x20 )
			{
				retSlot = i;
				return waitFailed;
			}
		haveFailedSlot = false;
	}



	for(;;)
	{
		while(polledIdx < nPolled)
		{
			int ret = _pollSlot(retSlot);
			++polledIdx;
			if( ret != _noWait )
			{
				return ret;
			}
		}
		polledIdx = 0;
retr_intr:
		nPolled = ::epoll_wait( ePoll, epolls, maxActiveSockP1, timeout );
		if( nPolled < 0 )
		{
			if (errno == EINTR)
				goto retr_intr;
			throw PSystemError( "poll failed", errno );
		}
		else if( nPolled == 0 )
			return waitTimeout;
		//PTRACE5( "poll: nPolled=%d", nPolled );
	}//forever
}


#else

PMultiSocketRW::PMultiSocketRW( SOCKET queue, int numberOfConnections_ )
{
	PASSERT3(numberOfConnections_ < MaxSockets);
	numberOfSlots = numberOfConnections_ + 1;
	sockFlags = new BYTE[numberOfSlots];
	slots = new _Slot[numberOfSlots];
	polls = new pollfd[ numberOfSlots];
	maxActiveSockP1 = 0;
	int slot = _addSocket( queue );
	PASSERT5( slot == 0 );
	polls[ slot ].events = POLLIN;
	haveFailedSlot = false;
	nPolled = 0;
}

PMultiSocketRW::~PMultiSocketRW()
{
	delete[] sockFlags;
	delete[] slots;
	delete[] polls;
}

int PMultiSocketRW::_addSocket( SOCKET sock )
{
	int slotNum = -1;
	for( int i=0; i < maxActiveSockP1 ; ++i )
		if( sockFlags[ i ] == 0x00 )
		{
			slotNum = i;
			break;
		}

	if( slotNum < 0 )
	{
		PASSERT4( maxActiveSockP1 < MaxSockets );
		slotNum = maxActiveSockP1++;
	}

	_Slot& slot = slots[ slotNum ];
	polls[ slotNum ].fd = sock;
	polls[ slotNum ].events = 0;
	polls[ slotNum ].revents = 0;

	slot.sock = sock;
	sockFlags[ slotNum ] = 0x01;
	return slotNum;
}

int PMultiSocketRW::addSocket( SOCKET sock )
{
	int flags = ::fcntl( sock, F_GETFL, 0 );
	::fcntl( sock, F_SETFL, flags | O_NONBLOCK );
	return _addSocket( sock );
}

void PMultiSocketRW::removeSocket( int slotNum )
{
	PASSERT4( slotNum > 0 && slotNum < maxActiveSockP1 );
	PASSERT4( sockFlags[ slotNum ] & 0x01 );

	_Slot& slot = slots[ slotNum ];
	::shutdown(slot.sock,2);
	::close( slot.sock );

	sockFlags[ slotNum ] = 0x00;
	if (slotNum == maxActiveSockP1 - 1)
	{
		--maxActiveSockP1;
		while (maxActiveSockP1 > 0 && sockFlags[maxActiveSockP1 - 1] == 0x00)
			--maxActiveSockP1;
	}

	polls[ slotNum ].events = 0;
	polls[ slotNum ].revents = 0;
	polls[ slotNum ].fd = -1;
}

void PMultiSocketRW::shutdownSockets()
{

	for( int i=0; i < maxActiveSockP1 ; ++i )
		if( sockFlags[ i ] & 0x01 )
		{
			_Slot& slot = slots[ i ];
			if ( i != 0)
			{
				::shutdown(slot.sock,2);
				::close( slot.sock );
			}
			sockFlags[ i ] = 0x00;
			polls[ i ].events = 0;
			polls[ i ].revents = 0;
			polls[ i ].fd = -1;
		}
	maxActiveSockP1 = 0;
	nPolled = 0;
}

size_t PMultiSocketRW::read( int slotNum, BYTE* buf, size_t bufSz, bool incomplete )
{
	//PTRACE5( "%scomplete read: %d bytes from slot %d", incomplete ? "in" : "", bufSz, slotNum );
	//PTRACE5( "read: slot %d: flags=%X", slotNum, sockFlags[ slotNum ] );

	PASSERT4( slotNum > 0 && slotNum < maxActiveSockP1 );
	PASSERT4( sockFlags[ slotNum ] & 0x01 );
	if( sockFlags[ slotNum ] & 0x20 )
		return 0;//failed
	PASSERT4( ( sockFlags[ slotNum ] & 0x02 ) == 0 );

	_Slot& slot = slots[ slotNum ];
	_Buf& _buf = slot.readBuf;
	_buf.buf = buf;
	_buf.sz = bufSz;
	_buf.offset = 0;

	sockFlags[ slotNum ] |= 0x02 | ( incomplete ? 0x08 : 0x00 );

	int rd = ::recv( slot.sock, buf, bufSz, 0 );
	if( rd < 0 && errno == EAGAIN )
		rd = 0;
	else if( rd < 0 )
	{
		_slotFailed( slotNum, "recv", errno );
		return 0;
	}
	else if( rd == 0 )
	{
		_slotFailed( slotNum, "recv", "Connection closed by peer",0);
		return 0;
	}

	_buf.offset += rd;
	//PTRACE5( "slot %d recv: %d read, offset=%d, sz=%d", slotNum, rd, _buf.offset, _buf.sz );

	if( _buf.offset == _buf.sz || (incomplete && rd > 0) )
	{
		sockFlags[ slotNum ] &= ~0x02;
		//PTRACE5( "slot %d recv complete op", slotNum );

		return _buf.offset;
	}
	else
	{
		polls[ slotNum ].events |= POLLIN;
		//PTRACE5( "read: slot %d: flags=%X", slotNum, sockFlags[ slotNum ] );
	}
	return 0;
}

size_t PMultiSocketRW::write( int slotNum, const BYTE* buf, size_t bufSz, bool incomplete )
{
	//PTRACE5( "write: %d bytes to slot %d", bufSz, slotNum );
	PASSERT4( slotNum > 0 && slotNum < maxActiveSockP1 );
	PASSERT4( sockFlags[ slotNum ] & 0x01 );
	if( sockFlags[ slotNum ] & 0x20 )
		return 0;//failed
	PASSERT4( ( sockFlags[ slotNum ] & 0x04 ) == 0 );

	_Slot& slot = slots[ slotNum ];
	_Buf& _buf = slot.writeBuf;
	_buf.buf = (BYTE*)buf;//type cast is safe - ::send actually gets const BYTE*
	_buf.sz = bufSz;
	_buf.offset = 0;

	sockFlags[ slotNum ] |= 0x04 | ( incomplete ? 0x10 : 0x00 );

	int wr = ::send( slot.sock, buf, bufSz, 0 );
	if( wr < 0 && errno == EAGAIN )
		wr = 0;
	else if( wr < 0 )
	{
		_slotFailed( slotNum, "send", errno );
		return 0;
	}

	_buf.offset += wr;
	//PTRACE5( "slot %d write: %d written, offset=%d, sz=%d", slotNum, wr, _buf.offset, _buf.sz );

	if( _buf.offset == _buf.sz || (incomplete && wr > 0))
	{
		sockFlags[ slotNum ] &= ~0x04;
		//PTRACE5( "slot %d write complete op", slotNum );
		return _buf.offset;
	}
	else
	{
		polls[ slotNum ].events |= POLLOUT;
		//PTRACE5( "write: slot %d: flags=%X", slotNum, sockFlags[ slotNum ] );
	}
	return 0;
}

int PMultiSocketRW::_pollSlotIn( int slotNum )
{
	PASSERT5( polls[ slotNum ].events & POLLIN );
	PASSERT5( sockFlags[ slotNum ] & 0x02 );

	_Slot& slot = slots[ slotNum ];
	_Buf& buf = slot.readBuf;

	polls[ slotNum ].revents &= ~POLLIN;//for safe second pass via nPolled
	int rd = ::recv( slot.sock, buf.buf + buf.offset, buf.sz - buf.offset, 0 );
	if( rd < 0 && errno == EAGAIN )
		rd = 0;
	else if( rd < 0 )
	{
		_slotFailed( slotNum, "recv", errno );
		return waitFailed;
	}
	else if( rd == 0 )
	{
		_slotFailed( slotNum, "recv", "Connection closed by peer",0);
		return waitFailed;
	}

	buf.offset += rd;
	//PTRACE5( "pollSlotIn %d recv: %d read, offset=%d, sz=%d", slotNum, rd, buf.offset, buf.sz );

	if( buf.offset == buf.sz || ( (sockFlags[ slotNum ] & 0x08)&& rd >0 ) )
	{
		sockFlags[ slotNum ] &= ~0x02;
		polls[ slotNum ].events &= ~POLLIN;
		PASSERT5( ( sockFlags[ slotNum ] & 0x02 ) == 0 );
		//PTRACE5( "pollSlotIn: slot %d: flags=%X", slotNum, sockFlags[ slotNum ] );
		return waitRead;
	}
	return _noWait;
}

int PMultiSocketRW::_pollSlotOut( int slotNum )
{
	PASSERT5( polls[ slotNum ].events & POLLOUT );
	PASSERT5( sockFlags[ slotNum ] & 0x04 );

	_Slot& slot = slots[ slotNum ];
	_Buf& buf = slot.writeBuf;
	polls[ slotNum ].revents &= ~POLLOUT;//for safe second pass via nPolled

	int wr = ::send( slot.sock, buf.buf + buf.offset, buf.sz - buf.offset, 0 );
	if( wr < 0 && errno == EAGAIN )
		wr = 0;
	else if( wr < 0 )
	{
		_slotFailed( slotNum, "send", errno );
		return waitFailed;
	}
	buf.offset += wr;
	//PTRACE5( "pollSlotOut slot %d write: %d written, offset=%d, sz=%d", slotNum, wr, buf.offset, buf.sz );

	if( buf.offset == buf.sz || ( ( sockFlags[ slotNum ] & 0x10 ) && wr > 0 ) )
	{
		sockFlags[ slotNum ] &= ~0x04;
		polls[ slotNum ].events &= ~POLLOUT;
		//PTRACE5( "pollSlotOut: slot %d: flags=%X", slotNum, sockFlags[ slotNum ] );
		return waitWrite;
	}
	return _noWait;
}

int PMultiSocketRW::_pollSlot( int& retSlot, int slotNum )
{
	PASSERT5( slotNum > 0 && slotNum < maxActiveSockP1 );
	PASSERT4( sockFlags[ slotNum ] & 0x01 );

	int revents = polls[ slotNum ].revents;
	if( revents == 0 )
		return _noWait;
	if( revents & ( POLLNVAL | POLLERR ) )
	{
		_slotFailed( slotNum, "POLLERR", "Connection error" );
		retSlot = slotNum;
		return waitFailed;
	}
	else if( revents & POLLHUP )
	{
		_slotFailed( slotNum, "POLLHUP", "Connection closed" );
		retSlot = slotNum;
		return waitFailed;
	}
	else if( revents & POLLOUT )
	{
		//PTRACE5( "slot %d POLLOUT: flags=%X", slotNum, sockFlags[ slotNum ] );
		int ret = _pollSlotOut( slotNum );
		retSlot = slotNum;
		return ret;
	}
	else if( revents & POLLIN )
	{
		//PTRACE5( "slot %d POLLIN: flags=%X", slotNum, sockFlags[ slotNum ] );

		int ret = _pollSlotIn( slotNum );
		retSlot = slotNum;
		return ret;
	}

	else
	{
		PASSERT( 0 );
		return _noWait;
	}
}

int PMultiSocketRW::wait( int& retSlot, int timeout )
{
	if( haveFailedSlot )
	{
		for( int i=1; i < maxActiveSockP1 ; ++i )
			if( sockFlags[ i ] & 0x20 )
			{
				retSlot = i;
				return waitFailed;
			}
		haveFailedSlot = false;
	}

	//	if( writtenSlot >= 0 )
	//		{
	//		retSlot = writtenSlot;
	//		writtenSlot = -1;
	//PTRACE( "wait: writeSlot" );
	//		return waitWrite;
	//		}
	//	else if( readSlot >= 0 )
	//		{
	//		retSlot = readSlot;
	//		readSlot = -1;
	//PTRACE( "wait: readSlot" );
	//		PASSERT5( ( sockFlags[ retSlot ] & 0x02 ) == 0 );
	//		return waitRead;
	//		}

	if( nPolled > 0 )
	{
		for( ; polledIdx < maxActiveSockP1 ; ++polledIdx )
		{
			if ( sockFlags[polledIdx] & 0x01 )
			{
				int ret = _pollSlot( retSlot, polledIdx );
				if( ret != _noWait )
				{
					++polledIdx;
					--nPolled;
					return ret;
				}
			}
			else
			{
				++polledIdx;
				--nPolled;
			}
		}
		nPolled = 0; //should not happen but can be ignored 
	}

	for(;;)
	{
		//PTRACE5( "poll" );
		//for( int i=0; i < maxActiveSockP1 ; ++i )
		//{
		//PTRACE5( "\tslot=%d events=%X", i, polls[ i ].events );
		//}
retr_intr:
		nPolled = ::poll( polls, maxActiveSockP1, timeout );
		if( nPolled < 0 )
		{
			if (errno == EINTR)
				goto retr_intr;
			throw PSystemError( "poll failed", errno );
		}
		else if( nPolled == 0 )
			return waitTimeout;
		//PTRACE5( "poll: nPolled=%d", nPolled );

		polledIdx = 1;
		if( polls[ 0 ].revents )
		{
			if( polls[ 0 ].revents & ( POLLERR | POLLNVAL ) )
				throw PSystemError( "pipe failed", "POLLERR" );
			++polledIdx;
			--nPolled;
			return waitQueue;
		}

		for( ; nPolled > 0 && polledIdx < maxActiveSockP1 ; ++polledIdx )
		{
			if ( sockFlags[polledIdx] & 0x01 )
			{
				int ret = _pollSlot( retSlot, polledIdx );
				if( ret != _noWait )
				{
					++polledIdx;
					--nPolled;
					return ret;
				}
			}
		}
	}//forever
}

#endif // epoll

void PMultiSocketRW::_slotFailed( int slotNum, const char* msg, const char* errMsg, int errCode )
{
	if( sockFlags[ slotNum ] & 0x20 )
		return;//already failed
	haveFailedSlot = true;
	sockFlags[ slotNum ] |= 0x20;
	slots[ slotNum ]._failedMsg = msg;
	slots[ slotNum ]._failedErrMsg = errMsg;
	slots[ slotNum ]._failedErrCode = errCode;
}

void PMultiSocketRW::_slotFailed( int slotNum, const char* msg, int errCode )
{
	if( sockFlags[ slotNum ] & 0x20 )
		return;//already failed
	haveFailedSlot = true;
	sockFlags[ slotNum ] |= 0x20;
	slots[ slotNum ]._failedMsg = msg;
	PSystemError::errorMessage( slots[ slotNum ]._failedErrMsg, errCode );
	slots[ slotNum ]._failedErrCode = errCode;
}

void PMultiSocketRW::forceSlotFailure( int slot, const char* errMsg, int errCode )
{
	_slotFailed( slot, "forceSlotFailure", errMsg, errCode );
}

size_t PMultiSocketRW::getReadSize( int slotNum ) const
{
	PASSERT4( ( sockFlags[ slotNum ] & 0x02 ) == 0 );
	return slots[ slotNum ].readBuf.offset;
}

size_t PMultiSocketRW::getWrittenSize( int slotNum ) const
{
	PASSERT4( ( sockFlags[ slotNum ] & 0x04 ) == 0 );
	return slots[ slotNum ].writeBuf.offset;
}

#endif
