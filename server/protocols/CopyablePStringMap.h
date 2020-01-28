#ifndef CopyablePStringMap_h_included
#define CopyablePStringMap_h_included

////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2008 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "ppcontainer.h"

////////////////////////////////////////////////////////////////////////////////

// Sometimes we need to store a PStringMap in a copyable object.  However,
// PStringMap does not support assignment and copying so we need to subclass it.
//lint -esym(1509, CopyablePStringMap) -esym(1512, CopyablePStringMap) -e{578} -e{1942}

template<typename T>
class CopyablePStringMap : public PStringMap<T>
{
	typedef PStringMap<T> _Parent; //lint !e1516

public:
	typedef TYPENAME _Parent::key_type					key_type;
	typedef TYPENAME _Parent::value_type				value_type;
	typedef TYPENAME _Parent::mapped_type				mapped_type;
	typedef TYPENAME _Parent::iterator					iterator;
	typedef TYPENAME _Parent::const_iterator			const_iterator;
	typedef TYPENAME _Parent::reverse_iterator			reverse_iterator;
	typedef TYPENAME _Parent::const_reverse_iterator	const_reverse_iterator;

public:
	CopyablePStringMap() {}
	CopyablePStringMap(const CopyablePStringMap& other) { _copy( other ); } //lint !e1538
	CopyablePStringMap& operator=(const CopyablePStringMap& other)
	{
		if (&other != this)
		{
			this->clear();
			_copy(other);
		}
		return *this;
	}

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
	CopyablePStringMap( CopyablePStringMap&& other ) { this->swap( other ); }
	CopyablePStringMap& operator =( CopyablePStringMap&& other )
	{
		PASSERT5( &other != this );
		this->clear();
		this->swap( other );
		return *this;
	}
#endif

private:
	void _copy(const CopyablePStringMap& other)
	{
		for (const_iterator it = other.begin(); it != other.end(); ++it)
			_Parent::insert(*it);
	}
};

////////////////////////////////////////////////////////////////////////////////

#endif // CopyablePStringMap_h_included
