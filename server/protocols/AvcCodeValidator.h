//------------------------------------------------------------------------------
//	AvcCodeValidator.h
//------------------------------------------------------------------------------
//
//	Copyright (c) 2001-2010 PYR Software Ltd. All rights reserved. This document 
//	is for the use of licensed users only and is subject to the terms and 
//	conditions of the accompanying and/or applicable license between you and 
//	PYR Software Ltd, including without limitation those terms and conditions 
//	prohibiting unauthorized translation, reverse engineering, decompilation, 
//	disassembly, and other inappropriate use of any software provided. Any 
//	unauthorized use, copying, modification, distribution, publication or 
//	disclosure of information is a violation of copyright laws. No reproduction 
//	in whole or in part of this document may be made without express written 
//	consent of PYR Software Ltd.
//
//
//------------------------------------------------------------------------------

#ifndef	avccodevalidator_h_included
#define	avccodevalidator_h_included

#include "commlib.h"

class AvcCodeValidator
{
public:
	static bool validateAvcCode( UINT32 avc)
	{
		if( avc > 999999 || avc == 0)
			return false;

		UINT32 dig65 = avc / 10000;
		UINT32 dig43 = (avc % 10000) / 100;
		UINT32 dig21 = avc % 100;
		UINT32 div = (dig65 * 100 + dig21) % 97;
		return (div == dig43);
	}
};

#endif //avccodevalidator_h_included
