#include "ppinclude.h"
#include "ppstring.h"
#include "pplogfile.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "_commcomm.h"
#include "commsharedmem.h"
#include "commsharedmemloop.h"

void CommSharedMemoryLoop::_sharedMemoryError(const char * msg)
{
	PString err;
	err.assign("CommSharedMemoryLoop object '").append(sharedResourceName.c_str()).
		append("' - '").append(msg).append("'");
	clearEverything();
	throw PError(err.c_str());
}


void CommSharedMemoryLoop::dump()
{
	PLock lc(cs);
	if (initObject)
	{
		PLog ("Dump=== CommSharedMemoryLoop '%s': queue.size=%d, controlFlag=%x, readFrame=%d, writeFrame=%d",
			sharedResourceName.c_str(),
			wrQueue.size(),
			sharedAddress->controlFlag,
			sharedAddress->readFrame,
			sharedAddress->writeFrame);
	}
	else
	{
		PLog("Dump=== Object not initialized");
	}
}

void CommSharedMemoryLoop::clearEverything()
{
	try
	{
		if (_logger)
		{
			_log("Close shared memory: received packets(bytes) %u(%" PRIu64 "), sent %u(%" PRIu64 ")", packetsIn,bytesIn,packetsOut,bytesOut);

		}
		initObject=false;
		packetsIn = 0;
		packetsOut = 0;
		bytesIn = 0;
		bytesOut = 0;
		_whenStored = 0;
		wrQueue.clear();
		readEvents[0].close();
		readEvents[1].close();
		writeEvents[0].close();
		writeEvents[1].close();
		sharedMemory.close();
		lockMutex[0].close();
		lockMutex[1].close();
#ifdef PWIN32
		if (peerProcess)
		{
			CloseHandle(peerProcess);
			peerProcess = NULL;
		}
#endif
		PTRACE3("clearEverything %s",sharedResourceName.c_str());
	}
	catch(PError & err)
	{
		PLog("CommSharedMemoryLoop::clearEverything error - '%s'. Ignored",err.why());
	}
}

UINT32 CommSharedMemoryLoop::_writeMsg(_CommMsg &msg)
{
	_Write& wr = wrBlocks[ msg.priority ];
	PASSERT3( wr.offset < 0 );
	wr.msg.moveFrom( msg );
	wr.offset = 0;
	wr.msg.internal.mux = msg.internal.mux;
	wr.msg.internal.muxChannelId = msg.internal.muxChannelId;
	return _writeMsg(msg.priority);
}

void CommSharedMemoryLoop::_writeToBuffer(INT32 currentReadPosition, INT32 & currentWritePosition, const BYTE * byteToWrite, size_t length)
{
	if (currentReadPosition >= SHARED_MEMORY_BLOCK || 
		currentWritePosition >= SHARED_MEMORY_BLOCK )
	{
		PTRACE3("Aseertion%d: readp=%d, writep=%d", readBlock,currentReadPosition, currentWritePosition);
		PASSERT3(0);
	}
	bytesOut+=length;
	BYTE * writeBuffer = (BYTE *) sharedAddress[ readBlock?0:1 ].buffer;
	if (currentReadPosition > currentWritePosition)
	{
		memcpy(writeBuffer + currentWritePosition, byteToWrite, length);
		currentWritePosition += length;
	}
	else
	{
		int lengthToWrite = SHARED_MEMORY_BLOCK - currentWritePosition;
		if (lengthToWrite >= length)
		{
			memcpy(writeBuffer + currentWritePosition, byteToWrite, length);
			currentWritePosition += length;
		}
		else 
		{
			memcpy(writeBuffer + currentWritePosition, byteToWrite, lengthToWrite);
			length -= lengthToWrite;
			memcpy(writeBuffer, byteToWrite + lengthToWrite, length);
			currentWritePosition = length;
		}
	}
	if (currentWritePosition == SHARED_MEMORY_BLOCK)
		currentWritePosition = 0;

}


UINT32 CommSharedMemoryLoop::_writeMsg(int priority)
{

	UINT32 returnCode = 0;
	int bufferAvailable;
	UINT16 bytesToWrite = 0;
	_Write& wr = wrBlocks[ priority ];
	PASSERT3( wr.offset >= 0 );	
	PASSERT3(initObject);
	int block = readBlock?0:1;

	//	lock(block);

	INT32 currentReadPosition = sharedAddress[block].readFrame;
	INT32 currentWritePosition = sharedAddress[block].writeFrame;
	if (currentReadPosition == currentWritePosition)
		bufferAvailable = SHARED_MEMORY_BLOCK - 1;
	else if (currentReadPosition > currentWritePosition)
		bufferAvailable = currentReadPosition - currentWritePosition - 1;
	else
		bufferAvailable = SHARED_MEMORY_BLOCK - currentWritePosition + currentReadPosition - 1;

	int messageSize = wr.msg.body._size() - wr.offset; 
	//	PTRACE5("write%d: rp=%d, wp=%d, availabe=%d, size=%d",readBlock,currentReadPosition, currentWritePosition,bufferAvailable,messageSize);   
	if (bufferAvailable > (SHARED_HEADER_SIZE + 8))
	{
		returnCode = eWasWritten;
		BYTE flag = wr.offset ? 0:0x80;
		if ( (bufferAvailable - SHARED_HEADER_SIZE) < messageSize)
		{
			bytesToWrite = bufferAvailable - SHARED_HEADER_SIZE;
			returnCode |= eBufferFull;
		}
		else
		{
			flag|=0x40;
			bytesToWrite = messageSize;
			if (bufferAvailable - bytesToWrite - SHARED_HEADER_SIZE  <=  SHARED_HEADER_SIZE + 8)
				returnCode |= eBufferFull;
		}
		_writeToBuffer(currentReadPosition, currentWritePosition, (BYTE *)&flag, sizeof(flag));
		BYTE pr = wr.msg.priority & 0x0f;
		_writeToBuffer(currentReadPosition, currentWritePosition, (BYTE *)&pr, sizeof(pr));
		_writeToBuffer(currentReadPosition, currentWritePosition, (BYTE *)&wr.msg.flags, sizeof(wr.msg.flags));
		_writeToBuffer(currentReadPosition, currentWritePosition, (BYTE *)&wr.msg.type, sizeof(wr.msg.type));
		if (muxEnabled)
		{
			INT16 mux = wr.msg.internal.mux;
			UINT32 muxChannelId = wr.msg.internal.muxChannelId; 
			_writeToBuffer(currentReadPosition, currentWritePosition, (BYTE *)&mux, sizeof(mux));
			_writeToBuffer(currentReadPosition, currentWritePosition, (BYTE *)&muxChannelId, sizeof(muxChannelId));
		}
		else
		{
			UINT32 zero = 0;
			_writeToBuffer(currentReadPosition, currentWritePosition, (BYTE *)&zero, sizeof(INT16));
			_writeToBuffer(currentReadPosition, currentWritePosition, (BYTE *)&zero, sizeof(UINT32));
		}
		_writeToBuffer(currentReadPosition, currentWritePosition, (BYTE *)&bytesToWrite, sizeof(bytesToWrite));
		if( wr.msg.internal.flags & _CommMsg::Patch8 && wr.offset==0 )
		{
			UINT32 p0;
			UINT32 p1;
			CommMsgBody::writeUINT32((BYTE *) &p0, wr.msg.internal.patch0 );
			CommMsgBody::writeUINT32((BYTE *) &p1, wr.msg.internal.patch1 );

			_writeToBuffer(currentReadPosition, currentWritePosition, (BYTE *)&p0, sizeof(p0));
			_writeToBuffer(currentReadPosition, currentWritePosition, (BYTE *)&p1, sizeof(p1));			
			_writeToBuffer(currentReadPosition, currentWritePosition, wr.msg.body._readPtr()+8, bytesToWrite-8);
		}
		else
		{
			_writeToBuffer(currentReadPosition, currentWritePosition, wr.msg.body._readPtr()+wr.offset, bytesToWrite);
		}
		//		PTRACE5("write%d: wrote =%d, new wp=%d",readBlock,bytesToWrite,currentWritePosition);
		if (flag& 0x40)
		{
			wr.offset = -1;
			wr.msg.body._dealloc();
			packetsOut++;
		}
		else
		{
			wr.offset += bytesToWrite;
		}
		if (returnCode & eBufferFull)
		{
			sharedAddress[block].writeFlag = 1;
			//			PTRACE5("write%d: write flag set to 1",readBlock);
		}
		sharedAddress[block].writeFrame = currentWritePosition;
		// recalculate available buffer
		currentReadPosition = sharedAddress[block].readFrame;
		if (currentReadPosition == currentWritePosition)
			bufferAvailable = SHARED_MEMORY_BLOCK - 1;
		else if (currentReadPosition > currentWritePosition)
			bufferAvailable = currentReadPosition - currentWritePosition - 1;
		else
			bufferAvailable = SHARED_MEMORY_BLOCK - currentWritePosition + currentReadPosition - 1;
		if (bufferAvailable <= (SHARED_HEADER_SIZE + 8))
		{
			returnCode |= eBufferFull;
			sharedAddress[block].writeFlag = 1;
			//			PTRACE5("after write%d: rp=%d, wp=%d, availabe=%d - buffer full",readBlock,currentReadPosition, currentWritePosition,bufferAvailable);   
		}
		else
		{
			returnCode &= ~eBufferFull;
			sharedAddress[block].writeFlag = 0;
			//			PTRACE5("after write%d: rp=%d, wp=%d, availabe=%d - have space",readBlock, currentReadPosition, currentWritePosition,bufferAvailable);   
		}
	}
	else
	{
		returnCode |= eBufferFull;
		sharedAddress[block].writeFlag = 1;
	}
	//	unlock(block);
	return returnCode;
}


CommSharedMemoryLoop::CommSharedMemoryLoop()
{
	//	compatible= compatible_;
	slot = 0;
	initObject=false;
	//	multiConnection=false;
	_whenStored = 0;
	_logger = 0;
	muxEnabled = false;
	sharedAddress=NULL;
	lastTimeInQueue=0;
	packetsIn = 0;
	packetsOut = 0;
	bytesIn = 0;
	bytesOut = 0;

#ifdef PWIN32
	peerProcess = NULL;
#endif
	for( int i=0; i < _COMM_MSGPRIORITIES ; ++i )
	{
		wrBlocks[ i ].offset = -1;
		wrBlocks[ i ].msg.body._dealloc();
		rdMsgs[i].body._dealloc();
	}

}


void CommSharedMemoryLoop::forceDisconnectSharedMemory()
{
	PLock lc(cs);

	if (initObject == false)
	{
		return;
	}

	PTRACE3("forceDisconnectSharedMem %s - after lock(): readBlock = %d, sharedAddres=%p",sharedResourceName.c_str(),readBlock,sharedAddress);
	if (sharedAddress)
	{
		lock(0);
		lock(1);
		sharedAddress[0].controlFlag=0;
		sharedAddress[1].controlFlag=0;
		unlock(1);
		unlock(0);
		readEvents[0].set();
		readEvents[1].set();
	}
	clearEverything();
}

void CommSharedMemoryLoop::Create(const char * sharedResourceName_)
{
	try
	{
		PLock lc(cs);
		readBlock = 0;

		PString tmpSharedResourceName;
		sharedResourceName.assign(SHARED_GLOBAL).append(sharedResourceName_);
		PTRACE3("Shared memoroy create:  %s",sharedResourceName.c_str());
		if (_logger)
		{
			_log("Create shared memory");
		}
		tmpSharedResourceName.assign(sharedResourceName);
		tmpSharedResourceName.append("0readEventL");
		readEvents[0].create(false,tmpSharedResourceName.c_str());

		tmpSharedResourceName.assign(sharedResourceName);
		tmpSharedResourceName.append("1readEventL");
		readEvents[1].create(true,tmpSharedResourceName.c_str());

		tmpSharedResourceName.assign(sharedResourceName);
		tmpSharedResourceName.append("0writeEventL");
		writeEvents[0].create(false,tmpSharedResourceName.c_str());

		tmpSharedResourceName.assign(sharedResourceName);
		tmpSharedResourceName.append("1writeEventL");
		writeEvents[1].create(true,tmpSharedResourceName.c_str());
		tmpSharedResourceName.assign(sharedResourceName);
		tmpSharedResourceName.append("0lockMutexL");
		lockMutex[0].create(tmpSharedResourceName.c_str());

		tmpSharedResourceName.assign(sharedResourceName);
		tmpSharedResourceName.append("1lockMutexL");
		lockMutex[1].create(tmpSharedResourceName.c_str());
		tmpSharedResourceName.assign(sharedResourceName);
		tmpSharedResourceName.append("sharedMemoryL");

		sharedAddress= (SharedAddress *) sharedMemory.create(sizeof(SharedAddress)*2,tmpSharedResourceName.c_str());
		for( int i=0; i < _COMM_MSGPRIORITIES ; ++i )
		{
			wrBlocks[ i ].offset = -1;
			wrBlocks[ i ].msg.body._dealloc();
			rdMsgs[i].body._dealloc();
		}
		initObject=true;
		lock(0);
		lock(1);
		sharedAddress[0].controlFlag = SHARED_CHANNEL_OPEN;
		sharedAddress[0].readFrame = 0;
		sharedAddress[0].writeFrame = 0;
		sharedAddress[0].writeFlag = 0;
		sharedAddress[1].controlFlag = SHARED_CHANNEL_OPEN;
		sharedAddress[1].readFrame = 0;
		sharedAddress[1].writeFrame = 0;
		sharedAddress[1].writeFlag = 0;
		unlock(1);
		unlock(0);
	}
	catch(PError &err)
	{
		_sharedMemoryError(err.why());
	}
}


void CommSharedMemoryLoop::Open(const char * sharedResourceName_)
{
	try
	{
		readBlock = 1;
		PLock lc(cs);
		PString tmpSharedResourceName;
		sharedResourceName.assign(SHARED_GLOBAL).append(sharedResourceName_);
		PTRACE3("Shared memory opening %s:",sharedResourceName.c_str());
		if (_logger)
		{
			_log("Open shared memory");
		}

		tmpSharedResourceName.assign(sharedResourceName);
		tmpSharedResourceName.append("0readEventL");
		readEvents[0].open(true,tmpSharedResourceName.c_str());

		tmpSharedResourceName.assign(sharedResourceName);
		tmpSharedResourceName.append("1readEventL");
		readEvents[1].open(false,tmpSharedResourceName.c_str());

		tmpSharedResourceName.assign(sharedResourceName);
		tmpSharedResourceName.append("0writeEventL");
		writeEvents[0].open(true,tmpSharedResourceName.c_str());

		tmpSharedResourceName.assign(sharedResourceName);
		tmpSharedResourceName.append("1writeEventL");

		writeEvents[1].open(false,tmpSharedResourceName.c_str());
		tmpSharedResourceName.assign(sharedResourceName);
		tmpSharedResourceName.append("0lockMutexL");
		lockMutex[0].open(tmpSharedResourceName.c_str());

		tmpSharedResourceName.assign(sharedResourceName);
		tmpSharedResourceName.append("1lockMutexL");
		lockMutex[1].open(tmpSharedResourceName.c_str());
		tmpSharedResourceName.assign(sharedResourceName);
		tmpSharedResourceName.append("sharedMemoryL");

		sharedAddress= (SharedAddress *) sharedMemory.open(tmpSharedResourceName.c_str());

		for( int i=0; i < _COMM_MSGPRIORITIES ; ++i )
		{
			wrBlocks[ i ].offset = -1;
			wrBlocks[ i ].msg.body._dealloc();
			rdMsgs[i].body._dealloc();
		}
#ifdef PWIN32
		PString processId = initSharedMemProcessId(sharedResourceName);
		if (processId.length())
		{
			UINT32 prId = strtoul(processId.c_str(),0,16);
			peerProcess = OpenProcess(SYNCHRONIZE,FALSE,prId);
			if (peerProcess == NULL)
			{
				PTRACE3("Shared memory Open: 'OpenProcess()' error: %d, processId = %d",::GetLastError(), prId);
			}
		}
		else
		{
			peerProcess = NULL;
		}
#endif
		initObject=true;
	}
	catch(PError &err)
	{
		_sharedMemoryError(err.why());
	}

}

void CommSharedMemoryLoop::pushMessage(_CommMsg & msg)
{

	PLock lc(cs);
	if (initObject == false)
	{
		PTRACE3("pushMessage - initObject=false");
		return;
	}
	wrQueue.push( msg );
	_pushData();
}

void CommSharedMemoryLoop::closeSharedMemory()
{
	PLock lc(cs);
	PTRACE3("closeSharedMemory %s",sharedResourceName.c_str());
	if (initObject == false)
	{
		PTRACE3("closeSharedMemory - initObject=false");
		return;
	}
	if (sharedAddress)
	{
		lock(0);
		lock(1);
		sharedAddress[0].controlFlag=0;
		sharedAddress[1].controlFlag=0;
		unlock(1);
		unlock(0);
		readEvents[0].set();
		readEvents[1].set();
	}
	clearEverything();
}

void CommSharedMemoryLoop::peerReadData()
{
	PLock lc(cs);

	if (initObject == false)
	{
		PTRACE3("peerReadData - initObject=false");
		return;
	}
	//	PTRACE5("peerReadData%d ", readBlock);
	_pushData();
}

void CommSharedMemoryLoop::_pushData()
{
	int wrPr;
	bool flagWasWrite = false;
	bool nothingToSend = true;
	//	bool bufferFull = false;
	for(wrPr=_COMM_MSGPRIORITIES-1; wrPr >= 0 ; --wrPr )
	{

		UINT32 ret=0;
		while( (wrQueue.size()>0 && wrPr == wrQueue.peek().priority ) || wrBlocks[wrPr].offset >= 0)
		{
			nothingToSend = false;
			if (wrBlocks[wrPr].offset >= 0)
			{
				ret = _writeMsg( wrPr );
			}
			else
			{
				_CommMsg& msg = wrQueue.peek();
				ret = _writeMsg( msg );
				wrQueue.pop(lastTimeInQueue);
			}
			flagWasWrite = flagWasWrite || (ret & eWasWritten) ;
			if( ret & eBufferFull)
			{
				//				bufferFull =true;
				break;
			}
		}
		if ( ret & eBufferFull )
		{
			//			bufferFull = true;
			break;
		}
	}
	if (flagWasWrite )
	{
		//		PTRACE5("pushData%d - read event set, buffer %s", readBlock, (bufferFull ? "full":"not full"));
		readEvents[readBlock].set();
	}
	else
	{
		if (nothingToSend)
		{
			//			PTRACE5("pushData%d - nothing to send, clear writeFlag", readBlock);
			sharedAddress[readBlock?0:1].writeFlag = 0;
		}

	}
}



void CommSharedMemoryLoop::_readFromBuffer(INT32 & currentReadPosition, INT32 currentWritePosition, BYTE * byteToRead, size_t length)
{
	if (currentReadPosition >= SHARED_MEMORY_BLOCK || 
		currentWritePosition >= SHARED_MEMORY_BLOCK )
	{
		PTRACE3("Assertion%d: readp=%d, writep=%d", readBlock,currentReadPosition, currentWritePosition);
		PASSERT3(0);
	}
	if (currentWritePosition == currentReadPosition)
	{
		PTRACE3("Assertion%d: readp=writep=%d", readBlock,currentReadPosition);
		PASSERT3(0);
	}

	bytesIn+=length;

	BYTE * readBuffer = (BYTE *) sharedAddress[ readBlock ].buffer;
	if (currentWritePosition > currentReadPosition )
	{
		memcpy(byteToRead, readBuffer + currentReadPosition, length);
		currentReadPosition += length;
	}
	else
	{
		int lengthToRead = SHARED_MEMORY_BLOCK - currentReadPosition;
		if (lengthToRead >= length)
		{
			memcpy(byteToRead, readBuffer + currentReadPosition, length);
			currentReadPosition += length;
		}
		else 
		{
			memcpy(byteToRead, readBuffer + currentReadPosition, lengthToRead);
			length -= lengthToRead;
			memcpy(byteToRead + lengthToRead, readBuffer, length);
			currentReadPosition = length;
		}
	}
	if (currentReadPosition == SHARED_MEMORY_BLOCK)
		currentReadPosition = 0;

}

void CommSharedMemoryLoop::_shiftReadBuffer(INT32 & currentReadPosition, INT32 currentWritePosition,  size_t length)
{
	if (currentReadPosition >= SHARED_MEMORY_BLOCK || 
		currentWritePosition >= SHARED_MEMORY_BLOCK )
	{
		PTRACE3("Assertion%d: readp=%d, writep=%d", readBlock,currentReadPosition, currentWritePosition);
		PASSERT3(0);
	}
	if (currentWritePosition == currentReadPosition)
	{
		PTRACE3("Assertion%d: readp=writep=%d", readBlock,currentReadPosition);
		PASSERT3(0);
	}
	if (currentWritePosition > currentReadPosition )
	{
		currentReadPosition += length;
	}
	else
	{
		int lengthToRead = SHARED_MEMORY_BLOCK - currentReadPosition;
		if (lengthToRead >= length)
		{
			currentReadPosition += length;
		}
		else 
		{
			length -= lengthToRead;
			currentReadPosition = length;
		}
	}
	if (currentReadPosition == SHARED_MEMORY_BLOCK)
		currentReadPosition = 0;

}



int CommSharedMemoryLoop::getMessage(_CommMsg & message)
{
	PLock lc(cs);
	if (initObject == false)
	{
		PTRACE3("getMessage - initObject=false");
		return -1;
	}

	if (!(sharedAddress[readBlock].controlFlag & SHARED_CHANNEL_OPEN))
	{
		return -1;
	}

	while(1)
	{
		int  bufferAvailable;
		INT32 currentReadPosition = sharedAddress[readBlock].readFrame;
		INT32 currentWritePosition = sharedAddress[readBlock].writeFrame;
		if (currentWritePosition == currentReadPosition )
			bufferAvailable = 0;
		else if (currentWritePosition > currentReadPosition )
			bufferAvailable = currentWritePosition - currentReadPosition;
		else
			bufferAvailable = SHARED_MEMORY_BLOCK - currentReadPosition + currentWritePosition;
		//		PTRACE5("read%d: rp=%d, wp=%d, availabe=%d",readBlock,currentReadPosition, currentWritePosition,bufferAvailable);
		if (bufferAvailable > 0)
		{
			if (bufferAvailable < SHARED_HEADER_SIZE)
			{
				PTRACE3("Assertion%d :  rp=%d, wp=%d, availabe=%d",readBlock,currentReadPosition, currentWritePosition,bufferAvailable);
				PASSERT3(0);
			}
		}
		else
		{
			//			PTRACE5("GetMessage%d : no available buffer write flag %s",readBlock,(sharedAddress[readBlock].writeFlag?"set":"not set"));
			if (sharedAddress[readBlock].writeFlag)
			{
				writeEvents[readBlock].set();
			}
			return 0;
		}
		BYTE b; 
		_readFromBuffer(currentReadPosition,currentWritePosition, (BYTE*) &b, sizeof(b));
		bool isFirst = ( b & 0x80 ) != 0;
		bool isLast =  ( b & 0x40 ) != 0;

		BYTE priority; 
		_readFromBuffer(currentReadPosition,currentWritePosition, (BYTE*) &priority, sizeof(priority));	
		priority &= 0x3F;
		PASSERT3( priority < _COMM_MSGPRIORITIES );
		_CommMsg& msg = rdMsgs[ priority ];

		UINT16 size;
		if( isFirst )
		{
			UINT16 flags;
			_readFromBuffer(currentReadPosition,currentWritePosition, (BYTE*) &flags, sizeof(flags));	
			BYTE type;
			_readFromBuffer(currentReadPosition,currentWritePosition, (BYTE*) &type, sizeof(type));	
			msg.priority = priority;
			msg.flags = flags;
			msg.type = type;
			if (muxEnabled)
			{
				_readFromBuffer(currentReadPosition,currentWritePosition, (BYTE*) &msg.internal.mux, sizeof(msg.internal.mux));	
				_readFromBuffer(currentReadPosition,currentWritePosition, (BYTE*) &msg.internal.muxChannelId, sizeof(msg.internal.muxChannelId));	
			}
			else
			{
				msg.internal.mux = 0;
				msg.internal.muxChannelId = 0;
				_shiftReadBuffer(currentReadPosition,currentWritePosition,6);
			}

			_readFromBuffer(currentReadPosition,currentWritePosition, (BYTE*) &size, sizeof(size));	
			if ( (size + SHARED_HEADER_SIZE ) > bufferAvailable)
			{
				PTRACE3("Assertion%d: size = %d, buffer=%d",readBlock,size,bufferAvailable);
				PASSERT3(0);
			}
			if (size > 0)
				_readFromBuffer(currentReadPosition,currentWritePosition, msg.body._append( size ), size);	
		}
		else
		{
			_shiftReadBuffer(currentReadPosition,currentWritePosition,(SHARED_HEADER_SIZE - 2 - 2));
			_readFromBuffer(currentReadPosition,currentWritePosition, (BYTE*) &size, sizeof(size));	
			if ( (size + SHARED_HEADER_SIZE ) > bufferAvailable)
			{
				PTRACE3("Assertion%d: size = %d, buffer=%d",readBlock,size,bufferAvailable);
				PASSERT3(0);
			}
			if (size > 0)
				_readFromBuffer(currentReadPosition,currentWritePosition, msg.body._append( size ), size);	
		}
		//		PTRACE5("read%d: size=%d, new rp=%d", readBlock, size, currentReadPosition);
		sharedAddress[readBlock].readFrame = currentReadPosition;
		if( isLast )
		{
			packetsIn++;
			message.moveFrom(msg);
			msg.body._dealloc();
			if (sharedAddress[readBlock].writeFlag)
			{
				//				PTRACE5("read%d: set readyToWrite event",readBlock);
				writeEvents[readBlock].set();
			}
			return 1;
		}
	}
}

