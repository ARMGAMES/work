#if defined(P_AIX64) || defined(P_AIX32)

#include <stdlib.h>
#include "ppsig.h"

// PLATFORM-DEPENDENT: stack trace from within signal handler
// extern "C" void mt__trce(int fd, int sig, struct sigcontext *cntx, int node);

int PSigHandler::log = PSIGHANDLER_NO_TRACE_DUMP;
pthread_key_t PSigHandler::key;
pthread_once_t PSigHandler::once = PTHREAD_ONCE_INIT;

void PSigHandler::handler(int sig, int code, struct sigcontext* cntx)
{
	// PLATFORM-DEPENDENT:  need -ltools_ptr library 
	//    if (log != PSIGHANDLER_NO_TRACE_DUMP)
	//        mt__trce(log, sig, cntx, 0);

	jmp_buf* jmp = getJumpBuf();
	// SNAFU: got fatal signal out of registration scope - die now
	// Any attempt to ignore it will most probably lead to nasty freeze
	PASSERT(jmp);
	longjmp(*jmp, sig);
}

void PSigHandler::onceInit()
{
	if (pthread_key_create(&key, freeSpecific))
		throw PSystemError("pthread_key_create", errno);
}

void PSigHandler::init(int fd)
{
	pthread_once(&once, onceInit);
	log = fd;

	struct sigaction action;
	action.sa_handler = reinterpret_cast<void (*)(int)>(handler);
	action.sa_flags = 0;
	sigemptyset(&action.sa_mask);

	sigaction(SIGSEGV, &action, 0);
	sigaction(SIGBUS, &action, 0);
	sigaction(SIGFPE, &action, 0);
	sigaction(SIGSYS, &action, 0);
	sigaction(SIGILL, &action, 0);

	/*...*/
}

void PSigHandler::freeSpecific(void* obj)
{
	delete static_cast<PSigSpecific*>(obj);
}

PSigSpecific* PSigHandler::getSpecific()
{
	PSigSpecific* spec = static_cast<PSigSpecific*>(pthread_getspecific(key));
	if (!spec)
	{
		spec = new PSigSpecific();
		if (pthread_setspecific(key, spec))
			throw PSystemError("pthread_setspecific", errno);
	}
	return spec;
}

const char* PSigError::sigNames[] = 
{
	"UNKNOWN",
	"SIGHUP",
	"SIGINT",
	"SIGQUIT",
	"SIGILL",
	"SIGTRAP",
	"SIGABRT",
	"SIGEMT",
	"SIGFPE",
	"SIGKILL",
	"SIGBUS",
	"SIGSEGV",
	"SIGSYS",
	"SIGPIPE",
	"SIGALRM",
	"SIGTERM",
	"SIGURG",
	"SIGSTOP",
	"SIGTSTP",
	"SIGCONT",
	"SIGCHLD",
	"SIGTTIN",
	"SIGTTOU",
	"SIGIO",
	"SIGXCPU",
	"SIGXFSZ",
	"SIGMSG",
	"SIGWINCH",
	"SIGPWR",
	"SIGUSR1",
	"SIGUSR2",
	"SIGPROF",
	"SIGDANGER",
	"SIGVTALRM"
};

const char* PSigError::getName(int signum)
{
	signum = signum > 0 && signum < sizeof(sigNames)/sizeof(char*)? signum: 0;
	return sigNames[signum];
}

#endif
