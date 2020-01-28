#ifndef VipStoreInventorySharedCli_h_included
#define VipStoreInventorySharedCli_h_included

#include "commcli.h"

typedef PNewPtrVector<PString> InvItemSpecsColl;


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

class InvItemStockCli2
{
public:
	InvItemStockCli2( const bool available_, const UINT32 stockCount_, const bool unlimitedStock_) 
		: available( available_), stockCount(stockCount_), unlimitedStock(unlimitedStock_)
	{
	}
	InvItemStockCli2( CommMsgParser& parser);

	virtual ~InvItemStockCli2(){}

	virtual void print();
	virtual void compose(CommMsgBody& body) const {};

public:
	bool isAvailable() const { return available;}
	const InvItemSpecsColl& getSpecs() const { return specs;}
	INT32 getStockCount() const { return stockCount;}
	bool isUnlimitedStock() const { return unlimitedStock;}

protected:
	bool available;
	INT32 stockCount; // signed int. negative means it's on the back order.
	bool unlimitedStock;
	InvItemSpecsColl specs;
};

class InvItemStocksCli2
{
public:
	InvItemStocksCli2(){}
	InvItemStocksCli2( CommMsgParser& parser);

	virtual ~InvItemStocksCli2(){}

	virtual void print();

public:

	typedef PNewPtrVector<InvItemStockCli2> InvItemStockColl2_;
	const InvItemStockColl2_& getStocks() const { return stocks;}

protected:
	InvItemStockColl2_ stocks;
};

//////////////////////////////////////////////////////////////////////////
// PYR-15672: VipStore alternate delivery address

class VipStoreAlternateClientAddress
{
	UINT32 addrId;
	PString userId;
	PString fullName;
	PString addr1;
	PString addr2;
	PString city;
	PString state;
	PString country;
	PString code;
	PString phone;
	PString fiscalCode; // PYR-18383
	bool	haveFiscalCode; // TODO: get rid after July 2012 build. not composable.

public:
	// New alternative address
	static const UINT32 CLIENT_DEFAULT_ADDRESS = 0;
	// User's fiscal code passed as alternative address
	static const UINT32 CLIENT_MAIN_ADDRESS = 0xFFFFFFFF; // PYR-18383

	VipStoreAlternateClientAddress() : addrId(CLIENT_DEFAULT_ADDRESS), haveFiscalCode(true)
	{
	}
	VipStoreAlternateClientAddress(	const UINT32 addrId_, 
		const char* userId_,
		const char* fullName_,
		const char* addr1_,
		const char* addr2_,
		const char* city_,
		const char* state_,
		const char* country_,
		const char* code_,
		const char* phone_,
		const char* fiscalCode_) : haveFiscalCode(true)
	{
		init(	addrId_, 
				userId_, 
				fullName_, 
				addr1_, 
				addr2_, 
				city_, 
				state_, 
				country_, 
				code_,
				phone_,
				fiscalCode_);
	}
	void init(	const UINT32 addrId_, 
				const char* userId_,
				const char* fullName_,
				const char* addr1_,
				const char* addr2_,
				const char* city_,
				const char* state_,
				const char* country_,
				const char* code_,
				const char* phone_,
				const char* fiscalCode_)
	{
		addrId	= addrId_;
		userId	= userId_;
		fullName.assign	( PString::ltrim(fullName_),PString::rtrim(fullName_));
		addr1.assign	( PString::ltrim(addr1_),	PString::rtrim(addr1_));
		addr2.assign	( PString::ltrim(addr2_),	PString::rtrim(addr2_));
		city.assign		( PString::ltrim(city_),	PString::rtrim(city_));
		state.assign	( PString::ltrim(state_),	PString::rtrim(state_));
		country.assign	( PString::ltrim(country_),	PString::rtrim(country_));
		code.assign		( PString::ltrim(code_),	PString::rtrim(code_));
		phone.assign	( PString::ltrim(phone_),	PString::rtrim(phone_));
		fiscalCode.assign( PString::ltrim(fiscalCode_),	PString::rtrim(fiscalCode_)); // PYR-18383
		haveFiscalCode = true;
	}
	VipStoreAlternateClientAddress(CommMsgParser& parser)
	{
		parse(parser);
	}
	bool equals(const VipStoreAlternateClientAddress& rhs) const
	{
		if(this == &rhs)
			return true;

		return	addrId	== rhs.addrId &&
				userId.equals(rhs.userId) &&
				fullName.equals(rhs.fullName) &&
				addr1.equals(rhs.addr1) &&
				addr2.equals(rhs.addr2) &&
				city.equals(rhs.city) &&
				state.equals(rhs.state) &&
				country.equals(rhs.country) &&
				code.equals(rhs.code) &&
				phone.equals(rhs.phone) &&
				fiscalCode.equals(rhs.fiscalCode); // PYR-18383
	}
	bool isEmpty() const
	{
		return	addrId == CLIENT_DEFAULT_ADDRESS &&
				userId.length()	== 0 &&
				addr1.length()	== 0 &&
				addr2.length()	== 0 &&
				city.length()	== 0 &&
				state.length()	== 0 &&
				country.length()== 0 &&
				code.length()	== 0 &&
				phone.length()	== 0 &&
				fiscalCode.length()	== 0; // PYR-18383
	}
	bool isValid() const
	{
		// must me either empty or have at least addr1, city and country are populated
		return	isEmpty() || (	addr1.length() > 0	&&
								city.length() > 0	&&
								country.length() > 0 );
	}
	void compose(CommMsgBody& msg, bool includeFiscalCode = true) const
	{
		msg.composeUINT32( addrId)
			.composeString(userId)
			.composeString(fullName)
			.composeString(addr1)
			.composeString(addr2)
			.composeString(city)
			.composeString(state)
			.composeString(country)
			.composeString(code)
			.composeString(phone)
			;
			if( includeFiscalCode )
			{
				// TODO: remove conditional compose after July 2012 build
				// we always parse out fiscasl code 
				// we do not compose fiscal code only in vipStore->oldClient
				//
				msg.composeString(fiscalCode) // PYR-18383
				;
			}
	}
	void parse(CommMsgParser& parser)
	{
		parser.parseUINT32(addrId)
			.parseStringP(userId)
			.parseStringP(fullName)
			.parseStringP(addr1)
			.parseStringP(addr2)
			.parseStringP(city)
			.parseStringP(state)
			.parseStringP(country)
			.parseStringP(code)
			.parseStringP(phone)
			;
		// TODO remove condition after a few builds of clients (this is done May 2012)
		if (!parser.parseEnded())
		{
			parser.parseStringP(fiscalCode); // PYR-18383
			haveFiscalCode = true; // fiscal code received - set flag
		}
		else
		{
			fiscalCode = "";
			haveFiscalCode = false; // no fiscal code received - set flag
		}
	}

	void clear()
	{
		init( CLIENT_DEFAULT_ADDRESS, "", "", "", "", "", "", "", "", "", "");
	}

	const UINT32 getAddrId()	const { return addrId; }
	const char* getUserId()		const { return userId; }
	const char* getFullName()	const { return fullName; }
	const char* getAddr1()		const { return addr1; }
	const char* getAddr2()		const { return addr2; }
	const char* getCity()		const { return city; }
	const char* getState()		const { return state; }
	const char* getCountry()	const { return country; }
	const char* getCode()		const { return code; }
	const char* getPhone()		const { return phone; }
	const char* getFiscalCode()	const { return fiscalCode; } // PYR-18383
	bool isHaveFiscalCode()		const { return haveFiscalCode; } // TODO: get rid after July 2012 build

	void print() const
	{
		PLog("VSACA: id=%u uid='%s'", addrId, userId.c_str());
	}

	// PYR-18383
	bool isNewAlternativeAddress() const
	{
		return (addrId == VipStoreAlternateClientAddress::CLIENT_DEFAULT_ADDRESS);
	}

	// PYR-18383
	bool isUserOwnAddressFiscalCodeOnly() const
	{
		return (addrId == VipStoreAlternateClientAddress::CLIENT_MAIN_ADDRESS);
	}

	// PYR-18383
	bool isActualAlternativeAddressId() const
	{
		return (addrId != VipStoreAlternateClientAddress::CLIENT_DEFAULT_ADDRESS &&
				addrId != VipStoreAlternateClientAddress::CLIENT_MAIN_ADDRESS);
	}
};

#endif //VipStoreInventorySharedCli_h_included
