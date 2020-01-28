#include "ppclient.h"

#ifdef PWIN32

#include "ppwinwnd.h"
#include "pplogfile.h"

long POpenFile( const char* path, const PStringEncoding* encString )
{
	return (long)_PWin32ShellExecute( NULL, "open", path, NULL, NULL, encString, SW_SHOWNORMAL );
}

long POpenFolder( const char* path, const PStringEncoding* encString )
{
	return POpenFile( path, encString );
}

long POpenBrowserWindow( const char* url, const PStringEncoding* encString )
{
	PTRACE4( "OpenBrowserWindow( %s )", url );
	return POpenFile( url, encString );
}

void PStrCopyToClipboard( const char* str, const PStringEncoding* encString )
{
	_PWin32CopyToClipboard( str, encString );
}

#elif defined (P_MAC10X)

#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>
#include "UniString_Mac.h"

long POpenFile( const char* path, const PStringEncoding* encString )
{
	OSStatus err = kLSDataUnavailableErr;
	if( path && *path )
	{
		UniString_Mac u( path, encString );
		CFURLRef theURL;
		
		const char* mailPrefix = "mailto:";
		if( !strncmp( path, mailPrefix, strlen( mailPrefix ) ) )
			theURL = CFURLCreateWithString( kCFAllocatorDefault, u, NULL );
		else
			theURL = CFURLCreateWithFileSystemPath( kCFAllocatorDefault, u, kCFURLPOSIXPathStyle, false );
		
		// Now open it
		if( theURL )
			err = LSOpenCFURLRef( theURL, NULL ); // For 10.4 use LSOpenURLsWithRole
		CFSafeRelease( theURL );
	}
	return err;
}

long POpenFolder( const char* path, const PStringEncoding* encString )
{
	OSStatus err = kLSDataUnavailableErr;
	if( path && *path )
	{
		UniString_Mac u( path, encString );
		CFURLRef theURL = CFURLCreateWithFileSystemPath( kCFAllocatorDefault, u, kCFURLPOSIXPathStyle, true );

		// Now open it
		if( theURL )
			err = LSOpenCFURLRef( theURL, NULL ); // For 10.4 use LSOpenURLsWithRole
		CFSafeRelease( theURL );
	}
	return err;
}

long POpenBrowserWindow( const char* url, const PStringEncoding* encString )
{
	OSStatus err = kLSDataUnavailableErr;
	if( url && *url )
	{
		UniString_Mac u( url, encString );
		CFMutableStringRef ms = CFStringCreateMutableCopy( kCFAllocatorDefault, 0, u );
		CFStringFindAndReplace( ms, CFSTR("\r"), CFSTR(""), CFRangeMake(0, CFStringGetLength(ms)), 0 );
		CFStringFindAndReplace( ms, CFSTR("\n"), CFSTR(""), CFRangeMake(0, CFStringGetLength(ms)), 0 );
		CFStringFindAndReplace( ms, CFSTR("\t"), CFSTR(""), CFRangeMake(0, CFStringGetLength(ms)), 0 );
		CFStringFindAndReplace( ms, CFSTR(" "), CFSTR("%20"), CFRangeMake(0, CFStringGetLength(ms)), 0 );
		CFURLRef theURL = CFURLCreateWithString( kCFAllocatorDefault, ms, NULL );
		CFSafeRelease( ms );
		//			UInt8 s[1024];
		//			CFIndex len;
		//			CFStringGetBytes( u, CFRangeMake(0, u.length()), kCFStringEncodingUTF8, ' ', false, s, 1024, &len );
		//			CFURLRef theURL = CFURLCreateWithBytes( kCFAllocatorDefault, s, len, kCFStringEncodingUTF8, NULL );
		if( theURL ) // Now check for proper schema
		{
			CFStringRef schema = CFURLCopyScheme( theURL );
			if( !schema )
			{
				CFSafeRelease( theURL );
				UniString_Mac u2( CFSTR("http://") );
				u2 += u;
				theURL = CFURLCreateWithString( kCFAllocatorDefault, u2, NULL );
			}
			CFSafeRelease( schema );
		}
		else // Try file system
		{
			theURL = CFURLCreateWithFileSystemPath( kCFAllocatorDefault, u, kCFURLPOSIXPathStyle, false );
		}

		// Now open it
		if( theURL )
			err = LSOpenCFURLRef( theURL, NULL ); // For 10.4 use LSOpenURLsWithRole
		CFSafeRelease( theURL );
	}
	return err;
}

#endif