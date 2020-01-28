#include "ppinclude.h"
#include "ppsocket.h"
#include "commurl.h"
#include "commmail.h"
#include "commpop3.h"

static bool _readFirstPop3Ln( CommSocketLineReader& reader, PString & s )
{
	s="";
	reader.readLine( s );
	const char* p = PString::ltrim( s );
	return PString::compareIgnoreCase( p, "+OK", 3 /*strlen("+OK")*/ ) == 0;
}

static bool _readFirstPop3Ln( PString& ret, CommSocketLineReader& reader, PString & s )
{
	s="";
	reader.readLine( s );
	const char* p = PString::ltrim( s );
	if( PString::compareIgnoreCase( p, "+OK", 3 /*strlen("+OK")*/ ) == 0 )
	{
		ret = s + 3 /*strlen("+OK")*/;
		return true;
	}
	else
		return false;
}

static void _readPop3MultiLn( PString& ret, CommSocketLineReader& reader )
{
	for(;;)
	{
		PString s;
		reader.readLine( s );
		if( *s == '.' )
		{
			if( s[1] == 0 )
				break;//forever
			else
				ret.append( s + 1 ).append( "\r\n" );
		}
		else
			ret.append( s ).append( "\r\n" );
	}
}

CommPop3::CommPop3()
{
	port = -1;
	sock = 0;
	in = 0;
	haveMore_ = false;
	current=0;
}

void CommPop3::init( const char* host_, int port_, const char* user_, const char* password_ )
{
	host = host_;
	port = port_;
	user = user_;
	password = password_;
}

void CommPop3::getMailDeletePending( PNewPtrVector< PString >& v, int maxMessages, int timeout )
{
	PASSERT3( sock == 0 );
	sock = new PIPSocket();
	sock->connect( host, port );

	PASSERT3( in == 0 );
	in = new CommSocketLineReader( *sock, timeout );
	PString reply;
	bool Ok = _readFirstPop3Ln( *in, reply );
	if( !Ok )
	{
		PString err("Unexpected POP3 prompt:");
		err.append(reply.c_str());
		throw PInternalError( err.c_str());
	}

	{
		PString out;
		out.append( "USER " ).append( user ).append( "\r\n" );
		sock->sendComplete( (const BYTE*)out.c_str(), strlen( out ) );
		Ok = _readFirstPop3Ln( *in, reply );
		if( !Ok )
		{
			PString err("Unexpected POP3 response to USER:");
			err.append(reply.c_str());
			throw PInternalError( err.c_str());
		}
	}

	{
		PString out;
		out.append( "PASS " ).append( password ).append( "\r\n" );
		sock->sendComplete( (const BYTE*)out.c_str(), strlen( out ) );
		Ok = _readFirstPop3Ln( *in, reply );
		if( !Ok )
		{
			PString err("Unexpected POP3 response to PASS:");
			err.append(reply.c_str());
			throw PInternalError( err.c_str());
		}
	}
	current=0;
	getNextMailDeletePending(v,maxMessages );

}

void CommPop3::getNextMailDeletePending( PNewPtrVector< PString >& v, int maxMessages )
{
	PASSERT3( sock !=0 );

	PString out;
	out.append( "STAT" ).append( "\r\n" );
	sock->sendComplete( (const BYTE*)out.c_str(), strlen( out ) );
	PString stat;
	PString reply;
	bool Ok = _readFirstPop3Ln( stat, *in, reply );
	if( !Ok )
	{
		PString err("Unexpected POP3 response to STAT:");
		err.append(reply.c_str());
		throw PInternalError( err.c_str());
	}
	nOutstandingMessages = atoi( stat );
	if (nOutstandingMessages>maxMessages)
	{
		haveMore_ = true;
		nOutstandingMessages = maxMessages;
	}
	else
	{
		haveMore_ = false;
	}
	for( int i=1; i <= nOutstandingMessages ; ++i )//1 to n
	{
		PString out;
		out.append( "RETR " ).appendInt( i + current ).append( "\r\n" );
		sock->sendComplete( (const BYTE*)out.c_str(), strlen( out ) );
		Ok = _readFirstPop3Ln( *in, reply );
		if( !Ok )
		{
			PString err("Unexpected POP3 response to RETR:");
			err.append(reply.c_str());
			throw PInternalError( err.c_str());
		}

		PString* msg = new PString();
		v.push_back( msg );
		_readPop3MultiLn( *msg, *in );
	}
}


void CommPop3::rollbackDelete()
{
	PASSERT3( sock );
	_quit();
}

void CommPop3::commitDelete()
{
	markAsDelete();
	_quit();
}

void CommPop3::markAsDelete()
{
	PASSERT3( sock );
	for( int i=1; i <= nOutstandingMessages ; ++i )//1 to n
	{
		PString out;
		PString reply;
		out.append( "DELE " ).appendInt( i + current ).append( "\r\n" );
		sock->sendComplete( (const BYTE*)out.c_str(), strlen( out ) );
		bool Ok = _readFirstPop3Ln( *in, reply );
		if( !Ok )
		{
			PString err("Unexpected POP3 response to DELE:");
			err.append(reply.c_str());
			throw PInternalError( err.c_str());
		}
	}
	current += nOutstandingMessages;
}

void CommPop3::_quit()
{
	if( sock )
	{
		PString out;
		out.append( "QUIT" ).append( "\r\n" );
		sock->sendComplete( (const BYTE*)out.c_str(), strlen( out ) );
		PString reply;
		_readFirstPop3Ln( *in ,reply);//read but not analyze

		delete in;
		in = 0;
		sock->close();
		delete sock;
		sock = 0;
	}
}

void CommPop3::getMail( PNewPtrVector< PString >& v )
{
	getMailDeletePending( v );
	commitDelete();
}

CommPop3::~CommPop3()
{
	//just in case - _quit()
	try
	{
		_quit();
	}
	catch( PError& )
	{
	}
	if (in)
		delete in;
	if (sock)
	{
		sock->close();
		delete sock;
	}

}
