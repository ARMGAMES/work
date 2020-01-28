//------------------------------------------------------------------------------
//	ppsharedprefsmac.cpp
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

#if defined(P_MAC10X)

// ***************************************************

#include <CoreFoundation/CoreFoundation.h>
#include <stdio.h>		// printf, fprinf, stderr
#include <fcntl.h>		// open
#include <unistd.h>		// close
#include <sys/stat.h>	// umask, chmod
#if !defined( P_IOS ) && !defined( MAC_COCOA ) 
	#include <Carbon/Carbon.h>
#endif // !defined( P_IOS )

#include "ppsharedprefsmac.h"

#ifndef CFSafeRelease	
#define CFSafeRelease(_cf)					\
	do										\
		{									\
		if ((_cf) != NULL)					\
			{								\
			CFRelease((CFTypeRef) _cf);		\
			_cf = NULL;						\
			}								\
		}									\
	while (0)
#endif // CFSafeRelease

// ***************************************************

#if !defined( MAC_COCOA )
static
#endif
CFURLRef CFPreferences_CopySharedCFURLRef( CFStringRef inApplicationID );
static SInt32 CFProperty_SaveToXMLFile( CFPropertyListRef inCFPRef, CFURLRef inCFURLRef );
static CFPropertyListRef CFProperty_CreateFromXMLFile( CFURLRef inCFURLRef, CFOptionFlags inMutabilityOption );

// ****************************************************
// 
// CFPreferences_CopySharedAppValue( inKey, inApplicationID )
//
// Purpose:	 This is a special version of CFPreferences_CopyAppValue that acesses
//			 preferences stored in a shared location that writeable by non - admin users
//			 ( /Users / Shared / Library / Preferences ).
//
// Inputs:	 inKey - the key, must not be null
//			 inApplicationID - the Application ID
//
// Returns:	 CFPropertyListRef - the CFProperty list, Null if key not found
//
// Notes:	 If not null then Caller must release the returned value
//

CFPropertyListRef CFPreferences_CopySharedAppValue( CFStringRef inKey, CFStringRef inApplicationID )
{
	CFPropertyListRef tCFPropertyListRef = NULL;
	
	if( inKey ) // if the input key is valid
	{
			// create a URL to the shared preferences file
		CFURLRef tCFURLRef = CFPreferences_CopySharedCFURLRef( inApplicationID );
		if( tCFURLRef ) // if that succedded ...
		{
				// load the existing preferences from the file
			CFDictionaryRef tCFDictionaryRef = (CFDictionaryRef)
								CFProperty_CreateFromXMLFile( tCFURLRef, kCFPropertyListImmutable );
			// if this was successful and of the correct type ... 
			if( tCFDictionaryRef && ( CFDictionaryGetTypeID() == CFGetTypeID( tCFDictionaryRef ) ) )
			{
				// look for our preference in the dictionary
				if( CFDictionaryGetValueIfPresent( tCFDictionaryRef, inKey, (const void**)&tCFPropertyListRef ) )
					CFRetain( tCFPropertyListRef );
			}
			CFSafeRelease( tCFDictionaryRef );
		}
		CFSafeRelease( tCFURLRef );
	}

	return tCFPropertyListRef;
}	// CFPreferences_CopySharedAppValue

// ****************************************************
// 
// CFPreferences_SetSharedAppValue( inKey, inValue, inApplicationID )
//
// Purpose:	 This is a special version of CFPreferences_SetAppValue that stores preferences in a 
//			 location that is writeable by non - admin users ( /Users / Shared / Library / Preferences ).
//
// Inputs:	 inKey - the key, must not be null
//			 inValue - the value to be saved
//			 inApplicationID - the Application ID
//
// Returns:	 Boolean - TRUE if successful
//

Boolean CFPreferences_SetSharedAppValue( CFStringRef inKey, CFPropertyListRef inValue, CFStringRef inApplicationID )
{
	Boolean result = FALSE;	 // assume failure ( pessimist! )
	
	// create a URL to the shared preferences file
	CFURLRef tCFURLRef = CFPreferences_CopySharedCFURLRef( inApplicationID	);
	if( tCFURLRef ) // if that succedded ...
	{
					   // load the existing preferences from the file
		CFMutableDictionaryRef	tCFMutableDictionaryRef = (CFMutableDictionaryRef)
							CFProperty_CreateFromXMLFile( tCFURLRef, kCFPropertyListMutableContainers );
		
		// if this failed or was of the wrong type ... 
		if( !tCFMutableDictionaryRef || ( CFDictionaryGetTypeID( ) != CFGetTypeID( tCFMutableDictionaryRef ) ) )
		{
			CFSafeRelease( tCFMutableDictionaryRef );
			tCFMutableDictionaryRef = CFDictionaryCreateMutable( kCFAllocatorDefault, 0, &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks );
		}
		
		// if we have a dictionary now ... 
		if( tCFMutableDictionaryRef )
		{
			if( inValue ) // then set our value
				CFDictionarySetValue( tCFMutableDictionaryRef, inKey, inValue );
			else
				CFDictionaryRemoveValue( tCFMutableDictionaryRef, inKey );
			// and save our preferences back out to the file
			SInt32 status = CFProperty_SaveToXMLFile( tCFMutableDictionaryRef, tCFURLRef );
			result = ( noErr == status );
			
		}
		CFSafeRelease( tCFMutableDictionaryRef );
	}
	CFSafeRelease( tCFURLRef );
	return result;
}	// CFPreferences_SetSharedAppValue

// ***************************************************
// 
//	CFPreferences_CopySharedCFURLRef( inApplicationID )
// 
//	Purpose:  return the URL for the shared preferences file for this application
// 
//	Inputs:	  inApplicationID - the application ID
// 
//	Returns:  CFURLRef - the URL
// 
#if !defined( MAC_COCOA )
static CFURLRef CFPreferences_CopySharedCFURLRef( CFStringRef inApplicationID )
{
	CFURLRef	result = NULL;
	OSStatus	status = noErr;

	// if kCFPreferencesCurrentApplication is used as the inApplicationID ... 
	//		 ... convert that to the identifier of the current application.
#if !defined( P_IOS )
	if( CFEqual( inApplicationID, kCFPreferencesCurrentApplication ) )
	{
		inApplicationID = CFBundleGetIdentifier( CFBundleGetMainBundle( ) );
		if( !inApplicationID )
			status = coreFoundationUnknownErr;
	}

	FSRef sharedFSRef;
	status = FSFindFolder( kOnAppropriateDisk, kSharedUserDataFolderType, TRUE, &sharedFSRef );
	if( noErr == status )
	{
		CFURLRef sharedURL = CFURLCreateFromFSRef( kCFAllocatorDefault, &sharedFSRef );
		if( sharedURL )
		{
			FSRef prefsFSRef;
			status = FSFindFolder( kUserDomain, kPreferencesFolderType, TRUE, &prefsFSRef );
			if( noErr == status )
			{
				CFURLRef prefsURL = CFURLCreateFromFSRef( kCFAllocatorDefault, &prefsFSRef );
				if( prefsURL )
				{
					CFStringRef prefsFolder = CFURLCopyLastPathComponent( prefsURL );
					CFURLRef parent	 = CFURLCreateCopyDeletingLastPathComponent( kCFAllocatorDefault, prefsURL );
					CFStringRef libraryFolder = CFURLCopyLastPathComponent( parent );
					CFSafeRelease( parent );
					if( prefsFolder && libraryFolder )
					{
						CFURLRef sharedLibraryURL = CFURLCreateCopyAppendingPathComponent( kCFAllocatorDefault, sharedURL, libraryFolder, true );
						CFURLRef sharedPrefsURL = CFURLCreateCopyAppendingPathComponent( kCFAllocatorDefault, sharedLibraryURL, prefsFolder, true );
						CFSafeRelease( sharedLibraryURL );
						if( sharedPrefsURL )
						{
							CFStringRef prefsFile = CFStringCreateWithFormat( kCFAllocatorDefault, NULL, CFSTR("%@.plist"), inApplicationID );
							result = CFURLCreateCopyAppendingPathComponent( kCFAllocatorDefault, sharedPrefsURL, prefsFile, false );
							CFSafeRelease( prefsFile );
						}
						CFSafeRelease( sharedPrefsURL );
					}
					CFSafeRelease( libraryFolder );
					CFSafeRelease( prefsFolder );
				}
				CFSafeRelease( prefsURL );
			}
		}
		CFSafeRelease( sharedURL );
	}
#endif // !defined( P_IOS )
	
	return result;
}	// CFPreferences_CopySharedCFURLRef
#endif

// *****************************************************
// 
//	CFProperty_SaveToXMLFile( inCFPRef, inCFURLRef )
// 
//	Purpose:  saves CFProperty list into an XML file
// 
//	Inputs:	  inCFPRef - the CFProperty list to save
//			  inCFURLRef - the file to save it to
// 
//	Returns:  SInt32 - error code ( 0 == no error ) 
// 
static SInt32 CFProperty_SaveToXMLFile( CFPropertyListRef inCFPRef, CFURLRef inCFURLRef )
{
	SInt32		result = noErr;

#if !defined( P_IOS )
	mode_t		oldMode = umask( 0 );
	
	char		urlPath[PATH_MAX];
	if( !CFURLGetFileSystemRepresentation( inCFURLRef, TRUE, ( UInt8 * ) urlPath, PATH_MAX ) )
		result = 1;
	
	// Create any directories leading up to the file.  
	// CFURLWriteDataAndPropertiesToResource will create the file itself, if necessary.
	
	if( noErr == result )
	{
		char* cursor = urlPath + 1;	// start after leading slash

		while( TRUE )
		{
			cursor = strchr( cursor, '/' ); // look for the next slash
			if( !cursor )	// no next slash, we've reached the last component
			{
				result = noErr;
				break;
			}
			
			*cursor = 0;					// terminate string at the slash
			
			result = mkdir( urlPath, ALLPERMS );
			if( result < 0 )
			{
				result = errno;
				if ( errno != EEXIST )
					break;
			}

			*cursor = '/';					// restore the slash
			cursor += 1;					// skip over the slash
		}
	}
	
	// Convert the property list into XML data.
	
	CFDataRef	xmlCFDataRef = NULL;
	if ( noErr == result )
	{
		xmlCFDataRef = CFPropertyListCreateXMLData( kCFAllocatorDefault, inCFPRef );
		if ( !xmlCFDataRef )
			result = 1;
	}
	
	// Write the XML data to the file.
	
	if( noErr == result )
	{
		if( CFURLWriteDataAndPropertiesToResource( inCFURLRef, xmlCFDataRef, NULL, &result ) )
			result = noErr;
	}
	
	(void)umask( oldMode );
	CFSafeRelease( xmlCFDataRef );
#endif // !defined( P_IOS )
	
	return result;
}	// CFProperty_SaveToXMLFile

// *****************************************************
// 
//	CFProperty_CreateFromXMLFile( inCFURLRef, inMutabilityOption )
// 
//	Purpose:  loads a CFProperty list from an XML file
// 
//	Inputs:	  inCFURLRef - the file to load it from
//			inMutabilityOption - the mutability option
//
//	Returns:  inCFPRef - the CFProperty list
//
static CFPropertyListRef CFProperty_CreateFromXMLFile( CFURLRef inCFURLRef, CFOptionFlags inMutabilityOption )
{
	CFDataRef xmlCFDataRef = NULL;
	CFPropertyListRef myCFPropertyListRef = NULL;

	// Read the XML file.
	if ( CFURLCreateDataAndPropertiesFromResource( kCFAllocatorDefault, inCFURLRef, &xmlCFDataRef, NULL, NULL, NULL ) )
		myCFPropertyListRef = CFPropertyListCreateFromXMLData( kCFAllocatorDefault, xmlCFDataRef, inMutabilityOption, NULL );
	CFSafeRelease( xmlCFDataRef );
	
	return myCFPropertyListRef;
}	// CFProperty_CreateFromXMLFile

#endif // !defined(P_MAC10X)
