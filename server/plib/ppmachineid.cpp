//------------------------------------------------------------------------------
//	ppmachineid.cpp
//------------------------------------------------------------------------------

#include "ppmachineid.h"
#include "ppstring.h"

//------------------------------------------------------------------------------

#ifdef PWIN32

//------------------------------------------------------------------------------

static const char* _getMachineIdImpl( PString& machineUIID )
{
	const DWORD dwBuffSize = 1024;
	DWORD dwBuffLen = dwBuffSize;
	char regKeyBuff[dwBuffSize];
	ZeroMemory( regKeyBuff, sizeof( regKeyBuff ) );

	HKEY hKey;
	DWORD type = REG_SZ;
	bool success = false;

	// attempt normal registry operations
	LONG lRet = RegOpenKeyExA( HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Cryptography\\", 0, KEY_READ | KEY_QUERY_VALUE, &hKey );
	if( lRet == ERROR_SUCCESS )
	{
		lRet = RegQueryValueExA( hKey, "MachineGuid", NULL, &type, ( LPBYTE )regKeyBuff, &dwBuffLen );
		RegCloseKey( hKey );

		if( ( lRet == ERROR_SUCCESS ) && dwBuffLen && ( dwBuffLen <= dwBuffSize ) )
		{
			success = true;
		}
	}

	if( !success )
	{
		// attempt redirected registry operation
		lRet = RegOpenKeyExA( HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Cryptography\\", 0 , KEY_WOW64_64KEY | KEY_READ | KEY_QUERY_VALUE, &hKey );
		if( lRet == ERROR_SUCCESS )
		{
			lRet = RegQueryValueExA( hKey, "MachineGuid", NULL, &type, ( LPBYTE )regKeyBuff, &dwBuffLen );
			RegCloseKey( hKey );

			if( ( lRet == ERROR_SUCCESS ) && dwBuffLen && ( dwBuffLen <= dwBuffSize ) )
			{
				success = true;
			}
		}
	}

	if( success )
	{
		for( const char* p = regKeyBuff ; *p ; ++p )
		{
			if( *p != '-' )
			{
				machineUIID.append( *p );
			}
		}

		return machineUIID.c_str();
	}

	return nullptr;
}

//------------------------------------------------------------------------------
#elif defined ( P_MAC10X )
//------------------------------------------------------------------------------

#include <IOKit/IOKitLib.h>

static const char* _getMachineIdImpl( PString& machineUIID )
{
	try
	{
		io_registry_entry_t ioRegRoot = IORegistryEntryFromPath( kIOMasterPortDefault, "IOService:/" );
		if( MACH_PORT_NULL == ioRegRoot )
			return nullptr;
		CFStringRef sUuid = ( CFStringRef ) IORegistryEntryCreateCFProperty( ioRegRoot, CFSTR( kIOPlatformUUIDKey ), kCFAllocatorDefault, 0 );
		IOObjectRelease( ioRegRoot );
		if( nullptr == sUuid )
			return nullptr;
		CFMutableStringRef ms = CFStringCreateMutableCopy( kCFAllocatorDefault, 0, sUuid );
		CFStringFindAndReplace( ms, CFSTR("-"), CFSTR(""), CFRangeMake( 0, CFStringGetLength( ms ) ), 0 );
		machineUIID.assign( CFStringGetCStringPtr( ms, kCFStringEncodingUTF8 ) );
		CFSafeRelease( ms );
		CFSafeRelease( sUuid );
	}
	catch( ... )
	{
		return nullptr;
	}

	return machineUIID.c_str();
}

//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------
#if defined( PWIN32 ) || defined( P_MAC10X )
//------------------------------------------------------------------------------

const char* getMachineId()
{
	static PString machineIdStr;
	static const char* machineId = _getMachineIdImpl( machineIdStr );
	return machineId;
}

//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------

