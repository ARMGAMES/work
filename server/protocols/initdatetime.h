#ifndef initdatetime_h
#define initdatetime_h

#include "dbmstmt.h"
#include "pyrtime.h"

void initTimeStruct(SQL_TIMESTAMP_STRUCT& ts, const SrvTime& st );
void initTimeStruct(SQL_TIMESTAMP_STRUCT& ts, const CommUtcTime& st );
void initDateStruct(SQL_DATE_STRUCT& ds, const SrvDate& sd );
void initSrvTime(SrvTime& srvTime, const SQL_TIMESTAMP_STRUCT* sqlTime);
void initSrvTime(CommUtcTime& srvTime, const SQL_TIMESTAMP_STRUCT* sqlTime);
void initSrvDate(SrvDate& srvDate, const SQL_DATE_STRUCT* sqlDate);

void initTimeStructDateHMS(
								  SQL_TIMESTAMP_STRUCT& ts, 
								  const SrvDate& sd,
								  BYTE	_hour,
								  BYTE	_minute,
								  BYTE	_second
								  );

#endif
