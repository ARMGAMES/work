#ifndef commtmsimple_h_included
#define commtmsimple_h_included

#include "ppinclude.h"
#include "ppthread.h"
#include "ppsocket.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "commrtable.h"
#include "commssl.h"

class _CommSimpleThreadBase;
/* abstract */ class CommSimpleThreadManagerBase : public CommThreadManager
{
private:
	friend class _CommSimpleThreadWriter;
	friend class _CommSimpleThreadReader;
	friend class _CommSimpleThreadBase;

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

public:
	UINT32 MaxBlockSize;
	UINT32 CompressBitsSize;
	UINT32 DefaultProtocol;
#ifndef COMM_CLIENT_ONLY
	UINT32 maximumMessageSize;
	UINT32 maximumMessageRate;
	UINT32 maximumMessageRatePeriod;
	bool   maxMessageSizeDebug; 
	bool   maxMessageRateDebug; 
	UINT32 minimumProtocol;
	UINT32 publicKey[5];
	UINT32 privateKey[5];
	bool   enableServerEncryption;
#endif
	bool   enableClientEncryption;

	_CommProtocolLog * comm_logging;


public:
	CommSimpleThreadManagerBase( CommRoutingTable& routingTable_ );
	void setOption( const char* optName, const char* optValue ) override;
	/* virtual*/ ~CommSimpleThreadManagerBase() {}

public:
	void connect( _CommPhysicalConnection* physConn ) override;
	void forceDisconnect( _CommPhysicalConnection* physConn ) override;
	const char * whoAreYou() const override
	{
		if (managerIdent.length())
			return managerIdent.c_str();
		else
			return "CommSimpleThreadManagerBase";
	}
	void setWhoAreYou(const char * managerIdent_) override
	{
		managerIdent.assign("CommSimpleThreadManagerBase:");
		managerIdent.append(managerIdent_);
		managerType.assign(managerIdent_);
	}


	void createSlaveThread( int& slaveIdx, UINT32& slaveChannelId, const char* addr, int masterIdx, UINT32 masterChannelId );
	void pushToThread( int slaveIdx, _CommMsg& m );
	void _debugEmulateNetworkFailure();

	//helpers to allow access for _CommSimpleThread<...>
	int getFreezeReconnectTimeout() const { return freezeReconnectTimeout; }
	int getTcpConnectTimeout() const { return tcpConnectTimeout; }
	int getSslConnectTimeout() const { return sslConnectTimeout; }
	const char* getThreadPriority() const { return threadPriority; }
	bool getTcpNoDelay() const { return tcpNoDelay; }
	UINT32 getTcpBufferRcv() const { return tcpBufferRcv; }
	UINT32 getTcpBufferSnd() const { return tcpBufferSnd; }

private:

	void _freeSocket( int threadIdx );
	int _createThread( UINT32& channelId, _CommPhysicalConnection* physConn, const char* addr, int masterIdx, UINT32 masterChannelId );

public://overridables
	virtual _CommSimpleThreadBase* _createNewThread( int threadIdx, _CommPhysicalConnection* physConn ) = 0;
};

class CommSimpleThreadManager : public CommSimpleThreadManagerBase
{
public:
	CommSimpleThreadManager( CommRoutingTable& routingTable_ )
		: CommSimpleThreadManagerBase( routingTable_ )
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
			return "CommSimpleThreadManager";
	}
	void setWhoAreYou(const char * managerIdent_) override
	{
		managerIdent.assign("CommSimpleThreadManager:");
		managerIdent.append(managerIdent_);
		managerType.assign(managerIdent_);
	}

	_CommSimpleThreadBase* _createNewThread( int threadIdx, _CommPhysicalConnection* physConn ) override;
};

class CommSimpleThreadManagerSSL : public CommSimpleThreadManagerBase
{
private:
	friend class _CommSimpleSSL;
	CommSSLClient* cli;

public:
	CommSimpleThreadManagerSSL( CommRoutingTable& routingTable_ );
	void addClientTrustedCertificate( const BYTE* p, size_t sz );
	void clientRequireServerCertificateCN( const char* cn_ );
	void addClientTrustedCertificateEncrypted( const BYTE* p, UINT32 key );
	void clientRequireServerCertificateCNEncrypted( const char* cn, UINT32 key );
	const char * whoAreYou() const override
	{
		if (managerIdent.length())
			return managerIdent.c_str();
		else
			return "CommSimpleThreadManagerSSL";
	}
	void setWhoAreYou(const char * managerIdent_) override
	{
		managerIdent.assign("CommSimpleThreadManagerSSL:");
		managerIdent.append(managerIdent_);
		managerType.assign(managerIdent_);
	}

	~CommSimpleThreadManagerSSL();

public:
	bool handles( const char* address ) const override;
	void parseSocketAddr( PString& retAddr, int& port, const char* address ) override;
	void composeSocketAddr( PString& retAddr, const char* address, int port ) override;
	_CommSimpleThreadBase* _createNewThread( int threadIdx, _CommPhysicalConnection* physConn ) override;
};

#endif
