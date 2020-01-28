#include "commhtml.h"

void CommHtmlTemplate::load( const char* fName )
{
	FILE* f = fopen( fName, "rt" );
	PASSERT3( f );
	load( f );
	fclose( f );
}

void CommHtmlTemplate::load( FILE* f )
{
	PBlock block;
	BYTE buf[ 1024 ];
	for(;;)
	{
		int rd = fread( buf, 1, sizeof(buf), f );
		if( rd == 0 )
			break;//forever
		memcpy( block.append( rd ), buf, rd );
	}

	load( block.ptr(), block.size() );
}

void CommHtmlTemplate::load( const BYTE* p, size_t sz )
{
	clear();
	_parse( p, sz );
}

void CommHtmlTemplate::_characters( iterator iter, const char* s, int n )
{
	if( iter != entities.end() )
	{
		if( ( *iter ).second.size() )
		{
			_Item& item = ( *iter ).second.back();
			if( item.type == 'T' )
				item.append( s, n );
			else
				( *iter ).second.push_back( _Item( 'T', s, n ) );
		}
		else
			( *iter ).second.push_back( _Item( 'T', s, n ) );
	}
}

void CommHtmlTemplate::_parse( const BYTE* p, size_t sz )
{
	const BYTE* pEnd = p + sz;
	const int maxSz = 256;
	char buf[ maxSz ];
	int bufPos = 0;
	iterator iter = entities.end();
	for(;;)
	{
		if( p == pEnd )
		{
			_characters( iter, buf, bufPos );
			break;//forever
		}
		int c = *p++;
		buf[ bufPos++ ] = c;
		if( *buf == '<' && c == '>' )
		{
			if( strncmp( buf + 1, "MAGIC.", 6 /* strlen( "MAGIC." ) */ ) == 0 )
			{
				PASSERT( iter == entities.end() );
				const pair< iterator, bool > r = entities.insert( buf + 1 + 6 /* strlen( "MAGIC." ) */, bufPos - 2 - 6 /* strlen( "MAGIC." ) */, _Entity() );
				PASSERT4( r.second );
				iter = r.first;
			}
			else if( strncmp( buf + 1, "/MAGIC.", 7 /* strlen( "/MAGIC." ) */ ) == 0 )
			{
				PASSERT( iter != entities.end() );
				PASSERT( PString::compare( ( *iter ).first, buf + 1 + 7 /* strlen( "/MAGIC." ) */, bufPos - 2 - 7 /* strlen( "/MAGIC." ) */ ) == 0 );
				iter = entities.end();
			}
			else if( strncmp( buf + 1, "SPELL.", 6 /* strlen( "SPELL." ) */ ) == 0 )
			{
				PASSERT( iter != entities.end() );
				( *iter ).second.push_back( _Item( 'R', buf + 1 + 6 /* strlen( "SPELL." ) */, bufPos - 2 - 6 /* strlen( "SPELL." ) */ ) );
			}
			else
			{
				_characters( iter, buf, bufPos );
			}
			bufPos = 0;
		}
		else if( c == '<' )
		{
			_characters( iter, buf, bufPos - 1 );
			*buf = '<';
			bufPos = 1;
		}
		else if( bufPos == maxSz )
		{
			_characters( iter, buf, bufPos );
			bufPos = 0;
		}
	}//forever
}

bool CommHtmlTemplate::_compose( PString& buf, iterator iter )
{
	if( iter != entities.end() )
	{
		const vector< _Item >& items = ( *iter ).second;
		const int nj = items.size();
		for( int j=0; j < nj ; ++j )
		{
			const _Item& item = items[ j ];
			switch( item.type )
			{
			case 'T':
				if( encString )
				{
					for( const char* p = item; *p; ++p )
						encString->append( buf, *p );
				}
				else
				{
					buf += item;
				}
				break;
			case 'R':
				spell( buf, item );
				break;
			}
		}
		return true;
	}
	return false;
}

bool CommHtmlTemplate::magic( PString& buf, const char* name )
{
	return _compose( buf, entities.find( name ) );
}

void CommHtmlTemplate::merge( CommHtmlTemplate& other, bool overwrite )
{
	iterator otherIter;
	for( otherIter = other.entities.begin(); otherIter != other.entities.end(); ++otherIter )
	{
		iterator iter = entities.find( ( *otherIter ).first );
		if( iter == entities.end() )
		{
			const pair< iterator, bool > r = entities.insert( ( *otherIter ).first, _Entity() );
			PASSERT4( r.second );
			( *r.first ).second.swap( ( *otherIter ).second );
		}
		else if( overwrite )
		{
			( *iter ).second.swap( ( *otherIter ).second );
		}
	}
}

void CommHtmlTemplate::composeAll( PString& buf )
{
	iterator iter;
	for( iter = entities.begin(); iter != entities.end(); ++iter )
		_compose( buf, iter );
}
