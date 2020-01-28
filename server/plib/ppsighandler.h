#ifndef ppsighandler_h_included
#define ppsighandler_h_included

//
// set PSIGHANDLER_USE_UNWINDER to desired use mode (for whole project):
//
// 1 - PSIGHANDLER mode - to wrap top-level function calls (with unnwinder)
//     e.g. PSIGHANDLER(server.obj->_safeProcessMessage( m ));
// 0 - PSIGRETURN mode  - to mark return points (without unwinder)
//     e.g. PLock lock( cs ); PSIGRETURN();
//
#define PSIGHANDLER_USE_UNWINDER      0

#define PSIGHANDLER_NO_TRACE_DUMP    -1
#define PSIGHANDLER_FRAMES_MAX        256

#ifdef P_AIX64
#include <signal.h>
#include <setjmp.h>
#include <pthread.h>
#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"

#if PSIGHANDLER_USE_UNWINDER
class PSigUnwinder
{
    bool stackAlloc;
    static void constructMemory(size_t size, void* obj);
    void constructUnwinder();
public:
    PSigUnwinder()
    {
        constructUnwinder();
    }
    PSigUnwinder(const PSigUnwinder& other)
    {
        constructUnwinder();
    }
    PSigUnwinder& operator=(const PSigUnwinder& other)
    {
        return *this;
    }

    virtual ~PSigUnwinder();

    /* must be called in the derived object constructor!!! */
    void registerUnwinder();

    void* operator new(size_t sz)
    {
        void* beg = ::operator new(sz);
        constructMemory(sz, beg);
        return beg;
    }
    void* operator new(size_t sz, void* p)
    {
        void* beg = ::operator new(sz, p);
        constructMemory(sz, beg);
        return beg;
    }
    void* operator new[](size_t sz)
    {
        void* beg = ::operator new[](sz);
        constructMemory(sz, beg);
        return beg;
    }
    void* operator new[](size_t sz, void* p)
    {
        void* beg = ::operator new[](sz, p);
        constructMemory(sz, beg);
        return beg;
    }
    void* operator new(size_t sz, const std::nothrow_t& t)
    {
        void* beg = ::operator new(sz, t);
        constructMemory(sz, beg);
        return beg;
    }
    void* operator new[](size_t sz, const std::nothrow_t& t)
    {
        void* beg = ::operator new[](sz, t);
        constructMemory(sz, beg);
        return beg;
    }
};

template <class T> class PSigStackTmpl
{
private:
    T* stack;
    size_t sz;
    size_t nx;
    void allocate()
    {
        if (sz == nx)
            stack = static_cast<T*>(realloc(stack, (sz += 512) * sizeof(T)));
    }
public:
    PSigStackTmpl() : stack(0), sz(0), nx(0) { allocate(); }
    ~PSigStackTmpl() { free(stack); }
    void push_back(const T& t) { allocate(); stack[nx++] = t; }
    void pop_back() { --nx; }
    T& front() { return stack[0]; }
    T& back() { return stack[nx - 1]; }
    T& at(size_t i) { return stack[i]; }
    void clear() { nx = 0; }
    size_t size() { return nx; }
};

typedef PSigStackTmpl<PSigUnwinder*> PSigStack;

//
// EXPERIMENTAL!!!
// (currently works with vector-like (single class) containers without
// restrictions, and works with map-like (class pair) containers ONLY if
// neither of the classes is derived from PSigUnwinder 
//
template<class T> class PSigUnwindTmpl: public PSigUnwinder
{
private:
    T p;

public:
    PSigUnwindTmpl() : p() { registerUnwinder(); }
    operator T&() { return p; }
    T& instance() { return p; }
    operator T*() { return &p; }
    operator const T&() const { return p; }
    const T& instance() const { return p; }
    operator const T*() const { return &p; }

private: // yet prohibited
    PSigUnwindTmpl(const PSigUnwindTmpl&);
    PSigUnwindTmpl& operator=(const PSigUnwindTmpl&);
};
// EXPERIMENTAL END

#else // no unwinder

class PSigUnwinder
{
public:
    void registerUnwinder() { }
};

#endif


struct PSigFrame
{
    jmp_buf jumpBuf;
#if PSIGHANDLER_USE_UNWINDER
    PSigStack callStack;
#endif
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
#if PSIGHANDLER_USE_UNWINDER
    static PSigStack* getStack()
    {
        PSigSpecific* spec = getSpecific();
        int currFrame = spec->nextFrame < PSIGHANDLER_FRAMES_MAX?
            spec->nextFrame - 1: PSIGHANDLER_FRAMES_MAX - 1;
        return 0 <= currFrame? &spec->frameVec[currFrame].callStack: 0;
    }
#endif
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

#if PSIGHANDLER_USE_UNWINDER

#define PSIGHANDLER(fcall)                                              \
            try                                                         \
            {                                                           \
                PSigHandler __frame__;                                  \
                int __sigNum__ = setjmp(*PSigHandler::getJumpBuf());    \
                if (__sigNum__)                                         \
                    throw PSigError(__sigNum__);                        \
                fcall;                                                  \
            }                                                           \
            catch(PError& er)                                           \
            {                                                           \
                PLog("Caught PError in %s: %s\n", #fcall, er.why());    \
            }

#define PSIGRETURN()                                                    \

#else // no unwinder

#define PSIGHANDLER(fcall) fcall;

#define PSIGRETURN()                                                    \
            PSigHandler __frame__;                                      \
            int __sigNum__ = setjmp(*PSigHandler::getJumpBuf());        \
            if (__sigNum__)                                             \
                throw PSigError(__sigNum__);
#endif

#define PSIGHANDLER_INIT(fd)                                            \
            PSigHandler::init(fd);

#else // not UNIX

#define PSIGRETURN()
#define PSIGHANDLER(fcall) fcall;
#define PSIGHANDLER_INIT(fd)
class PSigUnwinder 
{
public:
    void registerUnwinder() { }
};
#endif

#endif
