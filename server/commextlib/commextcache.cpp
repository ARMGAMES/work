#include <pplib.h>
#include <commlib.h>
#include "commextobject.h"
#include "commextcache.h"


void CommExtCache::addIntObjectType(const char * name, UINT32 max)
{
	PTRACE3("CommExtCache::addIntObjectType('%s')",name);
	PWLock lock(typeCs);
	_CommExtCacheMaps::iterator im = cacheMaps.find(name);
	if (im != cacheMaps.end())
	{
		PTRACE3("CommExtCache::addIntObjectType(%s) - Object already exists",name);
		PASSERT3(0);
	}
	cacheMaps.insert(_CommExtCacheMaps::value_type(name,new _CommExtIntCacheTypeMap(*this,name,max)));
}

void CommExtCache::addStringObjectType(const char * name, UINT32 max)
{
	PTRACE3("CommExtCache::addStringObjectType('%s')",name);
	PWLock lock(typeCs);
	_CommExtCacheMaps::iterator im = cacheMaps.find(name);
	if (im != cacheMaps.end())
	{
		PTRACE3("CommExtCache::addStringObjectType(%s) - Object already exists",name);
		PASSERT3(0);
	}
	cacheMaps.insert(_CommExtCacheMaps::value_type(name,new _CommExtStringCacheTypeMap(*this,name,max)));
}

bool CommExtCache::isObjectType(const char * name)
{
	PTRACE5("CommExtCache::isObjectType('%s')",name);
	PRLock lock(typeCs);
	_CommExtCacheMaps::iterator im = cacheMaps.find(name);
	return (im != cacheMaps.end());
}


void CommExtCache::changeMaxObjectType(const char * name, UINT32 max)
{
	PTRACE3("CommExtCache::changeMaxObjectType('%s')",name);
	PWLock lock(typeCs);
	_CommExtCacheMaps::iterator im = cacheMaps.find(name);
	if (im == cacheMaps.end())
	{
		PTRACE3("CommExtCache::changeMaxObjectType(%s) - Object not found",name);
		PASSERT3(0);
	}
	(*im).second->maxNumber = max;
}



/*
CommExtCache::_CommExtCacheTypeMap *  CommExtCache::getObjectTypeMap(const char * name)
{
	PRLock lock(globalCs); 
	_CommExtCacheMaps::iterator im = maps.find(name);
	if (im == maps.end())
	{
		PTRACE3("Object type %s not found",name);
		PASSERT3(0);
	}
	return (*im).second;
}
*/

bool CommExtCache::_isStringType(const char * type)
{
	PTRACE5("CommExtCache::_isStringType('%s')",type);
	PRLock lock(typeCs);
	_CommExtCacheMaps::iterator im = cacheMaps.find(type);
	if (im == cacheMaps.end())
	{
		PTRACE3("CommExtCache::_isStringType: Object type %s not found",type);
		PASSERT3(0);
	}
	return (*im).second->whatType() == StringTypeItem;
}



CommExtCache::_CommExtCacheItem * CommExtCache::_findItem(const char * type, UINT32 id)
{
	PTRACE5("CommExtCache::_findItem('%s',%d)",type,id);
	PASSERT3(id != 0);
	PWLock lock(typeCs);
	_CommExtCacheMaps::iterator im = cacheMaps.find(type);
	if (im == cacheMaps.end() || (*im).second->whatType() != IntTypeItem)
	{
		PTRACE3("CommExtCache::_findItem: Object type %s not found",type);
		PASSERT3(0);
	}
	return (*im).second->_findItem(id,false);
}

CommExtCache::_CommExtCacheItem * CommExtCache::_findItem(const char * type, const char * id)
{
	PTRACE5("CommExtCache::_findItem('%s',%s)",type,id);
	PASSERT3(id != 0);
	PWLock lock(typeCs);
	_CommExtCacheMaps::iterator im = cacheMaps.find(type);
	if (im == cacheMaps.end() || (*im).second->whatType() != StringTypeItem)
	{
		PTRACE3("CommExtCache::_findItem: Object type %s not found",type);
		PASSERT3(0);
	}
	return (*im).second->_findItem(id,false);
}

CommExtCache::_CommExtCacheItem * CommExtCache::_findItemNoAccess(const char * type, UINT32 id)
{
	PTRACE5("CommExtCache::_findItemNoAccess('%s',%d)",type,id);
	PASSERT3(id != 0);
	PRLock lock(typeCs);
	_CommExtCacheMaps::iterator im = cacheMaps.find(type);
	if (im == cacheMaps.end() || (*im).second->whatType() != IntTypeItem)
	{
		PTRACE3("CommExtCache::_findItem: Object type %s not found",type);
		PASSERT3(0);
	}
	return (*im).second->_findItem(id,true);
}

CommExtCache::_CommExtCacheItem * CommExtCache::_findItemNoAccess(const char * type, const char * id)
{
	PTRACE5("CommExtCache::_findItemNoAccess('%s',%s)",type,id);
	PASSERT3(id != 0);
	PRLock lock(typeCs);
	_CommExtCacheMaps::iterator im = cacheMaps.find(type);
	if (im == cacheMaps.end() || (*im).second->whatType() != StringTypeItem)
	{
		PTRACE3("CommExtCache::_findItem: Object type %s not found",type);
		PASSERT3(0);
	}
	return (*im).second->_findItem(id,true);
}


CommExtCache::_CommExtCacheItem * CommExtCache::_CommExtIntCacheTypeMap::_findItem( UINT32 id, bool noAccess)
{
	_CommExtIntCacheMap::iterator it = cacheMap.find(id);
	if (it != cacheMap.end())
	{
		if (noAccess == false && maxNumber && (*it).second->thisId != accessList.back())
		{
			accessList.splice(accessList.end(),accessList, (*it).second->accessListIt);
			(*it).second->accessListIt = --accessList.end();
		}
		return (*it).second;
	}
	else
		return 0;
}


CommExtCache::_CommExtCacheItem * CommExtCache::_CommExtStringCacheTypeMap::_findItem( const char * id, bool noAccess)
{
	_CommExtStringCacheMap::iterator it = cacheMap.find(id);
	if (it != cacheMap.end())
	{
		if (noAccess == false && maxNumber && (*it).second->thisId != accessList.back())
		{
			accessList.splice(accessList.end(),accessList, (*it).second->accessListIt);
			(*it).second->accessListIt = --accessList.end();
		}
		return (*it).second;
	}
	else
		return 0;
}

CommExtCache::CommExtIntIterator CommExtCache::beginInt(const char * type)
{
	PTRACE5("CommExtCache::begin('%s')",type);
	PRLock lock(typeCs);
	_CommExtCacheMaps::iterator im = cacheMaps.find(type);
	if (im == cacheMaps.end())
	{
		PTRACE3("CommExtCache::begin: Object type %s not found",type);
		PASSERT3(0);
	}
	PASSERT3((*im).second->whatType() == IntTypeItem);
	CommExtIntIterator it;
	it.intMap = (_CommExtIntCacheTypeMap *) (*im).second;
	it.it=it.intMap->cacheMap.begin();
	return it;
}

CommExtCache::CommExtStringIterator CommExtCache::beginString(const char * type)
{
	PTRACE5("CommExtCache::begin('%s')",type);
	PRLock lock(typeCs);
	_CommExtCacheMaps::iterator im = cacheMaps.find(type);
	if (im == cacheMaps.end())
	{
		PTRACE3("CommExtCache::begin: Object type %s not found",type);
		PASSERT3(0);
	}
	PASSERT3((*im).second->whatType() == StringTypeItem);
	CommExtStringIterator it;
	it.stringMap = (_CommExtStringCacheTypeMap *) (*im).second;
	it.it=it.stringMap->cacheMap.begin();
	return it;
}

void CommExtCache::_getAllItems(const char * type, vector<UINT32> & ids)
{
	PTRACE5("CommExtCache::_getAllItems('%s')",type);
	PRLock lock(typeCs);
	_CommExtCacheMaps::iterator im = cacheMaps.find(type);
	if (im == cacheMaps.end())
	{
		PTRACE3("CommExtCache::_getAllItems: Object type %s not found",type);
		PASSERT3(0);
	}
	PASSERT3((*im).second->whatType() == IntTypeItem);
	_CommExtIntCacheTypeMap *intMap = (_CommExtIntCacheTypeMap *) (*im).second;
	ids.reserve(intMap->cacheMap.size());
	_CommExtIntCacheMap::iterator it;
	for (it = intMap->cacheMap.begin(); it != intMap->cacheMap.end(); ++it)
	{
		ids.push_back((*it).first);
	}
}

void CommExtCache::_getAllItems(const char * type, vector<PString> & ids)
{
	PTRACE5("CommExtCache::_getAllItems('%s')",type);
	PRLock lock(typeCs);
	_CommExtCacheMaps::iterator im = cacheMaps.find(type);
	if (im == cacheMaps.end())
	{
		PTRACE3("CommExtCache::_getAllItems: Object type %s not found",type);
		PASSERT3(0);
	}
	PASSERT3((*im).second->whatType() == StringTypeItem);
	_CommExtStringCacheTypeMap *strMap = (_CommExtStringCacheTypeMap *) (*im).second;
	ids.reserve(strMap->cacheMap.size());
	_CommExtStringCacheMap::iterator it;
	for (it = strMap->cacheMap.begin(); it != strMap->cacheMap.end(); ++it)
	{
		ids.push_back((*it).first);
	}
}

CommExtCache::_CommExtCacheItem * CommExtCache::_findItem(UINT32 thisId)
{
	PTRACE5("CommExtCache::_findItem(thisId=%08X)",thisId);
	PRLock lock(typeCs);
	_CommExtCacheItem **item = itemsMap.find(thisId);
	if (item)
		return *item;
	else
		return 0;
}





CommExtCache::_CommExtCacheItem * CommExtCache::_addItem(const char * type, UINT32 id)
{
	PTRACE5("CommExtCache::_addItem('%s',%d)",type,id);
	PASSERT3(id != 0);
	PWLock lock(typeCs);
	
	_CommExtCacheMaps::iterator im = cacheMaps.find(type);
	if (im == cacheMaps.end() || (*im).second->whatType() != IntTypeItem)
	{
		PTRACE3("CommExtCache::_addItem: Object type %s not found",type);
		PASSERT3(0);
	}
	return (*im).second->_addItem(itemsMap,id);
}

CommExtCache::_CommExtCacheItem * CommExtCache::_addItem(const char * type, const char * id)
{
	PTRACE5("CommExtCache::_addItem('%s',%s)",type,id);
	PASSERT3(id != 0);
	PWLock lock(typeCs);
	
	_CommExtCacheMaps::iterator im = cacheMaps.find(type);
	if (im == cacheMaps.end() || (*im).second->whatType() != StringTypeItem)
	{
		PTRACE3("CommExtCache::_addItem: Object type %s not found",type);
		PASSERT3(0);
	}
	return (*im).second->_addItem(itemsMap,id);
}



CommExtCache::_CommExtCacheItem * CommExtCache::_CommExtIntCacheTypeMap::_addItem( CommIdMap<_CommExtCacheItem	*> & itemsMap_, UINT32 id)
{

	_CommExtIntCacheMap::iterator itOld = cacheMap.find(id);
	if (itOld != cacheMap.end())
	{
		PTRACE3("CommExtCache::_addItem: Object %s/%d already exists",typeName.c_str(),id);
		PASSERT3(0);
	}

	if (maxNumber && cacheMap.size() > maxNumber)
	{
		list<UINT32>::iterator  it;
		it =accessList.begin();
		while (cacheMap.size() > maxNumber && it != accessList.end())
		{
			UINT32 removeId = *it;
			_CommExtCacheItem **itemToRemove = itemsMap_.find(removeId);
			if (itemToRemove)
			{
				if ((*itemToRemove)->readAccess.size() == 0  && (*itemToRemove)->exclusiveWaiting.size() == 0 && (*itemToRemove)->exclusiveAccess == 0 && (*itemToRemove)->extData == 0)
				{
					PTRACE5("CommExtCache::_addItem('%s',%d): item %08X is removed",typeName.c_str(),id,removeId);
					parent.itemRemoved(typeName,(*((_CommExtCacheIntItem * )(*itemToRemove))->mapIter).first,(*itemToRemove)->obj);
					if ((*itemToRemove)->obj)
						delete (*itemToRemove)->obj;
					it = accessList.erase(it);
					cacheMap.erase(((_CommExtCacheIntItem * )(*itemToRemove))->mapIter);
					delete (*itemToRemove);
					itemsMap_.remove(removeId);
					
				}
				else
				{
					if ((*itemToRemove)->extData == 0)
						PTRACE3("CommExtCache::_addItem('%s',%d): to be removed item %08X is locked: read=%d, wait=%d, exclusive=%08x",typeName.c_str(),id,removeId, 
						         (int) (*itemToRemove)->readAccess.size(),(int)(*itemToRemove)->exclusiveWaiting.size(), (*itemToRemove)->exclusiveAccess);
					else
						PTRACE5("CommExtCache::_addItem('%s',%d): to be removed item %08X is locked by extData", typeName.c_str(), id, removeId);
					++it;
				}
			}
			else
			{
				PTRACE3("CommExtCache::_addItem('%s',%d): to be removed item %08X is not found",typeName.c_str(),id,removeId);
				++it;
			}
		}
	}

	pair<_CommExtIntCacheMap::iterator,bool> res = cacheMap.insert(_CommExtIntCacheMap::value_type(id, new _CommExtCacheIntItem()));
	if (res.second == false)
		PASSERT3(0);
	(*res.first).second->thisId = itemsMap_.add((*res.first).second);
	((_CommExtCacheIntItem * )(*res.first).second)->mapIter = res.first;
	(*res.first).second->mapPtr = this;

	if (maxNumber)
	{
		(*res.first).second->accessListIt = accessList.insert(accessList.end(),(*res.first).second->thisId);
	}
	PTRACE5("CommExtCache::item %08X added",(*res.first).second->thisId);
	return (*res.first).second;
}

CommExtCache::_CommExtCacheItem * CommExtCache::_CommExtStringCacheTypeMap::_addItem( CommIdMap<_CommExtCacheItem	*> & itemsMap_,  const char * id)
{

	_CommExtStringCacheMap::iterator itOld = cacheMap.find(id);
	if (itOld != cacheMap.end())
	{
		PTRACE3("CommExtCache::_addItem: Object %s/%s already exists",typeName.c_str(),id);
		PASSERT3(0);
	}

	if (maxNumber && cacheMap.size() > maxNumber)
	{
		list<UINT32>::iterator  it;
		it =accessList.begin();
		while (cacheMap.size() > maxNumber && it != accessList.end())
		{
			UINT32 removeId = *it;
			_CommExtCacheItem **itemToRemove = itemsMap_.find(removeId);
			if (itemToRemove)
			{
				if ((*itemToRemove)->readAccess.size() == 0  && (*itemToRemove)->exclusiveWaiting.size() == 0 && (*itemToRemove)->exclusiveAccess == 0 && (*itemToRemove)->extData == 0)
				{
					PTRACE5("CommExtCache::_addItem('%s',%s): item %08X is removed",typeName.c_str(),id,removeId);
					parent.itemRemoved(typeName,(*((_CommExtCacheStringItem * )(*itemToRemove))->mapIter).first,(*itemToRemove)->obj);
					if ((*itemToRemove)->obj)
						delete (*itemToRemove)->obj;
					it = accessList.erase(it);
					cacheMap.erase(((_CommExtCacheStringItem * )(*itemToRemove))->mapIter);
					delete (*itemToRemove);
					itemsMap_.remove(removeId);
				}
				else
				{
					if ((*itemToRemove)->extData == 0)
						PTRACE3("CommExtCache::_addItem('%s',%s): to be removed item %08X is locked: read=%d, wait=%d, exclusive=%08x", typeName.c_str(), id, removeId,
						        (int)(*itemToRemove)->readAccess.size(), (int)(*itemToRemove)->exclusiveWaiting.size(), (*itemToRemove)->exclusiveAccess);
					else
						PTRACE5("CommExtCache::_addItem('%s',%s): to be removed item %08X is locked by extData", typeName.c_str(), id, removeId);
					++it;
				}
			}
			else
			{
				PTRACE3("CommExtCache::_addItem('%s',%s): to be removed item %08X is not found",typeName.c_str(),id,removeId);
				++it;
			}
		}
	}

	pair<_CommExtStringCacheMap::iterator,bool> res = cacheMap.insert(_CommExtStringCacheMap::value_type(id, new _CommExtCacheStringItem()));
	if (res.second == false)
		PASSERT3(0);
	(*res.first).second->thisId = itemsMap_.add((*res.first).second);
	((_CommExtCacheStringItem * )(*res.first).second)->mapIter = res.first;
	(*res.first).second->mapPtr = this;

	if (maxNumber)
	{
		(*res.first).second->accessListIt = accessList.insert(accessList.end(),(*res.first).second->thisId);
	}
	PTRACE5("CommExtCache::item %08X added",(*res.first).second->thisId);
	return (*res.first).second;
}


void CommExtCache::_removeItem(const char * type, const char * id)
{
	PTRACE5("CommExtCache::_removeItem('%s',%s)",type,id);
	PASSERT3(id != 0);
	PWLock lock(typeCs);
	
	_CommExtCacheMaps::iterator im = cacheMaps.find(type);
	if (im == cacheMaps.end() || (*im).second->whatType() != StringTypeItem)
	{
		PTRACE3("CommExtCache::_removeItem: Object type %s not found",type);
		PASSERT3(0);
	}
	(*im).second->_removeItem(itemsMap,id);
}

void CommExtCache::_removeItem(const char * type, UINT32 id)
{
	PTRACE5("CommExtCache::_removeItem('%s',%d)",type,id);
	PASSERT3(id != 0);
	PWLock lock(typeCs);
	
	_CommExtCacheMaps::iterator im = cacheMaps.find(type);
	if (im == cacheMaps.end() || (*im).second->whatType() != IntTypeItem)
	{
		PTRACE3("CommExtCache::_removeItem: Object type %s not found",type);
		PASSERT3(0);
	}
	(*im).second->_removeItem(itemsMap,id);
}


void CommExtCache::_CommExtStringCacheTypeMap::_removeItem( CommIdMap<_CommExtCacheItem	*> & itemsMap_, const char * id)
{
	_CommExtStringCacheMap::iterator it = cacheMap.find(id);
	if (it != cacheMap.end())
	{
		_removeItem(itemsMap_,(*it).second);
	}
}

void CommExtCache::_CommExtIntCacheTypeMap::_removeItem( CommIdMap<_CommExtCacheItem	*> & itemsMap_, UINT32 id)
{
	_CommExtIntCacheMap::iterator it = cacheMap.find(id);
	if (it != cacheMap.end())
	{
		_removeItem(itemsMap_,(*it).second);
	}
}

void CommExtCache::_CommExtIntCacheTypeMap::_removeItem( CommIdMap<_CommExtCacheItem	*> & itemsMap_, _CommExtCacheItem *item_)
{
	_CommExtCacheIntItem * item = (_CommExtCacheIntItem *)item_;
	parent.itemRemoved(typeName,item->thisId,item->obj);
	if (maxNumber)
		accessList.erase(item->accessListIt);
	itemsMap_.remove(item->thisId);
	if (item->obj)
		delete item->obj;
	if (item->extData)
		delete item->extData;

	cacheMap.erase(item->mapIter);
	delete item;
}

void CommExtCache::_CommExtStringCacheTypeMap::_removeItem( CommIdMap<_CommExtCacheItem	*> & itemsMap_, _CommExtCacheItem *item_)
{
	_CommExtCacheStringItem * item = (_CommExtCacheStringItem *)item_;
	parent.itemRemoved(typeName,item->thisId,item->obj);
	if (maxNumber)
		accessList.erase(item->accessListIt);
	itemsMap_.remove(item->thisId);
	if (item->obj)
		delete item->obj;
	if (item->extData)
		delete item->extData;
	cacheMap.erase(item->mapIter);
	delete item;
}

void CommExtCache::_removeItem(UINT32 thisId)
{
	PTRACE5("CommExtCache::_removeItem(%08X)",thisId);
	PWLock lock(typeCs);

	_CommExtCacheItem **item = itemsMap.find(thisId);
	if (item)
	{
		(*item)->mapPtr->_removeItem(itemsMap,*item);
	}
}

void CommExtCache::printStat()
{
	PRLock lock(typeCs);
	_CommExtCacheMaps::iterator im;
	for (im = cacheMaps.begin(); im != cacheMaps.end(); ++im)
	{
		PLog("Cache '%s', size %d", (*im).second->typeName.c_str(),  (*im).second->size());
	}
}

void CommExtCache::printStatItem(UINT32 itemId)
{
	PRLock lock(typeCs);
	_CommExtCacheItem *item = _findItem( itemId );
	if (item)
		item->printStat();
	else
		PLog("item printStat: item %08X not found",itemId);
}

UINT32 CommExtCache::objectsInCache(const char * name)
{
	PTRACE5("CommExtCache::objectsInCache('%s')",name);
	PRLock lock(typeCs);
	_CommExtCacheMaps::iterator im = cacheMaps.find(name);
	return (im != cacheMaps.end()) ?  (*im).second->size() : 0;
}

void CommExtCache::_releaseCache(const char * type)
{
	PTRACE5("CommExtCache::_releaseCache('%s')",type);
	PRLock lock(typeCs);
	_CommExtCacheMaps::iterator im = cacheMaps.find(type);
	if (im == cacheMaps.end())
	{
		PTRACE3("CommExtCache::_releaseCache: Object type %s not found",type);
		PASSERT3(0);
	}
	if ( (*im).second->whatType() == StringTypeItem)
	{
		_CommExtStringCacheTypeMap *strMap = (_CommExtStringCacheTypeMap *) (*im).second;
		_CommExtStringCacheMap::iterator it;
		for (it = strMap->cacheMap.begin(); it != strMap->cacheMap.end(); )
		{
			_CommExtCacheStringItem *item = (_CommExtCacheStringItem*) (*it).second;
			PASSERT3(it == item->mapIter);
			if (item->readAccess.size() == 0 && item->exclusiveWaiting.size() == 0 && item->exclusiveAccess == 0 && !item->extData)
			{
				PASSERT3(item->writePtr == COM_EXT_INVALID_PTR);
				strMap->parent.itemRemoved(type,item->thisId,item->obj);
				if (strMap->maxNumber)
					strMap->accessList.erase(item->accessListIt);
				itemsMap.remove(item->thisId);
				if (item->obj)
					delete item->obj;
				if (item->extData)
					delete item->extData;

				it = strMap->cacheMap.erase(it);
				delete item;
			}
			else
				++it;
		}
	}
	else if ( (*im).second->whatType() == IntTypeItem)
	{
		_CommExtIntCacheTypeMap *strMap = (_CommExtIntCacheTypeMap *) (*im).second;
		_CommExtIntCacheMap::iterator it;
		for (it = strMap->cacheMap.begin(); it != strMap->cacheMap.end(); )
		{
			_CommExtCacheIntItem *item = (_CommExtCacheIntItem*) (*it).second;
			PASSERT3(it == item->mapIter);
			if (item->readAccess.size() == 0 && item->exclusiveWaiting.size() == 0 && item->exclusiveAccess == 0 && !item->extData)
			{
				PASSERT3(item->writePtr == COM_EXT_INVALID_PTR);
				strMap->parent.itemRemoved(type,item->thisId,item->obj);
				if (strMap->maxNumber)
					strMap->accessList.erase(item->accessListIt);
				itemsMap.remove(item->thisId);
				if (item->obj)
					delete item->obj;
				if (item->extData)
					delete item->extData;

#if defined (P_MAC10X)
				strMap->cacheMap.erase(it++);
#else
				it = strMap->cacheMap.erase(it);
#endif
				
				delete item;
			}
			else
				++it;
		}
	}
}



void CommExtCache::_CommExtCacheItem::printStat()
{
	if (mapPtr->whatType() == IntTypeItem)
		PLog("###Item=%08X: '%s'/%d ready=%s, obj=%p, writePtr=%p, exclusiveAccess=%08X, extData=%p", thisId, mapPtr->typeName.c_str(), (*((_CommExtCacheIntItem*)this)->mapIter).first, (ready ? "yes":"no"), obj, writePtr, exclusiveAccess, extData);
	else
		PLog("###Item=%08X: '%s'/'%s' ready=%s, obj=%p, writePtr=%p, exclusiveAccess=%08X, extData=%p", thisId, mapPtr->typeName.c_str(), (*((_CommExtCacheStringItem*)this)->mapIter).first, (ready ? "yes":"no"), obj, writePtr, exclusiveAccess, extData);

	if (readAccess.size())
	{
		PString ps;
		map<UINT32,CommExtObject *>::iterator it;
		int i;
		for (i=0,it=readAccess.begin(); it != readAccess.end(); ++it,i++)
		{
			if (!(i%10))
			{
				if (ps.length())
					PLog("%s", ps.c_str());
				ps="####Read access:";
			}
			char buff[30];
			sprintf(buff,"%08X:%p,",(*it).first, (*it).second); 
			ps.append(buff);
		}
		PLog("%s", ps.c_str());
	}
	else
	{
		PLog("####No read access requests");
	}
	if (exclusiveWaiting.size())
	{
		PString ps;
		int i;
		for (i=0; i < exclusiveWaiting.size(); i++)
		{
			if (!(i%10))
			{
				if (ps.length())
					PLog("%s", ps.c_str());
				ps="####Write waiting:";
			}
			ps.appendHexInt(exclusiveWaiting[i]).append(",");
		}
		PLog("%s", ps.c_str());
	}
	else
	{
		PLog("####No write waiting requests");
	}



}






/*

void CommExtCache::addObject(const char * name, UINT32 id, CommExtObject * obj )
{
	_CommExtCacheMaps::iterator im = maps.find(name);
	if (im == maps.end())
	{
		PTRACE3("Object type %s not found",name);
		PASSERT3(0);
	}

	pair<_CommExtCacheMap::iterator,bool> res = (*im).second->cacheMap.insert(_CommExtCacheMap::value_type(id,_CommExtCacheItem((*im).second->accessList)));
	if (res.second == false)
		PASSERT3(0);
	_CommExtCacheItem & item = (*res.first).second;
	item.id = id;
	item.obj = obj;
	item.accessIter = (*im).second->accessList.insert((*im).second->accessList.end(),id);
}



void CommExtCache::removeObject(const char * name, UINT32 id)
{
	_CommExtCacheMaps::iterator im = maps.find(name);
	if (im == maps.end())
	{
		PTRACE3("Object type %s not found",name);
		PASSERT3(0);
	}
	_CommExtCacheMap::iterator it = (*im).second->cacheMap.find(id);
	if (it != (*im).second->cacheMap.end())
	{
		(*im).second->accessList.erase((*it).second.accessIter);
		(*im).second->cacheMap.erase(it);
	}

}
CommExtCache::_CommExtCacheItem * CommExtCache::getObject(const char * name, UINT32 id)
{
	
	_CommExtCacheMaps::iterator im = maps.find(name);
	if (im == maps.end())
	{
		PTRACE3("Object type %s not found",name);
		PASSERT3(0);
	}
	_CommExtCacheItem * item = _findItem((*im).second, id);
	return item;
}
*/


