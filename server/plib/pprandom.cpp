#ifdef _WIN32//cannot use PWIN32 at this point :-(
#define _WIN32_WINNT 0x400
#include <winsock2.h>
#include <windows.h>
#include <wincrypt.h>
#endif

#include "ppinclude.h"
#include "pperror.h"
#include "pprandom.h"

#ifdef PWIN32
/* Intel Chipset CSP type */
#define PROV_INTEL_SEC       22 

/* Intel Chipset CSP name */
#define INTEL_DEF_PROV     "Intel Hardware Cryptographic Service Provider"

class _PIntelRNGImpl
{
private:
	HCRYPTPROV hProv;

public:
	_PIntelRNGImpl()
	{
		hProv = 0;
	}

	~_PIntelRNGImpl()
	{
		CryptReleaseContext( hProv, 0 );
	}

	void init()
	{
		BOOL Ok = CryptAcquireContext( &hProv, NULL, INTEL_DEF_PROV, PROV_INTEL_SEC, 0 );
		if( !Ok )
			throw PSystemError( "CryptAcquireContext(" INTEL_DEF_PROV ")", GetLastError() );
	}

	void rnd( BYTE* p, size_t sz )
	{
		BOOL Ok = CryptGenRandom( hProv, sz, p );
		if( !Ok )
			throw PSystemError( "CryptGenRandom()", GetLastError() );
	}
};

PIntelRNG::PIntelRNG()
{
	impl = new _PIntelRNGImpl();
}

void PIntelRNG::init()
{
	_PIntelRNGImpl* impl_ = (_PIntelRNGImpl*)impl;
	impl_->init();
}

void PIntelRNG::rnd( BYTE* p, size_t sz )
{
	_PIntelRNGImpl* impl_ = (_PIntelRNGImpl*)impl;
	impl_->rnd( p, sz );
}

PIntelRNG::~PIntelRNG()
{
	_PIntelRNGImpl* impl_ = (_PIntelRNGImpl*)impl;
	delete impl_;
}

#endif
