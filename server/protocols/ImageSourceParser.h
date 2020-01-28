//------------------------------------------------------------------------------
//	ImageSourceParser.h
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

#if !defined( __imagesourceparser_h_included__ )
#define __imagesourceparser_h_included__

//------------------------------------------------------------------------------

#include "commsgml.h"
#include "ppunistring.h"

//------------------------------------------------------------------------------

class ImageSourceParser : private CommSgmlParser
	{
	PString rBuf;

	private:
	PUniString* styleBuf;

	protected:
	using CommSgmlParser::encString;
	const PStringEncoding* encStringOut;

	public:
	ImageSourceParser( const PStringEncoding* encString_, const PStringEncoding* encStringOut_ )
		: CommSgmlParser( encString_, true ), encStringOut( encStringOut_ ), styleBuf( NULL )
		{
		}
	virtual ~ImageSourceParser()
		{
		delete styleBuf;
		}

	private:
	virtual void imageSource( PString& source ) = 0;

	private:
	void _parseStyle( PUniString& buf );

	public:
	void parse( PString& buf );

	private: // CommSgmlParser overridables
	void uniCharacters( const PUNICHAR* begin, const PUNICHAR* end ) override;
	void tag( const char* name, CommSgmlTagAttributes* attributes ) override;

	private: // prohibit default copy constructor & assignment operator
	ImageSourceParser( const ImageSourceParser& other );
	ImageSourceParser& operator=( const ImageSourceParser& other );
	};

//------------------------------------------------------------------------------
#endif // !defined( __imagesourceparser_h_included__ )
