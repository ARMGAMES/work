#ifndef commtmrwudp_h_included
#define commtmrwudp_h_included

#include "ppinclude.h"
#ifdef UDT_DEVELOPMENT
#include "ppsocketudt.h"
#endif

#include "ppthread.h"
#include "ppsocket.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "commrtable.h"
#include "commtmrw.h"


#ifdef PWIN32

class _CommRWUDPThread;
class _CommRWUDPWorkerThread;

/* abstract */ class CommRWUDPThreadManager : public CommThreadManager
{
public:
	struct Peer; 
	typedef map<UINT64,Peer *> PeerMap;

	struct UdpCommBuffer;

	PUDPSocket * sock;
	HANDLE completionPort;
	int numBuffers;
	int numThreads;
	int numConcurrent;
	int udpPacketSize;
	int keepAliveRead;
	int keepAliveWrite;
	int keepAlivePoll;
	int numSlots;
	int resend;
	int udpBufferSizeRcv;
	int udpBufferSizeSnd;

	UdpCommBuffer *udpbuffs;
	PeerMap peers;

private:
	typedef CommThreadManager _Parent;

	PReadWriteLock localLock;
protected:

public:
	bool   isFar;
	_CommProtocolLog * comm_logging;

private:
	_CommRWUDPWorkerThread *workerThreads;
	int lPort;
	bool started;


public:
	CommRWUDPThreadManager( CommRoutingTable& routingTable_ );
	void setOption( const char* optName, const char* optValue ) override;
	void start() override;
	void shutdown() override;
	bool terminated() override;
	/* virtual */ ~CommRWUDPThreadManager();

	void removePeer( Peer* peer);
	void processMsg(UdpCommBuffer * buff);
	void processError(UdpCommBuffer * buff, UINT32 err, const char * errorMsg);
public:
//	void connect( _CommPhysicalConnection* physConn ) override;
	void forceDisconnect( _CommPhysicalConnection* physConn ) override;
	const char * whoAreYou() const override 
	{
		return "CommRWUDPThreadManagerBase";
	} 
	void loggerInformation(CommMsgBody &body) override; 
	void dynamicInit(PIniFile& iniFile) override;

	// publish for thin client

private:
	_CommRWUDPThread* _createNewThread( int threadIdx );
	/* inline */ static _CommRWUDPThread* _rwThread( Thread& thread );
	void _processMsg(Peer* peer, UdpCommBuffer * buff);
	void _internalForceDisconnect(Peer* peer, int errCode, const char * errMsg);

private:

	void _pushInSocket( const PUDPSocket::Descriptor& sockDescr, const char* addr, UINT64 key, UdpCommBuffer * buff );
	void _pushSocket( const CommRWUDPThreadManager::Peer* sock );

	int  _numberOfConnections();
	bool _freeSocket( int threadIdx );//to be called from RWThread
	//returns true if thread should be terminated
	//no thread operations allowed after freeSocket returned true//no thread operations allowed after freeSocket returned true
private:

	friend class _CommRWUDPThread;
	friend class _CommRWUDPWorkerThread;

public:

	void listenPort( int port ) override;

	//	virtual void loggerInformation(CommMsgBody &body); 
	//	virtual void dynamicInit(PIniFile& iniFile);
	virtual bool connectIsOutOfProcess() { return false;}

public:
	bool handles( const char* address ) const override;
	void parseSocketAddr( PString& retAddr, int& port, const char* address ) override;
	void composeSocketAddr( PString& retAddr, const char* address, int port ) override;
};

#endif



#ifdef UDT_DEVELOPMENT
class CommRWThreadManagerUDT : public CommRWThreadManagerBaseIP
{
private:
	int inUdpBuffer;
	int outUdpBuffer;
	int udpMSS;
	int udpFlightFlagSize;
	int udpMaxSendBufferSize;
    int udpMaxRecvBufferSize;
	int udpRTT;
	int udpEstimatedBandwidth;


public:
#ifndef UDT_DLL
	PUDTSocketContainer pudtContainer;
#endif

	CommRWThreadManagerUDT( CommRoutingTable& routingTable_, bool enableMux = false );
	const char * whoAreYou() const override
	{
		if (managerIdent.length())
			return managerIdent.c_str();
		else
			return "CommRWThreadManagerUDT";
	}
	void listenPort( int port ) override;
	void setWhoAreYou(const char * managerIdent_) override
	{
		managerIdent.assign("CommRWThreadManagerUDT:");
		managerIdent.append(managerIdent_);
		managerType.assign(managerIdent_);
	}
	bool handles( const char* address ) const override;
	void parseSocketAddr( PString& retAddr, int& port, const char* address ) override;
	void composeSocketAddr( PString& retAddr, const char* address, int port ) override;
	void loggerInformation(CommMsgBody &body) override;
	void start() override;
	void shutdown() override;
	void setOption( const char* optName, const char* optValue ) override;
	UINT32 printCounters() override
	{
		UINT32 _max = CommRWThreadManagerBaseIP::printCounters();
		pudtContainer.printCounters();
		return _max;
	}
private:
	_CommRWThreadBase* _createNewThread( int threadIdx ) override;
};

class CommRWThreadManagerSSLUDT : public CommRWThreadManagerUDT, public CommRWThreadManagerBaseClientSSL, public CommRWThreadManagerBaseServerSSL
{
public:
	CommRWThreadManagerSSLUDT( CommRoutingTable& routingTable_, bool enableMux = false );
	const char * whoAreYou() const override
	{
		if (managerIdent.length())
			return managerIdent.c_str();
		else
			return "CommRWThreadManagerSSLUDT";
	}
	void setWhoAreYou(const char * managerIdent_) override
	{
		managerIdent.assign("CommRWThreadManagerSSLUDT:");
		managerIdent.append(managerIdent_);
		managerType.assign(managerIdent_);
	}
	bool handles( const char* address ) const override;
	void parseSocketAddr( PString& retAddr, int& port, const char* address ) override;
	void composeSocketAddr( PString& retAddr, const char* address, int port ) override;
	void loggerInformation(CommMsgBody &body) override;
private:
	_CommRWThreadBase* _createNewThread( int threadIdx ) override;
};

#endif




#endif
