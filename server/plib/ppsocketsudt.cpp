#include "ppinclude.h"
#ifdef UDT_DEVELOPMENT
#include "ppsocketudt.h"


PMultiSocketRWUDT::PMultiSocketRWUDT( SOCKET queue, int numberOfConnections_) 
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
	pudtContainer = 0;

	haveFailedSlot = false;
	nWaitHandles = -1;
	waitExpectedOp = 0;
	startSocket=0;
}

PMultiSocketRWUDT::~PMultiSocketRWUDT()
{
	delete[] sockFlags;
	delete[] slots;
	delete[] waitHandles;
	delete[] waitHandlesMap;
}


int PMultiSocketRWUDT::addSocket( SOCKET sock )
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
		
		HANDLE ev = pudtgetsockreadevent((PUDTSOCKET)( sock));
		if( ev == NULL )
			throw PError( "Unable to UDT::getsockreadevent");
		slot.readEvent = ev;
		ev = pudtgetsockwriteevent((PUDTSOCKET)( sock));
		if( ev == NULL )
		{ 
			throw PError( "Unable to UDT::getsockwriteevent");
		}
		slot.writeEvent = ev;
	}
	slot.sock = (HANDLE)sock;
	sockFlags[ slotNum ] = 0x01;

	nWaitHandles = -1;
	waitExpectedOp = 0;
	return slotNum;
}

void PMultiSocketRWUDT::removeSocket( int slotNum )
{
	PASSERT4( slotNum > 0 && slotNum < maxActiveSockP1 );
	PASSERT4( sockFlags[ slotNum ] & 0x01 );

	_Slot& slot = slots[ slotNum ];
	pudtclose( (PUDTSOCKET)slot.sock );
	slot.writeEvent = 0;
	slot.readEvent = 0;

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

void PMultiSocketRWUDT::shutdownSockets()
{
	for( int i=0; i < maxActiveSockP1 ; ++i )
		if( sockFlags[ i ] & 0x01 )
		{
			_Slot& slot = slots[ i ];
			if( i != 0 )
			{
				pudtclose( (PUDTSOCKET)slot.sock );
			}
			sockFlags[ i ] = 0x00;
		}
	maxActiveSockP1 = 0;
	nWaitHandles = -1;
	waitExpectedOp = 0;
}




size_t PMultiSocketRWUDT::read( int slotNum, BYTE* buf, size_t bufSz, bool incomplete )
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
	int ret; 
	while( (ret=pudtrecvnonblocking(  (PUDTSOCKET) slot.sock, (char*)_buf.buf+_buf.offset, _buf.sz-_buf.offset)) > 0)
	{
		_buf.offset += ret;

		if ( incomplete || _buf.offset == _buf.sz)
		{
			sockFlags[ slotNum ] &= ~0x02;
			waitExpectedOp = slotNum;
			return _buf.offset;
		}
	}
	if (ret < 0)
	{
		_slotFailed( slotNum, "recvnonblocking", ret );
		return 0;
	}
	return 0;

}

size_t PMultiSocketRWUDT::write( int slotNum, const BYTE* buf, size_t bufSz, bool incomplete )
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
	int ret;
	while( (ret=pudtsendnonblocking(  (PUDTSOCKET)slot.sock, (char*)_buf.buf + _buf.offset, _buf.sz-_buf.offset)) > 0)
	{
		_buf.offset += ret;
		if ( incomplete || _buf.offset == _buf.sz)
		{
			sockFlags[ slotNum ] &= ~ 0x04;
			waitExpectedOp = -slotNum;
			return _buf.offset;
		}
	}
	if (ret<0)
	{
		_slotFailed( slotNum, "WriteFile", ret );
		return 0;
	}
	return 0;
}

int PMultiSocketRWUDT::wait( int& retSlot, int timeout )
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
					waitHandles[ nWaitHandles ] = slots[ i ].writeEvent;
					waitHandlesMap[ nWaitHandles++ ] = -i;
				}
				if( flags & 0x02 )
				{
					waitHandles[ nWaitHandles ] = slots[ i ].readEvent;
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
			_Buf& _buf = slot.writeBuf;
			int ret;
			ret=pudtsendnonblocking(  (PUDTSOCKET)slot.sock, (char*)_buf.buf + _buf.offset, _buf.sz-_buf.offset);
			if (ret < 0)
			{
				_slotFailed( slotNum, "sendnonblocking", ret);
				retSlot = slotNum;
				return waitFailed;
			}
			else if (ret > 0)
			{
				_buf.offset += ret;
				PASSERT5( _buf.offset <= _buf.sz );
				if( ( sockFlags[ slotNum ] & 0x10 ) || _buf.offset == _buf.sz )
				{
					sockFlags[ slotNum ] &= ~0x04;
					waitExpectedOp = -slotNum;
					retSlot = slotNum;
					return waitWrite;
				}
			}
		}
		else//read
		{
			PASSERT5( slotNum < maxActiveSockP1 );
			_Slot& slot = slots[ slotNum ];
			_Buf& _buf = slot.readBuf;
			int ret;
			ret=pudtrecvnonblocking(  (PUDTSOCKET)slot.sock, (char*)_buf.buf + _buf.offset, _buf.sz-_buf.offset);
			if (ret < 0)
			{
				_slotFailed( slotNum, "recvnonblocking", ret);
				retSlot = slotNum;
				return waitFailed;
			}
			else if (ret > 0)
			{
				_buf.offset += ret;
				PASSERT5( _buf.offset <= _buf.sz );
				if( ( sockFlags[ slotNum ] & 0x08 ) || _buf.offset == _buf.sz )
				{
					sockFlags[ slotNum ] &= ~0x02;
					waitExpectedOp = -slotNum;
					retSlot = slotNum;
					return waitRead;
				}
			}
		}//read
	}//forever
}

void PMultiSocketRWUDT::_slotFailed( int slotNum, const char* msg, const char* errMsg, int errCode )
{
	if( sockFlags[ slotNum ] & 0x20 )
		return;//already failed
	haveFailedSlot = true;
	sockFlags[ slotNum ] |= 0x20;
	slots[ slotNum ]._failedMsg = msg;
	slots[ slotNum ]._failedErrMsg = errMsg;
	slots[ slotNum ]._failedErrCode = errCode;
}

void PMultiSocketRWUDT::_slotFailed( int slotNum, const char* msg, int errCode )
{
	if( sockFlags[ slotNum ] & 0x20 )
		return;//already failed
	haveFailedSlot = true;
	sockFlags[ slotNum ] |= 0x20;
	slots[ slotNum ]._failedMsg = msg;
	PSystemError::errorMessage( slots[ slotNum ]._failedErrMsg, errCode );
	slots[ slotNum ]._failedErrCode = errCode;
}

void PMultiSocketRWUDT::forceSlotFailure( int slot, const char* errMsg, int errCode )
{
	_slotFailed( slot, "forceSlotFailure", errMsg, errCode );
}

size_t PMultiSocketRWUDT::getReadSize( int slotNum ) const
{
	PASSERT4( ( sockFlags[ slotNum ] & 0x02 ) == 0 );
	return slots[ slotNum ].readBuf.offset;
}

size_t PMultiSocketRWUDT::getWrittenSize( int slotNum ) const
{
	PASSERT4( ( sockFlags[ slotNum ] & 0x04 ) == 0 );
	return slots[ slotNum ].writeBuf.offset;
}

//****************************************************************************

#endif