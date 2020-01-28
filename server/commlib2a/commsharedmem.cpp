#include "ppinclude.h"
#include "ppstring.h"
#include "pplogfile.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "_commcomm.h"
#include "commsharedmem.h"

PString initSharedMemProcessId(const PString& sharedResourceName)
{
	static const size_t sharedGlobalLen = strlen(SHARED_GLOBAL);
	static const char* pyrlPrefix = "pyrl_";
	static const size_t pyrlPrefixLen = strlen(pyrlPrefix);
	static const char* pyrPrefix = "pyr_";
	static const size_t pyrPrefixLen = strlen(pyrPrefix);

	PString processId, tmpSharedResourceName;
	tmpSharedResourceName.assign(sharedResourceName);
	if (PString::startsWith(tmpSharedResourceName.c_str() + sharedGlobalLen, pyrlPrefix))
	{
		processId.assign(tmpSharedResourceName.c_str() + sharedGlobalLen + pyrlPrefixLen, 8);
	}
	else if (PString::startsWith(tmpSharedResourceName.c_str() + sharedGlobalLen, pyrPrefix))
	{
		processId.assign(tmpSharedResourceName.c_str() + sharedGlobalLen + pyrPrefixLen, 8);
	}
	return processId;
}

void CommSharedMemory::_sharedMemoryError(const char * msg)
{
	PString err;
	err.assign("CommSharedMemory object '").append(sharedResourceName.c_str()).
		append("' - '").append(msg).append("'");
	clearEverything();
	throw PError(err.c_str());
}


void CommSharedMemory::dump()
{
	PLock lc(cs);
	if (initObject)
	{
		PLog ("Dump=== CommSharedMemory '%s': queue.size=%d, controlFlag=%x, beginFrame=%d, endFrame=%d",
			sharedResourceName.c_str(),
			wrQueue.size(),
			sharedAddress->controlFlag,
			sharedAddress->beginFrame,
			sharedAddress->endFrame);
	}
	else
	{
		PLog("Dump=== Object not initialized");
	}
}

void CommSharedMemory::clearEverything()
{
	try
	{
		initObject=false;
		readEvents[0].close();
		readEvents[1].close();
		writeEvents[0].close();
		writeEvents[1].close();
		sharedMemory.close();
		lockMutex[0].close();
		if (!compatible)
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
		PLog("CommSharedMemory::clearEverything error - '%s'. Ignored",err.why());
	}
}

bool CommSharedMemory::_writeMsg(_CommMsg &msg, bool & setWrite)
{
	_Write& wr = wrBlocks[ msg.priority ];
	PASSERT3( wr.offset < 0 );
	wr.msg.moveFrom( msg );
	wr.offset = 0;
	wr.msg.internal.mux = msg.internal.mux;
	wr.msg.internal.muxChannelId = msg.internal.muxChannelId;
	return _writeMsg(msg.priority, setWrite);
}

bool CommSharedMemory::_writeMsg(int priority, bool & setWrite)
{

	int bufferAvailable;
	UINT16 bytesToWrite = 0;
	_Write& wr = wrBlocks[ priority ];
	PASSERT3( wr.offset >= 0 );	
	PASSERT3(initObject);
	int block = readBlock?0:1;

	lock(block);
	if ((bufferAvailable = SHARED_MEMORY_BLOCK - sharedAddress[block].endFrame) > (SHARED_HEADER_SIZE + 8))
	{
		setWrite = sharedAddress[readBlock?0:1].endFrame == 0;
		//		setWrite = true;
		BYTE * writeBuffer = (BYTE *) (sharedAddress[block].buffer + sharedAddress[block].endFrame); 
		BYTE flag = wr.offset ? 0:0x80;
		if ( (bufferAvailable - SHARED_HEADER_SIZE) < (wr.msg.body._size() - wr.offset))
		{
			bytesToWrite = bufferAvailable - SHARED_HEADER_SIZE;
			sharedAddress[block].controlFlag |= SHARED_CHANNEL_WAIT_TO_WRITE;
		}
		else
		{
			flag|=0x40;
			bytesToWrite = (UINT16)(wr.msg.body._size() - wr.offset);
		}
		*writeBuffer++ = flag;
		*writeBuffer++ = wr.msg.priority & 0x0f;
		CommMsgBody::writeUINT16( writeBuffer, wr.msg.flags );
		writeBuffer += 2;
		*writeBuffer++ = wr.msg.type;
		if (muxEnabled)
		{
			int mux = wr.msg.internal.mux;
			UINT32 muxChannelId = wr.msg.internal.muxChannelId; 
			CommMsgBody::writeUINT16( writeBuffer, mux );
			writeBuffer += 2;
			CommMsgBody::writeUINT32( writeBuffer, muxChannelId );
			writeBuffer += 4;
		}
		else
		{
			CommMsgBody::writeUINT16( writeBuffer, 0 );
			writeBuffer += 2;
			CommMsgBody::writeUINT32( writeBuffer, 0 );
			writeBuffer += 4;
		}
		CommMsgBody::writeUINT16(writeBuffer,bytesToWrite);
		writeBuffer+=2;
		if( wr.msg.internal.flags & _CommMsg::Patch8 && wr.offset==0 )
		{
			CommMsgBody::writeUINT32( writeBuffer, wr.msg.internal.patch0 );
			writeBuffer += 4;
			CommMsgBody::writeUINT32( writeBuffer, wr.msg.internal.patch1 );
			writeBuffer += 4;
			memcpy(writeBuffer,wr.msg.body._readPtr()+8,bytesToWrite-8);
		}
		else
		{
			memcpy(writeBuffer,wr.msg.body._readPtr()+wr.offset,bytesToWrite);
		}

		sharedAddress[block].endFrame+= (bytesToWrite + SHARED_HEADER_SIZE);
		if (flag& 0x40)
		{
			wr.offset = -1;
			wr.msg.body._dealloc();
		}
		else
		{
			wr.offset += bytesToWrite;
		}

		unlock(block);
		return (flag & 0x40) ? true : false;
	}
	else
	{
		setWrite = false;
		sharedAddress[block].controlFlag |= SHARED_CHANNEL_WAIT_TO_WRITE;
	}

	unlock(block);
	return false;
}

bool CommSharedMemory::_writeConnectMessage(const char * connectionMessage)
{
	int bufferAvailable;
	PASSERT3(initObject);

	lock(0);
	if (! (sharedAddress[0].controlFlag & SHARED_CHANNEL_OPEN))
	{
		unlock(0);
		return false;
	}
	if ((bufferAvailable = SHARED_MEMORY_BLOCK - sharedAddress[0].endFrame) > strlen(connectionMessage))
	{
		BYTE * writeBuffer = (BYTE *) (sharedAddress[0].buffer + sharedAddress[0].endFrame); 
		strcpy((char *)writeBuffer,(char *)connectionMessage);
		sharedAddress[0].endFrame+= strlen(connectionMessage)+1;
		unlock(0);
		readEvents[0].set();
		return true;
	}
	unlock(0);
	return false;
}

CommSharedMemory::CommSharedMemory(bool compatible_)
{
	compatible= compatible_;
	initObject=false;
	multiConnection=false;
	muxEnabled = false;
	sharedAddress=NULL;
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


void CommSharedMemory::forceDisconnectSharedMemory()
{
	PLock lc(cs);

	if (initObject == false)
	{
		return;
	}
	PTRACE3("forceDisconnectSharedMem %s - after lock(): readBlock = %d, multiConnection=%s, sharedAddres=%p",sharedResourceName.c_str(),readBlock,(multiConnection?"true":"false"),sharedAddress);
	if (sharedAddress)
	{
		if (multiConnection == false)
		{
			lock(0);
			if (!compatible)
				lock(1);
			sharedAddress[0].controlFlag=0;
			sharedAddress[1].controlFlag=0;
			if (!compatible)
				unlock(1);
			unlock(0);
			readEvents[0].set();
			readEvents[1].set();
		}
		else if (readBlock==0)
		{
			lock(0);
			sharedAddress[0].controlFlag=0;
			unlock(0);
			readEvents[0].set();
		}

	}
	clearEverything();
}

void CommSharedMemory::Create(const char * sharedResourceName_)
{
	try
	{
		PLock lc(cs);
		readBlock = 0;

		PString tmpSharedResourceName;
		sharedResourceName.assign(SHARED_GLOBAL).append(sharedResourceName_);

		PTRACE3("Shared memoroy create:  %s",sharedResourceName.c_str());
		tmpSharedResourceName.assign(sharedResourceName);
		tmpSharedResourceName.append("0readEvent");
		readEvents[0].create(false,tmpSharedResourceName.c_str());

		tmpSharedResourceName.assign(sharedResourceName);
		tmpSharedResourceName.append("1readEvent");
		readEvents[1].create(true,tmpSharedResourceName.c_str());

		tmpSharedResourceName.assign(sharedResourceName);
		tmpSharedResourceName.append("0writeEvent");
		writeEvents[0].create(false,tmpSharedResourceName.c_str());

		tmpSharedResourceName.assign(sharedResourceName);
		tmpSharedResourceName.append("1writeEvent");
		writeEvents[1].create(true,tmpSharedResourceName.c_str());

		if (!compatible)
		{
			tmpSharedResourceName.assign(sharedResourceName);
			tmpSharedResourceName.append("0lockMutex");
			lockMutex[0].create(tmpSharedResourceName.c_str());

			tmpSharedResourceName.assign(sharedResourceName);
			tmpSharedResourceName.append("1lockMutex");
			lockMutex[1].create(tmpSharedResourceName.c_str());
		}
		else
		{
			tmpSharedResourceName.assign(sharedResourceName);
			tmpSharedResourceName.append("lockMutex");
			lockMutex[0].create(tmpSharedResourceName.c_str());
		}

		tmpSharedResourceName.assign(sharedResourceName);
		tmpSharedResourceName.append("sharedMemory");
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
		sharedAddress[0].beginFrame = 0;
		sharedAddress[0].endFrame = 0;
		sharedAddress[1].controlFlag = SHARED_CHANNEL_OPEN;
		sharedAddress[1].beginFrame = 0;
		sharedAddress[1].endFrame = 0;
		unlock(1);
		unlock(0);
	}
	catch(PError &err)
	{
		_sharedMemoryError(err.why());
	}
}

void CommSharedMemory::Accept(const char * sharedResourceName_)
{
	try
	{
		readBlock=0;
		PLock lc(cs);
		PString tmpSharedResourceName;
		sharedResourceName.assign(SHARED_GLOBAL).append(sharedResourceName_);
		PTRACE3("Shared memory create accept %s:",sharedResourceName.c_str());
		tmpSharedResourceName.assign(sharedResourceName);
		tmpSharedResourceName.append("0readEvent");
		readEvents[0].create(true,tmpSharedResourceName.c_str());

		tmpSharedResourceName.assign(sharedResourceName);
		tmpSharedResourceName.append("lockMutex");
		lockMutex[0].create(tmpSharedResourceName.c_str());

		tmpSharedResourceName.assign(sharedResourceName);
		tmpSharedResourceName.append("sharedMemory");
		sharedAddress= (SharedAddress *) sharedMemory.create(sizeof(SharedAddress),tmpSharedResourceName.c_str());
		multiConnection = true;
		initObject=true;
		lock(0);
		sharedAddress[0].controlFlag = SHARED_CHANNEL_OPEN;
		sharedAddress[0].beginFrame = 0;
		sharedAddress[0].endFrame = 0;
		unlock(0);
	}
	catch(PError &err)
	{
		_sharedMemoryError(err.why());
	}


}

void CommSharedMemory::Open(const char * sharedResourceName_)
{
	try
	{
		readBlock = 1;
		PLock lc(cs);
		PString tmpSharedResourceName;
		sharedResourceName.assign(SHARED_GLOBAL).append(sharedResourceName_);
		PTRACE3("Shared memory opening %s:",sharedResourceName.c_str());

		tmpSharedResourceName.assign(sharedResourceName);
		tmpSharedResourceName.append("0readEvent");
		readEvents[0].open(true,tmpSharedResourceName.c_str());

		tmpSharedResourceName.assign(sharedResourceName);
		tmpSharedResourceName.append("1readEvent");
		readEvents[1].open(false,tmpSharedResourceName.c_str());

		tmpSharedResourceName.assign(sharedResourceName);
		tmpSharedResourceName.append("0writeEvent");
		writeEvents[0].open(true,tmpSharedResourceName.c_str());

		tmpSharedResourceName.assign(sharedResourceName);
		tmpSharedResourceName.append("1writeEvent");

		writeEvents[1].open(false,tmpSharedResourceName.c_str());

		if (!compatible)
		{
			tmpSharedResourceName.assign(sharedResourceName);
			tmpSharedResourceName.append("0lockMutex");
			lockMutex[0].open(tmpSharedResourceName.c_str());

			tmpSharedResourceName.assign(sharedResourceName);
			tmpSharedResourceName.append("1lockMutex");
			lockMutex[1].open(tmpSharedResourceName.c_str());
		}
		else
		{
			tmpSharedResourceName.assign(sharedResourceName);
			tmpSharedResourceName.append("lockMutex");
			lockMutex[0].open(tmpSharedResourceName.c_str());
		}

		tmpSharedResourceName.assign(sharedResourceName);
		tmpSharedResourceName.append("sharedMemory");

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

bool CommSharedMemory::Connect(const char * sharedResourceName_, const char * newConnection)
{
	try
	{
		readBlock = 1;
		PLock lc(cs);
		PString tmpSharedResourceName;
		sharedResourceName.assign(SHARED_GLOBAL).append(sharedResourceName_);
		PTRACE3("Shared memory %s connecting to: %s",newConnection,sharedResourceName.c_str());

		tmpSharedResourceName.assign(sharedResourceName);
		tmpSharedResourceName.append("0readEvent");

		readEvents[0].open(false,tmpSharedResourceName.c_str());

		tmpSharedResourceName.assign(sharedResourceName);
		tmpSharedResourceName.append("lockMutex");
		lockMutex[0].open(tmpSharedResourceName.c_str());

		tmpSharedResourceName.assign(sharedResourceName);
		tmpSharedResourceName.append("sharedMemory");
		sharedAddress= (SharedAddress *) sharedMemory.open(tmpSharedResourceName.c_str());

		multiConnection = true;
		initObject=true;
		bool ret = _writeConnectMessage(newConnection);
		clearEverything();
		if (ret)
		{
			PTRACE3("Shared memory %s connected to : %s",newConnection,sharedResourceName.c_str());
		}
		return ret;
	}
	catch(PError &err)
	{
		_sharedMemoryError(err.why());
	}
	return false;
}


void CommSharedMemory::pushMessage(_CommMsg & msg)
{

	PLock lc(cs);
	if (initObject == false)
	{
		PTRACE3("pushMessage - initObject=false");
		return;
	}

	/*
	if (wrQueue.size())
	{
	if (!(sharedAddress[readBlock?0:1].controlFlag & SHARED_CHANNEL_WAIT_TO_WRITE)) 
	//		if (writeEvents[readBlock?0:1].wait(0) == 0)
	{
	peerReadData();
	}
	}
	*/

	if(wrBlocks[msg.priority].offset>=0)
	{
		wrQueue.push( msg );
	}
	else
	{
		_CommMsg msg2;
		bool msgReady = false;

		if( wrQueue.size() == 0 || msg.priority > wrQueue.peek().priority )
		{
			msg2.moveFrom(msg);
			msgReady = true;
		}
		else
		{
			wrQueue.push( msg );
			if(wrBlocks[wrQueue.peek().priority].offset <0)
			{
				msg2.moveFrom(wrQueue.peek());
				wrQueue.pop();
				msgReady = true;
			}
		}
		if (msgReady)
		{
			bool flagWasWrite = false;
			while(1)
			{
				bool flagWrite1 = false;
				bool ok = _writeMsg(msg2,flagWrite1);
				flagWasWrite = flagWrite1 || flagWasWrite;
				if (ok == false) 
					break;
				if(wrQueue.size() && wrBlocks[wrQueue.peek().priority].offset <0)
				{
					msg2.moveFrom(wrQueue.peek());
					wrQueue.pop();
				}
				else
					break;
			}
			if (flagWasWrite)
				readEvents[readBlock].set();
		}
	}

}

void CommSharedMemory::closeSharedMemory()
{
	PLock lc(cs);
	if (initObject == false)
	{
		PTRACE3("closeSharedMemory - initObject=false");
		return;
	}
	int bufferAvailable;
	int block = readBlock?0:1; 
	lock(block);
	if ((bufferAvailable = SHARED_MEMORY_BLOCK - sharedAddress[block].endFrame) > 0)
	{
		*(BYTE *)(sharedAddress[block].buffer + sharedAddress[block].endFrame) = 0x20; 
		sharedAddress[block].endFrame+=1;
		readEvents[readBlock].set();
	}
	unlock(block);
	clearEverything();
}

void CommSharedMemory::peerReadData()
{
	PLock lc(cs);

	int wrPr;
	bool flagWasWrite = false;
	if (initObject == false)
	{
		PTRACE3("peerReadData - initObject=false");
		return;
	}

	for(wrPr=_COMM_MSGPRIORITIES-1; wrPr >= 0 ; --wrPr )
	{
		if(wrBlocks[wrPr].offset>=0)
		{
			bool flagWrite1 = false;
			bool Ok_1 = _writeMsg( wrPr,flagWrite1 );
			flagWasWrite = flagWasWrite || flagWrite1;
			if( Ok_1 == false)
				break;
		}

		if(wrBlocks[wrPr].offset>=0)
		{
			break;
		}

		bool Ok_2=true;
		while( wrQueue.size()>0 && wrPr == wrQueue.peek().priority && wrBlocks[wrPr].offset < 0)
		{
			bool flagWrite1 = false;
			_CommMsg& msg = wrQueue.peek();
			Ok_2 = _writeMsg( msg,flagWrite1 );
			wrQueue.pop();
			flagWasWrite = flagWasWrite || flagWrite1;
			if( Ok_2==false)
				break;
		}
		if (Ok_2 == false || wrBlocks[wrPr].offset>=0)
		{
			break;
		}
	}
	if (flagWasWrite)
		readEvents[readBlock].set();
}

int CommSharedMemory::getMessage(_CommMsg & message)
{
	PLock lc(cs);
	if (initObject == false)
	{
		PTRACE3("getMessage - initObject=false");
		return -1;
	}
	lock(readBlock);

	if (!sharedAddress[readBlock].endFrame)
	{
		if (! (sharedAddress[readBlock].controlFlag & SHARED_CHANNEL_OPEN))
		{
			unlock(readBlock);
			return -1;
		}
		unlock(readBlock);
		return 0;
	}

	const BYTE  * readBuffer = (const BYTE *) (sharedAddress[readBlock].buffer+sharedAddress[readBlock].beginFrame);

	BYTE b = *readBuffer++;
	bool isFirst = ( b & 0x80 ) != 0;
	bool isLast =  ( b & 0x40 ) != 0;
	bool isClose = ( b & 0x20 ) != 0; 

	if ( isClose || ( !(sharedAddress[readBlock].controlFlag & SHARED_CHANNEL_OPEN) && isLast == false ))
	{
		unlock(readBlock);
		return -1;
	}

	BYTE priority = *readBuffer++ & 0x3F;
	PASSERT3( priority < _COMM_MSGPRIORITIES );
	_CommMsg& msg = rdMsgs[ priority ];


	if( isFirst )
	{
		UINT16 flags = CommMsgBody::readUINT16( readBuffer );
		readBuffer += 2;
		BYTE type = *readBuffer++;
		msg.priority = priority;
		msg.flags = flags;
		msg.type = type;
		if (muxEnabled)
		{
			msg.internal.mux = CommMsgBody::readUINT16( readBuffer );
			readBuffer += 2;
			msg.internal.muxChannelId = CommMsgBody::readUINT32( readBuffer );
			readBuffer += 4;
		}
		else
		{
			msg.internal.mux = 0;
			msg.internal.muxChannelId = 0;
			readBuffer += 6;
		}

		size_t size = CommMsgBody::readUINT16( readBuffer );
		readBuffer+=2;
		memcpy( msg.body._append( size ), readBuffer, size );
		readBuffer+=size;
		sharedAddress[readBlock].beginFrame+=(size + SHARED_HEADER_SIZE);
	}
	else
	{
		readBuffer+= (SHARED_HEADER_SIZE - 2 - 2);
		size_t size = CommMsgBody::readUINT16( readBuffer );
		readBuffer+=2;
		memcpy( msg.body._append( size ), readBuffer, size );
		readBuffer+=size;
		sharedAddress[readBlock].beginFrame+=(size + SHARED_HEADER_SIZE);

	}
	if( isLast )
	{
		message.moveFrom(msg);
		msg.body._dealloc();
	}
	PASSERT3(sharedAddress[readBlock].beginFrame <= sharedAddress[readBlock].endFrame);
	if (sharedAddress[readBlock].beginFrame == sharedAddress[readBlock].endFrame)
	{
		sharedAddress[readBlock].beginFrame =0;
		sharedAddress[readBlock].endFrame =0;
		if (sharedAddress[readBlock].controlFlag & SHARED_CHANNEL_WAIT_TO_WRITE)
		{
			sharedAddress[readBlock].controlFlag &=~SHARED_CHANNEL_WAIT_TO_WRITE;
			writeEvents[readBlock].set();
		}
	}
	unlock(readBlock);
	return isLast ? 1 : 0;
}

int CommSharedMemory::acceptConnectionMessage(char * connection)
{
	PLock lc(cs);
	if (initObject == false)
	{
		PTRACE3("acceptMessage - initObject=false");
		return -1;
	}
	lock(0);
	if (! (sharedAddress[0].controlFlag & SHARED_CHANNEL_OPEN))
	{
		unlock(0);
		return -1;
	}

	if (!sharedAddress[0].endFrame)
	{
		unlock(0);
		return 0;
	}

	const BYTE  * readBuffer = (const BYTE *) (sharedAddress[0].buffer+sharedAddress[0].beginFrame);
	int len = strlen((char *)readBuffer);
	strcpy((char *)connection,(char*)readBuffer);

	sharedAddress[0].beginFrame+=(len+1);

	PASSERT3(sharedAddress[0].beginFrame <= sharedAddress[0].endFrame);
	if (sharedAddress[0].beginFrame == sharedAddress[0].endFrame)
	{
		sharedAddress[0].beginFrame =0;
		sharedAddress[0].endFrame =0;
	}
	unlock(0);
	PTRACE3("Shared memory %s accept ConnectionMessage from: %s",sharedResourceName.c_str(),connection);
	return 1;
}

