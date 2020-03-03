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
#include "i18n.strmap.h"
#include "nextrandom.h"
#include "commonmacros.h"
#include "TimeUtils.h" // movet TimeUtils to a separate file
#include "Base26Converter.h"
#include "commgrdppwdsid.h"
#include "commgrdencryptedpwd.h"
#include "misc.h"

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

int StrToCents( const char* str_p );

void splitFullName( PString& firstName, PString& inBetween, PString& lastName, const char* fullName );
const char* splitLastAndSecondName(const char* lastAndSecondName, const char* secondName, PString& onlyLastName); // #21003

bool parseAchAccount(const char* achAcct, PString& routingNum, PString& accountNum, PString& accountType);

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


bool i18n_locale_valid(UINT32 locale);
UINT32 i18n_get_emLocale(UINT32 emLocale, UINT32 locale);

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

// these functions mostly related to field validation for Check or Wire processors
const char* replaceCharWithString(PString& sourceStr, char C, const char* str); // char C in sourceStr will be replaced with str
const char* replaceNonAlphaNumWithString(PString& sourceStr, const char* str); // non alphanum char in sourceStr will be replaced with str
// -end


const char* checkAccountByIBAN(const char* iban, const char* account);
const char* removeLeadingZeros(const char* account);

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


// normalizes input dates such as "March 32, 2011" to a proper date such as "April 1, 2011".
// numDays is by default set to 0, and is a number of days to add to "inputDate"
bool normalizeSrvDate(const SrvDate& inputDate, SrvDate& outputDate, INT32 numDays = 0);

// normalizes input times such as "10:59:185" to a proper "in-range" time such as "11:02:05".
// numSeconds is by default set to 0, and is a number of seconds to add to "inputTime"
bool normalizeSrvTime(const SrvTime& inputTime, SrvTime& outputTime, INT32 numSeconds = 0);

UINT32 checkUserIdRules( const char* userId );// PYR-20357

// PYR-20549
// Merging 2 32bit values into 1 64 value and viceversa
UINT64 get64From32(UINT32 intValue, UINT32 intValue2);
void set32From64(UINT64 val64, UINT32& intValue, UINT32& intValue2);

//PYR-20528 
UINT32 getPlayersLimitForCountry( const PString& restriction, const char * countryCode );

const char* titleEnumToString(eUserTitle eTitle); // not i18n

void processReopenLogFile(CommMsgParser& inParser, CommMsgBody& outBody, UINT32 reqId = 0);

	//#14804 - replace characters that not in safeCharSet with escape code \xHH 
	// default minimalistic char set is enough to represent names, phone numbers and e-mails
const char* toEscapedString(const char* originalStr, PString& safeStr, const char* safeCharSet);
const char* toEscapedString(const char* originalStr, PString& safeStr);

bool isPowerOf2(UINT64 v);

void jsonEscape(const char* utf8String, PString& out);	

UINT64 prefixedStr2u64(const char* str);

// Mask... functions were MOVED here from cashiertrans.h after BUILD1015
void maskCardNumber( const char* cardNum, PString& cardMasked, int keep=5  );
void maskNumber(const char* number, PString& maskedNumber, int keep = 4);
void maskNumber(const char* number, PString& maskedNumber, int keepLeft, int keepRight);

int daysInMonth(UINT16 year, BYTE month);

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

// PYR-33657
void replaceSubstring(PString& data, const char* toReplace, const char* replacement);

bool isValidCurrencyCode(const char* currencyCode); // PYR-38845

bool splitEmail(const char* email, PString& handle, PString& domain, bool deAliasHandle = false);  // PYR-51233

// for debug purpose - NEVER to be used for bodies with sensitive data (unless wrapped by ifdef _DEBUG)
const char* msgBodyToString(const CommMsgBody& body, PString& str);
const char* removeSpacesFromString(PString& out, const char* str);
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
