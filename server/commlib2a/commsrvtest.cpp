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

//#define SSL

class ServerObjectC;
class ClientConn : public CommClientGConnection
	{
	public:
	ServerObjectC* obj;

	public:
	ClientConn( CommClientGuardFactory& guardFactory )
	: CommClientGConnection( guardFactory )
		{
		obj = 0;
		}

	void connected() override;
	bool closed( int errCode, const char* errMsg ) override
		{
  		PTRACE( "connection closed: %d '%s'", errCode, errMsg );
		return CommClientGConnection::closed( errCode, errMsg ); 
		}
	void processGMessage( UINT32 requestId, UINT32 msgId, const CommMsgBody& body ) override
		{
		switch( msgId )
			{
			case 0x0002:
				{
				CommMsgParser parser( body );
				const char* s;
				parser.parseString( s );
				PTRACE( "'%s'", s );
				}
				break;
			}
		}
	};

class ClientSubscr : public CommClientSubscriber
	{
	public:
	void leafAppended( const Path& path, const CommMsgBody& body ) override
		{
		CommMsgParser parser( body );
		const char* s;
		UINT32 nClients;
		parser.parseString( s ).parseUINT32( nClients );
		PTRACE( "%s", s );
		PTRACE( "%d", nClients );
		}
	void leafUpdated( const Path& path, const CommMsgBody& body ) override
		{
		CommMsgParser parser( body );
		const char* s;
		UINT32 nClients;
		parser.parseString( s ).parseUINT32( nClients );
		PTRACE( "%s", s );
		PTRACE( "%d", nClients );
		}
	void nodeChanged( const Path& ) override
		{
		int path[] = { 0 };
		const CommMsgBody& l = leaf( path, 1 );
		CommMsgParser parser( l );
		const char* s;
		UINT32 nClients;
		parser.parseString( s ).parseUINT32( nClients );
		PTRACE( "%s", s );
		PTRACE( "%d", nClients );
		}
	};

class ServerObjectC : public CommServerObject
	{
	private:
	//static CommClientNullGuardFactory guardFactory;
	static CommClientPlainTextPasswordGuardFactory guardFactory;
	ClientConn conn;
	CommClientConnectionPool cliPool;
	ClientSubscr subscr;
	CommClientSubscriberPool cliSubscrPool;

	clock_t connStart;

	public:
	ServerObjectC( _CommInterface& inter )
	: CommServerObject( inter ),
	  cliPool( inter ),
	  cliSubscrPool( inter ),
	  conn( guardFactory )
		{
		conn.obj = this;
		connStart = clock();
		//cliPool.connect( conn.underlyingConnection(), "ip:192.168.1.177:2346", "serverSInstance" );
		cliPool.connect( conn.underlyingConnection(), "ip:localhost:2346", "serverSInstance" );
		cliSubscrPool.subscribe( subscr, "ip:localhost:2346", "serverSInstance", "DATA" );
		}
	void connected() override
		{
		PTRACE( "ConnTime: %d ticks", clock() - connStart );
		}

	public:
	void processMessage( _CommMsg& msg ) override
		{
		cliPool._processMessage( msg );
		cliSubscrPool._processMessage( msg );
		}
	};
///* static */ CommClientNullGuardFactory ServerObjectC::guardFactory;
/* static */ CommClientPlainTextPasswordGuardFactory ServerObjectC::guardFactory( "user", "user" );

/* virtual */ void ClientConn::connected()
	{
	obj->connected();
	CommMsgBody body;
	body.composeString( "Hello, server!" );
	post( 0x0001, body );
	}

class ServerObjectFactoryC : public CommServerObjectFactory
	{
	public:
	/* new */ CommServerObject* createServerObject( _CommInterface& inter ) const override
		{
		return new ServerObjectC( inter );
		}
	};

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
	void getUserAuth( const char* guardType, const char* user, CommMsgBody& clientExtra, CommMsgBody& request ) override //is called before connected()
		{
		PASSERT3( strcmp( guardType, CommServerPlainTextPasswordGuard::userAuthHandled() ) == 0 );
		const char* password = user;
		CommMsgParser parser( request );
		CommMsgBody reply;
		CommServerPlainTextPasswordGuard::processUserAuth( reply, parser, user, password );
		CommMsgParser replyParser( reply );
		gotUserAuth( &replyParser );
		}
	void qualityUpdated( int q ) override
		{
		PTRACE( "server quality: %d", q );
		}
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
		ret->monitorQuality();
		return ret;
		}
	};

class ServerObjectS : public CommServerObject
	{
	private:
	//static CommServerNullGuardFactory guardFactory;
	static CommServerPlainTextPasswordGuardFactory guardFactory;
	ServerConnFactory connFactory;
	CommServerConnectionPool srvPool;
	CommServerPublisherPool srvPublishPool;
	CommServerPublisher publish;

	int nClients;//business data

	public:
	ServerObjectS( _CommInterface& inter )
	: CommServerObject( inter ),
	  srvPool( inter ),
	  srvPublishPool( inter ),
	  connFactory( guardFactory )
		{
		connFactory.obj = this;
		srvPool.registerFactory( "", connFactory );

		nClients = 0;
		{
		CommMsgBody body;
		body.composeString( "Broadcast hello!" ).composeUINT32( nClients );
		publish.appendLeaf( 0, 0, body, 0 );
		}
		srvPublishPool.publish( "DATA", publish );
		}

	public:
	void processMessage( _CommMsg& msg ) override
		{
		srvPool._processMessage( msg );
		srvPublishPool._processMessage( msg );
		}

	public:
	void processMyMessage( ServerConn* conn, UINT32 msgId, const CommMsgBody& body )
		{
		switch( msgId )
			{
			case 0x0001:
				{
				CommMsgParser parser( body );
				UINT32 requestId;
				const char* s;
				parser.parseUINT32( requestId ).parseString( s );
				PTRACE( "'%s'", s );

				CommMsgBody body;
				body.composeUINT32( requestId ).composeString( "Hello, client!" );
				conn->post( 0x0002, body );

				++nClients;

				CommServerPublisher::UpdateMsg update;
				publish.startUpdateMsg( update );
				int path[] = { 0 };
				CommMsgBody body2;
				body2.composeString( "B-Hello!" ).composeUINT32( nClients );
				publish.updateLeaf( path, 1, body2, &update );
				publish.finishUpdateMsg( update );
				publish.postUpdate( update );
				}
				break;
			}
		}
	};
///* static */ CommServerNullGuardFactory ServerObjectS::guardFactory;
/* static */ CommServerPlainTextPasswordGuardFactory ServerObjectS::guardFactory;
///* static */ ServerConnFactory ServerObjectS::connFactory( ServerObjectS::guardFactory );

class ServerObjectFactoryS : public CommServerObjectFactory
	{
	public:
	/* new */ CommServerObject* createServerObject( _CommInterface& inter ) const override
		{
		return new ServerObjectS( inter );
		}
	};

void ServerConn::processMessage( UINT32 msgId, const CommMsgBody& body )
	{
	obj->processMyMessage( this, msgId, body );
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
	ServerObjectFactoryC factoryC;

	public:
	Daemon();
	void start( const char* dir, int nParms, char** parms ); override
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
#ifdef SSL
	CommSSL::startup();
#endif

#ifdef _MSC_VER
PLog( "_MSC_VER=%d, sizeof(size_t)=%d", _MSC_VER, sizeof(size_t) );
#endif
	{
	PLog( "Daemon parms:%d", nParms );
	for( int i=0; i < nParms ; ++i )
		PLog( "%s", parms[ i ] );
	}

	int flags = 0x00;//0x01 = client, 0x02 = server
	for( int i=0; i < nParms ; ++i )
		if( strcmp( parms[ i ], "-server" ) == 0 )
			flags |= 0x02;
		else if( strcmp( parms[ i ], "-client" ) == 0 )
			flags |= 0x01;
	if( flags == 0x00 )
		flags = 0x03;

	bool client = ( flags & 0x01 ) != 0;
	bool server = ( flags & 0x02 ) != 0;

	if( server )
		theRoutingTable.addAlias(  "ip:*:*" );
	if( client )
		{
	 	CommRule rule;
		rule.mask = "ip:*:*";
		rule.conns.push_back( "pyrssl0://$0:$1" );
		rule.conns.push_back( "pyrtcp0://$0:$1" );
		theRoutingTable.addRule( rule );
		}

	if( server )
		{
		rw.listenPort( 2346 );
		rw.listenPort( 443 );
		rw.setOption( "keepalive", "5000/60000" );
#ifdef SSL
		rw.setServerKeyCert( "newreq.pem", "newcert.pem", "password3" );
#endif
		}
	if( client )
		{
#ifdef SSL
		rw.addClientTrustedCertificate( (const BYTE*)trusted, strlen( trusted ) );
		rw.clientRequireServerCertificateCN( "ace.pyr" );
#endif
		}

	if( server )
		{
		local.registerServerObjectFactory( "serverS", factoryS );
		local.createServerObject( "serverS", "serverSInstance" );
		}

	if( client )
		{
		local.registerServerObjectFactory( "serverC", factoryC );
		local.createServerObject( "serverC", "serverCInstance" );
		}

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

#ifdef SSL
	CommSSL::cleanup();
#endif
	PSocket::cleanup();
	}

//*****************************************************************************

struct SerialNumber
        {
        DWORD dword1;
        DWORD dword2;
        };

BOOL GetSerialNumber(SerialNumber *serial, DWORD *features)
        {
        DWORD flags;
        DWORD dw1,dw2;
        _asm {
                xor eax,eax;
                cpuid;
                cmp eax,3;
                jl failure;
                mov eax,1;
                cpuid;
                mov flags,edx;
                mov eax,3;
                cpuid;
                mov dw2,edx;
                mov dw1,ecx;
                }
                *features=flags;
                serial->dword1=dw1;
                serial->dword2=dw2;
                        //test for feature
                BOOL b=flags&0x040000;
                return b;
        failure:
                return FALSE;
        }


#ifdef SSL
void _getFile()//DEBUG!
	{
	PSocket::startup();
	CommSSL::startup();
	const char* url = "http://192.168.1.201/bugzilla/index.html";

	CommHttpRequest rq;
	rq.setUrl( url );

	PIPSocket sock;
	sock.connect( rq.socketHost(), rq.socketPort() );
	PString rqStr;
	rq.createGetRequest( rqStr );
	sock.sendComplete( (const BYTE*)rqStr.c_str(), strlen( rqStr ) );

	BYTE buf[ 256 ];
	size_t offset;
	size_t sz;
	CommHttpResponse resp;
	for(;;)
		{
		int rd = sock.recv( buf, sizeof( buf ) );
		const BYTE* p = buf;
		bool Ok = resp.read( p, rd );
		if( Ok )
			{
			offset = p - buf;
			sz = rd;
			break;
			}
		}

	if( resp.getRespCode() != 200 )
		throw PInternalError( "HTTP Response != 200" );
	int len = -1;
	const char* cl = resp.getHeader( "Content-Length" );
	if( cl )
		len = atoi( cl );

	FILE* f = fopen( "url", "wb" );
	fwrite( buf + offset, 1, sz - offset, f );
	if( len >= 0 )
		len -= ( sz - offset );
	for(;;)
		{
		if( len == 0 )
			break;
		int rd;
		bool Ok = true;
		try
			{
			rd = sock.recv( buf, sizeof( buf ) );
			}
		catch( PError& err )
			{
			PTRACE( "error reading from socket: %s", err.why() );
			Ok = false;
			}
		if( !Ok )
			break;//forever
		fwrite( buf, 1, rd, f );
		if( len >= 0 )
			len -= rd;
		}
	fclose( f );

	PBlock pubKey;
	CommSSLSign::publicKeyFromCert( pubKey, "newcert.pem" );
	PString pubKeyStr;
	CommSSLBase64::encode( pubKeyStr, pubKey.ptr(), pubKey.size() );
	printf( "PUBKEY: '%s'", pubKeyStr.c_str() );
//"MEgCQQC3H63dQis0hJGQ6RwVCFMVvP0Fq9xayjqf1xAHqCBL+wHE4WYyP1TXewqD+CqGhMhCKwDB1IjjFoU1nENX2WSLAgMBAAE="

	PIniFile ini( "abc.ini" );
	PIniFile::Section* sec = ini.getSection( "sign" );

	CommSSLSign sign;
	sign.setPrivateKey( "newreq.pem", "password3" );
	for( int i=0; i < sec->items.size() ; ++i )
		{
		PString nvp;
		nvp.append( sec->items[ i ].name ).append( '=' ).append( sec->items[ i ].value ).append( '\n' );
		sign.addData( (const BYTE*)nvp.c_str(), strlen( nvp ) );
		}
	PBlock signature;
	sign.sign( signature );

	PString signatureStr;
	CommSSLBase64::encode( signatureStr, signature.ptr(), signature.size() );
	printf( "SIGN: '%s'", signatureStr.c_str() );

	CommSSLVerify verify;
	PBlock pubKeyC;
	CommSSLBase64::decode( pubKeyC, pubKeyStr );
	PBlock signatureC;
	CommSSLBase64::decode( signatureC, signatureStr );
	verify.setPublicKey( pubKeyC.ptr(), pubKeyC.size() );
//verify.addData( (const BYTE*)"1", 1 );
	for( int j=0; j < sec->items.size() ; ++j )
		{
		PString nvp;
		nvp.append( sec->items[ j ].name ).append( '=' ).append( sec->items[ j ].value ).append( '\n' );
		verify.addData( (const BYTE*)nvp.c_str(), strlen( nvp ) );
		}
	verify.verify( signatureC.ptr(), signatureC.size() );

	//EVP_ENCODE_CTX ctx;
	//EVP_DecodeInit( &ctx );
	//EVP_DecodeUpdate( &ctx, in, inl );
	//EVP_DecodeFinal( &ctx, out, outl );
	//EVP_PKEY *d2i_PublicKey(int type, EVP_PKEY **a, unsigned char **pp, long length);

	//const BYTE* sig;
	//size_t sigLen;
	//EVP_MD_CTX ctx;
	//EVP_PKEY *pubKey;
	//EVP_VerifyInit( &ctx, EVP_sha1() );
	//EVP_VerifyUpdate( &ctx, data, sz );
	//EVP_VerifyFinal( &ctx, sig, sigLen, pubKey );
	}
#endif

//#define WIN32SERVICENAME 
//#define LOGFILENAME "pyr.log"
int main( int argc, char** argv )
	{
//	CommSSL::startup();
//	CommSSL::recryptPrivateKey( "newreq4.pem", "password4", "newreq5.pem", "password5" );

	//_getFile();
		SerialNumber serial;
        DWORD features;
        BOOL b=GetSerialNumber(&serial,&features);
        printf("%04x-%04x-%04x-%04x\n\r",
                        HIWORD(serial.dword2),LOWORD(serial.dword2),
                        HIWORD(serial.dword1),LOWORD(serial.dword1)
                        );

//int flag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );
//flag |= _CRTDBG_CHECK_ALWAYS_DF;// | _CRTDBG_DELAY_FREE_MEM_DF;
//_CrtSetDbgFlag( flag );
	Daemon daemon;
	return PWin32ServiceDaemonMain( daemon, argc, argv, "pyr.log", "PYRTable" );
/*
	if( argc == 2 && strcmp( argv[ 1 ], "-install" ) == 0 )
		{
		PLogInit( new PLogFile( 0, PLogFile::alsoToStdout ) );
		PWin32Service::installService( WIN32SERVICENAME );
		return 0;
		}
	else if( argc == 2 && strcmp( argv[ 1 ], "-remove" ) == 0 )
		{
		PLogInit( new PLogFile( 0, PLogFile::alsoToStdout ) );
		PWin32Service::removeService( WIN32SERVICENAME );
		return 0;
		}
	else if( argc >= 2 && strcmp( argv[ 1 ], "-debug" ) == 0 )
		{
		PLogInit( new PLogFile( LOGFILENAME, PLogFile::alsoToStdout ) );

		Daemon daemon;
		PConsoleApplication app( daemon, argc - 2, argv + 2 );
		app.run();
		return 0;
		}
	else if( argc == 1 )
		{
		PString logFileName;
		PWin32Service::serviceDir( logFileName, argv[ 0 ] );
		logFileName.append( LOGFILENAME );

		PLogInit( new PLogFile( logFileName, PLogFile::alsoToStdout ) );

		Daemon daemon;
		PWin32Service svc( daemon, WIN32SERVICENAME );
		svc.run();
		return 0;
		}

	printf( "Usage: %s {-install|-remove|-debug}\n", argv[ 0 ] );
	return 1;
*/
	}

