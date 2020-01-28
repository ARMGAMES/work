#ifndef tlbbop_h_included
#define tlbbop_h_included

enum TlbStandingsListType
{
	eTlbYear = 0,
	eTlbMonth = 1,
	eTlbWeek = 2,
	eTlbInterval = 3,
	eTlbCountry = 4,
	eTlbPrevYear = 5,
	eTlbPrevMonth = 6,
	eTlbPrevWeek = 7,

	// PYR-53981 - If adding to the enum, please keep this up-to-date.
	TlbStandingsListType_last = eTlbPrevWeek,
};

enum TlbPlanetKeys
{ //lint -e{849} (duplicate enum values)
	eKeyRegularTlb = 0, // Regular TLB board
	eKeyItRegularTlb = 100, // Regular TLB board
							// PYR-21301
	eKeyItMini		= 151,
	eKeyItLow		= 153,
	eKeyItMedium	= 155,
	eKeyItHigh		= 157,

	// PYR-15925
	eKeyFrRegularTlb = 200, // Regular TLB board

	// PYR-27225
	eKeyUSNJRegularTlb = 300, // Regular TLB board
};

// [MJ 2010-08-20: #15925]
inline bool isTlbKeyRegular(UINT32 tlbKey)  
{
	return (tlbKey == eKeyRegularTlb) || (tlbKey == eKeyItRegularTlb) || (tlbKey == eKeyFrRegularTlb) || (tlbKey == eKeyUSNJRegularTlb);
}

inline bool isNewITBopLevels(UINT32 tlbKey)
{
	return (tlbKey >= eKeyItMini) && (tlbKey <= eKeyItHigh);
}

#endif // tlbbop_h_included
