// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
// Parts of this file are being maintained using automated scripts.
// The script maintenance parts are surrounded by language-specific
// comment lines that begin with <<== and end with ==>> markers.
// When required, manual changes can made to this file as usual. 
// Such changes should be communicated to the current maintainer 
// of GIT module in ADMIN/asppages/admin/_licenses. 
// -=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#include "shared.h"
#include "commssl.h"
#include "commhttp.h"
#include "commcgi.h"
#include "dbm.h"
#include "sites.h"
#include "ppi18n_x.h"
#include "nextrandom.h"
#include "ppstrtok.h"
#include "composecolls.h"

void _appendMoney( PString& buf, INT64 chips64 )
	{
	char tmp[ 64 ];
	char dollarsBuf[ 32 ];
	sprintf( tmp, "%s.%02u", p_i64toa( ( chips64 / 100 ), dollarsBuf ), (UINT32)(chips64 % 100) );
	buf.append( tmp );
	}

INT64 centstoi64( const char* str, const char** ppos )
{
	INT64 cents = 0;
	bool negative = false;
	const char* p = str;
	while( isspace( *p ) )
	{
		++p; // eat any leading space
	}
	if( *p == '-' )
	{
		negative = true;
		++p;
	}
	else if( *p == '+' )
	{
		++p;
	}
	if( ! isdigit( *p ) ) // not a number
	{
		if( ppos != NULL )
		{
			*ppos = str; // don't move a pointer
		}
		return 0;
	}
	while( isdigit( *p ) )
	{
		cents *= 10;
		cents += *p - '0';
		++p;
	}
	cents *= 100; // that was a dollar part
	if( *p == '.' ) // now cents
	{
		++p;
		if( isdigit( *p ) )
		{
			cents += ( *p - '0' ) * 10;
			++p;
			if( isdigit( *p ) )
			{
				cents += *p - '0';
				++p;
			}
			while( isdigit( *p ) )
			{
				++p; // eat any other trailing digits
			}
		}
	}
	if( negative )
	{
		cents = -cents;
	}
	if( ppos != NULL )
	{
		*ppos = p;
	}
	return cents;
}

UINT32 getNextUINT( const char*& str )
	{
	char* retPtr;
	UINT32 result = strtoul( str, &retPtr, 10);
	if( *retPtr )
		retPtr++;
	str = retPtr;
	return result;
	}

INT32 getNextINT( const char*& str )
{
	char* retPtr;
	INT32 result = strtol( str, &retPtr, 10);
	if( *retPtr )
		retPtr++;
	str = retPtr;
	return result;
}

UINT64 getNextUINT64New( const char*& str )
{
	if( str == nullptr )
	{
		return 0;
	}

	UINT64 result = p_atou64(str);
	UINT32 stringlen = strlen(str);
	int i;

	// skip whitespace
	for( i = 0; i < stringlen; ++i )
	{
		if( !isspace((int)(unsigned char)str[i]) )
		{
			break;
		}
	}

	for( ; i < stringlen; ++i )
	{
		if( str[i] < '0' || str[i] > '9' )
		{
			break;
		}
	}

	if( i < stringlen )
	{
		str += i + 1;
	}
	else
	{
		str += stringlen;
	}

	return result;
}

////////////////////////////////////////////////////////////////////////////////
// [AO 2010-04-20] Helper functions to encode/decode vectors and maps of integers.
// No syntax checking is done, only call decode functions on strings
// that were encoded with the corresponding encode functions.

static const char delim = ':';

UINT32 encodeInt32Vector(const vector<INT32>& vec, PString& str)
{
	for (vector<INT32>::const_iterator it = vec.begin(); it != vec.end(); ++it)
		str.appendInt(*it).append(delim);

	return vec.size();
}
UINT32 decodeInt32Vector(const char* str, vector<INT32>& vec)
{
	vec.clear();
	while (*str)
		vec.push_back(getNextINT(str));

	return vec.size();
}

UINT32 encodeUint32Vector(const vector<UINT32>& vec, PString& str)
{
	for (vector<UINT32>::const_iterator it = vec.begin(); it != vec.end(); ++it)
		str.appendUint(*it).append(delim);

	return vec.size();
}

UINT32 decodeUint32Vector(const char* str, vector<UINT32>& vec)
{
	vec.clear();
	while (*str)
		vec.push_back(getNextUINT(str));

	return vec.size();
}

UINT32 encodeUint32Map( const PStringMap<UINT32>& mapIn, PString& strOut )
{
	for( PStringMap<UINT32>::const_iterator it = mapIn.begin(); it != mapIn.end(); ++it )
	{
		strOut.appendUint( strlen( it->first ) ).append( delim )
			  .append( it->first ).append( '=' )
			  .appendUint( it->second ).append( delim );
	}
	return mapIn.size();
}
// PYR-38436
UINT32 encodeInt32Map( const PStringMap<INT32>& mapIn, PString& strOut )
{
	for( auto& it : mapIn )
	{
		strOut.appendUint( strlen( it.first ) ).append( delim )
		.append( it.first ).append( '=' )
		.appendInt( it.second ).append( delim );
	}
	return mapIn.size();
}

UINT32 decodeUint32Map( const char* strIn, PStringMap<UINT32>& mapOut )
{
	mapOut.clear();
	while( *strIn )
	{
		UINT32 len = getNextUINT( strIn );
		const char* pNext = strIn + len;
		PString name( strIn, pNext );
		strIn = pNext + 1;
		mapOut.insert( make_pair( name, getNextUINT( strIn ) ) );
	}

	return mapOut.size();
}
// PYR-38436
INT32 decodeInt32Map( const char* strIn, PStringMap<INT32>& mapOut )
{
	mapOut.clear();
	while( *strIn )
	{
		UINT32 len = getNextUINT( strIn );
		const char* pNext = strIn + len;
		PString name( strIn, pNext );
		strIn = pNext + 1;
		mapOut.insert( make_pair( name, getNextINT( strIn ) ) );
	}

	return mapOut.size();
}


////////////////////////////////////////////////////////////////////////////////


void parseString(CommMsgParser& parser, PString& str)
	{
	const char* buffer;
	parser.parseString( buffer );
	str.assign( buffer );
	}

//does NOT empty vector!!!
UINT32 parseStringVector( CommMsgParser& parser, vector<PString>& strings )
{
	UINT32 sz;
	parser.parseUINT32(sz);
	strings.reserve(sz + strings.size());
	for (int n = 0; n < sz; ++n )
	{
		const char* oneString;
		parser.parseString( oneString );
		strings.push_back( oneString );
	}

	return sz;
}

//does NOT empty vector!!!
UINT32 parseStringVectorWithInitSize( CommMsgParser& parser, vector<PString>& strings, UINT32 initSize )
{
	UINT32 sz;
	parser.parseUINT32(sz);

	// PYR-52366
	strings.reserve(min<UINT32>(sz, initSize) + strings.size());
	for (int n = 0; n < sz; ++n )
	{
		const char* oneString;
		parser.parseString( oneString );
		strings.emplace_back( oneString );
	}

	return sz;
}


UINT32 composePStringSet( CommMsgBody& body, const PStringSet& strings )
{
	const UINT32 sz = strings.size();
	body.composeUINT32(sz);
	for ( PStringSet::const_iterator i=strings.begin(); i!=strings.end(); ++i )
	{
		body.composeString(*i);
	}
	return sz;
}

UINT32 parsePStringSet( CommMsgParser& parser, PStringSet& strings )
{
	UINT32 sz;
	parser.parseUINT32(sz);
	for (int n = 0; n < sz; ++n )
	{
		const char* oneString;
		parser.parseString( oneString );
		strings.insert( oneString );
	}

	return sz;
}

UINT32 composeStringVector( CommMsgBody& body, const vector<PString>& strings )
{
	UINT32 sz = strings.size();
	body.composeUINT32(sz);
	for (int n = 0; n < sz; ++n )
	{
		body.composeString(strings[n]);
	}

	return sz;
}

//does NOT empty vector!!!
UINT32 parseUint32Vector( CommMsgParser& parser, vector<UINT32>& vec )
{
	UINT32 size;
	parser.parseUINT32(size);
	vec.reserve(size + vec.size());
	for (int i = 0; i < size; ++i )
	{
		UINT32 n;
		parser.parseUINT32(n);
		vec.push_back(n);
	}

	return size;
}

//does NOT empty vector!!!
UINT32 parseUint32VectorWithInitSize( CommMsgParser& parser, vector<UINT32>& vec, UINT32 initSize )
{
	UINT32 size;
	parser.parseUINT32(size);

	// PYR-52366
	vec.reserve(min<UINT32>(size, initSize) + vec.size());
	for (int i = 0; i < size; ++i )
	{
		UINT32 n;
		parser.parseUINT32(n);
		vec.push_back(n);
	}

	return size;
}

//does NOT empty vector!!!
UINT16 parseStringVector_SizeUINT16( CommMsgParser& parser, vector<PString>& strings )
{
	UINT16 sz;
	parser.parseUINT16(sz);
	strings.reserve(sz + strings.size());
	for (int n = 0; n < sz; ++n )
	{
		const char* oneString;
		parser.parseString( oneString );
		strings.push_back( oneString );
	}
	return sz;
}

UINT16 composeStringVector_SizeUINT16( CommMsgBody& body, const vector<PString>& strings )
{
	UINT16 sz = static_cast<UINT16>( strings.size() ); //-V1029
	body.composeUINT16(sz);
	for (int n = 0; n < sz; ++n )
	{
		body.composeString(strings[n]);
	}
	return sz;
}

UINT16 parsePStringSet_SizeUINT16( CommMsgParser& parser, PStringSet& vec )
{
	UINT16 sz;
	parser.parseUINT16(sz);
	for (int n = 0; n < sz; ++n)
	{
		const char* right;
		parser.parseString(right);
		vec.insert(right);
	}
	return sz;
}

UINT32 parseUint32Set( CommMsgParser& parser, set<UINT32>& s )
{
	UINT32 size;
	parser.parseUINT32(size);
	for (int i = 0; i < size; ++i )
	{
		UINT32 n;
		parser.parseUINT32(n);
		s.insert(n);
	}
	return size;
}

void composeUint32Set( CommMsgBody& body, const set<UINT32>& s )
{
	body.composeINT32( s.size() );
	for( set<UINT32>::const_iterator it = s.begin(); it != s.end(); ++it )
	{
		body.composeUINT32( *it );
	}
}

// Faster but overwrites original vector
UINT32 parseUint32VectorOverwrite( CommMsgParser& parser, vector<UINT32>& vec )
{
	UINT32 size;
	parser.parseUINT32(size);
	vec.resize(size);
	for (UINT32 i = 0; i < size; ++i)
		parser.parseUINT32(vec[i]);

	return size;
}

UINT32 composeUint32Vector( CommMsgBody& body, const vector<UINT32>& vec )
{
	UINT32 size = vec.size();
	body.composeUINT32(size);
	for (int i = 0; i < size; ++i )
	{
		body.composeUINT32(vec[i]);
	}

	return size;
}
UINT32 parseInt32VectorOverwrite( CommMsgParser& parser, vector<INT32>& vec )
{
	UINT32 size;
	parser.parseUINT32(size);
	vec.resize(size);
	for (UINT32 i = 0; i < size; ++i)
		parser.parseINT32(vec[i]);

	return size;
}

UINT32 composeInt32Vector( CommMsgBody& body, const vector<INT32>& vec )
{
	UINT32 size = vec.size();
	body.composeUINT32(size);
	for (int i = 0; i < size; ++i )
	{
		body.composeINT32(vec[i]);
	}

	return size;
}

UINT32 composeInt16Vector( CommMsgBody& body, const vector<INT16>& vec )
{
	UINT32 size = vec.size();
	body.composeUINT32(size);
	for (int i = 0; i < size; ++i )
	{
		body.composeINT16(vec[i]);
	}

	return size;
}

//does NOT empty vector!!!
UINT32 parseInt16Vector( CommMsgParser& parser, vector<INT16>& vec )
{
	UINT32 size;
	parser.parseUINT32(size);
	vec.reserve(size + vec.size());
	for (int i = 0; i < size; ++i )
	{
		INT16 n;
		parser.parseINT16(n);
		vec.push_back(n);
	}

	return size;
}

// Faster but overwrites original vector
UINT32 parseBoolVectorOverwrite( CommMsgParser& parser, vector<bool>& vec )
{
	UINT32 size;
	parser.parseUINT32(size);
	vec.resize(size);
	for (UINT32 i = 0; i < size; ++i)
	{
		bool b;
		parser.parseBOOL(b);
		vec[i] = b;
	}

	return size;
}

UINT32 composeBoolVector( CommMsgBody& body, const vector<bool>& vec )
{
	UINT32 size = vec.size();
	body.composeUINT32(size);
	for (int i = 0; i < size; ++i )
	{
		body.composeBOOL(vec[i]);
	}

	return size;
}
// Does not empty map!
UINT32 parseUint32PstringMap( CommMsgParser& parser, PStringMap<UINT32>& psMap )
{
	UINT32 size;
	parser.parseUINT32(size);
	for (int i = 0; i < size; ++i )
	{
		const char* key;
		UINT32 n;
		parser.parseString(key).parseUINT32(n);
		psMap.insert(make_pair(key, n));
	}
	return size;
}

UINT32 composeUint32PstringMap( CommMsgBody& body, const PStringMap<UINT32>& psMap )
{
	UINT32 size = psMap.size();
	body.composeUINT32(size);
	for (PStringMap<UINT32>::const_iterator it = psMap.begin(); it != psMap.end(); ++it)
	{
		body.composeString(it->first).composeUINT32(it->second);
	}
	return size;
}

UINT32 parseInt32PstringMap( CommMsgParser& parser, PStringMap<INT32>& psMap )
{
	UINT32 size;
	parser.parseUINT32(size);
	for (int i = 0; i < size; ++i )
	{
		const char* key;
		INT32 n;
		parser.parseString(key).parseINT32(n);
		psMap.insert(make_pair(key, n));
	}
	return size;
}

UINT32 composeInt32PstringMap( CommMsgBody& body, const PStringMap<INT32>& psMap )
{
	UINT32 size = psMap.size();
	body.composeUINT32(size);
	for (PStringMap<INT32>::const_iterator it = psMap.begin(); it != psMap.end(); ++it)
	{
		body.composeString(it->first).composeINT32(it->second);
	}
	return size;
}

void composeIniFile(const PIniFile& ini, CommMsgBody& body)
	{
	body.composeUINT32( ini.sections.size() );
	for( int i=0; i < ini.sections.size() ; ++i )
		{
		const PIniFile::Section& sec = ini.sections[ i ];
		body.composeString( sec.name );

		body.composeUINT32( sec.items.size() );
		for( int j=0; j < sec.items.size(); ++j )
			{
			const PIniFile::Item& item = sec.items[ j ];
			body.composeString( item.name );
			body.composeString( item.value );
			}
		}
	}

void composeIniFileEx(const PIniFileEx& ini, CommMsgBody& body)
{
	body.composeUINT32( ini.sections.size() );
	for( int i=0; i < ini.sections.size() ; ++i )
	{
		const PIniFileEx::Section& sec = ini.sections[ i ];
		body.composeString( sec.name );

		body.composeUINT32( sec.items.size() );
		for( int j=0; j < sec.items.size(); ++j )
		{
			const PIniFileEx::Item& item = sec.items[ j ];
			body.composeString( item.name );
			body.composeString( item.value );
		}
	}
}

void parseIniFile(PIniFile& ini, CommMsgParser& parser)
	{
	ini.sections.clear();
	UINT32 nSec;
	parser.parseUINT32( nSec );
	for( int i = 0; i < nSec; i++ )
		{
		const char* secName;
		parser.parseString( secName );
		PIniFile::Section* sec = &*ini.sections.insert( ini.sections.end(), PIniFile::Section( secName ) );
		UINT32 nItems;
		parser.parseUINT32( nItems );
		for( int j = 0; j < nItems; j++ )
			{
			const char* item, *value;
			parser.parseString( item ).parseString( value );
			sec->items.push_back( PIniFile::Item( item, value ) );
			}
		}
	}

void parseIniFileEx(PIniFileEx& ini, CommMsgParser& parser)
{
	ini.sections.clear();
	UINT32 nSec;
	parser.parseUINT32( nSec );
	for( int i = 0; i < nSec; i++ )
	{
		const char* secName;
		parser.parseString( secName );
		//PIniFileEx::Section* sec = &*ini.sections.insert( ini.sections.end(), PIniFileEx::Section( ini, secName ) );
		PIniFileEx::Section* sec = ini.addSection(secName);
		UINT32 nItems;
		parser.parseUINT32( nItems );
		for( int j = 0; j < nItems; j++ )
		{
			const char* item, *value;
			parser.parseString( item ).parseString( value );
			sec->items.push_back( PIniFileEx::Item( item, value ) );
		}
	}
}


static bool strToDateTime( 
	const char* dateTimeStr, 
	int& year, int& month, int& day, 
	int& hour, int& minute, int& second,
	char dateDelim, // = '-', 
	char timeDelim  //= ':'   
	)
{
	char* dummy;

//Date:
	year = strtol( dateTimeStr, &dummy, 10 );

	dateTimeStr = PString::ltrim( dummy );

	if( *dateTimeStr == dateDelim )
		++dateTimeStr; 
	else return false; 
		 
	month = strtol( dateTimeStr, &dummy, 10 );
	dateTimeStr = PString::ltrim( dummy );
	
	if( *dateTimeStr == dateDelim )
		++dateTimeStr; 
	else return false; 

	day = strtol( dateTimeStr, &dummy, 10 );
	dateTimeStr = PString::ltrim( dummy );

	// scripts use the 3rd date delimiter between date and time (2010-12-31-08:00) 
	if( *dateTimeStr == dateDelim )
		++dateTimeStr; 
	// but it's not required and can be a space

// Time:
	hour = strtol( dateTimeStr, &dummy, 10 );
	dateTimeStr = PString::ltrim( dummy );

	if( *dateTimeStr == timeDelim )
		++dateTimeStr; 
	else  return false; 

	minute = strtol( dateTimeStr, &dummy, 10 );
	dateTimeStr = PString::ltrim( dummy );

	if( *dateTimeStr == timeDelim )
		second = strtol( ++dateTimeStr, &dummy, 10 );
	else // second is optional  
		second = 0; 
	return true;
}

static bool strToDate( 
	const char* dateStr, 
	int& year, int& month, int& day, 
	char dateDelim // = '-' 
	)
{
	char* dummy;

//Date:
	year = strtol( dateStr, &dummy, 10 );

	dateStr = PString::ltrim( dummy );

	if( *dateStr == dateDelim )
		++dateStr; 
	else return false; 
		 
	month = strtol( dateStr, &dummy, 10 );
	dateStr = PString::ltrim( dummy );
	
	if( *dateStr == dateDelim )
		++dateStr; 
	else return false; 

	day = strtol( dateStr, &dummy, 10 );
	return true;
}

bool strToSrvTime( const char* dateTimeStr, SrvTime& srvTime, 
				  char dateDelim, // = '-',
				  char timeDelim  //= ':'  
				  )
{
	int year, month, day, hour, minute, second;
	if( !strToDateTime( dateTimeStr, year, month, day, hour, minute, second, dateDelim, timeDelim))
	{
		PTRACE("!strToSrvTime(%s) %c %c", dateTimeStr, dateDelim, timeDelim);
		return false;
	}
	srvTime._year = year;
	srvTime._month = month;
	srvTime._day = day;
	srvTime._hour = hour;
	srvTime._minute = minute;
	srvTime._second = second;
	return true;
}

bool strToSrvDate( const char* dateStr, SrvDate& srvDate, char dateDelim /* = '-'*/  )
{
	int year, month, day;
	if( !strToDate( dateStr, year, month, day, dateDelim))
	{
		return false;
	}
	srvDate._year = year;
	srvDate._month = month;
	srvDate._day = day;
	return true;
}

bool xmlDateTimeToSrvTimeLocal( const char* const xmlDateTime, SrvTime& srvTimeLocal, SrvTime* srvTimeUtc /*= NULL*/ )
{
    // YYYY-MM-DDThh:mm:ss[.fff]('Z'|(('+'|'-')hh:mm)
    // e.g. 2000-01-01T00:00:00.123-05:00
    struct tm tempTm = {0};
    char zone[32];
    zone[sizeof(zone) - 1] = '\0';

    // make sure that we parse all 7 fields from the string
    if( sscanf( xmlDateTime, "%d-%2d-%2dT%2d:%2d:%2d%31s",
                &tempTm.tm_year, &tempTm.tm_mon, &tempTm.tm_mday,
                &tempTm.tm_hour, &tempTm.tm_min, &tempTm.tm_sec, zone ) < 7 )
        return false;

    tempTm.tm_year -= 1900;
    tempTm.tm_mon--;
    tempTm.tm_isdst = -1; // use system local time settings

    const char* timeZone = zone;
    if( *timeZone == '.' ) // skip fractional seconds
        while( isdigit( *++timeZone ) ); //lint !e722

    if( !*timeZone ) // no timezone - invalid format of dateTime
        return false;

    if( *timeZone == '+' || *timeZone == '-' )
    {
        int hours = 0;
        int minutes = 0;
        sscanf( timeZone, "%3d:%2d", &hours, &minutes );
        if( hours < 0 )
        {
            minutes = -minutes;
        }
        tempTm.tm_min -= minutes;
        tempTm.tm_hour -= hours;
    }
    else if( *timeZone != 'Z' )
    {
        return false;
    }
    // fix potentially out-of-range members of tm struct
    if( mktime( &tempTm ) == time_t(-1) )
        return false;

    if( srvTimeUtc )
        srvTimeUtc->assign( tempTm );

    time_t tempTime = plib_mkUTCtime( &tempTm );
    srvTimeLocal.decode( tempTime );

    return true;
}

bool convertStringFromHex( const char* strValue, PBlock& ret )
{
    const UINT32 size = strlen(strValue);
    ret.ensure( (size + 1)/2 );
    UINT32 flag = 0; // shift inside the byte for the current hex digit (can be 0 or 4)
    BYTE* pval = ret.ptr() + ret.size() - 1; //lint !e864 ... depends on order of evaluation
    for( const char* str = strValue + size - 1; str >= strValue; --str )
    {
        if( flag == 0 )
            *pval = 0;

        if( *str >= '0' && *str <= '9' )
            *pval += (*str - '0') << flag;
        else if( *str >='a' && *str <= 'f' )
            *pval += (*str - 'a' + 0xA) << flag;
        else if( *str >= 'A' && *str <= 'F' )
            *pval += (*str - 'A' + 0xA) << flag;
        else
        {
            ret.clear();
            return false;
        }
        if( flag == 0 )
        {
            flag = 4;
        }
        else
        {
            flag = 0;
            --pval;
        }
    }
    return true;
}

const char* convertHexToString( const BYTE* binArray, UINT32 binSize, PString& val )
{ // might have a leading zero after conversion
    val.cut(0);
    for( const BYTE* p = binArray; p < binArray + binSize; ++p )
    {
        char buf[2]; // hex representation of one byte
        buf[0] = (*p >> 4) + '0';
        if( buf[0] > '9' )
            buf[0] += 'a' - '9' - 1;

        buf[1] = (*p & 0xF) + '0';
        if( buf[1] > '9' )
            buf[1] += 'a' - '9' - 1;

        val.append( buf, sizeof(buf) );
    }
    return val;
}

void convertStringToMacAddress(const PString& src, UINT64& dst) // hex string with delimiters to UINT64
{
	dst = 0;
	const char* ptr = src.c_str() + src.length() - 1;
	for(int shift = 0; shift < 64 && ptr >= src.c_str(); shift += 4, --ptr)
	{
		if(*ptr >= '0' && *ptr <= '9')
			dst += static_cast<UINT64>(*ptr - '0') << shift;
		else
			if(*ptr >= 'A' && *ptr <= 'F')
				dst += static_cast<UINT64>(*ptr - 'A' + 0xA) << shift;
			else
				if(*ptr >='a' && *ptr <= 'f')
					dst += static_cast<UINT64>(*ptr - 'a' + 0xA) << shift;
				else
					shift -= 4; // non-hexadecimal character ignored by negating increment
	}
}

void convertMacAddressToString(UINT64 src, PString& dst) // UINT64 to hex string with delimiters
{
	dst.cut(0);
	for(int shift = 0; shift < 64; shift += 4)
	{
		char c = (src >> shift) & 0xF + '0';
		if(c > '9')
			c += 'A' - '9' - 1;
		dst.append(c);
		if (shift & 0x4)
			dst.append(':');
	}
}

bool olderThanFullYears(const SrvDate& birthDate, const SrvTime& srvNow, UINT32 fullYears)
{
	SrvDate srvTodayLessYears( srvNow );
	srvTodayLessYears._year -= fullYears;
	int res = birthDate.compareTo(srvTodayLessYears);
	return 0 >= res;
}

int calcTournamentPoints( int place, int placeTo, int numPlayers, int buyIn, int fppBuyIn, 
						  BYTE numFormula, UINT32 /*tournFlags*/, double& nonRoundedPoints /* PYR-18026 */ )
{	
	nonRoundedPoints = 0.0; // PYR-18026

	const double FPP_DOLLAR_VALUE = 0.0161; //#10385
	PASSERT( numFormula <= 2 );
	if( !numFormula ) //just clear points
		return 0;

	double bInFactor = 1;
	double placePoints = ( place + placeTo ) / 2.;
	double EntrantFactor = numPlayers;

	if( !buyIn )
		{
		if( fppBuyIn )
			{
			if( numFormula == TLB_FORMULA_ORIG )
				bInFactor = 0.2 + FPP_DOLLAR_VALUE * fppBuyIn;
			else if( numFormula == TLB_FORMULA_JAN03 ) //-V547
				bInFactor = FPP_DOLLAR_VALUE * fppBuyIn;
			}
		else
			{
			if( numFormula == TLB_FORMULA_ORIG ) 
				bInFactor = 0.2;
			else if( numFormula == TLB_FORMULA_JAN03 ) //-V547
				bInFactor = .0;
			}
		}
	else
		bInFactor = buyIn / 100.;

	double points = 0;

	if( numFormula == TLB_FORMULA_ORIG )
		{ //'Old' formula:
		bInFactor = log10( bInFactor ) + 1;
		double m = 0.15 * EntrantFactor;
		placePoints = (m+1-placePoints);
		if( placePoints < 0 || bInFactor < 0 )
			return 0;

		points = 100. * (50. * sqrt(EntrantFactor) * bInFactor * placePoints/((m+1)*(m/2)));
		}
	else if( numFormula == TLB_FORMULA_JAN03 ) //-V547
		{ //'New' formula:
		/*
		1) points=10*(sqrt(N)/sqrt(K))*(1+log(b+0.25))
		2) only 15% get points rounded up (to include more players)
		3) b=0 for freerolls;
		4) b=FPP_DOLLAR_VALUE*FPP for FPP buy-ins
		5) points for a player for a tournament rounded to 2 places after the dot
		and the rest is discarded
		6) tournament date is the start date
		7) eligibility defined in the tournament script; default TLB=No for Sit & Go
		tourneys and TLB=Yes for all other.
		*/
		bInFactor = log10( bInFactor + 0.25 ) + 1;
		if( !placePoints )                   //@@@ OldAuthor V550 An odd precise comparison. It's probably better to use a comparison with defined precision: fabs(placePoints) < Epsilon. dumbbot shared.cpp 969
			return 0; // protection of DIVISION_BY_ZERO
		else
			{
			double pointsSum = 0.;
			int nShared;
			int i;
			for( nShared = 0, i = place; i <= placeTo ; ++i, ++nShared )
				{
				double pp = 0.;
				if(	!( i > ceil( 0.15 * numPlayers ) ) )
					pp = 100. * 10. * (sqrt(EntrantFactor)/sqrt((double)i)) * bInFactor;
				pointsSum += pp;
				}
			if(nShared == 0)
				nShared = 1; //sanity
			points = pointsSum / nShared;
			}
		}

	nonRoundedPoints = points;  // PYR-18026

	int roundedPoints = (int)points; //-V2003

	if( ( points - roundedPoints ) >= 0.5 ) 
		roundedPoints++;

	if( roundedPoints > 0 )
		return roundedPoints;
	else 
		return 0;
}

////////////////////////////////////////////////////////////////////////////////

struct CountryPair
{
	const char* name;
	const char* code;
};

struct _CountryStateCodes
{
	const char* stateName;
	const char* countryCode;
	const char* stateCode;
};

int StrToCents( const char* str_p )
{
	char* dummy;
	int dollars_, decimalCents_;
	if ( ( dollars_ = strtoul( str_p, &dummy, 10 ) ) < 0 )
		return -1;

	const char* strBuff = PString::ltrim( dummy );
	if( *strBuff  == '.' )
		{ 
		if ( ( decimalCents_ = strtoul( strBuff + 1, &dummy, 10 ) ) < 0 )
			return -1;
		
		if( decimalCents_ > 99 )
			return -1;
		}
	else
		decimalCents_ = 0;

	return ( 100 * dollars_ + decimalCents_ );
}

void readDbmSyncScriptFromFile(CommMsgBody & syncMsg, const PIniFile & scriptFile)
{
	enum 
	{
		UpdateOrInsert = 0,
		AppendOnly,
		ClearAndAppend
	};


	const PIniFile::Section* section = scriptFile.getSection("OLAPS");
	PASSERT(section);

	int numOlaps = section->getIntProperty("NumberfOlapsToSync",0);

	if(numOlaps == 0)
	{
		throw PError("Number of OLAP DB's is Zero ! Aborting sync OLAP");
	}

	syncMsg
		.composeUINT32(numOlaps);

	for(int olapCounter = 1; olapCounter <= numOlaps;olapCounter++)
	{
		PString olapItemName = "OlapName";
		olapItemName.appendInt(olapCounter);
		PString olapName = section->getProperty(olapItemName.c_str());
		syncMsg
			.composeString(olapName);
	}

	section = scriptFile.getSection("TABLES");
	PASSERT(section);

	int numTables = section->getIntProperty("NumberOfTablesToSync",0);
	if(numTables == 0)
	{
		throw PError("Number of Tables to sync is Zero ! Aborting sync OLAP");
	}

	syncMsg
		.composeUINT32(numTables);

	for(int tablesCounter = 1;tablesCounter <= numTables; tablesCounter++)
	{
		section = scriptFile.getSection("TABLES");
		if(section == NULL)
		{
			throw PError("Couldn't find section TABLES in INI file");
		}
		
		PString tableItemName = "Table";
		tableItemName.appendInt(tablesCounter);
		PString tableName = section->getProperty(tableItemName.c_str());
		if(tableName.length() == 0)
		{
			PString errDscp = "Couldn't find table name for entry ";
			errDscp += tableItemName;
			errDscp += "in TABLES section";
			throw PError(errDscp);
		}
		syncMsg
			.composeString(tableName);

		section = scriptFile.getSection(tableName.c_str());
		if(section == NULL)
		{
			PString errDscp = "Couldn't find section for table name ";
			errDscp += tableName;
			throw PError(errDscp);
		}

		BYTE excutionMethod = UpdateOrInsert;

		PString execMethodStr = section->getProperty("ExecutionMethod");
		if(PString::compareIgnoreCase("AppendOnly",execMethodStr,execMethodStr.length()) == 0)
		{
			excutionMethod = AppendOnly;
		}
		else if(PString::compareIgnoreCase("ClearAndAppend",execMethodStr,execMethodStr.length()) == 0)
		{
			excutionMethod = ClearAndAppend;
		}

		bool isFullTableSync = section->getIntProperty("IsFullTableSync",0) > 0 ? true : false;

		if(isFullTableSync)
		{
			excutionMethod = ClearAndAppend;
		}

		int primaryKeysNumber = section->getIntProperty("PrimaryKeysNumber",0);
		syncMsg
			.composeBYTE(excutionMethod)
			.composeBOOL(isFullTableSync)
			.composeUINT32(primaryKeysNumber);

		for(int primKeyCounter = 1;primKeyCounter <= primaryKeysNumber; primKeyCounter++)
		{
			PString primaryKeyNameItem = "PrimaryKeyName";
			primaryKeyNameItem.appendInt(primKeyCounter);
			PString primaryKeyName = section->getProperty(primaryKeyNameItem.c_str());

			syncMsg
				.composeString(primaryKeyName);
		}

		PString selectKeyName = section->getProperty("SelectKeyName");

		syncMsg
			.composeString(selectKeyName);

		PString selectKeyType = section->getProperty("SelectKeyType");

		if(PString::compareIgnoreCase("INT32",selectKeyType,selectKeyType.length()) == 0)
		{
			int minRange = section->getIntProperty("SelectMinRange",0);
			int maxRange = section->getIntProperty("SelectMaxRange",100000);
			syncMsg
				.composeINT16(4) //SQL_INTEGER
				.composeBOOL(false)
				.composeINT32(minRange)
				.composeINT16(4) //SQL_INTEGER
				.composeBOOL(false)
				.composeINT32(maxRange);
		}
		else if(PString::compareIgnoreCase("INT16",selectKeyType,selectKeyType.length()) == 0)
		{
			int minRange = section->getIntProperty("SelectMinRange",0);
			int maxRange = section->getIntProperty("SelectMaxRange",100000);
			syncMsg
				.composeINT16(5) //SQL_SMALLINT
				.composeBOOL(false)
				.composeINT32(minRange)
				.composeINT16(5) //SQL_SMALLINT
				.composeBOOL(false)
				.composeINT32(maxRange);
		}
		else if(PString::compareIgnoreCase("INT64",selectKeyType,selectKeyType.length()) == 0)
		{
			//Need to change property to retrieve 64 bits
			INT64 minRange = p_atoi64( section->getProperty("SelectMinRange") );
			INT64 maxRange = p_atoi64( section->getProperty("SelectMaxRange") );
			syncMsg
				.composeINT16(-25) //SQL_C_SBIGINT
				.composeBOOL(false)
				.composeINT64(minRange)
				.composeINT16(-25) //SQL_C_SBIGINT
				.composeBOOL(false)
				.composeINT64(maxRange);
		}
		else if(PString::compareIgnoreCase("STRING",selectKeyType,selectKeyType.length()) == 0)
		{
			PString from = section->getProperty("SelectMinRange");
			PString to = section->getProperty("SelectMaxRange");
			syncMsg
				.composeINT16(12) //SQL_VARCHAR
				.composeBOOL(false)
				.composeString(from)
				.composeINT16(12) //SQL_VARCHAR
				.composeBOOL(false)
				.composeString(to);
		}
		else if(PString::compareIgnoreCase("TIMESTAMP",selectKeyType,selectKeyType.length()) == 0)
		{
			SrvTime fromTime;
			SrvTime toTime;
			SrvTime period;

			if(!strToSrvTime(section->getProperty("SelectMinRange"),fromTime))
			{
				throw PError("Min Range Timestamp format is wrong");
			}
			if(!strToSrvTime(section->getProperty("SelectMaxRange"),toTime))
			{
				throw PError("Max Range Timestamp format is wrong");
			}
			if(!strToSrvTime(section->getProperty("SelectPeriod"),period))
			{
				throw PError("Period Timestamp format is wrong");
			}

			syncMsg
				.composeINT16(93) //SQL_TYPE_TIMESTAMP
				.composeBOOL(false);
			fromTime.compose(syncMsg);
			syncMsg
				.composeINT16(93) //SQL_TYPE_TIMESTAMP
				.composeBOOL(false);
			toTime.compose(syncMsg);
			syncMsg
				.composeINT16(93) //SQL_TYPE_TIMESTAMP
				.composeBOOL(false);
			period.compose(syncMsg);
		}
		else if(PString::compareIgnoreCase("DATE",selectKeyType,selectKeyType.length()) == 0)
		{

			SrvDate fromDate;
			SrvDate toDate;
			SrvDate period;

			if(!strToSrvDate(section->getProperty("SelectMinRange"),fromDate))
			{
				throw PError("Min Range Date format is wrong");
			}
			if(!strToSrvDate(section->getProperty("SelectMaxRange"),toDate))
			{
				throw PError("Max Range Date format is wrong");
			}
			if(!strToSrvDate(section->getProperty("SelectPeriod"),period))
			{
				throw PError("Period Date format is wrong");
			}

			syncMsg
				.composeINT16(91) //SQL_TYPE_DATE
				.composeBOOL(false);
			fromDate.compose(syncMsg);
			syncMsg
				.composeINT16(91) //SQL_TYPE_DATE
				.composeBOOL(false);
			toDate.compose(syncMsg);
			syncMsg
				.composeINT16(91) //SQL_TYPE_DATE
				.composeBOOL(false);
			period.compose(syncMsg);
		}
	}
}

const char* strVectorToStr(PString& str, const vector<PString>& strVec)
{
	return strVectorToStr(str, strVec, ',');
}

const char* strVectorToStr(PString& str, const vector<PString>& strVec, char delimeter)
{
	str.assign("");
	for(int n = 0; n < strVec.size(); n++)
	{
		if( 0 != n )
		{
			str.append(delimeter);
		}
		str.append(strVec[n]);
	}
	return str.c_str();
}

const char* strSetToStr(PString& str, const PStringSet& strSet)
{
	return strSetToStr(str, strSet, ',');
}

const char* strSetToStr(PString& str, const PStringSet& strSet, char delimeter)
{
	str.assign("");
	for(PStringSet::const_iterator it = strSet.begin(); it != strSet.end(); ++it)
	{
		if(it != strSet.begin())
		{
			str.append(delimeter);
		}
		str.append(*it);
	}
	return str.c_str();
}

template <typename PStringCmpT>
static const char* stlStrSetToStrT(PString& str, const set<PString, PStringCmpT>& strSet, char delimeter)
{
	str.assign("");
	for(auto it = strSet.begin(); it != strSet.end(); ++it)
	{
		if(it != strSet.begin())
		{
			str.append(delimeter);
		}
		str.append(*it);
	}
	return str.c_str();
}
// the same as above but works with stl string set
const char* stlStrSetToStr(PString& str, const set<PString, PStringCmp>& strSet)
{ // PYR-51386 converted to template implementation
	return stlStrSetToStrT(str, strSet, ',');
}

const char* stlStrSetToStrIgnoreCase(PString& str, const set<PString, stringLessIgnoreCase>& strSet)
{ // PYR-51386
	return stlStrSetToStrT(str, strSet, ',');
}

const char* stlStrSetToStr(PString& str, const set<PString, PStringCmp>& strSet, char delimeter)
{ // PYR-51386 converted to template implementation
	return stlStrSetToStrT(str, strSet, delimeter);
}

const char* stlStrSetToStrIgnoreCase(PString& str, const set<PString, stringLessIgnoreCase>& strSet, char delimeter)
{ // PYR-51386
	return stlStrSetToStrT(str, strSet, delimeter);
}

const char* uintSetToStr(PString& str, const set<UINT32>& uintSet)
{
	return uintSetToStr(str, uintSet, ',');
}

const char* uintSetToStr(PString& str, const set<UINT32>& uintSet, char delimeter)
{
	str.assign("");
	for(set<UINT32>::const_iterator it = uintSet.begin(); it != uintSet.end(); ++it)
	{
		if(it != uintSet.begin())
		{
			str.append(delimeter);
		}
		str.appendUint(*it);
	}
	return str.c_str();
}

const char* multimapStrStrToString(PString& str, const multimap<PString, PString, PStringCmp>& mm)
{
	str.assign("");
	str.append("(").appendUint(mm.size()).append(") ");
	for (const auto& pair: mm)
	{
		str.append(pair.first).append(":").append(pair.second).append(",");
	}
	return str;
}

bool removeStringFromStrVector(const char* str, vector<PString>& strVec)
{
	for(int n = 0; n < strVec.size(); n++)
	{
		if(strVec[n].equals(str))
		{
			strVec.erase(strVec.begin() + n);
			return true; //string found and deleted
		}
	}
	return false;//string was not found
}

///reads comma-separated string and puts it's contents into vector
void readStrVectorFromString(const char* str, vector<PString>& strVec)
{
	readStrVectorFromString(str, strVec, ',');
}

///reads delimeter-separated string and puts it's contents into vector
void readStrVectorFromString(const char* str, vector<PString>& strVec, char delimeter)
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
			strVec.push_back(str_tmp);
		p = commaPtr + 1;
		if(*p)
			commaPtr = strchr(p, delimeter);
		else
			break;
	}
	if(*p)
		strVec.push_back(p);
}

///reads comma-separated string and puts it's contents into stringset
void readStrSetFromString(const char* str, PStringSet& strSet)
{
	if(!str || !*str)
		return;
	const char* p = str;
	PString str_tmp;
	const char* commaPtr = strchr(str, ',');
	while(commaPtr)
	{
		str_tmp.assign(p, commaPtr);
		if(*str_tmp)
			strSet.insert(str_tmp);
		p = commaPtr + 1;
		if(*p)
			commaPtr = strchr(p, ',');
		else
			break;
	}
	if(*p)
		strSet.insert(p);
}

template <typename PStringCmpT>
static void readStlStrSetFromStringT(const char* str, set<PString, PStringCmpT>& strSet)
{
	if(!str || !*str)
		return;
	const char* p = str;
	PString str_tmp;
	const char* commaPtr = strchr(str, ',');
	while(commaPtr)
	{
		str_tmp.assign(p, commaPtr);
		if(*str_tmp)
			strSet.insert(str_tmp);
		p = commaPtr + 1;
		if(*p)
			commaPtr = strchr(p, ',');
		else
			break;
	}
	if(*p)
		strSet.insert(p);
}

void readStlStrSetFromString(const char* str, set<PString, PStringCmp>& strSet) 
{ // PYR-51386 converted to template implementation
	readStlStrSetFromStringT(str, strSet);
}
void readStlStrSetFromStringIgnoreCase(const char* str, set<PString, stringLessIgnoreCase>& strSet)
{ // PYR-51386
	readStlStrSetFromStringT(str, strSet);
}

void readPStringSetFromString(const char* str, PStringSet& strSet, char delimeter, bool toUpper, bool trim)
{
	vector<PString> strVec;
	readStrVectorFromString(str, strVec, delimeter, false/*keepEmpty*/, trim);

	if(strVec.size() == 0)
		return;

	for (vector<PString>::iterator it = strVec.begin(); it != strVec.end(); ++it)
	{
		if (toUpper)
		{
			it->toUpper();
		}
		strSet.insert(*it);
	}
}

void readStrVectorFromString(const char* str, vector<PString>& strVec, char delimeter, bool keepEmpty, bool trim)
{
	if(!str || (!*str && !keepEmpty))
		return;
	const char* p = str;
	PString str_tmp;
	const char* commaPtr = strchr(str, delimeter);

	while(commaPtr)
	{
		str_tmp.assign(p, commaPtr);

		if( trim )
		{
			PString trimmed(PString::ltrim(str_tmp), PString::rtrim(str_tmp));
			if(*trimmed || keepEmpty)
				strVec.push_back(trimmed);
		}
		else if(*str_tmp || keepEmpty)
			strVec.push_back(str_tmp);

		p = commaPtr + 1;
		if(*p)
			commaPtr = strchr(p, delimeter);
		else
			break;
	}

	if(*p || keepEmpty)
	{
		if( trim )
		{
			PString trimmed(PString::ltrim(p), PString::rtrim(p));
			if(*trimmed || keepEmpty)
				strVec.push_back(trimmed);
		}
		else
			strVec.push_back(p);
	}

}


//static 
unsigned char CompareStringUtils::_toUpper( unsigned char c )
{
	if( c >= 'a' && c <= 'z' )
		return c + 'A' - 'a';
	else
		return c;
}

//static 
bool CompareStringUtils::_endsWithIgnoreCase( const char* s, const char* s2 )
{
	int n = strlen( s2 );

	int sLen = strlen( s );
	if( sLen < n )
			return false;

	const char* p2 = s2;
	for( const char* p = s + sLen - n; p < s + sLen ; ++p, ++p2 )
		if( _toUpper( *p ) != _toUpper( *p2 ) )
			return false;
	return true;
}

//static 
bool CompareStringUtils::_startWithIgnoreCase( const char* s, const char* s2)
{
	int n = strlen(s2);
	int sLen = strlen( s );
	if( sLen < n )
			return false;

	const char* p2 = s2;
	for( const char* p = s; p2 < s2 + n ; ++p, ++p2 )
		if( _toUpper( *p ) != _toUpper( *p2 ) )
			return false;
	return true;
}

// does NOT properly split UTF-8; TODO - move it to gatewayhelper.cpp and review its usage in cashier
void splitFullName( PString& firstName, PString& inBetween, PString& lastName, const char* fullName )
{    
    //Replace comma with space inside name string
    PString _fullName;
    for(const char*p = fullName; *p != '\0'; ++p)
        if(*p == ',')
            _fullName.append(' ');
        else
            _fullName.append(*p);

    static const char* const _prefixes[] = { "Mr ", "Ms ", "Mrs ", "Miss ", 0 };
	static const char* const _suffixes[] = { " SR", " JR", " SR.", " JR.", " I", " II", " III", " IV", 0 };
	PString trimmed( _fullName.ltrim(), _fullName.rtrim() );
    UINT32 i;
	for( i=0; _prefixes[ i ] != 0 ; ++i )
	{
		const char* prefix = _prefixes[ i ];
		if( CompareStringUtils::_startWithIgnoreCase( trimmed, prefix ) )
		{
			trimmed.assign( trimmed.c_str() + strlen(prefix) );
			break;
		}
	}


	for( i=0; _suffixes[ i ] != 0 ; ++i )
	{
		const char* suffix = _suffixes[ i ];
		if( CompareStringUtils::_endsWithIgnoreCase( trimmed, suffix ) )
		{
			trimmed.cut( trimmed.length() - strlen( suffix ) );
			break;//for( i )
		}
	}

    PString trimname(trimmed.c_str(), trimmed.rtrim());
    const char* spc = strrchr( trimname, ' ' );
	if( spc )
    {
        PString tmp( trimname, spc );
        const char* spc0 = strchr( tmp, ' ' );
        if( spc0 )
        {
	        firstName.assign( tmp, spc0 );
	        inBetween = spc0 + 1;
	        lastName = spc + 1;
        }
        else
        {
	        firstName = tmp;
	        inBetween = "";
	        lastName = spc + 1;
        }
    }
    else
    {//not 100% clear
        firstName = trimname;
        inBetween = "";
        lastName = "";
    }

	vector<PString> middles;
	strTokenize(inBetween.c_str(), middles, " ");
	UINT32 total = middles.size();
	
	if(firstName.length() == 1 && total)
	{		
		firstName.append(' ').append(middles[0]);
		inBetween="";
		for(i=1; i<total; ++i)
		{
			if(i>1)
				inBetween.append(' ');
			inBetween.append(middles[i]);
		}
		middles.erase(middles.begin());
	}
	
	total = middles.size();
	if(lastName.length() == 1 && total)
	{
		PString temp;
		temp.assign(middles[total-1]).append(' ').append(lastName);
		inBetween="";
		lastName.assign(temp);
		for(i=0; i<total-1; ++i)
		{
			if(i>0)
				inBetween.append(' ');
			inBetween.append(middles[i]);
		}

	}
}

const char* splitLastAndSecondName(const char* lastAndSecondName, const char* secondName, PString& onlyLastName) // #21003
{
	onlyLastName = lastAndSecondName;
	if (secondName && *secondName)
	{
		if (PStringX::isUtf8(secondName))
		{
			secondName += 1;
		}
		if (onlyLastName.endsWith(secondName))
		{
			onlyLastName.cut(onlyLastName.length() - PString::length(secondName));
			onlyLastName.cut(onlyLastName.rtrim() - onlyLastName.c_str());
		}
		else
		{
			PLog("!splitLastAndSecondName(%s,%s)", lastAndSecondName, secondName);
		}
	}
	return onlyLastName.c_str();
}

bool parseAchAccount( const char* achAcct, PString& routingNum, PString& accountNum, PString& accountType )
{
	routingNum = accountNum = accountType = "";

	if( ! achAcct )
		return false;
	if( ! *achAcct )
		return false;

	int i;
	for( i = 0; achAcct[i] && achAcct[i] != '@'; i++ )
		routingNum.append( achAcct[i] );

	if( ! achAcct[i] )
		return false;

	for( i++; achAcct[i] && achAcct[i] != '-'; i++ )
		accountNum.append( achAcct[i] );

	if( ! achAcct[i] )
		return false;

	if( achAcct[i] == '-' )
		i++;
	if( ! achAcct[i] )
		return false;

	accountType.append( achAcct[i] );

	return true;
}

//////////////////////////////////////////////////////////////////////////
// ----- WireAccountInfo Implementation----------------------------------
//////////////////////////////////////////////////////////////////////////

void WireAccountInfo::HolderInfo::compose(CommMsgBody& body) const
{
	body
		.composeString(fullName)
		.composeString(phone)
		.composeString(address)
		.composeString(city)
		.composeString(state)
		.composeString(zip)
		.composeString(countryCode)
		;
}

void WireAccountInfo::HolderInfo::parse(CommMsgParser& parser)
{
	parser
		.parseStringP(fullName)
		.parseStringP(phone)
		.parseStringP(address)
		.parseStringP(city)
		.parseStringP(state)
		.parseStringP(zip)
		.parseStringP(countryCode)
		;
}

void WireAccountInfo::BankInfo::compose(CommMsgBody& body) const
{
	body
		.composeString(userAccount)
		.composeString(currencyCode)
		.composeString(iban)
		.composeString(swiftCode)
		.composeString(bankName)
		.composeString(bankAddr)
		.composeString(comments)
		;
}

void WireAccountInfo::BankInfo::parse(CommMsgParser& parser)
{
	parser
		.parseStringP(userAccount)
		.parseStringP(currencyCode)
		.parseStringP(iban)
		.parseStringP(swiftCode)
		.parseStringP(bankName)
		.parseStringP(bankAddr)
		.parseStringP(comments)
		;
}

void WireAccountInfo::compose( CommMsgBody& body ) const
{
	body
		.composeString(userId)
		.composeSrvTime(when)
		.composeUINT32(cc_id);

	CommMsgBody holderBody;
	holder.compose(holderBody);
	body.composeMsgBody(holderBody);

	CommMsgBody bankBody;
	bank.compose(bankBody);
	body.composeMsgBody(bankBody);

	body.composeString(maskedCardNum);
	body.composeString(userAccountPlain);
    body.composeINT16(bankValidationCode);
}

void WireAccountInfo::parse( CommMsgParser& parser )
{
	parser
		.parseStringP(userId)
		.parseSrvTime(when)
		.parseUINT32(cc_id);

	CommMsgBody holderBody;
	parser.parseMsgBody(holderBody);
	CommMsgParser holderParser(holderBody);
	holder.parse(holderParser);

	CommMsgBody bankBody;
	parser.parseMsgBody(bankBody);
	CommMsgParser bankParser(bankBody);
	bank.parse(bankParser);

	parser.parseStringP(maskedCardNum);
	parser.parseStringP(userAccountPlain);
    parser.parseINT16(bankValidationCode); // #18100
    // Note: this struct also used in OLAP. use parseEnded when new parameter introduced.
}

void Vendor::composeData(CommMsgBody& body) const
{
	body
		.composeUINT32(vendorId)
		.composeString(name)
		.composeString(descr)
		.composeUINT32(adminIntId)
		;
}

void Vendor::parseData(CommMsgParser& parser)
{
	const char* name_;
	const char* descr_;
	parser
		.parseUINT32(vendorId)
		.parseString(name_)
		.parseString(descr_)
		;
	if(!parser.parseEnded())
		parser.parseUINT32(adminIntId);
	name = name_;
	descr = descr_;
}

void Product::composeData(CommMsgBody& body) const
{
	body
		.composeUINT32(productId)
		.composeUINT32(vendorId)
		.composeString(descr)
		.composeUINT32(price)
		.composeString(currency)
		.composeString(allowedCountries)
		.composeUINT32(adminIntId)
		;
}

void Product::parseData(CommMsgParser& parser)
{
	const char* descr_;
	const char* currency_;
	const char* allowedCountries_;
	parser
		.parseUINT32(productId)
		.parseUINT32(vendorId)
		.parseString(descr_)
		.parseUINT32(price)
		.parseString(currency_)
		.parseString(allowedCountries_)
		;
	if(!parser.parseEnded())
		parser.parseUINT32(adminIntId);
	descr = descr_;
	currency = currency_;
	allowedCountries = allowedCountries_;
}

void ClaimCode::composeData(CommMsgBody& body) const
{
	body
		.composeUINT32(claimCodeId)
		.composeUINT32(productId)
		.composeString(claimCode)
		;
	expDate.compose(body);
	body
		.composeUINT32(orderId)
		.composeUINT32((UINT32)status)
		.composeUINT32(adminIntId)
		.composeSrvTime(created)
		;
}

void ClaimCode::parseData(CommMsgParser& parser)
{
	const char* claimCode_;
	UINT32 status_;
	parser
		.parseUINT32(claimCodeId)
		.parseUINT32(productId)
		.parseString(claimCode_)
		;
	expDate.parse(parser);
	parser
		.parseUINT32(orderId)
		.parseUINT32(status_)
		.parseUINT32(adminIntId)
		.parseSrvTime(created)
		;
	claimCode = claimCode_;
	status = (eClaimCodeStatus)status_;
}

void GiftCardInfo::composeData(CommMsgBody& body) const
{
	body
		.composeUINT32(specialType)
		.composeString(claimCode)
		.composeString(vendorName)
		.composeString(vendorDescr)
		.composeUINT32(price)
		.composeString(currency)
		.composeSrvDate(expDate) // PYR-23546
		;
	// DO NOT ADD NEW PARAMETERS BELOW - ADD THEM IN VipStorePurchaseHelper::composeReply
}

void GiftCardInfo::parseData(CommMsgParser& parser)
{
	const char* claimCode_;
	const char* vendorName_;
	const char* vendorDescr_;
	const char* currency_;
	parser
		.parseUINT32(specialType)
		.parseString(claimCode_)
		.parseString(vendorName_)
		.parseString(vendorDescr_)
		.parseUINT32(price)
		.parseString(currency_)
		.parseSrvDate(expDate) // PYR-23546
		;
	claimCode = claimCode_;
	vendorName = vendorName_;
	vendorDescr = vendorDescr_;
	currency = currency_;
}

void IniDataFromDbm::compose( CommMsgBody& body ) const
{
	body.composeUINT32(sections.size());
	for (int n = 0; n < sections.size(); n++)
	{
		body.composeString(sections[n].name);
		body.composeUINT32(sections[n].items.size());
		for (int i = 0; i < sections[n].items.size(); i++)
		{
			const Item& curItem = sections[n].items[i];
			body
				.composeString(curItem.name)
				.composeString(curItem.value)
				.composeINT32(curItem.type)
				;
		}
	}
}

void IniDataFromDbm::parse( CommMsgParser& parser )
{
	sections.clear();
	const char* name;
	const char* value;
	INT32		type;

	UINT32	numSections, numItems;

	parser.parseUINT32(numSections);
	for (int n = 0; n < numSections; n++)
	{
		parser.parseString(name);
		Section* section = addSection(name);
		parser.parseUINT32(numItems);
		for (int i = 0; i < numItems; i++)
		{
			parser
				.parseString(name)
				.parseString(value)
				.parseINT32(type)
				;
			Item item(name, value, type);
			section->items.push_back(item);
		}
	}
}

void IniDataFromDbm::merge( PIniFile& file, bool doLog /* =true */ )
{
	PLog("IniDataFromDbm::merge");
	for (int n = 0; n < sections.size(); n++)
	{
		const Section& sectionFromDbm = sections[n];
		PIniFile::Section* sectionFromFile = file.getSection(sectionFromDbm.name);
		if (sectionFromFile)
		{
			if (doLog)
			{
				PLog("D[%s]", sectionFromFile->name.c_str());
			}
			for (int i = 0; i < sectionFromDbm.items.size(); i++)
			{
				const Item& itemData = sectionFromDbm.items[i];
				bool found = false;
				for (int c = 0; c < sectionFromFile->items.size(); c++)
				{
					PIniFile::Item& itemFile = sectionFromFile->items[c];
					if (itemData.type == eNonUnique)
					{
						if (itemData.name.equalsIgnoreCase(itemFile.name) && itemData.value.equals(itemFile.value))
						{
							found = true;
							break;
						}
						else
						{
							continue;
						}
					}
					if (itemData.type == eUnique)
					{
						found = true;
						sectionFromFile->setProperty(itemData.name, itemData.value);
						if (doLog)
						{
							PLog("D|%s=%s", itemData.name.c_str(), itemData.value.c_str());
						}
						// PYR-88700 - force log even !doLog
						for (const PIniFile::Item& item : sectionFromFile->items)
						{
							if (item.name.equalsIgnoreCase(itemData.name) && !item.value.equals(itemData.value))
							{
								PLog("Error: INIDATA merge flag misconfigured (INIDATA.FLAGS) - multiple definitions found for eUnique type; [%s] %s=%s", sectionFromFile->name.c_str(), item.name.c_str(), item.value.c_str());
							}
						}
						break;
					}
				}
				if (!found)
				{
					PIniFile::Item item(itemData.name, itemData.value);
					sectionFromFile->items.push_back(item);
					if (doLog)
					{
						PLog("D|%s=%s", itemData.name.c_str(), itemData.value.c_str());
					}
				}
			}
		}
		else
		{
			PIniFile::Section* section = file.addSection(sectionFromDbm.name);
			if (doLog)
			{
				PLog("D[%s]", section->name.c_str());
			}
			for (int i = 0; i < sectionFromDbm.items.size(); i++)
			{
				PIniFile::Item item(sectionFromDbm.items[i].name, sectionFromDbm.items[i].value);
				section->items.push_back(item);
				if (doLog)
				{
					PLog("D|%s=%s", item.name.c_str(), item.value.c_str());
				}
			}
		}
	}
}

IniDataFromDbm::Section* IniDataFromDbm::addSection( const char* secName )
{
	int n = sections.size();
	for( int i=0; i < n ; ++i )
	{
		Section& sec = sections[ i ];
		if( PString::compareIgnoreCase( secName, sec.name ) == 0 )
			return &sec;
	}
	return &*sections.insert( sections.end(), Section( secName ) );
}

const IniDataFromDbm::Item* IniDataFromDbm::findItem( const char* section_name, const char* item_name, int& secIdx ) const
{
	for(secIdx = 0; secIdx < sections.size(); secIdx++)
	{
		const Section& sec = sections[secIdx];
		if(sec.name.equalsIgnoreCase(section_name))
		{
			for(int itemIdx = 0; itemIdx < sec.items.size(); itemIdx++)
			{
				const Item& item = sec.items[itemIdx];
				if(item.name.equalsIgnoreCase(item_name))
				{
					return &item;
				}
			}
		}
	}
	secIdx = -1;
	return 0;
}

const char* IniDataFromDbm::getDbItemValue( int secIdx, const char* fileItemName, const char* fileItemValue ) const
{
	if (secIdx < 0 || secIdx >= sections.size())
	{
		return "";
	}

	const Section& sec = sections[secIdx];
	for(int itemIdx = 0; itemIdx < sec.items.size(); itemIdx++)
	{
		const Item& item = sec.items[itemIdx];
		if(item.name.equalsIgnoreCase(fileItemName) &&
		   item.value.equalsIgnoreCase(fileItemValue))
		{
			return item.value.c_str();
		}
	}
	return "";
}

void IniDataFromDbm::composeAmmendedIniFile( const PIniFile& file, CommMsgBody& iniBody, bool doLog /* =true */ ) const
{
	PLog("composeAmmendedIniFile");
	UINT32 counter = 0;
	CommMsgBody rawData;
	for(int secIdx = 0; secIdx < file.sections.size(); secIdx++)
	{
		const PIniFile::Section& fileSection = file.sections[secIdx];
		const char* fileSectionName = fileSection.name;
		if (doLog)
		{
			PLog("  [%s]", fileSectionName);
		}
		for(int itemIdx = 0; itemIdx < fileSection.items.size(); itemIdx++)
		{
			const PIniFile::Item& fileItem =  fileSection.items[itemIdx];
			const char* fileItemName = fileItem.name;
			const char* fileItemValue = fileItem.value;
			int sectionIdx;
			const Item* dbItem = findItem(fileSectionName, fileItemName, sectionIdx);
			const char* dbItemValue = dbItem ? dbItem->value.c_str() : "";
			if (dbItem && dbItemValue[0] && dbItem->type == eNonUnique)  //#7847
			{
				dbItemValue = getDbItemValue(sectionIdx, fileItemName, fileItemValue);
			}
			CommMsgBody oneItemBody;
			oneItemBody
				.composeString(fileSectionName)
				.composeString(fileItemName)
				.composeString(fileItemValue)
				.composeString(dbItemValue);
			rawData.composeMsgBody(oneItemBody);
			counter++;
			if (doLog)
			{
				PLog("  %s=[%s|%s]", fileItemName, fileItemValue, dbItemValue);
			}
		}
	}
	iniBody.composeUINT32(counter);
	if(counter)
		iniBody.merge(rawData);
}

const IniDataFromDbm::Section* IniDataFromDbm::getSection( const char* sectionName ) const
{
    for(int secIdx = 0; secIdx < sections.size(); secIdx++)
    {
        const Section& sec = sections[secIdx];
        if(sec.name.equalsIgnoreCase(sectionName))
        {
            return &sec;
        }
    }
    return NULL;
}

const char* IniDataFromDbm::Section::getProperty( const char* name_, const char* defaultValue ) const
{
    int n = items.size();
    for(int i = 0; i < n ; ++i)
    {
        const Item& item = items[i];
        if( item.name.equalsIgnoreCase(name_))
            return item.value;
    }
    return defaultValue;
}

////////////////////////////////////////////////////////////////////////////////
// PYR-29604
// Ban server enumerations and structures moved from shared.h/.cpp to BanServerCommon.h/.cpp
////////////////////////////////////////////////////////////////////////////////

void VIPStoreBonusInfo::composeData( CommMsgBody& body ) const
{
	body
		.composeString(bonusName)
		.composeUINT32(mustEarn)
		.composeUINT32(expPeriod)
		.composeSrvTime(expDate)
		.composeUINT32(releasePortion)
		.composeUINT32(fppReleasePortion)
		.composeBOOL(isInstantBonus);
	// DO NOT ADD NEW PARAMETERS BELOW - ADD THEM IN VipStorePurchaseHelper::composeReply
}

void VIPStoreBonusInfo::parseData( CommMsgParser& parser )
{
	parser
		.parseStringP(bonusName)
		.parseUINT32(mustEarn)
		.parseUINT32(expPeriod)
		.parseSrvTime(expDate)
		.parseUINT32(releasePortion)
		.parseUINT32(fppReleasePortion)
		.parseBOOL(isInstantBonus);
}

bool i18n_locale_valid(UINT32 locale)
{
	return(locale <= MAX_LOCALES);
}

UINT32 i18n_get_emLocale(UINT32 emLocale, UINT32 locale)
{
	return(i18n_locale_valid(emLocale) ? emLocale : locale);
}

void TLBPrize::composeData( CommMsgBody& body ) const
{
	body
		.composeString(userId)
		.composeUINT32(intervalId)
		.composeUINT32(division)
		.composeUINT32(subdivision)
		.composeINT32(amount)
		.composeString(comment)
		.composeString(currency)
		;
}

void TLBPrize::parseData( CommMsgParser& parser )
{
	parser
		.parseStringP(userId)
		.parseUINT32(intervalId)
		.parseUINT32(division)
		.parseUINT32(subdivision)
		.parseINT32(amount)
		.parseStringP(comment)
		.parseStringP(currency)
		;
}

void TLBPrize::log() const
{
	PLog( "TLB Prize: '%s' interval %d, div %d, subdiv %d, amount %d (%s)",
		userId.c_str(), intervalId, division, subdivision, amount, currency.c_str() );
}

void FrozenObject::composeData(CommMsgBody& body) const
{
	body
		.composeUINT32(objectId)
		.composeUINT32(objectType)
		.composeString(objectValue);
	expDate.compose(body);
	body
		.composeUINT32((UINT32)sessionId); // #9576
	created.compose(body);
	body
		.composeUINT32(adminIntId)
		.composeString(comment);
	forcedExpDate.compose(body);
	body
		.composeUINT32(forcedAdminIntId)
		.composeString(forcedCommet)
		.composeUINT64(sessionId) // #9576
		.composeUINT32(freezeReason); // PYR-24890
}

void FrozenObject::parseData(CommMsgParser& parser)
{
	const char* objectValue_;
	const char* comment_;
	const char* forcedCommet_;
	UINT32 sessionId32;
	parser
		.parseUINT32(objectId)
		.parseUINT32(objectType)
		.parseString(objectValue_);
	expDate.parse(parser);
	parser
		.parseUINT32(sessionId32); // #9576
	created.parse(parser);
	parser
		.parseUINT32(adminIntId)
		.parseString(comment_);
	forcedExpDate.parse(parser);
	parser
		.parseUINT32(forcedAdminIntId)
		.parseString(forcedCommet_)
		.parseUINT64(sessionId)
		.parseUINT32(freezeReason);

	objectValue = objectValue_;
	comment = comment_;
	forcedCommet = forcedCommet_;
}

void FreezeEvent::composeData(CommMsgBody& body) const
{
	body
		.composeUINT32(eventId)
		.composeUINT32(userIntId)
		.composeUINT32(objectId);
	when.compose(body);
	body
		.composeString(installId)
		.composeString(volumeLabel)
		.composeString(macAddress)
		.composeString(ipAddress)
		.composeString(extraInfo)
		.composeString(routerMacAddress);
}

void FreezeEvent::parseData(CommMsgParser& parser)
{
	const char* installId_;
	const char* volumeLabel_;
	const char* macAddress_;
	const char* routerMacAddress_;
	const char* ipAddress_;
	const char* extraInfo_;
	parser
		.parseUINT32(eventId)
		.parseUINT32(userIntId)
		.parseUINT32(objectId);
	when.parse(parser);
	parser
		.parseString(installId_)
		.parseString(volumeLabel_)
		.parseString(macAddress_)
		.parseString(ipAddress_)
		.parseString(extraInfo_);
	
	// TODO: leonidh - remove conditional parsing after build 2015.09
	if (!parser.parseEnded())
	{
		parser.parseString(routerMacAddress_);
	}
	else
	{
		routerMacAddress_ = "";
	}
		
	installId = installId_;
	volumeLabel = volumeLabel_;
	macAddress = macAddress_;
	routerMacAddress = routerMacAddress_;
	ipAddress = ipAddress_;
	extraInfo = extraInfo_;
}

void unwrapReplayMessageBody(const CommMsgBody& bodyIn, CommMsgBody& bodyOut, SrvTime& msg_time, time_t& msg_t)
{
	int size = bodyIn._size() - sizeof(UINT32) - sizeof(SrvTime);
	BYTE * ptr = bodyOut._alloc(size);

	memcpy(ptr,bodyIn._readPtr(),size);
	CommMsgBody tmp;
	BYTE *ptr1 =tmp._alloc(sizeof(UINT32) + sizeof(SrvTime)); //-V119
	memcpy(ptr1,bodyIn._readPtr() + size,sizeof(UINT32) + sizeof(SrvTime)); //-V119
	CommMsgParser parser(tmp);
	parser.parseUINT32((UINT32&)msg_t);
	msg_time.parse(parser);
}

void wrapReplayMessageBody(const CommMsgBody& bodyIn, CommMsgBody& bodyOut, const SrvTime& msg_time, time_t msg_t)
{
	bodyOut.copyFrom(bodyIn);
	bodyOut.composeUINT32(msg_t);
	msg_time.compose(bodyOut);
}

void strTokenize(const char* p, vector<PString>& tokens, const char* sep)
{       
    const char* ln = p;
    PString token;

    bool foundEscape;
    if(*ln == '\0' || *sep == '\0')
        return;
    else if(*ln == *sep)
    {
        token.assign("");
        tokens.push_back(token);
        foundEscape = false;
    }
    else if(*ln != '"')
    {
        token.append(*ln);
        foundEscape = false;
    }
    else
        foundEscape = true;

    while(*ln != '\0')
    {
        ++ln;
        if(*ln == '"' && !foundEscape && token.length() == 0)
        {
            foundEscape = true;
            continue;                    
        }

        if(*ln == '\0')
        {
            PString trimmedToken(PString::ltrim(token), PString::rtrim(token));
            tokens.push_back(trimmedToken);
            break;
        }
        else if(*ln != *sep)
        {
            if(foundEscape)
            {
                if(*ln == '"' && (*(ln+1) == '"'))
                {
                    ++ln; //escaped double quote
                }
                else if(*ln == '"')
                {
                    foundEscape = false;
                    continue;
                }
            }
            token.append(*ln);            
        }
        else
        {                
            if(foundEscape)
                token.append(*ln);            
            else
            {                    
                PString trimmedToken(PString::ltrim(token), PString::rtrim(token));
                tokens.push_back(trimmedToken);
                token.assign("");
            }
        }
    }    
}

void readStrVectorFromIniFile( const PIniFile& iniFile, const char* sectionName, const char* itemName, const char* defaultStrValue, vector<PString>& strItems )
{
	PLog("readStrVectorFromIniFile: [%s] %s", sectionName, itemName);
	strItems.clear();
	const PIniFile::Section* section = iniFile.getSection(sectionName);
	if(section)
	{
		const char* itemStr = section->getProperty(itemName, defaultStrValue);
		readStrVectorFromString(itemStr, strItems);
		PLog(" %d items read (%s)", (INT32)strItems.size(), itemStr);
	}
	else
	{
		PLog(" section not found");
	}
}

bool isStringInVector( const char* str, const vector<PString>& vec, bool ignoreCase /*= false*/ )
{
	if (str == NULL)
		return false;

	for (int n = 0; n < vec.size(); n++)
	{
		bool match = ignoreCase ? vec[n].equalsIgnoreCase(str) : vec[n].equals(str);
		if(match)
		{
			return true;
		}
	}
	return false;
}

PString& urlEncodeValue(const char* value, PString& encodedValue, bool notLast /*= true*/)
{
	encodedValue = "";
	CommCgi::urlEncode(encodedValue, value);
	if(notLast)
	{
		encodedValue.append('&');
	}
	return encodedValue;
}

void encodeAsTransactionComment(INT32 value, PString& comment)
{
	comment.appendInt(value);
}

void normalizeEmail(const char* email, PString& normalizedEmail)
{
	normalizedEmail = PString(PString::ltrim(email), PString::rtrim(email)); // trimmed in a scope of PYR-28772
	normalizedEmail.toLower();
}

void genTmpPwd( PString& out )
{
	static const char _tmp_passwdSymbols[] = "abcdefghijkmnpqrstuvwxyz23456789";

	out = TEMP_PWD_PREFIX;
	const size_t BSZ = 6;
	BYTE buff[BSZ*2];
	CommSSL::generateRandomBytes( buff, BSZ*2 );
	for( int i = 0; i < BSZ; i++ )
	{
		out.append( char( _tmp_passwdSymbols[ (*(UINT16*)(buff + i*2)) % strlen(_tmp_passwdSymbols)] ) ); //-V1032
	}
}

const char* getItemValue(PString& out, const PIniFile::Item& item)
{
	const char* comment = item.value.find(';');
	const char* comment2 = item.value.find("//");
	if( !comment || ( comment2 && comment > comment2 ) )
	{
		comment = comment2;
	}
	PString value;
	if( comment )
	{
		value.assign( item.value, comment );
	}
	else
	{
		value.assign( item.value );
	}
	out.assign( value, value.rtrim() );
	
	return out.c_str();
}

INT64 inverseConvRate(INT64 convRate)
{
	if (convRate == 0)
	{
		PLog("inverseConvRate ERROR: convRate=0, changed to CONV_RATE_BASE");
		return CONV_RATE_BASE;
	}
	if (convRate == CONV_RATE_BASE)
	{
		return CONV_RATE_BASE;
	}
	return mulDivRnd64(CONV_RATE_BASE, CONV_RATE_BASE, convRate, DO_ROUND_CONVRATE);
}

INT64 ConvRatesAndMargins::calculateConvAmounts(INT32 amtSrc_)
{
	amounts.init(amtSrc_);
	if (!conversionExists())
	{
		return amounts.amtDst;
	}
	if (isZeroCentsProcCur(curSrc) && (amounts.amtSrc % HUNDRED_CENTS) != 0) // PYR-56416
	{
		PLog("MINOR UNIT CONV SRC %d %s->%s", amounts.amtSrc, curSrc.c_str(), curDst.c_str());
	}
	// do truncation in amount calculation, last default parameter in mulDivRnd64 call is false
	// the only reason for amounts truncation is comaptibility with the client's truncation for deposits
	amounts.amtSrcUS	= mulDivRnd64NonZero(amounts.amtSrc, rateSrc, CONV_RATE_BASE);
	amounts.amtDst      = mulDivRnd64NonZero(amounts.amtSrc, rateFin, CONV_RATE_BASE);
	amounts.amtDstNoMgn = mulDivRnd64NonZero(amounts.amtSrc, rateMid, CONV_RATE_BASE); // converted amount with Mid-market rate and Zero margin
	if (isZeroCentsProcCur(curDst) && (amounts.amtDst % HUNDRED_CENTS) != 0) // PYR-56416
	{
		INT64 _amtDst =  (amounts.amtDst / HUNDRED_CENTS) * HUNDRED_CENTS;
		PLog("MINOR UNIT CONV DST %d %s->%s=%lld(%lld)", amounts.amtSrc, curSrc.c_str(), curDst.c_str(), amounts.amtDst, _amtDst);
		amounts.amtDst = _amtDst;
	}
	amounts.amtDstUS    = mulDivRnd64NonZero(amounts.amtDst, rateDst, CONV_RATE_BASE);
	if (mgnVal != 0)
	{
		// calculate margins for reporting
		// amtMgn sign is the same as (amtSrc_*mgnVal_) sign
		amounts.amtMgnDst = amounts.amtDstNoMgn - amounts.amtDst;
		amounts.amtMgnSrc = mulDivRnd64NonZero(amounts.amtMgnDst, rateDst, rateSrc);
		amounts.amtMgnUS  = mulDivRnd64NonZero(amounts.amtMgnDst, rateDst, CONV_RATE_BASE);
	}
	return amounts.amtDst;
}
bool ConvRatesAndMargins::conversionExists() const
{
	if (curSrc.length() == 0 && curDst.length() == 0)
		return false;
	return (!compareCurrencies(curSrc, curDst));
}
void ConvRatesAndMargins::parse(CommMsgParser& parser)
{
	UINT32 intActionType;
	parser
		.parseINT64  (rateSrc  )
		.parseINT64  (rateDst  )
		.parseINT64  (rateMid  )
		.parseINT64  (rateFin  )
		.parseINT32  (mgnVal   )
		.parseINT32  (amounts.amtMgnSrc)
		.parseINT32  (amounts.amtMgnDst)
		.parseINT32  (amounts.amtMgnUS )
		.parseStringP(curSrc   )
		.parseINT32  (amounts.amtSrc   )
		.parseINT32  (amounts.amtSrcUS )
		.parseStringP(curDst   )
		.parseINT64  (amounts.amtDst   )
		.parseINT64  (amounts.amtDstNoMgn)
		.parseINT32  (amounts.amtDstUS )
		.parseUINT32 (intActionType)
		;
	actionType = ConversionActionType (intActionType);
}
void ConvRatesAndMargins::compose(CommMsgBody& body) const
{
	body
		.composeINT64 (rateSrc  )
		.composeINT64 (rateDst  )
		.composeINT64 (rateMid  )
		.composeINT64 (rateFin  )
		.composeINT32 (mgnVal   )
		.composeINT32 (amounts.amtMgnSrc)
		.composeINT32 (amounts.amtMgnDst)
		.composeINT32 (amounts.amtMgnUS )
		.composeString(curSrc   )
		.composeINT32 (amounts.amtSrc   )
		.composeINT32 (amounts.amtSrcUS )
		.composeString(curDst   )
		.composeINT64 (amounts.amtDst   )
		.composeINT64 (amounts.amtDstNoMgn)
		.composeINT32 (amounts.amtDstUS )
		.composeUINT32(UINT32(actionType))
		;
}

void ConvRatesAndMargins::parseVector(CommMsgParser& parser, vector<ConvRatesAndMargins>& vec)
{
	UINT32 size;
	parser.parseUINT32(size);
	vec.reserve(size);
	ConvRatesAndMargins cm;
	for(int i = 0; i < size; ++i)
	{
		cm.parse(parser);
		vec.push_back(cm);
	}
}
void ConvRatesAndMargins::composeVector(CommMsgBody& body, const vector<ConvRatesAndMargins>& vec)
{
	body.composeUINT32( vec.size() );
	for( int i = 0; i < vec.size(); ++i )
	{
		vec[i].compose(body);
	}
}

void ConvRatesAndMargins::mergeVectors(vector<ConvRatesAndMargins>& vecIO, const vector<ConvRatesAndMargins>& vecToMerge)
{
	if (vecToMerge.empty())
		return;

	for (int i = 0; i < vecToMerge.size(); ++i)
	{
		const ConvRatesAndMargins& cmToMerge = vecToMerge[i];
		mergeIntoVector(vecIO, cmToMerge);
	}
}

void ConvRatesAndMargins::mergeIntoVector(vector<ConvRatesAndMargins>& vecIO, const ConvRatesAndMargins& cmToMerge)
{
	bool foundForIncrement = false;
	for (int n = 0; n < vecIO.size(); ++n)
	{
		ConvRatesAndMargins& cmIO = vecIO[n];
		if (cmToMerge.curSrc.equals(cmIO.curSrc) && 
			cmToMerge.curDst.equals(cmIO.curDst) && 
			cmToMerge.actionType == cmIO.actionType)
		{
			foundForIncrement = true;
			cmIO.amounts.add(cmToMerge.amounts); // update - increment
			break;
		}
	}
	if (!foundForIncrement)
	{
		vecIO.push_back(cmToMerge); // insert
	}
}

const char* ConvRatesAndMargins::vectorToString(PString& out, const vector<ConvRatesAndMargins>& vec)
{
	out.assign("[").appendInt(vec.size()).append(']');
	for (int i = 0; i < vec.size(); ++i)
	{
		const ConvRatesAndMargins& cm = vec[i];
		if (i != 0)
			out.append(' ');
		out.appendInt(cm.amounts.amtSrc).append(cm.curSrc).append('>')
			.appendInt64(cm.amounts.amtDst).append(cm.curDst);
	}
	return out.c_str();
}


void ConvRatesAndMargins::reverseConversionDirection() // swaps Src and Dst
{
	swap(curSrc, curDst);
	swap(rateSrc, rateDst);
	rateMid = inverseConvRate(rateMid);
	rateFin = inverseConvRate(rateFin);
	mgnVal = -mgnVal;

	INT64 tmp = amounts.amtDst;
	amounts.amtDst = amounts.amtSrc;
	amounts.amtSrc = tmp;

	swap(amounts.amtSrcUS, amounts.amtDstUS);
	swap(amounts.amtMgnSrc, amounts.amtMgnDst);

	amounts.amtMgnSrc = -amounts.amtMgnSrc;
	amounts.amtMgnDst = -amounts.amtMgnDst;
	amounts.amtMgnUS  = -amounts.amtMgnUS;
	amounts.amtDstNoMgn = amounts.amtDst + amounts.amtMgnDst;
}

bool ConvRatesAndMargins::isUp1Conversion() const // checks if a 1 cent of destination currency worth more than a source
{
	return ((amounts.amtDst == 1) && (amounts.amtSrc > 0) && (INT64(amounts.amtSrc) * rateFin < CONV_RATE_BASE));
}

bool ConvRatesAndMargins::removeUp1Conversion()
{
	bool isUp1 = isUp1Conversion();
	if (isUp1)
	{
		amounts.clear(); // no up-converting to 1 cent
	}
	return isUp1;
}

void composeCurrencyContext(CommMsgBody& currencyContext, const SrvTime& srvTime, const vector<ConvRatesAndMargins>& cmVec)
{
	currencyContext.composeSrvTime(srvTime);
	ConvRatesAndMargins::composeVector(currencyContext, cmVec);
}
void parseCurrencyContext(CommMsgParser& currencyContext, SrvTime& srvTime, vector<ConvRatesAndMargins>& cmVec)
{
	currencyContext.parseSrvTime(srvTime);
	ConvRatesAndMargins::parseVector(currencyContext, cmVec);
}

bool accountIsSourceNotDest(enum ConversionActionType actionType)
{
	switch (actionType)
	{
		case eConvActionTournRegister:
		case eConvActionCashout:
			return true;

		case eConvActionTournUnregister:
		case eConvActionTournWon:
		case eConvActionTournPsCancelled:
		case eConvActionTournPsCancelRollforw:
		case eConvActionDeposit:
		case eConvActionTableLeave:
		case eConvActionQuestCashCredit:   // PYR-32134
		case eConvActionSportsBookGeneric: // PYR-35556
		case eConvActionSportsBookWin:     // PYR-35556
		case eConvActionSportsBookVoid:    // PYR-35556
		case eConvActionStarsDraftSecureFunds  : // PYR-40346
		case eConvActionStarsDraftContestPayout: // PYR-40346
		case eConvActionStarsDraftVoidEntry    : // PYR-40346
		case eConvActionStarsDraftReversePayout: // PYR-40346
		case eConvActionStarsDraftRefundPartial: // PYR-40346
		case eConvActionPoolBettingPlace    : // PYR-42410
		case eConvActionPoolBettingSettle   : // PYR-42410
		case eConvActionPoolBettingCashout  : // PYR-42410
		case eConvActionPoolBettingVoid     : // PYR-42410
			return false;

		// cases below are true for debit-redeem transactions and false for credit-purchase
		case eConvActionForcedInterAcctTransfer:
		case eConvActionAutoInterAcctTransfer:
		case eConvActionInterAcctTransfer:
		case eConvActionReconcileDebt:
		case eConvActionP2PTransfer:
		default:
			PLog("accountIsSourceNotDest: actionType=%d not listed", UINT32(actionType));
			return false;
	}
}


// #9217
bool checkItemSiteId(UINT32 userSiteId, UINT32 itemMask)
{
    if (itemMask == 0)      // unchecked
        return true;
	else
        return (itemMask & userSiteId) != 0;
}

bool compareCurrencies(const char* currency1, const char* currency2, bool logFaultyLogicIfEmptyCurrency /*= true*/)
{
	if ((!currency1 || !*currency1) && (!currency2 || !*currency2))
		return true; // PYR-51774 no FL logging if both currencies are null or empty

	return PString::equals(
		normalizedCurrency(currency1, logFaultyLogicIfEmptyCurrency), 
		normalizedCurrency(currency2, logFaultyLogicIfEmptyCurrency));
}

const char* normalizedCurrency(const char* currency, bool logFaultyLogicIfEmptyCurrency /*= true*/)
{
	if (!currency || !*currency)
	{
		if (logFaultyLogicIfEmptyCurrency)
		{
			PTRACE5(/*LOG_TRACE_FAULTY_LOGIC*/ "normCur(%s)", currency ? "EMP" : "NUL"); // PYR-51774
		}
		return "USD";
	}
	return currency;
}

bool isZeroCentsProcCur(const char* processorCurrency) // PYR-56416 returns true for PROCESSOR external currencies that required minor units (cents) to be zero
{
	return PString::equals(processorCurrency, "CLP"); // Chilean Peso, only non-PS-account currencies can be added
}


void UserProperty::compose( CommMsgBody& body ) const
{
	body
		.composeINT32(propType)
		.composeINT32(propInt)
		.composeString(propStr);
	propWhen.compose(body);
}

void UserProperty::parse( CommMsgParser& parser )
{
	parser
		.parseINT32(propType)
		.parseINT32(propInt)
		.parseStringP(propStr);
	propWhen.parse(parser);
}

/*static*/ const UserProperty* UserProperty::findInVector(const vector<UserProperty>& userPropsVec, int propType_)
{
	for (int i = 0; i < userPropsVec.size(); ++i)
	{
		const UserProperty& up = userPropsVec[i];
		if (up.propType == propType_)
		{
			return &up;
		}
	}
	return NULL;
}
/*static*/ UserProperty* UserProperty::findInVector(vector<UserProperty>& userPropsVec, int propType_) //-V659
{
	for (int i = 0; i < userPropsVec.size(); ++i)
	{
		UserProperty& up = userPropsVec[i];
		if (up.propType == propType_)
		{
			return &up;
		}
	}
	return NULL;
}

/*static*/ bool UserProperty::findInVector(const vector<UserProperty>& userPropsVec, int propType_, UserProperty& userProp)
{
	const UserProperty* up = findInVector(userPropsVec, propType_);
	if (!up)
		return false;
	userProp = *up;
	return true;
}

// PYR-18101
void UserSpecialInstructions::compose( CommMsgBody& body ) const
{
	body
		.composeINT32(departmentId)
		.composeString(instructions)
		.composeSrvTime(when)
		;
}

// PYR-18101
void UserSpecialInstructions::parse( CommMsgParser& parser )
{
	parser
		.parseINT32(departmentId)
		.parseStringP(instructions)
		.parseSrvTime(when)
		;
}

void EnvoyBankInfo::parse( CommMsgParser& parser )
{
	parser
		.parseUINT32(bankrefId)
		.parseStringP(currency)
		.parseStringP(name)
		.parseStringP(city)
		.parseStringP(address)
		.parseStringP(swift)
		.parseStringP(url)
		.parseStringP(accountNum)
		.parseStringP(iban)
		.parseStringP(benficiary)
		.parseStringP(extraInfo)
		.parseBOOL(preferred)

		.parseStringP(paysystem)
		.parseStringP(bankId)
		.parseStringP(comment)
		.parseUINT32 (flags)
		.parseStringP(country);
}

void EnvoyBankInfo::compose( CommMsgBody& body ) const
{
	body
		.composeUINT32(bankrefId)
		.composeString(currency)
		.composeString(name)
		.composeString(city)
		.composeString(address)
		.composeString(swift)
		.composeString(url)
		.composeString(accountNum)
		.composeString(iban)
		.composeString(benficiary)
		.composeString(extraInfo)
		.composeBOOL(preferred)

		.composeString(paysystem)
		.composeString(bankId)
		.composeString(comment)
		.composeUINT32(flags)
		.composeString(country);
}

void EnvoyCountryBanks::parse( CommMsgParser& parser )
{
	parser.parseStringP(country);
	UINT32 sz;
	parser.parseUINT32(sz);
	for (int i = 0; i < sz; ++i)
	{
		EnvoyBankInfo info;
		info.parse(parser);
		banks.push_back(info);
	}
}

void EnvoyCountryBanks::compose( CommMsgBody& body ) const
{
	body
		.composeString(country)
		.composeUINT32(banks.size());
	for (int i = 0; i < banks.size(); ++i)
	{
		banks[i].compose(body);
	}
}

bool isAlmostTheSame(INT32 first, INT32 second, UINT32 tol_x_1Million)
{
	if (first == second)
		return true;
	if ((first < 0 && second > 0) || (first > 0 && second < 0))
		return false; // must have the same sign
	if (0 == first || 0 == second)
		return false; // non-zero never almost equals to zero

	// make both positive
	if (first < 0)
		first = 0-first;
	if (second < 0)
		second = 0-second;

	UINT32 d; // relative difference * 1 million
	if (first > second)
		d = PMulDiv(first-second, ONE_MILLION, first );
	else // second > first
		d = PMulDiv(second-first, ONE_MILLION, second);

	return ( d < tol_x_1Million );
}

// If the function succeeds, the return value is the result of the multiplication and division
// If either an overflow occurred or denominator is 0, the return value is -1 (similar to MulDiv)
// The local usage of floating point variables was blessed by DL
INT64 mulDivRnd64 ( INT64 number, INT64 numerator, INT64 denominator, bool doRoundNotTruncation /* =false */ )
{
	if (0 == number || 0 == numerator)
		return 0;
	if (0 == denominator)
	{
		PLog("mulDivRnd64 error div/0");
		return -1;
	}
	double result = double(number) * double(numerator) / double(denominator);
	const double INT64_MAXIMUM = double(ONE64<<63);
	if (result >= INT64_MAXIMUM || result < -INT64_MAXIMUM)
	{
		char buf1[32], buf2[32], buf3[32];
		PLog("mulDivRnd64 error overflow: (%s,%s,%s)", p_i64toa(number, buf1), p_i64toa(numerator, buf2), p_i64toa(denominator, buf3));
		return -1;
	}
	// round and convert
	if (doRoundNotTruncation)
	{
		if (result < 0)
			result -= 0.5;
		else
			result += 0.5;
	}
	return INT64(result); //-V2003
}

// non-zero amount can not be converted to zero amount
INT64 mulDivRnd64NonZero ( INT64 number, INT64 numerator, INT64 denominator, bool doRoundNotTruncation /* =false */ )
{
	INT64 result = mulDivRnd64(number, numerator, denominator, doRoundNotTruncation);
	if (0 == result)
	{
		if (number > 0)
			result = +1;
		else if (number < 0)
			result = -1;
	}
	return result;
}

INT64 mulMulDivDiv64 ( INT64 number, INT64 numerator1, INT64 numerator2, INT64 denominator1, INT64 denominator2, bool doRoundNotTruncation /* =false */ )
{
	if (0 == number || 0 == numerator1 || 0 == numerator2)
		return 0;
	if (0 == denominator1 || 0 == denominator2)
	{
		PLog("mulMulDivDiv64 error div/0");
		return -1;
	}
	double result = double(number) * double(numerator1) * double(numerator2)/ (double(denominator1) * double(denominator2));
	const double INT64_MAXIMUM = double(ONE64<<63);
	if (result >= INT64_MAXIMUM || result < -INT64_MAXIMUM)
	{
		char buf1[32], buf2[32], buf3[32], buf4[32], buf5[32];
		PLog("mulMulDivDiv64 error overflow: %s*%s*%s/(%s*%s)", p_i64toa(number, buf1), p_i64toa(numerator1, buf2), 
			p_i64toa(numerator2, buf3), p_i64toa(denominator1, buf4), p_i64toa(denominator2, buf5));
		return -1;
	}
	// round and convert
	if (doRoundNotTruncation)
	{
		if (result < 0)
			result -= 0.5;
		else
			result += 0.5;
	}
	return INT64(result); //-V2003
}

UINT64 prevPowerOf(UINT32 base, UINT64 number)
{
	if(base <= 1 || number <= 1)
		return 0;

	UINT64 power = 1;
	while(number > power * base)
	{
		power *= base;
		if(power > (UINT64)-1 / base)
			break;
	}
	return power;
}

void readUintSetFromString( const char* str, set<UINT32>& uintSet, char delimeter )
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

void readMultimapStrStr(const char* str, multimap<PString, PString, PStringCmp>& result,
	const char delimPairs, const char delimInsidePair, const bool trim)
{
	result.clear();
	vector<PString> pairs;
	readStrVectorFromString(str, pairs, delimPairs);
	for (const PString& onePair : pairs)
	{
		vector<PString> gwAndCode;
		readStrVectorFromString(onePair, gwAndCode, delimInsidePair);
		if (gwAndCode.size() != 2)
		{
			PLog(LOG_TRACE_FAULTY_LOGIC "bad config: '%s' must have been <str>%c<int>", onePair.c_str(), delimInsidePair);
			continue;
		}
		if (trim)
		{
			gwAndCode[0] = PString(PString::ltrim(gwAndCode[0]), PString::rtrim(gwAndCode[0]));
			gwAndCode[1] = PString(PString::ltrim(gwAndCode[1]), PString::rtrim(gwAndCode[1]));
		}
		result.emplace(gwAndCode[0], gwAndCode[1]);
	}
}

void trimSpaces(const vector<PString>&inputStrs, vector<PString>& outputStrs)
{
    outputStrs.clear();
    for (size_t i=0; i<inputStrs.size(); ++i)
    {
        PString str(inputStrs[i].ltrim(), inputStrs[i].rtrim());
        if (str[0]!='\0')
            outputStrs.push_back(str);
    }
}

UINT64 makeUINT64( // concatenates the specified values
    UINT32 intLow, // Specifies the low-order 32 bits of the new value
    UINT32 intHigh) // Specifies the high-order 32 bits of the new value
{
	return (UINT64(intHigh) << 32) | UINT64(intLow);
}

void splitStringByChunks(const char* str, UINT32 chunkSize, vector<PString>& chunks)
{
	if(!str || !*str || !chunkSize)
	{
		return;
	}

	UINT32 totalLength = PString::length(str);
	const char* endPtr = str + totalLength;
	const char* from = str;

	while(from != endPtr)
	{
		const char* to = from + chunkSize > endPtr ? endPtr : from + chunkSize;
		PString chunk(from, to);
		chunks.push_back(chunk);
		from = to;
	}
}

const char* srvTimeToString(const SrvTime& srvTime, PString& strValue, char dateSeparator, char middleSeparator, char timeSeparator)
{
	char strValueBuf[64] = {0};
	if(!srvTime.isNull())
	{
		sprintf(strValueBuf, "%d%c%02d%c%02d%c%02d%c%02d%c%02d",
			srvTime._year, dateSeparator, srvTime._month , dateSeparator, srvTime._day,	middleSeparator, 
			srvTime._hour, timeSeparator, srvTime._minute, timeSeparator, srvTime._second);
	}
	strValue = strValueBuf;
	return strValue.c_str();
}

const char* srvTimeToString(const CommUtcTime& srvTime, PString& strValue, char dateSeparator, char middleSeparator, char timeSeparator)
{
	char strValueBuf[64] = {0};
	if(!srvTime.isNull())
	{
		sprintf(strValueBuf, "%d%c%02d%c%02d%c%02d%c%02d%c%02d",
			srvTime._year, dateSeparator, srvTime._month , dateSeparator, srvTime._day,	middleSeparator, 
			srvTime._hour, timeSeparator, srvTime._minute, timeSeparator, srvTime._second);
	}
	strValue = strValueBuf;
	return strValue.c_str();
}

const char* srvDateToString(const SrvDate& srvDate, PString& strValue, char dateSeparator)
{
	char strValueBuf[64] = {0};
	if(!srvDate.isNull())
	{
		sprintf(strValueBuf, "%d%c%02d%c%02d", srvDate._year, dateSeparator, srvDate._month , dateSeparator, srvDate._day);
	}
	strValue = strValueBuf;
	return strValue.c_str();
}

SrvTime currentLocalTime()
{
	SrvTime timeNow;
	timeNow.currentLocalTime();
	return timeNow;
}

// ReadableInt for 2013-Dec-23 is 20131223: YYYYMMDD
INT32 srvDateToReadableInt(const SrvDate& d)
{
	return TimeUtils::dateToIntForDb(d);
}
INT32 srvDateToReadableInt(const SrvTime& d) // ignores day fraction
{
	return (INT32)d._year * 10000 + (INT32)d._month * 100 + (INT32)d._day;
}
INT64 srvTimeToReadableInt64(const SrvTime& t)
{
	return TimeUtils::timeToInt64ForDb(t);
}
INT64 utcTimeToReadableInt64(const CommUtcTime& t)
{
	return TimeUtilsUtc::timeToInt64ForDb(t);
}

const SrvDate& readableIntToSrvDate(INT32 readableDate, SrvDate& d)
{
	d = TimeUtils::intForDbToDate(readableDate); 
	PASSERT5(d._day <= 31 && d._month <= 12 && d._year < 10000);
	return d;
}

int datesIntervalExclWeekends(const SrvDate& from, const SrvDate& to) // originally written for #12875
{
	SrvDate base; // First Saturday 1970 - January 3
	base._year  = 1970;
	base._month = 1;
	base._day   = 3;
	time_t timeBase = base.encode();

//	Sa Su Mo Tu We Th Fr | Sa Su Mo Tu <- week days starting from weekend
//	 0  1  2  3  4  5  6 |  7  8  9 10 <- all days since one Saturday: Saturday is 0, Sunday is 1, ..., Friday is 6
//	----------------------------------
//	 0  0  0  1  2  3  4 |  5  5  5  6 <- weekdays since one Saturday is same as weekdays since one Monday: the weekends count as next Monday

	time_t timeFrom = from.encode();
	int daysSinceBaseFrom = (timeFrom - timeBase)/SECONDS_IN_A_DAY;
	int daysSinceMondayFrom = daysSinceBaseFrom % 7 - 2;
	int weekdaysSinceBaseFrom = (daysSinceBaseFrom / 7) * 5 + max(0, daysSinceMondayFrom);

	time_t timeTo = to.encode();
	int daysSinceBaseTo = (timeTo - timeBase)/SECONDS_IN_A_DAY;
	int daysSinceMondayTo = daysSinceBaseTo % 7 - 2;
	int weekdaysSinceBaseTo = (daysSinceBaseTo / 7) * 5 + max(0, daysSinceMondayTo);

	int diff = weekdaysSinceBaseTo - weekdaysSinceBaseFrom;
	return diff;
}

// split the long string into separate lines and log one by one
void PLogLines( const char* longStr )
{
	vector<PString> lines;
	strTokenize(longStr, lines, "\n");
	for( int i=0; i<lines.size(); i++ )
	{
		PLog("%s", lines[i].c_str());
	}
}

/*lint -save -e1775*/
bool luhnCheckNumber(const char* str)
{
    try
    {
        int sum = 0;
        bool alt = false;
        int len = strlen(str);
        for(int i = len - 1; i >= 0; i--)
        {
            int digit = str[i] - '0';
            if(alt)
            {
                digit *= 2;
                if(digit > 9)
                {
                    digit -= 9;
                }
            }
            sum += digit;
            alt = !alt;
        }
        return ( sum % 10 ) == 0;
    }
    catch(const PError& er)
    {
        PLog("luhnCheckNumber generated exception for '%s': %s", str, er.why());
        return true;
    }
}

char luhnGenerateChar(const char* str) 
{
    try
    {
        int sum = 0;
        bool alt = true;
        int len = strlen(str);
        for(int i = len - 1; i >= 0; i--)
        {
            int digit = str[i] - '0';
            if(alt)
            {
                digit *= 2;
                if(digit > 9)
                {
                    digit -= 9;
                }
            }
            sum += digit;
            alt = !alt;
        }
        int modulo = sum % 10;
        if (modulo == 0)
            return '0';
        modulo = 10 - modulo;
        return (char)(modulo + '0');
    }
    catch(const PError& er)
    {
        PLog("luhnGenerateChar generated exception for '%s': %s", str, er.why());
        return '0';
    }
}
/*lint -restore*/

BYTE getCashTransactionsCounter(UINT64 flags) 
{
    UINT64 counter = (flags >> FIRST_TRANSACTIONS_COUNTER_BIT) & 0xF;
    return (BYTE)counter;
}

const char* paysystemGroupNameToEnglishName(const char* groupName) // #14352
{
	if(0 == strcmp(groupName, TRANSFER_OUT_NAME))
	{
		return "Transfers Sent";
	}
	if(0 == strcmp(groupName, TRANSFER_IN_NAME))
	{
		return "Transfers Received";
	}
	if(0 == strcmp(groupName, ATAS_APPROVAL_NAME))
	{
		return "ATAS Cashout";
	}
	if(0 == strcmp(groupName, CONVERT_FUNDS_NAME))
	{
		return "Currency Conversion";
	}
	return groupName;
}

// #14494
void UserAllAssetsConversion::compose(CommMsgBody& body) const
{
	// client will use the result of compose - append new data, never insert
	body
		.composeINT32(availDst)
		.composeString(currencyDst)
		.composeUINT32(assets.size());
	for (int i = 0; i < assets.size(); ++i)
	{
		CommMsgBody oneBody;
		const OneAsset& asset = assets[i];
		oneBody
			.composeINT32 (asset.assetType)
			.composeINT32 (asset.typedAmount)
			.composeString(asset.currency)
			.composeINT32 (asset.chipsDst)
			.composeINT64 (asset.convRate)
			;
		body.composeMsgBody(oneBody);
	}
}
void UserAllAssetsConversion::parse(CommMsgParser& parser)
{
	UINT32 assetsSize;
	parser
		.parseINT32(availDst)
		.parseStringP(currencyDst)
		.parseUINT32(assetsSize);
	assets.reserve(assetsSize);
	for (int i = 0; i < assetsSize; ++i)
	{
		OneAsset asset;
		CommMsgBody oneBody;
		parser.parseMsgBody(oneBody);
		CommMsgParser oneParser(oneBody);
		INT32 assetTypeInt;
		oneParser
			.parseINT32  (assetTypeInt)
			.parseINT32  (asset.typedAmount)
			.parseStringP(asset.currency)
			.parseINT32  (asset.chipsDst)
			.parseINT64  (asset.convRate)
			;
		asset.assetType = (eAssetType) assetTypeInt;
		assets.push_back(asset);
	}
}
const char* UserAllAssetsConversion::toString(PString& out) const
{
	out.assign("").appendInt(availDst).append(currencyDst).append('=');
	for (int i = 0; i < assets.size(); ++i)
	{
		const OneAsset& a = assets[i];
		if (i != 0)
			out.append('+');
		const char* logMarker = LOG_TRACE_FAULTY_LOGIC;
		switch (a.assetType) //-V719
		{
		case eAssetTypeChipsOnly 	: logMarker = "c"; break;
		case eAssetTypeTChipsOnly	: logMarker = "t"; break;
		case eAssetTypeChipsAndT	: logMarker = "c&t"; break;
		case eAssetTypeFpp      	: logMarker = "f"; break;
		case eAssetTypeTickets  	: logMarker = "TT"; break;
		case eAssetTypeBonus		: logMarker = "b"; break;
		case eAssetTypeVipRewardBonus : logMarker = "v"; break;
		case eAssetTypeUncleared	: logMarker = "u"; break;
		} //lint !e787 // enum constant '...' not used within switch
		out .append('(').append(logMarker).appendInt(a.typedAmount).append(a.currency)
			.append('*').appendInt64(a.convRate).append('=').appendInt(a.chipsDst).append(')');
	}
	return out.c_str();
}
bool UserAllAssetsConversion::conversionExists() const
{
	for (int i = 0; i < assets.size(); ++i)
	{
		switch (assets[i].assetType)
		{
		case eAssetTypeTChipsOnly	:
		case eAssetTypeChipsAndT	:
		case eAssetTypeFpp      	:
		case eAssetTypeTickets  	:
			return true;
		case eAssetTypeChipsOnly	:
			if (!compareCurrencies(assets[i].currency, currencyDst))
				return true;
		} //lint !e787 // enum constant '...' not used within switch
	}
	return false; // same currency chips-only and uncleared
}
void UserAllAssetsConversion::addAsset(const OneAsset& asset)
{
	availDst += asset.chipsDst;
	for (int i = 0; i < assets.size(); ++i)
	{
		OneAsset& existingAsset = assets[i];
		if (existingAsset.assetType == asset.assetType && existingAsset.currency.equals(asset.currency))
		{
			existingAsset.typedAmount += asset.typedAmount;
			existingAsset.chipsDst += asset.chipsDst;
			return;
		}
	}
	assets.push_back(asset);
}
void UserAllAssetsConversion::clear()
{
	assets.clear();
	availDst = 0;
	currencyDst = "";
}
bool UserAllAssetsConversion::equalsBySrc(const UserAllAssetsConversion& another) const
{
	UINT32 size = assets.size();
	if (size != another.assets.size())
		return false;
	for (int i = 0; i < size; ++i)
	{
		const OneAsset& a = assets[i];
		const OneAsset& b = another.assets[i];
		if ((a.assetType != b.assetType) || (a.typedAmount != b.typedAmount) || !a.currency.equals(b.currency))
			return false;
	}
	return true;
}
INT32 UserAllAssetsConversion::getTypedAssetTotalDst(eAssetType assetType) const
{
	INT32 totalDst = 0;
	for (int i = 0; i < assets.size(); ++i)
	{
		const OneAsset& a = assets[i];
		if (a.assetType == assetType)
		{
			totalDst += a.chipsDst;
		}
	}
	return totalDst;
}

const char* replaceCharWithString(PString& sourceStr, char C, const char* str)
{
    PString tmpStr;
    for (int ii=0; ii < sourceStr.length(); ii++)
    {
        if (sourceStr[ii] == C)
            tmpStr.append(str);
        else
            tmpStr.append(sourceStr[ii]);
    }
    sourceStr.assign(tmpStr);
    return sourceStr.c_str();
}

const char* replaceNonAlphaNumWithString(PString& sourceStr, const char* str)
{
    PString tmpStr;
    for (int ii=0; ii < sourceStr.length(); ii++)
    {
        if (!isalpha ((unsigned char)sourceStr[ii]) && !isdigit((unsigned char)sourceStr[ii]))
            tmpStr.append(str);
        else
            tmpStr.append(sourceStr[ii]);
    }
    sourceStr.assign(tmpStr);
    return sourceStr.c_str();
}

// removeRepresentmentSuffix was moved untouch from cashierobject.cpp - 20100520
void removeRepresentmentSuffix(PString& gatewayName)
{
	if (gatewayName.endsWith(REPRESENTMENT_1_STR) || gatewayName.endsWith(REPRESENTMENT_2_STR))
	{
		gatewayName.cut(gatewayName.length()-(sizeof(REPRESENTMENT_1_STR)-1));
	}
}

const char* removeLeadingZeros(const char* account)
{
	const char* ptr = account;
	while (ptr && *ptr == '0')
	{
		++ptr;
	}
	return ptr;
}

const char* checkAccountByIBAN(const char* iban, const char* account)
{
    // 1. Remove spaces from IBAN
    PString noSpacesIban;
	removeSpacesFromString(noSpacesIban, iban);
	// 2.
	const char* ptr = removeLeadingZeros(account);
	// 3. Remove spaces from account
	PString noSpacesAccount;
	removeSpacesFromString(noSpacesAccount, ptr);

    return PString::find(noSpacesIban, noSpacesAccount);
}


bool UserBalanceData::verifyTransfer( const UserBalanceData& transfer ) const
{
	// Check if the transfer has all currencies and amounts do not exceed the maximums.
	for( int i = 0; i < transfer.cashAssets.size(); ++i )
	{
		const UserBalanceData::Cash& cashToTransfer = transfer.cashAssets[i];
		size_t c;
		for( c = 0; c < cashAssets.size(); ++c )
		{
			const UserBalanceData::Cash& cashAtUser = cashAssets[c];
			if( cashToTransfer.currency.equals( cashAtUser.currency ) )
			{
				if (cashToTransfer.chips > cashAtUser.chips || cashToTransfer.tChips > cashAtUser.tChips )
					return false;
				break;
			}
		}
		if (c == cashAssets.size())
			return false;
	}
	return transfer.playChips <= playChips && ((INT64)transfer.fpp * 100 + transfer.fppCents) <= ((INT64)fpp * 100 + fppCents)
		&& transfer.nac2 <= nac2 && transfer.nac2 >= 0;
}

bool UserBalanceData::hasChips() const
{
	for (const UserBalanceData::Cash& cash : cashAssets)
	{
		if (cash.chips)
		{
			return true;
		}
	}

	return false;
}


// in depth treatment of bit-counting problem can be found 
// at http://en.wikipedia.org/wiki/Hamming_weight
UINT32 countBits(UINT64 mask)
{
    UINT32 cnt;
    for (cnt = 0; mask != 0; ++cnt) //lint !e441 // for clause irregularity: loop variable not found in 2nd for expression
        mask &= mask - 1;
    return cnt;
}

//  cardNum = prefix + (truncated or left-padded with 0s 64-bit random) + luhnChar
const char* CardnumGenerator::generateCardNum(PString& cardNum, UINT32 numOfDigits, const char* prefix)
{
	INT32 numOfRandomDigits = numOfDigits - strlen(prefix) - 1;
	if (numOfRandomDigits <= 0 || numOfRandomDigits > 18)
	{
		PLog(LOG_TRACE_FAULTY_LOGIC ": generateCardNum() numOfRandomDigits=%d", numOfRandomDigits);
		PASSERT5(0);
		return (cardNum = "");
	}
	UINT64 rnd64 = makeUINT64(randomizer.next32(), randomizer.next32()); //-V681
	int safetyCounter = 0;
	while (rnd64 > confidenceLimit(numOfRandomDigits) && ++safetyCounter < 10)
	{
		rnd64 = makeUINT64(randomizer.next32(), randomizer.next32()); //-V681
	}
	// create string of truncated or left-padded with 0s rnd64 (no suitable p_.. function)
	char buf[64];
	memset(buf, '0', sizeof(buf)/2); //-V512
	p_u64toa(rnd64, buf + sizeof(buf)/2);
	char* randomStr = buf + strlen(buf) - numOfRandomDigits;

	// first digit should not be zero
	if (!*prefix && randomStr[0] == '0')
	{
		randomStr[0] = '1' + randomizer.next32(9);
	}
	cardNum.assign(prefix).append(randomStr);
	char lastChar = luhnGenerateChar(cardNum); // last char is always for luhn verification
	cardNum.append(lastChar);
	return cardNum.c_str();
}
UINT64 CardnumGenerator::confidenceLimit(UINT32 numOfRandomDigits) const // = ((2^64 / 10^N) * 10^N)
{
	const UINT64 UINT64_MAX_  = 0xFFFFFFFFFFFFFFFF; // 18446744073709551615 - 20 decimal chars
	const UINT64 E06 = 1000000U;
	const UINT64 E07 = 10000000U;
	const UINT64 E08 = 100000000U;
	const UINT64 E09 = 1000000000U;
	const UINT64 E10 = 10000000000U;
	const UINT64 E11 = 100000000000U;
	const UINT64 E12 = 1000000000000U;
	const UINT64 E13 = 10000000000000U;
	const UINT64 E14 = 100000000000000U;
	const UINT64 E15 = 1000000000000000U;
	const UINT64 E16 = 10000000000000000U;
	const UINT64 E17 = 100000000000000000U;
	const UINT64 E18 = 1000000000000000000U;
	const UINT64 E19 = 10000000000000000000U;

	switch (numOfRandomDigits)
	{
	case  6: return UINT64_MAX_ / E06 * E06; // 18446744073709000000
	case  7: return UINT64_MAX_ / E07 * E07; // 18446744073700000000
	case  8: return UINT64_MAX_ / E08 * E08; // 18446744073700000000
	case  9: return UINT64_MAX_ / E09 * E09; // 18446744073000000000
	case 10: return UINT64_MAX_ / E10 * E10; // 18446744070000000000
	case 11: return UINT64_MAX_ / E11 * E11; // 18446744000000000000
	case 12: return UINT64_MAX_ / E12 * E12; // 18446744000000000000
	case 13: return UINT64_MAX_ / E13 * E13; // 18446740000000000000
	case 14: return UINT64_MAX_ / E14 * E14; // 18446700000000000000
	case 15: return UINT64_MAX_ / E15 * E15; // 18446000000000000000
	case 16: return UINT64_MAX_ / E16 * E16; // 18440000000000000000
	case 17: return UINT64_MAX_ / E17 * E17; // 18400000000000000000
	case 18: return UINT64_MAX_ / E18 * E18; // 18000000000000000000
	case 19: return UINT64_MAX_ / E19 * E19; // 10000000000000000000 //-V2007

	default: return UINT64_MAX_;				// 18446744073709551615
	}
}

// PYR-16389
void LeagueData::parse( CommMsgParser& parser )
{
    UINT32 numTournPoints;
    parser
        .parseUINT32(leagueId)
        .parseUINT32(numTournPoints);
    tournPoints.resize(numTournPoints);
    for(UINT32 i=0; i<numTournPoints; ++i)
    {
        parser
            .parseINT64(tournPoints[i].first)
            .parseStringP(tournPoints[i].second);
    }

    UINT32 numRankings;
    parser.parseUINT32(numRankings);
    rankings.resize(numRankings);
    for(UINT32 i=0; i<numRankings; ++i)
    {
        parser.parseStringP(rankings[i]);
    }

    parseUint32VectorOverwrite(parser, results);

    UINT32 numWins;
    parser.parseUINT32(numWins);
    tournWins.resize(numWins);
    for(UINT32 i=0; i<numWins; ++i)
    {
        parser
            .parseINT64(tournWins[i].first)
            .parseStringP(tournWins[i].second);
    }

    parseUint32VectorOverwrite(parser, tourns);

    tournPointsSent = 0;
    rankingsSent = 0;
    resultsSent = 0;
    tournWinsSent = 0;
    tournsSent = 0;
}

// this method changes the state of the object to make sure it's consistent
void LeagueData::compose( CommMsgBody& body )
{
    tournPointsSent = 0;
    rankingsSent = 0;
    resultsSent = 0;
    tournWinsSent = 0;
    tournsSent = 0;
    composePortion(body, tournPoints.size(), rankings.size(), results.size(), tournWins.size(), tourns.size());
}

void LeagueData::composePortion( CommMsgBody& body
                         , UINT32 numTournPoints
                         , UINT32 numRankings
                         , UINT32 numResults
                         , UINT32 numWins
                         , UINT32 numTourns )
const
{
    body.composeUINT32(leagueId);
    body.composeUINT32(numTournPoints);
    for(UINT32 i=0; i<numTournPoints; ++i)
    {
        body
            .composeINT64(tournPoints[tournPointsSent + i].first)
            .composeString(tournPoints[tournPointsSent + i].second);
    }
    body.composeUINT32(numRankings);
    for(UINT32 i=0; i<numRankings; ++i)
    {
        body.composeString(rankings[rankingsSent + i]);
    }
    body.composeUINT32(numResults);
    for(UINT32 i=0; i<numResults; ++i)
    {
        body.composeUINT32(results[resultsSent + i]);
    }
    body.composeUINT32(numWins);
    for(UINT32 i=0; i<numWins; ++i)
    {
        body
            .composeINT64(tournWins[tournWinsSent + i].first)
            .composeString(tournWins[tournWinsSent + i].second);
    }
    body.composeUINT32(numTourns);
    for(UINT32 i=0; i<numTourns; ++i)
    {
        body.composeUINT32(tourns[tournsSent + i]);
    }
}

// copied from cashierobject.cpp
void parseEncryptedData(CommMsgParser& parser, UINT32 algoId, PString& encryptedStr)
{
	const BYTE* encrypted;
	PString s;
	size_t encr_sz;
	parser._parseVarBlock(encrypted, encr_sz);
	if (!encr_sz)
	{
		encryptedStr = "";
		return;
	}
	CommSSLBase64::encode( s, encrypted, encr_sz );

	encryptedStr.assign("1").append( (char)(algoId + '0') ).append(s);
}

vector< PString > parseEncryptedStrings( CommMsgParser& parser )
{
	BYTE algoId;
	UINT32 numCrypted;
	parser.
		parseBYTE(algoId).
		parseUINT32(numCrypted);

	vector< PString > encryptedStrings( numCrypted );

	for(auto&& encryptedStr: encryptedStrings)
	{
		parseEncryptedData( parser, algoId, encryptedStr );
	}
	return encryptedStrings;
}

void parseDecryptedData(CommMsgParser& parser, PString& decryptedStr)
{
	const BYTE* decrypted;
	size_t decr_sz;
	parser._parseVarBlock(decrypted, decr_sz);
	decryptedStr.assign((const char*)decrypted, decr_sz);
}

void composeStringForDecrypt(const PString& encryptedString, CommMsgBody& msgBody)
{
	if (encryptedString.length() <= 2)
	{
//		PTRACE5("!cSFD '%s'", encryptedString.c_str());
		BYTE zero = 0;
		msgBody.composeBYTE(0)._composeVarBlock(&zero, 0);
		return;
	}
	bool encoded = encryptedString[0] != '0';
	BYTE ecryptBYTE = encryptedString[1];
	BYTE algoId = ecryptBYTE - '0';

	if(encoded)
	{
		PBlock msgBlock;
		CommSSLBase64::decode( msgBlock, encryptedString.c_str() + 2 );
		msgBody.composeBYTE(algoId);
		msgBody._composeVarBlock(msgBlock.ptr(), msgBlock.size());
	}
	else
	{
		msgBody.
			composeBYTE(algoId).
			_composeVarBlock((const BYTE*)encryptedString.c_str() + 2, encryptedString.length() - 2);
	}
}

void composeStringForEncrypt(const PString& stringToEncrypt, CommMsgBody& body)
{
	body._composeVarBlock((const BYTE*)stringToEncrypt.c_str(), stringToEncrypt.length());
}

// PYR-18206
const char* sanitizeStringForLogging(const PString &origStr, PString& sanitizedStr, UINT32 maxLength )
{
	sanitizedStr.assign( "" );
	UINT32 origStrLen = origStr.length();
	
	for( int i = 0; i < origStrLen && i < maxLength; ++i )
	{
		char c = origStr.charAt( i );
		if( c > 0 && c < 32 )  
		{
			c = '_';
		}

		sanitizedStr.append( c );
	}

	return sanitizedStr.c_str();
}

// normalizes input dates such as "March 32, 2011" to a proper date such as "April 1, 2011".
// numDays is by default set to 0, and is a number of days to add to "inputDate"
bool normalizeSrvDate(const SrvDate& inputDate, SrvDate& outputDate, INT32 numDays)
{
	SrvTime srvTime;
	srvTime._year = inputDate._year;
	srvTime._month = inputDate._month;
	srvTime._day = inputDate._day;

	struct tm stm;
	srvTime.mk_tm(stm);
	stm.tm_mday += numDays;

	if (mktime(&stm) < 0)	// to normalize the date-time
		return false;

	srvTime.assign(stm);
	outputDate.assign(srvTime);
	return true;
}

// normalizes input times such as "10:59:185" to a proper "in-range" time such as "11:02:05".
// numSeconds is by default set to 0, and is a number of seconds to add to "inputTime"
bool normalizeSrvTime(const SrvTime& inputTime, SrvTime& outputTime, INT32 numSeconds)
{
	struct tm stm;
	inputTime.mk_tm(stm);
	stm.tm_sec += numSeconds;
	if (mktime(&stm) < 0)	// to normalize the time
		return false;

	outputTime.assign(stm);
	return true;
}

// PYR-20357
// This code moved from lobby
UINT32 checkUserIdRules( const char* userId )
{
	UINT32 errCode = 0;
	const int len = strlen(userId);
	if( len && (userId[len-1] == ' ') )
		errCode |= TrailingBlanks; //do not allow trailing blanks in user id - this one because it does not work properly in SQL string comparison for equality
	if( userId[0] == ' ' )
		errCode |= LeadingBlanks; //do not allow leadin blanks either, but not sure why.
	if( strstr(userId, "  ") != 0 )
		errCode |= DoubleSpaces; //do not allow double spaces in user id. It's a business rule.
	for( int k = 0; userId[k]; k++ )
	{
		if( userId[k] < 32 && userId[k] > 0 )
		{
			errCode |= ControlChar;
			break;
		}
	}

	return errCode;
}

// PYR-20549
// Merging 2 32bit values into 1 64 value and viceversa
UINT64 get64From32(UINT32 intValue, UINT32 intValue2) 
{
    UINT64 value64 = intValue2;
    value64 <<= 32;
    value64 |= intValue;
    return value64; 
}

void set32From64(UINT64 val64, UINT32& intValue, UINT32& intValue2)
{
    intValue = (UINT32)(val64 & 0xFFFFFFFF);
    intValue2 = (UINT32)((val64 >> 32) & 0xFFFFFFFF);
}

//PYR-20528
//reads quantifier for specified country from restriction string, e.g. "2:CN;RU-10;"
//for CN will return 1, for RU will return 10 
UINT32 getPlayersLimitForCountry( const PString& restriction, const char * countryCode )
{
	if( countryCode && *countryCode && *( countryCode + 1 ) && !*( countryCode + 2 ) )
	{
		const char *ptr = restriction.findIgnoreCase( countryCode );
		if( ptr )
		{
			if( *( ptr + 2 ) == '-'  ) //quantifier specified
			{
				UINT32 n =  atoi( ptr + 3 );
				if( n )
					return n ;
			}
			return 1; // no quantifier found
		}
	}
	else
	{
		PLog( "PlayerCountryRestriction::playersForCountry() error: invalid country code '%s'", countryCode ? countryCode : "" );
	}
	return 0; //no restriction for the country
}


const char* titleEnumToString(eUserTitle eTitle)
{	// not i18n
	// do NOT use for producing output to user
	switch (eTitle)
	{
	case eUserTitleMr:
		return "Mr";
	case eUserTitleMrs:
		return "Mrs";
	case eUserTitleMiss:
		return "Miss";
	case eUserTitleMs:
		return "Ms";
	case eUserTitleDr:
		return "Dr";
	case eUserTitleNone:
	default:
		return "";
	}
}

void processReopenLogFile(CommMsgParser& inParser, CommMsgBody& outBody, UINT32 reqId/* = 0*/)
{
	if (!reqId)
	{
		inParser.parseUINT32(reqId);
	}

	INT16 errCode = PLogReopen() ? 0 : 1;

	outBody
		.composeUINT32(reqId)
		.composeINT16(errCode);
	if(errCode)
	{
		outBody.composeString("PLogReopen failed");
		PLog("PLogReopen failed");
	}
	else
	{
		PLog("Log file is reopened.");
	}
}

//////// #22570 begin //////


//////// #22570 end ////////

//////// #14804 begin ////////

static PString& appendCharHex(PString& str, char c)
{
	static const char hex[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
	str.append("\\x").append(hex[(c >> 4) & 0x0F]).append(hex[c & 0x0F]);
	return str;
}

const char* toEscapedString(const char* originalStr, PString& safeStr, const char* safeCharSet)
{
	const char* p = originalStr;
	safeStr = "";
	while (*p)
	{
		if (strchr(safeCharSet, *p))
			safeStr.append(*p);
		else 
			appendCharHex(safeStr, *p);
		++p;
	}
	return safeStr;
}

const char* toEscapedString(const char* originalStr, PString& safeStr)
{
	static const char defaultSafeCharSet[] = 
	{
		" +-()./@_*#"
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
	};
	return toEscapedString(originalStr, safeStr, defaultSafeCharSet);
}

//////// #14804 end ////////

//------------------Blitz Tournament--------------------

void BlitzTournUserChips::compose( CommMsgBody &body ) const
{
	body
		.composeString( user )
		.composeINT32( deltaChips );
}

void BlitzTournUserChips::parse( CommMsgParser &parser )
{
	parser
		.parseStringP( user )
		.parseINT32( deltaChips );
}


void BlitzTournChipsUpdateR::add( const char * user, INT32 deltaChips )
{
	data.push_back( BlitzTournUserChips( user, deltaChips ) );
}

void BlitzTournChipsUpdateR::compose( CommMsgBody &body ) const
{
	body
		.composeUINT32( tournId )
		.composeUINT64( handId )
		.composeUINT64( tableId ) // PYR-29205
		.composeINT32( lastOrdinal );
	composeVectorOfComposableValues( data, body );
}

void BlitzTournChipsUpdateR::parse( CommMsgParser & parser )
{
	parser
		.parseUINT32( reqId )
		.parseUINT32( tournId )
		.parseUINT64( handId )
		.parseUINT64( tableId ) // PYR-29205
		.parseUINT32( lastOrdinal );
	parseVectorOfParsableValues( data, parser );
}

	// moved here from requestdbmprepstmt.h after build 998
const char* removeHtmlTags(PString& cleanStr, const char* strWithTags)
{
	cleanStr = "";
	const char* p = strWithTags;
	while (*p)
	{
		char c = *(p++);
		if (c != '<')
		{
			cleanStr.append(c);
			continue;
		}
		while (*p)
		{
			if (*(p++) == '>')
				break;
		}
	}
	return cleanStr;
}

const char* centsToString(PString& out, INT64 amountInCents)
{
	char buf[64];
	if (amountInCents < 0)
	{
		sprintf(buf, "-%lld.%02d", -amountInCents / 100, int(-amountInCents % 100));
	}
	else
	{
		sprintf(buf, "%lld.%02d", amountInCents / 100, int(amountInCents % 100));
	}
	out = buf;
	return out.c_str();
}

int getBinTypeNumber(const PString& binType)
{
	int binTypeN = -1;
	if (binType.equalsIgnoreCase("Electron"))
		binTypeN = 0;
	else if (binType.equalsIgnoreCase("Maestro"))
		binTypeN = 1;
	else if (binType.equalsIgnoreCase("Credit"))
		binTypeN = 2;
	else if (binType.equalsIgnoreCase("Debit"))
		binTypeN = 3;
	else if (binType.equalsIgnoreCase("Unknown"))
		binTypeN = 4;
	return binTypeN;
}

bool isPowerOf2(UINT64 v)
{
	if (v == 0)
		return false;
	return ((v & (v - 1)) == 0);
}

void jsonEscape(const char* utf8String, PString& out)	// PYR-25781
{
	static const char *u0085 = "\xC2\x85";			// utf8 next line
	static const char *u2028 = "\xE2\x80\xA8";		// utf8 line separator
	static const char *u2029 = "\xE2\x80\xA9";		// utf8 paragraph separator

	UINT32 length = strlen(utf8String);
	for (UINT32 i = 0; i < length; ++i) // lint !e850
	{
		unsigned char ch = utf8String[i];
		switch(ch)
		{
			case '"':
				out.append("\\\"");
				break;
			case '\\':
				out.append("\\\\");
				break;
			case '\b':
				out.append("\\b");
				break;
			case '\f':
				out.append("\\f");
				break;
			case '\n':
				out.append("\\n");
				break;
			case '\r':
				out.append("\\r");
				break;
			case '\t':
				out.append("\\t");
				break;
			case '/':
				out.append("\\/");
				break;
			default:
				//Reference: http://www.unicode.org/versions/Unicode5.1.0/
				//C0: \u0000 - \u001F; 
				if (ch <= 0x1F)
				{
					char tmp[32];
					sprintf(tmp, "%04X", ch);	// ch is unsigned, max FF. 
					out.append("\\u");
					out.append(tmp);
				}
				else if (!strncmp(&utf8String[i], u0085, strlen(u0085)))	// utf8 next line
				{
					out.append("\\u0085");
					i += strlen(u0085) - 1;
				}
				else if (!strncmp(&utf8String[i], u2028, strlen(u2028)))	// utf8 line separator
				{
					out.append("\\u2028");
					i += strlen(u2028) - 1;
				}
				else if (!strncmp(&utf8String[i], u2029, strlen(u2029)))	// utf8 paragraph separator
				{
					out.append("\\u2029");
					i += strlen(u2029) - 1;
				}
				else
				{
					out.append(ch);
				}
		}
	}
}

UINT64 prefixedStr2u64(const char* str)	// PYR-23124: moved from NewsServer.cpp
{
	if(!str)
		return 0;

	while(isspace((int)(unsigned char)*str))
		++str;

	if(!*str)
		return 0;

	UINT32 radix = 10;

	if(*str == '0')
	{
		++str;
		radix = 8;
		if(*str == 'x' || *str == 'X')
		{
			++str;
			radix = 16;
		}
	}

	UINT64 result64 = 0;

	if(radix == 8 || radix == 10)
	{
		for( ; *str && *str >= '0' && *str < radix + '0'; ++str)
		{
			result64 = result64 * radix + (*str - '0');
		}
		return result64;
	}

	for( ; *str && (*str >= '0' && *str <= '9' || *str >= 'A' && *str <= 'F' || *str >= 'a' && *str <= 'f'); ++str)
	{
		if(*str >= '0' && *str <= '9')
			result64 = result64 * 16 + (*str - '0');
		else if(*str >= 'A' && *str <= 'F')
			result64 = result64 * 16 + (*str - 'A' + 10);
		else //if(*str >= 'a' && *str <= 'f')
			result64 = result64 * 16 + (*str - 'a' + 10);
	}
	return result64;
}

// Mask... functions were MOVED here from cashiertrans.cpp after BUILD1015
void maskCardNumber( const char* cardNum, PString& cardMasked, int keep)
{
	cardMasked = "";
	for( int n = 0; cardNum[n] ; n++ )
	{ // if contains symbols other than 0..9 or '@' then do not mask
		if( (cardNum[n] > '9' || cardNum[n] < '0') && cardNum[n] != '@' )
		{
			cardMasked = cardNum;		
			return;
		}
	}

	int l = strlen( cardNum );
	for( int i = 0; i < l ; ++i )
	{
		if( i == 0 || i > l - keep )
			cardMasked.append( cardNum[ i ] );
		else
			cardMasked.append( '*' );
	}
}

void maskNumber(const char* number, PString& maskedNumber, int keep)
{
	maskedNumber = "";
	int l = strlen( number );
	for( int i = 0; i < l ; ++i )
	{
		if( i == 0 || i > l - keep )
			maskedNumber.append( number[ i ] );
		else
			maskedNumber.append( '*' );
	}
}

void maskNumber(const char* number, PString& maskedNumber, int keepLeft, int keepRight)
{
	maskedNumber = "";
	int l = strlen( number );
	for( int i = 0; i < l ; ++i )
	{
		if( i < keepLeft || i >= l - keepRight )
			maskedNumber.append( number[ i ] );
		else
			maskedNumber.append( '*' );
	}
}

int daysInMonth(UINT16 year, BYTE month)	//works till 2100
{
	if(month == 4 || month == 6 || month == 9 || month == 11)
		return 30;

	if(month == 2)
		return (year % 4 == 0 ? 29 : 28);

	return 31;
}

// Used internally by p_strtolAdv(), p_strtol(), etc. to check
// various error conditions from strtol() and strtoul().
// May throw PError
// PYR-28774
static void checkGetNextForErrors(bool conversionAdvancesPointer, const char* fnName, const char* endPtr, const char* userStr)
{
	if (errno)
	{
		PString buf;
		buf.append(fnName).append(": overflow parsing '").append(userStr).append("'");
		throw PError(buf);
	}
	else if (endPtr == userStr)
	{
		PString buf;
		buf.append(fnName).append(": no valid number in '").append(userStr).append("'");
		throw PError(buf);
	}
	else if (!conversionAdvancesPointer && *endPtr != '\0')
	{
		PString buf;
		buf.append(fnName).append(": garbage at end of '").append(userStr).append("'");
		throw PError(buf);
	}
}

// PYR-28774
INT32 p_strtolAdv(const char* (&userStr))
{
	errno = 0;

	char* endPtr = NULL;
	long res = strtol(userStr, &endPtr, 0); //-V126

	checkGetNextForErrors(true, "p_strtolAdv", endPtr, userStr);

	userStr = endPtr;
	return res;
}

// PYR-28774
INT32 p_strtol(const char* userStr)
{
	errno = 0;

	char* endPtr = NULL;
	long res = strtol(userStr, &endPtr, 0); //-V126

	checkGetNextForErrors(false, "p_strtol", endPtr, userStr);

	return res;
}

// PYR-28774
UINT32 p_strtoulAdv(const char* (&userStr))
{
	errno = 0;

	char* endPtr = NULL;
	unsigned long res = strtoul(userStr, &endPtr, 0); //-V126

	checkGetNextForErrors(true, "p_strtoulAdv", endPtr, userStr);

	userStr = endPtr;
	return res;
}

// PYR-28774
UINT32 p_strtoul(const char* userStr)
{
	errno = 0;

	char* endPtr = NULL;
	unsigned long res = strtoul(userStr, &endPtr, 0); //-V126

	checkGetNextForErrors(false, "p_strtoul", endPtr, userStr);

	return res;
}


// PYR-33657
int _replaceOnceSubstring(PString& data, int offset, const char* toReplace, const char* replacement)
{
	const char* pos = PString::find(data.c_str() + offset, toReplace);

	if( !pos )
		return -1;

	PString tmp(data.c_str(), pos);
	tmp
		.append(replacement)
		.append( pos + strlen(toReplace) )
		;

	int newOffset  = pos - data.c_str() + strlen(replacement);
	data = tmp;

	return newOffset;
}


void replaceSubstring(PString& data, const char* toReplace, const char* replacement)
{
	int offset = 0;
	while(true)
	{
		offset = _replaceOnceSubstring(data, offset, toReplace, replacement);
		if(offset < 0)
			break;
	}
}

// AmountsInCurrencies was MOVED here from dbmstructhlp.cpp after BUILD1047
void AmountsInCurrencies::addAmount( INT32 amount, const char* currency )
{
	for (size_t i = 0; i < data.size(); ++i)
	{
		if (compareCurrencies(data[i].currency, currency))
		{
			data[i].amount += amount; //-V1026
			return;
		}
	}
	AmountInCurrency amtCur;
	amtCur.amount = amount;
	amtCur.currency = currency[0] == '\0' ? "USD" : currency;
	data.push_back(amtCur);
}

INT32 AmountsInCurrencies::getAmount( const char* currency ) const
{
	for (size_t i = 0; i < data.size(); ++i)
	{
		if (compareCurrencies(data[i].currency, currency))
		{
			return data[i].amount;
		}
	}
	return 0;
}

void AmountsInCurrencies::parse( CommMsgParser& parser )
{
	parseVectorOfParsableValues(data, parser);
}

void AmountsInCurrencies::compose( CommMsgBody& body ) const
{
	composeVectorOfComposableValues(data, body);
}

const char* AmountsInCurrencies::toString(PString& out) const
{
	out.assign("[").appendInt(size()).append(']');
	for (size_t i = 0; i < size(); ++i) 
	{
		if (i != 0)
			out.append(' ');
		out.appendInt(data[i].amount).append(data[i].currency);
	}
	return out.c_str();
}



// PYR-38845
bool isValidCurrencyCode(const char* currencyCode)
{
	if (!currencyCode)
		return false;
	return (strlen(currencyCode) == 3 && isupper(currencyCode[0]) && isupper(currencyCode[1]) && isupper(currencyCode[2]));
}

//does NOT empty vector!!!
UINT32 parseUint64Vector( CommMsgParser& parser, vector<UINT64>& vec )
{
	UINT32 size;
	parser.parseUINT32(size);
	vec.reserve(size + vec.size());
	for (int i = 0; i < size; ++i )
	{
		UINT64 n;
		parser.parseUINT64(n);
		vec.push_back(n);
	}
	return size;
}

//does NOT empty vector!!!
UINT32 parseUint64VectorWithInitSize( CommMsgParser& parser, vector<UINT64>& vec, UINT32 initSize )
{
	UINT32 size;
	parser.parseUINT32(size);

	// PYR-52366
	vec.reserve(min<UINT32>(size, initSize) + vec.size());
	for (int i = 0; i < size; ++i )
	{
		UINT64 n;
		parser.parseUINT64(n);
		vec.push_back(n);
	}
	return size;
}

UINT32 composeUint64Vector( CommMsgBody& body, const vector<UINT64>& vec )
{
	UINT32 size = vec.size();
	body.composeUINT32(size);
	for (int i = 0; i < size; ++i )
	{
		body.composeUINT64(vec[i]);
	}
	return size;
}

//does NOT empty vector!!!
UINT32 parseInt64Vector( CommMsgParser& parser, vector<INT64>& vec )
{
	UINT32 size;
	parser.parseUINT32(size);
	vec.reserve(size + vec.size());
	for (int i = 0; i < size; ++i )
	{
		INT64 n;
		parser.parseINT64(n);
		vec.push_back(n);
	}
	return size;
}

UINT32 composeInt64Vector( CommMsgBody& body, const vector<INT64>& vec )
{
	UINT32 size = vec.size();
	body.composeUINT32(size);
	for (int i = 0; i < size; ++i )
	{
		body.composeINT64(vec[i]);
	}
	return size;
}

bool parseAndGuardDecryptBody(CommMsgParser& parser, const CommServerGuard* guard, CommMsgBody& resultBody)
{
	const BYTE * p;
	size_t size;
	parser._parseVarBlock(p, size);

	if (!guard)
		return false;
	guard->decryptMsg(p, size, resultBody);
	return true;
}

bool parseGuardEncryptedString(CommMsgParser& parser, const CommServerGuard* guard, PString& resultString)
{
	resultString = "";
	CommMsgBody decryptedBody;
	if (!parseAndGuardDecryptBody(parser, guard, decryptedBody))
		return false;
	CommMsgParser decryptedBodyParser(decryptedBody);
	decryptedBodyParser.parseStringP(resultString);
	return true;
}

bool parseGuardEncryptedStrings(CommMsgParser& parser, const CommServerGuard* guard, PString& resultString1, PString& resultString2)
{
	resultString1 = "";
	resultString2 = "";
	CommMsgBody decryptedBody;
	if (!parseAndGuardDecryptBody(parser, guard, decryptedBody))
		return false;
	CommMsgParser decryptedBodyParser(decryptedBody);
	decryptedBodyParser
		.parseStringP(resultString1)
		.parseStringP(resultString2);
	return true;
}

void composeAfterGuardEncryptString(CommMsgBody& msgBody, const CommServerGuard* guard, const PString& str)
{
	PBlock block;
	if (guard)
	{
		CommMsgBody srcBody;
		srcBody.composeString(str);
		guard->encryptMsg(srcBody, block);
	}
	msgBody.composePBlock(block);
}

void composeAfterGuardEncryptStrings(CommMsgBody& msgBody, const CommServerGuard* guard, const PString& str1, const PString& str2)
{
	PBlock block;
	if (guard)
	{
		CommMsgBody srcBody;
		srcBody.composeString(str1).composeString(str2);
		guard->encryptMsg(srcBody, block);
	}
	msgBody.composePBlock(block);
}

// copy of client code function _daysInMonth
static int _daysInMonth( int year, int month )
{
	static const char days[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

	if( 1 <= month && month <= 12 )
	{
		if( month == 2 && ( ( year % 4 ) == 0 ) && ( ( year % 100 ) || ( year % 400 ) == 0 ) )
			return 29;
		else
			return days[ month - 1 ];
	}
	else
	{
		return 0;
	}
}

// copy of client code function _isValidDate
bool isSrvDateValid(const SrvDate& value)
{
	if( 0 < value._year && 0 < value._month && value._month <= 12 && 0 < value._day )
		return value._day <= _daysInMonth( value._year, value._month );
	return false;
}

const char* msgBodyToString(const CommMsgBody& body, PString& str)
{
	str.append("sz=").appendInt(body._size()).append(" {");
	CommMsgParser p(body);
	while (!p.parseEnded())
	{
		BYTE b;
		p.parseBYTE(b);
		char buf[5];
		sprintf(buf, "%02X", b);
		str.append(' ').append(buf);
	}
	str.append('}');
	return str.c_str();
}

const char* removeSpacesFromString(PString& out, const char* str)
{
	out = "";
	for (const char* ptr = str; ptr && *ptr; ++ptr)
	{
		if (!isspace((BYTE)*ptr))
		{
			out.append(*ptr);
		}
	}
	return out.c_str();
}

// Splits a given email address into handle@domain
// Optionally de-alias google emails' handle
bool splitEmail(const char* email, PString& handle, PString& domain, bool deAliasHandle)
{
	handle.clear();
	domain.clear();

	// Separate handler and domain
	PString normEmail(email);
	const char* pCharInEmail = normEmail.find('@');
	if (!pCharInEmail)
	{
		return false;
	}
	PString emailHandle(normEmail, pCharInEmail);
	domain.assign(++pCharInEmail);

	if (deAliasHandle && (domain.startsWith("gmail.") || domain.startsWith("googlemail.")))
	{
		// Remove alias variations in handle with google domains
		if ( (pCharInEmail = emailHandle.find('+')) != NULL )
		{
			emailHandle = PString(emailHandle, pCharInEmail);
		}

		// Remove '.'
		for (const char* p = emailHandle.c_str(); p && *p; ++p)
		{
			if (*p != '.')
			{
				handle.append(*p);
			}
		}
	}
	else
	{
		handle = emailHandle;
	}
	return true;
}

// @return true if all characters are digits, for empty string returns true as well
bool containsDigitsOnly(const char* str)
{
	while (*str)
	{
		if (!isdigit(*str))
			return false;
		str++;
	}
	return true;
}

const char* verticalToString1(eGameVerticalFlag vertical)
{
	switch (vertical)
	{
	case eGameVerticalFlagPoker:
		return "P";
	case eGameVerticalFlagCasino:
		return "C";
	case eGameVerticalFlagSportsbook:
		return "S";
	case eGameVerticalFlagNone:
	default:
		return "";
	}
}

const char* appendWithDelimiter(PString& target, const char* addition, char delimiter /*= ','*/)
{
	if (target.length() > 0)
		target.append(delimiter);
	target.append(addition);
	return target.c_str();
}

void PDumpEndLine(const PString& str)
{
	if ((str.length() == 0) || (str[str.length() - 1] != '\n') )
	{
		// For the function to print logs correctly in a multithreaded module, it has to update the string before calling PDump
		PString strEL(str);
		strEL.append('\n');
		PDump(strEL);
	}
	else
	{
		PDump(str);
	}
}

const char* cutStrAtDelimiter(PString& str, const PString& delimiter)
{
	const char* truncate = str.findIgnoreCase(delimiter);
	if (truncate)
	{
		str = PString(str.c_str(), truncate);
	}
	return str.c_str();
}