#include "commmsgbody.h"
#include "psqlvalue.h"
#include "initdatetime.h"

//
// SQLSMALLINT wrapper
//
void PSqlSmallInt::init(SQLSMALLINT value_)
{
	value = value_;
}

void PSqlSmallInt::operator=(SQLSMALLINT value_)
{
	init(value_);
}

void PSqlSmallInt::parse(CommMsgParser& parser)
{
	UINT16 value_;
	parser.parseUINT16(value_);
	value = value_;
}

/*virtual*/ void PSqlSmallInt::compose(CommMsgBody& body) const
{
	body.composeUINT16(value);
}

//
// SQLINTEGER wrapper
//
void PSqlInt::init(SQLINTEGER value_)
{
	value = value_;
}

void PSqlInt::operator=(SQLINTEGER value_)
{
	init(value_);
}

void PSqlInt::parse(CommMsgParser& parser)
{
	UINT32 value_;
	parser.parseUINT32(value_);
	value = value_;
}

/*virtual*/ void PSqlInt::compose(CommMsgBody& body) const
{
	body.composeUINT32(value);
}

//
// Nullable SQLINTEGER wrapper
//
void PSqlIntNullable::init(SQLINTEGER value_)
{
	value = value_;
	cb = 0;
}

void PSqlIntNullable::operator=(SQLINTEGER value_)
{
	init(value_);
}

void PSqlIntNullable::parse(CommMsgParser& parser)
{
	bool isNull;
	parser.parseBOOL(isNull);
	if(isNull)
	{
		cb = SQL_NULL_DATA;
	}
	else
	{
		cb = 0;
		PSqlInt::parse(parser);
	}
}

/*virtual*/ void PSqlIntNullable::compose(CommMsgBody& body) const
{
	body.composeBOOL(cb == SQL_NULL_DATA); // isNull
	if(cb != SQL_NULL_DATA)
	{
		PSqlInt::compose(body);
	}
}

//
// SQLBIGINT wrapper
//
void PSqlBigInt::init(SQLBIGINT value_)
{
	value = value_;
}

void PSqlBigInt::operator=(SQLBIGINT value_)
{
	init(value_);
}

void PSqlBigInt::parse(CommMsgParser& parser)
{
	UINT64 value_;
	parser.parseUINT64(value_);
	value = value_;
}

/*virtual*/ void PSqlBigInt::compose(CommMsgBody& body) const
{
	body.composeUINT64(value);
}

//
// SQL_TIMESTAMP_STRUCT wrapper
//
void PSqlTimestamp::init(const SrvTime& value_)
{
	initTimeStruct(value, value_);
}

void PSqlTimestamp::init(const CommUtcTime& value_)
{
	initTimeStruct(value, value_);
}

void PSqlTimestamp::operator=(const SrvTime& value_)
{
	init(value_);
}

void PSqlTimestamp::parse(CommMsgParser& parser)
{
	SrvTime srvTimeValue;
	srvTimeValue.parse(parser);
	initTimeStruct(value, srvTimeValue);
}

/*virtual*/ void PSqlTimestamp::compose(CommMsgBody& body) const
{
	SrvTime srvTimeValue;
	initSrvTime(srvTimeValue, &value);
	srvTimeValue.compose(body);
}

void PSqlTimestamp::getValue(SrvTime& srvTime) const
{
	if(cb == SQL_NULL_DATA)
		srvTime.setNull();
	else
		initSrvTime(srvTime, &value);
}

void PSqlTimestamp::init_1990IfNull(const SrvTime& value_)
{
	if (value_.isNull())
	{
		SrvTime t1990;
		t1990._year  = 1990;
		t1990._month = 1;
		t1990._day   = 1;
		init(t1990);
		return;
	}
	init(value_);
}

void PSqlTimestamp::getValue(CommUtcTime& srvTime) const
{
	if(cb == SQL_NULL_DATA)
		srvTime.setNull();
	else
		initSrvTime(srvTime, &value);
}

void PSqlTimestamp::getValue_NullIf1990(SrvTime& srvTime) const
{
	getValue(srvTime);
	if (srvTime._year == 1990 && srvTime._month == 1 && srvTime._day == 1 &&
		srvTime._hour == 0 && srvTime._minute == 0 && srvTime._second == 0 )
	{
		srvTime.setNull();
	}
}
//
// Nullable SQL_TIMESTAMP_STRUCT wrapper
//
void PSqlTimestampNullable::init(const SrvTime& value_)
{
	if(value_.isNull())
	{
		cb = SQL_NULL_DATA;
	}
	else
	{
		cb = 0;
		initTimeStruct(value, value_);
	}
}

void PSqlTimestampNullable::operator=(const SrvTime& value_)
{
	init(value_);
}

void PSqlTimestampNullable::parse(CommMsgParser& parser)
{
	bool isNull;
	parser.parseBOOL(isNull);
	if(isNull)
	{
		cb = SQL_NULL_DATA;
	}
	else
	{
        cb = 0;
		SrvTime srvTimeValue;
		srvTimeValue.parse(parser);
		initTimeStruct(value, srvTimeValue);
	}
}

/*virtual*/ void PSqlTimestampNullable::compose(CommMsgBody& body) const
{
	body.composeBOOL(cb == SQL_NULL_DATA); // isNull
	if(cb != SQL_NULL_DATA)
	{
		SrvTime srvTimeValue;
		initSrvTime(srvTimeValue, &value);
		srvTimeValue.compose(body);
	}
}

//
// SQL_DATE_STRUCT wrapper
//
void PSqlDate::init(const SrvDate& value_)
{
	initDateStruct(value, value_);
}

void PSqlDate::operator=(const SrvDate& value_)
{
	init(value_);
}

void PSqlDate::parse(CommMsgParser& parser)
{
	SrvDate srvDateValue;
	srvDateValue.parse(parser);
	initDateStruct(value, srvDateValue);
}

/*virtual*/ void PSqlDate::compose(CommMsgBody& body) const
{
	SrvDate srvDateValue;
	initSrvDate(srvDateValue, &value);
	srvDateValue.compose(body);
}

void PSqlDate::getValue(SrvDate& srvDate) const
{
	if(cb == SQL_NULL_DATA)
		srvDate.setNull();
	else
		initSrvDate(srvDate, &value);
}

void PSqlDate::init_1990IfNull(const SrvDate& value_)
{
	if (value_.isNull())
	{
		SrvDate t1990;
		t1990._year  = 1990;
		t1990._month = 1;
		t1990._day   = 1;
		init(t1990);
		return;
	}
	init(value_);
}

void PSqlDate::getValue_NullIf1990(SrvDate& srvDate) const
{
	getValue(srvDate);
	if (srvDate._year == 1990 && srvDate._month == 1 && srvDate._day == 1)
	{
		srvDate.setNull();
	}
}
//
// Nullable SQL_DATE_STRUCT wrapper
//
void PSqlDateNullable::init(const SrvDate& value_)
{
	if(value_.isNull())
	{
		cb = SQL_NULL_DATA;
	}
	else
	{
		cb = 0;
		initDateStruct(value, value_);
	}
}

void PSqlDateNullable::operator=(const SrvDate& value_)
{
    init(value_);
}

void PSqlDateNullable::parse(CommMsgParser& parser)
{
	bool isNull;
	parser.parseBOOL(isNull);
	if(isNull)
	{
		cb = SQL_NULL_DATA;
	}
	else
	{
        cb = 0;
		SrvDate srvDateValue;
		srvDateValue.parse(parser);
		initDateStruct(value, srvDateValue);
	}
}

/*virtual*/ void PSqlDateNullable::compose(CommMsgBody& body) const
{
	body.composeBOOL(cb == SQL_NULL_DATA); // isNull
	if(cb != SQL_NULL_DATA)
	{
		SrvDate srvDateValue;
		initSrvDate(srvDateValue, &value);
		srvDateValue.compose(body);
	}
}

void PSqlXml::parse(CommMsgParser& parser)
{
	bool isNull;
	parser.parseBOOL(isNull);
	if(isNull)
	{
		clearAndSetNull();
	}
	else
	{
		parser.parseStringP(xmlInUtf8);
		cb = xmlInUtf8.length();
	}
}

/*virtual*/ void PSqlXml::compose(CommMsgBody& body) const
{
	body.composeBOOL(isNull());
	if(!isNull())
	{
		body.composeString(xmlInUtf8);
	}
}
// PYR-46890
void PSqlTournamentId::init( UINT32 value_ )
{
	value = (UINT64)value_;
}

void PSqlTournamentId::operator=( UINT32 value_ )
{
	init( value_ );
}

void PSqlTournamentId::parse( CommMsgParser& parser )
{
	UINT32 value_;
	parser.parseUINT32( value_ );
	value = (UINT64)value_;
}

void PSqlTournamentId::compose( CommMsgBody& body ) const //override
{
	body.composeUINT32( value );
}

void PSqlTournIdNullable::init( UINT32 value_ )
{
	value = (UINT64)value_;
	cb = 0;
}

void PSqlTournIdNullable::operator=( UINT32 value_ )
{
	init( value_ );
}

void PSqlTournIdNullable::parse( CommMsgParser& parser )
{
	bool isNull;
	parser.parseBOOL( isNull );
	if( isNull )
	{
		cb = SQL_NULL_DATA;
	}
	else
	{
		cb = 0;
		PSqlTournamentId::parse( parser );
	}
}

/*virtual*/ void PSqlTournIdNullable::compose( CommMsgBody& body ) const
{
	body.composeBOOL( cb == SQL_NULL_DATA ); // isNull
	if( cb != SQL_NULL_DATA )
	{
		PSqlTournamentId::compose( body );
	}
}

void PSqlBigIntAsUInt32::init(UINT32 value_)
{
	if (value_ == UINT32(-1))
	{
		value = -1;
	}
	else
	{
		value = value_;
	}
}

void PSqlBigIntAsUInt32::operator=(UINT32 value_)
{
	init(value_);
}

void PSqlBigIntAsUInt32::parse(CommMsgParser& parser)
{
	UINT32 value_;
	parser.parseUINT32(value_);
	value = value_;
}

void PSqlBigIntAsUInt32::compose(CommMsgBody& body) const //override
{
	body.composeUINT32(value);
}
