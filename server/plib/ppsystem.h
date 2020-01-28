#ifndef ppsystem_h_included
#define ppsystem_h_included

#include "ppinclude.h"
#include "ppstring.h"
#include "ppcontainer.h"

#define PSYSINFO_SYSVOL		"SYSVOL"
#define PSYSINFO_MAC		"MAC"
#define PSYSINFO_INSTALL	"INSTALL"

class PSystemInfo
	{
	public:
	bool getSystemInfo( vector< PNameValue >& ret );//NOTE: may generate and set install id, returns true if new install id was generated
	void getSystemInfoEx( vector< PNameValue >& ret );//NOTE: may generate and set install id, returns true if new install id was generated
	virtual void generateInstallId( UINT32& a, UINT32& b );//default implementation is time-based
	virtual void generateMD5( const PString& s, PString& md5hash ) { md5hash = s; }//default implementation is copy

	public:
	//IMPORTANT: if anything in PSystemInfo implementation is changed, new version # must be assigned 
	//           and old implementation behaviour MUST be 100% preserved when setVersion( old_version_number ) is called
	//           This is necessary e.g. for password encryption stuff
	int getVersion() const;
	void setVersion( int v );//if called with invalid v, throws; if not called, the most recent implementation assumed
	};

bool PGetSystemInfo( vector< PNameValue >& ret );//calls PSystemInfo::getSystemInfo
void PGetSystemInfoEx( vector< PNameValue >& ret );//calls PSystemInfo::getSystemInfo
void logProcessHeapInfo();
void logCompilerInfo();

#ifdef PWIN32
#ifndef PWIN64

class PGetMemoryInfo
{
private:
	typedef struct _PROCESS_MEMORY_COUNTERS 
	{
		DWORD cb;
		DWORD PageFaultCount;
		SIZE_T PeakWorkingSetSize;
		SIZE_T WorkingSetSize;
		SIZE_T QuotaPeakPagedPoolUsage;
		SIZE_T QuotaPagedPoolUsage;
		SIZE_T QuotaPeakNonPagedPoolUsage;
		SIZE_T QuotaNonPagedPoolUsage;
		SIZE_T PagefileUsage;
		SIZE_T PeakPagefileUsage;
		SIZE_T PrivateUsage;
	} PROCESS_MEMORY_COUNTERS_EX;
	typedef BOOL (WINAPI *HSI)
		(
		HANDLE HeapHandle,
		PROCESS_MEMORY_COUNTERS_EX *s,
		SIZE_T HeapInformationLength
		);

	
	HMODULE hm;
	HSI fp;
public:
	PGetMemoryInfo()
	{
		hm = 0;
		fp = 0;
	}
	~PGetMemoryInfo();
	bool init();
	UINT32 getWorkingSetSize();
};


#endif
#endif



#endif
