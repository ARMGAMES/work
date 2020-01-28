#ifndef commsmtp_h_included
#define commsmtp_h_included

#include "ppinclude.h"
#include "commmail.h"

class _CommSmtpMailTransError : public PInternalError
{
public:
	_CommSmtpMailTransError( const char* msg )
		: PInternalError( msg )
	{
	}
};

class CommSmtp
{
private:
	PString host;
	int port;
	PString thisHost;

public:
	CommSmtp();
	void init( const char* host_, int port_, const char* thisHost_ );
	void sendMail( const CommMail& mail, int timeout = 0  );
	void sendRawMail( const char* from, const vector< CommMail::Address >& to, const BYTE* p, size_t sz, int timeout = 0  );
	int sendMultipleMails( const vector< CommMail >& mails, bool* oks, int timeout = 0  );
	//oks, if present, is an input/output parameter: 
	//  oks size MUST be the same as mails.size()
	//	if oks[ i ] is true, mail is not sent; 
	//	if oks[ i ] is false and mail is successfully sent, oks[ i ] is set to true

private:
	void _begin( PIPSocket& sock, CommSocketLineReader& in );
	void _mailTransaction( PIPSocket& sock, CommSocketLineReader& in, const CommMail& mail );
	void _mailTransaction( PIPSocket& sock, CommSocketLineReader& in, const char* from, const vector< CommMail::Address >& to, const BYTE* p, size_t sz );
	void _end( PIPSocket& sock, CommSocketLineReader& in );
};

#endif
