#include "ppinclude.h"
#include "ppstring.h"
#include "pperror.h"
#include "pplogfile.h"
#include "commhttp.h"

CommHttpServerRequest::CommHttpServerRequest()
{
	state = 0;
}

void CommHttpServerRequest::clear()
{
	state = 0;
	partialLine="";
	method="";
	url.clear();
	headers.clear();
}

bool CommHttpServerRequest::read( const BYTE*& p, size_t sz )
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
			if( partialLine.length() == 0 )
			{//ignore
				partialLine = "";
				continue;//forever
			}

			const char* ln = partialLine;
			const char* sp = strchr( ln, ' ' );
			if( sp == 0 )
				throw PInternalError( "Request-URI expected in HTTP request line" );
			method.assign( ln, sp );
			ln = sp + 1;

			sp = strchr( ln, ' ' );
			if( sp == 0 )
				throw PInternalError( "HTTP-Version expected in HTTP request line" );
			PString path( ln, sp );
			if( strncmp( path, "http://", 7 /* strlen( "http://" ) */ ) == 0 ||
				strncmp( path, "https://", 8 /* strlen( "https://" ) */ ) == 0 )
			{//absoluteURI
				CommHttp::parseUrl( url, path );
			}
			else
			{
				url.host = "";
				url.port = 0;
				url.path = path;
			}

			ln = sp + 1;
			if( strncmp( ln, "HTTP/", 5 /* strlen( "HTTP/" ) */ ) != 0 )
				throw PInternalError( "'HTTP/' expected in HTTP request line" );
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
			if( headers.size() > 0 && *hdrLn == ' ' || *hdrLn == '\t' )
				headers.back().value.append( ' ' ).append( PString::ltrim( hdrLn ), PString::rtrim( hdrLn ) );
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

const char* CommHttpServerRequest::getHeader( const char* name ) const
{
	PASSERT3( state == 2 );
	int n = headers.size();
	for( int i=0; i < n ; ++i )
		if( PString::compareIgnoreCase( headers[ i ].name, name ) == 0 )
			return headers[ i ].value;
	return 0;
}

//****************************************************************************

CommHttpServerResponse::CommHttpServerResponse()
{
	respCode = 200;
	reason = "OK";
}

void CommHttpServerResponse::clear()
{
	respCode = 200;
	reason = "OK";
	headers.clear();
}



void CommHttpServerResponse::setResponse( int respCode_, const char* reason_ )
{
	respCode = respCode_;
	reason = reason_;
}

void CommHttpServerResponse::addHeader( const char* name, const char* value )
{
	CommHttpHeader& hdr = *headers.insert( headers.end(), CommHttpHeader() );
	hdr.name = name;
	hdr.value = value;
}

void CommHttpServerResponse::createResponse( PString& ret )
{
	ret.append( "HTTP/1.0 " ).appendInt( respCode ).append( ' ' ).append( reason ).append( "\r\n" );

	int n = headers.size();
	for( int i=0; i < n ; ++i )
		ret.append( headers[ i ].name ).append( ": " ).append( headers[ i ].value ).append( "\r\n" );
	ret.append( "\r\n" );
}
