//
//  Base26Converter.h
//  PokerStars
//
//

#ifndef Base26Converter_h
#define Base26Converter_h

#include "dbm.h"

// PYR-20741
class Base26Converter
{
public:
	Base26Converter()
	{
	}

	const char* toBase26( UINT64 val, PString& str )
	{
		vector< char > word;
		word.reserve( 20 );
		str.cut( 0 );
		do
		{
			UINT64 remainder = val % 26;
			word.push_back( _BASE_26_DICTIONARY_[ remainder ] );
			val = val / 26;
		}
		while( val > 0 );

		for( UINT32 i = 1; i <= word.size(); ++i )
		{
			str.append( word[ word.size() - i ] );
		}
		return str;
	}

	bool fromBase26( const char* str, UINT64& val )
	{
		val = 0;
		UINT32 len = PString::length( str );
		if( len > 13 )
			return false;
		for( UINT32 i = 0; i < len; ++i )
		{
			const char* p = PString::find( _BASE_26_DICTIONARY_, str[ i ] );
			if( !p )
			{
				val = 0;
				return false;
			}
			val *= 26;
			val += ( p - _BASE_26_DICTIONARY_ );
		}
		return true;
	}

	bool fromBase26( const char* str, UINT32& val )
	{
		val = 0;
		UINT32 len = PString::length( str );
		if( len > 7 )
			return false;
		for( UINT32 i = 0; i < len; ++i )
		{
			const char* p = PString::find( _BASE_26_DICTIONARY_, str[ i ] );
			if( !p )
			{
				val = 0;
				return false;
			}
			val *= 26;
			val += ( p - _BASE_26_DICTIONARY_ );
		}
		return true;
	}
};

#endif /* Base26Converter_h */
