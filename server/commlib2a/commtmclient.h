#ifndef commtmclient_h_included
#define commtmclient_h_included

#include "ppinclude.h"
#include "ppthread.h"
#include "ppsocket.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "commrtable.h"
#include "commssl.h"
#include "commtmrw.h"

class _CommClientThreadBase;
/* abstract */ class CommClientThreadManagerBase : public CommThreadManager
{
private:
	friend class _CommClientThreadBase;

	typedef CommThreadManager _Parent;
	PString httpsProxy;
	int httpsProxyPort;
	int freezeReconnectTimeout;
	int tcpConnectTimeout;
	int sslConnectTimeout;
	PString threadPriority;
	bool tcpNoDelay;
	int tcpBufferRcv;
	int tcpBufferSnd;
	bool optimizePacketSize;


public:
	UINT32 MaxBlockSize;
	UINT32 CompressBitsSize;
	UINT32 DefaultProtocol;
	bool   enableClientEncryption;

	_CommProtocolLog * comm_logging;

public:
	CommClientThreadManagerBase( CommRoutingTable& routingTable_ );
	void setOption( const char* optName, const char* optValue ) override;
	/* virtual*/ ~CommClientThreadManagerBase() {}

public:
	void connect( _CommPhysicalConnection* physConn ) override;
	void forceDisconnect( _CommPhysicalConnection* physConn ) override;
	void _debugEmulateNetworkFailure();
	const char * whoAreYou() const override
	{
		if (managerIdent.length())
			return managerIdent.c_str();
		else
			return "CommClientThreadManagerBase";
	}
	void setWhoAreYou(const char * managerIdent_) override
	{
		managerIdent.assign("CommClientThreadManagerBase:");
		managerIdent.append(managerIdent_);
		managerType.assign(managerIdent_);
	}


	//helpers to allow access for _CommSimpleThread<...>
	int getTcpConnectTimeout() const { return tcpConnectTimeout; }
	bool getOptimizePacketSize() const { return optimizePacketSize; }
	int getSslConnectTimeout() const { return sslConnectTimeout; }
	const char* getThreadPriority() const { return threadPriority; }
	bool getTcpNoDelay() const { return tcpNoDelay; }
	UINT32 getTcpBufferRcv() const { return tcpBufferRcv; }
	UINT32 getTcpBufferSnd() const { return tcpBufferSnd; }
	const char* getHttpsProxy() const { return httpsProxy.c_str(); }
	int getHttpsProxyPort() const { return httpsProxyPort; }

private:

	void _freeSocket( int threadIdx );
	int _createThread( UINT32& channelId, _CommPhysicalConnection* physConn, const char* addr, int masterIdx, UINT32 masterChannelId );

public://overridables
	virtual _CommClientThreadBase* _createNewThread( int threadIdx, _CommPhysicalConnection* physConn ) = 0;
};

class CommClientThreadManager : public CommClientThreadManagerBase
{
public:
	CommClientThreadManager( CommRoutingTable& routingTable_ )
		: CommClientThreadManagerBase( routingTable_ )
	{
	}

public:
	bool handles( const char* address ) const override;
	void parseSocketAddr( PString& retAddr, int& port, const char* address ) override;
	void composeSocketAddr( PString& retAddr, const char* address, int port ) override;
	const char * whoAreYou() const override
	{
		if (managerIdent.length())
			return managerIdent.c_str();
		else
			return "CommClientThreadManager";
	}
	void setWhoAreYou(const char * managerIdent_) override
	{
		managerIdent.assign("CommClientThreadManager:");
		managerIdent.append(managerIdent_);
		managerType.assign(managerIdent_);
	}

	_CommClientThreadBase* _createNewThread( int threadIdx, _CommPhysicalConnection* physConn ) override;
};

class CommClientThreadManagerSSL : public CommClientThreadManagerBase, public CommRWThreadManagerBaseClientSSL
{
private:
	friend class _CommClientSSL;

public:
	CommClientThreadManagerSSL( CommRoutingTable& routingTable_ );

	const char * whoAreYou() const override
	{
		if (managerIdent.length())
			return managerIdent.c_str();
		else
			return "CommClientThreadManagerSSL";
	}
	void setWhoAreYou(const char * managerIdent_) override
	{
		managerIdent.assign("CommClientThreadManagerSSL:");
		managerIdent.append(managerIdent_);
		managerType.assign(managerIdent_);
	}


	~CommClientThreadManagerSSL();

public:
	bool handles( const char* address ) const override;
	void parseSocketAddr( PString& retAddr, int& port, const char* address ) override;
	void composeSocketAddr( PString& retAddr, const char* address, int port ) override;
	_CommClientThreadBase* _createNewThread( int threadIdx, _CommPhysicalConnection* physConn ) override;
};

#endif
