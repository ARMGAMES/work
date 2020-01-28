#ifndef commurl_h_included
#define commurl_h_included

#include "ppinclude.h"
#include "ppstring.h"
#include "ppsocket.h"
#include "commmsgbody.h"

struct CommUrl
{
public:
	PString host;
	int port;
	PString path;
	void clear()
	{
		host="";
		path="";
		port=0;
	}
	void compose(CommMsgBody &body) const
	{
		body.composeString(host)
			.composeINT32(port)
			.composeString(path);
	}
	void parse(CommMsgParser & parser)
	{
		parser.parseStringP(host)
			  .parseINT32(port)
			  .parseStringP(path);
	}
	static void urlEncode( PString& ret, const char* s, const char* sEnd = 0 );//APPENDS
	static void urlDecode( PString& ret, const char* s, const char* sEnd = 0 );//APPENDS
	static void htmlEncode( PString& ret, const char* s );//APPENDS
};

class CommLine
{
public:
	static bool read( PString& ret, const BYTE*& p, size_t maxSz );
	static bool read( PString& ret, const char *& p, size_t maxSz );
	static bool read( PString& ret, const char*& p );
	//either partially reads line and returns false, 
	//or completely reads line and returns true 
	//if line is only partially read, 'p' is read completely and symbols read added to 'ret'
};

class CommSocketLineReader
{
private:
	PIPSocket* sock;
	BYTE buf[ 256 ];
	size_t offset;
	size_t sz;
	int timeout;

public:
	CommSocketLineReader( PIPSocket& sock_, int timeout_ = 0 );
	void readLine( PString& ret );
};

#endif
