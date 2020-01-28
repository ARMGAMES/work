#ifndef cliutils_h_included
#define cliutils_h_included
//------------------------------------------------------------------------------

#include "lobby.h"

inline INT64 bigBlind( PotStruct potStruct, INT64 loBet, INT64 hiBet )
{
	switch( potStruct )
	{
	case StructLimit:
	case StructMultiGameMixed:
		return loBet;
	}
	return hiBet;
}

//------------------------------------------------------------------------------
#endif	// clitable_h_included
