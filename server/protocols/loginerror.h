//loginerror.h
#ifndef loginerror_h_included
#define loginerror_h_included

enum AuthLogInCode
{
	AuthLogInNormal 					= 1000,
	AuthLogInLimitedRSA 				= 1001,
	AuthLogInLimitedSMS 				= 1002,
	AuthLogInLimitedEULA 				= 1004, // PYR-24802
	AuthLogInLimitedGeoIpPending 		= 1005, // PYR-27243
	AuthLogInLimitedGeoIpRejected 		= 1006, // PYR-27243
	AuthLogInLimitedGeoIpUnknown 		= 1007, // PYR-27243
	AuthLogInLimitedGeoIpBlockedSoftware= 1008, // PYR-27243
	AuthLogInLimitedGeoIpUsingProxy 	= 1009, // PYR-27243
	AuthLogInLimitedLTDL 				= 1010, // PYR-42099
	AuthLogInLimitedInExclusionBoundary = 1011, // PYR-74823
};

enum AuthErrorSubcode
{
	AuthSubErrorNone = 0,
	AuthSubErrorInternal = 1, // 44
	AuthSubErrorPwd = 2,
	//AuthSubErrorNoSuchUser = 3, // PYR-11185 - obsolete
	AuthSubErrorDoubleLoginAttempt = 4,
	AuthSubErrorWasConverted = 5,
	AuthSubErrorLicenseMismatchIt = 6,
	AuthSubErrorLicenseMismatchNotIt = 7,
	//AuthSubErrorGracePeriod = 8, // (D) - not in use after #21414
	AuthSubErrorRestricted = 9,
	AuthSubErrorItOnCOM = 10,
	AuthSubErrorAcctountClosed = 11,
	AuthSubErrorLicenseMismatch = 12,
	AuthSubErrorFrOnCOM = 13,
	AuthSubErrorEeOnCOM = 14,
	AuthSubErrorHgNotAllowed = 15,
	AuthSubErrorBeOnCOM = 16,
	AuthSubErrorHgNotAllowedFr = 17,
	AuthSubErrorEsOnCOM = 18,
	AuthSubErrorDkOnCOM = 19,
	AuthSubErrorAcctountClosed2 = 20,
	AuthSubErrorAPCOnDiffDevice		= 21,		//	PYR-52886. Authenticated Poker Connection exists on a different device
	AuthSubErrorAPCOnSameDevice		= 22,		//	PYR-52886. Authenticated Poker Connection exists on the same device
	AuthSubErrorTransferAPCNotReady = 23,		//	PYR-52886. Transfer APC Login not ready. Client should retry.
};

#endif //loginerror_h_included
