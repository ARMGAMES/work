#ifndef vppmultipliers_h_included
#define vppmultipliers_h_included

#include "commlib.h"
#include "licenserules.h"

static const char* const pokerCurrencies[] = { "USD", "EUR", "GBP", "CAD", "INR" }; //The list of allowed currencies

struct VppMultiplier
{
	enum Type { undefined = 0, local = 1, global = 2, compatibility = 3};
	static const INT32 maxVppMultiplier = 100; //100x = 10000%

	INT32 value; //x100 value
	Type type;

	VppMultiplier() : value(0), type(undefined) {}
	VppMultiplier(INT32 value_, Type type_) : value(value_), type(type_) {}
};

class VppMultipliers : public map<LicenceIdFlags, VppMultiplier>
{
public:
	VppMultipliers() {}
	VppMultipliers(const char* initStr) { fromString(initStr); }
	const char* toString(PString& ret) const;
	void fillFromString(const char* initStr)
	{
		clear();
		fromString(initStr);
	}
	void fillFromIniSection( const IniDataFromDbm::Section* section, const char* currencyToFill );
	INT32 getMultiplierX100(UINT32 licenseId) const;	//x100
	INT32 getMaxMultiplierX100() const;	//x100

	// exposed to share with Lobby
	static void parseSection( const IniDataFromDbm::Section* section, PStringMap<UINT32>& licenseCurrencyPairToMultiplierMap );
	//PYR-21113
	//validates the parameter name used in dbm ini data, ex. "COM-USD"
	static bool isValidLicenseCurrencyPair( const char* name, PString& err );
	//reads VPP_Multiplier_XXX parameter value, validates and return multiplier value in percents
	static UINT32 readVppMultiplierFromIniParam( const char* param, PString& err );
	static bool isValidCurrency( const char* name );
private:
	void fromString(const char* initStr);
};

#endif //vppmultipliers_h_included
