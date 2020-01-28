//composecolls.h
#ifndef composecolls_h_included
#define composecolls_h_included

////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2010 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "pyrtime.h"
#include "ppcontainer.h"

////////////////////////////////////////////////////////////////////////////////
// compose/parse collection (list,vector - anything with resize() ) of objects 
// that implement compose/parse methods as a flat message
//
template <class T>
void composeCollOfComposableValues( const T& v, CommMsgBody& body)
{
	UINT32 v_size = v.size();
	body.composeUINT32( v_size);
	for( typename T::const_iterator it = v.begin(); it !=  v.end(); ++it )
	{
		it->compose( body);
	}
}

template <class T>
void parseCollOfParsableValues( T& v, CommMsgParser& parser)
{
	UINT32 v_size;
	parser.parseUINT32(v_size);
	v.resize(v_size);
	for( typename T::iterator it = v.begin(); it !=  v.end(); ++it )
	{
		// do parsing on the value in-place
		//
		it->parse( parser);
	}
}

////////////////////////////////////////////////////////////////////////////////
// compose/parse vector of objects that implement compose/parse methods
// as a flat message
//
template <class T>
void composeVectorOfComposableValues( const T& v, CommMsgBody& body)
{
	UINT32 v_size = v.size();
	body.composeUINT32( v_size);
	for( UINT32 i = 0; i < v_size; ++i)
	{
		v[i].compose( body);
	}
}

template <class T>
void parseVectorOfParsableValues( T& v, CommMsgParser& parser)
{
	UINT32 v_size;
	parser.parseUINT32(v_size);
	v.resize(v_size);
	for( UINT32 i = 0; i < v_size; ++i)
	{
		// do parsing on the value in-place
		//
		v[i].parse( parser);
	}
}
////////////////////////////////////////////////////////////////////////////////
// compose/parse vector of objects that implement compose/parse methods
// as a flat message
// PYR-40177 versions of the two routines above, which take and pass on a version
// number. This is necessary to avoid default argument to versioned parse() and
// compose().
template <class T>
void composeVectorOfComposableValuesVer( const T& v, CommMsgBody& body, const INT32 verNum)
{
	UINT32 v_size = v.size();
	body.composeUINT32( v_size);
	for( UINT32 i = 0; i < v_size; ++i)
	{
		v[i].compose( body, verNum);
	}
}

template <class T>
void parseVectorOfParsableValuesVer( T& v, CommMsgParser& parser, const INT32 verNum)
{
	UINT32 v_size;
	parser.parseUINT32(v_size);
	v.resize(v_size);
	for( UINT32 i = 0; i < v_size; ++i)
	{
		// do parsing on the value in-place
		//
		v[i].parse( parser, verNum);
	}
}
//////////////////////////////////////////////////////////////////////////
//
template <class T>
void composeComposableObjectAsSeparateMessageBody( T& o, CommMsgBody& body)
{
	CommMsgBody msgBody;
	o.compose( msgBody);
	body.composeMsgBody(msgBody);
}

template <class T>
void parseParsableObjectFromSeparateMessageBody( T& o, CommMsgParser& parser)
{
	CommMsgBody msgBody;
	parser.parseMsgBody(msgBody);
	CommMsgParser msgBodyParser(msgBody);
	o.parse( msgBodyParser);
}

//////////////////////////////////////////////////////////////////////////
// PYR-40177 alternatives to two functions above which take a version number
// and pass it on to the elements' parse() and compose() methods.
template <class T>
void composeComposableObjectAsSeparateMessageBodyVer( T& o, CommMsgBody& body, const INT32 verNum)
{
	CommMsgBody msgBody;
	o.compose( msgBody, verNum );
	body.composeMsgBody(msgBody);
}

template <class T>
void parseParsableObjectFromSeparateMessageBodyVer( T& o, CommMsgParser& parser, const INT32 verNum)
{
	CommMsgBody msgBody;
	parser.parseMsgBody(msgBody);
	CommMsgParser msgBodyParser(msgBody);
	o.parse( msgBodyParser, verNum);
}

////////////////////////////////////////////////////////////////////////////////
// compose/parse vector of objects that implement compose/parse methods
// as a collection of message bodies
//

template <class T>
void composeVectorOfMessageBodiesOfComposableValues( T& v, CommMsgBody& body)
{
	UINT32 v_size = static_cast< UINT32 >( v.size() );
	body.composeUINT32( v_size);
	for( UINT32 i = 0; i < v_size; ++i)
	{
		CommMsgBody msgBody;
		v[i].compose( msgBody);
		body.composeMsgBody(msgBody);
	}
}

template <class T>
void parseVectorOfParsableMessageBodies( T& v, CommMsgParser& parser)
{
	UINT32 v_size;
	parser.parseUINT32(v_size);
	v.resize(v_size);
	for( UINT32 i = 0; i < v_size; ++i)
	{
		CommMsgBody msgBody;
		parser.parseMsgBody(msgBody);
		CommMsgParser msgBodyParser(msgBody);
		v[i].parse( msgBodyParser);
	}
}

////////////////////////////////////////////////////////////////////////////////
// compose/parse vector of objects that implement compose/parse methods
// as a collection of message bodies, passing a version number down to the
// compose/parse methods of the vector elements to determine the format.
// it would be nice to compose the version number into the message and
// discover it on parse, but the unversioned legacy messages would break that.

template <class T>
void composeVectorOfMessageBodiesOfComposableValuesVer( T& v, CommMsgBody& body, const INT32 verNum )
{
	UINT32 v_size = v.size();
	body.composeUINT32( v_size);
	for( UINT32 i = 0; i < v_size; ++i)
	{
		CommMsgBody msgBody;
		v[i].compose( msgBody, verNum);
		body.composeMsgBody(msgBody);
	}
}

template <class T>
void parseVectorOfParsableMessageBodiesVer( T& v, CommMsgParser& parser, const INT32 verNum)
{
	UINT32 v_size;
	parser.parseUINT32(v_size);
	v.resize(v_size);
	
	for( UINT32 i = 0; i < v_size; ++i)
	{
		CommMsgBody msgBody;
		parser.parseMsgBody(msgBody);
		CommMsgParser msgBodyParser(msgBody);
		v[i].parse( msgBodyParser, verNum);
	}
}

////////////////////////////////////////////////////////////////////////////////
// compose/parse vector of message bodies of simple types
//

template<class T, class F>
void composeVectorOfMessageBodies(CommMsgBody& body, const vector<T>& v, const F& composer)
{
	UINT32 sz = v.size();
	body.composeUINT32(sz);
	for (UINT32 i = 0; i < sz; ++i)
	{
		CommMsgBody msgBody;
		composer.compose(msgBody, v[i]);
		body.composeMsgBody(msgBody);
	}
}

template<class T, class F>
void parseVectorOfMessageBodies(CommMsgParser& parser, vector<T>& v, const F& composer)
{
	UINT32 sz;
	parser.parseUINT32(sz);
	v.resize(sz);
	for (UINT32 i = 0; i < sz; ++i)
	{
		CommMsgBody msgBody;
		parser.parseMsgBody(msgBody);
		CommMsgParser msgBodyParser(msgBody);
		composer.parse(msgBodyParser, v[i]);
	}
}

//////////////////////////////////////////////////////////////////////////
// compose/parse implementation general case supporting objects
//
template <typename T>
struct Composer
{
	static void compose(CommMsgBody& body, const T& val)
	{
		val.compose(body);
	}
	static void parse(CommMsgParser& parser, T& val)
	{
		val.parse(parser);
	}
};

// Template specializations for basic types
template<>
struct Composer<PString>
{
	static void compose(CommMsgBody& body, const PString& val)
	{
		body.composeString(val);
	}
	static void parse(CommMsgParser& parser, PString& val)
	{
		parser.parseStringP(val);
	}
};
typedef Composer<PString> ComposerPString;

template<>
struct Composer<const char*>
{
	static void compose( CommMsgBody& body, const char* val )
	{
		body.composeString( val );
	}
	static void parse( CommMsgParser& parser, const char*& val )
	{
		parser.parseString( val );
	}
};
typedef Composer<const char*> ComposerCharPtr;

template<>
struct Composer<INT8>
{
	static void compose(CommMsgBody& body, INT8 val)
	{
		body.composeINT8(val);
	}
	static void parse(CommMsgParser& parser, INT8& val)
	{
		parser.parseINT8(val);
	}
};
typedef Composer<INT8> ComposerINT8;

template<>
struct Composer<UINT16>
{
	static void compose(CommMsgBody& body, UINT16 val)
	{
		body.composeUINT16(val);
	}
	static void parse(CommMsgParser& parser, UINT16& val)
	{
		parser.parseUINT16(val);
	}
};
typedef Composer<UINT16> ComposerUINT16;

template<>
struct Composer<INT16>
{
	static void compose(CommMsgBody& body, INT16 val)
	{
		body.composeINT16(val);
	}
	static void parse(CommMsgParser& parser, INT16& val)
	{
		parser.parseINT16(val);
	}
};
typedef Composer<INT16> ComposerINT16;

template<>
struct Composer<UINT32>
{
	static void compose(CommMsgBody& body, UINT32 val)
	{
		body.composeUINT32(val);
	}
	static void parse(CommMsgParser& parser, UINT32& val)
	{
		parser.parseUINT32(val);
	}
};
typedef Composer<UINT32> ComposerUINT32;

template<>
struct Composer<INT32>
{
	static void compose(CommMsgBody& body, INT32 val)
	{
		body.composeINT32(val);
	}
	static void parse(CommMsgParser& parser, INT32& val)
	{
		parser.parseINT32(val);
	}
};
typedef Composer<INT32> ComposerINT32;

template<>
struct Composer<UINT64>
{
	static void compose(CommMsgBody& body, UINT64 val)
	{
		body.composeUINT64(val);
	}
	static void parse(CommMsgParser& parser, UINT64& val)
	{
		parser.parseUINT64(val);
	}
};
typedef Composer<UINT64> ComposerUINT64;

template<>
struct Composer<INT64>
{
	static void compose(CommMsgBody& body, INT64 val)
	{
		body.composeINT64(val);
	}
	static void parse(CommMsgParser& parser, INT64& val)
	{
		parser.parseINT64(val);
	}
};
typedef Composer<INT64> ComposerINT64;

template<>
struct Composer<bool>
{
	static void compose(CommMsgBody& body, bool val)
	{
		body.composeBOOL(val);
	}
	static void parse(CommMsgParser& parser, bool& val)
	{
		parser.parseBOOL(val);
	}
};
typedef Composer<bool> ComposerBOOL;

template<>
struct Composer<BYTE>
{
	static void compose(CommMsgBody& body, BYTE val)
	{
		body.composeBYTE(val);
	}
	static void parse(CommMsgParser& parser, BYTE& val)
	{
		parser.parseBYTE(val);
	}
};
typedef Composer<BYTE> ComposerBYTE;
typedef Composer<BYTE> ComposerUINT8;

template<>
struct Composer<SrvTime>
{
public:
	static void compose(CommMsgBody& body, const SrvTime& val)
	{
		body.composeSrvTime(val);
	}
	static void parse(CommMsgParser& parser, SrvTime& val)
	{
		parser.parseSrvTime(val);
	}
};
typedef Composer<SrvTime> ComposerSrvTime;

template<>
struct Composer<SrvDate>
{
public:
	static void compose(CommMsgBody& body, const SrvDate& val)
	{
		body.composeSrvDate(val);
	}
	static void parse(CommMsgParser& parser, SrvDate& val)
	{
		parser.parseSrvDate(val);
	}
};
typedef Composer<SrvDate> ComposerSrvDate;

// just a convenient uniform way to define a compose/parse for a PStringSet to avoid including shared.cpp implementation
// this is a composer/parser object and it DOES NOT CLEAR the set before parsing the data into it
//
template<>
struct Composer< PStringSet >
{
public:
	static void compose( CommMsgBody& body, const PStringSet& val )
	{
		body.composeUINT32( static_cast< UINT32 >( val.size() ) );
		for( auto&& iter : val )
		{
			body.composeString( iter );
		}
	}
	static void parse( CommMsgParser& parser, PStringSet& val )
	{
		UINT32 sz;
		parser.parseUINT32( sz );
		for( UINT32 n = 0; n < sz; ++n )
		{
			const char* oneString;
			parser.parseString( oneString );
			val.insert( oneString );
		}
	}
};
typedef Composer<PStringSet> ComposerPStringSet;

template<typename TFirst, typename TSecond>
struct Composer<pair<TFirst,TSecond>>
{
public:
	static void compose(CommMsgBody& body, const pair<TFirst,TSecond>& val)
	{
		Composer<TFirst>::compose(body, val.first);
		Composer<TSecond>::compose(body, val.second);
	}

	static void parse(CommMsgParser& parser, pair<TFirst,TSecond>& val)
	{
		Composer<TFirst>::parse(parser, val.first);
		Composer<TSecond>::parse(parser, val.second);
	}
};
template<typename TFirst,typename TSecond> using ComposerPair = Composer<pair<TFirst,TSecond>>;

///////////////////////////////////////////////////////////////////////////////////
// compose/parse container like vector or list implementing resize() function
//
template <template<class, class> class Container, class T>
void composeVectorOrList( CommMsgBody& body, const Container<T, allocator<T> >& cntr)
{
	body.composeUINT32( static_cast< UINT32 >( cntr.size() ) );

	for ( typename Container<T, allocator<T> >::const_iterator it = cntr.begin(); it != cntr.end(); ++it )
	{
		Composer<T>::compose( body, *it );
	}
}

template <template<class, class> class Container, class T>
void parseVectorOrList( CommMsgParser& parser, Container<T, allocator<T> >& cntr, bool append = false)
{
	INT32 size;
	parser.parseINT32(size);
	if( size == 0 )
	{
		if( !append )
		{
			cntr.clear();
		}

		return;
	}

	Container<T, allocator<T> > tmpCntr;
	tmpCntr.resize(size);

	for ( typename Container<T, allocator<T> >::iterator it = tmpCntr.begin(); it != tmpCntr.end(); ++it )
	{
		Composer<T>::parse( parser, *it );
	}

	if( append )
	{
		copy( tmpCntr.begin(), tmpCntr.end(), back_inserter(cntr) );
	}
	else
	{
		cntr.swap( tmpCntr );
	}
}

// template specialization for BOOL, which should be parsed into temporary variable
template <template<class, class> class Container>
void parseVectorOrList( CommMsgParser& parser, Container<bool, allocator<bool> >& cntr, bool append = false)
{
	INT32 size;
	parser.parseINT32(size);
	if( size == 0 )
	{
		if( !append )
		{
			cntr.clear();
		}

		return;
	}

	Container<bool, allocator<bool> > tmpCntr;
	tmpCntr.resize(size);

	for ( typename Container<bool, allocator<bool> >::iterator it = tmpCntr.begin(); it != tmpCntr.end(); ++it )
	{
		bool tmp;
		Composer<bool>::parse( parser, tmp );
		*it = tmp;
	}

	if( append )
	{
		copy( tmpCntr.begin(), tmpCntr.end(), back_inserter( cntr ) );
	}
	else
	{
		cntr.swap( tmpCntr );
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
// compose/parse (each element as message body) container like vector or list implementing resize() function
//
template <template<class, class> class Container, class T>
void composeVectorOrListAsMsgBodies( CommMsgBody& body, const Container<T, allocator<T> >& cntr )
{
	body.composeINT32( cntr.size() );

	for ( typename Container<T, allocator<T> >::const_iterator it = cntr.begin(); it != cntr.end(); ++it )
	{
		CommMsgBody tmpBody;
		Composer<T>::compose( tmpBody, *it );
		body.composeMsgBody( tmpBody );
	}
}

template <template<class, class> class Container, class T>
void parseVectorOrListAsMsgBodies( CommMsgParser& parser, Container<T, allocator<T> >& cntr, bool append = false)
{
	INT32 size;
	parser.parseINT32(size);
	if( size == 0 )
	{
		if( !append )
		{
			cntr.clear();
		}

		return;
	}

	Container<T, allocator<T> > tmpCntr;
	tmpCntr.resize(size);

	for ( typename Container<T, allocator<T> >::iterator it = tmpCntr.begin(); it != tmpCntr.end(); ++it )
	{
		CommMsgBody tmpBody;
		parser.parseMsgBody( tmpBody );

		CommMsgParser tmpParser( tmpBody );
		Composer<T>::parse( tmpParser, *it );
	}

	if( append )
	{
		copy( tmpCntr.begin(), tmpCntr.end(), back_inserter( cntr ) );
	}
	else
	{
		cntr.swap( tmpCntr );
	}
}

// template specialization for BOOL, which should be parsed into temporary variable
template <template<class, class> class Container>
void parseVectorOrListAsMsgBodies( CommMsgParser& parser, Container<bool, allocator<bool> >& cntr, bool append = false)
{
	INT32 size;
	parser.parseINT32(size);
	if( size == 0 )
	{
		if( !append )
		{
			cntr.clear();
		}

		return;
	}

	Container<bool, allocator<bool> > tmpCntr;
	tmpCntr.resize(size);

	for ( typename Container<bool, allocator<bool> >::iterator it = tmpCntr.begin(); it != tmpCntr.end(); ++it )
	{
		CommMsgBody tmpBody;
		parser.parseMsgBody( tmpBody );

		CommMsgParser tmpParser( tmpBody );
		bool tmp;
		tmpParser.parseBOOL( tmp );
		*it = tmp;
	}

	if( append )
	{
		copy( tmpCntr.begin(), tmpCntr.end(), back_inserter( cntr ) );
	}
	else
	{
		cntr.swap( tmpCntr );
	}
}

////////////////////////////////////////////////////////////////////////////
// two classes implementing chain compose/parse for vector or list
//
class SrvCommMsgBody : public CommMsgBody
{
public:
	template <template<class, class> class Container, class T>
	CommMsgBody& composeVectorOrList( const Container<T, allocator<T>>& cntr )
	{
		::composeVectorOrList( *this, cntr );
		return *this;
	}
};

class SrvCommMsgParser : public CommMsgParser
{
public:
	SrvCommMsgParser( const CommMsgBody& msgBody ) : CommMsgParser( msgBody ) {}

	template <template<class, class> class Container, class T>
	CommMsgParser& parseVectorOrList( Container<T, allocator<T>>& cntr )
	{
		::parseVectorOrList( *this, cntr );
		return *this;
	}
};

//////////////////////////////////////////////////////////////////////////
// compose/parse vectors of simple objects that do not implement compose/parse
// using composer objects defined above
//
template<class T, class F>
UINT32 composeVector( CommMsgBody& body, const vector<T>& v, const F& composer)
{
	UINT32 sz = static_cast< UINT32 >( v.size() );
	body.composeUINT32(sz);
	for (UINT32 n = 0; n < sz; ++n )
	{
		composer.compose(body, v[n]);
	}
	return sz;
}

template<class T, class F>
UINT32 parseVectorWithOverride( CommMsgParser& parser, vector<T>& v, const F& composer)
{
	UINT32 sz;
	parser.parseUINT32(sz);
	v.resize(sz);
	for (UINT32 n = 0; n < sz; ++n )
	{
		composer.parse(parser, v[n]);
	}
	return sz;
}

// PYR-52366
template<class T, class F>
UINT32 parseVectorWithOverrideAndInitSize( CommMsgParser& parser, vector<T>& v, const F& composer, UINT32 initSize)
{
	UINT32 sz;
	parser.parseUINT32(sz);
	v.clear();
	v.reserve(min<UINT32>(sz, initSize));

	for (UINT32 n = 0; n < sz; ++n )
	{
		v.push_back(T());
		composer.parse(parser, v.back());
	}

	return sz;
}

// PYR-52366
template<class T, class F>
UINT32 parseVectorOfMessageBodiesWithOverrideAndInitSize( CommMsgParser& parser, vector<T>& v, const F& composer, UINT32 initSize)
{
	UINT32 sz;
	parser.parseUINT32(sz);
	v.clear();
	v.reserve(min<UINT32>(sz, initSize));

	for (UINT32 n = 0; n < sz; ++n )
	{
		v.push_back(T());
		CommMsgBody msgBody;
		parser.parseMsgBody(msgBody);
		CommMsgParser msgBodyParser(msgBody);
		composer.parse(msgBodyParser, v.back());
	}

	return sz;
}

// bool vectors are packed as bit values and have to be parsed through a temp value
// create a partially specialized template for them
template<class F>
UINT32 parseVectorWithOverride( CommMsgParser& parser, vector<bool>& v, const F& composer)
{
	UINT32 sz;
	parser.parseUINT32(sz);
	v.resize(sz);
	for (UINT32 n = 0; n < sz; ++n )
	{
		bool vTmp;
		composer.parse(parser, vTmp);
		v[n]=vTmp;
	}
	return sz;
}

// compose/parse sets of simple objects that do not implement compose/parse
template<class T, class F>
UINT32 composeSet( CommMsgBody& body, const set<T>& s, const F& composer)
{
	UINT32 sz = s.size();
	body.composeUINT32(sz);
	for ( typename set<T>::const_iterator it = s.begin(); it != s.end(); ++it )
	{
		composer.compose(body, *it);
	}
	return sz;
}

template<class T, class F>
void parseSetWithOverride( CommMsgParser& parser, set<T>& s, const F& composer)
{
	s.clear();
	UINT32 sz;
	parser.parseUINT32(sz);
	for (UINT32 n = 0; n < sz; ++n )
	{
		T val;
		composer.parse(parser, val);
		s.insert(val);
	}
}

//////////////////////////////////////////////////////////////////////////
// compose/parse for PStringMap/std::map

template <class keyT, class valueT, class Compare>
void composeMap(CommMsgBody& body, const map<keyT, valueT, Compare>& map)
{
	body.composeUINT32(static_cast<UINT32>(map.size()));

	for (const auto& kv : map)
	{
		Composer<keyT>::compose(body, kv.first);
		Composer<valueT>::compose(body, kv.second);
	}
}

template <class keyT, class valueT, class Compare>
void parseMapWithOverride(CommMsgParser& parser, map<keyT, valueT, Compare>& map)
{
	map.clear();
	UINT32 sz;
	parser.parseUINT32(sz);
	for (UINT32 n = 0; n < sz; ++n)
	{
		keyT key;
		Composer<keyT>::parse(parser, key);
		Composer<valueT>::parse(parser, map[key]);
	}
}

template <class T>
void composePStringMap(CommMsgBody& body, const PStringMap<T>& map)
{
	body.composeUINT32(static_cast<UINT32>(map.size()));

	for (auto it = map.begin(); it != map.end(); ++it)
	{
		Composer<PString>::compose(body, it->first); // key
		Composer<T>::compose(body, it->second);      // value
	}
}

template <class T>
void parsePStringMapWithOverride(CommMsgParser& parser, PStringMap<T>& map)
{
	map.clear();
	UINT32 sz;
	parser.parseUINT32(sz);
	for (UINT32 n = 0; n < sz; ++n)
	{
		PString key;
		T value;
		Composer<PString>::parse(parser, key);
		Composer<T>::parse(parser, value);
		map.insert(make_pair(key, value));
	}
}

//////////////////////////////////////////////////////////////////////////
// compose/parse sets of simple objects that implement compose/parse

template<class T>
void composeSetOfComposableObjects( CommMsgBody& body, const set<T>& s )
{
	UINT32 sz = static_cast< UINT32 >( s.size() );
	body.composeUINT32(sz);
	for ( typename set<T>::const_iterator it = s.begin(); it != s.end(); ++it )
	{
		it->compose(body);
	}
}

// works with sets/multisets, sets/mulisets with custom comparison
template<typename C>
void parseSetOfParsableObjects( CommMsgParser& parser, C& s)
{
	s.clear();
	UINT32 sz;
	parser.parseUINT32(sz);
	for (UINT32 n = 0; n < sz; ++n )
	{
		typename C::value_type val;
		val.parse(parser);
		s.insert(val);
	}
}

//////////////////////////////////////////////////////////////////////////
// support for compose/parse of enums as int types
//
template <typename EnumType, typename IntType>
class ComposerEnumAsIntType
{
public:
	static void compose(CommMsgBody& body, EnumType val)
	{
		Composer<IntType>::compose(body, static_cast<IntType>(val)); //lint !e571
	}
	static void parse(CommMsgParser& parser, EnumType& val)
	{
		IntType intVal;
		Composer<IntType>::parse(parser, intVal);
		val = static_cast<EnumType>(intVal);
	}
};

// BYTE
template <class EnumType>
class ComposerEnumAsBYTE : public ComposerEnumAsIntType<EnumType, BYTE>
{};

template <class EnumType>
void parseEnumAsBYTE(CommMsgParser& parser, EnumType& val)
{
	ComposerEnumAsBYTE<EnumType>::parse(parser, val);
}

template <class EnumType>
void composeEnumAsBYTE(CommMsgBody& body, EnumType val)
{
	ComposerEnumAsBYTE<EnumType>::compose(body, val);
}

// UINT16
template <class EnumType>
class ComposerEnumAsUINT16 : public ComposerEnumAsIntType<EnumType, UINT16>
{};

template <class EnumType>
void parseEnumAsUINT16(CommMsgParser& parser, EnumType& val)
{
	ComposerEnumAsUINT16<EnumType>::parse(parser, val);
}

template <class EnumType>
void composeEnumAsUINT16(CommMsgBody& body, EnumType val)
{
	ComposerEnumAsUINT16<EnumType>::compose(body, val);
}

// UINT32
template <class EnumType>
class ComposerEnumAsUINT32 : public ComposerEnumAsIntType<EnumType, UINT32>
{};

template <class EnumType>
void parseEnumAsUINT32(CommMsgParser& parser, EnumType& val)
{
	ComposerEnumAsUINT32<EnumType>::parse(parser, val);
}

template <class EnumType>
void composeEnumAsUINT32(CommMsgBody& body, EnumType val)
{
	ComposerEnumAsUINT32<EnumType>::compose(body, val);
}

// UINT64
template <class EnumType>
class ComposerEnumAsUINT64 : public ComposerEnumAsIntType<EnumType, UINT64>
{};

template <class EnumType>
void parseEnumAsUINT64(CommMsgParser& parser, EnumType& val)
{
	ComposerEnumAsUINT64<EnumType>::parse(parser, val);
}

template <class EnumType>
void composeEnumAsUINT64(CommMsgBody& body, EnumType val)
{
	ComposerEnumAsUINT64<EnumType>::compose(body, val);
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// support for conditional parses
//
inline void parseINT8IfNotParseEnded(CommMsgParser& parser, INT8& val, const INT8 defaultVal)
{
	if (parser.parseEnded())
	{
		val = defaultVal;
		return;
	}
	parser.parseINT8(val);
}

inline void parseBYTEIfNotParseEnded(CommMsgParser& parser, BYTE& val, const BYTE defaultVal)
{
	if (parser.parseEnded())
	{
		val = defaultVal;
		return;
	}
	parser.parseBYTE(val);
}

inline void parseBOOLIfNotParseEnded(CommMsgParser& parser, bool& val, const bool defaultVal)
{
	if (parser.parseEnded())
	{
		val = defaultVal;
		return;
	}
	parser.parseBOOL(val);
}

inline void parseINT16IfNotParseEnded(CommMsgParser& parser, INT16& val, const INT16 defaultVal)
{
	if (parser.parseEnded())
	{
		val = defaultVal;
		return;
	}
	parser.parseINT16(val);
}

inline void parseUINT16IfNotParseEnded(CommMsgParser& parser, UINT16& val, const UINT16 defaultVal)
{
	if (parser.parseEnded())
	{
		val = defaultVal;
		return;
	}
	parser.parseUINT16(val);
}

inline void parseINT32IfNotParseEnded(CommMsgParser& parser, INT32& val, const INT32 defaultVal)
{
	if (parser.parseEnded())
	{
		val = defaultVal;
		return;
	}
	parser.parseINT32(val);
}

inline void parseUINT32IfNotParseEnded(CommMsgParser& parser, UINT32& val, const UINT32 defaultVal)
{
	if (parser.parseEnded())
	{
		val = defaultVal;
		return;
	}
	parser.parseUINT32(val);
}

inline void parseINT64IfNotParseEnded(CommMsgParser& parser, INT64& val, const INT64 defaultVal)
{
	if (parser.parseEnded())
	{
		val = defaultVal;
		return;
	}
	parser.parseINT64(val);
}

inline void parseUINT64IfNotParseEnded(CommMsgParser& parser, UINT64& val, const UINT64 defaultVal)
{
	if (parser.parseEnded())
	{
		val = defaultVal;
		return;
	}
	parser.parseUINT64(val);
}

inline void parseStringIfNotParseEnded(CommMsgParser& parser, PString& val, const char* defaultVal)
{
	if (parser.parseEnded())
	{
		val = defaultVal;
		return;
	}
	parser.parseStringP(val);
}

inline void parseSrvTimeIfNotParseEnded(CommMsgParser& parser, CommSrvTime& val, const CommSrvTime& defaultVal)
{
	if (parser.parseEnded())
	{
		val = defaultVal;
		return;
	}
	parser.parseSrvTime(val);
}

inline void parseSrvDateIfNotParseEnded(CommMsgParser& parser, CommSrvDate& val, const CommSrvDate& defaultVal)
{
	if (parser.parseEnded())
	{
		val = defaultVal;
		return;
	}
	parser.parseSrvDate(val);
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// support for compose/parse of enums as UINT32
//
// OBSOLETE - use ComposerEnumAsUINT32 instead
template <class EnumType_>
class ComposerENUM32asUINT32
{
public:
	static void compose(CommMsgBody& body, const EnumType_& val)
	{
		body.composeUINT32(val);
	}
	static void parse(CommMsgParser& parser, EnumType_& val)
	{
		UINT32 tmpVal;
		parser.parseUINT32(tmpVal);
		val = static_cast<EnumType_>(tmpVal);
	}
};

// OBSOLETE - use parseEnumAsUINT32 instead
template <class EnumType_>
void parseEnum32asUINT32(CommMsgParser& parser, EnumType_& val)
{
	ComposerENUM32asUINT32<EnumType_>().parse(parser, val);
}

// OBSOLETE - use composeEnumAsUINT32 instead
template <class EnumType_>
void composeEnum32asUINT32(CommMsgBody& body, EnumType_& val)
{
	ComposerENUM32asUINT32<EnumType_>().compose(body, val);
}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// A convenient form to encapsulate shift, body and update function
// to help write fewer lines
// Uses a private reference to a body
//
class VectorRefSizeUpdater
{
	size_t shift;
	CommMsgBody& body_ref;		
public:
	VectorRefSizeUpdater( CommMsgBody& body_ ) : shift(0), body_ref( body_ )
	{
		body_ref.composeVectorSize( 0, shift );
	}
	CommMsgBody& updateVectorSize( INT32 count )
	{
		body_ref.updateVectorSize( count, shift );
		return body_ref;
	}
};
// Uses a private instance of a message body accessible as a public member
class VectorSizeUpdaterWithBody
{
	size_t shift;
public:
	CommMsgBody body;
public:
	VectorSizeUpdaterWithBody() : shift(0)
	{
		body.composeVectorSize( 0, shift );
	}
	CommMsgBody& updateVectorSize( INT32 count )
	{
		body.updateVectorSize( count, shift );
		return body;
	}
};
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// compose/parse object as body
template< class T >
void composeObjectAsBody( CommMsgBody& body, const T& obj )
{
	CommMsgBody objBody;
	obj.compose(objBody);
	body.composeMsgBody(objBody);
}

template< class T >
void parseObjectAsBody( CommMsgParser& parser, T& obj )
{
	CommMsgBody objBody;
	parser.parseMsgBody(objBody);
	CommMsgParser objParser(objBody);
	obj.parse(objParser);
}
//////////////////////////////////////////////////////////////////////////

#endif // composecolls_h_included
