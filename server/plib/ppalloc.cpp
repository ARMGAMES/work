#include "ppinclude.h"
#include "ppalloc.h"

#ifdef PWIN32
/* static */ int _PAllocStartIndexGetter::gcpnInited = 0;
/* static */ _GetCurrentProcessorNumber _PAllocStartIndexGetter::gcpn = 0;

/* static */ void _PAllocStartIndexGetter::_initGcpn()
	{
#ifdef PWINCE
	//leave gcpn == 0;
	gcpnInited = 1;
#else
	HMODULE hl = ::LoadLibrary( "Kernel32.dll" );
	if( hl )
		{
		_GetCurrentProcessorNumber gcpn_ = (_GetCurrentProcessorNumber)::GetProcAddress( hl, "GetCurrentProcessorNumber" );
		gcpn = gcpn_;
		}		
	gcpnInited = 1;
#endif
	}
#endif
