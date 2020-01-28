#include "_commmime.h"

/* static */ void _CommMime::getHdrName( PString& ret, const char*& s )
{
	skipSpace(s);
	for( ; *s && *s != ':' ; ++s )
		ret.append( *s );
	if( !*s++ )
		throw PInternalError( "_CommMime: ':' expected in header" );
}

/* static */ void _CommMime::getHdrValue( PString& ret, const char*& s )
{
	skipSpace(s);
	for( ; *s && *s != ';' ; ++s )
		ret.append( *s );
	if( *s )
		++s;
}

/* static */ void _CommMime::getHdrAttr( PString& name, PString& value, const char*& s )
{
	skipSpace(s);
	for( ; *s && *s != '=' ; ++s )
		name.append( *s );
	if( !*s++ )
		throw PInternalError( "_CommMime: '=' expected in header" );
	skipSpace(s);
	char delim = 0;
	if( *s == '\"' )
	{
		delim = '\"';
		++s;
	}
	if (delim)
		for( ; *s && *s != delim ; ++s )
			value.append( *s );
	else
		for( ; *s && *s != ';' ; ++s )
			value.append( *s );

	skipSpace(s);
	if( *s == ';' )
		++s;
	else if( delim && *s == delim )
	{
		++s;
		skipSpace(s);
		if( *s == ';' )
			++s;
		else if( *s )
			throw PInternalError( "_CommMime: ';' expected in header" );
	}
	skipSpace(s);
}

/* static */ void _CommMime::getHdrAttrNoName( PString& name, PString& value, const char*& s, BYTE & delim )
{
	delim = 0;
	skipSpace(s);
	for( ; *s && *s != '='; ++s )
		name.append( *s );
	if( !*s )
	{
		value.moveFrom(name);
		return;
	}
	if (*s == '=')
		s++;
	skipSpace(s);
	if( *s == '\"' )
	{
		delim = '\"';
		++s;
	}
	if (delim)
		for( ; *s && *s != delim ; ++s )
			value.append( *s );
	else
		for( ; *s && *s != ';' ; ++s )
			value.append( *s );

	skipSpace(s);
	if( *s == ';' )
		++s;
	else if( delim && *s == delim )
	{
		++s;
		skipSpace(s);
		if( *s == ';' )
			++s;
		else if( *s )
			throw PInternalError( "_CommMime: ';' expected in header" );
	}
	skipSpace(s);
}
