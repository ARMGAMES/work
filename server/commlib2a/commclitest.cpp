#include "commmsgbody.h"
#include "commqueue.h"
#include "commtmrw.h"
#include "commtmlocal.h"
#include "commtmsimple.h"
#include "commcli.h"
#include "commgrdnull.h"
#include "commgrdppwd.h"
#include "commconcli.h"

//#define SSL

#include "commssl.h"

class ClientObject;
class ClientConn : public CommClientConnection
	{
	public:
	ClientObject* obj;

	public:
	ClientConn( CommClientGuardFactory& guardFactory )
	: CommClientConnection( guardFactory )
		{
		obj = 0;
		}

	void connected() override
		{
		CommMsgBody body;
		body.composeString( "Hello, server!" );
		post( 0x0001, body );
		}
	bool closed( int errCode, const char* errMsg ) override
		{
		PTRACE( "connection closed: %d '%s'", errCode, errMsg );
		return false;
		}
	void processMessage( UINT32 msgId, const CommMsgBody& body ) override
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

class ClientObject : public CommConsoleClientObject
	{
	private:
	static CommClientPlainTextPasswordGuardFactory guardFactory;
	ClientConn conn;
	CommClientConnectionPool cliPool;
	ClientSubscr subscr;
	CommClientSubscriberPool cliSubscrPool;

	public:
	ClientObject( CommConsoleClientInterface& inter )
	: CommConsoleClientObject( inter ),
	  cliPool( inter ),
	  cliSubscrPool( inter ),
	  conn( guardFactory )
		{
		guardFactory.setAuth( "User", "User" );

		conn.obj = this;
#ifdef SSL
		cliPool.connect( conn, /* "ip:192.168.1.101:443" */ "ip:10.30.0.37:443", "serverSInstance" );
		cliSubscrPool.subscribe( subscr, /*"ip:192.168.1.101:443"*/ "ip:10.30.0.37:443", "serverSInstance", "DATA" );
#else
		cliPool.connect( conn, /* "ip:192.168.1.101:443" */ "ip:10.30.0.37:2346", "serverSInstance" );
		cliSubscrPool.subscribe( subscr, /*"ip:192.168.1.101:443"*/ "ip:10.30.0.37:2346", "serverSInstance", "DATA" );
#endif
		}

	public:
	void processMessage( _CommMsg& msg ) override
		{
		cliPool._processMessage( msg );
		cliSubscrPool._processMessage( msg );
		}
	void processCommand( const char* cmd ) override
		{
		char buf[ 256 ];
		PASSERT( strlen( cmd ) + 1 < sizeof(buf) );
		strcpy( buf, cmd );

		char* tok = strtok( buf, " " );
		if( !tok )
			return;
		if( strcmp( tok, "send" ) == 0 )
			{
			char* tok2 = strtok( 0, " " );
			int n = atoi( tok2 );

			for( int i=0; i < n ; ++i )
				{
				CommMsgBody body;
				body.composeString( "Hello again!" );
				conn.post( 0x0001, body );
				}
			}
		else
			PLog( "Unknown command '%s'", tok );
		}
	}; 
/* static */ CommClientPlainTextPasswordGuardFactory ClientObject::guardFactory;

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


#ifndef PWINCE
int main()
#else
int wmain()
#endif
	{
	PLogInit( new PLogFile( "srv.log", PLogFile::alsoToStdout | PLogFile::printThreadId | PLogFile::perDateLimited ) );
	PLog( "Starting..." );
#ifdef _MSC_VER
	PLog( "_MSC_VER=%d", _MSC_VER );
#endif
	try
		{
		PSocket::startup();
#ifdef SSL
		CommSSL::startup();
#endif

		{
		CommRoutingTable theRoutingTable;
		CommRule rule;
		rule.mask = "ip:*:*";
#ifdef SSL
		rule.conns.push_back( "pyrssl0://$0:$1" );
#else
		rule.conns.push_back( "pyrtcp0://$0:$1" );
#endif
		theRoutingTable.addRule( rule );
		theRoutingTable.setOption( "maxfailures", "10" );

#ifdef SSL
		CommSimpleThreadManagerSSL simple( theRoutingTable );
		simple.addClientTrustedCertificate( (const BYTE*)trusted, strlen( trusted ) );
		simple.clientRequireServerCertificateCN( "ace.pyr" );
#else
		CommSimpleThreadManager simple( theRoutingTable );
#endif

		//simple.setOption( "httpsproxy", "192.168.1.201:80" );
		simple.setOption( "freezereconnect", "3000" );
		simple.setOption( "threadpriority", "+2" );

		CommConsoleClientThreadManager console( theRoutingTable );
		CommConsoleClientInterface& inter = console.createInterface();
		ClientObject obj( inter );
		console.registerObject( obj );

		theRoutingTable.start();

		for(;;)
			{
			char cmd[ 256 ];
			gets( cmd );
			int l = strlen( cmd );
			if( cmd[ l - 1 ] == '\n' )
				cmd[ l - 1 ] = 0;
			if( strcmp( cmd, "exit" ) == 0 )
				break;
			else
				console.pushCommand( cmd );
			}

		theRoutingTable.shutdown();
		bool Ok = theRoutingTable.waitUntilTerminated( 5 );
		if( Ok )
			PLog( "main: Threads terminated" );
		else
			PLog( "main: Threads not terminated - exiting anyway" );
		}

		PSocket::cleanup();
		}
	catch( PError& err )
		{
		PLog( "Error in main: '%s'\n", err.why() );
		}
	return 0;
	}

