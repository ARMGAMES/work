#ifndef commext_h_included
#define commext_h_included
#include <pplib.h>

#include <algorithm>

template <class T> 
class CommExtVector : public vector<T>
{
private:
	typedef vector<T> _Parent;
public: 
	using _Parent::size;
	using _Parent::resize;
	using _Parent::clear;
public:
	void compose(CommMsgBody & body) const
	{
		body.composeUINT32(size());
		for (size_t i=0; i<size(); ++i)
		{
			(*this)[i].compose(body);
		}
	}
	void parse(CommMsgParser & parser)
	{
		clear();
		UINT32 count;
		parser.parseUINT32(count);
		resize(count);
		for (size_t i=0; i<count; i++)
		{
			(*this)[i].parse(parser);
		}
	}
	void remove(size_t i)
	{
		(*this).erase((*this).begin() + i);
	}
	bool equals(const CommExtVector& other) const
	{
		if(size() != other.size()) return false;
		for(size_t i=0; i<size(); ++i)
		{
			if(!(*this)[i].equals(other[i])) return false;
		}
		return true;
	}
};


class CommExtUINT32Vector : public vector<UINT32>
{
public:
	void compose(CommMsgBody & body) const
	{
		body.composeUINT32(size());
		for (size_t i=0; i<size(); ++i)
		{
			body.composeUINT32((*this)[i]);
		}
	}
	void parse(CommMsgParser & parser)
	{
		clear();
		UINT32 count;
		parser.parseUINT32(count);
		resize(count);
		for (size_t i=0; i<count; i++)
		{
			parser.parseUINT32((*this)[i]);
		}
	}
	bool equals(const CommExtUINT32Vector& other) const
	{
		if(size() != other.size()) return false;
		for(size_t i=0; i<size(); ++i)
		{
			if((*this)[i] != other[i]) return false;
		}
		return true;
	}
};

class CommExtUINT16Vector : public vector<UINT16>
{
public:
	void compose(CommMsgBody & body) const
	{
		body.composeUINT32(size());
		for (size_t i=0; i<size(); ++i)
		{
			body.composeUINT16((*this)[i]);
		}
	}
	void parse(CommMsgParser & parser)
	{
		clear();
		UINT32 count;
		parser.parseUINT32(count);
		resize(count);
		for (size_t i=0; i<count; i++)
		{
			parser.parseUINT16((*this)[i]);
		}
	}
	bool equals(const CommExtUINT32Vector& other) const
	{
		if(size() != other.size()) return false;
		for(size_t i=0; i<size(); ++i)
		{
			if((*this)[i] != other[i]) return false;
		}
		return true;
	}
};


class CommExtPStringVector : public vector<PString>
{
public:
	void compose(CommMsgBody & body) const
	{
		body.composeUINT32(size());
		for (size_t i=0; i<size(); ++i)
		{
			body.composeString((*this)[i]);
		}
	}
	void parse(CommMsgParser & parser)
	{
		clear();
		UINT32 count;
		parser.parseUINT32(count);
		resize(count);
		for (size_t i=0; i<count; i++)
		{
			parser.parseStringP( (*this)[i]);
		}
	}
	bool equals(const CommExtPStringVector& other) const
	{
		if(size() != other.size()) return false;
		for(size_t i=0; i<size(); ++i)
		{
			if(!(*this)[i].equals(other[i])) return false;
		}
		return true;
	}
};


template <class T> 
class CommExtPtrVector : public vector<T*>
{
private:
	typedef vector<T*> _Parent;
public: 
	using _Parent::size;
	using _Parent::resize;
	using _Parent::clear;

public:
	void compose(CommMsgBody & body) const
	{
		body.composeUINT32(size());
		for (size_t i=0; i<size(); ++i)
		{
			(*this)[i]->compose(body);
		}
	}
	void parse(CommMsgParser & parser)
	{
		size_t i;
		for (size_t i=0; i<size(); ++i)
		{
			delete (*this)[i];
		}
		clear();
		UINT32 count;
		parser.parseUINT32(count);
		resize(count);
		for (i=0; i<count; i++)
		{
			(*this)[i] = new T();
			(*this)[i]->parse(parser);
		}
	}
	~CommExtPtrVector()
	{
		for (size_t i=0; i<size(); ++i)
		{
			delete (*this)[i];
		}
	}
	bool equals(const CommExtPtrVector& other) const
	{
		if(size() != other.size()) return false;
		for(size_t i=0; i<size(); ++i)
		{
			if((*this)[i] != other[i]) return false;
		}
		return true;
	}
};

struct CommExtStringPair
{
	PString name;
	PString value;
	void compose(CommMsgBody & body) const
	{
		body.composeString(name)
			.composeString(value); 
	}
	void parse(CommMsgParser & parser)
	{
		parser.parseStringP(name)
			  .parseStringP(value);
	}
	bool equals(const CommExtStringPair& other) const
	{
		if(name.equals(other.name) && value.equals(other.value)) 
			return true;
		else
			return false;
	}
};

class CommExtStringPairVector : public  CommExtVector<CommExtStringPair>
{
public:
	const char * find(const char * name) const
	{
		for (size_t i = 0; i < size(); i++)
		{
			if ((*this)[i].name.equals(name))
				return (*this)[i].value.c_str();
		}
		return 0;
	}
	void insert(const char * name,const char * value )
	{
		CommExtStringPair & p = *CommExtVector<CommExtStringPair>::insert(end(),CommExtStringPair());	
		p.name = name;
		p.value = value;
	}

	void insertMove(PString & name,PString & value )
	{
		CommExtStringPair & p = *CommExtVector<CommExtStringPair>::insert(end(),CommExtStringPair());	
		p.name.moveFrom(name);
		p.value.moveFrom(value);
	}
	bool equals(const CommExtStringPairVector& other) const
	{
		if(size() != other.size()) return false;
		for(size_t i=0; i<size(); ++i)
		{
			if(!(*this)[i].equals(other[i])) return false;
		}
		return true;
	}
};


class CommExtStringPairMap : public  PStringMap<PString>
{
public:
	void compose(CommMsgBody & body) const
	{
		body.composeUINT32(size());
		const_iterator it;
		for (it=begin(); it!=end(); ++it)
		{
			body.composeString((*it).first);
			body.composeString((*it).second);
		}
	}
	void parse(CommMsgParser & parser)
	{
		clear();
		UINT32 count;
		parser.parseUINT32(count);
		for ( int i = 0; i< count; i++)
		{
			const char * name;
			parser.parseString(name);
			iterator it = PStringMap<PString>::insert( value_type( name, PString() ) ).first;
			parser.parseStringP((*it).second);
		}
	}
	const char * find(const char * name) const
	{
		const_iterator it  = PStringMap<PString>::find(name);
		if (it != end())
			return (*it).second.c_str();
		else
			return 0;
	}
	void insert(const char * name,const char * value )
	{
		iterator it = PStringMap<PString>::insert( value_type( name, PString() ) ).first;
		(*it).second = value;
	}
	bool equals(const CommExtStringPairMap& other) const 
	{
		if(size() != other.size()) return false;
		const_iterator _iter1;
		const_iterator _iter2;
		for(_iter1=other.begin(),_iter2=begin(); _iter2!=end(); ++_iter1,++_iter2)
		{
			if (strcmp(_iter1->first,_iter2->first) || !_iter1->second.equals(_iter2->second))
				return false;
		}
		return true;
	}
};

class CommExtUINT32StringMap : public  map<UINT32,PString>
{
public:
	void compose(CommMsgBody & body) const
	{
		body.composeUINT32(size());
		const_iterator it;
		for (it = begin(); it != end(); ++it)
		{
			body.composeUINT32((*it).first);
			body.composeString((*it).second);
		}
	}
	void parse(CommMsgParser & parser)
	{
		clear();
		UINT32 count;
		parser.parseUINT32(count);
		for (int i = 0; i < count; i++)
		{
			UINT32 key;
			parser.parseUINT32(key);
			iterator it = map<UINT32, PString>::insert(value_type(key, PString())).first;
			parser.parseStringP((*it).second);
		}
	}
	const char * find(UINT32 key) const
	{
		const_iterator it = map<UINT32, PString>::find(key);
		if (it != end())
			return (*it).second.c_str();
		else
			return 0;
	}
	void insert(UINT32 key, const char * value)
	{
		iterator it = map<UINT32, PString>::insert(value_type(key, PString())).first;
		(*it).second = value;
	}
	bool equals(const CommExtUINT32StringMap& other) const
	{
		if (size() != other.size()) return false;
		const_iterator _iter1;
		const_iterator _iter2;
		for (_iter1 = other.begin(), _iter2 = begin(); _iter2 != end(); ++_iter1, ++_iter2)
		{
			if (_iter1->first != _iter2->first || !_iter1->second.equals(_iter2->second))
				return false;
		}
		return true;
	}
};

template<class T>
struct CommExtPair
{
	PString name;
	T value;
	void compose(CommMsgBody & body) const
	{
		body.composeString(name);
		value.compose(body); 
	}
	void parse(CommMsgParser & parser)
	{
		parser.parseStringP(name);
		value.parse(parser);
	}
	bool equals(const CommExtPair& other) const
	{
		if(name.equals(other.name) && value.equals(other.value)) 
			return true;
		else
			return false;
	}
};

template<class T>
class CommExtPairVector : public  CommExtVector<CommExtPair<T> >
{
private:
	typedef CommExtVector<CommExtPair<T> > _Parent;
public: 
	using _Parent::size;
	using _Parent::resize;
	using _Parent::clear;

public:
	const T * find(const char * name) const
	{
		for (size_t i = 0; i < size(); i++)
		{
			if ((*this)[i].name.equalsIgnoreCase(name))
				return &(*this)[i].value;
		}
		return 0;
	}

	T * find(const char * name)
	{
		for (size_t i = 0; i < size(); i++)
		{
			if ((*this)[i].name.equalsIgnoreCase(name))
				return &(*this)[i].value;
		}
		return 0;
	}

	size_t findIndex(const char * name) const
	{
		for (size_t i = 0; i < size(); i++)
		{
			if ((*this)[i].name.equalsIgnoreCase(name))
				return i;
		}
		return -1;
	}

	T & insert(const char * name)
	{
		CommExtPair<T> & p = *CommExtVector<CommExtPair<T> >::insert((*this).end(),CommExtPair<T>());
		p.name = name;
		return p.value;
	}
	T & insertMove(PString & name)
	{
		CommExtPair<T> & p = *CommExtVector<CommExtPair<T> >::insert((*this).end(),CommExtPair<T>());
		p.name.moveFrom(name);
		return p.value;
	}

	bool equals(const CommExtPairVector& other) const
	{
		if(size() != other.size()) return false;
		for(size_t i=0; i<size(); ++i)
		{
			if(!(*this)[i].equals(other[i])) return false;
		}
		return true;
	}
};

template<class T>
class CommExtPairMap : public  PStringMap<T>
{
private:
	typedef PStringMap<T> _Parent;
public: 
	using _Parent::size;
	using _Parent::clear;
	using _Parent::begin;
	using _Parent::end;

public:
	typedef typename _Parent::iterator iterator;
	typedef typename _Parent::const_iterator const_iterator;

	void compose(CommMsgBody & body) const
	{
		body.composeUINT32(size());
		const_iterator it;
		for (it =  begin(); it != end(); ++it)
		{
			body.composeString((*it).first);
			(*it).second.compose(body);
		}
	}
	void parse(CommMsgParser & parser)
	{
		clear();
		UINT32 count;
		parser.parseUINT32(count);
		for ( int i = 0; i< count; i++)
		{
			const char * name;
			parser.parseString(name);
			iterator it = PStringMap<T>::insert( value_type( name,T() ) ).first;
			(*it).second.parse(parser);
		}
	}
	const T * find(const char * name) const
	{
		const_iterator it  = PStringMap<T>::find(name);
		if (it !=  end())
			return &(*it).second;
		else
			return 0;
	}
	T * find(const char * name) 
	{
		iterator it  = PStringMap<T>::find(name);
		if (it !=  end())
			return &(*it).second;
		else
			return 0;
	}
	T & insert(const char * name )
	{
		iterator it = PStringMap<T>::insert( value_type( name, T() ) ).first;
		return (*it).second;
	}
	bool equals(const CommExtPairMap& other) const
	{
		if(size() != other.size()) return false;
		const_iterator _iter1;
		const_iterator _iter2;
		for(_iter1=other.begin(),_iter2=begin(); _iter2!=end(); ++_iter1,++_iter2)
		{
			if (strcmp(_iter1->first,_iter2->first) || !_iter1->second.equals(_iter2->second))
				return false;
		}
		return true;
	}
};

class CommExtStringSet : public  PStringSet
{
public:
	void compose(CommMsgBody & body) const
	{
		body.composeUINT32(size());
		const_iterator it;
		for (it=begin(); it!=end(); ++it)
		{
			body.composeString((*it));
		}
	}
	void parse(CommMsgParser & parser)
	{
		clear();
		UINT32 count;
		parser.parseUINT32(count);
		for ( int i = 0; i< count; i++)
		{
			const char * val;
			parser.parseString(val);
			insert( val );
		}
	}
	bool equals(const CommExtStringSet& other) const
	{
		if(size() != other.size()) return false;
		const_iterator _iter1;
		const_iterator _iter2;
		for(_iter1=other.begin(),_iter2=begin(); _iter2!=end(); ++_iter1,++_iter2)
		{
			if(strcmp(*_iter2,*_iter1))
				return false;
		}
		return true;
	}
};

class CommExtUINT32Set : public set<UINT32>
{
public:
	void compose(CommMsgBody & body) const
	{
		body.composeUINT32(size());
		const_iterator it;
		for (it=begin(); it!=end(); ++it)
		{
			body.composeUINT32((*it));
		}
	}
	void parse(CommMsgParser & parser)
	{
		clear();
		UINT32 count;
		parser.parseUINT32(count);
		for (size_t i=0; i<count; i++)
		{
			UINT32 val;
			parser.parseUINT32(val);
			insert(val);
		}
	}
	bool isExist(const UINT32 val) const
	{
		const_iterator it = find(val);
		return it != end();
	}
	bool equals(const CommExtUINT32Set& other) const
	{
		if(size() != other.size()) return false;
		const_iterator _iter1;
		const_iterator _iter2;
		for(_iter1=other.begin(),_iter2=begin(); _iter2!=end(); ++_iter1,++_iter2)
		{
			if (*_iter1 != *_iter2)
				return false;
		}
		return true;
	}
	bool intersection(const CommExtUINT32Set& other) const
	{
		if(size() == 0 || other.size() == 0) return false;
		const_iterator _iter1 = other.begin();
		const_iterator _iter2 = begin();
		while( _iter1 != other.end() && _iter2 != end())
		{
			if(*_iter2 == *_iter1) return true;
			if (*_iter2 > *_iter1)
				_iter1 = std::lower_bound(_iter1,other.end(),*_iter2);
			else
				_iter2 = std::lower_bound(_iter2,end(),*_iter1);
		}
		return false;
	}
};

class CommExtUINT32MapSet : public map<int, CommExtUINT32Set>	// using for deleted emails/attachments
{
public:
	void add(UINT32 a, UINT32 b)
	{
		pair<iterator, bool> p;
		CommExtUINT32Set n;

		if(b == -1)
		{
			n.insert(-1);
			p = insert(value_type(a, n));
			if(!p.second)
			{
				p.first->second.swap(n);
			}
		}
		else
		{
			p = insert(value_type(a, n));
			if(!p.second && *p.first->second.begin() == -1)
			{
				n.insert(b);
				p.first->second.swap(n);
			}
			else
			{
				p.first->second.insert(b);
			}
		}
	}
	void compose(CommMsgBody & body) const
	{
		body.composeUINT32(size());
		const_iterator it;
		for (it = begin(); it != end(); ++it)
		{
			body.composeUINT32((*it).first);
			it->second.compose(body);
		}
	}
	void parse(CommMsgParser & parser)
	{
		UINT32 count;
		clear();
		parser.parseUINT32(count);
		for ( int i = 0; i< count; i++)
		{
			UINT32 val;
			CommExtUINT32Set n;
			parser.parseUINT32(val);
			iterator it = insert(value_type(val,n)).first;
			it->second.parse(parser);
		}
	}
	bool equals(const CommExtUINT32MapSet& other) const
	{
		if(size() != other.size()) return false;
		const_iterator _iter1;
		const_iterator _iter2;
		for(_iter1=other.begin(),_iter2=begin(); _iter2!=end(); ++_iter1,++_iter2)
		{
			if ((_iter1->first != _iter2->first) || !_iter1->second.equals(_iter2->second))
				return false;
		}
		return true;
	}
};

struct CommExtStringUINT32Pair
{
	PString name;
	UINT32 value;
	void compose(CommMsgBody & body) const
	{
		body.composeString(name)
			.composeUINT32(value); 
	}
	void parse(CommMsgParser & parser)
	{
		parser.parseStringP(name)
			.parseUINT32(value);
	}
	bool equals(const CommExtStringUINT32Pair& other) const
	{
		if(name.equals(other.name) && (value == other.value)) 
			return true;
		else
			return false;
	}
};

class CommExtStringUINT32PairVector : public  CommExtVector<CommExtStringUINT32Pair>
{
public:
	UINT32 find(const char * name) const
	{
		for (size_t i = 0; i < size(); i++)
		{
			if ((*this)[i].name.equals(name))
				return (*this)[i].value;
		}
		return 0;
	}
	void insert(const char * name, UINT32 value )
	{
		CommExtStringUINT32Pair & p = *CommExtVector<CommExtStringUINT32Pair>::insert(end(),CommExtStringUINT32Pair());	
		p.name = name;
		p.value = value;
	}
	bool equals(const CommExtStringUINT32PairVector& other) const
	{
		if(size() != other.size()) return false;
		for(size_t i=0; i<size(); ++i)
		{
			if(!(*this)[i].equals(other[i])) return false;
		}
		return true;
	}
};

struct CommExtStringUINT32UINT32
{
	PString name;
	UINT32 value1;
	UINT32 value2;
	void compose(CommMsgBody & body) const
	{
		body.composeString(name)
			.composeUINT32(value1)
			.composeUINT32(value2); 
	}
	void parse(CommMsgParser & parser)
	{
		parser.parseStringP(name)
			.parseUINT32(value1)
			.parseUINT32(value2);
	}
	bool equals(const CommExtStringUINT32UINT32& other) const
	{
		if(name.equals(other.name) && (value1 == other.value1) && (value2 == other.value2)) 
			return true;
		else
			return false;
	}
};

class CommExtStringUINT32UINT32Vector : public  CommExtVector<CommExtStringUINT32UINT32>
{
public:
	void insert(const char * name, UINT32 value1, UINT32 value2)
	{
		CommExtStringUINT32UINT32 & p = *CommExtVector<CommExtStringUINT32UINT32>::insert(end(),CommExtStringUINT32UINT32());	
		p.name = name;
		p.value1 = value1;
		p.value2 = value2;
	}
	bool equals(const CommExtStringUINT32UINT32Vector& other) const
	{
		if(size() != other.size()) return false;
		for(size_t i=0; i<size(); ++i)
		{
			if(!(*this)[i].equals(other[i])) return false;
		}
		return true;
	}
};

struct CommExtUINT32Pair
{
	UINT32 value1;
	UINT32 value2;
	void compose(CommMsgBody & body) const
	{
		body.composeUINT32(value1)
			.composeUINT32(value2);
	}
	void parse(CommMsgParser & parser)
	{
		parser.parseUINT32(value1)
			.parseUINT32(value2);
	}
	bool equals(const CommExtUINT32Pair& other) const
	{
		if ((value1 == other.value1) && (value2 == other.value2))
			return true;
		else
			return false;
	}
};


#endif
