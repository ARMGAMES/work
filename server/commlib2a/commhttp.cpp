#include "ppinclude.h"
#include "ppstring.h"
#include "pperror.h"
#include "pplogfile.h"
#include "commhttp.h"

/* static */ void CommHttp::parseUrl( CommUrl& ret, const char* url )
{
	if( strncmp( url, "http://", 7 /* strlen( "http://" ) */ ) == 0 )
	{
		url += 7 /* strlen( "http://" */;
		ret.port = 80;//default
	}
	else if( strncmp( url, "https://", 8 /* strlen( "https://" ) */ ) == 0 )
	{
		url += 8 /* strlen( "https://" */;
		ret.port = 443;//default
	}
	else
		throw PInternalError( "Unknown protocol" );

	const char* slash = strchr( url, '/' );
	const char* colon = strchr( url, ':' );
	if( !slash )
	{
		if( colon )
		{
			ret.host.assign( url, colon );
			ret.port = atoi( colon + 1 );
		}
		else
			ret.host = url;
		ret.path = "/";
	}
	else
	{
		if( colon && colon < slash )
		{
			ret.host.assign( url, colon );
			ret.port = atoi( colon + 1 );
		}
		else
			ret.host.assign( url, slash );

		const char* pound = strchr( slash, '#' );
		if( pound )
			ret.path.assign( slash, pound );
		else
			ret.path = slash;
	}
}

/* static */ void CommHttp::makeUrl( PString& ret, const CommUrl& url )
{
	ret.assign( "http://" ).append( url.host );
	if( url.port != 80 )
		ret.append( ":" ).appendInt( url.port );
	ret.append( url.path );
}

//****************************************************************************

CommHttpRequest::CommHttpRequest()
{
	proxyPort = 0;
	noPortInHostName=false;
}

void CommHttpRequest::clear()
{
	proxyPort = 0;
	noPortInHostName=false;
	headers.clear();
	proxy="";
	url.clear();
}


void CommHttpRequest::setUrl( const char* url_ )
{
	CommHttp::parseUrl( url, url_ );
}

void CommHttpRequest::setProxy( const char* proxy_, int proxyPort_ )
{
	proxy = proxy_;
	proxyPort = proxyPort_;
}

void CommHttpRequest::addHeader( const char* name, const char* value )
{
	CommHttpHeader& hdr = *headers.insert( headers.end(), CommHttpHeader() );
	hdr.name = name;
	hdr.value = value;
}

void CommHttpRequest::createGenericRequestHeader( PString& ret, size_t sz, const char* method, const char * contentType )
{
	char buf[ 32 ];
	sprintf( buf, "%llu", (UINT64)sz );
	addHeader( "Content-Type", contentType);
	addHeader( "Content-Length", buf );
	_createRequestHeader( ret, method );
}

void CommHttpRequest::_createRequestHeader( PString& ret, const char* method )
{
	if( *proxy == 0 )
	{//no proxy
		ret.append( method ).append( " " ).append( url.path ).append( " HTTP/1.0\r\n" );
		if (noPortInHostName)
			ret.append( "Host: " ).append( url.host ).append( "\r\n" );
		else
			ret.append( "Host: " ).append( url.host ).append( ':' ).appendInt( url.port ).append( "\r\n" );
		ret.append( "Pragma: no-cache\r\n" );
	}
	else
	{
		ret.append( method ).append( " http://" ).append( url.host ).append( ':' ).appendInt( url.port ).append( url.path ).append( " HTTP/1.0\r\n" );
		if (noPortInHostName)
			ret.append( "Host: " ).append( url.host ).append( "\r\n" );
		else
			ret.append( "Host: " ).append( url.host ).append( ':' ).appendInt( url.port ).append( "\r\n" );
		ret.append( "Pragma: no-cache\r\n" );
	}

	int n = headers.size();
	for( int i=0; i < n ; ++i )
		ret.append( headers[ i ].name ).append( ": " ).append( headers[ i ].value ).append( "\r\n" );
	ret.append( "\r\n" );
}

void CommHttpRequest::createGetRequest( PString& ret )
{
	_createRequestHeader( ret, "GET" );
}

void CommHttpRequest::createPostRequestHeader( PString& ret, size_t sz, const char * contentType )
{
	char buf[ 32 ];
	sprintf( buf, "%llu", (UINT64)sz );
	addHeader( "Content-Type", contentType);
	addHeader( "Content-Length", buf );
	_createRequestHeader( ret, "POST" );
}

void CommHttpRequest::createConnectRequest( PString& ret )
{
	PASSERT3( *proxy );//CONNECT without proxy does not make much sense
	ret.append( "CONNECT " ).append( url.host ).append( ':' ).appendInt( url.port ).append( " HTTP/1.0\r\n" );

	int n = headers.size();
	for( int i=0; i < n ; ++i )
		ret.append( headers[ i ].name ).append( ": " ).append( headers[ i ].value ).append( "\r\n" );
	ret.append( "\r\n" );
}

void CommHttp11Request::_createRequestHeader( PString& ret, const char* method )
{
	if( *proxy == 0 )
	{//no proxy
		ret.append( method ).append( " " ).append( url.path ).append( " HTTP/1.1\r\n" );
		if (noPortInHostName)
			ret.append( "Host: " ).append( url.host ).append( "\r\n" );
		else
			ret.append( "Host: " ).append( url.host ).append( ':' ).appendInt( url.port ).append( "\r\n" );
		ret.append( "Pragma: no-cache\r\n" );
	}
	else
	{
		ret.append( method ).append( " http://" ).append( url.host ).append( ':' ).appendInt( url.port ).append( url.path ).append( " HTTP/1.1\r\n" );
		if (noPortInHostName)
			ret.append( "Host: " ).append( url.host ).append( "\r\n" );
		else
			ret.append( "Host: " ).append( url.host ).append( ':' ).appendInt( url.port ).append( "\r\n" );
		ret.append( "Pragma: no-cache\r\n" );
	}

	int n = headers.size();
	for( int i=0; i < n ; ++i )
		ret.append( headers[ i ].name ).append( ": " ).append( headers[ i ].value ).append( "\r\n" );
	ret.append( "\r\n" );
}

void CommHttp11Request::createConnectRequest( PString& ret )
{
	PASSERT3( *proxy );//CONNECT without proxy does not make much sense
	ret.append( "CONNECT " ).append( url.host ).append( ':' ).appendInt( url.port ).append( " HTTP/1.1\r\n" );

	int n = headers.size();
	for( int i=0; i < n ; ++i )
		ret.append( headers[ i ].name ).append( ": " ).append( headers[ i ].value ).append( "\r\n" );
	ret.append( "\r\n" );
}

void CommHttp11Request::createGetRequest( PString& ret )
{
	_createRequestHeader( ret, "GET" );
}

void CommHttp11Request::createPostRequestHeader( PString& ret, size_t sz, const char * contentType  )
{
	char buf[ 32 ];
	sprintf( buf, "%llu", (UINT64)sz );
	addHeader( "Content-Type", contentType );
	addHeader( "Content-Length", buf );
	_createRequestHeader( ret, "POST" );
}

void CommHttp11Request::createGenericRequestHeader( PString& ret, size_t sz, const char* method, const char * contentType )
{
	char buf[ 32 ];
	sprintf( buf, "%llu", (UINT64)sz );
	addHeader( "Content-Type", contentType);
	addHeader( "Content-Length", buf );
	_createRequestHeader( ret, method );
}

void CommHttp11Request::setLastRequest()
{
	addHeader( "Connection", "close" );
}



//****************************************************************************

CommHttpResponse::CommHttpResponse()
{
	state = 0;
	version=0;
	//partialLine = "";
}

void CommHttpResponse::clear()
{
	state = 0;
	version=0;
	partialLine = "";
	respCode=0;
	reason="";
	headers.clear();

}

bool CommHttpResponse::read( const BYTE*& p, size_t sz )
{
	PASSERT3( state < 2 );
	const BYTE* pEnd = p + sz;
	for(;;)
	{
		bool lineOk = CommLine::read( partialLine, p, pEnd - p );
		if( !lineOk )
		{
			PASSERT3( p == pEnd );
			return false;
		}
		if( state == 0 )
		{
			if( partialLine.length() == 0 )//empty, assuming respCode=200
			{
				respCode = 200;
				state = 2;
				//partialLine = "";
				return true;
			}

			const char* ln = partialLine;
			if( strncmp( ln, "HTTP/", 5 /* strlen( "HTTP/" ) */ ) != 0 )
				throw PInternalError( "'HTTP/' expected at the beginning of HTTP status line" );
			if( !strncmp( ln + 5,"1.1",3))
				version=1;
			const char* sp = strchr( ln, ' ' );
			if( sp == 0 )
				throw PInternalError( "Status code expected in HTTP status line" );
			ln = sp + 1;

			respCode = atoi( ln );
			sp = strchr( ln, ' ' );
			if( sp == 0 )
				throw PInternalError( "Reason string expected in HTTP status line" );
			reason = sp + 1;
			state = 1;
		}
		else
		{
			PASSERT3( state == 1 );
			const char* hdrLn = partialLine;
			if( *hdrLn == 0 )
			{
				state = 2;
				return true;
			}
			if( headers.size() > 0 && (*hdrLn == ' ' || *hdrLn == '\t' ))
			{
				headers.back().value.append( ' ' ).append( PString::ltrim( hdrLn ), PString::rtrim( hdrLn ) );
			}
			else
			{
				const char* colon = strchr( hdrLn, ':' );
				if( colon == 0 )
				{
					//ignore
				}
				else
				{
					PString name( hdrLn, colon );
					PString value( PString::ltrim( colon + 1 ), PString::rtrim( colon + 1 ) );

					CommHttpHeader& hdr = *headers.insert( headers.end(), CommHttpHeader() );
					hdr.name = name;
					hdr.value = value;
				}
			}
		}//state == 1
		partialLine = "";
	}//forever
}

const char* CommHttpResponse::getHeader( const char* name ) const
{
	PASSERT3( state == 2 );
	int n = headers.size();
	for( int i=0; i < n ; ++i )
		if( PString::compareIgnoreCase( headers[ i ].name, name ) == 0 )
			return headers[ i ].value;
	return 0;
}

int CommHttp11ResponseData::read( const BYTE*& p, size_t sz )
{
	const BYTE* pEnd = p + sz;
	bool lineOk;

	if (chunked)
	{
		for (;;)
		{
			lineOk = CommLine::read( partialLine, p, pEnd - p );
			if( !lineOk )
			{
				PASSERT3( p == pEnd );
				return -1;
			}
			switch (state )
			{
			case 0:
				{
					length = strtoul(partialLine,0,16);
					partialLine.assign("");
					if (length)
					{
						state = 1;
						return length;
					}
					else
					{
						state = 3;
						break;
					}
				}
				break;
			case 1: // have read first "\r\n" line read next with length
				{
					state =2;
					break;
				}
			case 2:
				{
					length = strtoul(partialLine,0,16);
					partialLine.assign("");
					if (length)
					{
						state = 1;
						return length;
					}
					else
					{
						state = 3;
						break;
					}
				}
				break;
			case 3:
				{
					state =4;
					return 0;
				}
				break;
			case 4:
				return 0;
			}
		}
	}
	else
	{
		if (state == 0)
		{
			state =3;
			return length;
		}
		else if (state == 3)
		{
			state =4;
			return 0;
		}
		else
		{
			return 0;
		}

	}
}

CommHttp11ResponseData::CommHttp11ResponseData(CommHttpResponse & response_) :
response(response_)
{
	const char * trType = response.getHeader("Transfer-Encoding");
	if (trType)
	{
		if (!PString::compareIgnoreCase(trType,"chunked"))
		{
			state=0;
			chunked=true;
			length=0;
			return;
		}
	}
	chunked=false;
	length=0;
	state =0;
	const char* cl = response.getHeader( "Content-Length" );
	if( cl )
		length = atoi( cl );
}
