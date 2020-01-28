// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Parts of this file are being maintained using automated scripts.
// The script maintenance parts are surrounded by language-specific
// comment lines that begin with <<== and end with ==>> markers.
// When required, manual changes can made to this file as usual. 
// Such changes should be communicated to the current maintainer 
// of GIT module in ADMIN/asppages/admin/_licenses. 
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#ifndef	shared_h_included
#define	shared_h_included

#ifdef PROTOCOL_CLIENT_ONLY
#	error shared.h in client
#endif

#include "ppinclude.h"
#include "ppcontainer.h"
#include "commmsgbody.h"
#include "pyrtime.h"
#include "getservconnip.h"
#include "i18n.strmap.h"
#include "nextrandom.h"
#include "licenseid.h"
#include "tlbbop.h"
#include "PlayChipsScaler.h" // PYR-23932
#include "clientplatforms.h" // PYR-26426
#include "commonmacros.h"
#include "TimeUtils.h" // movet TimeUtils to a separate file
#include "clientshared.h"
#include "PlayChipsDefs.h"
#include "Base26Converter.h"
#include "brands.h"
#include "commgrdppwdsid.h"
#include "commgrdencryptedpwd.h"
#include "misc.h"
#include "TicketType.h"

void _appendMoney( PString& buf, INT64 chips64 );
INT64 centstoi64( const char* str, const char** ppos = NULL ); // accepts '+10' '-.10' '10.5' '10.05', sets ppos to after parsed cents (or str if cents not found)

INT32 getNextINT(const char*& str);
UINT32 encodeInt32Vector(const vector<INT32>& vec, PString& str);
UINT32 decodeInt32Vector(const char* str, vector<INT32>& vec);
UINT32 getNextUINT(const char*& str);
UINT32 encodeUint32Vector(const vector<UINT32>& vec, PString& str);
UINT32 decodeUint32Vector(const char* str, vector<UINT32>& vec);
UINT32 encodeUint32Map(const PStringMap<UINT32>& mapIn, PString& strOut);
UINT32 encodeInt32Map(const PStringMap<INT32>& mapIn, PString& strOut); // PYR-38436
UINT32 decodeUint32Map(const char* strIn, PStringMap<UINT32>& mapOut);
INT32 decodeInt32Map(const char* strIn, PStringMap<INT32>& mapOut);
UINT64 getNextUINT64New( const char*& str ); // added word New due to naming conflict with function in hhistory

//const char*  getCurrencyByLicense(UINT32 licenseId);
bool compareCurrencies(const char* currency1, const char* currency2, bool logFaultyLogicIfEmptyCurrency = true);
const char* normalizedCurrency(const char* currency, bool logFaultyLogicIfEmptyCurrency = true);
bool isZeroCentsProcCur(const char* processorCurrency); // PYR-56416 returns true if PROCESSOR external currency required 0 minor units (cents)

void parseString(CommMsgParser& parser, PString& str);

struct UserFiveContainer
{
	UINT32 realMoney;
	UINT32 playMoney;
	UINT32 wMoney;
	UINT32 tMoney;
	UINT32 fpp;
	UINT32 scalePM; // PYR-23932
	PString currency; // PYR-28924

	UserFiveContainer()
	{
		realMoney = 0;
		playMoney = 0;
		wMoney = 0;
		tMoney = 0;
		fpp = 0;
		scalePM = 1; // PYR-23932
	}

	void parse(CommMsgParser& parser)
	{
		parser
			.parseUINT32(realMoney)
			.parseUINT32(playMoney)
			.parseUINT32(wMoney)
			.parseUINT32(tMoney)
			.parseUINT32(fpp)
			.parseUINT32(scalePM) // PYR-23932
			.parseStringP(currency) // PYR-28924
			;
	}

	void compose(CommMsgBody& body) const
	{
		body
			.composeUINT32(realMoney)
			.composeUINT32(playMoney)
			.composeUINT32(wMoney)
			.composeUINT32(tMoney)
			.composeUINT32(fpp)
			.composeUINT32(scalePM) // PYR-23932
			.composeString(currency) // PYR-28924
			;
	}
};

struct TournBuyIn
{
	UINT32 tournId;
	SrvTime startTime;
	UserFiveContainer buyIn;
	PString refundCurrency;

	TournBuyIn()
	{
		tournId = 0;
		startTime.setNull();
	}

	void parse(CommMsgParser& parser)
	{
		parser.parseUINT32(tournId);
		startTime.parse(parser);
		buyIn.parse(parser);
		parser.parseStringP(refundCurrency);
	}

	void compose(CommMsgBody& body) const
	{
		body.composeUINT32(tournId);
		startTime.compose(body);
		buyIn.compose(body);
		body.composeString(refundCurrency);
	}
};



// Does NOT empty vector!!!
// [AO 2009-12-11] Returns the number of items parsed/composed
UINT32 parseStringVector( CommMsgParser& parser, vector<PString>& strings );
UINT32 parseStringVectorWithInitSize( CommMsgParser& parser, vector<PString>& strings, UINT32 initSize );	// PYR-52366
UINT32 composeStringVector( CommMsgBody& body, const vector<PString>& strings );
UINT32 parseUint32Vector( CommMsgParser& parser, vector<UINT32>& vec );
UINT32 parseUint32VectorWithInitSize( CommMsgParser& parser, vector<UINT32>& vec, UINT32 initSize );	// PYR-52366
UINT32 composeUint32Vector( CommMsgBody& body, const vector<UINT32>& vec );
UINT32 parseInt32VectorOverwrite( CommMsgParser& parser, vector<INT32>& vec );
UINT32 composeInt32Vector( CommMsgBody& body, const vector<INT32>& vec );
UINT32 parseUint64Vector( CommMsgParser& parser, vector<UINT64>& vec );
UINT32 parseUint64VectorWithInitSize( CommMsgParser& parser, vector<UINT64>& vec, UINT32 initSize );	// PYR-52366
UINT32 composeUint64Vector( CommMsgBody& body, const vector<UINT64>& vec );
UINT32 parseInt64Vector( CommMsgParser& parser, vector<INT64>& vec );
UINT32 composeInt64Vector( CommMsgBody& body, const vector<INT64>& vec );
UINT32 parseInt16Vector( CommMsgParser& parser, vector<INT16>& vec );
UINT32 composeInt16Vector( CommMsgBody& body, const vector<INT16>& vec );
// parseVector & composeVector works with T that implements parse & compose.
template<class T>
void parseVector(CommMsgParser& parser, vector<T>& vec )
{
	UINT32 _size;
	parser.parseUINT32(_size);
	vec.resize(_size);
	for(size_t i = 0; i < _size; ++i)
	{
		vec[i].parse(parser);
	}
}
template<class T>
void composeVector(CommMsgBody& body, const vector<T>& vec )
{
	body.composeUINT32(vec.size());
	for(size_t i = 0; i < vec.size(); ++i)
	{
		vec[i].compose(body);
	}
}

// the following 3 functions use UINT16 for the size of vector/set instead of UINT32
UINT16 parseStringVector_SizeUINT16( CommMsgParser& parser, vector<PString>& strings );
UINT16 composeStringVector_SizeUINT16( CommMsgBody& body, const vector<PString>& strings );
UINT16 parsePStringSet_SizeUINT16( CommMsgParser& parser, PStringSet& vec );

// [AO 2010-04-19] Faster but overwrites original vector
UINT32 parseUint32VectorOverwrite( CommMsgParser& parser, vector<UINT32>& vec );

UINT32 parseUint32Set( CommMsgParser& parser, set<UINT32>& s );
void composeUint32Set( CommMsgBody& body, const set<UINT32>& s );

UINT32 composeBoolVector( CommMsgBody& body, const vector<bool>& vec );
UINT32 parseBoolVectorOverwrite( CommMsgParser& parser, vector<bool>& vec );

// [AO 2010-04-19] Commonly used, refactored.  Does not empty map
UINT32 parseUint32PstringMap( CommMsgParser& parser, PStringMap<UINT32>& psMap );
UINT32 composeUint32PstringMap( CommMsgBody& body, const PStringMap<UINT32>& psMap );

UINT32 parseInt32PstringMap( CommMsgParser& parser, PStringMap<INT32>& psMap );
UINT32 composeInt32PstringMap( CommMsgBody& body, const PStringMap<INT32>& psMap );

UINT32 composePStringSet( CommMsgBody& body, const PStringSet& strings );
UINT32 parsePStringSet( CommMsgParser& parser, PStringSet& strings ); // does not clear the set

void composeIniFile(const PIniFile& ini, CommMsgBody& body);
void composeIniFileEx(const PIniFileEx& ini, CommMsgBody& body);
void parseIniFile(PIniFile& ini, CommMsgParser& parser);
void parseIniFileEx(PIniFileEx& ini, CommMsgParser& parser);

//bool strToDateTime( const char* dateTimeStr, int& year, int& month, int& day, int& hour, int& minute, char dateDelim = '-', char timeDelim = ':' );
bool strToSrvTime( const char* dateTimeStr, SrvTime& srvTime, char dateDelim = '-', char timeDelim = ':' );
bool strToSrvDate( const char* dateStr, SrvDate& srvDate, char dateDelim = '-' );
bool xmlDateTimeToSrvTimeLocal( const char* const xmlDateTime, SrvTime& srvTimeLocal, SrvTime* srvTimeUtc = NULL ); // xml dateTime type to SrvTime in local tz
bool convertStringFromHex( const char* strValue, PBlock& ret ); // hex encoded string to binary array
const char* convertHexToString( const BYTE* binArray, UINT32 binSize, PString& val ); // binary array to hex encoded string
void convertStringToMacAddress(const PString& src, UINT64& dst); // hex string with delimiters to UINT64
void convertMacAddressToString(UINT64 src, PString& dst); // UINT64 to hex string with delimiters
const char* srvTimeToString(const SrvTime& srvTime, PString& strValue, char dateSeparator='-', char middleSeparator=' ', char timeSeparator=':');
const char* srvTimeToString(const CommUtcTime& srvTime, PString& strValue, char dateSeparator='-', char middleSeparator=' ', char timeSeparator=':');
inline const char* srvTimeToStringForDb(const SrvTime& srvTime, PString& strValue) { return srvTimeToString(srvTime, strValue, '-', '-', '.'); }
inline const char* srvTimeToStringForDb(const CommUtcTime& srvTime, PString& strValue) { return srvTimeToString(srvTime, strValue, '-', '-', '.'); }
const char* srvDateToString(const SrvDate& srvDate, PString& strValue, char dateSeparator='-');
bool olderThanFullYears(const SrvDate& birthDate, const SrvTime& srvNow, UINT32 fullYears);
SrvTime currentLocalTime(); // obsolete, use TimeUtils::getCurrentLocalTime() from TimeUtils.h instead
INT32 srvDateToReadableInt(const SrvDate& d);
INT32 srvDateToReadableInt(const SrvTime& d);
const SrvDate& readableIntToSrvDate(INT32 readableDate, SrvDate& d);
INT64 srvTimeToReadableInt64(const SrvTime& t);
INT64 utcTimeToReadableInt64(const CommUtcTime& t);
bool containsDigitsOnly(const char* str); // @return true if all characters are digits, for empty string returns true as well


bool isSrvDateValid(const SrvDate& d); // PYR-49672

int calcTournamentPoints( int place, int placeTo, int numPlayers, int buyIn, int fppBuyIn, BYTE numFormula, UINT32 tournFlags, double& nonRoundedPoints );
void readDbmSyncScriptFromFile(CommMsgBody & syncMsg, const PIniFile & scriptFile);

inline char boolToLogChar(bool b) { return b ? 'y' : 'n'; }
const char* strVectorToStr(PString& str, const vector<PString>& strVec);
const char* strVectorToStr(PString& str, const vector<PString>& strVec, char delimeter);
const char* strSetToStr(PString& str, const PStringSet& strSet);
const char* strSetToStr(PString& str, const PStringSet& strSet, char delimeter);
const char* stlStrSetToStr(PString& str, const set<PString, PStringCmp>& strSet);
const char* stlStrSetToStrIgnoreCase(PString& str, const set<PString, stringLessIgnoreCase>& strSet);
const char* stlStrSetToStr(PString& str, const set<PString, PStringCmp>& strSet, char delimeter);
const char* stlStrSetToStrIgnoreCase(PString& str, const set<PString, stringLessIgnoreCase>& strSet, char delimeter);
const char* uintSetToStr(PString& str, const set<UINT32>& uintSet);
const char* uintSetToStr(PString& str, const set<UINT32>& uintSet, char delimeter);
const char* multimapStrStrToString(PString& str, const multimap<PString, PString, PStringCmp>& mm);

	// does not trim, ignores empty strings
void readStrVectorFromString(const char* str, vector<PString>& strVec);
void readStrVectorFromString(const char* str, vector<PString>& strVec, char delimeter, bool keepEmpty, bool trim);
void readStrVectorFromString(const char* str, vector<PString>& strVec, char delimeter);
void readStrSetFromString(const char* str, PStringSet& strSet);
void readPStringSetFromString(const char* str, PStringSet& strSet, char delimeter, bool toUpper, bool trim);

void readStlStrSetFromString(const char* str, set<PString, PStringCmp>& strSet);
void readStlStrSetFromStringIgnoreCase(const char* str, set<PString, stringLessIgnoreCase>& strSet); // PYR-51386

bool removeStringFromStrVector(const char* str, vector<PString>& strVec);

template<class INT32_TYPE>
void readIntSetFromString( const char* str, set<INT32_TYPE>& uintSet, char delimeter ) // implementation is copied from readUintSetFromString
{
	if(!str || !*str)
		return;
	const char* p = str;
	PString str_tmp;
	const char* commaPtr = strchr(str, delimeter);
	while(commaPtr)
	{
		str_tmp.assign(p, commaPtr);
		if(*str_tmp)
		{
			uintSet.insert(atoi(str_tmp));
		}
		p = commaPtr + 1;
		if(*p)
			commaPtr = strchr(p, delimeter);
		else
			break;
	}
	if(*p)
	{
		uintSet.insert(atoi(p));
	}
}
void readUintSetFromString(const char* str, set<UINT32>& uintSet, char delimeter);
void readUintLicenceIdSetFromString(const char* str, set<UINT32>& uintSet, const char delimeter =','); // PYR-98749 Does NOT clear before insert
void readMultimapStrStr(const char* str, multimap<PString, PString, PStringCmp>& result,
	char delimPairs = ',', char delimInsidePair = ':', bool trim = true); // PYR-114093 Does clear result before inserts

const char* centsToString(PString& out, INT64 amountInCents);
int getBinTypeNumber(const PString& binType);

// PYR-27470
class CompareStringUtils
{
	static unsigned char _toUpper( unsigned char c );

public:
	static bool _endsWithIgnoreCase( const char* s, const char* s2 );
	static bool _startWithIgnoreCase( const char* s, const char* s2);
};


// [2012-4-25 lingz] trim both the leading and trailing spaces of all strings in a vector, with empty string removed
void trimSpaces(const vector<PString>&inputStrs, vector<PString>& outputStrs); 

const char* cutStrAtDelimiter(PString& str, const PString& delimiter);
	// used to remove tags from tournament names
const char* removeHtmlTags(PString& cleanStr, const char* strWithTags);

////////////////////// FPP Store Items ////////////////////////////

#define CHIPS_AWARDED_NAME				"Chips Awarded"
#define BONUS_ID_NAME					"Bonus"

////////////////////// FPP Store Items END ////////////////////////////

struct NameValuePair
{
	PString name;
	PString value;
	NameValuePair(const char* pair_name, const char* pair_value)
	{
		name = pair_name;
		value = pair_value;
	}
};


const char* countryName( const char* code );
const char* getStateCodeByName(const char* stateName, const char* countryCode);
int StrToCents( const char* str_p );

INT64 inverseConvRate(INT64 convRate);

#define DO_ROUND_CONVRATE true

// ConvRatesAndMargins were moved to srvshared/sharedconversion and back on 2016-05-26
struct ConvRatesAndMargins
{
	PString curSrc;
	PString curDst;
	enum ConversionActionType actionType;
	INT64 rateSrc, rateDst; // rates Src/US , Dst/US, both are < INT32_MAX
	INT64 rateMid, rateFin; // rates Src/Dst, Src/Dst+mgn, might exceed INT32_MAX
	INT32 mgnVal; // conversion margin value x MARGIN_MULTIPLIER

	struct Amounts
	{
		INT32 amtSrc;
		INT32 amtSrcUS;
		INT32 amtDstUS;
		INT64 amtDst; // for 'cheap' currencies might exceed int32 limit
		INT64 amtDstNoMgn;
		// margin amounts in corresponding currencies
		INT32 amtMgnSrc;
		INT32 amtMgnDst;
		INT32 amtMgnUS;

		void add (const Amounts& another)
		{
			amtSrc   += another.amtSrc;
			amtSrcUS += another.amtSrcUS;
			amtDst   += another.amtDst;
			amtDstUS += another.amtDstUS;
			amtDstNoMgn += another.amtDstNoMgn;
			amtMgnSrc += another.amtMgnSrc;
			amtMgnDst += another.amtMgnDst;
			amtMgnUS  += another.amtMgnUS;
		}
		void clear()
		{
			amtMgnUS = amtMgnSrc = amtMgnDst = 0;
			amtSrc = amtSrcUS = amtDstUS = 0;
			amtDst = amtDstNoMgn = 0;
		}
		void init(INT32 amtSrc_)
		{
			amtMgnUS = amtMgnSrc = amtMgnDst = 0;
			amtSrc = amtSrcUS = amtDstUS = amtSrc_;
			amtDst = amtDstNoMgn = amtSrc_;
		}
		bool equals(const Amounts& a) const
		{
			return (amtSrc      == a.amtSrc
				&&	amtSrcUS    == a.amtSrcUS
				&&	amtDstUS    == a.amtDstUS
				&&	amtDst      == a.amtDst
				&&	amtDstNoMgn == a.amtDstNoMgn
				&&	amtMgnSrc   == a.amtMgnSrc
				&&	amtMgnDst   == a.amtMgnDst
				&&	amtMgnUS    == a.amtMgnUS);
		}
	};
	Amounts amounts;

	ConvRatesAndMargins()
	{
		clear();
	}
	void clear()
	{
		curSrc = curDst = "";
		rateSrc = rateDst = rateMid = rateFin = CONV_RATE_BASE;
		mgnVal = 0;
		actionType = eConvActionZeroMargin;
		amounts.clear();
	}
	INT64 calculateConvAmounts(INT32 amtSrc);
	bool conversionExists() const;
	void parse(CommMsgParser& parser);
	void compose(CommMsgBody& body) const;
	void reverseConversionDirection(); // swaps Src and Dst
	bool isUp1Conversion() const; // if destination amount worth more than a source
	bool removeUp1Conversion(); // cleares amounts if Up1 conversion exists

	static void composeVector(CommMsgBody& body, const vector<ConvRatesAndMargins>& vec);
	static void parseVector(CommMsgParser& parser, vector<ConvRatesAndMargins>& vec);
	static void mergeVectors(vector<ConvRatesAndMargins>& vecIO, const vector<ConvRatesAndMargins>& vecToMerge);
	static void mergeIntoVector(vector<ConvRatesAndMargins>& vecIO, const ConvRatesAndMargins& cmToMerge);
	static const char* vectorToString(PString& out, const vector<ConvRatesAndMargins>& vec);
};

void composeCurrencyContext(CommMsgBody& currencyContext, const SrvTime& srvTime, const vector<ConvRatesAndMargins>& cmVec);
void parseCurrencyContext(CommMsgParser& currencyContext, SrvTime& srvTime, vector<ConvRatesAndMargins>& cmVec);
bool accountIsSourceNotDest(enum ConversionActionType actionType);
void splitFullName( PString& firstName, PString& inBetween, PString& lastName, const char* fullName );
const char* splitLastAndSecondName(const char* lastAndSecondName, const char* secondName, PString& onlyLastName); // #21003

/* MONEYGRAM is obsolete - #16411
#define MG_STATUS_ISSUED			0 //PS id given to user
#define MG_STATUS_AUTOCOMPLETED		1 //PS id received from MG, user credited
#define MG_STATUS_ADMIN_COMPLETED	2 //was unmatched, completed by admin
#define MG_STATUS_SUPERCEEDED		3 //was issued, then another unmatched was completed by admin, superceeding this one
#define MG_STATUS_REUSED			4 //same PS id was already used at least once
#define MG_STATUS_UNMATCHED			5 // PS id was not found among issued PS ids (typo by user?)
#define MG_STATUS_LIMIT_PENDING		6 // was issued, but couldn;t autocomplete because limit was exceeded
#define MG_STATUS_REUSED_PENDING	7 // same as reused, but user was not credited because of configuration parameter
#define MG_STATUS_REUSED_APPROVED	8 // was reused_pending, approved by admin
#define MG_STATUS_LIMIT_APPROVED	9 // was limit_pending, approved by admin
#define MG_STATUS_NOTALLOWED		10 // user id banned or closed or self-excluded
#define MG_STATUS_NOTALLOWED_APPROVED	11 // was not_allowed, approved by admin
#define MG_STATUS_IGNORED			12 // bug#5330 - a final state, where no funds are credited to the player
*/

bool parseAchAccount(const char* achAcct, PString& routingNum, PString& accountNum, PString& accountType);

struct WireAccountInfo
{
	WireAccountInfo()
	{
		cc_id = 0;
		status = eWireAccountStatus_Created;
        bankValidationCode = eDbtWireBankValidationWasNotValidated;
	}
	PString userId; //user that requested to save his info

	eWireAccountStatus status; 

	SrvTime when;
	UINT32	cc_id;
    INT16   bankValidationCode;

	struct HolderInfo 
	{
		PString	fullName;
		PString phone;
		PString	address;
		PString	city;
		PString	state;
		PString	zip;
		PString countryCode;
		void compose(CommMsgBody& body) const;
		void parse(CommMsgParser& parser);
	} holder;

	struct BankInfo 
	{
		PString userAccount; // content is context specific: might be crypted, might be plain
		PString currencyCode;
		PString iban;
		PString	swiftCode;
		PString	bankName;
		PString	bankAddr;
		PString	comments;
		void compose(CommMsgBody& body) const;
		void parse(CommMsgParser& parser);
	} bank;

	PString maskedCardNum;
	PString userAccountPlain;
	void compose(CommMsgBody& body) const;
	void parse(CommMsgParser& parser);
};

struct UserFrBankAccount
{
    bool    isValid;
    PString bankName;
	PString bankCountry; // #15111
    PString accountNumberCrypted; // FR,PT,CZ - plain acct prefixed with "00" after #23636 completion; DE - encrypted
    PString accountNumberDecrypted;
    PString reference;

    UserFrBankAccount() : isValid(false) {}
    void parse(CommMsgParser& parser)
    {
        parser
            .parseStringP(bankName)
            .parseStringP(accountNumberCrypted)			
            .parseStringP(reference)
            .parseBOOL(isValid)
			.parseStringP(bankCountry);
		// conditional parsing

		if (accountNumberCrypted.startsWith("00")) // if account is crypted we should decrypt it 
		{
			accountNumberDecrypted = accountNumberCrypted.c_str() + 2;
		}
		else
		{
			accountNumberDecrypted = "";
		}
    }
    void compose(CommMsgBody& body) const
    {
        body
            .composeString(bankName)
            .composeString(accountNumberCrypted)			
            .composeString(reference)
            .composeBOOL(isValid)
			.composeString(bankCountry);
		// append only - after PYR-54565 the struct is serializable
    }

	bool equalsIgnoreValidity(const UserFrBankAccount& other) const
	{
		return bankName.equals(other.bankName)
			&& bankCountry.equals(other.bankCountry)
			&& accountNumberCrypted.equals(other.accountNumberCrypted)
			&& reference.equals(other.reference)
			;
	}

};

struct Vendor
{
	UINT32 vendorId;
	PString name;
	PString descr;
	UINT32 adminIntId;

	Vendor()
	{
		vendorId = adminIntId = 0;
	}

	void composeData(CommMsgBody& body) const;
	void parseData(CommMsgParser& parser);
};

struct Product
{
	UINT32 productId;
	UINT32 vendorId;
	PString descr;
	UINT32 price;
	PString currency;
	PString allowedCountries;
	UINT32 adminIntId;

	Product()
	{
		productId = vendorId = price = adminIntId = 0;
	}

	void composeData(CommMsgBody& body) const;
	void parseData(CommMsgParser& parser);
};

enum eClaimCodeStatus
{
	eClaimCodeOpen = 0,
	eClaimCodeIssued = 1,
	eClaimCodeCancelled = 2
};

struct ClaimCode
{
	UINT32 claimCodeId;
	UINT32 productId;
	PString claimCode;
	SrvDate expDate;
	UINT32 orderId;
	eClaimCodeStatus status;
	UINT32 adminIntId;
	SrvTime created; // PYR-18240

	ClaimCode()
	{
		claimCodeId = productId = orderId = adminIntId = 0;
		status = eClaimCodeOpen;
	}

	void composeData(CommMsgBody& body) const;
	void parseData(CommMsgParser& parser);
};

struct GiftCardInfo
{
	UINT32 specialType;
	PString claimCode;
	PString vendorName;
	PString vendorDescr;
	UINT32 price;
	PString currency;
	SrvDate expDate; // PYR-23546
	
	GiftCardInfo()
	{
		specialType = price = 0;
	}

	void composeData(CommMsgBody& body) const;
	void parseData(CommMsgParser& parser);
};

struct IniDataFromDbm
{
	enum
	{
		eUnique		= 0,
		eNonUnique  = 1
	};
	class Item
	{
	public: //lint --e{1925}
		PString name;
		PString value;
		int		type;
	public:
		Item() : type(0) {}
		Item( const char* name_, const char* value_, int type_ )
			: name( name_ ), value( value_ ), type(type_) {}
	};//Item
	
	class Section
	{
	public: //lint --e{1925}
		PString name;
		vector< Item > items;
	public:
		Section() {}
		explicit Section( const char* name_ ) : name( name_ ){}
        const char* getProperty( const char* name_, const char* defaultValue = NULL ) const;
        int getIntProperty(const char* name_, int defaultValue, int radix = 10) const
        {
            const char* strVal = getProperty(name_);
            char* dummy;
            return strVal ? strtol(strVal, &dummy, radix) : defaultValue;
        }
		INT64 getInt64Property( const char* name_, INT64 defaultValue ) const
		{
			const char* s = getProperty( name_ );
			if( s == 0 )
				return defaultValue;
			return p_atoi64( s );
		}
	};//Section
	vector< Section > sections;

	Section* addSection(const char* secName);

	void compose(CommMsgBody& body) const;
	void parse(CommMsgParser& parser);

	void merge(PIniFile& file, bool doLog = true);
	const Item* findItem(const char* section_name, const char* item_name, int& secIdx) const;
	void composeAmmendedIniFile(const PIniFile& file, CommMsgBody& iniBody, bool doLog = true) const;
	const char* getDbItemValue(int secIdx, const char* fileItemName, const char* fileItemValue) const;

    const Section* getSection(const char* sectionName) const;
};

////////////////////////////////////////////////////////////////////////////////
// PYR-29604
// Ban server enumerations and structures moved from shared.h/.cpp to BanServerCommon.h/.cpp
////////////////////////////////////////////////////////////////////////////////

struct VIPStoreBonusInfo
{
	UINT32 bonusId; // PYR-89232
	PString bonusName;
	UINT32 mustEarn;
	UINT32 expPeriod;
	SrvTime expDate;
	UINT32 releasePortion;
	UINT32 fppReleasePortion;
	bool isInstantBonus; // [AO 2010-05-27] #10905
	
	VIPStoreBonusInfo()
	{
		bonusId = releasePortion = fppReleasePortion = mustEarn = expPeriod = 0;
		isInstantBonus = false;
	}

	void composeData(CommMsgBody& body) const;
	void parseData(CommMsgParser& parser);
};

struct VIPStoreRollInfo // PYR-31836
{
	UINT32 rollId;
	PString rollName; // for fallback purpose
	UINT32 clearPointsMask; // ePlayerPointsMask
	SrvTime validUntil;
	INT32 clearPoints;
	INT32 clearBets;
	INT32 clearStakes; // PYR-38741
	PString currency; // PYR-38741
	INT32 rollType; // PYR-47059
	INT32 gameMinBet; // PYR-47059 - for FSB gameMinBet contains the exact bet amount, not minimum

	VIPStoreRollInfo() : rollId(0), clearPointsMask(0), clearPoints(0), clearBets(0), clearStakes(0), rollType(/* eRollType::eRollType_Default */0), gameMinBet(0) {}
	void composeData(CommMsgBody& body) const
	{
		body
			.composeUINT32(rollId)
			.composeString(rollName)
			.composeUINT32(clearPointsMask)
			.composeSrvTime(validUntil)
			.composeINT32(clearPoints)
			.composeINT32(clearBets)
			.composeINT32(clearStakes)
			.composeString(currency)
			.composeINT32(rollType)
			.composeINT32(gameMinBet)
			;
		// DO NOT ADD NEW PARAMETERS BELOW - ADD THEM IN VipStorePurchaseHelper::composeReply
	}
	void parseData(CommMsgParser& parser)
	{
		parser
			.parseUINT32(rollId)
			.parseStringP(rollName)
			.parseUINT32(clearPointsMask)
			.parseSrvTime(validUntil)
			.parseINT32(clearPoints)
			.parseINT32(clearBets)
			.parseINT32(clearStakes)
			.parseStringP(currency);

		// PYR-47059
		if (!parser.parseEnded())
		{
			parser
				.parseINT32(rollType)
				.parseINT32(gameMinBet)
				;
		}
	}
};

struct VIPStoreFreeBetInfo // PYR-41444
{
	UINT64 fppTransId;
	UINT64 campaignId; // PYR-79221
	INT32 chipsAwarded;
	PString freeBetCurrency;

	static const UINT32 useCampaignId64 = 0xFFFFFFFF;

	VIPStoreFreeBetInfo() : fppTransId(0), campaignId(0), chipsAwarded(0) {}

	void composeData(CommMsgBody& body) const
	{
		body
			.composeUINT64(fppTransId)
			.composeUINT32(useCampaignId64) // PYR-79221
			.composeINT32(chipsAwarded)
			.composeString(freeBetCurrency)
			.composeUINT64(campaignId)  // PYR-79221
			;
		// DO NOT ADD NEW PARAMETERS BELOW - ADD THEM IN VipStorePurchaseHelper::composeReply
	}

	void parseData(CommMsgParser& parser)
	{
		UINT32 dummyCampaignId;  // PYR-79221
		parser
			.parseUINT64(fppTransId)
			.parseUINT32(dummyCampaignId)
			.parseINT32(chipsAwarded)
			.parseStringP(freeBetCurrency)
			;
		if (dummyCampaignId == useCampaignId64)   // PYR-79221
		{
			parser.parseUINT64(campaignId);
		}
		else
		{
			campaignId = dummyCampaignId;
		}
	}
};

bool i18n_locale_valid(UINT32 locale);
UINT32 i18n_get_emLocale(UINT32 emLocale, UINT32 locale);

/////////////////////////////////// #7214 - affiliate deposits ////////////////
enum eAffDepositUniqueness
{
	eUniquePerDeal = 1,
	eUniquePerAffiliate = 2,
	eUniqueGlobally = 3
};
#define affDepositUniqueMask		3 //uses first two bits, see enum eAffDepositUniqueness
#define affDepositDebitOnExpiry		(1 << 2) //4 if set, then debit user after expiry date, unless he collected targetFpp
#define affDepositDebitToNegative	(1 << 3) //8 if set, when debiting user, allow negative balance, otherwise only take what's in user's bankroll
#define affDepositCheckAffiliation	(1 << 4) //16 if set, check if user is indeed has a promo that belongs to this affiliate

/////////////////////////////////// #7214 - affiliate deposits end ////////////////

struct TLBPrize
{
	PString userId;
	UINT32 intervalId;
	UINT32 division;
	UINT32 subdivision;
	INT32 amount;
	PString currency;
	PString comment;

	TLBPrize()
	{
		intervalId = division = subdivision = amount = 0;
	}
	
	void composeData(CommMsgBody& body) const;
	void parseData(CommMsgParser& parser);
	void parse(CommMsgParser& parser) // required for templates to work
	{
		parseData(parser);
	}
	void log() const;
};

struct FrozenObject
{
	UINT32 objectId;
	UINT32 objectType;
	PString objectValue;
	SrvTime expDate;
	UINT64 sessionId; // #9576 - increased size
	SrvTime created;
	UINT32 adminIntId;
	PString comment;
	SrvTime forcedExpDate;
	UINT32 forcedAdminIntId;
	PString forcedCommet;
	UINT32 freezeReason; // PYR-24890

	FrozenObject() : objectId(0), objectType(0), sessionId(0), adminIntId(0), forcedAdminIntId(0), freezeReason(0)
	{
	}

	void composeData(CommMsgBody& body) const;
	void parseData(CommMsgParser& parser);
};

struct FreezeEvent
{
	UINT32 eventId;
	UINT32 userIntId;
	UINT32 objectId;
	SrvTime when;
	PString installId;
	PString volumeLabel;
	PString macAddress;
	PString routerMacAddress;
	PString ipAddress;
	PString extraInfo;

	FreezeEvent()
	{
		eventId = userIntId = objectId = 0;
	}

	void composeData(CommMsgBody& body) const;
	void parseData(CommMsgParser& parser);
};

struct FrozenObjectInfo
{
	UINT32 objectId;
	SrvTime expDate;
	UINT32 freezeReason; // PYR-24890

	FrozenObjectInfo() : objectId(0), freezeReason(0)
	{
	}
};

void unwrapReplayMessageBody(const CommMsgBody& bodyIn, CommMsgBody& bodyOut, SrvTime& msg_time, time_t& msg_t);
void wrapReplayMessageBody(const CommMsgBody& bodyIn, CommMsgBody& bodyOut, const SrvTime& msg_time, time_t msg_t);

	// only first char of 'sep' used, trims L/R spaces/tabs, allows empty string
	// '"' quotas used to enclose string with 'sep'
void strTokenize(const char* p, vector<PString>& tokens, const char* sep);

void readStrVectorFromIniFile(const PIniFile& iniFile, const char* sectionName, const char* itemName, const char* defaultValue, vector<PString>& strItems);

bool isStringInVector(const char* str, const vector<PString>& vec, bool ignoreCase = false);
PString& urlEncodeValue(const char* value, PString& encodedValue, bool notLast = true);

void encodeAsTransactionComment(INT32 value, PString& comment);

void normalizeEmail(const char* email, PString& normalizedEmail);
void genTmpPwd( PString& out );

const char* getItemValue(PString& out, const PIniFile::Item& item);

bool checkItemSiteId(UINT32 userSiteId, UINT32 itemMask); 

//	<<== @license2-tld: TLD for license-related domain @ ====
#define DOMAIN_NAME_COM "com"
#define DOMAIN_NAME_NET "net"
#define DOMAIN_NAME_IT "it"
#define DOMAIN_NAME_FR "fr"
#define DOMAIN_NAME_EE "ee"
#define DOMAIN_NAME_BE "be"
#define DOMAIN_NAME_ES "es"
#define DOMAIN_NAME_DK "dk"
#define DOMAIN_NAME_EU "eu"
#define LICENCE_NAME_USNJ "usnj" 		//	it's not domain TLD, rather to support <LICENSE> tag indicating license.
#define DOMAIN_NAME_UK "uk"
#define DOMAIN_NAME_BG "bg"
#define DOMAIN_NAME_DESH "desh" 		//	it's not domain TLD, rather to support <LICENSE> tag indicating license.
#define DOMAIN_NAME_ROM "ro"
#define DOMAIN_NAME_PT "pt"
#define DOMAIN_NAME_CZ "cz"
#define DOMAIN_NAME_IN "in"
#define DOMAIN_NAME_RU "ru"
#define LICENCE_NAME_USPA "uspa" 		//	it's not domain TLD, rather to support <LICENSE> tag indicating license.
#define DOMAIN_NAME_SE "se"
#define DOMAIN_NAME_CH "ch"
#define LICENCE_NAME_ARBA "arba" 		//	it's not domain TLD, rather to support <LICENSE> tag indicating license.
#define LICENCE_NAME_USCO "usco" 		//	it's not domain TLD, rather to support <LICENSE> tag indicating license.
#define DOMAIN_NAME_DE "de"
#define DOMAIN_NAME_GR "gr"
//	==== @license2-tld: TLD for license-related domain @ ==>>

// Temporary - should be removed
const char* getDomainByLicenseOld(UINT32 accountLicenseId);
const char* getFullDomainByLicense(UINT32 accountLicenseId);
// Returns brand name PokerStars.xxx
const char* getBrandNameByLicense(UINT32 accountLicenseId);
// Template server is still using it for tag LICENSE!!! As use of this tag become obsolete this function should be removed
const char* getDomainBySite(UINT32 siteId);
const char* getFullDomainBySite(UINT32 siteId, const char* country = NULL);
const char* getBrandNameBySite(UINT32 siteId);

struct UserProperty
{
	INT32   propType; // enum eUserPropertyType
	INT32   propInt;
	PString propStr;
	SrvTime	propWhen;
	// do not add any new composable/parsable members before reviewing/fixing existing APIs with UserProperty

	UserProperty()
	{
		propInt = propType = 0;
		propWhen.setNull();
	}
	UserProperty(eUserPropertyType propType_, int propInt_, const char* propStr_, const SrvTime& propWhen_) :
		propType(propType_), propInt(propInt_), propStr(propStr_), propWhen(propWhen_) 
	{}
	void compose(CommMsgBody& body) const;
	void parse(CommMsgParser& parser);
	static bool findInVector(const vector<UserProperty>& userPropsVec, int propType, UserProperty& userProp);
	static const UserProperty* findInVector(const vector<UserProperty>& userPropsVec, int propType);
	static UserProperty* findInVector(vector<UserProperty>& userPropsVec, int propType);
};

// PYR-18101
struct UserSpecialInstructions
{
	int     departmentId;
	PString instructions;
	SrvTime	when;

	UserSpecialInstructions() : departmentId(0) {}
	UserSpecialInstructions(eSpecialInstructionsType departmentId_, const char* instructions_, const SrvTime& when_) :
	departmentId(departmentId_), instructions(instructions_), when(when_) 
	{}
	void compose(CommMsgBody& body) const;
	void parse(CommMsgParser& parser);
};

struct EnvoyBankInfo
{
	UINT32  bankrefId;
	PString currency;
	PString name;
	PString city;
	PString address;
	PString swift;
	PString url;
	PString accountNum;
	PString iban;
	PString benficiary;
	PString extraInfo;
	bool	preferred;
	// #16406
	PString paysystem;
	PString bankId; // bank ID used for inter-bank inter-processors communication
	PString comment; // for PSTARSPAY it's processing time
	UINT32  flags; // see enum eEnvoyBankInfoFlags (b0:bank use NO reference)
	PString country;

	EnvoyBankInfo() : bankrefId(0), preferred(false), paysystem(ENVOY_LBT_NAME), flags(0) {}
	void parse(CommMsgParser& parser);
	void compose(CommMsgBody& body) const;
};

struct EnvoyCountryBanks
{
	PString country;
	vector<EnvoyBankInfo> banks;
	void parse(CommMsgParser& parser);
	void compose(CommMsgBody& body) const;
};

// #20109 new processor uses envoyref table now need to set the processor flag!!!
enum EnvoyLbtReferenceFlags
{
	eEnvoyLbtReferenceFlagsForEnvoy = 0, // Envoy, Lobanet(pstarspay)
	eEnvoyLbtReferenceFlagsForLobanetError = 1, // Lobanet save bank info error
	eEnvoyLbtReferenceFlagsForInstantBank = 2, // GlueFinance processor
	eEnvoyLbtReferenceFlagsForPayNearMe = 3, // PayNearMe processor
};

struct EnvoyLbtReference
{
	//table reference:PYRFIN.ENVOYREF
	PString psRefId;
    PString prefix;
    PString userid;
    PString fullname;    
    PString currency;
    PString bankName;
    PString bankAccount;
    PString bonus;
    UINT32  amount;
    UINT32  bankRefid;
	//#16406 new fields for PSTARSPAY
	UINT32  customAmount;
	PString extras;
	UINT32  flags; // #20109 EnvoyLbtReferenceFlags

    //#internal fields, not in compose and parse
	UINT32 liscense;
	UINT32 userIntId;
	PString country;
	PString prefLanguage;
	PString ipAddress;
	PString firstName;
	PString lastName;
	
	EnvoyLbtReference() : amount(0), bankRefid(0), customAmount(0), flags(0), liscense(0), userIntId(0) {}

	void parse(CommMsgParser& parser)
    {        
        parser.parseStringP(psRefId)
			.parseStringP(prefix)
            .parseStringP(userid)
            .parseStringP(fullname)
            .parseStringP(currency)
            .parseStringP(bonus)
            .parseStringP(bankName)
            .parseStringP(bankAccount)
            .parseUINT32(amount)
            .parseUINT32(bankRefid)		    
			.parseUINT32(customAmount)
			.parseStringP(extras)
			.parseUINT32(flags)
			.parseUINT32(liscense) // #20294
			;
    }
    void compose(CommMsgBody& body) const
    {
        body.composeString(psRefId)
			.composeString(prefix)
            .composeString(userid)
            .composeString(fullname)
            .composeString(currency)
            .composeString(bonus)
            .composeString(bankName)
            .composeString(bankAccount)
            .composeUINT32(amount)
            .composeUINT32(bankRefid)			
			.composeUINT32(customAmount)
			.composeString(extras)
			.composeUINT32(flags)
			.composeUINT32(liscense) // #20294
			;
    }
};

struct EnvoyLbtTrans
{
    UINT32  transId;
    UINT32  amount;
    PString currency;
    PString bankInfo;
    PString country;
    PString epacsRef;
    PString merchantRef;
    PString uniqueRef;
    PString transType;
    PString status;
    PString comment;
    PString userid;
    PString fullname;
    PString bonuscode;
    SrvTime currentTime;

    void parse(CommMsgParser& parser)
    {        
        parser
			.parseUINT32(transId)
            .parseUINT32(amount)
            .parseStringP(currency)
            .parseStringP(bankInfo)
            .parseStringP(country)
            .parseStringP(epacsRef)
            .parseStringP(merchantRef)
            .parseStringP(uniqueRef)
            .parseStringP(transType)
            .parseStringP(status)
            .parseStringP(comment)
            .parseStringP(userid)
            .parseStringP(fullname)
            .parseStringP(bonuscode)
			.parseSrvTime(currentTime);
    }
    void compose(CommMsgBody& body) const
    {
        body.composeUINT32(transId)
            .composeUINT32(amount)
            .composeString(currency)
            .composeString(bankInfo)
            .composeString(country)
            .composeString(epacsRef)
            .composeString(merchantRef)
            .composeString(uniqueRef)
            .composeString(transType)
            .composeString(status)
            .composeString(comment)
            .composeString(userid)
            .composeString(fullname)
            .composeString(bonuscode)
			.composeSrvTime(currentTime);
    }

	EnvoyLbtTrans()
	{
		transId = 0;
		amount = 0;
	}
};

struct EnvoyLbtTransEx : public EnvoyLbtTrans
{
	UINT32 customAmount;
	PString operatorName;
//	PString gateway;
//	PString paysystem;

	EnvoyLbtTransEx() : EnvoyLbtTrans(), customAmount(0) {}
    void parse(CommMsgParser& parser)
    { //lint !e1511 (hiding)
		EnvoyLbtTrans::parse(parser);
		parser
			.parseUINT32 (customAmount)
			.parseStringP(operatorName)
//			.parseStringP(gateway)
//			.parseStringP(paysystem)
			;
	}
    void compose(CommMsgBody& body) const
	{ //lint !e1511 (hiding)
		EnvoyLbtTrans::compose(body);
		body
			.composeUINT32(customAmount)
			.composeString(operatorName)
//			.composeString(gateway)
//			.composeString(paysystem)
			;
	}
};

bool isAlmostTheSame(INT32 first, INT32 second, UINT32 tol_x_ONE_MILLION);
INT64 mulDivRnd64        (INT64 number, INT64 numerator, INT64 denominator, bool doRoundNotTruncation = false);
INT64 mulDivRnd64NonZero (INT64 number, INT64 numerator, INT64 denominator, bool doRoundNotTruncation = false);
INT64 mulMulDivDiv64 ( INT64 number, INT64 numerator1, INT64 numerator2, INT64 denominator1, INT64 denominator2, bool doRoundNotTruncation = false);
UINT64 makeUINT64(UINT32 intLow, UINT32 intHigh); // concatenates two 32-bit ints into one 64-bit
UINT64 prevPowerOf(UINT32 base, UINT64 number);

void splitStringByChunks(const char* str, UINT32 chunkSize, vector<PString>& chunks);

int datesIntervalExclWeekends(const SrvDate& from, const SrvDate& to);

// split the long string into separate lines and log one by one
void PLogLines( const char* longStr );

bool luhnCheckNumber(const char* str);
char luhnGenerateChar(const char* str);

BYTE getCashTransactionsCounter(UINT64 flags);
const char* paysystemGroupNameToEnglishName(const char* groupName); // #14352

// #14492
struct UserAllAssetsConversion
{
	struct OneAsset
	{
		eAssetType	assetType;
		INT32   	typedAmount;
		PString 	currency;
		INT32   	chipsDst;
		INT64      	convRate;

		OneAsset() : assetType(eAssetTypeUndefined), typedAmount(0), chipsDst(0), convRate(CONV_RATE_BASE) {}
	};

	PString currencyDst;
	INT32 availDst; // This is AVAILABLE total in currencyDst. It does NOT include unspendable uncleared amount.
	vector<OneAsset> assets;

	UserAllAssetsConversion() : availDst(0) {}
	void compose(CommMsgBody& body) const;
	void parse(CommMsgParser& parser);
	const char* toString(PString& out) const;
	bool conversionExists() const;
	void addAsset(const OneAsset& asset);
	void clear();
	bool equalsBySrc(const UserAllAssetsConversion& another) const;
	INT32 getTypedAssetTotalDst(eAssetType assetType) const;
};

// these functions mostly related to field validation for Check or Wire processors
const char* replaceCharWithString(PString& sourceStr, char C, const char* str); // char C in sourceStr will be replaced with str
const char* replaceNonAlphaNumWithString(PString& sourceStr, const char* str); // non alphanum char in sourceStr will be replaced with str
// -end

void removeRepresentmentSuffix(PString& gatewayName);

const char* checkAccountByIBAN(const char* iban, const char* account);
const char* removeLeadingZeros(const char* account);

// #14344 this struct used by cashier & gcdbm
struct UnCashedChequeEmailReminder
{
    PString templateName;
    PString userId;
    UINT32 transId;
    INT32 dayReminder;
    INT32 fxAmount;
    INT32 converted;
    PString fxCurrency; // fx currency
    PString procCurrency; // processor currency
    PString xTransStr;  // cheque number
    SrvTime started; // cheque cashout request timestamp

    void parse(CommMsgParser& parser);
    void compose(CommMsgBody& body) const;
    void composeForTemplate(CommMsgBody& body) const; // for sending email

	UnCashedChequeEmailReminder() : transId(0), dayReminder(0), fxAmount(0), converted(0) {}
};

void translateGameType( UINT32 dbGame, BYTE& gameType , BYTE& isHiLo );
int translateTableGame( BYTE gameType, BYTE isHiLo );
int translateTableMultiGame( BYTE gameType, BYTE isHiLo ); // PYR-66899
bool checkUserFilter(const char* userId, const vector<PString>& userFilter);
bool checkUserFilter2(UINT32 userIntId, const vector<PString>& userFilter);
bool isUserIntIdFilter(const PString& filter);

struct UserBalanceData
{
	struct Cash
	{
		PString currency;
		INT32	chips;
		INT32	tChips;
		Cash() : chips( 0 ), tChips( 0 ) {}
		void compose( CommMsgBody& body ) const
		{
			body
				.composeString( currency )
				.composeINT32( chips )
				.composeINT32( tChips );
		}
		void parse(CommMsgParser& parser)
		{
			parser
				.parseStringP( currency )
				.parseINT32( chips )
				.parseINT32( tChips );
		}
	};
	vector< Cash > cashAssets;
	INT64 playChips;
	INT32 fpp;
	INT32 fppCents;
	INT32 nac2; // PYR-33529, Biz NAC

	UserBalanceData() : playChips( 0 ), fpp( 0 ), fppCents( 0 ), nac2 ( 0 ) {}
	void compose( CommMsgBody& body ) const
	{
		body.composeUINT32( cashAssets.size() );
		for( UINT32 i = 0; i < cashAssets.size(); ++i )
		{
			cashAssets[i].compose( body );
		}
		body
			.composeINT64( playChips )
			.composeINT32( fpp )
			.composeINT32( fppCents )
			.composeINT32( nac2 );
	}
	void parse( CommMsgParser& parser )
	{
		UINT32 sz;
		parser.parseUINT32( sz );
		for( UINT32 i = 0; i < sz; ++i )
		{
			Cash cash;
			cash.parse( parser );
			cashAssets.push_back( cash );
		}
		parser
			.parseINT64( playChips )
			.parseINT32( fpp )
			.parseINT32( fppCents )
			.parseINT32( nac2 );
	}
	bool verifyTransfer( const UserBalanceData& transfer ) const;
	bool hasChips() const;
	void plog() const
	{
		for( UINT32 i = 0; i < cashAssets.size(); ++i )
		{
			PLog( "%d,%d(%s)", cashAssets[i].chips, cashAssets[i].tChips, cashAssets[i].currency.c_str() );
		}
		PLog( "%" PRId64 ",%d,%d,%d", playChips, fpp, fppCents, nac2 );
	}
};


// PYR-15453
struct OlympicTable
{
	UINT32 tableIndex;
	UINT64 tableId;
	PString player0;
	PString player1;

	// default constructor
	OlympicTable()
	: tableIndex(0), tableId(0)
	{
	}

	// initialize all members
	OlympicTable( UINT32 tableIndex_, UINT64 tableId_, const char* player0_, const char* player1_ )
	: tableIndex(tableIndex_), tableId(tableId_), player0(player0_), player1(player1_) 
	{
	}

	void parse( CommMsgParser& parser )
	{
		parser
			.parseUINT32(tableIndex)
			.parseUINT64(tableId) // PYR-29205
			.parseStringP(player0)
			.parseStringP(player1);
	}

	void compose(CommMsgBody& body) const
	{
		body.composeUINT32(tableIndex)
			.composeUINT64(tableId) // PYR-29205
			.composeString(player0)
			.composeString(player1);
	}
};

UINT32 countBits(UINT64 mask);

class RequestAverage
{
	UINT64 total;
	UINT32 n;

public:
	void clear()
	{
		total = 0;
		n = 0;
	}
	void add( UINT32 t )
	{
		total += t;
		n++;
	}
	UINT64 getAverage() const
	{
		if( ! n )
			return 0;
		return total / n;
	}
	RequestAverage()
	{
		clear();
	}
};

class CardnumGenerator
{
	PrngXorShift256 randomizer;
	UINT64 confidenceLimit(UINT32 numOfRandomDigits) const; // = ((2^64 / 10^N) * 10^N)
public:
	void reInit() {randomizer.seedTime();}
	const char* generateCardNum(PString& cardNum, UINT32 numOfDigits, const char* prefix = "");
};

// PYR-16389
struct LeagueData
{
    typedef pair<INT64, PString> Int64PStringPair;

    UINT32 leagueId;
    vector<Int64PStringPair> tournPoints;
    vector<UINT32> tourns;
    vector<PString> rankings;
    vector<UINT32> results;
    vector<Int64PStringPair> tournWins;

    LeagueData()
        : leagueId( 0 )
        , tournPointsSent( 0 )
        , tournsSent( 0 ) 
        , rankingsSent( 0 )
        , resultsSent( 0 )
        , tournWinsSent( 0 )
    {}

	explicit LeagueData( UINT32 leagueId_ )
        : leagueId( leagueId_ )
        , tournPointsSent( 0 )
        , tournsSent( 0 ) 
        , rankingsSent( 0 )
        , resultsSent( 0 )
        , tournWinsSent( 0 )
    {}

    void parse( CommMsgParser& parser );
    void compose( CommMsgBody& body );
    void composeForCrop( CommMsgBody& body, UINT32& cropPortion );
    bool hasMoreToSend() const
    {
        return
            tournPointsSent < tournPoints.size()
            || tournsSent < tourns.size()
            || rankingsSent < rankings.size()
            || resultsSent < results.size()
            || tournWinsSent < tournWins.size();
    }

private:
    void composePortion( CommMsgBody& body, UINT32 numTournPoints, UINT32 numRankings, UINT32 numResults, UINT32 numWins, UINT32 numTourns ) const;

    UINT32 tournPointsSent;
    UINT32 tournsSent;
    UINT32 rankingsSent;
    UINT32 resultsSent;
    UINT32 tournWinsSent;
};

vector< PString > parseEncryptedStrings(CommMsgParser& parser);
void parseEncryptedData(CommMsgParser& parser, UINT32 algoId, PString& encryptedStr);
void parseDecryptedData(CommMsgParser& parser, PString& decryptedStr);
void composeStringForDecrypt(const PString& encryptedString, CommMsgBody& msgBody);
void composeStringForEncrypt(const PString& stringToEncrypt, CommMsgBody& body);

// PYR-43573, parses a Block, if !guard, Block is still parsed away
bool parseAndGuardDecryptBody(CommMsgParser& parser, const CommServerGuard* guard, CommMsgBody& resultBody);
bool parseGuardEncryptedString(CommMsgParser& parser, const CommServerGuard* guard, PString& resultString);
bool parseGuardEncryptedStrings(CommMsgParser& parser, const CommServerGuard* guard, PString& resultString1, PString& resultString2);

// PYR-43573, composes a Block, if !guard, functions compose empty Block
void composeAfterGuardEncryptString(CommMsgBody& msgBody, const CommServerGuard* guard, const PString& str);
void composeAfterGuardEncryptStrings(CommMsgBody& msgBody, const CommServerGuard* guard, const PString& str1, const PString& str2);

// PYR-18205
const char* sanitizeStringForLogging(const PString &origStr, PString& sanitizedStr, UINT32 maxLength );

// PYR-15261
void setShowSomeCardsTableFlag( UINT32& tableFlags, UINT32 lobbySetting, bool isPlayMoney, HandType handType );

// normalizes input dates such as "March 32, 2011" to a proper date such as "April 1, 2011".
// numDays is by default set to 0, and is a number of days to add to "inputDate"
bool normalizeSrvDate(const SrvDate& inputDate, SrvDate& outputDate, INT32 numDays = 0);

// normalizes input times such as "10:59:185" to a proper "in-range" time such as "11:02:05".
// numSeconds is by default set to 0, and is a number of seconds to add to "inputTime"
bool normalizeSrvTime(const SrvTime& inputTime, SrvTime& outputTime, INT32 numSeconds = 0);

// PYR-20357
enum UserIdFormatError
{
	TrailingBlanks = 1,
	LeadingBlanks = 2,
	DoubleSpaces = 4,
	ControlChar = 8,
};

UINT32 checkUserIdRules( const char* userId );// PYR-20357

// PYR-20549
// Merging 2 32bit values into 1 64 value and viceversa
UINT64 get64From32(UINT32 intValue, UINT32 intValue2);
void set32From64(UINT64 val64, UINT32& intValue, UINT32& intValue2);

//PYR-20528 
UINT32 getPlayersLimitForCountry( const PString& restriction, const char * countryCode );

// PYR-20580
/* Usage:
    1. call init() to initialize an object
    2. call dynamicInit() to enable or disable event logs.
    3. call writeLog() to write a record into the log. Note userId and userIntId can't be 
       both empty. If it happens, the record will be ignored when the event log is processed
       by importlog utility.
    4. call checkDayChange() with a timer periodically.
    Note: go to EpisTransportServerObject and see how it uses EventLogger as an example.
*/
class RegulatorsEventLogger
{
    UINT32 licenseId;
    PString loggerStationName;
    UINT32 transportIndex;
    bool enableEventLog;
    BYTE logStartDay;

    // the log file will reside in the same directory as the system PLog file
    PLogFile* logFile;
    void createLogFile(const char* eventLogFileName);

    // deal with string that might be empty. If it is empty, return string "''" instead.
    static const char* getString(const char* str)
    {
        return (str && str[0])? str : "''";
    }

public:
    RegulatorsEventLogger()
        : licenseId(eLicenceDefault),
        transportIndex(0),
        enableEventLog(false),
        logStartDay(0),
        logFile(NULL)
    {
    }

    ~RegulatorsEventLogger()
    {
        delete logFile;
    }

    // called by the regulator transport init
    void init(UINT32 licenseId_, const PString& loggerStationName_, 
        UINT32 transportIndex_, const char* eventLogFileName)
    {
        licenseId = licenseId_;
        loggerStationName = loggerStationName_;
        transportIndex = transportIndex_;
        createLogFile(eventLogFileName);
    }

    // called by the regulator transport dynamicInit
    void dynamicInit(bool enableEventLog_)
    {
        enableEventLog = enableEventLog_;
    }

    // write the event to the log file
    void writeLog(const char* messageType, 
        const char* userId, 
        const char* userIntId, 
        const char* licenseProtocol, 
        const char* internalMessageId,
        const char* externalMessageId,
        UINT16 responseCode, 
        const char* responseStr, 
        const char* extraInfo);

    // put a line in the event log file when a new day starts. This ensures that 
    // an event log file with perDate property will be generated and closed every day.
    void checkDayChange();
};

const char* titleEnumToString(eUserTitle eTitle); // not i18n

void processReopenLogFile(CommMsgParser& inParser, CommMsgBody& outBody, UINT32 reqId = 0);

//#22570
struct MigrationGameInfo
{
	//these flags are composed in one byte bitmask:
	enum GameTypeFlags { gtfTourn = 0x1, gtfBlitz = 0x2 };
	enum GameAttributesFlags { gafPlayMoney = 0x20, gafDummy = 0x40 };

	UINT64 id;
	UINT32 sites;
	BYTE type;
	MigrationGameInfo(): id(0), sites(0), type(0){}

	void compose( CommMsgBody &body) const
	{
		body.composeBYTE(type).composeUINT64(id).composeUINT32(sites);
	}
	void parse( CommMsgParser &parser)
	{
		parser.parseBYTE(type).parseUINT64(id).parseUINT32(sites);
	}
	static const char* vectorToStringOfId(PString& s, const vector<MigrationGameInfo>& v)
	{
		s = "";
		s.append("[").appendUint(v.size()).append("]={");
		for (int i = 0; i < v.size(); ++i)
		{
			if (i != 0)
				s.append(",");
			s.appendUint64(v[i].id);
		}
		s.append("}");
		return s;
	}
};


// #22570
class MigrationUserActiveFilter // allowes in some cases migrating users to be registered in some tournaments.
{
	typedef PStringMap< vector<UINT32> > RulesMap;
	RulesMap allowedTypes;
public:
		// format is: filterName=FROM.TO:type[,type...]
		// FROM/TO = {COM|IT|FR|EE|BE|ES|DK|EU}
		// type = RM - real money, PM - play money, DRM - dummy real money, DPM - dummy play money
		// in log file tournament types would be printed as numbers: 1=RM, 33=PM, 65=DRM, 97=DPM
	void readAndPrint(const PIniFile::Section& section, const char* filterName, UINT32 gtfMask /*see MigrationGameInfo*/);
	bool isPassed(UINT32 licenseFrom, UINT32 licenseTo, const vector<MigrationGameInfo>& tourns) const;
	void clear();
};

struct RejectedUser
{
	PString userId;
	INT16 errCode;
	PString errDesc;
	RejectedUser(): errCode( DBM_NO_ERROR ){}
	RejectedUser( const char * userId_, INT16 errCode_, const char* errDesc_ )
		: userId( userId_ ), errCode(errCode_), errDesc( errDesc_ ){}
	void compose(CommMsgBody & body) const
	{
		body.composeString( userId )
			.composeINT16( errCode )
			.composeString( errDesc );
	}
	void parse(CommMsgParser & parser)
	{
		parser.parseStringP( userId )
			.parseINT16( errCode )
			.parseStringP( errDesc );
	}
};

	//#14804 - replace characters that not in safeCharSet with escape code \xHH 
	// default minimalistic char set is enough to represent names, phone numbers and e-mails
const char* toEscapedString(const char* originalStr, PString& safeStr, const char* safeCharSet);
const char* toEscapedString(const char* originalStr, PString& safeStr);

struct BlitzTournUserChips
{
	PString user;
	INT32 deltaChips;

	BlitzTournUserChips( const char * user_ = 0, INT32 deltaChips_ = 0 )
		:user(user_), deltaChips(deltaChips_){}
	void compose( CommMsgBody & body ) const;
	void parse( CommMsgParser &parser );
};

struct BlitzTournChipsUpdateR 
{
	UINT32 reqId;
	UINT32 tournId;
	UINT64 handId;
	UINT64 tableId;
	UINT32 lastOrdinal;
	vector< BlitzTournUserChips > data;
	void add( const char * user, INT32 deltaChips );
	void compose( CommMsgBody &body ) const;
	void parse( CommMsgParser & parser );
	BlitzTournChipsUpdateR(): reqId(0), tournId(0), handId(0), tableId(0), lastOrdinal(0) {};
};

// PYR-24630
#define UNKNOWN_AAMS_SESSION_ID "unknown"

class AamsSessionIdUpdateHelper
{
public:
	
	// returns	true if notification needs to be sent
	//			false if no notification needed
	// isSet is true if the original has been changed as a result
	//			since it can change from 'unknown' to a known value it does not always mirror return value
	//
	static bool checkSetAndNotify( const char* aamsSessionId, PString& originalAamsSessionId, const bool haveUsers, bool& isSet )
	{
		isSet = false;
		if( PString::equals( aamsSessionId, UNKNOWN_AAMS_SESSION_ID ) )
		{
			// know that there's is an aams session id just don't know what it is
			if( haveUsers && originalAamsSessionId.length() == 0 )
			{
				originalAamsSessionId = UNKNOWN_AAMS_SESSION_ID;
			}
			// else - have 0 users. 
			// so either the tournament is over - in which case so we either don't need to preserve aams session id 
			// or no users have registered - in which case we must not aams session id
			//
			return false;
		}
		const bool isOriginalUnknown = originalAamsSessionId.equals( UNKNOWN_AAMS_SESSION_ID );

		if( originalAamsSessionId.length() == 0 || isOriginalUnknown )
		{
			// original is empty or unknown and the new one is actual
			originalAamsSessionId = aamsSessionId;
			
			isSet = true;
			
			return !isOriginalUnknown; // notification needed only if original was empty
		}
		
		if( !originalAamsSessionId.equals( aamsSessionId ) )
		{
			// original was not empty and not 'unknown' but the new one is an actual value and differs from original - something is wrong
			PLog(LOG_TRACE_FAULTY_LOGIC ": trying to overwrite originalAamsSessionId = '%s' with '%s'", originalAamsSessionId.c_str(), aamsSessionId );
			PASSERT5(0);
			return false;
		}
		// original and new are the same
		return false;
	}
};

// PYR-24241
struct BonusStrings
{
	UINT32 bonusId;
	PString bonusName;
	PString bonusDescription;
	PString bonusUrl;

	BonusStrings() : bonusId(0) {}
	BonusStrings(UINT32 id, const char* name, const char* descr, const char* url)
		: bonusId(id), bonusName(name), bonusDescription(descr), bonusUrl(url) {}

	void parse(CommMsgParser& parserParent)
	{
		CommMsgBody body;
		parserParent.parseMsgBody(body);

		CommMsgParser parser(body);
		parser
			.parseUINT32(bonusId)
			.parseStringP(bonusName)
			.parseStringP(bonusDescription)
			.parseStringP(bonusUrl)
			;
	}
	void compose(CommMsgBody& bodyParent) const
	{
		CommMsgBody body;
		body
			.composeUINT32(bonusId)
			.composeString(bonusName)
			.composeString(bonusDescription)
			.composeString(bonusUrl)
			;
		bodyParent.composeMsgBody(body);
	}
};

struct RollStrings
{
	UINT32 id;
	PString shortDescription;
	PString longDescription;

	RollStrings() : id(0) {}
	void parse(CommMsgParser& parser)
	{
		parser
			.parseUINT32(id)
			.parseStringP(shortDescription)
			.parseStringP(longDescription)
			;
	}
};

bool isPowerOf2(UINT64 v);

void jsonEscape(const char* utf8String, PString& out);		// PYR-25781

UINT64 prefixedStr2u64(const char* str);	// PYR-23124 moved from NewsServer.cpp, changed name from "my_atou64"


// Mask... functions were MOVED here from cashiertrans.h after BUILD1015
void maskCardNumber( const char* cardNum, PString& cardMasked, int keep=5  );
void maskNumber(const char* number, PString& maskedNumber, int keep = 4);
void maskNumber(const char* number, PString& maskedNumber, int keepLeft, int keepRight);

int daysInMonth(UINT16 year, BYTE month);

class MessageIPStatistics
{
	PStringMap< UINT32 > counters;

public:
	bool increment( const CommServerConnection* conn, UINT32 level )
	{
		PString ipAddr;
		int port;
		if( ! getServConnIp( *conn, ipAddr, port ) )
			return true;
		if( ! ipAddr.length() )
			return true;

		return increment( ipAddr, level );
	}
	bool increment( const char* ipAddr, UINT32 level )
	{
		if( !ipAddr || !*ipAddr )
			return true;

		PStringMap< UINT32 >::iterator it = counters.insert( make_pair( ipAddr, 0 ) ).first;
		if( (*it).second > level )
		{
			PLog( "++counter exceeded ip='%s'", ipAddr );
			return false;
		}
		(*it).second++;
		return true;
	}
	bool isLimitReached( const CommServerConnection* conn, UINT32 level )
	{
		PString ipAddr;
		int port;
		if( ! getServConnIp( *conn, ipAddr, port ) )
			return false;
		if( ! ipAddr.length() )
			return false;

		return isLimitReached( ipAddr, level );
	}
	bool isLimitReached( const char* ipAddr, UINT32 level )
	{
		if( !ipAddr || !*ipAddr )
			return false;

		PStringMap< UINT32 >::iterator it = counters.find( ipAddr );
		if( it == counters.end() )
			return false;
		
		return it->second >= level;
	}

	void clear()
	{
		counters.clear();
	}
};

class UserRequestStatistics // moved from lobbyobject.h and changed 
{
	PStringMap< UINT32 > data;

public:
	UINT32 limit;
	
	UserRequestStatistics(UINT32 limit_ = 0) : limit(limit_)
	{}

	bool increment( const char* user )
	{
		pair< PStringMap<UINT32>::iterator, bool > ret = data.insert( make_pair( user, 1 ) );
		if( ! ret.second )
		{
			if( (*ret.first).second > limit )
				return false;
			(*ret.first).second++;
		}
		return true;
	}

	bool isLimitReached( const char* user )
	{
		PStringMap<UINT32>::iterator ret = data.find( user );
		if( ret == data.end() )
			return false;
		
		return ret->second >= limit; // limit reached but not exceeded
	}

	void clear()
	{
		data.clear();
	}

	bool decrement( const char* user )
	{
		PStringMap<UINT32>::iterator it = data.find(user);
		if(data.end() == it) return false;
		UINT32& counter = it->second;
		PASSERT5(counter);
		--counter;
		if(!counter)
		{
			data.erase(it);
		}
		return true;
	}
};

// Gets the next integer from the given user string.  These are safer functions than
// using atoi(), but depend on C-locale and are therefore slower.  Use when user-input is untrusted
// and efficiency is not that important.
// The p_strtolAdv() and p_strtoulAdv() versions increment the given character pointer to after the parsed number.
// The return values may be assigned to INT32/UINT32.
// Throws "PError" in case of integer overflow, or when the string does not contain a number.
// Also, p_strtol() and p_strtoul()
INT32 p_strtolAdv(const char* (&userStr));
INT32 p_strtol(const char* userStr);
UINT32 p_strtoulAdv(const char* (&userStr));
UINT32 p_strtoul(const char* userStr);

// PYR-27251
struct UserRefundInfo
{
	PString userId;
	INT32 realMoney;
	INT64 playMoney;
	INT32 fpp;
	INT32 tChips;
	INT32 scalePM;
	PString currency;
	INT16 errorStatus;

	UserRefundInfo() : 
		realMoney(0),
		playMoney(0),
		fpp(0),
		tChips(0),
		scalePM(1),
		errorStatus(DBM_NO_ERROR)
	{}

	UserRefundInfo(const char* user_, INT32 realMoney_, INT64 playMoney_, INT32 fpp_, 
		INT32 tChips_, INT32 scalePM_, const PString& currency_, INT16 errorStatus_) : 
		userId(user_),
		realMoney(realMoney_),
		playMoney(playMoney_),
		fpp(fpp_),
		tChips(tChips_),
		scalePM(scalePM_),
		currency(currency_),
		errorStatus(errorStatus_)
	{}

	void parse(CommMsgParser& parser)
	{
		parser
			.parseStringP(userId)
			.parseINT32(realMoney)
			.parseINT64(playMoney)
			.parseINT32(fpp)
			.parseINT32(tChips)
			.parseINT32(scalePM)
			.parseStringP(currency)
			.parseINT16(errorStatus)
			;
	}

	void compose(CommMsgBody& body) const
	{
		body
			.composeString(userId)
			.composeINT32(realMoney)
			.composeINT64(playMoney)
			.composeINT32(fpp)
			.composeINT32(tChips)
			.composeINT32(scalePM)
			.composeString(currency)
			.composeINT16(errorStatus)
			;
	}
};

//MOVED here from dbmanager.cpp after BUILD1026
const char* getLimitTypeComment(INT32 /*eTableLimitType*/ type, UINT32 licenseId); // PYR-112845
bool isTournLimit(INT32 type);
bool isTableLimit(INT32 type);
bool isBuyInLimitEx(INT32 type);
bool isCasinoLimit(INT32 type);
bool isSportsLimit(INT32 type);
bool isCzLimit(INT32 type);

// PYR-33657
void replaceSubstring(PString& data, const char* toReplace, const char* replacement);

// AmountInCurrency was MOVED here from dbmstruct.h after BUILD1047
struct AmountInCurrency
{
	INT32 amount;
	PString currency;
	AmountInCurrency() : amount(0) {}
	AmountInCurrency(INT32 amount_, const char* currency_) : amount(amount_), currency(currency_) {}

	void parse(CommMsgParser& parser)
	{
		parser
			.parseINT32(amount)
			.parseStringP(currency);
		// DO NOT add new fields. This structure is composed as plain data (not as body) by AmountsInCurrencies which is used in olap msg (like, DBM_Q_OLAP_REPLICATE_PSBO_GAMEOVER)
	}
	void compose( CommMsgBody& body ) const
	{
		body
			.composeINT32(amount)
			.composeString(currency);
		// DO NOT add new fields. This structure is composed as plain data (not as body) by AmountsInCurrencies which is used in olap msg (like, DBM_Q_OLAP_REPLICATE_PSBO_GAMEOVER)
	}
	const char* toString(PString& out) const
	{
		out.assign("").appendInt(amount).append(currency);
		return out;
	}
};

struct AmountsInCurrencies
{
	vector<AmountInCurrency> data;
	void addAmount(INT32 amount, const char* currency);
	INT32 getAmount(const char* currency) const;
	void clear() { data.clear(); }
	UINT32 size() const { return data.size(); }

	void parse(CommMsgParser& parser);
	void compose(CommMsgBody& body) const;
	const char* toString(PString& out) const;
};

// moved from CRMServerObject.h 
class TrackCode 
{
public:
	PString	trackCode;
	UINT32	status;				// 1: active; 0: passive
	UINT32	userTrackCodeDB;	// 1: mainDB; 0: CRMDB
	UINT32	trackCodeFlags;		//	enum TrackCodeFlags (PYR-25971)

	TrackCode( const char* trackCode_, UINT32 status_, UINT32 db_, UINT32 trackCodeFlags_ )
		: trackCode( trackCode_ ), status( status_ ), userTrackCodeDB( db_ ), trackCodeFlags( trackCodeFlags_ /* PYR-25971 */ )
	{
	}
	TrackCode()
		: status( 0 ), userTrackCodeDB( 0 ), trackCodeFlags( 0 )
	{
	}
	TrackCode( CommMsgParser& parser )
	{
		parse( parser );
	}
	CommMsgBody& compose( CommMsgBody& body ) const
	{
		return
			body.composeString(trackCode)
				.composeUINT32(status)
				.composeUINT32(userTrackCodeDB)
				.composeUINT32(trackCodeFlags);
	}
	CommMsgParser& parse( CommMsgParser& parser )
	{
		return	
			parser.parseStringP(trackCode)
				  .parseUINT32(status)
				  .parseUINT32(userTrackCodeDB)
				  .parseUINT32(trackCodeFlags);
	}
	void print() const
	{
		PTRACE4( "trackcode: %s; status: %u; trackCodeFlags: %X db: %u", trackCode.c_str(), status, trackCodeFlags, userTrackCodeDB );
	}
	// the following helpers added for ATF compatability
	CommMsgBody& composeMsg( CommMsgBody& body ) const
	{
		return compose( body );
	}
	CommMsgParser& parseMsg( CommMsgParser& parser )
	{
		return parse( parser );
	}
	void toTraceString(PString& buf) const
	{
		buf.append( "trackcode: " )
			.append( trackCode )
			.append( "; status: " )
			.appendUint( status )
			.append( "; trackCodeFlags: " )
			.appendHexInt( trackCodeFlags )
			.append( " db: " )
			.appendUint( userTrackCodeDB );
	}
	bool equals( const TrackCode& other ) const
	{
		return trackCode.equals( other.trackCode )
			&& status == other.status
			&& userTrackCodeDB == other.userTrackCodeDB
			&& trackCodeFlags == other.trackCodeFlags;
	}
	void clear()
	{
		trackCode.clear();
		status = 0;
		userTrackCodeDB = 0;
		trackCodeFlags = 0;
	}
};

struct UserBrandPair // used in DBM_Q_GET_USERS_REGISTRATION_BRAND
{
	UINT32 userIntID;
	BrandTypes brandId;

	UserBrandPair() : userIntID(0), brandId(BrandType_None) {}
	UserBrandPair(UINT32 userIntID_, BrandTypes brandId_) : userIntID(userIntID_), brandId(brandId_) {}

	CommMsgBody& compose( CommMsgBody& body ) const
	{
		return body
			.composeUINT32(userIntID)
			.composeUINT32(brandId);
	}
	CommMsgParser& parse( CommMsgParser& parser )
	{
		UINT32 brandId_;
		parser
			.parseUINT32(userIntID)
			.parseUINT32(brandId_);
		brandId = static_cast<BrandTypes>(brandId_);
		return parser;
	}
};

void encodeZipPString(const PString& in, PString& out); // PYR-37487

// PYR-74625: When using this method, please ensure the message being decompressed DOESN'T come from the end users, due to known vulnerabilities in zlib.
void decodeUnzipPString(const PString& in, PString& out); // PYR-37487

bool isValidCurrencyCode(const char* currencyCode); // PYR-38845

bool splitEmail(const char* email, PString& handle, PString& domain, bool deAliasHandle = false);  // PYR-51233

// for debug purpose - NEVER to be used for bodies with sensitive data (unless wrapped by ifdef _DEBUG)
const char* msgBodyToString(const CommMsgBody& body, PString& str);
const char* removeSpacesFromString(PString& out, const char* str);
const char* verticalToString1(eGameVerticalFlag vertical);
const char* appendWithDelimiter(PString& target, const char* addition, char delim = ',');

void PDumpEndLine(const PString& str);

#ifndef PROTOCOL_CLIENT_ONLY
// PYR-41301: not applicable to client
class GuardUtils
{
public:
	// This function will get password hash only if the guard is PPWDSSID guard and sid is not used. In any other case, the hash is empty.
	static const char* getPwdHashFromGuard(const char* guardType, const CommServerGuard* srvGuard, PString& pwdHash)
	{
		pwdHash.clear();
		if (PString::equals(guardType, _COMM_PPWDSID_GUARD_NAME))	
		{
			const CommServerPlainTextPasswordSidGuard* guard = dynamic_cast<const CommServerPlainTextPasswordSidGuard*>(srvGuard);
			if (guard != NULL && !guard->isSidUsed())
			{
				guard->getPwdHash(pwdHash);
			}
		}
		else if (PString::equals(guardType, _COMM_PPWD_GUARD_NAME))	
		{
			const CommServerPlainTextPasswordGuard* guard = dynamic_cast<const CommServerPlainTextPasswordGuard*>(srvGuard);
			if (guard != NULL)
			{
				guard->getPwdHash(pwdHash);
			}
		}
		else if (PString::equals(guardType, _COMM_ENCRYPTEDPASSWORDSID_GUARD_NAME))	
		{
			const CommServerEncryptedPwdGuard* guard = dynamic_cast<const CommServerEncryptedPwdGuard*>(srvGuard);
			if (guard != NULL)
			{
				guard->getPwdHash(pwdHash);
			}
		}
		return pwdHash.c_str();
	}
};
#endif

#endif	//shared_h_included
