#include "commlocalapp.h"
#include "commgrdppwd.h"

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
	void getUserAuth( const char* guardType, const char* user, CommMsgBody& clientExtra, CommMsgBody& request ) override
//	void getUserAuth( const char* guardType, const char* user, CommMsgBody& request ) override
		{
		PASSERT3( strcmp( guardType, CommServerPlainTextPasswordGuard::userAuthHandled() ) == 0 );
		const char* password = user;
		CommMsgParser parser( request );
		CommMsgBody reply;
		CommServerPlainTextPasswordGuard::processUserAuth( reply, parser, user, password );
		CommMsgParser replyParser( reply );
		gotUserAuth( &replyParser );
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
		return new ServerConn( obj );
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

int main( int argc, char** argv )
	{
//DebugBreak();
	ServerObjectFactoryS factory;
	return CommMultiLocalAppMain( argc, argv, factory );
	}
