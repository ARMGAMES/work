#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"
#include "ppsocket.h"
#include "commurl.h"
#include "commsmtp.h"

/*
static int _readSmtpLn( CommSocketLineReader& reader )
{
PString s;
reader.readLine( s );
return atoi( s );
}
*/
static int _readSmtpLn( CommSocketLineReader& reader, PString& s )
{
	s = "";
	reader.readLine( s );
	return atoi( s );
}

static void _throwError( const char* s, const char* err )
{
	PString msg( s );
	msg.append( ": " ).append( err );
	throw PInternalError( msg );
}

CommSmtp::CommSmtp()
{
	port = -1;
}

void CommSmtp::init( const char* host_, int port_, const char* thisHost_ )
{
	host = host_;
	port = port_;
	thisHost = thisHost_;
}

void CommSmtp::_begin( PIPSocket& sock, CommSocketLineReader& in )
{
	PString err;
	int code = _readSmtpLn( in, err );
	if( code != 220 )
		_throwError( "Unexpected SMTP prompt", err );

	{
		PString out;
		out.append( "HELO " ).append( thisHost ).append( "\r\n" );
		sock.sendComplete( (const BYTE*)out.c_str(), strlen( out ) );
		code = _readSmtpLn( in, err );
		if( code != 250 )
			_throwError( "Unexpected SMTP response to HELO", err );
	}
}


static void _throwMailTransError( const char* s, const char* err )
{
	PString msg( s );
	msg.append( ": " ).append( err );
	throw _CommSmtpMailTransError( msg );
}

void CommSmtp::_mailTransaction( PIPSocket& sock, CommSocketLineReader& in, const CommMail& mail )
{
	PString data;
	mail.out( data );
	_mailTransaction( sock, in, mail.from().addrNormalized, mail.addresses(), (const BYTE*)data.c_str(), strlen( data ) );
}

void CommSmtp::_mailTransaction( PIPSocket& sock, CommSocketLineReader& in, const char* from, const vector< CommMail::Address >& to, const BYTE* p, size_t sz )
{
	PString err;
	{
		//	PString normalFrom;
		//	CommMail::normalizeAddress( normalFrom, from );
		PLog( "MAIL FROM:<%s>", from );

		PString out;
		out.append( "MAIL FROM:<" ).append( from ).append( ">\r\n" );
		sock.sendComplete( (const BYTE*)out.c_str(), strlen( out ) );
		int code = _readSmtpLn( in, err );
		if( code != 250 )
			_throwMailTransError( "Unexpected SMTP response to MAIL FROM", err );
	}

	const vector< CommMail::Address >& addrs = to;
	int n = addrs.size();
	bool recipients=false;
	for( int i=0 ; i < n ; ++i )
	{
		const CommMail::Address& addr = addrs[ i ];

		//		PString to;
		//		CommMail::normalizeAddress( to, addr.addr );
		if (addr.alreadyProcessed)
		{
			PLog( "<%s> is not sent", addr.addrNormalized.c_str() );
		}
		else
		{
			PLog( "RCPT TO:<%s>", addr.addrNormalized.c_str() );
			recipients = true;

			PString out;
			out.append( "RCPT TO:<" ).append( addr.addrNormalized).append( ">\r\n" );
			sock.sendComplete( (const BYTE*)out.c_str(), strlen( out ) );
			int code = _readSmtpLn( in, err );
			if( code != 250 )
				_throwMailTransError( PString( "Error relaying to " ).append( addr.addr ), err );
		}
	}
	if (recipients == false)
	{
		_throwMailTransError( "Invalid number of recipients", "No valid recipients");
	}
	{
		PString out;
		out.append( "DATA\r\n" );
		sock.sendComplete( (const BYTE*)out.c_str(), strlen( out ) );
		int code = _readSmtpLn( in, err );
		if( code != 354 )
			_throwMailTransError( "Unexpected SMTP response to DATA", err );
	}

	{
		const BYTE* pEnd = p + sz;
		for(;;)
		{
			PString line;
			bool Ok = CommLine::read( line, p, pEnd - p );

			PString out;
			if( *line == '.' )
				out.append( "." ).append( line ).append( "\r\n" );
			else
				out.append( line ).append( "\r\n" );
			sock.sendComplete( (const BYTE*)out.c_str(), strlen( out ) );
			if( !Ok )
				break;
		}

		PString out;
		out.append( "\r\n.\r\n" );
		sock.sendComplete( (const BYTE*)out.c_str(), strlen( out ) );
		int code = _readSmtpLn( in, err );
		if( code != 250 )
			_throwMailTransError( "Unexpected SMTP response to data sent", err );
	}
}

void CommSmtp::_end( PIPSocket& sock, CommSocketLineReader& in )
{
	PString out;
	out.append( "QUIT\r\n" );
	sock.sendComplete( (const BYTE*)out.c_str(), strlen( out ) );
	PString err;
	_readSmtpLn( in, err );//wait for response; ignore code and err
}

void CommSmtp::sendMail( const CommMail& mail, int timeout )
{
	PIPSocket sock;
	bool Ok = sock.enableKeepAlive();
	if( !Ok )
		PLog( "CommSmtp: unable to enable keepAlive" );
	sock.connect( host, port );

	CommSocketLineReader in( sock, timeout );

	_begin( sock, in );
	try 
	{
		_mailTransaction( sock, in, mail );
	}
	catch( _CommSmtpMailTransError& err )//other errors (e.g. socket errors) - will not be caught and will be propagated to upper level
	{
		PLog( "CommSmtp::sendMail: _CommSmtpMailTransError while sending mail: %s", err.why() );
		//exception (most likely - socket one) in the following block will be propagated to upper level
		PString out( "RSET\r\n" );
		sock.sendComplete( (const BYTE*)out.c_str(), strlen( out ) );
		PString err2Str;
		int code = _readSmtpLn( in, err2Str );
		if( code == 250 )
			PLog( "CommSmtp::sendMail: RSET Ok" );
		else
			PLog( "CommSmtp::sendMail: RSET failed: %s", err2Str.c_str() );
		_end( sock, in );
		sock.close();
		throw err;
	}
	_end( sock, in );
	sock.close();
}

int CommSmtp::sendMultipleMails( const vector< CommMail >& mails, bool* oks, int timeout )
{
	PIPSocket sock;
	bool Ok = sock.enableKeepAlive();
	if( !Ok )
		PLog( "CommSmtp: unable to enable keepAlive" );
	sock.connect( host, port );

	CommSocketLineReader in( sock, timeout );

	_begin( sock, in );
	int n = mails.size();
	int ret = 0;
	for( int i=0; i < n ; ++i )
	{
		if( oks && oks[ i ] )//don't try to send if already ok
			continue;
		try
		{
			_mailTransaction( sock, in, mails[ i ] );
			if( oks )
				oks[ i ] = true;
			++ret;
		}
		catch( _CommSmtpMailTransError& err )//other errors (e.g. socket errors) - will not be caught and will be propagated to upper level
		{
			PLog( "CommSmtp::sendMultipleMails(...): _CommSmtpMailTransError while sending mail #%d of %d: %s", i + 1, n, err.why() );

			//exception (most likely - socket one) in the following block will be propagated to upper level
			PString out( "RSET\r\n" );
			sock.sendComplete( (const BYTE*)out.c_str(), strlen( out ) );

			PString err2Str;
			int code = _readSmtpLn( in, err2Str );
			if( code == 250 )
				PLog( "CommSmtp::sendMultipleMails(...): RSET Ok" );
			else
				PLog( "CommSmtp::sendMultipleMails(...): RSET failed: %s", err2Str.c_str() );
		}
	}
	_end( sock, in );
	sock.close();
	return ret;
}

void CommSmtp::sendRawMail( const char* from, const vector< CommMail::Address >& to, const BYTE* p, size_t sz, int timeout )
{
	PIPSocket sock;
	bool Ok = sock.enableKeepAlive();
	if( !Ok )
		PLog( "CommSmtp: unable to enable keepAlive" );
	sock.connect( host, port );

	CommSocketLineReader in( sock, timeout );

	_begin( sock, in );
	try
	{
		_mailTransaction( sock, in, from, to, p, sz );
	}
	catch( _CommSmtpMailTransError& err )//other errors (e.g. socket errors) - will not be caught and will be propagated to upper level
	{
		PLog( "CommSmtp::sendMail: _CommSmtpMailTransError while sending mail: %s", err.why() );
		//exception (most likely - socket one) in the following block will be propagated to upper level
		PString out( "RSET\r\n" );
		sock.sendComplete( (const BYTE*)out.c_str(), strlen( out ) );
		PString err2Str;
		int code = _readSmtpLn( in, err2Str );
		if( code == 250 )
			PLog( "CommSmtp::sendMail: RSET Ok" );
		else
			PLog( "CommSmtp::sendMail: RSET failed: %s", err2Str.c_str() );
		_end( sock, in );
		sock.close();
		throw err;
	}

	_end( sock, in );
	sock.close();
}
