#ifndef	stringroutines_h_included
#define	stringroutines_h_included

#include "ppstring.h"
#include <vector>

using namespace std;

// copied from shared.h so that client team can use it.
void splitString(const char* str, vector<PString>& strVec, char delimeter, bool keepEmpty, bool trim);
bool splitBracketedString( const char *source, vector<PString>& output, PString& errStr, bool ignoreNested = false );
void stripHtmlTags(const char* text, PString& res);
inline bool emptyOrOnlySpacesAndTabs( const char* str ) // PYR-52977 - helper function to check for empty userId.
{
	return !*str || str == PString::rtrim( str );
}

//Populates result with substring surrounded with startTag string and terminator character.
//If terminator is not found composes the rest of source string.
//Ex: source="[abc]:123[def]:234"  getTaggedSubstring( source, "[abc]:", '[', result ); => result="123"
//								   getTaggedSubstring( source, "[def]:", '[', result ); => result="234"
bool getTaggedSubstring( const char* source, const char* startTag, char terminator, PString& result );

// PYR-92250 
class StringTokenUtil
{
public:
	static const char* getRhsFromTokenUntilNL( const char* strIn, const char* keyName, PString& pstrOut )
	{
		pstrOut.cut( 0 );
		const char* keyStart = PString::find( strIn, keyName );
		if( keyStart )
		{
			keyStart += strlen( keyName );
			const char* keyEnd = PString::find( keyStart, '\n' );
			if( keyEnd )
			{
				pstrOut.assign( keyStart, keyEnd - keyStart );
			}
			else
			{
				pstrOut.append( keyStart );
			}
		}
		return pstrOut;
	}
};

#endif
