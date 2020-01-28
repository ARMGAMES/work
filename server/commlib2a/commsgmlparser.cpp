#include "commsgml.h"
#include "pplogfile.h"
#include "ppcheck.h"

CommSgmlTagAttributes::CommSgmlTagAttributes()
{
}

CommSgmlTagAttributes::~CommSgmlTagAttributes()
{
	int n = items.size();
	for( int i=0; i < n ; ++i )
	{
		_Item& it = items[ i ];

		::free( it.name );
		if( it.value )
			::free( it.value );
	}
}

void CommSgmlTagAttributes::_add( const char* name, const char* value )
{
	PASSERT4( name );
	_Item& it = *items.insert( items.end(), _Item() );
	it.found = false;
	it.name = strdup( name );
	it.value = value ? strdup( value ) : NULL;
}

pair< bool, const char* > CommSgmlTagAttributes::find( const char* name, const PStringEncoding* encString ) const
{
	int n = items.size();
	for( int i=0; i < n ; ++i )
	{
		if( encString->compare( items[ i ].name, name ) == 0 )
		{
			( const_cast<_Item&>(items[ i ]) ).found = true;//found is in fact mutable
			return pair< bool, const char* >( true, items[ i ].value );
		}
	}
	return pair< bool, const char* >( false, (const char *) NULL );
}

pair< bool, const char* > CommSgmlTagAttributes::findIgnoreCase( const char* name, const PStringEncoding* encString ) const
{
	int n = items.size();
	for( int i=0; i < n ; ++i )
	{
		if( encString->compareIgnoreCase( items[ i ].name, name ) == 0 )
		{
			( const_cast<_Item&>(items[ i ]) ).found = true;//found is in fact mutable
			return pair< bool, const char* >( true, items[ i ].value );
		}
	}
	return pair< bool, const char* >( false, (const char *) NULL );
}

void CommSgmlTagAttributes::warnAboutUnused( CommSgmlWarnings *sgmlWarnings, int line ) const
{
	if( sgmlWarnings )
	{
		int n = items.size();
		for( int i=0; i < n ; ++i )
		{
			const _Item& it = items[ i ];

			if( !it.found )
				sgmlWarnings->warning( line, PString( "unused attribute '" ).append( it.name ).append( "' - ignored" ) );
		}
	}
}

void CommSgmlTagAttributes::enumerate( vector< pair< const char*, const char* > >& buf ) const
{
	int n = items.size();
	for( int i=0; i < n ; ++i )
		buf.insert( buf.end(), make_pair( items[ i ].name, items[ i ].value ) );
}

void CommSgmlTagAttributes::enumerate( vector< pair< PString, PString > >& buf ) const
{
	int n = items.size();
	for( int i=0; i < n ; ++i )
		buf.insert( buf.end(), make_pair( items[ i ].name, items[ i ].value ) );
}

//****************************************************************************

//+++ warnings
//+++ XML end-tag
//+++ emit comments when necessary

struct _CommSgmlNamedEntity
{
	const char* name;
	PUNICHAR value;

	bool operator <( const _CommSgmlNamedEntity& other ) const
	{
		if( name[ 0 ] != other.name[ 0 ] )
			return name[ 0 ] < other.name[ 0 ];
		return name[ 1 ] < other.name[ 1 ]; // sic - 1st 2 symbols only, see below
	}
};

const static _CommSgmlNamedEntity _entities[] =
//MUST be sorted
{//Sgml4.01 24.2 - 24.4
	//+++
	{ "amp", 38 },
	{ "apos", 39 },
	{ "copy", 169 },
	{ "gt", 62 },
	{ "lt", 60 },
	{ "nbsp", 160 },
	{ "quot", 34 },
	{ "reg", 174 },
	{ "trade", 8482 },
	{ "zwsp", 8203 }, // !!! NON-STANDARD: ZERO WIDTH SPACE !!!
};
const static _CommSgmlNamedEntity* _entitiesEnd = _entities + sizeof(_entities)/sizeof(_entities[ 0 ]);

class _CommSgmlEntityFinder
{
public:
	enum { MaxCharEntityLength = 16 };
	enum { _bufSz = MaxCharEntityLength };

private:
	char namedBuf[ _bufSz ];
	int namedBufLen;
	const _CommSgmlNamedEntity* named;
	int mode;//-1 - error, 0 - none, 1 - char, 2 - num, 3 - numDec, 4 - numHex0, 5 - numHex+

public:
	PUNICHAR_RET found;
	int foundLen;
	PUNICHAR restoreBuf[ _bufSz ];
	PUNICHAR* restoreBufEnd;

	_CommSgmlEntityFinder()
	{
		mode = 0;
		named = NULL;
		found = -1;
		foundLen = 0;
		restoreBufEnd = restoreBuf;
	}
	bool push( PUNICHAR c );
	void end();

private:
	/* inline */ void _clearRestoreBuf();
	/* inline */ void _addToRestoreBuf( PUNICHAR c );
};

inline void _CommSgmlEntityFinder::_clearRestoreBuf()
{
	restoreBufEnd = restoreBuf;
}

inline void _CommSgmlEntityFinder::_addToRestoreBuf( PUNICHAR c )
{
	*restoreBufEnd = c;
	++restoreBufEnd;
	PASSERT4( restoreBufEnd - restoreBuf < _bufSz );
}

bool _CommSgmlEntityFinder::push( PUNICHAR c )
{
	switch( mode )
	{
	case 0:
		if( c == '#' )
		{
			_addToRestoreBuf( c );
			mode = 2;
			return true;
		}
		else if( c > 127 )//ANSI only in character entities now
		{
			_addToRestoreBuf( c );
			mode = -1;
			return false;
		}
		else
		{
			PASSERT3( named == NULL );
			namedBuf[ 0 ] = ( char )PString::toLower( ( BYTE )c );//cast Ok as c < 127 (**)
			namedBuf[ 1 ] = 0;
			namedBufLen = 1;
			_CommSgmlNamedEntity item;
			item.name = namedBuf;
			_DEBUG_CHECK_TABLE_ORDER( _entities, _entitiesEnd );
			const _CommSgmlNamedEntity* it = lower_bound( _entities, _entitiesEnd, item );
			if( it != _entitiesEnd && ((*it).name)[ 0 ] == namedBuf[ 0 ] )
			{
				named = it;
				if( named->name[ 1 ] == 0 )
				{
					found = named->value;
					foundLen = 1;
					_clearRestoreBuf();
				}
				else
					_addToRestoreBuf( c );
				mode = 1;
				return true;
			}
			else
			{
				_addToRestoreBuf( c );
				mode = -1;
				return false;
			}
		}
		break;

	case 1:
		{
			if( c > 127 )//ANSI only in character entities now
			{
				_addToRestoreBuf( c );
				mode = -1;
				return false;
			}

			PASSERT3( named );
			namedBuf[ namedBufLen++ ] = ( char )PString::toLower( ( BYTE )c );//cast Ok as c < 127 (**)
			if( named->name[ namedBufLen - 1 ] == namedBuf[ namedBufLen - 1 ] )
			{
				if( named->name[ namedBufLen ] == 0 )
				{
					found = named->value;
					foundLen = namedBufLen;
					_clearRestoreBuf();
				}
				else
					_addToRestoreBuf( c );
				//mode still = 1
				return true;
			}
			else
			{
				for( ++named; named < _entitiesEnd ; ++named )
				{
					int cmp = PString::compareIgnoreCase( namedBuf, named->name, namedBufLen );
					if( cmp == 0 )
					{
						if( named->name[ namedBufLen ] == 0 )
						{
							found = named->value;
							foundLen = namedBufLen;
							_clearRestoreBuf();
						}
						else
							_addToRestoreBuf( c );
						//mode still = 1
						return true;
					}
					else if( cmp < 0 )
					{
						_addToRestoreBuf( c );
						mode = -1;
						return false;
					}
				}
				_addToRestoreBuf( c );
				mode = -1;
				return false;
			}
		}
		break;

	case 2://'#'
		{
			if( c == 'x' || c == 'X' )
			{
				_addToRestoreBuf( c );
				mode = 4;
				return true;
			}
			else if( c >= '0' && c <= '9' )
			{
				found = (int)c - (int)'0';
				foundLen = 2;
				_clearRestoreBuf();
				mode = 3;
				return true;
			}
			else
			{
				_addToRestoreBuf( c );
				mode = -1;
				return false;
			}
		}
		break;

	case 3://'#D'
		{
			if( c >= '0' && c <= '9' )
			{
				int n = (int)found * 10 + (int)c - (int)'0';
				if( n <= PUNICHAR_MAX )
				{
					found = (PUNICHAR)n;
					++foundLen;
					_clearRestoreBuf();
					// mode still = 3
					return true;
				}
				//else fall down
			}

			_addToRestoreBuf( c );
			mode = -1;
			return false;
		}
		break;

	case 4://'#x'
		{
			int n = -1;
			if( c >= '0' && c <= '9' )
				n = (int)c - (int)'0';
			else if( c >= 'a' && c <= 'z' )
				n = (int)c - (int)'a' + 10;
			else if( c >= 'A' && c <= 'Z' )
				n = (int)c - (int)'A' + 10;

			if( n >= 0 )
			{
				found = n;
				foundLen = 3;
				_clearRestoreBuf();
				mode = 5;
				return true;
			}
			else
			{
				_addToRestoreBuf( c );
				mode = -1;
				return false;
			}
		}
		break;

	case 5://'#xX'
		{
			int n = -1;
			if( c >= '0' && c <= '9' )
				n = (int)c - (int)'0';
			else if( c >= 'a' && c <= 'z' )
				n = (int)c - (int)'a' + 10;
			else if( c >= 'A' && c <= 'Z' )
				n = (int)c - (int)'A' + 10;

			if( n >= 0 )
			{
				int nn = (int)found * 16 + n;
				if( nn <= PUNICHAR_MAX )
				{
					found = (PUNICHAR)nn;
					++foundLen;
					_clearRestoreBuf();
					// mode still = 5
					return true;
				}
				//else fall down
			}
			//else fall down

			_addToRestoreBuf( c );
			mode = -1;
			return false;
		}
		break;

	default:
		PASSERT4( 0 );
		break;
	}
	return false;
}

void _CommSgmlEntityFinder::end()
{
	if( found >= 0 )
	{
		if( restoreBufEnd - restoreBuf == 1 && *restoreBuf == ';' )
		{
			++foundLen;
			--restoreBufEnd;
		}
	}
}

//****************************************************************************

CommSgmlParser::CommSgmlParser( const PStringEncoding* encString_, bool useUniCharacters, CommSgmlWarnings *sgmlWarnings_ )
{
	useUniChars = useUniCharacters;
	encString = encString_;
	sgmlWarnings = sgmlWarnings_;
	bufPos = buf;
	line = 0;//just to initialize
}

CommSgmlParser::~CommSgmlParser()
{
}

void CommSgmlParser::_flushCharacters()
{
	if( useUniChars )
		uniCharacters( buf, bufPos );
	else
	{
		PString tmp;
		//+++? tmp.reserve()
		for( const PUNICHAR* p = buf ; p < bufPos ; ++p )
			encString->append( tmp, *p );
		characters( tmp, tmp + tmp.length() );
	}
	bufPos = buf;
}

inline void CommSgmlParser::_flush()
{
	if( bufPos != buf )
		_flushCharacters();
}

inline void CommSgmlParser::_character( PUNICHAR c )
{
	PASSERT3( c != 0 );
	*bufPos++ = c;
	if( bufPos == buf + _bufSz )
		_flushCharacters();
}

inline void CommSgmlParser::_eof()
{
	_flush();
	eof();
}

inline void CommSgmlParser::_tag( const char* name, CommSgmlTagAttributes* attrs )
{
	_flush();
	tag( name, attrs );
}

//****************************************************************************

class _CommSgmlInputStream : public PUniCharInputStream
{
private:
	int& line;

public:
	_CommSgmlInputStream( PUniInputStream& s, int& line_ )
		: PUniCharInputStream( s ),
		line( line_ )
	{
		line = 1;
	}

public:
	PUNICHAR_RET get()
	{
		PUNICHAR_RET ch = PUniCharInputStream::get();
		if( ch == '\n' )
			++line;
		return ch;
	}
	void unget( PUNICHAR_RET b )
	{
		if( b == '\n' )
			--line;
		PUniCharInputStream::unget( b );
	}
};

//****************************************************************************

void CommSgmlParser::parse( PUniInputStream& uniStream )
{
	_CommSgmlInputStream ucs( uniStream, line );
	_parse( ucs );
}

static inline void _appendUniChar( PBlock& b, PUNICHAR c )
{
	memcpy( b.append( sizeof(PUNICHAR) ), &c, sizeof(PUNICHAR) );
}

static inline bool _isWS( PUNICHAR c )
{
	return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

static PUNICHAR_RET _skipWS( PUniCharInputStream& s )
{
	for(;;)
	{
		PUNICHAR_RET c = s.get();
		if( !_isWS( c ) )
			return c;
	}
}

static void _ungetString( _CommSgmlInputStream& s, const PUNICHAR* begin, const PUNICHAR* end )
{
	for( const PUNICHAR* p = end - 1; p >= begin ; --p )//LIFO order!
		s.unget( *p );
}

void CommSgmlParser::_parse( _CommSgmlInputStream& s )
{
	//_stopParse = false;
	for(;;)
	{
		PUNICHAR_RET ch = -1;
		PUNICHAR_RET c = s.get();
		if( c < 0 )
		{
			_eof();
			break;//forever
		}

		if( c == '&' )
		{
			_CommSgmlEntityFinder entity;
			for(;;)
			{
				PUNICHAR_RET c = s.get();
				if( c < 0 )
					break;
				if( !entity.push( c ) )
					break;
			}
			entity.end();
			if( entity.found >= 0 )
			{
				ch = entity.found;
				_ungetString( s, entity.restoreBuf, entity.restoreBufEnd );
			}
			else
			{
				ch = '&';
				_ungetString( s, entity.restoreBuf, entity.restoreBufEnd );
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
						PSkipUntil( s, "-->" );
					else
						PSkipUntil( s, '>' );
				}
				else
					PSkipUntil( s, '>' );
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
						_tag( tagName, NULL );
						tagEnd = true;
						break;
					}
					else if( c == '<' )
					{
						s.unget( '<' );
						_tag( tagName, NULL );
						tagEnd = true;
						break;
					}
					else if( c == '>' )
					{
						_tag( tagName, NULL );
						tagEnd = true;
						break;
					}
					else if( _isWS( c ) )
						break;
					else
						encString->append( tagName, c );
				}//forever #1.1

				if( tagEnd )
				{
					//if( _stopParse )
					//	break;//forever
					//else
					continue;//forever
				}

				CommSgmlTagAttributes attrs;
				for(;;)//forever #1.2
				{
					c = _skipWS( s );
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
							attrs._add( attrName, NULL );
							tagEnd = true;
							break;
						}
						else if( c < 0 || c == '>' )
						{
							attrs._add( attrName, NULL );
							tagEnd = true;
							break;
						}
						else
							encString->append( attrName, c );
					}
					if( tagEnd )
						break;//sic!

					s.unget( c );
					c = _skipWS( s );
					if( c != '=' )
					{
						attrs._add( attrName, NULL );
						s.unget( c );
						continue;
					}

					c = _skipWS( s );
					char delim = ' ';
					PBlock attrValue;//of PUNICHAR's
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
						{
							break;
						}
						else if( delim == ' ' && _isWS( c ) )
						{
							break;
						}
						else
						{
							_appendUniChar( attrValue, c );
						}
					}
					_appendUniChar( attrValue, 0 );

					//resolve intra-attribute entities
					//check if any '&'s present
					const PUNICHAR* ampFound = NULL;
					const PUNICHAR* p;
					for( p=(PUNICHAR*)attrValue.ptr(); *p ; ++p )
						if( *p == '&' )
						{
							ampFound = p;
							break;//for( i )
						}

					if( ampFound )
					{
						PBlock newVal;//of PUNICHAR's
						for( p=(PUNICHAR*)attrValue.ptr(); *p ; )
						{
							if( *p == '&' )
							{
								_CommSgmlEntityFinder entity;
								const PUNICHAR* pStart = ++p;
								for(;;)
								{
									PUNICHAR_RET c = *p++;
									if( c == 0 )
										break;
									if( !entity.push( c ) )
										break;
								}
								entity.end();
								if( entity.found >= 0 )
								{
									_appendUniChar( newVal, entity.found );
									p = pStart + entity.foundLen;
								}
								else
								{
									_appendUniChar( newVal, '&' );
									p = pStart;
								}
							}
							else
								_appendUniChar( newVal, *p++ );
						}
						_appendUniChar( newVal, 0 );

						attrValue.moveFrom( newVal );
					}
					/* --- old ANSI-only version, left here for reference
					const char* amp = strchr( attrValue, '&' );
					if( amp )
					{
						PString newVal;
						for( const char* p = attrValue ; *p ; )
						{
							if( *p == '&' )
							{
								_CommSgmlEntityFinder entity;
								const char* pStart = ++p;
								for(;;)
								{
									PUNICHAR_RET c = *p++;
									if( c == 0 )
										break;
									if( !entity.push( c ) )
										break;
								}
								entity.eof();
								if( entity.found >= 0 )
								{
									newVal.append( entity.found );
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
					*/

					int avLen = 0;
					for( p=(PUNICHAR*)attrValue.ptr(); *p ; ++p )
						++avLen;
					PString attrValue2;
					encString->append( attrValue2, (PUNICHAR*)attrValue.ptr(), ((PUNICHAR*)attrValue.ptr()) + avLen );
					attrs._add( attrName, attrValue2 );
					if( tagEnd )
						break;//sic!
				}//forever #1.2
				_tag( tagName, &attrs );
			}//if( '<' )
		}//c == '<'
		else
			ch = c;

		if( ch >= 0 )
			_character( ch );

		//if( _stopParse )
		//	break;//forever
	}//forever
}

/*
void CommSgmlParser::stopParse()
{
_stopParse = true;
}

void CommSgmlParser::recursiveContinueParse()
{
bool oldStopParse = _stopParse;
_parse( *parserStream );
_stopParse = oldStopParse;
}
*/

void CommSgmlParser::_composeCharacter( PString& buf, PUNICHAR_RET c, bool attr, const PStringEncoding* encString )
{
	switch( c )
	{
	case '&':
		encString->append( buf, '&' );
		encString->append( buf, 'a' );
		encString->append( buf, 'm' );
		encString->append( buf, 'p' );
		encString->append( buf, ';' );
		break;
	case '<':
		encString->append( buf, '&' );
		encString->append( buf, 'l' );
		encString->append( buf, 't' );
		encString->append( buf, ';' );
		break;
	case '>':
		encString->append( buf, '&' );
		encString->append( buf, 'g' );
		encString->append( buf, 't' );
		encString->append( buf, ';' );
		break;
	case '\"':
		if( attr )
		{
			encString->append( buf, '&' );
			encString->append( buf, 'q' );
			encString->append( buf, 'u' );
			encString->append( buf, 'o' );
			encString->append( buf, 't' );
			encString->append( buf, ';' );
		}
		else
		{
			encString->append( buf, c );
		}
		break;
	default:
		if( c > 0 )
			encString->append( buf, c );
		break;
	}
}
