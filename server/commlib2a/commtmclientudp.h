#ifndef commtmclientudp_h_included
#define commtmclientudp_h_included


#include "ppinclude.h"
#include "ppthread.h"
#include "ppsocket.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "commrtable.h"

#ifdef PWIN32

class _CommClientUDPThreadBase;
/* abstract */ class CommClientUDPThreadManager : public CommThreadManager
{
private:
	friend class _CommClientUDPThreadBase;
	friend class _CommClientUDPPlain;

	typedef CommThreadManager _Parent;
	PString threadPriority;
	int udpConnectTimeout;
	int udpPacketSize;
	int keepAliveRead;
	int keepAliveWrite;
	int keepAlivePoll;
	int resend;


public:
	_CommProtocolLog * comm_logging;

public:
	CommClientUDPThreadManager( CommRoutingTable& routingTable_ );
	void setOption( const char* optName, const char* optValue ) override;
	/* virtual*/ ~CommClientUDPThreadManager() {}

public:
	void connect( _CommPhysicalConnection* physConn ) override;
	void forceDisconnect( _CommPhysicalConnection* physConn ) override;
	void _debugEmulateNetworkFailure();
	const char * whoAreYou() const override
	{
		if (managerIdent.length())
			return managerIdent.c_str();
		else
			return "CommClientUDPThreadManager";
	}
	void setWhoAreYou(const char * managerIdent_) override
	{
		managerIdent.assign("CommClientUDPThreadManager:");
		managerIdent.append(managerIdent_);
		managerType.assign(managerIdent_);
	}


	const char* getThreadPriority() const { return threadPriority; }
	int getUdpConnectTimeout() const { return udpConnectTimeout; }
	int getKeepAlivePoll() const { return keepAlivePoll; }
private:

	void _freeSocket( int threadIdx );
	int _createThread( UINT32& channelId, _CommPhysicalConnection* physConn, const char* addr, int masterIdx, UINT32 masterChannelId );

public://overridables
	bool handles( const char* address ) const override;
	void parseSocketAddr( PString& retAddr, int& port, const char* address ) override;
	void composeSocketAddr( PString& retAddr, const char* address, int port ) override;
	_CommClientUDPThreadBase* _createNewThread( int threadIdx, _CommPhysicalConnection* physConn );
};

#endif
#endif
