#include "ppincludebase.h"
#include "currency.h"

struct NVS {
	const char*	name;
	int	val;
};

static const NVS nameval[] = {
//
// Add only needed currencies below here:
//
	{ "AUD",    36  },
	{ "CAD",    124 },
	{ "DKK",    208 },
	{ "EUR",    978 },
	{ "GBP",    826 },
	{ "NOK",    578 },
	{ "SEK",    752 },
	{ "USD",    840 },
	{ "RUB",    643 },
	{ "NZD",    554 },
	{ "CHF",    756 },
    { "CZK",    203 },
    { "BRL",    986 },
    { "BGN",    975 },
    { "CLP",    152 },
    { "COP",    170 },
    { "HRK",    191 },
    { "HUF",    348 },
    { "MXN",    484 },
    { "KZT",    398 },
    { "LTL",    440 },
    { "PYG",    600 },
    { "PEN",    604 },
    { "PLN",    985 },
    { "RON",    946 },
    { "RSD",    941 },
    { "UAH",    980 },
	{ "MDL",    498 },
	{ "PHP",    608 },
	{ "ARS",     32 },
	{ "CNY",    156 },
	{ "EEK",    233 },
	{ "HKD",    344 },
	{ "INR",    356 },
	{ "IDR",    360 },
	{ "KRW",    410 },
	{ "LVL",    428 },
	{ "MYR",    458 },
	{ "SGD",    702 },
	{ "ZAR",    710 },
	{ "TWD",    901 },
	{ "THB",    764 },
	{ "VND",    704 },
	{ "ISK",    352 },
	{ "JPY",    392 },
	{ "BYR",    974 }

};

int CurrencyCode2number(const char* code)
{
	if (!code || !*code)
		return 0;
	for (int i = 0; i < sizeof(nameval)/sizeof(nameval[0]); ++i)
		if (strcmp(nameval[i].name, code) == 0)
			return nameval[i].val;
	return 0;
}

const char* CurrencyNumber2code(int number)
{
	if (number <= 0)
		return 0;
	for (int i = 0; i < sizeof(nameval)/sizeof(nameval[0]); ++i)
		if (nameval[i].val == number)
			return nameval[i].name;
	return 0;
}
