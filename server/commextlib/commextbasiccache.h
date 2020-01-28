#ifndef commextbasiccache_h_included
#define commextbasiccache_h_included
#include <pplib.h>
#include <ppcontainer.h>

class CommExtBasicCache
{

private:
	struct _CommExtBasicCacheItem;
	typedef PStringMap<_CommExtBasicCacheItem *> _CommExtBasicMap;
	typedef _CommExtBasicMap::iterator _CommExtBasicMapIterator; 
	typedef list<_CommExtBasicMapIterator> _CommExtBasicList;
	typedef _CommExtBasicList::iterator _CommExtBasicListIterator;

	struct _CommExtBasicCacheItem
	{
		PBlock data;
		_CommExtBasicListIterator accessListIt;
	};

	class _CommExtBasicCacheMap
	{
		_CommExtBasicList accessList;
		_CommExtBasicMap  cacheMap;
		size_t maxNumber;
		size_t maxMemory;
		size_t _memory;
		_CommExtBasicCacheMap(size_t maxNumber_,size_t maxMemory_)
		{
			maxNumber = maxNumber_;
			maxMemory = maxMemory_;
			_memory = 0;
		}
		~_CommExtBasicCacheMap()
		{
			_CommExtBasicMapIterator itm;
			for (itm = cacheMap.begin(); itm != cacheMap.end(); ++itm)
			{
				delete (*itm).second;
			}
		}
		PBlock * _findItem( const char * id)
		{
			_CommExtBasicMapIterator it = cacheMap.find(id);
			if (it != cacheMap.end())
			{
				if ((maxNumber ||  maxMemory) && it != accessList.back())
				{
					accessList.splice(accessList.end(),accessList, (*it).second->accessListIt);
					(*it).second->accessListIt = --accessList.end();
				}
				return &(*it).second->data;
			}
			else
				return 0;

		}
		bool _addItem( const char * id, PBlock & data, PString& errorMsg, bool update)
		{
			if (maxMemory && maxMemory < data.size())
			{
				errorMsg.assign("Value has bigger size than container - ").appendUint64(data.size());
				return false;
			}
			_CommExtBasicMapIterator itOld = cacheMap.find(id);
			if (itOld != cacheMap.end())
			{
				if(update)
				{
					_memory-=(*itOld).second->data.size();
					_memory+=data.size();
					(*itOld).second->data.moveFrom(data);
					if ((maxNumber ||  maxMemory) && itOld != accessList.back())
					{
						accessList.splice(accessList.end(),accessList, (*itOld).second->accessListIt);
						(*itOld).second->accessListIt = --accessList.end();
					}
					_CommExtBasicListIterator it =accessList.begin();
					while ( maxMemory && _memory > maxMemory && it != accessList.end())
					{
						_CommExtBasicMapIterator itm = *it;
						_memory-=(*itm).second->data.size();
						it = accessList.erase(it);
						delete (*itm).second;
						cacheMap.erase(itm);
					}
					return true;
				}
				else
				{
					errorMsg.assign("Value with this key already exists - ").append(id);
					return false;
				}
			}

			if (maxNumber && cacheMap.size() > maxNumber || maxMemory && (_memory + data.size()) > maxMemory)
			{
				_CommExtBasicListIterator it;
				it =accessList.begin();
				while ((maxNumber && cacheMap.size() > maxNumber || maxMemory && (_memory + data.size()) > maxMemory ) && it != accessList.end())
				{
					_CommExtBasicMapIterator itm = *it;
					_memory-=(*itm).second->data.size();
					it = accessList.erase(it);
					delete (*itm).second;
					cacheMap.erase(itm);
				}
			}

			pair<_CommExtBasicMapIterator,bool> res = cacheMap.insert(_CommExtBasicMap::value_type(id, new _CommExtBasicCacheItem()));
			if (res.second == false)
				PASSERT3(0);
			if (maxNumber || maxMemory)
			{
				(*res.first).second->accessListIt = accessList.insert(accessList.end(),res.first);
			}
			_memory+=data.size();
			(*res.first).second->data.moveFrom(data);
			return true;
		}
		void _removeItem( const char * id)
		{
			_CommExtBasicMapIterator itm = cacheMap.find(id);
			if (itm != cacheMap.end())
			{
				if (maxNumber || maxMemory)
				{
					_memory-=(*itm).second->data.size();
					accessList.erase((*itm).second->accessListIt);
				}
				delete (*itm).second;
				cacheMap.erase(itm);
			}
		}
		size_t size() const
		{
			return cacheMap.size();
		}
		size_t memory() const
		{
			return _memory;
		}
		friend class CommExtBasicCache;
	};


	typedef PStringMap<_CommExtBasicCacheMap * > _CommExtBasicCacheMaps;
	_CommExtBasicCacheMaps cacheMaps;


private:
	_CommExtBasicCacheMap * _findMap(const char * type)
	{
		_CommExtBasicCacheMaps::iterator im = cacheMaps.find(type);
		if (im == cacheMaps.end())
		{
			PTRACE3("CommExtBasicCache::_findMap: Cache of type %s not found",type);
			PASSERT3(0);
		}
		return (*im).second;
	}

public:
	~CommExtBasicCache()
	{
		_CommExtBasicCacheMaps::iterator im;
		for (im = cacheMaps.begin(); im != cacheMaps.end(); ++im)
		{
			delete (*im).second;
		}
	}


	size_t size(const char * type)
	{
		return _findMap(type)->size();
	}
	size_t memory(const char * type)
	{
		return _findMap(type)->memory();
	}

	const PBlock * findItem(const char * type, const char * id)
	{
		PASSERT3(id != 0);
		return _findMap(type)->_findItem(id);
	}

	bool  addItem(const char * type, const char * id, PBlock & data, PString& errorMsg)
	{
		PTRACE5("CommExtBasicCache::addItem('%s',%s)",type,id);
		PASSERT3(id != 0);
		return _findMap(type)->_addItem(id,data,errorMsg, false);
	}

	bool  updateItem(const char * type, const char * id, PBlock & data, PString& errorMsg)
	{
		PTRACE5("CommExtBasicCache::updateItem('%s',%s)",type,id);
		PASSERT3(id != 0);
		return _findMap(type)->_addItem(id,data,errorMsg, true);
	}

	void removeItem(const char * type, const char * id)
	{
		PTRACE5("CommExtBasicCache::removeItem('%s',%s)",type,id);
		PASSERT3(id != 0);
		_findMap(type)->_removeItem(id);
	}

	bool addObjectType(const char * type, PString& errorMsg, size_t maxNumber = 0, size_t maxMemory = 0)
	{
		PTRACE3("CommExtBasicCache::addCacheType('%s')",type);

		_CommExtBasicCacheMaps::iterator im = cacheMaps.find(type);
		if (im != cacheMaps.end())
		{
			PTRACE3("CommExtBasicCache::addObjectType(%s) - Cache of this type already exists",type);
			errorMsg.assign("Cache of this type already exists - ").append(type);
			return false;
		}
		cacheMaps.insert(_CommExtBasicCacheMaps::value_type(type,new _CommExtBasicCacheMap(maxNumber,maxMemory)));
		return true;
	}

	bool deleteObjectType(const char * type, PString& errorMsg)
	{
		PTRACE3("CommExtBasicCache::deleteCacheType('%s')",type);
		_CommExtBasicCacheMaps::iterator im = cacheMaps.find(type);
		if (im == cacheMaps.end())
		{
			PTRACE3("CommExtBasicCache::deleteCacheType(%s) - Cache of this type not found",type);
			errorMsg.assign("Cache of this type not found - ").append(type);
			return false;
		}
		delete (*im).second;
		cacheMaps.erase(im);
		return true;
	}

	void getAllTypes(vector<PString>& types)
	{
		PTRACE3("CommExtBasicCache::getAllTypes");

		_CommExtBasicCacheMaps::iterator im;
		for(im = cacheMaps.begin(); im != cacheMaps.end(); ++im)
		{
			types.push_back((*im).first);
		}
	}
};

#endif

