#ifndef ppsocketudt_h_included
#define ppsocketudt_h_included



#ifndef UDT_DLL
#include "udt/pudtsocket.h"
#define PUDTSOCKET UINT32
#define pudtgetsockreadevent pudtContainer->getsockreadevent 
#define pudtgetsockwriteevent pudtContainer->getsockwriteevent 
#define pudtclose pudtContainer->close 
#define pudtrecvnonblocking pudtContainer->recvnonblocking 
#define pudtsendnonblocking pudtContainer->sendnonblocking 
#define PUDTERROR -1
#else
#include "ppsocket.h"
#include <c:\udt\udt4\src\udt.h>
#define PUDTSOCKET UDTSOCKET 
#define pudtgetsockreadevent UDT::getsockreadevent 
#define pudtgetsockwriteevent UDT::getsockwriteevent 
#define pudtclose UDT::close 
#define pudtrecvnonblocking UDT::recvnonblocking 
#define pudtsendnonblocking UDT::sendnonblocking 
#define PUDTERROR UDT::ERROR
#endif


class PMultiSocketRWUDT
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
		HANDLE readEvent;
		HANDLE writeEvent;
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

#ifndef UDT_DLL
	PUDTSocketContainer * pudtContainer;
#endif
public:
	PMultiSocketRWUDT( SOCKET queue, int numberOfConnections_);
	~PMultiSocketRWUDT();
	void setSocketContainer(PUDTSocketContainer *cont_)
	{
		pudtContainer = cont_;

	}
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
};

#endif