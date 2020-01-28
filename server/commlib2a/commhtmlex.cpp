#include "commhtml.h"

static inline int _strncmp(const PUNICHAR* s, const char* t, size_t n)
{
	if (!n)
		return 0;
	while (--n && *s && *t && *s == *t)
		++s, ++t;
	return *s - *t;
}

static inline int _strcmp(const PUNICHAR* s, const char* t)
{
	while (*s && *t && *s == *t)
		++s, ++t;
	return *s - *t;
}

void CommHtmlTemplateEx::load( PUniInputStream& f_ )
{
	PUniCharInputStream f( f_ );
	PUniString str;
	for(;;)
	{
		PUNICHAR_RET c = f.get();
		if( c < 0 )
			break;
		PASSERT5( c );
		if( c == 0 )
			c = ' ';
		str.append((PUNICHAR) c);
	}
	load( str.c_str(), str.length() );
}

void CommHtmlTemplateEx::load( PInputStream& f, const PStreamEncoding& enc )
{
	PUniInputStreamFromStream uf( f, enc );
	load( uf );
}

void CommHtmlTemplateEx::load( const PUNICHAR* p, size_t sz )
{
	entities.clear();
	_parse( p, sz );
}

CommHtmlTemplateEx::CommHtmlTemplateEx( )
{
	encString = 0;
}

CommHtmlTemplateEx::CommHtmlTemplateEx( const PStringEncoding* encString_ )
{
	encString = encString_->clone();
}

void CommHtmlTemplateEx::init( const PStringEncoding* encString_ )
{
	if (!encString)
		encString = encString_->clone();
}

CommHtmlTemplateEx::~CommHtmlTemplateEx( )
{
	delete encString;
}

void CommHtmlTemplateEx::_characters( EntitiesIterator& curEntity, const PUNICHAR* s, int n )
{
	if( curEntity != entities.end())
	{
		vector< _Item >& items = (*curEntity).second;
		if( items.size() != 0 )
		{
			_Item& item = items.back();
			if( item.type == 'T' )
				item.data.append( s, n );
			else
				items.push_back( _Item( 'T', s, n ) );
		}
		else
			items.push_back( _Item( 'T', s, n ) );
	}
}

#define PASSERTEX(cond, msg, line) (void)((cond) || (throw PInternalError(PString(msg).appendInt(line).c_str()), 0))

void CommHtmlTemplateEx::_parse( const PUNICHAR* p, size_t sz )
{
	int line = 1;
	const PUNICHAR* pEnd = p + sz;
	const int maxSz = 256;
	PUNICHAR buf[ maxSz ];
	int bufPos = 0;
	EntitiesIterator curEntity = entities.end();
	for(;;)
	{
		if( p == pEnd )
		{
			_characters( curEntity, buf, bufPos );
			break;//forever
		}
		PUNICHAR c = *p++;
		if (c == '\n')
		{
			++line;
		}
		buf[ bufPos++ ] = c;
		if( *buf == '<' && c == '>' )
		{
			if( _strncmp( buf+1, "MAGIC.", 6 /* strlen( "MAGIC." ) */ ) == 0 )
			{
				PASSERTEX( curEntity == entities.end(), "<MAGIC.xxx> Error @:", line );
				PUniString name(buf+1+6/* strlen( "MAGIC." ) */, bufPos-2-6/* strlen( "MAGIC." ) */);
				curEntity = entities.insert(make_pair(name, vector< _Item >())).first;
			}
			else if( _strncmp( buf+1, "/MAGIC.", 7 /* strlen( "/MAGIC." ) */ ) == 0 )
			{
				PASSERTEX( curEntity != entities.end(), "</MAGIC.xxx> Error @:", line );
				PASSERT( (*curEntity).first.equals( PUniString( buf+1+7/* strlen( "/MAGIC." ) */, bufPos-2-7/* strlen( "/MAGIC." ) */) ) );
				curEntity = entities.end();
			}
			else if( _strncmp( buf+1, "SPELL.", 6 /* strlen( "SPELL." ) */ ) == 0 )
			{
				PASSERTEX( curEntity != entities.end(), "<SPELL.xxx> Error @:", line );
				(*curEntity).second.push_back( _Item( 'R', buf+1+6/* strlen( "SPELL." ) */, bufPos-2-6/* strlen( "SPELL." ) */ ) );
			}
			else
				_characters( curEntity, buf, bufPos );
			bufPos = 0;
		}
		else if( c == '<' )
		{
			_characters( curEntity, buf, bufPos - 1 );
			*buf = '<';
			bufPos = 1;
		}
		else if( bufPos == maxSz )
		{
			_characters( curEntity, buf, bufPos );
			bufPos = 0;
		}
	}//forever
}

bool CommHtmlTemplateEx::magic( PString& buf, const char* name )
{
	PUniString name_uni;
	name_uni.reserve(strlen(name));
	const char* s = name;
	while (*s)
	{
		PUNICHAR c = *s++;
		name_uni.append(c);
	}
	ConstEntitiesIterator it = entities.find(name_uni);
	if (it != entities.end())
	{
		const vector< _Item >& items = (*it).second;
		int nj = items.size();
		for( int j=0; j < nj ; ++j )
		{
			const _Item& item = items[ j ];
			switch( item.type )
			{
			case 'T':
				encString->append( buf, items[ j ].data, NULL );
				break;
			case 'R':
				PString name_;
				encString->append( name_, items[ j ].data, NULL );
				spell( buf, name_.c_str() );
				break;
			}
		}
		return true;
	}
	//printf( "\n%s\n", name );
	//PASSERT( 0 );
	return false;
}
