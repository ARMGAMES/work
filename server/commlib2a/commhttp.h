#ifndef commhttp_h_included
#define commhttp_h_included

#include "ppinclude.h"
#include "ppstring.h"
#include "commurl.h"

struct CommHttpHeader
{
	PString name;
	PString value;
	void compose(CommMsgBody &body) const
	{
		body.composeString(name)
			.composeString(value);
	}
	void parse(CommMsgParser &parser)
	{
		parser.parseStringP(name)
			.parseStringP(value);
	}
};

class CommHttp
{
public:
	static void parseUrl( CommUrl& ret, const char* url );
	static void makeUrl( PString& ret, const CommUrl& url );
};

class CommHttpRequest
{
private:
	CommUrl url;
	vector< CommHttpHeader > headers;
	PString proxy;
	int proxyPort;
	bool noPortInHostName;

public:
	CommHttpRequest();
	void clear();
	void setUrl( const char* url_ );
	void setNoPortInHostName()
	{
		noPortInHostName=true;
	};
	void setProxy( const char* proxy, int proxyPort );
	void addHeader( const char* name, const char* value );

public:
	const char* socketHost() const { return *proxy ? proxy : url.host; }
	int socketPort() const { return *proxy ? proxyPort : url.port; }

	void createGetRequest( PString& ret );
	void createPostRequestHeader( PString& ret, size_t sz, const char * contentType = "application/x-www-form-urlencoded" );
	void createConnectRequest( PString& ret );
	void createGenericRequestHeader( PString& ret, size_t sz, const char* method, const char * contentType );
private:
	void _createRequestHeader( PString& ret, const char* method );

	friend class CommHttp11Request;
};

class CommHttpResponse
{
private:
	int state;//0 - HTTP status line, 1 - headers, 2 - body
	int version;
	PString partialLine;

	int respCode;
	PString reason;
	vector< CommHttpHeader > headers;

public:
	CommHttpResponse();
	void clear();
	bool read( const BYTE*& p, size_t sz );
	//either partially reads header and returns false, 
	//or completely reads header and returns true 
	//if header is only partially read, 'p' is read completely

public:
	int getRespCode() const
	{
		PASSERT3( state == 2 );
		return respCode;
	}
	const char * getReason() const
	{
		PASSERT3( state == 2 );
		return reason.c_str();
	}

	int getVersion() const
	{
		return version;
	}
	const char* getHeader( const char* name ) const;
	const char* getHeader( const char* name, const char* defaultValue ) const
	{
		const char* ret = getHeader( name );
		return ret ? ret : defaultValue;
	}
	const vector< CommHttpHeader >& getHeaders() const
	{
		return headers;
	}
	void copyFrom(const CommHttpResponse &other)
	{
		state=other.state;
		version=other.version;
		partialLine = other.partialLine;
		respCode = other.respCode;
		reason = other.reason;;
		headers = other.headers;
	}
	void compose(CommMsgBody & body) const
	{
		body.composeINT32(state)
			.composeINT32(version)
			.composeINT32(respCode)
			.composeString(reason)
			.composeUINT32(headers.size());
		for (auto h : headers)
		{
			body.composeObject<CommHttpHeader>(h);
		}
	}
	void parse(CommMsgParser &parser)
	{
		UINT32 sz;
		parser.parseINT32(state)
			  .parseINT32(version)
			  .parseINT32(respCode)
			  .parseStringP(reason)
  			  .parseUINT32(sz);
		headers.resize(sz);
		for (UINT32 i = 0; i<sz; i++)
		{
			headers[i].parse(parser);
		}
	}

};


class CommHttp11Request : public CommHttpRequest
{
private:

public:
	CommHttp11Request(){};

public:

	void createGetRequest( PString& ret );
	void createPostRequestHeader( PString& ret, size_t sz, const char * contentType = "application/x-www-form-urlencoded"  );
	void createGenericRequestHeader( PString& ret, size_t sz, const char* method, const char * contentType );
	void createConnectRequest( PString& ret );
	void setLastRequest();


private:
	void _createRequestHeader( PString& ret, const char* method );
};

class CommHttp11ResponseData
{

	bool chunked;
	int  length;
	int state; // 0 - first request, 1 - body ; 2 - last request after 0 length 
	CommHttpResponse & response;
	PString partialLine;

public:
	CommHttp11ResponseData(CommHttpResponse & response_);
	int read( const BYTE*& p, size_t sz );
};

//****************************************************************************

class CommHttpServerRequest
{
private:
	int state;//0 - HTTP request line, 1 - headers, 2 - body
	PString partialLine;

	PString method;
	CommUrl url;
	vector< CommHttpHeader > headers;

public:
	CommHttpServerRequest();
	void clear();
	bool read( const BYTE*& p, size_t sz );
	//either partially reads header and returns false, 
	//or completely reads header and returns true 
	//if header is only partially read, 'p' is read completely

public:
	const char* getMethod() const
	{
		PASSERT3( state == 2 );
		return method;
	}
	const char* getPath() const
	{
		PASSERT3( state == 2 );
		return url.path;
	}
	const char* getHeader( const char* name ) const;
	const char* getHeader( const char* name, const char* defaultValue ) const
	{
		const char* ret = getHeader( name );
		return ret ? ret : defaultValue;
	}
	const vector< CommHttpHeader > & getHeaders() const
	{
		return headers;
	}
	void copyFrom(const CommHttpServerRequest &other)
	{
		state=other.state;
		partialLine = other.partialLine;
		method=other.method;
		url=other.url;
		headers = other.headers;
	}
	void compose(CommMsgBody & body) const
	{
		body.composeINT32(state)
			.composeString(method)
			.composeObject<CommUrl>(url)
			.composeUINT32(headers.size());
		for (auto h : headers)
		{
			body.composeObject<CommHttpHeader>(h);
		}
	}
	void parse(CommMsgParser &parser)
	{
		UINT32 sz;
		parser.parseINT32(state)
			  .parseStringP(method)
			  .parseObject<CommUrl>(url)
			  .parseUINT32(sz);
		headers.resize(sz);
		for (UINT32 i = 0; i<sz; i++)
		{
			headers[i].parse(parser);
		}
	}
};

class CommHttpServerResponse
{
private:
	int respCode;
	PString reason;
	vector< CommHttpHeader > headers;

public:
	CommHttpServerResponse();
	void clear();
	void setResponse( int respCode_, const char* reason_ );
	void addHeader( const char* name, const char* value );

public:
	void createResponse( PString& ret );
};

#endif
