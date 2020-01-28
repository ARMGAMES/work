#include "normuserid.h"
#include "dbm.h"

static const unsigned char _normMap[ 256 ] =
{
	#include "_normuserid.c"
};

void normalizeUserId( const char* userId, char* ret, size_t ret_sz )
{
	char* retEnd = ret + ret_sz - 1;//points to trailing 0 in case of too-long string
	for( ; *userId ; ++userId )
	{
		unsigned char c = _normMap[ (unsigned char)*userId ];//conversion to unsigned type is crucial!
		if( c )
		{
			if( ret >= retEnd )
				break;
			*ret++ = c;
		}
	}
	*ret = 0;
}

/// @return true if both IDs are the same after normalization
bool areUserIdsEqualNormalized( const char* userIdA, const char* userIdB )
{
	if (strcmp(userIdA, userIdB) == 0)
	{
		return true; // same IDs, no need for additional checks
	}

	char normalizedA[USER_NAME_LEN];
	char normalizedB[USER_NAME_LEN];

	normalizeUserId(userIdA, normalizedA, USER_NAME_LEN);
	normalizeUserId(userIdB, normalizedB, USER_NAME_LEN);
	
	return strcmp(normalizedA, normalizedB) == 0;
}
