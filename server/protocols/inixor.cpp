//------------------------------------------------------------------------------
//	inixor.cpp
//------------------------------------------------------------------------------
//
//	Copyright (c) 2001-2011 PYR Software Ltd. All rights reserved. This document 
//	is for the use of licensed users only and is subject to the terms and 
//	conditions of the accompanying and/or applicable license between you and 
//	PYR Software Ltd, including without limitation those terms and conditions 
//	prohibiting unauthorized translation, reverse engineering, decompilation, 
//	disassembly, and other inappropriate use of any software provided. Any 
//	unauthorized use, copying, modification, distribution, publication or 
//	disclosure of information is a violation of copyright laws. No reproduction 
//	in whole or in part of this document may be made without express written 
//	consent of PYR Software Ltd.
//
//
//------------------------------------------------------------------------------

#include "inixor.h"
#include "ppstream.h"
#include "ppzip.h"
#include "commssl.h"

//------------------------------------------------------------------------------

PIniFile::Section* EncIniFile::getSection( const char* secName )
{
	size_t n = sections.size();
	for( size_t i = 0; i < n ; ++i )
	{
		PIniFile::Section& sec = sections[ i ];
		if( PString::compareIgnoreCase( secName, sec.name ) == 0 )
			return &sec;
	}
	return 0;
}

const PIniFile::Section* EncIniFile::getSection( const char* secName ) const
{
	size_t n = sections.size();
	for( size_t i = 0; i < n ; ++i )
	{
		const PIniFile::Section& sec = sections[ i ];
		if( PString::compareIgnoreCase( secName, sec.name ) == 0 )
			return &sec;
	}
	return 0;
}

vector< PIniFile::Section >::iterator EncIniFile::_addSection( const char* secName )
{
	size_t n = sections.size();
	for( size_t i = 0; i < n ; ++i )
	{
		PIniFile::Section& sec = sections[ i ];
		if( PString::compareIgnoreCase( secName, sec.name ) == 0 )
			return sections.begin() + i;
	}
	return sections.insert( sections.end(), PIniFile::Section( secName ) );
}

PIniFile::Section* EncIniFile::addSection( const char* secName )
{
	return &*_addSection( secName );
}

void EncIniFile::eraseSection( const char* secName )
{
	vector< PIniFile::Section >::iterator iter;
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

void EncIniFile::setSection( const PIniFile::Section& sec )
{
	PIniFile::Section* found = getSection( sec.name );
	if( found )
		*found = sec;
	else
		sections.push_back( sec );
}

static bool _readLine( PString& ret, FILE* f )
{
	ret = "";
	for(;;)
	{
		int c = fgetc( f );
		if( c < 0 )
			return *ret ? true : false;
		if( c == '\n' )
			return true;
		else if( c != '\r' && c != 0 )
			ret.append( (char)c );
	}
}

static bool _readLine( PString& ret, const BYTE*& p, const BYTE* pEnd )
{
	ret = "";
	for(;;)
	{
		int c;
		if( p >= pEnd )
			c = -1;
		else
			c = *p++;
		if( c < 0 )
			return *ret ? true : false;
		if( c == '\n' )
			return true;
		else if( c != '\r' && c != 0 )
			ret.append( (char)c );
	}
}

void EncIniFile::_loadLine( vector< PIniFile::Section >::iterator& section, int line, const char* ln )
{
	PString s( PString::ltrim( ln ), PString::rtrim( ln ) );
	bool Ok = true;
	if( *s == ';' || *s == '#' || *s == 0 )
		return;
	else if( *s == '[' && s[ s.length() - 1 ] == ']' )
	{
		const PString secName( s + 1, s + s.length() - 1 );
		section = _addSection( secName );
	}
	else
	{
		if( section == sections.end() )
			section = _addSection( "" );//empty section
		const char* eq = strchr( s, '=' );
		if( eq == 0 )
			Ok = false;
		else
		{
			PIniFile::Item& item = *(*section).items.insert( (*section).items.end(), PIniFile::Item() );
			item.name.assign( s, eq );
			item.value = eq + 1;
		}
	}
	if( !Ok )
	{
		throw PInternalError( PString( "EncIniFile: Error in line #" ).appendInt( line ) );
	}
}

void EncIniFile::_load( const char* fileName, const PStringEncoding* encString )
{
	FILE* f = plib_fopen( fileName, "rt", encString );
	if( !f )
	{
		PString msg;
		msg.append( "Unable to open ini file '" ).append( fileName ).append( "'" );
		throw PInternalError( msg );
	}

	sections.erase( sections.begin(), sections.end() );
	vector< PIniFile::Section >::iterator section = sections.end();
	int line = 0;
	for(;;)
	{
		PString ln;
		bool Ok = _readLine( ln, f );
		if( !Ok )
		{
			fclose( f );
			return;
		}
		++line;

		try
		{
			_loadLine( section, line, ln );
		}
		catch( PError& )
		{
			fclose( f );
			throw;
		}
	}
}

void EncIniFile::_load( const BYTE* p, size_t sz )
{
	const BYTE* pEnd = p + sz;

	sections.erase( sections.begin(), sections.end() );
	vector< PIniFile::Section >::iterator section = sections.end();
	int line = 0;
	for(;;)
	{
		PString ln;
		bool Ok = _readLine( ln, p, pEnd );
		if( !Ok )
			return;
		++line;

		_loadLine( section, line, ln );
	}
}

void EncIniFile::save( const char* fileName, const PStringEncoding* encString ) const
{
	FILE* f = plib_fopen( fileName, "wt", encString );
	if( !f )
		throw PInternalError( PString( "Unable to open ini file '" ).append( fileName ).append( "' for writing" ) );

	size_t n = sections.size();
	for( size_t i = 0; i < n ; ++i )
	{
		const PIniFile::Section& sec = sections[ i ];
		fprintf( f, "[%s]\n", sec.name.c_str() );
		size_t nj = sec.items.size();
		for( size_t j = 0; j < nj ; ++j )
		{
			const PIniFile::Item& item = sec.items[ j ];
			fprintf( f, "%s=%s\n", item.name.c_str(), item.value.c_str() );
		}
		fprintf( f, "\n" );
	}
	fclose( f );
}

void EncIniFile::save( PString& dst ) const
{
	size_t n = sections.size();
	for( size_t i = 0; i < n ; ++i )
	{
		const PIniFile::Section& sec = sections[ i ];
		PString s;
		s.append( '[' );
		s.append( sec.name );
		s.append( "]\n" );
		dst.append( s );

		size_t nj = sec.items.size();
		for( size_t j = 0; j < nj ; ++j )
		{
			const PIniFile::Item& item = sec.items[ j ];
			s = item.name;
			s.append( '=' );
			s.append( item.value );
			s.append( '\n' );
			dst.append( s );
		}
		dst.append( '\n' );
	}
}

//------------------------------------------------------------------------------

static void _xorBlock( PBlock& buf, const PBlock& xr )
{
	PASSERT( xr.size() );
	for( size_t i = 0; i < buf.size(); ++i )
		buf.ptr()[ i ] ^= xr.ptr()[ i % xr.size() ];
}

//------------------------------------------------------------------------------

static const char PassStr1[] = "Microsoft .NET Runtime Error: %s\0/update.ini";
static const char PassStr2[] = ".NET Framework Internal Error";

//------------------------------------------------------------------------------

void ZipXor::xorBlock( PBlock& buf )
{
	PBlock xor1, xor2;
	memcpy( xor1.append( sizeof( PassStr1 ) ), PassStr1, sizeof( PassStr1 ) );
	memcpy( xor2.append( sizeof( PassStr2 ) ), PassStr2, sizeof( PassStr2 ) );
	_xorBlock( xor1, xor2 );
	_xorBlock( buf, xor1 );
}

bool ZipXor::isZip( const PBlock& buf )
{
	return ( 2 <= buf.size() ) && ( buf.ptr()[ 0 ] == 0x1f ) && ( buf.ptr()[ 1 ] == 0x8b ); // GZip magic number
}

void ZipXor::zip( PBlock& buf, const BYTE* ptr, size_t size )
{
	PBlock zipBuf;
	PMemOutputStream zipOutStr( zipBuf );
	PGZip zip( zipOutStr );
	zip.write( ptr, size );
	zip.close();

	buf.moveFrom( zipBuf );
}

void ZipXor::zipBlock( PBlock& buf )
{
	zip( buf, buf.ptr(), buf.size() );
}

bool ZipXor::unZipBlock( PBlock& buf )
{
	PBlock unZipBuf;

	if( isZip( buf ) )
	{
		BYTE uzBuf[ 0x400 ];

		PMemInputStream unZipInStr( buf );
		PUnGZip unZip( unZipInStr );
		while( size_t r = unZip.read( uzBuf, sizeof( uzBuf ) ) )
			memcpy( unZipBuf.append( r ), uzBuf, r );
	}

	buf.moveFrom( unZipBuf );

	return buf.size() != 0;
}

//------------------------------------------------------------------------------

bool XorIniFile::fromFile( PBlock& buf, const char* fileName, const PStringEncoding* encString )
{
	if( FILE* f = plib_fopen( fileName, "rb", encString ) )
	{
		fseek( f, 0, SEEK_END );
		const size_t sizeOf = ftell( f );
		fseek( f, 0, SEEK_SET );

		PASSERT( sizeOf );

		const size_t r = fread( buf.alloc( sizeOf ), 1, sizeOf, f );
		fclose( f );

		PASSERT( r == sizeOf );
	}

	return buf.size() != 0;
}

//------------------------------------------------------------------------------

void XorIniFile::loadXor( const char* fileName, const PStringEncoding* encString )
{
	PBlock buf;
	if( fromFile( buf, fileName, encString ) )
	{
		loadXorBlock( buf );
	}
}

//------------------------------------------------------------------------------

void XorIniFile::loadXorBlock( PBlock& buf )
{
	ZipXor::unZipXor( buf );
	load( buf.ptr(), buf.size() );
}

//------------------------------------------------------------------------------
