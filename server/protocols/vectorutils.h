// vectorutils.h 
//////////////////////////////////////////////////////////////////////////

#ifndef vectorutils_h_included
#define vectorutils_h_included

#include "ppinclude.h"
#include "ppstrtok.h"

//////////////////////////////////////////////////////////////////////////

// combined PStringTokenizer and PStringTokenizer::iterator implementation from ppstrtok.h
// with additional functionality of providing information about most recently found right separator
//
template< class Separator > class PStringTokenizerIterator
{
private:
	typedef typename Separator::CharType CharType;
	typedef typename Separator::ValueType ValueType;

	const CharType*	str;
	const Separator	separator;
	const bool		skipEmptyTokens;
	CharType		lastRightSeparator;

	const CharType* p0;
	const CharType* p;

public:
	PStringTokenizerIterator( const CharType* s, ValueType separator_, bool skipEmptTokens_ = true )
		: str( s ), separator( separator_ ), skipEmptyTokens( skipEmptTokens_ ), lastRightSeparator( 0 ), p0( NULL ), p( s )
	{
	}

private:

	void _iterate()
	{
		if( p )
		{
			if( skipEmptyTokens )
			{
				while( *p && separator.isSeparator( *p ) )
					++p;
			}

			if( *p )
			{
				if( skipEmptyTokens )
					p0 = p++;
				else
					p0 = !p0 ? p : ++p;

				while( *p )
				{
					if( separator.isSeparator( *p ) )
					{
						lastRightSeparator = *p;
						break;
					}
					++p;
				}
			}
			else
			{
				p0 = p = NULL;
			}
		}
	}

public:
	operator bool() const
	{
		return p0 != NULL;
	}
	bool nextToken()
	{
		_iterate();
		return operator bool();
	}
	const CharType* tokenBegin() const
	{
		return p0;
	}
	const CharType* tokenEnd() const
	{
		return p;
	}
	size_t tokenLength() const
	{
		return p - p0;
	}
	const CharType getLastRightSeparator() const
	{
		return lastRightSeparator;
	}
	bool isLastToken() const
	{
		return !p || *p == 0;
	}
};

//////////////////////////////////////////////////////////////////////////
// basic 'find in vector' functionality
class VectorUtils
{
public:
	template<class T, class C>
	static int find(const vector<T>& v, const C& c)
	{
		for( int idx = 0; idx < v.size(); ++idx)
		{
			if( c(v[idx]))
				return idx;
		}
		return -1;
	}
	template <class T, class C>
	static int addIfNotFound(vector<T>& v, const C& c)
	{
		int idx = find(v, c);
		if( idx < 0)
		{
			v.push_back(c.getVal());
			idx = (v.size() - 1);
		}
		return idx;
	}
	template <class T>
	class FindVal
	{
		T val;
	public:
		FindVal(const T val_) : val(val_){}
		bool operator()(const T& v) const 
		{ 
			return v == val;
		}
		const T& getVal() const { return val;}
	};
	class FindPString
	{
		PString str;
	public:
		FindPString(const char* str_) : str(str_){}
		bool operator()(const char* v) const 
		{ 
			return str.equals(v);
		}
		const char* getVal() const { return str;}
	};
	class FindPStringIgnoreCase
	{
		PString str;
	public:
		FindPStringIgnoreCase(const char* str_) : str(str_){}
		bool operator()(const char* v) const 
		{ 
			return (PString::compareIgnoreCase(v, str) == 0);
		}
		const char* getVal() const { return str;}
	};
	// check if str argument starts with any of the vector element
	// ignore case
	class FindPStringStartMaskIgnoreCase
	{
		PString str;
	public:
		FindPStringStartMaskIgnoreCase(const char* str_) : str(str_){}
		bool operator()(const char* v) const 
		{ 
			return (PString::compareIgnoreCase(v, str, PString::length(v)) == 0);
		}
		const char* getVal() const { return str;}
	};
	template<class T>
	class FindPair
	{
		T val;
	public:
		FindPair(const T& val_) : val(val_){}
		bool operator()(const T& v) const 
		{ 
			return (v.first == val.first && v.second == val.second);
		}
		const T& getVal() const { return val;}
	};
	template<class F, class T>
	static void diffVectors(const vector<T>& v1, const vector<T>& v2, vector<T>& vdiff)
	{
		const vector<T>& vlong	= (v1.size() > v2.size() ? v1 : v2);
		const vector<T>& vshort	= (&vlong == &v1 ? v2 : v1);
		diffVectorsPrimSec< F, T >( vlong, vshort, vdiff );
	}
	// subtracts vSecondary from vPrimary as ABCDEF - CDEXYZ = ABF
	template<class F, class T>
	static void diffVectorsPrimSec(const vector<T>& vPrimary, const vector<T>& vSecondary, vector<T>& vdiff)
	{
		for(int i = 0; i < vPrimary.size(); ++i)
		{
			if(0 > VectorUtils::find(vSecondary, F(vPrimary[i])))
			{
				vdiff.push_back(vPrimary[i]);
			}
		}
	}
};
//////////////////////////////////////////////////////////////////////////
class VectorStringConvert
{
public:
	template<class T, class C>
	static const char* vectorToString(const vector<T>& v, const C& c, PString& s, const char* delimiter)
	{
		s = "";
		for(int i = 0; i < v.size(); ++i)
		{
			if(i > 0)
				s.append(delimiter);
			PString converted;
			s.append(c.toString(v[i], converted));
		}
		return s;
	}
	template<class T_it,  class C>
	static const char*  collToString( const T_it& ifirst_, const T_it& ilast, const C& c, PString& s, const char* delimiter)
	{ 
		s = "";
		for( T_it ifirst = ifirst_; ifirst != ilast; ++ifirst)
		{
			if(ifirst_ != ifirst)
				s.append(delimiter);
			PString converted;
			s.append(c.toString(*ifirst, converted));
		}
		return s;
	}
	template<class T_it,  class C>
	static const char*  collToStringWithPrefix( const char* prefix, const T_it& ifirst_, const T_it& ilast, const C& c, PString& s, const char* delimiter, const char* postfix)
	{ 
		s = prefix;
		for( T_it ifirst = ifirst_; ifirst != ilast; ++ifirst)
		{
			if(ifirst_ != ifirst)
				s.append(delimiter);
			PString converted;
			s.append(c.toString(*ifirst, converted));
		}
		s.append( postfix );
		return s;
	}
	class UINT32ToString
	{
	public:
		const char* toString(const UINT32 v, PString& s) const 
		{ 
			s.appendUint(v);
			return s;
		}
	};
	class INT32ToString
	{
	public:
		const char* toString(const INT32 v, PString& s) const 
		{ 
			s.appendInt(v);
			return s;
		}
	};
	class INT64toString
	{
	public:
		const char* toString(const INT64 v, PString& s) const 
		{ 
			s.appendInt64(v);
			return s;
		}
	};
	class UINT64toString
	{
	public:
		const char* toString(const INT64 v, PString& s) const
		{
			s.appendUint64(v);
			return s;
		}
	};
	class StringToString
	{
	public:
		const char* toString(const PString& v, PString& s) const 
		{ 
			s.append(v);
			return s;
		}
	};
};

//////////////////////////////////////////////////////////////////////////
class StringVectorConvert
{
public:
	template<class T>
	static void toNumVector(const char* str, vector<T>& v, const char* delimiter = ",", const char* empty = "-", bool append = false)
	{ 
		toVector(str, v, Conv2i<T>(), delimiter, empty, append); 
	}
	static void toNumVector(const char* str, vector<INT64>& v, const char* delimiter = ",", const char* empty = "-", bool append = false)
	{ 
		toVector(str, v, Conv2i64<INT64>(), delimiter, empty, append); 
	}
	static void toNumVector(const char* str, vector<UINT64>& v, const char* delimiter = ",", const char* empty = "-", bool append = false)
	{ 
		toVector(str, v, Conv2u64<UINT64>(), delimiter, empty, append); 
	}
	static void toBoolVector(const char* str, vector<bool>& v, const char* delimiter = ",", const char* empty = "-", bool append = false)
	{ 
		toVector(str, v, Conv2b(), delimiter, empty, append); 
	}
	static void toYNBoolVector( const char* str, vector<bool>& v, const char* delimiter = ",", const char* empty = "-", bool append = false )
	{
		toVector( str, v, Conv2ynb(), delimiter, empty, append );
	}
	static void toStringVector(const char* str, vector<PString>& v, const char* delimiter = ",", const char* empty = "-", bool append = false)
	{ 
		toVector(str, v, Conv2s(), delimiter, empty, append); 
	}
	// uses a completely different tokenizer
	static void toStringVectorWithEscape(const char* str, vector<PString>& v, const char* delimiter = ",", const char* escape = "\\", bool append = false)
	{ 
		toVectorWithEscape(str, v, Conv2s(), delimiter, escape, append); 
	}
	// copied from shared.cpp - this seems like a good place for it
	// TODO: later will replace calls to splitStringIntoChunks in the rest of the code base to this one
	//
	static void splitStringIntoChunks( const char* str, const INT32 chunkSize, vector<PString>& chunks )
	{
		if( !str || !*str || !chunkSize)
		{
			return;
		}
		const INT32 totalLength	= PString::length(str);
		const char* endPtr		= str + totalLength;
		const char* from		= str;

		while(from != endPtr)
		{
			const char* to = ( from + chunkSize > endPtr ) ? endPtr : ( from + chunkSize );
			PString chunk( from, to );
			chunks.push_back(chunk);
			from = to;
		}
	}
private:
	template<class T, class C>
	static void toVector(const char* str, vector<T>& v, const C& conv, const char* delimiter, const char* empty, bool append)
	{
		if( !append)
		{
			v.clear();
		}
		if( str == NULL || *str == '\0')
		{
			return;
		}
		PString nextTok;
		PStrTok strTok(str, delimiter);
		if (PStrTok::iterator it = strTok.first())
		{
			do 
			{
				nextTok.assign(it.begin(), it.end());
				if(nextTok.equals(empty))
				{
					v.push_back(conv.getEmpty());
				}
				else
				{
					v.push_back(conv.convert(nextTok));
				}
			} 
			while (++it);
		}
	}
	// uses a completely different tokenizer
	template<class T, class C>
	static void toVectorWithEscape( const char* str, vector<T>& v, const C& conv, const char* delimiter, const char* escape, bool append )
	{
		if( !append)
		{
			v.clear();
		}
		if( str == NULL || *str == '\0')
		{
			return;
		}
		const INT32 escLen = escape ? strlen( escape ) : 0;

		PStringTokenizerIterator<_PTokenSeparator> tokenizer( str, delimiter );
		
		PString accumTok;
		accumTok.reserve( strlen( str ) );
		while ( tokenizer.nextToken() )
		{
			accumTok.append( tokenizer.tokenBegin(), tokenizer.tokenEnd() );
		
			if( accumTok.endsWith( escape ) )
			{
				if( !tokenizer.isLastToken() )
				{
					accumTok.cut( accumTok.length() - escLen );
					if( !accumTok.endsWith( escape ) )
					{
						accumTok.append( delimiter );
						continue;
					}
				}
			}
			v.push_back( conv.convert( accumTok ) );
			accumTok.cut(0);
		} 
	}
	class Conv2s 
	{ 
	public: 
		const char* convert(const char* v) const { return v;}
		const char* getEmpty() const { return "";}
	};
	template<class T>
	class Conv2i 
	{ 
	public: 
		T convert(const char* v) const { return atoi(v);	}
		T getEmpty() const { return 0;}
	};
	template<class T>
	class Conv2i64
	{ 
	public: 
		T convert(const char* v) const { return p_atoi64(v);	}
		T getEmpty() const { return 0;}
	};
	template<class T>
	class Conv2u64
	{ 
	public: 
		T convert(const char* v) const { return p_atou64(v);	}
		T getEmpty() const { return 0;}
	};
	class Conv2b 
	{ 
	public: 
		bool convert(const char* v) const { return atoi(v) != 0;	}
		bool getEmpty() const { return false;}
	};
	class Conv2ynb
	{
	public:
		bool convert( const char* value_ ) const 
		{
			PString value( PString::ltrim( value_ ), PString::rtrim( value_ ) );
			if( PString::compareIgnoreCase( value, "yes" ) == 0 || PString::compareIgnoreCase( value, "true" ) == 0 || PString::compareIgnoreCase( value, "1" ) == 0 )
			{
				return true;
			}
			else if( PString::compareIgnoreCase( value, "no" ) == 0 || PString::compareIgnoreCase( value, "false" ) == 0 || PString::compareIgnoreCase( value, "0" ) == 0 )
			{
				return false;
			}
			else
			{
				throw PError( "Invalid boolean value" );
			}
		}
		bool getEmpty() const { return false; }
	};
};

template< typename T, size_t sz >
std::vector<T> makeVector( const T (&arr)[sz] )
{
	return std::vector<T>(arr, arr + sz);
}

#endif // vectorutils_h_included

