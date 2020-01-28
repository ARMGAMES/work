#ifndef commexthttpthreadmanager_h_included
#define commexthttpthreadmanager_h_included

#include "ppincludebase.h"//_before_ openssl headers

#include <openssl/crypto.h>
//#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#include "ppinclude.h"
#include "ppstring.h"
#include "pplogfile.h"
#include "commlib.h"
#include "_commextthreadmanager.h"

class CommExtHttpClientInstance;
#if !defined( COMM_CLIENT_ONLY )
class CommExtHttpServerInstance;
#endif
class CommExtHttpInstance;

class CommExtHttpConnection : public _CommExtPhysConnection
{
public:
	PBlock  receivedData;
	int     sendState; // 0-initial, 1- ready to send, 2-sending done

	CommExtHttpConnection()
	{
		sendState = 0;

	}
	virtual ~CommExtHttpConnection()
	{}
	void disconnect()
	{
		mgr()->forceDisconnect(*this);
	}

	void send(PBlock & toSend)
	{
		sendState=1;
		CommMsgBody body;
		body.composePBlock(toSend);
		mgr()->push(this,body);
	}
	void send(const char * toSend, size_t size)
	{
		sendState=1;
		CommMsgBody body;
		body._composeVarBlock((const BYTE *)toSend, size);
		mgr()->push(this,body);
	}

	virtual void connected() {}
	virtual void disconnected(int  errCode, const char *msg){}
	virtual void sent() {}
	virtual void received(){}
	virtual bool receivedInThread(PBlock &toSend){return true;}
	virtual bool isInbound() = 0;
	virtual void parseHeader(CommMsgParser & parser) = 0;
};




class CommExtHttpInstance : public CommExtThreadManager
{
private:
	_CommInterface & baseInterface;
	void _safeProcessMessageFromThread(CommExtHttpConnection*conn, UINT32 msgId, CommMsgBody& body)
	{
		try
		{
			switch(msgId)
			{
			case 0:
				conn->connected();
				break;
			case 1:
				{
					INT32 errCode;
					const char * msg;
					CommMsgParser parser(body);
					parser.parseINT32(errCode)
						  .parseString(msg);
					conn->threadId = -1;
					conn->slot = -1;
					conn->channelId = 0;
					conn->disconnected(errCode,msg);
					if (incoming)
					{
						removePhysConn(conn->id());
						delete conn;
					}
				}
				break;
			case 4:
				{
					CommMsgParser parser(body);
					parser.parseStringP(conn->address)
						  .parseINT32(conn->threadId)
						  .parseINT32(conn->slot)
						  .parseUINT32(conn->channelId);
				}
				break;
			case 2:
				{
					CommMsgParser parser(body);
					conn->parseHeader(parser);
					parser.parsePBlock(conn->receivedData);
					conn->received();
				}
				break;
			case 3:
				conn->sendState=2;
				conn->sent();
				break;
			default:
				PLog( "Unknown msgId CommExtHttpConnection::_safeProcessMessageFromThread( %02X )", msgId);
				break;
			}
		}
		catch( PError& err )
		{
			PLog( "PError caught/ignored in CommExtHttpConnection::_safeProcessMessageFromThread( %02X ): %s", msgId, err.why() );
		}
		catch( const exception& exc )
		{
			PLog( "STD exception caught/ignored in CommExtHttpConnection::_safeProcessMessageFromThread( %02X ): %s", msgId, exc.what() );
		}
		catch( ... )
		{
			PLog( "... caught/ignored in CommExtHttpConnection::_safeProcessMessageFromThread( %02X )", msgId );
		}
	}

	virtual BYTE getMesageId() const
	{
		return incoming ? ( _COMM_MSGTYPE_INTERNAL_COMMUNICATION + 2 ) : ( _COMM_MSGTYPE_INTERNAL_COMMUNICATION + 3 );
	}

	void readDone(UINT32 connId,CommMsgBody &body) override
	{
		_CommMsg msg( getMesageId(), _COMM_MSGPRIORITY_INITCONNECT, body );
		msg.internal.patch0 = connId;
		msg.internal.patch1 = 2;
		baseInterface._internalPost(msg);
	}
	void sentDone(UINT32 connId) override
	{
		_CommMsg msg( getMesageId(), _COMM_MSGPRIORITY_INITCONNECT );
		msg.internal.patch0 = connId;
		msg.internal.patch1 = 3;
		baseInterface._internalPost(msg);
	}
	void connected(UINT32 connId) override
	{
		_CommMsg msg( getMesageId(), _COMM_MSGPRIORITY_INITCONNECT );
		msg.internal.patch0 = connId;
		msg.internal.patch1 = 0;
		baseInterface._internalPost(msg);
	}
	void disconnected(UINT32 connId, int errCode, const char * errMsg) override
	{
		_CommMsg msg( getMesageId(), _COMM_MSGPRIORITY_INITCONNECT );
		msg.internal.patch0 = connId;
		msg.internal.patch1 = 1;
		msg.body.composeINT32(errCode)
				.composeString(errMsg);
		baseInterface._internalPost(msg);
	}
	virtual void connecting(UINT32 connId, const char * address, UINT32 threadId, UINT32 slot, UINT32 channelId) override
	{
		_CommMsg msg( getMesageId(), _COMM_MSGPRIORITY_INITCONNECT );
		msg.internal.patch0 = connId;
		msg.internal.patch1 = 4;
		msg.body.composeString(address)
			.composeUINT32(threadId)
			.composeUINT32(slot)
			.composeUINT32(channelId);
		baseInterface._internalPost(msg);
	}

public:
	CommExtHttpInstance(_CommInterface & baseInterface_, bool incoming_) : CommExtThreadManager(incoming_),baseInterface(baseInterface_)
	{
	}
	~CommExtHttpInstance()
	{
	}
	CommExtHttpConnection * findHttpConnection(UINT32 connId)
	{
		return (CommExtHttpConnection*) findPhysConn(connId);
	}

	bool _processMessage( _CommMsg& msg )
	{
		if( msg.type != getMesageId() )
			return false;
		UINT32 id = msg.internal.patch0;
		CommExtHttpConnection * conn = findHttpConnection(id);
		if (conn == 0)
		{
			PLog("CommExtHttpInstance::_processMessage: message %d to connId %08X: connection not found",msg.internal.patch1,msg.internal.patch0);
			return true;
		}
		_safeProcessMessageFromThread(conn,msg.internal.patch1,msg.body);
		return true;
	}


};

template <class SSL_type>
struct CommSSLInterlock
{
	PInterlocked lock;
	SSL_type * ssl_;
	CommSSLInterlock():lock(1)
	{
		ssl_ = new SSL_type();
	}
	~CommSSLInterlock()
	{
		delete ssl_;
	}
	bool removeLock()
	{
		return lock.decrement();
	}
	void setLock()
	{
		lock.increment();
	}

};


class CommExtHttpClientConnection : public CommExtHttpConnection
{
public:
	CommHttpResponse header;
	CommSSLInterlock<CommSSLClient> * cli;

public:
	CommExtHttpClientConnection() 
	{
		cli = 0;
	}
	void connect(const char * address_)
	{
		mgr()->connect(this, address_, cli);
	}
	bool isInbound() override
	{
		return false;
	}
	~CommExtHttpClientConnection()
	{
		if (cli && cli->removeLock())
		{
			delete cli;
		}
	}
	void addClientTrustedCertificate(const BYTE* p, size_t sz)
	{
		if (cli == 0)
			cli = new CommSSLInterlock<CommSSLClient>();
		cli->ssl_->addTrustedCertificate(p, sz);
	}

	void clientRequireServerCertificateCN(const char* cn)
	{
		if (cli == 0)
			cli = new CommSSLInterlock<CommSSLClient>();
		cli->ssl_->requireServerCertificateCN(cn);
	}

	void addClientTrustedCertificateEncrypted(const BYTE* p, UINT32 key)
	{
		if (cli == 0)
			cli = new CommSSLInterlock<CommSSLClient>();
		cli->ssl_->addTrustedCertificateEncrypted(p, key);
	}

	void clientRequireServerCertificateCNEncrypted(const char* cn, UINT32 key)
	{
		if (cli == 0)
			cli = new CommSSLInterlock<CommSSLClient>();
		cli->ssl_->requireServerCertificateCNEncrypted(cn, key);
	}
	void setClientSniHostName(const char *hostName)
	{
		if (cli == 0)
			cli = new CommSSLInterlock<CommSSLClient>();
		cli->ssl_->setSniHostName(hostName);
	}
	void setServerCertificateCallback(void (*fun)(void *ssl))
	{
		if (cli == 0)
			cli = new CommSSLInterlock<CommSSLClient>();
		cli->ssl_->setCheckServerCertCallback(fun);
	}
	void parseHeader(CommMsgParser & parser) override
	{
		parser.parseObject<CommHttpResponse>(header);
	}


};




class CommExtHttpClientInstance : public CommExtHttpInstance
{
public:
	CommExtHttpClientInstance(_CommInterface & baseInterface_) : CommExtHttpInstance(baseInterface_,false)
	{
	}
	~CommExtHttpClientInstance()
	{
	}
	virtual UINT32 addHttpConnection(CommExtHttpClientConnection * conn)
	{
		PASSERT3(conn->id() == 0);
		return addPhysConn(conn);
	}
	_CommExtManagableThread* _createNewThread(int threadIdx) override;


};



#if !defined( COMM_CLIENT_ONLY )

class CommExtHttpServerConnection : public CommExtHttpConnection
{
public:
	CommHttpServerRequest header;

public:
	CommExtHttpServerConnection()
	{
	}
	bool isInbound() override
	{
		return true;
	}
	void parseHeader(CommMsgParser & parser) override
	{
		parser.parseObject<CommHttpServerRequest>(header);
	}
	friend class CommExtHttpServerInstance;
};


class CommExtHttpServerConnectionFactory
{
public:
	CommExtHttpServerConnectionFactory()
	{}
	virtual ~CommExtHttpServerConnectionFactory()
	{}
	virtual CommExtHttpServerConnection * createConnection() = 0;
};

class CommExtHttpServerInstance : public CommExtHttpInstance
{
public:
	CommExtHttpServerConnectionFactory &factory;
public:
	CommExtHttpServerInstance(_CommInterface & baseInterface_,CommExtHttpServerConnectionFactory &factory_) : CommExtHttpInstance(baseInterface_,true),factory(factory_)
	{
	}
	~CommExtHttpServerInstance()
	{
	}
	_CommExtManagableThread* _createNewThread(int threadIdx) override;
	_CommExtPhysConnection* _createConnection() override
	{
		return factory.createConnection();
	}

};

#endif	// !defined( COMM_CLIENT_ONLY )

class CommExtHttpsClientInstance : public CommExtHttpClientInstance
{
	BYTE getMesageId() const override
	{
		return incoming ? ( _COMM_MSGTYPE_INTERNAL_COMMUNICATION + 4 ) : ( _COMM_MSGTYPE_INTERNAL_COMMUNICATION + 5 );
	}

public:
	CommExtHttpsClientInstance(_CommInterface & baseInterface_) : CommExtHttpClientInstance(baseInterface_)
	{
	}
	~CommExtHttpsClientInstance()
	{
	}
	UINT32 addHttpConnection(CommExtHttpClientConnection * conn) override
	{
		PASSERT3(conn->id() == 0);
		if (!conn->cli)
			conn->cli = new CommSSLInterlock<CommSSLClient>();
		return addPhysConn(conn);
	}
	UINT32 addHttpsConnection(CommExtHttpClientConnection * conn)
	{
		return addHttpConnection(conn);
	}

	_CommExtManagableThread* _createNewThread(int threadIdx) override;
};

#if !defined( COMM_CLIENT_ONLY )

class CommExtHttpsServerInstance : public CommExtHttpServerInstance
{
public:
	CommSSLInterlock<CommSSLServer> * srv;
	CommExtHttpsServerInstance(_CommInterface & baseInterface_,CommExtHttpServerConnectionFactory &factory_) : CommExtHttpServerInstance(baseInterface_,factory_)
	{
		srv = new CommSSLInterlock<CommSSLServer>();
	}
	~CommExtHttpsServerInstance()
	{
		if (srv && srv->removeLock())
		{
			delete srv;
		}

	}
	void setServerKeyCert(const char* keyFile, const char* certFile, const char* password, const char * sniName)
	{
		if (srv == 0)
			srv = new CommSSLInterlock<CommSSLServer>();
		srv->ssl_->setKeyCert(keyFile, certFile, password, sniName);
	}

	void setServerKeyCertFromMemory(const BYTE* keyFileContent,
		size_t keyFileContentSize,
		const BYTE* certFileContent,
		size_t certFileContentSize,
		const char* password, const char * sniName)
	{
		if (srv == 0)
			srv = new CommSSLInterlock<CommSSLServer>();
		srv->ssl_->setKeyCertFromMemory(keyFileContent, keyFileContentSize, certFileContent, certFileContentSize, password, sniName);
	}
	void setServerKeyCert(PBlock & keyFile_, PBlock & certFile_, const char* password_, const char * sniHost)
	{
		if (srv == 0)
			srv = new CommSSLInterlock<CommSSLServer>();
		srv->ssl_->setKeyCertFromMemory(keyFile_.ptr(), keyFile_.size(), certFile_.ptr(), certFile_.size(), password_, sniHost);
	}
	_CommExtManagableThread* _createNewThread(int threadIdx) override;
};

#endif	// !defined( COMM_CLIENT_ONLY )




#endif
