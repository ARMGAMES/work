#ifndef psqlvalue_h_included
#define psqlvalue_h_included

 //lint -save -e1511 -e1721

#include "dbcommon.h"
#include "pyrtime.h"
#include "initdata.h"

//
// Base Sql value class
//
struct PSqlValue
{
	SQLLEN cb;
	virtual void compose(CommMsgBody& body) const = 0;
	bool isNull() const { return cb == SQL_NULL_DATA; }

protected:
	PSqlValue() { cb = 0; }
	void setNull() { cb = SQL_NULL_DATA; }
};

//
// SQLSMALLINT wrapper
//
struct PSqlSmallInt : public PSqlValue
{
	SQLSMALLINT value;

	PSqlSmallInt() : PSqlValue(), value(0) {}

	void init(SQLSMALLINT value_);
	void operator=(SQLSMALLINT value_);

	void parse(CommMsgParser& parser);
	void compose(CommMsgBody& body) const override;
};

//
// SQLINTEGER wrapper
//
struct PSqlInt : public PSqlValue
{
	SQLINTEGER value;

	PSqlInt() : PSqlValue(), value(0) {}

	void init(SQLINTEGER value_);
	void operator=(SQLINTEGER value_);

	void parse(CommMsgParser& parser);
	void compose(CommMsgBody& body) const override;
};

//
// Nullable SQLINTEGER wrapper
//
struct PSqlIntNullable : public PSqlInt
{
	PSqlIntNullable() : PSqlInt() {}

	void init(SQLINTEGER value_);
	void operator=(SQLINTEGER value_);

	void parse(CommMsgParser& parser);
	void compose(CommMsgBody& body) const override;
	PSqlValue::setNull;
};

//
// SQLBIGINT wrapper
//
struct PSqlBigInt : public PSqlValue
{
	SQLBIGINT value;
	
	PSqlBigInt() : PSqlValue(), value(0) {}
	
	void init(SQLBIGINT value_);
	void operator=(SQLBIGINT value_);
	void parse(CommMsgParser& parser);
	void compose(CommMsgBody& body) const override;
};

//
// SQLCHAR[] wrapper
//
template< int N >
struct PSqlString : public PSqlValue
{
	SQLCHAR value[N];

	PSqlString() : PSqlValue()
	{
		value[0] = 0;
	}

	void init(const char* value_)
	{
		strCopy((char*)value, N, value_);
	}

	void operator=(const char* value_)
	{
		init(value_);
	}

	void initCut(const char* value_)
	{
		strCopyCut((char*)value, N, value_);
	}

	void parse(CommMsgParser& parser)
	{
		const char* value_;
		parser.parseString(value_);
		strCopy((char*)value, N, value_);
	}

	void compose(CommMsgBody& body) const override
	{
		body.composeString((char*)value);
	}

	const char* c_str() const
	{
		return (const char*)value;
	}

	bool isEmptyOrNull() const // (field IS NULL) || (length == 0)
	{
		return cb == 0 || isNull();
	}
};

//
// SQL_TIMESTAMP_STRUCT wrapper
//
struct PSqlTimestamp : public PSqlValue
{
	SQL_TIMESTAMP_STRUCT value;

	PSqlTimestamp() : PSqlValue() {} //-V730

	void init(const SrvTime& value_);
	void init(const CommUtcTime& value_); // for easier initialization if we have CommUtcTime instead of SrvTime
	void operator=(const SrvTime& value_);
	void parse(CommMsgParser& parser);
	void compose(CommMsgBody& body) const override;
	void getValue(SrvTime& srvTime) const;
	void getValue(CommUtcTime& srvTime) const;

	void init_1990IfNull(const SrvTime& value_);
	void getValue_NullIf1990(SrvTime& srvTime) const;
};

//
// Nullable SQL_TIMESTAMP_STRUCT wrapper
//
struct PSqlTimestampNullable : public PSqlTimestamp
{
	PSqlTimestampNullable() : PSqlTimestamp() {}

	void init(const SrvTime& value_);
	void operator=(const SrvTime& value_);
	void parse(CommMsgParser& parser);
	void compose(CommMsgBody& body) const override;
	PSqlValue::setNull;
};

//
// SQL_DATE_STRUCT wrapper
//
struct PSqlDate : public PSqlValue
{
	SQL_DATE_STRUCT value;

	PSqlDate() : PSqlValue() {} //-V730

	void init(const SrvDate& value_);
	void operator=(const SrvDate& value_);
	void parse(CommMsgParser& parser);
	void compose(CommMsgBody& body) const override;
	void getValue(SrvDate& srvDate) const;
	void init_1990IfNull(const SrvDate& value_);
	void getValue_NullIf1990(SrvDate& srvDate) const; // do NOT use for DOB or similar real past dates
};

//
// Nullable SQL_DATE_STRUCT wrapper
//
struct PSqlDateNullable : public PSqlDate
{
	PSqlDateNullable() : PSqlDate() {}

	void init(const SrvDate& value_);
	void operator=(const SrvDate& value_);
	void parse(CommMsgParser& parser);
	void compose(CommMsgBody& body) const override;
	PSqlValue::setNull;
};

//
// (Nullable) PSqlXml 
//
struct PSqlXml : public PSqlValue
{
private:
	PString xmlInUtf8;
public:
	PSqlXml() : PSqlValue()	{}
	void init(const PString& value_)
	{
		xmlInUtf8.assign(value_);
		cb = xmlInUtf8.length();
	}
	void clearAndSetNull() 
	{
		setNull();
		xmlInUtf8.assign("");
	}
	void parse(CommMsgParser& parser);
	void compose(CommMsgBody& body) const override;
	const char* c_str() const
	{
		return xmlInUtf8;
	}
};

// PYR-46890
// bigint tournament id compatible with 32-bit tournid
struct PSqlTournamentId : public PSqlValue
{
	friend class DbmStatement;
protected:
	SQLBIGINT value;

public:
	PSqlTournamentId() : PSqlValue(), value(0) {}

	void init( UINT32 value_ );
	void operator=( UINT32 value_ );
	void parse( CommMsgParser& parser );
	void compose( CommMsgBody& body ) const override;
	UINT32 getValue() const { PASSERT( value >= 0 && value <= PUINT32_MAX ); return (UINT32)value; }
};

struct PSqlTournIdNullable : public PSqlTournamentId
{
	PSqlTournIdNullable() : PSqlTournamentId() {}

	void init( UINT32 value_ );
	void operator=( UINT32 value_ );

	void parse( CommMsgParser& parser );
	void compose( CommMsgBody& body ) const override;
	PSqlValue::setNull;
};

// PYR-69531
// internal DB value in range [-1...0xFFFFFFFF-1] exposed for outside operations (parse, compose, getValue) as UINT32 in range [0...0xFFFFFFFF]
struct PSqlBigIntAsUInt32 : public PSqlValue
{
	friend class DbmStatement;
protected:
	SQLBIGINT value;

public:
	PSqlBigIntAsUInt32() : PSqlValue(), value(0) {}

	void init(UINT32 value_);
	void operator=(UINT32 value_);
	void parse(CommMsgParser& parser);
	void compose(CommMsgBody& body) const override;
	UINT32 getUint32() const { PASSERT(value >= -1 && value < PUINT32_MAX); return (UINT32)value; }
	INT64 getInt64() const { return value; }
};
//lint -restore

#endif // psqlvalue_h_included
