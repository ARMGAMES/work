//------------------------------------------------------------------------------
//	inixor.h
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

#ifndef __inixor_h__
#define __inixor_h__

//------------------------------------------------------------------------------

#include "ppinifile.h"

//------------------------------------------------------------------------------

class EncIniFile
{
public:
	vector< PIniFile::Section > sections;

public:
	EncIniFile()
	{
	}
	EncIniFile( const char* fileName, const PStringEncoding* encString = NULL )
	{
		_load( fileName, encString );
	}
	void load( const char* fileName, const PStringEncoding* encString = NULL ) { _load( fileName, encString ); }
	void load( const BYTE* p, size_t sz ) { _load( p, sz ); }
	void save( const char* fileName, const PStringEncoding* encString = NULL ) const;
	void save( PString& dst ) const;

	PIniFile::Section* getSection( const char* secName );
	const PIniFile::Section* getSection( const char* secName ) const;
	PIniFile::Section* addSection( const char* secName );
	void eraseSection( const char* secName );
	void setSection( const PIniFile::Section& sec );

private:
	vector< PIniFile::Section >::iterator _addSection( const char* secName );
	void _loadLine( vector< PIniFile::Section >::iterator& section, int line, const char* ln );
	void _load( const char* fileName, const PStringEncoding* encString );
	void _load( const BYTE* p, size_t sz );
};//PIniFile

struct ZipXor // dummy
{
	static void xorBlock( PBlock& buf );

	static bool isZip( const PBlock& buf );
	static void zip( PBlock& buf, const BYTE* ptr, size_t size );
	static void zipBlock( PBlock& buf );
	static bool unZipBlock( PBlock& buf );

	static void zipXor( PBlock& buf )
	{
		zipBlock( buf );
		xorBlock( buf );
	}
	static bool unZipXor( PBlock& buf )
	{
		xorBlock( buf );
		return unZipBlock( buf );
	}

	static void checkSignature( PBlock& buf, const PBlock* keyPubl = nullptr );
};

class XorIniFile : public EncIniFile
{
private:
	bool fromFile( PBlock& buf, const char* fileName, const PStringEncoding* encString );

public:
	bool xorLoad( const char* fileName, const PStringEncoding* encString = nullptr, bool checkSignature = true, const PBlock* keyPubl = nullptr );
	void loadXor( const char* fileName, const PStringEncoding* encString = nullptr );

public:	// modify content of buf
	bool xorLoadBlock( PBlock& buf, bool checkSignature = true, const PBlock* keyPubl = nullptr );
	void loadXorBlock( PBlock& buf );
};

#endif
