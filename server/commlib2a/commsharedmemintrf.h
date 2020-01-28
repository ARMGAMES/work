#ifndef commsharedmemintrf_h_included
#define commsharedmemintrf_h_included

class CommSharedMemoryIntrf
{
public:
	virtual void forceDisconnectSharedMemory() = 0;
	virtual const PString & getResourceName() = 0;
	virtual ~CommSharedMemoryIntrf() {}
};

#endif
