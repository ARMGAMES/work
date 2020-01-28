//------------------------------------------------------------------------------
//	ImageSourceParser.cpp
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

#include "ImageSourceParser.h"

//------------------------------------------------------------------------------

void ImageSourceParser::_parseStyle( PUniString& buf )
	{
	PUNICHAR cur = 0;
	PString* source = NULL;
	for( const PUNICHAR* p = buf; *p; ++p )
		{
		if( source )
			{
			if( *p == cur )
				{
				imageSource( *source );

				PStringEncoding::StringParser parser;
				encString->createNewParser( parser );

				parser.init( *source );
				while( PUNICHAR_RET c = parser.nextChar() )
					_composeCharacter( rBuf, c, false, encStringOut );

				delete source;
				source = NULL;

				_composeCharacter( rBuf, *p, false, encStringOut );
				cur = 0;
				}
			else
				{
				encString->append( *source, *p );
				}
			continue;
			}

		if( ( ( *p == 'U' || *p == 'u' ) && cur == 0 ) 
			|| ( ( *p == 'R' || *p == 'r' ) && ( cur == 'U' || cur == 'u' ) ) 
			|| ( ( *p == 'L' || *p == 'l' ) && ( cur == 'R' || cur == 'r' ) ) 
			|| ( *p == '(' && ( cur == 'L' || cur == 'l' ) ) )
			{
			_composeCharacter( rBuf, *p, false, encStringOut );
			cur = *p;
			}
		else if( ( *p == '"' || *p == '\'' ) && cur == '(' )
			{
			source = new PString();

			_composeCharacter( rBuf, *p, false, encStringOut );
			cur = *p;
			}
		else if( ( *p != ' ' && *p != '\t' && *p != '\r' && *p != '\n' ) 
			|| ( cur != 'L' && cur != 'l' && cur != '(' ) )
			{
			_composeCharacter( rBuf, *p, false, encStringOut );
			cur = 0;
			}
		}

	if( source )
		{
		PStringEncoding::StringParser parser;
		encString->createNewParser( parser );

		parser.init( *source );
		while( PUNICHAR_RET c = parser.nextChar() )
			_composeCharacter( rBuf, c, false, encStringOut );

		delete source;
		}
	}

void ImageSourceParser::parse( PString& buf )
	{
	rBuf.cut( 0 );
	if( buf.length() && ( buf[ 0 ] != '^' ) )
		{
		PUniInputStreamFromString uniStream( buf, *encString );
		CommSgmlParser::parse( uniStream );
		buf.moveFrom( rBuf );
		}
	}

void ImageSourceParser::uniCharacters( const PUNICHAR* begin, const PUNICHAR* end )
	{
	if( styleBuf )
		{
		styleBuf->append( begin, end );
		return;
		}

	for( const PUNICHAR* p = begin; p < end; ++p )
		_composeCharacter( rBuf, *p, false, encStringOut );
	}

void ImageSourceParser::tag( const char* name, CommSgmlTagAttributes* attributes )
	{
	if( styleBuf )
		{
		_parseStyle( *styleBuf );

		delete styleBuf;
		styleBuf = NULL;
		}

	if( PString::compareIgnoreCase( name, "STYLE" ) == 0 )
		styleBuf = new PUniString();

	encStringOut->append( rBuf, '<' );

	for( const char* p = name; *p; ++p )
		_composeCharacter( rBuf, *p, true, encStringOut );

	if( attributes )
		{
		vector< pair< PString, PString > > attrs;
		attributes->enumerate( attrs );

		if( PString::compareIgnoreCase( name, "IMG" ) == 0 )
			{
			for( size_t i = 0; i < attrs.size(); ++i )
				{
				if( PString::compareIgnoreCase( attrs[ i ].first, "SRC" ) == 0 )
					{
					imageSource( attrs[ i ].second );
					break;
					}
				}
			}

		PStringEncoding::StringParser parser;
		encString->createNewParser( parser );

		for( size_t i = 0; i < attrs.size(); ++i )
			{
			encStringOut->append( rBuf, ' ' );

			for( const char* p = attrs[ i ].first; *p; ++p )
				encStringOut->append( rBuf, *p );

			if( attrs[ i ].second.length() )
				{
				encStringOut->append( rBuf, '=' );
				encStringOut->append( rBuf, '"' );

				if( PString::compareIgnoreCase( attrs[ i ].first, "STYLE" ) == 0 )
					{
					PUniString buf;
					parser.init( attrs[ i ].second );
					while( PUNICHAR_RET c = parser.nextChar() )
						buf.append( c );
					_parseStyle( buf );
					}
				else
					{
					parser.init( attrs[ i ].second );
					while( PUNICHAR_RET c = parser.nextChar() )
						_composeCharacter( rBuf, c, true, encStringOut );
					}

				encStringOut->append( rBuf, '"' );
				}
			}
		}

	encStringOut->append( rBuf, '>' );
	}

//------------------------------------------------------------------------------
