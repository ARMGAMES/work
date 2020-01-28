#ifndef commtmrw_h_included
#define commtmrw_h_included


#include "ppinclude.h"

#include "ppthread.h"
#include "ppsocket.h"
#include "commmsgbody.h"
#include "commqueue.h"
#include "commrtable.h"
#include "commssl.h"

class _CommRWAcceptThread : public PThread
{
private:
	//	bool exited;

public:
	_CommRWAcceptThread()
	{
	}
	virtual void shutdown()=0;
	virtual void threadState(CommMsgBody &body)=0;
};

class _CommRWConnectThread : public PThread
{
public:
	typedef PQueue< PQDeque<_CommPhysicalConnection* > > Queue;

protected:
	//	bool exited;
	Queue q;

public:
	_CommRWConnectThread()
	{
	}
	void pushConnection( _CommPhysicalConnection* conn )
	{
		q.push( conn );
	}

public:
	void shutdown()
	{
		q.shutdown();
	}
private:
};

class CommRWThreadManagerBaseIP;
class _CommRWAcceptThreadIP;

class _CommRWConnectThreadIP : public _CommRWConnectThread
{
private:
	CommRWThreadManagerBaseIP& mgr;

public:
	_CommRWConnectThreadIP( CommRWThreadManagerBaseIP& mgr_ );
public:
	void run() override;
private:
	void _connect( _CommPhysicalConnection* conn );
};


#ifdef PPOSIX
class _CommRWAcceptThreadUnix;
class _CommRWConnectThreadUnix;
#endif

class _CommRWThreadBase;


#ifdef PPOSIX
class CommRWThreadManagerUnix;
#endif








/* abstract */ class CommRWThreadManagerBase : public CommThreadManager
{
public:
	struct Socket
	{
		PBaseSocket::Descriptor sockDescr;
		_CommPhysicalConnection* physConn;
		static void composeSockDescriptor(PBaseSocket::Descriptor & sockDescr, CommMsgBody & body)
		{
			if (sizeof(sockDescr.sock) > sizeof(UINT32))
			{
				PASSERT3(((UINT64)sockDescr.sock)>>32 == 0);
			}
			body.composeUINT32((UINT32)sockDescr.sock);
			body._composeFixedBlock((const BYTE *)&sockDescr.addr, sizeof(sockDescr.addr));
		}
		static void parseSockDescriptor(PBaseSocket::Descriptor & sockDescr, CommMsgParser & parser)
		{
			UINT32 s;
			parser.parseUINT32(s);
			sockDescr.sock = (SOCKET) s;
			const BYTE *descr;
			parser._parseFixedBlock(descr,sizeof(sockDescr.addr));
			memcpy((void *)&sockDescr.addr, descr,sizeof(sockDescr.addr));
		}
	};

public:



private:
	typedef CommThreadManager _Parent;

protected:
	bool muxEnabled;

public:
	bool   isFar;
	int    keepAliveWrite;
	int    keepAliveRead;
	bool   keepAliveIgnoreDataPackets;
	int    listenBacklog;
	bool   outTcpNoDelay;
	bool   inTcpNoDelay;
	bool   optimizePacketSize;
	int    inTcpBufferRcv;
	int    inTcpBufferSnd;
	int    outTcpBufferRcv;
	int    outTcpBufferSnd;
	int    MaxBlockSize;
	UINT32 MaxBlockSizeVer5;
	UINT32 CompressBitsSize;
	UINT32 DefaultProtocol;
#ifndef COMM_CLIENT_ONLY
	UINT32 minimumProtocol;
	UINT32 publicKey[5];
	UINT32 privateKey[5];
	bool   enableServerEncryption;
	UINT32 minEnforcedBlock;
	UINT32 maxEnforcedBlock;
	UINT32 randomProtectionEnforced;
	UINT32 maximumMessageSize;
	UINT32 maximumMessageRate;
	UINT32 maximumMessageRatePeriod;
	bool   maxMessageSizeDebug; 
	bool   maxMessageRateDebug; 
	
#endif
	bool   enableClientEncryption;
	_CommProtocolLog * comm_logging;
	int maxNumberOfConnections;
	UINT32 tcpConnectTimeout;
	UINT32 handshakeTimeout;

protected:
	/* new */ _CommRWConnectThread* connThread;
	vector< /* new */ _CommRWAcceptThread* > acceptThreads;
	bool started;
	PString acceptThreadPriority;
	PString connectThreadPriority;


public:

	CommRWThreadManagerBase( CommRoutingTable& routingTable_, bool enableMux );
	void setOption( const char* optName, const char* optValue ) override;
	void start() override;
	void shutdown() override;
	bool terminated() override;
	/* virtual */ ~CommRWThreadManagerBase();

public:
	void connect( _CommPhysicalConnection* physConn ) override;
	void forceDisconnect( _CommPhysicalConnection* physConn ) override;
	const char * whoAreYou() const override
	{
		return "CommRWThreadManagerBase";
	} 
	void loggerInformation(CommMsgBody &body) override; 
	void dynamicInit(PIniFile& iniFile) override;

	// publish for thin client
	void pushInSocket( const PBaseSocket::Descriptor& sockDescr, const char* addr )
	{
		_pushInSocket(sockDescr, addr);
	}

private:
	virtual _CommRWThreadBase* _createNewThread( int threadIdx ) = 0;


protected:

	void _pushInSocket( const PBaseSocket::Descriptor& sockDescr, const char* addr );
	bool _allowNewConnection();
	int  _numberOfConnections();
	void _pushOutSocket( const Socket& sock )
	{
		_pushSocket( sock );
		//no need to notify routingTable - will be notified by the RWThread after socket receipt
	}

	bool _freeSocket( int threadIdx );//to be called from RWThread
	//returns true if thread should be terminated
	//no thread operations allowed after freeSocket returned true//no thread operations allowed after freeSocket returned true
private:
	/* inline */ static _CommRWThreadBase* _rwThread( Thread& thread );
	void _pushSocket( const Socket& sock );

	friend class _CommRWThreadBase;
	friend class _CommRWAcceptThreadIP;
	friend class _CommRWConnectThreadIP;
#ifdef UDT_DEVELOPMENT
	friend class _CommRWAcceptThreadUDT;
	friend class _CommRWConnectThreadUDT;
#endif
#ifdef PPOSIX
	friend class _CommRWAcceptThreadUnix;
	friend class _CommRWConnectThreadUnix;
#endif
	friend class CommRWThreadManagerBaseIP;
#ifdef PPOSIX
	friend class CommRWThreadManagerUnix;
#endif
};

#ifdef PPOSIX
class CommRWThreadManagerUnix : public CommRWThreadManagerBase
{
private:
	vector<PString> listenPorts;
public:
	CommRWThreadManagerUnix( CommRoutingTable& routingTable_, bool enableMux = false);
	//	void setOption( const char* optName, const char* optValue ) override;

	/* virtual */ ~CommRWThreadManagerUnix(){}
public:
	void listenName( const char * port ) override;
	const char * whoAreYou() const override
	{
		return "CommRWThreadManagerUnix";
	} 
	//	virtual void loggerInformation(CommMsgBody &body); 
	//	virtual void dynamicInit(PIniFile& iniFile);
	void acceptCancel() override;
	bool acceptReinstate() override;
	bool handles( const char* address ) const override;
	void parseSocketAddr( PString& retAddr, int& port, const char* address ) override;
	void composeSocketAddr( PString& retAddr, const char* address, int port  ) override;

private:
	_CommRWThreadBase* _createNewThread( int threadIdx ) override;

	friend class _CommRWAcceptThreadUnix;
	friend class _CommRWConnectThreadUnix;

};
#endif

class CommRWThreadManagerBaseIP : public CommRWThreadManagerBase
{
protected:
	vector<int> listenPorts;
public:
	CommRWThreadManagerBaseIP( CommRoutingTable& routingTable_, bool enableMux );
	//	void setOption( const char* optName, const char* optValue ) override;

	/* virtual */ ~CommRWThreadManagerBaseIP(){}

public:
	void listenPort( int port ) override;
	const char * whoAreYou() const override
	{
		return "CommRWThreadManagerBaseIP";
	} 
	//	virtual void loggerInformation(CommMsgBody &body); 
	//	virtual void dynamicInit(PIniFile& iniFile);
	virtual bool connectIsOutOfProcess() { return false;}
	void acceptCancel() override;
	bool acceptReinstate() override;

private:
	_CommRWThreadBase* _createNewThread( int threadIdx ) override = 0;

	friend class _CommRWAcceptThreadIP;
	friend class _CommRWConnectThreadIP;

};



class CommRWThreadManager : public CommRWThreadManagerBaseIP
{
public:
	CommRWThreadManager( CommRoutingTable& routingTable_, bool enableMux = false );
	const char * whoAreYou() const override
	{
		if (managerIdent.length())
			return managerIdent.c_str();
		else
			return "CommRWThreadManager";
	}

	void setWhoAreYou(const char * managerIdent_) override
	{
		managerIdent.assign("CommRWThreadManager:");
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

class CommRWThreadManagerBaseSSL 
{
public:
	CommRWThreadManagerBaseSSL()
	{
	}
	virtual ~CommRWThreadManagerBaseSSL()
	{}
	virtual void setServerKeyCert( PBlock & keyFile_, PBlock & certFile_, const char* password_, const char * sniHost = 0 ) = 0;


};

class CommRWThreadManagerBaseServerSSL : public CommRWThreadManagerBaseSSL
{
public:
	CommSSLServer* srv;

	CommRWThreadManagerBaseServerSSL();
	~CommRWThreadManagerBaseServerSSL();

public:
	void setServerKeyCert( const char* keyFile, const char* certFile, const char* password, const char * sniName = 0 );
	void setServerKeyCertFromMemory( const BYTE* keyFileContent, 
		size_t keyFileContentSize,
		const BYTE* certFileContent,
		size_t certFileContentSize,
		const char* password, const char * sniName = 0  );
	void setServerKeyCert( PBlock & keyFile_, PBlock & certFile_, const char* password_, const char * sniHost = 0 ) override;
};



class CommRWThreadManagerBaseClientSSL 
{
public:
	CommSSLClient* cli;

	CommRWThreadManagerBaseClientSSL();
	~CommRWThreadManagerBaseClientSSL();

public:
	void addClientTrustedCertificate( const BYTE* p, size_t sz );
	void clientRequireServerCertificateCN( const char* cn_ );
	void addClientTrustedCertificateEncrypted( const BYTE* p, UINT32 key );
	void clientRequireServerCertificateCNEncrypted( const char* cn, UINT32 key );
	void setClientSniHostName(const char *hostName);

};

class CommRWThreadManagerSSL : public CommRWThreadManagerBaseIP, public CommRWThreadManagerBaseServerSSL, public CommRWThreadManagerBaseClientSSL
{
public:

	CommRWThreadManagerSSL( CommRoutingTable& routingTable_ );
	~CommRWThreadManagerSSL();

public:
	const char * whoAreYou() const override 
	{
		if (managerIdent.length())
			return managerIdent.c_str();
		else
			return "CommRWThreadManagerSSL";
	}

	void setWhoAreYou(const char * managerIdent_) override
	{
		managerIdent.assign("CommRWThreadManagerSSL:");
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


/* abstract */ class CommRWThreadManagerProcess : public CommRWThreadManagerBaseIP,public CommRWThreadManagerBaseSSL
{
public:
	PString commLoggingName;
	struct CertFiles
	{
		PBlock  keyFile;
		PBlock  certFile;
		PString password;
	};
	CertFiles certFileDefault;
	PStringMap<CertFiles> certFiles;

	PString dirName;
	PString logFileDirName;
	PString communicatorProcessName;

public:
	CommRWThreadManagerProcess( CommRoutingTable& routingTable_ );
	void setOption( const char* optName, const char* optValue ) override;

public:
	const char * whoAreYou() const override
	{
		if (managerIdent.length())
			return managerIdent.c_str();
		else
			return "CommRWThreadManagerProcess";
	}

	void setWhoAreYou(const char * managerIdent_) override
	{
		managerIdent.assign("CommRWThreadManagerProcess:");
		managerIdent.append(managerIdent_);
		managerType.assign(managerIdent_);
	}
	void setServerKeyCert( PBlock & keyFile_, PBlock & certFile_, const char* password_, const char * sniHost = 0 ) override
	{
		if (!sniHost)
		{
			certFileDefault.keyFile.moveFrom(keyFile_);
			certFileDefault.certFile.moveFrom(certFile_);
			certFileDefault.password.assign(password_);
		}
		else
		{
			PStringMap<CertFiles>::iterator it = certFiles.insert(sniHost,CertFiles()).first;
			(*it).second.keyFile.moveFrom(keyFile_);
			(*it).second.certFile.moveFrom(certFile_);
			(*it).second.password.assign(password_);
		}
	}
	void setDir( const char * dirName_)
	{
		dirName.assign(dirName_);
	}
	void setLogFileDir( const char * dirName_)
	{
		logFileDirName.assign(dirName_);
	}
	bool handles( const char* address ) const override;
	void parseSocketAddr( PString& retAddr, int& port, const char* address ) override;
	void composeSocketAddr( PString& retAddr, const char* address, int port ) override;
	void loggerInformation(CommMsgBody &body) override;

private:
	_CommRWThreadBase* _createNewThread( int threadIdx ) override;

};



/* abstract */ class CommRWThreadManagerProcessClient : public CommRWThreadManagerBaseIP
{
public:
	PString commLoggingName;
	struct Cert
	{
		bool encrypted;
		PBlock  certFile;
		UINT32  key;
	};
	struct CN
	{
		bool encrypted;
		PString cn;
		UINT32  key;
	};
	vector <Cert *> certs;
	vector <CN *> cns;
	PString sniHostName;




	PString dirName;
	PString logFileDirName;
	PString	httpsProxy;
	int httpsProxyPort;
	bool tcpNoDelay;
	int sslConnectTimeout;
	int	tcpBuffer;
	PString communicatorProcessName;


public:
	CommRWThreadManagerProcessClient( CommRoutingTable& routingTable_ );

	~CommRWThreadManagerProcessClient()
	{
		for( auto&& cert : certs )
			delete cert;

		for( auto&& cn : cns )
			delete cn;
	}

	void setOption( const char* optName, const char* optValue ) override;

public:
	bool connectIsOutOfProcess() override { return true; }
	const char * whoAreYou() const override
	{
		if (managerIdent.length())
			return managerIdent.c_str();
		else
			return "CommRWThreadManagerProcessClient";
	}

	void setWhoAreYou(const char * managerIdent_) override
	{
		managerIdent.assign("CommRWThreadManagerProcessClient:");
		managerIdent.append(managerIdent_);
		managerType.assign(managerIdent_);
	}
	void setDir( const char * dirName_)
	{
		dirName.assign(dirName_);
	}
	void setLogFileDir( const char * dirName_)
	{
		logFileDirName.assign(dirName_);
	}
	void addClientTrustedCertificate( const BYTE* p, size_t sz )
	{
		Cert * s = *certs.insert(certs.end(), new Cert());
		memcpy(s->certFile.append(sz),p,sz);
		s->encrypted = false;
		s->key = 0;
	}
	void clientRequireServerCertificateCN( const char* cn_ )
	{
		CN * cn = * cns.insert(cns.end(),new CN());
		cn->cn.assign(cn_);
		cn->encrypted = false;
		cn->key = 0;
	}
	void addClientTrustedCertificateEncrypted( const BYTE* p, UINT32 key )
	{
		int size = CommMsgBody::readUINT32(p);
		if (size <= 0)
			throw PInternalError( "_CommSSLClient: invalid certificate" );

		Cert * s = *certs.insert(certs.end(),new Cert());
		memcpy(s->certFile.append(size + sizeof(UINT32)),p,size + sizeof(UINT32));
		s->encrypted = true;
		s->key = key;
	}
	void clientRequireServerCertificateCNEncrypted( const char* cn_, UINT32 key )
	{
		CN * cn = *cns.insert(cns.end(),new CN());
		cn->cn.assign(cn_);
		cn->encrypted = true;
		cn->key = key;
	}
	void setClientSniHostName(const char *hostName )
	{
		sniHostName = hostName;
	}
	bool handles( const char* address ) const override;
	void parseSocketAddr( PString& retAddr, int& port, const char* address ) override;
	void composeSocketAddr( PString& retAddr, const char* address, int port ) override;

private:
	_CommRWThreadBase* _createNewThread( int threadIdx ) override;

};


/* abstract */ class CommRWThreadManagerMultiProcess : public CommRWThreadManagerProcess
{
public:
public:
	CommRWThreadManagerMultiProcess( CommRoutingTable& routingTable_ );
	void setOption( const char* optName, const char* optValue ) override;

public:
	const char * whoAreYou() const override
	{
		if (managerIdent.length())
			return managerIdent.c_str();
		else
			return "CommRWThreadManagerMultiProcess";
	}

	void setWhoAreYou(const char * managerIdent_) override
	{
		managerIdent.assign("CommRWThreadManagerMultiProcess:");
		managerIdent.append(managerIdent_);
		managerType.assign(managerIdent_);
	}

private:
	_CommRWThreadBase* _createNewThread( int threadIdx ) override;

};





#endif
