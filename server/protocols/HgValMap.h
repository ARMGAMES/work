#ifndef HgValMap_h_included
#define HgValMap_h_included

////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2010 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////

#include "ppinclude.h"

//////////////////////////////////////////////////////////////////////////
//
// Templates for classes containing a map of values and implementing find/update/remove value
// interface. saves time when only a limited number of usage scenarios is required, makes code
// simple and shorter by eliminating the need to re-implement the same find/add/update/clean
// for different types of trivial maps
// 
// Also implements begin/end/erase methods which allows to use this class as if it were
// a normal map - for enumeration.
//
// ValMap class implements all that basic machinery and is used as a base
// by two pointer containers:
//
// PtrMapOwner class allows creating map containing pointers owned by the map 
// i.e. when a pointer is removed from the map it will be cleaned up.
// PtrMapIndex class does not own pointers stored in it.
//
// For an example on how to use all of the above - scroll down to "USAGE EXAMPLES:"
//
// getValues returns an actual map object encapsulated within the ValueMap - this can be useful
// if it is needed to pass an actual map as a parameter to a function that does not understand
// this higher level abstraction.
//
// extractAndRemove returns a value removed from the map but does not destroy it
// as it's no longer owned by the map
//
// I had to add the TData parameter because if I use PStringMap I have no access to T::data_type
// due to a private nature of inheritance
//
//
//////////////////////////////////////////////////////////////////////////

// T_rev_iter and T_const_rev_iter are defined for cases where TMap does not support reverse iterators
// like unordered_map in which case both parameters can be specified as void
// ValMap< unordered_map<>, void, void >
//
template < class TMap, class T_rev_iter = typename TMap::reverse_iterator, class T_const_rev_iter = typename TMap::const_reverse_iterator >
class ValMap
{

public:
	typedef TYPENAME TMap::iterator			iterator;
	typedef TYPENAME TMap::const_iterator	const_iterator;

	typedef TYPENAME T_rev_iter				reverse_iterator;
	typedef TYPENAME T_const_rev_iter		const_reverse_iterator;

protected:
	typedef TMap map_type;
	typedef TYPENAME TMap::mapped_type data_type;
	typedef TYPENAME TMap::value_type value_type;
	typedef TYPENAME TMap::key_type key_type;

	virtual void clean(const iterator&) {}

public:

	class CreateDataTypeInterface
	{
	public:
		virtual data_type createObject() const = 0;
		virtual bool canCreate() const { return true; };
		~CreateDataTypeInterface(){}
	};

	ValMap(){}
	virtual ~ValMap(){}

	size_t			size()	const	{ return values.size();								}
	const_iterator	begin()	const	{ const_iterator it = values.begin();	return it;	}
	const_iterator	end()	const	{ const_iterator it = values.end();		return it;	}
	iterator		begin()			{ return values.begin(); }
	iterator		end()			{ return values.end(); }

	const_iterator	find( const key_type& k ) const	{ const_iterator it = values.find ( k );	return it; }
	iterator		find( const key_type& k )		{ return values.find( k ); }

	const_iterator	lower_bound( const key_type& k ) const	{ iterator it = values.lower_bound( k );	return it;	}
	const_iterator	upper_bound( const key_type& k ) const	{ iterator it = values.upper_bound( k );	return it;	}
	iterator		lower_bound( const key_type& k )		{ return values.lower_bound( k );}
	iterator		upper_bound( const key_type& k )		{ return values.upper_bound( k );}


	const_reverse_iterator	rbegin()const	{ const_reverse_iterator it = values.rbegin();	return it;	}
	const_reverse_iterator	rend()	const	{ const_reverse_iterator it = values.rend();	return it;	}
	reverse_iterator		rbegin()		{ return values.rbegin(); }
	reverse_iterator		rend()			{ return values.rend(); }


	void clearAll()
	{
		iterator it = values.begin(); 
		while(it != values.end())
		{
			iterator it2 = it;
			++it;
			erase(it2);
		}
	}
	void erase(const iterator& it)
	{
		clean(it);
		values.erase(it);
	}
	void erase(const iterator& it_first, const iterator& it_last )
	{
		iterator it = it_first;
		while( it != it_last )
		{
			clean(it);
			++it;
		}
		values.erase(it_first, it_last);
	}
	data_type extractAndRemove(iterator& it)
	{
		data_type retVal = (*it).second;
		// erase the value from the map but do not clean(it)
		// notice how this is different from the previous function
		values.erase(it);
		return retVal;
	}
	bool findOrAddValue( const key_type& key, const CreateDataTypeInterface& cdti, bool& isAdded, data_type& retVal, data_type dft = (data_type)0 )
	{
		isAdded = false;
		iterator it = values.find(key);
		if( it != values.end())
		{
			retVal = (*it).second;
			return true;
		}
		if( !cdti.canCreate() )
		{
			retVal = dft;
			return false;
		}
		isAdded = true;
		retVal = values.insert( value_type( key, cdti.createObject() ) ).first->second;
		return true;
	}
	bool findValue(const key_type& key, data_type& retVal, data_type dft = (data_type)0)
	{
		retVal = dft;
		iterator it = values.find(key);
		if( it != values.end())
		{
			retVal = (*it).second;
			return true;
		}
		return false;
	}
	bool findValue(const key_type& key, data_type& retVal, data_type dft = (data_type)0) const
	{
		retVal = dft;
		const_iterator it = values.find(key);
		if( it != values.end())
		{
			retVal = (*it).second;
			return true;
		}
		return false;
	}
	bool isFound( const key_type& key ) const
	{
		return values.find(key) != values.end();
	}
	void updateValue(const key_type& key, const data_type& val)
	{
		pair<iterator, bool> res = values.insert(value_type(key, val));
		if( !res.second )
		{
			clean(res.first);
			(*res.first).second = val;
		}
	}
	bool removeValue(const key_type& key)
	{
		iterator it = values.find(key);
		if( it != values.end())
		{
			erase(it);
			return true;
		}
		return false;
	}
	data_type extractValue(const key_type& key)
	{
		iterator it = values.find(key);
		if( it != values.end())
		{
			return extractAndRemove(it);
		}
		return NULL;
	}
	map_type& getValues() { return values;}
	const map_type& getValues() const { return values;}

private:
	map_type values;
};

//////////////////////////////////////////////////////////////////////////

// T_rev_iter and T_const_rev_iter are defined for cases where TMap does not support reverse iterators
// like unordered_map in which case both parameters can be specified as void
// PtrMapIndex< unordered_map<>, void, void >
//
template < class TMap, class T_rev_iterPO = typename TMap::reverse_iterator, class T_const_rev_iterPO = typename TMap::const_reverse_iterator >
class PtrMapIndex : public ValMap< TMap, T_rev_iterPO, T_const_rev_iterPO >
{
public:
	typedef ValMap<TMap, T_rev_iterPO, T_const_rev_iterPO> Parent_;

	typename Parent_::data_type findValue(const typename Parent_::key_type& key)
	{
		typename Parent_::data_type retVal = NULL;
		Parent_::findValue(key, retVal, NULL);
		return retVal;
	}
	const typename Parent_::data_type findValue(const typename Parent_::key_type& key) const
	{
		typename Parent_::data_type retVal = NULL;
		Parent_::findValue(key, retVal, NULL);
		return retVal;
	}
	using Parent_::findValue;
};

//////////////////////////////////////////////////////////////////////////

// T_rev_iter and T_const_rev_iter are defined for cases where TMap does not support reverse iterators
// like unordered_map in which case both parameters can be specified as void
// PtrMapOwner< unordered_map<>, void, void >
//
template < class TMap, class T_rev_iterPO = typename TMap::reverse_iterator, class T_const_rev_iterPO = typename TMap::const_reverse_iterator >
class PtrMapOwner : public ValMap< TMap, T_rev_iterPO, T_const_rev_iterPO >
{
public:
	typedef ValMap<TMap, T_rev_iterPO, T_const_rev_iterPO> Parent_;

protected:
	void clean(const typename Parent_::iterator& it) override
	{
		delete (*it).second;
		(*it).second = NULL;
	}
public:
	~PtrMapOwner()
	{
		clearAll();
	}
	typename Parent_::data_type findValue(const typename Parent_::key_type& key)
	{
		typename Parent_::data_type retVal = NULL;
		Parent_::findValue(key, retVal, NULL);
		return retVal;
	}
	const typename Parent_::data_type findValue(const typename Parent_::key_type& key) const
	{
		typename Parent_::data_type retVal = NULL;
		Parent_::findValue(key, retVal, NULL);
		return retVal;
	}
	const typename Parent_::data_type findOrAddValue(const typename Parent_::key_type& key, const CreateDataTypeInterface& cdti, bool& isAdded)
	{
		typename Parent_::data_type retVal = NULL;
		Parent_::findOrAddValue(key, cdti, isAdded, retVal, NULL);
		return retVal;
	}
	using Parent_::findValue;
};

//////////////////////////////////////////////////////////////////////////
// USAGE EXAMPLES:
//
// for a map that owns pointers:
//
// typedef PStringMap< ObjectType* > StringToObjectMapType_;
// typedef PtrMapOwner< StringToObjectMapType_ > StringToObjectMap_;
// StringToObjectMap_ stomp;
// 
// for a map that does not own pointers:
//
// typedef std::map< UINT32, ObjectType*> NumToObjectMapType_;
// typedef PtrMapIndex< NumToObjectMapType_ > NumToObjectMap_;
// NumToObjectMap_ nomp;
//
// ObjectType* p = stomp.findValue(osFolder);
// if(p == NULL)
// {
//		p = new ObjectType(...);
// 
// 		stomp.updateValue("blah", p);
// }
//
//
// typedef PStringMap<UINT32> StringToNumMapType_;
// typedef ValMap<StringToNumMapType_ > StringToNumMap_;
// StringToNumMap_ snump;
// snump.updateValue("blah", 1);
//
//////////////////////////////////////////////////////////////////////////
#endif // HgValMap_h_included
