#include "ppincludebase.h"
#include "ppblock.h"

inline void _PBlock::_exactAppend( size_t sz_ )
{
	PASSERT5( _isValid() );
	sz += sz_;
	if( sz > reserved )
	{
		reserved = sz;
		p = _realloc( p, reserved );
	}
}

BYTE* _PBlock::insert( size_t pos, size_t sz_ )
{
	PASSERT5( _isValid() );
	PASSERT5( pos <= sz );
	if( sz + sz_ > reserved )
	{
		reserved = _growSize( sz + sz_ );
		p = _realloc( p, reserved );
	}
	memmove( p + pos + sz_, p + pos, sz - pos );
	sz += sz_;
	return p + pos;
}

BYTE* _PBlock::exactInsert( size_t pos, size_t sz_ )
{
	PASSERT5( _isValid() );
	PASSERT5( pos <= sz );
	if( sz + sz_ > reserved )
	{
		reserved = sz + sz_;
		p = _realloc( p, reserved );
	}
	memmove( p + pos + sz_, p + pos, sz - pos );
	sz += sz_;
	return p + pos;
}

_PBlock::_PBlock( const _PBlock& other )
{
	PASSERT5( other._isValid() );
	if( other.sz == 0 )
	{
		_init();
	}
	else
	{
		p = (BYTE*)malloc( other.sz );
		if( !p )
			throw PSystemMemoryError( "_PBlock::_PBlock( other ), number of bytes requested", other.sz );
		reserved = sz = other.sz;
		memcpy( p, other.p, sz );
	}
}

_PBlock::_PBlock( size_t sz_ )
{
	if( sz_ == 0 )
	{
		_init();
	}
	else
	{
		p = (BYTE*)malloc( sz_ );
		if( !p )
			throw PSystemMemoryError( "_PBlock::_alloc, number of bytes requested", sz_ );
		reserved = sz = sz_;
	}
}

_PBlock::_PBlock( size_t sz_, size_t reserve_ )
{
	reserve_ += sz_;
	if( reserve_ == 0 )
	{
		_init();
	}
	else
	{
		p = (BYTE*)malloc( reserve_ );
		if( !p )
			throw PSystemMemoryError( "_PBlock::_PBlock(2), number of bytes requested", reserve_ );
		sz = sz_;
		reserved = reserve_;
	}
}

_PBlock& _PBlock::operator=( const _PBlock& other )
{
	PASSERT5( _isValid() );
	PASSERT5( other._isValid() );
	if( &other == this )
		return *this;

	if( other.sz == 0 )
	{
		_del();
		_init();
		return *this;
	}

	if( ! _fitSize( other.sz ) )
	{
		_del();
		p = (BYTE*)malloc( other.sz );
		if( !p )
		{
			_init();
			throw PSystemMemoryError( "_PBlock::=, number of bytes requested", other.sz );
		}
		reserved = other.sz;
	}
	sz = other.sz;
	memcpy( p, other.p, sz );
	return *this;
}

BYTE* _PBlock::alloc( size_t sz_ )
{
	PASSERT5( _isValid() );
	if( sz_ == 0 )
	{
		_del();
		_init();
		return p;
	}

	if( ! _fitSize( sz_ ) )
	{
		_del();
		p = (BYTE*)malloc( sz_ );
		if( !p )
		{
			_init();
			throw PSystemMemoryError( "_PBlock::alloc, number of bytes requested", sz_ );
		}
		reserved = sz_;
	}
	sz = sz_;
	return p;
}

BYTE* _PBlock::alloc( size_t sz_, size_t reserve_ )
{
	PASSERT5( _isValid() );
	reserve_ += sz_;
	_del();
	if( reserve_ == 0 )
	{
		_init();
	}
	else
	{
		p = (BYTE*)malloc( reserve_ );
		if( !p )
		{
			_init();
			throw PSystemMemoryError( "_PBlock::alloc(2), number of bytes requested", reserve_ );
		}
		reserved = reserve_;
		sz = sz_;
	}
	return p;
}

ptrdiff_t _PBlock::shrink()
{
	PASSERT5( _isValid() );
	if( p && ( sz < reserved ) )
	{
		BYTE* const _p( p );
		p = _realloc( p, sz );
		reserved = sz;
		return p - _p;
	}

	return 0;
}

void _PBlock::reserve( size_t reserved_ )
{
	PASSERT5( _isValid() );
	if( reserved < reserved_ )
	{
		p = _realloc( p, reserved_ );
		reserved = reserved_;
	}
}

BYTE* _PBlock::exactAppend( size_t sz_ )
{
	PASSERT5( _isValid() );
	size_t oldSz = sz;
	_exactAppend( sz_ );
	return p + oldSz;
}

void _PBlock::moveFrom( _PBlock& other )
{
	PASSERT5( _isValid() );
	PASSERT5( other._isValid() );
	if( this == &other )
		return;
	_del();
	_moveFrom( other );
}
