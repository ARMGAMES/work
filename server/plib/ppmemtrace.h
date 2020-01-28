#ifndef MEM_TRACE_H
#define MEM_TRACE_H

#if ENABLE_MEMORY_TRACING == 1

void* mallocTrace(size_t, const char*, int, bool, unsigned char);
void freeTrace(void*, unsigned char);
void* reallocTrace(void*, size_t, const char*, int);

#if defined(malloc)
#	undef malloc
#endif

#if defined(calloc)
#	undef calloc
#endif

#if defined(realloc)
#	undef realloc
#endif

#if defined(free)
#	undef free
#endif

#define ALLOCATION_TYPE_MALLOC 1
#define ALLOCATION_TYPE_NEW 2
#define ALLOCATION_TYPE_NEW_ARRAY 3
#define ALLOCATION_TYPE_HEAP_ALLOC 4

#define malloc(s) mallocTrace((s), __FILE__, __LINE__, false, ALLOCATION_TYPE_MALLOC)
#define calloc(s) mallocTrace((s), __FILE__, __LINE__, true, ALLOCATION_TYPE_MALLOC)
#define free(p) freeTrace((p), ALLOCATION_TYPE_MALLOC)
#define realloc(p, s) reallocTrace((p), (s), __FILE__, __LINE__)

void* operator new (size_t, const char*, int);
void* operator new[](size_t, const char*, int);
void operator delete(void* ptr, const char* file, int line) noexcept;
void operator delete[](void*, const char*, int) noexcept;

#include "ppmemtraceenable.h"

#endif // ENABLE_MEMORY_TRACING

#endif // MEM_TRACE_H
