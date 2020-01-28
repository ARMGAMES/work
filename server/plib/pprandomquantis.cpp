#include "ppincludebase.h"
#include "pperror.h"
#include "pprandom.h"

#ifdef PWIN32

#ifdef PLIB_QUANTIS2
#include "quantis2/Quantis_Compat.h"
#else
#include "quantis/quantis.h"
#endif

class _PQuantisRNGImpl
{
public:
	_PQuantisRNGImpl()
	{
	}

	~_PQuantisRNGImpl()
	{
	}

	void init()
	{
		int Ok = quantisInit();
		if( !Ok )
			throw PSystemError( "Quantis RNG Hardware Initialization Error.", GetLastError() );
	}

	void rnd( BYTE* p, size_t sz )
	{
		if( sz != (size_t) quantisRead(0, p, sz) )
			throw PSystemError( "Quantis RNG Read Error", GetLastError() );
	}
};

//////////////////////////////////////////////////////

PQuantisRNG::PQuantisRNG()
{
	impl = new _PQuantisRNGImpl();
}

void PQuantisRNG::init()
{
	_PQuantisRNGImpl* impl_ = (_PQuantisRNGImpl*)impl;
	impl_->init();
}

void PQuantisRNG::rnd( BYTE* p, size_t sz )
{
	_PQuantisRNGImpl* impl_ = (_PQuantisRNGImpl*)impl;
	impl_->rnd( p, sz );
}

PQuantisRNG::~PQuantisRNG()
{
	_PQuantisRNGImpl* impl_ = (_PQuantisRNGImpl*)impl;
	delete impl_;
}

#endif
