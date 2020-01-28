#include "commhtml.h"

static inline bool _isWS( int c )
{
	return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

class _CommHtmlEntities
{
public:
	class Item
	{
	public:
		int expansion;

	private:
		int refStart;
		int refEnd;
		UINT16* ref;

	public:
		Item()
		{
			expansion = 0;
			refStart = refEnd = 0;
			ref = 0;
		}
		Item( const Item& other )
		{
			_copyFrom( other );
		}
		Item& operator =( const Item& other )
		{
			_del();
			_copyFrom( other );
			return *this;
		}
		~Item()
		{
			_del();
		}

	public:
		void _assign( int c, int i );
		int _index( int c ) const
		{
			return c >= refStart && c < refEnd ? ref[ c - refStart ] : 0;
		}

	private:
		void _copyFrom( const Item& other )
		{
			expansion = other.expansion;
			refStart = other.refStart;
			refEnd = other.refEnd;
			if( refEnd != refStart )
			{
				int sz = ( refEnd - refStart )*sizeof(UINT16);
				ref = (UINT16*)malloc( sz );
                PASSERT3(ref);
				memcpy( ref, other.ref, sz );
			}
			else
				ref = 0;
		}
		void _del()
		{
			if( ref )
				free( ref );
		}
	};

public:
	int maxLen;
private:
	vector< Item > items;

public:
	_CommHtmlEntities();
	bool isEmpty() const
	{
		return items.size() <= 0;
	}
	const Item& root() const
	{
		PASSERT3( items.size() > 0 );
		return items[ 0 ];
	}
	const Item* leaf( const Item& item, int c ) const
	{
		int idx = item._index( c );
		PASSERT3( idx < items.size() );
		return idx ? &items[ idx ] : 0;
	}

private:
	void _append( const char* name, int value );
};

void _CommHtmlEntities::Item::_assign( int c, int i )
{
	if( refStart == refEnd )
	{
		ref = (UINT16*)malloc( sizeof(UINT16) );
		refStart = c;
		refEnd = refStart + 1;
	}
	else if( c < refStart )
	{
		UINT16* newRef = (UINT16*)malloc( ( refEnd - c )*sizeof(UINT16) );
        PASSERT3(newRef);
		memset( newRef, 0, ( refStart - c )*sizeof(UINT16) );
		memcpy( newRef + ( refStart - c ), ref, ( refEnd - refStart )*sizeof(UINT16) );
		free( ref );
		ref = newRef;
		refStart = c;
	}
	else if( c >= refEnd )
	{
		UINT16* tmp = (UINT16*)realloc( ref, ( c + 1 - refStart )*sizeof(UINT16) ); // `tmp` is used to avoid leaking memory if `realloc` fails (in which case 0 is returned)
        PASSERT3(tmp);
        ref = tmp;
		memset( ref + ( refEnd - refStart ), 0, ( c + 1 - refEnd )*sizeof(UINT16) );
		refEnd = c + 1;
	}
	ref[ c - refStart ] = (UINT16)i;
}

void _CommHtmlEntities::_append( const char* name, int value )
{
	int len = strlen( name );
	if( len > maxLen )
		maxLen = len;
	int idx = 0;
	for( const char* p = name; *p ; ++p )
	{
		Item& item = items[ idx ];
		int newIdx =  item._index( *p );
		if( !newIdx )
		{
			items.push_back( Item() );
			newIdx = items.size() - 1;
			items[ idx ]._assign( *p, newIdx );
		}
		idx = newIdx;
	}
	items[ idx ].expansion = value;
}

static struct { const char* name; int value; } _entities[] = 
{
	{ "amp", 38 },
	{ "lt", 60 },
	{ "gt", 62 },
	{ "quot", 34 },
	{ "nbsp", 160 },
	{ 0, 0 }
};

_CommHtmlEntities::_CommHtmlEntities()
{
	maxLen = 0;
	int i;

	items.erase( items.begin(), items.end() );
	items.push_back( Item() );

	for( i=0; _entities[ i ].name ; ++i )
		_append( _entities[ i ].name, _entities[ i ].value );

	for( i=0; i < 256 ; ++i )
	{
		char buf[ 16 ];
		sprintf( buf, "#%d", i );
		_append( buf, i );
		sprintf( buf, "#X%X", i );
		_append( buf, i );
	}
}

class _CommHtmlByteStream
{
private:
	const BYTE* p;
	size_t sz;
	size_t pos;

public:
	_CommHtmlByteStream( const BYTE* p_, size_t sz_ )
	{
		p = p_;
		sz = sz_;
		pos = 0;
	}

	int read()
	{
		if( pos >= sz )
			return -1;
		return p[ pos++ ];
	}
};

class _CommHtmlParserStream
{
private:
	_CommHtmlByteStream& s;
	BYTE buf[ 32 ];
	BYTE* bufPtr;
	BYTE* bufEnd;

public:
	_CommHtmlParserStream( _CommHtmlByteStream& s_ )
		: s( s_ )
	{
		bufPtr = bufEnd = buf;
	}

public:
	int get()
	{
		if( bufPtr == bufEnd )
		{
			//int ret = s.read();
			//printf( "%c", ret );
			//return ret;
			return s.read();
		}
		int ret = *bufPtr;
		if( ++bufPtr == buf + sizeof(buf) )
			bufPtr = buf;
		return ret;
	}
	void unget( int b )
	{
		if( b < 0 )
			return;
		*bufEnd = b;
		if( ++bufEnd == buf + sizeof(buf) )
			bufEnd = buf;
		PASSERT3( bufEnd != bufPtr );
	}
	void unget( const BYTE* begin, const BYTE* end )
	{
		for( const BYTE* p = begin ; p < end ; ++p )
			unget( *p );
	}
	int _skipWS()
	{
		for(;;)
		{
			int c = get();
			if( !_isWS( c ) )
				return c;
		}
	}
	void _skipUntil( BYTE c )
	{
		for(;;)
		{
			int cc = get();
			if( cc < 0 || cc == c )
				return;
		}
	}
	void _skipUntil( const char* pattern );
};

void _CommHtmlParserStream::_skipUntil( const char* pattern )
{
	int n = strlen( pattern );
	PBlock buf( n );
	BYTE* ptr = buf.ptr();
	for( int i=0; i < n ; ++i )
	{
		int c = get();
		if( c < 0 )
			return;
		ptr[ i ] = (BYTE)c;
	}
	BYTE* p = ptr;

	for(;;)
	{
		int nn = ptr + n - p;
		if( memcmp( p, pattern, nn ) == 0 &&
			memcmp( ptr, pattern + nn, n - nn ) == 0 )
			return;
		int c = get();
		if( c < 0 )
			return;
		*p = (BYTE)c;
		if( ++p >= ptr + n )
			p = ptr;
	}
}

class _CommHtmlEntityFinder
{
public:
	const _CommHtmlEntities& _htmlEntities;
	const _CommHtmlEntities::Item* entity;
	BYTE buf[ 16 + 1 ];//16 - limit for named entity length
	BYTE* bufEnd;
	int found;
	int foundLen;

	_CommHtmlEntityFinder( const _CommHtmlEntities& htmlEntities )
		: _htmlEntities( htmlEntities )
	{
		PASSERT3( _htmlEntities.maxLen < sizeof(buf) );
		found = 0;
		foundLen = 0;
		entity = _htmlEntities.isEmpty() ? 0 : &_htmlEntities.root();
		bufEnd = buf;
	}
	bool push( BYTE c );
	void eof();
};

bool _CommHtmlEntityFinder::push( BYTE c )
{
	if( entity == 0 )
		return false;
	*bufEnd++ = (BYTE)c;
	if( buf[ 0 ] == '#' )
		c = toupper( c );//for numeric character references
	PASSERT3( bufEnd < buf + sizeof(buf) );
	const _CommHtmlEntities::Item* newEntity = _htmlEntities.leaf( *entity, c );
	if( newEntity )
	{
		entity = newEntity;
		if( entity->expansion )
		{
			found = entity->expansion;
			foundLen = bufEnd - buf;
		}
		//no 'emptyEntity' optimization allowed (*)
		return true;
	}
	else
		return false;
}

void _CommHtmlEntityFinder::eof()
{
	if( entity == 0 )
		return;
	if( found )
	{
		PASSERT3( foundLen <= bufEnd - buf );
		if( buf[ foundLen ] == ';' )
			++foundLen;//(*)
	}
}

CommHtmlParser::CommHtmlParser( const BYTE* p_, size_t sz_ )
{
	htmlEntities = new _CommHtmlEntities();
	byteStream = 0;
	parserStream = 0;
	p = p_;
	sz = sz_;
	bufPos = buf;
	doUppercase = true;
}

CommHtmlParser::~CommHtmlParser()
{
	delete parserStream;
	delete byteStream;
	delete htmlEntities;
}

inline void CommHtmlParser::_flush()
{
	if( bufPos != buf )
	{
		characters( buf, bufPos );
		bufPos = buf;
	}
}

inline void CommHtmlParser::_character( char c )
{
	*bufPos++ = c ? c : ' ';
	if( bufPos == buf + sizeof(buf) )
	{
		characters( buf, bufPos );
		bufPos = buf;
	}
}

inline void CommHtmlParser::_eof()
{
	_flush();
	eof();
}

inline void CommHtmlParser::_tag( const char* name, const PStringMap< PString >* attrs )
{
	_flush();
	tag( name, attrs );
}

void CommHtmlParser::parse()
{
	delete byteStream;
	byteStream = new _CommHtmlByteStream( p, sz );
	delete parserStream;
	parserStream = new _CommHtmlParserStream( *byteStream );
	_parse( *parserStream );
}

void CommHtmlParser::_parse( _CommHtmlParserStream& s )
{
	_stopParse = false;
	for(;;)
	{
		int ch = -1;
		int c = s.get();
		if( c < 0 )
		{
			_eof();
			break;//forever
		}

		if( c == '&' )
		{
			_CommHtmlEntityFinder entity( *htmlEntities );
			for(;;)
			{
				int c = s.get();
				if( c < 0 )
					break;
				if( !entity.push( c ) )
					break;
			}
			entity.eof();
			if( entity.found )
			{
				ch = entity.found;
				s.unget( entity.buf + entity.foundLen, entity.bufEnd );
			}
			else
			{
				ch = '&';
				s.unget( entity.buf, entity.bufEnd );
			}
		}
		else if( c == '<' )
		{
			c = s.get();
			if( c == '!' )
			{
				c = s.get();
				if( c == '-' )
				{
					c = s.get();
					if( c == '-' )
						s._skipUntil( "-->" );
					else
						s._skipUntil( '>' );
				}
				else
					s._skipUntil( '>' );
			}
			else
			{
				s.unget( c );
				PString tagName;
				bool tagEnd = false;
				for(;;)//forever #1.1
				{
					c = s.get();
					if( c < 0 )
					{
						_tag( tagName, 0 );
						tagEnd = true;
						break;
					}
					else if( c == '<' )
					{
						s.unget( '<' );
						_tag( tagName, 0 );
						tagEnd = true;
						break;
					}
					else if( c == '>' )
					{
						_tag( tagName, 0 );
						tagEnd = true;
						break;
					}
					else if( _isWS( c ) )
						break;
					else
						tagName.append( (char)c );
				}//forever #1.1

				if( tagEnd )
				{
					if( _stopParse )
						break;//forever
					else
						continue;//forever
				}

				PStringMap< PString > attrs;
				for(;;)//forever #1.2
				{
					c = s._skipWS();
					if( c == '<' )
					{
						s.unget( '<' );
						break;
					}
					else if( c < 0 || c == '>' )
						break;
					s.unget( c );

					PString attrName;
					bool tagEnd = false;
					for(;;)
					{
						c = s.get();
						if( _isWS( c ) || c == '=' )
							break;
						else if( c == '<' )
						{
							s.unget( '<' );
							attrs.insert( PStringMap< PString >::value_type( attrName, "" ) );
							tagEnd = true;
							break;
						}
						else if( c < 0 || c == '>' )
						{
							attrs.insert( PStringMap< PString >::value_type( attrName, "" ) );
							tagEnd = true;
							break;
						}
						else
						{
							if( doUppercase )
								attrName.append( (char)toupper( c ) );
							else
								attrName.append( (char)c );
						}
					}
					if( tagEnd )
						break;//sic!

					s.unget( c );
					c = s._skipWS();
					if( c != '=' )
					{
						attrs.insert( PStringMap< PString >::value_type( attrName, "" ) );
						s.unget( c );
						continue;
					}

					c = s._skipWS();
					char delim = ' ';
					PString attrValue;
					if( c == '\"' )
						delim = '\"';
					else if( c == '\'' )
						delim = '\'';
					else
						s.unget( c );
					tagEnd = false;
					for(;;)
					{
						c = s.get();
						if( c == '<' )
						{
							s.unget( '<' );
							tagEnd = true;
							break;
						}
						if( c < 0 || c == '>' )
						{
							tagEnd = true;
							break;
						}
						else if( c == delim )
							break;
						else if( delim == ' ' && _isWS( c ) )
							break;
						else if( c != '\r' || c != '\n' && c != '\t' )
							attrValue.append( (char)c );
					}

					const char* amp = strchr( attrValue, '&' );
					if( amp )
					{
						PString newVal;
						for( const char* p = attrValue ; *p ; )
						{
							if( *p == '&' )
							{
								_CommHtmlEntityFinder entity( *htmlEntities );
								const char* pStart = ++p;
								for(;;)
								{
									int c = *p++;
									if( c == 0 )
										break;
									if( !entity.push( c ) )
										break;
								}
								entity.eof();
								if( entity.found )
								{
									newVal.append( (char)entity.found );
									p = pStart + entity.foundLen;
								}
								else
								{
									newVal.append( '&' );
									p = pStart;
								}
							}
							else
								newVal.append( *p++ );
						}
						attrValue = newVal;
					}

					attrs.insert( PStringMap< PString >::value_type( attrName, attrValue ) );
					if( tagEnd )
						break;//sic!
				}//forever #1.2
				_tag( tagName, &attrs );
			}//if( '<' )
		}//c == '<'
		else
			ch = c;

		if( ch >= 0 )
			_character( (char)ch );

		if( _stopParse )
			break;//forever
	}//forever
}

void CommHtmlParser::stopParse()
{
	_stopParse = true;
}

void CommHtmlParser::recursiveContinueParse()
{
	bool oldStopParse = _stopParse;
	_parse( *parserStream );
	_stopParse = oldStopParse;
}
