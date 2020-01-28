#include "ppinclude.h"
#include "ppstring.h"

const char* prefix = "TXT_";
const char* headerFileName = "_msg.h";

struct _File
{
public:
	typedef PStringMap< PString > _MsgsType;

public:
	PString name;
	_MsgsType msgs;

public:
	_File( const char* name_ )
		: name( name_ ) {}
};

int main( int argc, char** argv )
{
	PNewPtrVector< _File > files;
	int i;

	//load args
	for( i = 1; i < argc; ++i )
	{
		if( *argv[ i ] == '-' || *argv[ i ] == '/' )
		{
			const char* option = argv[ i ] + 1;
			if( strncmp( option, "prefix=", 7 /* strlen("prefix=") */ ) == 0 )
				prefix = option + 7/* strlen("prefix=") */;
			else if( strncmp( option, "header=", 7 /* strlen("header=") */ ) == 0 )
				headerFileName = option + 7/* strlen("header=") */;
		}
		else
			files.push_back( new _File( argv[ i ] ) );
	}

	if( files.size() == 0 )
	{
		printf( "Usage: _ppmsgc [-prefix=<prefix>] [-header=<header-file-name>] <msg-file> [<msg-file>...]\n" );
		return -1;
	}

	//load files
	int n = files.size();
	for( i = 0; i < n; ++i )
	{
		_File* file = files[ i ];
		FILE* f = fopen( file->name, "rt" );
		if( f == 0 )
		{
			printf( "Unable to open file '%s'\n", file->name.c_str() );
			return -2;
		}

		for(; ; )
		{
			char buf[ 4096 ];
			if( fgets( buf, sizeof( buf ), f ) == 0 )
				break;//forever

			if( buf[ strlen( buf ) - 1 ] == '\n' )
				buf[ strlen( buf ) - 1 ] = 0;

			//			char* pound = strchr( buf, '#' );
			//			if( pound )
			//				*pound = 0;

			char* name = strtok( buf, " \t" );
			if( name && *name != '#' )
			{
				char* value = strtok( 0, "" );
				if( !value )
					value = "";
				bool Ok = file->msgs.insert( _File::_MsgsType::value_type( name, value ) ).second;
				if( !Ok )
				{
					printf( "Error: duplicate element '%s' in file '%s'\n", name, file->name.c_str() );
					return -3;
				}
			}
		}
		fclose( f );
	}

	//check equivalence
	for( i = 1; i < n; ++i )
	{
		int n0 = files[ 0 ]->msgs.size();
		int ni = files[ i ]->msgs.size();
		if( ni != n0 )
		{
			printf( "Error: file '%s' has %s elements than file '%s'\n", files[ 0 ]->name.c_str(), n0 < ni ? "less" : "more", files[ i ]->name.c_str() );
			return -4;
		}

		_File::_MsgsType::iterator i0 = files[ 0 ]->msgs.begin();
		_File::_MsgsType::iterator ii = files[ i ]->msgs.begin();

		for( int j = 0; j < n0; ++j )
		{
			const char* name0 = ( *i0 ).first;
			const char* nameI = ( *ii ).first;

			if( strcmp( name0, nameI ) != 0 )
			{
				printf( "Error: file '%s' has elements different from file '%s'\n", files[ 0 ]->name.c_str(), files[ i ]->name.c_str() );
				return -4;
			}

			++i0;
			++ii;
		}
	}

	//generate .h
	{
		FILE* f = fopen( headerFileName, "wt" );
		int j = 0;
		for( _File::_MsgsType::iterator iter = files[ 0 ]->msgs.begin(); iter != files[ 0 ]->msgs.end(); ++iter )
			fprintf( f, "#define %s%s %d\n", prefix, ( *iter ).first, j++ );
		fclose( f );
	}

	//generate .c
	for( i = 0; i < n; ++i )
	{
		PString fName;
		fName.append( files[ i ]->name ).append( ".c" );
		FILE* f = fopen( fName, "wt" );
		for( _File::_MsgsType::iterator iter = files[ i ]->msgs.begin(); iter != files[ i ]->msgs.end(); ++iter )
		{
			fprintf( f, " /* %s%s */ \"", prefix, ( *iter ).first );
			for( const char* p = ( *iter ).second.c_str(); *p; ++p )
				if( *p == '\"' )
					fprintf( f, "\\\"" );
				else
					fprintf( f, "%c", *p );
			fprintf( f, "\"" );
			fprintf( f, ",\n" );
		}
		fclose( f );
	}

	return 0;
}
