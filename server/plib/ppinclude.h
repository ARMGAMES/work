#ifndef ppinclude_h_included
#define ppinclude_h_included

// PLIB - Platform LIBrary
// Platform macros: LINUX, _WIN32, P_AIX32, P_AIX64, P_MAC10X
// Additional macros: 
//		PBIG_ENDIAN/PLITTLE_ENDIAN (default - none defined)
//		PNOTHREADS (default=OFF)

// ************** PLATFORM-SPECIFIC DEFINITIONS

#include "ppincludebase.h"

#if defined( LINUX ) && !defined(P_ANDROID)
// ************** PLATFORM-SPECIFIC DEFINITIONS: LINUX

#include <vector>
#include <deque>
#include <set>
#include <map>
#include <list>
#include <queue>
#include <functional>
#include <algorithm>
#include <typeinfo>
using namespace std;

#elif defined( P_ANDROID )
// ************** PLATFORM-SPECIFIC DEFINITIONS: ANDROID

#include <vector>
#include <deque>
#include <set>
#include <map>
#include <list>
#include <queue>
#include <functional>
#include <algorithm>
#include <typeinfo>
using namespace std;



#elif defined( P_MAC10X )
// ************** PLATFORM-SPECIFIC DEFINITIONS: MAC

#include <vector>
#include <deque>
#include <set>
#include <map>
#include <list>
#include <queue>
#include <functional>
#include <algorithm>
#include <typeinfo>
using namespace std;

#elif defined( P_AIX32 )
// ************** PLATFORM-SPECIFIC DEFINITIONS: AIX GCC 32 bits

#include <vector>
#include <deque>
#include <set>
#include <map>
#include <list>
#include <queue>
#include <functional>
#include <algorithm>
#include <typeinfo>
using namespace std;

#elif defined( P_AIX64 )
// ************** PLATFORM-SPECIFIC DEFINITIONS: AIX XLC 64 bits

#include <vector>
#include <deque>
#include <set>
#include <map>
#include <list>
#include <queue>
#include <functional>
#include <algorithm>
#include <typeinfo>
using namespace std;

#elif defined( PWIN64 )
// ************** PLATFORM-SPECIFIC DEFINITIONS: WIN64

#if defined( _MSC_VER )
#if _MSC_VER < 1700
# pragma warning ( disable:4786 )//identifier was truncated in the debug information
# pragma warning ( disable:4018 )// '</>/...' : signed/unsigned mismatch
# pragma warning ( disable:4244 )//conversion from 'blah-blah' to 'yada-yada', possible loss of data
# pragma warning ( disable:4267 )//conversion from size_t to 32-bit
#endif
# pragma warning ( 3:4706 )//assignment within if
# pragma warning ( error:4541 )//dynamic_cast w/o RTTI enabled
# pragma warning ( disable:4996 )//was declared deprecated
#endif

#include <vector>
#include <deque>
#include <map>
#include <set>
#include <list>
#include <queue>
#include <functional>
#include <algorithm>
#include <typeinfo>
#include <iterator>
using namespace std;

#elif defined( PWIN32 )
// ************** PLATFORM-SPECIFIC DEFINITIONS: WIN32

#if defined( _MSC_VER )

#if _MSC_VER < 1700
# pragma warning ( disable:4786 )//identifier was truncated in the debug information
# pragma warning ( disable:4018 )// '</>/...' : signed/unsigned mismatch
# pragma warning ( disable:4244 )//conversion from 'blah-blah' to 'yada-yada', possible loss of data
# pragma warning ( disable:4267 )//conversion from size_t to 32-bit
#endif

# pragma warning ( 3:4706 )//assignment within if
# pragma warning ( error:4541 )//dynamic_cast w/o RTTI enabled

# if _MSC_VER >= 1400 //Visual Studio 2005+
# pragma warning ( disable: 4996 )//was declared deprecated
# endif

#endif

#include <vector>
#include <deque>
#include <map>
#include <set>
#include <list>
#include <queue>
#include <functional>
#include <algorithm>
#include <typeinfo.h>
#include <iterator>
using namespace std;

#else
#error Unknown platform
#endif

// ************** END OF PLATFORM-SPECIFIC DEFINITIONS
#include "ppsig.h"


template< class T >
class PAutoPtr
{
private:
	T* p;

public:
	PAutoPtr()
	{
		p = 0;
	}

	PAutoPtr( T* p_ )
	{
		p = p_;
	}
	~PAutoPtr()
	{
		delete p;
	}

public:
	operator const T*() const
	{
		return p;
	}
	operator T*()
	{
		return p;
	}

	const T* operator->() const
	{
		return p;
	}
	T* operator->()
	{
		return p;
	}

	const T& operator*() const
	{
		return *p;
	}
	T& operator*()
	{
		return *p;
	}

	T* extract()
	{
		T* ret = p;
		p = 0;
		return ret;
	}
	void swap( PAutoPtr & other)
	{
		T* ret = p;
		p = other.p;
		other.p=ret;
	}

	void reset( T* p_ = 0 )
	{
		if( p != p_ )
		{
			delete p;
			p = p_;
		}
	}

private://prohibited
	PAutoPtr( const PAutoPtr< T >& other );
	PAutoPtr& operator =( const PAutoPtr< T >& other );
};

template< class T >
class PArrayAutoPtr
{
private:
	T* p;

public:
	PArrayAutoPtr( T* p_ )
	{
		p = p_;
	}
	~PArrayAutoPtr()
	{
		delete [] p;
	}

public:
	operator const T*() const
	{
		return p;
	}
	operator T*()
	{
		return p;
	}

	const T& operator[]( int i ) const
	{
		return p[ i ];
	}
	T& operator[]( int i )
	{
		return p[ i ];
	}

	T* extract()
	{
		T* ret = p;
		p = 0;
		return ret;
	}

private://prohibited
	PArrayAutoPtr( const PArrayAutoPtr< T >& other );
	PArrayAutoPtr& operator =( const PArrayAutoPtr< T >& other );
};

#include <memory>

template< class T >
class PNewPtrVector : private vector< T* >
{
private:
	typedef vector< T* > _Parent;

public:
	PNewPtrVector()
	{
	}

public:
	void clear()
	{
		_clear();
		_Parent::clear();
	}
	~PNewPtrVector()
	{
		_clear();
	}

public:
	typedef TYPENAME _Parent::value_type value_type;
	typedef TYPENAME _Parent::iterator iterator;
	typedef TYPENAME _Parent::const_iterator const_iterator;
	typedef TYPENAME _Parent::reverse_iterator reverse_iterator;
	typedef TYPENAME _Parent::const_reverse_iterator const_reverse_iterator;

public:
	void push_back( /* new */ T* t )
	{
		_Parent::push_back( t );
	}
	void push_back( std::unique_ptr< T >&& p )
	{
		push_back( p.release() );
	}
	iterator insert( iterator where, /* new */ T* t )
	{
		return _Parent::insert( where, t );
	}
	iterator insert( iterator where, std::unique_ptr< T >&& p )
	{
		return insert( where, p.release() );
	}
	void set( iterator iter, /* new */ T* t )
	{
		delete *iter;
		(*iter) = t;
	}
	void set( iterator iter, std::unique_ptr< T >&& p )
	{
		set( p.release() );
	}
	iterator erase( iterator iter )
	{
		delete *iter;
		return _Parent::erase( iter );
	}
	/* new */ T* extract( iterator iter )
	{
		T* ret = *iter;
		*iter = 0;
		return ret;
	}

public:
	const T* operator[]( int i ) const
	{
		return _Parent::operator[]( i );
	}
	T* operator[]( int i )
	{
		return _Parent::operator[]( i );
	}

public:
	void swap( PNewPtrVector< T >& other )
	{
		_Parent::swap( other );
	}

public:
	using _Parent::size;
	using _Parent::empty;
	using _Parent::begin;
	using _Parent::end;
	using _Parent::rbegin;
	using _Parent::rend;
	using _Parent::reserve;

private:
	void _clear();

private://prohibited
	PNewPtrVector( const PNewPtrVector< T >& other );
	PNewPtrVector& operator =( const PNewPtrVector< T >& other );
};

template< class T >
void PNewPtrVector< T >::_clear()
{
	for( TYPENAME _Parent::iterator iter = _Parent::begin(); iter != _Parent::end() ; ++iter )
		delete *iter;
}

#endif
