// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Parts of this file are being maintained using automated scripts.
// The script maintenance parts are surrounded by language-specific
// comment lines that begin with <<== and end with ==>> markers.
// When required, manual changes can made to this file as usual. 
// Such changes should be communicated to the current maintainer 
// of GIT module in ADMIN/asppages/admin/_licenses. 
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#ifndef licenseid_h_included
#define licenseid_h_included

#include "liquidityPoolDefs.h" // PYR-58269

//when adding to this enum, please also change getFullDomainByLicense() accordingly
enum LicenceId
{
//	<<== @license2-definitions: License definitions @ ====
	eLicenceDefault		= 0,
	_LicenceDefaultBit_ForMaskUseOnly = 1, // To be used when it's necessary to store multiple licenses in one integer. See its usage in maskContains()
	eLicenceItaly          = ( 1 << 3 ),	//	8
	eLicenceFrance         = ( 1 << 4 ),	//	16
	eLicenceEstonia        = ( 1 << 5 ),	//	32
	eLicenceBelgium        = ( 1 << 6 ),	//	64
	eLicenceSpain          = ( 1 << 7 ),	//	128
	eLicenceDenmark        = ( 1 << 8 ),	//	256
	eLicenceEU             = ( 1 << 9 ),	//	512
	eLicenceFB             = ( 1 << 10),	//	1024
	eLicenceUSNJ           = ( 1 << 11),	//	2048
	eLicenceUK             = ( 1 << 12),	//	4096
	eLicenceBG             = ( 1 << 13),	//	8192
	eLicenceDESH           = ( 1 << 14),	//	16384
	eLicencePortugal_Old   = ( 1 << 15),	//	32768
	eLicenceRomania        = ( 1 << 16),	//	65536
	eLicencePortugal       = ( 1 << 17),	//	131072
	eLicenceCZ             = ( 1 << 18),	//	262144
	eLicenceIndia          = ( 1 << 19),	//	524288
	eLicenceAsia           = ( 1 << 20),	//	1048576
	eLicenceRU             = ( 1 << 21),	//	2097152
	eLicenceUSPA           = ( 1 << 22),	//	4194304
	eLicenceSE             = ( 1 << 23),	//	8388608
	eLicenceCH             = ( 1 << 24),	//	16777216
	eLicenceARBA           = (0x01000001),	//	16777217
	eLicenceUSCO           = (0x01000002),	//	16777218
	eLicenceDE             = (0x01000003),	//	16777219
	eLicenceGR             = (0x01000022),	//	16777250

//	Procedure for manually adding license:
//
//		1. Choose next license definition from possible candidates:
//				eLicenceFuture1 = 0x01000004,
//				eLicenceFuture2 = 0x01000005,
//				eLicenceFuture3 = 0x01000006,	...
//
//		2. Update licensemask.cpp/h. 

//  always keep eLicenceLast update to date
	eLicenceLast		= eLicenceGR,
//	==== @license2-definitions: License definitions @ ==>>

	eLicenceUNDEFINED   = 0x80000000, // (1<<31)=(-2147483648) stored in DB as signed negative value. if casted to UINT32 then +2147483648.
	                                // Should NOT be used in replication messages. On 2016.Main.08 is used by:
	                                // a - Majordomo for cases when licenseId is not present (NULL) PYR-36050
	                                // b - DB Tables LicenseId new column default value: starting from PYR-46483

	// Deprecated, use LicenceBitmask_SharedLiquidityPool instead.
	eLicenceSharedLiqudityPool = eLicenceSpain | eLicenceFrance | eLicencePortugal, // PYR-52938, PYR-65589

	// eLicenceAll is removed, use LicenceBitmask_All in the context of a licence mask
};

// PYR-109751 - Backwards compatible typedef for the enum above. Deprecated, use LicenceId instead.
[[deprecated("use LicenceId instead")]] typedef LicenceId LicenceIdFlags;

enum eHost
{
	// Hosts are not bitmasks. If bitmask is ever needed, it should be calulated as 2^(hostId-eHost_Central)
	eHost_Unknown = 0,				// may never be serialized
	eHost_Central = 1,				// the new proper name for IOM host in the code
	eHost_IOM     = eHost_Central,	// obsolete, do not use, kept for compatibility only
	// Specific peripheral host IDs are not listed on purpose. They can be any value in the range (eHost_Central,eHost_Maximum].

	// OBSOLETE AND WILL BE REMOVED in PYR-90648
	eHost_USNJ    = 2,

	eHost_Maximum = 4,				// the current limit imposed by OlapMsgSysInfo implementation
    // PYR-36050  = 31 is reserved by Majordomo
};

#endif // licenseid_h_included
