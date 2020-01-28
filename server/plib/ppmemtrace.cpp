#if ENABLE_MEMORY_TRACING == 1

#define DO_NOT_REDEFINE_NEW

#include <atomic>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#if (defined(_WIN64) || defined(_WIN32))
#	include <malloc.h>
#	include <process.h>
#endif // _WIN64 || _WIN32

void* (*system_malloc)(size_t) = &malloc;
void (*system_free)(void*) = &free;

#include "ppmemtrace.h"

using namespace std;

namespace
{

constexpr unsigned int ALLOCATION_GUARD_PREFIX = 0xDEADBEEF;
constexpr unsigned int ALLOCATION_GUARD_SUFFIX = 0x0FEEDBAD;
constexpr unsigned int ALLOCATION_GUARD_SUFFIX_SIZE = sizeof(ALLOCATION_GUARD_SUFFIX) - sizeof(const unsigned char);

enum LeakedListType : bool
{
	MemoryLeak,
	MismatchedDeallocation
};

inline const char* toString(LeakedListType type)
{
	switch (type)
	{
		case MemoryLeak: return "MemoryLeak";
		case MismatchedDeallocation: return "MismatchedDeallocation";
		default: return "Unknown";
	}
}

struct AllocationData
{
	const char* m_file;
	unsigned short m_lineNum;
	size_t m_allocSize;
};

struct AllocListNode
{
	void setAllocType(unsigned char allocType)
	{
		unsigned char* allocTypePtr = reinterpret_cast<unsigned char*>(&m_allocGuardSuffix) + ALLOCATION_GUARD_SUFFIX_SIZE;
		*allocTypePtr |= (allocType << 4);
	}

	unsigned char getAllocType() const
	{
		const unsigned char* allocTypePtr = reinterpret_cast<const unsigned char*>(&m_allocGuardSuffix) + ALLOCATION_GUARD_SUFFIX_SIZE;
		return (*allocTypePtr & 0x30) >> 4;
	}

	unsigned int m_allocGuardPrefix;
	AllocListNode* m_prev;
	AllocListNode* m_next;
	AllocationData m_allocData;
	unsigned int m_allocGuardSuffix;
};

struct AllocationList
{
	AllocationList(LeakedListType type) : m_head(nullptr), m_type(type)
	{
		unlock();
		FILE* status = openFile("leak_trace_is_enabled");
		if (status)
		{
			fclose(status);
		}
	}

	~AllocationList()
	{
		if (m_head)
		{
			FILE* out = openFile(getOutputFileName());
			if (out)
			{
				fprintf(out, "**** Begin summary of %s ****\r\n", toString(m_type));
				uint64_t totalBytesLeaked = 0;
				AllocListNode* node = m_head;
				while (node)
				{
					fprintf(out, "%s:%u (%llu bytes)\r\n", node->m_allocData.m_file, node->m_allocData.m_lineNum, (uint64_t)node->m_allocData.m_allocSize);
					totalBytesLeaked += node->m_allocData.m_allocSize;
					AllocListNode* next = node->m_next;
					unsigned char origAllocType = node->getAllocType();
					if (origAllocType == ALLOCATION_TYPE_HEAP_ALLOC)
					{
						system_free(node);
					}
					node = next;
				}
				fprintf(out, "**** End summary of %s ****\r\n", toString(m_type));
				if (m_type == MemoryLeak)
				{
					fprintf(out, "Leaked %llu byte(s)\r\n", totalBytesLeaked);
				}
				fclose(out);
			}
		}
	}

	void insert(AllocListNode* node)
	{
		lock();
		if (m_head)
		{
			node->m_next = m_head;
			m_head->m_prev = node;
			node->m_prev = nullptr;
		}
		else
		{
			node->m_prev = node->m_next = nullptr;
		}
		m_head = node;
		unlock();
	}

	void erase(AllocListNode* node)
	{
		lock();
		if (node->m_prev)
		{
			node->m_prev->m_next = node->m_next;
		}
		if (node->m_next)
		{
			node->m_next->m_prev = node->m_prev;
		}
		if (m_head == node)
		{
			m_head = node->m_next;
		}
		unlock();
	}

private:
	void lock()
	{
		while (m_lock.test_and_set(memory_order_acquire));
	}

	void unlock()
	{
		m_lock.clear(memory_order_release);
	}

	const char* getOutputFileName() const
	{
		return (m_type == MemoryLeak ? "memory_leaks" : "mismatched_deallocation");
	}

	static const char* getOutputFileDirectory()
	{
		return getenv("mem_trace_dir");
	}

	static FILE* openFile(const char* fileName)
	{
		char fullFileName[1024];
		const char* memTraceDir = getOutputFileDirectory();
		snprintf(fullFileName, sizeof(fullFileName), "%s%s.%d%s", (memTraceDir ? memTraceDir : ""), fileName, _getpid(), ".txt");
		return fopen(fullFileName, "a");
	}

	AllocListNode* m_head;
	LeakedListType m_type;
	atomic_flag m_lock;

} leaked(MemoryLeak), mismatchedDeallocation(MismatchedDeallocation);

} // unnamed namespace

void* mallocTrace(size_t size, const char* file, int lineNum, bool zeroMemory, unsigned char allocType)
{
	void* const ptr = system_malloc(sizeof(AllocListNode) + size);
	if (!ptr) return nullptr;
	AllocListNode* allocNode = reinterpret_cast<AllocListNode*>(ptr);
	allocNode->m_allocGuardPrefix = ALLOCATION_GUARD_PREFIX;
	allocNode->m_allocGuardSuffix = ALLOCATION_GUARD_SUFFIX;
	allocNode->setAllocType(allocType);
	allocNode->m_allocData.m_file = file;
	allocNode->m_allocData.m_lineNum = static_cast<unsigned short>(lineNum);
	allocNode->m_allocData.m_allocSize = size;
	leaked.insert(allocNode);
	void* ret = allocNode + 1;
	if (zeroMemory)
	{
		memset(ret, 0, size);
	}
	return ret;
}

void freeTrace(void* ptr, unsigned char deallocType)
{
	if (!ptr)
	{
		return;
	}
	AllocListNode* const allocNode = reinterpret_cast<AllocListNode*>(ptr) - 1;
	if (allocNode->m_allocGuardPrefix == ALLOCATION_GUARD_PREFIX && !memcmp((reinterpret_cast<const unsigned char*>(&allocNode->m_allocGuardSuffix)), (reinterpret_cast<const unsigned char*>(&ALLOCATION_GUARD_SUFFIX)), ALLOCATION_GUARD_SUFFIX_SIZE))
	{
		unsigned char origAllocType = allocNode->getAllocType();
		if (origAllocType != deallocType)
		{
			AllocListNode* mismatchedDeallocNode = reinterpret_cast<AllocListNode*>(system_malloc(sizeof(AllocListNode)));
			if (mismatchedDeallocNode)
			{
				mismatchedDeallocNode->m_allocGuardPrefix = ALLOCATION_GUARD_PREFIX;
				mismatchedDeallocNode->m_allocGuardSuffix = ALLOCATION_GUARD_SUFFIX;
				mismatchedDeallocNode->setAllocType(ALLOCATION_TYPE_HEAP_ALLOC);
				mismatchedDeallocNode->m_allocData.m_file = allocNode->m_allocData.m_file;
				mismatchedDeallocNode->m_allocData.m_lineNum = allocNode->m_allocData.m_lineNum;
				mismatchedDeallocNode->m_allocData.m_allocSize = allocNode->m_allocData.m_allocSize;
				mismatchedDeallocation.insert(mismatchedDeallocNode);
			}
		}
		leaked.erase(allocNode);
		system_free(allocNode);
	}
}

void* reallocTrace(void* oldPtr, size_t size, const char* file, int lineNum)
{
	if (!oldPtr)
	{
		return mallocTrace(size, file, lineNum, false, ALLOCATION_TYPE_MALLOC);
	}
	if (!size)
	{
		freeTrace(oldPtr, ALLOCATION_TYPE_MALLOC);
		return nullptr;
	}
#if (defined(_WIN64) || defined(_WIN32))
	if (size > _HEAP_MAXREQ)
	{
		errno = ENOMEM;
		return nullptr;
	}
#endif // _WIN64 || _WIN32
	void* const newPtr = mallocTrace(size, file, lineNum, false, ALLOCATION_TYPE_MALLOC);
	if (!newPtr)
	{
		errno = ENOMEM;
		return nullptr;
	}
	AllocListNode* const oldAllocNode = reinterpret_cast<AllocListNode*>(oldPtr) - 1;
	memcpy(newPtr, oldPtr, min(oldAllocNode->m_allocData.m_allocSize, size));
	freeTrace(oldPtr, ALLOCATION_TYPE_MALLOC);
	return newPtr;
}

void* operator new(size_t size, const char* file, int line)
{
	return mallocTrace(size, file, line, false, ALLOCATION_TYPE_NEW);
}

void* operator new[](size_t size, const char* file, int line)
{
	return mallocTrace(size, file, line, false, ALLOCATION_TYPE_NEW_ARRAY);
}

void operator delete(void* ptr) noexcept
{
	freeTrace(ptr, ALLOCATION_TYPE_NEW);
}

void operator delete(void* ptr, const char*, int) noexcept
{
	freeTrace(ptr, ALLOCATION_TYPE_NEW);
}

void operator delete[](void* ptr) noexcept
{
	freeTrace(ptr, ALLOCATION_TYPE_NEW_ARRAY);
}

void operator delete[](void* ptr, const char*, int) noexcept
{
	freeTrace(ptr, ALLOCATION_TYPE_NEW_ARRAY);
}

#endif // ENABLE_MEMORY_TRACING
