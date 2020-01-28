#ifndef ppalloc_h_included
#define ppalloc_h_included

#include "ppthread.h"
//#define NOALLOCGUESS

class PPageMallocAllocator//mostly testing one
//thread safe
	{
	public:
	struct Stats
		{
		int nAllocs;
		int nAllocPages;
		int allocTime;
		int nDeallocs;
		int nDeallocPages;
		int deallocTime;
		};

	private:
	Stats stats;

	public:
	const Stats& getStats() const { return stats; }

	public:
	static size_t pageSize() { return 4096; }
	PPageMallocAllocator() { memset( &stats, 0, sizeof(stats) ); }
	BYTE* allocPages( int nPages )
		{
		++stats.nAllocs;
		stats.nAllocPages += nPages;

		UINT32 t0 = SYSTEMTICKS();
		BYTE* ret = (BYTE*)::malloc( nPages * pageSize() );
		UINT32 t1 = SYSTEMTICKS();
		stats.allocTime += t1 - t0;
		return ret;
		}
	void deallocPages( BYTE* p, int nPages )
		{
		++stats.nDeallocs;
		stats.nDeallocPages += nPages;

		UINT32 t0 = SYSTEMTICKS();
		::free( p );
		UINT32 t1 = SYSTEMTICKS();
		stats.deallocTime += t1 - t0;
		}
	};

class PMallocAllocatorStub//mostly testing one
//stub - no constructor overhead and no stats
	{
	public:
	BYTE* allocate( size_t sz )
		{
		return (BYTE*)::malloc( sz );
		}
	BYTE* allocateEx( size_t sz, size_t* retSz )
		{
		BYTE* ret = (BYTE*)::malloc( sz );
		if( retSz )
			*retSz = sz;
		return ret;
		}
	BYTE* reallocate( void* p, size_t sz )
		{
		return (BYTE*)::realloc( p, sz );
		}
	BYTE* reallocateEx( void* p, size_t sz, size_t* retSz )
		{
		BYTE* ret = (BYTE*)::realloc( p, sz );
		if( retSz )
			*retSz = sz;
		return ret;
		}
	void deallocate( void* p )
		{
		::free( p );
		}
	};

class PMallocAllocator//mostly testing one
//thread safe
	{
	public:
	struct Stats
		{
		int nAllocs;
		int allocTime;
		int nReallocs;
		int reallocTime;
		int nDeallocs;
		int deallocTime;
		};

	private:
	Stats stats;

	public:
	PMallocAllocator()
		{
		memset( &stats, 0, sizeof(stats) );
		}

	public:
	BYTE* allocate( size_t sz ) { return allocateEx( sz, 0 ); }
	BYTE* allocateEx( size_t sz, size_t* retSz )
		{
		++stats.nAllocs;

		UINT32 t0 = SYSTEMTICKS();
		BYTE* ret = (BYTE*)::malloc( sz );
		UINT32 t1 = SYSTEMTICKS();
		stats.allocTime += t1 - t0;
		if( retSz )
			*retSz = sz;
		return ret;
		}
	BYTE* reallocate( void* p, size_t sz ) { return reallocateEx( p, sz, 0 ); }
	BYTE* reallocateEx( void* p, size_t sz, size_t* retSz )
		{
		++stats.nReallocs;

		UINT32 t0 = SYSTEMTICKS();
		BYTE* ret = (BYTE*)::realloc( p, sz );
		UINT32 t1 = SYSTEMTICKS();
		stats.reallocTime += t1 - t0;
		if( retSz )
			*retSz = sz;
		return ret;
		}
	void deallocate( void* p )
		{
		++stats.nDeallocs;

		UINT32 t0 = SYSTEMTICKS();
		::free( p );
		UINT32 t1 = SYSTEMTICKS();
		stats.deallocTime += t1 - t0;
		}
	};

#ifdef PWIN32
class PPagePageAllocator
//thread safe
	{
	public:
	struct Stats
		{
		int nAllocs;
		int nAllocPages;
		int allocTime;
		int nDeallocs;
		int nDeallocPages;
		int deallocTime;
		};

	private:
	Stats stats;

	public:
	const Stats& getStats() const { return stats; }

	public:
	static size_t pageSize() { return 4096; }
	PPagePageAllocator() { memset( &stats, 0, sizeof(stats) ); }
	~PPagePageAllocator()
		{
		}
	BYTE* allocPages( int nPages )
		{
		++stats.nAllocs;
		stats.nAllocPages += nPages;

		UINT32 t0 = SYSTEMTICKS();
		BYTE* ret = (BYTE*)::VirtualAlloc( NULL, nPages * pageSize(), MEM_COMMIT, PAGE_READWRITE );
		UINT32 t1 = SYSTEMTICKS();
		stats.allocTime += t1 - t0;
		return ret;
		}
	void deallocPages( BYTE* p, int nPages )
		{
		++stats.nDeallocs;
		stats.nDeallocPages += nPages;

		UINT32 t0 = SYSTEMTICKS();
		::VirtualFree( p, 0 /*nPages * pageSize(),*/, MEM_RELEASE );
		UINT32 t1 = SYSTEMTICKS();
		stats.deallocTime += t1 - t0;
		}
	};
#endif

#ifdef PWIN32
typedef DWORD (WINAPI *_GetCurrentProcessorNumber)(
  void 
);

class _PAllocStartIndexGetter
	{
	private:
	static int gcpnInited;
	static _GetCurrentProcessorNumber gcpn;

	private:
	bool _getCPUSupported;
	int nCPUs;


	private:
	static void _initGcpn();

	public:
	_PAllocStartIndexGetter()
		{
		if( gcpnInited == 0 )
			_initGcpn();
		_getCPUSupported = gcpn != 0;
		if( _getCPUSupported )
			{
			SYSTEM_INFO si;
			::GetSystemInfo( &si );
			nCPUs = si.dwNumberOfProcessors;
			}
		}
	bool getCurrentCPUSupported() const { return _getCPUSupported; }

	UINT32 getStartIndexSeed( int n ) 
		{
		if( _getCPUSupported )
			{
			PASSERT5( gcpn );
			int cpu = (*gcpn)();
			return cpu * n / nCPUs;//* 2736285
			}
		else
			return THREADID();
		}
	};


#else
/*class _PAllocStartIndexGetter
	{
	public:
	UINT32 getStartIndexSeed( int n ) 
		{
		return THREADID();
		}
	}*/
#endif

template< class PageAlloc, class AllocParms >
class PPageCachingAllocator
	{
	public:
	enum { NCachesPerBunch = 1 << AllocParms::NCachesPerBunch_Log2 };

	public:
	struct CacheStats
		{
		int nAsyncAllocs;
		int nSyncAllocs;
		int nAsyncDeallocs;
		int nSyncDeallocs;

		void add( const CacheStats& other )
			{
			nAsyncAllocs += other.nAsyncAllocs;
			nSyncAllocs += other.nSyncAllocs;
			nAsyncDeallocs += other.nAsyncDeallocs;
			nSyncDeallocs += other.nSyncDeallocs;
			}
		};
	struct BunchStats
		{
		int nNoSpaceAllocs;
		int nNoSpaceDeallocs;
		};
	struct Stats
		{
		int nHugeAllocs;
		int nHugeAllocPages;
		int nHugeDeallocs;
		int nHugeDeallocPages;
		};

	private:
	struct _Page
		{
		_Page* next;
		};
	struct _Cache
		{
		int guessLocked;

		PCriticalSection cs;
		_Page* cache;//'cache' also used as 'guessHasPages' 
		int nToCache;//nCached < nToCache also used as 'guessHasSpace'
		int nCached;

		CacheStats stats;
		};
	struct _CacheBunch
		{
		_Cache caches[ NCachesPerBunch ];

		BunchStats stats;
		};

	private:
	PageAlloc* allocator;
	_PAllocStartIndexGetter idxGetter;
	_CacheBunch cacheBunches[ AllocParms::MaxPagesCached ];
	Stats stats;

	public:
	PPageCachingAllocator();
	void _construct( PageAlloc& allocator_ );
	void _setNChunksToCache( int chunkSize, int nToCache );
	~PPageCachingAllocator();

	void getStats( Stats& stats_ ) const
		{
		stats_ = stats;
		}
	void getBunchStats( BunchStats& bStats, CacheStats& cStats, int chunkSize ) const;

	public:
	static size_t pageSize() { return PageAlloc::pageSize(); }
	BYTE* allocPages( int nPages );
	void deallocPages( BYTE* p, int nPages );
	};

template< class PageAlloc, class AllocParms >
PPageCachingAllocator< PageAlloc, AllocParms >::PPageCachingAllocator()
	{
	}

template< class PageAlloc, class AllocParms >
void PPageCachingAllocator< PageAlloc, AllocParms >::_construct( PageAlloc& allocator_ )
	{
	allocator = &allocator_;
	for( int i=0; i < AllocParms::MaxPagesCached ; ++i )
		{
		_CacheBunch& bunch = cacheBunches[ i ];
		memset( &bunch.stats, 0, sizeof(bunch.stats) );
		for( int j=0; j < NCachesPerBunch ; ++j )
			{
			_Cache& cache = bunch.caches[ j ];
			cache.guessLocked = 0;
			cache.nToCache = 0;
			cache.cache = 0;
			cache.nCached = 0;
			memset( &cache.stats, 0, sizeof(cache.stats) );
			}
		}
	memset( &stats, 0, sizeof(stats) );
	}

template< class PageAlloc, class AllocParms >
void PPageCachingAllocator< PageAlloc, AllocParms >::_setNChunksToCache( int chunkSize, int nToCache )
	{
	PASSERT4( chunkSize <= AllocParms::MaxPagesCached );
	_CacheBunch& bunch = cacheBunches[ chunkSize - 1 ];
	for( int j=0; j < NCachesPerBunch ; ++j )
		{
		_Cache& cache = bunch.caches[ j ];
		PLock lock( cache.cs );
		cache.nToCache = nToCache;
		}
	}

template< class PageAlloc, class AllocParms >
PPageCachingAllocator< PageAlloc, AllocParms >::~PPageCachingAllocator()
	{
	for( int i=0; i < AllocParms::MaxPagesCached ; ++i )
		{
		_CacheBunch& bunch = cacheBunches[ i ];
		for( int j=0; j < NCachesPerBunch ; ++j )
			{
			_Cache& cache = bunch.caches[ j ];
			PLock lock( cache.cs );
			for( _Page* pp = cache.cache ; pp ; )
				{
				_Page* del = pp;
				pp = pp->next;
				allocator->deallocPages( (BYTE*)del, i );
				}
			}
		}
	}

template< class PageAlloc, class AllocParms >
void PPageCachingAllocator< PageAlloc, AllocParms >::getBunchStats( BunchStats& bStats, CacheStats& cStats, int chunkSize ) const
	{
	PASSERT4( chunkSize >= 0 && chunkSize <= AllocParms::MaxPagesCached );
	const _CacheBunch& bunch = cacheBunches[ chunkSize - 1 ];
	bStats = bunch.stats;
	memset( &cStats, 0, sizeof(CacheStats) );
	for( int j=0; j < NCachesPerBunch ; ++j )
		{
		const _Cache& cache = bunch.caches[ j ];
		//no lock 
		cStats.add( cache.stats );
		}
	}

template< class PageAlloc, class AllocParms >
BYTE* PPageCachingAllocator< PageAlloc, AllocParms >::allocPages( int nPages )
	{
	if( nPages >= AllocParms::MaxPagesCached )
		{
		++stats.nHugeAllocs;
		stats.nHugeAllocPages += nPages;
		return allocator->allocPages( nPages );
		}

	_CacheBunch& bunch = cacheBunches[ nPages - 1 ];

	UINT32 seed = idxGetter.getStartIndexSeed( NCachesPerBunch );
	//try to find unlocked cache
	int i, firstHasPages = -1;
	for( i=0; i < NCachesPerBunch ; ++i )
		{
		int idx = ( i + seed ) & ( NCachesPerBunch - 1 );//NCachesPerBunch is power of 2
		_Cache& cache = bunch.caches[ idx ];

		bool guessLocked = cache.guessLocked != 0;
		bool guessHasPages = cache.cache != 0;
		if( !guessLocked && guessHasPages )
			{
			bool locked = cache.cs._tryLock();
			if( locked )
				{
				cache.guessLocked = 1;

				if( cache.cache )
					{
					BYTE* ret = (BYTE*)cache.cache;
					cache.cache = cache.cache->next;
					--cache.nCached;
					++cache.stats.nAsyncAllocs;

					cache.guessLocked = 0;
					cache.cs._unlock();
					return ret;
					}

				cache.guessLocked = 0;
				cache.cs._unlock();
				}
			}

		if( guessHasPages && firstHasPages < 0 )
			firstHasPages = idx;
		}

	//try to alloc synchronously from cache - still better to lock than [supposedly] to go to kernel
	if( firstHasPages >= 0 )
		{
		_Cache& cache = bunch.caches[ firstHasPages ];
		PLock lock( cache.cs );
		cache.guessLocked = 1;
		if( cache.cache )
			{
			BYTE* ret = (BYTE*)cache.cache;
			cache.cache = cache.cache->next;
			--cache.nCached;
			++cache.stats.nSyncAllocs;
			cache.guessLocked = 0;
			return ret;
			}
		cache.guessLocked = 0;
		}

	//no space - going to underlying level
	++bunch.stats.nNoSpaceAllocs;
	return allocator->allocPages( nPages );
	}

template< class PageAlloc, class AllocParms >
void PPageCachingAllocator< PageAlloc, AllocParms >::deallocPages( BYTE* p, int nPages )
	{
	if( nPages >= AllocParms::MaxPagesCached )
		{
		++stats.nHugeDeallocs;
		stats.nHugeDeallocPages += nPages;
		allocator->deallocPages( p, nPages );
		return;
		}

	_CacheBunch& bunch = cacheBunches[ nPages - 1 ];

	UINT32 seed = idxGetter.getStartIndexSeed( NCachesPerBunch );
	//try to find unlocked cache
	int i, firstHasSpace = -1;
	for( i=0; i < NCachesPerBunch ; ++i )
		{
		int idx = ( i + seed ) & ( NCachesPerBunch - 1 );//NCachesPerBunch is power of 2
		_Cache& cache = bunch.caches[ idx ];

		bool guessLocked = cache.guessLocked != 0;
		bool guessHasSpace = cache.nCached < cache.nToCache;
		if( !guessLocked && guessHasSpace )
			{
			bool locked = cache.cs._tryLock();
			if( locked )
				{
				cache.guessLocked = 1;

				if( cache.nCached < cache.nToCache )
					{
					++cache.stats.nAsyncDeallocs;
					_Page* pp = (_Page*)p;
					pp->next = cache.cache;
					cache.cache = pp;
					++cache.nCached;

					cache.guessLocked = 0;
					cache.cs._unlock();
					return;
					}

				cache.guessLocked = 0;
				cache.cs._unlock();
				}
			}
		
		if( guessHasSpace && firstHasSpace < 0 )
			firstHasSpace = idx;
		}

	//go synch
	if( firstHasSpace >= 0 )
		{
		int idx = seed & ( NCachesPerBunch - 1 );//NCachesPerBunch is power of 2

		_Cache& cache = bunch.caches[ idx ];
		PLock lock( cache.cs );
		cache.guessLocked = 1;
		if( cache.nCached < cache.nToCache )
			{
			_Page* pp = (_Page*)p;
			pp->next = cache.cache;
			cache.cache = pp;
			++cache.nCached;

			++cache.stats.nSyncDeallocs;
			cache.guessLocked = 0;
			return;
			}

		cache.guessLocked = 0;
		}

	//no space at all
	++bunch.stats.nNoSpaceDeallocs;
	allocator->deallocPages( p, nPages );
	}

#if 0//deprecated
template< class PageAlloc >
class PPageAllocatorFixedSizeCache
//thread safe
	{
	private:
	struct _Page
		{
		_Page* next;
		};

	private:
	PageAlloc* allocator;
	int nPages;
	int nChunksToCache;

	PCriticalSection cs;
	_Page* cache;
	int nCached;

	public:
	static size_t pageSize() { return PageAlloc::pageSize(); }
	PPageAllocatorFixedSizeCache() {}
	~PPageAllocatorFixedSizeCache()
		{
		for( _Page* p = cache ; p ; )
			{
			_Page* nextP = p->next;
			allocator->deallocPages( p, nPages );
			p = nextP;
			}
		}
	void _construct( PageAlloc& allocator_, int nPages_, int nChunksToCache_ )
		{
		allocator = &allocator_;
		nPages = nPages_;
		nChunksToCache = nChunksToCache_;

		cache = 0;
		nCached = 0;
		}

	BYTE* allocPages( int nPages_ )
		{
		PASSERT5( nPages_ == nPages );
		PLock lock( cs );
		if( cache )
			{
			BYTE* ret = (BYTE*)cache;
			cache = cache->next;
			--nCached;
			return ret;
			}
		lock.unlock();
		return allocator->allocPages( nPages );
		}
	void deallocPages( BYTE* p, int nPages_ )
		{
		PASSERT5( nPages_ == nPages );
		PLock lock( cs );
		if( nCached >= nChunksToCache )
			{
			lock.unlock();
			allocator->deallocPages( p, nPages );
			return;
			}

		_Page* pp = (_Page*)p;
		pp->next = cache;
		cache = pp;
		++nCached;
		}
	};
#endif

//==============================================================================

class PDefaultAllocTraits
	{
	public:
	enum { isTemporaryOnly = 0 };
	};

class PNoAllocPageExtraData
	{//empty
	};

inline void* _PAllocGetPointerData( void* p )
	{
	return *( ((void**)p) - 1 );
	}

template< class PageAlloc, class AllocTraits, class PageExtraData >
class PFixedSizeAllocator
//!!! thread-unaware
//Compatible with _PAllocGetPointerData (see comments re. _PAllocGetPointerData below)
	{
	private:
	struct _Page;
	struct _Node
		{
		_Page* page;//!!!IMPORTANT: MUST be immediately before actual data; to be compatible with _PAllocGetPointerData
		};
	struct _FreeNode : public _Node
		{
		_FreeNode* nextFree;
		_FreeNode* prevFree;
		};
	struct _Page
		{
		PageExtraData xtra;//!!!IMPORTANT: MUST be at the very beginning of the page; to be compatible with _PAllocGetPointerData
		_FreeNode* firstFree;
		//_FreeNode* lastFree;
		size_t nUsed;
		_Page* nextWithFree;
		_Page* prevWithFree;
		_Page* next;
		_Page* prev;
		_Page* nextFree;//completely free
		_Page* prevFree;//completely free
		};

	private:
	PageAlloc* allocator;
	size_t _itemSize;
	PageExtraData xtra;//will be populated to each page
	size_t nPageAllocPagesPerPage;
	size_t nPerPage;
	size_t nUsed;

	_Page* firstPageWithFree;//list of pages with free nodes
	//_Page* lastPageWithFree;//list of pages with free nodes

	_Page* firstFreePage;//list of completely free pages
	//_Page* lastFreePage;//list of completely free pages

	_Page* firstPage;//list of all pages
	//_Page* lastPage;//list of all pages

	int nPages;
	size_t removeSz;
	//vector< /* new */ _Page* > pages;

	public:
	PFixedSizeAllocator( PageAlloc& allocator_, int itemSize_, int nPageAllocPagesPerPage_, PageExtraData* xtra_ = 0 );
	//{ to use istead of 'normal' constructor
	PFixedSizeAllocator() {}
	void _construct( PageAlloc& allocator_, int itemSize_, int nPageAllocPagesPerPage_, PageExtraData* xtra_ = 0 );
	//}
	~PFixedSizeAllocator();

	size_t itemSize() const { return _itemSize; }
	size_t size() const
		{
		return nUsed;
		}
	BYTE* allocate()
		{
		if( firstPageWithFree == 0 )
			{
			_addNewPage();
			}
		PASSERT5( firstPageWithFree );
		_Page* page = firstPageWithFree;
		_FreeNode* node = page->firstFree;
		PASSERT5( node );

		//remove from free list on page
		if( node->prevFree )
			node->prevFree->nextFree = node->nextFree;
		else
			page->firstFree = node->nextFree;
		if( node->nextFree )
			node->nextFree->prevFree = node->prevFree;
		//else
		//	page->lastFree = node->prevFree;

		if( page->firstFree == 0 )
			{
			//PASSERT5( page->lastFree == 0 );

			//remove from list of pages with free nodes;
			//simplified as page here always == firstPageWithFree
			if( page->nextWithFree == 0 )
				{
				PASSERT5( firstPageWithFree == page );
				//PASSERT5( lastPageWithFree == page );
				firstPageWithFree = /*lastPageWithFree =*/ 0;
				}
			else
				{
				firstPageWithFree = page->nextWithFree;
				firstPageWithFree->prevWithFree = 0;
				}
			page->nextWithFree = page->prevWithFree = 0;//not really necessary, just in case; see also (*)
			}

		if( ++page->nUsed == 1 )
			{
			//--nFreePages;
			//remove from list of free pages
			if( page->prevFree )
				page->prevFree->nextFree = page->nextFree;
			else
				firstFreePage = page->nextFree;
			if( page->nextFree )
				page->nextFree->prevFree = page->prevFree;
			//else
			//	lastFreePage = page->prevFree;
			page->nextFree = page->prevFree = 0;//not really necessary, just in case; see also (**)
			}
		else if( !AllocTraits::isTemporaryOnly )
			{//attempt to maintain 'kinda-sort' on nUsed
			if( page->prevWithFree && page->nUsed > page->prevWithFree->nUsed )
				{//swap with prev
				_Page* prev = page->prevWithFree;
				
				prev->nextWithFree = page->nextWithFree;
				if( prev->nextWithFree )
					prev->nextWithFree->prevWithFree = prev;
				//else
				//	lastPageWithFree = prev;
				
				page->prevWithFree = prev->prevWithFree;
				if( page->prevWithFree )
					page->prevWithFree->nextWithFree = page;
				else
					firstPageWithFree = page;

				page->nextWithFree = prev;
				prev->prevWithFree = page;
				}
			}

		//increase count
		++nUsed;

		return ((BYTE*)node) + sizeof(_Node);
		}
	void deallocate( void* p )
		{
		_FreeNode* node = (_FreeNode*)(((BYTE*)p) - sizeof(_Node));

		//decrease count
		--nUsed;

		//add to free list on page
		_Page* page = node->page;
		if( page->firstFree == 0 )
			{
			//PASSERT5( page->lastFree == 0 );
			node->nextFree = node->prevFree = 0;
			page->firstFree = /*page->lastFree =*/ node;
			}
		else
			{//adding to head
			//PASSERT5( page->lastFree != 0 );
			node->nextFree = page->firstFree;
			page->firstFree = node;
			node->prevFree = 0;
			node->nextFree->prevFree = node;
			}

		PASSERT5( page->nUsed > 0 );
		if( --page->nUsed == 0 )
			{
			//++nFreePages;
			PASSERT5( page->nextFree == 0 );//(**)
			PASSERT5( page->prevFree == 0 );//(**)
			_addToFreePages( page );
			}
		
		if( page->nUsed == nPerPage - 1 )
			{
			PASSERT5( page->nextWithFree == 0 );//(*)
			PASSERT5( page->prevWithFree == 0 );//(*)
			_addToPagesWithFree( page );
			}
		else if( !AllocTraits::isTemporaryOnly )
			{//attempt to maintain 'kinda-sort' on nUsed
			if( page->nextWithFree && page->nUsed < page->nextWithFree->nUsed )
				{//swap with next
				_Page* next = page->nextWithFree;
				
				next->prevWithFree = page->prevWithFree;
				if( next->prevWithFree )
					next->prevWithFree->nextWithFree = next;
				else
					firstPageWithFree = next;
				
				page->nextWithFree = next->nextWithFree;
				if( page->nextWithFree )
					page->nextWithFree->prevWithFree = page;
				//else
				//	lastPageWithFree = page;

				page->prevWithFree = next;
				next->nextWithFree = page;
				}
			}

		//remove page if possible and necessary
		if( firstFreePage && nUsed < removeSz )//2nd condition is for hysteresis
			{//remove page
			_delFreePage();
			}
		}
	void _check();

	public://_mtGuess - should be safe to call from another thread and provide reasonable results most of the time
	int _mtGuessNPages() const { return nPages; }
	bool _mtGuessCanAllocMyself() const { return firstPageWithFree != 0; }

	private:
	void _addNewPage();
	void _delFreePage();
	void _addToPagesWithFree( _Page* page );
	void _addToFreePages( _Page* page );
	size_t _nodeSize() { return max( sizeof(_Node) + _itemSize, sizeof(_FreeNode) ); }
	void _calcRemoveSz()
		{
		//( pages.size() - 1 ) * nPerPage - nUsed > HYSTERESIS
		//assume HYSTERESIS = nPerPage - 1
		//( pages.size() - 1 ) * nPerPage - nUsed > nPerPage - 1
		//( pages.size() - 1 ) * nPerPage - nPerPage + 1 > nUsed
		//nUsed < ( pages.size() - 1 ) * nPerPage - nPerPage + 1

		int n = nPages;//pages.size();
		if( n < 2 )
			removeSz = 0;//never
		else
			removeSz = ( n - 2 ) * nPerPage + 1;
		}
	};

template< class PageAlloc, class AllocTraits, class PageExtraData >
PFixedSizeAllocator< PageAlloc, AllocTraits, PageExtraData >::PFixedSizeAllocator( PageAlloc& allocator_, int itemSize_, int nPageAllocPagesPerPage_, PageExtraData* xtra_ )
	{
	_construct( allocator_, itemSize_, nPageAllocPagesPerPage_, xtra_ );
	}

template< class PageAlloc, class AllocTraits, class PageExtraData >
void PFixedSizeAllocator< PageAlloc, AllocTraits, PageExtraData >::_construct( PageAlloc& allocator_, int itemSize_, int nPageAllocPagesPerPage_, PageExtraData* xtra_ )
	{
	allocator = &allocator_;

	PASSERT4( itemSize_ > 0 );
	_itemSize = itemSize_;
	if( xtra_ )
		xtra = *xtra_;

	nPageAllocPagesPerPage = nPageAllocPagesPerPage_;
	nPerPage = ( allocator->pageSize() * nPageAllocPagesPerPage - sizeof(_Page) ) / _nodeSize();
	PASSERT4( nPerPage > 0 );

	nUsed = 0;
	firstPageWithFree = /*lastPageWithFree =*/ 0;
	firstFreePage = /*lastFreePage =*/ 0;
	firstPage = /*lastPage =*/ 0;
	nPages = 0;
	_calcRemoveSz();
	}

template< class PageAlloc, class AllocTraits, class PageExtraData >
PFixedSizeAllocator< PageAlloc, AllocTraits, PageExtraData >::~PFixedSizeAllocator()
	{
	for( _Page* p = firstPage ; p ; )
		{
		_Page* nextP = p->next;
		allocator->deallocPages( (BYTE*)p, nPageAllocPagesPerPage );
		p = nextP;
		}
	}

template< class PageAlloc, class AllocTraits, class PageExtraData >
void PFixedSizeAllocator< PageAlloc, AllocTraits, PageExtraData >::_addToPagesWithFree( _Page* page )
	{
	//add to head of list of pages with free items
	if( firstPageWithFree == 0 )
		{
		//PASSERT5( lastPageWithFree == 0 );
		page->nextWithFree = page->prevWithFree = 0;
		firstPageWithFree = /*lastPageWithFree =*/ page;
		}
	else
		{//adding to head
		//_addToPagesWithFree is called when only 1 item is free, so adding to head will maintain 'kinda-sort' on nUsed
		//PASSERT5( lastPageWithFree != 0 );
		page->nextWithFree = firstPageWithFree;
		firstPageWithFree = page;
		page->prevWithFree = 0;
		page->nextWithFree->prevWithFree = page;
		}
	}

template< class PageAlloc, class AllocTraits, class PageExtraData >
void PFixedSizeAllocator< PageAlloc, AllocTraits, PageExtraData >::_addToFreePages( _Page* page )
	{
	//add to list of completely free pages
	if( firstFreePage == 0 )
		{
		//PASSERT5( lastFreePage == 0 );
		page->nextFree = page->prevFree = 0;
		firstFreePage = /*lastFreePage =*/ page;
		}
	else
		{//adding to head
		//PASSERT5( lastFreePage != 0 );
		page->nextFree = firstFreePage;
		firstFreePage = page;
		page->prevFree = 0;
		page->nextFree->prevFree = page;
		}
	}

template< class PageAlloc, class AllocTraits, class PageExtraData >
void PFixedSizeAllocator< PageAlloc, AllocTraits, PageExtraData >::_addNewPage()
	{
	PASSERT5( nPerPage >= 1 );

	BYTE* data = allocator->allocPages( nPageAllocPagesPerPage );
	_Page* page = (_Page*)data;
	page->xtra = xtra;
	BYTE* nodes = data + sizeof(_Page);
	size_t nodeSz = _nodeSize();

	//init free nodes
	if( nPerPage == 1 )
		{
		_FreeNode* node = (_FreeNode*)nodes;
		node->nextFree = 0;
		node->prevFree = 0;
		page->firstFree = node;
		//page->lastFree = node;
		node->page = page;
		}
	else
		{
		{//first one
		_FreeNode* node = (_FreeNode*)nodes;
		node->nextFree = (_FreeNode*)( nodes + nodeSz );
		node->prevFree = 0;
		page->firstFree = node;
		node->page = page;
		}
		for( int i=1; i < nPerPage - 1 ; ++i )
			{
			_FreeNode* node = (_FreeNode*)( nodes + i * nodeSz );
			node->nextFree = (_FreeNode*)( nodes + ( i + 1 ) * nodeSz );
			node->prevFree = (_FreeNode*)( nodes + ( i - 1 ) * nodeSz );
			node->page = page;
			}
		{//last one
		_FreeNode* node = (_FreeNode*)( nodes + ( nPerPage - 1 ) * nodeSz );
		node->nextFree = 0;
		node->prevFree = (_FreeNode*)( nodes + ( nPerPage - 2 ) * nodeSz );
		//page->lastFree = node;
		node->page = page;
		}
		}

	page->nUsed = 0;

	//pages.push_back( page );
	if( firstPage == 0 )
		{
		//PASSERT5( lastPage == 0 );
		page->next = page->prev = 0;
		firstPage = /*lastPage =*/ page;
		}
	else
		{//adding to head
		//PASSERT5( lastPage != 0 );
		page->next = firstPage;
		firstPage = page;
		page->prev = 0;
		page->next->prev = page;
		}
	++nPages;
	_calcRemoveSz();

	//++nFreePages;
	_addToFreePages( page );

	//_addToPagesWithFree( page );
	PASSERT5( firstPageWithFree == 0 );
	//PASSERT5( lastPageWithFree == 0 );
	page->nextWithFree = page->prevWithFree = 0;
	firstPageWithFree = /*lastPageWithFree =*/ page;
	}

template< class PageAlloc, class AllocTraits, class PageExtraData >
void PFixedSizeAllocator< PageAlloc, AllocTraits, PageExtraData >::_delFreePage()
	{
	PASSERT5( firstFreePage );
	_Page* delPage = firstFreePage;
	PASSERT5( delPage->nUsed == 0 );

	//remove from list of free pages;
	//simplified as delPage here always == firstFreePage
	if( delPage->nextFree == 0 )
		{
		PASSERT5( firstFreePage == delPage );
		//PASSERT5( lastFreePage == delPage );
		firstFreePage = /*lastFreePage =*/ 0;
		}
	else
		{
		firstFreePage = delPage->nextFree;
		firstFreePage->prevFree = 0;
		}
	//delPage->nextFree = page->prevFree = 0;//not really necessary, just in case; see also (**)

	//remove from list of pages with free nodes
	if( delPage->prevWithFree )
		delPage->prevWithFree->nextWithFree = delPage->nextWithFree;
	else
		firstPageWithFree = delPage->nextWithFree;
	if( delPage->nextWithFree )
		delPage->nextWithFree->prevWithFree = delPage->prevWithFree;
	//else
	//	lastPageWithFree = delPage->prevWithFree;

	//remove from list of all pages
	if( delPage->prev )
		delPage->prev->next = delPage->next;
	else
		firstPage = delPage->next;
	if( delPage->next )
		delPage->next->prev = delPage->prev;
	//else
	//	lastPage = delPage->prev;
	--nPages;

	allocator->deallocPages( (BYTE*)delPage, nPageAllocPagesPerPage );
	//pages.erase( pages.begin() + idx );
	//--nFreePages;

	_calcRemoveSz();
	}

template< class PageAlloc, class AllocTraits, class PageExtraData >
void PFixedSizeAllocator< PageAlloc, AllocTraits, PageExtraData >::_check()
	{
	int i=0;
	int ii=0;
	_Page* p;
	for( p = firstPage ; p ; p = p->next )
		{
		if( p->next )
			PASSERT5( p->next->prev == p );
		//else
		//	PASSERT5( p == lastPage );
		++i;

		int j=0;
		for( _FreeNode* nn = p->firstFree ; nn ; nn = nn->nextFree )
			{
			++j;
			}
		PASSERT5( j == nPerPage - p->nUsed );
		ii += j;

		if( p->nUsed == 0 )
			{
			bool Ok = false;
			for( _Page* pp = firstFreePage ; pp ; pp = pp->nextFree )
				{
				if( pp == p )
					{
					Ok = true;
					break;//for( pp )
					}
				}
			PASSERT5( Ok );
			}

		if( p->nUsed < nPerPage )
			{
			PASSERT5( p->firstFree );
			bool Ok = false;
			for( _Page* pp = firstPageWithFree ; pp ; pp = pp->nextWithFree )
				{
				if( pp == p )
					{
					Ok = true;
					break;//for( pp )
					}
				}
			PASSERT5( Ok );
			}
		else
			PASSERT5( p->firstFree == 0 );
		}
	PASSERT5( i == nPages );
	PASSERT5( ii == nPages * nPerPage - nUsed );

	for( p = firstFreePage ; p ; p = p->nextFree )
		{
		PASSERT5( p->nUsed == 0 );
		if( p->nextFree )
			PASSERT5( p->nextFree->prevFree == p );
		//else
		//	PASSERT5( p == lastFreePage );
		}

	for( p = firstPageWithFree ; p ; p = p->nextWithFree )
		{
		PASSERT5( p->nUsed < nPerPage );
		if( p->nextWithFree )
			PASSERT5( p->nextWithFree->prevWithFree == p );
		//else
		//	PASSERT5( p == lastPageWithFree );
		}
	}

//==============================================================================

struct _PMTBucketAllocatorExtraData
	{
	int allocIdx;
	int bucketNum;
	};

struct PMTAllocatorBucketStats
	{
	int nDelListAllocs;
	int nAllocs[ 4 ];//0 - 'good' (still after delList), '1' - notLocked, '2' - hasFree, '3' - worst
	int nDelListDeallocs;
	int nDeallocs[ 2 ];//0 - 'ideal' (before delList), '1' - sync
 
	void add( const PMTAllocatorBucketStats& other )
		{
		nDelListAllocs += other.nDelListAllocs;
		nAllocs[ 0 ] += other.nAllocs[ 0 ];
		nAllocs[ 1 ] += other.nAllocs[ 1 ];
		nAllocs[ 2 ] += other.nAllocs[ 2 ];
		nAllocs[ 3 ] += other.nAllocs[ 3 ];
		nDelListDeallocs += other.nDelListDeallocs;
		nDeallocs[ 0 ] += other.nDeallocs[ 0 ];
		nDeallocs[ 1 ] += other.nDeallocs[ 1 ];
		}
	};

template< class PageAlloc, class AllocParms >
class _PMTFixedSizeAllocatorBucket
//compatible with _PAllocGetPointerData
//returns pointer to _PMTBucketAllocatorExtraData as _PAllocGetPointerData result
//AllocParms requirements: AllocTraits requirements + enum{ NDelLists, NDelListItems }
	{
	private:
	struct _DelList
		{
		int guessLocked;//not under cs for reading; optimization-only; after successful check will cs.tryEnter() to make sure
		//int guessHasFree;//not under cs for reading; optimization-only; after successful check will re-check from under cs to make sure
		//int guessHasUsed;//not under cs for reading; optimization-only; after successful check will re-check from under cs to make sure

		PCriticalSection cs;
		int head;//head < 0 means 'empty'; tail still is valid
		int tail;//head == tail means 'full'
		BYTE* list[ AllocParms::NDelListItems ];

		bool guessHasFree() const { return head != tail; }
		bool guessHasUsed() const { return head >= 0; }
		};

	private:
	_DelList delLists[ AllocParms::NDelLists ];
	int _guessLocked;
	PCriticalSection cs;
	PFixedSizeAllocator< PageAlloc, AllocParms, _PMTBucketAllocatorExtraData > allocator;
	PMTAllocatorBucketStats stats;

	public:
/*	_PMTFixedSizeAllocatorBucket( PageAlloc& allocator_, int itemSize_, int nPageAllocPagesPerPage_ )
	: PFixedSizeAllocator( allocator_, itemSize_, nPageAllocPagesPerPage_ )
		{
		_guessLocked = 0;
		}*/

	//{ to use instead of 'normal' constructor
	_PMTFixedSizeAllocatorBucket() {}
	void _construct( PageAlloc& pageAlloc_, int itemSize_, int nPageAllocPagesPerPage_, _PMTBucketAllocatorExtraData* xtra_ )
		{
		allocator._construct( pageAlloc_, itemSize_, nPageAllocPagesPerPage_, xtra_ );
		_guessLocked = 0;

		for( int i=0; i < AllocParms::NDelLists ; ++i )
			{
			_DelList& list = delLists[ i ];
			list.guessLocked = 0;
			//list.guessHasFree = 1;
			//list.guessHasUsed = 0;
			list.head = -1;
			list.tail = 0;
			}

		memset( &stats, 0, sizeof(stats) );
		}
	//}

	public:
	const PMTAllocatorBucketStats& getStats() const { return stats; }
	bool guessLocked() const { return _guessLocked != 0;	}
	bool guessCanAllocMyself() const { return allocator._mtGuessCanAllocMyself(); }
	int guessNPages() const { return allocator._mtGuessNPages(); }

	void lock()
		{
		cs._lock();
		_guessLocked = 1;
		}
	bool tryLock()
		{
		bool ret = cs._tryLock();
		if( ret )
			_guessLocked = 1;
		return ret;
		}
	void unlock()
		{
		_guessLocked = 0;
		cs._unlock();
		}

	BYTE* tryPutDelItem( BYTE* p )
		{
		for( int i=0; i < AllocParms::NDelLists ; ++i )
		//no point in using getCurrentCPU: a) they are already separated at Allocator level ; b) experimentally confirmed
			{
			_DelList& list = delLists[ i ];
			if( !list.guessLocked && list.guessHasFree() )
				{
				bool locked = list.cs._tryLock();
				if( !locked )
					continue;//for( i )

				list.guessLocked = 1;
				if( list.head != list.tail )
					{
					list.list[ list.tail ] = (BYTE*)p;
					if( list.head < 0 )
						{
						list.head = list.tail;
						//list.guessHasUsed = 1;
						}
					if( ++list.tail == AllocParms::NDelListItems )
						list.tail = 0;
					//if( list.head == list.tail )
					//	list.guessHasFree = 0;

					++stats.nDelListDeallocs;
					list.guessLocked = 0;
					list.cs._unlock();
					return 0;
					}
				//unlikely but possible
				list.guessLocked = 0;
				list.cs._unlock();
				}
			}
		return p;
		}

	BYTE* tryGetDelItem()
		{
		for( int i=0; i < AllocParms::NDelLists ; ++i )
		//no point in using getCurrentCPU: a) they are already separated at Allocator level ; b) experimentally confirmed
			{
			_DelList& list = delLists[ i ];
			if( !list.guessLocked && list.guessHasUsed() )
				{
				bool locked = list.cs._tryLock();
				if( !locked )
					continue;//for( i )

				list.guessLocked = 1;
				if( list.head >= 0 )
					{
					BYTE* ret = list.list[ list.head ];
						
					if( ++list.head == AllocParms::NDelListItems )
						list.head = 0;
					if( list.head == list.tail )
						{
						list.head = -1;
						list.tail = 0;//not really mandatory
						//list.guessHasUsed = 0;
						}
					//list.guessHasFree = 1;

					++stats.nDelListAllocs;
					list.guessLocked = 0;
					list.cs._unlock();
					return ret;
					}
				list.guessLocked = 0;
				list.cs._unlock();
				}
			}
		return 0;
		}

	BYTE* allocateEx( size_t sz, size_t* retSz, int statsContext )
		{
		PASSERT5( sz <= allocator.itemSize() );
		if( retSz )
			*retSz = allocator.itemSize();
		++stats.nAllocs[ statsContext ];
		return allocator.allocate();
		}
	void deallocate( void* p, int statsContext )
		{
		++stats.nDeallocs[ statsContext ];
		allocator.deallocate( p );
		}
	size_t getAllocSize( void* p ) const
		{
		return allocator.itemSize();
		}
	};

template< class PageAlloc, class AllocTraits >
class _PMTGenericPageAllocatorBucket
//compatible with _PAllocGetPointerData
//returns pointer to _PMTBucketAllocatorExtraData as _PAllocGetPointerData result
	{
	private:
	struct _Hdr
		{
		size_t nPages;
		_PMTBucketAllocatorExtraData* xtra;//!!!IMPORTANT: MUST be immediately before actual data; to be compatible with _PAllocGetPointerData
		};

	private:
	//int _guessLocked;
	//PCriticalSection cs;//no need to lock?
	PageAlloc* allocator;
	_PMTBucketAllocatorExtraData xtra;

	PMTAllocatorBucketStats stats;

	public:
/*	_PMTGenericPageAllocatorBucket( PageAlloc& allocator_, int itemSize_, int nPageAllocPagesPerPage_ )
		{
		allocator = &allocator_;
		_guessLocked = 0;
		}*/

	//{ to use instead of 'normal' constructor
	_PMTGenericPageAllocatorBucket() {}
	void _construct( PageAlloc& pageAlloc_, _PMTBucketAllocatorExtraData* xtra_ )
		{
		allocator = &pageAlloc_;
		xtra = *xtra_;
		//_guessLocked = 0;
		memset( &stats, 0, sizeof(stats) );
		}
	//}

	public:
	const PMTAllocatorBucketStats& getStats() const { return stats; }
	bool guessLocked() const { return false; }//{ return _guessLocked != 0; }

	void lock()
		{
		//cs._lock();
		//_guessLocked = 1;
		}
	bool tryLock()
		{
		//bool ret = cs._tryLock();
		//if( ret )
		//	_guessLocked = 1;
		//return ret;
		return true;
		}
	void unlock()
		{
		//_guessLocked = 0;
		//cs._unlock();
		}

	BYTE* allocateEx( size_t sz, size_t* retSz, int statsContext )
		{
		int nPages = ( sz + sizeof(_Hdr) ) / allocator->pageSize();
		_Hdr* h = (_Hdr*)allocator->allocPages( nPages );
		h->nPages = nPages;//for dealloc
		h->xtra = &xtra;//compatibility
		if( retSz )
			*retSz = allocator->pageSize() * nPages - sizeof(_Hdr);
		++stats.nAllocs[ statsContext ];
		return (BYTE*)h + sizeof(_Hdr);
		}
	void deallocate( void* p, int statsContext )
		{
		_Hdr* h = (_Hdr*)(((BYTE*)p) - sizeof(_Hdr));
		++stats.nDeallocs[ statsContext ];
		allocator->deallocPages( (BYTE*)h, h->nPages );
		}
	size_t getAllocSize( void* p ) const
		{
		_Hdr* h = (_Hdr*)(((BYTE*)p) - sizeof(_Hdr));
		return allocator->pageSize() * h->nPages - sizeof(_Hdr);
		}
	};

template< class BucketAlloc, class AllocParms >
class PMTBucketAllocator
//BucketAlloc requirements:
//MUST consist only of buckets compatible with _PAllocGetPointerData
//                             and returning pointer to _PMTBucketAllocatorExtraData as _PAllocGetPointerData result
//functions:
//static int getBucketNum( size_t sz )
//
//BucketAlloc()
//
//bool guessLocked( int bucket )
//bool guessCanAllocItself( int bucket )
//int guessNPages( int bucket )
//
//void lock( int bucket )
//bool tryLock( int bucket )
//void unlock( int bucket )
//
//BYTE* allocate( int bucket, size_t sz ) - non-locked!
//void deallocate( int bucket, void* p ) - non-locked!
//
//
//AllocParms requirements:
//enum { NAllocators_Log2, NDeleteLists_Log2, NItemsPerDeleteList_Log2 };
	{
	public:
	enum { NAllocators = 1 << AllocParms::NAllocators_Log2 };

	private:
	_PAllocStartIndexGetter idxGetter;
	BucketAlloc allocators[ NAllocators ];

	public:
	PMTBucketAllocator();
	bool getCurrentCPUSupported() const { return idxGetter.getCurrentCPUSupported(); }
	int nBucketAllocators() const { return NAllocators; }
	BucketAlloc& _getBucketAllocator( int i ) { return allocators[ i ]; }

	BYTE* allocate( size_t sz ) { return allocateEx( sz, 0 ); }
	BYTE* allocateEx( size_t sz, size_t* retSz );//retSz always >= sz
	BYTE* reallocate( void* p, size_t sz ) { return reallocateEx( p, sz, 0 ); }
	BYTE* reallocateEx( void* p, size_t sz, size_t* retSz );
	void deallocate( void* p );
	~PMTBucketAllocator();

	static int getNBuckets() { return BucketAlloc::getNBuckets(); }
	void getStats( PMTAllocatorBucketStats& stats, int bucketNum ) const;

	private:
	/* inline */ BYTE* _syncAlloc( int allocIdx, int bucket, size_t sz, size_t* retSz, int statsContext );
	};

template< class BucketAlloc, class AllocParms >
PMTBucketAllocator< BucketAlloc, AllocParms >::PMTBucketAllocator()
	{
	}

template< class BucketAlloc, class AllocParms >
PMTBucketAllocator< BucketAlloc, AllocParms >::~PMTBucketAllocator()
	{
	}

template< class BucketAlloc, class AllocParms >
BYTE* PMTBucketAllocator< BucketAlloc, AllocParms >::reallocateEx( void* p, size_t sz, size_t* retSz )
	{
	const _PMTBucketAllocatorExtraData* xtra = (const _PMTBucketAllocatorExtraData*)_PAllocGetPointerData( p );
	BucketAlloc& allocator = allocators[ xtra->allocIdx ];
	int bucket = xtra->bucketNum;

	size_t curSz = allocator.getAllocSize( bucket, p );
	if( sz > curSz )
		{
		BYTE* ret = allocateEx( sz, retSz );
		memcpy( ret, p, curSz );
		deallocate( p );
		return ret;
		}
	else// if( sz <= curSz )
		{
		int newBucket = BucketAlloc::getBucketNum( sz );
		if( newBucket == bucket )
			{//do nothing
			return (BYTE*)p;
			}

		//not clear whether it is better to realloc here or to keep it as is...
		//current implementation will realloc
		BYTE* ret = allocateEx( sz, retSz );
		memcpy( ret, p, sz );
		deallocate( p );
		return ret;
		}
	}

template< class BucketAlloc, class AllocParms >
void PMTBucketAllocator< BucketAlloc, AllocParms >::deallocate( void* p )
	{
	const _PMTBucketAllocatorExtraData* xtra = (const _PMTBucketAllocatorExtraData*)_PAllocGetPointerData( p );
	BucketAlloc& allocator = allocators[ xtra->allocIdx ];
	int bucket = xtra->bucketNum;

	//try to lock 'our' bucket
	bool locked = 
#ifndef NOALLOCGUESS
		!allocator.guessLocked( bucket ) && 
#endif
		allocator.tryLock( bucket );
	if( locked )
		{
		allocator.deallocate( bucket, p, 0 );
		allocator.unlock( bucket );
		return;
		}

	p = allocator.tryPutDelItem( bucket, (BYTE*)p );
	if( p == 0 )
		{
		return;
		}

	//very unlikely - everything is locked
	//will go synchronous
//printf( "deallocSync\n" );
	allocator.lock( bucket );
	allocator.deallocate( bucket, p, 1 );
	allocator.unlock( bucket );
	}

template< class BucketAlloc, class AllocParms >
inline BYTE* PMTBucketAllocator< BucketAlloc, AllocParms >::_syncAlloc( int allocIdx, int bucket, size_t sz, size_t* retSz, int statsContext )
	{
	BucketAlloc& allocator = allocators[ allocIdx ];
	allocator.lock( bucket );
	BYTE* ret = allocator.allocateEx( bucket, sz, retSz, statsContext );
	allocator.unlock( bucket );
	return ret;
	}

template< class BucketAlloc, class AllocParms >
BYTE* PMTBucketAllocator< BucketAlloc, AllocParms >::allocateEx( size_t sz, size_t* retSz )
	{
	int bucket = BucketAlloc::getBucketNum( sz );

	UINT32 seed = idxGetter.getStartIndexSeed( NAllocators );
	//try to find unlocked bucket
	int i, firstHasFree = -1;
	int bestNotLocked = -1, bestNotLockedNPages;//'best' means 'with least pages'
	for( i=0; i < NAllocators ; ++i )
		{
		int idx = ( i + seed ) & ( NAllocators - 1 );//NAllocators is power of 2
		BucketAlloc& allocator = allocators[ idx ];

		BYTE* p = allocator.tryGetDelItem( bucket );
		if( p )
			{
			return p;
			}

#ifndef NOALLOCGUESS
		bool guessLocked = allocator.guessLocked( bucket );
		bool guessHasFree = allocator.guessCanAllocItself( bucket );
		if( !guessLocked && guessHasFree )
#else
		bool guessLocked = false;
		bool guessHasFree = true;
#endif
			{
			bool locked = allocator.tryLock( bucket );
			if( locked )
				{
				BYTE* ret = allocator.allocateEx( bucket, sz, retSz, 0 );
				allocator.unlock( bucket );
				return ret;
				}
			else
				{
				guessLocked = true;
				}
			}

		if( !guessLocked )
			{
			int nPages = allocator.guessNPages( bucket );
			if( bestNotLocked < 0 || nPages < bestNotLockedNPages )
				{
				bestNotLocked = idx;
				bestNotLockedNPages = nPages;
				}
			}
		if( guessHasFree && firstHasFree < 0 )
			firstHasFree = idx;
		}

	//unlikely - not found any unlocked && with free
	//not locked should go first, otherwise it will cause huge congestions at page boundaries
	if( bestNotLocked >= 0 )
		{
		return _syncAlloc( bestNotLocked, bucket, sz, retSz, 1 );
		}

	//very unlikely - not found any unlocked
	//will try at least to use one that has free slots
	if( firstHasFree >= 0 )
		{
		return _syncAlloc( firstHasFree, bucket, sz, retSz, 2 );
		}

	//neither !locked nor free
	int idx = seed & ( NAllocators - 1 );//NAllocators is power of 2
	return _syncAlloc( idx, bucket, sz, retSz, 3 );
	}

template< class BucketAlloc, class AllocParms >
void PMTBucketAllocator< BucketAlloc, AllocParms >::getStats( PMTAllocatorBucketStats& stats, int bucketNum ) const
	{
	memset( &stats, 0, sizeof(PMTAllocatorBucketStats) );
	for( int i=0; i < NAllocators; ++i )
		{
		const BucketAlloc& allocator = allocators[ i ];
		stats.add( allocator.getStats( bucketNum ) );
		}
	}

#endif
