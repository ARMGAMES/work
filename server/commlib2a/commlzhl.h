#ifndef commlzhl_h
#define commlzhl_h

#include "ppinclude.h"
#include "ppstring.h"
#include "pplogfile.h"
#include "commstreamencryption.h"

class CommLZHL
{
private:
	void* impl;//pimpl
	int _level;

public:
	static size_t calcMaxBuf( size_t sz );

	CommLZHL(int level_, CommStreamEncryption * encryption);
	size_t compress( BYTE* dst, const BYTE* src, size_t srcSz );
	size_t decompress( BYTE* dst, size_t dstSz, const BYTE* src, size_t srcSz );
	~CommLZHL();
};

#endif
