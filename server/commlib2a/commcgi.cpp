#include "ppinclude.h"
#include "ppstring.h"
#include "pperror.h"
#include "_commmime.h"
#include "commurl.h"
#include "commcgi.h"

const char* CommCgiProperties::getString( CommCgiProperties::iterator iter ) const
{
	if( (*iter).second.isBinary )
		return (const char*)( (*iter).second.b.ptr() );
	else
		return (*iter).second.s;
}

void CommCgiProperties::getStringTrim( CommCgiProperties::iterator iter, PString &ret ) const
{
	const char * r = getString( iter );
	ret.assign(PString::ltrim(r),PString::rtrim(r));
}

int CommCgiProperties::getInt( CommCgiProperties::iterator iter ) const
{
	return atoi( getString( iter ) );
}

INT64 CommCgiProperties::getInt64( CommCgiProperties::iterator iter ) const
{
	return p_atoi64( getString( iter ) );
}


bool CommCgiProperties::getBool( CommCgiProperties::iterator iter ) const
{
	const char* s = getString( iter );
	return *s == 'y' || *s == 'Y' || atoi( s );
}

void CommCgiProperties::getBinary( const BYTE*& p, size_t& sz, CommCgiProperties::iterator iter ) const
{
	PASSERT3( (*iter).second.isBinary );
	p = (*iter).second.b.ptr();
	sz = (*iter).second.b.size() - 1;
}

const char* CommCgiProperties::getString( const char* name, const char* defaultValue ) const
{
	iterator iter = nameBegin( name );
	if( isNameEnd( iter, name ) )
		return defaultValue;
	return getString( iter );
}

void CommCgiProperties::getStringTrim( const char* name, PString & ret ) const
{
	const char * r = getString(name);
	if (r)
	{
		ret.assign(PString::ltrim(r),PString::rtrim(r));
	}
	else
		ret="";
}

void CommCgiProperties::getBinary( const char* name,  const BYTE*& p, size_t& sz) const
{
	iterator iter = nameBegin( name );
	if( isNameEnd( iter, name ) )
	{
		sz = 0;
		return;
	}
	getBinary(p, sz, iter );
}

const char* CommCgiProperties::getFileName( const char* name ) const
{
	iterator iter = nameBegin( name );
	if( isNameEnd( iter, name ) )
		return 0;
	return getFileName( iter );
}

int CommCgiProperties::getInt( const char* name, int defaultValue ) const
{
	iterator iter = nameBegin( name );
	if( isNameEnd( iter, name ) )
		return defaultValue;
	return getInt( iter );
}

INT64 CommCgiProperties::getInt64( const char* name, int defaultValue ) const
{
	iterator iter = nameBegin( name );
	if( isNameEnd( iter, name ) )
		return defaultValue;
	return getInt64( iter );
}

bool CommCgiProperties::getBool( const char* name, bool defaultValue ) const
{
	iterator iter = nameBegin( name );
	if( isNameEnd( iter, name ) )
		return defaultValue;
	return getBool( iter );
}

void CommCgiProperties::insert( const char* name, const char* value )
{
	_Item val;
	val.isBinary = false;
	val.s = value;
	props.insert( PropsType::value_type( name, val ) );
}

PBlock& CommCgiProperties::insertBinary( const char* name, const char * fileName )
{
	_Item val;
	val.isBinary = true;
	PropsType::iterator it = props.insert( PropsType::value_type( name, val ) ).first;
	if (fileName)
		(*it).second.fileName = fileName;
	return (*it).second.b;
}

//****************************************************************************

void CommCgi::_parseQuery( const char* query )
{
	if( !query )
		throw PInternalError( "CommCgi: No query string" );
	queryStr = query;
	for( const char* p = query; p && *p ; )
	{
		const char* eq = strchr( p, '=' );
		const char* amp = strchr( p, '&' );
		PString name;
		if( !eq )
			throw PInternalError( "CommCgi URL: no '=' sign" );
		if( !amp )
		{
			//name.append( p, eq );
			CommCgi::urlDecode( name, p, eq );
			PString encValue;
			CommCgi::urlDecode( encValue, eq + 1 );
			queryProps.insert( name, encValue );
			p = 0;
		}
		else if( eq > amp )
			throw PInternalError( "CommCgi URL: no '=' sign" );
		else
		{
			//name.append( p, eq );
			CommCgi::urlDecode( name, p, eq );
			PString encValue;
			CommCgi::urlDecode( encValue, eq + 1, amp );
			queryProps.insert( name, encValue );
			p = amp + 1;
		}
	}
}

void CommCgi::_parseCookie( const char* cookie )
{
	if( !cookie )
		return;
	for( const char* p = cookie; p ; )
	{
		while( *p == ' ' || *p == '\t' )
			++p;
		if( *p == 0 )
			break;
		const char* eq = strchr( p, '=' );
		const char* scol = strchr( p, ';' );
		if( !eq )
		{
			if( scol )
			{
				PString name( p, scol );
				cookieProps.insert( name, "" );
				p = scol + 1;
			}
			else
			{
				cookieProps.insert( p, "" );
				p = 0;
			}
		}
		else if( !scol )
		{
			PString name( p, eq );
			const char* value = eq + 1;
			const char* end = value + strlen( value );
			while( end[ -1 ] == ' ' || end[ -1 ] == '\t' )
				--end;
			PASSERT3( end >= value );
			PString encValue;
			CommCgi::urlDecode( encValue, value, end );
			cookieProps.insert( name, encValue );
			p = 0;
		}
		else if( eq > scol )
		{
			PString name( p, scol );
			cookieProps.insert( name, "" );
			p = scol + 1;
		}
		else
		{
			PString name( p, eq );
			PString encValue;
			CommCgi::urlDecode( encValue, eq + 1, scol );
			cookieProps.insert( name, encValue );
			p = scol + 1;
		}
	}
}

#if 0
inline void _blockAppend( PBlock& block, int c )
{
	*block.append( 1 ) = c;
}

inline void _blockAppend( PBlock& block, const void* p, int n )
{
	memcpy( block.append( n ), p, n );
}

/*
static FILE* debugF = 0;
static void _open()//DEBUG
{
debugF = fopen( "qqq", "rb" );
}
*/

inline int _readC()
{
	int c = fgetc( stdin );
	//if( !debugF )
	//	_open();
	//int c = fgetc( debugF );

	if( c < 0 )
		throw PInternalError( "CommCgi: Unexpected end of input stream" );
	return c;
}

inline void _readC( char cc )
{
	int c = _readC();
	if( c != cc )
		throw PInternalError( PString( "CommCgi multipart: '" ).append( cc ).append( "' expected" ) );
}

static void _skipFirstBlock( const char* boundary )
{
	static const char* delim = "--";
	int nFit = 0;
	for(;;)
	{
		int c = _readC();
		if( nFit < 2 )
		{
			if( c == delim[ nFit ] )
				++nFit;
		}
		else
		{
			if( c != boundary[ nFit - 2 ] )
				nFit = 0;
			else if( boundary[ ++nFit - 2 ] == 0 )
				break;
		}
	}

	_readC( '\r' );
	_readC( '\n' );
}

static bool _readBlock( PBlock& block, const char* boundary )
{
	static const char* delim = "\r\n--";
	int nFit = 0;
	int cc = -1;
	for(;;)
	{
		int c;
		if( cc >= 0 )
		{
			c = cc;
			cc = -1;
		}
		else
			c = _readC();

		if( nFit == 0 )
		{
			if( c != '\r' )
				_blockAppend( block, c );
			else
				nFit = 1;
		}
		else if( nFit < 4 )
		{
			if( c != delim[ nFit ] )
			{
				_blockAppend( block, delim, nFit );
				cc = c;
				nFit = 0;
			}
			else
				++nFit;
		}
		else
		{
			if( c != boundary[ nFit - 4 ] )
			{
				_blockAppend( block, delim, nFit );
				_blockAppend( block, boundary, nFit - 4 );
				cc = c;
				nFit = 0;
			}
			else if( boundary[ ++nFit - 4 ] == 0 )
				break;
		}
	}

	_blockAppend( block, 0 );//sic!
	int c = _readC();
	if( c == '\r' )
	{
		_readC( '\n' );
		return false;
	}
	else if( c == '-' )
	{
		_readC( '-' );
		_readC( '\r' );
		_readC( '\n' );
		return true;
	}
	else
		throw PInternalError( "CommCgi multipart: unexpected symbol after boundary" );
}
#endif

struct _CommCgiRd//for _CommMimeMultipart
{
	int read() { return fgetc( stdin ); }
};

CommCgi::CommCgi( int allowedMethods, const char* noCgiQuery )
{
	if( noCgiQuery )
	{
		_parseQuery( noCgiQuery );
		methodId = GET;
	}
	else
	{
		const char* method = getenv( "REQUEST_METHOD" );
		//method = "POST";//DEBUG
		if( !method )
			throw PInternalError( "CommCgi: REQUEST_METHOD expected" );

		if( strcmp( method, "GET" ) == 0 )
		{
			if( ( allowedMethods & GET ) == 0 )
				throw PInternalError( "CommCgi: GET is not allowed" );
			methodId = GET;
			_parseQuery( getenv( "QUERY_STRING" ) );
		}
		else if( strcmp( method, "HEAD" ) == 0 )
		{
			if( ( allowedMethods & GET ) == 0 )
				throw PInternalError( "CommCgi: HEAD is not allowed" );
			methodId = HEAD;
			_parseQuery( getenv( "QUERY_STRING" ) );
		}
		else if( strcmp( method, "POST" ) == 0 )
		{
			const char* contentType = getenv( "CONTENT_TYPE" );
			//contentType = "multipart/form-data; boundary=---------------------------198713071217322";//DEBUG
			if( !contentType )
				throw PInternalError( "CommCgi: CONTENT_TYPE expected" );

			PString type;
			_CommMime::getHdrValue( type, contentType );
			if( PString::compareIgnoreCase( type, "multipart/form-data" ) == 0 )
			{
				if( ( allowedMethods & POSTMULTIPART ) == 0 )
					throw PInternalError( "CommCgi: POSTMULTIPART is not allowed" );
				methodId = POSTMULTIPART;

				PString boundary;
				for(;;)
				{
					if( !*contentType )
						break;

					PString name;
					PString value;
					_CommMime::getHdrAttr( name, value, contentType );
					if( PString::compareIgnoreCase( name, "boundary" ) == 0 )
						boundary = value;
				}

				if( !*boundary )
					throw PInternalError( "CommCgi multipart: boundary not found in Content-Type" );

				_SETSTDINBINARYMODE();
				//_open();
				_CommCgiRd rd;
				_CommMimeMultipart< _CommCgiRd > multipart( rd, boundary );

				multipart.skipFirstBlock();
				for(;;)
				{
					PString name;
					PString fileName;
					for(;;)
					{
						PString line;
						multipart.readHeaderLine( line );
						if( line.length() == 0 )
							break;
						const char* ln = line;
						PString type;
						_CommMime::getHdrName( type, ln );
						if( PString::compareIgnoreCase( type, "Content-Disposition" ) == 0 )
						{
							type = "";
							_CommMime::getHdrValue( type, ln );
							for(;;)
							{
								if( !*ln )
									break;

								PString attrName;
								PString attrValue;
								_CommMime::getHdrAttr( attrName, attrValue, ln );
								if( PString::compareIgnoreCase( attrName, "name" ) == 0 )
									name = attrValue;
								else if( PString::compareIgnoreCase( attrName, "filename" ) == 0 )
									fileName = attrValue;

							}
						}
					}
					if( !*name )
						throw PInternalError( "CommCgi multipart: name not found" );
					PBlock& block = queryProps.insertBinary( name, fileName );
					bool isLast = multipart.readBlock( block );
					*block.append( 1 ) = 0;
					if( isLast )
						break;
				}
			}
			else
			{
				if( ( allowedMethods & POST ) == 0 )
					throw PInternalError( "CommCgi: POST is not allowed" );
				methodId = POST;
				const char* contentLength = getenv( "CONTENT_LENGTH" );
				if( !contentLength )
					throw PInternalError( "CommCgi: CONTENT_LENGTH expected" );

				int len = atoi( contentLength );
				PString qStr;
				for( int i=0; i < len ; ++i )
				{
					int c = fgetc( stdin );
					if( c <= 0 )
						throw PInternalError( "CommCgi: Unexpected end of input stream" );
					qStr.append( (char)c );
				}
				_parseQuery( qStr );
			}
		}
		else
			throw PInternalError( "CommCgi: Unknown REQUEST_METHOD" );

		_parseCookie( getenv( "HTTP_COOKIE" ) );
	}

#if 0
	agentId = 0;
	const char* userAgent = getenv( "HTTP_USER_AGENT" );
	if( userAgent )
	{
		PString ua;
		for( const char* p = userAgent; *p ; ++p )
			ua += toupper( *p );
		const char* agentVer = 0;
		const char* moz = strstr( ua, "MOZILLA" );
		if( moz )
		{
			const char* comp = strstr( ua, "COMPATIBLE" );
			if( comp )
			{
				const char* webtv = strstr( ua, "WEBTV" );
				if( webtv )
				{
					agentId = 'W';
					agentVer = webtv + 5/*strlen( "WEBTV" )*/;
				}
				else
				{
					const char* msie = strstr( ua, "MSIE" );
					if( msie )
					{
						agentId = 'M';
						agentVer = msie + 4/*strlen( "MSIE" )*/;
					}
				}
			}
			else
			{
				agentId = 'N';
				agentVer = moz + 7/*strlen("MOZILLA")*/;
			}
		}

		agentMajor = 0;
		agentMinor = 0;
		if( agentVer )
		{
			for(; *agentVer && !isdigit( *agentVer ) ;)
				++agentVer;
			if( *agentVer )
			{
				char* end;
				agentMajor = strtoul( agentVer, &end, 10 );
				if( *end == '.' )
					agentMinor = strtoul( end + 1, 0, 10 );
			}
		}
		if( agentMajor == 0 )
			agentId = 0;
	}
#endif//0
}

//****************************************************************************

CommCgiReply::CommCgiReply( bool headers_ )
{
	cgi = 0;
	headers = headers_;
	_init();
}

CommCgiReply::CommCgiReply( CommCgi& cgi_, bool headers_ )
{
	cgi = &cgi_;
	headers = headers_;
	_init();
}

void CommCgiReply::setStatus( int status_, const char* msg )
{
	status = status_;
	statusMsg = msg;
}

void CommCgiReply::setLocation( const char* location_ )
{
	location = location_;
}

void CommCgiReply::setAbsLocation( const char* server, const char* file, const char* queryString, bool isHttps )
{
	location = isHttps ? "https://":"http://";
	location.append( server ).append( file );
	if( queryString )
		location.append( '?' ).append( queryString );
}

void CommCgiReply::setContentTypeHtml( const char* charset )
{
	contentType = "text/html; charset=";
	contentType.append( charset );
}

void CommCgiReply::setContentType( const char* s, const char* charset )
{
	contentType = s;
	if( charset && *charset )
		contentType.append( "; charset=" ).append( charset );
}

void CommCgiReply::addHeader( const char* name, const char* value )
{
	headerNames.push_back( name );
	headerValues.push_back( value );
}

void CommCgiReply::setBinaryContent( const BYTE* binaryBody_, size_t binaryBodySz_ )
{
	binaryBody = binaryBody_;
	binaryBodySz = binaryBodySz_;
}

void CommCgiReply::setContent( const char* body_ )
{
	body = body_;
}

void CommCgiReply::_init()
{
	status = 0;
	contentType = "text/plain";
	body = 0;
	binaryBody = 0;
}

void CommCgiReply::addCookie( const char* name, const char* value, time_t expires, const char* path, const char* domain, bool secure )
{
	static const char* _wdays[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
	static const char* _months[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
	PString cookie = name;
	if( value )
	{
		cookie.append( '=' );
		CommCgi::urlEncode( cookie, value );
	}
	if( expires )
	{
		if( expires < 0 )
			cookie.append( "; expires=Tue, 01-Jan-1980 00:00:00 GMT" );
		else
		{
			struct tm* tm = gmtime( &expires );
			PASSERT3( tm );
			char buf[ 64 ];
			sprintf( buf, "; expires=%s, %02d-%s-%04d %02d:%02d:%02d GMT", _wdays[ tm->tm_wday ], tm->tm_mday, _months[ tm->tm_mon ], tm->tm_year + 1900, tm->tm_hour, tm->tm_min, tm->tm_sec );
			cookie.append( buf );
		}
	}
	if( path )
		cookie.append( "; path=" ).append( path );
	if( domain )
		cookie.append( "; domain=" ).append( domain );
	if( secure )
		cookie.append( "; secure; HTTPOnly" );
	addHeader( "Set-Cookie", cookie );
}

void CommCgiReply::out()
{
	_SETSTDOUTBINARYMODE();
	if( headers )
	{
		if( location.length() )
		{
			PASSERT3( status == 0 );
			PASSERT3( body == 0 );

			printf( "Location: %s\n", location.c_str() );
			_printExtraHdrs();
			printf( "\n" );
			return;
		}

		if( status )
			printf( "Status: %d %s\n", status, statusMsg.c_str() );
		printf( "Content-Type: %s\n", contentType.c_str() );

		_printExtraHdrs();

		//if( body )
		//printf( "Content-Length: %d\n", buf.length() );//excluded due to netscape problems
		printf( "\n" );
	}
	if( status != 0 || !cgi || cgi->method() != CommCgi::HEAD )
	{
		if( body )
			printf( "%s", body );
		else if( binaryBody )
			fwrite( binaryBody, binaryBodySz, 1, stdout );
	}
}

void CommCgiReply::_printExtraHdrs()
{
	int n = headerNames.size();
	for( int i=0; i < n ; ++i )
		printf( "%s: %s\n", headerNames[ i ].c_str(), headerValues[ i ].c_str() );
}
