#ifndef VipStoreInventoryShared_h_included
#define VipStoreInventoryShared_h_included

////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 2009 PYR Software Ltd. All rights reserved.
////////////////////////////////////////////////////////////////////////////////


#include "commlib.h"
#include "pyrtime.h"
#include "dbm.h"
#include "getservconnip.h"
#include "normuserid.h"
#include "i18n.strmap.h"
#include "VipStoreInventorySharedCli.h"
#include "VipStoreMruPtrCache.h"
#include "shared.h"
#include "invlogging.h"
#include "composecolls.h"
#include "invcomposecolls.h"
#include "fppstoreitem.h"

////////////////////////////////////////////////////////////////////////////////

typedef vector<FppItemProperty> InvFppItemPropertyColl;
typedef vector<FppItemProperty::FppValues> InvFppItemPropertyValuesColl;
typedef vector<PString> InvItemPropColl;
typedef vector<InvItemPropColl> InvItemPropCollColl;

#define STOREORDERS_NUMBER_OF_SPECS 4
#define UNKNOWN_PROPERTY_MAPPING "UNK"


////////////////////////////////////////////////////////////////////////////////


class CountryList : public list<PString>
{
private:
	CountryList( const CountryList&);
	const CountryList& operator=( const CountryList&);

public:
	CountryList(){};

	bool findCountry( const char* country) const;
};

////////////////////////////////////////////////////////////////////////////////

enum QuantityLimits 
{ 
	QUANTITY_LIMITED = 0, 
	QUANTITY_UNLIMITED = 1, 
	QUANTITY_DEFAULT = -1
};

template< class T>
class ObjectInstanceOrReference
{
	// prohibit copying
	ObjectInstanceOrReference( const ObjectInstanceOrReference<T>&);
	const ObjectInstanceOrReference& operator=(const ObjectInstanceOrReference<T>&);

public:
	ObjectInstanceOrReference() : objRef( objInst) {}
	ObjectInstanceOrReference( T& objRef_) : objRef( objRef_){}
	ObjectInstanceOrReference( const T& objRef_) : objRef( objRef_){}
	
	// const reference returns a reference to objRef which was either initialized
	// from the outside object or from the objInst
	//
	operator const T& () const {return objRef;}

	// mutable reference returns a reference to an objInst because we never want to
	// modify anything outside our object
	//
	T& getInst() { return objInst;}

private:
	T  objInst;
	const T& objRef;
};

class InvWarehouse
{

	// prohibit copy constructor
	//
	InvWarehouse( const InvWarehouse&);
	const InvWarehouse& operator=(const InvWarehouse&);
private:
	InvWarehouse() 
		: id(0), name(""), description("")
		, stocksThisItem(false)
		, unlimitedPossible(false)
//		, unlimitedDefault(?) //@@@ AlexN need intitialization of enum
//		, quantityLimits(?)  //@@@ AlexN need intitialization of enum
	{  }

public:
	
	InvWarehouse(	UINT32 id_, 
					const PString& name_, 
					const PString& description_, 
					const PString& officeName_,
					bool isUsed_, 
					bool isUnlimitedPossible_,
					QuantityLimits unlimitedDefault_,
					QuantityLimits quantityLimits_ );

	InvWarehouse(	UINT32 id_, 
					const PString& name_, 
					const PString& description_,
					const PString& officeName_,
					bool isUsed_, 
					bool isUnlimitedPossible_,
					QuantityLimits unlimitedDefault_,
					QuantityLimits quantityLimits_,
					const CountryList& cachedDefaultCountries_);
public:
	
	const CountryList& getDefaultCountries() const	{return defaultCountries;}
	const CountryList& getCustomCountries()	const	{return customCountries;}

	// warehouse stock the specified item
	//
	bool  isUsed() const { return stocksThisItem != 0;}
	
	// warehouse allows to stock unlimited items - this is usually the case for
	// vendors but we treat them as warehouses all the same
	//
	bool  isUnlimitedPossible() const { return unlimitedPossible != 0;}

	// what is the default value for the unlimited (1 or 0) - I may make it a bool
	//
	QuantityLimits  getUnlimitedDefault() const { return unlimitedDefault;}

	// when we specify that the item will be stocked by this warehouse we
	// specify if the warehouse will actually stock unlimited items
	// this is not a bool for now because there still may be a possibility in
	// how limited/unlimited/default would be handled
	//
	QuantityLimits  getQuantityLimits() const { return quantityLimits;}

	// tell this warehouse whether to stock this item or not
	// use QUANTITY_LIMITED = 0 or QUANTITY_UNLIMITED = 1 values
	//
	void  setStockInfo( bool stocksThisItem_, QuantityLimits quantityLimits_)
	{
		stocksThisItem = stocksThisItem_;

		// if we're somehow instructed to use default - get the default value
		// and assign it to the quantityLimits
		//
		if( QUANTITY_DEFAULT == quantityLimits)
		{
			quantityLimits = getUnlimitedDefault();
		}
		else if( QUANTITY_LIMITED == quantityLimits_ || isUnlimitedPossible())
		{
			quantityLimits = quantityLimits_;
		}
		else
		{
			PLog("Attempted to set Unlimited to (%d) for warehouse (%u) Possible is (%s). Ignoring.",
				quantityLimits_, 
				id, 
				( isUnlimitedPossible() ? "true" : "false"));
		}
	}

protected:
	// for use in 
	void addDefaultCountry( const PString& country);
	void addCustomCountry( const PString& country);
	void clearCustomCountries() { customCountries.clear();}
	
	// clear the data contained by the instance
	// inside ObjectInstanceOrReference - not in the
	// reference, so if someone by mistake called clear
	// on an object that refers to a shared cached
	// copy of the default countries, the shared
	// copy will not be harmed
	//
	void clearDefaultCountries(){ defaultCountries.getInst().clear();} 

public:

	const UINT32  id;
	const PString name;
	const PString description;
	const PString officeName;
	
private:

	bool stocksThisItem;
	bool unlimitedPossible;
	QuantityLimits unlimitedDefault;
	QuantityLimits quantityLimits;
	
	static void addCountry( CountryList& cl, const PString& c);

	ObjectInstanceOrReference<CountryList> defaultCountries;
	CountryList customCountries;
};

class InvWarehouseInternal : public InvWarehouse
{
public:
	InvWarehouseInternal(	UINT32 id_, 
							const PString& name_, 
							const PString& description_,
							const PString& officeName_,
							bool isUsed_, 
							bool isUnlimitedPossible_,
							QuantityLimits unlimitedDefault_,
							QuantityLimits quantityLimits_ )
							:
							InvWarehouse(	id_,
											name_,
											description_,
											officeName_,
											isUsed_,
											isUnlimitedPossible_,
											unlimitedDefault_,
											quantityLimits_){};
	InvWarehouseInternal(	UINT32 id_, 
							const PString& name_, 
							const PString& description_,
							const PString& officeName_,
							bool isUsed_, 
							bool isUnlimitedPossible_,
							QuantityLimits unlimitedDefault_,
							QuantityLimits quantityLimits_,
							const CountryList& cachedDefaultCountries_)
							:
							InvWarehouse(	id_,
											name_,
											description_,
											officeName_,
											isUsed_,
											isUnlimitedPossible_,
											unlimitedDefault_,
											quantityLimits_,
											cachedDefaultCountries_){};

	using InvWarehouse::addDefaultCountry;
	using InvWarehouse::addCustomCountry;
	using InvWarehouse::clearCustomCountries;
	using InvWarehouse::clearDefaultCountries;
};
////////////////////////////////////////////////////////////////////////////////

class InvWarehouseVector
{

	InvWarehouseVector( const InvWarehouseVector&) = delete;
	InvWarehouseVector& operator=(const InvWarehouseVector&) = delete;
public:

	class IProcessWarehouse
	{
	public:
		virtual ~IProcessWarehouse(){}
		virtual void processWarehouse( const InvWarehouse& w) = 0;
	};

	InvWarehouseVector(){};
	~InvWarehouseVector();
	
	InvWarehouseInternal* getWarehouse(UINT32 warehouseId) const;
		
	void compose(CommMsgBody& body) const;
	void parse(CommMsgParser& parser);
	
	void forEachWarehouse( IProcessWarehouse& pwh) const;

	void print() const;

protected:

	void addWarehouse(	UINT32 id, 
						const PString& name, 
						const PString& descr,
						const PString& officeName,
						bool isUsed, 
						bool isUnlimitedPossible, 
						QuantityLimits unlimitedDefault, 
						QuantityLimits quantityLimits);
	
	void addWarehouse(	UINT32 id, 
						const PString& name, 
						const PString& descr,
						const PString& officeName,
						bool isUsed, 
						bool isUnlimitedPossible, 
						QuantityLimits unlimitedDefault, 
						QuantityLimits quantityLimits,
						const CountryList& defaultCountries);

	void clearCustomRoutes();
	void clearDefaultRoutes();

private:
	typedef map<UINT32,InvWarehouseInternal*>	WarehousesMap_;
	WarehousesMap_								warehouses;
};

class InvWarehouseVectorInternal : public InvWarehouseVector
{
public:
	using InvWarehouseVector::addWarehouse;
	using InvWarehouseVector::clearCustomRoutes;
	using InvWarehouseVector::clearDefaultRoutes;
};

//////////////////////////////////////////////////////////////////////////
class InvDefaultRouting;

class InvItemRouting
{
	// prohibit copy
	InvItemRouting( const InvItemRouting&);
	const InvItemRouting& operator=(const InvItemRouting&);

public:
	// define public constant for active revision number
	//
	static const UINT32 activeRoutingRevision = 0;
public:
	
	// interfaces for enumerators
	// 'process' functions are not const as they may change the state of
	// the object that inherits these interfaces
	//
	class IProcessRoute
	{
	public:
		virtual ~IProcessRoute(){}
		virtual void processRoute( UINT32 warehouseId, const PString& country) = 0;
	};

	// used by the client - set composeDefaultRouting to false so the client
	// does not send default routing to the server since this data does not
	// get changed on the client and is not saved to the database
	//
	InvItemRouting(CommMsgParser& parser) :	composeDefaultRouting(false),
											custRevNum(0),
											itemId(UINT32(-1)) { parse( parser);}

	// get item id of the constructed item
	//
	UINT32 getItemId() const { return itemId;}

	// Route item to a specified country. 
	// Returns a warehouse ID where the item will be routed from.
	// If returns ALL_WAREHOUSES - the route to the specified country does not exist
	// for NULL value in the STOREITEMS.COUNTRIES use ALL_COUNTRIES define
	UINT32 route( const PString& country) const;

	// get the intersection of country sets - those where the item needs to ship 
	// and those where the warehouse with the specified id does ship
	//
	// it is to be used by whoever wants to determine which custom routes make sense
	// for this item i.e. we do not need routes that deliver to places where the item does not
	// need to be shipped (shown) and we do not need routes to countries where the warehouse
	// cannot ship either
	//
	void getPossibleCustomRoutesForWarehouse(	UINT32 warehouseId,
												CountryList& possibleCustomRoutes, 
												const CountryList& showInCountries) const;
	
	// get a list of unresolved default and custom conflicts - basically a list of countries
	// that are associated with more than one warehouse for a given item
	//
	void getUnresolvedDefaultConflicts( CountryList& unresolvedConflicts) const;
	void getUnresolvedCustomConflicts( CountryList& unresolvedConflicts) const;

	// chose this interface to mirror the UI - custom countries are assigned
	// on a warehouse basis and are not added or deleted, instead a certain set
	// of custom countries will be applied and as a result discard old custom
	// countries for the warehouse and assign new ones
	//
	// IMPORTANT NOTES: 
	//
	// 1. before making a series of calls for each warehouse the client
	//    absolutely MUST call clearCustomRoutes.
	//
	// 2. after this function was called - check if there are any unresolved
	//    custom conflicts
	//
	// 3. this function must be called for each warehouse for which there's custom routing
	//    being set up.
	//
	// 4. If a warehouse does not stock an item, then the route will not be set for that
	//    country+warehouse pair
	//
	// 5. Even if default routing is not set up for country X right now we allow creating a
	//    custom route to this country 
	//
	//
	UINT32 setCustomCountries( UINT32 warehouseId, const vector<PString>& countries);

	// clear custom routes from warehouses and a custom warehouse by country map
	//
	void clearCustomRoutes();

	// sets if a warehouse is used and if it uses unlimited stock
	//
	// IMPORTANT: after this function was called - check if there are any unresolved
	//			  custom conflicts
	//
	void setWarehouseStockInfo( UINT32 warehouseId, bool isUsed, bool unlimited);

	// allow clients to retrieve custom routes - primarily for updating the database
	//
	void forEachCustomRoute( IProcessRoute& ipr) const;

	// provide enumerator for warehouses to allow clients decide which
	// data they really need from the warehouse instead of me packaging
	// something for them
	//
	void forEachWarehouse( InvWarehouseVector::IProcessWarehouse& w) const;

	// serialization
	void compose(CommMsgBody& body) const;
	void parse(CommMsgParser& parser);

	// simple checks for getting/setting items
	//
	bool isSettableItem() const { return itemId > ALL_ITEMS;}

	// debug
	void print() const;
	
	// get custom routing revision number
	//
	UINT32 getCustRevNum()				const { return custRevNum;}
	UINT32 getRequestedRevNum()			const { return requestedRevNum;}
	UINT32 getLastActiveRoutingRevNum() const {	return lastActiveRoutingRevNum; }
	bool   isActiveRoutingRevision()	const { return requestedRevNum == activeRoutingRevision; }
	

protected:

	// used by the server - set composeDefaultRouting to true as the client
	// needs to receive it
	//
	// also uses cached default routing by using a reference to warehouse by
	// country map in the passed defaultRouting object
	//
	InvItemRouting(	UINT32 itemId_, 
		UINT32 revNum,
		const InvItemRouting& defaultRouting) 
		: 
		composeDefaultRouting(true),
		requestedRevNum(revNum),
		custRevNum(0),
		lastActiveRoutingRevNum( 0),
		itemId(itemId_),
		// initialize defaultWarehouseByCountry as a const reference to
		// an existing cached instance of the default warehouse by country map
		//
		defaultWarehouseByCountry(
		defaultRouting.
		defaultWarehouseByCountry.
		operator const WarehouseByCountryMap_&())
	{
	}
	// constructor for derived classes that do not use
	// cached default routing - i.e. InvDefaultRouting which
	// needs an actual container to store default routing to which
	// then the rest of the non-default routing objects will refer
	//
	// used by the server - set composeDefaultRouting to true as the client
	// needs to receive it
	//
	InvItemRouting(	UINT32 itemId_, 
					UINT32 revNum)
					: 
					composeDefaultRouting(true),
					requestedRevNum(revNum),
					custRevNum(0),
					lastActiveRoutingRevNum( 0),
					itemId(itemId_)
	{
	}

	// constructor for derived classes that actually do want to send
	// default routing back like InvDefaultRouting
	//
	// used by the client to construct an object from the parser and
	// send modified default routing to the server
	//
	InvItemRouting(	CommMsgParser& parser, 
					bool composeDefaultRouting_) 
					:	
					composeDefaultRouting(composeDefaultRouting_),
					custRevNum(0),
					itemId(UINT32(-1))
	{ 
		parse( parser);
	}

	// warehouses must be added first for addRoute to work correctly
	// we just get a list of _all_warehouses in the database and feed it
	// to this class
	//
	void addWarehouse(	UINT32 id, 
						const PString& name, 
						const PString& descr, 
						const PString& officeName,
						bool isUsed, 
						bool isUnlimitedPossible, 
						QuantityLimits unlimitedDefault, 
						QuantityLimits quantityLimits);

	// allows to specify itemId which would be used during de-serialization
	// where itemId would be ALL_ITEMS - for default routes
	//
	bool addRawRoute( UINT32 warehouseId, const PString& country, UINT32 rawItemId);

	// custom route revision number - every time custom routing changes for any
	// item, this number gets incremented by 1 i.e. it's not necessarily consecutive for every
	// specific item but it is consecutive in the sense that it reflects an order in which
	// custom routing was changed for items
	//
	void setCustRevNum		( UINT32 custRevNum_) { custRevNum = custRevNum_;}

	// when the object is about to be populated with routing information - this
	// value is set to either activeRoutingRevision to get routing data from the
	// active routing table or to some number to get routing data
	// from INVROUTREVDATA
	//
	void setRequestedRevNum	( UINT32 revNum_	) { requestedRevNum = revNum_;}

	void forEachDefaultRoute( IProcessRoute& ipr) const;
	
	InvWarehouseVectorInternal& getWarehousesVectorInternal() 
	{ 
		return warehouses;
	}
	void setLastActiveRoutingRevNum( UINT32 lastActiveRoutingRevNum_)
	{
		lastActiveRoutingRevNum = lastActiveRoutingRevNum_;
	}
	InvWarehouseInternal* getWarehouse(UINT32 warehouseId) const
	{
		return warehouses.getWarehouse( warehouseId);
	}
	void clearDefaultRoutes()
	{
		// need to cast object wrapper to the mutable reference
		// in order to access collection mutation methods
		//
		defaultWarehouseByCountry.getInst().clear();

		warehouses.clearDefaultRoutes();
	}

private:

	// WarehouseColl_ vector would generally be quite small so storing it
	// inside a map by value should be be a big deal especially because
	// the whole routing admin functionality will be used very rarely
	//
	typedef vector<UINT32> WarehouseColl_;
	typedef PStringMap<WarehouseColl_> WarehouseByCountryMap_;
	
	// routing serialization
	//
	void composeRouting(CommMsgBody& body, const WarehouseByCountryMap_& wcm) const;
	void parseRouting(CommMsgParser& parser, UINT32 itemId);
	
	// adds a warehouse to a collection of warehouses associated with a particular country in the map
	// made static because it does not use any instance members
	//
	static void addWarehouseToCountryMap(	UINT32 warehouseId, 
											const PString& country, 
											WarehouseByCountryMap_& warehouseByCountry);

	// helper function for getUnresolvedXXXXXConflicts
	// uses warehouses otherwise would have been a static
	//
	void getConflicts( const WarehouseByCountryMap_& wcm, CountryList& conflicts) const;

	// helper function for iterating through either custom or default maps
	//
	void forEachRouteInTheMap( const WarehouseByCountryMap_& wcm, IProcessRoute& ipr) const;

	// called only after warehouses were added
	//
	bool addCustomRoute( UINT32 warehouseId, const PString& country);

	// find a route in a particular map - helper for the route() function
	//
	UINT32 routeThroughMap( const PString& country, const WarehouseByCountryMap_& wcm) const;

	// functor to calculate number of warehouse/country pairs in a map to be used in for_each
	// must be a member class since it uses private data types and is not exposed to the
	// outside world
	//
	class CountRoutesFunctor
	{
		UINT32 numRoutes;
	public:
		CountRoutesFunctor() : numRoutes(0){};

		UINT32 getNumRoutes() const { return numRoutes;}

		void operator()( const WarehouseByCountryMap_::value_type& wcm_val)
		{
			numRoutes += wcm_val.second.size();
		}
	};

public:
	PString	auditComment;
private:
	
	//data

	// when client calls setCustomCountries set this to true
	// so we knew not to serialize default routing as the data is going
	// back to the server and the server does not need it
	//
	bool						composeDefaultRouting;
	UINT32						requestedRevNum;
	UINT32						custRevNum;
	UINT32						lastActiveRoutingRevNum;

	UINT32						itemId;
	WarehouseByCountryMap_		customWarehouseByCountry;

	ObjectInstanceOrReference<WarehouseByCountryMap_> defaultWarehouseByCountry;
	InvWarehouseVectorInternal	warehouses;
};

////////////////////////////////////////////////////////////////////////////////

// expose addWarehouse and addRawRoute for some very specific clients
// like CRM DBM which needs to construct the object from scratch and 
// needs to call these functions
//
class InvItemRoutingInternal : public InvItemRouting
{
public:
	// use default route caching
	//
	InvItemRoutingInternal( UINT32	itemId_, 
							UINT32	revNum_,
							bool	allWarehouses_,
							const	InvItemRouting& defaultRouting_) 
							:	
							InvItemRouting( itemId_, 
											revNum_,
											defaultRouting_), 
							allWarehouses( allWarehouses_),
							cached( false)
	{
	}
	// do not use default route caching
	//
	InvItemRoutingInternal( UINT32	itemId_, 
							UINT32	revNum_,
							bool	allWarehouses_) 
							:	
							InvItemRouting( itemId_, 
											revNum_), 
							allWarehouses( allWarehouses_),
							cached( false)
	{
	}

	using InvItemRouting::addWarehouse;
	using InvItemRouting::addRawRoute;
	using InvItemRouting::setCustRevNum;
	using InvItemRouting::getWarehousesVectorInternal;
	using InvItemRouting::setLastActiveRoutingRevNum;

	bool isAllWarehouses()	const { return allWarehouses;}
	bool isCached()			const { return cached;}
	
	void setCached( bool cached_)
	{
		cached = cached_;
	}

private:
	const bool allWarehouses;
	bool cached;
};

// a variation of a smart pointer for an internal routing instance
// the instance will be freed upon destruct or reassignment
// if it is not a cached instance
//
class InvItemRoutingInternalHolder
{
	const InvItemRoutingInternal* p;

	void clear()
	{
		if( p != NULL && !p->isCached())
		{
			delete p;
		}
		p = NULL;
	}
	const InvItemRoutingInternal* getObject() const
	{
		if( p == NULL)
		{
			PLog("InvItemRoutingInternalHolder is NULL");
		}
		return p;
	}

public:
	InvItemRoutingInternalHolder() : p(NULL)
	{
	}
	~InvItemRoutingInternalHolder()
	{
		clear();
	}
	operator const InvItemRoutingInternal* () const
	{
		return getObject();
	}
	void attach( const InvItemRoutingInternal* pCachedOrNot)
	{
		clear();

		p = pCachedOrNot;
	}
	const InvItemRoutingInternal* detach()
	{
		const InvItemRoutingInternal* retVal = p;
		
		p = NULL;

		return retVal;
	}
	
private:
	InvItemRoutingInternalHolder( const InvItemRoutingInternalHolder& other );
	InvItemRoutingInternalHolder& operator =( const InvItemRoutingInternalHolder& other );
};

//////////////////////////////////////////////////////////////////////////

// this class is meant to be used by whoever wants to change default routing
// all it does is hides functions which are not needed for default routing
// and adds a couple of functions dealing with setting and clearing default routes
// so that the admin client would be able to work with it and not cause too much trouble
//
class InvDefaultRouting : public InvItemRouting
{
public:
	InvDefaultRouting(UINT32 revNum) :	InvItemRouting( ALL_ITEMS, revNum) {}

	// while constructing default routing object from the parser - tell the
	// underlying routing object to send default routing information back to the
	// server - use protected constructor and specify second parameter as true
	//
	InvDefaultRouting( CommMsgParser& parser ) : InvItemRouting(parser, true){}
	
	UINT32 setDefaultCountries( UINT32 warehouseId, const vector<PString>& countries);
	
	using InvItemRouting::clearDefaultRoutes;
	using InvItemRouting::forEachDefaultRoute;
	using InvItemRouting::getWarehousesVectorInternal;
	using InvItemRouting::getWarehouse;

private:
	using InvItemRouting::getItemId;
	using InvItemRouting::route;
	using InvItemRouting::getPossibleCustomRoutesForWarehouse;
	using InvItemRouting::getUnresolvedDefaultConflicts;
	using InvItemRouting::getUnresolvedCustomConflicts;
	using InvItemRouting::setCustomCountries;
	using InvItemRouting::clearCustomRoutes;
	using InvItemRouting::setWarehouseStockInfo;
	using InvItemRouting::forEachCustomRoute;
	using InvItemRouting::isSettableItem;
	using InvItemRouting::getCustRevNum;
};

class InvDefaultRoutingInternal : public InvDefaultRouting
{
public:
	InvDefaultRoutingInternal(UINT32 revNum) : 
	  InvDefaultRouting( revNum) {}

	InvDefaultRoutingInternal( CommMsgParser& parser ) : InvDefaultRouting( parser) {}

	using InvItemRouting::addRawRoute;
	using InvItemRouting::setLastActiveRoutingRevNum;
	using InvItemRouting::setRequestedRevNum;
};

//////////////////////////////////////////////////////////////////////////


class InvRoutingCache : public MruPtrCache< UINT32, InvItemRoutingInternal>
{
public:
	static const UINT32 DEFAULT_CACHE_SIZE = 1000;

	InvRoutingCache( UINT32 cacheSize_ = DEFAULT_CACHE_SIZE) : 
												MruPtrCache( cacheSize_), 
												lastActiveRoutingRevNum(0),
												cachedLastActiveRevNum(false),
												defaultRouting(NULL)
	{
	}
	~InvRoutingCache()
	{
		clear();
	}
	void clear()
	{
		// clear cache
		//
		purge();

		// clear cached last active revision number for default routing
		//
		cachedLastActiveRevNum  = false;
		lastActiveRoutingRevNum = 0;

		clearDefaultRouting();
	}

	void setLastActiveRoutingRevNum( UINT32 revNum) 
	{ 
		// cache the value and make a note that we cached it
		//
		lastActiveRoutingRevNum = revNum;
		cachedLastActiveRevNum  = true;
	}
	UINT32 getLastActiveRoutingRevNum() const 
	{ 
		return lastActiveRoutingRevNum;
	}
	bool isCachedLastActiveRevNum() const 
	{ 
		return cachedLastActiveRevNum;
	}
	bool isCachedDefaultRouting( const InvDefaultRoutingInternal*& p) const 
	{
		p = NULL;

		if( defaultRouting != NULL)
		{
			p = defaultRouting;
			return true;
		}
		return false;
	}
	void cacheDefaultRouting( const InvDefaultRoutingInternal* p)
	{
		clearDefaultRouting();

		defaultRouting = p;
	}

private:

	void clearDefaultRouting()
	{
		if( defaultRouting != NULL)
		{
			delete defaultRouting;
			defaultRouting = NULL;
		}
	}

private:

	// hide purge() since we provide our own version of 
	// clear() which also clears cached default routing
	// 
	CacheObjectType_::purge;

private:

	UINT32	lastActiveRoutingRevNum;
	bool	cachedLastActiveRevNum;
	
	const InvDefaultRoutingInternal* defaultRouting;

};
//////////////////////////////////////////////////////////////////////////
// a lot of times we need warehouse information which must be consumed 
// in a form of vectors - for example to construct different statements
// the following class caches relevant warehouse information in the form
// that is convenient for consumption
//
class WarehousesData : public InvWarehouseVector::IProcessWarehouse
{
	// prohibit copy constructor
	//
	WarehousesData( const WarehousesData&) = delete;
	const WarehousesData& operator=(const WarehousesData&) = delete;

public:
	WarehousesData(){}
	~WarehousesData(){}

	void processWarehouse( const InvWarehouse& w)
	{
		processRawData( w.id, w.isUsed(), w.name, w.getQuantityLimits());
	}

	void processRawData( UINT32 warehouseId, bool used, const PString& warehouseName, QuantityLimits unlimited)
	{

		allWarehouseNames[warehouseId] = warehouseName;

		allWarehouses.push_back(warehouseId);

		if( used)
		{
			usedWarehouses.push_back(warehouseId);
			unlimitedForUsed.push_back( unlimited == QUANTITY_UNLIMITED ? true : false);
		}
	}

	const vector<bool>&	getUnlimitedForUsed() const { return unlimitedForUsed;}
	const vector<UINT32>& getUsedWarehouses() const { return usedWarehouses;}
	const vector<UINT32>& getAllWarehouses () const { return allWarehouses;}

	typedef map<UINT32,PString> WarehouseNamesMap_;

	const WarehouseNamesMap_& getAllWarehouseNames() const { return allWarehouseNames;}

private:
	vector<UINT32>		usedWarehouses;
	vector<bool>		unlimitedForUsed;
	vector<UINT32>		allWarehouses;
	WarehouseNamesMap_	allWarehouseNames;
};

//////////////////////////////////////////////////////////////////////////

class InvNotificationEmail
{
public:
	InvNotificationEmail(	UINT32 emailId_, 
							const PString& email_,
							const PString& name_,
							bool  office_,
							bool  on_) :	emailId( emailId_), 
											email( email_),
											name( name_),
											office( office_),
											on( on_){};

public:
	const UINT32 emailId;
	const PString email;
	const PString name;

	bool isOffice() const { return office;}
	bool isOn() const { return on;}

	void setOn( bool on_)
	{
		if( !isOffice())
		{
			on = on_;
		}
	}
	void print();

private:
	bool office;
	bool on;
};

//////////////////////////////////////////////////////////////////////////

class InvGetWarehouseStockItemParams
{
public:
	InvGetWarehouseStockItemParams( UINT32 swiid_, bool skipNotifications_ = false) :
	swiid( swiid_), skipNotifications( skipNotifications_)
	{
	}

	InvGetWarehouseStockItemParams( CommMsgParser& parser) : swiid(0), skipNotifications( false)
	{
		parser.parseUINT32(swiid)
			  .parseBOOL(skipNotifications);
	}

	void compose(CommMsgBody& body) const
	{
		body.composeUINT32(swiid)
			.composeBOOL(skipNotifications);
	}

	UINT32	getSWIID() const { return swiid;}
	bool	isSkippingNotifications() const { return skipNotifications;}

private:
	UINT32	swiid;
	bool	skipNotifications;
};

class InvWarehouseStockItem
{
	static const char* const AUDIT_COMMENT_CHECK_OUT;
	static const char* const AUDIT_COMMENT_UN_CHECK_OUT;
	static const char* const AUDIT_COMMENT_PURCHASE;
	static const char* const AUDIT_COMMENT_SHIP;
	static const char* const AUDIT_COMMENT_CANCEL;
	static const char* const AUDIT_COMMENT_MOVED_OUT;
	static const char* const AUDIT_COMMENT_MOVED_IN;
	static const char* const AUDIT_COMMENT_MOVED_IN_FROM_NOWHERE;
	static const char* const AUDIT_COMMENT_WAREHOUSE_UPDATE_ABS;
	static const char* const AUDIT_COMMENT_WAREHOUSE_UPDATE_INC;
	static const char* const AUDIT_COMMENT_WAREHOUSE_UPDATE_DEC; // PYR-20293
	static const char* const AUDIT_COMMENT_ADMIN_UPDATE;

public:
	InvWarehouseStockItem( bool skipNotifications_);

	InvWarehouseStockItem( CommMsgParser& parser);
	
public:
	UINT32  getNumModified()		const { return numModified;}
	UINT32	getSWIID()				const { return swiid;}
	UINT32	getItemId()				const { return itemId;}
	UINT32	getWarehouseId()		const { return warehouseId;}
	INT32	getNumAvailable()		const { return numAvailable;}
	UINT32	getNumOrdered()			const { return numOrdered;}
	UINT32	getNumReserved()		const { return numReserved;}
	UINT32	getNumCheckedOut()		const { return numCheckedOut;}
	UINT32	getNumWarehouse()		const { return numWhse;}
	UINT32	getMonetaryValue()		const { return monetaryValue;}
	UINT32	getLowAlert1()			const { return lowAlert1;}
	UINT32	getLowAlert2()			const { return lowAlert2;}
	UINT32	getFppPrice()			const { return fppPrice;}
	UINT32	getAuditChangeType()	const { return auditChangeType;}

	bool	isOutOfStock()			const { return outOfStock;}
	bool	isOutOfStockOverride()	const { return outOfStockOverride;}
	bool	isUnlimited()			const { return unlimited;}
	bool	isUnlimitedPossible()	const { return unlimitedPossible;}
	bool	isReservable()			const { return canReserve;}
	bool	isSkippingNotifications() const { return skipNotifications;}
	bool	isAuditCommentPresent()	const { return auditComment.length() > 0;}
	bool	isLowUnitAlertNeeded()	const;

	const char* getStockId()		const { return stockId.c_str();}
	const char* getWarehouseName()	const { return warehouseName.c_str();}
	const char* getOfficeName()		const { return officeName.c_str();}
	const char* getItemDescription()const { return itemDescription.c_str();}
	const char* getAuditComment()	const { return auditComment.c_str();}

	typedef PNewPtrVector<InvNotificationEmail> NotificationsColl_;

	const InvItemSpecsColl& getSpecs() const { return specs;}
	const NotificationsColl_& getNotifications() const { return notifications;}
	
	void compose(CommMsgBody& body) const;

	// does not allow to change check-out as it is meant to be only used in the admin
	//
	void modifyStockItem(	const PString& auditComment_,
							INT32	numAvailable_,
							UINT32	numReserved_,
							UINT32	monetaryValue_,
							UINT32	lowAlert1_,
							UINT32	lowAlert2_,
							bool	outOfStock_,
							bool	outOfStockOverride_,
							bool	unlimited_,
							bool	canReserve_)
	{
		auditComment		= auditComment_;
		auditChangeType		= eInvAuditChange_Unknown;
		monetaryValue		= monetaryValue_;
		lowAlert1			= lowAlert1_;
		lowAlert2			= lowAlert2_;
		outOfStock			= outOfStock_;
		outOfStockOverride	= outOfStockOverride_;
		canReserve			= canReserve_;

		if( unlimited_ && !unlimitedPossible)
		{
			PLog("Attempted to set Unlimited On in a warehouse that does not support Unlimited stock. Leaving Unlimited unchanged.");
		}
		else
		{
			unlimited = unlimited_;
		}

		if( numAvailable_ >= 0 || outOfStockOverride_)
		{
			numAvailable = numAvailable_;
		}
		else
		{
			PLog("Attempted to set Available to a negative value when OutOfStockOverride is not on. Leaving numAvailable unchanged.");
		}
		
		if( canReserve_)
		{
			// numReserved_ cannot be < 0 because its unsigned
			// no need to check for that
			//
			numReserved	= numReserved_;
		}
		else
		{
			// if someone has just prohibited reserving the item -
			// move items that have been reserved to the available pool
			//
			if( numReserved > 0)
			{
				numAvailable += numReserved;
				numReserved = 0;
			}

			// if someone tried to reserve items while prohibiting it at the same time
			// log a message and simply ignore it
			//
			if( numReserved_ > 0)
			{
				PLog("InvWarehouseStockItem::modifyStockItem attempted to reserve" 
					"(%d) for an item (%u) that cannot be reserved.", 
					itemId, numReserved_);
			}
		}


		skipSerializingForServer = true;
	}

	enum ENotificationChangeResult
	{
		NotFound = -1,
		CannotModifyOffice = 0,
		Modified = 1,
	};
	ENotificationChangeResult changeNotification( UINT32 emailId_, bool on_);

	// clears only non-office notifications
	//
	void clearNotifications();

	void print();

	// this is just a convenience for marking audit comments in a special way
	// we already have a user id of the user who made a change but we already
	// have quite substantial amount of information in the audit comment for
	// every sort of occasion having an extra hint for an admin would not hurt
	// as we will hot nave many of those entries anyway compared to the amount of
	// purchase records in the audit table, but it would be a useful hint for the
	// admin if it just wants to know if a change was done by the CSR and it 
	// does not matter which one
	//
	void prefixAdminAuditComment();

protected:
	void addNotification(	UINT32 emailId_, 
							const PString& email_,
							const PString& name_,
							bool on_,
							bool office_);

	void init(	UINT32 swiid_,
				UINT32 numModified_, 
				UINT32 itemId_, 
				UINT32 warehouseId_, 
				INT32  numAvailable_, 
				UINT32 numOrdered_, 
				UINT32 numReserved_,
				UINT32 numCheckedOut_,
				UINT32 numWhse_,
				UINT32 monetaryValue_, 
				UINT32 lowAlert1_, 
				UINT32 lowAlert2_, 
				UINT32 fppPrice_, 
				bool outOfStock_, 
				bool outOfStockOverride_, 
				bool unlimited_,
				bool unlimitedPossible_,
				bool canReserve_, 
				const char* stockId_, 
				const char* warehouseName_, 
				const char* officeName_, 
				const char* itemDescription_, 
				const vector<const char*>& specsColl_);

	// used only by the server
	//
	bool isAvailableForSale( bool useReservedPool) const;
	bool checkOutItem( bool useReservedPool, bool& fromReserved);
	bool checkOutItem( bool useReservedPool, vector<bool>& fromReserved, bool& allAsRequested, UINT32 numItems);
	void unCheckOutItem( bool useReservedPool);
	void unCheckOutItem( const vector<bool>& useReservedPool);
	bool purchaseItem();
	bool cancelOrder( const PString& auditComment_, const bool decrementOrderCounter, bool returnToStock);
	bool ship();
	bool moveOrderFrom( InvWarehouseStockItem& old, 
						bool orderedFromReserved, 
						bool useReservedPool, 
						bool& fromReserved);
	
	// for now this function is trivial and makes no decisions so it does not
	// need to return a status like all previous business logic functions
	//
	void updateWarehouseQuantity(UINT32 newWhseQuantity, bool isIncrement);

	void updateWarehouseQuantityDec(UINT32 adjustment); // PYR-20293

	bool isInitialized() const { return initialized;}

	enum eInventoryAuditChangeType
	{
		eInvAuditChange_Unknown				= 0,
		eInvAuditChange_Checkout			= 1,
		eInvAuditChange_UnCheckout			= 2,
		eInvAuditChange_Purchase			= 3,
		eInvAuditChange_CancelPurchase		= 4,
		eInvAuditChange_Ship				= 5,
		eInvAuditChange_MovedOut			= 6,
		eInvAuditChange_MovedIn				= 7,
		eInvAuditChange_MovedInFromNowhere	= 8,
		eInvAuditChange_WarehouseUpdateAbs	= 9,
		eInvAuditChange_WarehouseUpdateInc	= 10,
		eInvAuditChange_AdminUpdate			= 11,
		eInvAuditChange_WarehouseUpdateDec	= 12, // PYR-20293
	};
private:

	UINT32	numModified; // sequence will be used to check if a record was modified
	UINT32	swiid;
	UINT32	itemId;
	UINT32	warehouseId;
	INT32	numAvailable;
	UINT32	numOrdered;
	UINT32	numReserved;
	UINT32	numCheckedOut;
	UINT32	numWhse;
	UINT32	monetaryValue;
	UINT32	lowAlert1;
	UINT32	lowAlert2;
	UINT32	fppPrice;

	bool	outOfStock;
	bool	outOfStockOverride;
	bool	unlimited;
	bool	unlimitedPossible;
	bool	canReserve;

	// this can only be set by either of the constructors and
	// cannot be changed afterwards. once an object had been
	// created it does not change the way it handles notifications
	// so there should not be any confusion
	bool	skipNotifications;
	
	PString	stockId;
	PString	warehouseName;
	PString	officeName;
	PString	itemDescription;
	PString auditComment;
	
	eInventoryAuditChangeType auditChangeType;
	
	InvItemSpecsColl specs;
	NotificationsColl_ notifications;
	
	
	// the following members are not serialized
	//
	
	// a hint that the object is going from client to server so it does not need
	// to send SPEC_ values
	bool skipSerializingForServer;

	// a hint that initialized was called once so if it's called again
	// an exception occurs - this is just a protection against accidental
	// incorrect use of object init() is protected so this situation is unlikely
	//
	bool initialized;
};

//////////////////////////////////////////////////////////////////////////

// expose addNotification only for some very specific clients
//

// like CRM DBM which needs to construct the object from scratch and 
// needs to call these functions
//
class InvWarehouseStockItemInternal : public InvWarehouseStockItem
{
public:
	InvWarehouseStockItemInternal( bool skipNotifications_) : 
	  InvWarehouseStockItem( skipNotifications_){}

	using InvWarehouseStockItem::addNotification;
	using InvWarehouseStockItem::init;

	using InvWarehouseStockItem::checkOutItem;
	using InvWarehouseStockItem::unCheckOutItem;
	using InvWarehouseStockItem::purchaseItem;
	using InvWarehouseStockItem::cancelOrder;
	using InvWarehouseStockItem::ship;
	using InvWarehouseStockItem::isAvailableForSale;
	using InvWarehouseStockItem::moveOrderFrom;
	using InvWarehouseStockItem::updateWarehouseQuantity;
	using InvWarehouseStockItem::updateWarehouseQuantityDec; // PYR-20293
	using InvWarehouseStockItem::isInitialized;
	using InvWarehouseStockItem::prefixAdminAuditComment;
};

//////////////////////////////////////////////////////////////////////////

class AddStockInterface
{
public:
	AddStockInterface(){};
	virtual ~AddStockInterface(){};
	virtual void addStock( const InvItemSpecsColl& specs, const bool available, const UINT32 stockCount, bool unlimitedStock) = 0;
	virtual void print() = 0;
	virtual void compose(CommMsgBody& body) const {};
};

//////////////////////////////////////////////////////////////////////////

class InvItemStock2 : public InvItemStockCli2
{
public:
	InvItemStock2( const InvItemSpecsColl& specs_, const bool available_, const UINT32 stockCount_, bool unlimitedStock_ );
	InvItemStock2( CommMsgParser& parser) : InvItemStockCli2( parser){};

	void compose(CommMsgBody& body) const override;
};

class InvItemStocks2 : public InvItemStocksCli2, public AddStockInterface
{
public:
	InvItemStocks2(){}
	InvItemStocks2( CommMsgParser& parser );

	void compose(CommMsgBody& body) const override;
	void addStock( const InvItemSpecsColl& specs, const bool available, const UINT32 stockCount, bool unlimitedStock) override;
	void print() override
	{
		InvItemStocksCli2::print();
	};
};

// expose addStock only for some very specific clients
//
class InvItemStocksInternal2 : public InvItemStocks2
{
public:
	using InvItemStocks2::addStock;
};


//////////////////////////////////////////////////////////////////////////

class UpdateInventoryFunctorBase
{
public:
	UpdateInventoryFunctorBase(){};

	virtual ~UpdateInventoryFunctorBase(){}
	virtual bool update(InvWarehouseStockItemInternal& whi) const = 0;
};

class UpdateInventoryCancel : public UpdateInventoryFunctorBase
{
	const PString comment;
	const bool returnToStock;
	const bool cancellingBeforeShipped; 	// PYR-17956

public:
	UpdateInventoryCancel(	const char* comment_, 
							const bool returnToStock_,
							const bool cancellingBeforeShipped_) 
							:	
							comment( comment_), 
							returnToStock(returnToStock_),
							cancellingBeforeShipped( cancellingBeforeShipped_){};

	bool update(InvWarehouseStockItemInternal& whi) const
	{
		return whi.cancelOrder( comment, cancellingBeforeShipped, returnToStock);
	}
};

class UpdateInventoryShip : public UpdateInventoryFunctorBase
{
public:
	UpdateInventoryShip(){};

	bool update(InvWarehouseStockItemInternal& whi) const
	{
		return whi.ship();
	}
};

//////////////////////////////////////////////////////////////////////////

class InvOrderByIdReply
{
public:
	InvOrderByIdReply(	UINT32 orderId_, 
						UINT32 warehouseId_, 
						const char* stockId_) :	orderId( orderId_),
												warehouseId( warehouseId_),
												stockId( stockId_){};
	InvOrderByIdReply( CommMsgParser& parser)
	{
		parse( parser);
	}

	UINT32 getOrderId()			const { return orderId;}
	UINT32 getWarehouseId()		const { return warehouseId;}
	const char* getStockId()	const { return stockId.c_str();}

	void compose(CommMsgBody& body) const
	{
		body.composeUINT32(orderId)
			.composeUINT32(warehouseId)
			.composeString(stockId);
	}

	void print()
	{
		PLog("\r\nInvOrderByIdReply:\r\n\torderId(%u)\r\n\tWarehouseId(%u)\r\n\tStockId (%s)",
			 orderId, warehouseId, stockId.c_str());
	}
	
private:
	void parse(CommMsgParser& parser)
	{
		parser.parseUINT32( orderId)
			  .parseUINT32(warehouseId)
			  .parseStringP(stockId);
	}

private:
	UINT32  orderId;
	UINT32  warehouseId;
	PString stockId;
};

class InvMoveOrderParams
{
public:
	InvMoveOrderParams(	UINT32 orderId_, 
						UINT32 newWarehouseId_,
						bool   fromReserved_,
						bool oldOrderNotAttachedToWarehouse_ = false) 
						:	
						orderId(orderId_),
						newWarehouseId( newWarehouseId_),
						fromReserved( fromReserved_),
						oldOrderNotAttachedToWarehouse(oldOrderNotAttachedToWarehouse_){}

	InvMoveOrderParams( CommMsgParser& parser) :	orderId(0),
													newWarehouseId( 0),
													fromReserved( false)
	{
		parse( parser);
	}

	UINT32	getOrderId()		const { return orderId;}
	UINT32	getNewWarehouseId()	const { return newWarehouseId;}
	bool	isFromReserved()	const { return fromReserved;}
	bool	isOldOrderNotFromWarehouse() const { return oldOrderNotAttachedToWarehouse;}

	void compose(CommMsgBody& body) const
	{
		body.composeUINT32(orderId)
			.composeUINT32(newWarehouseId)
			.composeBOOL(fromReserved)
			.composeBOOL(oldOrderNotAttachedToWarehouse);
	}

	void print() const
	{
		PLog(	"InvMoveOrderParams: orderId(%u) newWHSE(%u) R(%d) FrNowhere(%d)", 
				orderId, 
				newWarehouseId, 
				fromReserved ? 1 : 0,
				oldOrderNotAttachedToWarehouse ? 1 : 0);
	}

private:
	void parse(CommMsgParser& parser)
	{
		parser.parseUINT32(orderId)
			  .parseUINT32(newWarehouseId)
			  .parseBOOL(fromReserved)
			  .parseBOOL(oldOrderNotAttachedToWarehouse);
	}

private:
	UINT32	orderId;
	UINT32	newWarehouseId;
	bool	fromReserved;
	bool	oldOrderNotAttachedToWarehouse;
};
//////////////////////////////////////////////////////////////////////////

class InvReturnItemParams
{
public:
	InvReturnItemParams( UINT32  warehouseId_,
						 UINT32  orderId_,
						 UINT32  numItems_,
						 const char* stockId_,
						 const char* user_,
						 const char* country_,
						 const char* whseRef_,
						 const char* reason_,
						 const char* retComment_) : warehouseId	( warehouseId_),
													orderId		( orderId_),
													numItems	( numItems_),
													stockId		( stockId_),
													whseRef		( whseRef_),
													reason		( reason_),
													retComment	( retComment_),
													user		( user_),
													country		( country_){}

	InvReturnItemParams( CommMsgParser& parser) : warehouseId( 0),
												  orderId	 ( 0),
												  numItems	 ( 0)
	{
		parse( parser);
	}

	void compose(CommMsgBody& body) const
	{
		body.composeUINT32( warehouseId)
			.composeUINT32( orderId)
			.composeUINT32( numItems)
			.composeString( stockId)
			.composeString( whseRef)
			.composeString( reason)
			.composeString( retComment)
			.composeString( user)
			.composeString( country);
	}

	UINT32 getWarehouseId()			const { return warehouseId; }
	UINT32 getOrderId()				const { return orderId;}
	UINT32 getNumItems()			const { return numItems;}
	
	const char* getStockId()		const { return stockId;}
	const char* getUser()			const { return user;}
	const char* getCountry()		const { return country;}
	const char* getWarehouseRef()	const { return whseRef;}
	const char* getReason()			const { return reason;}
	const char* getComment()		const { return retComment;}

private:

	void parse(CommMsgParser& parser)
	{
		parser.parseUINT32( warehouseId)
			  .parseUINT32( orderId)
			  .parseUINT32( numItems)
			  .parseStringP( stockId)
			  .parseStringP( whseRef)
			  .parseStringP( reason)
			  .parseStringP( retComment)
			  .parseStringP( user)
			  .parseStringP( country);
	}

	UINT32  warehouseId;
	UINT32  orderId;
	UINT32  numItems;

	PString stockId;
	PString whseRef;
	PString reason;
	PString retComment;
	PString user;
	PString country;
};

class InvUpdateReturnItemParams
{

public:
	InvUpdateReturnItemParams( UINT32 refNum_,
							   UINT32 orderId_,
							   PString retComment_) : refNum	( refNum_),
													 orderId	( orderId_),
													 retComment	( retComment_){}

	InvUpdateReturnItemParams( CommMsgParser& parser) : refNum ( 0),
														orderId( 0)
	{
		parse( parser);
	}

	void compose(CommMsgBody& body) const
	{
		body.composeUINT32( refNum)
			.composeUINT32( orderId)
			.composeString( retComment);
	}

	UINT32 getRefNum()		 const { return refNum;}
	UINT32 getOrderId()		 const { return orderId;}
	const char* getComment() const { return retComment;}

	void print() const
	{
		PLog("InvUpdateReturnItemParams refNum(%u) orderId(%u) comment(%s)", refNum, orderId, retComment.c_str());
	}
	

private:

	void parse(CommMsgParser& parser)
	{
		parser.parseUINT32( refNum)
			  .parseUINT32( orderId)
			  .parseStringP( retComment);
	}

	UINT32  refNum;
	UINT32  orderId;
	PString retComment;
};

////////////////////////////////////////////////////////////////////////////////

class InvUpdateWarehouseItemStockParams
{
public:
	InvUpdateWarehouseItemStockParams(	UINT32	swiid_, 
										UINT32	whseNum_, 
										bool	increment_) :	swiid( swiid_), 
																numWhse( whseNum_), 
																increment( increment_),
																decrement(false)
	{}

	// PYR-20293
	InvUpdateWarehouseItemStockParams(	UINT32	swiid_, 
										INT32	whseNum_) :		swiid(swiid_), 
																numWhse(abs(whseNum_)), 
																increment(true),
																decrement(whseNum_ < 0)
	{}

	InvUpdateWarehouseItemStockParams(CommMsgParser& parser)
	: 	decrement(false)
	{
		parse( parser);
	}

	UINT32	getSWIID()		const { return swiid;}
	UINT32	getNumWhse()	const { return numWhse;}
	bool	isIncrement()	const { return increment;}
	bool	isDecrement()	const { return decrement; } // PYR-20293

	void compose(CommMsgBody& body) const
	{
		body.composeUINT32( swiid)
			.composeUINT32( numWhse)
			.composeBOOL  ( increment);
	}

	void print() const
	{
		PLog("InvUpdWhseItemStockParams: SWIID(%u) numWhse(%u) isInc(%d)", swiid, numWhse, increment ? 1 : 0);
	}

private:

	void parse(CommMsgParser& parser)
	{
		parser.parseUINT32( swiid)
			  .parseUINT32( numWhse)
			  .parseBOOL  ( increment);
	}

	UINT32 swiid;
	UINT32 numWhse;
	bool   increment;
	bool   decrement; // PYR-20293 - not composed/parsed
};

////////////////////////////////////////////////////////////////////////////////

class InvRevDefaultRoutingConflict
{
public:

	static const UINT32 missingRouteVal = 0xFFFFFFFF;

	enum ConflictNature 
	{ 
		MORE_THAN_ONE_SOURCE = 0, 
		REDUNDANT_CUSTOM_ROUTING = 1,
		CUSTOM_ROUTING_CONFLICT = 2,
		MISSING_ROUTE_CONFLICT = 3,
		UNIDENTIFIED_CONFLICT = 4,
	};
	InvRevDefaultRoutingConflict(	UINT32 itemId_, 
									const char* itemDescr_,
									UINT32 rtSum_, 
									UINT32 whseCount_,
									UINT32 whseId_,
									const char* country_) : itemId( itemId_),
															itemDescr( itemDescr_),
															routeItemIdSum( rtSum_),
															whseCount( whseCount_),
															whseId( whseId_),
															country( country_)
	{
		if( getConflictNature() == CUSTOM_ROUTING_CONFLICT ||
			getConflictNature() == UNIDENTIFIED_CONFLICT)
		{
			PLog("ERROR: suspicious routing conflict detected.");
			print();
		}
	}
	InvRevDefaultRoutingConflict(CommMsgParser& parser)
	{
		parse( parser);
	}

	UINT32 getItemId()			const { return itemId;}
	UINT32 getWhseCount()		const { return whseCount;}
	UINT32 getRouteItemIdSum()	const { return routeItemIdSum;}
	UINT32 getWarehouseId()		const { return whseId;}
	const char* getCountry()	const { return country.c_str();}
	const char* getItemDescr()	const { return itemDescr.c_str();}

	void compose(CommMsgBody& body) const
	{
		body.composeUINT32( itemId)
			.composeUINT32( routeItemIdSum)
			.composeUINT32( whseCount)
			.composeUINT32( whseId)
			.composeString( country)
			.composeString( itemDescr);
	}

	ConflictNature getConflictNature() const;
	const char* const getConflictDescr() const;

	void print() const;
	
private:

	void parse(CommMsgParser& parser)
	{
		parser.parseUINT32( itemId)
			  .parseUINT32( routeItemIdSum)
			  .parseUINT32( whseCount)
			  .parseUINT32( whseId)
			  .parseStringP( country)
			  .parseStringP( itemDescr);
	}

	UINT32 itemId;
	UINT32 routeItemIdSum;
	UINT32 whseCount;
	UINT32 whseId;
	PString country;
	PString itemDescr;
};

class InvRevEditDefaultRoutingReply
{
public:
	typedef PNewPtrVector<InvRevDefaultRoutingConflict> DRC_Coll_;

	InvRevEditDefaultRoutingReply( UINT32 revNum) : 
		defaultRouting( revNum){};
	
	InvRevEditDefaultRoutingReply(CommMsgParser& parser) : defaultRouting( parser)
	{
		parse( parser);
	}

	const DRC_Coll_& getConflicts() const 
	{ 
		return conflicts;
	}
	// since we plan on modifying the returned object - this cannot be a const function
	// 
	InvDefaultRouting& getDefaultRouting()
	{ 
		return defaultRouting;
	}

	void print() const;

	void compose(CommMsgBody& body) const;

protected:

	InvDefaultRoutingInternal& getDefaultRoutingInternal()
	{ 
		return defaultRouting;
	}
	void addConflict(	UINT32 itemId, 
						const char* descr,
						UINT32 rtSum, 
						UINT32 whseCount,
						UINT32 whseId,
						const char* country)
	{
		conflicts.push_back( new InvRevDefaultRoutingConflict( itemId, descr, rtSum, whseCount, whseId, country));
	}
	
private:
	void parse(CommMsgParser& parser);

	InvDefaultRoutingInternal defaultRouting;
	DRC_Coll_ conflicts;
};

class InvRevEditDefaultRoutingReplyInternal : public InvRevEditDefaultRoutingReply
{
public:
	InvRevEditDefaultRoutingReplyInternal( UINT32 revNum) : 
	  InvRevEditDefaultRoutingReply( revNum){}

	using InvRevEditDefaultRoutingReply::getDefaultRoutingInternal;
	using InvRevEditDefaultRoutingReply::addConflict;

};
////////////////////////////////////////////////////////////////////////////////

class IProcessMapping
{
public:
	IProcessMapping(){}
	virtual ~IProcessMapping(){}

	virtual void processMapping( const char* propName, const char* propValue, const char* mapTo) = 0;

};

////////////////////////////////////////////////////////////////////////////////

// Contains mapping information of what property values are available for
// properties with specified indices and which literals they translate to 
// when used in the item stock id
//
// When initialized, takes a collection of FppItemProperty as its props parameter
// which has already been pre-filtered and does not contain any special properties 
// with names starting with "_" and contains only properties that can be used to select
// a specific physical item in the store. This collection will be used to create a
// mapping of property name to the property index as it appears in the props collection.
// 
// properties color and size may be absent from the props collection but they must
// appear in the final stock id and so they must be created as a part of the mapping
// so if we do not have them in the props collection we will still add them
// to the mapping with indices that immediately follow those in the props collection.
//
class VipStorePhysicalItemPropertyMapping : public IProcessMapping
{
	
public:
	VipStorePhysicalItemPropertyMapping() : fakeColorIdx(0),
											fakeSizeIdx(0){}
	
	// read property names and what they map to from the database
	// and add it to the internal map based on the position a property with
	// the same name appears in the props collection with special handling
	// for Color and Size
	//
	void init(	const InvFppItemPropertyColl& props);
	void processMapping(const char* propName, const char* propValue, const char* mapTo);
	

	// get a literal that a property with a a specified index and name maps to
	//
	const char* mapPropValue(	UINT32 outerPropertyIndex, 
		const PString& propValue) const;

	virtual ~VipStorePhysicalItemPropertyMapping()
	{
		cleanup();
	}

	void print() const;

	// check if the property collection we initialized from did not contain Color
	// property
	bool isFakeColor() const { return fakeColorIdx >= 0;}

	// check if the property collection we initialized from did not contain Size
	// property
	bool isFakeSize () const { return fakeSizeIdx >= 0;}

	// what a literal for the fake color will look like
	const char* getFakeColor() const 
	{ 
		return mapPropValue(fakeColorIdx, "");
	}

	// what a literal for the fake size will look like
	const char* getFakeSize()  const 
	{ 
		return mapPropValue(fakeSizeIdx, "");
	}

private:

	// create a map of property name to property index how they appear in the props collection
	// with a special handling for size and color
	//
	static void populatePropertyNameToIndexMap(	const InvFppItemPropertyColl& props,
		PStringMap<UINT32>& nameToIndexMap,
		int&  outFakeColorIdx,
		int&  outFakeSizeIdx);

	void cleanup()
	{
		for( PropertyValMapsColl_::iterator it = propertyValMapsColl.begin();
			it != propertyValMapsColl.end(); ++ it)
		{
			delete (*it).second;
		}
		propertyValMapsColl.clear();
		nameToIndexMap.clear();
	}

	typedef PStringMap<PString> PropertyValMap_;
	typedef map<UINT32,PropertyValMap_*> PropertyValMapsColl_;
	PropertyValMapsColl_ propertyValMapsColl;

private:
	
	int fakeColorIdx;
	int fakeSizeIdx;
	PStringMap<UINT32> nameToIndexMap;

	const static char* const NO_MAPPED_VALUE;
	const static char* const COLOR_PROP_NAME;
	const static char* const SIZE_PROP_NAME;
};

//////////////////////////////////////////////////////////////////////////

// functions used in creation of item stock id from fpp item property collection
// works in conjunction with an instance of VipStorePhysicalItemPropertyMapping
// class
//
class VipStoreInventoryStockIdUtils
{
public:

	// take fppitem.props and create a new collection which does not have special properties
	// starting with "_" that are not used for selecting a variation of a physical item to be 
	// purchased. i.e. while "color" and "size" allow to pick which variation of T-shirt X will be
	// bought, the "_category" property value is totally irrelevant in this case
	//
	// also filter out editable properties - those that have "!!" values
	//
	static void createFilteredItemPropertyColl(	const InvFppItemPropertyColl& props,
		InvFppItemPropertyColl& propColl);

	// take a filtered collection and create a collection of property variations
	// for example
	// {{A,B},{1,2},{a,b}} would translate into a collection
	// {{A,1,a},{A,1,b},{A,2,a},{A,2,b},{B,1,a},{B,1,b},{B,2,a},{B,2,b}}
	//
	static void createPropertyVariantsFromFilteredProps(const InvFppItemPropertyColl& props,
		InvItemPropCollColl& outValueVariantsColl);

	// take a single property variation and translate it into a stock id based on the
	// instance of the VipStorePhysicalItemPropertyMapping created from the filtered
	// item properties
	//
	static void createStockID(	const InvItemPropColl& compositValueColl,
		const VipStorePhysicalItemPropertyMapping& propValMapping,
		UINT32 itemId,
		PString& stockID);

	static bool isNotUnderstoodByInventory( const FppItemProperty& fppItemProperty);
	static bool isEditableProperty( const InvFppItemPropertyValuesColl& values);
	static UINT32 getPropCountUnderstoodByInventory( const InvFppItemPropertyColl& props);

public:
	const static char* const INVISIBLE_PROPERTY_PREFIX;
	const static char* const EDITABLE_PROPERTY_VALUE;

private:
	static bool isInvisibleProperty( const PString& propName);
	static void createPropertyVariantsImpl(	const InvFppItemPropertyColl& propColl, 
		const InvFppItemPropertyColl::const_iterator& itPropColl, 
		InvItemPropColl& outComposedValueColl, 
		InvItemPropCollColl& outValueVariantsColl);
};

//////////////////////////////////////////////////////////////////////////
// [AN 2010-04-08] PYR-14464
//
class InvRoutesToCover
{
public:
	typedef PStringMap<UINT32>		RoutesForItemsByKey_;
	typedef map<UINT32, PString*>	ItemsDescriptionsById_;

private:
	RoutesForItemsByKey_	routesForItemsByKey;
	RoutesForItemsByKey_	deletedRoutesForAllCountries;
	ItemsDescriptionsById_	itemDescriptionsById;

	static const char* createKey( UINT32 itemId, const char* country, PString& key);
	static bool isKeyPresent( const char* key, const RoutesForItemsByKey_& m);
	static bool addRouteToMap( UINT32 itemId, const char* country, RoutesForItemsByKey_& m);
	
public:
	InvRoutesToCover(){}
	~InvRoutesToCover();
	void addRouteToCover( UINT32 itemId, const char* country, const char* itemDescription);
	void coverRoute( UINT32 itemId, const char* country);
	void getUnroutedItems( PStringMap<UINT32>& itemsByDescr) const;
};

//////////////////////////////////////////////////////////////////////////

#endif //VipStoreInventoryShared_h_included
