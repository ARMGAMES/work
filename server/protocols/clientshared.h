// clientshare.h
#ifndef clientshared_h_included
#define clientshared_h_included

#include "dbm.h"

inline bool isBonusClearedWithCPP(UINT32 flags) // PYR-32329
{
	if ((flags & clearedWithCPPAndVPPCombination1) && (flags & clearedWithCPPAndVPPCombination2))
		return false;
	return (flags & clearedWithCPPAndVPPCombination1) || (flags & clearedWithCPPAndVPPCombination2);
}

inline bool isBonusClearedWithVPP(UINT32 flags) // PYR-32329
{
	if ((flags & clearedWithCPPAndVPPCombination1) && (flags & clearedWithCPPAndVPPCombination2))
		return false;
	return (flags & clearedWithCPPAndVPPCombination1) || ((flags & clearedWithCPPAndVPPCombination2) == 0);
}

inline bool isBonusClearedWithSPP(UINT32 flags) // PYR-32329
{
	return (flags & clearedWithSPP) != 0;
}

#endif // clientshared_h_included