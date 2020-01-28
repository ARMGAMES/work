#ifndef ppseque_h_included
#define ppseque_h_included

template< class T, class PAlloc >
class PQSeque
//Single-linked-list-based container for PQueue
//to replace less effective PQDeque
	{
	private:
	struct _Page;
	struct _Node
		{
		_Node* next;
		_Page* page;
		BYTE data[ sizeof(T) ];
		};
	struct _Page
		{
		int nPerPage;//this page
		_Node* free;
		size_t nUsed;
		_Page* nextWithFree;
		_Page* prevWithFree;
		_Page* next;
		_Page* prev;
		_Page* nextFree;//completely free
		_Page* prevFree;//completely free
		};

	private:
	PAlloc* allocator;
	size_t minPerPage;//
	_Node* first;
	_Node* last;
	size_t nUsed;
	size_t nAlloc;//sum of page->nPerPage

	_Page* firstPageWithFree;//list of pages with free nodes
	//_Page* lastPageWithFree;//list of pages with free nodes

	_Page* firstFreePage;//list of completely free pages
	//_Page* lastFreePage;//list of completely free pages

	_Page* firstPage;//list of all pages
	//_Page* lastPage;//list of all pages

	int nPages;
	size_t removeSz;

	public:
	typedef T value_type;

	public:
	PQSeque( PAlloc& allocator_, int minPerPage_ = 16 )
		{
		_construct( allocator_, minPerpage_ );
		}
	//{ pair to be used instead of 'normal' constructor
	PQSeque();
	void _construct( PAlloc& allocator_, int minPerPage_ = 16 );
	//}
	~PQSeque();
	static int pageSizeToNPerPage( size_t pageSz ) { return ( pageSz - sizeof(_Page) ) / sizeof(_Node); }

	size_t size() const
		{
		return nUsed;
		}
	void push( const T& t )
		{
		if( firstPageWithFree == 0 )
			{
			_addNewPage();
			}
		PASSERT5( firstPageWithFree );
		_Page* page = firstPageWithFree;
		_Node* node = page->free;
		PASSERT5( node );

		//remove from free list on page
		page->free = node->next;
		if( page->free == 0 )
			{
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

		//insert into the end of slist
		if( last == 0 )
			{
			PASSERT5( first == 0 );
			first = last = node;
			node->next = 0;
			}
		else
			{
			PASSERT5( first != 0 );
			PASSERT5( last->next == 0 );
			last->next = node;
			node->next = 0;
			last = node;
			}

		//increase count
		++nUsed;

		//construct object
		new( node->data ) T( t );
		}
	const T& peek() const
		{
		PASSERT4( size() > 0 );
		PASSERT5( first != 0 );
		PASSERT5( last != 0 );
		return *(T*)(first->data);
		}
	void pop()
		{
		PASSERT4( size() > 0 );
		PASSERT5( first != 0 );
		PASSERT5( last != 0 );

		//destruct object
		(*(T*)(first->data)).T::~T();

		//decrease count
		--nUsed;

		//remove from slist
		_Node* node = first;
		if( last == first )
			first = last = 0;
		else
			first = first->next;
		
		//add to free list on page
		_Page* page = node->page;
		node->next = page->free;//may be 0
		page->free = node;
		PASSERT5( page->nUsed > 0 );
		if( --page->nUsed == 0 )
			{
			//++nFreePages;
			PASSERT5( page->nextFree == 0 );//(**)
			PASSERT5( page->prevFree == 0 );//(**)
			_addToFreePages( page );
			}
		
		if( page->nUsed == page->nPerPage - 1 )
			{
			PASSERT5( page->nextWithFree == 0 );//(*)
			PASSERT5( page->prevWithFree == 0 );//(*)
			_addToPagesWithFree( page );
			}

		//remove page if possible and necessary
		if( firstFreePage && nUsed < removeSz )//2nd condition is for hysteresis
			{//remove page
			_delFreePage();
			}
		}
	void _check();

	private:
	void _addNewPage();
	void _delFreePage();
	void _addToPagesWithFree( _Page* page );
	void _addToFreePages( _Page* page );
	void _calcRemoveSz()
		{
		// nAlloc - nPerPage - nUsed > HYSTERESIS
		//nAlloc - nUsed > HYSTERESIS + nPerPage (which page?)
		//nUsed < nAlloc - HYSTERESIS - nPerPage
		//assuming HYSTERESIS = nPerPage - 1, and nPerPage = avgPerPage = nAlloc / nPages
		//nUsed < nAlloc - 2 * avgPerPage + 1


		if( nPages == 0 )
			removeSz = 0;
		else
			{
			int avgPerPage = nAlloc / nPages;
			int rSz = nAlloc - 2 * avgPerPage + 1;
			removeSz = rSz < 0 ? 0 : rSz;
			}
		}
	};

template< class T, class PAlloc >
PQSeque< T, PAlloc >::PQSeque()
	{
	allocator = 0;
	}

template< class T, class PAlloc >
void PQSeque< T, PAlloc >::_construct( PAlloc& allocator_, int minPerPage_ )
	{
	allocator = &allocator_;

	PASSERT( minPerPage_ > 0 );
	minPerPage = minPerPage_;
	first = last = 0;
	nUsed = 0;
	nAlloc = 0;
	firstPageWithFree = /*lastPageWithFree =*/ 0;
	firstFreePage = /*lastFreePage =*/ 0;
	firstPage = /*lastPage =*/ 0;
	nPages = 0;
	_calcRemoveSz();
	}

template< class T, class PAlloc >
PQSeque< T, PAlloc >::~PQSeque()
	{
	for( _Node* n = first ; n ; n = n->next )
		{
		(*(T*)(n->data)).T::~T();
		}

	for( _Page* p = firstPage ; p ; )
		{
		_Page* nextP = p->next;
		allocator->deallocate( p );
		p = nextP;
		}
	}

template< class T, class PAlloc >
void PQSeque< T, PAlloc >::_addToPagesWithFree( _Page* page )
	{
	//add to list of pages with free items; always adding to head - as it will ensure that last page in the list will never be inserted into and will eventually free
	if( firstPageWithFree == 0 )
		{
		//PASSERT5( lastPageWithFree == 0 );
		page->nextWithFree = page->prevWithFree = 0;
		firstPageWithFree = /*lastPageWithFree =*/ page;
		}
	else
		{//adding to head
		//PASSERT5( lastPageWithFree != 0 );
		page->nextWithFree = firstPageWithFree;
		firstPageWithFree = page;
		page->prevWithFree = 0;
		page->nextWithFree->prevWithFree = page;
		}
	}

template< class T, class PAlloc >
void PQSeque< T, PAlloc >::_addToFreePages( _Page* page )
	{
	//add to list of pages with completely free pages
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

template< class T, class PAlloc >
void PQSeque< T, PAlloc >::_addNewPage()
	{
	PASSERT5( minPerPage > 0 );

	size_t reqSz, actualSz;
	reqSz = sizeof(_Page) + minPerPage * sizeof(_Node);
	BYTE* data = allocator->allocateEx( reqSz, &actualSz );
	PASSERT5( actualSz >= reqSz );

	_Page* page = (_Page*)data;
	page->nPerPage = ( actualSz - sizeof(_Page) ) / sizeof(_Node);
	PASSERT5( page->nPerPage >= minPerPage );

	_Node* nodes = (_Node*)(data + sizeof(_Page));
	for( int i=0; i < page->nPerPage - 1 ; ++i )
		{
		_Node* node = nodes + i;
		node->next = nodes + ( i + 1 );
		node->page = page;
		}
	nodes[ page->nPerPage - 1 ].next = 0;
	nodes[ page->nPerPage - 1 ].page = page;

	page->free = nodes + 0;
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
	nAlloc += page->nPerPage;
	_calcRemoveSz();

	//++nFreePages;
	_addToFreePages( page );

	//_addToPagesWithFree( page );
	PASSERT5( firstPageWithFree == 0 );
	//PASSERT5( lastPageWithFree == 0 );
	page->nextWithFree = page->prevWithFree = 0;
	firstPageWithFree = /*lastPageWithFree =*/ page;
	}

template< class T, class PAlloc >
void PQSeque< T, PAlloc >::_delFreePage()
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
	nAlloc -= delPage->nPerPage;
	PASSERT5( nAlloc >= nUsed );
	--nPages;

	allocator->deallocate( delPage );
	//pages.erase( pages.begin() + idx );
	//--nFreePages;

	_calcRemoveSz();
	}

template< class T, class PAlloc >
void PQSeque< T, PAlloc >::_check()
	{
	int i=0;
	_Node* n;
	for( n = first ; n ; n = n->next )
		{
		if( n->next == 0 )
			PASSERT5( n == last );
		++i;
		}
	PASSERT5( i == nUsed );

	i=0;
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
		for( _Node* nn = p->free ; nn ; nn = nn->next )
			{
			++j;
			}
		PASSERT5( j == p->nPerPage - p->nUsed );
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

		if( p->nUsed < p->nPerPage )
			{
			PASSERT5( p->free );
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
			PASSERT5( p->free == 0 );
		}
	PASSERT5( i == nPages );
	PASSERT5( ii == nAlloc - nUsed );

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
		PASSERT5( p->nUsed < p->nPerPage );
		if( p->nextWithFree )
			PASSERT5( p->nextWithFree->prevWithFree == p );
		//else
		//	PASSERT5( p == lastPageWithFree );
		}
	}

#endif
