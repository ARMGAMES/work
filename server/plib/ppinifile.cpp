#include "ppinifile.h"

const char* PIniFile::Section::getProperty( const char* name_ ) const
{
	for( auto&& item : items )
	{
		if( PString::compareIgnoreCase( name_, item.name ) == 0 )
			return item.value;
	}
	return 0;
}

const char* PIniFile::Section::_getMacros( const char* name_ ) const
{
	for( auto&& item : _macros )
	{
		if( PString::compare( name_, item.name ) == 0 )
			return item.value;
	}
	return 0;
}

void PIniFile::Section::setProperty( const char* name_, const char* value )
{
	for( auto&& item : items )
	{
		if( PString::compareIgnoreCase( name_, item.name ) == 0 )
		{
			item.value = value;
			return;
		}
	}
	items.push_back( Item( name_, value ) );
}

void PIniFile::Section::eraseProperty( const char* name_ )
{
	vector< Item >::iterator iter;
	for( iter = items.begin(); iter != items.end(); )
	{
		if( PString::compareIgnoreCase( name_, (*iter).name ) == 0 )
		{
			iter = items.erase( iter );
			continue;
		}
		++iter;
	}
}

int PIniFile::Section::getIntProperty( const char* name_, int defaultValue ) const
{
	const char* s = getProperty( name_ );
	if( s == 0 )
		return defaultValue;
	int radix = 10;
	if( *s != 0 && s[ 0 ] == 'x' )
	{
		radix = 16;
		s += 1;
	}

	char* dummy;
	return (int)strtol( s, &dummy, radix );
}

INT64 PIniFile::Section::getInt64Property( const char* name_, INT64 defaultValue ) const
{
	const char* s = getProperty( name_ );
	if( s == 0 )
		return defaultValue;
	return p_atoi64( s );
}

PIniFile::Section* PIniFile::getSection( const char* secName )
{
	for( auto&& sec : sections )
	{
		if( PString::compareIgnoreCase( secName, sec.name ) == 0 )
			return &sec;
	}
	return 0;
}

const PIniFile::Section* PIniFile::getSection( const char* secName ) const
{
	for( auto&& sec : sections )
	{
		if( PString::compareIgnoreCase( secName, sec.name ) == 0 )
			return &sec;
	}
	return 0;
}

PIniFile::Section* PIniFile::addSection( const char* secName )
{
	for( auto&& sec : sections )
	{
		if( PString::compareIgnoreCase( secName, sec.name ) == 0 )
			return &sec;
	}
	return &*sections.insert( sections.end(), Section( secName ) );
}

void PIniFile::eraseSection( const char* secName )
{
	vector< Section >::iterator iter;
	for( iter = sections.begin(); iter != sections.end(); )
	{
		if( PString::compareIgnoreCase( secName, (*iter).name ) == 0 )
		{
			iter = sections.erase( iter );
			continue;
		}
		++iter;
	}
}

void PIniFile::setSection( const Section& sec )
{
	Section* found = getSection( sec.name );
	if( found )
		*found = sec;
	else
		sections.push_back( sec );
}

static int _readChar(FILE* f)
{
    return fgetc(f);
}

static int _readChar(const BYTE*& p, const BYTE* pEnd)
{
    return (p >= pEnd ? -1 : *p++);
}

static int _appendChar(PString& ret, int& lineNum, int c)
{
    if (c < 0)
        return *ret ? 1 : 0;
	if (c == '\n')
	{
		++lineNum;
		return 1;
	}
    if (c != '\r' && c != 0)
    {
        ret.append((char)c);
        return 2;
    }
    return 3;
}

template <typename... ARGS>
static bool _readLineInternal(PString& ret, int& lineNum, size_t& lineLen, ARGS&&... args)
{
    for (;;)
    {
        switch (_appendChar(ret, lineNum, _readChar(std::forward<ARGS>(args)...)))
        {
            case 0: return false;
            case 1: return true;
            case 2: ++lineLen; break;
        }
    }
}

template <typename... ARGS>
static bool _readLine(PString& ret, int& lineNum, ARGS&&... args)
{
    static const char* concatSuffix = "<<<CONTINUE>>>";
    static const size_t concatSuffixLen = strlen(concatSuffix);
    ret = "";
    size_t retLen = 0;
    while (1)
    {
        PString line;
        size_t lineLen = 0;
        if (!_readLineInternal(line, lineNum, lineLen, std::forward<ARGS>(args)...))
        {
            return false;
        }
        if (line[0] == ';' || line[0] == '#')
        {
            continue;
        }
        const char* concatSuffixStart = line.find(concatSuffix);
        if (concatSuffixStart && concatSuffixStart + concatSuffixLen == line.c_str() + lineLen)
        {
            ret.reserve(retLen + lineLen - concatSuffixLen);
            for (size_t i = 0, n = concatSuffixStart - line.c_str(); i < n; ++i)
            {
                ret.append(line.charAt(i));
                ++retLen;
            }
        }
        else
        {
            ret.append(line);
            break;
        }
    }
    return true;
}

const char* PIniFile::_getMacros(vector< Section >::iterator& section, const char* name ) const
{
	const char * found = (*section)._getMacros(name);
	if (!found && (*section).name.length())
	{
		const Section* foundSection = getSection( "" );
		if( foundSection )
			found = foundSection->_getMacros(name);
	}
	return found;
}

void PIniFile::_includeMacros(vector< Section >::iterator& section, PString &res, const char * data)
{
	while(*data)
	{
		const char * amp=strchr(data,'&');
		if (!amp)
		{
			res.append(data);
			break;
		}
		const size_t pos=strcspn(amp + 1,"& ");
		if (*(amp + 1 + pos) == '&')
		{
			PString mcr(amp,amp + 2 + pos);
			const char * replace = _getMacros(section,mcr);
			if (replace)
			{
				res.append(data,amp);
				res.append(replace);
				data = amp + 2 + pos;
				continue;
			}
		}
		res.append(data,amp+1);
		data=amp + 1;
	}

}

void PIniFile::_loadLine( vector< Section >::iterator& section, int line, const char* ln )
{
	PString s( PString::ltrim( ln ), PString::rtrim( ln ) );
	bool Ok = true;
	if( *s == ';' || *s == '#' || *s == 0 )
		return;
	else if( *s == '[' && s[ s.length() - 1 ] == ']' )
	{
		section = sections.insert( sections.end(), Section() );
		(*section).name.assign( s + 1, s + s.length() - 1 );
	}
	else if (*s == '@')
	{
		PString fileName;
		_includeMacros(section, fileName, s + 1);
		_loadInclude(section, fileName);
	}
	else
	{
		if( section == sections.end() )
			section = sections.insert( sections.end(), Section() );//empty section
		const char* eq = strchr( s, '=' );
		if( eq == 0 )
			Ok = false;
		else
		{
			bool processed = false;
			if (eq - s >= 3 && *s == '&' && *(eq-1) == '&')
			{
				PString macroName;
				macroName.assign(s, eq);
				if (_getMacros(section, macroName) == nullptr)
				{
					Item& item = *section->_macros.insert((*section)._macros.end(), Item());
					item.name.moveFrom(macroName);
					_includeMacros(section, item.value, eq + 1);
					processed = true;
				}
			}
			if (!processed)
			{
				Item& item = *(*section).items.insert( (*section).items.end(), Item() );
				item.name.assign( s, eq );
				_includeMacros(section,item.value,eq + 1);
				PString newName;
				_includeMacros(section, newName, item.name);
				item.name.moveFrom(newName);
			}
		}
	}
	if( !Ok )
	{
		throw PInternalError( PString( "PIniFile: Error in line #" ).appendInt( line ) );
	}
}

void PIniFile::_load( const char* fileName )
{
	_dir = fileName;
	const char * p1 = strrchr(_dir.c_str(),'/');
	const char * p2 = strrchr(_dir.c_str(),'\\');
	if (p1>p2)
		_dir.cut(p1 - _dir.c_str() + 1);
	else if (p2>p1)
		_dir.cut(p2 - _dir.c_str() + 1);
	else 
	{
		_dir="";
	}
	FILE* f = fopen( fileName, "rt" );
	if( !f )
	{
		PString msg;
		msg.append( "Unable to open ini file '" ).append( fileName ).append( "'" );
		throw PInternalError( msg );
	}

	sections.erase( sections.begin(), sections.end() );

	vector< Section >::iterator section = sections.end();
	int line = 0;
	for(;;)
	{
		PString ln;
		bool Ok = _readLine( ln, line, f );
		if( !Ok )
		{
			fclose( f );
			return;
		}

		try
		{
			_loadLine( section, line, ln );
		}
		catch( const PError& )
		{
			fclose( f );
			throw;
		}
	}
}

void PIniFile::_loadInclude( vector< Section >::iterator& section, const char* fileName )
{
	PString _file;
	if (*fileName == '\\' || *fileName == '/' || strchr(fileName,':'))
	{
		_file = fileName;
	}
	else
	{
		_file = _dir;
		_file.append(fileName);
	}
	FILE* f = fopen( _file.c_str(), "rt" );
	if( !f )
	{
		PString msg;
		msg.append( "Unable to open ini file '" ).append( fileName ).append( "'" );
		throw PInternalError( msg );
	}

	int line = 0;
	for(;;)
	{
		PString ln;
		bool Ok = _readLine( ln, line, f );
		if( !Ok )
		{
			fclose( f );
			return;
		}

		try
		{
			_loadLine( section, line, ln );
		}
		catch( const PError& )
		{
			fclose( f );
			throw;
		}
	}
}


void PIniFile::_load( const BYTE* p, size_t sz )
{
	const BYTE* pEnd = p + sz;

	sections.erase( sections.begin(), sections.end() );
	vector< Section >::iterator section = sections.end();
	int line = 0;
	for(;;)
	{
		PString ln;
		bool Ok = _readLine( ln, line, p, pEnd );
		if( !Ok )
			return;

		_loadLine( section, line, ln );
	}
}

void PIniFile::save( const char* fileName ) const
{
	FILE* f = fopen( fileName, "wt" );
	if( !f )
		throw PInternalError( PString( "Unable to open ini file '" ).append( fileName ).append( "' for writing" ) );

	for( auto&& sec : sections )
	{
		fprintf( f, "[%s]\n", sec.name.c_str() );
		for( auto&& item : sec.items )
		{
			fprintf( f, "%s=%s\n", item.name.c_str(), item.value.c_str() );
		}
		fprintf( f, "\n" );
	}
	fclose( f );
}

void PIniFile::save( PString& dst ) const
{
	for( auto&& sec : sections )
	{
		PString s;
		s.append( '[' );
		s.append( sec.name );
		s.append( "]\n" );
		dst.append( s );

		for( auto&& item : sec.items )
		{
			s = item.name;
			s.append( '=' );
			s.append( item.value );
			s.append( '\n' );
			dst.append( s );
		}
		dst.append( '\n' );
	}
}
