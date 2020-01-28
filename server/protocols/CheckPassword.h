// ===========================================================
// Copyright (C) 2018 PYR Software Ltd. All rights reserved.
// ===========================================================

#ifndef	checkpassword_h_included
#define	checkpassword_h_included

#include "ppinclude.h"
#include "commlib.h"
#include "pyrtime.h"

enum EnumPasswordCheckRes : INT32
{
	pcrSucess							= 0,
	pcrErrorContainsUserID				= 1,
	pcrErrorContainsFirstName			= 2,
	pcrErrorContainsLastName			= 3,
	pcrErrorContainsDateOfBirth			= 4,
	pcrErrorNotStartingWithAlphaChar	= 5,	//	A-Za-z
	pcrErrorNotEnoughCharacterTypes		= 6,	//	numbers, symbols, upper case, lower case
	pcrErrorTooShort					= 7,
	pcrErrorIsTemporaryPassword			= 8,
	pcrErrorContainsSecondLastName		= 9,
};

EnumPasswordCheckRes checkPassword( UINT32 licenseId,
									const char* userId,
									const char* password,
									const char* firstName,
									const char* lastName,
									const char* secondLastName,
									const SrvDate& dateOfBirth );	// firstName, lastName and dateOfBirth are enforced if present


#endif	//	checkpassword_h_included
