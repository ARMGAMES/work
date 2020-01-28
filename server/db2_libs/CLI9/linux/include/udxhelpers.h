/**
 * Datatype Helper API for User Defined Functions and Aggregates.
 * (C) Copyright IBM Corp. 2007, 2011  All Rights Reserved.
 *
 * Contains an implementation of the Fliegel and van Flandern algorithm:
 * Fliegel, H. F. and van Flandern, T. C. (1968). Communications of the ACM, 
 * Vol. 11, No. 10 (October, 1968). 
 * Also available online at various locations. 
 * See http://portal.acm.org/citation.cfm?id=364096.364097&dl=GUIDE&dl=ACM
 */

#include <time.h>
#include <math.h>
#include <string.h>
#include <sys/time.h>

#ifndef UDX_HELPERS_INCLUDED
#define UDX_HELPERS_INCLUDED

#ifdef _DIAB_TOOL
#   define _INT64(a) a##LL;
#   define _UINT64(a) a##ULL;
#elif _WIN32
#   define _INT64(a) a##i64;
#   define _UINT64(a) a##ui64;
#elif __GNUC__
#   define _INT64(a) a##LL;
#   define _UINT64(a) a##ULL;
#elif __xlC__
#   define _INT64(a) a##LL;
#   define _UINT64(a) a##ULL;
#elif __SUNPRO_CC
#   define _INT64(a) a##LL;
#   define _UINT64(a) a##ULL;
#elif __HP_aCC
#   define _INT64(a) a##LL;
#   define _UINT64(a) a##ULL;
#else
#   error Definition of _INT64 macro failed due to uknown compiler
#endif

#ifndef NULL
#define NULL 0;
#endif

//NOTE: This is also declared in nz_utf8.h, geninl.h
//This is also defined in udxharness.cpp, nz_utf8.cpp
extern uchar utf8Lengths[];
//\NOTE

#ifndef utf8len
//NOTE: This definition is copied in nz_utf8.h, udharness.cpp and geninl.h
#define utf8len(s) (utf8Lengths[(*((const uchar *)s)) >> 4])
//\NOTE
#endif

//NOTE: This is also declared in geninl.h, nz_utf8.h
//This is also defined in udxharness.cpp, nz_utf8.cpp
extern "C" bool isValidUtf8 (const uchar* bytes, int length, int* charLength =NULL);
//\NOTE

namespace nz
{
namespace udx_ver2
{
    
/**
 * UTF8 Helpers
 * 
 * Two routines to help handing NCHAR and NVARCHAR datatypes. A quick 
 * character-counter, and a longer, more expensive full UTF8 validator and 
 * character counter.
 */
namespace utf8helpers
{
    /**
     * Returns the quick UTF8 character count of a string.
     * This routine will return a value for an invalid utf8 string.
     * argument bytes: the string, not null-terminated
     * argument length: the number of bytes to look at
     * returns: number of utf8 characters. Indeterminate value if bytes is not 
     *    a valid utf8 string.
     * throws: error (opaque exception object) if length <0 or if bytes is NULL
     * 
     * NOTE: This is a replica of internal utf8CharCount
     */
    inline int UTF8CharCount(const char* bytes, int length)
    {
        if(bytes==NULL)
            throwError(NZ_ERROR_ILLVAL, "UTF8CharCount: null string given", 0);
        if(length<0)
            throwError(NZ_ERROR_ILLVAL, "UTF8CharCount: negative length", 0);
        
        const uchar* str = (uchar*) bytes;
        int clen = 0;
        for (const uchar *send = str + length; str < send ;clen++)
        {
            str += utf8len (str);
        }
        return clen;
    }
   
    /**
    * Checks if a given string represents valid UTF8 characters. 
    * Returns number of characters if needed.
    * argument bytes: the string to check, not null-terminated
    * argument length: the number of chars in bytes to look at
    * argument charLength: optional. If present and bytes is valid, charLength 
    *   is set to number of UTF8characters in bytes. If present and bytes is 
    *   not valid, charLength is set to -1
    * returns: true if length is 0 or bytes[0...length-1] is a valid UTF8 
    *   string. false otherwise
    * throws: error (opaque exception object) if bytes is NULL or if length < 0
    */
   inline bool isValidUTF8(const char* bytes, int length, int* charLength= NULL)
   {
       if(charLength)
           return ::isValidUtf8( (uchar*) &bytes[0], length, charLength);
       return ::isValidUtf8( (uchar*) &bytes[0], length, NULL);
   }
};

/**
 * Date-Time Helpers
 * 
 * Functions and constants provided to facilitate efficient and easy 
 * conversion between internal Netezza temporal formats and common
 * teporal formats.
 * 
 * Behavior with respect to errors: an error occurs if a function 
 * is passed a errorFlag parameter and the parameter is true 
 * after the function call, or if the function throws an object.
 * 
 * Unless otherwise noted, if the function writes to one or 
 * more variables and an error occurs, the variables written to 
 * will contain an indeterminate value. Similarly, if a function
 * returns a value and a non-throw error occurs, the value 
 * returned is also indeterminate.
 */   
namespace dthelpers
{
   // RANGE SPECIFIERS
   /**
    * The minimum value that the Netezza Encoded Date can have.
    */
   static const int32 ENC_DATE_MIN=-730119;
   
   /**
    * The maximum value that the Netezza Encoded Date can have.
    */
   static const int32 ENC_DATE_MAX=2921939;
   
   /**
    * The minimum value that the Netezza Encoded Time can have.
    */            
   static const int64 ENC_TIME_MIN=_INT64(0);
   
   /**
    * The maximum value that the Netezza Encoded Time can have.
    */
   static const int64 ENC_TIME_MAX=_INT64(86399999999);
   
   /**
    * The minimum value that the Netezza Encoded TimeTZ Offset part can have.
    */
   static const int32 ENC_TIMETZ_OFFSET_MIN=-46800;
   
   /**
    * The maximum value that the Netezza Encoded TimeTZ Offset part can have. 
    */
   static const int32 ENC_TIMETZ_OFFSET_MAX=46740;
   
   /**
    * The minimum value that the Netezza Encoded Timestamp can have. 
    */
   static const int64 ENC_TIMESTAMP_MIN=_INT64(-63082281600000000);
   
   /**
    * The maximum value that the Netezza Encoded Timestamp can have. 
    */
   static const int64 ENC_TIMESTAMP_MAX=_INT64(252455615999999999);
   
   /**
    * The minimum value that the Netezza Encoded Interval Month part can 
    * have. 
    */
   static const int32 ENC_INTERVAL_MONTH_MIN=-3000000;
   
   /**
    * The maximum value that the Netezza Encoded Interval Month part can 
    * have. 
    */
   static const int32 ENC_INTERVAL_MONTH_MAX=3000000;
   
   /**
    * The minimum value that the Decoded Year can have. 
    * NOTE: Does not apply to the 'years' value of Interval. 
    */
   static const uint16 SQL_YEAR_MIN=1;
   
   /**
    * The maximum value that the Decoded Year can have. 
    * NOTE: Does not apply to the 'years' value of Interval. 
    */
   static const uint16 SQL_YEAR_MAX=9999;
   
   /**
    * The minimum value that the Decoded Time Zone Offset part can have. 
    */
   static const int16 SQL_OFFSET_MIN=-779;
   
   /**
    * The maximum value that the Decoded Time Zone Offset part can have. 
    */
   static const int16 SQL_OFFSET_MAX=780;
   
   /**
    * The start of the time_t Epoch (1/1/1970) represented as an NZ Date. 
    */
   static const int32 EPOCH_START_AS_DATE=-10957;
   
   /**
    * The end of the time_t Epoch (1/19/2038) represented as an NZ Date. 
    */
   static const int32 EPOCH_END_AS_DATE=13898;
   
   /**
    * The start of the time_t Epoch (00:00:00.000000, 1/19/1970) represented 
    *   as an NZ Timestamp. 
    */
   static const int64 EPOCH_START_AS_TIMESTAMP=_INT64(-946684800000000); 
   
   /**
    * The end of the time_t Epoch (03:14:07.999999, 1/19/2038) represented 
    * as an NZ Timestamp. Note the additional 999,999 microseconds.
    */
   static const int64 EPOCH_END_AS_TIMESTAMP=_INT64(1200798847999999);

   //MISCELLANEOUS CONSTANTS
   static const int32 SECONDS_PER_DAY=86400;
   static const int64 MICROS_PER_SECOND=1000000;
   static const int64 MICROS_PER_DAY=(int64)SECONDS_PER_DAY * MICROS_PER_SECOND;
   static const int64 MICROS_PER_MINUTE=60* MICROS_PER_SECOND;
   
   /*
    * Correction to the Fliegel and van Flandern algorithm. This correction is
    * necessary because the Fliegel and van Flandern algorithm is centered 
    * around some ~4000 BC, but NPS dates are centered around 1/1/2000. 
    */    
   static const int32 SQL_FLIEGEL_CORRECTION=2451545;
   
   /**
    * Ignore Buffer
    * 
    * Provided so that API functions can be called without allocating storage 
    * for data that's not needed.
    * 
    * For instance, to decode NZ time to h:m:s only and ignore microseconds:
    *   uint8 h,m,s;
    *   IgnoreBuffer ignore;
    *   decodeTime(nzTime,&h,&m,&s,&ignore.u32);
    * 
    * Similarly, to decode date to day and month only:
    *   uint8 month,day;
    *   IgnoreBuffer ignore;
    *   decodeDate(nzDate,&month,&day,&ignore.u16);
    * 
    * Some API functions use the buffer in this fashion. Naturally, the buffer
    * is not meant to contain any kind of meaningful data at any time. 
    */
   union IgnoreBuffer
   {
       uint8 u8;
       uint16 u16;
       uint32 u32;
       uint64 u64;
       int8 s8;
       int16 s16;
       int32 s32;
       int64 s64;
   };
   
   /**
     * Commonly used error messages.
     */   
    static const char* ERROR_MESSAGES[] = {"null pointer","invalid data",
                                     "generic error"};

    enum ErrorIndex
    {
        DT_ERROR_NULL_POINTER=0,
        DT_ERROR_INVALID_DATA=1,
        DT_ERROR_GENERIC=2
    };
   
   //FUNCTION DECLARATIONS
   bool isLeapYear(uint32);
   bool isAnyNull(const void*);
   bool isAnyNull(const void*,const void*);
   bool isAnyNull(const void*,const void*,const void*);
   bool isAnyNull(const void*,const void*,const void*,const void*);
   bool errorCheck(bool,ErrorIndex,bool* =NULL);
   uint8 numDaysInMonth(uint32,uint32,bool* =NULL);
   uint32 getYearDay(uint32,uint32,uint32,bool* =NULL);   
   bool isValidDate(uint32,uint32,uint32);
   bool isValidEpochDate(int32);
   bool isValidTime(int64);
   bool isValidTimeTzOffset(int32);
   bool isValidTimeTz(int64,int32);
   bool isValidTimestamp(int64);
   bool isValidEpochTimestamp(int64);
   bool isValidInterval(int32,int64);
   bool isValidDate(uint32,uint32,uint32);
   bool isValidTime(uint32,uint32,uint32,uint32);
   bool isValidTimeTz(uint32,uint32,uint32,uint32,int32);
   bool isValidTimestamp(uint32,uint32,uint32,uint32,uint32,uint32,uint32);
   bool isValidEpoch(int32);
   bool isValidTimeValUsecs(int32 usecs);
   bool isValidTimeVal(const struct timeval&);
   bool isValidTimeStruct(const struct tm&);
   bool isValidSqlMonth(uint32);
   bool isValidSqlYear(uint32);
   bool isValidSqlOffset(int32);
   int64 offsetTimestamp(int64,int32,bool* =NULL);   
   int64 offsetTime(int64,int32,bool* =NULL);
   int32 offsetEpoch(int32,int32,bool* =NULL);    
   struct ::tm offsetTimeStruct(const struct tm&,int32,bool* =NULL);
   void decodeDate(int32, uint8*, uint8*, uint16*, bool* =NULL);
   void decodeDate(int32,int32*,bool* =NULL);
   void decodeDate(int32,struct tm*,bool* =NULL);
   void decodeTime(int64,uint8*,uint8*,uint8*,uint32*, bool* =NULL);
   void decodeTimeTz(int64,int32,uint8*,uint8*,uint8*,uint32*,int16*, 
                     bool* =NULL);
   void decodeTimestamp(int64,uint8*,uint8*,uint16*,uint8*,uint8*,uint8*, 
                        uint32*,bool* =NULL);
   void decodeTimestamp(int64,int32*,bool* =NULL);
   void decodeTimestamp(int64,struct tm*,bool* =NULL);
   void decodeTimestamp(int64,struct timeval*,bool* =NULL);
   void encodeDate(uint32,uint32,uint32,int32*,bool* =NULL);
   void encodeDate(int32,int32*,bool* =NULL);
   void encodeDate(const struct tm&,int32*,bool* =NULL);
   void encodeTime(uint32,uint32,uint32,uint32,int64*,bool* =NULL);
   void encodeTimeTz(uint32,uint32,uint32,uint32,int32,int64*,int32*,
                     bool* =NULL);
   void encodeTimestamp(uint32,uint32,uint32,uint32,uint32,uint32,uint32, 
                        int64*,bool* =NULL);
   void encodeTimestamp(int32,int64*,bool* =NULL);
   void encodeTimestamp(const struct timeval&,int64*,bool* =NULL);
   void encodeTimestamp(const struct tm&,int64*,bool* =NULL);
   
   //PRIVATE FUNCTIONS
   class PrivateFunctions
   {
   public:
       //all the callers
       friend bool isValidTimeStruct(const struct tm&);
       friend void decodeDate(int32 encodedDate, struct tm* result, 
                             bool* errorFlag);
       friend void decodeTimestamp(int64 encodedTimestamp, uint8* month, 
                                  uint8* day, uint16* year, uint8* hour, 
                                  uint8* minute, uint8* second, uint32* mcrs, 
                                  bool* errorFlag);
       friend void decodeTimestamp(int64 encodedTimestamp, struct tm* result,
               bool* errorFlag);
       friend void encodeTimestamp(const struct tm& ts, 
                                   int64* encodedTimestamp, bool* errorFlag);
       friend void encodeDate(const struct tm& date, int32* encodedDate, 
                                   bool* errorFlag);
       friend struct ::tm offsetTimeStruct(const struct tm&, int32, bool*);
   private:
       PrivateFunctions() {}
       ~PrivateFunctions() {}
       
       /**
      * Not intended for use outside of the API.
      */
       inline static int32 nzDateFromNzTimestamp(int64 encodedTimestamp)
       {
           return ((int32) (encodedTimestamp / ((int64)MICROS_PER_DAY)));
       }
       
       /**
        * Not intended for use outside of the API.
        */
       inline static int64 nzTimeFromNzTimestamp(int64 encodedTimestamp)
       {
           int64 val = encodedTimestamp % ((int64)MICROS_PER_DAY);
           //Negatives are handled as special casse.
           //We convert times like (- 3 hours) to (+21 hours) and
           //decrement the day counter. the decrement happens in
           //decodeTimestamp(int64,uint8*,uint8*,uint16*,uint8*,...
           if(val<0)
               val=val +((int64)MICROS_PER_DAY);
           return val;
       }
       
       /**
        * Not intended for use outside of the API.
        */
       inline static void initializeTimeStruct(struct tm* x)
       {
           memset(x,0,sizeof(struct tm)); //prevents garbage when casting from
           //uint8/uint16 to int32. Otherwise the field gets extra random 
           //bits from memory that could change the value.
       }
       
       /**
        * Not intended for use outside of the API.
        */
       inline static void formatTimeStruct(struct tm* x, int32 nzDate)
       {
           x->tm_yday=(int32) (getYearDay((uint32)x->tm_mon, 
                                          (uint32)x->tm_mday,
                                          (uint32)x->tm_year));
           x->tm_mon=x->tm_mon-1;
           x->tm_year=x->tm_year-1900;
           x->tm_wday=(int32) ((nzDate-ENC_DATE_MIN+1)%7);
           x->tm_isdst=-1;
       }
       
       /**
        * Not intended for use outside of the API.
        */       
       inline static bool hasGoodDateLowerBounds(const struct tm& x)
       {
           return x.tm_mday>=1 && x.tm_mon>=0 && 
               x.tm_year+1900>=SQL_YEAR_MIN;
       }
       
       /**
        * Not intended for use outside of the API.
        */
       inline static bool hasGoodTimeLowerBounds(const struct tm& x)
       {
           return x.tm_hour>=0 && x.tm_min>=0 && x.tm_sec>=0;
       }
   };
   
   //MISCELLANEOUS FUNCTIONS
   //SEE ALSO "OFFSET FUNCTIONS" BELOW.   
   /**
    * Tells whether or not a given year is a leap year.
    * argument year: the year number.
    * returns: true if year%4 is 0 and (year%100 is not 0 
    *   or (year%100 is 0 and year%400 is 0). false otherwise.
    */
   inline bool isLeapYear(uint32 year)
   {
       return year%4==0 && (year%100!=0 || year%400==0); 
   }
   
   /**
    * Find out if a pointer is NULL.
    * returns: true if any argument is null. false otherwise.
    */
   inline bool isAnyNull(const void* a)
   {
       return a==NULL;
   }
   
   inline bool isAnyNull(const void* a, const void* b)
   {
       return a==NULL || b==NULL;
   }
   
   inline bool isAnyNull(const void* a, const void* b, const void* c)
   {
       return a==NULL || b==NULL || c==NULL;
   }
   
   inline bool isAnyNull(const void* a, const void* b, const void* c, 
                         const void* d)
   {
       return isAnyNull(a,c) || isAnyNull(b,d);
   }
   
   /**
    * Check a boolean value and throw, if error. An error
    * is defined as expr being true. If errorFlag is provided, 
    * sets errorFlag to expr and returns expr. If errorFlag is not provided, 
    * throws an exception if expr is true; returns false if if expr is false.
    * argument expr: the boolean value to check
    * argument message: the number of the message (ERROR_MESSAGES[message])
    *   to include into the error, in case of one.
    * argument errorFlag: optional, if not null, set to expr.
    * throws: error (opaque exception object) if expr is true and errorFlag is 
    *   NULL.
    * returns: true if expr is true and errorFlag is not NULL; 
    *   false if expr is false.
    */    
   inline bool errorCheck(bool expr, ErrorIndex message, bool* errorFlag)
   {
       if(errorFlag!=NULL) {
           *errorFlag=expr;
           return expr;
       }
       if(expr)
           throwError(NZ_ERROR_ILLVAL, ERROR_MESSAGES[message], 0);
       return false;
   }
   
   /**
    * Find out the total number of days in a given month.
    * argument month: the month number, 1 to 12.
    * argument year: the year number, 1 to 9999 Used to find out the correct 
    * number of days if month is February.
    * argument errorFlag: optional, if not NULL, set to true if 
    *   isValidSqlMonth(month) is false or isValidSqlYear(year) is false; 
    *   set to false otherwise.
    * returns: 30 if month in (4, 6, 9, 11), 
    *   31 if month in (1, 3, 5, 7, 8, 10, 12), 
    *   28 if month is 2 and isLeapYear(year), 
    *   29 if month is 2 and !isLeapYear(year).
    *   0 if errorFlag is not NULL 
    *   and (isValidYearNumber(year) is FALSE or 
    *   isValidMonthNumber(month) is FALSE) 
    * throws: error (opaque exception object) if errorFlag is NULL 
    *   and ((isValidYearNumber(year) is FALSE 
    *   or isValidMonthNumber(month) is FALSE)). 
    */
   inline uint8 numDaysInMonth(uint32 month, uint32 year, 
                               bool* errorFlag)
   {
       if (errorCheck(!isValidSqlMonth(month), DT_ERROR_INVALID_DATA, errorFlag)
         ||errorCheck(!isValidSqlYear(year),  DT_ERROR_INVALID_DATA, errorFlag))
           return 0;
       if(month==4 || month==6 || month==9 || month==11)
           return 30;
       else if(month==2) {
           if(isLeapYear(year))
               return 29;
           return 28;
       }
       return 31;
   }
   
   /**
    * Structure used to assist the getYearDay method. Stores somewhat of a 
    * day-offset for a month.
    * For instance, for January, 23rd, monthEndings[0]=0 so january 23rd 
    * is day 22 (start counting at 0) of the year. For March the 15th, 
    * of a non-leap year monthEndings[2]=59, so 3/15 is day 14+59 of the year.
    */
   static const int16 monthEndings[12] = {0,31,59,90,120,151,181,212,243,273,
                                          304,334};
   
   /**
    * Given a m/d/y format date, returns the day number of the year.
    * argument month: month number 1 to 12
    * argument year: year number, i.e. 2005
    * argument day: day of month, 1 to 31.
    * argument errorFlag: optional, if not NULL, set to true if 
    *   isValidDate(month,day,year) is false. set to false otherwise.
    * returns: d [0,364] for non leap years and [0,365] for leap years, 
    *   0 if isValidDate(month,day,year) is false and errorFlag is not NULL.
    * throws: error (opaque exception object) if isValidDate(month,day,year) 
    *   is false and errorFlag is NULL.
    */
   inline uint32 getYearDay(uint32 month, uint32 day, uint32 year, 
                              bool* errorFlag) //errorFlag is truly optional
                                               //(see declaration above)
   {
       if(errorCheck(!isValidDate(month,day,year),  DT_ERROR_INVALID_DATA, 
                     errorFlag))
           return 0;  
       if(isLeapYear(year) && month>=3)
           return monthEndings[month-1]+day;
       return monthEndings[month-1]+day-1;
   } 
   
   //ENCODED RANGE CHECKERS
   /**
    * Tells whether or not a Netezza encoded Date value is valid and within
    * the Netezza Date range.
    * argument encodedDate: a Date encoded in NZ Format.
    * returns: false if encodedDate<ENC_DATE_MIN or encodedDate>ENC_DATE_MAX. 
    *   true otherwise.
    */
   inline bool isValidDate(int32 encodedDate)
   {
       return ENC_DATE_MIN<=encodedDate && encodedDate<=ENC_DATE_MAX ;
   }
   
   /**
    * Tells whether or not a Netezza encoded Date value is valid and within 
    * the time_t Epoch range. 
    * argument encodedDate: a Date encoded in NZ Format.
    * returns: false if encodedDate<EPOCH_START_AS_DATE or 
    *   encodedDate>EPOCH_END_AS_DATE. true otherwise.
    */
   inline bool isValidEpochDate(int32 encodedDate)
   {
       return EPOCH_START_AS_DATE<=encodedDate && 
           encodedDate<=EPOCH_END_AS_DATE;
   }
   
   /**
    * Tells whether or not a Netezza encoded Time value is valid and within 
    * range.
    * argument encodedTime: a Time encoded in NZ format.
    * returns: false if encodedTime<ENC_TIME_MIN, or encodedTime>ENC_TIME_MAX.
    *   true otherwise.
    */
   inline bool isValidTime(int64 encodedTime)
   {
       return ENC_TIME_MIN<=encodedTime && encodedTime<=ENC_TIME_MAX;
   }
   
   /**
    * Tells whether or not a the offset part of a Netezza encoded TimeTz is 
    * valid and within range.
    * argument encodedZone: a Time Offset encoded in NZ format.
    * returns: false if encodedZone>ENC_TIMETZ_OFFSET_MAX, or 
    *   encodedZone<ENC_TIMETZ_OFFSET_MIN, or encodedZone%60 is not 0. 
    *   true otherwise.
    */
   inline bool isValidTimeTzOffset(int32 encodedZone)
   {
       return encodedZone%60==0 && ENC_TIMETZ_OFFSET_MIN<=encodedZone &&
           encodedZone<=ENC_TIMETZ_OFFSET_MAX; 
   }
   
   /**
    * Tells whether or not a Netezza encoded TimeTZ value is valid and within 
    * range.
    * argument encodedTime: a Time encoded in NZ format.
    * argument encodedZone: a Time Offset encoded in NZ format.
    * returns: false if isValidTime(encodedTime) is false, or 
    *   isValidTimeTzOffset(encodedZone) is false. true otherwise.
    */            
   inline bool isValidTimeTz(int64 encodedTime, int32 encodedZone)
   {
       return isValidTime(encodedTime) && isValidTimeTzOffset(encodedZone);
   }
   
   /**
    * Tells whether or not a Netezza encoded Timestamp value is valid and 
    * within range.
    * argument encodedTimestamp: a Timestamp encoded in NZ Format.
    * returns: false if encodedTimestamp<ENC_TIMESTAMP_MIN or 
    *   encodedTimestamp>ENC_TIMESTAMP_MAX. true otherwise.
    */
   inline bool isValidTimestamp(int64 encodedTimestamp)
   {
       return ENC_TIMESTAMP_MIN<=encodedTimestamp && 
           encodedTimestamp<=ENC_TIMESTAMP_MAX; 
   }
   
   /**
    * Tells whether or not a Netezza encoded Timestamp value is valid and 
    * within the time_t Epoch range.
    * argument encodedTimestamp: a Timestamp encoded in NZ Format.
    * returns: false if encodedTimestamp<EPOCH_START_AS_TIMESTAMP or 
    *   encodedTimestamp>EPOCH_END_AS_TIMESTAMP. true otherwise.
    */
   inline bool isValidEpochTimestamp(int64 encodedTimestamp)
   {
       return EPOCH_START_AS_TIMESTAMP<=encodedTimestamp &&
           encodedTimestamp<=EPOCH_END_AS_TIMESTAMP;
   }
   
   /**
    * Tells whether or not a Netezza encoded Interval value is valid and 
    * within range.
    * argument intervalMonth: the Month part of an Interval encoded in NZ 
    *   Format.
    * argument intervalTime: the Time part of an Interval encoded in NZ 
    *   Format.
    * returns: false if intervalMonth<ENC_INTERVAL_MONTH_MIN or 
    *   intervalMonth>ENC_INTERVAL_MONTH_MAX. true otherwise.
    */
   inline bool isValidInterval(int32 intervalMonth, int64 intervalTime)
   {
       return ENC_INTERVAL_MONTH_MIN<=intervalMonth&& 
           intervalMonth<=ENC_INTERVAL_MONTH_MAX;
   }

   //DECODED RANGE CHECKERS
   /**
    * Tells if a month number is valid. 
    * argument month: the month number, 1 to 12.
    * returns: true if month>=1 && month<=12. false otherwise.
    */
   inline bool isValidSqlMonth(uint32  month)
   {
       return 1<=month && month<=12;
   }
   
   /**
    * Tells if a year number is valid. 
    * argument year: the year number, i.e. 1999
    * returns: true if year>=SQL_YEAR_MIN && year<=SQL_YEAR_MAX. 
    *   false otherwise.
    */
   inline bool isValidSqlYear(uint32 year)
   {
       return SQL_YEAR_MIN<=year && year<=SQL_YEAR_MAX;
   }
   
   /**
    * Tells whether or not a decoded m/d/y Date value is valid and within the 
    * Netezza Date range.
    * argument month: the month, 1 to 12 inclusive.
    * argument day: the day, 1 to 31 inclusive.
    * argument year: the year of the date, SQL_YEAR_MIN to SQL_YEAR_MAX 
    *   inclusive.
    * returns: false if (month>12 or month<1) or (day<1 or day>31) or 
    *   (year<SQL_YEAR_MIN or year>SQL_YEAR_MAX) or 
    *   (month is in (4, 6, 9, 11) and day>30) or (isLeapYear(year) and 
    *   month=2 and day>29) or (!isLeapYear(year) and month=2 and day>28). 
    *   true otherwise.
    */
   inline bool isValidDate(uint32 month, uint32 day, uint32 year)
   {
       return isValidSqlMonth(month) && isValidSqlYear(year) && 1<=day && 
              day<=numDaysInMonth(month, year);
   }
   
   /**
    * Tells whether or not a decoded h:m:s:micros Time value is valid and 
    * within the Netezza Time range.
    * argument hour: the hour, 0 to 23 inclusive.
    * argument minute: the minute, 0 to 59 inclusive.
    * argument second: the second, 0 to 59 inclusive.
    * argument mcrs: the microsecond, 0 to 999,999 inclusive.
    * returns: false if hour>23 or minute>59 or second>59 or micros>999,999. 
    *   true otherwise.
    */
   inline bool isValidTime(uint32 hour, uint32 minute, uint32 second, 
                           uint32 mcrs)
   {
       return hour<=23 && minute<=59 && second<=59 && mcrs<=999999;
   }
   
   /**
    * Tells whether or not a time offset value is within the valid API range.
    * argument offset: offset, in minutes, SQL_OFFSET_MIN to SQL_OFFSET_MAX 
    *   inclusive.
    * returns: false if offset>SQL_OFFSET_MAX or offset<SQL_OFFSET_MIN. 
    *   true otherwise.
    */
   inline bool isValidSqlOffset(int32 offset)
   {
       return  SQL_OFFSET_MIN<=offset && offset<=SQL_OFFSET_MAX ;
   }
   
   /**
    * Tells whether or not a decoded h:m:s:micros +offset TimeTZ value is 
    * valid and within the Netezza TimeTZ range.
    * argument hour: the hour, 0 to 23 inclusive.
    * argument minute: the minute, 0 to 59 inclusive.
    * argument second: the second, 0 to 59 inclusive.
    * argument mcrs: the microsecond, 0 to 999,999 inclusive.
    * argument sqlOffset: the time zone offset, in minutes, SQL_OFFSET_MIN 
    *   to SQL_OFFSET_MAX
    * returns: false if isValidTime(hour, minute, second, mcrs) is false or 
    *   isValidSqlOffset(sqlOffset) is false. true otherwise.
    */
   inline bool isValidTimeTz(uint32 hour, uint32 minute, uint32 second, 
                             uint32 mcrs, int32 sqlOffset)
   {
       return isValidTime(hour,minute,second,mcrs) && 
              isValidSqlOffset(sqlOffset);
   }
   
   /**
    * Tells whether or not a decoded m/d/y, h:m:s:micros Timestamp value is
    * valid and within the Netezza Timestamp range.
    * argument month: the month, 1 to 12 inclusive.
    * argument day: the day, 1 to 31 inclusive.
    * argument year: the year, SQL_YEAR_MIN to SQL_YEAR_MAX inclusive.
    * argument hour: the hour, 0 to 23 inclusive.
    * argument minute: the minute, 0 to 59 inclusive.
    * argument second: the second, 0 to 59 inclusive.
    * argument mcrs: the microsecond, 0 to 999,999 inclusive.
    * returns: false if isValidDate(month, day, year) is false or 
    *   isValidTime(hour, minute, second, micros) is false. true otherwise.
    */
   inline bool isValidTimestamp(uint32 month, uint32 day, uint32 year, 
                                uint32 hour, uint32 minute, uint32 second, 
                                uint32 mcrs)
   {
       return isValidTime(hour,minute,second,mcrs) && 
              isValidDate(month,day,year);
   }
   
   /**
    * Tells whether or not a decoded time_t value is valid and can be decoded 
    * to NZ Timestamp or Date.
    * argument time: a c++ time_t value. Forced to be a signed int32.
    * returns: false if time<0. true otherwise.
    */
   inline bool isValidEpoch(int32 time)
   {
       return time>=0;
   }
   
   /**
    * Tells whether or not the microsecond part of a timeval structure is 
    * valid.
    * argument usecs: the tv_usec part of a struct timeval datum.
    * returns: false if usecs<0 or usecs>999,999. true otherwise.
    */   
   inline bool isValidTimeValUsecs(int32 usecs)
   {
       return 0<=usecs && usecs<=999999;
   }
   
   /**
    * Tells whether or not a struct timeval datum is valid and can be encoded
    * into an NZ Timestamp.
    * argument tv: the datum to check
    * returns: false if isValidEpoch(tv.tv_sec) is false or 
    *   isValidTimeValUsecs(tv.tv_usec) is false. true otherwise.
    * throws: error (opaque exception object) if &tv is NULL.
    */   
   inline bool isValidTimeVal(const struct timeval& tv)
   {
       errorCheck(isAnyNull(&tv),DT_ERROR_NULL_POINTER,NULL);
       return isValidEpoch(tv.tv_sec)&&isValidTimeValUsecs(tv.tv_usec);
   }
   
   /**
    * Tells whether or not a given tm structure can be decoded to NZ Timestamp 
    * or NZ Date.
    * argument ts: the tm structure
    * returns: false if any ts.(tm_mon, tm_mday, tm_hour, tm_min, tm_sec) is
    *   negative or ts.tm_year+1900<SQL_YEAR_MIN 
    *   or isValidDate(ts.tm_mon+1, ts.tm_day, ts.tm_year+1900) is false 
    *   or isValidTime(ts.tm_hour, ts.tm_min, ts.tm_sec) is false. 
    *   true otherwise.
    * throws: error (opaque exception object) if &timestamp is NULL.
   */
   inline bool isValidTimeStruct(const struct tm& ts)
   {
       errorCheck(isAnyNull(&ts),DT_ERROR_NULL_POINTER,NULL);
       return PrivateFunctions::hasGoodDateLowerBounds(ts) &&
              PrivateFunctions::hasGoodTimeLowerBounds(ts) &&
              isValidDate(ts.tm_mon+1, ts.tm_mday, ts.tm_year+1900) &&
              isValidTime(ts.tm_hour, ts.tm_min, ts.tm_sec, 0);
   }
 
   //OFFSET FUNCTIONS
   /**
    * Applies an offset to a NZ Timestamp.
    * argument nzTimestamp: the Timestamp value to offset.
    * argument sqlOffset: the time offset, in minutes, SQL_OFFSET_MIN to 
    *   SQL_OFFSET_MAX inclusive.
    * optional argument errorFlag: if not NULL, *set to true if 
    *   isValidSqlOffset(sqlOffset) is false, or isValidTimestamp(nzTimestamp) 
    *   is false or isValidTimestamp(nzTimestamp+sqlOffset*60*1,000,000) is 
    *   false; *set to false otherwise.
    * returns: ts=nzTimestamp + sqlOffset*60*1,000,000, if *errorFlag is NULL
    *   and an exception is not thrown or if *errorFlag is not NULL and, after 
    *   the call, *errorFlag is false. Indeterminate value otherwise.
    * throws: error (opaque exception object) if errorFlag is NULL and 
    *   (isValidSqlOffset(sqlOffset) is false or isValidTimestamp(nzTimestamp)
    *   is false or isValidTimestamp(nzTimestamp+sqlOffset*60*1,000,000) is
    *   false).
    */
   inline int64 offsetTimestamp(int64 nzTimestamp, int32 sqlOffset,
                                bool* errorFlag)
   {
       if(errorCheck(!isValidTimestamp(nzTimestamp) || 
                     !isValidSqlOffset(sqlOffset), DT_ERROR_INVALID_DATA, 
                     errorFlag))
           return -1;
       int64 result = nzTimestamp+((int64)sqlOffset)*MICROS_PER_MINUTE;
       if(errorCheck(!isValidTimestamp(result),DT_ERROR_INVALID_DATA, 
                     errorFlag))
           return -1;
       return result;
   }
   
   /**
    * Applies an offset to a NZ Time. If nzTime with offset runs over 
    * 23:59:59.999999, 'wraps around' back at zero. For example, applying 
    * '+120 minutes' to the encoded equivalent of '23:00:00' returns the 
    * encoded equivalent of '01:00:00'.
    * argument nzTimestamp: the time value to offset.
    * argument sqlOffset: the time offset, in minutes, SQL_OFFSET_MIN to 
    *   SQL_OFFSET_MAX inclusive.
    * optional argument errorFlag: if not NULL, *set to true if 
    *   isValidSqlOffset(sqlOffset) is false or isValidTime(nzTime) is false; 
    *   false otherwise.
    * returns: t= (nzTime + sqlOffset*60*1,000,000) mod (ENC_TIME_MAX+1), if 
    *   *errorFlag is NULL and an exception is not thrown or if *errorFlag is 
    *   not NULL and, after the call, *errorFlag is false. 
    *   Indeterminate value otherwise.
    * throws: error (opaque exception object) if errorFlag is NULL and 
    *   (isValidSqlOffset(sqlOffset) is false or isValidTime(nzTime) is false).
    */   
   inline int64 offsetTime(int64 nzTime, int32 sqlOffset, 
                           bool* errorFlag)
   {
       if(errorCheck(!isValidTime(nzTime) || !isValidSqlOffset(sqlOffset),
                     DT_ERROR_INVALID_DATA, errorFlag))
           return -1;
       int64 val= 
           (nzTime+((int64)sqlOffset)*MICROS_PER_MINUTE)%(ENC_TIME_MAX+1);
       if(val<0)
                val=val +((int64)MICROS_PER_DAY);
       return val;
   }

   /**
    * Applies an offset to a time_t. Treats time_t as if it is offsetable. 
    * This slightly violates the time_t specification but allows for very 
    * easy usage.
    * argument time: the time value
    * argument sqlOffset: the time offset, in minutes, SQL_OFFSET_MIN to 
    *   SQL_OFFSET_MAX inclusive.
    * optional argument errorFlag: if not NULL, *set to true if 
    *   isValidSqlOffset(sqlOffset) is false or  isValidEpoch(time) is false 
    *   or isValidEpoch(time+sqlOffset*60) is false; *set to false otherwise.
    * returns: t=time+sqlOffset*60, if *errorFlag is NULL and an exception is 
    *   not thrown or if *errorFlag is not NULL and, after the call, 
    *   *errorFlag is false. Indeterminate value otherwise.
    * throws: error (opaque exception object) if errorFlag is NULL and 
    *   (isValidSqlOffset(sqlOffset) is false, or isValidTime_T(time) is 
    *   false or isValidTime_T(time+sqlOffset*60) is false.
    */   
   inline int32 offsetEpoch(int32 time, int32 sqlOffset, bool* errorFlag)
   {
       if(errorCheck(!isValidEpoch(time)||!isValidSqlOffset(sqlOffset),
                     DT_ERROR_INVALID_DATA, errorFlag))
           return -1;
       int32 result = time+sqlOffset*60;
       if(errorCheck(!isValidEpoch(result),DT_ERROR_INVALID_DATA,errorFlag))
           return -1;
       return result;       
   }
   
   /**
    * Applies an offset to a struct tm.
    * argument time: the struct tm value to offset.
    * argument sqlOffset: the time offset, in minutes, SQL_OFFSET_MIN to 
    *   SQL_OFFSET_MAX inclusive.
    * optional argument errorFlag: if not NULL, *set to true if 
    *   isValidSqlOffset(sqlOffset) is false or isValidTimeStruct(time) is 
    *   false or isValidTimeStruct(offset_time) is false; *set to false 
    *   otherwise.
    * returns: let offset_time = tm +/- appropriate offset added (may result 
    *   in different day, month, etc.) offset_time.tm_isdst is set to -1. 
    *   The fields set are offset_time.tm_mday, offset_time.tm_yday, 
    *   offset_time.tm_year, offset_time.tm_mon, offset_time.tm_hour, 
    *   offset_time.tm_min and offset_time.tm_sec. When applicable, all other 
    *   fields of offset_time are set to 0. Returns offset_time, if 
    *   *errorFlag is NULL and an exception is not thrown or if, 
    *   *errorFlag is not NULL and, after the call, *errorFlag is false. 
    *   Returns an indeterminate value otherwise.
    * throws: error (opaque exception object) if &time is NULL or (errorFlag is NULL 
    *   and (isValidSqlOffset(sqlOffset) is false or isValidTimeStruct(time) 
    *   is false or isValidTimeStruct(offset_time) is false)).
    */   
   inline struct ::tm offsetTimeStruct(const struct tm& time, int32 sqlOffset, 
                             bool* errorFlag)
   {
       errorCheck(isAnyNull(&time),DT_ERROR_NULL_POINTER,NULL);
       struct tm result;
       PrivateFunctions::initializeTimeStruct(&result);
       int64 timestamp;
       encodeTimestamp(time,&timestamp,errorFlag);
       if(errorFlag && *errorFlag)
           return result;
       timestamp=offsetTimestamp(timestamp,sqlOffset,errorFlag);
      
       if(errorFlag && *errorFlag)
           return result;
       decodeTimestamp(timestamp,&result,errorFlag);
       return result;
   }
  
   //DECODERS
   /**
    * Converts a Netezza encoded Date value to m/d/y.
    * argument encodedDate: a Date encoded in NZ Format.
    * argument day: the parameter in which to record the day count, 
    *   1 to 31 inclusive.
    * argument month: the parameter in which to record the month number, 
    *   1 to 12 inclusive.
    * argument year: the parameter in which to record the year number, 
    *   SQL_YEAR_MIN to SQL_YEAR_MAX inclusive.
    * optional argument errorFlag: if not NULL, *set to true if 
    *   isValidDate(encodedDate) is false; *set to false otherwise.
    * throws: error (opaque exception object) if any(month,day,year) is NULL or 
    *   (errorFlag is NULL and isValidDate(encodedDate) is false).
    */ 
   inline void decodeDate(int32 encodedDate, uint8* month, uint8* day, 
                          uint16* year, bool* errorFlag)
   {
       errorCheck(isAnyNull(month,day,year), DT_ERROR_NULL_POINTER, NULL);
       if(errorCheck(!isValidDate(encodedDate),DT_ERROR_INVALID_DATA, 
                     errorFlag))
          return;
       int32 l, n, i, j;
       l = encodedDate + 68569 + SQL_FLIEGEL_CORRECTION;
       n = (4 * l) / 146097;
       l -= (146097 * n + 3) / 4;
       i = (4000 * (l + 1)) / 1461001;
       l += 31 - (1461 * i) / 4;
       j = (80 * l) / 2447;
       *day = (uint8)(l - (2447 * j) / 80);
       l = j / 11;
       *month = (uint8)((j + 2) - (12 * l));
       *year = (uint16)(100 * (n - 49) + i + l);
   }
   
   /**
    * Converts a Netezza encoded Date value to time_t. Treats encodedDate as 
    * if it is UTC. Resulting time_t represents the time 00:00:00 on the 
    * specified date.
    * argument encodedDate: a Date encoded in NZ Format.
    * argument result: the parameter in which to record the time_t date 
    *   representation. Forced to be signed int32.
    * optional argument errorFlag: if not NULL, *set to true if 
    *   isValidEpochDate(encodedDate) is false; *set to false otherwise.
    * throws: error (opaque exception object) if result is NULL or (errorFlag is NULL 
    *   and isValidEpochDate(encodedDate) is false).
    */
   inline void decodeDate(int32 encodedDate, int32* result, 
                          bool* errorFlag)
   {
       errorCheck(isAnyNull(result), DT_ERROR_NULL_POINTER, NULL);
       if(errorCheck(!isValidEpochDate(encodedDate), DT_ERROR_INVALID_DATA, 
                     errorFlag))
           return;
       *result = (encodedDate-EPOCH_START_AS_DATE)* SECONDS_PER_DAY;
   }
   
   /**
    * Converts a Netezza encoded Date value to struct tm. Resulting tm 
    * represents the time 00:00:00 on the specified date, with an unknown 
    * daylight savings status.
    * argument encodedDate: a Date encoded in NZ Format.
    * argument result: the structure where the decoded Date is written, 
    *   such that result->tm_year, result->tm_mon, result-> tm_mday, 
    *   result->tm_yday and result->tm_wday contain the appropriate fields 
    *   in tm format. result->tm_isdst is set to -1. When applicable, all the 
    *   other fields of result are set to 0.
    * optional argument errorFlag: if not NULL, *set to true if 
    *   isValidDate(encodedDate) is false; *set to false otherwise.
    * throws: error (opaque exception object) if result is NULL or 
    *   (errorFlag is NULL and isValidDate(encodedDate) is false).
    */
   inline void decodeDate(int32 encodedDate, struct tm* result, 
                          bool* errorFlag)
   {
       errorCheck(isAnyNull(result), DT_ERROR_NULL_POINTER, NULL);
       PrivateFunctions::initializeTimeStruct(result);
       //temps are used instead of addressing fields of tm directly
       //[ (uint8*) &result->tm_mon] because diab screws up the value
       //when casting between uint8/16 and 32. 
       //This comment also applies to decodeTimestamp(int64, struct tm*, bool*)
       uint8 mon,day;
       uint16 year;
       decodeDate(encodedDate,&mon,&day,&year,errorFlag);
       if(errorFlag && *errorFlag) //means decodeDate() errored out
           return;      
       result->tm_mon=mon;
       result->tm_mday=day;
       result->tm_year=year;
       PrivateFunctions::formatTimeStruct(result, encodedDate);
       result->tm_hour=0;
       result->tm_min=0;
       result->tm_sec=0;
   }
   
   /**
    * Converts a Netezza encoded Time value to h:m:s:micros.
    * argument encodedTime: a Time encoded in NZ Format.
    * argument hour: the parameter in which to record the hour, 
    *   0 to 23 inclusive.
    * argument minute: the parameter in which to record the minute, 
    *   0 to 59 inclusive.
    * argument second: the parameter in which to record the second, 
    *   0 to 59 inclusive.
    * argument mcrs: the parameter in which to record the microsecond, 
    *   0 to 999,999 inclusive.
    * optional argument errorFlag: if not NULL, *set to true if 
    *   isValidTime(encodedTime) is false; *set to false otherwise.
    * throws: error (opaque exception object) if any(hour,minute,second,mcrs) is NULL 
    *   or (errorFlag is NULL and isValidTime(encodedTime) is false).
    */   
   inline void decodeTime(int64 encodedTime, uint8* hour, uint8* minute, 
                          uint8* second, uint32* mcrs, bool* errorFlag)
   {
       errorCheck(isAnyNull(hour,minute,second,mcrs), DT_ERROR_NULL_POINTER, 
                  NULL);
       if(errorCheck(!isValidTime(encodedTime), DT_ERROR_INVALID_DATA, 
                     errorFlag))
           return;
       int64 interMediary = encodedTime%MICROS_PER_SECOND;
       *mcrs=interMediary;
       //make intermediary a number of seconds
       interMediary=encodedTime/MICROS_PER_SECOND; 
       *second=interMediary%60; 
       //inter/60=total number of minutes. 
       //that %60 = total number of minutes after the hour.
       *minute=(interMediary/60)%60; 
       //there is 3600 seconds in an hour;
       *hour=interMediary/3600; 
   }
   
   /**
    * Converts a Netezza encoded TimeTz value to h:m:s:micros.
    * argument encodedTime: a Time encoded in NZ Format.
    * argument encodedZone: a Time Offset encoded in NZ format
    * argument hour: the parameter in which to record the hour, 
    *   0 to 23 inclusive.
    * argument minute: the parameter in which to record the minute, 
    *   0 to 59 inclusive.
    * argument second: the parameter in which to record the second, 
    *   0 to 59 inclusive.
    * argument mcrs: the parameter in which to record the microsecond 
    *   0 to 999,999 inclusive.
    * argument sqlOffset: the parameter in which to record the offset in 
    *   minutes, SQL_OFFSET_MIN to SQL_OFFSET_MAX inclusive.
    * optional argument errorFlag: if not NULL, *set to true if 
    *   isValidTimeTz(encodedTime, encodedZone) is false; *set to false 
    *   otherwise.
    * throws: error (opaque exception object) if any(hour,minute,second,mcrs,sqloffset)
    *   is NULL or (errorFlag is NULL and 
    *   isValidTimeTZ(encodedTime, encodedZone) is false).
    */    
   inline void decodeTimeTz(int64 encodedTime, int32 encodedZone, 
                            uint8* hour,  uint8* minute, uint8* second, 
                            uint32* mcrs, int16* sqlOffset, 
                            bool* errorFlag)
   {
       errorCheck(isAnyNull(sqlOffset), DT_ERROR_NULL_POINTER, NULL);
       if(errorCheck(!isValidTimeTzOffset(encodedZone),DT_ERROR_INVALID_DATA,
                     errorFlag)) 
           return;
       decodeTime(encodedTime,hour,minute,second,mcrs,errorFlag);
       if(errorFlag && *errorFlag) 
           return;
       *sqlOffset=encodedZone/(-60);
   }
   
   /**
    * Converts a Netezza encoded Timestamp value to m/d/y, h:m:s:micros.
    * argument encodedTimestamp: a Timestamp encoded in NZ Format.
    * argument day: the parameter in which to record the day count, 
    *   1 to 31 inclusive.
    * argument month: the parameter in which to record the month number, 
    *   1 to 12 inclusive.
    * argument year: the parameter in which to record the year number, 
    *   SQL_YEAR_MIN to SQL_YEAR_MAX inclusive.
    * argument hour: the parameter in which to record the hour, 
    *   0 to 23 inclusive.
    * argument minute: the parameter in which to record the minute, 
    *   0 to 59 inclusive.
    * argument second: the parameter in which to record the second, 
    *   0 to 59 inclusive.
    * argument mcrs: the parameter in which to record the microsecond, 
    *   0 to 999,999 inclusive.
    * optional argument errorFlag: if not NULL, *set to true if 
    *   isValidTimestamp(encodedTimestamp) is false; *set to false otherwise.
    * throws: error (opaque exception object) if 
    *   any(month,day,year,hour,minute,second,mcrs) is NULL or 
    *   (errorFlag is NULL and isValidTimestamp(encodedTimestamp) is false).
    */
   inline void decodeTimestamp(int64 encodedTimestamp, uint8* month, 
                               uint8* day, uint16* year, uint8* hour, 
                               uint8* minute, uint8* second, uint32* mcrs, 
                               bool* errorFlag)
   {
       int32 date = PrivateFunctions::nzDateFromNzTimestamp(encodedTimestamp);
       int64 time = PrivateFunctions::nzTimeFromNzTimestamp(encodedTimestamp);
       if(encodedTimestamp<0&&time!=0)
           date--; //to adjust for negative times
       decodeDate(date,month,day,year,errorFlag);
       if(errorFlag && *errorFlag)
           return;
       decodeTime(time,hour,minute,second,mcrs,errorFlag);
   }
   
   /**
    * Converts a Netezza encoded Timestamp value to time_t. Drops the 
    *   microseconds after the last whole minute of the timestamp value.
    * argument encodedTimestamp: a Timestamp encoded in NZ Format.
    * argument result: the parameter in which to return the resulting time_t
    *   value. Forced to be signed int32.
    * optional argument errorFlag: if not NULL, *set to true if 
    *   isValidEpochTimestamp(encodedTimestamp) is false; *set to false 
    *   otherwise.
    * throws: error (opaque exception object) if result is NULL or 
    *   (errorFlag is NULL and isValidEpochTimestamp(encodedTimestamp) is 
    *   false).
    */   
   inline void decodeTimestamp(int64 encodedTimestamp, int32* result, 
                               bool* errorFlag)
   {
       errorCheck(isAnyNull(result),DT_ERROR_NULL_POINTER,NULL);
       if(errorCheck(!isValidEpochTimestamp(encodedTimestamp),
                     DT_ERROR_INVALID_DATA,errorFlag))
           return;
       *result= ((encodedTimestamp-EPOCH_START_AS_DATE*MICROS_PER_DAY)
                /MICROS_PER_SECOND);
   }
   
   /**
    * Converts a Netezza encoded Timestamp value to struct timeval.
    * argument encodedTimestamp: a Timestamp encoded in NZ Format.
    * argument result: the structure where the decoded Timestamp is written.
    * optional argument errorFlag: if not NULL, *set to true if 
    *   isValidEpochTimestamp(encodedTimestamp) is false; *set to false 
    *   otherwise.
    * throws: error (opaque exception object) if result is NULL or 
    *   (errorFlag is NULL and isValidEpochTimestamp(encodedTimestamp) is 
    *   false).
    */   
   inline void decodeTimestamp(int64 encodedTimestamp, struct timeval* result,
                               bool* errorFlag)
   {
       errorCheck(isAnyNull(result),DT_ERROR_NULL_POINTER,NULL);
       int32 sec = result->tv_sec;
       decodeTimestamp(encodedTimestamp, &sec, errorFlag);
       if(errorFlag && *errorFlag) //decodeTimestamp() errored out
           return;
       result->tv_sec = sec;
       result->tv_usec = encodedTimestamp%MICROS_PER_SECOND;
   }
   
   /**
    * Converts a Netezza encoded Timestamp value to struct tm. Drops the 
    * microseconds after the last whole minute of the timestamp value.
    * argument encodedTimestamp: a Timestamp encoded in NZ Format.
    * argument result: the structure where the decoded Timestamp is written, 
    *   such that result->tm_hour, result->tm_min, result->tm_sec, 
    *   result->tm_year, result->tm_mon, result->tm_mday, result->tm_yday, 
    *   and result->tm_wday contain the appropriate fields in tm format. 
    *   Result->tm_isdst is set to -1. If applicable, all other fields of 
    *   result are set to 0.
    * optional argument errorFlag: if not NULL, *set to true if 
    *   isValidTimestamp(encodedTimestamp) is false; *set to false otherwise.
    * throws: error (opaque exception object) if result is NULL or 
    *   (errorFlag is NULL and isValidTimestamp(encodedTimestamp) is false).
    */
   inline void decodeTimestamp(int64 encodedTimestamp, struct tm* result,
                               bool* errorFlag)
   {
       errorCheck(isAnyNull(result),DT_ERROR_NULL_POINTER,NULL);
       PrivateFunctions::initializeTimeStruct(result);
       //for explanation as to why temp vars are used, see comment in
       //decodeDate(int32, struct tm*, bool*)
       uint8 mon,day,hour,min,sec;
       uint16 year; 
       IgnoreBuffer ignore;
       decodeTimestamp(encodedTimestamp,&mon,&day,&year,&hour,&min,&sec, 
                       &ignore.u32,errorFlag);
       result->tm_mon=mon;
       result->tm_mday=day;
       result->tm_year=year;
       result->tm_hour=hour;
       result->tm_min=min;
       result->tm_sec=sec;
       if(errorFlag && *errorFlag) //decodeTimestamp() errored out
           return;
       PrivateFunctions::formatTimeStruct(result, 
                  PrivateFunctions::nzDateFromNzTimestamp(encodedTimestamp));
   }
   
   //ENCODERS
   /**
    * Converts a m/d/y Date value to a Netezza encoded Date.
    * argument day: the day count, 1 to 31 inclusive.
    * argument month: the month number, 1 to 12 inclusive.
    * argument year: the year number, SQL_YEAR_MIN to SQL_YEAR_MAX inclusive.
    * argument encodedDate: the parameter in which to record the Date 
    *   encoded in NZ Format.
    * optional argument errorFlag: if not NULL, *set to true if 
    *   isValidDate(month,day,year) is false; *set to false otherwise.
    * throws: error (opaque exception object) if encodedDate is NULL or 
    *   (errorFlag is NULL and isValidDate(month, day, year) is false).
    */   
   inline void encodeDate(uint32 month, uint32 day, uint32 year, 
                          int32* encodedDate, bool* errorFlag)
   {
       errorCheck(isAnyNull(encodedDate),DT_ERROR_NULL_POINTER,NULL);
       if(errorCheck(!isValidDate(month,day,year),DT_ERROR_INVALID_DATA,
                     errorFlag))
           return;
       
       int y =year;
       int m=month;
       int d=day;
       int m12= (m - 14) / 12;
       *encodedDate=((1461 * (y + 4800 + m12)) / 4 + 
                    (367 * (m - 2 - 12 * (m12))) / 12 -
                    (3 * ((y + 4900 + m12) / 100)) / 4 + d - 32075) - 
                    SQL_FLIEGEL_CORRECTION;
   }
   
   /**
    * Converts a time_t Date value to a Netezza encoded Date. Drops the hours,
    * minutes and seconds elapsed after the last whole day in the time_t value.
    * argument date: the time_t date value
    * argument encodedDate: the parameter in which to record the Date encoded 
    *   in NZ Format.
    * optional argument errorFlag: if not NULL, *set to true if 
    *   isValidEpoch(date) is false; *set to false otherwise.
    * throws: error (opaque exception object) if encodedDate is NULL or 
    *   (errorFlag is NULL and isValidEpoch(date) is false).
    */
   inline void encodeDate(int32 date, int32* encodedDate, 
                          bool* errorFlag)
   {
       errorCheck(isAnyNull(encodedDate),DT_ERROR_NULL_POINTER,NULL);
       if(errorCheck(!isValidEpoch(date),DT_ERROR_INVALID_DATA,errorFlag))
           return;
       *encodedDate=date/SECONDS_PER_DAY+EPOCH_START_AS_DATE;
   }
   
   /**
    * Converts a struct tm value to a Netezza encoded Date. Uses tm.tm_year, 
    *   tm.tm_mon and tm.tm_day fields of date only. Ignores the rest. It is 
    *   desireable that date passes isValidTimeStruct(), but not fully 
    *   required.
    * argument date: the struct tm date value
    * argument encodedDate: the parameter in which to record the Date encoded 
    *   in NZ Format.
    * optional argument errorFlag: if not NULL, *set to true if date.tm_mon<0 
    *   or date.tm_mday<1 or date.tm_year+1900<SQL_YEAR_MIN or 
    *   isValidDate(date.tm_mon+1, date.tm_mday, date.tm_year) is false; 
    *   *set to false otherwise.
    * throws: error (opaque exception object) if &date is NULL or encodedDate is NULL 
    *   or (errorFlag is NULL and (isValidDate(date.tm_mon+1, date.tm_mday, 
    *   date.tm_year+1900) is false or date.tm_mon<0 or date.tm_mday<1 or 
    *   date.tm_year+1900<SQL_YEAR_MIN)).
    */
   inline void encodeDate(const struct tm& date, int32* encodedDate, 
                          bool* errorFlag)
   {
       errorCheck(isAnyNull(&date), DT_ERROR_NULL_POINTER, NULL);
       if(errorCheck(!PrivateFunctions::hasGoodDateLowerBounds(date), 
                     DT_ERROR_INVALID_DATA, errorFlag))
           return;
       encodeDate((uint32)date.tm_mon+1,(uint32)date.tm_mday,
                  (uint32)date.tm_year+1900, encodedDate, errorFlag);
   }
   
   /**
    * Converts a h:m:s:micros Time value to a Netezza encoded Time.
    * argument hour: the hour, 0 to 23 inclusive.
    * argument minute: the minute, 0 to 59 inclusive.
    * argument second: the second, 0 to 59 inclusive.
    * argument mcrs: the microsecond, 0 to 999,999 inclusive.
    * argument encodedTime: the parameter in which to record the Time encoded 
    *   in NZ Format.
    * optional argument errorFlag: if not NULL, *set to true if 
    *   isValidTime(hour,minute,second,mcrs) is false; *set to false otherwise.
    * throws: error (opaque exception object) if encodedTime is NULL or 
    *   (errorFlag is NULL and isValidTime(hour, minute, second, mcrs) is 
    *   false).
    */
   inline void encodeTime(uint32 hour, uint32 minute, uint32 second, 
                          uint32 mcrs, int64* encodedTime, 
                          bool* errorFlag)
   {
      errorCheck(isAnyNull(encodedTime), DT_ERROR_NULL_POINTER, NULL);
      if(errorCheck(!isValidTime(hour,minute,second,mcrs), 
                    DT_ERROR_INVALID_DATA, errorFlag))
          return;
      *encodedTime= ((int64)(hour*3600+minute*60+second)* MICROS_PER_SECOND + 
                    (int64)mcrs);

   }
   
   /**
    * Converts a h:m:s:micros TimeTZ value to a Netezza encoded TimeTZ.
    * argument hour: the hour, 0 to 23 inclusive.
    * argument minute: the minute, 0 to 59 inclusive.
    * argument second: the second, 0 to 59 inclusive.
    * argument mcrs: the microsecond, 0 to 999,999 inclusive.
    * argument sqlOffset: offset in minutes, SQL_OFFSET_MIN to 
    *   SQL_OFFSET_MAX inclusive.
    * argument encodedTime: the parameter in which to record the Time encoded 
    *   in NZ Format.
    * argument encodedZone: the parameter in which to record the Time Offset 
    *   in NZ Format.
    * optional argument errorFlag: if not NULL, *set to true if 
    *   isValidTimeTz(hour,minute,second,mcrs) is false; *set to false 
    *   otherwise.
    * throws: error (opaque exception object) if any *argument is NULL or 
    *   (errorFlag is NULL and isValidTimeTZ(hour, minute, second, mcrs, 
    *   sqlOffset) is false).
    */   
   inline void encodeTimeTz(uint32 hour, uint32 minute, uint32 second,
                            uint32 mcrs, int32 sqlOffset, int64* encodedTime,
                            int32* encodedZone, bool* errorFlag)
   {
       errorCheck(isAnyNull(encodedZone),DT_ERROR_NULL_POINTER,NULL);
       if(errorCheck(!isValidSqlOffset(sqlOffset),DT_ERROR_INVALID_DATA,
                     errorFlag))
           return;
       encodeTime(hour,minute,second,mcrs,encodedTime,errorFlag);
       if(errorFlag&&*errorFlag)
           return;
       *encodedZone=sqlOffset*-60;
   }
   
   /**
    * Converts a m/d/y, h:m:s:micros Timestamp value to a Netezza encoded 
    * Timestamp.
    * argument year: the year of the date, SQL_YEAR_MIN to SQL_YEAR_MAX
    *   inclusive.
    * argument month: the month, 1 to 12 inclusive.
    * argument day: the day, 1 to 31 inclusive.
    * argument hour: the hour, 0 to 23 inclusive.
    * argument minute: the minute, 0 to 59 inclusive.
    * argument second: the second, 0 to 59 inclusive.
    * argument mcrs: the microsecond, 0 to 999,999 inclusive.
    * argument encodedTimestamp: the parameter in which to record the Timestamp 
    *   in NZ Format.
    * optional argument errorFlag: if not NULL, *set to true if 
    *   isValidTimestamp(month, day, year, hour, minute, second, mcrs) is 
    *   false; *set to false otherwise.
    * throws: error (opaque exception object) if encodedTimestamp is NULL or 
    *   (errorFlag is NULL and isValidTimestamp(month, day, year, hour, 
    *   minute, second, mcrs) is false).
     */
   inline void encodeTimestamp(uint32 month, uint32 day, uint32 year, 
                               uint32 hour, uint32 minute, uint32 second,
                               uint32 mcrs, int64* encodedTimestamp,
                               bool* errorFlag)
   {
       errorCheck(isAnyNull(encodedTimestamp),DT_ERROR_NULL_POINTER,NULL);
       int32 eDate;
       encodeDate(month,day,year,&eDate,errorFlag);
       if(errorFlag && *errorFlag)
           return;
       int64 eTime;
       encodeTime(hour,minute,second,mcrs,&eTime,errorFlag);
       if(errorFlag && *errorFlag)
           return;
       *encodedTimestamp=((int64)eDate*MICROS_PER_DAY)+eTime;
   }
   
   /**
    * Converts a time_t value to a Netezza encoded Timestamp. Encodes the 
    * value in UTC - applies no offsets. Adds 0 microseconds to the encoded 
    * value.
    * argument ts: the time_t Timestamp value
    * argument encodedTimestamp: the parameter in which to record the Timestamp 
    *   encoded in NZ Format.
    * optional argument errorFlag: if not NULL, *set to true if 
    *   isValidEpoch(ts) is false; *set to false otherwise.
    * throws: error (opaque exception object) if encodedTimestamp is NULL or 
    *   (errorFlag is NULL and isValidEpoch(ts) is false).
    */
   inline void encodeTimestamp(int32 ts, int64* encodedTimestamp, 
                               bool* errorFlag)
   {
       errorCheck(isAnyNull(encodedTimestamp),DT_ERROR_NULL_POINTER,NULL);
       *encodedTimestamp=(((int64)ts)%SECONDS_PER_DAY)*MICROS_PER_SECOND;
       int32 eDate;
       encodeDate(ts,&eDate,errorFlag);
       if(errorFlag && *errorFlag)
           return;
       *encodedTimestamp+=((int64) eDate)*MICROS_PER_DAY;
   }
   
   /**
    * Converts a struct timeval value to a Netezza encoded Timestamp.
    * argument ts: the struct timeval Timestamp value
    * argument encodedTimestamp: the parameter in which to record the 
    *   Timestamp encoded in NZ Format.
    * optional argument errorFlag: if not NULL, *set to true if 
    *   isValidTimeVal(ts) is false; *set to false otherwise.
    * throws: error (opaque exception object) if &ts is NULL or encodedTimestamp 
    *   is NULL or (errorFlag is NULL and isValidTimeVal(ts) is false).
    */
   inline void encodeTimestamp(const struct timeval& ts, 
                               int64* encodedTimestamp, bool* errorFlag)
   {
       errorCheck(isAnyNull(&ts),DT_ERROR_NULL_POINTER,NULL);
       if(errorCheck(!isValidTimeValUsecs(ts.tv_usec),
                     DT_ERROR_INVALID_DATA,errorFlag))
           return;
       encodeTimestamp(ts.tv_sec,encodedTimestamp,errorFlag);
       if(errorFlag && *errorFlag)
           return;
       *encodedTimestamp+=ts.tv_usec;
   }
   
   /**
    * Converts a struct tm value to a Netezza encoded Timestamp. Uses 
    *   ts.tm_year, ts.tm_day, ts.tm_mon, ts.tm_hour, ts.tm_min and ts.tm_sec 
    *   fields of ts only. Ignores the rest. ts must pass isValidTimeStruct(). 
    *   Adds 0 microseconds to the encoded value.
    * argument ts: the struct tm Timestamp value
    * argument encodedTimestamp: the parameter in which to record the 
    *   Timestamp encoded in NZ Format.
    * optional argument errorFlag: if not NULL, *set to true if 
    *   isValidTimeStruct(ts) is false; *set to false otherwise.
    * throws: error (opaque exception object) if &ts is NULL or encodedTimestamp is 
    *   NULL or (errorFlag is NULL and isValidTimeStruct(ts) is false).
    */
   inline void encodeTimestamp(const struct tm& ts, int64* encodedTimestamp,
                               bool* errorFlag)
   {
       errorCheck(isAnyNull(&ts),DT_ERROR_NULL_POINTER,NULL);
       if(errorCheck(!PrivateFunctions::hasGoodDateLowerBounds(ts) || 
                     !PrivateFunctions::hasGoodTimeLowerBounds(ts),
                     DT_ERROR_INVALID_DATA,errorFlag))
           return;
       encodeTimestamp(ts.tm_mon+1,ts.tm_mday,ts.tm_year+1900,ts.tm_hour,
                       ts.tm_min,ts.tm_sec,0,encodedTimestamp,errorFlag);
   }
}
}
};

#endif /*UDX_HELPERS_INCLUDED*/ 

