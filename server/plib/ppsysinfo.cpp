#include "ppsystem.h"
#include "pplogfile.h"

#ifdef PWIN32

#ifndef PWIN64
PGetMemoryInfo::~PGetMemoryInfo()
{
	if (hm)
		FreeLibrary(hm);
}

bool PGetMemoryInfo::init()
{
	try
	{
		hm = LoadLibraryA( "psapi.dll" );
		if( hm )
		{
			fp = (HSI)(void*)GetProcAddress( hm, "GetProcessMemoryInfo" );
			if( fp )
			{
				return true;
			}
			else
			{
				FreeLibrary(hm);
				hm=0;
			}
		}
	}
	catch(const PError &er)
	{
		(void)er;
		PTRACE5("PGetMemoryInfo: error %s",er.why());
	}
	return false;
}

UINT32 PGetMemoryInfo::getWorkingSetSize()
{
	try
	{
		if (hm && fp)
		{
			PROCESS_MEMORY_COUNTERS_EX s;
			BOOL ret = fp(GetCurrentProcess(),&s,sizeof(s));
			if (ret)
			{
				PTRACE5("WorkingSetSize = %u,PagefileUsage=%u,PrivateUsage=%u",
					s.WorkingSetSize,s.PagefileUsage,s.PrivateUsage);

				return s.PagefileUsage;
			}
		}
	}
	catch(const PError &er)
	{
		(void)er;
		PTRACE5("PGetMemoryInfo: error %s",er.why());
	}
	return 0;
}

#endif // PWIN64


static void _getVolumeId( PString& ret, const char* dir )
{
	char drive[ _MAX_DRIVE ];
	_splitpath( dir, drive, 0, 0, 0 );
	if( *drive )
	{
		PString root( drive );
		root.append( "\\" );
		char label[ 64 ];
		char fs[ 32 ];
		DWORD id, maxFileLen, flags;
		GetVolumeInformationA( root, label, sizeof(label), &id, &maxFileLen, &flags, fs, sizeof(fs) );

		char buf[ 32 ];
		wsprintfA( buf, "%08X", id );
		ret = buf;
	}
	else
		ret = "";
}

//****************************************************************************

struct ASTAT
{
	ADAPTER_STATUS adapt;
	NAME_BUFFER    NameBuff [30];
};
typedef UCHAR (APIENTRY *NETBIOS)( PNCB pncb );

static void _getMacs( vector< PNameValue >& ret )
{
	HMODULE netapi = LoadLibraryA( "NETAPI32.DLL" );
	if( !netapi )
		return;

	NETBIOS netbios = (NETBIOS)(void*)GetProcAddress( netapi, "Netbios" );
	if( !netbios )
		return;

	NCB Ncb;
	//         char NetName[50];

	LANA_ENUM lenum;
	memset( &Ncb, 0, sizeof(Ncb) );
	Ncb.ncb_command = NCBENUM;
	Ncb.ncb_buffer = (UCHAR *)&lenum;
	Ncb.ncb_length = sizeof(lenum);
	UCHAR uRetCode = netbios( &Ncb );
	if( uRetCode != NRC_GOODRET )
		return;
	//	printf( "The NCBENUM return code is: 0x%x; n = %d\n", uRetCode, lenum.length );

	for( int i=0; i < lenum.length ;i++)
	{
		ASTAT Adapter;

		memset( &Ncb, 0, sizeof(Ncb) );
		Ncb.ncb_command = NCBRESET;
		Ncb.ncb_lana_num = lenum.lana[i];

		uRetCode = netbios( &Ncb );
		if( uRetCode != NRC_GOODRET )
			continue;
		//		printf( "The NCBRESET on LANA %d return code is: 0x%x \n",
		//				 lenum.lana[i], uRetCode );

		memset( &Ncb, 0, sizeof (Ncb) );
		Ncb.ncb_command = NCBASTAT;
		Ncb.ncb_lana_num = lenum.lana[i];
		memset( Ncb.ncb_callname, ' ', sizeof( Ncb.ncb_callname));
		Ncb.ncb_callname[0]='*';
		Ncb.ncb_buffer = (unsigned char *) &Adapter;
		Ncb.ncb_length = sizeof(Adapter);

		uRetCode = netbios( &Ncb );
		//		printf( "The NCBASTAT on LANA %d return code is: 0x%x \n",
		//			 lenum.lana[i], uRetCode );
		if( uRetCode == NRC_GOODRET )
		{
			char buf[ 32 ];
			wsprintfA( buf, "%02X%02X%02X%02X%02X%02X", 
				Adapter.adapt.adapter_address[0],
				Adapter.adapt.adapter_address[1],
				Adapter.adapt.adapter_address[2],
				Adapter.adapt.adapter_address[3],
				Adapter.adapt.adapter_address[4],
				Adapter.adapt.adapter_address[5] );
			ret.push_back( PNameValue( PSYSINFO_MAC, buf ) );
		}
	}
}

#define MAX_ADAPTER_DESCRIPTION_LENGTH  128 // arb.
#define MAX_ADAPTER_NAME_LENGTH         256 // arb.
#define MAX_ADAPTER_ADDRESS_LENGTH      8   // arb.
typedef struct {
	char String[4 * 4];
} IP_ADDRESS_STRING, *PIP_ADDRESS_STRING, IP_MASK_STRING, *PIP_MASK_STRING;

typedef struct _IP_ADDR_STRING {
	struct _IP_ADDR_STRING* Next;
	IP_ADDRESS_STRING IpAddress;
	IP_MASK_STRING IpMask;
	DWORD Context;
} IP_ADDR_STRING, *PIP_ADDR_STRING;

typedef struct _IP_ADAPTER_INFO 
{  struct _IP_ADAPTER_INFO* Next;  
DWORD ComboIndex; 
char AdapterName[MAX_ADAPTER_NAME_LENGTH + 4];  
char Description[MAX_ADAPTER_DESCRIPTION_LENGTH + 4]; 
UINT AddressLength; 
BYTE Address[MAX_ADAPTER_ADDRESS_LENGTH]; 
DWORD Index; 
UINT Type;  
UINT DhcpEnabled; 
PIP_ADDR_STRING CurrentIpAddress; 
IP_ADDR_STRING IpAddressList;  
IP_ADDR_STRING GatewayList; 
IP_ADDR_STRING DhcpServer; 
BOOL HaveWins; 
IP_ADDR_STRING PrimaryWinsServer; 
IP_ADDR_STRING SecondaryWinsServer; 
time_t LeaseObtained;  
time_t LeaseExpires;
} IP_ADAPTER_INFO,  *PIP_ADAPTER_INFO;

typedef DWORD (WINAPI * GetAdaptersInfo)(   PIP_ADAPTER_INFO pAdapterInfo, PULONG pOutBufLen  );

static void _getMacsEx( vector< PNameValue >& ret )
{

	HMODULE iphlpapi = LoadLibraryA( "iphlpapi.DLL" );
	if( !iphlpapi )
		return;

	GetAdaptersInfo getainfo = (GetAdaptersInfo)(void*)GetProcAddress( iphlpapi, "GetAdaptersInfo" );
	if( !getainfo )
		return;

	IP_ADAPTER_INFO AdapterInfo[16];  

	DWORD dwBufLen = sizeof(AdapterInfo); 

	DWORD dwStatus = getainfo(AdapterInfo, &dwBufLen);
	if (dwStatus != ERROR_SUCCESS)
		return;


	PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
	do 
	{
		char buf[ 32 ];
		wsprintfA( buf, "%02X%02X%02X%02X%02X%02X", 
			pAdapterInfo->Address[0],
			pAdapterInfo->Address[1],
			pAdapterInfo->Address[2],
			pAdapterInfo->Address[3],
			pAdapterInfo->Address[4],
			pAdapterInfo->Address[5] );
		ret.push_back( PNameValue( PSYSINFO_MAC, buf ) );
		pAdapterInfo = pAdapterInfo->Next; 
	}
	while(pAdapterInfo);    
	FreeLibrary( iphlpapi );
	return;
}

//****************************************************************************

struct _PWin32RegKey
{
public:
	HKEY key;

	_PWin32RegKey()
	{
		key = NULL;
	}
	~_PWin32RegKey()
	{
		if( key != NULL )
			RegCloseKey( key );
	}
};

static bool _writeHiddenKey( const char* name, DWORD value )
{
	_PWin32RegKey key1;
	LONG err = RegOpenKeyExA( HKEY_CURRENT_USER, "Software", 0, KEY_READ | KEY_WRITE, &key1.key );
	if( err != ERROR_SUCCESS )
		return false;

	_PWin32RegKey key2;
	DWORD disp;
	err = RegCreateKeyExA( key1.key, "VB and VBA Program Settings", 0, NULL, 0, KEY_READ | KEY_WRITE, NULL, &key2.key, &disp );
	if( err != ERROR_SUCCESS )
		return false;

	_PWin32RegKey key3;
	err = RegCreateKeyExA( key2.key, "Plugin", 0, NULL, 0, KEY_READ | KEY_WRITE, NULL, &key3.key, &disp );
	if( err != ERROR_SUCCESS )
		return false;

	DWORD val = value;
	err = RegSetValueExA( key3.key, name, 0, REG_DWORD, (BYTE*)&val, sizeof(DWORD) );
	if( err != ERROR_SUCCESS )
		return false;

	return true;
}

bool _readHiddenKey( const char* name, DWORD& value )
{
	_PWin32RegKey key1;
	LONG err = RegOpenKeyExA( HKEY_CURRENT_USER, "Software", 0, KEY_READ, &key1.key );
	if( err != ERROR_SUCCESS )
		return false;

	_PWin32RegKey key2;
	err = RegOpenKeyExA( key1.key, "VB and VBA Program Settings", 0, KEY_READ, &key2.key );
	if( err != ERROR_SUCCESS )
		return false;

	_PWin32RegKey key3;
	err = RegOpenKeyExA( key2.key, "Plugin", 0, KEY_READ, &key3.key );
	if( err != ERROR_SUCCESS )
		return false;

	DWORD val;
	DWORD type, sz = sizeof(DWORD);
	err = RegQueryValueExA( key3.key, name, 0, &type, (BYTE*)&val, &sz );
	if( err != ERROR_SUCCESS || type != REG_DWORD )
		return false;

	value = val;
	return true;
}

//****************************************************************************

#define INSTKEYA "InstanceA"
#define INSTKEYB "InstanceB"

/* virtual */ void PSystemInfo::generateInstallId( UINT32& a, UINT32& b )
{
	a = UINT32( TIME( 0 ) );
	b = SYSTEMTICKS();
}

bool PSystemInfo::getSystemInfo( vector< PNameValue >& ret )
{
	DWORD instA=0, instB=0;
	bool instIdGenerated = false;
	bool Ok = _readHiddenKey( INSTKEYA, instA );
	if( Ok )
		Ok = _readHiddenKey( INSTKEYB, instB );
	if( !Ok )
	{
		UINT32 a, b;
		generateInstallId( a, b );
		Ok = _writeHiddenKey( INSTKEYA, a );
		if( Ok )//verify
			Ok = _readHiddenKey( INSTKEYA, instA );
		if( Ok )
			Ok = _writeHiddenKey( INSTKEYB, b );
		if( Ok )//verify
			Ok = _readHiddenKey( INSTKEYB, instB );
		if( Ok )
			instIdGenerated = true;
	}
	if( Ok )
	{
		char buf[ 32 ];
		sprintf( buf, "%08lX%08lX", instA, instB );
		ret.push_back( PNameValue( PSYSINFO_INSTALL, buf ) );
	}

	char dir[ MAX_PATH ];
	UINT sysDirOk = GetSystemDirectoryA( dir, MAX_PATH );
	if( sysDirOk )
	{
		PString id;
		_getVolumeId( id, dir );
		if( *id )
			ret.push_back( PNameValue( PSYSINFO_SYSVOL, id ) );
	}

	_getMacs( ret );
	return instIdGenerated;
}

void PSystemInfo::getSystemInfoEx( vector< PNameValue >& ret )
{
	char dir[ MAX_PATH ];
	UINT sysDirOk = GetSystemDirectoryA( dir, MAX_PATH );
	if( sysDirOk )
	{
		PString id;
		_getVolumeId( id, dir );
		if( *id )
			ret.push_back( PNameValue( PSYSINFO_SYSVOL, id ) );
	}

	_getMacsEx( ret );
}

int PSystemInfo::getVersion() const
{
	return 0;
}

void PSystemInfo::setVersion( int v )
{
	PASSERT( v == 0 );
}


#elif defined( P_IOS )

#include "PS_SysInfo.h"
#include "IPAddress.h"

void PSystemInfo::generateInstallId( UINT32& a, UINT32& b )
{
}

bool PSystemInfo::getSystemInfo( vector< PNameValue >& ret )
{
	bool result = false;
	PString baseId;

	// Fetch saved version or save new if created
	CFStringRef value = (CFStringRef)CFPreferencesCopyAppValue( CFSTR("PSYSINFO_INSTALL"), kCFPreferencesCurrentApplication );
	if( value )
	{
		convertCFStringRef( value, baseId );
	}
	else
	{
		result = true;
		createInstallIdentifier( baseId );
		value = createCFStringRef( baseId );
		CFPreferencesSetAppValue( CFSTR("PSYSINFO_INSTALL"), value, kCFPreferencesCurrentApplication );
		CFPreferencesAppSynchronize( kCFPreferencesCurrentApplication );
	}
	CFSafeRelease( value );

	// Install ID
	PString id( baseId );
	id.cut( 16 );
	ret.push_back( PNameValue( PSYSINFO_INSTALL, id ) );
	
	// Add serial hash instead of Vol ID
	PString vol;
	generateMD5( baseId, vol );
	vol.cut( 12 );
	ret.push_back( PNameValue( PSYSINFO_SYSVOL, vol ) );
	
	const char* mac = _macAddress();
	// Add in MAC address
	ret.push_back( PNameValue( PSYSINFO_MAC, mac ) );
	
	return result;
}

void PSystemInfo::getSystemInfoEx( vector< PNameValue >& ret )
{
	return;
}

int PSystemInfo::getVersion() const
{
	return 0;
}

void PSystemInfo::setVersion( int v )
{
	PASSERT( v == 0 );
}

#elif defined (P_MAC10X)

#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/network/IOEthernetInterface.h>
#include <IOKit/network/IONetworkInterface.h>
#include <IOKit/network/IOEthernetController.h>
#include "ppsharedprefsmac.h"

static kern_return_t _findEthernetInterfaces( io_iterator_t *matchingServices )
{
	CFMutableDictionaryRef matchingDict;
	CFMutableDictionaryRef propertyMatchDict;

	matchingDict = IOServiceMatching( kIOEthernetInterfaceClass );

	if( matchingDict )
	{
		propertyMatchDict = CFDictionaryCreateMutable( kCFAllocatorDefault, 0,
			&kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks );

		if( propertyMatchDict )
		{
			CFDictionarySetValue( propertyMatchDict, CFSTR(kIOPrimaryInterface), kCFBooleanTrue ); 
			CFDictionarySetValue( matchingDict, CFSTR(kIOPropertyMatchKey), propertyMatchDict );
			CFRelease( propertyMatchDict );
		}
	}

	return IOServiceGetMatchingServices( kIOMasterPortDefault, matchingDict, matchingServices );    
}

static kern_return_t _getMACAddress( io_iterator_t intfIterator, UInt8 *MACAddress, UInt8 bufferSize )
{
	io_object_t intfService;
	io_object_t controllerService;
	kern_return_t kernResult = KERN_FAILURE;

	if( bufferSize < kIOEthernetAddressSize )
		return kernResult;

	bzero( MACAddress, bufferSize );
	while( intfService = IOIteratorNext( intfIterator ) )
	{
		kernResult = IORegistryEntryGetParentEntry( intfService,
			kIOServicePlane, &controllerService );

		if( KERN_SUCCESS == kernResult )
		{
			CFTypeRef MACAddressAsCFData = IORegistryEntryCreateCFProperty( controllerService,
				CFSTR(kIOMACAddress), kCFAllocatorDefault, 0 );
			if( MACAddressAsCFData )
			{
				CFDataGetBytes( (CFDataRef)MACAddressAsCFData, CFRangeMake(0, kIOEthernetAddressSize), MACAddress );
				CFRelease( MACAddressAsCFData );
			}
			IOObjectRelease( controllerService );
		}
		IOObjectRelease( intfService );
	}

	return kernResult;
}

static bool _getMAC( PString& addr )
{
	bool result = false;
	addr = 0;
	io_iterator_t intfIterator;
	if( KERN_SUCCESS == _findEthernetInterfaces( &intfIterator ) )
	{
		UInt8 MACAddress[kIOEthernetAddressSize];
		if( KERN_SUCCESS == _getMACAddress( intfIterator, MACAddress, sizeof(MACAddress) ) )
		{
			char buf[24];
			snprintf( buf, 24, "%02x%02x%02x%02x%02x%02x",
				MACAddress[0], MACAddress[1], MACAddress[2],
				MACAddress[3], MACAddress[4], MACAddress[5] );
			addr.append( buf );
			result = true;
		}
	}
	IOObjectRelease( intfIterator );
	return result;
}

void _getSerialNumber( PString& serial )
{
	io_service_t    platformExpert = IOServiceGetMatchingService(kIOMasterPortDefault,
		IOServiceMatching("IOPlatformExpertDevice"));

	if( platformExpert )
	{
		CFTypeRef serialNumber = IORegistryEntryCreateCFProperty( platformExpert,
			CFSTR(kIOPlatformSerialNumberKey),
			kCFAllocatorDefault, 0);
		if( serialNumber && (CFGetTypeID(serialNumber) == CFStringGetTypeID()) )
		{
			CFStringRef serialNumberAsCFString = (CFStringRef)serialNumber;
			if( serialNumberAsCFString )
			{
				char buf[256];
				if( CFStringGetCString( serialNumberAsCFString, buf, 256, kCFStringEncodingASCII ) )
				{
					serial = 0;
					serial.append( buf );
				}
			}
		}
		CFSafeRelease( serialNumber );
		IOObjectRelease(platformExpert);
	}
}
/* virtual */ void PSystemInfo::generateInstallId( UINT32& a, UINT32& b )
{
	a = TIME( 0 );
	b = SYSTEMTICKS();
}

static CFStringRef INST_APP_ID = CFSTR(".audio_codecs_sample_configX");
static CFStringRef INST_APP_ID_OLD = CFSTR("audio_codecs_sample_configX");
static CFStringRef INSTKEYA = CFSTR("InstanceA");

static void _calcChecksum( const PString& md5, PString& checksum )
{
	if( md5.length() > 0 )
	{
		char buf[ 16 ];
		int i, cnt = md5.length() / sizeof(UInt32);

		// First xor each 4 chars of md5
		UInt32 a = 0x5d64a863; // Salt
		const UInt32 *pa = (const UInt32*)md5.c_str();
		for( i = 0; i < cnt; ++i )
			a ^= *pa++;
		sprintf( buf, "%08lX", a );

		// Now assign to checksum
		checksum.assign( buf );
	}
	else
		checksum = 0;
}

static void _splitInstallId( const PString& value, PString& id, PString& checksum )
{
	int len = value.length();
	if( len >= 16 ) // Id + Checksum = 16 + 8 for proper value
	{
		id.assign( value, 16 );
		checksum = 0;
		for( int i = 16; i < len; ++i )
			checksum.append( value.charAt( i ) );
	}
	else
	{
		id = 0;
		checksum = 0;
	}
}

static bool _getInstallValues( CFStringRef app, CFStringRef key, PString& shared, PString& user )
{
	bool result = false;
	char buf[256];
	shared = 0;
	user = 0;
	// Shared
	CFPropertyListRef value = CFPreferences_CopySharedAppValue( key, app );
	if( value && (CFGetTypeID(value) == CFStringGetTypeID()) )
	{
		if( CFStringGetCString( (CFStringRef)value, buf, 256, kCFStringEncodingASCII ) )
		{
			shared.assign( buf );
			result = true;
		}
		CFRelease( value );
	}
	// User
	value = CFPreferencesCopyAppValue( key, app );
	if( value && (CFGetTypeID(value) == CFStringGetTypeID()) )
	{
		if( CFStringGetCString( (CFStringRef)value, buf, 256, kCFStringEncodingASCII ) )
		{
			user.assign( buf );
			result = true;
		}
		CFRelease( value );
	}
	return result;
}

static bool _setInstallValues( CFStringRef key, const PString& value )
{
	bool result = false;
	CFStringRef s = CFStringCreateWithCString( kCFAllocatorDefault, value, kCFStringEncodingASCII );
	if( s )
	{
		// Save in shared
		result = CFPreferences_SetSharedAppValue( key, s, INST_APP_ID );
		// And in user
		CFPreferencesSetAppValue( key, s, INST_APP_ID );
		result |= CFPreferencesAppSynchronize( INST_APP_ID );
		CFRelease( s );
	}
	return result;
}

bool PSystemInfo::getSystemInfo( vector< PNameValue >& ret )
{
	bool instIdGenerated = false;
	PString mac, serial;
	_getMAC( mac );
	_getSerialNumber( serial );

	PString id, checksum;

	PString shared, user;
	bool save = false;
	bool Ok = _getInstallValues( INST_APP_ID, INSTKEYA, shared, user );
	if( !Ok )
	{
		Ok = _getInstallValues( INST_APP_ID_OLD, INSTKEYA, shared, user );
		save = true;
	}
	if( Ok )
	{
		Ok = false;
		// Shared
		PString sharedId, sharedChecksum, sharedMD5;
		_splitInstallId( shared, sharedId, sharedChecksum );
		generateMD5( sharedId, sharedMD5 );
		PString sharedCheckC;
		_calcChecksum( sharedMD5, sharedCheckC );
		// User
		PString userId, userChecksum, userMD5;
		_splitInstallId( user, userId, userChecksum );
		generateMD5( userId, userMD5 );
		PString userCheckC;
		_calcChecksum( userMD5, userCheckC );
		// Now check them
		if( userChecksum.equals( userCheckC ) ) // User Ok
		{
			id.assign( userId );
			checksum.assign( userChecksum );
			Ok = true;
			if( !user.equals( shared ) )
				save = true;
		}
		else if( sharedChecksum.equals( sharedCheckC ) ) // Shared OK
		{
			id.assign( sharedId );
			checksum.assign( sharedChecksum );
			save = true;
			Ok = true;
		}
		else if( user.length() == 16 ) // No checksum
		{
			id.assign( userId );
			checksum.assign( userCheckC );
			Ok = true;
			save = true;
		}
		else if( shared.length() == 16 ) // No checksum
		{
			id.assign( sharedId );
			checksum.assign( sharedCheckC );
			Ok = true;
			save = true;
		}
		else
			save = true;
	}

	if( !Ok )
	{
		UINT32 a, b;
		generateInstallId( a, b );
		char buf[ 32 ];
		sprintf( buf, "%08X%08X", a, b );
		id.assign( buf );
		PString md5;
		generateMD5( id, md5 );
		_calcChecksum( md5, checksum );
		instIdGenerated = true;
		save = true;
	}
	if( save )
	{
		PString value;
		value.assign( id );
		value.append( checksum );
		Ok = _setInstallValues( INSTKEYA, value );
	}

	// Install ID
	if( Ok )
		ret.push_back( PNameValue( PSYSINFO_INSTALL, id ) );

	// Add serial hash instead of Vol ID
	PString vol;
	generateMD5( serial, vol );
	vol.cut(12);
	ret.push_back( PNameValue( PSYSINFO_SYSVOL, vol ) );

	// Add in MAC address
	ret.push_back( PNameValue( PSYSINFO_MAC, mac ) );

	return instIdGenerated;
}

void PSystemInfo::getSystemInfoEx( vector< PNameValue >& ret )
{
	return;
}

int PSystemInfo::getVersion() const
{
	return 0;
}

void PSystemInfo::setVersion( int v )
{
	PASSERT( v == 0 );
}

UINT64 getHighResCounter()
{
	struct timeval tv;
	gettimeofday( &tv, 0 );
	return (UINT64) tv.tv_sec * 1000000 + tv.tv_usec;

}

#elif defined (PPOSIX)

/* virtual */ void PSystemInfo::generateInstallId( UINT32& a, UINT32& b )
{
	a = TIME( 0 );
	b = SYSTEMTICKS();
}

bool PSystemInfo::getSystemInfo( vector< PNameValue >& ret )
{
	return false;
}

void PSystemInfo::getSystemInfoEx( vector< PNameValue >& ret )
{
	return;
}

int PSystemInfo::getVersion() const
{
	return 0;
}

void PSystemInfo::setVersion( int v )
{
	PASSERT( v == 0 );
}

UINT64 getHighResCounter()
{
	struct timeval tv;
	gettimeofday( &tv, 0 );
	return (UINT64) tv.tv_sec * 1000000 + tv.tv_usec;
}

#endif 


void logProcessHeapInfo()
{
#if defined (_WIN32) && !defined(COMM_CLIENT_ONLY)

	typedef BOOL(WINAPI *HQI)(
		HANDLE                 HeapHandle,
		HEAP_INFORMATION_CLASS HeapInformationClass,
		PVOID                  HeapInformation,
		SIZE_T                 HeapInformationLength,
		PSIZE_T                ReturnLength
		);


	HMODULE hl = LoadLibraryA("Kernel32.dll");
	if (hl)
	{
		HQI fp = (HQI)(void*)GetProcAddress(hl, "HeapQueryInformation"); // double cast to avoid compiler warning
		if (fp)
		{
			HANDLE heap = GetProcessHeap();
			ULONG heapInformation = 0;
			BOOL ret = fp(heap, HeapCompatibilityInformation, &heapInformation, sizeof(heapInformation), NULL);

			PLog("lPHI: heap=%p ret=%d heapInfo=%lld", (void*)(heap), INT32(ret), INT64(heapInformation));
		}
		else
		{
			PLog("lPHI: cannot find 'HeapQueryInformation' func");
		}
		FreeLibrary(hl);
	}
	else
	{
		PLog("lPHI: cannot load 'Kernel32.dll'");
	}
#else
#endif
}

void logCompilerInfo()
{
#if defined(_MSC_VER)
	const UINT32 buildNumber = (_MSC_FULL_VER < 100000000) ? (_MSC_FULL_VER % 10000) : (_MSC_FULL_VER % 100000);
	PLog("Microsoft C/C++ Compiler %u.%u.%u", _MSC_VER / 100, _MSC_VER % 100, buildNumber);
#elif defined(__INTEL_COMPILER)
	PLog("Intel compiler %d.%d", __INTEL_COMPILER / 100, (__INTEL_COMPILER % 100) / 10);
#elif defined(__clang__)
	PLog("Clang Compiler %u.%u.%u", __clang_major__, __clang_minor__, __clang_patchlevel__);
#elif defined(__GNUC__)
	PLog("GCC Compiler %u.%u.%u", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#else
	PLog("Unknown C/C++ Compiler");
#endif
}


//****************************************************************************

bool PGetSystemInfo( vector< PNameValue >& ret )
{
	PSystemInfo info;
	return info.getSystemInfo( ret );
}

void PGetSystemInfoEx( vector< PNameValue >& ret )
{
	PSystemInfo info;
	info.getSystemInfoEx( ret );
}

