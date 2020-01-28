#ifndef commcgi_h_included
#define commcgi_h_included

#include "ppinclude.h"
#include "ppstring.h"
#include "pperror.h"
#include "ppcontainer.h"

class CommCgiProperties
{
private:
	struct _Item
	{
		bool isBinary;
		PString s;
		PBlock b;
		PString fileName; 
	};
	typedef PStringMap< _Item > PropsType;
public:
	typedef PropsType::const_iterator iterator;

private:
	PropsType props;

public:
	CommCgiProperties() {}

	bool empty() const
	{
		return props.size() == 0;
	}

	iterator begin() const
	{
		return props.begin();
	}
	bool isEnd( iterator iter ) const
	{
		return iter == props.end();
	}
	iterator nameBegin( const char* name ) const
	{
		return props.lower_bound( name );
	}
	bool isNameEnd( iterator iter, const char* name ) const
	{
		return iter == props.end() || strcmp( (*iter).first, name ) != 0;
	}

	bool isBinary( iterator iter ) const
	{
		return (*iter).second.isBinary;
	}
	const char* getName( iterator iter ) const
	{
		return (*iter).first;
	}
	const char* getFileName( iterator iter ) const
	{
		return (const char*) (*iter).second.fileName;
	}

	const char* getString( iterator iter ) const;
	void getStringTrim( iterator iter, PString & ret ) const;
	int getInt( iterator iter ) const;
	INT64 getInt64( iterator iter ) const;
	bool getBool( iterator iter ) const;
	void getBinary( const BYTE*& p, size_t& sz, iterator iter ) const;

	//SHORTCUTS
	const char* getString( const char* name, const char* defaultValue = 0 ) const;
	void getStringTrim( const char* name, PString & ret) const;
	const char* getFileName( const char* name) const;
	int getInt( const char* name, int defaultValue = 0 ) const;
	INT64 getInt64( const char* name, int defaultValue = 0 ) const;
	bool getBool( const char* name, bool defaultValue ) const;
	void getBinary( const char * name, const BYTE*& p, size_t& sz ) const;

public:
	void insert( const char* name, const char* value );
	PBlock& insertBinary( const char* name, const char * fileName_ = 0 );

private://prohibited
	CommCgiProperties( const CommCgiProperties& other );
	CommCgiProperties& operator=( const CommCgiProperties& other );
};

class CommCgi
{
public:
	enum { HEAD = 0x00, GET = 0x01, POST = 0x02, POSTMULTIPART = 0x04 };

	//STATIC HELPERS
public:
	static void urlEncode( PString& ret, const char* s, const char* sEnd = 0 )//APPENDS
	{
		CommUrl::urlEncode( ret, s, sEnd );
	}
	static void urlDecode( PString& ret, const char* s, const char* sEnd = 0 )//APPENDS
	{
		CommUrl::urlDecode( ret, s, sEnd );
	}

private:
	PString queryStr;
	CommCgiProperties queryProps;
	CommCgiProperties cookieProps;
	int methodId;
	//	char agentId;
	//	int agentMajor;
	//	int agentMinor;

public:
	CommCgi( int modesAllowed = GET, const char* noCgiQuery = 0 );//GET automatically allows HEAD

	const char* queryString() const
	{
		return queryStr;
	}
	const CommCgiProperties& query() const
	{
		return queryProps;
	}
	const CommCgiProperties& cookies() const
	{
		return cookieProps;
	}
	const char* httpReferer() const
	{
		return getenv( "HTTP_REFERER" );
	}
	const char* remoteAddr() const
	{
		return getenv( "REMOTE_ADDR" );
	}
	const char* userAgent() const
	{
		return getenv( "HTTP_USER_AGENT" );
	}
	const char* serverName() const
	{
		return getenv( "SERVER_NAME" );
	}
	const char* scriptName() const
	{
		return getenv( "SCRIPT_NAME" );
	}
	const char* documentRoot() const
	{
		return getenv( "DOCUMENT_ROOT" );
	}
	bool isSSL() const
	{
		return getenv( "SSL_PROTOCOL" ) != 0;
	}

	/*
	char userAgentId() const//'M' - Microsoft, 'N' - Netscape, 'W' - WebTV
	{
	return agentId;
	}
	int userAgentMajorVer() const
	{
	return agentMajor;
	}
	int userAgentMinorVer() const
	{
	return agentMinor;
	}
	*/
	int method() const
	{
		return methodId;
	}

private:
	void _parseQuery( const char* query );
	void _parseCookie( const char* cookie );

private://prohibited
	CommCgi( const CommCgi& other );
	CommCgi& operator=( const CommCgi& other );
};

class CommCgiReply
{
private:
	CommCgi* cgi;
	bool headers;

	int status;
	PString statusMsg;
	PString location;//Location:
	PString contentType;
	vector< PString > headerNames;
	vector< PString > headerValues;
	const char* body;
	const BYTE* binaryBody;
	size_t binaryBodySz;

public:
	CommCgiReply( bool headers_ = true );
	CommCgiReply( CommCgi& cgi_, bool headers_ = true );

	void setStatus( int status_, const char* msg );
	void setLocation( const char* location_ );
	void setAbsLocation( const char* server, const char* file, const char* queryString = 0, bool isHttps = false );
	void setContentTypeHtml( const char* charset = "iso-8859-1" );
	void setContentType( const char* s, const char* charset = 0 );
	void addHeader( const char* name, const char* value );
	void addCookie( const char* name, const char* value, time_t expires = 0, const char* path = 0, const char* domain = 0, bool secure = false );
	void setBinaryContent( const BYTE* binaryBody_, size_t binaryBodySz_ );
	void setContent( const char* body_ );

	void out();

private:
	void _init();
	void _printExtraHdrs();
};

#endif
