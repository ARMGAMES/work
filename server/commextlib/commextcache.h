#ifndef commextcache_h_included
#define commextcache_h_included

#include "commextobject.h"

#define COM_EXT_INVALID_PTR ((CommExtObject *) (-1))
#define COM_EXT_COPY_PTR ((CommExtObject *) (-2))
#define COM_EXT_DELETE_PTR ((CommExtObject *) (-3))

class CommExtCache
{

public:
	struct _CommExtCacheItem;

private:
//public:

	enum {IntTypeItem = 1, StringTypeItem = 2}; 

	typedef map<UINT32,_CommExtCacheItem * > _CommExtIntCacheMap;
	typedef PStringMap<_CommExtCacheItem * > _CommExtStringCacheMap;
	struct _CommExtCacheTypeMap;


public:
	struct _CommExtCacheItem
	{
		UINT32 thisId;
		CommExtObject * obj;
		CommExtObject * writePtr;
		CommExtObject * extData;
		map<UINT32,CommExtObject *>   readAccess;
		deque<UINT32> exclusiveWaiting;
		UINT32        exclusiveAccess;
		list<UINT32>::iterator  accessListIt;
		_CommExtCacheTypeMap * mapPtr;
		bool ready;
		_CommExtCacheItem( )
		{
			thisId = 0;
			obj = 0;
			writePtr = COM_EXT_INVALID_PTR;
			exclusiveAccess = 0;
			mapPtr = 0;
			ready = false;
			extData = 0;
		}
		CommExtObject * hasReadAccess(UINT32 reqId) 
		{
			map<UINT32,CommExtObject *>::iterator it;
			it = readAccess.find(reqId);
			if (it != readAccess.end())
			{
				return (*it).second;
			}
			else
			{
				return COM_EXT_INVALID_PTR;
			}
		}
		void setReadAccess(UINT32 reqId,CommExtObject * ptr)
		{
			readAccess[reqId]=ptr;
		}

		bool checkReadAccess(CommExtObject * ptr)
		{
			if (ptr == obj)
				return false;
			map<UINT32,CommExtObject *>::iterator it;
			for (it = readAccess.begin(); it != readAccess.end(); ++it)
			{
				if ((*it).second == ptr)
					break;
			}
			return it == readAccess.end();
		}
		bool releaseReadRequest(UINT32 reqId)
		{
			CommExtObject * ptr =  hasReadAccess(reqId);
			if ( ptr != COM_EXT_INVALID_PTR)
			{
				readAccess.erase(reqId);
				return checkReadAccess(ptr);
			}
			return false;
		}

		void releaseExclusiveRequest(UINT32 reqId)
		{
			if (exclusiveAccess == reqId)
			{
				exclusiveAccess = 0;
				writePtr = COM_EXT_INVALID_PTR;
				return;
			}
			deque<UINT32>::iterator itq;
			for (itq = exclusiveWaiting.begin(); itq != exclusiveWaiting.end(); ++itq)
			{
				if ((*itq) == reqId)
				{
					exclusiveWaiting.erase(itq);
					break;
				}
			}
		}
		void printStat();

	};


private:
	struct _CommExtCacheIntItem : public _CommExtCacheItem
	{
		_CommExtIntCacheMap::iterator mapIter;
		
	};
	struct _CommExtCacheStringItem : public _CommExtCacheItem
	{
		_CommExtStringCacheMap::iterator mapIter;
	};



	struct _CommExtCacheTypeMap
	{
		list<UINT32> accessList;
		UINT32 maxNumber;
		PString typeName;
		_CommExtCacheTypeMap(const char * typeName_,UINT32 maxNumber_)
		{
			typeName = typeName_;
			maxNumber=maxNumber_;
		}
		virtual int whatType() const = 0;
		virtual _CommExtCacheItem * _findItem( UINT32 id, bool noAccess) 
		{ 
			PASSERT3(0);
		}
		virtual _CommExtCacheItem * _findItem( const char * id, bool noAccess)
		{ 
			PASSERT3(0);
		}
		virtual _CommExtCacheItem * _addItem( CommIdMap<_CommExtCacheItem	*> & itemsMap_, UINT32 id) 
		{ 
			PASSERT3(0);
		}
		virtual _CommExtCacheItem * _addItem( CommIdMap<_CommExtCacheItem	*> & itemsMap_, const char * id)
		{ 
			PASSERT3(0);
		}
		virtual void _removeItem( CommIdMap<_CommExtCacheItem	*> & itemsMap_, UINT32 id) 
		{ 
			PASSERT3(0);
		}
		virtual void _removeItem( CommIdMap<_CommExtCacheItem	*> & itemsMap_, const char * id)
		{ 
			PASSERT3(0);
		}
		virtual void _removeItem( CommIdMap<_CommExtCacheItem	*> & itemsMap_, _CommExtCacheItem *item_)
		{ 
			PASSERT3(0);
		}
		virtual size_t size() const
		{
			PASSERT3(0);
		}

	};

	struct _CommExtIntCacheTypeMap : public _CommExtCacheTypeMap
	{
		CommExtCache & parent;
		_CommExtIntCacheMap cacheMap;
		_CommExtIntCacheTypeMap(CommExtCache & parent_,const char * typeName_,UINT32 maxNumber_) : _CommExtCacheTypeMap(typeName_,maxNumber_),parent(parent_)
		{
		}
		int whatType() const override
		{
			return IntTypeItem;
		}
		_CommExtCacheItem * _findItem( UINT32 id, bool noAccess) override;
		_CommExtCacheItem * _addItem( CommIdMap<_CommExtCacheItem	*> & itemsMap_,UINT32 id) override;
		void _removeItem( CommIdMap<_CommExtCacheItem	*> & itemsMap_, UINT32 id) override;
		void _removeItem( CommIdMap<_CommExtCacheItem	*> & itemsMap_, _CommExtCacheItem *item_) override;
		size_t size() const  override
		{
			return cacheMap.size();
		}
	};

	struct _CommExtStringCacheTypeMap : public _CommExtCacheTypeMap
	{
		CommExtCache & parent;
		_CommExtStringCacheMap cacheMap;
		_CommExtStringCacheTypeMap(CommExtCache & parent_,const char * typeName_,UINT32 maxNumber_) : _CommExtCacheTypeMap(typeName_,maxNumber_),parent(parent_)
		{
		}
		int whatType() const override
		{
			return StringTypeItem;
		}
		_CommExtCacheItem * _findItem( const char * id,  bool noAccess) override;
		_CommExtCacheItem * _addItem( CommIdMap<_CommExtCacheItem	*> & itemsMap_, const char * id) override;
		void _removeItem( CommIdMap<_CommExtCacheItem	*> & itemsMap_, const char * id) override;
		void _removeItem( CommIdMap<_CommExtCacheItem	*> & itemsMap_, _CommExtCacheItem *item_) override;
		size_t size() const  override
		{
			return cacheMap.size();
		}



	};

	typedef PStringMap<_CommExtCacheTypeMap * > _CommExtCacheMaps;

	
	CommIdMap<_CommExtCacheItem	*> itemsMap;
	_CommExtCacheMaps cacheMaps;

	PReadWriteLock typeCs;




//	_CommExtCacheTypeMap *  CommExtCache::getObjectTypeMap(const char * name);



public:
	struct CommExtIntIterator
	{
		_CommExtIntCacheMap::iterator it;
		_CommExtIntCacheTypeMap       *intMap;
		CommExtIntIterator & operator++()
		{
			++it;
			return *this;
		}
		bool end()
		{
			return it == intMap->cacheMap.end();
		}
		UINT32 operator *()
		{
			return (*it).first;
		}

	};
	struct CommExtStringIterator
	{
		_CommExtStringCacheMap::iterator it;
		_CommExtStringCacheTypeMap       *stringMap;
		CommExtStringIterator & operator++()
		{
			++it;
			return *this;
		}
		bool end()
		{
			return it == stringMap->cacheMap.end();
		}
		const char * operator *()
		{
			return (*it).first;
		}
	};

	CommExtIntIterator beginInt(const char * type);
	CommExtStringIterator beginString(const char * type);


	virtual ~CommExtCache(){}
	
	bool                _isStringType(const char * type);
	_CommExtCacheItem * _findItem(const char * type, UINT32 id);
	_CommExtCacheItem * _findItem(const char * type, const char * id);
	_CommExtCacheItem * _findItemNoAccess(const char * type, UINT32 id);
	_CommExtCacheItem * _findItemNoAccess(const char * type, const char * id);

	_CommExtCacheItem * _findItem(UINT32 thisId);

	_CommExtCacheItem * _addItem(const char * type, UINT32 id);
	_CommExtCacheItem * _addItem(const char * type, const char * id);

	void _getAllItems(const char * type, vector<UINT32> & ids);
	void _getAllItems(const char * type, vector<PString> & ids);
	void addIntObjectType(const char * name, UINT32 max = 0);
	void addStringObjectType(const char * name, UINT32 max = 0);
	void changeMaxObjectType(const char * name, UINT32 max);
	bool isObjectType(const char * name);
	void _removeItem(const char * type, UINT32 id);
	void _removeItem(const char * type, const char * id);
	void _removeItem(UINT32 thisId);

	void printStat();
	void printStatItem(UINT32 itemId);
	UINT32 objectsInCache(const char * name);
	void _releaseCache(const char * type);
	virtual void itemRemoved(const char * name, UINT32 id, CommExtObject * ptr){}
	virtual void itemRemoved(const char * name, const char * id, CommExtObject * ptr){}

	//	void addObject(const char * name, UINT32 id, CommExtObject * obj);
//	void removeObject(const char * name, UINT32 id);
//	_CommExtCacheItem * getObject(const char * name, UINT32 id);




};





#endif

