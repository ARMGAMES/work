#ifndef ppstrtok_h_included
#define ppstrtok_h_included

#include "ppinclude.h"

template< class Separator > class PStringTokenizer
{
private:
	typedef typename Separator::CharType CharType;
	typedef typename Separator::ValueType ValueType;
	typedef typename Separator::Ptr SeparatorPtr;

private:
	const CharType* str;
	const Separator separator;

private:
	const bool skipEmptyTokens;

public:
	PStringTokenizer( const CharType* s, ValueType separator_, bool skipEmptTokens_ = true )
		: str( s ), separator( separator_ ), skipEmptyTokens( skipEmptTokens_ )
	{
	}

public:
	class iterator
	{
		friend class PStringTokenizer< Separator >;

	private:
		const SeparatorPtr separator;

	private:
		const CharType* p0;
		const CharType* p;

	private:
		const bool skipEmptyTokens;

	private:
		void _iterate()
		{
			if( p )
			{
				if( skipEmptyTokens )
				{
					while( *p && separator->isSeparator( *p ) )
						++p;
				}

				if( *p )
				{
					if( skipEmptyTokens )
						p0 = p++;
					else
						p0 = !p0 ? p : ++p;
					
					while( *p && !separator->isSeparator( *p ) )
						++p;
				}
				else
				{
					p0 = p = NULL;
				}
			}
		}

	private:
		iterator( const Separator& separator_, const CharType* s, bool skipEmptyTokens_ )
			: separator( separator_ ), p0( NULL ), p( s ), skipEmptyTokens( skipEmptyTokens_ )
		{
			_iterate();
		}

	public:
		iterator( const iterator& other )
			: separator( other.separator ), p0( other.p0 ), p( other.p ), skipEmptyTokens( other.skipEmptyTokens )
		{
		}

	public:
		operator bool() const
		{
			return p0 != NULL;
		}
		iterator& operator++()
		{
			_iterate();
			return *this;
		}
		iterator operator++( int )
		{
			iterator iter( *this );
			_iterate();
			return iter;
		}

	public:
		const CharType* begin() const
		{
			return p0;
		}
		const CharType* end() const
		{
			return p;
		}
		size_t length() const
		{
			return p - p0;
		}

	public:
		bool isLast() const
		{
			return !p || *p == 0;
		}

	private: // prohibit default assignment operator
		iterator& operator=( const iterator& other );
	};

public:
	iterator first() const
	{
		return iterator( separator, str, skipEmptyTokens );
	}

private: // prohibit default assignment operator
	PStringTokenizer& operator=( const PStringTokenizer& other );
};

template< class C > class PSimpleTokenSeparator
{
	C separator;

public:
	typedef C CharType;
	typedef C ValueType;

public:
	typedef PSimpleTokenSeparator Ptr;

public:
	PSimpleTokenSeparator( C separator_ )
		: separator( separator_ )
	{
	}
	PSimpleTokenSeparator( const PSimpleTokenSeparator& other )
		: separator( other.separator )
	{
	}

public:
	const PSimpleTokenSeparator* operator->() const
	{
		return this;
	}

public:
	bool isSeparator( C c ) const
	{
		return c == separator;
	}
};

template< class C > class PTokenSeparator
{
	const C* separator;

public:
	typedef C CharType;
	typedef const C* ValueType;

public:
	typedef PTokenSeparator Ptr;

public:
	PTokenSeparator( const C* separator_ )
		: separator( separator_ )
	{
	}
	PTokenSeparator( const PTokenSeparator& other )
		: separator( other.separator )
	{
	}

public:
	const PTokenSeparator* operator->() const
	{
		return this;
	}

public:
	bool isSeparator( C c ) const
	{
		for( const C* sep = separator; *sep; ++sep )
		{
			if( *sep == c )
				return true;
		}
		return false;
	}
};

class _PTokenSeparator
{
	UINT32 separator[ 0x8 ];

public:
	typedef char CharType;
	typedef const char* ValueType;

public:
	class Ptr
	{
		const _PTokenSeparator* ptr;

	public:
		Ptr( const _PTokenSeparator& r )
			: ptr( &r )
		{
		}
		Ptr( const Ptr& other )
			: ptr( other.ptr )
		{
		}

	public:
		const _PTokenSeparator* operator->() const
		{
			return ptr;
		}
	};

public:
	_PTokenSeparator( const char* separator_ )
	{
		::memset( separator, 0, sizeof(separator) );
		while( *separator_ )
		{
			separator[ (unsigned char)*separator_ >> 5 ] |= 0x1 << ( *separator_ & 0x1f );
			++separator_;
		}
	}

public:
	bool isSeparator( char c ) const
	{
		return ( separator[ (unsigned char)c >> 5 ] & ( 0x1 << ( c & 0x1f ) ) ) != 0;
	}
};

typedef PStringTokenizer< PSimpleTokenSeparator< char > > _PStrTok;
typedef PStringTokenizer< _PTokenSeparator > PStrTok;

#endif
