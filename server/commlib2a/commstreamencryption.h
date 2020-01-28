#ifndef commstreamencryption_h_included
#define commstreamencryption_h_included
#include "ppinclude.h"

class CommStreamEncryption 
{
public:
	virtual BYTE getRead() = 0;
	virtual BYTE getWrite() = 0;
	virtual ~CommStreamEncryption() {}
};

#endif
