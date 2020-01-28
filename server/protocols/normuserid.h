#ifndef	normuserid_h_included
#define	normuserid_h_included

#include "ppincludebase.h"

void normalizeUserId( const char* userId, char* ret, size_t ret_sz );
/// @return true if both IDs are the same after normalization
bool areUserIdsEqualNormalized( const char* userIdA, const char* userIdB );

#endif
