#ifndef pprandom_h_included
#define pprandom_h_included

#include "ppincludebase.h"

class PBaseRNG
{
public:
	virtual ~PBaseRNG() {}

	virtual void init() = 0;
	virtual void rnd( BYTE* p, size_t sz ) = 0;
};

#ifdef PWIN32

class PIntelRNG : public PBaseRNG
{
private:
	void* impl;//pimpl

public:
	PIntelRNG();
	~PIntelRNG();

	void init() override;
	void rnd( BYTE* p, size_t sz ) override;
};

class PQuantisRNG : public PBaseRNG
{
private:
	void* impl;//pimpl

public:
	PQuantisRNG();
	~PQuantisRNG();

	void init() override;
	void rnd( BYTE* p, size_t sz ) override;
};

#endif

#endif

