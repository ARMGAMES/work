#ifndef ppcontainer_h_included
#define ppcontainer_h_included

#include "ppinclude.h"
#include "ppstring.h"


struct PStringCmp : public binary_function< const char*, const char*, bool >
{
public:
	bool operator() ( const char* a, const char* b ) const
	{
		return strcmp( a, b ) < 0;
	}
};

//Warning: implementation of the PStringMap is tricky, designed to avoid creating PString object(s) for each find
//Functionally PStringMap is almost equivalent to map< PString, T >
template< class T, class Pr = PStringCmp >
class PStringMap : private map< const char*, T, Pr >
{
private:
	typedef map< const char*, T, Pr > _Parent;
	//in fact, it should a map of char* to T, but it will create problems with types for find etc.

public:
	PStringMap()
	{
	}
	~PStringMap()
	{
		_clear();
	}

public:
#ifdef MOVE_CONSTRUCTOR_SUPPORTED
	PStringMap( PStringMap&& other ) { swap( other ); }
	PStringMap& operator=( PStringMap&& other ) { swap( other ); return *this; }
#endif

public:
	typedef TYPENAME _Parent::key_type key_type;
	typedef TYPENAME _Parent::value_type value_type;
	typedef TYPENAME _Parent::iterator iterator;
	typedef TYPENAME _Parent::const_iterator const_iterator;
	typedef TYPENAME _Parent::reverse_iterator reverse_iterator;
	typedef TYPENAME _Parent::const_reverse_iterator const_reverse_iterator;
	typedef TYPENAME _Parent::mapped_type mapped_type; // AN - checked with DL it was ok to add it

	pair< iterator, bool > insert( const value_type& x )
	{
		char* key = _dup( x.first );
		pair< iterator, bool > ret = _Parent::insert( value_type( key, x.second ) );
		if( !ret.second )
			_free( key );
		return ret;
	}
	pair< iterator, bool > insert( value_type&& x )
	{
		char* key = _dup( x.first );
		pair< iterator, bool > ret = _Parent::insert( value_type( key, move( x.second ) ) );
		if( !ret.second )
			_free( key );
		return ret;
	}
	pair< iterator, bool > insert( const char* s, const T& x )
	{
		return insert( make_pair( s, x ) );
	}
	pair< iterator, bool > insert( const char* s, T&& x )
	{
		return insert( make_pair( s, move( x ) ) );
	}
	pair< iterator, bool > insert( const char* s, int n, const T& x )
	{
		char* key = _dup( s, n );
		pair< iterator, bool > ret = _Parent::insert( value_type( key, x ) );
		if( !ret.second )
			_free( key );
		return ret;
	}
	pair< iterator, bool > insert( const char* s, int n, T&& x )
	{
		char* key = _dup( s, n );
		pair< iterator, bool > ret = _Parent::insert( value_type( key, move( x ) ) );
		if( !ret.second )
			_free( key );
		return ret;
	}
	iterator erase( iterator pos )
	{
		char* key = const_cast< char* >( pos->first );//see comment re. _Parent
		
#if defined (P_MAC10X)
		iterator posNew = pos;
		++posNew;
		_Parent::erase( pos );
#else
		iterator posNew = _Parent::erase( pos );
#endif
		
		_free( key );
		return posNew;
	}

	const_iterator erase( const_iterator pos )
	{
		char* key = const_cast< char* >( pos->first );//see comment re. _Parent
#if defined (P_MAC10X)
		const_iterator posNew = pos;
		++posNew;
		_Parent::erase( pos );
#else
		const_iterator posNew = _Parent::erase( pos );
#endif
		_free( key );
		return posNew;
	}
	size_t erase( const char* x )
	{
		iterator pos = find( x );
		if( pos == end() )
			return 0;
		erase( pos );
		return 1;
	}
	void clear()
	{
		_clear();
	}

	void swap( PStringMap< T, Pr >& other )
	{
		_Parent::swap( other );
	}

public:
	using _Parent::begin;
	using _Parent::end;
	using _Parent::rbegin;
	using _Parent::rend;
	using _Parent::size;
	using _Parent::empty;

	using _Parent::find;
	using _Parent::lower_bound;
	using _Parent::upper_bound;

	using _Parent::cbegin;
	using _Parent::cend;
	using _Parent::crbegin;
	using _Parent::crend;

private:
	static char* _dup( const char* s )
	{
		return s ? strdup( s ) : 0;
	}
	static char* _dup( const char* s, int n )
	{
		if( n )
		{
			char* p = ( char* )malloc( n + 1 );
			strncpy( p, s, n );
			p[ n ] = 0;
			return p;
		}
		else
		{
			return 0;
		}
	}
	static void _free( char* s )
	{
		if( s )
			free( s );
	}
	void _clear();

private://prohibited
	PStringMap( const PStringMap< T, Pr >& other );
	PStringMap& operator =( const PStringMap< T, Pr >& other );
};

template< class T, class Pr >
void PStringMap< T, Pr >::_clear()
{
	for( iterator iter = begin(); iter != end(); ++iter )
		_free( const_cast< char* >( iter->first ) );
	_Parent::clear();
}

template< class Pr >
class PStringSetT : private set< const char*, Pr >
{
private:
	typedef set< const char*, Pr > _Parent;
	//in fact, it should a set of char*, but it will create problems with types for find etc.

public:
	PStringSetT()
	{
	}
	~PStringSetT()
	{
		_clear();
	}

public:
#ifdef MOVE_CONSTRUCTOR_SUPPORTED
	PStringSetT( PStringSetT&& other ) { swap( other ); }
	PStringSetT& operator=( PStringSetT&& other ) { swap( other ); return *this; }
#endif

public:
	typedef TYPENAME _Parent::key_type key_type;
	typedef TYPENAME _Parent::value_type value_type;
	typedef TYPENAME _Parent::iterator iterator;
	typedef TYPENAME _Parent::const_iterator const_iterator;

	pair< iterator, bool > insert( const char* x )
	{
		char* key = _dup( x );
		pair< iterator, bool > ret = _Parent::insert( key );
		if( !ret.second )
			_free( key );
		return ret;
	}
	pair< iterator, bool > insert( const char* x, int n )
	{
		char* key = _dup( x, n );
		pair< iterator, bool > ret = _Parent::insert( key );
		if( !ret.second )
			_free( key );
		return ret;
	}
	void erase( iterator pos )
	{
		char* key = const_cast< char* >( *pos );//see comment re. _Parent
		_Parent::erase( pos );
		_free( key );
	}
	size_t erase( const char* x )
	{
		iterator pos = find( x );
		if( pos == end() )
			return 0;
		erase( pos );
		return 1;
	}
	void clear()
	{
		_clear();
	}

public:
	using _Parent::begin;
	using _Parent::end;
	using _Parent::rbegin;
	using _Parent::rend;
	using _Parent::size;
	using _Parent::empty;

	using _Parent::find;
	using _Parent::lower_bound;
	using _Parent::upper_bound;

#if _MSC_VER >= 1700
	using _Parent::cbegin;
	using _Parent::cend;
	using _Parent::crbegin;
	using _Parent::crend;
#endif

public:
	void swap( PStringSetT& other )
	{
		_Parent::swap( other );
	}

private:
	static char* _dup( const char* s )
	{
		return s ? strdup( s ) : 0;
	}
	static char* _dup( const char* s, int n )
	{
		if( n )
		{
			char* p = ( char* )malloc( n + 1 );
			strncpy( p, s, n );
			p[ n ] = 0;
			return p;
		}
		else
		{
			return 0;
		}
	}
	static void _free( char* s )
	{
		if( s )
			free( s );
	}
	void _clear();

private://prohibited
	PStringSetT( const PStringSetT& other );
	PStringSetT& operator =( const PStringSetT& other );
};

template< class Pr >
void PStringSetT< Pr >::_clear()
{
	for( iterator iter = begin(); iter != end(); ++iter )
		_free( const_cast< char* >( *iter ) );
	_Parent::clear();
}

typedef PStringSetT< PStringCmp > PStringSet;

struct PNameValue
{
public:
	PString name;
	PString value;

public:
	PNameValue()
	{
	}
	PNameValue( const char* name_, const char* value_ )
		: name( name_ ),
		value( value_ )
	{
	}
};

#endif
