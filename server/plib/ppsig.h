#ifndef ppsig_h_included
#define ppsig_h_included

#if defined(P_AIX64) || defined(P_AIX32)

#define PSIGHANDLER_NO_TRACE_DUMP    -1
#define PSIGHANDLER_FRAMES_MAX        256


#include <signal.h>
#include <setjmp.h>
#include <pthread.h>
#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"

struct PSigFrame
{
	jmp_buf jumpBuf;
};

struct PSigSpecific
{
	int nextFrame;
	PSigFrame frameVec[PSIGHANDLER_FRAMES_MAX];
	PSigSpecific() { nextFrame = 0; }
};

class PSigHandler
{
	static int log;
	static pthread_key_t key;
	static pthread_once_t once;
	static void handler(int sig, int code, struct sigcontext* cntx);
	static PSigSpecific* getSpecific();
	static void onceInit();
	static void freeSpecific(void*);
public:
	static void init(int fd = PSIGHANDLER_NO_TRACE_DUMP);
	static jmp_buf* getJumpBuf()
	{
		PSigSpecific* spec = getSpecific();
		int currFrame = spec->nextFrame < PSIGHANDLER_FRAMES_MAX?
			spec->nextFrame - 1: PSIGHANDLER_FRAMES_MAX - 1;
		return 0 <= currFrame? &spec->frameVec[currFrame].jumpBuf: 0;
	}
	PSigHandler()
	{
		PSigSpecific* spec = getSpecific();
		++spec->nextFrame;
		if (spec->nextFrame == PSIGHANDLER_FRAMES_MAX)
			PLog("ERROR! PSIGHANDLER_FRAMES_MAX exceeded!");
	}
	~PSigHandler()
	{
		PSigSpecific* spec = getSpecific();
		--spec->nextFrame;
	}
};

class PSigError : public PInternalError
{
	static const char* sigNames[];
	static const char* getName(int sig);
public:
	PSigError(): PInternalError(getName(0)) { }
	PSigError(int sig): PInternalError(getName(sig)) { }
};

#define PSIGRETURN()                                                    \
	PSigHandler __frame__;                                      \
	int __sigNum__ = setjmp(*PSigHandler::getJumpBuf());        \
	if (__sigNum__)                                             \
	throw PSigError(__sigNum__);

#define PSIGHANDLER_INIT(fd)                                            \
	PSigHandler::init(fd);

#else // not AIX

#define PSIGRETURN()
#define PSIGHANDLER_INIT(fd)

#endif 

#endif
