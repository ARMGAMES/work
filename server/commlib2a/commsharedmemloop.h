#ifndef commsharedmemloop_h_included
#define commsharedmemloop_h_included


#include "ppinclude.h"
#include "ppstring.h"
#include "pplogfile.h"
#include "ppipc.h"
#include "commsharedmemintrf.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "_commcomm.h"
#include "commssl.h"

#define SHARED_MEMORY_BLOCK 4096


#define SHARED_ON_READY_READ  0
#define SHARED_ON_READY_WRITE 1
#define SHARED_HEADER_SIZE 13

#define SHARED_CHANNEL_OPEN 1


class CommSharedMemoryLoop : public CommSharedMemoryIntrf
{

private:
	//	bool compatible;
	enum {eWasWritten=1, eBufferFull=2};

	struct SharedAddress
	{
		volatile UINT32 controlFlag;
		volatile UINT32 writeFlag;
		volatile INT32 readFrame;
		volatile INT32 writeFrame;
		char buffer[SHARED_MEMORY_BLOCK];
	};

	struct _Write
	{
		_CommMsg msg;
		int offset;
	};


	PString sharedResourceName; 
	bool initObject;
	//	bool multiConnection;
	bool muxEnabled;

	UINT32 packetsIn;
	UINT32 packetsOut;
	UINT64 bytesIn;
	UINT64 bytesOut;
	_CommProtocolLog *_logger;
	int slot;
	UINT32 _whenStored;
	PCriticalSection cs;

	PNamedEvent readEvents[2];
	PNamedEvent writeEvents[2];
	PNamedMutex lockMutex[2];
	PNamedSharedMemory sharedMemory;
#ifdef PWIN32
	HANDLE peerProcess;
#endif
	SharedAddress * sharedAddress;
	int readBlock;

	UINT32 lastTimeInQueue;

	_Write wrBlocks[_COMM_MSGPRIORITIES];
	_CommMsg rdMsgs[_COMM_MSGPRIORITIES];

	void _sharedMemoryError(const char * msg);
	void clearEverything();
	inline void _writeToBuffer(INT32 currentReadPosition, INT32 & currentWritePosition, const BYTE * byteToWrite, size_t length);
	inline void _readFromBuffer(INT32 & currentReadPosition, INT32 currentWritePosition, BYTE * byteToRead, size_t length);
	inline void _shiftReadBuffer(INT32 & currentReadPosition, INT32 currentWritePosition,  size_t length);
	UINT32 _writeMsg(_CommMsg &msg);
	UINT32 _writeMsg(int priority);
	void   _pushData();
	void lock(int n)
	{
		PASSERT3(initObject);
		lockMutex[n].set();
	}

	void unlock(int n)
	{
		PASSERT3(initObject);
		lockMutex[n].release();
	}

	//	bool _writeConnectMessage(const char * connectionMessage);
public:
	CommMsgSimpleTimeControlQueue wrQueue;
	void enableMux()
	{
		muxEnabled=true;
	}
	CommSharedMemoryLoop();
	void forceDisconnectSharedMemory() override;
	const PString & getResourceName() override
	{
		return sharedResourceName;
	}

	~CommSharedMemoryLoop()
	{
		CommSharedMemoryLoop::forceDisconnectSharedMemory();
	}
	void construct(_CommProtocolLog * logger_, int slot_)
	{
		_logger = logger_;
		slot = slot_;
	}
	bool init() const
	{
		return initObject;
	}

	SOCKET getReadHandle() const
	{
		if (initObject == false)
		{
			PTRACE3("getReadHandle - initObject=false");
			return NULL;
		}
		/*
		if (multiConnection)
		return readEvents[0].getSocket();
		else
		*/
		return readEvents[readBlock?0:1].getSocket();
	}

	SOCKET getWriteHandle() const
	{
		if (initObject == false)
		{
			PTRACE3("getWriteHandle - initObject=false");
			return NULL;
		}

		return writeEvents[readBlock?0:1].getSocket();
	}
	void resetReadEvent()
	{
		if (initObject == false)
		{
			PTRACE3("resetReadEvent - initObject=false");
			return;
		}
		/*
		if (multiConnection)
		readEvents[0].reset();
		else
		*/
		readEvents[readBlock?0:1].reset();
	}

	void resetWriteEvent()
	{
		if (initObject == false)
		{
			PTRACE3("resetWriteEvent - initObject=false");
			return;
		}
		writeEvents[readBlock?0:1].reset();
	}

#ifdef PWIN32
	HANDLE getPeerProcessHandle() const
	{
		if (initObject == false)
		{
			PTRACE3("getPeerProcessHandle - initObject=false");
			return NULL;
		}

		return peerProcess;
	}
#endif
	void Create(const char * sharedResourceName_);
	//	void Accept(const char * sharedResourceName_);
	void Open(const char * sharedResourceName_);
	//	bool Connect(const char * sharedResourceName_, const char * newConnection);
	void pushMessage(_CommMsg & msg);
	void closeSharedMemory();
	void peerReadData();
	int getMessage(_CommMsg & message);
	//	int acceptConnectionMessage(char * connection);
	void dump();
	void _log(const char * fmt, ...)
	{
		if(_logger)
		{
			PString st;
			st.appendHexInt(THREADID());
			st += "-";
			st += sharedResourceName;
			st += "-";
			st.appendInt(slot);
			st += "-";
			va_list args;
        	va_start( args, fmt );
	    	_logger->_CommLog(st,fmt, args);
		    va_end(args);
		}
	}

	bool serviceCallTransport(UINT32 id, CommMsgBody *msg_body)
	{
		switch(id)
		{
		case  _SERVICE_REQUEST_STATISTIC:
			{
				if(msg_body)
				{
					msg_body->composeUINT32(wrQueue.size()).
						      composeUINT32(lastTimeInQueue);
				}
				UINT32 now = SYSTEMTICKS();
				if (_whenStored == 0)
					_whenStored = now;
				else
				{
					if (now-_whenStored > 60000)
					{
						if (_logger)
						{
							_log("Received packets(bytes) %u(%" PRIu64 "), sent %u(%" PRIu64 ")", 
								 packetsIn,bytesIn,packetsOut,bytesOut);
						}
						packetsIn=packetsOut=0;
						bytesIn=bytesOut=0;
						_whenStored = now;
					}
				}

				return true;
			}
			break;
		default:
			break;
		}
		return false;
	}
};

class CommSharedMemoryEncryptedLoop : public CommSharedMemoryLoop
{
private:
	CommSSLStreamEncrypt * forSend;
	CommSSLStreamDecrypt * forReceive;
public:
	CommSharedMemoryEncryptedLoop() : CommSharedMemoryLoop()
	{
		forSend = 0;
		forReceive = 0;
	}
	~CommSharedMemoryEncryptedLoop()
	{
		if (forSend)
		{
			delete forSend;
		}
		if (forReceive)
		{
			delete forReceive;
		}
	}
	void setPasswords(const char * passForSend, const char * passForReceive, const unsigned char * saltSend, const unsigned char * saltReceive)
	{
		forSend = new CommSSLStreamEncrypt(passForSend, saltSend);
		forReceive = new CommSSLStreamDecrypt(passForReceive, saltReceive);
	}
	int getMessage(_CommMsg & message)
	{
		int ret = CommSharedMemoryLoop::getMessage(message);
		if ( (ret > 0) && forReceive)
		{
			PBlock body;
			forReceive->decrypt(message.body._readPtr()+1,message.body._size()-1,body);

			message.priority = *message.body._readPtr();
			message.body._shift(1);
			memcpy(message.body._writePtr(),body.ptr(),body.size());
		}
		return ret;
	}
	void pushMessage(_CommMsg & message)
	{
		if (forSend)
		{
			PBlock body;
			forSend->encrypt(message.body._readPtr(),message.body._size(),body);
			message.body._append(1);
			*message.body._writePtr() = message.priority;
			memcpy(message.body._writePtr()+1,body.ptr(),body.size());
			message.priority=_COMM_MSGPRIORITY_INITCONNECT;
		}
		CommSharedMemoryLoop::pushMessage(message);	
	}
};

#endif
