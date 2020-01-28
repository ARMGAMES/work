#ifndef ppidmap_h_included
#define ppidmap_h_included

#include "ppinclude.h"
#include "pperror.h"
#include "pplogfile.h"

template< class T >
class PlibIdMap//tricky 'no collision' hashtable with efficient constant-time search
{
private:
	struct Item
	{
		UINT32 id;
		T data;
	};
	class Generator
	{
	private:
		UINT32 _next;

	public:
		Generator()
		{
			_next = ( (UINT32) TIME( 0 ) << 10 );// this can make a difference only if application was restarted 
			//		and there can be 'zombie' connections with ids 
			//		from previous application 'life'
			// << 10 is quite arbitrary: 
			// peeked on assumption that there will be no more than 1024 new ids/sec
		}
		UINT32 next()
		{
			return ++_next;
		}
	};

private:
	Generator gen;
	int nBits;	//capacity = 1 << nBits;
	//reallocation+rehashing occurs when usedItems exceeds capacity / 2
	Item* items;
	int usedItems;

public:
	PlibIdMap( int nBits_ = 8 );
	void clear();
	~PlibIdMap();

public:
	size_t size() const
	{
		return usedItems;
	}
	UINT32 add( const T& t );//NOTE: 0 is a prohibited value for id. Can be used as a special key
	void remove( UINT32 id )
	{
		removeIndex( _hash( id, nBits ) );
	}
	const T* find( UINT32 id ) const
	{
		if( id == 0 )
			return 0;
		Item& item = items[ _hash( id, nBits ) ];
		return item.id == id ? &item.data : 0;
	}
	T* find( UINT32 id )
	{
		if( id == 0 )
			return 0;
		Item& item = items[ _hash( id, nBits ) ];
		return item.id == id ? &item.data : 0;
	}

	T* firstIndex( int& i )
	{
		i = 0;
		return _nextIndex( i );
	}
	T* nextIndex( int& i )
	{
		++i;
		return _nextIndex( i );
	}

	const T* firstIndex( int& i ) const
	{
		i = 0;
		return _nextIndex( i );
	}
	const T* nextIndex( int& i ) const
	{
		++i;
		return _nextIndex( i );
	}

	UINT32 id( int index ) { return items[ index ].id; }
	//int index( UINT32 key ) { return _hash( key ) ); }
	void removeIndex( int index );

private:
	static UINT32 _hash( UINT32 id, int nBits )
	{//NOTE:	caution needed if changing hashing algorithm
		//			see comment about rehashing in 'add' function
		UINT32 ret = ( 214013 * (UINT32)id ) & ( ( 1 << nBits ) - 1 );
		//We should take less significant bits (instead of most significant according to Knuth vol.2), 
		//	because most significant bits will not likely change on id increments
		PASSERT5( ret < ( UINT32( 1 ) << nBits ) );
		return ret;
	}
	T* _nextIndex( int& i );
	const T* _nextIndex( int& i ) const;
};

template< class T >
PlibIdMap< T >::PlibIdMap( int nBits_ )
{
	PASSERT3( nBits_ >= 3 );
	nBits = nBits_;
	items = new Item[ (UINT32) (1 << nBits) ];
	int n = 1 << nBits;
	for( int i=0; i < n ; ++i )
		items[ i ].id = 0;
	usedItems = 0;
}


template< class T >
void PlibIdMap< T >::clear()
{
	int n = 1 << nBits;
	for( int i=0; i < n ; ++i )
		items[ i ].id = 0;
	usedItems = 0;
}

template< class T >
UINT32 PlibIdMap< T >::add( const T& t )
{
	if( ++usedItems >= ( 1 << ( nBits - 1 ) ) )
	{//NOTE:	rehashing with newBits > nBits will NOT create any collisions 
		//			because the _hash() function in this case just adds extra bits to the result
		//			Therefore, if hash keys were different, they will remain different
		int newBits = nBits + 1;
		PTRACE3( "PlibIdMap: 1/2 capacity limit exceeded - rehashing; usedItems=%d, newBits=%d, newSize=%u", usedItems, newBits, (UINT32) (1 << newBits));
		Item* newItems = new Item[ (UINT32) (1 << newBits) ];

		int i, n = 1 << newBits;
		for( i=0; i < n ; ++i )
			newItems[ i ].id = 0;

		n = 1 << nBits;
		for( i=0; i < n ; ++i )
		{
			Item& item = items[ i ];
			if( item.id )
			{
				Item& newItem = newItems[ _hash( item.id, newBits ) ];
				PASSERT3( newItem.id == 0 );//'no collision' check
				newItem.id = item.id;
				newItem.data = item.data;
			}
		}

		nBits = newBits;
		delete [] items;
		items = newItems;
		PTRACE3( "PlibIdMap: rehash completed" );
	}

	for( int i=0;; ++i )
	{
		UINT32 id = gen.next();
		if( id == 0 )
			continue;//forever
		Item& item = items[ _hash( id, nBits ) ];
		if( item.id != 0 )
		{
			//			if( i >= 4 && ( i >> 2 == 0 ) )
			//				{
			//				PTRACE3( "CommIdMap: collision #%d while generating id", i + 1 );
			//				}
			continue;//forever
		}

		if (i>=24)
		{
			PTRACE3( "PlibIdMap: %d collisions occured", i );
		}
		item.id = id;
		item.data = t;
		return id;
	}
}


template< class T >
T* PlibIdMap< T >::_nextIndex( int& i )
{
	for(;;)
	{
		if( i >= ( 1 << nBits ) )
			return 0;
		if( items[ i ].id != 0 )
			return &items[ i ].data;
		++i;
	}
}

template< class T >
const T* PlibIdMap< T >::_nextIndex( int& i ) const
{
	for(;;)
	{
		if( i >= ( 1 << nBits ) )
			return 0;
		if( items[ i ].id != 0 )
			return &items[ i ].data;
		++i;
	}
}

template< class T >
void PlibIdMap< T >::removeIndex( int index )
{
	Item& item = items[ index ];
	PASSERT3( item.id != 0 );
	item.id = 0;
	--usedItems;
}

template< class T >
PlibIdMap< T >::~PlibIdMap()
{
	delete [] items;
}

#endif
