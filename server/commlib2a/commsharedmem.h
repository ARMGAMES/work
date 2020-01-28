#ifndef commsharedmem_h_included
#define commsharedmem_h_included


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
#define SHARED_CHANNEL_WAIT_TO_WRITE 2

#if defined (PWIN32) && !defined(COMM_CLIENT_ONLY)
#define SHARED_GLOBAL "Global\\"
#else
#define SHARED_GLOBAL ""
#endif

PString initSharedMemProcessId(const PString& sharedResourceName);

class CommSharedMemory : public CommSharedMemoryIntrf
{

private:
	bool compatible;

	struct SharedAddress
	{
		UINT32 controlFlag;
		UINT32 beginFrame;
		UINT32 endFrame;
		char buffer[SHARED_MEMORY_BLOCK];
	};

	struct _Write
	{
		_CommMsg msg;
		int offset;
	};


	PString sharedResourceName; 
	bool initObject;
	bool multiConnection;
	bool muxEnabled;


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

	CommMsgSimpleQueue wrQueue;
	_Write wrBlocks[_COMM_MSGPRIORITIES];
	_CommMsg rdMsgs[_COMM_MSGPRIORITIES];

	void _sharedMemoryError(const char * msg);
	void clearEverything();
	bool _writeMsg(_CommMsg &msg, bool & setWrite);
	bool _writeMsg(int priority, bool & setWrite);
	void lock(int n)
	{
		PASSERT3(initObject);
		if (compatible)
			lockMutex[0].set();
		else
			lockMutex[n].set();
	}

	void unlock(int n)
	{
		PASSERT3(initObject);
		if (compatible)
			lockMutex[0].release();
		else
			lockMutex[n].release();
	}

	bool _writeConnectMessage(const char * connectionMessage);
public:

	void enableMux()
	{
		muxEnabled=true;
	}
	CommSharedMemory(bool compatible_ = false);
	void forceDisconnectSharedMemory() override;
	const PString & getResourceName() override
	{
		return sharedResourceName;
	}

	~CommSharedMemory()
	{
		CommSharedMemory::forceDisconnectSharedMemory();
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

		if (multiConnection)
			return readEvents[0].getSocket();
		else
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
			PTRACE("resetReadEvent - initObject=false");
			return;
		}

		if (multiConnection)
			readEvents[0].reset();
		else
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
	void Accept(const char * sharedResourceName_);
	void Open(const char * sharedResourceName_);
	bool Connect(const char * sharedResourceName_, const char * newConnection);
	void pushMessage(_CommMsg & msg);
	void closeSharedMemory();
	void peerReadData();
	int getMessage(_CommMsg & message);
	int acceptConnectionMessage(char * connection);
	void dump();
	bool serviceCallTransport(UINT32 id, CommMsgBody *msg_body)
	{
		switch(id)
		{
		case  _SERVICE_REQUEST_STATISTIC:
			{
				if(msg_body)
				{
					msg_body->composeUINT32(wrQueue.size());
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

class CommSharedMemoryEncrypted : public CommSharedMemory
{
private:
	CommSSLStreamEncrypt * forSend;
	CommSSLStreamDecrypt * forReceive;
public:
	CommSharedMemoryEncrypted() : CommSharedMemory(true)
	{
		forSend = 0;
		forReceive = 0;
	}
	~CommSharedMemoryEncrypted()
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
		int ret = CommSharedMemory::getMessage(message);
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
		CommSharedMemory::pushMessage(message);	
	}
};

#endif
