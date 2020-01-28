#ifdef P_AIX64

#include <stdlib.h>
#include "ppsighandler.h"

// PLATFORM-DEPENDENT: must be long enough to hold void*
typedef unsigned long PTRINT;

// PLATFORM-DEPENDENT: stack trace from within signal handler
extern "C" void mt__trce(int fd, int sig, struct sigcontext *cntx, int node);

// PLATFORM-INDEPENDENT: high bit indicator is valid for *ANY* UNIX
static const PTRINT HIGHBIT = 1UL << (sizeof(PTRINT) * 8 - 1);

static inline PSigUnwinder* highbitOn(void* ptr) 
{ 
    return reinterpret_cast<PSigUnwinder*>(reinterpret_cast<PTRINT>(ptr) | HIGHBIT); 
}

static inline PSigUnwinder* highbitOff(void* ptr) 
{ 
    return reinterpret_cast<PSigUnwinder*>(reinterpret_cast<PTRINT>(ptr) & ~HIGHBIT); 
}

static inline bool highbitIsSet(void* ptr) 
{ 
    return reinterpret_cast<PTRINT>(ptr) & HIGHBIT; 
}

int PSigHandler::log = PSIGHANDLER_NO_TRACE_DUMP;
pthread_key_t PSigHandler::key;
pthread_once_t PSigHandler::once = PTHREAD_ONCE_INIT;

#if PSIGHANDLER_USE_UNWINDER
void PSigUnwinder::constructUnwinder()
{
    PSigStack* stack = PSigHandler::getStack();
    if (!stack)
    {
        stackAlloc = false;
        return;
    }

    if (stack->size() && highbitIsSet(stack->back()))
    {
        // heap frame invariant (highbitIsSet(beg) && highbitIsSet(end))
        PASSERT(stack->size() > 1 && highbitIsSet(stack->at(stack->size() - 2)));

        if (highbitOff(stack->at(stack->size() - 2)) <= this && 
            this < highbitOff(stack->back()))
        {
            stackAlloc = false;
            return;
        }
        else
        {
            stack->pop_back();
            stack->pop_back();
        }
    }

    stackAlloc = true;
    stack->push_back(this);
}

void PSigUnwinder::registerUnwinder()
{
    if (!stackAlloc)
        return;

    PSigStack* stack = PSigHandler::getStack();
    // invariant (!stackAlloc || (stackAlloc && stack))
    PASSERT(stack); 

    while (stack->size())
    {
        if (highbitIsSet(stack->back()))
        {
            // heap frame invariant (highbitIsSet(beg) && highbitIsSet(end))
            PASSERT(stack->size() > 1 && highbitIsSet(stack->at(stack->size() - 2)));

            stack->pop_back();
            stack->pop_back();
            continue;
        }

        if (stack->back() == this)
            break;

        stack->back()->stackAlloc = false;
        stack->pop_back();
    }
}

PSigUnwinder::~PSigUnwinder()
{
    if (!stackAlloc)
        return;

    PSigStack* stack = PSigHandler::getStack();
    // invariant (!stackAlloc || (stackAlloc && stack && stack->size()))
    PASSERT(stack && stack->size()); 

    if (highbitIsSet(stack->back()))
    {
        // heap frame invariant (highbitIsSet(beg) && highbitIsSet(end))
        PASSERT(stack->size() > 1 && highbitIsSet(stack->at(stack->size() - 2)));
        stack->pop_back();
        stack->pop_back();
    }

    // Principal invariant
    PASSERT(stack->back() == this);

    stack->pop_back();
}

void PSigUnwinder::constructMemory(size_t size, void* beg)
{
    PSigStack* stack = PSigHandler::getStack();

    if (!stack)
        return;

    if (stack->size() && highbitIsSet(stack->back()))
    {
        // heap frame invariant (highbitIsSet(beg) && highbitIsSet(end))
        PASSERT(stack->size() > 1 && highbitIsSet(stack->at(stack->size() - 2)));
        stack->pop_back();
        stack->pop_back();
    }

    stack->push_back(highbitOn(beg));
    stack->push_back(highbitOn(static_cast<char*>(beg) + size));
}

#endif

void PSigHandler::handler(int sig, int code, struct sigcontext* cntx)
{
    // PLATFORM-DEPENDENT:  need -ltools_ptr library 
    if (log != PSIGHANDLER_NO_TRACE_DUMP)
        mt__trce(log, sig, cntx, 0);

#if PSIGHANDLER_USE_UNWINDER
    PSigStack* stack = getStack();
    // SNAFU: got fatal signal out of registration scope - die now
    // Any attempt to ignore it will most probably lead to nasty freeze
    PASSERT(stack);

    while (stack->size())
    {
        if (highbitIsSet(stack->back()))
        {
            // heap frame invariant (highbitIsSet(beg) && highbitIsSet(end))
            PASSERT(stack->size() > 1 && highbitIsSet(stack->at(stack->size() - 2)));
            stack->pop_back();
            stack->pop_back();
            continue;
        }
        stack->back()->~PSigUnwinder();
    }
#endif

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
