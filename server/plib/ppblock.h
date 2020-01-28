#ifndef ppblock_h_included
#define ppblock_h_included

#include "ppincludebase.h"
#include "pperror.h"

#ifdef MOVE_CONSTRUCTOR_SUPPORTED
#include <utility> // Perhaps could be removed when 'Defaulted Functions' are supported by the compiler
#endif

class _PBlock
	//Warning: no destructor
	//Don't use unless you exactly know what are you doing
{
private:
	BYTE* p;
	size_t sz;
	size_t reserved;

public:
	_PBlock()
	{
		_init();
	}
	_PBlock( const _PBlock& other );
	_PBlock( _PBlock& other, bool ) //"moving" constructor
	{
		PASSERT5( other._isValid() );
		_moveFrom( other );
	}
	_PBlock( size_t sz_ );
	_PBlock( size_t sz_, size_t reserve_ );
	_PBlock& operator=( const _PBlock& other );
#ifdef MOVE_CONSTRUCTOR_SUPPORTED
	_PBlock( _PBlock&& other )
	{
		PASSERT5( other._isValid() );
		_moveFrom( other );
	}
	_PBlock& operator=( _PBlock&& other )
	{
		PASSERT5( this != &other );
		moveFrom( other );
		return *this;
	}
#endif
	~_PBlock() {}

public:
	BYTE* alloc( size_t sz_ );
	BYTE* alloc( size_t sz_, size_t reserve_ );
	BYTE* exactAppend( size_t sz_ );
	BYTE* insert( size_t pos, size_t sz_ );
	BYTE* exactInsert( size_t pos, size_t sz_ );
	void moveFrom( _PBlock& other );
	void cut( size_t sz_ )
	{
		PASSERT4( sz_ <= sz );
		sz = sz_;
	}
	ptrdiff_t shrink();
	void reserve( size_t reserved_ );
	void clear()
	{
		_del();
		_init();
	}

public:
	//{ DANGEROUS: don't use unless you exactly know what are you doing
	void _init()
	{
		p = 0;
		sz = 0;
		reserved = 0;
	}
	void _free()
	{
		_del();
	}
	void _rawCopy( _PBlock& other )
	{
		p = other.ptr();
		sz = other.size();
		reserved = other.reserved;
	}
	//} DANGEROUS: don't use unless you exactly know what are you doing

public:
	const BYTE* ptr() const
	{
		return p;
	}
	BYTE* ptr()
	{
		return p;
	}
	size_t size() const
	{
		return sz;
	}

private:
	void _del()
	{
		if( p )
			free( p );
	}
	/* inline */ void _moveFrom( _PBlock& other )
	{
		p = other.p;
		sz = other.sz;
		reserved = other.reserved;
		other._init();
	}

	static const size_t BLOCK_GROW_QUANTUM = 32;
	/* inline */ static size_t _growSize( size_t sz )
	{
		return ( sz <= BLOCK_GROW_QUANTUM ) ? BLOCK_GROW_QUANTUM * 2 : sz * 2;
	}

	/* inline */ static BYTE* _realloc( BYTE* p, size_t sz )
	{
		BYTE* tmp = (BYTE*)realloc( p, sz );
		if( !tmp )
			throw PSystemMemoryError( "_PBlock::_realloc, number of bytes requested", sz );
		return tmp;
	}
	/* inline */ void _append( size_t sz_ )
	{
		sz += sz_;
		if( sz > reserved )
		{
			reserved = _growSize( sz );
			p = _realloc( p, reserved );
		}
	}
	/* inline */ void _exactAppend( size_t sz_ );

	bool _isValid() const { return ( !p == !reserved ) && ( reserved >= sz ); }

protected:
	bool _fitSize( size_t sz_ ) const
	{
		return ( sz_ <= reserved ) && ( ( reserved <= BLOCK_GROW_QUANTUM * 2 ) || ( sz_ >= reserved / 2 ) );
	}
	void _cut( size_t sz_ )
	{
		PASSERT5( sz_ <= reserved );
		sz = sz_;
	}

public:
	BYTE* append( size_t sz_ )
	{
		size_t oldSz = sz;
		_append( sz_ );
		return p + oldSz;
	}

	BYTE* ensure( size_t sz_ )
	{
		if( reserved < sz_ )
			alloc( sz_ );
		else
			sz = sz_;
		return p;
	}
};

class PBlock : public _PBlock
{
public:
	PBlock()
		: _PBlock()
	{
	}
	PBlock( PBlock& other, bool /* true */ )//"moving" constructor
		: _PBlock( other, true )
	{
	}
	PBlock( size_t sz_ )
		: _PBlock( sz_ )
	{
	}
	PBlock( size_t sz_, size_t reserve )
		: _PBlock( sz_, reserve )
	{
	}
#ifdef MOVE_CONSTRUCTOR_SUPPORTED
	PBlock( PBlock&& other )
		: _PBlock( std::move( other ) )
	{
	}
	PBlock& operator=( PBlock&& other )
	{
		moveFrom( other );
		return *this;
	}
	PBlock( const PBlock& other )
		: _PBlock( other )
	{
	}
	PBlock& operator=( const PBlock& other )
	{
		_PBlock::operator=( other );
		return *this;
	}
#endif
	~PBlock()
	{
		_PBlock::_free();
	}

private:
	void _free();
	void _rawCopy( const _PBlock& other );
};

#endif    
