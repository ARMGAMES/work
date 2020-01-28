//------------------------------------------------------------------------------
//	ppsharedprefsmac.h
//------------------------------------------------------------------------------
//
//	Copyright (c) 2001-2007 PYR Software Ltd. All rights reserved. This document 
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

#if !defined(__ppsharedprefsmac_h_included__)
#define __ppsharedprefsmac_h_included__

#if defined(P_MAC10X)

//*****************************************************

#include <CoreFoundation/CoreFoundation.h>

// ****************************************************
// 
// CFPreferences_CopySharedAppValue( inKey, inApplicationID )
//
// Purpose:  This is a special version of CFPreferencesCopyAppValue that acesses
//           preferences stored in a shared location that writeable by non - admin users
//           ( /Users/Shared/Library/Preferences ).
//
// Inputs:   inKey - the key, must not be null
//           inApplicationID - the Application ID
//
// Returns:  CFPropertyListRef - the CFProperty list, Null if key not found
//
// Notes:    If not null then Caller must release the returned value
//

extern CFPropertyListRef CFPreferences_CopySharedAppValue( CFStringRef inKey, CFStringRef inApplicationID );


// ****************************************************
// 
// CFPreferences_SetSharedAppValue( inKey, inValue, inApplicationID )
//
// Purpose:  This is a special version of CFPreferences_SetAppValue that stores preferences in a 
//           location that is writeable by non - admin users ( /Users/Shared/Library/Preferences ).
//
// Inputs:   inKey - the key, must not be null
//           inValue - the value to be saved
//           inApplicationID - the Application ID
//
// Returns:  Boolean - TRUE if successful
//

extern Boolean CFPreferences_SetSharedAppValue( CFStringRef inKey, CFPropertyListRef inValue, CFStringRef inApplicationID );

//*****************************************************

#endif // !defined(P_MAC10X)

#endif // !defined(__ppsharedprefsmac_h_included__)
