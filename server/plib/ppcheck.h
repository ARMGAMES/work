#ifndef ppcheck_h_included
#define ppcheck_h_included

#include "pperror.h"

#if defined( _DEBUG )

#define DEBUG_CHECK_TABLE_ORDER( table )									\
	static _DebugCheckTableOrder table##DebugOrder_( table )

#define _DEBUG_CHECK_TABLE_ORDER( table, tableEnd )							\
	static _DebugCheckTableOrder table##DebugOrder_( table, tableEnd )

class _DebugCheckTableOrder
	{
	public:
	template< class T >
	_DebugCheckTableOrder( const T* table, const T* tableEnd )
		{
		const T* l = table;
		for( const T* r = l + 1; r < tableEnd; ++l, ++r )
			PASSERT( *l < *r );
		}

	template< class T, size_t N >
	_DebugCheckTableOrder( const T( &table )[ N ] )
		: _DebugCheckTableOrder( table, table + N )
		{
		}
	};

#else

#define DEBUG_CHECK_TABLE_ORDER( table )
#define _DEBUG_CHECK_TABLE_ORDER( table, tableEnd )

#endif

#endif
