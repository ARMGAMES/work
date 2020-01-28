//#include "commmsgbody.h"
//#include "commqueue.h"
//#include "commtmrw.h"
//#include "commtmlocal.h"
//#include "commtmsimple.h"
//#include "commcli.h"
//#include "commgrdnull.h"
//#include "ppdaemon.h"
#include "pplib.h"
#include "commlib.h"
#include "commssl.h"
#include "commhttp.h"
#include "commgrdrsa.h"
#include "commgrdaesencrypted.h"
#include "commgrdtoken.h"


//#define SSL



// auth request message wrapper


class ClientConn : public CommClientGConnection
{
	public:
	UINT32 counter;

	public:
	ClientConn( CommClientGuardFactory& guardFactory )
	: CommClientGConnection( guardFactory )
	{
		counter=0;
	}

	void connected() override;
	bool closedOrDisconnected( int errCode, const char* errMsg ) override
	{
  		PTRACE( "connection closed: %d '%s'", errCode, errMsg );
		return false;
	}
	void processGMessage( UINT32 requestId, UINT32 msgId, const CommMsgBody& body, AsyncCall* call  ) override
	{
  		PTRACE( "processGMessage: %d ", msgId);
		if (call)
			call->processReply(requestId,msgId,body);

	}
};


/* virtual */ void ClientConn::connected()
{
}

//*****************************************************************************
//*****************************************************************************

class ServerObjectS;
class ServerConn : public CommServerConnection
{
	private:
	ServerObjectS* obj;

	public:
	ServerConn( ServerObjectS* obj_ )
	{
		obj = obj_;
	}
	void processMessage( UINT32 msgId, const CommMsgBody& body ) override;
	void getUserAuth( const char* guardType, const char* user, CommMsgBody& clientExtra, CommMsgBody& request ) override;
	void closedOrDisconnected( int errCode, const char* errMsg ) override
	{
  		PTRACE( "connection closed: %d '%s'", errCode, errMsg );
	}
};

class Acall: public CommClientGConnection::AsyncCall
{
public:
	ServerObjectS* obj;	
	UINT32 connId;	

	Acall( ServerObjectS* obj_, UINT32 _connId)
	{
		obj = obj_;
		connId= _connId;
	}
	void processReply( UINT32 requestId, UINT32 msgId, const CommMsgBody& body ); 
};


class ServerConnFactory : public CommServerConnectionFactory
{
	public:
	ServerObjectS* obj;

	public:
	ServerConnFactory( CommServerGuardFactory& guardFactory )
	: CommServerConnectionFactory( guardFactory )
	{
		obj = 0;
	}

	public:
	/* new */ CommServerConnection* createConnection() override
	{
		CommServerConnection* ret = new ServerConn( obj );
		return ret;
	}
};

class ServerObjectS : public CommServerObject
{
	private:
	//static CommServerNullGuardFactory guardFactory;
	static CommClientNullGuardFactory guardFactoryC;
	static CommServerTokenGuardFactory guardFactoryT;
	static CommServerAesEncryptedGuardFactory guardFactoryE;

	ServerConnFactory connFactory;
	CommServerConnectionPool srvPool;

	ClientConn conn;
	CommClientConnectionPool cliPool;



	public:
	ServerObjectS( _CommInterface& inter )
	: CommServerObject( inter ),
	  srvPool( inter ),
	  cliPool( inter ),
	  connFactory( guardFactoryT ),
	  conn(guardFactoryC)

	{
		connFactory.obj = this;
		connFactory.addGuardFactory(guardFactoryE);
		srvPool.registerFactory( "", connFactory );
		cliPool.connect( conn.underlyingConnection(), "ip:10.30.0.105:4320", "AuthServerInstance","authCheck" );
	}
	

	public:
	void processMessage( _CommMsg& msg ) override
	{
		srvPool._processMessage( msg );
		cliPool._processMessage( msg );
	}

	public:
	void processMyMessage( ServerConn* conn, UINT32 msgId, const CommMsgBody& body )
	{
	}
	void processAsyncReply( UINT32 connId, const CommMsgBody& body )
	{
		CommServerConnection* conn = srvPool.findConnection(connId);
		CommMsgParser parser(body);
		INT16 errCode;
		parser.parseINT16(errCode);
		if (errCode)
		{
			const char * errMessage;
			parser.parseString(errMessage);
			PTRACE3("getAuth - errorCode=%d errMessage='%s'",errCode,errMessage);
			if (conn)
				conn->gotUserAuth(0,2,errMessage);
		}
		else
		{
			CommMsgBody authBody;
			parser.parseMsgBody(authBody);
			if (conn)
			{
				CommMsgParser authParser(authBody);
				if (conn->gotUserAuth(&authParser))
				{
					PTRACE3("getAuth - auth granted");
				}
				else
				{
					PTRACE3("getAuth denied");		
				}
			}
		}
	}

	void getUserAuth( const char* guardType, const char* user, CommMsgBody& clientExtra, CommMsgBody& request, UINT32 connId )
	{

		CommMsgBody body;
		body.composeString(user);
		body.composeString(guardType);
		body.composeMsgBody(request);
		body.composeMsgBody(clientExtra);
		conn.post(1,body,new Acall(this,connId));
	}
};
///* static */ CommServerNullGuardFactory ServerObjectS::guardFactory;
CommClientNullGuardFactory ServerObjectS::guardFactoryC;
/* static */ CommServerTokenGuardFactory ServerObjectS::guardFactoryT;
/* static */ CommServerAesEncryptedGuardFactory ServerObjectS::guardFactoryE;
///* static */ ServerConnFactory ServerObjectS::connFactory( ServerObjectS::guardFactory );

class ServerObjectFactoryS : public CommServerObjectFactory
{
	public:
	/* new */ CommServerObject* createServerObject( _CommInterface& inter ) const override
	{
		return new ServerObjectS( inter );
	}
};

void Acall::processReply( UINT32 requestId, UINT32 msgId, const CommMsgBody& body ) 
{
	obj->processAsyncReply(connId,body);
}

void ServerConn::processMessage( UINT32 msgId, const CommMsgBody& body )
{
	obj->processMyMessage( this, msgId, body );
}

void ServerConn::getUserAuth( const char* guardType, const char* user, CommMsgBody& clientExtra, CommMsgBody& request )//is called before connected()
{
	obj->getUserAuth( guardType, user, clientExtra, request, id() );
}


//****************************************************************************




















class Daemon : public PDaemon
{
	private:
	CommRoutingTable theRoutingTable;
#ifdef SSL
	CommRWThreadManagerSSL rw;
#else
	CommRWThreadManager rw;
#endif
	CommLocalThreadManager local;
	ServerObjectFactoryS factoryS;

	public:
	Daemon();
	void start( const char* dir, int nParms, char** parms ) override;
	void stop() override;
};

Daemon::Daemon()
: rw( theRoutingTable ),
  local( theRoutingTable )
{
}

static const char* trusted =
"-----BEGIN CERTIFICATE-----\n"
"MIIDGDCCAoGgAwIBAgIBADANBgkqhkiG9w0BAQQFADBsMQswCQYDVQQGEwJDQTEQ\n"
"MA4GA1UECBMHT250YXJpbzEWMBQGA1UEBxMNUmljaG1vbmQgSGlsbDEZMBcGA1UE\n"
"ChMQUFlSIFNvZnR3YXJlIEx0ZDEYMBYGA1UEAxMPcHlyc29mdHdhcmUuY29tMB4X\n"
"DTAxMDQxOTE4MTgzOFoXDTAyMDQxOTE4MTgzOFowbDELMAkGA1UEBhMCQ0ExEDAO\n"
"BgNVBAgTB09udGFyaW8xFjAUBgNVBAcTDVJpY2htb25kIEhpbGwxGTAXBgNVBAoT\n"
"EFBZUiBTb2Z0d2FyZSBMdGQxGDAWBgNVBAMTD3B5cnNvZnR3YXJlLmNvbTCBnzAN\n"
"BgkqhkiG9w0BAQEFAAOBjQAwgYkCgYEAt9544/OgQOVN4aMdD0LhzLVRef2aJm7s\n"
"cFzwuqzYZBw+5wGZ5PkX/J3KNZol819VyH3pUDwBRkd73OaQ0AxS8ABZveOZZ5E4\n"
"8hLT3S2zS96WZWAQhup6UNXTqCY9lYLUOtQqZjRIEEPdX/oAchVbo+xr6bxhjkmo\n"
"FKhwQ19ExK8CAwEAAaOByTCBxjAdBgNVHQ4EFgQUpOpfGqn/ui6cv6fLeAMx6tuP\n"
"y5EwgZYGA1UdIwSBjjCBi4AUpOpfGqn/ui6cv6fLeAMx6tuPy5GhcKRuMGwxCzAJ\n"
"BgNVBAYTAkNBMRAwDgYDVQQIEwdPbnRhcmlvMRYwFAYDVQQHEw1SaWNobW9uZCBI\n"
"aWxsMRkwFwYDVQQKExBQWVIgU29mdHdhcmUgTHRkMRgwFgYDVQQDEw9weXJzb2Z0\n"
"d2FyZS5jb22CAQAwDAYDVR0TBAUwAwEB/zANBgkqhkiG9w0BAQQFAAOBgQByYj1K\n"
"AX+C8qigwORX41YCLvO9PCTwRW8B9kows8TjOrWpcyXh2zcuT0zLdrNiVPKwK6qP\n"
"xYmJpuLMvcwJzON9HKCKnqVlhFPUtSzI2qZsxNkHlgBQ8TLAXv4TvSD4NO+rs/lK\n"
"Dl0x5YNAHpwSBBbETHWF+t/1kZ8ImrNYkIWajg==\n"
"-----END CERTIFICATE-----\n";

void Daemon::start( const char* dir, int nParms, char** parms )
{
	PSocket::startup();
	CommSSL::startup();

	PLog( "Daemon parms:%d", nParms );

	theRoutingTable.addAlias(  "ip:*:2346" );
 	CommRule rule;
	rule.mask = "ip:*:*";
	rule.conns.push_back( "pyrtcp0://$0:$1" );
	theRoutingTable.addRule( rule );

	rw.listenPort( 2346 );
	local.registerServerObjectFactory( "serverS", factoryS );
	local.createServerObject( "serverS", "serverSInstance" );
	theRoutingTable.start();
}

void Daemon::stop()
{
	theRoutingTable.shutdown();
	bool Ok = theRoutingTable.waitUntilTerminated( 5 );
	if( Ok )
		PLog( "Daemon::stop(): Threads terminated" );
	else
		PLog( "Daemon::stop(): Threads not terminated - exiting anyway" );

	CommSSL::cleanup();
	PSocket::cleanup();
}

int main( int argc, char** argv )
{
	Daemon daemon;
	return PWin32ServiceDaemonMain( daemon, argc, argv, "pyr.log", "PYRTable" );
}

//*****************************************************************************

