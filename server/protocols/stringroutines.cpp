#include "stringroutines.h"
#include "i18n.utils.h"

void splitString(const char* str, vector<PString>& strVec, char delimeter, bool keepEmpty, bool trim)
{
	if(!str || (!*str && !keepEmpty))
		return;
	const char* p = str;
	PString str_tmp;
	const char* commaPtr = strchr(str, delimeter);

	while(commaPtr)
	{
		str_tmp.assign(p, commaPtr);

		if( trim )
		{
			PString trimmed(PString::ltrim(str_tmp), PString::rtrim(str_tmp));
			if(*trimmed || keepEmpty)
				strVec.push_back(trimmed);
		}
		else if(*str_tmp || keepEmpty)
			strVec.push_back(str_tmp);

		p = commaPtr + 1;
		if(*p)
			commaPtr = strchr(p, delimeter);
		else
			break;
	}

	if(*p || keepEmpty)
	{
		if( trim )
		{
			PString trimmed(PString::ltrim(p), PString::rtrim(p));
			if(*trimmed || keepEmpty)
				strVec.push_back(trimmed);
		}
		else
			strVec.push_back(p);
	}

}

bool splitBracketedString( const char* source, vector<PString>& output, PString& errStr, bool ignoreNested  )
{
	if( !source || !*source )
	{
		errStr="empty string";
		return false;
	}

	int level = 0;
	const char *p = source;
	const char *start = 0;
	while( *p )
	{
		if( *p == '(' )
		{
			if( level > 0 )
			{
				if( !ignoreNested )
				{
					errStr= PString( "nested left bracket: '" ).append( p ).append("'");
					return false;
				}
			}
			else
			{
				start = p + 1;
			}

			++level;
		}
		else if( *p == ')' )
		{
			if( level == 1 )
			{
				PASSERT( start );
				if( start < p )
					output.push_back( PString( start, p ) );
				start = 0;
			}
			else if( level == 0 )
			{
				errStr= PString( "not balanced right bracket: '" ).append( p ).append("'");
				return false;
			}
			--level;
		}
		++p;
	}

	if( 0 != level )
	{
		errStr= PString( "ended with open bracket" );
		return false;
	}

	return true;
}

void stripHtmlTags(const char* text, PString& res)
{
	res.cut(0);
	const char* ptr = strchr(text, '<');
	if (!ptr)
	{
		res.assign(text);
		return;
	}
	res.assign(text, ptr);  // assign text before first html tag
	while (*ptr)
	{
		while (*ptr && *ptr != '>') ++ptr;  // skip html tag
		if (*ptr)
			++ptr;
		while (*ptr && *ptr != '<') // append non-html text
		{
			i18nAppendChar(res, *ptr);
			++ptr;
		}
	}
}

bool getTaggedSubstring( const char* source, const char* startTag, char terminator, PString& result )
{
	if( source && startTag )
	{
		size_t startTagLen = strlen( startTag );
		if( startTagLen < strlen( source ) )
		{
			const char* p = strstr( source, startTag );
			if( p )
			{
				p += startTagLen;
				const char* p1 = strchr( p, terminator );
				if( p1 )
					result.assign( p, p1 );
				else
					result.assign( p );
				return true;
			}
		}
	}
	return false;
}
