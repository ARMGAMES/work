#ifndef _commssl_h
#define _commssl_h

#include "ppinclude.h"
#include "ppstring.h"
#include "pplogfile.h"
#include "_commcomm.h"
#include "commssl.h"
#include "commstreamencryption.h"

class _CommSSLBIOCommunicator;
class _CommSSLCommunicator
{
private:
	CommSSLClient* cli;
	CommSSLServer* srv;
	_CommVirtualCommunicator* vComm;
	/* new */ _CommSSLBIOCommunicator* comm;
	bool isServer;
	int state;
	void* ssl;//SSL*

	int initErr;
	BYTE* rdBuf;
	size_t rdSz;
	int rdErr;
	const BYTE* wrBuf;
	size_t wrSz;
	int wrErr;

public:
	static bool hasCompression( int type ) { return false; }
	void setCompression( int type, size_t BuffSize ) {}
	void setEncryption( CommStreamEncryption * encryption_ ) {}
	void flush() {}
	bool initConnection(UINT32 & checkSumm);
	bool generateRandom(UINT32 * secureRandom, int size);

public:
	_CommSSLCommunicator();
	void construct(  CommSSLClient* cli_, CommSSLServer* srv_, _CommVirtualCommunicator* vComm_ );
	void init( bool inbound );
	~_CommSSLCommunicator();

public:
	size_t writeIncomplete( const BYTE* p, size_t sz );
	size_t readIncomplete( BYTE* p, size_t sz );

	_CommAsyncRet asyncWriteOk();
	_CommAsyncRet asyncReadOk();

private:
	void _init();
	void _del();

	bool _checkInitConn();
	bool _initConn();
	_CommAsyncRet _reinitConn();
};

//typedef _CommCompleteCommunicator< _CommSSLCommunicator > _CommCompleteSSLCommunicator;

#endif
